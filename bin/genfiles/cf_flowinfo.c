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
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
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
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 131 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 310
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 371
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 388
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 215 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 225
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 232
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 898 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_RgnOrder_RgnPO;
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
# 130 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 65
void*Cyc_Tcutil_compress(void*t);
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 264 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 102
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 144
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 185
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 188
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 191
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 197
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 202
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 222 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
# 232
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 236
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 241
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 252
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 259
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 263
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 265
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 45 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 49
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp42D;return((_tmp42D.PlaceL).val=x,(((_tmp42D.PlaceL).tag=1,_tmp42D)));}
# 52
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp42E;return((_tmp42E.UnknownL).val=0,(((_tmp42E.UnknownL).tag=2,_tmp42E)));}
# 55
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp42F;return((_tmp42F.BottomFL).val=0,(((_tmp42F.BottomFL).tag=1,_tmp42F)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple12 _tmp432;union Cyc_CfFlowInfo_FlowInfo _tmp431;return((_tmp431.ReachableFL).val=((_tmp432.f1=fd,((_tmp432.f2=r,_tmp432)))),(((_tmp431.ReachableFL).tag=2,_tmp431)));}
# 61
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp469;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp468;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp467;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp466;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp465;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp464;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp463;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp462;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp461;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp460;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp45F;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp45E;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp45D;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp45C;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp45B;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp45A;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp459;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp458;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp457;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp456;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp455;struct Cyc_CfFlowInfo_FlowEnv*_tmp454;return(_tmp454=_region_malloc(r,sizeof(*_tmp454)),((_tmp454->r=r,((_tmp454->zero=(void*)(
# 68
(_tmp456=_region_malloc(r,sizeof(*_tmp456)),((_tmp456[0]=((_tmp455.tag=0,_tmp455)),_tmp456)))),((_tmp454->notzeroall=(void*)(
(_tmp458=_region_malloc(r,sizeof(*_tmp458)),((_tmp458[0]=((_tmp457.tag=1,_tmp457)),_tmp458)))),((_tmp454->notzerothis=(void*)(
(_tmp45A=_region_malloc(r,sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp459.tag=2,_tmp459)),_tmp45A)))),((_tmp454->unknown_none=(void*)(
(_tmp45C=_region_malloc(r,sizeof(*_tmp45C)),((_tmp45C[0]=((_tmp45B.tag=3,((_tmp45B.f1=Cyc_CfFlowInfo_NoneIL,_tmp45B)))),_tmp45C)))),((_tmp454->unknown_this=(void*)(
(_tmp45E=_region_malloc(r,sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp45D.tag=3,((_tmp45D.f1=Cyc_CfFlowInfo_ThisIL,_tmp45D)))),_tmp45E)))),((_tmp454->unknown_all=(void*)(
(_tmp460=_region_malloc(r,sizeof(*_tmp460)),((_tmp460[0]=((_tmp45F.tag=3,((_tmp45F.f1=Cyc_CfFlowInfo_AllIL,_tmp45F)))),_tmp460)))),((_tmp454->esc_none=(void*)(
(_tmp462=_region_malloc(r,sizeof(*_tmp462)),((_tmp462[0]=((_tmp461.tag=4,((_tmp461.f1=Cyc_CfFlowInfo_NoneIL,_tmp461)))),_tmp462)))),((_tmp454->esc_this=(void*)(
(_tmp464=_region_malloc(r,sizeof(*_tmp464)),((_tmp464[0]=((_tmp463.tag=4,((_tmp463.f1=Cyc_CfFlowInfo_ThisIL,_tmp463)))),_tmp464)))),((_tmp454->esc_all=(void*)(
(_tmp466=_region_malloc(r,sizeof(*_tmp466)),((_tmp466[0]=((_tmp465.tag=4,((_tmp465.f1=Cyc_CfFlowInfo_AllIL,_tmp465)))),_tmp466)))),((_tmp454->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp454->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp454->dummy_place=(
(_tmp469=_region_malloc(r,sizeof(*_tmp469)),((_tmp469->root=(void*)((_tmp467=_region_malloc(r,sizeof(*_tmp467)),((_tmp467[0]=((_tmp468.tag=1,((_tmp468.f1=& dummy_exp,((_tmp468.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp468)))))),_tmp467)))),((_tmp469->fields=0,_tmp469)))))),_tmp454)))))))))))))))))))))))))));}
# 85
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 88
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 91
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 93
return 0;}
# 97
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{
void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2:
# 101
 if(_tmp21 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{
# 104
const char*_tmp46D;void*_tmp46C[1];struct Cyc_String_pa_PrintArg_struct _tmp46B;return(struct _dyneither_ptr)((_tmp46B.tag=0,((_tmp46B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp46C[0]=& _tmp46B,Cyc_aprintf(((_tmp46D="reachable from %s",_tag_dyneither(_tmp46D,sizeof(char),18))),_tag_dyneither(_tmp46C,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp470;void*_tmp46F;(_tmp46F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp470="error locations not for VarRoots",_tag_dyneither(_tmp470,sizeof(char),33))),_tag_dyneither(_tmp46F,sizeof(void*),0)));}_LL0:;};}
# 110
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 114
return loc1;}
# 119
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 126
int _tmp2A=0;
struct Cyc_Dict_Dict _tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
# 131
return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};
# 134
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){
void*_tmp31=_tmp30.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){
# 146
int _tmp32=0;_npop_handler(1);return _tmp32;}}{
# 149
int _tmp33=1;_npop_handler(1);return _tmp33;};}
# 138
;_pop_region(temp);}
# 137
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp35=_tmp2D;void*_tmp36;void*_tmp37;_LL6: _tmp36=_tmp35;_LL7:
# 153
 return 1;_LL8: _tmp37=_tmp35;_LL9:(void)_throw(_tmp37);_LL5:;}};};}
# 157
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 163
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp38=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp38,f)== 0)return n;
n=n + 1;}{
# 170
struct Cyc_Core_Impossible_exn_struct _tmp47D;const char*_tmp47C;void*_tmp47B[1];struct Cyc_String_pa_PrintArg_struct _tmp47A;struct Cyc_Core_Impossible_exn_struct*_tmp479;(int)_throw((void*)((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479[0]=((_tmp47D.tag=Cyc_Core_Impossible,((_tmp47D.f1=(struct _dyneither_ptr)((_tmp47A.tag=0,((_tmp47A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp47B[0]=& _tmp47A,Cyc_aprintf(((_tmp47C="get_field_index_fs failed: %s",_tag_dyneither(_tmp47C,sizeof(char),30))),_tag_dyneither(_tmp47B,sizeof(void*),1)))))))),_tmp47D)))),_tmp479)))));};}
# 173
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3E=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp40;struct Cyc_List_List*_tmp42;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E;if(_tmp3F->tag != 11)goto _LLD;else{_tmp40=(_tmp3F->f1).aggr_info;}}_LLC: {
# 176
struct Cyc_List_List*_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp40))->impl))->fields;
_tmp42=_tmp43;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E;if(_tmp41->tag != 12)goto _LLF;else{_tmp42=_tmp41->f2;}}_LLE:
# 179
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10: {
# 181
struct Cyc_Core_Impossible_exn_struct _tmp48A;const char*_tmp489;void*_tmp488[1];struct Cyc_String_pa_PrintArg_struct _tmp487;struct Cyc_Core_Impossible_exn_struct*_tmp486;(int)_throw((void*)((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486[0]=((_tmp48A.tag=Cyc_Core_Impossible,((_tmp48A.f1=(struct _dyneither_ptr)((_tmp487.tag=0,((_tmp487.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp488[0]=& _tmp487,Cyc_aprintf(((_tmp489="get_field_index failed: %s",_tag_dyneither(_tmp489,sizeof(char),27))),_tag_dyneither(_tmp488,sizeof(void*),1)))))))),_tmp48A)))),_tmp486)))));}_LLA:;}
# 186
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp48B;struct _tuple0 _tmp4A=(_tmp48B.f1=r1,((_tmp48B.f2=r2,_tmp48B)));struct Cyc_Absyn_Vardecl*_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4E;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp54;int _tmp58;int _tmp5A;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp4B->tag != 0)goto _LL14;else{_tmp4C=_tmp4B->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp4D->tag != 0)goto _LL14;else{_tmp4E=_tmp4D->f1;}};_LL13:
 return(int)_tmp4C - (int)_tmp4E;_LL14: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp4F->tag != 0)goto _LL16;}_LL15:
 return - 1;_LL16: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp50=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp50->tag != 0)goto _LL18;}_LL17:
 return 1;_LL18:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp51=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp51->tag != 1)goto _LL1A;else{_tmp52=_tmp51->f1;}}{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp53=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp53->tag != 1)goto _LL1A;else{_tmp54=_tmp53->f1;}};_LL19:
 return(int)_tmp52 - (int)_tmp54;_LL1A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp55->tag != 1)goto _LL1C;}_LL1B:
 return - 1;_LL1C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp56=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp56->tag != 1)goto _LL1E;}_LL1D:
 return 1;_LL1E:{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp57->tag != 2)goto _LL11;else{_tmp58=_tmp57->f1;}}{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp59=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp59->tag != 2)goto _LL11;else{_tmp5A=_tmp59->f1;}};_LL1F:
 return _tmp58 - _tmp5A;_LL11:;};}
