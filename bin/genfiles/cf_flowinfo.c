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
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 383
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 65
void*Cyc_Tcutil_compress(void*t);
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 260 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 338
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 137
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 139
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 183
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 186
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 192
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 197
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 217 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
# 227
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 231
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 236
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 242
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 254
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 258
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 260
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 45 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 49
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp3D3;return((_tmp3D3.PlaceL).val=x,(((_tmp3D3.PlaceL).tag=1,_tmp3D3)));}
# 52
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp3D4;return((_tmp3D4.UnknownL).val=0,(((_tmp3D4.UnknownL).tag=2,_tmp3D4)));}
# 55
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp3D5;return((_tmp3D5.BottomFL).val=0,(((_tmp3D5.BottomFL).tag=1,_tmp3D5)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple12 _tmp3D8;union Cyc_CfFlowInfo_FlowInfo _tmp3D7;return((_tmp3D7.ReachableFL).val=((_tmp3D8.f1=fd,((_tmp3D8.f2=r,_tmp3D8)))),(((_tmp3D7.ReachableFL).tag=2,_tmp3D7)));}
# 61
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp40F;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp40E;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp40D;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp40C;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp40B;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp40A;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp409;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp408;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp407;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp406;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp405;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp404;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp403;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp402;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp401;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp400;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp3FF;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3FE;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp3FD;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3FC;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp3FB;struct Cyc_CfFlowInfo_FlowEnv*_tmp3FA;return(_tmp3FA=_region_malloc(r,sizeof(*_tmp3FA)),((_tmp3FA->r=r,((_tmp3FA->zero=(void*)(
# 68
(_tmp3FC=_region_malloc(r,sizeof(*_tmp3FC)),((_tmp3FC[0]=((_tmp3FB.tag=0,_tmp3FB)),_tmp3FC)))),((_tmp3FA->notzeroall=(void*)(
(_tmp3FE=_region_malloc(r,sizeof(*_tmp3FE)),((_tmp3FE[0]=((_tmp3FD.tag=1,_tmp3FD)),_tmp3FE)))),((_tmp3FA->notzerothis=(void*)(
(_tmp400=_region_malloc(r,sizeof(*_tmp400)),((_tmp400[0]=((_tmp3FF.tag=2,_tmp3FF)),_tmp400)))),((_tmp3FA->unknown_none=(void*)(
(_tmp402=_region_malloc(r,sizeof(*_tmp402)),((_tmp402[0]=((_tmp401.tag=3,((_tmp401.f1=Cyc_CfFlowInfo_NoneIL,_tmp401)))),_tmp402)))),((_tmp3FA->unknown_this=(void*)(
(_tmp404=_region_malloc(r,sizeof(*_tmp404)),((_tmp404[0]=((_tmp403.tag=3,((_tmp403.f1=Cyc_CfFlowInfo_ThisIL,_tmp403)))),_tmp404)))),((_tmp3FA->unknown_all=(void*)(
(_tmp406=_region_malloc(r,sizeof(*_tmp406)),((_tmp406[0]=((_tmp405.tag=3,((_tmp405.f1=Cyc_CfFlowInfo_AllIL,_tmp405)))),_tmp406)))),((_tmp3FA->esc_none=(void*)(
(_tmp408=_region_malloc(r,sizeof(*_tmp408)),((_tmp408[0]=((_tmp407.tag=4,((_tmp407.f1=Cyc_CfFlowInfo_NoneIL,_tmp407)))),_tmp408)))),((_tmp3FA->esc_this=(void*)(
(_tmp40A=_region_malloc(r,sizeof(*_tmp40A)),((_tmp40A[0]=((_tmp409.tag=4,((_tmp409.f1=Cyc_CfFlowInfo_ThisIL,_tmp409)))),_tmp40A)))),((_tmp3FA->esc_all=(void*)(
(_tmp40C=_region_malloc(r,sizeof(*_tmp40C)),((_tmp40C[0]=((_tmp40B.tag=4,((_tmp40B.f1=Cyc_CfFlowInfo_AllIL,_tmp40B)))),_tmp40C)))),((_tmp3FA->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp3FA->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp3FA->dummy_place=(
(_tmp40F=_region_malloc(r,sizeof(*_tmp40F)),((_tmp40F->root=(void*)((_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40E.tag=1,((_tmp40E.f1=& dummy_exp,((_tmp40E.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp40E)))))),_tmp40D)))),((_tmp40F->fields=0,_tmp40F)))))),_tmp3FA)))))))))))))))))))))))))));}
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
const char*_tmp413;void*_tmp412[1];struct Cyc_String_pa_PrintArg_struct _tmp411;return(struct _dyneither_ptr)((_tmp411.tag=0,((_tmp411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp412[0]=& _tmp411,Cyc_aprintf(((_tmp413="reachable from %s",_tag_dyneither(_tmp413,sizeof(char),18))),_tag_dyneither(_tmp412,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp416;void*_tmp415;(_tmp415=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp416="error locations not for VarRoots",_tag_dyneither(_tmp416,sizeof(char),33))),_tag_dyneither(_tmp415,sizeof(void*),0)));}_LL0:;};}
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
struct Cyc_Core_Impossible_exn_struct _tmp423;const char*_tmp422;void*_tmp421[1];struct Cyc_String_pa_PrintArg_struct _tmp420;struct Cyc_Core_Impossible_exn_struct*_tmp41F;(int)_throw((void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp423.tag=Cyc_Core_Impossible,((_tmp423.f1=(struct _dyneither_ptr)((_tmp420.tag=0,((_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp421[0]=& _tmp420,Cyc_aprintf(((_tmp422="get_field_index_fs failed: %s",_tag_dyneither(_tmp422,sizeof(char),30))),_tag_dyneither(_tmp421,sizeof(void*),1)))))))),_tmp423)))),_tmp41F)))));};}
# 173
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3E=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp40;struct Cyc_List_List*_tmp42;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E;if(_tmp3F->tag != 11)goto _LLD;else{_tmp40=(_tmp3F->f1).aggr_info;}}_LLC: {
# 176
struct Cyc_List_List*_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp40))->impl))->fields;
_tmp42=_tmp43;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E;if(_tmp41->tag != 12)goto _LLF;else{_tmp42=_tmp41->f2;}}_LLE:
# 179
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10: {
# 181
struct Cyc_Core_Impossible_exn_struct _tmp430;const char*_tmp42F;void*_tmp42E[1];struct Cyc_String_pa_PrintArg_struct _tmp42D;struct Cyc_Core_Impossible_exn_struct*_tmp42C;(int)_throw((void*)((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=((_tmp430.tag=Cyc_Core_Impossible,((_tmp430.f1=(struct _dyneither_ptr)((_tmp42D.tag=0,((_tmp42D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp42E[0]=& _tmp42D,Cyc_aprintf(((_tmp42F="get_field_index failed: %s",_tag_dyneither(_tmp42F,sizeof(char),27))),_tag_dyneither(_tmp42E,sizeof(void*),1)))))))),_tmp430)))),_tmp42C)))));}_LLA:;}
# 186
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp431;struct _tuple0 _tmp4A=(_tmp431.f1=r1,((_tmp431.f2=r2,_tmp431)));struct Cyc_Absyn_Vardecl*_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4E;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp54;int _tmp58;int _tmp5A;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f1;if(_tmp4B->tag != 0)goto _LL14;else{_tmp4C=_tmp4B->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A.f2;if(_tmp4D->tag != 0)goto _LL14;else{_tmp4E=_tmp4D->f1;}};_LL13:
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
{struct _dyneither_ptr*_tmp43E;const char*_tmp43D;void*_tmp43C[1];struct Cyc_String_pa_PrintArg_struct _tmp43B;struct Cyc_List_List*_tmp43A;sl=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->hd=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=((_tmp43B.tag=0,((_tmp43B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp66->name).f2),((_tmp43C[0]=& _tmp43B,Cyc_aprintf(((_tmp43D="%s",_tag_dyneither(_tmp43D,sizeof(char),3))),_tag_dyneither(_tmp43C,sizeof(void*),1)))))))),_tmp43E)))),((_tmp43A->tl=sl,_tmp43A))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp67=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp64;if(_tmp67->tag != 1)goto _LL25;else{_tmp68=_tmp67->f1;}}_LL24:
{struct _dyneither_ptr*_tmp44B;const char*_tmp44A;void*_tmp449[1];struct Cyc_Int_pa_PrintArg_struct _tmp448;struct Cyc_List_List*_tmp447;sl=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->hd=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=((_tmp448.tag=1,((_tmp448.f1=(unsigned long)((int)_tmp68),((_tmp449[0]=& _tmp448,Cyc_aprintf(((_tmp44A="mpt%d",_tag_dyneither(_tmp44A,sizeof(char),6))),_tag_dyneither(_tmp449,sizeof(void*),1)))))))),_tmp44B)))),((_tmp447->tl=sl,_tmp447))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp69=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp64;if(_tmp69->tag != 2)goto _LL20;else{_tmp6A=_tmp69->f1;}}_LL26:
{struct _dyneither_ptr*_tmp458;const char*_tmp457;void*_tmp456[1];struct Cyc_Int_pa_PrintArg_struct _tmp455;struct Cyc_List_List*_tmp454;sl=((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454->hd=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458[0]=((_tmp455.tag=1,((_tmp455.f1=(unsigned long)_tmp6A,((_tmp456[0]=& _tmp455,Cyc_aprintf(((_tmp457="param%d",_tag_dyneither(_tmp457,sizeof(char),8))),_tag_dyneither(_tmp456,sizeof(void*),1)))))))),_tmp458)))),((_tmp454->tl=sl,_tmp454))))));}goto _LL20;_LL20:;}
# 225
for(0;_tmp63 != 0;_tmp63=_tmp63->tl){
struct _dyneither_ptr*_tmp465;const char*_tmp464;void*_tmp463[1];struct Cyc_Int_pa_PrintArg_struct _tmp462;struct Cyc_List_List*_tmp461;sl=((_tmp461=_cycalloc(sizeof(*_tmp461)),((_tmp461->hd=((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465[0]=((_tmp462.tag=1,((_tmp462.f1=(unsigned long)((int)_tmp63->hd),((_tmp463[0]=& _tmp462,Cyc_aprintf(((_tmp464="%d",_tag_dyneither(_tmp464,sizeof(char),3))),_tag_dyneither(_tmp463,sizeof(void*),1)))))))),_tmp465)))),((_tmp461->tl=sl,_tmp461))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp470;const char*_tmp46F;void*_tmp46E[1];const char*_tmp46D;struct _dyneither_ptr*_tmp46C;struct _dyneither_ptr*_tmp7F=(_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C[0]=((_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((_tmp46F="",_tag_dyneither(_tmp46F,sizeof(char),1))),((_tmp46E[0]=& _tmp470,Cyc_aprintf(((_tmp46D="%s",_tag_dyneither(_tmp46D,sizeof(char),3))),_tag_dyneither(_tmp46E,sizeof(void*),1)))))))),_tmp46C)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp475;void*_tmp474[2];struct Cyc_String_pa_PrintArg_struct _tmp473;struct Cyc_String_pa_PrintArg_struct _tmp472;*_tmp7F=((_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F),((_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp474[0]=& _tmp473,((_tmp474[1]=& _tmp472,Cyc_aprintf(((_tmp475="%s.%s",_tag_dyneither(_tmp475,sizeof(char),6))),_tag_dyneither(_tmp474,sizeof(void*),2))))))))))))));}
return _tmp7F;};}
# 243 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp47B(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp47A,unsigned int*_tmp479,void***_tmp477){for(*_tmp47A=0;*_tmp47A < *_tmp479;(*_tmp47A)++){(*_tmp477)[*_tmp47A]=(*fenv)->unknown_all;}}
# 248
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 254
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp47A;unsigned int _tmp479;struct _dyneither_ptr _tmp478;void**_tmp477;unsigned int _tmp476;struct _dyneither_ptr d=(_tmp476=sz,((_tmp477=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp476)),((_tmp478=_tag_dyneither(_tmp477,sizeof(void*),_tmp476),((((_tmp479=_tmp476,_tmp47B(& fenv,& _tmp47A,& _tmp479,& _tmp477))),_tmp478)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct _dyneither_ptr*_tmp8A;void*_tmp8B;struct Cyc_Absyn_Aggrfield*_tmp89=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp8A=(*_tmp89).name;_tmp8B=(*_tmp89).type;
if(_get_dyneither_size(*_tmp8A,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp8B,leafval);}}
# 261
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 265
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 269
struct _RegionHandle*_tmp91;struct Cyc_List_List*_tmp92;struct _tuple15*_tmp90=env;_tmp91=(*_tmp90).f1;_tmp92=(*_tmp90).f2;{
void*_tmp93=Cyc_Tcutil_rsubstitute(_tmp91,_tmp92,f->type);
struct Cyc_Absyn_Aggrfield*_tmp47C;return(_tmp47C=_region_malloc(_tmp91,sizeof(*_tmp47C)),((_tmp47C->name=f->name,((_tmp47C->tq=f->tq,((_tmp47C->type=_tmp93,((_tmp47C->width=f->width,((_tmp47C->attributes=f->attributes,_tmp47C)))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 278
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 284
struct _RegionHandle _tmp95=_new_region("temp");struct _RegionHandle*temp=& _tmp95;_push_region(temp);
# 288
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp96=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp97=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp98=Cyc_Tcutil_tvar_kind(_tmp96,& Cyc_Tcutil_bk);_LL28: if((*_tmp98).kind != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
 goto _LL2B;_LL2A: if((*_tmp98).kind != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
 continue;_LL2C:;_LL2D:
 goto _LL27;_LL27:;}{
# 297
struct _tuple16*_tmp47F;struct Cyc_List_List*_tmp47E;inst=((_tmp47E=_region_malloc(temp,sizeof(*_tmp47E)),((_tmp47E->hd=((_tmp47F=_region_malloc(temp,sizeof(*_tmp47F)),((_tmp47F->f1=_tmp96,((_tmp47F->f2=_tmp97,_tmp47F)))))),((_tmp47E->tl=inst,_tmp47E))))));};}
# 299
if(inst != 0){
struct _tuple15 _tmp480;struct _tuple15 _tmp9B=(_tmp480.f1=temp,((_tmp480.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp480)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp9B,fs);
struct _dyneither_ptr _tmp9C=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp9C;}else{
# 304
struct _dyneither_ptr _tmp9E=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp9E;}}
# 288
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp486(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp485,unsigned int*_tmp484,void***_tmp482){for(*_tmp485=0;*_tmp485 < *_tmp484;(*_tmp485)++){(*_tmp482)[*_tmp485]=(*fenv)->unknown_all;}}
# 308
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 312
int is_tagged=0;
# 316
{void*_tmp9F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Datatypefield*_tmpA1;struct Cyc_List_List*_tmpA3;union Cyc_Absyn_AggrInfoU _tmpA5;struct Cyc_List_List*_tmpA6;enum Cyc_Absyn_AggrKind _tmpA8;struct Cyc_List_List*_tmpA9;void*_tmpAB;union Cyc_Absyn_Constraint*_tmpAC;void*_tmpAE;union Cyc_Absyn_Constraint*_tmpB0;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9F;if(_tmpA0->tag != 4)goto _LL31;else{if((((_tmpA0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL31;_tmpA1=((struct _tuple2)(((_tmpA0->f1).field_info).KnownDatatypefield).val).f2;}}_LL30:
# 318
 if(_tmpA1->typs == 0)
return leafval;
_tmpA3=_tmpA1->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9F;if(_tmpA2->tag != 10)goto _LL33;else{_tmpA3=_tmpA2->f1;}}_LL32: {
# 322
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA3);
# 325
unsigned int _tmp485;unsigned int _tmp484;struct _dyneither_ptr _tmp483;void**_tmp482;unsigned int _tmp481;struct _dyneither_ptr d=(_tmp481=sz,((_tmp482=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp481)),((_tmp483=_tag_dyneither(_tmp482,sizeof(void*),_tmp481),((((_tmp484=_tmp481,_tmp486(& fenv,& _tmp485,& _tmp484,& _tmp482))),_tmp483)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpA3))->hd)).f2,leafval);
_tmpA3=_tmpA3->tl;}}{
# 330
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp48C;struct Cyc_CfFlowInfo_UnionRInfo _tmp48B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp48A;return(void*)((_tmp48A=_region_malloc(fenv->r,sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp48C.tag=6,((_tmp48C.f1=((_tmp48B.is_union=0,((_tmp48B.fieldnum=- 1,_tmp48B)))),((_tmp48C.f2=d,_tmp48C)))))),_tmp48A))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F;if(_tmpA4->tag != 11)goto _LL35;else{_tmpA5=(_tmpA4->f1).aggr_info;_tmpA6=(_tmpA4->f1).targs;}}_LL34: {
# 332
struct Cyc_Absyn_Aggrdecl*_tmpB8=Cyc_Absyn_get_known_aggrdecl(_tmpA5);
if(_tmpB8->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->tagged)return fenv->unknown_all;
is_tagged=1;
if(_tmpB8->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpB9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields;
if(_tmpA6 == 0){_tmpA8=_tmpB8->kind;_tmpA9=_tmpB9;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp492;struct Cyc_CfFlowInfo_UnionRInfo _tmp491;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp490;return(void*)((_tmp490=_region_malloc(fenv->r,sizeof(*_tmp490)),((_tmp490[0]=(
(_tmp492.tag=6,((_tmp492.f1=((_tmp491.is_union=_tmpB8->kind == Cyc_Absyn_UnionA,((_tmp491.fieldnum=- 1,_tmp491)))),((_tmp492.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpB8->tvs,_tmpA6,_tmpB9,_tmpB8->kind == Cyc_Absyn_UnionA,leafval),_tmp492)))))),_tmp490))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9F;if(_tmpA7->tag != 12)goto _LL37;else{_tmpA8=_tmpA7->f1;_tmpA9=_tmpA7->f2;}}_LL36:
# 345
 if(_tmpA8 == Cyc_Absyn_UnionA  && !is_tagged)return fenv->unknown_all;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp498;struct Cyc_CfFlowInfo_UnionRInfo _tmp497;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp496;return(void*)((_tmp496=_region_malloc(fenv->r,sizeof(*_tmp496)),((_tmp496[0]=((_tmp498.tag=6,((_tmp498.f1=((_tmp497.is_union=_tmpA8 == Cyc_Absyn_UnionA,((_tmp497.fieldnum=- 1,_tmp497)))),((_tmp498.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpA9,_tmpA8 == Cyc_Absyn_UnionA,leafval),_tmp498)))))),_tmp496))));};_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F;if(_tmpAA->tag != 8)goto _LL39;else{_tmpAB=(_tmpAA->f1).elt_type;_tmpAC=(_tmpAA->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAC))goto _LL39;_LL38:
# 350
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpAB)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9F;if(_tmpAD->tag != 19)goto _LL3B;else{_tmpAE=(void*)_tmpAD->f1;}}_LL3A:
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F;if(_tmpAF->tag != 5)goto _LL3D;else{_tmpB0=((_tmpAF->f1).ptr_atts).nullable;}}if(!(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0)))goto _LL3D;_LL3C:
{void*_tmpC0=leafval;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0;if(_tmpC1->tag != 3)goto _LL42;else{if(_tmpC1->f1 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0;if(_tmpC2->tag != 3)goto _LL44;else{if(_tmpC2->f1 != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
 return fenv->notzeroall;_LL44:;_LL45:
 goto _LL3F;_LL3F:;}
# 358
goto _LL2E;_LL3D:;_LL3E:
 goto _LL2E;_LL2E:;}
# 362
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 365
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 369
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpC3=r;struct Cyc_Absyn_Exp*_tmpC5;int _tmpC6;void*_tmpC7;int _tmpC9;int _tmpCA;struct _dyneither_ptr _tmpCB;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC4->tag != 7)goto _LL49;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;_tmpC7=(void*)_tmpC4->f3;}}_LL48:
# 372
 if(_tmpC5 == e  && _tmpC6 == env_iteration){
*needs_unconsume=1;
return 0;}
# 376
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC8->tag != 6)goto _LL4B;else{_tmpC9=(_tmpC8->f1).is_union;_tmpCA=(_tmpC8->f1).fieldnum;_tmpCB=_tmpC8->f2;}}_LL4A:
# 378
 if(!_tmpC9  || _tmpCA == - 1){
unsigned int _tmpCC=_get_dyneither_size(_tmpCB,sizeof(void*));
{int i=0;for(0;i < _tmpCC;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCB,sizeof(void*),i)),needs_unconsume))
return 1;}}
# 384
return 0;}else{
# 387
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCB,sizeof(void*),_tmpCA)),needs_unconsume);}_LL4B:;_LL4C:
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp49E(struct _dyneither_ptr*_tmpD4,unsigned int*_tmp49D,unsigned int*_tmp49C,void***_tmp49A){for(*_tmp49D=0;*_tmp49D < *_tmp49C;(*_tmp49D)++){(*_tmp49A)[*_tmp49D]=*((void**)_check_dyneither_subscript(*_tmpD4,sizeof(void*),(int)*_tmp49D));}}
# 392
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpCD=r;struct Cyc_Absyn_Exp*_tmpCF;int _tmpD0;void*_tmpD1;struct Cyc_CfFlowInfo_UnionRInfo _tmpD3;struct _dyneither_ptr _tmpD4;_LL4E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpCE=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCD;if(_tmpCE->tag != 7)goto _LL50;else{_tmpCF=_tmpCE->f1;_tmpD0=_tmpCE->f2;_tmpD1=(void*)_tmpCE->f3;}}_LL4F:
# 395
 return _tmpD1;_LL50: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD;if(_tmpD2->tag != 6)goto _LL52;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD2->f2;}}_LL51: {
# 397
unsigned int _tmpD5=_get_dyneither_size(_tmpD4,sizeof(void*));
int change=0;
unsigned int _tmp49D;unsigned int _tmp49C;struct _dyneither_ptr _tmp49B;void**_tmp49A;unsigned int _tmp499;struct _dyneither_ptr d2=(_tmp499=_tmpD5,((_tmp49A=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp499)),((_tmp49B=_tag_dyneither(_tmp49A,sizeof(void*),_tmp499),((((_tmp49C=_tmp499,_tmp49E(& _tmpD4,& _tmp49D,& _tmp49C,& _tmp49A))),_tmp49B)))))));
{int i=0;for(0;i < _tmpD5;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmpD4,sizeof(void*),i)));
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmpD4,sizeof(void*),i)))
change=1;}}
# 405
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4A1;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4A0;return(void*)((_tmp4A0=_region_malloc(fenv->r,sizeof(*_tmp4A0)),((_tmp4A0[0]=((_tmp4A1.tag=6,((_tmp4A1.f1=_tmpD3,((_tmp4A1.f2=d2,_tmp4A1)))))),_tmp4A0))));}else{
return r;}}_LL52:;_LL53:
 return r;_LL4D:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp4AB(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4AA,unsigned int*_tmp4A9,void***_tmp4A7){for(*_tmp4AA=0;*_tmp4AA < *_tmp4A9;(*_tmp4AA)++){(*_tmp4A7)[*_tmp4AA]=(*fenv)->unknown_all;}}static void _tmp4B4(struct _dyneither_ptr*_tmpEF,unsigned int*_tmp4B3,unsigned int*_tmp4B2,void***_tmp4B0){for(*_tmp4B3=0;*_tmp4B3 < *_tmp4B2;(*_tmp4B3)++){(*_tmp4B0)[*_tmp4B3]=((void**)(*_tmpEF).curr)[(int)*_tmp4B3];}}
