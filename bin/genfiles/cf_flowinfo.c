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
union Cyc_CfFlowInfo_AbsLVal _tmp40E;return((_tmp40E.PlaceL).val=x,(((_tmp40E.PlaceL).tag=1,_tmp40E)));}
# 53
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp40F;return((_tmp40F.UnknownL).val=0,(((_tmp40F.UnknownL).tag=2,_tmp40F)));}
# 56
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp410;return((_tmp410.BottomFL).val=0,(((_tmp410.BottomFL).tag=1,_tmp410)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple11 _tmp413;union Cyc_CfFlowInfo_FlowInfo _tmp412;return((_tmp412.ReachableFL).val=((_tmp413.f1=fd,((_tmp413.f2=r,_tmp413)))),(((_tmp412.ReachableFL).tag=2,_tmp412)));}
# 62
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp44A;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp449;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp448;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp447;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp446;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp445;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp444;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp443;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp442;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp441;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp440;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43F;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp43E;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43D;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp43C;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp43B;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp43A;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp439;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp438;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp437;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp436;struct Cyc_CfFlowInfo_FlowEnv*_tmp435;return(_tmp435=_region_malloc(r,sizeof(*_tmp435)),((_tmp435->r=r,((_tmp435->zero=(void*)(
# 69
(_tmp437=_region_malloc(r,sizeof(*_tmp437)),((_tmp437[0]=((_tmp436.tag=0,_tmp436)),_tmp437)))),((_tmp435->notzeroall=(void*)(
(_tmp439=_region_malloc(r,sizeof(*_tmp439)),((_tmp439[0]=((_tmp438.tag=1,_tmp438)),_tmp439)))),((_tmp435->notzerothis=(void*)(
(_tmp43B=_region_malloc(r,sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43A.tag=2,_tmp43A)),_tmp43B)))),((_tmp435->unknown_none=(void*)(
(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp43C.tag=3,((_tmp43C.f1=Cyc_CfFlowInfo_NoneIL,_tmp43C)))),_tmp43D)))),((_tmp435->unknown_this=(void*)(
(_tmp43F=_region_malloc(r,sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp43E.tag=3,((_tmp43E.f1=Cyc_CfFlowInfo_ThisIL,_tmp43E)))),_tmp43F)))),((_tmp435->unknown_all=(void*)(
(_tmp441=_region_malloc(r,sizeof(*_tmp441)),((_tmp441[0]=((_tmp440.tag=3,((_tmp440.f1=Cyc_CfFlowInfo_AllIL,_tmp440)))),_tmp441)))),((_tmp435->esc_none=(void*)(
(_tmp443=_region_malloc(r,sizeof(*_tmp443)),((_tmp443[0]=((_tmp442.tag=4,((_tmp442.f1=Cyc_CfFlowInfo_NoneIL,_tmp442)))),_tmp443)))),((_tmp435->esc_this=(void*)(
(_tmp445=_region_malloc(r,sizeof(*_tmp445)),((_tmp445[0]=((_tmp444.tag=4,((_tmp444.f1=Cyc_CfFlowInfo_ThisIL,_tmp444)))),_tmp445)))),((_tmp435->esc_all=(void*)(
(_tmp447=_region_malloc(r,sizeof(*_tmp447)),((_tmp447[0]=((_tmp446.tag=4,((_tmp446.f1=Cyc_CfFlowInfo_AllIL,_tmp446)))),_tmp447)))),((_tmp435->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp435->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp435->dummy_place=(
(_tmp44A=_region_malloc(r,sizeof(*_tmp44A)),((_tmp44A->root=(void*)((_tmp448=_region_malloc(r,sizeof(*_tmp448)),((_tmp448[0]=((_tmp449.tag=1,((_tmp449.f1=& dummy_exp,((_tmp449.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp449)))))),_tmp448)))),((_tmp44A->fields=0,_tmp44A)))))),_tmp435)))))))))))))))))))))))))));}
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
const char*_tmp44E;void*_tmp44D[1];struct Cyc_String_pa_PrintArg_struct _tmp44C;return(struct _dyneither_ptr)((_tmp44C.tag=0,((_tmp44C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp44D[0]=& _tmp44C,Cyc_aprintf(((_tmp44E="reachable from %s",_tag_dyneither(_tmp44E,sizeof(char),18))),_tag_dyneither(_tmp44D,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp451;void*_tmp450;(_tmp450=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp451="error locations not for VarRoots",_tag_dyneither(_tmp451,sizeof(char),33))),_tag_dyneither(_tmp450,sizeof(void*),0)));}_LL0:;};}
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
union Cyc_CfFlowInfo_RelnOp _tmp452;return((_tmp452.RConst).val=i,(((_tmp452.RConst).tag=1,_tmp452)));}
# 166
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RVar(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp453;return((_tmp453.RVar).val=v,(((_tmp453.RVar).tag=2,_tmp453)));}
# 169
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_CfFlowInfo_RelnOp _tmp454;return((_tmp454.RNumelts).val=v,(((_tmp454.RNumelts).tag=3,_tmp454)));}
# 172
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RTvar(struct Cyc_Absyn_Tvar*tv){
union Cyc_CfFlowInfo_RelnOp _tmp455;return((_tmp455.RTvar).val=tv,(((_tmp455.RTvar).tag=4,_tmp455)));}
# 177
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp3C=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp3C,f)== 0)return n;
n=n + 1;}{
# 184
struct Cyc_Core_Impossible_exn_struct _tmp462;const char*_tmp461;void*_tmp460[1];struct Cyc_String_pa_PrintArg_struct _tmp45F;struct Cyc_Core_Impossible_exn_struct*_tmp45E;(int)_throw((void*)((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp462.tag=Cyc_Core_Impossible,((_tmp462.f1=(struct _dyneither_ptr)((_tmp45F.tag=0,((_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp460[0]=& _tmp45F,Cyc_aprintf(((_tmp461="get_field_index_fs failed: %s",_tag_dyneither(_tmp461,sizeof(char),30))),_tag_dyneither(_tmp460,sizeof(void*),1)))))))),_tmp462)))),_tmp45E)))));};}
# 187
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp42=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp44;struct Cyc_List_List*_tmp46;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp42;if(_tmp43->tag != 11)goto _LLD;else{_tmp44=(_tmp43->f1).aggr_info;}}_LLC: {
# 190
struct Cyc_List_List*_tmp47=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp44))->impl))->fields;
_tmp46=_tmp47;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp42;if(_tmp45->tag != 12)goto _LLF;else{_tmp46=_tmp45->f2;}}_LLE:
# 193
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp46,f);_LLF:;_LL10: {
# 195
struct Cyc_Core_Impossible_exn_struct _tmp46F;const char*_tmp46E;void*_tmp46D[1];struct Cyc_String_pa_PrintArg_struct _tmp46C;struct Cyc_Core_Impossible_exn_struct*_tmp46B;(int)_throw((void*)((_tmp46B=_cycalloc(sizeof(*_tmp46B)),((_tmp46B[0]=((_tmp46F.tag=Cyc_Core_Impossible,((_tmp46F.f1=(struct _dyneither_ptr)((_tmp46C.tag=0,((_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp46D[0]=& _tmp46C,Cyc_aprintf(((_tmp46E="get_field_index failed: %s",_tag_dyneither(_tmp46E,sizeof(char),27))),_tag_dyneither(_tmp46D,sizeof(void*),1)))))))),_tmp46F)))),_tmp46B)))));}_LLA:;}
# 200
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp470;struct _tuple0 _tmp4E=(_tmp470.f1=r1,((_tmp470.f2=r2,_tmp470)));struct Cyc_Absyn_Vardecl*_tmp50;struct Cyc_Absyn_Vardecl*_tmp52;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp58;int _tmp5C;int _tmp5E;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f1;if(_tmp4F->tag != 0)goto _LL14;else{_tmp50=_tmp4F->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp51=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E.f2;if(_tmp51->tag != 0)goto _LL14;else{_tmp52=_tmp51->f1;}};_LL13:
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
{struct _dyneither_ptr*_tmp47D;const char*_tmp47C;void*_tmp47B[1];struct Cyc_String_pa_PrintArg_struct _tmp47A;struct Cyc_List_List*_tmp479;sl=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->hd=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D[0]=((_tmp47A.tag=0,((_tmp47A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6A->name).f2),((_tmp47B[0]=& _tmp47A,Cyc_aprintf(((_tmp47C="%s",_tag_dyneither(_tmp47C,sizeof(char),3))),_tag_dyneither(_tmp47B,sizeof(void*),1)))))))),_tmp47D)))),((_tmp479->tl=sl,_tmp479))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6B=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp68;if(_tmp6B->tag != 1)goto _LL25;else{_tmp6C=_tmp6B->f1;}}_LL24:
{struct _dyneither_ptr*_tmp48A;const char*_tmp489;void*_tmp488[1];struct Cyc_Int_pa_PrintArg_struct _tmp487;struct Cyc_List_List*_tmp486;sl=((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486->hd=((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp487.tag=1,((_tmp487.f1=(unsigned long)((int)_tmp6C),((_tmp488[0]=& _tmp487,Cyc_aprintf(((_tmp489="mpt%d",_tag_dyneither(_tmp489,sizeof(char),6))),_tag_dyneither(_tmp488,sizeof(void*),1)))))))),_tmp48A)))),((_tmp486->tl=sl,_tmp486))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6D=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp68;if(_tmp6D->tag != 2)goto _LL20;else{_tmp6E=_tmp6D->f1;}}_LL26:
{struct _dyneither_ptr*_tmp497;const char*_tmp496;void*_tmp495[1];struct Cyc_Int_pa_PrintArg_struct _tmp494;struct Cyc_List_List*_tmp493;sl=((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->hd=((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497[0]=((_tmp494.tag=1,((_tmp494.f1=(unsigned long)_tmp6E,((_tmp495[0]=& _tmp494,Cyc_aprintf(((_tmp496="param%d",_tag_dyneither(_tmp496,sizeof(char),8))),_tag_dyneither(_tmp495,sizeof(void*),1)))))))),_tmp497)))),((_tmp493->tl=sl,_tmp493))))));}goto _LL20;_LL20:;}
# 239
for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
struct _dyneither_ptr*_tmp4A4;const char*_tmp4A3;void*_tmp4A2[1];struct Cyc_Int_pa_PrintArg_struct _tmp4A1;struct Cyc_List_List*_tmp4A0;sl=((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->hd=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A1.tag=1,((_tmp4A1.f1=(unsigned long)((int)_tmp67->hd),((_tmp4A2[0]=& _tmp4A1,Cyc_aprintf(((_tmp4A3="%d",_tag_dyneither(_tmp4A3,sizeof(char),3))),_tag_dyneither(_tmp4A2,sizeof(void*),1)))))))),_tmp4A4)))),((_tmp4A0->tl=sl,_tmp4A0))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp4AF;const char*_tmp4AE;void*_tmp4AD[1];const char*_tmp4AC;struct _dyneither_ptr*_tmp4AB;struct _dyneither_ptr*_tmp83=(_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB[0]=((_tmp4AF.tag=0,((_tmp4AF.f1=(struct _dyneither_ptr)((_tmp4AE="",_tag_dyneither(_tmp4AE,sizeof(char),1))),((_tmp4AD[0]=& _tmp4AF,Cyc_aprintf(((_tmp4AC="%s",_tag_dyneither(_tmp4AC,sizeof(char),3))),_tag_dyneither(_tmp4AD,sizeof(void*),1)))))))),_tmp4AB)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp4B4;void*_tmp4B3[2];struct Cyc_String_pa_PrintArg_struct _tmp4B2;struct Cyc_String_pa_PrintArg_struct _tmp4B1;*_tmp83=((_tmp4B1.tag=0,((_tmp4B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp83),((_tmp4B2.tag=0,((_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp4B3[0]=& _tmp4B2,((_tmp4B3[1]=& _tmp4B1,Cyc_aprintf(((_tmp4B4="%s.%s",_tag_dyneither(_tmp4B4,sizeof(char),6))),_tag_dyneither(_tmp4B3,sizeof(void*),2))))))))))))));}
return _tmp83;};}
# 257 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp4BA(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4B9,unsigned int*_tmp4B8,void***_tmp4B6){for(*_tmp4B9=0;*_tmp4B9 < *_tmp4B8;(*_tmp4B9)++){(*_tmp4B6)[*_tmp4B9]=(void*)(*fenv)->unknown_all;}}
# 262
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 268
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp4B9;unsigned int _tmp4B8;struct _dyneither_ptr _tmp4B7;void**_tmp4B6;unsigned int _tmp4B5;struct _dyneither_ptr d=(_tmp4B5=sz,((_tmp4B6=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4B5)),((_tmp4B7=_tag_dyneither(_tmp4B6,sizeof(void*),_tmp4B5),((((_tmp4B8=_tmp4B5,_tmp4BA(& fenv,& _tmp4B9,& _tmp4B8,& _tmp4B6))),_tmp4B7)))))));
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
struct Cyc_Absyn_Aggrfield*_tmp4BB;return(_tmp4BB=_region_malloc(_tmp95,sizeof(*_tmp4BB)),((_tmp4BB->name=f->name,((_tmp4BB->tq=f->tq,((_tmp4BB->type=_tmp97,((_tmp4BB->width=f->width,((_tmp4BB->attributes=f->attributes,_tmp4BB)))))))))));};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
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
struct _tuple15*_tmp4BE;struct Cyc_List_List*_tmp4BD;inst=((_tmp4BD=_region_malloc(temp,sizeof(*_tmp4BD)),((_tmp4BD->hd=((_tmp4BE=_region_malloc(temp,sizeof(*_tmp4BE)),((_tmp4BE->f1=_tmp9A,((_tmp4BE->f2=_tmp9B,_tmp4BE)))))),((_tmp4BD->tl=inst,_tmp4BD))))));};}
# 313
if(inst != 0){
struct _tuple14 _tmp4BF;struct _tuple14 _tmp9F=(_tmp4BF.f1=temp,((_tmp4BF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp4BF)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp9F,fs);
struct _dyneither_ptr _tmpA0=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA0;}else{
# 318
struct _dyneither_ptr _tmpA2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA2;}}
# 302
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp4C5(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4C4,unsigned int*_tmp4C3,void***_tmp4C1){for(*_tmp4C4=0;*_tmp4C4 < *_tmp4C3;(*_tmp4C4)++){(*_tmp4C1)[*_tmp4C4]=(void*)(*fenv)->unknown_all;}}
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
unsigned int _tmp4C4;unsigned int _tmp4C3;struct _dyneither_ptr _tmp4C2;void**_tmp4C1;unsigned int _tmp4C0;struct _dyneither_ptr d=(_tmp4C0=sz,((_tmp4C1=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4C0)),((_tmp4C2=_tag_dyneither(_tmp4C1,sizeof(void*),_tmp4C0),((((_tmp4C3=_tmp4C0,_tmp4C5(& fenv,& _tmp4C4,& _tmp4C3,& _tmp4C1))),_tmp4C2)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpA7))->hd)).f2,leafval);
_tmpA7=_tmpA7->tl;}}{
# 342
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4CB;struct Cyc_CfFlowInfo_UnionRInfo _tmp4CA;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C9;return(void*)((_tmp4C9=_region_malloc(fenv->r,sizeof(*_tmp4C9)),((_tmp4C9[0]=((_tmp4CB.tag=6,((_tmp4CB.f1=((_tmp4CA.is_union=0,((_tmp4CA.fieldnum=- 1,_tmp4CA)))),((_tmp4CB.f2=d,_tmp4CB)))))),_tmp4C9))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA3;if(_tmpA8->tag != 11)goto _LL35;else{_tmpA9=(_tmpA8->f1).aggr_info;_tmpAA=(_tmpA8->f1).targs;}}_LL34: {
# 344
struct Cyc_Absyn_Aggrdecl*_tmpBC=Cyc_Absyn_get_known_aggrdecl(_tmpA9);
if(_tmpBC->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBC->impl))->tagged)return fenv->unknown_all;
is_tagged=1;
if(_tmpBC->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpBD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBC->impl))->fields;
if(_tmpAA == 0){_tmpAC=_tmpBC->kind;_tmpAD=_tmpBD;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4D1;struct Cyc_CfFlowInfo_UnionRInfo _tmp4D0;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4CF;return(void*)((_tmp4CF=_region_malloc(fenv->r,sizeof(*_tmp4CF)),((_tmp4CF[0]=(
(_tmp4D1.tag=6,((_tmp4D1.f1=((_tmp4D0.is_union=_tmpBC->kind == Cyc_Absyn_UnionA,((_tmp4D0.fieldnum=- 1,_tmp4D0)))),((_tmp4D1.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpBC->tvs,_tmpAA,_tmpBD,_tmpBC->kind == Cyc_Absyn_UnionA,leafval),_tmp4D1)))))),_tmp4CF))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL37;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL36:
# 357
 if(_tmpAC == Cyc_Absyn_UnionA  && !is_tagged)return fenv->unknown_all;{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4D7;struct Cyc_CfFlowInfo_UnionRInfo _tmp4D6;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D5;return(void*)((_tmp4D5=_region_malloc(fenv->r,sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D7.tag=6,((_tmp4D7.f1=((_tmp4D6.is_union=_tmpAC == Cyc_Absyn_UnionA,((_tmp4D6.fieldnum=- 1,_tmp4D6)))),((_tmp4D7.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpAD,_tmpAC == Cyc_Absyn_UnionA,leafval),_tmp4D7)))))),_tmp4D5))));};_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3;if(_tmpAE->tag != 8)goto _LL39;else{_tmpAF=(_tmpAE->f1).elt_type;_tmpB0=(_tmpAE->f1).zero_term;}}if(!
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
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp4DD(struct _dyneither_ptr*_tmpDA,unsigned int*_tmp4DC,unsigned int*_tmp4DB,void***_tmp4D9){for(*_tmp4DC=0;*_tmp4DC < *_tmp4DB;(*_tmp4DC)++){(*_tmp4D9)[*_tmp4DC]=(void*)*((void**)_check_dyneither_subscript(*_tmpDA,sizeof(void*),(int)*_tmp4DC));}}
# 410
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpD3=r;struct Cyc_Absyn_Exp*_tmpD5;int _tmpD6;void*_tmpD7;struct Cyc_CfFlowInfo_UnionRInfo _tmpD9;struct _dyneither_ptr _tmpDA;struct Cyc_Absyn_Vardecl*_tmpDC;void*_tmpDD;_LL50: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpD4->tag != 7)goto _LL52;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;_tmpD7=(void*)_tmpD4->f3;}}_LL51:
# 413
 return _tmpD7;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpD8->tag != 6)goto _LL54;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;}}_LL53: {
# 415
unsigned int _tmpDE=_get_dyneither_size(_tmpDA,sizeof(void*));
int change=0;
unsigned int _tmp4DC;unsigned int _tmp4DB;struct _dyneither_ptr _tmp4DA;void**_tmp4D9;unsigned int _tmp4D8;struct _dyneither_ptr d2=(_tmp4D8=_tmpDE,((_tmp4D9=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4D8)),((_tmp4DA=_tag_dyneither(_tmp4D9,sizeof(void*),_tmp4D8),((((_tmp4DB=_tmp4D8,_tmp4DD(& _tmpDA,& _tmp4DC,& _tmp4DB,& _tmp4D9))),_tmp4DA)))))));
{int i=0;for(0;i < _tmpDE;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmpDA,sizeof(void*),i)));
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmpDA,sizeof(void*),i)))
change=1;}}
# 423
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4E0;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4DF;return(void*)((_tmp4DF=_region_malloc(fenv->r,sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E0.tag=6,((_tmp4E0.f1=_tmpD9,((_tmp4E0.f2=d2,_tmp4E0)))))),_tmp4DF))));}else{
return r;}}_LL54: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD3;if(_tmpDB->tag != 8)goto _LL56;else{_tmpDC=_tmpDB->f1;_tmpDD=(void*)_tmpDB->f2;}}_LL55: {
# 427
void*_tmpE5=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpDD);
if(_tmpE5 != _tmpDD){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4E3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E2;return(void*)((_tmp4E2=_region_malloc(fenv->r,sizeof(*_tmp4E2)),((_tmp4E2[0]=((_tmp4E3.tag=8,((_tmp4E3.f1=_tmpDC,((_tmp4E3.f2=_tmpE5,_tmp4E3)))))),_tmp4E2))));}else{
return r;}}_LL56:;_LL57:
 return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp4ED(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp4EC,unsigned int*_tmp4EB,void***_tmp4E9){for(*_tmp4EC=0;*_tmp4EC < *_tmp4EB;(*_tmp4EC)++){(*_tmp4E9)[*_tmp4EC]=(void*)(*fenv)->unknown_all;}}static void _tmp4F6(struct _dyneither_ptr*_tmpFC,unsigned int*_tmp4F5,unsigned int*_tmp4F4,void***_tmp4F2){for(*_tmp4F5=0;*_tmp4F5 < *_tmp4F4;(*_tmp4F5)++){(*_tmp4F2)[*_tmp4F5]=(void*)((void**)(*_tmpFC).curr)[(int)*_tmp4F5];}}
