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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 400
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 901 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 910
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1167
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 66
void*Cyc_Tcutil_compress(void*t);
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 160
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 173
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 278 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 324
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 100
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 102
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 104
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 50 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 61
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 63
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 68
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 77
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 83
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 88
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 96
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 116
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 118
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 120
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 123
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 139 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 158
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 161
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 164
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 167
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 173
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 178
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 200 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 204
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 209
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 220
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 227
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 231
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 233
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 45 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 49
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp38A;return((_tmp38A.PlaceL).val=x,(((_tmp38A.PlaceL).tag=1,_tmp38A)));}
# 52
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp38B;return((_tmp38B.UnknownL).val=0,(((_tmp38B.UnknownL).tag=2,_tmp38B)));}
# 55
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp38C;return((_tmp38C.BottomFL).val=0,(((_tmp38C.BottomFL).tag=1,_tmp38C)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple11 _tmp38F;union Cyc_CfFlowInfo_FlowInfo _tmp38E;return((_tmp38E.ReachableFL).val=((_tmp38F.f1=fd,((_tmp38F.f2=r,_tmp38F)))),(((_tmp38E.ReachableFL).tag=2,_tmp38E)));}
# 61
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp3C6;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp3C5;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3C4;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3C3;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3C2;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3C1;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3C0;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3BF;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3BE;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3BD;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3BC;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3BB;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3BA;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3B9;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3B8;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3B7;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp3B6;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3B5;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp3B4;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3B3;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp3B2;struct Cyc_CfFlowInfo_FlowEnv*_tmp3B1;return(_tmp3B1=_region_malloc(r,sizeof(*_tmp3B1)),((_tmp3B1->r=r,((_tmp3B1->zero=(void*)(
# 68
(_tmp3B3=_region_malloc(r,sizeof(*_tmp3B3)),((_tmp3B3[0]=((_tmp3B2.tag=0,_tmp3B2)),_tmp3B3)))),((_tmp3B1->notzeroall=(void*)(
(_tmp3B5=_region_malloc(r,sizeof(*_tmp3B5)),((_tmp3B5[0]=((_tmp3B4.tag=1,_tmp3B4)),_tmp3B5)))),((_tmp3B1->notzerothis=(void*)(
(_tmp3B7=_region_malloc(r,sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3B6.tag=2,_tmp3B6)),_tmp3B7)))),((_tmp3B1->unknown_none=(void*)(
(_tmp3B9=_region_malloc(r,sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3B8.tag=3,((_tmp3B8.f1=Cyc_CfFlowInfo_NoneIL,_tmp3B8)))),_tmp3B9)))),((_tmp3B1->unknown_this=(void*)(
(_tmp3BB=_region_malloc(r,sizeof(*_tmp3BB)),((_tmp3BB[0]=((_tmp3BA.tag=3,((_tmp3BA.f1=Cyc_CfFlowInfo_ThisIL,_tmp3BA)))),_tmp3BB)))),((_tmp3B1->unknown_all=(void*)(
(_tmp3BD=_region_malloc(r,sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BC.tag=3,((_tmp3BC.f1=Cyc_CfFlowInfo_AllIL,_tmp3BC)))),_tmp3BD)))),((_tmp3B1->esc_none=(void*)(
(_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF)),((_tmp3BF[0]=((_tmp3BE.tag=4,((_tmp3BE.f1=Cyc_CfFlowInfo_NoneIL,_tmp3BE)))),_tmp3BF)))),((_tmp3B1->esc_this=(void*)(
(_tmp3C1=_region_malloc(r,sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C0.tag=4,((_tmp3C0.f1=Cyc_CfFlowInfo_ThisIL,_tmp3C0)))),_tmp3C1)))),((_tmp3B1->esc_all=(void*)(
(_tmp3C3=_region_malloc(r,sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C2.tag=4,((_tmp3C2.f1=Cyc_CfFlowInfo_AllIL,_tmp3C2)))),_tmp3C3)))),((_tmp3B1->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp3B1->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp3B1->dummy_place=(
(_tmp3C6=_region_malloc(r,sizeof(*_tmp3C6)),((_tmp3C6->root=(void*)((_tmp3C4=_region_malloc(r,sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C5.tag=1,((_tmp3C5.f1=& dummy_exp,((_tmp3C5.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp3C5)))))),_tmp3C4)))),((_tmp3C6->fields=0,_tmp3C6)))))),_tmp3B1)))))))))))))))))))))))))));}
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
struct Cyc_CfFlowInfo_Place _stmttmp0=*place;void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=_stmttmp0;_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{
void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2:
# 101
 if(_tmp21 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{
# 104
const char*_tmp3CA;void*_tmp3C9[1];struct Cyc_String_pa_PrintArg_struct _tmp3C8;return(struct _dyneither_ptr)((_tmp3C8.tag=0,((_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp3C9[0]=& _tmp3C8,Cyc_aprintf(((_tmp3CA="reachable from %s",_tag_dyneither(_tmp3CA,sizeof(char),18))),_tag_dyneither(_tmp3C9,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp3CD;void*_tmp3CC;(_tmp3CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3CD="error locations not for VarRoots",_tag_dyneither(_tmp3CD,sizeof(char),33))),_tag_dyneither(_tmp3CC,sizeof(void*),0)));}_LL0:;};}
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
return _tmp2B;}struct _tuple13{void*f1;unsigned int f2;};
# 134
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple13 _tmp30=*((struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(iter,& _tmp30)){
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
# 162
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp38=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp38,f)== 0)return n;
n=n + 1;}{
# 169
struct Cyc_Core_Impossible_exn_struct _tmp3DA;const char*_tmp3D9;void*_tmp3D8[1];struct Cyc_String_pa_PrintArg_struct _tmp3D7;struct Cyc_Core_Impossible_exn_struct*_tmp3D6;(int)_throw((void*)((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3DA.tag=Cyc_Core_Impossible,((_tmp3DA.f1=(struct _dyneither_ptr)((_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp3D8[0]=& _tmp3D7,Cyc_aprintf(((_tmp3D9="get_field_index_fs failed: %s",_tag_dyneither(_tmp3D9,sizeof(char),30))),_tag_dyneither(_tmp3D8,sizeof(void*),1)))))))),_tmp3DA)))),_tmp3D6)))));};}
# 172
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_stmttmp1=Cyc_Tcutil_compress(t);void*_tmp3E=_stmttmp1;union Cyc_Absyn_AggrInfoU _tmp40;struct Cyc_List_List*_tmp42;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E;if(_tmp3F->tag != 11)goto _LLD;else{_tmp40=(_tmp3F->f1).aggr_info;}}_LLC: {
# 175
struct Cyc_List_List*_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp40))->impl))->fields;
_tmp42=_tmp43;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E;if(_tmp41->tag != 12)goto _LLF;else{_tmp42=_tmp41->f2;}}_LLE:
# 178
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10: {
# 180
struct Cyc_Core_Impossible_exn_struct _tmp3E7;const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_PrintArg_struct _tmp3E4;struct Cyc_Core_Impossible_exn_struct*_tmp3E3;(int)_throw((void*)((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=((_tmp3E7.tag=Cyc_Core_Impossible,((_tmp3E7.f1=(struct _dyneither_ptr)((_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp3E5[0]=& _tmp3E4,Cyc_aprintf(((_tmp3E6="get_field_index failed: %s",_tag_dyneither(_tmp3E6,sizeof(char),27))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))))),_tmp3E7)))),_tmp3E3)))));}_LLA:;}
# 185
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp3E8;struct _tuple0 _stmttmp2=(_tmp3E8.f1=r1,((_tmp3E8.f2=r2,_tmp3E8)));struct _tuple0 _tmp49=_stmttmp2;struct Cyc_Absyn_Vardecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4D;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_Absyn_Exp*_tmp53;int _tmp57;int _tmp59;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4A=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp49.f1;if(_tmp4A->tag != 0)goto _LL14;else{_tmp4B=_tmp4A->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp49.f2;if(_tmp4C->tag != 0)goto _LL14;else{_tmp4D=_tmp4C->f1;}};_LL13:
 return(int)_tmp4B - (int)_tmp4D;_LL14: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp49.f1;if(_tmp4E->tag != 0)goto _LL16;}_LL15:
 return - 1;_LL16: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp49.f2;if(_tmp4F->tag != 0)goto _LL18;}_LL17:
 return 1;_LL18:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp50=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp49.f1;if(_tmp50->tag != 1)goto _LL1A;else{_tmp51=_tmp50->f1;}}{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp52=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp49.f2;if(_tmp52->tag != 1)goto _LL1A;else{_tmp53=_tmp52->f1;}};_LL19:
 return(int)_tmp51 - (int)_tmp53;_LL1A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp54=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp49.f1;if(_tmp54->tag != 1)goto _LL1C;}_LL1B:
 return - 1;_LL1C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp49.f2;if(_tmp55->tag != 1)goto _LL1E;}_LL1D:
 return 1;_LL1E:{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp49.f1;if(_tmp56->tag != 2)goto _LL11;else{_tmp57=_tmp56->f1;}}{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp58=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp49.f2;if(_tmp58->tag != 2)goto _LL11;else{_tmp59=_tmp58->f1;}};_LL1F:
 return _tmp57 - _tmp59;_LL11:;};}