# 412
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4A4;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4A3;return(void*)((_tmp4A3=_region_malloc(fenv->r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((_tmp4A4.tag=7,((_tmp4A4.f1=consumer,((_tmp4A4.f2=iteration,((_tmp4A4.f3=(void*)r,_tmp4A4)))))))),_tmp4A3))));}{
struct _tuple0 _tmp4A5;struct _tuple0 _tmpDF=(_tmp4A5.f1=Cyc_Tcutil_compress(t),((_tmp4A5.f2=r,_tmp4A5)));struct Cyc_List_List*_tmpE1;struct Cyc_CfFlowInfo_UnionRInfo _tmpE3;struct _dyneither_ptr _tmpE4;union Cyc_Absyn_AggrInfoU _tmpE6;struct Cyc_CfFlowInfo_UnionRInfo _tmpE8;struct _dyneither_ptr _tmpE9;enum Cyc_Absyn_AggrKind _tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_CfFlowInfo_UnionRInfo _tmpEE;struct _dyneither_ptr _tmpEF;_LL55:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDF.f1;if(_tmpE0->tag != 10)goto _LL57;else{_tmpE1=_tmpE0->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDF.f2;if(_tmpE2->tag != 6)goto _LL57;else{_tmpE3=_tmpE2->f1;_tmpE4=_tmpE2->f2;}};_LL56: {
# 417
unsigned int _tmpF0=_get_dyneither_size(_tmpE4,sizeof(void*));
unsigned int _tmp4AA;unsigned int _tmp4A9;struct _dyneither_ptr _tmp4A8;void**_tmp4A7;unsigned int _tmp4A6;struct _dyneither_ptr d2=(_tmp4A6=_tmpF0,((_tmp4A7=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4A6)),((_tmp4A8=_tag_dyneither(_tmp4A7,sizeof(void*),_tmp4A6),((((_tmp4A9=_tmp4A6,_tmp4AB(& fenv,& _tmp4AA,& _tmp4A9,& _tmp4A7))),_tmp4A8)))))));
{int i=0;for(0;i < _tmpF0;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpE1))->hd)).f2,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpE4,sizeof(void*),i)));
_tmpE1=_tmpE1->tl;}}{
# 423
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4AE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4AD;return(void*)((_tmp4AD=_region_malloc(fenv->r,sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=6,((_tmp4AE.f1=_tmpE3,((_tmp4AE.f2=d2,_tmp4AE)))))),_tmp4AD))));};}_LL57:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDF.f1;if(_tmpE5->tag != 11)goto _LL59;else{_tmpE6=(_tmpE5->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpE7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDF.f2;if(_tmpE7->tag != 6)goto _LL59;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;}};_LL58: {
# 425
struct Cyc_Absyn_Aggrdecl*_tmpF7=Cyc_Absyn_get_known_aggrdecl(_tmpE6);
if(_tmpF7->impl == 0)return r;
_tmpEB=_tmpF7->kind;_tmpEC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF7->impl))->fields;_tmpEE=_tmpE8;_tmpEF=_tmpE9;goto _LL5A;}_LL59:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDF.f1;if(_tmpEA->tag != 12)goto _LL5B;else{_tmpEB=_tmpEA->f1;_tmpEC=_tmpEA->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpED=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDF.f2;if(_tmpED->tag != 6)goto _LL5B;else{_tmpEE=_tmpED->f1;_tmpEF=_tmpED->f2;}};_LL5A: {
# 429
unsigned int _tmp4B3;unsigned int _tmp4B2;struct _dyneither_ptr _tmp4B1;void**_tmp4B0;unsigned int _tmp4AF;struct _dyneither_ptr d2=(_tmp4AF=_get_dyneither_size(_tmpEF,sizeof(void*)),((_tmp4B0=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4AF)),((_tmp4B1=_tag_dyneither(_tmp4B0,sizeof(void*),_tmp4AF),((((_tmp4B2=_tmp4AF,_tmp4B4(& _tmpEF,& _tmp4B3,& _tmp4B2,& _tmp4B0))),_tmp4B1)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpEC);
{int i=0;for(0;i < sz;(i ++,_tmpEC=_tmpEC->tl)){
struct _dyneither_ptr*_tmpF9;void*_tmpFA;struct Cyc_Absyn_Aggrfield*_tmpF8=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpEC))->hd;_tmpF9=(*_tmpF8).name;_tmpFA=(*_tmpF8).type;
if(_get_dyneither_size(*_tmpF9,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpFA,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpEF,sizeof(void*),i)));}}{
# 436
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4B7;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4B6;return(void*)((_tmp4B6=_region_malloc(fenv->r,sizeof(*_tmp4B6)),((_tmp4B6[0]=((_tmp4B7.tag=6,((_tmp4B7.f1=_tmpEE,((_tmp4B7.f2=d2,_tmp4B7)))))),_tmp4B6))));};}_LL5B:;_LL5C:
 return r;_LL54:;};}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 441
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 444
struct _RegionHandle _tmp101=_new_region("r");struct _RegionHandle*r=& _tmp101;_push_region(r);
{struct _tuple18 _tmp4B8;struct _tuple18 elem=(_tmp4B8.f1=place,((_tmp4B8.f2=0,_tmp4B8)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp102=elem.f1;
void*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_CfFlowInfo_Place _tmp103=*place;_tmp104=_tmp103.root;_tmp105=_tmp103.fields;{
void*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_CfFlowInfo_Place _tmp106=*_tmp102;_tmp107=_tmp106.root;_tmp108=_tmp106.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmp104,_tmp107)!= 0)
continue;
for(0;_tmp105 != 0  && _tmp108 != 0;(_tmp105=_tmp105->tl,_tmp108=_tmp108->tl)){
if((int)_tmp105->hd != (int)_tmp108->hd)break;}
if(_tmp105 == 0){
int _tmp109=1;_npop_handler(0);return _tmp109;}};}{
# 458
int _tmp10A=0;_npop_handler(0);return _tmp10A;};}
# 445
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 471
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 473
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 476
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp4B9;pile->places=((_tmp4B9=_region_malloc(pile->rgn,sizeof(*_tmp4B9)),((_tmp4B9->hd=place,((_tmp4B9->tl=pile->places,_tmp4B9))))));}}
# 479
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp10D=r;void*_tmp10F;struct Cyc_CfFlowInfo_Place*_tmp111;struct Cyc_CfFlowInfo_UnionRInfo _tmp113;struct _dyneither_ptr _tmp114;_LL5E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10D;if(_tmp10E->tag != 7)goto _LL60;else{_tmp10F=(void*)_tmp10E->f3;}}_LL5F:
 Cyc_CfFlowInfo_add_places(pile,_tmp10F);return;_LL60: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp10D;if(_tmp110->tag != 5)goto _LL62;else{_tmp111=_tmp110->f1;}}_LL61:
 Cyc_CfFlowInfo_add_place(pile,_tmp111);return;_LL62: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp112=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp10D;if(_tmp112->tag != 6)goto _LL64;else{_tmp113=_tmp112->f1;_tmp114=_tmp112->f2;}}_LL63:
# 484
{int i=0;for(0;i < _get_dyneither_size(_tmp114,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp114.curr)[i]);}}
return;_LL64:;_LL65:
 return;_LL5D:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp4BF(struct _RegionHandle**r,void**new_val,unsigned int*_tmp4BE,unsigned int*_tmp4BD,void***_tmp4BB,struct _dyneither_ptr*_tmp118){for(*_tmp4BE=0;*_tmp4BE < *_tmp4BD;(*_tmp4BE)++){(*_tmp4BB)[*_tmp4BE]=
# 499
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp118).curr)[(int)*_tmp4BE]);}}
# 494
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 496
void*_tmp115=old_val;int _tmp117;struct _dyneither_ptr _tmp118;struct Cyc_Absyn_Exp*_tmp11A;int _tmp11B;void*_tmp11C;_LL67: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp116=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115;if(_tmp116->tag != 6)goto _LL69;else{_tmp117=(_tmp116->f1).is_union;_tmp118=_tmp116->f2;}}_LL68: {
# 498
unsigned int _tmp4BE;unsigned int _tmp4BD;struct _dyneither_ptr _tmp4BC;void**_tmp4BB;unsigned int _tmp4BA;struct _dyneither_ptr d2=(_tmp4BA=_get_dyneither_size(_tmp118,sizeof(void*)),((_tmp4BB=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp4BA)),((_tmp4BC=_tag_dyneither(_tmp4BB,sizeof(void*),_tmp4BA),((((_tmp4BD=_tmp4BA,_tmp4BF(& r,& new_val,& _tmp4BE,& _tmp4BD,& _tmp4BB,& _tmp118))),_tmp4BC)))))));
# 502
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4C5;struct Cyc_CfFlowInfo_UnionRInfo _tmp4C4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C3;return(void*)((_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C5.tag=6,((_tmp4C5.f1=((_tmp4C4.is_union=_tmp117,((_tmp4C4.fieldnum=- 1,_tmp4C4)))),((_tmp4C5.f2=d2,_tmp4C5)))))),_tmp4C3))));}_LL69: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp119=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115;if(_tmp119->tag != 7)goto _LL6B;else{_tmp11A=_tmp119->f1;_tmp11B=_tmp119->f2;_tmp11C=(void*)_tmp119->f3;}}_LL6A: {
# 504
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4C8;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4C7;return(void*)((_tmp4C7=_region_malloc(r,sizeof(*_tmp4C7)),((_tmp4C7[0]=((_tmp4C8.tag=7,((_tmp4C8.f1=_tmp11A,((_tmp4C8.f2=_tmp11B,((_tmp4C8.f3=(void*)Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp11C),_tmp4C8)))))))),_tmp4C7))));}_LL6B:;_LL6C:
 return new_val;_LL66:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp4CE(struct _dyneither_ptr*d,unsigned int*_tmp4CD,unsigned int*_tmp4CC,void***_tmp4CA){for(*_tmp4CD=0;*_tmp4CD < *_tmp4CC;(*_tmp4CD)++){(*_tmp4CA)[*_tmp4CD]=((void**)(*d).curr)[(int)*_tmp4CD];}}
