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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
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
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
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
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 95
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 102
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 109
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
# 110
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
# 114
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
# 115
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 137
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 139
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 140
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
# 143
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
# 159
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
# 160
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 178
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
# 181
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 183
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 184
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 186
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
# 187
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
# 188
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
# 189
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
# 190
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 192
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
# 193
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 194
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 195
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 197
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 198
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
# 199
void Cyc_CfFlowInfo_print_root(void*root);
# 200
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
# 201
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
# 202
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
# 203
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
# 204
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 217 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 218
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 219
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 220
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 221
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 222
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 223
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
# 227
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 231
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 236
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
# 237
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 242
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 254
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
# 255
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
# 256
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 258
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 260
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 45 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
# 46
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
# 47
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 49
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
# 50
union Cyc_CfFlowInfo_AbsLVal _tmp4A5;return((_tmp4A5.PlaceL).val=x,(((_tmp4A5.PlaceL).tag=1,_tmp4A5)));}
# 52
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
# 53
union Cyc_CfFlowInfo_AbsLVal _tmp4A6;return((_tmp4A6.UnknownL).val=0,(((_tmp4A6.UnknownL).tag=2,_tmp4A6)));}
# 55
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp4A7;return((_tmp4A7.BottomFL).val=0,(((_tmp4A7.BottomFL).tag=1,_tmp4A7)));}
# 56
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
# 57
struct _tuple12 _tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4A9;return((_tmp4A9.ReachableFL).val=((_tmp4AA.f1=fd,((_tmp4AA.f2=r,_tmp4AA)))),(((_tmp4A9.ReachableFL).tag=2,_tmp4A9)));}
# 61
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
# 62
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
# 63
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
# 64
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
# 65
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 66
struct Cyc_CfFlowInfo_Place*_tmp4E1;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp4E0;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp4DF;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4DE;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4DD;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4DC;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4DB;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4DA;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp4D9;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D8;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D7;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D6;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D5;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D4;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp4D3;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4D2;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp4D1;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4D0;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp4CF;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CE;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp4CD;struct Cyc_CfFlowInfo_FlowEnv*_tmp4CC;return(_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC)),((_tmp4CC->r=r,((_tmp4CC->zero=(void*)(
# 68
(_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE)),((_tmp4CE[0]=((_tmp4CD.tag=0,_tmp4CD)),_tmp4CE)))),((_tmp4CC->notzeroall=(void*)(
# 69
(_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4CF.tag=1,_tmp4CF)),_tmp4D0)))),((_tmp4CC->notzerothis=(void*)(
# 70
(_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2)),((_tmp4D2[0]=((_tmp4D1.tag=2,_tmp4D1)),_tmp4D2)))),((_tmp4CC->unknown_none=(void*)(
# 71
(_tmp4D4=_region_malloc(r,sizeof(*_tmp4D4)),((_tmp4D4[0]=((_tmp4D3.tag=3,((_tmp4D3.f1=Cyc_CfFlowInfo_NoneIL,_tmp4D3)))),_tmp4D4)))),((_tmp4CC->unknown_this=(void*)(
# 72
(_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D5.tag=3,((_tmp4D5.f1=Cyc_CfFlowInfo_ThisIL,_tmp4D5)))),_tmp4D6)))),((_tmp4CC->unknown_all=(void*)(
# 73
(_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8)),((_tmp4D8[0]=((_tmp4D7.tag=3,((_tmp4D7.f1=Cyc_CfFlowInfo_AllIL,_tmp4D7)))),_tmp4D8)))),((_tmp4CC->esc_none=(void*)(
# 74
(_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4D9.tag=4,((_tmp4D9.f1=Cyc_CfFlowInfo_NoneIL,_tmp4D9)))),_tmp4DA)))),((_tmp4CC->esc_this=(void*)(
# 75
(_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC[0]=((_tmp4DB.tag=4,((_tmp4DB.f1=Cyc_CfFlowInfo_ThisIL,_tmp4DB)))),_tmp4DC)))),((_tmp4CC->esc_all=(void*)(
# 76
(_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE[0]=((_tmp4DD.tag=4,((_tmp4DD.f1=Cyc_CfFlowInfo_AllIL,_tmp4DD)))),_tmp4DE)))),((_tmp4CC->mt_flowdict=
# 77
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp4CC->mt_place_set=
# 78
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4CC->dummy_place=(
# 79
(_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1->root=(void*)((_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E0.tag=1,((_tmp4E0.f1=& dummy_exp,((_tmp4E0.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4E0)))))),_tmp4DF)))),((_tmp4E1->fields=0,_tmp4E1)))))),_tmp4CC)))))))))))))))))))))))))));}
# 85
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
# 86
if(set != 0){
# 88
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
# 89
return 1;else{
# 91
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 93
return 0;}
# 97
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
# 98
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{
# 99
void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2:
# 101
 if(_tmp21 == 0)
# 102
return Cyc_Absynpp_qvar2string(_tmp24->name);else{
# 104
const char*_tmp4E5;void*_tmp4E4[1];struct Cyc_String_pa_PrintArg_struct _tmp4E3;return(struct _dyneither_ptr)((_tmp4E3.tag=0,((_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp4E4[0]=& _tmp4E3,Cyc_aprintf(((_tmp4E5="reachable from %s",_tag_dyneither(_tmp4E5,sizeof(char),18))),_tag_dyneither(_tmp4E4,sizeof(void*),1))))))));}_LL3:;_LL4: {
# 105
const char*_tmp4E8;void*_tmp4E7;(_tmp4E7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E8="error locations not for VarRoots",_tag_dyneither(_tmp4E8,sizeof(char),33))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}_LL0:;};}
# 110
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 114
return loc1;}
# 119
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 126
int _tmp2A=0;
# 127
struct Cyc_Dict_Dict _tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
# 131
return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};
# 134
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
# 135
if((int)s1.t == (int)s2.t)return 1;{
# 136
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
# 137
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
# 138
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
# 139
struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
# 140
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){
# 141
void*_tmp31=_tmp30.f1;
# 142
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){
# 146
int _tmp32=0;_npop_handler(1);return _tmp32;}}{
# 149
int _tmp33=1;_npop_handler(1);return _tmp33;};}
# 138
;_pop_region(temp);}
# 137
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp35=_tmp2D;_LL6:;_LL7:
# 153
 return 1;_LL8:;_LL9:(void)_throw(_tmp35);_LL5:;}};};}
# 157
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
# 158
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 163
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
# 164
int n=0;
# 165
for(0;(unsigned int)fs;fs=fs->tl){
# 166
struct _dyneither_ptr*_tmp36=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
# 167
if(Cyc_strptrcmp(_tmp36,f)== 0)return n;
# 168
n=n + 1;}{
# 170
struct Cyc_Core_Impossible_exn_struct _tmp4F5;const char*_tmp4F4;void*_tmp4F3[1];struct Cyc_String_pa_PrintArg_struct _tmp4F2;struct Cyc_Core_Impossible_exn_struct*_tmp4F1;(int)_throw((void*)((_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F5.tag=Cyc_Core_Impossible,((_tmp4F5.f1=(struct _dyneither_ptr)((_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4F3[0]=& _tmp4F2,Cyc_aprintf(((_tmp4F4="get_field_index_fs failed: %s",_tag_dyneither(_tmp4F4,sizeof(char),30))),_tag_dyneither(_tmp4F3,sizeof(void*),1)))))))),_tmp4F5)))),_tmp4F1)))));};}
# 173
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
# 174
void*_tmp3C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3E;union Cyc_Absyn_AggrInfoU _tmp3F;struct Cyc_List_List*_tmp41;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C;if(_tmp3D->tag != 11)goto _LLD;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3E.aggr_info;}}_LLC: {
# 176
struct Cyc_List_List*_tmp42=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp3F))->impl))->fields;
# 177
_tmp41=_tmp42;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C;if(_tmp40->tag != 12)goto _LLF;else{_tmp41=_tmp40->f2;}}_LLE:
# 179
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp41,f);_LLF:;_LL10: {
# 181
struct Cyc_Core_Impossible_exn_struct _tmp502;const char*_tmp501;void*_tmp500[1];struct Cyc_String_pa_PrintArg_struct _tmp4FF;struct Cyc_Core_Impossible_exn_struct*_tmp4FE;(int)_throw((void*)((_tmp4FE=_cycalloc(sizeof(*_tmp4FE)),((_tmp4FE[0]=((_tmp502.tag=Cyc_Core_Impossible,((_tmp502.f1=(struct _dyneither_ptr)((_tmp4FF.tag=0,((_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp500[0]=& _tmp4FF,Cyc_aprintf(((_tmp501="get_field_index failed: %s",_tag_dyneither(_tmp501,sizeof(char),27))),_tag_dyneither(_tmp500,sizeof(void*),1)))))))),_tmp502)))),_tmp4FE)))));}_LLA:;}
# 186
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
# 187
if((int)r1 == (int)r2)
# 188
return 0;{
# 189
struct _tuple0 _tmp503;struct _tuple0 _tmp49=(_tmp503.f1=r1,((_tmp503.f2=r2,_tmp503)));void*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp4D;struct Cyc_Absyn_Vardecl*_tmp4F;void*_tmp50;void*_tmp52;void*_tmp54;struct Cyc_Absyn_Exp*_tmp56;void*_tmp57;struct Cyc_Absyn_Exp*_tmp59;void*_tmp5A;void*_tmp5C;void*_tmp5E;int _tmp60;void*_tmp61;int _tmp63;_LL12: _tmp4A=_tmp49.f1;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4A;if(_tmp4B->tag != 0)goto _LL14;else{_tmp4C=_tmp4B->f1;}};_tmp4D=_tmp49.f2;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4D;if(_tmp4E->tag != 0)goto _LL14;else{_tmp4F=_tmp4E->f1;}};_LL13:
# 190
 return(int)_tmp4C - (int)_tmp4F;_LL14: _tmp50=_tmp49.f1;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp51=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp50;if(_tmp51->tag != 0)goto _LL16;};_LL15:
# 191
 return - 1;_LL16: _tmp52=_tmp49.f2;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp53=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp52;if(_tmp53->tag != 0)goto _LL18;};_LL17:
# 192
 return 1;_LL18: _tmp54=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp54;if(_tmp55->tag != 1)goto _LL1A;else{_tmp56=_tmp55->f1;}};_tmp57=_tmp49.f2;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp58=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp57;if(_tmp58->tag != 1)goto _LL1A;else{_tmp59=_tmp58->f1;}};_LL19:
# 193
 return(int)_tmp56 - (int)_tmp59;_LL1A: _tmp5A=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp5B=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp5A;if(_tmp5B->tag != 1)goto _LL1C;};_LL1B:
# 194
 return - 1;_LL1C: _tmp5C=_tmp49.f2;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp5D=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp5C;if(_tmp5D->tag != 1)goto _LL1E;};_LL1D:
# 195
 return 1;_LL1E: _tmp5E=_tmp49.f1;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp5F=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp5E;if(_tmp5F->tag != 2)goto _LL11;else{_tmp60=_tmp5F->f1;}};_tmp61=_tmp49.f2;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp62=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp61;if(_tmp62->tag != 2)goto _LL11;else{_tmp63=_tmp62->f1;}};_LL1F:
# 196
 return _tmp60 - _tmp63;_LL11:;};}
