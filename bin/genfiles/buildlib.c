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
typedef int Cyc_bool;
# 69 "core.h"
void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;
# 38
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 234 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
# 244
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 155
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1097 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1158
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 60 "absynpp.h"
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct Cyc_FlatList{struct Cyc_FlatList*tl;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 114 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 81 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 89
extern char*Ccomp;
# 91
static int Cyc_do_setjmp=0;
# 93
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 97
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 100
if(Cyc_log_file == 0){
{const char*_tmp3DC;void*_tmp3DB;(_tmp3DB=0,Cyc_fprintf(Cyc_stderr,((_tmp3DC="Internal error: log file is NULL\n",_tag_dyneither(_tmp3DC,sizeof(char),34))),_tag_dyneither(_tmp3DB,sizeof(void*),0)));}
exit(1);}{
# 104
int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp2;};}
# 109
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp3DD;Cyc_current_targets=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr),_tmp3DD))));}struct _tuple19{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 116
typedef struct _tuple19*Cyc_line_t;
struct _tuple19*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 123
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 130
typedef struct _tuple20*Cyc_suck_line_t;
struct _tuple20*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 138
typedef struct _tuple21*Cyc_ifdefined_t;struct _tuple22{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};
# 140
typedef struct _tuple22*Cyc_spec_t;
# 142
struct _tuple22*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 160
int Cyc_numdef=0;
# 162
static struct Cyc_List_List*Cyc_cppargs=0;
# 165
const int Cyc_lex_base[425]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,1171,1185,1255};
const int Cyc_lex_backtrk[425]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[425]={- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3481]={0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3481]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 172
int state;int base;int backtrk;
int c;
state=start_state;
# 176
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 180
state=(- state)- 1;}
# 182
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 190
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 194
c=256;}}else{
# 196
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 199
if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
# 202
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp3E3;const char*_tmp3E2;struct Cyc_Lexing_Error_exn_struct*_tmp3E1;(int)_throw((void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E3.tag=Cyc_Lexing_Error,((_tmp3E3.f1=((_tmp3E2="empty token",_tag_dyneither(_tmp3E2,sizeof(char),12))),_tmp3E3)))),_tmp3E1)))));}else{
# 208
return lbuf->lex_last_action;}}else{
# 211
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 215
struct _tuple19*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL0:
# 174 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
struct Cyc_Set_Set**_tmp3E4;Cyc_current_targets=((_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((_tmp3E4[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3E4))));}{
# 179
struct _tuple19*_tmp3E5;return(_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3E5->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp3E5)))));};case 1: _LL1:
# 182 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL2:
# 184
 return 0;default: _LL3:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}{
# 188
struct Cyc_Lexing_Error_exn_struct _tmp3EB;const char*_tmp3EA;struct Cyc_Lexing_Error_exn_struct*_tmp3E9;(int)_throw((void*)((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EB.tag=Cyc_Lexing_Error,((_tmp3EB.f1=((_tmp3EA="some action didn't return!",_tag_dyneither(_tmp3EA,sizeof(char),27))),_tmp3EB)))),_tmp3E9)))));};}
# 190
struct _tuple19*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL5:
# 188 "buildlib.cyl"
{struct _dyneither_ptr*_tmp3EC;Cyc_current_source=((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3EC))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3ED;Cyc_current_targets=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3ED))));}
Cyc_token(lexbuf);
return 0;case 1: _LL6:
# 196
{struct _dyneither_ptr*_tmp3EE;Cyc_current_source=((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3EE))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3EF;Cyc_current_targets=((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3EF))));}
Cyc_args(lexbuf);
return 0;case 2: _LL7:
# 204
{struct _dyneither_ptr*_tmp3F0;Cyc_current_source=((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3F0))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3F1;Cyc_current_targets=((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3F1))));}
Cyc_token(lexbuf);
return 0;default: _LL8:
# 210
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{
# 213
struct Cyc_Lexing_Error_exn_struct _tmp3F7;const char*_tmp3F6;struct Cyc_Lexing_Error_exn_struct*_tmp3F5;(int)_throw((void*)((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F7.tag=Cyc_Lexing_Error,((_tmp3F7.f1=((_tmp3F6="some action didn't return!",_tag_dyneither(_tmp3F6,sizeof(char),27))),_tmp3F7)))),_tmp3F5)))));};}
# 215
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA: {
# 213 "buildlib.cyl"
struct _dyneither_ptr*_tmp3F8;struct _dyneither_ptr*_tmp15=(_tmp3F8=_cycalloc(sizeof(*_tmp3F8)),((_tmp3F8[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3F8)));
{struct Cyc_List_List*_tmp3F9;Cyc_current_args=((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->hd=_tmp15,((_tmp3F9->tl=Cyc_current_args,_tmp3F9))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {
# 219
struct _dyneither_ptr*_tmp3FA;struct _dyneither_ptr*_tmp18=(_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3FA)));
{struct Cyc_List_List*_tmp3FB;Cyc_current_args=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=_tmp18,((_tmp3FB->tl=Cyc_current_args,_tmp3FB))))));}
return Cyc_args(lexbuf);}case 2: _LLC: {
# 225
struct _dyneither_ptr*_tmp3FC;struct _dyneither_ptr*_tmp1B=(_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3FC)));
{struct Cyc_List_List*_tmp3FD;Cyc_current_args=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD->hd=_tmp1B,((_tmp3FD->tl=Cyc_current_args,_tmp3FD))))));}
return Cyc_token(lexbuf);}default: _LLD:
# 230
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}{
# 233
struct Cyc_Lexing_Error_exn_struct _tmp403;const char*_tmp402;struct Cyc_Lexing_Error_exn_struct*_tmp401;(int)_throw((void*)((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=((_tmp403.tag=Cyc_Lexing_Error,((_tmp403.f1=((_tmp402="some action didn't return!",_tag_dyneither(_tmp402,sizeof(char),27))),_tmp403)))),_tmp401)))));};}
# 235
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLF:
# 234 "buildlib.cyl"
{struct _dyneither_ptr*_tmp404;Cyc_add_target(((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp404)))));}return Cyc_token(lexbuf);case 1: _LL10:
# 237 "buildlib.cyl"
 return 0;case 2: _LL11:
# 240 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL12:
# 243 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL13:
# 246 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL14:
# 248
 return Cyc_token(lexbuf);case 6: _LL15:
# 250
 return Cyc_token(lexbuf);case 7: _LL16:
# 252
 return Cyc_token(lexbuf);case 8: _LL17:
# 255 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL18:
# 258 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL19:
# 261 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL1A:
# 263
 return Cyc_token(lexbuf);case 12: _LL1B:
# 265
 return Cyc_token(lexbuf);case 13: _LL1C:
# 267
 return Cyc_token(lexbuf);case 14: _LL1D:
# 269
 return Cyc_token(lexbuf);case 15: _LL1E:
# 271
 return Cyc_token(lexbuf);case 16: _LL1F:
# 273
 return Cyc_token(lexbuf);case 17: _LL20:
# 275
 return Cyc_token(lexbuf);case 18: _LL21:
# 277
 return Cyc_token(lexbuf);case 19: _LL22:
# 279
 return Cyc_token(lexbuf);case 20: _LL23:
# 281
 return Cyc_token(lexbuf);case 21: _LL24:
# 283
 return Cyc_token(lexbuf);case 22: _LL25:
# 285
 return Cyc_token(lexbuf);case 23: _LL26:
# 287
 return Cyc_token(lexbuf);case 24: _LL27:
# 290 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL28:
# 292
 return Cyc_token(lexbuf);case 26: _LL29:
# 294
 return Cyc_token(lexbuf);case 27: _LL2A:
# 296
 return Cyc_token(lexbuf);case 28: _LL2B:
# 298
 return Cyc_token(lexbuf);case 29: _LL2C:
# 300
 return Cyc_token(lexbuf);case 30: _LL2D:
# 302
 return Cyc_token(lexbuf);case 31: _LL2E:
# 304
 return Cyc_token(lexbuf);case 32: _LL2F:
# 306
 return Cyc_token(lexbuf);case 33: _LL30:
# 308
 return Cyc_token(lexbuf);case 34: _LL31:
# 310
 return Cyc_token(lexbuf);case 35: _LL32:
# 312
 return Cyc_token(lexbuf);case 36: _LL33:
# 314
 return Cyc_token(lexbuf);case 37: _LL34:
# 316
 return Cyc_token(lexbuf);case 38: _LL35:
# 318
 return Cyc_token(lexbuf);case 39: _LL36:
# 320
 return Cyc_token(lexbuf);case 40: _LL37:
# 322
 return Cyc_token(lexbuf);case 41: _LL38:
# 324
 return Cyc_token(lexbuf);case 42: _LL39:
# 326
 return Cyc_token(lexbuf);case 43: _LL3A:
# 328
 return Cyc_token(lexbuf);case 44: _LL3B:
# 330
 return Cyc_token(lexbuf);case 45: _LL3C:
# 332
 return Cyc_token(lexbuf);case 46: _LL3D:
# 334
 return Cyc_token(lexbuf);case 47: _LL3E:
# 336
 return Cyc_token(lexbuf);case 48: _LL3F:
# 339 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL40:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}{
# 343
struct Cyc_Lexing_Error_exn_struct _tmp40A;const char*_tmp409;struct Cyc_Lexing_Error_exn_struct*_tmp408;(int)_throw((void*)((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=((_tmp40A.tag=Cyc_Lexing_Error,((_tmp40A.f1=((_tmp409="some action didn't return!",_tag_dyneither(_tmp409,sizeof(char),27))),_tmp40A)))),_tmp408)))));};}
# 345
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL42:
# 344 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL43:
# 345 "buildlib.cyl"
 return 0;case 2: _LL44:
# 346 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL45:
# 347 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL46:
# 350 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL47:
# 353 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL48:
# 355
 return Cyc_string(lexbuf);case 7: _LL49:
# 356 "buildlib.cyl"
 return 0;case 8: _LL4A:
# 357 "buildlib.cyl"
 return 0;case 9: _LL4B:
# 358 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL4C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}{
# 362
struct Cyc_Lexing_Error_exn_struct _tmp410;const char*_tmp40F;struct Cyc_Lexing_Error_exn_struct*_tmp40E;(int)_throw((void*)((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=((_tmp410.tag=Cyc_Lexing_Error,((_tmp410.f1=((_tmp40F="some action didn't return!",_tag_dyneither(_tmp40F,sizeof(char),27))),_tmp410)))),_tmp40E)))));};}
# 364
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL4E:
# 367 "buildlib.cyl"
 return 0;case 1: _LL4F:
# 369
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50:
# 376 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp413;void*_tmp412;(_tmp412=0,Cyc_log(((_tmp413="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp413,sizeof(char),44))),_tag_dyneither(_tmp412,sizeof(void*),0)));}
return 1;case 3: _LL51:
# 382 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_log(((_tmp416="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp416,sizeof(char),44))),_tag_dyneither(_tmp415,sizeof(void*),0)));}
return 1;case 4: _LL52:
# 388 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp419;void*_tmp418;(_tmp418=0,Cyc_log(((_tmp419="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp419,sizeof(char),44))),_tag_dyneither(_tmp418,sizeof(void*),0)));}
return 1;case 5: _LL53:
# 394 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_log(((_tmp41C="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp41C,sizeof(char),44))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}
return 1;case 6: _LL54:
# 398
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_log(((_tmp41F="Warning: use of region sidestepped\n",_tag_dyneither(_tmp41F,sizeof(char),36))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}
return 1;case 7: _LL55:
# 402
{const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_log(((_tmp422="Warning: use of __extension__ deleted\n",_tag_dyneither(_tmp422,sizeof(char),39))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
return 1;case 8: _LL56:
# 407 "buildlib.cyl"
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_log(((_tmp425="Warning: use of mode HI deleted\n",_tag_dyneither(_tmp425,sizeof(char),33))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
return 1;case 9: _LL57:
# 410
{const char*_tmp428;void*_tmp427;(_tmp427=0,Cyc_log(((_tmp428="Warning: use of mode SI deleted\n",_tag_dyneither(_tmp428,sizeof(char),33))),_tag_dyneither(_tmp427,sizeof(void*),0)));}
return 1;case 10: _LL58:
# 413
{const char*_tmp42B;void*_tmp42A;(_tmp42A=0,Cyc_log(((_tmp42B="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp42B,sizeof(char),33))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}
return 1;case 11: _LL59:
# 416
{const char*_tmp42E;void*_tmp42D;(_tmp42D=0,Cyc_log(((_tmp42E="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp42E,sizeof(char),33))),_tag_dyneither(_tmp42D,sizeof(void*),0)));}
return 1;case 12: _LL5A:
# 419
{const char*_tmp431;void*_tmp430;(_tmp430=0,Cyc_log(((_tmp431="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp431,sizeof(char),33))),_tag_dyneither(_tmp430,sizeof(void*),0)));}
return 1;case 13: _LL5B:
# 422
{const char*_tmp434;void*_tmp433;(_tmp433=0,Cyc_log(((_tmp434="Warning: use of mode word deleted\n",_tag_dyneither(_tmp434,sizeof(char),35))),_tag_dyneither(_tmp433,sizeof(void*),0)));}
return 1;case 14: _LL5C:
# 425
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LL5D:
# 427
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E:
# 429
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LL5F:
# 431
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL60:
# 436 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LL61:
# 438
 return 1;case 20: _LL62:
# 440
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp437;void*_tmp436;(_tmp436=0,Cyc_log(((_tmp437="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp437,sizeof(char),42))),_tag_dyneither(_tmp436,sizeof(void*),0)));}
return 1;case 21: _LL63:
# 446
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{
# 450
struct Cyc_Lexing_Error_exn_struct _tmp43D;const char*_tmp43C;struct Cyc_Lexing_Error_exn_struct*_tmp43B;(int)_throw((void*)((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43D.tag=Cyc_Lexing_Error,((_tmp43D.f1=((_tmp43C="some action didn't return!",_tag_dyneither(_tmp43C,sizeof(char),27))),_tmp43D)))),_tmp43B)))));};}
# 452
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL66:
# 450 "buildlib.cyl"
 return 0;case 1: _LL67:
# 452
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL68:
# 454
{const char*_tmp440;void*_tmp43F;(_tmp43F=0,Cyc_log(((_tmp440="Warning: unclosed string\n",_tag_dyneither(_tmp440,sizeof(char),26))),_tag_dyneither(_tmp43F,sizeof(void*),0)));}
{const char*_tmp444;void*_tmp443[1];struct Cyc_String_pa_PrintArg_struct _tmp442;(_tmp442.tag=0,((_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp443[0]=& _tmp442,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp444="%s",_tag_dyneither(_tmp444,sizeof(char),3))),_tag_dyneither(_tmp443,sizeof(void*),1)))))));}return 1;case 3: _LL69:
# 457
{const char*_tmp448;void*_tmp447[1];struct Cyc_String_pa_PrintArg_struct _tmp446;(_tmp446.tag=0,((_tmp446.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp447[0]=& _tmp446,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp448="%s",_tag_dyneither(_tmp448,sizeof(char),3))),_tag_dyneither(_tmp447,sizeof(void*),1)))))));}return 1;case 4: _LL6A:
# 459
{const char*_tmp44C;void*_tmp44B[1];struct Cyc_String_pa_PrintArg_struct _tmp44A;(_tmp44A.tag=0,((_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp44B[0]=& _tmp44A,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp44C="%s",_tag_dyneither(_tmp44C,sizeof(char),3))),_tag_dyneither(_tmp44B,sizeof(void*),1)))))));}return 1;case 5: _LL6B:
# 461
{const char*_tmp450;void*_tmp44F[1];struct Cyc_String_pa_PrintArg_struct _tmp44E;(_tmp44E.tag=0,((_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp44F[0]=& _tmp44E,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp450="%s",_tag_dyneither(_tmp450,sizeof(char),3))),_tag_dyneither(_tmp44F,sizeof(void*),1)))))));}return 1;case 6: _LL6C:
# 463
{const char*_tmp454;void*_tmp453[1];struct Cyc_String_pa_PrintArg_struct _tmp452;(_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp453[0]=& _tmp452,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp454="%s",_tag_dyneither(_tmp454,sizeof(char),3))),_tag_dyneither(_tmp453,sizeof(void*),1)))))));}return 1;case 7: _LL6D:
# 465
{const char*_tmp458;void*_tmp457[1];struct Cyc_String_pa_PrintArg_struct _tmp456;(_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp457[0]=& _tmp456,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp458="%s",_tag_dyneither(_tmp458,sizeof(char),3))),_tag_dyneither(_tmp457,sizeof(void*),1)))))));}return 1;case 8: _LL6E:
# 467
{const char*_tmp45C;void*_tmp45B[1];struct Cyc_String_pa_PrintArg_struct _tmp45A;(_tmp45A.tag=0,((_tmp45A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp45B[0]=& _tmp45A,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp45C="%s",_tag_dyneither(_tmp45C,sizeof(char),3))),_tag_dyneither(_tmp45B,sizeof(void*),1)))))));}return 1;default: _LL6F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{
# 471
struct Cyc_Lexing_Error_exn_struct _tmp462;const char*_tmp461;struct Cyc_Lexing_Error_exn_struct*_tmp460;(int)_throw((void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp462.tag=Cyc_Lexing_Error,((_tmp462.f1=((_tmp461="some action didn't return!",_tag_dyneither(_tmp461,sizeof(char),27))),_tmp462)))),_tmp460)))));};}
# 473
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL71:
# 477 "buildlib.cyl"
 return 0;case 1: _LL72:
# 479
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LL73:
# 483
 ++ Cyc_parens_to_match;
return 1;case 3: _LL74:
# 486
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LL75:
# 489
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LL76:
# 492
 return 1;case 6: _LL77:
# 494
 return 1;default: _LL78:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}{
# 498
struct Cyc_Lexing_Error_exn_struct _tmp468;const char*_tmp467;struct Cyc_Lexing_Error_exn_struct*_tmp466;(int)_throw((void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((_tmp468.tag=Cyc_Lexing_Error,((_tmp468.f1=((_tmp467="some action didn't return!",_tag_dyneither(_tmp467,sizeof(char),27))),_tmp468)))),_tmp466)))));};}
# 500
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL7A:
# 498 "buildlib.cyl"
{const char*_tmp46B;void*_tmp46A;(_tmp46A=0,Cyc_log(((_tmp46B="Warning: unclosed string\n",_tag_dyneither(_tmp46B,sizeof(char),26))),_tag_dyneither(_tmp46A,sizeof(void*),0)));}return 0;case 1: _LL7B:
# 500
 return 0;case 2: _LL7C:
# 502
{const char*_tmp46E;void*_tmp46D;(_tmp46D=0,Cyc_log(((_tmp46E="Warning: unclosed string\n",_tag_dyneither(_tmp46E,sizeof(char),26))),_tag_dyneither(_tmp46D,sizeof(void*),0)));}return 1;case 3: _LL7D:
# 504
 return 1;case 4: _LL7E:
# 506
 return 1;case 5: _LL7F:
# 508
 return 1;case 6: _LL80:
# 510
 return 1;case 7: _LL81:
# 512
 return 1;case 8: _LL82:
# 514
 return 1;default: _LL83:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}{
# 518
struct Cyc_Lexing_Error_exn_struct _tmp474;const char*_tmp473;struct Cyc_Lexing_Error_exn_struct*_tmp472;(int)_throw((void*)((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472[0]=((_tmp474.tag=Cyc_Lexing_Error,((_tmp474.f1=((_tmp473="some action didn't return!",_tag_dyneither(_tmp473,sizeof(char),27))),_tmp474)))),_tmp472)))));};}
# 520
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL85:
# 518 "buildlib.cyl"
{const char*_tmp477;void*_tmp476;(_tmp476=0,Cyc_log(((_tmp477="Warning: unclosed comment\n",_tag_dyneither(_tmp477,sizeof(char),27))),_tag_dyneither(_tmp476,sizeof(void*),0)));}return 0;case 1: _LL86:
# 520
 return 0;case 2: _LL87:
# 522
 return 1;default: _LL88:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{
# 526
struct Cyc_Lexing_Error_exn_struct _tmp47D;const char*_tmp47C;struct Cyc_Lexing_Error_exn_struct*_tmp47B;(int)_throw((void*)((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B[0]=((_tmp47D.tag=Cyc_Lexing_Error,((_tmp47D.f1=((_tmp47C="some action didn't return!",_tag_dyneither(_tmp47C,sizeof(char),27))),_tmp47D)))),_tmp47B)))));};}
# 528
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple20*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL8A:
# 530 "buildlib.cyl"
{const char*_tmp47E;Cyc_current_line=((_tmp47E="#define ",_tag_dyneither(_tmp47E,sizeof(char),9)));}
Cyc_suck_macroname(lexbuf);{
struct _tuple20*_tmp47F;return(_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F->f1=Cyc_current_line,((_tmp47F->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp47F)))));};case 1: _LL8B:
# 534
 return Cyc_suck_line(lexbuf);case 2: _LL8C:
# 536
 return 0;default: _LL8D:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}{
# 540
struct Cyc_Lexing_Error_exn_struct _tmp485;const char*_tmp484;struct Cyc_Lexing_Error_exn_struct*_tmp483;(int)_throw((void*)((_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=((_tmp485.tag=Cyc_Lexing_Error,((_tmp485.f1=((_tmp484="some action didn't return!",_tag_dyneither(_tmp484,sizeof(char),27))),_tmp485)))),_tmp483)))));};}
# 542
struct _tuple20*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL8F:
# 540 "buildlib.cyl"
{struct _dyneither_ptr*_tmp486;Cyc_current_source=((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp486))));}
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL90:
# 544
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{
# 547
struct Cyc_Lexing_Error_exn_struct _tmp48C;const char*_tmp48B;struct Cyc_Lexing_Error_exn_struct*_tmp48A;(int)_throw((void*)((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp48C.tag=Cyc_Lexing_Error,((_tmp48C.f1=((_tmp48B="some action didn't return!",_tag_dyneither(_tmp48B,sizeof(char),27))),_tmp48C)))),_tmp48A)))));};}
# 549
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL92:
# 547 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL93:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}{
# 551
struct Cyc_Lexing_Error_exn_struct _tmp492;const char*_tmp491;struct Cyc_Lexing_Error_exn_struct*_tmp490;(int)_throw((void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((_tmp492.tag=Cyc_Lexing_Error,((_tmp492.f1=((_tmp491="some action didn't return!",_tag_dyneither(_tmp491,sizeof(char),27))),_tmp492)))),_tmp490)))));};}
# 553
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple22*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL95:
# 554 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL96:
# 556
 Cyc_current_headerfile=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
Cyc_current_symbols=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple22*_tmp493;return(_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->f1=Cyc_current_headerfile,((_tmp493->f2=Cyc_current_symbols,((_tmp493->f3=Cyc_current_omit_symbols,((_tmp493->f4=Cyc_current_hstubs,((_tmp493->f5=Cyc_current_cstubs,((_tmp493->f6=Cyc_current_cycstubs,_tmp493)))))))))))));};case 2: _LL97:
# 576
 return Cyc_spec(lexbuf);case 3: _LL98:
# 578
 return 0;case 4: _LL99:
# 580
{const char*_tmp497;void*_tmp496[1];struct Cyc_Int_pa_PrintArg_struct _tmp495;(_tmp495.tag=1,((_tmp495.f1=(unsigned long)((int)
# 582
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp496[0]=& _tmp495,Cyc_fprintf(Cyc_stderr,((_tmp497="Error in .cys file: expected header file name, found '%c' instead\n",_tag_dyneither(_tmp497,sizeof(char),67))),_tag_dyneither(_tmp496,sizeof(void*),1)))))));}
return 0;default: _LL9A:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}{
# 587
struct Cyc_Lexing_Error_exn_struct _tmp49D;const char*_tmp49C;struct Cyc_Lexing_Error_exn_struct*_tmp49B;(int)_throw((void*)((_tmp49B=_cycalloc(sizeof(*_tmp49B)),((_tmp49B[0]=((_tmp49D.tag=Cyc_Lexing_Error,((_tmp49D.f1=((_tmp49C="some action didn't return!",_tag_dyneither(_tmp49C,sizeof(char),27))),_tmp49D)))),_tmp49B)))));};}
# 589
struct _tuple22*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL9C:
# 587 "buildlib.cyl"
 return 0;case 1: _LL9D:
# 589
 return 0;case 2: _LL9E:
# 591
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9F:
# 596
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;case 4: _LLA0:
# 601
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp49E;struct _tuple21*x=(_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp49E->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp49E)))));
{struct Cyc_List_List*_tmp49F;Cyc_current_hstubs=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F->hd=x,((_tmp49F->tl=Cyc_current_hstubs,_tmp49F))))));}
return 1;};case 5: _LLA1: {
# 609
struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4A0;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((_tmp4A0="hstub",_tag_dyneither(_tmp4A0,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp83;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp85=Cyc_substring((struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp4A1;struct _tuple21*x=(_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->f1=(struct _dyneither_ptr)_tmp85,((_tmp4A1->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4A1)))));
{struct Cyc_List_List*_tmp4A2;Cyc_current_hstubs=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->hd=x,((_tmp4A2->tl=Cyc_current_hstubs,_tmp4A2))))));}
return 1;};};};}case 6: _LLA2:
# 623
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp4A3;struct _tuple21*x=(_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4A3->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4A3)))));
{struct Cyc_List_List*_tmp4A4;Cyc_current_cstubs=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4->hd=x,((_tmp4A4->tl=Cyc_current_cstubs,_tmp4A4))))));}
return 1;};case 7: _LLA3: {
# 631
struct _dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4A5;_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((_tmp4A5="cstub",_tag_dyneither(_tmp4A5,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp8A;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp8C=Cyc_substring((struct _dyneither_ptr)_tmp8A,0,(unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp4A6;struct _tuple21*x=(_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6->f1=(struct _dyneither_ptr)_tmp8C,((_tmp4A6->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4A6)))));
{struct Cyc_List_List*_tmp4A7;Cyc_current_cstubs=((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7->hd=x,((_tmp4A7->tl=Cyc_current_cstubs,_tmp4A7))))));}
return 1;};};};}case 8: _LLA4:
# 645
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp4A8;struct _tuple21*x=(_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4A8->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4A8)))));
{struct Cyc_List_List*_tmp4A9;Cyc_current_cycstubs=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9->hd=x,((_tmp4A9->tl=Cyc_current_cycstubs,_tmp4A9))))));}
return 1;};case 9: _LLA5: {
# 653
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4AA;_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((_tmp4AA="cycstub",_tag_dyneither(_tmp4AA,sizeof(char),8)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp91,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp91;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp93=Cyc_substring((struct _dyneither_ptr)_tmp91,0,(unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple21*_tmp4AB;struct _tuple21*x=(_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB->f1=(struct _dyneither_ptr)_tmp93,((_tmp4AB->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4AB)))));
{struct Cyc_List_List*_tmp4AC;Cyc_current_cycstubs=((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC->hd=x,((_tmp4AC->tl=Cyc_current_cycstubs,_tmp4AC))))));}
return 1;};};};}case 10: _LLA6:
# 667
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*_tmp4AD;struct _dyneither_ptr*x=(_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4AD)));
{struct Cyc_List_List*_tmp4AE;Cyc_current_cpp=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->hd=x,((_tmp4AE->tl=Cyc_current_cpp,_tmp4AE))))));}
return 1;};case 11: _LLA7:
# 674
 return 1;case 12: _LLA8:
# 676
 return 1;case 13: _LLA9:
# 678
{const char*_tmp4B2;void*_tmp4B1[1];struct Cyc_Int_pa_PrintArg_struct _tmp4B0;(_tmp4B0.tag=1,((_tmp4B0.f1=(unsigned long)((int)
# 680
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4B1[0]=& _tmp4B0,Cyc_fprintf(Cyc_stderr,((_tmp4B2="Error in .cys file: expected command, found '%c' instead\n",_tag_dyneither(_tmp4B2,sizeof(char),58))),_tag_dyneither(_tmp4B1,sizeof(void*),1)))))));}
return 0;default: _LLAA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}{
# 685
struct Cyc_Lexing_Error_exn_struct _tmp4B8;const char*_tmp4B7;struct Cyc_Lexing_Error_exn_struct*_tmp4B6;(int)_throw((void*)((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6[0]=((_tmp4B8.tag=Cyc_Lexing_Error,((_tmp4B8.f1=((_tmp4B7="some action didn't return!",_tag_dyneither(_tmp4B7,sizeof(char),27))),_tmp4B8)))),_tmp4B6)))));};}
# 687
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAC:
# 690 "buildlib.cyl"
{struct _dyneither_ptr*_tmp4BB;struct Cyc_List_List*_tmp4BA;Cyc_snarfed_symbols=((_tmp4BA=_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA->hd=((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp4BB)))),((_tmp4BA->tl=Cyc_snarfed_symbols,_tmp4BA))))));}
return 1;case 1: _LLAD:
# 693
 return 1;case 2: _LLAE:
# 695
 return 0;case 3: _LLAF:
# 697
{const char*_tmp4BE;void*_tmp4BD;(_tmp4BD=0,Cyc_fprintf(Cyc_stderr,((_tmp4BE="Error in .cys file: unexpected end-of-file\n",_tag_dyneither(_tmp4BE,sizeof(char),44))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}
# 699
return 0;case 4: _LLB0:
# 701
{const char*_tmp4C2;void*_tmp4C1[1];struct Cyc_Int_pa_PrintArg_struct _tmp4C0;(_tmp4C0.tag=1,((_tmp4C0.f1=(unsigned long)((int)
# 703
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4C1[0]=& _tmp4C0,Cyc_fprintf(Cyc_stderr,((_tmp4C2="Error in .cys file: expected symbol, found '%c' instead\n",_tag_dyneither(_tmp4C2,sizeof(char),57))),_tag_dyneither(_tmp4C1,sizeof(void*),1)))))));}
return 0;default: _LLB1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}{
# 708
struct Cyc_Lexing_Error_exn_struct _tmp4C8;const char*_tmp4C7;struct Cyc_Lexing_Error_exn_struct*_tmp4C6;(int)_throw((void*)((_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4C8.tag=Cyc_Lexing_Error,((_tmp4C8.f1=((_tmp4C7="some action didn't return!",_tag_dyneither(_tmp4C7,sizeof(char),27))),_tmp4C8)))),_tmp4C6)))));};}
# 710
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLB3:
# 714 "buildlib.cyl"
{const char*_tmp4CB;void*_tmp4CA;(_tmp4CA=0,Cyc_log(((_tmp4CB="Warning: unclosed brace\n",_tag_dyneither(_tmp4CB,sizeof(char),25))),_tag_dyneither(_tmp4CA,sizeof(void*),0)));}return 0;case 1: _LLB4:
# 716
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LLB5:
# 721
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LLB6:
# 725
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB7:
# 729
{const char*_tmp4CC;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp4CC="/*",_tag_dyneither(_tmp4CC,sizeof(char),3))));}
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLB8:
# 733
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LLB9:
# 736
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLBA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}{
# 741
struct Cyc_Lexing_Error_exn_struct _tmp4D2;const char*_tmp4D1;struct Cyc_Lexing_Error_exn_struct*_tmp4D0;(int)_throw((void*)((_tmp4D0=_cycalloc(sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4D2.tag=Cyc_Lexing_Error,((_tmp4D2.f1=((_tmp4D1="some action didn't return!",_tag_dyneither(_tmp4D1,sizeof(char),27))),_tmp4D2)))),_tmp4D0)))));};}
# 743
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLBC:
# 741 "buildlib.cyl"
{const char*_tmp4D5;void*_tmp4D4;(_tmp4D4=0,Cyc_log(((_tmp4D5="Warning: unclosed string\n",_tag_dyneither(_tmp4D5,sizeof(char),26))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}return 0;case 1: _LLBD:
# 743
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBE:
# 745
{const char*_tmp4D8;void*_tmp4D7;(_tmp4D7=0,Cyc_log(((_tmp4D8="Warning: unclosed string\n",_tag_dyneither(_tmp4D8,sizeof(char),26))),_tag_dyneither(_tmp4D7,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LLBF:
# 749
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LLC0:
# 752
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LLC1:
# 755
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LLC2:
# 758
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LLC3:
# 761
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LLC4:
# 764
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}{
# 769
struct Cyc_Lexing_Error_exn_struct _tmp4DE;const char*_tmp4DD;struct Cyc_Lexing_Error_exn_struct*_tmp4DC;(int)_throw((void*)((_tmp4DC=_cycalloc(sizeof(*_tmp4DC)),((_tmp4DC[0]=((_tmp4DE.tag=Cyc_Lexing_Error,((_tmp4DE.f1=((_tmp4DD="some action didn't return!",_tag_dyneither(_tmp4DD,sizeof(char),27))),_tmp4DE)))),_tmp4DC)))));};}
# 771
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC7:
# 769 "buildlib.cyl"
{const char*_tmp4E1;void*_tmp4E0;(_tmp4E0=0,Cyc_log(((_tmp4E1="Warning: unclosed comment\n",_tag_dyneither(_tmp4E1,sizeof(char),27))),_tag_dyneither(_tmp4E0,sizeof(void*),0)));}return 0;case 1: _LLC8:
# 771
{const char*_tmp4E2;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp4E2="*/",_tag_dyneither(_tmp4E2,sizeof(char),3))));}return 0;case 2: _LLC9:
# 773
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLCA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}{
# 778
struct Cyc_Lexing_Error_exn_struct _tmp4E8;const char*_tmp4E7;struct Cyc_Lexing_Error_exn_struct*_tmp4E6;(int)_throw((void*)((_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6[0]=((_tmp4E8.tag=Cyc_Lexing_Error,((_tmp4E8.f1=((_tmp4E7="some action didn't return!",_tag_dyneither(_tmp4E7,sizeof(char),27))),_tmp4E8)))),_tmp4E6)))));};}
# 780
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 783 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 786
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _dyneither_ptr*_tmpBD;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_List_List*_tmpDD;void*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;int _tmpE2;struct Cyc_Absyn_Exp*_tmpE3;void**_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE7;void*_tmpE9;void*_tmpEB;struct Cyc_Absyn_Exp*_tmpED;struct _dyneither_ptr*_tmpEE;struct Cyc_Absyn_Exp*_tmpF0;struct _dyneither_ptr*_tmpF1;void*_tmpF3;void*_tmpF4;struct Cyc_List_List*_tmpF7;_LLCD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpBC->tag != 1)goto _LLCF;else{_tmpBD=(_tmpBC->f1)->f2;}}_LLCE:
# 790
 Cyc_add_target(_tmpBD);
return;_LLCF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpBE->tag != 2)goto _LLD1;else{_tmpBF=_tmpBE->f2;}}_LLD0:
# 793
 for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBF->hd,dep);}
