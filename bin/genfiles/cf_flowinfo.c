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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 909
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1162
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
# 266 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 312
int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 75
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NTvar NTvar;};
# 58 "pratt.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
typedef union Cyc_Pratt_Node Cyc_Pratt_node_t;
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NTvar(struct Cyc_Absyn_Tvar*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);struct Cyc_Pratt_Graph;
# 66
typedef struct Cyc_Pratt_Graph Cyc_Pratt_graph_t;
# 70
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
# 75
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,int a);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 80
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RTvar(struct Cyc_Absyn_Tvar*);
# 88
enum Cyc_CfFlowInfo_Relation{Cyc_CfFlowInfo_Req  = 0,Cyc_CfFlowInfo_Rneq  = 1,Cyc_CfFlowInfo_Rlte  = 2,Cyc_CfFlowInfo_Rlt  = 3};
typedef enum Cyc_CfFlowInfo_Relation Cyc_CfFlowInfo_relation_t;struct Cyc_CfFlowInfo_Reln{union Cyc_CfFlowInfo_RelnOp rop1;enum Cyc_CfFlowInfo_Relation relation;union Cyc_CfFlowInfo_RelnOp rop2;};
# 95
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 104
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 110
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 115
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 122
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 144
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 146
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 149
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 165 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 184
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 187
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 190
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 193
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 199
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 204
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 228 "cf_flowinfo.h"
struct Cyc_Absyn_Vardecl*Cyc_CfFlowInfo_nonesc_vardecl(void*b);
# 231
int Cyc_CfFlowInfo_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_RelnOp*p);
# 233
struct Cyc_List_List*Cyc_CfFlowInfo_add_relation(struct _RegionHandle*rgn,union Cyc_CfFlowInfo_RelnOp rop1,enum Cyc_CfFlowInfo_Relation r,union Cyc_CfFlowInfo_RelnOp rop2,struct Cyc_List_List*relns);
# 236
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 238
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 240
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 242
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
# 251
int Cyc_CfFlowInfo_consistent_relations(struct Cyc_List_List*rlns);
# 255
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 259
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 264
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2);
# 270
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 275
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 282
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 286
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 288
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 46 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp410;return((_tmp410.PlaceL).val=x,(((_tmp410.PlaceL).tag=1,_tmp410)));}
# 53
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp411;return((_tmp411.UnknownL).val=0,(((_tmp411.UnknownL).tag=2,_tmp411)));}
# 56
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp412;return((_tmp412.BottomFL).val=0,(((_tmp412.BottomFL).tag=1,_tmp412)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple11 _tmp415;union Cyc_CfFlowInfo_FlowInfo _tmp414;return((_tmp414.ReachableFL).val=((_tmp415.f1=fd,((_tmp415.f2=r,_tmp415)))),(((_tmp414.ReachableFL).tag=2,_tmp414)));}
# 62
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp44C;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp44B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp44A;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp449;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp448;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp447;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp446;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp445;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp444;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp443;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp442;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp441;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp440;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43F;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp43E;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp43D;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp43C;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp43B;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp43A;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp439;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp438;struct Cyc_CfFlowInfo_FlowEnv*_tmp437;return(_tmp437=_region_malloc(r,sizeof(*_tmp437)),((_tmp437->r=r,((_tmp437->zero=(void*)(
# 69
(_tmp439=_region_malloc(r,sizeof(*_tmp439)),((_tmp439[0]=((_tmp438.tag=0,_tmp438)),_tmp439)))),((_tmp437->notzeroall=(void*)(
(_tmp43B=_region_malloc(r,sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43A.tag=1,_tmp43A)),_tmp43B)))),((_tmp437->notzerothis=(void*)(
(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp43C.tag=2,_tmp43C)),_tmp43D)))),((_tmp437->unknown_none=(void*)(
(_tmp43F=_region_malloc(r,sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp43E.tag=3,((_tmp43E.f1=Cyc_CfFlowInfo_NoneIL,_tmp43E)))),_tmp43F)))),((_tmp437->unknown_this=(void*)(
(_tmp441=_region_malloc(r,sizeof(*_tmp441)),((_tmp441[0]=((_tmp440.tag=3,((_tmp440.f1=Cyc_CfFlowInfo_ThisIL,_tmp440)))),_tmp441)))),((_tmp437->unknown_all=(void*)(
(_tmp443=_region_malloc(r,sizeof(*_tmp443)),((_tmp443[0]=((_tmp442.tag=3,((_tmp442.f1=Cyc_CfFlowInfo_AllIL,_tmp442)))),_tmp443)))),((_tmp437->esc_none=(void*)(
(_tmp445=_region_malloc(r,sizeof(*_tmp445)),((_tmp445[0]=((_tmp444.tag=4,((_tmp444.f1=Cyc_CfFlowInfo_NoneIL,_tmp444)))),_tmp445)))),((_tmp437->esc_this=(void*)(
(_tmp447=_region_malloc(r,sizeof(*_tmp447)),((_tmp447[0]=((_tmp446.tag=4,((_tmp446.f1=Cyc_CfFlowInfo_ThisIL,_tmp446)))),_tmp447)))),((_tmp437->esc_all=(void*)(
(_tmp449=_region_malloc(r,sizeof(*_tmp449)),((_tmp449[0]=((_tmp448.tag=4,((_tmp448.f1=Cyc_CfFlowInfo_AllIL,_tmp448)))),_tmp449)))),((_tmp437->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp437->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp437->dummy_place=(
(_tmp44C=_region_malloc(r,sizeof(*_tmp44C)),((_tmp44C->root=(void*)((_tmp44A=_region_malloc(r,sizeof(*_tmp44A)),((_tmp44A[0]=((_tmp44B.tag=1,((_tmp44B.f1=& dummy_exp,((_tmp44B.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp44B)))))),_tmp44A)))),((_tmp44C->fields=0,_tmp44C)))))),_tmp437)))))))))))))))))))))))))));}
# 86
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 89
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 92
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 94
return 0;}
# 98
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{
void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2:
# 102
 if(_tmp21 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{
# 105
const char*_tmp450;void*_tmp44F[1];struct Cyc_String_pa_PrintArg_struct _tmp44E;return(struct _dyneither_ptr)((_tmp44E.tag=0,((_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp44F[0]=& _tmp44E,Cyc_aprintf(((_tmp450="reachable from %s",_tag_dyneither(_tmp450,sizeof(char),18))),_tag_dyneither(_tmp44F,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp453;void*_tmp452;(_tmp452=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp453="error locations not for VarRoots",_tag_dyneither(_tmp453,sizeof(char),33))),_tag_dyneither(_tmp452,sizeof(void*),0)));}_LL0:;};}
# 111
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 115
return loc1;}
# 120
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 127
int _tmp2A=0;
struct Cyc_Dict_Dict _tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
# 132
return _tmp2B;}struct _tuple13{void*f1;unsigned int f2;};
# 135
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple13 _tmp30=*((struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(iter,& _tmp30)){
void*_tmp31=_tmp30.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){
# 147
int _tmp32=0;_npop_handler(1);return _tmp32;}}{
# 150
int _tmp33=1;_npop_handler(1);return _tmp33;};}
# 139
;_pop_region(temp);}
# 138
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp35=_tmp2D;void*_tmp36;void*_tmp37;_LL6: _tmp36=_tmp35;_LL7:
# 154
 return 1;_LL8: _tmp37=_tmp35;_LL9:(void)_throw(_tmp37);_LL5:;}};};}
# 158
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 163
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp454;return((_tmp454.RConst).val=i,(((_tmp454.RConst).tag=1,_tmp454)));}
# 166
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RVar(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp455;return((_tmp455.RVar).val=v,(((_tmp455.RVar).tag=2,_tmp455)));}
# 169
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp456;return((_tmp456.RNumelts).val=v,(((_tmp456.RNumelts).tag=3,_tmp456)));}
# 172
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RTvar(struct Cyc_Absyn_Tvar*tv){
union Cyc_CfFlowInfo_RelnOp _tmp457;return((_tmp457.RTvar).val=tv,(((_tmp457.RTvar).tag=4,_tmp457)));}
# 177
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp3C=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp3C,f)== 0)return n;
n=n + 1;}{
# 184
struct Cyc_Core_Impossible_exn_struct _tmp464;const char*_tmp463;void*_tmp462[1];struct Cyc_String_pa_PrintArg_struct _tmp461;struct Cyc_Core_Impossible_exn_struct*_tmp460;(int)_throw((void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp464.tag=Cyc_Core_Impossible,((_tmp464.f1=(struct _dyneither_ptr)((_tmp461.tag=0,((_tmp461.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp462[0]=& _tmp461,Cyc_aprintf(((_tmp463="get_field_index_fs failed: %s",_tag_dyneither(_tmp463,sizeof(char),30))),_tag_dyneither(_tmp462,sizeof(void*),1)))))))),_tmp464)))),_tmp460)))));};}
# 187
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp42=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp44;struct Cyc_List_List*_tmp46;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp42;if(_tmp43->tag != 11)goto _LLD;else{_tmp44=(_tmp43->f1).aggr_info;}}_LLC: {
# 190
struct Cyc_List_List*_tmp47=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp44))->impl))->fields;
_tmp46=_tmp47;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp42;if(_tmp45->tag != 12)goto _LLF;else{_tmp46=_tmp45->f2;}}_LLE:
# 193
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp46,f);_LLF:;_LL10: {
# 195
struct Cyc_Core_Impossible_exn_struct _tmp471;const char*_tmp470;void*_tmp46F[1];struct Cyc_String_pa_PrintArg_struct _tmp46E;struct Cyc_Core_Impossible_exn_struct*_tmp46D;(int)_throw((void*)((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp471.tag=Cyc_Core_Impossible,((_tmp471.f1=(struct _dyneither_ptr)((_tmp46E.tag=0,((_tmp46E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp46F[0]=& _tmp46E,Cyc_aprintf(((_tmp470="get_field_index failed: %s",_tag_dyneither(_tmp470,sizeof(char),27))),_tag_dyneither(_tmp46F,sizeof(void*),1)))))))),_tmp471)))),_tmp46D)))));}_LLA:;}
# 200
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp472;struct _tuple0 _tmp4E=(_tmp472.f1=r1,((_tmp472.f2=r2,_tmp472)));struct Cyc_Absyn_Vardecl*_tmp50;struct Cyc_Absyn_Vardecl*_tmp52;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp58;int _tmp5C;int _tmp5E;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp4F->tag != 0)goto _LL14;else{_tmp50=_tmp4F->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp51=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp51->tag != 0)goto _LL14;else{_tmp52=_tmp51->f1;}};_LL13:
 return(int)_tmp50 - (int)_tmp52;_LL14: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp53=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp53->tag != 0)goto _LL16;}_LL15:
 return - 1;_LL16: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp54=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp54->tag != 0)goto _LL18;}_LL17:
 return 1;_LL18:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp55->tag != 1)goto _LL1A;else{_tmp56=_tmp55->f1;}}{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp57=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp57->tag != 1)goto _LL1A;else{_tmp58=_tmp57->f1;}};_LL19:
 return(int)_tmp56 - (int)_tmp58;_LL1A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp59=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp59->tag != 1)goto _LL1C;}_LL1B:
 return - 1;_LL1C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp5A=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp5A->tag != 1)goto _LL1E;}_LL1D:
 return 1;_LL1E:{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp5B=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp5B->tag != 2)goto _LL11;else{_tmp5C=_tmp5B->f1;}}{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp5D=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp5D->tag != 2)goto _LL11;else{_tmp5E=_tmp5D->f1;}};_LL1F:
 return _tmp5C - _tmp5E;_LL11:;};}
# 214
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 216
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 219
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
void*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_CfFlowInfo_Place _tmp5F=*p1;_tmp60=_tmp5F.root;_tmp61=_tmp5F.fields;{
void*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_CfFlowInfo_Place _tmp62=*p2;_tmp63=_tmp62.root;_tmp64=_tmp62.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp60,_tmp63);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp61,_tmp64);};};};}
# 231
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
void*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_CfFlowInfo_Place _tmp65=*p;_tmp66=_tmp65.root;_tmp67=_tmp65.fields;
{void*_tmp68=_tmp66;struct Cyc_Absyn_Vardecl*_tmp6A;struct Cyc_Absyn_Exp*_tmp6C;int _tmp6E;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp69=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL23;else{_tmp6A=_tmp69->f1;}}_LL22:
{struct _dyneither_ptr*_tmp47F;const char*_tmp47E;void*_tmp47D[1];struct Cyc_String_pa_PrintArg_struct _tmp47C;struct Cyc_List_List*_tmp47B;sl=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->hd=((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp47C.tag=0,((_tmp47C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6A->name).f2),((_tmp47D[0]=& _tmp47C,Cyc_aprintf(((_tmp47E="%s",_tag_dyneither(_tmp47E,sizeof(char),3))),_tag_dyneither(_tmp47D,sizeof(void*),1)))))))),_tmp47F)))),((_tmp47B->tl=sl,_tmp47B))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6B=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp68;if(_tmp6B->tag != 1)goto _LL25;else{_tmp6C=_tmp6B->f1;}}_LL24:
{struct _dyneither_ptr*_tmp48C;const char*_tmp48B;void*_tmp48A[1];struct Cyc_Int_pa_PrintArg_struct _tmp489;struct Cyc_List_List*_tmp488;sl=((_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp489.tag=1,((_tmp489.f1=(unsigned long)((int)_tmp6C),((_tmp48A[0]=& _tmp489,Cyc_aprintf(((_tmp48B="mpt%d",_tag_dyneither(_tmp48B,sizeof(char),6))),_tag_dyneither(_tmp48A,sizeof(void*),1)))))))),_tmp48C)))),((_tmp488->tl=sl,_tmp488))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6D=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp68;if(_tmp6D->tag != 2)goto _LL20;else{_tmp6E=_tmp6D->f1;}}_LL26:
{struct _dyneither_ptr*_tmp499;const char*_tmp498;void*_tmp497[1];struct Cyc_Int_pa_PrintArg_struct _tmp496;struct Cyc_List_List*_tmp495;sl=((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->hd=((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499[0]=((_tmp496.tag=1,((_tmp496.f1=(unsigned long)_tmp6E,((_tmp497[0]=& _tmp496,Cyc_aprintf(((_tmp498="param%d",_tag_dyneither(_tmp498,sizeof(char),8))),_tag_dyneither(_tmp497,sizeof(void*),1)))))))),_tmp499)))),((_tmp495->tl=sl,_tmp495))))));}goto _LL20;_LL20:;}
# 239
for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
struct _dyneither_ptr*_tmp4A6;const char*_tmp4A5;void*_tmp4A4[1];struct Cyc_Int_pa_PrintArg_struct _tmp4A3;struct Cyc_List_List*_tmp4A2;sl=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->hd=((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A3.tag=1,((_tmp4A3.f1=(unsigned long)((int)_tmp67->hd),((_tmp4A4[0]=& _tmp4A3,Cyc_aprintf(((_tmp4A5="%d",_tag_dyneither(_tmp4A5,sizeof(char),3))),_tag_dyneither(_tmp4A4,sizeof(void*),1)))))))),_tmp4A6)))),((_tmp4A2->tl=sl,_tmp4A2))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp4B1;const char*_tmp4B0;void*_tmp4AF[1];const char*_tmp4AE;struct _dyneither_ptr*_tmp4AD;struct _dyneither_ptr*_tmp83=(_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4B1.tag=0,((_tmp4B1.f1=(struct _dyneither_ptr)((_tmp4B0="",_tag_dyneither(_tmp4B0,sizeof(char),1))),((_tmp4AF[0]=& _tmp4B1,Cyc_aprintf(((_tmp4AE="%s",_tag_dyneither(_tmp4AE,sizeof(char),3))),_tag_dyneither(_tmp4AF,sizeof(void*),1)))))))),_tmp4AD)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp4B6;void*_tmp4B5[2];struct Cyc_String_pa_PrintArg_struct _tmp4B4;struct Cyc_String_pa_PrintArg_struct _tmp4B3;*_tmp83=((_tmp4B3.tag=0,((_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp83),((_tmp4B4.tag=0,((_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp4B5[0]=& _tmp4B4,((_tmp4B5[1]=& _tmp4B3,Cyc_aprintf(((_tmp4B6="%s.%s",_tag_dyneither(_tmp4B6,sizeof(char),6))),_tag_dyneither(_tmp4B5,sizeof(void*),2))))))))))))));}
return _tmp83;};}
# 257 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp4BC(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4BB,unsigned int*_tmp4BA,void***_tmp4B8){for(*_tmp4BB=0;*_tmp4BB < *_tmp4BA;(*_tmp4BB)++){(*_tmp4B8)[*_tmp4BB]=(void*)(*fenv)->unknown_all;}}
# 262
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 268
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp4BB;unsigned int _tmp4BA;struct _dyneither_ptr _tmp4B9;void**_tmp4B8;unsigned int _tmp4B7;struct _dyneither_ptr d=(_tmp4B7=sz,((_tmp4B8=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4B7)),((_tmp4B9=_tag_dyneither(_tmp4B8,sizeof(void*),_tmp4B7),((((_tmp4BA=_tmp4B7,_tmp4BC(& fenv,& _tmp4BB,& _tmp4BA,& _tmp4B8))),_tmp4B9)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct _dyneither_ptr*_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrfield*_tmp8D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp8E=_tmp8D->name;_tmp8F=_tmp8D->type;
if(_get_dyneither_size(*_tmp8E,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp8F,leafval);}}
# 275
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 279
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 283
struct _RegionHandle*_tmp95;struct Cyc_List_List*_tmp96;struct _tuple14*_tmp94=env;_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;{
void*_tmp97=Cyc_Tcutil_rsubstitute(_tmp95,_tmp96,f->type);
struct Cyc_Absyn_Aggrfield*_tmp4BD;return(_tmp4BD=_region_malloc(_tmp95,sizeof(*_tmp4BD)),((_tmp4BD->name=f->name,((_tmp4BD->tq=f->tq,((_tmp4BD->type=_tmp97,((_tmp4BD->width=f->width,((_tmp4BD->attributes=f->attributes,_tmp4BD)))))))))));};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 292
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 298
struct _RegionHandle _tmp99=_new_region("temp");struct _RegionHandle*temp=& _tmp99;_push_region(temp);
# 302
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp9A=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp9B=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp9C=Cyc_Tcutil_tvar_kind(_tmp9A,& Cyc_Tcutil_bk);_LL28: if(_tmp9C->kind != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
 goto _LL2B;_LL2A: if(_tmp9C->kind != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
 continue;_LL2C:;_LL2D:
 goto _LL27;_LL27:;}{
# 311
struct _tuple15*_tmp4C0;struct Cyc_List_List*_tmp4BF;inst=((_tmp4BF=_region_malloc(temp,sizeof(*_tmp4BF)),((_tmp4BF->hd=((_tmp4C0=_region_malloc(temp,sizeof(*_tmp4C0)),((_tmp4C0->f1=_tmp9A,((_tmp4C0->f2=_tmp9B,_tmp4C0)))))),((_tmp4BF->tl=inst,_tmp4BF))))));};}
# 313
if(inst != 0){
struct _tuple14 _tmp4C1;struct _tuple14 _tmp9F=(_tmp4C1.f1=temp,((_tmp4C1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp4C1)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp9F,fs);
struct _dyneither_ptr _tmpA0=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA0;}else{
# 318
struct _dyneither_ptr _tmpA2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA2;}}
# 302
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp4C7(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4C6,unsigned int*_tmp4C5,void***_tmp4C3){for(*_tmp4C6=0;*_tmp4C6 < *_tmp4C5;(*_tmp4C6)++){(*_tmp4C3)[*_tmp4C6]=(void*)(*fenv)->unknown_all;}}
# 322
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 326
int is_tagged=0;
# 328
{void*_tmpA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Datatypefield*_tmpA5;struct Cyc_List_List*_tmpA7;union Cyc_Absyn_AggrInfoU _tmpA9;struct Cyc_List_List*_tmpAA;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*_tmpAD;void*_tmpAF;union Cyc_Absyn_Constraint*_tmpB0;void*_tmpB2;union Cyc_Absyn_Constraint*_tmpB4;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 4)goto _LL31;else{if((((_tmpA4->f1).field_info).KnownDatatypefield).tag != 2)goto _LL31;_tmpA5=((struct _tuple2)(((_tmpA4->f1).field_info).KnownDatatypefield).val).f2;}}_LL30:
# 330
 if(_tmpA5->typs == 0)
return leafval;
_tmpA7=_tmpA5->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA3;if(_tmpA6->tag != 10)goto _LL33;else{_tmpA7=_tmpA6->f1;}}_LL32: {
# 334
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA7);
# 337
unsigned int _tmp4C6;unsigned int _tmp4C5;struct _dyneither_ptr _tmp4C4;void**_tmp4C3;unsigned int _tmp4C2;struct _dyneither_ptr d=(_tmp4C2=sz,((_tmp4C3=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4C2)),((_tmp4C4=_tag_dyneither(_tmp4C3,sizeof(void*),_tmp4C2),((((_tmp4C5=_tmp4C2,_tmp4C7(& fenv,& _tmp4C6,& _tmp4C5,& _tmp4C3))),_tmp4C4)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpA7))->hd)).f2,leafval);
_tmpA7=_tmpA7->tl;}}{
# 342
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4CD;struct Cyc_CfFlowInfo_UnionRInfo _tmp4CC;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4CB;return(void*)((_tmp4CB=_region_malloc(fenv->r,sizeof(*_tmp4CB)),((_tmp4CB[0]=((_tmp4CD.tag=6,((_tmp4CD.f1=((_tmp4CC.is_union=0,((_tmp4CC.fieldnum=- 1,_tmp4CC)))),((_tmp4CD.f2=d,_tmp4CD)))))),_tmp4CB))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA3;if(_tmpA8->tag != 11)goto _LL35;else{_tmpA9=(_tmpA8->f1).aggr_info;_tmpAA=(_tmpA8->f1).targs;}}_LL34: {
# 344
struct Cyc_Absyn_Aggrdecl*_tmpBC=Cyc_Absyn_get_known_aggrdecl(_tmpA9);
if(_tmpBC->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBC->impl))->tagged)return fenv->unknown_all;
is_tagged=1;
if(_tmpBC->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpBD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBC->impl))->fields;
if(_tmpAA == 0){_tmpAC=_tmpBC->kind;_tmpAD=_tmpBD;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4D3;struct Cyc_CfFlowInfo_UnionRInfo _tmp4D2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D1;return(void*)((_tmp4D1=_region_malloc(fenv->r,sizeof(*_tmp4D1)),((_tmp4D1[0]=(
(_tmp4D3.tag=6,((_tmp4D3.f1=((_tmp4D2.is_union=_tmpBC->kind == Cyc_Absyn_UnionA,((_tmp4D2.fieldnum=- 1,_tmp4D2)))),((_tmp4D3.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpBC->tvs,_tmpAA,_tmpBD,_tmpBC->kind == Cyc_Absyn_UnionA,leafval),_tmp4D3)))))),_tmp4D1))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL37;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL36:
# 357
 if(_tmpAC == Cyc_Absyn_UnionA  && !is_tagged)return fenv->unknown_all;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4D9;struct Cyc_CfFlowInfo_UnionRInfo _tmp4D8;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D7;return(void*)((_tmp4D7=_region_malloc(fenv->r,sizeof(*_tmp4D7)),((_tmp4D7[0]=((_tmp4D9.tag=6,((_tmp4D9.f1=((_tmp4D8.is_union=_tmpAC == Cyc_Absyn_UnionA,((_tmp4D8.fieldnum=- 1,_tmp4D8)))),((_tmp4D9.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpAD,_tmpAC == Cyc_Absyn_UnionA,leafval),_tmp4D9)))))),_tmp4D7))));};_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3;if(_tmpAE->tag != 8)goto _LL39;else{_tmpAF=(_tmpAE->f1).elt_type;_tmpB0=(_tmpAE->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0))goto _LL39;_LL38:
# 362
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpAF)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA3;if(_tmpB1->tag != 19)goto _LL3B;else{_tmpB2=(void*)_tmpB1->f1;}}_LL3A:
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3;if(_tmpB3->tag != 5)goto _LL3D;else{_tmpB4=((_tmpB3->f1).ptr_atts).nullable;}}if(!(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB4)))goto _LL3D;_LL3C:
{void*_tmpC4=leafval;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC4;if(_tmpC5->tag != 3)goto _LL42;else{if(_tmpC5->f1 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC4;if(_tmpC6->tag != 3)goto _LL44;else{if(_tmpC6->f1 != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
 return fenv->notzeroall;_LL44:;_LL45:
 goto _LL3F;_LL3F:;}
# 370
goto _LL2E;_LL3D:;_LL3E:
 goto _LL2E;_LL2E:;}
# 374
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 377
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 383
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpC7=r;struct Cyc_Absyn_Exp*_tmpC9;int _tmpCA;void*_tmpCB;int _tmpCD;int _tmpCE;struct _dyneither_ptr _tmpCF;void*_tmpD1;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpC8->tag != 7)goto _LL49;else{_tmpC9=_tmpC8->f1;_tmpCA=_tmpC8->f2;_tmpCB=(void*)_tmpC8->f3;}}_LL48:
# 386
 if(_tmpC9 == e  && _tmpCA == env_iteration){
*needs_unconsume=1;
return 0;}
# 390
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCC->tag != 6)goto _LL4B;else{_tmpCD=(_tmpCC->f1).is_union;_tmpCE=(_tmpCC->f1).fieldnum;_tmpCF=_tmpCC->f2;}}_LL4A:
# 392
 if(!_tmpCD  || _tmpCE == - 1){
unsigned int _tmpD2=_get_dyneither_size(_tmpCF,sizeof(void*));
{int i=0;for(0;i < _tmpD2;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCF,sizeof(void*),i)),needs_unconsume))
return 1;}}
# 398
return 0;}else{
# 401
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCF,sizeof(void*),_tmpCE)),needs_unconsume);}_LL4B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpD0->tag != 8)goto _LL4D;else{_tmpD1=(void*)_tmpD0->f2;}}_LL4C:
# 403
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpD1,needs_unconsume);_LL4D:;_LL4E:
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp4DF(struct _dyneither_ptr*_tmpDA,unsigned int*_tmp4DE,unsigned int*_tmp4DD,void***_tmp4DB){for(*_tmp4DE=0;*_tmp4DE < *_tmp4DD;(*_tmp4DE)++){(*_tmp4DB)[*_tmp4DE]=(void*)*((void**)_check_dyneither_subscript(*_tmpDA,sizeof(void*),(int)*_tmp4DE));}}
# 410
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpD3=r;struct Cyc_Absyn_Exp*_tmpD5;int _tmpD6;void*_tmpD7;struct Cyc_CfFlowInfo_UnionRInfo _tmpD9;struct _dyneither_ptr _tmpDA;struct Cyc_Absyn_Vardecl*_tmpDC;void*_tmpDD;_LL50: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpD4->tag != 7)goto _LL52;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;_tmpD7=(void*)_tmpD4->f3;}}_LL51:
# 413
 return _tmpD7;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpD8->tag != 6)goto _LL54;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;}}_LL53: {
# 415
unsigned int _tmpDE=_get_dyneither_size(_tmpDA,sizeof(void*));
int change=0;
unsigned int _tmp4DE;unsigned int _tmp4DD;struct _dyneither_ptr _tmp4DC;void**_tmp4DB;unsigned int _tmp4DA;struct _dyneither_ptr d2=(_tmp4DA=_tmpDE,((_tmp4DB=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4DA)),((_tmp4DC=_tag_dyneither(_tmp4DB,sizeof(void*),_tmp4DA),((((_tmp4DD=_tmp4DA,_tmp4DF(& _tmpDA,& _tmp4DE,& _tmp4DD,& _tmp4DB))),_tmp4DC)))))));
{int i=0;for(0;i < _tmpDE;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmpDA,sizeof(void*),i)));
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmpDA,sizeof(void*),i)))
change=1;}}
# 423
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4E2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4E1;return(void*)((_tmp4E1=_region_malloc(fenv->r,sizeof(*_tmp4E1)),((_tmp4E1[0]=((_tmp4E2.tag=6,((_tmp4E2.f1=_tmpD9,((_tmp4E2.f2=d2,_tmp4E2)))))),_tmp4E1))));}else{
return r;}}_LL54: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpDB->tag != 8)goto _LL56;else{_tmpDC=_tmpDB->f1;_tmpDD=(void*)_tmpDB->f2;}}_LL55: {
# 427
void*_tmpE5=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpDD);
if(_tmpE5 != _tmpDD){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4E5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E4;return(void*)((_tmp4E4=_region_malloc(fenv->r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E5.tag=8,((_tmp4E5.f1=_tmpDC,((_tmp4E5.f2=_tmpE5,_tmp4E5)))))),_tmp4E4))));}else{
return r;}}_LL56:;_LL57:
 return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp4EF(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4EE,unsigned int*_tmp4ED,void***_tmp4EB){for(*_tmp4EE=0;*_tmp4EE < *_tmp4ED;(*_tmp4EE)++){(*_tmp4EB)[*_tmp4EE]=(void*)(*fenv)->unknown_all;}}static void _tmp4F8(struct _dyneither_ptr*_tmpFC,unsigned int*_tmp4F7,unsigned int*_tmp4F6,void***_tmp4F4){for(*_tmp4F7=0;*_tmp4F7 < *_tmp4F6;(*_tmp4F7)++){(*_tmp4F4)[*_tmp4F7]=(void*)((void**)(*_tmpFC).curr)[(int)*_tmp4F7];}}
# 437
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp4E6;struct _tuple0 _tmpE9=(_tmp4E6.f1=Cyc_Tcutil_compress(t),((_tmp4E6.f2=r,_tmp4E6)));struct Cyc_Absyn_Vardecl*_tmpEB;void*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_CfFlowInfo_UnionRInfo _tmpF0;struct _dyneither_ptr _tmpF1;union Cyc_Absyn_AggrInfoU _tmpF3;struct Cyc_CfFlowInfo_UnionRInfo _tmpF5;struct _dyneither_ptr _tmpF6;enum Cyc_Absyn_AggrKind _tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_CfFlowInfo_UnionRInfo _tmpFB;struct _dyneither_ptr _tmpFC;_LL59: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpEA->tag != 8)goto _LL5B;else{_tmpEB=_tmpEA->f1;_tmpEC=(void*)_tmpEA->f2;}}_LL5A: {
# 440
void*_tmpFD=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpEC);
if(_tmpFD != _tmpEC){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4E9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E8;return(void*)((_tmp4E8=_region_malloc(fenv->r,sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4E9.tag=8,((_tmp4E9.f1=_tmpEB,((_tmp4E9.f2=_tmpFD,_tmp4E9)))))),_tmp4E8))));}else{
return r;}}_LL5B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpED=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpED->tag != 10)goto _LL5D;else{_tmpEE=_tmpED->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpEF->tag != 6)goto _LL5D;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}};_LL5C: {
# 444
unsigned int _tmp100=_get_dyneither_size(_tmpF1,sizeof(void*));
unsigned int _tmp4EE;unsigned int _tmp4ED;struct _dyneither_ptr _tmp4EC;void**_tmp4EB;unsigned int _tmp4EA;struct _dyneither_ptr d2=(_tmp4EA=_tmp100,((_tmp4EB=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4EA)),((_tmp4EC=_tag_dyneither(_tmp4EB,sizeof(void*),_tmp4EA),((((_tmp4ED=_tmp4EA,_tmp4EF(& fenv,& _tmp4EE,& _tmp4ED,& _tmp4EB))),_tmp4EC)))))));
{int i=0;for(0;i < _tmp100;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpEE))->hd)).f2,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF1,sizeof(void*),i)));
_tmpEE=_tmpEE->tl;}}{
# 450
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F1;return(void*)((_tmp4F1=_region_malloc(fenv->r,sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F2.tag=6,((_tmp4F2.f1=_tmpF0,((_tmp4F2.f2=d2,_tmp4F2)))))),_tmp4F1))));};}_LL5D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpF2->tag != 11)goto _LL5F;else{_tmpF3=(_tmpF2->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpF4->tag != 6)goto _LL5F;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}};_LL5E: {
# 452
struct Cyc_Absyn_Aggrdecl*_tmp107=Cyc_Absyn_get_known_aggrdecl(_tmpF3);
if(_tmp107->impl == 0)return r;
_tmpF8=_tmp107->kind;_tmpF9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp107->impl))->fields;_tmpFB=_tmpF5;_tmpFC=_tmpF6;goto _LL60;}_LL5F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpF7->tag != 12)goto _LL61;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpFA->tag != 6)goto _LL61;else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;}};_LL60: {
# 456
unsigned int _tmp4F7;unsigned int _tmp4F6;struct _dyneither_ptr _tmp4F5;void**_tmp4F4;unsigned int _tmp4F3;struct _dyneither_ptr d2=(_tmp4F3=_get_dyneither_size(_tmpFC,sizeof(void*)),((_tmp4F4=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F3)),((_tmp4F5=_tag_dyneither(_tmp4F4,sizeof(void*),_tmp4F3),((((_tmp4F6=_tmp4F3,_tmp4F8(& _tmpFC,& _tmp4F7,& _tmp4F6,& _tmp4F4))),_tmp4F5)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF9);
{int i=0;for(0;i < sz;(i ++,_tmpF9=_tmpF9->tl)){
struct _dyneither_ptr*_tmp109;void*_tmp10A;struct Cyc_Absyn_Aggrfield*_tmp108=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpF9))->hd;_tmp109=_tmp108->name;_tmp10A=_tmp108->type;
if(_get_dyneither_size(*_tmp109,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp10A,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpFC,sizeof(void*),i)));}}{
# 463
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4FB;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4FA;return(void*)((_tmp4FA=_region_malloc(fenv->r,sizeof(*_tmp4FA)),((_tmp4FA[0]=((_tmp4FB.tag=6,((_tmp4FB.f1=_tmpFB,((_tmp4FB.f2=d2,_tmp4FB)))))),_tmp4FA))));};}_LL61:;_LL62:
# 465
 if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4FE;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4FD;return(void*)((_tmp4FD=_region_malloc(fenv->r,sizeof(*_tmp4FD)),((_tmp4FD[0]=((_tmp4FE.tag=7,((_tmp4FE.f1=consumer,((_tmp4FE.f2=iteration,((_tmp4FE.f3=r,_tmp4FE)))))))),_tmp4FD))));}else{
return r;}_LL58:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 471
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 474
struct _RegionHandle _tmp113=_new_region("r");struct _RegionHandle*r=& _tmp113;_push_region(r);
{struct _tuple17 _tmp4FF;struct _tuple17 elem=(_tmp4FF.f1=place,((_tmp4FF.f2=0,_tmp4FF)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp114=elem.f1;
void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_CfFlowInfo_Place _tmp115=*place;_tmp116=_tmp115.root;_tmp117=_tmp115.fields;{
void*_tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_CfFlowInfo_Place _tmp118=*_tmp114;_tmp119=_tmp118.root;_tmp11A=_tmp118.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmp116,_tmp119)!= 0)
continue;
for(0;_tmp117 != 0  && _tmp11A != 0;(_tmp117=_tmp117->tl,_tmp11A=_tmp11A->tl)){
if((int)_tmp117->hd != (int)_tmp11A->hd)break;}
if(_tmp117 == 0){
int _tmp11B=1;_npop_handler(0);return _tmp11B;}};}{
# 488
int _tmp11C=0;_npop_handler(0);return _tmp11C;};}
# 475
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 501
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 503
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 506
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp500;pile->places=((_tmp500=_region_malloc(pile->rgn,sizeof(*_tmp500)),((_tmp500->hd=place,((_tmp500->tl=pile->places,_tmp500))))));}}
# 509
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp11F=r;void*_tmp121;void*_tmp123;struct Cyc_CfFlowInfo_Place*_tmp125;struct Cyc_CfFlowInfo_UnionRInfo _tmp127;struct _dyneither_ptr _tmp128;_LL64: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp120=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp11F;if(_tmp120->tag != 7)goto _LL66;else{_tmp121=(void*)_tmp120->f3;}}_LL65:
 Cyc_CfFlowInfo_add_places(pile,_tmp121);return;_LL66: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp122=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11F;if(_tmp122->tag != 8)goto _LL68;else{_tmp123=(void*)_tmp122->f2;}}_LL67:
 Cyc_CfFlowInfo_add_places(pile,_tmp123);return;_LL68: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp124=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp11F;if(_tmp124->tag != 5)goto _LL6A;else{_tmp125=_tmp124->f1;}}_LL69:
 Cyc_CfFlowInfo_add_place(pile,_tmp125);return;_LL6A: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp126=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp11F;if(_tmp126->tag != 6)goto _LL6C;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LL6B:
# 515
{int i=0;for(0;i < _get_dyneither_size(_tmp128,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp128.curr)[i]);}}
return;_LL6C:;_LL6D:
 return;_LL63:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp506(struct _RegionHandle**r,void**new_val,unsigned int*_tmp505,unsigned int*_tmp504,void***_tmp502,struct _dyneither_ptr*_tmp12C){for(*_tmp505=0;*_tmp505 < *_tmp504;(*_tmp505)++){(*_tmp502)[*_tmp505]=(void*)
# 530
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp12C).curr)[(int)*_tmp505]);}}
# 525
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 527
void*_tmp129=old_val;int _tmp12B;struct _dyneither_ptr _tmp12C;struct Cyc_Absyn_Exp*_tmp12E;int _tmp12F;void*_tmp130;struct Cyc_Absyn_Vardecl*_tmp132;void*_tmp133;_LL6F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12A->tag != 6)goto _LL71;else{_tmp12B=(_tmp12A->f1).is_union;_tmp12C=_tmp12A->f2;}}_LL70: {
# 529
unsigned int _tmp505;unsigned int _tmp504;struct _dyneither_ptr _tmp503;void**_tmp502;unsigned int _tmp501;struct _dyneither_ptr d2=(_tmp501=_get_dyneither_size(_tmp12C,sizeof(void*)),((_tmp502=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp501)),((_tmp503=_tag_dyneither(_tmp502,sizeof(void*),_tmp501),((((_tmp504=_tmp501,_tmp506(& r,& new_val,& _tmp505,& _tmp504,& _tmp502,& _tmp12C))),_tmp503)))))));
# 533
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp50C;struct Cyc_CfFlowInfo_UnionRInfo _tmp50B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp50A;return(void*)((_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp50C.tag=6,((_tmp50C.f1=((_tmp50B.is_union=_tmp12B,((_tmp50B.fieldnum=- 1,_tmp50B)))),((_tmp50C.f2=d2,_tmp50C)))))),_tmp50A))));}_LL71: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12D->tag != 7)goto _LL73;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;_tmp130=(void*)_tmp12D->f3;}}_LL72: {
# 535
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp50F;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp50E;return(void*)((_tmp50E=_region_malloc(r,sizeof(*_tmp50E)),((_tmp50E[0]=((_tmp50F.tag=7,((_tmp50F.f1=_tmp12E,((_tmp50F.f2=_tmp12F,((_tmp50F.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp130),_tmp50F)))))))),_tmp50E))));}_LL73: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp131->tag != 8)goto _LL75;else{_tmp132=_tmp131->f1;_tmp133=(void*)_tmp131->f2;}}_LL74: {
# 537
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp512;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp511;return(void*)((_tmp511=_region_malloc(r,sizeof(*_tmp511)),((_tmp511[0]=((_tmp512.tag=8,((_tmp512.f1=_tmp132,((_tmp512.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp133),_tmp512)))))),_tmp511))));}_LL75:;_LL76:
 return new_val;_LL6E:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp518(struct _dyneither_ptr*d,unsigned int*_tmp517,unsigned int*_tmp516,void***_tmp514){for(*_tmp517=0;*_tmp517 < *_tmp516;(*_tmp517)++){(*_tmp514)[*_tmp517]=(void*)((void**)(*d).curr)[(int)*_tmp517];}}
# 544
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 546
void*_tmp13F=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp13F == rval)return d;{
unsigned int _tmp517;unsigned int _tmp516;struct _dyneither_ptr _tmp515;void**_tmp514;unsigned int _tmp513;struct _dyneither_ptr res=(_tmp513=_get_dyneither_size(d,sizeof(void*)),((_tmp514=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp513)),((_tmp515=_tag_dyneither(_tmp514,sizeof(void*),_tmp513),((((_tmp516=_tmp513,_tmp518(& d,& _tmp517,& _tmp516,& _tmp514))),_tmp515)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 558
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 562
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple18 _tmp519;struct _tuple18 _tmp145=(_tmp519.f1=fs,((_tmp519.f2=old_val,_tmp519)));int _tmp146;struct Cyc_List_List*_tmp147;int _tmp149;struct _dyneither_ptr _tmp14A;struct Cyc_Absyn_Vardecl*_tmp14C;void*_tmp14D;_LL78: if(_tmp145.f1 == 0)goto _LL7A;_tmp146=(int)(_tmp145.f1)->hd;_tmp147=(_tmp145.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp148=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145.f2;if(_tmp148->tag != 6)goto _LL7A;else{_tmp149=(_tmp148->f1).is_union;_tmp14A=_tmp148->f2;}};_LL79: {
# 566
void*_tmp14E=Cyc_CfFlowInfo_insert_place_outer(r,_tmp147,*((void**)_check_dyneither_subscript(_tmp14A,sizeof(void*),_tmp146)),new_val);
struct _dyneither_ptr _tmp14F=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp14A,_tmp146,_tmp14E);
if((void**)_tmp14F.curr == (void**)_tmp14A.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp51F;struct Cyc_CfFlowInfo_UnionRInfo _tmp51E;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp51D;return(void*)((_tmp51D=_region_malloc(r,sizeof(*_tmp51D)),((_tmp51D[0]=((_tmp51F.tag=6,((_tmp51F.f1=((_tmp51E.is_union=_tmp149,((_tmp51E.fieldnum=- 1,_tmp51E)))),((_tmp51F.f2=_tmp14F,_tmp51F)))))),_tmp51D))));}}_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp14B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp145.f2;if(_tmp14B->tag != 8)goto _LL7C;else{_tmp14C=_tmp14B->f1;_tmp14D=(void*)_tmp14B->f2;}}_LL7B: {
# 571
void*_tmp153=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp14D,new_val);
if(_tmp153 == _tmp14D)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp522;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp521;return(void*)((_tmp521=_region_malloc(r,sizeof(*_tmp521)),((_tmp521[0]=((_tmp522.tag=8,((_tmp522.f1=_tmp14C,((_tmp522.f2=_tmp153,_tmp522)))))),_tmp521))));}}_LL7C:;_LL7D: {
struct Cyc_Core_Impossible_exn_struct _tmp528;const char*_tmp527;struct Cyc_Core_Impossible_exn_struct*_tmp526;(int)_throw((void*)((_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526[0]=((_tmp528.tag=Cyc_Core_Impossible,((_tmp528.f1=((_tmp527="bad insert place",_tag_dyneither(_tmp527,sizeof(char),17))),_tmp528)))),_tmp526)))));}_LL77:;};}
# 581
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 585
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp159=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp159,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp15A;_push_handler(& _tmp15A);{int _tmp15C=0;if(setjmp(_tmp15A.handler))_tmp15C=1;if(!_tmp15C){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp159);;_pop_handler();}else{void*_tmp15B=(void*)_exn_thrown;void*_tmp15E=_tmp15B;void*_tmp160;_LL7F: {struct Cyc_Dict_Absent_exn_struct*_tmp15F=(struct Cyc_Dict_Absent_exn_struct*)_tmp15E;if(_tmp15F->tag != Cyc_Dict_Absent)goto _LL81;}_LL80:
 continue;_LL81: _tmp160=_tmp15E;_LL82:(void)_throw(_tmp160);_LL7E:;}};}
switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL83:
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL84:
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL85:
 newval=fenv->esc_none;break;}
# 597
Cyc_CfFlowInfo_add_places(pile,oldval);{
void*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_CfFlowInfo_Place _tmp161=*_tmp159;_tmp162=_tmp161.root;_tmp163=_tmp161.fields;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp162,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp163,
# 602
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp162),newval));};};}
# 605
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 617
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp164=r;enum Cyc_CfFlowInfo_InitLevel _tmp166;enum Cyc_CfFlowInfo_InitLevel _tmp168;void*_tmp16D;_LL88: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp165=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp165->tag != 3)goto _LL8A;else{_tmp166=_tmp165->f1;}}_LL89:
 return _tmp166;_LL8A: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp167=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp167->tag != 4)goto _LL8C;else{_tmp168=_tmp167->f1;}}_LL8B:
 return _tmp168;_LL8C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp169=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp169->tag != 0)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp16A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp16A->tag != 1)goto _LL90;}_LL8F:
 return Cyc_CfFlowInfo_AllIL;_LL90: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp16B=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp16B->tag != 2)goto _LL92;}_LL91:
 return Cyc_CfFlowInfo_ThisIL;_LL92: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp16C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp164;if(_tmp16C->tag != 7)goto _LL94;else{_tmp16D=(void*)_tmp16C->f3;}}_LL93:
 return Cyc_CfFlowInfo_NoneIL;_LL94:;_LL95: {
struct Cyc_Core_Impossible_exn_struct _tmp52E;const char*_tmp52D;struct Cyc_Core_Impossible_exn_struct*_tmp52C;(int)_throw((void*)((_tmp52C=_cycalloc(sizeof(*_tmp52C)),((_tmp52C[0]=((_tmp52E.tag=Cyc_Core_Impossible,((_tmp52E.f1=((_tmp52D="initlevel_approx",_tag_dyneither(_tmp52D,sizeof(char),17))),_tmp52E)))),_tmp52C)))));}_LL87:;}
# 628
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 630
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp171=r;void*_tmp173;int _tmp175;struct _dyneither_ptr _tmp176;struct _dyneither_ptr _tmp178;struct Cyc_CfFlowInfo_Place*_tmp17A;_LL97: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp172=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp171;if(_tmp172->tag != 8)goto _LL99;else{_tmp173=(void*)_tmp172->f2;}}_LL98:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp173,acc);_LL99: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp174=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp171;if(_tmp174->tag != 6)goto _LL9B;else{if((_tmp174->f1).is_union != 1)goto _LL9B;_tmp175=(_tmp174->f1).fieldnum;_tmp176=_tmp174->f2;}}_LL9A: {
# 637
unsigned int _tmp17B=_get_dyneither_size(_tmp176,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp175 == - 1){
int i=0;for(0;i < _tmp17B;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp176,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 647
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp176,sizeof(void*),_tmp175)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL96;}_LL9B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp177=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp171;if(_tmp177->tag != 6)goto _LL9D;else{if((_tmp177->f1).is_union != 0)goto _LL9D;_tmp178=_tmp177->f2;}}_LL9C: {
# 651
unsigned int _tmp17C=_get_dyneither_size(_tmp178,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp17C;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp178,sizeof(void*),i)),this_ans);}}
goto _LL96;}_LL9D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp179=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp171;if(_tmp179->tag != 5)goto _LL9F;else{_tmp17A=_tmp179->f1;}}_LL9E:
# 657
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp17A))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 660
{struct Cyc_List_List*_tmp52F;env->seen=((_tmp52F=_region_malloc(env->e,sizeof(*_tmp52F)),((_tmp52F->hd=_tmp17A,((_tmp52F->tl=env->seen,_tmp52F))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp17A),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 666
goto _LL96;_LL9F:;_LLA0:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL96:;}
# 669
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 673
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp17E=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp530;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17F=(_tmp530.e=_tmp17E,((_tmp530.d=d,((_tmp530.seen=0,_tmp530)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp17F,r,Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};
# 680
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
void*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_CfFlowInfo_Place _tmp181=*place;_tmp182=_tmp181.root;_tmp183=_tmp181.fields;{
void*_tmp184=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp182);
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple19 _tmp531;struct _tuple19 _tmp186=(_tmp531.f1=_tmp184,((_tmp531.f2=(int)_tmp183->hd,_tmp531)));struct Cyc_CfFlowInfo_UnionRInfo _tmp189;struct _dyneither_ptr _tmp18A;int _tmp18B;struct Cyc_CfFlowInfo_UnionRInfo _tmp18D;struct _dyneither_ptr _tmp18E;int _tmp18F;_LLA2:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp187=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp186.f1;if(_tmp187->tag != 8)goto _LLA4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp188=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp187->f2);if(_tmp188->tag != 6)goto _LLA4;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;}}}_tmp18B=_tmp186.f2;_LLA3:
# 686
 _tmp18D=_tmp189;_tmp18E=_tmp18A;_tmp18F=_tmp18B;goto _LLA5;_LLA4:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp18C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp186.f1;if(_tmp18C->tag != 6)goto _LLA6;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_tmp18F=_tmp186.f2;_LLA5:
# 688
 _tmp184=*((void**)_check_dyneither_subscript(_tmp18E,sizeof(void*),_tmp18F));
goto _LLA1;_LLA6:;_LLA7: {
# 694
struct Cyc_Core_Impossible_exn_struct _tmp537;const char*_tmp536;struct Cyc_Core_Impossible_exn_struct*_tmp535;(int)_throw((void*)((_tmp535=_cycalloc(sizeof(*_tmp535)),((_tmp535[0]=((_tmp537.tag=Cyc_Core_Impossible,((_tmp537.f1=((_tmp536="bad lookup_place",_tag_dyneither(_tmp536,sizeof(char),17))),_tmp537)))),_tmp535)))));}_LLA1:;}
# 696
return _tmp184;};}
# 699
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp193=rval;void*_tmp196;void*_tmp198;int _tmp19A;int _tmp19B;struct _dyneither_ptr _tmp19C;_LLA9: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp194=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp193;if(_tmp194->tag != 4)goto _LLAB;}_LLAA:
 return 0;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp195=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp193;if(_tmp195->tag != 7)goto _LLAD;else{_tmp196=(void*)_tmp195->f3;}}_LLAC:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp196);_LLAD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp197=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp193;if(_tmp197->tag != 8)goto _LLAF;else{_tmp198=(void*)_tmp197->f2;}}_LLAE:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp198);_LLAF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp199=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp193;if(_tmp199->tag != 6)goto _LLB1;else{_tmp19A=(_tmp199->f1).is_union;_tmp19B=(_tmp199->f1).fieldnum;_tmp19C=_tmp199->f2;}}_LLB0:
# 705
 if(_tmp19A  && _tmp19B != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp19C,sizeof(void*),_tmp19B)));else{
# 708
unsigned int sz=_get_dyneither_size(_tmp19C,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp19C.curr)[i]))return 0;}}
return 1;}_LLB1:;_LLB2:
# 713
 return 1;_LLA8:;}
# 716
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 721
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp19D=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp538;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp538=_region_malloc(_tmp19D,sizeof(*_tmp538)),((_tmp538->rgn=_tmp19D,((_tmp538->places=0,_tmp538)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp545(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp544,unsigned int*_tmp543,void***_tmp541,struct _dyneither_ptr*_tmp1AF,struct _dyneither_ptr*_tmp1AC){for(*_tmp544=0;*_tmp544 < *_tmp543;(*_tmp544)++){(*_tmp541)[*_tmp544]=(void*)
# 759
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1AC).curr)[(int)*_tmp544],*((void**)_check_dyneither_subscript(*_tmp1AF,sizeof(void*),(int)*_tmp544)));}}
# 739
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 744
struct _tuple0 _tmp539;struct _tuple0 _tmp1A0=(_tmp539.f1=oldval,((_tmp539.f2=newval,_tmp539)));void*_tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1A4;void*_tmp1A5;struct Cyc_CfFlowInfo_Place*_tmp1A8;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AB;struct _dyneither_ptr _tmp1AC;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AE;struct _dyneither_ptr _tmp1AF;enum Cyc_CfFlowInfo_InitLevel _tmp1B1;_LLB4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A1->tag != 8)goto _LLB6;else{_tmp1A2=(void*)_tmp1A1->f2;}}_LLB5:
# 746
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp1A2,newval);_LLB6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1A3->tag != 8)goto _LLB8;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=(void*)_tmp1A3->f2;}}_LLB7: {
# 748
void*_tmp1B2=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp1A5);
if(_tmp1B2 == _tmp1A5)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp53C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp53B;return(void*)((_tmp53B=_region_malloc((env->fenv)->r,sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53C.tag=8,((_tmp53C.f1=_tmp1A4,((_tmp53C.f2=_tmp1B2,_tmp53C)))))),_tmp53B))));}}_LLB8:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A6->tag != 4)goto _LLBA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1A7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1A7->tag != 5)goto _LLBA;else{_tmp1A8=_tmp1A7->f1;}};_LLB9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A8);goto _LLBB;_LLBA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A9=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A9->tag != 4)goto _LLBC;}_LLBB:
# 753
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp53F;void*_tmp53E;(_tmp53E=0,Cyc_Tcutil_terr(env->loc,((_tmp53F="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp53F,sizeof(char),67))),_tag_dyneither(_tmp53E,sizeof(void*),0)));}
# 756
return(env->fenv)->esc_all;_LLBC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1AA->tag != 6)goto _LLBE;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1AD->tag != 6)goto _LLBE;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}};_LLBD: {
# 758
unsigned int _tmp544;unsigned int _tmp543;struct _dyneither_ptr _tmp542;void**_tmp541;unsigned int _tmp540;struct _dyneither_ptr new_d=
(_tmp540=_get_dyneither_size(_tmp1AC,sizeof(void*)),((_tmp541=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp540)),((_tmp542=_tag_dyneither(_tmp541,sizeof(void*),_tmp540),((((_tmp543=_tmp540,_tmp545(& env,& _tmp544,& _tmp543,& _tmp541,& _tmp1AF,& _tmp1AC))),_tmp542)))))));
# 762
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1AC,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1AC.curr)[i]){
change=1;break;}}}
# 767
if(!change){
if(!_tmp1AB.is_union)return oldval;
new_d=_tmp1AC;}else{
# 772
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1AC,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1AF,sizeof(void*),i))){
change=1;break;}}}
# 777
if(!change){
if(!_tmp1AB.is_union)return newval;
new_d=_tmp1AF;}}{
# 782
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp548;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp547;return(void*)((_tmp547=_region_malloc((env->fenv)->r,sizeof(*_tmp547)),((_tmp547[0]=((_tmp548.tag=6,((_tmp548.f1=_tmp1AE,((_tmp548.f2=new_d,_tmp548)))))),_tmp547))));};}_LLBE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1B0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1B0->tag != 4)goto _LLC0;else{_tmp1B1=_tmp1B0->f1;}}_LLBF:
# 784
 switch(_tmp1B1){case Cyc_CfFlowInfo_NoneIL: _LLC2:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLC3:
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLC4:
 return(env->fenv)->unknown_all;}_LLC0:;_LLC1:
# 789
 return newval;_LLB3:;}
# 795
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 799
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 804
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp549;struct _tuple18 _tmp1BE=(_tmp549.f1=fs,((_tmp549.f2=oldval,_tmp549)));struct Cyc_Absyn_Vardecl*_tmp1C0;void*_tmp1C1;int _tmp1C2;struct Cyc_List_List*_tmp1C3;int _tmp1C5;int _tmp1C6;struct _dyneither_ptr _tmp1C7;_LLC7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1BF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1BE.f2;if(_tmp1BF->tag != 8)goto _LLC9;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=(void*)_tmp1BF->f2;}}_LLC8: {
# 807
void*_tmp1C8=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp1C1,newval);
if(_tmp1C8 == _tmp1C1)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp54C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54B;return(void*)((_tmp54B=_region_malloc((env->fenv)->r,sizeof(*_tmp54B)),((_tmp54B[0]=((_tmp54C.tag=8,((_tmp54C.f1=_tmp1C0,((_tmp54C.f2=_tmp1C8,_tmp54C)))))),_tmp54B))));}}_LLC9: if(_tmp1BE.f1 == 0)goto _LLCB;_tmp1C2=(int)(_tmp1BE.f1)->hd;_tmp1C3=(_tmp1BE.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1C4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1BE.f2;if(_tmp1C4->tag != 6)goto _LLCB;else{_tmp1C5=(_tmp1C4->f1).is_union;_tmp1C6=(_tmp1C4->f1).fieldnum;_tmp1C7=_tmp1C4->f2;}};_LLCA: {
# 811
void*_tmp1CB=Cyc_CfFlowInfo_assign_place_outer(env,_tmp1C3,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1C7,sizeof(void*),_tmp1C2)),newval);
# 813
struct _dyneither_ptr _tmp1CC=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C7,_tmp1C2,_tmp1CB);
if((void**)_tmp1CC.curr == (void**)_tmp1C7.curr  && (!_tmp1C5  || _tmp1C6 == _tmp1C2))return oldval;
_tmp1C7=_tmp1CC;
# 818
if(_tmp1C5){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp1C7,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp1C2){
struct _dyneither_ptr _tmp1CD=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C7,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp1C7,sizeof(void*),i))));
# 828
if((void**)_tmp1CD.curr != (void**)_tmp1C7.curr){
_tmp1C7=_tmp1CD;
changed=1;}}}}
# 836
if(changed){
void*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_CfFlowInfo_Place*_tmp1CE=env->root_place;_tmp1CF=_tmp1CE->root;_tmp1D0=_tmp1CE->fields;{
int*_tmp54D;struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp54D=_cycalloc_atomic(sizeof(*_tmp54D)),((_tmp54D[0]=fs_prefix,_tmp54D)))),_tmp1D0);
struct Cyc_CfFlowInfo_Place*_tmp54E;struct Cyc_CfFlowInfo_Place*_tmp1D2=(_tmp54E=_region_malloc((env->pile)->rgn,sizeof(*_tmp54E)),((_tmp54E->root=_tmp1CF,((_tmp54E->fields=_tmp1D1,_tmp54E)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1D2,0);};}}{
# 843
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp554;struct Cyc_CfFlowInfo_UnionRInfo _tmp553;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp552;return(void*)((_tmp552=_region_malloc((env->fenv)->r,sizeof(*_tmp552)),((_tmp552[0]=((_tmp554.tag=6,((_tmp554.f1=((_tmp553.is_union=_tmp1C5,((_tmp553.fieldnum=_tmp1C2,_tmp553)))),((_tmp554.f2=_tmp1C7,_tmp554)))))),_tmp552))));};}_LLCB:;_LLCC: {
struct Cyc_Core_Impossible_exn_struct _tmp55A;const char*_tmp559;struct Cyc_Core_Impossible_exn_struct*_tmp558;(int)_throw((void*)((_tmp558=_cycalloc(sizeof(*_tmp558)),((_tmp558[0]=((_tmp55A.tag=Cyc_Core_Impossible,((_tmp55A.f1=((_tmp559="bad assign place",_tag_dyneither(_tmp559,sizeof(char),17))),_tmp55A)))),_tmp558)))));}_LLC6:;};}
# 847
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 858
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1DB=fenv->r;
void*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1DC=place;_tmp1DD=_tmp1DC->root;_tmp1DE=_tmp1DC->fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp55D;struct Cyc_CfFlowInfo_AssignEnv _tmp55C;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp55C.fenv=fenv,((_tmp55C.pile=((_tmp55D=_region_malloc(_tmp1DB,sizeof(*_tmp55D)),((_tmp55D->rgn=_tmp1DB,((_tmp55D->places=0,_tmp55D)))))),((_tmp55C.d=d,((_tmp55C.loc=loc,((_tmp55C.root_place=place,((_tmp55C.all_changed=all_changed,_tmp55C)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1DE,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1DD),r);
# 866
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1DD,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 877
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 887
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 889
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp570(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp56F,unsigned int*_tmp56E,void***_tmp56C,struct _dyneither_ptr*_tmp21F,struct _dyneither_ptr*_tmp21B){for(*_tmp56F=0;*_tmp56F < *_tmp56E;(*_tmp56F)++){(*_tmp56C)[*_tmp56F]=(void*)
# 963 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp21B).curr)[(int)*_tmp56F],*((void**)_check_dyneither_subscript(*_tmp21F,sizeof(void*),(int)*_tmp56F)));}}
# 899
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1E1=(env->fenv)->r;
# 903
{struct _tuple0 _tmp55E;struct _tuple0 _tmp1E3=(_tmp55E.f1=r1,((_tmp55E.f2=r2,_tmp55E)));struct Cyc_Absyn_Vardecl*_tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Vardecl*_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1EE;void*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;int _tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;int _tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F9;int _tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;int _tmp1FE;void*_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp201;struct Cyc_CfFlowInfo_Place*_tmp203;struct Cyc_CfFlowInfo_Place*_tmp205;struct Cyc_CfFlowInfo_Place*_tmp209;struct Cyc_CfFlowInfo_Place*_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp20F;struct Cyc_CfFlowInfo_Place*_tmp211;struct Cyc_CfFlowInfo_Place*_tmp213;int _tmp219;int _tmp21A;struct _dyneither_ptr _tmp21B;int _tmp21D;int _tmp21E;struct _dyneither_ptr _tmp21F;_LLCE:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1E4->tag != 8)goto _LLD0;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=(void*)_tmp1E4->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1E7->tag != 8)goto _LLD0;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=(void*)_tmp1E7->f2;}};_LLCF:
# 906
 if(_tmp1E5 == _tmp1E8){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp561;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp560;return(void*)((_tmp560=_region_malloc(_tmp1E1,sizeof(*_tmp560)),((_tmp560[0]=((_tmp561.tag=8,((_tmp561.f1=_tmp1E5,((_tmp561.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E6,_tmp1E9),_tmp561)))))),_tmp560))));}else{
# 909
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E6,_tmp1E9);}_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1EA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1EA->tag != 8)goto _LLD2;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=(void*)_tmp1EA->f2;}}_LLD1:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1EC,r2);_LLD2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1ED=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1ED->tag != 8)goto _LLD4;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=(void*)_tmp1ED->f2;}}_LLD3:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EF);_LLD4:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1F0->tag != 7)goto _LLD6;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;_tmp1F3=(void*)_tmp1F0->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1F4->tag != 7)goto _LLD6;else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;_tmp1F7=(void*)_tmp1F4->f3;}};_LLD5:
# 914
 if(_tmp1F1 == _tmp1F5  && _tmp1F2 == _tmp1F6){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp564;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp563;return(void*)((_tmp563=_region_malloc(_tmp1E1,sizeof(*_tmp563)),((_tmp563[0]=((_tmp564.tag=7,((_tmp564.f1=_tmp1F1,((_tmp564.f2=_tmp1F2,((_tmp564.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F3,_tmp1F7),_tmp564)))))))),_tmp563))));}
{void*_tmp224=_tmp1F3;struct Cyc_CfFlowInfo_Place*_tmp226;_LLF1: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp225=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp224;if(_tmp225->tag != 5)goto _LLF3;else{_tmp226=_tmp225->f1;}}_LLF2:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp226);goto _LLF0;_LLF3:;_LLF4:
 goto _LLF0;_LLF0:;}
# 920
{void*_tmp227=_tmp1F7;struct Cyc_CfFlowInfo_Place*_tmp229;_LLF6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp228=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp227;if(_tmp228->tag != 5)goto _LLF8;else{_tmp229=_tmp228->f1;}}_LLF7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp229);goto _LLF5;_LLF8:;_LLF9:
 goto _LLF5;_LLF5:;}
# 924
goto _LLCD;_LLD6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1F8->tag != 7)goto _LLD8;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;_tmp1FB=(void*)_tmp1F8->f3;}}_LLD7: {
# 926
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp567;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp566;return(void*)((_tmp566=_region_malloc(_tmp1E1,sizeof(*_tmp566)),((_tmp566[0]=((_tmp567.tag=7,((_tmp567.f1=_tmp1F9,((_tmp567.f2=_tmp1FA,((_tmp567.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1FB,r2),_tmp567)))))))),_tmp566))));}_LLD8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1FC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1FC->tag != 7)goto _LLDA;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;_tmp1FF=(void*)_tmp1FC->f3;}}_LLD9: {
# 928
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp56A;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp569;return(void*)((_tmp569=_region_malloc(_tmp1E1,sizeof(*_tmp569)),((_tmp569[0]=((_tmp56A.tag=7,((_tmp56A.f1=_tmp1FD,((_tmp56A.f2=_tmp1FE,((_tmp56A.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1FF),_tmp56A)))))))),_tmp569))));}_LLDA:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp200=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp200->tag != 5)goto _LLDC;else{_tmp201=_tmp200->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp202=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp202->tag != 5)goto _LLDC;else{_tmp203=_tmp202->f1;}};_LLDB:
# 931
 if(Cyc_CfFlowInfo_place_cmp(_tmp201,_tmp203)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp201);
Cyc_CfFlowInfo_add_place(env->pile,_tmp203);
goto _LLCD;_LLDC:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp204=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp204->tag != 5)goto _LLDE;else{_tmp205=_tmp204->f1;}}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp206=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp206->tag != 2)goto _LLDE;};_LLDD:
# 936
 _tmp209=_tmp205;goto _LLDF;_LLDE:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp207=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp207->tag != 2)goto _LLE0;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp208=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp208->tag != 5)goto _LLE0;else{_tmp209=_tmp208->f1;}};_LLDF:
# 938
 Cyc_CfFlowInfo_add_place(env->pile,_tmp209);
return(env->fenv)->notzerothis;_LLE0:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp20A->tag != 5)goto _LLE2;else{_tmp20B=_tmp20A->f1;}}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20C=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp20C->tag != 1)goto _LLE2;};_LLE1:
# 942
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20B);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLFA:
 return(env->fenv)->notzeroall;default: _LLFB:
 return(env->fenv)->notzerothis;}_LLE2:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20D=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp20D->tag != 1)goto _LLE4;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp20E->tag != 5)goto _LLE4;else{_tmp20F=_tmp20E->f1;}};_LLE3:
# 948
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20F);
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL: _LLFD:
 return(env->fenv)->notzeroall;default: _LLFE:
 return(env->fenv)->notzerothis;}_LLE4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp210=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp210->tag != 5)goto _LLE6;else{_tmp211=_tmp210->f1;}}_LLE5:
# 954
 Cyc_CfFlowInfo_add_place(env->pile,_tmp211);goto _LLCD;_LLE6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp212=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp212->tag != 5)goto _LLE8;else{_tmp213=_tmp212->f1;}}_LLE7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp213);goto _LLCD;_LLE8:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp214=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp214->tag != 1)goto _LLEA;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp215=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp215->tag != 2)goto _LLEA;};_LLE9:
# 958
 goto _LLEB;_LLEA:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp216=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp216->tag != 2)goto _LLEC;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp217=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp217->tag != 1)goto _LLEC;};_LLEB:
 return(env->fenv)->notzerothis;_LLEC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp218=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp218->tag != 6)goto _LLEE;else{_tmp219=(_tmp218->f1).is_union;_tmp21A=(_tmp218->f1).fieldnum;_tmp21B=_tmp218->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp21C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp21C->tag != 6)goto _LLEE;else{_tmp21D=(_tmp21C->f1).is_union;_tmp21E=(_tmp21C->f1).fieldnum;_tmp21F=_tmp21C->f2;}};_LLED: {
# 962
unsigned int _tmp56F;unsigned int _tmp56E;struct _dyneither_ptr _tmp56D;void**_tmp56C;unsigned int _tmp56B;struct _dyneither_ptr new_d=
(_tmp56B=_get_dyneither_size(_tmp21B,sizeof(void*)),((_tmp56C=(void**)_region_malloc(_tmp1E1,_check_times(sizeof(void*),_tmp56B)),((_tmp56D=_tag_dyneither(_tmp56C,sizeof(void*),_tmp56B),((((_tmp56E=_tmp56B,_tmp570(& env,& _tmp56F,& _tmp56E,& _tmp56C,& _tmp21F,& _tmp21B))),_tmp56D)))))));
# 967
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp21B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp21B.curr)[i]){
change=1;break;}}}
# 972
if(!change){
if(!_tmp219)return r1;
new_d=_tmp21B;}else{
# 977
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp21B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp21F,sizeof(void*),i))){
change=1;break;}}}
# 982
if(!change){
if(!_tmp219)return r2;
new_d=_tmp21F;}}{
# 987
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp576;struct Cyc_CfFlowInfo_UnionRInfo _tmp575;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp574;return(void*)((_tmp574=_region_malloc(_tmp1E1,sizeof(*_tmp574)),((_tmp574[0]=((_tmp576.tag=6,((_tmp576.f1=((_tmp575.is_union=_tmp219,((_tmp575.fieldnum=_tmp21A == _tmp21E?_tmp21A: - 1,_tmp575)))),((_tmp576.f2=new_d,_tmp576)))))),_tmp574))));};}_LLEE:;_LLEF:
# 989
 goto _LLCD;_LLCD:;}{
# 991
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp577;struct _tuple0 _tmp236=(_tmp577.f1=r1,((_tmp577.f2=r2,_tmp577)));_LL101: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp237=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp236.f1;if(_tmp237->tag != 4)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp238=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp236.f2;if(_tmp238->tag != 4)goto _LL105;}_LL104: {
# 996
struct _tuple20 _tmp578;struct _tuple20 _tmp23A=(_tmp578.f1=il1,((_tmp578.f2=il2,_tmp578)));_LL108: if(_tmp23A.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL10A;_LL109:
 goto _LL10B;_LL10A: if(_tmp23A.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL10C;_LL10B: return(env->fenv)->esc_none;_LL10C: if(_tmp23A.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL10E;_LL10D:
 goto _LL10F;_LL10E: if(_tmp23A.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL110;_LL10F: return(env->fenv)->esc_this;_LL110:;_LL111:
 return(env->fenv)->esc_all;_LL107:;}_LL105:;_LL106: {
# 1002
struct _tuple20 _tmp579;struct _tuple20 _tmp23C=(_tmp579.f1=il1,((_tmp579.f2=il2,_tmp579)));_LL113: if(_tmp23C.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL115;_LL114:
 goto _LL116;_LL115: if(_tmp23C.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL117;_LL116: return(env->fenv)->unknown_none;_LL117: if(_tmp23C.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL119;_LL118:
 goto _LL11A;_LL119: if(_tmp23C.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL11B;_LL11A: return(env->fenv)->unknown_this;_LL11B:;_LL11C:
 return(env->fenv)->unknown_all;_LL112:;}_LL100:;};};}struct _tuple21{union Cyc_CfFlowInfo_RelnOp f1;union Cyc_CfFlowInfo_RelnOp f2;};
# 1010
static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp r1,union Cyc_CfFlowInfo_RelnOp r2){
struct _tuple21 _tmp57A;struct _tuple21 _tmp23E=(_tmp57A.f1=r1,((_tmp57A.f2=r2,_tmp57A)));unsigned int _tmp23F;unsigned int _tmp240;struct Cyc_Absyn_Vardecl*_tmp241;struct Cyc_Absyn_Vardecl*_tmp242;struct Cyc_Absyn_Vardecl*_tmp243;struct Cyc_Absyn_Vardecl*_tmp244;struct Cyc_Absyn_Tvar*_tmp245;struct Cyc_Absyn_Tvar*_tmp246;_LL11E: if(((_tmp23E.f1).RConst).tag != 1)goto _LL120;_tmp23F=(unsigned int)((_tmp23E.f1).RConst).val;if(((_tmp23E.f2).RConst).tag != 1)goto _LL120;_tmp240=(unsigned int)((_tmp23E.f2).RConst).val;_LL11F:
 return _tmp23F == _tmp240;_LL120: if(((_tmp23E.f1).RVar).tag != 2)goto _LL122;_tmp241=(struct Cyc_Absyn_Vardecl*)((_tmp23E.f1).RVar).val;if(((_tmp23E.f2).RVar).tag != 2)goto _LL122;_tmp242=(struct Cyc_Absyn_Vardecl*)((_tmp23E.f2).RVar).val;_LL121:
 return _tmp241 == _tmp242;_LL122: if(((_tmp23E.f1).RNumelts).tag != 3)goto _LL124;_tmp243=(struct Cyc_Absyn_Vardecl*)((_tmp23E.f1).RNumelts).val;if(((_tmp23E.f2).RNumelts).tag != 3)goto _LL124;_tmp244=(struct Cyc_Absyn_Vardecl*)((_tmp23E.f2).RNumelts).val;_LL123:
 return _tmp243 == _tmp244;_LL124: if(((_tmp23E.f1).RTvar).tag != 4)goto _LL126;_tmp245=(struct Cyc_Absyn_Tvar*)((_tmp23E.f1).RTvar).val;if(((_tmp23E.f2).RTvar).tag != 4)goto _LL126;_tmp246=(struct Cyc_Absyn_Tvar*)((_tmp23E.f2).RTvar).val;_LL125:
# 1016
 return Cyc_Absyn_tvar_cmp(_tmp245,_tmp246)== 0;_LL126:;_LL127:
 return 0;_LL11D:;}
# 1021
static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
# 1025
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp247=r1s;for(0;_tmp247 != 0;_tmp247=_tmp247->tl){
struct Cyc_CfFlowInfo_Reln*_tmp248=(struct Cyc_CfFlowInfo_Reln*)_tmp247->hd;
int found=0;
{struct Cyc_List_List*_tmp249=r2s;for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
struct Cyc_CfFlowInfo_Reln*_tmp24A=(struct Cyc_CfFlowInfo_Reln*)_tmp249->hd;
if(_tmp248 == _tmp24A  || (Cyc_CfFlowInfo_same_relop(_tmp248->rop1,_tmp24A->rop1) && _tmp248->relation == _tmp24A->relation) && 
# 1034
Cyc_CfFlowInfo_same_relop(_tmp248->rop2,_tmp24A->rop2)){
{struct Cyc_List_List*_tmp57B;res=((_tmp57B=_region_malloc(r,sizeof(*_tmp57B)),((_tmp57B->hd=_tmp248,((_tmp57B->tl=res,_tmp57B))))));}
found=1;
break;}}}
# 1040
if(!found)diff=1;}}
# 1042
if(!diff)res=r1s;
# 1044
return res;};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 1047
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1050
struct _tuple22 _tmp57C;struct _tuple22 _tmp24D=(_tmp57C.f1=f1,((_tmp57C.f2=f2,_tmp57C)));struct Cyc_Dict_Dict _tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_Dict_Dict _tmp250;struct Cyc_List_List*_tmp251;_LL129: if(((_tmp24D.f1).BottomFL).tag != 1)goto _LL12B;_LL12A:
 return f2;_LL12B: if(((_tmp24D.f2).BottomFL).tag != 1)goto _LL12D;_LL12C:
 return f1;_LL12D: if(((_tmp24D.f1).ReachableFL).tag != 2)goto _LL128;_tmp24E=((struct _tuple11)((_tmp24D.f1).ReachableFL).val).f1;_tmp24F=((struct _tuple11)((_tmp24D.f1).ReachableFL).val).f2;if(((_tmp24D.f2).ReachableFL).tag != 2)goto _LL128;_tmp250=((struct _tuple11)((_tmp24D.f2).ReachableFL).val).f1;_tmp251=((struct _tuple11)((_tmp24D.f2).ReachableFL).val).f2;_LL12E:
# 1056
 if(_tmp24E.t == _tmp250.t  && _tmp24F == _tmp251)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp252=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp57F;struct Cyc_CfFlowInfo_JoinEnv _tmp57E;struct Cyc_CfFlowInfo_JoinEnv _tmp253=(_tmp57E.fenv=fenv,((_tmp57E.pile=((_tmp57F=_region_malloc(_tmp252,sizeof(*_tmp57F)),((_tmp57F->rgn=_tmp252,((_tmp57F->places=0,_tmp57F)))))),((_tmp57E.d1=_tmp24E,((_tmp57E.d2=_tmp250,_tmp57E)))))));
struct Cyc_Dict_Dict _tmp254=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp253,_tmp24E,_tmp250);
struct Cyc_List_List*_tmp255=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp24F,_tmp251);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp253.pile,all_changed,_tmp254),_tmp255);};_LL128:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1068
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){
# 1075
union Cyc_CfFlowInfo_FlowInfo _tmp259;void*_tmp25A;struct _tuple12 _tmp258=pr1;_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp25C;void*_tmp25D;struct _tuple12 _tmp25B=pr2;_tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp259,_tmp25C);
struct _tuple23 _tmp580;struct _tuple23 _tmp25F=(_tmp580.f1=_tmp259,((_tmp580.f2=_tmp25C,((_tmp580.f3=outflow,_tmp580)))));struct Cyc_Dict_Dict _tmp260;struct Cyc_Dict_Dict _tmp261;struct Cyc_Dict_Dict _tmp262;struct Cyc_List_List*_tmp263;_LL130: if(((_tmp25F.f1).BottomFL).tag != 1)goto _LL132;_LL131: {
struct _tuple12 _tmp581;return(_tmp581.f1=outflow,((_tmp581.f2=_tmp25D,_tmp581)));}_LL132: if(((_tmp25F.f2).BottomFL).tag != 1)goto _LL134;_LL133: {
struct _tuple12 _tmp582;return(_tmp582.f1=outflow,((_tmp582.f2=_tmp25A,_tmp582)));}_LL134: if(((_tmp25F.f1).ReachableFL).tag != 2)goto _LL136;_tmp260=((struct _tuple11)((_tmp25F.f1).ReachableFL).val).f1;if(((_tmp25F.f2).ReachableFL).tag != 2)goto _LL136;_tmp261=((struct _tuple11)((_tmp25F.f2).ReachableFL).val).f1;if(((_tmp25F.f3).ReachableFL).tag != 2)goto _LL136;_tmp262=((struct _tuple11)((_tmp25F.f3).ReachableFL).val).f1;_tmp263=((struct _tuple11)((_tmp25F.f3).ReachableFL).val).f2;_LL135:
# 1083
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp25A,_tmp25D)){struct _tuple12 _tmp583;return(_tmp583.f1=outflow,((_tmp583.f2=_tmp25D,_tmp583)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp25D,_tmp25A)){struct _tuple12 _tmp584;return(_tmp584.f1=outflow,((_tmp584.f2=_tmp25A,_tmp584)));}{
struct _RegionHandle*_tmp268=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp587;struct Cyc_CfFlowInfo_JoinEnv _tmp586;struct Cyc_CfFlowInfo_JoinEnv _tmp269=(_tmp586.fenv=fenv,((_tmp586.pile=((_tmp587=_region_malloc(_tmp268,sizeof(*_tmp587)),((_tmp587->rgn=_tmp268,((_tmp587->places=0,_tmp587)))))),((_tmp586.d1=_tmp260,((_tmp586.d2=_tmp261,_tmp586)))))));
void*_tmp26A=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp269,0,_tmp25A,_tmp25D);
struct _tuple12 _tmp588;return(_tmp588.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp269.pile,all_changed,_tmp262),_tmp263),((_tmp588.f2=_tmp26A,_tmp588)));};_LL136:;_LL137: {
# 1091
struct Cyc_Core_Impossible_exn_struct _tmp58E;const char*_tmp58D;struct Cyc_Core_Impossible_exn_struct*_tmp58C;(int)_throw((void*)((_tmp58C=_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((_tmp58E.tag=Cyc_Core_Impossible,((_tmp58E.f1=((_tmp58D="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp58D,sizeof(char),37))),_tmp58E)))),_tmp58C)))));}_LL12F:;};};}
# 1095
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp598(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp597,unsigned int*_tmp596,void***_tmp594,struct _dyneither_ptr*_tmp284,struct _dyneither_ptr*_tmp280){for(*_tmp597=0;*_tmp597 < *_tmp596;(*_tmp597)++){(*_tmp594)[*_tmp597]=(void*)
# 1136
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp597,((void**)(*_tmp280).curr)[(int)*_tmp597],*((void**)_check_dyneither_subscript(*_tmp284,sizeof(void*),(int)*_tmp597)));}}
# 1096
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1104
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1109
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1117
struct _tuple0 _tmp58F;struct _tuple0 _tmp272=(_tmp58F.f1=r1,((_tmp58F.f2=r2,_tmp58F)));struct Cyc_Absyn_Vardecl*_tmp276;void*_tmp277;struct Cyc_Absyn_Vardecl*_tmp279;void*_tmp27A;int _tmp27E;int _tmp27F;struct _dyneither_ptr _tmp280;int _tmp282;int _tmp283;struct _dyneither_ptr _tmp284;_LL139: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp273=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp273->tag != 7)goto _LL13B;}_LL13A:
# 1119
 goto _LL13C;_LL13B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp274=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp274->tag != 7)goto _LL13D;}_LL13C:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL13D:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp275=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp275->tag != 8)goto _LL13F;else{_tmp276=_tmp275->f1;_tmp277=(void*)_tmp275->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp278=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp278->tag != 8)goto _LL13F;else{_tmp279=_tmp278->f1;_tmp27A=(void*)_tmp278->f2;}};_LL13E:
# 1122
 if(_tmp276 == _tmp279){
void*_tmp285=Cyc_CfFlowInfo_after_absRval(env,_tmp277,_tmp27A);
if(_tmp285 == _tmp277)return r1;
if(_tmp285 == _tmp27A)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp592;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp591;return(void*)((_tmp591=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp591)),((_tmp591[0]=((_tmp592.tag=8,((_tmp592.f1=_tmp276,((_tmp592.f2=_tmp285,_tmp592)))))),_tmp591))));};}else{
# 1128
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}_LL13F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp27B->tag != 8)goto _LL141;}_LL140:
 goto _LL142;_LL141: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp27C->tag != 8)goto _LL143;}_LL142:
# 1131
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL143:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp27D->tag != 6)goto _LL145;else{_tmp27E=(_tmp27D->f1).is_union;_tmp27F=(_tmp27D->f1).fieldnum;_tmp280=_tmp27D->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp281=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp281->tag != 6)goto _LL145;else{_tmp282=(_tmp281->f1).is_union;_tmp283=(_tmp281->f1).fieldnum;_tmp284=_tmp281->f2;}};_LL144: {
# 1134
unsigned int _tmp597;unsigned int _tmp596;struct _dyneither_ptr _tmp595;void**_tmp594;unsigned int _tmp593;struct _dyneither_ptr new_d=
(_tmp593=
_get_dyneither_size(_tmp280,sizeof(void*)),((_tmp594=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp593)),((_tmp595=_tag_dyneither(_tmp594,sizeof(void*),_tmp593),((((_tmp596=_tmp593,_tmp598(& env,& _tmp597,& _tmp596,& _tmp594,& _tmp284,& _tmp280))),_tmp595)))))));
# 1141
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp280,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp280.curr)[i]){
change=1;break;}}}
# 1146
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp280,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp284,sizeof(void*),i))){
change=1;break;}}}
# 1153
if(!change)
return r2;{
# 1157
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp59E;struct Cyc_CfFlowInfo_UnionRInfo _tmp59D;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp59C;return(void*)((_tmp59C=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp59C)),((_tmp59C[0]=((_tmp59E.tag=6,((_tmp59E.f1=((_tmp59D.is_union=_tmp27E,((_tmp59D.fieldnum=_tmp27F == _tmp283?_tmp27F: - 1,_tmp59D)))),((_tmp59E.f2=new_d,_tmp59E)))))),_tmp59C))));};}_LL145:;_LL146: {
# 1165
struct Cyc_Core_Impossible_exn_struct _tmp5A4;const char*_tmp5A3;struct Cyc_Core_Impossible_exn_struct*_tmp5A2;(int)_throw((void*)((_tmp5A2=_cycalloc(sizeof(*_tmp5A2)),((_tmp5A2[0]=((_tmp5A4.tag=Cyc_Core_Impossible,((_tmp5A4.f1=((_tmp5A3="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5A3,sizeof(char),33))),_tmp5A4)))),_tmp5A2)))));}_LL138:;};}
# 1168
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1171
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp292=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp293=env->changed;
{struct Cyc_List_List*_tmp5A5;*env->last_field_cell=((_tmp5A5=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5A5)),((_tmp5A5->hd=(void*)key,((_tmp5A5->tl=0,_tmp5A5))))));}
# 1177
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp295=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp292;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp293;
return _tmp295;};};}
# 1184
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1186
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp5A6;*env->curr_place=((_tmp5A6.root=root,((_tmp5A6.fields=0,_tmp5A6))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1193
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1199
struct _tuple22 _tmp5A7;struct _tuple22 _tmp298=(_tmp5A7.f1=f1,((_tmp5A7.f2=f2,_tmp5A7)));struct Cyc_Dict_Dict _tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_Dict_Dict _tmp29B;struct Cyc_List_List*_tmp29C;_LL148: if(((_tmp298.f1).BottomFL).tag != 1)goto _LL14A;_LL149:
 goto _LL14B;_LL14A: if(((_tmp298.f2).BottomFL).tag != 1)goto _LL14C;_LL14B:
 return Cyc_CfFlowInfo_BottomFL();_LL14C: if(((_tmp298.f1).ReachableFL).tag != 2)goto _LL147;_tmp299=((struct _tuple11)((_tmp298.f1).ReachableFL).val).f1;_tmp29A=((struct _tuple11)((_tmp298.f1).ReachableFL).val).f2;if(((_tmp298.f2).ReachableFL).tag != 2)goto _LL147;_tmp29B=((struct _tuple11)((_tmp298.f2).ReachableFL).val).f1;_tmp29C=((struct _tuple11)((_tmp298.f2).ReachableFL).val).f2;_LL14D:
# 1203
 if(_tmp299.t == _tmp29B.t  && _tmp29A == _tmp29C)return f1;{
# 1206
struct _RegionHandle*_tmp29D=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp29E=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp5AB;struct Cyc_CfFlowInfo_AfterEnv _tmp5AA;struct Cyc_CfFlowInfo_AfterEnv _tmp29F=(_tmp5AA.joinenv=(((_tmp5AA.joinenv).fenv=fenv,(((_tmp5AA.joinenv).pile=((_tmp5AB=_region_malloc(_tmp29D,sizeof(*_tmp5AB)),((_tmp5AB->rgn=_tmp29D,((_tmp5AB->places=0,_tmp5AB)))))),(((_tmp5AA.joinenv).d1=_tmp299,(((_tmp5AA.joinenv).d2=_tmp29B,_tmp5AA.joinenv)))))))),((_tmp5AA.chg1=chg1,((_tmp5AA.chg2=chg2,((_tmp5AA.curr_place=_tmp29E,((_tmp5AA.last_field_cell=& _tmp29E->fields,((_tmp5AA.changed=Cyc_CfFlowInfo_Neither,_tmp5AA)))))))))));
# 1212
struct Cyc_Dict_Dict _tmp2A0=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp29F,_tmp299,_tmp29B);
struct Cyc_List_List*_tmp2A1=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp29A,_tmp29C);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp29F.joinenv).pile,all_changed,_tmp2A0),_tmp2A1);};_LL147:;}
# 1221
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1225
{struct _tuple0 _tmp5AC;struct _tuple0 _tmp2A5=(_tmp5AC.f1=r1,((_tmp5AC.f2=r2,_tmp5AC)));struct Cyc_Absyn_Vardecl*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2AA;void*_tmp2AB;struct Cyc_CfFlowInfo_Place*_tmp2AF;struct Cyc_CfFlowInfo_Place*_tmp2B1;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B5;struct _dyneither_ptr _tmp2B6;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B8;struct _dyneither_ptr _tmp2B9;_LL14F:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2A6->tag != 8)goto _LL151;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=(void*)_tmp2A6->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2A9->tag != 8)goto _LL151;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=(void*)_tmp2A9->f2;}};_LL150:
# 1227
 return _tmp2A7 == _tmp2AA  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp2A8,_tmp2AB);_LL151: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2AC->tag != 8)goto _LL153;}_LL152:
 goto _LL154;_LL153: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2AD->tag != 8)goto _LL155;}_LL154:
 return 0;_LL155:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2AE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2AE->tag != 5)goto _LL157;else{_tmp2AF=_tmp2AE->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2B0->tag != 5)goto _LL157;else{_tmp2B1=_tmp2B0->f1;}};_LL156:
 return Cyc_CfFlowInfo_place_cmp(_tmp2AF,_tmp2B1)== 0;_LL157: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2B2->tag != 5)goto _LL159;}_LL158:
 goto _LL15A;_LL159: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2B3->tag != 5)goto _LL15B;}_LL15A:
 return 0;_LL15B:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2B4->tag != 6)goto _LL15D;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2B7->tag != 6)goto _LL15D;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;}};_LL15C:
# 1234
 if((void**)_tmp2B6.curr == (void**)_tmp2B9.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp2B6,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2B6.curr)[i],*((void**)_check_dyneither_subscript(_tmp2B9,sizeof(void*),i))))return 0;}}
return 1;_LL15D:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2BA=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2BA->tag != 1)goto _LL15F;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2BB=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2BB->tag != 2)goto _LL15F;};_LL15E:
 return 1;_LL15F: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2BC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2BC->tag != 2)goto _LL161;}_LL160:
 return 0;_LL161: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp2BD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2BD->tag != 0)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2BE=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2BE->tag != 1)goto _LL165;}_LL164:
 return 0;_LL165:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2BF=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2BF->tag != 4)goto _LL167;}{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2C0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2C0->tag != 4)goto _LL167;};_LL166:
 goto _LL14E;_LL167: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2C1=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2C1->tag != 4)goto _LL169;}_LL168:
 return 0;_LL169:;_LL16A:
 goto _LL14E;_LL14E:;}{
# 1246
struct _tuple20 _tmp5AD;struct _tuple20 _tmp2C3=(_tmp5AD.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5AD.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp5AD)));_LL16C: if(_tmp2C3.f1 != Cyc_CfFlowInfo_AllIL)goto _LL16E;if(_tmp2C3.f2 != Cyc_CfFlowInfo_AllIL)goto _LL16E;_LL16D:
 return 1;_LL16E: if(_tmp2C3.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL170;_LL16F:
 return 1;_LL170: if(_tmp2C3.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL172;_LL171:
 return 0;_LL172: if(_tmp2C3.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL174;_LL173:
 return 1;_LL174: if(_tmp2C3.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL16B;_LL175:
 return 0;_LL16B:;};}
# 1256
struct Cyc_List_List*Cyc_CfFlowInfo_add_relation(struct _RegionHandle*rgn,union Cyc_CfFlowInfo_RelnOp rop1,enum Cyc_CfFlowInfo_Relation relation,union Cyc_CfFlowInfo_RelnOp rop2,struct Cyc_List_List*relns){
# 1260
{struct Cyc_List_List*_tmp2C4=relns;for(0;_tmp2C4 != 0;_tmp2C4=_tmp2C4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C5=(struct Cyc_CfFlowInfo_Reln*)_tmp2C4->hd;
if((Cyc_CfFlowInfo_same_relop(_tmp2C5->rop1,rop1) && _tmp2C5->relation == relation) && 
Cyc_CfFlowInfo_same_relop(_tmp2C5->rop2,rop2))return relns;}}{
# 1265
struct Cyc_CfFlowInfo_Reln*_tmp5B0;struct Cyc_List_List*_tmp5AF;return(_tmp5AF=_region_malloc(rgn,sizeof(*_tmp5AF)),((_tmp5AF->hd=((_tmp5B0=_region_malloc(rgn,sizeof(*_tmp5B0)),((_tmp5B0->rop1=rop1,((_tmp5B0->relation=relation,((_tmp5B0->rop2=rop2,_tmp5B0)))))))),((_tmp5AF->tl=relns,_tmp5AF)))));};}
# 1268
static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 1272
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C8=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp2C9=r2s;for(0;_tmp2C9 != 0;_tmp2C9=_tmp2C9->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2CA=(struct Cyc_CfFlowInfo_Reln*)_tmp2C9->hd;
if(_tmp2C8 == _tmp2CA  || (Cyc_CfFlowInfo_same_relop(_tmp2C8->rop1,_tmp2CA->rop1) && _tmp2C8->relation == _tmp2CA->relation) && 
# 1279
Cyc_CfFlowInfo_same_relop(_tmp2C8->rop2,_tmp2CA->rop2)){
found=1;
break;}}}
# 1284
if(!found)
return 0;}
# 1287
return 1;}
# 1295
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1297
struct _tuple22 _tmp5B1;struct _tuple22 _tmp2CC=(_tmp5B1.f1=f1,((_tmp5B1.f2=f2,_tmp5B1)));struct Cyc_Dict_Dict _tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;struct Cyc_List_List*_tmp2D0;_LL177: if(((_tmp2CC.f1).BottomFL).tag != 1)goto _LL179;_LL178:
 return 1;_LL179: if(((_tmp2CC.f2).BottomFL).tag != 1)goto _LL17B;_LL17A:
 return 0;_LL17B: if(((_tmp2CC.f1).ReachableFL).tag != 2)goto _LL176;_tmp2CD=((struct _tuple11)((_tmp2CC.f1).ReachableFL).val).f1;_tmp2CE=((struct _tuple11)((_tmp2CC.f1).ReachableFL).val).f2;if(((_tmp2CC.f2).ReachableFL).tag != 2)goto _LL176;_tmp2CF=((struct _tuple11)((_tmp2CC.f2).ReachableFL).val).f1;_tmp2D0=((struct _tuple11)((_tmp2CC.f2).ReachableFL).val).f2;_LL17C:
# 1301
 if(_tmp2CD.t == _tmp2CF.t  && _tmp2CE == _tmp2D0)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2CD,_tmp2CF) && 
Cyc_CfFlowInfo_relns_approx(_tmp2CE,_tmp2D0);_LL176:;}
# 1307
static int Cyc_CfFlowInfo_rop_contains_var(union Cyc_CfFlowInfo_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp2D1=r;struct Cyc_Absyn_Vardecl*_tmp2D2;struct Cyc_Absyn_Vardecl*_tmp2D3;_LL17E: if((_tmp2D1.RVar).tag != 2)goto _LL180;_tmp2D2=(struct Cyc_Absyn_Vardecl*)(_tmp2D1.RVar).val;_LL17F:
 return v == _tmp2D2;_LL180: if((_tmp2D1.RNumelts).tag != 3)goto _LL182;_tmp2D3=(struct Cyc_Absyn_Vardecl*)(_tmp2D1.RNumelts).val;_LL181:
 return v == _tmp2D3;_LL182: if((_tmp2D1.RTvar).tag != 4)goto _LL184;_LL183:
 goto _LL185;_LL184: if((_tmp2D1.RConst).tag != 1)goto _LL17D;_LL185:
 return 0;_LL17D:;}
# 1316
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2D4=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(Cyc_CfFlowInfo_rop_contains_var(_tmp2D4->rop1,v) || Cyc_CfFlowInfo_rop_contains_var(_tmp2D4->rop2,v)){
found=1;
break;}}
# 1326
if(!found)return rs;{
# 1328
struct Cyc_List_List*_tmp2D5=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2D6=(struct Cyc_CfFlowInfo_Reln*)p->hd;
if(Cyc_CfFlowInfo_rop_contains_var(_tmp2D6->rop1,v) || Cyc_CfFlowInfo_rop_contains_var(_tmp2D6->rop2,v))continue;{
struct Cyc_List_List*_tmp5B2;_tmp2D5=((_tmp5B2=_region_malloc(rgn,sizeof(*_tmp5B2)),((_tmp5B2->hd=_tmp2D6,((_tmp5B2->tl=_tmp2D5,_tmp5B2))))));};}
# 1334
return _tmp2D5;};}
# 1337
struct Cyc_Absyn_Vardecl*Cyc_CfFlowInfo_nonesc_vardecl(void*b){
{void*_tmp2D8=b;struct Cyc_Absyn_Vardecl*_tmp2DA;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DE;struct Cyc_Absyn_Vardecl*_tmp2E0;_LL187: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2D9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp2D8;if(_tmp2D9->tag != 5)goto _LL189;else{_tmp2DA=_tmp2D9->f1;}}_LL188:
 _tmp2DC=_tmp2DA;goto _LL18A;_LL189: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2DB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp2D8;if(_tmp2DB->tag != 4)goto _LL18B;else{_tmp2DC=_tmp2DB->f1;}}_LL18A:
 _tmp2DE=_tmp2DC;goto _LL18C;_LL18B: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp2D8;if(_tmp2DD->tag != 3)goto _LL18D;else{_tmp2DE=_tmp2DD->f1;}}_LL18C:
 _tmp2E0=_tmp2DE;goto _LL18E;_LL18D: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2DF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp2D8;if(_tmp2DF->tag != 1)goto _LL18F;else{_tmp2E0=_tmp2DF->f1;}}_LL18E:
# 1343
 if(!_tmp2E0->escapes)return _tmp2E0;
goto _LL186;_LL18F:;_LL190:
 goto _LL186;_LL186:;}
# 1347
return 0;}
# 1353
int Cyc_CfFlowInfo_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_RelnOp*p){
# 1355
{void*_tmp2E1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E5;_LL192: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2E1;if(_tmp2E2->tag != 19)goto _LL194;else{_tmp2E3=(void*)_tmp2E2->f1;}}_LL193:
# 1357
{void*_tmp2E6=Cyc_Tcutil_compress(_tmp2E3);struct Cyc_Absyn_Tvar*_tmp2E8;_LL199: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E7->tag != 2)goto _LL19B;else{_tmp2E8=_tmp2E7->f1;}}_LL19A:
*p=Cyc_CfFlowInfo_RTvar(_tmp2E8);return 1;_LL19B:;_LL19C:
 goto _LL198;_LL198:;}
# 1361
goto _LL191;_LL194: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2E4=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2E1;if(_tmp2E4->tag != 18)goto _LL196;else{_tmp2E5=_tmp2E4->f1;}}_LL195:
# 1363
 e=_tmp2E5;
goto _LL191;_LL196:;_LL197:
 goto _LL191;_LL191:;}
# 1367
{void*_tmp2E9=e->r;struct Cyc_Absyn_Exp*_tmp2EB;void*_tmp2ED;int _tmp2EF;void*_tmp2F2;struct Cyc_Absyn_Tvar*_tmp2F5;_LL19E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E9;if(_tmp2EA->tag != 13)goto _LL1A0;else{_tmp2EB=_tmp2EA->f2;}}_LL19F:
 return Cyc_CfFlowInfo_exp2relnop(_tmp2EB,p);_LL1A0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E9;if(_tmp2EC->tag != 1)goto _LL1A2;else{_tmp2ED=(void*)_tmp2EC->f2;}}_LL1A1: {
# 1370
struct Cyc_Absyn_Vardecl*_tmp2F6=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2ED);
if(_tmp2F6 != 0){
*p=Cyc_CfFlowInfo_RVar(_tmp2F6);
return 1;}
# 1375
goto _LL19D;}_LL1A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2EE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E9;if(_tmp2EE->tag != 0)goto _LL1A4;else{if(((_tmp2EE->f1).Int_c).tag != 5)goto _LL1A4;_tmp2EF=((struct _tuple6)((_tmp2EE->f1).Int_c).val).f2;}}_LL1A3:
# 1377
*p=Cyc_CfFlowInfo_RConst((unsigned int)_tmp2EF);
return 1;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2F0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E9;if(_tmp2F0->tag != 2)goto _LL1A6;else{if(_tmp2F0->f1 != Cyc_Absyn_Numelts)goto _LL1A6;if(_tmp2F0->f2 == 0)goto _LL1A6;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(_tmp2F0->f2)->hd)->r;if(_tmp2F1->tag != 1)goto _LL1A6;else{_tmp2F2=(void*)_tmp2F1->f2;}};}}_LL1A5: {
# 1380
struct Cyc_Absyn_Vardecl*_tmp2F7=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2F2);
if(_tmp2F7 != 0){
*p=Cyc_CfFlowInfo_RNumelts(_tmp2F7);
return 1;}
# 1385
goto _LL19D;}_LL1A6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2F3=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2E9;if(_tmp2F3->tag != 38)goto _LL1A8;else{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)((void*)_tmp2F3->f1);if(_tmp2F4->tag != 2)goto _LL1A8;else{_tmp2F5=_tmp2F4->f1;}}}_LL1A7:
# 1387
*p=Cyc_CfFlowInfo_RTvar(_tmp2F5);
return 1;_LL1A8:;_LL1A9:
 goto _LL19D;_LL19D:;}
# 1391
return 0;}
# 1394
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp2F8=e->r;void*_tmp2FA;_LL1AB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp2F9->tag != 1)goto _LL1AD;else{_tmp2FA=(void*)_tmp2F9->f2;}}_LL1AC: {
# 1397
struct Cyc_Absyn_Vardecl*_tmp2FB=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2FA);
if(_tmp2FB != 0)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2FB);else{
return r;}}_LL1AD:;_LL1AE:
 return r;_LL1AA:;}
# 1405
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp5B3;return(_tmp5B3=_region_malloc(r2,sizeof(*_tmp5B3)),((_tmp5B3[0]=*r,_tmp5B3)));}
# 1409
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
# 1413
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_CfFlowInfo_Reln*_tmp2FD=(struct Cyc_CfFlowInfo_Reln*)r1->hd;
struct Cyc_CfFlowInfo_Reln*_tmp2FE=(struct Cyc_CfFlowInfo_Reln*)r2->hd;
if((!Cyc_CfFlowInfo_same_relop(_tmp2FD->rop1,_tmp2FE->rop1) || _tmp2FD->relation != _tmp2FE->relation) || !
# 1419
Cyc_CfFlowInfo_same_relop(_tmp2FD->rop2,_tmp2FE->rop2))return 0;}
# 1421
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 1425
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 1429
if(v->escapes)return r;
# 1432
r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);
# 1437
{void*_tmp2FF=e->r;struct Cyc_Absyn_Exp*_tmp301;_LL1B0: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp300=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF;if(_tmp300->tag != 33)goto _LL1B2;else{_tmp301=(_tmp300->f1).num_elts;if((_tmp300->f1).fat_result != 1)goto _LL1B2;}}_LL1B1:
# 1440
 malloc_loop: {
void*_tmp302=_tmp301->r;struct Cyc_Absyn_Exp*_tmp304;void*_tmp306;_LL1B5: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp303=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp302;if(_tmp303->tag != 13)goto _LL1B7;else{_tmp304=_tmp303->f2;}}_LL1B6:
 _tmp301=_tmp304;goto malloc_loop;_LL1B7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp302;if(_tmp305->tag != 1)goto _LL1B9;else{_tmp306=(void*)_tmp305->f2;}}_LL1B8: {
# 1444
struct Cyc_Absyn_Vardecl*_tmp307=Cyc_CfFlowInfo_nonesc_vardecl(_tmp306);
if(_tmp307 != 0)
return Cyc_CfFlowInfo_add_relation(rgn,Cyc_CfFlowInfo_RVar(_tmp307),Cyc_CfFlowInfo_Req,Cyc_CfFlowInfo_RNumelts(v),r);else{
return r;}}_LL1B9:;_LL1BA:
 return r;_LL1B4:;}_LL1B2:;_LL1B3:
# 1451
 goto _LL1AF;_LL1AF:;}
# 1454
{void*_tmp308=Cyc_Tcutil_compress(v->type);_LL1BC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp308;if(_tmp309->tag != 6)goto _LL1BE;}_LL1BD:
 goto _LL1BB;_LL1BE: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp308;if(_tmp30A->tag != 19)goto _LL1C0;}_LL1BF:
 goto _LL1BB;_LL1C0:;_LL1C1:
 return r;_LL1BB:;}{
# 1460
union Cyc_CfFlowInfo_RelnOp eop=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(e,& eop))
return Cyc_CfFlowInfo_add_relation(rgn,Cyc_CfFlowInfo_RVar(v),Cyc_CfFlowInfo_Req,eop,r);
# 1464
return r;};}
# 1467
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp30B=e1->r;void*_tmp30D;_LL1C3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30B;if(_tmp30C->tag != 1)goto _LL1C5;else{_tmp30D=(void*)_tmp30C->f2;}}_LL1C4: {
# 1470
struct Cyc_Absyn_Vardecl*_tmp30E=Cyc_CfFlowInfo_nonesc_vardecl(_tmp30D);
if(_tmp30E != 0)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp30E,e2);else{
return r;}}_LL1C5:;_LL1C6:
 return r;_LL1C2:;}