# 200
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 202
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 205
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
# 206
if((int)p1 == (int)p2)
# 207
return 0;{
# 208
void*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_CfFlowInfo_Place _tmp64=*p1;_tmp65=_tmp64.root;_tmp66=_tmp64.fields;{
# 209
void*_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_Place _tmp67=*p2;_tmp68=_tmp67.root;_tmp69=_tmp67.fields;{
# 210
int i=Cyc_CfFlowInfo_root_cmp(_tmp65,_tmp68);
# 211
if(i != 0)
# 212
return i;
# 213
return Cyc_CfFlowInfo_intlist_cmp(_tmp66,_tmp69);};};};}
# 217
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
# 218
struct Cyc_List_List*sl=0;
# 219
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_CfFlowInfo_Place _tmp6A=*p;_tmp6B=_tmp6A.root;_tmp6C=_tmp6A.fields;
# 220
{void*_tmp6D=_tmp6B;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Exp*_tmp71;int _tmp73;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp6E->tag != 0)goto _LL23;else{_tmp6F=_tmp6E->f1;}}_LL22:
# 221
{struct _dyneither_ptr*_tmp510;const char*_tmp50F;void*_tmp50E[1];struct Cyc_String_pa_PrintArg_struct _tmp50D;struct Cyc_List_List*_tmp50C;sl=((_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C->hd=((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510[0]=((_tmp50D.tag=0,((_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6F->name).f2),((_tmp50E[0]=& _tmp50D,Cyc_aprintf(((_tmp50F="%s",_tag_dyneither(_tmp50F,sizeof(char),3))),_tag_dyneither(_tmp50E,sizeof(void*),1)))))))),_tmp510)))),((_tmp50C->tl=sl,_tmp50C))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp70=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp70->tag != 1)goto _LL25;else{_tmp71=_tmp70->f1;}}_LL24:
# 222
{struct _dyneither_ptr*_tmp51D;const char*_tmp51C;void*_tmp51B[1];struct Cyc_Int_pa_PrintArg_struct _tmp51A;struct Cyc_List_List*_tmp519;sl=((_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519->hd=((_tmp51D=_cycalloc(sizeof(*_tmp51D)),((_tmp51D[0]=((_tmp51A.tag=1,((_tmp51A.f1=(unsigned long)((int)_tmp71),((_tmp51B[0]=& _tmp51A,Cyc_aprintf(((_tmp51C="mpt%d",_tag_dyneither(_tmp51C,sizeof(char),6))),_tag_dyneither(_tmp51B,sizeof(void*),1)))))))),_tmp51D)))),((_tmp519->tl=sl,_tmp519))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp72=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp6D;if(_tmp72->tag != 2)goto _LL20;else{_tmp73=_tmp72->f1;}}_LL26:
# 223
{struct _dyneither_ptr*_tmp52A;const char*_tmp529;void*_tmp528[1];struct Cyc_Int_pa_PrintArg_struct _tmp527;struct Cyc_List_List*_tmp526;sl=((_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526->hd=((_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp527.tag=1,((_tmp527.f1=(unsigned long)_tmp73,((_tmp528[0]=& _tmp527,Cyc_aprintf(((_tmp529="param%d",_tag_dyneither(_tmp529,sizeof(char),8))),_tag_dyneither(_tmp528,sizeof(void*),1)))))))),_tmp52A)))),((_tmp526->tl=sl,_tmp526))))));}goto _LL20;_LL20:;}
# 225
for(0;_tmp6C != 0;_tmp6C=_tmp6C->tl){
# 226
struct _dyneither_ptr*_tmp537;const char*_tmp536;void*_tmp535[1];struct Cyc_Int_pa_PrintArg_struct _tmp534;struct Cyc_List_List*_tmp533;sl=((_tmp533=_cycalloc(sizeof(*_tmp533)),((_tmp533->hd=((_tmp537=_cycalloc(sizeof(*_tmp537)),((_tmp537[0]=((_tmp534.tag=1,((_tmp534.f1=(unsigned long)((int)_tmp6C->hd),((_tmp535[0]=& _tmp534,Cyc_aprintf(((_tmp536="%d",_tag_dyneither(_tmp536,sizeof(char),3))),_tag_dyneither(_tmp535,sizeof(void*),1)))))))),_tmp537)))),((_tmp533->tl=sl,_tmp533))))));}{
# 227
struct Cyc_String_pa_PrintArg_struct _tmp542;const char*_tmp541;void*_tmp540[1];const char*_tmp53F;struct _dyneither_ptr*_tmp53E;struct _dyneither_ptr*_tmp88=(_tmp53E=_cycalloc(sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp542.tag=0,((_tmp542.f1=(struct _dyneither_ptr)((_tmp541="",_tag_dyneither(_tmp541,sizeof(char),1))),((_tmp540[0]=& _tmp542,Cyc_aprintf(((_tmp53F="%s",_tag_dyneither(_tmp53F,sizeof(char),3))),_tag_dyneither(_tmp540,sizeof(void*),1)))))))),_tmp53E)));
# 228
for(0;sl != 0;sl=sl->tl){
# 229
const char*_tmp547;void*_tmp546[2];struct Cyc_String_pa_PrintArg_struct _tmp545;struct Cyc_String_pa_PrintArg_struct _tmp544;*_tmp88=((_tmp544.tag=0,((_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp88),((_tmp545.tag=0,((_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp546[0]=& _tmp545,((_tmp546[1]=& _tmp544,Cyc_aprintf(((_tmp547="%s.%s",_tag_dyneither(_tmp547,sizeof(char),6))),_tag_dyneither(_tmp546,sizeof(void*),2))))))))))))));}
# 230
return _tmp88;};}
# 243 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp54D(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp54C,unsigned int*_tmp54B,void***_tmp549){for(*_tmp54C=0;*_tmp54C < *_tmp54B;(*_tmp54C)++){(*_tmp549)[*_tmp54C]=(*fenv)->unknown_all;}}
# 248
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 254
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
# 255
unsigned int _tmp54C;unsigned int _tmp54B;struct _dyneither_ptr _tmp54A;void**_tmp549;unsigned int _tmp548;struct _dyneither_ptr d=(_tmp548=sz,((_tmp549=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp548)),((_tmp54A=_tag_dyneither(_tmp549,sizeof(void*),_tmp548),((((_tmp54B=_tmp548,_tmp54D(& fenv,& _tmp54C,& _tmp54B,& _tmp549))),_tmp54A)))))));
# 256
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
# 257
struct Cyc_Absyn_Aggrfield _tmp93;struct _dyneither_ptr*_tmp94;void*_tmp95;struct Cyc_Absyn_Aggrfield*_tmp92=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp93=*_tmp92;_tmp94=_tmp93.name;_tmp95=_tmp93.type;
# 258
if(_get_dyneither_size(*_tmp94,sizeof(char))!= 1)
# 259
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp95,leafval);}}
# 261
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 265
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 269
struct _tuple15 _tmp9B;struct _RegionHandle*_tmp9C;struct Cyc_List_List*_tmp9D;struct _tuple15*_tmp9A=env;_tmp9B=*_tmp9A;_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{
# 270
void*_tmp9E=Cyc_Tcutil_rsubstitute(_tmp9C,_tmp9D,f->type);
# 271
struct Cyc_Absyn_Aggrfield*_tmp54E;return(_tmp54E=_region_malloc(_tmp9C,sizeof(*_tmp54E)),((_tmp54E->name=f->name,((_tmp54E->tq=f->tq,((_tmp54E->type=_tmp9E,((_tmp54E->width=f->width,((_tmp54E->attributes=f->attributes,_tmp54E)))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 278
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 284
struct _RegionHandle _tmpA0=_new_region("temp");struct _RegionHandle*temp=& _tmpA0;_push_region(temp);
# 288
{struct Cyc_List_List*inst=0;
# 289
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
# 290
struct Cyc_Absyn_Tvar*_tmpA1=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 291
void*_tmpA2=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
# 292
{struct Cyc_Absyn_Kind*_tmpA3=Cyc_Tcutil_tvar_kind(_tmpA1,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmpA4;enum Cyc_Absyn_KindQual _tmpA5;struct Cyc_Absyn_Kind _tmpA6;enum Cyc_Absyn_KindQual _tmpA7;_LL28: _tmpA4=*_tmpA3;_tmpA5=_tmpA4.kind;if(_tmpA5 != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
# 293
 goto _LL2B;_LL2A: _tmpA6=*_tmpA3;_tmpA7=_tmpA6.kind;if(_tmpA7 != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
# 294
 continue;_LL2C:;_LL2D:
# 295
 goto _LL27;_LL27:;}{
# 297
struct _tuple16*_tmp551;struct Cyc_List_List*_tmp550;inst=((_tmp550=_region_malloc(temp,sizeof(*_tmp550)),((_tmp550->hd=((_tmp551=_region_malloc(temp,sizeof(*_tmp551)),((_tmp551->f1=_tmpA1,((_tmp551->f2=_tmpA2,_tmp551)))))),((_tmp550->tl=inst,_tmp550))))));};}
# 299
if(inst != 0){
# 300
struct _tuple15 _tmp552;struct _tuple15 _tmpAA=(_tmp552.f1=temp,((_tmp552.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp552)));
# 301
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpAA,fs);
# 302
struct _dyneither_ptr _tmpAB=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpAB;}else{
# 304
struct _dyneither_ptr _tmpAD=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpAD;}}
# 288
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp558(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp557,unsigned int*_tmp556,void***_tmp554){for(*_tmp557=0;*_tmp557 < *_tmp556;(*_tmp557)++){(*_tmp554)[*_tmp557]=(*fenv)->unknown_all;}}
# 308
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 312
int is_tagged=0;
# 316
{void*_tmpAE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo _tmpB0;union Cyc_Absyn_DatatypeFieldInfoU _tmpB1;struct _tuple2 _tmpB2;struct Cyc_Absyn_Datatypefield*_tmpB3;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_AggrInfo _tmpB7;union Cyc_Absyn_AggrInfoU _tmpB8;struct Cyc_List_List*_tmpB9;enum Cyc_Absyn_AggrKind _tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_Absyn_ArrayInfo _tmpBE;void*_tmpBF;union Cyc_Absyn_Constraint*_tmpC0;void*_tmpC2;struct Cyc_Absyn_PtrInfo _tmpC4;struct Cyc_Absyn_PtrAtts _tmpC5;union Cyc_Absyn_Constraint*_tmpC6;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpAE;if(_tmpAF->tag != 4)goto _LL31;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.field_info;if((_tmpB1.KnownDatatypefield).tag != 2)goto _LL31;_tmpB2=(struct _tuple2)(_tmpB1.KnownDatatypefield).val;_tmpB3=_tmpB2.f2;}}_LL30:
# 318
 if(_tmpB3->typs == 0)
# 319
return leafval;
# 320
_tmpB5=_tmpB3->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAE;if(_tmpB4->tag != 10)goto _LL33;else{_tmpB5=_tmpB4->f1;}}_LL32: {
# 322
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB5);
# 325
unsigned int _tmp557;unsigned int _tmp556;struct _dyneither_ptr _tmp555;void**_tmp554;unsigned int _tmp553;struct _dyneither_ptr d=(_tmp553=sz,((_tmp554=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp553)),((_tmp555=_tag_dyneither(_tmp554,sizeof(void*),_tmp553),((((_tmp556=_tmp553,_tmp558(& fenv,& _tmp557,& _tmp556,& _tmp554))),_tmp555)))))));
# 326
{int i=0;for(0;i < sz;++ i){
# 327
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpB5))->hd)).f2,leafval);
# 328
_tmpB5=_tmpB5->tl;}}{
# 330
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp55E;struct Cyc_CfFlowInfo_UnionRInfo _tmp55D;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp55C;return(void*)((_tmp55C=_region_malloc(fenv->r,sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55E.tag=6,((_tmp55E.f1=((_tmp55D.is_union=0,((_tmp55D.fieldnum=- 1,_tmp55D)))),((_tmp55E.f2=d,_tmp55E)))))),_tmp55C))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE;if(_tmpB6->tag != 11)goto _LL35;else{_tmpB7=_tmpB6->f1;_tmpB8=_tmpB7.aggr_info;_tmpB9=_tmpB7.targs;}}_LL34: {
# 332
struct Cyc_Absyn_Aggrdecl*_tmpCE=Cyc_Absyn_get_known_aggrdecl(_tmpB8);
# 333
if(_tmpCE->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE->impl))->tagged)return fenv->unknown_all;
# 334
is_tagged=1;
# 335
if(_tmpCE->impl == 0)
# 336
goto _LL2E;{
# 337
struct Cyc_List_List*_tmpCF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE->impl))->fields;
# 338
if(_tmpB9 == 0){_tmpBB=_tmpCE->kind;_tmpBC=_tmpCF;goto _LL36;}{
# 339
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp564;struct Cyc_CfFlowInfo_UnionRInfo _tmp563;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp562;return(void*)((_tmp562=_region_malloc(fenv->r,sizeof(*_tmp562)),((_tmp562[0]=(
# 340
(_tmp564.tag=6,((_tmp564.f1=((_tmp563.is_union=_tmpCE->kind == Cyc_Absyn_UnionA,((_tmp563.fieldnum=- 1,_tmp563)))),((_tmp564.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpCE->tvs,_tmpB9,_tmpCF,_tmpCE->kind == Cyc_Absyn_UnionA,leafval),_tmp564)))))),_tmp562))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAE;if(_tmpBA->tag != 12)goto _LL37;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}_LL36:
# 345
 if(_tmpBB == Cyc_Absyn_UnionA  && !is_tagged)return fenv->unknown_all;{
# 346
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp56A;struct Cyc_CfFlowInfo_UnionRInfo _tmp569;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp568;return(void*)((_tmp568=_region_malloc(fenv->r,sizeof(*_tmp568)),((_tmp568[0]=((_tmp56A.tag=6,((_tmp56A.f1=((_tmp569.is_union=_tmpBB == Cyc_Absyn_UnionA,((_tmp569.fieldnum=- 1,_tmp569)))),((_tmp56A.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpBC,_tmpBB == Cyc_Absyn_UnionA,leafval),_tmp56A)))))),_tmp568))));};_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE;if(_tmpBD->tag != 8)goto _LL39;else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBE.elt_type;_tmpC0=_tmpBE.zero_term;}}if(!
# 347
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC0))goto _LL39;_LL38:
# 350
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpBF)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpC1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAE;if(_tmpC1->tag != 19)goto _LL3B;else{_tmpC2=(void*)_tmpC1->f1;}}_LL3A:
# 351
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE;if(_tmpC3->tag != 5)goto _LL3D;else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC4.ptr_atts;_tmpC6=_tmpC5.nullable;}}if(!(!
# 352
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6)))goto _LL3D;_LL3C:
# 353
{void*_tmpD6=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpD8;enum Cyc_CfFlowInfo_InitLevel _tmpDA;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD6;if(_tmpD7->tag != 3)goto _LL42;else{_tmpD8=_tmpD7->f1;if(_tmpD8 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
# 354
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpD9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD6;if(_tmpD9->tag != 3)goto _LL44;else{_tmpDA=_tmpD9->f1;if(_tmpDA != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
# 355
 return fenv->notzeroall;_LL44:;_LL45:
# 356
 goto _LL3F;_LL3F:;}
# 358
goto _LL2E;_LL3D:;_LL3E:
# 359
 goto _LL2E;_LL2E:;}
# 362
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 365
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
# 366
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 369
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
# 370
void*_tmpDB=r;struct Cyc_Absyn_Exp*_tmpDD;int _tmpDE;void*_tmpDF;struct Cyc_CfFlowInfo_UnionRInfo _tmpE1;int _tmpE2;int _tmpE3;struct _dyneither_ptr _tmpE4;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpDB;if(_tmpDC->tag != 7)goto _LL49;else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDC->f2;_tmpDF=(void*)_tmpDC->f3;}}_LL48:
# 372
 if(_tmpDD == e  && _tmpDE == env_iteration){
# 373
*needs_unconsume=1;
# 374
return 0;}
# 376
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpE0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB;if(_tmpE0->tag != 6)goto _LL4B;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE1.is_union;_tmpE3=_tmpE1.fieldnum;_tmpE4=_tmpE0->f2;}}_LL4A:
# 378
 if(!_tmpE2  || _tmpE3 == - 1){
# 379
unsigned int _tmpE5=_get_dyneither_size(_tmpE4,sizeof(void*));
# 380
{int i=0;for(0;i < _tmpE5;++ i){
# 381
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpE4,sizeof(void*),i)),needs_unconsume))
# 382
return 1;}}
# 384
return 0;}else{
# 387
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpE4,sizeof(void*),_tmpE3)),needs_unconsume);}_LL4B:;_LL4C:
# 388
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp570(struct _dyneither_ptr*_tmpED,unsigned int*_tmp56F,unsigned int*_tmp56E,void***_tmp56C){for(*_tmp56F=0;*_tmp56F < *_tmp56E;(*_tmp56F)++){(*_tmp56C)[*_tmp56F]=*((void**)_check_dyneither_subscript(*_tmpED,sizeof(void*),(int)*_tmp56F));}}
# 392
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
# 393
void*_tmpE6=r;struct Cyc_Absyn_Exp*_tmpE8;int _tmpE9;void*_tmpEA;struct Cyc_CfFlowInfo_UnionRInfo _tmpEC;struct _dyneither_ptr _tmpED;_LL4E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE6;if(_tmpE7->tag != 7)goto _LL50;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;_tmpEA=(void*)_tmpE7->f3;}}_LL4F:
# 395
 return _tmpEA;_LL50: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE6;if(_tmpEB->tag != 6)goto _LL52;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;}}_LL51: {
# 397
unsigned int _tmpEE=_get_dyneither_size(_tmpED,sizeof(void*));
# 398
int change=0;
# 399
unsigned int _tmp56F;unsigned int _tmp56E;struct _dyneither_ptr _tmp56D;void**_tmp56C;unsigned int _tmp56B;struct _dyneither_ptr d2=(_tmp56B=_tmpEE,((_tmp56C=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp56B)),((_tmp56D=_tag_dyneither(_tmp56C,sizeof(void*),_tmp56B),((((_tmp56E=_tmp56B,_tmp570(& _tmpED,& _tmp56F,& _tmp56E,& _tmp56C))),_tmp56D)))))));
# 400
{int i=0;for(0;i < _tmpEE;++ i){
# 401
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmpED,sizeof(void*),i)));
# 402
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmpED,sizeof(void*),i)))
# 403
change=1;}}
# 405
if(change){
# 406
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp573;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp572;return(void*)((_tmp572=_region_malloc(fenv->r,sizeof(*_tmp572)),((_tmp572[0]=((_tmp573.tag=6,((_tmp573.f1=_tmpEC,((_tmp573.f2=d2,_tmp573)))))),_tmp572))));}else{
# 407
return r;}}_LL52:;_LL53:
# 408
 return r;_LL4D:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp57D(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp57C,unsigned int*_tmp57B,void***_tmp579){for(*_tmp57C=0;*_tmp57C < *_tmp57B;(*_tmp57C)++){(*_tmp579)[*_tmp57C]=(*fenv)->unknown_all;}}static void _tmp586(unsigned int*_tmp585,unsigned int*_tmp584,void***_tmp582,struct _dyneither_ptr*_tmp10F){for(*_tmp585=0;*_tmp585 < *_tmp584;(*_tmp585)++){(*_tmp582)[*_tmp585]=((void**)(*_tmp10F).curr)[(int)*_tmp585];}}
# 412
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
# 413
if(Cyc_Tcutil_is_noalias_pointer(t)){
# 414
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp576;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp575;return(void*)((_tmp575=_region_malloc(fenv->r,sizeof(*_tmp575)),((_tmp575[0]=((_tmp576.tag=7,((_tmp576.f1=consumer,((_tmp576.f2=iteration,((_tmp576.f3=(void*)r,_tmp576)))))))),_tmp575))));}{
# 415
struct _tuple0 _tmp577;struct _tuple0 _tmpF8=(_tmp577.f1=Cyc_Tcutil_compress(t),((_tmp577.f2=r,_tmp577)));void*_tmpF9;struct Cyc_List_List*_tmpFB;void*_tmpFC;struct Cyc_CfFlowInfo_UnionRInfo _tmpFE;struct _dyneither_ptr _tmpFF;void*_tmp100;struct Cyc_Absyn_AggrInfo _tmp102;union Cyc_Absyn_AggrInfoU _tmp103;void*_tmp104;struct Cyc_CfFlowInfo_UnionRInfo _tmp106;struct _dyneither_ptr _tmp107;void*_tmp108;enum Cyc_Absyn_AggrKind _tmp10A;struct Cyc_List_List*_tmp10B;void*_tmp10C;struct Cyc_CfFlowInfo_UnionRInfo _tmp10E;struct _dyneither_ptr _tmp10F;_LL55: _tmpF9=_tmpF8.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF9;if(_tmpFA->tag != 10)goto _LL57;else{_tmpFB=_tmpFA->f1;}};_tmpFC=_tmpF8.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpFC;if(_tmpFD->tag != 6)goto _LL57;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}};_LL56: {
# 417
unsigned int _tmp110=_get_dyneither_size(_tmpFF,sizeof(void*));
# 418
unsigned int _tmp57C;unsigned int _tmp57B;struct _dyneither_ptr _tmp57A;void**_tmp579;unsigned int _tmp578;struct _dyneither_ptr d2=(_tmp578=_tmp110,((_tmp579=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp578)),((_tmp57A=_tag_dyneither(_tmp579,sizeof(void*),_tmp578),((((_tmp57B=_tmp578,_tmp57D(& fenv,& _tmp57C,& _tmp57B,& _tmp579))),_tmp57A)))))));
# 419
{int i=0;for(0;i < _tmp110;++ i){
# 420
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpFB))->hd)).f2,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpFF,sizeof(void*),i)));
# 421
_tmpFB=_tmpFB->tl;}}{
# 423
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp580;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp57F;return(void*)((_tmp57F=_region_malloc(fenv->r,sizeof(*_tmp57F)),((_tmp57F[0]=((_tmp580.tag=6,((_tmp580.f1=_tmpFE,((_tmp580.f2=d2,_tmp580)))))),_tmp57F))));};}_LL57: _tmp100=_tmpF8.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 11)goto _LL59;else{_tmp102=_tmp101->f1;_tmp103=_tmp102.aggr_info;}};_tmp104=_tmpF8.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp105=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp104;if(_tmp105->tag != 6)goto _LL59;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;}};_LL58: {
# 425
struct Cyc_Absyn_Aggrdecl*_tmp117=Cyc_Absyn_get_known_aggrdecl(_tmp103);
# 426
if(_tmp117->impl == 0)return r;
# 427
_tmp10A=_tmp117->kind;_tmp10B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp117->impl))->fields;_tmp10E=_tmp106;_tmp10F=_tmp107;goto _LL5A;}_LL59: _tmp108=_tmpF8.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp109=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp108;if(_tmp109->tag != 12)goto _LL5B;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}};_tmp10C=_tmpF8.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp10C;if(_tmp10D->tag != 6)goto _LL5B;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;}};_LL5A: {
# 429
unsigned int _tmp585;unsigned int _tmp584;struct _dyneither_ptr _tmp583;void**_tmp582;unsigned int _tmp581;struct _dyneither_ptr d2=(_tmp581=_get_dyneither_size(_tmp10F,sizeof(void*)),((_tmp582=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp581)),((_tmp583=_tag_dyneither(_tmp582,sizeof(void*),_tmp581),((((_tmp584=_tmp581,_tmp586(& _tmp585,& _tmp584,& _tmp582,& _tmp10F))),_tmp583)))))));
# 430
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10B);
# 431
{int i=0;for(0;i < sz;(i ++,_tmp10B=_tmp10B->tl)){
# 432
struct Cyc_Absyn_Aggrfield _tmp119;struct _dyneither_ptr*_tmp11A;void*_tmp11B;struct Cyc_Absyn_Aggrfield*_tmp118=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp10B))->hd;_tmp119=*_tmp118;_tmp11A=_tmp119.name;_tmp11B=_tmp119.type;
# 433
if(_get_dyneither_size(*_tmp11A,sizeof(char))!= 1)
# 434
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp11B,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp10F,sizeof(void*),i)));}}{
# 436
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp589;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp588;return(void*)((_tmp588=_region_malloc(fenv->r,sizeof(*_tmp588)),((_tmp588[0]=((_tmp589.tag=6,((_tmp589.f1=_tmp10E,((_tmp589.f2=d2,_tmp589)))))),_tmp588))));};}_LL5B:;_LL5C:
# 437
 return r;_LL54:;};}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 441
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 444
struct _RegionHandle _tmp122=_new_region("r");struct _RegionHandle*r=& _tmp122;_push_region(r);
# 445
{struct _tuple18 _tmp58A;struct _tuple18 elem=(_tmp58A.f1=place,((_tmp58A.f2=0,_tmp58A)));
# 446
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
# 447
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
# 448
struct Cyc_CfFlowInfo_Place*_tmp123=elem.f1;
# 449
void*_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_CfFlowInfo_Place _tmp124=*place;_tmp125=_tmp124.root;_tmp126=_tmp124.fields;{
# 450
void*_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_CfFlowInfo_Place _tmp127=*_tmp123;_tmp128=_tmp127.root;_tmp129=_tmp127.fields;
# 451
if(Cyc_CfFlowInfo_root_cmp(_tmp125,_tmp128)!= 0)
# 452
continue;
# 453
for(0;_tmp126 != 0  && _tmp129 != 0;(_tmp126=_tmp126->tl,_tmp129=_tmp129->tl)){
# 454
if((int)_tmp126->hd != (int)_tmp129->hd)break;}
# 455
if(_tmp126 == 0){
# 456
int _tmp12A=1;_npop_handler(0);return _tmp12A;}};}{
# 458
int _tmp12B=0;_npop_handler(0);return _tmp12B;};}
# 445
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 471
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 473
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 476
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
# 477
struct Cyc_List_List*_tmp58B;pile->places=((_tmp58B=_region_malloc(pile->rgn,sizeof(*_tmp58B)),((_tmp58B->hd=place,((_tmp58B->tl=pile->places,_tmp58B))))));}}
# 479
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
# 480
void*_tmp12E=r;void*_tmp130;struct Cyc_CfFlowInfo_Place*_tmp132;struct Cyc_CfFlowInfo_UnionRInfo _tmp134;struct _dyneither_ptr _tmp135;_LL5E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp12E;if(_tmp12F->tag != 7)goto _LL60;else{_tmp130=(void*)_tmp12F->f3;}}_LL5F:
# 481
 Cyc_CfFlowInfo_add_places(pile,_tmp130);return;_LL60: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp12E;if(_tmp131->tag != 5)goto _LL62;else{_tmp132=_tmp131->f1;}}_LL61:
# 482
 Cyc_CfFlowInfo_add_place(pile,_tmp132);return;_LL62: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp133=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12E;if(_tmp133->tag != 6)goto _LL64;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL63:
# 484
{int i=0;for(0;i < _get_dyneither_size(_tmp135,sizeof(void*));++ i){
# 485
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp135.curr)[i]);}}
# 486
return;_LL64:;_LL65:
# 487
 return;_LL5D:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp591(struct _RegionHandle**r,void**new_val,unsigned int*_tmp590,unsigned int*_tmp58F,void***_tmp58D,struct _dyneither_ptr*_tmp13A){for(*_tmp590=0;*_tmp590 < *_tmp58F;(*_tmp590)++){(*_tmp58D)[*_tmp590]=
# 499
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp13A).curr)[(int)*_tmp590]);}}
# 494
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 496
void*_tmp136=old_val;struct Cyc_CfFlowInfo_UnionRInfo _tmp138;int _tmp139;struct _dyneither_ptr _tmp13A;struct Cyc_Absyn_Exp*_tmp13C;int _tmp13D;void*_tmp13E;_LL67: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp137=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp136;if(_tmp137->tag != 6)goto _LL69;else{_tmp138=_tmp137->f1;_tmp139=_tmp138.is_union;_tmp13A=_tmp137->f2;}}_LL68: {
# 498
unsigned int _tmp590;unsigned int _tmp58F;struct _dyneither_ptr _tmp58E;void**_tmp58D;unsigned int _tmp58C;struct _dyneither_ptr d2=(_tmp58C=_get_dyneither_size(_tmp13A,sizeof(void*)),((_tmp58D=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp58C)),((_tmp58E=_tag_dyneither(_tmp58D,sizeof(void*),_tmp58C),((((_tmp58F=_tmp58C,_tmp591(& r,& new_val,& _tmp590,& _tmp58F,& _tmp58D,& _tmp13A))),_tmp58E)))))));
# 502
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp597;struct Cyc_CfFlowInfo_UnionRInfo _tmp596;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp595;return(void*)((_tmp595=_region_malloc(r,sizeof(*_tmp595)),((_tmp595[0]=((_tmp597.tag=6,((_tmp597.f1=((_tmp596.is_union=_tmp139,((_tmp596.fieldnum=- 1,_tmp596)))),((_tmp597.f2=d2,_tmp597)))))),_tmp595))));}_LL69: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp13B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp136;if(_tmp13B->tag != 7)goto _LL6B;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;_tmp13E=(void*)_tmp13B->f3;}}_LL6A: {
# 504
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp59A;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp599;return(void*)((_tmp599=_region_malloc(r,sizeof(*_tmp599)),((_tmp599[0]=((_tmp59A.tag=7,((_tmp59A.f1=_tmp13C,((_tmp59A.f2=_tmp13D,((_tmp59A.f3=(void*)Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp13E),_tmp59A)))))))),_tmp599))));}_LL6B:;_LL6C:
# 505
 return new_val;_LL66:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp5A0(struct _dyneither_ptr*d,unsigned int*_tmp59F,unsigned int*_tmp59E,void***_tmp59C){for(*_tmp59F=0;*_tmp59F < *_tmp59E;(*_tmp59F)++){(*_tmp59C)[*_tmp59F]=((void**)(*d).curr)[(int)*_tmp59F];}}