# 199
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 201
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 204
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _stmttmp3=*p1;void*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_CfFlowInfo_Place _tmp5B=_stmttmp3;_tmp5C=_tmp5B.root;_tmp5D=_tmp5B.fields;{
struct Cyc_CfFlowInfo_Place _stmttmp4=*p2;void*_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_CfFlowInfo_Place _tmp5E=_stmttmp4;_tmp5F=_tmp5E.root;_tmp60=_tmp5E.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp5C,_tmp5F);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp5D,_tmp60);};};};}
# 216
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _stmttmp5=*p;void*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_CfFlowInfo_Place _tmp61=_stmttmp5;_tmp62=_tmp61.root;_tmp63=_tmp61.fields;
{void*_tmp64=_tmp62;struct Cyc_Absyn_Vardecl*_tmp66;struct Cyc_Absyn_Exp*_tmp68;int _tmp6A;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp64;if(_tmp65->tag != 0)goto _LL23;else{_tmp66=_tmp65->f1;}}_LL22:
{struct _dyneither_ptr*_tmp3F5;const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_PrintArg_struct _tmp3F2;struct Cyc_List_List*_tmp3F1;sl=((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1->hd=((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp66->name).f2),((_tmp3F3[0]=& _tmp3F2,Cyc_aprintf(((_tmp3F4="%s",_tag_dyneither(_tmp3F4,sizeof(char),3))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))))),_tmp3F5)))),((_tmp3F1->tl=sl,_tmp3F1))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp67=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp64;if(_tmp67->tag != 1)goto _LL25;else{_tmp68=_tmp67->f1;}}_LL24:
{struct _dyneither_ptr*_tmp402;const char*_tmp401;void*_tmp400[1];struct Cyc_Int_pa_PrintArg_struct _tmp3FF;struct Cyc_List_List*_tmp3FE;sl=((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE->hd=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=((_tmp3FF.tag=1,((_tmp3FF.f1=(unsigned long)((int)_tmp68),((_tmp400[0]=& _tmp3FF,Cyc_aprintf(((_tmp401="mpt%d",_tag_dyneither(_tmp401,sizeof(char),6))),_tag_dyneither(_tmp400,sizeof(void*),1)))))))),_tmp402)))),((_tmp3FE->tl=sl,_tmp3FE))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp69=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp64;if(_tmp69->tag != 2)goto _LL20;else{_tmp6A=_tmp69->f1;}}_LL26:
{struct _dyneither_ptr*_tmp40F;const char*_tmp40E;void*_tmp40D[1];struct Cyc_Int_pa_PrintArg_struct _tmp40C;struct Cyc_List_List*_tmp40B;sl=((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B->hd=((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=((_tmp40C.tag=1,((_tmp40C.f1=(unsigned long)_tmp6A,((_tmp40D[0]=& _tmp40C,Cyc_aprintf(((_tmp40E="param%d",_tag_dyneither(_tmp40E,sizeof(char),8))),_tag_dyneither(_tmp40D,sizeof(void*),1)))))))),_tmp40F)))),((_tmp40B->tl=sl,_tmp40B))))));}goto _LL20;_LL20:;}
# 224
for(0;_tmp63 != 0;_tmp63=_tmp63->tl){
struct _dyneither_ptr*_tmp41C;const char*_tmp41B;void*_tmp41A[1];struct Cyc_Int_pa_PrintArg_struct _tmp419;struct Cyc_List_List*_tmp418;sl=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C[0]=((_tmp419.tag=1,((_tmp419.f1=(unsigned long)((int)_tmp63->hd),((_tmp41A[0]=& _tmp419,Cyc_aprintf(((_tmp41B="%d",_tag_dyneither(_tmp41B,sizeof(char),3))),_tag_dyneither(_tmp41A,sizeof(void*),1)))))))),_tmp41C)))),((_tmp418->tl=sl,_tmp418))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp427;const char*_tmp426;void*_tmp425[1];const char*_tmp424;struct _dyneither_ptr*_tmp423;struct _dyneither_ptr*_tmp7F=(_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423[0]=((_tmp427.tag=0,((_tmp427.f1=(struct _dyneither_ptr)((_tmp426="",_tag_dyneither(_tmp426,sizeof(char),1))),((_tmp425[0]=& _tmp427,Cyc_aprintf(((_tmp424="%s",_tag_dyneither(_tmp424,sizeof(char),3))),_tag_dyneither(_tmp425,sizeof(void*),1)))))))),_tmp423)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp42C;void*_tmp42B[2];struct Cyc_String_pa_PrintArg_struct _tmp42A;struct Cyc_String_pa_PrintArg_struct _tmp429;*_tmp7F=((_tmp429.tag=0,((_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F),((_tmp42A.tag=0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp42B[0]=& _tmp42A,((_tmp42B[1]=& _tmp429,Cyc_aprintf(((_tmp42C="%s.%s",_tag_dyneither(_tmp42C,sizeof(char),6))),_tag_dyneither(_tmp42B,sizeof(void*),2))))))))))))));}
return _tmp7F;};}
# 242 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp432(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp431,unsigned int*_tmp430,void***_tmp42E){for(*_tmp431=0;*_tmp431 < *_tmp430;(*_tmp431)++){(*_tmp42E)[*_tmp431]=(void*)(*fenv)->unknown_all;}}
# 247
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 253
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp431;unsigned int _tmp430;struct _dyneither_ptr _tmp42F;void**_tmp42E;unsigned int _tmp42D;struct _dyneither_ptr d=(_tmp42D=sz,((_tmp42E=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp42D)),((_tmp42F=_tag_dyneither(_tmp42E,sizeof(void*),_tmp42D),((((_tmp430=_tmp42D,_tmp432(& fenv,& _tmp431,& _tmp430,& _tmp42E))),_tmp42F)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmp6=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct _dyneither_ptr*_tmp8A;void*_tmp8B;struct Cyc_Absyn_Aggrfield*_tmp89=_stmttmp6;_tmp8A=_tmp89->name;_tmp8B=_tmp89->type;
if(_get_dyneither_size(*_tmp8A,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp8B,leafval);}}
# 261
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 265
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 269
struct _RegionHandle*_tmp91;struct Cyc_List_List*_tmp92;struct _tuple14*_tmp90=env;_tmp91=_tmp90->f1;_tmp92=_tmp90->f2;{
void*_tmp93=Cyc_Tcutil_rsubstitute(_tmp91,_tmp92,f->type);
struct Cyc_Absyn_Exp*_tmp94=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp95=_tmp94 == 0?0: Cyc_Tcutil_rsubsexp(_tmp91,_tmp92,_tmp94);
struct Cyc_Absyn_Aggrfield*_tmp433;return(_tmp433=_region_malloc(_tmp91,sizeof(*_tmp433)),((_tmp433->name=f->name,((_tmp433->tq=f->tq,((_tmp433->type=_tmp93,((_tmp433->width=f->width,((_tmp433->attributes=f->attributes,((_tmp433->requires_clause=_tmp95,_tmp433)))))))))))));};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 280
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 286
struct _RegionHandle _tmp97=_new_region("temp");struct _RegionHandle*temp=& _tmp97;_push_region(temp);
# 290
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp98=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp99=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_stmttmp7=Cyc_Tcutil_tvar_kind(_tmp98,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp9A=_stmttmp7;_LL28: if(_tmp9A->kind != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
 goto _LL2B;_LL2A: if(_tmp9A->kind != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
 continue;_LL2C:;_LL2D:
 goto _LL27;_LL27:;}{
# 299
struct _tuple15*_tmp436;struct Cyc_List_List*_tmp435;inst=((_tmp435=_region_malloc(temp,sizeof(*_tmp435)),((_tmp435->hd=((_tmp436=_region_malloc(temp,sizeof(*_tmp436)),((_tmp436->f1=_tmp98,((_tmp436->f2=_tmp99,_tmp436)))))),((_tmp435->tl=inst,_tmp435))))));};}
# 301
if(inst != 0){
struct _tuple14 _tmp437;struct _tuple14 _tmp9D=(_tmp437.f1=temp,((_tmp437.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp437)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp9D,fs);
struct _dyneither_ptr _tmp9E=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp9E;}else{
# 306
struct _dyneither_ptr _tmpA0=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA0;}}
# 290
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp43D(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp43C,unsigned int*_tmp43B,void***_tmp439){for(*_tmp43C=0;*_tmp43C < *_tmp43B;(*_tmp43C)++){(*_tmp439)[*_tmp43C]=(void*)(*fenv)->unknown_all;}}
# 310
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 316
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 318
{void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmpA1=_stmttmp8;struct Cyc_Absyn_Datatypefield*_tmpA3;struct Cyc_List_List*_tmpA5;union Cyc_Absyn_AggrInfoU _tmpA7;struct Cyc_List_List*_tmpA8;enum Cyc_Absyn_AggrKind _tmpAA;struct Cyc_List_List*_tmpAB;void*_tmpAD;union Cyc_Absyn_Constraint*_tmpAE;void*_tmpB0;union Cyc_Absyn_Constraint*_tmpB2;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA1;if(_tmpA2->tag != 4)goto _LL31;else{if((((_tmpA2->f1).field_info).KnownDatatypefield).tag != 2)goto _LL31;_tmpA3=((struct _tuple2)(((_tmpA2->f1).field_info).KnownDatatypefield).val).f2;}}_LL30:
# 320
 if(_tmpA3->typs == 0)
return leafval;
_tmpA5=_tmpA3->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA1;if(_tmpA4->tag != 10)goto _LL33;else{_tmpA5=_tmpA4->f1;}}_LL32: {
# 324
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA5);
# 327
unsigned int _tmp43C;unsigned int _tmp43B;struct _dyneither_ptr _tmp43A;void**_tmp439;unsigned int _tmp438;struct _dyneither_ptr d=(_tmp438=sz,((_tmp439=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp438)),((_tmp43A=_tag_dyneither(_tmp439,sizeof(void*),_tmp438),((((_tmp43B=_tmp438,_tmp43D(& fenv,& _tmp43C,& _tmp43B,& _tmp439))),_tmp43A)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpA5))->hd)).f2,leafval);
_tmpA5=_tmpA5->tl;}}{
# 332
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp443;struct Cyc_CfFlowInfo_UnionRInfo _tmp442;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp441;return(void*)((_tmp441=_region_malloc(fenv->r,sizeof(*_tmp441)),((_tmp441[0]=((_tmp443.tag=6,((_tmp443.f1=((_tmp442.is_union=0,((_tmp442.fieldnum=- 1,_tmp442)))),((_tmp443.f2=d,_tmp443)))))),_tmp441))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA1;if(_tmpA6->tag != 11)goto _LL35;else{_tmpA7=(_tmpA6->f1).aggr_info;_tmpA8=(_tmpA6->f1).targs;}}_LL34: {
# 334
struct Cyc_Absyn_Aggrdecl*_tmpBA=Cyc_Absyn_get_known_aggrdecl(_tmpA7);
if(_tmpBA->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpBB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields;
if(_tmpA8 == 0){_tmpAA=_tmpBA->kind;_tmpAB=_tmpBB;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp449;struct Cyc_CfFlowInfo_UnionRInfo _tmp448;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp447;return(void*)((_tmp447=_region_malloc(fenv->r,sizeof(*_tmp447)),((_tmp447[0]=(
(_tmp449.tag=6,((_tmp449.f1=((_tmp448.is_union=_tmpBA->kind == Cyc_Absyn_UnionA,((_tmp448.fieldnum=- 1,_tmp448)))),((_tmp449.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpBA->tvs,_tmpA8,_tmpBB,_tmpBA->kind == Cyc_Absyn_UnionA,leafval),_tmp449)))))),_tmp447))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA1;if(_tmpA9->tag != 12)goto _LL37;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpA9->f2;}}_LL36: {
# 345
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp44F;struct Cyc_CfFlowInfo_UnionRInfo _tmp44E;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp44D;return(void*)((_tmp44D=_region_malloc(fenv->r,sizeof(*_tmp44D)),((_tmp44D[0]=((_tmp44F.tag=6,((_tmp44F.f1=((_tmp44E.is_union=_tmpAA == Cyc_Absyn_UnionA,((_tmp44E.fieldnum=- 1,_tmp44E)))),((_tmp44F.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpAB,_tmpAA == Cyc_Absyn_UnionA,leafval),_tmp44F)))))),_tmp44D))));}_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1;if(_tmpAC->tag != 8)goto _LL39;else{_tmpAD=(_tmpAC->f1).elt_type;_tmpAE=(_tmpAC->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAE))goto _LL39;_LL38:
# 349
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpAD)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA1;if(_tmpAF->tag != 19)goto _LL3B;else{_tmpB0=(void*)_tmpAF->f1;}}_LL3A:
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1;if(_tmpB1->tag != 5)goto _LL3D;else{_tmpB2=((_tmpB1->f1).ptr_atts).nullable;}}if(!(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB2)))goto _LL3D;_LL3C:
{void*_tmpC2=leafval;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC3=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC2;if(_tmpC3->tag != 3)goto _LL42;else{if(_tmpC3->f1 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC2;if(_tmpC4->tag != 3)goto _LL44;else{if(_tmpC4->f1 != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
 return fenv->notzeroall;_LL44:;_LL45:
 goto _LL3F;_LL3F:;}
# 357
goto _LL2E;_LL3D:;_LL3E:
 goto _LL2E;_LL2E:;}
# 361
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 364
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 370
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpC5=r;struct Cyc_Absyn_Exp*_tmpC7;int _tmpC8;void*_tmpC9;int _tmpCB;int _tmpCC;struct _dyneither_ptr _tmpCD;void*_tmpCF;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpC6->tag != 7)goto _LL49;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;_tmpC9=(void*)_tmpC6->f3;}}_LL48:
# 373
 if(_tmpC7 == e  && _tmpC8 == env_iteration){
*needs_unconsume=1;
return 0;}
# 377
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpCA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCA->tag != 6)goto _LL4B;else{_tmpCB=(_tmpCA->f1).is_union;_tmpCC=(_tmpCA->f1).fieldnum;_tmpCD=_tmpCA->f2;}}_LL4A:
# 379
 if(!_tmpCB  || _tmpCC == - 1){
unsigned int _tmpD0=_get_dyneither_size(_tmpCD,sizeof(void*));
{int i=0;for(0;i < _tmpD0;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpCD.curr)[i],needs_unconsume))
return 1;}}
# 385
return 0;}else{
# 388
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpCD,sizeof(void*),_tmpCC)),needs_unconsume);}_LL4B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCE->tag != 8)goto _LL4D;else{_tmpCF=(void*)_tmpCE->f2;}}_LL4C:
# 390
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpCF,needs_unconsume);_LL4D:;_LL4E:
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp455(struct _dyneither_ptr*_tmpD8,unsigned int*_tmp454,unsigned int*_tmp453,void***_tmp451){for(*_tmp454=0;*_tmp454 < *_tmp453;(*_tmp454)++){(*_tmp451)[*_tmp454]=(void*)((void**)(*_tmpD8).curr)[(int)*_tmp454];}}
# 397
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpD1=r;struct Cyc_Absyn_Exp*_tmpD3;int _tmpD4;void*_tmpD5;struct Cyc_CfFlowInfo_UnionRInfo _tmpD7;struct _dyneither_ptr _tmpD8;struct Cyc_Absyn_Vardecl*_tmpDA;void*_tmpDB;_LL50: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD1;if(_tmpD2->tag != 7)goto _LL52;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD2->f2;_tmpD5=(void*)_tmpD2->f3;}}_LL51:
# 400
 return _tmpD5;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD1;if(_tmpD6->tag != 6)goto _LL54;else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD6->f2;}}_LL53: {
# 402
unsigned int _tmpDC=_get_dyneither_size(_tmpD8,sizeof(void*));
int change=0;
unsigned int _tmp454;unsigned int _tmp453;struct _dyneither_ptr _tmp452;void**_tmp451;unsigned int _tmp450;struct _dyneither_ptr d2=(_tmp450=_tmpDC,((_tmp451=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp450)),((_tmp452=_tag_dyneither(_tmp451,sizeof(void*),_tmp450),((((_tmp453=_tmp450,_tmp455(& _tmpD8,& _tmp454,& _tmp453,& _tmp451))),_tmp452)))))));
{int i=0;for(0;i < _tmpDC;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpD8.curr)[i]);
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= ((void**)_tmpD8.curr)[i])
change=1;}}
# 410
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp458;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp457;return(void*)((_tmp457=_region_malloc(fenv->r,sizeof(*_tmp457)),((_tmp457[0]=((_tmp458.tag=6,((_tmp458.f1=_tmpD7,((_tmp458.f2=d2,_tmp458)))))),_tmp457))));}else{
return r;}}_LL54: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD1;if(_tmpD9->tag != 8)goto _LL56;else{_tmpDA=_tmpD9->f1;_tmpDB=(void*)_tmpD9->f2;}}_LL55: {
# 414
void*_tmpE3=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpDB);
if(_tmpE3 != _tmpDB){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp45B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp45A;return(void*)((_tmp45A=_region_malloc(fenv->r,sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp45B.tag=8,((_tmp45B.f1=_tmpDA,((_tmp45B.f2=_tmpE3,_tmp45B)))))),_tmp45A))));}else{
return r;}}_LL56:;_LL57:
 return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp465(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp464,unsigned int*_tmp463,void***_tmp461){for(*_tmp464=0;*_tmp464 < *_tmp463;(*_tmp464)++){(*_tmp461)[*_tmp464]=(void*)(*fenv)->unknown_all;}}static void _tmp46E(struct _dyneither_ptr*_tmpF9,unsigned int*_tmp46D,unsigned int*_tmp46C,void***_tmp46A){for(*_tmp46D=0;*_tmp46D < *_tmp46C;(*_tmp46D)++){(*_tmp46A)[*_tmp46D]=(void*)((void**)(*_tmpF9).curr)[(int)*_tmp46D];}}
