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
Cyc_stderr,fmt,ap);{const char*_tmpA20;void*_tmpA1F;(_tmpA1F=0,Cyc_fprintf(Cyc_stderr,((
_tmpA20="\n",_tag_dyneither(_tmpA20,sizeof(char),2))),_tag_dyneither(_tmpA1F,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA23;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA22;(int)_throw((void*)((
_tmpA22=_cycalloc_atomic(sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA23)),_tmpA22)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA26;void*
_tmpA25;(_tmpA25=0,Cyc_fprintf(Cyc_stderr,((_tmpA26="\n",_tag_dyneither(_tmpA26,
sizeof(char),2))),_tag_dyneither(_tmpA25,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA29;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA28;(int)_throw((void*)((_tmpA28=_cycalloc_atomic(sizeof(*_tmpA28)),((
_tmpA28[0]=((_tmpA29.tag=Cyc_Toc_Toc_Impossible,_tmpA29)),_tmpA28)))));};}char
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
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA2A;skip_stmt_opt=((_tmpA2A=
_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A[0]=Cyc_Absyn_skip_stmt(0),_tmpA2A))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA2D;struct Cyc_Absyn_Cast_e_struct*_tmpA2C;
return(void*)((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=((_tmpA2D.tag=15,((
_tmpA2D.f1=(void*)t,((_tmpA2D.f2=e,((_tmpA2D.f3=0,((_tmpA2D.f4=Cyc_Absyn_No_coercion,
_tmpA2D)))))))))),_tmpA2C))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA30;struct Cyc_Absyn_Deref_e_struct*_tmpA2F;return(void*)((_tmpA2F=_cycalloc(
sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=21,((_tmpA30.f1=e,_tmpA30)))),
_tmpA2F))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA33;struct Cyc_Absyn_Subscript_e_struct*
_tmpA32;return(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((
_tmpA33.tag=24,((_tmpA33.f1=e1,((_tmpA33.f2=e2,_tmpA33)))))),_tmpA32))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA36;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA35;return(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((
_tmpA36.tag=37,((_tmpA36.f1=s,_tmpA36)))),_tmpA35))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA39;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA38;return(void*)((_tmpA38=
_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA39.tag=18,((_tmpA39.f1=(void*)t,
_tmpA39)))),_tmpA38))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA3C;struct Cyc_Absyn_FnCall_e_struct*
_tmpA3B;return(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((
_tmpA3C.tag=11,((_tmpA3C.f1=e,((_tmpA3C.f2=es,((_tmpA3C.f3=0,_tmpA3C)))))))),
_tmpA3B))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA3F;struct Cyc_Absyn_Exp_s_struct*
_tmpA3E;return(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((
_tmpA3F.tag=1,((_tmpA3F.f1=e,_tmpA3F)))),_tmpA3E))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA42;struct Cyc_Absyn_Seq_s_struct*_tmpA41;return(void*)((_tmpA41=_cycalloc(
sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=2,((_tmpA42.f1=s1,((_tmpA42.f2=s2,
_tmpA42)))))),_tmpA41))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA45;struct Cyc_Absyn_Conditional_e_struct*_tmpA44;return(void*)((_tmpA44=
_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=6,((_tmpA45.f1=e1,((
_tmpA45.f2=e2,((_tmpA45.f3=e3,_tmpA45)))))))),_tmpA44))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA48;struct Cyc_Absyn_AggrMember_e_struct*_tmpA47;return(void*)((_tmpA47=
_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=22,((_tmpA48.f1=e,((
_tmpA48.f2=n,((_tmpA48.f3=0,((_tmpA48.f4=0,_tmpA48)))))))))),_tmpA47))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA4B;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA4A;return(void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((
_tmpA4B.tag=23,((_tmpA4B.f1=e,((_tmpA4B.f2=n,((_tmpA4B.f3=0,((_tmpA4B.f4=0,
_tmpA4B)))))))))),_tmpA4A))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA4E;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA4D;return(void*)((_tmpA4D=_cycalloc(
sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=36,((_tmpA4E.f1=tdopt,((_tmpA4E.f2=
ds,_tmpA4E)))))),_tmpA4D))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA51;
struct Cyc_Absyn_Goto_s_struct*_tmpA50;return(void*)((_tmpA50=_cycalloc(sizeof(*
_tmpA50)),((_tmpA50[0]=((_tmpA51.tag=8,((_tmpA51.f1=v,((_tmpA51.f2=s,_tmpA51)))))),
_tmpA50))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=t;
enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE: {struct Cyc_Core_Impossible_struct
_tmpA57;const char*_tmpA56;struct Cyc_Core_Impossible_struct*_tmpA55;(int)_throw((
void*)((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA57.tag=Cyc_Core_Impossible,((
_tmpA57.f1=((_tmpA56="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA56,sizeof(char),44))),_tmpA57)))),_tmpA55)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA5D;const char*_tmpA5C;struct Cyc_Core_Impossible_struct*
_tmpA5B;(int)_throw((void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((
_tmpA5D.tag=Cyc_Core_Impossible,((_tmpA5D.f1=((_tmpA5C="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpA5C,sizeof(char),64))),_tmpA5D)))),_tmpA5B)))));}_LL0:;}
return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmpD1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD3;void*_tmpD4;_LL14: {
struct Cyc_Absyn_PointerType_struct*_tmpD2=(struct Cyc_Absyn_PointerType_struct*)
_tmpD1;if(_tmpD2->tag != 5)goto _LL16;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD3.elt_typ;}}
_LL15: return Cyc_Toc_getFunctionType(fS,_tmpD4);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct
_tmpA63;const char*_tmpA62;struct Cyc_Core_Impossible_struct*_tmpA61;(int)_throw((
void*)((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA63.tag=Cyc_Core_Impossible,((
_tmpA63.f1=((_tmpA62="impossible type (not pointer)",_tag_dyneither(_tmpA62,
sizeof(char),30))),_tmpA63)))),_tmpA61)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmpD8=e->r;union Cyc_Absyn_Cnst
_tmpDA;struct _tuple3 _tmpDB;char _tmpDC;union Cyc_Absyn_Cnst _tmpDE;struct _tuple4
_tmpDF;short _tmpE0;union Cyc_Absyn_Cnst _tmpE2;struct _tuple5 _tmpE3;int _tmpE4;union
Cyc_Absyn_Cnst _tmpE6;struct _tuple6 _tmpE7;long long _tmpE8;union Cyc_Absyn_Cnst
_tmpEA;int _tmpEB;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*
_tmpF1;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*
_tmpF7;_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL1B;else{_tmpDA=_tmpD9->f1;if((_tmpDA.Char_c).tag
!= 2)goto _LL1B;_tmpDB=(struct _tuple3)(_tmpDA.Char_c).val;_tmpDC=_tmpDB.f2;}}
_LL1A: return _tmpDC == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(
struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpDD->tag != 0)goto _LL1D;else{_tmpDE=
_tmpDD->f1;if((_tmpDE.Short_c).tag != 3)goto _LL1D;_tmpDF=(struct _tuple4)(_tmpDE.Short_c).val;
_tmpE0=_tmpDF.f2;}}_LL1C: return _tmpE0 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE1=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE1->tag != 0)goto _LL1F;
else{_tmpE2=_tmpE1->f1;if((_tmpE2.Int_c).tag != 4)goto _LL1F;_tmpE3=(struct _tuple5)(
_tmpE2.Int_c).val;_tmpE4=_tmpE3.f2;}}_LL1E: return _tmpE4 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE5=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE5->tag != 0)goto _LL21;
else{_tmpE6=_tmpE5->f1;if((_tmpE6.LongLong_c).tag != 5)goto _LL21;_tmpE7=(struct
_tuple6)(_tmpE6.LongLong_c).val;_tmpE8=_tmpE7.f2;}}_LL20: return _tmpE8 == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpE9=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;
if(_tmpE9->tag != 0)goto _LL23;else{_tmpEA=_tmpE9->f1;if((_tmpEA.Null_c).tag != 1)
goto _LL23;_tmpEB=(int)(_tmpEA.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpEC=(struct Cyc_Absyn_Cast_e_struct*)_tmpD8;if(_tmpEC->tag != 15)goto _LL25;
else{_tmpED=_tmpEC->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpED);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpEE=(struct Cyc_Absyn_Tuple_e_struct*)_tmpD8;if(_tmpEE->tag != 25)goto _LL27;
else{_tmpEF=_tmpEE->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEF);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF0=(struct Cyc_Absyn_Array_e_struct*)_tmpD8;if(
_tmpF0->tag != 27)goto _LL29;else{_tmpF1=_tmpF0->f1;}}_LL28: _tmpF3=_tmpF1;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpD8;if(_tmpF2->tag != 29)goto _LL2B;else{_tmpF3=_tmpF2->f3;}}_LL2A: _tmpF5=
_tmpF3;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF4=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD8;if(_tmpF4->tag != 26)goto _LL2D;else{_tmpF5=_tmpF4->f2;}}_LL2C: _tmpF7=
_tmpF5;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF6=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD8;if(_tmpF6->tag != 36)goto _LL2F;else{
_tmpF7=_tmpF6->f2;}}_LL2E: for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpF7->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t);static int Cyc_Toc_is_nullable(void*t){void*
_tmpF8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFA;struct Cyc_Absyn_PtrAtts
_tmpFB;union Cyc_Absyn_Constraint*_tmpFC;_LL32: {struct Cyc_Absyn_PointerType_struct*
_tmpF9=(struct Cyc_Absyn_PointerType_struct*)_tmpF8;if(_tmpF9->tag != 5)goto _LL34;
else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.ptr_atts;_tmpFC=_tmpFB.nullable;}}_LL33:
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFC);
_LL34:;_LL35: {const char*_tmpA66;void*_tmpA65;(_tmpA65=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA66="is_nullable",
_tag_dyneither(_tmpA66,sizeof(char),12))),_tag_dyneither(_tmpA65,sizeof(void*),0)));}
_LL31:;}static char _tmp10B[1]="";static char _tmp10C[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp100;struct Cyc_Dict_Dict*_tmp101;
struct Cyc_Toc_TocState _tmpFF=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp100=_tmpFF.dyn;_tmp101=_tmpFF.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10C,_tmp10C,_tmp10C + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10B,_tmp10B,
_tmp10B + 1}};{struct _tuple13 _tmpA67;pair=((_tmpA67.f1=x,((_tmpA67.f2=tag,_tmpA67))));}{
struct _DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,
_tmp100);{struct _tuple0**_tmp104=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp101,(struct _tuple13*)& pair);if(
_tmp104 != 0){struct _tuple0*_tmp105=*_tmp104;_npop_handler(0);return _tmp105;}{
struct _tuple13*_tmpA68;struct _tuple13*_tmp106=(_tmpA68=_cycalloc(sizeof(*_tmpA68)),((
_tmpA68->f1=x,((_tmpA68->f2=tag,_tmpA68)))));struct _dyneither_ptr*_tmpA6B;struct
_tuple0*_tmpA6A;struct _tuple0*res=(_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->f1=(*
x).f1,((_tmpA6A->f2=((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA6B)))),_tmpA6A)))));*_tmp101=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp101,(
struct _tuple13*)_tmp106,res);{struct _tuple0*_tmp107=res;_npop_handler(0);return
_tmp107;};};};_pop_dynregion(d);};};}struct _tuple16{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp10E;struct Cyc_List_List**
_tmp10F;struct Cyc_Toc_TocState _tmp10D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10E=_tmp10D.dyn;_tmp10F=_tmp10D.tuple_types;{struct _DynRegionFrame _tmp110;
struct _RegionHandle*d=_open_dynregion(& _tmp110,_tmp10E);{struct Cyc_List_List*
_tmp111=*_tmp10F;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct _tuple16 _tmp113;
void*_tmp114;struct Cyc_List_List*_tmp115;struct _tuple16*_tmp112=(struct _tuple16*)
_tmp111->hd;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{struct Cyc_List_List*
_tmp116=tqs0;for(0;_tmp116 != 0  && _tmp115 != 0;(_tmp116=_tmp116->tl,_tmp115=
_tmp115->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp116->hd)).f2,(void*)
_tmp115->hd))break;}if(_tmp116 == 0  && _tmp115 == 0){void*_tmp117=_tmp114;
_npop_handler(0);return _tmp117;}};}}{struct Cyc_Int_pa_struct _tmpA73;void*_tmpA72[
1];const char*_tmpA71;struct _dyneither_ptr*_tmpA70;struct _dyneither_ptr*xname=(
_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=(struct _dyneither_ptr)((_tmpA73.tag=
1,((_tmpA73.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA72[0]=& _tmpA73,
Cyc_aprintf(((_tmpA71="_tuple%d",_tag_dyneither(_tmpA71,sizeof(char),9))),
_tag_dyneither(_tmpA72,sizeof(void*),1)))))))),_tmpA70)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA76;struct Cyc_List_List*
_tmpA75;_tmp118=((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->hd=((_tmpA76=
_cycalloc(sizeof(*_tmpA76)),((_tmpA76->name=Cyc_Absyn_fieldname(i),((_tmpA76->tq=
Cyc_Toc_mt_tq,((_tmpA76->type=(void*)ts2->hd,((_tmpA76->width=0,((_tmpA76->attributes=
0,_tmpA76)))))))))))),((_tmpA75->tl=_tmp118,_tmpA75))))));}}_tmp118=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp118);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA7B;struct _tuple0*_tmpA7A;struct Cyc_Absyn_Aggrdecl*_tmpA79;struct Cyc_Absyn_Aggrdecl*
_tmp11B=(_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->kind=Cyc_Absyn_StructA,((
_tmpA79->sc=Cyc_Absyn_Public,((_tmpA79->name=((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((
_tmpA7A->f1=Cyc_Absyn_Rel_n(0),((_tmpA7A->f2=xname,_tmpA7A)))))),((_tmpA79->tvs=
0,((_tmpA79->impl=((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->exist_vars=0,((
_tmpA7B->rgn_po=0,((_tmpA7B->fields=_tmp118,((_tmpA7B->tagged=0,_tmpA7B)))))))))),((
_tmpA79->attributes=0,_tmpA79)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA81;struct Cyc_Absyn_Aggr_d_struct _tmpA80;struct Cyc_List_List*_tmpA7F;Cyc_Toc_result_decls=((
_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->hd=Cyc_Absyn_new_decl((void*)((
_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA80.tag=6,((_tmpA80.f1=
_tmp11B,_tmpA80)))),_tmpA81)))),0),((_tmpA7F->tl=Cyc_Toc_result_decls,_tmpA7F))))));}{
struct _tuple16*_tmpA84;struct Cyc_List_List*_tmpA83;*_tmp10F=((_tmpA83=
_region_malloc(d,sizeof(*_tmpA83)),((_tmpA83->hd=((_tmpA84=_region_malloc(d,
sizeof(*_tmpA84)),((_tmpA84->f1=x,((_tmpA84->f2=ts,_tmpA84)))))),((_tmpA83->tl=*
_tmp10F,_tmpA83))))));}{void*_tmp121=x;_npop_handler(0);return _tmp121;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp129=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpA91;const char*
_tmpA90;void*_tmpA8F[1];struct Cyc_Int_pa_struct _tmpA8E;struct _tuple0*_tmpA8D;
struct _tuple0*res=(_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->f1=Cyc_Absyn_Loc_n,((
_tmpA8D->f2=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=(struct
_dyneither_ptr)((_tmpA8E.tag=1,((_tmpA8E.f1=(unsigned int)_tmp129,((_tmpA8F[0]=&
_tmpA8E,Cyc_aprintf(((_tmpA90="_tmp%X",_tag_dyneither(_tmpA90,sizeof(char),7))),
_tag_dyneither(_tmpA8F,sizeof(void*),1)))))))),_tmpA91)))),_tmpA8D)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp130;struct Cyc_Xarray_Xarray*
_tmp131;struct Cyc_Toc_TocState _tmp12F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp130=_tmp12F.dyn;_tmp131=_tmp12F.temp_labels;{struct _DynRegionFrame _tmp132;
struct _RegionHandle*d=_open_dynregion(& _tmp132,_tmp130);{int _tmp133=Cyc_Toc_fresh_label_counter
++;if(_tmp133 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp131)){
struct _dyneither_ptr*_tmp134=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp131,_tmp133);_npop_handler(0);return _tmp134;}{struct Cyc_Int_pa_struct
_tmpA99;void*_tmpA98[1];const char*_tmpA97;struct _dyneither_ptr*_tmpA96;struct
_dyneither_ptr*res=(_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=(struct
_dyneither_ptr)((_tmpA99.tag=1,((_tmpA99.f1=(unsigned int)_tmp133,((_tmpA98[0]=&
_tmpA99,Cyc_aprintf(((_tmpA97="_LL%X",_tag_dyneither(_tmpA97,sizeof(char),6))),
_tag_dyneither(_tmpA98,sizeof(void*),1)))))))),_tmpA96)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp131,res)!= _tmp133){const char*
_tmpA9C;void*_tmpA9B;(_tmpA9B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9C="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpA9C,sizeof(char),43))),_tag_dyneither(_tmpA9B,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp137=res;_npop_handler(0);return _tmp137;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp13C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13C))->hd)->name)
!= 0){++ ans;_tmp13C=_tmp13C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a);static struct _tuple7*Cyc_Toc_arg_to_c(struct _tuple7*a){struct Cyc_Core_Opt*
_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;struct _tuple7 _tmp13D=*a;
_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;_tmp140=_tmp13D.f3;{struct _tuple7*_tmpA9D;
return(_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->f1=_tmp13E,((_tmpA9D->f2=
_tmp13F,((_tmpA9D->f3=Cyc_Toc_typ_to_c(_tmp140),_tmpA9D)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp143;void*_tmp144;struct _tuple9 _tmp142=*
x;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;{struct _tuple9*_tmpA9E;return(_tmpA9E=
_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->f1=_tmp143,((_tmpA9E->f2=Cyc_Toc_typ_to_c(
_tmp144),_tmpA9E)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
Cyc_Toc_typ_to_c_array(void*t){void*_tmp146=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp148;void*_tmp149;struct Cyc_Absyn_Tqual _tmp14A;struct Cyc_Absyn_Exp*_tmp14B;
union Cyc_Absyn_Constraint*_tmp14C;struct Cyc_Position_Segment*_tmp14D;struct Cyc_Core_Opt*
_tmp14F;struct Cyc_Core_Opt _tmp150;void*_tmp151;_LL37: {struct Cyc_Absyn_ArrayType_struct*
_tmp147=(struct Cyc_Absyn_ArrayType_struct*)_tmp146;if(_tmp147->tag != 9)goto _LL39;
else{_tmp148=_tmp147->f1;_tmp149=_tmp148.elt_type;_tmp14A=_tmp148.tq;_tmp14B=
_tmp148.num_elts;_tmp14C=_tmp148.zero_term;_tmp14D=_tmp148.zt_loc;}}_LL38: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp149),_tmp14A,_tmp14B,Cyc_Absyn_false_conref,
_tmp14D);_LL39: {struct Cyc_Absyn_Evar_struct*_tmp14E=(struct Cyc_Absyn_Evar_struct*)
_tmp146;if(_tmp14E->tag != 1)goto _LL3B;else{_tmp14F=_tmp14E->f2;if(_tmp14F == 0)
goto _LL3B;_tmp150=*_tmp14F;_tmp151=(void*)_tmp150.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(
_tmp151);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(t);_LL36:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpA9F;return(_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->name=f->name,((
_tmpA9F->tq=Cyc_Toc_mt_tq,((_tmpA9F->type=Cyc_Toc_typ_to_c(f->type),((_tmpA9F->width=
f->width,((_tmpA9F->attributes=f->attributes,_tmpA9F)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAA0;cs=((_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((
_tmpAA0[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAA0))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAA1;r=((_tmpAA1=
_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAA1))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAA2;r=((_tmpAA2=
_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAA2))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
void*Cyc_Toc_typ_to_c(void*t){void*_tmp156=t;struct Cyc_Core_Opt*_tmp159;struct
Cyc_Core_Opt*_tmp15B;struct Cyc_Core_Opt _tmp15C;void*_tmp15D;struct Cyc_Absyn_Tvar*
_tmp15F;struct Cyc_Absyn_DatatypeFieldInfo _tmp162;union Cyc_Absyn_DatatypeFieldInfoU
_tmp163;struct _tuple1 _tmp164;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*
_tmp166;struct Cyc_Absyn_PtrInfo _tmp169;void*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;
struct Cyc_Absyn_PtrAtts _tmp16C;union Cyc_Absyn_Constraint*_tmp16D;struct Cyc_Absyn_ArrayInfo
_tmp172;void*_tmp173;struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;
struct Cyc_Position_Segment*_tmp176;struct Cyc_Absyn_FnInfo _tmp178;void*_tmp179;
struct Cyc_List_List*_tmp17A;int _tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17C;struct
Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17F;enum Cyc_Absyn_AggrKind _tmp181;
struct Cyc_List_List*_tmp182;struct Cyc_Absyn_AggrInfo _tmp184;union Cyc_Absyn_AggrInfoU
_tmp185;struct Cyc_List_List*_tmp186;struct _tuple0*_tmp188;struct Cyc_List_List*
_tmp18A;struct _tuple0*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Typedefdecl*
_tmp18E;void**_tmp18F;void*_tmp192;_LL3E: {struct Cyc_Absyn_VoidType_struct*
_tmp157=(struct Cyc_Absyn_VoidType_struct*)_tmp156;if(_tmp157->tag != 0)goto _LL40;}
_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*_tmp158=(struct Cyc_Absyn_Evar_struct*)
_tmp156;if(_tmp158->tag != 1)goto _LL42;else{_tmp159=_tmp158->f2;if(_tmp159 != 0)
goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;_LL42: {struct Cyc_Absyn_Evar_struct*
_tmp15A=(struct Cyc_Absyn_Evar_struct*)_tmp156;if(_tmp15A->tag != 1)goto _LL44;
else{_tmp15B=_tmp15A->f2;if(_tmp15B == 0)goto _LL44;_tmp15C=*_tmp15B;_tmp15D=(void*)
_tmp15C.v;}}_LL43: return Cyc_Toc_typ_to_c(_tmp15D);_LL44: {struct Cyc_Absyn_VarType_struct*
_tmp15E=(struct Cyc_Absyn_VarType_struct*)_tmp156;if(_tmp15E->tag != 2)goto _LL46;
else{_tmp15F=_tmp15E->f1;}}_LL45: if(Cyc_Tcutil_tvar_kind(_tmp15F,Cyc_Absyn_BoxKind)
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp156;if(_tmp160->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp161=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp156;if(_tmp161->tag != 4)goto _LL4A;else{_tmp162=_tmp161->f1;_tmp163=_tmp162.field_info;
if((_tmp163.KnownDatatypefield).tag != 2)goto _LL4A;_tmp164=(struct _tuple1)(
_tmp163.KnownDatatypefield).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;}}_LL49: {
const char*_tmpAA3;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp166->name,((
_tmpAA3="_struct",_tag_dyneither(_tmpAA3,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp167=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp156;if(_tmp167->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAA6;void*_tmpAA5;(_tmpAA5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA6="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAA6,sizeof(char),29))),_tag_dyneither(_tmpAA5,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp168=(struct Cyc_Absyn_PointerType_struct*)
_tmp156;if(_tmp168->tag != 5)goto _LL4E;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.elt_typ;
_tmp16B=_tmp169.elt_tq;_tmp16C=_tmp169.ptr_atts;_tmp16D=_tmp16C.bounds;}}_LL4D:
_tmp16A=Cyc_Toc_typ_to_c_array(_tmp16A);{void*_tmp19D=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16D);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp19E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19D;if(_tmp19E->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16A,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16B,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp16E=(struct Cyc_Absyn_IntType_struct*)_tmp156;if(_tmp16E->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp16F=(struct Cyc_Absyn_FloatType_struct*)
_tmp156;if(_tmp16F->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp170=(struct Cyc_Absyn_DoubleType_struct*)_tmp156;if(_tmp170->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp171=(struct Cyc_Absyn_ArrayType_struct*)
_tmp156;if(_tmp171->tag != 9)goto _LL56;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.elt_type;
_tmp174=_tmp172.tq;_tmp175=_tmp172.num_elts;_tmp176=_tmp172.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp173),_tmp174,_tmp175,Cyc_Absyn_false_conref,
_tmp176);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp177=(struct Cyc_Absyn_FnType_struct*)
_tmp156;if(_tmp177->tag != 10)goto _LL58;else{_tmp178=_tmp177->f1;_tmp179=_tmp178.ret_typ;
_tmp17A=_tmp178.args;_tmp17B=_tmp178.c_varargs;_tmp17C=_tmp178.cyc_varargs;
_tmp17D=_tmp178.attributes;}}_LL57: {struct Cyc_List_List*_tmp19F=0;for(0;_tmp17D
!= 0;_tmp17D=_tmp17D->tl){void*_tmp1A0=(void*)_tmp17D->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A1=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A2=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A0;if(_tmp1A2->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A3=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A0;if(_tmp1A3->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1A4=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1A0;if(_tmp1A4->tag != 20)
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAA7;_tmp19F=((
_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=(void*)_tmp17D->hd,((_tmpAA7->tl=
_tmp19F,_tmpAA7))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A6=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17A);if(_tmp17C != 0){void*_tmp1A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAA8;struct _tuple7*_tmp1A8=(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((
_tmpAA8->f1=_tmp17C->name,((_tmpAA8->f2=_tmp17C->tq,((_tmpAA8->f3=_tmp1A7,
_tmpAA8)))))));struct Cyc_List_List*_tmpAA9;_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A6,((
_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->hd=_tmp1A8,((_tmpAA9->tl=0,
_tmpAA9)))))));}{struct Cyc_Absyn_FnType_struct _tmpAAF;struct Cyc_Absyn_FnInfo
_tmpAAE;struct Cyc_Absyn_FnType_struct*_tmpAAD;return(void*)((_tmpAAD=_cycalloc(
sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAF.tag=10,((_tmpAAF.f1=((_tmpAAE.tvars=0,((
_tmpAAE.effect=0,((_tmpAAE.ret_typ=Cyc_Toc_typ_to_c(_tmp179),((_tmpAAE.args=
_tmp1A6,((_tmpAAE.c_varargs=_tmp17B,((_tmpAAE.cyc_varargs=0,((_tmpAAE.rgn_po=0,((
_tmpAAE.attributes=_tmp19F,_tmpAAE)))))))))))))))),_tmpAAF)))),_tmpAAD))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp17E=(struct Cyc_Absyn_TupleType_struct*)
_tmp156;if(_tmp17E->tag != 11)goto _LL5A;else{_tmp17F=_tmp17E->f1;}}_LL59: _tmp17F=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp17F);return Cyc_Toc_add_tuple_type(_tmp17F);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp180=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp156;if(_tmp180->tag != 13)goto _LL5C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpAB2;struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB1;return(void*)((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((
_tmpAB2.tag=13,((_tmpAB2.f1=_tmp181,((_tmpAB2.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp182),_tmpAB2)))))),_tmpAB1))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp183=(struct Cyc_Absyn_AggrType_struct*)
_tmp156;if(_tmp183->tag != 12)goto _LL5E;else{_tmp184=_tmp183->f1;_tmp185=_tmp184.aggr_info;
_tmp186=_tmp184.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B0=_tmp185;struct
_tuple2 _tmp1B1;_LL87: if((_tmp1B0.UnknownAggr).tag != 1)goto _LL89;_tmp1B1=(struct
_tuple2)(_tmp1B0.UnknownAggr).val;_LL88: return t;_LL89:;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(_tmp185);if(
_tmp1B2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp187=(struct Cyc_Absyn_EnumType_struct*)
_tmp156;if(_tmp187->tag != 14)goto _LL60;else{_tmp188=_tmp187->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp189=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp156;if(_tmp189->tag != 15)goto _LL62;else{_tmp18A=_tmp189->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18A);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp156;if(_tmp18B->tag != 18)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LL63: if(_tmp18F == 0  || Cyc_noexpand_r){
if(_tmp18D != 0){struct Cyc_Absyn_TypedefType_struct _tmpAB5;struct Cyc_Absyn_TypedefType_struct*
_tmpAB4;return(void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((
_tmpAB5.tag=18,((_tmpAB5.f1=_tmp18C,((_tmpAB5.f2=0,((_tmpAB5.f3=_tmp18E,((
_tmpAB5.f4=0,_tmpAB5)))))))))),_tmpAB4))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpABB;void**_tmpABA;struct Cyc_Absyn_TypedefType_struct*_tmpAB9;return(void*)((
_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABB.tag=18,((_tmpABB.f1=
_tmp18C,((_tmpABB.f2=0,((_tmpABB.f3=_tmp18E,((_tmpABB.f4=((_tmpABA=_cycalloc(
sizeof(*_tmpABA)),((_tmpABA[0]=Cyc_Toc_typ_to_c_array(*_tmp18F),_tmpABA)))),
_tmpABB)))))))))),_tmpAB9))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp190=(
struct Cyc_Absyn_TagType_struct*)_tmp156;if(_tmp190->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp191=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp156;if(_tmp191->tag != 16)goto _LL68;
else{_tmp192=(void*)_tmp191->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp193=(struct Cyc_Absyn_DynRgnType_struct*)_tmp156;
if(_tmp193->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp194=(struct Cyc_Absyn_HeapRgn_struct*)_tmp156;if(
_tmp194->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpABE;void*_tmpABD;(_tmpABD=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpABE="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpABE,sizeof(char),55))),_tag_dyneither(_tmpABD,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp195=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp156;if(_tmp195->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpAC1;void*_tmpAC0;(
_tmpAC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC1="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpAC1,sizeof(char),57))),_tag_dyneither(_tmpAC0,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp196=(struct Cyc_Absyn_AccessEff_struct*)
_tmp156;if(_tmp196->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp197=(struct Cyc_Absyn_JoinEff_struct*)_tmp156;if(_tmp197->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp198=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp156;if(_tmp198->tag != 25)goto _LL74;}_LL73: {const char*_tmpAC4;void*_tmpAC3;(
_tmpAC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC4="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAC4,sizeof(char),49))),_tag_dyneither(_tmpAC3,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp199=(struct Cyc_Absyn_ValueofType_struct*)
_tmp156;if(_tmp199->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAC7;void*_tmpAC6;(
_tmpAC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC7="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAC7,sizeof(char),51))),_tag_dyneither(_tmpAC6,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C0=t;struct
Cyc_Absyn_ArrayInfo _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;_LL8C: {
struct Cyc_Absyn_ArrayType_struct*_tmp1C1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C0;if(_tmp1C1->tag != 9)goto _LL8E;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C2.elt_type;
_tmp1C4=_tmp1C2.tq;}}_LL8D: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C3,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1C4,Cyc_Absyn_false_conref),e);_LL8E:;_LL8F:
return Cyc_Toc_cast_it(t,e);_LL8B:;}static int Cyc_Toc_atomic_typ(void*t);static int
Cyc_Toc_atomic_typ(void*t){void*_tmp1C5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp1D0;void*_tmp1D1;struct Cyc_Absyn_AggrInfo _tmp1D3;union Cyc_Absyn_AggrInfoU
_tmp1D4;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1D8;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1D9;struct _tuple1 _tmp1DA;struct Cyc_Absyn_Datatypedecl*
_tmp1DB;struct Cyc_Absyn_Datatypefield*_tmp1DC;struct Cyc_List_List*_tmp1DE;_LL91: {
struct Cyc_Absyn_VoidType_struct*_tmp1C6=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C5;if(_tmp1C6->tag != 0)goto _LL93;}_LL92: return 1;_LL93: {struct Cyc_Absyn_VarType_struct*
_tmp1C7=(struct Cyc_Absyn_VarType_struct*)_tmp1C5;if(_tmp1C7->tag != 2)goto _LL95;}
_LL94: return 0;_LL95: {struct Cyc_Absyn_IntType_struct*_tmp1C8=(struct Cyc_Absyn_IntType_struct*)
_tmp1C5;if(_tmp1C8->tag != 6)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_EnumType_struct*
_tmp1C9=(struct Cyc_Absyn_EnumType_struct*)_tmp1C5;if(_tmp1C9->tag != 14)goto _LL99;}
_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CA=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C5;if(_tmp1CA->tag != 15)goto _LL9B;}_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_FloatType_struct*
_tmp1CB=(struct Cyc_Absyn_FloatType_struct*)_tmp1C5;if(_tmp1CB->tag != 7)goto _LL9D;}
_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_DoubleType_struct*_tmp1CC=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C5;if(_tmp1CC->tag != 8)goto _LL9F;}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_FnType_struct*
_tmp1CD=(struct Cyc_Absyn_FnType_struct*)_tmp1C5;if(_tmp1CD->tag != 10)goto _LLA1;}
_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_TagType_struct*_tmp1CE=(struct Cyc_Absyn_TagType_struct*)
_tmp1C5;if(_tmp1CE->tag != 20)goto _LLA3;}_LLA2: return 1;_LLA3: {struct Cyc_Absyn_ArrayType_struct*
_tmp1CF=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C5;if(_tmp1CF->tag != 9)goto _LLA5;
else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1D0.elt_type;}}_LLA4: return Cyc_Toc_atomic_typ(
_tmp1D1);_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp1D2=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C5;if(_tmp1D2->tag != 12)goto _LLA7;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.aggr_info;}}
_LLA6:{union Cyc_Absyn_AggrInfoU _tmp1E3=_tmp1D4;struct _tuple2 _tmp1E4;_LLB8: if((
_tmp1E3.UnknownAggr).tag != 1)goto _LLBA;_tmp1E4=(struct _tuple2)(_tmp1E3.UnknownAggr).val;
_LLB9: return 0;_LLBA:;_LLBB: goto _LLB7;_LLB7:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E5=
Cyc_Absyn_get_known_aggrdecl(_tmp1D4);if(_tmp1E5->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;for(
0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E6->hd)->type))return 0;}}return 1;};_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D5=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C5;if(_tmp1D5->tag != 13)goto
_LLA9;else{_tmp1D6=_tmp1D5->f2;}}_LLA8: for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D6->hd)->type))return 0;}
return 1;_LLA9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1D7=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C5;if(_tmp1D7->tag != 4)goto _LLAB;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.field_info;
if((_tmp1D9.KnownDatatypefield).tag != 2)goto _LLAB;_tmp1DA=(struct _tuple1)(
_tmp1D9.KnownDatatypefield).val;_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;}}_LLAA:
_tmp1DE=_tmp1DC->typs;goto _LLAC;_LLAB: {struct Cyc_Absyn_TupleType_struct*_tmp1DD=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C5;if(_tmp1DD->tag != 11)goto _LLAD;else{
_tmp1DE=_tmp1DD->f1;}}_LLAC: for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1DE->hd)).f2))return 0;}return 1;_LLAD: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1DF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C5;if(_tmp1DF->tag != 3)goto
_LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_PointerType_struct*_tmp1E0=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C5;if(_tmp1E0->tag != 5)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C5;if(_tmp1E1->tag != 17)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E2=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C5;if(_tmp1E2->tag != 16)goto
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpACB;void*_tmpACA[1];struct Cyc_String_pa_struct
_tmpAC9;(_tmpAC9.tag=0,((_tmpAC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpACA[0]=& _tmpAC9,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACB="atomic_typ:  bad type %s",
_tag_dyneither(_tmpACB,sizeof(char),25))),_tag_dyneither(_tmpACA,sizeof(void*),1)))))));}
_LL90:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1EA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EC;void*
_tmp1ED;_LLBD: {struct Cyc_Absyn_PointerType_struct*_tmp1EB=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EA;if(_tmp1EB->tag != 5)goto _LLBF;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EC.elt_typ;}}
_LLBE: {void*_tmp1EE=Cyc_Tcutil_compress(_tmp1ED);_LLC2: {struct Cyc_Absyn_VoidType_struct*
_tmp1EF=(struct Cyc_Absyn_VoidType_struct*)_tmp1EE;if(_tmp1EF->tag != 0)goto _LLC4;}
_LLC3: return 1;_LLC4:;_LLC5: return 0;_LLC1:;}_LLBF:;_LLC0: return 0;_LLBC:;}static
int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1F0=Cyc_Tcutil_compress(
t);_LLC7: {struct Cyc_Absyn_VarType_struct*_tmp1F1=(struct Cyc_Absyn_VarType_struct*)
_tmp1F0;if(_tmp1F1->tag != 2)goto _LLC9;}_LLC8: return 1;_LLC9:;_LLCA: return 0;_LLC6:;}
static int Cyc_Toc_is_void_star_or_tvar(void*t);static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp1F2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F4;union Cyc_Absyn_AggrInfoU _tmp1F5;struct Cyc_List_List*_tmp1F7;_LLCC: {
struct Cyc_Absyn_AggrType_struct*_tmp1F3=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F2;if(_tmp1F3->tag != 12)goto _LLCE;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F4.aggr_info;}}
_LLCD: {struct Cyc_Absyn_Aggrdecl*_tmp1F8=Cyc_Absyn_get_known_aggrdecl(_tmp1F5);
if(_tmp1F8->impl == 0){const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="is_poly_field: type missing fields",
_tag_dyneither(_tmpACE,sizeof(char),35))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}
_tmp1F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F8->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F2;if(_tmp1F6->tag != 13)goto _LLD0;else{_tmp1F7=_tmp1F6->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FB=Cyc_Absyn_lookup_field(_tmp1F7,f);if(_tmp1FB == 0){
const char*_tmpAD2;void*_tmpAD1[1];struct Cyc_String_pa_struct _tmpAD0;(_tmpAD0.tag=
0,((_tmpAD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAD1[0]=&
_tmpAD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD2="is_poly_field: bad field %s",_tag_dyneither(_tmpAD2,sizeof(char),28))),
_tag_dyneither(_tmpAD1,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FB->type);}_LLD0:;_LLD1: {const char*_tmpAD6;void*_tmpAD5[1];struct Cyc_String_pa_struct
_tmpAD4;(_tmpAD4.tag=0,((_tmpAD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAD5[0]=& _tmpAD4,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD6="is_poly_field: bad type %s",
_tag_dyneither(_tmpAD6,sizeof(char),27))),_tag_dyneither(_tmpAD5,sizeof(void*),1)))))));}
_LLCB:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp202=e->r;struct Cyc_Absyn_Exp*_tmp204;struct
_dyneither_ptr*_tmp205;struct Cyc_Absyn_Exp*_tmp207;struct _dyneither_ptr*_tmp208;
_LLD3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp203=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp202;if(_tmp203->tag != 22)goto _LLD5;else{_tmp204=_tmp203->f1;_tmp205=_tmp203->f2;}}
_LLD4: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp204->topt))->v,_tmp205);_LLD5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp206=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp202;if(_tmp206->tag != 23)goto _LLD7;else{
_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;}}_LLD6: {void*_tmp209=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20B;void*_tmp20C;_LLDA: {struct Cyc_Absyn_PointerType_struct*_tmp20A=(struct
Cyc_Absyn_PointerType_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LLDC;else{_tmp20B=
_tmp20A->f1;_tmp20C=_tmp20B.elt_typ;}}_LLDB: return Cyc_Toc_is_poly_field(_tmp20C,
_tmp208);_LLDC:;_LLDD: {const char*_tmpADA;void*_tmpAD9[1];struct Cyc_String_pa_struct
_tmpAD8;(_tmpAD8.tag=0,((_tmpAD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v)),((
_tmpAD9[0]=& _tmpAD8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADA="is_poly_project: bad type %s",_tag_dyneither(_tmpADA,sizeof(char),29))),
_tag_dyneither(_tmpAD9,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpADB;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpADB=_cycalloc(
sizeof(*_tmpADB)),((_tmpADB->hd=s,((_tmpADB->tl=0,_tmpADB)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpADC;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpADC=_cycalloc(sizeof(*
_tmpADC)),((_tmpADC->hd=s,((_tmpADC->tl=0,_tmpADC)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpADD[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpADD[1]=s,((_tmpADD[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpADE[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpADE[1]=n,((
_tmpADE[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpADF[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpADF[1]=n,((
_tmpADF[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAE0[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAE0[2]=n,((_tmpAE0[1]=s,((_tmpAE0[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE0,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAE1;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->hd=e,((_tmpAE1->tl=0,_tmpAE1)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAE2;throw_match_stmt_opt=((
_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAE2))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp218=e->r;union Cyc_Absyn_Cnst _tmp21A;struct _dyneither_ptr _tmp21B;_LLDF: {
struct Cyc_Absyn_Const_e_struct*_tmp219=(struct Cyc_Absyn_Const_e_struct*)_tmp218;
if(_tmp219->tag != 0)goto _LLE1;else{_tmp21A=_tmp219->f1;if((_tmp21A.String_c).tag
!= 7)goto _LLE1;_tmp21B=(struct _dyneither_ptr)(_tmp21A.String_c).val;}}_LLE0:
is_string=1;goto _LLDE;_LLE1:;_LLE2: goto _LLDE;_LLDE:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAE8;struct
Cyc_Absyn_Var_d_struct _tmpAE7;struct Cyc_List_List*_tmpAE6;Cyc_Toc_result_decls=((
_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->hd=Cyc_Absyn_new_decl((void*)((
_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE7.tag=0,((_tmpAE7.f1=vd,
_tmpAE7)))),_tmpAE8)))),0),((_tmpAE6->tl=Cyc_Toc_result_decls,_tmpAE6))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpAEF;
struct _tuple15*_tmpAEE;struct _tuple15*_tmpAED;struct _tuple15*_tmpAEC[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpAEC[2]=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((
_tmpAED->f1=0,((_tmpAED->f2=xplussz,_tmpAED)))))),((_tmpAEC[1]=((_tmpAEE=
_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->f1=0,((_tmpAEE->f2=xexp,_tmpAEE)))))),((
_tmpAEC[0]=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->f1=0,((_tmpAEF->f2=
xexp,_tmpAEF)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAEC,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp224;int _tmp225;struct Cyc_Toc_Env*
_tmp223=nv;_tmp224=*_tmp223;_tmp225=_tmp224.toplevel;return _tmp225;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Toc_Env _tmp227;struct Cyc_Dict_Dict _tmp228;struct Cyc_Toc_Env*_tmp226=nv;
_tmp227=*_tmp226;_tmp228=_tmp227.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp228,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpAF0;return(_tmpAF0=_region_malloc(r,
sizeof(*_tmpAF0)),((_tmpAF0->break_lab=(struct _dyneither_ptr**)0,((_tmpAF0->continue_lab=(
struct _dyneither_ptr**)0,((_tmpAF0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpAF0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpAF0->toplevel=(int)1,((_tmpAF0->rgn=(struct _RegionHandle*)r,_tmpAF0)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22B;struct _dyneither_ptr**_tmp22C;struct _dyneither_ptr**
_tmp22D;struct Cyc_Toc_FallthruInfo*_tmp22E;struct Cyc_Dict_Dict _tmp22F;int _tmp230;
struct Cyc_Toc_Env*_tmp22A=e;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.break_lab;_tmp22D=
_tmp22B.continue_lab;_tmp22E=_tmp22B.fallthru_info;_tmp22F=_tmp22B.varmap;
_tmp230=_tmp22B.toplevel;{struct Cyc_Toc_Env*_tmpAF1;return(_tmpAF1=
_region_malloc(r,sizeof(*_tmpAF1)),((_tmpAF1->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22C),((_tmpAF1->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22D),((_tmpAF1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22E,((_tmpAF1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22F),((_tmpAF1->toplevel=(
int)_tmp230,((_tmpAF1->rgn=(struct _RegionHandle*)r,_tmpAF1)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp233;struct _dyneither_ptr**_tmp234;struct
_dyneither_ptr**_tmp235;struct Cyc_Toc_FallthruInfo*_tmp236;struct Cyc_Dict_Dict
_tmp237;int _tmp238;struct Cyc_Toc_Env*_tmp232=e;_tmp233=*_tmp232;_tmp234=_tmp233.break_lab;
_tmp235=_tmp233.continue_lab;_tmp236=_tmp233.fallthru_info;_tmp237=_tmp233.varmap;
_tmp238=_tmp233.toplevel;{struct Cyc_Toc_Env*_tmpAF2;return(_tmpAF2=
_region_malloc(r,sizeof(*_tmpAF2)),((_tmpAF2->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp234),((_tmpAF2->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp235),((_tmpAF2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp236,((_tmpAF2->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp237),((_tmpAF2->toplevel=(
int)0,((_tmpAF2->rgn=(struct _RegionHandle*)r,_tmpAF2)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23B;struct _dyneither_ptr**_tmp23C;struct _dyneither_ptr**
_tmp23D;struct Cyc_Toc_FallthruInfo*_tmp23E;struct Cyc_Dict_Dict _tmp23F;int _tmp240;
struct Cyc_Toc_Env*_tmp23A=e;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.break_lab;_tmp23D=
_tmp23B.continue_lab;_tmp23E=_tmp23B.fallthru_info;_tmp23F=_tmp23B.varmap;
_tmp240=_tmp23B.toplevel;{struct Cyc_Toc_Env*_tmpAF3;return(_tmpAF3=
_region_malloc(r,sizeof(*_tmpAF3)),((_tmpAF3->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23C),((_tmpAF3->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23D),((_tmpAF3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23E,((_tmpAF3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23F),((_tmpAF3->toplevel=(
int)1,((_tmpAF3->rgn=(struct _RegionHandle*)r,_tmpAF3)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp242=(*x).f1;struct Cyc_List_List*_tmp243;_LLE4: if((
_tmp242.Rel_n).tag != 1)goto _LLE6;_tmp243=(struct Cyc_List_List*)(_tmp242.Rel_n).val;
_LLE5: {const char*_tmpAF7;void*_tmpAF6[1];struct Cyc_String_pa_struct _tmpAF5;(
_tmpAF5.tag=0,((_tmpAF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpAF6[0]=& _tmpAF5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpAF7="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpAF7,sizeof(char),30))),_tag_dyneither(_tmpAF6,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp248;struct _dyneither_ptr**_tmp249;
struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict
_tmp24C;int _tmp24D;struct Cyc_Toc_Env*_tmp247=e;_tmp248=*_tmp247;_tmp249=_tmp248.break_lab;
_tmp24A=_tmp248.continue_lab;_tmp24B=_tmp248.fallthru_info;_tmp24C=_tmp248.varmap;
_tmp24D=_tmp248.toplevel;{struct Cyc_Dict_Dict _tmp24E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24C),x,y);struct Cyc_Toc_Env*_tmpAF8;return(_tmpAF8=_region_malloc(r,
sizeof(*_tmpAF8)),((_tmpAF8->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp249),((_tmpAF8->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24A),((_tmpAF8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24B,((_tmpAF8->varmap=(struct Cyc_Dict_Dict)_tmp24E,((_tmpAF8->toplevel=(int)
_tmp24D,((_tmpAF8->rgn=(struct _RegionHandle*)r,_tmpAF8)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp251;struct _dyneither_ptr**_tmp252;struct _dyneither_ptr**
_tmp253;struct Cyc_Toc_FallthruInfo*_tmp254;struct Cyc_Dict_Dict _tmp255;int _tmp256;
struct Cyc_Toc_Env*_tmp250=e;_tmp251=*_tmp250;_tmp252=_tmp251.break_lab;_tmp253=
_tmp251.continue_lab;_tmp254=_tmp251.fallthru_info;_tmp255=_tmp251.varmap;
_tmp256=_tmp251.toplevel;{struct Cyc_Toc_Env*_tmpAF9;return(_tmpAF9=
_region_malloc(r,sizeof(*_tmpAF9)),((_tmpAF9->break_lab=(struct _dyneither_ptr**)
0,((_tmpAF9->continue_lab=(struct _dyneither_ptr**)0,((_tmpAF9->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp254,((_tmpAF9->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp255),((_tmpAF9->toplevel=(int)_tmp256,((_tmpAF9->rgn=(struct _RegionHandle*)
r,_tmpAF9)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpAFA;fallthru_vars=((
_tmpAFA=_region_malloc(r,sizeof(*_tmpAFA)),((_tmpAFA->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpAFA->tl=fallthru_vars,_tmpAFA))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25A;struct _dyneither_ptr**_tmp25B;struct
_dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict
_tmp25E;int _tmp25F;struct Cyc_Toc_Env*_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;
_tmp25C=_tmp25A.continue_lab;_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;
_tmp25F=_tmp25A.toplevel;{struct Cyc_Toc_Env _tmp261;struct Cyc_Dict_Dict _tmp262;
struct Cyc_Toc_Env*_tmp260=next_case_env;_tmp261=*_tmp260;_tmp262=_tmp261.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAFB;struct Cyc_Toc_FallthruInfo*fi=(_tmpAFB=
_region_malloc(r,sizeof(*_tmpAFB)),((_tmpAFB->label=fallthru_l,((_tmpAFB->binders=
fallthru_vars,((_tmpAFB->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262),_tmpAFB)))))));
struct _dyneither_ptr**_tmpAFE;struct Cyc_Toc_Env*_tmpAFD;return(_tmpAFD=
_region_malloc(r,sizeof(*_tmpAFD)),((_tmpAFD->break_lab=(struct _dyneither_ptr**)((
_tmpAFE=_region_malloc(r,sizeof(*_tmpAFE)),((_tmpAFE[0]=break_l,_tmpAFE)))),((
_tmpAFD->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25C),((
_tmpAFD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAFD->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp25E),((_tmpAFD->toplevel=(int)_tmp25F,((_tmpAFD->rgn=(
struct _RegionHandle*)r,_tmpAFD)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp267;struct _dyneither_ptr**
_tmp268;struct _dyneither_ptr**_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct
Cyc_Dict_Dict _tmp26B;int _tmp26C;struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;
_tmp268=_tmp267.break_lab;_tmp269=_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;
_tmp26B=_tmp267.varmap;_tmp26C=_tmp267.toplevel;{struct _dyneither_ptr**_tmpB01;
struct Cyc_Toc_Env*_tmpB00;return(_tmpB00=_region_malloc(r,sizeof(*_tmpB00)),((
_tmpB00->break_lab=(struct _dyneither_ptr**)((_tmpB01=_region_malloc(r,sizeof(*
_tmpB01)),((_tmpB01[0]=break_l,_tmpB01)))),((_tmpB00->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp269),((_tmpB00->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB00->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26B),((_tmpB00->toplevel=(
int)_tmp26C,((_tmpB00->rgn=(struct _RegionHandle*)r,_tmpB00)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp270;struct _dyneither_ptr**_tmp271;struct _dyneither_ptr**_tmp272;struct Cyc_Toc_FallthruInfo*
_tmp273;struct Cyc_Dict_Dict _tmp274;int _tmp275;struct Cyc_Toc_Env*_tmp26F=e;
_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;_tmp272=_tmp270.continue_lab;_tmp273=
_tmp270.fallthru_info;_tmp274=_tmp270.varmap;_tmp275=_tmp270.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB04;struct Cyc_Toc_Env*_tmpB03;return(_tmpB03=_region_malloc(r,sizeof(*
_tmpB03)),((_tmpB03->break_lab=(struct _dyneither_ptr**)0,((_tmpB03->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp272),((_tmpB03->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB04=_region_malloc(r,sizeof(*_tmpB04)),((
_tmpB04->label=next_l,((_tmpB04->binders=0,((_tmpB04->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB04)))))))),((_tmpB03->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp274),((_tmpB03->toplevel=(int)_tmp275,((_tmpB03->rgn=(struct _RegionHandle*)
r,_tmpB03)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp278=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp279=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp278;if(_tmp279->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27A=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp278;if(_tmp27A->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27B=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp278;if(_tmp27B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB07;void*_tmpB06;(_tmpB06=0,Cyc_Tcutil_terr(e->loc,((
_tmpB07="dereference of NULL pointer",_tag_dyneither(_tmpB07,sizeof(char),28))),
_tag_dyneither(_tmpB06,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27C=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp278;if(_tmp27C->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27D=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp278;if(_tmp27D->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB0A;void*_tmpB09;(_tmpB09=0,Cyc_Tcutil_warn(e->loc,((
_tmpB0A="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB0A,
sizeof(char),46))),_tag_dyneither(_tmpB09,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB0D;void*_tmpB0C;(_tmpB0C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0D="need_null_check",_tag_dyneither(
_tmpB0D,sizeof(char),16))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp284=e->annot;
struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp286=_tmp285->f1;}}_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp287=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB10;void*_tmpB0F;(_tmpB0F=0,Cyc_Tcutil_terr(e->loc,((
_tmpB10="dereference of NULL pointer",_tag_dyneither(_tmpB10,sizeof(char),28))),
_tag_dyneither(_tmpB0F,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28A=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp284;if(_tmp28A->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28B=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28B->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB13;void*_tmpB12;(
_tmpB12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB13="get_relns",_tag_dyneither(_tmpB13,sizeof(char),10))),_tag_dyneither(
_tmpB12,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp290=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp292;struct Cyc_Absyn_PtrAtts
_tmp293;union Cyc_Absyn_Constraint*_tmp294;union Cyc_Absyn_Constraint*_tmp295;
struct Cyc_Absyn_ArrayInfo _tmp297;struct Cyc_Absyn_Exp*_tmp298;_LL103: {struct Cyc_Absyn_PointerType_struct*
_tmp291=(struct Cyc_Absyn_PointerType_struct*)_tmp290;if(_tmp291->tag != 5)goto
_LL105;else{_tmp292=_tmp291->f1;_tmp293=_tmp292.ptr_atts;_tmp294=_tmp293.bounds;
_tmp295=_tmp293.zero_term;}}_LL104: {void*_tmp299=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp294);struct Cyc_Absyn_Exp*
_tmp29C;_LL10A: {struct Cyc_Absyn_DynEither_b_struct*_tmp29A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp299;if(_tmp29A->tag != 0)goto _LL10C;}_LL10B: return 0;_LL10C: {struct Cyc_Absyn_Upper_b_struct*
_tmp29B=(struct Cyc_Absyn_Upper_b_struct*)_tmp299;if(_tmp29B->tag != 1)goto _LL109;
else{_tmp29C=_tmp29B->f1;}}_LL10D: {unsigned int _tmp29E;int _tmp29F;struct
_tuple10 _tmp29D=Cyc_Evexp_eval_const_uint_exp(_tmp29C);_tmp29E=_tmp29D.f1;
_tmp29F=_tmp29D.f2;return _tmp29F  && i <= _tmp29E;}_LL109:;}_LL105: {struct Cyc_Absyn_ArrayType_struct*
_tmp296=(struct Cyc_Absyn_ArrayType_struct*)_tmp290;if(_tmp296->tag != 9)goto
_LL107;else{_tmp297=_tmp296->f1;_tmp298=_tmp297.num_elts;}}_LL106: if(_tmp298 == 0)
return 0;{unsigned int _tmp2A1;int _tmp2A2;struct _tuple10 _tmp2A0=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp298);_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;return
_tmp2A2  && i <= _tmp2A1;};_LL107:;_LL108: return 0;_LL102:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A3=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A3->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A4=_tmp2A3->rop;struct Cyc_Absyn_Vardecl*_tmp2A5;struct
Cyc_Absyn_Vardecl*_tmp2A6;_LL10F: if((_tmp2A4.LessNumelts).tag != 3)goto _LL111;
_tmp2A5=(struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessNumelts).val;_LL110: _tmp2A6=
_tmp2A5;goto _LL112;_LL111: if((_tmp2A4.LessEqNumelts).tag != 5)goto _LL113;_tmp2A6=(
struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessEqNumelts).val;_LL112: if(_tmp2A6 == v)
return 1;else{goto _LL10E;}_LL113:;_LL114: continue;_LL10E:;};}return 0;}static int
Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp2A7=e->r;void*_tmp2A9;
struct Cyc_Absyn_Vardecl*_tmp2AB;void*_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AF;
void*_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B3;void*_tmp2B5;struct Cyc_Absyn_Vardecl*
_tmp2B7;enum Cyc_Absyn_Primop _tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;_LL116: {struct Cyc_Absyn_Var_e_struct*
_tmp2A8=(struct Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2A8->tag != 1)goto _LL118;
else{_tmp2A9=(void*)_tmp2A8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2AA=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2A9;if(_tmp2AA->tag != 5)goto _LL118;else{_tmp2AB=_tmp2AA->f1;}};}}_LL117:
_tmp2AF=_tmp2AB;goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AC=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2AC->tag != 1)goto _LL11A;else{_tmp2AD=(
void*)_tmp2AC->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2AE=(struct Cyc_Absyn_Local_b_struct*)
_tmp2AD;if(_tmp2AE->tag != 4)goto _LL11A;else{_tmp2AF=_tmp2AE->f1;}};}}_LL119:
_tmp2B3=_tmp2AF;goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B0=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2B0->tag != 1)goto _LL11C;else{_tmp2B1=(
void*)_tmp2B0->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2B2=(struct Cyc_Absyn_Global_b_struct*)
_tmp2B1;if(_tmp2B2->tag != 1)goto _LL11C;else{_tmp2B3=_tmp2B2->f1;}};}}_LL11B:
_tmp2B7=_tmp2B3;goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2B4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2B4->tag != 1)goto _LL11E;else{_tmp2B5=(
void*)_tmp2B4->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2B6=(struct Cyc_Absyn_Param_b_struct*)
_tmp2B5;if(_tmp2B6->tag != 3)goto _LL11E;else{_tmp2B7=_tmp2B6->f1;}};}}_LL11D: if(
_tmp2B7->escapes)return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B7))return 1;
goto _LL115;_LL11E: {struct Cyc_Absyn_Primop_e_struct*_tmp2B8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A7;if(_tmp2B8->tag != 3)goto _LL120;else{_tmp2B9=_tmp2B8->f1;if(_tmp2B9 != Cyc_Absyn_Numelts)
goto _LL120;_tmp2BA=_tmp2B8->f2;if(_tmp2BA == 0)goto _LL120;_tmp2BB=*_tmp2BA;
_tmp2BC=(struct Cyc_Absyn_Exp*)_tmp2BB.hd;}}_LL11F:{void*_tmp2BD=_tmp2BC->r;void*
_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C1;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C5;void*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2C9;void*_tmp2CB;struct Cyc_Absyn_Vardecl*
_tmp2CD;_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2BE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2BE->tag != 1)goto _LL125;else{_tmp2BF=(void*)_tmp2BE->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2C0=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BF;if(_tmp2C0->tag
!= 5)goto _LL125;else{_tmp2C1=_tmp2C0->f1;}};}}_LL124: _tmp2C5=_tmp2C1;goto _LL126;
_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C2->tag != 1)goto _LL127;else{_tmp2C3=(void*)_tmp2C2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C4=(struct Cyc_Absyn_Local_b_struct*)_tmp2C3;if(
_tmp2C4->tag != 4)goto _LL127;else{_tmp2C5=_tmp2C4->f1;}};}}_LL126: _tmp2C9=_tmp2C5;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C6->tag != 1)goto _LL129;else{_tmp2C7=(void*)_tmp2C6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C8=(struct Cyc_Absyn_Global_b_struct*)_tmp2C7;if(
_tmp2C8->tag != 1)goto _LL129;else{_tmp2C9=_tmp2C8->f1;}};}}_LL128: _tmp2CD=_tmp2C9;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2CA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2CA->tag != 1)goto _LL12B;else{_tmp2CB=(void*)_tmp2CA->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_struct*)_tmp2CB;if(
_tmp2CC->tag != 3)goto _LL12B;else{_tmp2CD=_tmp2CC->f1;}};}}_LL12A: return _tmp2CD == 
v;_LL12B:;_LL12C: goto _LL122;_LL122:;}goto _LL115;_LL120:;_LL121: goto _LL115;_LL115:;}
return 0;}static char _tmp2DF[8]="*bogus*";struct _tuple17{void*f1;void*f2;};static
int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){struct Cyc_Absyn_Vardecl*x;{
void*_tmp2CE=a->r;void*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D4;
struct Cyc_Absyn_Vardecl*_tmp2D6;void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;
void*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DE;_LL12E: {struct Cyc_Absyn_Var_e_struct*
_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2CF->tag != 1)goto _LL130;
else{_tmp2D0=(void*)_tmp2CF->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2D0;if(_tmp2D1->tag != 5)goto _LL130;else{_tmp2D2=_tmp2D1->f1;}};}}_LL12F:
_tmp2D6=_tmp2D2;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2D3->tag != 1)goto _LL132;else{_tmp2D4=(
void*)_tmp2D3->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D4;if(_tmp2D5->tag != 4)goto _LL132;else{_tmp2D6=_tmp2D5->f1;}};}}_LL131:
_tmp2DA=_tmp2D6;goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2D7->tag != 1)goto _LL134;else{_tmp2D8=(
void*)_tmp2D7->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D8;if(_tmp2D9->tag != 1)goto _LL134;else{_tmp2DA=_tmp2D9->f1;}};}}_LL133:
_tmp2DE=_tmp2DA;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CE;if(_tmp2DB->tag != 1)goto _LL136;else{_tmp2DC=(
void*)_tmp2DB->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)
_tmp2DC;if(_tmp2DD->tag != 3)goto _LL136;else{_tmp2DE=_tmp2DD->f1;}};}}_LL135: if(
_tmp2DE->escapes)goto _LL137;x=_tmp2DE;goto _LL12D;_LL136:;_LL137: {static struct
_dyneither_ptr bogus_string={_tmp2DF,_tmp2DF,_tmp2DF + 8};static struct _tuple0
bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL12D:;}inner_loop: {void*_tmp2E0=i->r;
void*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;union Cyc_Absyn_Cnst _tmp2E5;struct
_tuple5 _tmp2E6;enum Cyc_Absyn_Sign _tmp2E7;int _tmp2E8;union Cyc_Absyn_Cnst _tmp2EA;
struct _tuple5 _tmp2EB;enum Cyc_Absyn_Sign _tmp2EC;int _tmp2ED;union Cyc_Absyn_Cnst
_tmp2EF;struct _tuple5 _tmp2F0;enum Cyc_Absyn_Sign _tmp2F1;int _tmp2F2;enum Cyc_Absyn_Primop
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List _tmp2F6;struct Cyc_Absyn_Exp*
_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List _tmp2F9;struct Cyc_Absyn_Exp*
_tmp2FA;void*_tmp2FC;struct Cyc_Absyn_Vardecl*_tmp2FE;void*_tmp300;struct Cyc_Absyn_Vardecl*
_tmp302;void*_tmp304;struct Cyc_Absyn_Vardecl*_tmp306;void*_tmp308;struct Cyc_Absyn_Vardecl*
_tmp30A;_LL139: {struct Cyc_Absyn_Cast_e_struct*_tmp2E1=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2E0;if(_tmp2E1->tag != 15)goto _LL13B;else{_tmp2E2=(void*)_tmp2E1->f1;_tmp2E3=
_tmp2E1->f2;}}_LL13A: i=_tmp2E3;goto inner_loop;_LL13B: {struct Cyc_Absyn_Const_e_struct*
_tmp2E4=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2E4->tag != 0)goto _LL13D;
else{_tmp2E5=_tmp2E4->f1;if((_tmp2E5.Int_c).tag != 4)goto _LL13D;_tmp2E6=(struct
_tuple5)(_tmp2E5.Int_c).val;_tmp2E7=_tmp2E6.f1;if(_tmp2E7 != Cyc_Absyn_None)goto
_LL13D;_tmp2E8=_tmp2E6.f2;}}_LL13C: _tmp2ED=_tmp2E8;goto _LL13E;_LL13D: {struct Cyc_Absyn_Const_e_struct*
_tmp2E9=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2E9->tag != 0)goto _LL13F;
else{_tmp2EA=_tmp2E9->f1;if((_tmp2EA.Int_c).tag != 4)goto _LL13F;_tmp2EB=(struct
_tuple5)(_tmp2EA.Int_c).val;_tmp2EC=_tmp2EB.f1;if(_tmp2EC != Cyc_Absyn_Signed)
goto _LL13F;_tmp2ED=_tmp2EB.f2;}}_LL13E: return _tmp2ED >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2ED + 1),x->type);_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2EE=(struct Cyc_Absyn_Const_e_struct*)_tmp2E0;if(_tmp2EE->tag != 0)goto _LL141;
else{_tmp2EF=_tmp2EE->f1;if((_tmp2EF.Int_c).tag != 4)goto _LL141;_tmp2F0=(struct
_tuple5)(_tmp2EF.Int_c).val;_tmp2F1=_tmp2F0.f1;if(_tmp2F1 != Cyc_Absyn_Unsigned)
goto _LL141;_tmp2F2=_tmp2F0.f2;}}_LL140: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2F2 + 1),x->type);_LL141: {struct Cyc_Absyn_Primop_e_struct*
_tmp2F3=(struct Cyc_Absyn_Primop_e_struct*)_tmp2E0;if(_tmp2F3->tag != 3)goto _LL143;
else{_tmp2F4=_tmp2F3->f1;if(_tmp2F4 != Cyc_Absyn_Mod)goto _LL143;_tmp2F5=_tmp2F3->f2;
if(_tmp2F5 == 0)goto _LL143;_tmp2F6=*_tmp2F5;_tmp2F7=(struct Cyc_Absyn_Exp*)_tmp2F6.hd;
_tmp2F8=_tmp2F6.tl;if(_tmp2F8 == 0)goto _LL143;_tmp2F9=*_tmp2F8;_tmp2FA=(struct Cyc_Absyn_Exp*)
_tmp2F9.hd;}}_LL142: return Cyc_Toc_check_leq_size(relns,x,_tmp2FA);_LL143: {
struct Cyc_Absyn_Var_e_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(
_tmp2FB->tag != 1)goto _LL145;else{_tmp2FC=(void*)_tmp2FB->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2FD=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FC;if(_tmp2FD->tag != 5)goto _LL145;
else{_tmp2FE=_tmp2FD->f1;}};}}_LL144: _tmp302=_tmp2FE;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp2FF=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp2FF->tag
!= 1)goto _LL147;else{_tmp300=(void*)_tmp2FF->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp301=(struct Cyc_Absyn_Local_b_struct*)_tmp300;if(_tmp301->tag != 4)goto _LL147;
else{_tmp302=_tmp301->f1;}};}}_LL146: _tmp306=_tmp302;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp303=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp303->tag
!= 1)goto _LL149;else{_tmp304=(void*)_tmp303->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp305=(struct Cyc_Absyn_Global_b_struct*)_tmp304;if(_tmp305->tag != 1)goto _LL149;
else{_tmp306=_tmp305->f1;}};}}_LL148: _tmp30A=_tmp306;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp307=(struct Cyc_Absyn_Var_e_struct*)_tmp2E0;if(_tmp307->tag
!= 1)goto _LL14B;else{_tmp308=(void*)_tmp307->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp309=(struct Cyc_Absyn_Param_b_struct*)_tmp308;if(_tmp309->tag != 3)goto _LL14B;
else{_tmp30A=_tmp309->f1;}};}}_LL14A: if(_tmp30A->escapes)return 0;{struct Cyc_List_List*
_tmp30B=relns;for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30C=(struct Cyc_CfFlowInfo_Reln*)_tmp30B->hd;if(_tmp30C->vd == _tmp30A){union
Cyc_CfFlowInfo_RelnOp _tmp30D=_tmp30C->rop;struct Cyc_Absyn_Vardecl*_tmp30E;struct
_tuple11 _tmp30F;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp311;unsigned int _tmp312;
_LL14E: if((_tmp30D.LessNumelts).tag != 3)goto _LL150;_tmp30E=(struct Cyc_Absyn_Vardecl*)(
_tmp30D.LessNumelts).val;_LL14F: if(x == _tmp30E)return 1;else{goto _LL14D;}_LL150:
if((_tmp30D.LessVar).tag != 2)goto _LL152;_tmp30F=(struct _tuple11)(_tmp30D.LessVar).val;
_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_LL151:{struct _tuple17 _tmpB14;struct
_tuple17 _tmp314=(_tmpB14.f1=Cyc_Tcutil_compress(_tmp311),((_tmpB14.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v),_tmpB14)));void*_tmp315;
void*_tmp317;void*_tmp318;struct Cyc_Absyn_PtrInfo _tmp31A;struct Cyc_Absyn_PtrAtts
_tmp31B;union Cyc_Absyn_Constraint*_tmp31C;_LL157: _tmp315=_tmp314.f1;{struct Cyc_Absyn_TagType_struct*
_tmp316=(struct Cyc_Absyn_TagType_struct*)_tmp315;if(_tmp316->tag != 20)goto _LL159;
else{_tmp317=(void*)_tmp316->f1;}};_tmp318=_tmp314.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp319=(struct Cyc_Absyn_PointerType_struct*)_tmp318;if(_tmp319->tag != 5)goto
_LL159;else{_tmp31A=_tmp319->f1;_tmp31B=_tmp31A.ptr_atts;_tmp31C=_tmp31B.bounds;}};
_LL158:{void*_tmp31D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp31C);struct Cyc_Absyn_Exp*_tmp31F;_LL15C: {struct Cyc_Absyn_Upper_b_struct*
_tmp31E=(struct Cyc_Absyn_Upper_b_struct*)_tmp31D;if(_tmp31E->tag != 1)goto _LL15E;
else{_tmp31F=_tmp31E->f1;}}_LL15D: {struct Cyc_Absyn_Exp*_tmp320=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp317,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp320,_tmp31F))return 1;goto _LL15B;}_LL15E:;_LL15F:
goto _LL15B;_LL15B:;}goto _LL156;_LL159:;_LL15A: goto _LL156;_LL156:;}{struct Cyc_List_List*
_tmp321=relns;for(0;_tmp321 != 0;_tmp321=_tmp321->tl){struct Cyc_CfFlowInfo_Reln*
_tmp322=(struct Cyc_CfFlowInfo_Reln*)_tmp321->hd;if(_tmp322->vd == _tmp310){union
Cyc_CfFlowInfo_RelnOp _tmp323=_tmp322->rop;struct Cyc_Absyn_Vardecl*_tmp324;struct
Cyc_Absyn_Vardecl*_tmp325;unsigned int _tmp326;struct _tuple11 _tmp327;struct Cyc_Absyn_Vardecl*
_tmp328;_LL161: if((_tmp323.LessEqNumelts).tag != 5)goto _LL163;_tmp324=(struct Cyc_Absyn_Vardecl*)(
_tmp323.LessEqNumelts).val;_LL162: _tmp325=_tmp324;goto _LL164;_LL163: if((_tmp323.LessNumelts).tag
!= 3)goto _LL165;_tmp325=(struct Cyc_Absyn_Vardecl*)(_tmp323.LessNumelts).val;
_LL164: if(x == _tmp325)return 1;goto _LL160;_LL165: if((_tmp323.EqualConst).tag != 1)
goto _LL167;_tmp326=(unsigned int)(_tmp323.EqualConst).val;_LL166: return Cyc_Toc_check_const_array(
_tmp326,x->type);_LL167: if((_tmp323.LessVar).tag != 2)goto _LL169;_tmp327=(struct
_tuple11)(_tmp323.LessVar).val;_tmp328=_tmp327.f1;_LL168: if(Cyc_Toc_check_leq_size_var(
relns,x,_tmp328))return 1;goto _LL160;_LL169:;_LL16A: goto _LL160;_LL160:;}}}goto
_LL14D;_LL152: if((_tmp30D.LessConst).tag != 4)goto _LL154;_tmp312=(unsigned int)(
_tmp30D.LessConst).val;_LL153: return Cyc_Toc_check_const_array(_tmp312,x->type);
_LL154:;_LL155: goto _LL14D;_LL14D:;}}}goto _LL138;_LL14B:;_LL14C: goto _LL138;_LL138:;}
return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB17;void*_tmpB16;(_tmpB16=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB17="Missing type in primop ",_tag_dyneither(_tmpB17,sizeof(char),24))),
_tag_dyneither(_tmpB16,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1A="Missing type in primop ",
_tag_dyneither(_tmpB1A,sizeof(char),24))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple9*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e);static struct _tuple9*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){struct _tuple9*_tmpB1B;return(_tmpB1B=_cycalloc(sizeof(*
_tmpB1B)),((_tmpB1B->f1=Cyc_Toc_mt_tq,((_tmpB1B->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB1B)))));}static struct _tuple15*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static struct
_tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple15*_tmpB1C;return(_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((
_tmpB1C->f1=0,((_tmpB1C->f2=e,_tmpB1C)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp32F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp32F): Cyc_Toc_malloc_ptr(_tmp32F));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp32F);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp330=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp330 != 0;_tmp330=_tmp330->tl){struct _tuple15 _tmp332;struct Cyc_List_List*
_tmp333;struct Cyc_Absyn_Exp*_tmp334;struct _tuple15*_tmp331=(struct _tuple15*)
_tmp330->hd;_tmp332=*_tmp331;_tmp333=_tmp332.f1;_tmp334=_tmp332.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp333 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp333->tl != 0){const char*_tmpB1F;void*_tmpB1E;(_tmpB1E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB1F="multiple designators in array",
_tag_dyneither(_tmpB1F,sizeof(char),30))),_tag_dyneither(_tmpB1E,sizeof(void*),0)));}{
void*_tmp337=(void*)_tmp333->hd;void*_tmp338=_tmp337;struct Cyc_Absyn_Exp*_tmp33A;
_LL16C: {struct Cyc_Absyn_ArrayElement_struct*_tmp339=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp338;if(_tmp339->tag != 0)goto _LL16E;else{_tmp33A=_tmp339->f1;}}_LL16D: Cyc_Toc_exp_to_c(
nv,_tmp33A);e_index=_tmp33A;goto _LL16B;_LL16E: {struct Cyc_Absyn_FieldName_struct*
_tmp33B=(struct Cyc_Absyn_FieldName_struct*)_tmp338;if(_tmp33B->tag != 1)goto
_LL16B;}_LL16F: {const char*_tmpB22;void*_tmpB21;(_tmpB21=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB22="field name designators in array",
_tag_dyneither(_tmpB22,sizeof(char),32))),_tag_dyneither(_tmpB21,sizeof(void*),0)));}
_LL16B:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp33E=_tmp334->r;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Vardecl*_tmp342;
struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp344;int _tmp345;void*_tmp347;
struct Cyc_List_List*_tmp348;_LL171: {struct Cyc_Absyn_Array_e_struct*_tmp33F=(
struct Cyc_Absyn_Array_e_struct*)_tmp33E;if(_tmp33F->tag != 27)goto _LL173;else{
_tmp340=_tmp33F->f1;}}_LL172: s=Cyc_Toc_init_array(nv,lval,_tmp340,s);goto _LL170;
_LL173: {struct Cyc_Absyn_Comprehension_e_struct*_tmp341=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp33E;if(_tmp341->tag != 28)goto _LL175;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;
_tmp344=_tmp341->f3;_tmp345=_tmp341->f4;}}_LL174: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp342,_tmp343,_tmp344,_tmp345,s,0);goto _LL170;_LL175: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp346=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp33E;if(_tmp346->tag != 30)goto
_LL177;else{_tmp347=(void*)_tmp346->f1;_tmp348=_tmp346->f2;}}_LL176: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp347,_tmp348,s);goto _LL170;_LL177:;_LL178: Cyc_Toc_exp_to_c(nv,_tmp334);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp334,0),s,0);goto _LL170;_LL170:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp349=vd->name;void*_tmp34A=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34C;struct _RegionHandle*_tmp34D;
struct Cyc_Toc_Env*_tmp34B=nv;_tmp34C=*_tmp34B;_tmp34D=_tmp34C.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB25;struct Cyc_Absyn_Local_b_struct*_tmpB24;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp34D,nv,_tmp349,Cyc_Absyn_varb_exp(_tmp349,(void*)((_tmpB24=_cycalloc(sizeof(*
_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=4,((_tmpB25.f1=vd,_tmpB25)))),_tmpB24)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp349,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp349,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp349,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp349,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp34E=e2->r;struct Cyc_List_List*_tmp350;
struct Cyc_Absyn_Vardecl*_tmp352;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*
_tmp354;int _tmp355;void*_tmp357;struct Cyc_List_List*_tmp358;_LL17A: {struct Cyc_Absyn_Array_e_struct*
_tmp34F=(struct Cyc_Absyn_Array_e_struct*)_tmp34E;if(_tmp34F->tag != 27)goto _LL17C;
else{_tmp350=_tmp34F->f1;}}_LL17B: body=Cyc_Toc_init_array(nv2,lval,_tmp350,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL17C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp351=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34E;if(_tmp351->tag != 28)goto _LL17E;else{_tmp352=_tmp351->f1;_tmp353=_tmp351->f2;
_tmp354=_tmp351->f3;_tmp355=_tmp351->f4;}}_LL17D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp352,_tmp353,_tmp354,_tmp355,Cyc_Toc_skip_stmt_dl(),0);goto _LL179;
_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp356=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34E;if(_tmp356->tag != 30)goto _LL180;else{_tmp357=(void*)_tmp356->f1;_tmp358=
_tmp356->f2;}}_LL17F: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp357,_tmp358,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL180:;_LL181: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL179;_LL179:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp34A,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp349,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp35B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp35B != 0;_tmp35B=_tmp35B->tl){struct _tuple15 _tmp35D;
struct Cyc_List_List*_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct _tuple15*_tmp35C=(
struct _tuple15*)_tmp35B->hd;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;
if(_tmp35E == 0){const char*_tmpB28;void*_tmpB27;(_tmpB27=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB28="empty designator list",
_tag_dyneither(_tmpB28,sizeof(char),22))),_tag_dyneither(_tmpB27,sizeof(void*),0)));}
if(_tmp35E->tl != 0){const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="too many designators in anonymous struct",
_tag_dyneither(_tmpB2B,sizeof(char),41))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}{
void*_tmp364=(void*)_tmp35E->hd;struct _dyneither_ptr*_tmp366;_LL183: {struct Cyc_Absyn_FieldName_struct*
_tmp365=(struct Cyc_Absyn_FieldName_struct*)_tmp364;if(_tmp365->tag != 1)goto
_LL185;else{_tmp366=_tmp365->f1;}}_LL184: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp366,0);{void*_tmp367=_tmp35F->r;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Vardecl*
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;int _tmp36E;void*
_tmp370;struct Cyc_List_List*_tmp371;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp368=(struct Cyc_Absyn_Array_e_struct*)_tmp367;if(_tmp368->tag != 27)goto _LL18A;
else{_tmp369=_tmp368->f1;}}_LL189: s=Cyc_Toc_init_array(nv,lval,_tmp369,s);goto
_LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp36A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp367;if(_tmp36A->tag != 28)goto _LL18C;else{_tmp36B=_tmp36A->f1;_tmp36C=_tmp36A->f2;
_tmp36D=_tmp36A->f3;_tmp36E=_tmp36A->f4;}}_LL18B: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp36B,_tmp36C,_tmp36D,_tmp36E,s,0);goto _LL187;_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp36F=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp367;if(_tmp36F->tag != 30)goto
_LL18E;else{_tmp370=(void*)_tmp36F->f1;_tmp371=_tmp36F->f2;}}_LL18D: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp370,_tmp371,s);goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv,_tmp35F);
if(Cyc_Toc_is_poly_field(struct_type,_tmp366))_tmp35F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35F);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp35F,0),0),s,0);goto _LL187;_LL187:;}goto _LL182;}_LL185:;_LL186: {const char*
_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="array designator in struct",
_tag_dyneither(_tmpB2E,sizeof(char),27))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}
_LL182:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp375;struct _RegionHandle*_tmp376;
struct Cyc_Toc_Env*_tmp374=nv;_tmp375=*_tmp374;_tmp376=_tmp375.rgn;{struct Cyc_List_List*
_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple9*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp376,Cyc_Toc_tup_to_c,
es);void*_tmp378=Cyc_Toc_add_tuple_type(_tmp377);struct _tuple0*_tmp379=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37A=Cyc_Absyn_var_exp(_tmp379,0);struct Cyc_Absyn_Stmt*
_tmp37B=Cyc_Absyn_exp_stmt(_tmp37A,0);struct Cyc_Absyn_Exp*(*_tmp37C)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp376,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37D);for(0;_tmp37D != 0;(
_tmp37D=_tmp37D->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37D->hd;
struct Cyc_Absyn_Exp*lval=_tmp37C(_tmp37A,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp37E=e->r;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Vardecl*_tmp382;
struct Cyc_Absyn_Exp*_tmp383;struct Cyc_Absyn_Exp*_tmp384;int _tmp385;_LL191: {
struct Cyc_Absyn_Array_e_struct*_tmp37F=(struct Cyc_Absyn_Array_e_struct*)_tmp37E;
if(_tmp37F->tag != 27)goto _LL193;else{_tmp380=_tmp37F->f1;}}_LL192: _tmp37B=Cyc_Toc_init_array(
nv,lval,_tmp380,_tmp37B);goto _LL190;_LL193: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp381=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp37E;if(_tmp381->tag != 28)
goto _LL195;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;_tmp384=_tmp381->f3;
_tmp385=_tmp381->f4;}}_LL194: _tmp37B=Cyc_Toc_init_comprehension(nv,lval,_tmp382,
_tmp383,_tmp384,_tmp385,_tmp37B,0);goto _LL190;_LL195:;_LL196: Cyc_Toc_exp_to_c(nv,
e);_tmp37B=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37C(
_tmp37A,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp37B,0);goto _LL190;_LL190:;};}}
return Cyc_Toc_make_struct(nv,_tmp379,_tmp378,_tmp37B,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp386=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp386 != 0;_tmp386=_tmp386->tl){struct Cyc_Absyn_Aggrfield*
_tmp387=(struct Cyc_Absyn_Aggrfield*)_tmp386->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp387->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB36;void*_tmpB35[1];const char*_tmpB34;void*_tmpB33;(
_tmpB33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB36.tag=0,((_tmpB36.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB35[0]=& _tmpB36,Cyc_aprintf(((_tmpB34="get_member_offset %s failed",
_tag_dyneither(_tmpB34,sizeof(char),28))),_tag_dyneither(_tmpB35,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB33,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp38C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38D=Cyc_Absyn_var_exp(_tmp38C,0);struct Cyc_Absyn_Stmt*
_tmp38E=Cyc_Absyn_exp_stmt(_tmp38D,0);struct Cyc_Absyn_Exp*(*_tmp38F)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp390=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp391=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp393;union Cyc_Absyn_AggrInfoU _tmp394;
_LL198: {struct Cyc_Absyn_AggrType_struct*_tmp392=(struct Cyc_Absyn_AggrType_struct*)
_tmp391;if(_tmp392->tag != 12)goto _LL19A;else{_tmp393=_tmp392->f1;_tmp394=_tmp393.aggr_info;}}
_LL199: ad=Cyc_Absyn_get_known_aggrdecl(_tmp394);goto _LL197;_LL19A:;_LL19B: {
const char*_tmpB39;void*_tmpB38;(_tmpB38=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB39="init_struct: bad struct type",
_tag_dyneither(_tmpB39,sizeof(char),29))),_tag_dyneither(_tmpB38,sizeof(void*),0)));}
_LL197:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp398;struct _RegionHandle*_tmp399;struct Cyc_Toc_Env*_tmp397=
nv;_tmp398=*_tmp397;_tmp399=_tmp398.rgn;{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp399,dles);for(0;
_tmp39A != 0;_tmp39A=_tmp39A->tl){struct _tuple15 _tmp39C;struct Cyc_List_List*
_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple15*_tmp39B=(struct _tuple15*)
_tmp39A->hd;_tmp39C=*_tmp39B;_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39E->topt))->v);
if(_tmp39D == 0){const char*_tmpB3C;void*_tmpB3B;(_tmpB3B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3C="empty designator list",
_tag_dyneither(_tmpB3C,sizeof(char),22))),_tag_dyneither(_tmpB3B,sizeof(void*),0)));}
if(_tmp39D->tl != 0){struct _tuple0*_tmp3A1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A2=Cyc_Absyn_var_exp(_tmp3A1,0);for(0;_tmp39D != 0;_tmp39D=_tmp39D->tl){void*
_tmp3A3=(void*)_tmp39D->hd;struct _dyneither_ptr*_tmp3A5;_LL19D: {struct Cyc_Absyn_FieldName_struct*
_tmp3A4=(struct Cyc_Absyn_FieldName_struct*)_tmp3A3;if(_tmp3A4->tag != 1)goto
_LL19F;else{_tmp3A5=_tmp3A4->f1;}}_LL19E: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A5))_tmp3A2=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A2);_tmp38E=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38F(_tmp38D,_tmp3A5,0),_tmp3A2,0),0),
_tmp38E,0);goto _LL19C;_LL19F:;_LL1A0: {const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB3F="array designator in struct",_tag_dyneither(_tmpB3F,sizeof(char),27))),
_tag_dyneither(_tmpB3E,sizeof(void*),0)));}_LL19C:;}Cyc_Toc_exp_to_c(nv,_tmp39E);
_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A2,
_tmp39E,0),0),_tmp38E,0);}else{void*_tmp3A8=(void*)_tmp39D->hd;struct
_dyneither_ptr*_tmp3AA;_LL1A2: {struct Cyc_Absyn_FieldName_struct*_tmp3A9=(struct
Cyc_Absyn_FieldName_struct*)_tmp3A8;if(_tmp3A9->tag != 1)goto _LL1A4;else{_tmp3AA=
_tmp3A9->f1;}}_LL1A3: {struct Cyc_Absyn_Exp*lval=_tmp38F(_tmp38D,_tmp3AA,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3AA);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp38E,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3AB=_tmp39E->r;
struct Cyc_List_List*_tmp3AD;struct Cyc_Absyn_Vardecl*_tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;int _tmp3B2;void*_tmp3B4;struct Cyc_List_List*
_tmp3B5;_LL1A7: {struct Cyc_Absyn_Array_e_struct*_tmp3AC=(struct Cyc_Absyn_Array_e_struct*)
_tmp3AB;if(_tmp3AC->tag != 27)goto _LL1A9;else{_tmp3AD=_tmp3AC->f1;}}_LL1A8:
_tmp38E=Cyc_Toc_init_array(nv,lval,_tmp3AD,_tmp38E);goto _LL1A6;_LL1A9: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3AE=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3AB;if(_tmp3AE->tag != 28)goto _LL1AB;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;
_tmp3B1=_tmp3AE->f3;_tmp3B2=_tmp3AE->f4;}}_LL1AA: _tmp38E=Cyc_Toc_init_comprehension(
nv,lval,_tmp3AF,_tmp3B0,_tmp3B1,_tmp3B2,_tmp38E,0);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B3=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3AB;if(_tmp3B3->tag != 30)goto
_LL1AD;else{_tmp3B4=(void*)_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;}}_LL1AC: _tmp38E=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B4,_tmp3B5,_tmp38E);goto _LL1A6;_LL1AD:;_LL1AE: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp39E->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp39E);{struct Cyc_Absyn_Aggrfield*_tmp3B6=Cyc_Absyn_lookup_decl_field(ad,
_tmp3AA);if(Cyc_Toc_is_poly_field(struct_type,_tmp3AA) && !was_ptr_type)_tmp39E=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39E);if(has_exists)_tmp39E=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B6))->type),
_tmp39E);_tmp38E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp39E,0),0),_tmp38E,0);goto _LL1A6;};}_LL1A6:;}goto _LL1A1;}_LL1A4:;_LL1A5: {
const char*_tmpB42;void*_tmpB41;(_tmpB41=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB42="array designator in struct",
_tag_dyneither(_tmpB42,sizeof(char),27))),_tag_dyneither(_tmpB41,sizeof(void*),0)));}
_LL1A1:;}}}return Cyc_Toc_make_struct(nv,_tmp38C,_tmp390,_tmp38E,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB45;struct Cyc_Absyn_Increment_e_struct*
_tmpB44;return Cyc_Absyn_new_exp((void*)((_tmpB44=_cycalloc(sizeof(*_tmpB44)),((
_tmpB44[0]=((_tmpB45.tag=5,((_tmpB45.f1=e1,((_tmpB45.f2=incr,_tmpB45)))))),
_tmpB44)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3BB=e1->r;struct Cyc_Absyn_Stmt*
_tmp3BD;void*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C2;
struct _dyneither_ptr*_tmp3C3;int _tmp3C4;int _tmp3C5;_LL1B0: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3BC=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3BB;if(_tmp3BC->tag != 37)goto
_LL1B2;else{_tmp3BD=_tmp3BC->f1;}}_LL1B1: Cyc_Toc_lvalue_assign_stmt(_tmp3BD,fs,f,
f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp3BE=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3BB;if(_tmp3BE->tag != 15)goto _LL1B4;else{_tmp3BF=(void*)_tmp3BE->f1;_tmp3C0=
_tmp3BE->f2;}}_LL1B3: Cyc_Toc_lvalue_assign(_tmp3C0,fs,f,f_env);goto _LL1AF;_LL1B4: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3C1=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3BB;if(_tmp3C1->tag != 22)goto _LL1B6;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;
_tmp3C4=_tmp3C1->f3;_tmp3C5=_tmp3C1->f4;}}_LL1B5: e1->r=_tmp3C2->r;{struct Cyc_List_List*
_tmpB46;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB46=_cycalloc(
sizeof(*_tmpB46)),((_tmpB46->hd=_tmp3C3,((_tmpB46->tl=fs,_tmpB46)))))),f,f_env);}
goto _LL1AF;_LL1B6:;_LL1B7: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1AF;}
_LL1AF:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3C7=s->r;
struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Decl*_tmp3CB;struct Cyc_Absyn_Stmt*
_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CE;_LL1B9: {struct Cyc_Absyn_Exp_s_struct*
_tmp3C8=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C7;if(_tmp3C8->tag != 1)goto _LL1BB;
else{_tmp3C9=_tmp3C8->f1;}}_LL1BA: Cyc_Toc_lvalue_assign(_tmp3C9,fs,f,f_env);goto
_LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_struct*_tmp3CA=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3C7;if(_tmp3CA->tag != 12)goto _LL1BD;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}
_LL1BC: Cyc_Toc_lvalue_assign_stmt(_tmp3CC,fs,f,f_env);goto _LL1B8;_LL1BD: {struct
Cyc_Absyn_Seq_s_struct*_tmp3CD=(struct Cyc_Absyn_Seq_s_struct*)_tmp3C7;if(_tmp3CD->tag
!= 2)goto _LL1BF;else{_tmp3CE=_tmp3CD->f2;}}_LL1BE: Cyc_Toc_lvalue_assign_stmt(
_tmp3CE,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {const char*_tmpB4A;void*_tmpB49[1];
struct Cyc_String_pa_struct _tmpB48;(_tmpB48.tag=0,((_tmpB48.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB49[0]=&
_tmpB48,Cyc_Toc_toc_impos(((_tmpB4A="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB4A,sizeof(char),23))),_tag_dyneither(_tmpB49,sizeof(void*),1)))))));}_LL1B8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3D2=
e->r;void*_tmp3D4;void**_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp**
_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D9;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D3=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D2;if(_tmp3D3->tag != 15)goto _LL1C4;
else{_tmp3D4=(void**)& _tmp3D3->f1;_tmp3D5=(void**)((void**)& _tmp3D3->f1);_tmp3D6=
_tmp3D3->f2;_tmp3D7=(struct Cyc_Absyn_Exp**)& _tmp3D3->f2;}}_LL1C3:*_tmp3D7=Cyc_Toc_push_address_exp(*
_tmp3D7);*_tmp3D5=Cyc_Absyn_cstar_typ(*_tmp3D5,Cyc_Toc_mt_tq);return e;_LL1C4: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D8=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D2;
if(_tmp3D8->tag != 21)goto _LL1C6;else{_tmp3D9=_tmp3D8->f1;}}_LL1C5: return _tmp3D9;
_LL1C6:;_LL1C7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB4E;void*_tmpB4D[1];struct Cyc_String_pa_struct _tmpB4C;(_tmpB4C.tag=0,((
_tmpB4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB4D[0]=& _tmpB4C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB4E="can't take & of %s",_tag_dyneither(_tmpB4E,sizeof(char),19))),
_tag_dyneither(_tmpB4D,sizeof(void*),1)))))));};_LL1C1:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB4F;result=((
_tmpB4F=_region_malloc(r2,sizeof(*_tmpB4F)),((_tmpB4F->hd=(void*)f((void*)x->hd,
env),((_tmpB4F->tl=0,_tmpB4F))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB50;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB50=_region_malloc(r2,sizeof(*_tmpB50)),((_tmpB50->hd=(void*)f((void*)x->hd,
env),((_tmpB50->tl=0,_tmpB50))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*_tmpB51;
return(_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->f1=0,((_tmpB51->f2=e,
_tmpB51)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3E0=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E2;_LL1C9: {struct Cyc_Absyn_PointerType_struct*
_tmp3E1=(struct Cyc_Absyn_PointerType_struct*)_tmp3E0;if(_tmp3E1->tag != 5)goto
_LL1CB;else{_tmp3E2=_tmp3E1->f1;}}_LL1CA: return _tmp3E2;_LL1CB:;_LL1CC: {const
char*_tmpB54;void*_tmpB53;(_tmpB53=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB54="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB54,sizeof(char),28))),_tag_dyneither(_tmpB53,sizeof(void*),0)));}
_LL1C8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3E5=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3E8;enum Cyc_Absyn_Size_of _tmp3E9;enum Cyc_Absyn_Sign
_tmp3EB;enum Cyc_Absyn_Size_of _tmp3EC;enum Cyc_Absyn_Sign _tmp3F0;enum Cyc_Absyn_Size_of
_tmp3F1;enum Cyc_Absyn_Sign _tmp3F3;enum Cyc_Absyn_Size_of _tmp3F4;enum Cyc_Absyn_Sign
_tmp3F6;enum Cyc_Absyn_Size_of _tmp3F7;_LL1CE: {struct Cyc_Absyn_PointerType_struct*
_tmp3E6=(struct Cyc_Absyn_PointerType_struct*)_tmp3E5;if(_tmp3E6->tag != 5)goto
_LL1D0;}_LL1CF: res=Cyc_Absyn_null_exp(0);goto _LL1CD;_LL1D0: {struct Cyc_Absyn_IntType_struct*
_tmp3E7=(struct Cyc_Absyn_IntType_struct*)_tmp3E5;if(_tmp3E7->tag != 6)goto _LL1D2;
else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;if(_tmp3E9 != Cyc_Absyn_Char_sz)goto
_LL1D2;}}_LL1D1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3E8,'\000'),0);goto
_LL1CD;_LL1D2: {struct Cyc_Absyn_IntType_struct*_tmp3EA=(struct Cyc_Absyn_IntType_struct*)
_tmp3E5;if(_tmp3EA->tag != 6)goto _LL1D4;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=_tmp3EA->f2;
if(_tmp3EC != Cyc_Absyn_Short_sz)goto _LL1D4;}}_LL1D3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3EB,0),0);goto _LL1CD;_LL1D4: {struct Cyc_Absyn_EnumType_struct*_tmp3ED=(
struct Cyc_Absyn_EnumType_struct*)_tmp3E5;if(_tmp3ED->tag != 14)goto _LL1D6;}_LL1D5:
goto _LL1D7;_LL1D6: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3EE=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3E5;if(_tmp3EE->tag != 15)goto _LL1D8;}_LL1D7: _tmp3F0=Cyc_Absyn_Unsigned;goto
_LL1D9;_LL1D8: {struct Cyc_Absyn_IntType_struct*_tmp3EF=(struct Cyc_Absyn_IntType_struct*)
_tmp3E5;if(_tmp3EF->tag != 6)goto _LL1DA;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;
if(_tmp3F1 != Cyc_Absyn_Int_sz)goto _LL1DA;}}_LL1D9: _tmp3F3=_tmp3F0;goto _LL1DB;
_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3F2=(struct Cyc_Absyn_IntType_struct*)
_tmp3E5;if(_tmp3F2->tag != 6)goto _LL1DC;else{_tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;
if(_tmp3F4 != Cyc_Absyn_Long_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp3F3,0),0);goto _LL1CD;_LL1DC: {struct Cyc_Absyn_IntType_struct*_tmp3F5=(struct
Cyc_Absyn_IntType_struct*)_tmp3E5;if(_tmp3F5->tag != 6)goto _LL1DE;else{_tmp3F6=
_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;if(_tmp3F7 != Cyc_Absyn_LongLong_sz)goto _LL1DE;}}
_LL1DD: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3F6,(long long)0),0);
goto _LL1CD;_LL1DE: {struct Cyc_Absyn_FloatType_struct*_tmp3F8=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E5;if(_tmp3F8->tag != 7)goto _LL1E0;}_LL1DF: goto _LL1E1;_LL1E0: {struct Cyc_Absyn_DoubleType_struct*
_tmp3F9=(struct Cyc_Absyn_DoubleType_struct*)_tmp3E5;if(_tmp3F9->tag != 8)goto
_LL1E2;}_LL1E1:{const char*_tmpB55;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB55="0.0",_tag_dyneither(_tmpB55,sizeof(char),4)))),0);}goto _LL1CD;_LL1E2:;
_LL1E3: {const char*_tmpB59;void*_tmpB58[1];struct Cyc_String_pa_struct _tmpB57;(
_tmpB57.tag=0,((_tmpB57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB58[0]=& _tmpB57,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB59="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB59,sizeof(char),40))),_tag_dyneither(_tmpB58,sizeof(void*),1)))))));}
_LL1CD:;}{struct Cyc_Core_Opt*_tmpB5A;res->topt=((_tmpB5A=_cycalloc(sizeof(*
_tmpB5A)),((_tmpB5A->v=t,_tmpB5A))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp3FF=Cyc_Toc_typ_to_c(elt_type);void*_tmp400=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp401=Cyc_Absyn_cstar_typ(_tmp3FF,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB5B;struct Cyc_Core_Opt*_tmp402=(_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((
_tmpB5B->v=_tmp401,_tmpB5B)));struct Cyc_Absyn_Exp*xinit;{void*_tmp403=e1->r;
struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*
_tmp408;_LL1E5: {struct Cyc_Absyn_Deref_e_struct*_tmp404=(struct Cyc_Absyn_Deref_e_struct*)
_tmp403;if(_tmp404->tag != 21)goto _LL1E7;else{_tmp405=_tmp404->f1;}}_LL1E6: if(!
is_dyneither){_tmp405=Cyc_Toc_cast_it(fat_ptr_type,_tmp405);{struct Cyc_Core_Opt*
_tmpB5C;_tmp405->topt=((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->v=
fat_ptr_type,_tmpB5C))));};}Cyc_Toc_exp_to_c(nv,_tmp405);xinit=_tmp405;goto
_LL1E4;_LL1E7: {struct Cyc_Absyn_Subscript_e_struct*_tmp406=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp403;if(_tmp406->tag != 24)goto _LL1E9;else{_tmp407=_tmp406->f1;_tmp408=_tmp406->f2;}}
_LL1E8: if(!is_dyneither){_tmp407=Cyc_Toc_cast_it(fat_ptr_type,_tmp407);{struct
Cyc_Core_Opt*_tmpB5D;_tmp407->topt=((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((
_tmpB5D->v=fat_ptr_type,_tmpB5D))));};}Cyc_Toc_exp_to_c(nv,_tmp407);Cyc_Toc_exp_to_c(
nv,_tmp408);{struct Cyc_Absyn_Exp*_tmpB5E[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB5E[2]=_tmp408,((_tmpB5E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB5E[0]=_tmp407,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E4;_LL1E9:;_LL1EA: {const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB61,sizeof(char),53))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}
_LL1E4:;}{struct _tuple0*_tmp40E=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp410;
struct _RegionHandle*_tmp411;struct Cyc_Toc_Env*_tmp40F=nv;_tmp410=*_tmp40F;
_tmp411=_tmp410.rgn;{struct Cyc_Toc_Env*_tmp412=Cyc_Toc_add_varmap(_tmp411,nv,
_tmp40E,Cyc_Absyn_var_exp(_tmp40E,0));struct Cyc_Absyn_Vardecl*_tmpB62;struct Cyc_Absyn_Vardecl*
_tmp413=(_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->sc=Cyc_Absyn_Public,((
_tmpB62->name=_tmp40E,((_tmpB62->tq=Cyc_Toc_mt_tq,((_tmpB62->type=_tmp400,((
_tmpB62->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB62->rgn=0,((_tmpB62->attributes=
0,((_tmpB62->escapes=0,_tmpB62)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB65;struct Cyc_Absyn_Local_b_struct*_tmpB64;struct Cyc_Absyn_Local_b_struct*
_tmp414=(_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB65.tag=4,((
_tmpB65.f1=_tmp413,_tmpB65)))),_tmpB64)));struct Cyc_Absyn_Exp*_tmp415=Cyc_Absyn_varb_exp(
_tmp40E,(void*)_tmp414,0);{struct Cyc_Core_Opt*_tmpB66;_tmp415->topt=((_tmpB66=
_cycalloc(sizeof(*_tmpB66)),((_tmpB66->v=fat_ptr_type,_tmpB66))));}{struct Cyc_Absyn_Exp*
_tmp417=Cyc_Absyn_deref_exp(_tmp415,0);{struct Cyc_Core_Opt*_tmpB67;_tmp417->topt=((
_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->v=elt_type,_tmpB67))));}Cyc_Toc_exp_to_c(
_tmp412,_tmp417);{struct _tuple0*_tmp419=Cyc_Toc_temp_var();_tmp412=Cyc_Toc_add_varmap(
_tmp411,_tmp412,_tmp419,Cyc_Absyn_var_exp(_tmp419,0));{struct Cyc_Absyn_Vardecl*
_tmpB68;struct Cyc_Absyn_Vardecl*_tmp41A=(_tmpB68=_cycalloc(sizeof(*_tmpB68)),((
_tmpB68->sc=Cyc_Absyn_Public,((_tmpB68->name=_tmp419,((_tmpB68->tq=Cyc_Toc_mt_tq,((
_tmpB68->type=_tmp3FF,((_tmpB68->initializer=(struct Cyc_Absyn_Exp*)_tmp417,((
_tmpB68->rgn=0,((_tmpB68->attributes=0,((_tmpB68->escapes=0,_tmpB68)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB6B;struct Cyc_Absyn_Local_b_struct*_tmpB6A;
struct Cyc_Absyn_Local_b_struct*_tmp41B=(_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((
_tmpB6A[0]=((_tmpB6B.tag=4,((_tmpB6B.f1=_tmp41A,_tmpB6B)))),_tmpB6A)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_varb_exp(_tmp419,(
void*)_tmp41B,0);_tmp41C->topt=_tmp417->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp41C,e2,0);z_init->topt=_tmp41C->topt;}Cyc_Toc_exp_to_c(
_tmp412,z_init);{struct _tuple0*_tmp41D=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB6C;struct Cyc_Absyn_Vardecl*_tmp41E=(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((
_tmpB6C->sc=Cyc_Absyn_Public,((_tmpB6C->name=_tmp41D,((_tmpB6C->tq=Cyc_Toc_mt_tq,((
_tmpB6C->type=_tmp3FF,((_tmpB6C->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB6C->rgn=0,((_tmpB6C->attributes=0,((_tmpB6C->escapes=0,_tmpB6C)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB6F;struct Cyc_Absyn_Local_b_struct*_tmpB6E;
struct Cyc_Absyn_Local_b_struct*_tmp41F=(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((
_tmpB6E[0]=((_tmpB6F.tag=4,((_tmpB6F.f1=_tmp41E,_tmpB6F)))),_tmpB6E)));_tmp412=
Cyc_Toc_add_varmap(_tmp411,_tmp412,_tmp41D,Cyc_Absyn_var_exp(_tmp41D,0));{struct
Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41B,0);_tmp420->topt=
_tmp417->topt;{struct Cyc_Absyn_Exp*_tmp421=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp420,_tmp421,0);{
struct Cyc_Core_Opt*_tmpB70;_tmp422->topt=((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((
_tmpB70->v=Cyc_Absyn_sint_typ,_tmpB70))));}Cyc_Toc_exp_to_c(_tmp412,_tmp422);{
struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_varb_exp(_tmp41D,(void*)_tmp41F,0);_tmp424->topt=
_tmp417->topt;{struct Cyc_Absyn_Exp*_tmp425=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp424,_tmp425,0);{
struct Cyc_Core_Opt*_tmpB71;_tmp426->topt=((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((
_tmpB71->v=Cyc_Absyn_sint_typ,_tmpB71))));}Cyc_Toc_exp_to_c(_tmp412,_tmp426);{
struct Cyc_Absyn_Exp*_tmpB72[2];struct Cyc_List_List*_tmp428=(_tmpB72[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB72[0]=Cyc_Absyn_varb_exp(_tmp40E,(void*)
_tmp414,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB72,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp429=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp428,0),
_tmp429,0);{struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp422,_tmp426,0),0);struct Cyc_Absyn_Stmt*_tmp42B=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp40E,(void*)_tmp414,0),Cyc_Toc_curr_sp,0);_tmp42C=Cyc_Toc_cast_it(
_tmp401,_tmp42C);{struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_deref_exp(_tmp42C,0);
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_assign_exp(_tmp42D,Cyc_Absyn_var_exp(
_tmp41D,0),0);struct Cyc_Absyn_Stmt*_tmp42F=Cyc_Absyn_exp_stmt(_tmp42E,0);_tmp42F=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42A,_tmp42B,Cyc_Absyn_skip_stmt(
0),0),_tmp42F,0);{struct Cyc_Absyn_Var_d_struct*_tmpB78;struct Cyc_Absyn_Var_d_struct
_tmpB77;struct Cyc_Absyn_Decl*_tmpB76;_tmp42F=Cyc_Absyn_decl_stmt(((_tmpB76=
_cycalloc(sizeof(*_tmpB76)),((_tmpB76->r=(void*)((_tmpB78=_cycalloc(sizeof(*
_tmpB78)),((_tmpB78[0]=((_tmpB77.tag=0,((_tmpB77.f1=_tmp41E,_tmpB77)))),_tmpB78)))),((
_tmpB76->loc=0,_tmpB76)))))),_tmp42F,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB7E;
struct Cyc_Absyn_Var_d_struct _tmpB7D;struct Cyc_Absyn_Decl*_tmpB7C;_tmp42F=Cyc_Absyn_decl_stmt(((
_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->r=(void*)((_tmpB7E=_cycalloc(
sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7D.tag=0,((_tmpB7D.f1=_tmp41A,_tmpB7D)))),
_tmpB7E)))),((_tmpB7C->loc=0,_tmpB7C)))))),_tmp42F,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB84;struct Cyc_Absyn_Var_d_struct _tmpB83;struct Cyc_Absyn_Decl*_tmpB82;_tmp42F=
Cyc_Absyn_decl_stmt(((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->r=(void*)((
_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB83.tag=0,((_tmpB83.f1=
_tmp413,_tmpB83)))),_tmpB84)))),((_tmpB82->loc=0,_tmpB82)))))),_tmp42F,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp42F);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp444=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp446;union Cyc_Absyn_AggrInfoU _tmp447;_LL1EC: {struct Cyc_Absyn_AggrType_struct*
_tmp445=(struct Cyc_Absyn_AggrType_struct*)_tmp444;if(_tmp445->tag != 12)goto
_LL1EE;else{_tmp446=_tmp445->f1;_tmp447=_tmp446.aggr_info;}}_LL1ED: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp447);goto _LL1EB;_LL1EE:;_LL1EF: {struct Cyc_String_pa_struct _tmpB8C;void*
_tmpB8B[1];const char*_tmpB8A;void*_tmpB89;(_tmpB89=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpB8C.tag=0,((_tmpB8C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB8B[0]=& _tmpB8C,Cyc_aprintf(((
_tmpB8A="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB8A,sizeof(char),51))),_tag_dyneither(_tmpB8B,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB89,sizeof(void*),0)));}_LL1EB:;}{struct _tuple0*_tmp44C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44D=Cyc_Absyn_var_exp(_tmp44C,0);struct Cyc_Absyn_Exp*
_tmp44E=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44D,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_neq_exp(_tmp44E,_tmp44F,0);struct Cyc_Absyn_Exp*
_tmp451=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44D,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp452=Cyc_Absyn_exp_stmt(_tmp451,0);struct Cyc_Absyn_Stmt*
_tmp453=Cyc_Absyn_ifthenelse_stmt(_tmp450,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp454=Cyc_Absyn_declare_stmt(_tmp44C,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp453,_tmp452,0),0);return Cyc_Toc_stmt_exp_r(
_tmp454);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp455=e->r;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_Absyn_Exp*_tmp459;struct
_dyneither_ptr*_tmp45A;int _tmp45B;int*_tmp45C;struct Cyc_Absyn_Exp*_tmp45E;struct
_dyneither_ptr*_tmp45F;int _tmp460;int*_tmp461;_LL1F1: {struct Cyc_Absyn_Cast_e_struct*
_tmp456=(struct Cyc_Absyn_Cast_e_struct*)_tmp455;if(_tmp456->tag != 15)goto _LL1F3;
else{_tmp457=_tmp456->f2;}}_LL1F2: {const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB8F="cast on lhs!",_tag_dyneither(_tmpB8F,sizeof(char),13))),_tag_dyneither(
_tmpB8E,sizeof(void*),0)));}_LL1F3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp458=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp455;if(_tmp458->tag != 22)goto _LL1F5;
else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;_tmp45B=_tmp458->f4;_tmp45C=(int*)&
_tmp458->f4;}}_LL1F4: {void*_tmp464=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp459->topt))->v);struct Cyc_Absyn_AggrInfo _tmp466;union Cyc_Absyn_AggrInfoU
_tmp467;_LL1FA: {struct Cyc_Absyn_AggrType_struct*_tmp465=(struct Cyc_Absyn_AggrType_struct*)
_tmp464;if(_tmp465->tag != 12)goto _LL1FC;else{_tmp466=_tmp465->f1;_tmp467=_tmp466.aggr_info;}}
_LL1FB: {struct Cyc_Absyn_Aggrdecl*_tmp468=Cyc_Absyn_get_known_aggrdecl(_tmp467);*
f_tag=Cyc_Toc_get_member_offset(_tmp468,_tmp45A);{const char*_tmpB94;void*_tmpB93[
2];struct Cyc_String_pa_struct _tmpB92;struct Cyc_String_pa_struct _tmpB91;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpB91.tag=0,((_tmpB91.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp45A),((_tmpB92.tag=0,((_tmpB92.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp468->name).f2),((_tmpB93[0]=&
_tmpB92,((_tmpB93[1]=& _tmpB91,Cyc_aprintf(((_tmpB94="_union_%s_%s",
_tag_dyneither(_tmpB94,sizeof(char),13))),_tag_dyneither(_tmpB93,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpB95;*tagged_member_type=Cyc_Absyn_strct(((_tmpB95=
_cycalloc(sizeof(*_tmpB95)),((_tmpB95[0]=str,_tmpB95)))));}if(clear_read)*
_tmp45C=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp468->impl))->tagged;};}
_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1F5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp45D=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp455;if(_tmp45D->tag != 23)goto
_LL1F7;else{_tmp45E=_tmp45D->f1;_tmp45F=_tmp45D->f2;_tmp460=_tmp45D->f4;_tmp461=(
int*)& _tmp45D->f4;}}_LL1F6: {void*_tmp46E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp470;void*_tmp471;
_LL1FF: {struct Cyc_Absyn_PointerType_struct*_tmp46F=(struct Cyc_Absyn_PointerType_struct*)
_tmp46E;if(_tmp46F->tag != 5)goto _LL201;else{_tmp470=_tmp46F->f1;_tmp471=_tmp470.elt_typ;}}
_LL200: {void*_tmp472=Cyc_Tcutil_compress(_tmp471);struct Cyc_Absyn_AggrInfo
_tmp474;union Cyc_Absyn_AggrInfoU _tmp475;_LL204: {struct Cyc_Absyn_AggrType_struct*
_tmp473=(struct Cyc_Absyn_AggrType_struct*)_tmp472;if(_tmp473->tag != 12)goto
_LL206;else{_tmp474=_tmp473->f1;_tmp475=_tmp474.aggr_info;}}_LL205: {struct Cyc_Absyn_Aggrdecl*
_tmp476=Cyc_Absyn_get_known_aggrdecl(_tmp475);*f_tag=Cyc_Toc_get_member_offset(
_tmp476,_tmp45F);{const char*_tmpB9A;void*_tmpB99[2];struct Cyc_String_pa_struct
_tmpB98;struct Cyc_String_pa_struct _tmpB97;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp45F),((_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp476->name).f2),((_tmpB99[0]=& _tmpB98,((_tmpB99[1]=&
_tmpB97,Cyc_aprintf(((_tmpB9A="_union_%s_%s",_tag_dyneither(_tmpB9A,sizeof(char),
13))),_tag_dyneither(_tmpB99,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpB9B;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((
_tmpB9B[0]=str,_tmpB9B)))));}if(clear_read)*_tmp461=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp476->impl))->tagged;};}_LL206:;_LL207: return 0;_LL203:;}_LL201:;
_LL202: return 0;_LL1FE:;}_LL1F7:;_LL1F8: return 0;_LL1F0:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp47C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp47C,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp47D=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp47D,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp47C,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpBFA(unsigned int*_tmpBF9,unsigned int*_tmpBF8,struct _tuple0***
_tmpBF6){for(*_tmpBF9=0;*_tmpBF9 < *_tmpBF8;(*_tmpBF9)++){(*_tmpBF6)[*_tmpBF9]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp47E=e->r;if(e->topt == 0){const char*_tmpB9F;void*_tmpB9E[1];struct Cyc_String_pa_struct
_tmpB9D;(_tmpB9D.tag=0,((_tmpB9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpB9E[0]=& _tmpB9D,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9F="exp_to_c: no type for %s",
_tag_dyneither(_tmpB9F,sizeof(char),25))),_tag_dyneither(_tmpB9E,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp482=
_tmp47E;union Cyc_Absyn_Cnst _tmp484;int _tmp485;struct _tuple0*_tmp488;void*_tmp489;
struct _tuple0*_tmp48B;enum Cyc_Absyn_Primop _tmp48D;struct Cyc_List_List*_tmp48E;
struct Cyc_Absyn_Exp*_tmp490;enum Cyc_Absyn_Incrementor _tmp491;struct Cyc_Absyn_Exp*
_tmp493;struct Cyc_Core_Opt*_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*
_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*
_tmp49F;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*
_tmp4A4;struct Cyc_List_List*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*
_tmp4A8;struct Cyc_Absyn_VarargCallInfo*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AB;
struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_VarargCallInfo*_tmp4AD;struct Cyc_Absyn_VarargCallInfo
_tmp4AE;int _tmp4AF;struct Cyc_List_List*_tmp4B0;struct Cyc_Absyn_VarargInfo*
_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*
_tmp4B7;struct Cyc_List_List*_tmp4B8;void*_tmp4BA;void**_tmp4BB;struct Cyc_Absyn_Exp*
_tmp4BC;int _tmp4BD;enum Cyc_Absyn_Coercion _tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;
struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_Absyn_Exp*
_tmp4C5;void*_tmp4C7;void*_tmp4C9;void*_tmp4CA;struct _dyneither_ptr*_tmp4CC;void*
_tmp4CE;void*_tmp4CF;unsigned int _tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*
_tmp4D5;struct _dyneither_ptr*_tmp4D6;int _tmp4D7;int _tmp4D8;struct Cyc_Absyn_Exp*
_tmp4DA;struct _dyneither_ptr*_tmp4DB;int _tmp4DC;int _tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*
_tmp4E4;struct Cyc_Absyn_Vardecl*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E8;int _tmp4E9;struct _tuple0*_tmp4EB;struct Cyc_List_List*_tmp4EC;struct Cyc_List_List*
_tmp4ED;struct Cyc_Absyn_Aggrdecl*_tmp4EE;void*_tmp4F0;struct Cyc_List_List*
_tmp4F1;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Datatypedecl*_tmp4F4;struct
Cyc_Absyn_Datatypefield*_tmp4F5;struct Cyc_Absyn_MallocInfo _tmp4F9;int _tmp4FA;
struct Cyc_Absyn_Exp*_tmp4FB;void**_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;int _tmp4FE;
struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*
_tmp503;struct _dyneither_ptr*_tmp504;struct Cyc_Absyn_Stmt*_tmp506;_LL209: {
struct Cyc_Absyn_Const_e_struct*_tmp483=(struct Cyc_Absyn_Const_e_struct*)_tmp482;
if(_tmp483->tag != 0)goto _LL20B;else{_tmp484=_tmp483->f1;if((_tmp484.Null_c).tag
!= 1)goto _LL20B;_tmp485=(int)(_tmp484.Null_c).val;}}_LL20A: {struct Cyc_Absyn_Exp*
_tmp50A=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp50A,
_tmp50A))->r;else{struct Cyc_Absyn_Exp*_tmpBA0[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBA0[2]=_tmp50A,((_tmpBA0[1]=_tmp50A,((_tmpBA0[0]=_tmp50A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL208;}_LL20B: {struct Cyc_Absyn_Const_e_struct*
_tmp486=(struct Cyc_Absyn_Const_e_struct*)_tmp482;if(_tmp486->tag != 0)goto _LL20D;}
_LL20C: goto _LL208;_LL20D: {struct Cyc_Absyn_Var_e_struct*_tmp487=(struct Cyc_Absyn_Var_e_struct*)
_tmp482;if(_tmp487->tag != 1)goto _LL20F;else{_tmp488=_tmp487->f1;_tmp489=(void*)
_tmp487->f2;}}_LL20E:{struct _handler_cons _tmp50C;_push_handler(& _tmp50C);{int
_tmp50E=0;if(setjmp(_tmp50C.handler))_tmp50E=1;if(!_tmp50E){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp488))->r;;_pop_handler();}else{void*_tmp50D=(void*)_exn_thrown;void*
_tmp510=_tmp50D;_LL260: {struct Cyc_Dict_Absent_struct*_tmp511=(struct Cyc_Dict_Absent_struct*)
_tmp510;if(_tmp511->tag != Cyc_Dict_Absent)goto _LL262;}_LL261: {const char*_tmpBA4;
void*_tmpBA3[1];struct Cyc_String_pa_struct _tmpBA2;(_tmpBA2.tag=0,((_tmpBA2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp488)),((
_tmpBA3[0]=& _tmpBA2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA4="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBA4,sizeof(char),
32))),_tag_dyneither(_tmpBA3,sizeof(void*),1)))))));}_LL262:;_LL263:(void)_throw(
_tmp510);_LL25F:;}};}goto _LL208;_LL20F: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp48A=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp482;if(_tmp48A->tag != 2)goto
_LL211;else{_tmp48B=_tmp48A->f1;}}_LL210: {const char*_tmpBA7;void*_tmpBA6;(
_tmpBA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA7="unknownid",_tag_dyneither(_tmpBA7,sizeof(char),10))),_tag_dyneither(
_tmpBA6,sizeof(void*),0)));}_LL211: {struct Cyc_Absyn_Primop_e_struct*_tmp48C=(
struct Cyc_Absyn_Primop_e_struct*)_tmp482;if(_tmp48C->tag != 3)goto _LL213;else{
_tmp48D=_tmp48C->f1;_tmp48E=_tmp48C->f2;}}_LL212: {struct Cyc_List_List*_tmp517=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp48E);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp48E);switch(_tmp48D){case Cyc_Absyn_Numelts: _LL264: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48E))->hd;{void*_tmp518=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp51A;struct Cyc_Absyn_Exp*
_tmp51B;struct Cyc_Absyn_PtrInfo _tmp51D;void*_tmp51E;struct Cyc_Absyn_PtrAtts
_tmp51F;union Cyc_Absyn_Constraint*_tmp520;union Cyc_Absyn_Constraint*_tmp521;
union Cyc_Absyn_Constraint*_tmp522;_LL267: {struct Cyc_Absyn_ArrayType_struct*
_tmp519=(struct Cyc_Absyn_ArrayType_struct*)_tmp518;if(_tmp519->tag != 9)goto
_LL269;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp51A.num_elts;}}_LL268: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp51B))){const char*_tmpBAA;void*_tmpBA9;(
_tmpBA9=0,Cyc_Tcutil_terr(e->loc,((_tmpBAA="can't calculate numelts",
_tag_dyneither(_tmpBAA,sizeof(char),24))),_tag_dyneither(_tmpBA9,sizeof(void*),0)));}
e->r=_tmp51B->r;goto _LL266;_LL269: {struct Cyc_Absyn_PointerType_struct*_tmp51C=(
struct Cyc_Absyn_PointerType_struct*)_tmp518;if(_tmp51C->tag != 5)goto _LL26B;else{
_tmp51D=_tmp51C->f1;_tmp51E=_tmp51D.elt_typ;_tmp51F=_tmp51D.ptr_atts;_tmp520=
_tmp51F.nullable;_tmp521=_tmp51F.bounds;_tmp522=_tmp51F.zero_term;}}_LL26A:{void*
_tmp525=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp521);struct Cyc_Absyn_Exp*_tmp528;_LL26E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp526=(struct Cyc_Absyn_DynEither_b_struct*)_tmp525;if(_tmp526->tag != 0)goto
_LL270;}_LL26F:{struct Cyc_Absyn_Exp*_tmpBAB[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBAB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp51E),0),((_tmpBAB[0]=(
struct Cyc_Absyn_Exp*)_tmp48E->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBAB,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26D;_LL270: {struct Cyc_Absyn_Upper_b_struct*_tmp527=(struct Cyc_Absyn_Upper_b_struct*)
_tmp525;if(_tmp527->tag != 1)goto _LL26D;else{_tmp528=_tmp527->f1;}}_LL271: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp522)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp48E->hd);struct Cyc_Absyn_Exp*_tmpBAC[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBAC[1]=_tmp528,((_tmpBAC[0]=(struct Cyc_Absyn_Exp*)_tmp48E->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBAC,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp520)){if(!Cyc_Evexp_c_can_eval(_tmp528)){const
char*_tmpBAF;void*_tmpBAE;(_tmpBAE=0,Cyc_Tcutil_terr(e->loc,((_tmpBAF="can't calculate numelts",
_tag_dyneither(_tmpBAF,sizeof(char),24))),_tag_dyneither(_tmpBAE,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp528,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp528->r;goto _LL26D;}}goto _LL26D;_LL26D:;}goto _LL266;_LL26B:;_LL26C: {const
char*_tmpBB4;void*_tmpBB3[2];struct Cyc_String_pa_struct _tmpBB2;struct Cyc_String_pa_struct
_tmpBB1;(_tmpBB1.tag=0,((_tmpBB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBB3[0]=& _tmpBB2,((
_tmpBB3[1]=& _tmpBB1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB4="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB4,
sizeof(char),41))),_tag_dyneither(_tmpBB3,sizeof(void*),2)))))))))))));}_LL266:;}
break;}case Cyc_Absyn_Plus: _LL265:{void*_tmp531=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp517))->hd);struct Cyc_Absyn_PtrInfo _tmp533;
void*_tmp534;struct Cyc_Absyn_PtrAtts _tmp535;union Cyc_Absyn_Constraint*_tmp536;
union Cyc_Absyn_Constraint*_tmp537;_LL274: {struct Cyc_Absyn_PointerType_struct*
_tmp532=(struct Cyc_Absyn_PointerType_struct*)_tmp531;if(_tmp532->tag != 5)goto
_LL276;else{_tmp533=_tmp532->f1;_tmp534=_tmp533.elt_typ;_tmp535=_tmp533.ptr_atts;
_tmp536=_tmp535.bounds;_tmp537=_tmp535.zero_term;}}_LL275:{void*_tmp538=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp536);struct Cyc_Absyn_Exp*_tmp53B;_LL279: {struct Cyc_Absyn_DynEither_b_struct*
_tmp539=(struct Cyc_Absyn_DynEither_b_struct*)_tmp538;if(_tmp539->tag != 0)goto
_LL27B;}_LL27A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48E->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBB5[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB5[2]=e2,((_tmpBB5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp534),0),((_tmpBB5[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB5,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL278;}_LL27B: {struct Cyc_Absyn_Upper_b_struct*_tmp53A=(struct
Cyc_Absyn_Upper_b_struct*)_tmp538;if(_tmp53A->tag != 1)goto _LL278;else{_tmp53B=
_tmp53A->f1;}}_LL27C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp537)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48E->tl))->hd;struct Cyc_Absyn_Exp*_tmpBB6[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBB6[2]=e2,((_tmpBB6[1]=_tmp53B,((_tmpBB6[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL278;_LL278:;}goto _LL273;_LL276:;_LL277: goto _LL273;
_LL273:;}break;case Cyc_Absyn_Minus: _LL272: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp517->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBB7[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB7[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBB7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBB7[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL27D: goto _LL27E;case Cyc_Absyn_Neq: _LL27E: goto _LL27F;case Cyc_Absyn_Gt: _LL27F:
goto _LL280;case Cyc_Absyn_Gte: _LL280: goto _LL281;case Cyc_Absyn_Lt: _LL281: goto
_LL282;case Cyc_Absyn_Lte: _LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48E->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp517->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
break;}default: _LL283: break;}goto _LL208;}_LL213: {struct Cyc_Absyn_Increment_e_struct*
_tmp48F=(struct Cyc_Absyn_Increment_e_struct*)_tmp482;if(_tmp48F->tag != 5)goto
_LL215;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;}}_LL214: {void*e2_cyc_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp490->topt))->v;void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;const char*_tmpBB8;struct _dyneither_ptr incr_str=(_tmpBB8="increment",
_tag_dyneither(_tmpBB8,sizeof(char),10));if(_tmp491 == Cyc_Absyn_PreDec  || 
_tmp491 == Cyc_Absyn_PostDec){const char*_tmpBB9;incr_str=((_tmpBB9="decrement",
_tag_dyneither(_tmpBB9,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp490,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBBD;void*_tmpBBC[1];
struct Cyc_String_pa_struct _tmpBBB;(_tmpBBB.tag=0,((_tmpBBB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBC[0]=& _tmpBBB,Cyc_Tcutil_terr(
e->loc,((_tmpBBD="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBBD,sizeof(char),74))),_tag_dyneither(_tmpBBC,sizeof(void*),1)))))));}{
const char*_tmpBC0;void*_tmpBBF;(_tmpBBF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC0="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBC0,sizeof(char),30))),_tag_dyneither(_tmpBBF,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp490,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBC1;_tmp545->topt=((_tmpBC1=_cycalloc(sizeof(*
_tmpBC1)),((_tmpBC1->v=Cyc_Absyn_sint_typ,_tmpBC1))));}switch(_tmp491){case Cyc_Absyn_PreInc:
_LL285:{struct Cyc_Absyn_AssignOp_e_struct _tmpBC7;struct Cyc_Core_Opt*_tmpBC6;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBC5;e->r=(void*)((_tmpBC5=_cycalloc(
sizeof(*_tmpBC5)),((_tmpBC5[0]=((_tmpBC7.tag=4,((_tmpBC7.f1=_tmp490,((_tmpBC7.f2=((
_tmpBC6=_cycalloc_atomic(sizeof(*_tmpBC6)),((_tmpBC6->v=(void*)Cyc_Absyn_Plus,
_tmpBC6)))),((_tmpBC7.f3=_tmp545,_tmpBC7)))))))),_tmpBC5))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL286:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBCD;struct Cyc_Core_Opt*_tmpBCC;struct Cyc_Absyn_AssignOp_e_struct*_tmpBCB;e->r=(
void*)((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCD.tag=4,((
_tmpBCD.f1=_tmp490,((_tmpBCD.f2=((_tmpBCC=_cycalloc_atomic(sizeof(*_tmpBCC)),((
_tmpBCC->v=(void*)Cyc_Absyn_Minus,_tmpBCC)))),((_tmpBCD.f3=_tmp545,_tmpBCD)))))))),
_tmpBCB))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL287:{const char*_tmpBD1;
void*_tmpBD0[1];struct Cyc_String_pa_struct _tmpBCF;(_tmpBCF.tag=0,((_tmpBCF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD0[0]=& _tmpBCF,Cyc_Tcutil_terr(
e->loc,((_tmpBD1="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBD1,sizeof(char),59))),_tag_dyneither(_tmpBD0,sizeof(void*),1)))))));}{
const char*_tmpBD4;void*_tmpBD3;(_tmpBD3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD4="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBD4,sizeof(char),34))),_tag_dyneither(_tmpBD3,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp490);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp491 == Cyc_Absyn_PostInc
 || _tmp491 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp491 == Cyc_Absyn_PreDec  || _tmp491 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBD5[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD5[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBD5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBD5[0]=Cyc_Toc_push_address_exp(_tmp490),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD5,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp553=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp490->topt))->v);enum Cyc_Absyn_Size_of _tmp555;int _tmp558;_LL28A: {
struct Cyc_Absyn_IntType_struct*_tmp554=(struct Cyc_Absyn_IntType_struct*)_tmp553;
if(_tmp554->tag != 6)goto _LL28C;else{_tmp555=_tmp554->f2;}}_LL28B: switch(_tmp555){
case Cyc_Absyn_Char_sz: _LL294: fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL295: fn_e=
_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL296: fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL297: {struct Cyc_Core_Impossible_struct
_tmpBDB;const char*_tmpBDA;struct Cyc_Core_Impossible_struct*_tmpBD9;(int)_throw((
void*)((_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((_tmpBD9[0]=((_tmpBDB.tag=Cyc_Core_Impossible,((
_tmpBDB.f1=((_tmpBDA="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBDA,sizeof(char),44))),_tmpBDB)))),_tmpBD9)))));}}goto _LL289;
_LL28C: {struct Cyc_Absyn_FloatType_struct*_tmp556=(struct Cyc_Absyn_FloatType_struct*)
_tmp553;if(_tmp556->tag != 7)goto _LL28E;}_LL28D: fn_e=_tmp491 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL289;_LL28E: {struct Cyc_Absyn_DoubleType_struct*_tmp557=(struct Cyc_Absyn_DoubleType_struct*)
_tmp553;if(_tmp557->tag != 8)goto _LL290;else{_tmp558=_tmp557->f1;}}_LL28F: switch(
_tmp558){case 1: _LL299: fn_e=_tmp491 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL29A: fn_e=_tmp491 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL289;_LL290: {struct Cyc_Absyn_PointerType_struct*_tmp559=(struct Cyc_Absyn_PointerType_struct*)
_tmp553;if(_tmp559->tag != 5)goto _LL292;}_LL291: fn_e=_tmp491 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL289;_LL292:;_LL293: {struct Cyc_Core_Impossible_struct _tmpBE1;const char*
_tmpBE0;struct Cyc_Core_Impossible_struct*_tmpBDF;(int)_throw((void*)((_tmpBDF=
_cycalloc(sizeof(*_tmpBDF)),((_tmpBDF[0]=((_tmpBE1.tag=Cyc_Core_Impossible,((
_tmpBE1.f1=((_tmpBE0="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBE0,sizeof(char),64))),_tmpBE1)))),_tmpBDF)))));}_LL289:;}{
struct Cyc_Absyn_Exp*_tmpBE2[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBE2[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBE2[0]=_tmp490,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE2,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
== (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp490)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp490,0,Cyc_Toc_incr_lvalue,_tmp491);e->r=_tmp490->r;}}}goto _LL208;};};}_LL215: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp492=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp482;if(_tmp492->tag != 4)goto _LL217;else{_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;
_tmp495=_tmp492->f3;}}_LL216: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp493->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp495->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp493,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp493);Cyc_Toc_exp_to_c(nv,
_tmp495);e->r=Cyc_Toc_tagged_union_assignop(_tmp493,e1_old_typ,_tmp494,_tmp495,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp493,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp493,_tmp494,_tmp495,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp493);Cyc_Toc_exp_to_c(nv,_tmp493);Cyc_Toc_exp_to_c(nv,_tmp495);{int done=0;
if(_tmp494 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp494->v){case Cyc_Absyn_Plus: _LL29C: change=_tmp495;break;case Cyc_Absyn_Minus:
_LL29D: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp495,0);break;default:
_LL29E: {const char*_tmpBE5;void*_tmpBE4;(_tmpBE4=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE5="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBE5,sizeof(char),27))),_tag_dyneither(_tmpBE4,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp564=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBE6[3];e->r=Cyc_Toc_fncall_exp_r(_tmp564,((_tmpBE6[2]=change,((
_tmpBE6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBE6[0]=Cyc_Toc_push_address_exp(
_tmp493),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE6,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp494->v){case Cyc_Absyn_Plus:
_LL2A0: done=1;{struct Cyc_Absyn_Exp*_tmpBE7[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp493),((_tmpBE7[1]=_tmp495,((
_tmpBE7[0]=_tmp493,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A1: {const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBEA,sizeof(char),39))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp495->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp495->r,0));if(!Cyc_Absyn_is_lvalue(_tmp493)){{struct _tuple18 _tmpBED;struct
_tuple18*_tmpBEC;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp493,0,Cyc_Toc_assignop_lvalue,((_tmpBEC=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBEC[0]=((_tmpBED.f1=_tmp494,((_tmpBED.f2=
_tmp495,_tmpBED)))),_tmpBEC)))));}e->r=_tmp493->r;}}goto _LL208;};};};}_LL217: {
struct Cyc_Absyn_Conditional_e_struct*_tmp496=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp482;if(_tmp496->tag != 6)goto _LL219;else{_tmp497=_tmp496->f1;_tmp498=_tmp496->f2;
_tmp499=_tmp496->f3;}}_LL218: Cyc_Toc_exp_to_c(nv,_tmp497);Cyc_Toc_exp_to_c(nv,
_tmp498);Cyc_Toc_exp_to_c(nv,_tmp499);goto _LL208;_LL219: {struct Cyc_Absyn_And_e_struct*
_tmp49A=(struct Cyc_Absyn_And_e_struct*)_tmp482;if(_tmp49A->tag != 7)goto _LL21B;
else{_tmp49B=_tmp49A->f1;_tmp49C=_tmp49A->f2;}}_LL21A: Cyc_Toc_exp_to_c(nv,
_tmp49B);Cyc_Toc_exp_to_c(nv,_tmp49C);goto _LL208;_LL21B: {struct Cyc_Absyn_Or_e_struct*
_tmp49D=(struct Cyc_Absyn_Or_e_struct*)_tmp482;if(_tmp49D->tag != 8)goto _LL21D;
else{_tmp49E=_tmp49D->f1;_tmp49F=_tmp49D->f2;}}_LL21C: Cyc_Toc_exp_to_c(nv,
_tmp49E);Cyc_Toc_exp_to_c(nv,_tmp49F);goto _LL208;_LL21D: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4A0=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp482;if(_tmp4A0->tag != 9)goto _LL21F;
else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}_LL21E: Cyc_Toc_exp_to_c(nv,
_tmp4A1);Cyc_Toc_exp_to_c(nv,_tmp4A2);goto _LL208;_LL21F: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4A3=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp482;if(_tmp4A3->tag != 10)goto
_LL221;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}}_LL220: _tmp4A7=_tmp4A4;
_tmp4A8=_tmp4A5;goto _LL222;_LL221: {struct Cyc_Absyn_FnCall_e_struct*_tmp4A6=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp482;if(_tmp4A6->tag != 11)goto _LL223;else{
_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;_tmp4A9=_tmp4A6->f3;if(_tmp4A9 != 0)goto
_LL223;}}_LL222: Cyc_Toc_exp_to_c(nv,_tmp4A7);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4A8);goto _LL208;_LL223: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4AA=(struct Cyc_Absyn_FnCall_e_struct*)_tmp482;if(_tmp4AA->tag != 11)goto
_LL225;else{_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AA->f2;_tmp4AD=_tmp4AA->f3;if(
_tmp4AD == 0)goto _LL225;_tmp4AE=*_tmp4AD;_tmp4AF=_tmp4AE.num_varargs;_tmp4B0=
_tmp4AE.injectors;_tmp4B1=_tmp4AE.vai;}}_LL224: {struct Cyc_Toc_Env _tmp56C;struct
_RegionHandle*_tmp56D;struct Cyc_Toc_Env*_tmp56B=nv;_tmp56C=*_tmp56B;_tmp56D=
_tmp56C.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4AF,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4B1->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4AC);int num_normargs=num_args - _tmp4AF;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4AC=_tmp4AC->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AC))->hd);{
struct Cyc_List_List*_tmpBEE;new_args=((_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((
_tmpBEE->hd=(struct Cyc_Absyn_Exp*)_tmp4AC->hd,((_tmpBEE->tl=new_args,_tmpBEE))))));};}}{
struct Cyc_Absyn_Exp*_tmpBF1[3];struct Cyc_List_List*_tmpBF0;new_args=((_tmpBF0=
_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBF1[2]=num_varargs_exp,((_tmpBF1[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBF1[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBF0->tl=
new_args,_tmpBF0))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4AB);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AB,new_args,0),0);if(_tmp4B1->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp571=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4B1->type));struct Cyc_Absyn_DatatypeInfo _tmp573;union Cyc_Absyn_DatatypeInfoU
_tmp574;struct Cyc_Absyn_Datatypedecl**_tmp575;struct Cyc_Absyn_Datatypedecl*
_tmp576;_LL2A4: {struct Cyc_Absyn_DatatypeType_struct*_tmp572=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp571;if(_tmp572->tag != 3)goto _LL2A6;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.datatype_info;
if((_tmp574.KnownDatatype).tag != 2)goto _LL2A6;_tmp575=(struct Cyc_Absyn_Datatypedecl**)(
_tmp574.KnownDatatype).val;_tmp576=*_tmp575;}}_LL2A5: tud=_tmp576;goto _LL2A3;
_LL2A6:;_LL2A7: {const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF4="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBF4,sizeof(char),44))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}
_LL2A3:;}{unsigned int _tmpBF9;unsigned int _tmpBF8;struct _dyneither_ptr _tmpBF7;
struct _tuple0**_tmpBF6;unsigned int _tmpBF5;struct _dyneither_ptr vs=(_tmpBF5=(
unsigned int)_tmp4AF,((_tmpBF6=(struct _tuple0**)_region_malloc(_tmp56D,
_check_times(sizeof(struct _tuple0*),_tmpBF5)),((_tmpBF7=_tag_dyneither(_tmpBF6,
sizeof(struct _tuple0*),_tmpBF5),((((_tmpBF8=_tmpBF5,_tmpBFA(& _tmpBF9,& _tmpBF8,&
_tmpBF6))),_tmpBF7)))))));if(_tmp4AF != 0){struct Cyc_List_List*_tmp579=0;{int i=
_tmp4AF - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpBFB;_tmp579=((_tmpBFB=
_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpBFB->tl=_tmp579,_tmpBFB))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp579,0),s,0);{
int i=0;for(0;_tmp4AC != 0;(((_tmp4AC=_tmp4AC->tl,_tmp4B0=_tmp4B0->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4AC->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp57C;struct
_tuple0*_tmp57D;struct Cyc_List_List*_tmp57E;struct Cyc_Absyn_Datatypefield*
_tmp57B=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4B0))->hd;_tmp57C=*_tmp57B;_tmp57D=_tmp57C.name;_tmp57E=_tmp57C.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp57E))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp57D),0),s,0);{const char*_tmpBFC;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp57D,((_tmpBFC="_struct",_tag_dyneither(_tmpBFC,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpBFD[3];struct Cyc_List_List*
_tmp580=(_tmpBFD[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBFD[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpBFD[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBFD,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4AC
!= 0;(_tmp4AC=_tmp4AC->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AC->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AC->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL208;}_LL225: {struct Cyc_Absyn_Throw_e_struct*_tmp4B2=(struct Cyc_Absyn_Throw_e_struct*)
_tmp482;if(_tmp4B2->tag != 12)goto _LL227;else{_tmp4B3=_tmp4B2->f1;}}_LL226: Cyc_Toc_exp_to_c(
nv,_tmp4B3);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4B3),0))->r;goto _LL208;_LL227: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4B4=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp482;if(_tmp4B4->tag != 13)
goto _LL229;else{_tmp4B5=_tmp4B4->f1;}}_LL228: Cyc_Toc_exp_to_c(nv,_tmp4B5);goto
_LL208;_LL229: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4B6=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp482;if(_tmp4B6->tag != 14)goto _LL22B;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;}}
_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B7);for(0;_tmp4B8 != 0;_tmp4B8=_tmp4B8->tl){enum 
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4B8->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp586=Cyc_Tcutil_compress((void*)_tmp4B8->hd);_LL2A9: {struct Cyc_Absyn_VarType_struct*
_tmp587=(struct Cyc_Absyn_VarType_struct*)_tmp586;if(_tmp587->tag != 2)goto _LL2AB;}
_LL2AA: goto _LL2AC;_LL2AB: {struct Cyc_Absyn_DatatypeType_struct*_tmp588=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp586;if(_tmp588->tag != 3)goto _LL2AD;}_LL2AC:
continue;_LL2AD:;_LL2AE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4B7,0))->r;goto _LL2A8;_LL2A8:;}
break;}}goto _LL208;_LL22B: {struct Cyc_Absyn_Cast_e_struct*_tmp4B9=(struct Cyc_Absyn_Cast_e_struct*)
_tmp482;if(_tmp4B9->tag != 15)goto _LL22D;else{_tmp4BA=(void**)& _tmp4B9->f1;
_tmp4BB=(void**)((void**)& _tmp4B9->f1);_tmp4BC=_tmp4B9->f2;_tmp4BD=_tmp4B9->f3;
_tmp4BE=_tmp4B9->f4;}}_LL22C: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BC->topt))->v;void*new_typ=*_tmp4BB;*_tmp4BB=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BC);{struct _tuple17 _tmpBFE;struct _tuple17
_tmp58A=(_tmpBFE.f1=Cyc_Tcutil_compress(old_t2),((_tmpBFE.f2=Cyc_Tcutil_compress(
new_typ),_tmpBFE)));void*_tmp58B;struct Cyc_Absyn_PtrInfo _tmp58D;void*_tmp58E;
struct Cyc_Absyn_PtrInfo _tmp590;void*_tmp591;struct Cyc_Absyn_PtrInfo _tmp593;void*
_tmp594;_LL2B0: _tmp58B=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58C=(
struct Cyc_Absyn_PointerType_struct*)_tmp58B;if(_tmp58C->tag != 5)goto _LL2B2;else{
_tmp58D=_tmp58C->f1;}};_tmp58E=_tmp58A.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp58F=(struct Cyc_Absyn_PointerType_struct*)_tmp58E;if(_tmp58F->tag != 5)goto
_LL2B2;else{_tmp590=_tmp58F->f1;}};_LL2B1: {int _tmp596=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp58D.ptr_atts).nullable);int _tmp597=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).nullable);
void*_tmp598=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp58D.ptr_atts).bounds);void*_tmp599=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp590.ptr_atts).bounds);
int _tmp59A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).zero_term);int _tmp59B=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).zero_term);{struct _tuple17 _tmpBFF;
struct _tuple17 _tmp59D=(_tmpBFF.f1=_tmp598,((_tmpBFF.f2=_tmp599,_tmpBFF)));void*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;
void*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AE;void*_tmp5B0;_LL2B7: _tmp59E=_tmp59D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_struct*)_tmp59E;
if(_tmp59F->tag != 1)goto _LL2B9;else{_tmp5A0=_tmp59F->f1;}};_tmp5A1=_tmp59D.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A1;
if(_tmp5A2->tag != 1)goto _LL2B9;else{_tmp5A3=_tmp5A2->f1;}};_LL2B8: if((!Cyc_Evexp_c_can_eval(
_tmp5A0) || !Cyc_Evexp_c_can_eval(_tmp5A3)) && !Cyc_Evexp_same_const_exp(_tmp5A0,
_tmp5A3)){const char*_tmpC02;void*_tmpC01;(_tmpC01=0,Cyc_Tcutil_terr(e->loc,((
_tmpC02="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC02,sizeof(char),54))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}if(_tmp596
 && !_tmp597){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC05;void*_tmpC04;(
_tmpC04=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC05="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC05,
sizeof(char),44))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}if(_tmp4BE != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC09;void*_tmpC08[1];struct Cyc_String_pa_struct _tmpC07;(_tmpC07.tag=
0,((_tmpC07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC08[0]=& _tmpC07,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC09="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC09,sizeof(char),41))),_tag_dyneither(_tmpC08,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4BC);if(do_null_check){if(!_tmp4BD){
const char*_tmpC0C;void*_tmpC0B;(_tmpC0B=0,Cyc_Tcutil_warn(e->loc,((_tmpC0C="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC0C,sizeof(char),58))),_tag_dyneither(_tmpC0B,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC0D;e->r=Cyc_Toc_cast_it_r(*_tmp4BB,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->hd=
_tmp4BC,((_tmpC0D->tl=0,_tmpC0D)))))),0));};}};}goto _LL2B6;_LL2B9: _tmp5A4=
_tmp59D.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5A5=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5A4;if(_tmp5A5->tag != 1)goto _LL2BB;else{_tmp5A6=_tmp5A5->f1;}};_tmp5A7=
_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5A8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A7;if(_tmp5A8->tag != 0)goto _LL2BB;};_LL2BA: if(!Cyc_Evexp_c_can_eval(_tmp5A6)){
const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,Cyc_Tcutil_terr(e->loc,((_tmpC10="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC10,sizeof(char),71))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}
if(_tmp4BE == Cyc_Absyn_NonNull_to_Null){const char*_tmpC14;void*_tmpC13[1];struct
Cyc_String_pa_struct _tmpC12;(_tmpC12.tag=0,((_tmpC12.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC13[0]=& _tmpC12,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC14="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC14,sizeof(char),44))),_tag_dyneither(_tmpC13,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp59A  && !(_tmp590.elt_tq).real_const) && !
_tmp59B)_tmp5A6=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5A6,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5A6,_tmp4BC))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Toc__tag_dyneither_e;if(_tmp59A){struct _tuple0*
_tmp5C2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Absyn_var_exp(_tmp5C2,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5C4=_tmp4BC->r;union Cyc_Absyn_Cnst _tmp5C6;
struct _dyneither_ptr _tmp5C7;_LL2C0: {struct Cyc_Absyn_Const_e_struct*_tmp5C5=(
struct Cyc_Absyn_Const_e_struct*)_tmp5C4;if(_tmp5C5->tag != 0)goto _LL2C2;else{
_tmp5C6=_tmp5C5->f1;if((_tmp5C6.String_c).tag != 7)goto _LL2C2;_tmp5C7=(struct
_dyneither_ptr)(_tmp5C6.String_c).val;}}_LL2C1: arg3=_tmp5A6;goto _LL2BF;_LL2C2:;
_LL2C3:{struct Cyc_Absyn_Exp*_tmpC15[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4BC),((_tmpC15[1]=_tmp5A6,((_tmpC15[0]=
_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC15,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}goto _LL2BF;
_LL2BF:;}if(!_tmp59B  && !(_tmp590.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp590.elt_typ),0);struct Cyc_Absyn_Exp*_tmpC16[3];struct Cyc_Absyn_Exp*
_tmp5CA=Cyc_Absyn_fncall_exp(_tmp5C1,((_tmpC16[2]=arg3,((_tmpC16[1]=_tmp5C9,((
_tmpC16[0]=_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC16,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);struct Cyc_Absyn_Stmt*
_tmp5CB=Cyc_Absyn_exp_stmt(_tmp5CA,0);_tmp5CB=Cyc_Absyn_declare_stmt(_tmp5C2,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp4BC,_tmp5CB,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CB);};}
else{struct Cyc_Absyn_Exp*_tmpC17[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C1,((_tmpC17[
2]=_tmp5A6,((_tmpC17[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp590.elt_typ),
0),((_tmpC17[0]=_tmp4BC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC17,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B6;
_LL2BB: _tmp5A9=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A9;if(_tmp5AA->tag != 0)goto _LL2BD;};_tmp5AB=_tmp59D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AC=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AB;if(_tmp5AC->tag != 1)goto _LL2BD;
else{_tmp5AD=_tmp5AC->f1;}};_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5AD)){const char*
_tmpC1A;void*_tmpC19;(_tmpC19=0,Cyc_Tcutil_terr(e->loc,((_tmpC1A="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC1A,sizeof(char),71))),_tag_dyneither(_tmpC19,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC1D;void*_tmpC1C;(_tmpC1C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1D="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC1D,sizeof(char),45))),_tag_dyneither(_tmpC1C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D2=_tmp5AD;if(_tmp59A  && !_tmp59B)_tmp5D2=Cyc_Absyn_add_exp(
_tmp5AD,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC1E[3];struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Absyn_fncall_exp(
_tmp5D3,((_tmpC1E[2]=_tmp5D2,((_tmpC1E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58D.elt_typ),0),((_tmpC1E[0]=_tmp4BC,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp597){struct Cyc_List_List*_tmpC1F;_tmp5D4->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->hd=Cyc_Absyn_copy_exp(
_tmp5D4),((_tmpC1F->tl=0,_tmpC1F)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4BB,_tmp5D4);
goto _LL2B6;};};_LL2BD: _tmp5AE=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5AF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5AE;if(_tmp5AF->tag != 0)goto
_LL2B6;};_tmp5B0=_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B1=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto _LL2B6;};_LL2BE:
DynCast: if((_tmp59A  && !_tmp59B) && !(_tmp590.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC22;void*_tmpC21;(_tmpC21=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC22="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC22,sizeof(char),70))),_tag_dyneither(_tmpC21,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC23[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D9,((_tmpC23[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC23[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58D.elt_typ),0),((
_tmpC23[0]=_tmp4BC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC23,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2B6;
_LL2B6:;}goto _LL2AF;}_LL2B2: _tmp591=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp592=(struct Cyc_Absyn_PointerType_struct*)_tmp591;if(_tmp592->tag != 5)goto
_LL2B4;else{_tmp593=_tmp592->f1;}};_tmp594=_tmp58A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp595=(struct Cyc_Absyn_IntType_struct*)_tmp594;if(_tmp595->tag != 6)goto _LL2B4;};
_LL2B3:{void*_tmp5DB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp593.ptr_atts).bounds);_LL2C5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5DC=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5DB;if(_tmp5DC->tag != 0)goto
_LL2C7;}_LL2C6: _tmp4BC->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BC->r,
_tmp4BC->loc),Cyc_Toc_curr_sp);goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}
goto _LL2AF;_LL2B4:;_LL2B5: goto _LL2AF;_LL2AF:;}goto _LL208;}_LL22D: {struct Cyc_Absyn_Address_e_struct*
_tmp4BF=(struct Cyc_Absyn_Address_e_struct*)_tmp482;if(_tmp4BF->tag != 16)goto
_LL22F;else{_tmp4C0=_tmp4BF->f1;}}_LL22E:{void*_tmp5DD=_tmp4C0->r;struct _tuple0*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*
_tmp5E3;_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5DE=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5DD;if(_tmp5DE->tag != 29)goto _LL2CC;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;
_tmp5E1=_tmp5DE->f3;}}_LL2CB: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC27;void*
_tmpC26[1];struct Cyc_String_pa_struct _tmpC25;(_tmpC25.tag=0,((_tmpC25.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4C0->loc)),((
_tmpC26[0]=& _tmpC25,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC27="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC27,
sizeof(char),42))),_tag_dyneither(_tmpC26,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C0->topt))->v,_tmp5E0 != 0,1,0,
_tmp5E1,_tmp5DF))->r;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E2=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DD;if(_tmp5E2->tag != 25)goto _LL2CE;else{
_tmp5E3=_tmp5E2->f1;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2B;void*
_tmpC2A[1];struct Cyc_String_pa_struct _tmpC29;(_tmpC29.tag=0,((_tmpC29.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4C0->loc)),((
_tmpC2A[0]=& _tmpC29,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC2B="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC2B,
sizeof(char),42))),_tag_dyneither(_tmpC2A,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E3))->r;goto _LL2C9;_LL2CE:;_LL2CF: Cyc_Toc_exp_to_c(nv,_tmp4C0);if(!
Cyc_Absyn_is_lvalue(_tmp4C0)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4C0,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4C0);}goto _LL2C9;
_LL2C9:;}goto _LL208;_LL22F: {struct Cyc_Absyn_New_e_struct*_tmp4C1=(struct Cyc_Absyn_New_e_struct*)
_tmp482;if(_tmp4C1->tag != 17)goto _LL231;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;}}
_LL230: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2F;void*_tmpC2E[1];struct Cyc_String_pa_struct
_tmpC2D;(_tmpC2D.tag=0,((_tmpC2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C3->loc)),((_tmpC2E[0]=& _tmpC2D,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC2F="%s: new at top-level",
_tag_dyneither(_tmpC2F,sizeof(char),21))),_tag_dyneither(_tmpC2E,sizeof(void*),1)))))));}{
void*_tmp5ED=_tmp4C3->r;struct Cyc_List_List*_tmp5EF;struct Cyc_Absyn_Vardecl*
_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;int _tmp5F4;
struct _tuple0*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_List_List*_tmp5F8;
struct Cyc_Absyn_Aggrdecl*_tmp5F9;struct Cyc_List_List*_tmp5FB;_LL2D1: {struct Cyc_Absyn_Array_e_struct*
_tmp5EE=(struct Cyc_Absyn_Array_e_struct*)_tmp5ED;if(_tmp5EE->tag != 27)goto _LL2D3;
else{_tmp5EF=_tmp5EE->f1;}}_LL2D2: {struct _tuple0*_tmp5FC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Absyn_var_exp(_tmp5FC,0);struct Cyc_Absyn_Stmt*
_tmp5FE=Cyc_Toc_init_array(nv,_tmp5FD,_tmp5EF,Cyc_Absyn_exp_stmt(_tmp5FD,0));
void*old_elt_typ;{void*_tmp5FF=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp601;void*_tmp602;struct Cyc_Absyn_Tqual _tmp603;struct Cyc_Absyn_PtrAtts _tmp604;
union Cyc_Absyn_Constraint*_tmp605;_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp600=(struct Cyc_Absyn_PointerType_struct*)_tmp5FF;if(_tmp600->tag != 5)goto
_LL2DE;else{_tmp601=_tmp600->f1;_tmp602=_tmp601.elt_typ;_tmp603=_tmp601.elt_tq;
_tmp604=_tmp601.ptr_atts;_tmp605=_tmp604.zero_term;}}_LL2DD: old_elt_typ=_tmp602;
goto _LL2DB;_LL2DE:;_LL2DF: {const char*_tmpC32;void*_tmpC31;old_elt_typ=((_tmpC31=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC32="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC32,sizeof(char),52))),_tag_dyneither(_tmpC31,sizeof(void*),0))));}_LL2DB:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp608=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5EF),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C2 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp609);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C2;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp609);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FC,_tmp608,(struct Cyc_Absyn_Exp*)e1,_tmp5FE,0));goto
_LL2D0;};}_LL2D3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F0=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5ED;if(_tmp5F0->tag != 28)goto _LL2D5;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;
_tmp5F3=_tmp5F0->f3;_tmp5F4=_tmp5F0->f4;}}_LL2D4: {int is_dyneither_ptr=0;{void*
_tmp60A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp60C;void*_tmp60D;
struct Cyc_Absyn_Tqual _tmp60E;struct Cyc_Absyn_PtrAtts _tmp60F;union Cyc_Absyn_Constraint*
_tmp610;union Cyc_Absyn_Constraint*_tmp611;_LL2E1: {struct Cyc_Absyn_PointerType_struct*
_tmp60B=(struct Cyc_Absyn_PointerType_struct*)_tmp60A;if(_tmp60B->tag != 5)goto
_LL2E3;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60C.elt_typ;_tmp60E=_tmp60C.elt_tq;
_tmp60F=_tmp60C.ptr_atts;_tmp610=_tmp60F.bounds;_tmp611=_tmp60F.zero_term;}}
_LL2E2: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp610)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E0;
_LL2E3:;_LL2E4: {const char*_tmpC35;void*_tmpC34;(_tmpC34=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC35="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC35,sizeof(char),42))),_tag_dyneither(_tmpC34,sizeof(void*),0)));}
_LL2E0:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F3->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F2);{struct Cyc_Absyn_Exp*_tmp614=
Cyc_Absyn_var_exp(max,0);if(_tmp5F4)_tmp614=Cyc_Absyn_add_exp(_tmp614,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F1,Cyc_Absyn_var_exp(max,0),_tmp5F3,_tmp5F4,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp616;struct _RegionHandle*_tmp617;struct Cyc_Toc_Env*_tmp615=
nv;_tmp616=*_tmp615;_tmp617=_tmp616.rgn;{struct _tuple19*_tmpC38;struct Cyc_List_List*
_tmpC37;struct Cyc_List_List*decls=(_tmpC37=_region_malloc(_tmp617,sizeof(*
_tmpC37)),((_tmpC37->hd=((_tmpC38=_region_malloc(_tmp617,sizeof(*_tmpC38)),((
_tmpC38->f1=max,((_tmpC38->f2=Cyc_Absyn_uint_typ,((_tmpC38->f3=(struct Cyc_Absyn_Exp*)
_tmp5F2,_tmpC38)))))))),((_tmpC37->tl=0,_tmpC37)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4C2 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC39[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC39[1]=_tmp614,((
_tmpC39[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C2;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC3A[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC3A[1]=_tmp614,((_tmpC3A[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC3D;struct Cyc_List_List*
_tmpC3C;decls=((_tmpC3C=_region_malloc(_tmp617,sizeof(*_tmpC3C)),((_tmpC3C->hd=((
_tmpC3D=_region_malloc(_tmp617,sizeof(*_tmpC3D)),((_tmpC3D->f1=a,((_tmpC3D->f2=
ptr_typ,((_tmpC3D->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC3D)))))))),((_tmpC3C->tl=
decls,_tmpC3C))))));}if(is_dyneither_ptr){struct _tuple0*_tmp61C=Cyc_Toc_temp_var();
void*_tmp61D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC3E[3];struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_fncall_exp(
_tmp61E,((_tmpC3E[2]=_tmp614,((_tmpC3E[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC3E[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC41;struct Cyc_List_List*_tmpC40;decls=((_tmpC40=
_region_malloc(_tmp617,sizeof(*_tmpC40)),((_tmpC40->hd=((_tmpC41=_region_malloc(
_tmp617,sizeof(*_tmpC41)),((_tmpC41->f1=_tmp61C,((_tmpC41->f2=_tmp61D,((_tmpC41->f3=(
struct Cyc_Absyn_Exp*)_tmp61F,_tmpC41)))))))),((_tmpC40->tl=decls,_tmpC40))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp61C,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp623=decls;for(0;_tmp623 != 0;_tmp623=_tmp623->tl){struct
_tuple0*_tmp625;void*_tmp626;struct Cyc_Absyn_Exp*_tmp627;struct _tuple19 _tmp624=*((
struct _tuple19*)_tmp623->hd);_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;_tmp627=
_tmp624.f3;s=Cyc_Absyn_declare_stmt(_tmp625,_tmp626,_tmp627,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D0;};};};}_LL2D5: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F5=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5ED;if(_tmp5F5->tag != 29)goto _LL2D7;
else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=_tmp5F5->f3;_tmp5F9=_tmp5F5->f4;}}
_LL2D6: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4C3->topt))->v,_tmp5F7 != 0,1,_tmp4C2,_tmp5F8,_tmp5F6))->r;goto _LL2D0;_LL2D7: {
struct Cyc_Absyn_Tuple_e_struct*_tmp5FA=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5ED;
if(_tmp5FA->tag != 25)goto _LL2D9;else{_tmp5FB=_tmp5FA->f1;}}_LL2D8: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4C2,_tmp5FB))->r;goto _LL2D0;_LL2D9:;_LL2DA: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C3->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp62A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp62B=Cyc_Absyn_var_exp(_tmp62A,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp62B,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4C2
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C2;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp62C=_tmp4C3->r;void*_tmp62E;
struct Cyc_Absyn_Exp*_tmp62F;_LL2E6: {struct Cyc_Absyn_Cast_e_struct*_tmp62D=(
struct Cyc_Absyn_Cast_e_struct*)_tmp62C;if(_tmp62D->tag != 15)goto _LL2E8;else{
_tmp62E=(void*)_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL2E7:{struct _tuple17 _tmpC42;
struct _tuple17 _tmp631=(_tmpC42.f1=Cyc_Tcutil_compress(_tmp62E),((_tmpC42.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp62F->topt))->v),_tmpC42)));void*
_tmp632;struct Cyc_Absyn_PtrInfo _tmp634;void*_tmp635;struct Cyc_Absyn_PtrAtts
_tmp636;union Cyc_Absyn_Constraint*_tmp637;void*_tmp638;struct Cyc_Absyn_PtrInfo
_tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;union Cyc_Absyn_Constraint*_tmp63C;_LL2EB:
_tmp632=_tmp631.f1;{struct Cyc_Absyn_PointerType_struct*_tmp633=(struct Cyc_Absyn_PointerType_struct*)
_tmp632;if(_tmp633->tag != 5)goto _LL2ED;else{_tmp634=_tmp633->f1;_tmp635=_tmp634.elt_typ;
_tmp636=_tmp634.ptr_atts;_tmp637=_tmp636.bounds;}};_tmp638=_tmp631.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp639=(struct Cyc_Absyn_PointerType_struct*)_tmp638;if(_tmp639->tag != 5)goto
_LL2ED;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.ptr_atts;_tmp63C=_tmp63B.bounds;}};
_LL2EC:{struct _tuple17 _tmpC43;struct _tuple17 _tmp63E=(_tmpC43.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp637),((_tmpC43.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63C),_tmpC43)));void*_tmp63F;void*_tmp641;struct Cyc_Absyn_Exp*
_tmp643;_LL2F0: _tmp63F=_tmp63E.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp640=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp63F;if(_tmp640->tag != 0)goto _LL2F2;};
_tmp641=_tmp63E.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp642=(struct Cyc_Absyn_Upper_b_struct*)
_tmp641;if(_tmp642->tag != 1)goto _LL2F2;else{_tmp643=_tmp642->f1;}};_LL2F1: Cyc_Toc_exp_to_c(
nv,_tmp62F);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp644=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC44[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp644,((_tmpC44[3]=_tmp643,((_tmpC44[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp635),0),((_tmpC44[1]=_tmp62F,((_tmpC44[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2EF;};_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}goto _LL2EA;_LL2ED:;
_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2E5;_LL2E8:;_LL2E9: goto _LL2E5;_LL2E5:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_exp_stmt(_tmp62B,0);struct Cyc_Absyn_Exp*
_tmp647=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C3);_tmp646=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62B,_tmp647,0),_tmp4C3,0),
_tmp646,0);{void*_tmp648=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp62A,_tmp648,(struct Cyc_Absyn_Exp*)mexp,_tmp646,0));};}
goto _LL2D0;};}_LL2D0:;}goto _LL208;_LL231: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C4=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp482;if(_tmp4C4->tag != 19)goto
_LL233;else{_tmp4C5=_tmp4C4->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C5);goto _LL208;
_LL233: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C6=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp482;if(_tmp4C6->tag != 18)goto _LL235;else{_tmp4C7=(void*)_tmp4C6->f1;}}_LL234:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC47;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC46;e->r=(void*)((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC47.tag=
18,((_tmpC47.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C7),_tmpC47)))),_tmpC46))));}
goto _LL208;_LL235: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4C8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp482;if(_tmp4C8->tag != 20)goto _LL237;else{_tmp4C9=(void*)_tmp4C8->f1;_tmp4CA=(
void*)_tmp4C8->f2;{struct Cyc_Absyn_StructField_struct*_tmp4CB=(struct Cyc_Absyn_StructField_struct*)
_tmp4CA;if(_tmp4CB->tag != 0)goto _LL237;else{_tmp4CC=_tmp4CB->f1;}};}}_LL236:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC51;struct Cyc_Absyn_StructField_struct
_tmpC50;struct Cyc_Absyn_StructField_struct*_tmpC4F;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC4E;e->r=(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC51.tag=
20,((_tmpC51.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C9),((_tmpC51.f2=(void*)((
void*)((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=0,((
_tmpC50.f1=_tmp4CC,_tmpC50)))),_tmpC4F))))),_tmpC51)))))),_tmpC4E))));}goto
_LL208;_LL237: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4CD=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp482;if(_tmp4CD->tag != 20)goto _LL239;else{_tmp4CE=(void*)_tmp4CD->f1;_tmp4CF=(
void*)_tmp4CD->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4D0=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4CF;if(_tmp4D0->tag != 1)goto _LL239;else{_tmp4D1=_tmp4D0->f1;}};}}_LL238:{
void*_tmp64F=Cyc_Tcutil_compress(_tmp4CE);struct Cyc_Absyn_AggrInfo _tmp651;union
Cyc_Absyn_AggrInfoU _tmp652;struct Cyc_List_List*_tmp654;_LL2F5: {struct Cyc_Absyn_AggrType_struct*
_tmp650=(struct Cyc_Absyn_AggrType_struct*)_tmp64F;if(_tmp650->tag != 12)goto
_LL2F7;else{_tmp651=_tmp650->f1;_tmp652=_tmp651.aggr_info;}}_LL2F6: {struct Cyc_Absyn_Aggrdecl*
_tmp657=Cyc_Absyn_get_known_aggrdecl(_tmp652);if(_tmp657->impl == 0){const char*
_tmpC54;void*_tmpC53;(_tmpC53=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC54="struct fields must be known",
_tag_dyneither(_tmpC54,sizeof(char),28))),_tag_dyneither(_tmpC53,sizeof(void*),0)));}
_tmp654=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp657->impl))->fields;goto
_LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp653=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64F;if(_tmp653->tag != 13)goto _LL2F9;else{_tmp654=_tmp653->f2;}}_LL2F8: {
struct Cyc_Absyn_Aggrfield*_tmp65A=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp654,(int)_tmp4D1);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC5E;struct Cyc_Absyn_StructField_struct _tmpC5D;struct Cyc_Absyn_StructField_struct*
_tmpC5C;struct Cyc_Absyn_Offsetof_e_struct*_tmpC5B;e->r=(void*)((_tmpC5B=
_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5E.tag=20,((_tmpC5E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC5E.f2=(void*)((void*)((_tmpC5C=_cycalloc(
sizeof(*_tmpC5C)),((_tmpC5C[0]=((_tmpC5D.tag=0,((_tmpC5D.f1=_tmp65A->name,
_tmpC5D)))),_tmpC5C))))),_tmpC5E)))))),_tmpC5B))));}goto _LL2F4;}_LL2F9: {struct
Cyc_Absyn_TupleType_struct*_tmp655=(struct Cyc_Absyn_TupleType_struct*)_tmp64F;
if(_tmp655->tag != 11)goto _LL2FB;}_LL2FA:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC68;struct Cyc_Absyn_StructField_struct _tmpC67;struct Cyc_Absyn_StructField_struct*
_tmpC66;struct Cyc_Absyn_Offsetof_e_struct*_tmpC65;e->r=(void*)((_tmpC65=
_cycalloc(sizeof(*_tmpC65)),((_tmpC65[0]=((_tmpC68.tag=20,((_tmpC68.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC68.f2=(void*)((void*)((_tmpC66=_cycalloc(
sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC67.tag=0,((_tmpC67.f1=Cyc_Absyn_fieldname((
int)(_tmp4D1 + 1)),_tmpC67)))),_tmpC66))))),_tmpC68)))))),_tmpC65))));}goto _LL2F4;
_LL2FB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp656=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp64F;if(_tmp656->tag != 4)goto _LL2FD;}_LL2FC: if(_tmp4D1 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC72;struct Cyc_Absyn_StructField_struct _tmpC71;struct Cyc_Absyn_StructField_struct*
_tmpC70;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6F;e->r=(void*)((_tmpC6F=
_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC72.tag=20,((_tmpC72.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC72.f2=(void*)((void*)((_tmpC70=_cycalloc(
sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=0,((_tmpC71.f1=Cyc_Toc_tag_sp,
_tmpC71)))),_tmpC70))))),_tmpC72)))))),_tmpC6F))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC7C;struct Cyc_Absyn_StructField_struct _tmpC7B;struct Cyc_Absyn_StructField_struct*
_tmpC7A;struct Cyc_Absyn_Offsetof_e_struct*_tmpC79;e->r=(void*)((_tmpC79=
_cycalloc(sizeof(*_tmpC79)),((_tmpC79[0]=((_tmpC7C.tag=20,((_tmpC7C.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC7C.f2=(void*)((void*)((_tmpC7A=_cycalloc(
sizeof(*_tmpC7A)),((_tmpC7A[0]=((_tmpC7B.tag=0,((_tmpC7B.f1=Cyc_Absyn_fieldname((
int)_tmp4D1),_tmpC7B)))),_tmpC7A))))),_tmpC7C)))))),_tmpC79))));}goto _LL2F4;
_LL2FD:;_LL2FE: {const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7F="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC7F,sizeof(char),41))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}
_LL2F4:;}goto _LL208;_LL239: {struct Cyc_Absyn_Deref_e_struct*_tmp4D2=(struct Cyc_Absyn_Deref_e_struct*)
_tmp482;if(_tmp4D2->tag != 21)goto _LL23B;else{_tmp4D3=_tmp4D2->f1;}}_LL23A: {void*
_tmp66D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D3->topt))->v);{
void*_tmp66E=_tmp66D;struct Cyc_Absyn_PtrInfo _tmp670;void*_tmp671;struct Cyc_Absyn_Tqual
_tmp672;struct Cyc_Absyn_PtrAtts _tmp673;void*_tmp674;union Cyc_Absyn_Constraint*
_tmp675;union Cyc_Absyn_Constraint*_tmp676;union Cyc_Absyn_Constraint*_tmp677;
_LL300: {struct Cyc_Absyn_PointerType_struct*_tmp66F=(struct Cyc_Absyn_PointerType_struct*)
_tmp66E;if(_tmp66F->tag != 5)goto _LL302;else{_tmp670=_tmp66F->f1;_tmp671=_tmp670.elt_typ;
_tmp672=_tmp670.elt_tq;_tmp673=_tmp670.ptr_atts;_tmp674=_tmp673.rgn;_tmp675=
_tmp673.nullable;_tmp676=_tmp673.bounds;_tmp677=_tmp673.zero_term;}}_LL301:{void*
_tmp678=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp676);struct Cyc_Absyn_Exp*_tmp67A;_LL305: {struct Cyc_Absyn_Upper_b_struct*
_tmp679=(struct Cyc_Absyn_Upper_b_struct*)_tmp678;if(_tmp679->tag != 1)goto _LL307;
else{_tmp67A=_tmp679->f1;}}_LL306: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4D3);Cyc_Toc_exp_to_c(nv,_tmp4D3);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC82;void*_tmpC81;(_tmpC81=0,Cyc_Tcutil_warn(e->loc,((_tmpC82="inserted null check due to dereference",
_tag_dyneither(_tmpC82,sizeof(char),39))),_tag_dyneither(_tmpC81,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC83;_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp66D),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC83=_cycalloc(sizeof(*
_tmpC83)),((_tmpC83->hd=Cyc_Absyn_copy_exp(_tmp4D3),((_tmpC83->tl=0,_tmpC83)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp677)){unsigned int _tmp680;int _tmp681;struct _tuple10 _tmp67F=Cyc_Evexp_eval_const_uint_exp(
_tmp67A);_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;if(!_tmp681  || _tmp680 <= 0){const
char*_tmpC86;void*_tmpC85;(_tmpC85=0,Cyc_Tcutil_terr(e->loc,((_tmpC86="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC86,sizeof(char),42))),_tag_dyneither(_tmpC85,sizeof(void*),0)));}}
goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_struct*_tmp67B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp678;if(_tmp67B->tag != 0)goto _LL304;}_LL308: {struct Cyc_Absyn_Exp*_tmp684=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC87;_tmp684->topt=((_tmpC87=_cycalloc(sizeof(*
_tmpC87)),((_tmpC87->v=Cyc_Absyn_uint_typ,_tmpC87))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4D3,_tmp684);Cyc_Toc_exp_to_c(nv,e);goto _LL304;}_LL304:;}goto _LL2FF;_LL302:;
_LL303: {const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8A="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC8A,sizeof(char),29))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}
_LL2FF:;}goto _LL208;}_LL23B: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D4=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp482;if(_tmp4D4->tag != 22)goto _LL23D;
else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;_tmp4D7=_tmp4D4->f3;_tmp4D8=_tmp4D4->f4;}}
_LL23C: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D5->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4D5);if(
_tmp4D7  && _tmp4D8)e->r=Cyc_Toc_check_tagged_union(_tmp4D5,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4D6,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;}_LL23D: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4D9=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp482;if(_tmp4D9->tag != 23)goto _LL23F;
else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;_tmp4DC=_tmp4D9->f3;_tmp4DD=_tmp4D9->f4;}}
_LL23E: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4DA->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4DA);Cyc_Toc_exp_to_c(
nv,_tmp4DA);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp689;struct Cyc_Absyn_Tqual
_tmp68A;struct Cyc_Absyn_PtrAtts _tmp68B;void*_tmp68C;union Cyc_Absyn_Constraint*
_tmp68D;union Cyc_Absyn_Constraint*_tmp68E;union Cyc_Absyn_Constraint*_tmp68F;
struct Cyc_Absyn_PtrInfo _tmp688=Cyc_Toc_get_ptr_type(e1typ);_tmp689=_tmp688.elt_typ;
_tmp68A=_tmp688.elt_tq;_tmp68B=_tmp688.ptr_atts;_tmp68C=_tmp68B.rgn;_tmp68D=
_tmp68B.nullable;_tmp68E=_tmp68B.bounds;_tmp68F=_tmp68B.zero_term;{void*_tmp690=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp68E);struct Cyc_Absyn_Exp*_tmp692;_LL30A: {struct Cyc_Absyn_Upper_b_struct*
_tmp691=(struct Cyc_Absyn_Upper_b_struct*)_tmp690;if(_tmp691->tag != 1)goto _LL30C;
else{_tmp692=_tmp691->f1;}}_LL30B: {unsigned int _tmp695;int _tmp696;struct
_tuple10 _tmp694=Cyc_Evexp_eval_const_uint_exp(_tmp692);_tmp695=_tmp694.f1;
_tmp696=_tmp694.f2;if(_tmp696){if(_tmp695 < 1){const char*_tmpC8D;void*_tmpC8C;(
_tmpC8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC8D="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC8D,
sizeof(char),44))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,Cyc_Tcutil_warn(
e->loc,((_tmpC90="inserted null check due to dereference",_tag_dyneither(_tmpC90,
sizeof(char),39))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC91[1];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC91[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC91,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp692)){const char*_tmpC94;void*
_tmpC93;(_tmpC93=0,Cyc_Tcutil_terr(e->loc,((_tmpC94="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC94,sizeof(char),47))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC95[4];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC95[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC95[2]=Cyc_Absyn_sizeoftyp_exp(_tmp689,0),((
_tmpC95[1]=_tmp692,((_tmpC95[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp693=(struct Cyc_Absyn_DynEither_b_struct*)_tmp690;if(_tmp693->tag != 0)goto
_LL309;}_LL30D: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp689);{struct Cyc_Absyn_Exp*
_tmpC96[3];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp68A),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC96[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC96[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC96[0]=Cyc_Absyn_new_exp(_tmp4DA->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC96,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL309;}_LL309:;}if(
_tmp4DC  && _tmp4DD)e->r=Cyc_Toc_check_tagged_union(_tmp4DA,Cyc_Toc_typ_to_c(
e1typ),_tmp689,_tmp4DB,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4DD)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;};}_LL23F: {struct Cyc_Absyn_Subscript_e_struct*_tmp4DE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp482;if(_tmp4DE->tag != 24)goto _LL241;
else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL240: {void*_tmp6A0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DF->topt))->v);{void*_tmp6A1=
_tmp6A0;struct Cyc_List_List*_tmp6A3;struct Cyc_Absyn_PtrInfo _tmp6A5;void*_tmp6A6;
struct Cyc_Absyn_Tqual _tmp6A7;struct Cyc_Absyn_PtrAtts _tmp6A8;void*_tmp6A9;union
Cyc_Absyn_Constraint*_tmp6AA;union Cyc_Absyn_Constraint*_tmp6AB;union Cyc_Absyn_Constraint*
_tmp6AC;_LL30F: {struct Cyc_Absyn_TupleType_struct*_tmp6A2=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A1;if(_tmp6A2->tag != 11)goto _LL311;else{_tmp6A3=_tmp6A2->f1;}}_LL310: Cyc_Toc_exp_to_c(
nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);{unsigned int _tmp6AE;int _tmp6AF;struct
_tuple10 _tmp6AD=Cyc_Evexp_eval_const_uint_exp(_tmp4E0);_tmp6AE=_tmp6AD.f1;
_tmp6AF=_tmp6AD.f2;if(!_tmp6AF){const char*_tmpC99;void*_tmpC98;(_tmpC98=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC99="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpC99,sizeof(char),44))),_tag_dyneither(_tmpC98,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DF,Cyc_Absyn_fieldname((int)(_tmp6AE + 1)));
goto _LL30E;};_LL311: {struct Cyc_Absyn_PointerType_struct*_tmp6A4=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A1;if(_tmp6A4->tag != 5)goto _LL313;else{_tmp6A5=_tmp6A4->f1;_tmp6A6=_tmp6A5.elt_typ;
_tmp6A7=_tmp6A5.elt_tq;_tmp6A8=_tmp6A5.ptr_atts;_tmp6A9=_tmp6A8.rgn;_tmp6AA=
_tmp6A8.nullable;_tmp6AB=_tmp6A8.bounds;_tmp6AC=_tmp6A8.zero_term;}}_LL312: {
struct Cyc_List_List*_tmp6B2=Cyc_Toc_get_relns(_tmp4DF);int in_bnds=0;{void*
_tmp6B3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AB);_LL316:;_LL317: in_bnds=Cyc_Toc_check_bounds(_tmp6A0,_tmp6B2,_tmp4DF,
_tmp4E0);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpC9D;void*
_tmpC9C[1];struct Cyc_String_pa_struct _tmpC9B;(_tmpC9B.tag=0,((_tmpC9B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC9C[0]=&
_tmpC9B,Cyc_Tcutil_warn(e->loc,((_tmpC9D="bounds check necessary for %s",
_tag_dyneither(_tmpC9D,sizeof(char),30))),_tag_dyneither(_tmpC9C,sizeof(void*),1)))))));}
_LL315:;}Cyc_Toc_exp_to_c(nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6B7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AB);struct Cyc_Absyn_Exp*_tmp6B9;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp6B8=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B7;if(_tmp6B8->tag != 1)goto _LL31B;
else{_tmp6B9=_tmp6B8->f1;}}_LL31A: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AA);void*ta1=Cyc_Toc_typ_to_c(_tmp6A6);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A7);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AC)){if(!Cyc_Evexp_c_can_eval(_tmp6B9)){const char*_tmpCA0;void*_tmpC9F;(
_tmpC9F=0,Cyc_Tcutil_terr(e->loc,((_tmpCA0="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA0,sizeof(char),40))),_tag_dyneither(_tmpC9F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4DF);struct Cyc_Absyn_Exp*_tmpCA1[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCA1[2]=_tmp4E0,((_tmpCA1[1]=_tmp6B9,((
_tmpCA1[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6B9)){const char*_tmpCA4;void*_tmpCA3;(
_tmpCA3=0,Cyc_Tcutil_terr(e->loc,((_tmpCA4="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA4,sizeof(char),40))),_tag_dyneither(_tmpCA3,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,Cyc_Tcutil_warn(
e->loc,((_tmpCA7="inserted null check due to dereference",_tag_dyneither(_tmpCA7,
sizeof(char),39))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCA8[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCA8[3]=_tmp4E0,((_tmpCA8[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCA8[1]=
_tmp6B9,((_tmpCA8[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA8,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6B9)){const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_terr(
e->loc,((_tmpCAB="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCAB,sizeof(char),40))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAC[2];_tmp4E0->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCAC[1]=Cyc_Absyn_copy_exp(_tmp4E0),((_tmpCAC[0]=_tmp6B9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL318;}_LL31B: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BA=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6B7;if(_tmp6BA->tag != 0)goto _LL318;}
_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6A6);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A7),
Cyc_Absyn_aggrmember_exp(_tmp4DF,Cyc_Toc_curr_sp,0)),_tmp4E0);}else{struct Cyc_Absyn_Exp*
_tmp6C6=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCAD[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A7),Cyc_Absyn_fncall_exp(
_tmp6C6,((_tmpCAD[2]=_tmp4E0,((_tmpCAD[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCAD[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL318;}
_LL318:;}goto _LL30E;}_LL313:;_LL314: {const char*_tmpCB0;void*_tmpCAF;(_tmpCAF=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCB0="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCB0,sizeof(char),49))),_tag_dyneither(_tmpCAF,sizeof(void*),0)));}_LL30E:;}
goto _LL208;}_LL241: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E1=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp482;if(_tmp4E1->tag != 25)goto _LL243;else{_tmp4E2=_tmp4E1->f1;}}_LL242: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E2))->r;else{struct
Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E2);void*_tmp6CB=Cyc_Toc_add_tuple_type(
_tmp6CA);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4E2 != 0;(_tmp4E2=_tmp4E2->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E2->hd);{struct _tuple15*
_tmpCB3;struct Cyc_List_List*_tmpCB2;dles=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((
_tmpCB2->hd=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->f1=0,((_tmpCB3->f2=(
struct Cyc_Absyn_Exp*)_tmp4E2->hd,_tmpCB3)))))),((_tmpCB2->tl=dles,_tmpCB2))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL208;_LL243: {struct Cyc_Absyn_Array_e_struct*
_tmp4E3=(struct Cyc_Absyn_Array_e_struct*)_tmp482;if(_tmp4E3->tag != 27)goto _LL245;
else{_tmp4E4=_tmp4E3->f1;}}_LL244: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E4);{
struct Cyc_List_List*_tmp6CE=_tmp4E4;for(0;_tmp6CE != 0;_tmp6CE=_tmp6CE->tl){
struct _tuple15 _tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct _tuple15*_tmp6CF=(struct
_tuple15*)_tmp6CE->hd;_tmp6D0=*_tmp6CF;_tmp6D1=_tmp6D0.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D1);}}goto _LL208;_LL245: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E5=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp482;if(_tmp4E5->tag != 28)goto _LL247;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL246: {unsigned int _tmp6D3;int _tmp6D4;struct _tuple10 _tmp6D2=Cyc_Evexp_eval_const_uint_exp(
_tmp4E7);_tmp6D3=_tmp6D2.f1;_tmp6D4=_tmp6D2.f2;{void*_tmp6D5=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4E8);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4E8)){if(!_tmp6D4){
const char*_tmpCB6;void*_tmpCB5;(_tmpCB5=0,Cyc_Tcutil_terr(_tmp4E7->loc,((_tmpCB6="cannot determine value of constant",
_tag_dyneither(_tmpCB6,sizeof(char),35))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6D3;++ i){struct _tuple15*_tmpCB9;struct Cyc_List_List*
_tmpCB8;es=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->hd=((_tmpCB9=
_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->f1=0,((_tmpCB9->f2=_tmp4E8,_tmpCB9)))))),((
_tmpCB8->tl=es,_tmpCB8))))));}}if(_tmp4E9){struct Cyc_Absyn_Exp*_tmp6DA=Cyc_Toc_cast_it(
_tmp6D5,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCBC;struct Cyc_List_List*
_tmpCBB;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->hd=((
_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC->f1=0,((_tmpCBC->f2=_tmp6DA,
_tmpCBC)))))),((_tmpCBB->tl=0,_tmpCBB)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL208;};};}_LL247: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4EA=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp482;if(_tmp4EA->tag != 29)goto _LL249;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL248: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EC
!= 0,0,0,_tmp4ED,_tmp4EB))->r;else{if(_tmp4EE == 0){const char*_tmpCBF;void*
_tmpCBE;(_tmpCBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCBF="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCBF,sizeof(
char),38))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4EE;struct Cyc_Toc_Env _tmp6E0;struct
_RegionHandle*_tmp6E1;struct Cyc_Toc_Env*_tmp6DF=nv;_tmp6E0=*_tmp6DF;_tmp6E1=
_tmp6E0.rgn;{struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E1,e->loc,_tmp4ED,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EE->impl))->tagged){struct
_tuple20 _tmp6E4;struct Cyc_Absyn_Aggrfield*_tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;
struct _tuple20*_tmp6E3=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E2))->hd;_tmp6E4=*_tmp6E3;_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{void*
_tmp6E7=_tmp6E5->type;Cyc_Toc_exp_to_c(nv,_tmp6E6);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E7))_tmp6E6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E6->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4EE,_tmp6E5->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCC4;struct _tuple15*_tmpCC3;struct _tuple15*_tmpCC2[2];
struct Cyc_List_List*_tmp6E8=(_tmpCC2[1]=((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((
_tmpCC3->f1=0,((_tmpCC3->f2=_tmp6E6,_tmpCC3)))))),((_tmpCC2[0]=((_tmpCC4=
_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=0,((_tmpCC4->f2=field_tag_exp,_tmpCC4)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC2,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6E8,0);struct Cyc_Absyn_FieldName_struct*_tmpCCA;struct Cyc_Absyn_FieldName_struct
_tmpCC9;void*_tmpCC8[1];struct Cyc_List_List*ds=(_tmpCC8[0]=(void*)((_tmpCCA=
_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCC9.tag=1,((_tmpCC9.f1=_tmp6E5->name,
_tmpCC9)))),_tmpCCA)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC8,sizeof(void*),1)));struct _tuple15*_tmpCCD;struct _tuple15*
_tmpCCC[1];struct Cyc_List_List*dles=(_tmpCCC[0]=((_tmpCCD=_cycalloc(sizeof(*
_tmpCCD)),((_tmpCCD->f1=ds,((_tmpCCD->f2=umem,_tmpCCD)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCC,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp6F1=0;struct Cyc_List_List*_tmp6F2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){struct Cyc_List_List*
_tmp6F3=_tmp6E2;for(0;_tmp6F3 != 0;_tmp6F3=_tmp6F3->tl){if((*((struct _tuple20*)
_tmp6F3->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F2->hd){struct _tuple20
_tmp6F5;struct Cyc_Absyn_Aggrfield*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F7;struct
_tuple20*_tmp6F4=(struct _tuple20*)_tmp6F3->hd;_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;
_tmp6F7=_tmp6F5.f2;{void*_tmp6F8=_tmp6F6->type;Cyc_Toc_exp_to_c(nv,_tmp6F7);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6F8))_tmp6F7->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F7->r,0));{struct _tuple15*_tmpCD0;struct Cyc_List_List*
_tmpCCF;_tmp6F1=((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->hd=((_tmpCD0=
_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->f1=0,((_tmpCD0->f2=_tmp6F7,_tmpCD0)))))),((
_tmpCCF->tl=_tmp6F1,_tmpCCF))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F1));}};};}
goto _LL208;_LL249: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp4EF=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp482;if(_tmp4EF->tag != 30)goto _LL24B;else{_tmp4F0=(void*)_tmp4EF->f1;_tmp4F1=
_tmp4EF->f2;}}_LL24A: {struct Cyc_List_List*fs;{void*_tmp6FB=Cyc_Tcutil_compress(
_tmp4F0);struct Cyc_List_List*_tmp6FD;_LL31E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6FC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6FB;if(_tmp6FC->tag != 13)goto
_LL320;else{_tmp6FD=_tmp6FC->f2;}}_LL31F: fs=_tmp6FD;goto _LL31D;_LL320:;_LL321: {
const char*_tmpCD4;void*_tmpCD3[1];struct Cyc_String_pa_struct _tmpCD2;(_tmpCD2.tag=
0,((_tmpCD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4F0)),((_tmpCD3[0]=& _tmpCD2,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD4="anon struct has type %s",
_tag_dyneither(_tmpCD4,sizeof(char),24))),_tag_dyneither(_tmpCD3,sizeof(void*),1)))))));}
_LL31D:;}{struct Cyc_Toc_Env _tmp702;struct _RegionHandle*_tmp703;struct Cyc_Toc_Env*
_tmp701=nv;_tmp702=*_tmp701;_tmp703=_tmp702.rgn;{struct Cyc_List_List*_tmp704=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp703,e->loc,_tmp4F1,Cyc_Absyn_StructA,fs);for(0;_tmp704 != 0;_tmp704=_tmp704->tl){
struct _tuple20 _tmp706;struct Cyc_Absyn_Aggrfield*_tmp707;struct Cyc_Absyn_Exp*
_tmp708;struct _tuple20*_tmp705=(struct _tuple20*)_tmp704->hd;_tmp706=*_tmp705;
_tmp707=_tmp706.f1;_tmp708=_tmp706.f2;{void*_tmp709=_tmp707->type;Cyc_Toc_exp_to_c(
nv,_tmp708);if(Cyc_Toc_is_void_star_or_tvar(_tmp709))_tmp708->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp708->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp4F1);}goto _LL208;};}_LL24B: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F2=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp482;if(_tmp4F2->tag != 31)goto _LL24D;else{
_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;}}_LL24C: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp70A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Absyn_var_exp(_tmp70A,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCD5;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4F5->name,((_tmpCD5="_struct",_tag_dyneither(_tmpCD5,sizeof(char),8)))));}
tag_exp=_tmp4F4->is_extensible?Cyc_Absyn_var_exp(_tmp4F5->name,0): Cyc_Toc_datatype_tag(
_tmp4F4,_tmp4F5->name);member_exp=_tmp70B;{struct Cyc_List_List*_tmp70D=_tmp4F5->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F3 != 0;(
_tmp4F3=_tmp4F3->tl,_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCD8;struct
Cyc_List_List*_tmpCD7;dles=((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=((
_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->f1=0,((_tmpCD8->f2=cur_e,_tmpCD8)))))),((
_tmpCD7->tl=dles,_tmpCD7))))));};}{struct _tuple15*_tmpCDB;struct Cyc_List_List*
_tmpCDA;dles=((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->hd=((_tmpCDB=
_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->f1=0,((_tmpCDB->f2=tag_exp,_tmpCDB)))))),((
_tmpCDA->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCDA))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCDC;struct Cyc_List_List*_tmp712=(_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((
_tmpCDC->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCDC->tl=0,_tmpCDC)))));{int i=1;for(0;_tmp4F3 != 0;(((_tmp4F3=
_tmp4F3->tl,i ++)),_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp713=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCDD;_tmp712=((_tmpCDD=_cycalloc(sizeof(*
_tmpCDD)),((_tmpCDD->hd=_tmp713,((_tmpCDD->tl=_tmp712,_tmpCDD))))));};}}{struct
Cyc_Absyn_Stmt*_tmp715=Cyc_Absyn_exp_stmt(_tmp70B,0);struct Cyc_List_List*_tmpCDE;
struct Cyc_Absyn_Stmt*_tmp716=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE->hd=_tmp715,((_tmpCDE->tl=_tmp712,_tmpCDE))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp70A,datatype_ctype,0,_tmp716,0));};}goto _LL208;};}
_LL24D: {struct Cyc_Absyn_Enum_e_struct*_tmp4F6=(struct Cyc_Absyn_Enum_e_struct*)
_tmp482;if(_tmp4F6->tag != 32)goto _LL24F;}_LL24E: goto _LL250;_LL24F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp482;if(_tmp4F7->tag != 33)goto
_LL251;}_LL250: goto _LL208;_LL251: {struct Cyc_Absyn_Malloc_e_struct*_tmp4F8=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp482;if(_tmp4F8->tag != 34)goto _LL253;else{
_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.is_calloc;_tmp4FB=_tmp4F9.rgn;_tmp4FC=
_tmp4F9.elt_type;_tmp4FD=_tmp4F9.num_elts;_tmp4FE=_tmp4F9.fat_result;}}_LL252: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FC)));Cyc_Toc_exp_to_c(nv,
_tmp4FD);if(_tmp4FE){struct _tuple0*_tmp719=Cyc_Toc_temp_var();struct _tuple0*
_tmp71A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4FA){xexp=_tmp4FD;if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp719,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FC,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp719,0));}{struct Cyc_Absyn_Exp*_tmpCDF[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCDF[2]=Cyc_Absyn_var_exp(_tmp719,0),((_tmpCDF[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCDF[0]=Cyc_Absyn_var_exp(_tmp71A,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4FD,0);if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp719,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp4FC,Cyc_Absyn_var_exp(_tmp719,0));}{struct Cyc_Absyn_Exp*_tmpCE0[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE0[2]=Cyc_Absyn_var_exp(_tmp719,0),((_tmpCE0[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE0[0]=Cyc_Absyn_var_exp(_tmp71A,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_declare_stmt(_tmp719,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp71A,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71D);};}else{struct Cyc_Absyn_Exp*_tmp71E=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp71F=_tmp4FD->r;union Cyc_Absyn_Cnst _tmp721;struct _tuple5 _tmp722;
int _tmp723;_LL323: {struct Cyc_Absyn_Const_e_struct*_tmp720=(struct Cyc_Absyn_Const_e_struct*)
_tmp71F;if(_tmp720->tag != 0)goto _LL325;else{_tmp721=_tmp720->f1;if((_tmp721.Int_c).tag
!= 4)goto _LL325;_tmp722=(struct _tuple5)(_tmp721.Int_c).val;_tmp723=_tmp722.f2;
if(_tmp723 != 1)goto _LL325;}}_LL324: goto _LL322;_LL325:;_LL326: _tmp71E=Cyc_Absyn_times_exp(
_tmp71E,_tmp4FD,0);goto _LL322;_LL322:;}if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp71E))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FC,
_tmp71E))->r;}}goto _LL208;}_LL253: {struct Cyc_Absyn_Swap_e_struct*_tmp4FF=(
struct Cyc_Absyn_Swap_e_struct*)_tmp482;if(_tmp4FF->tag != 35)goto _LL255;else{
_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;}}_LL254: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCE3;void*_tmpCE2;(_tmpCE2=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE3="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCE3,sizeof(char),57))),_tag_dyneither(_tmpCE2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp500);Cyc_Toc_exp_to_c(
nv,_tmp501);{struct Cyc_Absyn_Exp*_tmpCE4[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCE4[1]=Cyc_Toc_push_address_exp(_tmp501),((_tmpCE4[0]=Cyc_Toc_push_address_exp(
_tmp500),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE4,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL208;};}
_LL255: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp502=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp482;if(_tmp502->tag != 38)goto _LL257;else{_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;}}
_LL256: {void*_tmp727=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp503->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp503);{void*_tmp728=
_tmp727;struct Cyc_Absyn_AggrInfo _tmp72A;union Cyc_Absyn_AggrInfoU _tmp72B;struct
Cyc_Absyn_Aggrdecl**_tmp72C;struct Cyc_Absyn_Aggrdecl*_tmp72D;_LL328: {struct Cyc_Absyn_AggrType_struct*
_tmp729=(struct Cyc_Absyn_AggrType_struct*)_tmp728;if(_tmp729->tag != 12)goto
_LL32A;else{_tmp72A=_tmp729->f1;_tmp72B=_tmp72A.aggr_info;if((_tmp72B.KnownAggr).tag
!= 2)goto _LL32A;_tmp72C=(struct Cyc_Absyn_Aggrdecl**)(_tmp72B.KnownAggr).val;
_tmp72D=*_tmp72C;}}_LL329: {struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72D,_tmp504),0);struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_aggrmember_exp(
_tmp503,_tmp504,0);struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_aggrmember_exp(_tmp72F,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp730,_tmp72E,0))->r;goto _LL327;}
_LL32A:;_LL32B: {const char*_tmpCE7;void*_tmpCE6;(_tmpCE6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE7="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCE7,sizeof(char),31))),_tag_dyneither(_tmpCE6,sizeof(void*),0)));}
_LL327:;}goto _LL208;}_LL257: {struct Cyc_Absyn_StmtExp_e_struct*_tmp505=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp482;if(_tmp505->tag != 37)goto _LL259;else{_tmp506=
_tmp505->f1;}}_LL258: Cyc_Toc_stmt_to_c(nv,_tmp506);goto _LL208;_LL259: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp507=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp482;if(_tmp507->tag != 36)
goto _LL25B;}_LL25A: {const char*_tmpCEA;void*_tmpCE9;(_tmpCE9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEA="UnresolvedMem",
_tag_dyneither(_tmpCEA,sizeof(char),14))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}
_LL25B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp508=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp482;if(_tmp508->tag != 26)goto _LL25D;}_LL25C: {const char*_tmpCED;void*_tmpCEC;(
_tmpCEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCED="compoundlit",_tag_dyneither(_tmpCED,sizeof(char),12))),_tag_dyneither(
_tmpCEC,sizeof(void*),0)));}_LL25D: {struct Cyc_Absyn_Valueof_e_struct*_tmp509=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp482;if(_tmp509->tag != 39)goto _LL208;}
_LL25E: {const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="valueof(-)",
_tag_dyneither(_tmpCF0,sizeof(char),11))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
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
struct Cyc_Absyn_Stmt*s;{void*_tmp739=p->r;struct Cyc_Absyn_Vardecl*_tmp73B;struct
Cyc_Absyn_Vardecl _tmp73C;struct _tuple0*_tmp73D;struct Cyc_Absyn_Pat*_tmp73E;
struct Cyc_Absyn_Vardecl*_tmp740;struct Cyc_Absyn_Vardecl _tmp741;struct _tuple0*
_tmp742;struct Cyc_Absyn_Vardecl*_tmp745;struct Cyc_Absyn_Pat*_tmp746;enum Cyc_Absyn_Sign
_tmp749;int _tmp74A;char _tmp74C;struct _dyneither_ptr _tmp74E;struct Cyc_Absyn_Enumdecl*
_tmp750;struct Cyc_Absyn_Enumfield*_tmp751;void*_tmp753;struct Cyc_Absyn_Enumfield*
_tmp754;struct Cyc_Absyn_Pat*_tmp756;struct Cyc_Absyn_Pat _tmp757;void*_tmp758;
struct Cyc_Absyn_Datatypedecl*_tmp75A;struct Cyc_Absyn_Datatypefield*_tmp75B;
struct Cyc_List_List*_tmp75C;struct Cyc_List_List*_tmp75E;struct Cyc_List_List*
_tmp760;struct Cyc_Absyn_AggrInfo*_tmp762;struct Cyc_Absyn_AggrInfo*_tmp764;struct
Cyc_Absyn_AggrInfo _tmp765;union Cyc_Absyn_AggrInfoU _tmp766;struct Cyc_List_List*
_tmp767;struct Cyc_Absyn_Pat*_tmp769;_LL32D: {struct Cyc_Absyn_Var_p_struct*
_tmp73A=(struct Cyc_Absyn_Var_p_struct*)_tmp739;if(_tmp73A->tag != 1)goto _LL32F;
else{_tmp73B=_tmp73A->f1;_tmp73C=*_tmp73B;_tmp73D=_tmp73C.name;_tmp73E=_tmp73A->f2;}}
_LL32E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73D,r),rgn,t,r,path,
_tmp73E,fail_stmt,decls);_LL32F: {struct Cyc_Absyn_TagInt_p_struct*_tmp73F=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp739;if(_tmp73F->tag != 3)goto _LL331;else{
_tmp740=_tmp73F->f2;_tmp741=*_tmp740;_tmp742=_tmp741.name;}}_LL330: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp742,r);goto _LL332;_LL331: {struct Cyc_Absyn_Wild_p_struct*_tmp743=(
struct Cyc_Absyn_Wild_p_struct*)_tmp739;if(_tmp743->tag != 0)goto _LL333;}_LL332: s=
Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL333: {struct Cyc_Absyn_Reference_p_struct*
_tmp744=(struct Cyc_Absyn_Reference_p_struct*)_tmp739;if(_tmp744->tag != 2)goto
_LL335;else{_tmp745=_tmp744->f1;_tmp746=_tmp744->f2;}}_LL334: {struct _tuple0*
_tmp76D=Cyc_Toc_temp_var();{struct _tuple22*_tmpCF3;struct Cyc_List_List*_tmpCF2;
decls=((_tmpCF2=_region_malloc(rgn,sizeof(*_tmpCF2)),((_tmpCF2->hd=((_tmpCF3=
_region_malloc(rgn,sizeof(*_tmpCF3)),((_tmpCF3->f1=_tmp76D,((_tmpCF3->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCF3)))))),((_tmpCF2->tl=decls,_tmpCF2))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp745->name,Cyc_Absyn_var_exp(_tmp76D,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp76D,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp770=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp746,fail_stmt,decls);_tmp770.f3=Cyc_Absyn_seq_stmt(s,_tmp770.f3,
0);return _tmp770;};}_LL335: {struct Cyc_Absyn_Null_p_struct*_tmp747=(struct Cyc_Absyn_Null_p_struct*)
_tmp739;if(_tmp747->tag != 8)goto _LL337;}_LL336: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32C;_LL337: {struct Cyc_Absyn_Int_p_struct*_tmp748=(struct
Cyc_Absyn_Int_p_struct*)_tmp739;if(_tmp748->tag != 9)goto _LL339;else{_tmp749=
_tmp748->f1;_tmp74A=_tmp748->f2;}}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp749,_tmp74A,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Char_p_struct*
_tmp74B=(struct Cyc_Absyn_Char_p_struct*)_tmp739;if(_tmp74B->tag != 10)goto _LL33B;
else{_tmp74C=_tmp74B->f1;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74C,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Float_p_struct*_tmp74D=(
struct Cyc_Absyn_Float_p_struct*)_tmp739;if(_tmp74D->tag != 11)goto _LL33D;else{
_tmp74E=_tmp74D->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp74E,
0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Enum_p_struct*_tmp74F=(struct
Cyc_Absyn_Enum_p_struct*)_tmp739;if(_tmp74F->tag != 12)goto _LL33F;else{_tmp750=
_tmp74F->f1;_tmp751=_tmp74F->f2;}}_LL33E:{struct Cyc_Absyn_Enum_e_struct _tmpCF6;
struct Cyc_Absyn_Enum_e_struct*_tmpCF5;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=32,((
_tmpCF6.f1=_tmp751->name,((_tmpCF6.f2=(struct Cyc_Absyn_Enumdecl*)_tmp750,((
_tmpCF6.f3=(struct Cyc_Absyn_Enumfield*)_tmp751,_tmpCF6)))))))),_tmpCF5)))),0),
fail_stmt);}goto _LL32C;_LL33F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp752=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp739;if(_tmp752->tag != 13)goto _LL341;else{
_tmp753=(void*)_tmp752->f1;_tmp754=_tmp752->f2;}}_LL340:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpCF9;struct Cyc_Absyn_AnonEnum_e_struct*_tmpCF8;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF9.tag=33,((
_tmpCF9.f1=_tmp754->name,((_tmpCF9.f2=(void*)_tmp753,((_tmpCF9.f3=(struct Cyc_Absyn_Enumfield*)
_tmp754,_tmpCF9)))))))),_tmpCF8)))),0),fail_stmt);}goto _LL32C;_LL341: {struct Cyc_Absyn_Pointer_p_struct*
_tmp755=(struct Cyc_Absyn_Pointer_p_struct*)_tmp739;if(_tmp755->tag != 5)goto
_LL343;else{_tmp756=_tmp755->f1;_tmp757=*_tmp756;_tmp758=_tmp757.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp759=(struct Cyc_Absyn_Datatype_p_struct*)_tmp758;if(_tmp759->tag != 7)goto
_LL343;else{_tmp75A=_tmp759->f1;_tmp75B=_tmp759->f2;_tmp75C=_tmp759->f3;}};}}
_LL342: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp775=Cyc_Toc_temp_var();const
char*_tmpCFA;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75B->name,((
_tmpCFA="_struct",_tag_dyneither(_tmpCFA,sizeof(char),8))));void*_tmp776=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp776,r);struct Cyc_List_List*_tmp777=_tmp75B->typs;for(0;_tmp75C != 0;(((
_tmp75C=_tmp75C->tl,_tmp777=((struct Cyc_List_List*)_check_null(_tmp777))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp778=(struct Cyc_Absyn_Pat*)_tmp75C->hd;if(_tmp778->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp779=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp777))->hd)).f2;struct _tuple0*_tmp77A=Cyc_Toc_temp_var();
void*_tmp77B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp778->topt))->v;void*
_tmp77C=Cyc_Toc_typ_to_c(_tmp77B);struct Cyc_Absyn_Exp*_tmp77D=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp775,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp779)))_tmp77D=Cyc_Toc_cast_it(_tmp77C,_tmp77D);{struct
_tuple22*_tmpCFD;struct Cyc_List_List*_tmpCFC;decls=((_tmpCFC=_region_malloc(rgn,
sizeof(*_tmpCFC)),((_tmpCFC->hd=((_tmpCFD=_region_malloc(rgn,sizeof(*_tmpCFD)),((
_tmpCFD->f1=_tmp77A,((_tmpCFD->f2=_tmp77C,_tmpCFD)))))),((_tmpCFC->tl=decls,
_tmpCFC))))));}{struct _tuple21 _tmp780=Cyc_Toc_xlate_pat(nv,rgn,_tmp77B,Cyc_Absyn_var_exp(
_tmp77A,0),_tmp77D,_tmp778,fail_stmt,decls);nv=_tmp780.f1;decls=_tmp780.f2;{
struct Cyc_Absyn_Stmt*_tmp781=_tmp780.f3;struct Cyc_Absyn_Stmt*_tmp782=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77A,0),_tmp77D,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp782,_tmp781,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp775,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp75A->is_extensible?Cyc_Absyn_var_exp(_tmp75B->name,0): Cyc_Toc_datatype_tag(
_tmp75A,_tmp75B->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp775,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp775,_tmp776,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL32C;};};_LL343: {struct Cyc_Absyn_Datatype_p_struct*_tmp75D=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp739;if(_tmp75D->tag != 7)goto _LL345;else{
_tmp75E=_tmp75D->f3;}}_LL344: _tmp760=_tmp75E;goto _LL346;_LL345: {struct Cyc_Absyn_Tuple_p_struct*
_tmp75F=(struct Cyc_Absyn_Tuple_p_struct*)_tmp739;if(_tmp75F->tag != 4)goto _LL347;
else{_tmp760=_tmp75F->f1;}}_LL346: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp760 != 0;(_tmp760=_tmp760->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp784=(struct Cyc_Absyn_Pat*)
_tmp760->hd;if(_tmp784->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp785=Cyc_Toc_temp_var();void*_tmp786=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp784->topt))->v;{struct _tuple22*_tmpD00;struct Cyc_List_List*_tmpCFF;decls=((
_tmpCFF=_region_malloc(rgn,sizeof(*_tmpCFF)),((_tmpCFF->hd=((_tmpD00=
_region_malloc(rgn,sizeof(*_tmpD00)),((_tmpD00->f1=_tmp785,((_tmpD00->f2=Cyc_Toc_typ_to_c(
_tmp786),_tmpD00)))))),((_tmpCFF->tl=decls,_tmpCFF))))));}{struct _tuple21 _tmp789=
Cyc_Toc_xlate_pat(nv,rgn,_tmp786,Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp784,fail_stmt,decls);nv=_tmp789.f1;decls=
_tmp789.f2;{struct Cyc_Absyn_Stmt*_tmp78A=_tmp789.f3;struct Cyc_Absyn_Stmt*_tmp78B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp78B,
_tmp78A,0),0);};};};}goto _LL32C;};_LL347: {struct Cyc_Absyn_Aggr_p_struct*_tmp761=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp739;if(_tmp761->tag != 6)goto _LL349;else{
_tmp762=_tmp761->f1;if(_tmp762 != 0)goto _LL349;}}_LL348: {const char*_tmpD03;void*
_tmpD02;(_tmpD02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD03="unresolved aggregate pattern!",_tag_dyneither(_tmpD03,sizeof(char),30))),
_tag_dyneither(_tmpD02,sizeof(void*),0)));}_LL349: {struct Cyc_Absyn_Aggr_p_struct*
_tmp763=(struct Cyc_Absyn_Aggr_p_struct*)_tmp739;if(_tmp763->tag != 6)goto _LL34B;
else{_tmp764=_tmp763->f1;if(_tmp764 == 0)goto _LL34B;_tmp765=*_tmp764;_tmp766=
_tmp765.aggr_info;_tmp767=_tmp763->f3;}}_LL34A: {struct Cyc_Absyn_Aggrdecl*
_tmp78E=Cyc_Absyn_get_known_aggrdecl(_tmp766);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp78E->impl))->tagged){struct _tuple23 _tmp790;struct Cyc_List_List*
_tmp791;struct Cyc_Absyn_Pat*_tmp792;struct _tuple23*_tmp78F=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp767))->hd;_tmp790=*_tmp78F;_tmp791=_tmp790.f1;
_tmp792=_tmp790.f2;{struct _dyneither_ptr*f;{void*_tmp793=(void*)((struct Cyc_List_List*)
_check_null(_tmp791))->hd;struct _dyneither_ptr*_tmp795;_LL354: {struct Cyc_Absyn_FieldName_struct*
_tmp794=(struct Cyc_Absyn_FieldName_struct*)_tmp793;if(_tmp794->tag != 1)goto
_LL356;else{_tmp795=_tmp794->f1;}}_LL355: f=_tmp795;goto _LL353;_LL356:;_LL357: {
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD06="no field name in tagged union pattern",
_tag_dyneither(_tmpD06,sizeof(char),38))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
_LL353:;}{struct _tuple0*_tmp798=Cyc_Toc_temp_var();void*_tmp799=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp792->topt))->v;void*_tmp79A=Cyc_Toc_typ_to_c(
_tmp799);{struct _tuple22*_tmpD09;struct Cyc_List_List*_tmpD08;decls=((_tmpD08=
_region_malloc(rgn,sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=_region_malloc(rgn,
sizeof(*_tmpD09)),((_tmpD09->f1=_tmp798,((_tmpD09->f2=_tmp79A,_tmpD09)))))),((
_tmpD08->tl=decls,_tmpD08))))));}{struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp79E=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp79E=Cyc_Toc_cast_it(_tmp79A,_tmp79E);_tmp79D=Cyc_Toc_cast_it(_tmp79A,_tmp79D);{
struct _tuple21 _tmp79F=Cyc_Toc_xlate_pat(nv,rgn,_tmp799,Cyc_Absyn_var_exp(_tmp798,
0),_tmp79D,_tmp792,fail_stmt,decls);nv=_tmp79F.f1;decls=_tmp79F.f2;{struct Cyc_Absyn_Stmt*
_tmp7A0=_tmp79F.f3;struct Cyc_Absyn_Stmt*_tmp7A1=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp78E,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp798,0),_tmp79E,0);s=Cyc_Absyn_seq_stmt(_tmp7A1,Cyc_Absyn_seq_stmt(_tmp7A2,
_tmp7A0,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp767 != 0;_tmp767=
_tmp767->tl){struct _tuple23*_tmp7A3=(struct _tuple23*)_tmp767->hd;struct Cyc_Absyn_Pat*
_tmp7A4=(*_tmp7A3).f2;if(_tmp7A4->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7A5=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A3).f1))->hd;struct _dyneither_ptr*_tmp7A7;_LL359: {struct Cyc_Absyn_FieldName_struct*
_tmp7A6=(struct Cyc_Absyn_FieldName_struct*)_tmp7A5;if(_tmp7A6->tag != 1)goto
_LL35B;else{_tmp7A7=_tmp7A6->f1;}}_LL35A: f=_tmp7A7;goto _LL358;_LL35B:;_LL35C: {
struct Cyc_Toc_Match_error_struct _tmpD0C;struct Cyc_Toc_Match_error_struct*_tmpD0B;(
int)_throw((void*)((_tmpD0B=_cycalloc_atomic(sizeof(*_tmpD0B)),((_tmpD0B[0]=((
_tmpD0C.tag=Cyc_Toc_Match_error,_tmpD0C)),_tmpD0B)))));}_LL358:;}{struct _tuple0*
_tmp7AA=Cyc_Toc_temp_var();void*_tmp7AB=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A4->topt))->v;void*_tmp7AC=Cyc_Toc_typ_to_c(_tmp7AB);{struct _tuple22*
_tmpD0F;struct Cyc_List_List*_tmpD0E;decls=((_tmpD0E=_region_malloc(rgn,sizeof(*
_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=_region_malloc(rgn,sizeof(*_tmpD0F)),((_tmpD0F->f1=
_tmp7AA,((_tmpD0F->f2=_tmp7AC,_tmpD0F)))))),((_tmpD0E->tl=decls,_tmpD0E))))));}{
struct _tuple21 _tmp7AF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AB,Cyc_Absyn_var_exp(_tmp7AA,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7A4,fail_stmt,decls);nv=_tmp7AF.f1;
decls=_tmp7AF.f2;{struct Cyc_Absyn_Exp*_tmp7B0=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp78E->impl))->fields,f)))->type))
_tmp7B0=Cyc_Toc_cast_it(_tmp7AC,_tmp7B0);{struct Cyc_Absyn_Stmt*_tmp7B1=_tmp7AF.f3;
struct Cyc_Absyn_Stmt*_tmp7B2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AA,0),
_tmp7B0,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7B2,_tmp7B1,0),0);};};};};};}}
goto _LL32C;}_LL34B: {struct Cyc_Absyn_Pointer_p_struct*_tmp768=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp739;if(_tmp768->tag != 5)goto _LL34D;else{_tmp769=_tmp768->f1;}}_LL34C: {
struct _tuple0*_tmp7B3=Cyc_Toc_temp_var();void*_tmp7B4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp769->topt))->v;{struct _tuple22*_tmpD12;struct Cyc_List_List*
_tmpD11;decls=((_tmpD11=_region_malloc(rgn,sizeof(*_tmpD11)),((_tmpD11->hd=((
_tmpD12=_region_malloc(rgn,sizeof(*_tmpD12)),((_tmpD12->f1=_tmp7B3,((_tmpD12->f2=
Cyc_Toc_typ_to_c(_tmp7B4),_tmpD12)))))),((_tmpD11->tl=decls,_tmpD11))))));}{
struct _tuple21 _tmp7B7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B4,Cyc_Absyn_var_exp(_tmp7B3,
0),Cyc_Absyn_deref_exp(path,0),_tmp769,fail_stmt,decls);nv=_tmp7B7.f1;decls=
_tmp7B7.f2;{struct Cyc_Absyn_Stmt*_tmp7B8=_tmp7B7.f3;struct Cyc_Absyn_Stmt*_tmp7B9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B3,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7B8,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7B9,0);else{s=_tmp7B9;}goto _LL32C;};};}_LL34D: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp76A=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp739;if(_tmp76A->tag != 14)goto
_LL34F;}_LL34E: {const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="unknownid pat",
_tag_dyneither(_tmpD15,sizeof(char),14))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}
_LL34F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76B=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp739;if(_tmp76B->tag != 15)goto _LL351;}_LL350: {const char*_tmpD18;void*_tmpD17;(
_tmpD17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD18="unknowncall pat",_tag_dyneither(_tmpD18,sizeof(char),16))),
_tag_dyneither(_tmpD17,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Exp_p_struct*
_tmp76C=(struct Cyc_Absyn_Exp_p_struct*)_tmp739;if(_tmp76C->tag != 16)goto _LL32C;}
_LL352: {const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1B="exp pat",
_tag_dyneither(_tmpD1B,sizeof(char),8))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}
_LL32C:;}{struct _tuple21 _tmpD1C;return(_tmpD1C.f1=nv,((_tmpD1C.f2=decls,((
_tmpD1C.f3=s,_tmpD1C)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD1D;
return(_tmpD1D=_region_malloc(r,sizeof(*_tmpD1D)),((_tmpD1D->f1=Cyc_Toc_fresh_label(),((
_tmpD1D->f2=Cyc_Toc_fresh_label(),((_tmpD1D->f3=sc,_tmpD1D)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7C3=Cyc_Tcutil_compress(_tmp7C2);_LL35E: {struct Cyc_Absyn_IntType_struct*
_tmp7C4=(struct Cyc_Absyn_IntType_struct*)_tmp7C3;if(_tmp7C4->tag != 6)goto _LL360;}
_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_EnumType_struct*_tmp7C5=(struct Cyc_Absyn_EnumType_struct*)
_tmp7C3;if(_tmp7C5->tag != 14)goto _LL362;}_LL361: leave_as_switch=1;goto _LL35D;
_LL362:;_LL363: leave_as_switch=0;goto _LL35D;_LL35D:;}{struct Cyc_List_List*
_tmp7C6=scs;for(0;_tmp7C6 != 0;_tmp7C6=_tmp7C6->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7C7=scs;for(0;_tmp7C7 != 0;_tmp7C7=_tmp7C7->tl){struct Cyc_Absyn_Stmt*
_tmp7C8=((struct Cyc_Absyn_Switch_clause*)_tmp7C7->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C7->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7C8,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7CA;struct _RegionHandle*_tmp7CB;struct Cyc_Toc_Env*_tmp7C9=
nv;_tmp7CA=*_tmp7C9;_tmp7CB=_tmp7CA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CB,nv,next_l),_tmp7C8);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CD;
struct _RegionHandle*_tmp7CE;struct Cyc_Toc_Env*_tmp7CC=nv;_tmp7CD=*_tmp7CC;
_tmp7CE=_tmp7CD.rgn;{struct Cyc_Toc_Env*_tmp7CF=Cyc_Toc_share_env(_tmp7CE,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7CE,Cyc_Toc_gen_label,
_tmp7CE,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D0=lscs;for(0;_tmp7D0 != 0;
_tmp7D0=_tmp7D0->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D0->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D0->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D0->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D2;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_Stmt*_tmp7D4;struct _tuple21
_tmp7D1=Cyc_Toc_xlate_pat(_tmp7CF,_tmp7CE,_tmp7C2,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D2=_tmp7D1.f1;_tmp7D3=_tmp7D1.f2;_tmp7D4=_tmp7D1.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D5=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D2,_tmp7D5);_tmp7D4=Cyc_Absyn_seq_stmt(
_tmp7D4,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D5,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D3;{
struct Cyc_List_List*_tmpD1E;nvs=((_tmpD1E=_region_malloc(_tmp7CE,sizeof(*_tmpD1E)),((
_tmpD1E->hd=_tmp7D2,((_tmpD1E->tl=nvs,_tmpD1E))))));}{struct Cyc_List_List*
_tmpD1F;test_stmts=((_tmpD1F=_region_malloc(_tmp7CE,sizeof(*_tmpD1F)),((_tmpD1F->hd=
_tmp7D4,((_tmpD1F->tl=test_stmts,_tmpD1F))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7D9;struct _dyneither_ptr*_tmp7DA;struct _dyneither_ptr*_tmp7DB;
struct Cyc_Absyn_Switch_clause*_tmp7DC;struct _tuple24*_tmp7D8=(struct _tuple24*)
lscs->hd;_tmp7D9=*_tmp7D8;_tmp7DA=_tmp7D9.f1;_tmp7DB=_tmp7D9.f2;_tmp7DC=_tmp7D9.f3;{
struct Cyc_Toc_Env*_tmp7DD=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DC->body;struct Cyc_Toc_Env _tmp7DF;struct
_RegionHandle*_tmp7E0;struct Cyc_Toc_Env*_tmp7DE=_tmp7CF;_tmp7DF=*_tmp7DE;_tmp7E0=
_tmp7DF.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7E2;struct _dyneither_ptr*_tmp7E3;
struct Cyc_Absyn_Switch_clause*_tmp7E4;struct _tuple24*_tmp7E1=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7E2=*_tmp7E1;_tmp7E3=_tmp7E2.f2;
_tmp7E4=_tmp7E2.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7E0,
_tmp7DD,end_l,_tmp7E3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7E4->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7E0,
_tmp7DD,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7DA,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DB,s,0),0);{struct Cyc_List_List*_tmpD20;stmts=((_tmpD20=_region_malloc(
_tmp7CE,sizeof(*_tmpD20)),((_tmpD20->hd=s,((_tmpD20->tl=stmts,_tmpD20))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7E7;struct _tuple0*_tmp7E8;void*_tmp7E9;struct _tuple22*_tmp7E6=(struct
_tuple22*)decls->hd;_tmp7E7=*_tmp7E6;_tmp7E8=_tmp7E7.f1;_tmp7E9=_tmp7E7.f2;res=
Cyc_Absyn_declare_stmt(_tmp7E8,_tmp7E9,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD21;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((
_tmpD21->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD21->tl=0,_tmpD21)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7EB=s->r;struct Cyc_Absyn_Exp*_tmp7EE;struct
Cyc_Absyn_Stmt*_tmp7F0;struct Cyc_Absyn_Stmt*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F3;
struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Stmt*_tmp7F6;struct Cyc_Absyn_Stmt*
_tmp7F7;struct _tuple8 _tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Stmt*
_tmp7FB;struct Cyc_Absyn_Stmt*_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FF;struct Cyc_Absyn_Stmt*
_tmp801;struct Cyc_Absyn_Exp*_tmp803;struct _tuple8 _tmp804;struct Cyc_Absyn_Exp*
_tmp805;struct _tuple8 _tmp806;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*
_tmp808;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_List_List*_tmp80B;struct Cyc_List_List*
_tmp80D;struct Cyc_Absyn_Switch_clause**_tmp80E;struct Cyc_Absyn_Decl*_tmp810;
struct Cyc_Absyn_Stmt*_tmp811;struct _dyneither_ptr*_tmp813;struct Cyc_Absyn_Stmt*
_tmp814;struct Cyc_Absyn_Stmt*_tmp816;struct _tuple8 _tmp817;struct Cyc_Absyn_Exp*
_tmp818;struct Cyc_Absyn_Stmt*_tmp81A;struct Cyc_List_List*_tmp81B;struct Cyc_Absyn_Exp*
_tmp81D;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp7EC=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7EB;if(_tmp7EC->tag != 0)goto _LL367;}_LL366: return;_LL367: {struct Cyc_Absyn_Exp_s_struct*
_tmp7ED=(struct Cyc_Absyn_Exp_s_struct*)_tmp7EB;if(_tmp7ED->tag != 1)goto _LL369;
else{_tmp7EE=_tmp7ED->f1;}}_LL368: Cyc_Toc_exp_to_c(nv,_tmp7EE);return;_LL369: {
struct Cyc_Absyn_Seq_s_struct*_tmp7EF=(struct Cyc_Absyn_Seq_s_struct*)_tmp7EB;if(
_tmp7EF->tag != 2)goto _LL36B;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7EF->f2;}}
_LL36A: Cyc_Toc_stmt_to_c(nv,_tmp7F0);s=_tmp7F1;continue;_LL36B: {struct Cyc_Absyn_Return_s_struct*
_tmp7F2=(struct Cyc_Absyn_Return_s_struct*)_tmp7EB;if(_tmp7F2->tag != 3)goto _LL36D;
else{_tmp7F3=_tmp7F2->f1;}}_LL36C: {struct Cyc_Core_Opt*topt=0;if(_tmp7F3 != 0){{
struct Cyc_Core_Opt*_tmpD22;topt=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F3->topt))->v),
_tmpD22))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7F3);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp81F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp820=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp81F,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp81F,(void*)topt->v,_tmp7F3,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp820,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F4=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7EB;if(_tmp7F4->tag != 4)goto _LL36F;
else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F4->f2;_tmp7F7=_tmp7F4->f3;}}_LL36E: Cyc_Toc_exp_to_c(
nv,_tmp7F5);Cyc_Toc_stmt_to_c(nv,_tmp7F6);s=_tmp7F7;continue;_LL36F: {struct Cyc_Absyn_While_s_struct*
_tmp7F8=(struct Cyc_Absyn_While_s_struct*)_tmp7EB;if(_tmp7F8->tag != 5)goto _LL371;
else{_tmp7F9=_tmp7F8->f1;_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F8->f2;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7FA);{struct Cyc_Toc_Env _tmp822;struct _RegionHandle*_tmp823;struct Cyc_Toc_Env*
_tmp821=nv;_tmp822=*_tmp821;_tmp823=_tmp822.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp823,nv),_tmp7FB);return;};_LL371: {struct Cyc_Absyn_Break_s_struct*_tmp7FC=(
struct Cyc_Absyn_Break_s_struct*)_tmp7EB;if(_tmp7FC->tag != 6)goto _LL373;else{
_tmp7FD=_tmp7FC->f1;}}_LL372: {struct Cyc_Toc_Env _tmp825;struct _dyneither_ptr**
_tmp826;struct Cyc_Toc_Env*_tmp824=nv;_tmp825=*_tmp824;_tmp826=_tmp825.break_lab;
if(_tmp826 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp826,0);{int dest_depth=_tmp7FD == 0?0:
_tmp7FD->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL373: {struct Cyc_Absyn_Continue_s_struct*_tmp7FE=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7EB;if(_tmp7FE->tag != 7)goto _LL375;else{_tmp7FF=_tmp7FE->f1;}}_LL374: {
struct Cyc_Toc_Env _tmp828;struct _dyneither_ptr**_tmp829;struct Cyc_Toc_Env*_tmp827=
nv;_tmp828=*_tmp827;_tmp829=_tmp828.continue_lab;if(_tmp829 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp829,0);_tmp801=_tmp7FF;goto _LL376;}_LL375: {struct Cyc_Absyn_Goto_s_struct*
_tmp800=(struct Cyc_Absyn_Goto_s_struct*)_tmp7EB;if(_tmp800->tag != 8)goto _LL377;
else{_tmp801=_tmp800->f2;}}_LL376: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp801))->try_depth,s);return;_LL377: {struct Cyc_Absyn_For_s_struct*
_tmp802=(struct Cyc_Absyn_For_s_struct*)_tmp7EB;if(_tmp802->tag != 9)goto _LL379;
else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f2;_tmp805=_tmp804.f1;_tmp806=_tmp802->f3;
_tmp807=_tmp806.f1;_tmp808=_tmp802->f4;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp803);Cyc_Toc_exp_to_c(
nv,_tmp805);Cyc_Toc_exp_to_c(nv,_tmp807);{struct Cyc_Toc_Env _tmp82B;struct
_RegionHandle*_tmp82C;struct Cyc_Toc_Env*_tmp82A=nv;_tmp82B=*_tmp82A;_tmp82C=
_tmp82B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82C,nv),_tmp808);return;};
_LL379: {struct Cyc_Absyn_Switch_s_struct*_tmp809=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7EB;if(_tmp809->tag != 10)goto _LL37B;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;}}
_LL37A: Cyc_Toc_xlate_switch(nv,s,_tmp80A,_tmp80B);return;_LL37B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7EB;if(_tmp80C->tag != 11)goto
_LL37D;else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80C->f2;}}_LL37C: {struct Cyc_Toc_Env
_tmp82E;struct Cyc_Toc_FallthruInfo*_tmp82F;struct Cyc_Toc_Env*_tmp82D=nv;_tmp82E=*
_tmp82D;_tmp82F=_tmp82E.fallthru_info;if(_tmp82F == 0){const char*_tmpD25;void*
_tmpD24;(_tmpD24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD25="fallthru in unexpected place",_tag_dyneither(_tmpD25,sizeof(char),29))),
_tag_dyneither(_tmpD24,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp833;struct
Cyc_List_List*_tmp834;struct Cyc_Dict_Dict _tmp835;struct Cyc_Toc_FallthruInfo
_tmp832=*_tmp82F;_tmp833=_tmp832.label;_tmp834=_tmp832.binders;_tmp835=_tmp832.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp833,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp80E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp834);struct Cyc_List_List*_tmp837=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80D);for(0;_tmp836 != 0;(_tmp836=_tmp836->tl,
_tmp837=_tmp837->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp837))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp835,(
struct _tuple0*)_tmp836->hd),(struct Cyc_Absyn_Exp*)_tmp837->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37D: {struct Cyc_Absyn_Decl_s_struct*_tmp80F=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7EB;if(_tmp80F->tag != 12)goto _LL37F;else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;}}
_LL37E:{void*_tmp838=_tmp810->r;struct Cyc_Absyn_Vardecl*_tmp83A;struct Cyc_Absyn_Pat*
_tmp83C;struct Cyc_Absyn_Exp*_tmp83D;struct Cyc_List_List*_tmp83F;struct Cyc_Absyn_Fndecl*
_tmp841;struct Cyc_Absyn_Tvar*_tmp843;struct Cyc_Absyn_Vardecl*_tmp844;int _tmp845;
struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Tvar*
_tmp849;struct Cyc_Absyn_Vardecl*_tmp84A;_LL388: {struct Cyc_Absyn_Var_d_struct*
_tmp839=(struct Cyc_Absyn_Var_d_struct*)_tmp838;if(_tmp839->tag != 0)goto _LL38A;
else{_tmp83A=_tmp839->f1;}}_LL389: {struct Cyc_Toc_Env _tmp84C;struct _RegionHandle*
_tmp84D;struct Cyc_Toc_Env*_tmp84B=nv;_tmp84C=*_tmp84B;_tmp84D=_tmp84C.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD28;struct Cyc_Absyn_Local_b_struct*_tmpD27;
struct Cyc_Toc_Env*_tmp84E=Cyc_Toc_add_varmap(_tmp84D,nv,_tmp83A->name,Cyc_Absyn_varb_exp(
_tmp83A->name,(void*)((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=((
_tmpD28.tag=4,((_tmpD28.f1=_tmp83A,_tmpD28)))),_tmpD27)))),0));Cyc_Toc_local_decl_to_c(
_tmp84E,_tmp84E,_tmp83A,_tmp811);}goto _LL387;}_LL38A: {struct Cyc_Absyn_Let_d_struct*
_tmp83B=(struct Cyc_Absyn_Let_d_struct*)_tmp838;if(_tmp83B->tag != 2)goto _LL38C;
else{_tmp83C=_tmp83B->f1;_tmp83D=_tmp83B->f3;}}_LL38B:{void*_tmp851=_tmp83C->r;
struct Cyc_Absyn_Vardecl*_tmp853;struct Cyc_Absyn_Pat*_tmp854;struct Cyc_Absyn_Pat
_tmp855;void*_tmp856;_LL397: {struct Cyc_Absyn_Var_p_struct*_tmp852=(struct Cyc_Absyn_Var_p_struct*)
_tmp851;if(_tmp852->tag != 1)goto _LL399;else{_tmp853=_tmp852->f1;_tmp854=_tmp852->f2;
_tmp855=*_tmp854;_tmp856=_tmp855.r;{struct Cyc_Absyn_Wild_p_struct*_tmp857=(
struct Cyc_Absyn_Wild_p_struct*)_tmp856;if(_tmp857->tag != 0)goto _LL399;};}}_LL398: {
struct _tuple0*old_name=_tmp853->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp853->name=new_name;_tmp853->initializer=(struct Cyc_Absyn_Exp*)_tmp83D;{
struct Cyc_Absyn_Var_d_struct _tmpD2B;struct Cyc_Absyn_Var_d_struct*_tmpD2A;_tmp810->r=(
void*)((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A[0]=((_tmpD2B.tag=0,((
_tmpD2B.f1=_tmp853,_tmpD2B)))),_tmpD2A))));}{struct Cyc_Toc_Env _tmp85B;struct
_RegionHandle*_tmp85C;struct Cyc_Toc_Env*_tmp85A=nv;_tmp85B=*_tmp85A;_tmp85C=
_tmp85B.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD2E;struct Cyc_Absyn_Local_b_struct*
_tmpD2D;struct Cyc_Toc_Env*_tmp85D=Cyc_Toc_add_varmap(_tmp85C,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=((_tmpD2E.tag=
4,((_tmpD2E.f1=_tmp853,_tmpD2E)))),_tmpD2D)))),0));Cyc_Toc_local_decl_to_c(
_tmp85D,nv,_tmp853,_tmp811);}goto _LL396;};}_LL399:;_LL39A: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp83C,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83D->topt))->v,_tmp83D,
_tmp811))->r;goto _LL396;_LL396:;}goto _LL387;_LL38C: {struct Cyc_Absyn_Letv_d_struct*
_tmp83E=(struct Cyc_Absyn_Letv_d_struct*)_tmp838;if(_tmp83E->tag != 3)goto _LL38E;
else{_tmp83F=_tmp83E->f1;}}_LL38D: {struct Cyc_List_List*_tmp860=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp83F);if(_tmp860 == 0){const char*_tmpD31;
void*_tmpD30;(_tmpD30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD31="empty Letv_d",_tag_dyneither(_tmpD31,sizeof(char),
13))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD34;struct Cyc_Absyn_Var_d_struct*_tmpD33;_tmp810->r=(void*)((_tmpD33=
_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=0,((_tmpD34.f1=(struct Cyc_Absyn_Vardecl*)
_tmp860->hd,_tmpD34)))),_tmpD33))));}_tmp860=_tmp860->tl;for(0;_tmp860 != 0;
_tmp860=_tmp860->tl){struct Cyc_Absyn_Var_d_struct _tmpD37;struct Cyc_Absyn_Var_d_struct*
_tmpD36;struct Cyc_Absyn_Decl*_tmp865=Cyc_Absyn_new_decl((void*)((_tmpD36=
_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=0,((_tmpD37.f1=(struct Cyc_Absyn_Vardecl*)
_tmp860->hd,_tmpD37)))),_tmpD36)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp865,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL387;}_LL38E: {struct Cyc_Absyn_Fn_d_struct*
_tmp840=(struct Cyc_Absyn_Fn_d_struct*)_tmp838;if(_tmp840->tag != 1)goto _LL390;
else{_tmp841=_tmp840->f1;}}_LL38F: {struct _tuple0*_tmp868=_tmp841->name;struct
Cyc_Toc_Env _tmp86A;struct _RegionHandle*_tmp86B;struct Cyc_Toc_Env*_tmp869=nv;
_tmp86A=*_tmp869;_tmp86B=_tmp86A.rgn;{struct Cyc_Toc_Env*_tmp86C=Cyc_Toc_add_varmap(
_tmp86B,nv,_tmp841->name,Cyc_Absyn_var_exp(_tmp868,0));Cyc_Toc_fndecl_to_c(
_tmp86C,_tmp841,0);Cyc_Toc_stmt_to_c(_tmp86C,_tmp811);}goto _LL387;}_LL390: {
struct Cyc_Absyn_Region_d_struct*_tmp842=(struct Cyc_Absyn_Region_d_struct*)
_tmp838;if(_tmp842->tag != 4)goto _LL392;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;
_tmp845=_tmp842->f3;_tmp846=_tmp842->f4;}}_LL391: {struct Cyc_Absyn_Stmt*_tmp86D=
_tmp811;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp844->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp86F;struct _RegionHandle*_tmp870;struct Cyc_Toc_Env*
_tmp86E=nv;_tmp86F=*_tmp86E;_tmp870=_tmp86F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp870,nv,x_var,x_exp),_tmp86D);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86D,0))->r;
else{if(_tmp846 == 0){struct Cyc_Absyn_Exp*_tmpD3A[1];struct Cyc_Absyn_Exp*_tmpD39[
1];struct Cyc_List_List*_tmpD38;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD38=
_cycalloc(sizeof(*_tmpD38)),((_tmpD38->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD38->tl=0,_tmpD38)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD39[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD39,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD3A[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3A,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp846);{struct Cyc_Absyn_Exp*
_tmpD3C[1];struct Cyc_Absyn_Exp*_tmpD3B[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD3B[1]=(struct Cyc_Absyn_Exp*)_tmp846,((_tmpD3B[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD3C[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3C,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL392: {struct Cyc_Absyn_Alias_d_struct*_tmp847=(struct Cyc_Absyn_Alias_d_struct*)
_tmp838;if(_tmp847->tag != 5)goto _LL394;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;
_tmp84A=_tmp847->f3;}}_LL393: {struct _tuple0*old_name=_tmp84A->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp84A->name=new_name;_tmp84A->initializer=(
struct Cyc_Absyn_Exp*)_tmp848;{struct Cyc_Absyn_Decl_s_struct _tmpD4B;struct Cyc_Absyn_Var_d_struct*
_tmpD4A;struct Cyc_Absyn_Var_d_struct _tmpD49;struct Cyc_Absyn_Decl*_tmpD48;struct
Cyc_Absyn_Decl_s_struct*_tmpD47;s->r=(void*)((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((
_tmpD47[0]=((_tmpD4B.tag=12,((_tmpD4B.f1=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((
_tmpD48->r=(void*)((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD49.tag=
0,((_tmpD49.f1=_tmp84A,_tmpD49)))),_tmpD4A)))),((_tmpD48->loc=0,_tmpD48)))))),((
_tmpD4B.f2=_tmp811,_tmpD4B)))))),_tmpD47))));}{struct Cyc_Toc_Env _tmp87C;struct
_RegionHandle*_tmp87D;struct Cyc_Toc_Env*_tmp87B=nv;_tmp87C=*_tmp87B;_tmp87D=
_tmp87C.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD4E;struct Cyc_Absyn_Local_b_struct*
_tmpD4D;struct Cyc_Toc_Env*_tmp87E=Cyc_Toc_add_varmap(_tmp87D,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4E.tag=
4,((_tmpD4E.f1=_tmp84A,_tmpD4E)))),_tmpD4D)))),0));Cyc_Toc_local_decl_to_c(
_tmp87E,nv,_tmp84A,_tmp811);}return;};}_LL394:;_LL395: {const char*_tmpD51;void*
_tmpD50;(_tmpD50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD51="bad nested declaration within function",_tag_dyneither(_tmpD51,sizeof(
char),39))),_tag_dyneither(_tmpD50,sizeof(void*),0)));}_LL387:;}return;_LL37F: {
struct Cyc_Absyn_Label_s_struct*_tmp812=(struct Cyc_Absyn_Label_s_struct*)_tmp7EB;
if(_tmp812->tag != 13)goto _LL381;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}
_LL380: s=_tmp814;continue;_LL381: {struct Cyc_Absyn_Do_s_struct*_tmp815=(struct
Cyc_Absyn_Do_s_struct*)_tmp7EB;if(_tmp815->tag != 14)goto _LL383;else{_tmp816=
_tmp815->f1;_tmp817=_tmp815->f2;_tmp818=_tmp817.f1;}}_LL382: {struct Cyc_Toc_Env
_tmp884;struct _RegionHandle*_tmp885;struct Cyc_Toc_Env*_tmp883=nv;_tmp884=*
_tmp883;_tmp885=_tmp884.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp885,nv),
_tmp816);Cyc_Toc_exp_to_c(nv,_tmp818);return;}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp819=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7EB;if(_tmp819->tag != 15)goto
_LL385;else{_tmp81A=_tmp819->f1;_tmp81B=_tmp819->f2;}}_LL384: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD52;
e_exp->topt=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->v=e_typ,_tmpD52))));}{
struct Cyc_Toc_Env _tmp888;struct _RegionHandle*_tmp889;struct Cyc_Toc_Env*_tmp887=
nv;_tmp888=*_tmp887;_tmp889=_tmp888.rgn;{struct Cyc_Toc_Env*_tmp88A=Cyc_Toc_add_varmap(
_tmp889,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp88A,_tmp81A);{struct Cyc_Absyn_Stmt*
_tmp88B=Cyc_Absyn_seq_stmt(_tmp81A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp88C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88D=
Cyc_Absyn_var_exp(_tmp88C,0);struct Cyc_Absyn_Vardecl*_tmp88E=Cyc_Absyn_new_vardecl(
_tmp88C,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD53;_tmp88D->topt=((
_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->v=Cyc_Absyn_exn_typ(),_tmpD53))));}{
struct Cyc_Core_Opt*_tmpD64;struct Cyc_Absyn_Var_p_struct*_tmpD63;struct Cyc_Absyn_Pat*
_tmpD62;struct Cyc_Core_Opt*_tmpD61;struct Cyc_Absyn_Var_p_struct _tmpD60;struct Cyc_Absyn_Pat*
_tmpD5F;struct Cyc_Absyn_Pat*_tmp890=(_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((
_tmpD5F->r=(void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD60.tag=
1,((_tmpD60.f1=_tmp88E,((_tmpD60.f2=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((
_tmpD62->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD62->topt=((_tmpD61=_cycalloc(
sizeof(*_tmpD61)),((_tmpD61->v=Cyc_Absyn_exn_typ(),_tmpD61)))),((_tmpD62->loc=0,
_tmpD62)))))))),_tmpD60)))))),_tmpD63)))),((_tmpD5F->topt=((_tmpD64=_cycalloc(
sizeof(*_tmpD64)),((_tmpD64->v=Cyc_Absyn_exn_typ(),_tmpD64)))),((_tmpD5F->loc=0,
_tmpD5F)))))));struct Cyc_Absyn_Exp*_tmp891=Cyc_Absyn_throw_exp(_tmp88D,0);{
struct Cyc_Core_Opt*_tmpD65;_tmp891->topt=((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((
_tmpD65->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD65))));}{struct Cyc_Absyn_Stmt*
_tmp893=Cyc_Absyn_exp_stmt(_tmp891,0);struct Cyc_Core_Opt*_tmpD6B;struct Cyc_List_List*
_tmpD6A;struct Cyc_Absyn_Switch_clause*_tmpD69;struct Cyc_Absyn_Switch_clause*
_tmp894=(_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->pattern=_tmp890,((
_tmpD69->pat_vars=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->v=((_tmpD6A=
_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=_tmp88E,((_tmpD6A->tl=0,_tmpD6A)))))),
_tmpD6B)))),((_tmpD69->where_clause=0,((_tmpD69->body=_tmp893,((_tmpD69->loc=0,
_tmpD69)))))))))));struct Cyc_List_List*_tmpD6C;struct Cyc_Absyn_Stmt*_tmp895=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp81B,((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->hd=_tmp894,((_tmpD6C->tl=
0,_tmpD6C))))))),0);Cyc_Toc_stmt_to_c(_tmp88A,_tmp895);{struct Cyc_List_List*
_tmpD6D;struct Cyc_Absyn_Exp*_tmp896=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD6D->tl=0,_tmpD6D)))))),0);struct Cyc_List_List*
_tmpD6E;struct Cyc_Absyn_Stmt*_tmp897=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD6E->tl=0,_tmpD6E)))))),0),0);struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp899=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp897,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp898,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp896,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp899,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88B,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp895,0),0),0),0),0),0))->r;};};};};}
return;};}_LL385: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81C=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7EB;if(_tmp81C->tag != 16)goto _LL364;else{_tmp81D=_tmp81C->f1;}}_LL386: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81D);{struct Cyc_List_List*
_tmpD6F;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->hd=_tmp81D,((_tmpD6F->tl=0,
_tmpD6F)))))),0));};}return;_LL364:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A7=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A7;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8A8=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8A9=f->args;for(0;_tmp8A9 != 0;_tmp8A9=_tmp8A9->tl){struct
_tuple0*_tmpD70;struct _tuple0*_tmp8AA=(_tmpD70=_cycalloc(sizeof(*_tmpD70)),((
_tmpD70->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD70->f2=(*((struct
_tuple25*)_tmp8A9->hd)).f1,_tmpD70)))));(*((struct _tuple25*)_tmp8A9->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8A9->hd)).f3);_tmp8A8=Cyc_Toc_add_varmap(frgn,_tmp8A8,
_tmp8AA,Cyc_Absyn_var_exp(_tmp8AA,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8AD;struct Cyc_Absyn_Tqual
_tmp8AE;void*_tmp8AF;int _tmp8B0;struct Cyc_Absyn_VarargInfo _tmp8AC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AD=_tmp8AC.name;_tmp8AE=_tmp8AC.tq;_tmp8AF=
_tmp8AC.type;_tmp8B0=_tmp8AC.inject;{void*_tmp8B1=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8AF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8AE,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD71;struct _tuple0*_tmp8B2=(_tmpD71=_cycalloc(sizeof(*_tmpD71)),((
_tmpD71->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD71->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8AD))->v,_tmpD71)))));{
struct _tuple25*_tmpD74;struct Cyc_List_List*_tmpD73;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD73=
_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((
_tmpD74->f1=(struct _dyneither_ptr*)_tmp8AD->v,((_tmpD74->f2=_tmp8AE,((_tmpD74->f3=
_tmp8B1,_tmpD74)))))))),((_tmpD73->tl=0,_tmpD73)))))));}_tmp8A8=Cyc_Toc_add_varmap(
frgn,_tmp8A8,_tmp8B2,Cyc_Absyn_var_exp(_tmp8B2,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){((struct Cyc_Absyn_Vardecl*)_tmp8B6->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B6->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8A8),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL39B: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39C: return Cyc_Absyn_Extern;
default: _LL39D: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8B7=ad->name;struct _DynRegionHandle*_tmp8B9;struct Cyc_Dict_Dict*_tmp8BA;
struct Cyc_Toc_TocState _tmp8B8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8B9=_tmp8B8.dyn;_tmp8BA=_tmp8B8.aggrs_so_far;{struct _DynRegionFrame _tmp8BB;
struct _RegionHandle*d=_open_dynregion(& _tmp8BB,_tmp8B9);{int seen_defn_before;
struct _tuple14**_tmp8BC=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8BA,_tmp8B7);if(_tmp8BC == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD75;v=((
_tmpD75=_region_malloc(d,sizeof(*_tmpD75)),((_tmpD75->f1=ad,((_tmpD75->f2=Cyc_Absyn_strctq(
_tmp8B7),_tmpD75))))));}else{struct _tuple14*_tmpD76;v=((_tmpD76=_region_malloc(d,
sizeof(*_tmpD76)),((_tmpD76->f1=ad,((_tmpD76->f2=Cyc_Absyn_unionq_typ(_tmp8B7),
_tmpD76))))));}*_tmp8BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8BA,_tmp8B7,v);};}else{struct
_tuple14 _tmp8C0;struct Cyc_Absyn_Aggrdecl*_tmp8C1;void*_tmp8C2;struct _tuple14*
_tmp8BF=*_tmp8BC;_tmp8C0=*_tmp8BF;_tmp8C1=_tmp8C0.f1;_tmp8C2=_tmp8C0.f2;if(
_tmp8C1->impl == 0){{struct _tuple14*_tmpD77;*_tmp8BA=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8BA,
_tmp8B7,((_tmpD77=_region_malloc(d,sizeof(*_tmpD77)),((_tmpD77->f1=ad,((_tmpD77->f2=
_tmp8C2,_tmpD77)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD78;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD78=_cycalloc(sizeof(*_tmpD78)),((
_tmpD78->kind=ad->kind,((_tmpD78->sc=Cyc_Absyn_Public,((_tmpD78->name=ad->name,((
_tmpD78->tvs=0,((_tmpD78->impl=0,((_tmpD78->attributes=ad->attributes,_tmpD78)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD79;
new_ad->impl=((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->exist_vars=0,((
_tmpD79->rgn_po=0,((_tmpD79->fields=0,((_tmpD79->tagged=0,_tmpD79))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8C5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){struct Cyc_Absyn_Aggrfield*
_tmp8C6=(struct Cyc_Absyn_Aggrfield*)_tmp8C5->hd;struct Cyc_Absyn_Aggrfield*
_tmpD7A;struct Cyc_Absyn_Aggrfield*_tmp8C7=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->name=_tmp8C6->name,((_tmpD7A->tq=Cyc_Toc_mt_tq,((_tmpD7A->type=Cyc_Toc_typ_to_c_array(
_tmp8C6->type),((_tmpD7A->width=_tmp8C6->width,((_tmpD7A->attributes=_tmp8C6->attributes,
_tmpD7A)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8C8=_tmp8C7->type;struct _dyneither_ptr*_tmp8C9=_tmp8C7->name;const char*
_tmpD7F;void*_tmpD7E[2];struct Cyc_String_pa_struct _tmpD7D;struct Cyc_String_pa_struct
_tmpD7C;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD7C.tag=0,((_tmpD7C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8C9),((_tmpD7D.tag=0,((_tmpD7D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD7E[0]=&
_tmpD7D,((_tmpD7E[1]=& _tmpD7C,Cyc_aprintf(((_tmpD7F="_union_%s_%s",
_tag_dyneither(_tmpD7F,sizeof(char),13))),_tag_dyneither(_tmpD7E,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD80;struct _dyneither_ptr*str=(_tmpD80=_cycalloc(sizeof(*
_tmpD80)),((_tmpD80[0]=s,_tmpD80)));struct Cyc_Absyn_Aggrfield*_tmpD81;struct Cyc_Absyn_Aggrfield*
_tmp8CA=(_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->name=Cyc_Toc_val_sp,((
_tmpD81->tq=Cyc_Toc_mt_tq,((_tmpD81->type=_tmp8C8,((_tmpD81->width=0,((_tmpD81->attributes=
0,_tmpD81)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD82;struct Cyc_Absyn_Aggrfield*
_tmp8CB=(_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->name=Cyc_Toc_tag_sp,((
_tmpD82->tq=Cyc_Toc_mt_tq,((_tmpD82->type=Cyc_Absyn_sint_typ,((_tmpD82->width=0,((
_tmpD82->attributes=0,_tmpD82)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD83[2];
struct Cyc_List_List*_tmp8CC=(_tmpD83[1]=_tmp8CA,((_tmpD83[0]=_tmp8CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD83,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD88;struct _tuple0*_tmpD87;struct Cyc_Absyn_Aggrdecl*
_tmpD86;struct Cyc_Absyn_Aggrdecl*_tmp8CD=(_tmpD86=_cycalloc(sizeof(*_tmpD86)),((
_tmpD86->kind=Cyc_Absyn_StructA,((_tmpD86->sc=Cyc_Absyn_Public,((_tmpD86->name=((
_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->f1=Cyc_Absyn_Loc_n,((_tmpD87->f2=
str,_tmpD87)))))),((_tmpD86->tvs=0,((_tmpD86->impl=((_tmpD88=_cycalloc(sizeof(*
_tmpD88)),((_tmpD88->exist_vars=0,((_tmpD88->rgn_po=0,((_tmpD88->fields=_tmp8CC,((
_tmpD88->tagged=0,_tmpD88)))))))))),((_tmpD86->attributes=0,_tmpD86)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD8E;struct Cyc_Absyn_Aggr_d_struct _tmpD8D;
struct Cyc_List_List*_tmpD8C;Cyc_Toc_result_decls=((_tmpD8C=_cycalloc(sizeof(*
_tmpD8C)),((_tmpD8C->hd=Cyc_Absyn_new_decl((void*)((_tmpD8E=_cycalloc(sizeof(*
_tmpD8E)),((_tmpD8E[0]=((_tmpD8D.tag=6,((_tmpD8D.f1=_tmp8CD,_tmpD8D)))),_tmpD8E)))),
0),((_tmpD8C->tl=Cyc_Toc_result_decls,_tmpD8C))))));}_tmp8C7->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD8F;new_fields=((_tmpD8F=_cycalloc(sizeof(*
_tmpD8F)),((_tmpD8F->hd=_tmp8C7,((_tmpD8F->tl=new_fields,_tmpD8F))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpD99;struct Cyc_Absyn_Aggr_d_struct
_tmpD98;struct Cyc_Absyn_Aggr_d_struct*_tmpD97;struct Cyc_List_List*_tmpD96;Cyc_Toc_result_decls=((
_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->hd=((_tmpD99=_cycalloc(sizeof(*
_tmpD99)),((_tmpD99->r=(void*)((_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97[0]=((
_tmpD98.tag=6,((_tmpD98.f1=new_ad,_tmpD98)))),_tmpD97)))),((_tmpD99->loc=0,
_tmpD99)))))),((_tmpD96->tl=Cyc_Toc_result_decls,_tmpD96))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8E4;struct Cyc_Set_Set**_tmp8E5;struct Cyc_Toc_TocState
_tmp8E3=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8E4=
_tmp8E3.dyn;_tmp8E5=_tmp8E3.datatypes_so_far;{struct _DynRegionFrame _tmp8E6;
struct _RegionHandle*d=_open_dynregion(& _tmp8E6,_tmp8E4);{struct _tuple0*_tmp8E7=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8E5,_tmp8E7)){_npop_handler(0);return;}*_tmp8E5=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8E5,_tmp8E7);}{struct Cyc_List_List*_tmp8E8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8E8 != 0;_tmp8E8=_tmp8E8->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8E8->hd;struct Cyc_List_List*_tmp8E9=0;int i=
1;{struct Cyc_List_List*_tmp8EA=f->typs;for(0;_tmp8EA != 0;(_tmp8EA=_tmp8EA->tl,i
++)){struct _dyneither_ptr*_tmp8EB=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpD9A;struct Cyc_Absyn_Aggrfield*_tmp8EC=(_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((
_tmpD9A->name=_tmp8EB,((_tmpD9A->tq=(*((struct _tuple9*)_tmp8EA->hd)).f1,((
_tmpD9A->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8EA->hd)).f2),((
_tmpD9A->width=0,((_tmpD9A->attributes=0,_tmpD9A)))))))))));struct Cyc_List_List*
_tmpD9B;_tmp8E9=((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->hd=_tmp8EC,((
_tmpD9B->tl=_tmp8E9,_tmpD9B))))));}}{struct Cyc_Absyn_Aggrfield*_tmpD9E;struct Cyc_List_List*
_tmpD9D;_tmp8E9=((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->hd=((_tmpD9E=
_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->name=Cyc_Toc_tag_sp,((_tmpD9E->tq=Cyc_Toc_mt_tq,((
_tmpD9E->type=Cyc_Absyn_sint_typ,((_tmpD9E->width=0,((_tmpD9E->attributes=0,
_tmpD9E)))))))))))),((_tmpD9D->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8E9),_tmpD9D))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDA3;const char*_tmpDA2;struct Cyc_Absyn_Aggrdecl*_tmpDA1;struct Cyc_Absyn_Aggrdecl*
_tmp8F1=(_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->kind=Cyc_Absyn_StructA,((
_tmpDA1->sc=Cyc_Absyn_Public,((_tmpDA1->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDA2="_struct",_tag_dyneither(_tmpDA2,sizeof(char),8)))),((_tmpDA1->tvs=0,((
_tmpDA1->impl=((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->exist_vars=0,((
_tmpDA3->rgn_po=0,((_tmpDA3->fields=_tmp8E9,((_tmpDA3->tagged=0,_tmpDA3)))))))))),((
_tmpDA1->attributes=0,_tmpDA1)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDA9;
struct Cyc_Absyn_Aggr_d_struct _tmpDA8;struct Cyc_List_List*_tmpDA7;Cyc_Toc_result_decls=((
_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->hd=Cyc_Absyn_new_decl((void*)((
_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9[0]=((_tmpDA8.tag=6,((_tmpDA8.f1=
_tmp8F1,_tmpDA8)))),_tmpDA9)))),0),((_tmpDA7->tl=Cyc_Toc_result_decls,_tmpDA7))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp8F9;struct Cyc_Dict_Dict*_tmp8FA;struct
Cyc_Toc_TocState _tmp8F8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8F9=_tmp8F8.dyn;_tmp8FA=_tmp8F8.xdatatypes_so_far;{struct _DynRegionFrame
_tmp8FB;struct _RegionHandle*d=_open_dynregion(& _tmp8FB,_tmp8F9);{struct _tuple0*
_tmp8FC=xd->name;struct Cyc_List_List*_tmp8FD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FD->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp8FF=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp8FE,Cyc_Absyn_false_conref,0);int*_tmp900=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8FA,f->name);int
_tmp901;_LL3A0: if(_tmp900 != 0)goto _LL3A2;_LL3A1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp902=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,
initopt);_tmp902->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDAF;struct Cyc_Absyn_Var_d_struct
_tmpDAE;struct Cyc_List_List*_tmpDAD;Cyc_Toc_result_decls=((_tmpDAD=_cycalloc(
sizeof(*_tmpDAD)),((_tmpDAD->hd=Cyc_Absyn_new_decl((void*)((_tmpDAF=_cycalloc(
sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDAE.tag=0,((_tmpDAE.f1=_tmp902,_tmpDAE)))),
_tmpDAF)))),0),((_tmpDAD->tl=Cyc_Toc_result_decls,_tmpDAD))))));}*_tmp8FA=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp8FA,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp906=f->typs;for(0;_tmp906 != 0;(_tmp906=_tmp906->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDB7;void*_tmpDB6[1];const char*_tmpDB5;struct
_dyneither_ptr*_tmpDB4;struct _dyneither_ptr*_tmp907=(_tmpDB4=_cycalloc(sizeof(*
_tmpDB4)),((_tmpDB4[0]=(struct _dyneither_ptr)((_tmpDB7.tag=1,((_tmpDB7.f1=(
unsigned long)i,((_tmpDB6[0]=& _tmpDB7,Cyc_aprintf(((_tmpDB5="f%d",_tag_dyneither(
_tmpDB5,sizeof(char),4))),_tag_dyneither(_tmpDB6,sizeof(void*),1)))))))),_tmpDB4)));
struct Cyc_Absyn_Aggrfield*_tmpDB8;struct Cyc_Absyn_Aggrfield*_tmp908=(_tmpDB8=
_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->name=_tmp907,((_tmpDB8->tq=(*((struct
_tuple9*)_tmp906->hd)).f1,((_tmpDB8->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp906->hd)).f2),((_tmpDB8->width=0,((_tmpDB8->attributes=0,_tmpDB8)))))))))));
struct Cyc_List_List*_tmpDB9;fields=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->hd=_tmp908,((_tmpDB9->tl=fields,_tmpDB9))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDBC;struct Cyc_List_List*_tmpDBB;fields=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((
_tmpDBB->hd=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->name=Cyc_Toc_tag_sp,((
_tmpDBC->tq=Cyc_Toc_mt_tq,((_tmpDBC->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDBC->width=0,((_tmpDBC->attributes=0,_tmpDBC)))))))))))),((
_tmpDBB->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDBB))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDC1;const char*_tmpDC0;
struct Cyc_Absyn_Aggrdecl*_tmpDBF;struct Cyc_Absyn_Aggrdecl*_tmp911=(_tmpDBF=
_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->kind=Cyc_Absyn_StructA,((_tmpDBF->sc=Cyc_Absyn_Public,((
_tmpDBF->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDC0="_struct",
_tag_dyneither(_tmpDC0,sizeof(char),8)))),((_tmpDBF->tvs=0,((_tmpDBF->impl=((
_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->exist_vars=0,((_tmpDC1->rgn_po=0,((
_tmpDC1->fields=fields,((_tmpDC1->tagged=0,_tmpDC1)))))))))),((_tmpDBF->attributes=
0,_tmpDBF)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDC7;struct Cyc_Absyn_Aggr_d_struct
_tmpDC6;struct Cyc_List_List*_tmpDC5;Cyc_Toc_result_decls=((_tmpDC5=_cycalloc(
sizeof(*_tmpDC5)),((_tmpDC5->hd=Cyc_Absyn_new_decl((void*)((_tmpDC7=_cycalloc(
sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDC6.tag=6,((_tmpDC6.f1=_tmp911,_tmpDC6)))),
_tmpDC7)))),0),((_tmpDC5->tl=Cyc_Toc_result_decls,_tmpDC5))))));}goto _LL39F;};};};}
_LL3A2: if(_tmp900 == 0)goto _LL3A4;_tmp901=*_tmp900;if(_tmp901 != 0)goto _LL3A4;
_LL3A3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp918=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp919=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,(
struct Cyc_Absyn_Exp*)_tmp918);_tmp919->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDCD;struct Cyc_Absyn_Var_d_struct _tmpDCC;struct Cyc_List_List*_tmpDCB;Cyc_Toc_result_decls=((
_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->hd=Cyc_Absyn_new_decl((void*)((
_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCC.tag=0,((_tmpDCC.f1=
_tmp919,_tmpDCC)))),_tmpDCD)))),0),((_tmpDCB->tl=Cyc_Toc_result_decls,_tmpDCB))))));}*
_tmp8FA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp8FA,f->name,1);}goto _LL39F;_LL3A4:;_LL3A5: goto _LL39F;_LL39F:;}};
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
void*_tmp91D=init->r;struct Cyc_Absyn_Vardecl*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;
struct Cyc_Absyn_Exp*_tmp921;int _tmp922;_LL3A7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp91E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp91D;if(_tmp91E->tag != 28)
goto _LL3A9;else{_tmp91F=_tmp91E->f1;_tmp920=_tmp91E->f2;_tmp921=_tmp91E->f3;
_tmp922=_tmp91E->f4;}}_LL3A8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp91F,_tmp920,_tmp921,_tmp922,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A6;_LL3A9:;_LL3AA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp924;struct _RegionHandle*_tmp925;struct Cyc_Toc_Env*_tmp923=init_nv;_tmp924=*
_tmp923;_tmp925=_tmp924.rgn;{struct Cyc_Toc_Env*_tmp926=Cyc_Toc_set_toplevel(
_tmp925,init_nv);Cyc_Toc_exp_to_c(_tmp926,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A6;_LL3A6:;}else{void*_tmp927=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp929;void*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;union Cyc_Absyn_Constraint*
_tmp92C;_LL3AC:{struct Cyc_Absyn_ArrayType_struct*_tmp928=(struct Cyc_Absyn_ArrayType_struct*)
_tmp927;if(_tmp928->tag != 9)goto _LL3AE;else{_tmp929=_tmp928->f1;_tmp92A=_tmp929.elt_type;
_tmp92B=_tmp929.num_elts;_tmp92C=_tmp929.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92C))goto _LL3AE;_LL3AD: if(_tmp92B == 0){const char*
_tmpDD0;void*_tmpDCF;(_tmpDCF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD0="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDD0,sizeof(char),55))),_tag_dyneither(_tmpDCF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92B;struct Cyc_Absyn_Exp*
_tmp92F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp930=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp92F,
_tmp930,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AB;};_LL3AE:;_LL3AF: goto _LL3AB;
_LL3AB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp932;struct _RegionHandle*
_tmp933;struct Cyc_Toc_Env*_tmp931=nv;_tmp932=*_tmp931;_tmp933=_tmp932.rgn;{
struct Cyc_Absyn_Stmt*_tmp934=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp935=Cyc_Toc_share_env(_tmp933,nv);struct Cyc_Toc_Env*_tmp937;struct Cyc_List_List*
_tmp938;struct Cyc_Absyn_Stmt*_tmp939;struct _tuple21 _tmp936=Cyc_Toc_xlate_pat(
_tmp935,_tmp933,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp937=_tmp936.f1;_tmp938=_tmp936.f2;_tmp939=_tmp936.f3;Cyc_Toc_stmt_to_c(
_tmp937,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp939,s,0),0);for(0;_tmp938 != 0;_tmp938=_tmp938->tl){
struct _tuple22 _tmp93B;struct _tuple0*_tmp93C;void*_tmp93D;struct _tuple22*_tmp93A=(
struct _tuple22*)_tmp938->hd;_tmp93B=*_tmp93A;_tmp93C=_tmp93B.f1;_tmp93D=_tmp93B.f2;
s=Cyc_Absyn_declare_stmt(_tmp93C,_tmp93D,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp93E=e->r;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_Absyn_Exp*_tmp942;struct
Cyc_Absyn_Exp*_tmp944;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp948;
struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*
_tmp94E;struct Cyc_List_List*_tmp950;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*
_tmp953;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*
_tmp958;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*
_tmp95C;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*
_tmp961;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*
_tmp965;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_List_List*
_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_List_List*_tmp96C;void*_tmp96E;
void**_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct _tuple7*_tmp972;struct _tuple7
_tmp973;void*_tmp974;void**_tmp975;struct Cyc_List_List*_tmp976;struct Cyc_List_List*
_tmp978;struct Cyc_List_List*_tmp97A;void*_tmp97C;void**_tmp97D;void*_tmp97F;void**
_tmp980;struct Cyc_Absyn_Stmt*_tmp982;struct Cyc_Absyn_MallocInfo _tmp984;struct Cyc_Absyn_MallocInfo*
_tmp985;_LL3B1: {struct Cyc_Absyn_Deref_e_struct*_tmp93F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp93E;if(_tmp93F->tag != 21)goto _LL3B3;else{_tmp940=_tmp93F->f1;}}_LL3B2:
_tmp942=_tmp940;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp941=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp93E;if(_tmp941->tag != 22)goto _LL3B5;
else{_tmp942=_tmp941->f1;}}_LL3B4: _tmp944=_tmp942;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp943=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp93E;if(_tmp943->tag != 23)goto
_LL3B7;else{_tmp944=_tmp943->f1;}}_LL3B6: _tmp946=_tmp944;goto _LL3B8;_LL3B7: {
struct Cyc_Absyn_Address_e_struct*_tmp945=(struct Cyc_Absyn_Address_e_struct*)
_tmp93E;if(_tmp945->tag != 16)goto _LL3B9;else{_tmp946=_tmp945->f1;}}_LL3B8:
_tmp948=_tmp946;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_struct*_tmp947=(
struct Cyc_Absyn_Throw_e_struct*)_tmp93E;if(_tmp947->tag != 12)goto _LL3BB;else{
_tmp948=_tmp947->f1;}}_LL3BA: _tmp94A=_tmp948;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp949=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp93E;if(_tmp949->tag != 13)
goto _LL3BD;else{_tmp94A=_tmp949->f1;}}_LL3BC: _tmp94C=_tmp94A;goto _LL3BE;_LL3BD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp93E;if(_tmp94B->tag != 19)goto _LL3BF;else{_tmp94C=_tmp94B->f1;}}_LL3BE:
_tmp94E=_tmp94C;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_struct*_tmp94D=(
struct Cyc_Absyn_Increment_e_struct*)_tmp93E;if(_tmp94D->tag != 5)goto _LL3C1;else{
_tmp94E=_tmp94D->f1;}}_LL3C0: Cyc_Toc_exptypes_to_c(_tmp94E);goto _LL3B0;_LL3C1: {
struct Cyc_Absyn_Primop_e_struct*_tmp94F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp93E;if(_tmp94F->tag != 3)goto _LL3C3;else{_tmp950=_tmp94F->f2;}}_LL3C2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp950);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_struct*_tmp951=(struct Cyc_Absyn_And_e_struct*)
_tmp93E;if(_tmp951->tag != 7)goto _LL3C5;else{_tmp952=_tmp951->f1;_tmp953=_tmp951->f2;}}
_LL3C4: _tmp955=_tmp952;_tmp956=_tmp953;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_struct*
_tmp954=(struct Cyc_Absyn_Or_e_struct*)_tmp93E;if(_tmp954->tag != 8)goto _LL3C7;
else{_tmp955=_tmp954->f1;_tmp956=_tmp954->f2;}}_LL3C6: _tmp958=_tmp955;_tmp959=
_tmp956;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp957=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp93E;if(_tmp957->tag != 9)goto _LL3C9;else{_tmp958=_tmp957->f1;_tmp959=_tmp957->f2;}}
_LL3C8: _tmp95B=_tmp958;_tmp95C=_tmp959;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp95A=(struct Cyc_Absyn_Subscript_e_struct*)_tmp93E;if(_tmp95A->tag != 24)goto
_LL3CB;else{_tmp95B=_tmp95A->f1;_tmp95C=_tmp95A->f2;}}_LL3CA: _tmp95E=_tmp95B;
_tmp95F=_tmp95C;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_struct*_tmp95D=(
struct Cyc_Absyn_Swap_e_struct*)_tmp93E;if(_tmp95D->tag != 35)goto _LL3CD;else{
_tmp95E=_tmp95D->f1;_tmp95F=_tmp95D->f2;}}_LL3CC: _tmp961=_tmp95E;_tmp962=_tmp95F;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp960=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp93E;if(_tmp960->tag != 4)goto _LL3CF;else{_tmp961=_tmp960->f1;_tmp962=_tmp960->f3;}}
_LL3CE: Cyc_Toc_exptypes_to_c(_tmp961);Cyc_Toc_exptypes_to_c(_tmp962);goto _LL3B0;
_LL3CF: {struct Cyc_Absyn_Conditional_e_struct*_tmp963=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp93E;if(_tmp963->tag != 6)goto _LL3D1;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;
_tmp966=_tmp963->f3;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp964);Cyc_Toc_exptypes_to_c(
_tmp965);Cyc_Toc_exptypes_to_c(_tmp966);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp967=(struct Cyc_Absyn_FnCall_e_struct*)_tmp93E;if(_tmp967->tag != 11)goto
_LL3D3;else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3D2: _tmp96B=_tmp968;
_tmp96C=_tmp969;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp96A=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp93E;if(_tmp96A->tag != 10)goto _LL3D5;
else{_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;}}_LL3D4: Cyc_Toc_exptypes_to_c(
_tmp96B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96C);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_Cast_e_struct*
_tmp96D=(struct Cyc_Absyn_Cast_e_struct*)_tmp93E;if(_tmp96D->tag != 15)goto _LL3D7;
else{_tmp96E=(void**)& _tmp96D->f1;_tmp96F=(void**)((void**)& _tmp96D->f1);_tmp970=
_tmp96D->f2;}}_LL3D6:*_tmp96F=Cyc_Toc_typ_to_c(*_tmp96F);Cyc_Toc_exptypes_to_c(
_tmp970);goto _LL3B0;_LL3D7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp971=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp93E;if(_tmp971->tag != 26)goto _LL3D9;
else{_tmp972=_tmp971->f1;_tmp973=*_tmp972;_tmp974=_tmp973.f3;_tmp975=(void**)&(*
_tmp971->f1).f3;_tmp976=_tmp971->f2;}}_LL3D8:*_tmp975=Cyc_Toc_typ_to_c(*_tmp975);
_tmp978=_tmp976;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp977=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp93E;if(_tmp977->tag != 36)
goto _LL3DB;else{_tmp978=_tmp977->f2;}}_LL3DA: _tmp97A=_tmp978;goto _LL3DC;_LL3DB: {
struct Cyc_Absyn_Array_e_struct*_tmp979=(struct Cyc_Absyn_Array_e_struct*)_tmp93E;
if(_tmp979->tag != 27)goto _LL3DD;else{_tmp97A=_tmp979->f1;}}_LL3DC: for(0;_tmp97A
!= 0;_tmp97A=_tmp97A->tl){struct Cyc_Absyn_Exp*_tmp995;struct _tuple15 _tmp994=*((
struct _tuple15*)_tmp97A->hd);_tmp995=_tmp994.f2;Cyc_Toc_exptypes_to_c(_tmp995);}
goto _LL3B0;_LL3DD: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97B=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp93E;if(_tmp97B->tag != 20)goto _LL3DF;else{_tmp97C=(void**)& _tmp97B->f1;
_tmp97D=(void**)((void**)& _tmp97B->f1);}}_LL3DE: _tmp980=_tmp97D;goto _LL3E0;
_LL3DF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp97E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp93E;if(_tmp97E->tag != 18)goto _LL3E1;else{_tmp97F=(void**)& _tmp97E->f1;
_tmp980=(void**)((void**)& _tmp97E->f1);}}_LL3E0:*_tmp980=Cyc_Toc_typ_to_c(*
_tmp980);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp981=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp93E;if(_tmp981->tag != 37)goto _LL3E3;else{_tmp982=_tmp981->f1;}}_LL3E2: Cyc_Toc_stmttypes_to_c(
_tmp982);goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Malloc_e_struct*_tmp983=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp93E;if(_tmp983->tag != 34)goto _LL3E5;else{_tmp984=_tmp983->f1;_tmp985=(struct
Cyc_Absyn_MallocInfo*)& _tmp983->f1;}}_LL3E4: if(_tmp985->elt_type != 0){void**
_tmpDD1;_tmp985->elt_type=((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp985->elt_type))),_tmpDD1))));}Cyc_Toc_exptypes_to_c(
_tmp985->num_elts);goto _LL3B0;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp986=(
struct Cyc_Absyn_Const_e_struct*)_tmp93E;if(_tmp986->tag != 0)goto _LL3E7;}_LL3E6:
goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Var_e_struct*_tmp987=(struct Cyc_Absyn_Var_e_struct*)
_tmp93E;if(_tmp987->tag != 1)goto _LL3E9;}_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp988=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp93E;if(_tmp988->tag != 2)goto
_LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Enum_e_struct*_tmp989=(struct
Cyc_Absyn_Enum_e_struct*)_tmp93E;if(_tmp989->tag != 32)goto _LL3ED;}_LL3EC: goto
_LL3EE;_LL3ED: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp98A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp93E;if(_tmp98A->tag != 33)goto _LL3EF;}_LL3EE: goto _LL3B0;_LL3EF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp93E;if(_tmp98B->tag != 30)goto
_LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Datatype_e_struct*_tmp98C=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp93E;if(_tmp98C->tag != 31)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp93E;if(_tmp98D->tag != 29)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp98E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp93E;if(_tmp98E->tag != 28)
goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_struct*_tmp98F=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp93E;if(_tmp98F->tag != 25)goto _LL3F9;}_LL3F8:
goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_struct*_tmp990=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp93E;if(_tmp990->tag != 14)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_struct*
_tmp991=(struct Cyc_Absyn_New_e_struct*)_tmp93E;if(_tmp991->tag != 17)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_struct*_tmp992=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp93E;if(_tmp992->tag != 39)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp993=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp93E;if(_tmp993->tag != 38)goto
_LL3B0;}_LL400:{const char*_tmpDD4;void*_tmpDD3;(_tmpDD3=0,Cyc_Tcutil_terr(e->loc,((
_tmpDD4="Cyclone expression within C code",_tag_dyneither(_tmpDD4,sizeof(char),
33))),_tag_dyneither(_tmpDD3,sizeof(void*),0)));}goto _LL3B0;_LL3B0:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp999=d->r;struct Cyc_Absyn_Vardecl*_tmp99B;struct
Cyc_Absyn_Fndecl*_tmp99D;struct Cyc_Absyn_Aggrdecl*_tmp99F;struct Cyc_Absyn_Enumdecl*
_tmp9A1;struct Cyc_Absyn_Typedefdecl*_tmp9A3;_LL402: {struct Cyc_Absyn_Var_d_struct*
_tmp99A=(struct Cyc_Absyn_Var_d_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL404;
else{_tmp99B=_tmp99A->f1;}}_LL403: _tmp99B->type=Cyc_Toc_typ_to_c(_tmp99B->type);
if(_tmp99B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp99B->initializer));goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_struct*
_tmp99C=(struct Cyc_Absyn_Fn_d_struct*)_tmp999;if(_tmp99C->tag != 1)goto _LL406;
else{_tmp99D=_tmp99C->f1;}}_LL405: _tmp99D->ret_type=Cyc_Toc_typ_to_c(_tmp99D->ret_type);{
struct Cyc_List_List*_tmp9AF=_tmp99D->args;for(0;_tmp9AF != 0;_tmp9AF=_tmp9AF->tl){(*((
struct _tuple25*)_tmp9AF->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9AF->hd)).f3);}}
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_struct*_tmp99E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp999;if(_tmp99E->tag != 6)goto _LL408;else{_tmp99F=_tmp99E->f1;}}_LL407: Cyc_Toc_aggrdecl_to_c(
_tmp99F);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_struct*_tmp9A0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp999;if(_tmp9A0->tag != 8)goto _LL40A;else{_tmp9A1=_tmp9A0->f1;}}_LL409: if(
_tmp9A1->fields != 0){struct Cyc_List_List*_tmp9B0=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A1->fields))->v;for(0;_tmp9B0 != 0;_tmp9B0=_tmp9B0->tl){
struct Cyc_Absyn_Enumfield*_tmp9B1=(struct Cyc_Absyn_Enumfield*)_tmp9B0->hd;if(
_tmp9B1->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B1->tag));}}
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A2=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp999;if(_tmp9A2->tag != 9)goto _LL40C;else{_tmp9A3=_tmp9A2->f1;}}_LL40B:{struct
Cyc_Core_Opt*_tmpDD5;_tmp9A3->defn=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((
_tmpDD5->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A3->defn))->v),_tmpDD5))));}goto _LL401;_LL40C: {struct Cyc_Absyn_Let_d_struct*
_tmp9A4=(struct Cyc_Absyn_Let_d_struct*)_tmp999;if(_tmp9A4->tag != 2)goto _LL40E;}
_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_struct*_tmp9A5=(struct Cyc_Absyn_Letv_d_struct*)
_tmp999;if(_tmp9A5->tag != 3)goto _LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9A6=(struct Cyc_Absyn_Datatype_d_struct*)_tmp999;if(_tmp9A6->tag != 7)goto
_LL412;}_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_struct*_tmp9A7=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp999;if(_tmp9A7->tag != 10)goto _LL414;}
_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_struct*_tmp9A8=(struct Cyc_Absyn_Using_d_struct*)
_tmp999;if(_tmp9A8->tag != 11)goto _LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9A9=(struct Cyc_Absyn_ExternC_d_struct*)_tmp999;if(_tmp9A9->tag != 12)goto
_LL418;}_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AA=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp999;if(_tmp9AA->tag != 13)
goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_struct*_tmp9AB=(
struct Cyc_Absyn_Region_d_struct*)_tmp999;if(_tmp9AB->tag != 4)goto _LL41C;}_LL41B:
goto _LL41D;_LL41C: {struct Cyc_Absyn_Alias_d_struct*_tmp9AC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp999;if(_tmp9AC->tag != 5)goto _LL41E;}_LL41D:{const char*_tmpDD8;void*_tmpDD7;(
_tmpDD7=0,Cyc_Tcutil_terr(d->loc,((_tmpDD8="Cyclone declaration within C code",
_tag_dyneither(_tmpDD8,sizeof(char),34))),_tag_dyneither(_tmpDD7,sizeof(void*),0)));}
goto _LL401;_LL41E: {struct Cyc_Absyn_Porton_d_struct*_tmp9AD=(struct Cyc_Absyn_Porton_d_struct*)
_tmp999;if(_tmp9AD->tag != 14)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9AE=(struct Cyc_Absyn_Portoff_d_struct*)_tmp999;if(_tmp9AE->tag != 15)goto
_LL401;}_LL421: goto _LL401;_LL401:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9B5=s->r;
struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Stmt*_tmp9B9;struct Cyc_Absyn_Stmt*
_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BE;struct Cyc_Absyn_Stmt*
_tmp9BF;struct Cyc_Absyn_Stmt*_tmp9C0;struct _tuple8 _tmp9C2;struct Cyc_Absyn_Exp*
_tmp9C3;struct Cyc_Absyn_Stmt*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C6;struct _tuple8
_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C8;struct _tuple8 _tmp9C9;struct Cyc_Absyn_Exp*
_tmp9CA;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CD;struct Cyc_List_List*
_tmp9CE;struct Cyc_Absyn_Decl*_tmp9D0;struct Cyc_Absyn_Stmt*_tmp9D1;struct Cyc_Absyn_Stmt*
_tmp9D3;struct _tuple8 _tmp9D4;struct Cyc_Absyn_Exp*_tmp9D5;_LL423: {struct Cyc_Absyn_Exp_s_struct*
_tmp9B6=(struct Cyc_Absyn_Exp_s_struct*)_tmp9B5;if(_tmp9B6->tag != 1)goto _LL425;
else{_tmp9B7=_tmp9B6->f1;}}_LL424: Cyc_Toc_exptypes_to_c(_tmp9B7);goto _LL422;
_LL425: {struct Cyc_Absyn_Seq_s_struct*_tmp9B8=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B5;if(_tmp9B8->tag != 2)goto _LL427;else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B8->f2;}}
_LL426: Cyc_Toc_stmttypes_to_c(_tmp9B9);Cyc_Toc_stmttypes_to_c(_tmp9BA);goto
_LL422;_LL427: {struct Cyc_Absyn_Return_s_struct*_tmp9BB=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B5;if(_tmp9BB->tag != 3)goto _LL429;else{_tmp9BC=_tmp9BB->f1;}}_LL428: if(
_tmp9BC != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BC);goto _LL422;
_LL429: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9BD=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B5;if(_tmp9BD->tag != 4)goto _LL42B;else{_tmp9BE=_tmp9BD->f1;_tmp9BF=_tmp9BD->f2;
_tmp9C0=_tmp9BD->f3;}}_LL42A: Cyc_Toc_exptypes_to_c(_tmp9BE);Cyc_Toc_stmttypes_to_c(
_tmp9BF);Cyc_Toc_stmttypes_to_c(_tmp9C0);goto _LL422;_LL42B: {struct Cyc_Absyn_While_s_struct*
_tmp9C1=(struct Cyc_Absyn_While_s_struct*)_tmp9B5;if(_tmp9C1->tag != 5)goto _LL42D;
else{_tmp9C2=_tmp9C1->f1;_tmp9C3=_tmp9C2.f1;_tmp9C4=_tmp9C1->f2;}}_LL42C: Cyc_Toc_exptypes_to_c(
_tmp9C3);Cyc_Toc_stmttypes_to_c(_tmp9C4);goto _LL422;_LL42D: {struct Cyc_Absyn_For_s_struct*
_tmp9C5=(struct Cyc_Absyn_For_s_struct*)_tmp9B5;if(_tmp9C5->tag != 9)goto _LL42F;
else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f2;_tmp9C8=_tmp9C7.f1;_tmp9C9=_tmp9C5->f3;
_tmp9CA=_tmp9C9.f1;_tmp9CB=_tmp9C5->f4;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9C6);
Cyc_Toc_exptypes_to_c(_tmp9C8);Cyc_Toc_exptypes_to_c(_tmp9CA);Cyc_Toc_stmttypes_to_c(
_tmp9CB);goto _LL422;_LL42F: {struct Cyc_Absyn_Switch_s_struct*_tmp9CC=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B5;if(_tmp9CC->tag != 10)goto _LL431;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}
_LL430: Cyc_Toc_exptypes_to_c(_tmp9CD);for(0;_tmp9CE != 0;_tmp9CE=_tmp9CE->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9CE->hd)->body);}goto _LL422;_LL431: {struct
Cyc_Absyn_Decl_s_struct*_tmp9CF=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B5;if(
_tmp9CF->tag != 12)goto _LL433;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}
_LL432: Cyc_Toc_decltypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(_tmp9D1);goto
_LL422;_LL433: {struct Cyc_Absyn_Do_s_struct*_tmp9D2=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B5;if(_tmp9D2->tag != 14)goto _LL435;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;
_tmp9D5=_tmp9D4.f1;}}_LL434: Cyc_Toc_stmttypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(
_tmp9D5);goto _LL422;_LL435: {struct Cyc_Absyn_Skip_s_struct*_tmp9D6=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B5;if(_tmp9D6->tag != 0)goto _LL437;}_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_Break_s_struct*
_tmp9D7=(struct Cyc_Absyn_Break_s_struct*)_tmp9B5;if(_tmp9D7->tag != 6)goto _LL439;}
_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Continue_s_struct*_tmp9D8=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B5;if(_tmp9D8->tag != 7)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Goto_s_struct*
_tmp9D9=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B5;if(_tmp9D9->tag != 8)goto _LL43D;}
_LL43C: goto _LL422;_LL43D: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DA=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B5;if(_tmp9DA->tag != 11)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Label_s_struct*
_tmp9DB=(struct Cyc_Absyn_Label_s_struct*)_tmp9B5;if(_tmp9DB->tag != 13)goto _LL441;}
_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DC=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B5;if(_tmp9DC->tag != 15)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9DD=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B5;if(_tmp9DD->tag != 16)goto
_LL422;}_LL444:{const char*_tmpDDB;void*_tmpDDA;(_tmpDDA=0,Cyc_Tcutil_terr(s->loc,((
_tmpDDB="Cyclone statement in C code",_tag_dyneither(_tmpDDB,sizeof(char),28))),
_tag_dyneither(_tmpDDA,sizeof(void*),0)));}goto _LL422;_LL422:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDDE;void*
_tmpDDD;(_tmpDDD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDDE="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDDE,sizeof(char),29))),
_tag_dyneither(_tmpDDD,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E2=d->r;struct Cyc_Absyn_Vardecl*_tmp9E4;struct Cyc_Absyn_Fndecl*
_tmp9E6;struct Cyc_Absyn_Aggrdecl*_tmp9EC;struct Cyc_Absyn_Datatypedecl*_tmp9EE;
struct Cyc_Absyn_Enumdecl*_tmp9F0;struct Cyc_Absyn_Typedefdecl*_tmp9F2;struct Cyc_List_List*
_tmp9F6;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9FA;struct Cyc_List_List*
_tmp9FC;_LL446: {struct Cyc_Absyn_Var_d_struct*_tmp9E3=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E2;if(_tmp9E3->tag != 0)goto _LL448;else{_tmp9E4=_tmp9E3->f1;}}_LL447: {
struct _tuple0*_tmp9FD=_tmp9E4->name;if(_tmp9E4->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDDF;_tmp9FD=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->f1=Cyc_Absyn_Rel_n(
0),((_tmpDDF->f2=(*_tmp9FD).f2,_tmpDDF))))));}if(_tmp9E4->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDE2;struct Cyc_Absyn_Global_b_struct*_tmpDE1;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E4->name,Cyc_Absyn_varb_exp(_tmp9FD,(void*)((
_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE2.tag=1,((_tmpDE2.f1=
_tmp9E4,_tmpDE2)))),_tmpDE1)))),0));}_tmp9E4->name=_tmp9FD;_tmp9E4->sc=Cyc_Toc_scope_to_c(
_tmp9E4->sc);_tmp9E4->type=Cyc_Toc_typ_to_c_array(_tmp9E4->type);{struct Cyc_List_List*
_tmpDE3;Cyc_Toc_result_decls=((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->hd=
d,((_tmpDE3->tl=Cyc_Toc_result_decls,_tmpDE3))))));}goto _LL445;}_LL448: {struct
Cyc_Absyn_Fn_d_struct*_tmp9E5=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E2;if(_tmp9E5->tag
!= 1)goto _LL44A;else{_tmp9E6=_tmp9E5->f1;}}_LL449: {struct _tuple0*_tmpA02=
_tmp9E6->name;if(_tmp9E6->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDE4;_tmpA02=((
_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->f1=Cyc_Absyn_Rel_n(0),((_tmpDE4->f2=(*
_tmpA02).f2,_tmpDE4))))));}_tmp9E6->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9E6->name,Cyc_Absyn_var_exp(_tmpA02,0));_tmp9E6->name=_tmpA02;Cyc_Toc_fndecl_to_c(
nv,_tmp9E6,cinclude);{struct Cyc_List_List*_tmpDE5;Cyc_Toc_result_decls=((_tmpDE5=
_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=d,((_tmpDE5->tl=Cyc_Toc_result_decls,
_tmpDE5))))));}goto _LL445;}_LL44A: {struct Cyc_Absyn_Let_d_struct*_tmp9E7=(struct
Cyc_Absyn_Let_d_struct*)_tmp9E2;if(_tmp9E7->tag != 2)goto _LL44C;}_LL44B: goto
_LL44D;_LL44C: {struct Cyc_Absyn_Letv_d_struct*_tmp9E8=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E2;if(_tmp9E8->tag != 3)goto _LL44E;}_LL44D: {const char*_tmpDE8;void*_tmpDE7;(
_tmpDE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDE8="letdecl at toplevel",_tag_dyneither(_tmpDE8,sizeof(char),20))),
_tag_dyneither(_tmpDE7,sizeof(void*),0)));}_LL44E: {struct Cyc_Absyn_Region_d_struct*
_tmp9E9=(struct Cyc_Absyn_Region_d_struct*)_tmp9E2;if(_tmp9E9->tag != 4)goto _LL450;}
_LL44F: {const char*_tmpDEB;void*_tmpDEA;(_tmpDEA=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDEB="region decl at toplevel",
_tag_dyneither(_tmpDEB,sizeof(char),24))),_tag_dyneither(_tmpDEA,sizeof(void*),0)));}
_LL450: {struct Cyc_Absyn_Alias_d_struct*_tmp9EA=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E2;if(_tmp9EA->tag != 5)goto _LL452;}_LL451: {const char*_tmpDEE;void*_tmpDED;(
_tmpDED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDEE="alias decl at toplevel",_tag_dyneither(_tmpDEE,sizeof(char),23))),
_tag_dyneither(_tmpDED,sizeof(void*),0)));}_LL452: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9EB=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9E2;if(_tmp9EB->tag != 6)goto _LL454;
else{_tmp9EC=_tmp9EB->f1;}}_LL453: Cyc_Toc_aggrdecl_to_c(_tmp9EC);goto _LL445;
_LL454: {struct Cyc_Absyn_Datatype_d_struct*_tmp9ED=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E2;if(_tmp9ED->tag != 7)goto _LL456;else{_tmp9EE=_tmp9ED->f1;}}_LL455: if(
_tmp9EE->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9EE);else{Cyc_Toc_datatypedecl_to_c(
_tmp9EE);}goto _LL445;_LL456: {struct Cyc_Absyn_Enum_d_struct*_tmp9EF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E2;if(_tmp9EF->tag != 8)goto _LL458;else{_tmp9F0=_tmp9EF->f1;}}_LL457: Cyc_Toc_enumdecl_to_c(
nv,_tmp9F0);{struct Cyc_List_List*_tmpDEF;Cyc_Toc_result_decls=((_tmpDEF=
_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->hd=d,((_tmpDEF->tl=Cyc_Toc_result_decls,
_tmpDEF))))));}goto _LL445;_LL458: {struct Cyc_Absyn_Typedef_d_struct*_tmp9F1=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9E2;if(_tmp9F1->tag != 9)goto _LL45A;else{
_tmp9F2=_tmp9F1->f1;}}_LL459: _tmp9F2->name=_tmp9F2->name;_tmp9F2->tvs=0;if(
_tmp9F2->defn != 0){struct Cyc_Core_Opt*_tmpDF0;_tmp9F2->defn=((_tmpDF0=_cycalloc(
sizeof(*_tmpDF0)),((_tmpDF0->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F2->defn))->v),_tmpDF0))));}else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F2->kind))->v){case Cyc_Absyn_BoxKind: _LL466:{
struct Cyc_Core_Opt*_tmpDF1;_tmp9F2->defn=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((
_tmpDF1->v=Cyc_Absyn_void_star_typ(),_tmpDF1))));}break;default: _LL467:{struct
Cyc_Core_Opt*_tmpDF2;_tmp9F2->defn=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((
_tmpDF2->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDF2))));}break;}}{struct Cyc_List_List*
_tmpDF3;Cyc_Toc_result_decls=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->hd=
d,((_tmpDF3->tl=Cyc_Toc_result_decls,_tmpDF3))))));}goto _LL445;_LL45A: {struct
Cyc_Absyn_Porton_d_struct*_tmp9F3=(struct Cyc_Absyn_Porton_d_struct*)_tmp9E2;if(
_tmp9F3->tag != 14)goto _LL45C;}_LL45B: goto _LL45D;_LL45C: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F4=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E2;if(_tmp9F4->tag != 15)goto
_LL45E;}_LL45D: goto _LL445;_LL45E: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F5=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E2;if(_tmp9F5->tag != 10)goto _LL460;
else{_tmp9F6=_tmp9F5->f2;}}_LL45F: _tmp9F8=_tmp9F6;goto _LL461;_LL460: {struct Cyc_Absyn_Using_d_struct*
_tmp9F7=(struct Cyc_Absyn_Using_d_struct*)_tmp9E2;if(_tmp9F7->tag != 11)goto _LL462;
else{_tmp9F8=_tmp9F7->f2;}}_LL461: _tmp9FA=_tmp9F8;goto _LL463;_LL462: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F9=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E2;if(_tmp9F9->tag != 12)goto
_LL464;else{_tmp9FA=_tmp9F9->f1;}}_LL463: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FA,top,
cinclude);goto _LL445;_LL464: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FB=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E2;if(_tmp9FB->tag != 13)goto _LL445;
else{_tmp9FC=_tmp9FB->f1;}}_LL465: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FC,top,1);goto
_LL445;_LL445:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA11;struct Cyc_Core_NewRegion _tmpA10=Cyc_Core_new_dynregion();
_tmpA11=_tmpA10.dynregion;{struct _DynRegionFrame _tmpA12;struct _RegionHandle*d=
_open_dynregion(& _tmpA12,_tmpA11);{struct Cyc_Dict_Dict*_tmpDFE;struct Cyc_Dict_Dict*
_tmpDFD;struct Cyc_Set_Set**_tmpDFC;struct Cyc_Dict_Dict*_tmpDFB;struct Cyc_List_List**
_tmpDFA;struct Cyc_Toc_TocState*_tmpDF9;Cyc_Toc_toc_state=((_tmpDF9=_cycalloc(
sizeof(*_tmpDF9)),((_tmpDF9->dyn=(struct _DynRegionHandle*)_tmpA11,((_tmpDF9->tuple_types=(
struct Cyc_List_List**)((_tmpDFA=_region_malloc(d,sizeof(*_tmpDFA)),((_tmpDFA[0]=
0,_tmpDFA)))),((_tmpDF9->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpDFB=
_region_malloc(d,sizeof(*_tmpDFB)),((_tmpDFB[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFB)))),((_tmpDF9->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpDFC=
_region_malloc(d,sizeof(*_tmpDFC)),((_tmpDFC[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFC)))),((_tmpDF9->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpDFD=
_region_malloc(d,sizeof(*_tmpDFD)),((_tmpDFD[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFD)))),((_tmpDF9->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpDFE=_region_malloc(
d,sizeof(*_tmpDFE)),((_tmpDFE[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpDFE)))),((_tmpDF9->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDF9))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpDFF;Cyc_Toc_globals=_tag_dyneither(((_tmpDFF=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpDFF[0]=& Cyc_Toc__throw_str,((_tmpDFF[1]=&
Cyc_Toc_setjmp_str,((_tmpDFF[2]=& Cyc_Toc__push_handler_str,((_tmpDFF[3]=& Cyc_Toc__pop_handler_str,((
_tmpDFF[4]=& Cyc_Toc__exn_thrown_str,((_tmpDFF[5]=& Cyc_Toc__npop_handler_str,((
_tmpDFF[6]=& Cyc_Toc__check_null_str,((_tmpDFF[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpDFF[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDFF[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpDFF[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpDFF[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpDFF[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpDFF[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpDFF[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpDFF[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpDFF[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDFF[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpDFF[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDFF[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpDFF[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDFF[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpDFF[22]=& Cyc_Toc__cycalloc_str,((_tmpDFF[23]=& Cyc_Toc__cyccalloc_str,((
_tmpDFF[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpDFF[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpDFF[26]=& Cyc_Toc__region_malloc_str,((_tmpDFF[27]=& Cyc_Toc__region_calloc_str,((
_tmpDFF[28]=& Cyc_Toc__check_times_str,((_tmpDFF[29]=& Cyc_Toc__new_region_str,((
_tmpDFF[30]=& Cyc_Toc__push_region_str,((_tmpDFF[31]=& Cyc_Toc__pop_region_str,((
_tmpDFF[32]=& Cyc_Toc__open_dynregion_str,((_tmpDFF[33]=& Cyc_Toc__push_dynregion_str,((
_tmpDFF[34]=& Cyc_Toc__pop_dynregion_str,((_tmpDFF[35]=& Cyc_Toc__reset_region_str,((
_tmpDFF[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpDFF[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpDFF)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA1A=_new_region("start");struct
_RegionHandle*start=& _tmpA1A;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA1C;struct Cyc_Toc_TocState _tmpA1B=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1C=_tmpA1B.dyn;Cyc_Core_free_dynregion(
_tmpA1C);};{struct Cyc_List_List*_tmpA1D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA1D;};;
_pop_region(start);};}
