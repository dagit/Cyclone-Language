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
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
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
Cyc_stderr,fmt,ap);{const char*_tmpA34;void*_tmpA33;(_tmpA33=0,Cyc_fprintf(Cyc_stderr,((
_tmpA34="\n",_tag_dyneither(_tmpA34,sizeof(char),2))),_tag_dyneither(_tmpA33,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA37;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA36;(int)_throw((void*)((
_tmpA36=_cycalloc_atomic(sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA37.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA37)),_tmpA36)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA3A;void*
_tmpA39;(_tmpA39=0,Cyc_fprintf(Cyc_stderr,((_tmpA3A="\n",_tag_dyneither(_tmpA3A,
sizeof(char),2))),_tag_dyneither(_tmpA39,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA3D;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA3C;(int)_throw((void*)((_tmpA3C=_cycalloc_atomic(sizeof(*_tmpA3C)),((
_tmpA3C[0]=((_tmpA3D.tag=Cyc_Toc_Toc_Impossible,_tmpA3D)),_tmpA3C)))));};}char
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
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA3E;skip_stmt_opt=((_tmpA3E=
_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=Cyc_Absyn_skip_stmt(0),_tmpA3E))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA41;struct Cyc_Absyn_Cast_e_struct*_tmpA40;
return(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=15,((
_tmpA41.f1=(void*)t,((_tmpA41.f2=e,((_tmpA41.f3=0,((_tmpA41.f4=Cyc_Absyn_No_coercion,
_tmpA41)))))))))),_tmpA40))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA44;struct Cyc_Absyn_Deref_e_struct*_tmpA43;return(void*)((_tmpA43=_cycalloc(
sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=21,((_tmpA44.f1=e,_tmpA44)))),
_tmpA43))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA47;struct Cyc_Absyn_Subscript_e_struct*
_tmpA46;return(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((
_tmpA47.tag=24,((_tmpA47.f1=e1,((_tmpA47.f2=e2,_tmpA47)))))),_tmpA46))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA4A;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA49;return(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((
_tmpA4A.tag=37,((_tmpA4A.f1=s,_tmpA4A)))),_tmpA49))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA4D;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4C;return(void*)((_tmpA4C=
_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=18,((_tmpA4D.f1=(void*)t,
_tmpA4D)))),_tmpA4C))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA50;struct Cyc_Absyn_FnCall_e_struct*
_tmpA4F;return(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((
_tmpA50.tag=11,((_tmpA50.f1=e,((_tmpA50.f2=es,((_tmpA50.f3=0,_tmpA50)))))))),
_tmpA4F))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA53;struct Cyc_Absyn_Exp_s_struct*
_tmpA52;return(void*)((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((
_tmpA53.tag=1,((_tmpA53.f1=e,_tmpA53)))),_tmpA52))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA56;struct Cyc_Absyn_Seq_s_struct*_tmpA55;return(void*)((_tmpA55=_cycalloc(
sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=2,((_tmpA56.f1=s1,((_tmpA56.f2=s2,
_tmpA56)))))),_tmpA55))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA59;struct Cyc_Absyn_Conditional_e_struct*_tmpA58;return(void*)((_tmpA58=
_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=6,((_tmpA59.f1=e1,((
_tmpA59.f2=e2,((_tmpA59.f3=e3,_tmpA59)))))))),_tmpA58))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA5C;struct Cyc_Absyn_AggrMember_e_struct*_tmpA5B;return(void*)((_tmpA5B=
_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=22,((_tmpA5C.f1=e,((
_tmpA5C.f2=n,((_tmpA5C.f3=0,((_tmpA5C.f4=0,_tmpA5C)))))))))),_tmpA5B))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA5F;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA5E;return(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((
_tmpA5F.tag=23,((_tmpA5F.f1=e,((_tmpA5F.f2=n,((_tmpA5F.f3=0,((_tmpA5F.f4=0,
_tmpA5F)))))))))),_tmpA5E))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA62;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA61;return(void*)((_tmpA61=_cycalloc(
sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA62.tag=36,((_tmpA62.f1=tdopt,((_tmpA62.f2=
ds,_tmpA62)))))),_tmpA61))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA65;
struct Cyc_Absyn_Goto_s_struct*_tmpA64;return(void*)((_tmpA64=_cycalloc(sizeof(*
_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=8,((_tmpA65.f1=v,((_tmpA65.f2=s,_tmpA65)))))),
_tmpA64))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA6B;const char*_tmpA6A;struct Cyc_Core_Impossible_struct*_tmpA69;(int)_throw((
void*)((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69[0]=((_tmpA6B.tag=Cyc_Core_Impossible,((
_tmpA6B.f1=((_tmpA6A="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA6A,sizeof(char),44))),_tmpA6B)))),_tmpA69)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA78;const char*_tmpA77;void*_tmpA76[1];
struct Cyc_String_pa_struct _tmpA75;struct Cyc_Core_Impossible_struct*_tmpA74;(int)
_throw((void*)((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA78.tag=
Cyc_Core_Impossible,((_tmpA78.f1=(struct _dyneither_ptr)((_tmpA75.tag=0,((_tmpA75.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA76[
0]=& _tmpA75,Cyc_aprintf(((_tmpA77="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA77,sizeof(char),67))),_tag_dyneither(_tmpA76,sizeof(void*),1)))))))),
_tmpA78)))),_tmpA74)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
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
_tmpA7E;const char*_tmpA7D;struct Cyc_Core_Impossible_struct*_tmpA7C;(int)_throw((
void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7E.tag=Cyc_Core_Impossible,((
_tmpA7E.f1=((_tmpA7D="impossible type (not pointer)",_tag_dyneither(_tmpA7D,
sizeof(char),30))),_tmpA7E)))),_tmpA7C)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
_LL34:;_LL35: {const char*_tmpA81;void*_tmpA80;(_tmpA80=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA81="is_nullable",
_tag_dyneither(_tmpA81,sizeof(char),12))),_tag_dyneither(_tmpA80,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA82;pair=((_tmpA82.f1=x,((_tmpA82.f2=tag,_tmpA82))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA83;struct _tuple13*_tmp108=(_tmpA83=_cycalloc(sizeof(*_tmpA83)),((
_tmpA83->f1=x,((_tmpA83->f2=tag,_tmpA83)))));struct _dyneither_ptr*_tmpA86;struct
_tuple0*_tmpA85;struct _tuple0*res=(_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->f1=(*
x).f1,((_tmpA85->f2=((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA86)))),_tmpA85)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA8E;void*_tmpA8D[
1];const char*_tmpA8C;struct _dyneither_ptr*_tmpA8B;struct _dyneither_ptr*xname=(
_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=(struct _dyneither_ptr)((_tmpA8E.tag=
1,((_tmpA8E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA8D[0]=& _tmpA8E,
Cyc_aprintf(((_tmpA8C="_tuple%d",_tag_dyneither(_tmpA8C,sizeof(char),9))),
_tag_dyneither(_tmpA8D,sizeof(void*),1)))))))),_tmpA8B)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA91;struct Cyc_List_List*
_tmpA90;_tmp11A=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->hd=((_tmpA91=
_cycalloc(sizeof(*_tmpA91)),((_tmpA91->name=Cyc_Absyn_fieldname(i),((_tmpA91->tq=
Cyc_Toc_mt_tq,((_tmpA91->type=(void*)ts2->hd,((_tmpA91->width=0,((_tmpA91->attributes=
0,_tmpA91)))))))))))),((_tmpA90->tl=_tmp11A,_tmpA90))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA96;struct _tuple0*_tmpA95;struct Cyc_Absyn_Aggrdecl*_tmpA94;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->kind=Cyc_Absyn_StructA,((
_tmpA94->sc=Cyc_Absyn_Public,((_tmpA94->name=((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((
_tmpA95->f1=Cyc_Absyn_Rel_n(0),((_tmpA95->f2=xname,_tmpA95)))))),((_tmpA94->tvs=
0,((_tmpA94->impl=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->exist_vars=0,((
_tmpA96->rgn_po=0,((_tmpA96->fields=_tmp11A,((_tmpA96->tagged=0,_tmpA96)))))))))),((
_tmpA94->attributes=0,_tmpA94)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA9C;struct Cyc_Absyn_Aggr_d_struct _tmpA9B;struct Cyc_List_List*_tmpA9A;Cyc_Toc_result_decls=((
_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=Cyc_Absyn_new_decl((void*)((
_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9B.tag=6,((_tmpA9B.f1=
_tmp11D,_tmpA9B)))),_tmpA9C)))),0),((_tmpA9A->tl=Cyc_Toc_result_decls,_tmpA9A))))));}{
struct _tuple16*_tmpA9F;struct Cyc_List_List*_tmpA9E;*_tmp111=((_tmpA9E=
_region_malloc(d,sizeof(*_tmpA9E)),((_tmpA9E->hd=((_tmpA9F=_region_malloc(d,
sizeof(*_tmpA9F)),((_tmpA9F->f1=x,((_tmpA9F->f2=ts,_tmpA9F)))))),((_tmpA9E->tl=*
_tmp111,_tmpA9E))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpAAC;const char*
_tmpAAB;void*_tmpAAA[1];struct Cyc_Int_pa_struct _tmpAA9;struct _tuple0*_tmpAA8;
struct _tuple0*res=(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->f1=Cyc_Absyn_Loc_n,((
_tmpAA8->f2=((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=(struct
_dyneither_ptr)((_tmpAA9.tag=1,((_tmpAA9.f1=(unsigned int)_tmp12B,((_tmpAAA[0]=&
_tmpAA9,Cyc_aprintf(((_tmpAAB="_tmp%X",_tag_dyneither(_tmpAAB,sizeof(char),7))),
_tag_dyneither(_tmpAAA,sizeof(void*),1)))))))),_tmpAAC)))),_tmpAA8)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAB4;void*_tmpAB3[1];const char*_tmpAB2;struct _dyneither_ptr*_tmpAB1;struct
_dyneither_ptr*res=(_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=(struct
_dyneither_ptr)((_tmpAB4.tag=1,((_tmpAB4.f1=(unsigned int)_tmp135,((_tmpAB3[0]=&
_tmpAB4,Cyc_aprintf(((_tmpAB2="_LL%X",_tag_dyneither(_tmpAB2,sizeof(char),6))),
_tag_dyneither(_tmpAB3,sizeof(void*),1)))))))),_tmpAB1)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAB7;void*_tmpAB6;(_tmpAB6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB7="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAB7,sizeof(char),43))),_tag_dyneither(_tmpAB6,sizeof(void*),0)));}{
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
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAB8;
return(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->f1=_tmp140,((_tmpAB8->f2=
_tmp141,((_tmpAB8->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAB8)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpAB9;return(_tmpAB9=
_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->f1=_tmp145,((_tmpAB9->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpAB9)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
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
_tmpABA;return(_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->name=f->name,((
_tmpABA->tq=Cyc_Toc_mt_tq,((_tmpABA->type=Cyc_Toc_typ_to_c(f->type),((_tmpABA->width=
f->width,((_tmpABA->attributes=f->attributes,_tmpABA)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpABB;cs=((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((
_tmpABB[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpABB))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABC;r=((_tmpABC=
_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABC))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABD;r=((_tmpABD=
_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABD))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
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
const char*_tmpABE;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpABE="_struct",_tag_dyneither(_tmpABE,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAC1;void*_tmpAC0;(_tmpAC0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC1="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAC1,sizeof(char),29))),_tag_dyneither(_tmpAC0,sizeof(void*),0)));}
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
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAC2;_tmp1A1=((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->hd=(void*)_tmp17F->hd,((_tmpAC2->tl=
_tmp1A1,_tmpAC2))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A8=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1A9=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAC3;struct _tuple7*_tmp1AA=(_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((
_tmpAC3->f1=_tmp17E->name,((_tmpAC3->f2=_tmp17E->tq,((_tmpAC3->f3=_tmp1A9,
_tmpAC3)))))));struct Cyc_List_List*_tmpAC4;_tmp1A8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A8,((
_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->hd=_tmp1AA,((_tmpAC4->tl=0,
_tmpAC4)))))));}{struct Cyc_Absyn_FnType_struct _tmpACA;struct Cyc_Absyn_FnInfo
_tmpAC9;struct Cyc_Absyn_FnType_struct*_tmpAC8;return(void*)((_tmpAC8=_cycalloc(
sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpACA.tag=10,((_tmpACA.f1=((_tmpAC9.tvars=0,((
_tmpAC9.effect=0,((_tmpAC9.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpAC9.args=
_tmp1A8,((_tmpAC9.c_varargs=_tmp17D,((_tmpAC9.cyc_varargs=0,((_tmpAC9.rgn_po=0,((
_tmpAC9.attributes=_tmp1A1,_tmpAC9)))))))))))))))),_tmpACA)))),_tmpAC8))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpACD;struct Cyc_Absyn_AnonAggrType_struct*
_tmpACC;return(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((
_tmpACD.tag=13,((_tmpACD.f1=_tmp183,((_tmpACD.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpACD)))))),_tmpACC))));}_LL5C: {
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
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpAD0;struct Cyc_Absyn_TypedefType_struct*
_tmpACF;return(void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((
_tmpAD0.tag=18,((_tmpAD0.f1=_tmp18E,((_tmpAD0.f2=0,((_tmpAD0.f3=_tmp190,((
_tmpAD0.f4=0,_tmpAD0)))))))))),_tmpACF))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAD6;void**_tmpAD5;struct Cyc_Absyn_TypedefType_struct*_tmpAD4;return(void*)((
_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD6.tag=18,((_tmpAD6.f1=
_tmp18E,((_tmpAD6.f2=0,((_tmpAD6.f3=_tmp190,((_tmpAD6.f4=((_tmpAD5=_cycalloc(
sizeof(*_tmpAD5)),((_tmpAD5[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAD5)))),
_tmpAD6)))))))))),_tmpAD4))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAD9;void*_tmpAD8;(_tmpAD8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD9="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAD9,sizeof(char),55))),_tag_dyneither(_tmpAD8,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpADC;void*_tmpADB;(
_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADC="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpADC,sizeof(char),57))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpADF;void*_tmpADE;(
_tmpADE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADF="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpADF,sizeof(char),49))),_tag_dyneither(_tmpADE,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAE2;void*_tmpAE1;(
_tmpAE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE2="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAE2,sizeof(char),51))),_tag_dyneither(_tmpAE1,sizeof(void*),0)));}_LL3D:;}
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
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpAE6;void*_tmpAE5[1];struct Cyc_String_pa_struct
_tmpAE4;(_tmpAE4.tag=0,((_tmpAE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAE5[0]=& _tmpAE4,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE6="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAE6,sizeof(char),25))),_tag_dyneither(_tmpAE5,sizeof(void*),1)))))));}
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
if(_tmp1FA->impl == 0){const char*_tmpAE9;void*_tmpAE8;(_tmpAE8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE9="is_poly_field: type missing fields",
_tag_dyneither(_tmpAE9,sizeof(char),35))),_tag_dyneither(_tmpAE8,sizeof(void*),0)));}
_tmp1F9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FA->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F4;if(_tmp1F8->tag != 13)goto _LLD0;else{_tmp1F9=_tmp1F8->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FD=Cyc_Absyn_lookup_field(_tmp1F9,f);if(_tmp1FD == 0){
const char*_tmpAED;void*_tmpAEC[1];struct Cyc_String_pa_struct _tmpAEB;(_tmpAEB.tag=
0,((_tmpAEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAEC[0]=&
_tmpAEB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAED="is_poly_field: bad field %s",_tag_dyneither(_tmpAED,sizeof(char),28))),
_tag_dyneither(_tmpAEC,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FD->type);}_LLD0:;_LLD1: {const char*_tmpAF1;void*_tmpAF0[1];struct Cyc_String_pa_struct
_tmpAEF;(_tmpAEF.tag=0,((_tmpAEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAF0[0]=& _tmpAEF,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF1="is_poly_field: bad type %s",
_tag_dyneither(_tmpAF1,sizeof(char),27))),_tag_dyneither(_tmpAF0,sizeof(void*),1)))))));}
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
_tmp20A);_LLDC:;_LLDD: {const char*_tmpAF5;void*_tmpAF4[1];struct Cyc_String_pa_struct
_tmpAF3;(_tmpAF3.tag=0,((_tmpAF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp209->topt))->v)),((
_tmpAF4[0]=& _tmpAF3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAF5="is_poly_project: bad type %s",_tag_dyneither(_tmpAF5,sizeof(char),29))),
_tag_dyneither(_tmpAF4,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAF6;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAF6=_cycalloc(
sizeof(*_tmpAF6)),((_tmpAF6->hd=s,((_tmpAF6->tl=0,_tmpAF6)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAF7;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAF7=_cycalloc(sizeof(*
_tmpAF7)),((_tmpAF7->hd=s,((_tmpAF7->tl=0,_tmpAF7)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAF8[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAF8[1]=s,((_tmpAF8[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAF9[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAF9[1]=n,((
_tmpAF9[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAFA[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAFA[1]=n,((
_tmpAFA[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAFA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAFB[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAFB[2]=n,((_tmpAFB[1]=s,((_tmpAFB[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFB,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAFC;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC->hd=e,((_tmpAFC->tl=0,_tmpAFC)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAFD;throw_match_stmt_opt=((
_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAFD))));}return*throw_match_stmt_opt;}static
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB03;struct
Cyc_Absyn_Var_d_struct _tmpB02;struct Cyc_List_List*_tmpB01;Cyc_Toc_result_decls=((
_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->hd=Cyc_Absyn_new_decl((void*)((
_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB02.tag=0,((_tmpB02.f1=vd,
_tmpB02)))),_tmpB03)))),0),((_tmpB01->tl=Cyc_Toc_result_decls,_tmpB01))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB0A;
struct _tuple15*_tmpB09;struct _tuple15*_tmpB08;struct _tuple15*_tmpB07[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB07[2]=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((
_tmpB08->f1=0,((_tmpB08->f2=xplussz,_tmpB08)))))),((_tmpB07[1]=((_tmpB09=
_cycalloc(sizeof(*_tmpB09)),((_tmpB09->f1=0,((_tmpB09->f2=xexp,_tmpB09)))))),((
_tmpB07[0]=((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->f1=0,((_tmpB0A->f2=
xexp,_tmpB0A)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB07,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
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
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB0B;return(_tmpB0B=_region_malloc(r,
sizeof(*_tmpB0B)),((_tmpB0B->break_lab=(struct _dyneither_ptr**)0,((_tmpB0B->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB0B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB0B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB0B->toplevel=(int)1,((_tmpB0B->rgn=(struct _RegionHandle*)r,_tmpB0B)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22D;struct _dyneither_ptr**_tmp22E;struct _dyneither_ptr**
_tmp22F;struct Cyc_Toc_FallthruInfo*_tmp230;struct Cyc_Dict_Dict _tmp231;int _tmp232;
struct Cyc_Toc_Env*_tmp22C=e;_tmp22D=*_tmp22C;_tmp22E=_tmp22D.break_lab;_tmp22F=
_tmp22D.continue_lab;_tmp230=_tmp22D.fallthru_info;_tmp231=_tmp22D.varmap;
_tmp232=_tmp22D.toplevel;{struct Cyc_Toc_Env*_tmpB0C;return(_tmpB0C=
_region_malloc(r,sizeof(*_tmpB0C)),((_tmpB0C->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22E),((_tmpB0C->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpB0C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp230,((_tmpB0C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp231),((_tmpB0C->toplevel=(
int)_tmp232,((_tmpB0C->rgn=(struct _RegionHandle*)r,_tmpB0C)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp235;struct _dyneither_ptr**_tmp236;struct
_dyneither_ptr**_tmp237;struct Cyc_Toc_FallthruInfo*_tmp238;struct Cyc_Dict_Dict
_tmp239;int _tmp23A;struct Cyc_Toc_Env*_tmp234=e;_tmp235=*_tmp234;_tmp236=_tmp235.break_lab;
_tmp237=_tmp235.continue_lab;_tmp238=_tmp235.fallthru_info;_tmp239=_tmp235.varmap;
_tmp23A=_tmp235.toplevel;{struct Cyc_Toc_Env*_tmpB0D;return(_tmpB0D=
_region_malloc(r,sizeof(*_tmpB0D)),((_tmpB0D->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp236),((_tmpB0D->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpB0D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp238,((_tmpB0D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp239),((_tmpB0D->toplevel=(
int)0,((_tmpB0D->rgn=(struct _RegionHandle*)r,_tmpB0D)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23D;struct _dyneither_ptr**_tmp23E;struct _dyneither_ptr**
_tmp23F;struct Cyc_Toc_FallthruInfo*_tmp240;struct Cyc_Dict_Dict _tmp241;int _tmp242;
struct Cyc_Toc_Env*_tmp23C=e;_tmp23D=*_tmp23C;_tmp23E=_tmp23D.break_lab;_tmp23F=
_tmp23D.continue_lab;_tmp240=_tmp23D.fallthru_info;_tmp241=_tmp23D.varmap;
_tmp242=_tmp23D.toplevel;{struct Cyc_Toc_Env*_tmpB0E;return(_tmpB0E=
_region_malloc(r,sizeof(*_tmpB0E)),((_tmpB0E->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23E),((_tmpB0E->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpB0E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp240,((_tmpB0E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp241),((_tmpB0E->toplevel=(
int)1,((_tmpB0E->rgn=(struct _RegionHandle*)r,_tmpB0E)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp244=(*x).f1;struct Cyc_List_List*_tmp245;_LLE4: if((
_tmp244.Rel_n).tag != 1)goto _LLE6;_tmp245=(struct Cyc_List_List*)(_tmp244.Rel_n).val;
_LLE5: {const char*_tmpB12;void*_tmpB11[1];struct Cyc_String_pa_struct _tmpB10;(
_tmpB10.tag=0,((_tmpB10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB11[0]=& _tmpB10,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB12="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB12,sizeof(char),30))),_tag_dyneither(_tmpB11,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp24A;struct _dyneither_ptr**_tmp24B;
struct _dyneither_ptr**_tmp24C;struct Cyc_Toc_FallthruInfo*_tmp24D;struct Cyc_Dict_Dict
_tmp24E;int _tmp24F;struct Cyc_Toc_Env*_tmp249=e;_tmp24A=*_tmp249;_tmp24B=_tmp24A.break_lab;
_tmp24C=_tmp24A.continue_lab;_tmp24D=_tmp24A.fallthru_info;_tmp24E=_tmp24A.varmap;
_tmp24F=_tmp24A.toplevel;{struct Cyc_Dict_Dict _tmp250=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24E),x,y);struct Cyc_Toc_Env*_tmpB13;return(_tmpB13=_region_malloc(r,
sizeof(*_tmpB13)),((_tmpB13->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24B),((_tmpB13->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24C),((_tmpB13->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24D,((_tmpB13->varmap=(struct Cyc_Dict_Dict)_tmp250,((_tmpB13->toplevel=(int)
_tmp24F,((_tmpB13->rgn=(struct _RegionHandle*)r,_tmpB13)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp253;struct _dyneither_ptr**_tmp254;struct _dyneither_ptr**
_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;
struct Cyc_Toc_Env*_tmp252=e;_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=
_tmp253.continue_lab;_tmp256=_tmp253.fallthru_info;_tmp257=_tmp253.varmap;
_tmp258=_tmp253.toplevel;{struct Cyc_Toc_Env*_tmpB14;return(_tmpB14=
_region_malloc(r,sizeof(*_tmpB14)),((_tmpB14->break_lab=(struct _dyneither_ptr**)
0,((_tmpB14->continue_lab=(struct _dyneither_ptr**)0,((_tmpB14->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpB14->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp257),((_tmpB14->toplevel=(int)_tmp258,((_tmpB14->rgn=(struct _RegionHandle*)
r,_tmpB14)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB15;fallthru_vars=((
_tmpB15=_region_malloc(r,sizeof(*_tmpB15)),((_tmpB15->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB15->tl=fallthru_vars,_tmpB15))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25C;struct _dyneither_ptr**_tmp25D;struct
_dyneither_ptr**_tmp25E;struct Cyc_Toc_FallthruInfo*_tmp25F;struct Cyc_Dict_Dict
_tmp260;int _tmp261;struct Cyc_Toc_Env*_tmp25B=e;_tmp25C=*_tmp25B;_tmp25D=_tmp25C.break_lab;
_tmp25E=_tmp25C.continue_lab;_tmp25F=_tmp25C.fallthru_info;_tmp260=_tmp25C.varmap;
_tmp261=_tmp25C.toplevel;{struct Cyc_Toc_Env _tmp263;struct Cyc_Dict_Dict _tmp264;
struct Cyc_Toc_Env*_tmp262=next_case_env;_tmp263=*_tmp262;_tmp264=_tmp263.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB16;struct Cyc_Toc_FallthruInfo*fi=(_tmpB16=
_region_malloc(r,sizeof(*_tmpB16)),((_tmpB16->label=fallthru_l,((_tmpB16->binders=
fallthru_vars,((_tmpB16->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp264),_tmpB16)))))));
struct _dyneither_ptr**_tmpB19;struct Cyc_Toc_Env*_tmpB18;return(_tmpB18=
_region_malloc(r,sizeof(*_tmpB18)),((_tmpB18->break_lab=(struct _dyneither_ptr**)((
_tmpB19=_region_malloc(r,sizeof(*_tmpB19)),((_tmpB19[0]=break_l,_tmpB19)))),((
_tmpB18->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25E),((
_tmpB18->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB18->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp260),((_tmpB18->toplevel=(int)_tmp261,((_tmpB18->rgn=(
struct _RegionHandle*)r,_tmpB18)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp269;struct _dyneither_ptr**
_tmp26A;struct _dyneither_ptr**_tmp26B;struct Cyc_Toc_FallthruInfo*_tmp26C;struct
Cyc_Dict_Dict _tmp26D;int _tmp26E;struct Cyc_Toc_Env*_tmp268=e;_tmp269=*_tmp268;
_tmp26A=_tmp269.break_lab;_tmp26B=_tmp269.continue_lab;_tmp26C=_tmp269.fallthru_info;
_tmp26D=_tmp269.varmap;_tmp26E=_tmp269.toplevel;{struct _dyneither_ptr**_tmpB1C;
struct Cyc_Toc_Env*_tmpB1B;return(_tmpB1B=_region_malloc(r,sizeof(*_tmpB1B)),((
_tmpB1B->break_lab=(struct _dyneither_ptr**)((_tmpB1C=_region_malloc(r,sizeof(*
_tmpB1C)),((_tmpB1C[0]=break_l,_tmpB1C)))),((_tmpB1B->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpB1B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB1B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26D),((_tmpB1B->toplevel=(
int)_tmp26E,((_tmpB1B->rgn=(struct _RegionHandle*)r,_tmpB1B)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp272;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*
_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;struct Cyc_Toc_Env*_tmp271=e;
_tmp272=*_tmp271;_tmp273=_tmp272.break_lab;_tmp274=_tmp272.continue_lab;_tmp275=
_tmp272.fallthru_info;_tmp276=_tmp272.varmap;_tmp277=_tmp272.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB1F;struct Cyc_Toc_Env*_tmpB1E;return(_tmpB1E=_region_malloc(r,sizeof(*
_tmpB1E)),((_tmpB1E->break_lab=(struct _dyneither_ptr**)0,((_tmpB1E->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp274),((_tmpB1E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB1F=_region_malloc(r,sizeof(*_tmpB1F)),((
_tmpB1F->label=next_l,((_tmpB1F->binders=0,((_tmpB1F->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB1F)))))))),((_tmpB1E->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp276),((_tmpB1E->toplevel=(int)_tmp277,((_tmpB1E->rgn=(struct _RegionHandle*)
r,_tmpB1E)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27A=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27B=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27A;if(_tmp27B->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27C=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27A;if(_tmp27C->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27A;if(_tmp27D->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB22;void*_tmpB21;(_tmpB21=0,Cyc_Tcutil_terr(e->loc,((
_tmpB22="dereference of NULL pointer",_tag_dyneither(_tmpB22,sizeof(char),28))),
_tag_dyneither(_tmpB21,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27E=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27A;if(_tmp27E->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27F=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp27A;if(_tmp27F->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB25;void*_tmpB24;(_tmpB24=0,Cyc_Tcutil_warn(e->loc,((
_tmpB25="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB25,
sizeof(char),46))),_tag_dyneither(_tmpB24,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB28;void*_tmpB27;(_tmpB27=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB28="need_null_check",_tag_dyneither(
_tmpB28,sizeof(char),16))),_tag_dyneither(_tmpB27,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp286=e->annot;
struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp28A;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp287=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp286;if(_tmp287->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp288=_tmp287->f1;}}_LLF7: return _tmp288;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp286;if(_tmp289->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp28A=_tmp289->f1;}}_LLF9: return _tmp28A;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp28B=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp286;if(_tmp28B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,Cyc_Tcutil_terr(e->loc,((
_tmpB2B="dereference of NULL pointer",_tag_dyneither(_tmpB2B,sizeof(char),28))),
_tag_dyneither(_tmpB2A,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28C=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp286;if(_tmp28C->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28D=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp286;if(_tmp28D->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB2E;void*_tmpB2D;(
_tmpB2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2E="get_relns",_tag_dyneither(_tmpB2E,sizeof(char),10))),_tag_dyneither(
_tmpB2D,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
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
struct _tuple17 _tmpB2F;struct _tuple17 _tmp327=(_tmpB2F.f1=Cyc_Tcutil_compress(
_tmp323),((_tmpB2F.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB2F)));void*_tmp328;void*_tmp32A;void*_tmp32B;
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
_tmpB32;void*_tmpB31;(_tmpB31=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB32="Missing type in primop ",
_tag_dyneither(_tmpB32,sizeof(char),24))),_tag_dyneither(_tmpB31,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB35;void*_tmpB34;(_tmpB34=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB35="Missing type in primop ",_tag_dyneither(_tmpB35,sizeof(char),24))),
_tag_dyneither(_tmpB34,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*
_tmpB36;return(_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->f1=Cyc_Toc_mt_tq,((
_tmpB36->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB36)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB37;return(
_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->f1=0,((_tmpB37->f2=e,_tmpB37)))));};}
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
count --,0);else{if(_tmp347->tl != 0){const char*_tmpB3A;void*_tmpB39;(_tmpB39=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB3A="multiple designators in array",
_tag_dyneither(_tmpB3A,sizeof(char),30))),_tag_dyneither(_tmpB39,sizeof(void*),0)));}{
void*_tmp34B=(void*)_tmp347->hd;void*_tmp34C=_tmp34B;struct Cyc_Absyn_Exp*_tmp34E;
_LL176: {struct Cyc_Absyn_ArrayElement_struct*_tmp34D=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp34C;if(_tmp34D->tag != 0)goto _LL178;else{_tmp34E=_tmp34D->f1;}}_LL177: Cyc_Toc_exp_to_c(
nv,_tmp34E);e_index=_tmp34E;goto _LL175;_LL178: {struct Cyc_Absyn_FieldName_struct*
_tmp34F=(struct Cyc_Absyn_FieldName_struct*)_tmp34C;if(_tmp34F->tag != 1)goto
_LL175;}_LL179: {const char*_tmpB3D;void*_tmpB3C;(_tmpB3C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB3D="field name designators in array",
_tag_dyneither(_tmpB3D,sizeof(char),32))),_tag_dyneither(_tmpB3C,sizeof(void*),0)));}
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
_tmpB40;struct Cyc_Absyn_Local_b_struct*_tmpB3F;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp361,nv,_tmp35D,Cyc_Absyn_varb_exp(_tmp35D,(void*)((_tmpB3F=_cycalloc(sizeof(*
_tmpB3F)),((_tmpB3F[0]=((_tmpB40.tag=4,((_tmpB40.f1=vd,_tmpB40)))),_tmpB3F)))),0));
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
if(_tmp372 == 0){const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="empty designator list",
_tag_dyneither(_tmpB43,sizeof(char),22))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}
if(_tmp372->tl != 0){const char*_tmpB46;void*_tmpB45;(_tmpB45=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB46="too many designators in anonymous struct",
_tag_dyneither(_tmpB46,sizeof(char),41))),_tag_dyneither(_tmpB45,sizeof(void*),0)));}{
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
_tmpB49;void*_tmpB48;(_tmpB48=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB49="array designator in struct",
_tag_dyneither(_tmpB49,sizeof(char),27))),_tag_dyneither(_tmpB48,sizeof(void*),0)));}
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
struct Cyc_String_pa_struct _tmpB51;void*_tmpB50[1];const char*_tmpB4F;void*_tmpB4E;(
_tmpB4E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB51.tag=0,((_tmpB51.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB50[0]=& _tmpB51,Cyc_aprintf(((_tmpB4F="get_member_offset %s failed",
_tag_dyneither(_tmpB4F,sizeof(char),28))),_tag_dyneither(_tmpB50,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB4E,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
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
const char*_tmpB54;void*_tmpB53;(_tmpB53=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB54="init_struct: bad struct type",
_tag_dyneither(_tmpB54,sizeof(char),29))),_tag_dyneither(_tmpB53,sizeof(void*),0)));}
_LL1A1:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3AC;struct _RegionHandle*_tmp3AD;struct Cyc_Toc_Env*_tmp3AB=
nv;_tmp3AC=*_tmp3AB;_tmp3AD=_tmp3AC.rgn;{struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AD,dles);for(0;
_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){struct _tuple15 _tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;struct _tuple15*_tmp3AF=(struct _tuple15*)
_tmp3AE->hd;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B2->topt))->v);
if(_tmp3B1 == 0){const char*_tmpB57;void*_tmpB56;(_tmpB56=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="empty designator list",
_tag_dyneither(_tmpB57,sizeof(char),22))),_tag_dyneither(_tmpB56,sizeof(void*),0)));}
if(_tmp3B1->tl != 0){struct _tuple0*_tmp3B5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B6=Cyc_Absyn_var_exp(_tmp3B5,0);for(0;_tmp3B1 != 0;_tmp3B1=_tmp3B1->tl){void*
_tmp3B7=(void*)_tmp3B1->hd;struct _dyneither_ptr*_tmp3B9;_LL1A7: {struct Cyc_Absyn_FieldName_struct*
_tmp3B8=(struct Cyc_Absyn_FieldName_struct*)_tmp3B7;if(_tmp3B8->tag != 1)goto
_LL1A9;else{_tmp3B9=_tmp3B8->f1;}}_LL1A8: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B9))_tmp3B6=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B6);_tmp3A2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A3(_tmp3A1,_tmp3B9,0),_tmp3B6,0),0),
_tmp3A2,0);goto _LL1A6;_LL1A9:;_LL1AA: {const char*_tmpB5A;void*_tmpB59;(_tmpB59=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB5A="array designator in struct",_tag_dyneither(_tmpB5A,sizeof(char),27))),
_tag_dyneither(_tmpB59,sizeof(void*),0)));}_LL1A6:;}Cyc_Toc_exp_to_c(nv,_tmp3B2);
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
const char*_tmpB5D;void*_tmpB5C;(_tmpB5C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5D="array designator in struct",
_tag_dyneither(_tmpB5D,sizeof(char),27))),_tag_dyneither(_tmpB5C,sizeof(void*),0)));}
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
struct Cyc_Absyn_Increment_e_struct _tmpB60;struct Cyc_Absyn_Increment_e_struct*
_tmpB5F;return Cyc_Absyn_new_exp((void*)((_tmpB5F=_cycalloc(sizeof(*_tmpB5F)),((
_tmpB5F[0]=((_tmpB60.tag=5,((_tmpB60.f1=e1,((_tmpB60.f2=incr,_tmpB60)))))),
_tmpB5F)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
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
_tmpB61;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB61=_cycalloc(
sizeof(*_tmpB61)),((_tmpB61->hd=_tmp3D7,((_tmpB61->tl=fs,_tmpB61)))))),f,f_env);}
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
_tmp3E2,fs,f,f_env);goto _LL1C2;_LL1C9:;_LL1CA: {const char*_tmpB65;void*_tmpB64[1];
struct Cyc_String_pa_struct _tmpB63;(_tmpB63.tag=0,((_tmpB63.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB64[0]=&
_tmpB63,Cyc_Toc_toc_impos(((_tmpB65="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB65,sizeof(char),23))),_tag_dyneither(_tmpB64,sizeof(void*),1)))))));}_LL1C2:;}
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
char*_tmpB69;void*_tmpB68[1];struct Cyc_String_pa_struct _tmpB67;(_tmpB67.tag=0,((
_tmpB67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB68[0]=& _tmpB67,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB69="can't take & of %s",_tag_dyneither(_tmpB69,sizeof(char),19))),
_tag_dyneither(_tmpB68,sizeof(void*),1)))))));};_LL1CB:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB6A;result=((
_tmpB6A=_region_malloc(r2,sizeof(*_tmpB6A)),((_tmpB6A->hd=(void*)f((void*)x->hd,
env),((_tmpB6A->tl=0,_tmpB6A))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB6B;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB6B=_region_malloc(r2,sizeof(*_tmpB6B)),((_tmpB6B->hd=(void*)f((void*)x->hd,
env),((_tmpB6B->tl=0,_tmpB6B))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple15*_tmpB6C;
return(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->f1=0,((_tmpB6C->f2=e,
_tmpB6C)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3F4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3F6;_LL1D3: {struct Cyc_Absyn_PointerType_struct*
_tmp3F5=(struct Cyc_Absyn_PointerType_struct*)_tmp3F4;if(_tmp3F5->tag != 5)goto
_LL1D5;else{_tmp3F6=_tmp3F5->f1;}}_LL1D4: return _tmp3F6;_LL1D5:;_LL1D6: {const
char*_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6F="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB6F,sizeof(char),28))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}
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
_LL1EC;}_LL1EB:{const char*_tmpB70;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB70="0.0",_tag_dyneither(_tmpB70,sizeof(char),4)))),0);}goto _LL1D7;_LL1EC:;
_LL1ED: {const char*_tmpB74;void*_tmpB73[1];struct Cyc_String_pa_struct _tmpB72;(
_tmpB72.tag=0,((_tmpB72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB73[0]=& _tmpB72,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB74="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB74,sizeof(char),40))),_tag_dyneither(_tmpB73,sizeof(void*),1)))))));}
_LL1D7:;}{struct Cyc_Core_Opt*_tmpB75;res->topt=((_tmpB75=_cycalloc(sizeof(*
_tmpB75)),((_tmpB75->v=t,_tmpB75))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp413=Cyc_Toc_typ_to_c(elt_type);void*_tmp414=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp415=Cyc_Absyn_cstar_typ(_tmp413,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB76;struct Cyc_Core_Opt*_tmp416=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((
_tmpB76->v=_tmp415,_tmpB76)));struct Cyc_Absyn_Exp*xinit;{void*_tmp417=e1->r;
struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*
_tmp41C;_LL1EF: {struct Cyc_Absyn_Deref_e_struct*_tmp418=(struct Cyc_Absyn_Deref_e_struct*)
_tmp417;if(_tmp418->tag != 21)goto _LL1F1;else{_tmp419=_tmp418->f1;}}_LL1F0: if(!
is_dyneither){_tmp419=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp419,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB77;_tmp419->topt=((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((
_tmpB77->v=fat_ptr_type,_tmpB77))));};}Cyc_Toc_exp_to_c(nv,_tmp419);xinit=
_tmp419;goto _LL1EE;_LL1F1: {struct Cyc_Absyn_Subscript_e_struct*_tmp41A=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp417;if(_tmp41A->tag != 24)goto _LL1F3;else{
_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL1F2: if(!is_dyneither){_tmp41B=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp41B,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB78;
_tmp41B->topt=((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->v=fat_ptr_type,
_tmpB78))));};}Cyc_Toc_exp_to_c(nv,_tmp41B);Cyc_Toc_exp_to_c(nv,_tmp41C);{struct
Cyc_Absyn_Exp*_tmpB79[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB79[2]=_tmp41C,((_tmpB79[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB79[0]=_tmp41B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB79,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EE;_LL1F3:;_LL1F4: {const char*_tmpB7C;void*_tmpB7B;(_tmpB7B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7C="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB7C,sizeof(char),53))),_tag_dyneither(_tmpB7B,sizeof(void*),0)));}
_LL1EE:;}{struct _tuple0*_tmp422=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp424;
struct _RegionHandle*_tmp425;struct Cyc_Toc_Env*_tmp423=nv;_tmp424=*_tmp423;
_tmp425=_tmp424.rgn;{struct Cyc_Toc_Env*_tmp426=Cyc_Toc_add_varmap(_tmp425,nv,
_tmp422,Cyc_Absyn_var_exp(_tmp422,0));struct Cyc_Absyn_Vardecl*_tmpB7D;struct Cyc_Absyn_Vardecl*
_tmp427=(_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->sc=Cyc_Absyn_Public,((
_tmpB7D->name=_tmp422,((_tmpB7D->tq=Cyc_Toc_mt_tq,((_tmpB7D->type=_tmp414,((
_tmpB7D->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB7D->rgn=0,((_tmpB7D->attributes=
0,((_tmpB7D->escapes=0,_tmpB7D)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB80;struct Cyc_Absyn_Local_b_struct*_tmpB7F;struct Cyc_Absyn_Local_b_struct*
_tmp428=(_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB80.tag=4,((
_tmpB80.f1=_tmp427,_tmpB80)))),_tmpB7F)));struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_varb_exp(
_tmp422,(void*)_tmp428,0);{struct Cyc_Core_Opt*_tmpB81;_tmp429->topt=((_tmpB81=
_cycalloc(sizeof(*_tmpB81)),((_tmpB81->v=fat_ptr_type,_tmpB81))));}{struct Cyc_Absyn_Exp*
_tmp42B=Cyc_Absyn_deref_exp(_tmp429,0);{struct Cyc_Core_Opt*_tmpB82;_tmp42B->topt=((
_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->v=elt_type,_tmpB82))));}Cyc_Toc_exp_to_c(
_tmp426,_tmp42B);{struct _tuple0*_tmp42D=Cyc_Toc_temp_var();_tmp426=Cyc_Toc_add_varmap(
_tmp425,_tmp426,_tmp42D,Cyc_Absyn_var_exp(_tmp42D,0));{struct Cyc_Absyn_Vardecl*
_tmpB83;struct Cyc_Absyn_Vardecl*_tmp42E=(_tmpB83=_cycalloc(sizeof(*_tmpB83)),((
_tmpB83->sc=Cyc_Absyn_Public,((_tmpB83->name=_tmp42D,((_tmpB83->tq=Cyc_Toc_mt_tq,((
_tmpB83->type=_tmp413,((_tmpB83->initializer=(struct Cyc_Absyn_Exp*)_tmp42B,((
_tmpB83->rgn=0,((_tmpB83->attributes=0,((_tmpB83->escapes=0,_tmpB83)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB86;struct Cyc_Absyn_Local_b_struct*_tmpB85;
struct Cyc_Absyn_Local_b_struct*_tmp42F=(_tmpB85=_cycalloc(sizeof(*_tmpB85)),((
_tmpB85[0]=((_tmpB86.tag=4,((_tmpB86.f1=_tmp42E,_tmpB86)))),_tmpB85)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_varb_exp(_tmp42D,(
void*)_tmp42F,0);_tmp430->topt=_tmp42B->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp430,e2,0);z_init->topt=_tmp430->topt;}Cyc_Toc_exp_to_c(
_tmp426,z_init);{struct _tuple0*_tmp431=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB87;struct Cyc_Absyn_Vardecl*_tmp432=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((
_tmpB87->sc=Cyc_Absyn_Public,((_tmpB87->name=_tmp431,((_tmpB87->tq=Cyc_Toc_mt_tq,((
_tmpB87->type=_tmp413,((_tmpB87->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB87->rgn=0,((_tmpB87->attributes=0,((_tmpB87->escapes=0,_tmpB87)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB8A;struct Cyc_Absyn_Local_b_struct*_tmpB89;
struct Cyc_Absyn_Local_b_struct*_tmp433=(_tmpB89=_cycalloc(sizeof(*_tmpB89)),((
_tmpB89[0]=((_tmpB8A.tag=4,((_tmpB8A.f1=_tmp432,_tmpB8A)))),_tmpB89)));_tmp426=
Cyc_Toc_add_varmap(_tmp425,_tmp426,_tmp431,Cyc_Absyn_var_exp(_tmp431,0));{struct
Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_varb_exp(_tmp42D,(void*)_tmp42F,0);_tmp434->topt=
_tmp42B->topt;{struct Cyc_Absyn_Exp*_tmp435=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp434,_tmp435,0);{
struct Cyc_Core_Opt*_tmpB8B;_tmp436->topt=((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((
_tmpB8B->v=Cyc_Absyn_sint_typ,_tmpB8B))));}Cyc_Toc_exp_to_c(_tmp426,_tmp436);{
struct Cyc_Absyn_Exp*_tmp438=Cyc_Absyn_varb_exp(_tmp431,(void*)_tmp433,0);_tmp438->topt=
_tmp42B->topt;{struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp438,_tmp439,0);{
struct Cyc_Core_Opt*_tmpB8C;_tmp43A->topt=((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((
_tmpB8C->v=Cyc_Absyn_sint_typ,_tmpB8C))));}Cyc_Toc_exp_to_c(_tmp426,_tmp43A);{
struct Cyc_Absyn_Exp*_tmpB8D[2];struct Cyc_List_List*_tmp43C=(_tmpB8D[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB8D[0]=Cyc_Absyn_varb_exp(_tmp422,(void*)
_tmp428,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB8D,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
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
0),0),_tmp443,0);{struct Cyc_Absyn_Var_d_struct*_tmpB93;struct Cyc_Absyn_Var_d_struct
_tmpB92;struct Cyc_Absyn_Decl*_tmpB91;_tmp443=Cyc_Absyn_decl_stmt(((_tmpB91=
_cycalloc(sizeof(*_tmpB91)),((_tmpB91->r=(void*)((_tmpB93=_cycalloc(sizeof(*
_tmpB93)),((_tmpB93[0]=((_tmpB92.tag=0,((_tmpB92.f1=_tmp432,_tmpB92)))),_tmpB93)))),((
_tmpB91->loc=0,_tmpB91)))))),_tmp443,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB99;
struct Cyc_Absyn_Var_d_struct _tmpB98;struct Cyc_Absyn_Decl*_tmpB97;_tmp443=Cyc_Absyn_decl_stmt(((
_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97->r=(void*)((_tmpB99=_cycalloc(
sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB98.tag=0,((_tmpB98.f1=_tmp42E,_tmpB98)))),
_tmpB99)))),((_tmpB97->loc=0,_tmpB97)))))),_tmp443,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB9F;struct Cyc_Absyn_Var_d_struct _tmpB9E;struct Cyc_Absyn_Decl*_tmpB9D;_tmp443=
Cyc_Absyn_decl_stmt(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->r=(void*)((
_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F[0]=((_tmpB9E.tag=0,((_tmpB9E.f1=
_tmp427,_tmpB9E)))),_tmpB9F)))),((_tmpB9D->loc=0,_tmpB9D)))))),_tmp443,0);}e->r=
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
_tmp45B);goto _LL1F5;_LL1F8:;_LL1F9: {struct Cyc_String_pa_struct _tmpBA7;void*
_tmpBA6[1];const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBA6[0]=& _tmpBA7,Cyc_aprintf(((
_tmpBA5="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBA5,sizeof(char),51))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBA4,sizeof(void*),0)));}_LL1F5:;}{struct _tuple0*_tmp460=Cyc_Toc_temp_var();
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
else{_tmp46B=_tmp46A->f2;}}_LL1FC: {const char*_tmpBAA;void*_tmpBA9;(_tmpBA9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAA="cast on lhs!",_tag_dyneither(_tmpBAA,sizeof(char),13))),_tag_dyneither(
_tmpBA9,sizeof(void*),0)));}_LL1FD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp46C=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp469;if(_tmp46C->tag != 22)goto _LL1FF;
else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;_tmp46F=_tmp46C->f4;_tmp470=(int*)&
_tmp46C->f4;}}_LL1FE: {void*_tmp478=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46D->topt))->v);struct Cyc_Absyn_AggrInfo _tmp47A;union Cyc_Absyn_AggrInfoU
_tmp47B;_LL204: {struct Cyc_Absyn_AggrType_struct*_tmp479=(struct Cyc_Absyn_AggrType_struct*)
_tmp478;if(_tmp479->tag != 12)goto _LL206;else{_tmp47A=_tmp479->f1;_tmp47B=_tmp47A.aggr_info;}}
_LL205: {struct Cyc_Absyn_Aggrdecl*_tmp47C=Cyc_Absyn_get_known_aggrdecl(_tmp47B);*
f_tag=Cyc_Toc_get_member_offset(_tmp47C,_tmp46E);{const char*_tmpBAF;void*_tmpBAE[
2];struct Cyc_String_pa_struct _tmpBAD;struct Cyc_String_pa_struct _tmpBAC;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBAC.tag=0,((_tmpBAC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp46E),((_tmpBAD.tag=0,((_tmpBAD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47C->name).f2),((_tmpBAE[0]=&
_tmpBAD,((_tmpBAE[1]=& _tmpBAC,Cyc_aprintf(((_tmpBAF="_union_%s_%s",
_tag_dyneither(_tmpBAF,sizeof(char),13))),_tag_dyneither(_tmpBAE,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBB0;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB0=
_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=str,_tmpBB0)))));}if(clear_read)*
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
_tmp48A,_tmp473);{const char*_tmpBB5;void*_tmpBB4[2];struct Cyc_String_pa_struct
_tmpBB3;struct Cyc_String_pa_struct _tmpBB2;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp473),((_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp48A->name).f2),((_tmpBB4[0]=& _tmpBB3,((_tmpBB4[1]=&
_tmpBB2,Cyc_aprintf(((_tmpBB5="_union_%s_%s",_tag_dyneither(_tmpBB5,sizeof(char),
13))),_tag_dyneither(_tmpBB4,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBB6;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((
_tmpBB6[0]=str,_tmpBB6)))));}if(clear_read)*_tmp475=0;return((struct Cyc_Absyn_AggrdeclImpl*)
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
static void _tmpC15(unsigned int*_tmpC14,unsigned int*_tmpC13,struct _tuple0***
_tmpC11){for(*_tmpC14=0;*_tmpC14 < *_tmpC13;(*_tmpC14)++){(*_tmpC11)[*_tmpC14]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp492=e->r;if(e->topt == 0){const char*_tmpBBA;void*_tmpBB9[1];struct Cyc_String_pa_struct
_tmpBB8;(_tmpBB8.tag=0,((_tmpBB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBB9[0]=& _tmpBB8,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBA="exp_to_c: no type for %s",
_tag_dyneither(_tmpBBA,sizeof(char),25))),_tag_dyneither(_tmpBB9,sizeof(void*),1)))))));}{
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
_tmp51E))->r;else{struct Cyc_Absyn_Exp*_tmpBBB[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBBB[2]=_tmp51E,((_tmpBBB[1]=_tmp51E,((_tmpBBB[0]=_tmp51E,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL212;}_LL215: {struct Cyc_Absyn_Const_e_struct*
_tmp49A=(struct Cyc_Absyn_Const_e_struct*)_tmp496;if(_tmp49A->tag != 0)goto _LL217;}
_LL216: goto _LL212;_LL217: {struct Cyc_Absyn_Var_e_struct*_tmp49B=(struct Cyc_Absyn_Var_e_struct*)
_tmp496;if(_tmp49B->tag != 1)goto _LL219;else{_tmp49C=_tmp49B->f1;_tmp49D=(void*)
_tmp49B->f2;}}_LL218:{struct _handler_cons _tmp520;_push_handler(& _tmp520);{int
_tmp522=0;if(setjmp(_tmp520.handler))_tmp522=1;if(!_tmp522){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp49C))->r;;_pop_handler();}else{void*_tmp521=(void*)_exn_thrown;void*
_tmp524=_tmp521;_LL26A: {struct Cyc_Dict_Absent_struct*_tmp525=(struct Cyc_Dict_Absent_struct*)
_tmp524;if(_tmp525->tag != Cyc_Dict_Absent)goto _LL26C;}_LL26B: {const char*_tmpBBF;
void*_tmpBBE[1];struct Cyc_String_pa_struct _tmpBBD;(_tmpBBD.tag=0,((_tmpBBD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp49C)),((
_tmpBBE[0]=& _tmpBBD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBF="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBBF,sizeof(char),
32))),_tag_dyneither(_tmpBBE,sizeof(void*),1)))))));}_LL26C:;_LL26D:(void)_throw(
_tmp524);_LL269:;}};}goto _LL212;_LL219: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp49E=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp496;if(_tmp49E->tag != 2)goto
_LL21B;else{_tmp49F=_tmp49E->f1;}}_LL21A: {const char*_tmpBC2;void*_tmpBC1;(
_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC2="unknownid",_tag_dyneither(_tmpBC2,sizeof(char),10))),_tag_dyneither(
_tmpBC1,sizeof(void*),0)));}_LL21B: {struct Cyc_Absyn_Primop_e_struct*_tmp4A0=(
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
struct Cyc_Absyn_Exp*)_check_null(_tmp52F))){const char*_tmpBC5;void*_tmpBC4;(
_tmpBC4=0,Cyc_Tcutil_terr(e->loc,((_tmpBC5="can't calculate numelts",
_tag_dyneither(_tmpBC5,sizeof(char),24))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
e->r=_tmp52F->r;goto _LL270;_LL273: {struct Cyc_Absyn_PointerType_struct*_tmp530=(
struct Cyc_Absyn_PointerType_struct*)_tmp52C;if(_tmp530->tag != 5)goto _LL275;else{
_tmp531=_tmp530->f1;_tmp532=_tmp531.elt_typ;_tmp533=_tmp531.ptr_atts;_tmp534=
_tmp533.nullable;_tmp535=_tmp533.bounds;_tmp536=_tmp533.zero_term;}}_LL274:{void*
_tmp539=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp535);struct Cyc_Absyn_Exp*_tmp53C;_LL278: {struct Cyc_Absyn_DynEither_b_struct*
_tmp53A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp539;if(_tmp53A->tag != 0)goto
_LL27A;}_LL279:{struct Cyc_Absyn_Exp*_tmpBC6[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBC6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp532),0),((_tmpBC6[0]=(
struct Cyc_Absyn_Exp*)_tmp4A2->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBC6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL277;_LL27A: {struct Cyc_Absyn_Upper_b_struct*_tmp53B=(struct Cyc_Absyn_Upper_b_struct*)
_tmp539;if(_tmp53B->tag != 1)goto _LL277;else{_tmp53C=_tmp53B->f1;}}_LL27B: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp536)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4A2->hd);struct Cyc_Absyn_Exp*_tmpBC7[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBC7[1]=_tmp53C,((_tmpBC7[0]=(struct Cyc_Absyn_Exp*)_tmp4A2->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBC7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp534)){if(!Cyc_Evexp_c_can_eval(_tmp53C)){const
char*_tmpBCA;void*_tmpBC9;(_tmpBC9=0,Cyc_Tcutil_terr(e->loc,((_tmpBCA="can't calculate numelts",
_tag_dyneither(_tmpBCA,sizeof(char),24))),_tag_dyneither(_tmpBC9,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp53C,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp53C->r;goto _LL277;}}goto _LL277;_LL277:;}goto _LL270;_LL275:;_LL276: {const
char*_tmpBCF;void*_tmpBCE[2];struct Cyc_String_pa_struct _tmpBCD;struct Cyc_String_pa_struct
_tmpBCC;(_tmpBCC.tag=0,((_tmpBCC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBCD.tag=0,((_tmpBCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBCE[0]=& _tmpBCD,((
_tmpBCE[1]=& _tmpBCC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBCF="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBCF,
sizeof(char),41))),_tag_dyneither(_tmpBCE,sizeof(void*),2)))))))))))));}_LL270:;}
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
Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBD0[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD0[2]=e2,((_tmpBD0[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp548),0),((_tmpBD0[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL282;}_LL285: {struct Cyc_Absyn_Upper_b_struct*_tmp54E=(struct
Cyc_Absyn_Upper_b_struct*)_tmp54C;if(_tmp54E->tag != 1)goto _LL282;else{_tmp54F=
_tmp54E->f1;}}_LL286: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp54B)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;struct Cyc_Absyn_Exp*_tmpBD1[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBD1[2]=e2,((_tmpBD1[1]=_tmp54F,((_tmpBD1[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL282;_LL282:;}goto _LL27D;_LL280:;_LL281: goto _LL27D;
_LL27D:;}break;case Cyc_Absyn_Minus: _LL27C: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp52B))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A2->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp52B->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBD2[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD2[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBD2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBD2[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBD2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
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
int is_dyneither=0;const char*_tmpBD3;struct _dyneither_ptr incr_str=(_tmpBD3="increment",
_tag_dyneither(_tmpBD3,sizeof(char),10));if(_tmp4A5 == Cyc_Absyn_PreDec  || 
_tmp4A5 == Cyc_Absyn_PostDec){const char*_tmpBD4;incr_str=((_tmpBD4="decrement",
_tag_dyneither(_tmpBD4,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A4,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBD8;void*_tmpBD7[1];
struct Cyc_String_pa_struct _tmpBD6;(_tmpBD6.tag=0,((_tmpBD6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD7[0]=& _tmpBD6,Cyc_Tcutil_terr(
e->loc,((_tmpBD8="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBD8,sizeof(char),74))),_tag_dyneither(_tmpBD7,sizeof(void*),1)))))));}{
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBDB,sizeof(char),30))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4A4,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBDC;_tmp559->topt=((_tmpBDC=_cycalloc(sizeof(*
_tmpBDC)),((_tmpBDC->v=Cyc_Absyn_sint_typ,_tmpBDC))));}switch(_tmp4A5){case Cyc_Absyn_PreInc:
_LL28F:{struct Cyc_Absyn_AssignOp_e_struct _tmpBE2;struct Cyc_Core_Opt*_tmpBE1;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBE0;e->r=(void*)((_tmpBE0=_cycalloc(
sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE2.tag=4,((_tmpBE2.f1=_tmp4A4,((_tmpBE2.f2=((
_tmpBE1=_cycalloc_atomic(sizeof(*_tmpBE1)),((_tmpBE1->v=(void*)Cyc_Absyn_Plus,
_tmpBE1)))),((_tmpBE2.f3=_tmp559,_tmpBE2)))))))),_tmpBE0))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL290:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBE8;struct Cyc_Core_Opt*_tmpBE7;struct Cyc_Absyn_AssignOp_e_struct*_tmpBE6;e->r=(
void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE8.tag=4,((
_tmpBE8.f1=_tmp4A4,((_tmpBE8.f2=((_tmpBE7=_cycalloc_atomic(sizeof(*_tmpBE7)),((
_tmpBE7->v=(void*)Cyc_Absyn_Minus,_tmpBE7)))),((_tmpBE8.f3=_tmp559,_tmpBE8)))))))),
_tmpBE6))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL291:{const char*_tmpBEC;
void*_tmpBEB[1];struct Cyc_String_pa_struct _tmpBEA;(_tmpBEA.tag=0,((_tmpBEA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBEB[0]=& _tmpBEA,Cyc_Tcutil_terr(
e->loc,((_tmpBEC="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBEC,sizeof(char),59))),_tag_dyneither(_tmpBEB,sizeof(void*),1)))))));}{
const char*_tmpBEF;void*_tmpBEE;(_tmpBEE=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEF="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBEF,sizeof(char),34))),_tag_dyneither(_tmpBEE,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4A4);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4A5 == Cyc_Absyn_PostInc
 || _tmp4A5 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4A5 == Cyc_Absyn_PreDec  || _tmp4A5 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBF0[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBF0[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBF0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBF0[0]=Cyc_Toc_push_address_exp(_tmp4A4),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF0,sizeof(struct Cyc_Absyn_Exp*),
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
_tmpBF6;const char*_tmpBF5;struct Cyc_Core_Impossible_struct*_tmpBF4;(int)_throw((
void*)((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4[0]=((_tmpBF6.tag=Cyc_Core_Impossible,((
_tmpBF6.f1=((_tmpBF5="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBF5,sizeof(char),44))),_tmpBF6)))),_tmpBF4)))));}}goto _LL293;
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
goto _LL293;_LL29C:;_LL29D: {struct Cyc_Core_Impossible_struct _tmpBFC;const char*
_tmpBFB;struct Cyc_Core_Impossible_struct*_tmpBFA;(int)_throw((void*)((_tmpBFA=
_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFC.tag=Cyc_Core_Impossible,((
_tmpBFC.f1=((_tmpBFB="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBFB,sizeof(char),64))),_tmpBFC)))),_tmpBFA)))));}_LL293:;}{
struct Cyc_Absyn_Exp*_tmpBFD[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBFD[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBFD[0]=_tmp4A4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBFD,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
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
_LL2A8: {const char*_tmpC00;void*_tmpBFF;(_tmpBFF=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC00="bad t ? pointer arithmetic",
_tag_dyneither(_tmpC00,sizeof(char),27))),_tag_dyneither(_tmpBFF,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp578=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpC01[3];e->r=Cyc_Toc_fncall_exp_r(_tmp578,((_tmpC01[2]=change,((
_tmpC01[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC01[0]=Cyc_Toc_push_address_exp(
_tmp4A7),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC01,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4A8->v){case Cyc_Absyn_Plus:
_LL2AA: done=1;{struct Cyc_Absyn_Exp*_tmpC02[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A7),((_tmpC02[1]=_tmp4A9,((
_tmpC02[0]=_tmp4A7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC02,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2AB: {const char*_tmpC05;void*_tmpC04;(_tmpC04=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC05="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpC05,sizeof(char),39))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4A9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4A9->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4A7)){{struct _tuple18 _tmpC08;struct
_tuple18*_tmpC07;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4A7,0,Cyc_Toc_assignop_lvalue,((_tmpC07=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpC07[0]=((_tmpC08.f1=_tmp4A8,((_tmpC08.f2=
_tmp4A9,_tmpC08)))),_tmpC07)))));}e->r=_tmp4A7->r;}}goto _LL212;};};};}_LL221: {
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
struct Cyc_List_List*_tmpC09;new_args=((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((
_tmpC09->hd=(struct Cyc_Absyn_Exp*)_tmp4C0->hd,((_tmpC09->tl=new_args,_tmpC09))))));};}}{
struct Cyc_Absyn_Exp*_tmpC0C[3];struct Cyc_List_List*_tmpC0B;new_args=((_tmpC0B=
_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC0C[2]=num_varargs_exp,((_tmpC0C[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC0C[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC0B->tl=
new_args,_tmpC0B))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4BF);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4BF,new_args,0),0);if(_tmp4C5->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp585=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4C5->type));struct Cyc_Absyn_DatatypeInfo _tmp587;union Cyc_Absyn_DatatypeInfoU
_tmp588;struct Cyc_Absyn_Datatypedecl**_tmp589;struct Cyc_Absyn_Datatypedecl*
_tmp58A;_LL2AE: {struct Cyc_Absyn_DatatypeType_struct*_tmp586=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp585;if(_tmp586->tag != 3)goto _LL2B0;else{_tmp587=_tmp586->f1;_tmp588=_tmp587.datatype_info;
if((_tmp588.KnownDatatype).tag != 2)goto _LL2B0;_tmp589=(struct Cyc_Absyn_Datatypedecl**)(
_tmp588.KnownDatatype).val;_tmp58A=*_tmp589;}}_LL2AF: tud=_tmp58A;goto _LL2AD;
_LL2B0:;_LL2B1: {const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0F="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC0F,sizeof(char),44))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}
_LL2AD:;}{unsigned int _tmpC14;unsigned int _tmpC13;struct _dyneither_ptr _tmpC12;
struct _tuple0**_tmpC11;unsigned int _tmpC10;struct _dyneither_ptr vs=(_tmpC10=(
unsigned int)_tmp4C3,((_tmpC11=(struct _tuple0**)_region_malloc(_tmp581,
_check_times(sizeof(struct _tuple0*),_tmpC10)),((_tmpC12=_tag_dyneither(_tmpC11,
sizeof(struct _tuple0*),_tmpC10),((((_tmpC13=_tmpC10,_tmpC15(& _tmpC14,& _tmpC13,&
_tmpC11))),_tmpC12)))))));if(_tmp4C3 != 0){struct Cyc_List_List*_tmp58D=0;{int i=
_tmp4C3 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC16;_tmp58D=((_tmpC16=
_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC16->tl=_tmp58D,_tmpC16))))));}}s=Cyc_Absyn_declare_stmt(
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
tud,_tmp591),0),s,0);{const char*_tmpC17;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp591,((_tmpC17="_struct",_tag_dyneither(_tmpC17,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC18[3];struct Cyc_List_List*
_tmp594=(_tmpC18[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC18[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC18[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC18,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
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
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4CC->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp59A=Cyc_Tcutil_compress((void*)_tmp4CC->hd);_LL2B3: {struct Cyc_Absyn_VarType_struct*
_tmp59B=(struct Cyc_Absyn_VarType_struct*)_tmp59A;if(_tmp59B->tag != 2)goto _LL2B5;}
_LL2B4: goto _LL2B6;_LL2B5: {struct Cyc_Absyn_DatatypeType_struct*_tmp59C=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp59A;if(_tmp59C->tag != 3)goto _LL2B7;}_LL2B6:
continue;_LL2B7:;_LL2B8: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4CB,0))->r;goto _LL2B2;_LL2B2:;}
break;}}goto _LL212;_LL235: {struct Cyc_Absyn_Cast_e_struct*_tmp4CD=(struct Cyc_Absyn_Cast_e_struct*)
_tmp496;if(_tmp4CD->tag != 15)goto _LL237;else{_tmp4CE=(void**)& _tmp4CD->f1;
_tmp4CF=(void**)((void**)& _tmp4CD->f1);_tmp4D0=_tmp4CD->f2;_tmp4D1=_tmp4CD->f3;
_tmp4D2=_tmp4CD->f4;}}_LL236: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4D0->topt))->v;void*new_typ=*_tmp4CF;*_tmp4CF=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4D0);{struct _tuple17 _tmpC19;struct _tuple17
_tmp59E=(_tmpC19.f1=Cyc_Tcutil_compress(old_t2),((_tmpC19.f2=Cyc_Tcutil_compress(
new_typ),_tmpC19)));void*_tmp59F;struct Cyc_Absyn_PtrInfo _tmp5A1;void*_tmp5A2;
struct Cyc_Absyn_PtrInfo _tmp5A4;void*_tmp5A5;struct Cyc_Absyn_PtrInfo _tmp5A7;void*
_tmp5A8;_LL2BA: _tmp59F=_tmp59E.f1;{struct Cyc_Absyn_PointerType_struct*_tmp5A0=(
struct Cyc_Absyn_PointerType_struct*)_tmp59F;if(_tmp5A0->tag != 5)goto _LL2BC;else{
_tmp5A1=_tmp5A0->f1;}};_tmp5A2=_tmp59E.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5A3=(struct Cyc_Absyn_PointerType_struct*)_tmp5A2;if(_tmp5A3->tag != 5)goto
_LL2BC;else{_tmp5A4=_tmp5A3->f1;}};_LL2BB: {int _tmp5AA=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A1.ptr_atts).nullable);int _tmp5AB=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A4.ptr_atts).nullable);
void*_tmp5AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A1.ptr_atts).bounds);void*_tmp5AD=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A4.ptr_atts).bounds);
int _tmp5AE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5A1.ptr_atts).zero_term);int _tmp5AF=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A4.ptr_atts).zero_term);{struct _tuple17 _tmpC1A;
struct _tuple17 _tmp5B1=(_tmpC1A.f1=_tmp5AC,((_tmpC1A.f2=_tmp5AD,_tmpC1A)));void*
_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B4;void*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B7;
void*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5BA;void*_tmp5BB;void*_tmp5BD;void*_tmp5BF;
struct Cyc_Absyn_Exp*_tmp5C1;void*_tmp5C2;void*_tmp5C4;_LL2C1: _tmp5B2=_tmp5B1.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B3=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B2;
if(_tmp5B3->tag != 1)goto _LL2C3;else{_tmp5B4=_tmp5B3->f1;}};_tmp5B5=_tmp5B1.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B6=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B5;
if(_tmp5B6->tag != 1)goto _LL2C3;else{_tmp5B7=_tmp5B6->f1;}};_LL2C2: if((!Cyc_Evexp_c_can_eval(
_tmp5B4) || !Cyc_Evexp_c_can_eval(_tmp5B7)) && !Cyc_Evexp_same_const_exp(_tmp5B4,
_tmp5B7)){const char*_tmpC1D;void*_tmpC1C;(_tmpC1C=0,Cyc_Tcutil_terr(e->loc,((
_tmpC1D="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC1D,sizeof(char),54))),_tag_dyneither(_tmpC1C,sizeof(void*),0)));}if(_tmp5AA
 && !_tmp5AB){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC20;void*_tmpC1F;(
_tmpC1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC20="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC20,
sizeof(char),44))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}if(_tmp4D2 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC24;void*_tmpC23[1];struct Cyc_String_pa_struct _tmpC22;(_tmpC22.tag=
0,((_tmpC22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC23[0]=& _tmpC22,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC24="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC24,sizeof(char),41))),_tag_dyneither(_tmpC23,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4D0);if(do_null_check){if(!_tmp4D1){
const char*_tmpC27;void*_tmpC26;(_tmpC26=0,Cyc_Tcutil_warn(e->loc,((_tmpC27="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC27,sizeof(char),58))),_tag_dyneither(_tmpC26,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC28;e->r=Cyc_Toc_cast_it_r(*_tmp4CF,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=
_tmp4D0,((_tmpC28->tl=0,_tmpC28)))))),0));};}};}goto _LL2C0;_LL2C3: _tmp5B8=
_tmp5B1.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5B9=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5B8;if(_tmp5B9->tag != 1)goto _LL2C5;else{_tmp5BA=_tmp5B9->f1;}};_tmp5BB=
_tmp5B1.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BB;if(_tmp5BC->tag != 0)goto _LL2C5;};_LL2C4: if(!Cyc_Evexp_c_can_eval(_tmp5BA)){
const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,Cyc_Tcutil_terr(e->loc,((_tmpC2B="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC2B,sizeof(char),71))),_tag_dyneither(_tmpC2A,sizeof(void*),0)));}
if(_tmp4D2 == Cyc_Absyn_NonNull_to_Null){const char*_tmpC2F;void*_tmpC2E[1];struct
Cyc_String_pa_struct _tmpC2D;(_tmpC2D.tag=0,((_tmpC2D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC2E[0]=& _tmpC2D,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC2F="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC2F,sizeof(char),44))),_tag_dyneither(_tmpC2E,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5AE  && !(_tmp5A4.elt_tq).real_const) && !
_tmp5AF)_tmp5BA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5BA,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5BA,_tmp4D0))->r;}else{
struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Toc__tag_dyneither_e;if(_tmp5AE){struct _tuple0*
_tmp5D6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5D7=Cyc_Absyn_var_exp(_tmp5D6,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5D8=_tmp4D0->r;union Cyc_Absyn_Cnst _tmp5DA;
struct _dyneither_ptr _tmp5DB;_LL2CA: {struct Cyc_Absyn_Const_e_struct*_tmp5D9=(
struct Cyc_Absyn_Const_e_struct*)_tmp5D8;if(_tmp5D9->tag != 0)goto _LL2CC;else{
_tmp5DA=_tmp5D9->f1;if((_tmp5DA.String_c).tag != 7)goto _LL2CC;_tmp5DB=(struct
_dyneither_ptr)(_tmp5DA.String_c).val;}}_LL2CB: arg3=_tmp5BA;goto _LL2C9;_LL2CC:;
_LL2CD:{struct Cyc_Absyn_Exp*_tmpC30[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4D0),((_tmpC30[1]=_tmp5BA,((_tmpC30[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5D7),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC30,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2C9;_LL2C9:;}if(!_tmp5AF  && !(_tmp5A4.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5DD=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A4.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC31[3];struct Cyc_Absyn_Exp*_tmp5DE=Cyc_Absyn_fncall_exp(_tmp5D5,((_tmpC31[2]=
arg3,((_tmpC31[1]=_tmp5DD,((_tmpC31[0]=_tmp5D7,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC31,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5DF=Cyc_Absyn_exp_stmt(_tmp5DE,0);_tmp5DF=
Cyc_Absyn_declare_stmt(_tmp5D6,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4D0,_tmp5DF,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5DF);};}else{struct Cyc_Absyn_Exp*
_tmpC32[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D5,((_tmpC32[2]=_tmp5BA,((_tmpC32[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A4.elt_typ),0),((_tmpC32[0]=
_tmp4D0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC32,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2C0;
_LL2C5: _tmp5BD=_tmp5B1.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BD;if(_tmp5BE->tag != 0)goto _LL2C7;};_tmp5BF=_tmp5B1.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5C0=(struct Cyc_Absyn_Upper_b_struct*)_tmp5BF;if(_tmp5C0->tag != 1)goto _LL2C7;
else{_tmp5C1=_tmp5C0->f1;}};_LL2C6: if(!Cyc_Evexp_c_can_eval(_tmp5C1)){const char*
_tmpC35;void*_tmpC34;(_tmpC34=0,Cyc_Tcutil_terr(e->loc,((_tmpC35="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC35,sizeof(char),71))),_tag_dyneither(_tmpC34,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC38="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC38,sizeof(char),45))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E6=_tmp5C1;if(_tmp5AE  && !_tmp5AF)_tmp5E6=Cyc_Absyn_add_exp(
_tmp5C1,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5E7=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC39[3];struct Cyc_Absyn_Exp*_tmp5E8=Cyc_Absyn_fncall_exp(
_tmp5E7,((_tmpC39[2]=_tmp5E6,((_tmpC39[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5A1.elt_typ),0),((_tmpC39[0]=_tmp4D0,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5AB){struct Cyc_List_List*_tmpC3A;_tmp5E8->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->hd=Cyc_Absyn_copy_exp(
_tmp5E8),((_tmpC3A->tl=0,_tmpC3A)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4CF,_tmp5E8);
goto _LL2C0;};};_LL2C7: _tmp5C2=_tmp5B1.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5C3=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C2;if(_tmp5C3->tag != 0)goto
_LL2C0;};_tmp5C4=_tmp5B1.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C5=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5C4;if(_tmp5C5->tag != 0)goto _LL2C0;};_LL2C8:
DynCast: if((_tmp5AE  && !_tmp5AF) && !(_tmp5A4.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC3D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC3D,sizeof(char),70))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5ED=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC3E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5ED,((_tmpC3E[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC3E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A1.elt_typ),0),((
_tmpC3E[0]=_tmp4D0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2C0;
_LL2C0:;}goto _LL2B9;}_LL2BC: _tmp5A5=_tmp59E.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5A6=(struct Cyc_Absyn_PointerType_struct*)_tmp5A5;if(_tmp5A6->tag != 5)goto
_LL2BE;else{_tmp5A7=_tmp5A6->f1;}};_tmp5A8=_tmp59E.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5A9=(struct Cyc_Absyn_IntType_struct*)_tmp5A8;if(_tmp5A9->tag != 6)goto _LL2BE;};
_LL2BD:{void*_tmp5EF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A7.ptr_atts).bounds);_LL2CF: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5F0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5EF;if(_tmp5F0->tag != 0)goto
_LL2D1;}_LL2D0: _tmp4D0->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4D0->r,
_tmp4D0->loc),Cyc_Toc_curr_sp);goto _LL2CE;_LL2D1:;_LL2D2: goto _LL2CE;_LL2CE:;}
goto _LL2B9;_LL2BE:;_LL2BF: goto _LL2B9;_LL2B9:;}goto _LL212;}_LL237: {struct Cyc_Absyn_Address_e_struct*
_tmp4D3=(struct Cyc_Absyn_Address_e_struct*)_tmp496;if(_tmp4D3->tag != 16)goto
_LL239;else{_tmp4D4=_tmp4D3->f1;}}_LL238:{void*_tmp5F1=_tmp4D4->r;struct _tuple0*
_tmp5F3;struct Cyc_List_List*_tmp5F4;struct Cyc_List_List*_tmp5F5;struct Cyc_List_List*
_tmp5F7;_LL2D4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5F1;if(_tmp5F2->tag != 29)goto _LL2D6;else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;
_tmp5F5=_tmp5F2->f3;}}_LL2D5: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC42;void*
_tmpC41[1];struct Cyc_String_pa_struct _tmpC40;(_tmpC40.tag=0,((_tmpC40.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D4->loc)),((
_tmpC41[0]=& _tmpC40,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC42="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC42,
sizeof(char),42))),_tag_dyneither(_tmpC41,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v,_tmp5F4 != 0,1,0,
_tmp5F5,_tmp5F3))->r;goto _LL2D3;_LL2D6: {struct Cyc_Absyn_Tuple_e_struct*_tmp5F6=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5F1;if(_tmp5F6->tag != 25)goto _LL2D8;else{
_tmp5F7=_tmp5F6->f1;}}_LL2D7: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC46;void*
_tmpC45[1];struct Cyc_String_pa_struct _tmpC44;(_tmpC44.tag=0,((_tmpC44.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D4->loc)),((
_tmpC45[0]=& _tmpC44,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC46="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC46,
sizeof(char),42))),_tag_dyneither(_tmpC45,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5F7))->r;goto _LL2D3;_LL2D8:;_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp4D4);if(!
Cyc_Absyn_is_lvalue(_tmp4D4)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4D4,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D4);}goto _LL2D3;
_LL2D3:;}goto _LL212;_LL239: {struct Cyc_Absyn_New_e_struct*_tmp4D5=(struct Cyc_Absyn_New_e_struct*)
_tmp496;if(_tmp4D5->tag != 17)goto _LL23B;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;}}
_LL23A: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC4A;void*_tmpC49[1];struct Cyc_String_pa_struct
_tmpC48;(_tmpC48.tag=0,((_tmpC48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D7->loc)),((_tmpC49[0]=& _tmpC48,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC4A="%s: new at top-level",
_tag_dyneither(_tmpC4A,sizeof(char),21))),_tag_dyneither(_tmpC49,sizeof(void*),1)))))));}{
void*_tmp601=_tmp4D7->r;struct Cyc_List_List*_tmp603;struct Cyc_Absyn_Vardecl*
_tmp605;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Exp*_tmp607;int _tmp608;
struct _tuple0*_tmp60A;struct Cyc_List_List*_tmp60B;struct Cyc_List_List*_tmp60C;
struct Cyc_Absyn_Aggrdecl*_tmp60D;struct Cyc_List_List*_tmp60F;_LL2DB: {struct Cyc_Absyn_Array_e_struct*
_tmp602=(struct Cyc_Absyn_Array_e_struct*)_tmp601;if(_tmp602->tag != 27)goto _LL2DD;
else{_tmp603=_tmp602->f1;}}_LL2DC: {struct _tuple0*_tmp610=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp611=Cyc_Absyn_var_exp(_tmp610,0);struct Cyc_Absyn_Stmt*
_tmp612=Cyc_Toc_init_array(nv,_tmp611,_tmp603,Cyc_Absyn_exp_stmt(_tmp611,0));
void*old_elt_typ;{void*_tmp613=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp615;void*_tmp616;struct Cyc_Absyn_Tqual _tmp617;struct Cyc_Absyn_PtrAtts _tmp618;
union Cyc_Absyn_Constraint*_tmp619;_LL2E6: {struct Cyc_Absyn_PointerType_struct*
_tmp614=(struct Cyc_Absyn_PointerType_struct*)_tmp613;if(_tmp614->tag != 5)goto
_LL2E8;else{_tmp615=_tmp614->f1;_tmp616=_tmp615.elt_typ;_tmp617=_tmp615.elt_tq;
_tmp618=_tmp615.ptr_atts;_tmp619=_tmp618.zero_term;}}_LL2E7: old_elt_typ=_tmp616;
goto _LL2E5;_LL2E8:;_LL2E9: {const char*_tmpC4D;void*_tmpC4C;old_elt_typ=((_tmpC4C=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4D="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC4D,sizeof(char),52))),_tag_dyneither(_tmpC4C,sizeof(void*),0))));}_LL2E5:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp61C=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp61D=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp603),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D6 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp61D);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4D6;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp61D);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp610,_tmp61C,(struct Cyc_Absyn_Exp*)e1,_tmp612,0));goto
_LL2DA;};}_LL2DD: {struct Cyc_Absyn_Comprehension_e_struct*_tmp604=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp601;if(_tmp604->tag != 28)goto _LL2DF;else{_tmp605=_tmp604->f1;_tmp606=_tmp604->f2;
_tmp607=_tmp604->f3;_tmp608=_tmp604->f4;}}_LL2DE: {int is_dyneither_ptr=0;{void*
_tmp61E=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp620;void*_tmp621;
struct Cyc_Absyn_Tqual _tmp622;struct Cyc_Absyn_PtrAtts _tmp623;union Cyc_Absyn_Constraint*
_tmp624;union Cyc_Absyn_Constraint*_tmp625;_LL2EB: {struct Cyc_Absyn_PointerType_struct*
_tmp61F=(struct Cyc_Absyn_PointerType_struct*)_tmp61E;if(_tmp61F->tag != 5)goto
_LL2ED;else{_tmp620=_tmp61F->f1;_tmp621=_tmp620.elt_typ;_tmp622=_tmp620.elt_tq;
_tmp623=_tmp620.ptr_atts;_tmp624=_tmp623.bounds;_tmp625=_tmp623.zero_term;}}
_LL2EC: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp624)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2EA;
_LL2ED:;_LL2EE: {const char*_tmpC50;void*_tmpC4F;(_tmpC4F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC50="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC50,sizeof(char),42))),_tag_dyneither(_tmpC4F,sizeof(void*),0)));}
_LL2EA:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp607->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp606);{struct Cyc_Absyn_Exp*_tmp628=
Cyc_Absyn_var_exp(max,0);if(_tmp608)_tmp628=Cyc_Absyn_add_exp(_tmp628,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp605,Cyc_Absyn_var_exp(max,0),_tmp607,_tmp608,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp62A;struct _RegionHandle*_tmp62B;struct Cyc_Toc_Env*_tmp629=
nv;_tmp62A=*_tmp629;_tmp62B=_tmp62A.rgn;{struct _tuple19*_tmpC53;struct Cyc_List_List*
_tmpC52;struct Cyc_List_List*decls=(_tmpC52=_region_malloc(_tmp62B,sizeof(*
_tmpC52)),((_tmpC52->hd=((_tmpC53=_region_malloc(_tmp62B,sizeof(*_tmpC53)),((
_tmpC53->f1=max,((_tmpC53->f2=Cyc_Absyn_uint_typ,((_tmpC53->f3=(struct Cyc_Absyn_Exp*)
_tmp606,_tmpC53)))))))),((_tmpC52->tl=0,_tmpC52)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4D6 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC54[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC54[1]=_tmp628,((
_tmpC54[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D6;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC55[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC55[1]=_tmp628,((_tmpC55[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC58;struct Cyc_List_List*
_tmpC57;decls=((_tmpC57=_region_malloc(_tmp62B,sizeof(*_tmpC57)),((_tmpC57->hd=((
_tmpC58=_region_malloc(_tmp62B,sizeof(*_tmpC58)),((_tmpC58->f1=a,((_tmpC58->f2=
ptr_typ,((_tmpC58->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC58)))))))),((_tmpC57->tl=
decls,_tmpC57))))));}if(is_dyneither_ptr){struct _tuple0*_tmp630=Cyc_Toc_temp_var();
void*_tmp631=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp632=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC59[3];struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_fncall_exp(
_tmp632,((_tmpC59[2]=_tmp628,((_tmpC59[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC59[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC59,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC5C;struct Cyc_List_List*_tmpC5B;decls=((_tmpC5B=
_region_malloc(_tmp62B,sizeof(*_tmpC5B)),((_tmpC5B->hd=((_tmpC5C=_region_malloc(
_tmp62B,sizeof(*_tmpC5C)),((_tmpC5C->f1=_tmp630,((_tmpC5C->f2=_tmp631,((_tmpC5C->f3=(
struct Cyc_Absyn_Exp*)_tmp633,_tmpC5C)))))))),((_tmpC5B->tl=decls,_tmpC5B))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp630,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp637=decls;for(0;_tmp637 != 0;_tmp637=_tmp637->tl){struct
_tuple0*_tmp639;void*_tmp63A;struct Cyc_Absyn_Exp*_tmp63B;struct _tuple19 _tmp638=*((
struct _tuple19*)_tmp637->hd);_tmp639=_tmp638.f1;_tmp63A=_tmp638.f2;_tmp63B=
_tmp638.f3;s=Cyc_Absyn_declare_stmt(_tmp639,_tmp63A,_tmp63B,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2DA;};};};}_LL2DF: {struct Cyc_Absyn_Aggregate_e_struct*_tmp609=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp601;if(_tmp609->tag != 29)goto _LL2E1;
else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=_tmp609->f3;_tmp60D=_tmp609->f4;}}
_LL2E0: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D7->topt))->v,_tmp60B != 0,1,_tmp4D6,_tmp60C,_tmp60A))->r;goto _LL2DA;_LL2E1: {
struct Cyc_Absyn_Tuple_e_struct*_tmp60E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp601;
if(_tmp60E->tag != 25)goto _LL2E3;else{_tmp60F=_tmp60E->f1;}}_LL2E2: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4D6,_tmp60F))->r;goto _LL2DA;_LL2E3:;_LL2E4: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D7->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp63E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp63F=Cyc_Absyn_var_exp(_tmp63E,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp63F,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4D6
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D6;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp640=_tmp4D7->r;void*_tmp642;
struct Cyc_Absyn_Exp*_tmp643;_LL2F0: {struct Cyc_Absyn_Cast_e_struct*_tmp641=(
struct Cyc_Absyn_Cast_e_struct*)_tmp640;if(_tmp641->tag != 15)goto _LL2F2;else{
_tmp642=(void*)_tmp641->f1;_tmp643=_tmp641->f2;}}_LL2F1:{struct _tuple17 _tmpC5D;
struct _tuple17 _tmp645=(_tmpC5D.f1=Cyc_Tcutil_compress(_tmp642),((_tmpC5D.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp643->topt))->v),_tmpC5D)));void*
_tmp646;struct Cyc_Absyn_PtrInfo _tmp648;void*_tmp649;struct Cyc_Absyn_PtrAtts
_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;void*_tmp64C;struct Cyc_Absyn_PtrInfo
_tmp64E;struct Cyc_Absyn_PtrAtts _tmp64F;union Cyc_Absyn_Constraint*_tmp650;_LL2F5:
_tmp646=_tmp645.f1;{struct Cyc_Absyn_PointerType_struct*_tmp647=(struct Cyc_Absyn_PointerType_struct*)
_tmp646;if(_tmp647->tag != 5)goto _LL2F7;else{_tmp648=_tmp647->f1;_tmp649=_tmp648.elt_typ;
_tmp64A=_tmp648.ptr_atts;_tmp64B=_tmp64A.bounds;}};_tmp64C=_tmp645.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp64D=(struct Cyc_Absyn_PointerType_struct*)_tmp64C;if(_tmp64D->tag != 5)goto
_LL2F7;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64E.ptr_atts;_tmp650=_tmp64F.bounds;}};
_LL2F6:{struct _tuple17 _tmpC5E;struct _tuple17 _tmp652=(_tmpC5E.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp64B),((_tmpC5E.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp650),_tmpC5E)));void*_tmp653;void*_tmp655;struct Cyc_Absyn_Exp*
_tmp657;_LL2FA: _tmp653=_tmp652.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp654=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp653;if(_tmp654->tag != 0)goto _LL2FC;};
_tmp655=_tmp652.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp656=(struct Cyc_Absyn_Upper_b_struct*)
_tmp655;if(_tmp656->tag != 1)goto _LL2FC;else{_tmp657=_tmp656->f1;}};_LL2FB: Cyc_Toc_exp_to_c(
nv,_tmp643);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp658=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC5F[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp658,((_tmpC5F[3]=_tmp657,((_tmpC5F[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp649),0),((_tmpC5F[1]=_tmp643,((_tmpC5F[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5F,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F9;};_LL2FC:;_LL2FD: goto _LL2F9;_LL2F9:;}goto _LL2F4;_LL2F7:;
_LL2F8: goto _LL2F4;_LL2F4:;}goto _LL2EF;_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp65A=Cyc_Absyn_exp_stmt(_tmp63F,0);struct Cyc_Absyn_Exp*
_tmp65B=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4D7);_tmp65A=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp63F,_tmp65B,0),_tmp4D7,0),
_tmp65A,0);{void*_tmp65C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63E,_tmp65C,(struct Cyc_Absyn_Exp*)mexp,_tmp65A,0));};}
goto _LL2DA;};}_LL2DA:;}goto _LL212;_LL23B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4D8=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp496;if(_tmp4D8->tag != 19)goto
_LL23D;else{_tmp4D9=_tmp4D8->f1;}}_LL23C: Cyc_Toc_exp_to_c(nv,_tmp4D9);goto _LL212;
_LL23D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4DA=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp496;if(_tmp4DA->tag != 18)goto _LL23F;else{_tmp4DB=(void*)_tmp4DA->f1;}}_LL23E:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC62;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC61;e->r=(void*)((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=
18,((_tmpC62.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DB),_tmpC62)))),_tmpC61))));}
goto _LL212;_LL23F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4DC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp496;if(_tmp4DC->tag != 20)goto _LL241;else{_tmp4DD=(void*)_tmp4DC->f1;_tmp4DE=(
void*)_tmp4DC->f2;{struct Cyc_Absyn_StructField_struct*_tmp4DF=(struct Cyc_Absyn_StructField_struct*)
_tmp4DE;if(_tmp4DF->tag != 0)goto _LL241;else{_tmp4E0=_tmp4DF->f1;}};}}_LL240:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC6C;struct Cyc_Absyn_StructField_struct
_tmpC6B;struct Cyc_Absyn_StructField_struct*_tmpC6A;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC69;e->r=(void*)((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6C.tag=
20,((_tmpC6C.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DD),((_tmpC6C.f2=(void*)((
void*)((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=0,((
_tmpC6B.f1=_tmp4E0,_tmpC6B)))),_tmpC6A))))),_tmpC6C)))))),_tmpC69))));}goto
_LL212;_LL241: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4E1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp496;if(_tmp4E1->tag != 20)goto _LL243;else{_tmp4E2=(void*)_tmp4E1->f1;_tmp4E3=(
void*)_tmp4E1->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4E4=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4E3;if(_tmp4E4->tag != 1)goto _LL243;else{_tmp4E5=_tmp4E4->f1;}};}}_LL242:{
void*_tmp663=Cyc_Tcutil_compress(_tmp4E2);struct Cyc_Absyn_AggrInfo _tmp665;union
Cyc_Absyn_AggrInfoU _tmp666;struct Cyc_List_List*_tmp668;_LL2FF: {struct Cyc_Absyn_AggrType_struct*
_tmp664=(struct Cyc_Absyn_AggrType_struct*)_tmp663;if(_tmp664->tag != 12)goto
_LL301;else{_tmp665=_tmp664->f1;_tmp666=_tmp665.aggr_info;}}_LL300: {struct Cyc_Absyn_Aggrdecl*
_tmp66B=Cyc_Absyn_get_known_aggrdecl(_tmp666);if(_tmp66B->impl == 0){const char*
_tmpC6F;void*_tmpC6E;(_tmpC6E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6F="struct fields must be known",
_tag_dyneither(_tmpC6F,sizeof(char),28))),_tag_dyneither(_tmpC6E,sizeof(void*),0)));}
_tmp668=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66B->impl))->fields;goto
_LL302;}_LL301: {struct Cyc_Absyn_AnonAggrType_struct*_tmp667=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp663;if(_tmp667->tag != 13)goto _LL303;else{_tmp668=_tmp667->f2;}}_LL302: {
struct Cyc_Absyn_Aggrfield*_tmp66E=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp668,(int)_tmp4E5);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC79;struct Cyc_Absyn_StructField_struct _tmpC78;struct Cyc_Absyn_StructField_struct*
_tmpC77;struct Cyc_Absyn_Offsetof_e_struct*_tmpC76;e->r=(void*)((_tmpC76=
_cycalloc(sizeof(*_tmpC76)),((_tmpC76[0]=((_tmpC79.tag=20,((_tmpC79.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC79.f2=(void*)((void*)((_tmpC77=_cycalloc(
sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC78.tag=0,((_tmpC78.f1=_tmp66E->name,
_tmpC78)))),_tmpC77))))),_tmpC79)))))),_tmpC76))));}goto _LL2FE;}_LL303: {struct
Cyc_Absyn_TupleType_struct*_tmp669=(struct Cyc_Absyn_TupleType_struct*)_tmp663;
if(_tmp669->tag != 11)goto _LL305;}_LL304:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC83;struct Cyc_Absyn_StructField_struct _tmpC82;struct Cyc_Absyn_StructField_struct*
_tmpC81;struct Cyc_Absyn_Offsetof_e_struct*_tmpC80;e->r=(void*)((_tmpC80=
_cycalloc(sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC83.tag=20,((_tmpC83.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC83.f2=(void*)((void*)((_tmpC81=_cycalloc(
sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=0,((_tmpC82.f1=Cyc_Absyn_fieldname((
int)(_tmp4E5 + 1)),_tmpC82)))),_tmpC81))))),_tmpC83)))))),_tmpC80))));}goto _LL2FE;
_LL305: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp66A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp663;if(_tmp66A->tag != 4)goto _LL307;}_LL306: if(_tmp4E5 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC8D;struct Cyc_Absyn_StructField_struct _tmpC8C;struct Cyc_Absyn_StructField_struct*
_tmpC8B;struct Cyc_Absyn_Offsetof_e_struct*_tmpC8A;e->r=(void*)((_tmpC8A=
_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A[0]=((_tmpC8D.tag=20,((_tmpC8D.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC8D.f2=(void*)((void*)((_tmpC8B=_cycalloc(
sizeof(*_tmpC8B)),((_tmpC8B[0]=((_tmpC8C.tag=0,((_tmpC8C.f1=Cyc_Toc_tag_sp,
_tmpC8C)))),_tmpC8B))))),_tmpC8D)))))),_tmpC8A))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC97;struct Cyc_Absyn_StructField_struct _tmpC96;struct Cyc_Absyn_StructField_struct*
_tmpC95;struct Cyc_Absyn_Offsetof_e_struct*_tmpC94;e->r=(void*)((_tmpC94=
_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((_tmpC97.tag=20,((_tmpC97.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4E2),((_tmpC97.f2=(void*)((void*)((_tmpC95=_cycalloc(
sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=0,((_tmpC96.f1=Cyc_Absyn_fieldname((
int)_tmp4E5),_tmpC96)))),_tmpC95))))),_tmpC97)))))),_tmpC94))));}goto _LL2FE;
_LL307:;_LL308: {const char*_tmpC9A;void*_tmpC99;(_tmpC99=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9A="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC9A,sizeof(char),41))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}
_LL2FE:;}goto _LL212;_LL243: {struct Cyc_Absyn_Deref_e_struct*_tmp4E6=(struct Cyc_Absyn_Deref_e_struct*)
_tmp496;if(_tmp4E6->tag != 21)goto _LL245;else{_tmp4E7=_tmp4E6->f1;}}_LL244: {void*
_tmp681=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E7->topt))->v);{
void*_tmp682=_tmp681;struct Cyc_Absyn_PtrInfo _tmp684;void*_tmp685;struct Cyc_Absyn_Tqual
_tmp686;struct Cyc_Absyn_PtrAtts _tmp687;void*_tmp688;union Cyc_Absyn_Constraint*
_tmp689;union Cyc_Absyn_Constraint*_tmp68A;union Cyc_Absyn_Constraint*_tmp68B;
_LL30A: {struct Cyc_Absyn_PointerType_struct*_tmp683=(struct Cyc_Absyn_PointerType_struct*)
_tmp682;if(_tmp683->tag != 5)goto _LL30C;else{_tmp684=_tmp683->f1;_tmp685=_tmp684.elt_typ;
_tmp686=_tmp684.elt_tq;_tmp687=_tmp684.ptr_atts;_tmp688=_tmp687.rgn;_tmp689=
_tmp687.nullable;_tmp68A=_tmp687.bounds;_tmp68B=_tmp687.zero_term;}}_LL30B:{void*
_tmp68C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp68A);struct Cyc_Absyn_Exp*_tmp68E;_LL30F: {struct Cyc_Absyn_Upper_b_struct*
_tmp68D=(struct Cyc_Absyn_Upper_b_struct*)_tmp68C;if(_tmp68D->tag != 1)goto _LL311;
else{_tmp68E=_tmp68D->f1;}}_LL310: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4E7);Cyc_Toc_exp_to_c(nv,_tmp4E7);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC9D;void*_tmpC9C;(_tmpC9C=0,Cyc_Tcutil_warn(e->loc,((_tmpC9D="inserted null check due to dereference",
_tag_dyneither(_tmpC9D,sizeof(char),39))),_tag_dyneither(_tmpC9C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC9E;_tmp4E7->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp681),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC9E=_cycalloc(sizeof(*
_tmpC9E)),((_tmpC9E->hd=Cyc_Absyn_copy_exp(_tmp4E7),((_tmpC9E->tl=0,_tmpC9E)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp68B)){unsigned int _tmp694;int _tmp695;struct _tuple10 _tmp693=Cyc_Evexp_eval_const_uint_exp(
_tmp68E);_tmp694=_tmp693.f1;_tmp695=_tmp693.f2;if(!_tmp695  || _tmp694 <= 0){const
char*_tmpCA1;void*_tmpCA0;(_tmpCA0=0,Cyc_Tcutil_terr(e->loc,((_tmpCA1="cannot determine dereference is in bounds",
_tag_dyneither(_tmpCA1,sizeof(char),42))),_tag_dyneither(_tmpCA0,sizeof(void*),0)));}}
goto _LL30E;}_LL311: {struct Cyc_Absyn_DynEither_b_struct*_tmp68F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp68C;if(_tmp68F->tag != 0)goto _LL30E;}_LL312: {struct Cyc_Absyn_Exp*_tmp698=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpCA2;_tmp698->topt=((_tmpCA2=_cycalloc(sizeof(*
_tmpCA2)),((_tmpCA2->v=Cyc_Absyn_uint_typ,_tmpCA2))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4E7,_tmp698);Cyc_Toc_exp_to_c(nv,e);goto _LL30E;}_LL30E:;}goto _LL309;_LL30C:;
_LL30D: {const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpCA5,sizeof(char),29))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}
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
nv,_tmp4EE);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp69D;struct Cyc_Absyn_Tqual
_tmp69E;struct Cyc_Absyn_PtrAtts _tmp69F;void*_tmp6A0;union Cyc_Absyn_Constraint*
_tmp6A1;union Cyc_Absyn_Constraint*_tmp6A2;union Cyc_Absyn_Constraint*_tmp6A3;
struct Cyc_Absyn_PtrInfo _tmp69C=Cyc_Toc_get_ptr_type(e1typ);_tmp69D=_tmp69C.elt_typ;
_tmp69E=_tmp69C.elt_tq;_tmp69F=_tmp69C.ptr_atts;_tmp6A0=_tmp69F.rgn;_tmp6A1=
_tmp69F.nullable;_tmp6A2=_tmp69F.bounds;_tmp6A3=_tmp69F.zero_term;{void*_tmp6A4=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6A2);struct Cyc_Absyn_Exp*_tmp6A6;_LL314: {struct Cyc_Absyn_Upper_b_struct*
_tmp6A5=(struct Cyc_Absyn_Upper_b_struct*)_tmp6A4;if(_tmp6A5->tag != 1)goto _LL316;
else{_tmp6A6=_tmp6A5->f1;}}_LL315: {unsigned int _tmp6A9;int _tmp6AA;struct
_tuple10 _tmp6A8=Cyc_Evexp_eval_const_uint_exp(_tmp6A6);_tmp6A9=_tmp6A8.f1;
_tmp6AA=_tmp6A8.f2;if(_tmp6AA){if(_tmp6A9 < 1){const char*_tmpCA8;void*_tmpCA7;(
_tmpCA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpCA8="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCA8,
sizeof(char),44))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_warn(
e->loc,((_tmpCAB="inserted null check due to dereference",_tag_dyneither(_tmpCAB,
sizeof(char),39))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAC[1];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCAC[0]=Cyc_Absyn_new_exp(_tmp4EE->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6A6)){const char*_tmpCAF;void*
_tmpCAE;(_tmpCAE=0,Cyc_Tcutil_terr(e->loc,((_tmpCAF="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCAF,sizeof(char),47))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCB0[4];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCB0[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCB0[2]=Cyc_Absyn_sizeoftyp_exp(_tmp69D,0),((
_tmpCB0[1]=_tmp6A6,((_tmpCB0[0]=Cyc_Absyn_new_exp(_tmp4EE->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL313;}_LL316: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6A7=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6A4;if(_tmp6A7->tag != 0)goto
_LL313;}_LL317: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp69D);{struct Cyc_Absyn_Exp*
_tmpCB1[3];_tmp4EE->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp69E),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCB1[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCB1[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCB1[0]=Cyc_Absyn_new_exp(_tmp4EE->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCB1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL313;}_LL313:;}if(
_tmp4F0  && _tmp4F1)e->r=Cyc_Toc_check_tagged_union(_tmp4EE,Cyc_Toc_typ_to_c(
e1typ),_tmp69D,_tmp4EF,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4F1)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL212;};}_LL249: {struct Cyc_Absyn_Subscript_e_struct*_tmp4F2=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp496;if(_tmp4F2->tag != 24)goto _LL24B;
else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}}_LL24A: {void*_tmp6B4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F3->topt))->v);{void*_tmp6B5=
_tmp6B4;struct Cyc_List_List*_tmp6B7;struct Cyc_Absyn_PtrInfo _tmp6B9;void*_tmp6BA;
struct Cyc_Absyn_Tqual _tmp6BB;struct Cyc_Absyn_PtrAtts _tmp6BC;void*_tmp6BD;union
Cyc_Absyn_Constraint*_tmp6BE;union Cyc_Absyn_Constraint*_tmp6BF;union Cyc_Absyn_Constraint*
_tmp6C0;_LL319: {struct Cyc_Absyn_TupleType_struct*_tmp6B6=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B5;if(_tmp6B6->tag != 11)goto _LL31B;else{_tmp6B7=_tmp6B6->f1;}}_LL31A: Cyc_Toc_exp_to_c(
nv,_tmp4F3);Cyc_Toc_exp_to_c(nv,_tmp4F4);{unsigned int _tmp6C2;int _tmp6C3;struct
_tuple10 _tmp6C1=Cyc_Evexp_eval_const_uint_exp(_tmp4F4);_tmp6C2=_tmp6C1.f1;
_tmp6C3=_tmp6C1.f2;if(!_tmp6C3){const char*_tmpCB4;void*_tmpCB3;(_tmpCB3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB4="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCB4,sizeof(char),44))),_tag_dyneither(_tmpCB3,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F3,Cyc_Absyn_fieldname((int)(_tmp6C2 + 1)));
goto _LL318;};_LL31B: {struct Cyc_Absyn_PointerType_struct*_tmp6B8=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B5;if(_tmp6B8->tag != 5)goto _LL31D;else{_tmp6B9=_tmp6B8->f1;_tmp6BA=_tmp6B9.elt_typ;
_tmp6BB=_tmp6B9.elt_tq;_tmp6BC=_tmp6B9.ptr_atts;_tmp6BD=_tmp6BC.rgn;_tmp6BE=
_tmp6BC.nullable;_tmp6BF=_tmp6BC.bounds;_tmp6C0=_tmp6BC.zero_term;}}_LL31C: {
struct Cyc_List_List*_tmp6C6=Cyc_Toc_get_relns(_tmp4F3);int in_bnds=0;{void*
_tmp6C7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BF);_LL320:;_LL321: in_bnds=Cyc_Toc_check_bounds(_tmp6B4,_tmp6C6,_tmp4F3,
_tmp4F4);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCB8;void*
_tmpCB7[1];struct Cyc_String_pa_struct _tmpCB6;(_tmpCB6.tag=0,((_tmpCB6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB7[0]=&
_tmpCB6,Cyc_Tcutil_warn(e->loc,((_tmpCB8="bounds check necessary for %s",
_tag_dyneither(_tmpCB8,sizeof(char),30))),_tag_dyneither(_tmpCB7,sizeof(void*),1)))))));}
_LL31F:;}Cyc_Toc_exp_to_c(nv,_tmp4F3);Cyc_Toc_exp_to_c(nv,_tmp4F4);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6BF);struct Cyc_Absyn_Exp*_tmp6CD;_LL323: {struct Cyc_Absyn_Upper_b_struct*
_tmp6CC=(struct Cyc_Absyn_Upper_b_struct*)_tmp6CB;if(_tmp6CC->tag != 1)goto _LL325;
else{_tmp6CD=_tmp6CC->f1;}}_LL324: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6BE);void*ta1=Cyc_Toc_typ_to_c(_tmp6BA);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6BB);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6C0)){if(!Cyc_Evexp_c_can_eval(_tmp6CD)){const char*_tmpCBB;void*_tmpCBA;(
_tmpCBA=0,Cyc_Tcutil_terr(e->loc,((_tmpCBB="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCBB,sizeof(char),40))),_tag_dyneither(_tmpCBA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4F3);struct Cyc_Absyn_Exp*_tmpCBC[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCBC[2]=_tmp4F4,((_tmpCBC[1]=_tmp6CD,((
_tmpCBC[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCBC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6CD)){const char*_tmpCBF;void*_tmpCBE;(
_tmpCBE=0,Cyc_Tcutil_terr(e->loc,((_tmpCBF="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCBF,sizeof(char),40))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCC2;void*_tmpCC1;(_tmpCC1=0,Cyc_Tcutil_warn(
e->loc,((_tmpCC2="inserted null check due to dereference",_tag_dyneither(_tmpCC2,
sizeof(char),39))),_tag_dyneither(_tmpCC1,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC3[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCC3[3]=_tmp4F4,((_tmpCC3[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCC3[1]=
_tmp6CD,((_tmpCC3[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC3,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6CD)){const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,Cyc_Tcutil_terr(
e->loc,((_tmpCC6="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCC6,sizeof(char),40))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCC7[2];_tmp4F4->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCC7[1]=Cyc_Absyn_copy_exp(_tmp4F4),((_tmpCC7[0]=_tmp6CD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC7,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL322;}_LL325: {struct Cyc_Absyn_DynEither_b_struct*_tmp6CE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6CB;if(_tmp6CE->tag != 0)goto _LL322;}
_LL326: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6BA);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BB),
Cyc_Absyn_aggrmember_exp(_tmp4F3,Cyc_Toc_curr_sp,0)),_tmp4F4);}else{struct Cyc_Absyn_Exp*
_tmp6DA=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCC8[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BB),Cyc_Absyn_fncall_exp(
_tmp6DA,((_tmpCC8[2]=_tmp4F4,((_tmpCC8[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCC8[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL322;}
_LL322:;}goto _LL318;}_LL31D:;_LL31E: {const char*_tmpCCB;void*_tmpCCA;(_tmpCCA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCCB="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCCB,sizeof(char),49))),_tag_dyneither(_tmpCCA,sizeof(void*),0)));}_LL318:;}
goto _LL212;}_LL24B: {struct Cyc_Absyn_Tuple_e_struct*_tmp4F5=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp496;if(_tmp4F5->tag != 25)goto _LL24D;else{_tmp4F6=_tmp4F5->f1;}}_LL24C: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F6))->r;else{struct
Cyc_List_List*_tmp6DE=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F6);void*_tmp6DF=Cyc_Toc_add_tuple_type(
_tmp6DE);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F6 != 0;(_tmp4F6=_tmp4F6->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F6->hd);{struct _tuple15*
_tmpCCE;struct Cyc_List_List*_tmpCCD;dles=((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((
_tmpCCD->hd=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=0,((_tmpCCE->f2=(
struct Cyc_Absyn_Exp*)_tmp4F6->hd,_tmpCCE)))))),((_tmpCCD->tl=dles,_tmpCCD))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL212;_LL24D: {struct Cyc_Absyn_Array_e_struct*
_tmp4F7=(struct Cyc_Absyn_Array_e_struct*)_tmp496;if(_tmp4F7->tag != 27)goto _LL24F;
else{_tmp4F8=_tmp4F7->f1;}}_LL24E: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F8);{
struct Cyc_List_List*_tmp6E2=_tmp4F8;for(0;_tmp6E2 != 0;_tmp6E2=_tmp6E2->tl){
struct _tuple15 _tmp6E4;struct Cyc_Absyn_Exp*_tmp6E5;struct _tuple15*_tmp6E3=(struct
_tuple15*)_tmp6E2->hd;_tmp6E4=*_tmp6E3;_tmp6E5=_tmp6E4.f2;Cyc_Toc_exp_to_c(nv,
_tmp6E5);}}goto _LL212;_LL24F: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4F9=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp496;if(_tmp4F9->tag != 28)goto _LL251;
else{_tmp4FA=_tmp4F9->f1;_tmp4FB=_tmp4F9->f2;_tmp4FC=_tmp4F9->f3;_tmp4FD=_tmp4F9->f4;}}
_LL250: {unsigned int _tmp6E7;int _tmp6E8;struct _tuple10 _tmp6E6=Cyc_Evexp_eval_const_uint_exp(
_tmp4FB);_tmp6E7=_tmp6E6.f1;_tmp6E8=_tmp6E6.f2;{void*_tmp6E9=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FC->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4FC);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4FC)){if(!_tmp6E8){
const char*_tmpCD1;void*_tmpCD0;(_tmpCD0=0,Cyc_Tcutil_terr(_tmp4FB->loc,((_tmpCD1="cannot determine value of constant",
_tag_dyneither(_tmpCD1,sizeof(char),35))),_tag_dyneither(_tmpCD0,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6E7;++ i){struct _tuple15*_tmpCD4;struct Cyc_List_List*
_tmpCD3;es=((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=((_tmpCD4=
_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->f1=0,((_tmpCD4->f2=_tmp4FC,_tmpCD4)))))),((
_tmpCD3->tl=es,_tmpCD3))))));}}if(_tmp4FD){struct Cyc_Absyn_Exp*_tmp6EE=Cyc_Toc_cast_it(
_tmp6E9,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCD7;struct Cyc_List_List*
_tmpCD6;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->hd=((
_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->f1=0,((_tmpCD7->f2=_tmp6EE,
_tmpCD7)))))),((_tmpCD6->tl=0,_tmpCD6)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL212;};};}_LL251: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4FE=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp496;if(_tmp4FE->tag != 29)goto _LL253;
else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;_tmp501=_tmp4FE->f3;_tmp502=_tmp4FE->f4;}}
_LL252: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp500
!= 0,0,0,_tmp501,_tmp4FF))->r;else{if(_tmp502 == 0){const char*_tmpCDA;void*
_tmpCD9;(_tmpCD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCDA="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCDA,sizeof(
char),38))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp502;struct Cyc_Toc_Env _tmp6F4;struct
_RegionHandle*_tmp6F5;struct Cyc_Toc_Env*_tmp6F3=nv;_tmp6F4=*_tmp6F3;_tmp6F5=
_tmp6F4.rgn;{struct Cyc_List_List*_tmp6F6=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6F5,e->loc,_tmp501,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp502->impl))->tagged){struct
_tuple20 _tmp6F8;struct Cyc_Absyn_Aggrfield*_tmp6F9;struct Cyc_Absyn_Exp*_tmp6FA;
struct _tuple20*_tmp6F7=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6F6))->hd;_tmp6F8=*_tmp6F7;_tmp6F9=_tmp6F8.f1;_tmp6FA=_tmp6F8.f2;{void*
_tmp6FB=_tmp6F9->type;Cyc_Toc_exp_to_c(nv,_tmp6FA);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6FB))_tmp6FA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6FA->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp502,_tmp6F9->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCDF;struct _tuple15*_tmpCDE;struct _tuple15*_tmpCDD[2];
struct Cyc_List_List*_tmp6FC=(_tmpCDD[1]=((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE->f1=0,((_tmpCDE->f2=_tmp6FA,_tmpCDE)))))),((_tmpCDD[0]=((_tmpCDF=
_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->f1=0,((_tmpCDF->f2=field_tag_exp,_tmpCDF)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDD,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6FC,0);struct Cyc_Absyn_FieldName_struct*_tmpCE5;struct Cyc_Absyn_FieldName_struct
_tmpCE4;void*_tmpCE3[1];struct Cyc_List_List*ds=(_tmpCE3[0]=(void*)((_tmpCE5=
_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE4.tag=1,((_tmpCE4.f1=_tmp6F9->name,
_tmpCE4)))),_tmpCE5)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE3,sizeof(void*),1)));struct _tuple15*_tmpCE8;struct _tuple15*
_tmpCE7[1];struct Cyc_List_List*dles=(_tmpCE7[0]=((_tmpCE8=_cycalloc(sizeof(*
_tmpCE8)),((_tmpCE8->f1=ds,((_tmpCE8->f2=umem,_tmpCE8)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE7,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp705=0;struct Cyc_List_List*_tmp706=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp706 != 0;_tmp706=_tmp706->tl){struct Cyc_List_List*
_tmp707=_tmp6F6;for(0;_tmp707 != 0;_tmp707=_tmp707->tl){if((*((struct _tuple20*)
_tmp707->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp706->hd){struct _tuple20
_tmp709;struct Cyc_Absyn_Aggrfield*_tmp70A;struct Cyc_Absyn_Exp*_tmp70B;struct
_tuple20*_tmp708=(struct _tuple20*)_tmp707->hd;_tmp709=*_tmp708;_tmp70A=_tmp709.f1;
_tmp70B=_tmp709.f2;{void*_tmp70C=_tmp70A->type;Cyc_Toc_exp_to_c(nv,_tmp70B);if(
Cyc_Toc_is_void_star_or_tvar(_tmp70C))_tmp70B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp70B->r,0));{struct _tuple15*_tmpCEB;struct Cyc_List_List*
_tmpCEA;_tmp705=((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=((_tmpCEB=
_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->f1=0,((_tmpCEB->f2=_tmp70B,_tmpCEB)))))),((
_tmpCEA->tl=_tmp705,_tmpCEA))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp705));}};};}
goto _LL212;_LL253: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp503=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp496;if(_tmp503->tag != 30)goto _LL255;else{_tmp504=(void*)_tmp503->f1;_tmp505=
_tmp503->f2;}}_LL254: {struct Cyc_List_List*fs;{void*_tmp70F=Cyc_Tcutil_compress(
_tmp504);struct Cyc_List_List*_tmp711;_LL328: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp710=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp70F;if(_tmp710->tag != 13)goto
_LL32A;else{_tmp711=_tmp710->f2;}}_LL329: fs=_tmp711;goto _LL327;_LL32A:;_LL32B: {
const char*_tmpCEF;void*_tmpCEE[1];struct Cyc_String_pa_struct _tmpCED;(_tmpCED.tag=
0,((_tmpCED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp504)),((_tmpCEE[0]=& _tmpCED,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEF="anon struct has type %s",
_tag_dyneither(_tmpCEF,sizeof(char),24))),_tag_dyneither(_tmpCEE,sizeof(void*),1)))))));}
_LL327:;}{struct Cyc_Toc_Env _tmp716;struct _RegionHandle*_tmp717;struct Cyc_Toc_Env*
_tmp715=nv;_tmp716=*_tmp715;_tmp717=_tmp716.rgn;{struct Cyc_List_List*_tmp718=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp717,e->loc,_tmp505,Cyc_Absyn_StructA,fs);for(0;_tmp718 != 0;_tmp718=_tmp718->tl){
struct _tuple20 _tmp71A;struct Cyc_Absyn_Aggrfield*_tmp71B;struct Cyc_Absyn_Exp*
_tmp71C;struct _tuple20*_tmp719=(struct _tuple20*)_tmp718->hd;_tmp71A=*_tmp719;
_tmp71B=_tmp71A.f1;_tmp71C=_tmp71A.f2;{void*_tmp71D=_tmp71B->type;Cyc_Toc_exp_to_c(
nv,_tmp71C);if(Cyc_Toc_is_void_star_or_tvar(_tmp71D))_tmp71C->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp71C->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp505);}goto _LL212;};}_LL255: {struct Cyc_Absyn_Datatype_e_struct*_tmp506=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp496;if(_tmp506->tag != 31)goto _LL257;else{
_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;_tmp509=_tmp506->f3;}}_LL256: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp71E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp71F=Cyc_Absyn_var_exp(_tmp71E,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCF0;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp509->name,((_tmpCF0="_struct",_tag_dyneither(_tmpCF0,sizeof(char),8)))));}
tag_exp=_tmp508->is_extensible?Cyc_Absyn_var_exp(_tmp509->name,0): Cyc_Toc_datatype_tag(
_tmp508,_tmp509->name);member_exp=_tmp71F;{struct Cyc_List_List*_tmp721=_tmp509->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp507 != 0;(
_tmp507=_tmp507->tl,_tmp721=_tmp721->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp507->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp721))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCF3;struct
Cyc_List_List*_tmpCF2;dles=((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->hd=((
_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->f1=0,((_tmpCF3->f2=cur_e,_tmpCF3)))))),((
_tmpCF2->tl=dles,_tmpCF2))))));};}{struct _tuple15*_tmpCF6;struct Cyc_List_List*
_tmpCF5;dles=((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=((_tmpCF6=
_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=0,((_tmpCF6->f2=tag_exp,_tmpCF6)))))),((
_tmpCF5->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCF5))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCF7;struct Cyc_List_List*_tmp726=(_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((
_tmpCF7->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCF7->tl=0,_tmpCF7)))));{int i=1;for(0;_tmp507 != 0;(((_tmp507=
_tmp507->tl,i ++)),_tmp721=_tmp721->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp507->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp721))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp727=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCF8;_tmp726=((_tmpCF8=_cycalloc(sizeof(*
_tmpCF8)),((_tmpCF8->hd=_tmp727,((_tmpCF8->tl=_tmp726,_tmpCF8))))));};}}{struct
Cyc_Absyn_Stmt*_tmp729=Cyc_Absyn_exp_stmt(_tmp71F,0);struct Cyc_List_List*_tmpCF9;
struct Cyc_Absyn_Stmt*_tmp72A=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((
_tmpCF9->hd=_tmp729,((_tmpCF9->tl=_tmp726,_tmpCF9))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp71E,datatype_ctype,0,_tmp72A,0));};}goto _LL212;};}
_LL257: {struct Cyc_Absyn_Enum_e_struct*_tmp50A=(struct Cyc_Absyn_Enum_e_struct*)
_tmp496;if(_tmp50A->tag != 32)goto _LL259;}_LL258: goto _LL25A;_LL259: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp50B=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp496;if(_tmp50B->tag != 33)goto
_LL25B;}_LL25A: goto _LL212;_LL25B: {struct Cyc_Absyn_Malloc_e_struct*_tmp50C=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp496;if(_tmp50C->tag != 34)goto _LL25D;else{
_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.is_calloc;_tmp50F=_tmp50D.rgn;_tmp510=
_tmp50D.elt_type;_tmp511=_tmp50D.num_elts;_tmp512=_tmp50D.fat_result;}}_LL25C: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp510)));Cyc_Toc_exp_to_c(nv,
_tmp511);if(_tmp512){struct _tuple0*_tmp72D=Cyc_Toc_temp_var();struct _tuple0*
_tmp72E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp50E){xexp=_tmp511;if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp72D,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp510,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp72D,0));}{struct Cyc_Absyn_Exp*_tmpCFA[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCFA[2]=Cyc_Absyn_var_exp(_tmp72D,0),((_tmpCFA[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCFA[0]=Cyc_Absyn_var_exp(_tmp72E,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCFA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp511,0);if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp72D,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp510,Cyc_Absyn_var_exp(_tmp72D,0));}{struct Cyc_Absyn_Exp*_tmpCFB[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCFB[2]=Cyc_Absyn_var_exp(_tmp72D,0),((_tmpCFB[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCFB[0]=Cyc_Absyn_var_exp(_tmp72E,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp731=Cyc_Absyn_declare_stmt(_tmp72D,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp72E,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp731);};}else{struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp733=_tmp511->r;union Cyc_Absyn_Cnst _tmp735;struct _tuple5 _tmp736;
int _tmp737;_LL32D: {struct Cyc_Absyn_Const_e_struct*_tmp734=(struct Cyc_Absyn_Const_e_struct*)
_tmp733;if(_tmp734->tag != 0)goto _LL32F;else{_tmp735=_tmp734->f1;if((_tmp735.Int_c).tag
!= 4)goto _LL32F;_tmp736=(struct _tuple5)(_tmp735.Int_c).val;_tmp737=_tmp736.f2;
if(_tmp737 != 1)goto _LL32F;}}_LL32E: goto _LL32C;_LL32F:;_LL330: _tmp732=Cyc_Absyn_times_exp(
_tmp732,_tmp511,0);goto _LL32C;_LL32C:;}if(_tmp50F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50F;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp732))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp510,
_tmp732))->r;}}goto _LL212;}_LL25D: {struct Cyc_Absyn_Swap_e_struct*_tmp513=(
struct Cyc_Absyn_Swap_e_struct*)_tmp496;if(_tmp513->tag != 35)goto _LL25F;else{
_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;}}_LL25E: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp514->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp515->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCFE;void*_tmpCFD;(_tmpCFD=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFE="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCFE,sizeof(char),57))),_tag_dyneither(_tmpCFD,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp514);Cyc_Toc_exp_to_c(
nv,_tmp515);{struct Cyc_Absyn_Exp*_tmpCFF[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCFF[1]=Cyc_Toc_push_address_exp(_tmp515),((_tmpCFF[0]=Cyc_Toc_push_address_exp(
_tmp514),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCFF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL212;};}
_LL25F: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp516=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp496;if(_tmp516->tag != 38)goto _LL261;else{_tmp517=_tmp516->f1;_tmp518=_tmp516->f2;}}
_LL260: {void*_tmp73B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp517->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp517);{void*_tmp73C=
_tmp73B;struct Cyc_Absyn_AggrInfo _tmp73E;union Cyc_Absyn_AggrInfoU _tmp73F;struct
Cyc_Absyn_Aggrdecl**_tmp740;struct Cyc_Absyn_Aggrdecl*_tmp741;_LL332: {struct Cyc_Absyn_AggrType_struct*
_tmp73D=(struct Cyc_Absyn_AggrType_struct*)_tmp73C;if(_tmp73D->tag != 12)goto
_LL334;else{_tmp73E=_tmp73D->f1;_tmp73F=_tmp73E.aggr_info;if((_tmp73F.KnownAggr).tag
!= 2)goto _LL334;_tmp740=(struct Cyc_Absyn_Aggrdecl**)(_tmp73F.KnownAggr).val;
_tmp741=*_tmp740;}}_LL333: {struct Cyc_Absyn_Exp*_tmp742=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp741,_tmp518),0);struct Cyc_Absyn_Exp*_tmp743=Cyc_Absyn_aggrmember_exp(
_tmp517,_tmp518,0);struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_aggrmember_exp(_tmp743,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp744,_tmp742,0))->r;goto _LL331;}
_LL334:;_LL335: {const char*_tmpD02;void*_tmpD01;(_tmpD01=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD02="non-aggregate type in tagcheck",
_tag_dyneither(_tmpD02,sizeof(char),31))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}
_LL331:;}goto _LL212;}_LL261: {struct Cyc_Absyn_StmtExp_e_struct*_tmp519=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp496;if(_tmp519->tag != 37)goto _LL263;else{_tmp51A=
_tmp519->f1;}}_LL262: Cyc_Toc_stmt_to_c(nv,_tmp51A);goto _LL212;_LL263: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp51B=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp496;if(_tmp51B->tag != 36)
goto _LL265;}_LL264: {const char*_tmpD05;void*_tmpD04;(_tmpD04=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD05="UnresolvedMem",
_tag_dyneither(_tmpD05,sizeof(char),14))),_tag_dyneither(_tmpD04,sizeof(void*),0)));}
_LL265: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp51C=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp496;if(_tmp51C->tag != 26)goto _LL267;}_LL266: {const char*_tmpD08;void*_tmpD07;(
_tmpD07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpD08="compoundlit",_tag_dyneither(_tmpD08,sizeof(char),12))),_tag_dyneither(
_tmpD07,sizeof(void*),0)));}_LL267: {struct Cyc_Absyn_Valueof_e_struct*_tmp51D=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp496;if(_tmp51D->tag != 39)goto _LL212;}
_LL268: {const char*_tmpD0B;void*_tmpD0A;(_tmpD0A=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0B="valueof(-)",
_tag_dyneither(_tmpD0B,sizeof(char),11))),_tag_dyneither(_tmpD0A,sizeof(void*),0)));}
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
struct Cyc_Absyn_Stmt*s;{void*_tmp74D=p->r;struct Cyc_Absyn_Vardecl*_tmp74F;struct
Cyc_Absyn_Vardecl _tmp750;struct _tuple0*_tmp751;struct Cyc_Absyn_Pat*_tmp752;
struct Cyc_Absyn_Vardecl*_tmp754;struct Cyc_Absyn_Vardecl _tmp755;struct _tuple0*
_tmp756;struct Cyc_Absyn_Vardecl*_tmp759;struct Cyc_Absyn_Pat*_tmp75A;enum Cyc_Absyn_Sign
_tmp75D;int _tmp75E;char _tmp760;struct _dyneither_ptr _tmp762;struct Cyc_Absyn_Enumdecl*
_tmp764;struct Cyc_Absyn_Enumfield*_tmp765;void*_tmp767;struct Cyc_Absyn_Enumfield*
_tmp768;struct Cyc_Absyn_Pat*_tmp76A;struct Cyc_Absyn_Pat _tmp76B;void*_tmp76C;
struct Cyc_Absyn_Datatypedecl*_tmp76E;struct Cyc_Absyn_Datatypefield*_tmp76F;
struct Cyc_List_List*_tmp770;struct Cyc_List_List*_tmp772;struct Cyc_List_List*
_tmp774;struct Cyc_Absyn_AggrInfo*_tmp776;struct Cyc_Absyn_AggrInfo*_tmp778;struct
Cyc_Absyn_AggrInfo _tmp779;union Cyc_Absyn_AggrInfoU _tmp77A;struct Cyc_List_List*
_tmp77B;struct Cyc_Absyn_Pat*_tmp77D;_LL337: {struct Cyc_Absyn_Var_p_struct*
_tmp74E=(struct Cyc_Absyn_Var_p_struct*)_tmp74D;if(_tmp74E->tag != 1)goto _LL339;
else{_tmp74F=_tmp74E->f1;_tmp750=*_tmp74F;_tmp751=_tmp750.name;_tmp752=_tmp74E->f2;}}
_LL338: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp751,r),rgn,t,r,path,
_tmp752,fail_stmt,decls);_LL339: {struct Cyc_Absyn_TagInt_p_struct*_tmp753=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp74D;if(_tmp753->tag != 3)goto _LL33B;else{
_tmp754=_tmp753->f2;_tmp755=*_tmp754;_tmp756=_tmp755.name;}}_LL33A: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp756,r);goto _LL33C;_LL33B: {struct Cyc_Absyn_Wild_p_struct*_tmp757=(
struct Cyc_Absyn_Wild_p_struct*)_tmp74D;if(_tmp757->tag != 0)goto _LL33D;}_LL33C: s=
Cyc_Toc_skip_stmt_dl();goto _LL336;_LL33D: {struct Cyc_Absyn_Reference_p_struct*
_tmp758=(struct Cyc_Absyn_Reference_p_struct*)_tmp74D;if(_tmp758->tag != 2)goto
_LL33F;else{_tmp759=_tmp758->f1;_tmp75A=_tmp758->f2;}}_LL33E: {struct _tuple0*
_tmp781=Cyc_Toc_temp_var();{struct _tuple22*_tmpD0E;struct Cyc_List_List*_tmpD0D;
decls=((_tmpD0D=_region_malloc(rgn,sizeof(*_tmpD0D)),((_tmpD0D->hd=((_tmpD0E=
_region_malloc(rgn,sizeof(*_tmpD0E)),((_tmpD0E->f1=_tmp781,((_tmpD0E->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD0E)))))),((_tmpD0D->tl=decls,_tmpD0D))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp759->name,Cyc_Absyn_var_exp(_tmp781,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp781,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp784=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp75A,fail_stmt,decls);_tmp784.f3=Cyc_Absyn_seq_stmt(s,_tmp784.f3,
0);return _tmp784;};}_LL33F: {struct Cyc_Absyn_Null_p_struct*_tmp75B=(struct Cyc_Absyn_Null_p_struct*)
_tmp74D;if(_tmp75B->tag != 8)goto _LL341;}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL336;_LL341: {struct Cyc_Absyn_Int_p_struct*_tmp75C=(struct
Cyc_Absyn_Int_p_struct*)_tmp74D;if(_tmp75C->tag != 9)goto _LL343;else{_tmp75D=
_tmp75C->f1;_tmp75E=_tmp75C->f2;}}_LL342: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp75D,_tmp75E,0),fail_stmt);goto _LL336;_LL343: {struct Cyc_Absyn_Char_p_struct*
_tmp75F=(struct Cyc_Absyn_Char_p_struct*)_tmp74D;if(_tmp75F->tag != 10)goto _LL345;
else{_tmp760=_tmp75F->f1;}}_LL344: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp760,0),fail_stmt);goto _LL336;_LL345: {struct Cyc_Absyn_Float_p_struct*_tmp761=(
struct Cyc_Absyn_Float_p_struct*)_tmp74D;if(_tmp761->tag != 11)goto _LL347;else{
_tmp762=_tmp761->f1;}}_LL346: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp762,
0),fail_stmt);goto _LL336;_LL347: {struct Cyc_Absyn_Enum_p_struct*_tmp763=(struct
Cyc_Absyn_Enum_p_struct*)_tmp74D;if(_tmp763->tag != 12)goto _LL349;else{_tmp764=
_tmp763->f1;_tmp765=_tmp763->f2;}}_LL348:{struct Cyc_Absyn_Enum_e_struct _tmpD11;
struct Cyc_Absyn_Enum_e_struct*_tmpD10;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD11.tag=32,((
_tmpD11.f1=_tmp765->name,((_tmpD11.f2=(struct Cyc_Absyn_Enumdecl*)_tmp764,((
_tmpD11.f3=(struct Cyc_Absyn_Enumfield*)_tmp765,_tmpD11)))))))),_tmpD10)))),0),
fail_stmt);}goto _LL336;_LL349: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp766=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp74D;if(_tmp766->tag != 13)goto _LL34B;else{
_tmp767=(void*)_tmp766->f1;_tmp768=_tmp766->f2;}}_LL34A:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD14;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD13;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[0]=((_tmpD14.tag=33,((
_tmpD14.f1=_tmp768->name,((_tmpD14.f2=(void*)_tmp767,((_tmpD14.f3=(struct Cyc_Absyn_Enumfield*)
_tmp768,_tmpD14)))))))),_tmpD13)))),0),fail_stmt);}goto _LL336;_LL34B: {struct Cyc_Absyn_Pointer_p_struct*
_tmp769=(struct Cyc_Absyn_Pointer_p_struct*)_tmp74D;if(_tmp769->tag != 5)goto
_LL34D;else{_tmp76A=_tmp769->f1;_tmp76B=*_tmp76A;_tmp76C=_tmp76B.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp76D=(struct Cyc_Absyn_Datatype_p_struct*)_tmp76C;if(_tmp76D->tag != 7)goto
_LL34D;else{_tmp76E=_tmp76D->f1;_tmp76F=_tmp76D->f2;_tmp770=_tmp76D->f3;}};}}
_LL34C: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp789=Cyc_Toc_temp_var();const
char*_tmpD15;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp76F->name,((
_tmpD15="_struct",_tag_dyneither(_tmpD15,sizeof(char),8))));void*_tmp78A=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp78A,r);struct Cyc_List_List*_tmp78B=_tmp76F->typs;for(0;_tmp770 != 0;(((
_tmp770=_tmp770->tl,_tmp78B=((struct Cyc_List_List*)_check_null(_tmp78B))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp78C=(struct Cyc_Absyn_Pat*)_tmp770->hd;if(_tmp78C->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp78D=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp78B))->hd)).f2;struct _tuple0*_tmp78E=Cyc_Toc_temp_var();
void*_tmp78F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp78C->topt))->v;void*
_tmp790=Cyc_Toc_typ_to_c(_tmp78F);struct Cyc_Absyn_Exp*_tmp791=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp789,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp78D)))_tmp791=Cyc_Toc_cast_it(_tmp790,_tmp791);{struct
_tuple22*_tmpD18;struct Cyc_List_List*_tmpD17;decls=((_tmpD17=_region_malloc(rgn,
sizeof(*_tmpD17)),((_tmpD17->hd=((_tmpD18=_region_malloc(rgn,sizeof(*_tmpD18)),((
_tmpD18->f1=_tmp78E,((_tmpD18->f2=_tmp790,_tmpD18)))))),((_tmpD17->tl=decls,
_tmpD17))))));}{struct _tuple21 _tmp794=Cyc_Toc_xlate_pat(nv,rgn,_tmp78F,Cyc_Absyn_var_exp(
_tmp78E,0),_tmp791,_tmp78C,fail_stmt,decls);nv=_tmp794.f1;decls=_tmp794.f2;{
struct Cyc_Absyn_Stmt*_tmp795=_tmp794.f3;struct Cyc_Absyn_Stmt*_tmp796=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp78E,0),_tmp791,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp796,_tmp795,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp789,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp76E->is_extensible?Cyc_Absyn_var_exp(_tmp76F->name,0): Cyc_Toc_datatype_tag(
_tmp76E,_tmp76F->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp789,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp789,_tmp78A,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL336;};};_LL34D: {struct Cyc_Absyn_Datatype_p_struct*_tmp771=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp74D;if(_tmp771->tag != 7)goto _LL34F;else{
_tmp772=_tmp771->f3;}}_LL34E: _tmp774=_tmp772;goto _LL350;_LL34F: {struct Cyc_Absyn_Tuple_p_struct*
_tmp773=(struct Cyc_Absyn_Tuple_p_struct*)_tmp74D;if(_tmp773->tag != 4)goto _LL351;
else{_tmp774=_tmp773->f1;}}_LL350: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp774 != 0;(_tmp774=_tmp774->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp798=(struct Cyc_Absyn_Pat*)
_tmp774->hd;if(_tmp798->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp799=Cyc_Toc_temp_var();void*_tmp79A=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp798->topt))->v;{struct _tuple22*_tmpD1B;struct Cyc_List_List*_tmpD1A;decls=((
_tmpD1A=_region_malloc(rgn,sizeof(*_tmpD1A)),((_tmpD1A->hd=((_tmpD1B=
_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->f1=_tmp799,((_tmpD1B->f2=Cyc_Toc_typ_to_c(
_tmp79A),_tmpD1B)))))),((_tmpD1A->tl=decls,_tmpD1A))))));}{struct _tuple21 _tmp79D=
Cyc_Toc_xlate_pat(nv,rgn,_tmp79A,Cyc_Absyn_var_exp(_tmp799,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp798,fail_stmt,decls);nv=_tmp79D.f1;decls=
_tmp79D.f2;{struct Cyc_Absyn_Stmt*_tmp79E=_tmp79D.f3;struct Cyc_Absyn_Stmt*_tmp79F=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp799,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp79F,
_tmp79E,0),0);};};};}goto _LL336;};_LL351: {struct Cyc_Absyn_Aggr_p_struct*_tmp775=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp74D;if(_tmp775->tag != 6)goto _LL353;else{
_tmp776=_tmp775->f1;if(_tmp776 != 0)goto _LL353;}}_LL352: {const char*_tmpD1E;void*
_tmpD1D;(_tmpD1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD1E="unresolved aggregate pattern!",_tag_dyneither(_tmpD1E,sizeof(char),30))),
_tag_dyneither(_tmpD1D,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Aggr_p_struct*
_tmp777=(struct Cyc_Absyn_Aggr_p_struct*)_tmp74D;if(_tmp777->tag != 6)goto _LL355;
else{_tmp778=_tmp777->f1;if(_tmp778 == 0)goto _LL355;_tmp779=*_tmp778;_tmp77A=
_tmp779.aggr_info;_tmp77B=_tmp777->f3;}}_LL354: {struct Cyc_Absyn_Aggrdecl*
_tmp7A2=Cyc_Absyn_get_known_aggrdecl(_tmp77A);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7A2->impl))->tagged){struct _tuple23 _tmp7A4;struct Cyc_List_List*
_tmp7A5;struct Cyc_Absyn_Pat*_tmp7A6;struct _tuple23*_tmp7A3=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp77B))->hd;_tmp7A4=*_tmp7A3;_tmp7A5=_tmp7A4.f1;
_tmp7A6=_tmp7A4.f2;{struct _dyneither_ptr*f;{void*_tmp7A7=(void*)((struct Cyc_List_List*)
_check_null(_tmp7A5))->hd;struct _dyneither_ptr*_tmp7A9;_LL35E: {struct Cyc_Absyn_FieldName_struct*
_tmp7A8=(struct Cyc_Absyn_FieldName_struct*)_tmp7A7;if(_tmp7A8->tag != 1)goto
_LL360;else{_tmp7A9=_tmp7A8->f1;}}_LL35F: f=_tmp7A9;goto _LL35D;_LL360:;_LL361: {
const char*_tmpD21;void*_tmpD20;(_tmpD20=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD21="no field name in tagged union pattern",
_tag_dyneither(_tmpD21,sizeof(char),38))),_tag_dyneither(_tmpD20,sizeof(void*),0)));}
_LL35D:;}{struct _tuple0*_tmp7AC=Cyc_Toc_temp_var();void*_tmp7AD=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7A6->topt))->v;void*_tmp7AE=Cyc_Toc_typ_to_c(
_tmp7AD);{struct _tuple22*_tmpD24;struct Cyc_List_List*_tmpD23;decls=((_tmpD23=
_region_malloc(rgn,sizeof(*_tmpD23)),((_tmpD23->hd=((_tmpD24=_region_malloc(rgn,
sizeof(*_tmpD24)),((_tmpD24->f1=_tmp7AC,((_tmpD24->f2=_tmp7AE,_tmpD24)))))),((
_tmpD23->tl=decls,_tmpD23))))));}{struct Cyc_Absyn_Exp*_tmp7B1=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7B2=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7B2=Cyc_Toc_cast_it(_tmp7AE,_tmp7B2);_tmp7B1=Cyc_Toc_cast_it(_tmp7AE,_tmp7B1);{
struct _tuple21 _tmp7B3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AD,Cyc_Absyn_var_exp(_tmp7AC,
0),_tmp7B1,_tmp7A6,fail_stmt,decls);nv=_tmp7B3.f1;decls=_tmp7B3.f2;{struct Cyc_Absyn_Stmt*
_tmp7B4=_tmp7B3.f3;struct Cyc_Absyn_Stmt*_tmp7B5=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7A2,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7B6=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7AC,0),_tmp7B2,0);s=Cyc_Absyn_seq_stmt(_tmp7B5,Cyc_Absyn_seq_stmt(_tmp7B6,
_tmp7B4,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp77B != 0;_tmp77B=
_tmp77B->tl){struct _tuple23*_tmp7B7=(struct _tuple23*)_tmp77B->hd;struct Cyc_Absyn_Pat*
_tmp7B8=(*_tmp7B7).f2;if(_tmp7B8->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7B9=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7B7).f1))->hd;struct _dyneither_ptr*_tmp7BB;_LL363: {struct Cyc_Absyn_FieldName_struct*
_tmp7BA=(struct Cyc_Absyn_FieldName_struct*)_tmp7B9;if(_tmp7BA->tag != 1)goto
_LL365;else{_tmp7BB=_tmp7BA->f1;}}_LL364: f=_tmp7BB;goto _LL362;_LL365:;_LL366: {
struct Cyc_Toc_Match_error_struct _tmpD27;struct Cyc_Toc_Match_error_struct*_tmpD26;(
int)_throw((void*)((_tmpD26=_cycalloc_atomic(sizeof(*_tmpD26)),((_tmpD26[0]=((
_tmpD27.tag=Cyc_Toc_Match_error,_tmpD27)),_tmpD26)))));}_LL362:;}{struct _tuple0*
_tmp7BE=Cyc_Toc_temp_var();void*_tmp7BF=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7B8->topt))->v;void*_tmp7C0=Cyc_Toc_typ_to_c(_tmp7BF);{struct _tuple22*
_tmpD2A;struct Cyc_List_List*_tmpD29;decls=((_tmpD29=_region_malloc(rgn,sizeof(*
_tmpD29)),((_tmpD29->hd=((_tmpD2A=_region_malloc(rgn,sizeof(*_tmpD2A)),((_tmpD2A->f1=
_tmp7BE,((_tmpD2A->f2=_tmp7C0,_tmpD2A)))))),((_tmpD29->tl=decls,_tmpD29))))));}{
struct _tuple21 _tmp7C3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BF,Cyc_Absyn_var_exp(_tmp7BE,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7B8,fail_stmt,decls);nv=_tmp7C3.f1;
decls=_tmp7C3.f2;{struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7A2->impl))->fields,f)))->type))
_tmp7C4=Cyc_Toc_cast_it(_tmp7C0,_tmp7C4);{struct Cyc_Absyn_Stmt*_tmp7C5=_tmp7C3.f3;
struct Cyc_Absyn_Stmt*_tmp7C6=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BE,0),
_tmp7C4,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7C6,_tmp7C5,0),0);};};};};};}}
goto _LL336;}_LL355: {struct Cyc_Absyn_Pointer_p_struct*_tmp77C=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp74D;if(_tmp77C->tag != 5)goto _LL357;else{_tmp77D=_tmp77C->f1;}}_LL356: {
struct _tuple0*_tmp7C7=Cyc_Toc_temp_var();void*_tmp7C8=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77D->topt))->v;{struct _tuple22*_tmpD2D;struct Cyc_List_List*
_tmpD2C;decls=((_tmpD2C=_region_malloc(rgn,sizeof(*_tmpD2C)),((_tmpD2C->hd=((
_tmpD2D=_region_malloc(rgn,sizeof(*_tmpD2D)),((_tmpD2D->f1=_tmp7C7,((_tmpD2D->f2=
Cyc_Toc_typ_to_c(_tmp7C8),_tmpD2D)))))),((_tmpD2C->tl=decls,_tmpD2C))))));}{
struct _tuple21 _tmp7CB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C8,Cyc_Absyn_var_exp(_tmp7C7,
0),Cyc_Absyn_deref_exp(path,0),_tmp77D,fail_stmt,decls);nv=_tmp7CB.f1;decls=
_tmp7CB.f2;{struct Cyc_Absyn_Stmt*_tmp7CC=_tmp7CB.f3;struct Cyc_Absyn_Stmt*_tmp7CD=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C7,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7CC,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7CD,0);else{s=_tmp7CD;}goto _LL336;};};}_LL357: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp77E=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp74D;if(_tmp77E->tag != 14)goto
_LL359;}_LL358: {const char*_tmpD30;void*_tmpD2F;(_tmpD2F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD30="unknownid pat",
_tag_dyneither(_tmpD30,sizeof(char),14))),_tag_dyneither(_tmpD2F,sizeof(void*),0)));}
_LL359: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp77F=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp74D;if(_tmp77F->tag != 15)goto _LL35B;}_LL35A: {const char*_tmpD33;void*_tmpD32;(
_tmpD32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD33="unknowncall pat",_tag_dyneither(_tmpD33,sizeof(char),16))),
_tag_dyneither(_tmpD32,sizeof(void*),0)));}_LL35B: {struct Cyc_Absyn_Exp_p_struct*
_tmp780=(struct Cyc_Absyn_Exp_p_struct*)_tmp74D;if(_tmp780->tag != 16)goto _LL336;}
_LL35C: {const char*_tmpD36;void*_tmpD35;(_tmpD35=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD36="exp pat",
_tag_dyneither(_tmpD36,sizeof(char),8))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}
_LL336:;}{struct _tuple21 _tmpD37;return(_tmpD37.f1=nv,((_tmpD37.f2=decls,((
_tmpD37.f3=s,_tmpD37)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD38;
return(_tmpD38=_region_malloc(r,sizeof(*_tmpD38)),((_tmpD38->f1=Cyc_Toc_fresh_label(),((
_tmpD38->f2=Cyc_Toc_fresh_label(),((_tmpD38->f3=sc,_tmpD38)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7D6=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7D7=Cyc_Tcutil_compress(_tmp7D6);_LL368: {struct Cyc_Absyn_IntType_struct*
_tmp7D8=(struct Cyc_Absyn_IntType_struct*)_tmp7D7;if(_tmp7D8->tag != 6)goto _LL36A;}
_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_EnumType_struct*_tmp7D9=(struct Cyc_Absyn_EnumType_struct*)
_tmp7D7;if(_tmp7D9->tag != 14)goto _LL36C;}_LL36B: leave_as_switch=1;goto _LL367;
_LL36C:;_LL36D: leave_as_switch=0;goto _LL367;_LL367:;}{struct Cyc_List_List*
_tmp7DA=scs;for(0;_tmp7DA != 0;_tmp7DA=_tmp7DA->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7DA->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7DA->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7DB=scs;for(0;_tmp7DB != 0;_tmp7DB=_tmp7DB->tl){struct Cyc_Absyn_Stmt*
_tmp7DC=((struct Cyc_Absyn_Switch_clause*)_tmp7DB->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7DB->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7DC,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7DE;struct _RegionHandle*_tmp7DF;struct Cyc_Toc_Env*_tmp7DD=
nv;_tmp7DE=*_tmp7DD;_tmp7DF=_tmp7DE.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7DF,nv,next_l),_tmp7DC);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7E1;
struct _RegionHandle*_tmp7E2;struct Cyc_Toc_Env*_tmp7E0=nv;_tmp7E1=*_tmp7E0;
_tmp7E2=_tmp7E1.rgn;{struct Cyc_Toc_Env*_tmp7E3=Cyc_Toc_share_env(_tmp7E2,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7E2,Cyc_Toc_gen_label,
_tmp7E2,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7E4=lscs;for(0;_tmp7E4 != 0;
_tmp7E4=_tmp7E4->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7E4->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7E4->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7E4->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7E6;struct Cyc_List_List*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E8;struct _tuple21
_tmp7E5=Cyc_Toc_xlate_pat(_tmp7E3,_tmp7E2,_tmp7D6,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7E6=_tmp7E5.f1;_tmp7E7=_tmp7E5.f2;_tmp7E8=_tmp7E5.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7E9=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7E6,_tmp7E9);_tmp7E8=Cyc_Absyn_seq_stmt(
_tmp7E8,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7E9,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7E7;{
struct Cyc_List_List*_tmpD39;nvs=((_tmpD39=_region_malloc(_tmp7E2,sizeof(*_tmpD39)),((
_tmpD39->hd=_tmp7E6,((_tmpD39->tl=nvs,_tmpD39))))));}{struct Cyc_List_List*
_tmpD3A;test_stmts=((_tmpD3A=_region_malloc(_tmp7E2,sizeof(*_tmpD3A)),((_tmpD3A->hd=
_tmp7E8,((_tmpD3A->tl=test_stmts,_tmpD3A))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7ED;struct _dyneither_ptr*_tmp7EE;struct _dyneither_ptr*_tmp7EF;
struct Cyc_Absyn_Switch_clause*_tmp7F0;struct _tuple24*_tmp7EC=(struct _tuple24*)
lscs->hd;_tmp7ED=*_tmp7EC;_tmp7EE=_tmp7ED.f1;_tmp7EF=_tmp7ED.f2;_tmp7F0=_tmp7ED.f3;{
struct Cyc_Toc_Env*_tmp7F1=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7F0->body;struct Cyc_Toc_Env _tmp7F3;struct
_RegionHandle*_tmp7F4;struct Cyc_Toc_Env*_tmp7F2=_tmp7E3;_tmp7F3=*_tmp7F2;_tmp7F4=
_tmp7F3.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7F6;struct _dyneither_ptr*_tmp7F7;
struct Cyc_Absyn_Switch_clause*_tmp7F8;struct _tuple24*_tmp7F5=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7F6=*_tmp7F5;_tmp7F7=_tmp7F6.f2;
_tmp7F8=_tmp7F6.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7F4,
_tmp7F1,end_l,_tmp7F7,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F8->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7F4,
_tmp7F1,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7EE,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7EF,s,0),0);{struct Cyc_List_List*_tmpD3B;stmts=((_tmpD3B=_region_malloc(
_tmp7E2,sizeof(*_tmpD3B)),((_tmpD3B->hd=s,((_tmpD3B->tl=stmts,_tmpD3B))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7FB;struct _tuple0*_tmp7FC;void*_tmp7FD;struct _tuple22*_tmp7FA=(struct
_tuple22*)decls->hd;_tmp7FB=*_tmp7FA;_tmp7FC=_tmp7FB.f1;_tmp7FD=_tmp7FB.f2;res=
Cyc_Absyn_declare_stmt(_tmp7FC,_tmp7FD,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD3C;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((
_tmpD3C->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD3C->tl=0,_tmpD3C)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7FF=s->r;struct Cyc_Absyn_Exp*_tmp802;struct
Cyc_Absyn_Stmt*_tmp804;struct Cyc_Absyn_Stmt*_tmp805;struct Cyc_Absyn_Exp*_tmp807;
struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Stmt*_tmp80A;struct Cyc_Absyn_Stmt*
_tmp80B;struct _tuple8 _tmp80D;struct Cyc_Absyn_Exp*_tmp80E;struct Cyc_Absyn_Stmt*
_tmp80F;struct Cyc_Absyn_Stmt*_tmp811;struct Cyc_Absyn_Stmt*_tmp813;struct Cyc_Absyn_Stmt*
_tmp815;struct Cyc_Absyn_Exp*_tmp817;struct _tuple8 _tmp818;struct Cyc_Absyn_Exp*
_tmp819;struct _tuple8 _tmp81A;struct Cyc_Absyn_Exp*_tmp81B;struct Cyc_Absyn_Stmt*
_tmp81C;struct Cyc_Absyn_Exp*_tmp81E;struct Cyc_List_List*_tmp81F;struct Cyc_List_List*
_tmp821;struct Cyc_Absyn_Switch_clause**_tmp822;struct Cyc_Absyn_Decl*_tmp824;
struct Cyc_Absyn_Stmt*_tmp825;struct _dyneither_ptr*_tmp827;struct Cyc_Absyn_Stmt*
_tmp828;struct Cyc_Absyn_Stmt*_tmp82A;struct _tuple8 _tmp82B;struct Cyc_Absyn_Exp*
_tmp82C;struct Cyc_Absyn_Stmt*_tmp82E;struct Cyc_List_List*_tmp82F;struct Cyc_Absyn_Exp*
_tmp831;_LL36F: {struct Cyc_Absyn_Skip_s_struct*_tmp800=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7FF;if(_tmp800->tag != 0)goto _LL371;}_LL370: return;_LL371: {struct Cyc_Absyn_Exp_s_struct*
_tmp801=(struct Cyc_Absyn_Exp_s_struct*)_tmp7FF;if(_tmp801->tag != 1)goto _LL373;
else{_tmp802=_tmp801->f1;}}_LL372: Cyc_Toc_exp_to_c(nv,_tmp802);return;_LL373: {
struct Cyc_Absyn_Seq_s_struct*_tmp803=(struct Cyc_Absyn_Seq_s_struct*)_tmp7FF;if(
_tmp803->tag != 2)goto _LL375;else{_tmp804=_tmp803->f1;_tmp805=_tmp803->f2;}}
_LL374: Cyc_Toc_stmt_to_c(nv,_tmp804);s=_tmp805;continue;_LL375: {struct Cyc_Absyn_Return_s_struct*
_tmp806=(struct Cyc_Absyn_Return_s_struct*)_tmp7FF;if(_tmp806->tag != 3)goto _LL377;
else{_tmp807=_tmp806->f1;}}_LL376: {struct Cyc_Core_Opt*topt=0;if(_tmp807 != 0){{
struct Cyc_Core_Opt*_tmpD3D;topt=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp807->topt))->v),
_tmpD3D))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp807);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp833=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp834=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp833,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp833,(void*)topt->v,_tmp807,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp834,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL377: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp808=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7FF;if(_tmp808->tag != 4)goto _LL379;
else{_tmp809=_tmp808->f1;_tmp80A=_tmp808->f2;_tmp80B=_tmp808->f3;}}_LL378: Cyc_Toc_exp_to_c(
nv,_tmp809);Cyc_Toc_stmt_to_c(nv,_tmp80A);s=_tmp80B;continue;_LL379: {struct Cyc_Absyn_While_s_struct*
_tmp80C=(struct Cyc_Absyn_While_s_struct*)_tmp7FF;if(_tmp80C->tag != 5)goto _LL37B;
else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80D.f1;_tmp80F=_tmp80C->f2;}}_LL37A: Cyc_Toc_exp_to_c(
nv,_tmp80E);{struct Cyc_Toc_Env _tmp836;struct _RegionHandle*_tmp837;struct Cyc_Toc_Env*
_tmp835=nv;_tmp836=*_tmp835;_tmp837=_tmp836.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp837,nv),_tmp80F);return;};_LL37B: {struct Cyc_Absyn_Break_s_struct*_tmp810=(
struct Cyc_Absyn_Break_s_struct*)_tmp7FF;if(_tmp810->tag != 6)goto _LL37D;else{
_tmp811=_tmp810->f1;}}_LL37C: {struct Cyc_Toc_Env _tmp839;struct _dyneither_ptr**
_tmp83A;struct Cyc_Toc_Env*_tmp838=nv;_tmp839=*_tmp838;_tmp83A=_tmp839.break_lab;
if(_tmp83A != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp83A,0);{int dest_depth=_tmp811 == 0?0:
_tmp811->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL37D: {struct Cyc_Absyn_Continue_s_struct*_tmp812=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7FF;if(_tmp812->tag != 7)goto _LL37F;else{_tmp813=_tmp812->f1;}}_LL37E: {
struct Cyc_Toc_Env _tmp83C;struct _dyneither_ptr**_tmp83D;struct Cyc_Toc_Env*_tmp83B=
nv;_tmp83C=*_tmp83B;_tmp83D=_tmp83C.continue_lab;if(_tmp83D != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp83D,0);_tmp815=_tmp813;goto _LL380;}_LL37F: {struct Cyc_Absyn_Goto_s_struct*
_tmp814=(struct Cyc_Absyn_Goto_s_struct*)_tmp7FF;if(_tmp814->tag != 8)goto _LL381;
else{_tmp815=_tmp814->f2;}}_LL380: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp815))->try_depth,s);return;_LL381: {struct Cyc_Absyn_For_s_struct*
_tmp816=(struct Cyc_Absyn_For_s_struct*)_tmp7FF;if(_tmp816->tag != 9)goto _LL383;
else{_tmp817=_tmp816->f1;_tmp818=_tmp816->f2;_tmp819=_tmp818.f1;_tmp81A=_tmp816->f3;
_tmp81B=_tmp81A.f1;_tmp81C=_tmp816->f4;}}_LL382: Cyc_Toc_exp_to_c(nv,_tmp817);Cyc_Toc_exp_to_c(
nv,_tmp819);Cyc_Toc_exp_to_c(nv,_tmp81B);{struct Cyc_Toc_Env _tmp83F;struct
_RegionHandle*_tmp840;struct Cyc_Toc_Env*_tmp83E=nv;_tmp83F=*_tmp83E;_tmp840=
_tmp83F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp840,nv),_tmp81C);return;};
_LL383: {struct Cyc_Absyn_Switch_s_struct*_tmp81D=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7FF;if(_tmp81D->tag != 10)goto _LL385;else{_tmp81E=_tmp81D->f1;_tmp81F=_tmp81D->f2;}}
_LL384: Cyc_Toc_xlate_switch(nv,s,_tmp81E,_tmp81F);return;_LL385: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp820=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7FF;if(_tmp820->tag != 11)goto
_LL387;else{_tmp821=_tmp820->f1;_tmp822=_tmp820->f2;}}_LL386: {struct Cyc_Toc_Env
_tmp842;struct Cyc_Toc_FallthruInfo*_tmp843;struct Cyc_Toc_Env*_tmp841=nv;_tmp842=*
_tmp841;_tmp843=_tmp842.fallthru_info;if(_tmp843 == 0){const char*_tmpD40;void*
_tmpD3F;(_tmpD3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD40="fallthru in unexpected place",_tag_dyneither(_tmpD40,sizeof(char),29))),
_tag_dyneither(_tmpD3F,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp847;struct
Cyc_List_List*_tmp848;struct Cyc_Dict_Dict _tmp849;struct Cyc_Toc_FallthruInfo
_tmp846=*_tmp843;_tmp847=_tmp846.label;_tmp848=_tmp846.binders;_tmp849=_tmp846.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp847,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp822)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp84A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp848);struct Cyc_List_List*_tmp84B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp821);for(0;_tmp84A != 0;(_tmp84A=_tmp84A->tl,
_tmp84B=_tmp84B->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp84B))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp849,(
struct _tuple0*)_tmp84A->hd),(struct Cyc_Absyn_Exp*)_tmp84B->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL387: {struct Cyc_Absyn_Decl_s_struct*_tmp823=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7FF;if(_tmp823->tag != 12)goto _LL389;else{_tmp824=_tmp823->f1;_tmp825=_tmp823->f2;}}
_LL388:{void*_tmp84C=_tmp824->r;struct Cyc_Absyn_Vardecl*_tmp84E;struct Cyc_Absyn_Pat*
_tmp850;struct Cyc_Absyn_Exp*_tmp851;struct Cyc_List_List*_tmp853;struct Cyc_Absyn_Fndecl*
_tmp855;struct Cyc_Absyn_Tvar*_tmp857;struct Cyc_Absyn_Vardecl*_tmp858;int _tmp859;
struct Cyc_Absyn_Exp*_tmp85A;struct Cyc_Absyn_Exp*_tmp85C;struct Cyc_Absyn_Tvar*
_tmp85D;struct Cyc_Absyn_Vardecl*_tmp85E;_LL392: {struct Cyc_Absyn_Var_d_struct*
_tmp84D=(struct Cyc_Absyn_Var_d_struct*)_tmp84C;if(_tmp84D->tag != 0)goto _LL394;
else{_tmp84E=_tmp84D->f1;}}_LL393: {struct Cyc_Toc_Env _tmp860;struct _RegionHandle*
_tmp861;struct Cyc_Toc_Env*_tmp85F=nv;_tmp860=*_tmp85F;_tmp861=_tmp860.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD43;struct Cyc_Absyn_Local_b_struct*_tmpD42;
struct Cyc_Toc_Env*_tmp862=Cyc_Toc_add_varmap(_tmp861,nv,_tmp84E->name,Cyc_Absyn_varb_exp(
_tmp84E->name,(void*)((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((
_tmpD43.tag=4,((_tmpD43.f1=_tmp84E,_tmpD43)))),_tmpD42)))),0));Cyc_Toc_local_decl_to_c(
_tmp862,_tmp862,_tmp84E,_tmp825);}goto _LL391;}_LL394: {struct Cyc_Absyn_Let_d_struct*
_tmp84F=(struct Cyc_Absyn_Let_d_struct*)_tmp84C;if(_tmp84F->tag != 2)goto _LL396;
else{_tmp850=_tmp84F->f1;_tmp851=_tmp84F->f3;}}_LL395:{void*_tmp865=_tmp850->r;
struct Cyc_Absyn_Vardecl*_tmp867;struct Cyc_Absyn_Pat*_tmp868;struct Cyc_Absyn_Pat
_tmp869;void*_tmp86A;_LL3A1: {struct Cyc_Absyn_Var_p_struct*_tmp866=(struct Cyc_Absyn_Var_p_struct*)
_tmp865;if(_tmp866->tag != 1)goto _LL3A3;else{_tmp867=_tmp866->f1;_tmp868=_tmp866->f2;
_tmp869=*_tmp868;_tmp86A=_tmp869.r;{struct Cyc_Absyn_Wild_p_struct*_tmp86B=(
struct Cyc_Absyn_Wild_p_struct*)_tmp86A;if(_tmp86B->tag != 0)goto _LL3A3;};}}_LL3A2: {
struct _tuple0*old_name=_tmp867->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp867->name=new_name;_tmp867->initializer=(struct Cyc_Absyn_Exp*)_tmp851;{
struct Cyc_Absyn_Var_d_struct _tmpD46;struct Cyc_Absyn_Var_d_struct*_tmpD45;_tmp824->r=(
void*)((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45[0]=((_tmpD46.tag=0,((
_tmpD46.f1=_tmp867,_tmpD46)))),_tmpD45))));}{struct Cyc_Toc_Env _tmp86F;struct
_RegionHandle*_tmp870;struct Cyc_Toc_Env*_tmp86E=nv;_tmp86F=*_tmp86E;_tmp870=
_tmp86F.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD49;struct Cyc_Absyn_Local_b_struct*
_tmpD48;struct Cyc_Toc_Env*_tmp871=Cyc_Toc_add_varmap(_tmp870,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD49.tag=
4,((_tmpD49.f1=_tmp867,_tmpD49)))),_tmpD48)))),0));Cyc_Toc_local_decl_to_c(
_tmp871,nv,_tmp867,_tmp825);}goto _LL3A0;};}_LL3A3:;_LL3A4: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp850,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp851->topt))->v,_tmp851,
_tmp825))->r;goto _LL3A0;_LL3A0:;}goto _LL391;_LL396: {struct Cyc_Absyn_Letv_d_struct*
_tmp852=(struct Cyc_Absyn_Letv_d_struct*)_tmp84C;if(_tmp852->tag != 3)goto _LL398;
else{_tmp853=_tmp852->f1;}}_LL397: {struct Cyc_List_List*_tmp874=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp853);if(_tmp874 == 0){const char*_tmpD4C;
void*_tmpD4B;(_tmpD4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4C="empty Letv_d",_tag_dyneither(_tmpD4C,sizeof(char),
13))),_tag_dyneither(_tmpD4B,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD4F;struct Cyc_Absyn_Var_d_struct*_tmpD4E;_tmp824->r=(void*)((_tmpD4E=
_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=0,((_tmpD4F.f1=(struct Cyc_Absyn_Vardecl*)
_tmp874->hd,_tmpD4F)))),_tmpD4E))));}_tmp874=_tmp874->tl;for(0;_tmp874 != 0;
_tmp874=_tmp874->tl){struct Cyc_Absyn_Var_d_struct _tmpD52;struct Cyc_Absyn_Var_d_struct*
_tmpD51;struct Cyc_Absyn_Decl*_tmp879=Cyc_Absyn_new_decl((void*)((_tmpD51=
_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=0,((_tmpD52.f1=(struct Cyc_Absyn_Vardecl*)
_tmp874->hd,_tmpD52)))),_tmpD51)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp879,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL391;}_LL398: {struct Cyc_Absyn_Fn_d_struct*
_tmp854=(struct Cyc_Absyn_Fn_d_struct*)_tmp84C;if(_tmp854->tag != 1)goto _LL39A;
else{_tmp855=_tmp854->f1;}}_LL399: {struct _tuple0*_tmp87C=_tmp855->name;struct
Cyc_Toc_Env _tmp87E;struct _RegionHandle*_tmp87F;struct Cyc_Toc_Env*_tmp87D=nv;
_tmp87E=*_tmp87D;_tmp87F=_tmp87E.rgn;{struct Cyc_Toc_Env*_tmp880=Cyc_Toc_add_varmap(
_tmp87F,nv,_tmp855->name,Cyc_Absyn_var_exp(_tmp87C,0));Cyc_Toc_fndecl_to_c(
_tmp880,_tmp855,0);Cyc_Toc_stmt_to_c(_tmp880,_tmp825);}goto _LL391;}_LL39A: {
struct Cyc_Absyn_Region_d_struct*_tmp856=(struct Cyc_Absyn_Region_d_struct*)
_tmp84C;if(_tmp856->tag != 4)goto _LL39C;else{_tmp857=_tmp856->f1;_tmp858=_tmp856->f2;
_tmp859=_tmp856->f3;_tmp85A=_tmp856->f4;}}_LL39B: {struct Cyc_Absyn_Stmt*_tmp881=
_tmp825;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp858->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp883;struct _RegionHandle*_tmp884;struct Cyc_Toc_Env*
_tmp882=nv;_tmp883=*_tmp882;_tmp884=_tmp883.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp884,nv,x_var,x_exp),_tmp881);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp881,0))->r;
else{if(_tmp85A == 0){struct Cyc_Absyn_Exp*_tmpD55[1];struct Cyc_Absyn_Exp*_tmpD54[
1];struct Cyc_List_List*_tmpD53;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD53=
_cycalloc(sizeof(*_tmpD53)),((_tmpD53->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD53->tl=0,_tmpD53)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD54[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD54,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp881,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD55[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD55,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp85A);{struct Cyc_Absyn_Exp*
_tmpD57[1];struct Cyc_Absyn_Exp*_tmpD56[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD56[1]=(struct Cyc_Absyn_Exp*)_tmp85A,((_tmpD56[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD56,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp881,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD57[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD57,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL39C: {struct Cyc_Absyn_Alias_d_struct*_tmp85B=(struct Cyc_Absyn_Alias_d_struct*)
_tmp84C;if(_tmp85B->tag != 5)goto _LL39E;else{_tmp85C=_tmp85B->f1;_tmp85D=_tmp85B->f2;
_tmp85E=_tmp85B->f3;}}_LL39D: {struct _tuple0*old_name=_tmp85E->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp85E->name=new_name;_tmp85E->initializer=(
struct Cyc_Absyn_Exp*)_tmp85C;{struct Cyc_Absyn_Decl_s_struct _tmpD66;struct Cyc_Absyn_Var_d_struct*
_tmpD65;struct Cyc_Absyn_Var_d_struct _tmpD64;struct Cyc_Absyn_Decl*_tmpD63;struct
Cyc_Absyn_Decl_s_struct*_tmpD62;s->r=(void*)((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((
_tmpD62[0]=((_tmpD66.tag=12,((_tmpD66.f1=((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((
_tmpD63->r=(void*)((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD64.tag=
0,((_tmpD64.f1=_tmp85E,_tmpD64)))),_tmpD65)))),((_tmpD63->loc=0,_tmpD63)))))),((
_tmpD66.f2=_tmp825,_tmpD66)))))),_tmpD62))));}{struct Cyc_Toc_Env _tmp890;struct
_RegionHandle*_tmp891;struct Cyc_Toc_Env*_tmp88F=nv;_tmp890=*_tmp88F;_tmp891=
_tmp890.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD69;struct Cyc_Absyn_Local_b_struct*
_tmpD68;struct Cyc_Toc_Env*_tmp892=Cyc_Toc_add_varmap(_tmp891,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD69.tag=
4,((_tmpD69.f1=_tmp85E,_tmpD69)))),_tmpD68)))),0));Cyc_Toc_local_decl_to_c(
_tmp892,nv,_tmp85E,_tmp825);}return;};}_LL39E:;_LL39F: {const char*_tmpD6C;void*
_tmpD6B;(_tmpD6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6C="bad nested declaration within function",_tag_dyneither(_tmpD6C,sizeof(
char),39))),_tag_dyneither(_tmpD6B,sizeof(void*),0)));}_LL391:;}return;_LL389: {
struct Cyc_Absyn_Label_s_struct*_tmp826=(struct Cyc_Absyn_Label_s_struct*)_tmp7FF;
if(_tmp826->tag != 13)goto _LL38B;else{_tmp827=_tmp826->f1;_tmp828=_tmp826->f2;}}
_LL38A: s=_tmp828;continue;_LL38B: {struct Cyc_Absyn_Do_s_struct*_tmp829=(struct
Cyc_Absyn_Do_s_struct*)_tmp7FF;if(_tmp829->tag != 14)goto _LL38D;else{_tmp82A=
_tmp829->f1;_tmp82B=_tmp829->f2;_tmp82C=_tmp82B.f1;}}_LL38C: {struct Cyc_Toc_Env
_tmp898;struct _RegionHandle*_tmp899;struct Cyc_Toc_Env*_tmp897=nv;_tmp898=*
_tmp897;_tmp899=_tmp898.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp899,nv),
_tmp82A);Cyc_Toc_exp_to_c(nv,_tmp82C);return;}_LL38D: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp82D=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7FF;if(_tmp82D->tag != 15)goto
_LL38F;else{_tmp82E=_tmp82D->f1;_tmp82F=_tmp82D->f2;}}_LL38E: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD6D;
e_exp->topt=((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->v=e_typ,_tmpD6D))));}{
struct Cyc_Toc_Env _tmp89C;struct _RegionHandle*_tmp89D;struct Cyc_Toc_Env*_tmp89B=
nv;_tmp89C=*_tmp89B;_tmp89D=_tmp89C.rgn;{struct Cyc_Toc_Env*_tmp89E=Cyc_Toc_add_varmap(
_tmp89D,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp89E,_tmp82E);{struct Cyc_Absyn_Stmt*
_tmp89F=Cyc_Absyn_seq_stmt(_tmp82E,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8A0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8A1=
Cyc_Absyn_var_exp(_tmp8A0,0);struct Cyc_Absyn_Vardecl*_tmp8A2=Cyc_Absyn_new_vardecl(
_tmp8A0,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD6E;_tmp8A1->topt=((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->v=Cyc_Absyn_exn_typ(),_tmpD6E))));}{
struct Cyc_Core_Opt*_tmpD7F;struct Cyc_Absyn_Var_p_struct*_tmpD7E;struct Cyc_Absyn_Pat*
_tmpD7D;struct Cyc_Core_Opt*_tmpD7C;struct Cyc_Absyn_Var_p_struct _tmpD7B;struct Cyc_Absyn_Pat*
_tmpD7A;struct Cyc_Absyn_Pat*_tmp8A4=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->r=(void*)((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7B.tag=
1,((_tmpD7B.f1=_tmp8A2,((_tmpD7B.f2=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((
_tmpD7D->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD7D->topt=((_tmpD7C=_cycalloc(
sizeof(*_tmpD7C)),((_tmpD7C->v=Cyc_Absyn_exn_typ(),_tmpD7C)))),((_tmpD7D->loc=0,
_tmpD7D)))))))),_tmpD7B)))))),_tmpD7E)))),((_tmpD7A->topt=((_tmpD7F=_cycalloc(
sizeof(*_tmpD7F)),((_tmpD7F->v=Cyc_Absyn_exn_typ(),_tmpD7F)))),((_tmpD7A->loc=0,
_tmpD7A)))))));struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Absyn_throw_exp(_tmp8A1,0);{
struct Cyc_Core_Opt*_tmpD80;_tmp8A5->topt=((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((
_tmpD80->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD80))));}{struct Cyc_Absyn_Stmt*
_tmp8A7=Cyc_Absyn_exp_stmt(_tmp8A5,0);struct Cyc_Core_Opt*_tmpD86;struct Cyc_List_List*
_tmpD85;struct Cyc_Absyn_Switch_clause*_tmpD84;struct Cyc_Absyn_Switch_clause*
_tmp8A8=(_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->pattern=_tmp8A4,((
_tmpD84->pat_vars=((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86->v=((_tmpD85=
_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=_tmp8A2,((_tmpD85->tl=0,_tmpD85)))))),
_tmpD86)))),((_tmpD84->where_clause=0,((_tmpD84->body=_tmp8A7,((_tmpD84->loc=0,
_tmpD84)))))))))));struct Cyc_List_List*_tmpD87;struct Cyc_Absyn_Stmt*_tmp8A9=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp82F,((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=_tmp8A8,((_tmpD87->tl=
0,_tmpD87))))))),0);Cyc_Toc_stmt_to_c(_tmp89E,_tmp8A9);{struct Cyc_List_List*
_tmpD88;struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD88->tl=0,_tmpD88)))))),0);struct Cyc_List_List*
_tmpD89;struct Cyc_Absyn_Stmt*_tmp8AB=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD89->tl=0,_tmpD89)))))),0),0);struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8AB,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8AC,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8AA,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8AD,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp89F,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8A9,0),0),0),0),0),0))->r;};};};};}
return;};}_LL38F: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp830=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7FF;if(_tmp830->tag != 16)goto _LL36E;else{_tmp831=_tmp830->f1;}}_LL390: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp831);{struct Cyc_List_List*
_tmpD8A;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=_tmp831,((_tmpD8A->tl=0,
_tmpD8A)))))),0));};}return;_LL36E:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8BB=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8BB;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8BC=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8BD=f->args;for(0;_tmp8BD != 0;_tmp8BD=_tmp8BD->tl){struct
_tuple0*_tmpD8B;struct _tuple0*_tmp8BE=(_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((
_tmpD8B->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8B->f2=(*((struct
_tuple25*)_tmp8BD->hd)).f1,_tmpD8B)))));(*((struct _tuple25*)_tmp8BD->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8BD->hd)).f3);_tmp8BC=Cyc_Toc_add_varmap(frgn,_tmp8BC,
_tmp8BE,Cyc_Absyn_var_exp(_tmp8BE,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8C1;struct Cyc_Absyn_Tqual
_tmp8C2;void*_tmp8C3;int _tmp8C4;struct Cyc_Absyn_VarargInfo _tmp8C0=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8C1=_tmp8C0.name;_tmp8C2=_tmp8C0.tq;_tmp8C3=
_tmp8C0.type;_tmp8C4=_tmp8C0.inject;{void*_tmp8C5=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8C3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C2,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD8C;struct _tuple0*_tmp8C6=(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((
_tmpD8C->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD8C->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8C1))->v,_tmpD8C)))));{
struct _tuple25*_tmpD8F;struct Cyc_List_List*_tmpD8E;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD8E=
_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->hd=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((
_tmpD8F->f1=(struct _dyneither_ptr*)_tmp8C1->v,((_tmpD8F->f2=_tmp8C2,((_tmpD8F->f3=
_tmp8C5,_tmpD8F)))))))),((_tmpD8E->tl=0,_tmpD8E)))))));}_tmp8BC=Cyc_Toc_add_varmap(
frgn,_tmp8BC,_tmp8C6,Cyc_Absyn_var_exp(_tmp8C6,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8CA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8CA != 0;_tmp8CA=_tmp8CA->tl){((struct Cyc_Absyn_Vardecl*)_tmp8CA->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8CA->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8BC),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL3A5: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A6: return Cyc_Absyn_Extern;
default: _LL3A7: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8CB=ad->name;struct _DynRegionHandle*_tmp8CD;struct Cyc_Dict_Dict*_tmp8CE;
struct Cyc_Toc_TocState _tmp8CC=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8CD=_tmp8CC.dyn;_tmp8CE=_tmp8CC.aggrs_so_far;{struct _DynRegionFrame _tmp8CF;
struct _RegionHandle*d=_open_dynregion(& _tmp8CF,_tmp8CD);{int seen_defn_before;
struct _tuple14**_tmp8D0=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8CE,_tmp8CB);if(_tmp8D0 == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD90;v=((
_tmpD90=_region_malloc(d,sizeof(*_tmpD90)),((_tmpD90->f1=ad,((_tmpD90->f2=Cyc_Absyn_strctq(
_tmp8CB),_tmpD90))))));}else{struct _tuple14*_tmpD91;v=((_tmpD91=_region_malloc(d,
sizeof(*_tmpD91)),((_tmpD91->f1=ad,((_tmpD91->f2=Cyc_Absyn_unionq_typ(_tmp8CB),
_tmpD91))))));}*_tmp8CE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CE,_tmp8CB,v);};}else{struct
_tuple14 _tmp8D4;struct Cyc_Absyn_Aggrdecl*_tmp8D5;void*_tmp8D6;struct _tuple14*
_tmp8D3=*_tmp8D0;_tmp8D4=*_tmp8D3;_tmp8D5=_tmp8D4.f1;_tmp8D6=_tmp8D4.f2;if(
_tmp8D5->impl == 0){{struct _tuple14*_tmpD92;*_tmp8CE=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CE,
_tmp8CB,((_tmpD92=_region_malloc(d,sizeof(*_tmpD92)),((_tmpD92->f1=ad,((_tmpD92->f2=
_tmp8D6,_tmpD92)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD93;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD93=_cycalloc(sizeof(*_tmpD93)),((
_tmpD93->kind=ad->kind,((_tmpD93->sc=Cyc_Absyn_Public,((_tmpD93->name=ad->name,((
_tmpD93->tvs=0,((_tmpD93->impl=0,((_tmpD93->attributes=ad->attributes,_tmpD93)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD94;
new_ad->impl=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->exist_vars=0,((
_tmpD94->rgn_po=0,((_tmpD94->fields=0,((_tmpD94->tagged=0,_tmpD94))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8D9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8D9 != 0;_tmp8D9=_tmp8D9->tl){struct Cyc_Absyn_Aggrfield*
_tmp8DA=(struct Cyc_Absyn_Aggrfield*)_tmp8D9->hd;struct Cyc_Absyn_Aggrfield*
_tmpD95;struct Cyc_Absyn_Aggrfield*_tmp8DB=(_tmpD95=_cycalloc(sizeof(*_tmpD95)),((
_tmpD95->name=_tmp8DA->name,((_tmpD95->tq=Cyc_Toc_mt_tq,((_tmpD95->type=Cyc_Toc_typ_to_c_array(
_tmp8DA->type),((_tmpD95->width=_tmp8DA->width,((_tmpD95->attributes=_tmp8DA->attributes,
_tmpD95)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8DC=_tmp8DB->type;struct _dyneither_ptr*_tmp8DD=_tmp8DB->name;const char*
_tmpD9A;void*_tmpD99[2];struct Cyc_String_pa_struct _tmpD98;struct Cyc_String_pa_struct
_tmpD97;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD97.tag=0,((_tmpD97.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DD),((_tmpD98.tag=0,((_tmpD98.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD99[0]=&
_tmpD98,((_tmpD99[1]=& _tmpD97,Cyc_aprintf(((_tmpD9A="_union_%s_%s",
_tag_dyneither(_tmpD9A,sizeof(char),13))),_tag_dyneither(_tmpD99,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD9B;struct _dyneither_ptr*str=(_tmpD9B=_cycalloc(sizeof(*
_tmpD9B)),((_tmpD9B[0]=s,_tmpD9B)));struct Cyc_Absyn_Aggrfield*_tmpD9C;struct Cyc_Absyn_Aggrfield*
_tmp8DE=(_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->name=Cyc_Toc_val_sp,((
_tmpD9C->tq=Cyc_Toc_mt_tq,((_tmpD9C->type=_tmp8DC,((_tmpD9C->width=0,((_tmpD9C->attributes=
0,_tmpD9C)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9D;struct Cyc_Absyn_Aggrfield*
_tmp8DF=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->name=Cyc_Toc_tag_sp,((
_tmpD9D->tq=Cyc_Toc_mt_tq,((_tmpD9D->type=Cyc_Absyn_sint_typ,((_tmpD9D->width=0,((
_tmpD9D->attributes=0,_tmpD9D)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD9E[2];
struct Cyc_List_List*_tmp8E0=(_tmpD9E[1]=_tmp8DE,((_tmpD9E[0]=_tmp8DF,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD9E,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpDA3;struct _tuple0*_tmpDA2;struct Cyc_Absyn_Aggrdecl*
_tmpDA1;struct Cyc_Absyn_Aggrdecl*_tmp8E1=(_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((
_tmpDA1->kind=Cyc_Absyn_StructA,((_tmpDA1->sc=Cyc_Absyn_Public,((_tmpDA1->name=((
_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2->f1=Cyc_Absyn_Loc_n,((_tmpDA2->f2=
str,_tmpDA2)))))),((_tmpDA1->tvs=0,((_tmpDA1->impl=((_tmpDA3=_cycalloc(sizeof(*
_tmpDA3)),((_tmpDA3->exist_vars=0,((_tmpDA3->rgn_po=0,((_tmpDA3->fields=_tmp8E0,((
_tmpDA3->tagged=0,_tmpDA3)))))))))),((_tmpDA1->attributes=0,_tmpDA1)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpDA9;struct Cyc_Absyn_Aggr_d_struct _tmpDA8;
struct Cyc_List_List*_tmpDA7;Cyc_Toc_result_decls=((_tmpDA7=_cycalloc(sizeof(*
_tmpDA7)),((_tmpDA7->hd=Cyc_Absyn_new_decl((void*)((_tmpDA9=_cycalloc(sizeof(*
_tmpDA9)),((_tmpDA9[0]=((_tmpDA8.tag=6,((_tmpDA8.f1=_tmp8E1,_tmpDA8)))),_tmpDA9)))),
0),((_tmpDA7->tl=Cyc_Toc_result_decls,_tmpDA7))))));}_tmp8DB->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpDAA;new_fields=((_tmpDAA=_cycalloc(sizeof(*
_tmpDAA)),((_tmpDAA->hd=_tmp8DB,((_tmpDAA->tl=new_fields,_tmpDAA))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDB4;struct Cyc_Absyn_Aggr_d_struct
_tmpDB3;struct Cyc_Absyn_Aggr_d_struct*_tmpDB2;struct Cyc_List_List*_tmpDB1;Cyc_Toc_result_decls=((
_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->hd=((_tmpDB4=_cycalloc(sizeof(*
_tmpDB4)),((_tmpDB4->r=(void*)((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=((
_tmpDB3.tag=6,((_tmpDB3.f1=new_ad,_tmpDB3)))),_tmpDB2)))),((_tmpDB4->loc=0,
_tmpDB4)))))),((_tmpDB1->tl=Cyc_Toc_result_decls,_tmpDB1))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8F8;struct Cyc_Set_Set**_tmp8F9;struct Cyc_Toc_TocState
_tmp8F7=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F8=
_tmp8F7.dyn;_tmp8F9=_tmp8F7.datatypes_so_far;{struct _DynRegionFrame _tmp8FA;
struct _RegionHandle*d=_open_dynregion(& _tmp8FA,_tmp8F8);{struct _tuple0*_tmp8FB=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8F9,_tmp8FB)){_npop_handler(0);return;}*_tmp8F9=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8F9,_tmp8FB);}{struct Cyc_List_List*_tmp8FC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8FC != 0;_tmp8FC=_tmp8FC->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FC->hd;struct Cyc_List_List*_tmp8FD=0;int i=
1;{struct Cyc_List_List*_tmp8FE=f->typs;for(0;_tmp8FE != 0;(_tmp8FE=_tmp8FE->tl,i
++)){struct _dyneither_ptr*_tmp8FF=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDB5;struct Cyc_Absyn_Aggrfield*_tmp900=(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((
_tmpDB5->name=_tmp8FF,((_tmpDB5->tq=(*((struct _tuple9*)_tmp8FE->hd)).f1,((
_tmpDB5->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8FE->hd)).f2),((
_tmpDB5->width=0,((_tmpDB5->attributes=0,_tmpDB5)))))))))));struct Cyc_List_List*
_tmpDB6;_tmp8FD=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=_tmp900,((
_tmpDB6->tl=_tmp8FD,_tmpDB6))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDB9;struct Cyc_List_List*
_tmpDB8;_tmp8FD=((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=((_tmpDB9=
_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->name=Cyc_Toc_tag_sp,((_tmpDB9->tq=Cyc_Toc_mt_tq,((
_tmpDB9->type=Cyc_Absyn_sint_typ,((_tmpDB9->width=0,((_tmpDB9->attributes=0,
_tmpDB9)))))))))))),((_tmpDB8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8FD),_tmpDB8))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDBE;const char*_tmpDBD;struct Cyc_Absyn_Aggrdecl*_tmpDBC;struct Cyc_Absyn_Aggrdecl*
_tmp905=(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->kind=Cyc_Absyn_StructA,((
_tmpDBC->sc=Cyc_Absyn_Public,((_tmpDBC->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDBD="_struct",_tag_dyneither(_tmpDBD,sizeof(char),8)))),((_tmpDBC->tvs=0,((
_tmpDBC->impl=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->exist_vars=0,((
_tmpDBE->rgn_po=0,((_tmpDBE->fields=_tmp8FD,((_tmpDBE->tagged=0,_tmpDBE)))))))))),((
_tmpDBC->attributes=0,_tmpDBC)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDC4;
struct Cyc_Absyn_Aggr_d_struct _tmpDC3;struct Cyc_List_List*_tmpDC2;Cyc_Toc_result_decls=((
_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->hd=Cyc_Absyn_new_decl((void*)((
_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC3.tag=6,((_tmpDC3.f1=
_tmp905,_tmpDC3)))),_tmpDC4)))),0),((_tmpDC2->tl=Cyc_Toc_result_decls,_tmpDC2))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp90D;struct Cyc_Dict_Dict*_tmp90E;struct
Cyc_Toc_TocState _tmp90C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp90D=_tmp90C.dyn;_tmp90E=_tmp90C.xdatatypes_so_far;{struct _DynRegionFrame
_tmp90F;struct _RegionHandle*d=_open_dynregion(& _tmp90F,_tmp90D);{struct _tuple0*
_tmp910=xd->name;struct Cyc_List_List*_tmp911=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp911 != 0;_tmp911=_tmp911->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp911->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp912=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp913=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp912,Cyc_Absyn_false_conref,0);int*_tmp914=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp90E,f->name);int
_tmp915;_LL3AA: if(_tmp914 != 0)goto _LL3AC;_LL3AB: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp916=Cyc_Absyn_new_vardecl(f->name,_tmp913,
initopt);_tmp916->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDCA;struct Cyc_Absyn_Var_d_struct
_tmpDC9;struct Cyc_List_List*_tmpDC8;Cyc_Toc_result_decls=((_tmpDC8=_cycalloc(
sizeof(*_tmpDC8)),((_tmpDC8->hd=Cyc_Absyn_new_decl((void*)((_tmpDCA=_cycalloc(
sizeof(*_tmpDCA)),((_tmpDCA[0]=((_tmpDC9.tag=0,((_tmpDC9.f1=_tmp916,_tmpDC9)))),
_tmpDCA)))),0),((_tmpDC8->tl=Cyc_Toc_result_decls,_tmpDC8))))));}*_tmp90E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90E,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp91A=f->typs;for(0;_tmp91A != 0;(_tmp91A=_tmp91A->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDD2;void*_tmpDD1[1];const char*_tmpDD0;struct
_dyneither_ptr*_tmpDCF;struct _dyneither_ptr*_tmp91B=(_tmpDCF=_cycalloc(sizeof(*
_tmpDCF)),((_tmpDCF[0]=(struct _dyneither_ptr)((_tmpDD2.tag=1,((_tmpDD2.f1=(
unsigned long)i,((_tmpDD1[0]=& _tmpDD2,Cyc_aprintf(((_tmpDD0="f%d",_tag_dyneither(
_tmpDD0,sizeof(char),4))),_tag_dyneither(_tmpDD1,sizeof(void*),1)))))))),_tmpDCF)));
struct Cyc_Absyn_Aggrfield*_tmpDD3;struct Cyc_Absyn_Aggrfield*_tmp91C=(_tmpDD3=
_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->name=_tmp91B,((_tmpDD3->tq=(*((struct
_tuple9*)_tmp91A->hd)).f1,((_tmpDD3->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp91A->hd)).f2),((_tmpDD3->width=0,((_tmpDD3->attributes=0,_tmpDD3)))))))))));
struct Cyc_List_List*_tmpDD4;fields=((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((
_tmpDD4->hd=_tmp91C,((_tmpDD4->tl=fields,_tmpDD4))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDD7;struct Cyc_List_List*_tmpDD6;fields=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((
_tmpDD6->hd=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->name=Cyc_Toc_tag_sp,((
_tmpDD7->tq=Cyc_Toc_mt_tq,((_tmpDD7->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDD7->width=0,((_tmpDD7->attributes=0,_tmpDD7)))))))))))),((
_tmpDD6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDD6))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDDC;const char*_tmpDDB;
struct Cyc_Absyn_Aggrdecl*_tmpDDA;struct Cyc_Absyn_Aggrdecl*_tmp925=(_tmpDDA=
_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->kind=Cyc_Absyn_StructA,((_tmpDDA->sc=Cyc_Absyn_Public,((
_tmpDDA->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDDB="_struct",
_tag_dyneither(_tmpDDB,sizeof(char),8)))),((_tmpDDA->tvs=0,((_tmpDDA->impl=((
_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->exist_vars=0,((_tmpDDC->rgn_po=0,((
_tmpDDC->fields=fields,((_tmpDDC->tagged=0,_tmpDDC)))))))))),((_tmpDDA->attributes=
0,_tmpDDA)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDE2;struct Cyc_Absyn_Aggr_d_struct
_tmpDE1;struct Cyc_List_List*_tmpDE0;Cyc_Toc_result_decls=((_tmpDE0=_cycalloc(
sizeof(*_tmpDE0)),((_tmpDE0->hd=Cyc_Absyn_new_decl((void*)((_tmpDE2=_cycalloc(
sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE1.tag=6,((_tmpDE1.f1=_tmp925,_tmpDE1)))),
_tmpDE2)))),0),((_tmpDE0->tl=Cyc_Toc_result_decls,_tmpDE0))))));}goto _LL3A9;};};};}
_LL3AC: if(_tmp914 == 0)goto _LL3AE;_tmp915=*_tmp914;if(_tmp915 != 0)goto _LL3AE;
_LL3AD: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp92C=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp92D=Cyc_Absyn_new_vardecl(f->name,_tmp913,(
struct Cyc_Absyn_Exp*)_tmp92C);_tmp92D->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDE8;struct Cyc_Absyn_Var_d_struct _tmpDE7;struct Cyc_List_List*_tmpDE6;Cyc_Toc_result_decls=((
_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=Cyc_Absyn_new_decl((void*)((
_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE7.tag=0,((_tmpDE7.f1=
_tmp92D,_tmpDE7)))),_tmpDE8)))),0),((_tmpDE6->tl=Cyc_Toc_result_decls,_tmpDE6))))));}*
_tmp90E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90E,f->name,1);}goto _LL3A9;_LL3AE:;_LL3AF: goto _LL3A9;_LL3A9:;}};
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
void*_tmp931=init->r;struct Cyc_Absyn_Vardecl*_tmp933;struct Cyc_Absyn_Exp*_tmp934;
struct Cyc_Absyn_Exp*_tmp935;int _tmp936;_LL3B1: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp932=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp931;if(_tmp932->tag != 28)
goto _LL3B3;else{_tmp933=_tmp932->f1;_tmp934=_tmp932->f2;_tmp935=_tmp932->f3;
_tmp936=_tmp932->f4;}}_LL3B2: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp933,_tmp934,_tmp935,_tmp936,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3B0;_LL3B3:;_LL3B4: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp938;struct _RegionHandle*_tmp939;struct Cyc_Toc_Env*_tmp937=init_nv;_tmp938=*
_tmp937;_tmp939=_tmp938.rgn;{struct Cyc_Toc_Env*_tmp93A=Cyc_Toc_set_toplevel(
_tmp939,init_nv);Cyc_Toc_exp_to_c(_tmp93A,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3B0;_LL3B0:;}else{void*_tmp93B=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp93D;void*_tmp93E;struct Cyc_Absyn_Exp*_tmp93F;union Cyc_Absyn_Constraint*
_tmp940;_LL3B6:{struct Cyc_Absyn_ArrayType_struct*_tmp93C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp93B;if(_tmp93C->tag != 9)goto _LL3B8;else{_tmp93D=_tmp93C->f1;_tmp93E=_tmp93D.elt_type;
_tmp93F=_tmp93D.num_elts;_tmp940=_tmp93D.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp940))goto _LL3B8;_LL3B7: if(_tmp93F == 0){const char*
_tmpDEB;void*_tmpDEA;(_tmpDEA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDEB="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDEB,sizeof(char),55))),_tag_dyneither(_tmpDEA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp93F;struct Cyc_Absyn_Exp*
_tmp943=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp944=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp943,
_tmp944,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3B5;};_LL3B8:;_LL3B9: goto _LL3B5;
_LL3B5:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp946;struct _RegionHandle*
_tmp947;struct Cyc_Toc_Env*_tmp945=nv;_tmp946=*_tmp945;_tmp947=_tmp946.rgn;{
struct Cyc_Absyn_Stmt*_tmp948=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp949=Cyc_Toc_share_env(_tmp947,nv);struct Cyc_Toc_Env*_tmp94B;struct Cyc_List_List*
_tmp94C;struct Cyc_Absyn_Stmt*_tmp94D;struct _tuple21 _tmp94A=Cyc_Toc_xlate_pat(
_tmp949,_tmp947,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp94B=_tmp94A.f1;_tmp94C=_tmp94A.f2;_tmp94D=_tmp94A.f3;Cyc_Toc_stmt_to_c(
_tmp94B,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp94D,s,0),0);for(0;_tmp94C != 0;_tmp94C=_tmp94C->tl){
struct _tuple22 _tmp94F;struct _tuple0*_tmp950;void*_tmp951;struct _tuple22*_tmp94E=(
struct _tuple22*)_tmp94C->hd;_tmp94F=*_tmp94E;_tmp950=_tmp94F.f1;_tmp951=_tmp94F.f2;
s=Cyc_Absyn_declare_stmt(_tmp950,_tmp951,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp952=e->r;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*_tmp956;struct
Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Exp*_tmp95C;
struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*
_tmp962;struct Cyc_List_List*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*
_tmp967;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*
_tmp96C;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*
_tmp970;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*
_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*
_tmp979;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_Absyn_Exp*_tmp97C;struct Cyc_List_List*
_tmp97D;struct Cyc_Absyn_Exp*_tmp97F;struct Cyc_List_List*_tmp980;void*_tmp982;
void**_tmp983;struct Cyc_Absyn_Exp*_tmp984;struct _tuple7*_tmp986;struct _tuple7
_tmp987;void*_tmp988;void**_tmp989;struct Cyc_List_List*_tmp98A;struct Cyc_List_List*
_tmp98C;struct Cyc_List_List*_tmp98E;void*_tmp990;void**_tmp991;void*_tmp993;void**
_tmp994;struct Cyc_Absyn_Stmt*_tmp996;struct Cyc_Absyn_MallocInfo _tmp998;struct Cyc_Absyn_MallocInfo*
_tmp999;_LL3BB: {struct Cyc_Absyn_Deref_e_struct*_tmp953=(struct Cyc_Absyn_Deref_e_struct*)
_tmp952;if(_tmp953->tag != 21)goto _LL3BD;else{_tmp954=_tmp953->f1;}}_LL3BC:
_tmp956=_tmp954;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp955=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp952;if(_tmp955->tag != 22)goto _LL3BF;
else{_tmp956=_tmp955->f1;}}_LL3BE: _tmp958=_tmp956;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp957=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp952;if(_tmp957->tag != 23)goto
_LL3C1;else{_tmp958=_tmp957->f1;}}_LL3C0: _tmp95A=_tmp958;goto _LL3C2;_LL3C1: {
struct Cyc_Absyn_Address_e_struct*_tmp959=(struct Cyc_Absyn_Address_e_struct*)
_tmp952;if(_tmp959->tag != 16)goto _LL3C3;else{_tmp95A=_tmp959->f1;}}_LL3C2:
_tmp95C=_tmp95A;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_struct*_tmp95B=(
struct Cyc_Absyn_Throw_e_struct*)_tmp952;if(_tmp95B->tag != 12)goto _LL3C5;else{
_tmp95C=_tmp95B->f1;}}_LL3C4: _tmp95E=_tmp95C;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp95D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp952;if(_tmp95D->tag != 13)
goto _LL3C7;else{_tmp95E=_tmp95D->f1;}}_LL3C6: _tmp960=_tmp95E;goto _LL3C8;_LL3C7: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp95F=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp952;if(_tmp95F->tag != 19)goto _LL3C9;else{_tmp960=_tmp95F->f1;}}_LL3C8:
_tmp962=_tmp960;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_struct*_tmp961=(
struct Cyc_Absyn_Increment_e_struct*)_tmp952;if(_tmp961->tag != 5)goto _LL3CB;else{
_tmp962=_tmp961->f1;}}_LL3CA: Cyc_Toc_exptypes_to_c(_tmp962);goto _LL3BA;_LL3CB: {
struct Cyc_Absyn_Primop_e_struct*_tmp963=(struct Cyc_Absyn_Primop_e_struct*)
_tmp952;if(_tmp963->tag != 3)goto _LL3CD;else{_tmp964=_tmp963->f2;}}_LL3CC:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp964);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_struct*_tmp965=(struct Cyc_Absyn_And_e_struct*)
_tmp952;if(_tmp965->tag != 7)goto _LL3CF;else{_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;}}
_LL3CE: _tmp969=_tmp966;_tmp96A=_tmp967;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_struct*
_tmp968=(struct Cyc_Absyn_Or_e_struct*)_tmp952;if(_tmp968->tag != 8)goto _LL3D1;
else{_tmp969=_tmp968->f1;_tmp96A=_tmp968->f2;}}_LL3D0: _tmp96C=_tmp969;_tmp96D=
_tmp96A;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp96B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp952;if(_tmp96B->tag != 9)goto _LL3D3;else{_tmp96C=_tmp96B->f1;_tmp96D=_tmp96B->f2;}}
_LL3D2: _tmp96F=_tmp96C;_tmp970=_tmp96D;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_struct*
_tmp96E=(struct Cyc_Absyn_Subscript_e_struct*)_tmp952;if(_tmp96E->tag != 24)goto
_LL3D5;else{_tmp96F=_tmp96E->f1;_tmp970=_tmp96E->f2;}}_LL3D4: _tmp972=_tmp96F;
_tmp973=_tmp970;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_struct*_tmp971=(
struct Cyc_Absyn_Swap_e_struct*)_tmp952;if(_tmp971->tag != 35)goto _LL3D7;else{
_tmp972=_tmp971->f1;_tmp973=_tmp971->f2;}}_LL3D6: _tmp975=_tmp972;_tmp976=_tmp973;
goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_struct*_tmp974=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp952;if(_tmp974->tag != 4)goto _LL3D9;else{_tmp975=_tmp974->f1;_tmp976=_tmp974->f3;}}
_LL3D8: Cyc_Toc_exptypes_to_c(_tmp975);Cyc_Toc_exptypes_to_c(_tmp976);goto _LL3BA;
_LL3D9: {struct Cyc_Absyn_Conditional_e_struct*_tmp977=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp952;if(_tmp977->tag != 6)goto _LL3DB;else{_tmp978=_tmp977->f1;_tmp979=_tmp977->f2;
_tmp97A=_tmp977->f3;}}_LL3DA: Cyc_Toc_exptypes_to_c(_tmp978);Cyc_Toc_exptypes_to_c(
_tmp979);Cyc_Toc_exptypes_to_c(_tmp97A);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_struct*
_tmp97B=(struct Cyc_Absyn_FnCall_e_struct*)_tmp952;if(_tmp97B->tag != 11)goto
_LL3DD;else{_tmp97C=_tmp97B->f1;_tmp97D=_tmp97B->f2;}}_LL3DC: _tmp97F=_tmp97C;
_tmp980=_tmp97D;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp97E=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp952;if(_tmp97E->tag != 10)goto _LL3DF;
else{_tmp97F=_tmp97E->f1;_tmp980=_tmp97E->f2;}}_LL3DE: Cyc_Toc_exptypes_to_c(
_tmp97F);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp980);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_Cast_e_struct*
_tmp981=(struct Cyc_Absyn_Cast_e_struct*)_tmp952;if(_tmp981->tag != 15)goto _LL3E1;
else{_tmp982=(void**)& _tmp981->f1;_tmp983=(void**)((void**)& _tmp981->f1);_tmp984=
_tmp981->f2;}}_LL3E0:*_tmp983=Cyc_Toc_typ_to_c(*_tmp983);Cyc_Toc_exptypes_to_c(
_tmp984);goto _LL3BA;_LL3E1: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp985=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp952;if(_tmp985->tag != 26)goto _LL3E3;
else{_tmp986=_tmp985->f1;_tmp987=*_tmp986;_tmp988=_tmp987.f3;_tmp989=(void**)&(*
_tmp985->f1).f3;_tmp98A=_tmp985->f2;}}_LL3E2:*_tmp989=Cyc_Toc_typ_to_c(*_tmp989);
_tmp98C=_tmp98A;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp98B=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp952;if(_tmp98B->tag != 36)
goto _LL3E5;else{_tmp98C=_tmp98B->f2;}}_LL3E4: _tmp98E=_tmp98C;goto _LL3E6;_LL3E5: {
struct Cyc_Absyn_Array_e_struct*_tmp98D=(struct Cyc_Absyn_Array_e_struct*)_tmp952;
if(_tmp98D->tag != 27)goto _LL3E7;else{_tmp98E=_tmp98D->f1;}}_LL3E6: for(0;_tmp98E
!= 0;_tmp98E=_tmp98E->tl){struct Cyc_Absyn_Exp*_tmp9A9;struct _tuple15 _tmp9A8=*((
struct _tuple15*)_tmp98E->hd);_tmp9A9=_tmp9A8.f2;Cyc_Toc_exptypes_to_c(_tmp9A9);}
goto _LL3BA;_LL3E7: {struct Cyc_Absyn_Offsetof_e_struct*_tmp98F=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp952;if(_tmp98F->tag != 20)goto _LL3E9;else{_tmp990=(void**)& _tmp98F->f1;
_tmp991=(void**)((void**)& _tmp98F->f1);}}_LL3E8: _tmp994=_tmp991;goto _LL3EA;
_LL3E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp992=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp952;if(_tmp992->tag != 18)goto _LL3EB;else{_tmp993=(void**)& _tmp992->f1;
_tmp994=(void**)((void**)& _tmp992->f1);}}_LL3EA:*_tmp994=Cyc_Toc_typ_to_c(*
_tmp994);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_StmtExp_e_struct*_tmp995=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp952;if(_tmp995->tag != 37)goto _LL3ED;else{_tmp996=_tmp995->f1;}}_LL3EC: Cyc_Toc_stmttypes_to_c(
_tmp996);goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Malloc_e_struct*_tmp997=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp952;if(_tmp997->tag != 34)goto _LL3EF;else{_tmp998=_tmp997->f1;_tmp999=(struct
Cyc_Absyn_MallocInfo*)& _tmp997->f1;}}_LL3EE: if(_tmp999->elt_type != 0){void**
_tmpDEC;_tmp999->elt_type=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp999->elt_type))),_tmpDEC))));}Cyc_Toc_exptypes_to_c(
_tmp999->num_elts);goto _LL3BA;_LL3EF: {struct Cyc_Absyn_Const_e_struct*_tmp99A=(
struct Cyc_Absyn_Const_e_struct*)_tmp952;if(_tmp99A->tag != 0)goto _LL3F1;}_LL3F0:
goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Var_e_struct*_tmp99B=(struct Cyc_Absyn_Var_e_struct*)
_tmp952;if(_tmp99B->tag != 1)goto _LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp99C=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp952;if(_tmp99C->tag != 2)goto
_LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Enum_e_struct*_tmp99D=(struct
Cyc_Absyn_Enum_e_struct*)_tmp952;if(_tmp99D->tag != 32)goto _LL3F7;}_LL3F6: goto
_LL3F8;_LL3F7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp99E=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp952;if(_tmp99E->tag != 33)goto _LL3F9;}_LL3F8: goto _LL3BA;_LL3F9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp99F=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp952;if(_tmp99F->tag != 30)goto
_LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Datatype_e_struct*_tmp9A0=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp952;if(_tmp9A0->tag != 31)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Aggregate_e_struct*_tmp9A1=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp952;if(_tmp9A1->tag != 29)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9A2=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp952;if(_tmp9A2->tag != 28)
goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_struct*_tmp9A3=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp952;if(_tmp9A3->tag != 25)goto _LL403;}_LL402:
goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9A4=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp952;if(_tmp9A4->tag != 14)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_New_e_struct*
_tmp9A5=(struct Cyc_Absyn_New_e_struct*)_tmp952;if(_tmp9A5->tag != 17)goto _LL407;}
_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_struct*_tmp9A6=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp952;if(_tmp9A6->tag != 39)goto _LL409;}_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9A7=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp952;if(_tmp9A7->tag != 38)goto
_LL3BA;}_LL40A:{const char*_tmpDEF;void*_tmpDEE;(_tmpDEE=0,Cyc_Tcutil_terr(e->loc,((
_tmpDEF="Cyclone expression within C code",_tag_dyneither(_tmpDEF,sizeof(char),
33))),_tag_dyneither(_tmpDEE,sizeof(void*),0)));}goto _LL3BA;_LL3BA:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp9AD=d->r;struct Cyc_Absyn_Vardecl*_tmp9AF;struct
Cyc_Absyn_Fndecl*_tmp9B1;struct Cyc_Absyn_Aggrdecl*_tmp9B3;struct Cyc_Absyn_Enumdecl*
_tmp9B5;struct Cyc_Absyn_Typedefdecl*_tmp9B7;_LL40C: {struct Cyc_Absyn_Var_d_struct*
_tmp9AE=(struct Cyc_Absyn_Var_d_struct*)_tmp9AD;if(_tmp9AE->tag != 0)goto _LL40E;
else{_tmp9AF=_tmp9AE->f1;}}_LL40D: _tmp9AF->type=Cyc_Toc_typ_to_c(_tmp9AF->type);
if(_tmp9AF->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9AF->initializer));goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_struct*
_tmp9B0=(struct Cyc_Absyn_Fn_d_struct*)_tmp9AD;if(_tmp9B0->tag != 1)goto _LL410;
else{_tmp9B1=_tmp9B0->f1;}}_LL40F: _tmp9B1->ret_type=Cyc_Toc_typ_to_c(_tmp9B1->ret_type);{
struct Cyc_List_List*_tmp9C3=_tmp9B1->args;for(0;_tmp9C3 != 0;_tmp9C3=_tmp9C3->tl){(*((
struct _tuple25*)_tmp9C3->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9C3->hd)).f3);}}
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_struct*_tmp9B2=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9AD;if(_tmp9B2->tag != 6)goto _LL412;else{_tmp9B3=_tmp9B2->f1;}}_LL411: Cyc_Toc_aggrdecl_to_c(
_tmp9B3);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_struct*_tmp9B4=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9AD;if(_tmp9B4->tag != 8)goto _LL414;else{_tmp9B5=_tmp9B4->f1;}}_LL413: if(
_tmp9B5->fields != 0){struct Cyc_List_List*_tmp9C4=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9B5->fields))->v;for(0;_tmp9C4 != 0;_tmp9C4=_tmp9C4->tl){
struct Cyc_Absyn_Enumfield*_tmp9C5=(struct Cyc_Absyn_Enumfield*)_tmp9C4->hd;if(
_tmp9C5->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C5->tag));}}
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_struct*_tmp9B6=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9AD;if(_tmp9B6->tag != 9)goto _LL416;else{_tmp9B7=_tmp9B6->f1;}}_LL415:{struct
Cyc_Core_Opt*_tmpDF0;_tmp9B7->defn=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((
_tmpDF0->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B7->defn))->v),_tmpDF0))));}goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_struct*
_tmp9B8=(struct Cyc_Absyn_Let_d_struct*)_tmp9AD;if(_tmp9B8->tag != 2)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_struct*_tmp9B9=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9AD;if(_tmp9B9->tag != 3)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9BA=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9AD;if(_tmp9BA->tag != 7)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_struct*_tmp9BB=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9AD;if(_tmp9BB->tag != 10)goto _LL41E;}
_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_struct*_tmp9BC=(struct Cyc_Absyn_Using_d_struct*)
_tmp9AD;if(_tmp9BC->tag != 11)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9BD=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9AD;if(_tmp9BD->tag != 12)goto
_LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9BE=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9AD;if(_tmp9BE->tag != 13)
goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_struct*_tmp9BF=(
struct Cyc_Absyn_Region_d_struct*)_tmp9AD;if(_tmp9BF->tag != 4)goto _LL426;}_LL425:
goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_struct*_tmp9C0=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9AD;if(_tmp9C0->tag != 5)goto _LL428;}_LL427:{const char*_tmpDF3;void*_tmpDF2;(
_tmpDF2=0,Cyc_Tcutil_terr(d->loc,((_tmpDF3="Cyclone declaration within C code",
_tag_dyneither(_tmpDF3,sizeof(char),34))),_tag_dyneither(_tmpDF2,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_struct*_tmp9C1=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9AD;if(_tmp9C1->tag != 14)goto _LL42A;}_LL429: goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9C2=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9AD;if(_tmp9C2->tag != 15)goto
_LL40B;}_LL42B: goto _LL40B;_LL40B:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9C9=s->r;
struct Cyc_Absyn_Exp*_tmp9CB;struct Cyc_Absyn_Stmt*_tmp9CD;struct Cyc_Absyn_Stmt*
_tmp9CE;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Stmt*
_tmp9D3;struct Cyc_Absyn_Stmt*_tmp9D4;struct _tuple8 _tmp9D6;struct Cyc_Absyn_Exp*
_tmp9D7;struct Cyc_Absyn_Stmt*_tmp9D8;struct Cyc_Absyn_Exp*_tmp9DA;struct _tuple8
_tmp9DB;struct Cyc_Absyn_Exp*_tmp9DC;struct _tuple8 _tmp9DD;struct Cyc_Absyn_Exp*
_tmp9DE;struct Cyc_Absyn_Stmt*_tmp9DF;struct Cyc_Absyn_Exp*_tmp9E1;struct Cyc_List_List*
_tmp9E2;struct Cyc_Absyn_Decl*_tmp9E4;struct Cyc_Absyn_Stmt*_tmp9E5;struct Cyc_Absyn_Stmt*
_tmp9E7;struct _tuple8 _tmp9E8;struct Cyc_Absyn_Exp*_tmp9E9;_LL42D: {struct Cyc_Absyn_Exp_s_struct*
_tmp9CA=(struct Cyc_Absyn_Exp_s_struct*)_tmp9C9;if(_tmp9CA->tag != 1)goto _LL42F;
else{_tmp9CB=_tmp9CA->f1;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9CB);goto _LL42C;
_LL42F: {struct Cyc_Absyn_Seq_s_struct*_tmp9CC=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9C9;if(_tmp9CC->tag != 2)goto _LL431;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}
_LL430: Cyc_Toc_stmttypes_to_c(_tmp9CD);Cyc_Toc_stmttypes_to_c(_tmp9CE);goto
_LL42C;_LL431: {struct Cyc_Absyn_Return_s_struct*_tmp9CF=(struct Cyc_Absyn_Return_s_struct*)
_tmp9C9;if(_tmp9CF->tag != 3)goto _LL433;else{_tmp9D0=_tmp9CF->f1;}}_LL432: if(
_tmp9D0 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9D0);goto _LL42C;
_LL433: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9D1=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9C9;if(_tmp9D1->tag != 4)goto _LL435;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=_tmp9D1->f2;
_tmp9D4=_tmp9D1->f3;}}_LL434: Cyc_Toc_exptypes_to_c(_tmp9D2);Cyc_Toc_stmttypes_to_c(
_tmp9D3);Cyc_Toc_stmttypes_to_c(_tmp9D4);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_struct*
_tmp9D5=(struct Cyc_Absyn_While_s_struct*)_tmp9C9;if(_tmp9D5->tag != 5)goto _LL437;
else{_tmp9D6=_tmp9D5->f1;_tmp9D7=_tmp9D6.f1;_tmp9D8=_tmp9D5->f2;}}_LL436: Cyc_Toc_exptypes_to_c(
_tmp9D7);Cyc_Toc_stmttypes_to_c(_tmp9D8);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_struct*
_tmp9D9=(struct Cyc_Absyn_For_s_struct*)_tmp9C9;if(_tmp9D9->tag != 9)goto _LL439;
else{_tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f2;_tmp9DC=_tmp9DB.f1;_tmp9DD=_tmp9D9->f3;
_tmp9DE=_tmp9DD.f1;_tmp9DF=_tmp9D9->f4;}}_LL438: Cyc_Toc_exptypes_to_c(_tmp9DA);
Cyc_Toc_exptypes_to_c(_tmp9DC);Cyc_Toc_exptypes_to_c(_tmp9DE);Cyc_Toc_stmttypes_to_c(
_tmp9DF);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_struct*_tmp9E0=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9C9;if(_tmp9E0->tag != 10)goto _LL43B;else{_tmp9E1=_tmp9E0->f1;_tmp9E2=_tmp9E0->f2;}}
_LL43A: Cyc_Toc_exptypes_to_c(_tmp9E1);for(0;_tmp9E2 != 0;_tmp9E2=_tmp9E2->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9E2->hd)->body);}goto _LL42C;_LL43B: {struct
Cyc_Absyn_Decl_s_struct*_tmp9E3=(struct Cyc_Absyn_Decl_s_struct*)_tmp9C9;if(
_tmp9E3->tag != 12)goto _LL43D;else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E3->f2;}}
_LL43C: Cyc_Toc_decltypes_to_c(_tmp9E4);Cyc_Toc_stmttypes_to_c(_tmp9E5);goto
_LL42C;_LL43D: {struct Cyc_Absyn_Do_s_struct*_tmp9E6=(struct Cyc_Absyn_Do_s_struct*)
_tmp9C9;if(_tmp9E6->tag != 14)goto _LL43F;else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E6->f2;
_tmp9E9=_tmp9E8.f1;}}_LL43E: Cyc_Toc_stmttypes_to_c(_tmp9E7);Cyc_Toc_exptypes_to_c(
_tmp9E9);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_struct*_tmp9EA=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9C9;if(_tmp9EA->tag != 0)goto _LL441;}_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_struct*
_tmp9EB=(struct Cyc_Absyn_Break_s_struct*)_tmp9C9;if(_tmp9EB->tag != 6)goto _LL443;}
_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_struct*_tmp9EC=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9C9;if(_tmp9EC->tag != 7)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_struct*
_tmp9ED=(struct Cyc_Absyn_Goto_s_struct*)_tmp9C9;if(_tmp9ED->tag != 8)goto _LL447;}
_LL446: goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9EE=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9C9;if(_tmp9EE->tag != 11)goto _LL449;}_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_struct*
_tmp9EF=(struct Cyc_Absyn_Label_s_struct*)_tmp9C9;if(_tmp9EF->tag != 13)goto _LL44B;}
_LL44A: goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9F0=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9C9;if(_tmp9F0->tag != 15)goto _LL44D;}_LL44C: goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9F1=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9C9;if(_tmp9F1->tag != 16)goto
_LL42C;}_LL44E:{const char*_tmpDF6;void*_tmpDF5;(_tmpDF5=0,Cyc_Tcutil_terr(s->loc,((
_tmpDF6="Cyclone statement in C code",_tag_dyneither(_tmpDF6,sizeof(char),28))),
_tag_dyneither(_tmpDF5,sizeof(void*),0)));}goto _LL42C;_LL42C:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDF9;void*
_tmpDF8;(_tmpDF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDF9="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDF9,sizeof(char),29))),
_tag_dyneither(_tmpDF8,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9F6=d->r;struct Cyc_Absyn_Vardecl*_tmp9F8;struct Cyc_Absyn_Fndecl*
_tmp9FA;struct Cyc_Absyn_Aggrdecl*_tmpA00;struct Cyc_Absyn_Datatypedecl*_tmpA02;
struct Cyc_Absyn_Enumdecl*_tmpA04;struct Cyc_Absyn_Typedefdecl*_tmpA06;struct Cyc_List_List*
_tmpA0A;struct Cyc_List_List*_tmpA0C;struct Cyc_List_List*_tmpA0E;struct Cyc_List_List*
_tmpA10;_LL450: {struct Cyc_Absyn_Var_d_struct*_tmp9F7=(struct Cyc_Absyn_Var_d_struct*)
_tmp9F6;if(_tmp9F7->tag != 0)goto _LL452;else{_tmp9F8=_tmp9F7->f1;}}_LL451: {
struct _tuple0*_tmpA11=_tmp9F8->name;if(_tmp9F8->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDFA;_tmpA11=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->f1=Cyc_Absyn_Rel_n(
0),((_tmpDFA->f2=(*_tmpA11).f2,_tmpDFA))))));}if(_tmp9F8->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F8->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9F8->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDFD;struct Cyc_Absyn_Global_b_struct*_tmpDFC;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F8->name,Cyc_Absyn_varb_exp(_tmpA11,(void*)((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=1,((_tmpDFD.f1=
_tmp9F8,_tmpDFD)))),_tmpDFC)))),0));}_tmp9F8->name=_tmpA11;_tmp9F8->sc=Cyc_Toc_scope_to_c(
_tmp9F8->sc);_tmp9F8->type=Cyc_Toc_typ_to_c_array(_tmp9F8->type);{struct Cyc_List_List*
_tmpDFE;Cyc_Toc_result_decls=((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=
d,((_tmpDFE->tl=Cyc_Toc_result_decls,_tmpDFE))))));}goto _LL44F;}_LL452: {struct
Cyc_Absyn_Fn_d_struct*_tmp9F9=(struct Cyc_Absyn_Fn_d_struct*)_tmp9F6;if(_tmp9F9->tag
!= 1)goto _LL454;else{_tmp9FA=_tmp9F9->f1;}}_LL453: {struct _tuple0*_tmpA16=
_tmp9FA->name;if(_tmp9FA->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDFF;_tmpA16=((
_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->f1=Cyc_Absyn_Rel_n(0),((_tmpDFF->f2=(*
_tmpA16).f2,_tmpDFF))))));}_tmp9FA->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9FA->name,Cyc_Absyn_var_exp(_tmpA16,0));_tmp9FA->name=_tmpA16;Cyc_Toc_fndecl_to_c(
nv,_tmp9FA,cinclude);{struct Cyc_List_List*_tmpE00;Cyc_Toc_result_decls=((_tmpE00=
_cycalloc(sizeof(*_tmpE00)),((_tmpE00->hd=d,((_tmpE00->tl=Cyc_Toc_result_decls,
_tmpE00))))));}goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_struct*_tmp9FB=(struct
Cyc_Absyn_Let_d_struct*)_tmp9F6;if(_tmp9FB->tag != 2)goto _LL456;}_LL455: goto
_LL457;_LL456: {struct Cyc_Absyn_Letv_d_struct*_tmp9FC=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9F6;if(_tmp9FC->tag != 3)goto _LL458;}_LL457: {const char*_tmpE03;void*_tmpE02;(
_tmpE02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE03="letdecl at toplevel",_tag_dyneither(_tmpE03,sizeof(char),20))),
_tag_dyneither(_tmpE02,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_struct*
_tmp9FD=(struct Cyc_Absyn_Region_d_struct*)_tmp9F6;if(_tmp9FD->tag != 4)goto _LL45A;}
_LL459: {const char*_tmpE06;void*_tmpE05;(_tmpE05=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE06="region decl at toplevel",
_tag_dyneither(_tmpE06,sizeof(char),24))),_tag_dyneither(_tmpE05,sizeof(void*),0)));}
_LL45A: {struct Cyc_Absyn_Alias_d_struct*_tmp9FE=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9F6;if(_tmp9FE->tag != 5)goto _LL45C;}_LL45B: {const char*_tmpE09;void*_tmpE08;(
_tmpE08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpE09="alias decl at toplevel",_tag_dyneither(_tmpE09,sizeof(char),23))),
_tag_dyneither(_tmpE08,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9FF=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9F6;if(_tmp9FF->tag != 6)goto _LL45E;
else{_tmpA00=_tmp9FF->f1;}}_LL45D: Cyc_Toc_aggrdecl_to_c(_tmpA00);goto _LL44F;
_LL45E: {struct Cyc_Absyn_Datatype_d_struct*_tmpA01=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9F6;if(_tmpA01->tag != 7)goto _LL460;else{_tmpA02=_tmpA01->f1;}}_LL45F: if(
_tmpA02->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA02);else{Cyc_Toc_datatypedecl_to_c(
_tmpA02);}goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_struct*_tmpA03=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9F6;if(_tmpA03->tag != 8)goto _LL462;else{_tmpA04=_tmpA03->f1;}}_LL461: Cyc_Toc_enumdecl_to_c(
nv,_tmpA04);{struct Cyc_List_List*_tmpE0A;Cyc_Toc_result_decls=((_tmpE0A=
_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->hd=d,((_tmpE0A->tl=Cyc_Toc_result_decls,
_tmpE0A))))));}goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_struct*_tmpA05=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9F6;if(_tmpA05->tag != 9)goto _LL464;else{
_tmpA06=_tmpA05->f1;}}_LL463: _tmpA06->name=_tmpA06->name;_tmpA06->tvs=0;if(
_tmpA06->defn != 0){struct Cyc_Core_Opt*_tmpE0B;_tmpA06->defn=((_tmpE0B=_cycalloc(
sizeof(*_tmpE0B)),((_tmpE0B->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA06->defn))->v),_tmpE0B))));}else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmpA06->kind))->v){case Cyc_Absyn_BoxKind: _LL470:{
struct Cyc_Core_Opt*_tmpE0C;_tmpA06->defn=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((
_tmpE0C->v=Cyc_Absyn_void_star_typ(),_tmpE0C))));}break;default: _LL471:{struct
Cyc_Core_Opt*_tmpE0D;_tmpA06->defn=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((
_tmpE0D->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE0D))));}break;}}{struct Cyc_List_List*
_tmpE0E;Cyc_Toc_result_decls=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=
d,((_tmpE0E->tl=Cyc_Toc_result_decls,_tmpE0E))))));}goto _LL44F;_LL464: {struct
Cyc_Absyn_Porton_d_struct*_tmpA07=(struct Cyc_Absyn_Porton_d_struct*)_tmp9F6;if(
_tmpA07->tag != 14)goto _LL466;}_LL465: goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA08=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9F6;if(_tmpA08->tag != 15)goto
_LL468;}_LL467: goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_struct*_tmpA09=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9F6;if(_tmpA09->tag != 10)goto _LL46A;
else{_tmpA0A=_tmpA09->f2;}}_LL469: _tmpA0C=_tmpA0A;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_struct*
_tmpA0B=(struct Cyc_Absyn_Using_d_struct*)_tmp9F6;if(_tmpA0B->tag != 11)goto _LL46C;
else{_tmpA0C=_tmpA0B->f2;}}_LL46B: _tmpA0E=_tmpA0C;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA0D=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9F6;if(_tmpA0D->tag != 12)goto
_LL46E;else{_tmpA0E=_tmpA0D->f1;}}_LL46D: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0E,top,
cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA0F=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9F6;if(_tmpA0F->tag != 13)goto _LL44F;
else{_tmpA10=_tmpA0F->f1;}}_LL46F: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA10,top,1);goto
_LL44F;_LL44F:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA25;struct Cyc_Core_NewRegion _tmpA24=Cyc_Core_new_dynregion();
_tmpA25=_tmpA24.dynregion;{struct _DynRegionFrame _tmpA26;struct _RegionHandle*d=
_open_dynregion(& _tmpA26,_tmpA25);{struct Cyc_Dict_Dict*_tmpE19;struct Cyc_Dict_Dict*
_tmpE18;struct Cyc_Set_Set**_tmpE17;struct Cyc_Dict_Dict*_tmpE16;struct Cyc_List_List**
_tmpE15;struct Cyc_Toc_TocState*_tmpE14;Cyc_Toc_toc_state=((_tmpE14=_cycalloc(
sizeof(*_tmpE14)),((_tmpE14->dyn=(struct _DynRegionHandle*)_tmpA25,((_tmpE14->tuple_types=(
struct Cyc_List_List**)((_tmpE15=_region_malloc(d,sizeof(*_tmpE15)),((_tmpE15[0]=
0,_tmpE15)))),((_tmpE14->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE16=
_region_malloc(d,sizeof(*_tmpE16)),((_tmpE16[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE16)))),((_tmpE14->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE17=
_region_malloc(d,sizeof(*_tmpE17)),((_tmpE17[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE17)))),((_tmpE14->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE18=
_region_malloc(d,sizeof(*_tmpE18)),((_tmpE18[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE18)))),((_tmpE14->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE19=_region_malloc(
d,sizeof(*_tmpE19)),((_tmpE19[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE19)))),((_tmpE14->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE14))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE1A;Cyc_Toc_globals=_tag_dyneither(((_tmpE1A=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE1A[0]=& Cyc_Toc__throw_str,((_tmpE1A[1]=&
Cyc_Toc_setjmp_str,((_tmpE1A[2]=& Cyc_Toc__push_handler_str,((_tmpE1A[3]=& Cyc_Toc__pop_handler_str,((
_tmpE1A[4]=& Cyc_Toc__exn_thrown_str,((_tmpE1A[5]=& Cyc_Toc__npop_handler_str,((
_tmpE1A[6]=& Cyc_Toc__check_null_str,((_tmpE1A[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE1A[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE1A[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE1A[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE1A[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE1A[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE1A[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE1A[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE1A[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE1A[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE1A[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE1A[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE1A[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE1A[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE1A[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE1A[22]=& Cyc_Toc__cycalloc_str,((_tmpE1A[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE1A[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE1A[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE1A[26]=& Cyc_Toc__region_malloc_str,((_tmpE1A[27]=& Cyc_Toc__region_calloc_str,((
_tmpE1A[28]=& Cyc_Toc__check_times_str,((_tmpE1A[29]=& Cyc_Toc__new_region_str,((
_tmpE1A[30]=& Cyc_Toc__push_region_str,((_tmpE1A[31]=& Cyc_Toc__pop_region_str,((
_tmpE1A[32]=& Cyc_Toc__open_dynregion_str,((_tmpE1A[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE1A[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE1A[35]=& Cyc_Toc__reset_region_str,((
_tmpE1A[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE1A[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE1A)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA2E=_new_region("start");struct
_RegionHandle*start=& _tmpA2E;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA30;struct Cyc_Toc_TocState _tmpA2F=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA30=_tmpA2F.dyn;Cyc_Core_free_dynregion(
_tmpA30);};{struct Cyc_List_List*_tmpA31=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA31;};;
_pop_region(start);};}