# 511
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 513
void*_tmp148=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
# 514
if(_tmp148 == rval)return d;{
# 515
unsigned int _tmp59F;unsigned int _tmp59E;struct _dyneither_ptr _tmp59D;void**_tmp59C;unsigned int _tmp59B;struct _dyneither_ptr res=(_tmp59B=_get_dyneither_size(d,sizeof(void*)),((_tmp59C=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp59B)),((_tmp59D=_tag_dyneither(_tmp59C,sizeof(void*),_tmp59B),((((_tmp59E=_tmp59B,_tmp5A0(& d,& _tmp59F,& _tmp59E,& _tmp59C))),_tmp59D)))))));
# 516
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
# 517
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 525
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 529
if(fs == 0)
# 530
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
# 531
struct _tuple19 _tmp5A1;struct _tuple19 _tmp14E=(_tmp5A1.f1=fs,((_tmp5A1.f2=old_val,_tmp5A1)));struct Cyc_List_List*_tmp14F;struct Cyc_List_List _tmp150;int _tmp151;struct Cyc_List_List*_tmp152;void*_tmp153;struct Cyc_CfFlowInfo_UnionRInfo _tmp155;int _tmp156;struct _dyneither_ptr _tmp157;_LL6E: _tmp14F=_tmp14E.f1;if(_tmp14F == 0)goto _LL70;_tmp150=*_tmp14F;_tmp151=(int)_tmp150.hd;_tmp152=_tmp150.tl;_tmp153=_tmp14E.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp154=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp153;if(_tmp154->tag != 6)goto _LL70;else{_tmp155=_tmp154->f1;_tmp156=_tmp155.is_union;_tmp157=_tmp154->f2;}};_LL6F: {
# 533
void*_tmp158=Cyc_CfFlowInfo_insert_place_outer(r,_tmp152,*((void**)_check_dyneither_subscript(_tmp157,sizeof(void*),_tmp151)),new_val);
# 534
struct _dyneither_ptr _tmp159=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp157,_tmp151,_tmp158);
# 535
if((void**)_tmp159.curr == (void**)_tmp157.curr)return old_val;else{
# 536
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5A7;struct Cyc_CfFlowInfo_UnionRInfo _tmp5A6;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5A5;return(void*)((_tmp5A5=_region_malloc(r,sizeof(*_tmp5A5)),((_tmp5A5[0]=((_tmp5A7.tag=6,((_tmp5A7.f1=((_tmp5A6.is_union=_tmp156,((_tmp5A6.fieldnum=- 1,_tmp5A6)))),((_tmp5A7.f2=_tmp159,_tmp5A7)))))),_tmp5A5))));}}_LL70:;_LL71: {
# 537
struct Cyc_Core_Impossible_exn_struct _tmp5AD;const char*_tmp5AC;struct Cyc_Core_Impossible_exn_struct*_tmp5AB;(int)_throw((void*)((_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((_tmp5AB[0]=((_tmp5AD.tag=Cyc_Core_Impossible,((_tmp5AD.f1=((_tmp5AC="bad insert place",_tag_dyneither(_tmp5AC,sizeof(char),17))),_tmp5AD)))),_tmp5AB)))));}_LL6D:;};}
# 544
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 548
while(pile->places != 0){
# 549
struct Cyc_CfFlowInfo_Place*_tmp160=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
# 550
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
# 551
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp160,0);{
# 552
void*oldval;void*newval;
# 553
{struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp160);;_pop_handler();}else{void*_tmp162=(void*)_exn_thrown;void*_tmp165=_tmp162;_LL73: {struct Cyc_Dict_Absent_exn_struct*_tmp166=(struct Cyc_Dict_Absent_exn_struct*)_tmp165;if(_tmp166->tag != Cyc_Dict_Absent)goto _LL75;}_LL74:
# 554
 continue;_LL75:;_LL76:(void)_throw(_tmp165);_LL72:;}};}
# 555
switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL77:
# 556
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL78:
# 557
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL79:
# 558
 newval=fenv->esc_none;break;}
# 560
Cyc_CfFlowInfo_add_places(pile,oldval);{
# 561
void*_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_CfFlowInfo_Place _tmp167=*_tmp160;_tmp168=_tmp167.root;_tmp169=_tmp167.fields;
# 562
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp168,
# 563
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp169,
# 565
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp168),newval));};};}
# 568
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 580
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
# 581
void*_tmp16A=r;enum Cyc_CfFlowInfo_InitLevel _tmp16C;enum Cyc_CfFlowInfo_InitLevel _tmp16E;void*_tmp173;_LL7C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp16B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp16B->tag != 3)goto _LL7E;else{_tmp16C=_tmp16B->f1;}}_LL7D:
# 582
 return _tmp16C;_LL7E: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp16D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp16D->tag != 4)goto _LL80;else{_tmp16E=_tmp16D->f1;}}_LL7F:
# 583
 return _tmp16E;_LL80: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp16F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp16F->tag != 0)goto _LL82;}_LL81:
# 584
 goto _LL83;_LL82: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp170=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp170->tag != 1)goto _LL84;}_LL83:
# 585
 return Cyc_CfFlowInfo_AllIL;_LL84: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp171=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp171->tag != 2)goto _LL86;}_LL85:
# 586
 return Cyc_CfFlowInfo_ThisIL;_LL86: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp172=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp16A;if(_tmp172->tag != 7)goto _LL88;else{_tmp173=(void*)_tmp172->f3;}}_LL87:
# 587
 return Cyc_CfFlowInfo_NoneIL;_LL88:;_LL89: {
# 588
struct Cyc_Core_Impossible_exn_struct _tmp5B3;const char*_tmp5B2;struct Cyc_Core_Impossible_exn_struct*_tmp5B1;(int)_throw((void*)((_tmp5B1=_cycalloc(sizeof(*_tmp5B1)),((_tmp5B1[0]=((_tmp5B3.tag=Cyc_Core_Impossible,((_tmp5B3.f1=((_tmp5B2="initlevel_approx",_tag_dyneither(_tmp5B2,sizeof(char),17))),_tmp5B3)))),_tmp5B1)))));}_LL7B:;}
# 591
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 593
enum Cyc_CfFlowInfo_InitLevel this_ans;
# 594
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
# 595
{void*_tmp177=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp179;int _tmp17A;int _tmp17B;struct _dyneither_ptr _tmp17C;struct Cyc_CfFlowInfo_UnionRInfo _tmp17E;int _tmp17F;struct _dyneither_ptr _tmp180;struct Cyc_CfFlowInfo_Place*_tmp182;_LL8B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp178=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177;if(_tmp178->tag != 6)goto _LL8D;else{_tmp179=_tmp178->f1;_tmp17A=_tmp179.is_union;if(_tmp17A != 1)goto _LL8D;_tmp17B=_tmp179.fieldnum;_tmp17C=_tmp178->f2;}}_LL8C: {
# 599
unsigned int _tmp183=_get_dyneither_size(_tmp17C,sizeof(void*));
# 600
this_ans=Cyc_CfFlowInfo_NoneIL;
# 601
if(_tmp17B == - 1){
# 602
int i=0;for(0;i < _tmp183;++ i){
# 603
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp17C,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
# 604
this_ans=Cyc_CfFlowInfo_AllIL;
# 605
break;}}}else{
# 609
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp17C,sizeof(void*),_tmp17B)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
# 610
this_ans=Cyc_CfFlowInfo_AllIL;}
# 611
goto _LL8A;}_LL8D: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp17D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp177;if(_tmp17D->tag != 6)goto _LL8F;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17E.is_union;if(_tmp17F != 0)goto _LL8F;_tmp180=_tmp17D->f2;}}_LL8E: {
# 613
unsigned int _tmp184=_get_dyneither_size(_tmp180,sizeof(void*));
# 614
this_ans=Cyc_CfFlowInfo_AllIL;
# 615
{int i=0;for(0;i < _tmp184;++ i){
# 616
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp180,sizeof(void*),i)),this_ans);}}
# 617
goto _LL8A;}_LL8F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp181=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp177;if(_tmp181->tag != 5)goto _LL91;else{_tmp182=_tmp181->f1;}}_LL90:
# 619
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp182))
# 620
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 622
{struct Cyc_List_List*_tmp5B4;env->seen=((_tmp5B4=_region_malloc(env->e,sizeof(*_tmp5B4)),((_tmp5B4->hd=_tmp182,((_tmp5B4->tl=env->seen,_tmp5B4))))));}
# 623
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp182),Cyc_CfFlowInfo_AllIL);
# 624
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
# 625
if(this_ans == Cyc_CfFlowInfo_NoneIL)
# 626
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 628
goto _LL8A;_LL91:;_LL92:
# 629
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL8A:;}
# 631
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
# 632
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
# 633
return Cyc_CfFlowInfo_AllIL;}
# 635
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
# 636
struct _RegionHandle*_tmp186=env->r;
# 637
struct Cyc_CfFlowInfo_InitlevelEnv _tmp5B5;struct Cyc_CfFlowInfo_InitlevelEnv _tmp187=(_tmp5B5.e=_tmp186,((_tmp5B5.d=d,((_tmp5B5.seen=0,_tmp5B5)))));
# 638
return Cyc_CfFlowInfo_initlevel_rec(& _tmp187,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 642
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
# 643
void*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_CfFlowInfo_Place _tmp189=*place;_tmp18A=_tmp189.root;_tmp18B=_tmp189.fields;{
# 644
void*_tmp18C=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp18A);
# 645
for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){
# 646
struct _tuple20 _tmp5B6;struct _tuple20 _tmp18E=(_tmp5B6.f1=_tmp18C,((_tmp5B6.f2=(int)_tmp18B->hd,_tmp5B6)));void*_tmp18F;struct Cyc_CfFlowInfo_UnionRInfo _tmp191;struct _dyneither_ptr _tmp192;int _tmp193;_LL94: _tmp18F=_tmp18E.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp190=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp18F;if(_tmp190->tag != 6)goto _LL96;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;}};_tmp193=_tmp18E.f2;_LL95:
# 648
 _tmp18C=*((void**)_check_dyneither_subscript(_tmp192,sizeof(void*),_tmp193));
# 649
goto _LL93;_LL96:;_LL97: {
# 654
struct Cyc_Core_Impossible_exn_struct _tmp5BC;const char*_tmp5BB;struct Cyc_Core_Impossible_exn_struct*_tmp5BA;(int)_throw((void*)((_tmp5BA=_cycalloc(sizeof(*_tmp5BA)),((_tmp5BA[0]=((_tmp5BC.tag=Cyc_Core_Impossible,((_tmp5BC.f1=((_tmp5BB="bad lookup_place",_tag_dyneither(_tmp5BB,sizeof(char),17))),_tmp5BC)))),_tmp5BA)))));}_LL93:;}
# 656
return _tmp18C;};}
# 659
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
# 660
void*_tmp197=rval;void*_tmp19A;struct Cyc_CfFlowInfo_UnionRInfo _tmp19C;int _tmp19D;int _tmp19E;struct _dyneither_ptr _tmp19F;_LL99: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp198=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp197;if(_tmp198->tag != 4)goto _LL9B;}_LL9A:
# 661
 return 0;_LL9B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp199=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp197;if(_tmp199->tag != 7)goto _LL9D;else{_tmp19A=(void*)_tmp199->f3;}}_LL9C:
# 662
 return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL9D: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp19B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp197;if(_tmp19B->tag != 6)goto _LL9F;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.is_union;_tmp19E=_tmp19C.fieldnum;_tmp19F=_tmp19B->f2;}}_LL9E:
# 664
 if(_tmp19D  && _tmp19E != - 1)
# 665
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp19F,sizeof(void*),_tmp19E)));else{
# 667
unsigned int sz=_get_dyneither_size(_tmp19F,sizeof(void*));
# 668
{int i=0;for(0;i < sz;++ i){
# 669
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp19F.curr)[i]))return 0;}}
# 670
return 1;}_LL9F:;_LLA0:
# 672
 return 1;_LL98:;}
# 675
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
# 676
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 680
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
# 681
struct _RegionHandle*_tmp1A0=fenv->r;
# 682
struct Cyc_CfFlowInfo_EscPile*_tmp5BD;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp5BD=_region_malloc(_tmp1A0,sizeof(*_tmp5BD)),((_tmp5BD->rgn=_tmp1A0,((_tmp5BD->places=0,_tmp5BD)))));
# 683
Cyc_CfFlowInfo_add_places(pile,r);
# 684
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp5C7(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp5C6,unsigned int*_tmp5C5,void***_tmp5C3,struct _dyneither_ptr*_tmp1B2,struct _dyneither_ptr*_tmp1AE){for(*_tmp5C6=0;*_tmp5C6 < *_tmp5C5;(*_tmp5C6)++){(*_tmp5C3)[*_tmp5C6]=
# 712
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1AE).curr)[(int)*_tmp5C6],*((void**)_check_dyneither_subscript(*_tmp1B2,sizeof(void*),(int)*_tmp5C6)));}}
# 698
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 703
struct _tuple0 _tmp5BE;struct _tuple0 _tmp1A3=(_tmp5BE.f1=oldval,((_tmp5BE.f2=newval,_tmp5BE)));void*_tmp1A4;void*_tmp1A6;struct Cyc_CfFlowInfo_Place*_tmp1A8;void*_tmp1A9;void*_tmp1AB;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AD;struct _dyneither_ptr _tmp1AE;void*_tmp1AF;struct Cyc_CfFlowInfo_UnionRInfo _tmp1B1;struct _dyneither_ptr _tmp1B2;void*_tmp1B3;enum Cyc_CfFlowInfo_InitLevel _tmp1B5;_LLA2: _tmp1A4=_tmp1A3.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A5=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A4;if(_tmp1A5->tag != 4)goto _LLA4;};_tmp1A6=_tmp1A3.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1A7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A6;if(_tmp1A7->tag != 5)goto _LLA4;else{_tmp1A8=_tmp1A7->f1;}};_LLA3:
# 704
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A8);goto _LLA5;_LLA4: _tmp1A9=_tmp1A3.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1AA=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A9;if(_tmp1AA->tag != 4)goto _LLA6;};_LLA5:
# 706
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
# 707
const char*_tmp5C1;void*_tmp5C0;(_tmp5C0=0,Cyc_Tcutil_terr(env->loc,((_tmp5C1="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp5C1,sizeof(char),67))),_tag_dyneither(_tmp5C0,sizeof(void*),0)));}
# 709
return(env->fenv)->esc_all;_LLA6: _tmp1AB=_tmp1A3.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AB;if(_tmp1AC->tag != 6)goto _LLA8;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}};_tmp1AF=_tmp1A3.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1B0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AF;if(_tmp1B0->tag != 6)goto _LLA8;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}};_LLA7: {
# 711
unsigned int _tmp5C6;unsigned int _tmp5C5;struct _dyneither_ptr _tmp5C4;void**_tmp5C3;unsigned int _tmp5C2;struct _dyneither_ptr new_d=
# 712
(_tmp5C2=_get_dyneither_size(_tmp1AE,sizeof(void*)),((_tmp5C3=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp5C2)),((_tmp5C4=_tag_dyneither(_tmp5C3,sizeof(void*),_tmp5C2),((((_tmp5C5=_tmp5C2,_tmp5C7(& env,& _tmp5C6,& _tmp5C5,& _tmp5C3,& _tmp1B2,& _tmp1AE))),_tmp5C4)))))));
# 715
int change=0;
# 716
{int i=0;for(0;i < _get_dyneither_size(_tmp1AE,sizeof(void*));++ i){
# 717
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1AE.curr)[i]){
# 718
change=1;break;}}}
# 720
if(!change){
# 721
if(!_tmp1AD.is_union)return oldval;
# 722
new_d=_tmp1AE;}else{
# 725
change=0;
# 726
{int i=0;for(0;i < _get_dyneither_size(_tmp1AE,sizeof(void*));++ i){
# 727
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1B2,sizeof(void*),i))){
# 728
change=1;break;}}}
# 730
if(!change){
# 731
if(!_tmp1AD.is_union)return newval;
# 732
new_d=_tmp1B2;}}{
# 735
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5CA;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5C9;return(void*)((_tmp5C9=_region_malloc((env->fenv)->r,sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CA.tag=6,((_tmp5CA.f1=_tmp1B1,((_tmp5CA.f2=new_d,_tmp5CA)))))),_tmp5C9))));};}_LLA8: _tmp1B3=_tmp1A3.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1B4=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1B3;if(_tmp1B4->tag != 4)goto _LLAA;else{_tmp1B5=_tmp1B4->f1;}};_LLA9:
# 737
 switch(_tmp1B5){case Cyc_CfFlowInfo_NoneIL: _LLAC:
# 738
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLAD:
# 739
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLAE:
# 740
 return(env->fenv)->unknown_all;}_LLAA:;_LLAB:
# 742
 return newval;_LLA1:;}