# 437
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp4E4;struct _tuple0 _tmpE9=(_tmp4E4.f1=Cyc_Tcutil_compress(t),((_tmp4E4.f2=r,_tmp4E4)));struct Cyc_Absyn_Vardecl*_tmpEB;void*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_CfFlowInfo_UnionRInfo _tmpF0;struct _dyneither_ptr _tmpF1;union Cyc_Absyn_AggrInfoU _tmpF3;struct Cyc_CfFlowInfo_UnionRInfo _tmpF5;struct _dyneither_ptr _tmpF6;enum Cyc_Absyn_AggrKind _tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_CfFlowInfo_UnionRInfo _tmpFB;struct _dyneither_ptr _tmpFC;_LL59: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpEA->tag != 8)goto _LL5B;else{_tmpEB=_tmpEA->f1;_tmpEC=(void*)_tmpEA->f2;}}_LL5A: {
# 440
void*_tmpFD=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpEC);
if(_tmpFD != _tmpEC){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4E7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E6;return(void*)((_tmp4E6=_region_malloc(fenv->r,sizeof(*_tmp4E6)),((_tmp4E6[0]=((_tmp4E7.tag=8,((_tmp4E7.f1=_tmpEB,((_tmp4E7.f2=_tmpFD,_tmp4E7)))))),_tmp4E6))));}else{
return r;}}_LL5B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpED=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpED->tag != 10)goto _LL5D;else{_tmpEE=_tmpED->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpEF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpEF->tag != 6)goto _LL5D;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}};_LL5C: {
# 444
unsigned int _tmp100=_get_dyneither_size(_tmpF1,sizeof(void*));
unsigned int _tmp4EC;unsigned int _tmp4EB;struct _dyneither_ptr _tmp4EA;void**_tmp4E9;unsigned int _tmp4E8;struct _dyneither_ptr d2=(_tmp4E8=_tmp100,((_tmp4E9=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4E8)),((_tmp4EA=_tag_dyneither(_tmp4E9,sizeof(void*),_tmp4E8),((((_tmp4EB=_tmp4E8,_tmp4ED(& fenv,& _tmp4EC,& _tmp4EB,& _tmp4E9))),_tmp4EA)))))));
{int i=0;for(0;i < _tmp100;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpEE))->hd)).f2,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF1,sizeof(void*),i)));
_tmpEE=_tmpEE->tl;}}{
# 450
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F0;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4EF;return(void*)((_tmp4EF=_region_malloc(fenv->r,sizeof(*_tmp4EF)),((_tmp4EF[0]=((_tmp4F0.tag=6,((_tmp4F0.f1=_tmpF0,((_tmp4F0.f2=d2,_tmp4F0)))))),_tmp4EF))));};}_LL5D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpF2->tag != 11)goto _LL5F;else{_tmpF3=(_tmpF2->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpF4->tag != 6)goto _LL5F;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}};_LL5E: {
# 452
struct Cyc_Absyn_Aggrdecl*_tmp107=Cyc_Absyn_get_known_aggrdecl(_tmpF3);
if(_tmp107->impl == 0)return r;
_tmpF8=_tmp107->kind;_tmpF9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp107->impl))->fields;_tmpFB=_tmpF5;_tmpFC=_tmpF6;goto _LL60;}_LL5F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE9.f1;if(_tmpF7->tag != 12)goto _LL61;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpE9.f2;if(_tmpFA->tag != 6)goto _LL61;else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;}};_LL60: {
# 456
unsigned int _tmp4F5;unsigned int _tmp4F4;struct _dyneither_ptr _tmp4F3;void**_tmp4F2;unsigned int _tmp4F1;struct _dyneither_ptr d2=(_tmp4F1=_get_dyneither_size(_tmpFC,sizeof(void*)),((_tmp4F2=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp4F1)),((_tmp4F3=_tag_dyneither(_tmp4F2,sizeof(void*),_tmp4F1),((((_tmp4F4=_tmp4F1,_tmp4F6(& _tmpFC,& _tmp4F5,& _tmp4F4,& _tmp4F2))),_tmp4F3)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF9);
{int i=0;for(0;i < sz;(i ++,_tmpF9=_tmpF9->tl)){
struct _dyneither_ptr*_tmp109;void*_tmp10A;struct Cyc_Absyn_Aggrfield*_tmp108=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpF9))->hd;_tmp109=_tmp108->name;_tmp10A=_tmp108->type;
if(_get_dyneither_size(*_tmp109,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp10A,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpFC,sizeof(void*),i)));}}{
# 463
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F9;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F8;return(void*)((_tmp4F8=_region_malloc(fenv->r,sizeof(*_tmp4F8)),((_tmp4F8[0]=((_tmp4F9.tag=6,((_tmp4F9.f1=_tmpFB,((_tmp4F9.f2=d2,_tmp4F9)))))),_tmp4F8))));};}_LL61:;_LL62:
# 465
 if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4FC;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4FB;return(void*)((_tmp4FB=_region_malloc(fenv->r,sizeof(*_tmp4FB)),((_tmp4FB[0]=((_tmp4FC.tag=7,((_tmp4FC.f1=consumer,((_tmp4FC.f2=iteration,((_tmp4FC.f3=r,_tmp4FC)))))))),_tmp4FB))));}else{
return r;}_LL58:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 471
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 474
struct _RegionHandle _tmp113=_new_region("r");struct _RegionHandle*r=& _tmp113;_push_region(r);
{struct _tuple17 _tmp4FD;struct _tuple17 elem=(_tmp4FD.f1=place,((_tmp4FD.f2=0,_tmp4FD)));
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
struct Cyc_List_List*_tmp4FE;pile->places=((_tmp4FE=_region_malloc(pile->rgn,sizeof(*_tmp4FE)),((_tmp4FE->hd=place,((_tmp4FE->tl=pile->places,_tmp4FE))))));}}
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
 return;_LL63:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp504(struct _RegionHandle**r,void**new_val,unsigned int*_tmp503,unsigned int*_tmp502,void***_tmp500,struct _dyneither_ptr*_tmp12C){for(*_tmp503=0;*_tmp503 < *_tmp502;(*_tmp503)++){(*_tmp500)[*_tmp503]=(void*)
# 530
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp12C).curr)[(int)*_tmp503]);}}
# 525
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 527
void*_tmp129=old_val;int _tmp12B;struct _dyneither_ptr _tmp12C;struct Cyc_Absyn_Exp*_tmp12E;int _tmp12F;void*_tmp130;struct Cyc_Absyn_Vardecl*_tmp132;void*_tmp133;_LL6F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12A->tag != 6)goto _LL71;else{_tmp12B=(_tmp12A->f1).is_union;_tmp12C=_tmp12A->f2;}}_LL70: {
# 529
unsigned int _tmp503;unsigned int _tmp502;struct _dyneither_ptr _tmp501;void**_tmp500;unsigned int _tmp4FF;struct _dyneither_ptr d2=(_tmp4FF=_get_dyneither_size(_tmp12C,sizeof(void*)),((_tmp500=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp4FF)),((_tmp501=_tag_dyneither(_tmp500,sizeof(void*),_tmp4FF),((((_tmp502=_tmp4FF,_tmp504(& r,& new_val,& _tmp503,& _tmp502,& _tmp500,& _tmp12C))),_tmp501)))))));
# 533
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp50A;struct Cyc_CfFlowInfo_UnionRInfo _tmp509;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp508;return(void*)((_tmp508=_region_malloc(r,sizeof(*_tmp508)),((_tmp508[0]=((_tmp50A.tag=6,((_tmp50A.f1=((_tmp509.is_union=_tmp12B,((_tmp509.fieldnum=- 1,_tmp509)))),((_tmp50A.f2=d2,_tmp50A)))))),_tmp508))));}_LL71: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12D->tag != 7)goto _LL73;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;_tmp130=(void*)_tmp12D->f3;}}_LL72: {
# 535
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp50D;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp50C;return(void*)((_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C[0]=((_tmp50D.tag=7,((_tmp50D.f1=_tmp12E,((_tmp50D.f2=_tmp12F,((_tmp50D.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp130),_tmp50D)))))))),_tmp50C))));}_LL73: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp131->tag != 8)goto _LL75;else{_tmp132=_tmp131->f1;_tmp133=(void*)_tmp131->f2;}}_LL74: {
# 537
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp510;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp50F;return(void*)((_tmp50F=_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=8,((_tmp510.f1=_tmp132,((_tmp510.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp133),_tmp510)))))),_tmp50F))));}_LL75:;_LL76:
 return new_val;_LL6E:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp516(struct _dyneither_ptr*d,unsigned int*_tmp515,unsigned int*_tmp514,void***_tmp512){for(*_tmp515=0;*_tmp515 < *_tmp514;(*_tmp515)++){(*_tmp512)[*_tmp515]=(void*)((void**)(*d).curr)[(int)*_tmp515];}}