# 200
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 202
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 205
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
void*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_CfFlowInfo_Place _tmp5B=*p1;_tmp5C=_tmp5B.root;_tmp5D=_tmp5B.fields;{
void*_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_CfFlowInfo_Place _tmp5E=*p2;_tmp5F=_tmp5E.root;_tmp60=_tmp5E.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp5C,_tmp5F);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp5D,_tmp60);};};};}
# 217
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
void*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_CfFlowInfo_Place _tmp61=*p;_tmp62=_tmp61.root;_tmp63=_tmp61.fields;
{void*_tmp64=_tmp62;struct Cyc_Absyn_Vardecl*_tmp66;struct Cyc_Absyn_Exp*_tmp68;int _tmp6A;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp64;if(_tmp65->tag != 0)goto _LL23;else{_tmp66=_tmp65->f1;}}_LL22:
{struct _dyneither_ptr*_tmp498;const char*_tmp497;void*_tmp496[1];struct Cyc_String_pa_PrintArg_struct _tmp495;struct Cyc_List_List*_tmp494;sl=((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494->hd=((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=((_tmp495.tag=0,((_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp66->name).f2),((_tmp496[0]=& _tmp495,Cyc_aprintf(((_tmp497="%s",_tag_dyneither(_tmp497,sizeof(char),3))),_tag_dyneither(_tmp496,sizeof(void*),1)))))))),_tmp498)))),((_tmp494->tl=sl,_tmp494))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp67=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp64;if(_tmp67->tag != 1)goto _LL25;else{_tmp68=_tmp67->f1;}}_LL24:
{struct _dyneither_ptr*_tmp4A5;const char*_tmp4A4;void*_tmp4A3[1];struct Cyc_Int_pa_PrintArg_struct _tmp4A2;struct Cyc_List_List*_tmp4A1;sl=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=((_tmp4A2.tag=1,((_tmp4A2.f1=(unsigned long)((int)_tmp68),((_tmp4A3[0]=& _tmp4A2,Cyc_aprintf(((_tmp4A4="mpt%d",_tag_dyneither(_tmp4A4,sizeof(char),6))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))))),_tmp4A5)))),((_tmp4A1->tl=sl,_tmp4A1))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp69=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp64;if(_tmp69->tag != 2)goto _LL20;else{_tmp6A=_tmp69->f1;}}_LL26:
{struct _dyneither_ptr*_tmp4B2;const char*_tmp4B1;void*_tmp4B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp4AF;struct Cyc_List_List*_tmp4AE;sl=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->hd=((_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4AF.tag=1,((_tmp4AF.f1=(unsigned long)_tmp6A,((_tmp4B0[0]=& _tmp4AF,Cyc_aprintf(((_tmp4B1="param%d",_tag_dyneither(_tmp4B1,sizeof(char),8))),_tag_dyneither(_tmp4B0,sizeof(void*),1)))))))),_tmp4B2)))),((_tmp4AE->tl=sl,_tmp4AE))))));}goto _LL20;_LL20:;}
# 225
for(0;_tmp63 != 0;_tmp63=_tmp63->tl){
struct _dyneither_ptr*_tmp4BF;const char*_tmp4BE;void*_tmp4BD[1];struct Cyc_Int_pa_PrintArg_struct _tmp4BC;struct Cyc_List_List*_tmp4BB;sl=((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB->hd=((_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4BC.tag=1,((_tmp4BC.f1=(unsigned long)((int)_tmp63->hd),((_tmp4BD[0]=& _tmp4BC,Cyc_aprintf(((_tmp4BE="%d",_tag_dyneither(_tmp4BE,sizeof(char),3))),_tag_dyneither(_tmp4BD,sizeof(void*),1)))))))),_tmp4BF)))),((_tmp4BB->tl=sl,_tmp4BB))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp4CA;const char*_tmp4C9;void*_tmp4C8[1];const char*_tmp4C7;struct _dyneither_ptr*_tmp4C6;struct _dyneither_ptr*_tmp7F=(_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4CA.tag=0,((_tmp4CA.f1=(struct _dyneither_ptr)((_tmp4C9="",_tag_dyneither(_tmp4C9,sizeof(char),1))),((_tmp4C8[0]=& _tmp4CA,Cyc_aprintf(((_tmp4C7="%s",_tag_dyneither(_tmp4C7,sizeof(char),3))),_tag_dyneither(_tmp4C8,sizeof(void*),1)))))))),_tmp4C6)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp4CF;void*_tmp4CE[2];struct Cyc_String_pa_PrintArg_struct _tmp4CD;struct Cyc_String_pa_PrintArg_struct _tmp4CC;*_tmp7F=((_tmp4CC.tag=0,((_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F),((_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp4CE[0]=& _tmp4CD,((_tmp4CE[1]=& _tmp4CC,Cyc_aprintf(((_tmp4CF="%s.%s",_tag_dyneither(_tmp4CF,sizeof(char),6))),_tag_dyneither(_tmp4CE,sizeof(void*),2))))))))))))));}
return _tmp7F;};}
# 243 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp4D5(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4D4,unsigned int*_tmp4D3,void***_tmp4D1){for(*_tmp4D4=0;*_tmp4D4 < *_tmp4D3;(*_tmp4D4)++){(*_tmp4D1)[*_tmp4D4]=(*fenv)->unknown_all;}}
# 248
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 254
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp4D4;unsigned int _tmp4D3;struct _dyneither_ptr _tmp4D2;void**_tmp4D1;unsigned int _tmp4D0;struct _dyneither_ptr d=(_tmp4D0=sz,((_tmp4D1=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4D0)),((_tmp4D2=_tag_dyneither(_tmp4D1,sizeof(void*),_tmp4D0),((((_tmp4D3=_tmp4D0,_tmp4D5(& fenv,& _tmp4D4,& _tmp4D3,& _tmp4D1))),_tmp4D2)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct _dyneither_ptr*_tmp8A;void*_tmp8B;struct Cyc_Absyn_Aggrfield*_tmp89=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp8A=_tmp89->name;_tmp8B=_tmp89->type;
if(_get_dyneither_size(*_tmp8A,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp8B,leafval);}}
# 261
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 265
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 269
struct _RegionHandle*_tmp91;struct Cyc_List_List*_tmp92;struct _tuple15*_tmp90=env;_tmp91=_tmp90->f1;_tmp92=_tmp90->f2;{
void*_tmp93=Cyc_Tcutil_rsubstitute(_tmp91,_tmp92,f->type);
struct Cyc_Absyn_Aggrfield*_tmp4D6;return(_tmp4D6=_region_malloc(_tmp91,sizeof(*_tmp4D6)),((_tmp4D6->name=f->name,((_tmp4D6->tq=f->tq,((_tmp4D6->type=_tmp93,((_tmp4D6->width=f->width,((_tmp4D6->attributes=f->attributes,_tmp4D6)))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 278
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 284
struct _RegionHandle _tmp95=_new_region("temp");struct _RegionHandle*temp=& _tmp95;_push_region(temp);
# 288
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp96=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp97=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp98=Cyc_Tcutil_tvar_kind(_tmp96,& Cyc_Tcutil_bk);_LL28: if(_tmp98->kind != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
 goto _LL2B;_LL2A: if(_tmp98->kind != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
 continue;_LL2C:;_LL2D:
 goto _LL27;_LL27:;}{
# 297
struct _tuple16*_tmp4D9;struct Cyc_List_List*_tmp4D8;inst=((_tmp4D8=_region_malloc(temp,sizeof(*_tmp4D8)),((_tmp4D8->hd=((_tmp4D9=_region_malloc(temp,sizeof(*_tmp4D9)),((_tmp4D9->f1=_tmp96,((_tmp4D9->f2=_tmp97,_tmp4D9)))))),((_tmp4D8->tl=inst,_tmp4D8))))));};}
# 299
if(inst != 0){
struct _tuple15 _tmp4DA;struct _tuple15 _tmp9B=(_tmp4DA.f1=temp,((_tmp4DA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp4DA)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp9B,fs);
struct _dyneither_ptr _tmp9C=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp9C;}else{
# 304
struct _dyneither_ptr _tmp9E=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp9E;}}
# 288
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp4E0(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4DF,unsigned int*_tmp4DE,void***_tmp4DC){for(*_tmp4DF=0;*_tmp4DF < *_tmp4DE;(*_tmp4DF)++){(*_tmp4DC)[*_tmp4DF]=(*fenv)->unknown_all;}}
# 308
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 312
int is_tagged=0;
# 314
{void*_tmp9F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Datatypefield*_tmpA1;struct Cyc_List_List*_tmpA3;union Cyc_Absyn_AggrInfoU _tmpA5;struct Cyc_List_List*_tmpA6;enum Cyc_Absyn_AggrKind _tmpA8;struct Cyc_List_List*_tmpA9;void*_tmpAB;union Cyc_Absyn_Constraint*_tmpAC;void*_tmpAE;union Cyc_Absyn_Constraint*_tmpB0;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9F;if(_tmpA0->tag != 4)goto _LL31;else{if((((_tmpA0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL31;_tmpA1=((struct _tuple2)(((_tmpA0->f1).field_info).KnownDatatypefield).val).f2;}}_LL30:
# 316
 if(_tmpA1->typs == 0)
return leafval;
_tmpA3=_tmpA1->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9F;if(_tmpA2->tag != 10)goto _LL33;else{_tmpA3=_tmpA2->f1;}}_LL32: {
# 320
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA3);
# 323
unsigned int _tmp4DF;unsigned int _tmp4DE;struct _dyneither_ptr _tmp4DD;void**_tmp4DC;unsigned int _tmp4DB;struct _dyneither_ptr d=(_tmp4DB=sz,((_tmp4DC=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4DB)),((_tmp4DD=_tag_dyneither(_tmp4DC,sizeof(void*),_tmp4DB),((((_tmp4DE=_tmp4DB,_tmp4E0(& fenv,& _tmp4DF,& _tmp4DE,& _tmp4DC))),_tmp4DD)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpA3))->hd)).f2,leafval);
_tmpA3=_tmpA3->tl;}}{
# 328
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4E6;struct Cyc_CfFlowInfo_UnionRInfo _tmp4E5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4E4;return(void*)((_tmp4E4=_region_malloc(fenv->r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E6.tag=6,((_tmp4E6.f1=((_tmp4E5.is_union=0,((_tmp4E5.fieldnum=- 1,_tmp4E5)))),((_tmp4E6.f2=d,_tmp4E6)))))),_tmp4E4))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F;if(_tmpA4->tag != 11)goto _LL35;else{_tmpA5=(_tmpA4->f1).aggr_info;_tmpA6=(_tmpA4->f1).targs;}}_LL34: {
# 330
struct Cyc_Absyn_Aggrdecl*_tmpB8=Cyc_Absyn_get_known_aggrdecl(_tmpA5);
if(_tmpB8->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->tagged)return fenv->unknown_all;
is_tagged=1;
if(_tmpB8->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpB9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields;
if(_tmpA6 == 0){_tmpA8=_tmpB8->kind;_tmpA9=_tmpB9;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4EC;struct Cyc_CfFlowInfo_UnionRInfo _tmp4EB;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4EA;return(void*)((_tmp4EA=_region_malloc(fenv->r,sizeof(*_tmp4EA)),((_tmp4EA[0]=(
(_tmp4EC.tag=6,((_tmp4EC.f1=((_tmp4EB.is_union=_tmpB8->kind == Cyc_Absyn_UnionA,((_tmp4EB.fieldnum=- 1,_tmp4EB)))),((_tmp4EC.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpB8->tvs,_tmpA6,_tmpB9,_tmpB8->kind == Cyc_Absyn_UnionA,leafval),_tmp4EC)))))),_tmp4EA))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9F;if(_tmpA7->tag != 12)goto _LL37;else{_tmpA8=_tmpA7->f1;_tmpA9=_tmpA7->f2;}}_LL36:
# 343
 if(_tmpA8 == Cyc_Absyn_UnionA  && !is_tagged)return fenv->unknown_all;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F2;struct Cyc_CfFlowInfo_UnionRInfo _tmp4F1;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F0;return(void*)((_tmp4F0=_region_malloc(fenv->r,sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4F2.tag=6,((_tmp4F2.f1=((_tmp4F1.is_union=_tmpA8 == Cyc_Absyn_UnionA,((_tmp4F1.fieldnum=- 1,_tmp4F1)))),((_tmp4F2.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpA9,_tmpA8 == Cyc_Absyn_UnionA,leafval),_tmp4F2)))))),_tmp4F0))));};_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F;if(_tmpAA->tag != 8)goto _LL39;else{_tmpAB=(_tmpAA->f1).elt_type;_tmpAC=(_tmpAA->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAC))goto _LL39;_LL38:
# 348
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpAB)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9F;if(_tmpAD->tag != 19)goto _LL3B;else{_tmpAE=(void*)_tmpAD->f1;}}_LL3A:
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F;if(_tmpAF->tag != 5)goto _LL3D;else{_tmpB0=((_tmpAF->f1).ptr_atts).nullable;}}if(!(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0)))goto _LL3D;_LL3C:
{void*_tmpC0=leafval;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0;if(_tmpC1->tag != 3)goto _LL42;else{if(_tmpC1->f1 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0;if(_tmpC2->tag != 3)goto _LL44;else{if(_tmpC2->f1 != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
 return fenv->notzeroall;_LL44:;_LL45:
 goto _LL3F;_LL3F:;}
# 356
goto _LL2E;_LL3D:;_LL3E:
 goto _LL2E;_LL2E:;}
# 360
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 363
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 369
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpC3=r;struct Cyc_Absyn_Exp*_tmpC5;int _tmpC6;void*_tmpC7;int _tmpC9;int _tmpCA;struct _dyneither_ptr _tmpCB;void*_tmpCD;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC4->tag != 7)goto _LL49;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;_tmpC7=(void*)_tmpC4->f3;}}_LL48:
# 372
 if(_tmpC5 == e  && _tmpC6 == env_iteration){
*needs_unconsume=1;
return 0;}
# 376
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC8->tag != 6)goto _LL4B;else{_tmpC9=(_tmpC8->f1).is_union;_tmpCA=(_tmpC8->f1).fieldnum;_tmpCB=_tmpC8->f2;}}_LL4A:
# 378
 if(!_tmpC9  || _tmpCA == - 1){
unsigned int _tmpCE=_get_dyneither_size(_tmpCB,sizeof(void*));
{int i=0;for(0;i < _tmpCE;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCB,sizeof(void*),i)),needs_unconsume))
return 1;}}
# 384
return 0;}else{
# 387
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCB,sizeof(void*),_tmpCA)),needs_unconsume);}_LL4B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpCC->tag != 8)goto _LL4D;else{_tmpCD=(void*)_tmpCC->f2;}}_LL4C:
# 389
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpCD,needs_unconsume);_LL4D:;_LL4E:
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp4F8(struct _dyneither_ptr*_tmpD6,unsigned int*_tmp4F7,unsigned int*_tmp4F6,void***_tmp4F4){for(*_tmp4F7=0;*_tmp4F7 < *_tmp4F6;(*_tmp4F7)++){(*_tmp4F4)[*_tmp4F7]=*((void**)_check_dyneither_subscript(*_tmpD6,sizeof(void*),(int)*_tmp4F7));}}
# 396
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpCF=r;struct Cyc_Absyn_Exp*_tmpD1;int _tmpD2;void*_tmpD3;struct Cyc_CfFlowInfo_UnionRInfo _tmpD5;struct _dyneither_ptr _tmpD6;struct Cyc_Absyn_Vardecl*_tmpD8;void*_tmpD9;_LL50: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF;if(_tmpD0->tag != 7)goto _LL52;else{_tmpD1=_tmpD0->f1;_tmpD2=_tmpD0->f2;_tmpD3=(void*)_tmpD0->f3;}}_LL51:
# 399
 return _tmpD3;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF;if(_tmpD4->tag != 6)goto _LL54;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;}}_LL53: {
# 401
unsigned int _tmpDA=_get_dyneither_size(_tmpD6,sizeof(void*));
int change=0;
unsigned int _tmp4F7;unsigned int _tmp4F6;struct _dyneither_ptr _tmp4F5;void**_tmp4F4;unsigned int _tmp4F3;struct _dyneither_ptr d2=(_tmp4F3=_tmpDA,((_tmp4F4=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F3)),((_tmp4F5=_tag_dyneither(_tmp4F4,sizeof(void*),_tmp4F3),((((_tmp4F6=_tmp4F3,_tmp4F8(& _tmpD6,& _tmp4F7,& _tmp4F6,& _tmp4F4))),_tmp4F5)))))));
{int i=0;for(0;i < _tmpDA;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i)));
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i)))
change=1;}}
# 409
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4FB;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4FA;return(void*)((_tmp4FA=_region_malloc(fenv->r,sizeof(*_tmp4FA)),((_tmp4FA[0]=((_tmp4FB.tag=6,((_tmp4FB.f1=_tmpD5,((_tmp4FB.f2=d2,_tmp4FB)))))),_tmp4FA))));}else{
return r;}}_LL54: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF;if(_tmpD7->tag != 8)goto _LL56;else{_tmpD8=_tmpD7->f1;_tmpD9=(void*)_tmpD7->f2;}}_LL55: {
# 413
void*_tmpE1=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpD9);
if(_tmpE1 != _tmpD9){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4FE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4FD;return(void*)((_tmp4FD=_region_malloc(fenv->r,sizeof(*_tmp4FD)),((_tmp4FD[0]=((_tmp4FE.tag=8,((_tmp4FE.f1=_tmpD8,((_tmp4FE.f2=_tmpE1,_tmp4FE)))))),_tmp4FD))));}else{
return r;}}_LL56:;_LL57:
 return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp508(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp507,unsigned int*_tmp506,void***_tmp504){for(*_tmp507=0;*_tmp507 < *_tmp506;(*_tmp507)++){(*_tmp504)[*_tmp507]=(*fenv)->unknown_all;}}static void _tmp511(struct _dyneither_ptr*_tmpF8,unsigned int*_tmp510,unsigned int*_tmp50F,void***_tmp50D){for(*_tmp510=0;*_tmp510 < *_tmp50F;(*_tmp510)++){(*_tmp50D)[*_tmp510]=((void**)(*_tmpF8).curr)[(int)*_tmp510];}}
# 423
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp4FF;struct _tuple0 _tmpE5=(_tmp4FF.f1=Cyc_Tcutil_compress(t),((_tmp4FF.f2=r,_tmp4FF)));struct Cyc_Absyn_Vardecl*_tmpE7;void*_tmpE8;struct Cyc_List_List*_tmpEA;struct Cyc_CfFlowInfo_UnionRInfo _tmpEC;struct _dyneither_ptr _tmpED;union Cyc_Absyn_AggrInfoU _tmpEF;struct Cyc_CfFlowInfo_UnionRInfo _tmpF1;struct _dyneither_ptr _tmpF2;enum Cyc_Absyn_AggrKind _tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_CfFlowInfo_UnionRInfo _tmpF7;struct _dyneither_ptr _tmpF8;_LL59: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE5.f2;if(_tmpE6->tag != 8)goto _LL5B;else{_tmpE7=_tmpE6->f1;_tmpE8=(void*)_tmpE6->f2;}}_LL5A: {
# 426
void*_tmpF9=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpE8);
if(_tmpF9 != _tmpE8){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp502;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp501;return(void*)((_tmp501=_region_malloc(fenv->r,sizeof(*_tmp501)),((_tmp501[0]=((_tmp502.tag=8,((_tmp502.f1=_tmpE7,((_tmp502.f2=_tmpF9,_tmp502)))))),_tmp501))));}else{
return r;}}_LL5B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE5.f1;if(_tmpE9->tag != 10)goto _LL5D;else{_tmpEA=_tmpE9->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE5.f2;if(_tmpEB->tag != 6)goto _LL5D;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;}};_LL5C: {
# 430
unsigned int _tmpFC=_get_dyneither_size(_tmpED,sizeof(void*));
unsigned int _tmp507;unsigned int _tmp506;struct _dyneither_ptr _tmp505;void**_tmp504;unsigned int _tmp503;struct _dyneither_ptr d2=(_tmp503=_tmpFC,((_tmp504=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp503)),((_tmp505=_tag_dyneither(_tmp504,sizeof(void*),_tmp503),((((_tmp506=_tmp503,_tmp508(& fenv,& _tmp507,& _tmp506,& _tmp504))),_tmp505)))))));
{int i=0;for(0;i < _tmpFC;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpEA))->hd)).f2,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpED,sizeof(void*),i)));
_tmpEA=_tmpEA->tl;}}{
# 436
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp50B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp50A;return(void*)((_tmp50A=_region_malloc(fenv->r,sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp50B.tag=6,((_tmp50B.f1=_tmpEC,((_tmp50B.f2=d2,_tmp50B)))))),_tmp50A))));};}_LL5D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE5.f1;if(_tmpEE->tag != 11)goto _LL5F;else{_tmpEF=(_tmpEE->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE5.f2;if(_tmpF0->tag != 6)goto _LL5F;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}};_LL5E: {
# 438
struct Cyc_Absyn_Aggrdecl*_tmp103=Cyc_Absyn_get_known_aggrdecl(_tmpEF);
if(_tmp103->impl == 0)return r;
_tmpF4=_tmp103->kind;_tmpF5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp103->impl))->fields;_tmpF7=_tmpF1;_tmpF8=_tmpF2;goto _LL60;}_LL5F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE5.f1;if(_tmpF3->tag != 12)goto _LL61;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE5.f2;if(_tmpF6->tag != 6)goto _LL61;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}};_LL60: {
# 442
unsigned int _tmp510;unsigned int _tmp50F;struct _dyneither_ptr _tmp50E;void**_tmp50D;unsigned int _tmp50C;struct _dyneither_ptr d2=(_tmp50C=_get_dyneither_size(_tmpF8,sizeof(void*)),((_tmp50D=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp50C)),((_tmp50E=_tag_dyneither(_tmp50D,sizeof(void*),_tmp50C),((((_tmp50F=_tmp50C,_tmp511(& _tmpF8,& _tmp510,& _tmp50F,& _tmp50D))),_tmp50E)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF5);
{int i=0;for(0;i < sz;(i ++,_tmpF5=_tmpF5->tl)){
struct _dyneither_ptr*_tmp105;void*_tmp106;struct Cyc_Absyn_Aggrfield*_tmp104=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpF5))->hd;_tmp105=_tmp104->name;_tmp106=_tmp104->type;
if(_get_dyneither_size(*_tmp105,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp106,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF8,sizeof(void*),i)));}}{
# 449
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp514;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp513;return(void*)((_tmp513=_region_malloc(fenv->r,sizeof(*_tmp513)),((_tmp513[0]=((_tmp514.tag=6,((_tmp514.f1=_tmpF7,((_tmp514.f2=d2,_tmp514)))))),_tmp513))));};}_LL61:;_LL62:
# 451
 if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp517;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp516;return(void*)((_tmp516=_region_malloc(fenv->r,sizeof(*_tmp516)),((_tmp516[0]=((_tmp517.tag=7,((_tmp517.f1=consumer,((_tmp517.f2=iteration,((_tmp517.f3=r,_tmp517)))))))),_tmp516))));}else{
return r;}_LL58:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 457
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 460
struct _RegionHandle _tmp10F=_new_region("r");struct _RegionHandle*r=& _tmp10F;_push_region(r);
{struct _tuple18 _tmp518;struct _tuple18 elem=(_tmp518.f1=place,((_tmp518.f2=0,_tmp518)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp110=elem.f1;
void*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_CfFlowInfo_Place _tmp111=*place;_tmp112=_tmp111.root;_tmp113=_tmp111.fields;{
void*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_CfFlowInfo_Place _tmp114=*_tmp110;_tmp115=_tmp114.root;_tmp116=_tmp114.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmp112,_tmp115)!= 0)
continue;
for(0;_tmp113 != 0  && _tmp116 != 0;(_tmp113=_tmp113->tl,_tmp116=_tmp116->tl)){
if((int)_tmp113->hd != (int)_tmp116->hd)break;}
if(_tmp113 == 0){
int _tmp117=1;_npop_handler(0);return _tmp117;}};}{
# 474
int _tmp118=0;_npop_handler(0);return _tmp118;};}
# 461
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 487
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 489
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 492
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp519;pile->places=((_tmp519=_region_malloc(pile->rgn,sizeof(*_tmp519)),((_tmp519->hd=place,((_tmp519->tl=pile->places,_tmp519))))));}}
# 495
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp11B=r;void*_tmp11D;void*_tmp11F;struct Cyc_CfFlowInfo_Place*_tmp121;struct Cyc_CfFlowInfo_UnionRInfo _tmp123;struct _dyneither_ptr _tmp124;_LL64: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp11C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp11B;if(_tmp11C->tag != 7)goto _LL66;else{_tmp11D=(void*)_tmp11C->f3;}}_LL65:
 Cyc_CfFlowInfo_add_places(pile,_tmp11D);return;_LL66: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11B;if(_tmp11E->tag != 8)goto _LL68;else{_tmp11F=(void*)_tmp11E->f2;}}_LL67:
 Cyc_CfFlowInfo_add_places(pile,_tmp11F);return;_LL68: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp120=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp11B;if(_tmp120->tag != 5)goto _LL6A;else{_tmp121=_tmp120->f1;}}_LL69:
 Cyc_CfFlowInfo_add_place(pile,_tmp121);return;_LL6A: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp122=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp11B;if(_tmp122->tag != 6)goto _LL6C;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL6B:
# 501
{int i=0;for(0;i < _get_dyneither_size(_tmp124,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp124.curr)[i]);}}
return;_LL6C:;_LL6D:
 return;_LL63:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp51F(struct _RegionHandle**r,void**new_val,unsigned int*_tmp51E,unsigned int*_tmp51D,void***_tmp51B,struct _dyneither_ptr*_tmp128){for(*_tmp51E=0;*_tmp51E < *_tmp51D;(*_tmp51E)++){(*_tmp51B)[*_tmp51E]=
# 516
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp128).curr)[(int)*_tmp51E]);}}
# 511
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 513
void*_tmp125=old_val;int _tmp127;struct _dyneither_ptr _tmp128;struct Cyc_Absyn_Exp*_tmp12A;int _tmp12B;void*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12E;void*_tmp12F;_LL6F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp126=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp125;if(_tmp126->tag != 6)goto _LL71;else{_tmp127=(_tmp126->f1).is_union;_tmp128=_tmp126->f2;}}_LL70: {
# 515
unsigned int _tmp51E;unsigned int _tmp51D;struct _dyneither_ptr _tmp51C;void**_tmp51B;unsigned int _tmp51A;struct _dyneither_ptr d2=(_tmp51A=_get_dyneither_size(_tmp128,sizeof(void*)),((_tmp51B=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp51A)),((_tmp51C=_tag_dyneither(_tmp51B,sizeof(void*),_tmp51A),((((_tmp51D=_tmp51A,_tmp51F(& r,& new_val,& _tmp51E,& _tmp51D,& _tmp51B,& _tmp128))),_tmp51C)))))));
# 519
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp525;struct Cyc_CfFlowInfo_UnionRInfo _tmp524;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp523;return(void*)((_tmp523=_region_malloc(r,sizeof(*_tmp523)),((_tmp523[0]=((_tmp525.tag=6,((_tmp525.f1=((_tmp524.is_union=_tmp127,((_tmp524.fieldnum=- 1,_tmp524)))),((_tmp525.f2=d2,_tmp525)))))),_tmp523))));}_LL71: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp129=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp125;if(_tmp129->tag != 7)goto _LL73;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;_tmp12C=(void*)_tmp129->f3;}}_LL72: {
# 521
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp528;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp527;return(void*)((_tmp527=_region_malloc(r,sizeof(*_tmp527)),((_tmp527[0]=((_tmp528.tag=7,((_tmp528.f1=_tmp12A,((_tmp528.f2=_tmp12B,((_tmp528.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp12C),_tmp528)))))))),_tmp527))));}_LL73: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp125;if(_tmp12D->tag != 8)goto _LL75;else{_tmp12E=_tmp12D->f1;_tmp12F=(void*)_tmp12D->f2;}}_LL74: {
# 523
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp52B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp52A;return(void*)((_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52B.tag=8,((_tmp52B.f1=_tmp12E,((_tmp52B.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp12F),_tmp52B)))))),_tmp52A))));}_LL75:;_LL76:
 return new_val;_LL6E:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp531(struct _dyneither_ptr*d,unsigned int*_tmp530,unsigned int*_tmp52F,void***_tmp52D){for(*_tmp530=0;*_tmp530 < *_tmp52F;(*_tmp530)++){(*_tmp52D)[*_tmp530]=((void**)(*d).curr)[(int)*_tmp530];}}
# 530
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 532
void*_tmp13B=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp13B == rval)return d;{
unsigned int _tmp530;unsigned int _tmp52F;struct _dyneither_ptr _tmp52E;void**_tmp52D;unsigned int _tmp52C;struct _dyneither_ptr res=(_tmp52C=_get_dyneither_size(d,sizeof(void*)),((_tmp52D=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp52C)),((_tmp52E=_tag_dyneither(_tmp52D,sizeof(void*),_tmp52C),((((_tmp52F=_tmp52C,_tmp531(& d,& _tmp530,& _tmp52F,& _tmp52D))),_tmp52E)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 544
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 548
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp532;struct _tuple19 _tmp141=(_tmp532.f1=fs,((_tmp532.f2=old_val,_tmp532)));int _tmp142;struct Cyc_List_List*_tmp143;int _tmp145;struct _dyneither_ptr _tmp146;struct Cyc_Absyn_Vardecl*_tmp148;void*_tmp149;_LL78: if(_tmp141.f1 == 0)goto _LL7A;_tmp142=(int)(_tmp141.f1)->hd;_tmp143=(_tmp141.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp144=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp141.f2;if(_tmp144->tag != 6)goto _LL7A;else{_tmp145=(_tmp144->f1).is_union;_tmp146=_tmp144->f2;}};_LL79: {
# 552
void*_tmp14A=Cyc_CfFlowInfo_insert_place_outer(r,_tmp143,*((void**)_check_dyneither_subscript(_tmp146,sizeof(void*),_tmp142)),new_val);
struct _dyneither_ptr _tmp14B=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp146,_tmp142,_tmp14A);
if((void**)_tmp14B.curr == (void**)_tmp146.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp538;struct Cyc_CfFlowInfo_UnionRInfo _tmp537;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp536;return(void*)((_tmp536=_region_malloc(r,sizeof(*_tmp536)),((_tmp536[0]=((_tmp538.tag=6,((_tmp538.f1=((_tmp537.is_union=_tmp145,((_tmp537.fieldnum=- 1,_tmp537)))),((_tmp538.f2=_tmp14B,_tmp538)))))),_tmp536))));}}_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp147=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp141.f2;if(_tmp147->tag != 8)goto _LL7C;else{_tmp148=_tmp147->f1;_tmp149=(void*)_tmp147->f2;}}_LL7B: {
# 557
void*_tmp14F=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp149,new_val);
if(_tmp14F == _tmp149)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp53B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp53A;return(void*)((_tmp53A=_region_malloc(r,sizeof(*_tmp53A)),((_tmp53A[0]=((_tmp53B.tag=8,((_tmp53B.f1=_tmp148,((_tmp53B.f2=_tmp14F,_tmp53B)))))),_tmp53A))));}}_LL7C:;_LL7D: {
struct Cyc_Core_Impossible_exn_struct _tmp541;const char*_tmp540;struct Cyc_Core_Impossible_exn_struct*_tmp53F;(int)_throw((void*)((_tmp53F=_cycalloc(sizeof(*_tmp53F)),((_tmp53F[0]=((_tmp541.tag=Cyc_Core_Impossible,((_tmp541.f1=((_tmp540="bad insert place",_tag_dyneither(_tmp540,sizeof(char),17))),_tmp541)))),_tmp53F)))));}_LL77:;};}
# 567
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 571
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp155=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp155,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp156;_push_handler(& _tmp156);{int _tmp158=0;if(setjmp(_tmp156.handler))_tmp158=1;if(!_tmp158){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp155);;_pop_handler();}else{void*_tmp157=(void*)_exn_thrown;void*_tmp15A=_tmp157;void*_tmp15C;_LL7F: {struct Cyc_Dict_Absent_exn_struct*_tmp15B=(struct Cyc_Dict_Absent_exn_struct*)_tmp15A;if(_tmp15B->tag != Cyc_Dict_Absent)goto _LL81;}_LL80:
 continue;_LL81: _tmp15C=_tmp15A;_LL82:(void)_throw(_tmp15C);_LL7E:;}};}
switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL83:
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL84:
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL85:
 newval=fenv->esc_none;break;}
# 583
Cyc_CfFlowInfo_add_places(pile,oldval);{
void*_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_CfFlowInfo_Place _tmp15D=*_tmp155;_tmp15E=_tmp15D.root;_tmp15F=_tmp15D.fields;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp15E,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp15F,
# 588
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp15E),newval));};};}
# 591
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 603
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp160=r;enum Cyc_CfFlowInfo_InitLevel _tmp162;enum Cyc_CfFlowInfo_InitLevel _tmp164;void*_tmp169;_LL88: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp161=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp161->tag != 3)goto _LL8A;else{_tmp162=_tmp161->f1;}}_LL89:
 return _tmp162;_LL8A: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp163=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp163->tag != 4)goto _LL8C;else{_tmp164=_tmp163->f1;}}_LL8B:
 return _tmp164;_LL8C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp165=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp165->tag != 0)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp166=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp166->tag != 1)goto _LL90;}_LL8F:
 return Cyc_CfFlowInfo_AllIL;_LL90: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp167=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp167->tag != 2)goto _LL92;}_LL91:
 return Cyc_CfFlowInfo_ThisIL;_LL92: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp168=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp160;if(_tmp168->tag != 7)goto _LL94;else{_tmp169=(void*)_tmp168->f3;}}_LL93:
 return Cyc_CfFlowInfo_NoneIL;_LL94:;_LL95: {
struct Cyc_Core_Impossible_exn_struct _tmp547;const char*_tmp546;struct Cyc_Core_Impossible_exn_struct*_tmp545;(int)_throw((void*)((_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545[0]=((_tmp547.tag=Cyc_Core_Impossible,((_tmp547.f1=((_tmp546="initlevel_approx",_tag_dyneither(_tmp546,sizeof(char),17))),_tmp547)))),_tmp545)))));}_LL87:;}
# 614
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 616
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp16D=r;void*_tmp16F;int _tmp171;struct _dyneither_ptr _tmp172;struct _dyneither_ptr _tmp174;struct Cyc_CfFlowInfo_Place*_tmp176;_LL97: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp16E->tag != 8)goto _LL99;else{_tmp16F=(void*)_tmp16E->f2;}}_LL98:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp16F,acc);_LL99: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp170=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp170->tag != 6)goto _LL9B;else{if((_tmp170->f1).is_union != 1)goto _LL9B;_tmp171=(_tmp170->f1).fieldnum;_tmp172=_tmp170->f2;}}_LL9A: {
# 623
unsigned int _tmp177=_get_dyneither_size(_tmp172,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp171 == - 1){
int i=0;for(0;i < _tmp177;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 633
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),_tmp171)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL96;}_LL9B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp173=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp173->tag != 6)goto _LL9D;else{if((_tmp173->f1).is_union != 0)goto _LL9D;_tmp174=_tmp173->f2;}}_LL9C: {
# 637
unsigned int _tmp178=_get_dyneither_size(_tmp174,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp178;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp174,sizeof(void*),i)),this_ans);}}
goto _LL96;}_LL9D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp175=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp175->tag != 5)goto _LL9F;else{_tmp176=_tmp175->f1;}}_LL9E:
# 643
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp176))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 646
{struct Cyc_List_List*_tmp548;env->seen=((_tmp548=_region_malloc(env->e,sizeof(*_tmp548)),((_tmp548->hd=_tmp176,((_tmp548->tl=env->seen,_tmp548))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp176),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 652
goto _LL96;_LL9F:;_LLA0:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL96:;}
# 655
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 659
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp17A=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp549;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17B=(_tmp549.e=_tmp17A,((_tmp549.d=d,((_tmp549.seen=0,_tmp549)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp17B,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 666
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
void*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_CfFlowInfo_Place _tmp17D=*place;_tmp17E=_tmp17D.root;_tmp17F=_tmp17D.fields;{
void*_tmp180=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp17E);
for(0;_tmp17F != 0;_tmp17F=_tmp17F->tl){
struct _tuple20 _tmp54A;struct _tuple20 _tmp182=(_tmp54A.f1=_tmp180,((_tmp54A.f2=(int)_tmp17F->hd,_tmp54A)));struct Cyc_CfFlowInfo_UnionRInfo _tmp185;struct _dyneither_ptr _tmp186;int _tmp187;struct Cyc_CfFlowInfo_UnionRInfo _tmp189;struct _dyneither_ptr _tmp18A;int _tmp18B;_LLA2:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp183=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp182.f1;if(_tmp183->tag != 8)goto _LLA4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp184=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp183->f2);if(_tmp184->tag != 6)goto _LLA4;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}}_tmp187=_tmp182.f2;_LLA3:
# 672
 _tmp189=_tmp185;_tmp18A=_tmp186;_tmp18B=_tmp187;goto _LLA5;_LLA4:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp188=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp182.f1;if(_tmp188->tag != 6)goto _LLA6;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;}}_tmp18B=_tmp182.f2;_LLA5:
# 674
 _tmp180=*((void**)_check_dyneither_subscript(_tmp18A,sizeof(void*),_tmp18B));
goto _LLA1;_LLA6:;_LLA7: {
# 680
struct Cyc_Core_Impossible_exn_struct _tmp550;const char*_tmp54F;struct Cyc_Core_Impossible_exn_struct*_tmp54E;(int)_throw((void*)((_tmp54E=_cycalloc(sizeof(*_tmp54E)),((_tmp54E[0]=((_tmp550.tag=Cyc_Core_Impossible,((_tmp550.f1=((_tmp54F="bad lookup_place",_tag_dyneither(_tmp54F,sizeof(char),17))),_tmp550)))),_tmp54E)))));}_LLA1:;}
# 682
return _tmp180;};}
# 685
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp18F=rval;void*_tmp192;void*_tmp194;int _tmp196;int _tmp197;struct _dyneither_ptr _tmp198;_LLA9: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp190=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp18F;if(_tmp190->tag != 4)goto _LLAB;}_LLAA:
 return 0;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp191=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp18F;if(_tmp191->tag != 7)goto _LLAD;else{_tmp192=(void*)_tmp191->f3;}}_LLAC:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp192);_LLAD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp193=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp18F;if(_tmp193->tag != 8)goto _LLAF;else{_tmp194=(void*)_tmp193->f2;}}_LLAE:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp194);_LLAF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp195=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp18F;if(_tmp195->tag != 6)goto _LLB1;else{_tmp196=(_tmp195->f1).is_union;_tmp197=(_tmp195->f1).fieldnum;_tmp198=_tmp195->f2;}}_LLB0:
# 691
 if(_tmp196  && _tmp197 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp198,sizeof(void*),_tmp197)));else{
# 694
unsigned int sz=_get_dyneither_size(_tmp198,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp198.curr)[i]))return 0;}}
return 1;}_LLB1:;_LLB2:
# 699
 return 1;_LLA8:;}
# 702
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 707
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp199=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp551;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp551=_region_malloc(_tmp199,sizeof(*_tmp551)),((_tmp551->rgn=_tmp199,((_tmp551->places=0,_tmp551)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp55E(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp55D,unsigned int*_tmp55C,void***_tmp55A,struct _dyneither_ptr*_tmp1AB,struct _dyneither_ptr*_tmp1A8){for(*_tmp55D=0;*_tmp55D < *_tmp55C;(*_tmp55D)++){(*_tmp55A)[*_tmp55D]=
# 745
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1A8).curr)[(int)*_tmp55D],*((void**)_check_dyneither_subscript(*_tmp1AB,sizeof(void*),(int)*_tmp55D)));}}
# 725
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 730
struct _tuple0 _tmp552;struct _tuple0 _tmp19C=(_tmp552.f1=oldval,((_tmp552.f2=newval,_tmp552)));void*_tmp19E;struct Cyc_Absyn_Vardecl*_tmp1A0;void*_tmp1A1;struct Cyc_CfFlowInfo_Place*_tmp1A4;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A7;struct _dyneither_ptr _tmp1A8;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AA;struct _dyneither_ptr _tmp1AB;enum Cyc_CfFlowInfo_InitLevel _tmp1AD;_LLB4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp19D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1;if(_tmp19D->tag != 8)goto _LLB6;else{_tmp19E=(void*)_tmp19D->f2;}}_LLB5:
# 732
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp19E,newval);_LLB6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp19F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2;if(_tmp19F->tag != 8)goto _LLB8;else{_tmp1A0=_tmp19F->f1;_tmp1A1=(void*)_tmp19F->f2;}}_LLB7: {
# 734
void*_tmp1AE=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp1A1);
if(_tmp1AE == _tmp1A1)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp555;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp554;return(void*)((_tmp554=_region_malloc((env->fenv)->r,sizeof(*_tmp554)),((_tmp554[0]=((_tmp555.tag=8,((_tmp555.f1=_tmp1A0,((_tmp555.f2=_tmp1AE,_tmp555)))))),_tmp554))));}}_LLB8:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A2=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1;if(_tmp1A2->tag != 4)goto _LLBA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1A3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2;if(_tmp1A3->tag != 5)goto _LLBA;else{_tmp1A4=_tmp1A3->f1;}};_LLB9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A4);goto _LLBB;_LLBA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A5=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1;if(_tmp1A5->tag != 4)goto _LLBC;}_LLBB:
# 739
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp558;void*_tmp557;(_tmp557=0,Cyc_Tcutil_terr(env->loc,((_tmp558="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp558,sizeof(char),67))),_tag_dyneither(_tmp557,sizeof(void*),0)));}
# 742
return(env->fenv)->esc_all;_LLBC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1A6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f1;if(_tmp1A6->tag != 6)goto _LLBE;else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A6->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1A9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2;if(_tmp1A9->tag != 6)goto _LLBE;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}};_LLBD: {
# 744
unsigned int _tmp55D;unsigned int _tmp55C;struct _dyneither_ptr _tmp55B;void**_tmp55A;unsigned int _tmp559;struct _dyneither_ptr new_d=
(_tmp559=_get_dyneither_size(_tmp1A8,sizeof(void*)),((_tmp55A=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp559)),((_tmp55B=_tag_dyneither(_tmp55A,sizeof(void*),_tmp559),((((_tmp55C=_tmp559,_tmp55E(& env,& _tmp55D,& _tmp55C,& _tmp55A,& _tmp1AB,& _tmp1A8))),_tmp55B)))))));
# 748
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1A8,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A8.curr)[i]){
change=1;break;}}}
# 753
if(!change){
if(!_tmp1A7.is_union)return oldval;
new_d=_tmp1A8;}else{
# 758
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1A8,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1AB,sizeof(void*),i))){
change=1;break;}}}
# 763
if(!change){
if(!_tmp1A7.is_union)return newval;
new_d=_tmp1AB;}}{
# 768
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp561;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp560;return(void*)((_tmp560=_region_malloc((env->fenv)->r,sizeof(*_tmp560)),((_tmp560[0]=((_tmp561.tag=6,((_tmp561.f1=_tmp1AA,((_tmp561.f2=new_d,_tmp561)))))),_tmp560))));};}_LLBE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1AC=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19C.f2;if(_tmp1AC->tag != 4)goto _LLC0;else{_tmp1AD=_tmp1AC->f1;}}_LLBF:
# 770
 switch(_tmp1AD){case Cyc_CfFlowInfo_NoneIL: _LLC2:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLC3:
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLC4:
 return(env->fenv)->unknown_all;}_LLC0:;_LLC1:
# 775
 return newval;_LLB3:;}
# 781
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 785
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 790
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp562;struct _tuple19 _tmp1BA=(_tmp562.f1=fs,((_tmp562.f2=oldval,_tmp562)));struct Cyc_Absyn_Vardecl*_tmp1BC;void*_tmp1BD;int _tmp1BE;struct Cyc_List_List*_tmp1BF;int _tmp1C1;int _tmp1C2;struct _dyneither_ptr _tmp1C3;_LLC7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1BB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1BA.f2;if(_tmp1BB->tag != 8)goto _LLC9;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=(void*)_tmp1BB->f2;}}_LLC8: {
# 793
void*_tmp1C4=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp1BD,newval);
if(_tmp1C4 == _tmp1BD)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp565;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp564;return(void*)((_tmp564=_region_malloc((env->fenv)->r,sizeof(*_tmp564)),((_tmp564[0]=((_tmp565.tag=8,((_tmp565.f1=_tmp1BC,((_tmp565.f2=_tmp1C4,_tmp565)))))),_tmp564))));}}_LLC9: if(_tmp1BA.f1 == 0)goto _LLCB;_tmp1BE=(int)(_tmp1BA.f1)->hd;_tmp1BF=(_tmp1BA.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1C0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1BA.f2;if(_tmp1C0->tag != 6)goto _LLCB;else{_tmp1C1=(_tmp1C0->f1).is_union;_tmp1C2=(_tmp1C0->f1).fieldnum;_tmp1C3=_tmp1C0->f2;}};_LLCA: {
# 797
void*_tmp1C7=Cyc_CfFlowInfo_assign_place_outer(env,_tmp1BF,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1C3,sizeof(void*),_tmp1BE)),newval);
# 799
struct _dyneither_ptr _tmp1C8=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C3,_tmp1BE,_tmp1C7);
if((void**)_tmp1C8.curr == (void**)_tmp1C3.curr  && (!_tmp1C1  || _tmp1C2 == _tmp1BE))return oldval;
_tmp1C3=_tmp1C8;
# 804
if(_tmp1C1){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp1C3,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp1BE){
struct _dyneither_ptr _tmp1C9=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C3,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp1C3,sizeof(void*),i))));
# 814
if((void**)_tmp1C9.curr != (void**)_tmp1C3.curr){
_tmp1C3=_tmp1C9;
changed=1;}}}}
# 822
if(changed){
void*_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_CfFlowInfo_Place*_tmp1CA=env->root_place;_tmp1CB=_tmp1CA->root;_tmp1CC=_tmp1CA->fields;{
int*_tmp566;struct Cyc_List_List*_tmp1CD=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp566=_cycalloc_atomic(sizeof(*_tmp566)),((_tmp566[0]=fs_prefix,_tmp566)))),_tmp1CC);
struct Cyc_CfFlowInfo_Place*_tmp567;struct Cyc_CfFlowInfo_Place*_tmp1CE=(_tmp567=_region_malloc((env->pile)->rgn,sizeof(*_tmp567)),((_tmp567->root=_tmp1CB,((_tmp567->fields=_tmp1CD,_tmp567)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1CE,0);};}}{
# 829
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp56D;struct Cyc_CfFlowInfo_UnionRInfo _tmp56C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp56B;return(void*)((_tmp56B=_region_malloc((env->fenv)->r,sizeof(*_tmp56B)),((_tmp56B[0]=((_tmp56D.tag=6,((_tmp56D.f1=((_tmp56C.is_union=_tmp1C1,((_tmp56C.fieldnum=_tmp1BE,_tmp56C)))),((_tmp56D.f2=_tmp1C3,_tmp56D)))))),_tmp56B))));};}_LLCB:;_LLCC: {
struct Cyc_Core_Impossible_exn_struct _tmp573;const char*_tmp572;struct Cyc_Core_Impossible_exn_struct*_tmp571;(int)_throw((void*)((_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571[0]=((_tmp573.tag=Cyc_Core_Impossible,((_tmp573.f1=((_tmp572="bad assign place",_tag_dyneither(_tmp572,sizeof(char),17))),_tmp573)))),_tmp571)))));}_LLC6:;};}
# 833
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 844
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1D7=fenv->r;
void*_tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_CfFlowInfo_Place*_tmp1D8=place;_tmp1D9=_tmp1D8->root;_tmp1DA=_tmp1D8->fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp576;struct Cyc_CfFlowInfo_AssignEnv _tmp575;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp575.fenv=fenv,((_tmp575.pile=((_tmp576=_region_malloc(_tmp1D7,sizeof(*_tmp576)),((_tmp576->rgn=_tmp1D7,((_tmp576->places=0,_tmp576)))))),((_tmp575.d=d,((_tmp575.loc=loc,((_tmp575.root_place=place,((_tmp575.all_changed=all_changed,_tmp575)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1DA,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1D9),r);
# 852
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1D9,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 863
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 873
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 875
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp589(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp588,unsigned int*_tmp587,void***_tmp585,struct _dyneither_ptr*_tmp21B,struct _dyneither_ptr*_tmp217){for(*_tmp588=0;*_tmp588 < *_tmp587;(*_tmp588)++){(*_tmp585)[*_tmp588]=
# 949 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp217).curr)[(int)*_tmp588],*((void**)_check_dyneither_subscript(*_tmp21B,sizeof(void*),(int)*_tmp588)));}}
# 885
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1DD=(env->fenv)->r;
# 889
{struct _tuple0 _tmp577;struct _tuple0 _tmp1DF=(_tmp577.f1=r1,((_tmp577.f2=r2,_tmp577)));struct Cyc_Absyn_Vardecl*_tmp1E1;void*_tmp1E2;struct Cyc_Absyn_Vardecl*_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1EA;void*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1ED;int _tmp1EE;void*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;int _tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;int _tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F9;int _tmp1FA;void*_tmp1FB;struct Cyc_CfFlowInfo_Place*_tmp1FD;struct Cyc_CfFlowInfo_Place*_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp201;struct Cyc_CfFlowInfo_Place*_tmp205;struct Cyc_CfFlowInfo_Place*_tmp207;struct Cyc_CfFlowInfo_Place*_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp20D;struct Cyc_CfFlowInfo_Place*_tmp20F;int _tmp215;int _tmp216;struct _dyneither_ptr _tmp217;int _tmp219;int _tmp21A;struct _dyneither_ptr _tmp21B;_LLCE:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp1E0->tag != 8)goto _LLD0;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=(void*)_tmp1E0->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp1E3->tag != 8)goto _LLD0;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=(void*)_tmp1E3->f2;}};_LLCF:
# 892
 if(_tmp1E1 == _tmp1E4){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp57A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp579;return(void*)((_tmp579=_region_malloc(_tmp1DD,sizeof(*_tmp579)),((_tmp579[0]=((_tmp57A.tag=8,((_tmp57A.f1=_tmp1E1,((_tmp57A.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E2,_tmp1E5),_tmp57A)))))),_tmp579))));}else{
# 895
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E2,_tmp1E5);}_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp1E6->tag != 8)goto _LLD2;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=(void*)_tmp1E6->f2;}}_LLD1:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E8,r2);_LLD2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp1E9->tag != 8)goto _LLD4;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=(void*)_tmp1E9->f2;}}_LLD3:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EB);_LLD4:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1EC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp1EC->tag != 7)goto _LLD6;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;_tmp1EF=(void*)_tmp1EC->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp1F0->tag != 7)goto _LLD6;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;_tmp1F3=(void*)_tmp1F0->f3;}};_LLD5:
# 900
 if(_tmp1ED == _tmp1F1  && _tmp1EE == _tmp1F2){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp57D;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp57C;return(void*)((_tmp57C=_region_malloc(_tmp1DD,sizeof(*_tmp57C)),((_tmp57C[0]=((_tmp57D.tag=7,((_tmp57D.f1=_tmp1ED,((_tmp57D.f2=_tmp1EE,((_tmp57D.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1EF,_tmp1F3),_tmp57D)))))))),_tmp57C))));}
{void*_tmp220=_tmp1EF;struct Cyc_CfFlowInfo_Place*_tmp222;_LLF1: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp221=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp220;if(_tmp221->tag != 5)goto _LLF3;else{_tmp222=_tmp221->f1;}}_LLF2:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp222);goto _LLF0;_LLF3:;_LLF4:
 goto _LLF0;_LLF0:;}
# 906
{void*_tmp223=_tmp1F3;struct Cyc_CfFlowInfo_Place*_tmp225;_LLF6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp224=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp223;if(_tmp224->tag != 5)goto _LLF8;else{_tmp225=_tmp224->f1;}}_LLF7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp225);goto _LLF5;_LLF8:;_LLF9:
 goto _LLF5;_LLF5:;}
# 910
goto _LLCD;_LLD6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp1F4->tag != 7)goto _LLD8;else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;_tmp1F7=(void*)_tmp1F4->f3;}}_LLD7: {
# 912
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp580;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp57F;return(void*)((_tmp57F=_region_malloc(_tmp1DD,sizeof(*_tmp57F)),((_tmp57F[0]=((_tmp580.tag=7,((_tmp580.f1=_tmp1F5,((_tmp580.f2=_tmp1F6,((_tmp580.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F7,r2),_tmp580)))))))),_tmp57F))));}_LLD8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp1F8->tag != 7)goto _LLDA;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;_tmp1FB=(void*)_tmp1F8->f3;}}_LLD9: {
# 914
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp583;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp582;return(void*)((_tmp582=_region_malloc(_tmp1DD,sizeof(*_tmp582)),((_tmp582[0]=((_tmp583.tag=7,((_tmp583.f1=_tmp1F9,((_tmp583.f2=_tmp1FA,((_tmp583.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1FB),_tmp583)))))))),_tmp582))));}_LLDA:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp1FC->tag != 5)goto _LLDC;else{_tmp1FD=_tmp1FC->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp1FE->tag != 5)goto _LLDC;else{_tmp1FF=_tmp1FE->f1;}};_LLDB:
# 917
 if(Cyc_CfFlowInfo_place_cmp(_tmp1FD,_tmp1FF)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1FD);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1FF);
goto _LLCD;_LLDC:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp200=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp200->tag != 5)goto _LLDE;else{_tmp201=_tmp200->f1;}}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp202=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp202->tag != 2)goto _LLDE;};_LLDD:
# 922
 _tmp205=_tmp201;goto _LLDF;_LLDE:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp203=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp203->tag != 2)goto _LLE0;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp204=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp204->tag != 5)goto _LLE0;else{_tmp205=_tmp204->f1;}};_LLDF:
# 924
 Cyc_CfFlowInfo_add_place(env->pile,_tmp205);
return(env->fenv)->notzerothis;_LLE0:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp206=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp206->tag != 5)goto _LLE2;else{_tmp207=_tmp206->f1;}}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp208=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp208->tag != 1)goto _LLE2;};_LLE1:
# 928
 Cyc_CfFlowInfo_add_place(env->pile,_tmp207);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLFA:
 return(env->fenv)->notzeroall;default: _LLFB:
 return(env->fenv)->notzerothis;}_LLE2:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp209=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp209->tag != 1)goto _LLE4;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp20A->tag != 5)goto _LLE4;else{_tmp20B=_tmp20A->f1;}};_LLE3:
# 934
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20B);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL: _LLFD:
 return(env->fenv)->notzeroall;default: _LLFE:
 return(env->fenv)->notzerothis;}_LLE4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp20C->tag != 5)goto _LLE6;else{_tmp20D=_tmp20C->f1;}}_LLE5:
# 940
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20D);goto _LLCD;_LLE6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp20E->tag != 5)goto _LLE8;else{_tmp20F=_tmp20E->f1;}}_LLE7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20F);goto _LLCD;_LLE8:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp210=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp210->tag != 1)goto _LLEA;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp211=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp211->tag != 2)goto _LLEA;};_LLE9:
# 944
 goto _LLEB;_LLEA:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp212=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp212->tag != 2)goto _LLEC;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp213=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp213->tag != 1)goto _LLEC;};_LLEB:
 return(env->fenv)->notzerothis;_LLEC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp214=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f1;if(_tmp214->tag != 6)goto _LLEE;else{_tmp215=(_tmp214->f1).is_union;_tmp216=(_tmp214->f1).fieldnum;_tmp217=_tmp214->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp218=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1DF.f2;if(_tmp218->tag != 6)goto _LLEE;else{_tmp219=(_tmp218->f1).is_union;_tmp21A=(_tmp218->f1).fieldnum;_tmp21B=_tmp218->f2;}};_LLED: {
# 948
unsigned int _tmp588;unsigned int _tmp587;struct _dyneither_ptr _tmp586;void**_tmp585;unsigned int _tmp584;struct _dyneither_ptr new_d=
(_tmp584=_get_dyneither_size(_tmp217,sizeof(void*)),((_tmp585=(void**)_region_malloc(_tmp1DD,_check_times(sizeof(void*),_tmp584)),((_tmp586=_tag_dyneither(_tmp585,sizeof(void*),_tmp584),((((_tmp587=_tmp584,_tmp589(& env,& _tmp588,& _tmp587,& _tmp585,& _tmp21B,& _tmp217))),_tmp586)))))));
# 953
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp217,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp217.curr)[i]){
change=1;break;}}}
# 958
if(!change){
if(!_tmp215)return r1;
new_d=_tmp217;}else{
# 963
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp217,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp21B,sizeof(void*),i))){
change=1;break;}}}
# 968
if(!change){
if(!_tmp215)return r2;
new_d=_tmp21B;}}{
# 973
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp58F;struct Cyc_CfFlowInfo_UnionRInfo _tmp58E;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp58D;return(void*)((_tmp58D=_region_malloc(_tmp1DD,sizeof(*_tmp58D)),((_tmp58D[0]=((_tmp58F.tag=6,((_tmp58F.f1=((_tmp58E.is_union=_tmp215,((_tmp58E.fieldnum=_tmp216 == _tmp21A?_tmp216: - 1,_tmp58E)))),((_tmp58F.f2=new_d,_tmp58F)))))),_tmp58D))));};}_LLEE:;_LLEF:
# 975
 goto _LLCD;_LLCD:;}{
# 977
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp590;struct _tuple0 _tmp232=(_tmp590.f1=r1,((_tmp590.f2=r2,_tmp590)));_LL101: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp233=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp232.f1;if(_tmp233->tag != 4)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp234=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp232.f2;if(_tmp234->tag != 4)goto _LL105;}_LL104: {
# 982
struct _tuple21 _tmp591;struct _tuple21 _tmp236=(_tmp591.f1=il1,((_tmp591.f2=il2,_tmp591)));_LL108: if(_tmp236.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL10A;_LL109:
 goto _LL10B;_LL10A: if(_tmp236.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL10C;_LL10B: return(env->fenv)->esc_none;_LL10C: if(_tmp236.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL10E;_LL10D:
 goto _LL10F;_LL10E: if(_tmp236.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL110;_LL10F: return(env->fenv)->esc_this;_LL110:;_LL111:
 return(env->fenv)->esc_all;_LL107:;}_LL105:;_LL106: {
# 988
struct _tuple21 _tmp592;struct _tuple21 _tmp238=(_tmp592.f1=il1,((_tmp592.f2=il2,_tmp592)));_LL113: if(_tmp238.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL115;_LL114:
 goto _LL116;_LL115: if(_tmp238.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL117;_LL116: return(env->fenv)->unknown_none;_LL117: if(_tmp238.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL119;_LL118:
 goto _LL11A;_LL119: if(_tmp238.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL11B;_LL11A: return(env->fenv)->unknown_this;_LL11B:;_LL11C:
 return(env->fenv)->unknown_all;_LL112:;}_LL100:;};};}
# 996
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp593;return((_tmp593.EqualConst).val=i,(((_tmp593.EqualConst).tag=1,_tmp593)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*v,void*t){
struct _tuple11 _tmp596;union Cyc_CfFlowInfo_RelnOp _tmp595;return((_tmp595.LessVar).val=((_tmp596.f1=v,((_tmp596.f2=t,_tmp596)))),(((_tmp595.LessVar).tag=2,_tmp595)));}
# 1000
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp597;return((_tmp597.LessNumelts).val=v,(((_tmp597.LessNumelts).tag=3,_tmp597)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp598;return((_tmp598.LessConst).val=i,(((_tmp598.LessConst).tag=4,_tmp598)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp599;return((_tmp599.LessEqConst).val=i,(((_tmp599.LessEqConst).tag=6,_tmp599)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp59A;return((_tmp59A.LessEqNumelts).val=v,(((_tmp59A.LessEqNumelts).tag=5,_tmp59A)));}struct _tuple22{union Cyc_CfFlowInfo_RelnOp f1;union Cyc_CfFlowInfo_RelnOp f2;};
# 1008
static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp r1,union Cyc_CfFlowInfo_RelnOp r2){
struct _tuple22 _tmp59B;struct _tuple22 _tmp241=(_tmp59B.f1=r1,((_tmp59B.f2=r2,_tmp59B)));unsigned int _tmp242;unsigned int _tmp243;struct Cyc_Absyn_Vardecl*_tmp244;struct Cyc_Absyn_Vardecl*_tmp245;struct Cyc_Absyn_Vardecl*_tmp246;struct Cyc_Absyn_Vardecl*_tmp247;unsigned int _tmp248;unsigned int _tmp249;unsigned int _tmp24A;unsigned int _tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;struct Cyc_Absyn_Vardecl*_tmp24D;_LL11E: if(((_tmp241.f1).EqualConst).tag != 1)goto _LL120;_tmp242=(unsigned int)((_tmp241.f1).EqualConst).val;if(((_tmp241.f2).EqualConst).tag != 1)goto _LL120;_tmp243=(unsigned int)((_tmp241.f2).EqualConst).val;_LL11F:
 return _tmp242 == _tmp243;_LL120: if(((_tmp241.f1).LessVar).tag != 2)goto _LL122;_tmp244=((struct _tuple11)((_tmp241.f1).LessVar).val).f1;if(((_tmp241.f2).LessVar).tag != 2)goto _LL122;_tmp245=((struct _tuple11)((_tmp241.f2).LessVar).val).f1;_LL121:
 return _tmp244 == _tmp245;_LL122: if(((_tmp241.f1).LessNumelts).tag != 3)goto _LL124;_tmp246=(struct Cyc_Absyn_Vardecl*)((_tmp241.f1).LessNumelts).val;if(((_tmp241.f2).LessNumelts).tag != 3)goto _LL124;_tmp247=(struct Cyc_Absyn_Vardecl*)((_tmp241.f2).LessNumelts).val;_LL123:
 return _tmp246 == _tmp247;_LL124: if(((_tmp241.f1).LessConst).tag != 4)goto _LL126;_tmp248=(unsigned int)((_tmp241.f1).LessConst).val;if(((_tmp241.f2).LessConst).tag != 4)goto _LL126;_tmp249=(unsigned int)((_tmp241.f2).LessConst).val;_LL125:
 return _tmp248 == _tmp249;_LL126: if(((_tmp241.f1).LessEqConst).tag != 6)goto _LL128;_tmp24A=(unsigned int)((_tmp241.f1).LessEqConst).val;if(((_tmp241.f2).LessEqConst).tag != 6)goto _LL128;_tmp24B=(unsigned int)((_tmp241.f2).LessEqConst).val;_LL127:
 return _tmp24A == _tmp24B;_LL128: if(((_tmp241.f1).LessEqNumelts).tag != 5)goto _LL12A;_tmp24C=(struct Cyc_Absyn_Vardecl*)((_tmp241.f1).LessEqNumelts).val;if(((_tmp241.f2).LessEqNumelts).tag != 5)goto _LL12A;_tmp24D=(struct Cyc_Absyn_Vardecl*)((_tmp241.f2).LessEqNumelts).val;_LL129:
 return _tmp24C == _tmp24D;_LL12A:;_LL12B:
 return 0;_LL11D:;}
# 1019
static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
# 1023
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp24E=r1s;for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp24F=(struct Cyc_CfFlowInfo_Reln*)_tmp24E->hd;
int found=0;
{struct Cyc_List_List*_tmp250=r2s;for(0;_tmp250 != 0;_tmp250=_tmp250->tl){
struct Cyc_CfFlowInfo_Reln*_tmp251=(struct Cyc_CfFlowInfo_Reln*)_tmp250->hd;
if(_tmp24F == _tmp251  || _tmp24F->vd == _tmp251->vd  && Cyc_CfFlowInfo_same_relop(_tmp24F->rop,_tmp251->rop)){
{struct Cyc_List_List*_tmp59C;res=((_tmp59C=_region_malloc(r,sizeof(*_tmp59C)),((_tmp59C->hd=_tmp24F,((_tmp59C->tl=res,_tmp59C))))));}
found=1;
break;}}}
# 1036
if(!found)diff=1;}}
# 1038
if(!diff)res=r1s;
# 1040
return res;};}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 1043
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1046
struct _tuple23 _tmp59D;struct _tuple23 _tmp254=(_tmp59D.f1=f1,((_tmp59D.f2=f2,_tmp59D)));struct Cyc_Dict_Dict _tmp255;struct Cyc_List_List*_tmp256;struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp258;_LL12D: if(((_tmp254.f1).BottomFL).tag != 1)goto _LL12F;_LL12E:
 return f2;_LL12F: if(((_tmp254.f2).BottomFL).tag != 1)goto _LL131;_LL130:
 return f1;_LL131: if(((_tmp254.f1).ReachableFL).tag != 2)goto _LL12C;_tmp255=((struct _tuple12)((_tmp254.f1).ReachableFL).val).f1;_tmp256=((struct _tuple12)((_tmp254.f1).ReachableFL).val).f2;if(((_tmp254.f2).ReachableFL).tag != 2)goto _LL12C;_tmp257=((struct _tuple12)((_tmp254.f2).ReachableFL).val).f1;_tmp258=((struct _tuple12)((_tmp254.f2).ReachableFL).val).f2;_LL132:
# 1052
 if(_tmp255.t == _tmp257.t  && _tmp256 == _tmp258)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp259=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp5A0;struct Cyc_CfFlowInfo_JoinEnv _tmp59F;struct Cyc_CfFlowInfo_JoinEnv _tmp25A=(_tmp59F.fenv=fenv,((_tmp59F.pile=((_tmp5A0=_region_malloc(_tmp259,sizeof(*_tmp5A0)),((_tmp5A0->rgn=_tmp259,((_tmp5A0->places=0,_tmp5A0)))))),((_tmp59F.d1=_tmp255,((_tmp59F.d2=_tmp257,_tmp59F)))))));
struct Cyc_Dict_Dict _tmp25B=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp25A,_tmp255,_tmp257);
struct Cyc_List_List*_tmp25C=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp256,_tmp258);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp25A.pile,all_changed,_tmp25B),_tmp25C);};_LL12C:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1064
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1071
union Cyc_CfFlowInfo_FlowInfo _tmp260;void*_tmp261;struct _tuple13 _tmp25F=pr1;_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp263;void*_tmp264;struct _tuple13 _tmp262=pr2;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp260,_tmp263);
struct _tuple24 _tmp5A1;struct _tuple24 _tmp266=(_tmp5A1.f1=_tmp260,((_tmp5A1.f2=_tmp263,((_tmp5A1.f3=outflow,_tmp5A1)))));struct Cyc_Dict_Dict _tmp267;struct Cyc_Dict_Dict _tmp268;struct Cyc_Dict_Dict _tmp269;struct Cyc_List_List*_tmp26A;_LL134: if(((_tmp266.f1).BottomFL).tag != 1)goto _LL136;_LL135: {
struct _tuple13 _tmp5A2;return(_tmp5A2.f1=outflow,((_tmp5A2.f2=_tmp264,_tmp5A2)));}_LL136: if(((_tmp266.f2).BottomFL).tag != 1)goto _LL138;_LL137: {
struct _tuple13 _tmp5A3;return(_tmp5A3.f1=outflow,((_tmp5A3.f2=_tmp261,_tmp5A3)));}_LL138: if(((_tmp266.f1).ReachableFL).tag != 2)goto _LL13A;_tmp267=((struct _tuple12)((_tmp266.f1).ReachableFL).val).f1;if(((_tmp266.f2).ReachableFL).tag != 2)goto _LL13A;_tmp268=((struct _tuple12)((_tmp266.f2).ReachableFL).val).f1;if(((_tmp266.f3).ReachableFL).tag != 2)goto _LL13A;_tmp269=((struct _tuple12)((_tmp266.f3).ReachableFL).val).f1;_tmp26A=((struct _tuple12)((_tmp266.f3).ReachableFL).val).f2;_LL139:
# 1079
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp261,_tmp264)){struct _tuple13 _tmp5A4;return(_tmp5A4.f1=outflow,((_tmp5A4.f2=_tmp264,_tmp5A4)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp264,_tmp261)){struct _tuple13 _tmp5A5;return(_tmp5A5.f1=outflow,((_tmp5A5.f2=_tmp261,_tmp5A5)));}{
struct _RegionHandle*_tmp26F=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp5A8;struct Cyc_CfFlowInfo_JoinEnv _tmp5A7;struct Cyc_CfFlowInfo_JoinEnv _tmp270=(_tmp5A7.fenv=fenv,((_tmp5A7.pile=((_tmp5A8=_region_malloc(_tmp26F,sizeof(*_tmp5A8)),((_tmp5A8->rgn=_tmp26F,((_tmp5A8->places=0,_tmp5A8)))))),((_tmp5A7.d1=_tmp267,((_tmp5A7.d2=_tmp268,_tmp5A7)))))));
void*_tmp271=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp270,0,_tmp261,_tmp264);
struct _tuple13 _tmp5A9;return(_tmp5A9.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp270.pile,all_changed,_tmp269),_tmp26A),((_tmp5A9.f2=_tmp271,_tmp5A9)));};_LL13A:;_LL13B: {
# 1087
struct Cyc_Core_Impossible_exn_struct _tmp5AF;const char*_tmp5AE;struct Cyc_Core_Impossible_exn_struct*_tmp5AD;(int)_throw((void*)((_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5AF.tag=Cyc_Core_Impossible,((_tmp5AF.f1=((_tmp5AE="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp5AE,sizeof(char),37))),_tmp5AF)))),_tmp5AD)))));}_LL133:;};};}
# 1091
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5B9(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp5B8,unsigned int*_tmp5B7,void***_tmp5B5,struct _dyneither_ptr*_tmp28B,struct _dyneither_ptr*_tmp287){for(*_tmp5B8=0;*_tmp5B8 < *_tmp5B7;(*_tmp5B8)++){(*_tmp5B5)[*_tmp5B8]=
# 1132
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp5B8,((void**)(*_tmp287).curr)[(int)*_tmp5B8],*((void**)_check_dyneither_subscript(*_tmp28B,sizeof(void*),(int)*_tmp5B8)));}}
# 1092
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1100
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1105
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1113
struct _tuple0 _tmp5B0;struct _tuple0 _tmp279=(_tmp5B0.f1=r1,((_tmp5B0.f2=r2,_tmp5B0)));struct Cyc_Absyn_Vardecl*_tmp27D;void*_tmp27E;struct Cyc_Absyn_Vardecl*_tmp280;void*_tmp281;int _tmp285;int _tmp286;struct _dyneither_ptr _tmp287;int _tmp289;int _tmp28A;struct _dyneither_ptr _tmp28B;_LL13D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp27A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp279.f1;if(_tmp27A->tag != 7)goto _LL13F;}_LL13E:
# 1115
 goto _LL140;_LL13F: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp27B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp279.f2;if(_tmp27B->tag != 7)goto _LL141;}_LL140:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL141:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp279.f1;if(_tmp27C->tag != 8)goto _LL143;else{_tmp27D=_tmp27C->f1;_tmp27E=(void*)_tmp27C->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp279.f2;if(_tmp27F->tag != 8)goto _LL143;else{_tmp280=_tmp27F->f1;_tmp281=(void*)_tmp27F->f2;}};_LL142:
# 1118
 if(_tmp27D == _tmp280){
void*_tmp28C=Cyc_CfFlowInfo_after_absRval(env,_tmp27E,_tmp281);
if(_tmp28C == _tmp27E)return r1;
if(_tmp28C == _tmp281)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5B3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B2;return(void*)((_tmp5B2=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5B2)),((_tmp5B2[0]=((_tmp5B3.tag=8,((_tmp5B3.f1=_tmp27D,((_tmp5B3.f2=_tmp28C,_tmp5B3)))))),_tmp5B2))));};}else{
# 1124
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}_LL143: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp282=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp279.f2;if(_tmp282->tag != 8)goto _LL145;}_LL144:
 goto _LL146;_LL145: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp283=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp279.f1;if(_tmp283->tag != 8)goto _LL147;}_LL146:
# 1127
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL147:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp284=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp279.f1;if(_tmp284->tag != 6)goto _LL149;else{_tmp285=(_tmp284->f1).is_union;_tmp286=(_tmp284->f1).fieldnum;_tmp287=_tmp284->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp288=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp279.f2;if(_tmp288->tag != 6)goto _LL149;else{_tmp289=(_tmp288->f1).is_union;_tmp28A=(_tmp288->f1).fieldnum;_tmp28B=_tmp288->f2;}};_LL148: {
# 1130
unsigned int _tmp5B8;unsigned int _tmp5B7;struct _dyneither_ptr _tmp5B6;void**_tmp5B5;unsigned int _tmp5B4;struct _dyneither_ptr new_d=
(_tmp5B4=
_get_dyneither_size(_tmp287,sizeof(void*)),((_tmp5B5=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp5B4)),((_tmp5B6=_tag_dyneither(_tmp5B5,sizeof(void*),_tmp5B4),((((_tmp5B7=_tmp5B4,_tmp5B9(& env,& _tmp5B8,& _tmp5B7,& _tmp5B5,& _tmp28B,& _tmp287))),_tmp5B6)))))));
# 1137
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp287,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp287.curr)[i]){
change=1;break;}}}
# 1142
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp287,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp28B,sizeof(void*),i))){
change=1;break;}}}
# 1149
if(!change)
return r2;{
# 1153
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5BF;struct Cyc_CfFlowInfo_UnionRInfo _tmp5BE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5BD;return(void*)((_tmp5BD=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5BD)),((_tmp5BD[0]=((_tmp5BF.tag=6,((_tmp5BF.f1=((_tmp5BE.is_union=_tmp285,((_tmp5BE.fieldnum=_tmp286 == _tmp28A?_tmp286: - 1,_tmp5BE)))),((_tmp5BF.f2=new_d,_tmp5BF)))))),_tmp5BD))));};}_LL149:;_LL14A: {
# 1161
struct Cyc_Core_Impossible_exn_struct _tmp5C5;const char*_tmp5C4;struct Cyc_Core_Impossible_exn_struct*_tmp5C3;(int)_throw((void*)((_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((_tmp5C3[0]=((_tmp5C5.tag=Cyc_Core_Impossible,((_tmp5C5.f1=((_tmp5C4="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5C4,sizeof(char),33))),_tmp5C5)))),_tmp5C3)))));}_LL13C:;};}
# 1164
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1167
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp299=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp29A=env->changed;
{struct Cyc_List_List*_tmp5C6;*env->last_field_cell=((_tmp5C6=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5C6)),((_tmp5C6->hd=(void*)key,((_tmp5C6->tl=0,_tmp5C6))))));}
# 1173
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp29C=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp299;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp29A;
return _tmp29C;};};}
# 1180
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1182
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp5C7;*env->curr_place=((_tmp5C7.root=root,((_tmp5C7.fields=0,_tmp5C7))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1189
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1195
struct _tuple23 _tmp5C8;struct _tuple23 _tmp29F=(_tmp5C8.f1=f1,((_tmp5C8.f2=f2,_tmp5C8)));struct Cyc_Dict_Dict _tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*_tmp2A3;_LL14C: if(((_tmp29F.f1).BottomFL).tag != 1)goto _LL14E;_LL14D:
 goto _LL14F;_LL14E: if(((_tmp29F.f2).BottomFL).tag != 1)goto _LL150;_LL14F:
 return Cyc_CfFlowInfo_BottomFL();_LL150: if(((_tmp29F.f1).ReachableFL).tag != 2)goto _LL14B;_tmp2A0=((struct _tuple12)((_tmp29F.f1).ReachableFL).val).f1;_tmp2A1=((struct _tuple12)((_tmp29F.f1).ReachableFL).val).f2;if(((_tmp29F.f2).ReachableFL).tag != 2)goto _LL14B;_tmp2A2=((struct _tuple12)((_tmp29F.f2).ReachableFL).val).f1;_tmp2A3=((struct _tuple12)((_tmp29F.f2).ReachableFL).val).f2;_LL151:
# 1199
 if(_tmp2A0.t == _tmp2A2.t  && _tmp2A1 == _tmp2A3)return f1;{
# 1202
struct _RegionHandle*_tmp2A4=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp2A5=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp5CC;struct Cyc_CfFlowInfo_AfterEnv _tmp5CB;struct Cyc_CfFlowInfo_AfterEnv _tmp2A6=(_tmp5CB.joinenv=(((_tmp5CB.joinenv).fenv=fenv,(((_tmp5CB.joinenv).pile=((_tmp5CC=_region_malloc(_tmp2A4,sizeof(*_tmp5CC)),((_tmp5CC->rgn=_tmp2A4,((_tmp5CC->places=0,_tmp5CC)))))),(((_tmp5CB.joinenv).d1=_tmp2A0,(((_tmp5CB.joinenv).d2=_tmp2A2,_tmp5CB.joinenv)))))))),((_tmp5CB.chg1=chg1,((_tmp5CB.chg2=chg2,((_tmp5CB.curr_place=_tmp2A5,((_tmp5CB.last_field_cell=& _tmp2A5->fields,((_tmp5CB.changed=Cyc_CfFlowInfo_Neither,_tmp5CB)))))))))));
# 1208
struct Cyc_Dict_Dict _tmp2A7=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2A6,_tmp2A0,_tmp2A2);
struct Cyc_List_List*_tmp2A8=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp2A1,_tmp2A3);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2A6.joinenv).pile,all_changed,_tmp2A7),_tmp2A8);};_LL14B:;}
# 1217
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1221
{struct _tuple0 _tmp5CD;struct _tuple0 _tmp2AC=(_tmp5CD.f1=r1,((_tmp5CD.f2=r2,_tmp5CD)));struct Cyc_Absyn_Vardecl*_tmp2AE;void*_tmp2AF;struct Cyc_Absyn_Vardecl*_tmp2B1;void*_tmp2B2;struct Cyc_CfFlowInfo_Place*_tmp2B6;struct Cyc_CfFlowInfo_Place*_tmp2B8;struct Cyc_CfFlowInfo_UnionRInfo _tmp2BC;struct _dyneither_ptr _tmp2BD;struct Cyc_CfFlowInfo_UnionRInfo _tmp2BF;struct _dyneither_ptr _tmp2C0;_LL153:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2AD->tag != 8)goto _LL155;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=(void*)_tmp2AD->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2B0->tag != 8)goto _LL155;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=(void*)_tmp2B0->f2;}};_LL154:
# 1223
 return _tmp2AE == _tmp2B1  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp2AF,_tmp2B2);_LL155: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2B3->tag != 8)goto _LL157;}_LL156:
 goto _LL158;_LL157: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2B4->tag != 8)goto _LL159;}_LL158:
 return 0;_LL159:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2B5->tag != 5)goto _LL15B;else{_tmp2B6=_tmp2B5->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2B7->tag != 5)goto _LL15B;else{_tmp2B8=_tmp2B7->f1;}};_LL15A:
 return Cyc_CfFlowInfo_place_cmp(_tmp2B6,_tmp2B8)== 0;_LL15B: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B9=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2B9->tag != 5)goto _LL15D;}_LL15C:
 goto _LL15E;_LL15D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2BA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2BA->tag != 5)goto _LL15F;}_LL15E:
 return 0;_LL15F:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2BB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2BB->tag != 6)goto _LL161;else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2BE=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2BE->tag != 6)goto _LL161;else{_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;}};_LL160:
# 1230
 if((void**)_tmp2BD.curr == (void**)_tmp2C0.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp2BD,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2BD.curr)[i],*((void**)_check_dyneither_subscript(_tmp2C0,sizeof(void*),i))))return 0;}}
return 1;_LL161:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2C1=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2C1->tag != 1)goto _LL163;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2C2=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2C2->tag != 2)goto _LL163;};_LL162:
 return 1;_LL163: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2C3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2C3->tag != 2)goto _LL165;}_LL164:
 return 0;_LL165: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp2C4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2C4->tag != 0)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2C5=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2C5->tag != 1)goto _LL169;}_LL168:
 return 0;_LL169:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2C6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2C6->tag != 4)goto _LL16B;}{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2C7=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f2;if(_tmp2C7->tag != 4)goto _LL16B;};_LL16A:
 goto _LL152;_LL16B: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2C8=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2AC.f1;if(_tmp2C8->tag != 4)goto _LL16D;}_LL16C:
 return 0;_LL16D:;_LL16E:
 goto _LL152;_LL152:;}{
# 1242
struct _tuple21 _tmp5CE;struct _tuple21 _tmp2CA=(_tmp5CE.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5CE.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp5CE)));_LL170: if(_tmp2CA.f1 != Cyc_CfFlowInfo_AllIL)goto _LL172;if(_tmp2CA.f2 != Cyc_CfFlowInfo_AllIL)goto _LL172;_LL171:
 return 1;_LL172: if(_tmp2CA.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL174;_LL173:
 return 1;_LL174: if(_tmp2CA.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL176;_LL175:
 return 0;_LL176: if(_tmp2CA.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL178;_LL177:
 return 1;_LL178: if(_tmp2CA.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL16F;_LL179:
 return 0;_LL16F:;};}
# 1251
static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 1255
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2CB=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp2CC=r2s;for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2CD=(struct Cyc_CfFlowInfo_Reln*)_tmp2CC->hd;
if(_tmp2CB == _tmp2CD  || _tmp2CB->vd == _tmp2CD->vd  && Cyc_CfFlowInfo_same_relop(_tmp2CB->rop,_tmp2CD->rop)){
found=1;
break;}}}
# 1265
if(!found)
return 0;}
# 1268
return 1;}
# 1276
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1278
struct _tuple23 _tmp5CF;struct _tuple23 _tmp2CF=(_tmp5CF.f1=f1,((_tmp5CF.f2=f2,_tmp5CF)));struct Cyc_Dict_Dict _tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_Dict_Dict _tmp2D2;struct Cyc_List_List*_tmp2D3;_LL17B: if(((_tmp2CF.f1).BottomFL).tag != 1)goto _LL17D;_LL17C:
 return 1;_LL17D: if(((_tmp2CF.f2).BottomFL).tag != 1)goto _LL17F;_LL17E:
 return 0;_LL17F: if(((_tmp2CF.f1).ReachableFL).tag != 2)goto _LL17A;_tmp2D0=((struct _tuple12)((_tmp2CF.f1).ReachableFL).val).f1;_tmp2D1=((struct _tuple12)((_tmp2CF.f1).ReachableFL).val).f2;if(((_tmp2CF.f2).ReachableFL).tag != 2)goto _LL17A;_tmp2D2=((struct _tuple12)((_tmp2CF.f2).ReachableFL).val).f1;_tmp2D3=((struct _tuple12)((_tmp2CF.f2).ReachableFL).val).f2;_LL180:
# 1282
 if(_tmp2D0.t == _tmp2D2.t  && _tmp2D1 == _tmp2D3)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2D0,_tmp2D2) && 
Cyc_CfFlowInfo_relns_approx(_tmp2D1,_tmp2D3);_LL17A:;}
# 1288
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2D4=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(_tmp2D4->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp2D5=_tmp2D4->rop;struct Cyc_Absyn_Vardecl*_tmp2D6;struct Cyc_Absyn_Vardecl*_tmp2D7;struct Cyc_Absyn_Vardecl*_tmp2D8;_LL182: if((_tmp2D5.LessVar).tag != 2)goto _LL184;_tmp2D6=((struct _tuple11)(_tmp2D5.LessVar).val).f1;_LL183:
 _tmp2D7=_tmp2D6;goto _LL185;_LL184: if((_tmp2D5.LessNumelts).tag != 3)goto _LL186;_tmp2D7=(struct Cyc_Absyn_Vardecl*)(_tmp2D5.LessNumelts).val;_LL185:
 _tmp2D8=_tmp2D7;goto _LL187;_LL186: if((_tmp2D5.LessEqNumelts).tag != 5)goto _LL188;_tmp2D8=(struct Cyc_Absyn_Vardecl*)(_tmp2D5.LessEqNumelts).val;_LL187:
 if(v == _tmp2D8)found=1;goto _LL181;_LL188:;_LL189:
 goto _LL181;_LL181:;};}
# 1301
if(!found)return rs;{
# 1303
struct Cyc_List_List*_tmp2D9=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2DA=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(_tmp2DA->vd != v){
{union Cyc_CfFlowInfo_RelnOp _tmp2DB=_tmp2DA->rop;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DD;struct Cyc_Absyn_Vardecl*_tmp2DE;_LL18B: if((_tmp2DB.LessVar).tag != 2)goto _LL18D;_tmp2DC=((struct _tuple11)(_tmp2DB.LessVar).val).f1;_LL18C:
 _tmp2DD=_tmp2DC;goto _LL18E;_LL18D: if((_tmp2DB.LessNumelts).tag != 3)goto _LL18F;_tmp2DD=(struct Cyc_Absyn_Vardecl*)(_tmp2DB.LessNumelts).val;_LL18E:
 _tmp2DE=_tmp2DD;goto _LL190;_LL18F: if((_tmp2DB.LessEqNumelts).tag != 5)goto _LL191;_tmp2DE=(struct Cyc_Absyn_Vardecl*)(_tmp2DB.LessEqNumelts).val;_LL190:
 if(v == _tmp2DE)continue;goto _LL18A;_LL191:;_LL192:
 goto _LL18A;_LL18A:;}{
# 1313
struct Cyc_List_List*_tmp5D0;_tmp2D9=((_tmp5D0=_region_malloc(rgn,sizeof(*_tmp5D0)),((_tmp5D0->hd=_tmp2DA,((_tmp5D0->tl=_tmp2D9,_tmp5D0))))));};}}
# 1316
return _tmp2D9;};}
# 1319
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
{void*_tmp2E0=e->r;struct Cyc_Absyn_Vardecl*_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EC;_LL194: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E0;if(_tmp2E1->tag != 1)goto _LL196;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2E2=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2E1->f2);if(_tmp2E2->tag != 1)goto _LL196;else{_tmp2E3=_tmp2E2->f1;}}}_LL195:
 _tmp2E6=_tmp2E3;goto _LL197;_LL196: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E0;if(_tmp2E4->tag != 1)goto _LL198;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2E4->f2);if(_tmp2E5->tag != 3)goto _LL198;else{_tmp2E6=_tmp2E5->f1;}}}_LL197:
 _tmp2E9=_tmp2E6;goto _LL199;_LL198: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E0;if(_tmp2E7->tag != 1)goto _LL19A;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2E8=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2E7->f2);if(_tmp2E8->tag != 4)goto _LL19A;else{_tmp2E9=_tmp2E8->f1;}}}_LL199:
 _tmp2EC=_tmp2E9;goto _LL19B;_LL19A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E0;if(_tmp2EA->tag != 1)goto _LL19C;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2EB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2EA->f2);if(_tmp2EB->tag != 5)goto _LL19C;else{_tmp2EC=_tmp2EB->f1;}}}_LL19B:
# 1325
 if(!_tmp2EC->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2EC);
goto _LL193;_LL19C:;_LL19D:
 goto _LL193;_LL193:;}
# 1330
return r;}
# 1333
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp5D1;return(_tmp5D1=_region_malloc(r2,sizeof(*_tmp5D1)),((_tmp5D1[0]=*r,_tmp5D1)));}
# 1337
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
# 1341
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_CfFlowInfo_Reln*_tmp2EE=(struct Cyc_CfFlowInfo_Reln*)r1->hd;
struct Cyc_CfFlowInfo_Reln*_tmp2EF=(struct Cyc_CfFlowInfo_Reln*)r2->hd;
if(_tmp2EE->vd != _tmp2EF->vd  || !Cyc_CfFlowInfo_same_relop(_tmp2EE->rop,_tmp2EF->rop))return 0;}
# 1347
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 1351
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 1354
if(v->escapes)return r;
# 1357
r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);
# 1362
{void*_tmp2F0=e->r;struct Cyc_Absyn_Exp*_tmp2F2;_LL19F: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2F1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F0;if(_tmp2F1->tag != 32)goto _LL1A1;else{_tmp2F2=(_tmp2F1->f1).num_elts;if((_tmp2F1->f1).fat_result != 1)goto _LL1A1;}}_LL1A0:
# 1364
 malloc_loop: {
void*_tmp2F3=_tmp2F2->r;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_Absyn_Vardecl*_tmp2F8;struct Cyc_Absyn_Vardecl*_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp301;_LL1A4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2F4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2F4->tag != 13)goto _LL1A6;else{_tmp2F5=_tmp2F4->f2;}}_LL1A5:
 _tmp2F2=_tmp2F5;goto malloc_loop;_LL1A6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2F6->tag != 1)goto _LL1A8;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2F7=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2F6->f2);if(_tmp2F7->tag != 5)goto _LL1A8;else{_tmp2F8=_tmp2F7->f1;}}}_LL1A7:
 _tmp2FB=_tmp2F8;goto _LL1A9;_LL1A8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2F9->tag != 1)goto _LL1AA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2FA=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2F9->f2);if(_tmp2FA->tag != 4)goto _LL1AA;else{_tmp2FB=_tmp2FA->f1;}}}_LL1A9:
 _tmp2FE=_tmp2FB;goto _LL1AB;_LL1AA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2FC->tag != 1)goto _LL1AC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2FD=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2FC->f2);if(_tmp2FD->tag != 3)goto _LL1AC;else{_tmp2FE=_tmp2FD->f1;}}}_LL1AB:
 _tmp301=_tmp2FE;goto _LL1AD;_LL1AC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2FF->tag != 1)goto _LL1AE;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp300=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2FF->f2);if(_tmp300->tag != 1)goto _LL1AE;else{_tmp301=_tmp300->f1;}}}_LL1AD:
# 1371
 if(_tmp301->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5D4;struct Cyc_List_List*_tmp5D3;return(_tmp5D3=_region_malloc(rgn,sizeof(*_tmp5D3)),((_tmp5D3->hd=((_tmp5D4=_region_malloc(rgn,sizeof(*_tmp5D4)),((_tmp5D4->vd=_tmp301,((_tmp5D4->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp5D4)))))),((_tmp5D3->tl=r,_tmp5D3)))));};_LL1AE:;_LL1AF:
 return r;_LL1A3:;}_LL1A1:;_LL1A2:
# 1375
 goto _LL19E;_LL19E:;}
# 1378
{void*_tmp304=Cyc_Tcutil_compress(v->type);_LL1B1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp304;if(_tmp305->tag != 6)goto _LL1B3;}_LL1B2:
 goto _LL1B0;_LL1B3:;_LL1B4:
# 1381
 return r;_LL1B0:;}
# 1385
loop: {
void*_tmp306=e->r;struct Cyc_Absyn_Exp*_tmp308;int _tmp30A;struct Cyc_Absyn_Exp*_tmp30C;enum Cyc_Absyn_Primop _tmp30E;struct Cyc_Absyn_Exp*_tmp30F;_LL1B6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp307=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp307->tag != 13)goto _LL1B8;else{_tmp308=_tmp307->f2;}}_LL1B7:
 e=_tmp308;goto loop;_LL1B8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp309=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp309->tag != 0)goto _LL1BA;else{if(((_tmp309->f1).Int_c).tag != 5)goto _LL1BA;_tmp30A=((struct _tuple6)((_tmp309->f1).Int_c).val).f2;}}_LL1B9: {
# 1389
struct Cyc_CfFlowInfo_Reln*_tmp5D7;struct Cyc_List_List*_tmp5D6;return(_tmp5D6=_region_malloc(rgn,sizeof(*_tmp5D6)),((_tmp5D6->hd=((_tmp5D7=_region_malloc(rgn,sizeof(*_tmp5D7)),((_tmp5D7->vd=v,((_tmp5D7->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp30A),_tmp5D7)))))),((_tmp5D6->tl=r,_tmp5D6)))));}_LL1BA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp30B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp30B->tag != 2)goto _LL1BC;else{if(_tmp30B->f1 != Cyc_Absyn_Mod)goto _LL1BC;if(_tmp30B->f2 == 0)goto _LL1BC;if((_tmp30B->f2)->tl == 0)goto _LL1BC;_tmp30C=(struct Cyc_Absyn_Exp*)((_tmp30B->f2)->tl)->hd;}}_LL1BB:
# 1393
{void*_tmp312=_tmp30C->r;enum Cyc_Absyn_Primop _tmp314;struct Cyc_Absyn_Exp*_tmp315;_LL1C1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp313=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp312;if(_tmp313->tag != 2)goto _LL1C3;else{_tmp314=_tmp313->f1;if(_tmp313->f2 == 0)goto _LL1C3;_tmp315=(struct Cyc_Absyn_Exp*)(_tmp313->f2)->hd;}}_LL1C2:
# 1395
{void*_tmp316=_tmp315->r;struct Cyc_Absyn_Vardecl*_tmp319;struct Cyc_Absyn_Vardecl*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp322;_LL1C6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp317=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp317->tag != 1)goto _LL1C8;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp318=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp317->f2);if(_tmp318->tag != 1)goto _LL1C8;else{_tmp319=_tmp318->f1;}}}_LL1C7:
 _tmp31C=_tmp319;goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp31A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp31A->tag != 1)goto _LL1CA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp31B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp31A->f2);if(_tmp31B->tag != 4)goto _LL1CA;else{_tmp31C=_tmp31B->f1;}}}_LL1C9:
 _tmp31F=_tmp31C;goto _LL1CB;_LL1CA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp31D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp31D->tag != 1)goto _LL1CC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp31D->f2);if(_tmp31E->tag != 3)goto _LL1CC;else{_tmp31F=_tmp31E->f1;}}}_LL1CB:
 _tmp322=_tmp31F;goto _LL1CD;_LL1CC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp320=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp320->tag != 1)goto _LL1CE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp321=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp320->f2);if(_tmp321->tag != 5)goto _LL1CE;else{_tmp322=_tmp321->f1;}}}_LL1CD:
# 1400
 if(_tmp322->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5DA;struct Cyc_List_List*_tmp5D9;return(_tmp5D9=_region_malloc(rgn,sizeof(*_tmp5D9)),((_tmp5D9->hd=((_tmp5DA=_region_malloc(rgn,sizeof(*_tmp5DA)),((_tmp5DA->vd=v,((_tmp5DA->rop=Cyc_CfFlowInfo_LessNumelts(_tmp322),_tmp5DA)))))),((_tmp5D9->tl=r,_tmp5D9)))));};_LL1CE:;_LL1CF:
 goto _LL1C5;_LL1C5:;}
