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
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 324
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 352
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 379
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1172 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1174
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple16 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 76 "parse_tab.h"
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
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();
# 87
extern int Cyc_Tcutil_warn_alias_coerce;
# 90
extern int Cyc_Tcutil_warn_region_coerce;
# 342 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple18{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple18 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261 "cf_flowinfo.h"
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
struct _dyneither_ptr*_tmp3D7;struct Cyc_List_List*_tmp3D6;Cyc_ccargs=((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6->hd=((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7[0]=s,_tmp3D7)))),((_tmp3D6->tl=Cyc_ccargs,_tmp3D6))))));}
# 99
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3D8;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D8="vc",_tag_dyneither(_tmp3D8,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3D9;Cyc_add_ccarg(((_tmp3D9="-DVC_C",_tag_dyneither(_tmp3D9,sizeof(char),7))));};}else{
# 104
const char*_tmp3DA;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3DA="gcc",_tag_dyneither(_tmp3DA,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3DB;Cyc_add_ccarg(((_tmp3DB="-DGCC_C",_tag_dyneither(_tmp3DB,sizeof(char),8))));};}}}
# 112
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 120
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3DC;Cyc_output_file=((_tmp3DC=_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC[0]=s,_tmp3DC))));}
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
struct _dyneither_ptr*_tmp3DF;struct Cyc_List_List*_tmp3DE;Cyc_cppargs=((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE->hd=((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF[0]=s,_tmp3DF)))),((_tmp3DE->tl=Cyc_cppargs,_tmp3DE))))));}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3E3;void*_tmp3E2[1];struct Cyc_String_pa_PrintArg_struct _tmp3E1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E1.tag=0,((_tmp3E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E2[0]=& _tmp3E1,Cyc_aprintf(((_tmp3E3="-iprefix %s",_tag_dyneither(_tmp3E3,sizeof(char),12))),_tag_dyneither(_tmp3E2,sizeof(void*),1)))))))));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3E7;void*_tmp3E6[1];struct Cyc_String_pa_PrintArg_struct _tmp3E5;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E5.tag=0,((_tmp3E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E6[0]=& _tmp3E5,Cyc_aprintf(((_tmp3E7="-iwithprefix %s",_tag_dyneither(_tmp3E7,sizeof(char),16))),_tag_dyneither(_tmp3E6,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3EB;void*_tmp3EA[1];struct Cyc_String_pa_PrintArg_struct _tmp3E9;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E9.tag=0,((_tmp3E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3EA[0]=& _tmp3E9,Cyc_aprintf(((_tmp3EB="-iwithprefixbefore %s",_tag_dyneither(_tmp3EB,sizeof(char),22))),_tag_dyneither(_tmp3EA,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3EF;void*_tmp3EE[1];struct Cyc_String_pa_PrintArg_struct _tmp3ED;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3EE[0]=& _tmp3ED,Cyc_aprintf(((_tmp3EF="-isystem %s",_tag_dyneither(_tmp3EF,sizeof(char),12))),_tag_dyneither(_tmp3EE,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3F3;void*_tmp3F2[1];struct Cyc_String_pa_PrintArg_struct _tmp3F1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3F1.tag=0,((_tmp3F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F2[0]=& _tmp3F1,Cyc_aprintf(((_tmp3F3="-idirafter %s",_tag_dyneither(_tmp3F3,sizeof(char),14))),_tag_dyneither(_tmp3F2,sizeof(void*),1)))))))));}
# 167
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
Cyc_toc_r=1;
Cyc_target_arch=s;}
# 174
static void Cyc_print_version(){
{const char*_tmp3F7;void*_tmp3F6[1];struct Cyc_String_pa_PrintArg_struct _tmp3F5;(_tmp3F5.tag=0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3F6[0]=& _tmp3F5,Cyc_printf(((_tmp3F7="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3F7,sizeof(char),34))),_tag_dyneither(_tmp3F6,sizeof(void*),1)))))));}
{const char*_tmp3FC;void*_tmp3FB[2];struct Cyc_String_pa_PrintArg_struct _tmp3FA;struct Cyc_String_pa_PrintArg_struct _tmp3F9;(_tmp3F9.tag=0,((_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3FA.tag=0,((_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3FB[0]=& _tmp3FA,((_tmp3FB[1]=& _tmp3F9,Cyc_printf(((_tmp3FC="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3FC,sizeof(char),39))),_tag_dyneither(_tmp3FB,sizeof(void*),2)))))))))))));}
{const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_PrintArg_struct _tmp3FE;(_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3FF[0]=& _tmp3FE,Cyc_printf(((_tmp400="Standard library directory: %s\n",_tag_dyneither(_tmp400,sizeof(char),32))),_tag_dyneither(_tmp3FF,sizeof(void*),1)))))));}
{const char*_tmp404;void*_tmp403[1];struct Cyc_String_pa_PrintArg_struct _tmp402;(_tmp402.tag=0,((_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp403[0]=& _tmp402,Cyc_printf(((_tmp404="Standard include directory: %s\n",_tag_dyneither(_tmp404,sizeof(char),32))),_tag_dyneither(_tmp403,sizeof(void*),1)))))));}
exit(0);}
# 183
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp26 <= 4)return 0;else{
const char*_tmp405;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp405=".cyc",_tag_dyneither(_tmp405,sizeof(char),5))))== 0;}}
# 189
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{
struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp28 - 2);
struct _dyneither_ptr*_tmp408;struct Cyc_List_List*_tmp407;Cyc_cyclone_exec_path=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->hd=((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=_tmp29,_tmp408)))),((_tmp407->tl=Cyc_cyclone_exec_path,_tmp407))))));};}
# 202
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp409;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp409="-lxml",_tag_dyneither(_tmp409,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 208
const char*_tmp40A;Cyc_add_ccarg(((_tmp40A="-lxml_a",_tag_dyneither(_tmp40A,sizeof(char),8))));}}else{
# 210
struct _dyneither_ptr*_tmp40D;struct Cyc_List_List*_tmp40C;Cyc_libargs=((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C->hd=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D[0]=s,_tmp40D)))),((_tmp40C->tl=Cyc_libargs,_tmp40C))))));}}
# 213
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 217
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp40E;Cyc_add_ccarg(((_tmp40E="-save-temps",_tag_dyneither(_tmp40E,sizeof(char),12))));};}
# 222
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp40F;Cyc_add_cpparg(((_tmp40F="-M",_tag_dyneither(_tmp40F,sizeof(char),3))));};}
# 229
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp410;Cyc_dependencies_target=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=s,_tmp410))));}
# 234
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp411;Cyc_add_ccarg(((_tmp411="-c",_tag_dyneither(_tmp411,sizeof(char),3))));};}
# 239
static void Cyc_set_nocppprecomp(){
{const char*_tmp412;Cyc_add_cpparg(((_tmp412="-no-cpp-precomp",_tag_dyneither(_tmp412,sizeof(char),16))));}{
const char*_tmp413;Cyc_add_ccarg(((_tmp413="-no-cpp-precomp",_tag_dyneither(_tmp413,sizeof(char),16))));};}
# 244
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 248
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 251
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 255
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
{const char*_tmp414;Cyc_add_ccarg(((_tmp414="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp414,sizeof(char),18))));}
Cyc_set_elim_se();}
# 260
static void Cyc_set_noboundschecks(){
const char*_tmp415;Cyc_add_ccarg(((_tmp415="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp415,sizeof(char),23))));}
# 263
static void Cyc_set_nonullchecks(){
const char*_tmp416;Cyc_add_ccarg(((_tmp416="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp416,sizeof(char),21))));}
# 266
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 272
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp417;Cyc_add_ccarg(((_tmp417="-DNO_CYC_PREFIX",_tag_dyneither(_tmp417,sizeof(char),16))));};}
# 277
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp418;Cyc_add_ccarg(((_tmp418="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp418,sizeof(char),21))));}{
const char*_tmp419;Cyc_add_cpparg(((_tmp419="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp419,sizeof(char),21))));};}
# 283
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp41A;Cyc_add_ccarg(((_tmp41A="-pg",_tag_dyneither(_tmp41A,sizeof(char),4))));};}
# 288
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp41B;Cyc_add_ccarg(((_tmp41B="-S",_tag_dyneither(_tmp41B,sizeof(char),3))));};}
# 293
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;}
# 302
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 306
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp41C;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41C="cyc",_tag_dyneither(_tmp41C,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp41D;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41D="none",_tag_dyneither(_tmp41D,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 314
const char*_tmp421;void*_tmp420[1];struct Cyc_String_pa_PrintArg_struct _tmp41F;(_tmp41F.tag=0,((_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp420[0]=& _tmp41F,Cyc_fprintf(Cyc_stderr,((_tmp421="Input type '%s' not supported\n",_tag_dyneither(_tmp421,sizeof(char),31))),_tag_dyneither(_tmp420,sizeof(void*),1)))))));}}}
# 318
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 324
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
return(struct _dyneither_ptr)_tmp45;}
# 330
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 332
static void Cyc_add_other(struct _dyneither_ptr s){
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 335
{struct _dyneither_ptr*_tmp424;struct Cyc_List_List*_tmp423;Cyc_cyclone_files=((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->hd=((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424[0]=s,_tmp424)))),((_tmp423->tl=Cyc_cyclone_files,_tmp423))))));}{
# 338
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp425;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp425=".c",_tag_dyneither(_tmp425,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 342
Cyc_add_ccarg(s);}}
# 345
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 356
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4B=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 364
if(spec_file == 0){
{const char*_tmp429;void*_tmp428[1];struct Cyc_String_pa_PrintArg_struct _tmp427;(_tmp427.tag=0,((_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp428[0]=& _tmp427,Cyc_fprintf(Cyc_stderr,((_tmp429="Error opening spec file %s\n",_tag_dyneither(_tmp429,sizeof(char),28))),_tag_dyneither(_tmp428,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 370
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp42E;void*_tmp42D[2];struct Cyc_String_pa_PrintArg_struct _tmp42C;struct Cyc_Int_pa_PrintArg_struct _tmp42B;(_tmp42B.tag=1,((_tmp42B.f1=(unsigned long)c,((_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42D[0]=& _tmp42C,((_tmp42D[1]=& _tmp42B,Cyc_fprintf(Cyc_stderr,((_tmp42E="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp42E,sizeof(char),55))),_tag_dyneither(_tmp42D,sizeof(void*),2)))))))))))));}
# 379
Cyc_fflush(Cyc_stderr);}
# 381
goto CLEANUP_AND_RETURN;}
# 383
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp432;void*_tmp431[1];struct Cyc_String_pa_PrintArg_struct _tmp430;(_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp431[0]=& _tmp430,Cyc_fprintf(Cyc_stderr,((_tmp432="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp432,sizeof(char),44))),_tag_dyneither(_tmp431,sizeof(void*),1)))))));}
# 391
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 394
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 398
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp436;void*_tmp435[1];struct Cyc_String_pa_PrintArg_struct _tmp434;(_tmp434.tag=0,((_tmp434.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp435[0]=& _tmp434,Cyc_fprintf(Cyc_stderr,((_tmp436="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp436,sizeof(char),50))),_tag_dyneither(_tmp435,sizeof(void*),1)))))));}
# 404
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 408
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 413
if(c == '*'){
# 415
{struct _tuple21*_tmp443;struct _dyneither_ptr*_tmp442;const char*_tmp441;struct _dyneither_ptr*_tmp440;struct Cyc_List_List*_tmp43F;_tmp4B=((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F->hd=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->f1=((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp442)))),((_tmp443->f2=(
(_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp441="",_tag_dyneither(_tmp441,sizeof(char),1)))),_tmp440)))),_tmp443)))))),((_tmp43F->tl=_tmp4B,_tmp43F))))));}
goto JUST_AFTER_STAR;}
# 420
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp448;void*_tmp447[2];struct Cyc_String_pa_PrintArg_struct _tmp446;struct Cyc_String_pa_PrintArg_struct _tmp445;(_tmp445.tag=0,((_tmp445.f1=(struct _dyneither_ptr)
# 430
_tag_dyneither(strname,sizeof(char),256),((_tmp446.tag=0,((_tmp446.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp447[0]=& _tmp446,((_tmp447[1]=& _tmp445,Cyc_fprintf(Cyc_stderr,((_tmp448="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp448,sizeof(char),50))),_tag_dyneither(_tmp447,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 436
{struct _tuple21*_tmp451;struct _dyneither_ptr*_tmp450;struct _dyneither_ptr*_tmp44F;struct Cyc_List_List*_tmp44E;_tmp4B=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E->hd=((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451->f1=((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp450)))),((_tmp451->f2=(
(_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp44F)))),_tmp451)))))),((_tmp44E->tl=_tmp4B,_tmp44E))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 441
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp45B(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp45A,unsigned int*_tmp459,struct _dyneither_ptr**_tmp457){for(*_tmp45A=0;*_tmp45A < *_tmp459;(*_tmp45A)++){(*_tmp457)[*_tmp45A]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp45A];}}
# 447
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp67=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp66 > 4096)goto DONE;
while(1){
# 457
while(1){
if(i >= _tmp66)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 463
j=0;
# 468
while(1){
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 474
++ i;
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 481
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 484
++ i;}
# 486
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 489
struct _dyneither_ptr*_tmp454;struct Cyc_List_List*_tmp453;_tmp67=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453->hd=((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp454)))),((_tmp453->tl=_tmp67,_tmp453))))));};}
# 491
DONE:
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
{struct Cyc_List_List*_tmp455;_tmp67=((_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp455->tl=_tmp67,_tmp455))))));}{
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
unsigned int _tmp45A;unsigned int _tmp459;struct _dyneither_ptr _tmp458;struct _dyneither_ptr*_tmp457;unsigned int _tmp456;struct _dyneither_ptr _tmp6D=(_tmp456=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp457=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp456)),((_tmp458=_tag_dyneither(_tmp457,sizeof(struct _dyneither_ptr),_tmp456),((((_tmp459=_tmp456,_tmp45B(& _tmp6C,& _tmp45A,& _tmp459,& _tmp457))),_tmp458)))))));
return _tmp6D;};};}
# 501
int Cyc_compile_failure=0;
# 503
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;void*_tmp78;_LL1:;_LL2:
# 507
 Cyc_compile_failure=1;
{const char*_tmp460;void*_tmp45F[2];struct Cyc_String_pa_PrintArg_struct _tmp45E;struct Cyc_String_pa_PrintArg_struct _tmp45D;(_tmp45D.tag=0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp45E.tag=0,((_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp45F[0]=& _tmp45E,((_tmp45F[1]=& _tmp45D,Cyc_fprintf(Cyc_stderr,((_tmp460="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp460,sizeof(char),29))),_tag_dyneither(_tmp45F,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp78=_tmp77;_LL4:(void)_throw(_tmp78);_LL0:;}};}
# 514
void CYCALLOCPROFILE_mark(const char*s);
# 516
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 520
const char*_tmp461;struct _dyneither_ptr exn_string=(_tmp461="",_tag_dyneither(_tmp461,sizeof(char),1));
const char*_tmp462;struct _dyneither_ptr explain_string=(_tmp462="",_tag_dyneither(_tmp462,sizeof(char),1));
int other_exn=0;
struct Cyc_Core_Impossible_exn_struct _tmp468;const char*_tmp467;struct Cyc_Core_Impossible_exn_struct*_tmp466;void*ex=(void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((_tmp468.tag=Cyc_Core_Impossible,((_tmp468.f1=((_tmp467="",_tag_dyneither(_tmp467,sizeof(char),1))),_tmp468)))),_tmp466))));
struct Cyc_List_List*_tmp7D=0;
# 526
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 528
{struct _handler_cons _tmp7E;_push_handler(& _tmp7E);{int _tmp80=0;if(setjmp(_tmp7E.handler))_tmp80=1;if(!_tmp80){_tmp7D=f(env,tds);;_pop_handler();}else{void*_tmp7F=(void*)_exn_thrown;void*_tmp82=_tmp7F;struct _dyneither_ptr _tmp84;struct _dyneither_ptr _tmp87;struct _dyneither_ptr _tmp89;void*_tmp8A;void*_tmp8B;_LL6: {struct Cyc_Core_Impossible_exn_struct*_tmp83=(struct Cyc_Core_Impossible_exn_struct*)_tmp82;if(_tmp83->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp84=_tmp83->f1;}}_LL7:
# 531
{const char*_tmp469;exn_string=((_tmp469="Exception Core::Impossible",_tag_dyneither(_tmp469,sizeof(char),27)));}
explain_string=_tmp84;
goto _LL5;_LL8: {struct Cyc_Dict_Absent_exn_struct*_tmp85=(struct Cyc_Dict_Absent_exn_struct*)_tmp82;if(_tmp85->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:
# 535
{const char*_tmp46A;exn_string=((_tmp46A="Exception Dict::Absent",_tag_dyneither(_tmp46A,sizeof(char),23)));}
goto _LL5;_LLA: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp86=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp82;if(_tmp86->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp87=_tmp86->f1;}}_LLB:
# 538
{const char*_tmp46B;exn_string=((_tmp46B="Exception Core::Invalid_argument",_tag_dyneither(_tmp46B,sizeof(char),33)));}
explain_string=_tmp87;
goto _LL5;_LLC: {struct Cyc_Core_Failure_exn_struct*_tmp88=(struct Cyc_Core_Failure_exn_struct*)_tmp82;if(_tmp88->tag != Cyc_Core_Failure)goto _LLE;else{_tmp89=_tmp88->f1;}}_LLD:
# 542
{const char*_tmp46C;exn_string=((_tmp46C="Exception Core::Failure",_tag_dyneither(_tmp46C,sizeof(char),24)));}
explain_string=_tmp89;
goto _LL5;_LLE: _tmp8A=_tmp82;_LLF:
# 546
 ex=_tmp8A;
other_exn=1;
{const char*_tmp46D;exn_string=((_tmp46D="Uncaught exception",_tag_dyneither(_tmp46D,sizeof(char),19)));}
goto _LL5;_LL10: _tmp8B=_tmp82;_LL11:(void)_throw(_tmp8B);_LL5:;}};}
# 551
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
{const char*_tmp46E;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp46E="",_tag_dyneither(_tmp46E,sizeof(char),1))))!= 0){
Cyc_compile_failure=1;{
const char*_tmp474;void*_tmp473[3];struct Cyc_String_pa_PrintArg_struct _tmp472;struct Cyc_String_pa_PrintArg_struct _tmp471;struct Cyc_String_pa_PrintArg_struct _tmp470;(_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp471.tag=0,((_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp473[0]=& _tmp472,((_tmp473[1]=& _tmp471,((_tmp473[2]=& _tmp470,Cyc_fprintf(Cyc_stderr,((_tmp474="\n%s thrown during %s: %s",_tag_dyneither(_tmp474,sizeof(char),25))),_tag_dyneither(_tmp473,sizeof(void*),3)))))))))))))))))));};}}
# 558
if(Cyc_compile_failure){
{const char*_tmp477;void*_tmp476;(_tmp476=0,Cyc_fprintf(Cyc_stderr,((_tmp477="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp477,sizeof(char),22))),_tag_dyneither(_tmp476,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
if(other_exn)
(int)_throw(ex);
return _tmp7D;}else{
# 565
if(Cyc_v_r){
{const char*_tmp47B;void*_tmp47A[1];struct Cyc_String_pa_PrintArg_struct _tmp479;(_tmp479.tag=0,((_tmp479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp47A[0]=& _tmp479,Cyc_fprintf(Cyc_stderr,((_tmp47B="%s completed.\n",_tag_dyneither(_tmp47B,sizeof(char),15))),_tag_dyneither(_tmp47A,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}}
# 570
return _tmp7D;}
# 573
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*_tmpA1=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);
return _tmpA1;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 581
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 583
struct _RegionHandle*_tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct _tuple22 _tmpA2=*tcenv;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
Cyc_Tc_tc(_tmpA3,_tmpA4,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA4,tds);
return tds;}
# 590
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 597
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmpA6;struct Cyc___cycFILE*_tmpA7;struct Cyc___cycFILE*_tmpA8;struct _tuple23*_tmpA5=params;_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;_tmpA8=_tmpA5->f3;{
struct Cyc_Interface_I*_tmpA9=Cyc_Interface_extract(_tmpA6->ae);
if(_tmpA7 == 0)
Cyc_Interface_save(_tmpA9,_tmpA8);else{
# 603
struct Cyc_Interface_I*_tmpAA=Cyc_Interface_parse(_tmpA7);
const char*_tmp480;const char*_tmp47F;struct _tuple20*_tmp47E;if(!Cyc_Interface_is_subinterface(_tmpAA,_tmpA9,((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->f1=((_tmp47F="written interface",_tag_dyneither(_tmp47F,sizeof(char),18))),((_tmp47E->f2=((_tmp480="maximal interface",_tag_dyneither(_tmp480,sizeof(char),18))),_tmp47E))))))))
Cyc_compile_failure=1;else{
# 607
Cyc_Interface_save(_tmpAA,_tmpA8);}}
# 609
return tds;};}
# 612
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 615
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 620
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 622
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 624
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 629
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 632
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_fprintf(Cyc_stderr,((_tmp483="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp483,sizeof(char),53))),_tag_dyneither(_tmp482,sizeof(void*),0)));}
return 1;}{
# 636
const char*_tmp485;const char*_tmp484;struct Cyc___cycFILE*_tmpB0=Cyc_try_file_open(file,((_tmp484="r",_tag_dyneither(_tmp484,sizeof(char),2))),((_tmp485="internal compiler file",_tag_dyneither(_tmp485,sizeof(char),23))));
if(_tmpB0 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpB7=1024;unsigned int i;for(i=0;i < _tmpB7;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpB0);
if(n_read != 1024  && !Cyc_feof(_tmpB0)){
Cyc_compile_failure=1;
{const char*_tmp489;void*_tmp488[1];struct Cyc_String_pa_PrintArg_struct _tmp487;(_tmp487.tag=0,((_tmp487.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp488[0]=& _tmp487,Cyc_fprintf(Cyc_stderr,((_tmp489="\nError: problem copying %s\n",_tag_dyneither(_tmp489,sizeof(char),28))),_tag_dyneither(_tmp488,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 649
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp48D;void*_tmp48C[1];struct Cyc_String_pa_PrintArg_struct _tmp48B;(_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp48C[0]=& _tmp48B,Cyc_fprintf(Cyc_stderr,((_tmp48D="\nError: problem copying %s\n",_tag_dyneither(_tmp48D,sizeof(char),28))),_tag_dyneither(_tmp48C,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 657
Cyc_fclose(_tmpB0);
return 0;};};}
# 662
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpBA=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpBA.expand_typedefs=!Cyc_noexpand_r;
_tmpBA.to_VC=Cyc_Cyclone_tovc_r;
_tmpBA.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpBA.generate_line_directives=Cyc_generate_line_directives_r;
_tmpBA.print_full_evars=Cyc_print_full_evars_r;
_tmpBA.print_all_tvars=Cyc_print_all_tvars_r;
_tmpBA.print_all_kinds=Cyc_print_all_kinds_r;
_tmpBA.print_all_effects=Cyc_print_all_effects_r;
# 673
if(Cyc_inline_functions_r){
const char*_tmp490;void*_tmp48F;(_tmp48F=0,Cyc_fprintf(out_file,((_tmp490="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp490,sizeof(char),27))),_tag_dyneither(_tmp48F,sizeof(void*),0)));}
# 677
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp493;void*_tmp492;(_tmp492=0,Cyc_fprintf(out_file,((_tmp493="#include <setjmp.h>\n",_tag_dyneither(_tmp493,sizeof(char),21))),_tag_dyneither(_tmp492,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 683
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 685
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpBA);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 689
Cyc_Absyndump_set_params(& _tmpBA);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 692
Cyc_fflush(out_file);
return tds;}
# 698
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 706
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpBF=0;
unsigned long _tmpC0=Cyc_strlen((struct _dyneither_ptr)s);
# 711
while(_tmpC0 > 0){
struct _dyneither_ptr _tmpC1=Cyc_strchr(s,c);
if((char*)_tmpC1.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp496;struct Cyc_List_List*_tmp495;_tmpBF=((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->hd=((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496[0]=s,_tmp496)))),((_tmp495->tl=_tmpBF,_tmp495))))));}
break;}else{
# 718
{struct _dyneither_ptr*_tmp499;struct Cyc_List_List*_tmp498;_tmpBF=((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->hd=((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpC1).curr - s.curr)/ sizeof(char))),_tmp499)))),((_tmp498->tl=_tmpBF,_tmp498))))));}
_tmpC0 -=(((struct _dyneither_ptr)_tmpC1).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpC1,sizeof(char),1);}}
# 723
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBF);};}
# 727
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpC6;_push_handler(& _tmpC6);{int _tmpC8=0;if(setjmp(_tmpC6.handler))_tmpC8=1;if(!_tmpC8){{const char*_tmp49A;f=Cyc_file_open(file,((_tmp49A="r",_tag_dyneither(_tmp49A,sizeof(char),2))));};_pop_handler();}else{void*_tmpC7=(void*)_exn_thrown;void*_tmpCB=_tmpC7;void*_tmpCC;_LL13:;_LL14: goto _LL12;_LL15: _tmpCC=_tmpCB;_LL16:(void)_throw(_tmpCC);_LL12:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 735
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp49B;struct _dyneither_ptr tmp=(_tmp49B="",_tag_dyneither(_tmp49B,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpCD=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpCD.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCD)== 0)continue;
{const char*_tmp49C;_tmpCD=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCD,((_tmp49C=":",_tag_dyneither(_tmp49C,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCD,(struct _dyneither_ptr)tmp);}
# 743
return tmp;}
# 748
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpD0=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpD0.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpD0)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpD0,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp49D;return(_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=s,_tmp49D)));}};}
# 756
return 0;}
# 759
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpD2=Cyc_find(dirs,file);
if(_tmpD2 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp4A2;void*_tmp4A1[2];struct Cyc_String_pa_PrintArg_struct _tmp4A0;struct Cyc_String_pa_PrintArg_struct _tmp49F;(_tmp49F.tag=0,((_tmp49F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp4A0.tag=0,((_tmp4A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp4A1[0]=& _tmp4A0,((_tmp4A1[1]=& _tmp49F,Cyc_fprintf(Cyc_stderr,((_tmp4A2="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp4A2,sizeof(char),56))),_tag_dyneither(_tmp4A1,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp4B2;const char*_tmp4B1;void*_tmp4B0[2];struct Cyc_String_pa_PrintArg_struct _tmp4AF;struct Cyc_String_pa_PrintArg_struct _tmp4AE;struct Cyc_Core_Failure_exn_struct*_tmp4AD;(int)_throw((void*)((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4B2.tag=Cyc_Core_Failure,((_tmp4B2.f1=(struct _dyneither_ptr)((_tmp4AE.tag=0,((_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp4AF.tag=0,((_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp4B0[0]=& _tmp4AF,((_tmp4B0[1]=& _tmp4AE,Cyc_aprintf(((_tmp4B1="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp4B1,sizeof(char),56))),_tag_dyneither(_tmp4B0,sizeof(void*),2)))))))))))))),_tmp4B2)))),_tmp4AD)))));};}
# 771
return*_tmpD2;}
# 776
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 778
struct Cyc_List_List*_tmpDD=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp4B5;struct Cyc_List_List*_tmp4B4;_tmpDD=((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4->hd=((_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4B5)))),((_tmp4B4->tl=_tmpDD,_tmp4B4))))));}
# 782
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD);
return _tmpDD;}
# 788
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 790
struct Cyc_List_List*_tmpE0=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp4B6;_tmpE0=((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4B6->tl=_tmpE0,_tmp4B6))))));}{
struct _dyneither_ptr*_tmp4B9;struct Cyc_List_List*_tmp4B8;_tmpE0=((_tmp4B8=_cycalloc(sizeof(*_tmp4B8)),((_tmp4B8->hd=((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4B9)))),((_tmp4B8->tl=_tmpE0,_tmp4B8))))));};}
# 795
_tmpE0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE0);
return _tmpE0;}
# 800
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL17:
 goto _LL18;case '"': _LL18:
 goto _LL19;case ';': _LL19:
 goto _LL1A;case '&': _LL1A:
 goto _LL1B;case '(': _LL1B:
 goto _LL1C;case ')': _LL1C:
 goto _LL1D;case '|': _LL1D:
 goto _LL1E;case '^': _LL1E:
 goto _LL1F;case '<': _LL1F:
 goto _LL20;case '>': _LL20:
 goto _LL21;case '\n': _LL21:
# 815
 goto _LL22;case '\t': _LL22:
 return 1;default: _LL23:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4C2(unsigned int*_tmp4C1,unsigned int*_tmp4C0,char**_tmp4BE){for(*_tmp4C1=0;*_tmp4C1 < *_tmp4C0;(*_tmp4C1)++){(*_tmp4BE)[*_tmp4C1]=(char)'\000';}}
# 822
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpE4=Cyc_strlen((struct _dyneither_ptr)s);
# 826
int _tmpE5=0;
int _tmpE6=0;
{int i=0;for(0;i < _tmpE4;++ i){
char _tmpE7=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpE7 == '\'')++ _tmpE5;else{
if(Cyc_is_other_special(_tmpE7))++ _tmpE6;}}}
# 835
if(_tmpE5 == 0  && _tmpE6 == 0)
return s;
# 839
if(_tmpE5 == 0){
struct _dyneither_ptr*_tmp4BC;struct _dyneither_ptr*_tmp4BB[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4BB[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4BB[1]=((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC[0]=(struct _dyneither_ptr)s,_tmp4BC)))),((_tmp4BB[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BB,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 843
unsigned long _tmpEC=(_tmpE4 + _tmpE5)+ _tmpE6;
unsigned int _tmp4C1;unsigned int _tmp4C0;struct _dyneither_ptr _tmp4BF;char*_tmp4BE;unsigned int _tmp4BD;struct _dyneither_ptr s2=(_tmp4BD=_tmpEC + 1,((_tmp4BE=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4BD + 1)),((_tmp4BF=_tag_dyneither(_tmp4BE,sizeof(char),_tmp4BD + 1),((((_tmp4C0=_tmp4BD,((_tmp4C2(& _tmp4C1,& _tmp4C0,& _tmp4BE),_tmp4BE[_tmp4C0]=(char)0)))),_tmp4BF)))))));
int _tmpED=0;
int _tmpEE=0;
for(0;_tmpED < _tmpE4;++ _tmpED){
char _tmpEF=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpED));
if(_tmpEF == '\''  || Cyc_is_other_special(_tmpEF)){
char _tmp4C5;char _tmp4C4;struct _dyneither_ptr _tmp4C3;(_tmp4C3=_dyneither_ptr_plus(s2,sizeof(char),_tmpEE ++),((_tmp4C4=*((char*)_check_dyneither_subscript(_tmp4C3,sizeof(char),0)),((_tmp4C5='\\',((_get_dyneither_size(_tmp4C3,sizeof(char))== 1  && (_tmp4C4 == '\000'  && _tmp4C5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4C3.curr)=_tmp4C5)))))));}{
char _tmp4C8;char _tmp4C7;struct _dyneither_ptr _tmp4C6;(_tmp4C6=_dyneither_ptr_plus(s2,sizeof(char),_tmpEE ++),((_tmp4C7=*((char*)_check_dyneither_subscript(_tmp4C6,sizeof(char),0)),((_tmp4C8=_tmpEF,((_get_dyneither_size(_tmp4C6,sizeof(char))== 1  && (_tmp4C7 == '\000'  && _tmp4C8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4C6.curr)=_tmp4C8)))))));};}
# 853
return(struct _dyneither_ptr)s2;};}
# 855
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4C9;return(_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9[0]=Cyc_sh_escape_string(*sp),_tmp4C9)));}
# 859
static void Cyc_process_file(struct _dyneither_ptr filename){
# 861
struct _dyneither_ptr _tmpFB=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4CA;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp4CA=".cyp",_tag_dyneither(_tmp4CA,sizeof(char),5))));
const char*_tmp4CB;struct _dyneither_ptr _tmpFD=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp4CB=".cyci",_tag_dyneither(_tmp4CB,sizeof(char),6))));
const char*_tmp4CC;struct _dyneither_ptr _tmpFE=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp4CC=".cycio",_tag_dyneither(_tmp4CC,sizeof(char),7))));
const char*_tmp4CD;struct _dyneither_ptr _tmpFF=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp4CD=".c",_tag_dyneither(_tmp4CD,sizeof(char),3))));
# 868
if(Cyc_v_r){const char*_tmp4D1;void*_tmp4D0[1];struct Cyc_String_pa_PrintArg_struct _tmp4CF;(_tmp4CF.tag=0,((_tmp4CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4D0[0]=& _tmp4CF,Cyc_fprintf(Cyc_stderr,((_tmp4D1="Compiling %s\n",_tag_dyneither(_tmp4D1,sizeof(char),14))),_tag_dyneither(_tmp4D0,sizeof(void*),1)))))));}{
# 871
const char*_tmp4D3;const char*_tmp4D2;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4D2="r",_tag_dyneither(_tmp4D2,sizeof(char),2))),((_tmp4D3="input file",_tag_dyneither(_tmp4D3,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 878
const char*_tmp4DA;struct _dyneither_ptr*_tmp4D9;const char*_tmp4D8;struct Cyc_List_List*_tmp4D7;struct _dyneither_ptr _tmp103=
Cyc_str_sepstr(((_tmp4D7=_cycalloc(sizeof(*_tmp4D7)),((_tmp4D7->hd=((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=(struct _dyneither_ptr)((_tmp4D8="",_tag_dyneither(_tmp4D8,sizeof(char),1))),_tmp4D9)))),((_tmp4D7->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4D7)))))),(
(_tmp4DA=" ",_tag_dyneither(_tmp4DA,sizeof(char),2))));
# 888
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmp104=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4DB;_tmp104=Cyc_add_subdir(_tmp104,((_tmp4DB="include",_tag_dyneither(_tmp4DB,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4DE;struct Cyc_List_List*_tmp4DD;_tmp104=((_tmp4DD=_cycalloc(sizeof(*_tmp4DD)),((_tmp4DD->hd=((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE[0]=def_inc_path,_tmp4DE)))),((_tmp4DD->tl=_tmp104,_tmp4DD))))));}{
# 894
char*_tmp108=getenv("CYCLONE_INCLUDE_PATH");
if(_tmp108 != 0){
char*_tmp4DF;_tmp104=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4DF=_tmp108,_tag_dyneither(_tmp4DF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4DF,1)))),':'),_tmp104);}{
# 899
const char*_tmp4E6;struct _dyneither_ptr*_tmp4E5;const char*_tmp4E4;struct Cyc_List_List*_tmp4E3;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4E3=_cycalloc(sizeof(*_tmp4E3)),((_tmp4E3->hd=((_tmp4E5=_cycalloc(sizeof(*_tmp4E5)),((_tmp4E5[0]=(struct _dyneither_ptr)((_tmp4E4="",_tag_dyneither(_tmp4E4,sizeof(char),1))),_tmp4E5)))),((_tmp4E3->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp104),_tmp4E3)))))),(
(_tmp4E6=" -I",_tag_dyneither(_tmp4E6,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_String_pa_PrintArg_struct _tmp4E8;ofile_string=(struct _dyneither_ptr)((_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4E9[0]=& _tmp4E8,Cyc_aprintf(((_tmp4EA=" > %s",_tag_dyneither(_tmp4EA,sizeof(char),6))),_tag_dyneither(_tmp4E9,sizeof(void*),1))))))));}else{
# 908
const char*_tmp4EB;ofile_string=((_tmp4EB="",_tag_dyneither(_tmp4EB,sizeof(char),1)));}}else{
# 910
const char*_tmp4EF;void*_tmp4EE[1];struct Cyc_String_pa_PrintArg_struct _tmp4ED;ofile_string=(struct _dyneither_ptr)((_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpFC)),((_tmp4EE[0]=& _tmp4ED,Cyc_aprintf(((_tmp4EF=" > %s",_tag_dyneither(_tmp4EF,sizeof(char),6))),_tag_dyneither(_tmp4EE,sizeof(void*),1))))))));}{
# 912
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 916
if(Cyc_dependencies_target == 0){
# 920
const char*_tmp4F0;fixup_string=((_tmp4F0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4F0,sizeof(char),35)));}else{
# 925
const char*_tmp4F4;void*_tmp4F3[1];struct Cyc_String_pa_PrintArg_struct _tmp4F2;fixup_string=(struct _dyneither_ptr)(
(_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4F3[0]=& _tmp4F2,Cyc_aprintf(((_tmp4F4=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4F4,sizeof(char),29))),_tag_dyneither(_tmp4F3,sizeof(void*),1))))))));}}else{
# 929
const char*_tmp4F5;fixup_string=((_tmp4F5="",_tag_dyneither(_tmp4F5,sizeof(char),1)));}{
# 931
const char*_tmp4FE;void*_tmp4FD[6];struct Cyc_String_pa_PrintArg_struct _tmp4FC;struct Cyc_String_pa_PrintArg_struct _tmp4FB;struct Cyc_String_pa_PrintArg_struct _tmp4FA;struct Cyc_String_pa_PrintArg_struct _tmp4F9;struct Cyc_String_pa_PrintArg_struct _tmp4F8;struct Cyc_String_pa_PrintArg_struct _tmp4F7;struct _dyneither_ptr _tmp116=(_tmp4F7.tag=0,((_tmp4F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4F8.tag=0,((_tmp4F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4F9.tag=0,((_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 934
Cyc_sh_escape_string(filename)),((_tmp4FA.tag=0,((_tmp4FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp103),((_tmp4FC.tag=0,((_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4FD[0]=& _tmp4FC,((_tmp4FD[1]=& _tmp4FB,((_tmp4FD[2]=& _tmp4FA,((_tmp4FD[3]=& _tmp4F9,((_tmp4FD[4]=& _tmp4F8,((_tmp4FD[5]=& _tmp4F7,Cyc_aprintf(((_tmp4FE="%s %s%s %s%s%s",_tag_dyneither(_tmp4FE,sizeof(char),15))),_tag_dyneither(_tmp4FD,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 937
if(Cyc_v_r){const char*_tmp502;void*_tmp501[1];struct Cyc_String_pa_PrintArg_struct _tmp500;(_tmp500.tag=0,((_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp116),((_tmp501[0]=& _tmp500,Cyc_fprintf(Cyc_stderr,((_tmp502="%s\n",_tag_dyneither(_tmp502,sizeof(char),4))),_tag_dyneither(_tmp501,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp116,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp505;void*_tmp504;(_tmp504=0,Cyc_fprintf(Cyc_stderr,((_tmp505="\nError: preprocessing\n",_tag_dyneither(_tmp505,sizeof(char),23))),_tag_dyneither(_tmp504,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
return;}
# 944
if(Cyc_stop_after_cpp_r)return;
# 947
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpFC);{
const char*_tmp507;const char*_tmp506;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFC,((_tmp506="r",_tag_dyneither(_tmp506,sizeof(char),2))),((_tmp507="file",_tag_dyneither(_tmp507,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 951
struct Cyc_List_List*tds=0;
# 954
{struct _handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))_tmp11E=1;if(!_tmp11E){
{const char*_tmp508;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp508="parsing",_tag_dyneither(_tmp508,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11D=(void*)_exn_thrown;void*_tmp121=_tmp11D;void*_tmp122;void*_tmp123;_LL26: _tmp122=_tmp121;_LL27:
# 958
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
(int)_throw(_tmp122);_LL28: _tmp123=_tmp121;_LL29:(void)_throw(_tmp123);_LL25:;}};}
# 962
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
return;}
# 968
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 973
struct _RegionHandle _tmp124=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp124;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp125=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 978
struct _tuple22 _tmp509;struct _tuple22 _tmp126=(_tmp509.f1=tc_rgn,((_tmp509.f2=_tmp125,_tmp509)));
{const char*_tmp50A;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50A="type checking",_tag_dyneither(_tmp50A,sizeof(char),14))),Cyc_do_typecheck,& _tmp126,tds);}
# 981
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 985
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp50B;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50B="control-flow checking",_tag_dyneither(_tmp50B,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 993
if(Cyc_generate_interface_r){
const char*_tmp50E;const char*_tmp50D;const char*_tmp50C;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp50C=(const char*)_untag_dyneither_ptr(_tmpFD,sizeof(char),1),_tag_dyneither(_tmp50C,sizeof(char),_get_zero_arr_size_char((void*)_tmp50C,1)))),((_tmp50D="w",_tag_dyneither(_tmp50D,sizeof(char),2))),((_tmp50E="interface file",_tag_dyneither(_tmp50E,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 998
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1005
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpFD,sizeof(char),1),"r");
const char*_tmp510;const char*_tmp50F;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpFE,((_tmp50F="w",_tag_dyneither(_tmp50F,sizeof(char),2))),((_tmp510="interface object file",_tag_dyneither(_tmp510,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1012
Cyc_Position_reset_position(_tmpFD);{
struct _tuple23 _tmp511;struct _tuple23 _tmp12C=(_tmp511.f1=_tmp125,((_tmp511.f2=inter_file,((_tmp511.f3=inter_objfile,_tmp511)))));
{const char*_tmp512;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp512="interface checking",_tag_dyneither(_tmp512,sizeof(char),19))),Cyc_do_interface,& _tmp12C,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1020
if(Cyc_cf_r)goto PRINTC;
# 1023
{const char*_tmp513;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp513="translation to C",_tag_dyneither(_tmp513,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 1029
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1037
const char*_tmp514;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp514="post-pass to VC",_tag_dyneither(_tmp514,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1044
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp516;const char*_tmp515;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp515="w",_tag_dyneither(_tmp515,sizeof(char),2))),((_tmp516="output file",_tag_dyneither(_tmp516,sizeof(char),12))));}else{
# 1050
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp518;const char*_tmp517;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp517="w",_tag_dyneither(_tmp517,sizeof(char),2))),((_tmp518="output file",_tag_dyneither(_tmp518,sizeof(char),12))));}else{
# 1054
const char*_tmp51A;const char*_tmp519;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFF,((_tmp519="w",_tag_dyneither(_tmp519,sizeof(char),2))),((_tmp51A="output file",_tag_dyneither(_tmp51A,sizeof(char),12))));}}
# 1056
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1058
{struct _handler_cons _tmp13A;_push_handler(& _tmp13A);{int _tmp13C=0;if(setjmp(_tmp13A.handler))_tmp13C=1;if(!_tmp13C){
if(!Cyc_noprint_r){
const char*_tmp51B;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp51B="printing",_tag_dyneither(_tmp51B,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1059
;_pop_handler();}else{void*_tmp13B=(void*)_exn_thrown;void*_tmp13F=_tmp13B;void*_tmp140;void*_tmp141;_LL2B: _tmp140=_tmp13F;_LL2C:
# 1063
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp51E;struct Cyc_List_List*_tmp51D;Cyc_cfiles=((_tmp51D=_cycalloc(sizeof(*_tmp51D)),((_tmp51D->hd=((_tmp51E=_cycalloc(sizeof(*_tmp51E)),((_tmp51E[0]=(struct _dyneither_ptr)_tmpFF,_tmp51E)))),((_tmp51D->tl=Cyc_cfiles,_tmp51D))))));}
(int)_throw(_tmp140);_LL2D: _tmp141=_tmp13F;_LL2E:(void)_throw(_tmp141);_LL2A:;}};}
# 1068
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp521;struct Cyc_List_List*_tmp520;Cyc_cfiles=((_tmp520=_cycalloc(sizeof(*_tmp520)),((_tmp520->hd=((_tmp521=_cycalloc(sizeof(*_tmp521)),((_tmp521[0]=(struct _dyneither_ptr)_tmpFF,_tmp521)))),((_tmp520->tl=Cyc_cfiles,_tmp520))))));};}
# 973
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp15E[8]="<final>";
# 1073
static struct _dyneither_ptr Cyc_final_str={_tmp15E,_tmp15E,_tmp15E + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1076
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1080
struct _dyneither_ptr basename;
{struct _handler_cons _tmp15F;_push_handler(& _tmp15F);{int _tmp161=0;if(setjmp(_tmp15F.handler))_tmp161=1;if(!_tmp161){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp160=(void*)_exn_thrown;void*_tmp163=_tmp160;void*_tmp165;_LL30: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp164=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp163;if(_tmp164->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31:
# 1084
 basename=*n;goto _LL2F;_LL32: _tmp165=_tmp163;_LL33:(void)_throw(_tmp165);_LL2F:;}};}{
# 1086
const char*_tmp522;struct _dyneither_ptr _tmp166=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp522=".cycio",_tag_dyneither(_tmp522,sizeof(char),7))));
const char*_tmp524;const char*_tmp523;struct Cyc___cycFILE*_tmp167=Cyc_try_file_open((struct _dyneither_ptr)_tmp166,((_tmp523="rb",_tag_dyneither(_tmp523,sizeof(char),3))),((_tmp524="interface object file",_tag_dyneither(_tmp524,sizeof(char),22))));
if(_tmp167 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1093
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp166);{
# 1095
struct Cyc_Interface_I*_tmp168=Cyc_Interface_load(_tmp167);
Cyc_file_close(_tmp167);
return _tmp168;};};};}
# 1100
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1104
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp52A(unsigned int*_tmp529,unsigned int*_tmp528,int**_tmp526){for(*_tmp529=0;*_tmp529 < *_tmp528;(*_tmp529)++){(*_tmp526)[*_tmp529]=(int)0;}}static void _tmp532(unsigned int*_tmp531,unsigned int*_tmp530,struct _dyneither_ptr**_tmp52E){for(*_tmp531=0;*_tmp531 < *_tmp530;(*_tmp531)++){(*_tmp52E)[*_tmp531]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1141
_tag_dyneither(0,0,0));}}static void _tmp538(unsigned int*_tmp537,unsigned int*_tmp536,struct _dyneither_ptr**_tmp534){for(*_tmp537=0;*_tmp537 < *_tmp536;(*_tmp537)++){(*_tmp534)[*_tmp537]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1107
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1112
GC_blacklist_warn_clear();{
# 1114
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1119
struct _RegionHandle _tmp16C=_new_region("r");struct _RegionHandle*r=& _tmp16C;_push_region(r);{
# 1121
unsigned int _tmp529;unsigned int _tmp528;struct _dyneither_ptr _tmp527;int*_tmp526;unsigned int _tmp525;struct _dyneither_ptr _tmp16D=(_tmp525=(unsigned int)argc,((_tmp526=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp525)),((_tmp527=_tag_dyneither(_tmp526,sizeof(int),_tmp525),((((_tmp528=_tmp525,_tmp52A(& _tmp529,& _tmp528,& _tmp526))),_tmp527)))))));
int _tmp16E=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp52B;if(Cyc_strncmp(((_tmp52B="-B",_tag_dyneither(_tmp52B,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;}else{
# 1129
const char*_tmp52C;if(Cyc_strcmp(((_tmp52C="-b",_tag_dyneither(_tmp52C,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;}}}{
# 1141
unsigned int _tmp531;unsigned int _tmp530;struct _dyneither_ptr _tmp52F;struct _dyneither_ptr*_tmp52E;unsigned int _tmp52D;struct _dyneither_ptr _tmp171=(_tmp52D=(unsigned int)(_tmp16E + 1),((_tmp52E=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp52D)),((_tmp52F=_tag_dyneither(_tmp52E,sizeof(struct _dyneither_ptr),_tmp52D),((((_tmp530=_tmp52D,_tmp532(& _tmp531,& _tmp530,& _tmp52E))),_tmp52F)))))));
unsigned int _tmp537;unsigned int _tmp536;struct _dyneither_ptr _tmp535;struct _dyneither_ptr*_tmp534;unsigned int _tmp533;struct _dyneither_ptr _tmp172=(_tmp533=(unsigned int)(argc - _tmp16E),((_tmp534=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp533)),((_tmp535=_tag_dyneither(_tmp534,sizeof(struct _dyneither_ptr),_tmp533),((((_tmp536=_tmp533,_tmp538(& _tmp537,& _tmp536,& _tmp534))),_tmp535)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp171,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp172,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp171,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp172,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1150
struct _tuple24*_tmpA91;const char*_tmpA90;const char*_tmpA8F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA8E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA8D;const char*_tmpA8C;struct _tuple24*_tmpA8B;const char*_tmpA8A;const char*_tmpA89;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA88;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA87;const char*_tmpA86;struct _tuple24*_tmpA85;const char*_tmpA84;const char*_tmpA83;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA82;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA81;const char*_tmpA80;struct _tuple24*_tmpA7F;const char*_tmpA7E;const char*_tmpA7D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA7C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA7B;const char*_tmpA7A;struct _tuple24*_tmpA79;const char*_tmpA78;const char*_tmpA77;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA76;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA75;const char*_tmpA74;struct _tuple24*_tmpA73;const char*_tmpA72;const char*_tmpA71;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA70;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6F;const char*_tmpA6E;struct _tuple24*_tmpA6D;const char*_tmpA6C;const char*_tmpA6B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA69;const char*_tmpA68;struct _tuple24*_tmpA67;const char*_tmpA66;const char*_tmpA65;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA64;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA63;const char*_tmpA62;struct _tuple24*_tmpA61;const char*_tmpA60;const char*_tmpA5F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5D;const char*_tmpA5C;struct _tuple24*_tmpA5B;const char*_tmpA5A;const char*_tmpA59;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA58;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA57;const char*_tmpA56;struct _tuple24*_tmpA55;const char*_tmpA54;const char*_tmpA53;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA52;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA51;const char*_tmpA50;struct _tuple24*_tmpA4F;const char*_tmpA4E;const char*_tmpA4D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA4C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA4B;const char*_tmpA4A;struct _tuple24*_tmpA49;const char*_tmpA48;const char*_tmpA47;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA46;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA45;const char*_tmpA44;struct _tuple24*_tmpA43;const char*_tmpA42;const char*_tmpA41;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA40;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA3F;const char*_tmpA3E;struct _tuple24*_tmpA3D;const char*_tmpA3C;const char*_tmpA3B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA3A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA39;const char*_tmpA38;struct _tuple24*_tmpA37;const char*_tmpA36;const char*_tmpA35;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA34;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA33;const char*_tmpA32;struct _tuple24*_tmpA31;const char*_tmpA30;const char*_tmpA2F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA2E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA2D;const char*_tmpA2C;struct _tuple24*_tmpA2B;const char*_tmpA2A;const char*_tmpA29;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA28;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA27;const char*_tmpA26;struct _tuple24*_tmpA25;const char*_tmpA24;const char*_tmpA23;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA22;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA21;const char*_tmpA20;struct _tuple24*_tmpA1F;const char*_tmpA1E;const char*_tmpA1D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA1C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA1B;const char*_tmpA1A;struct _tuple24*_tmpA19;const char*_tmpA18;const char*_tmpA17;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA16;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA15;const char*_tmpA14;struct _tuple24*_tmpA13;const char*_tmpA12;const char*_tmpA11;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA10;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA0F;const char*_tmpA0E;struct _tuple24*_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA0A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA09;const char*_tmpA08;struct _tuple24*_tmpA07;const char*_tmpA06;const char*_tmpA05;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA04;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA03;const char*_tmpA02;struct _tuple24*_tmpA01;const char*_tmpA00;const char*_tmp9FF;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9FE;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9FD;const char*_tmp9FC;struct _tuple24*_tmp9FB;const char*_tmp9FA;const char*_tmp9F9;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9F8;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9F7;const char*_tmp9F6;struct _tuple24*_tmp9F5;const char*_tmp9F4;const char*_tmp9F3;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F2;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F1;const char*_tmp9F0;struct _tuple24*_tmp9EF;const char*_tmp9EE;const char*_tmp9ED;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9EC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9EB;const char*_tmp9EA;struct _tuple24*_tmp9E9;const char*_tmp9E8;const char*_tmp9E7;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E6;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E5;const char*_tmp9E4;struct _tuple24*_tmp9E3;const char*_tmp9E2;const char*_tmp9E1;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E0;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DF;const char*_tmp9DE;struct _tuple24*_tmp9DD;const char*_tmp9DC;const char*_tmp9DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DA;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D9;const char*_tmp9D8;struct _tuple24*_tmp9D7;const char*_tmp9D6;const char*_tmp9D5;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D4;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D3;const char*_tmp9D2;struct _tuple24*_tmp9D1;const char*_tmp9D0;const char*_tmp9CF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CD;const char*_tmp9CC;struct _tuple24*_tmp9CB;const char*_tmp9CA;const char*_tmp9C9;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C8;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C7;const char*_tmp9C6;struct _tuple24*_tmp9C5;const char*_tmp9C4;const char*_tmp9C3;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C2;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C1;const char*_tmp9C0;struct _tuple24*_tmp9BF;const char*_tmp9BE;const char*_tmp9BD;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9BC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9BB;const char*_tmp9BA;struct _tuple24*_tmp9B9;const char*_tmp9B8;const char*_tmp9B7;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9B6;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9B5;const char*_tmp9B4;struct _tuple24*_tmp9B3;const char*_tmp9B2;const char*_tmp9B1;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B0;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9AF;const char*_tmp9AE;struct _tuple24*_tmp9AD;const char*_tmp9AC;const char*_tmp9AB;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9AA;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9A9;const char*_tmp9A8;struct _tuple24*_tmp9A7;const char*_tmp9A6;const char*_tmp9A5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A3;const char*_tmp9A2;struct _tuple24*_tmp9A1;const char*_tmp9A0;const char*_tmp99F;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp99E;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp99D;const char*_tmp99C;struct _tuple24*_tmp99B;const char*_tmp99A;const char*_tmp999;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp998;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp997;const char*_tmp996;struct _tuple24*_tmp995;const char*_tmp994;const char*_tmp993;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp992;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp991;const char*_tmp990;struct _tuple24*_tmp98F;const char*_tmp98E;const char*_tmp98D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp98C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98B;const char*_tmp98A;struct _tuple24*_tmp989;const char*_tmp988;const char*_tmp987;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp986;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp985;const char*_tmp984;struct _tuple24*_tmp983;const char*_tmp982;const char*_tmp981;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp980;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97F;const char*_tmp97E;struct _tuple24*_tmp97D;const char*_tmp97C;const char*_tmp97B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp979;const char*_tmp978;struct _tuple24*_tmp977;const char*_tmp976;const char*_tmp975;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp974;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp973;const char*_tmp972;struct _tuple24*_tmp971;const char*_tmp970;const char*_tmp96F;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp96E;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp96D;const char*_tmp96C;struct _tuple24*_tmp96B;const char*_tmp96A;const char*_tmp969;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp968;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp967;const char*_tmp966;struct _tuple24*_tmp965;const char*_tmp964;const char*_tmp963;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp962;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp961;const char*_tmp960;struct _tuple24*_tmp95F;const char*_tmp95E;const char*_tmp95D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95B;const char*_tmp95A;struct _tuple24*_tmp959;const char*_tmp958;const char*_tmp957;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp956;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp955;const char*_tmp954;struct _tuple24*_tmp953;const char*_tmp952;const char*_tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp950;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94F;const char*_tmp94E;struct _tuple24*_tmp94D;const char*_tmp94C;const char*_tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp949;const char*_tmp948;struct _tuple24*_tmp947;const char*_tmp946;const char*_tmp945;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp944;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp943;const char*_tmp942;struct _tuple24*_tmp941;const char*_tmp940;const char*_tmp93F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93D;const char*_tmp93C;struct _tuple24*_tmp93B;const char*_tmp93A;const char*_tmp939;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp938;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp937;const char*_tmp936;struct _tuple24*_tmp935;const char*_tmp934;const char*_tmp933;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp932;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp931;const char*_tmp930;struct _tuple24*_tmp92F;const char*_tmp92E;const char*_tmp92D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92B;const char*_tmp92A;struct _tuple24*_tmp929;const char*_tmp928;const char*_tmp927;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp926;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp925;const char*_tmp924;struct _tuple24*_tmp923;const char*_tmp922;const char*_tmp921;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp920;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp91F;const char*_tmp91E;struct _tuple24*_tmp91D;const char*_tmp91C;const char*_tmp91B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp91A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp919;const char*_tmp918;struct _tuple24*_tmp917;const char*_tmp916;const char*_tmp915;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp914;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp913;const char*_tmp912;struct _tuple24*_tmp911;const char*_tmp910;const char*_tmp90F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp90E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp90D;const char*_tmp90C;struct _tuple24*_tmp90B;const char*_tmp90A;const char*_tmp909;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp908;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp907;const char*_tmp906;struct _tuple24*_tmp905;const char*_tmp904;const char*_tmp903;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp902;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp901;const char*_tmp900;struct _tuple24*_tmp8FF;const char*_tmp8FE;const char*_tmp8FD;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8FC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8FB;const char*_tmp8FA;struct _tuple24*_tmp8F9;const char*_tmp8F8;const char*_tmp8F7;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F6;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F5;const char*_tmp8F4;struct _tuple24*_tmp8F3;const char*_tmp8F2;const char*_tmp8F1;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F0;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8EF;const char*_tmp8EE;struct _tuple24*_tmp8ED;const char*_tmp8EC;const char*_tmp8EB;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8EA;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E9;const char*_tmp8E8;struct _tuple24*_tmp8E7;const char*_tmp8E6;const char*_tmp8E5;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E4;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E3;const char*_tmp8E2;struct _tuple24*_tmp8E1;const char*_tmp8E0;const char*_tmp8DF;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8DE;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8DD;const char*_tmp8DC;struct _tuple24*_tmp8DB;const char*_tmp8DA;const char*_tmp8D9;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D8;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D7;const char*_tmp8D6;struct _tuple24*_tmp8D5;const char*_tmp8D4;const char*_tmp8D3;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D2;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D1;const char*_tmp8D0;struct _tuple24*_tmp8CF;const char*_tmp8CE;const char*_tmp8CD;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8CC;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8CB;const char*_tmp8CA;struct _tuple24*_tmp8C9[76];struct Cyc_List_List*options=
(_tmp8C9[75]=(
# 1378
(_tmp8CF=_region_malloc(r,sizeof(*_tmp8CF)),((_tmp8CF->f1=((_tmp8CE="--interface",_tag_dyneither(_tmp8CE,sizeof(char),12))),((_tmp8CF->f2=0,((_tmp8CF->f3=((_tmp8CD=" <file>",_tag_dyneither(_tmp8CD,sizeof(char),8))),((_tmp8CF->f4=(void*)(
(_tmp8CB=_region_malloc(r,sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CC.tag=5,((_tmp8CC.f1=Cyc_set_specified_interface,_tmp8CC)))),_tmp8CB)))),((_tmp8CF->f5=(
(_tmp8CA="Set the interface file to be <file>.",_tag_dyneither(_tmp8CA,sizeof(char),37))),_tmp8CF)))))))))))),((_tmp8C9[74]=(
# 1375
(_tmp8D5=_region_malloc(r,sizeof(*_tmp8D5)),((_tmp8D5->f1=((_tmp8D4="--geninterface",_tag_dyneither(_tmp8D4,sizeof(char),15))),((_tmp8D5->f2=0,((_tmp8D5->f3=((_tmp8D3="",_tag_dyneither(_tmp8D3,sizeof(char),1))),((_tmp8D5->f4=(void*)(
(_tmp8D1=_region_malloc(r,sizeof(*_tmp8D1)),((_tmp8D1[0]=((_tmp8D2.tag=3,((_tmp8D2.f1=& Cyc_generate_interface_r,_tmp8D2)))),_tmp8D1)))),((_tmp8D5->f5=(
(_tmp8D0="Generate interface files",_tag_dyneither(_tmp8D0,sizeof(char),25))),_tmp8D5)))))))))))),((_tmp8C9[73]=(
# 1372
(_tmp8DB=_region_malloc(r,sizeof(*_tmp8DB)),((_tmp8DB->f1=((_tmp8DA="-idirafter",_tag_dyneither(_tmp8DA,sizeof(char),11))),((_tmp8DB->f2=0,((_tmp8DB->f3=((_tmp8D9="<dir>",_tag_dyneither(_tmp8D9,sizeof(char),6))),((_tmp8DB->f4=(void*)(
(_tmp8D7=_region_malloc(r,sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D8.tag=5,((_tmp8D8.f1=Cyc_add_idirafter,_tmp8D8)))),_tmp8D7)))),((_tmp8DB->f5=(
(_tmp8D6="Add the directory to the second include path.",_tag_dyneither(_tmp8D6,sizeof(char),46))),_tmp8DB)))))))))))),((_tmp8C9[72]=(
# 1369
(_tmp8E1=_region_malloc(r,sizeof(*_tmp8E1)),((_tmp8E1->f1=((_tmp8E0="-isystem",_tag_dyneither(_tmp8E0,sizeof(char),9))),((_tmp8E1->f2=0,((_tmp8E1->f3=((_tmp8DF="<dir>",_tag_dyneither(_tmp8DF,sizeof(char),6))),((_tmp8E1->f4=(void*)(
(_tmp8DD=_region_malloc(r,sizeof(*_tmp8DD)),((_tmp8DD[0]=((_tmp8DE.tag=5,((_tmp8DE.f1=Cyc_add_isystem,_tmp8DE)))),_tmp8DD)))),((_tmp8E1->f5=(
(_tmp8DC="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8DC,sizeof(char),90))),_tmp8E1)))))))))))),((_tmp8C9[71]=(
# 1366
(_tmp8E7=_region_malloc(r,sizeof(*_tmp8E7)),((_tmp8E7->f1=((_tmp8E6="-iwithprefixbefore",_tag_dyneither(_tmp8E6,sizeof(char),19))),((_tmp8E7->f2=0,((_tmp8E7->f3=((_tmp8E5="<dir>",_tag_dyneither(_tmp8E5,sizeof(char),6))),((_tmp8E7->f4=(void*)(
(_tmp8E3=_region_malloc(r,sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=5,((_tmp8E4.f1=Cyc_add_iwithprefixbefore,_tmp8E4)))),_tmp8E3)))),((_tmp8E7->f5=(
(_tmp8E2="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8E2,sizeof(char),45))),_tmp8E7)))))))))))),((_tmp8C9[70]=(
# 1363
(_tmp8ED=_region_malloc(r,sizeof(*_tmp8ED)),((_tmp8ED->f1=((_tmp8EC="-iwithprefix",_tag_dyneither(_tmp8EC,sizeof(char),13))),((_tmp8ED->f2=0,((_tmp8ED->f3=((_tmp8EB="<dir>",_tag_dyneither(_tmp8EB,sizeof(char),6))),((_tmp8ED->f4=(void*)(
(_tmp8E9=_region_malloc(r,sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EA.tag=5,((_tmp8EA.f1=Cyc_add_iwithprefix,_tmp8EA)))),_tmp8E9)))),((_tmp8ED->f5=(
(_tmp8E8="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp8E8,sizeof(char),47))),_tmp8ED)))))))))))),((_tmp8C9[69]=(
# 1360
(_tmp8F3=_region_malloc(r,sizeof(*_tmp8F3)),((_tmp8F3->f1=((_tmp8F2="-iprefix",_tag_dyneither(_tmp8F2,sizeof(char),9))),((_tmp8F3->f2=0,((_tmp8F3->f3=((_tmp8F1="<prefix>",_tag_dyneither(_tmp8F1,sizeof(char),9))),((_tmp8F3->f4=(void*)(
(_tmp8EF=_region_malloc(r,sizeof(*_tmp8EF)),((_tmp8EF[0]=((_tmp8F0.tag=5,((_tmp8F0.f1=Cyc_add_iprefix,_tmp8F0)))),_tmp8EF)))),((_tmp8F3->f5=(
(_tmp8EE="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp8EE,sizeof(char),67))),_tmp8F3)))))))))))),((_tmp8C9[68]=(
# 1357
(_tmp8F9=_region_malloc(r,sizeof(*_tmp8F9)),((_tmp8F9->f1=((_tmp8F8="-noregions",_tag_dyneither(_tmp8F8,sizeof(char),11))),((_tmp8F9->f2=0,((_tmp8F9->f3=((_tmp8F7="",_tag_dyneither(_tmp8F7,sizeof(char),1))),((_tmp8F9->f4=(void*)(
(_tmp8F5=_region_malloc(r,sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F6.tag=3,((_tmp8F6.f1=& Cyc_Absyn_no_regions,_tmp8F6)))),_tmp8F5)))),((_tmp8F9->f5=(
(_tmp8F4="Generate code that doesn't use regions",_tag_dyneither(_tmp8F4,sizeof(char),39))),_tmp8F9)))))))))))),((_tmp8C9[67]=(
# 1354
(_tmp8FF=_region_malloc(r,sizeof(*_tmp8FF)),((_tmp8FF->f1=((_tmp8FE="-port",_tag_dyneither(_tmp8FE,sizeof(char),6))),((_tmp8FF->f2=0,((_tmp8FF->f3=((_tmp8FD="",_tag_dyneither(_tmp8FD,sizeof(char),1))),((_tmp8FF->f4=(void*)(
(_tmp8FB=_region_malloc(r,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=0,((_tmp8FC.f1=Cyc_set_port_c_code,_tmp8FC)))),_tmp8FB)))),((_tmp8FF->f5=(
(_tmp8FA="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp8FA,sizeof(char),38))),_tmp8FF)))))))))))),((_tmp8C9[66]=(
# 1351
(_tmp905=_region_malloc(r,sizeof(*_tmp905)),((_tmp905->f1=((_tmp904="-detailedlocation",_tag_dyneither(_tmp904,sizeof(char),18))),((_tmp905->f2=0,((_tmp905->f3=((_tmp903="",_tag_dyneither(_tmp903,sizeof(char),1))),((_tmp905->f4=(void*)(
(_tmp901=_region_malloc(r,sizeof(*_tmp901)),((_tmp901[0]=((_tmp902.tag=4,((_tmp902.f1=& Cyc_Position_use_gcc_style_location,_tmp902)))),_tmp901)))),((_tmp905->f5=(
(_tmp900="Try to give more detailed location information for errors",_tag_dyneither(_tmp900,sizeof(char),58))),_tmp905)))))))))))),((_tmp8C9[65]=(
# 1348
(_tmp90B=_region_malloc(r,sizeof(*_tmp90B)),((_tmp90B->f1=((_tmp90A="-noregister",_tag_dyneither(_tmp90A,sizeof(char),12))),((_tmp90B->f2=0,((_tmp90B->f3=((_tmp909="",_tag_dyneither(_tmp909,sizeof(char),1))),((_tmp90B->f4=(void*)(
(_tmp907=_region_malloc(r,sizeof(*_tmp907)),((_tmp907[0]=((_tmp908.tag=3,((_tmp908.f1=& Cyc_Parse_no_register,_tmp908)))),_tmp907)))),((_tmp90B->f5=(
(_tmp906="Treat register storage class as public",_tag_dyneither(_tmp906,sizeof(char),39))),_tmp90B)))))))))))),((_tmp8C9[64]=(
# 1345
(_tmp911=_region_malloc(r,sizeof(*_tmp911)),((_tmp911->f1=((_tmp910="-warnregioncoerce",_tag_dyneither(_tmp910,sizeof(char),18))),((_tmp911->f2=0,((_tmp911->f3=((_tmp90F="",_tag_dyneither(_tmp90F,sizeof(char),1))),((_tmp911->f4=(void*)(
(_tmp90D=_region_malloc(r,sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=3,((_tmp90E.f1=& Cyc_Tcutil_warn_region_coerce,_tmp90E)))),_tmp90D)))),((_tmp911->f5=(
(_tmp90C="Warn when any region coercion is inserted",_tag_dyneither(_tmp90C,sizeof(char),42))),_tmp911)))))))))))),((_tmp8C9[63]=(
# 1342
(_tmp917=_region_malloc(r,sizeof(*_tmp917)),((_tmp917->f1=((_tmp916="-warnaliascoercion",_tag_dyneither(_tmp916,sizeof(char),19))),((_tmp917->f2=0,((_tmp917->f3=((_tmp915="",_tag_dyneither(_tmp915,sizeof(char),1))),((_tmp917->f4=(void*)(
(_tmp913=_region_malloc(r,sizeof(*_tmp913)),((_tmp913[0]=((_tmp914.tag=3,((_tmp914.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp914)))),_tmp913)))),((_tmp917->f5=(
(_tmp912="Warn when any alias coercion is inserted",_tag_dyneither(_tmp912,sizeof(char),41))),_tmp917)))))))))))),((_tmp8C9[62]=(
# 1339
(_tmp91D=_region_malloc(r,sizeof(*_tmp91D)),((_tmp91D->f1=((_tmp91C="-warnnullchecks",_tag_dyneither(_tmp91C,sizeof(char),16))),((_tmp91D->f2=0,((_tmp91D->f3=((_tmp91B="",_tag_dyneither(_tmp91B,sizeof(char),1))),((_tmp91D->f4=(void*)(
(_tmp919=_region_malloc(r,sizeof(*_tmp919)),((_tmp919[0]=((_tmp91A.tag=3,((_tmp91A.f1=& Cyc_Toc_warn_all_null_deref,_tmp91A)))),_tmp919)))),((_tmp91D->f5=(
(_tmp918="Warn when any null check can't be eliminated",_tag_dyneither(_tmp918,sizeof(char),45))),_tmp91D)))))))))))),((_tmp8C9[61]=(
# 1336
(_tmp923=_region_malloc(r,sizeof(*_tmp923)),((_tmp923->f1=((_tmp922="-warnboundschecks",_tag_dyneither(_tmp922,sizeof(char),18))),((_tmp923->f2=0,((_tmp923->f3=((_tmp921="",_tag_dyneither(_tmp921,sizeof(char),1))),((_tmp923->f4=(void*)(
(_tmp91F=_region_malloc(r,sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp920.tag=3,((_tmp920.f1=& Cyc_Toc_warn_bounds_checks,_tmp920)))),_tmp91F)))),((_tmp923->f5=(
(_tmp91E="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp91E,sizeof(char),44))),_tmp923)))))))))))),((_tmp8C9[60]=(
# 1333
(_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929->f1=((_tmp928="-CI",_tag_dyneither(_tmp928,sizeof(char),4))),((_tmp929->f2=0,((_tmp929->f3=((_tmp927=" <file>",_tag_dyneither(_tmp927,sizeof(char),8))),((_tmp929->f4=(void*)(
(_tmp925=_region_malloc(r,sizeof(*_tmp925)),((_tmp925[0]=((_tmp926.tag=5,((_tmp926.f1=Cyc_set_cyc_include,_tmp926)))),_tmp925)))),((_tmp929->f5=(
(_tmp924="Set cyc_include.h to be <file>",_tag_dyneither(_tmp924,sizeof(char),31))),_tmp929)))))))))))),((_tmp8C9[59]=(
# 1330
(_tmp92F=_region_malloc(r,sizeof(*_tmp92F)),((_tmp92F->f1=((_tmp92E="-compile-for-boot",_tag_dyneither(_tmp92E,sizeof(char),18))),((_tmp92F->f2=0,((_tmp92F->f3=((_tmp92D="",_tag_dyneither(_tmp92D,sizeof(char),1))),((_tmp92F->f4=(void*)(
(_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92C.tag=3,((_tmp92C.f1=& Cyc_Lex_compile_for_boot_r,_tmp92C)))),_tmp92B)))),((_tmp92F->f5=(
(_tmp92A="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp92A,sizeof(char),71))),_tmp92F)))))))))))),((_tmp8C9[58]=(
# 1327
(_tmp935=_region_malloc(r,sizeof(*_tmp935)),((_tmp935->f1=((_tmp934="-nocyc_setjmp",_tag_dyneither(_tmp934,sizeof(char),14))),((_tmp935->f2=0,((_tmp935->f3=((_tmp933="",_tag_dyneither(_tmp933,sizeof(char),1))),((_tmp935->f4=(void*)(
(_tmp931=_region_malloc(r,sizeof(*_tmp931)),((_tmp931[0]=((_tmp932.tag=3,((_tmp932.f1=& Cyc_nocyc_setjmp_r,_tmp932)))),_tmp931)))),((_tmp935->f5=(
(_tmp930="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp930,sizeof(char),46))),_tmp935)))))))))))),((_tmp8C9[57]=(
# 1324
(_tmp93B=_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B->f1=((_tmp93A="-printalleffects",_tag_dyneither(_tmp93A,sizeof(char),17))),((_tmp93B->f2=0,((_tmp93B->f3=((_tmp939="",_tag_dyneither(_tmp939,sizeof(char),1))),((_tmp93B->f4=(void*)(
(_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937[0]=((_tmp938.tag=3,((_tmp938.f1=& Cyc_print_all_effects_r,_tmp938)))),_tmp937)))),((_tmp93B->f5=(
(_tmp936="Print effects for functions (type debugging)",_tag_dyneither(_tmp936,sizeof(char),45))),_tmp93B)))))))))))),((_tmp8C9[56]=(
# 1321
(_tmp941=_region_malloc(r,sizeof(*_tmp941)),((_tmp941->f1=((_tmp940="-printfullevars",_tag_dyneither(_tmp940,sizeof(char),16))),((_tmp941->f2=0,((_tmp941->f3=((_tmp93F="",_tag_dyneither(_tmp93F,sizeof(char),1))),((_tmp941->f4=(void*)(
(_tmp93D=_region_malloc(r,sizeof(*_tmp93D)),((_tmp93D[0]=((_tmp93E.tag=3,((_tmp93E.f1=& Cyc_print_full_evars_r,_tmp93E)))),_tmp93D)))),((_tmp941->f5=(
(_tmp93C="Print full information for evars (type debugging)",_tag_dyneither(_tmp93C,sizeof(char),50))),_tmp941)))))))))))),((_tmp8C9[55]=(
# 1318
(_tmp947=_region_malloc(r,sizeof(*_tmp947)),((_tmp947->f1=((_tmp946="-printallkinds",_tag_dyneither(_tmp946,sizeof(char),15))),((_tmp947->f2=0,((_tmp947->f3=((_tmp945="",_tag_dyneither(_tmp945,sizeof(char),1))),((_tmp947->f4=(void*)(
(_tmp943=_region_malloc(r,sizeof(*_tmp943)),((_tmp943[0]=((_tmp944.tag=3,((_tmp944.f1=& Cyc_print_all_kinds_r,_tmp944)))),_tmp943)))),((_tmp947->f5=(
(_tmp942="Always print kinds of type variables",_tag_dyneither(_tmp942,sizeof(char),37))),_tmp947)))))))))))),((_tmp8C9[54]=(
# 1315
(_tmp94D=_region_malloc(r,sizeof(*_tmp94D)),((_tmp94D->f1=((_tmp94C="-printalltvars",_tag_dyneither(_tmp94C,sizeof(char),15))),((_tmp94D->f2=0,((_tmp94D->f3=((_tmp94B="",_tag_dyneither(_tmp94B,sizeof(char),1))),((_tmp94D->f4=(void*)(
(_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949[0]=((_tmp94A.tag=3,((_tmp94A.f1=& Cyc_print_all_tvars_r,_tmp94A)))),_tmp949)))),((_tmp94D->f5=(
(_tmp948="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp948,sizeof(char),57))),_tmp94D)))))))))))),((_tmp8C9[53]=(
# 1312
(_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953->f1=((_tmp952="-noexpandtypedefs",_tag_dyneither(_tmp952,sizeof(char),18))),((_tmp953->f2=0,((_tmp953->f3=((_tmp951="",_tag_dyneither(_tmp951,sizeof(char),1))),((_tmp953->f4=(void*)(
(_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F[0]=((_tmp950.tag=3,((_tmp950.f1=& Cyc_noexpand_r,_tmp950)))),_tmp94F)))),((_tmp953->f5=(
(_tmp94E="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp94E,sizeof(char),41))),_tmp953)))))))))))),((_tmp8C9[52]=(
# 1309
(_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959->f1=((_tmp958="-noremoveunused",_tag_dyneither(_tmp958,sizeof(char),16))),((_tmp959->f2=0,((_tmp959->f3=((_tmp957="",_tag_dyneither(_tmp957,sizeof(char),1))),((_tmp959->f4=(void*)(
(_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955[0]=((_tmp956.tag=3,((_tmp956.f1=& Cyc_noshake_r,_tmp956)))),_tmp955)))),((_tmp959->f5=(
(_tmp954="Don't remove externed variables that aren't used",_tag_dyneither(_tmp954,sizeof(char),49))),_tmp959)))))))))))),((_tmp8C9[51]=(
# 1306
(_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F->f1=((_tmp95E="-nogc",_tag_dyneither(_tmp95E,sizeof(char),6))),((_tmp95F->f2=0,((_tmp95F->f3=((_tmp95D="",_tag_dyneither(_tmp95D,sizeof(char),1))),((_tmp95F->f4=(void*)(
(_tmp95B=_region_malloc(r,sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95C.tag=3,((_tmp95C.f1=& Cyc_nogc_r,_tmp95C)))),_tmp95B)))),((_tmp95F->f5=(
(_tmp95A="Don't link in the garbage collector",_tag_dyneither(_tmp95A,sizeof(char),36))),_tmp95F)))))))))))),((_tmp8C9[50]=(
# 1303
(_tmp965=_region_malloc(r,sizeof(*_tmp965)),((_tmp965->f1=((_tmp964="-nocyc",_tag_dyneither(_tmp964,sizeof(char),7))),((_tmp965->f2=0,((_tmp965->f3=((_tmp963="",_tag_dyneither(_tmp963,sizeof(char),1))),((_tmp965->f4=(void*)(
(_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961[0]=((_tmp962.tag=0,((_tmp962.f1=Cyc_set_nocyc,_tmp962)))),_tmp961)))),((_tmp965->f5=(
(_tmp960="Don't add implicit namespace Cyc",_tag_dyneither(_tmp960,sizeof(char),33))),_tmp965)))))))))))),((_tmp8C9[49]=(
# 1300
(_tmp96B=_region_malloc(r,sizeof(*_tmp96B)),((_tmp96B->f1=((_tmp96A="-no-cpp-precomp",_tag_dyneither(_tmp96A,sizeof(char),16))),((_tmp96B->f2=0,((_tmp96B->f3=((_tmp969="",_tag_dyneither(_tmp969,sizeof(char),1))),((_tmp96B->f4=(void*)(
(_tmp967=_region_malloc(r,sizeof(*_tmp967)),((_tmp967[0]=((_tmp968.tag=0,((_tmp968.f1=Cyc_set_nocppprecomp,_tmp968)))),_tmp967)))),((_tmp96B->f5=(
(_tmp966="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp966,sizeof(char),40))),_tmp96B)))))))))))),((_tmp8C9[48]=(
# 1297
(_tmp971=_region_malloc(r,sizeof(*_tmp971)),((_tmp971->f1=((_tmp970="-use-cpp",_tag_dyneither(_tmp970,sizeof(char),9))),((_tmp971->f2=0,((_tmp971->f3=((_tmp96F="<path>",_tag_dyneither(_tmp96F,sizeof(char),7))),((_tmp971->f4=(void*)(
(_tmp96D=_region_malloc(r,sizeof(*_tmp96D)),((_tmp96D[0]=((_tmp96E.tag=5,((_tmp96E.f1=Cyc_set_cpp,_tmp96E)))),_tmp96D)))),((_tmp971->f5=(
(_tmp96C="Indicate which preprocessor to use",_tag_dyneither(_tmp96C,sizeof(char),35))),_tmp971)))))))))))),((_tmp8C9[47]=(
# 1294
(_tmp977=_region_malloc(r,sizeof(*_tmp977)),((_tmp977->f1=((_tmp976="--inline-checks",_tag_dyneither(_tmp976,sizeof(char),16))),((_tmp977->f2=0,((_tmp977->f3=((_tmp975="",_tag_dyneither(_tmp975,sizeof(char),1))),((_tmp977->f4=(void*)(
(_tmp973=_region_malloc(r,sizeof(*_tmp973)),((_tmp973[0]=((_tmp974.tag=0,((_tmp974.f1=Cyc_set_inline_functions,_tmp974)))),_tmp973)))),((_tmp977->f5=(
(_tmp972="Inline bounds checks instead of #define",_tag_dyneither(_tmp972,sizeof(char),40))),_tmp977)))))))))))),((_tmp8C9[46]=(
# 1291
(_tmp97D=_region_malloc(r,sizeof(*_tmp97D)),((_tmp97D->f1=((_tmp97C="--noboundschecks",_tag_dyneither(_tmp97C,sizeof(char),17))),((_tmp97D->f2=0,((_tmp97D->f3=((_tmp97B="",_tag_dyneither(_tmp97B,sizeof(char),1))),((_tmp97D->f4=(void*)(
(_tmp979=_region_malloc(r,sizeof(*_tmp979)),((_tmp979[0]=((_tmp97A.tag=0,((_tmp97A.f1=Cyc_set_noboundschecks,_tmp97A)))),_tmp979)))),((_tmp97D->f5=(
(_tmp978="Disable bounds checks",_tag_dyneither(_tmp978,sizeof(char),22))),_tmp97D)))))))))))),((_tmp8C9[45]=(
# 1288
(_tmp983=_region_malloc(r,sizeof(*_tmp983)),((_tmp983->f1=((_tmp982="--nonullchecks",_tag_dyneither(_tmp982,sizeof(char),15))),((_tmp983->f2=0,((_tmp983->f3=((_tmp981="",_tag_dyneither(_tmp981,sizeof(char),1))),((_tmp983->f4=(void*)(
(_tmp97F=_region_malloc(r,sizeof(*_tmp97F)),((_tmp97F[0]=((_tmp980.tag=0,((_tmp980.f1=Cyc_set_nonullchecks,_tmp980)))),_tmp97F)))),((_tmp983->f5=(
(_tmp97E="Disable null checks",_tag_dyneither(_tmp97E,sizeof(char),20))),_tmp983)))))))))))),((_tmp8C9[44]=(
# 1285
(_tmp989=_region_malloc(r,sizeof(*_tmp989)),((_tmp989->f1=((_tmp988="--nochecks",_tag_dyneither(_tmp988,sizeof(char),11))),((_tmp989->f2=0,((_tmp989->f3=((_tmp987="",_tag_dyneither(_tmp987,sizeof(char),1))),((_tmp989->f4=(void*)(
(_tmp985=_region_malloc(r,sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=0,((_tmp986.f1=Cyc_set_nochecks,_tmp986)))),_tmp985)))),((_tmp989->f5=(
(_tmp984="Disable bounds/null checks",_tag_dyneither(_tmp984,sizeof(char),27))),_tmp989)))))))))))),((_tmp8C9[43]=(
# 1282
(_tmp98F=_region_malloc(r,sizeof(*_tmp98F)),((_tmp98F->f1=((_tmp98E="--nolineno",_tag_dyneither(_tmp98E,sizeof(char),11))),((_tmp98F->f2=0,((_tmp98F->f3=((_tmp98D="",_tag_dyneither(_tmp98D,sizeof(char),1))),((_tmp98F->f4=(void*)(
(_tmp98B=_region_malloc(r,sizeof(*_tmp98B)),((_tmp98B[0]=((_tmp98C.tag=0,((_tmp98C.f1=Cyc_clear_lineno,_tmp98C)))),_tmp98B)))),((_tmp98F->f5=(
(_tmp98A="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp98A,sizeof(char),63))),_tmp98F)))))))))))),((_tmp8C9[42]=(
# 1279
(_tmp995=_region_malloc(r,sizeof(*_tmp995)),((_tmp995->f1=((_tmp994="-save-c",_tag_dyneither(_tmp994,sizeof(char),8))),((_tmp995->f2=0,((_tmp995->f3=((_tmp993="",_tag_dyneither(_tmp993,sizeof(char),1))),((_tmp995->f4=(void*)(
(_tmp991=_region_malloc(r,sizeof(*_tmp991)),((_tmp991[0]=((_tmp992.tag=3,((_tmp992.f1=& Cyc_save_c_r,_tmp992)))),_tmp991)))),((_tmp995->f5=(
(_tmp990="Don't delete temporary C files",_tag_dyneither(_tmp990,sizeof(char),31))),_tmp995)))))))))))),((_tmp8C9[41]=(
# 1276
(_tmp99B=_region_malloc(r,sizeof(*_tmp99B)),((_tmp99B->f1=((_tmp99A="-save-temps",_tag_dyneither(_tmp99A,sizeof(char),12))),((_tmp99B->f2=0,((_tmp99B->f3=((_tmp999="",_tag_dyneither(_tmp999,sizeof(char),1))),((_tmp99B->f4=(void*)(
(_tmp997=_region_malloc(r,sizeof(*_tmp997)),((_tmp997[0]=((_tmp998.tag=0,((_tmp998.f1=Cyc_set_save_temps,_tmp998)))),_tmp997)))),((_tmp99B->f5=(
(_tmp996="Don't delete temporary files",_tag_dyneither(_tmp996,sizeof(char),29))),_tmp99B)))))))))))),((_tmp8C9[40]=(
# 1273
(_tmp9A1=_region_malloc(r,sizeof(*_tmp9A1)),((_tmp9A1->f1=((_tmp9A0="-c-comp",_tag_dyneither(_tmp9A0,sizeof(char),8))),((_tmp9A1->f2=0,((_tmp9A1->f3=((_tmp99F=" <compiler>",_tag_dyneither(_tmp99F,sizeof(char),12))),((_tmp9A1->f4=(void*)(
(_tmp99D=_region_malloc(r,sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99E.tag=5,((_tmp99E.f1=Cyc_set_c_compiler,_tmp99E)))),_tmp99D)))),((_tmp9A1->f5=(
(_tmp99C="Produce C output for the given compiler",_tag_dyneither(_tmp99C,sizeof(char),40))),_tmp9A1)))))))))))),((_tmp8C9[39]=(
# 1270
(_tmp9A7=_region_malloc(r,sizeof(*_tmp9A7)),((_tmp9A7->f1=((_tmp9A6="-un-gcc",_tag_dyneither(_tmp9A6,sizeof(char),8))),((_tmp9A7->f2=0,((_tmp9A7->f3=((_tmp9A5="",_tag_dyneither(_tmp9A5,sizeof(char),1))),((_tmp9A7->f4=(void*)(
(_tmp9A3=_region_malloc(r,sizeof(*_tmp9A3)),((_tmp9A3[0]=((_tmp9A4.tag=0,((_tmp9A4.f1=Cyc_set_tovc,_tmp9A4)))),_tmp9A3)))),((_tmp9A7->f5=(
(_tmp9A2="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp9A2,sizeof(char),57))),_tmp9A7)))))))))))),((_tmp8C9[38]=(
# 1267
(_tmp9AD=_region_malloc(r,sizeof(*_tmp9AD)),((_tmp9AD->f1=((_tmp9AC="-no-elim-statement-expressions",_tag_dyneither(_tmp9AC,sizeof(char),31))),((_tmp9AD->f2=0,((_tmp9AD->f3=((_tmp9AB="",_tag_dyneither(_tmp9AB,sizeof(char),1))),((_tmp9AD->f4=(void*)(
(_tmp9A9=_region_malloc(r,sizeof(*_tmp9A9)),((_tmp9A9[0]=((_tmp9AA.tag=4,((_tmp9AA.f1=& Cyc_elim_se_r,_tmp9AA)))),_tmp9A9)))),((_tmp9AD->f5=(
(_tmp9A8="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9A8,sizeof(char),47))),_tmp9AD)))))))))))),((_tmp8C9[37]=(
# 1263
(_tmp9B3=_region_malloc(r,sizeof(*_tmp9B3)),((_tmp9B3->f1=((_tmp9B2="-elim-statement-expressions",_tag_dyneither(_tmp9B2,sizeof(char),28))),((_tmp9B3->f2=0,((_tmp9B3->f3=((_tmp9B1="",_tag_dyneither(_tmp9B1,sizeof(char),1))),((_tmp9B3->f4=(void*)(
(_tmp9AF=_region_malloc(r,sizeof(*_tmp9AF)),((_tmp9AF[0]=((_tmp9B0.tag=3,((_tmp9B0.f1=& Cyc_elim_se_r,_tmp9B0)))),_tmp9AF)))),((_tmp9B3->f5=(
# 1266
(_tmp9AE="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9AE,sizeof(char),66))),_tmp9B3)))))))))))),((_tmp8C9[36]=(
# 1260
(_tmp9B9=_region_malloc(r,sizeof(*_tmp9B9)),((_tmp9B9->f1=((_tmp9B8="-up",_tag_dyneither(_tmp9B8,sizeof(char),4))),((_tmp9B9->f2=0,((_tmp9B9->f3=((_tmp9B7="",_tag_dyneither(_tmp9B7,sizeof(char),1))),((_tmp9B9->f4=(void*)(
(_tmp9B5=_region_malloc(r,sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=4,((_tmp9B6.f1=& Cyc_pp_r,_tmp9B6)))),_tmp9B5)))),((_tmp9B9->f5=(
(_tmp9B4="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9B4,sizeof(char),55))),_tmp9B9)))))))))))),((_tmp8C9[35]=(
# 1257
(_tmp9BF=_region_malloc(r,sizeof(*_tmp9BF)),((_tmp9BF->f1=((_tmp9BE="-pp",_tag_dyneither(_tmp9BE,sizeof(char),4))),((_tmp9BF->f2=0,((_tmp9BF->f3=((_tmp9BD="",_tag_dyneither(_tmp9BD,sizeof(char),1))),((_tmp9BF->f4=(void*)(
(_tmp9BB=_region_malloc(r,sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BC.tag=3,((_tmp9BC.f1=& Cyc_pp_r,_tmp9BC)))),_tmp9BB)))),((_tmp9BF->f5=(
(_tmp9BA="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9BA,sizeof(char),47))),_tmp9BF)))))))))))),((_tmp8C9[34]=(
# 1254
(_tmp9C5=_region_malloc(r,sizeof(*_tmp9C5)),((_tmp9C5->f1=((_tmp9C4="-ic",_tag_dyneither(_tmp9C4,sizeof(char),4))),((_tmp9C5->f2=0,((_tmp9C5->f3=((_tmp9C3="",_tag_dyneither(_tmp9C3,sizeof(char),1))),((_tmp9C5->f4=(void*)(
(_tmp9C1=_region_malloc(r,sizeof(*_tmp9C1)),((_tmp9C1[0]=((_tmp9C2.tag=3,((_tmp9C2.f1=& Cyc_ic_r,_tmp9C2)))),_tmp9C1)))),((_tmp9C5->f5=(
(_tmp9C0="Activate the link-checker",_tag_dyneither(_tmp9C0,sizeof(char),26))),_tmp9C5)))))))))))),((_tmp8C9[33]=(
# 1251
(_tmp9CB=_region_malloc(r,sizeof(*_tmp9CB)),((_tmp9CB->f1=((_tmp9CA="-stopafter-toc",_tag_dyneither(_tmp9CA,sizeof(char),15))),((_tmp9CB->f2=0,((_tmp9CB->f3=((_tmp9C9="",_tag_dyneither(_tmp9C9,sizeof(char),1))),((_tmp9CB->f4=(void*)(
(_tmp9C7=_region_malloc(r,sizeof(*_tmp9C7)),((_tmp9C7[0]=((_tmp9C8.tag=3,((_tmp9C8.f1=& Cyc_toc_r,_tmp9C8)))),_tmp9C7)))),((_tmp9CB->f5=(
(_tmp9C6="Stop after translation to C",_tag_dyneither(_tmp9C6,sizeof(char),28))),_tmp9CB)))))))))))),((_tmp8C9[32]=(
# 1248
(_tmp9D1=_region_malloc(r,sizeof(*_tmp9D1)),((_tmp9D1->f1=((_tmp9D0="-stopafter-cf",_tag_dyneither(_tmp9D0,sizeof(char),14))),((_tmp9D1->f2=0,((_tmp9D1->f3=((_tmp9CF="",_tag_dyneither(_tmp9CF,sizeof(char),1))),((_tmp9D1->f4=(void*)(
(_tmp9CD=_region_malloc(r,sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CE.tag=3,((_tmp9CE.f1=& Cyc_cf_r,_tmp9CE)))),_tmp9CD)))),((_tmp9D1->f5=(
(_tmp9CC="Stop after control-flow checking",_tag_dyneither(_tmp9CC,sizeof(char),33))),_tmp9D1)))))))))))),((_tmp8C9[31]=(
# 1245
(_tmp9D7=_region_malloc(r,sizeof(*_tmp9D7)),((_tmp9D7->f1=((_tmp9D6="-noprint",_tag_dyneither(_tmp9D6,sizeof(char),9))),((_tmp9D7->f2=0,((_tmp9D7->f3=((_tmp9D5="",_tag_dyneither(_tmp9D5,sizeof(char),1))),((_tmp9D7->f4=(void*)(
(_tmp9D3=_region_malloc(r,sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=3,((_tmp9D4.f1=& Cyc_noprint_r,_tmp9D4)))),_tmp9D3)))),((_tmp9D7->f5=(
(_tmp9D2="Do not print output (when stopping early)",_tag_dyneither(_tmp9D2,sizeof(char),42))),_tmp9D7)))))))))))),((_tmp8C9[30]=(
# 1242
(_tmp9DD=_region_malloc(r,sizeof(*_tmp9DD)),((_tmp9DD->f1=((_tmp9DC="-stopafter-tc",_tag_dyneither(_tmp9DC,sizeof(char),14))),((_tmp9DD->f2=0,((_tmp9DD->f3=((_tmp9DB="",_tag_dyneither(_tmp9DB,sizeof(char),1))),((_tmp9DD->f4=(void*)(
(_tmp9D9=_region_malloc(r,sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=3,((_tmp9DA.f1=& Cyc_tc_r,_tmp9DA)))),_tmp9D9)))),((_tmp9DD->f5=(
(_tmp9D8="Stop after type checking",_tag_dyneither(_tmp9D8,sizeof(char),25))),_tmp9DD)))))))))))),((_tmp8C9[29]=(
# 1239
(_tmp9E3=_region_malloc(r,sizeof(*_tmp9E3)),((_tmp9E3->f1=((_tmp9E2="-stopafter-parse",_tag_dyneither(_tmp9E2,sizeof(char),17))),((_tmp9E3->f2=0,((_tmp9E3->f3=((_tmp9E1="",_tag_dyneither(_tmp9E1,sizeof(char),1))),((_tmp9E3->f4=(void*)(
(_tmp9DF=_region_malloc(r,sizeof(*_tmp9DF)),((_tmp9DF[0]=((_tmp9E0.tag=3,((_tmp9E0.f1=& Cyc_parseonly_r,_tmp9E0)))),_tmp9DF)))),((_tmp9E3->f5=(
(_tmp9DE="Stop after parsing",_tag_dyneither(_tmp9DE,sizeof(char),19))),_tmp9E3)))))))))))),((_tmp8C9[28]=(
# 1236
(_tmp9E9=_region_malloc(r,sizeof(*_tmp9E9)),((_tmp9E9->f1=((_tmp9E8="-E",_tag_dyneither(_tmp9E8,sizeof(char),3))),((_tmp9E9->f2=0,((_tmp9E9->f3=((_tmp9E7="",_tag_dyneither(_tmp9E7,sizeof(char),1))),((_tmp9E9->f4=(void*)(
(_tmp9E5=_region_malloc(r,sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E6.tag=3,((_tmp9E6.f1=& Cyc_stop_after_cpp_r,_tmp9E6)))),_tmp9E5)))),((_tmp9E9->f5=(
(_tmp9E4="Stop after preprocessing",_tag_dyneither(_tmp9E4,sizeof(char),25))),_tmp9E9)))))))))))),((_tmp8C9[27]=(
# 1233
(_tmp9EF=_region_malloc(r,sizeof(*_tmp9EF)),((_tmp9EF->f1=((_tmp9EE="-Wall",_tag_dyneither(_tmp9EE,sizeof(char),6))),((_tmp9EF->f2=0,((_tmp9EF->f3=((_tmp9ED="",_tag_dyneither(_tmp9ED,sizeof(char),1))),((_tmp9EF->f4=(void*)(
(_tmp9EB=_region_malloc(r,sizeof(*_tmp9EB)),((_tmp9EB[0]=((_tmp9EC.tag=0,((_tmp9EC.f1=Cyc_set_all_warnings,_tmp9EC)))),_tmp9EB)))),((_tmp9EF->f5=(
(_tmp9EA="Turn on all warnings",_tag_dyneither(_tmp9EA,sizeof(char),21))),_tmp9EF)))))))))))),((_tmp8C9[26]=(
# 1230
(_tmp9F5=_region_malloc(r,sizeof(*_tmp9F5)),((_tmp9F5->f1=((_tmp9F4="-Wlose-unique",_tag_dyneither(_tmp9F4,sizeof(char),14))),((_tmp9F5->f2=0,((_tmp9F5->f3=((_tmp9F3="",_tag_dyneither(_tmp9F3,sizeof(char),1))),((_tmp9F5->f4=(void*)(
(_tmp9F1=_region_malloc(r,sizeof(*_tmp9F1)),((_tmp9F1[0]=((_tmp9F2.tag=3,((_tmp9F2.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp9F2)))),_tmp9F1)))),((_tmp9F5->f5=(
(_tmp9F0="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp9F0,sizeof(char),49))),_tmp9F5)))))))))))),((_tmp8C9[25]=(
# 1227
(_tmp9FB=_region_malloc(r,sizeof(*_tmp9FB)),((_tmp9FB->f1=((_tmp9FA="-b",_tag_dyneither(_tmp9FA,sizeof(char),3))),((_tmp9FB->f2=0,((_tmp9FB->f3=((_tmp9F9="<arch>",_tag_dyneither(_tmp9F9,sizeof(char),7))),((_tmp9FB->f4=(void*)(
(_tmp9F7=_region_malloc(r,sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=5,((_tmp9F8.f1=Cyc_set_target_arch,_tmp9F8)))),_tmp9F7)))),((_tmp9FB->f5=(
(_tmp9F6="Set target architecture; implies -stopafter-toc",_tag_dyneither(_tmp9F6,sizeof(char),48))),_tmp9FB)))))))))))),((_tmp8C9[24]=(
# 1224
(_tmpA01=_region_malloc(r,sizeof(*_tmpA01)),((_tmpA01->f1=((_tmpA00="-MT",_tag_dyneither(_tmpA00,sizeof(char),4))),((_tmpA01->f2=0,((_tmpA01->f3=((_tmp9FF=" <target>",_tag_dyneither(_tmp9FF,sizeof(char),10))),((_tmpA01->f4=(void*)(
(_tmp9FD=_region_malloc(r,sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=5,((_tmp9FE.f1=Cyc_set_dependencies_target,_tmp9FE)))),_tmp9FD)))),((_tmpA01->f5=(
(_tmp9FC="Give target for dependencies",_tag_dyneither(_tmp9FC,sizeof(char),29))),_tmpA01)))))))))))),((_tmp8C9[23]=(
# 1220
(_tmpA07=_region_malloc(r,sizeof(*_tmpA07)),((_tmpA07->f1=((_tmpA06="-MG",_tag_dyneither(_tmpA06,sizeof(char),4))),((_tmpA07->f2=0,((_tmpA07->f3=((_tmpA05="",_tag_dyneither(_tmpA05,sizeof(char),1))),((_tmpA07->f4=(void*)(
(_tmpA03=_region_malloc(r,sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=1,((_tmpA04.f1=Cyc_add_cpparg,_tmpA04)))),_tmpA03)))),((_tmpA07->f5=(
# 1223
(_tmpA02="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmpA02,sizeof(char),68))),_tmpA07)))))))))))),((_tmp8C9[22]=(
# 1217
(_tmpA0D=_region_malloc(r,sizeof(*_tmpA0D)),((_tmpA0D->f1=((_tmpA0C="-M",_tag_dyneither(_tmpA0C,sizeof(char),3))),((_tmpA0D->f2=0,((_tmpA0D->f3=((_tmpA0B="",_tag_dyneither(_tmpA0B,sizeof(char),1))),((_tmpA0D->f4=(void*)(
(_tmpA09=_region_malloc(r,sizeof(*_tmpA09)),((_tmpA09[0]=((_tmpA0A.tag=0,((_tmpA0A.f1=Cyc_set_produce_dependencies,_tmpA0A)))),_tmpA09)))),((_tmpA0D->f5=(
(_tmpA08="Produce dependencies",_tag_dyneither(_tmpA08,sizeof(char),21))),_tmpA0D)))))))))))),((_tmp8C9[21]=(
# 1214
(_tmpA13=_region_malloc(r,sizeof(*_tmpA13)),((_tmpA13->f1=((_tmpA12="-S",_tag_dyneither(_tmpA12,sizeof(char),3))),((_tmpA13->f2=0,((_tmpA13->f3=((_tmpA11="",_tag_dyneither(_tmpA11,sizeof(char),1))),((_tmpA13->f4=(void*)(
(_tmpA0F=_region_malloc(r,sizeof(*_tmpA0F)),((_tmpA0F[0]=((_tmpA10.tag=0,((_tmpA10.f1=Cyc_set_stop_after_asmfile,_tmpA10)))),_tmpA0F)))),((_tmpA13->f5=(
(_tmpA0E="Stop after producing assembly code",_tag_dyneither(_tmpA0E,sizeof(char),35))),_tmpA13)))))))))))),((_tmp8C9[20]=(
# 1211
(_tmpA19=_region_malloc(r,sizeof(*_tmpA19)),((_tmpA19->f1=((_tmpA18="-pa",_tag_dyneither(_tmpA18,sizeof(char),4))),((_tmpA19->f2=0,((_tmpA19->f3=((_tmpA17="",_tag_dyneither(_tmpA17,sizeof(char),1))),((_tmpA19->f4=(void*)(
(_tmpA15=_region_malloc(r,sizeof(*_tmpA15)),((_tmpA15[0]=((_tmpA16.tag=0,((_tmpA16.f1=Cyc_set_pa,_tmpA16)))),_tmpA15)))),((_tmpA19->f5=(
(_tmpA14="Compile for profiling with aprof",_tag_dyneither(_tmpA14,sizeof(char),33))),_tmpA19)))))))))))),((_tmp8C9[19]=(
# 1208
(_tmpA1F=_region_malloc(r,sizeof(*_tmpA1F)),((_tmpA1F->f1=((_tmpA1E="-pg",_tag_dyneither(_tmpA1E,sizeof(char),4))),((_tmpA1F->f2=0,((_tmpA1F->f3=((_tmpA1D="",_tag_dyneither(_tmpA1D,sizeof(char),1))),((_tmpA1F->f4=(void*)(
(_tmpA1B=_region_malloc(r,sizeof(*_tmpA1B)),((_tmpA1B[0]=((_tmpA1C.tag=0,((_tmpA1C.f1=Cyc_set_pg,_tmpA1C)))),_tmpA1B)))),((_tmpA1F->f5=(
(_tmpA1A="Compile for profiling with gprof",_tag_dyneither(_tmpA1A,sizeof(char),33))),_tmpA1F)))))))))))),((_tmp8C9[18]=(
# 1205
(_tmpA25=_region_malloc(r,sizeof(*_tmpA25)),((_tmpA25->f1=((_tmpA24="-p",_tag_dyneither(_tmpA24,sizeof(char),3))),((_tmpA25->f2=0,((_tmpA25->f3=((_tmpA23="",_tag_dyneither(_tmpA23,sizeof(char),1))),((_tmpA25->f4=(void*)(
(_tmpA21=_region_malloc(r,sizeof(*_tmpA21)),((_tmpA21[0]=((_tmpA22.tag=1,((_tmpA22.f1=Cyc_add_ccarg,_tmpA22)))),_tmpA21)))),((_tmpA25->f5=(
(_tmpA20="Compile for profiling with prof",_tag_dyneither(_tmpA20,sizeof(char),32))),_tmpA25)))))))))))),((_tmp8C9[17]=(
# 1202
(_tmpA2B=_region_malloc(r,sizeof(*_tmpA2B)),((_tmpA2B->f1=((_tmpA2A="-g",_tag_dyneither(_tmpA2A,sizeof(char),3))),((_tmpA2B->f2=0,((_tmpA2B->f3=((_tmpA29="",_tag_dyneither(_tmpA29,sizeof(char),1))),((_tmpA2B->f4=(void*)(
(_tmpA27=_region_malloc(r,sizeof(*_tmpA27)),((_tmpA27[0]=((_tmpA28.tag=1,((_tmpA28.f1=Cyc_add_ccarg,_tmpA28)))),_tmpA27)))),((_tmpA2B->f5=(
(_tmpA26="Compile for debugging",_tag_dyneither(_tmpA26,sizeof(char),22))),_tmpA2B)))))))))))),((_tmp8C9[16]=(
# 1199
(_tmpA31=_region_malloc(r,sizeof(*_tmpA31)),((_tmpA31->f1=((_tmpA30="-fomit-frame-pointer",_tag_dyneither(_tmpA30,sizeof(char),21))),((_tmpA31->f2=0,((_tmpA31->f3=((_tmpA2F="",_tag_dyneither(_tmpA2F,sizeof(char),1))),((_tmpA31->f4=(void*)(
(_tmpA2D=_region_malloc(r,sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA2E.tag=1,((_tmpA2E.f1=Cyc_add_ccarg,_tmpA2E)))),_tmpA2D)))),((_tmpA31->f5=(
(_tmpA2C="Omit frame pointer",_tag_dyneither(_tmpA2C,sizeof(char),19))),_tmpA31)))))))))))),((_tmp8C9[15]=(
# 1196
(_tmpA37=_region_malloc(r,sizeof(*_tmpA37)),((_tmpA37->f1=((_tmpA36="-O3",_tag_dyneither(_tmpA36,sizeof(char),4))),((_tmpA37->f2=0,((_tmpA37->f3=((_tmpA35="",_tag_dyneither(_tmpA35,sizeof(char),1))),((_tmpA37->f4=(void*)(
(_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((_tmpA33[0]=((_tmpA34.tag=1,((_tmpA34.f1=Cyc_add_ccarg,_tmpA34)))),_tmpA33)))),((_tmpA37->f5=(
(_tmpA32="Even more optimization",_tag_dyneither(_tmpA32,sizeof(char),23))),_tmpA37)))))))))))),((_tmp8C9[14]=(
# 1193
(_tmpA3D=_region_malloc(r,sizeof(*_tmpA3D)),((_tmpA3D->f1=((_tmpA3C="-O2",_tag_dyneither(_tmpA3C,sizeof(char),4))),((_tmpA3D->f2=0,((_tmpA3D->f3=((_tmpA3B="",_tag_dyneither(_tmpA3B,sizeof(char),1))),((_tmpA3D->f4=(void*)(
(_tmpA39=_region_malloc(r,sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=1,((_tmpA3A.f1=Cyc_add_ccarg,_tmpA3A)))),_tmpA39)))),((_tmpA3D->f5=(
(_tmpA38="A higher level of optimization",_tag_dyneither(_tmpA38,sizeof(char),31))),_tmpA3D)))))))))))),((_tmp8C9[13]=(
# 1190
(_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((_tmpA43->f1=((_tmpA42="-O",_tag_dyneither(_tmpA42,sizeof(char),3))),((_tmpA43->f2=0,((_tmpA43->f3=((_tmpA41="",_tag_dyneither(_tmpA41,sizeof(char),1))),((_tmpA43->f4=(void*)(
(_tmpA3F=_region_malloc(r,sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA40.tag=1,((_tmpA40.f1=Cyc_add_ccarg,_tmpA40)))),_tmpA3F)))),((_tmpA43->f5=(
(_tmpA3E="Optimize",_tag_dyneither(_tmpA3E,sizeof(char),9))),_tmpA43)))))))))))),((_tmp8C9[12]=(
# 1187
(_tmpA49=_region_malloc(r,sizeof(*_tmpA49)),((_tmpA49->f1=((_tmpA48="-O0",_tag_dyneither(_tmpA48,sizeof(char),4))),((_tmpA49->f2=0,((_tmpA49->f3=((_tmpA47="",_tag_dyneither(_tmpA47,sizeof(char),1))),((_tmpA49->f4=(void*)(
(_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=1,((_tmpA46.f1=Cyc_add_ccarg,_tmpA46)))),_tmpA45)))),((_tmpA49->f5=(
(_tmpA44="Don't optimize",_tag_dyneither(_tmpA44,sizeof(char),15))),_tmpA49)))))))))))),((_tmp8C9[11]=(
# 1184
(_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F->f1=((_tmpA4E="-s",_tag_dyneither(_tmpA4E,sizeof(char),3))),((_tmpA4F->f2=0,((_tmpA4F->f3=((_tmpA4D="",_tag_dyneither(_tmpA4D,sizeof(char),1))),((_tmpA4F->f4=(void*)(
(_tmpA4B=_region_malloc(r,sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=1,((_tmpA4C.f1=Cyc_add_ccarg,_tmpA4C)))),_tmpA4B)))),((_tmpA4F->f5=(
(_tmpA4A="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA4A,sizeof(char),60))),_tmpA4F)))))))))))),((_tmp8C9[10]=(
# 1181
(_tmpA55=_region_malloc(r,sizeof(*_tmpA55)),((_tmpA55->f1=((_tmpA54="-x",_tag_dyneither(_tmpA54,sizeof(char),3))),((_tmpA55->f2=0,((_tmpA55->f3=((_tmpA53=" <language>",_tag_dyneither(_tmpA53,sizeof(char),12))),((_tmpA55->f4=(void*)(
(_tmpA51=_region_malloc(r,sizeof(*_tmpA51)),((_tmpA51[0]=((_tmpA52.tag=5,((_tmpA52.f1=Cyc_set_inputtype,_tmpA52)))),_tmpA51)))),((_tmpA55->f5=(
(_tmpA50="Specify <language> for the following input files",_tag_dyneither(_tmpA50,sizeof(char),49))),_tmpA55)))))))))))),((_tmp8C9[9]=(
# 1178
(_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((_tmpA5B->f1=((_tmpA5A="-c",_tag_dyneither(_tmpA5A,sizeof(char),3))),((_tmpA5B->f2=0,((_tmpA5B->f3=((_tmpA59="",_tag_dyneither(_tmpA59,sizeof(char),1))),((_tmpA5B->f4=(void*)(
(_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA58.tag=0,((_tmpA58.f1=Cyc_set_stop_after_objectfile,_tmpA58)))),_tmpA57)))),((_tmpA5B->f5=(
(_tmpA56="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA56,sizeof(char),61))),_tmpA5B)))))))))))),((_tmp8C9[8]=(
# 1175
(_tmpA61=_region_malloc(r,sizeof(*_tmpA61)),((_tmpA61->f1=((_tmpA60="-m",_tag_dyneither(_tmpA60,sizeof(char),3))),((_tmpA61->f2=1,((_tmpA61->f3=((_tmpA5F="<option>",_tag_dyneither(_tmpA5F,sizeof(char),9))),((_tmpA61->f4=(void*)(
(_tmpA5D=_region_malloc(r,sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5E.tag=1,((_tmpA5E.f1=Cyc_add_marg,_tmpA5E)))),_tmpA5D)))),((_tmpA61->f5=(
(_tmpA5C="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA5C,sizeof(char),52))),_tmpA61)))))))))))),((_tmp8C9[7]=(
# 1172
(_tmpA67=_region_malloc(r,sizeof(*_tmpA67)),((_tmpA67->f1=((_tmpA66="-l",_tag_dyneither(_tmpA66,sizeof(char),3))),((_tmpA67->f2=1,((_tmpA67->f3=((_tmpA65="<libname>",_tag_dyneither(_tmpA65,sizeof(char),10))),((_tmpA67->f4=(void*)(
(_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=1,((_tmpA64.f1=Cyc_add_libarg,_tmpA64)))),_tmpA63)))),((_tmpA67->f5=(
(_tmpA62="Library file",_tag_dyneither(_tmpA62,sizeof(char),13))),_tmpA67)))))))))))),((_tmp8C9[6]=(
# 1169
(_tmpA6D=_region_malloc(r,sizeof(*_tmpA6D)),((_tmpA6D->f1=((_tmpA6C="-L",_tag_dyneither(_tmpA6C,sizeof(char),3))),((_tmpA6D->f2=1,((_tmpA6D->f3=((_tmpA6B="<dir>",_tag_dyneither(_tmpA6B,sizeof(char),6))),((_tmpA6D->f4=(void*)(
(_tmpA69=_region_malloc(r,sizeof(*_tmpA69)),((_tmpA69[0]=((_tmpA6A.tag=1,((_tmpA6A.f1=Cyc_add_ccarg,_tmpA6A)))),_tmpA69)))),((_tmpA6D->f5=(
(_tmpA68="Add to the list of directories for -l",_tag_dyneither(_tmpA68,sizeof(char),38))),_tmpA6D)))))))))))),((_tmp8C9[5]=(
# 1166
(_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73->f1=((_tmpA72="-I",_tag_dyneither(_tmpA72,sizeof(char),3))),((_tmpA73->f2=1,((_tmpA73->f3=((_tmpA71="<dir>",_tag_dyneither(_tmpA71,sizeof(char),6))),((_tmpA73->f4=(void*)(
(_tmpA6F=_region_malloc(r,sizeof(*_tmpA6F)),((_tmpA6F[0]=((_tmpA70.tag=1,((_tmpA70.f1=Cyc_add_cpparg,_tmpA70)))),_tmpA6F)))),((_tmpA73->f5=(
(_tmpA6E="Add to the list of directories to search for include files",_tag_dyneither(_tmpA6E,sizeof(char),59))),_tmpA73)))))))))))),((_tmp8C9[4]=(
# 1163
(_tmpA79=_region_malloc(r,sizeof(*_tmpA79)),((_tmpA79->f1=((_tmpA78="-B",_tag_dyneither(_tmpA78,sizeof(char),3))),((_tmpA79->f2=1,((_tmpA79->f3=((_tmpA77="<file>",_tag_dyneither(_tmpA77,sizeof(char),7))),((_tmpA79->f4=(void*)(
(_tmpA75=_region_malloc(r,sizeof(*_tmpA75)),((_tmpA75[0]=((_tmpA76.tag=1,((_tmpA76.f1=Cyc_add_cyclone_exec_path,_tmpA76)))),_tmpA75)))),((_tmpA79->f5=(
(_tmpA74="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA74,sizeof(char),60))),_tmpA79)))))))))))),((_tmp8C9[3]=(
# 1160
(_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F->f1=((_tmpA7E="-D",_tag_dyneither(_tmpA7E,sizeof(char),3))),((_tmpA7F->f2=1,((_tmpA7F->f3=((_tmpA7D="<name>[=<value>]",_tag_dyneither(_tmpA7D,sizeof(char),17))),((_tmpA7F->f4=(void*)(
(_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7C.tag=1,((_tmpA7C.f1=Cyc_add_cpparg,_tmpA7C)))),_tmpA7B)))),((_tmpA7F->f5=(
(_tmpA7A="Pass definition to preprocessor",_tag_dyneither(_tmpA7A,sizeof(char),32))),_tmpA7F)))))))))))),((_tmp8C9[2]=(
# 1157
(_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85->f1=((_tmpA84="-o",_tag_dyneither(_tmpA84,sizeof(char),3))),((_tmpA85->f2=0,((_tmpA85->f3=((_tmpA83=" <file>",_tag_dyneither(_tmpA83,sizeof(char),8))),((_tmpA85->f4=(void*)(
(_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=5,((_tmpA82.f1=Cyc_set_output_file,_tmpA82)))),_tmpA81)))),((_tmpA85->f5=(
(_tmpA80="Set the output file name to <file>",_tag_dyneither(_tmpA80,sizeof(char),35))),_tmpA85)))))))))))),((_tmp8C9[1]=(
# 1154
(_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B->f1=((_tmpA8A="--version",_tag_dyneither(_tmpA8A,sizeof(char),10))),((_tmpA8B->f2=0,((_tmpA8B->f3=((_tmpA89="",_tag_dyneither(_tmpA89,sizeof(char),1))),((_tmpA8B->f4=(void*)(
(_tmpA87=_region_malloc(r,sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=0,((_tmpA88.f1=Cyc_print_version,_tmpA88)))),_tmpA87)))),((_tmpA8B->f5=(
(_tmpA86="Print version information and exit",_tag_dyneither(_tmpA86,sizeof(char),35))),_tmpA8B)))))))))))),((_tmp8C9[0]=(
# 1151
(_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91->f1=((_tmpA90="-v",_tag_dyneither(_tmpA90,sizeof(char),3))),((_tmpA91->f2=0,((_tmpA91->f3=((_tmpA8F="",_tag_dyneither(_tmpA8F,sizeof(char),1))),((_tmpA91->f4=(void*)(
(_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=3,((_tmpA8E.f1=& Cyc_v_r,_tmpA8E)))),_tmpA8D)))),((_tmpA91->f5=(
(_tmpA8C="Print compilation stages verbosely",_tag_dyneither(_tmpA8C,sizeof(char),35))),_tmpA91)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8C9,sizeof(struct _tuple24*),76)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1383
const char*_tmpA92;struct _dyneither_ptr optstring=(_tmpA92="Options:",_tag_dyneither(_tmpA92,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp171);
# 1388
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1396
char*_tmpA93;struct _dyneither_ptr cyclone_exec_prefix=(_tmpA93=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA93,sizeof(char),_get_zero_arr_size_char((void*)_tmpA93,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpA96;struct Cyc_List_List*_tmpA95;Cyc_cyclone_exec_path=((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->hd=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=cyclone_exec_prefix,_tmpA96)))),((_tmpA95->tl=Cyc_cyclone_exec_path,_tmpA95))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpA9C;const char*_tmpA9B;struct Cyc_List_List*_tmpA9A;Cyc_cyclone_exec_path=(
(_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA9B="cyc-lib",_tag_dyneither(_tmpA9B,sizeof(char),8)))),_tmpA9C)))),((_tmpA9A->tl=Cyc_cyclone_exec_path,_tmpA9A))))));}
# 1404
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1412
const char*_tmpA9D;struct _dyneither_ptr _tmp178=Cyc_do_find(cyclone_arch_path,((_tmpA9D="cycspecs",_tag_dyneither(_tmpA9D,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpAA1;void*_tmpAA0[1];struct Cyc_String_pa_PrintArg_struct _tmpA9F;(_tmpA9F.tag=0,((_tmpA9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp178),((_tmpAA0[0]=& _tmpA9F,Cyc_fprintf(Cyc_stderr,((_tmpAA1="Reading from specs file %s\n",_tag_dyneither(_tmpAA1,sizeof(char),28))),_tag_dyneither(_tmpAA0,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp17C=Cyc_read_specs(_tmp178);
struct _dyneither_ptr _tmp17D=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp17E;_push_handler(& _tmp17E);{int _tmp180=0;if(setjmp(_tmp17E.handler))_tmp180=1;if(!_tmp180){
{struct _dyneither_ptr _tmp181=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp17C,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp17D=Cyc_split_specs(_tmp181);}
# 1417
;_pop_handler();}else{void*_tmp17F=(void*)_exn_thrown;void*_tmp184=_tmp17F;void*_tmp186;_LL35: {struct Cyc_Core_Not_found_exn_struct*_tmp185=(struct Cyc_Core_Not_found_exn_struct*)_tmp184;if(_tmp185->tag != Cyc_Core_Not_found)goto _LL37;}_LL36:
# 1420
 goto _LL34;_LL37: _tmp186=_tmp184;_LL38:(void)_throw(_tmp186);_LL34:;}};}
# 1422
if((struct _dyneither_ptr*)_tmp17D.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp17D);}
# 1427
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp172);{
const char*_tmpAA2;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpAA2="",_tag_dyneither(_tmpAA2,sizeof(char),1))))== 0){
# 1445 "cyclone.cyc"
const char*_tmp188=Cyc_produce_dependencies?"":" -E";
const char*_tmpAAA;void*_tmpAA9[3];struct Cyc_String_pa_PrintArg_struct _tmpAA8;const char*_tmpAA7;struct Cyc_String_pa_PrintArg_struct _tmpAA6;struct Cyc_String_pa_PrintArg_struct _tmpAA5;Cyc_set_cpp((struct _dyneither_ptr)((_tmpAA5.tag=0,((_tmpAA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp178),((_tmpAA6.tag=0,((_tmpAA6.f1=(struct _dyneither_ptr)((_tmpAA7=_tmp188,_tag_dyneither(_tmpAA7,sizeof(char),_get_zero_arr_size_char((void*)_tmpAA7,1)))),((_tmpAA8.tag=0,((_tmpAA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAA9[0]=& _tmpAA8,((_tmpAA9[1]=& _tmpAA6,((_tmpAA9[2]=& _tmpAA5,Cyc_aprintf(((_tmpAAA="%s -w -x c%s -specs %s",_tag_dyneither(_tmpAAA,sizeof(char),23))),_tag_dyneither(_tmpAA9,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1449
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,Cyc_fprintf(Cyc_stderr,((_tmpAAD="missing file\n",_tag_dyneither(_tmpAAD,sizeof(char),14))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}
exit(1);}
# 1458
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpAAE;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAAE="cyc_setjmp.h",_tag_dyneither(_tmpAAE,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAAF;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAAF="cyc_include.h",_tag_dyneither(_tmpAAF,sizeof(char),14))));}
# 1464
{struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp36B != 0;_tmp36B=_tmp36B->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp36B->hd));
if(Cyc_compile_failure){int _tmp36C=1;_npop_handler(0);return _tmp36C;}}}
# 1469
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp36D=0;_npop_handler(0);return _tmp36D;}
# 1475
if(Cyc_ccargs == 0){int _tmp36E=0;_npop_handler(0);return _tmp36E;}
# 1478
{const char*_tmpAB0;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAB0="-L",_tag_dyneither(_tmpAB0,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1480
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAB1;struct _dyneither_ptr _tmp370=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAB1=" ",_tag_dyneither(_tmpAB1,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpAB3;struct Cyc_List_List*_tmpAB2;struct _dyneither_ptr _tmp371=Cyc_str_sepstr(((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAB2->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAB2)))))),((_tmpAB3=" ",_tag_dyneither(_tmpAB3,sizeof(char),2))));
# 1486
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpAB5;const char*_tmpAB4;int _tmp372=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1491
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAB5=".a",_tag_dyneither(_tmpAB5,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAB4=".lib",_tag_dyneither(_tmpAB4,sizeof(char),5))));
if(_tmp372){
stdlib=0;{
const char*_tmpAB6;stdlib_string=(struct _dyneither_ptr)((_tmpAB6="",_tag_dyneither(_tmpAB6,sizeof(char),1)));};}else{
# 1499
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpAB7;libcyc_flag=((_tmpAB7="-lcyc_a",_tag_dyneither(_tmpAB7,sizeof(char),8)));}
{const char*_tmpAB8;nogc_filename=((_tmpAB8="nogc_a.a",_tag_dyneither(_tmpAB8,sizeof(char),9)));}{
const char*_tmpAB9;runtime_filename=((_tmpAB9="runtime_cyc_a.o",_tag_dyneither(_tmpAB9,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpABA;libcyc_flag=((_tmpABA="-lcyc_nocheck",_tag_dyneither(_tmpABA,sizeof(char),14)));}
{const char*_tmpABB;nogc_filename=((_tmpABB="nogc.a",_tag_dyneither(_tmpABB,sizeof(char),7)));}{
const char*_tmpABC;runtime_filename=((_tmpABC="runtime_cyc.o",_tag_dyneither(_tmpABC,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpABD;libcyc_flag=((_tmpABD="-lcyc_pg",_tag_dyneither(_tmpABD,sizeof(char),9)));}
{const char*_tmpABE;runtime_filename=((_tmpABE="runtime_cyc_pg.o",_tag_dyneither(_tmpABE,sizeof(char),17)));}{
const char*_tmpABF;nogc_filename=((_tmpABF="nogc_pg.a",_tag_dyneither(_tmpABF,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1517
{const char*_tmpAC0;libcyc_flag=((_tmpAC0="-lcycboot",_tag_dyneither(_tmpAC0,sizeof(char),10)));}
{const char*_tmpAC1;nogc_filename=((_tmpAC1="nogc.a",_tag_dyneither(_tmpAC1,sizeof(char),7)));}{
const char*_tmpAC2;runtime_filename=((_tmpAC2="runtime_cyc.o",_tag_dyneither(_tmpAC2,sizeof(char),14)));};}else{
# 1521
{const char*_tmpAC3;libcyc_flag=((_tmpAC3="-lcyc",_tag_dyneither(_tmpAC3,sizeof(char),6)));}
{const char*_tmpAC4;nogc_filename=((_tmpAC4="nogc.a",_tag_dyneither(_tmpAC4,sizeof(char),7)));}{
const char*_tmpAC5;runtime_filename=((_tmpAC5="runtime_cyc.o",_tag_dyneither(_tmpAC5,sizeof(char),14)));};}}}}
# 1525
{const char*_tmpAC6;gc_filename=((_tmpAC6="gc.a",_tag_dyneither(_tmpAC6,sizeof(char),5)));}{
# 1527
struct _dyneither_ptr _tmp384=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp385=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1534
stdlib=0;{
const char*_tmpACC;void*_tmpACB[3];struct Cyc_String_pa_PrintArg_struct _tmpACA;struct Cyc_String_pa_PrintArg_struct _tmpAC9;struct Cyc_String_pa_PrintArg_struct _tmpAC8;stdlib_string=(struct _dyneither_ptr)((_tmpAC8.tag=0,((_tmpAC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp384),((_tmpAC9.tag=0,((_tmpAC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpACA.tag=0,((_tmpACA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp385),((_tmpACB[0]=& _tmpACA,((_tmpACB[1]=& _tmpAC9,((_tmpACB[2]=& _tmpAC8,Cyc_aprintf(((_tmpACC=" %s %s %s",_tag_dyneither(_tmpACC,sizeof(char),10))),_tag_dyneither(_tmpACB,sizeof(void*),3))))))))))))))))))));};};}
# 1538
if(Cyc_ic_r){struct _handler_cons _tmp38B;_push_handler(& _tmp38B);{int _tmp38D=0;if(setjmp(_tmp38B.handler))_tmp38D=1;if(!_tmp38D){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp372){
struct Cyc_List_List*_tmpACD;_tmp38E=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->hd=Cyc_final_strptr,((_tmpACD->tl=_tmp38E,_tmpACD))))));}{
# 1546
struct Cyc_Interface_I*_tmp390=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp38E,_tmp38E);
if(_tmp390 == 0){
{const char*_tmpAD0;void*_tmpACF;(_tmpACF=0,Cyc_fprintf(Cyc_stderr,((_tmpAD0="Error: interfaces incompatible\n",_tag_dyneither(_tmpAD0,sizeof(char),32))),_tag_dyneither(_tmpACF,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp393=1;_npop_handler(1);return _tmp393;};}
# 1553
if(_tmp372){
if(Cyc_output_file != 0){
const char*_tmpAD4;void*_tmpAD3[1];struct Cyc_String_pa_PrintArg_struct _tmpAD2;struct _dyneither_ptr _tmp394=(_tmpAD2.tag=0,((_tmpAD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAD3[0]=& _tmpAD2,Cyc_aprintf(((_tmpAD4="%s.cycio",_tag_dyneither(_tmpAD4,sizeof(char),9))),_tag_dyneither(_tmpAD3,sizeof(void*),1)))))));
const char*_tmpAD6;const char*_tmpAD5;struct Cyc___cycFILE*_tmp395=Cyc_try_file_open((struct _dyneither_ptr)_tmp394,((_tmpAD5="wb",_tag_dyneither(_tmpAD5,sizeof(char),3))),((_tmpAD6="interface object file",_tag_dyneither(_tmpAD6,sizeof(char),22))));
if(_tmp395 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp396=1;_npop_handler(1);return _tmp396;};}
# 1562
Cyc_Interface_save(_tmp390,_tmp395);
Cyc_file_close(_tmp395);}}else{
# 1566
const char*_tmpADB;const char*_tmpADA;struct _tuple20*_tmpAD9;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp390,(
(_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->f1=((_tmpADA="empty interface",_tag_dyneither(_tmpADA,sizeof(char),16))),((_tmpAD9->f2=((_tmpADB="global interface",_tag_dyneither(_tmpADB,sizeof(char),17))),_tmpAD9)))))))){
{const char*_tmpADE;void*_tmpADD;(_tmpADD=0,Cyc_fprintf(Cyc_stderr,((_tmpADE="Error: some objects are still undefined\n",_tag_dyneither(_tmpADE,sizeof(char),41))),_tag_dyneither(_tmpADD,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A1=1;_npop_handler(1);return _tmp3A1;};}}};};
# 1539
;_pop_handler();}else{void*_tmp38C=(void*)_exn_thrown;void*_tmp3A3=_tmp38C;void*_tmp3A4;void*_tmp3A5;_LL3A: _tmp3A4=_tmp3A3;_LL3B:
# 1575
{void*_tmp3A6=_tmp3A4;struct _dyneither_ptr _tmp3A8;struct _dyneither_ptr _tmp3AA;struct _dyneither_ptr _tmp3AD;_LL3F: {struct Cyc_Core_Failure_exn_struct*_tmp3A7=(struct Cyc_Core_Failure_exn_struct*)_tmp3A6;if(_tmp3A7->tag != Cyc_Core_Failure)goto _LL41;else{_tmp3A8=_tmp3A7->f1;}}_LL40:
# 1577
{const char*_tmpAE2;void*_tmpAE1[1];struct Cyc_String_pa_PrintArg_struct _tmpAE0;(_tmpAE0.tag=0,((_tmpAE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A8),((_tmpAE1[0]=& _tmpAE0,Cyc_fprintf(Cyc_stderr,((_tmpAE2="Exception Core::Failure %s\n",_tag_dyneither(_tmpAE2,sizeof(char),28))),_tag_dyneither(_tmpAE1,sizeof(void*),1)))))));}goto _LL3E;_LL41: {struct Cyc_Core_Impossible_exn_struct*_tmp3A9=(struct Cyc_Core_Impossible_exn_struct*)_tmp3A6;if(_tmp3A9->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp3AA=_tmp3A9->f1;}}_LL42:
# 1579
{const char*_tmpAE6;void*_tmpAE5[1];struct Cyc_String_pa_PrintArg_struct _tmpAE4;(_tmpAE4.tag=0,((_tmpAE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AA),((_tmpAE5[0]=& _tmpAE4,Cyc_fprintf(Cyc_stderr,((_tmpAE6="Exception Core::Impossible %s\n",_tag_dyneither(_tmpAE6,sizeof(char),31))),_tag_dyneither(_tmpAE5,sizeof(void*),1)))))));}goto _LL3E;_LL43: {struct Cyc_Dict_Absent_exn_struct*_tmp3AB=(struct Cyc_Dict_Absent_exn_struct*)_tmp3A6;if(_tmp3AB->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:
# 1581
{const char*_tmpAE9;void*_tmpAE8;(_tmpAE8=0,Cyc_fprintf(Cyc_stderr,((_tmpAE9="Exception Dict::Absent\n",_tag_dyneither(_tmpAE9,sizeof(char),24))),_tag_dyneither(_tmpAE8,sizeof(void*),0)));}goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp3AC=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp3A6;if(_tmp3AC->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp3AD=_tmp3AC->f1;}}_LL46:
# 1583
{const char*_tmpAED;void*_tmpAEC[1];struct Cyc_String_pa_PrintArg_struct _tmpAEB;(_tmpAEB.tag=0,((_tmpAEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AD),((_tmpAEC[0]=& _tmpAEB,Cyc_fprintf(Cyc_stderr,((_tmpAED="Exception Core::Invalid_argument %s\n",_tag_dyneither(_tmpAED,sizeof(char),37))),_tag_dyneither(_tmpAEC,sizeof(void*),1)))))));}goto _LL3E;_LL47:;_LL48:
# 1585
{const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,Cyc_fprintf(Cyc_stderr,((_tmpAF0="Uncaught exception\n",_tag_dyneither(_tmpAF0,sizeof(char),20))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}goto _LL3E;_LL3E:;}
# 1587
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3BB=1;_npop_handler(0);return _tmp3BB;};_LL3C: _tmp3A5=_tmp3A3;_LL3D:(void)_throw(_tmp3A5);_LL39:;}};}{
# 1592
const char*_tmpAF1;struct _dyneither_ptr outfile_str=(_tmpAF1="",_tag_dyneither(_tmpAF1,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpAF5;void*_tmpAF4[1];struct Cyc_String_pa_PrintArg_struct _tmpAF3;outfile_str=(struct _dyneither_ptr)((_tmpAF3.tag=0,((_tmpAF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAF4[0]=& _tmpAF3,Cyc_aprintf(((_tmpAF5=" -o %s",_tag_dyneither(_tmpAF5,sizeof(char),7))),_tag_dyneither(_tmpAF4,sizeof(void*),1))))))));}{
# 1596
const char*_tmpAFD;void*_tmpAFC[5];struct Cyc_String_pa_PrintArg_struct _tmpAFB;struct Cyc_String_pa_PrintArg_struct _tmpAFA;struct Cyc_String_pa_PrintArg_struct _tmpAF9;struct Cyc_String_pa_PrintArg_struct _tmpAF8;struct Cyc_String_pa_PrintArg_struct _tmpAF7;struct _dyneither_ptr _tmp3BF=
(_tmpAF7.tag=0,((_tmpAF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp371),((_tmpAF8.tag=0,((_tmpAF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpAF9.tag=0,((_tmpAF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp370),((_tmpAFA.tag=0,((_tmpAFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAFB.tag=0,((_tmpAFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAFC[0]=& _tmpAFB,((_tmpAFC[1]=& _tmpAFA,((_tmpAFC[2]=& _tmpAF9,((_tmpAFC[3]=& _tmpAF8,((_tmpAFC[4]=& _tmpAF7,Cyc_aprintf(((_tmpAFD="%s %s %s%s%s",_tag_dyneither(_tmpAFD,sizeof(char),13))),_tag_dyneither(_tmpAFC,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1599
if(Cyc_v_r){{const char*_tmpB01;void*_tmpB00[1];struct Cyc_String_pa_PrintArg_struct _tmpAFF;(_tmpAFF.tag=0,((_tmpAFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3BF),((_tmpB00[0]=& _tmpAFF,Cyc_fprintf(Cyc_stderr,((_tmpB01="%s\n",_tag_dyneither(_tmpB01,sizeof(char),4))),_tag_dyneither(_tmpB00,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3BF,sizeof(char),1))!= 0){
{const char*_tmpB04;void*_tmpB03;(_tmpB03=0,Cyc_fprintf(Cyc_stderr,((_tmpB04="Error: C compiler failed\n",_tag_dyneither(_tmpB04,sizeof(char),26))),_tag_dyneither(_tmpB03,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3C5=1;_npop_handler(0);return _tmp3C5;};}
# 1606
Cyc_remove_cfiles();{
# 1608
int _tmp3C6=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3C6;};};};};};
# 1119 "cyclone.cyc"
;_pop_region(r);};}