# 424
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp45C;struct _tuple0 _stmttmp9=(_tmp45C.f1=Cyc_Tcutil_compress(t),((_tmp45C.f2=r,_tmp45C)));struct _tuple0 _tmpE6=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmpE8;void*_tmpE9;struct Cyc_List_List*_tmpEB;struct Cyc_CfFlowInfo_UnionRInfo _tmpED;struct _dyneither_ptr _tmpEE;union Cyc_Absyn_AggrInfoU _tmpF0;struct Cyc_CfFlowInfo_UnionRInfo _tmpF2;struct _dyneither_ptr _tmpF3;enum Cyc_Absyn_AggrKind _tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_CfFlowInfo_UnionRInfo _tmpF8;struct _dyneither_ptr _tmpF9;_LL59: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE6.f2;if(_tmpE7->tag != 8)goto _LL5B;else{_tmpE8=_tmpE7->f1;_tmpE9=(void*)_tmpE7->f2;}}_LL5A: {
# 427
void*_tmpFA=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpE9);
if(_tmpFA != _tmpE9){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp45F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp45E;return(void*)((_tmp45E=_region_malloc(fenv->r,sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp45F.tag=8,((_tmp45F.f1=_tmpE8,((_tmp45F.f2=_tmpFA,_tmp45F)))))),_tmp45E))));}else{
return r;}}_LL5B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE6.f1;if(_tmpEA->tag != 10)goto _LL5D;else{_tmpEB=_tmpEA->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE6.f2;if(_tmpEC->tag != 6)goto _LL5D;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}};_LL5C: {
# 431
unsigned int _tmpFD=_get_dyneither_size(_tmpEE,sizeof(void*));
unsigned int _tmp464;unsigned int _tmp463;struct _dyneither_ptr _tmp462;void**_tmp461;unsigned int _tmp460;struct _dyneither_ptr d2=(_tmp460=_tmpFD,((_tmp461=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp460)),((_tmp462=_tag_dyneither(_tmp461,sizeof(void*),_tmp460),((((_tmp463=_tmp460,_tmp465(& fenv,& _tmp464,& _tmp463,& _tmp461))),_tmp462)))))));
{int i=0;for(0;i < _tmpFD;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpEB))->hd)).f2,consumer,iteration,((void**)_tmpEE.curr)[i]);
_tmpEB=_tmpEB->tl;}}{
# 437
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp468;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp467;return(void*)((_tmp467=_region_malloc(fenv->r,sizeof(*_tmp467)),((_tmp467[0]=((_tmp468.tag=6,((_tmp468.f1=_tmpED,((_tmp468.f2=d2,_tmp468)))))),_tmp467))));};}_LL5D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE6.f1;if(_tmpEF->tag != 11)goto _LL5F;else{_tmpF0=(_tmpEF->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE6.f2;if(_tmpF1->tag != 6)goto _LL5F;else{_tmpF2=_tmpF1->f1;_tmpF3=_tmpF1->f2;}};_LL5E: {
# 439
struct Cyc_Absyn_Aggrdecl*_tmp104=Cyc_Absyn_get_known_aggrdecl(_tmpF0);
if(_tmp104->impl == 0)return r;
_tmpF5=_tmp104->kind;_tmpF6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp104->impl))->fields;_tmpF8=_tmpF2;_tmpF9=_tmpF3;goto _LL60;}_LL5F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE6.f1;if(_tmpF4->tag != 12)goto _LL61;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE6.f2;if(_tmpF7->tag != 6)goto _LL61;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}};_LL60: {
# 443
unsigned int _tmp46D;unsigned int _tmp46C;struct _dyneither_ptr _tmp46B;void**_tmp46A;unsigned int _tmp469;struct _dyneither_ptr d2=(_tmp469=_get_dyneither_size(_tmpF9,sizeof(void*)),((_tmp46A=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp469)),((_tmp46B=_tag_dyneither(_tmp46A,sizeof(void*),_tmp469),((((_tmp46C=_tmp469,_tmp46E(& _tmpF9,& _tmp46D,& _tmp46C,& _tmp46A))),_tmp46B)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF6);
{int i=0;for(0;i < sz;(i ++,_tmpF6=_tmpF6->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmpA=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpF6))->hd;struct _dyneither_ptr*_tmp106;void*_tmp107;struct Cyc_Absyn_Aggrfield*_tmp105=_stmttmpA;_tmp106=_tmp105->name;_tmp107=_tmp105->type;
if(_get_dyneither_size(*_tmp106,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp107,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF9,sizeof(void*),i)));}}{
# 450
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp471;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp470;return(void*)((_tmp470=_region_malloc(fenv->r,sizeof(*_tmp470)),((_tmp470[0]=((_tmp471.tag=6,((_tmp471.f1=_tmpF8,((_tmp471.f2=d2,_tmp471)))))),_tmp470))));};}_LL61:;_LL62:
# 452
 if(Cyc_Tcutil_is_noalias_pointer(t,0)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp474;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp473;return(void*)((_tmp473=_region_malloc(fenv->r,sizeof(*_tmp473)),((_tmp473[0]=((_tmp474.tag=7,((_tmp474.f1=consumer,((_tmp474.f2=iteration,((_tmp474.f3=r,_tmp474)))))))),_tmp473))));}else{
return r;}_LL58:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 458
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 461
struct _RegionHandle _tmp111=_new_region("r");struct _RegionHandle*r=& _tmp111;_push_region(r);
{struct _tuple17 _tmp475;struct _tuple17 elem=(_tmp475.f1=place,((_tmp475.f2=0,_tmp475)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp112=elem.f1;
struct Cyc_CfFlowInfo_Place _stmttmpB=*place;void*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_CfFlowInfo_Place _tmp113=_stmttmpB;_tmp114=_tmp113.root;_tmp115=_tmp113.fields;{
struct Cyc_CfFlowInfo_Place _stmttmpC=*_tmp112;void*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_CfFlowInfo_Place _tmp116=_stmttmpC;_tmp117=_tmp116.root;_tmp118=_tmp116.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmp114,_tmp117)!= 0)
continue;
for(0;_tmp115 != 0  && _tmp118 != 0;(_tmp115=_tmp115->tl,_tmp118=_tmp118->tl)){
if((int)_tmp115->hd != (int)_tmp118->hd)break;}
if(_tmp115 == 0){
int _tmp119=1;_npop_handler(0);return _tmp119;}};}{
# 475
int _tmp11A=0;_npop_handler(0);return _tmp11A;};}
# 462
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 488
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 490
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 493
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp476;pile->places=((_tmp476=_region_malloc(pile->rgn,sizeof(*_tmp476)),((_tmp476->hd=place,((_tmp476->tl=pile->places,_tmp476))))));}}
# 496
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp11D=r;void*_tmp11F;void*_tmp121;struct Cyc_CfFlowInfo_Place*_tmp123;struct Cyc_CfFlowInfo_UnionRInfo _tmp125;struct _dyneither_ptr _tmp126;_LL64: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp11E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp11D;if(_tmp11E->tag != 7)goto _LL66;else{_tmp11F=(void*)_tmp11E->f3;}}_LL65:
 Cyc_CfFlowInfo_add_places(pile,_tmp11F);return;_LL66: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp120=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11D;if(_tmp120->tag != 8)goto _LL68;else{_tmp121=(void*)_tmp120->f2;}}_LL67:
 Cyc_CfFlowInfo_add_places(pile,_tmp121);return;_LL68: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp122=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp11D;if(_tmp122->tag != 5)goto _LL6A;else{_tmp123=_tmp122->f1;}}_LL69:
 Cyc_CfFlowInfo_add_place(pile,_tmp123);return;_LL6A: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp124=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp11D;if(_tmp124->tag != 6)goto _LL6C;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL6B:
# 502
{int i=0;for(0;i < _get_dyneither_size(_tmp126,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp126.curr)[i]);}}
return;_LL6C:;_LL6D:
 return;_LL63:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp47C(struct _RegionHandle**r,void**new_val,unsigned int*_tmp47B,unsigned int*_tmp47A,void***_tmp478,struct _dyneither_ptr*_tmp12A){for(*_tmp47B=0;*_tmp47B < *_tmp47A;(*_tmp47B)++){(*_tmp478)[*_tmp47B]=(void*)
# 517
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp12A).curr)[(int)*_tmp47B]);}}
# 512
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 514
void*_tmp127=old_val;int _tmp129;struct _dyneither_ptr _tmp12A;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12D;void*_tmp12E;struct Cyc_Absyn_Vardecl*_tmp130;void*_tmp131;_LL6F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp128=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp127;if(_tmp128->tag != 6)goto _LL71;else{_tmp129=(_tmp128->f1).is_union;_tmp12A=_tmp128->f2;}}_LL70: {
# 516
unsigned int _tmp47B;unsigned int _tmp47A;struct _dyneither_ptr _tmp479;void**_tmp478;unsigned int _tmp477;struct _dyneither_ptr d2=(_tmp477=_get_dyneither_size(_tmp12A,sizeof(void*)),((_tmp478=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp477)),((_tmp479=_tag_dyneither(_tmp478,sizeof(void*),_tmp477),((((_tmp47A=_tmp477,_tmp47C(& r,& new_val,& _tmp47B,& _tmp47A,& _tmp478,& _tmp12A))),_tmp479)))))));
# 520
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp482;struct Cyc_CfFlowInfo_UnionRInfo _tmp481;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp480;return(void*)((_tmp480=_region_malloc(r,sizeof(*_tmp480)),((_tmp480[0]=((_tmp482.tag=6,((_tmp482.f1=((_tmp481.is_union=_tmp129,((_tmp481.fieldnum=- 1,_tmp481)))),((_tmp482.f2=d2,_tmp482)))))),_tmp480))));}_LL71: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp127;if(_tmp12B->tag != 7)goto _LL73;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;_tmp12E=(void*)_tmp12B->f3;}}_LL72: {
# 522
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp485;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp484;return(void*)((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484[0]=((_tmp485.tag=7,((_tmp485.f1=_tmp12C,((_tmp485.f2=_tmp12D,((_tmp485.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp12E),_tmp485)))))))),_tmp484))));}_LL73: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp127;if(_tmp12F->tag != 8)goto _LL75;else{_tmp130=_tmp12F->f1;_tmp131=(void*)_tmp12F->f2;}}_LL74: {
# 524
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp488;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp487;return(void*)((_tmp487=_region_malloc(r,sizeof(*_tmp487)),((_tmp487[0]=((_tmp488.tag=8,((_tmp488.f1=_tmp130,((_tmp488.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp131),_tmp488)))))),_tmp487))));}_LL75:;_LL76:
 return new_val;_LL6E:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp48E(struct _dyneither_ptr*d,unsigned int*_tmp48D,unsigned int*_tmp48C,void***_tmp48A){for(*_tmp48D=0;*_tmp48D < *_tmp48C;(*_tmp48D)++){(*_tmp48A)[*_tmp48D]=(void*)((void**)(*d).curr)[(int)*_tmp48D];}}
# 531
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 533
void*_tmp13D=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp13D == rval)return d;{
unsigned int _tmp48D;unsigned int _tmp48C;struct _dyneither_ptr _tmp48B;void**_tmp48A;unsigned int _tmp489;struct _dyneither_ptr res=(_tmp489=_get_dyneither_size(d,sizeof(void*)),((_tmp48A=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp489)),((_tmp48B=_tag_dyneither(_tmp48A,sizeof(void*),_tmp489),((((_tmp48C=_tmp489,_tmp48E(& d,& _tmp48D,& _tmp48C,& _tmp48A))),_tmp48B)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 545
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 549
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple18 _tmp48F;struct _tuple18 _stmttmpD=(_tmp48F.f1=fs,((_tmp48F.f2=old_val,_tmp48F)));struct _tuple18 _tmp142=_stmttmpD;int _tmp143;struct Cyc_List_List*_tmp144;int _tmp146;struct _dyneither_ptr _tmp147;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;_LL78: if(_tmp142.f1 == 0)goto _LL7A;_tmp143=(int)(_tmp142.f1)->hd;_tmp144=(_tmp142.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp145=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp142.f2;if(_tmp145->tag != 6)goto _LL7A;else{_tmp146=(_tmp145->f1).is_union;_tmp147=_tmp145->f2;}};_LL79: {
# 553
void*_tmp14B=Cyc_CfFlowInfo_insert_place_outer(r,_tmp144,*((void**)_check_dyneither_subscript(_tmp147,sizeof(void*),_tmp143)),new_val);
struct _dyneither_ptr _tmp14C=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp147,_tmp143,_tmp14B);
if((void**)_tmp14C.curr == (void**)_tmp147.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp495;struct Cyc_CfFlowInfo_UnionRInfo _tmp494;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp493;return(void*)((_tmp493=_region_malloc(r,sizeof(*_tmp493)),((_tmp493[0]=((_tmp495.tag=6,((_tmp495.f1=((_tmp494.is_union=_tmp146,((_tmp494.fieldnum=- 1,_tmp494)))),((_tmp495.f2=_tmp14C,_tmp495)))))),_tmp493))));}}_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp148=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp142.f2;if(_tmp148->tag != 8)goto _LL7C;else{_tmp149=_tmp148->f1;_tmp14A=(void*)_tmp148->f2;}}_LL7B: {
# 558
void*_tmp150=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp14A,new_val);
if(_tmp150 == _tmp14A)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp498;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp497;return(void*)((_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497[0]=((_tmp498.tag=8,((_tmp498.f1=_tmp149,((_tmp498.f2=_tmp150,_tmp498)))))),_tmp497))));}}_LL7C:;_LL7D: {
struct Cyc_Core_Impossible_exn_struct _tmp49E;const char*_tmp49D;struct Cyc_Core_Impossible_exn_struct*_tmp49C;(int)_throw((void*)((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C[0]=((_tmp49E.tag=Cyc_Core_Impossible,((_tmp49E.f1=((_tmp49D="bad insert place",_tag_dyneither(_tmp49D,sizeof(char),17))),_tmp49E)))),_tmp49C)))));}_LL77:;};}
# 568
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 572
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp157=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp157,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp157);;_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15C=_tmp159;void*_tmp15E;_LL7F: {struct Cyc_Dict_Absent_exn_struct*_tmp15D=(struct Cyc_Dict_Absent_exn_struct*)_tmp15C;if(_tmp15D->tag != Cyc_Dict_Absent)goto _LL81;}_LL80:
 continue;_LL81: _tmp15E=_tmp15C;_LL82:(void)_throw(_tmp15E);_LL7E:;}};}
{enum Cyc_CfFlowInfo_InitLevel _stmttmpE=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);switch(_stmttmpE){case Cyc_CfFlowInfo_AllIL: _LL83:
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL84:
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL85:
 newval=fenv->esc_none;break;}}