# 544
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 546
void*_tmp13F=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp13F == rval)return d;{
unsigned int _tmp515;unsigned int _tmp514;struct _dyneither_ptr _tmp513;void**_tmp512;unsigned int _tmp511;struct _dyneither_ptr res=(_tmp511=_get_dyneither_size(d,sizeof(void*)),((_tmp512=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp511)),((_tmp513=_tag_dyneither(_tmp512,sizeof(void*),_tmp511),((((_tmp514=_tmp511,_tmp516(& d,& _tmp515,& _tmp514,& _tmp512))),_tmp513)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 558
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 562
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple18 _tmp517;struct _tuple18 _tmp145=(_tmp517.f1=fs,((_tmp517.f2=old_val,_tmp517)));int _tmp146;struct Cyc_List_List*_tmp147;int _tmp149;struct _dyneither_ptr _tmp14A;struct Cyc_Absyn_Vardecl*_tmp14C;void*_tmp14D;_LL78: if(_tmp145.f1 == 0)goto _LL7A;_tmp146=(int)(_tmp145.f1)->hd;_tmp147=(_tmp145.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp148=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145.f2;if(_tmp148->tag != 6)goto _LL7A;else{_tmp149=(_tmp148->f1).is_union;_tmp14A=_tmp148->f2;}};_LL79: {
# 566
void*_tmp14E=Cyc_CfFlowInfo_insert_place_outer(r,_tmp147,*((void**)_check_dyneither_subscript(_tmp14A,sizeof(void*),_tmp146)),new_val);
struct _dyneither_ptr _tmp14F=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp14A,_tmp146,_tmp14E);
if((void**)_tmp14F.curr == (void**)_tmp14A.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp51D;struct Cyc_CfFlowInfo_UnionRInfo _tmp51C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp51B;return(void*)((_tmp51B=_region_malloc(r,sizeof(*_tmp51B)),((_tmp51B[0]=((_tmp51D.tag=6,((_tmp51D.f1=((_tmp51C.is_union=_tmp149,((_tmp51C.fieldnum=- 1,_tmp51C)))),((_tmp51D.f2=_tmp14F,_tmp51D)))))),_tmp51B))));}}_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp14B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp145.f2;if(_tmp14B->tag != 8)goto _LL7C;else{_tmp14C=_tmp14B->f1;_tmp14D=(void*)_tmp14B->f2;}}_LL7B: {
# 571
void*_tmp153=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp14D,new_val);
if(_tmp153 == _tmp14D)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp520;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp51F;return(void*)((_tmp51F=_region_malloc(r,sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp520.tag=8,((_tmp520.f1=_tmp14C,((_tmp520.f2=_tmp153,_tmp520)))))),_tmp51F))));}}_LL7C:;_LL7D: {
struct Cyc_Core_Impossible_exn_struct _tmp526;const char*_tmp525;struct Cyc_Core_Impossible_exn_struct*_tmp524;(int)_throw((void*)((_tmp524=_cycalloc(sizeof(*_tmp524)),((_tmp524[0]=((_tmp526.tag=Cyc_Core_Impossible,((_tmp526.f1=((_tmp525="bad insert place",_tag_dyneither(_tmp525,sizeof(char),17))),_tmp526)))),_tmp524)))));}_LL77:;};}
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
struct Cyc_Core_Impossible_exn_struct _tmp52C;const char*_tmp52B;struct Cyc_Core_Impossible_exn_struct*_tmp52A;(int)_throw((void*)((_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52C.tag=Cyc_Core_Impossible,((_tmp52C.f1=((_tmp52B="initlevel_approx",_tag_dyneither(_tmp52B,sizeof(char),17))),_tmp52C)))),_tmp52A)))));}_LL87:;}
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
{struct Cyc_List_List*_tmp52D;env->seen=((_tmp52D=_region_malloc(env->e,sizeof(*_tmp52D)),((_tmp52D->hd=_tmp17A,((_tmp52D->tl=env->seen,_tmp52D))))));}
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
struct Cyc_CfFlowInfo_InitlevelEnv _tmp52E;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17F=(_tmp52E.e=_tmp17E,((_tmp52E.d=d,((_tmp52E.seen=0,_tmp52E)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp17F,r,Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};
# 680
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
void*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_CfFlowInfo_Place _tmp181=*place;_tmp182=_tmp181.root;_tmp183=_tmp181.fields;{
void*_tmp184=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp182);
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple19 _tmp52F;struct _tuple19 _tmp186=(_tmp52F.f1=_tmp184,((_tmp52F.f2=(int)_tmp183->hd,_tmp52F)));struct Cyc_CfFlowInfo_UnionRInfo _tmp189;struct _dyneither_ptr _tmp18A;int _tmp18B;struct Cyc_CfFlowInfo_UnionRInfo _tmp18D;struct _dyneither_ptr _tmp18E;int _tmp18F;_LLA2:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp187=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp186.f1;if(_tmp187->tag != 8)goto _LLA4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp188=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp187->f2);if(_tmp188->tag != 6)goto _LLA4;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;}}}_tmp18B=_tmp186.f2;_LLA3:
# 686
 _tmp18D=_tmp189;_tmp18E=_tmp18A;_tmp18F=_tmp18B;goto _LLA5;_LLA4:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp18C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp186.f1;if(_tmp18C->tag != 6)goto _LLA6;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_tmp18F=_tmp186.f2;_LLA5:
# 688
 _tmp184=*((void**)_check_dyneither_subscript(_tmp18E,sizeof(void*),_tmp18F));