# 1404
goto _LL1C0;_LL1C3:;_LL1C4:
 goto _LL1C0;_LL1C0:;}
# 1407
goto _LL1B5;_LL1BC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp30D->tag != 2)goto _LL1BE;else{_tmp30E=_tmp30D->f1;if(_tmp30D->f2 == 0)goto _LL1BE;_tmp30F=(struct Cyc_Absyn_Exp*)(_tmp30D->f2)->hd;}}_LL1BD:
# 1409
{void*_tmp325=_tmp30F->r;struct Cyc_Absyn_Vardecl*_tmp328;struct Cyc_Absyn_Vardecl*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32E;struct Cyc_Absyn_Vardecl*_tmp331;_LL1D1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp326=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp325;if(_tmp326->tag != 1)goto _LL1D3;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp327=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp326->f2);if(_tmp327->tag != 1)goto _LL1D3;else{_tmp328=_tmp327->f1;}}}_LL1D2:
 _tmp32B=_tmp328;goto _LL1D4;_LL1D3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp329=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp325;if(_tmp329->tag != 1)goto _LL1D5;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp32A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp329->f2);if(_tmp32A->tag != 4)goto _LL1D5;else{_tmp32B=_tmp32A->f1;}}}_LL1D4:
 _tmp32E=_tmp32B;goto _LL1D6;_LL1D5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp32C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp325;if(_tmp32C->tag != 1)goto _LL1D7;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp32D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp32C->f2);if(_tmp32D->tag != 3)goto _LL1D7;else{_tmp32E=_tmp32D->f1;}}}_LL1D6:
 _tmp331=_tmp32E;goto _LL1D8;_LL1D7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp32F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp325;if(_tmp32F->tag != 1)goto _LL1D9;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp330=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp32F->f2);if(_tmp330->tag != 5)goto _LL1D9;else{_tmp331=_tmp330->f1;}}}_LL1D8:
# 1414
 if(_tmp331->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5DD;struct Cyc_List_List*_tmp5DC;return(_tmp5DC=_region_malloc(rgn,sizeof(*_tmp5DC)),((_tmp5DC->hd=((_tmp5DD=_region_malloc(rgn,sizeof(*_tmp5DD)),((_tmp5DD->vd=v,((_tmp5DD->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp331),_tmp5DD)))))),((_tmp5DC->tl=r,_tmp5DC)))));};_LL1D9:;_LL1DA:
 goto _LL1D0;_LL1D0:;}
# 1418
goto _LL1B5;_LL1BE:;_LL1BF:
 goto _LL1B5;_LL1B5:;}
# 1421
return r;}
# 1424
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{void*_tmp334=e1->r;struct Cyc_Absyn_Vardecl*_tmp337;struct Cyc_Absyn_Vardecl*_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33D;struct Cyc_Absyn_Vardecl*_tmp340;_LL1DC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp335=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp334;if(_tmp335->tag != 1)goto _LL1DE;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp336=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp335->f2);if(_tmp336->tag != 1)goto _LL1DE;else{_tmp337=_tmp336->f1;}}}_LL1DD:
 _tmp33A=_tmp337;goto _LL1DF;_LL1DE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp338=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp334;if(_tmp338->tag != 1)goto _LL1E0;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp339=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp338->f2);if(_tmp339->tag != 3)goto _LL1E0;else{_tmp33A=_tmp339->f1;}}}_LL1DF:
 _tmp33D=_tmp33A;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp334;if(_tmp33B->tag != 1)goto _LL1E2;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp33C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp33B->f2);if(_tmp33C->tag != 4)goto _LL1E2;else{_tmp33D=_tmp33C->f1;}}}_LL1E1:
 _tmp340=_tmp33D;goto _LL1E3;_LL1E2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp334;if(_tmp33E->tag != 1)goto _LL1E4;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp33F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp33E->f2);if(_tmp33F->tag != 5)goto _LL1E4;else{_tmp340=_tmp33F->f1;}}}_LL1E3:
# 1430
 if(!_tmp340->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp340,e2);
goto _LL1DB;_LL1E4:;_LL1E5:
 goto _LL1DB;_LL1DB:;}
# 1435
return r;}
# 1438
static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){
{const char*_tmp5E1;void*_tmp5E0[1];struct Cyc_String_pa_PrintArg_struct _tmp5DF;(_tmp5DF.tag=0,((_tmp5DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name)),((_tmp5E0[0]=& _tmp5DF,Cyc_fprintf(Cyc_stderr,((_tmp5E1="%s",_tag_dyneither(_tmp5E1,sizeof(char),3))),_tag_dyneither(_tmp5E0,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp344=r->rop;unsigned int _tmp345;struct Cyc_Absyn_Vardecl*_tmp346;struct Cyc_Absyn_Vardecl*_tmp347;unsigned int _tmp348;unsigned int _tmp349;struct Cyc_Absyn_Vardecl*_tmp34A;_LL1E7: if((_tmp344.EqualConst).tag != 1)goto _LL1E9;_tmp345=(unsigned int)(_tmp344.EqualConst).val;_LL1E8:
{const char*_tmp5E5;void*_tmp5E4[1];struct Cyc_Int_pa_PrintArg_struct _tmp5E3;(_tmp5E3.tag=1,((_tmp5E3.f1=(unsigned long)((int)_tmp345),((_tmp5E4[0]=& _tmp5E3,Cyc_fprintf(Cyc_stderr,((_tmp5E5="==%d",_tag_dyneither(_tmp5E5,sizeof(char),5))),_tag_dyneither(_tmp5E4,sizeof(void*),1)))))));}goto _LL1E6;_LL1E9: if((_tmp344.LessVar).tag != 2)goto _LL1EB;_tmp346=((struct _tuple11)(_tmp344.LessVar).val).f1;_LL1EA:
{const char*_tmp5E9;void*_tmp5E8[1];struct Cyc_String_pa_PrintArg_struct _tmp5E7;(_tmp5E7.tag=0,((_tmp5E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp346->name)),((_tmp5E8[0]=& _tmp5E7,Cyc_fprintf(Cyc_stderr,((_tmp5E9="<%s",_tag_dyneither(_tmp5E9,sizeof(char),4))),_tag_dyneither(_tmp5E8,sizeof(void*),1)))))));}goto _LL1E6;_LL1EB: if((_tmp344.LessNumelts).tag != 3)goto _LL1ED;_tmp347=(struct Cyc_Absyn_Vardecl*)(_tmp344.LessNumelts).val;_LL1EC:
{const char*_tmp5ED;void*_tmp5EC[1];struct Cyc_String_pa_PrintArg_struct _tmp5EB;(_tmp5EB.tag=0,((_tmp5EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp347->name)),((_tmp5EC[0]=& _tmp5EB,Cyc_fprintf(Cyc_stderr,((_tmp5ED="<numelts(%s)",_tag_dyneither(_tmp5ED,sizeof(char),13))),_tag_dyneither(_tmp5EC,sizeof(void*),1)))))));}goto _LL1E6;_LL1ED: if((_tmp344.LessConst).tag != 4)goto _LL1EF;_tmp348=(unsigned int)(_tmp344.LessConst).val;_LL1EE:
{const char*_tmp5F1;void*_tmp5F0[1];struct Cyc_Int_pa_PrintArg_struct _tmp5EF;(_tmp5EF.tag=1,((_tmp5EF.f1=(unsigned long)((int)_tmp348),((_tmp5F0[0]=& _tmp5EF,Cyc_fprintf(Cyc_stderr,((_tmp5F1="<%d",_tag_dyneither(_tmp5F1,sizeof(char),4))),_tag_dyneither(_tmp5F0,sizeof(void*),1)))))));}goto _LL1E6;_LL1EF: if((_tmp344.LessEqConst).tag != 6)goto _LL1F1;_tmp349=(unsigned int)(_tmp344.LessEqConst).val;_LL1F0:
{const char*_tmp5F5;void*_tmp5F4[1];struct Cyc_Int_pa_PrintArg_struct _tmp5F3;(_tmp5F3.tag=1,((_tmp5F3.f1=(unsigned long)((int)_tmp349),((_tmp5F4[0]=& _tmp5F3,Cyc_fprintf(Cyc_stderr,((_tmp5F5="<=%d",_tag_dyneither(_tmp5F5,sizeof(char),5))),_tag_dyneither(_tmp5F4,sizeof(void*),1)))))));}goto _LL1E6;_LL1F1: if((_tmp344.LessEqNumelts).tag != 5)goto _LL1E6;_tmp34A=(struct Cyc_Absyn_Vardecl*)(_tmp344.LessEqNumelts).val;_LL1F2:
{const char*_tmp5F9;void*_tmp5F8[1];struct Cyc_String_pa_PrintArg_struct _tmp5F7;(_tmp5F7.tag=0,((_tmp5F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp34A->name)),((_tmp5F8[0]=& _tmp5F7,Cyc_fprintf(Cyc_stderr,((_tmp5F9="<=numelts(%s)",_tag_dyneither(_tmp5F9,sizeof(char),14))),_tag_dyneither(_tmp5F8,sizeof(void*),1)))))));}goto _LL1E6;_LL1E6:;};}
# 1449
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp5FC;void*_tmp5FB;(_tmp5FB=0,Cyc_fprintf(Cyc_stderr,((_tmp5FC=",",_tag_dyneither(_tmp5FC,sizeof(char),2))),_tag_dyneither(_tmp5FB,sizeof(void*),0)));}}}
# 1456
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1F3:
{const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,Cyc_fprintf(Cyc_stderr,((_tmp5FF="uninitialized",_tag_dyneither(_tmp5FF,sizeof(char),14))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL1F4:
{const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_fprintf(Cyc_stderr,((_tmp602="this-initialized",_tag_dyneither(_tmp602,sizeof(char),17))),_tag_dyneither(_tmp601,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1F5:
{const char*_tmp605;void*_tmp604;(_tmp604=0,Cyc_fprintf(Cyc_stderr,((_tmp605="all-initialized",_tag_dyneither(_tmp605,sizeof(char),16))),_tag_dyneither(_tmp604,sizeof(void*),0)));}break;}}
# 1464
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp365=root;struct Cyc_Absyn_Vardecl*_tmp367;struct Cyc_Absyn_Exp*_tmp369;void*_tmp36A;_LL1F8: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp366=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp365;if(_tmp366->tag != 0)goto _LL1FA;else{_tmp367=_tmp366->f1;}}_LL1F9:
# 1467
{const char*_tmp609;void*_tmp608[1];struct Cyc_String_pa_PrintArg_struct _tmp607;(_tmp607.tag=0,((_tmp607.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp367->name)),((_tmp608[0]=& _tmp607,Cyc_fprintf(Cyc_stderr,((_tmp609="Root(%s)",_tag_dyneither(_tmp609,sizeof(char),9))),_tag_dyneither(_tmp608,sizeof(void*),1)))))));}goto _LL1F7;_LL1FA: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp368=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp365;if(_tmp368->tag != 1)goto _LL1FC;else{_tmp369=_tmp368->f1;_tmp36A=(void*)_tmp368->f2;}}_LL1FB:
# 1469
{const char*_tmp60E;void*_tmp60D[2];struct Cyc_String_pa_PrintArg_struct _tmp60C;struct Cyc_String_pa_PrintArg_struct _tmp60B;(_tmp60B.tag=0,((_tmp60B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp36A)),((_tmp60C.tag=0,((_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp369)),((_tmp60D[0]=& _tmp60C,((_tmp60D[1]=& _tmp60B,Cyc_fprintf(Cyc_stderr,((_tmp60E="MallocPt(%s,%s)",_tag_dyneither(_tmp60E,sizeof(char),16))),_tag_dyneither(_tmp60D,sizeof(void*),2)))))))))))));}goto _LL1F7;_LL1FC: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp36B=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp365;if(_tmp36B->tag != 2)goto _LL1F7;}_LL1FD:
# 1472
{const char*_tmp611;void*_tmp610;(_tmp610=0,Cyc_fprintf(Cyc_stderr,((_tmp611="InitParam(_,_)",_tag_dyneither(_tmp611,sizeof(char),15))),_tag_dyneither(_tmp610,sizeof(void*),0)));}goto _LL1F7;_LL1F7:;}
# 1476
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp375=p->fields;
while(_tmp375 != 0){
{const char*_tmp615;void*_tmp614[1];struct Cyc_Int_pa_PrintArg_struct _tmp613;(_tmp613.tag=1,((_tmp613.f1=(unsigned long)((int)_tmp375->hd),((_tmp614[0]=& _tmp613,Cyc_fprintf(Cyc_stderr,((_tmp615=".%d",_tag_dyneither(_tmp615,sizeof(char),4))),_tag_dyneither(_tmp614,sizeof(void*),1)))))));}
_tmp375=_tmp375->tl;}{
# 1483
const char*_tmp618;void*_tmp617;(_tmp617=0,Cyc_fprintf(Cyc_stderr,((_tmp618=" ",_tag_dyneither(_tmp618,sizeof(char),2))),_tag_dyneither(_tmp617,sizeof(void*),0)));};};}
# 1486
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp37B=1;
while(x != 0){
if(!_tmp37B){{const char*_tmp61B;void*_tmp61A;(_tmp61A=0,Cyc_fprintf(Cyc_stderr,((_tmp61B=", ",_tag_dyneither(_tmp61B,sizeof(char),3))),_tag_dyneither(_tmp61A,sizeof(void*),0)));}_tmp37B=0;}
pr(x->hd);
x=x->tl;}{
# 1493
const char*_tmp61E;void*_tmp61D;(_tmp61D=0,Cyc_fprintf(Cyc_stderr,((_tmp61E="\n",_tag_dyneither(_tmp61E,sizeof(char),2))),_tag_dyneither(_tmp61D,sizeof(void*),0)));};}
# 1496
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp380=_new_region("r");struct _RegionHandle*r=& _tmp380;_push_region(r);
{struct _handler_cons _tmp381;_push_handler(& _tmp381);{int _tmp383=0;if(setjmp(_tmp381.handler))_tmp383=1;if(!_tmp383){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp384=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp621;void*_tmp620;(_tmp620=0,Cyc_fprintf(Cyc_stderr,((_tmp621="{ ",_tag_dyneither(_tmp621,sizeof(char),3))),_tag_dyneither(_tmp620,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp384)){
void*_tmp387=_tmp384.f1;
if(!first){
{const char*_tmp624;void*_tmp623;(_tmp623=0,Cyc_fprintf(Cyc_stderr,((_tmp624=", ",_tag_dyneither(_tmp624,sizeof(char),3))),_tag_dyneither(_tmp623,sizeof(void*),0)));}
first=0;}
# 1509
pr(_tmp387);}{
# 1511
const char*_tmp627;void*_tmp626;(_tmp626=0,Cyc_fprintf(Cyc_stderr,((_tmp627="}\n",_tag_dyneither(_tmp627,sizeof(char),3))),_tag_dyneither(_tmp626,sizeof(void*),0)));};}
# 1499
;_pop_handler();}else{void*_tmp382=(void*)_exn_thrown;void*_tmp38D=_tmp382;void*_tmp38E;void*_tmp38F;_LL1FF: _tmp38E=_tmp38D;_LL200:
# 1514
{const char*_tmp62A;void*_tmp629;(_tmp629=0,Cyc_fprintf(Cyc_stderr,((_tmp62A="{ }\n",_tag_dyneither(_tmp62A,sizeof(char),5))),_tag_dyneither(_tmp629,sizeof(void*),0)));}
_npop_handler(0);return;_LL201: _tmp38F=_tmp38D;_LL202:(void)_throw(_tmp38F);_LL1FE:;}};}
# 1498
;_pop_region(r);}
# 1520
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp392=rval;enum Cyc_CfFlowInfo_InitLevel _tmp397;enum Cyc_CfFlowInfo_InitLevel _tmp399;struct Cyc_CfFlowInfo_Place*_tmp39B;int _tmp39D;int _tmp39E;struct _dyneither_ptr _tmp39F;struct Cyc_Absyn_Exp*_tmp3A1;int _tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A5;void*_tmp3A6;_LL204: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp393=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp393->tag != 0)goto _LL206;}_LL205:
{const char*_tmp62D;void*_tmp62C;(_tmp62C=0,Cyc_fprintf(Cyc_stderr,((_tmp62D="Zero",_tag_dyneither(_tmp62D,sizeof(char),5))),_tag_dyneither(_tmp62C,sizeof(void*),0)));}goto _LL203;_LL206: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp394=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp394->tag != 1)goto _LL208;}_LL207:
{const char*_tmp630;void*_tmp62F;(_tmp62F=0,Cyc_fprintf(Cyc_stderr,((_tmp630="NotZeroAll",_tag_dyneither(_tmp630,sizeof(char),11))),_tag_dyneither(_tmp62F,sizeof(void*),0)));}goto _LL203;_LL208: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp395=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp395->tag != 2)goto _LL20A;}_LL209:
{const char*_tmp633;void*_tmp632;(_tmp632=0,Cyc_fprintf(Cyc_stderr,((_tmp633="NotZeroThis",_tag_dyneither(_tmp633,sizeof(char),12))),_tag_dyneither(_tmp632,sizeof(void*),0)));}goto _LL203;_LL20A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp396=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp396->tag != 3)goto _LL20C;else{_tmp397=_tmp396->f1;}}_LL20B:
{const char*_tmp636;void*_tmp635;(_tmp635=0,Cyc_fprintf(Cyc_stderr,((_tmp636="Unknown(",_tag_dyneither(_tmp636,sizeof(char),9))),_tag_dyneither(_tmp635,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp397);
{const char*_tmp639;void*_tmp638;(_tmp638=0,Cyc_fprintf(Cyc_stderr,((_tmp639=")",_tag_dyneither(_tmp639,sizeof(char),2))),_tag_dyneither(_tmp638,sizeof(void*),0)));}goto _LL203;_LL20C: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp398=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp398->tag != 4)goto _LL20E;else{_tmp399=_tmp398->f1;}}_LL20D:
{const char*_tmp63C;void*_tmp63B;(_tmp63B=0,Cyc_fprintf(Cyc_stderr,((_tmp63C="Esc(",_tag_dyneither(_tmp63C,sizeof(char),5))),_tag_dyneither(_tmp63B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp399);
{const char*_tmp63F;void*_tmp63E;(_tmp63E=0,Cyc_fprintf(Cyc_stderr,((_tmp63F=")",_tag_dyneither(_tmp63F,sizeof(char),2))),_tag_dyneither(_tmp63E,sizeof(void*),0)));}goto _LL203;_LL20E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp39A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp39A->tag != 5)goto _LL210;else{_tmp39B=_tmp39A->f1;}}_LL20F:
{const char*_tmp642;void*_tmp641;(_tmp641=0,Cyc_fprintf(Cyc_stderr,((_tmp642="AddrOf(",_tag_dyneither(_tmp642,sizeof(char),8))),_tag_dyneither(_tmp641,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp39B);
{const char*_tmp645;void*_tmp644;(_tmp644=0,Cyc_fprintf(Cyc_stderr,((_tmp645=")",_tag_dyneither(_tmp645,sizeof(char),2))),_tag_dyneither(_tmp644,sizeof(void*),0)));}goto _LL203;_LL210: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp39C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp39C->tag != 6)goto _LL212;else{_tmp39D=(_tmp39C->f1).is_union;_tmp39E=(_tmp39C->f1).fieldnum;_tmp39F=_tmp39C->f2;}}_LL211:
# 1532
 if(_tmp39D){
{const char*_tmp648;void*_tmp647;(_tmp647=0,Cyc_fprintf(Cyc_stderr,((_tmp648="AggrUnion{",_tag_dyneither(_tmp648,sizeof(char),11))),_tag_dyneither(_tmp647,sizeof(void*),0)));}
if(_tmp39E != - 1){
const char*_tmp64C;void*_tmp64B[1];struct Cyc_Int_pa_PrintArg_struct _tmp64A;(_tmp64A.tag=1,((_tmp64A.f1=(unsigned long)_tmp39E,((_tmp64B[0]=& _tmp64A,Cyc_fprintf(Cyc_stderr,((_tmp64C="tag == %d;",_tag_dyneither(_tmp64C,sizeof(char),11))),_tag_dyneither(_tmp64B,sizeof(void*),1)))))));}}else{
# 1537
const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_fprintf(Cyc_stderr,((_tmp64F="AggrStruct{",_tag_dyneither(_tmp64F,sizeof(char),12))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp39F,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp39F.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp39F,sizeof(void*))){const char*_tmp652;void*_tmp651;(_tmp651=0,Cyc_fprintf(Cyc_stderr,((_tmp652=",",_tag_dyneither(_tmp652,sizeof(char),2))),_tag_dyneither(_tmp651,sizeof(void*),0)));}}}
# 1542
{const char*_tmp655;void*_tmp654;(_tmp654=0,Cyc_fprintf(Cyc_stderr,((_tmp655="}",_tag_dyneither(_tmp655,sizeof(char),2))),_tag_dyneither(_tmp654,sizeof(void*),0)));}
goto _LL203;_LL212: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp3A0->tag != 7)goto _LL214;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;_tmp3A3=(void*)_tmp3A0->f3;}}_LL213:
# 1545
{const char*_tmp65A;void*_tmp659[2];struct Cyc_String_pa_PrintArg_struct _tmp658;struct Cyc_Int_pa_PrintArg_struct _tmp657;(_tmp657.tag=1,((_tmp657.f1=(unsigned long)_tmp3A2,((_tmp658.tag=0,((_tmp658.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3A1)),((_tmp659[0]=& _tmp658,((_tmp659[1]=& _tmp657,Cyc_fprintf(Cyc_stderr,((_tmp65A="[Consumed(%s,%d,",_tag_dyneither(_tmp65A,sizeof(char),17))),_tag_dyneither(_tmp659,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3A3);{const char*_tmp65D;void*_tmp65C;(_tmp65C=0,Cyc_fprintf(Cyc_stderr,((_tmp65D="]",_tag_dyneither(_tmp65D,sizeof(char),2))),_tag_dyneither(_tmp65C,sizeof(void*),0)));}
goto _LL203;_LL214: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3A4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp392;if(_tmp3A4->tag != 8)goto _LL203;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=(void*)_tmp3A4->f2;}}_LL215:
# 1549
{const char*_tmp661;void*_tmp660[1];struct Cyc_String_pa_PrintArg_struct _tmp65F;(_tmp65F.tag=0,((_tmp65F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3A5->name)),((_tmp660[0]=& _tmp65F,Cyc_fprintf(Cyc_stderr,((_tmp661="[NamedLocation(%s,",_tag_dyneither(_tmp661,sizeof(char),19))),_tag_dyneither(_tmp660,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3A6);{const char*_tmp664;void*_tmp663;(_tmp663=0,Cyc_fprintf(Cyc_stderr,((_tmp664="]",_tag_dyneither(_tmp664,sizeof(char),2))),_tag_dyneither(_tmp663,sizeof(void*),0)));}
goto _LL203;_LL203:;}
# 1555
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp667;void*_tmp666;(_tmp666=0,Cyc_fprintf(Cyc_stderr,((_tmp667="    ",_tag_dyneither(_tmp667,sizeof(char),5))),_tag_dyneither(_tmp666,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp66A;void*_tmp669;(_tmp669=0,Cyc_fprintf(Cyc_stderr,((_tmp66A=" --> ",_tag_dyneither(_tmp66A,sizeof(char),6))),_tag_dyneither(_tmp669,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp66D;void*_tmp66C;(_tmp66C=0,Cyc_fprintf(Cyc_stderr,((_tmp66D="\n",_tag_dyneither(_tmp66D,sizeof(char),2))),_tag_dyneither(_tmp66C,sizeof(void*),0)));};}
# 1563
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1567
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp3D5=f;struct Cyc_Dict_Dict _tmp3D6;_LL217: if((_tmp3D5.BottomFL).tag != 1)goto _LL219;_LL218:
{const char*_tmp670;void*_tmp66F;(_tmp66F=0,Cyc_fprintf(Cyc_stderr,((_tmp670="  BottomFL\n",_tag_dyneither(_tmp670,sizeof(char),12))),_tag_dyneither(_tmp66F,sizeof(void*),0)));}goto _LL216;_LL219: if((_tmp3D5.ReachableFL).tag != 2)goto _LL216;_tmp3D6=((struct _tuple12)(_tmp3D5.ReachableFL).val).f1;_LL21A:
# 1571
{const char*_tmp673;void*_tmp672;(_tmp672=0,Cyc_fprintf(Cyc_stderr,((_tmp673="  ReachableFL:\n",_tag_dyneither(_tmp673,sizeof(char),16))),_tag_dyneither(_tmp672,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp3D6);
{const char*_tmp676;void*_tmp675;(_tmp675=0,Cyc_fprintf(Cyc_stderr,((_tmp676="\n",_tag_dyneither(_tmp676,sizeof(char),2))),_tag_dyneither(_tmp675,sizeof(void*),0)));}
goto _LL216;_LL216:;}
# 1586 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp3DD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3EA;struct Cyc_List_List*_tmp3EC;struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3F2;struct Cyc_List_List*_tmp3F4;void*_tmp3F6;void*_tmp3F7;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FC;void*_tmp3FE;void*_tmp400;void*_tmp402;void*_tmp404;void*_tmp405;void*_tmp407;_LL21C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3DD;if(_tmp3DE->tag != 26)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3DD;if(_tmp3DF->tag != 0)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E0->tag != 6)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp3E1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E1->tag != 7)goto _LL224;}_LL223:
 goto _LL225;_LL224: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E2->tag != 13)goto _LL226;}_LL225:
 goto _LL227;_LL226: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E3->tag != 14)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E4->tag != 19)goto _LL22A;}_LL229:
 goto _LL22B;_LL22A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3DD;if(_tmp3E5->tag != 20)goto _LL22C;}_LL22B:
 goto _LL22D;_LL22C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DD;if(_tmp3E6->tag != 1)goto _LL22E;}_LL22D:
 return 0;_LL22E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3DD;if(_tmp3E7->tag != 21)goto _LL230;}_LL22F: {
# 1598
void*_tmp409=rgn;_LL253: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp40A=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp409;if(_tmp40A->tag != 0)goto _LL255;}_LL254:
 return 1;_LL255:;_LL256:
 return 0;_LL252:;}_LL230: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3DD;if(_tmp3E8->tag != 22)goto _LL232;}_LL231: {
# 1603
void*_tmp40B=rgn;_LL258: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp40C=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp40B;if(_tmp40C->tag != 1)goto _LL25A;}_LL259:
 return 1;_LL25A:;_LL25B:
 return 0;_LL257:;}_LL232: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E9->tag != 2)goto _LL234;else{_tmp3EA=_tmp3E9->f1;}}_LL233: {
# 1608
void*_tmp40D=rgn;struct Cyc_Absyn_Tvar*_tmp411;_LL25D: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp40E=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp40D;if(_tmp40E->tag != 1)goto _LL25F;}_LL25E:
 goto _LL260;_LL25F: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp40F=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp40D;if(_tmp40F->tag != 0)goto _LL261;}_LL260:
 return 0;_LL261: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp410=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp40D;if(_tmp410->tag != 2)goto _LL25C;else{_tmp411=_tmp410->f1;}}_LL262:
 return Cyc_Absyn_tvar_cmp(_tmp3EA,_tmp411)== 0;_LL25C:;}_LL234: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3DD;if(_tmp3EB->tag != 3)goto _LL236;else{_tmp3EC=(_tmp3EB->f1).targs;}}_LL235:
# 1621 "cf_flowinfo.cyc"
 _tmp3EE=_tmp3EC;goto _LL237;_LL236: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3DD;if(_tmp3ED->tag != 17)goto _LL238;else{_tmp3EE=_tmp3ED->f2;}}_LL237:
 _tmp3F0=_tmp3EE;goto _LL239;_LL238: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3DD;if(_tmp3EF->tag != 11)goto _LL23A;else{_tmp3F0=(_tmp3EF->f1).targs;}}_LL239:
 _tmp3F2=_tmp3F0;goto _LL23B;_LL23A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3F1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3DD;if(_tmp3F1->tag != 4)goto _LL23C;else{_tmp3F2=(_tmp3F1->f1).targs;}}_LL23B:
 _tmp3F4=_tmp3F2;goto _LL23D;_LL23C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3F3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3DD;if(_tmp3F3->tag != 24)goto _LL23E;else{_tmp3F4=_tmp3F3->f1;}}_LL23D:
# 1626
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp3F4);_LL23E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD;if(_tmp3F5->tag != 5)goto _LL240;else{_tmp3F6=(_tmp3F5->f1).elt_typ;_tmp3F7=((_tmp3F5->f1).ptr_atts).rgn;}}_LL23F:
# 1628
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3F7) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3F6);_LL240: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3DD;if(_tmp3F8->tag != 9)goto _LL242;}_LL241:
 return 0;_LL242: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3DD;if(_tmp3F9->tag != 10)goto _LL244;else{_tmp3FA=_tmp3F9->f1;}}_LL243:
# 1631
 for(0;_tmp3FA != 0;_tmp3FA=_tmp3FA->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp3FA->hd)).f2))return 1;}
return 0;_LL244: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3DD;if(_tmp3FB->tag != 12)goto _LL246;else{_tmp3FC=_tmp3FB->f2;}}_LL245:
# 1635
 for(0;_tmp3FC != 0;_tmp3FC=_tmp3FC->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp3FC->hd)->type))return 1;}
return 0;_LL246: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DD;if(_tmp3FD->tag != 8)goto _LL248;else{_tmp3FE=(_tmp3FD->f1).elt_type;}}_LL247:
 _tmp400=_tmp3FE;goto _LL249;_LL248: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3DD;if(_tmp3FF->tag != 23)goto _LL24A;else{_tmp400=(void*)_tmp3FF->f1;}}_LL249:
 _tmp402=_tmp400;goto _LL24B;_LL24A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DD;if(_tmp401->tag != 25)goto _LL24C;else{_tmp402=(void*)_tmp401->f1;}}_LL24B:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp402);_LL24C: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3DD;if(_tmp403->tag != 16)goto _LL24E;else{_tmp404=(void*)_tmp403->f1;_tmp405=(void*)_tmp403->f2;}}_LL24D:
# 1642
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp404) || Cyc_CfFlowInfo_contains_region(rgn,_tmp405);_LL24E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3DD;if(_tmp406->tag != 15)goto _LL250;else{_tmp407=(void*)_tmp406->f1;}}_LL24F:
# 1645
 return 0;_LL250: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3DD;if(_tmp408->tag != 18)goto _LL21B;}_LL251:
 return 0;_LL21B:;}struct _tuple25{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1651
static void Cyc_CfFlowInfo_kill_root(struct _tuple25*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp413;struct Cyc_Dict_Dict*_tmp414;void*_tmp415;struct _tuple25*_tmp412=env;_tmp413=_tmp412->f1;_tmp414=(struct Cyc_Dict_Dict*)& _tmp412->f2;_tmp415=_tmp412->f3;{
void*_tmp416=root;struct Cyc_Absyn_Vardecl*_tmp418;void*_tmp41A;_LL264: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp417=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp416;if(_tmp417->tag != 0)goto _LL266;else{_tmp418=_tmp417->f1;}}_LL265:
# 1656
 if(Cyc_CfFlowInfo_contains_region(_tmp415,_tmp418->type))
# 1658
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp413,_tmp418->type,0,_tmp413->unknown_none);
*_tmp414=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp414,root,rval);
goto _LL263;_LL266: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp419=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp416;if(_tmp419->tag != 1)goto _LL268;else{_tmp41A=(void*)_tmp419->f2;}}_LL267:
# 1663
 if(!Cyc_CfFlowInfo_contains_region(_tmp415,_tmp41A))
# 1665
*_tmp414=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp414,root,rval);
goto _LL263;_LL268: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp41B=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp416;if(_tmp41B->tag != 2)goto _LL263;}_LL269:
# 1669
 goto _LL263;_LL263:;};}
# 1674
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1676
void*_tmp41C=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp41E;_LL26B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp41C;if(_tmp41D->tag != 2)goto _LL26D;else{_tmp41E=_tmp41D->f1;}}_LL26C: {
# 1678
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp677;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp677.tag=2,((_tmp677.f1=_tmp41E,_tmp677)));
void*v2=(void*)& v;
struct _tuple25 _tmp678;struct _tuple25 env=(_tmp678.f1=fenv,((_tmp678.f2=fenv->mt_flowdict,((_tmp678.f3=v2,_tmp678)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL26D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp41C;if(_tmp41F->tag != 21)goto _LL26F;}_LL26E: {
# 1685
struct _tuple25 _tmp679;struct _tuple25 env=(_tmp679.f1=fenv,((_tmp679.f2=fenv->mt_flowdict,((_tmp679.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp679)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL26F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp41C;if(_tmp420->tag != 22)goto _LL271;}_LL270: {
# 1690
struct _tuple25 _tmp67A;struct _tuple25 env=(_tmp67A.f1=fenv,((_tmp67A.f2=fenv->mt_flowdict,((_tmp67A.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp67A)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL271:;_LL272: {
# 1694
struct Cyc_Core_Impossible_exn_struct _tmp680;const char*_tmp67F;struct Cyc_Core_Impossible_exn_struct*_tmp67E;(int)_throw((void*)((_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E[0]=((_tmp680.tag=Cyc_Core_Impossible,((_tmp680.f1=((_tmp67F="kill_flowdict_region",_tag_dyneither(_tmp67F,sizeof(char),21))),_tmp680)))),_tmp67E)))));}_LL26A:;}
# 1699
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1701
return 0;}
# 1706
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp428=f;struct Cyc_Dict_Dict _tmp429;struct Cyc_List_List*_tmp42A;_LL274: if((_tmp428.BottomFL).tag != 1)goto _LL276;_LL275:
 return f;_LL276: if((_tmp428.ReachableFL).tag != 2)goto _LL273;_tmp429=((struct _tuple12)(_tmp428.ReachableFL).val).f1;_tmp42A=((struct _tuple12)(_tmp428.ReachableFL).val).f2;_LL277: {
# 1710
struct Cyc_Dict_Dict _tmp42B=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp429,rgn);
struct Cyc_List_List*_tmp42C=Cyc_CfFlowInfo_kill_relns_region(_tmp42A,rgn);
# 1713
return Cyc_CfFlowInfo_ReachableFL(_tmp42B,_tmp42C);}_LL273:;}