# 748
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
# 749
if(*n > 0){*n=*n - 1;return 1;}else{
# 750
return 0;}}
# 752
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 757
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
# 758
struct _tuple19 _tmp5CB;struct _tuple19 _tmp1BF=(_tmp5CB.f1=fs,((_tmp5CB.f2=oldval,_tmp5CB)));struct Cyc_List_List*_tmp1C0;struct Cyc_List_List _tmp1C1;int _tmp1C2;struct Cyc_List_List*_tmp1C3;void*_tmp1C4;struct Cyc_CfFlowInfo_UnionRInfo _tmp1C6;int _tmp1C7;int _tmp1C8;struct _dyneither_ptr _tmp1C9;_LLB1: _tmp1C0=_tmp1BF.f1;if(_tmp1C0 == 0)goto _LLB3;_tmp1C1=*_tmp1C0;_tmp1C2=(int)_tmp1C1.hd;_tmp1C3=_tmp1C1.tl;_tmp1C4=_tmp1BF.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1C5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1C4;if(_tmp1C5->tag != 6)goto _LLB3;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C6.is_union;_tmp1C8=_tmp1C6.fieldnum;_tmp1C9=_tmp1C5->f2;}};_LLB2: {
# 760
void*_tmp1CA=Cyc_CfFlowInfo_assign_place_outer(env,_tmp1C3,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1C9,sizeof(void*),_tmp1C2)),newval);
# 762
struct _dyneither_ptr _tmp1CB=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C9,_tmp1C2,_tmp1CA);
# 763
if((void**)_tmp1CB.curr == (void**)_tmp1C9.curr  && (!_tmp1C7  || _tmp1C8 == _tmp1C2))return oldval;
# 764
_tmp1C9=_tmp1CB;
# 767
if(_tmp1C7){
# 768
int changed=0;
# 769
int sz=(int)_get_dyneither_size(_tmp1C9,sizeof(void*));
# 770
{int i=0;for(0;i < sz;++ i){
# 771
if(i != _tmp1C2){
# 772
struct _dyneither_ptr _tmp1CC=
# 773
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C9,i,
# 774
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp1C9,sizeof(void*),i))));
# 777
if((void**)_tmp1CC.curr != (void**)_tmp1C9.curr){
# 778
_tmp1C9=_tmp1CC;
# 779
changed=1;}}}}
# 785
if(changed){
# 786
struct Cyc_CfFlowInfo_Place _tmp1CE;void*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_CfFlowInfo_Place*_tmp1CD=env->root_place;_tmp1CE=*_tmp1CD;_tmp1CF=_tmp1CE.root;_tmp1D0=_tmp1CE.fields;{
# 787
int*_tmp5CC;struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp5CC=_cycalloc_atomic(sizeof(*_tmp5CC)),((_tmp5CC[0]=fs_prefix,_tmp5CC)))),_tmp1D0);
# 788
struct Cyc_CfFlowInfo_Place*_tmp5CD;struct Cyc_CfFlowInfo_Place*_tmp1D2=(_tmp5CD=_region_malloc((env->pile)->rgn,sizeof(*_tmp5CD)),((_tmp5CD->root=_tmp1CF,((_tmp5CD->fields=_tmp1D1,_tmp5CD)))));
# 789
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1D2,0);};}}{
# 792
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5D3;struct Cyc_CfFlowInfo_UnionRInfo _tmp5D2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5D1;return(void*)((_tmp5D1=_region_malloc((env->fenv)->r,sizeof(*_tmp5D1)),((_tmp5D1[0]=((_tmp5D3.tag=6,((_tmp5D3.f1=((_tmp5D2.is_union=_tmp1C7,((_tmp5D2.fieldnum=_tmp1C2,_tmp5D2)))),((_tmp5D3.f2=_tmp1C9,_tmp5D3)))))),_tmp5D1))));};}_LLB3:;_LLB4: {
# 793
struct Cyc_Core_Impossible_exn_struct _tmp5D9;const char*_tmp5D8;struct Cyc_Core_Impossible_exn_struct*_tmp5D7;(int)_throw((void*)((_tmp5D7=_cycalloc(sizeof(*_tmp5D7)),((_tmp5D7[0]=((_tmp5D9.tag=Cyc_Core_Impossible,((_tmp5D9.f1=((_tmp5D8="bad assign place",_tag_dyneither(_tmp5D8,sizeof(char),17))),_tmp5D9)))),_tmp5D7)))));}_LLB0:;};}
# 796
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 801
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
# 802
struct _RegionHandle*_tmp1DB=fenv->r;
# 803
struct Cyc_CfFlowInfo_Place _tmp1DD;void*_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_CfFlowInfo_Place*_tmp1DC=place;_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DD.root;_tmp1DF=_tmp1DD.fields;{
# 804
struct Cyc_CfFlowInfo_EscPile*_tmp5DC;struct Cyc_CfFlowInfo_AssignEnv _tmp5DB;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5DB.fenv=fenv,((_tmp5DB.pile=((_tmp5DC=_region_malloc(_tmp1DB,sizeof(*_tmp5DC)),((_tmp5DC->rgn=_tmp1DB,((_tmp5DC->places=0,_tmp5DC)))))),((_tmp5DB.d=d,((_tmp5DB.loc=loc,((_tmp5DB.root_place=place,((_tmp5DB.all_changed=all_changed,_tmp5DB)))))))))));
# 805
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1DF,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1DE),r);
# 809
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1DE,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 820
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
# 821
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 830
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 832
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5EC(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp5EB,unsigned int*_tmp5EA,void***_tmp5E8,struct _dyneither_ptr*_tmp22C,struct _dyneither_ptr*_tmp226){for(*_tmp5EB=0;*_tmp5EB < *_tmp5EA;(*_tmp5EB)++){(*_tmp5E8)[*_tmp5EB]=
# 898 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp226).curr)[(int)*_tmp5EB],*((void**)_check_dyneither_subscript(*_tmp22C,sizeof(void*),(int)*_tmp5EB)));}}
# 842
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
# 843
if(r1 == r2)return r1;{
# 844
struct _RegionHandle*_tmp1E2=(env->fenv)->r;
# 846
{struct _tuple0 _tmp5DD;struct _tuple0 _tmp1E4=(_tmp5DD.f1=r1,((_tmp5DD.f2=r2,_tmp5DD)));void*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;int _tmp1E8;void*_tmp1E9;void*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EC;int _tmp1ED;void*_tmp1EE;void*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;int _tmp1F2;void*_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;int _tmp1F7;void*_tmp1F8;void*_tmp1F9;struct Cyc_CfFlowInfo_Place*_tmp1FB;void*_tmp1FC;struct Cyc_CfFlowInfo_Place*_tmp1FE;void*_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp201;void*_tmp202;void*_tmp204;void*_tmp206;struct Cyc_CfFlowInfo_Place*_tmp208;void*_tmp209;struct Cyc_CfFlowInfo_Place*_tmp20B;void*_tmp20C;void*_tmp20E;void*_tmp210;struct Cyc_CfFlowInfo_Place*_tmp212;void*_tmp213;struct Cyc_CfFlowInfo_Place*_tmp215;void*_tmp216;struct Cyc_CfFlowInfo_Place*_tmp218;void*_tmp219;void*_tmp21B;void*_tmp21D;void*_tmp21F;void*_tmp221;struct Cyc_CfFlowInfo_UnionRInfo _tmp223;int _tmp224;int _tmp225;struct _dyneither_ptr _tmp226;void*_tmp227;struct Cyc_CfFlowInfo_UnionRInfo _tmp229;int _tmp22A;int _tmp22B;struct _dyneither_ptr _tmp22C;_LLB6: _tmp1E5=_tmp1E4.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1E6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E5;if(_tmp1E6->tag != 7)goto _LLB8;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;_tmp1E9=(void*)_tmp1E6->f3;}};_tmp1EA=_tmp1E4.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1EB=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1EA;if(_tmp1EB->tag != 7)goto _LLB8;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;_tmp1EE=(void*)_tmp1EB->f3;}};_LLB7:
# 849
 if(_tmp1E7 == _tmp1EC  && _tmp1E8 == _tmp1ED){
# 850
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp5E0;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5DF;return(void*)((_tmp5DF=_region_malloc(_tmp1E2,sizeof(*_tmp5DF)),((_tmp5DF[0]=((_tmp5E0.tag=7,((_tmp5E0.f1=_tmp1E7,((_tmp5E0.f2=_tmp1E8,((_tmp5E0.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E9,_tmp1EE),_tmp5E0)))))))),_tmp5DF))));}
# 851
{void*_tmp22F=_tmp1E9;struct Cyc_CfFlowInfo_Place*_tmp231;_LLD3: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp230=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp22F;if(_tmp230->tag != 5)goto _LLD5;else{_tmp231=_tmp230->f1;}}_LLD4:
# 852
 Cyc_CfFlowInfo_add_place(env->pile,_tmp231);goto _LLD2;_LLD5:;_LLD6:
# 853
 goto _LLD2;_LLD2:;}
# 855
{void*_tmp232=_tmp1EE;struct Cyc_CfFlowInfo_Place*_tmp234;_LLD8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp233=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp232;if(_tmp233->tag != 5)goto _LLDA;else{_tmp234=_tmp233->f1;}}_LLD9:
# 856
 Cyc_CfFlowInfo_add_place(env->pile,_tmp234);goto _LLD7;_LLDA:;_LLDB:
# 857
 goto _LLD7;_LLD7:;}
# 859
goto _LLB5;_LLB8: _tmp1EF=_tmp1E4.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1EF;if(_tmp1F0->tag != 7)goto _LLBA;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;_tmp1F3=(void*)_tmp1F0->f3;}};_LLB9: {
# 861
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp5E3;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E2;return(void*)((_tmp5E2=_region_malloc(_tmp1E2,sizeof(*_tmp5E2)),((_tmp5E2[0]=((_tmp5E3.tag=7,((_tmp5E3.f1=_tmp1F1,((_tmp5E3.f2=_tmp1F2,((_tmp5E3.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F3,r2),_tmp5E3)))))))),_tmp5E2))));}_LLBA: _tmp1F4=_tmp1E4.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F5=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F4;if(_tmp1F5->tag != 7)goto _LLBC;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;_tmp1F8=(void*)_tmp1F5->f3;}};_LLBB: {
# 863
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp5E6;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E5;return(void*)((_tmp5E5=_region_malloc(_tmp1E2,sizeof(*_tmp5E5)),((_tmp5E5[0]=((_tmp5E6.tag=7,((_tmp5E6.f1=_tmp1F6,((_tmp5E6.f2=_tmp1F7,((_tmp5E6.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1F8),_tmp5E6)))))))),_tmp5E5))));}_LLBC: _tmp1F9=_tmp1E4.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F9;if(_tmp1FA->tag != 5)goto _LLBE;else{_tmp1FB=_tmp1FA->f1;}};_tmp1FC=_tmp1E4.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FC;if(_tmp1FD->tag != 5)goto _LLBE;else{_tmp1FE=_tmp1FD->f1;}};_LLBD:
# 866
 if(Cyc_CfFlowInfo_place_cmp(_tmp1FB,_tmp1FE)== 0)return r1;
# 867
Cyc_CfFlowInfo_add_place(env->pile,_tmp1FB);
# 868
Cyc_CfFlowInfo_add_place(env->pile,_tmp1FE);
# 869
goto _LLB5;_LLBE: _tmp1FF=_tmp1E4.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp200=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1FF;if(_tmp200->tag != 5)goto _LLC0;else{_tmp201=_tmp200->f1;}};_tmp202=_tmp1E4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp203=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp202;if(_tmp203->tag != 2)goto _LLC0;};_LLBF:
# 871
 _tmp208=_tmp201;goto _LLC1;_LLC0: _tmp204=_tmp1E4.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp205=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp204;if(_tmp205->tag != 2)goto _LLC2;};_tmp206=_tmp1E4.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp207=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp206;if(_tmp207->tag != 5)goto _LLC2;else{_tmp208=_tmp207->f1;}};_LLC1:
# 873
 Cyc_CfFlowInfo_add_place(env->pile,_tmp208);
# 874
return(env->fenv)->notzerothis;_LLC2: _tmp209=_tmp1E4.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LLC4;else{_tmp20B=_tmp20A->f1;}};_tmp20C=_tmp1E4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20D=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp20C;if(_tmp20D->tag != 1)goto _LLC4;};_LLC3:
# 877
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20B);
# 878
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLDC:
# 879
 return(env->fenv)->notzeroall;default: _LLDD:
# 880
 return(env->fenv)->notzerothis;}_LLC4: _tmp20E=_tmp1E4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20F=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp20E;if(_tmp20F->tag != 1)goto _LLC6;};_tmp210=_tmp1E4.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp211=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp210;if(_tmp211->tag != 5)goto _LLC6;else{_tmp212=_tmp211->f1;}};_LLC5:
# 883
 Cyc_CfFlowInfo_add_place(env->pile,_tmp212);
# 884
switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL: _LLDF:
# 885
 return(env->fenv)->notzeroall;default: _LLE0:
# 886
 return(env->fenv)->notzerothis;}_LLC6: _tmp213=_tmp1E4.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp214=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLC8;else{_tmp215=_tmp214->f1;}};_LLC7:
# 889
 Cyc_CfFlowInfo_add_place(env->pile,_tmp215);goto _LLB5;_LLC8: _tmp216=_tmp1E4.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp217=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp216;if(_tmp217->tag != 5)goto _LLCA;else{_tmp218=_tmp217->f1;}};_LLC9:
# 890
 Cyc_CfFlowInfo_add_place(env->pile,_tmp218);goto _LLB5;_LLCA: _tmp219=_tmp1E4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp21A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp219;if(_tmp21A->tag != 1)goto _LLCC;};_tmp21B=_tmp1E4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp21C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp21B;if(_tmp21C->tag != 2)goto _LLCC;};_LLCB:
# 893
 goto _LLCD;_LLCC: _tmp21D=_tmp1E4.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp21E=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp21D;if(_tmp21E->tag != 2)goto _LLCE;};_tmp21F=_tmp1E4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp220=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp21F;if(_tmp220->tag != 1)goto _LLCE;};_LLCD:
# 894
 return(env->fenv)->notzerothis;_LLCE: _tmp221=_tmp1E4.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp222=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp221;if(_tmp222->tag != 6)goto _LLD0;else{_tmp223=_tmp222->f1;_tmp224=_tmp223.is_union;_tmp225=_tmp223.fieldnum;_tmp226=_tmp222->f2;}};_tmp227=_tmp1E4.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp228=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp227;if(_tmp228->tag != 6)goto _LLD0;else{_tmp229=_tmp228->f1;_tmp22A=_tmp229.is_union;_tmp22B=_tmp229.fieldnum;_tmp22C=_tmp228->f2;}};_LLCF: {
# 897
unsigned int _tmp5EB;unsigned int _tmp5EA;struct _dyneither_ptr _tmp5E9;void**_tmp5E8;unsigned int _tmp5E7;struct _dyneither_ptr new_d=
# 898
(_tmp5E7=_get_dyneither_size(_tmp226,sizeof(void*)),((_tmp5E8=(void**)_region_malloc(_tmp1E2,_check_times(sizeof(void*),_tmp5E7)),((_tmp5E9=_tag_dyneither(_tmp5E8,sizeof(void*),_tmp5E7),((((_tmp5EA=_tmp5E7,_tmp5EC(& env,& _tmp5EB,& _tmp5EA,& _tmp5E8,& _tmp22C,& _tmp226))),_tmp5E9)))))));
# 902
int change=0;
# 903
{int i=0;for(0;i < _get_dyneither_size(_tmp226,sizeof(void*));++ i){
# 904
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp226.curr)[i]){
# 905
change=1;break;}}}
# 907
if(!change){
# 908
if(!_tmp224)return r1;
# 909
new_d=_tmp226;}else{
# 912
change=0;
# 913
{int i=0;for(0;i < _get_dyneither_size(_tmp226,sizeof(void*));++ i){
# 914
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp22C,sizeof(void*),i))){
# 915
change=1;break;}}}
# 917
if(!change){
# 918
if(!_tmp224)return r2;
# 919
new_d=_tmp22C;}}{
# 922
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp5F2;struct Cyc_CfFlowInfo_UnionRInfo _tmp5F1;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5F0;return(void*)((_tmp5F0=_region_malloc(_tmp1E2,sizeof(*_tmp5F0)),((_tmp5F0[0]=((_tmp5F2.tag=6,((_tmp5F2.f1=((_tmp5F1.is_union=_tmp224,((_tmp5F1.fieldnum=_tmp225 == _tmp22B?_tmp225: - 1,_tmp5F1)))),((_tmp5F2.f2=new_d,_tmp5F2)))))),_tmp5F0))));};}_LLD0:;_LLD1:
# 924
 goto _LLB5;_LLB5:;}{
# 926
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
# 927
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
# 928
struct _tuple0 _tmp5F3;struct _tuple0 _tmp241=(_tmp5F3.f1=r1,((_tmp5F3.f2=r2,_tmp5F3)));void*_tmp242;void*_tmp244;_LLE3: _tmp242=_tmp241.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp243=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp242;if(_tmp243->tag != 4)goto _LLE5;};_LLE4:
# 929
 goto _LLE6;_LLE5: _tmp244=_tmp241.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp245=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp244;if(_tmp245->tag != 4)goto _LLE7;};_LLE6: {
# 931
struct _tuple21 _tmp5F4;struct _tuple21 _tmp247=(_tmp5F4.f1=il1,((_tmp5F4.f2=il2,_tmp5F4)));enum Cyc_CfFlowInfo_InitLevel _tmp248;enum Cyc_CfFlowInfo_InitLevel _tmp249;enum Cyc_CfFlowInfo_InitLevel _tmp24A;enum Cyc_CfFlowInfo_InitLevel _tmp24B;_LLEA: _tmp248=_tmp247.f2;if(_tmp248 != Cyc_CfFlowInfo_NoneIL)goto _LLEC;_LLEB:
# 932
 goto _LLED;_LLEC: _tmp249=_tmp247.f1;if(_tmp249 != Cyc_CfFlowInfo_NoneIL)goto _LLEE;_LLED: return(env->fenv)->esc_none;_LLEE: _tmp24A=_tmp247.f2;if(_tmp24A != Cyc_CfFlowInfo_ThisIL)goto _LLF0;_LLEF:
# 933
 goto _LLF1;_LLF0: _tmp24B=_tmp247.f1;if(_tmp24B != Cyc_CfFlowInfo_ThisIL)goto _LLF2;_LLF1: return(env->fenv)->esc_this;_LLF2:;_LLF3:
# 934
 return(env->fenv)->esc_all;_LLE9:;}_LLE7:;_LLE8: {
# 937
struct _tuple21 _tmp5F5;struct _tuple21 _tmp24D=(_tmp5F5.f1=il1,((_tmp5F5.f2=il2,_tmp5F5)));enum Cyc_CfFlowInfo_InitLevel _tmp24E;enum Cyc_CfFlowInfo_InitLevel _tmp24F;enum Cyc_CfFlowInfo_InitLevel _tmp250;enum Cyc_CfFlowInfo_InitLevel _tmp251;_LLF5: _tmp24E=_tmp24D.f2;if(_tmp24E != Cyc_CfFlowInfo_NoneIL)goto _LLF7;_LLF6:
# 938
 goto _LLF8;_LLF7: _tmp24F=_tmp24D.f1;if(_tmp24F != Cyc_CfFlowInfo_NoneIL)goto _LLF9;_LLF8: return(env->fenv)->unknown_none;_LLF9: _tmp250=_tmp24D.f2;if(_tmp250 != Cyc_CfFlowInfo_ThisIL)goto _LLFB;_LLFA:
# 939
 goto _LLFC;_LLFB: _tmp251=_tmp24D.f1;if(_tmp251 != Cyc_CfFlowInfo_ThisIL)goto _LLFD;_LLFC: return(env->fenv)->unknown_this;_LLFD:;_LLFE:
# 940
 return(env->fenv)->unknown_all;_LLF4:;}_LLE2:;};};}