goto _LLA1;_LLA6:;_LLA7: {
# 694
struct Cyc_Core_Impossible_exn_struct _tmp535;const char*_tmp534;struct Cyc_Core_Impossible_exn_struct*_tmp533;(int)_throw((void*)((_tmp533=_cycalloc(sizeof(*_tmp533)),((_tmp533[0]=((_tmp535.tag=Cyc_Core_Impossible,((_tmp535.f1=((_tmp534="bad lookup_place",_tag_dyneither(_tmp534,sizeof(char),17))),_tmp535)))),_tmp533)))));}_LLA1:;}
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
struct Cyc_CfFlowInfo_EscPile*_tmp536;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp536=_region_malloc(_tmp19D,sizeof(*_tmp536)),((_tmp536->rgn=_tmp19D,((_tmp536->places=0,_tmp536)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp543(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp542,unsigned int*_tmp541,void***_tmp53F,struct _dyneither_ptr*_tmp1AF,struct _dyneither_ptr*_tmp1AC){for(*_tmp542=0;*_tmp542 < *_tmp541;(*_tmp542)++){(*_tmp53F)[*_tmp542]=(void*)
# 759
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1AC).curr)[(int)*_tmp542],*((void**)_check_dyneither_subscript(*_tmp1AF,sizeof(void*),(int)*_tmp542)));}}
# 739
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 744
struct _tuple0 _tmp537;struct _tuple0 _tmp1A0=(_tmp537.f1=oldval,((_tmp537.f2=newval,_tmp537)));void*_tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1A4;void*_tmp1A5;struct Cyc_CfFlowInfo_Place*_tmp1A8;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AB;struct _dyneither_ptr _tmp1AC;struct Cyc_CfFlowInfo_UnionRInfo _tmp1AE;struct _dyneither_ptr _tmp1AF;enum Cyc_CfFlowInfo_InitLevel _tmp1B1;_LLB4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A1->tag != 8)goto _LLB6;else{_tmp1A2=(void*)_tmp1A1->f2;}}_LLB5:
# 746
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp1A2,newval);_LLB6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1A3->tag != 8)goto _LLB8;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=(void*)_tmp1A3->f2;}}_LLB7: {
# 748
void*_tmp1B2=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp1A5);
if(_tmp1B2 == _tmp1A5)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp53A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp539;return(void*)((_tmp539=_region_malloc((env->fenv)->r,sizeof(*_tmp539)),((_tmp539[0]=((_tmp53A.tag=8,((_tmp53A.f1=_tmp1A4,((_tmp53A.f2=_tmp1B2,_tmp53A)))))),_tmp539))));}}_LLB8:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A6->tag != 4)goto _LLBA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1A7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1A7->tag != 5)goto _LLBA;else{_tmp1A8=_tmp1A7->f1;}};_LLB9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1A8);goto _LLBB;_LLBA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A9=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1A9->tag != 4)goto _LLBC;}_LLBB:
# 753
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp53D;void*_tmp53C;(_tmp53C=0,Cyc_Tcutil_terr(env->loc,((_tmp53D="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp53D,sizeof(char),67))),_tag_dyneither(_tmp53C,sizeof(void*),0)));}
# 756
return(env->fenv)->esc_all;_LLBC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f1;if(_tmp1AA->tag != 6)goto _LLBE;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1AD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1AD->tag != 6)goto _LLBE;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}};_LLBD: {
# 758
unsigned int _tmp542;unsigned int _tmp541;struct _dyneither_ptr _tmp540;void**_tmp53F;unsigned int _tmp53E;struct _dyneither_ptr new_d=
(_tmp53E=_get_dyneither_size(_tmp1AC,sizeof(void*)),((_tmp53F=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp53E)),((_tmp540=_tag_dyneither(_tmp53F,sizeof(void*),_tmp53E),((((_tmp541=_tmp53E,_tmp543(& env,& _tmp542,& _tmp541,& _tmp53F,& _tmp1AF,& _tmp1AC))),_tmp540)))))));
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
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp546;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp545;return(void*)((_tmp545=_region_malloc((env->fenv)->r,sizeof(*_tmp545)),((_tmp545[0]=((_tmp546.tag=6,((_tmp546.f1=_tmp1AE,((_tmp546.f2=new_d,_tmp546)))))),_tmp545))));};}_LLBE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1B0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A0.f2;if(_tmp1B0->tag != 4)goto _LLC0;else{_tmp1B1=_tmp1B0->f1;}}_LLBF:
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
struct _tuple18 _tmp547;struct _tuple18 _tmp1BE=(_tmp547.f1=fs,((_tmp547.f2=oldval,_tmp547)));struct Cyc_Absyn_Vardecl*_tmp1C0;void*_tmp1C1;int _tmp1C2;struct Cyc_List_List*_tmp1C3;int _tmp1C5;int _tmp1C6;struct _dyneither_ptr _tmp1C7;_LLC7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1BF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1BE.f2;if(_tmp1BF->tag != 8)goto _LLC9;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=(void*)_tmp1BF->f2;}}_LLC8: {
# 807
void*_tmp1C8=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp1C1,newval);
if(_tmp1C8 == _tmp1C1)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp54A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp549;return(void*)((_tmp549=_region_malloc((env->fenv)->r,sizeof(*_tmp549)),((_tmp549[0]=((_tmp54A.tag=8,((_tmp54A.f1=_tmp1C0,((_tmp54A.f2=_tmp1C8,_tmp54A)))))),_tmp549))));}}_LLC9: if(_tmp1BE.f1 == 0)goto _LLCB;_tmp1C2=(int)(_tmp1BE.f1)->hd;_tmp1C3=(_tmp1BE.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1C4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1BE.f2;if(_tmp1C4->tag != 6)goto _LLCB;else{_tmp1C5=(_tmp1C4->f1).is_union;_tmp1C6=(_tmp1C4->f1).fieldnum;_tmp1C7=_tmp1C4->f2;}};_LLCA: {
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
int*_tmp54B;struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp54B=_cycalloc_atomic(sizeof(*_tmp54B)),((_tmp54B[0]=fs_prefix,_tmp54B)))),_tmp1D0);
struct Cyc_CfFlowInfo_Place*_tmp54C;struct Cyc_CfFlowInfo_Place*_tmp1D2=(_tmp54C=_region_malloc((env->pile)->rgn,sizeof(*_tmp54C)),((_tmp54C->root=_tmp1CF,((_tmp54C->fields=_tmp1D1,_tmp54C)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1D2,0);};}}{
# 843
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp552;struct Cyc_CfFlowInfo_UnionRInfo _tmp551;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp550;return(void*)((_tmp550=_region_malloc((env->fenv)->r,sizeof(*_tmp550)),((_tmp550[0]=((_tmp552.tag=6,((_tmp552.f1=((_tmp551.is_union=_tmp1C5,((_tmp551.fieldnum=_tmp1C2,_tmp551)))),((_tmp552.f2=_tmp1C7,_tmp552)))))),_tmp550))));};}_LLCB:;_LLCC: {
struct Cyc_Core_Impossible_exn_struct _tmp558;const char*_tmp557;struct Cyc_Core_Impossible_exn_struct*_tmp556;(int)_throw((void*)((_tmp556=_cycalloc(sizeof(*_tmp556)),((_tmp556[0]=((_tmp558.tag=Cyc_Core_Impossible,((_tmp558.f1=((_tmp557="bad assign place",_tag_dyneither(_tmp557,sizeof(char),17))),_tmp558)))),_tmp556)))));}_LLC6:;};}
# 847
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 858
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1DB=fenv->r;
void*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1DC=place;_tmp1DD=_tmp1DC->root;_tmp1DE=_tmp1DC->fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp55B;struct Cyc_CfFlowInfo_AssignEnv _tmp55A;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp55A.fenv=fenv,((_tmp55A.pile=((_tmp55B=_region_malloc(_tmp1DB,sizeof(*_tmp55B)),((_tmp55B->rgn=_tmp1DB,((_tmp55B->places=0,_tmp55B)))))),((_tmp55A.d=d,((_tmp55A.loc=loc,((_tmp55A.root_place=place,((_tmp55A.all_changed=all_changed,_tmp55A)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1DE,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1DD),r);
# 866
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1DD,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 877
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 887
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 889
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp56E(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp56D,unsigned int*_tmp56C,void***_tmp56A,struct _dyneither_ptr*_tmp21F,struct _dyneither_ptr*_tmp21B){for(*_tmp56D=0;*_tmp56D < *_tmp56C;(*_tmp56D)++){(*_tmp56A)[*_tmp56D]=(void*)
# 963 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp21B).curr)[(int)*_tmp56D],*((void**)_check_dyneither_subscript(*_tmp21F,sizeof(void*),(int)*_tmp56D)));}}
# 899
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1E1=(env->fenv)->r;
# 903
{struct _tuple0 _tmp55C;struct _tuple0 _tmp1E3=(_tmp55C.f1=r1,((_tmp55C.f2=r2,_tmp55C)));struct Cyc_Absyn_Vardecl*_tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Vardecl*_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1EE;void*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;int _tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;int _tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F9;int _tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;int _tmp1FE;void*_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp201;struct Cyc_CfFlowInfo_Place*_tmp203;struct Cyc_CfFlowInfo_Place*_tmp205;struct Cyc_CfFlowInfo_Place*_tmp209;struct Cyc_CfFlowInfo_Place*_tmp20B;struct Cyc_CfFlowInfo_Place*_tmp20F;struct Cyc_CfFlowInfo_Place*_tmp211;struct Cyc_CfFlowInfo_Place*_tmp213;int _tmp219;int _tmp21A;struct _dyneither_ptr _tmp21B;int _tmp21D;int _tmp21E;struct _dyneither_ptr _tmp21F;_LLCE:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1E4->tag != 8)goto _LLD0;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=(void*)_tmp1E4->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1E7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1E7->tag != 8)goto _LLD0;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=(void*)_tmp1E7->f2;}};_LLCF:
# 906
 if(_tmp1E5 == _tmp1E8){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp55F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp55E;return(void*)((_tmp55E=_region_malloc(_tmp1E1,sizeof(*_tmp55E)),((_tmp55E[0]=((_tmp55F.tag=8,((_tmp55F.f1=_tmp1E5,((_tmp55F.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E6,_tmp1E9),_tmp55F)))))),_tmp55E))));}else{
# 909
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1E6,_tmp1E9);}_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1EA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1EA->tag != 8)goto _LLD2;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=(void*)_tmp1EA->f2;}}_LLD1:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1EC,r2);_LLD2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1ED=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1ED->tag != 8)goto _LLD4;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=(void*)_tmp1ED->f2;}}_LLD3:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EF);_LLD4:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp1F0->tag != 7)goto _LLD6;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;_tmp1F3=(void*)_tmp1F0->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1F4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1F4->tag != 7)goto _LLD6;else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;_tmp1F7=(void*)_tmp1F4->f3;}};_LLD5:
# 914
 if(_tmp1F1 == _tmp1F5  && _tmp1F2 == _tmp1F6){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp562;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp561;return(void*)((_tmp561=_region_malloc(_tmp1E1,sizeof(*_tmp561)),((_tmp561[0]=((_tmp562.tag=7,((_tmp562.f1=_tmp1F1,((_tmp562.f2=_tmp1F2,((_tmp562.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F3,_tmp1F7),_tmp562)))))))),_tmp561))));}
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
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp565;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp564;return(void*)((_tmp564=_region_malloc(_tmp1E1,sizeof(*_tmp564)),((_tmp564[0]=((_tmp565.tag=7,((_tmp565.f1=_tmp1F9,((_tmp565.f2=_tmp1FA,((_tmp565.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1FB,r2),_tmp565)))))))),_tmp564))));}_LLD8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1FC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp1FC->tag != 7)goto _LLDA;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;_tmp1FF=(void*)_tmp1FC->f3;}}_LLD9: {
# 928
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp568;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp567;return(void*)((_tmp567=_region_malloc(_tmp1E1,sizeof(*_tmp567)),((_tmp567[0]=((_tmp568.tag=7,((_tmp568.f1=_tmp1FD,((_tmp568.f2=_tmp1FE,((_tmp568.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1FF),_tmp568)))))))),_tmp567))));}_LLDA:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp200=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f1;if(_tmp200->tag != 5)goto _LLDC;else{_tmp201=_tmp200->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp202=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3.f2;if(_tmp202->tag != 5)goto _LLDC;else{_tmp203=_tmp202->f1;}};_LLDB:
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
unsigned int _tmp56D;unsigned int _tmp56C;struct _dyneither_ptr _tmp56B;void**_tmp56A;unsigned int _tmp569;struct _dyneither_ptr new_d=
(_tmp569=_get_dyneither_size(_tmp21B,sizeof(void*)),((_tmp56A=(void**)_region_malloc(_tmp1E1,_check_times(sizeof(void*),_tmp569)),((_tmp56B=_tag_dyneither(_tmp56A,sizeof(void*),_tmp569),((((_tmp56C=_tmp569,_tmp56E(& env,& _tmp56D,& _tmp56C,& _tmp56A,& _tmp21F,& _tmp21B))),_tmp56B)))))));
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
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp574;struct Cyc_CfFlowInfo_UnionRInfo _tmp573;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp572;return(void*)((_tmp572=_region_malloc(_tmp1E1,sizeof(*_tmp572)),((_tmp572[0]=((_tmp574.tag=6,((_tmp574.f1=((_tmp573.is_union=_tmp219,((_tmp573.fieldnum=_tmp21A == _tmp21E?_tmp21A: - 1,_tmp573)))),((_tmp574.f2=new_d,_tmp574)))))),_tmp572))));};}_LLEE:;_LLEF:
# 989
 goto _LLCD;_LLCD:;}{
# 991
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp575;struct _tuple0 _tmp236=(_tmp575.f1=r1,((_tmp575.f2=r2,_tmp575)));_LL101: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp237=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp236.f1;if(_tmp237->tag != 4)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp238=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp236.f2;if(_tmp238->tag != 4)goto _LL105;}_LL104: {
# 996
struct _tuple20 _tmp576;struct _tuple20 _tmp23A=(_tmp576.f1=il1,((_tmp576.f2=il2,_tmp576)));_LL108: if(_tmp23A.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL10A;_LL109:
 goto _LL10B;_LL10A: if(_tmp23A.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL10C;_LL10B: return(env->fenv)->esc_none;_LL10C: if(_tmp23A.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL10E;_LL10D:
 goto _LL10F;_LL10E: if(_tmp23A.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL110;_LL10F: return(env->fenv)->esc_this;_LL110:;_LL111:
 return(env->fenv)->esc_all;_LL107:;}_LL105:;_LL106: {
# 1002
struct _tuple20 _tmp577;struct _tuple20 _tmp23C=(_tmp577.f1=il1,((_tmp577.f2=il2,_tmp577)));_LL113: if(_tmp23C.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL115;_LL114:
 goto _LL116;_LL115: if(_tmp23C.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL117;_LL116: return(env->fenv)->unknown_none;_LL117: if(_tmp23C.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL119;_LL118:
 goto _LL11A;_LL119: if(_tmp23C.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL11B;_LL11A: return(env->fenv)->unknown_this;_LL11B:;_LL11C:
 return(env->fenv)->unknown_all;_LL112:;}_LL100:;};};}struct _tuple21{union Cyc_CfFlowInfo_RelnOp f1;union Cyc_CfFlowInfo_RelnOp f2;};
# 1010
static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp r1,union Cyc_CfFlowInfo_RelnOp r2){
struct _tuple21 _tmp578;struct _tuple21 _tmp23E=(_tmp578.f1=r1,((_tmp578.f2=r2,_tmp578)));unsigned int _tmp23F;unsigned int _tmp240;struct Cyc_Absyn_Vardecl*_tmp241;struct Cyc_Absyn_Vardecl*_tmp242;struct Cyc_Absyn_Vardecl*_tmp243;struct Cyc_Absyn_Vardecl*_tmp244;struct Cyc_Absyn_Tvar*_tmp245;struct Cyc_Absyn_Tvar*_tmp246;_LL11E: if(((_tmp23E.f1).RConst).tag != 1)goto _LL120;_tmp23F=(unsigned int)((_tmp23E.f1).RConst).val;if(((_tmp23E.f2).RConst).tag != 1)goto _LL120;_tmp240=(unsigned int)((_tmp23E.f2).RConst).val;_LL11F:
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
{struct Cyc_List_List*_tmp579;res=((_tmp579=_region_malloc(r,sizeof(*_tmp579)),((_tmp579->hd=_tmp248,((_tmp579->tl=res,_tmp579))))));}
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
struct _tuple22 _tmp57A;struct _tuple22 _tmp24D=(_tmp57A.f1=f1,((_tmp57A.f2=f2,_tmp57A)));struct Cyc_Dict_Dict _tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_Dict_Dict _tmp250;struct Cyc_List_List*_tmp251;_LL129: if(((_tmp24D.f1).BottomFL).tag != 1)goto _LL12B;_LL12A:
 return f2;_LL12B: if(((_tmp24D.f2).BottomFL).tag != 1)goto _LL12D;_LL12C:
 return f1;_LL12D: if(((_tmp24D.f1).ReachableFL).tag != 2)goto _LL128;_tmp24E=((struct _tuple11)((_tmp24D.f1).ReachableFL).val).f1;_tmp24F=((struct _tuple11)((_tmp24D.f1).ReachableFL).val).f2;if(((_tmp24D.f2).ReachableFL).tag != 2)goto _LL128;_tmp250=((struct _tuple11)((_tmp24D.f2).ReachableFL).val).f1;_tmp251=((struct _tuple11)((_tmp24D.f2).ReachableFL).val).f2;_LL12E:
# 1056
 if(_tmp24E.t == _tmp250.t  && _tmp24F == _tmp251)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp252=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp57D;struct Cyc_CfFlowInfo_JoinEnv _tmp57C;struct Cyc_CfFlowInfo_JoinEnv _tmp253=(_tmp57C.fenv=fenv,((_tmp57C.pile=((_tmp57D=_region_malloc(_tmp252,sizeof(*_tmp57D)),((_tmp57D->rgn=_tmp252,((_tmp57D->places=0,_tmp57D)))))),((_tmp57C.d1=_tmp24E,((_tmp57C.d2=_tmp250,_tmp57C)))))));
struct Cyc_Dict_Dict _tmp254=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp253,_tmp24E,_tmp250);
struct Cyc_List_List*_tmp255=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp24F,_tmp251);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp253.pile,all_changed,_tmp254),_tmp255);};_LL128:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1068
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){
# 1075
union Cyc_CfFlowInfo_FlowInfo _tmp259;void*_tmp25A;struct _tuple12 _tmp258=pr1;_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp25C;void*_tmp25D;struct _tuple12 _tmp25B=pr2;_tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp259,_tmp25C);
struct _tuple23 _tmp57E;struct _tuple23 _tmp25F=(_tmp57E.f1=_tmp259,((_tmp57E.f2=_tmp25C,((_tmp57E.f3=outflow,_tmp57E)))));struct Cyc_Dict_Dict _tmp260;struct Cyc_Dict_Dict _tmp261;struct Cyc_Dict_Dict _tmp262;struct Cyc_List_List*_tmp263;_LL130: if(((_tmp25F.f1).BottomFL).tag != 1)goto _LL132;_LL131: {
struct _tuple12 _tmp57F;return(_tmp57F.f1=outflow,((_tmp57F.f2=_tmp25D,_tmp57F)));}_LL132: if(((_tmp25F.f2).BottomFL).tag != 1)goto _LL134;_LL133: {
struct _tuple12 _tmp580;return(_tmp580.f1=outflow,((_tmp580.f2=_tmp25A,_tmp580)));}_LL134: if(((_tmp25F.f1).ReachableFL).tag != 2)goto _LL136;_tmp260=((struct _tuple11)((_tmp25F.f1).ReachableFL).val).f1;if(((_tmp25F.f2).ReachableFL).tag != 2)goto _LL136;_tmp261=((struct _tuple11)((_tmp25F.f2).ReachableFL).val).f1;if(((_tmp25F.f3).ReachableFL).tag != 2)goto _LL136;_tmp262=((struct _tuple11)((_tmp25F.f3).ReachableFL).val).f1;_tmp263=((struct _tuple11)((_tmp25F.f3).ReachableFL).val).f2;_LL135:
# 1083
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp25A,_tmp25D)){struct _tuple12 _tmp581;return(_tmp581.f1=outflow,((_tmp581.f2=_tmp25D,_tmp581)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp25D,_tmp25A)){struct _tuple12 _tmp582;return(_tmp582.f1=outflow,((_tmp582.f2=_tmp25A,_tmp582)));}{
struct _RegionHandle*_tmp268=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp585;struct Cyc_CfFlowInfo_JoinEnv _tmp584;struct Cyc_CfFlowInfo_JoinEnv _tmp269=(_tmp584.fenv=fenv,((_tmp584.pile=((_tmp585=_region_malloc(_tmp268,sizeof(*_tmp585)),((_tmp585->rgn=_tmp268,((_tmp585->places=0,_tmp585)))))),((_tmp584.d1=_tmp260,((_tmp584.d2=_tmp261,_tmp584)))))));
void*_tmp26A=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp269,0,_tmp25A,_tmp25D);
struct _tuple12 _tmp586;return(_tmp586.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp269.pile,all_changed,_tmp262),_tmp263),((_tmp586.f2=_tmp26A,_tmp586)));};_LL136:;_LL137: {
# 1091
struct Cyc_Core_Impossible_exn_struct _tmp58C;const char*_tmp58B;struct Cyc_Core_Impossible_exn_struct*_tmp58A;(int)_throw((void*)((_tmp58A=_cycalloc(sizeof(*_tmp58A)),((_tmp58A[0]=((_tmp58C.tag=Cyc_Core_Impossible,((_tmp58C.f1=((_tmp58B="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp58B,sizeof(char),37))),_tmp58C)))),_tmp58A)))));}_LL12F:;};};}
# 1095
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp596(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp595,unsigned int*_tmp594,void***_tmp592,struct _dyneither_ptr*_tmp284,struct _dyneither_ptr*_tmp280){for(*_tmp595=0;*_tmp595 < *_tmp594;(*_tmp595)++){(*_tmp592)[*_tmp595]=(void*)
# 1136
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp595,((void**)(*_tmp280).curr)[(int)*_tmp595],*((void**)_check_dyneither_subscript(*_tmp284,sizeof(void*),(int)*_tmp595)));}}
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
struct _tuple0 _tmp58D;struct _tuple0 _tmp272=(_tmp58D.f1=r1,((_tmp58D.f2=r2,_tmp58D)));struct Cyc_Absyn_Vardecl*_tmp276;void*_tmp277;struct Cyc_Absyn_Vardecl*_tmp279;void*_tmp27A;int _tmp27E;int _tmp27F;struct _dyneither_ptr _tmp280;int _tmp282;int _tmp283;struct _dyneither_ptr _tmp284;_LL139: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp273=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp273->tag != 7)goto _LL13B;}_LL13A:
# 1119
 goto _LL13C;_LL13B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp274=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp274->tag != 7)goto _LL13D;}_LL13C:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL13D:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp275=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp275->tag != 8)goto _LL13F;else{_tmp276=_tmp275->f1;_tmp277=(void*)_tmp275->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp278=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp278->tag != 8)goto _LL13F;else{_tmp279=_tmp278->f1;_tmp27A=(void*)_tmp278->f2;}};_LL13E:
# 1122
 if(_tmp276 == _tmp279){
void*_tmp285=Cyc_CfFlowInfo_after_absRval(env,_tmp277,_tmp27A);
if(_tmp285 == _tmp277)return r1;
if(_tmp285 == _tmp27A)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp590;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp58F;return(void*)((_tmp58F=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp58F)),((_tmp58F[0]=((_tmp590.tag=8,((_tmp590.f1=_tmp276,((_tmp590.f2=_tmp285,_tmp590)))))),_tmp58F))));};}else{
# 1128
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}_LL13F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp27B->tag != 8)goto _LL141;}_LL140:
 goto _LL142;_LL141: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp27C->tag != 8)goto _LL143;}_LL142:
# 1131
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL143:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp272.f1;if(_tmp27D->tag != 6)goto _LL145;else{_tmp27E=(_tmp27D->f1).is_union;_tmp27F=(_tmp27D->f1).fieldnum;_tmp280=_tmp27D->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp281=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp272.f2;if(_tmp281->tag != 6)goto _LL145;else{_tmp282=(_tmp281->f1).is_union;_tmp283=(_tmp281->f1).fieldnum;_tmp284=_tmp281->f2;}};_LL144: {
# 1134
unsigned int _tmp595;unsigned int _tmp594;struct _dyneither_ptr _tmp593;void**_tmp592;unsigned int _tmp591;struct _dyneither_ptr new_d=
(_tmp591=
_get_dyneither_size(_tmp280,sizeof(void*)),((_tmp592=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp591)),((_tmp593=_tag_dyneither(_tmp592,sizeof(void*),_tmp591),((((_tmp594=_tmp591,_tmp596(& env,& _tmp595,& _tmp594,& _tmp592,& _tmp284,& _tmp280))),_tmp593)))))));
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
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp59C;struct Cyc_CfFlowInfo_UnionRInfo _tmp59B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp59A;return(void*)((_tmp59A=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp59A)),((_tmp59A[0]=((_tmp59C.tag=6,((_tmp59C.f1=((_tmp59B.is_union=_tmp27E,((_tmp59B.fieldnum=_tmp27F == _tmp283?_tmp27F: - 1,_tmp59B)))),((_tmp59C.f2=new_d,_tmp59C)))))),_tmp59A))));};}_LL145:;_LL146: {
# 1165
struct Cyc_Core_Impossible_exn_struct _tmp5A2;const char*_tmp5A1;struct Cyc_Core_Impossible_exn_struct*_tmp5A0;(int)_throw((void*)((_tmp5A0=_cycalloc(sizeof(*_tmp5A0)),((_tmp5A0[0]=((_tmp5A2.tag=Cyc_Core_Impossible,((_tmp5A2.f1=((_tmp5A1="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5A1,sizeof(char),33))),_tmp5A2)))),_tmp5A0)))));}_LL138:;};}
# 1168
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1171
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp292=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp293=env->changed;
{struct Cyc_List_List*_tmp5A3;*env->last_field_cell=((_tmp5A3=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5A3)),((_tmp5A3->hd=(void*)key,((_tmp5A3->tl=0,_tmp5A3))))));}
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
{struct Cyc_CfFlowInfo_Place _tmp5A4;*env->curr_place=((_tmp5A4.root=root,((_tmp5A4.fields=0,_tmp5A4))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1193
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1199
struct _tuple22 _tmp5A5;struct _tuple22 _tmp298=(_tmp5A5.f1=f1,((_tmp5A5.f2=f2,_tmp5A5)));struct Cyc_Dict_Dict _tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_Dict_Dict _tmp29B;struct Cyc_List_List*_tmp29C;_LL148: if(((_tmp298.f1).BottomFL).tag != 1)goto _LL14A;_LL149:
 goto _LL14B;_LL14A: if(((_tmp298.f2).BottomFL).tag != 1)goto _LL14C;_LL14B:
 return Cyc_CfFlowInfo_BottomFL();_LL14C: if(((_tmp298.f1).ReachableFL).tag != 2)goto _LL147;_tmp299=((struct _tuple11)((_tmp298.f1).ReachableFL).val).f1;_tmp29A=((struct _tuple11)((_tmp298.f1).ReachableFL).val).f2;if(((_tmp298.f2).ReachableFL).tag != 2)goto _LL147;_tmp29B=((struct _tuple11)((_tmp298.f2).ReachableFL).val).f1;_tmp29C=((struct _tuple11)((_tmp298.f2).ReachableFL).val).f2;_LL14D:
# 1203
 if(_tmp299.t == _tmp29B.t  && _tmp29A == _tmp29C)return f1;{
# 1206
struct _RegionHandle*_tmp29D=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp29E=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp5A9;struct Cyc_CfFlowInfo_AfterEnv _tmp5A8;struct Cyc_CfFlowInfo_AfterEnv _tmp29F=(_tmp5A8.joinenv=(((_tmp5A8.joinenv).fenv=fenv,(((_tmp5A8.joinenv).pile=((_tmp5A9=_region_malloc(_tmp29D,sizeof(*_tmp5A9)),((_tmp5A9->rgn=_tmp29D,((_tmp5A9->places=0,_tmp5A9)))))),(((_tmp5A8.joinenv).d1=_tmp299,(((_tmp5A8.joinenv).d2=_tmp29B,_tmp5A8.joinenv)))))))),((_tmp5A8.chg1=chg1,((_tmp5A8.chg2=chg2,((_tmp5A8.curr_place=_tmp29E,((_tmp5A8.last_field_cell=& _tmp29E->fields,((_tmp5A8.changed=Cyc_CfFlowInfo_Neither,_tmp5A8)))))))))));
# 1212
struct Cyc_Dict_Dict _tmp2A0=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp29F,_tmp299,_tmp29B);
struct Cyc_List_List*_tmp2A1=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp29A,_tmp29C);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp29F.joinenv).pile,all_changed,_tmp2A0),_tmp2A1);};_LL147:;}
# 1221
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1225
{struct _tuple0 _tmp5AA;struct _tuple0 _tmp2A5=(_tmp5AA.f1=r1,((_tmp5AA.f2=r2,_tmp5AA)));struct Cyc_Absyn_Vardecl*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2AA;void*_tmp2AB;struct Cyc_CfFlowInfo_Place*_tmp2AF;struct Cyc_CfFlowInfo_Place*_tmp2B1;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B5;struct _dyneither_ptr _tmp2B6;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B8;struct _dyneither_ptr _tmp2B9;_LL14F:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f1;if(_tmp2A6->tag != 8)goto _LL151;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=(void*)_tmp2A6->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A5.f2;if(_tmp2A9->tag != 8)goto _LL151;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=(void*)_tmp2A9->f2;}};_LL150:
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
struct _tuple20 _tmp5AB;struct _tuple20 _tmp2C3=(_tmp5AB.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5AB.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp5AB)));_LL16C: if(_tmp2C3.f1 != Cyc_CfFlowInfo_AllIL)goto _LL16E;if(_tmp2C3.f2 != Cyc_CfFlowInfo_AllIL)goto _LL16E;_LL16D:
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
struct Cyc_CfFlowInfo_Reln*_tmp5AE;struct Cyc_List_List*_tmp5AD;return(_tmp5AD=_region_malloc(rgn,sizeof(*_tmp5AD)),((_tmp5AD->hd=((_tmp5AE=_region_malloc(rgn,sizeof(*_tmp5AE)),((_tmp5AE->rop1=rop1,((_tmp5AE->relation=relation,((_tmp5AE->rop2=rop2,_tmp5AE)))))))),((_tmp5AD->tl=relns,_tmp5AD)))));};}
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
struct _tuple22 _tmp5AF;struct _tuple22 _tmp2CC=(_tmp5AF.f1=f1,((_tmp5AF.f2=f2,_tmp5AF)));struct Cyc_Dict_Dict _tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;struct Cyc_List_List*_tmp2D0;_LL177: if(((_tmp2CC.f1).BottomFL).tag != 1)goto _LL179;_LL178:
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
struct Cyc_List_List*_tmp5B0;_tmp2D5=((_tmp5B0=_region_malloc(rgn,sizeof(*_tmp5B0)),((_tmp5B0->hd=_tmp2D6,((_tmp5B0->tl=_tmp2D5,_tmp5B0))))));};}
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
{void*_tmp2E1=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Tvar*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E6;_LL192: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2E1;if(_tmp2E2->tag != 19)goto _LL194;else{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)((void*)_tmp2E2->f1);if(_tmp2E3->tag != 2)goto _LL194;else{_tmp2E4=_tmp2E3->f1;}}}_LL193:
# 1357
*p=Cyc_CfFlowInfo_RTvar(_tmp2E4);
return 1;_LL194: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2E1;if(_tmp2E5->tag != 18)goto _LL196;else{_tmp2E6=_tmp2E5->f1;}}_LL195:
# 1360
 e=_tmp2E6;
goto _LL191;_LL196:;_LL197:
 goto _LL191;_LL191:;}
# 1364
{void*_tmp2E7=e->r;struct Cyc_Absyn_Exp*_tmp2E9;void*_tmp2EB;int _tmp2ED;void*_tmp2F0;struct Cyc_Absyn_Tvar*_tmp2F3;_LL199: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2E8->tag != 13)goto _LL19B;else{_tmp2E9=_tmp2E8->f2;}}_LL19A:
 return Cyc_CfFlowInfo_exp2relnop(_tmp2E9,p);_LL19B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EA->tag != 1)goto _LL19D;else{_tmp2EB=(void*)_tmp2EA->f2;}}_LL19C: {
# 1367
struct Cyc_Absyn_Vardecl*_tmp2F4=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2EB);
if(_tmp2F4 != 0){
*p=Cyc_CfFlowInfo_RVar(_tmp2F4);
return 1;}
# 1372
goto _LL198;}_LL19D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2EC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EC->tag != 0)goto _LL19F;else{if(((_tmp2EC->f1).Int_c).tag != 5)goto _LL19F;_tmp2ED=((struct _tuple6)((_tmp2EC->f1).Int_c).val).f2;}}_LL19E:
# 1374
*p=Cyc_CfFlowInfo_RConst((unsigned int)_tmp2ED);
return 1;_LL19F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2EE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EE->tag != 2)goto _LL1A1;else{if(_tmp2EE->f1 != Cyc_Absyn_Numelts)goto _LL1A1;if(_tmp2EE->f2 == 0)goto _LL1A1;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(_tmp2EE->f2)->hd)->r;if(_tmp2EF->tag != 1)goto _LL1A1;else{_tmp2F0=(void*)_tmp2EF->f2;}};}}_LL1A0: {
# 1377
struct Cyc_Absyn_Vardecl*_tmp2F5=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2F0);
if(_tmp2F5 != 0){
*p=Cyc_CfFlowInfo_RNumelts(_tmp2F5);
return 1;}
# 1382
goto _LL198;}_LL1A1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2F1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2F1->tag != 38)goto _LL1A3;else{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)((void*)_tmp2F1->f1);if(_tmp2F2->tag != 2)goto _LL1A3;else{_tmp2F3=_tmp2F2->f1;}}}_LL1A2:
# 1384
*p=Cyc_CfFlowInfo_RTvar(_tmp2F3);
return 1;_LL1A3:;_LL1A4:
 goto _LL198;_LL198:;}