# 511
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 513
void*_tmp126=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp126 == rval)return d;{
unsigned int _tmp4CD;unsigned int _tmp4CC;struct _dyneither_ptr _tmp4CB;void**_tmp4CA;unsigned int _tmp4C9;struct _dyneither_ptr res=(_tmp4C9=_get_dyneither_size(d,sizeof(void*)),((_tmp4CA=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp4C9)),((_tmp4CB=_tag_dyneither(_tmp4CA,sizeof(void*),_tmp4C9),((((_tmp4CC=_tmp4C9,_tmp4CE(& d,& _tmp4CD,& _tmp4CC,& _tmp4CA))),_tmp4CB)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 525
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 529
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp4CF;struct _tuple19 _tmp12C=(_tmp4CF.f1=fs,((_tmp4CF.f2=old_val,_tmp4CF)));int _tmp12D;struct Cyc_List_List*_tmp12E;int _tmp130;struct _dyneither_ptr _tmp131;_LL6E: if(_tmp12C.f1 == 0)goto _LL70;_tmp12D=(int)(*_tmp12C.f1).hd;_tmp12E=(*_tmp12C.f1).tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp12F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12C.f2;if(_tmp12F->tag != 6)goto _LL70;else{_tmp130=(_tmp12F->f1).is_union;_tmp131=_tmp12F->f2;}};_LL6F: {
# 533
void*_tmp132=Cyc_CfFlowInfo_insert_place_outer(r,_tmp12E,*((void**)_check_dyneither_subscript(_tmp131,sizeof(void*),_tmp12D)),new_val);
struct _dyneither_ptr _tmp133=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp131,_tmp12D,_tmp132);
if((void**)_tmp133.curr == (void**)_tmp131.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4D5;struct Cyc_CfFlowInfo_UnionRInfo _tmp4D4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D3;return(void*)((_tmp4D3=_region_malloc(r,sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D5.tag=6,((_tmp4D5.f1=((_tmp4D4.is_union=_tmp130,((_tmp4D4.fieldnum=- 1,_tmp4D4)))),((_tmp4D5.f2=_tmp133,_tmp4D5)))))),_tmp4D3))));}}_LL70:;_LL71: {
struct Cyc_Core_Impossible_exn_struct _tmp4DB;const char*_tmp4DA;struct Cyc_Core_Impossible_exn_struct*_tmp4D9;(int)_throw((void*)((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DB.tag=Cyc_Core_Impossible,((_tmp4DB.f1=((_tmp4DA="bad insert place",_tag_dyneither(_tmp4DA,sizeof(char),17))),_tmp4DB)))),_tmp4D9)))));}_LL6D:;};}
# 544
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 548
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp13A=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp13A,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp13B;_push_handler(& _tmp13B);{int _tmp13D=0;if(setjmp(_tmp13B.handler))_tmp13D=1;if(!_tmp13D){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp13A);;_pop_handler();}else{void*_tmp13C=(void*)_exn_thrown;void*_tmp13F=_tmp13C;void*_tmp141;_LL73: {struct Cyc_Dict_Absent_exn_struct*_tmp140=(struct Cyc_Dict_Absent_exn_struct*)_tmp13F;if(_tmp140->tag != Cyc_Dict_Absent)goto _LL75;}_LL74:
 continue;_LL75: _tmp141=_tmp13F;_LL76:(void)_throw(_tmp141);_LL72:;}};}
switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL77:
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL78:
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL79:
 newval=fenv->esc_none;break;}
# 560
Cyc_CfFlowInfo_add_places(pile,oldval);{
void*_tmp143;struct Cyc_List_List*_tmp144;struct Cyc_CfFlowInfo_Place _tmp142=*_tmp13A;_tmp143=_tmp142.root;_tmp144=_tmp142.fields;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp143,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp144,
# 565
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp143),newval));};};}
# 568
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 580
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp145=r;enum Cyc_CfFlowInfo_InitLevel _tmp147;enum Cyc_CfFlowInfo_InitLevel _tmp149;void*_tmp14E;_LL7C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp146=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp146->tag != 3)goto _LL7E;else{_tmp147=_tmp146->f1;}}_LL7D:
 return _tmp147;_LL7E: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp148=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp148->tag != 4)goto _LL80;else{_tmp149=_tmp148->f1;}}_LL7F:
 return _tmp149;_LL80: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp14A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp14A->tag != 0)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp14B=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp14B->tag != 1)goto _LL84;}_LL83:
 return Cyc_CfFlowInfo_AllIL;_LL84: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp14C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp14C->tag != 2)goto _LL86;}_LL85:
 return Cyc_CfFlowInfo_ThisIL;_LL86: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp14D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp145;if(_tmp14D->tag != 7)goto _LL88;else{_tmp14E=(void*)_tmp14D->f3;}}_LL87:
 return Cyc_CfFlowInfo_NoneIL;_LL88:;_LL89: {
struct Cyc_Core_Impossible_exn_struct _tmp4E1;const char*_tmp4E0;struct Cyc_Core_Impossible_exn_struct*_tmp4DF;(int)_throw((void*)((_tmp4DF=_cycalloc(sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E1.tag=Cyc_Core_Impossible,((_tmp4E1.f1=((_tmp4E0="initlevel_approx",_tag_dyneither(_tmp4E0,sizeof(char),17))),_tmp4E1)))),_tmp4DF)))));}_LL7B:;}
# 591
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 593
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp152=r;int _tmp154;struct _dyneither_ptr _tmp155;struct _dyneither_ptr _tmp157;struct Cyc_CfFlowInfo_Place*_tmp159;_LL8B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp153=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152;if(_tmp153->tag != 6)goto _LL8D;else{if((_tmp153->f1).is_union != 1)goto _LL8D;_tmp154=(_tmp153->f1).fieldnum;_tmp155=_tmp153->f2;}}_LL8C: {
# 599
unsigned int _tmp15A=_get_dyneither_size(_tmp155,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp154 == - 1){
int i=0;for(0;i < _tmp15A;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp155,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 609
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp155,sizeof(void*),_tmp154)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL8A;}_LL8D: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp156=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp152;if(_tmp156->tag != 6)goto _LL8F;else{if((_tmp156->f1).is_union != 0)goto _LL8F;_tmp157=_tmp156->f2;}}_LL8E: {
# 613
unsigned int _tmp15B=_get_dyneither_size(_tmp157,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp15B;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp157,sizeof(void*),i)),this_ans);}}
goto _LL8A;}_LL8F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp158=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp152;if(_tmp158->tag != 5)goto _LL91;else{_tmp159=_tmp158->f1;}}_LL90:
# 619
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp159))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 622
{struct Cyc_List_List*_tmp4E2;env->seen=((_tmp4E2=_region_malloc(env->e,sizeof(*_tmp4E2)),((_tmp4E2->hd=_tmp159,((_tmp4E2->tl=env->seen,_tmp4E2))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp159),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 628
goto _LL8A;_LL91:;_LL92:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL8A:;}
# 631
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 635
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp15D=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp4E3;struct Cyc_CfFlowInfo_InitlevelEnv _tmp15E=(_tmp4E3.e=_tmp15D,((_tmp4E3.d=d,((_tmp4E3.seen=0,_tmp4E3)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp15E,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 642
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_CfFlowInfo_Place _tmp160=*place;_tmp161=_tmp160.root;_tmp162=_tmp160.fields;{
void*_tmp163=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp161);
for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct _tuple20 _tmp4E4;struct _tuple20 _tmp165=(_tmp4E4.f1=_tmp163,((_tmp4E4.f2=(int)_tmp162->hd,_tmp4E4)));struct Cyc_CfFlowInfo_UnionRInfo _tmp167;struct _dyneither_ptr _tmp168;int _tmp169;_LL94:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp166=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp165.f1;if(_tmp166->tag != 6)goto _LL96;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_tmp169=_tmp165.f2;_LL95:
# 648
 _tmp163=*((void**)_check_dyneither_subscript(_tmp168,sizeof(void*),_tmp169));
goto _LL93;_LL96:;_LL97: {
# 654
struct Cyc_Core_Impossible_exn_struct _tmp4EA;const char*_tmp4E9;struct Cyc_Core_Impossible_exn_struct*_tmp4E8;(int)_throw((void*)((_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4EA.tag=Cyc_Core_Impossible,((_tmp4EA.f1=((_tmp4E9="bad lookup_place",_tag_dyneither(_tmp4E9,sizeof(char),17))),_tmp4EA)))),_tmp4E8)))));}_LL93:;}
# 656
return _tmp163;};}
# 659
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp16D=rval;void*_tmp170;int _tmp172;int _tmp173;struct _dyneither_ptr _tmp174;_LL99: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp16E=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp16E->tag != 4)goto _LL9B;}_LL9A:
 return 0;_LL9B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp16F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp16F->tag != 7)goto _LL9D;else{_tmp170=(void*)_tmp16F->f3;}}_LL9C:
 return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL9D: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp171=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D;if(_tmp171->tag != 6)goto _LL9F;else{_tmp172=(_tmp171->f1).is_union;_tmp173=(_tmp171->f1).fieldnum;_tmp174=_tmp171->f2;}}_LL9E:
# 664
 if(_tmp172  && _tmp173 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp174,sizeof(void*),_tmp173)));else{
# 667
unsigned int sz=_get_dyneither_size(_tmp174,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp174.curr)[i]))return 0;}}
return 1;}_LL9F:;_LLA0:
# 672
 return 1;_LL98:;}
# 675
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 680
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp175=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp4EB;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp4EB=_region_malloc(_tmp175,sizeof(*_tmp4EB)),((_tmp4EB->rgn=_tmp175,((_tmp4EB->places=0,_tmp4EB)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp4F5(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp4F4,unsigned int*_tmp4F3,void***_tmp4F1,struct _dyneither_ptr*_tmp182,struct _dyneither_ptr*_tmp17F){for(*_tmp4F4=0;*_tmp4F4 < *_tmp4F3;(*_tmp4F4)++){(*_tmp4F1)[*_tmp4F4]=
# 712
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp17F).curr)[(int)*_tmp4F4],*((void**)_check_dyneither_subscript(*_tmp182,sizeof(void*),(int)*_tmp4F4)));}}
# 698
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 703
struct _tuple0 _tmp4EC;struct _tuple0 _tmp178=(_tmp4EC.f1=oldval,((_tmp4EC.f2=newval,_tmp4EC)));struct Cyc_CfFlowInfo_Place*_tmp17B;struct Cyc_CfFlowInfo_UnionRInfo _tmp17E;struct _dyneither_ptr _tmp17F;struct Cyc_CfFlowInfo_UnionRInfo _tmp181;struct _dyneither_ptr _tmp182;enum Cyc_CfFlowInfo_InitLevel _tmp184;_LLA2:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp179=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp178.f1;if(_tmp179->tag != 4)goto _LLA4;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp17A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp178.f2;if(_tmp17A->tag != 5)goto _LLA4;else{_tmp17B=_tmp17A->f1;}};_LLA3:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp17B);goto _LLA5;_LLA4: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp17C=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp178.f1;if(_tmp17C->tag != 4)goto _LLA6;}_LLA5:
# 706
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,Cyc_Tcutil_terr(env->loc,((_tmp4EF="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp4EF,sizeof(char),67))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}
# 709
return(env->fenv)->esc_all;_LLA6:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp17D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp178.f1;if(_tmp17D->tag != 6)goto _LLA8;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp180=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp178.f2;if(_tmp180->tag != 6)goto _LLA8;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}};_LLA7: {
# 711
unsigned int _tmp4F4;unsigned int _tmp4F3;struct _dyneither_ptr _tmp4F2;void**_tmp4F1;unsigned int _tmp4F0;struct _dyneither_ptr new_d=
(_tmp4F0=_get_dyneither_size(_tmp17F,sizeof(void*)),((_tmp4F1=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp4F0)),((_tmp4F2=_tag_dyneither(_tmp4F1,sizeof(void*),_tmp4F0),((((_tmp4F3=_tmp4F0,_tmp4F5(& env,& _tmp4F4,& _tmp4F3,& _tmp4F1,& _tmp182,& _tmp17F))),_tmp4F2)))))));
# 715
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp17F,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp17F.curr)[i]){
change=1;break;}}}
# 720
if(!change){
if(!_tmp17E.is_union)return oldval;
new_d=_tmp17F;}else{
# 725
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp17F,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp182,sizeof(void*),i))){
change=1;break;}}}
# 730
if(!change){
if(!_tmp17E.is_union)return newval;
new_d=_tmp182;}}{
# 735
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F8;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F7;return(void*)((_tmp4F7=_region_malloc((env->fenv)->r,sizeof(*_tmp4F7)),((_tmp4F7[0]=((_tmp4F8.tag=6,((_tmp4F8.f1=_tmp181,((_tmp4F8.f2=new_d,_tmp4F8)))))),_tmp4F7))));};}_LLA8: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp183=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp178.f2;if(_tmp183->tag != 4)goto _LLAA;else{_tmp184=_tmp183->f1;}}_LLA9:
# 737
 switch(_tmp184){case Cyc_CfFlowInfo_NoneIL: _LLAC:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLAD:
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLAE:
 return(env->fenv)->unknown_all;}_LLAA:;_LLAB:
# 742
 return newval;_LLA1:;}