# 945
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5F6;return((_tmp5F6.EqualConst).val=i,(((_tmp5F6.EqualConst).tag=1,_tmp5F6)));}
# 946
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*v,void*t){
# 947
struct _tuple11 _tmp5F9;union Cyc_CfFlowInfo_RelnOp _tmp5F8;return((_tmp5F8.LessVar).val=((_tmp5F9.f1=v,((_tmp5F9.f2=t,_tmp5F9)))),(((_tmp5F8.LessVar).tag=2,_tmp5F8)));}
# 949
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5FA;return((_tmp5FA.LessNumelts).val=v,(((_tmp5FA.LessNumelts).tag=3,_tmp5FA)));}
# 950
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5FB;return((_tmp5FB.LessConst).val=i,(((_tmp5FB.LessConst).tag=4,_tmp5FB)));}
# 951
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5FC;return((_tmp5FC.LessEqConst).val=i,(((_tmp5FC.LessEqConst).tag=6,_tmp5FC)));}
# 952
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*v){
# 953
union Cyc_CfFlowInfo_RelnOp _tmp5FD;return((_tmp5FD.LessEqNumelts).val=v,(((_tmp5FD.LessEqNumelts).tag=5,_tmp5FD)));}struct _tuple22{union Cyc_CfFlowInfo_RelnOp f1;union Cyc_CfFlowInfo_RelnOp f2;};
# 957
static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp r1,union Cyc_CfFlowInfo_RelnOp r2){
# 958
struct _tuple22 _tmp5FE;struct _tuple22 _tmp25A=(_tmp5FE.f1=r1,((_tmp5FE.f2=r2,_tmp5FE)));union Cyc_CfFlowInfo_RelnOp _tmp25B;unsigned int _tmp25C;union Cyc_CfFlowInfo_RelnOp _tmp25D;unsigned int _tmp25E;union Cyc_CfFlowInfo_RelnOp _tmp25F;struct _tuple11 _tmp260;struct Cyc_Absyn_Vardecl*_tmp261;union Cyc_CfFlowInfo_RelnOp _tmp262;struct _tuple11 _tmp263;struct Cyc_Absyn_Vardecl*_tmp264;union Cyc_CfFlowInfo_RelnOp _tmp265;struct Cyc_Absyn_Vardecl*_tmp266;union Cyc_CfFlowInfo_RelnOp _tmp267;struct Cyc_Absyn_Vardecl*_tmp268;union Cyc_CfFlowInfo_RelnOp _tmp269;unsigned int _tmp26A;union Cyc_CfFlowInfo_RelnOp _tmp26B;unsigned int _tmp26C;union Cyc_CfFlowInfo_RelnOp _tmp26D;unsigned int _tmp26E;union Cyc_CfFlowInfo_RelnOp _tmp26F;unsigned int _tmp270;union Cyc_CfFlowInfo_RelnOp _tmp271;struct Cyc_Absyn_Vardecl*_tmp272;union Cyc_CfFlowInfo_RelnOp _tmp273;struct Cyc_Absyn_Vardecl*_tmp274;_LL100: _tmp25B=_tmp25A.f1;if((_tmp25B.EqualConst).tag != 1)goto _LL102;_tmp25C=(unsigned int)(_tmp25B.EqualConst).val;_tmp25D=_tmp25A.f2;if((_tmp25D.EqualConst).tag != 1)goto _LL102;_tmp25E=(unsigned int)(_tmp25D.EqualConst).val;_LL101:
# 959
 return _tmp25C == _tmp25E;_LL102: _tmp25F=_tmp25A.f1;if((_tmp25F.LessVar).tag != 2)goto _LL104;_tmp260=(struct _tuple11)(_tmp25F.LessVar).val;_tmp261=_tmp260.f1;_tmp262=_tmp25A.f2;if((_tmp262.LessVar).tag != 2)goto _LL104;_tmp263=(struct _tuple11)(_tmp262.LessVar).val;_tmp264=_tmp263.f1;_LL103:
# 960
 return _tmp261 == _tmp264;_LL104: _tmp265=_tmp25A.f1;if((_tmp265.LessNumelts).tag != 3)goto _LL106;_tmp266=(struct Cyc_Absyn_Vardecl*)(_tmp265.LessNumelts).val;_tmp267=_tmp25A.f2;if((_tmp267.LessNumelts).tag != 3)goto _LL106;_tmp268=(struct Cyc_Absyn_Vardecl*)(_tmp267.LessNumelts).val;_LL105:
# 961
 return _tmp266 == _tmp268;_LL106: _tmp269=_tmp25A.f1;if((_tmp269.LessConst).tag != 4)goto _LL108;_tmp26A=(unsigned int)(_tmp269.LessConst).val;_tmp26B=_tmp25A.f2;if((_tmp26B.LessConst).tag != 4)goto _LL108;_tmp26C=(unsigned int)(_tmp26B.LessConst).val;_LL107:
# 962
 return _tmp26A == _tmp26C;_LL108: _tmp26D=_tmp25A.f1;if((_tmp26D.LessEqConst).tag != 6)goto _LL10A;_tmp26E=(unsigned int)(_tmp26D.LessEqConst).val;_tmp26F=_tmp25A.f2;if((_tmp26F.LessEqConst).tag != 6)goto _LL10A;_tmp270=(unsigned int)(_tmp26F.LessEqConst).val;_LL109:
# 963
 return _tmp26E == _tmp270;_LL10A: _tmp271=_tmp25A.f1;if((_tmp271.LessEqNumelts).tag != 5)goto _LL10C;_tmp272=(struct Cyc_Absyn_Vardecl*)(_tmp271.LessEqNumelts).val;_tmp273=_tmp25A.f2;if((_tmp273.LessEqNumelts).tag != 5)goto _LL10C;_tmp274=(struct Cyc_Absyn_Vardecl*)(_tmp273.LessEqNumelts).val;_LL10B:
# 964
 return _tmp272 == _tmp274;_LL10C:;_LL10D:
# 965
 return 0;_LLFF:;}
# 968
static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
# 969
if(r1s == r2s)return r1s;{
# 972
struct Cyc_List_List*res=0;
# 973
int diff=0;
# 974
{struct Cyc_List_List*_tmp275=r1s;for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
# 975
struct Cyc_CfFlowInfo_Reln*_tmp276=(struct Cyc_CfFlowInfo_Reln*)_tmp275->hd;
# 976
int found=0;
# 977
{struct Cyc_List_List*_tmp277=r2s;for(0;_tmp277 != 0;_tmp277=_tmp277->tl){
# 978
struct Cyc_CfFlowInfo_Reln*_tmp278=(struct Cyc_CfFlowInfo_Reln*)_tmp277->hd;
# 979
if(_tmp276 == _tmp278  || _tmp276->vd == _tmp278->vd  && Cyc_CfFlowInfo_same_relop(_tmp276->rop,_tmp278->rop)){
# 980
{struct Cyc_List_List*_tmp5FF;res=((_tmp5FF=_region_malloc(r,sizeof(*_tmp5FF)),((_tmp5FF->hd=_tmp276,((_tmp5FF->tl=res,_tmp5FF))))));}
# 981
found=1;
# 982
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
struct _tuple23 _tmp600;struct _tuple23 _tmp27B=(_tmp600.f1=f1,((_tmp600.f2=f2,_tmp600)));union Cyc_CfFlowInfo_FlowInfo _tmp27C;int _tmp27D;union Cyc_CfFlowInfo_FlowInfo _tmp27E;int _tmp27F;union Cyc_CfFlowInfo_FlowInfo _tmp280;struct _tuple12 _tmp281;struct Cyc_Dict_Dict _tmp282;struct Cyc_List_List*_tmp283;union Cyc_CfFlowInfo_FlowInfo _tmp284;struct _tuple12 _tmp285;struct Cyc_Dict_Dict _tmp286;struct Cyc_List_List*_tmp287;_LL10F: _tmp27C=_tmp27B.f1;if((_tmp27C.BottomFL).tag != 1)goto _LL111;_tmp27D=(int)(_tmp27C.BottomFL).val;_LL110:
# 996
 return f2;_LL111: _tmp27E=_tmp27B.f2;if((_tmp27E.BottomFL).tag != 1)goto _LL113;_tmp27F=(int)(_tmp27E.BottomFL).val;_LL112:
# 997
 return f1;_LL113: _tmp280=_tmp27B.f1;if((_tmp280.ReachableFL).tag != 2)goto _LL10E;_tmp281=(struct _tuple12)(_tmp280.ReachableFL).val;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;_tmp284=_tmp27B.f2;if((_tmp284.ReachableFL).tag != 2)goto _LL10E;_tmp285=(struct _tuple12)(_tmp284.ReachableFL).val;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;_LL114:
# 1001
 if(_tmp282.t == _tmp286.t  && _tmp283 == _tmp287)return f1;
# 1002
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
# 1003
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
# 1004
struct _RegionHandle*_tmp288=fenv->r;
# 1005
struct Cyc_CfFlowInfo_EscPile*_tmp603;struct Cyc_CfFlowInfo_JoinEnv _tmp602;struct Cyc_CfFlowInfo_JoinEnv _tmp289=(_tmp602.fenv=fenv,((_tmp602.pile=((_tmp603=_region_malloc(_tmp288,sizeof(*_tmp603)),((_tmp603->rgn=_tmp288,((_tmp603->places=0,_tmp603)))))),((_tmp602.d1=_tmp282,((_tmp602.d2=_tmp286,_tmp602)))))));
# 1006
struct Cyc_Dict_Dict _tmp28A=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp289,_tmp282,_tmp286);
# 1007
struct Cyc_List_List*_tmp28B=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp283,_tmp287);
# 1008
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp289.pile,all_changed,_tmp28A),_tmp28B);};_LL10E:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1013
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1020
union Cyc_CfFlowInfo_FlowInfo _tmp28F;void*_tmp290;struct _tuple13 _tmp28E=pr1;_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;{
# 1021
union Cyc_CfFlowInfo_FlowInfo _tmp292;void*_tmp293;struct _tuple13 _tmp291=pr2;_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;{
# 1022
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp28F,_tmp292);
# 1023
struct _tuple24 _tmp604;struct _tuple24 _tmp295=(_tmp604.f1=_tmp28F,((_tmp604.f2=_tmp292,((_tmp604.f3=outflow,_tmp604)))));union Cyc_CfFlowInfo_FlowInfo _tmp296;int _tmp297;union Cyc_CfFlowInfo_FlowInfo _tmp298;int _tmp299;union Cyc_CfFlowInfo_FlowInfo _tmp29A;struct _tuple12 _tmp29B;struct Cyc_Dict_Dict _tmp29C;union Cyc_CfFlowInfo_FlowInfo _tmp29D;struct _tuple12 _tmp29E;struct Cyc_Dict_Dict _tmp29F;union Cyc_CfFlowInfo_FlowInfo _tmp2A0;struct _tuple12 _tmp2A1;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*_tmp2A3;_LL116: _tmp296=_tmp295.f1;if((_tmp296.BottomFL).tag != 1)goto _LL118;_tmp297=(int)(_tmp296.BottomFL).val;_LL117: {
# 1024
struct _tuple13 _tmp605;return(_tmp605.f1=outflow,((_tmp605.f2=_tmp293,_tmp605)));}_LL118: _tmp298=_tmp295.f2;if((_tmp298.BottomFL).tag != 1)goto _LL11A;_tmp299=(int)(_tmp298.BottomFL).val;_LL119: {
# 1025
struct _tuple13 _tmp606;return(_tmp606.f1=outflow,((_tmp606.f2=_tmp290,_tmp606)));}_LL11A: _tmp29A=_tmp295.f1;if((_tmp29A.ReachableFL).tag != 2)goto _LL11C;_tmp29B=(struct _tuple12)(_tmp29A.ReachableFL).val;_tmp29C=_tmp29B.f1;_tmp29D=_tmp295.f2;if((_tmp29D.ReachableFL).tag != 2)goto _LL11C;_tmp29E=(struct _tuple12)(_tmp29D.ReachableFL).val;_tmp29F=_tmp29E.f1;_tmp2A0=_tmp295.f3;if((_tmp2A0.ReachableFL).tag != 2)goto _LL11C;_tmp2A1=(struct _tuple12)(_tmp2A0.ReachableFL).val;_tmp2A2=_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;_LL11B:
# 1028
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp290,_tmp293)){struct _tuple13 _tmp607;return(_tmp607.f1=outflow,((_tmp607.f2=_tmp293,_tmp607)));}
# 1029
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp293,_tmp290)){struct _tuple13 _tmp608;return(_tmp608.f1=outflow,((_tmp608.f2=_tmp290,_tmp608)));}{
# 1030
struct _RegionHandle*_tmp2A8=fenv->r;
# 1031
struct Cyc_CfFlowInfo_EscPile*_tmp60B;struct Cyc_CfFlowInfo_JoinEnv _tmp60A;struct Cyc_CfFlowInfo_JoinEnv _tmp2A9=(_tmp60A.fenv=fenv,((_tmp60A.pile=((_tmp60B=_region_malloc(_tmp2A8,sizeof(*_tmp60B)),((_tmp60B->rgn=_tmp2A8,((_tmp60B->places=0,_tmp60B)))))),((_tmp60A.d1=_tmp29C,((_tmp60A.d2=_tmp29F,_tmp60A)))))));
# 1032
void*_tmp2AA=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp2A9,0,_tmp290,_tmp293);
# 1033
struct _tuple13 _tmp60C;return(_tmp60C.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp2A9.pile,all_changed,_tmp2A2),_tmp2A3),((_tmp60C.f2=_tmp2AA,_tmp60C)));};_LL11C:;_LL11D: {
# 1036
struct Cyc_Core_Impossible_exn_struct _tmp612;const char*_tmp611;struct Cyc_Core_Impossible_exn_struct*_tmp610;(int)_throw((void*)((_tmp610=_cycalloc(sizeof(*_tmp610)),((_tmp610[0]=((_tmp612.tag=Cyc_Core_Impossible,((_tmp612.f1=((_tmp611="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp611,sizeof(char),37))),_tmp612)))),_tmp610)))));}_LL115:;};};}
# 1040
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp619(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp618,unsigned int*_tmp617,void***_tmp615,struct _dyneither_ptr*_tmp2C2,struct _dyneither_ptr*_tmp2BC){for(*_tmp618=0;*_tmp618 < *_tmp617;(*_tmp618)++){(*_tmp615)[*_tmp618]=
# 1069
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp618,((void**)(*_tmp2BC).curr)[(int)*_tmp618],*((void**)_check_dyneither_subscript(*_tmp2C2,sizeof(void*),(int)*_tmp618)));}}
# 1041
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
# 1042
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
# 1043
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
# 1044
if(changed1  && changed2)
# 1045
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1049
if(changed1){
# 1050
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
# 1051
return r1;
# 1052
env->changed=Cyc_CfFlowInfo_One;}
# 1054
if(changed2){
# 1055
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
# 1056
return r2;
# 1057
env->changed=Cyc_CfFlowInfo_Two;}{
# 1062
struct _tuple0 _tmp613;struct _tuple0 _tmp2B2=(_tmp613.f1=r1,((_tmp613.f2=r2,_tmp613)));void*_tmp2B3;void*_tmp2B5;void*_tmp2B7;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B9;int _tmp2BA;int _tmp2BB;struct _dyneither_ptr _tmp2BC;void*_tmp2BD;struct Cyc_CfFlowInfo_UnionRInfo _tmp2BF;int _tmp2C0;int _tmp2C1;struct _dyneither_ptr _tmp2C2;_LL11F: _tmp2B3=_tmp2B2.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2B3;if(_tmp2B4->tag != 7)goto _LL121;};_LL120:
# 1063
 goto _LL122;_LL121: _tmp2B5=_tmp2B2.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp2B6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2B5;if(_tmp2B6->tag != 7)goto _LL123;};_LL122:
# 1064
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL123: _tmp2B7=_tmp2B2.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B7;if(_tmp2B8->tag != 6)goto _LL125;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.is_union;_tmp2BB=_tmp2B9.fieldnum;_tmp2BC=_tmp2B8->f2;}};_tmp2BD=_tmp2B2.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2BE=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2BD;if(_tmp2BE->tag != 6)goto _LL125;else{_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BF.is_union;_tmp2C1=_tmp2BF.fieldnum;_tmp2C2=_tmp2BE->f2;}};_LL124: {
# 1067
unsigned int _tmp618;unsigned int _tmp617;struct _dyneither_ptr _tmp616;void**_tmp615;unsigned int _tmp614;struct _dyneither_ptr new_d=
# 1068
(_tmp614=
# 1069
_get_dyneither_size(_tmp2BC,sizeof(void*)),((_tmp615=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp614)),((_tmp616=_tag_dyneither(_tmp615,sizeof(void*),_tmp614),((((_tmp617=_tmp614,_tmp619(& env,& _tmp618,& _tmp617,& _tmp615,& _tmp2C2,& _tmp2BC))),_tmp616)))))));
# 1074
int change=0;
# 1075
{int i=0;for(0;i < _get_dyneither_size(_tmp2BC,sizeof(void*));++ i){
# 1076
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp2BC.curr)[i]){
# 1077
change=1;break;}}}
# 1079
if(!change)
# 1080
return r1;
# 1081
change=0;
# 1082
{int i=0;for(0;i < _get_dyneither_size(_tmp2BC,sizeof(void*));++ i){
# 1083
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp2C2,sizeof(void*),i))){
# 1084
change=1;break;}}}
# 1086
if(!change)
# 1087
return r2;{
# 1090
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp61F;struct Cyc_CfFlowInfo_UnionRInfo _tmp61E;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp61D;return(void*)((_tmp61D=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp61D)),((_tmp61D[0]=((_tmp61F.tag=6,((_tmp61F.f1=((_tmp61E.is_union=_tmp2BA,((_tmp61E.fieldnum=_tmp2BB == _tmp2C1?_tmp2BB: - 1,_tmp61E)))),((_tmp61F.f2=new_d,_tmp61F)))))),_tmp61D))));};}_LL125:;_LL126: {
# 1098
struct Cyc_Core_Impossible_exn_struct _tmp625;const char*_tmp624;struct Cyc_Core_Impossible_exn_struct*_tmp623;(int)_throw((void*)((_tmp623=_cycalloc(sizeof(*_tmp623)),((_tmp623[0]=((_tmp625.tag=Cyc_Core_Impossible,((_tmp625.f1=((_tmp624="after_absRval -- non-aggregates!",_tag_dyneither(_tmp624,sizeof(char),33))),_tmp625)))),_tmp623)))));}_LL11E:;};}
# 1101
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1104
if(r1 == r2)return r1;{
# 1105
struct Cyc_List_List**_tmp2CD=env->last_field_cell;
# 1106
enum Cyc_CfFlowInfo_WhoIsChanged _tmp2CE=env->changed;
# 1107
{struct Cyc_List_List*_tmp626;*env->last_field_cell=((_tmp626=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp626)),((_tmp626->hd=(void*)key,((_tmp626->tl=0,_tmp626))))));}
# 1110
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
# 1111
void*_tmp2D0=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
# 1112
env->last_field_cell=_tmp2CD;
# 1113
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
# 1114
env->changed=_tmp2CE;
# 1115
return _tmp2D0;};};}
# 1117
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1119
if(r1 == r2)return r1;
# 1120
{struct Cyc_CfFlowInfo_Place _tmp627;*env->curr_place=((_tmp627.root=root,((_tmp627.fields=0,_tmp627))));}
# 1121
env->last_field_cell=&(env->curr_place)->fields;
# 1122
env->changed=Cyc_CfFlowInfo_Neither;
# 1123
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1126
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1132
struct _tuple23 _tmp628;struct _tuple23 _tmp2D3=(_tmp628.f1=f1,((_tmp628.f2=f2,_tmp628)));union Cyc_CfFlowInfo_FlowInfo _tmp2D4;int _tmp2D5;union Cyc_CfFlowInfo_FlowInfo _tmp2D6;int _tmp2D7;union Cyc_CfFlowInfo_FlowInfo _tmp2D8;struct _tuple12 _tmp2D9;struct Cyc_Dict_Dict _tmp2DA;struct Cyc_List_List*_tmp2DB;union Cyc_CfFlowInfo_FlowInfo _tmp2DC;struct _tuple12 _tmp2DD;struct Cyc_Dict_Dict _tmp2DE;struct Cyc_List_List*_tmp2DF;_LL128: _tmp2D4=_tmp2D3.f1;if((_tmp2D4.BottomFL).tag != 1)goto _LL12A;_tmp2D5=(int)(_tmp2D4.BottomFL).val;_LL129:
# 1133
 goto _LL12B;_LL12A: _tmp2D6=_tmp2D3.f2;if((_tmp2D6.BottomFL).tag != 1)goto _LL12C;_tmp2D7=(int)(_tmp2D6.BottomFL).val;_LL12B:
# 1134
 return Cyc_CfFlowInfo_BottomFL();_LL12C: _tmp2D8=_tmp2D3.f1;if((_tmp2D8.ReachableFL).tag != 2)goto _LL127;_tmp2D9=(struct _tuple12)(_tmp2D8.ReachableFL).val;_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;_tmp2DC=_tmp2D3.f2;if((_tmp2DC.ReachableFL).tag != 2)goto _LL127;_tmp2DD=(struct _tuple12)(_tmp2DC.ReachableFL).val;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;_LL12D:
# 1136
 if(_tmp2DA.t == _tmp2DE.t  && _tmp2DB == _tmp2DF)return f1;{
# 1139
struct _RegionHandle*_tmp2E0=fenv->r;
# 1140
struct Cyc_CfFlowInfo_Place*_tmp2E1=fenv->dummy_place;
# 1141
struct Cyc_CfFlowInfo_JoinEnv _tmp62E;struct Cyc_CfFlowInfo_EscPile*_tmp62D;struct Cyc_CfFlowInfo_AfterEnv _tmp62C;struct Cyc_CfFlowInfo_AfterEnv _tmp2E2=(_tmp62C.joinenv=((_tmp62E.fenv=fenv,((_tmp62E.pile=((_tmp62D=_region_malloc(_tmp2E0,sizeof(*_tmp62D)),((_tmp62D->rgn=_tmp2E0,((_tmp62D->places=0,_tmp62D)))))),((_tmp62E.d1=_tmp2DA,((_tmp62E.d2=_tmp2DE,_tmp62E)))))))),((_tmp62C.chg1=chg1,((_tmp62C.chg2=chg2,((_tmp62C.curr_place=_tmp2E1,((_tmp62C.last_field_cell=& _tmp2E1->fields,((_tmp62C.changed=Cyc_CfFlowInfo_Neither,_tmp62C)))))))))));
# 1145
struct Cyc_Dict_Dict _tmp2E3=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2E2,_tmp2DA,_tmp2DE);
# 1146
struct Cyc_List_List*_tmp2E4=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp2DB,_tmp2DF);
# 1147
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2E2.joinenv).pile,all_changed,_tmp2E3),_tmp2E4);};_LL127:;}
# 1154
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
# 1155
if(r1 == r2)return 1;
# 1157
{struct _tuple0 _tmp62F;struct _tuple0 _tmp2E9=(_tmp62F.f1=r1,((_tmp62F.f2=r2,_tmp62F)));void*_tmp2EA;struct Cyc_CfFlowInfo_Place*_tmp2EC;void*_tmp2ED;struct Cyc_CfFlowInfo_Place*_tmp2EF;void*_tmp2F0;void*_tmp2F2;void*_tmp2F4;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F6;struct _dyneither_ptr _tmp2F7;void*_tmp2F8;struct Cyc_CfFlowInfo_UnionRInfo _tmp2FA;struct _dyneither_ptr _tmp2FB;void*_tmp2FC;void*_tmp2FE;void*_tmp300;void*_tmp302;void*_tmp304;void*_tmp306;void*_tmp308;void*_tmp30A;_LL12F: _tmp2EA=_tmp2E9.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2EB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2EA;if(_tmp2EB->tag != 5)goto _LL131;else{_tmp2EC=_tmp2EB->f1;}};_tmp2ED=_tmp2E9.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2EE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2ED;if(_tmp2EE->tag != 5)goto _LL131;else{_tmp2EF=_tmp2EE->f1;}};_LL130:
# 1158
 return Cyc_CfFlowInfo_place_cmp(_tmp2EC,_tmp2EF)== 0;_LL131: _tmp2F0=_tmp2E9.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F0;if(_tmp2F1->tag != 5)goto _LL133;};_LL132:
# 1159
 goto _LL134;_LL133: _tmp2F2=_tmp2E9.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F2;if(_tmp2F3->tag != 5)goto _LL135;};_LL134:
# 1160
 return 0;_LL135: _tmp2F4=_tmp2E9.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F4;if(_tmp2F5->tag != 6)goto _LL137;else{_tmp2F6=_tmp2F5->f1;_tmp2F7=_tmp2F5->f2;}};_tmp2F8=_tmp2E9.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F8;if(_tmp2F9->tag != 6)goto _LL137;else{_tmp2FA=_tmp2F9->f1;_tmp2FB=_tmp2F9->f2;}};_LL136:
# 1162
 if((void**)_tmp2F7.curr == (void**)_tmp2FB.curr)return 1;
# 1163
{int i=0;for(0;i < _get_dyneither_size(_tmp2F7,sizeof(void*));++ i){
# 1164
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2F7.curr)[i],*((void**)_check_dyneither_subscript(_tmp2FB,sizeof(void*),i))))return 0;}}
# 1165
return 1;_LL137: _tmp2FC=_tmp2E9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2FD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2FC;if(_tmp2FD->tag != 1)goto _LL139;};_tmp2FE=_tmp2E9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2FF=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2FE;if(_tmp2FF->tag != 2)goto _LL139;};_LL138:
# 1166
 return 1;_LL139: _tmp300=_tmp2E9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp301=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp300;if(_tmp301->tag != 2)goto _LL13B;};_LL13A:
# 1167
 return 0;_LL13B: _tmp302=_tmp2E9.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp303=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL13D;};_LL13C:
# 1168
 goto _LL13E;_LL13D: _tmp304=_tmp2E9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp305=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp304;if(_tmp305->tag != 1)goto _LL13F;};_LL13E:
# 1169
 return 0;_LL13F: _tmp306=_tmp2E9.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp307=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp306;if(_tmp307->tag != 4)goto _LL141;};_tmp308=_tmp2E9.f2;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp309=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp308;if(_tmp309->tag != 4)goto _LL141;};_LL140:
# 1170
 goto _LL12E;_LL141: _tmp30A=_tmp2E9.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp30B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp30A;if(_tmp30B->tag != 4)goto _LL143;};_LL142:
# 1171
 return 0;_LL143:;_LL144:
# 1172
 goto _LL12E;_LL12E:;}{
# 1174
struct _tuple21 _tmp630;struct _tuple21 _tmp30D=(_tmp630.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp630.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp630)));enum Cyc_CfFlowInfo_InitLevel _tmp30E;enum Cyc_CfFlowInfo_InitLevel _tmp30F;enum Cyc_CfFlowInfo_InitLevel _tmp310;enum Cyc_CfFlowInfo_InitLevel _tmp311;enum Cyc_CfFlowInfo_InitLevel _tmp312;enum Cyc_CfFlowInfo_InitLevel _tmp313;_LL146: _tmp30E=_tmp30D.f1;if(_tmp30E != Cyc_CfFlowInfo_AllIL)goto _LL148;_tmp30F=_tmp30D.f2;if(_tmp30F != Cyc_CfFlowInfo_AllIL)goto _LL148;_LL147:
# 1175
 return 1;_LL148: _tmp310=_tmp30D.f2;if(_tmp310 != Cyc_CfFlowInfo_NoneIL)goto _LL14A;_LL149:
# 1176
 return 1;_LL14A: _tmp311=_tmp30D.f1;if(_tmp311 != Cyc_CfFlowInfo_NoneIL)goto _LL14C;_LL14B:
# 1177
 return 0;_LL14C: _tmp312=_tmp30D.f2;if(_tmp312 != Cyc_CfFlowInfo_ThisIL)goto _LL14E;_LL14D:
# 1178
 return 1;_LL14E: _tmp313=_tmp30D.f1;if(_tmp313 != Cyc_CfFlowInfo_ThisIL)goto _LL145;_LL14F:
# 1179
 return 0;_LL145:;};}
# 1183
static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
# 1184
if(r1s == r2s)return 1;
# 1187
for(0;r1s != 0;r1s=r1s->tl){
# 1188
struct Cyc_CfFlowInfo_Reln*_tmp314=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
# 1189
int found=0;
# 1190
{struct Cyc_List_List*_tmp315=r2s;for(0;_tmp315 != 0;_tmp315=_tmp315->tl){
# 1191
struct Cyc_CfFlowInfo_Reln*_tmp316=(struct Cyc_CfFlowInfo_Reln*)_tmp315->hd;
# 1192
if(_tmp314 == _tmp316  || _tmp314->vd == _tmp316->vd  && Cyc_CfFlowInfo_same_relop(_tmp314->rop,_tmp316->rop)){
# 1193
found=1;
# 1194
break;}}}
# 1197
if(!found)
# 1198
return 0;}
# 1200
return 1;}
# 1208
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1210
struct _tuple23 _tmp631;struct _tuple23 _tmp318=(_tmp631.f1=f1,((_tmp631.f2=f2,_tmp631)));union Cyc_CfFlowInfo_FlowInfo _tmp319;int _tmp31A;union Cyc_CfFlowInfo_FlowInfo _tmp31B;int _tmp31C;union Cyc_CfFlowInfo_FlowInfo _tmp31D;struct _tuple12 _tmp31E;struct Cyc_Dict_Dict _tmp31F;struct Cyc_List_List*_tmp320;union Cyc_CfFlowInfo_FlowInfo _tmp321;struct _tuple12 _tmp322;struct Cyc_Dict_Dict _tmp323;struct Cyc_List_List*_tmp324;_LL151: _tmp319=_tmp318.f1;if((_tmp319.BottomFL).tag != 1)goto _LL153;_tmp31A=(int)(_tmp319.BottomFL).val;_LL152:
# 1211
 return 1;_LL153: _tmp31B=_tmp318.f2;if((_tmp31B.BottomFL).tag != 1)goto _LL155;_tmp31C=(int)(_tmp31B.BottomFL).val;_LL154:
# 1212
 return 0;_LL155: _tmp31D=_tmp318.f1;if((_tmp31D.ReachableFL).tag != 2)goto _LL150;_tmp31E=(struct _tuple12)(_tmp31D.ReachableFL).val;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;_tmp321=_tmp318.f2;if((_tmp321.ReachableFL).tag != 2)goto _LL150;_tmp322=(struct _tuple12)(_tmp321.ReachableFL).val;_tmp323=_tmp322.f1;_tmp324=_tmp322.f2;_LL156:
# 1214
 if(_tmp31F.t == _tmp323.t  && _tmp320 == _tmp324)return 1;
# 1215
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp31F,_tmp323) && 
# 1216
Cyc_CfFlowInfo_relns_approx(_tmp320,_tmp324);_LL150:;}
# 1220
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
# 1221
struct Cyc_List_List*p;
# 1222
int found=0;
# 1223
for(p=rs;!found  && p != 0;p=p->tl){
# 1224
struct Cyc_CfFlowInfo_Reln*_tmp325=(struct Cyc_CfFlowInfo_Reln*)p->hd;
# 1225
if(_tmp325->vd == v){found=1;break;}{
# 1226
union Cyc_CfFlowInfo_RelnOp _tmp326=_tmp325->rop;struct _tuple11 _tmp327;struct Cyc_Absyn_Vardecl*_tmp328;struct Cyc_Absyn_Vardecl*_tmp329;struct Cyc_Absyn_Vardecl*_tmp32A;_LL158: if((_tmp326.LessVar).tag != 2)goto _LL15A;_tmp327=(struct _tuple11)(_tmp326.LessVar).val;_tmp328=_tmp327.f1;_LL159:
# 1227
 _tmp329=_tmp328;goto _LL15B;_LL15A: if((_tmp326.LessNumelts).tag != 3)goto _LL15C;_tmp329=(struct Cyc_Absyn_Vardecl*)(_tmp326.LessNumelts).val;_LL15B:
# 1228
 _tmp32A=_tmp329;goto _LL15D;_LL15C: if((_tmp326.LessEqNumelts).tag != 5)goto _LL15E;_tmp32A=(struct Cyc_Absyn_Vardecl*)(_tmp326.LessEqNumelts).val;_LL15D:
# 1229
 if(v == _tmp32A)found=1;goto _LL157;_LL15E:;_LL15F:
# 1230
 goto _LL157;_LL157:;};}
# 1233
if(!found)return rs;{
# 1235
struct Cyc_List_List*_tmp32B=0;
# 1236
for(p=rs;p != 0;p=p->tl){
# 1237
struct Cyc_CfFlowInfo_Reln*_tmp32C=(struct Cyc_CfFlowInfo_Reln*)p->hd;
# 1238
if(_tmp32C->vd != v){
# 1239
{union Cyc_CfFlowInfo_RelnOp _tmp32D=_tmp32C->rop;struct _tuple11 _tmp32E;struct Cyc_Absyn_Vardecl*_tmp32F;struct Cyc_Absyn_Vardecl*_tmp330;struct Cyc_Absyn_Vardecl*_tmp331;_LL161: if((_tmp32D.LessVar).tag != 2)goto _LL163;_tmp32E=(struct _tuple11)(_tmp32D.LessVar).val;_tmp32F=_tmp32E.f1;_LL162:
# 1240
 _tmp330=_tmp32F;goto _LL164;_LL163: if((_tmp32D.LessNumelts).tag != 3)goto _LL165;_tmp330=(struct Cyc_Absyn_Vardecl*)(_tmp32D.LessNumelts).val;_LL164:
# 1241
 _tmp331=_tmp330;goto _LL166;_LL165: if((_tmp32D.LessEqNumelts).tag != 5)goto _LL167;_tmp331=(struct Cyc_Absyn_Vardecl*)(_tmp32D.LessEqNumelts).val;_LL166:
# 1242
 if(v == _tmp331)continue;goto _LL160;_LL167:;_LL168:
# 1243
 goto _LL160;_LL160:;}{
# 1245
struct Cyc_List_List*_tmp632;_tmp32B=((_tmp632=_region_malloc(rgn,sizeof(*_tmp632)),((_tmp632->hd=_tmp32C,((_tmp632->tl=_tmp32B,_tmp632))))));};}}
# 1248
return _tmp32B;};}
# 1251
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
# 1252
{void*_tmp333=e->r;void*_tmp335;struct Cyc_Absyn_Vardecl*_tmp337;void*_tmp339;struct Cyc_Absyn_Vardecl*_tmp33B;void*_tmp33D;struct Cyc_Absyn_Vardecl*_tmp33F;void*_tmp341;struct Cyc_Absyn_Vardecl*_tmp343;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp334=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp333;if(_tmp334->tag != 1)goto _LL16C;else{_tmp335=(void*)_tmp334->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp336=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp335;if(_tmp336->tag != 1)goto _LL16C;else{_tmp337=_tmp336->f1;}};}}_LL16B:
# 1253
 _tmp33B=_tmp337;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp338=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp333;if(_tmp338->tag != 1)goto _LL16E;else{_tmp339=(void*)_tmp338->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp33A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp339;if(_tmp33A->tag != 3)goto _LL16E;else{_tmp33B=_tmp33A->f1;}};}}_LL16D:
# 1254
 _tmp33F=_tmp33B;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp333;if(_tmp33C->tag != 1)goto _LL170;else{_tmp33D=(void*)_tmp33C->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp33E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp33D;if(_tmp33E->tag != 4)goto _LL170;else{_tmp33F=_tmp33E->f1;}};}}_LL16F:
# 1255
 _tmp343=_tmp33F;goto _LL171;_LL170: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp340=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp333;if(_tmp340->tag != 1)goto _LL172;else{_tmp341=(void*)_tmp340->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp342=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp341;if(_tmp342->tag != 5)goto _LL172;else{_tmp343=_tmp342->f1;}};}}_LL171:
# 1257
 if(!_tmp343->escapes)
# 1258
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp343);
# 1259
goto _LL169;_LL172:;_LL173:
# 1260
 goto _LL169;_LL169:;}
# 1262
return r;}
# 1265
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
# 1266
struct Cyc_CfFlowInfo_Reln*_tmp633;return(_tmp633=_region_malloc(r2,sizeof(*_tmp633)),((_tmp633[0]=*r,_tmp633)));}
# 1269
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
# 1270
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
# 1273
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
# 1274
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
# 1275
struct Cyc_CfFlowInfo_Reln*_tmp345=(struct Cyc_CfFlowInfo_Reln*)r1->hd;
# 1276
struct Cyc_CfFlowInfo_Reln*_tmp346=(struct Cyc_CfFlowInfo_Reln*)r2->hd;
# 1277
if(_tmp345->vd != _tmp346->vd  || !Cyc_CfFlowInfo_same_relop(_tmp345->rop,_tmp346->rop))return 0;}
# 1279
if(r1 != 0  || r2 != 0)return 0;else{
# 1280
return 1;}}
# 1283
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 1286
if(v->escapes)return r;
# 1289
r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);
# 1294
{void*_tmp347=e->r;struct Cyc_Absyn_MallocInfo _tmp349;struct Cyc_Absyn_Exp*_tmp34A;int _tmp34B;_LL175: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp348=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp347;if(_tmp348->tag != 32)goto _LL177;else{_tmp349=_tmp348->f1;_tmp34A=_tmp349.num_elts;_tmp34B=_tmp349.fat_result;if(_tmp34B != 1)goto _LL177;}}_LL176:
# 1296
 malloc_loop: {
# 1297
void*_tmp34C=_tmp34A->r;struct Cyc_Absyn_Exp*_tmp34E;void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;void*_tmp354;struct Cyc_Absyn_Vardecl*_tmp356;void*_tmp358;struct Cyc_Absyn_Vardecl*_tmp35A;void*_tmp35C;struct Cyc_Absyn_Vardecl*_tmp35E;_LL17A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp34D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp34D->tag != 13)goto _LL17C;else{_tmp34E=_tmp34D->f2;}}_LL17B:
# 1298
 _tmp34A=_tmp34E;goto malloc_loop;_LL17C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp34F->tag != 1)goto _LL17E;else{_tmp350=(void*)_tmp34F->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp351=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp350;if(_tmp351->tag != 5)goto _LL17E;else{_tmp352=_tmp351->f1;}};}}_LL17D:
# 1299
 _tmp356=_tmp352;goto _LL17F;_LL17E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp353=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp353->tag != 1)goto _LL180;else{_tmp354=(void*)_tmp353->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp355=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp354;if(_tmp355->tag != 4)goto _LL180;else{_tmp356=_tmp355->f1;}};}}_LL17F:
# 1300
 _tmp35A=_tmp356;goto _LL181;_LL180: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp357=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp357->tag != 1)goto _LL182;else{_tmp358=(void*)_tmp357->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp359=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp358;if(_tmp359->tag != 3)goto _LL182;else{_tmp35A=_tmp359->f1;}};}}_LL181:
# 1301
 _tmp35E=_tmp35A;goto _LL183;_LL182: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp35B->tag != 1)goto _LL184;else{_tmp35C=(void*)_tmp35B->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp35D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp35C;if(_tmp35D->tag != 1)goto _LL184;else{_tmp35E=_tmp35D->f1;}};}}_LL183:
# 1303
 if(_tmp35E->escapes)return r;{
# 1304
struct Cyc_CfFlowInfo_Reln*_tmp636;struct Cyc_List_List*_tmp635;return(_tmp635=_region_malloc(rgn,sizeof(*_tmp635)),((_tmp635->hd=((_tmp636=_region_malloc(rgn,sizeof(*_tmp636)),((_tmp636->vd=_tmp35E,((_tmp636->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp636)))))),((_tmp635->tl=r,_tmp635)))));};_LL184:;_LL185:
# 1305
 return r;_LL179:;}_LL177:;_LL178:
# 1307
 goto _LL174;_LL174:;}
# 1310
{void*_tmp361=Cyc_Tcutil_compress(v->type);_LL187: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp362=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp361;if(_tmp362->tag != 6)goto _LL189;}_LL188:
# 1311
 goto _LL186;_LL189:;_LL18A:
# 1313
 return r;_LL186:;}