# 1388
return 0;}
# 1391
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp2F6=e->r;void*_tmp2F8;_LL1A6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F6;if(_tmp2F7->tag != 1)goto _LL1A8;else{_tmp2F8=(void*)_tmp2F7->f2;}}_LL1A7: {
# 1394
struct Cyc_Absyn_Vardecl*_tmp2F9=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2F8);
if(_tmp2F9 != 0)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2F9);else{
return r;}}_LL1A8:;_LL1A9:
 return r;_LL1A5:;}
# 1402
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp5B1;return(_tmp5B1=_region_malloc(r2,sizeof(*_tmp5B1)),((_tmp5B1[0]=*r,_tmp5B1)));}
# 1406
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
# 1410
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_CfFlowInfo_Reln*_tmp2FB=(struct Cyc_CfFlowInfo_Reln*)r1->hd;
struct Cyc_CfFlowInfo_Reln*_tmp2FC=(struct Cyc_CfFlowInfo_Reln*)r2->hd;
if((!Cyc_CfFlowInfo_same_relop(_tmp2FB->rop1,_tmp2FC->rop1) || _tmp2FB->relation != _tmp2FC->relation) || !
# 1416
Cyc_CfFlowInfo_same_relop(_tmp2FB->rop2,_tmp2FC->rop2))return 0;}
# 1418
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 1422
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 1426
if(v->escapes)return r;
# 1429
r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);
# 1434
{void*_tmp2FD=e->r;struct Cyc_Absyn_Exp*_tmp2FF;_LL1AB: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2FE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD;if(_tmp2FE->tag != 33)goto _LL1AD;else{_tmp2FF=(_tmp2FE->f1).num_elts;if((_tmp2FE->f1).fat_result != 1)goto _LL1AD;}}_LL1AC:
# 1437
 malloc_loop: {
void*_tmp300=_tmp2FF->r;struct Cyc_Absyn_Exp*_tmp302;void*_tmp304;_LL1B0: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp301=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp300;if(_tmp301->tag != 13)goto _LL1B2;else{_tmp302=_tmp301->f2;}}_LL1B1:
 _tmp2FF=_tmp302;goto malloc_loop;_LL1B2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp303=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp300;if(_tmp303->tag != 1)goto _LL1B4;else{_tmp304=(void*)_tmp303->f2;}}_LL1B3: {
# 1441
struct Cyc_Absyn_Vardecl*_tmp305=Cyc_CfFlowInfo_nonesc_vardecl(_tmp304);
if(_tmp305 != 0)
return Cyc_CfFlowInfo_add_relation(rgn,Cyc_CfFlowInfo_RVar(_tmp305),Cyc_CfFlowInfo_Req,Cyc_CfFlowInfo_RNumelts(v),r);else{
return r;}}_LL1B4:;_LL1B5:
 return r;_LL1AF:;}_LL1AD:;_LL1AE:
# 1448
 goto _LL1AA;_LL1AA:;}
# 1451
{void*_tmp306=Cyc_Tcutil_compress(v->type);_LL1B7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp307=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp306;if(_tmp307->tag != 6)goto _LL1B9;}_LL1B8:
 goto _LL1B6;_LL1B9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp308=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp306;if(_tmp308->tag != 19)goto _LL1BB;}_LL1BA:
 goto _LL1B6;_LL1BB:;_LL1BC:
 return r;_LL1B6:;}{
# 1457
union Cyc_CfFlowInfo_RelnOp eop=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(e,& eop))
return Cyc_CfFlowInfo_add_relation(rgn,Cyc_CfFlowInfo_RVar(v),Cyc_CfFlowInfo_Req,eop,r);
# 1461
return r;};}
# 1464
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp309=e1->r;void*_tmp30B;_LL1BE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp309;if(_tmp30A->tag != 1)goto _LL1C0;else{_tmp30B=(void*)_tmp30A->f2;}}_LL1BF: {
# 1467
struct Cyc_Absyn_Vardecl*_tmp30C=Cyc_CfFlowInfo_nonesc_vardecl(_tmp30B);
if(_tmp30C != 0)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp30C,e2);else{
return r;}}_LL1C0:;_LL1C1:
 return r;_LL1BD:;}