# 748
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 752
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 757
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp4F9;struct _tuple19 _tmp18E=(_tmp4F9.f1=fs,((_tmp4F9.f2=oldval,_tmp4F9)));int _tmp18F;struct Cyc_List_List*_tmp190;int _tmp192;int _tmp193;struct _dyneither_ptr _tmp194;_LLB1: if(_tmp18E.f1 == 0)goto _LLB3;_tmp18F=(int)(*_tmp18E.f1).hd;_tmp190=(*_tmp18E.f1).tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp191=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp18E.f2;if(_tmp191->tag != 6)goto _LLB3;else{_tmp192=(_tmp191->f1).is_union;_tmp193=(_tmp191->f1).fieldnum;_tmp194=_tmp191->f2;}};_LLB2: {
# 760
void*_tmp195=Cyc_CfFlowInfo_assign_place_outer(env,_tmp190,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp194,sizeof(void*),_tmp18F)),newval);
# 762
struct _dyneither_ptr _tmp196=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp194,_tmp18F,_tmp195);
if((void**)_tmp196.curr == (void**)_tmp194.curr  && (!_tmp192  || _tmp193 == _tmp18F))return oldval;
_tmp194=_tmp196;
# 767
if(_tmp192){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp194,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp18F){
struct _dyneither_ptr _tmp197=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp194,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp194,sizeof(void*),i))));
# 777
if((void**)_tmp197.curr != (void**)_tmp194.curr){
_tmp194=_tmp197;
changed=1;}}}}
# 785
if(changed){
void*_tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_CfFlowInfo_Place*_tmp198=env->root_place;_tmp199=(*_tmp198).root;_tmp19A=(*_tmp198).fields;{
int*_tmp4FA;struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp4FA=_cycalloc_atomic(sizeof(*_tmp4FA)),((_tmp4FA[0]=fs_prefix,_tmp4FA)))),_tmp19A);
struct Cyc_CfFlowInfo_Place*_tmp4FB;struct Cyc_CfFlowInfo_Place*_tmp19C=(_tmp4FB=_region_malloc((env->pile)->rgn,sizeof(*_tmp4FB)),((_tmp4FB->root=_tmp199,((_tmp4FB->fields=_tmp19B,_tmp4FB)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp19C,0);};}}{
# 792
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp501;struct Cyc_CfFlowInfo_UnionRInfo _tmp500;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4FF;return(void*)((_tmp4FF=_region_malloc((env->fenv)->r,sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp501.tag=6,((_tmp501.f1=((_tmp500.is_union=_tmp192,((_tmp500.fieldnum=_tmp18F,_tmp500)))),((_tmp501.f2=_tmp194,_tmp501)))))),_tmp4FF))));};}_LLB3:;_LLB4: {
struct Cyc_Core_Impossible_exn_struct _tmp507;const char*_tmp506;struct Cyc_Core_Impossible_exn_struct*_tmp505;(int)_throw((void*)((_tmp505=_cycalloc(sizeof(*_tmp505)),((_tmp505[0]=((_tmp507.tag=Cyc_Core_Impossible,((_tmp507.f1=((_tmp506="bad assign place",_tag_dyneither(_tmp506,sizeof(char),17))),_tmp507)))),_tmp505)))));}_LLB0:;};}
# 796
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 801
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1A5=fenv->r;
void*_tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_CfFlowInfo_Place*_tmp1A6=place;_tmp1A7=(*_tmp1A6).root;_tmp1A8=(*_tmp1A6).fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp50A;struct Cyc_CfFlowInfo_AssignEnv _tmp509;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp509.fenv=fenv,((_tmp509.pile=((_tmp50A=_region_malloc(_tmp1A5,sizeof(*_tmp50A)),((_tmp50A->rgn=_tmp1A5,((_tmp50A->places=0,_tmp50A)))))),((_tmp509.d=d,((_tmp509.loc=loc,((_tmp509.root_place=place,((_tmp509.all_changed=all_changed,_tmp509)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1A8,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1A7),r);
# 809
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1A7,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 820
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 830
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 832
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp51A(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp519,unsigned int*_tmp518,void***_tmp516,struct _dyneither_ptr*_tmp1DD,struct _dyneither_ptr*_tmp1D9){for(*_tmp519=0;*_tmp519 < *_tmp518;(*_tmp519)++){(*_tmp516)[*_tmp519]=
# 898 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1D9).curr)[(int)*_tmp519],*((void**)_check_dyneither_subscript(*_tmp1DD,sizeof(void*),(int)*_tmp519)));}}
# 842
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1AB=(env->fenv)->r;
# 846
{struct _tuple0 _tmp50B;struct _tuple0 _tmp1AD=(_tmp50B.f1=r1,((_tmp50B.f2=r2,_tmp50B)));struct Cyc_Absyn_Exp*_tmp1AF;int _tmp1B0;void*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;int _tmp1B4;void*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;int _tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BB;int _tmp1BC;void*_tmp1BD;struct Cyc_CfFlowInfo_Place*_tmp1BF;struct Cyc_CfFlowInfo_Place*_tmp1C1;struct Cyc_CfFlowInfo_Place*_tmp1C3;struct Cyc_CfFlowInfo_Place*_tmp1C7;struct Cyc_CfFlowInfo_Place*_tmp1C9;struct Cyc_CfFlowInfo_Place*_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp1CF;struct Cyc_CfFlowInfo_Place*_tmp1D1;int _tmp1D7;int _tmp1D8;struct _dyneither_ptr _tmp1D9;int _tmp1DB;int _tmp1DC;struct _dyneither_ptr _tmp1DD;_LLB6:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1AE=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1AE->tag != 7)goto _LLB8;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;_tmp1B1=(void*)_tmp1AE->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1B2->tag != 7)goto _LLB8;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=(void*)_tmp1B2->f3;}};_LLB7:
# 849
 if(_tmp1AF == _tmp1B3  && _tmp1B0 == _tmp1B4){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp50E;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp50D;return(void*)((_tmp50D=_region_malloc(_tmp1AB,sizeof(*_tmp50D)),((_tmp50D[0]=((_tmp50E.tag=7,((_tmp50E.f1=_tmp1AF,((_tmp50E.f2=_tmp1B0,((_tmp50E.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B1,_tmp1B5),_tmp50E)))))))),_tmp50D))));}
{void*_tmp1E0=_tmp1B1;struct Cyc_CfFlowInfo_Place*_tmp1E2;_LLD3: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0;if(_tmp1E1->tag != 5)goto _LLD5;else{_tmp1E2=_tmp1E1->f1;}}_LLD4:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1E2);goto _LLD2;_LLD5:;_LLD6:
 goto _LLD2;_LLD2:;}
# 855
{void*_tmp1E3=_tmp1B5;struct Cyc_CfFlowInfo_Place*_tmp1E5;_LLD8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E4=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3;if(_tmp1E4->tag != 5)goto _LLDA;else{_tmp1E5=_tmp1E4->f1;}}_LLD9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1E5);goto _LLD7;_LLDA:;_LLDB:
 goto _LLD7;_LLD7:;}
# 859
goto _LLB5;_LLB8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1B6->tag != 7)goto _LLBA;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;_tmp1B9=(void*)_tmp1B6->f3;}}_LLB9: {
# 861
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp511;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp510;return(void*)((_tmp510=_region_malloc(_tmp1AB,sizeof(*_tmp510)),((_tmp510[0]=((_tmp511.tag=7,((_tmp511.f1=_tmp1B7,((_tmp511.f2=_tmp1B8,((_tmp511.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,_tmp1B9,r2),_tmp511)))))))),_tmp510))));}_LLBA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1BA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1BA->tag != 7)goto _LLBC;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;_tmp1BD=(void*)_tmp1BA->f3;}}_LLBB: {
# 863
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp514;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp513;return(void*)((_tmp513=_region_malloc(_tmp1AB,sizeof(*_tmp513)),((_tmp513[0]=((_tmp514.tag=7,((_tmp514.f1=_tmp1BB,((_tmp514.f2=_tmp1BC,((_tmp514.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1BD),_tmp514)))))))),_tmp513))));}_LLBC:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1BE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1BE->tag != 5)goto _LLBE;else{_tmp1BF=_tmp1BE->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1C0->tag != 5)goto _LLBE;else{_tmp1C1=_tmp1C0->f1;}};_LLBD:
# 866
 if(Cyc_CfFlowInfo_place_cmp(_tmp1BF,_tmp1C1)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1BF);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1C1);
goto _LLB5;_LLBE:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1C2->tag != 5)goto _LLC0;else{_tmp1C3=_tmp1C2->f1;}}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp1C4=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1C4->tag != 2)goto _LLC0;};_LLBF:
# 871
 _tmp1C7=_tmp1C3;goto _LLC1;_LLC0:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp1C5=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1C5->tag != 2)goto _LLC2;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C6=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1C6->tag != 5)goto _LLC2;else{_tmp1C7=_tmp1C6->f1;}};_LLC1:
# 873
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1C7);
return(env->fenv)->notzerothis;_LLC2:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1C8->tag != 5)goto _LLC4;else{_tmp1C9=_tmp1C8->f1;}}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1CA=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1CA->tag != 1)goto _LLC4;};_LLC3:
# 877
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1C9);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLDC:
 return(env->fenv)->notzeroall;default: _LLDD:
 return(env->fenv)->notzerothis;}_LLC4:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1CB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1CB->tag != 1)goto _LLC6;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1CC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1CC->tag != 5)goto _LLC6;else{_tmp1CD=_tmp1CC->f1;}};_LLC5:
# 883
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CD);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL: _LLDF:
 return(env->fenv)->notzeroall;default: _LLE0:
 return(env->fenv)->notzerothis;}_LLC6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1CE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1CE->tag != 5)goto _LLC8;else{_tmp1CF=_tmp1CE->f1;}}_LLC7:
# 889
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1CF);goto _LLB5;_LLC8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1D0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1D0->tag != 5)goto _LLCA;else{_tmp1D1=_tmp1D0->f1;}}_LLC9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D1);goto _LLB5;_LLCA:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1D2=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1D2->tag != 1)goto _LLCC;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp1D3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1D3->tag != 2)goto _LLCC;};_LLCB:
# 893
 goto _LLCD;_LLCC:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp1D4=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1D4->tag != 2)goto _LLCE;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1D5=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1D5->tag != 1)goto _LLCE;};_LLCD:
 return(env->fenv)->notzerothis;_LLCE:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1D6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f1;if(_tmp1D6->tag != 6)goto _LLD0;else{_tmp1D7=(_tmp1D6->f1).is_union;_tmp1D8=(_tmp1D6->f1).fieldnum;_tmp1D9=_tmp1D6->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1DA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AD.f2;if(_tmp1DA->tag != 6)goto _LLD0;else{_tmp1DB=(_tmp1DA->f1).is_union;_tmp1DC=(_tmp1DA->f1).fieldnum;_tmp1DD=_tmp1DA->f2;}};_LLCF: {
# 897
unsigned int _tmp519;unsigned int _tmp518;struct _dyneither_ptr _tmp517;void**_tmp516;unsigned int _tmp515;struct _dyneither_ptr new_d=
(_tmp515=_get_dyneither_size(_tmp1D9,sizeof(void*)),((_tmp516=(void**)_region_malloc(_tmp1AB,_check_times(sizeof(void*),_tmp515)),((_tmp517=_tag_dyneither(_tmp516,sizeof(void*),_tmp515),((((_tmp518=_tmp515,_tmp51A(& env,& _tmp519,& _tmp518,& _tmp516,& _tmp1DD,& _tmp1D9))),_tmp517)))))));
# 902
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1D9,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1D9.curr)[i]){
change=1;break;}}}
# 907
if(!change){
if(!_tmp1D7)return r1;
new_d=_tmp1D9;}else{
# 912
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1D9,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1DD,sizeof(void*),i))){
change=1;break;}}}
# 917
if(!change){
if(!_tmp1D7)return r2;
new_d=_tmp1DD;}}{
# 922
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp520;struct Cyc_CfFlowInfo_UnionRInfo _tmp51F;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp51E;return(void*)((_tmp51E=_region_malloc(_tmp1AB,sizeof(*_tmp51E)),((_tmp51E[0]=((_tmp520.tag=6,((_tmp520.f1=((_tmp51F.is_union=_tmp1D7,((_tmp51F.fieldnum=_tmp1D8 == _tmp1DC?_tmp1D8: - 1,_tmp51F)))),((_tmp520.f2=new_d,_tmp520)))))),_tmp51E))));};}_LLD0:;_LLD1:
# 924
 goto _LLB5;_LLB5:;}{
# 926
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp521;struct _tuple0 _tmp1F2=(_tmp521.f1=r1,((_tmp521.f2=r2,_tmp521)));_LLE3: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1F3=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1F2.f1;if(_tmp1F3->tag != 4)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1F4=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1F2.f2;if(_tmp1F4->tag != 4)goto _LLE7;}_LLE6: {
# 931
struct _tuple21 _tmp522;struct _tuple21 _tmp1F6=(_tmp522.f1=il1,((_tmp522.f2=il2,_tmp522)));_LLEA: if(_tmp1F6.f2 != Cyc_CfFlowInfo_NoneIL)goto _LLEC;_LLEB:
 goto _LLED;_LLEC: if(_tmp1F6.f1 != Cyc_CfFlowInfo_NoneIL)goto _LLEE;_LLED: return(env->fenv)->esc_none;_LLEE: if(_tmp1F6.f2 != Cyc_CfFlowInfo_ThisIL)goto _LLF0;_LLEF:
 goto _LLF1;_LLF0: if(_tmp1F6.f1 != Cyc_CfFlowInfo_ThisIL)goto _LLF2;_LLF1: return(env->fenv)->esc_this;_LLF2:;_LLF3:
 return(env->fenv)->esc_all;_LLE9:;}_LLE7:;_LLE8: {
# 937
struct _tuple21 _tmp523;struct _tuple21 _tmp1F8=(_tmp523.f1=il1,((_tmp523.f2=il2,_tmp523)));_LLF5: if(_tmp1F8.f2 != Cyc_CfFlowInfo_NoneIL)goto _LLF7;_LLF6:
 goto _LLF8;_LLF7: if(_tmp1F8.f1 != Cyc_CfFlowInfo_NoneIL)goto _LLF9;_LLF8: return(env->fenv)->unknown_none;_LLF9: if(_tmp1F8.f2 != Cyc_CfFlowInfo_ThisIL)goto _LLFB;_LLFA:
 goto _LLFC;_LLFB: if(_tmp1F8.f1 != Cyc_CfFlowInfo_ThisIL)goto _LLFD;_LLFC: return(env->fenv)->unknown_this;_LLFD:;_LLFE:
 return(env->fenv)->unknown_all;_LLF4:;}_LLE2:;};};}