# 1479
static union Cyc_Pratt_Node Cyc_CfFlowInfo_rop2node(union Cyc_CfFlowInfo_RelnOp r){
union Cyc_CfFlowInfo_RelnOp _tmp30F=r;struct Cyc_Absyn_Vardecl*_tmp310;struct Cyc_Absyn_Vardecl*_tmp311;struct Cyc_Absyn_Tvar*_tmp312;_LL1C8: if((_tmp30F.RVar).tag != 2)goto _LL1CA;_tmp310=(struct Cyc_Absyn_Vardecl*)(_tmp30F.RVar).val;_LL1C9:
 return Cyc_Pratt_NVar(_tmp310);_LL1CA: if((_tmp30F.RNumelts).tag != 3)goto _LL1CC;_tmp311=(struct Cyc_Absyn_Vardecl*)(_tmp30F.RNumelts).val;_LL1CB:
 return Cyc_Pratt_NNumelts(_tmp311);_LL1CC: if((_tmp30F.RTvar).tag != 4)goto _LL1CE;_tmp312=(struct Cyc_Absyn_Tvar*)(_tmp30F.RTvar).val;_LL1CD:
 return Cyc_Pratt_NTvar(_tmp312);_LL1CE:;_LL1CF: {
const char*_tmp5B6;void*_tmp5B5;(_tmp5B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B6="rop2node",_tag_dyneither(_tmp5B6,sizeof(char),9))),_tag_dyneither(_tmp5B5,sizeof(void*),0)));}_LL1C7:;}
# 1488
static struct _dyneither_ptr Cyc_CfFlowInfo_relation2string(enum Cyc_CfFlowInfo_Relation r){
switch(r){case Cyc_CfFlowInfo_Req: _LL1D0: {
const char*_tmp5B7;return(_tmp5B7="==",_tag_dyneither(_tmp5B7,sizeof(char),3));}case Cyc_CfFlowInfo_Rneq: _LL1D1: {
const char*_tmp5B8;return(_tmp5B8="!=",_tag_dyneither(_tmp5B8,sizeof(char),3));}case Cyc_CfFlowInfo_Rlt: _LL1D2: {
const char*_tmp5B9;return(_tmp5B9="<",_tag_dyneither(_tmp5B9,sizeof(char),2));}case Cyc_CfFlowInfo_Rlte: _LL1D3: {
const char*_tmp5BA;return(_tmp5BA="<=",_tag_dyneither(_tmp5BA,sizeof(char),3));}}}
# 1497
static struct _dyneither_ptr Cyc_CfFlowInfo_rop2string(union Cyc_CfFlowInfo_RelnOp r){
union Cyc_CfFlowInfo_RelnOp _tmp319=r;unsigned int _tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;struct Cyc_Absyn_Tvar*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31D;_LL1D6: if((_tmp319.RConst).tag != 1)goto _LL1D8;_tmp31A=(unsigned int)(_tmp319.RConst).val;_LL1D7: {
const char*_tmp5BE;void*_tmp5BD[1];struct Cyc_Int_pa_PrintArg_struct _tmp5BC;return(struct _dyneither_ptr)((_tmp5BC.tag=1,((_tmp5BC.f1=(unsigned long)((int)_tmp31A),((_tmp5BD[0]=& _tmp5BC,Cyc_aprintf(((_tmp5BE="%d",_tag_dyneither(_tmp5BE,sizeof(char),3))),_tag_dyneither(_tmp5BD,sizeof(void*),1))))))));}_LL1D8: if((_tmp319.RVar).tag != 2)goto _LL1DA;_tmp31B=(struct Cyc_Absyn_Vardecl*)(_tmp319.RVar).val;_LL1D9: {
const char*_tmp5C2;void*_tmp5C1[1];struct Cyc_String_pa_PrintArg_struct _tmp5C0;return(struct _dyneither_ptr)((_tmp5C0.tag=0,((_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31B->name)),((_tmp5C1[0]=& _tmp5C0,Cyc_aprintf(((_tmp5C2="%s",_tag_dyneither(_tmp5C2,sizeof(char),3))),_tag_dyneither(_tmp5C1,sizeof(void*),1))))))));}_LL1DA: if((_tmp319.RTvar).tag != 4)goto _LL1DC;_tmp31C=(struct Cyc_Absyn_Tvar*)(_tmp319.RTvar).val;_LL1DB:
 return Cyc_Absynpp_tvar2string(_tmp31C);_LL1DC: if((_tmp319.RNumelts).tag != 3)goto _LL1D5;_tmp31D=(struct Cyc_Absyn_Vardecl*)(_tmp319.RNumelts).val;_LL1DD: {
const char*_tmp5C6;void*_tmp5C5[1];struct Cyc_String_pa_PrintArg_struct _tmp5C4;return(struct _dyneither_ptr)((_tmp5C4.tag=0,((_tmp5C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31D->name)),((_tmp5C5[0]=& _tmp5C4,Cyc_aprintf(((_tmp5C6="numelts(%s)",_tag_dyneither(_tmp5C6,sizeof(char),12))),_tag_dyneither(_tmp5C5,sizeof(void*),1))))))));}_LL1D5:;}
# 1506
static struct _dyneither_ptr Cyc_CfFlowInfo_reln2string(struct Cyc_CfFlowInfo_Reln*r){
const char*_tmp5CC;void*_tmp5CB[3];struct Cyc_String_pa_PrintArg_struct _tmp5CA;struct Cyc_String_pa_PrintArg_struct _tmp5C9;struct Cyc_String_pa_PrintArg_struct _tmp5C8;return(struct _dyneither_ptr)((_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_CfFlowInfo_rop2string(r->rop2)),((_tmp5C9.tag=0,((_tmp5C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1507
Cyc_CfFlowInfo_relation2string(r->relation)),((_tmp5CA.tag=0,((_tmp5CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_rop2string(r->rop1)),((_tmp5CB[0]=& _tmp5CA,((_tmp5CB[1]=& _tmp5C9,((_tmp5CB[2]=& _tmp5C8,Cyc_aprintf(((_tmp5CC="%s %s %s",_tag_dyneither(_tmp5CC,sizeof(char),9))),_tag_dyneither(_tmp5CB,sizeof(void*),3))))))))))))))))))));}
# 1511
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
{const char*_tmp5D0;void*_tmp5CF[1];struct Cyc_String_pa_PrintArg_struct _tmp5CE;(_tmp5CE.tag=0,((_tmp5CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_reln2string((struct Cyc_CfFlowInfo_Reln*)r->hd)),((_tmp5CF[0]=& _tmp5CE,Cyc_fprintf(Cyc_stderr,((_tmp5D0="%s",_tag_dyneither(_tmp5D0,sizeof(char),3))),_tag_dyneither(_tmp5CF,sizeof(void*),1)))))));}
if(r->tl != 0){const char*_tmp5D3;void*_tmp5D2;(_tmp5D2=0,Cyc_fprintf(Cyc_stderr,((_tmp5D3=",",_tag_dyneither(_tmp5D3,sizeof(char),2))),_tag_dyneither(_tmp5D2,sizeof(void*),0)));}}}
# 1523
int Cyc_CfFlowInfo_consistent_relations(struct Cyc_List_List*rlns){
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
for(0;(unsigned int)G  && rlns != 0;rlns=rlns->tl){
# 1529
struct Cyc_CfFlowInfo_Reln*_tmp331=(struct Cyc_CfFlowInfo_Reln*)rlns->hd;unsigned int _tmp332;enum Cyc_CfFlowInfo_Relation _tmp333;unsigned int _tmp334;union Cyc_CfFlowInfo_RelnOp _tmp335;enum Cyc_CfFlowInfo_Relation _tmp336;unsigned int _tmp337;unsigned int _tmp338;enum Cyc_CfFlowInfo_Relation _tmp339;union Cyc_CfFlowInfo_RelnOp _tmp33A;union Cyc_CfFlowInfo_RelnOp _tmp33B;enum Cyc_CfFlowInfo_Relation _tmp33C;union Cyc_CfFlowInfo_RelnOp _tmp33D;_LL1DF: if(((_tmp331->rop1).RConst).tag != 1)goto _LL1E1;_tmp332=(unsigned int)((_tmp331->rop1).RConst).val;_tmp333=_tmp331->relation;if(((_tmp331->rop2).RConst).tag != 1)goto _LL1E1;_tmp334=(unsigned int)((_tmp331->rop2).RConst).val;_LL1E0:
# 1531
 switch(_tmp333){case Cyc_CfFlowInfo_Req: _LL1E9:
 if(_tmp332 != _tmp334)return 0;break;case Cyc_CfFlowInfo_Rneq: _LL1EA:
 if(_tmp332 == _tmp334)return 0;break;case Cyc_CfFlowInfo_Rlt: _LL1EB:
 if(_tmp332 >= _tmp334)return 0;break;case Cyc_CfFlowInfo_Rlte: _LL1EC:
 if(_tmp332 > _tmp334)return 0;break;}
# 1537
goto _LL1DE;_LL1E1: if(_tmp331->relation != Cyc_CfFlowInfo_Rneq)goto _LL1E3;_LL1E2:
# 1539
 goto _LL1DE;_LL1E3: _tmp335=_tmp331->rop1;_tmp336=_tmp331->relation;if(((_tmp331->rop2).RConst).tag != 1)goto _LL1E5;_tmp337=(unsigned int)((_tmp331->rop2).RConst).val;_LL1E4: {
# 1543
union Cyc_Pratt_Node _tmp33E=Cyc_CfFlowInfo_rop2node(_tmp335);
if(_tmp336 == Cyc_CfFlowInfo_Rlt)_tmp337=_tmp337 - 1;
G=Cyc_Pratt_add_edge(G,_tmp33E,Cyc_Pratt_zero_node,(int)_tmp337);
# 1547
if((unsigned int)G  && _tmp336 == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp33E,(int)(- _tmp337));
goto _LL1DE;}_LL1E5: if(((_tmp331->rop1).RConst).tag != 1)goto _LL1E7;_tmp338=(unsigned int)((_tmp331->rop1).RConst).val;_tmp339=_tmp331->relation;_tmp33A=_tmp331->rop2;_LL1E6: {
# 1553
union Cyc_Pratt_Node _tmp33F=Cyc_CfFlowInfo_rop2node(_tmp33A);
if(_tmp339 == Cyc_CfFlowInfo_Rlt)_tmp338=_tmp338 + 1;
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp33F,(int)(- _tmp338));
# 1557
if((unsigned int)G  && _tmp339 == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,_tmp33F,Cyc_Pratt_zero_node,(int)_tmp338);
goto _LL1DE;}_LL1E7: _tmp33B=_tmp331->rop1;_tmp33C=_tmp331->relation;_tmp33D=_tmp331->rop2;_LL1E8: {
# 1562
union Cyc_Pratt_Node _tmp340=Cyc_CfFlowInfo_rop2node(_tmp33B);
union Cyc_Pratt_Node _tmp341=Cyc_CfFlowInfo_rop2node(_tmp33D);
int i=_tmp33C == Cyc_CfFlowInfo_Rlt?- 1: 0;
# 1567
G=Cyc_Pratt_add_edge(G,_tmp340,_tmp341,i);
# 1569
if((unsigned int)G  && _tmp33C == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,_tmp341,_tmp340,i);
goto _LL1DE;}_LL1DE:;}
# 1574
if(G != 0)return 1;else{return 0;}}
# 1577
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1EE:
{const char*_tmp5D6;void*_tmp5D5;(_tmp5D5=0,Cyc_fprintf(Cyc_stderr,((_tmp5D6="uninitialized",_tag_dyneither(_tmp5D6,sizeof(char),14))),_tag_dyneither(_tmp5D5,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL1EF:
{const char*_tmp5D9;void*_tmp5D8;(_tmp5D8=0,Cyc_fprintf(Cyc_stderr,((_tmp5D9="this-initialized",_tag_dyneither(_tmp5D9,sizeof(char),17))),_tag_dyneither(_tmp5D8,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1F0:
{const char*_tmp5DC;void*_tmp5DB;(_tmp5DB=0,Cyc_fprintf(Cyc_stderr,((_tmp5DC="all-initialized",_tag_dyneither(_tmp5DC,sizeof(char),16))),_tag_dyneither(_tmp5DB,sizeof(void*),0)));}break;}}
# 1585
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp348=root;struct Cyc_Absyn_Vardecl*_tmp34A;struct Cyc_Absyn_Exp*_tmp34C;void*_tmp34D;_LL1F3: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp349=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp348;if(_tmp349->tag != 0)goto _LL1F5;else{_tmp34A=_tmp349->f1;}}_LL1F4:
# 1588
{const char*_tmp5E0;void*_tmp5DF[1];struct Cyc_String_pa_PrintArg_struct _tmp5DE;(_tmp5DE.tag=0,((_tmp5DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp34A->name)),((_tmp5DF[0]=& _tmp5DE,Cyc_fprintf(Cyc_stderr,((_tmp5E0="Root(%s)",_tag_dyneither(_tmp5E0,sizeof(char),9))),_tag_dyneither(_tmp5DF,sizeof(void*),1)))))));}goto _LL1F2;_LL1F5: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp34B=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp348;if(_tmp34B->tag != 1)goto _LL1F7;else{_tmp34C=_tmp34B->f1;_tmp34D=(void*)_tmp34B->f2;}}_LL1F6:
# 1590
{const char*_tmp5E5;void*_tmp5E4[2];struct Cyc_String_pa_PrintArg_struct _tmp5E3;struct Cyc_String_pa_PrintArg_struct _tmp5E2;(_tmp5E2.tag=0,((_tmp5E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp34D)),((_tmp5E3.tag=0,((_tmp5E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp34C)),((_tmp5E4[0]=& _tmp5E3,((_tmp5E4[1]=& _tmp5E2,Cyc_fprintf(Cyc_stderr,((_tmp5E5="MallocPt(%s,%s)",_tag_dyneither(_tmp5E5,sizeof(char),16))),_tag_dyneither(_tmp5E4,sizeof(void*),2)))))))))))));}goto _LL1F2;_LL1F7: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp34E=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp348;if(_tmp34E->tag != 2)goto _LL1F2;}_LL1F8:
# 1593
{const char*_tmp5E8;void*_tmp5E7;(_tmp5E7=0,Cyc_fprintf(Cyc_stderr,((_tmp5E8="InitParam(_,_)",_tag_dyneither(_tmp5E8,sizeof(char),15))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}goto _LL1F2;_LL1F2:;}
# 1597
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp358=p->fields;
while(_tmp358 != 0){
{const char*_tmp5EC;void*_tmp5EB[1];struct Cyc_Int_pa_PrintArg_struct _tmp5EA;(_tmp5EA.tag=1,((_tmp5EA.f1=(unsigned long)((int)_tmp358->hd),((_tmp5EB[0]=& _tmp5EA,Cyc_fprintf(Cyc_stderr,((_tmp5EC=".%d",_tag_dyneither(_tmp5EC,sizeof(char),4))),_tag_dyneither(_tmp5EB,sizeof(void*),1)))))));}
_tmp358=_tmp358->tl;}{
# 1604
const char*_tmp5EF;void*_tmp5EE;(_tmp5EE=0,Cyc_fprintf(Cyc_stderr,((_tmp5EF=" ",_tag_dyneither(_tmp5EF,sizeof(char),2))),_tag_dyneither(_tmp5EE,sizeof(void*),0)));};};}
# 1607
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp35E=1;
while(x != 0){
if(!_tmp35E){{const char*_tmp5F2;void*_tmp5F1;(_tmp5F1=0,Cyc_fprintf(Cyc_stderr,((_tmp5F2=", ",_tag_dyneither(_tmp5F2,sizeof(char),3))),_tag_dyneither(_tmp5F1,sizeof(void*),0)));}_tmp35E=0;}
pr(x->hd);
x=x->tl;}{
# 1614
const char*_tmp5F5;void*_tmp5F4;(_tmp5F4=0,Cyc_fprintf(Cyc_stderr,((_tmp5F5="\n",_tag_dyneither(_tmp5F5,sizeof(char),2))),_tag_dyneither(_tmp5F4,sizeof(void*),0)));};}
# 1617
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp363=_new_region("r");struct _RegionHandle*r=& _tmp363;_push_region(r);
{struct _handler_cons _tmp364;_push_handler(& _tmp364);{int _tmp366=0;if(setjmp(_tmp364.handler))_tmp366=1;if(!_tmp366){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple13 _tmp367=*((struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp5F8;void*_tmp5F7;(_tmp5F7=0,Cyc_fprintf(Cyc_stderr,((_tmp5F8="{ ",_tag_dyneither(_tmp5F8,sizeof(char),3))),_tag_dyneither(_tmp5F7,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(iter,& _tmp367)){
void*_tmp36A=_tmp367.f1;
if(!first){
{const char*_tmp5FB;void*_tmp5FA;(_tmp5FA=0,Cyc_fprintf(Cyc_stderr,((_tmp5FB=", ",_tag_dyneither(_tmp5FB,sizeof(char),3))),_tag_dyneither(_tmp5FA,sizeof(void*),0)));}
first=0;}
# 1630
pr(_tmp36A);}{
# 1632
const char*_tmp5FE;void*_tmp5FD;(_tmp5FD=0,Cyc_fprintf(Cyc_stderr,((_tmp5FE="}\n",_tag_dyneither(_tmp5FE,sizeof(char),3))),_tag_dyneither(_tmp5FD,sizeof(void*),0)));};}
# 1620
;_pop_handler();}else{void*_tmp365=(void*)_exn_thrown;void*_tmp370=_tmp365;void*_tmp371;void*_tmp372;_LL1FA: _tmp371=_tmp370;_LL1FB:
# 1635
{const char*_tmp601;void*_tmp600;(_tmp600=0,Cyc_fprintf(Cyc_stderr,((_tmp601="{ }\n",_tag_dyneither(_tmp601,sizeof(char),5))),_tag_dyneither(_tmp600,sizeof(void*),0)));}
_npop_handler(0);return;_LL1FC: _tmp372=_tmp370;_LL1FD:(void)_throw(_tmp372);_LL1F9:;}};}
# 1619
;_pop_region(r);}
# 1641
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp375=rval;enum Cyc_CfFlowInfo_InitLevel _tmp37A;enum Cyc_CfFlowInfo_InitLevel _tmp37C;struct Cyc_CfFlowInfo_Place*_tmp37E;int _tmp380;int _tmp381;struct _dyneither_ptr _tmp382;struct Cyc_Absyn_Exp*_tmp384;int _tmp385;void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp388;void*_tmp389;_LL1FF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp376=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp376->tag != 0)goto _LL201;}_LL200:
{const char*_tmp604;void*_tmp603;(_tmp603=0,Cyc_fprintf(Cyc_stderr,((_tmp604="Zero",_tag_dyneither(_tmp604,sizeof(char),5))),_tag_dyneither(_tmp603,sizeof(void*),0)));}goto _LL1FE;_LL201: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp377=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp377->tag != 1)goto _LL203;}_LL202:
{const char*_tmp607;void*_tmp606;(_tmp606=0,Cyc_fprintf(Cyc_stderr,((_tmp607="NotZeroAll",_tag_dyneither(_tmp607,sizeof(char),11))),_tag_dyneither(_tmp606,sizeof(void*),0)));}goto _LL1FE;_LL203: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp378=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp378->tag != 2)goto _LL205;}_LL204:
{const char*_tmp60A;void*_tmp609;(_tmp609=0,Cyc_fprintf(Cyc_stderr,((_tmp60A="NotZeroThis",_tag_dyneither(_tmp60A,sizeof(char),12))),_tag_dyneither(_tmp609,sizeof(void*),0)));}goto _LL1FE;_LL205: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp379=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp379->tag != 3)goto _LL207;else{_tmp37A=_tmp379->f1;}}_LL206:
{const char*_tmp60D;void*_tmp60C;(_tmp60C=0,Cyc_fprintf(Cyc_stderr,((_tmp60D="Unknown(",_tag_dyneither(_tmp60D,sizeof(char),9))),_tag_dyneither(_tmp60C,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp37A);
{const char*_tmp610;void*_tmp60F;(_tmp60F=0,Cyc_fprintf(Cyc_stderr,((_tmp610=")",_tag_dyneither(_tmp610,sizeof(char),2))),_tag_dyneither(_tmp60F,sizeof(void*),0)));}goto _LL1FE;_LL207: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp37B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp37B->tag != 4)goto _LL209;else{_tmp37C=_tmp37B->f1;}}_LL208:
{const char*_tmp613;void*_tmp612;(_tmp612=0,Cyc_fprintf(Cyc_stderr,((_tmp613="Esc(",_tag_dyneither(_tmp613,sizeof(char),5))),_tag_dyneither(_tmp612,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp37C);
{const char*_tmp616;void*_tmp615;(_tmp615=0,Cyc_fprintf(Cyc_stderr,((_tmp616=")",_tag_dyneither(_tmp616,sizeof(char),2))),_tag_dyneither(_tmp615,sizeof(void*),0)));}goto _LL1FE;_LL209: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp37D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp37D->tag != 5)goto _LL20B;else{_tmp37E=_tmp37D->f1;}}_LL20A:
{const char*_tmp619;void*_tmp618;(_tmp618=0,Cyc_fprintf(Cyc_stderr,((_tmp619="AddrOf(",_tag_dyneither(_tmp619,sizeof(char),8))),_tag_dyneither(_tmp618,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp37E);
{const char*_tmp61C;void*_tmp61B;(_tmp61B=0,Cyc_fprintf(Cyc_stderr,((_tmp61C=")",_tag_dyneither(_tmp61C,sizeof(char),2))),_tag_dyneither(_tmp61B,sizeof(void*),0)));}goto _LL1FE;_LL20B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp37F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp37F->tag != 6)goto _LL20D;else{_tmp380=(_tmp37F->f1).is_union;_tmp381=(_tmp37F->f1).fieldnum;_tmp382=_tmp37F->f2;}}_LL20C:
# 1653
 if(_tmp380){
{const char*_tmp61F;void*_tmp61E;(_tmp61E=0,Cyc_fprintf(Cyc_stderr,((_tmp61F="AggrUnion{",_tag_dyneither(_tmp61F,sizeof(char),11))),_tag_dyneither(_tmp61E,sizeof(void*),0)));}
if(_tmp381 != - 1){
const char*_tmp623;void*_tmp622[1];struct Cyc_Int_pa_PrintArg_struct _tmp621;(_tmp621.tag=1,((_tmp621.f1=(unsigned long)_tmp381,((_tmp622[0]=& _tmp621,Cyc_fprintf(Cyc_stderr,((_tmp623="tag == %d;",_tag_dyneither(_tmp623,sizeof(char),11))),_tag_dyneither(_tmp622,sizeof(void*),1)))))));}}else{
# 1658
const char*_tmp626;void*_tmp625;(_tmp625=0,Cyc_fprintf(Cyc_stderr,((_tmp626="AggrStruct{",_tag_dyneither(_tmp626,sizeof(char),12))),_tag_dyneither(_tmp625,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp382,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp382.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp382,sizeof(void*))){const char*_tmp629;void*_tmp628;(_tmp628=0,Cyc_fprintf(Cyc_stderr,((_tmp629=",",_tag_dyneither(_tmp629,sizeof(char),2))),_tag_dyneither(_tmp628,sizeof(void*),0)));}}}
# 1663
{const char*_tmp62C;void*_tmp62B;(_tmp62B=0,Cyc_fprintf(Cyc_stderr,((_tmp62C="}",_tag_dyneither(_tmp62C,sizeof(char),2))),_tag_dyneither(_tmp62B,sizeof(void*),0)));}
goto _LL1FE;_LL20D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp383=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp383->tag != 7)goto _LL20F;else{_tmp384=_tmp383->f1;_tmp385=_tmp383->f2;_tmp386=(void*)_tmp383->f3;}}_LL20E:
# 1666
{const char*_tmp631;void*_tmp630[2];struct Cyc_String_pa_PrintArg_struct _tmp62F;struct Cyc_Int_pa_PrintArg_struct _tmp62E;(_tmp62E.tag=1,((_tmp62E.f1=(unsigned long)_tmp385,((_tmp62F.tag=0,((_tmp62F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp384)),((_tmp630[0]=& _tmp62F,((_tmp630[1]=& _tmp62E,Cyc_fprintf(Cyc_stderr,((_tmp631="[Consumed(%s,%d,",_tag_dyneither(_tmp631,sizeof(char),17))),_tag_dyneither(_tmp630,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp386);{const char*_tmp634;void*_tmp633;(_tmp633=0,Cyc_fprintf(Cyc_stderr,((_tmp634="]",_tag_dyneither(_tmp634,sizeof(char),2))),_tag_dyneither(_tmp633,sizeof(void*),0)));}
goto _LL1FE;_LL20F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp387=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp375;if(_tmp387->tag != 8)goto _LL1FE;else{_tmp388=_tmp387->f1;_tmp389=(void*)_tmp387->f2;}}_LL210:
# 1670
{const char*_tmp638;void*_tmp637[1];struct Cyc_String_pa_PrintArg_struct _tmp636;(_tmp636.tag=0,((_tmp636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp388->name)),((_tmp637[0]=& _tmp636,Cyc_fprintf(Cyc_stderr,((_tmp638="[NamedLocation(%s,",_tag_dyneither(_tmp638,sizeof(char),19))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp389);{const char*_tmp63B;void*_tmp63A;(_tmp63A=0,Cyc_fprintf(Cyc_stderr,((_tmp63B="]",_tag_dyneither(_tmp63B,sizeof(char),2))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}
goto _LL1FE;_LL1FE:;}
# 1676
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp63E;void*_tmp63D;(_tmp63D=0,Cyc_fprintf(Cyc_stderr,((_tmp63E="    ",_tag_dyneither(_tmp63E,sizeof(char),5))),_tag_dyneither(_tmp63D,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp641;void*_tmp640;(_tmp640=0,Cyc_fprintf(Cyc_stderr,((_tmp641=" --> ",_tag_dyneither(_tmp641,sizeof(char),6))),_tag_dyneither(_tmp640,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp644;void*_tmp643;(_tmp643=0,Cyc_fprintf(Cyc_stderr,((_tmp644="\n",_tag_dyneither(_tmp644,sizeof(char),2))),_tag_dyneither(_tmp643,sizeof(void*),0)));};}
# 1684
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1688
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp3B8=f;struct Cyc_Dict_Dict _tmp3B9;_LL212: if((_tmp3B8.BottomFL).tag != 1)goto _LL214;_LL213:
{const char*_tmp647;void*_tmp646;(_tmp646=0,Cyc_fprintf(Cyc_stderr,((_tmp647="  BottomFL\n",_tag_dyneither(_tmp647,sizeof(char),12))),_tag_dyneither(_tmp646,sizeof(void*),0)));}goto _LL211;_LL214: if((_tmp3B8.ReachableFL).tag != 2)goto _LL211;_tmp3B9=((struct _tuple11)(_tmp3B8.ReachableFL).val).f1;_LL215:
# 1692
{const char*_tmp64A;void*_tmp649;(_tmp649=0,Cyc_fprintf(Cyc_stderr,((_tmp64A="  ReachableFL:\n",_tag_dyneither(_tmp64A,sizeof(char),16))),_tag_dyneither(_tmp649,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp3B9);
{const char*_tmp64D;void*_tmp64C;(_tmp64C=0,Cyc_fprintf(Cyc_stderr,((_tmp64D="\n",_tag_dyneither(_tmp64D,sizeof(char),2))),_tag_dyneither(_tmp64C,sizeof(void*),0)));}
goto _LL211;_LL211:;}
# 1707 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp3C0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3CD;struct Cyc_List_List*_tmp3CF;struct Cyc_List_List*_tmp3D1;struct Cyc_List_List*_tmp3D3;struct Cyc_List_List*_tmp3D5;struct Cyc_List_List*_tmp3D7;void*_tmp3D9;void*_tmp3DA;struct Cyc_List_List*_tmp3DD;struct Cyc_List_List*_tmp3DF;void*_tmp3E1;void*_tmp3E3;void*_tmp3E5;void*_tmp3E7;void*_tmp3E8;void*_tmp3EA;_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C1->tag != 26)goto _LL219;}_LL218:
 goto _LL21A;_LL219: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C2->tag != 0)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C3->tag != 6)goto _LL21D;}_LL21C:
 goto _LL21E;_LL21D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C4->tag != 7)goto _LL21F;}_LL21E:
 goto _LL220;_LL21F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C5->tag != 13)goto _LL221;}_LL220:
 goto _LL222;_LL221: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3C6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C6->tag != 14)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3C0;if(_tmp3C7->tag != 19)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3C8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3C0;if(_tmp3C8->tag != 20)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3C0;if(_tmp3C9->tag != 1)goto _LL229;}_LL228:
 return 0;_LL229: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3C0;if(_tmp3CA->tag != 21)goto _LL22B;}_LL22A: {
# 1719
void*_tmp3EC=rgn;_LL24E: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3ED=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EC;if(_tmp3ED->tag != 0)goto _LL250;}_LL24F:
 return 1;_LL250:;_LL251:
 return 0;_LL24D:;}_LL22B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3CB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3C0;if(_tmp3CB->tag != 22)goto _LL22D;}_LL22C: {
# 1724
void*_tmp3EE=rgn;_LL253: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3EF=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EE;if(_tmp3EF->tag != 1)goto _LL255;}_LL254:
 return 1;_LL255:;_LL256:
 return 0;_LL252:;}_LL22D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3CC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C0;if(_tmp3CC->tag != 2)goto _LL22F;else{_tmp3CD=_tmp3CC->f1;}}_LL22E: {
# 1729
void*_tmp3F0=rgn;struct Cyc_Absyn_Tvar*_tmp3F4;_LL258: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3F1=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3F0;if(_tmp3F1->tag != 1)goto _LL25A;}_LL259:
 goto _LL25B;_LL25A: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3F2=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3F0;if(_tmp3F2->tag != 0)goto _LL25C;}_LL25B:
 return 0;_LL25C: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp3F3=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp3F0;if(_tmp3F3->tag != 2)goto _LL257;else{_tmp3F4=_tmp3F3->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(_tmp3CD,_tmp3F4)== 0;_LL257:;}_LL22F: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3CE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3C0;if(_tmp3CE->tag != 3)goto _LL231;else{_tmp3CF=(_tmp3CE->f1).targs;}}_LL230:
# 1742 "cf_flowinfo.cyc"
 _tmp3D1=_tmp3CF;goto _LL232;_LL231: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3C0;if(_tmp3D0->tag != 17)goto _LL233;else{_tmp3D1=_tmp3D0->f2;}}_LL232:
 _tmp3D3=_tmp3D1;goto _LL234;_LL233: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C0;if(_tmp3D2->tag != 11)goto _LL235;else{_tmp3D3=(_tmp3D2->f1).targs;}}_LL234:
 _tmp3D5=_tmp3D3;goto _LL236;_LL235: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C0;if(_tmp3D4->tag != 4)goto _LL237;else{_tmp3D5=(_tmp3D4->f1).targs;}}_LL236:
 _tmp3D7=_tmp3D5;goto _LL238;_LL237: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3C0;if(_tmp3D6->tag != 24)goto _LL239;else{_tmp3D7=_tmp3D6->f1;}}_LL238:
# 1747
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp3D7);_LL239: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0;if(_tmp3D8->tag != 5)goto _LL23B;else{_tmp3D9=(_tmp3D8->f1).elt_typ;_tmp3DA=((_tmp3D8->f1).ptr_atts).rgn;}}_LL23A:
# 1749
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3DA) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3D9);_LL23B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3DB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3C0;if(_tmp3DB->tag != 9)goto _LL23D;}_LL23C:
 return 0;_LL23D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3DC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C0;if(_tmp3DC->tag != 10)goto _LL23F;else{_tmp3DD=_tmp3DC->f1;}}_LL23E:
# 1752
 for(0;_tmp3DD != 0;_tmp3DD=_tmp3DD->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple16*)_tmp3DD->hd)).f2))return 1;}
return 0;_LL23F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C0;if(_tmp3DE->tag != 12)goto _LL241;else{_tmp3DF=_tmp3DE->f2;}}_LL240:
# 1756
 for(0;_tmp3DF != 0;_tmp3DF=_tmp3DF->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp3DF->hd)->type))return 1;}
return 0;_LL241: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C0;if(_tmp3E0->tag != 8)goto _LL243;else{_tmp3E1=(_tmp3E0->f1).elt_type;}}_LL242:
 _tmp3E3=_tmp3E1;goto _LL244;_LL243: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C0;if(_tmp3E2->tag != 23)goto _LL245;else{_tmp3E3=(void*)_tmp3E2->f1;}}_LL244:
 _tmp3E5=_tmp3E3;goto _LL246;_LL245: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C0;if(_tmp3E4->tag != 25)goto _LL247;else{_tmp3E5=(void*)_tmp3E4->f1;}}_LL246:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3E5);_LL247: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3C0;if(_tmp3E6->tag != 16)goto _LL249;else{_tmp3E7=(void*)_tmp3E6->f1;_tmp3E8=(void*)_tmp3E6->f2;}}_LL248:
# 1763
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3E7) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3E8);_LL249: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3C0;if(_tmp3E9->tag != 15)goto _LL24B;else{_tmp3EA=(void*)_tmp3E9->f1;}}_LL24A:
# 1766
 return 0;_LL24B: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3C0;if(_tmp3EB->tag != 18)goto _LL216;}_LL24C:
 return 0;_LL216:;}struct _tuple24{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1772
static void Cyc_CfFlowInfo_kill_root(struct _tuple24*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F6;struct Cyc_Dict_Dict*_tmp3F7;void*_tmp3F8;struct _tuple24*_tmp3F5=env;_tmp3F6=_tmp3F5->f1;_tmp3F7=(struct Cyc_Dict_Dict*)& _tmp3F5->f2;_tmp3F8=_tmp3F5->f3;{
void*_tmp3F9=root;struct Cyc_Absyn_Vardecl*_tmp3FB;void*_tmp3FD;_LL25F: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3FA=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3F9;if(_tmp3FA->tag != 0)goto _LL261;else{_tmp3FB=_tmp3FA->f1;}}_LL260:
# 1777
 if(Cyc_CfFlowInfo_contains_region(_tmp3F8,_tmp3FB->type))
# 1779
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3F6,_tmp3FB->type,0,_tmp3F6->unknown_none);
*_tmp3F7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3F7,root,rval);
goto _LL25E;_LL261: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3FC=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp3F9;if(_tmp3FC->tag != 1)goto _LL263;else{_tmp3FD=(void*)_tmp3FC->f2;}}_LL262:
# 1784
 if(!Cyc_CfFlowInfo_contains_region(_tmp3F8,_tmp3FD))
# 1786
*_tmp3F7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3F7,root,rval);
goto _LL25E;_LL263: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3FE=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp3F9;if(_tmp3FE->tag != 2)goto _LL25E;}_LL264:
# 1790
 goto _LL25E;_LL25E:;};}
# 1795
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1797
void*_tmp3FF=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp401;_LL266: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3FF;if(_tmp400->tag != 2)goto _LL268;else{_tmp401=_tmp400->f1;}}_LL267: {
# 1799
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp64E;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp64E.tag=2,((_tmp64E.f1=_tmp401,_tmp64E)));
void*v2=(void*)& v;
struct _tuple24 _tmp64F;struct _tuple24 env=(_tmp64F.f1=fenv,((_tmp64F.f2=fenv->mt_flowdict,((_tmp64F.f3=v2,_tmp64F)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL268: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3FF;if(_tmp402->tag != 21)goto _LL26A;}_LL269: {
# 1806
struct _tuple24 _tmp650;struct _tuple24 env=(_tmp650.f1=fenv,((_tmp650.f2=fenv->mt_flowdict,((_tmp650.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp650)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL26A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3FF;if(_tmp403->tag != 22)goto _LL26C;}_LL26B: {
# 1811
struct _tuple24 _tmp651;struct _tuple24 env=(_tmp651.f1=fenv,((_tmp651.f2=fenv->mt_flowdict,((_tmp651.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp651)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL26C:;_LL26D: {
# 1815
struct Cyc_Core_Impossible_exn_struct _tmp657;const char*_tmp656;struct Cyc_Core_Impossible_exn_struct*_tmp655;(int)_throw((void*)((_tmp655=_cycalloc(sizeof(*_tmp655)),((_tmp655[0]=((_tmp657.tag=Cyc_Core_Impossible,((_tmp657.f1=((_tmp656="kill_flowdict_region",_tag_dyneither(_tmp656,sizeof(char),21))),_tmp657)))),_tmp655)))));}_LL265:;}
# 1820
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1822
return 0;}
# 1827
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp40B=f;struct Cyc_Dict_Dict _tmp40C;struct Cyc_List_List*_tmp40D;_LL26F: if((_tmp40B.BottomFL).tag != 1)goto _LL271;_LL270:
 return f;_LL271: if((_tmp40B.ReachableFL).tag != 2)goto _LL26E;_tmp40C=((struct _tuple11)(_tmp40B.ReachableFL).val).f1;_tmp40D=((struct _tuple11)(_tmp40B.ReachableFL).val).f2;_LL272: {
# 1831
struct Cyc_Dict_Dict _tmp40E=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp40C,rgn);
struct Cyc_List_List*_tmp40F=Cyc_CfFlowInfo_kill_relns_region(_tmp40D,rgn);
# 1834
return Cyc_CfFlowInfo_ReachableFL(_tmp40E,_tmp40F);}_LL26E:;}