# 1317
loop: {
# 1318
void*_tmp363=e->r;struct Cyc_Absyn_Exp*_tmp365;union Cyc_Absyn_Cnst _tmp367;struct _tuple6 _tmp368;int _tmp369;enum Cyc_Absyn_Primop _tmp36B;struct Cyc_List_List*_tmp36C;struct Cyc_List_List _tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_List_List _tmp36F;struct Cyc_Absyn_Exp*_tmp370;enum Cyc_Absyn_Primop _tmp372;struct Cyc_List_List*_tmp373;struct Cyc_List_List _tmp374;struct Cyc_Absyn_Exp*_tmp375;_LL18C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp364=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp364->tag != 13)goto _LL18E;else{_tmp365=_tmp364->f2;}}_LL18D:
# 1319
 e=_tmp365;goto loop;_LL18E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp366->tag != 0)goto _LL190;else{_tmp367=_tmp366->f1;if((_tmp367.Int_c).tag != 5)goto _LL190;_tmp368=(struct _tuple6)(_tmp367.Int_c).val;_tmp369=_tmp368.f2;}}_LL18F: {
# 1321
struct Cyc_CfFlowInfo_Reln*_tmp639;struct Cyc_List_List*_tmp638;return(_tmp638=_region_malloc(rgn,sizeof(*_tmp638)),((_tmp638->hd=((_tmp639=_region_malloc(rgn,sizeof(*_tmp639)),((_tmp639->vd=v,((_tmp639->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp369),_tmp639)))))),((_tmp638->tl=r,_tmp638)))));}_LL190: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp36A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp36A->tag != 2)goto _LL192;else{_tmp36B=_tmp36A->f1;if(_tmp36B != Cyc_Absyn_Mod)goto _LL192;_tmp36C=_tmp36A->f2;if(_tmp36C == 0)goto _LL192;_tmp36D=*_tmp36C;_tmp36E=_tmp36D.tl;if(_tmp36E == 0)goto _LL192;_tmp36F=*_tmp36E;_tmp370=(struct Cyc_Absyn_Exp*)_tmp36F.hd;}}_LL191:
# 1325
{void*_tmp378=_tmp370->r;enum Cyc_Absyn_Primop _tmp37A;struct Cyc_List_List*_tmp37B;struct Cyc_List_List _tmp37C;struct Cyc_Absyn_Exp*_tmp37D;_LL197: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp379=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp378;if(_tmp379->tag != 2)goto _LL199;else{_tmp37A=_tmp379->f1;_tmp37B=_tmp379->f2;if(_tmp37B == 0)goto _LL199;_tmp37C=*_tmp37B;_tmp37D=(struct Cyc_Absyn_Exp*)_tmp37C.hd;}}_LL198:
# 1327
{void*_tmp37E=_tmp37D->r;void*_tmp380;struct Cyc_Absyn_Vardecl*_tmp382;void*_tmp384;struct Cyc_Absyn_Vardecl*_tmp386;void*_tmp388;struct Cyc_Absyn_Vardecl*_tmp38A;void*_tmp38C;struct Cyc_Absyn_Vardecl*_tmp38E;_LL19C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp37F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp37F->tag != 1)goto _LL19E;else{_tmp380=(void*)_tmp37F->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp381=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp380;if(_tmp381->tag != 1)goto _LL19E;else{_tmp382=_tmp381->f1;}};}}_LL19D:
# 1328
 _tmp386=_tmp382;goto _LL19F;_LL19E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp383=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp383->tag != 1)goto _LL1A0;else{_tmp384=(void*)_tmp383->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp385=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp384;if(_tmp385->tag != 4)goto _LL1A0;else{_tmp386=_tmp385->f1;}};}}_LL19F:
# 1329
 _tmp38A=_tmp386;goto _LL1A1;_LL1A0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp387=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp387->tag != 1)goto _LL1A2;else{_tmp388=(void*)_tmp387->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp389=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp388;if(_tmp389->tag != 3)goto _LL1A2;else{_tmp38A=_tmp389->f1;}};}}_LL1A1:
# 1330
 _tmp38E=_tmp38A;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp38B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp38B->tag != 1)goto _LL1A4;else{_tmp38C=(void*)_tmp38B->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp38D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp38C;if(_tmp38D->tag != 5)goto _LL1A4;else{_tmp38E=_tmp38D->f1;}};}}_LL1A3:
# 1332
 if(_tmp38E->escapes)return r;{
# 1333
struct Cyc_CfFlowInfo_Reln*_tmp63C;struct Cyc_List_List*_tmp63B;return(_tmp63B=_region_malloc(rgn,sizeof(*_tmp63B)),((_tmp63B->hd=((_tmp63C=_region_malloc(rgn,sizeof(*_tmp63C)),((_tmp63C->vd=v,((_tmp63C->rop=Cyc_CfFlowInfo_LessNumelts(_tmp38E),_tmp63C)))))),((_tmp63B->tl=r,_tmp63B)))));};_LL1A4:;_LL1A5:
# 1334
 goto _LL19B;_LL19B:;}
# 1336
goto _LL196;_LL199:;_LL19A:
# 1337
 goto _LL196;_LL196:;}
# 1339
goto _LL18B;_LL192: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp371=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp371->tag != 2)goto _LL194;else{_tmp372=_tmp371->f1;_tmp373=_tmp371->f2;if(_tmp373 == 0)goto _LL194;_tmp374=*_tmp373;_tmp375=(struct Cyc_Absyn_Exp*)_tmp374.hd;}}_LL193:
# 1341
{void*_tmp391=_tmp375->r;void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp395;void*_tmp397;struct Cyc_Absyn_Vardecl*_tmp399;void*_tmp39B;struct Cyc_Absyn_Vardecl*_tmp39D;void*_tmp39F;struct Cyc_Absyn_Vardecl*_tmp3A1;_LL1A7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp392=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp391;if(_tmp392->tag != 1)goto _LL1A9;else{_tmp393=(void*)_tmp392->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp394=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp393;if(_tmp394->tag != 1)goto _LL1A9;else{_tmp395=_tmp394->f1;}};}}_LL1A8:
# 1342
 _tmp399=_tmp395;goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp396=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp391;if(_tmp396->tag != 1)goto _LL1AB;else{_tmp397=(void*)_tmp396->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp398=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp397;if(_tmp398->tag != 4)goto _LL1AB;else{_tmp399=_tmp398->f1;}};}}_LL1AA:
# 1343
 _tmp39D=_tmp399;goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp39A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp391;if(_tmp39A->tag != 1)goto _LL1AD;else{_tmp39B=(void*)_tmp39A->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp39C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp39B;if(_tmp39C->tag != 3)goto _LL1AD;else{_tmp39D=_tmp39C->f1;}};}}_LL1AC:
# 1344
 _tmp3A1=_tmp39D;goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp39E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp391;if(_tmp39E->tag != 1)goto _LL1AF;else{_tmp39F=(void*)_tmp39E->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3A0=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp39F;if(_tmp3A0->tag != 5)goto _LL1AF;else{_tmp3A1=_tmp3A0->f1;}};}}_LL1AE:
# 1346
 if(_tmp3A1->escapes)return r;{
# 1347
struct Cyc_CfFlowInfo_Reln*_tmp63F;struct Cyc_List_List*_tmp63E;return(_tmp63E=_region_malloc(rgn,sizeof(*_tmp63E)),((_tmp63E->hd=((_tmp63F=_region_malloc(rgn,sizeof(*_tmp63F)),((_tmp63F->vd=v,((_tmp63F->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp3A1),_tmp63F)))))),((_tmp63E->tl=r,_tmp63E)))));};_LL1AF:;_LL1B0:
# 1348
 goto _LL1A6;_LL1A6:;}
# 1350
goto _LL18B;_LL194:;_LL195:
# 1351
 goto _LL18B;_LL18B:;}
# 1353
return r;}
# 1356
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1357
{void*_tmp3A4=e1->r;void*_tmp3A6;struct Cyc_Absyn_Vardecl*_tmp3A8;void*_tmp3AA;struct Cyc_Absyn_Vardecl*_tmp3AC;void*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3B0;void*_tmp3B2;struct Cyc_Absyn_Vardecl*_tmp3B4;_LL1B2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A4;if(_tmp3A5->tag != 1)goto _LL1B4;else{_tmp3A6=(void*)_tmp3A5->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3A7=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp3A6;if(_tmp3A7->tag != 1)goto _LL1B4;else{_tmp3A8=_tmp3A7->f1;}};}}_LL1B3:
# 1358
 _tmp3AC=_tmp3A8;goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A4;if(_tmp3A9->tag != 1)goto _LL1B6;else{_tmp3AA=(void*)_tmp3A9->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3AB=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp3AA;if(_tmp3AB->tag != 3)goto _LL1B6;else{_tmp3AC=_tmp3AB->f1;}};}}_LL1B5:
# 1359
 _tmp3B0=_tmp3AC;goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A4;if(_tmp3AD->tag != 1)goto _LL1B8;else{_tmp3AE=(void*)_tmp3AD->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3AF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp3AE;if(_tmp3AF->tag != 4)goto _LL1B8;else{_tmp3B0=_tmp3AF->f1;}};}}_LL1B7:
# 1360
 _tmp3B4=_tmp3B0;goto _LL1B9;_LL1B8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3B1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A4;if(_tmp3B1->tag != 1)goto _LL1BA;else{_tmp3B2=(void*)_tmp3B1->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3B3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp3B2;if(_tmp3B3->tag != 5)goto _LL1BA;else{_tmp3B4=_tmp3B3->f1;}};}}_LL1B9:
# 1362
 if(!_tmp3B4->escapes)
# 1363
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp3B4,e2);
# 1364
goto _LL1B1;_LL1BA:;_LL1BB:
# 1365
 goto _LL1B1;_LL1B1:;}
# 1367
return r;}
# 1370
static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){
# 1371
{const char*_tmp643;void*_tmp642[1];struct Cyc_String_pa_PrintArg_struct _tmp641;(_tmp641.tag=0,((_tmp641.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name)),((_tmp642[0]=& _tmp641,Cyc_fprintf(Cyc_stderr,((_tmp643="%s",_tag_dyneither(_tmp643,sizeof(char),3))),_tag_dyneither(_tmp642,sizeof(void*),1)))))));}{
# 1372
union Cyc_CfFlowInfo_RelnOp _tmp3B8=r->rop;unsigned int _tmp3B9;struct _tuple11 _tmp3BA;struct Cyc_Absyn_Vardecl*_tmp3BB;struct Cyc_Absyn_Vardecl*_tmp3BC;unsigned int _tmp3BD;unsigned int _tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3BF;_LL1BD: if((_tmp3B8.EqualConst).tag != 1)goto _LL1BF;_tmp3B9=(unsigned int)(_tmp3B8.EqualConst).val;_LL1BE:
# 1373
{const char*_tmp647;void*_tmp646[1];struct Cyc_Int_pa_PrintArg_struct _tmp645;(_tmp645.tag=1,((_tmp645.f1=(unsigned long)((int)_tmp3B9),((_tmp646[0]=& _tmp645,Cyc_fprintf(Cyc_stderr,((_tmp647="==%d",_tag_dyneither(_tmp647,sizeof(char),5))),_tag_dyneither(_tmp646,sizeof(void*),1)))))));}goto _LL1BC;_LL1BF: if((_tmp3B8.LessVar).tag != 2)goto _LL1C1;_tmp3BA=(struct _tuple11)(_tmp3B8.LessVar).val;_tmp3BB=_tmp3BA.f1;_LL1C0:
# 1374
{const char*_tmp64B;void*_tmp64A[1];struct Cyc_String_pa_PrintArg_struct _tmp649;(_tmp649.tag=0,((_tmp649.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BB->name)),((_tmp64A[0]=& _tmp649,Cyc_fprintf(Cyc_stderr,((_tmp64B="<%s",_tag_dyneither(_tmp64B,sizeof(char),4))),_tag_dyneither(_tmp64A,sizeof(void*),1)))))));}goto _LL1BC;_LL1C1: if((_tmp3B8.LessNumelts).tag != 3)goto _LL1C3;_tmp3BC=(struct Cyc_Absyn_Vardecl*)(_tmp3B8.LessNumelts).val;_LL1C2:
# 1375
{const char*_tmp64F;void*_tmp64E[1];struct Cyc_String_pa_PrintArg_struct _tmp64D;(_tmp64D.tag=0,((_tmp64D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BC->name)),((_tmp64E[0]=& _tmp64D,Cyc_fprintf(Cyc_stderr,((_tmp64F="<numelts(%s)",_tag_dyneither(_tmp64F,sizeof(char),13))),_tag_dyneither(_tmp64E,sizeof(void*),1)))))));}goto _LL1BC;_LL1C3: if((_tmp3B8.LessConst).tag != 4)goto _LL1C5;_tmp3BD=(unsigned int)(_tmp3B8.LessConst).val;_LL1C4:
# 1376
{const char*_tmp653;void*_tmp652[1];struct Cyc_Int_pa_PrintArg_struct _tmp651;(_tmp651.tag=1,((_tmp651.f1=(unsigned long)((int)_tmp3BD),((_tmp652[0]=& _tmp651,Cyc_fprintf(Cyc_stderr,((_tmp653="<%d",_tag_dyneither(_tmp653,sizeof(char),4))),_tag_dyneither(_tmp652,sizeof(void*),1)))))));}goto _LL1BC;_LL1C5: if((_tmp3B8.LessEqConst).tag != 6)goto _LL1C7;_tmp3BE=(unsigned int)(_tmp3B8.LessEqConst).val;_LL1C6:
# 1377
{const char*_tmp657;void*_tmp656[1];struct Cyc_Int_pa_PrintArg_struct _tmp655;(_tmp655.tag=1,((_tmp655.f1=(unsigned long)((int)_tmp3BE),((_tmp656[0]=& _tmp655,Cyc_fprintf(Cyc_stderr,((_tmp657="<=%d",_tag_dyneither(_tmp657,sizeof(char),5))),_tag_dyneither(_tmp656,sizeof(void*),1)))))));}goto _LL1BC;_LL1C7: if((_tmp3B8.LessEqNumelts).tag != 5)goto _LL1BC;_tmp3BF=(struct Cyc_Absyn_Vardecl*)(_tmp3B8.LessEqNumelts).val;_LL1C8:
# 1378
{const char*_tmp65B;void*_tmp65A[1];struct Cyc_String_pa_PrintArg_struct _tmp659;(_tmp659.tag=0,((_tmp659.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BF->name)),((_tmp65A[0]=& _tmp659,Cyc_fprintf(Cyc_stderr,((_tmp65B="<=numelts(%s)",_tag_dyneither(_tmp65B,sizeof(char),14))),_tag_dyneither(_tmp65A,sizeof(void*),1)))))));}goto _LL1BC;_LL1BC:;};}
# 1381
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){
# 1382
for(0;r != 0;r=r->tl){
# 1383
Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
# 1384
if(r->tl != 0){const char*_tmp65E;void*_tmp65D;(_tmp65D=0,Cyc_fprintf(Cyc_stderr,((_tmp65E=",",_tag_dyneither(_tmp65E,sizeof(char),2))),_tag_dyneither(_tmp65D,sizeof(void*),0)));}}}
# 1388
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
# 1389
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C9:
# 1390
{const char*_tmp661;void*_tmp660;(_tmp660=0,Cyc_fprintf(Cyc_stderr,((_tmp661="uninitialized",_tag_dyneither(_tmp661,sizeof(char),14))),_tag_dyneither(_tmp660,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL1CA:
# 1391
{const char*_tmp664;void*_tmp663;(_tmp663=0,Cyc_fprintf(Cyc_stderr,((_tmp664="this-initialized",_tag_dyneither(_tmp664,sizeof(char),17))),_tag_dyneither(_tmp663,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1CB:
# 1392
{const char*_tmp667;void*_tmp666;(_tmp666=0,Cyc_fprintf(Cyc_stderr,((_tmp667="all-initialized",_tag_dyneither(_tmp667,sizeof(char),16))),_tag_dyneither(_tmp666,sizeof(void*),0)));}break;}}
# 1396
void Cyc_CfFlowInfo_print_root(void*root){
# 1397
void*_tmp3DA=root;struct Cyc_Absyn_Vardecl*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;void*_tmp3DF;_LL1CE: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3DB=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3DA;if(_tmp3DB->tag != 0)goto _LL1D0;else{_tmp3DC=_tmp3DB->f1;}}_LL1CF:
# 1399
{const char*_tmp66B;void*_tmp66A[1];struct Cyc_String_pa_PrintArg_struct _tmp669;(_tmp669.tag=0,((_tmp669.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3DC->name)),((_tmp66A[0]=& _tmp669,Cyc_fprintf(Cyc_stderr,((_tmp66B="Root(%s)",_tag_dyneither(_tmp66B,sizeof(char),9))),_tag_dyneither(_tmp66A,sizeof(void*),1)))))));}goto _LL1CD;_LL1D0: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3DD=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp3DA;if(_tmp3DD->tag != 1)goto _LL1D2;else{_tmp3DE=_tmp3DD->f1;_tmp3DF=(void*)_tmp3DD->f2;}}_LL1D1:
# 1401
{const char*_tmp670;void*_tmp66F[2];struct Cyc_String_pa_PrintArg_struct _tmp66E;struct Cyc_String_pa_PrintArg_struct _tmp66D;(_tmp66D.tag=0,((_tmp66D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1402
Cyc_Absynpp_typ2string(_tmp3DF)),((_tmp66E.tag=0,((_tmp66E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3DE)),((_tmp66F[0]=& _tmp66E,((_tmp66F[1]=& _tmp66D,Cyc_fprintf(Cyc_stderr,((_tmp670="MallocPt(%s,%s)",_tag_dyneither(_tmp670,sizeof(char),16))),_tag_dyneither(_tmp66F,sizeof(void*),2)))))))))))));}goto _LL1CD;_LL1D2: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3E0=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp3DA;if(_tmp3E0->tag != 2)goto _LL1CD;}_LL1D3:
# 1404
{const char*_tmp673;void*_tmp672;(_tmp672=0,Cyc_fprintf(Cyc_stderr,((_tmp673="InitParam(_,_)",_tag_dyneither(_tmp673,sizeof(char),15))),_tag_dyneither(_tmp672,sizeof(void*),0)));}goto _LL1CD;_LL1CD:;}
# 1408
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
# 1409
Cyc_CfFlowInfo_print_root(p->root);{
# 1410
struct Cyc_List_List*_tmp3EA=p->fields;
# 1411
while(_tmp3EA != 0){
# 1412
{const char*_tmp677;void*_tmp676[1];struct Cyc_Int_pa_PrintArg_struct _tmp675;(_tmp675.tag=1,((_tmp675.f1=(unsigned long)((int)_tmp3EA->hd),((_tmp676[0]=& _tmp675,Cyc_fprintf(Cyc_stderr,((_tmp677=".%d",_tag_dyneither(_tmp677,sizeof(char),4))),_tag_dyneither(_tmp676,sizeof(void*),1)))))));}
# 1413
_tmp3EA=_tmp3EA->tl;}{
# 1415
const char*_tmp67A;void*_tmp679;(_tmp679=0,Cyc_fprintf(Cyc_stderr,((_tmp67A=" ",_tag_dyneither(_tmp67A,sizeof(char),2))),_tag_dyneither(_tmp679,sizeof(void*),0)));};};}
# 1418
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
# 1419
int _tmp3F0=1;
# 1420
while(x != 0){
# 1421
if(!_tmp3F0){{const char*_tmp67D;void*_tmp67C;(_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D=", ",_tag_dyneither(_tmp67D,sizeof(char),3))),_tag_dyneither(_tmp67C,sizeof(void*),0)));}_tmp3F0=0;}
# 1422
pr((void*)x->hd);
# 1423
x=x->tl;}{
# 1425
const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680="\n",_tag_dyneither(_tmp680,sizeof(char),2))),_tag_dyneither(_tmp67F,sizeof(void*),0)));};}
# 1428
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
# 1429
struct _RegionHandle _tmp3F5=_new_region("r");struct _RegionHandle*r=& _tmp3F5;_push_region(r);
# 1430
{struct _handler_cons _tmp3F6;_push_handler(& _tmp3F6);{int _tmp3F8=0;if(setjmp(_tmp3F6.handler))_tmp3F8=1;if(!_tmp3F8){
# 1431
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
# 1432
struct _tuple14 _tmp3F9=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
# 1433
int first=1;
# 1434
{const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683="{ ",_tag_dyneither(_tmp683,sizeof(char),3))),_tag_dyneither(_tmp682,sizeof(void*),0)));}
# 1435
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp3F9)){
# 1436
void*_tmp3FC=_tmp3F9.f1;
# 1437
if(!first){
# 1438
{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686=", ",_tag_dyneither(_tmp686,sizeof(char),3))),_tag_dyneither(_tmp685,sizeof(void*),0)));}
# 1439
first=0;}
# 1441
pr(_tmp3FC);}{
# 1443
const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="}\n",_tag_dyneither(_tmp689,sizeof(char),3))),_tag_dyneither(_tmp688,sizeof(void*),0)));};}
# 1431
;_pop_handler();}else{void*_tmp3F7=(void*)_exn_thrown;void*_tmp402=_tmp3F7;_LL1D5:;_LL1D6:
# 1446
{const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="{ }\n",_tag_dyneither(_tmp68C,sizeof(char),5))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}
# 1447
_npop_handler(0);return;_LL1D7:;_LL1D8:(void)_throw(_tmp402);_LL1D4:;}};}
# 1430
;_pop_region(r);}
# 1452
void Cyc_CfFlowInfo_print_absrval(void*rval){
# 1453
void*_tmp405=rval;enum Cyc_CfFlowInfo_InitLevel _tmp40A;enum Cyc_CfFlowInfo_InitLevel _tmp40C;struct Cyc_CfFlowInfo_Place*_tmp40E;struct Cyc_CfFlowInfo_UnionRInfo _tmp410;int _tmp411;int _tmp412;struct _dyneither_ptr _tmp413;struct Cyc_Absyn_Exp*_tmp415;int _tmp416;void*_tmp417;_LL1DA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp406=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp406->tag != 0)goto _LL1DC;}_LL1DB:
# 1454
{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((_tmp68F="Zero",_tag_dyneither(_tmp68F,sizeof(char),5))),_tag_dyneither(_tmp68E,sizeof(void*),0)));}goto _LL1D9;_LL1DC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp407=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp407->tag != 1)goto _LL1DE;}_LL1DD:
# 1455
{const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(Cyc_stderr,((_tmp692="NotZeroAll",_tag_dyneither(_tmp692,sizeof(char),11))),_tag_dyneither(_tmp691,sizeof(void*),0)));}goto _LL1D9;_LL1DE: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp408=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp408->tag != 2)goto _LL1E0;}_LL1DF:
# 1456
{const char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_fprintf(Cyc_stderr,((_tmp695="NotZeroThis",_tag_dyneither(_tmp695,sizeof(char),12))),_tag_dyneither(_tmp694,sizeof(void*),0)));}goto _LL1D9;_LL1E0: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp409=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp409->tag != 3)goto _LL1E2;else{_tmp40A=_tmp409->f1;}}_LL1E1:
# 1457
{const char*_tmp698;void*_tmp697;(_tmp697=0,Cyc_fprintf(Cyc_stderr,((_tmp698="Unknown(",_tag_dyneither(_tmp698,sizeof(char),9))),_tag_dyneither(_tmp697,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp40A);
# 1458
{const char*_tmp69B;void*_tmp69A;(_tmp69A=0,Cyc_fprintf(Cyc_stderr,((_tmp69B=")",_tag_dyneither(_tmp69B,sizeof(char),2))),_tag_dyneither(_tmp69A,sizeof(void*),0)));}goto _LL1D9;_LL1E2: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp40B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp40B->tag != 4)goto _LL1E4;else{_tmp40C=_tmp40B->f1;}}_LL1E3:
# 1459
{const char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E="Esc(",_tag_dyneither(_tmp69E,sizeof(char),5))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp40C);
# 1460
{const char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,((_tmp6A1=")",_tag_dyneither(_tmp6A1,sizeof(char),2))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}goto _LL1D9;_LL1E4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp40D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp40D->tag != 5)goto _LL1E6;else{_tmp40E=_tmp40D->f1;}}_LL1E5:
# 1461
{const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="AddrOf(",_tag_dyneither(_tmp6A4,sizeof(char),8))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp40E);
# 1462
{const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7=")",_tag_dyneither(_tmp6A7,sizeof(char),2))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}goto _LL1D9;_LL1E6: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp40F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp40F->tag != 6)goto _LL1E8;else{_tmp410=_tmp40F->f1;_tmp411=_tmp410.is_union;_tmp412=_tmp410.fieldnum;_tmp413=_tmp40F->f2;}}_LL1E7:
# 1464
 if(_tmp411){
# 1465
{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="AggrUnion{",_tag_dyneither(_tmp6AA,sizeof(char),11))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
# 1466
if(_tmp412 != - 1){
# 1467
const char*_tmp6AE;void*_tmp6AD[1];struct Cyc_Int_pa_PrintArg_struct _tmp6AC;(_tmp6AC.tag=1,((_tmp6AC.f1=(unsigned long)_tmp412,((_tmp6AD[0]=& _tmp6AC,Cyc_fprintf(Cyc_stderr,((_tmp6AE="tag == %d;",_tag_dyneither(_tmp6AE,sizeof(char),11))),_tag_dyneither(_tmp6AD,sizeof(void*),1)))))));}}else{
# 1469
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_fprintf(Cyc_stderr,((_tmp6B1="AggrStruct{",_tag_dyneither(_tmp6B1,sizeof(char),12))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}
# 1470
{int i=0;for(0;i < _get_dyneither_size(_tmp413,sizeof(void*));++ i){
# 1471
Cyc_CfFlowInfo_print_absrval(((void**)_tmp413.curr)[i]);
# 1472
if(i + 1 < _get_dyneither_size(_tmp413,sizeof(void*))){const char*_tmp6B4;void*_tmp6B3;(_tmp6B3=0,Cyc_fprintf(Cyc_stderr,((_tmp6B4=",",_tag_dyneither(_tmp6B4,sizeof(char),2))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}}}
# 1474
{const char*_tmp6B7;void*_tmp6B6;(_tmp6B6=0,Cyc_fprintf(Cyc_stderr,((_tmp6B7="}",_tag_dyneither(_tmp6B7,sizeof(char),2))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));}
# 1475
goto _LL1D9;_LL1E8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp414=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp405;if(_tmp414->tag != 7)goto _LL1D9;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;_tmp417=(void*)_tmp414->f3;}}_LL1E9:
# 1477
{const char*_tmp6BC;void*_tmp6BB[2];struct Cyc_String_pa_PrintArg_struct _tmp6BA;struct Cyc_Int_pa_PrintArg_struct _tmp6B9;(_tmp6B9.tag=1,((_tmp6B9.f1=(unsigned long)_tmp416,((_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp415)),((_tmp6BB[0]=& _tmp6BA,((_tmp6BB[1]=& _tmp6B9,Cyc_fprintf(Cyc_stderr,((_tmp6BC="[Consumed(%s,%d,",_tag_dyneither(_tmp6BC,sizeof(char),17))),_tag_dyneither(_tmp6BB,sizeof(void*),2)))))))))))));}
# 1478
Cyc_CfFlowInfo_print_absrval(_tmp417);{const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_fprintf(Cyc_stderr,((_tmp6BF="]",_tag_dyneither(_tmp6BF,sizeof(char),2))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));};_LL1D9:;}
# 1482
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
# 1483
{const char*_tmp6C2;void*_tmp6C1;(_tmp6C1=0,Cyc_fprintf(Cyc_stderr,((_tmp6C2="    ",_tag_dyneither(_tmp6C2,sizeof(char),5))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}
# 1484
Cyc_CfFlowInfo_print_root(root);
# 1485
{const char*_tmp6C5;void*_tmp6C4;(_tmp6C4=0,Cyc_fprintf(Cyc_stderr,((_tmp6C5=" --> ",_tag_dyneither(_tmp6C5,sizeof(char),6))),_tag_dyneither(_tmp6C4,sizeof(void*),0)));}
# 1486
Cyc_CfFlowInfo_print_absrval(rval);{
# 1487
const char*_tmp6C8;void*_tmp6C7;(_tmp6C7=0,Cyc_fprintf(Cyc_stderr,((_tmp6C8="\n",_tag_dyneither(_tmp6C8,sizeof(char),2))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));};}
# 1490
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
# 1491
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1494
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
# 1495
union Cyc_CfFlowInfo_FlowInfo _tmp441=f;int _tmp442;struct _tuple12 _tmp443;struct Cyc_Dict_Dict _tmp444;_LL1EB: if((_tmp441.BottomFL).tag != 1)goto _LL1ED;_tmp442=(int)(_tmp441.BottomFL).val;_LL1EC:
# 1496
{const char*_tmp6CB;void*_tmp6CA;(_tmp6CA=0,Cyc_fprintf(Cyc_stderr,((_tmp6CB="  BottomFL\n",_tag_dyneither(_tmp6CB,sizeof(char),12))),_tag_dyneither(_tmp6CA,sizeof(void*),0)));}goto _LL1EA;_LL1ED: if((_tmp441.ReachableFL).tag != 2)goto _LL1EA;_tmp443=(struct _tuple12)(_tmp441.ReachableFL).val;_tmp444=_tmp443.f1;_LL1EE:
# 1498
{const char*_tmp6CE;void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,((_tmp6CE="  ReachableFL:\n",_tag_dyneither(_tmp6CE,sizeof(char),16))),_tag_dyneither(_tmp6CD,sizeof(void*),0)));}
# 1499
Cyc_CfFlowInfo_print_flowdict(_tmp444);
# 1500
{const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_fprintf(Cyc_stderr,((_tmp6D1="\n",_tag_dyneither(_tmp6D1,sizeof(char),2))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}
# 1501
goto _LL1EA;_LL1EA:;}
# 1513 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
# 1514
void*_tmp44B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp458;struct Cyc_Absyn_DatatypeInfo _tmp45A;struct Cyc_List_List*_tmp45B;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_AggrInfo _tmp45F;struct Cyc_List_List*_tmp460;struct Cyc_Absyn_DatatypeFieldInfo _tmp462;struct Cyc_List_List*_tmp463;struct Cyc_List_List*_tmp465;struct Cyc_Absyn_PtrInfo _tmp467;void*_tmp468;struct Cyc_Absyn_PtrAtts _tmp469;void*_tmp46A;struct Cyc_List_List*_tmp46D;struct Cyc_List_List*_tmp46F;struct Cyc_Absyn_ArrayInfo _tmp471;void*_tmp472;void*_tmp474;void*_tmp476;void*_tmp478;void*_tmp479;void*_tmp47B;_LL1F0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp44B;if(_tmp44C->tag != 26)goto _LL1F2;}_LL1F1:
# 1515
 goto _LL1F3;_LL1F2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp44B;if(_tmp44D->tag != 0)goto _LL1F4;}_LL1F3:
# 1516
 goto _LL1F5;_LL1F4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44B;if(_tmp44E->tag != 6)goto _LL1F6;}_LL1F5:
# 1517
 goto _LL1F7;_LL1F6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp44B;if(_tmp44F->tag != 7)goto _LL1F8;}_LL1F7:
# 1518
 goto _LL1F9;_LL1F8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp44B;if(_tmp450->tag != 13)goto _LL1FA;}_LL1F9:
# 1519
 goto _LL1FB;_LL1FA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp44B;if(_tmp451->tag != 14)goto _LL1FC;}_LL1FB:
# 1520
 goto _LL1FD;_LL1FC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp44B;if(_tmp452->tag != 19)goto _LL1FE;}_LL1FD:
# 1521
 goto _LL1FF;_LL1FE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp44B;if(_tmp453->tag != 20)goto _LL200;}_LL1FF:
# 1522
 goto _LL201;_LL200: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp44B;if(_tmp454->tag != 1)goto _LL202;}_LL201:
# 1523
 return 0;_LL202: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp44B;if(_tmp455->tag != 21)goto _LL204;}_LL203: {
# 1525
void*_tmp47D=rgn;_LL227: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp47E=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp47D;if(_tmp47E->tag != 0)goto _LL229;}_LL228:
# 1526
 return 1;_LL229:;_LL22A:
# 1527
 return 0;_LL226:;}_LL204: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp44B;if(_tmp456->tag != 22)goto _LL206;}_LL205: {
# 1530
void*_tmp47F=rgn;_LL22C: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp480=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp47F;if(_tmp480->tag != 1)goto _LL22E;}_LL22D:
# 1531
 return 1;_LL22E:;_LL22F:
# 1532
 return 0;_LL22B:;}_LL206: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp44B;if(_tmp457->tag != 2)goto _LL208;else{_tmp458=_tmp457->f1;}}_LL207: {
# 1535
void*_tmp481=rgn;struct Cyc_Absyn_Tvar*_tmp485;_LL231: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp482=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp481;if(_tmp482->tag != 1)goto _LL233;}_LL232:
# 1536
 goto _LL234;_LL233: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp483=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp481;if(_tmp483->tag != 0)goto _LL235;}_LL234:
# 1537
 return 0;_LL235: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp484=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp481;if(_tmp484->tag != 2)goto _LL230;else{_tmp485=_tmp484->f1;}}_LL236:
# 1538
 return Cyc_Absyn_tvar_cmp(_tmp458,_tmp485)== 0;_LL230:;}_LL208: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp44B;if(_tmp459->tag != 3)goto _LL20A;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp45A.targs;}}_LL209:
# 1548 "cf_flowinfo.cyc"
 _tmp45D=_tmp45B;goto _LL20B;_LL20A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44B;if(_tmp45C->tag != 17)goto _LL20C;else{_tmp45D=_tmp45C->f2;}}_LL20B:
# 1549
 _tmp460=_tmp45D;goto _LL20D;_LL20C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B;if(_tmp45E->tag != 11)goto _LL20E;else{_tmp45F=_tmp45E->f1;_tmp460=_tmp45F.targs;}}_LL20D:
# 1550
 _tmp463=_tmp460;goto _LL20F;_LL20E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp44B;if(_tmp461->tag != 4)goto _LL210;else{_tmp462=_tmp461->f1;_tmp463=_tmp462.targs;}}_LL20F:
# 1551
 _tmp465=_tmp463;goto _LL211;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp44B;if(_tmp464->tag != 24)goto _LL212;else{_tmp465=_tmp464->f1;}}_LL211:
# 1553
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp465);_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44B;if(_tmp466->tag != 5)goto _LL214;else{_tmp467=_tmp466->f1;_tmp468=_tmp467.elt_typ;_tmp469=_tmp467.ptr_atts;_tmp46A=_tmp469.rgn;}}_LL213:
# 1555
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp46A) || Cyc_CfFlowInfo_contains_region(rgn,_tmp468);_LL214: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B;if(_tmp46B->tag != 9)goto _LL216;}_LL215:
# 1556
 return 0;_LL216: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp44B;if(_tmp46C->tag != 10)goto _LL218;else{_tmp46D=_tmp46C->f1;}}_LL217:
# 1558
 for(0;_tmp46D != 0;_tmp46D=_tmp46D->tl){
# 1559
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp46D->hd)).f2))return 1;}
# 1560
return 0;_LL218: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44B;if(_tmp46E->tag != 12)goto _LL21A;else{_tmp46F=_tmp46E->f2;}}_LL219:
# 1562
 for(0;_tmp46F != 0;_tmp46F=_tmp46F->tl){
# 1563
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp46F->hd)->type))return 1;}
# 1564
return 0;_LL21A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44B;if(_tmp470->tag != 8)goto _LL21C;else{_tmp471=_tmp470->f1;_tmp472=_tmp471.elt_type;}}_LL21B:
# 1565
 _tmp474=_tmp472;goto _LL21D;_LL21C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp44B;if(_tmp473->tag != 23)goto _LL21E;else{_tmp474=(void*)_tmp473->f1;}}_LL21D:
# 1566
 _tmp476=_tmp474;goto _LL21F;_LL21E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp44B;if(_tmp475->tag != 25)goto _LL220;else{_tmp476=(void*)_tmp475->f1;}}_LL21F:
# 1567
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp476);_LL220: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp44B;if(_tmp477->tag != 16)goto _LL222;else{_tmp478=(void*)_tmp477->f1;_tmp479=(void*)_tmp477->f2;}}_LL221:
# 1569
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp478) || Cyc_CfFlowInfo_contains_region(rgn,_tmp479);_LL222: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp44B;if(_tmp47A->tag != 15)goto _LL224;else{_tmp47B=(void*)_tmp47A->f1;}}_LL223:
# 1572
 return 0;_LL224: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp44B;if(_tmp47C->tag != 18)goto _LL1EF;}_LL225:
# 1573
 return 0;_LL1EF:;}struct _tuple25{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1578
static void Cyc_CfFlowInfo_kill_root(struct _tuple25*env,void*root,void*rval){
# 1579
struct _tuple25 _tmp487;struct Cyc_CfFlowInfo_FlowEnv*_tmp488;struct Cyc_Dict_Dict _tmp489;struct Cyc_Dict_Dict*_tmp48A;void*_tmp48B;struct _tuple25*_tmp486=env;_tmp487=*_tmp486;_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;_tmp48A=(struct Cyc_Dict_Dict*)&(*_tmp486).f2;_tmp48B=_tmp487.f3;{
# 1580
void*_tmp48C=root;struct Cyc_Absyn_Vardecl*_tmp48E;void*_tmp490;_LL238: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp48D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp48C;if(_tmp48D->tag != 0)goto _LL23A;else{_tmp48E=_tmp48D->f1;}}_LL239:
# 1583
 if(Cyc_CfFlowInfo_contains_region(_tmp48B,_tmp48E->type))
# 1585
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp488,_tmp48E->type,0,_tmp488->unknown_none);
# 1586
*_tmp48A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp48A,root,rval);
# 1587
goto _LL237;_LL23A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp48F=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp48C;if(_tmp48F->tag != 1)goto _LL23C;else{_tmp490=(void*)_tmp48F->f2;}}_LL23B:
# 1590
 if(!Cyc_CfFlowInfo_contains_region(_tmp48B,_tmp490))
# 1592
*_tmp48A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp48A,root,rval);
# 1593
goto _LL237;_LL23C: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp491=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp48C;if(_tmp491->tag != 2)goto _LL237;}_LL23D:
# 1596
 goto _LL237;_LL237:;};}
# 1601
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1603
void*_tmp492=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp494;_LL23F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp492;if(_tmp493->tag != 2)goto _LL241;else{_tmp494=_tmp493->f1;}}_LL240: {
# 1605
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp6D2;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp6D2.tag=2,((_tmp6D2.f1=_tmp494,_tmp6D2)));
# 1606
void*v2=(void*)& v;
# 1607
struct _tuple25 _tmp6D3;struct _tuple25 env=(_tmp6D3.f1=fenv,((_tmp6D3.f2=fenv->mt_flowdict,((_tmp6D3.f3=v2,_tmp6D3)))));
# 1608
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
# 1609
return env.f2;}_LL241: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp495=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp492;if(_tmp495->tag != 21)goto _LL243;}_LL242: {
# 1612
struct _tuple25 _tmp6D4;struct _tuple25 env=(_tmp6D4.f1=fenv,((_tmp6D4.f2=fenv->mt_flowdict,((_tmp6D4.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp6D4)))));
# 1613
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
# 1614
return env.f2;}_LL243: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp492;if(_tmp496->tag != 22)goto _LL245;}_LL244: {
# 1617
struct _tuple25 _tmp6D5;struct _tuple25 env=(_tmp6D5.f1=fenv,((_tmp6D5.f2=fenv->mt_flowdict,((_tmp6D5.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp6D5)))));
# 1618
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
# 1619
return env.f2;}_LL245:;_LL246: {
# 1621
struct Cyc_Core_Impossible_exn_struct _tmp6DB;const char*_tmp6DA;struct Cyc_Core_Impossible_exn_struct*_tmp6D9;(int)_throw((void*)((_tmp6D9=_cycalloc(sizeof(*_tmp6D9)),((_tmp6D9[0]=((_tmp6DB.tag=Cyc_Core_Impossible,((_tmp6DB.f1=((_tmp6DA="kill_flowdict_region",_tag_dyneither(_tmp6DA,sizeof(char),21))),_tmp6DB)))),_tmp6D9)))));}_LL23E:;}
# 1626
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1628
return 0;}
# 1633
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
# 1634
union Cyc_CfFlowInfo_FlowInfo _tmp49E=f;int _tmp49F;struct _tuple12 _tmp4A0;struct Cyc_Dict_Dict _tmp4A1;struct Cyc_List_List*_tmp4A2;_LL248: if((_tmp49E.BottomFL).tag != 1)goto _LL24A;_tmp49F=(int)(_tmp49E.BottomFL).val;_LL249:
# 1635
 return f;_LL24A: if((_tmp49E.ReachableFL).tag != 2)goto _LL247;_tmp4A0=(struct _tuple12)(_tmp49E.ReachableFL).val;_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_LL24B: {
# 1637
struct Cyc_Dict_Dict _tmp4A3=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp4A1,rgn);
# 1638
struct Cyc_List_List*_tmp4A4=Cyc_CfFlowInfo_kill_relns_region(_tmp4A2,rgn);
# 1640
return Cyc_CfFlowInfo_ReachableFL(_tmp4A3,_tmp4A4);}_LL247:;}