# 945
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp524;return((_tmp524.EqualConst).val=i,(((_tmp524.EqualConst).tag=1,_tmp524)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*v,void*t){
struct _tuple11 _tmp527;union Cyc_CfFlowInfo_RelnOp _tmp526;return((_tmp526.LessVar).val=((_tmp527.f1=v,((_tmp527.f2=t,_tmp527)))),(((_tmp526.LessVar).tag=2,_tmp526)));}
# 949
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp528;return((_tmp528.LessNumelts).val=v,(((_tmp528.LessNumelts).tag=3,_tmp528)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp529;return((_tmp529.LessConst).val=i,(((_tmp529.LessConst).tag=4,_tmp529)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp52A;return((_tmp52A.LessEqConst).val=i,(((_tmp52A.LessEqConst).tag=6,_tmp52A)));}
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp52B;return((_tmp52B.LessEqNumelts).val=v,(((_tmp52B.LessEqNumelts).tag=5,_tmp52B)));}struct _tuple22{union Cyc_CfFlowInfo_RelnOp f1;union Cyc_CfFlowInfo_RelnOp f2;};
# 957
static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp r1,union Cyc_CfFlowInfo_RelnOp r2){
struct _tuple22 _tmp52C;struct _tuple22 _tmp201=(_tmp52C.f1=r1,((_tmp52C.f2=r2,_tmp52C)));unsigned int _tmp202;unsigned int _tmp203;struct Cyc_Absyn_Vardecl*_tmp204;struct Cyc_Absyn_Vardecl*_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;struct Cyc_Absyn_Vardecl*_tmp207;unsigned int _tmp208;unsigned int _tmp209;unsigned int _tmp20A;unsigned int _tmp20B;struct Cyc_Absyn_Vardecl*_tmp20C;struct Cyc_Absyn_Vardecl*_tmp20D;_LL100: if(((_tmp201.f1).EqualConst).tag != 1)goto _LL102;_tmp202=(unsigned int)((_tmp201.f1).EqualConst).val;if(((_tmp201.f2).EqualConst).tag != 1)goto _LL102;_tmp203=(unsigned int)((_tmp201.f2).EqualConst).val;_LL101:
 return _tmp202 == _tmp203;_LL102: if(((_tmp201.f1).LessVar).tag != 2)goto _LL104;_tmp204=((struct _tuple11)((_tmp201.f1).LessVar).val).f1;if(((_tmp201.f2).LessVar).tag != 2)goto _LL104;_tmp205=((struct _tuple11)((_tmp201.f2).LessVar).val).f1;_LL103:
 return _tmp204 == _tmp205;_LL104: if(((_tmp201.f1).LessNumelts).tag != 3)goto _LL106;_tmp206=(struct Cyc_Absyn_Vardecl*)((_tmp201.f1).LessNumelts).val;if(((_tmp201.f2).LessNumelts).tag != 3)goto _LL106;_tmp207=(struct Cyc_Absyn_Vardecl*)((_tmp201.f2).LessNumelts).val;_LL105:
 return _tmp206 == _tmp207;_LL106: if(((_tmp201.f1).LessConst).tag != 4)goto _LL108;_tmp208=(unsigned int)((_tmp201.f1).LessConst).val;if(((_tmp201.f2).LessConst).tag != 4)goto _LL108;_tmp209=(unsigned int)((_tmp201.f2).LessConst).val;_LL107:
 return _tmp208 == _tmp209;_LL108: if(((_tmp201.f1).LessEqConst).tag != 6)goto _LL10A;_tmp20A=(unsigned int)((_tmp201.f1).LessEqConst).val;if(((_tmp201.f2).LessEqConst).tag != 6)goto _LL10A;_tmp20B=(unsigned int)((_tmp201.f2).LessEqConst).val;_LL109:
 return _tmp20A == _tmp20B;_LL10A: if(((_tmp201.f1).LessEqNumelts).tag != 5)goto _LL10C;_tmp20C=(struct Cyc_Absyn_Vardecl*)((_tmp201.f1).LessEqNumelts).val;if(((_tmp201.f2).LessEqNumelts).tag != 5)goto _LL10C;_tmp20D=(struct Cyc_Absyn_Vardecl*)((_tmp201.f2).LessEqNumelts).val;_LL10B:
 return _tmp20C == _tmp20D;_LL10C:;_LL10D:
 return 0;_LLFF:;}
# 968
static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
# 972
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp20E=r1s;for(0;_tmp20E != 0;_tmp20E=_tmp20E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp20F=(struct Cyc_CfFlowInfo_Reln*)_tmp20E->hd;
int found=0;
{struct Cyc_List_List*_tmp210=r2s;for(0;_tmp210 != 0;_tmp210=_tmp210->tl){
struct Cyc_CfFlowInfo_Reln*_tmp211=(struct Cyc_CfFlowInfo_Reln*)_tmp210->hd;
if(_tmp20F == _tmp211  || _tmp20F->vd == _tmp211->vd  && Cyc_CfFlowInfo_same_relop(_tmp20F->rop,_tmp211->rop)){
{struct Cyc_List_List*_tmp52D;res=((_tmp52D=_region_malloc(r,sizeof(*_tmp52D)),((_tmp52D->hd=_tmp20F,((_tmp52D->tl=res,_tmp52D))))));}
found=1;
break;}}}
# 985
if(!found)diff=1;}}
# 987
if(!diff)res=r1s;
# 989
return res;};}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 992
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 995
struct _tuple23 _tmp52E;struct _tuple23 _tmp214=(_tmp52E.f1=f1,((_tmp52E.f2=f2,_tmp52E)));struct Cyc_Dict_Dict _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_Dict_Dict _tmp217;struct Cyc_List_List*_tmp218;_LL10F: if(((_tmp214.f1).BottomFL).tag != 1)goto _LL111;_LL110:
 return f2;_LL111: if(((_tmp214.f2).BottomFL).tag != 1)goto _LL113;_LL112:
 return f1;_LL113: if(((_tmp214.f1).ReachableFL).tag != 2)goto _LL10E;_tmp215=((struct _tuple12)((_tmp214.f1).ReachableFL).val).f1;_tmp216=((struct _tuple12)((_tmp214.f1).ReachableFL).val).f2;if(((_tmp214.f2).ReachableFL).tag != 2)goto _LL10E;_tmp217=((struct _tuple12)((_tmp214.f2).ReachableFL).val).f1;_tmp218=((struct _tuple12)((_tmp214.f2).ReachableFL).val).f2;_LL114:
# 1001
 if(_tmp215.t == _tmp217.t  && _tmp216 == _tmp218)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp219=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp531;struct Cyc_CfFlowInfo_JoinEnv _tmp530;struct Cyc_CfFlowInfo_JoinEnv _tmp21A=(_tmp530.fenv=fenv,((_tmp530.pile=((_tmp531=_region_malloc(_tmp219,sizeof(*_tmp531)),((_tmp531->rgn=_tmp219,((_tmp531->places=0,_tmp531)))))),((_tmp530.d1=_tmp215,((_tmp530.d2=_tmp217,_tmp530)))))));
struct Cyc_Dict_Dict _tmp21B=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp21A,_tmp215,_tmp217);
struct Cyc_List_List*_tmp21C=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp216,_tmp218);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp21A.pile,all_changed,_tmp21B),_tmp21C);};_LL10E:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1013
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1020
union Cyc_CfFlowInfo_FlowInfo _tmp220;void*_tmp221;struct _tuple13 _tmp21F=pr1;_tmp220=_tmp21F.f1;_tmp221=_tmp21F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp223;void*_tmp224;struct _tuple13 _tmp222=pr2;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp220,_tmp223);
struct _tuple24 _tmp532;struct _tuple24 _tmp226=(_tmp532.f1=_tmp220,((_tmp532.f2=_tmp223,((_tmp532.f3=outflow,_tmp532)))));struct Cyc_Dict_Dict _tmp227;struct Cyc_Dict_Dict _tmp228;struct Cyc_Dict_Dict _tmp229;struct Cyc_List_List*_tmp22A;_LL116: if(((_tmp226.f1).BottomFL).tag != 1)goto _LL118;_LL117: {
struct _tuple13 _tmp533;return(_tmp533.f1=outflow,((_tmp533.f2=_tmp224,_tmp533)));}_LL118: if(((_tmp226.f2).BottomFL).tag != 1)goto _LL11A;_LL119: {
struct _tuple13 _tmp534;return(_tmp534.f1=outflow,((_tmp534.f2=_tmp221,_tmp534)));}_LL11A: if(((_tmp226.f1).ReachableFL).tag != 2)goto _LL11C;_tmp227=((struct _tuple12)((_tmp226.f1).ReachableFL).val).f1;if(((_tmp226.f2).ReachableFL).tag != 2)goto _LL11C;_tmp228=((struct _tuple12)((_tmp226.f2).ReachableFL).val).f1;if(((_tmp226.f3).ReachableFL).tag != 2)goto _LL11C;_tmp229=((struct _tuple12)((_tmp226.f3).ReachableFL).val).f1;_tmp22A=((struct _tuple12)((_tmp226.f3).ReachableFL).val).f2;_LL11B:
# 1028
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp221,_tmp224)){struct _tuple13 _tmp535;return(_tmp535.f1=outflow,((_tmp535.f2=_tmp224,_tmp535)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp224,_tmp221)){struct _tuple13 _tmp536;return(_tmp536.f1=outflow,((_tmp536.f2=_tmp221,_tmp536)));}{
struct _RegionHandle*_tmp22F=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp539;struct Cyc_CfFlowInfo_JoinEnv _tmp538;struct Cyc_CfFlowInfo_JoinEnv _tmp230=(_tmp538.fenv=fenv,((_tmp538.pile=((_tmp539=_region_malloc(_tmp22F,sizeof(*_tmp539)),((_tmp539->rgn=_tmp22F,((_tmp539->places=0,_tmp539)))))),((_tmp538.d1=_tmp227,((_tmp538.d2=_tmp228,_tmp538)))))));
void*_tmp231=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp230,0,_tmp221,_tmp224);
struct _tuple13 _tmp53A;return(_tmp53A.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp230.pile,all_changed,_tmp229),_tmp22A),((_tmp53A.f2=_tmp231,_tmp53A)));};_LL11C:;_LL11D: {
# 1036
struct Cyc_Core_Impossible_exn_struct _tmp540;const char*_tmp53F;struct Cyc_Core_Impossible_exn_struct*_tmp53E;(int)_throw((void*)((_tmp53E=_cycalloc(sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp540.tag=Cyc_Core_Impossible,((_tmp540.f1=((_tmp53F="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp53F,sizeof(char),37))),_tmp540)))),_tmp53E)))));}_LL115:;};};}
# 1040
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp547(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp546,unsigned int*_tmp545,void***_tmp543,struct _dyneither_ptr*_tmp243,struct _dyneither_ptr*_tmp23F){for(*_tmp546=0;*_tmp546 < *_tmp545;(*_tmp546)++){(*_tmp543)[*_tmp546]=
# 1069
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp546,((void**)(*_tmp23F).curr)[(int)*_tmp546],*((void**)_check_dyneither_subscript(*_tmp243,sizeof(void*),(int)*_tmp546)));}}
# 1041
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1049
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1054
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1062
struct _tuple0 _tmp541;struct _tuple0 _tmp239=(_tmp541.f1=r1,((_tmp541.f2=r2,_tmp541)));int _tmp23D;int _tmp23E;struct _dyneither_ptr _tmp23F;int _tmp241;int _tmp242;struct _dyneither_ptr _tmp243;_LL11F: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp23A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp239.f1;if(_tmp23A->tag != 7)goto _LL121;}_LL120:
 goto _LL122;_LL121: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp23B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp239.f2;if(_tmp23B->tag != 7)goto _LL123;}_LL122:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL123:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp23C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp239.f1;if(_tmp23C->tag != 6)goto _LL125;else{_tmp23D=(_tmp23C->f1).is_union;_tmp23E=(_tmp23C->f1).fieldnum;_tmp23F=_tmp23C->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp240=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp239.f2;if(_tmp240->tag != 6)goto _LL125;else{_tmp241=(_tmp240->f1).is_union;_tmp242=(_tmp240->f1).fieldnum;_tmp243=_tmp240->f2;}};_LL124: {
# 1067
unsigned int _tmp546;unsigned int _tmp545;struct _dyneither_ptr _tmp544;void**_tmp543;unsigned int _tmp542;struct _dyneither_ptr new_d=
(_tmp542=
_get_dyneither_size(_tmp23F,sizeof(void*)),((_tmp543=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp542)),((_tmp544=_tag_dyneither(_tmp543,sizeof(void*),_tmp542),((((_tmp545=_tmp542,_tmp547(& env,& _tmp546,& _tmp545,& _tmp543,& _tmp243,& _tmp23F))),_tmp544)))))));
# 1074
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp23F,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp23F.curr)[i]){
change=1;break;}}}
# 1079
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp23F,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp243,sizeof(void*),i))){
change=1;break;}}}
# 1086
if(!change)
return r2;{
# 1090
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp54D;struct Cyc_CfFlowInfo_UnionRInfo _tmp54C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp54B;return(void*)((_tmp54B=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp54B)),((_tmp54B[0]=((_tmp54D.tag=6,((_tmp54D.f1=((_tmp54C.is_union=_tmp23D,((_tmp54C.fieldnum=_tmp23E == _tmp242?_tmp23E: - 1,_tmp54C)))),((_tmp54D.f2=new_d,_tmp54D)))))),_tmp54B))));};}_LL125:;_LL126: {
# 1098
struct Cyc_Core_Impossible_exn_struct _tmp553;const char*_tmp552;struct Cyc_Core_Impossible_exn_struct*_tmp551;(int)_throw((void*)((_tmp551=_cycalloc(sizeof(*_tmp551)),((_tmp551[0]=((_tmp553.tag=Cyc_Core_Impossible,((_tmp553.f1=((_tmp552="after_absRval -- non-aggregates!",_tag_dyneither(_tmp552,sizeof(char),33))),_tmp553)))),_tmp551)))));}_LL11E:;};}
# 1101
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1104
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp24E=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp24F=env->changed;
{struct Cyc_List_List*_tmp554;*env->last_field_cell=((_tmp554=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp554)),((_tmp554->hd=(void*)key,((_tmp554->tl=0,_tmp554))))));}
# 1110
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp251=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp24E;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp24F;
return _tmp251;};};}
# 1117
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1119
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp555;*env->curr_place=((_tmp555.root=root,((_tmp555.fields=0,_tmp555))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1126
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1132
struct _tuple23 _tmp556;struct _tuple23 _tmp254=(_tmp556.f1=f1,((_tmp556.f2=f2,_tmp556)));struct Cyc_Dict_Dict _tmp255;struct Cyc_List_List*_tmp256;struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp258;_LL128: if(((_tmp254.f1).BottomFL).tag != 1)goto _LL12A;_LL129:
 goto _LL12B;_LL12A: if(((_tmp254.f2).BottomFL).tag != 1)goto _LL12C;_LL12B:
 return Cyc_CfFlowInfo_BottomFL();_LL12C: if(((_tmp254.f1).ReachableFL).tag != 2)goto _LL127;_tmp255=((struct _tuple12)((_tmp254.f1).ReachableFL).val).f1;_tmp256=((struct _tuple12)((_tmp254.f1).ReachableFL).val).f2;if(((_tmp254.f2).ReachableFL).tag != 2)goto _LL127;_tmp257=((struct _tuple12)((_tmp254.f2).ReachableFL).val).f1;_tmp258=((struct _tuple12)((_tmp254.f2).ReachableFL).val).f2;_LL12D:
# 1136
 if(_tmp255.t == _tmp257.t  && _tmp256 == _tmp258)return f1;{
# 1139
struct _RegionHandle*_tmp259=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp25A=fenv->dummy_place;
struct Cyc_CfFlowInfo_JoinEnv _tmp55C;struct Cyc_CfFlowInfo_EscPile*_tmp55B;struct Cyc_CfFlowInfo_AfterEnv _tmp55A;struct Cyc_CfFlowInfo_AfterEnv _tmp25B=(_tmp55A.joinenv=((_tmp55C.fenv=fenv,((_tmp55C.pile=((_tmp55B=_region_malloc(_tmp259,sizeof(*_tmp55B)),((_tmp55B->rgn=_tmp259,((_tmp55B->places=0,_tmp55B)))))),((_tmp55C.d1=_tmp255,((_tmp55C.d2=_tmp257,_tmp55C)))))))),((_tmp55A.chg1=chg1,((_tmp55A.chg2=chg2,((_tmp55A.curr_place=_tmp25A,((_tmp55A.last_field_cell=& _tmp25A->fields,((_tmp55A.changed=Cyc_CfFlowInfo_Neither,_tmp55A)))))))))));
# 1145
struct Cyc_Dict_Dict _tmp25C=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp25B,_tmp255,_tmp257);
struct Cyc_List_List*_tmp25D=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp256,_tmp258);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp25B.joinenv).pile,all_changed,_tmp25C),_tmp25D);};_LL127:;}
# 1154
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1157
{struct _tuple0 _tmp55D;struct _tuple0 _tmp262=(_tmp55D.f1=r1,((_tmp55D.f2=r2,_tmp55D)));struct Cyc_CfFlowInfo_Place*_tmp264;struct Cyc_CfFlowInfo_Place*_tmp266;struct Cyc_CfFlowInfo_UnionRInfo _tmp26A;struct _dyneither_ptr _tmp26B;struct Cyc_CfFlowInfo_UnionRInfo _tmp26D;struct _dyneither_ptr _tmp26E;_LL12F:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp263=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp263->tag != 5)goto _LL131;else{_tmp264=_tmp263->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp265=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp265->tag != 5)goto _LL131;else{_tmp266=_tmp265->f1;}};_LL130:
 return Cyc_CfFlowInfo_place_cmp(_tmp264,_tmp266)== 0;_LL131: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp267=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp267->tag != 5)goto _LL133;}_LL132:
 goto _LL134;_LL133: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp268=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp268->tag != 5)goto _LL135;}_LL134:
 return 0;_LL135:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp269=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp269->tag != 6)goto _LL137;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp26C->tag != 6)goto _LL137;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26C->f2;}};_LL136:
# 1162
 if((void**)_tmp26B.curr == (void**)_tmp26E.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp26B,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp26B.curr)[i],*((void**)_check_dyneither_subscript(_tmp26E,sizeof(void*),i))))return 0;}}