# 796
return;_LLD1: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC0->tag != 22)goto _LLD3;else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;}}_LLD2:
# 798
 _tmpC4=_tmpC1;_tmpC5=_tmpC2;goto _LLD4;_LLD3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC3->tag != 8)goto _LLD5;else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC3->f2;}}_LLD4:
# 800
 _tmpC7=_tmpC4;_tmpC8=_tmpC5;goto _LLD6;_LLD5: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC6->tag != 3)goto _LLD7;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f3;}}_LLD6:
# 802
 Cyc_scan_exp(_tmpC7,dep);
Cyc_scan_exp(_tmpC8,dep);
return;_LLD7: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC9->tag != 19)goto _LLD9;else{_tmpCA=_tmpC9->f1;}}_LLD8:
# 806
 _tmpCC=_tmpCA;goto _LLDA;_LLD9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpCB->tag != 17)goto _LLDB;else{_tmpCC=_tmpCB->f1;}}_LLDA:
# 808
 _tmpCE=_tmpCC;goto _LLDC;_LLDB: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpCD=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpCD->tag != 14)goto _LLDD;else{_tmpCE=_tmpCD->f1;}}_LLDC:
# 810
 _tmpD0=_tmpCE;goto _LLDE;_LLDD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpCF=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpCF->tag != 4)goto _LLDF;else{_tmpD0=_tmpCF->f1;}}_LLDE:
# 812
 Cyc_scan_exp(_tmpD0,dep);
return;_LLDF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpD1->tag != 5)goto _LLE1;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_tmpD4=_tmpD1->f3;}}_LLE0:
# 815
 Cyc_scan_exp(_tmpD2,dep);
Cyc_scan_exp(_tmpD3,dep);
Cyc_scan_exp(_tmpD4,dep);
return;_LLE1: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpD5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpD5->tag != 6)goto _LLE3;else{_tmpD6=_tmpD5->f1;_tmpD7=_tmpD5->f2;}}_LLE2:
 _tmpD9=_tmpD6;_tmpDA=_tmpD7;goto _LLE4;_LLE3: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpD8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpD8->tag != 7)goto _LLE5;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;}}_LLE4:
# 821
 Cyc_scan_exp(_tmpD9,dep);
Cyc_scan_exp(_tmpDA,dep);
return;_LLE5: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpDB->tag != 9)goto _LLE7;else{_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;}}_LLE6:
# 825
 Cyc_scan_exp(_tmpDC,dep);
for(0;_tmpDD != 0;_tmpDD=_tmpDD->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDD->hd,dep);}
# 829
return;_LLE7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpDE->tag != 13)goto _LLE9;else{_tmpDF=(void*)_tmpDE->f1;_tmpE0=_tmpDE->f2;}}_LLE8:
# 831
 Cyc_scan_type(_tmpDF,dep);
Cyc_scan_exp(_tmpE0,dep);
return;_LLE9: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpE1->tag != 32)goto _LLEB;else{_tmpE2=(_tmpE1->f1).is_calloc;_tmpE3=(_tmpE1->f1).rgn;_tmpE4=(_tmpE1->f1).elt_type;_tmpE5=(_tmpE1->f1).num_elts;}}_LLEA:
# 835
 if(_tmpE3 != 0)Cyc_scan_exp(_tmpE3,dep);
if(_tmpE4 != 0)Cyc_scan_type(*_tmpE4,dep);
Cyc_scan_exp(_tmpE5,dep);
return;_LLEB: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpE6->tag != 36)goto _LLED;else{_tmpE7=_tmpE6->f1;}}_LLEC:
# 840
 Cyc_scan_exp(_tmpE7,dep);return;_LLED: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpE8->tag != 37)goto _LLEF;else{_tmpE9=(void*)_tmpE8->f1;}}_LLEE:
 _tmpEB=_tmpE9;goto _LLF0;_LLEF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpEA=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpEA->tag != 16)goto _LLF1;else{_tmpEB=(void*)_tmpEA->f1;}}_LLF0:
# 843
 Cyc_scan_type(_tmpEB,dep);
return;_LLF1: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpEC->tag != 20)goto _LLF3;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}}_LLF2:
# 846
 _tmpF0=_tmpED;_tmpF1=_tmpEE;goto _LLF4;_LLF3: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpEF->tag != 21)goto _LLF5;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}}_LLF4:
# 848
 Cyc_scan_exp(_tmpF0,dep);
Cyc_add_target(_tmpF1);
return;_LLF5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF2->tag != 18)goto _LLF7;else{_tmpF3=(void*)_tmpF2->f1;_tmpF4=(void*)_tmpF2->f2;}}_LLF6:
# 852
 Cyc_scan_type(_tmpF3,dep);
{void*_tmp108=_tmpF4;struct _dyneither_ptr*_tmp10A;_LL11C: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp109=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp108;if(_tmp109->tag != 0)goto _LL11E;else{_tmp10A=_tmp109->f1;}}_LL11D:
 Cyc_add_target(_tmp10A);goto _LL11B;_LL11E: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp10B=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp108;if(_tmp10B->tag != 1)goto _LL11B;}_LL11F:
 goto _LL11B;_LL11B:;}
# 857
return;_LLF7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF5->tag != 0)goto _LLF9;}_LLF8:
# 859
 return;_LLF9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF6->tag != 34)goto _LLFB;else{_tmpF7=_tmpF6->f2;}}_LLFA:
# 861
 for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){
struct Cyc_Absyn_Exp*_tmp10D;struct _tuple23*_tmp10C=(struct _tuple23*)_tmpF7->hd;_tmp10D=_tmp10C->f2;
Cyc_scan_exp(_tmp10D,dep);}
# 865
return;_LLFB: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF8->tag != 38)goto _LLFD;}_LLFC:
 return;_LLFD: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF9->tag != 33)goto _LLFF;}_LLFE:
# 868
{const char*_tmp4EB;void*_tmp4EA;(_tmp4EA=0,Cyc_fprintf(Cyc_stderr,((_tmp4EB="Error: unexpected Swap_e\n",_tag_dyneither(_tmp4EB,sizeof(char),26))),_tag_dyneither(_tmp4EA,sizeof(void*),0)));}
exit(1);return;_LLFF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFA->tag != 35)goto _LL101;}_LL100:
# 871
{const char*_tmp4EE;void*_tmp4ED;(_tmp4ED=0,Cyc_fprintf(Cyc_stderr,((_tmp4EE="Error: unexpected Stmt_e\n",_tag_dyneither(_tmp4EE,sizeof(char),26))),_tag_dyneither(_tmp4ED,sizeof(void*),0)));}
exit(1);return;_LL101: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFB->tag != 10)goto _LL103;}_LL102:
# 874
{const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,Cyc_fprintf(Cyc_stderr,((_tmp4F1="Error: unexpected Throw_e\n",_tag_dyneither(_tmp4F1,sizeof(char),27))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFC->tag != 11)goto _LL105;}_LL104:
# 877
{const char*_tmp4F4;void*_tmp4F3;(_tmp4F3=0,Cyc_fprintf(Cyc_stderr,((_tmp4F4="Error: unexpected NoInstantiate_e\n",_tag_dyneither(_tmp4F4,sizeof(char),35))),_tag_dyneither(_tmp4F3,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFD->tag != 12)goto _LL107;}_LL106:
# 880
{const char*_tmp4F7;void*_tmp4F6;(_tmp4F6=0,Cyc_fprintf(Cyc_stderr,((_tmp4F7="Error: unexpected Instantiate_e\n",_tag_dyneither(_tmp4F7,sizeof(char),33))),_tag_dyneither(_tmp4F6,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFE->tag != 15)goto _LL109;}_LL108:
# 883
{const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_fprintf(Cyc_stderr,((_tmp4FA="Error: unexpected New_e\n",_tag_dyneither(_tmp4FA,sizeof(char),25))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFF->tag != 23)goto _LL10B;}_LL10A:
# 886
{const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,Cyc_fprintf(Cyc_stderr,((_tmp4FD="Error: unexpected Tuple_e\n",_tag_dyneither(_tmp4FD,sizeof(char),27))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp100->tag != 24)goto _LL10D;}_LL10C:
# 889
{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_fprintf(Cyc_stderr,((_tmp500="Error: unexpected CompoundLit_e\n",_tag_dyneither(_tmp500,sizeof(char),33))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp101->tag != 25)goto _LL10F;}_LL10E:
# 892
{const char*_tmp503;void*_tmp502;(_tmp502=0,Cyc_fprintf(Cyc_stderr,((_tmp503="Error: unexpected Array_e\n",_tag_dyneither(_tmp503,sizeof(char),27))),_tag_dyneither(_tmp502,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp102->tag != 26)goto _LL111;}_LL110:
# 895
{const char*_tmp506;void*_tmp505;(_tmp505=0,Cyc_fprintf(Cyc_stderr,((_tmp506="Error: unexpected Comprehension_e\n",_tag_dyneither(_tmp506,sizeof(char),35))),_tag_dyneither(_tmp505,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp103->tag != 27)goto _LL113;}_LL112:
# 898
{const char*_tmp509;void*_tmp508;(_tmp508=0,Cyc_fprintf(Cyc_stderr,((_tmp509="Error: unexpected Aggregate_e\n",_tag_dyneither(_tmp509,sizeof(char),31))),_tag_dyneither(_tmp508,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp104->tag != 28)goto _LL115;}_LL114:
# 901
{const char*_tmp50C;void*_tmp50B;(_tmp50B=0,Cyc_fprintf(Cyc_stderr,((_tmp50C="Error: unexpected AnonStruct_e\n",_tag_dyneither(_tmp50C,sizeof(char),32))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp105=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp105->tag != 29)goto _LL117;}_LL116:
# 904
{const char*_tmp50F;void*_tmp50E;(_tmp50E=0,Cyc_fprintf(Cyc_stderr,((_tmp50F="Error: unexpected Datatype_e\n",_tag_dyneither(_tmp50F,sizeof(char),30))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp106->tag != 30)goto _LL119;}_LL118:
# 907
{const char*_tmp512;void*_tmp511;(_tmp511=0,Cyc_fprintf(Cyc_stderr,((_tmp512="Error: unexpected Enum_e\n",_tag_dyneither(_tmp512,sizeof(char),26))),_tag_dyneither(_tmp511,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp107->tag != 31)goto _LLCC;}_LL11A:
# 910
{const char*_tmp515;void*_tmp514;(_tmp514=0,Cyc_fprintf(Cyc_stderr,((_tmp515="Error: unexpected AnonEnum_e\n",_tag_dyneither(_tmp515,sizeof(char),30))),_tag_dyneither(_tmp514,sizeof(void*),0)));}
exit(1);return;_LLCC:;}
# 915
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 920
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp12C=t;struct Cyc_Absyn_PtrInfo _tmp131;void*_tmp133;struct Cyc_Absyn_Exp*_tmp134;union Cyc_Absyn_Constraint*_tmp135;struct Cyc_Absyn_FnInfo _tmp137;struct Cyc_List_List*_tmp139;union Cyc_Absyn_AggrInfoU _tmp13C;struct _dyneither_ptr*_tmp13E;struct _dyneither_ptr*_tmp140;struct Cyc_Absyn_Aggrdecl*_tmp143;struct Cyc_Absyn_Enumdecl*_tmp146;struct Cyc_Absyn_Datatypedecl*_tmp149;_LL121: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp12D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp12C;if(_tmp12D->tag != 0)goto _LL123;}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp12E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp12C;if(_tmp12E->tag != 6)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp12F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp12C;if(_tmp12F->tag != 7)goto _LL127;}_LL126:
# 926
 return;_LL127: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp130=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C;if(_tmp130->tag != 5)goto _LL129;else{_tmp131=_tmp130->f1;}}_LL128:
# 929
 Cyc_scan_type(_tmp131.elt_typ,dep);
return;_LL129: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp132=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C;if(_tmp132->tag != 8)goto _LL12B;else{_tmp133=(_tmp132->f1).elt_type;_tmp134=(_tmp132->f1).num_elts;_tmp135=(_tmp132->f1).zero_term;}}_LL12A:
# 932
 Cyc_scan_type(_tmp133,dep);
Cyc_scan_exp_opt(_tmp134,dep);
return;_LL12B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp136=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12C;if(_tmp136->tag != 9)goto _LL12D;else{_tmp137=_tmp136->f1;}}_LL12C:
# 936
 Cyc_scan_type(_tmp137.ret_typ,dep);
{struct Cyc_List_List*_tmp159=_tmp137.args;for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
void*_tmp15B;struct _tuple8*_tmp15A=(struct _tuple8*)_tmp159->hd;_tmp15B=_tmp15A->f3;
Cyc_scan_type(_tmp15B,dep);}}
# 941
if(_tmp137.cyc_varargs != 0)
Cyc_scan_type((_tmp137.cyc_varargs)->type,dep);
return;_LL12D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp138=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C;if(_tmp138->tag != 12)goto _LL12F;else{_tmp139=_tmp138->f2;}}_LL12E:
# 945
 for(0;_tmp139 != 0;_tmp139=_tmp139->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp139->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp139->hd)->width,dep);}
# 949
return;_LL12F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp12C;if(_tmp13A->tag != 14)goto _LL131;}_LL130:
# 951
 return;_LL131: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12C;if(_tmp13B->tag != 11)goto _LL133;else{_tmp13C=(_tmp13B->f1).aggr_info;}}_LL132: {
# 953
struct _dyneither_ptr*_tmp15D;struct _tuple10 _tmp15C=Cyc_Absyn_aggr_kinded_name(_tmp13C);_tmp15D=(_tmp15C.f2)->f2;
_tmp13E=_tmp15D;goto _LL134;}_LL133: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp12C;if(_tmp13D->tag != 13)goto _LL135;else{_tmp13E=(_tmp13D->f1)->f2;}}_LL134:
# 956
 _tmp140=_tmp13E;goto _LL136;_LL135: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12C;if(_tmp13F->tag != 17)goto _LL137;else{_tmp140=(_tmp13F->f1)->f2;}}_LL136:
# 958
 Cyc_add_target(_tmp140);
return;_LL137: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C;if(_tmp141->tag != 26)goto _LL139;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp142=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp141->f1)->r;if(_tmp142->tag != 0)goto _LL139;else{_tmp143=_tmp142->f1;}}}_LL138:
# 962
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp518;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp517;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp517=_cycalloc(sizeof(*_tmp517)),((_tmp517[0]=((_tmp518.tag=6,((_tmp518.f1=_tmp143,_tmp518)))),_tmp517)))),0),dep);}{
struct _dyneither_ptr*_tmp161;struct _tuple0*_tmp160=_tmp143->name;_tmp161=_tmp160->f2;
Cyc_add_target(_tmp161);
return;};_LL139: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp144=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C;if(_tmp144->tag != 26)goto _LL13B;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp145=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp144->f1)->r;if(_tmp145->tag != 1)goto _LL13B;else{_tmp146=_tmp145->f1;}}}_LL13A:
# 968
{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp51B;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp51A;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp51A=_cycalloc(sizeof(*_tmp51A)),((_tmp51A[0]=((_tmp51B.tag=8,((_tmp51B.f1=_tmp146,_tmp51B)))),_tmp51A)))),0),dep);}{
struct _dyneither_ptr*_tmp165;struct _tuple0*_tmp164=_tmp146->name;_tmp165=_tmp164->f2;
Cyc_add_target(_tmp165);
return;};_LL13B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C;if(_tmp147->tag != 26)goto _LL13D;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp148=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp147->f1)->r;if(_tmp148->tag != 2)goto _LL13D;else{_tmp149=_tmp148->f1;}}}_LL13C:
# 974
{const char*_tmp51E;void*_tmp51D;(_tmp51D=0,Cyc_fprintf(Cyc_stderr,((_tmp51E="Error: unexpected Datatype declaration\n",_tag_dyneither(_tmp51E,sizeof(char),40))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}
exit(1);return;_LL13D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12C;if(_tmp14A->tag != 1)goto _LL13F;}_LL13E:
# 977
{const char*_tmp521;void*_tmp520;(_tmp520=0,Cyc_fprintf(Cyc_stderr,((_tmp521="Error: unexpected Evar\n",_tag_dyneither(_tmp521,sizeof(char),24))),_tag_dyneither(_tmp520,sizeof(void*),0)));}
exit(1);return;_LL13F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12C;if(_tmp14B->tag != 2)goto _LL141;}_LL140:
# 980
{const char*_tmp524;void*_tmp523;(_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Error: unexpected VarType\n",_tag_dyneither(_tmp524,sizeof(char),27))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
exit(1);return;_LL141: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp12C;if(_tmp14C->tag != 3)goto _LL143;}_LL142:
# 983
{const char*_tmp527;void*_tmp526;(_tmp526=0,Cyc_fprintf(Cyc_stderr,((_tmp527="Error: unexpected DatatypeType\n",_tag_dyneither(_tmp527,sizeof(char),32))),_tag_dyneither(_tmp526,sizeof(void*),0)));}
exit(1);return;_LL143: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp14D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp12C;if(_tmp14D->tag != 4)goto _LL145;}_LL144:
# 986
{const char*_tmp52A;void*_tmp529;(_tmp529=0,Cyc_fprintf(Cyc_stderr,((_tmp52A="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp52A,sizeof(char),37))),_tag_dyneither(_tmp529,sizeof(void*),0)));}
exit(1);return;_LL145: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12C;if(_tmp14E->tag != 10)goto _LL147;}_LL146:
# 989
{const char*_tmp52D;void*_tmp52C;(_tmp52C=0,Cyc_fprintf(Cyc_stderr,((_tmp52D="Error: unexpected TupleType\n",_tag_dyneither(_tmp52D,sizeof(char),29))),_tag_dyneither(_tmp52C,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp12C;if(_tmp14F->tag != 15)goto _LL149;}_LL148:
# 992
{const char*_tmp530;void*_tmp52F;(_tmp52F=0,Cyc_fprintf(Cyc_stderr,((_tmp530="Error: unexpected RgnHandleType\n",_tag_dyneither(_tmp530,sizeof(char),33))),_tag_dyneither(_tmp52F,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp12C;if(_tmp150->tag != 16)goto _LL14B;}_LL14A:
# 995
{const char*_tmp533;void*_tmp532;(_tmp532=0,Cyc_fprintf(Cyc_stderr,((_tmp533="Error: unexpected DynRgnType\n",_tag_dyneither(_tmp533,sizeof(char),30))),_tag_dyneither(_tmp532,sizeof(void*),0)));}
exit(1);return;_LL14B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12C;if(_tmp151->tag != 20)goto _LL14D;}_LL14C:
# 998
{const char*_tmp536;void*_tmp535;(_tmp535=0,Cyc_fprintf(Cyc_stderr,((_tmp536="Error: unexpected HeapRgn\n",_tag_dyneither(_tmp536,sizeof(char),27))),_tag_dyneither(_tmp535,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp12C;if(_tmp152->tag != 21)goto _LL14F;}_LL14E:
# 1001
{const char*_tmp539;void*_tmp538;(_tmp538=0,Cyc_fprintf(Cyc_stderr,((_tmp539="Error: unexpected UniqueRgn\n",_tag_dyneither(_tmp539,sizeof(char),29))),_tag_dyneither(_tmp538,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp12C;if(_tmp153->tag != 22)goto _LL151;}_LL150:
# 1004
{const char*_tmp53C;void*_tmp53B;(_tmp53B=0,Cyc_fprintf(Cyc_stderr,((_tmp53C="Error: unexpected RefCntRgn\n",_tag_dyneither(_tmp53C,sizeof(char),29))),_tag_dyneither(_tmp53B,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp154=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp12C;if(_tmp154->tag != 23)goto _LL153;}_LL152:
# 1007
{const char*_tmp53F;void*_tmp53E;(_tmp53E=0,Cyc_fprintf(Cyc_stderr,((_tmp53F="Error: unexpected AccessEff\n",_tag_dyneither(_tmp53F,sizeof(char),29))),_tag_dyneither(_tmp53E,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp12C;if(_tmp155->tag != 24)goto _LL155;}_LL154:
# 1010
{const char*_tmp542;void*_tmp541;(_tmp541=0,Cyc_fprintf(Cyc_stderr,((_tmp542="Error: unexpected JoinEff\n",_tag_dyneither(_tmp542,sizeof(char),27))),_tag_dyneither(_tmp541,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp156=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp12C;if(_tmp156->tag != 25)goto _LL157;}_LL156:
# 1013
{const char*_tmp545;void*_tmp544;(_tmp544=0,Cyc_fprintf(Cyc_stderr,((_tmp545="Error: unexpected RgnsEff\n",_tag_dyneither(_tmp545,sizeof(char),27))),_tag_dyneither(_tmp544,sizeof(void*),0)));}
exit(1);return;_LL157: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp12C;if(_tmp157->tag != 19)goto _LL159;}_LL158:
# 1016
{const char*_tmp548;void*_tmp547;(_tmp547=0,Cyc_fprintf(Cyc_stderr,((_tmp548="Error: unexpected tag_t\n",_tag_dyneither(_tmp548,sizeof(char),25))),_tag_dyneither(_tmp547,sizeof(void*),0)));}
exit(1);return;_LL159: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp12C;if(_tmp158->tag != 18)goto _LL120;}_LL15A:
# 1019
{const char*_tmp54B;void*_tmp54A;(_tmp54A=0,Cyc_fprintf(Cyc_stderr,((_tmp54B="Error: unexpected valueof_t\n",_tag_dyneither(_tmp54B,sizeof(char),29))),_tag_dyneither(_tmp54A,sizeof(void*),0)));}
exit(1);return;_LL120:;}
# 1024
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp186=Cyc_current_targets;
struct _dyneither_ptr*_tmp187=Cyc_current_source;
{struct Cyc_Set_Set**_tmp54C;Cyc_current_targets=((_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp54C))));}
{void*_tmp189=d->r;struct Cyc_Absyn_Vardecl*_tmp18B;struct Cyc_Absyn_Fndecl*_tmp18D;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp191;struct Cyc_Absyn_Typedefdecl*_tmp193;_LL15C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp18A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp18A->tag != 0)goto _LL15E;else{_tmp18B=_tmp18A->f1;}}_LL15D: {
# 1030
struct _dyneither_ptr*_tmp1A0;struct _tuple0*_tmp19F=_tmp18B->name;_tmp1A0=_tmp19F->f2;
Cyc_current_source=_tmp1A0;
Cyc_scan_type(_tmp18B->type,dep);
Cyc_scan_exp_opt(_tmp18B->initializer,dep);
goto _LL15B;}_LL15E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp18C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp18C->tag != 1)goto _LL160;else{_tmp18D=_tmp18C->f1;}}_LL15F: {
# 1036
struct _dyneither_ptr*_tmp1A2;struct _tuple0*_tmp1A1=_tmp18D->name;_tmp1A2=_tmp1A1->f2;
Cyc_current_source=_tmp1A2;
Cyc_scan_type(_tmp18D->ret_type,dep);
{struct Cyc_List_List*_tmp1A3=_tmp18D->args;for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
void*_tmp1A5;struct _tuple8*_tmp1A4=(struct _tuple8*)_tmp1A3->hd;_tmp1A5=_tmp1A4->f3;
Cyc_scan_type(_tmp1A5,dep);}}
# 1043
if(_tmp18D->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp18D->cyc_varargs))->type,dep);
if(_tmp18D->is_inline){
const char*_tmp54F;void*_tmp54E;(_tmp54E=0,Cyc_fprintf(Cyc_stderr,((_tmp54F="Warning: ignoring inline function\n",_tag_dyneither(_tmp54F,sizeof(char),35))),_tag_dyneither(_tmp54E,sizeof(void*),0)));}
# 1048
goto _LL15B;}_LL160: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp18E=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp18E->tag != 6)goto _LL162;else{_tmp18F=_tmp18E->f1;}}_LL161: {
# 1050
struct _dyneither_ptr*_tmp1A9;struct _tuple0*_tmp1A8=_tmp18F->name;_tmp1A9=_tmp1A8->f2;
Cyc_current_source=_tmp1A9;
if((unsigned int)_tmp18F->impl){
{struct Cyc_List_List*_tmp1AA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18F->impl))->fields;for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){
struct Cyc_Absyn_Aggrfield*_tmp1AB=(struct Cyc_Absyn_Aggrfield*)_tmp1AA->hd;
Cyc_scan_type(_tmp1AB->type,dep);
Cyc_scan_exp_opt(_tmp1AB->width,dep);}}{
# 1060
struct Cyc_List_List*_tmp1AC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18F->impl))->fields;for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){;}};}
# 1064
goto _LL15B;}_LL162: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp190=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp190->tag != 8)goto _LL164;else{_tmp191=_tmp190->f1;}}_LL163: {
# 1066
struct _dyneither_ptr*_tmp1AE;struct _tuple0*_tmp1AD=_tmp191->name;_tmp1AE=_tmp1AD->f2;
Cyc_current_source=_tmp1AE;
if((unsigned int)_tmp191->fields){
{struct Cyc_List_List*_tmp1AF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp191->fields))->v;for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){
struct Cyc_Absyn_Enumfield*_tmp1B0=(struct Cyc_Absyn_Enumfield*)_tmp1AF->hd;
Cyc_scan_exp_opt(_tmp1B0->tag,dep);}}{
# 1075
struct Cyc_List_List*_tmp1B1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp191->fields))->v;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){;}};}
# 1079
goto _LL15B;}_LL164: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp192=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp192->tag != 9)goto _LL166;else{_tmp193=_tmp192->f1;}}_LL165: {
# 1081
struct _dyneither_ptr*_tmp1B3;struct _tuple0*_tmp1B2=_tmp193->name;_tmp1B3=_tmp1B2->f2;
Cyc_current_source=_tmp1B3;
if((unsigned int)_tmp193->defn)
Cyc_scan_type((void*)_check_null(_tmp193->defn),dep);
goto _LL15B;}_LL166: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp194=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp194->tag != 4)goto _LL168;}_LL167:
# 1087
{const char*_tmp552;void*_tmp551;(_tmp551=0,Cyc_fprintf(Cyc_stderr,((_tmp552="Error: unexpected region declaration",_tag_dyneither(_tmp552,sizeof(char),37))),_tag_dyneither(_tmp551,sizeof(void*),0)));}
exit(1);_LL168: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp195=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp195->tag != 5)goto _LL16A;}_LL169:
# 1090
{const char*_tmp555;void*_tmp554;(_tmp554=0,Cyc_fprintf(Cyc_stderr,((_tmp555="Error: unexpected alias declaration",_tag_dyneither(_tmp555,sizeof(char),36))),_tag_dyneither(_tmp554,sizeof(void*),0)));}
exit(1);_LL16A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp196=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp196->tag != 14)goto _LL16C;}_LL16B:
# 1093
{const char*_tmp558;void*_tmp557;(_tmp557=0,Cyc_fprintf(Cyc_stderr,((_tmp558="Error: unexpected __cyclone_port_on__",_tag_dyneither(_tmp558,sizeof(char),38))),_tag_dyneither(_tmp557,sizeof(void*),0)));}
exit(1);_LL16C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp197=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp197->tag != 15)goto _LL16E;}_LL16D:
# 1096
{const char*_tmp55B;void*_tmp55A;(_tmp55A=0,Cyc_fprintf(Cyc_stderr,((_tmp55B="Error: unexpected __cyclone_port_off__",_tag_dyneither(_tmp55B,sizeof(char),39))),_tag_dyneither(_tmp55A,sizeof(void*),0)));}
exit(1);_LL16E: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp198=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp198->tag != 2)goto _LL170;}_LL16F:
# 1099
{const char*_tmp55E;void*_tmp55D;(_tmp55D=0,Cyc_fprintf(Cyc_stderr,((_tmp55E="Error: unexpected let declaration\n",_tag_dyneither(_tmp55E,sizeof(char),35))),_tag_dyneither(_tmp55D,sizeof(void*),0)));}
exit(1);_LL170: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp199=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp199->tag != 7)goto _LL172;}_LL171:
# 1102
{const char*_tmp561;void*_tmp560;(_tmp560=0,Cyc_fprintf(Cyc_stderr,((_tmp561="Error: unexpected datatype declaration\n",_tag_dyneither(_tmp561,sizeof(char),40))),_tag_dyneither(_tmp560,sizeof(void*),0)));}
exit(1);_LL172: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp19A=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp19A->tag != 3)goto _LL174;}_LL173:
# 1105
{const char*_tmp564;void*_tmp563;(_tmp563=0,Cyc_fprintf(Cyc_stderr,((_tmp564="Error: unexpected let declaration\n",_tag_dyneither(_tmp564,sizeof(char),35))),_tag_dyneither(_tmp563,sizeof(void*),0)));}
exit(1);_LL174: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp19B=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp19B->tag != 10)goto _LL176;}_LL175:
# 1108
{const char*_tmp567;void*_tmp566;(_tmp566=0,Cyc_fprintf(Cyc_stderr,((_tmp567="Error: unexpected namespace declaration\n",_tag_dyneither(_tmp567,sizeof(char),41))),_tag_dyneither(_tmp566,sizeof(void*),0)));}
exit(1);_LL176: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp19C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp19C->tag != 11)goto _LL178;}_LL177:
# 1111
{const char*_tmp56A;void*_tmp569;(_tmp569=0,Cyc_fprintf(Cyc_stderr,((_tmp56A="Error: unexpected using declaration\n",_tag_dyneither(_tmp56A,sizeof(char),37))),_tag_dyneither(_tmp569,sizeof(void*),0)));}
exit(1);_LL178: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp19D=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp19D->tag != 12)goto _LL17A;}_LL179:
# 1114
{const char*_tmp56D;void*_tmp56C;(_tmp56C=0,Cyc_fprintf(Cyc_stderr,((_tmp56D="Error: unexpected extern \"C\" declaration\n",_tag_dyneither(_tmp56D,sizeof(char),42))),_tag_dyneither(_tmp56C,sizeof(void*),0)));}
exit(1);_LL17A: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp19E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp189;if(_tmp19E->tag != 13)goto _LL15B;}_LL17B:
# 1117
{const char*_tmp570;void*_tmp56F;(_tmp56F=0,Cyc_fprintf(Cyc_stderr,((_tmp570="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(_tmp570,sizeof(char),50))),_tag_dyneither(_tmp56F,sizeof(void*),0)));}
exit(1);_LL15B:;}{
# 1125
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp1CA=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp1CB;_push_handler(& _tmp1CB);{int _tmp1CD=0;if(setjmp(_tmp1CB.handler))_tmp1CD=1;if(!_tmp1CD){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp1CA);;_pop_handler();}else{void*_tmp1CC=(void*)_exn_thrown;void*_tmp1CF=_tmp1CC;void*_tmp1D1;_LL17D: {struct Cyc_Core_Not_found_exn_struct*_tmp1D0=(struct Cyc_Core_Not_found_exn_struct*)_tmp1CF;if(_tmp1D0->tag != Cyc_Core_Not_found)goto _LL17F;}_LL17E:
# 1130
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17C;_LL17F: _tmp1D1=_tmp1CF;_LL180:(void)_throw(_tmp1D1);_LL17C:;}};}{
# 1132
struct Cyc_Set_Set*_tmp1D2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp1CA,_tmp1D2);
# 1135
Cyc_current_targets=_tmp186;
Cyc_current_source=_tmp187;};};}
# 1139
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1143
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp1D3;_push_handler(& _tmp1D3);{int _tmp1D5=0;if(setjmp(_tmp1D3.handler))_tmp1D5=1;if(!_tmp1D5){{struct Cyc_Set_Set*_tmp1D6=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp1D6;};_pop_handler();}else{void*_tmp1D4=(void*)_exn_thrown;void*_tmp1D8=_tmp1D4;void*_tmp1DA;_LL182: {struct Cyc_Core_Not_found_exn_struct*_tmp1D9=(struct Cyc_Core_Not_found_exn_struct*)_tmp1D8;if(_tmp1D9->tag != Cyc_Core_Not_found)goto _LL184;}_LL183:
# 1146
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL184: _tmp1DA=_tmp1D8;_LL185:(void)_throw(_tmp1DA);_LL181:;}};}
# 1150
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1161 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1DB=curr;
# 1167
struct _dyneither_ptr*_tmp1DC=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1DB)> 0){
struct Cyc_Set_Set*_tmp1DD=emptyset;
struct Cyc_Iter_Iter _tmp1DE=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1DB);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1DE,& _tmp1DC)){
_tmp1DD=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1DD,Cyc_find(t,_tmp1DC));}
_tmp1DB=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1DD,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1DB);}
# 1176
return curr;};}
# 1179
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1185
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1189
if(Cyc_script_file == 0){
{const char*_tmp573;void*_tmp572;(_tmp572=0,Cyc_fprintf(Cyc_stderr,((_tmp573="Internal error: script file is NULL\n",_tag_dyneither(_tmp573,sizeof(char),37))),_tag_dyneither(_tmp572,sizeof(void*),0)));}
exit(1);}
# 1193
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1196
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT){
const char*_tmp578;void*_tmp577[2];struct Cyc_String_pa_PrintArg_struct _tmp576;struct Cyc_String_pa_PrintArg_struct _tmp575;(_tmp575.tag=0,((_tmp575.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp576.tag=0,((_tmp576.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp577[0]=& _tmp576,((_tmp577[1]=& _tmp575,Cyc_prscript(((_tmp578="if ! test -e %s; then mkdir %s; fi\n",_tag_dyneither(_tmp578,sizeof(char),36))),_tag_dyneither(_tmp577,sizeof(void*),2)))))))))))));}else{
# 1203
unsigned short _tmp579[0];int _tmp1E6=Cyc_open((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp579,sizeof(unsigned short),0));
if(_tmp1E6 == - 1){
if(mkdir((const char*)_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){
{const char*_tmp57D;void*_tmp57C[1];struct Cyc_String_pa_PrintArg_struct _tmp57B;(_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp57C[0]=& _tmp57B,Cyc_fprintf(Cyc_stderr,((_tmp57D="Error: could not create directory %s\n",_tag_dyneither(_tmp57D,sizeof(char),38))),_tag_dyneither(_tmp57C,sizeof(void*),1)))))));}
return 1;}}else{
# 1210
close(_tmp1E6);}}
# 1212
return 0;}
# 1215
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1219
struct _dyneither_ptr _tmp1EB=Cyc_strdup((struct _dyneither_ptr)file);
# 1221
struct Cyc_List_List*_tmp1EC=0;
while(1){
_tmp1EB=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1EB);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1EB)== 0)break;{
struct _dyneither_ptr*_tmp580;struct Cyc_List_List*_tmp57F;_tmp1EC=((_tmp57F=_cycalloc(sizeof(*_tmp57F)),((_tmp57F->hd=((_tmp580=_cycalloc(sizeof(*_tmp580)),((_tmp580[0]=(struct _dyneither_ptr)_tmp1EB,_tmp580)))),((_tmp57F->tl=_tmp1EC,_tmp57F))))));};}
# 1228
for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1EC->hd)))return 1;}
# 1231
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1238
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL186:
 goto _LL187;case '"': _LL187:
 goto _LL188;case ';': _LL188:
 goto _LL189;case '&': _LL189:
 goto _LL18A;case '(': _LL18A:
 goto _LL18B;case ')': _LL18B:
 goto _LL18C;case '|': _LL18C:
 goto _LL18D;case '^': _LL18D:
 goto _LL18E;case '<': _LL18E:
 goto _LL18F;case '>': _LL18F:
 goto _LL190;case '\n': _LL190:
# 1253
 goto _LL191;case '\t': _LL191:
 return 1;default: _LL192:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp589(unsigned int*_tmp588,unsigned int*_tmp587,char**_tmp585){for(*_tmp588=0;*_tmp588 < *_tmp587;(*_tmp588)++){(*_tmp585)[*_tmp588]='\000';}}
# 1259
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1EF=Cyc_strlen((struct _dyneither_ptr)s);
# 1263
int _tmp1F0=0;
int _tmp1F1=0;
{int i=0;for(0;i < _tmp1EF;++ i){
char _tmp1F2=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp1F2 == '\'')++ _tmp1F0;else{
if(Cyc_is_other_special(_tmp1F2))++ _tmp1F1;}}}
# 1272
if(_tmp1F0 == 0  && _tmp1F1 == 0)
return s;
# 1276
if(_tmp1F0 == 0){
struct _dyneither_ptr*_tmp583;struct _dyneither_ptr*_tmp582[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp582[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp582[1]=((_tmp583=_cycalloc(sizeof(*_tmp583)),((_tmp583[0]=(struct _dyneither_ptr)s,_tmp583)))),((_tmp582[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp582,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 1280
unsigned long _tmp1F7=(_tmp1EF + _tmp1F0)+ _tmp1F1;
unsigned int _tmp588;unsigned int _tmp587;struct _dyneither_ptr _tmp586;char*_tmp585;unsigned int _tmp584;struct _dyneither_ptr s2=(_tmp584=_tmp1F7 + 1,((_tmp585=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp584 + 1)),((_tmp586=_tag_dyneither(_tmp585,sizeof(char),_tmp584 + 1),((((_tmp587=_tmp584,((_tmp589(& _tmp588,& _tmp587,& _tmp585),_tmp585[_tmp587]=(char)0)))),_tmp586)))))));
int _tmp1F8=0;
int _tmp1F9=0;
for(0;_tmp1F8 < _tmp1EF;++ _tmp1F8){
char _tmp1FA=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp1F8));
if(_tmp1FA == '\''  || Cyc_is_other_special(_tmp1FA)){
char _tmp58C;char _tmp58B;struct _dyneither_ptr _tmp58A;(_tmp58A=_dyneither_ptr_plus(s2,sizeof(char),_tmp1F9 ++),((_tmp58B=*((char*)_check_dyneither_subscript(_tmp58A,sizeof(char),0)),((_tmp58C='\\',((_get_dyneither_size(_tmp58A,sizeof(char))== 1  && (_tmp58B == '\000'  && _tmp58C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp58A.curr)=_tmp58C)))))));}{
char _tmp58F;char _tmp58E;struct _dyneither_ptr _tmp58D;(_tmp58D=_dyneither_ptr_plus(s2,sizeof(char),_tmp1F9 ++),((_tmp58E=*((char*)_check_dyneither_subscript(_tmp58D,sizeof(char),0)),((_tmp58F=_tmp1FA,((_get_dyneither_size(_tmp58D,sizeof(char))== 1  && (_tmp58E == '\000'  && _tmp58F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp58D.curr)=_tmp58F)))))));};}
# 1290
return(struct _dyneither_ptr)s2;};}
# 1292
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp590;return(_tmp590=_cycalloc(sizeof(*_tmp590)),((_tmp590[0]=Cyc_sh_escape_string(*sp),_tmp590)));}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1297
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1303
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1308
{const char*_tmp596;void*_tmp595[1];const char*_tmp594;struct Cyc_String_pa_PrintArg_struct _tmp593;(_tmp593.tag=0,((_tmp593.f1=(struct _dyneither_ptr)(
(_tmp594=filename,_tag_dyneither(_tmp594,sizeof(char),_get_zero_arr_size_char((void*)_tmp594,1)))),((_tmp595[0]=& _tmp593,Cyc_fprintf(Cyc_stderr,((_tmp596="********************************* %s...\n",_tag_dyneither(_tmp596,sizeof(char),41))),_tag_dyneither(_tmp595,sizeof(void*),1)))))));}
# 1311
if(!Cyc_gathering()){const char*_tmp59C;void*_tmp59B[1];const char*_tmp59A;struct Cyc_String_pa_PrintArg_struct _tmp599;(_tmp599.tag=0,((_tmp599.f1=(struct _dyneither_ptr)((_tmp59A=filename,_tag_dyneither(_tmp59A,sizeof(char),_get_zero_arr_size_char((void*)_tmp59A,1)))),((_tmp59B[0]=& _tmp599,Cyc_log(((_tmp59C="\n%s:\n",_tag_dyneither(_tmp59C,sizeof(char),6))),_tag_dyneither(_tmp59B,sizeof(void*),1)))))));}{
# 1323 "buildlib.cyl"
const char*_tmp59D;struct _dyneither_ptr _tmp20E=Cyc_Filename_basename(((_tmp59D=filename,_tag_dyneither(_tmp59D,sizeof(char),_get_zero_arr_size_char((void*)_tmp59D,1)))));
const char*_tmp59E;struct _dyneither_ptr _tmp20F=Cyc_Filename_dirname(((_tmp59E=filename,_tag_dyneither(_tmp59E,sizeof(char),_get_zero_arr_size_char((void*)_tmp59E,1)))));
struct _dyneither_ptr _tmp210=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp20E);
const char*_tmp59F;const char*_tmp211=(const char*)_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp210,((_tmp59F=".iA",_tag_dyneither(_tmp59F,sizeof(char),4)))),sizeof(char),1);
const char*_tmp5A7;void*_tmp5A6[1];struct Cyc_String_pa_PrintArg_struct _tmp5A5;const char*_tmp5A3;void*_tmp5A2[1];struct Cyc_String_pa_PrintArg_struct _tmp5A1;const char*_tmp212=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp20F,sizeof(char))== 0?(_tmp5A5.tag=0,((_tmp5A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5A6[0]=& _tmp5A5,Cyc_aprintf(((_tmp5A7="%s.iB",_tag_dyneither(_tmp5A7,sizeof(char),6))),_tag_dyneither(_tmp5A6,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp20F,(struct _dyneither_ptr)((_tmp5A1.tag=0,((_tmp5A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5A2[0]=& _tmp5A1,Cyc_aprintf(((_tmp5A3="%s.iB",_tag_dyneither(_tmp5A3,sizeof(char),6))),_tag_dyneither(_tmp5A2,sizeof(void*),1))))))))),sizeof(char),1);
# 1331
const char*_tmp5AF;void*_tmp5AE[1];struct Cyc_String_pa_PrintArg_struct _tmp5AD;const char*_tmp5AB;void*_tmp5AA[1];struct Cyc_String_pa_PrintArg_struct _tmp5A9;const char*_tmp213=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp20F,sizeof(char))== 0?(_tmp5AD.tag=0,((_tmp5AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5AE[0]=& _tmp5AD,Cyc_aprintf(((_tmp5AF="%s.iC",_tag_dyneither(_tmp5AF,sizeof(char),6))),_tag_dyneither(_tmp5AE,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp20F,(struct _dyneither_ptr)((_tmp5A9.tag=0,((_tmp5A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5AA[0]=& _tmp5A9,Cyc_aprintf(((_tmp5AB="%s.iC",_tag_dyneither(_tmp5AB,sizeof(char),6))),_tag_dyneither(_tmp5AA,sizeof(void*),1))))))))),sizeof(char),1);
# 1335
const char*_tmp5B7;void*_tmp5B6[1];struct Cyc_String_pa_PrintArg_struct _tmp5B5;const char*_tmp5B3;void*_tmp5B2[1];struct Cyc_String_pa_PrintArg_struct _tmp5B1;const char*_tmp214=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp20F,sizeof(char))== 0?(_tmp5B5.tag=0,((_tmp5B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5B6[0]=& _tmp5B5,Cyc_aprintf(((_tmp5B7="%s.iD",_tag_dyneither(_tmp5B7,sizeof(char),6))),_tag_dyneither(_tmp5B6,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp20F,(struct _dyneither_ptr)((_tmp5B1.tag=0,((_tmp5B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210),((_tmp5B2[0]=& _tmp5B1,Cyc_aprintf(((_tmp5B3="%s.iD",_tag_dyneither(_tmp5B3,sizeof(char),6))),_tag_dyneither(_tmp5B2,sizeof(void*),1))))))))),sizeof(char),1);
# 1340
{struct _handler_cons _tmp215;_push_handler(& _tmp215);{int _tmp217=0;if(setjmp(_tmp215.handler))_tmp217=1;if(!_tmp217){
# 1343
{const char*_tmp5B8;if(Cyc_force_directory_prefixes(((_tmp5B8=filename,_tag_dyneither(_tmp5B8,sizeof(char),_get_zero_arr_size_char((void*)_tmp5B8,1)))))){
int _tmp219=1;_npop_handler(0);return _tmp219;}}
# 1348
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp5BE;void*_tmp5BD[1];const char*_tmp5BC;struct Cyc_String_pa_PrintArg_struct _tmp5BB;(_tmp5BB.tag=0,((_tmp5BB.f1=(struct _dyneither_ptr)((_tmp5BC=_tmp211,_tag_dyneither(_tmp5BC,sizeof(char),_get_zero_arr_size_char((void*)_tmp5BC,1)))),((_tmp5BD[0]=& _tmp5BB,Cyc_prscript(((_tmp5BE="cat >%s <<XXX\n",_tag_dyneither(_tmp5BE,sizeof(char),15))),_tag_dyneither(_tmp5BD,sizeof(void*),1)))))));}
{struct Cyc_List_List*_tmp21E=Cyc_current_cpp;for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){
const char*_tmp5C2;void*_tmp5C1[1];struct Cyc_String_pa_PrintArg_struct _tmp5C0;(_tmp5C0.tag=0,((_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp21E->hd)),((_tmp5C1[0]=& _tmp5C0,Cyc_prscript(((_tmp5C2="%s",_tag_dyneither(_tmp5C2,sizeof(char),3))),_tag_dyneither(_tmp5C1,sizeof(void*),1)))))));}}
{const char*_tmp5C8;void*_tmp5C7[1];const char*_tmp5C6;struct Cyc_String_pa_PrintArg_struct _tmp5C5;(_tmp5C5.tag=0,((_tmp5C5.f1=(struct _dyneither_ptr)((_tmp5C6=filename,_tag_dyneither(_tmp5C6,sizeof(char),_get_zero_arr_size_char((void*)_tmp5C6,1)))),((_tmp5C7[0]=& _tmp5C5,Cyc_prscript(((_tmp5C8="#include <%s>\n",_tag_dyneither(_tmp5C8,sizeof(char),15))),_tag_dyneither(_tmp5C7,sizeof(void*),1)))))));}
{const char*_tmp5CB;void*_tmp5CA;(_tmp5CA=0,Cyc_prscript(((_tmp5CB="XXX\n",_tag_dyneither(_tmp5CB,sizeof(char),5))),_tag_dyneither(_tmp5CA,sizeof(void*),0)));}
{const char*_tmp5D4;void*_tmp5D3[2];const char*_tmp5D2;struct Cyc_String_pa_PrintArg_struct _tmp5D1;const char*_tmp5D0;struct Cyc_String_pa_PrintArg_struct _tmp5CF;(_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((_tmp5D0=_tmp211,_tag_dyneither(_tmp5D0,sizeof(char),_get_zero_arr_size_char((void*)_tmp5D0,1)))),((_tmp5D1.tag=0,((_tmp5D1.f1=(struct _dyneither_ptr)((_tmp5D2=_tmp212,_tag_dyneither(_tmp5D2,sizeof(char),_get_zero_arr_size_char((void*)_tmp5D2,1)))),((_tmp5D3[0]=& _tmp5D1,((_tmp5D3[1]=& _tmp5CF,Cyc_prscript(((_tmp5D4="$GCC -E -dM -o %s -x c %s && \\\n",_tag_dyneither(_tmp5D4,sizeof(char),32))),_tag_dyneither(_tmp5D3,sizeof(void*),2)))))))))))));}
{const char*_tmp5DD;void*_tmp5DC[2];const char*_tmp5DB;struct Cyc_String_pa_PrintArg_struct _tmp5DA;const char*_tmp5D9;struct Cyc_String_pa_PrintArg_struct _tmp5D8;(_tmp5D8.tag=0,((_tmp5D8.f1=(struct _dyneither_ptr)((_tmp5D9=_tmp211,_tag_dyneither(_tmp5D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp5D9,1)))),((_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((_tmp5DB=_tmp213,_tag_dyneither(_tmp5DB,sizeof(char),_get_zero_arr_size_char((void*)_tmp5DB,1)))),((_tmp5DC[0]=& _tmp5DA,((_tmp5DC[1]=& _tmp5D8,Cyc_prscript(((_tmp5DD="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(_tmp5DD,sizeof(char),28))),_tag_dyneither(_tmp5DC,sizeof(void*),2)))))))))))));}{
const char*_tmp5E3;void*_tmp5E2[1];const char*_tmp5E1;struct Cyc_String_pa_PrintArg_struct _tmp5E0;(_tmp5E0.tag=0,((_tmp5E0.f1=(struct _dyneither_ptr)((_tmp5E1=_tmp211,_tag_dyneither(_tmp5E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E1,1)))),((_tmp5E2[0]=& _tmp5E0,Cyc_prscript(((_tmp5E3="rm %s\n",_tag_dyneither(_tmp5E3,sizeof(char),7))),_tag_dyneither(_tmp5E2,sizeof(void*),1)))))));};}else{
# 1361
maybe=Cyc_fopen(_tmp211,"w");
if(!((unsigned int)maybe)){
{const char*_tmp5E9;void*_tmp5E8[1];const char*_tmp5E7;struct Cyc_String_pa_PrintArg_struct _tmp5E6;(_tmp5E6.tag=0,((_tmp5E6.f1=(struct _dyneither_ptr)((_tmp5E7=_tmp211,_tag_dyneither(_tmp5E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E7,1)))),((_tmp5E8[0]=& _tmp5E6,Cyc_fprintf(Cyc_stderr,((_tmp5E9="Error: could not create file %s\n",_tag_dyneither(_tmp5E9,sizeof(char),33))),_tag_dyneither(_tmp5E8,sizeof(void*),1)))))));}{
int _tmp23C=1;_npop_handler(0);return _tmp23C;};}
# 1366
out_file=maybe;
{struct Cyc_List_List*_tmp23D=Cyc_current_cpp;for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){
Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp23D->hd),sizeof(char),1),out_file);}}
{const char*_tmp5EF;void*_tmp5EE[1];const char*_tmp5ED;struct Cyc_String_pa_PrintArg_struct _tmp5EC;(_tmp5EC.tag=0,((_tmp5EC.f1=(struct _dyneither_ptr)((_tmp5ED=filename,_tag_dyneither(_tmp5ED,sizeof(char),_get_zero_arr_size_char((void*)_tmp5ED,1)))),((_tmp5EE[0]=& _tmp5EC,Cyc_fprintf(out_file,((_tmp5EF="#include <%s>\n",_tag_dyneither(_tmp5EF,sizeof(char),15))),_tag_dyneither(_tmp5EE,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp242=Cstring_to_string(Ccomp);
const char*_tmp5F6;struct _dyneither_ptr*_tmp5F5;const char*_tmp5F4;struct Cyc_List_List*_tmp5F3;struct _dyneither_ptr _tmp243=
Cyc_str_sepstr(((_tmp5F3=_cycalloc(sizeof(*_tmp5F3)),((_tmp5F3->hd=((_tmp5F5=_cycalloc(sizeof(*_tmp5F5)),((_tmp5F5[0]=(struct _dyneither_ptr)((_tmp5F4="",_tag_dyneither(_tmp5F4,sizeof(char),1))),_tmp5F5)))),((_tmp5F3->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp5F3)))))),(
(_tmp5F6=" ",_tag_dyneither(_tmp5F6,sizeof(char),2))));
const char*_tmp601;void*_tmp600[4];struct Cyc_String_pa_PrintArg_struct _tmp5FF;const char*_tmp5FE;struct Cyc_String_pa_PrintArg_struct _tmp5FD;struct Cyc_String_pa_PrintArg_struct _tmp5FC;const char*_tmp5FB;struct Cyc_String_pa_PrintArg_struct _tmp5FA;char*cmd=(char*)_untag_dyneither_ptr(((_tmp5FA.tag=0,((_tmp5FA.f1=(struct _dyneither_ptr)((_tmp5FB=_tmp211,_tag_dyneither(_tmp5FB,sizeof(char),_get_zero_arr_size_char((void*)_tmp5FB,1)))),((_tmp5FC.tag=0,((_tmp5FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp243),((_tmp5FD.tag=0,((_tmp5FD.f1=(struct _dyneither_ptr)((_tmp5FE=_tmp212,_tag_dyneither(_tmp5FE,sizeof(char),_get_zero_arr_size_char((void*)_tmp5FE,1)))),((_tmp5FF.tag=0,((_tmp5FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp242),((_tmp600[0]=& _tmp5FF,((_tmp600[1]=& _tmp5FD,((_tmp600[2]=& _tmp5FC,((_tmp600[3]=& _tmp5FA,Cyc_aprintf(((_tmp601="%s -E -dM -o %s -x c %s %s",_tag_dyneither(_tmp601,sizeof(char),27))),_tag_dyneither(_tmp600,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);
# 1378
if(!system((const char*)cmd)){
# 1381
{const char*_tmp60C;void*_tmp60B[4];struct Cyc_String_pa_PrintArg_struct _tmp60A;const char*_tmp609;struct Cyc_String_pa_PrintArg_struct _tmp608;struct Cyc_String_pa_PrintArg_struct _tmp607;const char*_tmp606;struct Cyc_String_pa_PrintArg_struct _tmp605;cmd=(char*)_untag_dyneither_ptr(((_tmp605.tag=0,((_tmp605.f1=(struct _dyneither_ptr)((_tmp606=_tmp211,_tag_dyneither(_tmp606,sizeof(char),_get_zero_arr_size_char((void*)_tmp606,1)))),((_tmp607.tag=0,((_tmp607.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp243),((_tmp608.tag=0,((_tmp608.f1=(struct _dyneither_ptr)((_tmp609=_tmp213,_tag_dyneither(_tmp609,sizeof(char),_get_zero_arr_size_char((void*)_tmp609,1)))),((_tmp60A.tag=0,((_tmp60A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp242),((_tmp60B[0]=& _tmp60A,((_tmp60B[1]=& _tmp608,((_tmp60B[2]=& _tmp607,((_tmp60B[3]=& _tmp605,Cyc_aprintf(((_tmp60C="%s -E -o %s -x c %s %s",_tag_dyneither(_tmp60C,sizeof(char),23))),_tag_dyneither(_tmp60B,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);}
# 1383
system((const char*)cmd);}};}}
# 1388
if(Cyc_gathering()){int _tmp258=0;_npop_handler(0);return _tmp258;}{
# 1391
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp212,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp61D;const char*_tmp61C;void*_tmp61B[1];const char*_tmp61A;struct Cyc_String_pa_PrintArg_struct _tmp619;struct Cyc_NO_SUPPORT_exn_struct*_tmp618;(int)_throw((void*)((_tmp618=_cycalloc(sizeof(*_tmp618)),((_tmp618[0]=((_tmp61D.tag=Cyc_NO_SUPPORT,((_tmp61D.f1=(struct _dyneither_ptr)((_tmp619.tag=0,((_tmp619.f1=(struct _dyneither_ptr)((_tmp61A=_tmp212,_tag_dyneither(_tmp61A,sizeof(char),_get_zero_arr_size_char((void*)_tmp61A,1)))),((_tmp61B[0]=& _tmp619,Cyc_aprintf(((_tmp61C="can't open macrosfile %s",_tag_dyneither(_tmp61C,sizeof(char),25))),_tag_dyneither(_tmp61B,sizeof(void*),1)))))))),_tmp61D)))),_tmp618)))));}
# 1396
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp25F=Cyc_Lexing_from_file(in_file);
struct _tuple19*entry;
while((entry=((struct _tuple19*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp25F))!= 0){
struct _dyneither_ptr*_tmp261;struct Cyc_Set_Set*_tmp262;struct _tuple19*_tmp260=(struct _tuple19*)_check_null(entry);_tmp261=_tmp260->f1;_tmp262=_tmp260->f2;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp261,_tmp262);}
# 1405
Cyc_fclose(in_file);
# 1408
maybe=Cyc_fopen(_tmp213,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp62E;const char*_tmp62D;void*_tmp62C[1];const char*_tmp62B;struct Cyc_String_pa_PrintArg_struct _tmp62A;struct Cyc_NO_SUPPORT_exn_struct*_tmp629;(int)_throw((void*)((_tmp629=_cycalloc(sizeof(*_tmp629)),((_tmp629[0]=((_tmp62E.tag=Cyc_NO_SUPPORT,((_tmp62E.f1=(struct _dyneither_ptr)((_tmp62A.tag=0,((_tmp62A.f1=(struct _dyneither_ptr)((_tmp62B=_tmp213,_tag_dyneither(_tmp62B,sizeof(char),_get_zero_arr_size_char((void*)_tmp62B,1)))),((_tmp62C[0]=& _tmp62A,Cyc_aprintf(((_tmp62D="can't open declsfile %s",_tag_dyneither(_tmp62D,sizeof(char),24))),_tag_dyneither(_tmp62C,sizeof(void*),1)))))))),_tmp62E)))),_tmp629)))));}
# 1412
in_file=maybe;
_tmp25F=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp214,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp269=1;_npop_handler(0);return _tmp269;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp25F)){;}
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1422
maybe=Cyc_fopen(_tmp214,"r");
if(!((unsigned int)maybe)){int _tmp26A=1;_npop_handler(0);return _tmp26A;}
in_file=maybe;
{const char*_tmp62F;Cyc_Position_reset_position(((_tmp62F=_tmp214,_tag_dyneither(_tmp62F,sizeof(char),_get_zero_arr_size_char((void*)_tmp62F,1)))));}
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp26C=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1432
{struct Cyc_List_List*_tmp26D=_tmp26C;for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp26D->hd,t);}}{
# 1436
struct Cyc_Set_Set*_tmp26E=Cyc_reachable(start_symbols,t);
# 1439
struct Cyc_List_List*_tmp26F=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp270=_tmp26C;for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
struct Cyc_Absyn_Decl*_tmp271=(struct Cyc_Absyn_Decl*)_tmp270->hd;
struct _dyneither_ptr*name;
{void*_tmp272=_tmp271->r;struct Cyc_Absyn_Vardecl*_tmp274;struct Cyc_Absyn_Fndecl*_tmp276;struct Cyc_Absyn_Aggrdecl*_tmp278;struct Cyc_Absyn_Enumdecl*_tmp27A;struct Cyc_Absyn_Typedefdecl*_tmp27C;_LL195: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp273=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp273->tag != 0)goto _LL197;else{_tmp274=_tmp273->f1;}}_LL196: {
# 1448
struct _dyneither_ptr*_tmp289;struct _tuple0*_tmp288=_tmp274->name;_tmp289=_tmp288->f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp289);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp289))name=0;else{
name=_tmp289;}
goto _LL194;}_LL197: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp275=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp275->tag != 1)goto _LL199;else{_tmp276=_tmp275->f1;}}_LL198: {
# 1454
struct _dyneither_ptr*_tmp28B;struct _tuple0*_tmp28A=_tmp276->name;_tmp28B=_tmp28A->f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp28B);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp28B))name=0;else{
name=_tmp28B;}
goto _LL194;}_LL199: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp277=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp277->tag != 6)goto _LL19B;else{_tmp278=_tmp277->f1;}}_LL19A: {
# 1460
struct _dyneither_ptr*_tmp28D;struct _tuple0*_tmp28C=_tmp278->name;_tmp28D=_tmp28C->f2;
name=_tmp28D;
goto _LL194;}_LL19B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp279=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp279->tag != 8)goto _LL19D;else{_tmp27A=_tmp279->f1;}}_LL19C: {
# 1464
struct _dyneither_ptr*_tmp28F;struct _tuple0*_tmp28E=_tmp27A->name;_tmp28F=_tmp28E->f2;
name=_tmp28F;
# 1468
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp26E,name)){
struct Cyc_List_List*_tmp630;_tmp26F=((_tmp630=_cycalloc(sizeof(*_tmp630)),((_tmp630->hd=_tmp271,((_tmp630->tl=_tmp26F,_tmp630))))));}else{
# 1471
if((unsigned int)_tmp27A->fields){
struct Cyc_List_List*_tmp291=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp27A->fields))->v;for(0;_tmp291 != 0;_tmp291=_tmp291->tl){
struct Cyc_Absyn_Enumfield*_tmp292=(struct Cyc_Absyn_Enumfield*)_tmp291->hd;
struct _dyneither_ptr*_tmp294;struct _tuple0*_tmp293=_tmp292->name;_tmp294=_tmp293->f2;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp26E,_tmp294)){
{struct Cyc_List_List*_tmp631;_tmp26F=((_tmp631=_cycalloc(sizeof(*_tmp631)),((_tmp631->hd=_tmp271,((_tmp631->tl=_tmp26F,_tmp631))))));}
break;}}}}
# 1481
name=0;
goto _LL194;}_LL19D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp27B=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp27B->tag != 9)goto _LL19F;else{_tmp27C=_tmp27B->f1;}}_LL19E: {
# 1484
struct _dyneither_ptr*_tmp297;struct _tuple0*_tmp296=_tmp27C->name;_tmp297=_tmp296->f2;
name=_tmp297;
goto _LL194;}_LL19F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp27D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp27D->tag != 14)goto _LL1A1;}_LL1A0:
 goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp27E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp27E->tag != 15)goto _LL1A3;}_LL1A2:
 goto _LL1A4;_LL1A3: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp27F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp27F->tag != 2)goto _LL1A5;}_LL1A4:
 goto _LL1A6;_LL1A5: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp280=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp280->tag != 7)goto _LL1A7;}_LL1A6:
 goto _LL1A8;_LL1A7: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp281=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp281->tag != 3)goto _LL1A9;}_LL1A8:
 goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp282=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp282->tag != 10)goto _LL1AB;}_LL1AA:
 goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp283=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp283->tag != 11)goto _LL1AD;}_LL1AC:
 goto _LL1AE;_LL1AD: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp284=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp284->tag != 12)goto _LL1AF;}_LL1AE:
 goto _LL1B0;_LL1AF: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp285=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp285->tag != 13)goto _LL1B1;}_LL1B0:
 goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp286=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp286->tag != 4)goto _LL1B3;}_LL1B2:
 goto _LL1B4;_LL1B3: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp287=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp272;if(_tmp287->tag != 5)goto _LL194;}_LL1B4:
# 1498
 name=0;
goto _LL194;_LL194:;}
# 1501
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp26E,name)){
struct Cyc_List_List*_tmp632;_tmp26F=((_tmp632=_cycalloc(sizeof(*_tmp632)),((_tmp632->hd=_tmp271,((_tmp632->tl=_tmp26F,_tmp632))))));}}}
# 1506
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp299=1;_npop_handler(0);return _tmp299;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
const char*_tmp638;void*_tmp637[1];const char*_tmp636;struct Cyc_String_pa_PrintArg_struct _tmp635;struct _dyneither_ptr ifdefmacro=(_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((_tmp636=filename,_tag_dyneither(_tmp636,sizeof(char),_get_zero_arr_size_char((void*)_tmp636,1)))),((_tmp637[0]=& _tmp635,Cyc_aprintf(((_tmp638="_%s_",_tag_dyneither(_tmp638,sizeof(char),5))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));
{int _tmp29A=0;for(0;_tmp29A < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp29A){
if(((char*)ifdefmacro.curr)[_tmp29A]== '.'  || ((char*)ifdefmacro.curr)[_tmp29A]== '/'){
char _tmp63B;char _tmp63A;struct _dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp29A),((_tmp63A=*((char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='_',((_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}else{
if(((char*)ifdefmacro.curr)[_tmp29A]!= '_'  && ((char*)ifdefmacro.curr)[_tmp29A]!= '/'){
char _tmp63E;char _tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp29A),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((_tmp63E=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp29A]),((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'  && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}}}}
# 1518
{const char*_tmp643;void*_tmp642[2];struct Cyc_String_pa_PrintArg_struct _tmp641;struct Cyc_String_pa_PrintArg_struct _tmp640;(_tmp640.tag=0,((_tmp640.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp641.tag=0,((_tmp641.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp642[0]=& _tmp641,((_tmp642[1]=& _tmp640,Cyc_fprintf(out_file,((_tmp643="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp643,sizeof(char),23))),_tag_dyneither(_tmp642,sizeof(void*),2)))))))))))));}{
# 1525
struct Cyc_List_List*_tmp2A5=0;
struct Cyc_List_List*_tmp2A6=0;
{struct Cyc_List_List*_tmp2A7=_tmp26F;for(0;_tmp2A7 != 0;_tmp2A7=_tmp2A7->tl){
struct Cyc_Absyn_Decl*_tmp2A8=(struct Cyc_Absyn_Decl*)_tmp2A7->hd;
int _tmp2A9=0;
struct _dyneither_ptr*name;
{void*_tmp2AA=_tmp2A8->r;struct Cyc_Absyn_Vardecl*_tmp2AC;struct Cyc_Absyn_Fndecl*_tmp2AE;struct Cyc_Absyn_Aggrdecl*_tmp2B0;struct Cyc_Absyn_Enumdecl*_tmp2B2;struct Cyc_Absyn_Typedefdecl*_tmp2B4;_LL1B6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2AB->tag != 0)goto _LL1B8;else{_tmp2AC=_tmp2AB->f1;}}_LL1B7: {
# 1533
struct _dyneither_ptr*_tmp2C1;struct _tuple0*_tmp2C0=_tmp2AC->name;_tmp2C1=_tmp2C0->f2;
name=_tmp2C1;
goto _LL1B5;}_LL1B8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp2AD=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2AD->tag != 1)goto _LL1BA;else{_tmp2AE=_tmp2AD->f1;}}_LL1B9: {
# 1537
struct _dyneither_ptr*_tmp2C3;struct _tuple0*_tmp2C2=_tmp2AE->name;_tmp2C3=_tmp2C2->f2;
name=_tmp2C3;
goto _LL1B5;}_LL1BA: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp2AF=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2AF->tag != 6)goto _LL1BC;else{_tmp2B0=_tmp2AF->f1;}}_LL1BB: {
# 1541
struct _dyneither_ptr*_tmp2C5;struct _tuple0*_tmp2C4=_tmp2B0->name;_tmp2C5=_tmp2C4->f2;
name=_tmp2C5;
goto _LL1B5;}_LL1BC: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp2B1=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B1->tag != 8)goto _LL1BE;else{_tmp2B2=_tmp2B1->f1;}}_LL1BD: {
# 1545
struct _dyneither_ptr*_tmp2C7;struct _tuple0*_tmp2C6=_tmp2B2->name;_tmp2C7=_tmp2C6->f2;
name=_tmp2C7;
goto _LL1B5;}_LL1BE: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp2B3=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B3->tag != 9)goto _LL1C0;else{_tmp2B4=_tmp2B3->f1;}}_LL1BF: {
# 1549
struct _dyneither_ptr*_tmp2C9;struct _tuple0*_tmp2C8=_tmp2B4->name;_tmp2C9=_tmp2C8->f2;
name=_tmp2C9;
goto _LL1B5;}_LL1C0: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2B5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B5->tag != 4)goto _LL1C2;}_LL1C1:
 goto _LL1C3;_LL1C2: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp2B6=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B6->tag != 5)goto _LL1C4;}_LL1C3:
 goto _LL1C5;_LL1C4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp2B7=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B7->tag != 14)goto _LL1C6;}_LL1C5:
 goto _LL1C7;_LL1C6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp2B8=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B8->tag != 15)goto _LL1C8;}_LL1C7:
 goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp2B9=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2B9->tag != 2)goto _LL1CA;}_LL1C9:
 goto _LL1CB;_LL1CA: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2BA=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BA->tag != 7)goto _LL1CC;}_LL1CB:
 goto _LL1CD;_LL1CC: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp2BB=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BB->tag != 3)goto _LL1CE;}_LL1CD:
 goto _LL1CF;_LL1CE: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp2BC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BC->tag != 10)goto _LL1D0;}_LL1CF:
 goto _LL1D1;_LL1D0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp2BD=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BD->tag != 11)goto _LL1D2;}_LL1D1:
 goto _LL1D3;_LL1D2: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp2BE=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BE->tag != 12)goto _LL1D4;}_LL1D3:
 goto _LL1D5;_LL1D4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp2BF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2AA;if(_tmp2BF->tag != 13)goto _LL1B5;}_LL1D5:
# 1563
 name=0;
goto _LL1B5;_LL1B5:;}
# 1566
if(!((unsigned int)name) && !_tmp2A9)continue;
{struct Cyc_List_List*_tmp644;_tmp2A5=((_tmp644=_cycalloc(sizeof(*_tmp644)),((_tmp644->hd=_tmp2A8,((_tmp644->tl=_tmp2A5,_tmp644))))));}{
struct Cyc_List_List*_tmp645;_tmp2A6=((_tmp645=_cycalloc(sizeof(*_tmp645)),((_tmp645->hd=name,((_tmp645->tl=_tmp2A6,_tmp645))))));};}}
# 1572
{struct _handler_cons _tmp2CC;_push_handler(& _tmp2CC);{int _tmp2CE=0;if(setjmp(_tmp2CC.handler))_tmp2CE=1;if(!_tmp2CE){
{struct _RegionHandle _tmp2CF=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2CF;_push_region(tc_rgn);
{struct Cyc_Tcenv_Tenv*_tmp2D0=Cyc_Tcenv_tc_init(tc_rgn);
Cyc_Tc_tc(tc_rgn,_tmp2D0,1,_tmp2A5);}
# 1574
;_pop_region(tc_rgn);}
# 1573
;_pop_handler();}else{void*_tmp2CD=(void*)_exn_thrown;void*_tmp2D2=_tmp2CD;void*_tmp2D3;_LL1D7:;_LL1D8:
# 1579
{struct Cyc_NO_SUPPORT_exn_struct _tmp64B;const char*_tmp64A;struct Cyc_NO_SUPPORT_exn_struct*_tmp649;(int)_throw((void*)((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649[0]=((_tmp64B.tag=Cyc_NO_SUPPORT,((_tmp64B.f1=((_tmp64A="can't typecheck acquired declarations",_tag_dyneither(_tmp64A,sizeof(char),38))),_tmp64B)))),_tmp649)))));}
goto _LL1D6;_LL1D9: _tmp2D3=_tmp2D2;_LL1DA:(void)_throw(_tmp2D3);_LL1D6:;}};}
# 1584
{struct Cyc_List_List*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct _tuple24 _tmp64C;struct _tuple24 _tmp2D8=(_tmp64C.f1=_tmp2A5,((_tmp64C.f2=_tmp2A6,_tmp64C)));_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;for(0;
_tmp2D9 != 0  && _tmp2DA != 0;(_tmp2D9=_tmp2D9->tl,_tmp2DA=_tmp2DA->tl)){
struct Cyc_Absyn_Decl*_tmp2DB=(struct Cyc_Absyn_Decl*)_tmp2D9->hd;
struct _dyneither_ptr*_tmp2DC=(struct _dyneither_ptr*)_tmp2DA->hd;
int _tmp2DD=0;
if(!((unsigned int)_tmp2DC))
_tmp2DD=1;
# 1592
if((unsigned int)_tmp2DC){
{const char*_tmp650;void*_tmp64F[1];struct Cyc_String_pa_PrintArg_struct _tmp64E;ifdefmacro=((_tmp64E.tag=0,((_tmp64E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DC),((_tmp64F[0]=& _tmp64E,Cyc_aprintf(((_tmp650="_%s_def_",_tag_dyneither(_tmp650,sizeof(char),9))),_tag_dyneither(_tmp64F,sizeof(void*),1))))))));}
{const char*_tmp654;void*_tmp653[1];struct Cyc_String_pa_PrintArg_struct _tmp652;(_tmp652.tag=0,((_tmp652.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp653[0]=& _tmp652,Cyc_fprintf(out_file,((_tmp654="#ifndef %s\n",_tag_dyneither(_tmp654,sizeof(char),12))),_tag_dyneither(_tmp653,sizeof(void*),1)))))));}
{const char*_tmp658;void*_tmp657[1];struct Cyc_String_pa_PrintArg_struct _tmp656;(_tmp656.tag=0,((_tmp656.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp657[0]=& _tmp656,Cyc_fprintf(out_file,((_tmp658="#define %s\n",_tag_dyneither(_tmp658,sizeof(char),12))),_tag_dyneither(_tmp657,sizeof(void*),1)))))));}
# 1597
{struct Cyc_Absyn_Decl*_tmp659[1];Cyc_Absynpp_decllist2file(((_tmp659[0]=_tmp2DB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp659,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}{
const char*_tmp65C;void*_tmp65B;(_tmp65B=0,Cyc_fprintf(out_file,((_tmp65C="#endif\n",_tag_dyneither(_tmp65C,sizeof(char),8))),_tag_dyneither(_tmp65B,sizeof(void*),0)));};}else{
# 1602
struct Cyc_Absyn_Decl*_tmp65D[1];Cyc_Absynpp_decllist2file(((_tmp65D[0]=_tmp2DB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp65D,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}
# 1607
maybe=Cyc_fopen(_tmp212,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp66E;const char*_tmp66D;void*_tmp66C[1];const char*_tmp66B;struct Cyc_String_pa_PrintArg_struct _tmp66A;struct Cyc_NO_SUPPORT_exn_struct*_tmp669;(int)_throw((void*)((_tmp669=_cycalloc(sizeof(*_tmp669)),((_tmp669[0]=((_tmp66E.tag=Cyc_NO_SUPPORT,((_tmp66E.f1=(struct _dyneither_ptr)((_tmp66A.tag=0,((_tmp66A.f1=(struct _dyneither_ptr)((_tmp66B=_tmp212,_tag_dyneither(_tmp66B,sizeof(char),_get_zero_arr_size_char((void*)_tmp66B,1)))),((_tmp66C[0]=& _tmp66A,Cyc_aprintf(((_tmp66D="can't open macrosfile %s",_tag_dyneither(_tmp66D,sizeof(char),25))),_tag_dyneither(_tmp66C,sizeof(void*),1)))))))),_tmp66E)))),_tmp669)))));}
# 1610
in_file=maybe;
_tmp25F=Cyc_Lexing_from_file(in_file);{
struct _tuple20*entry2;
while((entry2=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp25F))!= 0){
struct _dyneither_ptr _tmp2F2;struct _dyneither_ptr*_tmp2F3;struct _tuple20*_tmp2F1=(struct _tuple20*)_check_null(entry2);_tmp2F2=_tmp2F1->f1;_tmp2F3=_tmp2F1->f2;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp26E,_tmp2F3)){
{const char*_tmp672;void*_tmp671[1];struct Cyc_String_pa_PrintArg_struct _tmp670;(_tmp670.tag=0,((_tmp670.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F3),((_tmp671[0]=& _tmp670,Cyc_fprintf(out_file,((_tmp672="#ifndef %s\n",_tag_dyneither(_tmp672,sizeof(char),12))),_tag_dyneither(_tmp671,sizeof(void*),1)))))));}
{const char*_tmp676;void*_tmp675[1];struct Cyc_String_pa_PrintArg_struct _tmp674;(_tmp674.tag=0,((_tmp674.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F2),((_tmp675[0]=& _tmp674,Cyc_fprintf(out_file,((_tmp676="%s\n",_tag_dyneither(_tmp676,sizeof(char),4))),_tag_dyneither(_tmp675,sizeof(void*),1)))))));}{
const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_fprintf(out_file,((_tmp679="#endif\n",_tag_dyneither(_tmp679,sizeof(char),8))),_tag_dyneither(_tmp678,sizeof(void*),0)));};}}
# 1621
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1624
if(hstubs != 0){
struct Cyc_List_List*_tmp2FC=hstubs;for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
struct _dyneither_ptr _tmp2FE;struct _dyneither_ptr _tmp2FF;struct _tuple21*_tmp2FD=(struct _tuple21*)_tmp2FC->hd;_tmp2FE=_tmp2FD->f1;_tmp2FF=_tmp2FD->f2;{
struct _dyneither_ptr*_tmp67A;if((char*)_tmp2FF.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2FE.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp67A=_cycalloc(sizeof(*_tmp67A)),((_tmp67A[0]=_tmp2FE,_tmp67A)))))))
# 1630
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2FF,sizeof(char),1),out_file);else{
# 1632
const char*_tmp67E;void*_tmp67D[1];struct Cyc_String_pa_PrintArg_struct _tmp67C;(_tmp67C.tag=0,((_tmp67C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FE),((_tmp67D[0]=& _tmp67C,Cyc_log(((_tmp67E="%s is not supported on this platform\n",_tag_dyneither(_tmp67E,sizeof(char),38))),_tag_dyneither(_tmp67D,sizeof(void*),1)))))));}};}}
# 1635
{const char*_tmp681;void*_tmp680;(_tmp680=0,Cyc_fprintf(out_file,((_tmp681="#endif\n",_tag_dyneither(_tmp681,sizeof(char),8))),_tag_dyneither(_tmp680,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp306=0;_npop_handler(0);return _tmp306;}else{
Cyc_fclose(out_file);}
# 1640
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp307=cstubs;for(0;_tmp307 != 0;_tmp307=_tmp307->tl){
struct _dyneither_ptr _tmp309;struct _dyneither_ptr _tmp30A;struct _tuple21*_tmp308=(struct _tuple21*)_tmp307->hd;_tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;{
struct _dyneither_ptr*_tmp682;if((char*)_tmp30A.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp309.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp682=_cycalloc(sizeof(*_tmp682)),((_tmp682[0]=_tmp309,_tmp682)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp30A,sizeof(char),1),out_file);};}};}
# 1651
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1653
{const char*_tmp688;void*_tmp687[1];const char*_tmp686;struct Cyc_String_pa_PrintArg_struct _tmp685;(_tmp685.tag=0,((_tmp685.f1=(struct _dyneither_ptr)((_tmp686=filename,_tag_dyneither(_tmp686,sizeof(char),_get_zero_arr_size_char((void*)_tmp686,1)))),((_tmp687[0]=& _tmp685,Cyc_fprintf(out_file,((_tmp688="#include <%s>\n\n",_tag_dyneither(_tmp688,sizeof(char),16))),_tag_dyneither(_tmp687,sizeof(void*),1)))))));}
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp310=cycstubs;for(0;_tmp310 != 0;_tmp310=_tmp310->tl){
struct _dyneither_ptr _tmp312;struct _dyneither_ptr _tmp313;struct _tuple21*_tmp311=(struct _tuple21*)_tmp310->hd;_tmp312=_tmp311->f1;_tmp313=_tmp311->f2;{
struct _dyneither_ptr*_tmp689;if((char*)_tmp313.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp312.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp689=_cycalloc(sizeof(*_tmp689)),((_tmp689[0]=_tmp312,_tmp689)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp313,sizeof(char),1),out_file);};}}{
# 1662
const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(out_file,((_tmp68C="\n",_tag_dyneither(_tmp68C,sizeof(char),2))),_tag_dyneither(_tmp68B,sizeof(void*),0)));};}{
# 1665
int _tmp317=0;_npop_handler(0);return _tmp317;};};};};};};};};
# 1343
;_pop_handler();}else{void*_tmp216=(void*)_exn_thrown;void*_tmp31D=_tmp216;struct _dyneither_ptr _tmp31F;struct _dyneither_ptr _tmp322;struct _dyneither_ptr _tmp324;struct _dyneither_ptr _tmp327;void*_tmp328;_LL1DC: {struct Cyc_Core_Impossible_exn_struct*_tmp31E=(struct Cyc_Core_Impossible_exn_struct*)_tmp31D;if(_tmp31E->tag != Cyc_Core_Impossible)goto _LL1DE;else{_tmp31F=_tmp31E->f1;}}_LL1DD:
# 1669
{const char*_tmp690;void*_tmp68F[1];struct Cyc_String_pa_PrintArg_struct _tmp68E;(_tmp68E.tag=0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31F),((_tmp68F[0]=& _tmp68E,Cyc_fprintf(Cyc_stderr,((_tmp690="Got Core::Impossible(%s)\n",_tag_dyneither(_tmp690,sizeof(char),26))),_tag_dyneither(_tmp68F,sizeof(void*),1)))))));}goto _LL1DB;_LL1DE: {struct Cyc_Dict_Absent_exn_struct*_tmp320=(struct Cyc_Dict_Absent_exn_struct*)_tmp31D;if(_tmp320->tag != Cyc_Dict_Absent)goto _LL1E0;}_LL1DF:
# 1671
{const char*_tmp693;void*_tmp692;(_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="Got Dict::Absent\n",_tag_dyneither(_tmp693,sizeof(char),18))),_tag_dyneither(_tmp692,sizeof(void*),0)));}goto _LL1DB;_LL1E0: {struct Cyc_Core_Failure_exn_struct*_tmp321=(struct Cyc_Core_Failure_exn_struct*)_tmp31D;if(_tmp321->tag != Cyc_Core_Failure)goto _LL1E2;else{_tmp322=_tmp321->f1;}}_LL1E1:
# 1673
{const char*_tmp697;void*_tmp696[1];struct Cyc_String_pa_PrintArg_struct _tmp695;(_tmp695.tag=0,((_tmp695.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp322),((_tmp696[0]=& _tmp695,Cyc_fprintf(Cyc_stderr,((_tmp697="Got Core::Failure(%s)\n",_tag_dyneither(_tmp697,sizeof(char),23))),_tag_dyneither(_tmp696,sizeof(void*),1)))))));}goto _LL1DB;_LL1E2: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp323=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp31D;if(_tmp323->tag != Cyc_Core_Invalid_argument)goto _LL1E4;else{_tmp324=_tmp323->f1;}}_LL1E3:
# 1675
{const char*_tmp69B;void*_tmp69A[1];struct Cyc_String_pa_PrintArg_struct _tmp699;(_tmp699.tag=0,((_tmp699.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp324),((_tmp69A[0]=& _tmp699,Cyc_fprintf(Cyc_stderr,((_tmp69B="Got Invalid_argument(%s)\n",_tag_dyneither(_tmp69B,sizeof(char),26))),_tag_dyneither(_tmp69A,sizeof(void*),1)))))));}goto _LL1DB;_LL1E4: {struct Cyc_Core_Not_found_exn_struct*_tmp325=(struct Cyc_Core_Not_found_exn_struct*)_tmp31D;if(_tmp325->tag != Cyc_Core_Not_found)goto _LL1E6;}_LL1E5:
# 1677
{const char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E="Got Not_found\n",_tag_dyneither(_tmp69E,sizeof(char),15))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}goto _LL1DB;_LL1E6: {struct Cyc_NO_SUPPORT_exn_struct*_tmp326=(struct Cyc_NO_SUPPORT_exn_struct*)_tmp31D;if(_tmp326->tag != Cyc_NO_SUPPORT)goto _LL1E8;else{_tmp327=_tmp326->f1;}}_LL1E7:
# 1679
{const char*_tmp6A2;void*_tmp6A1[1];struct Cyc_String_pa_PrintArg_struct _tmp6A0;(_tmp6A0.tag=0,((_tmp6A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp327),((_tmp6A1[0]=& _tmp6A0,Cyc_fprintf(Cyc_stderr,((_tmp6A2="No support because %s\n",_tag_dyneither(_tmp6A2,sizeof(char),23))),_tag_dyneither(_tmp6A1,sizeof(void*),1)))))));}goto _LL1DB;_LL1E8:;_LL1E9:
# 1681
{const char*_tmp6A5;void*_tmp6A4;(_tmp6A4=0,Cyc_fprintf(Cyc_stderr,((_tmp6A5="Got unknown exception\n",_tag_dyneither(_tmp6A5,sizeof(char),23))),_tag_dyneither(_tmp6A4,sizeof(void*),0)));}goto _LL1DB;_LL1EA: _tmp328=_tmp31D;_LL1EB:(void)_throw(_tmp328);_LL1DB:;}};}
# 1686
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
{const char*_tmp6AB;void*_tmp6AA[1];const char*_tmp6A9;struct Cyc_String_pa_PrintArg_struct _tmp6A8;(_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((_tmp6A9=filename,_tag_dyneither(_tmp6A9,sizeof(char),_get_zero_arr_size_char((void*)_tmp6A9,1)))),((_tmp6AA[0]=& _tmp6A8,Cyc_fprintf(Cyc_stderr,((_tmp6AB="Error: could not create file %s\n",_tag_dyneither(_tmp6AB,sizeof(char),33))),_tag_dyneither(_tmp6AA,sizeof(void*),1)))))));}
return 1;}
# 1691
out_file=maybe;
{const char*_tmp6B1;void*_tmp6B0[1];const char*_tmp6AF;struct Cyc_String_pa_PrintArg_struct _tmp6AE;(_tmp6AE.tag=0,((_tmp6AE.f1=(struct _dyneither_ptr)(
# 1694
(_tmp6AF=filename,_tag_dyneither(_tmp6AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp6AF,1)))),((_tmp6B0[0]=& _tmp6AE,Cyc_fprintf(out_file,((_tmp6B1="#error -- %s is not supported on this platform\n",_tag_dyneither(_tmp6B1,sizeof(char),48))),_tag_dyneither(_tmp6B0,sizeof(void*),1)))))));}
Cyc_fclose(out_file);
{const char*_tmp6B7;void*_tmp6B6[1];const char*_tmp6B5;struct Cyc_String_pa_PrintArg_struct _tmp6B4;(_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)(
(_tmp6B5=filename,_tag_dyneither(_tmp6B5,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B5,1)))),((_tmp6B6[0]=& _tmp6B4,Cyc_fprintf(Cyc_stderr,((_tmp6B7="Warning: %s will not be supported on this platform\n",_tag_dyneither(_tmp6B7,sizeof(char),52))),_tag_dyneither(_tmp6B6,sizeof(void*),1)))))));}
{const char*_tmp6BA;void*_tmp6B9;(_tmp6B9=0,Cyc_log(((_tmp6BA="Not supported on this platform\n",_tag_dyneither(_tmp6BA,sizeof(char),32))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}
# 1705
return 0;};}int Cyc_process_specfile(const char*file,const char*dir);static void _tmp6C5(unsigned int*_tmp6C4,unsigned int*_tmp6C3,char**_tmp6C2){for(*_tmp6C4=0;*_tmp6C4 < *_tmp6C3;(*_tmp6C4)++){(*_tmp6C2)[*_tmp6C4]='\000';}}
# 1709
int Cyc_process_specfile(const char*file,const char*dir){
struct Cyc___cycFILE*_tmp35E=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp35E)){
{const char*_tmp6C0;void*_tmp6BF[1];const char*_tmp6BE;struct Cyc_String_pa_PrintArg_struct _tmp6BD;(_tmp6BD.tag=0,((_tmp6BD.f1=(struct _dyneither_ptr)((_tmp6BE=file,_tag_dyneither(_tmp6BE,sizeof(char),_get_zero_arr_size_char((void*)_tmp6BE,1)))),((_tmp6BF[0]=& _tmp6BD,Cyc_fprintf(Cyc_stderr,((_tmp6C0="Error: could not open %s\n",_tag_dyneither(_tmp6C0,sizeof(char),26))),_tag_dyneither(_tmp6BF,sizeof(void*),1)))))));}
return 1;}{
# 1715
struct Cyc___cycFILE*_tmp363=_tmp35E;
# 1719
char*_tmp6CA;unsigned int _tmp6C9;char*_tmp6C8;unsigned int _tmp6C7;unsigned int _tmp6C6;struct _dyneither_ptr buf=(_tmp6CA=((_tmp6C9=(unsigned int)1024,((_tmp6C8=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6C9 + 1)),((((_tmp6C7=_tmp6C9,((_tmp6C5(& _tmp6C6,& _tmp6C7,& _tmp6C8),_tmp6C8[_tmp6C7]=(char)0)))),_tmp6C8)))))),_tag_dyneither(_tmp6CA,sizeof(char),_get_zero_arr_size_char((void*)_tmp6CA,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp364=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir(dir)){
{const char*_tmp6D0;void*_tmp6CF[1];const char*_tmp6CE;struct Cyc_String_pa_PrintArg_struct _tmp6CD;(_tmp6CD.tag=0,((_tmp6CD.f1=(struct _dyneither_ptr)((_tmp6CE=dir,_tag_dyneither(_tmp6CE,sizeof(char),_get_zero_arr_size_char((void*)_tmp6CE,1)))),((_tmp6CF[0]=& _tmp6CD,Cyc_fprintf(Cyc_stderr,((_tmp6D0="Error: can't change directory to %s\n",_tag_dyneither(_tmp6D0,sizeof(char),37))),_tag_dyneither(_tmp6CF,sizeof(void*),1)))))));}
return 1;}}
# 1727
if(Cyc_mode == Cyc_GATHER){
# 1729
struct _dyneither_ptr _tmp369=Cstring_to_string(Ccomp);
const char*_tmp6D4;void*_tmp6D3[1];struct Cyc_String_pa_PrintArg_struct _tmp6D2;system((const char*)_untag_dyneither_ptr(((_tmp6D2.tag=0,((_tmp6D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp369),((_tmp6D3[0]=& _tmp6D2,Cyc_aprintf(((_tmp6D4="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp6D4,sizeof(char),36))),_tag_dyneither(_tmp6D3,sizeof(void*),1)))))))),sizeof(char),1));}{
# 1733
struct Cyc_Lexing_lexbuf*_tmp36D=Cyc_Lexing_from_file(_tmp363);
struct _tuple22*entry;
while((entry=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp36D))!= 0){
struct _dyneither_ptr _tmp36F;struct Cyc_List_List*_tmp370;struct Cyc_List_List*_tmp371;struct Cyc_List_List*_tmp372;struct Cyc_List_List*_tmp373;struct Cyc_List_List*_tmp374;struct _tuple22*_tmp36E=(struct _tuple22*)_check_null(entry);_tmp36F=_tmp36E->f1;_tmp370=_tmp36E->f2;_tmp371=_tmp36E->f3;_tmp372=_tmp36E->f4;_tmp373=_tmp36E->f5;_tmp374=_tmp36E->f6;
# 1738
if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp36F,sizeof(char),1),_tmp370,_tmp371,_tmp372,_tmp373,_tmp374))
# 1740
return 1;}
# 1742
Cyc_fclose(_tmp363);
# 1744
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp364,sizeof(char),1)))){
{const char*_tmp6D8;void*_tmp6D7[1];struct Cyc_String_pa_PrintArg_struct _tmp6D6;(_tmp6D6.tag=0,((_tmp6D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp364),((_tmp6D7[0]=& _tmp6D6,Cyc_fprintf(Cyc_stderr,((_tmp6D8="Error: could not change directory to %s\n",_tag_dyneither(_tmp6D8,sizeof(char),41))),_tag_dyneither(_tmp6D7,sizeof(void*),1)))))));}
return 1;}}
# 1750
return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp6DD(unsigned int*_tmp6DC,unsigned int*_tmp6DB,char**_tmp6DA){for(*_tmp6DC=0;*_tmp6DC < *_tmp6DB;(*_tmp6DC)++){(*_tmp6DA)[*_tmp6DC]='\000';}}
# 1754
int Cyc_process_setjmp(const char*dir){
# 1757
char*_tmp6E2;unsigned int _tmp6E1;char*_tmp6E0;unsigned int _tmp6DF;unsigned int _tmp6DE;struct _dyneither_ptr buf=(_tmp6E2=((_tmp6E1=(unsigned int)1024,((_tmp6E0=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6E1 + 1)),((((_tmp6DF=_tmp6E1,((_tmp6DD(& _tmp6DE,& _tmp6DF,& _tmp6E0),_tmp6E0[_tmp6DF]=(char)0)))),_tmp6E0)))))),_tag_dyneither(_tmp6E2,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E2,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp37C=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(chdir(dir)){
{const char*_tmp6E8;void*_tmp6E7[1];const char*_tmp6E6;struct Cyc_String_pa_PrintArg_struct _tmp6E5;(_tmp6E5.tag=0,((_tmp6E5.f1=(struct _dyneither_ptr)((_tmp6E6=dir,_tag_dyneither(_tmp6E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E6,1)))),((_tmp6E7[0]=& _tmp6E5,Cyc_fprintf(Cyc_stderr,((_tmp6E8="Error: can't change directory to %s\n",_tag_dyneither(_tmp6E8,sizeof(char),37))),_tag_dyneither(_tmp6E7,sizeof(void*),1)))))));}
return 1;}
# 1763
{struct _tuple21*_tmp6F2;const char*_tmp6F1;const char*_tmp6F0;struct _tuple21*_tmp6EF[1];struct _dyneither_ptr*_tmp6E9[1];if(Cyc_process_file("setjmp.h",((_tmp6E9[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6E9,sizeof(struct _dyneither_ptr*),1)))),0,(
(_tmp6EF[0]=((_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((_tmp6F2->f1=((_tmp6F1="setjmp",_tag_dyneither(_tmp6F1,sizeof(char),7))),((_tmp6F2->f2=((_tmp6F0="extern int setjmp(jmp_buf);\n",_tag_dyneither(_tmp6F0,sizeof(char),29))),_tmp6F2)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6EF,sizeof(struct _tuple21*),1)))),0,0))
# 1766
return 1;}
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp37C,sizeof(char),1)))){
{const char*_tmp6F6;void*_tmp6F5[1];struct Cyc_String_pa_PrintArg_struct _tmp6F4;(_tmp6F4.tag=0,((_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37C),((_tmp6F5[0]=& _tmp6F4,Cyc_fprintf(Cyc_stderr,((_tmp6F6="Error: could not change directory to %s\n",_tag_dyneither(_tmp6F6,sizeof(char),41))),_tag_dyneither(_tmp6F5,sizeof(void*),1)))))));}
return 1;}
# 1771
return 0;}static char _tmp38E[13]="BUILDLIB.OUT";
# 1775
static struct _dyneither_ptr Cyc_output_dir={_tmp38E,_tmp38E,_tmp38E + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1779
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
struct Cyc_List_List*_tmp6F7;Cyc_spec_files=((_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7->hd=(const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp6F7->tl=Cyc_spec_files,_tmp6F7))))));}
# 1783
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1786
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1789
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1792
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp6FA;struct Cyc_List_List*_tmp6F9;Cyc_cppargs=((_tmp6F9=_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9->hd=((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=s,_tmp6FA)))),((_tmp6F9->tl=Cyc_cppargs,_tmp6F9))))));}
# 1795
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
{const char*_tmp6FE;void*_tmp6FD[1];struct Cyc_String_pa_PrintArg_struct _tmp6FC;(_tmp6FC.tag=0,((_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp6FD[0]=& _tmp6FC,Cyc_fprintf(Cyc_stderr,((_tmp6FE="Unsupported option %s\n",_tag_dyneither(_tmp6FE,sizeof(char),23))),_tag_dyneither(_tmp6FD,sizeof(void*),1)))))));}
Cyc_badparse=1;}
# 1804
void GC_blacklist_warn_clear();struct _tuple25{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
GC_blacklist_warn_clear();{
# 1808
struct _tuple25*_tmp76B;const char*_tmp76A;const char*_tmp769;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp768;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp767;const char*_tmp766;struct _tuple25*_tmp765;const char*_tmp764;const char*_tmp763;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp762;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp761;const char*_tmp760;struct _tuple25*_tmp75F;const char*_tmp75E;const char*_tmp75D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp75C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp75B;const char*_tmp75A;struct _tuple25*_tmp759;const char*_tmp758;const char*_tmp757;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp756;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp755;const char*_tmp754;struct _tuple25*_tmp753;const char*_tmp752;const char*_tmp751;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp750;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp74F;const char*_tmp74E;struct _tuple25*_tmp74D;const char*_tmp74C;const char*_tmp74B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp74A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp749;const char*_tmp748;struct _tuple25*_tmp747[6];struct Cyc_List_List*options=
(_tmp747[5]=(
# 1827
(_tmp74D=_cycalloc(sizeof(*_tmp74D)),((_tmp74D->f1=((_tmp74C="-",_tag_dyneither(_tmp74C,sizeof(char),2))),((_tmp74D->f2=1,((_tmp74D->f3=((_tmp74B="",_tag_dyneither(_tmp74B,sizeof(char),1))),((_tmp74D->f4=(void*)(
(_tmp749=_cycalloc(sizeof(*_tmp749)),((_tmp749[0]=((_tmp74A.tag=1,((_tmp74A.f1=Cyc_add_cpparg,_tmp74A)))),_tmp749)))),((_tmp74D->f5=(
(_tmp748="",_tag_dyneither(_tmp748,sizeof(char),1))),_tmp74D)))))))))))),((_tmp747[4]=(
# 1821
(_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753->f1=((_tmp752="-setjmp",_tag_dyneither(_tmp752,sizeof(char),8))),((_tmp753->f2=0,((_tmp753->f3=((_tmp751="",_tag_dyneither(_tmp751,sizeof(char),1))),((_tmp753->f4=(void*)(
(_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((_tmp750.tag=3,((_tmp750.f1=& Cyc_do_setjmp,_tmp750)))),_tmp74F)))),((_tmp753->f5=(
# 1826
(_tmp74E="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",_tag_dyneither(_tmp74E,sizeof(char),186))),_tmp753)))))))))))),((_tmp747[3]=(
# 1818
(_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759->f1=((_tmp758="-finish",_tag_dyneither(_tmp758,sizeof(char),8))),((_tmp759->f2=0,((_tmp759->f3=((_tmp757="",_tag_dyneither(_tmp757,sizeof(char),1))),((_tmp759->f4=(void*)(
(_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755[0]=((_tmp756.tag=0,((_tmp756.f1=Cyc_set_FINISH,_tmp756)))),_tmp755)))),((_tmp759->f5=(
(_tmp754="Produce Cyclone headers from pre-gathered C library info",_tag_dyneither(_tmp754,sizeof(char),57))),_tmp759)))))))))))),((_tmp747[2]=(
# 1815
(_tmp75F=_cycalloc(sizeof(*_tmp75F)),((_tmp75F->f1=((_tmp75E="-gatherscript",_tag_dyneither(_tmp75E,sizeof(char),14))),((_tmp75F->f2=0,((_tmp75F->f3=((_tmp75D="",_tag_dyneither(_tmp75D,sizeof(char),1))),((_tmp75F->f4=(void*)(
(_tmp75B=_cycalloc(sizeof(*_tmp75B)),((_tmp75B[0]=((_tmp75C.tag=0,((_tmp75C.f1=Cyc_set_GATHERSCRIPT,_tmp75C)))),_tmp75B)))),((_tmp75F->f5=(
(_tmp75A="Produce a script to gather C library info",_tag_dyneither(_tmp75A,sizeof(char),42))),_tmp75F)))))))))))),((_tmp747[1]=(
# 1812
(_tmp765=_cycalloc(sizeof(*_tmp765)),((_tmp765->f1=((_tmp764="-gather",_tag_dyneither(_tmp764,sizeof(char),8))),((_tmp765->f2=0,((_tmp765->f3=((_tmp763="",_tag_dyneither(_tmp763,sizeof(char),1))),((_tmp765->f4=(void*)(
(_tmp761=_cycalloc(sizeof(*_tmp761)),((_tmp761[0]=((_tmp762.tag=0,((_tmp762.f1=Cyc_set_GATHER,_tmp762)))),_tmp761)))),((_tmp765->f5=(
(_tmp760="Gather C library info but don't produce Cyclone headers",_tag_dyneither(_tmp760,sizeof(char),56))),_tmp765)))))))))))),((_tmp747[0]=(
# 1809
(_tmp76B=_cycalloc(sizeof(*_tmp76B)),((_tmp76B->f1=((_tmp76A="-d",_tag_dyneither(_tmp76A,sizeof(char),3))),((_tmp76B->f2=0,((_tmp76B->f3=((_tmp769=" <file>",_tag_dyneither(_tmp769,sizeof(char),8))),((_tmp76B->f4=(void*)(
(_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((_tmp768.tag=5,((_tmp768.f1=Cyc_set_output_dir,_tmp768)))),_tmp767)))),((_tmp76B->f5=(
(_tmp766="Set the output directory to <file>",_tag_dyneither(_tmp766,sizeof(char),35))),_tmp76B)))))))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp747,sizeof(struct _tuple25*),6)))))))))))));
# 1831
{const char*_tmp76C;Cyc_Arg_parse(options,Cyc_add_spec_file,((_tmp76C="Options:",_tag_dyneither(_tmp76C,sizeof(char),9))),argv);}
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp76D;Cyc_Arg_usage(options,(
# 1839
(_tmp76D="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",_tag_dyneither(_tmp76D,sizeof(char),59))));}
return 1;}
# 1843
if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
{const char*_tmp770;void*_tmp76F;(_tmp76F=0,Cyc_fprintf(Cyc_stderr,((_tmp770="Could not create file BUILDLIB.sh\n",_tag_dyneither(_tmp770,sizeof(char),35))),_tag_dyneither(_tmp76F,sizeof(void*),0)));}
exit(1);}
# 1849
{const char*_tmp773;void*_tmp772;(_tmp772=0,Cyc_prscript(((_tmp773="#!/bin/sh\n",_tag_dyneither(_tmp773,sizeof(char),11))),_tag_dyneither(_tmp772,sizeof(void*),0)));}{
const char*_tmp776;void*_tmp775;(_tmp775=0,Cyc_prscript(((_tmp776="GCC=\"gcc\"\n",_tag_dyneither(_tmp776,sizeof(char),11))),_tag_dyneither(_tmp775,sizeof(void*),0)));};}
# 1854
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
{const char*_tmp77A;void*_tmp779[1];struct Cyc_String_pa_PrintArg_struct _tmp778;(_tmp778.tag=0,((_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp779[0]=& _tmp778,Cyc_fprintf(Cyc_stderr,((_tmp77A="Error: could not create directory %s\n",_tag_dyneither(_tmp77A,sizeof(char),38))),_tag_dyneither(_tmp779,sizeof(void*),1)))))));}
return 1;}
# 1859
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp77E;void*_tmp77D[1];struct Cyc_String_pa_PrintArg_struct _tmp77C;(_tmp77C.tag=0,((_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp77D[0]=& _tmp77C,Cyc_prscript(((_tmp77E="cd %s\n",_tag_dyneither(_tmp77E,sizeof(char),7))),_tag_dyneither(_tmp77D,sizeof(void*),1)))))));}{
const char*_tmp781;void*_tmp780;(_tmp780=0,Cyc_prscript(((_tmp781="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp781,sizeof(char),38))),_tag_dyneither(_tmp780,sizeof(void*),0)));};}
# 1864
if(!Cyc_gathering()){
# 1867
{const char*_tmp782;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp782="BUILDLIB.LOG",_tag_dyneither(_tmp782,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_log_file)){
{const char*_tmp786;void*_tmp785[1];struct Cyc_String_pa_PrintArg_struct _tmp784;(_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp785[0]=& _tmp784,Cyc_fprintf(Cyc_stderr,((_tmp786="Error: could not create log file in directory %s\n",_tag_dyneither(_tmp786,sizeof(char),50))),_tag_dyneither(_tmp785,sizeof(void*),1)))))));}
return 1;}
# 1873
if(!Cyc_do_setjmp){
# 1875
{const char*_tmp787;Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp787="cstubs.c",_tag_dyneither(_tmp787,sizeof(char),9)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cstubs_file)){
{const char*_tmp78B;void*_tmp78A[1];struct Cyc_String_pa_PrintArg_struct _tmp789;(_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp78A[0]=& _tmp789,Cyc_fprintf(Cyc_stderr,((_tmp78B="Error: could not create cstubs.c in directory %s\n",_tag_dyneither(_tmp78B,sizeof(char),50))),_tag_dyneither(_tmp78A,sizeof(void*),1)))))));}
return 1;}
# 1882
{const char*_tmp78C;Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp78C="cycstubs.cyc",_tag_dyneither(_tmp78C,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cycstubs_file)){
{const char*_tmp790;void*_tmp78F[1];struct Cyc_String_pa_PrintArg_struct _tmp78E;(_tmp78E.tag=0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp78F[0]=& _tmp78E,Cyc_fprintf(Cyc_stderr,((_tmp790="Error: could not create cycstubs.c in directory %s\n",_tag_dyneither(_tmp790,sizeof(char),52))),_tag_dyneither(_tmp78F,sizeof(void*),1)))))));}
# 1887
return 1;}{
# 1889
const char*_tmp793;void*_tmp792;(_tmp792=0,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),((_tmp793="#include <core.h>\nusing Core;\n\n",_tag_dyneither(_tmp793,sizeof(char),32))),_tag_dyneither(_tmp792,sizeof(void*),0)));};}}{
# 1896
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 1902
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
{const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_fprintf(Cyc_stderr,((_tmp796="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp796,sizeof(char),22))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
exit(1);}}}
# 1910
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1913
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1921
return 0;};};}