# 584
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _stmttmpF=*_tmp157;void*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_CfFlowInfo_Place _tmp15F=_stmttmpF;_tmp160=_tmp15F.root;_tmp161=_tmp15F.fields;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp160,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp161,
# 589
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp160),newval));};};}
# 592
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 604
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp162=r;enum Cyc_CfFlowInfo_InitLevel _tmp164;enum Cyc_CfFlowInfo_InitLevel _tmp166;void*_tmp16B;_LL88: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp163=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp163->tag != 3)goto _LL8A;else{_tmp164=_tmp163->f1;}}_LL89:
 return _tmp164;_LL8A: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp165=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp165->tag != 4)goto _LL8C;else{_tmp166=_tmp165->f1;}}_LL8B:
 return _tmp166;_LL8C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp167=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp167->tag != 0)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp168=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp168->tag != 1)goto _LL90;}_LL8F:
 return Cyc_CfFlowInfo_AllIL;_LL90: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp169=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp169->tag != 2)goto _LL92;}_LL91:
 return Cyc_CfFlowInfo_ThisIL;_LL92: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp16A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp162;if(_tmp16A->tag != 7)goto _LL94;else{_tmp16B=(void*)_tmp16A->f3;}}_LL93:
 return Cyc_CfFlowInfo_NoneIL;_LL94:;_LL95: {
struct Cyc_Core_Impossible_exn_struct _tmp4A4;const char*_tmp4A3;struct Cyc_Core_Impossible_exn_struct*_tmp4A2;(int)_throw((void*)((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A4.tag=Cyc_Core_Impossible,((_tmp4A4.f1=((_tmp4A3="initlevel_approx",_tag_dyneither(_tmp4A3,sizeof(char),17))),_tmp4A4)))),_tmp4A2)))));}_LL87:;}
# 615
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 617
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp16F=r;void*_tmp171;int _tmp173;struct _dyneither_ptr _tmp174;struct _dyneither_ptr _tmp176;struct Cyc_CfFlowInfo_Place*_tmp178;_LL97: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp170=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16F;if(_tmp170->tag != 8)goto _LL99;else{_tmp171=(void*)_tmp170->f2;}}_LL98:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp171,acc);_LL99: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp172=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16F;if(_tmp172->tag != 6)goto _LL9B;else{if((_tmp172->f1).is_union != 1)goto _LL9B;_tmp173=(_tmp172->f1).fieldnum;_tmp174=_tmp172->f2;}}_LL9A: {
# 624
unsigned int _tmp179=_get_dyneither_size(_tmp174,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp173 == - 1){
int i=0;for(0;i < _tmp179;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp174.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 634
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp174,sizeof(void*),_tmp173)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL96;}_LL9B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp175=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16F;if(_tmp175->tag != 6)goto _LL9D;else{if((_tmp175->f1).is_union != 0)goto _LL9D;_tmp176=_tmp175->f2;}}_LL9C: {
# 638
unsigned int _tmp17A=_get_dyneither_size(_tmp176,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp17A;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp176.curr)[i],this_ans);}}
goto _LL96;}_LL9D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp177=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16F;if(_tmp177->tag != 5)goto _LL9F;else{_tmp178=_tmp177->f1;}}_LL9E:
# 644
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp178))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 647
{struct Cyc_List_List*_tmp4A5;env->seen=((_tmp4A5=_region_malloc(env->e,sizeof(*_tmp4A5)),((_tmp4A5->hd=_tmp178,((_tmp4A5->tl=env->seen,_tmp4A5))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp178),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 653
goto _LL96;_LL9F:;_LLA0:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL96:;}
# 656
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 660
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp17C=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp4A6;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17D=(_tmp4A6.e=_tmp17C,((_tmp4A6.d=d,((_tmp4A6.seen=0,_tmp4A6)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp17D,r,Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};
# 667
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _stmttmp10=*place;void*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_CfFlowInfo_Place _tmp17F=_stmttmp10;_tmp180=_tmp17F.root;_tmp181=_tmp17F.fields;{
void*_tmp182=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp180);
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
struct _tuple19 _tmp4A7;struct _tuple19 _stmttmp11=(_tmp4A7.f1=_tmp182,((_tmp4A7.f2=(int)_tmp181->hd,_tmp4A7)));struct _tuple19 _tmp183=_stmttmp11;struct Cyc_CfFlowInfo_UnionRInfo _tmp186;struct _dyneither_ptr _tmp187;int _tmp188;struct Cyc_CfFlowInfo_UnionRInfo _tmp18A;struct _dyneither_ptr _tmp18B;int _tmp18C;_LLA2:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp184=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp183.f1;if(_tmp184->tag != 8)goto _LLA4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp185=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp184->f2);if(_tmp185->tag != 6)goto _LLA4;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}}_tmp188=_tmp183.f2;_LLA3:
# 673
 _tmp18A=_tmp186;_tmp18B=_tmp187;_tmp18C=_tmp188;goto _LLA5;_LLA4:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp189=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp183.f1;if(_tmp189->tag != 6)goto _LLA6;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_tmp18C=_tmp183.f2;_LLA5:
# 675
 _tmp182=*((void**)_check_dyneither_subscript(_tmp18B,sizeof(void*),_tmp18C));
goto _LLA1;_LLA6:;_LLA7: {
# 681
struct Cyc_Core_Impossible_exn_struct _tmp4AD;const char*_tmp4AC;struct Cyc_Core_Impossible_exn_struct*_tmp4AB;(int)_throw((void*)((_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB[0]=((_tmp4AD.tag=Cyc_Core_Impossible,((_tmp4AD.f1=((_tmp4AC="bad lookup_place",_tag_dyneither(_tmp4AC,sizeof(char),17))),_tmp4AD)))),_tmp4AB)))));}_LLA1:;}
# 683
return _tmp182;};}
# 686
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp191=rval;void*_tmp194;void*_tmp196;int _tmp198;int _tmp199;struct _dyneither_ptr _tmp19A;_LLA9: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp192=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp191;if(_tmp192->tag != 4)goto _LLAB;}_LLAA:
 return 0;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp193=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp191;if(_tmp193->tag != 7)goto _LLAD;else{_tmp194=(void*)_tmp193->f3;}}_LLAC:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp194);_LLAD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp195=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp191;if(_tmp195->tag != 8)goto _LLAF;else{_tmp196=(void*)_tmp195->f2;}}_LLAE:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp196);_LLAF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp197=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp191;if(_tmp197->tag != 6)goto _LLB1;else{_tmp198=(_tmp197->f1).is_union;_tmp199=(_tmp197->f1).fieldnum;_tmp19A=_tmp197->f2;}}_LLB0:
# 692
 if(_tmp198  && _tmp199 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp19A,sizeof(void*),_tmp199)));else{
# 695
unsigned int sz=_get_dyneither_size(_tmp19A,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp19A.curr)[i]))return 0;}}
return 1;}_LLB1:;_LLB2:
# 700
 return 1;_LLA8:;}
# 703
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 708
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp19B=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp4AE;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp4AE=_region_malloc(_tmp19B,sizeof(*_tmp4AE)),((_tmp4AE->rgn=_tmp19B,((_tmp4AE->places=0,_tmp4AE)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp4BB(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp4BA,unsigned int*_tmp4B9,void***_tmp4B7,struct _dyneither_ptr*_tmp1AC,struct _dyneither_ptr*_tmp1A9){for(*_tmp4BA=0;*_tmp4BA < *_tmp4B9;(*_tmp4BA)++){(*_tmp4B7)[*_tmp4BA]=(void*)
# 746
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1A9).curr)[(int)*_tmp4BA],*((void**)_check_dyneither_subscript(*_tmp1AC,sizeof(void*),(int)*_tmp4BA)));}}
# 726
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 731
struct _tuple0 _tmp4AF;struct _tuple0 _stmttmp12=(_tmp4AF.f1=oldval,((_tmp4AF.f2=newval,_tmp4AF)));struct _tuple0 _tmp19D=_stmttmp12;void*_tmp19F;struct Cyc_Absyn_Vardecl*_tmp1A1;void*_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp1A5;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A8;struct _dyneither_ptr _tmp1A9;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AB;struct _dyneither_ptr _tmp1AC;enum Cyc_CfFlowInfo_InitLevel _tmp1AE;_LLB4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp19E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19D.f1;if(_tmp19E->tag != 8)goto _LLB6;else{_tmp19F=(void*)_tmp19E->f2;}}_LLB5:
# 733
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp19F,newval);_LLB6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp19D.f2;if(_tmp1A0->tag != 8)goto _LLB8;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=(void*)_tmp1A0->f2;}}_LLB7: {
# 735
void*_tmp1AF=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp1A2);
if(_tmp1AF == _tmp1A2)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4B2;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B1;return(void*)((_tmp4B1=_region_malloc((env->fenv)->r,sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B2.tag=8,((_tmp4B2.f1=_tmp1A1,((_tmp4B2.f2=_tmp1AF,_tmp4B2)))))),_tmp4B1))));}}_LLB8:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A3=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19D.f1;if(_tmp1A3->tag != 4)goto _LLBA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1A4=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19D.f2;if(_tmp1A4->tag != 5)goto _LLBA;else{_tmp1A5=_tmp1A4->f1;}};_LLB9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A5);goto _LLBB;_LLBA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19D.f1;if(_tmp1A6->tag != 4)goto _LLBC;}_LLBB:
# 740
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp4B5;void*_tmp4B4;(_tmp4B4=0,Cyc_Tcutil_terr(env->loc,((_tmp4B5="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp4B5,sizeof(char),67))),_tag_dyneither(_tmp4B4,sizeof(void*),0)));}
# 743
return(env->fenv)->esc_all;_LLBC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1A7=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19D.f1;if(_tmp1A7->tag != 6)goto _LLBE;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp19D.f2;if(_tmp1AA->tag != 6)goto _LLBE;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}};_LLBD: {
# 745
unsigned int _tmp4BA;unsigned int _tmp4B9;struct _dyneither_ptr _tmp4B8;void**_tmp4B7;unsigned int _tmp4B6;struct _dyneither_ptr new_d=
(_tmp4B6=_get_dyneither_size(_tmp1A9,sizeof(void*)),((_tmp4B7=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp4B6)),((_tmp4B8=_tag_dyneither(_tmp4B7,sizeof(void*),_tmp4B6),((((_tmp4B9=_tmp4B6,_tmp4BB(& env,& _tmp4BA,& _tmp4B9,& _tmp4B7,& _tmp1AC,& _tmp1A9))),_tmp4B8)))))));
# 749
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1A9,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A9.curr)[i]){
change=1;break;}}}
# 754
if(!change){
if(!_tmp1A8.is_union)return oldval;
new_d=_tmp1A9;}else{
# 759
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1A9,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1AC,sizeof(void*),i))){
change=1;break;}}}
# 764
if(!change){
if(!_tmp1A8.is_union)return newval;
new_d=_tmp1AC;}}{
# 769
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4BE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4BD;return(void*)((_tmp4BD=_region_malloc((env->fenv)->r,sizeof(*_tmp4BD)),((_tmp4BD[0]=((_tmp4BE.tag=6,((_tmp4BE.f1=_tmp1AB,((_tmp4BE.f2=new_d,_tmp4BE)))))),_tmp4BD))));};}_LLBE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1AD=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp19D.f2;if(_tmp1AD->tag != 4)goto _LLC0;else{_tmp1AE=_tmp1AD->f1;}}_LLBF:
# 771
 switch(_tmp1AE){case Cyc_CfFlowInfo_NoneIL: _LLC2:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLC3:
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLC4:
 return(env->fenv)->unknown_all;}_LLC0:;_LLC1:
# 776
 return newval;_LLB3:;}
# 782
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 786
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 791
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp4BF;struct _tuple18 _stmttmp13=(_tmp4BF.f1=fs,((_tmp4BF.f2=oldval,_tmp4BF)));struct _tuple18 _tmp1BB=_stmttmp13;struct Cyc_Absyn_Vardecl*_tmp1BD;void*_tmp1BE;int _tmp1BF;struct Cyc_List_List*_tmp1C0;int _tmp1C2;int _tmp1C3;struct _dyneither_ptr _tmp1C4;_LLC7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1BC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1BB.f2;if(_tmp1BC->tag != 8)goto _LLC9;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=(void*)_tmp1BC->f2;}}_LLC8: {
# 794
void*_tmp1C5=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp1BE,newval);
if(_tmp1C5 == _tmp1BE)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4C2;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4C1;return(void*)((_tmp4C1=_region_malloc((env->fenv)->r,sizeof(*_tmp4C1)),((_tmp4C1[0]=((_tmp4C2.tag=8,((_tmp4C2.f1=_tmp1BD,((_tmp4C2.f2=_tmp1C5,_tmp4C2)))))),_tmp4C1))));}}_LLC9: if(_tmp1BB.f1 == 0)goto _LLCB;_tmp1BF=(int)(_tmp1BB.f1)->hd;_tmp1C0=(_tmp1BB.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1C1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1BB.f2;if(_tmp1C1->tag != 6)goto _LLCB;else{_tmp1C2=(_tmp1C1->f1).is_union;_tmp1C3=(_tmp1C1->f1).fieldnum;_tmp1C4=_tmp1C1->f2;}};_LLCA: {
# 798
void*_tmp1C8=Cyc_CfFlowInfo_assign_place_outer(env,_tmp1C0,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1C4,sizeof(void*),_tmp1BF)),newval);
# 800
struct _dyneither_ptr _tmp1C9=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C4,_tmp1BF,_tmp1C8);
if((void**)_tmp1C9.curr == (void**)_tmp1C4.curr  && (!_tmp1C2  || _tmp1C3 == _tmp1BF))return oldval;
_tmp1C4=_tmp1C9;
# 805
if(_tmp1C2){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp1C4,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp1BF){
struct _dyneither_ptr _tmp1CA=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1C4,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp1C4,sizeof(void*),i))));
# 815
if((void**)_tmp1CA.curr != (void**)_tmp1C4.curr){
_tmp1C4=_tmp1CA;
changed=1;}}}}
# 823
if(changed){
struct Cyc_CfFlowInfo_Place*_stmttmp14=env->root_place;void*_tmp1CC;struct Cyc_List_List*_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp1CB=_stmttmp14;_tmp1CC=_tmp1CB->root;_tmp1CD=_tmp1CB->fields;{
int*_tmp4C3;struct Cyc_List_List*_tmp1CE=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp4C3=_cycalloc_atomic(sizeof(*_tmp4C3)),((_tmp4C3[0]=fs_prefix,_tmp4C3)))),_tmp1CD);
struct Cyc_CfFlowInfo_Place*_tmp4C4;struct Cyc_CfFlowInfo_Place*_tmp1CF=(_tmp4C4=_region_malloc((env->pile)->rgn,sizeof(*_tmp4C4)),((_tmp4C4->root=_tmp1CC,((_tmp4C4->fields=_tmp1CE,_tmp4C4)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1CF,0);};}}{
# 830
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4CA;struct Cyc_CfFlowInfo_UnionRInfo _tmp4C9;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C8;return(void*)((_tmp4C8=_region_malloc((env->fenv)->r,sizeof(*_tmp4C8)),((_tmp4C8[0]=((_tmp4CA.tag=6,((_tmp4CA.f1=((_tmp4C9.is_union=_tmp1C2,((_tmp4C9.fieldnum=_tmp1BF,_tmp4C9)))),((_tmp4CA.f2=_tmp1C4,_tmp4CA)))))),_tmp4C8))));};}_LLCB:;_LLCC: {
struct Cyc_Core_Impossible_exn_struct _tmp4D0;const char*_tmp4CF;struct Cyc_Core_Impossible_exn_struct*_tmp4CE;(int)_throw((void*)((_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE[0]=((_tmp4D0.tag=Cyc_Core_Impossible,((_tmp4D0.f1=((_tmp4CF="bad assign place",_tag_dyneither(_tmp4CF,sizeof(char),17))),_tmp4D0)))),_tmp4CE)))));}_LLC6:;};}
# 834
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 845
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1D9=fenv->r;
void*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_CfFlowInfo_Place*_tmp1DA=place;_tmp1DB=_tmp1DA->root;_tmp1DC=_tmp1DA->fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp4D3;struct Cyc_CfFlowInfo_AssignEnv _tmp4D2;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp4D2.fenv=fenv,((_tmp4D2.pile=((_tmp4D3=_region_malloc(_tmp1D9,sizeof(*_tmp4D3)),((_tmp4D3->rgn=_tmp1D9,((_tmp4D3->places=0,_tmp4D3)))))),((_tmp4D2.d=d,((_tmp4D2.loc=loc,((_tmp4D2.root_place=place,((_tmp4D2.all_changed=all_changed,_tmp4D2)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1DC,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1DB),r);
# 853
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1DB,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 864
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 874
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 876
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp4E6(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp4E5,unsigned int*_tmp4E4,void***_tmp4E2,struct _dyneither_ptr*_tmp21C,struct _dyneither_ptr*_tmp218){for(*_tmp4E5=0;*_tmp4E5 < *_tmp4E4;(*_tmp4E5)++){(*_tmp4E2)[*_tmp4E5]=(void*)
# 950 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp218).curr)[(int)*_tmp4E5],*((void**)_check_dyneither_subscript(*_tmp21C,sizeof(void*),(int)*_tmp4E5)));}}
# 886
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1DF=(env->fenv)->r;
# 890
{struct _tuple0 _tmp4D4;struct _tuple0 _stmttmp15=(_tmp4D4.f1=r1,((_tmp4D4.f2=r2,_tmp4D4)));struct _tuple0 _tmp1E0=_stmttmp15;struct Cyc_Absyn_Vardecl*_tmp1E2;void*_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Vardecl*_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;int _tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F2;int _tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;int _tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1FA;int _tmp1FB;void*_tmp1FC;struct Cyc_CfFlowInfo_Place*_tmp1FE;struct Cyc_CfFlowInfo_Place*_tmp200;struct Cyc_CfFlowInfo_Place*_tmp202;struct Cyc_CfFlowInfo_Place*_tmp206;struct Cyc_CfFlowInfo_Place*_tmp208;struct Cyc_CfFlowInfo_Place*_tmp20C;struct Cyc_CfFlowInfo_Place*_tmp20E;struct Cyc_CfFlowInfo_Place*_tmp210;int _tmp216;int _tmp217;struct _dyneither_ptr _tmp218;int _tmp21A;int _tmp21B;struct _dyneither_ptr _tmp21C;_LLCE:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp1E1->tag != 8)goto _LLD0;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=(void*)_tmp1E1->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp1E4->tag != 8)goto _LLD0;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=(void*)_tmp1E4->f2;}};_LLCF:
# 893
 if(_tmp1E2 == _tmp1E5){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4D7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D6;return(void*)((_tmp4D6=_region_malloc(_tmp1DF,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D7.tag=8,((_tmp4D7.f1=_tmp1E2,((_tmp4D7.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E3,_tmp1E6),_tmp4D7)))))),_tmp4D6))));}else{
# 896
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E3,_tmp1E6);}_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp1E7->tag != 8)goto _LLD2;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=(void*)_tmp1E7->f2;}}_LLD1:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E9,r2);_LLD2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1EA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp1EA->tag != 8)goto _LLD4;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=(void*)_tmp1EA->f2;}}_LLD3:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EC);_LLD4:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1ED=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp1ED->tag != 7)goto _LLD6;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;_tmp1F0=(void*)_tmp1ED->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F1=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp1F1->tag != 7)goto _LLD6;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;_tmp1F4=(void*)_tmp1F1->f3;}};_LLD5:
# 901
 if(_tmp1EE == _tmp1F2  && _tmp1EF == _tmp1F3){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4DA;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D9;return(void*)((_tmp4D9=_region_malloc(_tmp1DF,sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DA.tag=7,((_tmp4DA.f1=_tmp1EE,((_tmp4DA.f2=_tmp1EF,((_tmp4DA.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F0,_tmp1F4),_tmp4DA)))))))),_tmp4D9))));}
{void*_tmp221=_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp223;_LLF1: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp222=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp221;if(_tmp222->tag != 5)goto _LLF3;else{_tmp223=_tmp222->f1;}}_LLF2:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp223);goto _LLF0;_LLF3:;_LLF4:
 goto _LLF0;_LLF0:;}