return 1;_LL137:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp26F=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp26F->tag != 1)goto _LL139;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp270=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp270->tag != 2)goto _LL139;};_LL138:
 return 1;_LL139: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp271=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp271->tag != 2)goto _LL13B;}_LL13A:
 return 0;_LL13B: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp272=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp272->tag != 0)goto _LL13D;}_LL13C:
 goto _LL13E;_LL13D: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp273=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp273->tag != 1)goto _LL13F;}_LL13E:
 return 0;_LL13F:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp274=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp274->tag != 4)goto _LL141;}{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp275=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp262.f2;if(_tmp275->tag != 4)goto _LL141;};_LL140:
 goto _LL12E;_LL141: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp276=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp262.f1;if(_tmp276->tag != 4)goto _LL143;}_LL142:
 return 0;_LL143:;_LL144:
 goto _LL12E;_LL12E:;}{
# 1174
struct _tuple21 _tmp55E;struct _tuple21 _tmp278=(_tmp55E.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp55E.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp55E)));_LL146: if(_tmp278.f1 != Cyc_CfFlowInfo_AllIL)goto _LL148;if(_tmp278.f2 != Cyc_CfFlowInfo_AllIL)goto _LL148;_LL147:
 return 1;_LL148: if(_tmp278.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL14A;_LL149:
 return 1;_LL14A: if(_tmp278.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL14C;_LL14B:
 return 0;_LL14C: if(_tmp278.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL14E;_LL14D:
 return 1;_LL14E: if(_tmp278.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL145;_LL14F:
 return 0;_LL145:;};}
# 1183
static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 1187
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_CfFlowInfo_Reln*_tmp279=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp27A=r2s;for(0;_tmp27A != 0;_tmp27A=_tmp27A->tl){
struct Cyc_CfFlowInfo_Reln*_tmp27B=(struct Cyc_CfFlowInfo_Reln*)_tmp27A->hd;
if(_tmp279 == _tmp27B  || _tmp279->vd == _tmp27B->vd  && Cyc_CfFlowInfo_same_relop(_tmp279->rop,_tmp27B->rop)){
found=1;
break;}}}
# 1197
if(!found)
return 0;}
# 1200
return 1;}
# 1208
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1210
struct _tuple23 _tmp55F;struct _tuple23 _tmp27D=(_tmp55F.f1=f1,((_tmp55F.f2=f2,_tmp55F)));struct Cyc_Dict_Dict _tmp27E;struct Cyc_List_List*_tmp27F;struct Cyc_Dict_Dict _tmp280;struct Cyc_List_List*_tmp281;_LL151: if(((_tmp27D.f1).BottomFL).tag != 1)goto _LL153;_LL152:
 return 1;_LL153: if(((_tmp27D.f2).BottomFL).tag != 1)goto _LL155;_LL154:
 return 0;_LL155: if(((_tmp27D.f1).ReachableFL).tag != 2)goto _LL150;_tmp27E=((struct _tuple12)((_tmp27D.f1).ReachableFL).val).f1;_tmp27F=((struct _tuple12)((_tmp27D.f1).ReachableFL).val).f2;if(((_tmp27D.f2).ReachableFL).tag != 2)goto _LL150;_tmp280=((struct _tuple12)((_tmp27D.f2).ReachableFL).val).f1;_tmp281=((struct _tuple12)((_tmp27D.f2).ReachableFL).val).f2;_LL156:
# 1214
 if(_tmp27E.t == _tmp280.t  && _tmp27F == _tmp281)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp27E,_tmp280) && 
Cyc_CfFlowInfo_relns_approx(_tmp27F,_tmp281);_LL150:;}
# 1220
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp282=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(_tmp282->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp283=_tmp282->rop;struct Cyc_Absyn_Vardecl*_tmp284;struct Cyc_Absyn_Vardecl*_tmp285;struct Cyc_Absyn_Vardecl*_tmp286;_LL158: if((_tmp283.LessVar).tag != 2)goto _LL15A;_tmp284=((struct _tuple11)(_tmp283.LessVar).val).f1;_LL159:
 _tmp285=_tmp284;goto _LL15B;_LL15A: if((_tmp283.LessNumelts).tag != 3)goto _LL15C;_tmp285=(struct Cyc_Absyn_Vardecl*)(_tmp283.LessNumelts).val;_LL15B:
 _tmp286=_tmp285;goto _LL15D;_LL15C: if((_tmp283.LessEqNumelts).tag != 5)goto _LL15E;_tmp286=(struct Cyc_Absyn_Vardecl*)(_tmp283.LessEqNumelts).val;_LL15D:
 if(v == _tmp286)found=1;goto _LL157;_LL15E:;_LL15F:
 goto _LL157;_LL157:;};}
# 1233
if(!found)return rs;{
# 1235
struct Cyc_List_List*_tmp287=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp288=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(_tmp288->vd != v){
{union Cyc_CfFlowInfo_RelnOp _tmp289=_tmp288->rop;struct Cyc_Absyn_Vardecl*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;struct Cyc_Absyn_Vardecl*_tmp28C;_LL161: if((_tmp289.LessVar).tag != 2)goto _LL163;_tmp28A=((struct _tuple11)(_tmp289.LessVar).val).f1;_LL162:
 _tmp28B=_tmp28A;goto _LL164;_LL163: if((_tmp289.LessNumelts).tag != 3)goto _LL165;_tmp28B=(struct Cyc_Absyn_Vardecl*)(_tmp289.LessNumelts).val;_LL164:
 _tmp28C=_tmp28B;goto _LL166;_LL165: if((_tmp289.LessEqNumelts).tag != 5)goto _LL167;_tmp28C=(struct Cyc_Absyn_Vardecl*)(_tmp289.LessEqNumelts).val;_LL166:
 if(v == _tmp28C)continue;goto _LL160;_LL167:;_LL168:
 goto _LL160;_LL160:;}{
# 1245
struct Cyc_List_List*_tmp560;_tmp287=((_tmp560=_region_malloc(rgn,sizeof(*_tmp560)),((_tmp560->hd=_tmp288,((_tmp560->tl=_tmp287,_tmp560))))));};}}
# 1248
return _tmp287;};}
# 1251
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
{void*_tmp28E=e->r;struct Cyc_Absyn_Vardecl*_tmp291;struct Cyc_Absyn_Vardecl*_tmp294;struct Cyc_Absyn_Vardecl*_tmp297;struct Cyc_Absyn_Vardecl*_tmp29A;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp28E;if(_tmp28F->tag != 1)goto _LL16C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp290=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp28F->f2);if(_tmp290->tag != 1)goto _LL16C;else{_tmp291=_tmp290->f1;}}}_LL16B:
 _tmp294=_tmp291;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp28E;if(_tmp292->tag != 1)goto _LL16E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp293=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp292->f2);if(_tmp293->tag != 3)goto _LL16E;else{_tmp294=_tmp293->f1;}}}_LL16D:
 _tmp297=_tmp294;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp28E;if(_tmp295->tag != 1)goto _LL170;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp296=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp295->f2);if(_tmp296->tag != 4)goto _LL170;else{_tmp297=_tmp296->f1;}}}_LL16F:
 _tmp29A=_tmp297;goto _LL171;_LL170: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp298=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp28E;if(_tmp298->tag != 1)goto _LL172;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp299=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp298->f2);if(_tmp299->tag != 5)goto _LL172;else{_tmp29A=_tmp299->f1;}}}_LL171:
# 1257
 if(!_tmp29A->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp29A);
goto _LL169;_LL172:;_LL173:
 goto _LL169;_LL169:;}
# 1262
return r;}
# 1265
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp561;return(_tmp561=_region_malloc(r2,sizeof(*_tmp561)),((_tmp561[0]=*r,_tmp561)));}
# 1269
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
# 1273
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_CfFlowInfo_Reln*_tmp29C=(struct Cyc_CfFlowInfo_Reln*)r1->hd;
struct Cyc_CfFlowInfo_Reln*_tmp29D=(struct Cyc_CfFlowInfo_Reln*)r2->hd;
if(_tmp29C->vd != _tmp29D->vd  || !Cyc_CfFlowInfo_same_relop(_tmp29C->rop,_tmp29D->rop))return 0;}
# 1279
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 1283
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 1286
if(v->escapes)return r;
# 1289
r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);
# 1294
{void*_tmp29E=e->r;struct Cyc_Absyn_Exp*_tmp2A0;_LL175: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp29F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp29E;if(_tmp29F->tag != 32)goto _LL177;else{_tmp2A0=(_tmp29F->f1).num_elts;if((_tmp29F->f1).fat_result != 1)goto _LL177;}}_LL176:
# 1296
 malloc_loop: {
void*_tmp2A1=_tmp2A0->r;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A6;struct Cyc_Absyn_Vardecl*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AF;_LL17A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2A1;if(_tmp2A2->tag != 13)goto _LL17C;else{_tmp2A3=_tmp2A2->f2;}}_LL17B:
 _tmp2A0=_tmp2A3;goto malloc_loop;_LL17C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A1;if(_tmp2A4->tag != 1)goto _LL17E;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2A5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2A4->f2);if(_tmp2A5->tag != 5)goto _LL17E;else{_tmp2A6=_tmp2A5->f1;}}}_LL17D:
 _tmp2A9=_tmp2A6;goto _LL17F;_LL17E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A1;if(_tmp2A7->tag != 1)goto _LL180;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A8=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2A7->f2);if(_tmp2A8->tag != 4)goto _LL180;else{_tmp2A9=_tmp2A8->f1;}}}_LL17F:
 _tmp2AC=_tmp2A9;goto _LL181;_LL180: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A1;if(_tmp2AA->tag != 1)goto _LL182;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2AB=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2AA->f2);if(_tmp2AB->tag != 3)goto _LL182;else{_tmp2AC=_tmp2AB->f1;}}}_LL181:
 _tmp2AF=_tmp2AC;goto _LL183;_LL182: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2AD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2A1;if(_tmp2AD->tag != 1)goto _LL184;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2AE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2AD->f2);if(_tmp2AE->tag != 1)goto _LL184;else{_tmp2AF=_tmp2AE->f1;}}}_LL183:
# 1303
 if(_tmp2AF->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp564;struct Cyc_List_List*_tmp563;return(_tmp563=_region_malloc(rgn,sizeof(*_tmp563)),((_tmp563->hd=((_tmp564=_region_malloc(rgn,sizeof(*_tmp564)),((_tmp564->vd=_tmp2AF,((_tmp564->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp564)))))),((_tmp563->tl=r,_tmp563)))));};_LL184:;_LL185:
 return r;_LL179:;}_LL177:;_LL178:
# 1307
 goto _LL174;_LL174:;}
# 1310
{void*_tmp2B2=Cyc_Tcutil_compress(v->type);_LL187: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B2;if(_tmp2B3->tag != 6)goto _LL189;}_LL188:
 goto _LL186;_LL189:;_LL18A:
# 1313
 return r;_LL186:;}
# 1317
loop: {
void*_tmp2B4=e->r;struct Cyc_Absyn_Exp*_tmp2B6;int _tmp2B8;struct Cyc_Absyn_Exp*_tmp2BA;enum Cyc_Absyn_Primop _tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;_LL18C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B4;if(_tmp2B5->tag != 13)goto _LL18E;else{_tmp2B6=_tmp2B5->f2;}}_LL18D:
 e=_tmp2B6;goto loop;_LL18E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2B4;if(_tmp2B7->tag != 0)goto _LL190;else{if(((_tmp2B7->f1).Int_c).tag != 5)goto _LL190;_tmp2B8=((struct _tuple6)((_tmp2B7->f1).Int_c).val).f2;}}_LL18F: {
# 1321
struct Cyc_CfFlowInfo_Reln*_tmp567;struct Cyc_List_List*_tmp566;return(_tmp566=_region_malloc(rgn,sizeof(*_tmp566)),((_tmp566->hd=((_tmp567=_region_malloc(rgn,sizeof(*_tmp567)),((_tmp567->vd=v,((_tmp567->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp2B8),_tmp567)))))),((_tmp566->tl=r,_tmp566)))));}_LL190: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2B9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2B4;if(_tmp2B9->tag != 2)goto _LL192;else{if(_tmp2B9->f1 != Cyc_Absyn_Mod)goto _LL192;if(_tmp2B9->f2 == 0)goto _LL192;if((*_tmp2B9->f2).tl == 0)goto _LL192;_tmp2BA=(struct Cyc_Absyn_Exp*)(*(*_tmp2B9->f2).tl).hd;}}_LL191:
# 1325
{void*_tmp2C0=_tmp2BA->r;enum Cyc_Absyn_Primop _tmp2C2;struct Cyc_Absyn_Exp*_tmp2C3;_LL197: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2C1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C0;if(_tmp2C1->tag != 2)goto _LL199;else{_tmp2C2=_tmp2C1->f1;if(_tmp2C1->f2 == 0)goto _LL199;_tmp2C3=(struct Cyc_Absyn_Exp*)(*_tmp2C1->f2).hd;}}_LL198:
# 1327
{void*_tmp2C4=_tmp2C3->r;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2D0;_LL19C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL19E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2C6=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2C5->f2);if(_tmp2C6->tag != 1)goto _LL19E;else{_tmp2C7=_tmp2C6->f1;}}}_LL19D:
 _tmp2CA=_tmp2C7;goto _LL19F;_LL19E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C8->tag != 1)goto _LL1A0;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2C9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2C8->f2);if(_tmp2C9->tag != 4)goto _LL1A0;else{_tmp2CA=_tmp2C9->f1;}}}_LL19F:
 _tmp2CD=_tmp2CA;goto _LL1A1;_LL1A0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CB->tag != 1)goto _LL1A2;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2CB->f2);if(_tmp2CC->tag != 3)goto _LL1A2;else{_tmp2CD=_tmp2CC->f1;}}}_LL1A1:
 _tmp2D0=_tmp2CD;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CE->tag != 1)goto _LL1A4;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2CF=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2CE->f2);if(_tmp2CF->tag != 5)goto _LL1A4;else{_tmp2D0=_tmp2CF->f1;}}}_LL1A3:
# 1332
 if(_tmp2D0->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp56A;struct Cyc_List_List*_tmp569;return(_tmp569=_region_malloc(rgn,sizeof(*_tmp569)),((_tmp569->hd=((_tmp56A=_region_malloc(rgn,sizeof(*_tmp56A)),((_tmp56A->vd=v,((_tmp56A->rop=Cyc_CfFlowInfo_LessNumelts(_tmp2D0),_tmp56A)))))),((_tmp569->tl=r,_tmp569)))));};_LL1A4:;_LL1A5:
 goto _LL19B;_LL19B:;}
# 1336
goto _LL196;_LL199:;_LL19A:
 goto _LL196;_LL196:;}
# 1339
goto _LL18B;_LL192: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2BB=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2B4;if(_tmp2BB->tag != 2)goto _LL194;else{_tmp2BC=_tmp2BB->f1;if(_tmp2BB->f2 == 0)goto _LL194;_tmp2BD=(struct Cyc_Absyn_Exp*)(*_tmp2BB->f2).hd;}}_LL193:
# 1341
{void*_tmp2D3=_tmp2BD->r;struct Cyc_Absyn_Vardecl*_tmp2D6;struct Cyc_Absyn_Vardecl*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DF;_LL1A7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3;if(_tmp2D4->tag != 1)goto _LL1A9;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2D5=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2D4->f2);if(_tmp2D5->tag != 1)goto _LL1A9;else{_tmp2D6=_tmp2D5->f1;}}}_LL1A8:
 _tmp2D9=_tmp2D6;goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3;if(_tmp2D7->tag != 1)goto _LL1AB;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D8=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2D7->f2);if(_tmp2D8->tag != 4)goto _LL1AB;else{_tmp2D9=_tmp2D8->f1;}}}_LL1AA:
 _tmp2DC=_tmp2D9;goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3;if(_tmp2DA->tag != 1)goto _LL1AD;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2DB=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2DA->f2);if(_tmp2DB->tag != 3)goto _LL1AD;else{_tmp2DC=_tmp2DB->f1;}}}_LL1AC:
 _tmp2DF=_tmp2DC;goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3;if(_tmp2DD->tag != 1)goto _LL1AF;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2DE=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2DD->f2);if(_tmp2DE->tag != 5)goto _LL1AF;else{_tmp2DF=_tmp2DE->f1;}}}_LL1AE:
# 1346
 if(_tmp2DF->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp56D;struct Cyc_List_List*_tmp56C;return(_tmp56C=_region_malloc(rgn,sizeof(*_tmp56C)),((_tmp56C->hd=((_tmp56D=_region_malloc(rgn,sizeof(*_tmp56D)),((_tmp56D->vd=v,((_tmp56D->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp2DF),_tmp56D)))))),((_tmp56C->tl=r,_tmp56C)))));};_LL1AF:;_LL1B0:
 goto _LL1A6;_LL1A6:;}
# 1350
goto _LL18B;_LL194:;_LL195:
 goto _LL18B;_LL18B:;}
