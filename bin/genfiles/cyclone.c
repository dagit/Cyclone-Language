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
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
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
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 228 "core.h"
void Cyc_Core_rethrow(void*);
# 232
const char*Cyc_Core_get_exn_name(void*);
# 234
const char*Cyc_Core_get_exn_filename();
# 237
int Cyc_Core_get_exn_lineno();
# 246
struct _dyneither_ptr Cstring_to_string(char*);
# 256
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();
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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1177 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1179
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_FlatList{struct Cyc_FlatList*tl;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd;};
typedef struct _tuple10*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 44 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 55
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 74
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 78
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 89
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 118 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 227
extern int Cyc_Tcenv_warn_override;
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 120
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 127
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 162
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 50
int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;
# 60
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
# 91
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3C9;struct Cyc_List_List*_tmp3C8;Cyc_ccargs=((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8->hd=((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9[0]=s,_tmp3C9)))),((_tmp3C8->tl=Cyc_ccargs,_tmp3C8))))));}
# 99
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3CA;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3CA="vc",_tag_dyneither(_tmp3CA,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3CB;Cyc_add_ccarg(((_tmp3CB="-DVC_C",_tag_dyneither(_tmp3CB,sizeof(char),7))));};}else{
# 104
const char*_tmp3CC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3CC="gcc",_tag_dyneither(_tmp3CC,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3CD;Cyc_add_ccarg(((_tmp3CD="-DGCC_C",_tag_dyneither(_tmp3CD,sizeof(char),8))));};}}}
# 112
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 120
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3CE;Cyc_output_file=((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=s,_tmp3CE))));}
# 125
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 134
extern char*Cdef_inc_path;
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Ccomp;
extern char*Cversion;static char _tmp7[1]="";
# 141
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 146
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D1;struct Cyc_List_List*_tmp3D0;Cyc_cppargs=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->hd=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=s,_tmp3D1)))),((_tmp3D0->tl=Cyc_cppargs,_tmp3D0))))));}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D4[0]=& _tmp3D3,Cyc_aprintf(((_tmp3D5="-iprefix %s",_tag_dyneither(_tmp3D5,sizeof(char),12))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))))));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3D9;void*_tmp3D8[1];struct Cyc_String_pa_PrintArg_struct _tmp3D7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D8[0]=& _tmp3D7,Cyc_aprintf(((_tmp3D9="-iwithprefix %s",_tag_dyneither(_tmp3D9,sizeof(char),16))),_tag_dyneither(_tmp3D8,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3DD;void*_tmp3DC[1];struct Cyc_String_pa_PrintArg_struct _tmp3DB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DC[0]=& _tmp3DB,Cyc_aprintf(((_tmp3DD="-iwithprefixbefore %s",_tag_dyneither(_tmp3DD,sizeof(char),22))),_tag_dyneither(_tmp3DC,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_String_pa_PrintArg_struct _tmp3DF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E0[0]=& _tmp3DF,Cyc_aprintf(((_tmp3E1="-isystem %s",_tag_dyneither(_tmp3E1,sizeof(char),12))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_String_pa_PrintArg_struct _tmp3E3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E4[0]=& _tmp3E3,Cyc_aprintf(((_tmp3E5="-idirafter %s",_tag_dyneither(_tmp3E5,sizeof(char),14))),_tag_dyneither(_tmp3E4,sizeof(void*),1)))))))));}
# 167
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
Cyc_toc_r=1;
Cyc_target_arch=s;}
# 173
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 178
static void Cyc_print_version(){
{const char*_tmp3E9;void*_tmp3E8[1];struct Cyc_String_pa_PrintArg_struct _tmp3E7;(_tmp3E7.tag=0,((_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3E8[0]=& _tmp3E7,Cyc_printf(((_tmp3E9="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3E9,sizeof(char),34))),_tag_dyneither(_tmp3E8,sizeof(void*),1)))))));}
{const char*_tmp3EE;void*_tmp3ED[2];struct Cyc_String_pa_PrintArg_struct _tmp3EC;struct Cyc_String_pa_PrintArg_struct _tmp3EB;(_tmp3EB.tag=0,((_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3EC.tag=0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3ED[0]=& _tmp3EC,((_tmp3ED[1]=& _tmp3EB,Cyc_printf(((_tmp3EE="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3EE,sizeof(char),39))),_tag_dyneither(_tmp3ED,sizeof(void*),2)))))))))))));}
{const char*_tmp3F2;void*_tmp3F1[1];struct Cyc_String_pa_PrintArg_struct _tmp3F0;(_tmp3F0.tag=0,((_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3F1[0]=& _tmp3F0,Cyc_printf(((_tmp3F2="Standard library directory: %s\n",_tag_dyneither(_tmp3F2,sizeof(char),32))),_tag_dyneither(_tmp3F1,sizeof(void*),1)))))));}
{const char*_tmp3F6;void*_tmp3F5[1];struct Cyc_String_pa_PrintArg_struct _tmp3F4;(_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3F5[0]=& _tmp3F4,Cyc_printf(((_tmp3F6="Standard include directory: %s\n",_tag_dyneither(_tmp3F6,sizeof(char),32))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}
exit(0);}
# 187
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp26 <= 4)return 0;else{
const char*_tmp3F7;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp3F7=".cyc",_tag_dyneither(_tmp3F7,sizeof(char),5))))== 0;}}
# 193
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{
struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp28 - 2);
struct _dyneither_ptr*_tmp3FA;struct Cyc_List_List*_tmp3F9;Cyc_cyclone_exec_path=((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->hd=((_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA[0]=_tmp29,_tmp3FA)))),((_tmp3F9->tl=Cyc_cyclone_exec_path,_tmp3F9))))));};}
# 206
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp3FB;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3FB="-lxml",_tag_dyneither(_tmp3FB,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 212
const char*_tmp3FC;Cyc_add_ccarg(((_tmp3FC="-lxml_a",_tag_dyneither(_tmp3FC,sizeof(char),8))));}}else{
# 214
struct _dyneither_ptr*_tmp3FF;struct Cyc_List_List*_tmp3FE;Cyc_libargs=((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE->hd=((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF[0]=s,_tmp3FF)))),((_tmp3FE->tl=Cyc_libargs,_tmp3FE))))));}}
# 217
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 221
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp400;Cyc_add_ccarg(((_tmp400="-save-temps",_tag_dyneither(_tmp400,sizeof(char),12))));};}
# 226
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp401;Cyc_add_cpparg(((_tmp401="-M",_tag_dyneither(_tmp401,sizeof(char),3))));};}
# 233
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp402;Cyc_dependencies_target=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=s,_tmp402))));}
# 238
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp403;Cyc_add_ccarg(((_tmp403="-c",_tag_dyneither(_tmp403,sizeof(char),3))));};}
# 243
static void Cyc_set_nocppprecomp(){
{const char*_tmp404;Cyc_add_cpparg(((_tmp404="-no-cpp-precomp",_tag_dyneither(_tmp404,sizeof(char),16))));}{
const char*_tmp405;Cyc_add_ccarg(((_tmp405="-no-cpp-precomp",_tag_dyneither(_tmp405,sizeof(char),16))));};}
# 248
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 252
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 255
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 259
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
{const char*_tmp406;Cyc_add_ccarg(((_tmp406="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp406,sizeof(char),18))));}
Cyc_set_elim_se();}
# 264
static void Cyc_set_noboundschecks(){
const char*_tmp407;Cyc_add_ccarg(((_tmp407="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp407,sizeof(char),23))));}
# 267
static void Cyc_set_nonullchecks(){
const char*_tmp408;Cyc_add_ccarg(((_tmp408="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp408,sizeof(char),21))));}
# 270
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 276
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp409;Cyc_add_ccarg(((_tmp409="-DNO_CYC_PREFIX",_tag_dyneither(_tmp409,sizeof(char),16))));};}
# 281
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp40A;Cyc_add_ccarg(((_tmp40A="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp40A,sizeof(char),21))));}{
const char*_tmp40B;Cyc_add_cpparg(((_tmp40B="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp40B,sizeof(char),21))));};}
# 287
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp40C;Cyc_add_ccarg(((_tmp40C="-pg",_tag_dyneither(_tmp40C,sizeof(char),4))));};}
# 292
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp40D;Cyc_add_ccarg(((_tmp40D="-S",_tag_dyneither(_tmp40D,sizeof(char),3))));};}
# 297
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;}
# 306
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 310
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp40E;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp40E="cyc",_tag_dyneither(_tmp40E,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp40F;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp40F="none",_tag_dyneither(_tmp40F,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 318
const char*_tmp413;void*_tmp412[1];struct Cyc_String_pa_PrintArg_struct _tmp411;(_tmp411.tag=0,((_tmp411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp412[0]=& _tmp411,Cyc_fprintf(Cyc_stderr,((_tmp413="Input type '%s' not supported\n",_tag_dyneither(_tmp413,sizeof(char),31))),_tag_dyneither(_tmp412,sizeof(void*),1)))))));}}}
# 322
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 328
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
return(struct _dyneither_ptr)_tmp45;}
# 334
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 336
static void Cyc_add_other(struct _dyneither_ptr s){
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 339
{struct _dyneither_ptr*_tmp416;struct Cyc_List_List*_tmp415;Cyc_cyclone_files=((_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415->hd=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=s,_tmp416)))),((_tmp415->tl=Cyc_cyclone_files,_tmp415))))));}{
# 342
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp417;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp417=".c",_tag_dyneither(_tmp417,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 346
Cyc_add_ccarg(s);}}
# 349
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 360
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4B=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 368
if(spec_file == 0){
{const char*_tmp41B;void*_tmp41A[1];struct Cyc_String_pa_PrintArg_struct _tmp419;(_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp41A[0]=& _tmp419,Cyc_fprintf(Cyc_stderr,((_tmp41B="Error opening spec file %s\n",_tag_dyneither(_tmp41B,sizeof(char),28))),_tag_dyneither(_tmp41A,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 374
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp420;void*_tmp41F[2];struct Cyc_String_pa_PrintArg_struct _tmp41E;struct Cyc_Int_pa_PrintArg_struct _tmp41D;(_tmp41D.tag=1,((_tmp41D.f1=(unsigned long)c,((_tmp41E.tag=0,((_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp41F[0]=& _tmp41E,((_tmp41F[1]=& _tmp41D,Cyc_fprintf(Cyc_stderr,((_tmp420="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp420,sizeof(char),55))),_tag_dyneither(_tmp41F,sizeof(void*),2)))))))))))));}
# 383
Cyc_fflush(Cyc_stderr);}
# 385
goto CLEANUP_AND_RETURN;}
# 387
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp424;void*_tmp423[1];struct Cyc_String_pa_PrintArg_struct _tmp422;(_tmp422.tag=0,((_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp423[0]=& _tmp422,Cyc_fprintf(Cyc_stderr,((_tmp424="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp424,sizeof(char),44))),_tag_dyneither(_tmp423,sizeof(void*),1)))))));}
# 395
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 398
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 402
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp428;void*_tmp427[1];struct Cyc_String_pa_PrintArg_struct _tmp426;(_tmp426.tag=0,((_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp427[0]=& _tmp426,Cyc_fprintf(Cyc_stderr,((_tmp428="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp428,sizeof(char),50))),_tag_dyneither(_tmp427,sizeof(void*),1)))))));}
# 408
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 412
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 417
if(c == '*'){
# 419
{struct _tuple21*_tmp435;struct _dyneither_ptr*_tmp434;const char*_tmp433;struct _dyneither_ptr*_tmp432;struct Cyc_List_List*_tmp431;_tmp4B=((_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431->hd=((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435->f1=((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp434)))),((_tmp435->f2=(
(_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp433="",_tag_dyneither(_tmp433,sizeof(char),1)))),_tmp432)))),_tmp435)))))),((_tmp431->tl=_tmp4B,_tmp431))))));}
goto JUST_AFTER_STAR;}
# 424
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp43A;void*_tmp439[2];struct Cyc_String_pa_PrintArg_struct _tmp438;struct Cyc_String_pa_PrintArg_struct _tmp437;(_tmp437.tag=0,((_tmp437.f1=(struct _dyneither_ptr)
# 434
_tag_dyneither(strname,sizeof(char),256),((_tmp438.tag=0,((_tmp438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp439[0]=& _tmp438,((_tmp439[1]=& _tmp437,Cyc_fprintf(Cyc_stderr,((_tmp43A="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp43A,sizeof(char),50))),_tag_dyneither(_tmp439,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 440
{struct _tuple21*_tmp443;struct _dyneither_ptr*_tmp442;struct _dyneither_ptr*_tmp441;struct Cyc_List_List*_tmp440;_tmp4B=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->hd=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->f1=((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp442)))),((_tmp443->f2=(
(_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp441)))),_tmp443)))))),((_tmp440->tl=_tmp4B,_tmp440))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 445
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp44D(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp44C,unsigned int*_tmp44B,struct _dyneither_ptr**_tmp449){for(*_tmp44C=0;*_tmp44C < *_tmp44B;(*_tmp44C)++){(*_tmp449)[*_tmp44C]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp44C];}}
# 451
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp67=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp66 > 4096)goto DONE;
while(1){
# 461
while(1){
if(i >= _tmp66)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 467
j=0;
# 472
while(1){
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 478
++ i;
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 485
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 488
++ i;}
# 490
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 493
struct _dyneither_ptr*_tmp446;struct Cyc_List_List*_tmp445;_tmp67=((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445->hd=((_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp446)))),((_tmp445->tl=_tmp67,_tmp445))))));};}
# 495
DONE:
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
{struct Cyc_List_List*_tmp447;_tmp67=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp447->tl=_tmp67,_tmp447))))));}{
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
unsigned int _tmp44C;unsigned int _tmp44B;struct _dyneither_ptr _tmp44A;struct _dyneither_ptr*_tmp449;unsigned int _tmp448;struct _dyneither_ptr _tmp6D=(_tmp448=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp449=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp448)),((_tmp44A=_tag_dyneither(_tmp449,sizeof(struct _dyneither_ptr),_tmp448),((((_tmp44B=_tmp448,_tmp44D(& _tmp6C,& _tmp44C,& _tmp44B,& _tmp449))),_tmp44A)))))));
return _tmp6D;};};}
# 505
int Cyc_compile_failure=0;
# 507
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;void*_tmp78;_LL1:;_LL2:
# 511
 Cyc_compile_failure=1;
{const char*_tmp452;void*_tmp451[2];struct Cyc_String_pa_PrintArg_struct _tmp450;struct Cyc_String_pa_PrintArg_struct _tmp44F;(_tmp44F.tag=0,((_tmp44F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp451[0]=& _tmp450,((_tmp451[1]=& _tmp44F,Cyc_fprintf(Cyc_stderr,((_tmp452="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp452,sizeof(char),29))),_tag_dyneither(_tmp451,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp78=_tmp77;_LL4:(void)_throw(_tmp78);_LL0:;}};}
# 518
void CYCALLOCPROFILE_mark(const char*s);
# 520
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 524
const char*_tmp453;struct _dyneither_ptr explain_string=(_tmp453="",_tag_dyneither(_tmp453,sizeof(char),1));
struct Cyc_List_List*_tmp7D=0;
# 527
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 529
{struct _handler_cons _tmp7E;_push_handler(& _tmp7E);{int _tmp80=0;if(setjmp(_tmp7E.handler))_tmp80=1;if(!_tmp80){_tmp7D=f(env,tds);;_pop_handler();}else{void*_tmp7F=(void*)_exn_thrown;void*_tmp82=_tmp7F;void*_tmp83;void*_tmp84;_LL6: _tmp83=_tmp82;_LL7:
# 532
{const char*_tmp457;void*_tmp456[1];struct Cyc_String_pa_PrintArg_struct _tmp455;(_tmp455.tag=0,((_tmp455.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp456[0]=& _tmp455,Cyc_fprintf(Cyc_stderr,((_tmp457="COMPILER STAGE %s\n",_tag_dyneither(_tmp457,sizeof(char),19))),_tag_dyneither(_tmp456,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp83);_LL8: _tmp84=_tmp82;_LL9:(void)_throw(_tmp84);_LL5:;}};}
# 535
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp45A;void*_tmp459;(_tmp459=0,Cyc_fprintf(Cyc_stderr,((_tmp45A="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp45A,sizeof(char),22))),_tag_dyneither(_tmp459,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}else{
# 542
if(Cyc_v_r){
{const char*_tmp45E;void*_tmp45D[1];struct Cyc_String_pa_PrintArg_struct _tmp45C;(_tmp45C.tag=0,((_tmp45C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp45D[0]=& _tmp45C,Cyc_fprintf(Cyc_stderr,((_tmp45E="%s completed.\n",_tag_dyneither(_tmp45E,sizeof(char),15))),_tag_dyneither(_tmp45D,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}}
# 547
return _tmp7D;}
# 550
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp8E;_push_handler(& _tmp8E);{int _tmp90=0;if(setjmp(_tmp8E.handler))_tmp90=1;if(!_tmp90){
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp8F=(void*)_exn_thrown;void*_tmp92=_tmp8F;void*_tmp94;_LLB: {struct Cyc_Position_Exit_exn_struct*_tmp93=(struct Cyc_Position_Exit_exn_struct*)_tmp92;if(_tmp93->tag != Cyc_Position_Exit)goto _LLD;}_LLC:
# 557
 Cyc_compile_failure=1;goto _LLA;_LLD: _tmp94=_tmp92;_LLE:(void)_throw(_tmp94);_LLA:;}};}
# 559
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 563
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 565
struct _tuple22 _tmp95=*tcenv;struct _RegionHandle*_tmp97;struct Cyc_Tcenv_Tenv*_tmp98;struct _tuple22 _tmp96=_tmp95;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
Cyc_Tc_tc(_tmp97,_tmp98,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp98,tds);
return tds;}
# 572
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 579
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmp9A;struct Cyc___cycFILE*_tmp9B;struct Cyc___cycFILE*_tmp9C;struct _tuple23*_tmp99=params;_tmp9A=_tmp99->f1;_tmp9B=_tmp99->f2;_tmp9C=_tmp99->f3;{
struct Cyc_Interface_I*_tmp9D=Cyc_Interface_extract(_tmp9A->ae);
if(_tmp9B == 0)
Cyc_Interface_save(_tmp9D,_tmp9C);else{
# 585
struct Cyc_Interface_I*_tmp9E=Cyc_Interface_parse(_tmp9B);
const char*_tmp463;const char*_tmp462;struct _tuple20*_tmp461;if(!Cyc_Interface_is_subinterface(_tmp9E,_tmp9D,((_tmp461=_cycalloc(sizeof(*_tmp461)),((_tmp461->f1=((_tmp462="written interface",_tag_dyneither(_tmp462,sizeof(char),18))),((_tmp461->f2=((_tmp463="maximal interface",_tag_dyneither(_tmp463,sizeof(char),18))),_tmp461))))))))
Cyc_compile_failure=1;else{
# 589
Cyc_Interface_save(_tmp9E,_tmp9C);}}
# 591
return tds;};}
# 594
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 597
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 602
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 604
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 606
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 611
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 614
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp466;void*_tmp465;(_tmp465=0,Cyc_fprintf(Cyc_stderr,((_tmp466="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp466,sizeof(char),53))),_tag_dyneither(_tmp465,sizeof(void*),0)));}
return 1;}{
# 618
const char*_tmp468;const char*_tmp467;struct Cyc___cycFILE*_tmpA4=Cyc_try_file_open(file,((_tmp467="r",_tag_dyneither(_tmp467,sizeof(char),2))),((_tmp468="internal compiler file",_tag_dyneither(_tmp468,sizeof(char),23))));
if(_tmpA4 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpAB=1024;unsigned int i;for(i=0;i < _tmpAB;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpA4);
if(n_read != 1024  && !Cyc_feof(_tmpA4)){
Cyc_compile_failure=1;
{const char*_tmp46C;void*_tmp46B[1];struct Cyc_String_pa_PrintArg_struct _tmp46A;(_tmp46A.tag=0,((_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp46B[0]=& _tmp46A,Cyc_fprintf(Cyc_stderr,((_tmp46C="\nError: problem copying %s\n",_tag_dyneither(_tmp46C,sizeof(char),28))),_tag_dyneither(_tmp46B,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 631
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp470;void*_tmp46F[1];struct Cyc_String_pa_PrintArg_struct _tmp46E;(_tmp46E.tag=0,((_tmp46E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp46F[0]=& _tmp46E,Cyc_fprintf(Cyc_stderr,((_tmp470="\nError: problem copying %s\n",_tag_dyneither(_tmp470,sizeof(char),28))),_tag_dyneither(_tmp46F,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 639
Cyc_fclose(_tmpA4);
return 0;};};}
# 644
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpAE=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpAE.expand_typedefs=!Cyc_noexpand_r;
_tmpAE.to_VC=Cyc_Cyclone_tovc_r;
_tmpAE.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpAE.generate_line_directives=Cyc_generate_line_directives_r;
_tmpAE.print_full_evars=Cyc_print_full_evars_r;
_tmpAE.print_all_tvars=Cyc_print_all_tvars_r;
_tmpAE.print_all_kinds=Cyc_print_all_kinds_r;
_tmpAE.print_all_effects=Cyc_print_all_effects_r;
# 655
if(Cyc_inline_functions_r){
const char*_tmp473;void*_tmp472;(_tmp472=0,Cyc_fprintf(out_file,((_tmp473="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp473,sizeof(char),27))),_tag_dyneither(_tmp472,sizeof(void*),0)));}
# 659
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp476;void*_tmp475;(_tmp475=0,Cyc_fprintf(out_file,((_tmp476="#include <setjmp.h>\n",_tag_dyneither(_tmp476,sizeof(char),21))),_tag_dyneither(_tmp475,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 665
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 667
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpAE);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 671
Cyc_Absyndump_set_params(& _tmpAE);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 674
Cyc_fflush(out_file);
return tds;}
# 680
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 688
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpB3=0;
unsigned long _tmpB4=Cyc_strlen((struct _dyneither_ptr)s);
# 693
while(_tmpB4 > 0){
struct _dyneither_ptr _tmpB5=Cyc_strchr(s,c);
if((char*)_tmpB5.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp479;struct Cyc_List_List*_tmp478;_tmpB3=((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478->hd=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479[0]=s,_tmp479)))),((_tmp478->tl=_tmpB3,_tmp478))))));}
break;}else{
# 700
{struct _dyneither_ptr*_tmp47C;struct Cyc_List_List*_tmp47B;_tmpB3=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->hd=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpB5).curr - s.curr)/ sizeof(char))),_tmp47C)))),((_tmp47B->tl=_tmpB3,_tmp47B))))));}
_tmpB4 -=(((struct _dyneither_ptr)_tmpB5).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpB5,sizeof(char),1);}}
# 705
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB3);};}
# 709
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpBA;_push_handler(& _tmpBA);{int _tmpBC=0;if(setjmp(_tmpBA.handler))_tmpBC=1;if(!_tmpBC){{const char*_tmp47D;f=Cyc_file_open(file,((_tmp47D="r",_tag_dyneither(_tmp47D,sizeof(char),2))));};_pop_handler();}else{void*_tmpBB=(void*)_exn_thrown;void*_tmpBF=_tmpBB;void*_tmpC0;_LL10:;_LL11: goto _LLF;_LL12: _tmpC0=_tmpBF;_LL13:(void)_throw(_tmpC0);_LLF:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 717
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp47E;struct _dyneither_ptr tmp=(_tmp47E="",_tag_dyneither(_tmp47E,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC1=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC1)== 0)continue;
{const char*_tmp47F;_tmpC1=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC1,((_tmp47F=":",_tag_dyneither(_tmp47F,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC1,(struct _dyneither_ptr)tmp);}
# 725
return tmp;}
# 730
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC4=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC4.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC4)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpC4,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp480;return(_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480[0]=s,_tmp480)));}};}
# 738
return 0;}
# 741
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpC6=Cyc_find(dirs,file);
if(_tmpC6 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp485;void*_tmp484[2];struct Cyc_String_pa_PrintArg_struct _tmp483;struct Cyc_String_pa_PrintArg_struct _tmp482;(_tmp482.tag=0,((_tmp482.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp483.tag=0,((_tmp483.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp484[0]=& _tmp483,((_tmp484[1]=& _tmp482,Cyc_fprintf(Cyc_stderr,((_tmp485="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp485,sizeof(char),56))),_tag_dyneither(_tmp484,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp495;const char*_tmp494;void*_tmp493[2];struct Cyc_String_pa_PrintArg_struct _tmp492;struct Cyc_String_pa_PrintArg_struct _tmp491;struct Cyc_Core_Failure_exn_struct*_tmp490;(int)_throw((void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((_tmp495.tag=Cyc_Core_Failure,((_tmp495.f1=(struct _dyneither_ptr)((_tmp491.tag=0,((_tmp491.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp492.tag=0,((_tmp492.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp493[0]=& _tmp492,((_tmp493[1]=& _tmp491,Cyc_aprintf(((_tmp494="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp494,sizeof(char),56))),_tag_dyneither(_tmp493,sizeof(void*),2)))))))))))))),_tmp495)))),_tmp490)))));};}
# 753
return*_tmpC6;}
# 758
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 760
struct Cyc_List_List*_tmpD1=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp498;struct Cyc_List_List*_tmp497;_tmpD1=((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497->hd=((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp498)))),((_tmp497->tl=_tmpD1,_tmp497))))));}
# 764
_tmpD1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD1);
return _tmpD1;}
# 770
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 772
struct Cyc_List_List*_tmpD4=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp499;_tmpD4=((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp499->tl=_tmpD4,_tmp499))))));}{
struct _dyneither_ptr*_tmp49C;struct Cyc_List_List*_tmp49B;_tmpD4=((_tmp49B=_cycalloc(sizeof(*_tmp49B)),((_tmp49B->hd=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp49C)))),((_tmp49B->tl=_tmpD4,_tmp49B))))));};}
# 777
_tmpD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);
return _tmpD4;}
# 782
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL14:
 goto _LL15;case '"': _LL15:
 goto _LL16;case ';': _LL16:
 goto _LL17;case '&': _LL17:
 goto _LL18;case '(': _LL18:
 goto _LL19;case ')': _LL19:
 goto _LL1A;case '|': _LL1A:
 goto _LL1B;case '^': _LL1B:
 goto _LL1C;case '<': _LL1C:
 goto _LL1D;case '>': _LL1D:
 goto _LL1E;case '\n': _LL1E:
# 797
 goto _LL1F;case '\t': _LL1F:
 return 1;default: _LL20:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4A5(unsigned int*_tmp4A4,unsigned int*_tmp4A3,char**_tmp4A1){for(*_tmp4A4=0;*_tmp4A4 < *_tmp4A3;(*_tmp4A4)++){(*_tmp4A1)[*_tmp4A4]=(char)'\000';}}
# 804
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpD8=Cyc_strlen((struct _dyneither_ptr)s);
# 808
int _tmpD9=0;
int _tmpDA=0;
{int i=0;for(0;i < _tmpD8;++ i){
char _tmpDB=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpDB == '\'')++ _tmpD9;else{
if(Cyc_is_other_special(_tmpDB))++ _tmpDA;}}}
# 817
if(_tmpD9 == 0  && _tmpDA == 0)
return s;
# 821
if(_tmpD9 == 0){
struct _dyneither_ptr*_tmp49F;struct _dyneither_ptr*_tmp49E[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp49E[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp49E[1]=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=(struct _dyneither_ptr)s,_tmp49F)))),((_tmp49E[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49E,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 825
unsigned long _tmpE0=(_tmpD8 + _tmpD9)+ _tmpDA;
unsigned int _tmp4A4;unsigned int _tmp4A3;struct _dyneither_ptr _tmp4A2;char*_tmp4A1;unsigned int _tmp4A0;struct _dyneither_ptr s2=(_tmp4A0=_tmpE0 + 1,((_tmp4A1=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4A0 + 1)),((_tmp4A2=_tag_dyneither(_tmp4A1,sizeof(char),_tmp4A0 + 1),((((_tmp4A3=_tmp4A0,((_tmp4A5(& _tmp4A4,& _tmp4A3,& _tmp4A1),_tmp4A1[_tmp4A3]=(char)0)))),_tmp4A2)))))));
int _tmpE1=0;
int _tmpE2=0;
for(0;_tmpE1 < _tmpD8;++ _tmpE1){
char _tmpE3=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE1));
if(_tmpE3 == '\''  || Cyc_is_other_special(_tmpE3)){
char _tmp4A8;char _tmp4A7;struct _dyneither_ptr _tmp4A6;(_tmp4A6=_dyneither_ptr_plus(s2,sizeof(char),_tmpE2 ++),((_tmp4A7=*((char*)_check_dyneither_subscript(_tmp4A6,sizeof(char),0)),((_tmp4A8='\\',((_get_dyneither_size(_tmp4A6,sizeof(char))== 1  && (_tmp4A7 == '\000'  && _tmp4A8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4A6.curr)=_tmp4A8)))))));}{
char _tmp4AB;char _tmp4AA;struct _dyneither_ptr _tmp4A9;(_tmp4A9=_dyneither_ptr_plus(s2,sizeof(char),_tmpE2 ++),((_tmp4AA=*((char*)_check_dyneither_subscript(_tmp4A9,sizeof(char),0)),((_tmp4AB=_tmpE3,((_get_dyneither_size(_tmp4A9,sizeof(char))== 1  && (_tmp4AA == '\000'  && _tmp4AB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4A9.curr)=_tmp4AB)))))));};}
# 835
return(struct _dyneither_ptr)s2;};}
# 837
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4AC;return(_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=Cyc_sh_escape_string(*sp),_tmp4AC)));}
# 841
static void Cyc_process_file(struct _dyneither_ptr filename){
# 843
struct _dyneither_ptr _tmpEF=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4AD;struct _dyneither_ptr _tmpF0=Cyc_strconcat((struct _dyneither_ptr)_tmpEF,((_tmp4AD=".cyp",_tag_dyneither(_tmp4AD,sizeof(char),5))));
const char*_tmp4AE;struct _dyneither_ptr _tmpF1=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpEF,((_tmp4AE=".cyci",_tag_dyneither(_tmp4AE,sizeof(char),6))));
const char*_tmp4AF;struct _dyneither_ptr _tmpF2=Cyc_strconcat((struct _dyneither_ptr)_tmpEF,((_tmp4AF=".cycio",_tag_dyneither(_tmp4AF,sizeof(char),7))));
const char*_tmp4B0;struct _dyneither_ptr _tmpF3=Cyc_strconcat((struct _dyneither_ptr)_tmpEF,((_tmp4B0=".c",_tag_dyneither(_tmp4B0,sizeof(char),3))));
# 850
if(Cyc_v_r){const char*_tmp4B4;void*_tmp4B3[1];struct Cyc_String_pa_PrintArg_struct _tmp4B2;(_tmp4B2.tag=0,((_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4B3[0]=& _tmp4B2,Cyc_fprintf(Cyc_stderr,((_tmp4B4="Compiling %s\n",_tag_dyneither(_tmp4B4,sizeof(char),14))),_tag_dyneither(_tmp4B3,sizeof(void*),1)))))));}{
# 853
const char*_tmp4B6;const char*_tmp4B5;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4B5="r",_tag_dyneither(_tmp4B5,sizeof(char),2))),((_tmp4B6="input file",_tag_dyneither(_tmp4B6,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 860
const char*_tmp4BD;struct _dyneither_ptr*_tmp4BC;const char*_tmp4BB;struct Cyc_List_List*_tmp4BA;struct _dyneither_ptr _tmpF7=
Cyc_str_sepstr(((_tmp4BA=_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA->hd=((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC[0]=(struct _dyneither_ptr)((_tmp4BB="",_tag_dyneither(_tmp4BB,sizeof(char),1))),_tmp4BC)))),((_tmp4BA->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4BA)))))),(
(_tmp4BD=" ",_tag_dyneither(_tmp4BD,sizeof(char),2))));
# 870
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpF8=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4BE;_tmpF8=Cyc_add_subdir(_tmpF8,((_tmp4BE="include",_tag_dyneither(_tmp4BE,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4C1;struct Cyc_List_List*_tmp4C0;_tmpF8=((_tmp4C0=_cycalloc(sizeof(*_tmp4C0)),((_tmp4C0->hd=((_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1[0]=def_inc_path,_tmp4C1)))),((_tmp4C0->tl=_tmpF8,_tmp4C0))))));}{
# 876
char*_tmpFC=getenv("CYCLONE_INCLUDE_PATH");
if(_tmpFC != 0){
char*_tmp4C2;_tmpF8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4C2=_tmpFC,_tag_dyneither(_tmp4C2,sizeof(char),_get_zero_arr_size_char((void*)_tmp4C2,1)))),':'),_tmpF8);}{
# 881
const char*_tmp4C9;struct _dyneither_ptr*_tmp4C8;const char*_tmp4C7;struct Cyc_List_List*_tmp4C6;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6->hd=((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8[0]=(struct _dyneither_ptr)((_tmp4C7="",_tag_dyneither(_tmp4C7,sizeof(char),1))),_tmp4C8)))),((_tmp4C6->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpF8),_tmp4C6)))))),(
(_tmp4C9=" -I",_tag_dyneither(_tmp4C9,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4CD;void*_tmp4CC[1];struct Cyc_String_pa_PrintArg_struct _tmp4CB;ofile_string=(struct _dyneither_ptr)((_tmp4CB.tag=0,((_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4CC[0]=& _tmp4CB,Cyc_aprintf(((_tmp4CD=" > %s",_tag_dyneither(_tmp4CD,sizeof(char),6))),_tag_dyneither(_tmp4CC,sizeof(void*),1))))))));}else{
# 890
const char*_tmp4CE;ofile_string=((_tmp4CE="",_tag_dyneither(_tmp4CE,sizeof(char),1)));}}else{
# 892
const char*_tmp4D2;void*_tmp4D1[1];struct Cyc_String_pa_PrintArg_struct _tmp4D0;ofile_string=(struct _dyneither_ptr)((_tmp4D0.tag=0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF0)),((_tmp4D1[0]=& _tmp4D0,Cyc_aprintf(((_tmp4D2=" > %s",_tag_dyneither(_tmp4D2,sizeof(char),6))),_tag_dyneither(_tmp4D1,sizeof(void*),1))))))));}{
# 894
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 898
if(Cyc_dependencies_target == 0){
# 902
const char*_tmp4D3;fixup_string=((_tmp4D3=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4D3,sizeof(char),35)));}else{
# 907
const char*_tmp4D7;void*_tmp4D6[1];struct Cyc_String_pa_PrintArg_struct _tmp4D5;fixup_string=(struct _dyneither_ptr)(
(_tmp4D5.tag=0,((_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4D6[0]=& _tmp4D5,Cyc_aprintf(((_tmp4D7=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4D7,sizeof(char),29))),_tag_dyneither(_tmp4D6,sizeof(void*),1))))))));}}else{
# 911
const char*_tmp4D8;fixup_string=((_tmp4D8="",_tag_dyneither(_tmp4D8,sizeof(char),1)));}{
# 913
const char*_tmp4E1;void*_tmp4E0[6];struct Cyc_String_pa_PrintArg_struct _tmp4DF;struct Cyc_String_pa_PrintArg_struct _tmp4DE;struct Cyc_String_pa_PrintArg_struct _tmp4DD;struct Cyc_String_pa_PrintArg_struct _tmp4DC;struct Cyc_String_pa_PrintArg_struct _tmp4DB;struct Cyc_String_pa_PrintArg_struct _tmp4DA;struct _dyneither_ptr _tmp10A=(_tmp4DA.tag=0,((_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4DB.tag=0,((_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4DC.tag=0,((_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 916
Cyc_sh_escape_string(filename)),((_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF7),((_tmp4DF.tag=0,((_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4E0[0]=& _tmp4DF,((_tmp4E0[1]=& _tmp4DE,((_tmp4E0[2]=& _tmp4DD,((_tmp4E0[3]=& _tmp4DC,((_tmp4E0[4]=& _tmp4DB,((_tmp4E0[5]=& _tmp4DA,Cyc_aprintf(((_tmp4E1="%s %s%s %s%s%s",_tag_dyneither(_tmp4E1,sizeof(char),15))),_tag_dyneither(_tmp4E0,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 919
if(Cyc_v_r){const char*_tmp4E5;void*_tmp4E4[1];struct Cyc_String_pa_PrintArg_struct _tmp4E3;(_tmp4E3.tag=0,((_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp4E4[0]=& _tmp4E3,Cyc_fprintf(Cyc_stderr,((_tmp4E5="%s\n",_tag_dyneither(_tmp4E5,sizeof(char),4))),_tag_dyneither(_tmp4E4,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp10A,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4E8;void*_tmp4E7;(_tmp4E7=0,Cyc_fprintf(Cyc_stderr,((_tmp4E8="\nError: preprocessing\n",_tag_dyneither(_tmp4E8,sizeof(char),23))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
return;}
# 926
if(Cyc_stop_after_cpp_r)return;
# 929
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF0);{
const char*_tmp4EA;const char*_tmp4E9;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF0,((_tmp4E9="r",_tag_dyneither(_tmp4E9,sizeof(char),2))),((_tmp4EA="file",_tag_dyneither(_tmp4EA,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 933
struct Cyc_List_List*tds=0;
# 936
{struct _handler_cons _tmp110;_push_handler(& _tmp110);{int _tmp112=0;if(setjmp(_tmp110.handler))_tmp112=1;if(!_tmp112){
{const char*_tmp4EB;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4EB="parsing",_tag_dyneither(_tmp4EB,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp111=(void*)_exn_thrown;void*_tmp115=_tmp111;void*_tmp116;void*_tmp117;_LL23: _tmp116=_tmp115;_LL24:
# 940
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
Cyc_Core_rethrow(_tmp116);_LL25: _tmp117=_tmp115;_LL26:(void)_throw(_tmp117);_LL22:;}};}
# 944
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
return;}
# 950
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 955
struct _RegionHandle _tmp118=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp118;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp119=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 960
struct _tuple22 _tmp4EC;struct _tuple22 _tmp11A=(_tmp4EC.f1=tc_rgn,((_tmp4EC.f2=_tmp119,_tmp4EC)));
{const char*_tmp4ED;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4ED="type checking",_tag_dyneither(_tmp4ED,sizeof(char),14))),Cyc_do_typecheck,& _tmp11A,tds);}
# 963
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
_npop_handler(0);return;}
# 967
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4EE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4EE="control-flow checking",_tag_dyneither(_tmp4EE,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
_npop_handler(0);return;}
# 975
if(Cyc_generate_interface_r){
const char*_tmp4F1;const char*_tmp4F0;const char*_tmp4EF;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4EF=(const char*)_untag_dyneither_ptr(_tmpF1,sizeof(char),1),_tag_dyneither(_tmp4EF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4EF,1)))),((_tmp4F0="w",_tag_dyneither(_tmp4F0,sizeof(char),2))),((_tmp4F1="interface file",_tag_dyneither(_tmp4F1,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 980
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 987
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpF1,sizeof(char),1),"r");
const char*_tmp4F3;const char*_tmp4F2;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpF2,((_tmp4F2="w",_tag_dyneither(_tmp4F2,sizeof(char),2))),((_tmp4F3="interface object file",_tag_dyneither(_tmp4F3,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 994
Cyc_Position_reset_position(_tmpF1);{
struct _tuple23 _tmp4F4;struct _tuple23 _tmp120=(_tmp4F4.f1=_tmp119,((_tmp4F4.f2=inter_file,((_tmp4F4.f3=inter_objfile,_tmp4F4)))));
{const char*_tmp4F5;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F5="interface checking",_tag_dyneither(_tmp4F5,sizeof(char),19))),Cyc_do_interface,& _tmp120,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1002
if(Cyc_cf_r)goto PRINTC;
# 1005
{const char*_tmp4F6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F6="translation to C",_tag_dyneither(_tmp4F6,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
_npop_handler(0);return;}
# 1011
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpF0);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1019
const char*_tmp4F7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F7="post-pass to VC",_tag_dyneither(_tmp4F7,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1026
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp4F9;const char*_tmp4F8;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F8="w",_tag_dyneither(_tmp4F8,sizeof(char),2))),((_tmp4F9="output file",_tag_dyneither(_tmp4F9,sizeof(char),12))));}else{
# 1032
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp4FB;const char*_tmp4FA;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4FA="w",_tag_dyneither(_tmp4FA,sizeof(char),2))),((_tmp4FB="output file",_tag_dyneither(_tmp4FB,sizeof(char),12))));}else{
# 1036
const char*_tmp4FD;const char*_tmp4FC;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF3,((_tmp4FC="w",_tag_dyneither(_tmp4FC,sizeof(char),2))),((_tmp4FD="output file",_tag_dyneither(_tmp4FD,sizeof(char),12))));}}
# 1038
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1040
{struct _handler_cons _tmp12E;_push_handler(& _tmp12E);{int _tmp130=0;if(setjmp(_tmp12E.handler))_tmp130=1;if(!_tmp130){
if(!Cyc_noprint_r){
const char*_tmp4FE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FE="printing",_tag_dyneither(_tmp4FE,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1041
;_pop_handler();}else{void*_tmp12F=(void*)_exn_thrown;void*_tmp133=_tmp12F;void*_tmp134;void*_tmp135;_LL28: _tmp134=_tmp133;_LL29:
# 1045
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp501;struct Cyc_List_List*_tmp500;Cyc_cfiles=((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500->hd=((_tmp501=_cycalloc(sizeof(*_tmp501)),((_tmp501[0]=(struct _dyneither_ptr)_tmpF3,_tmp501)))),((_tmp500->tl=Cyc_cfiles,_tmp500))))));}
Cyc_Core_rethrow(_tmp134);_LL2A: _tmp135=_tmp133;_LL2B:(void)_throw(_tmp135);_LL27:;}};}
# 1050
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp504;struct Cyc_List_List*_tmp503;Cyc_cfiles=((_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503->hd=((_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504[0]=(struct _dyneither_ptr)_tmpF3,_tmp504)))),((_tmp503->tl=Cyc_cfiles,_tmp503))))));};}
# 955
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp152[8]="<final>";
# 1055
static struct _dyneither_ptr Cyc_final_str={_tmp152,_tmp152,_tmp152 + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1058
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1062
struct _dyneither_ptr basename;
{struct _handler_cons _tmp153;_push_handler(& _tmp153);{int _tmp155=0;if(setjmp(_tmp153.handler))_tmp155=1;if(!_tmp155){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp154=(void*)_exn_thrown;void*_tmp157=_tmp154;void*_tmp159;_LL2D: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp158=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp157;if(_tmp158->tag != Cyc_Core_Invalid_argument)goto _LL2F;}_LL2E:
# 1066
 basename=*n;goto _LL2C;_LL2F: _tmp159=_tmp157;_LL30:(void)_throw(_tmp159);_LL2C:;}};}{
# 1068
const char*_tmp505;struct _dyneither_ptr _tmp15A=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp505=".cycio",_tag_dyneither(_tmp505,sizeof(char),7))));
const char*_tmp507;const char*_tmp506;struct Cyc___cycFILE*_tmp15B=Cyc_try_file_open((struct _dyneither_ptr)_tmp15A,((_tmp506="rb",_tag_dyneither(_tmp506,sizeof(char),3))),((_tmp507="interface object file",_tag_dyneither(_tmp507,sizeof(char),22))));
if(_tmp15B == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1075
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15A);{
# 1077
struct Cyc_Interface_I*_tmp15C=Cyc_Interface_load(_tmp15B);
Cyc_file_close(_tmp15B);
return _tmp15C;};};};}
# 1082
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1086
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp50D(unsigned int*_tmp50C,unsigned int*_tmp50B,int**_tmp509){for(*_tmp50C=0;*_tmp50C < *_tmp50B;(*_tmp50C)++){(*_tmp509)[*_tmp50C]=(int)0;}}static void _tmp515(unsigned int*_tmp514,unsigned int*_tmp513,struct _dyneither_ptr**_tmp511){for(*_tmp514=0;*_tmp514 < *_tmp513;(*_tmp514)++){(*_tmp511)[*_tmp514]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1123
_tag_dyneither(0,0,0));}}static void _tmp51B(unsigned int*_tmp51A,unsigned int*_tmp519,struct _dyneither_ptr**_tmp517){for(*_tmp51A=0;*_tmp51A < *_tmp519;(*_tmp51A)++){(*_tmp517)[*_tmp51A]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1089
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1094
GC_blacklist_warn_clear();{
# 1096
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1101
struct _RegionHandle _tmp160=_new_region("r");struct _RegionHandle*r=& _tmp160;_push_region(r);{
# 1103
unsigned int _tmp50C;unsigned int _tmp50B;struct _dyneither_ptr _tmp50A;int*_tmp509;unsigned int _tmp508;struct _dyneither_ptr _tmp161=(_tmp508=(unsigned int)argc,((_tmp509=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp508)),((_tmp50A=_tag_dyneither(_tmp509,sizeof(int),_tmp508),((((_tmp50B=_tmp508,_tmp50D(& _tmp50C,& _tmp50B,& _tmp509))),_tmp50A)))))));
int _tmp162=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp50E;if(Cyc_strncmp(((_tmp50E="-B",_tag_dyneither(_tmp50E,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp161,sizeof(int),i))=1;
++ _tmp162;}else{
# 1111
const char*_tmp50F;if(Cyc_strcmp(((_tmp50F="-b",_tag_dyneither(_tmp50F,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp161,sizeof(int),i))=1;
++ _tmp162;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp161,sizeof(int),i))=1;
++ _tmp162;}}}{
# 1123
unsigned int _tmp514;unsigned int _tmp513;struct _dyneither_ptr _tmp512;struct _dyneither_ptr*_tmp511;unsigned int _tmp510;struct _dyneither_ptr _tmp165=(_tmp510=(unsigned int)(_tmp162 + 1),((_tmp511=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp510)),((_tmp512=_tag_dyneither(_tmp511,sizeof(struct _dyneither_ptr),_tmp510),((((_tmp513=_tmp510,_tmp515(& _tmp514,& _tmp513,& _tmp511))),_tmp512)))))));
unsigned int _tmp51A;unsigned int _tmp519;struct _dyneither_ptr _tmp518;struct _dyneither_ptr*_tmp517;unsigned int _tmp516;struct _dyneither_ptr _tmp166=(_tmp516=(unsigned int)(argc - _tmp162),((_tmp517=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp516)),((_tmp518=_tag_dyneither(_tmp517,sizeof(struct _dyneither_ptr),_tmp516),((((_tmp519=_tmp516,_tmp51B(& _tmp51A,& _tmp519,& _tmp517))),_tmp518)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp161,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp165,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1132
struct _tuple24*_tmpA98;const char*_tmpA97;const char*_tmpA96;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA95;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA94;const char*_tmpA93;struct _tuple24*_tmpA92;const char*_tmpA91;const char*_tmpA90;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA8F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA8E;const char*_tmpA8D;struct _tuple24*_tmpA8C;const char*_tmpA8B;const char*_tmpA8A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA89;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA88;const char*_tmpA87;struct _tuple24*_tmpA86;const char*_tmpA85;const char*_tmpA84;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA83;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA82;const char*_tmpA81;struct _tuple24*_tmpA80;const char*_tmpA7F;const char*_tmpA7E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA7D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA7C;const char*_tmpA7B;struct _tuple24*_tmpA7A;const char*_tmpA79;const char*_tmpA78;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA77;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA76;const char*_tmpA75;struct _tuple24*_tmpA74;const char*_tmpA73;const char*_tmpA72;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA71;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA70;const char*_tmpA6F;struct _tuple24*_tmpA6E;const char*_tmpA6D;const char*_tmpA6C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6A;const char*_tmpA69;struct _tuple24*_tmpA68;const char*_tmpA67;const char*_tmpA66;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA65;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA64;const char*_tmpA63;struct _tuple24*_tmpA62;const char*_tmpA61;const char*_tmpA60;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA5F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA5E;const char*_tmpA5D;struct _tuple24*_tmpA5C;const char*_tmpA5B;const char*_tmpA5A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA59;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA58;const char*_tmpA57;struct _tuple24*_tmpA56;const char*_tmpA55;const char*_tmpA54;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA53;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA52;const char*_tmpA51;struct _tuple24*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA4D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA4C;const char*_tmpA4B;struct _tuple24*_tmpA4A;const char*_tmpA49;const char*_tmpA48;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA47;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA46;const char*_tmpA45;struct _tuple24*_tmpA44;const char*_tmpA43;const char*_tmpA42;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA41;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA40;const char*_tmpA3F;struct _tuple24*_tmpA3E;const char*_tmpA3D;const char*_tmpA3C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA3B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA3A;const char*_tmpA39;struct _tuple24*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA35;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA34;const char*_tmpA33;struct _tuple24*_tmpA32;const char*_tmpA31;const char*_tmpA30;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA2F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA2E;const char*_tmpA2D;struct _tuple24*_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA29;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA28;const char*_tmpA27;struct _tuple24*_tmpA26;const char*_tmpA25;const char*_tmpA24;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA23;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA22;const char*_tmpA21;struct _tuple24*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA1D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA1C;const char*_tmpA1B;struct _tuple24*_tmpA1A;const char*_tmpA19;const char*_tmpA18;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA17;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA16;const char*_tmpA15;struct _tuple24*_tmpA14;const char*_tmpA13;const char*_tmpA12;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA11;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA10;const char*_tmpA0F;struct _tuple24*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA0B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA0A;const char*_tmpA09;struct _tuple24*_tmpA08;const char*_tmpA07;const char*_tmpA06;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA05;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA04;const char*_tmpA03;struct _tuple24*_tmpA02;const char*_tmpA01;const char*_tmpA00;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9FF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9FE;const char*_tmp9FD;struct _tuple24*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F8;const char*_tmp9F7;struct _tuple24*_tmp9F6;const char*_tmp9F5;const char*_tmp9F4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F2;const char*_tmp9F1;struct _tuple24*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9ED;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9EC;const char*_tmp9EB;struct _tuple24*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple24*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple24*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple24*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple24*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple24*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple24*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple24*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple24*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple24*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple24*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple24*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple24*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple24*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple24*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple24*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple24*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple24*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple24*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple24*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple24*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple24*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple24*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple24*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple24*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple24*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple24*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple24*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple24*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple24*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple24*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple24*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple24*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple24*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple24*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple24*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple24*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple24*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple24*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple24*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple24*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple24*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple24*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple24*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple24*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple24*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple24*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple24*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple24*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple24*_tmp8C4[78];struct Cyc_List_List*options=
(_tmp8C4[77]=(
# 1366
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="--manyerrors",_tag_dyneither(_tmp8C9,sizeof(char),13))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)(
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=0,((_tmp8C7.f1=Cyc_set_many_errors,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
(_tmp8C5="don't stop after only a few errors.",_tag_dyneither(_tmp8C5,sizeof(char),36))),_tmp8CA)))))))))))),((_tmp8C4[76]=(
# 1363
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="--interface",_tag_dyneither(_tmp8CF,sizeof(char),12))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE=" <file>",_tag_dyneither(_tmp8CE,sizeof(char),8))),((_tmp8D0->f4=(void*)(
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=5,((_tmp8CD.f1=Cyc_set_specified_interface,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
(_tmp8CB="Set the interface file to be <file>.",_tag_dyneither(_tmp8CB,sizeof(char),37))),_tmp8D0)))))))))))),((_tmp8C4[75]=(
# 1360
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="--geninterface",_tag_dyneither(_tmp8D5,sizeof(char),15))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="",_tag_dyneither(_tmp8D4,sizeof(char),1))),((_tmp8D6->f4=(void*)(
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=3,((_tmp8D3.f1=& Cyc_generate_interface_r,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
(_tmp8D1="Generate interface files",_tag_dyneither(_tmp8D1,sizeof(char),25))),_tmp8D6)))))))))))),((_tmp8C4[74]=(
# 1357
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-idirafter",_tag_dyneither(_tmp8DB,sizeof(char),11))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="<dir>",_tag_dyneither(_tmp8DA,sizeof(char),6))),((_tmp8DC->f4=(void*)(
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=5,((_tmp8D9.f1=Cyc_add_idirafter,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
(_tmp8D7="Add the directory to the second include path.",_tag_dyneither(_tmp8D7,sizeof(char),46))),_tmp8DC)))))))))))),((_tmp8C4[73]=(
# 1354
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-isystem",_tag_dyneither(_tmp8E1,sizeof(char),9))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="<dir>",_tag_dyneither(_tmp8E0,sizeof(char),6))),((_tmp8E2->f4=(void*)(
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=5,((_tmp8DF.f1=Cyc_add_isystem,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
(_tmp8DD="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8DD,sizeof(char),90))),_tmp8E2)))))))))))),((_tmp8C4[72]=(
# 1351
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-iwithprefixbefore",_tag_dyneither(_tmp8E7,sizeof(char),19))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="<dir>",_tag_dyneither(_tmp8E6,sizeof(char),6))),((_tmp8E8->f4=(void*)(
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=5,((_tmp8E5.f1=Cyc_add_iwithprefixbefore,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
(_tmp8E3="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8E3,sizeof(char),45))),_tmp8E8)))))))))))),((_tmp8C4[71]=(
# 1348
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-iwithprefix",_tag_dyneither(_tmp8ED,sizeof(char),13))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="<dir>",_tag_dyneither(_tmp8EC,sizeof(char),6))),((_tmp8EE->f4=(void*)(
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=5,((_tmp8EB.f1=Cyc_add_iwithprefix,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
(_tmp8E9="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp8E9,sizeof(char),47))),_tmp8EE)))))))))))),((_tmp8C4[70]=(
# 1345
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-iprefix",_tag_dyneither(_tmp8F3,sizeof(char),9))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="<prefix>",_tag_dyneither(_tmp8F2,sizeof(char),9))),((_tmp8F4->f4=(void*)(
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=5,((_tmp8F1.f1=Cyc_add_iprefix,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
(_tmp8EF="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp8EF,sizeof(char),67))),_tmp8F4)))))))))))),((_tmp8C4[69]=(
# 1342
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-noregions",_tag_dyneither(_tmp8F9,sizeof(char),11))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)(
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=3,((_tmp8F7.f1=& Cyc_Absyn_no_regions,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
(_tmp8F5="Generate code that doesn't use regions",_tag_dyneither(_tmp8F5,sizeof(char),39))),_tmp8FA)))))))))))),((_tmp8C4[68]=(
# 1339
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-port",_tag_dyneither(_tmp8FF,sizeof(char),6))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=0,((_tmp8FD.f1=Cyc_set_port_c_code,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
(_tmp8FB="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp8FB,sizeof(char),38))),_tmp900)))))))))))),((_tmp8C4[67]=(
# 1336
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-detailedlocation",_tag_dyneither(_tmp905,sizeof(char),18))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=4,((_tmp903.f1=& Cyc_Position_use_gcc_style_location,_tmp903)))),_tmp902)))),((_tmp906->f5=(
(_tmp901="Try to give more detailed location information for errors",_tag_dyneither(_tmp901,sizeof(char),58))),_tmp906)))))))))))),((_tmp8C4[66]=(
# 1333
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-noregister",_tag_dyneither(_tmp90B,sizeof(char),12))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)(
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=3,((_tmp909.f1=& Cyc_Parse_no_register,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
(_tmp907="Treat register storage class as public",_tag_dyneither(_tmp907,sizeof(char),39))),_tmp90C)))))))))))),((_tmp8C4[65]=(
# 1330
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="-warnregioncoerce",_tag_dyneither(_tmp911,sizeof(char),18))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=3,((_tmp90F.f1=& Cyc_Tcutil_warn_region_coerce,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
(_tmp90D="Warn when any region coercion is inserted",_tag_dyneither(_tmp90D,sizeof(char),42))),_tmp912)))))))))))),((_tmp8C4[64]=(
# 1327
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="-warnaliascoercion",_tag_dyneither(_tmp917,sizeof(char),19))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=3,((_tmp915.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp915)))),_tmp914)))),((_tmp918->f5=(
(_tmp913="Warn when any alias coercion is inserted",_tag_dyneither(_tmp913,sizeof(char),41))),_tmp918)))))))))))),((_tmp8C4[63]=(
# 1324
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="-warnnullchecks",_tag_dyneither(_tmp91D,sizeof(char),16))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)(
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=3,((_tmp91B.f1=& Cyc_Toc_warn_all_null_deref,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
(_tmp919="Warn when any null check can't be eliminated",_tag_dyneither(_tmp919,sizeof(char),45))),_tmp91E)))))))))))),((_tmp8C4[62]=(
# 1321
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="-warnboundschecks",_tag_dyneither(_tmp923,sizeof(char),18))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)(
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=3,((_tmp921.f1=& Cyc_Toc_warn_bounds_checks,_tmp921)))),_tmp920)))),((_tmp924->f5=(
(_tmp91F="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp91F,sizeof(char),44))),_tmp924)))))))))))),((_tmp8C4[61]=(
# 1318
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="-CI",_tag_dyneither(_tmp929,sizeof(char),4))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928=" <file>",_tag_dyneither(_tmp928,sizeof(char),8))),((_tmp92A->f4=(void*)(
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=5,((_tmp927.f1=Cyc_set_cyc_include,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
(_tmp925="Set cyc_include.h to be <file>",_tag_dyneither(_tmp925,sizeof(char),31))),_tmp92A)))))))))))),((_tmp8C4[60]=(
# 1315
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-compile-for-boot",_tag_dyneither(_tmp92F,sizeof(char),18))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)(
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=3,((_tmp92D.f1=& Cyc_Lex_compile_for_boot_r,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
(_tmp92B="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp92B,sizeof(char),71))),_tmp930)))))))))))),((_tmp8C4[59]=(
# 1312
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="-nocyc_setjmp",_tag_dyneither(_tmp935,sizeof(char),14))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=3,((_tmp933.f1=& Cyc_nocyc_setjmp_r,_tmp933)))),_tmp932)))),((_tmp936->f5=(
(_tmp931="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp931,sizeof(char),46))),_tmp936)))))))))))),((_tmp8C4[58]=(
# 1309
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-printalleffects",_tag_dyneither(_tmp93B,sizeof(char),17))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)(
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=3,((_tmp939.f1=& Cyc_print_all_effects_r,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
(_tmp937="Print effects for functions (type debugging)",_tag_dyneither(_tmp937,sizeof(char),45))),_tmp93C)))))))))))),((_tmp8C4[57]=(
# 1306
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-printfullevars",_tag_dyneither(_tmp941,sizeof(char),16))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=3,((_tmp93F.f1=& Cyc_print_full_evars_r,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
(_tmp93D="Print full information for evars (type debugging)",_tag_dyneither(_tmp93D,sizeof(char),50))),_tmp942)))))))))))),((_tmp8C4[56]=(
# 1303
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-printallkinds",_tag_dyneither(_tmp947,sizeof(char),15))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=3,((_tmp945.f1=& Cyc_print_all_kinds_r,_tmp945)))),_tmp944)))),((_tmp948->f5=(
(_tmp943="Always print kinds of type variables",_tag_dyneither(_tmp943,sizeof(char),37))),_tmp948)))))))))))),((_tmp8C4[55]=(
# 1300
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-printalltvars",_tag_dyneither(_tmp94D,sizeof(char),15))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=3,((_tmp94B.f1=& Cyc_print_all_tvars_r,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
(_tmp949="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp949,sizeof(char),57))),_tmp94E)))))))))))),((_tmp8C4[54]=(
# 1297
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-noexpandtypedefs",_tag_dyneither(_tmp953,sizeof(char),18))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=3,((_tmp951.f1=& Cyc_noexpand_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
(_tmp94F="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp94F,sizeof(char),41))),_tmp954)))))))))))),((_tmp8C4[53]=(
# 1294
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-noremoveunused",_tag_dyneither(_tmp959,sizeof(char),16))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=3,((_tmp957.f1=& Cyc_noshake_r,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
(_tmp955="Don't remove externed variables that aren't used",_tag_dyneither(_tmp955,sizeof(char),49))),_tmp95A)))))))))))),((_tmp8C4[52]=(
# 1291
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-nogc",_tag_dyneither(_tmp95F,sizeof(char),6))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)(
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=3,((_tmp95D.f1=& Cyc_nogc_r,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
(_tmp95B="Don't link in the garbage collector",_tag_dyneither(_tmp95B,sizeof(char),36))),_tmp960)))))))))))),((_tmp8C4[51]=(
# 1288
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-nocyc",_tag_dyneither(_tmp965,sizeof(char),7))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)(
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=0,((_tmp963.f1=Cyc_set_nocyc,_tmp963)))),_tmp962)))),((_tmp966->f5=(
(_tmp961="Don't add implicit namespace Cyc",_tag_dyneither(_tmp961,sizeof(char),33))),_tmp966)))))))))))),((_tmp8C4[50]=(
# 1285
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-no-cpp-precomp",_tag_dyneither(_tmp96B,sizeof(char),16))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)(
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=0,((_tmp969.f1=Cyc_set_nocppprecomp,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
(_tmp967="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp967,sizeof(char),40))),_tmp96C)))))))))))),((_tmp8C4[49]=(
# 1282
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="-use-cpp",_tag_dyneither(_tmp971,sizeof(char),9))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="<path>",_tag_dyneither(_tmp970,sizeof(char),7))),((_tmp972->f4=(void*)(
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=5,((_tmp96F.f1=Cyc_set_cpp,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
(_tmp96D="Indicate which preprocessor to use",_tag_dyneither(_tmp96D,sizeof(char),35))),_tmp972)))))))))))),((_tmp8C4[48]=(
# 1279
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="--inline-checks",_tag_dyneither(_tmp977,sizeof(char),16))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=0,((_tmp975.f1=Cyc_set_inline_functions,_tmp975)))),_tmp974)))),((_tmp978->f5=(
(_tmp973="Inline bounds checks instead of #define",_tag_dyneither(_tmp973,sizeof(char),40))),_tmp978)))))))))))),((_tmp8C4[47]=(
# 1276
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="--noboundschecks",_tag_dyneither(_tmp97D,sizeof(char),17))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=0,((_tmp97B.f1=Cyc_set_noboundschecks,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
(_tmp979="Disable bounds checks",_tag_dyneither(_tmp979,sizeof(char),22))),_tmp97E)))))))))))),((_tmp8C4[46]=(
# 1273
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="--nonullchecks",_tag_dyneither(_tmp983,sizeof(char),15))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=0,((_tmp981.f1=Cyc_set_nonullchecks,_tmp981)))),_tmp980)))),((_tmp984->f5=(
(_tmp97F="Disable null checks",_tag_dyneither(_tmp97F,sizeof(char),20))),_tmp984)))))))))))),((_tmp8C4[45]=(
# 1270
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="--nochecks",_tag_dyneither(_tmp989,sizeof(char),11))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)(
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=0,((_tmp987.f1=Cyc_set_nochecks,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
(_tmp985="Disable bounds/null checks",_tag_dyneither(_tmp985,sizeof(char),27))),_tmp98A)))))))))))),((_tmp8C4[44]=(
# 1267
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="--nolineno",_tag_dyneither(_tmp98F,sizeof(char),11))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)(
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=0,((_tmp98D.f1=Cyc_clear_lineno,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
(_tmp98B="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp98B,sizeof(char),63))),_tmp990)))))))))))),((_tmp8C4[43]=(
# 1264
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-save-c",_tag_dyneither(_tmp995,sizeof(char),8))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)(
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=3,((_tmp993.f1=& Cyc_save_c_r,_tmp993)))),_tmp992)))),((_tmp996->f5=(
(_tmp991="Don't delete temporary C files",_tag_dyneither(_tmp991,sizeof(char),31))),_tmp996)))))))))))),((_tmp8C4[42]=(
# 1261
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-save-temps",_tag_dyneither(_tmp99B,sizeof(char),12))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),((_tmp99C->f4=(void*)(
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=0,((_tmp999.f1=Cyc_set_save_temps,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
(_tmp997="Don't delete temporary files",_tag_dyneither(_tmp997,sizeof(char),29))),_tmp99C)))))))))))),((_tmp8C4[41]=(
# 1258
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-c-comp",_tag_dyneither(_tmp9A1,sizeof(char),8))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0=" <compiler>",_tag_dyneither(_tmp9A0,sizeof(char),12))),((_tmp9A2->f4=(void*)(
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=5,((_tmp99F.f1=Cyc_set_c_compiler,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
(_tmp99D="Produce C output for the given compiler",_tag_dyneither(_tmp99D,sizeof(char),40))),_tmp9A2)))))))))))),((_tmp8C4[40]=(
# 1255
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-un-gcc",_tag_dyneither(_tmp9A7,sizeof(char),8))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=0,((_tmp9A5.f1=Cyc_set_tovc,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
(_tmp9A3="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp9A3,sizeof(char),57))),_tmp9A8)))))))))))),((_tmp8C4[39]=(
# 1252
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-no-elim-statement-expressions",_tag_dyneither(_tmp9AD,sizeof(char),31))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)(
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=4,((_tmp9AB.f1=& Cyc_elim_se_r,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
(_tmp9A9="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9A9,sizeof(char),47))),_tmp9AE)))))))))))),((_tmp8C4[38]=(
# 1248
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-elim-statement-expressions",_tag_dyneither(_tmp9B3,sizeof(char),28))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=3,((_tmp9B1.f1=& Cyc_elim_se_r,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
# 1251
(_tmp9AF="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9AF,sizeof(char),66))),_tmp9B4)))))))))))),((_tmp8C4[37]=(
# 1245
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-up",_tag_dyneither(_tmp9B9,sizeof(char),4))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="",_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9BA->f4=(void*)(
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=4,((_tmp9B7.f1=& Cyc_pp_r,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
(_tmp9B5="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9B5,sizeof(char),55))),_tmp9BA)))))))))))),((_tmp8C4[36]=(
# 1242
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-pp",_tag_dyneither(_tmp9BF,sizeof(char),4))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE="",_tag_dyneither(_tmp9BE,sizeof(char),1))),((_tmp9C0->f4=(void*)(
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=3,((_tmp9BD.f1=& Cyc_pp_r,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
(_tmp9BB="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9BB,sizeof(char),47))),_tmp9C0)))))))))))),((_tmp8C4[35]=(
# 1239
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-ic",_tag_dyneither(_tmp9C5,sizeof(char),4))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)(
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=3,((_tmp9C3.f1=& Cyc_ic_r,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
(_tmp9C1="Activate the link-checker",_tag_dyneither(_tmp9C1,sizeof(char),26))),_tmp9C6)))))))))))),((_tmp8C4[34]=(
# 1236
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-stopafter-toc",_tag_dyneither(_tmp9CB,sizeof(char),15))),((_tmp9CC->f2=0,((_tmp9CC->f3=((_tmp9CA="",_tag_dyneither(_tmp9CA,sizeof(char),1))),((_tmp9CC->f4=(void*)(
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=3,((_tmp9C9.f1=& Cyc_toc_r,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
(_tmp9C7="Stop after translation to C",_tag_dyneither(_tmp9C7,sizeof(char),28))),_tmp9CC)))))))))))),((_tmp8C4[33]=(
# 1233
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-stopafter-cf",_tag_dyneither(_tmp9D1,sizeof(char),14))),((_tmp9D2->f2=0,((_tmp9D2->f3=((_tmp9D0="",_tag_dyneither(_tmp9D0,sizeof(char),1))),((_tmp9D2->f4=(void*)(
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=3,((_tmp9CF.f1=& Cyc_cf_r,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
(_tmp9CD="Stop after control-flow checking",_tag_dyneither(_tmp9CD,sizeof(char),33))),_tmp9D2)))))))))))),((_tmp8C4[32]=(
# 1230
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-noprint",_tag_dyneither(_tmp9D7,sizeof(char),9))),((_tmp9D8->f2=0,((_tmp9D8->f3=((_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1))),((_tmp9D8->f4=(void*)(
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=3,((_tmp9D5.f1=& Cyc_noprint_r,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
(_tmp9D3="Do not print output (when stopping early)",_tag_dyneither(_tmp9D3,sizeof(char),42))),_tmp9D8)))))))))))),((_tmp8C4[31]=(
# 1227
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-stopafter-tc",_tag_dyneither(_tmp9DD,sizeof(char),14))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC="",_tag_dyneither(_tmp9DC,sizeof(char),1))),((_tmp9DE->f4=(void*)(
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=3,((_tmp9DB.f1=& Cyc_tc_r,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
(_tmp9D9="Stop after type checking",_tag_dyneither(_tmp9D9,sizeof(char),25))),_tmp9DE)))))))))))),((_tmp8C4[30]=(
# 1224
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-stopafter-parse",_tag_dyneither(_tmp9E3,sizeof(char),17))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)(
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=3,((_tmp9E1.f1=& Cyc_parseonly_r,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
(_tmp9DF="Stop after parsing",_tag_dyneither(_tmp9DF,sizeof(char),19))),_tmp9E4)))))))))))),((_tmp8C4[29]=(
# 1221
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-E",_tag_dyneither(_tmp9E9,sizeof(char),3))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)(
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=3,((_tmp9E7.f1=& Cyc_stop_after_cpp_r,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
(_tmp9E5="Stop after preprocessing",_tag_dyneither(_tmp9E5,sizeof(char),25))),_tmp9EA)))))))))))),((_tmp8C4[28]=(
# 1218
(_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-Wall",_tag_dyneither(_tmp9EF,sizeof(char),6))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE="",_tag_dyneither(_tmp9EE,sizeof(char),1))),((_tmp9F0->f4=(void*)(
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=0,((_tmp9ED.f1=Cyc_set_all_warnings,_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=(
(_tmp9EB="Turn on all warnings",_tag_dyneither(_tmp9EB,sizeof(char),21))),_tmp9F0)))))))))))),((_tmp8C4[27]=(
# 1215
(_tmp9F6=_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="-Woverride",_tag_dyneither(_tmp9F5,sizeof(char),11))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4="",_tag_dyneither(_tmp9F4,sizeof(char),1))),((_tmp9F6->f4=(void*)(
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=3,((_tmp9F3.f1=& Cyc_Tcenv_warn_override,_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=(
(_tmp9F1="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmp9F1,sizeof(char),58))),_tmp9F6)))))))))))),((_tmp8C4[26]=(
# 1212
(_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-Wlose-unique",_tag_dyneither(_tmp9FB,sizeof(char),14))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA="",_tag_dyneither(_tmp9FA,sizeof(char),1))),((_tmp9FC->f4=(void*)(
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=3,((_tmp9F9.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=(
(_tmp9F7="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp9F7,sizeof(char),49))),_tmp9FC)))))))))))),((_tmp8C4[25]=(
# 1209
(_tmpA02=_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02->f1=((_tmpA01="-b",_tag_dyneither(_tmpA01,sizeof(char),3))),((_tmpA02->f2=0,((_tmpA02->f3=((_tmpA00="<arch>",_tag_dyneither(_tmpA00,sizeof(char),7))),((_tmpA02->f4=(void*)(
(_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=5,((_tmp9FF.f1=Cyc_set_target_arch,_tmp9FF)))),_tmp9FE)))),((_tmpA02->f5=(
(_tmp9FD="Set target architecture; implies -stopafter-toc",_tag_dyneither(_tmp9FD,sizeof(char),48))),_tmpA02)))))))))))),((_tmp8C4[24]=(
# 1206
(_tmpA08=_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08->f1=((_tmpA07="-MT",_tag_dyneither(_tmpA07,sizeof(char),4))),((_tmpA08->f2=0,((_tmpA08->f3=((_tmpA06=" <target>",_tag_dyneither(_tmpA06,sizeof(char),10))),((_tmpA08->f4=(void*)(
(_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=5,((_tmpA05.f1=Cyc_set_dependencies_target,_tmpA05)))),_tmpA04)))),((_tmpA08->f5=(
(_tmpA03="Give target for dependencies",_tag_dyneither(_tmpA03,sizeof(char),29))),_tmpA08)))))))))))),((_tmp8C4[23]=(
# 1202
(_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E->f1=((_tmpA0D="-MG",_tag_dyneither(_tmpA0D,sizeof(char),4))),((_tmpA0E->f2=0,((_tmpA0E->f3=((_tmpA0C="",_tag_dyneither(_tmpA0C,sizeof(char),1))),((_tmpA0E->f4=(void*)(
(_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=1,((_tmpA0B.f1=Cyc_add_cpparg,_tmpA0B)))),_tmpA0A)))),((_tmpA0E->f5=(
# 1205
(_tmpA09="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmpA09,sizeof(char),68))),_tmpA0E)))))))))))),((_tmp8C4[22]=(
# 1199
(_tmpA14=_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14->f1=((_tmpA13="-M",_tag_dyneither(_tmpA13,sizeof(char),3))),((_tmpA14->f2=0,((_tmpA14->f3=((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1))),((_tmpA14->f4=(void*)(
(_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10[0]=((_tmpA11.tag=0,((_tmpA11.f1=Cyc_set_produce_dependencies,_tmpA11)))),_tmpA10)))),((_tmpA14->f5=(
(_tmpA0F="Produce dependencies",_tag_dyneither(_tmpA0F,sizeof(char),21))),_tmpA14)))))))))))),((_tmp8C4[21]=(
# 1196
(_tmpA1A=_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A->f1=((_tmpA19="-S",_tag_dyneither(_tmpA19,sizeof(char),3))),((_tmpA1A->f2=0,((_tmpA1A->f3=((_tmpA18="",_tag_dyneither(_tmpA18,sizeof(char),1))),((_tmpA1A->f4=(void*)(
(_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=0,((_tmpA17.f1=Cyc_set_stop_after_asmfile,_tmpA17)))),_tmpA16)))),((_tmpA1A->f5=(
(_tmpA15="Stop after producing assembly code",_tag_dyneither(_tmpA15,sizeof(char),35))),_tmpA1A)))))))))))),((_tmp8C4[20]=(
# 1193
(_tmpA20=_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20->f1=((_tmpA1F="-pa",_tag_dyneither(_tmpA1F,sizeof(char),4))),((_tmpA20->f2=0,((_tmpA20->f3=((_tmpA1E="",_tag_dyneither(_tmpA1E,sizeof(char),1))),((_tmpA20->f4=(void*)(
(_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1D.tag=0,((_tmpA1D.f1=Cyc_set_pa,_tmpA1D)))),_tmpA1C)))),((_tmpA20->f5=(
(_tmpA1B="Compile for profiling with aprof",_tag_dyneither(_tmpA1B,sizeof(char),33))),_tmpA20)))))))))))),((_tmp8C4[19]=(
# 1190
(_tmpA26=_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26->f1=((_tmpA25="-pg",_tag_dyneither(_tmpA25,sizeof(char),4))),((_tmpA26->f2=0,((_tmpA26->f3=((_tmpA24="",_tag_dyneither(_tmpA24,sizeof(char),1))),((_tmpA26->f4=(void*)(
(_tmpA22=_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=0,((_tmpA23.f1=Cyc_set_pg,_tmpA23)))),_tmpA22)))),((_tmpA26->f5=(
(_tmpA21="Compile for profiling with gprof",_tag_dyneither(_tmpA21,sizeof(char),33))),_tmpA26)))))))))))),((_tmp8C4[18]=(
# 1187
(_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->f1=((_tmpA2B="-p",_tag_dyneither(_tmpA2B,sizeof(char),3))),((_tmpA2C->f2=0,((_tmpA2C->f3=((_tmpA2A="",_tag_dyneither(_tmpA2A,sizeof(char),1))),((_tmpA2C->f4=(void*)(
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=1,((_tmpA29.f1=Cyc_add_ccarg,_tmpA29)))),_tmpA28)))),((_tmpA2C->f5=(
(_tmpA27="Compile for profiling with prof",_tag_dyneither(_tmpA27,sizeof(char),32))),_tmpA2C)))))))))))),((_tmp8C4[17]=(
# 1184
(_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA31="-g",_tag_dyneither(_tmpA31,sizeof(char),3))),((_tmpA32->f2=0,((_tmpA32->f3=((_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),((_tmpA32->f4=(void*)(
(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=1,((_tmpA2F.f1=Cyc_add_ccarg,_tmpA2F)))),_tmpA2E)))),((_tmpA32->f5=(
(_tmpA2D="Compile for debugging",_tag_dyneither(_tmpA2D,sizeof(char),22))),_tmpA32)))))))))))),((_tmp8C4[16]=(
# 1181
(_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->f1=((_tmpA37="-fomit-frame-pointer",_tag_dyneither(_tmpA37,sizeof(char),21))),((_tmpA38->f2=0,((_tmpA38->f3=((_tmpA36="",_tag_dyneither(_tmpA36,sizeof(char),1))),((_tmpA38->f4=(void*)(
(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=1,((_tmpA35.f1=Cyc_add_ccarg,_tmpA35)))),_tmpA34)))),((_tmpA38->f5=(
(_tmpA33="Omit frame pointer",_tag_dyneither(_tmpA33,sizeof(char),19))),_tmpA38)))))))))))),((_tmp8C4[15]=(
# 1178
(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->f1=((_tmpA3D="-O3",_tag_dyneither(_tmpA3D,sizeof(char),4))),((_tmpA3E->f2=0,((_tmpA3E->f3=((_tmpA3C="",_tag_dyneither(_tmpA3C,sizeof(char),1))),((_tmpA3E->f4=(void*)(
(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=1,((_tmpA3B.f1=Cyc_add_ccarg,_tmpA3B)))),_tmpA3A)))),((_tmpA3E->f5=(
(_tmpA39="Even more optimization",_tag_dyneither(_tmpA39,sizeof(char),23))),_tmpA3E)))))))))))),((_tmp8C4[14]=(
# 1175
(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->f1=((_tmpA43="-O2",_tag_dyneither(_tmpA43,sizeof(char),4))),((_tmpA44->f2=0,((_tmpA44->f3=((_tmpA42="",_tag_dyneither(_tmpA42,sizeof(char),1))),((_tmpA44->f4=(void*)(
(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=1,((_tmpA41.f1=Cyc_add_ccarg,_tmpA41)))),_tmpA40)))),((_tmpA44->f5=(
(_tmpA3F="A higher level of optimization",_tag_dyneither(_tmpA3F,sizeof(char),31))),_tmpA44)))))))))))),((_tmp8C4[13]=(
# 1172
(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->f1=((_tmpA49="-O",_tag_dyneither(_tmpA49,sizeof(char),3))),((_tmpA4A->f2=0,((_tmpA4A->f3=((_tmpA48="",_tag_dyneither(_tmpA48,sizeof(char),1))),((_tmpA4A->f4=(void*)(
(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=1,((_tmpA47.f1=Cyc_add_ccarg,_tmpA47)))),_tmpA46)))),((_tmpA4A->f5=(
(_tmpA45="Optimize",_tag_dyneither(_tmpA45,sizeof(char),9))),_tmpA4A)))))))))))),((_tmp8C4[12]=(
# 1169
(_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->f1=((_tmpA4F="-O0",_tag_dyneither(_tmpA4F,sizeof(char),4))),((_tmpA50->f2=0,((_tmpA50->f3=((_tmpA4E="",_tag_dyneither(_tmpA4E,sizeof(char),1))),((_tmpA50->f4=(void*)(
(_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=1,((_tmpA4D.f1=Cyc_add_ccarg,_tmpA4D)))),_tmpA4C)))),((_tmpA50->f5=(
(_tmpA4B="Don't optimize",_tag_dyneither(_tmpA4B,sizeof(char),15))),_tmpA50)))))))))))),((_tmp8C4[11]=(
# 1166
(_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56->f1=((_tmpA55="-s",_tag_dyneither(_tmpA55,sizeof(char),3))),((_tmpA56->f2=0,((_tmpA56->f3=((_tmpA54="",_tag_dyneither(_tmpA54,sizeof(char),1))),((_tmpA56->f4=(void*)(
(_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=1,((_tmpA53.f1=Cyc_add_ccarg,_tmpA53)))),_tmpA52)))),((_tmpA56->f5=(
(_tmpA51="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA51,sizeof(char),60))),_tmpA56)))))))))))),((_tmp8C4[10]=(
# 1163
(_tmpA5C=_region_malloc(r,sizeof(*_tmpA5C)),((_tmpA5C->f1=((_tmpA5B="-x",_tag_dyneither(_tmpA5B,sizeof(char),3))),((_tmpA5C->f2=0,((_tmpA5C->f3=((_tmpA5A=" <language>",_tag_dyneither(_tmpA5A,sizeof(char),12))),((_tmpA5C->f4=(void*)(
(_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=5,((_tmpA59.f1=Cyc_set_inputtype,_tmpA59)))),_tmpA58)))),((_tmpA5C->f5=(
(_tmpA57="Specify <language> for the following input files",_tag_dyneither(_tmpA57,sizeof(char),49))),_tmpA5C)))))))))))),((_tmp8C4[9]=(
# 1160
(_tmpA62=_region_malloc(r,sizeof(*_tmpA62)),((_tmpA62->f1=((_tmpA61="-c",_tag_dyneither(_tmpA61,sizeof(char),3))),((_tmpA62->f2=0,((_tmpA62->f3=((_tmpA60="",_tag_dyneither(_tmpA60,sizeof(char),1))),((_tmpA62->f4=(void*)(
(_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=0,((_tmpA5F.f1=Cyc_set_stop_after_objectfile,_tmpA5F)))),_tmpA5E)))),((_tmpA62->f5=(
(_tmpA5D="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA5D,sizeof(char),61))),_tmpA62)))))))))))),((_tmp8C4[8]=(
# 1157
(_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->f1=((_tmpA67="-m",_tag_dyneither(_tmpA67,sizeof(char),3))),((_tmpA68->f2=1,((_tmpA68->f3=((_tmpA66="<option>",_tag_dyneither(_tmpA66,sizeof(char),9))),((_tmpA68->f4=(void*)(
(_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=1,((_tmpA65.f1=Cyc_add_marg,_tmpA65)))),_tmpA64)))),((_tmpA68->f5=(
(_tmpA63="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA63,sizeof(char),52))),_tmpA68)))))))))))),((_tmp8C4[7]=(
# 1154
(_tmpA6E=_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E->f1=((_tmpA6D="-l",_tag_dyneither(_tmpA6D,sizeof(char),3))),((_tmpA6E->f2=1,((_tmpA6E->f3=((_tmpA6C="<libname>",_tag_dyneither(_tmpA6C,sizeof(char),10))),((_tmpA6E->f4=(void*)(
(_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=1,((_tmpA6B.f1=Cyc_add_libarg,_tmpA6B)))),_tmpA6A)))),((_tmpA6E->f5=(
(_tmpA69="Library file",_tag_dyneither(_tmpA69,sizeof(char),13))),_tmpA6E)))))))))))),((_tmp8C4[6]=(
# 1151
(_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->f1=((_tmpA73="-L",_tag_dyneither(_tmpA73,sizeof(char),3))),((_tmpA74->f2=1,((_tmpA74->f3=((_tmpA72="<dir>",_tag_dyneither(_tmpA72,sizeof(char),6))),((_tmpA74->f4=(void*)(
(_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=1,((_tmpA71.f1=Cyc_add_ccarg,_tmpA71)))),_tmpA70)))),((_tmpA74->f5=(
(_tmpA6F="Add to the list of directories for -l",_tag_dyneither(_tmpA6F,sizeof(char),38))),_tmpA74)))))))))))),((_tmp8C4[5]=(
# 1148
(_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A->f1=((_tmpA79="-I",_tag_dyneither(_tmpA79,sizeof(char),3))),((_tmpA7A->f2=1,((_tmpA7A->f3=((_tmpA78="<dir>",_tag_dyneither(_tmpA78,sizeof(char),6))),((_tmpA7A->f4=(void*)(
(_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=1,((_tmpA77.f1=Cyc_add_cpparg,_tmpA77)))),_tmpA76)))),((_tmpA7A->f5=(
(_tmpA75="Add to the list of directories to search for include files",_tag_dyneither(_tmpA75,sizeof(char),59))),_tmpA7A)))))))))))),((_tmp8C4[4]=(
# 1145
(_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((_tmpA80->f1=((_tmpA7F="-B",_tag_dyneither(_tmpA7F,sizeof(char),3))),((_tmpA80->f2=1,((_tmpA80->f3=((_tmpA7E="<file>",_tag_dyneither(_tmpA7E,sizeof(char),7))),((_tmpA80->f4=(void*)(
(_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=1,((_tmpA7D.f1=Cyc_add_cyclone_exec_path,_tmpA7D)))),_tmpA7C)))),((_tmpA80->f5=(
(_tmpA7B="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA7B,sizeof(char),60))),_tmpA80)))))))))))),((_tmp8C4[3]=(
# 1142
(_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86->f1=((_tmpA85="-D",_tag_dyneither(_tmpA85,sizeof(char),3))),((_tmpA86->f2=1,((_tmpA86->f3=((_tmpA84="<name>[=<value>]",_tag_dyneither(_tmpA84,sizeof(char),17))),((_tmpA86->f4=(void*)(
(_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=1,((_tmpA83.f1=Cyc_add_cpparg,_tmpA83)))),_tmpA82)))),((_tmpA86->f5=(
(_tmpA81="Pass definition to preprocessor",_tag_dyneither(_tmpA81,sizeof(char),32))),_tmpA86)))))))))))),((_tmp8C4[2]=(
# 1139
(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->f1=((_tmpA8B="-o",_tag_dyneither(_tmpA8B,sizeof(char),3))),((_tmpA8C->f2=0,((_tmpA8C->f3=((_tmpA8A=" <file>",_tag_dyneither(_tmpA8A,sizeof(char),8))),((_tmpA8C->f4=(void*)(
(_tmpA88=_region_malloc(r,sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=5,((_tmpA89.f1=Cyc_set_output_file,_tmpA89)))),_tmpA88)))),((_tmpA8C->f5=(
(_tmpA87="Set the output file name to <file>",_tag_dyneither(_tmpA87,sizeof(char),35))),_tmpA8C)))))))))))),((_tmp8C4[1]=(
# 1136
(_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92->f1=((_tmpA91="--version",_tag_dyneither(_tmpA91,sizeof(char),10))),((_tmpA92->f2=0,((_tmpA92->f3=((_tmpA90="",_tag_dyneither(_tmpA90,sizeof(char),1))),((_tmpA92->f4=(void*)(
(_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=0,((_tmpA8F.f1=Cyc_print_version,_tmpA8F)))),_tmpA8E)))),((_tmpA92->f5=(
(_tmpA8D="Print version information and exit",_tag_dyneither(_tmpA8D,sizeof(char),35))),_tmpA92)))))))))))),((_tmp8C4[0]=(
# 1133
(_tmpA98=_region_malloc(r,sizeof(*_tmpA98)),((_tmpA98->f1=((_tmpA97="-v",_tag_dyneither(_tmpA97,sizeof(char),3))),((_tmpA98->f2=0,((_tmpA98->f3=((_tmpA96="",_tag_dyneither(_tmpA96,sizeof(char),1))),((_tmpA98->f4=(void*)(
(_tmpA94=_region_malloc(r,sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=3,((_tmpA95.f1=& Cyc_v_r,_tmpA95)))),_tmpA94)))),((_tmpA98->f5=(
(_tmpA93="Print compilation stages verbosely",_tag_dyneither(_tmpA93,sizeof(char),35))),_tmpA98)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8C4,sizeof(struct _tuple24*),78)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1371
const char*_tmpA99;struct _dyneither_ptr optstring=(_tmpA99="Options:",_tag_dyneither(_tmpA99,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp165);
# 1376
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1384
char*_tmpA9A;struct _dyneither_ptr cyclone_exec_prefix=(_tmpA9A=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA9A,sizeof(char),_get_zero_arr_size_char((void*)_tmpA9A,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpA9D;struct Cyc_List_List*_tmpA9C;Cyc_cyclone_exec_path=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=cyclone_exec_prefix,_tmpA9D)))),((_tmpA9C->tl=Cyc_cyclone_exec_path,_tmpA9C))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpAA3;const char*_tmpAA2;struct Cyc_List_List*_tmpAA1;Cyc_cyclone_exec_path=(
(_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->hd=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpAA2="cyc-lib",_tag_dyneither(_tmpAA2,sizeof(char),8)))),_tmpAA3)))),((_tmpAA1->tl=Cyc_cyclone_exec_path,_tmpAA1))))));}
# 1392
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1400
const char*_tmpAA4;struct _dyneither_ptr _tmp16C=Cyc_do_find(cyclone_arch_path,((_tmpAA4="cycspecs",_tag_dyneither(_tmpAA4,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpAA8;void*_tmpAA7[1];struct Cyc_String_pa_PrintArg_struct _tmpAA6;(_tmpAA6.tag=0,((_tmpAA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16C),((_tmpAA7[0]=& _tmpAA6,Cyc_fprintf(Cyc_stderr,((_tmpAA8="Reading from specs file %s\n",_tag_dyneither(_tmpAA8,sizeof(char),28))),_tag_dyneither(_tmpAA7,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp170=Cyc_read_specs(_tmp16C);
struct _dyneither_ptr _tmp171=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp172;_push_handler(& _tmp172);{int _tmp174=0;if(setjmp(_tmp172.handler))_tmp174=1;if(!_tmp174){
{struct _dyneither_ptr _tmp175=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp170,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp171=Cyc_split_specs(_tmp175);}
# 1405
;_pop_handler();}else{void*_tmp173=(void*)_exn_thrown;void*_tmp178=_tmp173;void*_tmp17A;_LL32: {struct Cyc_Core_Not_found_exn_struct*_tmp179=(struct Cyc_Core_Not_found_exn_struct*)_tmp178;if(_tmp179->tag != Cyc_Core_Not_found)goto _LL34;}_LL33:
# 1408
 goto _LL31;_LL34: _tmp17A=_tmp178;_LL35:(void)_throw(_tmp17A);_LL31:;}};}
# 1410
if((struct _dyneither_ptr*)_tmp171.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp171);}
# 1415
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp166);{
const char*_tmpAA9;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpAA9="",_tag_dyneither(_tmpAA9,sizeof(char),1))))== 0){
# 1433 "cyclone.cyc"
const char*_tmp17C=Cyc_produce_dependencies?"":" -E";
const char*_tmpAB1;void*_tmpAB0[3];struct Cyc_String_pa_PrintArg_struct _tmpAAF;const char*_tmpAAE;struct Cyc_String_pa_PrintArg_struct _tmpAAD;struct Cyc_String_pa_PrintArg_struct _tmpAAC;Cyc_set_cpp((struct _dyneither_ptr)((_tmpAAC.tag=0,((_tmpAAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16C),((_tmpAAD.tag=0,((_tmpAAD.f1=(struct _dyneither_ptr)((_tmpAAE=_tmp17C,_tag_dyneither(_tmpAAE,sizeof(char),_get_zero_arr_size_char((void*)_tmpAAE,1)))),((_tmpAAF.tag=0,((_tmpAAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAB0[0]=& _tmpAAF,((_tmpAB0[1]=& _tmpAAD,((_tmpAB0[2]=& _tmpAAC,Cyc_aprintf(((_tmpAB1="%s -w -x c%s -specs %s",_tag_dyneither(_tmpAB1,sizeof(char),23))),_tag_dyneither(_tmpAB0,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1437
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpAB4;void*_tmpAB3;(_tmpAB3=0,Cyc_fprintf(Cyc_stderr,((_tmpAB4="missing file\n",_tag_dyneither(_tmpAB4,sizeof(char),14))),_tag_dyneither(_tmpAB3,sizeof(void*),0)));}
exit(1);}
# 1446
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpAB5;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAB5="cyc_setjmp.h",_tag_dyneither(_tmpAB5,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAB6;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAB6="cyc_include.h",_tag_dyneither(_tmpAB6,sizeof(char),14))));}
# 1452
{struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp36B != 0;_tmp36B=_tmp36B->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp36B->hd));
if(Cyc_compile_failure){int _tmp36C=1;_npop_handler(0);return _tmp36C;}}}
# 1457
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp36D=0;_npop_handler(0);return _tmp36D;}
# 1463
if(Cyc_ccargs == 0){int _tmp36E=0;_npop_handler(0);return _tmp36E;}
# 1466
{const char*_tmpAB7;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAB7="-L",_tag_dyneither(_tmpAB7,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1468
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAB8;struct _dyneither_ptr _tmp370=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAB8=" ",_tag_dyneither(_tmpAB8,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpABA;struct Cyc_List_List*_tmpAB9;struct _dyneither_ptr _tmp371=Cyc_str_sepstr(((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAB9->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAB9)))))),((_tmpABA=" ",_tag_dyneither(_tmpABA,sizeof(char),2))));
# 1474
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpABC;const char*_tmpABB;int _tmp372=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1479
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpABC=".a",_tag_dyneither(_tmpABC,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpABB=".lib",_tag_dyneither(_tmpABB,sizeof(char),5))));
if(_tmp372){
stdlib=0;{
const char*_tmpABD;stdlib_string=(struct _dyneither_ptr)((_tmpABD="",_tag_dyneither(_tmpABD,sizeof(char),1)));};}else{
# 1487
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpABE;libcyc_flag=((_tmpABE="-lcyc_a",_tag_dyneither(_tmpABE,sizeof(char),8)));}
{const char*_tmpABF;nogc_filename=((_tmpABF="nogc_a.a",_tag_dyneither(_tmpABF,sizeof(char),9)));}{
const char*_tmpAC0;runtime_filename=((_tmpAC0="runtime_cyc_a.o",_tag_dyneither(_tmpAC0,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpAC1;libcyc_flag=((_tmpAC1="-lcyc_nocheck",_tag_dyneither(_tmpAC1,sizeof(char),14)));}
{const char*_tmpAC2;nogc_filename=((_tmpAC2="nogc.a",_tag_dyneither(_tmpAC2,sizeof(char),7)));}{
const char*_tmpAC3;runtime_filename=((_tmpAC3="runtime_cyc.o",_tag_dyneither(_tmpAC3,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpAC4;libcyc_flag=((_tmpAC4="-lcyc_pg",_tag_dyneither(_tmpAC4,sizeof(char),9)));}
{const char*_tmpAC5;runtime_filename=((_tmpAC5="runtime_cyc_pg.o",_tag_dyneither(_tmpAC5,sizeof(char),17)));}{
const char*_tmpAC6;nogc_filename=((_tmpAC6="nogc_pg.a",_tag_dyneither(_tmpAC6,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1505
{const char*_tmpAC7;libcyc_flag=((_tmpAC7="-lcycboot",_tag_dyneither(_tmpAC7,sizeof(char),10)));}
{const char*_tmpAC8;nogc_filename=((_tmpAC8="nogc.a",_tag_dyneither(_tmpAC8,sizeof(char),7)));}{
const char*_tmpAC9;runtime_filename=((_tmpAC9="runtime_cyc.o",_tag_dyneither(_tmpAC9,sizeof(char),14)));};}else{
# 1509
{const char*_tmpACA;libcyc_flag=((_tmpACA="-lcyc",_tag_dyneither(_tmpACA,sizeof(char),6)));}
{const char*_tmpACB;nogc_filename=((_tmpACB="nogc.a",_tag_dyneither(_tmpACB,sizeof(char),7)));}{
const char*_tmpACC;runtime_filename=((_tmpACC="runtime_cyc.o",_tag_dyneither(_tmpACC,sizeof(char),14)));};}}}}
# 1513
{const char*_tmpACD;gc_filename=((_tmpACD="gc.a",_tag_dyneither(_tmpACD,sizeof(char),5)));}{
# 1515
struct _dyneither_ptr _tmp384=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp385=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1522
stdlib=0;{
const char*_tmpAD3;void*_tmpAD2[3];struct Cyc_String_pa_PrintArg_struct _tmpAD1;struct Cyc_String_pa_PrintArg_struct _tmpAD0;struct Cyc_String_pa_PrintArg_struct _tmpACF;stdlib_string=(struct _dyneither_ptr)((_tmpACF.tag=0,((_tmpACF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp384),((_tmpAD0.tag=0,((_tmpAD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpAD1.tag=0,((_tmpAD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp385),((_tmpAD2[0]=& _tmpAD1,((_tmpAD2[1]=& _tmpAD0,((_tmpAD2[2]=& _tmpACF,Cyc_aprintf(((_tmpAD3=" %s %s %s",_tag_dyneither(_tmpAD3,sizeof(char),10))),_tag_dyneither(_tmpAD2,sizeof(void*),3))))))))))))))))))));};};}
# 1526
if(Cyc_ic_r){struct _handler_cons _tmp38B;_push_handler(& _tmp38B);{int _tmp38D=0;if(setjmp(_tmp38B.handler))_tmp38D=1;if(!_tmp38D){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp372){
struct Cyc_List_List*_tmpAD4;_tmp38E=((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->hd=Cyc_final_strptr,((_tmpAD4->tl=_tmp38E,_tmpAD4))))));}{
# 1534
struct Cyc_Interface_I*_tmp390=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp38E,_tmp38E);
if(_tmp390 == 0){
{const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,Cyc_fprintf(Cyc_stderr,((_tmpAD7="Error: interfaces incompatible\n",_tag_dyneither(_tmpAD7,sizeof(char),32))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp393=1;_npop_handler(1);return _tmp393;};}
# 1541
if(_tmp372){
if(Cyc_output_file != 0){
const char*_tmpADB;void*_tmpADA[1];struct Cyc_String_pa_PrintArg_struct _tmpAD9;struct _dyneither_ptr _tmp394=(_tmpAD9.tag=0,((_tmpAD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpADA[0]=& _tmpAD9,Cyc_aprintf(((_tmpADB="%s.cycio",_tag_dyneither(_tmpADB,sizeof(char),9))),_tag_dyneither(_tmpADA,sizeof(void*),1)))))));
const char*_tmpADD;const char*_tmpADC;struct Cyc___cycFILE*_tmp395=Cyc_try_file_open((struct _dyneither_ptr)_tmp394,((_tmpADC="wb",_tag_dyneither(_tmpADC,sizeof(char),3))),((_tmpADD="interface object file",_tag_dyneither(_tmpADD,sizeof(char),22))));
if(_tmp395 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp396=1;_npop_handler(1);return _tmp396;};}
# 1550
Cyc_Interface_save(_tmp390,_tmp395);
Cyc_file_close(_tmp395);}}else{
# 1554
const char*_tmpAE2;const char*_tmpAE1;struct _tuple20*_tmpAE0;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp390,(
(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->f1=((_tmpAE1="empty interface",_tag_dyneither(_tmpAE1,sizeof(char),16))),((_tmpAE0->f2=((_tmpAE2="global interface",_tag_dyneither(_tmpAE2,sizeof(char),17))),_tmpAE0)))))))){
{const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,Cyc_fprintf(Cyc_stderr,((_tmpAE5="Error: some objects are still undefined\n",_tag_dyneither(_tmpAE5,sizeof(char),41))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A1=1;_npop_handler(1);return _tmp3A1;};}}};};
# 1527
;_pop_handler();}else{void*_tmp38C=(void*)_exn_thrown;void*_tmp3A3=_tmp38C;void*_tmp3A4;void*_tmp3A5;_LL37: _tmp3A4=_tmp3A3;_LL38:
# 1564
{const char*_tmpAEF;void*_tmpAEE[3];const char*_tmpAED;struct Cyc_String_pa_PrintArg_struct _tmpAEC;const char*_tmpAEB;struct Cyc_String_pa_PrintArg_struct _tmpAEA;struct Cyc_Int_pa_PrintArg_struct _tmpAE9;(_tmpAE9.tag=1,((_tmpAE9.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpAEA.tag=0,((_tmpAEA.f1=(struct _dyneither_ptr)((_tmpAEB=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpAEB,sizeof(char),_get_zero_arr_size_char((void*)_tmpAEB,1)))),((_tmpAEC.tag=0,((_tmpAEC.f1=(struct _dyneither_ptr)((_tmpAED=Cyc_Core_get_exn_name(_tmp3A4),_tag_dyneither(_tmpAED,sizeof(char),_get_zero_arr_size_char((void*)_tmpAED,1)))),((_tmpAEE[0]=& _tmpAEC,((_tmpAEE[1]=& _tmpAEA,((_tmpAEE[2]=& _tmpAE9,Cyc_fprintf(Cyc_stderr,((_tmpAEF="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpAEF,sizeof(char),131))),_tag_dyneither(_tmpAEE,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3AD=1;_npop_handler(0);return _tmp3AD;};_LL39: _tmp3A5=_tmp3A3;_LL3A:(void)_throw(_tmp3A5);_LL36:;}};}{
# 1570
const char*_tmpAF0;struct _dyneither_ptr outfile_str=(_tmpAF0="",_tag_dyneither(_tmpAF0,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpAF4;void*_tmpAF3[1];struct Cyc_String_pa_PrintArg_struct _tmpAF2;outfile_str=(struct _dyneither_ptr)((_tmpAF2.tag=0,((_tmpAF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAF3[0]=& _tmpAF2,Cyc_aprintf(((_tmpAF4=" -o %s",_tag_dyneither(_tmpAF4,sizeof(char),7))),_tag_dyneither(_tmpAF3,sizeof(void*),1))))))));}{
# 1574
const char*_tmpAFC;void*_tmpAFB[5];struct Cyc_String_pa_PrintArg_struct _tmpAFA;struct Cyc_String_pa_PrintArg_struct _tmpAF9;struct Cyc_String_pa_PrintArg_struct _tmpAF8;struct Cyc_String_pa_PrintArg_struct _tmpAF7;struct Cyc_String_pa_PrintArg_struct _tmpAF6;struct _dyneither_ptr _tmp3B1=
(_tmpAF6.tag=0,((_tmpAF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp371),((_tmpAF7.tag=0,((_tmpAF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpAF8.tag=0,((_tmpAF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp370),((_tmpAF9.tag=0,((_tmpAF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAFA.tag=0,((_tmpAFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAFB[0]=& _tmpAFA,((_tmpAFB[1]=& _tmpAF9,((_tmpAFB[2]=& _tmpAF8,((_tmpAFB[3]=& _tmpAF7,((_tmpAFB[4]=& _tmpAF6,Cyc_aprintf(((_tmpAFC="%s %s %s%s%s",_tag_dyneither(_tmpAFC,sizeof(char),13))),_tag_dyneither(_tmpAFB,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1577
if(Cyc_v_r){{const char*_tmpB00;void*_tmpAFF[1];struct Cyc_String_pa_PrintArg_struct _tmpAFE;(_tmpAFE.tag=0,((_tmpAFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B1),((_tmpAFF[0]=& _tmpAFE,Cyc_fprintf(Cyc_stderr,((_tmpB00="%s\n",_tag_dyneither(_tmpB00,sizeof(char),4))),_tag_dyneither(_tmpAFF,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3B1,sizeof(char),1))!= 0){
{const char*_tmpB03;void*_tmpB02;(_tmpB02=0,Cyc_fprintf(Cyc_stderr,((_tmpB03="Error: C compiler failed\n",_tag_dyneither(_tmpB03,sizeof(char),26))),_tag_dyneither(_tmpB02,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3B7=1;_npop_handler(0);return _tmp3B7;};}
# 1584
Cyc_remove_cfiles();{
# 1586
int _tmp3B8=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3B8;};};};};};
# 1101 "cyclone.cyc"
;_pop_region(r);};}