# 907
{void*_tmp224=_tmp1F4;struct Cyc_CfFlowInfo_Place*_tmp226;_LLF6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp225=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp224;if(_tmp225->tag != 5)goto _LLF8;else{_tmp226=_tmp225->f1;}}_LLF7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp226);goto _LLF5;_LLF8:;_LLF9:
 goto _LLF5;_LLF5:;}
# 911
goto _LLCD;_LLD6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F5=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp1F5->tag != 7)goto _LLD8;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;_tmp1F8=(void*)_tmp1F5->f3;}}_LLD7: {
# 913
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4DD;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4DC;return(void*)((_tmp4DC=_region_malloc(_tmp1DF,sizeof(*_tmp4DC)),((_tmp4DC[0]=((_tmp4DD.tag=7,((_tmp4DD.f1=_tmp1F6,((_tmp4DD.f2=_tmp1F7,((_tmp4DD.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F8,r2),_tmp4DD)))))))),_tmp4DC))));}_LLD8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp1F9->tag != 7)goto _LLDA;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;_tmp1FC=(void*)_tmp1F9->f3;}}_LLD9: {
# 915
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4E0;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4DF;return(void*)((_tmp4DF=_region_malloc(_tmp1DF,sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E0.tag=7,((_tmp4E0.f1=_tmp1FA,((_tmp4E0.f2=_tmp1FB,((_tmp4E0.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1FC),_tmp4E0)))))))),_tmp4DF))));}_LLDA:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp1FD->tag != 5)goto _LLDC;else{_tmp1FE=_tmp1FD->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp1FF->tag != 5)goto _LLDC;else{_tmp200=_tmp1FF->f1;}};_LLDB:
# 918
 if(Cyc_CfFlowInfo_place_cmp(_tmp1FE,_tmp200)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1FE);
Cyc_CfFlowInfo_add_place(env->pile,_tmp200);
goto _LLCD;_LLDC:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp201=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp201->tag != 5)goto _LLDE;else{_tmp202=_tmp201->f1;}}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp203=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp203->tag != 2)goto _LLDE;};_LLDD:
# 923
 _tmp206=_tmp202;goto _LLDF;_LLDE:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp204=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp204->tag != 2)goto _LLE0;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp205=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp205->tag != 5)goto _LLE0;else{_tmp206=_tmp205->f1;}};_LLDF:
# 925
 Cyc_CfFlowInfo_add_place(env->pile,_tmp206);
return(env->fenv)->notzerothis;_LLE0:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp207=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp207->tag != 5)goto _LLE2;else{_tmp208=_tmp207->f1;}}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp209=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp209->tag != 1)goto _LLE2;};_LLE1:
# 929
 Cyc_CfFlowInfo_add_place(env->pile,_tmp208);{
enum Cyc_CfFlowInfo_InitLevel _stmttmp17=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);switch(_stmttmp17){case Cyc_CfFlowInfo_AllIL: _LLFA:
 return(env->fenv)->notzeroall;default: _LLFB:
 return(env->fenv)->notzerothis;}};_LLE2:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp20A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp20A->tag != 1)goto _LLE4;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp20B->tag != 5)goto _LLE4;else{_tmp20C=_tmp20B->f1;}};_LLE3:
# 935
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20C);{
enum Cyc_CfFlowInfo_InitLevel _stmttmp16=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);switch(_stmttmp16){case Cyc_CfFlowInfo_AllIL: _LLFD:
 return(env->fenv)->notzeroall;default: _LLFE:
 return(env->fenv)->notzerothis;}};_LLE4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp20D->tag != 5)goto _LLE6;else{_tmp20E=_tmp20D->f1;}}_LLE5:
# 941
 Cyc_CfFlowInfo_add_place(env->pile,_tmp20E);goto _LLCD;_LLE6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp20F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp20F->tag != 5)goto _LLE8;else{_tmp210=_tmp20F->f1;}}_LLE7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp210);goto _LLCD;_LLE8:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp211=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp211->tag != 1)goto _LLEA;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp212=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp212->tag != 2)goto _LLEA;};_LLE9:
# 945
 goto _LLEB;_LLEA:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp213=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp213->tag != 2)goto _LLEC;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp214=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp214->tag != 1)goto _LLEC;};_LLEB:
 return(env->fenv)->notzerothis;_LLEC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp215=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f1;if(_tmp215->tag != 6)goto _LLEE;else{_tmp216=(_tmp215->f1).is_union;_tmp217=(_tmp215->f1).fieldnum;_tmp218=_tmp215->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp219=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1E0.f2;if(_tmp219->tag != 6)goto _LLEE;else{_tmp21A=(_tmp219->f1).is_union;_tmp21B=(_tmp219->f1).fieldnum;_tmp21C=_tmp219->f2;}};_LLED: {
# 949
unsigned int _tmp4E5;unsigned int _tmp4E4;struct _dyneither_ptr _tmp4E3;void**_tmp4E2;unsigned int _tmp4E1;struct _dyneither_ptr new_d=
(_tmp4E1=_get_dyneither_size(_tmp218,sizeof(void*)),((_tmp4E2=(void**)_region_malloc(_tmp1DF,_check_times(sizeof(void*),_tmp4E1)),((_tmp4E3=_tag_dyneither(_tmp4E2,sizeof(void*),_tmp4E1),((((_tmp4E4=_tmp4E1,_tmp4E6(& env,& _tmp4E5,& _tmp4E4,& _tmp4E2,& _tmp21C,& _tmp218))),_tmp4E3)))))));
# 954
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp218,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp218.curr)[i]){
change=1;break;}}}
# 959
if(!change){
if(!_tmp216)return r1;
new_d=_tmp218;}else{
# 964
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp218,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp21C,sizeof(void*),i))){
change=1;break;}}}
# 969
if(!change){
if(!_tmp216)return r2;
new_d=_tmp21C;}}{
# 974
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4EC;struct Cyc_CfFlowInfo_UnionRInfo _tmp4EB;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4EA;return(void*)((_tmp4EA=_region_malloc(_tmp1DF,sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EC.tag=6,((_tmp4EC.f1=((_tmp4EB.is_union=_tmp216,((_tmp4EB.fieldnum=_tmp217 == _tmp21B?_tmp217: - 1,_tmp4EB)))),((_tmp4EC.f2=new_d,_tmp4EC)))))),_tmp4EA))));};}_LLEE:;_LLEF:
# 976
 goto _LLCD;_LLCD:;}{
# 978
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp4ED;struct _tuple0 _stmttmp18=(_tmp4ED.f1=r1,((_tmp4ED.f2=r2,_tmp4ED)));struct _tuple0 _tmp233=_stmttmp18;_LL101: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp234=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp233.f1;if(_tmp234->tag != 4)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp235=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp233.f2;if(_tmp235->tag != 4)goto _LL105;}_LL104: {
# 983
struct _tuple20 _tmp4EE;struct _tuple20 _stmttmp1A=(_tmp4EE.f1=il1,((_tmp4EE.f2=il2,_tmp4EE)));struct _tuple20 _tmp236=_stmttmp1A;_LL108: if(_tmp236.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL10A;_LL109:
 goto _LL10B;_LL10A: if(_tmp236.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL10C;_LL10B: return(env->fenv)->esc_none;_LL10C: if(_tmp236.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL10E;_LL10D:
 goto _LL10F;_LL10E: if(_tmp236.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL110;_LL10F: return(env->fenv)->esc_this;_LL110:;_LL111:
 return(env->fenv)->esc_all;_LL107:;}_LL105:;_LL106: {
# 989
struct _tuple20 _tmp4EF;struct _tuple20 _stmttmp19=(_tmp4EF.f1=il1,((_tmp4EF.f2=il2,_tmp4EF)));struct _tuple20 _tmp238=_stmttmp19;_LL113: if(_tmp238.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL115;_LL114:
 goto _LL116;_LL115: if(_tmp238.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL117;_LL116: return(env->fenv)->unknown_none;_LL117: if(_tmp238.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL119;_LL118:
 goto _LL11A;_LL119: if(_tmp238.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL11B;_LL11A: return(env->fenv)->unknown_this;_LL11B:;_LL11C:
 return(env->fenv)->unknown_all;_LL112:;}_LL100:;};};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 997
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1000
struct _tuple21 _tmp4F0;struct _tuple21 _stmttmp1B=(_tmp4F0.f1=f1,((_tmp4F0.f2=f2,_tmp4F0)));struct _tuple21 _tmp23B=_stmttmp1B;struct Cyc_Dict_Dict _tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23F;_LL11E: if(((_tmp23B.f1).BottomFL).tag != 1)goto _LL120;_LL11F:
 return f2;_LL120: if(((_tmp23B.f2).BottomFL).tag != 1)goto _LL122;_LL121:
 return f1;_LL122: if(((_tmp23B.f1).ReachableFL).tag != 2)goto _LL11D;_tmp23C=((struct _tuple11)((_tmp23B.f1).ReachableFL).val).f1;_tmp23D=((struct _tuple11)((_tmp23B.f1).ReachableFL).val).f2;if(((_tmp23B.f2).ReachableFL).tag != 2)goto _LL11D;_tmp23E=((struct _tuple11)((_tmp23B.f2).ReachableFL).val).f1;_tmp23F=((struct _tuple11)((_tmp23B.f2).ReachableFL).val).f2;_LL123:
# 1006
 if(_tmp23C.t == _tmp23E.t  && _tmp23D == _tmp23F)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp240=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp4F3;struct Cyc_CfFlowInfo_JoinEnv _tmp4F2;struct Cyc_CfFlowInfo_JoinEnv _tmp241=(_tmp4F2.fenv=fenv,((_tmp4F2.pile=((_tmp4F3=_region_malloc(_tmp240,sizeof(*_tmp4F3)),((_tmp4F3->rgn=_tmp240,((_tmp4F3->places=0,_tmp4F3)))))),((_tmp4F2.d1=_tmp23C,((_tmp4F2.d2=_tmp23E,_tmp4F2)))))));
struct Cyc_Dict_Dict _tmp242=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp241,_tmp23C,_tmp23E);
struct Cyc_List_List*_tmp243=Cyc_Relations_join_relns(fenv->r,_tmp23D,_tmp23F);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp241.pile,all_changed,_tmp242),_tmp243);};_LL11D:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1018
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){
# 1025
union Cyc_CfFlowInfo_FlowInfo _tmp248;void*_tmp249;struct _tuple12 _tmp247=pr1;_tmp248=_tmp247.f1;_tmp249=_tmp247.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp24B;void*_tmp24C;struct _tuple12 _tmp24A=pr2;_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp248,_tmp24B);
struct _tuple22 _tmp4F4;struct _tuple22 _stmttmp1C=(_tmp4F4.f1=_tmp248,((_tmp4F4.f2=_tmp24B,((_tmp4F4.f3=outflow,_tmp4F4)))));struct _tuple22 _tmp24D=_stmttmp1C;struct Cyc_Dict_Dict _tmp24E;struct Cyc_Dict_Dict _tmp24F;struct Cyc_Dict_Dict _tmp250;struct Cyc_List_List*_tmp251;_LL125: if(((_tmp24D.f1).BottomFL).tag != 1)goto _LL127;_LL126: {
struct _tuple12 _tmp4F5;return(_tmp4F5.f1=outflow,((_tmp4F5.f2=_tmp24C,_tmp4F5)));}_LL127: if(((_tmp24D.f2).BottomFL).tag != 1)goto _LL129;_LL128: {
struct _tuple12 _tmp4F6;return(_tmp4F6.f1=outflow,((_tmp4F6.f2=_tmp249,_tmp4F6)));}_LL129: if(((_tmp24D.f1).ReachableFL).tag != 2)goto _LL12B;_tmp24E=((struct _tuple11)((_tmp24D.f1).ReachableFL).val).f1;if(((_tmp24D.f2).ReachableFL).tag != 2)goto _LL12B;_tmp24F=((struct _tuple11)((_tmp24D.f2).ReachableFL).val).f1;if(((_tmp24D.f3).ReachableFL).tag != 2)goto _LL12B;_tmp250=((struct _tuple11)((_tmp24D.f3).ReachableFL).val).f1;_tmp251=((struct _tuple11)((_tmp24D.f3).ReachableFL).val).f2;_LL12A:
# 1033
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp249,_tmp24C)){struct _tuple12 _tmp4F7;return(_tmp4F7.f1=outflow,((_tmp4F7.f2=_tmp24C,_tmp4F7)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp24C,_tmp249)){struct _tuple12 _tmp4F8;return(_tmp4F8.f1=outflow,((_tmp4F8.f2=_tmp249,_tmp4F8)));}{
struct _RegionHandle*_tmp256=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp4FB;struct Cyc_CfFlowInfo_JoinEnv _tmp4FA;struct Cyc_CfFlowInfo_JoinEnv _tmp257=(_tmp4FA.fenv=fenv,((_tmp4FA.pile=((_tmp4FB=_region_malloc(_tmp256,sizeof(*_tmp4FB)),((_tmp4FB->rgn=_tmp256,((_tmp4FB->places=0,_tmp4FB)))))),((_tmp4FA.d1=_tmp24E,((_tmp4FA.d2=_tmp24F,_tmp4FA)))))));
void*_tmp258=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp257,0,_tmp249,_tmp24C);
struct _tuple12 _tmp4FC;return(_tmp4FC.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp257.pile,all_changed,_tmp250),_tmp251),((_tmp4FC.f2=_tmp258,_tmp4FC)));};_LL12B:;_LL12C: {
# 1041
struct Cyc_Core_Impossible_exn_struct _tmp502;const char*_tmp501;struct Cyc_Core_Impossible_exn_struct*_tmp500;(int)_throw((void*)((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((_tmp502.tag=Cyc_Core_Impossible,((_tmp502.f1=((_tmp501="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp501,sizeof(char),37))),_tmp502)))),_tmp500)))));}_LL124:;};};}
# 1045
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp50C(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp50B,unsigned int*_tmp50A,void***_tmp508,struct _dyneither_ptr*_tmp272,struct _dyneither_ptr*_tmp26E){for(*_tmp50B=0;*_tmp50B < *_tmp50A;(*_tmp50B)++){(*_tmp508)[*_tmp50B]=(void*)
# 1086
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp50B,((void**)(*_tmp26E).curr)[(int)*_tmp50B],*((void**)_check_dyneither_subscript(*_tmp272,sizeof(void*),(int)*_tmp50B)));}}
# 1046
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1054
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1059
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1067
struct _tuple0 _tmp503;struct _tuple0 _stmttmp1D=(_tmp503.f1=r1,((_tmp503.f2=r2,_tmp503)));struct _tuple0 _tmp260=_stmttmp1D;struct Cyc_Absyn_Vardecl*_tmp264;void*_tmp265;struct Cyc_Absyn_Vardecl*_tmp267;void*_tmp268;int _tmp26C;int _tmp26D;struct _dyneither_ptr _tmp26E;int _tmp270;int _tmp271;struct _dyneither_ptr _tmp272;_LL12E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp261=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp260.f1;if(_tmp261->tag != 7)goto _LL130;}_LL12F:
# 1069
 goto _LL131;_LL130: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp262=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp260.f2;if(_tmp262->tag != 7)goto _LL132;}_LL131:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL132:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp263=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp260.f1;if(_tmp263->tag != 8)goto _LL134;else{_tmp264=_tmp263->f1;_tmp265=(void*)_tmp263->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp266=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp260.f2;if(_tmp266->tag != 8)goto _LL134;else{_tmp267=_tmp266->f1;_tmp268=(void*)_tmp266->f2;}};_LL133:
# 1072
 if(_tmp264 == _tmp267){
void*_tmp273=Cyc_CfFlowInfo_after_absRval(env,_tmp265,_tmp268);
if(_tmp273 == _tmp265)return r1;
if(_tmp273 == _tmp268)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp506;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp505;return(void*)((_tmp505=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp505)),((_tmp505[0]=((_tmp506.tag=8,((_tmp506.f1=_tmp264,((_tmp506.f2=_tmp273,_tmp506)))))),_tmp505))));};}else{
# 1078
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}_LL134: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp269=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp260.f2;if(_tmp269->tag != 8)goto _LL136;}_LL135:
 goto _LL137;_LL136: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp26A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp260.f1;if(_tmp26A->tag != 8)goto _LL138;}_LL137:
# 1081
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL138:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp260.f1;if(_tmp26B->tag != 6)goto _LL13A;else{_tmp26C=(_tmp26B->f1).is_union;_tmp26D=(_tmp26B->f1).fieldnum;_tmp26E=_tmp26B->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp260.f2;if(_tmp26F->tag != 6)goto _LL13A;else{_tmp270=(_tmp26F->f1).is_union;_tmp271=(_tmp26F->f1).fieldnum;_tmp272=_tmp26F->f2;}};_LL139: {
# 1084
unsigned int _tmp50B;unsigned int _tmp50A;struct _dyneither_ptr _tmp509;void**_tmp508;unsigned int _tmp507;struct _dyneither_ptr new_d=
(_tmp507=
_get_dyneither_size(_tmp26E,sizeof(void*)),((_tmp508=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp507)),((_tmp509=_tag_dyneither(_tmp508,sizeof(void*),_tmp507),((((_tmp50A=_tmp507,_tmp50C(& env,& _tmp50B,& _tmp50A,& _tmp508,& _tmp272,& _tmp26E))),_tmp509)))))));
# 1091
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp26E,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp26E.curr)[i]){
change=1;break;}}}
# 1096
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp26E,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp272,sizeof(void*),i))){
change=1;break;}}}
# 1103
if(!change)
return r2;{
# 1107
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp512;struct Cyc_CfFlowInfo_UnionRInfo _tmp511;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp510;return(void*)((_tmp510=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp510)),((_tmp510[0]=((_tmp512.tag=6,((_tmp512.f1=((_tmp511.is_union=_tmp26C,((_tmp511.fieldnum=_tmp26D == _tmp271?_tmp26D: - 1,_tmp511)))),((_tmp512.f2=new_d,_tmp512)))))),_tmp510))));};}_LL13A:;_LL13B: {
# 1115
struct Cyc_Core_Impossible_exn_struct _tmp518;const char*_tmp517;struct Cyc_Core_Impossible_exn_struct*_tmp516;(int)_throw((void*)((_tmp516=_cycalloc(sizeof(*_tmp516)),((_tmp516[0]=((_tmp518.tag=Cyc_Core_Impossible,((_tmp518.f1=((_tmp517="after_absRval -- non-aggregates!",_tag_dyneither(_tmp517,sizeof(char),33))),_tmp518)))),_tmp516)))));}_LL12D:;};}
# 1118
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1121
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp281=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp282=env->changed;
{struct Cyc_List_List*_tmp519;*env->last_field_cell=((_tmp519=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp519)),((_tmp519->hd=(void*)key,((_tmp519->tl=0,_tmp519))))));}
# 1127
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp284=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp281;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp282;
return _tmp284;};};}
# 1134
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1136
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp51A;*env->curr_place=((_tmp51A.root=root,((_tmp51A.fields=0,_tmp51A))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1143
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1149
struct _tuple21 _tmp51B;struct _tuple21 _stmttmp1E=(_tmp51B.f1=f1,((_tmp51B.f2=f2,_tmp51B)));struct _tuple21 _tmp286=_stmttmp1E;struct Cyc_Dict_Dict _tmp287;struct Cyc_List_List*_tmp288;struct Cyc_Dict_Dict _tmp289;struct Cyc_List_List*_tmp28A;_LL13D: if(((_tmp286.f1).BottomFL).tag != 1)goto _LL13F;_LL13E:
 goto _LL140;_LL13F: if(((_tmp286.f2).BottomFL).tag != 1)goto _LL141;_LL140:
 return Cyc_CfFlowInfo_BottomFL();_LL141: if(((_tmp286.f1).ReachableFL).tag != 2)goto _LL13C;_tmp287=((struct _tuple11)((_tmp286.f1).ReachableFL).val).f1;_tmp288=((struct _tuple11)((_tmp286.f1).ReachableFL).val).f2;if(((_tmp286.f2).ReachableFL).tag != 2)goto _LL13C;_tmp289=((struct _tuple11)((_tmp286.f2).ReachableFL).val).f1;_tmp28A=((struct _tuple11)((_tmp286.f2).ReachableFL).val).f2;_LL142:
# 1153
 if(_tmp287.t == _tmp289.t  && _tmp288 == _tmp28A)return f1;{
# 1156
struct _RegionHandle*_tmp28B=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp28C=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp51F;struct Cyc_CfFlowInfo_AfterEnv _tmp51E;struct Cyc_CfFlowInfo_AfterEnv _tmp28D=(_tmp51E.joinenv=(((_tmp51E.joinenv).fenv=fenv,(((_tmp51E.joinenv).pile=((_tmp51F=_region_malloc(_tmp28B,sizeof(*_tmp51F)),((_tmp51F->rgn=_tmp28B,((_tmp51F->places=0,_tmp51F)))))),(((_tmp51E.joinenv).d1=_tmp287,(((_tmp51E.joinenv).d2=_tmp289,_tmp51E.joinenv)))))))),((_tmp51E.chg1=chg1,((_tmp51E.chg2=chg2,((_tmp51E.curr_place=_tmp28C,((_tmp51E.last_field_cell=& _tmp28C->fields,((_tmp51E.changed=Cyc_CfFlowInfo_Neither,_tmp51E)))))))))));
# 1162
struct Cyc_Dict_Dict _tmp28E=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp28D,_tmp287,_tmp289);
struct Cyc_List_List*_tmp28F=Cyc_Relations_join_relns(fenv->r,_tmp288,_tmp28A);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp28D.joinenv).pile,all_changed,_tmp28E),_tmp28F);};_LL13C:;}
# 1171
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1175
{struct _tuple0 _tmp520;struct _tuple0 _stmttmp1F=(_tmp520.f1=r1,((_tmp520.f2=r2,_tmp520)));struct _tuple0 _tmp293=_stmttmp1F;struct Cyc_Absyn_Vardecl*_tmp295;void*_tmp296;struct Cyc_Absyn_Vardecl*_tmp298;void*_tmp299;struct Cyc_CfFlowInfo_Place*_tmp29D;struct Cyc_CfFlowInfo_Place*_tmp29F;struct Cyc_CfFlowInfo_UnionRInfo _tmp2A3;struct _dyneither_ptr _tmp2A4;struct Cyc_CfFlowInfo_UnionRInfo _tmp2A6;struct _dyneither_ptr _tmp2A7;_LL144:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp294=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp294->tag != 8)goto _LL146;else{_tmp295=_tmp294->f1;_tmp296=(void*)_tmp294->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp297=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp297->tag != 8)goto _LL146;else{_tmp298=_tmp297->f1;_tmp299=(void*)_tmp297->f2;}};_LL145:
# 1177
 return _tmp295 == _tmp298  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp296,_tmp299);_LL146: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp29A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp29A->tag != 8)goto _LL148;}_LL147:
 goto _LL149;_LL148: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp29B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp29B->tag != 8)goto _LL14A;}_LL149:
 return 0;_LL14A:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp29C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp29C->tag != 5)goto _LL14C;else{_tmp29D=_tmp29C->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp29E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp29E->tag != 5)goto _LL14C;else{_tmp29F=_tmp29E->f1;}};_LL14B:
 return Cyc_CfFlowInfo_place_cmp(_tmp29D,_tmp29F)== 0;_LL14C: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2A0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp2A0->tag != 5)goto _LL14E;}_LL14D:
 goto _LL14F;_LL14E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2A1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2A1->tag != 5)goto _LL150;}_LL14F:
 return 0;_LL150:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp2A2->tag != 6)goto _LL152;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2A5->tag != 6)goto _LL152;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;}};_LL151:
# 1184
 if((void**)_tmp2A4.curr == (void**)_tmp2A7.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp2A4,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2A4.curr)[i],*((void**)_check_dyneither_subscript(_tmp2A7,sizeof(void*),i))))return 0;}}