# 1353
return r;}
# 1356
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{void*_tmp2E2=e1->r;struct Cyc_Absyn_Vardecl*_tmp2E5;struct Cyc_Absyn_Vardecl*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EE;_LL1B2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E2;if(_tmp2E3->tag != 1)goto _LL1B4;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2E4=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2E3->f2);if(_tmp2E4->tag != 1)goto _LL1B4;else{_tmp2E5=_tmp2E4->f1;}}}_LL1B3:
 _tmp2E8=_tmp2E5;goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E2;if(_tmp2E6->tag != 1)goto _LL1B6;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2E6->f2);if(_tmp2E7->tag != 3)goto _LL1B6;else{_tmp2E8=_tmp2E7->f1;}}}_LL1B5:
 _tmp2EB=_tmp2E8;goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E2;if(_tmp2E9->tag != 1)goto _LL1B8;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EA=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2E9->f2);if(_tmp2EA->tag != 4)goto _LL1B8;else{_tmp2EB=_tmp2EA->f1;}}}_LL1B7:
 _tmp2EE=_tmp2EB;goto _LL1B9;_LL1B8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E2;if(_tmp2EC->tag != 1)goto _LL1BA;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2ED=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2EC->f2);if(_tmp2ED->tag != 5)goto _LL1BA;else{_tmp2EE=_tmp2ED->f1;}}}_LL1B9:
# 1362
 if(!_tmp2EE->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp2EE,e2);
goto _LL1B1;_LL1BA:;_LL1BB:
 goto _LL1B1;_LL1B1:;}
# 1367
return r;}
# 1370
static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){
{const char*_tmp571;void*_tmp570[1];struct Cyc_String_pa_PrintArg_struct _tmp56F;(_tmp56F.tag=0,((_tmp56F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name)),((_tmp570[0]=& _tmp56F,Cyc_fprintf(Cyc_stderr,((_tmp571="%s",_tag_dyneither(_tmp571,sizeof(char),3))),_tag_dyneither(_tmp570,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp2F2=r->rop;unsigned int _tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;unsigned int _tmp2F6;unsigned int _tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F8;_LL1BD: if((_tmp2F2.EqualConst).tag != 1)goto _LL1BF;_tmp2F3=(unsigned int)(_tmp2F2.EqualConst).val;_LL1BE:
{const char*_tmp575;void*_tmp574[1];struct Cyc_Int_pa_PrintArg_struct _tmp573;(_tmp573.tag=1,((_tmp573.f1=(unsigned long)((int)_tmp2F3),((_tmp574[0]=& _tmp573,Cyc_fprintf(Cyc_stderr,((_tmp575="==%d",_tag_dyneither(_tmp575,sizeof(char),5))),_tag_dyneither(_tmp574,sizeof(void*),1)))))));}goto _LL1BC;_LL1BF: if((_tmp2F2.LessVar).tag != 2)goto _LL1C1;_tmp2F4=((struct _tuple11)(_tmp2F2.LessVar).val).f1;_LL1C0:
{const char*_tmp579;void*_tmp578[1];struct Cyc_String_pa_PrintArg_struct _tmp577;(_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2F4->name)),((_tmp578[0]=& _tmp577,Cyc_fprintf(Cyc_stderr,((_tmp579="<%s",_tag_dyneither(_tmp579,sizeof(char),4))),_tag_dyneither(_tmp578,sizeof(void*),1)))))));}goto _LL1BC;_LL1C1: if((_tmp2F2.LessNumelts).tag != 3)goto _LL1C3;_tmp2F5=(struct Cyc_Absyn_Vardecl*)(_tmp2F2.LessNumelts).val;_LL1C2:
{const char*_tmp57D;void*_tmp57C[1];struct Cyc_String_pa_PrintArg_struct _tmp57B;(_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2F5->name)),((_tmp57C[0]=& _tmp57B,Cyc_fprintf(Cyc_stderr,((_tmp57D="<numelts(%s)",_tag_dyneither(_tmp57D,sizeof(char),13))),_tag_dyneither(_tmp57C,sizeof(void*),1)))))));}goto _LL1BC;_LL1C3: if((_tmp2F2.LessConst).tag != 4)goto _LL1C5;_tmp2F6=(unsigned int)(_tmp2F2.LessConst).val;_LL1C4:
{const char*_tmp581;void*_tmp580[1];struct Cyc_Int_pa_PrintArg_struct _tmp57F;(_tmp57F.tag=1,((_tmp57F.f1=(unsigned long)((int)_tmp2F6),((_tmp580[0]=& _tmp57F,Cyc_fprintf(Cyc_stderr,((_tmp581="<%d",_tag_dyneither(_tmp581,sizeof(char),4))),_tag_dyneither(_tmp580,sizeof(void*),1)))))));}goto _LL1BC;_LL1C5: if((_tmp2F2.LessEqConst).tag != 6)goto _LL1C7;_tmp2F7=(unsigned int)(_tmp2F2.LessEqConst).val;_LL1C6:
{const char*_tmp585;void*_tmp584[1];struct Cyc_Int_pa_PrintArg_struct _tmp583;(_tmp583.tag=1,((_tmp583.f1=(unsigned long)((int)_tmp2F7),((_tmp584[0]=& _tmp583,Cyc_fprintf(Cyc_stderr,((_tmp585="<=%d",_tag_dyneither(_tmp585,sizeof(char),5))),_tag_dyneither(_tmp584,sizeof(void*),1)))))));}goto _LL1BC;_LL1C7: if((_tmp2F2.LessEqNumelts).tag != 5)goto _LL1BC;_tmp2F8=(struct Cyc_Absyn_Vardecl*)(_tmp2F2.LessEqNumelts).val;_LL1C8:
{const char*_tmp589;void*_tmp588[1];struct Cyc_String_pa_PrintArg_struct _tmp587;(_tmp587.tag=0,((_tmp587.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2F8->name)),((_tmp588[0]=& _tmp587,Cyc_fprintf(Cyc_stderr,((_tmp589="<=numelts(%s)",_tag_dyneither(_tmp589,sizeof(char),14))),_tag_dyneither(_tmp588,sizeof(void*),1)))))));}goto _LL1BC;_LL1BC:;};}
# 1381
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp58C;void*_tmp58B;(_tmp58B=0,Cyc_fprintf(Cyc_stderr,((_tmp58C=",",_tag_dyneither(_tmp58C,sizeof(char),2))),_tag_dyneither(_tmp58B,sizeof(void*),0)));}}}
# 1388
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C9:
{const char*_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_fprintf(Cyc_stderr,((_tmp58F="uninitialized",_tag_dyneither(_tmp58F,sizeof(char),14))),_tag_dyneither(_tmp58E,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL1CA:
{const char*_tmp592;void*_tmp591;(_tmp591=0,Cyc_fprintf(Cyc_stderr,((_tmp592="this-initialized",_tag_dyneither(_tmp592,sizeof(char),17))),_tag_dyneither(_tmp591,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1CB:
{const char*_tmp595;void*_tmp594;(_tmp594=0,Cyc_fprintf(Cyc_stderr,((_tmp595="all-initialized",_tag_dyneither(_tmp595,sizeof(char),16))),_tag_dyneither(_tmp594,sizeof(void*),0)));}break;}}
# 1396
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp313=root;struct Cyc_Absyn_Vardecl*_tmp315;struct Cyc_Absyn_Exp*_tmp317;void*_tmp318;_LL1CE: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp314=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp313;if(_tmp314->tag != 0)goto _LL1D0;else{_tmp315=_tmp314->f1;}}_LL1CF:
# 1399
{const char*_tmp599;void*_tmp598[1];struct Cyc_String_pa_PrintArg_struct _tmp597;(_tmp597.tag=0,((_tmp597.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp315->name)),((_tmp598[0]=& _tmp597,Cyc_fprintf(Cyc_stderr,((_tmp599="Root(%s)",_tag_dyneither(_tmp599,sizeof(char),9))),_tag_dyneither(_tmp598,sizeof(void*),1)))))));}goto _LL1CD;_LL1D0: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp316=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp313;if(_tmp316->tag != 1)goto _LL1D2;else{_tmp317=_tmp316->f1;_tmp318=(void*)_tmp316->f2;}}_LL1D1:
# 1401
{const char*_tmp59E;void*_tmp59D[2];struct Cyc_String_pa_PrintArg_struct _tmp59C;struct Cyc_String_pa_PrintArg_struct _tmp59B;(_tmp59B.tag=0,((_tmp59B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp318)),((_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp317)),((_tmp59D[0]=& _tmp59C,((_tmp59D[1]=& _tmp59B,Cyc_fprintf(Cyc_stderr,((_tmp59E="MallocPt(%s,%s)",_tag_dyneither(_tmp59E,sizeof(char),16))),_tag_dyneither(_tmp59D,sizeof(void*),2)))))))))))));}goto _LL1CD;_LL1D2: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp319=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp313;if(_tmp319->tag != 2)goto _LL1CD;}_LL1D3:
# 1404
{const char*_tmp5A1;void*_tmp5A0;(_tmp5A0=0,Cyc_fprintf(Cyc_stderr,((_tmp5A1="InitParam(_,_)",_tag_dyneither(_tmp5A1,sizeof(char),15))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}goto _LL1CD;_LL1CD:;}
# 1408
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp323=p->fields;
while(_tmp323 != 0){
{const char*_tmp5A5;void*_tmp5A4[1];struct Cyc_Int_pa_PrintArg_struct _tmp5A3;(_tmp5A3.tag=1,((_tmp5A3.f1=(unsigned long)((int)_tmp323->hd),((_tmp5A4[0]=& _tmp5A3,Cyc_fprintf(Cyc_stderr,((_tmp5A5=".%d",_tag_dyneither(_tmp5A5,sizeof(char),4))),_tag_dyneither(_tmp5A4,sizeof(void*),1)))))));}
_tmp323=_tmp323->tl;}{
# 1415
const char*_tmp5A8;void*_tmp5A7;(_tmp5A7=0,Cyc_fprintf(Cyc_stderr,((_tmp5A8=" ",_tag_dyneither(_tmp5A8,sizeof(char),2))),_tag_dyneither(_tmp5A7,sizeof(void*),0)));};};}
# 1418
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp329=1;
while(x != 0){
if(!_tmp329){{const char*_tmp5AB;void*_tmp5AA;(_tmp5AA=0,Cyc_fprintf(Cyc_stderr,((_tmp5AB=", ",_tag_dyneither(_tmp5AB,sizeof(char),3))),_tag_dyneither(_tmp5AA,sizeof(void*),0)));}_tmp329=0;}
pr((void*)x->hd);
x=x->tl;}{
# 1425
const char*_tmp5AE;void*_tmp5AD;(_tmp5AD=0,Cyc_fprintf(Cyc_stderr,((_tmp5AE="\n",_tag_dyneither(_tmp5AE,sizeof(char),2))),_tag_dyneither(_tmp5AD,sizeof(void*),0)));};}
# 1428
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp32E=_new_region("r");struct _RegionHandle*r=& _tmp32E;_push_region(r);
{struct _handler_cons _tmp32F;_push_handler(& _tmp32F);{int _tmp331=0;if(setjmp(_tmp32F.handler))_tmp331=1;if(!_tmp331){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp332=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp5B1;void*_tmp5B0;(_tmp5B0=0,Cyc_fprintf(Cyc_stderr,((_tmp5B1="{ ",_tag_dyneither(_tmp5B1,sizeof(char),3))),_tag_dyneither(_tmp5B0,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp332)){
void*_tmp335=_tmp332.f1;
if(!first){
{const char*_tmp5B4;void*_tmp5B3;(_tmp5B3=0,Cyc_fprintf(Cyc_stderr,((_tmp5B4=", ",_tag_dyneither(_tmp5B4,sizeof(char),3))),_tag_dyneither(_tmp5B3,sizeof(void*),0)));}
first=0;}
# 1441
pr(_tmp335);}{
# 1443
const char*_tmp5B7;void*_tmp5B6;(_tmp5B6=0,Cyc_fprintf(Cyc_stderr,((_tmp5B7="}\n",_tag_dyneither(_tmp5B7,sizeof(char),3))),_tag_dyneither(_tmp5B6,sizeof(void*),0)));};}
# 1431
;_pop_handler();}else{void*_tmp330=(void*)_exn_thrown;void*_tmp33B=_tmp330;void*_tmp33C;void*_tmp33D;_LL1D5: _tmp33C=_tmp33B;_LL1D6:
# 1446
{const char*_tmp5BA;void*_tmp5B9;(_tmp5B9=0,Cyc_fprintf(Cyc_stderr,((_tmp5BA="{ }\n",_tag_dyneither(_tmp5BA,sizeof(char),5))),_tag_dyneither(_tmp5B9,sizeof(void*),0)));}
_npop_handler(0);return;_LL1D7: _tmp33D=_tmp33B;_LL1D8:(void)_throw(_tmp33D);_LL1D4:;}};}
# 1430
;_pop_region(r);}
# 1452
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp340=rval;enum Cyc_CfFlowInfo_InitLevel _tmp345;enum Cyc_CfFlowInfo_InitLevel _tmp347;struct Cyc_CfFlowInfo_Place*_tmp349;int _tmp34B;int _tmp34C;struct _dyneither_ptr _tmp34D;struct Cyc_Absyn_Exp*_tmp34F;int _tmp350;void*_tmp351;_LL1DA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp341=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp341->tag != 0)goto _LL1DC;}_LL1DB:
{const char*_tmp5BD;void*_tmp5BC;(_tmp5BC=0,Cyc_fprintf(Cyc_stderr,((_tmp5BD="Zero",_tag_dyneither(_tmp5BD,sizeof(char),5))),_tag_dyneither(_tmp5BC,sizeof(void*),0)));}goto _LL1D9;_LL1DC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp342=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp342->tag != 1)goto _LL1DE;}_LL1DD:
{const char*_tmp5C0;void*_tmp5BF;(_tmp5BF=0,Cyc_fprintf(Cyc_stderr,((_tmp5C0="NotZeroAll",_tag_dyneither(_tmp5C0,sizeof(char),11))),_tag_dyneither(_tmp5BF,sizeof(void*),0)));}goto _LL1D9;_LL1DE: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp343=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp343->tag != 2)goto _LL1E0;}_LL1DF:
{const char*_tmp5C3;void*_tmp5C2;(_tmp5C2=0,Cyc_fprintf(Cyc_stderr,((_tmp5C3="NotZeroThis",_tag_dyneither(_tmp5C3,sizeof(char),12))),_tag_dyneither(_tmp5C2,sizeof(void*),0)));}goto _LL1D9;_LL1E0: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp344=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp344->tag != 3)goto _LL1E2;else{_tmp345=_tmp344->f1;}}_LL1E1:
{const char*_tmp5C6;void*_tmp5C5;(_tmp5C5=0,Cyc_fprintf(Cyc_stderr,((_tmp5C6="Unknown(",_tag_dyneither(_tmp5C6,sizeof(char),9))),_tag_dyneither(_tmp5C5,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp345);
{const char*_tmp5C9;void*_tmp5C8;(_tmp5C8=0,Cyc_fprintf(Cyc_stderr,((_tmp5C9=")",_tag_dyneither(_tmp5C9,sizeof(char),2))),_tag_dyneither(_tmp5C8,sizeof(void*),0)));}goto _LL1D9;_LL1E2: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp346=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp346->tag != 4)goto _LL1E4;else{_tmp347=_tmp346->f1;}}_LL1E3:
{const char*_tmp5CC;void*_tmp5CB;(_tmp5CB=0,Cyc_fprintf(Cyc_stderr,((_tmp5CC="Esc(",_tag_dyneither(_tmp5CC,sizeof(char),5))),_tag_dyneither(_tmp5CB,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp347);
{const char*_tmp5CF;void*_tmp5CE;(_tmp5CE=0,Cyc_fprintf(Cyc_stderr,((_tmp5CF=")",_tag_dyneither(_tmp5CF,sizeof(char),2))),_tag_dyneither(_tmp5CE,sizeof(void*),0)));}goto _LL1D9;_LL1E4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp348=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp348->tag != 5)goto _LL1E6;else{_tmp349=_tmp348->f1;}}_LL1E5:
{const char*_tmp5D2;void*_tmp5D1;(_tmp5D1=0,Cyc_fprintf(Cyc_stderr,((_tmp5D2="AddrOf(",_tag_dyneither(_tmp5D2,sizeof(char),8))),_tag_dyneither(_tmp5D1,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp349);
{const char*_tmp5D5;void*_tmp5D4;(_tmp5D4=0,Cyc_fprintf(Cyc_stderr,((_tmp5D5=")",_tag_dyneither(_tmp5D5,sizeof(char),2))),_tag_dyneither(_tmp5D4,sizeof(void*),0)));}goto _LL1D9;_LL1E6: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp34A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp34A->tag != 6)goto _LL1E8;else{_tmp34B=(_tmp34A->f1).is_union;_tmp34C=(_tmp34A->f1).fieldnum;_tmp34D=_tmp34A->f2;}}_LL1E7:
# 1464
 if(_tmp34B){
{const char*_tmp5D8;void*_tmp5D7;(_tmp5D7=0,Cyc_fprintf(Cyc_stderr,((_tmp5D8="AggrUnion{",_tag_dyneither(_tmp5D8,sizeof(char),11))),_tag_dyneither(_tmp5D7,sizeof(void*),0)));}
if(_tmp34C != - 1){
const char*_tmp5DC;void*_tmp5DB[1];struct Cyc_Int_pa_PrintArg_struct _tmp5DA;(_tmp5DA.tag=1,((_tmp5DA.f1=(unsigned long)_tmp34C,((_tmp5DB[0]=& _tmp5DA,Cyc_fprintf(Cyc_stderr,((_tmp5DC="tag == %d;",_tag_dyneither(_tmp5DC,sizeof(char),11))),_tag_dyneither(_tmp5DB,sizeof(void*),1)))))));}}else{
# 1469
const char*_tmp5DF;void*_tmp5DE;(_tmp5DE=0,Cyc_fprintf(Cyc_stderr,((_tmp5DF="AggrStruct{",_tag_dyneither(_tmp5DF,sizeof(char),12))),_tag_dyneither(_tmp5DE,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp34D,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp34D.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp34D,sizeof(void*))){const char*_tmp5E2;void*_tmp5E1;(_tmp5E1=0,Cyc_fprintf(Cyc_stderr,((_tmp5E2=",",_tag_dyneither(_tmp5E2,sizeof(char),2))),_tag_dyneither(_tmp5E1,sizeof(void*),0)));}}}
# 1474
{const char*_tmp5E5;void*_tmp5E4;(_tmp5E4=0,Cyc_fprintf(Cyc_stderr,((_tmp5E5="}",_tag_dyneither(_tmp5E5,sizeof(char),2))),_tag_dyneither(_tmp5E4,sizeof(void*),0)));}
goto _LL1D9;_LL1E8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp34E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp340;if(_tmp34E->tag != 7)goto _LL1D9;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34E->f2;_tmp351=(void*)_tmp34E->f3;}}_LL1E9:
# 1477
{const char*_tmp5EA;void*_tmp5E9[2];struct Cyc_String_pa_PrintArg_struct _tmp5E8;struct Cyc_Int_pa_PrintArg_struct _tmp5E7;(_tmp5E7.tag=1,((_tmp5E7.f1=(unsigned long)_tmp350,((_tmp5E8.tag=0,((_tmp5E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp34F)),((_tmp5E9[0]=& _tmp5E8,((_tmp5E9[1]=& _tmp5E7,Cyc_fprintf(Cyc_stderr,((_tmp5EA="[Consumed(%s,%d,",_tag_dyneither(_tmp5EA,sizeof(char),17))),_tag_dyneither(_tmp5E9,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp351);{const char*_tmp5ED;void*_tmp5EC;(_tmp5EC=0,Cyc_fprintf(Cyc_stderr,((_tmp5ED="]",_tag_dyneither(_tmp5ED,sizeof(char),2))),_tag_dyneither(_tmp5EC,sizeof(void*),0)));};_LL1D9:;}
# 1482
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp5F0;void*_tmp5EF;(_tmp5EF=0,Cyc_fprintf(Cyc_stderr,((_tmp5F0="    ",_tag_dyneither(_tmp5F0,sizeof(char),5))),_tag_dyneither(_tmp5EF,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp5F3;void*_tmp5F2;(_tmp5F2=0,Cyc_fprintf(Cyc_stderr,((_tmp5F3=" --> ",_tag_dyneither(_tmp5F3,sizeof(char),6))),_tag_dyneither(_tmp5F2,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp5F6;void*_tmp5F5;(_tmp5F5=0,Cyc_fprintf(Cyc_stderr,((_tmp5F6="\n",_tag_dyneither(_tmp5F6,sizeof(char),2))),_tag_dyneither(_tmp5F5,sizeof(void*),0)));};}
# 1490
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1494
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp37B=f;struct Cyc_Dict_Dict _tmp37C;_LL1EB: if((_tmp37B.BottomFL).tag != 1)goto _LL1ED;_LL1EC:
{const char*_tmp5F9;void*_tmp5F8;(_tmp5F8=0,Cyc_fprintf(Cyc_stderr,((_tmp5F9="  BottomFL\n",_tag_dyneither(_tmp5F9,sizeof(char),12))),_tag_dyneither(_tmp5F8,sizeof(void*),0)));}goto _LL1EA;_LL1ED: if((_tmp37B.ReachableFL).tag != 2)goto _LL1EA;_tmp37C=((struct _tuple12)(_tmp37B.ReachableFL).val).f1;_LL1EE:
# 1498
{const char*_tmp5FC;void*_tmp5FB;(_tmp5FB=0,Cyc_fprintf(Cyc_stderr,((_tmp5FC="  ReachableFL:\n",_tag_dyneither(_tmp5FC,sizeof(char),16))),_tag_dyneither(_tmp5FB,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp37C);
{const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,Cyc_fprintf(Cyc_stderr,((_tmp5FF="\n",_tag_dyneither(_tmp5FF,sizeof(char),2))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}
goto _LL1EA;_LL1EA:;}
# 1513 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp383=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp390;struct Cyc_List_List*_tmp392;struct Cyc_List_List*_tmp394;struct Cyc_List_List*_tmp396;struct Cyc_List_List*_tmp398;struct Cyc_List_List*_tmp39A;void*_tmp39C;void*_tmp39D;struct Cyc_List_List*_tmp3A0;struct Cyc_List_List*_tmp3A2;void*_tmp3A4;void*_tmp3A6;void*_tmp3A8;void*_tmp3AA;void*_tmp3AB;void*_tmp3AD;_LL1F0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 26)goto _LL1F2;}_LL1F1:
 goto _LL1F3;_LL1F2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp385=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp383;if(_tmp385->tag != 0)goto _LL1F4;}_LL1F3:
 goto _LL1F5;_LL1F4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp383;if(_tmp386->tag != 6)goto _LL1F6;}_LL1F5:
 goto _LL1F7;_LL1F6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp383;if(_tmp387->tag != 7)goto _LL1F8;}_LL1F7:
 goto _LL1F9;_LL1F8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp383;if(_tmp388->tag != 13)goto _LL1FA;}_LL1F9:
 goto _LL1FB;_LL1FA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp383;if(_tmp389->tag != 14)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp383;if(_tmp38A->tag != 19)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp383;if(_tmp38B->tag != 20)goto _LL200;}_LL1FF:
 goto _LL201;_LL200: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp383;if(_tmp38C->tag != 1)goto _LL202;}_LL201:
 return 0;_LL202: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp383;if(_tmp38D->tag != 21)goto _LL204;}_LL203: {
# 1525
void*_tmp3AF=rgn;_LL227: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3B0=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3AF;if(_tmp3B0->tag != 0)goto _LL229;}_LL228:
 return 1;_LL229:;_LL22A:
 return 0;_LL226:;}_LL204: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp383;if(_tmp38E->tag != 22)goto _LL206;}_LL205: {
# 1530
void*_tmp3B1=rgn;_LL22C: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3B2=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3B1;if(_tmp3B2->tag != 1)goto _LL22E;}_LL22D:
 return 1;_LL22E:;_LL22F:
 return 0;_LL22B:;}_LL206: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp383;if(_tmp38F->tag != 2)goto _LL208;else{_tmp390=_tmp38F->f1;}}_LL207: {
# 1535
void*_tmp3B3=rgn;struct Cyc_Absyn_Tvar*_tmp3B7;_LL231: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3B4=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3B3;if(_tmp3B4->tag != 1)goto _LL233;}_LL232:
 goto _LL234;_LL233: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3B5=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3B3;if(_tmp3B5->tag != 0)goto _LL235;}_LL234:
 return 0;_LL235: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp3B6=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp3B3;if(_tmp3B6->tag != 2)goto _LL230;else{_tmp3B7=_tmp3B6->f1;}}_LL236:
 return Cyc_Absyn_tvar_cmp(_tmp390,_tmp3B7)== 0;_LL230:;}_LL208: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp391=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp383;if(_tmp391->tag != 3)goto _LL20A;else{_tmp392=(_tmp391->f1).targs;}}_LL209:
# 1548 "cf_flowinfo.cyc"
 _tmp394=_tmp392;goto _LL20B;_LL20A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp383;if(_tmp393->tag != 17)goto _LL20C;else{_tmp394=_tmp393->f2;}}_LL20B:
 _tmp396=_tmp394;goto _LL20D;_LL20C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp383;if(_tmp395->tag != 11)goto _LL20E;else{_tmp396=(_tmp395->f1).targs;}}_LL20D:
 _tmp398=_tmp396;goto _LL20F;_LL20E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp383;if(_tmp397->tag != 4)goto _LL210;else{_tmp398=(_tmp397->f1).targs;}}_LL20F:
 _tmp39A=_tmp398;goto _LL211;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp383;if(_tmp399->tag != 24)goto _LL212;else{_tmp39A=_tmp399->f1;}}_LL211:
# 1553
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp39A);_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp39B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383;if(_tmp39B->tag != 5)goto _LL214;else{_tmp39C=(_tmp39B->f1).elt_typ;_tmp39D=((_tmp39B->f1).ptr_atts).rgn;}}_LL213:
# 1555
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp39D) || Cyc_CfFlowInfo_contains_region(rgn,_tmp39C);_LL214: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp39E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp383;if(_tmp39E->tag != 9)goto _LL216;}_LL215:
 return 0;_LL216: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp39F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp383;if(_tmp39F->tag != 10)goto _LL218;else{_tmp3A0=_tmp39F->f1;}}_LL217:
# 1558
 for(0;_tmp3A0 != 0;_tmp3A0=_tmp3A0->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp3A0->hd)).f2))return 1;}
return 0;_LL218: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3A1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp383;if(_tmp3A1->tag != 12)goto _LL21A;else{_tmp3A2=_tmp3A1->f2;}}_LL219:
# 1562
 for(0;_tmp3A2 != 0;_tmp3A2=_tmp3A2->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp3A2->hd)->type))return 1;}
return 0;_LL21A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp383;if(_tmp3A3->tag != 8)goto _LL21C;else{_tmp3A4=(_tmp3A3->f1).elt_type;}}_LL21B:
 _tmp3A6=_tmp3A4;goto _LL21D;_LL21C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3A5=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp383;if(_tmp3A5->tag != 23)goto _LL21E;else{_tmp3A6=(void*)_tmp3A5->f1;}}_LL21D:
 _tmp3A8=_tmp3A6;goto _LL21F;_LL21E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3A7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp383;if(_tmp3A7->tag != 25)goto _LL220;else{_tmp3A8=(void*)_tmp3A7->f1;}}_LL21F:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3A8);_LL220: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp3A9=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp383;if(_tmp3A9->tag != 16)goto _LL222;else{_tmp3AA=(void*)_tmp3A9->f1;_tmp3AB=(void*)_tmp3A9->f2;}}_LL221:
# 1569
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3AA) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3AB);_LL222: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3AC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp383;if(_tmp3AC->tag != 15)goto _LL224;else{_tmp3AD=(void*)_tmp3AC->f1;}}_LL223:
# 1572
 return 0;_LL224: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp383;if(_tmp3AE->tag != 18)goto _LL1EF;}_LL225:
 return 0;_LL1EF:;}struct _tuple25{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1578
static void Cyc_CfFlowInfo_kill_root(struct _tuple25*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B9;struct Cyc_Dict_Dict*_tmp3BA;void*_tmp3BB;struct _tuple25*_tmp3B8=env;_tmp3B9=(*_tmp3B8).f1;_tmp3BA=(struct Cyc_Dict_Dict*)&(*_tmp3B8).f2;_tmp3BB=(*_tmp3B8).f3;{
void*_tmp3BC=root;struct Cyc_Absyn_Vardecl*_tmp3BE;void*_tmp3C0;_LL238: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3BD=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3BC;if(_tmp3BD->tag != 0)goto _LL23A;else{_tmp3BE=_tmp3BD->f1;}}_LL239:
# 1583
 if(Cyc_CfFlowInfo_contains_region(_tmp3BB,_tmp3BE->type))
# 1585
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B9,_tmp3BE->type,0,_tmp3B9->unknown_none);
*_tmp3BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3BA,root,rval);
goto _LL237;_LL23A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3BF=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp3BC;if(_tmp3BF->tag != 1)goto _LL23C;else{_tmp3C0=(void*)_tmp3BF->f2;}}_LL23B:
# 1590
 if(!Cyc_CfFlowInfo_contains_region(_tmp3BB,_tmp3C0))
# 1592
*_tmp3BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3BA,root,rval);
goto _LL237;_LL23C: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3C1=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp3BC;if(_tmp3C1->tag != 2)goto _LL237;}_LL23D:
# 1596
 goto _LL237;_LL237:;};}
# 1601
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1603
void*_tmp3C2=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp3C4;_LL23F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 2)goto _LL241;else{_tmp3C4=_tmp3C3->f1;}}_LL240: {
# 1605
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp600;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp600.tag=2,((_tmp600.f1=_tmp3C4,_tmp600)));
void*v2=(void*)& v;
struct _tuple25 _tmp601;struct _tuple25 env=(_tmp601.f1=fenv,((_tmp601.f2=fenv->mt_flowdict,((_tmp601.f3=v2,_tmp601)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL241: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3C2;if(_tmp3C5->tag != 21)goto _LL243;}_LL242: {
# 1612
struct _tuple25 _tmp602;struct _tuple25 env=(_tmp602.f1=fenv,((_tmp602.f2=fenv->mt_flowdict,((_tmp602.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp602)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL243: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3C6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3C2;if(_tmp3C6->tag != 22)goto _LL245;}_LL244: {
# 1617
struct _tuple25 _tmp603;struct _tuple25 env=(_tmp603.f1=fenv,((_tmp603.f2=fenv->mt_flowdict,((_tmp603.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp603)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL245:;_LL246: {
# 1621
struct Cyc_Core_Impossible_exn_struct _tmp609;const char*_tmp608;struct Cyc_Core_Impossible_exn_struct*_tmp607;(int)_throw((void*)((_tmp607=_cycalloc(sizeof(*_tmp607)),((_tmp607[0]=((_tmp609.tag=Cyc_Core_Impossible,((_tmp609.f1=((_tmp608="kill_flowdict_region",_tag_dyneither(_tmp608,sizeof(char),21))),_tmp609)))),_tmp607)))));}_LL23E:;}
# 1626
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1628
return 0;}
# 1633
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp3CE=f;struct Cyc_Dict_Dict _tmp3CF;struct Cyc_List_List*_tmp3D0;_LL248: if((_tmp3CE.BottomFL).tag != 1)goto _LL24A;_LL249:
 return f;_LL24A: if((_tmp3CE.ReachableFL).tag != 2)goto _LL247;_tmp3CF=((struct _tuple12)(_tmp3CE.ReachableFL).val).f1;_tmp3D0=((struct _tuple12)(_tmp3CE.ReachableFL).val).f2;_LL24B: {
# 1637
struct Cyc_Dict_Dict _tmp3D1=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp3CF,rgn);
struct Cyc_List_List*_tmp3D2=Cyc_CfFlowInfo_kill_relns_region(_tmp3D0,rgn);
# 1640
return Cyc_CfFlowInfo_ReachableFL(_tmp3D1,_tmp3D2);}_LL247:;}