# 1476
static union Cyc_Pratt_Node Cyc_CfFlowInfo_rop2node(union Cyc_CfFlowInfo_RelnOp r){
union Cyc_CfFlowInfo_RelnOp _tmp30D=r;struct Cyc_Absyn_Vardecl*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_Absyn_Tvar*_tmp310;_LL1C3: if((_tmp30D.RVar).tag != 2)goto _LL1C5;_tmp30E=(struct Cyc_Absyn_Vardecl*)(_tmp30D.RVar).val;_LL1C4:
 return Cyc_Pratt_NVar(_tmp30E);_LL1C5: if((_tmp30D.RNumelts).tag != 3)goto _LL1C7;_tmp30F=(struct Cyc_Absyn_Vardecl*)(_tmp30D.RNumelts).val;_LL1C6:
 return Cyc_Pratt_NNumelts(_tmp30F);_LL1C7: if((_tmp30D.RTvar).tag != 4)goto _LL1C9;_tmp310=(struct Cyc_Absyn_Tvar*)(_tmp30D.RTvar).val;_LL1C8:
 return Cyc_Pratt_NTvar(_tmp310);_LL1C9:;_LL1CA: {
const char*_tmp5B4;void*_tmp5B3;(_tmp5B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B4="rop2node",_tag_dyneither(_tmp5B4,sizeof(char),9))),_tag_dyneither(_tmp5B3,sizeof(void*),0)));}_LL1C2:;}
# 1485
static struct _dyneither_ptr Cyc_CfFlowInfo_relation2string(enum Cyc_CfFlowInfo_Relation r){
switch(r){case Cyc_CfFlowInfo_Req: _LL1CB: {
const char*_tmp5B5;return(_tmp5B5="==",_tag_dyneither(_tmp5B5,sizeof(char),3));}case Cyc_CfFlowInfo_Rneq: _LL1CC: {
const char*_tmp5B6;return(_tmp5B6="!=",_tag_dyneither(_tmp5B6,sizeof(char),3));}case Cyc_CfFlowInfo_Rlt: _LL1CD: {
const char*_tmp5B7;return(_tmp5B7="<",_tag_dyneither(_tmp5B7,sizeof(char),2));}case Cyc_CfFlowInfo_Rlte: _LL1CE: {
const char*_tmp5B8;return(_tmp5B8="<=",_tag_dyneither(_tmp5B8,sizeof(char),3));}}}
# 1494
static struct _dyneither_ptr Cyc_CfFlowInfo_rop2string(union Cyc_CfFlowInfo_RelnOp r){
union Cyc_CfFlowInfo_RelnOp _tmp317=r;unsigned int _tmp318;struct Cyc_Absyn_Vardecl*_tmp319;struct Cyc_Absyn_Tvar*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;_LL1D1: if((_tmp317.RConst).tag != 1)goto _LL1D3;_tmp318=(unsigned int)(_tmp317.RConst).val;_LL1D2: {
const char*_tmp5BC;void*_tmp5BB[1];struct Cyc_Int_pa_PrintArg_struct _tmp5BA;return(struct _dyneither_ptr)((_tmp5BA.tag=1,((_tmp5BA.f1=(unsigned long)((int)_tmp318),((_tmp5BB[0]=& _tmp5BA,Cyc_aprintf(((_tmp5BC="%d",_tag_dyneither(_tmp5BC,sizeof(char),3))),_tag_dyneither(_tmp5BB,sizeof(void*),1))))))));}_LL1D3: if((_tmp317.RVar).tag != 2)goto _LL1D5;_tmp319=(struct Cyc_Absyn_Vardecl*)(_tmp317.RVar).val;_LL1D4: {
const char*_tmp5C0;void*_tmp5BF[1];struct Cyc_String_pa_PrintArg_struct _tmp5BE;return(struct _dyneither_ptr)((_tmp5BE.tag=0,((_tmp5BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp319->name)),((_tmp5BF[0]=& _tmp5BE,Cyc_aprintf(((_tmp5C0="%s",_tag_dyneither(_tmp5C0,sizeof(char),3))),_tag_dyneither(_tmp5BF,sizeof(void*),1))))))));}_LL1D5: if((_tmp317.RTvar).tag != 4)goto _LL1D7;_tmp31A=(struct Cyc_Absyn_Tvar*)(_tmp317.RTvar).val;_LL1D6:
 return Cyc_Absynpp_tvar2string(_tmp31A);_LL1D7: if((_tmp317.RNumelts).tag != 3)goto _LL1D0;_tmp31B=(struct Cyc_Absyn_Vardecl*)(_tmp317.RNumelts).val;_LL1D8: {
const char*_tmp5C4;void*_tmp5C3[1];struct Cyc_String_pa_PrintArg_struct _tmp5C2;return(struct _dyneither_ptr)((_tmp5C2.tag=0,((_tmp5C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31B->name)),((_tmp5C3[0]=& _tmp5C2,Cyc_aprintf(((_tmp5C4="numelts(%s)",_tag_dyneither(_tmp5C4,sizeof(char),12))),_tag_dyneither(_tmp5C3,sizeof(void*),1))))))));}_LL1D0:;}
# 1503
static struct _dyneither_ptr Cyc_CfFlowInfo_reln2string(struct Cyc_CfFlowInfo_Reln*r){
const char*_tmp5CA;void*_tmp5C9[3];struct Cyc_String_pa_PrintArg_struct _tmp5C8;struct Cyc_String_pa_PrintArg_struct _tmp5C7;struct Cyc_String_pa_PrintArg_struct _tmp5C6;return(struct _dyneither_ptr)((_tmp5C6.tag=0,((_tmp5C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_CfFlowInfo_rop2string(r->rop2)),((_tmp5C7.tag=0,((_tmp5C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_CfFlowInfo_relation2string(r->relation)),((_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_rop2string(r->rop1)),((_tmp5C9[0]=& _tmp5C8,((_tmp5C9[1]=& _tmp5C7,((_tmp5C9[2]=& _tmp5C6,Cyc_aprintf(((_tmp5CA="%s %s %s",_tag_dyneither(_tmp5CA,sizeof(char),9))),_tag_dyneither(_tmp5C9,sizeof(void*),3))))))))))))))))))));}
# 1508
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
{const char*_tmp5CE;void*_tmp5CD[1];struct Cyc_String_pa_PrintArg_struct _tmp5CC;(_tmp5CC.tag=0,((_tmp5CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_reln2string((struct Cyc_CfFlowInfo_Reln*)r->hd)),((_tmp5CD[0]=& _tmp5CC,Cyc_fprintf(Cyc_stderr,((_tmp5CE="%s",_tag_dyneither(_tmp5CE,sizeof(char),3))),_tag_dyneither(_tmp5CD,sizeof(void*),1)))))));}
if(r->tl != 0){const char*_tmp5D1;void*_tmp5D0;(_tmp5D0=0,Cyc_fprintf(Cyc_stderr,((_tmp5D1=",",_tag_dyneither(_tmp5D1,sizeof(char),2))),_tag_dyneither(_tmp5D0,sizeof(void*),0)));}}}
# 1520
int Cyc_CfFlowInfo_consistent_relations(struct Cyc_List_List*rlns){
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
for(0;(unsigned int)G  && rlns != 0;rlns=rlns->tl){
# 1526
struct Cyc_CfFlowInfo_Reln*_tmp32F=(struct Cyc_CfFlowInfo_Reln*)rlns->hd;unsigned int _tmp330;enum Cyc_CfFlowInfo_Relation _tmp331;unsigned int _tmp332;union Cyc_CfFlowInfo_RelnOp _tmp333;enum Cyc_CfFlowInfo_Relation _tmp334;unsigned int _tmp335;unsigned int _tmp336;enum Cyc_CfFlowInfo_Relation _tmp337;union Cyc_CfFlowInfo_RelnOp _tmp338;union Cyc_CfFlowInfo_RelnOp _tmp339;enum Cyc_CfFlowInfo_Relation _tmp33A;union Cyc_CfFlowInfo_RelnOp _tmp33B;_LL1DA: if(((_tmp32F->rop1).RConst).tag != 1)goto _LL1DC;_tmp330=(unsigned int)((_tmp32F->rop1).RConst).val;_tmp331=_tmp32F->relation;if(((_tmp32F->rop2).RConst).tag != 1)goto _LL1DC;_tmp332=(unsigned int)((_tmp32F->rop2).RConst).val;_LL1DB:
# 1528
 switch(_tmp331){case Cyc_CfFlowInfo_Req: _LL1E4:
 if(_tmp330 != _tmp332)return 0;break;case Cyc_CfFlowInfo_Rneq: _LL1E5:
 if(_tmp330 == _tmp332)return 0;break;case Cyc_CfFlowInfo_Rlt: _LL1E6:
 if(_tmp330 >= _tmp332)return 0;break;case Cyc_CfFlowInfo_Rlte: _LL1E7:
 if(_tmp330 > _tmp332)return 0;break;}
# 1534
goto _LL1D9;_LL1DC: if(_tmp32F->relation != Cyc_CfFlowInfo_Rneq)goto _LL1DE;_LL1DD:
# 1536
 goto _LL1D9;_LL1DE: _tmp333=_tmp32F->rop1;_tmp334=_tmp32F->relation;if(((_tmp32F->rop2).RConst).tag != 1)goto _LL1E0;_tmp335=(unsigned int)((_tmp32F->rop2).RConst).val;_LL1DF: {
# 1540
union Cyc_Pratt_Node _tmp33C=Cyc_CfFlowInfo_rop2node(_tmp333);
if(_tmp334 == Cyc_CfFlowInfo_Rlt)_tmp335=_tmp335 - 1;
G=Cyc_Pratt_add_edge(G,_tmp33C,Cyc_Pratt_zero_node,(int)_tmp335);
# 1544
if((unsigned int)G  && _tmp334 == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp33C,(int)(- _tmp335));
goto _LL1D9;}_LL1E0: if(((_tmp32F->rop1).RConst).tag != 1)goto _LL1E2;_tmp336=(unsigned int)((_tmp32F->rop1).RConst).val;_tmp337=_tmp32F->relation;_tmp338=_tmp32F->rop2;_LL1E1: {
# 1550
union Cyc_Pratt_Node _tmp33D=Cyc_CfFlowInfo_rop2node(_tmp338);
if(_tmp337 == Cyc_CfFlowInfo_Rlt)_tmp336=_tmp336 + 1;
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp33D,(int)(- _tmp336));
# 1554
if((unsigned int)G  && _tmp337 == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,_tmp33D,Cyc_Pratt_zero_node,(int)_tmp336);
goto _LL1D9;}_LL1E2: _tmp339=_tmp32F->rop1;_tmp33A=_tmp32F->relation;_tmp33B=_tmp32F->rop2;_LL1E3: {
# 1559
union Cyc_Pratt_Node _tmp33E=Cyc_CfFlowInfo_rop2node(_tmp339);
union Cyc_Pratt_Node _tmp33F=Cyc_CfFlowInfo_rop2node(_tmp33B);
int i=_tmp33A == Cyc_CfFlowInfo_Rlt?- 1: 0;
# 1564
G=Cyc_Pratt_add_edge(G,_tmp33E,_tmp33F,i);
# 1566
if((unsigned int)G  && _tmp33A == Cyc_CfFlowInfo_Req)
G=Cyc_Pratt_add_edge(G,_tmp33F,_tmp33E,i);
goto _LL1D9;}_LL1D9:;}
# 1571
if(G != 0)return 1;else{return 0;}}
# 1574
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1E9:
{const char*_tmp5D4;void*_tmp5D3;(_tmp5D3=0,Cyc_fprintf(Cyc_stderr,((_tmp5D4="uninitialized",_tag_dyneither(_tmp5D4,sizeof(char),14))),_tag_dyneither(_tmp5D3,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL1EA:
{const char*_tmp5D7;void*_tmp5D6;(_tmp5D6=0,Cyc_fprintf(Cyc_stderr,((_tmp5D7="this-initialized",_tag_dyneither(_tmp5D7,sizeof(char),17))),_tag_dyneither(_tmp5D6,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1EB:
{const char*_tmp5DA;void*_tmp5D9;(_tmp5D9=0,Cyc_fprintf(Cyc_stderr,((_tmp5DA="all-initialized",_tag_dyneither(_tmp5DA,sizeof(char),16))),_tag_dyneither(_tmp5D9,sizeof(void*),0)));}break;}}
# 1582
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp346=root;struct Cyc_Absyn_Vardecl*_tmp348;struct Cyc_Absyn_Exp*_tmp34A;void*_tmp34B;_LL1EE: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp347=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp346;if(_tmp347->tag != 0)goto _LL1F0;else{_tmp348=_tmp347->f1;}}_LL1EF:
# 1585
{const char*_tmp5DE;void*_tmp5DD[1];struct Cyc_String_pa_PrintArg_struct _tmp5DC;(_tmp5DC.tag=0,((_tmp5DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp348->name)),((_tmp5DD[0]=& _tmp5DC,Cyc_fprintf(Cyc_stderr,((_tmp5DE="Root(%s)",_tag_dyneither(_tmp5DE,sizeof(char),9))),_tag_dyneither(_tmp5DD,sizeof(void*),1)))))));}goto _LL1ED;_LL1F0: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp349=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp346;if(_tmp349->tag != 1)goto _LL1F2;else{_tmp34A=_tmp349->f1;_tmp34B=(void*)_tmp349->f2;}}_LL1F1:
# 1587
{const char*_tmp5E3;void*_tmp5E2[2];struct Cyc_String_pa_PrintArg_struct _tmp5E1;struct Cyc_String_pa_PrintArg_struct _tmp5E0;(_tmp5E0.tag=0,((_tmp5E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp34B)),((_tmp5E1.tag=0,((_tmp5E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp34A)),((_tmp5E2[0]=& _tmp5E1,((_tmp5E2[1]=& _tmp5E0,Cyc_fprintf(Cyc_stderr,((_tmp5E3="MallocPt(%s,%s)",_tag_dyneither(_tmp5E3,sizeof(char),16))),_tag_dyneither(_tmp5E2,sizeof(void*),2)))))))))))));}goto _LL1ED;_LL1F2: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp34C=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp346;if(_tmp34C->tag != 2)goto _LL1ED;}_LL1F3:
# 1590
{const char*_tmp5E6;void*_tmp5E5;(_tmp5E5=0,Cyc_fprintf(Cyc_stderr,((_tmp5E6="InitParam(_,_)",_tag_dyneither(_tmp5E6,sizeof(char),15))),_tag_dyneither(_tmp5E5,sizeof(void*),0)));}goto _LL1ED;_LL1ED:;}
# 1594
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp356=p->fields;
while(_tmp356 != 0){
{const char*_tmp5EA;void*_tmp5E9[1];struct Cyc_Int_pa_PrintArg_struct _tmp5E8;(_tmp5E8.tag=1,((_tmp5E8.f1=(unsigned long)((int)_tmp356->hd),((_tmp5E9[0]=& _tmp5E8,Cyc_fprintf(Cyc_stderr,((_tmp5EA=".%d",_tag_dyneither(_tmp5EA,sizeof(char),4))),_tag_dyneither(_tmp5E9,sizeof(void*),1)))))));}
_tmp356=_tmp356->tl;}{
# 1601
const char*_tmp5ED;void*_tmp5EC;(_tmp5EC=0,Cyc_fprintf(Cyc_stderr,((_tmp5ED=" ",_tag_dyneither(_tmp5ED,sizeof(char),2))),_tag_dyneither(_tmp5EC,sizeof(void*),0)));};};}
# 1604
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp35C=1;
while(x != 0){
if(!_tmp35C){{const char*_tmp5F0;void*_tmp5EF;(_tmp5EF=0,Cyc_fprintf(Cyc_stderr,((_tmp5F0=", ",_tag_dyneither(_tmp5F0,sizeof(char),3))),_tag_dyneither(_tmp5EF,sizeof(void*),0)));}_tmp35C=0;}
pr(x->hd);
x=x->tl;}{
# 1611
const char*_tmp5F3;void*_tmp5F2;(_tmp5F2=0,Cyc_fprintf(Cyc_stderr,((_tmp5F3="\n",_tag_dyneither(_tmp5F3,sizeof(char),2))),_tag_dyneither(_tmp5F2,sizeof(void*),0)));};}
# 1614
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp361=_new_region("r");struct _RegionHandle*r=& _tmp361;_push_region(r);
{struct _handler_cons _tmp362;_push_handler(& _tmp362);{int _tmp364=0;if(setjmp(_tmp362.handler))_tmp364=1;if(!_tmp364){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple13 _tmp365=*((struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp5F6;void*_tmp5F5;(_tmp5F5=0,Cyc_fprintf(Cyc_stderr,((_tmp5F6="{ ",_tag_dyneither(_tmp5F6,sizeof(char),3))),_tag_dyneither(_tmp5F5,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(iter,& _tmp365)){
void*_tmp368=_tmp365.f1;
if(!first){
{const char*_tmp5F9;void*_tmp5F8;(_tmp5F8=0,Cyc_fprintf(Cyc_stderr,((_tmp5F9=", ",_tag_dyneither(_tmp5F9,sizeof(char),3))),_tag_dyneither(_tmp5F8,sizeof(void*),0)));}
first=0;}
# 1627
pr(_tmp368);}{
# 1629
const char*_tmp5FC;void*_tmp5FB;(_tmp5FB=0,Cyc_fprintf(Cyc_stderr,((_tmp5FC="}\n",_tag_dyneither(_tmp5FC,sizeof(char),3))),_tag_dyneither(_tmp5FB,sizeof(void*),0)));};}
# 1617
;_pop_handler();}else{void*_tmp363=(void*)_exn_thrown;void*_tmp36E=_tmp363;void*_tmp36F;void*_tmp370;_LL1F5: _tmp36F=_tmp36E;_LL1F6:
# 1632
{const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,Cyc_fprintf(Cyc_stderr,((_tmp5FF="{ }\n",_tag_dyneither(_tmp5FF,sizeof(char),5))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}
_npop_handler(0);return;_LL1F7: _tmp370=_tmp36E;_LL1F8:(void)_throw(_tmp370);_LL1F4:;}};}
# 1616
;_pop_region(r);}
# 1638
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp373=rval;enum Cyc_CfFlowInfo_InitLevel _tmp378;enum Cyc_CfFlowInfo_InitLevel _tmp37A;struct Cyc_CfFlowInfo_Place*_tmp37C;int _tmp37E;int _tmp37F;struct _dyneither_ptr _tmp380;struct Cyc_Absyn_Exp*_tmp382;int _tmp383;void*_tmp384;struct Cyc_Absyn_Vardecl*_tmp386;void*_tmp387;_LL1FA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp374=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp374->tag != 0)goto _LL1FC;}_LL1FB:
{const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_fprintf(Cyc_stderr,((_tmp602="Zero",_tag_dyneither(_tmp602,sizeof(char),5))),_tag_dyneither(_tmp601,sizeof(void*),0)));}goto _LL1F9;_LL1FC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp375=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp375->tag != 1)goto _LL1FE;}_LL1FD:
{const char*_tmp605;void*_tmp604;(_tmp604=0,Cyc_fprintf(Cyc_stderr,((_tmp605="NotZeroAll",_tag_dyneither(_tmp605,sizeof(char),11))),_tag_dyneither(_tmp604,sizeof(void*),0)));}goto _LL1F9;_LL1FE: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp376=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp376->tag != 2)goto _LL200;}_LL1FF:
{const char*_tmp608;void*_tmp607;(_tmp607=0,Cyc_fprintf(Cyc_stderr,((_tmp608="NotZeroThis",_tag_dyneither(_tmp608,sizeof(char),12))),_tag_dyneither(_tmp607,sizeof(void*),0)));}goto _LL1F9;_LL200: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp377=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp377->tag != 3)goto _LL202;else{_tmp378=_tmp377->f1;}}_LL201:
{const char*_tmp60B;void*_tmp60A;(_tmp60A=0,Cyc_fprintf(Cyc_stderr,((_tmp60B="Unknown(",_tag_dyneither(_tmp60B,sizeof(char),9))),_tag_dyneither(_tmp60A,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp378);
{const char*_tmp60E;void*_tmp60D;(_tmp60D=0,Cyc_fprintf(Cyc_stderr,((_tmp60E=")",_tag_dyneither(_tmp60E,sizeof(char),2))),_tag_dyneither(_tmp60D,sizeof(void*),0)));}goto _LL1F9;_LL202: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp379=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp379->tag != 4)goto _LL204;else{_tmp37A=_tmp379->f1;}}_LL203:
{const char*_tmp611;void*_tmp610;(_tmp610=0,Cyc_fprintf(Cyc_stderr,((_tmp611="Esc(",_tag_dyneither(_tmp611,sizeof(char),5))),_tag_dyneither(_tmp610,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp37A);
{const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_fprintf(Cyc_stderr,((_tmp614=")",_tag_dyneither(_tmp614,sizeof(char),2))),_tag_dyneither(_tmp613,sizeof(void*),0)));}goto _LL1F9;_LL204: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp37B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp37B->tag != 5)goto _LL206;else{_tmp37C=_tmp37B->f1;}}_LL205:
{const char*_tmp617;void*_tmp616;(_tmp616=0,Cyc_fprintf(Cyc_stderr,((_tmp617="AddrOf(",_tag_dyneither(_tmp617,sizeof(char),8))),_tag_dyneither(_tmp616,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp37C);
{const char*_tmp61A;void*_tmp619;(_tmp619=0,Cyc_fprintf(Cyc_stderr,((_tmp61A=")",_tag_dyneither(_tmp61A,sizeof(char),2))),_tag_dyneither(_tmp619,sizeof(void*),0)));}goto _LL1F9;_LL206: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp37D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp37D->tag != 6)goto _LL208;else{_tmp37E=(_tmp37D->f1).is_union;_tmp37F=(_tmp37D->f1).fieldnum;_tmp380=_tmp37D->f2;}}_LL207:
# 1650
 if(_tmp37E){
{const char*_tmp61D;void*_tmp61C;(_tmp61C=0,Cyc_fprintf(Cyc_stderr,((_tmp61D="AggrUnion{",_tag_dyneither(_tmp61D,sizeof(char),11))),_tag_dyneither(_tmp61C,sizeof(void*),0)));}
if(_tmp37F != - 1){
const char*_tmp621;void*_tmp620[1];struct Cyc_Int_pa_PrintArg_struct _tmp61F;(_tmp61F.tag=1,((_tmp61F.f1=(unsigned long)_tmp37F,((_tmp620[0]=& _tmp61F,Cyc_fprintf(Cyc_stderr,((_tmp621="tag == %d;",_tag_dyneither(_tmp621,sizeof(char),11))),_tag_dyneither(_tmp620,sizeof(void*),1)))))));}}else{
# 1655
const char*_tmp624;void*_tmp623;(_tmp623=0,Cyc_fprintf(Cyc_stderr,((_tmp624="AggrStruct{",_tag_dyneither(_tmp624,sizeof(char),12))),_tag_dyneither(_tmp623,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp380,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp380.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp380,sizeof(void*))){const char*_tmp627;void*_tmp626;(_tmp626=0,Cyc_fprintf(Cyc_stderr,((_tmp627=",",_tag_dyneither(_tmp627,sizeof(char),2))),_tag_dyneither(_tmp626,sizeof(void*),0)));}}}
# 1660
{const char*_tmp62A;void*_tmp629;(_tmp629=0,Cyc_fprintf(Cyc_stderr,((_tmp62A="}",_tag_dyneither(_tmp62A,sizeof(char),2))),_tag_dyneither(_tmp629,sizeof(void*),0)));}
goto _LL1F9;_LL208: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp381=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp381->tag != 7)goto _LL20A;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;_tmp384=(void*)_tmp381->f3;}}_LL209:
# 1663
{const char*_tmp62F;void*_tmp62E[2];struct Cyc_String_pa_PrintArg_struct _tmp62D;struct Cyc_Int_pa_PrintArg_struct _tmp62C;(_tmp62C.tag=1,((_tmp62C.f1=(unsigned long)_tmp383,((_tmp62D.tag=0,((_tmp62D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp382)),((_tmp62E[0]=& _tmp62D,((_tmp62E[1]=& _tmp62C,Cyc_fprintf(Cyc_stderr,((_tmp62F="[Consumed(%s,%d,",_tag_dyneither(_tmp62F,sizeof(char),17))),_tag_dyneither(_tmp62E,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp384);{const char*_tmp632;void*_tmp631;(_tmp631=0,Cyc_fprintf(Cyc_stderr,((_tmp632="]",_tag_dyneither(_tmp632,sizeof(char),2))),_tag_dyneither(_tmp631,sizeof(void*),0)));}
goto _LL1F9;_LL20A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp385=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp373;if(_tmp385->tag != 8)goto _LL1F9;else{_tmp386=_tmp385->f1;_tmp387=(void*)_tmp385->f2;}}_LL20B:
# 1667
{const char*_tmp636;void*_tmp635[1];struct Cyc_String_pa_PrintArg_struct _tmp634;(_tmp634.tag=0,((_tmp634.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp386->name)),((_tmp635[0]=& _tmp634,Cyc_fprintf(Cyc_stderr,((_tmp636="[NamedLocation(%s,",_tag_dyneither(_tmp636,sizeof(char),19))),_tag_dyneither(_tmp635,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp387);{const char*_tmp639;void*_tmp638;(_tmp638=0,Cyc_fprintf(Cyc_stderr,((_tmp639="]",_tag_dyneither(_tmp639,sizeof(char),2))),_tag_dyneither(_tmp638,sizeof(void*),0)));}
goto _LL1F9;_LL1F9:;}
# 1673
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp63C;void*_tmp63B;(_tmp63B=0,Cyc_fprintf(Cyc_stderr,((_tmp63C="    ",_tag_dyneither(_tmp63C,sizeof(char),5))),_tag_dyneither(_tmp63B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp63F;void*_tmp63E;(_tmp63E=0,Cyc_fprintf(Cyc_stderr,((_tmp63F=" --> ",_tag_dyneither(_tmp63F,sizeof(char),6))),_tag_dyneither(_tmp63E,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp642;void*_tmp641;(_tmp641=0,Cyc_fprintf(Cyc_stderr,((_tmp642="\n",_tag_dyneither(_tmp642,sizeof(char),2))),_tag_dyneither(_tmp641,sizeof(void*),0)));};}
# 1681
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1685
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp3B6=f;struct Cyc_Dict_Dict _tmp3B7;_LL20D: if((_tmp3B6.BottomFL).tag != 1)goto _LL20F;_LL20E:
{const char*_tmp645;void*_tmp644;(_tmp644=0,Cyc_fprintf(Cyc_stderr,((_tmp645="  BottomFL\n",_tag_dyneither(_tmp645,sizeof(char),12))),_tag_dyneither(_tmp644,sizeof(void*),0)));}goto _LL20C;_LL20F: if((_tmp3B6.ReachableFL).tag != 2)goto _LL20C;_tmp3B7=((struct _tuple11)(_tmp3B6.ReachableFL).val).f1;_LL210:
# 1689
{const char*_tmp648;void*_tmp647;(_tmp647=0,Cyc_fprintf(Cyc_stderr,((_tmp648="  ReachableFL:\n",_tag_dyneither(_tmp648,sizeof(char),16))),_tag_dyneither(_tmp647,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp3B7);
{const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_fprintf(Cyc_stderr,((_tmp64B="\n",_tag_dyneither(_tmp64B,sizeof(char),2))),_tag_dyneither(_tmp64A,sizeof(void*),0)));}
goto _LL20C;_LL20C:;}
# 1704 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp3BE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3CB;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*_tmp3CF;struct Cyc_List_List*_tmp3D1;struct Cyc_List_List*_tmp3D3;struct Cyc_List_List*_tmp3D5;void*_tmp3D7;void*_tmp3D8;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DD;void*_tmp3DF;void*_tmp3E1;void*_tmp3E3;void*_tmp3E5;void*_tmp3E6;void*_tmp3E8;_LL212: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3BE;if(_tmp3BF->tag != 26)goto _LL214;}_LL213:
 goto _LL215;_LL214: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C0->tag != 0)goto _LL216;}_LL215:
 goto _LL217;_LL216: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C1->tag != 6)goto _LL218;}_LL217:
 goto _LL219;_LL218: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C2->tag != 7)goto _LL21A;}_LL219:
 goto _LL21B;_LL21A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C3->tag != 13)goto _LL21C;}_LL21B:
 goto _LL21D;_LL21C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C4->tag != 14)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C5->tag != 19)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3C6=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3BE;if(_tmp3C6->tag != 20)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3BE;if(_tmp3C7->tag != 1)goto _LL224;}_LL223:
 return 0;_LL224: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3C8=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3BE;if(_tmp3C8->tag != 21)goto _LL226;}_LL225: {
# 1716
void*_tmp3EA=rgn;_LL249: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3EB=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EA;if(_tmp3EB->tag != 0)goto _LL24B;}_LL24A:
 return 1;_LL24B:;_LL24C:
 return 0;_LL248:;}_LL226: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3C9=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3BE;if(_tmp3C9->tag != 22)goto _LL228;}_LL227: {
# 1721
void*_tmp3EC=rgn;_LL24E: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3ED=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EC;if(_tmp3ED->tag != 1)goto _LL250;}_LL24F:
 return 1;_LL250:;_LL251:
 return 0;_LL24D:;}_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3BE;if(_tmp3CA->tag != 2)goto _LL22A;else{_tmp3CB=_tmp3CA->f1;}}_LL229: {
# 1726
void*_tmp3EE=rgn;struct Cyc_Absyn_Tvar*_tmp3F2;_LL253: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp3EF=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EE;if(_tmp3EF->tag != 1)goto _LL255;}_LL254:
 goto _LL256;_LL255: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp3F0=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp3EE;if(_tmp3F0->tag != 0)goto _LL257;}_LL256:
 return 0;_LL257: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp3F1=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp3EE;if(_tmp3F1->tag != 2)goto _LL252;else{_tmp3F2=_tmp3F1->f1;}}_LL258:
 return Cyc_Absyn_tvar_cmp(_tmp3CB,_tmp3F2)== 0;_LL252:;}_LL22A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3CC=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3BE;if(_tmp3CC->tag != 3)goto _LL22C;else{_tmp3CD=(_tmp3CC->f1).targs;}}_LL22B:
# 1739 "cf_flowinfo.cyc"
 _tmp3CF=_tmp3CD;goto _LL22D;_LL22C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3CE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3BE;if(_tmp3CE->tag != 17)goto _LL22E;else{_tmp3CF=_tmp3CE->f2;}}_LL22D:
 _tmp3D1=_tmp3CF;goto _LL22F;_LL22E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BE;if(_tmp3D0->tag != 11)goto _LL230;else{_tmp3D1=(_tmp3D0->f1).targs;}}_LL22F:
 _tmp3D3=_tmp3D1;goto _LL231;_LL230: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3BE;if(_tmp3D2->tag != 4)goto _LL232;else{_tmp3D3=(_tmp3D2->f1).targs;}}_LL231:
 _tmp3D5=_tmp3D3;goto _LL233;_LL232: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3D4->tag != 24)goto _LL234;else{_tmp3D5=_tmp3D4->f1;}}_LL233:
# 1744
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp3D5);_LL234: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE;if(_tmp3D6->tag != 5)goto _LL236;else{_tmp3D7=(_tmp3D6->f1).elt_typ;_tmp3D8=((_tmp3D6->f1).ptr_atts).rgn;}}_LL235:
# 1746
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3D8) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3D7);_LL236: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3BE;if(_tmp3D9->tag != 9)goto _LL238;}_LL237:
 return 0;_LL238: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3DA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3BE;if(_tmp3DA->tag != 10)goto _LL23A;else{_tmp3DB=_tmp3DA->f1;}}_LL239:
# 1749
 for(0;_tmp3DB != 0;_tmp3DB=_tmp3DB->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple16*)_tmp3DB->hd)).f2))return 1;}
return 0;_LL23A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3DC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3BE;if(_tmp3DC->tag != 12)goto _LL23C;else{_tmp3DD=_tmp3DC->f2;}}_LL23B:
# 1753
 for(0;_tmp3DD != 0;_tmp3DD=_tmp3DD->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp3DD->hd)->type))return 1;}
return 0;_LL23C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE;if(_tmp3DE->tag != 8)goto _LL23E;else{_tmp3DF=(_tmp3DE->f1).elt_type;}}_LL23D:
 _tmp3E1=_tmp3DF;goto _LL23F;_LL23E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3E0->tag != 23)goto _LL240;else{_tmp3E1=(void*)_tmp3E0->f1;}}_LL23F:
 _tmp3E3=_tmp3E1;goto _LL241;_LL240: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3E2->tag != 25)goto _LL242;else{_tmp3E3=(void*)_tmp3E2->f1;}}_LL241:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3E3);_LL242: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3BE;if(_tmp3E4->tag != 16)goto _LL244;else{_tmp3E5=(void*)_tmp3E4->f1;_tmp3E6=(void*)_tmp3E4->f2;}}_LL243:
# 1760
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp3E5) || Cyc_CfFlowInfo_contains_region(rgn,_tmp3E6);_LL244: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3BE;if(_tmp3E7->tag != 15)goto _LL246;else{_tmp3E8=(void*)_tmp3E7->f1;}}_LL245:
# 1763
 return 0;_LL246: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3BE;if(_tmp3E9->tag != 18)goto _LL211;}_LL247:
 return 0;_LL211:;}struct _tuple24{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1769
static void Cyc_CfFlowInfo_kill_root(struct _tuple24*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F4;struct Cyc_Dict_Dict*_tmp3F5;void*_tmp3F6;struct _tuple24*_tmp3F3=env;_tmp3F4=_tmp3F3->f1;_tmp3F5=(struct Cyc_Dict_Dict*)& _tmp3F3->f2;_tmp3F6=_tmp3F3->f3;{
void*_tmp3F7=root;struct Cyc_Absyn_Vardecl*_tmp3F9;void*_tmp3FB;_LL25A: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3F8=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3F7;if(_tmp3F8->tag != 0)goto _LL25C;else{_tmp3F9=_tmp3F8->f1;}}_LL25B:
# 1774
 if(Cyc_CfFlowInfo_contains_region(_tmp3F6,_tmp3F9->type))
# 1776
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3F4,_tmp3F9->type,0,_tmp3F4->unknown_none);
*_tmp3F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3F5,root,rval);
goto _LL259;_LL25C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3FA=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp3F7;if(_tmp3FA->tag != 1)goto _LL25E;else{_tmp3FB=(void*)_tmp3FA->f2;}}_LL25D:
# 1781
 if(!Cyc_CfFlowInfo_contains_region(_tmp3F6,_tmp3FB))
# 1783
*_tmp3F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp3F5,root,rval);
goto _LL259;_LL25E: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3FC=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp3F7;if(_tmp3FC->tag != 2)goto _LL259;}_LL25F:
# 1787
 goto _LL259;_LL259:;};}
# 1792
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1794
void*_tmp3FD=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp3FF;_LL261: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3FD;if(_tmp3FE->tag != 2)goto _LL263;else{_tmp3FF=_tmp3FE->f1;}}_LL262: {
# 1796
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp64C;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp64C.tag=2,((_tmp64C.f1=_tmp3FF,_tmp64C)));
void*v2=(void*)& v;
struct _tuple24 _tmp64D;struct _tuple24 env=(_tmp64D.f1=fenv,((_tmp64D.f2=fenv->mt_flowdict,((_tmp64D.f3=v2,_tmp64D)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL263: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3FD;if(_tmp400->tag != 21)goto _LL265;}_LL264: {
# 1803
struct _tuple24 _tmp64E;struct _tuple24 env=(_tmp64E.f1=fenv,((_tmp64E.f2=fenv->mt_flowdict,((_tmp64E.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp64E)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL265: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3FD;if(_tmp401->tag != 22)goto _LL267;}_LL266: {
# 1808
struct _tuple24 _tmp64F;struct _tuple24 env=(_tmp64F.f1=fenv,((_tmp64F.f2=fenv->mt_flowdict,((_tmp64F.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp64F)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL267:;_LL268: {
# 1812
struct Cyc_Core_Impossible_exn_struct _tmp655;const char*_tmp654;struct Cyc_Core_Impossible_exn_struct*_tmp653;(int)_throw((void*)((_tmp653=_cycalloc(sizeof(*_tmp653)),((_tmp653[0]=((_tmp655.tag=Cyc_Core_Impossible,((_tmp655.f1=((_tmp654="kill_flowdict_region",_tag_dyneither(_tmp654,sizeof(char),21))),_tmp655)))),_tmp653)))));}_LL260:;}
# 1817
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1819
return 0;}
# 1824
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp409=f;struct Cyc_Dict_Dict _tmp40A;struct Cyc_List_List*_tmp40B;_LL26A: if((_tmp409.BottomFL).tag != 1)goto _LL26C;_LL26B:
 return f;_LL26C: if((_tmp409.ReachableFL).tag != 2)goto _LL269;_tmp40A=((struct _tuple11)(_tmp409.ReachableFL).val).f1;_tmp40B=((struct _tuple11)(_tmp409.ReachableFL).val).f2;_LL26D: {
# 1828
struct Cyc_Dict_Dict _tmp40C=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp40A,rgn);
struct Cyc_List_List*_tmp40D=Cyc_CfFlowInfo_kill_relns_region(_tmp40B,rgn);
# 1831
return Cyc_CfFlowInfo_ReachableFL(_tmp40C,_tmp40D);}_LL269:;}