return 1;_LL152:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2A8=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp2A8->tag != 1)goto _LL154;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2A9=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2A9->tag != 2)goto _LL154;};_LL153:
 return 1;_LL154: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2AA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2AA->tag != 2)goto _LL156;}_LL155:
 return 0;_LL156: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp2AB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2AB->tag != 0)goto _LL158;}_LL157:
 goto _LL159;_LL158: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2AC=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2AC->tag != 1)goto _LL15A;}_LL159:
 return 0;_LL15A:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2AD=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp2AD->tag != 4)goto _LL15C;}{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2AE=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp293.f2;if(_tmp2AE->tag != 4)goto _LL15C;};_LL15B:
 goto _LL143;_LL15C: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2AF=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp293.f1;if(_tmp2AF->tag != 4)goto _LL15E;}_LL15D:
 return 0;_LL15E:;_LL15F:
 goto _LL143;_LL143:;}{
# 1196
struct _tuple20 _tmp521;struct _tuple20 _stmttmp20=(_tmp521.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp521.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp521)));struct _tuple20 _tmp2B1=_stmttmp20;_LL161: if(_tmp2B1.f1 != Cyc_CfFlowInfo_AllIL)goto _LL163;if(_tmp2B1.f2 != Cyc_CfFlowInfo_AllIL)goto _LL163;_LL162:
 return 1;_LL163: if(_tmp2B1.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL165;_LL164:
 return 1;_LL165: if(_tmp2B1.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL167;_LL166:
 return 0;_LL167: if(_tmp2B1.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL169;_LL168:
 return 1;_LL169: if(_tmp2B1.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL160;_LL16A:
 return 0;_LL160:;};}
# 1211
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1213
struct _tuple21 _tmp522;struct _tuple21 _stmttmp21=(_tmp522.f1=f1,((_tmp522.f2=f2,_tmp522)));struct _tuple21 _tmp2B3=_stmttmp21;struct Cyc_Dict_Dict _tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;struct Cyc_List_List*_tmp2B7;_LL16C: if(((_tmp2B3.f1).BottomFL).tag != 1)goto _LL16E;_LL16D:
 return 1;_LL16E: if(((_tmp2B3.f2).BottomFL).tag != 1)goto _LL170;_LL16F:
 return 0;_LL170: if(((_tmp2B3.f1).ReachableFL).tag != 2)goto _LL16B;_tmp2B4=((struct _tuple11)((_tmp2B3.f1).ReachableFL).val).f1;_tmp2B5=((struct _tuple11)((_tmp2B3.f1).ReachableFL).val).f2;if(((_tmp2B3.f2).ReachableFL).tag != 2)goto _LL16B;_tmp2B6=((struct _tuple11)((_tmp2B3.f2).ReachableFL).val).f1;_tmp2B7=((struct _tuple11)((_tmp2B3.f2).ReachableFL).val).f2;_LL171:
# 1217
 if(_tmp2B4.t == _tmp2B6.t  && _tmp2B5 == _tmp2B7)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2B4,_tmp2B6) && 
Cyc_Relations_relns_approx(_tmp2B5,_tmp2B7);_LL16B:;}
# 1223
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL172:
{const char*_tmp525;void*_tmp524;(_tmp524=0,Cyc_fprintf(Cyc_stderr,((_tmp525="uninitialized",_tag_dyneither(_tmp525,sizeof(char),14))),_tag_dyneither(_tmp524,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL173:
{const char*_tmp528;void*_tmp527;(_tmp527=0,Cyc_fprintf(Cyc_stderr,((_tmp528="this-initialized",_tag_dyneither(_tmp528,sizeof(char),17))),_tag_dyneither(_tmp527,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL174:
{const char*_tmp52B;void*_tmp52A;(_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B="all-initialized",_tag_dyneither(_tmp52B,sizeof(char),16))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}break;}}
# 1231
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp2BF=root;struct Cyc_Absyn_Vardecl*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;void*_tmp2C4;_LL177: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2C0=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2BF;if(_tmp2C0->tag != 0)goto _LL179;else{_tmp2C1=_tmp2C0->f1;}}_LL178:
# 1234
{const char*_tmp52F;void*_tmp52E[1];struct Cyc_String_pa_PrintArg_struct _tmp52D;(_tmp52D.tag=0,((_tmp52D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C1->name)),((_tmp52E[0]=& _tmp52D,Cyc_fprintf(Cyc_stderr,((_tmp52F="Root(%s)",_tag_dyneither(_tmp52F,sizeof(char),9))),_tag_dyneither(_tmp52E,sizeof(void*),1)))))));}goto _LL176;_LL179: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2C2=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2BF;if(_tmp2C2->tag != 1)goto _LL17B;else{_tmp2C3=_tmp2C2->f1;_tmp2C4=(void*)_tmp2C2->f2;}}_LL17A:
# 1236
{const char*_tmp534;void*_tmp533[2];struct Cyc_String_pa_PrintArg_struct _tmp532;struct Cyc_String_pa_PrintArg_struct _tmp531;(_tmp531.tag=0,((_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2C4)),((_tmp532.tag=0,((_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2C3)),((_tmp533[0]=& _tmp532,((_tmp533[1]=& _tmp531,Cyc_fprintf(Cyc_stderr,((_tmp534="MallocPt(%s,%s)",_tag_dyneither(_tmp534,sizeof(char),16))),_tag_dyneither(_tmp533,sizeof(void*),2)))))))))))));}goto _LL176;_LL17B: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp2C5=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp2BF;if(_tmp2C5->tag != 2)goto _LL176;}_LL17C:
# 1239
{const char*_tmp537;void*_tmp536;(_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="InitParam(_,_)",_tag_dyneither(_tmp537,sizeof(char),15))),_tag_dyneither(_tmp536,sizeof(void*),0)));}goto _LL176;_LL176:;}
# 1243
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp2CF=p->fields;
while(_tmp2CF != 0){
{const char*_tmp53B;void*_tmp53A[1];struct Cyc_Int_pa_PrintArg_struct _tmp539;(_tmp539.tag=1,((_tmp539.f1=(unsigned long)((int)_tmp2CF->hd),((_tmp53A[0]=& _tmp539,Cyc_fprintf(Cyc_stderr,((_tmp53B=".%d",_tag_dyneither(_tmp53B,sizeof(char),4))),_tag_dyneither(_tmp53A,sizeof(void*),1)))))));}
_tmp2CF=_tmp2CF->tl;}{
# 1250
const char*_tmp53E;void*_tmp53D;(_tmp53D=0,Cyc_fprintf(Cyc_stderr,((_tmp53E=" ",_tag_dyneither(_tmp53E,sizeof(char),2))),_tag_dyneither(_tmp53D,sizeof(void*),0)));};};}
# 1253
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp2D5=1;
while(x != 0){
if(!_tmp2D5){{const char*_tmp541;void*_tmp540;(_tmp540=0,Cyc_fprintf(Cyc_stderr,((_tmp541=", ",_tag_dyneither(_tmp541,sizeof(char),3))),_tag_dyneither(_tmp540,sizeof(void*),0)));}_tmp2D5=0;}
pr(x->hd);
x=x->tl;}{
# 1260
const char*_tmp544;void*_tmp543;(_tmp543=0,Cyc_fprintf(Cyc_stderr,((_tmp544="\n",_tag_dyneither(_tmp544,sizeof(char),2))),_tag_dyneither(_tmp543,sizeof(void*),0)));};}
# 1263
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp2DA=_new_region("r");struct _RegionHandle*r=& _tmp2DA;_push_region(r);
{struct _handler_cons _tmp2DB;_push_handler(& _tmp2DB);{int _tmp2DD=0;if(setjmp(_tmp2DB.handler))_tmp2DD=1;if(!_tmp2DD){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple13 _tmp2DE=*((struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp547;void*_tmp546;(_tmp546=0,Cyc_fprintf(Cyc_stderr,((_tmp547="{ ",_tag_dyneither(_tmp547,sizeof(char),3))),_tag_dyneither(_tmp546,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(iter,& _tmp2DE)){
void*_tmp2E1=_tmp2DE.f1;
if(!first){
{const char*_tmp54A;void*_tmp549;(_tmp549=0,Cyc_fprintf(Cyc_stderr,((_tmp54A=", ",_tag_dyneither(_tmp54A,sizeof(char),3))),_tag_dyneither(_tmp549,sizeof(void*),0)));}
first=0;}
# 1276
pr(_tmp2E1);}{
# 1278
const char*_tmp54D;void*_tmp54C;(_tmp54C=0,Cyc_fprintf(Cyc_stderr,((_tmp54D="}\n",_tag_dyneither(_tmp54D,sizeof(char),3))),_tag_dyneither(_tmp54C,sizeof(void*),0)));};}
# 1266
;_pop_handler();}else{void*_tmp2DC=(void*)_exn_thrown;void*_tmp2E7=_tmp2DC;void*_tmp2E8;void*_tmp2E9;_LL17E: _tmp2E8=_tmp2E7;_LL17F:
# 1281
{const char*_tmp550;void*_tmp54F;(_tmp54F=0,Cyc_fprintf(Cyc_stderr,((_tmp550="{ }\n",_tag_dyneither(_tmp550,sizeof(char),5))),_tag_dyneither(_tmp54F,sizeof(void*),0)));}
_npop_handler(0);return;_LL180: _tmp2E9=_tmp2E7;_LL181:(void)_throw(_tmp2E9);_LL17D:;}};}
# 1265
;_pop_region(r);}
# 1287
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp2EC=rval;enum Cyc_CfFlowInfo_InitLevel _tmp2F1;enum Cyc_CfFlowInfo_InitLevel _tmp2F3;struct Cyc_CfFlowInfo_Place*_tmp2F5;int _tmp2F7;int _tmp2F8;struct _dyneither_ptr _tmp2F9;struct Cyc_Absyn_Exp*_tmp2FB;int _tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FF;void*_tmp300;_LL183: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp2ED=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2ED->tag != 0)goto _LL185;}_LL184:
{const char*_tmp553;void*_tmp552;(_tmp552=0,Cyc_fprintf(Cyc_stderr,((_tmp553="Zero",_tag_dyneither(_tmp553,sizeof(char),5))),_tag_dyneither(_tmp552,sizeof(void*),0)));}goto _LL182;_LL185: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2EE=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2EE->tag != 1)goto _LL187;}_LL186:
{const char*_tmp556;void*_tmp555;(_tmp555=0,Cyc_fprintf(Cyc_stderr,((_tmp556="NotZeroAll",_tag_dyneither(_tmp556,sizeof(char),11))),_tag_dyneither(_tmp555,sizeof(void*),0)));}goto _LL182;_LL187: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2EF=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2EF->tag != 2)goto _LL189;}_LL188:
{const char*_tmp559;void*_tmp558;(_tmp558=0,Cyc_fprintf(Cyc_stderr,((_tmp559="NotZeroThis",_tag_dyneither(_tmp559,sizeof(char),12))),_tag_dyneither(_tmp558,sizeof(void*),0)));}goto _LL182;_LL189: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp2F0=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2F0->tag != 3)goto _LL18B;else{_tmp2F1=_tmp2F0->f1;}}_LL18A:
{const char*_tmp55C;void*_tmp55B;(_tmp55B=0,Cyc_fprintf(Cyc_stderr,((_tmp55C="Unknown(",_tag_dyneither(_tmp55C,sizeof(char),9))),_tag_dyneither(_tmp55B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp2F1);
{const char*_tmp55F;void*_tmp55E;(_tmp55E=0,Cyc_fprintf(Cyc_stderr,((_tmp55F=")",_tag_dyneither(_tmp55F,sizeof(char),2))),_tag_dyneither(_tmp55E,sizeof(void*),0)));}goto _LL182;_LL18B: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2F2=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2F2->tag != 4)goto _LL18D;else{_tmp2F3=_tmp2F2->f1;}}_LL18C:
{const char*_tmp562;void*_tmp561;(_tmp561=0,Cyc_fprintf(Cyc_stderr,((_tmp562="Esc(",_tag_dyneither(_tmp562,sizeof(char),5))),_tag_dyneither(_tmp561,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp2F3);
{const char*_tmp565;void*_tmp564;(_tmp564=0,Cyc_fprintf(Cyc_stderr,((_tmp565=")",_tag_dyneither(_tmp565,sizeof(char),2))),_tag_dyneither(_tmp564,sizeof(void*),0)));}goto _LL182;_LL18D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2F4=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2F4->tag != 5)goto _LL18F;else{_tmp2F5=_tmp2F4->f1;}}_LL18E:
{const char*_tmp568;void*_tmp567;(_tmp567=0,Cyc_fprintf(Cyc_stderr,((_tmp568="AddrOf(",_tag_dyneither(_tmp568,sizeof(char),8))),_tag_dyneither(_tmp567,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp2F5);
{const char*_tmp56B;void*_tmp56A;(_tmp56A=0,Cyc_fprintf(Cyc_stderr,((_tmp56B=")",_tag_dyneither(_tmp56B,sizeof(char),2))),_tag_dyneither(_tmp56A,sizeof(void*),0)));}goto _LL182;_LL18F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2F6->tag != 6)goto _LL191;else{_tmp2F7=(_tmp2F6->f1).is_union;_tmp2F8=(_tmp2F6->f1).fieldnum;_tmp2F9=_tmp2F6->f2;}}_LL190:
# 1299
 if(_tmp2F7){
{const char*_tmp56E;void*_tmp56D;(_tmp56D=0,Cyc_fprintf(Cyc_stderr,((_tmp56E="AggrUnion{",_tag_dyneither(_tmp56E,sizeof(char),11))),_tag_dyneither(_tmp56D,sizeof(void*),0)));}
if(_tmp2F8 != - 1){
const char*_tmp572;void*_tmp571[1];struct Cyc_Int_pa_PrintArg_struct _tmp570;(_tmp570.tag=1,((_tmp570.f1=(unsigned long)_tmp2F8,((_tmp571[0]=& _tmp570,Cyc_fprintf(Cyc_stderr,((_tmp572="tag == %d;",_tag_dyneither(_tmp572,sizeof(char),11))),_tag_dyneither(_tmp571,sizeof(void*),1)))))));}}else{
# 1304
const char*_tmp575;void*_tmp574;(_tmp574=0,Cyc_fprintf(Cyc_stderr,((_tmp575="AggrStruct{",_tag_dyneither(_tmp575,sizeof(char),12))),_tag_dyneither(_tmp574,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp2F9,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp2F9.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp2F9,sizeof(void*))){const char*_tmp578;void*_tmp577;(_tmp577=0,Cyc_fprintf(Cyc_stderr,((_tmp578=",",_tag_dyneither(_tmp578,sizeof(char),2))),_tag_dyneither(_tmp577,sizeof(void*),0)));}}}
# 1309
{const char*_tmp57B;void*_tmp57A;(_tmp57A=0,Cyc_fprintf(Cyc_stderr,((_tmp57B="}",_tag_dyneither(_tmp57B,sizeof(char),2))),_tag_dyneither(_tmp57A,sizeof(void*),0)));}
goto _LL182;_LL191: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp2FA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2FA->tag != 7)goto _LL193;else{_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FA->f2;_tmp2FD=(void*)_tmp2FA->f3;}}_LL192:
# 1312
{const char*_tmp580;void*_tmp57F[2];struct Cyc_String_pa_PrintArg_struct _tmp57E;struct Cyc_Int_pa_PrintArg_struct _tmp57D;(_tmp57D.tag=1,((_tmp57D.f1=(unsigned long)_tmp2FC,((_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2FB)),((_tmp57F[0]=& _tmp57E,((_tmp57F[1]=& _tmp57D,Cyc_fprintf(Cyc_stderr,((_tmp580="[Consumed(%s,%d,",_tag_dyneither(_tmp580,sizeof(char),17))),_tag_dyneither(_tmp57F,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp2FD);{const char*_tmp583;void*_tmp582;(_tmp582=0,Cyc_fprintf(Cyc_stderr,((_tmp583=")]",_tag_dyneither(_tmp583,sizeof(char),3))),_tag_dyneither(_tmp582,sizeof(void*),0)));}
goto _LL182;_LL193: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2FE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2EC;if(_tmp2FE->tag != 8)goto _LL182;else{_tmp2FF=_tmp2FE->f1;_tmp300=(void*)_tmp2FE->f2;}}_LL194:
# 1316
{const char*_tmp587;void*_tmp586[1];struct Cyc_String_pa_PrintArg_struct _tmp585;(_tmp585.tag=0,((_tmp585.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FF->name)),((_tmp586[0]=& _tmp585,Cyc_fprintf(Cyc_stderr,((_tmp587="[NamedLocation(%s,",_tag_dyneither(_tmp587,sizeof(char),19))),_tag_dyneither(_tmp586,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp300);{const char*_tmp58A;void*_tmp589;(_tmp589=0,Cyc_fprintf(Cyc_stderr,((_tmp58A=")]",_tag_dyneither(_tmp58A,sizeof(char),3))),_tag_dyneither(_tmp589,sizeof(void*),0)));}
goto _LL182;_LL182:;}
# 1322
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp58D;void*_tmp58C;(_tmp58C=0,Cyc_fprintf(Cyc_stderr,((_tmp58D="    ",_tag_dyneither(_tmp58D,sizeof(char),5))),_tag_dyneither(_tmp58C,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp590;void*_tmp58F;(_tmp58F=0,Cyc_fprintf(Cyc_stderr,((_tmp590=" --> ",_tag_dyneither(_tmp590,sizeof(char),6))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp593;void*_tmp592;(_tmp592=0,Cyc_fprintf(Cyc_stderr,((_tmp593="\n",_tag_dyneither(_tmp593,sizeof(char),2))),_tag_dyneither(_tmp592,sizeof(void*),0)));};}
# 1330
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1334
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp32F=f;struct Cyc_Dict_Dict _tmp330;struct Cyc_List_List*_tmp331;_LL196: if((_tmp32F.BottomFL).tag != 1)goto _LL198;_LL197:
{const char*_tmp596;void*_tmp595;(_tmp595=0,Cyc_fprintf(Cyc_stderr,((_tmp596="  BottomFL\n",_tag_dyneither(_tmp596,sizeof(char),12))),_tag_dyneither(_tmp595,sizeof(void*),0)));}goto _LL195;_LL198: if((_tmp32F.ReachableFL).tag != 2)goto _LL195;_tmp330=((struct _tuple11)(_tmp32F.ReachableFL).val).f1;_tmp331=((struct _tuple11)(_tmp32F.ReachableFL).val).f2;_LL199:
# 1338
{const char*_tmp599;void*_tmp598;(_tmp598=0,Cyc_fprintf(Cyc_stderr,((_tmp599="  ReachableFL:\n",_tag_dyneither(_tmp599,sizeof(char),16))),_tag_dyneither(_tmp598,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp330);
{const char*_tmp59C;void*_tmp59B;(_tmp59B=0,Cyc_fprintf(Cyc_stderr,((_tmp59C="\n  Relations: ",_tag_dyneither(_tmp59C,sizeof(char),15))),_tag_dyneither(_tmp59B,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp331);
{const char*_tmp59F;void*_tmp59E;(_tmp59E=0,Cyc_fprintf(Cyc_stderr,((_tmp59F="\n",_tag_dyneither(_tmp59F,sizeof(char),2))),_tag_dyneither(_tmp59E,sizeof(void*),0)));}
goto _LL195;_LL195:;}
# 1355 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_stmttmp22=Cyc_Tcutil_compress(t);void*_tmp33A=_stmttmp22;struct Cyc_Absyn_Tvar*_tmp347;struct Cyc_List_List*_tmp349;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34F;struct Cyc_List_List*_tmp351;void*_tmp353;void*_tmp354;struct Cyc_List_List*_tmp357;struct Cyc_List_List*_tmp359;void*_tmp35B;void*_tmp35D;void*_tmp35F;void*_tmp361;void*_tmp362;void*_tmp364;_LL19B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp33A;if(_tmp33B->tag != 26)goto _LL19D;}_LL19C:
 goto _LL19E;_LL19D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp33A;if(_tmp33C->tag != 0)goto _LL19F;}_LL19E:
 goto _LL1A0;_LL19F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp33A;if(_tmp33D->tag != 6)goto _LL1A1;}_LL1A0:
 goto _LL1A2;_LL1A1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp33A;if(_tmp33E->tag != 7)goto _LL1A3;}_LL1A2:
 goto _LL1A4;_LL1A3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp33A;if(_tmp33F->tag != 13)goto _LL1A5;}_LL1A4:
 goto _LL1A6;_LL1A5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp33A;if(_tmp340->tag != 14)goto _LL1A7;}_LL1A6:
 goto _LL1A8;_LL1A7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33A;if(_tmp341->tag != 19)goto _LL1A9;}_LL1A8:
 goto _LL1AA;_LL1A9: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp33A;if(_tmp342->tag != 20)goto _LL1AB;}_LL1AA:
 goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33A;if(_tmp343->tag != 1)goto _LL1AD;}_LL1AC:
 return 0;_LL1AD: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp33A;if(_tmp344->tag != 21)goto _LL1AF;}_LL1AE: {
# 1367
void*_tmp366=rgn;_LL1D2: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp367=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp366;if(_tmp367->tag != 0)goto _LL1D4;}_LL1D3:
 return 1;_LL1D4:;_LL1D5:
 return 0;_LL1D1:;}_LL1AF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp33A;if(_tmp345->tag != 22)goto _LL1B1;}_LL1B0: {
# 1372
void*_tmp368=rgn;_LL1D7: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp369=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp368;if(_tmp369->tag != 1)goto _LL1D9;}_LL1D8:
 return 1;_LL1D9:;_LL1DA:
 return 0;_LL1D6:;}_LL1B1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp33A;if(_tmp346->tag != 2)goto _LL1B3;else{_tmp347=_tmp346->f1;}}_LL1B2: {
# 1377
void*_tmp36A=rgn;struct Cyc_Absyn_Tvar*_tmp36E;_LL1DC: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp36B=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp36A;if(_tmp36B->tag != 1)goto _LL1DE;}_LL1DD:
 goto _LL1DF;_LL1DE: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp36C=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp36A;if(_tmp36C->tag != 0)goto _LL1E0;}_LL1DF:
 return 0;_LL1E0: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp36D=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp36A;if(_tmp36D->tag != 2)goto _LL1DB;else{_tmp36E=_tmp36D->f1;}}_LL1E1:
 return Cyc_Absyn_tvar_cmp(_tmp347,_tmp36E)== 0;_LL1DB:;}_LL1B3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp33A;if(_tmp348->tag != 3)goto _LL1B5;else{_tmp349=(_tmp348->f1).targs;}}_LL1B4:
# 1390 "cf_flowinfo.cyc"
 _tmp34B=_tmp349;goto _LL1B6;_LL1B5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp33A;if(_tmp34A->tag != 17)goto _LL1B7;else{_tmp34B=_tmp34A->f2;}}_LL1B6:
 _tmp34D=_tmp34B;goto _LL1B8;_LL1B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33A;if(_tmp34C->tag != 11)goto _LL1B9;else{_tmp34D=(_tmp34C->f1).targs;}}_LL1B8:
 _tmp34F=_tmp34D;goto _LL1BA;_LL1B9: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp34E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp33A;if(_tmp34E->tag != 4)goto _LL1BB;else{_tmp34F=(_tmp34E->f1).targs;}}_LL1BA:
 _tmp351=_tmp34F;goto _LL1BC;_LL1BB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp350=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33A;if(_tmp350->tag != 24)goto _LL1BD;else{_tmp351=_tmp350->f1;}}_LL1BC:
# 1395
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp351);_LL1BD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33A;if(_tmp352->tag != 5)goto _LL1BF;else{_tmp353=(_tmp352->f1).elt_typ;_tmp354=((_tmp352->f1).ptr_atts).rgn;}}_LL1BE:
# 1397
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp354) || Cyc_CfFlowInfo_contains_region(rgn,_tmp353);_LL1BF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp355=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp33A;if(_tmp355->tag != 9)goto _LL1C1;}_LL1C0:
 return 0;_LL1C1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp356=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33A;if(_tmp356->tag != 10)goto _LL1C3;else{_tmp357=_tmp356->f1;}}_LL1C2:
# 1400
 for(0;_tmp357 != 0;_tmp357=_tmp357->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple16*)_tmp357->hd)).f2))return 1;}
return 0;_LL1C3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp358=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33A;if(_tmp358->tag != 12)goto _LL1C5;else{_tmp359=_tmp358->f2;}}_LL1C4:
# 1404
 for(0;_tmp359 != 0;_tmp359=_tmp359->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp359->hd)->type))return 1;}
return 0;_LL1C5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A;if(_tmp35A->tag != 8)goto _LL1C7;else{_tmp35B=(_tmp35A->f1).elt_type;}}_LL1C6:
 _tmp35D=_tmp35B;goto _LL1C8;_LL1C7: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33A;if(_tmp35C->tag != 23)goto _LL1C9;else{_tmp35D=(void*)_tmp35C->f1;}}_LL1C8:
 _tmp35F=_tmp35D;goto _LL1CA;_LL1C9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33A;if(_tmp35E->tag != 25)goto _LL1CB;else{_tmp35F=(void*)_tmp35E->f1;}}_LL1CA:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp35F);_LL1CB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp33A;if(_tmp360->tag != 16)goto _LL1CD;else{_tmp361=(void*)_tmp360->f1;_tmp362=(void*)_tmp360->f2;}}_LL1CC:
# 1411
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp361) || Cyc_CfFlowInfo_contains_region(rgn,_tmp362);_LL1CD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp33A;if(_tmp363->tag != 15)goto _LL1CF;else{_tmp364=(void*)_tmp363->f1;}}_LL1CE:
# 1414
 return 0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp33A;if(_tmp365->tag != 18)goto _LL19A;}_LL1D0:
 return 0;_LL19A:;}struct _tuple23{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1420
static void Cyc_CfFlowInfo_kill_root(struct _tuple23*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp370;struct Cyc_Dict_Dict*_tmp371;void*_tmp372;struct _tuple23*_tmp36F=env;_tmp370=_tmp36F->f1;_tmp371=(struct Cyc_Dict_Dict*)& _tmp36F->f2;_tmp372=_tmp36F->f3;{
void*_tmp373=root;struct Cyc_Absyn_Vardecl*_tmp375;void*_tmp377;_LL1E3: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp374=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp373;if(_tmp374->tag != 0)goto _LL1E5;else{_tmp375=_tmp374->f1;}}_LL1E4:
# 1425
 if(Cyc_CfFlowInfo_contains_region(_tmp372,_tmp375->type))
# 1427
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp370,_tmp375->type,0,_tmp370->unknown_none);
*_tmp371=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp371,root,rval);
goto _LL1E2;_LL1E5: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp376=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp373;if(_tmp376->tag != 1)goto _LL1E7;else{_tmp377=(void*)_tmp376->f2;}}_LL1E6:
# 1432
 if(!Cyc_CfFlowInfo_contains_region(_tmp372,_tmp377))
# 1434
*_tmp371=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp371,root,rval);
goto _LL1E2;_LL1E7: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp378=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp373;if(_tmp378->tag != 2)goto _LL1E2;}_LL1E8:
# 1438
 goto _LL1E2;_LL1E2:;};}
# 1443
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1445
void*_stmttmp23=Cyc_Tcutil_compress(rgn);void*_tmp379=_stmttmp23;struct Cyc_Absyn_Tvar*_tmp37B;_LL1EA: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp379;if(_tmp37A->tag != 2)goto _LL1EC;else{_tmp37B=_tmp37A->f1;}}_LL1EB: {
# 1447
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp5A0;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp5A0.tag=2,((_tmp5A0.f1=_tmp37B,_tmp5A0)));
void*v2=(void*)& v;
struct _tuple23 _tmp5A1;struct _tuple23 env=(_tmp5A1.f1=fenv,((_tmp5A1.f2=fenv->mt_flowdict,((_tmp5A1.f3=v2,_tmp5A1)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1EC: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp379;if(_tmp37C->tag != 21)goto _LL1EE;}_LL1ED: {
# 1454
struct _tuple23 _tmp5A2;struct _tuple23 env=(_tmp5A2.f1=fenv,((_tmp5A2.f2=fenv->mt_flowdict,((_tmp5A2.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp5A2)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1EE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp379;if(_tmp37D->tag != 22)goto _LL1F0;}_LL1EF: {
# 1459
struct _tuple23 _tmp5A3;struct _tuple23 env=(_tmp5A3.f1=fenv,((_tmp5A3.f2=fenv->mt_flowdict,((_tmp5A3.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp5A3)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1F0:;_LL1F1: {
# 1463
struct Cyc_Core_Impossible_exn_struct _tmp5A9;const char*_tmp5A8;struct Cyc_Core_Impossible_exn_struct*_tmp5A7;(int)_throw((void*)((_tmp5A7=_cycalloc(sizeof(*_tmp5A7)),((_tmp5A7[0]=((_tmp5A9.tag=Cyc_Core_Impossible,((_tmp5A9.f1=((_tmp5A8="kill_flowdict_region",_tag_dyneither(_tmp5A8,sizeof(char),21))),_tmp5A9)))),_tmp5A7)))));}_LL1E9:;}
# 1468
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1471
return 0;}
# 1476
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp385=f;struct Cyc_Dict_Dict _tmp386;struct Cyc_List_List*_tmp387;_LL1F3: if((_tmp385.BottomFL).tag != 1)goto _LL1F5;_LL1F4:
 return f;_LL1F5: if((_tmp385.ReachableFL).tag != 2)goto _LL1F2;_tmp386=((struct _tuple11)(_tmp385.ReachableFL).val).f1;_tmp387=((struct _tuple11)(_tmp385.ReachableFL).val).f2;_LL1F6: {
# 1480
struct Cyc_Dict_Dict _tmp388=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp386,rgn);
struct Cyc_List_List*_tmp389=Cyc_CfFlowInfo_kill_relns_region(_tmp387,rgn);
# 1483
return Cyc_CfFlowInfo_ReachableFL(_tmp388,_tmp389);}_LL1F2:;}
