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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
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
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
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
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
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
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
#endif

#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))


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
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

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
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
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
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
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

/* FIX?  Not sure if we want to pass filename and lineno in here... */
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
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
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

# 26 "cycboot.h"
 typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 35 "core.h"
typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 256 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 261
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 268
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 72
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[4]="Bad";char Cyc_Arg_Error[6]="Error";struct Cyc_Arg_Prefix_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Exact_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple0{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _dyneither_ptr x){
while(l != 0){
struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned long _tmp1=Cyc_strlen((struct _dyneither_ptr)_tmp0);
if(_tmp1 > 0  && (*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}{
# 80
struct Cyc_Core_Not_found_exn_struct _tmp77;struct Cyc_Core_Not_found_exn_struct*_tmp76;(int)_throw((void*)((_tmp76=_cycalloc_atomic(sizeof(*_tmp76)),((_tmp76[0]=((_tmp77.tag=Cyc_Core_Not_found,_tmp77)),_tmp76)))));};}
# 89
static struct _dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _dyneither_ptr s){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp4=Cyc_strlen((struct _dyneither_ptr)s);
{const char*_tmp79;const char*_tmp78;_tmp4 <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp78="len <= numelts(s)",_tag_dyneither(_tmp78,sizeof(char),18))),((_tmp79="arg.cyc",_tag_dyneither(_tmp79,sizeof(char),8))),93);}
if(howmuch > _tmp4){
Cyc_Buffer_add_string(b,s);
return _tag_dyneither(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0  && !isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && !isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _dyneither_ptr whatsleft=_tag_dyneither(0,0,0);
# 116
for(0;(i < _tmp4  && (int)((const char*)s.curr)[i]) && isspace((int)((const char*)s.curr)[i]);++ i){
;}
if(i < _tmp4  && (int)((const char*)s.curr)[i])whatsleft=_dyneither_ptr_plus(s,sizeof(char),i);
return whatsleft;};};};}void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc);static void _tmp7F(unsigned int*_tmp7E,unsigned int*_tmp7D,char**_tmp7B){for(*_tmp7E=0;*_tmp7E < *_tmp7D;(*_tmp7E)++){(*_tmp7B)[*_tmp7E]=(char)'\000';}}static void _tmp88(unsigned int*_tmp87,unsigned int*_tmp86,char**_tmp84){for(*_tmp87=0;*_tmp87 < *_tmp86;(*_tmp87)++){(*_tmp84)[*_tmp87]=(char)'\000';}}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc){
# 129
if((char*)item.curr != (char*)(_tag_dyneither(0,0,0)).curr)Cyc_Buffer_add_string(b,item);
if((char*)desc.curr == (char*)(_tag_dyneither(0,0,0)).curr)return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
unsigned int _tmp7E;unsigned int _tmp7D;struct _dyneither_ptr _tmp7C;char*_tmp7B;unsigned int _tmp7A;struct _dyneither_ptr indentstr=(_tmp7A=(unsigned int)(indent + 2),((_tmp7B=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp7A + 1)),((_tmp7C=_tag_dyneither(_tmp7B,sizeof(char),_tmp7A + 1),((((_tmp7D=_tmp7A,((_tmp7F(& _tmp7E,& _tmp7D,& _tmp7B),_tmp7B[_tmp7D]=(char)0)))),_tmp7C)))))));
{unsigned int i=0;for(0;i < indent + 1;++ i){
char _tmp82;char _tmp81;struct _dyneither_ptr _tmp80;(_tmp80=_dyneither_ptr_plus(indentstr,sizeof(char),(int)i),((_tmp81=*((char*)_check_dyneither_subscript(_tmp80,sizeof(char),0)),((_tmp82=i == 0?'\n':' ',((_get_dyneither_size(_tmp80,sizeof(char))== 1  && (_tmp81 == '\000'  && _tmp82 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp80.curr)=_tmp82)))))));}}{
unsigned long _tmpA=Cyc_strlen((struct _dyneither_ptr)item);
struct _dyneither_ptr itemsep;
if(Cyc_strlen((struct _dyneither_ptr)desc)> 0){
if(_tmpA + 1 > indent)
itemsep=indentstr;else{
# 144
unsigned int _tmp87;unsigned int _tmp86;struct _dyneither_ptr _tmp85;char*_tmp84;unsigned int _tmp83;struct _dyneither_ptr temp=(_tmp83=(indent - _tmpA)+ 1,((_tmp84=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp83 + 1)),((_tmp85=_tag_dyneither(_tmp84,sizeof(char),_tmp83 + 1),((((_tmp86=_tmp83,((_tmp88(& _tmp87,& _tmp86,& _tmp84),_tmp84[_tmp86]=(char)0)))),_tmp85)))))));
{unsigned int i=0;for(0;i < indent - _tmpA;++ i){char _tmp8B;char _tmp8A;struct _dyneither_ptr _tmp89;(_tmp89=_dyneither_ptr_plus(temp,sizeof(char),(int)i),((_tmp8A=*((char*)_check_dyneither_subscript(_tmp89,sizeof(char),0)),((_tmp8B=' ',((_get_dyneither_size(_tmp89,sizeof(char))== 1  && (_tmp8A == '\000'  && _tmp8B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp89.curr)=_tmp8B)))))));}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);
# 153
while((char*)desc.curr != (char*)(_tag_dyneither(0,0,0)).curr){
desc=Cyc_Arg_Justify_break_line(b,margin - indent,desc);
if((char*)desc.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)indentstr);else{
# 158
const char*_tmp8C;Cyc_Buffer_add_string(b,((_tmp8C="\n",_tag_dyneither(_tmp8C,sizeof(char),2))));}}
# 160
return;};};}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 166
{const char*_tmp90;void*_tmp8F[1];struct Cyc_String_pa_PrintArg_struct _tmp8E;(_tmp8E.tag=0,((_tmp8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)errmsg),((_tmp8F[0]=& _tmp8E,Cyc_fprintf(Cyc_stderr,((_tmp90="%s\n",_tag_dyneither(_tmp90,sizeof(char),4))),_tag_dyneither(_tmp8F,sizeof(void*),1)))))));}{
struct Cyc_Buffer_t*_tmp1A=Cyc_Buffer_create(1024);
while(speclist != 0){
Cyc_Arg_Justify_justify_b(_tmp1A,12,72,(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);
# 172
speclist=speclist->tl;}{
# 174
const char*_tmp94;void*_tmp93[1];struct Cyc_String_pa_PrintArg_struct _tmp92;(_tmp92.tag=0,((_tmp92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Buffer_contents(_tmp1A)),((_tmp93[0]=& _tmp92,Cyc_fprintf(Cyc_stderr,((_tmp94="%s",_tag_dyneither(_tmp94,sizeof(char),3))),_tag_dyneither(_tmp93,sizeof(void*),1)))))));};};}
# 177
int Cyc_Arg_current=0;
static struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 183
const char*_tmp95;struct _dyneither_ptr progname=prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr))?*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),prog_pos)):((_tmp95="(?)",_tag_dyneither(_tmp95,sizeof(char),4)));
{void*_tmp1E=e;struct _dyneither_ptr _tmp20;struct _dyneither_ptr _tmp22;struct _dyneither_ptr _tmp24;struct _dyneither_ptr _tmp25;struct _dyneither_ptr _tmp26;struct _dyneither_ptr _tmp28;_LL1: {struct Cyc_Arg_Unknown_Arg_error_struct*_tmp1F=(struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp1E;if(_tmp1F->tag != 0)goto _LL3;else{_tmp20=_tmp1F->f1;}}_LL2:
# 186
{const char*_tmp96;if(Cyc_strcmp((struct _dyneither_ptr)_tmp20,((_tmp96="-help",_tag_dyneither(_tmp96,sizeof(char),6))))!= 0){
const char*_tmp9B;void*_tmp9A[2];struct Cyc_String_pa_PrintArg_struct _tmp99;struct Cyc_String_pa_PrintArg_struct _tmp98;(_tmp98.tag=0,((_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20),((_tmp99.tag=0,((_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp9A[0]=& _tmp99,((_tmp9A[1]=& _tmp98,Cyc_fprintf(Cyc_stderr,((_tmp9B="%s: unknown option `%s'.\n",_tag_dyneither(_tmp9B,sizeof(char),26))),_tag_dyneither(_tmp9A,sizeof(void*),2)))))))))))));}}
goto _LL0;_LL3: {struct Cyc_Arg_Missing_Arg_error_struct*_tmp21=(struct Cyc_Arg_Missing_Arg_error_struct*)_tmp1E;if(_tmp21->tag != 1)goto _LL5;else{_tmp22=_tmp21->f1;}}_LL4:
# 190
{const char*_tmpA0;void*_tmp9F[2];struct Cyc_String_pa_PrintArg_struct _tmp9E;struct Cyc_String_pa_PrintArg_struct _tmp9D;(_tmp9D.tag=0,((_tmp9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22),((_tmp9E.tag=0,((_tmp9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp9F[0]=& _tmp9E,((_tmp9F[1]=& _tmp9D,Cyc_fprintf(Cyc_stderr,((_tmpA0="%s: option `%s' needs an argument.\n",_tag_dyneither(_tmpA0,sizeof(char),36))),_tag_dyneither(_tmp9F,sizeof(void*),2)))))))))))));}
goto _LL0;_LL5: {struct Cyc_Arg_Wrong_Arg_error_struct*_tmp23=(struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp1E;if(_tmp23->tag != 3)goto _LL7;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;_tmp26=_tmp23->f3;}}_LL6:
# 193
{const char*_tmpA7;void*_tmpA6[4];struct Cyc_String_pa_PrintArg_struct _tmpA5;struct Cyc_String_pa_PrintArg_struct _tmpA4;struct Cyc_String_pa_PrintArg_struct _tmpA3;struct Cyc_String_pa_PrintArg_struct _tmpA2;(_tmpA2.tag=0,((_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26),((_tmpA3.tag=0,((_tmpA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp24),((_tmpA4.tag=0,((_tmpA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25),((_tmpA5.tag=0,((_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmpA6[0]=& _tmpA5,((_tmpA6[1]=& _tmpA4,((_tmpA6[2]=& _tmpA3,((_tmpA6[3]=& _tmpA2,Cyc_fprintf(Cyc_stderr,((_tmpA7="%s: wrong argument `%s'; option `%s' expects %s.\n",_tag_dyneither(_tmpA7,sizeof(char),50))),_tag_dyneither(_tmpA6,sizeof(void*),4)))))))))))))))))))))))));}
# 195
goto _LL0;_LL7: {struct Cyc_Arg_Message_Arg_error_struct*_tmp27=(struct Cyc_Arg_Message_Arg_error_struct*)_tmp1E;if(_tmp27->tag != 2)goto _LL0;else{_tmp28=_tmp27->f1;}}_LL8:
# 197
{const char*_tmpAC;void*_tmpAB[2];struct Cyc_String_pa_PrintArg_struct _tmpAA;struct Cyc_String_pa_PrintArg_struct _tmpA9;(_tmpA9.tag=0,((_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28),((_tmpAA.tag=0,((_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname),((_tmpAB[0]=& _tmpAA,((_tmpAB[1]=& _tmpA9,Cyc_fprintf(Cyc_stderr,((_tmpAC="%s: %s.\n",_tag_dyneither(_tmpAC,sizeof(char),9))),_tag_dyneither(_tmpAB,sizeof(void*),2)))))))))))));}
goto _LL0;_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr orig_args){
# 208
Cyc_Arg_args=orig_args;{
# 210
int initpos=Cyc_Arg_current;
unsigned int l=_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));
# 213
if((char*)((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),(int)(l - 1)))->curr == (char*)(_tag_dyneither(0,0,0)).curr)l=l - 1;
++ Cyc_Arg_current;
while(Cyc_Arg_current < l){
struct _dyneither_ptr s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current));
if(((char*)s.curr != (char*)(_tag_dyneither(0,0,0)).curr  && _get_dyneither_size(s,sizeof(char))>= 1) && ((const char*)s.curr)[0]== '-'){
void*action;
{struct _handler_cons _tmp3D;_push_handler(& _tmp3D);{int _tmp3F=0;if(setjmp(_tmp3D.handler))_tmp3F=1;if(!_tmp3F){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{void*_tmp3E=(void*)_exn_thrown;void*_tmp41=_tmp3E;void*_tmp43;_LLA: {struct Cyc_Core_Not_found_exn_struct*_tmp42=(struct Cyc_Core_Not_found_exn_struct*)_tmp41;if(_tmp42->tag != Cyc_Core_Not_found)goto _LLC;}_LLB:
# 221
{struct Cyc_Arg_Unknown_Arg_error_struct _tmpAF;struct Cyc_Arg_Unknown_Arg_error_struct*_tmpAE;Cyc_Arg_stop(initpos,(void*)((_tmpAE=_cycalloc(sizeof(*_tmpAE)),((_tmpAE[0]=((_tmpAF.tag=0,((_tmpAF.f1=s,_tmpAF)))),_tmpAE)))),speclist,errmsg);}return;_LLC: _tmp43=_tmp41;_LLD:(void)_throw(_tmp43);_LL9:;}};}
# 223
{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))_tmp48=1;if(!_tmp48){
{void*_tmp49=action;void(*_tmp4B)();void(*_tmp4D)(struct _dyneither_ptr);int*_tmp4F;int*_tmp51;void(*_tmp53)(struct _dyneither_ptr,struct _dyneither_ptr);void(*_tmp55)(struct _dyneither_ptr);void(*_tmp57)(int);void(*_tmp59)(struct _dyneither_ptr);_LLF: {struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp4A=(struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp49;if(_tmp4A->tag != 0)goto _LL11;else{_tmp4B=_tmp4A->f1;}}_LL10:
 _tmp4B();goto _LLE;_LL11: {struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp4C=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp49;if(_tmp4C->tag != 1)goto _LL13;else{_tmp4D=_tmp4C->f1;}}_LL12:
 _tmp4D(s);goto _LLE;_LL13: {struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp4E=(struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp49;if(_tmp4E->tag != 3)goto _LL15;else{_tmp4F=_tmp4E->f1;}}_LL14:
*_tmp4F=1;goto _LLE;_LL15: {struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp50=(struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp49;if(_tmp50->tag != 4)goto _LL17;else{_tmp51=_tmp50->f1;}}_LL16:
*_tmp51=0;goto _LLE;_LL17: {struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*_tmp52=(struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp49;if(_tmp52->tag != 2)goto _LL19;else{_tmp53=_tmp52->f1;}}_LL18:
# 230
 if(Cyc_Arg_current + 1 < l){
_tmp53(s,*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 234
struct Cyc_Arg_Missing_Arg_error_struct _tmpB2;struct Cyc_Arg_Missing_Arg_error_struct*_tmpB1;Cyc_Arg_stop(initpos,(void*)((_tmpB1=_cycalloc(sizeof(*_tmpB1)),((_tmpB1[0]=((_tmpB2.tag=1,((_tmpB2.f1=s,_tmpB2)))),_tmpB1)))),speclist,errmsg);}
goto _LLE;_LL19: {struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp54=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp49;if(_tmp54->tag != 5)goto _LL1B;else{_tmp55=_tmp54->f1;}}_LL1A:
# 237
 if(Cyc_Arg_current + 1 < l){
_tmp55(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 241
struct Cyc_Arg_Missing_Arg_error_struct _tmpB5;struct Cyc_Arg_Missing_Arg_error_struct*_tmpB4;Cyc_Arg_stop(initpos,(void*)((_tmpB4=_cycalloc(sizeof(*_tmpB4)),((_tmpB4[0]=((_tmpB5.tag=1,((_tmpB5.f1=s,_tmpB5)))),_tmpB4)))),speclist,errmsg);}
goto _LLE;_LL1B: {struct Cyc_Arg_Int_spec_Arg_Spec_struct*_tmp56=(struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp49;if(_tmp56->tag != 6)goto _LL1D;else{_tmp57=_tmp56->f1;}}_LL1C: {
# 244
struct _dyneither_ptr arg=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1));
int n=0;
{const char*_tmpB9;void*_tmpB8[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmpB7;if(((_tmpB7.tag=2,((_tmpB7.f1=& n,((_tmpB8[0]=& _tmpB7,Cyc_sscanf(arg,((_tmpB9="%d",_tag_dyneither(_tmpB9,sizeof(char),3))),_tag_dyneither(_tmpB8,sizeof(void*),1))))))))!= 1){
{struct Cyc_Arg_Wrong_Arg_error_struct _tmpBF;const char*_tmpBE;struct Cyc_Arg_Wrong_Arg_error_struct*_tmpBD;Cyc_Arg_stop(initpos,(void*)((_tmpBD=_cycalloc(sizeof(*_tmpBD)),((_tmpBD[0]=((_tmpBF.tag=3,((_tmpBF.f1=s,((_tmpBF.f2=arg,((_tmpBF.f3=((_tmpBE="an integer",_tag_dyneither(_tmpBE,sizeof(char),11))),_tmpBF)))))))),_tmpBD)))),speclist,errmsg);}
_npop_handler(0);return;}}
# 250
_tmp57(n);
++ Cyc_Arg_current;
goto _LLE;}_LL1D: {struct Cyc_Arg_Rest_spec_Arg_Spec_struct*_tmp58=(struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp49;if(_tmp58->tag != 7)goto _LLE;else{_tmp59=_tmp58->f1;}}_LL1E:
# 254
 while(Cyc_Arg_current < l - 1){
_tmp59(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 258
goto _LLE;_LLE:;}
# 224
;_pop_handler();}else{void*_tmp47=(void*)_exn_thrown;void*_tmp65=_tmp47;struct _dyneither_ptr _tmp67;void*_tmp68;_LL20: {struct Cyc_Arg_Bad_exn_struct*_tmp66=(struct Cyc_Arg_Bad_exn_struct*)_tmp65;if(_tmp66->tag != Cyc_Arg_Bad)goto _LL22;else{_tmp67=_tmp66->f1;}}_LL21:
# 261
{struct Cyc_Arg_Message_Arg_error_struct _tmpC2;struct Cyc_Arg_Message_Arg_error_struct*_tmpC1;Cyc_Arg_stop(initpos,(void*)((_tmpC1=_cycalloc(sizeof(*_tmpC1)),((_tmpC1[0]=((_tmpC2.tag=2,((_tmpC2.f1=_tmp67,_tmpC2)))),_tmpC1)))),speclist,errmsg);}goto _LL1F;_LL22: _tmp68=_tmp65;_LL23:(void)_throw(_tmp68);_LL1F:;}};}
# 263
++ Cyc_Arg_current;}else{
# 266
{struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){anonfun(s);;_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp6F=_tmp6C;struct _dyneither_ptr _tmp71;void*_tmp72;_LL25: {struct Cyc_Arg_Bad_exn_struct*_tmp70=(struct Cyc_Arg_Bad_exn_struct*)_tmp6F;if(_tmp70->tag != Cyc_Arg_Bad)goto _LL27;else{_tmp71=_tmp70->f1;}}_LL26:
# 268
{struct Cyc_Arg_Message_Arg_error_struct _tmpC5;struct Cyc_Arg_Message_Arg_error_struct*_tmpC4;Cyc_Arg_stop(initpos,(void*)((_tmpC4=_cycalloc(sizeof(*_tmpC4)),((_tmpC4[0]=((_tmpC5.tag=2,((_tmpC5.f1=_tmp71,_tmpC5)))),_tmpC4)))),speclist,errmsg);}goto _LL24;_LL27: _tmp72=_tmp6F;_LL28:(void)_throw(_tmp72);_LL24:;}};}
# 270
++ Cyc_Arg_current;}}};}
