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
Cyc_stderr,fmt,ap);{const char*_tmpA22;void*_tmpA21;(_tmpA21=0,Cyc_fprintf(Cyc_stderr,((
_tmpA22="\n",_tag_dyneither(_tmpA22,sizeof(char),2))),_tag_dyneither(_tmpA21,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA25;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA24;(int)_throw((void*)((
_tmpA24=_cycalloc_atomic(sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA25.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA25)),_tmpA24)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA28;void*
_tmpA27;(_tmpA27=0,Cyc_fprintf(Cyc_stderr,((_tmpA28="\n",_tag_dyneither(_tmpA28,
sizeof(char),2))),_tag_dyneither(_tmpA27,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA2B;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA2A;(int)_throw((void*)((_tmpA2A=_cycalloc_atomic(sizeof(*_tmpA2A)),((
_tmpA2A[0]=((_tmpA2B.tag=Cyc_Toc_Toc_Impossible,_tmpA2B)),_tmpA2A)))));};}char
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
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA2C;skip_stmt_opt=((_tmpA2C=
_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=Cyc_Absyn_skip_stmt(0),_tmpA2C))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA2F;struct Cyc_Absyn_Cast_e_struct*_tmpA2E;
return(void*)((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=15,((
_tmpA2F.f1=(void*)t,((_tmpA2F.f2=e,((_tmpA2F.f3=0,((_tmpA2F.f4=Cyc_Absyn_No_coercion,
_tmpA2F)))))))))),_tmpA2E))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA32;struct Cyc_Absyn_Deref_e_struct*_tmpA31;return(void*)((_tmpA31=_cycalloc(
sizeof(*_tmpA31)),((_tmpA31[0]=((_tmpA32.tag=21,((_tmpA32.f1=e,_tmpA32)))),
_tmpA31))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA35;struct Cyc_Absyn_Subscript_e_struct*
_tmpA34;return(void*)((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34[0]=((
_tmpA35.tag=24,((_tmpA35.f1=e1,((_tmpA35.f2=e2,_tmpA35)))))),_tmpA34))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA38;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA37;return(void*)((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=((
_tmpA38.tag=37,((_tmpA38.f1=s,_tmpA38)))),_tmpA37))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA3B;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA3A;return(void*)((_tmpA3A=
_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=18,((_tmpA3B.f1=(void*)t,
_tmpA3B)))),_tmpA3A))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA3E;struct Cyc_Absyn_FnCall_e_struct*
_tmpA3D;return(void*)((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D[0]=((
_tmpA3E.tag=11,((_tmpA3E.f1=e,((_tmpA3E.f2=es,((_tmpA3E.f3=0,_tmpA3E)))))))),
_tmpA3D))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA41;struct Cyc_Absyn_Exp_s_struct*
_tmpA40;return(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((
_tmpA41.tag=1,((_tmpA41.f1=e,_tmpA41)))),_tmpA40))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA44;struct Cyc_Absyn_Seq_s_struct*_tmpA43;return(void*)((_tmpA43=_cycalloc(
sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=2,((_tmpA44.f1=s1,((_tmpA44.f2=s2,
_tmpA44)))))),_tmpA43))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA47;struct Cyc_Absyn_Conditional_e_struct*_tmpA46;return(void*)((_tmpA46=
_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=6,((_tmpA47.f1=e1,((
_tmpA47.f2=e2,((_tmpA47.f3=e3,_tmpA47)))))))),_tmpA46))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA4A;struct Cyc_Absyn_AggrMember_e_struct*_tmpA49;return(void*)((_tmpA49=
_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=22,((_tmpA4A.f1=e,((
_tmpA4A.f2=n,((_tmpA4A.f3=0,((_tmpA4A.f4=0,_tmpA4A)))))))))),_tmpA49))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA4D;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA4C;return(void*)((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((
_tmpA4D.tag=23,((_tmpA4D.f1=e,((_tmpA4D.f2=n,((_tmpA4D.f3=0,((_tmpA4D.f4=0,
_tmpA4D)))))))))),_tmpA4C))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA50;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA4F;return(void*)((_tmpA4F=_cycalloc(
sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=36,((_tmpA50.f1=tdopt,((_tmpA50.f2=
ds,_tmpA50)))))),_tmpA4F))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA53;
struct Cyc_Absyn_Goto_s_struct*_tmpA52;return(void*)((_tmpA52=_cycalloc(sizeof(*
_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=8,((_tmpA53.f1=v,((_tmpA53.f2=s,_tmpA53)))))),
_tmpA52))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA59;const char*_tmpA58;struct Cyc_Core_Impossible_struct*_tmpA57;(int)_throw((
void*)((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA59.tag=Cyc_Core_Impossible,((
_tmpA59.f1=((_tmpA58="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA58,sizeof(char),44))),_tmpA59)))),_tmpA57)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA66;const char*_tmpA65;void*_tmpA64[1];
struct Cyc_String_pa_struct _tmpA63;struct Cyc_Core_Impossible_struct*_tmpA62;(int)
_throw((void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA66.tag=
Cyc_Core_Impossible,((_tmpA66.f1=(struct _dyneither_ptr)((_tmpA63.tag=0,((_tmpA63.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA64[
0]=& _tmpA63,Cyc_aprintf(((_tmpA65="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA65,sizeof(char),67))),_tag_dyneither(_tmpA64,sizeof(void*),1)))))))),
_tmpA66)))),_tmpA62)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
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
_tmpA6C;const char*_tmpA6B;struct Cyc_Core_Impossible_struct*_tmpA6A;(int)_throw((
void*)((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6C.tag=Cyc_Core_Impossible,((
_tmpA6C.f1=((_tmpA6B="impossible type (not pointer)",_tag_dyneither(_tmpA6B,
sizeof(char),30))),_tmpA6C)))),_tmpA6A)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
_LL34:;_LL35: {const char*_tmpA6F;void*_tmpA6E;(_tmpA6E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6F="is_nullable",
_tag_dyneither(_tmpA6F,sizeof(char),12))),_tag_dyneither(_tmpA6E,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA70;pair=((_tmpA70.f1=x,((_tmpA70.f2=tag,_tmpA70))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA71;struct _tuple13*_tmp108=(_tmpA71=_cycalloc(sizeof(*_tmpA71)),((
_tmpA71->f1=x,((_tmpA71->f2=tag,_tmpA71)))));struct _dyneither_ptr*_tmpA74;struct
_tuple0*_tmpA73;struct _tuple0*res=(_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->f1=(*
x).f1,((_tmpA73->f2=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA74)))),_tmpA73)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA7C;void*_tmpA7B[
1];const char*_tmpA7A;struct _dyneither_ptr*_tmpA79;struct _dyneither_ptr*xname=(
_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79[0]=(struct _dyneither_ptr)((_tmpA7C.tag=
1,((_tmpA7C.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA7B[0]=& _tmpA7C,
Cyc_aprintf(((_tmpA7A="_tuple%d",_tag_dyneither(_tmpA7A,sizeof(char),9))),
_tag_dyneither(_tmpA7B,sizeof(void*),1)))))))),_tmpA79)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA7F;struct Cyc_List_List*
_tmpA7E;_tmp11A=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->hd=((_tmpA7F=
_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->name=Cyc_Absyn_fieldname(i),((_tmpA7F->tq=
Cyc_Toc_mt_tq,((_tmpA7F->type=(void*)ts2->hd,((_tmpA7F->width=0,((_tmpA7F->attributes=
0,_tmpA7F)))))))))))),((_tmpA7E->tl=_tmp11A,_tmpA7E))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA84;struct _tuple0*_tmpA83;struct Cyc_Absyn_Aggrdecl*_tmpA82;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->kind=Cyc_Absyn_StructA,((
_tmpA82->sc=Cyc_Absyn_Public,((_tmpA82->name=((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((
_tmpA83->f1=Cyc_Absyn_Rel_n(0),((_tmpA83->f2=xname,_tmpA83)))))),((_tmpA82->tvs=
0,((_tmpA82->impl=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->exist_vars=0,((
_tmpA84->rgn_po=0,((_tmpA84->fields=_tmp11A,((_tmpA84->tagged=0,_tmpA84)))))))))),((
_tmpA82->attributes=0,_tmpA82)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA8A;struct Cyc_Absyn_Aggr_d_struct _tmpA89;struct Cyc_List_List*_tmpA88;Cyc_Toc_result_decls=((
_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=Cyc_Absyn_new_decl((void*)((
_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA89.tag=6,((_tmpA89.f1=
_tmp11D,_tmpA89)))),_tmpA8A)))),0),((_tmpA88->tl=Cyc_Toc_result_decls,_tmpA88))))));}{
struct _tuple16*_tmpA8D;struct Cyc_List_List*_tmpA8C;*_tmp111=((_tmpA8C=
_region_malloc(d,sizeof(*_tmpA8C)),((_tmpA8C->hd=((_tmpA8D=_region_malloc(d,
sizeof(*_tmpA8D)),((_tmpA8D->f1=x,((_tmpA8D->f2=ts,_tmpA8D)))))),((_tmpA8C->tl=*
_tmp111,_tmpA8C))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpA9A;const char*
_tmpA99;void*_tmpA98[1];struct Cyc_Int_pa_struct _tmpA97;struct _tuple0*_tmpA96;
struct _tuple0*res=(_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->f1=Cyc_Absyn_Loc_n,((
_tmpA96->f2=((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=(struct
_dyneither_ptr)((_tmpA97.tag=1,((_tmpA97.f1=(unsigned int)_tmp12B,((_tmpA98[0]=&
_tmpA97,Cyc_aprintf(((_tmpA99="_tmp%X",_tag_dyneither(_tmpA99,sizeof(char),7))),
_tag_dyneither(_tmpA98,sizeof(void*),1)))))))),_tmpA9A)))),_tmpA96)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAA2;void*_tmpAA1[1];const char*_tmpAA0;struct _dyneither_ptr*_tmpA9F;struct
_dyneither_ptr*res=(_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=(struct
_dyneither_ptr)((_tmpAA2.tag=1,((_tmpAA2.f1=(unsigned int)_tmp135,((_tmpAA1[0]=&
_tmpAA2,Cyc_aprintf(((_tmpAA0="_LL%X",_tag_dyneither(_tmpAA0,sizeof(char),6))),
_tag_dyneither(_tmpAA1,sizeof(void*),1)))))))),_tmpA9F)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAA5;void*_tmpAA4;(_tmpAA4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA5="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAA5,sizeof(char),43))),_tag_dyneither(_tmpAA4,sizeof(void*),0)));}{
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
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAA6;
return(_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->f1=_tmp140,((_tmpAA6->f2=
_tmp141,((_tmpAA6->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAA6)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpAA7;return(_tmpAA7=
_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->f1=_tmp145,((_tmpAA7->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpAA7)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
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
_tmpAA8;return(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->name=f->name,((
_tmpAA8->tq=Cyc_Toc_mt_tq,((_tmpAA8->type=Cyc_Toc_typ_to_c(f->type),((_tmpAA8->width=
f->width,((_tmpAA8->attributes=f->attributes,_tmpAA8)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAA9;cs=((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((
_tmpAA9[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAA9))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAAA;r=((_tmpAAA=
_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAAA))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAAB;r=((_tmpAAB=
_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAAB))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
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
else{_tmp161=_tmp160->f1;}}_LL45: if(Cyc_Tcutil_tvar_kind(_tmp161,Cyc_Absyn_BoxKind)
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp158;if(_tmp162->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp163=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp158;if(_tmp163->tag != 4)goto _LL4A;else{_tmp164=_tmp163->f1;_tmp165=_tmp164.field_info;
if((_tmp165.KnownDatatypefield).tag != 2)goto _LL4A;_tmp166=(struct _tuple1)(
_tmp165.KnownDatatypefield).val;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;}}_LL49: {
const char*_tmpAAC;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpAAC="_struct",_tag_dyneither(_tmpAAC,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAAF;void*_tmpAAE;(_tmpAAE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAF="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAAF,sizeof(char),29))),_tag_dyneither(_tmpAAE,sizeof(void*),0)));}
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
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAB0;_tmp1A1=((
_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->hd=(void*)_tmp17F->hd,((_tmpAB0->tl=
_tmp1A1,_tmpAB0))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A8=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1A9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAB1;struct _tuple7*_tmp1AA=(_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((
_tmpAB1->f1=_tmp17E->name,((_tmpAB1->f2=_tmp17E->tq,((_tmpAB1->f3=_tmp1A9,
_tmpAB1)))))));struct Cyc_List_List*_tmpAB2;_tmp1A8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A8,((
_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->hd=_tmp1AA,((_tmpAB2->tl=0,
_tmpAB2)))))));}{struct Cyc_Absyn_FnType_struct _tmpAB8;struct Cyc_Absyn_FnInfo
_tmpAB7;struct Cyc_Absyn_FnType_struct*_tmpAB6;return(void*)((_tmpAB6=_cycalloc(
sizeof(*_tmpAB6)),((_tmpAB6[0]=((_tmpAB8.tag=10,((_tmpAB8.f1=((_tmpAB7.tvars=0,((
_tmpAB7.effect=0,((_tmpAB7.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpAB7.args=
_tmp1A8,((_tmpAB7.c_varargs=_tmp17D,((_tmpAB7.cyc_varargs=0,((_tmpAB7.rgn_po=0,((
_tmpAB7.attributes=_tmp1A1,_tmpAB7)))))))))))))))),_tmpAB8)))),_tmpAB6))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpABB;struct Cyc_Absyn_AnonAggrType_struct*
_tmpABA;return(void*)((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((
_tmpABB.tag=13,((_tmpABB.f1=_tmp183,((_tmpABB.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpABB)))))),_tmpABA))));}_LL5C: {
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
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpABE;struct Cyc_Absyn_TypedefType_struct*
_tmpABD;return(void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((
_tmpABE.tag=18,((_tmpABE.f1=_tmp18E,((_tmpABE.f2=0,((_tmpABE.f3=_tmp190,((
_tmpABE.f4=0,_tmpABE)))))))))),_tmpABD))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAC4;void**_tmpAC3;struct Cyc_Absyn_TypedefType_struct*_tmpAC2;return(void*)((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC4.tag=18,((_tmpAC4.f1=
_tmp18E,((_tmpAC4.f2=0,((_tmpAC4.f3=_tmp190,((_tmpAC4.f4=((_tmpAC3=_cycalloc(
sizeof(*_tmpAC3)),((_tmpAC3[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAC3)))),
_tmpAC4)))))))))),_tmpAC2))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAC7;void*_tmpAC6;(_tmpAC6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC7="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAC7,sizeof(char),55))),_tag_dyneither(_tmpAC6,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpACA;void*_tmpAC9;(
_tmpAC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpACA="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpACA,sizeof(char),57))),_tag_dyneither(_tmpAC9,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpACD;void*_tmpACC;(
_tmpACC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpACD="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpACD,sizeof(char),49))),_tag_dyneither(_tmpACC,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAD0;void*_tmpACF;(
_tmpACF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD0="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAD0,sizeof(char),51))),_tag_dyneither(_tmpACF,sizeof(void*),0)));}_LL3D:;}
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
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpAD4;void*_tmpAD3[1];struct Cyc_String_pa_struct
_tmpAD2;(_tmpAD2.tag=0,((_tmpAD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAD3[0]=& _tmpAD2,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD4="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAD4,sizeof(char),25))),_tag_dyneither(_tmpAD3,sizeof(void*),1)))))));}
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
if(_tmp1FA->impl == 0){const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD7="is_poly_field: type missing fields",
_tag_dyneither(_tmpAD7,sizeof(char),35))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));}
_tmp1F9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FA->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F4;if(_tmp1F8->tag != 13)goto _LLD0;else{_tmp1F9=_tmp1F8->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FD=Cyc_Absyn_lookup_field(_tmp1F9,f);if(_tmp1FD == 0){
const char*_tmpADB;void*_tmpADA[1];struct Cyc_String_pa_struct _tmpAD9;(_tmpAD9.tag=
0,((_tmpAD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpADA[0]=&
_tmpAD9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADB="is_poly_field: bad field %s",_tag_dyneither(_tmpADB,sizeof(char),28))),
_tag_dyneither(_tmpADA,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FD->type);}_LLD0:;_LLD1: {const char*_tmpADF;void*_tmpADE[1];struct Cyc_String_pa_struct
_tmpADD;(_tmpADD.tag=0,((_tmpADD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpADE[0]=& _tmpADD,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADF="is_poly_field: bad type %s",
_tag_dyneither(_tmpADF,sizeof(char),27))),_tag_dyneither(_tmpADE,sizeof(void*),1)))))));}
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
_tmp20A);_LLDC:;_LLDD: {const char*_tmpAE3;void*_tmpAE2[1];struct Cyc_String_pa_struct
_tmpAE1;(_tmpAE1.tag=0,((_tmpAE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp209->topt))->v)),((
_tmpAE2[0]=& _tmpAE1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE3="is_poly_project: bad type %s",_tag_dyneither(_tmpAE3,sizeof(char),29))),
_tag_dyneither(_tmpAE2,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAE4;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAE4=_cycalloc(
sizeof(*_tmpAE4)),((_tmpAE4->hd=s,((_tmpAE4->tl=0,_tmpAE4)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAE5;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAE5=_cycalloc(sizeof(*
_tmpAE5)),((_tmpAE5->hd=s,((_tmpAE5->tl=0,_tmpAE5)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAE6[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAE6[1]=s,((_tmpAE6[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAE7[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAE7[1]=n,((
_tmpAE7[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE7,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAE8[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAE8[1]=n,((
_tmpAE8[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAE9[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAE9[2]=n,((_tmpAE9[1]=s,((_tmpAE9[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE9,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAEA;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->hd=e,((_tmpAEA->tl=0,_tmpAEA)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAEB;throw_match_stmt_opt=((
_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAEB))));}return*throw_match_stmt_opt;}static
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAF1;struct
Cyc_Absyn_Var_d_struct _tmpAF0;struct Cyc_List_List*_tmpAEF;Cyc_Toc_result_decls=((
_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=Cyc_Absyn_new_decl((void*)((
_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF0.tag=0,((_tmpAF0.f1=vd,
_tmpAF0)))),_tmpAF1)))),0),((_tmpAEF->tl=Cyc_Toc_result_decls,_tmpAEF))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpAF8;
struct _tuple15*_tmpAF7;struct _tuple15*_tmpAF6;struct _tuple15*_tmpAF5[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpAF5[2]=((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((
_tmpAF6->f1=0,((_tmpAF6->f2=xplussz,_tmpAF6)))))),((_tmpAF5[1]=((_tmpAF7=
_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->f1=0,((_tmpAF7->f2=xexp,_tmpAF7)))))),((
_tmpAF5[0]=((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->f1=0,((_tmpAF8->f2=
xexp,_tmpAF8)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF5,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
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
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpAF9;return(_tmpAF9=_region_malloc(r,
sizeof(*_tmpAF9)),((_tmpAF9->break_lab=(struct _dyneither_ptr**)0,((_tmpAF9->continue_lab=(
struct _dyneither_ptr**)0,((_tmpAF9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpAF9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpAF9->toplevel=(int)1,((_tmpAF9->rgn=(struct _RegionHandle*)r,_tmpAF9)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22D;struct _dyneither_ptr**_tmp22E;struct _dyneither_ptr**
_tmp22F;struct Cyc_Toc_FallthruInfo*_tmp230;struct Cyc_Dict_Dict _tmp231;int _tmp232;
struct Cyc_Toc_Env*_tmp22C=e;_tmp22D=*_tmp22C;_tmp22E=_tmp22D.break_lab;_tmp22F=
_tmp22D.continue_lab;_tmp230=_tmp22D.fallthru_info;_tmp231=_tmp22D.varmap;
_tmp232=_tmp22D.toplevel;{struct Cyc_Toc_Env*_tmpAFA;return(_tmpAFA=
_region_malloc(r,sizeof(*_tmpAFA)),((_tmpAFA->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22E),((_tmpAFA->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpAFA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp230,((_tmpAFA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp231),((_tmpAFA->toplevel=(
int)_tmp232,((_tmpAFA->rgn=(struct _RegionHandle*)r,_tmpAFA)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp235;struct _dyneither_ptr**_tmp236;struct
_dyneither_ptr**_tmp237;struct Cyc_Toc_FallthruInfo*_tmp238;struct Cyc_Dict_Dict
_tmp239;int _tmp23A;struct Cyc_Toc_Env*_tmp234=e;_tmp235=*_tmp234;_tmp236=_tmp235.break_lab;
_tmp237=_tmp235.continue_lab;_tmp238=_tmp235.fallthru_info;_tmp239=_tmp235.varmap;
_tmp23A=_tmp235.toplevel;{struct Cyc_Toc_Env*_tmpAFB;return(_tmpAFB=
_region_malloc(r,sizeof(*_tmpAFB)),((_tmpAFB->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp236),((_tmpAFB->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpAFB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp238,((_tmpAFB->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp239),((_tmpAFB->toplevel=(
int)0,((_tmpAFB->rgn=(struct _RegionHandle*)r,_tmpAFB)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23D;struct _dyneither_ptr**_tmp23E;struct _dyneither_ptr**
_tmp23F;struct Cyc_Toc_FallthruInfo*_tmp240;struct Cyc_Dict_Dict _tmp241;int _tmp242;
struct Cyc_Toc_Env*_tmp23C=e;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.break_lab;_tmp23F=
_tmp23D.continue_lab;_tmp240=_tmp23D.fallthru_info;_tmp241=_tmp23D.varmap;
_tmp242=_tmp23D.toplevel;{struct Cyc_Toc_Env*_tmpAFC;return(_tmpAFC=
_region_malloc(r,sizeof(*_tmpAFC)),((_tmpAFC->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23E),((_tmpAFC->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpAFC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp240,((_tmpAFC->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp241),((_tmpAFC->toplevel=(
int)1,((_tmpAFC->rgn=(struct _RegionHandle*)r,_tmpAFC)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp244=(*x).f1;struct Cyc_List_List*_tmp245;_LLE4: if((
_tmp244.Rel_n).tag != 1)goto _LLE6;_tmp245=(struct Cyc_List_List*)(_tmp244.Rel_n).val;
_LLE5: {const char*_tmpB00;void*_tmpAFF[1];struct Cyc_String_pa_struct _tmpAFE;(
_tmpAFE.tag=0,((_tmpAFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpAFF[0]=& _tmpAFE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB00="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB00,sizeof(char),30))),_tag_dyneither(_tmpAFF,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp24A;struct _dyneither_ptr**_tmp24B;
struct _dyneither_ptr**_tmp24C;struct Cyc_Toc_FallthruInfo*_tmp24D;struct Cyc_Dict_Dict
_tmp24E;int _tmp24F;struct Cyc_Toc_Env*_tmp249=e;_tmp24A=*_tmp249;_tmp24B=_tmp24A.break_lab;
_tmp24C=_tmp24A.continue_lab;_tmp24D=_tmp24A.fallthru_info;_tmp24E=_tmp24A.varmap;
_tmp24F=_tmp24A.toplevel;{struct Cyc_Dict_Dict _tmp250=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24E),x,y);struct Cyc_Toc_Env*_tmpB01;return(_tmpB01=_region_malloc(r,
sizeof(*_tmpB01)),((_tmpB01->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24B),((_tmpB01->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24C),((_tmpB01->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24D,((_tmpB01->varmap=(struct Cyc_Dict_Dict)_tmp250,((_tmpB01->toplevel=(int)
_tmp24F,((_tmpB01->rgn=(struct _RegionHandle*)r,_tmpB01)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp253;struct _dyneither_ptr**_tmp254;struct _dyneither_ptr**
_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;
struct Cyc_Toc_Env*_tmp252=e;_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=
_tmp253.continue_lab;_tmp256=_tmp253.fallthru_info;_tmp257=_tmp253.varmap;
_tmp258=_tmp253.toplevel;{struct Cyc_Toc_Env*_tmpB02;return(_tmpB02=
_region_malloc(r,sizeof(*_tmpB02)),((_tmpB02->break_lab=(struct _dyneither_ptr**)
0,((_tmpB02->continue_lab=(struct _dyneither_ptr**)0,((_tmpB02->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpB02->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp257),((_tmpB02->toplevel=(int)_tmp258,((_tmpB02->rgn=(struct _RegionHandle*)
r,_tmpB02)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB03;fallthru_vars=((
_tmpB03=_region_malloc(r,sizeof(*_tmpB03)),((_tmpB03->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB03->tl=fallthru_vars,_tmpB03))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25C;struct _dyneither_ptr**_tmp25D;struct
_dyneither_ptr**_tmp25E;struct Cyc_Toc_FallthruInfo*_tmp25F;struct Cyc_Dict_Dict
_tmp260;int _tmp261;struct Cyc_Toc_Env*_tmp25B=e;_tmp25C=*_tmp25B;_tmp25D=_tmp25C.break_lab;
_tmp25E=_tmp25C.continue_lab;_tmp25F=_tmp25C.fallthru_info;_tmp260=_tmp25C.varmap;
_tmp261=_tmp25C.toplevel;{struct Cyc_Toc_Env _tmp263;struct Cyc_Dict_Dict _tmp264;
struct Cyc_Toc_Env*_tmp262=next_case_env;_tmp263=*_tmp262;_tmp264=_tmp263.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB04;struct Cyc_Toc_FallthruInfo*fi=(_tmpB04=
_region_malloc(r,sizeof(*_tmpB04)),((_tmpB04->label=fallthru_l,((_tmpB04->binders=
fallthru_vars,((_tmpB04->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp264),_tmpB04)))))));
struct _dyneither_ptr**_tmpB07;struct Cyc_Toc_Env*_tmpB06;return(_tmpB06=
_region_malloc(r,sizeof(*_tmpB06)),((_tmpB06->break_lab=(struct _dyneither_ptr**)((
_tmpB07=_region_malloc(r,sizeof(*_tmpB07)),((_tmpB07[0]=break_l,_tmpB07)))),((
_tmpB06->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25E),((
_tmpB06->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB06->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp260),((_tmpB06->toplevel=(int)_tmp261,((_tmpB06->rgn=(
struct _RegionHandle*)r,_tmpB06)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp269;struct _dyneither_ptr**
_tmp26A;struct _dyneither_ptr**_tmp26B;struct Cyc_Toc_FallthruInfo*_tmp26C;struct
Cyc_Dict_Dict _tmp26D;int _tmp26E;struct Cyc_Toc_Env*_tmp268=e;_tmp269=*_tmp268;
_tmp26A=_tmp269.break_lab;_tmp26B=_tmp269.continue_lab;_tmp26C=_tmp269.fallthru_info;
_tmp26D=_tmp269.varmap;_tmp26E=_tmp269.toplevel;{struct _dyneither_ptr**_tmpB0A;
struct Cyc_Toc_Env*_tmpB09;return(_tmpB09=_region_malloc(r,sizeof(*_tmpB09)),((
_tmpB09->break_lab=(struct _dyneither_ptr**)((_tmpB0A=_region_malloc(r,sizeof(*
_tmpB0A)),((_tmpB0A[0]=break_l,_tmpB0A)))),((_tmpB09->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpB09->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB09->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26D),((_tmpB09->toplevel=(
int)_tmp26E,((_tmpB09->rgn=(struct _RegionHandle*)r,_tmpB09)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp272;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*
_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;struct Cyc_Toc_Env*_tmp271=e;
_tmp272=*_tmp271;_tmp273=_tmp272.break_lab;_tmp274=_tmp272.continue_lab;_tmp275=
_tmp272.fallthru_info;_tmp276=_tmp272.varmap;_tmp277=_tmp272.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB0D;struct Cyc_Toc_Env*_tmpB0C;return(_tmpB0C=_region_malloc(r,sizeof(*
_tmpB0C)),((_tmpB0C->break_lab=(struct _dyneither_ptr**)0,((_tmpB0C->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp274),((_tmpB0C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB0D=_region_malloc(r,sizeof(*_tmpB0D)),((
_tmpB0D->label=next_l,((_tmpB0D->binders=0,((_tmpB0D->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB0D)))))))),((_tmpB0C->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp276),((_tmpB0C->toplevel=(int)_tmp277,((_tmpB0C->rgn=(struct _RegionHandle*)
r,_tmpB0C)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27A=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27B=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27A;if(_tmp27B->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27C=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27A;if(_tmp27C->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27A;if(_tmp27D->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB10;void*_tmpB0F;(_tmpB0F=0,Cyc_Tcutil_terr(e->loc,((
_tmpB10="dereference of NULL pointer",_tag_dyneither(_tmpB10,sizeof(char),28))),
_tag_dyneither(_tmpB0F,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27E=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27A;if(_tmp27E->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27F=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp27A;if(_tmp27F->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB13;void*_tmpB12;(_tmpB12=0,Cyc_Tcutil_warn(e->loc,((
_tmpB13="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB13,
sizeof(char),46))),_tag_dyneither(_tmpB12,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB16;void*_tmpB15;(_tmpB15=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB16="need_null_check",_tag_dyneither(
_tmpB16,sizeof(char),16))),_tag_dyneither(_tmpB15,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp286=e->annot;
struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp28A;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp287=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp286;if(_tmp287->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp288=_tmp287->f1;}}_LLF7: return _tmp288;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp286;if(_tmp289->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp28A=_tmp289->f1;}}_LLF9: return _tmp28A;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp28B=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp286;if(_tmp28B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB19;void*_tmpB18;(_tmpB18=0,Cyc_Tcutil_terr(e->loc,((
_tmpB19="dereference of NULL pointer",_tag_dyneither(_tmpB19,sizeof(char),28))),
_tag_dyneither(_tmpB18,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28C=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp286;if(_tmp28C->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28D=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp286;if(_tmp28D->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB1C;void*_tmpB1B;(
_tmpB1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1C="get_relns",_tag_dyneither(_tmpB1C,sizeof(char),10))),_tag_dyneither(
_tmpB1B,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
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
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp2A9=e->r;void*_tmp2AB;
struct Cyc_Absyn_Vardecl*_tmp2AD;void*_tmp2AF;struct Cyc_Absyn_Vardecl*_tmp2B1;
void*_tmp2B3;struct Cyc_Absyn_Vardecl*_tmp2B5;void*_tmp2B7;struct Cyc_Absyn_Vardecl*
_tmp2B9;enum Cyc_Absyn_Primop _tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List
_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;_LL116: {struct Cyc_Absyn_Var_e_struct*
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
goto _LL115;_LL11E: {struct Cyc_Absyn_Primop_e_struct*_tmp2BA=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A9;if(_tmp2BA->tag != 3)goto _LL120;else{_tmp2BB=_tmp2BA->f1;if(_tmp2BB != Cyc_Absyn_Numelts)
goto _LL120;_tmp2BC=_tmp2BA->f2;if(_tmp2BC == 0)goto _LL120;_tmp2BD=*_tmp2BC;
_tmp2BE=(struct Cyc_Absyn_Exp*)_tmp2BD.hd;}}_LL11F:{void*_tmp2BF=_tmp2BE->r;void*
_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C3;void*_tmp2C5;struct Cyc_Absyn_Vardecl*
_tmp2C7;void*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CB;void*_tmp2CD;struct Cyc_Absyn_Vardecl*
_tmp2CF;_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2C0=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BF;if(_tmp2C0->tag != 1)goto _LL125;else{_tmp2C1=(void*)_tmp2C0->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2C2=(struct Cyc_Absyn_Pat_b_struct*)_tmp2C1;if(_tmp2C2->tag
!= 5)goto _LL125;else{_tmp2C3=_tmp2C2->f1;}};}}_LL124: _tmp2C7=_tmp2C3;goto _LL126;
_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C4=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BF;if(_tmp2C4->tag != 1)goto _LL127;else{_tmp2C5=(void*)_tmp2C4->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C6=(struct Cyc_Absyn_Local_b_struct*)_tmp2C5;if(
_tmp2C6->tag != 4)goto _LL127;else{_tmp2C7=_tmp2C6->f1;}};}}_LL126: _tmp2CB=_tmp2C7;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BF;if(_tmp2C8->tag != 1)goto _LL129;else{_tmp2C9=(void*)_tmp2C8->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2CA=(struct Cyc_Absyn_Global_b_struct*)_tmp2C9;if(
_tmp2CA->tag != 1)goto _LL129;else{_tmp2CB=_tmp2CA->f1;}};}}_LL128: _tmp2CF=_tmp2CB;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2CC=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BF;if(_tmp2CC->tag != 1)goto _LL12B;else{_tmp2CD=(void*)_tmp2CC->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CE=(struct Cyc_Absyn_Param_b_struct*)_tmp2CD;if(
_tmp2CE->tag != 3)goto _LL12B;else{_tmp2CF=_tmp2CE->f1;}};}}_LL12A: return _tmp2CF == 
v;_LL12B:;_LL12C: goto _LL122;_LL122:;}goto _LL115;_LL120:;_LL121: goto _LL115;_LL115:;}
return 0;}static char _tmp2E1[8]="*bogus*";struct _tuple17{void*f1;void*f2;};static
int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{
void*_tmp2D0=a->r;void*_tmp2D2;struct Cyc_Absyn_Vardecl*_tmp2D4;void*_tmp2D6;
struct Cyc_Absyn_Vardecl*_tmp2D8;void*_tmp2DA;struct Cyc_Absyn_Vardecl*_tmp2DC;
void*_tmp2DE;struct Cyc_Absyn_Vardecl*_tmp2E0;_LL12E: {struct Cyc_Absyn_Var_e_struct*
_tmp2D1=(struct Cyc_Absyn_Var_e_struct*)_tmp2D0;if(_tmp2D1->tag != 1)goto _LL130;
else{_tmp2D2=(void*)_tmp2D1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2D3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2D2;if(_tmp2D3->tag != 5)goto _LL130;else{_tmp2D4=_tmp2D3->f1;}};}}_LL12F:
_tmp2D8=_tmp2D4;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D5=(struct
Cyc_Absyn_Var_e_struct*)_tmp2D0;if(_tmp2D5->tag != 1)goto _LL132;else{_tmp2D6=(
void*)_tmp2D5->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D7=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D6;if(_tmp2D7->tag != 4)goto _LL132;else{_tmp2D8=_tmp2D7->f1;}};}}_LL131:
_tmp2DC=_tmp2D8;goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2D9=(struct
Cyc_Absyn_Var_e_struct*)_tmp2D0;if(_tmp2D9->tag != 1)goto _LL134;else{_tmp2DA=(
void*)_tmp2D9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2DB=(struct Cyc_Absyn_Global_b_struct*)
_tmp2DA;if(_tmp2DB->tag != 1)goto _LL134;else{_tmp2DC=_tmp2DB->f1;}};}}_LL133:
_tmp2E0=_tmp2DC;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2DD=(struct
Cyc_Absyn_Var_e_struct*)_tmp2D0;if(_tmp2DD->tag != 1)goto _LL136;else{_tmp2DE=(
void*)_tmp2DD->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DF=(struct Cyc_Absyn_Param_b_struct*)
_tmp2DE;if(_tmp2DF->tag != 3)goto _LL136;else{_tmp2E0=_tmp2DF->f1;}};}}_LL135: if(
_tmp2E0->escapes)goto _LL137;x=_tmp2E0;goto _LL12D;_LL136:;_LL137: {static struct
_dyneither_ptr bogus_string={_tmp2E1,_tmp2E1,_tmp2E1 + 8};static struct _tuple0
bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL12D:;}inner_loop: {void*_tmp2E2=i->r;
void*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;union Cyc_Absyn_Cnst _tmp2E7;struct
_tuple5 _tmp2E8;enum Cyc_Absyn_Sign _tmp2E9;int _tmp2EA;union Cyc_Absyn_Cnst _tmp2EC;
struct _tuple5 _tmp2ED;enum Cyc_Absyn_Sign _tmp2EE;int _tmp2EF;union Cyc_Absyn_Cnst
_tmp2F1;struct _tuple5 _tmp2F2;enum Cyc_Absyn_Sign _tmp2F3;int _tmp2F4;enum Cyc_Absyn_Primop
_tmp2F6;struct Cyc_List_List*_tmp2F7;struct Cyc_List_List _tmp2F8;struct Cyc_Absyn_Exp*
_tmp2F9;struct Cyc_List_List*_tmp2FA;struct Cyc_List_List _tmp2FB;struct Cyc_Absyn_Exp*
_tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp300;void*_tmp302;struct Cyc_Absyn_Vardecl*
_tmp304;void*_tmp306;struct Cyc_Absyn_Vardecl*_tmp308;void*_tmp30A;struct Cyc_Absyn_Vardecl*
_tmp30C;_LL139: {struct Cyc_Absyn_Cast_e_struct*_tmp2E3=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2E2;if(_tmp2E3->tag != 15)goto _LL13B;else{_tmp2E4=(void*)_tmp2E3->f1;_tmp2E5=
_tmp2E3->f2;}}_LL13A: i=_tmp2E5;goto inner_loop;_LL13B: {struct Cyc_Absyn_Const_e_struct*
_tmp2E6=(struct Cyc_Absyn_Const_e_struct*)_tmp2E2;if(_tmp2E6->tag != 0)goto _LL13D;
else{_tmp2E7=_tmp2E6->f1;if((_tmp2E7.Int_c).tag != 4)goto _LL13D;_tmp2E8=(struct
_tuple5)(_tmp2E7.Int_c).val;_tmp2E9=_tmp2E8.f1;if(_tmp2E9 != Cyc_Absyn_None)goto
_LL13D;_tmp2EA=_tmp2E8.f2;}}_LL13C: _tmp2EF=_tmp2EA;goto _LL13E;_LL13D: {struct Cyc_Absyn_Const_e_struct*
_tmp2EB=(struct Cyc_Absyn_Const_e_struct*)_tmp2E2;if(_tmp2EB->tag != 0)goto _LL13F;
else{_tmp2EC=_tmp2EB->f1;if((_tmp2EC.Int_c).tag != 4)goto _LL13F;_tmp2ED=(struct
_tuple5)(_tmp2EC.Int_c).val;_tmp2EE=_tmp2ED.f1;if(_tmp2EE != Cyc_Absyn_Signed)
goto _LL13F;_tmp2EF=_tmp2ED.f2;}}_LL13E: return _tmp2EF >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2EF + 1),x->type);_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2F0=(struct Cyc_Absyn_Const_e_struct*)_tmp2E2;if(_tmp2F0->tag != 0)goto _LL141;
else{_tmp2F1=_tmp2F0->f1;if((_tmp2F1.Int_c).tag != 4)goto _LL141;_tmp2F2=(struct
_tuple5)(_tmp2F1.Int_c).val;_tmp2F3=_tmp2F2.f1;if(_tmp2F3 != Cyc_Absyn_Unsigned)
goto _LL141;_tmp2F4=_tmp2F2.f2;}}_LL140: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2F4 + 1),x->type);_LL141: {struct Cyc_Absyn_Primop_e_struct*
_tmp2F5=(struct Cyc_Absyn_Primop_e_struct*)_tmp2E2;if(_tmp2F5->tag != 3)goto _LL143;
else{_tmp2F6=_tmp2F5->f1;if(_tmp2F6 != Cyc_Absyn_Mod)goto _LL143;_tmp2F7=_tmp2F5->f2;
if(_tmp2F7 == 0)goto _LL143;_tmp2F8=*_tmp2F7;_tmp2F9=(struct Cyc_Absyn_Exp*)_tmp2F8.hd;
_tmp2FA=_tmp2F8.tl;if(_tmp2FA == 0)goto _LL143;_tmp2FB=*_tmp2FA;_tmp2FC=(struct Cyc_Absyn_Exp*)
_tmp2FB.hd;}}_LL142: return Cyc_Toc_check_leq_size(relns,x,_tmp2FC);_LL143: {
struct Cyc_Absyn_Var_e_struct*_tmp2FD=(struct Cyc_Absyn_Var_e_struct*)_tmp2E2;if(
_tmp2FD->tag != 1)goto _LL145;else{_tmp2FE=(void*)_tmp2FD->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2FF=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FE;if(_tmp2FF->tag != 5)goto _LL145;
else{_tmp300=_tmp2FF->f1;}};}}_LL144: _tmp304=_tmp300;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp301=(struct Cyc_Absyn_Var_e_struct*)_tmp2E2;if(_tmp301->tag
!= 1)goto _LL147;else{_tmp302=(void*)_tmp301->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp303=(struct Cyc_Absyn_Local_b_struct*)_tmp302;if(_tmp303->tag != 4)goto _LL147;
else{_tmp304=_tmp303->f1;}};}}_LL146: _tmp308=_tmp304;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp305=(struct Cyc_Absyn_Var_e_struct*)_tmp2E2;if(_tmp305->tag
!= 1)goto _LL149;else{_tmp306=(void*)_tmp305->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp307=(struct Cyc_Absyn_Global_b_struct*)_tmp306;if(_tmp307->tag != 1)goto _LL149;
else{_tmp308=_tmp307->f1;}};}}_LL148: _tmp30C=_tmp308;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp309=(struct Cyc_Absyn_Var_e_struct*)_tmp2E2;if(_tmp309->tag
!= 1)goto _LL14B;else{_tmp30A=(void*)_tmp309->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp30B=(struct Cyc_Absyn_Param_b_struct*)_tmp30A;if(_tmp30B->tag != 3)goto _LL14B;
else{_tmp30C=_tmp30B->f1;}};}}_LL14A: if(_tmp30C->escapes)return 0;{struct Cyc_List_List*
_tmp30D=relns;for(0;_tmp30D != 0;_tmp30D=_tmp30D->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30E=(struct Cyc_CfFlowInfo_Reln*)_tmp30D->hd;if(_tmp30E->vd == _tmp30C){union
Cyc_CfFlowInfo_RelnOp _tmp30F=_tmp30E->rop;struct Cyc_Absyn_Vardecl*_tmp310;struct
_tuple11 _tmp311;struct Cyc_Absyn_Vardecl*_tmp312;void*_tmp313;unsigned int _tmp314;
_LL14E: if((_tmp30F.LessNumelts).tag != 3)goto _LL150;_tmp310=(struct Cyc_Absyn_Vardecl*)(
_tmp30F.LessNumelts).val;_LL14F: if(x == _tmp310)return 1;else{goto _LL14D;}_LL150:
if((_tmp30F.LessVar).tag != 2)goto _LL152;_tmp311=(struct _tuple11)(_tmp30F.LessVar).val;
_tmp312=_tmp311.f1;_tmp313=_tmp311.f2;_LL151:{struct _tuple17 _tmpB1D;struct
_tuple17 _tmp316=(_tmpB1D.f1=Cyc_Tcutil_compress(_tmp313),((_tmpB1D.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v),_tmpB1D)));void*_tmp317;
void*_tmp319;void*_tmp31A;struct Cyc_Absyn_PtrInfo _tmp31C;struct Cyc_Absyn_PtrAtts
_tmp31D;union Cyc_Absyn_Constraint*_tmp31E;_LL157: _tmp317=_tmp316.f1;{struct Cyc_Absyn_TagType_struct*
_tmp318=(struct Cyc_Absyn_TagType_struct*)_tmp317;if(_tmp318->tag != 20)goto _LL159;
else{_tmp319=(void*)_tmp318->f1;}};_tmp31A=_tmp316.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp31B=(struct Cyc_Absyn_PointerType_struct*)_tmp31A;if(_tmp31B->tag != 5)goto
_LL159;else{_tmp31C=_tmp31B->f1;_tmp31D=_tmp31C.ptr_atts;_tmp31E=_tmp31D.bounds;}};
_LL158:{void*_tmp31F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp31E);struct Cyc_Absyn_Exp*_tmp321;_LL15C: {struct Cyc_Absyn_Upper_b_struct*
_tmp320=(struct Cyc_Absyn_Upper_b_struct*)_tmp31F;if(_tmp320->tag != 1)goto _LL15E;
else{_tmp321=_tmp320->f1;}}_LL15D: {struct Cyc_Absyn_Exp*_tmp322=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp319,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp322,_tmp321))return 1;goto _LL15B;}_LL15E:;_LL15F:
goto _LL15B;_LL15B:;}goto _LL156;_LL159:;_LL15A: goto _LL156;_LL156:;}{struct Cyc_List_List*
_tmp323=relns;for(0;_tmp323 != 0;_tmp323=_tmp323->tl){struct Cyc_CfFlowInfo_Reln*
_tmp324=(struct Cyc_CfFlowInfo_Reln*)_tmp323->hd;if(_tmp324->vd == _tmp312){union
Cyc_CfFlowInfo_RelnOp _tmp325=_tmp324->rop;struct Cyc_Absyn_Vardecl*_tmp326;struct
Cyc_Absyn_Vardecl*_tmp327;unsigned int _tmp328;struct _tuple11 _tmp329;struct Cyc_Absyn_Vardecl*
_tmp32A;_LL161: if((_tmp325.LessEqNumelts).tag != 5)goto _LL163;_tmp326=(struct Cyc_Absyn_Vardecl*)(
_tmp325.LessEqNumelts).val;_LL162: _tmp327=_tmp326;goto _LL164;_LL163: if((_tmp325.LessNumelts).tag
!= 3)goto _LL165;_tmp327=(struct Cyc_Absyn_Vardecl*)(_tmp325.LessNumelts).val;
_LL164: if(x == _tmp327)return 1;goto _LL160;_LL165: if((_tmp325.EqualConst).tag != 1)
goto _LL167;_tmp328=(unsigned int)(_tmp325.EqualConst).val;_LL166: return Cyc_Toc_check_const_array(
_tmp328,x->type);_LL167: if((_tmp325.LessVar).tag != 2)goto _LL169;_tmp329=(struct
_tuple11)(_tmp325.LessVar).val;_tmp32A=_tmp329.f1;_LL168: if(Cyc_Toc_check_leq_size_var(
relns,x,_tmp32A))return 1;goto _LL160;_LL169:;_LL16A: goto _LL160;_LL160:;}}}goto
_LL14D;_LL152: if((_tmp30F.LessConst).tag != 4)goto _LL154;_tmp314=(unsigned int)(
_tmp30F.LessConst).val;_LL153: return Cyc_Toc_check_const_array(_tmp314,x->type);
_LL154:;_LL155: goto _LL14D;_LL14D:;}}}goto _LL138;_LL14B:;_LL14C: goto _LL138;_LL138:;}
return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB20;void*_tmpB1F;(_tmpB1F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB20="Missing type in primop ",_tag_dyneither(_tmpB20,sizeof(char),24))),
_tag_dyneither(_tmpB1F,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmpB23;void*_tmpB22;(_tmpB22=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB23="Missing type in primop ",
_tag_dyneither(_tmpB23,sizeof(char),24))),_tag_dyneither(_tmpB22,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e);static struct _tuple9*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB24;return(_tmpB24=_cycalloc(sizeof(*
_tmpB24)),((_tmpB24->f1=Cyc_Toc_mt_tq,((_tmpB24->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB24)))));}static struct _tuple15*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB25;return(_tmpB25=_cycalloc(sizeof(*_tmpB25)),((
_tmpB25->f1=0,((_tmpB25->f2=e,_tmpB25)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp331=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp331): Cyc_Toc_malloc_ptr(_tmp331));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp331);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp332=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp332 != 0;_tmp332=_tmp332->tl){struct _tuple15 _tmp334;struct Cyc_List_List*
_tmp335;struct Cyc_Absyn_Exp*_tmp336;struct _tuple15*_tmp333=(struct _tuple15*)
_tmp332->hd;_tmp334=*_tmp333;_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp335 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp335->tl != 0){const char*_tmpB28;void*_tmpB27;(_tmpB27=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB28="multiple designators in array",
_tag_dyneither(_tmpB28,sizeof(char),30))),_tag_dyneither(_tmpB27,sizeof(void*),0)));}{
void*_tmp339=(void*)_tmp335->hd;void*_tmp33A=_tmp339;struct Cyc_Absyn_Exp*_tmp33C;
_LL16C: {struct Cyc_Absyn_ArrayElement_struct*_tmp33B=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp33A;if(_tmp33B->tag != 0)goto _LL16E;else{_tmp33C=_tmp33B->f1;}}_LL16D: Cyc_Toc_exp_to_c(
nv,_tmp33C);e_index=_tmp33C;goto _LL16B;_LL16E: {struct Cyc_Absyn_FieldName_struct*
_tmp33D=(struct Cyc_Absyn_FieldName_struct*)_tmp33A;if(_tmp33D->tag != 1)goto
_LL16B;}_LL16F: {const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB2B="field name designators in array",
_tag_dyneither(_tmpB2B,sizeof(char),32))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}
_LL16B:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp340=_tmp336->r;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Vardecl*_tmp344;
struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp346;int _tmp347;void*_tmp349;
struct Cyc_List_List*_tmp34A;_LL171: {struct Cyc_Absyn_Array_e_struct*_tmp341=(
struct Cyc_Absyn_Array_e_struct*)_tmp340;if(_tmp341->tag != 27)goto _LL173;else{
_tmp342=_tmp341->f1;}}_LL172: s=Cyc_Toc_init_array(nv,lval,_tmp342,s);goto _LL170;
_LL173: {struct Cyc_Absyn_Comprehension_e_struct*_tmp343=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp340;if(_tmp343->tag != 28)goto _LL175;else{_tmp344=_tmp343->f1;_tmp345=_tmp343->f2;
_tmp346=_tmp343->f3;_tmp347=_tmp343->f4;}}_LL174: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp344,_tmp345,_tmp346,_tmp347,s,0);goto _LL170;_LL175: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp348=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp340;if(_tmp348->tag != 30)goto
_LL177;else{_tmp349=(void*)_tmp348->f1;_tmp34A=_tmp348->f2;}}_LL176: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp349,_tmp34A,s);goto _LL170;_LL177:;_LL178: Cyc_Toc_exp_to_c(nv,_tmp336);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp336,0),s,0);goto _LL170;_LL170:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp34B=vd->name;void*_tmp34C=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34E;struct _RegionHandle*_tmp34F;
struct Cyc_Toc_Env*_tmp34D=nv;_tmp34E=*_tmp34D;_tmp34F=_tmp34E.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB2E;struct Cyc_Absyn_Local_b_struct*_tmpB2D;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp34F,nv,_tmp34B,Cyc_Absyn_varb_exp(_tmp34B,(void*)((_tmpB2D=_cycalloc(sizeof(*
_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=4,((_tmpB2E.f1=vd,_tmpB2E)))),_tmpB2D)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp34B,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp34B,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp34B,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp34B,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp350=e2->r;struct Cyc_List_List*_tmp352;
struct Cyc_Absyn_Vardecl*_tmp354;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*
_tmp356;int _tmp357;void*_tmp359;struct Cyc_List_List*_tmp35A;_LL17A: {struct Cyc_Absyn_Array_e_struct*
_tmp351=(struct Cyc_Absyn_Array_e_struct*)_tmp350;if(_tmp351->tag != 27)goto _LL17C;
else{_tmp352=_tmp351->f1;}}_LL17B: body=Cyc_Toc_init_array(nv2,lval,_tmp352,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL17C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp353=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp350;if(_tmp353->tag != 28)goto _LL17E;else{_tmp354=_tmp353->f1;_tmp355=_tmp353->f2;
_tmp356=_tmp353->f3;_tmp357=_tmp353->f4;}}_LL17D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp354,_tmp355,_tmp356,_tmp357,Cyc_Toc_skip_stmt_dl(),0);goto _LL179;
_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp358=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp350;if(_tmp358->tag != 30)goto _LL180;else{_tmp359=(void*)_tmp358->f1;_tmp35A=
_tmp358->f2;}}_LL17F: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp359,_tmp35A,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL180:;_LL181: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL179;_LL179:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp34C,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp34B,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp35D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp35D != 0;_tmp35D=_tmp35D->tl){struct _tuple15 _tmp35F;
struct Cyc_List_List*_tmp360;struct Cyc_Absyn_Exp*_tmp361;struct _tuple15*_tmp35E=(
struct _tuple15*)_tmp35D->hd;_tmp35F=*_tmp35E;_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;
if(_tmp360 == 0){const char*_tmpB31;void*_tmpB30;(_tmpB30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB31="empty designator list",
_tag_dyneither(_tmpB31,sizeof(char),22))),_tag_dyneither(_tmpB30,sizeof(void*),0)));}
if(_tmp360->tl != 0){const char*_tmpB34;void*_tmpB33;(_tmpB33=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB34="too many designators in anonymous struct",
_tag_dyneither(_tmpB34,sizeof(char),41))),_tag_dyneither(_tmpB33,sizeof(void*),0)));}{
void*_tmp366=(void*)_tmp360->hd;struct _dyneither_ptr*_tmp368;_LL183: {struct Cyc_Absyn_FieldName_struct*
_tmp367=(struct Cyc_Absyn_FieldName_struct*)_tmp366;if(_tmp367->tag != 1)goto
_LL185;else{_tmp368=_tmp367->f1;}}_LL184: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp368,0);{void*_tmp369=_tmp361->r;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*_tmp36F;int _tmp370;void*
_tmp372;struct Cyc_List_List*_tmp373;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp36A=(struct Cyc_Absyn_Array_e_struct*)_tmp369;if(_tmp36A->tag != 27)goto _LL18A;
else{_tmp36B=_tmp36A->f1;}}_LL189: s=Cyc_Toc_init_array(nv,lval,_tmp36B,s);goto
_LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp36C=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp369;if(_tmp36C->tag != 28)goto _LL18C;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36C->f2;
_tmp36F=_tmp36C->f3;_tmp370=_tmp36C->f4;}}_LL18B: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp36D,_tmp36E,_tmp36F,_tmp370,s,0);goto _LL187;_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp371=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp369;if(_tmp371->tag != 30)goto
_LL18E;else{_tmp372=(void*)_tmp371->f1;_tmp373=_tmp371->f2;}}_LL18D: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp372,_tmp373,s);goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv,_tmp361);
if(Cyc_Toc_is_poly_field(struct_type,_tmp368))_tmp361=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp361);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp361,0),0),s,0);goto _LL187;_LL187:;}goto _LL182;}_LL185:;_LL186: {const char*
_tmpB37;void*_tmpB36;(_tmpB36=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="array designator in struct",
_tag_dyneither(_tmpB37,sizeof(char),27))),_tag_dyneither(_tmpB36,sizeof(void*),0)));}
_LL182:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp377;struct _RegionHandle*_tmp378;
struct Cyc_Toc_Env*_tmp376=nv;_tmp377=*_tmp376;_tmp378=_tmp377.rgn;{struct Cyc_List_List*
_tmp379=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple9*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp378,Cyc_Toc_tup_to_c,
es);void*_tmp37A=Cyc_Toc_add_tuple_type(_tmp379);struct _tuple0*_tmp37B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37C=Cyc_Absyn_var_exp(_tmp37B,0);struct Cyc_Absyn_Stmt*
_tmp37D=Cyc_Absyn_exp_stmt(_tmp37C,0);struct Cyc_Absyn_Exp*(*_tmp37E)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp37F=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp378,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37F);for(0;_tmp37F != 0;(
_tmp37F=_tmp37F->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37F->hd;
struct Cyc_Absyn_Exp*lval=_tmp37E(_tmp37C,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp380=e->r;struct Cyc_List_List*_tmp382;struct Cyc_Absyn_Vardecl*_tmp384;
struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*_tmp386;int _tmp387;_LL191: {
struct Cyc_Absyn_Array_e_struct*_tmp381=(struct Cyc_Absyn_Array_e_struct*)_tmp380;
if(_tmp381->tag != 27)goto _LL193;else{_tmp382=_tmp381->f1;}}_LL192: _tmp37D=Cyc_Toc_init_array(
nv,lval,_tmp382,_tmp37D);goto _LL190;_LL193: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp383=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp380;if(_tmp383->tag != 28)
goto _LL195;else{_tmp384=_tmp383->f1;_tmp385=_tmp383->f2;_tmp386=_tmp383->f3;
_tmp387=_tmp383->f4;}}_LL194: _tmp37D=Cyc_Toc_init_comprehension(nv,lval,_tmp384,
_tmp385,_tmp386,_tmp387,_tmp37D,0);goto _LL190;_LL195:;_LL196: Cyc_Toc_exp_to_c(nv,
e);_tmp37D=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37E(
_tmp37C,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp37D,0);goto _LL190;_LL190:;};}}
return Cyc_Toc_make_struct(nv,_tmp37B,_tmp37A,_tmp37D,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp388=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp388 != 0;_tmp388=_tmp388->tl){struct Cyc_Absyn_Aggrfield*
_tmp389=(struct Cyc_Absyn_Aggrfield*)_tmp388->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp389->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB3F;void*_tmpB3E[1];const char*_tmpB3D;void*_tmpB3C;(
_tmpB3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB3F.tag=0,((_tmpB3F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB3E[0]=& _tmpB3F,Cyc_aprintf(((_tmpB3D="get_member_offset %s failed",
_tag_dyneither(_tmpB3D,sizeof(char),28))),_tag_dyneither(_tmpB3E,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB3C,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp38E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38F=Cyc_Absyn_var_exp(_tmp38E,0);struct Cyc_Absyn_Stmt*
_tmp390=Cyc_Absyn_exp_stmt(_tmp38F,0);struct Cyc_Absyn_Exp*(*_tmp391)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp392=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp393=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp395;union Cyc_Absyn_AggrInfoU _tmp396;
_LL198: {struct Cyc_Absyn_AggrType_struct*_tmp394=(struct Cyc_Absyn_AggrType_struct*)
_tmp393;if(_tmp394->tag != 12)goto _LL19A;else{_tmp395=_tmp394->f1;_tmp396=_tmp395.aggr_info;}}
_LL199: ad=Cyc_Absyn_get_known_aggrdecl(_tmp396);goto _LL197;_LL19A:;_LL19B: {
const char*_tmpB42;void*_tmpB41;(_tmpB41=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB42="init_struct: bad struct type",
_tag_dyneither(_tmpB42,sizeof(char),29))),_tag_dyneither(_tmpB41,sizeof(void*),0)));}
_LL197:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp39A;struct _RegionHandle*_tmp39B;struct Cyc_Toc_Env*_tmp399=
nv;_tmp39A=*_tmp399;_tmp39B=_tmp39A.rgn;{struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp39B,dles);for(0;
_tmp39C != 0;_tmp39C=_tmp39C->tl){struct _tuple15 _tmp39E;struct Cyc_List_List*
_tmp39F;struct Cyc_Absyn_Exp*_tmp3A0;struct _tuple15*_tmp39D=(struct _tuple15*)
_tmp39C->hd;_tmp39E=*_tmp39D;_tmp39F=_tmp39E.f1;_tmp3A0=_tmp39E.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A0->topt))->v);
if(_tmp39F == 0){const char*_tmpB45;void*_tmpB44;(_tmpB44=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB45="empty designator list",
_tag_dyneither(_tmpB45,sizeof(char),22))),_tag_dyneither(_tmpB44,sizeof(void*),0)));}
if(_tmp39F->tl != 0){struct _tuple0*_tmp3A3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A4=Cyc_Absyn_var_exp(_tmp3A3,0);for(0;_tmp39F != 0;_tmp39F=_tmp39F->tl){void*
_tmp3A5=(void*)_tmp39F->hd;struct _dyneither_ptr*_tmp3A7;_LL19D: {struct Cyc_Absyn_FieldName_struct*
_tmp3A6=(struct Cyc_Absyn_FieldName_struct*)_tmp3A5;if(_tmp3A6->tag != 1)goto
_LL19F;else{_tmp3A7=_tmp3A6->f1;}}_LL19E: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A7))_tmp3A4=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A4);_tmp390=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp391(_tmp38F,_tmp3A7,0),_tmp3A4,0),0),
_tmp390,0);goto _LL19C;_LL19F:;_LL1A0: {const char*_tmpB48;void*_tmpB47;(_tmpB47=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB48="array designator in struct",_tag_dyneither(_tmpB48,sizeof(char),27))),
_tag_dyneither(_tmpB47,sizeof(void*),0)));}_LL19C:;}Cyc_Toc_exp_to_c(nv,_tmp3A0);
_tmp390=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A4,
_tmp3A0,0),0),_tmp390,0);}else{void*_tmp3AA=(void*)_tmp39F->hd;struct
_dyneither_ptr*_tmp3AC;_LL1A2: {struct Cyc_Absyn_FieldName_struct*_tmp3AB=(struct
Cyc_Absyn_FieldName_struct*)_tmp3AA;if(_tmp3AB->tag != 1)goto _LL1A4;else{_tmp3AC=
_tmp3AB->f1;}}_LL1A3: {struct Cyc_Absyn_Exp*lval=_tmp391(_tmp38F,_tmp3AC,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3AC);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp390=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp390,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3AD=_tmp3A0->r;
struct Cyc_List_List*_tmp3AF;struct Cyc_Absyn_Vardecl*_tmp3B1;struct Cyc_Absyn_Exp*
_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;int _tmp3B4;void*_tmp3B6;struct Cyc_List_List*
_tmp3B7;_LL1A7: {struct Cyc_Absyn_Array_e_struct*_tmp3AE=(struct Cyc_Absyn_Array_e_struct*)
_tmp3AD;if(_tmp3AE->tag != 27)goto _LL1A9;else{_tmp3AF=_tmp3AE->f1;}}_LL1A8:
_tmp390=Cyc_Toc_init_array(nv,lval,_tmp3AF,_tmp390);goto _LL1A6;_LL1A9: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3B0=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3AD;if(_tmp3B0->tag != 28)goto _LL1AB;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;
_tmp3B3=_tmp3B0->f3;_tmp3B4=_tmp3B0->f4;}}_LL1AA: _tmp390=Cyc_Toc_init_comprehension(
nv,lval,_tmp3B1,_tmp3B2,_tmp3B3,_tmp3B4,_tmp390,0);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B5=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3AD;if(_tmp3B5->tag != 30)goto
_LL1AD;else{_tmp3B6=(void*)_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;}}_LL1AC: _tmp390=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B6,_tmp3B7,_tmp390);goto _LL1A6;_LL1AD:;_LL1AE: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A0->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3A0);{struct Cyc_Absyn_Aggrfield*_tmp3B8=Cyc_Absyn_lookup_decl_field(ad,
_tmp3AC);if(Cyc_Toc_is_poly_field(struct_type,_tmp3AC) && !was_ptr_type)_tmp3A0=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A0);if(has_exists)_tmp3A0=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B8))->type),
_tmp3A0);_tmp390=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3A0,0),0),_tmp390,0);goto _LL1A6;};}_LL1A6:;}goto _LL1A1;}_LL1A4:;_LL1A5: {
const char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="array designator in struct",
_tag_dyneither(_tmpB4B,sizeof(char),27))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}
_LL1A1:;}}}return Cyc_Toc_make_struct(nv,_tmp38E,_tmp392,_tmp390,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB4E;struct Cyc_Absyn_Increment_e_struct*
_tmpB4D;return Cyc_Absyn_new_exp((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((
_tmpB4D[0]=((_tmpB4E.tag=5,((_tmpB4E.f1=e1,((_tmpB4E.f2=incr,_tmpB4E)))))),
_tmpB4D)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3BD=e1->r;struct Cyc_Absyn_Stmt*
_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C4;
struct _dyneither_ptr*_tmp3C5;int _tmp3C6;int _tmp3C7;_LL1B0: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3BE=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3BD;if(_tmp3BE->tag != 37)goto
_LL1B2;else{_tmp3BF=_tmp3BE->f1;}}_LL1B1: Cyc_Toc_lvalue_assign_stmt(_tmp3BF,fs,f,
f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp3C0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3BD;if(_tmp3C0->tag != 15)goto _LL1B4;else{_tmp3C1=(void*)_tmp3C0->f1;_tmp3C2=
_tmp3C0->f2;}}_LL1B3: Cyc_Toc_lvalue_assign(_tmp3C2,fs,f,f_env);goto _LL1AF;_LL1B4: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3C3=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3BD;if(_tmp3C3->tag != 22)goto _LL1B6;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;
_tmp3C6=_tmp3C3->f3;_tmp3C7=_tmp3C3->f4;}}_LL1B5: e1->r=_tmp3C4->r;{struct Cyc_List_List*
_tmpB4F;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB4F=_cycalloc(
sizeof(*_tmpB4F)),((_tmpB4F->hd=_tmp3C5,((_tmpB4F->tl=fs,_tmpB4F)))))),f,f_env);}
goto _LL1AF;_LL1B6:;_LL1B7: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1AF;}
_LL1AF:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3C9=s->r;
struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Decl*_tmp3CD;struct Cyc_Absyn_Stmt*
_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D0;_LL1B9: {struct Cyc_Absyn_Exp_s_struct*
_tmp3CA=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C9;if(_tmp3CA->tag != 1)goto _LL1BB;
else{_tmp3CB=_tmp3CA->f1;}}_LL1BA: Cyc_Toc_lvalue_assign(_tmp3CB,fs,f,f_env);goto
_LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_struct*_tmp3CC=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3C9;if(_tmp3CC->tag != 12)goto _LL1BD;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}
_LL1BC: Cyc_Toc_lvalue_assign_stmt(_tmp3CE,fs,f,f_env);goto _LL1B8;_LL1BD: {struct
Cyc_Absyn_Seq_s_struct*_tmp3CF=(struct Cyc_Absyn_Seq_s_struct*)_tmp3C9;if(_tmp3CF->tag
!= 2)goto _LL1BF;else{_tmp3D0=_tmp3CF->f2;}}_LL1BE: Cyc_Toc_lvalue_assign_stmt(
_tmp3D0,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {const char*_tmpB53;void*_tmpB52[1];
struct Cyc_String_pa_struct _tmpB51;(_tmpB51.tag=0,((_tmpB51.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB52[0]=&
_tmpB51,Cyc_Toc_toc_impos(((_tmpB53="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB53,sizeof(char),23))),_tag_dyneither(_tmpB52,sizeof(void*),1)))))));}_LL1B8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3D4=
e->r;void*_tmp3D6;void**_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp**
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DB;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D5=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D4;if(_tmp3D5->tag != 15)goto _LL1C4;
else{_tmp3D6=(void**)& _tmp3D5->f1;_tmp3D7=(void**)((void**)& _tmp3D5->f1);_tmp3D8=
_tmp3D5->f2;_tmp3D9=(struct Cyc_Absyn_Exp**)& _tmp3D5->f2;}}_LL1C3:*_tmp3D9=Cyc_Toc_push_address_exp(*
_tmp3D9);*_tmp3D7=Cyc_Absyn_cstar_typ(*_tmp3D7,Cyc_Toc_mt_tq);return e;_LL1C4: {
struct Cyc_Absyn_Deref_e_struct*_tmp3DA=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D4;
if(_tmp3DA->tag != 21)goto _LL1C6;else{_tmp3DB=_tmp3DA->f1;}}_LL1C5: return _tmp3DB;
_LL1C6:;_LL1C7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB57;void*_tmpB56[1];struct Cyc_String_pa_struct _tmpB55;(_tmpB55.tag=0,((
_tmpB55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB56[0]=& _tmpB55,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB57="can't take & of %s",_tag_dyneither(_tmpB57,sizeof(char),19))),
_tag_dyneither(_tmpB56,sizeof(void*),1)))))));};_LL1C1:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB58;result=((
_tmpB58=_region_malloc(r2,sizeof(*_tmpB58)),((_tmpB58->hd=(void*)f((void*)x->hd,
env),((_tmpB58->tl=0,_tmpB58))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB59;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB59=_region_malloc(r2,sizeof(*_tmpB59)),((_tmpB59->hd=(void*)f((void*)x->hd,
env),((_tmpB59->tl=0,_tmpB59))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*_tmpB5A;
return(_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->f1=0,((_tmpB5A->f2=e,
_tmpB5A)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3E2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E4;_LL1C9: {struct Cyc_Absyn_PointerType_struct*
_tmp3E3=(struct Cyc_Absyn_PointerType_struct*)_tmp3E2;if(_tmp3E3->tag != 5)goto
_LL1CB;else{_tmp3E4=_tmp3E3->f1;}}_LL1CA: return _tmp3E4;_LL1CB:;_LL1CC: {const
char*_tmpB5D;void*_tmpB5C;(_tmpB5C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5D="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB5D,sizeof(char),28))),_tag_dyneither(_tmpB5C,sizeof(void*),0)));}
_LL1C8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3E7=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3EA;enum Cyc_Absyn_Size_of _tmp3EB;enum Cyc_Absyn_Sign
_tmp3ED;enum Cyc_Absyn_Size_of _tmp3EE;enum Cyc_Absyn_Sign _tmp3F2;enum Cyc_Absyn_Size_of
_tmp3F3;enum Cyc_Absyn_Sign _tmp3F5;enum Cyc_Absyn_Size_of _tmp3F6;enum Cyc_Absyn_Sign
_tmp3F8;enum Cyc_Absyn_Size_of _tmp3F9;_LL1CE: {struct Cyc_Absyn_PointerType_struct*
_tmp3E8=(struct Cyc_Absyn_PointerType_struct*)_tmp3E7;if(_tmp3E8->tag != 5)goto
_LL1D0;}_LL1CF: res=Cyc_Absyn_null_exp(0);goto _LL1CD;_LL1D0: {struct Cyc_Absyn_IntType_struct*
_tmp3E9=(struct Cyc_Absyn_IntType_struct*)_tmp3E7;if(_tmp3E9->tag != 6)goto _LL1D2;
else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3E9->f2;if(_tmp3EB != Cyc_Absyn_Char_sz)goto
_LL1D2;}}_LL1D1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3EA,'\000'),0);goto
_LL1CD;_LL1D2: {struct Cyc_Absyn_IntType_struct*_tmp3EC=(struct Cyc_Absyn_IntType_struct*)
_tmp3E7;if(_tmp3EC->tag != 6)goto _LL1D4;else{_tmp3ED=_tmp3EC->f1;_tmp3EE=_tmp3EC->f2;
if(_tmp3EE != Cyc_Absyn_Short_sz)goto _LL1D4;}}_LL1D3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3ED,0),0);goto _LL1CD;_LL1D4: {struct Cyc_Absyn_EnumType_struct*_tmp3EF=(
struct Cyc_Absyn_EnumType_struct*)_tmp3E7;if(_tmp3EF->tag != 14)goto _LL1D6;}_LL1D5:
goto _LL1D7;_LL1D6: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3F0=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3E7;if(_tmp3F0->tag != 15)goto _LL1D8;}_LL1D7: _tmp3F2=Cyc_Absyn_Unsigned;goto
_LL1D9;_LL1D8: {struct Cyc_Absyn_IntType_struct*_tmp3F1=(struct Cyc_Absyn_IntType_struct*)
_tmp3E7;if(_tmp3F1->tag != 6)goto _LL1DA;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;
if(_tmp3F3 != Cyc_Absyn_Int_sz)goto _LL1DA;}}_LL1D9: _tmp3F5=_tmp3F2;goto _LL1DB;
_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3F4=(struct Cyc_Absyn_IntType_struct*)
_tmp3E7;if(_tmp3F4->tag != 6)goto _LL1DC;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;
if(_tmp3F6 != Cyc_Absyn_Long_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp3F5,0),0);goto _LL1CD;_LL1DC: {struct Cyc_Absyn_IntType_struct*_tmp3F7=(struct
Cyc_Absyn_IntType_struct*)_tmp3E7;if(_tmp3F7->tag != 6)goto _LL1DE;else{_tmp3F8=
_tmp3F7->f1;_tmp3F9=_tmp3F7->f2;if(_tmp3F9 != Cyc_Absyn_LongLong_sz)goto _LL1DE;}}
_LL1DD: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3F8,(long long)0),0);
goto _LL1CD;_LL1DE: {struct Cyc_Absyn_FloatType_struct*_tmp3FA=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E7;if(_tmp3FA->tag != 7)goto _LL1E0;}_LL1DF: goto _LL1E1;_LL1E0: {struct Cyc_Absyn_DoubleType_struct*
_tmp3FB=(struct Cyc_Absyn_DoubleType_struct*)_tmp3E7;if(_tmp3FB->tag != 8)goto
_LL1E2;}_LL1E1:{const char*_tmpB5E;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB5E="0.0",_tag_dyneither(_tmpB5E,sizeof(char),4)))),0);}goto _LL1CD;_LL1E2:;
_LL1E3: {const char*_tmpB62;void*_tmpB61[1];struct Cyc_String_pa_struct _tmpB60;(
_tmpB60.tag=0,((_tmpB60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB61[0]=& _tmpB60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB62="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB62,sizeof(char),40))),_tag_dyneither(_tmpB61,sizeof(void*),1)))))));}
_LL1CD:;}{struct Cyc_Core_Opt*_tmpB63;res->topt=((_tmpB63=_cycalloc(sizeof(*
_tmpB63)),((_tmpB63->v=t,_tmpB63))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp401=Cyc_Toc_typ_to_c(elt_type);void*_tmp402=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp403=Cyc_Absyn_cstar_typ(_tmp401,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB64;struct Cyc_Core_Opt*_tmp404=(_tmpB64=_cycalloc(sizeof(*_tmpB64)),((
_tmpB64->v=_tmp403,_tmpB64)));struct Cyc_Absyn_Exp*xinit;{void*_tmp405=e1->r;
struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*
_tmp40A;_LL1E5: {struct Cyc_Absyn_Deref_e_struct*_tmp406=(struct Cyc_Absyn_Deref_e_struct*)
_tmp405;if(_tmp406->tag != 21)goto _LL1E7;else{_tmp407=_tmp406->f1;}}_LL1E6: if(!
is_dyneither){_tmp407=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp407,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB65;_tmp407->topt=((_tmpB65=_cycalloc(sizeof(*_tmpB65)),((
_tmpB65->v=fat_ptr_type,_tmpB65))));};}Cyc_Toc_exp_to_c(nv,_tmp407);xinit=
_tmp407;goto _LL1E4;_LL1E7: {struct Cyc_Absyn_Subscript_e_struct*_tmp408=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp405;if(_tmp408->tag != 24)goto _LL1E9;else{
_tmp409=_tmp408->f1;_tmp40A=_tmp408->f2;}}_LL1E8: if(!is_dyneither){_tmp409=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp409,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB66;
_tmp409->topt=((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66->v=fat_ptr_type,
_tmpB66))));};}Cyc_Toc_exp_to_c(nv,_tmp409);Cyc_Toc_exp_to_c(nv,_tmp40A);{struct
Cyc_Absyn_Exp*_tmpB67[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB67[2]=_tmp40A,((_tmpB67[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB67[0]=_tmp409,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB67,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E4;_LL1E9:;_LL1EA: {const char*_tmpB6A;void*_tmpB69;(_tmpB69=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6A="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB6A,sizeof(char),53))),_tag_dyneither(_tmpB69,sizeof(void*),0)));}
_LL1E4:;}{struct _tuple0*_tmp410=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp412;
struct _RegionHandle*_tmp413;struct Cyc_Toc_Env*_tmp411=nv;_tmp412=*_tmp411;
_tmp413=_tmp412.rgn;{struct Cyc_Toc_Env*_tmp414=Cyc_Toc_add_varmap(_tmp413,nv,
_tmp410,Cyc_Absyn_var_exp(_tmp410,0));struct Cyc_Absyn_Vardecl*_tmpB6B;struct Cyc_Absyn_Vardecl*
_tmp415=(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->sc=Cyc_Absyn_Public,((
_tmpB6B->name=_tmp410,((_tmpB6B->tq=Cyc_Toc_mt_tq,((_tmpB6B->type=_tmp402,((
_tmpB6B->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB6B->rgn=0,((_tmpB6B->attributes=
0,((_tmpB6B->escapes=0,_tmpB6B)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB6E;struct Cyc_Absyn_Local_b_struct*_tmpB6D;struct Cyc_Absyn_Local_b_struct*
_tmp416=(_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D[0]=((_tmpB6E.tag=4,((
_tmpB6E.f1=_tmp415,_tmpB6E)))),_tmpB6D)));struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_varb_exp(
_tmp410,(void*)_tmp416,0);{struct Cyc_Core_Opt*_tmpB6F;_tmp417->topt=((_tmpB6F=
_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F->v=fat_ptr_type,_tmpB6F))));}{struct Cyc_Absyn_Exp*
_tmp419=Cyc_Absyn_deref_exp(_tmp417,0);{struct Cyc_Core_Opt*_tmpB70;_tmp419->topt=((
_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->v=elt_type,_tmpB70))));}Cyc_Toc_exp_to_c(
_tmp414,_tmp419);{struct _tuple0*_tmp41B=Cyc_Toc_temp_var();_tmp414=Cyc_Toc_add_varmap(
_tmp413,_tmp414,_tmp41B,Cyc_Absyn_var_exp(_tmp41B,0));{struct Cyc_Absyn_Vardecl*
_tmpB71;struct Cyc_Absyn_Vardecl*_tmp41C=(_tmpB71=_cycalloc(sizeof(*_tmpB71)),((
_tmpB71->sc=Cyc_Absyn_Public,((_tmpB71->name=_tmp41B,((_tmpB71->tq=Cyc_Toc_mt_tq,((
_tmpB71->type=_tmp401,((_tmpB71->initializer=(struct Cyc_Absyn_Exp*)_tmp419,((
_tmpB71->rgn=0,((_tmpB71->attributes=0,((_tmpB71->escapes=0,_tmpB71)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB74;struct Cyc_Absyn_Local_b_struct*_tmpB73;
struct Cyc_Absyn_Local_b_struct*_tmp41D=(_tmpB73=_cycalloc(sizeof(*_tmpB73)),((
_tmpB73[0]=((_tmpB74.tag=4,((_tmpB74.f1=_tmp41C,_tmpB74)))),_tmpB73)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_varb_exp(_tmp41B,(
void*)_tmp41D,0);_tmp41E->topt=_tmp419->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp41E,e2,0);z_init->topt=_tmp41E->topt;}Cyc_Toc_exp_to_c(
_tmp414,z_init);{struct _tuple0*_tmp41F=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB75;struct Cyc_Absyn_Vardecl*_tmp420=(_tmpB75=_cycalloc(sizeof(*_tmpB75)),((
_tmpB75->sc=Cyc_Absyn_Public,((_tmpB75->name=_tmp41F,((_tmpB75->tq=Cyc_Toc_mt_tq,((
_tmpB75->type=_tmp401,((_tmpB75->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB75->rgn=0,((_tmpB75->attributes=0,((_tmpB75->escapes=0,_tmpB75)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB78;struct Cyc_Absyn_Local_b_struct*_tmpB77;
struct Cyc_Absyn_Local_b_struct*_tmp421=(_tmpB77=_cycalloc(sizeof(*_tmpB77)),((
_tmpB77[0]=((_tmpB78.tag=4,((_tmpB78.f1=_tmp420,_tmpB78)))),_tmpB77)));_tmp414=
Cyc_Toc_add_varmap(_tmp413,_tmp414,_tmp41F,Cyc_Absyn_var_exp(_tmp41F,0));{struct
Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_varb_exp(_tmp41B,(void*)_tmp41D,0);_tmp422->topt=
_tmp419->topt;{struct Cyc_Absyn_Exp*_tmp423=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp422,_tmp423,0);{
struct Cyc_Core_Opt*_tmpB79;_tmp424->topt=((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((
_tmpB79->v=Cyc_Absyn_sint_typ,_tmpB79))));}Cyc_Toc_exp_to_c(_tmp414,_tmp424);{
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_varb_exp(_tmp41F,(void*)_tmp421,0);_tmp426->topt=
_tmp419->topt;{struct Cyc_Absyn_Exp*_tmp427=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp426,_tmp427,0);{
struct Cyc_Core_Opt*_tmpB7A;_tmp428->topt=((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((
_tmpB7A->v=Cyc_Absyn_sint_typ,_tmpB7A))));}Cyc_Toc_exp_to_c(_tmp414,_tmp428);{
struct Cyc_Absyn_Exp*_tmpB7B[2];struct Cyc_List_List*_tmp42A=(_tmpB7B[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB7B[0]=Cyc_Absyn_varb_exp(_tmp410,(void*)
_tmp416,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB7B,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp42B=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42A,0),
_tmp42B,0);{struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp424,_tmp428,0),0);struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp410,(void*)_tmp416,0),Cyc_Toc_curr_sp,0);_tmp42E=Cyc_Toc_cast_it(
_tmp403,_tmp42E);{struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_deref_exp(_tmp42E,0);
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_assign_exp(_tmp42F,Cyc_Absyn_var_exp(
_tmp41F,0),0);struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_exp_stmt(_tmp430,0);_tmp431=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42C,_tmp42D,Cyc_Absyn_skip_stmt(
0),0),_tmp431,0);{struct Cyc_Absyn_Var_d_struct*_tmpB81;struct Cyc_Absyn_Var_d_struct
_tmpB80;struct Cyc_Absyn_Decl*_tmpB7F;_tmp431=Cyc_Absyn_decl_stmt(((_tmpB7F=
_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->r=(void*)((_tmpB81=_cycalloc(sizeof(*
_tmpB81)),((_tmpB81[0]=((_tmpB80.tag=0,((_tmpB80.f1=_tmp420,_tmpB80)))),_tmpB81)))),((
_tmpB7F->loc=0,_tmpB7F)))))),_tmp431,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB87;
struct Cyc_Absyn_Var_d_struct _tmpB86;struct Cyc_Absyn_Decl*_tmpB85;_tmp431=Cyc_Absyn_decl_stmt(((
_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->r=(void*)((_tmpB87=_cycalloc(
sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB86.tag=0,((_tmpB86.f1=_tmp41C,_tmpB86)))),
_tmpB87)))),((_tmpB85->loc=0,_tmpB85)))))),_tmp431,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB8D;struct Cyc_Absyn_Var_d_struct _tmpB8C;struct Cyc_Absyn_Decl*_tmpB8B;_tmp431=
Cyc_Absyn_decl_stmt(((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->r=(void*)((
_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB8C.tag=0,((_tmpB8C.f1=
_tmp415,_tmpB8C)))),_tmpB8D)))),((_tmpB8B->loc=0,_tmpB8B)))))),_tmp431,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp431);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp446=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp448;union Cyc_Absyn_AggrInfoU _tmp449;_LL1EC: {struct Cyc_Absyn_AggrType_struct*
_tmp447=(struct Cyc_Absyn_AggrType_struct*)_tmp446;if(_tmp447->tag != 12)goto
_LL1EE;else{_tmp448=_tmp447->f1;_tmp449=_tmp448.aggr_info;}}_LL1ED: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp449);goto _LL1EB;_LL1EE:;_LL1EF: {struct Cyc_String_pa_struct _tmpB95;void*
_tmpB94[1];const char*_tmpB93;void*_tmpB92;(_tmpB92=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpB95.tag=0,((_tmpB95.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB94[0]=& _tmpB95,Cyc_aprintf(((
_tmpB93="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB93,sizeof(char),51))),_tag_dyneither(_tmpB94,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB92,sizeof(void*),0)));}_LL1EB:;}{struct _tuple0*_tmp44E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_var_exp(_tmp44E,0);struct Cyc_Absyn_Exp*
_tmp450=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44F,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_neq_exp(_tmp450,_tmp451,0);struct Cyc_Absyn_Exp*
_tmp453=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44F,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp454=Cyc_Absyn_exp_stmt(_tmp453,0);struct Cyc_Absyn_Stmt*
_tmp455=Cyc_Absyn_ifthenelse_stmt(_tmp452,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp456=Cyc_Absyn_declare_stmt(_tmp44E,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp455,_tmp454,0),0);return Cyc_Toc_stmt_exp_r(
_tmp456);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp457=e->r;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp45B;struct
_dyneither_ptr*_tmp45C;int _tmp45D;int*_tmp45E;struct Cyc_Absyn_Exp*_tmp460;struct
_dyneither_ptr*_tmp461;int _tmp462;int*_tmp463;_LL1F1: {struct Cyc_Absyn_Cast_e_struct*
_tmp458=(struct Cyc_Absyn_Cast_e_struct*)_tmp457;if(_tmp458->tag != 15)goto _LL1F3;
else{_tmp459=_tmp458->f2;}}_LL1F2: {const char*_tmpB98;void*_tmpB97;(_tmpB97=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB98="cast on lhs!",_tag_dyneither(_tmpB98,sizeof(char),13))),_tag_dyneither(
_tmpB97,sizeof(void*),0)));}_LL1F3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp45A=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp457;if(_tmp45A->tag != 22)goto _LL1F5;
else{_tmp45B=_tmp45A->f1;_tmp45C=_tmp45A->f2;_tmp45D=_tmp45A->f4;_tmp45E=(int*)&
_tmp45A->f4;}}_LL1F4: {void*_tmp466=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45B->topt))->v);struct Cyc_Absyn_AggrInfo _tmp468;union Cyc_Absyn_AggrInfoU
_tmp469;_LL1FA: {struct Cyc_Absyn_AggrType_struct*_tmp467=(struct Cyc_Absyn_AggrType_struct*)
_tmp466;if(_tmp467->tag != 12)goto _LL1FC;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.aggr_info;}}
_LL1FB: {struct Cyc_Absyn_Aggrdecl*_tmp46A=Cyc_Absyn_get_known_aggrdecl(_tmp469);*
f_tag=Cyc_Toc_get_member_offset(_tmp46A,_tmp45C);{const char*_tmpB9D;void*_tmpB9C[
2];struct Cyc_String_pa_struct _tmpB9B;struct Cyc_String_pa_struct _tmpB9A;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9A.tag=0,((_tmpB9A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp45C),((_tmpB9B.tag=0,((_tmpB9B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp46A->name).f2),((_tmpB9C[0]=&
_tmpB9B,((_tmpB9C[1]=& _tmpB9A,Cyc_aprintf(((_tmpB9D="_union_%s_%s",
_tag_dyneither(_tmpB9D,sizeof(char),13))),_tag_dyneither(_tmpB9C,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpB9E;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9E=
_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E[0]=str,_tmpB9E)))));}if(clear_read)*
_tmp45E=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46A->impl))->tagged;};}
_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1F5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp45F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp457;if(_tmp45F->tag != 23)goto
_LL1F7;else{_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;_tmp462=_tmp45F->f4;_tmp463=(
int*)& _tmp45F->f4;}}_LL1F6: {void*_tmp470=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp460->topt))->v);struct Cyc_Absyn_PtrInfo _tmp472;void*_tmp473;
_LL1FF: {struct Cyc_Absyn_PointerType_struct*_tmp471=(struct Cyc_Absyn_PointerType_struct*)
_tmp470;if(_tmp471->tag != 5)goto _LL201;else{_tmp472=_tmp471->f1;_tmp473=_tmp472.elt_typ;}}
_LL200: {void*_tmp474=Cyc_Tcutil_compress(_tmp473);struct Cyc_Absyn_AggrInfo
_tmp476;union Cyc_Absyn_AggrInfoU _tmp477;_LL204: {struct Cyc_Absyn_AggrType_struct*
_tmp475=(struct Cyc_Absyn_AggrType_struct*)_tmp474;if(_tmp475->tag != 12)goto
_LL206;else{_tmp476=_tmp475->f1;_tmp477=_tmp476.aggr_info;}}_LL205: {struct Cyc_Absyn_Aggrdecl*
_tmp478=Cyc_Absyn_get_known_aggrdecl(_tmp477);*f_tag=Cyc_Toc_get_member_offset(
_tmp478,_tmp461);{const char*_tmpBA3;void*_tmpBA2[2];struct Cyc_String_pa_struct
_tmpBA1;struct Cyc_String_pa_struct _tmpBA0;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp461),((_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp478->name).f2),((_tmpBA2[0]=& _tmpBA1,((_tmpBA2[1]=&
_tmpBA0,Cyc_aprintf(((_tmpBA3="_union_%s_%s",_tag_dyneither(_tmpBA3,sizeof(char),
13))),_tag_dyneither(_tmpBA2,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBA4;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((
_tmpBA4[0]=str,_tmpBA4)))));}if(clear_read)*_tmp463=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp478->impl))->tagged;};}_LL206:;_LL207: return 0;_LL203:;}_LL201:;
_LL202: return 0;_LL1FE:;}_LL1F7:;_LL1F8: return 0;_LL1F0:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp47E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp47E,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp47F=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp47F,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp47E,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC03(unsigned int*_tmpC02,unsigned int*_tmpC01,struct _tuple0***
_tmpBFF){for(*_tmpC02=0;*_tmpC02 < *_tmpC01;(*_tmpC02)++){(*_tmpBFF)[*_tmpC02]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp480=e->r;if(e->topt == 0){const char*_tmpBA8;void*_tmpBA7[1];struct Cyc_String_pa_struct
_tmpBA6;(_tmpBA6.tag=0,((_tmpBA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBA7[0]=& _tmpBA6,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA8="exp_to_c: no type for %s",
_tag_dyneither(_tmpBA8,sizeof(char),25))),_tag_dyneither(_tmpBA7,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp484=
_tmp480;union Cyc_Absyn_Cnst _tmp486;int _tmp487;struct _tuple0*_tmp48A;void*_tmp48B;
struct _tuple0*_tmp48D;enum Cyc_Absyn_Primop _tmp48F;struct Cyc_List_List*_tmp490;
struct Cyc_Absyn_Exp*_tmp492;enum Cyc_Absyn_Incrementor _tmp493;struct Cyc_Absyn_Exp*
_tmp495;struct Cyc_Core_Opt*_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*
_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_List_List*
_tmp4AA;struct Cyc_Absyn_VarargCallInfo*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AD;
struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_VarargCallInfo*_tmp4AF;struct Cyc_Absyn_VarargCallInfo
_tmp4B0;int _tmp4B1;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_VarargInfo*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_List_List*_tmp4BA;void*_tmp4BC;void**_tmp4BD;struct Cyc_Absyn_Exp*
_tmp4BE;int _tmp4BF;enum Cyc_Absyn_Coercion _tmp4C0;struct Cyc_Absyn_Exp*_tmp4C2;
struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*
_tmp4C7;void*_tmp4C9;void*_tmp4CB;void*_tmp4CC;struct _dyneither_ptr*_tmp4CE;void*
_tmp4D0;void*_tmp4D1;unsigned int _tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*
_tmp4D7;struct _dyneither_ptr*_tmp4D8;int _tmp4D9;int _tmp4DA;struct Cyc_Absyn_Exp*
_tmp4DC;struct _dyneither_ptr*_tmp4DD;int _tmp4DE;int _tmp4DF;struct Cyc_Absyn_Exp*
_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*
_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*
_tmp4EA;int _tmp4EB;struct _tuple0*_tmp4ED;struct Cyc_List_List*_tmp4EE;struct Cyc_List_List*
_tmp4EF;struct Cyc_Absyn_Aggrdecl*_tmp4F0;void*_tmp4F2;struct Cyc_List_List*
_tmp4F3;struct Cyc_List_List*_tmp4F5;struct Cyc_Absyn_Datatypedecl*_tmp4F6;struct
Cyc_Absyn_Datatypefield*_tmp4F7;struct Cyc_Absyn_MallocInfo _tmp4FB;int _tmp4FC;
struct Cyc_Absyn_Exp*_tmp4FD;void**_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;int _tmp500;
struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*
_tmp505;struct _dyneither_ptr*_tmp506;struct Cyc_Absyn_Stmt*_tmp508;_LL209: {
struct Cyc_Absyn_Const_e_struct*_tmp485=(struct Cyc_Absyn_Const_e_struct*)_tmp484;
if(_tmp485->tag != 0)goto _LL20B;else{_tmp486=_tmp485->f1;if((_tmp486.Null_c).tag
!= 1)goto _LL20B;_tmp487=(int)(_tmp486.Null_c).val;}}_LL20A: {struct Cyc_Absyn_Exp*
_tmp50C=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp50C,
_tmp50C))->r;else{struct Cyc_Absyn_Exp*_tmpBA9[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBA9[2]=_tmp50C,((_tmpBA9[1]=_tmp50C,((_tmpBA9[0]=_tmp50C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL208;}_LL20B: {struct Cyc_Absyn_Const_e_struct*
_tmp488=(struct Cyc_Absyn_Const_e_struct*)_tmp484;if(_tmp488->tag != 0)goto _LL20D;}
_LL20C: goto _LL208;_LL20D: {struct Cyc_Absyn_Var_e_struct*_tmp489=(struct Cyc_Absyn_Var_e_struct*)
_tmp484;if(_tmp489->tag != 1)goto _LL20F;else{_tmp48A=_tmp489->f1;_tmp48B=(void*)
_tmp489->f2;}}_LL20E:{struct _handler_cons _tmp50E;_push_handler(& _tmp50E);{int
_tmp510=0;if(setjmp(_tmp50E.handler))_tmp510=1;if(!_tmp510){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp48A))->r;;_pop_handler();}else{void*_tmp50F=(void*)_exn_thrown;void*
_tmp512=_tmp50F;_LL260: {struct Cyc_Dict_Absent_struct*_tmp513=(struct Cyc_Dict_Absent_struct*)
_tmp512;if(_tmp513->tag != Cyc_Dict_Absent)goto _LL262;}_LL261: {const char*_tmpBAD;
void*_tmpBAC[1];struct Cyc_String_pa_struct _tmpBAB;(_tmpBAB.tag=0,((_tmpBAB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48A)),((
_tmpBAC[0]=& _tmpBAB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAD="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAD,sizeof(char),
32))),_tag_dyneither(_tmpBAC,sizeof(void*),1)))))));}_LL262:;_LL263:(void)_throw(
_tmp512);_LL25F:;}};}goto _LL208;_LL20F: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp48C=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp484;if(_tmp48C->tag != 2)goto
_LL211;else{_tmp48D=_tmp48C->f1;}}_LL210: {const char*_tmpBB0;void*_tmpBAF;(
_tmpBAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB0="unknownid",_tag_dyneither(_tmpBB0,sizeof(char),10))),_tag_dyneither(
_tmpBAF,sizeof(void*),0)));}_LL211: {struct Cyc_Absyn_Primop_e_struct*_tmp48E=(
struct Cyc_Absyn_Primop_e_struct*)_tmp484;if(_tmp48E->tag != 3)goto _LL213;else{
_tmp48F=_tmp48E->f1;_tmp490=_tmp48E->f2;}}_LL212: {struct Cyc_List_List*_tmp519=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp490);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp490);switch(_tmp48F){case Cyc_Absyn_Numelts: _LL264: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp490))->hd;{void*_tmp51A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp51C;struct Cyc_Absyn_Exp*
_tmp51D;struct Cyc_Absyn_PtrInfo _tmp51F;void*_tmp520;struct Cyc_Absyn_PtrAtts
_tmp521;union Cyc_Absyn_Constraint*_tmp522;union Cyc_Absyn_Constraint*_tmp523;
union Cyc_Absyn_Constraint*_tmp524;_LL267: {struct Cyc_Absyn_ArrayType_struct*
_tmp51B=(struct Cyc_Absyn_ArrayType_struct*)_tmp51A;if(_tmp51B->tag != 9)goto
_LL269;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51C.num_elts;}}_LL268: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp51D))){const char*_tmpBB3;void*_tmpBB2;(
_tmpBB2=0,Cyc_Tcutil_terr(e->loc,((_tmpBB3="can't calculate numelts",
_tag_dyneither(_tmpBB3,sizeof(char),24))),_tag_dyneither(_tmpBB2,sizeof(void*),0)));}
e->r=_tmp51D->r;goto _LL266;_LL269: {struct Cyc_Absyn_PointerType_struct*_tmp51E=(
struct Cyc_Absyn_PointerType_struct*)_tmp51A;if(_tmp51E->tag != 5)goto _LL26B;else{
_tmp51F=_tmp51E->f1;_tmp520=_tmp51F.elt_typ;_tmp521=_tmp51F.ptr_atts;_tmp522=
_tmp521.nullable;_tmp523=_tmp521.bounds;_tmp524=_tmp521.zero_term;}}_LL26A:{void*
_tmp527=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp523);struct Cyc_Absyn_Exp*_tmp52A;_LL26E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp528=(struct Cyc_Absyn_DynEither_b_struct*)_tmp527;if(_tmp528->tag != 0)goto
_LL270;}_LL26F:{struct Cyc_Absyn_Exp*_tmpBB4[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBB4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp520),0),((_tmpBB4[0]=(
struct Cyc_Absyn_Exp*)_tmp490->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBB4,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26D;_LL270: {struct Cyc_Absyn_Upper_b_struct*_tmp529=(struct Cyc_Absyn_Upper_b_struct*)
_tmp527;if(_tmp529->tag != 1)goto _LL26D;else{_tmp52A=_tmp529->f1;}}_LL271: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp524)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp490->hd);struct Cyc_Absyn_Exp*_tmpBB5[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBB5[1]=_tmp52A,((_tmpBB5[0]=(struct Cyc_Absyn_Exp*)_tmp490->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBB5,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp522)){if(!Cyc_Evexp_c_can_eval(_tmp52A)){const
char*_tmpBB8;void*_tmpBB7;(_tmpBB7=0,Cyc_Tcutil_terr(e->loc,((_tmpBB8="can't calculate numelts",
_tag_dyneither(_tmpBB8,sizeof(char),24))),_tag_dyneither(_tmpBB7,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp52A,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp52A->r;goto _LL26D;}}goto _LL26D;_LL26D:;}goto _LL266;_LL26B:;_LL26C: {const
char*_tmpBBD;void*_tmpBBC[2];struct Cyc_String_pa_struct _tmpBBB;struct Cyc_String_pa_struct
_tmpBBA;(_tmpBBA.tag=0,((_tmpBBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBBB.tag=0,((_tmpBBB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBBC[0]=& _tmpBBB,((
_tmpBBC[1]=& _tmpBBA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBD="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBBD,
sizeof(char),41))),_tag_dyneither(_tmpBBC,sizeof(void*),2)))))))))))));}_LL266:;}
break;}case Cyc_Absyn_Plus: _LL265:{void*_tmp533=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp519))->hd);struct Cyc_Absyn_PtrInfo _tmp535;
void*_tmp536;struct Cyc_Absyn_PtrAtts _tmp537;union Cyc_Absyn_Constraint*_tmp538;
union Cyc_Absyn_Constraint*_tmp539;_LL274: {struct Cyc_Absyn_PointerType_struct*
_tmp534=(struct Cyc_Absyn_PointerType_struct*)_tmp533;if(_tmp534->tag != 5)goto
_LL276;else{_tmp535=_tmp534->f1;_tmp536=_tmp535.elt_typ;_tmp537=_tmp535.ptr_atts;
_tmp538=_tmp537.bounds;_tmp539=_tmp537.zero_term;}}_LL275:{void*_tmp53A=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp538);struct Cyc_Absyn_Exp*_tmp53D;_LL279: {struct Cyc_Absyn_DynEither_b_struct*
_tmp53B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp53A;if(_tmp53B->tag != 0)goto
_LL27B;}_LL27A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp490))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp490->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBBE[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBBE[2]=e2,((_tmpBBE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp536),0),((_tmpBBE[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBE,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL278;}_LL27B: {struct Cyc_Absyn_Upper_b_struct*_tmp53C=(struct
Cyc_Absyn_Upper_b_struct*)_tmp53A;if(_tmp53C->tag != 1)goto _LL278;else{_tmp53D=
_tmp53C->f1;}}_LL27C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp539)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp490))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp490->tl))->hd;struct Cyc_Absyn_Exp*_tmpBBF[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBBF[2]=e2,((_tmpBBF[1]=_tmp53D,((_tmpBBF[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBF,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL278;_LL278:;}goto _LL273;_LL276:;_LL277: goto _LL273;
_LL273:;}break;case Cyc_Absyn_Minus: _LL272: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp519))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp490))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp519->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBC0[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBC0[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBC0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBC0[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBC0,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL27D: goto _LL27E;case Cyc_Absyn_Neq: _LL27E: goto _LL27F;case Cyc_Absyn_Gt: _LL27F:
goto _LL280;case Cyc_Absyn_Gte: _LL280: goto _LL281;case Cyc_Absyn_Lt: _LL281: goto
_LL282;case Cyc_Absyn_Lte: _LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp490))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp519))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp519->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL283: break;}goto _LL208;}_LL213: {
struct Cyc_Absyn_Increment_e_struct*_tmp491=(struct Cyc_Absyn_Increment_e_struct*)
_tmp484;if(_tmp491->tag != 5)goto _LL215;else{_tmp492=_tmp491->f1;_tmp493=_tmp491->f2;}}
_LL214: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp492->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBC1;struct _dyneither_ptr incr_str=(_tmpBC1="increment",
_tag_dyneither(_tmpBC1,sizeof(char),10));if(_tmp493 == Cyc_Absyn_PreDec  || 
_tmp493 == Cyc_Absyn_PostDec){const char*_tmpBC2;incr_str=((_tmpBC2="decrement",
_tag_dyneither(_tmpBC2,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp492,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBC6;void*_tmpBC5[1];
struct Cyc_String_pa_struct _tmpBC4;(_tmpBC4.tag=0,((_tmpBC4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBC5[0]=& _tmpBC4,Cyc_Tcutil_terr(
e->loc,((_tmpBC6="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBC6,sizeof(char),74))),_tag_dyneither(_tmpBC5,sizeof(void*),1)))))));}{
const char*_tmpBC9;void*_tmpBC8;(_tmpBC8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC9="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBC9,sizeof(char),30))),_tag_dyneither(_tmpBC8,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp492,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp547=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBCA;_tmp547->topt=((_tmpBCA=_cycalloc(sizeof(*
_tmpBCA)),((_tmpBCA->v=Cyc_Absyn_sint_typ,_tmpBCA))));}switch(_tmp493){case Cyc_Absyn_PreInc:
_LL285:{struct Cyc_Absyn_AssignOp_e_struct _tmpBD0;struct Cyc_Core_Opt*_tmpBCF;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBCE;e->r=(void*)((_tmpBCE=_cycalloc(
sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBD0.tag=4,((_tmpBD0.f1=_tmp492,((_tmpBD0.f2=((
_tmpBCF=_cycalloc_atomic(sizeof(*_tmpBCF)),((_tmpBCF->v=(void*)Cyc_Absyn_Plus,
_tmpBCF)))),((_tmpBD0.f3=_tmp547,_tmpBD0)))))))),_tmpBCE))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL286:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBD6;struct Cyc_Core_Opt*_tmpBD5;struct Cyc_Absyn_AssignOp_e_struct*_tmpBD4;e->r=(
void*)((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD6.tag=4,((
_tmpBD6.f1=_tmp492,((_tmpBD6.f2=((_tmpBD5=_cycalloc_atomic(sizeof(*_tmpBD5)),((
_tmpBD5->v=(void*)Cyc_Absyn_Minus,_tmpBD5)))),((_tmpBD6.f3=_tmp547,_tmpBD6)))))))),
_tmpBD4))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL287:{const char*_tmpBDA;
void*_tmpBD9[1];struct Cyc_String_pa_struct _tmpBD8;(_tmpBD8.tag=0,((_tmpBD8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD9[0]=& _tmpBD8,Cyc_Tcutil_terr(
e->loc,((_tmpBDA="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBDA,sizeof(char),59))),_tag_dyneither(_tmpBD9,sizeof(void*),1)))))));}{
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDD="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBDD,sizeof(char),34))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp492);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp493 == Cyc_Absyn_PostInc
 || _tmp493 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp493 == Cyc_Absyn_PreDec  || _tmp493 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBDE[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBDE[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBDE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBDE[0]=Cyc_Toc_push_address_exp(_tmp492),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDE,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp555=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp492->topt))->v);enum Cyc_Absyn_Size_of _tmp557;int _tmp55A;_LL28A: {
struct Cyc_Absyn_IntType_struct*_tmp556=(struct Cyc_Absyn_IntType_struct*)_tmp555;
if(_tmp556->tag != 6)goto _LL28C;else{_tmp557=_tmp556->f2;}}_LL28B: switch(_tmp557){
case Cyc_Absyn_Char_sz: _LL294: fn_e=_tmp493 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL295: fn_e=
_tmp493 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL296: fn_e=_tmp493 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL297: {struct Cyc_Core_Impossible_struct
_tmpBE4;const char*_tmpBE3;struct Cyc_Core_Impossible_struct*_tmpBE2;(int)_throw((
void*)((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE4.tag=Cyc_Core_Impossible,((
_tmpBE4.f1=((_tmpBE3="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBE3,sizeof(char),44))),_tmpBE4)))),_tmpBE2)))));}}goto _LL289;
_LL28C: {struct Cyc_Absyn_FloatType_struct*_tmp558=(struct Cyc_Absyn_FloatType_struct*)
_tmp555;if(_tmp558->tag != 7)goto _LL28E;}_LL28D: fn_e=_tmp493 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL289;_LL28E: {struct Cyc_Absyn_DoubleType_struct*_tmp559=(struct Cyc_Absyn_DoubleType_struct*)
_tmp555;if(_tmp559->tag != 8)goto _LL290;else{_tmp55A=_tmp559->f1;}}_LL28F: switch(
_tmp55A){case 1: _LL299: fn_e=_tmp493 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL29A: fn_e=_tmp493 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL289;_LL290: {struct Cyc_Absyn_PointerType_struct*_tmp55B=(struct Cyc_Absyn_PointerType_struct*)
_tmp555;if(_tmp55B->tag != 5)goto _LL292;}_LL291: fn_e=_tmp493 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL289;_LL292:;_LL293: {struct Cyc_Core_Impossible_struct _tmpBEA;const char*
_tmpBE9;struct Cyc_Core_Impossible_struct*_tmpBE8;(int)_throw((void*)((_tmpBE8=
_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBEA.tag=Cyc_Core_Impossible,((
_tmpBEA.f1=((_tmpBE9="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBE9,sizeof(char),64))),_tmpBEA)))),_tmpBE8)))));}_LL289:;}{
struct Cyc_Absyn_Exp*_tmpBEB[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBEB[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBEB[0]=_tmp492,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBEB,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
== (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp492)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp492,0,Cyc_Toc_incr_lvalue,_tmp493);e->r=_tmp492->r;}}}goto _LL208;};};}_LL215: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp494=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp484;if(_tmp494->tag != 4)goto _LL217;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;
_tmp497=_tmp494->f3;}}_LL216: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp495->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp497->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp495,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp495);Cyc_Toc_exp_to_c(nv,
_tmp497);e->r=Cyc_Toc_tagged_union_assignop(_tmp495,e1_old_typ,_tmp496,_tmp497,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp495,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp495,_tmp496,_tmp497,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp495);Cyc_Toc_exp_to_c(nv,_tmp495);Cyc_Toc_exp_to_c(nv,_tmp497);{int done=0;
if(_tmp496 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp496->v){case Cyc_Absyn_Plus: _LL29C: change=_tmp497;break;case Cyc_Absyn_Minus:
_LL29D: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp497,0);break;default:
_LL29E: {const char*_tmpBEE;void*_tmpBED;(_tmpBED=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEE="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBEE,sizeof(char),27))),_tag_dyneither(_tmpBED,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp566=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBEF[3];e->r=Cyc_Toc_fncall_exp_r(_tmp566,((_tmpBEF[2]=change,((
_tmpBEF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBEF[0]=Cyc_Toc_push_address_exp(
_tmp495),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBEF,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp496->v){case Cyc_Absyn_Plus:
_LL2A0: done=1;{struct Cyc_Absyn_Exp*_tmpBF0[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp495),((_tmpBF0[1]=_tmp497,((
_tmpBF0[0]=_tmp495,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF0,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A1: {const char*_tmpBF3;void*_tmpBF2;(_tmpBF2=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF3="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBF3,sizeof(char),39))),_tag_dyneither(_tmpBF2,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp497->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp497->r,0));if(!Cyc_Absyn_is_lvalue(_tmp495)){{struct _tuple18 _tmpBF6;struct
_tuple18*_tmpBF5;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp495,0,Cyc_Toc_assignop_lvalue,((_tmpBF5=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBF5[0]=((_tmpBF6.f1=_tmp496,((_tmpBF6.f2=
_tmp497,_tmpBF6)))),_tmpBF5)))));}e->r=_tmp495->r;}}goto _LL208;};};};}_LL217: {
struct Cyc_Absyn_Conditional_e_struct*_tmp498=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp484;if(_tmp498->tag != 6)goto _LL219;else{_tmp499=_tmp498->f1;_tmp49A=_tmp498->f2;
_tmp49B=_tmp498->f3;}}_LL218: Cyc_Toc_exp_to_c(nv,_tmp499);Cyc_Toc_exp_to_c(nv,
_tmp49A);Cyc_Toc_exp_to_c(nv,_tmp49B);goto _LL208;_LL219: {struct Cyc_Absyn_And_e_struct*
_tmp49C=(struct Cyc_Absyn_And_e_struct*)_tmp484;if(_tmp49C->tag != 7)goto _LL21B;
else{_tmp49D=_tmp49C->f1;_tmp49E=_tmp49C->f2;}}_LL21A: Cyc_Toc_exp_to_c(nv,
_tmp49D);Cyc_Toc_exp_to_c(nv,_tmp49E);goto _LL208;_LL21B: {struct Cyc_Absyn_Or_e_struct*
_tmp49F=(struct Cyc_Absyn_Or_e_struct*)_tmp484;if(_tmp49F->tag != 8)goto _LL21D;
else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}}_LL21C: Cyc_Toc_exp_to_c(nv,
_tmp4A0);Cyc_Toc_exp_to_c(nv,_tmp4A1);goto _LL208;_LL21D: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4A2=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp484;if(_tmp4A2->tag != 9)goto _LL21F;
else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}}_LL21E: Cyc_Toc_exp_to_c(nv,
_tmp4A3);Cyc_Toc_exp_to_c(nv,_tmp4A4);goto _LL208;_LL21F: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4A5=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp484;if(_tmp4A5->tag != 10)goto
_LL221;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;}}_LL220: _tmp4A9=_tmp4A6;
_tmp4AA=_tmp4A7;goto _LL222;_LL221: {struct Cyc_Absyn_FnCall_e_struct*_tmp4A8=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp484;if(_tmp4A8->tag != 11)goto _LL223;else{
_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;_tmp4AB=_tmp4A8->f3;if(_tmp4AB != 0)goto
_LL223;}}_LL222: Cyc_Toc_exp_to_c(nv,_tmp4A9);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4AA);goto _LL208;_LL223: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4AC=(struct Cyc_Absyn_FnCall_e_struct*)_tmp484;if(_tmp4AC->tag != 11)goto
_LL225;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AC->f2;_tmp4AF=_tmp4AC->f3;if(
_tmp4AF == 0)goto _LL225;_tmp4B0=*_tmp4AF;_tmp4B1=_tmp4B0.num_varargs;_tmp4B2=
_tmp4B0.injectors;_tmp4B3=_tmp4B0.vai;}}_LL224: {struct Cyc_Toc_Env _tmp56E;struct
_RegionHandle*_tmp56F;struct Cyc_Toc_Env*_tmp56D=nv;_tmp56E=*_tmp56D;_tmp56F=
_tmp56E.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4B1,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4B3->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4AE);int num_normargs=num_args - _tmp4B1;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4AE=_tmp4AE->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AE))->hd);{
struct Cyc_List_List*_tmpBF7;new_args=((_tmpBF7=_cycalloc(sizeof(*_tmpBF7)),((
_tmpBF7->hd=(struct Cyc_Absyn_Exp*)_tmp4AE->hd,((_tmpBF7->tl=new_args,_tmpBF7))))));};}}{
struct Cyc_Absyn_Exp*_tmpBFA[3];struct Cyc_List_List*_tmpBF9;new_args=((_tmpBF9=
_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBFA[2]=num_varargs_exp,((_tmpBFA[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBFA[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBF9->tl=
new_args,_tmpBF9))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4AD);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AD,new_args,0),0);if(_tmp4B3->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp573=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4B3->type));struct Cyc_Absyn_DatatypeInfo _tmp575;union Cyc_Absyn_DatatypeInfoU
_tmp576;struct Cyc_Absyn_Datatypedecl**_tmp577;struct Cyc_Absyn_Datatypedecl*
_tmp578;_LL2A4: {struct Cyc_Absyn_DatatypeType_struct*_tmp574=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp573;if(_tmp574->tag != 3)goto _LL2A6;else{_tmp575=_tmp574->f1;_tmp576=_tmp575.datatype_info;
if((_tmp576.KnownDatatype).tag != 2)goto _LL2A6;_tmp577=(struct Cyc_Absyn_Datatypedecl**)(
_tmp576.KnownDatatype).val;_tmp578=*_tmp577;}}_LL2A5: tud=_tmp578;goto _LL2A3;
_LL2A6:;_LL2A7: {const char*_tmpBFD;void*_tmpBFC;(_tmpBFC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFD="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBFD,sizeof(char),44))),_tag_dyneither(_tmpBFC,sizeof(void*),0)));}
_LL2A3:;}{unsigned int _tmpC02;unsigned int _tmpC01;struct _dyneither_ptr _tmpC00;
struct _tuple0**_tmpBFF;unsigned int _tmpBFE;struct _dyneither_ptr vs=(_tmpBFE=(
unsigned int)_tmp4B1,((_tmpBFF=(struct _tuple0**)_region_malloc(_tmp56F,
_check_times(sizeof(struct _tuple0*),_tmpBFE)),((_tmpC00=_tag_dyneither(_tmpBFF,
sizeof(struct _tuple0*),_tmpBFE),((((_tmpC01=_tmpBFE,_tmpC03(& _tmpC02,& _tmpC01,&
_tmpBFF))),_tmpC00)))))));if(_tmp4B1 != 0){struct Cyc_List_List*_tmp57B=0;{int i=
_tmp4B1 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC04;_tmp57B=((_tmpC04=
_cycalloc(sizeof(*_tmpC04)),((_tmpC04->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC04->tl=_tmp57B,_tmpC04))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp57B,0),s,0);{
int i=0;for(0;_tmp4AE != 0;(((_tmp4AE=_tmp4AE->tl,_tmp4B2=_tmp4B2->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4AE->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp57E;struct
_tuple0*_tmp57F;struct Cyc_List_List*_tmp580;struct Cyc_Absyn_Datatypefield*
_tmp57D=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4B2))->hd;_tmp57E=*_tmp57D;_tmp57F=_tmp57E.name;_tmp580=_tmp57E.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp580))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp57F),0),s,0);{const char*_tmpC05;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp57F,((_tmpC05="_struct",_tag_dyneither(_tmpC05,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC06[3];struct Cyc_List_List*
_tmp582=(_tmpC06[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC06[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC06[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC06,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4AE
!= 0;(_tmp4AE=_tmp4AE->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AE->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AE->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL208;}_LL225: {struct Cyc_Absyn_Throw_e_struct*_tmp4B4=(struct Cyc_Absyn_Throw_e_struct*)
_tmp484;if(_tmp4B4->tag != 12)goto _LL227;else{_tmp4B5=_tmp4B4->f1;}}_LL226: Cyc_Toc_exp_to_c(
nv,_tmp4B5);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4B5),0))->r;goto _LL208;_LL227: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4B6=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp484;if(_tmp4B6->tag != 13)
goto _LL229;else{_tmp4B7=_tmp4B6->f1;}}_LL228: Cyc_Toc_exp_to_c(nv,_tmp4B7);goto
_LL208;_LL229: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4B8=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp484;if(_tmp4B8->tag != 14)goto _LL22B;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;}}
_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B9);for(0;_tmp4BA != 0;_tmp4BA=_tmp4BA->tl){enum 
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4BA->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp588=Cyc_Tcutil_compress((void*)_tmp4BA->hd);_LL2A9: {struct Cyc_Absyn_VarType_struct*
_tmp589=(struct Cyc_Absyn_VarType_struct*)_tmp588;if(_tmp589->tag != 2)goto _LL2AB;}
_LL2AA: goto _LL2AC;_LL2AB: {struct Cyc_Absyn_DatatypeType_struct*_tmp58A=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp588;if(_tmp58A->tag != 3)goto _LL2AD;}_LL2AC:
continue;_LL2AD:;_LL2AE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4B9,0))->r;goto _LL2A8;_LL2A8:;}
break;}}goto _LL208;_LL22B: {struct Cyc_Absyn_Cast_e_struct*_tmp4BB=(struct Cyc_Absyn_Cast_e_struct*)
_tmp484;if(_tmp4BB->tag != 15)goto _LL22D;else{_tmp4BC=(void**)& _tmp4BB->f1;
_tmp4BD=(void**)((void**)& _tmp4BB->f1);_tmp4BE=_tmp4BB->f2;_tmp4BF=_tmp4BB->f3;
_tmp4C0=_tmp4BB->f4;}}_LL22C: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BE->topt))->v;void*new_typ=*_tmp4BD;*_tmp4BD=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BE);{struct _tuple17 _tmpC07;struct _tuple17
_tmp58C=(_tmpC07.f1=Cyc_Tcutil_compress(old_t2),((_tmpC07.f2=Cyc_Tcutil_compress(
new_typ),_tmpC07)));void*_tmp58D;struct Cyc_Absyn_PtrInfo _tmp58F;void*_tmp590;
struct Cyc_Absyn_PtrInfo _tmp592;void*_tmp593;struct Cyc_Absyn_PtrInfo _tmp595;void*
_tmp596;_LL2B0: _tmp58D=_tmp58C.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58E=(
struct Cyc_Absyn_PointerType_struct*)_tmp58D;if(_tmp58E->tag != 5)goto _LL2B2;else{
_tmp58F=_tmp58E->f1;}};_tmp590=_tmp58C.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp591=(struct Cyc_Absyn_PointerType_struct*)_tmp590;if(_tmp591->tag != 5)goto
_LL2B2;else{_tmp592=_tmp591->f1;}};_LL2B1: {int _tmp598=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp58F.ptr_atts).nullable);int _tmp599=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp592.ptr_atts).nullable);
void*_tmp59A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp58F.ptr_atts).bounds);void*_tmp59B=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp592.ptr_atts).bounds);
int _tmp59C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58F.ptr_atts).zero_term);int _tmp59D=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp592.ptr_atts).zero_term);{struct _tuple17 _tmpC08;
struct _tuple17 _tmp59F=(_tmpC08.f1=_tmp59A,((_tmpC08.f2=_tmp59B,_tmpC08)));void*
_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A2;void*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;
void*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A8;void*_tmp5A9;void*_tmp5AB;void*_tmp5AD;
struct Cyc_Absyn_Exp*_tmp5AF;void*_tmp5B0;void*_tmp5B2;_LL2B7: _tmp5A0=_tmp59F.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A1=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A0;
if(_tmp5A1->tag != 1)goto _LL2B9;else{_tmp5A2=_tmp5A1->f1;}};_tmp5A3=_tmp59F.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A4=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A3;
if(_tmp5A4->tag != 1)goto _LL2B9;else{_tmp5A5=_tmp5A4->f1;}};_LL2B8: if((!Cyc_Evexp_c_can_eval(
_tmp5A2) || !Cyc_Evexp_c_can_eval(_tmp5A5)) && !Cyc_Evexp_same_const_exp(_tmp5A2,
_tmp5A5)){const char*_tmpC0B;void*_tmpC0A;(_tmpC0A=0,Cyc_Tcutil_terr(e->loc,((
_tmpC0B="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC0B,sizeof(char),54))),_tag_dyneither(_tmpC0A,sizeof(void*),0)));}if(_tmp598
 && !_tmp599){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC0E;void*_tmpC0D;(
_tmpC0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC0E="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC0E,
sizeof(char),44))),_tag_dyneither(_tmpC0D,sizeof(void*),0)));}if(_tmp4C0 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC12;void*_tmpC11[1];struct Cyc_String_pa_struct _tmpC10;(_tmpC10.tag=
0,((_tmpC10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC11[0]=& _tmpC10,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC12="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC12,sizeof(char),41))),_tag_dyneither(_tmpC11,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4BE);if(do_null_check){if(!_tmp4BF){
const char*_tmpC15;void*_tmpC14;(_tmpC14=0,Cyc_Tcutil_warn(e->loc,((_tmpC15="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC15,sizeof(char),58))),_tag_dyneither(_tmpC14,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC16;e->r=Cyc_Toc_cast_it_r(*_tmp4BD,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=
_tmp4BE,((_tmpC16->tl=0,_tmpC16)))))),0));};}};}goto _LL2B6;_LL2B9: _tmp5A6=
_tmp59F.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5A7=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5A6;if(_tmp5A7->tag != 1)goto _LL2BB;else{_tmp5A8=_tmp5A7->f1;}};_tmp5A9=
_tmp59F.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A9;if(_tmp5AA->tag != 0)goto _LL2BB;};_LL2BA: if(!Cyc_Evexp_c_can_eval(_tmp5A8)){
const char*_tmpC19;void*_tmpC18;(_tmpC18=0,Cyc_Tcutil_terr(e->loc,((_tmpC19="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC19,sizeof(char),71))),_tag_dyneither(_tmpC18,sizeof(void*),0)));}
if(_tmp4C0 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC1D;void*_tmpC1C[1];struct
Cyc_String_pa_struct _tmpC1B;(_tmpC1B.tag=0,((_tmpC1B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC1C[0]=& _tmpC1B,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC1D="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC1D,sizeof(char),44))),_tag_dyneither(_tmpC1C,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp59C  && !(_tmp592.elt_tq).real_const) && !
_tmp59D)_tmp5A8=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5A8,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5A8,_tmp4BE))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Toc__tag_dyneither_e;if(_tmp59C){struct _tuple0*
_tmp5C4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C5=Cyc_Absyn_var_exp(_tmp5C4,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5C6=_tmp4BE->r;union Cyc_Absyn_Cnst _tmp5C8;
struct _dyneither_ptr _tmp5C9;_LL2C0: {struct Cyc_Absyn_Const_e_struct*_tmp5C7=(
struct Cyc_Absyn_Const_e_struct*)_tmp5C6;if(_tmp5C7->tag != 0)goto _LL2C2;else{
_tmp5C8=_tmp5C7->f1;if((_tmp5C8.String_c).tag != 7)goto _LL2C2;_tmp5C9=(struct
_dyneither_ptr)(_tmp5C8.String_c).val;}}_LL2C1: arg3=_tmp5A8;goto _LL2BF;_LL2C2:;
_LL2C3:{struct Cyc_Absyn_Exp*_tmpC1E[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4BE),((_tmpC1E[1]=_tmp5A8,((_tmpC1E[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5C5),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1E,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2BF;_LL2BF:;}if(!_tmp59D  && !(_tmp592.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5CB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp592.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC1F[3];struct Cyc_Absyn_Exp*_tmp5CC=Cyc_Absyn_fncall_exp(_tmp5C3,((_tmpC1F[2]=
arg3,((_tmpC1F[1]=_tmp5CB,((_tmpC1F[0]=_tmp5C5,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5CD=Cyc_Absyn_exp_stmt(_tmp5CC,0);_tmp5CD=
Cyc_Absyn_declare_stmt(_tmp5C4,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4BE,_tmp5CD,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CD);};}else{struct Cyc_Absyn_Exp*
_tmpC20[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C3,((_tmpC20[2]=_tmp5A8,((_tmpC20[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp592.elt_typ),0),((_tmpC20[0]=
_tmp4BE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC20,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B6;
_LL2BB: _tmp5AB=_tmp59F.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5AB;if(_tmp5AC->tag != 0)goto _LL2BD;};_tmp5AD=_tmp59F.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AE=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AD;if(_tmp5AE->tag != 1)goto _LL2BD;
else{_tmp5AF=_tmp5AE->f1;}};_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5AF)){const char*
_tmpC23;void*_tmpC22;(_tmpC22=0,Cyc_Tcutil_terr(e->loc,((_tmpC23="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC23,sizeof(char),71))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC26="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC26,sizeof(char),45))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D4=_tmp5AF;if(_tmp59C  && !_tmp59D)_tmp5D4=Cyc_Absyn_add_exp(
_tmp5AF,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC27[3];struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Absyn_fncall_exp(
_tmp5D5,((_tmpC27[2]=_tmp5D4,((_tmpC27[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58F.elt_typ),0),((_tmpC27[0]=_tmp4BE,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC27,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp599){struct Cyc_List_List*_tmpC28;_tmp5D6->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=Cyc_Absyn_copy_exp(
_tmp5D6),((_tmpC28->tl=0,_tmpC28)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4BD,_tmp5D6);
goto _LL2B6;};};_LL2BD: _tmp5B0=_tmp59F.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5B1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto
_LL2B6;};_tmp5B2=_tmp59F.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B3=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5B2;if(_tmp5B3->tag != 0)goto _LL2B6;};_LL2BE:
DynCast: if((_tmp59C  && !_tmp59D) && !(_tmp592.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC2B="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC2B,sizeof(char),70))),_tag_dyneither(_tmpC2A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5DB=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC2C[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5DB,((_tmpC2C[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC2C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58F.elt_typ),0),((
_tmpC2C[0]=_tmp4BE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2B6;
_LL2B6:;}goto _LL2AF;}_LL2B2: _tmp593=_tmp58C.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp594=(struct Cyc_Absyn_PointerType_struct*)_tmp593;if(_tmp594->tag != 5)goto
_LL2B4;else{_tmp595=_tmp594->f1;}};_tmp596=_tmp58C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp597=(struct Cyc_Absyn_IntType_struct*)_tmp596;if(_tmp597->tag != 6)goto _LL2B4;};
_LL2B3:{void*_tmp5DD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp595.ptr_atts).bounds);_LL2C5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5DE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5DD;if(_tmp5DE->tag != 0)goto
_LL2C7;}_LL2C6: _tmp4BE->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BE->r,
_tmp4BE->loc),Cyc_Toc_curr_sp);goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}
goto _LL2AF;_LL2B4:;_LL2B5: goto _LL2AF;_LL2AF:;}goto _LL208;}_LL22D: {struct Cyc_Absyn_Address_e_struct*
_tmp4C1=(struct Cyc_Absyn_Address_e_struct*)_tmp484;if(_tmp4C1->tag != 16)goto
_LL22F;else{_tmp4C2=_tmp4C1->f1;}}_LL22E:{void*_tmp5DF=_tmp4C2->r;struct _tuple0*
_tmp5E1;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp5E3;struct Cyc_List_List*
_tmp5E5;_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5E0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5DF;if(_tmp5E0->tag != 29)goto _LL2CC;else{_tmp5E1=_tmp5E0->f1;_tmp5E2=_tmp5E0->f2;
_tmp5E3=_tmp5E0->f3;}}_LL2CB: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC30;void*
_tmpC2F[1];struct Cyc_String_pa_struct _tmpC2E;(_tmpC2E.tag=0,((_tmpC2E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4C2->loc)),((
_tmpC2F[0]=& _tmpC2E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC30="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC30,
sizeof(char),42))),_tag_dyneither(_tmpC2F,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C2->topt))->v,_tmp5E2 != 0,1,0,
_tmp5E3,_tmp5E1))->r;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E4=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DF;if(_tmp5E4->tag != 25)goto _LL2CE;else{
_tmp5E5=_tmp5E4->f1;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC34;void*
_tmpC33[1];struct Cyc_String_pa_struct _tmpC32;(_tmpC32.tag=0,((_tmpC32.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4C2->loc)),((
_tmpC33[0]=& _tmpC32,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC34="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC34,
sizeof(char),42))),_tag_dyneither(_tmpC33,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E5))->r;goto _LL2C9;_LL2CE:;_LL2CF: Cyc_Toc_exp_to_c(nv,_tmp4C2);if(!
Cyc_Absyn_is_lvalue(_tmp4C2)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4C2,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4C2);}goto _LL2C9;
_LL2C9:;}goto _LL208;_LL22F: {struct Cyc_Absyn_New_e_struct*_tmp4C3=(struct Cyc_Absyn_New_e_struct*)
_tmp484;if(_tmp4C3->tag != 17)goto _LL231;else{_tmp4C4=_tmp4C3->f1;_tmp4C5=_tmp4C3->f2;}}
_LL230: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC38;void*_tmpC37[1];struct Cyc_String_pa_struct
_tmpC36;(_tmpC36.tag=0,((_tmpC36.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C5->loc)),((_tmpC37[0]=& _tmpC36,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC38="%s: new at top-level",
_tag_dyneither(_tmpC38,sizeof(char),21))),_tag_dyneither(_tmpC37,sizeof(void*),1)))))));}{
void*_tmp5EF=_tmp4C5->r;struct Cyc_List_List*_tmp5F1;struct Cyc_Absyn_Vardecl*
_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F5;int _tmp5F6;
struct _tuple0*_tmp5F8;struct Cyc_List_List*_tmp5F9;struct Cyc_List_List*_tmp5FA;
struct Cyc_Absyn_Aggrdecl*_tmp5FB;struct Cyc_List_List*_tmp5FD;_LL2D1: {struct Cyc_Absyn_Array_e_struct*
_tmp5F0=(struct Cyc_Absyn_Array_e_struct*)_tmp5EF;if(_tmp5F0->tag != 27)goto _LL2D3;
else{_tmp5F1=_tmp5F0->f1;}}_LL2D2: {struct _tuple0*_tmp5FE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_Absyn_var_exp(_tmp5FE,0);struct Cyc_Absyn_Stmt*
_tmp600=Cyc_Toc_init_array(nv,_tmp5FF,_tmp5F1,Cyc_Absyn_exp_stmt(_tmp5FF,0));
void*old_elt_typ;{void*_tmp601=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp603;void*_tmp604;struct Cyc_Absyn_Tqual _tmp605;struct Cyc_Absyn_PtrAtts _tmp606;
union Cyc_Absyn_Constraint*_tmp607;_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp602=(struct Cyc_Absyn_PointerType_struct*)_tmp601;if(_tmp602->tag != 5)goto
_LL2DE;else{_tmp603=_tmp602->f1;_tmp604=_tmp603.elt_typ;_tmp605=_tmp603.elt_tq;
_tmp606=_tmp603.ptr_atts;_tmp607=_tmp606.zero_term;}}_LL2DD: old_elt_typ=_tmp604;
goto _LL2DB;_LL2DE:;_LL2DF: {const char*_tmpC3B;void*_tmpC3A;old_elt_typ=((_tmpC3A=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC3B="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC3B,sizeof(char),52))),_tag_dyneither(_tmpC3A,sizeof(void*),0))));}_LL2DB:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp60A=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp60B=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5F1),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C4 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp60B);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C4;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp60B);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FE,_tmp60A,(struct Cyc_Absyn_Exp*)e1,_tmp600,0));goto
_LL2D0;};}_LL2D3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F2=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5EF;if(_tmp5F2->tag != 28)goto _LL2D5;else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;
_tmp5F5=_tmp5F2->f3;_tmp5F6=_tmp5F2->f4;}}_LL2D4: {int is_dyneither_ptr=0;{void*
_tmp60C=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp60E;void*_tmp60F;
struct Cyc_Absyn_Tqual _tmp610;struct Cyc_Absyn_PtrAtts _tmp611;union Cyc_Absyn_Constraint*
_tmp612;union Cyc_Absyn_Constraint*_tmp613;_LL2E1: {struct Cyc_Absyn_PointerType_struct*
_tmp60D=(struct Cyc_Absyn_PointerType_struct*)_tmp60C;if(_tmp60D->tag != 5)goto
_LL2E3;else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60E.elt_typ;_tmp610=_tmp60E.elt_tq;
_tmp611=_tmp60E.ptr_atts;_tmp612=_tmp611.bounds;_tmp613=_tmp611.zero_term;}}
_LL2E2: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp612)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E0;
_LL2E3:;_LL2E4: {const char*_tmpC3E;void*_tmpC3D;(_tmpC3D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3E="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC3E,sizeof(char),42))),_tag_dyneither(_tmpC3D,sizeof(void*),0)));}
_LL2E0:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F5->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F4);{struct Cyc_Absyn_Exp*_tmp616=
Cyc_Absyn_var_exp(max,0);if(_tmp5F6)_tmp616=Cyc_Absyn_add_exp(_tmp616,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F3,Cyc_Absyn_var_exp(max,0),_tmp5F5,_tmp5F6,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp618;struct _RegionHandle*_tmp619;struct Cyc_Toc_Env*_tmp617=
nv;_tmp618=*_tmp617;_tmp619=_tmp618.rgn;{struct _tuple19*_tmpC41;struct Cyc_List_List*
_tmpC40;struct Cyc_List_List*decls=(_tmpC40=_region_malloc(_tmp619,sizeof(*
_tmpC40)),((_tmpC40->hd=((_tmpC41=_region_malloc(_tmp619,sizeof(*_tmpC41)),((
_tmpC41->f1=max,((_tmpC41->f2=Cyc_Absyn_uint_typ,((_tmpC41->f3=(struct Cyc_Absyn_Exp*)
_tmp5F4,_tmpC41)))))))),((_tmpC40->tl=0,_tmpC40)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4C4 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC42[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC42[1]=_tmp616,((
_tmpC42[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C4;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC43[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC43[1]=_tmp616,((_tmpC43[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC43,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC46;struct Cyc_List_List*
_tmpC45;decls=((_tmpC45=_region_malloc(_tmp619,sizeof(*_tmpC45)),((_tmpC45->hd=((
_tmpC46=_region_malloc(_tmp619,sizeof(*_tmpC46)),((_tmpC46->f1=a,((_tmpC46->f2=
ptr_typ,((_tmpC46->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC46)))))))),((_tmpC45->tl=
decls,_tmpC45))))));}if(is_dyneither_ptr){struct _tuple0*_tmp61E=Cyc_Toc_temp_var();
void*_tmp61F=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp620=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC47[3];struct Cyc_Absyn_Exp*_tmp621=Cyc_Absyn_fncall_exp(
_tmp620,((_tmpC47[2]=_tmp616,((_tmpC47[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC47[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC47,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC4A;struct Cyc_List_List*_tmpC49;decls=((_tmpC49=
_region_malloc(_tmp619,sizeof(*_tmpC49)),((_tmpC49->hd=((_tmpC4A=_region_malloc(
_tmp619,sizeof(*_tmpC4A)),((_tmpC4A->f1=_tmp61E,((_tmpC4A->f2=_tmp61F,((_tmpC4A->f3=(
struct Cyc_Absyn_Exp*)_tmp621,_tmpC4A)))))))),((_tmpC49->tl=decls,_tmpC49))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp61E,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp625=decls;for(0;_tmp625 != 0;_tmp625=_tmp625->tl){struct
_tuple0*_tmp627;void*_tmp628;struct Cyc_Absyn_Exp*_tmp629;struct _tuple19 _tmp626=*((
struct _tuple19*)_tmp625->hd);_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;_tmp629=
_tmp626.f3;s=Cyc_Absyn_declare_stmt(_tmp627,_tmp628,_tmp629,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D0;};};};}_LL2D5: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F7=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5EF;if(_tmp5F7->tag != 29)goto _LL2D7;
else{_tmp5F8=_tmp5F7->f1;_tmp5F9=_tmp5F7->f2;_tmp5FA=_tmp5F7->f3;_tmp5FB=_tmp5F7->f4;}}
_LL2D6: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4C5->topt))->v,_tmp5F9 != 0,1,_tmp4C4,_tmp5FA,_tmp5F8))->r;goto _LL2D0;_LL2D7: {
struct Cyc_Absyn_Tuple_e_struct*_tmp5FC=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5EF;
if(_tmp5FC->tag != 25)goto _LL2D9;else{_tmp5FD=_tmp5FC->f1;}}_LL2D8: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4C4,_tmp5FD))->r;goto _LL2D0;_LL2D9:;_LL2DA: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C5->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp62C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp62D=Cyc_Absyn_var_exp(_tmp62C,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp62D,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4C4
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C4;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp62E=_tmp4C5->r;void*_tmp630;
struct Cyc_Absyn_Exp*_tmp631;_LL2E6: {struct Cyc_Absyn_Cast_e_struct*_tmp62F=(
struct Cyc_Absyn_Cast_e_struct*)_tmp62E;if(_tmp62F->tag != 15)goto _LL2E8;else{
_tmp630=(void*)_tmp62F->f1;_tmp631=_tmp62F->f2;}}_LL2E7:{struct _tuple17 _tmpC4B;
struct _tuple17 _tmp633=(_tmpC4B.f1=Cyc_Tcutil_compress(_tmp630),((_tmpC4B.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp631->topt))->v),_tmpC4B)));void*
_tmp634;struct Cyc_Absyn_PtrInfo _tmp636;void*_tmp637;struct Cyc_Absyn_PtrAtts
_tmp638;union Cyc_Absyn_Constraint*_tmp639;void*_tmp63A;struct Cyc_Absyn_PtrInfo
_tmp63C;struct Cyc_Absyn_PtrAtts _tmp63D;union Cyc_Absyn_Constraint*_tmp63E;_LL2EB:
_tmp634=_tmp633.f1;{struct Cyc_Absyn_PointerType_struct*_tmp635=(struct Cyc_Absyn_PointerType_struct*)
_tmp634;if(_tmp635->tag != 5)goto _LL2ED;else{_tmp636=_tmp635->f1;_tmp637=_tmp636.elt_typ;
_tmp638=_tmp636.ptr_atts;_tmp639=_tmp638.bounds;}};_tmp63A=_tmp633.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp63B=(struct Cyc_Absyn_PointerType_struct*)_tmp63A;if(_tmp63B->tag != 5)goto
_LL2ED;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63C.ptr_atts;_tmp63E=_tmp63D.bounds;}};
_LL2EC:{struct _tuple17 _tmpC4C;struct _tuple17 _tmp640=(_tmpC4C.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp639),((_tmpC4C.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63E),_tmpC4C)));void*_tmp641;void*_tmp643;struct Cyc_Absyn_Exp*
_tmp645;_LL2F0: _tmp641=_tmp640.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp642=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp641;if(_tmp642->tag != 0)goto _LL2F2;};
_tmp643=_tmp640.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp644=(struct Cyc_Absyn_Upper_b_struct*)
_tmp643;if(_tmp644->tag != 1)goto _LL2F2;else{_tmp645=_tmp644->f1;}};_LL2F1: Cyc_Toc_exp_to_c(
nv,_tmp631);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp646=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC4D[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp646,((_tmpC4D[3]=_tmp645,((_tmpC4D[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp637),0),((_tmpC4D[1]=_tmp631,((_tmpC4D[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4D,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2EF;};_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}goto _LL2EA;_LL2ED:;
_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2E5;_LL2E8:;_LL2E9: goto _LL2E5;_LL2E5:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp648=Cyc_Absyn_exp_stmt(_tmp62D,0);struct Cyc_Absyn_Exp*
_tmp649=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C5);_tmp648=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62D,_tmp649,0),_tmp4C5,0),
_tmp648,0);{void*_tmp64A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp62C,_tmp64A,(struct Cyc_Absyn_Exp*)mexp,_tmp648,0));};}
goto _LL2D0;};}_LL2D0:;}goto _LL208;_LL231: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C6=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp484;if(_tmp4C6->tag != 19)goto
_LL233;else{_tmp4C7=_tmp4C6->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C7);goto _LL208;
_LL233: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C8=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp484;if(_tmp4C8->tag != 18)goto _LL235;else{_tmp4C9=(void*)_tmp4C8->f1;}}_LL234:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC50;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC4F;e->r=(void*)((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=
18,((_tmpC50.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C9),_tmpC50)))),_tmpC4F))));}
goto _LL208;_LL235: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4CA=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp484;if(_tmp4CA->tag != 20)goto _LL237;else{_tmp4CB=(void*)_tmp4CA->f1;_tmp4CC=(
void*)_tmp4CA->f2;{struct Cyc_Absyn_StructField_struct*_tmp4CD=(struct Cyc_Absyn_StructField_struct*)
_tmp4CC;if(_tmp4CD->tag != 0)goto _LL237;else{_tmp4CE=_tmp4CD->f1;}};}}_LL236:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC5A;struct Cyc_Absyn_StructField_struct
_tmpC59;struct Cyc_Absyn_StructField_struct*_tmpC58;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC57;e->r=(void*)((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC5A.tag=
20,((_tmpC5A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CB),((_tmpC5A.f2=(void*)((
void*)((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=0,((
_tmpC59.f1=_tmp4CE,_tmpC59)))),_tmpC58))))),_tmpC5A)))))),_tmpC57))));}goto
_LL208;_LL237: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4CF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp484;if(_tmp4CF->tag != 20)goto _LL239;else{_tmp4D0=(void*)_tmp4CF->f1;_tmp4D1=(
void*)_tmp4CF->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4D2=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4D1;if(_tmp4D2->tag != 1)goto _LL239;else{_tmp4D3=_tmp4D2->f1;}};}}_LL238:{
void*_tmp651=Cyc_Tcutil_compress(_tmp4D0);struct Cyc_Absyn_AggrInfo _tmp653;union
Cyc_Absyn_AggrInfoU _tmp654;struct Cyc_List_List*_tmp656;_LL2F5: {struct Cyc_Absyn_AggrType_struct*
_tmp652=(struct Cyc_Absyn_AggrType_struct*)_tmp651;if(_tmp652->tag != 12)goto
_LL2F7;else{_tmp653=_tmp652->f1;_tmp654=_tmp653.aggr_info;}}_LL2F6: {struct Cyc_Absyn_Aggrdecl*
_tmp659=Cyc_Absyn_get_known_aggrdecl(_tmp654);if(_tmp659->impl == 0){const char*
_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="struct fields must be known",
_tag_dyneither(_tmpC5D,sizeof(char),28))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}
_tmp656=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp659->impl))->fields;goto
_LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp655=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp651;if(_tmp655->tag != 13)goto _LL2F9;else{_tmp656=_tmp655->f2;}}_LL2F8: {
struct Cyc_Absyn_Aggrfield*_tmp65C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp656,(int)_tmp4D3);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC67;struct Cyc_Absyn_StructField_struct _tmpC66;struct Cyc_Absyn_StructField_struct*
_tmpC65;struct Cyc_Absyn_Offsetof_e_struct*_tmpC64;e->r=(void*)((_tmpC64=
_cycalloc(sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC67.tag=20,((_tmpC67.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4D0),((_tmpC67.f2=(void*)((void*)((_tmpC65=_cycalloc(
sizeof(*_tmpC65)),((_tmpC65[0]=((_tmpC66.tag=0,((_tmpC66.f1=_tmp65C->name,
_tmpC66)))),_tmpC65))))),_tmpC67)))))),_tmpC64))));}goto _LL2F4;}_LL2F9: {struct
Cyc_Absyn_TupleType_struct*_tmp657=(struct Cyc_Absyn_TupleType_struct*)_tmp651;
if(_tmp657->tag != 11)goto _LL2FB;}_LL2FA:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC71;struct Cyc_Absyn_StructField_struct _tmpC70;struct Cyc_Absyn_StructField_struct*
_tmpC6F;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6E;e->r=(void*)((_tmpC6E=
_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E[0]=((_tmpC71.tag=20,((_tmpC71.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4D0),((_tmpC71.f2=(void*)((void*)((_tmpC6F=_cycalloc(
sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC70.tag=0,((_tmpC70.f1=Cyc_Absyn_fieldname((
int)(_tmp4D3 + 1)),_tmpC70)))),_tmpC6F))))),_tmpC71)))))),_tmpC6E))));}goto _LL2F4;
_LL2FB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp658=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp651;if(_tmp658->tag != 4)goto _LL2FD;}_LL2FC: if(_tmp4D3 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC7B;struct Cyc_Absyn_StructField_struct _tmpC7A;struct Cyc_Absyn_StructField_struct*
_tmpC79;struct Cyc_Absyn_Offsetof_e_struct*_tmpC78;e->r=(void*)((_tmpC78=
_cycalloc(sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC7B.tag=20,((_tmpC7B.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4D0),((_tmpC7B.f2=(void*)((void*)((_tmpC79=_cycalloc(
sizeof(*_tmpC79)),((_tmpC79[0]=((_tmpC7A.tag=0,((_tmpC7A.f1=Cyc_Toc_tag_sp,
_tmpC7A)))),_tmpC79))))),_tmpC7B)))))),_tmpC78))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC85;struct Cyc_Absyn_StructField_struct _tmpC84;struct Cyc_Absyn_StructField_struct*
_tmpC83;struct Cyc_Absyn_Offsetof_e_struct*_tmpC82;e->r=(void*)((_tmpC82=
_cycalloc(sizeof(*_tmpC82)),((_tmpC82[0]=((_tmpC85.tag=20,((_tmpC85.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4D0),((_tmpC85.f2=(void*)((void*)((_tmpC83=_cycalloc(
sizeof(*_tmpC83)),((_tmpC83[0]=((_tmpC84.tag=0,((_tmpC84.f1=Cyc_Absyn_fieldname((
int)_tmp4D3),_tmpC84)))),_tmpC83))))),_tmpC85)))))),_tmpC82))));}goto _LL2F4;
_LL2FD:;_LL2FE: {const char*_tmpC88;void*_tmpC87;(_tmpC87=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC88="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC88,sizeof(char),41))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}
_LL2F4:;}goto _LL208;_LL239: {struct Cyc_Absyn_Deref_e_struct*_tmp4D4=(struct Cyc_Absyn_Deref_e_struct*)
_tmp484;if(_tmp4D4->tag != 21)goto _LL23B;else{_tmp4D5=_tmp4D4->f1;}}_LL23A: {void*
_tmp66F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D5->topt))->v);{
void*_tmp670=_tmp66F;struct Cyc_Absyn_PtrInfo _tmp672;void*_tmp673;struct Cyc_Absyn_Tqual
_tmp674;struct Cyc_Absyn_PtrAtts _tmp675;void*_tmp676;union Cyc_Absyn_Constraint*
_tmp677;union Cyc_Absyn_Constraint*_tmp678;union Cyc_Absyn_Constraint*_tmp679;
_LL300: {struct Cyc_Absyn_PointerType_struct*_tmp671=(struct Cyc_Absyn_PointerType_struct*)
_tmp670;if(_tmp671->tag != 5)goto _LL302;else{_tmp672=_tmp671->f1;_tmp673=_tmp672.elt_typ;
_tmp674=_tmp672.elt_tq;_tmp675=_tmp672.ptr_atts;_tmp676=_tmp675.rgn;_tmp677=
_tmp675.nullable;_tmp678=_tmp675.bounds;_tmp679=_tmp675.zero_term;}}_LL301:{void*
_tmp67A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp678);struct Cyc_Absyn_Exp*_tmp67C;_LL305: {struct Cyc_Absyn_Upper_b_struct*
_tmp67B=(struct Cyc_Absyn_Upper_b_struct*)_tmp67A;if(_tmp67B->tag != 1)goto _LL307;
else{_tmp67C=_tmp67B->f1;}}_LL306: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4D5);Cyc_Toc_exp_to_c(nv,_tmp4D5);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC8B;void*_tmpC8A;(_tmpC8A=0,Cyc_Tcutil_warn(e->loc,((_tmpC8B="inserted null check due to dereference",
_tag_dyneither(_tmpC8B,sizeof(char),39))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC8C;_tmp4D5->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp66F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC8C=_cycalloc(sizeof(*
_tmpC8C)),((_tmpC8C->hd=Cyc_Absyn_copy_exp(_tmp4D5),((_tmpC8C->tl=0,_tmpC8C)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp679)){unsigned int _tmp682;int _tmp683;struct _tuple10 _tmp681=Cyc_Evexp_eval_const_uint_exp(
_tmp67C);_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;if(!_tmp683  || _tmp682 <= 0){const
char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,Cyc_Tcutil_terr(e->loc,((_tmpC8F="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC8F,sizeof(char),42))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}}
goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_struct*_tmp67D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp67A;if(_tmp67D->tag != 0)goto _LL304;}_LL308: {struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC90;_tmp686->topt=((_tmpC90=_cycalloc(sizeof(*
_tmpC90)),((_tmpC90->v=Cyc_Absyn_uint_typ,_tmpC90))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4D5,_tmp686);Cyc_Toc_exp_to_c(nv,e);goto _LL304;}_LL304:;}goto _LL2FF;_LL302:;
_LL303: {const char*_tmpC93;void*_tmpC92;(_tmpC92=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC93="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC93,sizeof(char),29))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}
_LL2FF:;}goto _LL208;}_LL23B: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D6=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp484;if(_tmp4D6->tag != 22)goto _LL23D;
else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;_tmp4D9=_tmp4D6->f3;_tmp4DA=_tmp4D6->f4;}}
_LL23C: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D7->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4D7);if(
_tmp4D9  && _tmp4DA)e->r=Cyc_Toc_check_tagged_union(_tmp4D7,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4D8,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;}_LL23D: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4DB=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp484;if(_tmp4DB->tag != 23)goto _LL23F;
else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;_tmp4DE=_tmp4DB->f3;_tmp4DF=_tmp4DB->f4;}}
_LL23E: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4DC->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4DC);Cyc_Toc_exp_to_c(
nv,_tmp4DC);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp68B;struct Cyc_Absyn_Tqual
_tmp68C;struct Cyc_Absyn_PtrAtts _tmp68D;void*_tmp68E;union Cyc_Absyn_Constraint*
_tmp68F;union Cyc_Absyn_Constraint*_tmp690;union Cyc_Absyn_Constraint*_tmp691;
struct Cyc_Absyn_PtrInfo _tmp68A=Cyc_Toc_get_ptr_type(e1typ);_tmp68B=_tmp68A.elt_typ;
_tmp68C=_tmp68A.elt_tq;_tmp68D=_tmp68A.ptr_atts;_tmp68E=_tmp68D.rgn;_tmp68F=
_tmp68D.nullable;_tmp690=_tmp68D.bounds;_tmp691=_tmp68D.zero_term;{void*_tmp692=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp690);struct Cyc_Absyn_Exp*_tmp694;_LL30A: {struct Cyc_Absyn_Upper_b_struct*
_tmp693=(struct Cyc_Absyn_Upper_b_struct*)_tmp692;if(_tmp693->tag != 1)goto _LL30C;
else{_tmp694=_tmp693->f1;}}_LL30B: {unsigned int _tmp697;int _tmp698;struct
_tuple10 _tmp696=Cyc_Evexp_eval_const_uint_exp(_tmp694);_tmp697=_tmp696.f1;
_tmp698=_tmp696.f2;if(_tmp698){if(_tmp697 < 1){const char*_tmpC96;void*_tmpC95;(
_tmpC95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC96="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC96,
sizeof(char),44))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC99;void*_tmpC98;(_tmpC98=0,Cyc_Tcutil_warn(
e->loc,((_tmpC99="inserted null check due to dereference",_tag_dyneither(_tmpC99,
sizeof(char),39))),_tag_dyneither(_tmpC98,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC9A[1];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC9A[0]=Cyc_Absyn_new_exp(_tmp4DC->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9A,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp694)){const char*_tmpC9D;void*
_tmpC9C;(_tmpC9C=0,Cyc_Tcutil_terr(e->loc,((_tmpC9D="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC9D,sizeof(char),47))),_tag_dyneither(_tmpC9C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC9E[4];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC9E[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC9E[2]=Cyc_Absyn_sizeoftyp_exp(_tmp68B,0),((
_tmpC9E[1]=_tmp694,((_tmpC9E[0]=Cyc_Absyn_new_exp(_tmp4DC->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9E,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp695=(struct Cyc_Absyn_DynEither_b_struct*)_tmp692;if(_tmp695->tag != 0)goto
_LL309;}_LL30D: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp68B);{struct Cyc_Absyn_Exp*
_tmpC9F[3];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp68C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC9F[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC9F[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC9F[0]=Cyc_Absyn_new_exp(_tmp4DC->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC9F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL309;}_LL309:;}if(
_tmp4DE  && _tmp4DF)e->r=Cyc_Toc_check_tagged_union(_tmp4DC,Cyc_Toc_typ_to_c(
e1typ),_tmp68B,_tmp4DD,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4DF)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;};}_LL23F: {struct Cyc_Absyn_Subscript_e_struct*_tmp4E0=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp484;if(_tmp4E0->tag != 24)goto _LL241;
else{_tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;}}_LL240: {void*_tmp6A2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E1->topt))->v);{void*_tmp6A3=
_tmp6A2;struct Cyc_List_List*_tmp6A5;struct Cyc_Absyn_PtrInfo _tmp6A7;void*_tmp6A8;
struct Cyc_Absyn_Tqual _tmp6A9;struct Cyc_Absyn_PtrAtts _tmp6AA;void*_tmp6AB;union
Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;union Cyc_Absyn_Constraint*
_tmp6AE;_LL30F: {struct Cyc_Absyn_TupleType_struct*_tmp6A4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A3;if(_tmp6A4->tag != 11)goto _LL311;else{_tmp6A5=_tmp6A4->f1;}}_LL310: Cyc_Toc_exp_to_c(
nv,_tmp4E1);Cyc_Toc_exp_to_c(nv,_tmp4E2);{unsigned int _tmp6B0;int _tmp6B1;struct
_tuple10 _tmp6AF=Cyc_Evexp_eval_const_uint_exp(_tmp4E2);_tmp6B0=_tmp6AF.f1;
_tmp6B1=_tmp6AF.f2;if(!_tmp6B1){const char*_tmpCA2;void*_tmpCA1;(_tmpCA1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA2="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCA2,sizeof(char),44))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4E1,Cyc_Absyn_fieldname((int)(_tmp6B0 + 1)));
goto _LL30E;};_LL311: {struct Cyc_Absyn_PointerType_struct*_tmp6A6=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A3;if(_tmp6A6->tag != 5)goto _LL313;else{_tmp6A7=_tmp6A6->f1;_tmp6A8=_tmp6A7.elt_typ;
_tmp6A9=_tmp6A7.elt_tq;_tmp6AA=_tmp6A7.ptr_atts;_tmp6AB=_tmp6AA.rgn;_tmp6AC=
_tmp6AA.nullable;_tmp6AD=_tmp6AA.bounds;_tmp6AE=_tmp6AA.zero_term;}}_LL312: {
struct Cyc_List_List*_tmp6B4=Cyc_Toc_get_relns(_tmp4E1);int in_bnds=0;{void*
_tmp6B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AD);_LL316:;_LL317: in_bnds=Cyc_Toc_check_bounds(_tmp6A2,_tmp6B4,_tmp4E1,
_tmp4E2);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCA6;void*
_tmpCA5[1];struct Cyc_String_pa_struct _tmpCA4;(_tmpCA4.tag=0,((_tmpCA4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA5[0]=&
_tmpCA4,Cyc_Tcutil_warn(e->loc,((_tmpCA6="bounds check necessary for %s",
_tag_dyneither(_tmpCA6,sizeof(char),30))),_tag_dyneither(_tmpCA5,sizeof(void*),1)))))));}
_LL315:;}Cyc_Toc_exp_to_c(nv,_tmp4E1);Cyc_Toc_exp_to_c(nv,_tmp4E2);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AD);struct Cyc_Absyn_Exp*_tmp6BB;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B9;if(_tmp6BA->tag != 1)goto _LL31B;
else{_tmp6BB=_tmp6BA->f1;}}_LL31A: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AC);void*ta1=Cyc_Toc_typ_to_c(_tmp6A8);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A9);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AE)){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCA9;void*_tmpCA8;(
_tmpCA8=0,Cyc_Tcutil_terr(e->loc,((_tmpCA9="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA9,sizeof(char),40))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4E1);struct Cyc_Absyn_Exp*_tmpCAA[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCAA[2]=_tmp4E2,((_tmpCAA[1]=_tmp6BB,((
_tmpCAA[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCAD;void*_tmpCAC;(
_tmpCAC=0,Cyc_Tcutil_terr(e->loc,((_tmpCAD="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCAD,sizeof(char),40))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCB0;void*_tmpCAF;(_tmpCAF=0,Cyc_Tcutil_warn(
e->loc,((_tmpCB0="inserted null check due to dereference",_tag_dyneither(_tmpCB0,
sizeof(char),39))),_tag_dyneither(_tmpCAF,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCB1[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCB1[3]=_tmp4E2,((_tmpCB1[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCB1[1]=
_tmp6BB,((_tmpCB1[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB1,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCB4;void*_tmpCB3;(_tmpCB3=0,Cyc_Tcutil_terr(
e->loc,((_tmpCB4="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCB4,sizeof(char),40))),_tag_dyneither(_tmpCB3,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCB5[2];_tmp4E2->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCB5[1]=Cyc_Absyn_copy_exp(_tmp4E2),((_tmpCB5[0]=_tmp6BB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB5,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL318;}_LL31B: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BC=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6B9;if(_tmp6BC->tag != 0)goto _LL318;}
_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6A8);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),
Cyc_Absyn_aggrmember_exp(_tmp4E1,Cyc_Toc_curr_sp,0)),_tmp4E2);}else{struct Cyc_Absyn_Exp*
_tmp6C8=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCB6[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),Cyc_Absyn_fncall_exp(
_tmp6C8,((_tmpCB6[2]=_tmp4E2,((_tmpCB6[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCB6[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL318;}
_LL318:;}goto _LL30E;}_LL313:;_LL314: {const char*_tmpCB9;void*_tmpCB8;(_tmpCB8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCB9="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCB9,sizeof(char),49))),_tag_dyneither(_tmpCB8,sizeof(void*),0)));}_LL30E:;}
goto _LL208;}_LL241: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E3=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp484;if(_tmp4E3->tag != 25)goto _LL243;else{_tmp4E4=_tmp4E3->f1;}}_LL242: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E4))->r;else{struct
Cyc_List_List*_tmp6CC=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E4);void*_tmp6CD=Cyc_Toc_add_tuple_type(
_tmp6CC);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4E4 != 0;(_tmp4E4=_tmp4E4->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E4->hd);{struct _tuple15*
_tmpCBC;struct Cyc_List_List*_tmpCBB;dles=((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((
_tmpCBB->hd=((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC->f1=0,((_tmpCBC->f2=(
struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmpCBC)))))),((_tmpCBB->tl=dles,_tmpCBB))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL208;_LL243: {struct Cyc_Absyn_Array_e_struct*
_tmp4E5=(struct Cyc_Absyn_Array_e_struct*)_tmp484;if(_tmp4E5->tag != 27)goto _LL245;
else{_tmp4E6=_tmp4E5->f1;}}_LL244: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E6);{
struct Cyc_List_List*_tmp6D0=_tmp4E6;for(0;_tmp6D0 != 0;_tmp6D0=_tmp6D0->tl){
struct _tuple15 _tmp6D2;struct Cyc_Absyn_Exp*_tmp6D3;struct _tuple15*_tmp6D1=(struct
_tuple15*)_tmp6D0->hd;_tmp6D2=*_tmp6D1;_tmp6D3=_tmp6D2.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D3);}}goto _LL208;_LL245: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E7=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp484;if(_tmp4E7->tag != 28)goto _LL247;
else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;_tmp4EA=_tmp4E7->f3;_tmp4EB=_tmp4E7->f4;}}
_LL246: {unsigned int _tmp6D5;int _tmp6D6;struct _tuple10 _tmp6D4=Cyc_Evexp_eval_const_uint_exp(
_tmp4E9);_tmp6D5=_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;{void*_tmp6D7=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4EA->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4EA);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4EA)){if(!_tmp6D6){
const char*_tmpCBF;void*_tmpCBE;(_tmpCBE=0,Cyc_Tcutil_terr(_tmp4E9->loc,((_tmpCBF="cannot determine value of constant",
_tag_dyneither(_tmpCBF,sizeof(char),35))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6D5;++ i){struct _tuple15*_tmpCC2;struct Cyc_List_List*
_tmpCC1;es=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=((_tmpCC2=
_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=_tmp4EA,_tmpCC2)))))),((
_tmpCC1->tl=es,_tmpCC1))))));}}if(_tmp4EB){struct Cyc_Absyn_Exp*_tmp6DC=Cyc_Toc_cast_it(
_tmp6D7,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCC5;struct Cyc_List_List*
_tmpCC4;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->hd=((
_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->f1=0,((_tmpCC5->f2=_tmp6DC,
_tmpCC5)))))),((_tmpCC4->tl=0,_tmpCC4)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL208;};};}_LL247: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4EC=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp484;if(_tmp4EC->tag != 29)goto _LL249;
else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;_tmp4EF=_tmp4EC->f3;_tmp4F0=_tmp4EC->f4;}}
_LL248: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EE
!= 0,0,0,_tmp4EF,_tmp4ED))->r;else{if(_tmp4F0 == 0){const char*_tmpCC8;void*
_tmpCC7;(_tmpCC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC8="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCC8,sizeof(
char),38))),_tag_dyneither(_tmpCC7,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4F0;struct Cyc_Toc_Env _tmp6E2;struct
_RegionHandle*_tmp6E3;struct Cyc_Toc_Env*_tmp6E1=nv;_tmp6E2=*_tmp6E1;_tmp6E3=
_tmp6E2.rgn;{struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E3,e->loc,_tmp4EF,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F0->impl))->tagged){struct
_tuple20 _tmp6E6;struct Cyc_Absyn_Aggrfield*_tmp6E7;struct Cyc_Absyn_Exp*_tmp6E8;
struct _tuple20*_tmp6E5=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E4))->hd;_tmp6E6=*_tmp6E5;_tmp6E7=_tmp6E6.f1;_tmp6E8=_tmp6E6.f2;{void*
_tmp6E9=_tmp6E7->type;Cyc_Toc_exp_to_c(nv,_tmp6E8);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E9))_tmp6E8->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E8->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4F0,_tmp6E7->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCCD;struct _tuple15*_tmpCCC;struct _tuple15*_tmpCCB[2];
struct Cyc_List_List*_tmp6EA=(_tmpCCB[1]=((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((
_tmpCCC->f1=0,((_tmpCCC->f2=_tmp6E8,_tmpCCC)))))),((_tmpCCB[0]=((_tmpCCD=
_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->f1=0,((_tmpCCD->f2=field_tag_exp,_tmpCCD)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCCB,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6EA,0);struct Cyc_Absyn_FieldName_struct*_tmpCD3;struct Cyc_Absyn_FieldName_struct
_tmpCD2;void*_tmpCD1[1];struct Cyc_List_List*ds=(_tmpCD1[0]=(void*)((_tmpCD3=
_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD2.tag=1,((_tmpCD2.f1=_tmp6E7->name,
_tmpCD2)))),_tmpCD3)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCD1,sizeof(void*),1)));struct _tuple15*_tmpCD6;struct _tuple15*
_tmpCD5[1];struct Cyc_List_List*dles=(_tmpCD5[0]=((_tmpCD6=_cycalloc(sizeof(*
_tmpCD6)),((_tmpCD6->f1=ds,((_tmpCD6->f2=umem,_tmpCD6)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD5,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp6F3=0;struct Cyc_List_List*_tmp6F4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F4 != 0;_tmp6F4=_tmp6F4->tl){struct Cyc_List_List*
_tmp6F5=_tmp6E4;for(0;_tmp6F5 != 0;_tmp6F5=_tmp6F5->tl){if((*((struct _tuple20*)
_tmp6F5->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F4->hd){struct _tuple20
_tmp6F7;struct Cyc_Absyn_Aggrfield*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F9;struct
_tuple20*_tmp6F6=(struct _tuple20*)_tmp6F5->hd;_tmp6F7=*_tmp6F6;_tmp6F8=_tmp6F7.f1;
_tmp6F9=_tmp6F7.f2;{void*_tmp6FA=_tmp6F8->type;Cyc_Toc_exp_to_c(nv,_tmp6F9);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6FA))_tmp6F9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F9->r,0));{struct _tuple15*_tmpCD9;struct Cyc_List_List*
_tmpCD8;_tmp6F3=((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=((_tmpCD9=
_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->f1=0,((_tmpCD9->f2=_tmp6F9,_tmpCD9)))))),((
_tmpCD8->tl=_tmp6F3,_tmpCD8))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F3));}};};}
goto _LL208;_LL249: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp4F1=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp484;if(_tmp4F1->tag != 30)goto _LL24B;else{_tmp4F2=(void*)_tmp4F1->f1;_tmp4F3=
_tmp4F1->f2;}}_LL24A: {struct Cyc_List_List*fs;{void*_tmp6FD=Cyc_Tcutil_compress(
_tmp4F2);struct Cyc_List_List*_tmp6FF;_LL31E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6FE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6FD;if(_tmp6FE->tag != 13)goto
_LL320;else{_tmp6FF=_tmp6FE->f2;}}_LL31F: fs=_tmp6FF;goto _LL31D;_LL320:;_LL321: {
const char*_tmpCDD;void*_tmpCDC[1];struct Cyc_String_pa_struct _tmpCDB;(_tmpCDB.tag=
0,((_tmpCDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4F2)),((_tmpCDC[0]=& _tmpCDB,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDD="anon struct has type %s",
_tag_dyneither(_tmpCDD,sizeof(char),24))),_tag_dyneither(_tmpCDC,sizeof(void*),1)))))));}
_LL31D:;}{struct Cyc_Toc_Env _tmp704;struct _RegionHandle*_tmp705;struct Cyc_Toc_Env*
_tmp703=nv;_tmp704=*_tmp703;_tmp705=_tmp704.rgn;{struct Cyc_List_List*_tmp706=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp705,e->loc,_tmp4F3,Cyc_Absyn_StructA,fs);for(0;_tmp706 != 0;_tmp706=_tmp706->tl){
struct _tuple20 _tmp708;struct Cyc_Absyn_Aggrfield*_tmp709;struct Cyc_Absyn_Exp*
_tmp70A;struct _tuple20*_tmp707=(struct _tuple20*)_tmp706->hd;_tmp708=*_tmp707;
_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;{void*_tmp70B=_tmp709->type;Cyc_Toc_exp_to_c(
nv,_tmp70A);if(Cyc_Toc_is_void_star_or_tvar(_tmp70B))_tmp70A->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp70A->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp4F3);}goto _LL208;};}_LL24B: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F4=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp484;if(_tmp4F4->tag != 31)goto _LL24D;else{
_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;_tmp4F7=_tmp4F4->f3;}}_LL24C: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp70C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp70D=Cyc_Absyn_var_exp(_tmp70C,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCDE;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4F7->name,((_tmpCDE="_struct",_tag_dyneither(_tmpCDE,sizeof(char),8)))));}
tag_exp=_tmp4F6->is_extensible?Cyc_Absyn_var_exp(_tmp4F7->name,0): Cyc_Toc_datatype_tag(
_tmp4F6,_tmp4F7->name);member_exp=_tmp70D;{struct Cyc_List_List*_tmp70F=_tmp4F7->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F5 != 0;(
_tmp4F5=_tmp4F5->tl,_tmp70F=_tmp70F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F5->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCE1;struct
Cyc_List_List*_tmpCE0;dles=((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->hd=((
_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=0,((_tmpCE1->f2=cur_e,_tmpCE1)))))),((
_tmpCE0->tl=dles,_tmpCE0))))));};}{struct _tuple15*_tmpCE4;struct Cyc_List_List*
_tmpCE3;dles=((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->hd=((_tmpCE4=
_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->f1=0,((_tmpCE4->f2=tag_exp,_tmpCE4)))))),((
_tmpCE3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCE3))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCE5;struct Cyc_List_List*_tmp714=(_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((
_tmpCE5->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCE5->tl=0,_tmpCE5)))));{int i=1;for(0;_tmp4F5 != 0;(((_tmp4F5=
_tmp4F5->tl,i ++)),_tmp70F=_tmp70F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F5->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp715=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCE6;_tmp714=((_tmpCE6=_cycalloc(sizeof(*
_tmpCE6)),((_tmpCE6->hd=_tmp715,((_tmpCE6->tl=_tmp714,_tmpCE6))))));};}}{struct
Cyc_Absyn_Stmt*_tmp717=Cyc_Absyn_exp_stmt(_tmp70D,0);struct Cyc_List_List*_tmpCE7;
struct Cyc_Absyn_Stmt*_tmp718=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((
_tmpCE7->hd=_tmp717,((_tmpCE7->tl=_tmp714,_tmpCE7))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp70C,datatype_ctype,0,_tmp718,0));};}goto _LL208;};}
_LL24D: {struct Cyc_Absyn_Enum_e_struct*_tmp4F8=(struct Cyc_Absyn_Enum_e_struct*)
_tmp484;if(_tmp4F8->tag != 32)goto _LL24F;}_LL24E: goto _LL250;_LL24F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F9=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp484;if(_tmp4F9->tag != 33)goto
_LL251;}_LL250: goto _LL208;_LL251: {struct Cyc_Absyn_Malloc_e_struct*_tmp4FA=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp484;if(_tmp4FA->tag != 34)goto _LL253;else{
_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FB.is_calloc;_tmp4FD=_tmp4FB.rgn;_tmp4FE=
_tmp4FB.elt_type;_tmp4FF=_tmp4FB.num_elts;_tmp500=_tmp4FB.fat_result;}}_LL252: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FE)));Cyc_Toc_exp_to_c(nv,
_tmp4FF);if(_tmp500){struct _tuple0*_tmp71B=Cyc_Toc_temp_var();struct _tuple0*
_tmp71C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4FC){xexp=_tmp4FF;if(_tmp4FD != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FD;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp71B,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FE,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp71B,0));}{struct Cyc_Absyn_Exp*_tmpCE8[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE8[2]=Cyc_Absyn_var_exp(_tmp71B,0),((_tmpCE8[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCE8[0]=Cyc_Absyn_var_exp(_tmp71C,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCE8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4FF,0);if(_tmp4FD != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FD;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp71B,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp4FE,Cyc_Absyn_var_exp(_tmp71B,0));}{struct Cyc_Absyn_Exp*_tmpCE9[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE9[2]=Cyc_Absyn_var_exp(_tmp71B,0),((_tmpCE9[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE9[0]=Cyc_Absyn_var_exp(_tmp71C,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp71F=Cyc_Absyn_declare_stmt(_tmp71B,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp71C,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71F);};}else{struct Cyc_Absyn_Exp*_tmp720=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp721=_tmp4FF->r;union Cyc_Absyn_Cnst _tmp723;struct _tuple5 _tmp724;
int _tmp725;_LL323: {struct Cyc_Absyn_Const_e_struct*_tmp722=(struct Cyc_Absyn_Const_e_struct*)
_tmp721;if(_tmp722->tag != 0)goto _LL325;else{_tmp723=_tmp722->f1;if((_tmp723.Int_c).tag
!= 4)goto _LL325;_tmp724=(struct _tuple5)(_tmp723.Int_c).val;_tmp725=_tmp724.f2;
if(_tmp725 != 1)goto _LL325;}}_LL324: goto _LL322;_LL325:;_LL326: _tmp720=Cyc_Absyn_times_exp(
_tmp720,_tmp4FF,0);goto _LL322;_LL322:;}if(_tmp4FD != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FD;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp720))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FE,
_tmp720))->r;}}goto _LL208;}_LL253: {struct Cyc_Absyn_Swap_e_struct*_tmp501=(
struct Cyc_Absyn_Swap_e_struct*)_tmp484;if(_tmp501->tag != 35)goto _LL255;else{
_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;}}_LL254: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp502->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp503->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEC="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCEC,sizeof(char),57))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp502);Cyc_Toc_exp_to_c(
nv,_tmp503);{struct Cyc_Absyn_Exp*_tmpCED[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCED[1]=Cyc_Toc_push_address_exp(_tmp503),((_tmpCED[0]=Cyc_Toc_push_address_exp(
_tmp502),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCED,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL208;};}
_LL255: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp504=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp484;if(_tmp504->tag != 38)goto _LL257;else{_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;}}
_LL256: {void*_tmp729=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp505->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp505);{void*_tmp72A=
_tmp729;struct Cyc_Absyn_AggrInfo _tmp72C;union Cyc_Absyn_AggrInfoU _tmp72D;struct
Cyc_Absyn_Aggrdecl**_tmp72E;struct Cyc_Absyn_Aggrdecl*_tmp72F;_LL328: {struct Cyc_Absyn_AggrType_struct*
_tmp72B=(struct Cyc_Absyn_AggrType_struct*)_tmp72A;if(_tmp72B->tag != 12)goto
_LL32A;else{_tmp72C=_tmp72B->f1;_tmp72D=_tmp72C.aggr_info;if((_tmp72D.KnownAggr).tag
!= 2)goto _LL32A;_tmp72E=(struct Cyc_Absyn_Aggrdecl**)(_tmp72D.KnownAggr).val;
_tmp72F=*_tmp72E;}}_LL329: {struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72F,_tmp506),0);struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_aggrmember_exp(
_tmp505,_tmp506,0);struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_aggrmember_exp(_tmp731,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp732,_tmp730,0))->r;goto _LL327;}
_LL32A:;_LL32B: {const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCF0,sizeof(char),31))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
_LL327:;}goto _LL208;}_LL257: {struct Cyc_Absyn_StmtExp_e_struct*_tmp507=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp484;if(_tmp507->tag != 37)goto _LL259;else{_tmp508=
_tmp507->f1;}}_LL258: Cyc_Toc_stmt_to_c(nv,_tmp508);goto _LL208;_LL259: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp509=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp484;if(_tmp509->tag != 36)
goto _LL25B;}_LL25A: {const char*_tmpCF3;void*_tmpCF2;(_tmpCF2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF3="UnresolvedMem",
_tag_dyneither(_tmpCF3,sizeof(char),14))),_tag_dyneither(_tmpCF2,sizeof(void*),0)));}
_LL25B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp50A=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp484;if(_tmp50A->tag != 26)goto _LL25D;}_LL25C: {const char*_tmpCF6;void*_tmpCF5;(
_tmpCF5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCF6="compoundlit",_tag_dyneither(_tmpCF6,sizeof(char),12))),_tag_dyneither(
_tmpCF5,sizeof(void*),0)));}_LL25D: {struct Cyc_Absyn_Valueof_e_struct*_tmp50B=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp484;if(_tmp50B->tag != 39)goto _LL208;}
_LL25E: {const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF9="valueof(-)",
_tag_dyneither(_tmpCF9,sizeof(char),11))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
_LL208:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
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
struct Cyc_Absyn_Stmt*s;{void*_tmp73B=p->r;struct Cyc_Absyn_Vardecl*_tmp73D;struct
Cyc_Absyn_Vardecl _tmp73E;struct _tuple0*_tmp73F;struct Cyc_Absyn_Pat*_tmp740;
struct Cyc_Absyn_Vardecl*_tmp742;struct Cyc_Absyn_Vardecl _tmp743;struct _tuple0*
_tmp744;struct Cyc_Absyn_Vardecl*_tmp747;struct Cyc_Absyn_Pat*_tmp748;enum Cyc_Absyn_Sign
_tmp74B;int _tmp74C;char _tmp74E;struct _dyneither_ptr _tmp750;struct Cyc_Absyn_Enumdecl*
_tmp752;struct Cyc_Absyn_Enumfield*_tmp753;void*_tmp755;struct Cyc_Absyn_Enumfield*
_tmp756;struct Cyc_Absyn_Pat*_tmp758;struct Cyc_Absyn_Pat _tmp759;void*_tmp75A;
struct Cyc_Absyn_Datatypedecl*_tmp75C;struct Cyc_Absyn_Datatypefield*_tmp75D;
struct Cyc_List_List*_tmp75E;struct Cyc_List_List*_tmp760;struct Cyc_List_List*
_tmp762;struct Cyc_Absyn_AggrInfo*_tmp764;struct Cyc_Absyn_AggrInfo*_tmp766;struct
Cyc_Absyn_AggrInfo _tmp767;union Cyc_Absyn_AggrInfoU _tmp768;struct Cyc_List_List*
_tmp769;struct Cyc_Absyn_Pat*_tmp76B;_LL32D: {struct Cyc_Absyn_Var_p_struct*
_tmp73C=(struct Cyc_Absyn_Var_p_struct*)_tmp73B;if(_tmp73C->tag != 1)goto _LL32F;
else{_tmp73D=_tmp73C->f1;_tmp73E=*_tmp73D;_tmp73F=_tmp73E.name;_tmp740=_tmp73C->f2;}}
_LL32E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73F,r),rgn,t,r,path,
_tmp740,fail_stmt,decls);_LL32F: {struct Cyc_Absyn_TagInt_p_struct*_tmp741=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp73B;if(_tmp741->tag != 3)goto _LL331;else{
_tmp742=_tmp741->f2;_tmp743=*_tmp742;_tmp744=_tmp743.name;}}_LL330: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp744,r);goto _LL332;_LL331: {struct Cyc_Absyn_Wild_p_struct*_tmp745=(
struct Cyc_Absyn_Wild_p_struct*)_tmp73B;if(_tmp745->tag != 0)goto _LL333;}_LL332: s=
Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL333: {struct Cyc_Absyn_Reference_p_struct*
_tmp746=(struct Cyc_Absyn_Reference_p_struct*)_tmp73B;if(_tmp746->tag != 2)goto
_LL335;else{_tmp747=_tmp746->f1;_tmp748=_tmp746->f2;}}_LL334: {struct _tuple0*
_tmp76F=Cyc_Toc_temp_var();{struct _tuple22*_tmpCFC;struct Cyc_List_List*_tmpCFB;
decls=((_tmpCFB=_region_malloc(rgn,sizeof(*_tmpCFB)),((_tmpCFB->hd=((_tmpCFC=
_region_malloc(rgn,sizeof(*_tmpCFC)),((_tmpCFC->f1=_tmp76F,((_tmpCFC->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCFC)))))),((_tmpCFB->tl=decls,_tmpCFB))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp747->name,Cyc_Absyn_var_exp(_tmp76F,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp76F,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp772=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp748,fail_stmt,decls);_tmp772.f3=Cyc_Absyn_seq_stmt(s,_tmp772.f3,
0);return _tmp772;};}_LL335: {struct Cyc_Absyn_Null_p_struct*_tmp749=(struct Cyc_Absyn_Null_p_struct*)
_tmp73B;if(_tmp749->tag != 8)goto _LL337;}_LL336: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32C;_LL337: {struct Cyc_Absyn_Int_p_struct*_tmp74A=(struct
Cyc_Absyn_Int_p_struct*)_tmp73B;if(_tmp74A->tag != 9)goto _LL339;else{_tmp74B=
_tmp74A->f1;_tmp74C=_tmp74A->f2;}}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp74B,_tmp74C,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Char_p_struct*
_tmp74D=(struct Cyc_Absyn_Char_p_struct*)_tmp73B;if(_tmp74D->tag != 10)goto _LL33B;
else{_tmp74E=_tmp74D->f1;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74E,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Float_p_struct*_tmp74F=(
struct Cyc_Absyn_Float_p_struct*)_tmp73B;if(_tmp74F->tag != 11)goto _LL33D;else{
_tmp750=_tmp74F->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp750,
0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Enum_p_struct*_tmp751=(struct
Cyc_Absyn_Enum_p_struct*)_tmp73B;if(_tmp751->tag != 12)goto _LL33F;else{_tmp752=
_tmp751->f1;_tmp753=_tmp751->f2;}}_LL33E:{struct Cyc_Absyn_Enum_e_struct _tmpCFF;
struct Cyc_Absyn_Enum_e_struct*_tmpCFE;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=32,((
_tmpCFF.f1=_tmp753->name,((_tmpCFF.f2=(struct Cyc_Absyn_Enumdecl*)_tmp752,((
_tmpCFF.f3=(struct Cyc_Absyn_Enumfield*)_tmp753,_tmpCFF)))))))),_tmpCFE)))),0),
fail_stmt);}goto _LL32C;_LL33F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp754=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp73B;if(_tmp754->tag != 13)goto _LL341;else{
_tmp755=(void*)_tmp754->f1;_tmp756=_tmp754->f2;}}_LL340:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD02;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD01;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=33,((
_tmpD02.f1=_tmp756->name,((_tmpD02.f2=(void*)_tmp755,((_tmpD02.f3=(struct Cyc_Absyn_Enumfield*)
_tmp756,_tmpD02)))))))),_tmpD01)))),0),fail_stmt);}goto _LL32C;_LL341: {struct Cyc_Absyn_Pointer_p_struct*
_tmp757=(struct Cyc_Absyn_Pointer_p_struct*)_tmp73B;if(_tmp757->tag != 5)goto
_LL343;else{_tmp758=_tmp757->f1;_tmp759=*_tmp758;_tmp75A=_tmp759.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp75B=(struct Cyc_Absyn_Datatype_p_struct*)_tmp75A;if(_tmp75B->tag != 7)goto
_LL343;else{_tmp75C=_tmp75B->f1;_tmp75D=_tmp75B->f2;_tmp75E=_tmp75B->f3;}};}}
_LL342: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp777=Cyc_Toc_temp_var();const
char*_tmpD03;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75D->name,((
_tmpD03="_struct",_tag_dyneither(_tmpD03,sizeof(char),8))));void*_tmp778=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp778,r);struct Cyc_List_List*_tmp779=_tmp75D->typs;for(0;_tmp75E != 0;(((
_tmp75E=_tmp75E->tl,_tmp779=((struct Cyc_List_List*)_check_null(_tmp779))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp77A=(struct Cyc_Absyn_Pat*)_tmp75E->hd;if(_tmp77A->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp77B=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp779))->hd)).f2;struct _tuple0*_tmp77C=Cyc_Toc_temp_var();
void*_tmp77D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77A->topt))->v;void*
_tmp77E=Cyc_Toc_typ_to_c(_tmp77D);struct Cyc_Absyn_Exp*_tmp77F=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp777,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp77B)))_tmp77F=Cyc_Toc_cast_it(_tmp77E,_tmp77F);{struct
_tuple22*_tmpD06;struct Cyc_List_List*_tmpD05;decls=((_tmpD05=_region_malloc(rgn,
sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=_region_malloc(rgn,sizeof(*_tmpD06)),((
_tmpD06->f1=_tmp77C,((_tmpD06->f2=_tmp77E,_tmpD06)))))),((_tmpD05->tl=decls,
_tmpD05))))));}{struct _tuple21 _tmp782=Cyc_Toc_xlate_pat(nv,rgn,_tmp77D,Cyc_Absyn_var_exp(
_tmp77C,0),_tmp77F,_tmp77A,fail_stmt,decls);nv=_tmp782.f1;decls=_tmp782.f2;{
struct Cyc_Absyn_Stmt*_tmp783=_tmp782.f3;struct Cyc_Absyn_Stmt*_tmp784=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77C,0),_tmp77F,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp784,_tmp783,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp777,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp75C->is_extensible?Cyc_Absyn_var_exp(_tmp75D->name,0): Cyc_Toc_datatype_tag(
_tmp75C,_tmp75D->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp777,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp777,_tmp778,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL32C;};};_LL343: {struct Cyc_Absyn_Datatype_p_struct*_tmp75F=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp73B;if(_tmp75F->tag != 7)goto _LL345;else{
_tmp760=_tmp75F->f3;}}_LL344: _tmp762=_tmp760;goto _LL346;_LL345: {struct Cyc_Absyn_Tuple_p_struct*
_tmp761=(struct Cyc_Absyn_Tuple_p_struct*)_tmp73B;if(_tmp761->tag != 4)goto _LL347;
else{_tmp762=_tmp761->f1;}}_LL346: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp762 != 0;(_tmp762=_tmp762->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp786=(struct Cyc_Absyn_Pat*)
_tmp762->hd;if(_tmp786->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp787=Cyc_Toc_temp_var();void*_tmp788=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp786->topt))->v;{struct _tuple22*_tmpD09;struct Cyc_List_List*_tmpD08;decls=((
_tmpD08=_region_malloc(rgn,sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=
_region_malloc(rgn,sizeof(*_tmpD09)),((_tmpD09->f1=_tmp787,((_tmpD09->f2=Cyc_Toc_typ_to_c(
_tmp788),_tmpD09)))))),((_tmpD08->tl=decls,_tmpD08))))));}{struct _tuple21 _tmp78B=
Cyc_Toc_xlate_pat(nv,rgn,_tmp788,Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp786,fail_stmt,decls);nv=_tmp78B.f1;decls=
_tmp78B.f2;{struct Cyc_Absyn_Stmt*_tmp78C=_tmp78B.f3;struct Cyc_Absyn_Stmt*_tmp78D=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp78D,
_tmp78C,0),0);};};};}goto _LL32C;};_LL347: {struct Cyc_Absyn_Aggr_p_struct*_tmp763=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp73B;if(_tmp763->tag != 6)goto _LL349;else{
_tmp764=_tmp763->f1;if(_tmp764 != 0)goto _LL349;}}_LL348: {const char*_tmpD0C;void*
_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD0C="unresolved aggregate pattern!",_tag_dyneither(_tmpD0C,sizeof(char),30))),
_tag_dyneither(_tmpD0B,sizeof(void*),0)));}_LL349: {struct Cyc_Absyn_Aggr_p_struct*
_tmp765=(struct Cyc_Absyn_Aggr_p_struct*)_tmp73B;if(_tmp765->tag != 6)goto _LL34B;
else{_tmp766=_tmp765->f1;if(_tmp766 == 0)goto _LL34B;_tmp767=*_tmp766;_tmp768=
_tmp767.aggr_info;_tmp769=_tmp765->f3;}}_LL34A: {struct Cyc_Absyn_Aggrdecl*
_tmp790=Cyc_Absyn_get_known_aggrdecl(_tmp768);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp790->impl))->tagged){struct _tuple23 _tmp792;struct Cyc_List_List*
_tmp793;struct Cyc_Absyn_Pat*_tmp794;struct _tuple23*_tmp791=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp769))->hd;_tmp792=*_tmp791;_tmp793=_tmp792.f1;
_tmp794=_tmp792.f2;{struct _dyneither_ptr*f;{void*_tmp795=(void*)((struct Cyc_List_List*)
_check_null(_tmp793))->hd;struct _dyneither_ptr*_tmp797;_LL354: {struct Cyc_Absyn_FieldName_struct*
_tmp796=(struct Cyc_Absyn_FieldName_struct*)_tmp795;if(_tmp796->tag != 1)goto
_LL356;else{_tmp797=_tmp796->f1;}}_LL355: f=_tmp797;goto _LL353;_LL356:;_LL357: {
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0F="no field name in tagged union pattern",
_tag_dyneither(_tmpD0F,sizeof(char),38))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}
_LL353:;}{struct _tuple0*_tmp79A=Cyc_Toc_temp_var();void*_tmp79B=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp794->topt))->v;void*_tmp79C=Cyc_Toc_typ_to_c(
_tmp79B);{struct _tuple22*_tmpD12;struct Cyc_List_List*_tmpD11;decls=((_tmpD11=
_region_malloc(rgn,sizeof(*_tmpD11)),((_tmpD11->hd=((_tmpD12=_region_malloc(rgn,
sizeof(*_tmpD12)),((_tmpD12->f1=_tmp79A,((_tmpD12->f2=_tmp79C,_tmpD12)))))),((
_tmpD11->tl=decls,_tmpD11))))));}{struct Cyc_Absyn_Exp*_tmp79F=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7A0=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7A0=Cyc_Toc_cast_it(_tmp79C,_tmp7A0);_tmp79F=Cyc_Toc_cast_it(_tmp79C,_tmp79F);{
struct _tuple21 _tmp7A1=Cyc_Toc_xlate_pat(nv,rgn,_tmp79B,Cyc_Absyn_var_exp(_tmp79A,
0),_tmp79F,_tmp794,fail_stmt,decls);nv=_tmp7A1.f1;decls=_tmp7A1.f2;{struct Cyc_Absyn_Stmt*
_tmp7A2=_tmp7A1.f3;struct Cyc_Absyn_Stmt*_tmp7A3=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp790,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp79A,0),_tmp7A0,0);s=Cyc_Absyn_seq_stmt(_tmp7A3,Cyc_Absyn_seq_stmt(_tmp7A4,
_tmp7A2,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp769 != 0;_tmp769=
_tmp769->tl){struct _tuple23*_tmp7A5=(struct _tuple23*)_tmp769->hd;struct Cyc_Absyn_Pat*
_tmp7A6=(*_tmp7A5).f2;if(_tmp7A6->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7A7=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A5).f1))->hd;struct _dyneither_ptr*_tmp7A9;_LL359: {struct Cyc_Absyn_FieldName_struct*
_tmp7A8=(struct Cyc_Absyn_FieldName_struct*)_tmp7A7;if(_tmp7A8->tag != 1)goto
_LL35B;else{_tmp7A9=_tmp7A8->f1;}}_LL35A: f=_tmp7A9;goto _LL358;_LL35B:;_LL35C: {
struct Cyc_Toc_Match_error_struct _tmpD15;struct Cyc_Toc_Match_error_struct*_tmpD14;(
int)_throw((void*)((_tmpD14=_cycalloc_atomic(sizeof(*_tmpD14)),((_tmpD14[0]=((
_tmpD15.tag=Cyc_Toc_Match_error,_tmpD15)),_tmpD14)))));}_LL358:;}{struct _tuple0*
_tmp7AC=Cyc_Toc_temp_var();void*_tmp7AD=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A6->topt))->v;void*_tmp7AE=Cyc_Toc_typ_to_c(_tmp7AD);{struct _tuple22*
_tmpD18;struct Cyc_List_List*_tmpD17;decls=((_tmpD17=_region_malloc(rgn,sizeof(*
_tmpD17)),((_tmpD17->hd=((_tmpD18=_region_malloc(rgn,sizeof(*_tmpD18)),((_tmpD18->f1=
_tmp7AC,((_tmpD18->f2=_tmp7AE,_tmpD18)))))),((_tmpD17->tl=decls,_tmpD17))))));}{
struct _tuple21 _tmp7B1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AD,Cyc_Absyn_var_exp(_tmp7AC,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7A6,fail_stmt,decls);nv=_tmp7B1.f1;
decls=_tmp7B1.f2;{struct Cyc_Absyn_Exp*_tmp7B2=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp790->impl))->fields,f)))->type))
_tmp7B2=Cyc_Toc_cast_it(_tmp7AE,_tmp7B2);{struct Cyc_Absyn_Stmt*_tmp7B3=_tmp7B1.f3;
struct Cyc_Absyn_Stmt*_tmp7B4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AC,0),
_tmp7B2,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7B4,_tmp7B3,0),0);};};};};};}}
goto _LL32C;}_LL34B: {struct Cyc_Absyn_Pointer_p_struct*_tmp76A=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp73B;if(_tmp76A->tag != 5)goto _LL34D;else{_tmp76B=_tmp76A->f1;}}_LL34C: {
struct _tuple0*_tmp7B5=Cyc_Toc_temp_var();void*_tmp7B6=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp76B->topt))->v;{struct _tuple22*_tmpD1B;struct Cyc_List_List*
_tmpD1A;decls=((_tmpD1A=_region_malloc(rgn,sizeof(*_tmpD1A)),((_tmpD1A->hd=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->f1=_tmp7B5,((_tmpD1B->f2=
Cyc_Toc_typ_to_c(_tmp7B6),_tmpD1B)))))),((_tmpD1A->tl=decls,_tmpD1A))))));}{
struct _tuple21 _tmp7B9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B6,Cyc_Absyn_var_exp(_tmp7B5,
0),Cyc_Absyn_deref_exp(path,0),_tmp76B,fail_stmt,decls);nv=_tmp7B9.f1;decls=
_tmp7B9.f2;{struct Cyc_Absyn_Stmt*_tmp7BA=_tmp7B9.f3;struct Cyc_Absyn_Stmt*_tmp7BB=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B5,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7BA,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7BB,0);else{s=_tmp7BB;}goto _LL32C;};};}_LL34D: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp76C=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp73B;if(_tmp76C->tag != 14)goto
_LL34F;}_LL34E: {const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1E="unknownid pat",
_tag_dyneither(_tmpD1E,sizeof(char),14))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}
_LL34F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76D=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp73B;if(_tmp76D->tag != 15)goto _LL351;}_LL350: {const char*_tmpD21;void*_tmpD20;(
_tmpD20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD21="unknowncall pat",_tag_dyneither(_tmpD21,sizeof(char),16))),
_tag_dyneither(_tmpD20,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Exp_p_struct*
_tmp76E=(struct Cyc_Absyn_Exp_p_struct*)_tmp73B;if(_tmp76E->tag != 16)goto _LL32C;}
_LL352: {const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD24="exp pat",
_tag_dyneither(_tmpD24,sizeof(char),8))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
_LL32C:;}{struct _tuple21 _tmpD25;return(_tmpD25.f1=nv,((_tmpD25.f2=decls,((
_tmpD25.f3=s,_tmpD25)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD26;
return(_tmpD26=_region_malloc(r,sizeof(*_tmpD26)),((_tmpD26->f1=Cyc_Toc_fresh_label(),((
_tmpD26->f2=Cyc_Toc_fresh_label(),((_tmpD26->f3=sc,_tmpD26)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C4=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7C5=Cyc_Tcutil_compress(_tmp7C4);_LL35E: {struct Cyc_Absyn_IntType_struct*
_tmp7C6=(struct Cyc_Absyn_IntType_struct*)_tmp7C5;if(_tmp7C6->tag != 6)goto _LL360;}
_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_EnumType_struct*_tmp7C7=(struct Cyc_Absyn_EnumType_struct*)
_tmp7C5;if(_tmp7C7->tag != 14)goto _LL362;}_LL361: leave_as_switch=1;goto _LL35D;
_LL362:;_LL363: leave_as_switch=0;goto _LL35D;_LL35D:;}{struct Cyc_List_List*
_tmp7C8=scs;for(0;_tmp7C8 != 0;_tmp7C8=_tmp7C8->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7C8->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7C8->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7C9=scs;for(0;_tmp7C9 != 0;_tmp7C9=_tmp7C9->tl){struct Cyc_Absyn_Stmt*
_tmp7CA=((struct Cyc_Absyn_Switch_clause*)_tmp7C9->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C9->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7CA,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7CC;struct _RegionHandle*_tmp7CD;struct Cyc_Toc_Env*_tmp7CB=
nv;_tmp7CC=*_tmp7CB;_tmp7CD=_tmp7CC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CD,nv,next_l),_tmp7CA);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CF;
struct _RegionHandle*_tmp7D0;struct Cyc_Toc_Env*_tmp7CE=nv;_tmp7CF=*_tmp7CE;
_tmp7D0=_tmp7CF.rgn;{struct Cyc_Toc_Env*_tmp7D1=Cyc_Toc_share_env(_tmp7D0,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7D0,Cyc_Toc_gen_label,
_tmp7D0,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D2=lscs;for(0;_tmp7D2 != 0;
_tmp7D2=_tmp7D2->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D2->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D2->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D2->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D4;struct Cyc_List_List*_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D6;struct _tuple21
_tmp7D3=Cyc_Toc_xlate_pat(_tmp7D1,_tmp7D0,_tmp7C4,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D4=_tmp7D3.f1;_tmp7D5=_tmp7D3.f2;_tmp7D6=_tmp7D3.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D7=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D4,_tmp7D7);_tmp7D6=Cyc_Absyn_seq_stmt(
_tmp7D6,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D7,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D5;{
struct Cyc_List_List*_tmpD27;nvs=((_tmpD27=_region_malloc(_tmp7D0,sizeof(*_tmpD27)),((
_tmpD27->hd=_tmp7D4,((_tmpD27->tl=nvs,_tmpD27))))));}{struct Cyc_List_List*
_tmpD28;test_stmts=((_tmpD28=_region_malloc(_tmp7D0,sizeof(*_tmpD28)),((_tmpD28->hd=
_tmp7D6,((_tmpD28->tl=test_stmts,_tmpD28))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7DB;struct _dyneither_ptr*_tmp7DC;struct _dyneither_ptr*_tmp7DD;
struct Cyc_Absyn_Switch_clause*_tmp7DE;struct _tuple24*_tmp7DA=(struct _tuple24*)
lscs->hd;_tmp7DB=*_tmp7DA;_tmp7DC=_tmp7DB.f1;_tmp7DD=_tmp7DB.f2;_tmp7DE=_tmp7DB.f3;{
struct Cyc_Toc_Env*_tmp7DF=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DE->body;struct Cyc_Toc_Env _tmp7E1;struct
_RegionHandle*_tmp7E2;struct Cyc_Toc_Env*_tmp7E0=_tmp7D1;_tmp7E1=*_tmp7E0;_tmp7E2=
_tmp7E1.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7E4;struct _dyneither_ptr*_tmp7E5;
struct Cyc_Absyn_Switch_clause*_tmp7E6;struct _tuple24*_tmp7E3=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7E4=*_tmp7E3;_tmp7E5=_tmp7E4.f2;
_tmp7E6=_tmp7E4.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7E2,
_tmp7DF,end_l,_tmp7E5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7E6->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7E2,
_tmp7DF,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7DC,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DD,s,0),0);{struct Cyc_List_List*_tmpD29;stmts=((_tmpD29=_region_malloc(
_tmp7D0,sizeof(*_tmpD29)),((_tmpD29->hd=s,((_tmpD29->tl=stmts,_tmpD29))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7E9;struct _tuple0*_tmp7EA;void*_tmp7EB;struct _tuple22*_tmp7E8=(struct
_tuple22*)decls->hd;_tmp7E9=*_tmp7E8;_tmp7EA=_tmp7E9.f1;_tmp7EB=_tmp7E9.f2;res=
Cyc_Absyn_declare_stmt(_tmp7EA,_tmp7EB,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD2A;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((
_tmpD2A->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD2A->tl=0,_tmpD2A)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7ED=s->r;struct Cyc_Absyn_Exp*_tmp7F0;struct
Cyc_Absyn_Stmt*_tmp7F2;struct Cyc_Absyn_Stmt*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F5;
struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Stmt*_tmp7F8;struct Cyc_Absyn_Stmt*
_tmp7F9;struct _tuple8 _tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Stmt*
_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FF;struct Cyc_Absyn_Stmt*_tmp801;struct Cyc_Absyn_Stmt*
_tmp803;struct Cyc_Absyn_Exp*_tmp805;struct _tuple8 _tmp806;struct Cyc_Absyn_Exp*
_tmp807;struct _tuple8 _tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Stmt*
_tmp80A;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_List_List*_tmp80D;struct Cyc_List_List*
_tmp80F;struct Cyc_Absyn_Switch_clause**_tmp810;struct Cyc_Absyn_Decl*_tmp812;
struct Cyc_Absyn_Stmt*_tmp813;struct _dyneither_ptr*_tmp815;struct Cyc_Absyn_Stmt*
_tmp816;struct Cyc_Absyn_Stmt*_tmp818;struct _tuple8 _tmp819;struct Cyc_Absyn_Exp*
_tmp81A;struct Cyc_Absyn_Stmt*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_Absyn_Exp*
_tmp81F;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp7EE=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7ED;if(_tmp7EE->tag != 0)goto _LL367;}_LL366: return;_LL367: {struct Cyc_Absyn_Exp_s_struct*
_tmp7EF=(struct Cyc_Absyn_Exp_s_struct*)_tmp7ED;if(_tmp7EF->tag != 1)goto _LL369;
else{_tmp7F0=_tmp7EF->f1;}}_LL368: Cyc_Toc_exp_to_c(nv,_tmp7F0);return;_LL369: {
struct Cyc_Absyn_Seq_s_struct*_tmp7F1=(struct Cyc_Absyn_Seq_s_struct*)_tmp7ED;if(
_tmp7F1->tag != 2)goto _LL36B;else{_tmp7F2=_tmp7F1->f1;_tmp7F3=_tmp7F1->f2;}}
_LL36A: Cyc_Toc_stmt_to_c(nv,_tmp7F2);s=_tmp7F3;continue;_LL36B: {struct Cyc_Absyn_Return_s_struct*
_tmp7F4=(struct Cyc_Absyn_Return_s_struct*)_tmp7ED;if(_tmp7F4->tag != 3)goto _LL36D;
else{_tmp7F5=_tmp7F4->f1;}}_LL36C: {struct Cyc_Core_Opt*topt=0;if(_tmp7F5 != 0){{
struct Cyc_Core_Opt*_tmpD2B;topt=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F5->topt))->v),
_tmpD2B))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7F5);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp821=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp822=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp821,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp821,(void*)topt->v,_tmp7F5,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp822,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F6=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7ED;if(_tmp7F6->tag != 4)goto _LL36F;
else{_tmp7F7=_tmp7F6->f1;_tmp7F8=_tmp7F6->f2;_tmp7F9=_tmp7F6->f3;}}_LL36E: Cyc_Toc_exp_to_c(
nv,_tmp7F7);Cyc_Toc_stmt_to_c(nv,_tmp7F8);s=_tmp7F9;continue;_LL36F: {struct Cyc_Absyn_While_s_struct*
_tmp7FA=(struct Cyc_Absyn_While_s_struct*)_tmp7ED;if(_tmp7FA->tag != 5)goto _LL371;
else{_tmp7FB=_tmp7FA->f1;_tmp7FC=_tmp7FB.f1;_tmp7FD=_tmp7FA->f2;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7FC);{struct Cyc_Toc_Env _tmp824;struct _RegionHandle*_tmp825;struct Cyc_Toc_Env*
_tmp823=nv;_tmp824=*_tmp823;_tmp825=_tmp824.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp825,nv),_tmp7FD);return;};_LL371: {struct Cyc_Absyn_Break_s_struct*_tmp7FE=(
struct Cyc_Absyn_Break_s_struct*)_tmp7ED;if(_tmp7FE->tag != 6)goto _LL373;else{
_tmp7FF=_tmp7FE->f1;}}_LL372: {struct Cyc_Toc_Env _tmp827;struct _dyneither_ptr**
_tmp828;struct Cyc_Toc_Env*_tmp826=nv;_tmp827=*_tmp826;_tmp828=_tmp827.break_lab;
if(_tmp828 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp828,0);{int dest_depth=_tmp7FF == 0?0:
_tmp7FF->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL373: {struct Cyc_Absyn_Continue_s_struct*_tmp800=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7ED;if(_tmp800->tag != 7)goto _LL375;else{_tmp801=_tmp800->f1;}}_LL374: {
struct Cyc_Toc_Env _tmp82A;struct _dyneither_ptr**_tmp82B;struct Cyc_Toc_Env*_tmp829=
nv;_tmp82A=*_tmp829;_tmp82B=_tmp82A.continue_lab;if(_tmp82B != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp82B,0);_tmp803=_tmp801;goto _LL376;}_LL375: {struct Cyc_Absyn_Goto_s_struct*
_tmp802=(struct Cyc_Absyn_Goto_s_struct*)_tmp7ED;if(_tmp802->tag != 8)goto _LL377;
else{_tmp803=_tmp802->f2;}}_LL376: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp803))->try_depth,s);return;_LL377: {struct Cyc_Absyn_For_s_struct*
_tmp804=(struct Cyc_Absyn_For_s_struct*)_tmp7ED;if(_tmp804->tag != 9)goto _LL379;
else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;_tmp807=_tmp806.f1;_tmp808=_tmp804->f3;
_tmp809=_tmp808.f1;_tmp80A=_tmp804->f4;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp805);Cyc_Toc_exp_to_c(
nv,_tmp807);Cyc_Toc_exp_to_c(nv,_tmp809);{struct Cyc_Toc_Env _tmp82D;struct
_RegionHandle*_tmp82E;struct Cyc_Toc_Env*_tmp82C=nv;_tmp82D=*_tmp82C;_tmp82E=
_tmp82D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82E,nv),_tmp80A);return;};
_LL379: {struct Cyc_Absyn_Switch_s_struct*_tmp80B=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7ED;if(_tmp80B->tag != 10)goto _LL37B;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}
_LL37A: Cyc_Toc_xlate_switch(nv,s,_tmp80C,_tmp80D);return;_LL37B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80E=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7ED;if(_tmp80E->tag != 11)goto
_LL37D;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}_LL37C: {struct Cyc_Toc_Env
_tmp830;struct Cyc_Toc_FallthruInfo*_tmp831;struct Cyc_Toc_Env*_tmp82F=nv;_tmp830=*
_tmp82F;_tmp831=_tmp830.fallthru_info;if(_tmp831 == 0){const char*_tmpD2E;void*
_tmpD2D;(_tmpD2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD2E="fallthru in unexpected place",_tag_dyneither(_tmpD2E,sizeof(char),29))),
_tag_dyneither(_tmpD2D,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp835;struct
Cyc_List_List*_tmp836;struct Cyc_Dict_Dict _tmp837;struct Cyc_Toc_FallthruInfo
_tmp834=*_tmp831;_tmp835=_tmp834.label;_tmp836=_tmp834.binders;_tmp837=_tmp834.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp835,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp810)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp838=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp836);struct Cyc_List_List*_tmp839=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80F);for(0;_tmp838 != 0;(_tmp838=_tmp838->tl,
_tmp839=_tmp839->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp839))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp837,(
struct _tuple0*)_tmp838->hd),(struct Cyc_Absyn_Exp*)_tmp839->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37D: {struct Cyc_Absyn_Decl_s_struct*_tmp811=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7ED;if(_tmp811->tag != 12)goto _LL37F;else{_tmp812=_tmp811->f1;_tmp813=_tmp811->f2;}}
_LL37E:{void*_tmp83A=_tmp812->r;struct Cyc_Absyn_Vardecl*_tmp83C;struct Cyc_Absyn_Pat*
_tmp83E;struct Cyc_Absyn_Exp*_tmp83F;struct Cyc_List_List*_tmp841;struct Cyc_Absyn_Fndecl*
_tmp843;struct Cyc_Absyn_Tvar*_tmp845;struct Cyc_Absyn_Vardecl*_tmp846;int _tmp847;
struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_Absyn_Tvar*
_tmp84B;struct Cyc_Absyn_Vardecl*_tmp84C;_LL388: {struct Cyc_Absyn_Var_d_struct*
_tmp83B=(struct Cyc_Absyn_Var_d_struct*)_tmp83A;if(_tmp83B->tag != 0)goto _LL38A;
else{_tmp83C=_tmp83B->f1;}}_LL389: {struct Cyc_Toc_Env _tmp84E;struct _RegionHandle*
_tmp84F;struct Cyc_Toc_Env*_tmp84D=nv;_tmp84E=*_tmp84D;_tmp84F=_tmp84E.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD31;struct Cyc_Absyn_Local_b_struct*_tmpD30;
struct Cyc_Toc_Env*_tmp850=Cyc_Toc_add_varmap(_tmp84F,nv,_tmp83C->name,Cyc_Absyn_varb_exp(
_tmp83C->name,(void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((
_tmpD31.tag=4,((_tmpD31.f1=_tmp83C,_tmpD31)))),_tmpD30)))),0));Cyc_Toc_local_decl_to_c(
_tmp850,_tmp850,_tmp83C,_tmp813);}goto _LL387;}_LL38A: {struct Cyc_Absyn_Let_d_struct*
_tmp83D=(struct Cyc_Absyn_Let_d_struct*)_tmp83A;if(_tmp83D->tag != 2)goto _LL38C;
else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f3;}}_LL38B:{void*_tmp853=_tmp83E->r;
struct Cyc_Absyn_Vardecl*_tmp855;struct Cyc_Absyn_Pat*_tmp856;struct Cyc_Absyn_Pat
_tmp857;void*_tmp858;_LL397: {struct Cyc_Absyn_Var_p_struct*_tmp854=(struct Cyc_Absyn_Var_p_struct*)
_tmp853;if(_tmp854->tag != 1)goto _LL399;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;
_tmp857=*_tmp856;_tmp858=_tmp857.r;{struct Cyc_Absyn_Wild_p_struct*_tmp859=(
struct Cyc_Absyn_Wild_p_struct*)_tmp858;if(_tmp859->tag != 0)goto _LL399;};}}_LL398: {
struct _tuple0*old_name=_tmp855->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp855->name=new_name;_tmp855->initializer=(struct Cyc_Absyn_Exp*)_tmp83F;{
struct Cyc_Absyn_Var_d_struct _tmpD34;struct Cyc_Absyn_Var_d_struct*_tmpD33;_tmp812->r=(
void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=0,((
_tmpD34.f1=_tmp855,_tmpD34)))),_tmpD33))));}{struct Cyc_Toc_Env _tmp85D;struct
_RegionHandle*_tmp85E;struct Cyc_Toc_Env*_tmp85C=nv;_tmp85D=*_tmp85C;_tmp85E=
_tmp85D.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD37;struct Cyc_Absyn_Local_b_struct*
_tmpD36;struct Cyc_Toc_Env*_tmp85F=Cyc_Toc_add_varmap(_tmp85E,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=
4,((_tmpD37.f1=_tmp855,_tmpD37)))),_tmpD36)))),0));Cyc_Toc_local_decl_to_c(
_tmp85F,nv,_tmp855,_tmp813);}goto _LL396;};}_LL399:;_LL39A: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp83E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83F->topt))->v,_tmp83F,
_tmp813))->r;goto _LL396;_LL396:;}goto _LL387;_LL38C: {struct Cyc_Absyn_Letv_d_struct*
_tmp840=(struct Cyc_Absyn_Letv_d_struct*)_tmp83A;if(_tmp840->tag != 3)goto _LL38E;
else{_tmp841=_tmp840->f1;}}_LL38D: {struct Cyc_List_List*_tmp862=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp841);if(_tmp862 == 0){const char*_tmpD3A;
void*_tmpD39;(_tmpD39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD3A="empty Letv_d",_tag_dyneither(_tmpD3A,sizeof(char),
13))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD3D;struct Cyc_Absyn_Var_d_struct*_tmpD3C;_tmp812->r=(void*)((_tmpD3C=
_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3D.tag=0,((_tmpD3D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp862->hd,_tmpD3D)))),_tmpD3C))));}_tmp862=_tmp862->tl;for(0;_tmp862 != 0;
_tmp862=_tmp862->tl){struct Cyc_Absyn_Var_d_struct _tmpD40;struct Cyc_Absyn_Var_d_struct*
_tmpD3F;struct Cyc_Absyn_Decl*_tmp867=Cyc_Absyn_new_decl((void*)((_tmpD3F=
_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=0,((_tmpD40.f1=(struct Cyc_Absyn_Vardecl*)
_tmp862->hd,_tmpD40)))),_tmpD3F)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp867,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL387;}_LL38E: {struct Cyc_Absyn_Fn_d_struct*
_tmp842=(struct Cyc_Absyn_Fn_d_struct*)_tmp83A;if(_tmp842->tag != 1)goto _LL390;
else{_tmp843=_tmp842->f1;}}_LL38F: {struct _tuple0*_tmp86A=_tmp843->name;struct
Cyc_Toc_Env _tmp86C;struct _RegionHandle*_tmp86D;struct Cyc_Toc_Env*_tmp86B=nv;
_tmp86C=*_tmp86B;_tmp86D=_tmp86C.rgn;{struct Cyc_Toc_Env*_tmp86E=Cyc_Toc_add_varmap(
_tmp86D,nv,_tmp843->name,Cyc_Absyn_var_exp(_tmp86A,0));Cyc_Toc_fndecl_to_c(
_tmp86E,_tmp843,0);Cyc_Toc_stmt_to_c(_tmp86E,_tmp813);}goto _LL387;}_LL390: {
struct Cyc_Absyn_Region_d_struct*_tmp844=(struct Cyc_Absyn_Region_d_struct*)
_tmp83A;if(_tmp844->tag != 4)goto _LL392;else{_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;
_tmp847=_tmp844->f3;_tmp848=_tmp844->f4;}}_LL391: {struct Cyc_Absyn_Stmt*_tmp86F=
_tmp813;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp846->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp871;struct _RegionHandle*_tmp872;struct Cyc_Toc_Env*
_tmp870=nv;_tmp871=*_tmp870;_tmp872=_tmp871.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp872,nv,x_var,x_exp),_tmp86F);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86F,0))->r;
else{if(_tmp848 == 0){struct Cyc_Absyn_Exp*_tmpD43[1];struct Cyc_Absyn_Exp*_tmpD42[
1];struct Cyc_List_List*_tmpD41;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD41=
_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD41->tl=0,_tmpD41)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD42[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD42,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp86F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD43[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD43,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp848);{struct Cyc_Absyn_Exp*
_tmpD45[1];struct Cyc_Absyn_Exp*_tmpD44[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD44[1]=(struct Cyc_Absyn_Exp*)_tmp848,((_tmpD44[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD44,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp86F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD45[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD45,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL392: {struct Cyc_Absyn_Alias_d_struct*_tmp849=(struct Cyc_Absyn_Alias_d_struct*)
_tmp83A;if(_tmp849->tag != 5)goto _LL394;else{_tmp84A=_tmp849->f1;_tmp84B=_tmp849->f2;
_tmp84C=_tmp849->f3;}}_LL393: {struct _tuple0*old_name=_tmp84C->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp84C->name=new_name;_tmp84C->initializer=(
struct Cyc_Absyn_Exp*)_tmp84A;{struct Cyc_Absyn_Decl_s_struct _tmpD54;struct Cyc_Absyn_Var_d_struct*
_tmpD53;struct Cyc_Absyn_Var_d_struct _tmpD52;struct Cyc_Absyn_Decl*_tmpD51;struct
Cyc_Absyn_Decl_s_struct*_tmpD50;s->r=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((
_tmpD50[0]=((_tmpD54.tag=12,((_tmpD54.f1=((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((
_tmpD51->r=(void*)((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD52.tag=
0,((_tmpD52.f1=_tmp84C,_tmpD52)))),_tmpD53)))),((_tmpD51->loc=0,_tmpD51)))))),((
_tmpD54.f2=_tmp813,_tmpD54)))))),_tmpD50))));}{struct Cyc_Toc_Env _tmp87E;struct
_RegionHandle*_tmp87F;struct Cyc_Toc_Env*_tmp87D=nv;_tmp87E=*_tmp87D;_tmp87F=
_tmp87E.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD57;struct Cyc_Absyn_Local_b_struct*
_tmpD56;struct Cyc_Toc_Env*_tmp880=Cyc_Toc_add_varmap(_tmp87F,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD57.tag=
4,((_tmpD57.f1=_tmp84C,_tmpD57)))),_tmpD56)))),0));Cyc_Toc_local_decl_to_c(
_tmp880,nv,_tmp84C,_tmp813);}return;};}_LL394:;_LL395: {const char*_tmpD5A;void*
_tmpD59;(_tmpD59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD5A="bad nested declaration within function",_tag_dyneither(_tmpD5A,sizeof(
char),39))),_tag_dyneither(_tmpD59,sizeof(void*),0)));}_LL387:;}return;_LL37F: {
struct Cyc_Absyn_Label_s_struct*_tmp814=(struct Cyc_Absyn_Label_s_struct*)_tmp7ED;
if(_tmp814->tag != 13)goto _LL381;else{_tmp815=_tmp814->f1;_tmp816=_tmp814->f2;}}
_LL380: s=_tmp816;continue;_LL381: {struct Cyc_Absyn_Do_s_struct*_tmp817=(struct
Cyc_Absyn_Do_s_struct*)_tmp7ED;if(_tmp817->tag != 14)goto _LL383;else{_tmp818=
_tmp817->f1;_tmp819=_tmp817->f2;_tmp81A=_tmp819.f1;}}_LL382: {struct Cyc_Toc_Env
_tmp886;struct _RegionHandle*_tmp887;struct Cyc_Toc_Env*_tmp885=nv;_tmp886=*
_tmp885;_tmp887=_tmp886.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp887,nv),
_tmp818);Cyc_Toc_exp_to_c(nv,_tmp81A);return;}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp81B=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7ED;if(_tmp81B->tag != 15)goto
_LL385;else{_tmp81C=_tmp81B->f1;_tmp81D=_tmp81B->f2;}}_LL384: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD5B;
e_exp->topt=((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->v=e_typ,_tmpD5B))));}{
struct Cyc_Toc_Env _tmp88A;struct _RegionHandle*_tmp88B;struct Cyc_Toc_Env*_tmp889=
nv;_tmp88A=*_tmp889;_tmp88B=_tmp88A.rgn;{struct Cyc_Toc_Env*_tmp88C=Cyc_Toc_add_varmap(
_tmp88B,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp88C,_tmp81C);{struct Cyc_Absyn_Stmt*
_tmp88D=Cyc_Absyn_seq_stmt(_tmp81C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp88E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88F=
Cyc_Absyn_var_exp(_tmp88E,0);struct Cyc_Absyn_Vardecl*_tmp890=Cyc_Absyn_new_vardecl(
_tmp88E,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD5C;_tmp88F->topt=((
_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->v=Cyc_Absyn_exn_typ(),_tmpD5C))));}{
struct Cyc_Core_Opt*_tmpD6D;struct Cyc_Absyn_Var_p_struct*_tmpD6C;struct Cyc_Absyn_Pat*
_tmpD6B;struct Cyc_Core_Opt*_tmpD6A;struct Cyc_Absyn_Var_p_struct _tmpD69;struct Cyc_Absyn_Pat*
_tmpD68;struct Cyc_Absyn_Pat*_tmp892=(_tmpD68=_cycalloc(sizeof(*_tmpD68)),((
_tmpD68->r=(void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD69.tag=
1,((_tmpD69.f1=_tmp890,((_tmpD69.f2=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((
_tmpD6B->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD6B->topt=((_tmpD6A=_cycalloc(
sizeof(*_tmpD6A)),((_tmpD6A->v=Cyc_Absyn_exn_typ(),_tmpD6A)))),((_tmpD6B->loc=0,
_tmpD6B)))))))),_tmpD69)))))),_tmpD6C)))),((_tmpD68->topt=((_tmpD6D=_cycalloc(
sizeof(*_tmpD6D)),((_tmpD6D->v=Cyc_Absyn_exn_typ(),_tmpD6D)))),((_tmpD68->loc=0,
_tmpD68)))))));struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_throw_exp(_tmp88F,0);{
struct Cyc_Core_Opt*_tmpD6E;_tmp893->topt=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((
_tmpD6E->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD6E))));}{struct Cyc_Absyn_Stmt*
_tmp895=Cyc_Absyn_exp_stmt(_tmp893,0);struct Cyc_Core_Opt*_tmpD74;struct Cyc_List_List*
_tmpD73;struct Cyc_Absyn_Switch_clause*_tmpD72;struct Cyc_Absyn_Switch_clause*
_tmp896=(_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->pattern=_tmp892,((
_tmpD72->pat_vars=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->v=((_tmpD73=
_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=_tmp890,((_tmpD73->tl=0,_tmpD73)))))),
_tmpD74)))),((_tmpD72->where_clause=0,((_tmpD72->body=_tmp895,((_tmpD72->loc=0,
_tmpD72)))))))))));struct Cyc_List_List*_tmpD75;struct Cyc_Absyn_Stmt*_tmp897=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp81D,((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=_tmp896,((_tmpD75->tl=
0,_tmpD75))))))),0);Cyc_Toc_stmt_to_c(_tmp88C,_tmp897);{struct Cyc_List_List*
_tmpD76;struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD76->tl=0,_tmpD76)))))),0);struct Cyc_List_List*
_tmpD77;struct Cyc_Absyn_Stmt*_tmp899=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD77->tl=0,_tmpD77)))))),0),0);struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp899,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp89A,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp898,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp89B,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88D,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp897,0),0),0),0),0),0))->r;};};};};}
return;};}_LL385: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81E=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7ED;if(_tmp81E->tag != 16)goto _LL364;else{_tmp81F=_tmp81E->f1;}}_LL386: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81F);{struct Cyc_List_List*
_tmpD78;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->hd=_tmp81F,((_tmpD78->tl=0,
_tmpD78)))))),0));};}return;_LL364:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A9=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A9;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8AA=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8AB=f->args;for(0;_tmp8AB != 0;_tmp8AB=_tmp8AB->tl){struct
_tuple0*_tmpD79;struct _tuple0*_tmp8AC=(_tmpD79=_cycalloc(sizeof(*_tmpD79)),((
_tmpD79->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD79->f2=(*((struct
_tuple25*)_tmp8AB->hd)).f1,_tmpD79)))));(*((struct _tuple25*)_tmp8AB->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8AB->hd)).f3);_tmp8AA=Cyc_Toc_add_varmap(frgn,_tmp8AA,
_tmp8AC,Cyc_Absyn_var_exp(_tmp8AC,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8AF;struct Cyc_Absyn_Tqual
_tmp8B0;void*_tmp8B1;int _tmp8B2;struct Cyc_Absyn_VarargInfo _tmp8AE=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AF=_tmp8AE.name;_tmp8B0=_tmp8AE.tq;_tmp8B1=
_tmp8AE.type;_tmp8B2=_tmp8AE.inject;{void*_tmp8B3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8B0,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD7A;struct _tuple0*_tmp8B4=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD7A->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8AF))->v,_tmpD7A)))));{
struct _tuple25*_tmpD7D;struct Cyc_List_List*_tmpD7C;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD7C=
_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->hd=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((
_tmpD7D->f1=(struct _dyneither_ptr*)_tmp8AF->v,((_tmpD7D->f2=_tmp8B0,((_tmpD7D->f3=
_tmp8B3,_tmpD7D)))))))),((_tmpD7C->tl=0,_tmpD7C)))))));}_tmp8AA=Cyc_Toc_add_varmap(
frgn,_tmp8AA,_tmp8B4,Cyc_Absyn_var_exp(_tmp8B4,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8B8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8B8 != 0;_tmp8B8=_tmp8B8->tl){((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8AA),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL39B: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39C: return Cyc_Absyn_Extern;
default: _LL39D: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8B9=ad->name;struct _DynRegionHandle*_tmp8BB;struct Cyc_Dict_Dict*_tmp8BC;
struct Cyc_Toc_TocState _tmp8BA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8BB=_tmp8BA.dyn;_tmp8BC=_tmp8BA.aggrs_so_far;{struct _DynRegionFrame _tmp8BD;
struct _RegionHandle*d=_open_dynregion(& _tmp8BD,_tmp8BB);{int seen_defn_before;
struct _tuple14**_tmp8BE=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8BC,_tmp8B9);if(_tmp8BE == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD7E;v=((
_tmpD7E=_region_malloc(d,sizeof(*_tmpD7E)),((_tmpD7E->f1=ad,((_tmpD7E->f2=Cyc_Absyn_strctq(
_tmp8B9),_tmpD7E))))));}else{struct _tuple14*_tmpD7F;v=((_tmpD7F=_region_malloc(d,
sizeof(*_tmpD7F)),((_tmpD7F->f1=ad,((_tmpD7F->f2=Cyc_Absyn_unionq_typ(_tmp8B9),
_tmpD7F))))));}*_tmp8BC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8BC,_tmp8B9,v);};}else{struct
_tuple14 _tmp8C2;struct Cyc_Absyn_Aggrdecl*_tmp8C3;void*_tmp8C4;struct _tuple14*
_tmp8C1=*_tmp8BE;_tmp8C2=*_tmp8C1;_tmp8C3=_tmp8C2.f1;_tmp8C4=_tmp8C2.f2;if(
_tmp8C3->impl == 0){{struct _tuple14*_tmpD80;*_tmp8BC=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8BC,
_tmp8B9,((_tmpD80=_region_malloc(d,sizeof(*_tmpD80)),((_tmpD80->f1=ad,((_tmpD80->f2=
_tmp8C4,_tmpD80)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD81;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD81=_cycalloc(sizeof(*_tmpD81)),((
_tmpD81->kind=ad->kind,((_tmpD81->sc=Cyc_Absyn_Public,((_tmpD81->name=ad->name,((
_tmpD81->tvs=0,((_tmpD81->impl=0,((_tmpD81->attributes=ad->attributes,_tmpD81)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD82;
new_ad->impl=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->exist_vars=0,((
_tmpD82->rgn_po=0,((_tmpD82->fields=0,((_tmpD82->tagged=0,_tmpD82))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8C7=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8C7 != 0;_tmp8C7=_tmp8C7->tl){struct Cyc_Absyn_Aggrfield*
_tmp8C8=(struct Cyc_Absyn_Aggrfield*)_tmp8C7->hd;struct Cyc_Absyn_Aggrfield*
_tmpD83;struct Cyc_Absyn_Aggrfield*_tmp8C9=(_tmpD83=_cycalloc(sizeof(*_tmpD83)),((
_tmpD83->name=_tmp8C8->name,((_tmpD83->tq=Cyc_Toc_mt_tq,((_tmpD83->type=Cyc_Toc_typ_to_c_array(
_tmp8C8->type),((_tmpD83->width=_tmp8C8->width,((_tmpD83->attributes=_tmp8C8->attributes,
_tmpD83)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8CA=_tmp8C9->type;struct _dyneither_ptr*_tmp8CB=_tmp8C9->name;const char*
_tmpD88;void*_tmpD87[2];struct Cyc_String_pa_struct _tmpD86;struct Cyc_String_pa_struct
_tmpD85;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD85.tag=0,((_tmpD85.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8CB),((_tmpD86.tag=0,((_tmpD86.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD87[0]=&
_tmpD86,((_tmpD87[1]=& _tmpD85,Cyc_aprintf(((_tmpD88="_union_%s_%s",
_tag_dyneither(_tmpD88,sizeof(char),13))),_tag_dyneither(_tmpD87,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD89;struct _dyneither_ptr*str=(_tmpD89=_cycalloc(sizeof(*
_tmpD89)),((_tmpD89[0]=s,_tmpD89)));struct Cyc_Absyn_Aggrfield*_tmpD8A;struct Cyc_Absyn_Aggrfield*
_tmp8CC=(_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->name=Cyc_Toc_val_sp,((
_tmpD8A->tq=Cyc_Toc_mt_tq,((_tmpD8A->type=_tmp8CA,((_tmpD8A->width=0,((_tmpD8A->attributes=
0,_tmpD8A)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD8B;struct Cyc_Absyn_Aggrfield*
_tmp8CD=(_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->name=Cyc_Toc_tag_sp,((
_tmpD8B->tq=Cyc_Toc_mt_tq,((_tmpD8B->type=Cyc_Absyn_sint_typ,((_tmpD8B->width=0,((
_tmpD8B->attributes=0,_tmpD8B)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD8C[2];
struct Cyc_List_List*_tmp8CE=(_tmpD8C[1]=_tmp8CC,((_tmpD8C[0]=_tmp8CD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD8C,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD91;struct _tuple0*_tmpD90;struct Cyc_Absyn_Aggrdecl*
_tmpD8F;struct Cyc_Absyn_Aggrdecl*_tmp8CF=(_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((
_tmpD8F->kind=Cyc_Absyn_StructA,((_tmpD8F->sc=Cyc_Absyn_Public,((_tmpD8F->name=((
_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->f1=Cyc_Absyn_Loc_n,((_tmpD90->f2=
str,_tmpD90)))))),((_tmpD8F->tvs=0,((_tmpD8F->impl=((_tmpD91=_cycalloc(sizeof(*
_tmpD91)),((_tmpD91->exist_vars=0,((_tmpD91->rgn_po=0,((_tmpD91->fields=_tmp8CE,((
_tmpD91->tagged=0,_tmpD91)))))))))),((_tmpD8F->attributes=0,_tmpD8F)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD97;struct Cyc_Absyn_Aggr_d_struct _tmpD96;
struct Cyc_List_List*_tmpD95;Cyc_Toc_result_decls=((_tmpD95=_cycalloc(sizeof(*
_tmpD95)),((_tmpD95->hd=Cyc_Absyn_new_decl((void*)((_tmpD97=_cycalloc(sizeof(*
_tmpD97)),((_tmpD97[0]=((_tmpD96.tag=6,((_tmpD96.f1=_tmp8CF,_tmpD96)))),_tmpD97)))),
0),((_tmpD95->tl=Cyc_Toc_result_decls,_tmpD95))))));}_tmp8C9->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD98;new_fields=((_tmpD98=_cycalloc(sizeof(*
_tmpD98)),((_tmpD98->hd=_tmp8C9,((_tmpD98->tl=new_fields,_tmpD98))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDA2;struct Cyc_Absyn_Aggr_d_struct
_tmpDA1;struct Cyc_Absyn_Aggr_d_struct*_tmpDA0;struct Cyc_List_List*_tmpD9F;Cyc_Toc_result_decls=((
_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->hd=((_tmpDA2=_cycalloc(sizeof(*
_tmpDA2)),((_tmpDA2->r=(void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((
_tmpDA1.tag=6,((_tmpDA1.f1=new_ad,_tmpDA1)))),_tmpDA0)))),((_tmpDA2->loc=0,
_tmpDA2)))))),((_tmpD9F->tl=Cyc_Toc_result_decls,_tmpD9F))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8E6;struct Cyc_Set_Set**_tmp8E7;struct Cyc_Toc_TocState
_tmp8E5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8E6=
_tmp8E5.dyn;_tmp8E7=_tmp8E5.datatypes_so_far;{struct _DynRegionFrame _tmp8E8;
struct _RegionHandle*d=_open_dynregion(& _tmp8E8,_tmp8E6);{struct _tuple0*_tmp8E9=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8E7,_tmp8E9)){_npop_handler(0);return;}*_tmp8E7=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8E7,_tmp8E9);}{struct Cyc_List_List*_tmp8EA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8EA->hd;struct Cyc_List_List*_tmp8EB=0;int i=
1;{struct Cyc_List_List*_tmp8EC=f->typs;for(0;_tmp8EC != 0;(_tmp8EC=_tmp8EC->tl,i
++)){struct _dyneither_ptr*_tmp8ED=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDA3;struct Cyc_Absyn_Aggrfield*_tmp8EE=(_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((
_tmpDA3->name=_tmp8ED,((_tmpDA3->tq=(*((struct _tuple9*)_tmp8EC->hd)).f1,((
_tmpDA3->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8EC->hd)).f2),((
_tmpDA3->width=0,((_tmpDA3->attributes=0,_tmpDA3)))))))))));struct Cyc_List_List*
_tmpDA4;_tmp8EB=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->hd=_tmp8EE,((
_tmpDA4->tl=_tmp8EB,_tmpDA4))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDA7;struct Cyc_List_List*
_tmpDA6;_tmp8EB=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->hd=((_tmpDA7=
_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->name=Cyc_Toc_tag_sp,((_tmpDA7->tq=Cyc_Toc_mt_tq,((
_tmpDA7->type=Cyc_Absyn_sint_typ,((_tmpDA7->width=0,((_tmpDA7->attributes=0,
_tmpDA7)))))))))))),((_tmpDA6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8EB),_tmpDA6))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDAC;const char*_tmpDAB;struct Cyc_Absyn_Aggrdecl*_tmpDAA;struct Cyc_Absyn_Aggrdecl*
_tmp8F3=(_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->kind=Cyc_Absyn_StructA,((
_tmpDAA->sc=Cyc_Absyn_Public,((_tmpDAA->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDAB="_struct",_tag_dyneither(_tmpDAB,sizeof(char),8)))),((_tmpDAA->tvs=0,((
_tmpDAA->impl=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->exist_vars=0,((
_tmpDAC->rgn_po=0,((_tmpDAC->fields=_tmp8EB,((_tmpDAC->tagged=0,_tmpDAC)))))))))),((
_tmpDAA->attributes=0,_tmpDAA)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDB2;
struct Cyc_Absyn_Aggr_d_struct _tmpDB1;struct Cyc_List_List*_tmpDB0;Cyc_Toc_result_decls=((
_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->hd=Cyc_Absyn_new_decl((void*)((
_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=((_tmpDB1.tag=6,((_tmpDB1.f1=
_tmp8F3,_tmpDB1)))),_tmpDB2)))),0),((_tmpDB0->tl=Cyc_Toc_result_decls,_tmpDB0))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp8FB;struct Cyc_Dict_Dict*_tmp8FC;struct
Cyc_Toc_TocState _tmp8FA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8FB=_tmp8FA.dyn;_tmp8FC=_tmp8FA.xdatatypes_so_far;{struct _DynRegionFrame
_tmp8FD;struct _RegionHandle*d=_open_dynregion(& _tmp8FD,_tmp8FB);{struct _tuple0*
_tmp8FE=xd->name;struct Cyc_List_List*_tmp8FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FF != 0;_tmp8FF=_tmp8FF->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FF->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp901=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp900,Cyc_Absyn_false_conref,0);int*_tmp902=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8FC,f->name);int
_tmp903;_LL3A0: if(_tmp902 != 0)goto _LL3A2;_LL3A1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp904=Cyc_Absyn_new_vardecl(f->name,_tmp901,
initopt);_tmp904->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDB8;struct Cyc_Absyn_Var_d_struct
_tmpDB7;struct Cyc_List_List*_tmpDB6;Cyc_Toc_result_decls=((_tmpDB6=_cycalloc(
sizeof(*_tmpDB6)),((_tmpDB6->hd=Cyc_Absyn_new_decl((void*)((_tmpDB8=_cycalloc(
sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB7.tag=0,((_tmpDB7.f1=_tmp904,_tmpDB7)))),
_tmpDB8)))),0),((_tmpDB6->tl=Cyc_Toc_result_decls,_tmpDB6))))));}*_tmp8FC=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp8FC,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp908=f->typs;for(0;_tmp908 != 0;(_tmp908=_tmp908->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDC0;void*_tmpDBF[1];const char*_tmpDBE;struct
_dyneither_ptr*_tmpDBD;struct _dyneither_ptr*_tmp909=(_tmpDBD=_cycalloc(sizeof(*
_tmpDBD)),((_tmpDBD[0]=(struct _dyneither_ptr)((_tmpDC0.tag=1,((_tmpDC0.f1=(
unsigned long)i,((_tmpDBF[0]=& _tmpDC0,Cyc_aprintf(((_tmpDBE="f%d",_tag_dyneither(
_tmpDBE,sizeof(char),4))),_tag_dyneither(_tmpDBF,sizeof(void*),1)))))))),_tmpDBD)));
struct Cyc_Absyn_Aggrfield*_tmpDC1;struct Cyc_Absyn_Aggrfield*_tmp90A=(_tmpDC1=
_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->name=_tmp909,((_tmpDC1->tq=(*((struct
_tuple9*)_tmp908->hd)).f1,((_tmpDC1->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp908->hd)).f2),((_tmpDC1->width=0,((_tmpDC1->attributes=0,_tmpDC1)))))))))));
struct Cyc_List_List*_tmpDC2;fields=((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((
_tmpDC2->hd=_tmp90A,((_tmpDC2->tl=fields,_tmpDC2))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDC5;struct Cyc_List_List*_tmpDC4;fields=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((
_tmpDC4->hd=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->name=Cyc_Toc_tag_sp,((
_tmpDC5->tq=Cyc_Toc_mt_tq,((_tmpDC5->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDC5->width=0,((_tmpDC5->attributes=0,_tmpDC5)))))))))))),((
_tmpDC4->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDC4))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDCA;const char*_tmpDC9;
struct Cyc_Absyn_Aggrdecl*_tmpDC8;struct Cyc_Absyn_Aggrdecl*_tmp913=(_tmpDC8=
_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->kind=Cyc_Absyn_StructA,((_tmpDC8->sc=Cyc_Absyn_Public,((
_tmpDC8->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDC9="_struct",
_tag_dyneither(_tmpDC9,sizeof(char),8)))),((_tmpDC8->tvs=0,((_tmpDC8->impl=((
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->exist_vars=0,((_tmpDCA->rgn_po=0,((
_tmpDCA->fields=fields,((_tmpDCA->tagged=0,_tmpDCA)))))))))),((_tmpDC8->attributes=
0,_tmpDC8)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDD0;struct Cyc_Absyn_Aggr_d_struct
_tmpDCF;struct Cyc_List_List*_tmpDCE;Cyc_Toc_result_decls=((_tmpDCE=_cycalloc(
sizeof(*_tmpDCE)),((_tmpDCE->hd=Cyc_Absyn_new_decl((void*)((_tmpDD0=_cycalloc(
sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDCF.tag=6,((_tmpDCF.f1=_tmp913,_tmpDCF)))),
_tmpDD0)))),0),((_tmpDCE->tl=Cyc_Toc_result_decls,_tmpDCE))))));}goto _LL39F;};};};}
_LL3A2: if(_tmp902 == 0)goto _LL3A4;_tmp903=*_tmp902;if(_tmp903 != 0)goto _LL3A4;
_LL3A3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp91B=Cyc_Absyn_new_vardecl(f->name,_tmp901,(
struct Cyc_Absyn_Exp*)_tmp91A);_tmp91B->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDD6;struct Cyc_Absyn_Var_d_struct _tmpDD5;struct Cyc_List_List*_tmpDD4;Cyc_Toc_result_decls=((
_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->hd=Cyc_Absyn_new_decl((void*)((
_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD5.tag=0,((_tmpDD5.f1=
_tmp91B,_tmpDD5)))),_tmpDD6)))),0),((_tmpDD4->tl=Cyc_Toc_result_decls,_tmpDD4))))));}*
_tmp8FC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp8FC,f->name,1);}goto _LL39F;_LL3A4:;_LL3A5: goto _LL39F;_LL39F:;}};
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
void*_tmp91F=init->r;struct Cyc_Absyn_Vardecl*_tmp921;struct Cyc_Absyn_Exp*_tmp922;
struct Cyc_Absyn_Exp*_tmp923;int _tmp924;_LL3A7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp920=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp91F;if(_tmp920->tag != 28)
goto _LL3A9;else{_tmp921=_tmp920->f1;_tmp922=_tmp920->f2;_tmp923=_tmp920->f3;
_tmp924=_tmp920->f4;}}_LL3A8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp921,_tmp922,_tmp923,_tmp924,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A6;_LL3A9:;_LL3AA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp926;struct _RegionHandle*_tmp927;struct Cyc_Toc_Env*_tmp925=init_nv;_tmp926=*
_tmp925;_tmp927=_tmp926.rgn;{struct Cyc_Toc_Env*_tmp928=Cyc_Toc_set_toplevel(
_tmp927,init_nv);Cyc_Toc_exp_to_c(_tmp928,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A6;_LL3A6:;}else{void*_tmp929=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp92B;void*_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;union Cyc_Absyn_Constraint*
_tmp92E;_LL3AC:{struct Cyc_Absyn_ArrayType_struct*_tmp92A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp929;if(_tmp92A->tag != 9)goto _LL3AE;else{_tmp92B=_tmp92A->f1;_tmp92C=_tmp92B.elt_type;
_tmp92D=_tmp92B.num_elts;_tmp92E=_tmp92B.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92E))goto _LL3AE;_LL3AD: if(_tmp92D == 0){const char*
_tmpDD9;void*_tmpDD8;(_tmpDD8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD9="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDD9,sizeof(char),55))),_tag_dyneither(_tmpDD8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92D;struct Cyc_Absyn_Exp*
_tmp931=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp932=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp931,
_tmp932,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AB;};_LL3AE:;_LL3AF: goto _LL3AB;
_LL3AB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp934;struct _RegionHandle*
_tmp935;struct Cyc_Toc_Env*_tmp933=nv;_tmp934=*_tmp933;_tmp935=_tmp934.rgn;{
struct Cyc_Absyn_Stmt*_tmp936=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp937=Cyc_Toc_share_env(_tmp935,nv);struct Cyc_Toc_Env*_tmp939;struct Cyc_List_List*
_tmp93A;struct Cyc_Absyn_Stmt*_tmp93B;struct _tuple21 _tmp938=Cyc_Toc_xlate_pat(
_tmp937,_tmp935,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp939=_tmp938.f1;_tmp93A=_tmp938.f2;_tmp93B=_tmp938.f3;Cyc_Toc_stmt_to_c(
_tmp939,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp93B,s,0),0);for(0;_tmp93A != 0;_tmp93A=_tmp93A->tl){
struct _tuple22 _tmp93D;struct _tuple0*_tmp93E;void*_tmp93F;struct _tuple22*_tmp93C=(
struct _tuple22*)_tmp93A->hd;_tmp93D=*_tmp93C;_tmp93E=_tmp93D.f1;_tmp93F=_tmp93D.f2;
s=Cyc_Absyn_declare_stmt(_tmp93E,_tmp93F,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp940=e->r;struct Cyc_Absyn_Exp*_tmp942;struct Cyc_Absyn_Exp*_tmp944;struct
Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp94A;
struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Exp*
_tmp950;struct Cyc_List_List*_tmp952;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*
_tmp955;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*
_tmp95A;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*
_tmp95E;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*
_tmp963;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*
_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_List_List*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_List_List*_tmp96E;void*_tmp970;
void**_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct _tuple7*_tmp974;struct _tuple7
_tmp975;void*_tmp976;void**_tmp977;struct Cyc_List_List*_tmp978;struct Cyc_List_List*
_tmp97A;struct Cyc_List_List*_tmp97C;void*_tmp97E;void**_tmp97F;void*_tmp981;void**
_tmp982;struct Cyc_Absyn_Stmt*_tmp984;struct Cyc_Absyn_MallocInfo _tmp986;struct Cyc_Absyn_MallocInfo*
_tmp987;_LL3B1: {struct Cyc_Absyn_Deref_e_struct*_tmp941=(struct Cyc_Absyn_Deref_e_struct*)
_tmp940;if(_tmp941->tag != 21)goto _LL3B3;else{_tmp942=_tmp941->f1;}}_LL3B2:
_tmp944=_tmp942;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp943=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp940;if(_tmp943->tag != 22)goto _LL3B5;
else{_tmp944=_tmp943->f1;}}_LL3B4: _tmp946=_tmp944;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp945=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp940;if(_tmp945->tag != 23)goto
_LL3B7;else{_tmp946=_tmp945->f1;}}_LL3B6: _tmp948=_tmp946;goto _LL3B8;_LL3B7: {
struct Cyc_Absyn_Address_e_struct*_tmp947=(struct Cyc_Absyn_Address_e_struct*)
_tmp940;if(_tmp947->tag != 16)goto _LL3B9;else{_tmp948=_tmp947->f1;}}_LL3B8:
_tmp94A=_tmp948;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_struct*_tmp949=(
struct Cyc_Absyn_Throw_e_struct*)_tmp940;if(_tmp949->tag != 12)goto _LL3BB;else{
_tmp94A=_tmp949->f1;}}_LL3BA: _tmp94C=_tmp94A;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp94B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp940;if(_tmp94B->tag != 13)
goto _LL3BD;else{_tmp94C=_tmp94B->f1;}}_LL3BC: _tmp94E=_tmp94C;goto _LL3BE;_LL3BD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94D=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp940;if(_tmp94D->tag != 19)goto _LL3BF;else{_tmp94E=_tmp94D->f1;}}_LL3BE:
_tmp950=_tmp94E;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_struct*_tmp94F=(
struct Cyc_Absyn_Increment_e_struct*)_tmp940;if(_tmp94F->tag != 5)goto _LL3C1;else{
_tmp950=_tmp94F->f1;}}_LL3C0: Cyc_Toc_exptypes_to_c(_tmp950);goto _LL3B0;_LL3C1: {
struct Cyc_Absyn_Primop_e_struct*_tmp951=(struct Cyc_Absyn_Primop_e_struct*)
_tmp940;if(_tmp951->tag != 3)goto _LL3C3;else{_tmp952=_tmp951->f2;}}_LL3C2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp952);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_struct*_tmp953=(struct Cyc_Absyn_And_e_struct*)
_tmp940;if(_tmp953->tag != 7)goto _LL3C5;else{_tmp954=_tmp953->f1;_tmp955=_tmp953->f2;}}
_LL3C4: _tmp957=_tmp954;_tmp958=_tmp955;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_struct*
_tmp956=(struct Cyc_Absyn_Or_e_struct*)_tmp940;if(_tmp956->tag != 8)goto _LL3C7;
else{_tmp957=_tmp956->f1;_tmp958=_tmp956->f2;}}_LL3C6: _tmp95A=_tmp957;_tmp95B=
_tmp958;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp959=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp940;if(_tmp959->tag != 9)goto _LL3C9;else{_tmp95A=_tmp959->f1;_tmp95B=_tmp959->f2;}}
_LL3C8: _tmp95D=_tmp95A;_tmp95E=_tmp95B;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp95C=(struct Cyc_Absyn_Subscript_e_struct*)_tmp940;if(_tmp95C->tag != 24)goto
_LL3CB;else{_tmp95D=_tmp95C->f1;_tmp95E=_tmp95C->f2;}}_LL3CA: _tmp960=_tmp95D;
_tmp961=_tmp95E;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_struct*_tmp95F=(
struct Cyc_Absyn_Swap_e_struct*)_tmp940;if(_tmp95F->tag != 35)goto _LL3CD;else{
_tmp960=_tmp95F->f1;_tmp961=_tmp95F->f2;}}_LL3CC: _tmp963=_tmp960;_tmp964=_tmp961;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp962=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp940;if(_tmp962->tag != 4)goto _LL3CF;else{_tmp963=_tmp962->f1;_tmp964=_tmp962->f3;}}
_LL3CE: Cyc_Toc_exptypes_to_c(_tmp963);Cyc_Toc_exptypes_to_c(_tmp964);goto _LL3B0;
_LL3CF: {struct Cyc_Absyn_Conditional_e_struct*_tmp965=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp940;if(_tmp965->tag != 6)goto _LL3D1;else{_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;
_tmp968=_tmp965->f3;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp966);Cyc_Toc_exptypes_to_c(
_tmp967);Cyc_Toc_exptypes_to_c(_tmp968);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp969=(struct Cyc_Absyn_FnCall_e_struct*)_tmp940;if(_tmp969->tag != 11)goto
_LL3D3;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}_LL3D2: _tmp96D=_tmp96A;
_tmp96E=_tmp96B;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp96C=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp940;if(_tmp96C->tag != 10)goto _LL3D5;
else{_tmp96D=_tmp96C->f1;_tmp96E=_tmp96C->f2;}}_LL3D4: Cyc_Toc_exptypes_to_c(
_tmp96D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96E);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_Cast_e_struct*
_tmp96F=(struct Cyc_Absyn_Cast_e_struct*)_tmp940;if(_tmp96F->tag != 15)goto _LL3D7;
else{_tmp970=(void**)& _tmp96F->f1;_tmp971=(void**)((void**)& _tmp96F->f1);_tmp972=
_tmp96F->f2;}}_LL3D6:*_tmp971=Cyc_Toc_typ_to_c(*_tmp971);Cyc_Toc_exptypes_to_c(
_tmp972);goto _LL3B0;_LL3D7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp973=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp940;if(_tmp973->tag != 26)goto _LL3D9;
else{_tmp974=_tmp973->f1;_tmp975=*_tmp974;_tmp976=_tmp975.f3;_tmp977=(void**)&(*
_tmp973->f1).f3;_tmp978=_tmp973->f2;}}_LL3D8:*_tmp977=Cyc_Toc_typ_to_c(*_tmp977);
_tmp97A=_tmp978;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp979=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp940;if(_tmp979->tag != 36)
goto _LL3DB;else{_tmp97A=_tmp979->f2;}}_LL3DA: _tmp97C=_tmp97A;goto _LL3DC;_LL3DB: {
struct Cyc_Absyn_Array_e_struct*_tmp97B=(struct Cyc_Absyn_Array_e_struct*)_tmp940;
if(_tmp97B->tag != 27)goto _LL3DD;else{_tmp97C=_tmp97B->f1;}}_LL3DC: for(0;_tmp97C
!= 0;_tmp97C=_tmp97C->tl){struct Cyc_Absyn_Exp*_tmp997;struct _tuple15 _tmp996=*((
struct _tuple15*)_tmp97C->hd);_tmp997=_tmp996.f2;Cyc_Toc_exptypes_to_c(_tmp997);}
goto _LL3B0;_LL3DD: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97D=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp940;if(_tmp97D->tag != 20)goto _LL3DF;else{_tmp97E=(void**)& _tmp97D->f1;
_tmp97F=(void**)((void**)& _tmp97D->f1);}}_LL3DE: _tmp982=_tmp97F;goto _LL3E0;
_LL3DF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp980=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp940;if(_tmp980->tag != 18)goto _LL3E1;else{_tmp981=(void**)& _tmp980->f1;
_tmp982=(void**)((void**)& _tmp980->f1);}}_LL3E0:*_tmp982=Cyc_Toc_typ_to_c(*
_tmp982);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp983=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp940;if(_tmp983->tag != 37)goto _LL3E3;else{_tmp984=_tmp983->f1;}}_LL3E2: Cyc_Toc_stmttypes_to_c(
_tmp984);goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Malloc_e_struct*_tmp985=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp940;if(_tmp985->tag != 34)goto _LL3E5;else{_tmp986=_tmp985->f1;_tmp987=(struct
Cyc_Absyn_MallocInfo*)& _tmp985->f1;}}_LL3E4: if(_tmp987->elt_type != 0){void**
_tmpDDA;_tmp987->elt_type=((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp987->elt_type))),_tmpDDA))));}Cyc_Toc_exptypes_to_c(
_tmp987->num_elts);goto _LL3B0;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp988=(
struct Cyc_Absyn_Const_e_struct*)_tmp940;if(_tmp988->tag != 0)goto _LL3E7;}_LL3E6:
goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Var_e_struct*_tmp989=(struct Cyc_Absyn_Var_e_struct*)
_tmp940;if(_tmp989->tag != 1)goto _LL3E9;}_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp98A=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp940;if(_tmp98A->tag != 2)goto
_LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Enum_e_struct*_tmp98B=(struct
Cyc_Absyn_Enum_e_struct*)_tmp940;if(_tmp98B->tag != 32)goto _LL3ED;}_LL3EC: goto
_LL3EE;_LL3ED: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp98C=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp940;if(_tmp98C->tag != 33)goto _LL3EF;}_LL3EE: goto _LL3B0;_LL3EF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp940;if(_tmp98D->tag != 30)goto
_LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Datatype_e_struct*_tmp98E=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp940;if(_tmp98E->tag != 31)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98F=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp940;if(_tmp98F->tag != 29)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp990=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp940;if(_tmp990->tag != 28)
goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_struct*_tmp991=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp940;if(_tmp991->tag != 25)goto _LL3F9;}_LL3F8:
goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_struct*_tmp992=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp940;if(_tmp992->tag != 14)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_struct*
_tmp993=(struct Cyc_Absyn_New_e_struct*)_tmp940;if(_tmp993->tag != 17)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_struct*_tmp994=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp940;if(_tmp994->tag != 39)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp995=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp940;if(_tmp995->tag != 38)goto
_LL3B0;}_LL400:{const char*_tmpDDD;void*_tmpDDC;(_tmpDDC=0,Cyc_Tcutil_terr(e->loc,((
_tmpDDD="Cyclone expression within C code",_tag_dyneither(_tmpDDD,sizeof(char),
33))),_tag_dyneither(_tmpDDC,sizeof(void*),0)));}goto _LL3B0;_LL3B0:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp99B=d->r;struct Cyc_Absyn_Vardecl*_tmp99D;struct
Cyc_Absyn_Fndecl*_tmp99F;struct Cyc_Absyn_Aggrdecl*_tmp9A1;struct Cyc_Absyn_Enumdecl*
_tmp9A3;struct Cyc_Absyn_Typedefdecl*_tmp9A5;_LL402: {struct Cyc_Absyn_Var_d_struct*
_tmp99C=(struct Cyc_Absyn_Var_d_struct*)_tmp99B;if(_tmp99C->tag != 0)goto _LL404;
else{_tmp99D=_tmp99C->f1;}}_LL403: _tmp99D->type=Cyc_Toc_typ_to_c(_tmp99D->type);
if(_tmp99D->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp99D->initializer));goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_struct*
_tmp99E=(struct Cyc_Absyn_Fn_d_struct*)_tmp99B;if(_tmp99E->tag != 1)goto _LL406;
else{_tmp99F=_tmp99E->f1;}}_LL405: _tmp99F->ret_type=Cyc_Toc_typ_to_c(_tmp99F->ret_type);{
struct Cyc_List_List*_tmp9B1=_tmp99F->args;for(0;_tmp9B1 != 0;_tmp9B1=_tmp9B1->tl){(*((
struct _tuple25*)_tmp9B1->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9B1->hd)).f3);}}
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_struct*_tmp9A0=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp99B;if(_tmp9A0->tag != 6)goto _LL408;else{_tmp9A1=_tmp9A0->f1;}}_LL407: Cyc_Toc_aggrdecl_to_c(
_tmp9A1);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_struct*_tmp9A2=(struct Cyc_Absyn_Enum_d_struct*)
_tmp99B;if(_tmp9A2->tag != 8)goto _LL40A;else{_tmp9A3=_tmp9A2->f1;}}_LL409: if(
_tmp9A3->fields != 0){struct Cyc_List_List*_tmp9B2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A3->fields))->v;for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){
struct Cyc_Absyn_Enumfield*_tmp9B3=(struct Cyc_Absyn_Enumfield*)_tmp9B2->hd;if(
_tmp9B3->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B3->tag));}}
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A4=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp99B;if(_tmp9A4->tag != 9)goto _LL40C;else{_tmp9A5=_tmp9A4->f1;}}_LL40B:{struct
Cyc_Core_Opt*_tmpDDE;_tmp9A5->defn=((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((
_tmpDDE->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A5->defn))->v),_tmpDDE))));}goto _LL401;_LL40C: {struct Cyc_Absyn_Let_d_struct*
_tmp9A6=(struct Cyc_Absyn_Let_d_struct*)_tmp99B;if(_tmp9A6->tag != 2)goto _LL40E;}
_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_struct*_tmp9A7=(struct Cyc_Absyn_Letv_d_struct*)
_tmp99B;if(_tmp9A7->tag != 3)goto _LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9A8=(struct Cyc_Absyn_Datatype_d_struct*)_tmp99B;if(_tmp9A8->tag != 7)goto
_LL412;}_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_struct*_tmp9A9=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp99B;if(_tmp9A9->tag != 10)goto _LL414;}
_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_struct*_tmp9AA=(struct Cyc_Absyn_Using_d_struct*)
_tmp99B;if(_tmp9AA->tag != 11)goto _LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9AB=(struct Cyc_Absyn_ExternC_d_struct*)_tmp99B;if(_tmp9AB->tag != 12)goto
_LL418;}_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AC=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp99B;if(_tmp9AC->tag != 13)
goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_struct*_tmp9AD=(
struct Cyc_Absyn_Region_d_struct*)_tmp99B;if(_tmp9AD->tag != 4)goto _LL41C;}_LL41B:
goto _LL41D;_LL41C: {struct Cyc_Absyn_Alias_d_struct*_tmp9AE=(struct Cyc_Absyn_Alias_d_struct*)
_tmp99B;if(_tmp9AE->tag != 5)goto _LL41E;}_LL41D:{const char*_tmpDE1;void*_tmpDE0;(
_tmpDE0=0,Cyc_Tcutil_terr(d->loc,((_tmpDE1="Cyclone declaration within C code",
_tag_dyneither(_tmpDE1,sizeof(char),34))),_tag_dyneither(_tmpDE0,sizeof(void*),0)));}
goto _LL401;_LL41E: {struct Cyc_Absyn_Porton_d_struct*_tmp9AF=(struct Cyc_Absyn_Porton_d_struct*)
_tmp99B;if(_tmp9AF->tag != 14)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9B0=(struct Cyc_Absyn_Portoff_d_struct*)_tmp99B;if(_tmp9B0->tag != 15)goto
_LL401;}_LL421: goto _LL401;_LL401:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9B7=s->r;
struct Cyc_Absyn_Exp*_tmp9B9;struct Cyc_Absyn_Stmt*_tmp9BB;struct Cyc_Absyn_Stmt*
_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BE;struct Cyc_Absyn_Exp*_tmp9C0;struct Cyc_Absyn_Stmt*
_tmp9C1;struct Cyc_Absyn_Stmt*_tmp9C2;struct _tuple8 _tmp9C4;struct Cyc_Absyn_Exp*
_tmp9C5;struct Cyc_Absyn_Stmt*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C8;struct _tuple8
_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CA;struct _tuple8 _tmp9CB;struct Cyc_Absyn_Exp*
_tmp9CC;struct Cyc_Absyn_Stmt*_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_List_List*
_tmp9D0;struct Cyc_Absyn_Decl*_tmp9D2;struct Cyc_Absyn_Stmt*_tmp9D3;struct Cyc_Absyn_Stmt*
_tmp9D5;struct _tuple8 _tmp9D6;struct Cyc_Absyn_Exp*_tmp9D7;_LL423: {struct Cyc_Absyn_Exp_s_struct*
_tmp9B8=(struct Cyc_Absyn_Exp_s_struct*)_tmp9B7;if(_tmp9B8->tag != 1)goto _LL425;
else{_tmp9B9=_tmp9B8->f1;}}_LL424: Cyc_Toc_exptypes_to_c(_tmp9B9);goto _LL422;
_LL425: {struct Cyc_Absyn_Seq_s_struct*_tmp9BA=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B7;if(_tmp9BA->tag != 2)goto _LL427;else{_tmp9BB=_tmp9BA->f1;_tmp9BC=_tmp9BA->f2;}}
_LL426: Cyc_Toc_stmttypes_to_c(_tmp9BB);Cyc_Toc_stmttypes_to_c(_tmp9BC);goto
_LL422;_LL427: {struct Cyc_Absyn_Return_s_struct*_tmp9BD=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B7;if(_tmp9BD->tag != 3)goto _LL429;else{_tmp9BE=_tmp9BD->f1;}}_LL428: if(
_tmp9BE != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BE);goto _LL422;
_LL429: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9BF=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B7;if(_tmp9BF->tag != 4)goto _LL42B;else{_tmp9C0=_tmp9BF->f1;_tmp9C1=_tmp9BF->f2;
_tmp9C2=_tmp9BF->f3;}}_LL42A: Cyc_Toc_exptypes_to_c(_tmp9C0);Cyc_Toc_stmttypes_to_c(
_tmp9C1);Cyc_Toc_stmttypes_to_c(_tmp9C2);goto _LL422;_LL42B: {struct Cyc_Absyn_While_s_struct*
_tmp9C3=(struct Cyc_Absyn_While_s_struct*)_tmp9B7;if(_tmp9C3->tag != 5)goto _LL42D;
else{_tmp9C4=_tmp9C3->f1;_tmp9C5=_tmp9C4.f1;_tmp9C6=_tmp9C3->f2;}}_LL42C: Cyc_Toc_exptypes_to_c(
_tmp9C5);Cyc_Toc_stmttypes_to_c(_tmp9C6);goto _LL422;_LL42D: {struct Cyc_Absyn_For_s_struct*
_tmp9C7=(struct Cyc_Absyn_For_s_struct*)_tmp9B7;if(_tmp9C7->tag != 9)goto _LL42F;
else{_tmp9C8=_tmp9C7->f1;_tmp9C9=_tmp9C7->f2;_tmp9CA=_tmp9C9.f1;_tmp9CB=_tmp9C7->f3;
_tmp9CC=_tmp9CB.f1;_tmp9CD=_tmp9C7->f4;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9C8);
Cyc_Toc_exptypes_to_c(_tmp9CA);Cyc_Toc_exptypes_to_c(_tmp9CC);Cyc_Toc_stmttypes_to_c(
_tmp9CD);goto _LL422;_LL42F: {struct Cyc_Absyn_Switch_s_struct*_tmp9CE=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B7;if(_tmp9CE->tag != 10)goto _LL431;else{_tmp9CF=_tmp9CE->f1;_tmp9D0=_tmp9CE->f2;}}
_LL430: Cyc_Toc_exptypes_to_c(_tmp9CF);for(0;_tmp9D0 != 0;_tmp9D0=_tmp9D0->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9D0->hd)->body);}goto _LL422;_LL431: {struct
Cyc_Absyn_Decl_s_struct*_tmp9D1=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B7;if(
_tmp9D1->tag != 12)goto _LL433;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=_tmp9D1->f2;}}
_LL432: Cyc_Toc_decltypes_to_c(_tmp9D2);Cyc_Toc_stmttypes_to_c(_tmp9D3);goto
_LL422;_LL433: {struct Cyc_Absyn_Do_s_struct*_tmp9D4=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B7;if(_tmp9D4->tag != 14)goto _LL435;else{_tmp9D5=_tmp9D4->f1;_tmp9D6=_tmp9D4->f2;
_tmp9D7=_tmp9D6.f1;}}_LL434: Cyc_Toc_stmttypes_to_c(_tmp9D5);Cyc_Toc_exptypes_to_c(
_tmp9D7);goto _LL422;_LL435: {struct Cyc_Absyn_Skip_s_struct*_tmp9D8=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B7;if(_tmp9D8->tag != 0)goto _LL437;}_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_Break_s_struct*
_tmp9D9=(struct Cyc_Absyn_Break_s_struct*)_tmp9B7;if(_tmp9D9->tag != 6)goto _LL439;}
_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Continue_s_struct*_tmp9DA=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B7;if(_tmp9DA->tag != 7)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Goto_s_struct*
_tmp9DB=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B7;if(_tmp9DB->tag != 8)goto _LL43D;}
_LL43C: goto _LL422;_LL43D: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DC=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B7;if(_tmp9DC->tag != 11)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Label_s_struct*
_tmp9DD=(struct Cyc_Absyn_Label_s_struct*)_tmp9B7;if(_tmp9DD->tag != 13)goto _LL441;}
_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DE=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B7;if(_tmp9DE->tag != 15)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9DF=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B7;if(_tmp9DF->tag != 16)goto
_LL422;}_LL444:{const char*_tmpDE4;void*_tmpDE3;(_tmpDE3=0,Cyc_Tcutil_terr(s->loc,((
_tmpDE4="Cyclone statement in C code",_tag_dyneither(_tmpDE4,sizeof(char),28))),
_tag_dyneither(_tmpDE3,sizeof(void*),0)));}goto _LL422;_LL422:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDE7;void*
_tmpDE6;(_tmpDE6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDE7="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDE7,sizeof(char),29))),
_tag_dyneither(_tmpDE6,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E4=d->r;struct Cyc_Absyn_Vardecl*_tmp9E6;struct Cyc_Absyn_Fndecl*
_tmp9E8;struct Cyc_Absyn_Aggrdecl*_tmp9EE;struct Cyc_Absyn_Datatypedecl*_tmp9F0;
struct Cyc_Absyn_Enumdecl*_tmp9F2;struct Cyc_Absyn_Typedefdecl*_tmp9F4;struct Cyc_List_List*
_tmp9F8;struct Cyc_List_List*_tmp9FA;struct Cyc_List_List*_tmp9FC;struct Cyc_List_List*
_tmp9FE;_LL446: {struct Cyc_Absyn_Var_d_struct*_tmp9E5=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E4;if(_tmp9E5->tag != 0)goto _LL448;else{_tmp9E6=_tmp9E5->f1;}}_LL447: {
struct _tuple0*_tmp9FF=_tmp9E6->name;if(_tmp9E6->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDE8;_tmp9FF=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->f1=Cyc_Absyn_Rel_n(
0),((_tmpDE8->f2=(*_tmp9FF).f2,_tmpDE8))))));}if(_tmp9E6->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E6->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E6->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDEB;struct Cyc_Absyn_Global_b_struct*_tmpDEA;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E6->name,Cyc_Absyn_varb_exp(_tmp9FF,(void*)((
_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=1,((_tmpDEB.f1=
_tmp9E6,_tmpDEB)))),_tmpDEA)))),0));}_tmp9E6->name=_tmp9FF;_tmp9E6->sc=Cyc_Toc_scope_to_c(
_tmp9E6->sc);_tmp9E6->type=Cyc_Toc_typ_to_c_array(_tmp9E6->type);{struct Cyc_List_List*
_tmpDEC;Cyc_Toc_result_decls=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->hd=
d,((_tmpDEC->tl=Cyc_Toc_result_decls,_tmpDEC))))));}goto _LL445;}_LL448: {struct
Cyc_Absyn_Fn_d_struct*_tmp9E7=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E4;if(_tmp9E7->tag
!= 1)goto _LL44A;else{_tmp9E8=_tmp9E7->f1;}}_LL449: {struct _tuple0*_tmpA04=
_tmp9E8->name;if(_tmp9E8->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDED;_tmpA04=((
_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->f1=Cyc_Absyn_Rel_n(0),((_tmpDED->f2=(*
_tmpA04).f2,_tmpDED))))));}_tmp9E8->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9E8->name,Cyc_Absyn_var_exp(_tmpA04,0));_tmp9E8->name=_tmpA04;Cyc_Toc_fndecl_to_c(
nv,_tmp9E8,cinclude);{struct Cyc_List_List*_tmpDEE;Cyc_Toc_result_decls=((_tmpDEE=
_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->hd=d,((_tmpDEE->tl=Cyc_Toc_result_decls,
_tmpDEE))))));}goto _LL445;}_LL44A: {struct Cyc_Absyn_Let_d_struct*_tmp9E9=(struct
Cyc_Absyn_Let_d_struct*)_tmp9E4;if(_tmp9E9->tag != 2)goto _LL44C;}_LL44B: goto
_LL44D;_LL44C: {struct Cyc_Absyn_Letv_d_struct*_tmp9EA=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E4;if(_tmp9EA->tag != 3)goto _LL44E;}_LL44D: {const char*_tmpDF1;void*_tmpDF0;(
_tmpDF0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF1="letdecl at toplevel",_tag_dyneither(_tmpDF1,sizeof(char),20))),
_tag_dyneither(_tmpDF0,sizeof(void*),0)));}_LL44E: {struct Cyc_Absyn_Region_d_struct*
_tmp9EB=(struct Cyc_Absyn_Region_d_struct*)_tmp9E4;if(_tmp9EB->tag != 4)goto _LL450;}
_LL44F: {const char*_tmpDF4;void*_tmpDF3;(_tmpDF3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDF4="region decl at toplevel",
_tag_dyneither(_tmpDF4,sizeof(char),24))),_tag_dyneither(_tmpDF3,sizeof(void*),0)));}
_LL450: {struct Cyc_Absyn_Alias_d_struct*_tmp9EC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E4;if(_tmp9EC->tag != 5)goto _LL452;}_LL451: {const char*_tmpDF7;void*_tmpDF6;(
_tmpDF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF7="alias decl at toplevel",_tag_dyneither(_tmpDF7,sizeof(char),23))),
_tag_dyneither(_tmpDF6,sizeof(void*),0)));}_LL452: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9ED=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9E4;if(_tmp9ED->tag != 6)goto _LL454;
else{_tmp9EE=_tmp9ED->f1;}}_LL453: Cyc_Toc_aggrdecl_to_c(_tmp9EE);goto _LL445;
_LL454: {struct Cyc_Absyn_Datatype_d_struct*_tmp9EF=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E4;if(_tmp9EF->tag != 7)goto _LL456;else{_tmp9F0=_tmp9EF->f1;}}_LL455: if(
_tmp9F0->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9F0);else{Cyc_Toc_datatypedecl_to_c(
_tmp9F0);}goto _LL445;_LL456: {struct Cyc_Absyn_Enum_d_struct*_tmp9F1=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E4;if(_tmp9F1->tag != 8)goto _LL458;else{_tmp9F2=_tmp9F1->f1;}}_LL457: Cyc_Toc_enumdecl_to_c(
nv,_tmp9F2);{struct Cyc_List_List*_tmpDF8;Cyc_Toc_result_decls=((_tmpDF8=
_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->hd=d,((_tmpDF8->tl=Cyc_Toc_result_decls,
_tmpDF8))))));}goto _LL445;_LL458: {struct Cyc_Absyn_Typedef_d_struct*_tmp9F3=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9E4;if(_tmp9F3->tag != 9)goto _LL45A;else{
_tmp9F4=_tmp9F3->f1;}}_LL459: _tmp9F4->name=_tmp9F4->name;_tmp9F4->tvs=0;if(
_tmp9F4->defn != 0){struct Cyc_Core_Opt*_tmpDF9;_tmp9F4->defn=((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F4->defn))->v),_tmpDF9))));}else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F4->kind))->v){case Cyc_Absyn_BoxKind: _LL466:{
struct Cyc_Core_Opt*_tmpDFA;_tmp9F4->defn=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((
_tmpDFA->v=Cyc_Absyn_void_star_typ(),_tmpDFA))));}break;default: _LL467:{struct
Cyc_Core_Opt*_tmpDFB;_tmp9F4->defn=((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((
_tmpDFB->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDFB))));}break;}}{struct Cyc_List_List*
_tmpDFC;Cyc_Toc_result_decls=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=
d,((_tmpDFC->tl=Cyc_Toc_result_decls,_tmpDFC))))));}goto _LL445;_LL45A: {struct
Cyc_Absyn_Porton_d_struct*_tmp9F5=(struct Cyc_Absyn_Porton_d_struct*)_tmp9E4;if(
_tmp9F5->tag != 14)goto _LL45C;}_LL45B: goto _LL45D;_LL45C: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F6=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E4;if(_tmp9F6->tag != 15)goto
_LL45E;}_LL45D: goto _LL445;_LL45E: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F7=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E4;if(_tmp9F7->tag != 10)goto _LL460;
else{_tmp9F8=_tmp9F7->f2;}}_LL45F: _tmp9FA=_tmp9F8;goto _LL461;_LL460: {struct Cyc_Absyn_Using_d_struct*
_tmp9F9=(struct Cyc_Absyn_Using_d_struct*)_tmp9E4;if(_tmp9F9->tag != 11)goto _LL462;
else{_tmp9FA=_tmp9F9->f2;}}_LL461: _tmp9FC=_tmp9FA;goto _LL463;_LL462: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9FB=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E4;if(_tmp9FB->tag != 12)goto
_LL464;else{_tmp9FC=_tmp9FB->f1;}}_LL463: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FC,top,
cinclude);goto _LL445;_LL464: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FD=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E4;if(_tmp9FD->tag != 13)goto _LL445;
else{_tmp9FE=_tmp9FD->f1;}}_LL465: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FE,top,1);goto
_LL445;_LL445:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA13;struct Cyc_Core_NewRegion _tmpA12=Cyc_Core_new_dynregion();
_tmpA13=_tmpA12.dynregion;{struct _DynRegionFrame _tmpA14;struct _RegionHandle*d=
_open_dynregion(& _tmpA14,_tmpA13);{struct Cyc_Dict_Dict*_tmpE07;struct Cyc_Dict_Dict*
_tmpE06;struct Cyc_Set_Set**_tmpE05;struct Cyc_Dict_Dict*_tmpE04;struct Cyc_List_List**
_tmpE03;struct Cyc_Toc_TocState*_tmpE02;Cyc_Toc_toc_state=((_tmpE02=_cycalloc(
sizeof(*_tmpE02)),((_tmpE02->dyn=(struct _DynRegionHandle*)_tmpA13,((_tmpE02->tuple_types=(
struct Cyc_List_List**)((_tmpE03=_region_malloc(d,sizeof(*_tmpE03)),((_tmpE03[0]=
0,_tmpE03)))),((_tmpE02->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE04=
_region_malloc(d,sizeof(*_tmpE04)),((_tmpE04[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE04)))),((_tmpE02->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE05=
_region_malloc(d,sizeof(*_tmpE05)),((_tmpE05[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE05)))),((_tmpE02->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE06=
_region_malloc(d,sizeof(*_tmpE06)),((_tmpE06[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE06)))),((_tmpE02->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE07=_region_malloc(
d,sizeof(*_tmpE07)),((_tmpE07[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE07)))),((_tmpE02->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE02))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE08;Cyc_Toc_globals=_tag_dyneither(((_tmpE08=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE08[0]=& Cyc_Toc__throw_str,((_tmpE08[1]=&
Cyc_Toc_setjmp_str,((_tmpE08[2]=& Cyc_Toc__push_handler_str,((_tmpE08[3]=& Cyc_Toc__pop_handler_str,((
_tmpE08[4]=& Cyc_Toc__exn_thrown_str,((_tmpE08[5]=& Cyc_Toc__npop_handler_str,((
_tmpE08[6]=& Cyc_Toc__check_null_str,((_tmpE08[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE08[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE08[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE08[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE08[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE08[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE08[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE08[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE08[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE08[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE08[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE08[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE08[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE08[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE08[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE08[22]=& Cyc_Toc__cycalloc_str,((_tmpE08[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE08[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE08[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE08[26]=& Cyc_Toc__region_malloc_str,((_tmpE08[27]=& Cyc_Toc__region_calloc_str,((
_tmpE08[28]=& Cyc_Toc__check_times_str,((_tmpE08[29]=& Cyc_Toc__new_region_str,((
_tmpE08[30]=& Cyc_Toc__push_region_str,((_tmpE08[31]=& Cyc_Toc__pop_region_str,((
_tmpE08[32]=& Cyc_Toc__open_dynregion_str,((_tmpE08[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE08[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE08[35]=& Cyc_Toc__reset_region_str,((
_tmpE08[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE08[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE08)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA1C=_new_region("start");struct
_RegionHandle*start=& _tmpA1C;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA1E;struct Cyc_Toc_TocState _tmpA1D=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1E=_tmpA1D.dyn;Cyc_Core_free_dynregion(
_tmpA1E);};{struct Cyc_List_List*_tmpA1F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA1F;};;
_pop_region(start);};}
