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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
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
# 129 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
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
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 945
extern void*Cyc_Absyn_sint_typ;
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1131
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 131 "dict.h"
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 176
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 70 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 74
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo);
# 76
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 79
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 76 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;
# 40 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 95
int Cyc_Tcutil_is_function_type(void*t);
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 342 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 38 "interface.cyc"
void Cyc_Lex_lex_init();
# 45
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};
# 57
typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;struct Cyc_Interface_I{struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};
# 65
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
struct Cyc_Interface_Ienv*_tmp247;return(_tmp247=_cycalloc(sizeof(*_tmp247)),((_tmp247->aggrdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp247->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp247->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp247->typedefdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp247->vardecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp247->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp247)))))))))))));}
# 76
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv*ie=0;
if(ie == 0)
ie=Cyc_Interface_new_ienv();
return ie;}
# 83
static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(void*ignored){
return Cyc_Interface_new_ienv();}
# 87
static struct Cyc_Interface_I*Cyc_Interface_skel2i(struct Cyc_Dict_Dict skel){
struct Cyc_Interface_I*_tmp248;return(_tmp248=_cycalloc(sizeof(*_tmp248)),((_tmp248->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),((_tmp248->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),_tmp248)))));}
# 91
struct Cyc_Interface_I*Cyc_Interface_empty(){
struct Cyc_Interface_I*_tmp249;return(_tmp249=_cycalloc(sizeof(*_tmp249)),((_tmp249->imports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),((_tmp249->exports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),_tmp249)))));}
# 97
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();
# 100
struct Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();
struct Cyc_Absyn_Datatypedecl*_tmp24A;struct Cyc_Absyn_Datatypedecl*_tmp5=(_tmp24A=_cycalloc(sizeof(*_tmp24A)),((_tmp24A[0]=*Cyc_Absyn_exn_tud,_tmp24A)));
_tmp5->sc=Cyc_Absyn_Public;
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,_tmp5);
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct Cyc_Absyn_Datatypefield*_tmp24B;struct Cyc_Absyn_Datatypefield*_tmp7=(_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd),_tmp24B)));
_tmp7->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp24C;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=(_tmp24C=_cycalloc(sizeof(*_tmp24C)),((_tmp24C->base=_tmp5,((_tmp24C->field=_tmp7,_tmp24C)))));
_tmp4->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,_tmp8);};}}
# 111
_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,_tmp4);{
# 113
struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();
struct _dyneither_ptr*_tmp254;const char*_tmp253;union Cyc_Absyn_Nmspace _tmp252;struct _tuple0*_tmp251;struct _tuple0*qmain=(_tmp251=_cycalloc(sizeof(*_tmp251)),((_tmp251->f1=(((_tmp252.Abs_n).val=0,(((_tmp252.Abs_n).tag=2,_tmp252)))),((_tmp251->f2=((_tmp254=_cycalloc(sizeof(*_tmp254)),((_tmp254[0]=(struct _dyneither_ptr)((_tmp253="main",_tag_dyneither(_tmp253,sizeof(char),5))),_tmp254)))),_tmp251)))));
struct Cyc_List_List*_tmp25C;struct _tuple8*_tmp25B;struct _tuple8*_tmp25A;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(qmain,
Cyc_Absyn_function_typ(0,0,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_sint_typ,(
(_tmp259=_cycalloc(sizeof(*_tmp259)),((_tmp259->hd=((_tmp25A=_cycalloc(sizeof(*_tmp25A)),((_tmp25A->f1=0,((_tmp25A->f2=Cyc_Absyn_empty_tqual(0),((_tmp25A->f3=Cyc_Absyn_uint_typ,_tmp25A)))))))),((_tmp259->tl=(
(_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C->hd=((_tmp25B=_cycalloc(sizeof(*_tmp25B)),((_tmp25B->f1=0,((_tmp25B->f2=Cyc_Absyn_empty_tqual(0),((_tmp25B->f3=
Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
# 121
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(void*)& Cyc_Absyn_HeapRgn_val,
# 123
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp25B)))))))),((_tmp25C->tl=0,_tmp25C)))))),_tmp259)))))),0,0,0,0),0);
# 127
_tmpC->sc=Cyc_Absyn_Extern;
_tmpB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);
_tmp3->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);
# 131
return _tmp3;};}
# 151 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
void*_tmp25D;(_tmp25D=0,Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp25D,sizeof(void*),0)));}
# 154
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
struct Cyc_Core_Invalid_argument_exn_struct _tmp260;struct Cyc_Core_Invalid_argument_exn_struct*_tmp25F;(int)_throw((void*)((_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F[0]=((_tmp260.tag=Cyc_Core_Invalid_argument,((_tmp260.f1=s,_tmp260)))),_tmp25F)))));}
# 157
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
const char*_tmp266;void*_tmp265[3];struct Cyc_String_pa_PrintArg_struct _tmp264;struct Cyc_String_pa_PrintArg_struct _tmp263;struct Cyc_String_pa_PrintArg_struct _tmp262;Cyc_Interface_err((struct _dyneither_ptr)((_tmp262.tag=0,((_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n)),((_tmp263.tag=0,((_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp264.tag=0,((_tmp264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((_tmp265[0]=& _tmp264,((_tmp265[1]=& _tmp263,((_tmp265[2]=& _tmp262,Cyc_aprintf(((_tmp266="fields of %s %s %s have never been defined",_tag_dyneither(_tmp266,sizeof(char),43))),_tag_dyneither(_tmp265,sizeof(void*),3)))))))))))))))))))));}
# 161
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
const char*_tmp26B;void*_tmp26A[2];struct Cyc_String_pa_PrintArg_struct _tmp269;struct Cyc_String_pa_PrintArg_struct _tmp268;Cyc_Interface_err((struct _dyneither_ptr)((_tmp268.tag=0,((_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n)),((_tmp269.tag=0,((_tmp269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((_tmp26A[0]=& _tmp269,((_tmp26A[1]=& _tmp268,Cyc_aprintf(((_tmp26B="the body of %s function %s has never been defined",_tag_dyneither(_tmp26B,sizeof(char),50))),_tag_dyneither(_tmp26A,sizeof(void*),2)))))))))))))));}
# 167
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){
const char*_tmp272;void*_tmp271[4];struct Cyc_String_pa_PrintArg_struct _tmp270;struct Cyc_String_pa_PrintArg_struct _tmp26F;struct Cyc_String_pa_PrintArg_struct _tmp26E;struct Cyc_String_pa_PrintArg_struct _tmp26D;Cyc_Interface_err((struct _dyneither_ptr)((_tmp26D.tag=0,((_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 171
Cyc_Absynpp_qvar2string(name2)),((_tmp26E.tag=0,((_tmp26E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp26F.tag=0,((_tmp26F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 170
Cyc_Absynpp_qvar2string(name1)),((_tmp270.tag=0,((_tmp270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((_tmp271[0]=& _tmp270,((_tmp271[1]=& _tmp26F,((_tmp271[2]=& _tmp26E,((_tmp271[3]=& _tmp26D,Cyc_aprintf(((_tmp272="declaration of %s %s relies on static %s %s",_tag_dyneither(_tmp272,sizeof(char),44))),_tag_dyneither(_tmp271,sizeof(void*),4)))))))))))))))))))))))))));}}
# 175
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){
const char*_tmp279;void*_tmp278[4];struct Cyc_String_pa_PrintArg_struct _tmp277;struct Cyc_String_pa_PrintArg_struct _tmp276;struct Cyc_String_pa_PrintArg_struct _tmp275;struct Cyc_String_pa_PrintArg_struct _tmp274;Cyc_Interface_err((struct _dyneither_ptr)((_tmp274.tag=0,((_tmp274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 179
Cyc_Absynpp_qvar2string(name2)),((_tmp275.tag=0,((_tmp275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp276.tag=0,((_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 178
Cyc_Absynpp_qvar2string(name1)),((_tmp277.tag=0,((_tmp277.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((_tmp278[0]=& _tmp277,((_tmp278[1]=& _tmp276,((_tmp278[2]=& _tmp275,((_tmp278[3]=& _tmp274,Cyc_aprintf(((_tmp279="declaration of %s %s relies on fields of abstract %s %s",_tag_dyneither(_tmp279,sizeof(char),56))),_tag_dyneither(_tmp278,sizeof(void*),4)))))))))))))))))))))))))));}}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 190
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;
# 192
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
struct Cyc_Interface_Seen*_tmp27A;return(_tmp27A=_cycalloc(sizeof(*_tmp27A)),((_tmp27A->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp27A->datatypes=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp27A)))));}
# 198
typedef struct Cyc_Dict_Dict Cyc_Interface_aenv_t;
# 200
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 203
static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 205
int _tmp2F=1;
for(0;l != 0;l=l->tl){
if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f(l->hd)))
_tmp2F=0;}
return _tmp2F;}
# 211
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp35=_tmp31;void*_tmp37;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp36=(struct Cyc_Dict_Absent_exn_struct*)_tmp35;if(_tmp36->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
 goto _LL0;_LL3: _tmp37=_tmp35;_LL4:(void)_throw(_tmp37);_LL0:;}};}{
int _tmp38=1;
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp38);
if(d->impl != 0){
struct Cyc_List_List*_tmp39=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp39 != 0;_tmp39=_tmp39->tl){
const char*_tmp27B;if(!Cyc_Interface_check_public_type(ae,seen,((_tmp27B="type",_tag_dyneither(_tmp27B,sizeof(char),5))),d->name,((struct Cyc_Absyn_Aggrfield*)_tmp39->hd)->type))
_tmp38=0;}}
# 221
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp38);
return _tmp38;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 225
static void*Cyc_Interface_get_type1(struct _tuple19*x){
return(*x).f2;}
# 228
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 232
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp3B;_push_handler(& _tmp3B);{int _tmp3D=0;if(setjmp(_tmp3B.handler))_tmp3D=1;if(!_tmp3D){{int _tmp3E=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp3E;};_pop_handler();}else{void*_tmp3C=(void*)_exn_thrown;void*_tmp40=_tmp3C;void*_tmp42;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp41=(struct Cyc_Dict_Absent_exn_struct*)_tmp40;if(_tmp41->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
 goto _LL5;_LL8: _tmp42=_tmp40;_LL9:(void)_throw(_tmp42);_LL5:;}};}{
int _tmp43=1;
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp43);
if(d->fields != 0){
struct Cyc_List_List*_tmp44=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp44 != 0;_tmp44=_tmp44->tl){
const char*_tmp27C;if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple19*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,((_tmp27C="datatype",_tag_dyneither(_tmp27C,sizeof(char),9))),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp44->hd)->typs))
# 241
_tmp43=0;}}
# 243
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp43);
return _tmp43;};}
# 247
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 251
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,_tag_dyneither(0,0,0),d->name,(void*)_check_null(d->defn));else{
return 1;}}
# 258
static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
const char*_tmp27D;return Cyc_Interface_check_public_type(ae,seen,((_tmp27D="variable",_tag_dyneither(_tmp27D,sizeof(char),9))),d->name,d->type);}
# 262
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns){
union Cyc_Absyn_Nmspace _tmp47=ns;struct Cyc_List_List*_tmp48;struct Cyc_List_List*_tmp49;_LLB: if((_tmp47.Abs_n).tag != 2)goto _LLD;_tmp48=(struct Cyc_List_List*)(_tmp47.Abs_n).val;_LLC:
 return _tmp48;_LLD: if((_tmp47.C_n).tag != 3)goto _LLF;_tmp49=(struct Cyc_List_List*)(_tmp47.C_n).val;_LLE:
 return _tmp49;_LLF:;_LL10: {
const char*_tmp27E;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp27E="get_abs_ns",_tag_dyneither(_tmp27E,sizeof(char),11))));}_LLA:;}
# 270
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4D;void*_tmp4F;void*_tmp51;void*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_Absyn_VarargInfo*_tmp55;struct Cyc_List_List*_tmp57;union Cyc_Absyn_AggrInfoU _tmp59;struct Cyc_List_List*_tmp5A;struct _tuple0*_tmp5C;struct Cyc_Absyn_Datatypedecl*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_Datatypedecl*_tmp61;struct Cyc_Absyn_Datatypefield*_tmp62;struct Cyc_List_List*_tmp63;_LL12: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B;if(_tmp4C->tag != 5)goto _LL14;else{_tmp4D=(_tmp4C->f1).elt_typ;}}_LL13:
 _tmp4F=_tmp4D;goto _LL15;_LL14: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B;if(_tmp4E->tag != 8)goto _LL16;else{_tmp4F=(_tmp4E->f1).elt_type;}}_LL15:
 _tmp51=_tmp4F;goto _LL17;_LL16:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B;if(_tmp50->tag != 17)goto _LL18;else{_tmp51=(void*)_tmp50->f4;}}if(!(_tmp51 != 0))goto _LL18;_LL17:
# 275
 return Cyc_Interface_check_public_type(ae,seen,obj,name,(void*)_check_null(_tmp51));_LL18: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4B;if(_tmp52->tag != 9)goto _LL1A;else{_tmp53=(_tmp52->f1).ret_typ;_tmp54=(_tmp52->f1).args;_tmp55=(_tmp52->f1).cyc_varargs;}}_LL19: {
# 278
int b=((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp54) && 
Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp53);
if(_tmp55 != 0){
void*_tmp65;struct Cyc_Absyn_VarargInfo _tmp64=*_tmp55;_tmp65=_tmp64.type;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp65);}
# 284
return b;}_LL1A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4B;if(_tmp56->tag != 10)goto _LL1C;else{_tmp57=_tmp56->f1;}}_LL1B:
# 287
 return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple19*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp57);_LL1C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp58=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B;if(_tmp58->tag != 11)goto _LL1E;else{_tmp59=(_tmp58->f1).aggr_info;_tmp5A=(_tmp58->f1).targs;}}_LL1D: {
# 290
struct Cyc_Absyn_Aggrdecl*_tmp66=Cyc_Absyn_get_known_aggrdecl(_tmp59);
if(_tmp66->sc == Cyc_Absyn_Static){
{const char*_tmp27F;Cyc_Interface_static_err(obj,name,((_tmp27F="type",_tag_dyneither(_tmp27F,sizeof(char),5))),_tmp66->name);}
return 0;}
# 295
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp5A) && 
Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp66);}_LL1E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4B;if(_tmp5B->tag != 13)goto _LL20;else{_tmp5C=_tmp5B->f1;}}_LL1F: {
# 299
union Cyc_Absyn_Nmspace _tmp69;struct _dyneither_ptr*_tmp6A;struct _tuple0*_tmp68=_tmp5C;_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;{
struct Cyc_List_List*_tmp6B=Cyc_Interface_get_abs_ns(_tmp69);
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp6C;_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){
{struct Cyc_Tcenv_Genv*_tmp6F=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp6B);
ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6F->enumdecls,_tmp6A);}
# 303
;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;void*_tmp71=_tmp6D;void*_tmp73;_LL27: {struct Cyc_Dict_Absent_exn_struct*_tmp72=(struct Cyc_Dict_Absent_exn_struct*)_tmp71;if(_tmp72->tag != Cyc_Dict_Absent)goto _LL29;}_LL28: {
# 307
const char*_tmp283;void*_tmp282[1];struct Cyc_String_pa_PrintArg_struct _tmp281;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp281.tag=0,((_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5C)),((_tmp282[0]=& _tmp281,Cyc_aprintf(((_tmp283="check_public_type (can't find enum %s)",_tag_dyneither(_tmp283,sizeof(char),39))),_tag_dyneither(_tmp282,sizeof(void*),1)))))))));}_LL29: _tmp73=_tmp71;_LL2A:(void)_throw(_tmp73);_LL26:;}};}
# 310
if(ed->sc == Cyc_Absyn_Static){
{const char*_tmp284;Cyc_Interface_static_err(obj,_tmp5C,((_tmp284="enum",_tag_dyneither(_tmp284,sizeof(char),5))),ed->name);}
return 0;}
# 314
return 1;};}_LL20: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4B;if(_tmp5D->tag != 3)goto _LL22;else{if((((_tmp5D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL22;_tmp5E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5D->f1).datatype_info).KnownDatatype).val);_tmp5F=(_tmp5D->f1).targs;}}_LL21: {
# 317
union Cyc_Absyn_Nmspace _tmp79;struct _dyneither_ptr*_tmp7A;struct _tuple0*_tmp78=_tmp5E->name;_tmp79=_tmp78->f1;_tmp7A=_tmp78->f2;{
struct Cyc_List_List*_tmp7B=Cyc_Interface_get_abs_ns(_tmp79);
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp7C;_push_handler(& _tmp7C);{int _tmp7E=0;if(setjmp(_tmp7C.handler))_tmp7E=1;if(!_tmp7E){
{struct Cyc_Tcenv_Genv*_tmp7F=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp7B);
tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F->datatypedecls,_tmp7A);}
# 321
;_pop_handler();}else{void*_tmp7D=(void*)_exn_thrown;void*_tmp81=_tmp7D;void*_tmp83;_LL2C: {struct Cyc_Dict_Absent_exn_struct*_tmp82=(struct Cyc_Dict_Absent_exn_struct*)_tmp81;if(_tmp82->tag != Cyc_Dict_Absent)goto _LL2E;}_LL2D: {
# 324
const char*_tmp288;void*_tmp287[1];struct Cyc_String_pa_PrintArg_struct _tmp286;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp286.tag=0,((_tmp286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5E->name)),((_tmp287[0]=& _tmp286,Cyc_aprintf(((_tmp288="check_public_type (can't find datatype %s)",_tag_dyneither(_tmp288,sizeof(char),43))),_tag_dyneither(_tmp287,sizeof(void*),1)))))))));}_LL2E: _tmp83=_tmp81;_LL2F:(void)_throw(_tmp83);_LL2B:;}};}
# 327
if(tud->sc == Cyc_Absyn_Static){
{const char*_tmp289;Cyc_Interface_static_err(obj,name,((_tmp289="datatype",_tag_dyneither(_tmp289,sizeof(char),9))),tud->name);}
return 0;}
# 331
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp5F) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};}_LL22: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4B;if(_tmp60->tag != 4)goto _LL24;else{if((((_tmp60->f1).field_info).KnownDatatypefield).tag != 2)goto _LL24;_tmp61=((struct _tuple1)(((_tmp60->f1).field_info).KnownDatatypefield).val).f1;_tmp62=((struct _tuple1)(((_tmp60->f1).field_info).KnownDatatypefield).val).f2;_tmp63=(_tmp60->f1).targs;}}_LL23: {
# 335
union Cyc_Absyn_Nmspace _tmp89;struct _dyneither_ptr*_tmp8A;struct _tuple0*_tmp88=_tmp61->name;_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;{
struct Cyc_List_List*_tmp8B=Cyc_Interface_get_abs_ns(_tmp89);
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp8C;_push_handler(& _tmp8C);{int _tmp8E=0;if(setjmp(_tmp8C.handler))_tmp8E=1;if(!_tmp8E){
{struct Cyc_Tcenv_Genv*_tmp8F=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp8B);
tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8F->datatypedecls,_tmp8A);}
# 339
;_pop_handler();}else{void*_tmp8D=(void*)_exn_thrown;void*_tmp91=_tmp8D;void*_tmp93;_LL31: {struct Cyc_Dict_Absent_exn_struct*_tmp92=(struct Cyc_Dict_Absent_exn_struct*)_tmp91;if(_tmp92->tag != Cyc_Dict_Absent)goto _LL33;}_LL32: {
# 343
const char*_tmp28D;void*_tmp28C[1];struct Cyc_String_pa_PrintArg_struct _tmp28B;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp28B.tag=0,((_tmp28B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp61->name)),((_tmp28C[0]=& _tmp28B,Cyc_aprintf(((_tmp28D="check_public_type (can't find datatype %s and search its fields)",_tag_dyneither(_tmp28D,sizeof(char),65))),_tag_dyneither(_tmp28C,sizeof(void*),1)))))))));}_LL33: _tmp93=_tmp91;_LL34:(void)_throw(_tmp93);_LL30:;}};}
# 346
if(tud->fields == 0){
const char*_tmp291;void*_tmp290[1];struct Cyc_String_pa_PrintArg_struct _tmp28F;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp28F.tag=0,((_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name)),((_tmp290[0]=& _tmp28F,Cyc_aprintf(((_tmp291="check_public_type (datatype %s has no fields)",_tag_dyneither(_tmp291,sizeof(char),46))),_tag_dyneither(_tmp290,sizeof(void*),1)))))))));}{
# 350
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp9A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
if(Cyc_strptrcmp((*_tmp62->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp9A->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp9A->hd;
break;}}}
# 357
if(tuf1 == 0){
const char*_tmp295;void*_tmp294[1];struct Cyc_String_pa_PrintArg_struct _tmp293;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp293.tag=0,((_tmp293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp62->name)),((_tmp294[0]=& _tmp293,Cyc_aprintf(((_tmp295="check_public_type (can't find datatypefield %s)",_tag_dyneither(_tmp295,sizeof(char),48))),_tag_dyneither(_tmp294,sizeof(void*),1)))))))));}{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 362
if(tud->sc == Cyc_Absyn_Static){
{const char*_tmp296;Cyc_Interface_static_err(obj,name,((_tmp296="datatype",_tag_dyneither(_tmp296,sizeof(char),9))),tud->name);}
return 0;}
# 366
if(tud->sc == Cyc_Absyn_Abstract){
{const char*_tmp297;Cyc_Interface_abstract_err(obj,name,((_tmp297="datatype",_tag_dyneither(_tmp297,sizeof(char),9))),tud->name);}
return 0;}
# 370
if(tuf->sc == Cyc_Absyn_Static){
{const char*_tmp29B;void*_tmp29A[1];struct Cyc_String_pa_PrintArg_struct _tmp299;Cyc_Interface_static_err(obj,name,(struct _dyneither_ptr)(
(_tmp299.tag=0,((_tmp299.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp29A[0]=& _tmp299,Cyc_aprintf(((_tmp29B="field %s of",_tag_dyneither(_tmp29B,sizeof(char),12))),_tag_dyneither(_tmp29A,sizeof(void*),1)))))))),tud->name);}
return 0;}
# 375
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp63) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};};}_LL24:;_LL25:
# 378
 return 1;_LL11:;}struct _tuple20{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 384
static void Cyc_Interface_extract_aggrdecl(struct _tuple20*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){
# 386
struct Cyc_Interface_Ienv*_tmpA4;struct Cyc_Interface_Ienv*_tmpA5;int _tmpA6;struct Cyc_Dict_Dict _tmpA7;struct Cyc_Interface_Seen*_tmpA8;struct _tuple20*_tmpA3=env;_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;_tmpA6=_tmpA3->f3;_tmpA7=_tmpA3->f4;_tmpA8=_tmpA3->f5;{
struct Cyc_Absyn_Aggrdecl*_tmpA9=*dp;
switch(_tmpA9->sc){case Cyc_Absyn_Static: _LL35:
# 390
 if(_tmpA6  && _tmpA9->impl == 0){
const char*_tmp29D;const char*_tmp29C;Cyc_Interface_fields_err(((_tmp29C="static",_tag_dyneither(_tmp29C,sizeof(char),7))),((_tmp29D="struct/union",_tag_dyneither(_tmp29D,sizeof(char),13))),_tmpA9->name);}
break;case Cyc_Absyn_Abstract: _LL36:
# 394
 if(_tmpA9->impl == 0){
if(_tmpA6){
const char*_tmp29F;const char*_tmp29E;Cyc_Interface_fields_err(((_tmp29E="abstract",_tag_dyneither(_tmp29E,sizeof(char),9))),((_tmp29F="struct/union",_tag_dyneither(_tmp29F,sizeof(char),13))),_tmpA9->name);}}else{
# 398
{struct Cyc_Absyn_Aggrdecl*_tmp2A0;_tmpA9=((_tmp2A0=_cycalloc(sizeof(*_tmp2A0)),((_tmp2A0[0]=*_tmpA9,_tmp2A0))));}
_tmpA9->impl=0;}
# 401
if(Cyc_Interface_check_public_aggrdecl(_tmpA7,_tmpA8,_tmpA9))
_tmpA5->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA5->aggrdecls,x,_tmpA9);
break;case Cyc_Absyn_Public: _LL37:
# 405
 if(_tmpA9->impl == 0){
{const char*_tmp2A2;const char*_tmp2A1;Cyc_Interface_fields_err(((_tmp2A1="public",_tag_dyneither(_tmp2A1,sizeof(char),7))),((_tmp2A2="struct/union",_tag_dyneither(_tmp2A2,sizeof(char),13))),_tmpA9->name);}
{struct Cyc_Absyn_Aggrdecl*_tmp2A3;_tmpA9=((_tmp2A3=_cycalloc(sizeof(*_tmp2A3)),((_tmp2A3[0]=*_tmpA9,_tmp2A3))));}
_tmpA9->sc=Cyc_Absyn_Abstract;}
# 410
if(Cyc_Interface_check_public_aggrdecl(_tmpA7,_tmpA8,_tmpA9))
_tmpA5->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA5->aggrdecls,x,_tmpA9);
break;case Cyc_Absyn_ExternC: _LL38:
 goto _LL39;case Cyc_Absyn_Extern: _LL39:
# 415
 if(Cyc_Interface_check_public_aggrdecl(_tmpA7,_tmpA8,_tmpA9))
_tmpA4->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA4->aggrdecls,x,_tmpA9);
break;case Cyc_Absyn_Register: _LL3A:
# 419
{const char*_tmp2A4;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2A4="add_aggrdecl",_tag_dyneither(_tmp2A4,sizeof(char),13))));}
break;}};}
# 423
static void Cyc_Interface_extract_enumdecl(struct _tuple20*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl**dp){
# 425
struct Cyc_Interface_Ienv*_tmpB4;struct Cyc_Interface_Ienv*_tmpB5;int _tmpB6;struct Cyc_Dict_Dict _tmpB7;struct Cyc_Interface_Seen*_tmpB8;struct _tuple20*_tmpB3=env;_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;_tmpB6=_tmpB3->f3;_tmpB7=_tmpB3->f4;_tmpB8=_tmpB3->f5;{
struct Cyc_Absyn_Enumdecl*_tmpB9=*dp;
switch(_tmpB9->sc){case Cyc_Absyn_Static: _LL3C:
# 429
 if(_tmpB6  && _tmpB9->fields == 0){
const char*_tmp2A6;const char*_tmp2A5;Cyc_Interface_fields_err(((_tmp2A5="static",_tag_dyneither(_tmp2A5,sizeof(char),7))),((_tmp2A6="enum",_tag_dyneither(_tmp2A6,sizeof(char),5))),_tmpB9->name);}
break;case Cyc_Absyn_Abstract: _LL3D:
# 433
 if(_tmpB9->fields == 0){
if(_tmpB6){
const char*_tmp2A8;const char*_tmp2A7;Cyc_Interface_fields_err(((_tmp2A7="abstract",_tag_dyneither(_tmp2A7,sizeof(char),9))),((_tmp2A8="enum",_tag_dyneither(_tmp2A8,sizeof(char),5))),_tmpB9->name);}}else{
# 437
{struct Cyc_Absyn_Enumdecl*_tmp2A9;_tmpB9=((_tmp2A9=_cycalloc(sizeof(*_tmp2A9)),((_tmp2A9[0]=*_tmpB9,_tmp2A9))));}
_tmpB9->fields=0;}
# 440
if(Cyc_Interface_check_public_enumdecl(_tmpB7,_tmpB8,_tmpB9))
_tmpB5->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB5->enumdecls,x,_tmpB9);
break;case Cyc_Absyn_Public: _LL3E:
# 444
 if(_tmpB9->fields == 0){
{const char*_tmp2AB;const char*_tmp2AA;Cyc_Interface_fields_err(((_tmp2AA="public",_tag_dyneither(_tmp2AA,sizeof(char),7))),((_tmp2AB="enum",_tag_dyneither(_tmp2AB,sizeof(char),5))),_tmpB9->name);}
{struct Cyc_Absyn_Enumdecl*_tmp2AC;_tmpB9=((_tmp2AC=_cycalloc(sizeof(*_tmp2AC)),((_tmp2AC[0]=*_tmpB9,_tmp2AC))));}
_tmpB9->sc=Cyc_Absyn_Abstract;}
# 449
if(Cyc_Interface_check_public_enumdecl(_tmpB7,_tmpB8,_tmpB9))
_tmpB5->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB5->enumdecls,x,_tmpB9);
break;case Cyc_Absyn_ExternC: _LL3F:
 goto _LL40;case Cyc_Absyn_Extern: _LL40:
# 454
 if(Cyc_Interface_check_public_enumdecl(_tmpB7,_tmpB8,_tmpB9))
_tmpB4->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB4->enumdecls,x,_tmpB9);
break;case Cyc_Absyn_Register: _LL41:
# 458
{const char*_tmp2AD;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2AD="add_enumdecl",_tag_dyneither(_tmp2AD,sizeof(char),13))));}
break;}};}
# 463
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 465
struct Cyc_List_List*_tmpC3=0;
{union Cyc_Absyn_Nmspace _tmpC4=(*f->name).f1;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC7;_LL44: if((_tmpC4.Rel_n).tag != 1)goto _LL46;_tmpC5=(struct Cyc_List_List*)(_tmpC4.Rel_n).val;_LL45:
 _tmpC6=_tmpC5;goto _LL47;_LL46: if((_tmpC4.Abs_n).tag != 2)goto _LL48;_tmpC6=(struct Cyc_List_List*)(_tmpC4.Abs_n).val;_LL47:
 _tmpC3=_tmpC6;goto _LL43;_LL48: if((_tmpC4.C_n).tag != 3)goto _LL4A;_tmpC7=(struct Cyc_List_List*)(_tmpC4.C_n).val;_LL49:
 _tmpC3=_tmpC7;goto _LL43;_LL4A:;_LL4B:
 goto _LL43;_LL43:;}{
# 472
struct Cyc_Dict_Dict*dict;
switch(f->sc){case Cyc_Absyn_Static: _LL4C:
 return;case Cyc_Absyn_Extern: _LL4D:
 dict=& i->imports;break;case Cyc_Absyn_Public: _LL4E:
 dict=& i->exports;break;default: _LL4F: {
const char*_tmp2AE;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2AE="add_xdatatypefielddecl",_tag_dyneither(_tmp2AE,sizeof(char),23))));}}{
# 480
struct Cyc_Interface_Ienv*env;
{struct _handler_cons _tmpC9;_push_handler(& _tmpC9);{int _tmpCB=0;if(setjmp(_tmpC9.handler))_tmpCB=1;if(!_tmpCB){
env=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpC3);;_pop_handler();}else{void*_tmpCA=(void*)_exn_thrown;void*_tmpCD=_tmpCA;void*_tmpCF;_LL52: {struct Cyc_Dict_Absent_exn_struct*_tmpCE=(struct Cyc_Dict_Absent_exn_struct*)_tmpCD;if(_tmpCE->tag != Cyc_Dict_Absent)goto _LL54;}_LL53:
# 485
 env=Cyc_Interface_new_ienv();
*dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*dict,_tmpC3,env);
goto _LL51;_LL54: _tmpCF=_tmpCD;_LL55:(void)_throw(_tmpCF);_LL51:;}};}{
# 489
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2AF;env->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,((_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF->base=d,((_tmp2AF->field=f,_tmp2AF)))))));};};};}
# 493
static void Cyc_Interface_extract_datatypedecl(struct _tuple20*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){
# 495
struct Cyc_Interface_Ienv*_tmpD2;struct Cyc_Interface_Ienv*_tmpD3;int _tmpD4;struct Cyc_Dict_Dict _tmpD5;struct Cyc_Interface_Seen*_tmpD6;struct Cyc_Interface_I*_tmpD7;struct _tuple20*_tmpD1=env;_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_tmpD4=_tmpD1->f3;_tmpD5=_tmpD1->f4;_tmpD6=_tmpD1->f5;_tmpD7=_tmpD1->f6;{
struct Cyc_Absyn_Datatypedecl*_tmpD8=*dp;
# 498
switch(_tmpD8->sc){case Cyc_Absyn_Static: _LL56:
# 500
 if((!_tmpD8->is_extensible  && _tmpD4) && _tmpD8->fields == 0){
const char*_tmp2B1;const char*_tmp2B0;Cyc_Interface_fields_err(((_tmp2B0="static",_tag_dyneither(_tmp2B0,sizeof(char),7))),((_tmp2B1="datatype",_tag_dyneither(_tmp2B1,sizeof(char),9))),_tmpD8->name);}
# 503
break;case Cyc_Absyn_Abstract: _LL57:
# 505
 if(_tmpD8->fields == 0){
if(!_tmpD8->is_extensible  && _tmpD4){
const char*_tmp2B3;const char*_tmp2B2;Cyc_Interface_fields_err(((_tmp2B2="abstract",_tag_dyneither(_tmp2B2,sizeof(char),9))),((_tmp2B3="datatype",_tag_dyneither(_tmp2B3,sizeof(char),9))),_tmpD8->name);}}else{
# 510
{struct Cyc_Absyn_Datatypedecl*_tmp2B4;_tmpD8=((_tmp2B4=_cycalloc(sizeof(*_tmp2B4)),((_tmp2B4[0]=*_tmpD8,_tmp2B4))));}
_tmpD8->fields=0;}
# 513
if(Cyc_Interface_check_public_datatypedecl(_tmpD5,_tmpD6,_tmpD8))
_tmpD3->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpD3->datatypedecls,x,_tmpD8);
break;case Cyc_Absyn_Public: _LL58:
# 517
{struct Cyc_Absyn_Datatypedecl*_tmp2B5;_tmpD8=((_tmp2B5=_cycalloc(sizeof(*_tmp2B5)),((_tmp2B5[0]=*_tmpD8,_tmp2B5))));}
if(!_tmpD8->is_extensible  && _tmpD8->fields == 0){
{const char*_tmp2B7;const char*_tmp2B6;Cyc_Interface_fields_err(((_tmp2B6="public",_tag_dyneither(_tmp2B6,sizeof(char),7))),((_tmp2B7="datatype",_tag_dyneither(_tmp2B7,sizeof(char),9))),_tmpD8->name);}
_tmpD8->sc=Cyc_Absyn_Abstract;}
# 522
if(Cyc_Interface_check_public_datatypedecl(_tmpD5,_tmpD6,_tmpD8)){
if(_tmpD8->is_extensible  && _tmpD8->fields != 0){
struct Cyc_List_List*_tmpE1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD8->fields))->v;
_tmpD8->fields=0;{
struct Cyc_List_List*_tmpE2=_tmpE1;for(0;_tmpE2 != 0;_tmpE2=_tmpE2->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpD7,_tmpD8,(struct Cyc_Absyn_Datatypefield*)_tmpE2->hd);}};}
# 530
_tmpD3->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpD3->datatypedecls,x,_tmpD8);}
# 532
break;case Cyc_Absyn_ExternC: _LL59: {
const char*_tmp2B8;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2B8="extract_datatypedecl",_tag_dyneither(_tmp2B8,sizeof(char),21))));}case Cyc_Absyn_Extern: _LL5A:
# 535
 if(Cyc_Interface_check_public_datatypedecl(_tmpD5,_tmpD6,_tmpD8)){
if(_tmpD8->is_extensible  && _tmpD8->fields != 0){
{struct Cyc_Absyn_Datatypedecl*_tmp2B9;_tmpD8=((_tmp2B9=_cycalloc(sizeof(*_tmp2B9)),((_tmp2B9[0]=*_tmpD8,_tmp2B9))));}{
struct Cyc_List_List*_tmpE5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD8->fields))->v;
_tmpD8->fields=0;{
struct Cyc_List_List*_tmpE6=_tmpE5;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpD7,_tmpD8,(struct Cyc_Absyn_Datatypefield*)_tmpE6->hd);}};};}
# 544
_tmpD2->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpD2->datatypedecls,x,_tmpD8);}
# 546
break;case Cyc_Absyn_Register: _LL5B:
# 548
{const char*_tmp2BA;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2BA="add_datatypedecl",_tag_dyneither(_tmp2BA,sizeof(char),17))));}
break;}};}
# 553
static void Cyc_Interface_extract_typedef(struct _tuple20*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d){
# 555
struct Cyc_Interface_Ienv*_tmpE9;struct Cyc_Dict_Dict _tmpEA;struct Cyc_Interface_Seen*_tmpEB;struct _tuple20*_tmpE8=env;_tmpE9=_tmpE8->f2;_tmpEA=_tmpE8->f4;_tmpEB=_tmpE8->f5;
if(Cyc_Interface_check_public_typedefdecl(_tmpEA,_tmpEB,d))
_tmpE9->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpE9->typedefdecls,x,d);}struct _tuple21{void*f1;int f2;};
# 560
static void Cyc_Interface_extract_ordinarie(struct _tuple20*env,struct _dyneither_ptr*x,struct _tuple21*v){
# 562
struct Cyc_Interface_Ienv*_tmpED;struct Cyc_Interface_Ienv*_tmpEE;int _tmpEF;struct Cyc_Dict_Dict _tmpF0;struct Cyc_Interface_Seen*_tmpF1;struct _tuple20*_tmpEC=env;_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;_tmpEF=_tmpEC->f3;_tmpF0=_tmpEC->f4;_tmpF1=_tmpEC->f5;{
void*_tmpF2=(*v).f1;
# 565
void*_tmpF3=_tmpF2;void*_tmpF5;_LL5E: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpF3;if(_tmpF4->tag != 0)goto _LL60;else{_tmpF5=(void*)_tmpF4->f1;}}_LL5F:
# 567
{void*_tmpF6=_tmpF5;struct Cyc_Absyn_Fndecl*_tmpF8;struct Cyc_Absyn_Vardecl*_tmpFA;_LL63: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpF7=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF6;if(_tmpF7->tag != 2)goto _LL65;else{_tmpF8=_tmpF7->f1;}}_LL64: {
# 569
struct Cyc_Absyn_Vardecl*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmpFB=(_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((_tmp2BB->sc=_tmpF8->sc,((_tmp2BB->name=_tmpF8->name,((_tmp2BB->tq=
# 572
Cyc_Absyn_empty_tqual(0),((_tmp2BB->type=(void*)_check_null(_tmpF8->cached_typ),((_tmp2BB->initializer=0,((_tmp2BB->rgn=0,((_tmp2BB->attributes=0,((_tmp2BB->escapes=0,_tmp2BB)))))))))))))))));
# 579
_tmpEF=0;
_tmpFA=_tmpFB;goto _LL66;}_LL65: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF6;if(_tmpF9->tag != 1)goto _LL67;else{_tmpFA=_tmpF9->f1;}}_LL66:
# 582
 if(_tmpFA->initializer != 0){
{struct Cyc_Absyn_Vardecl*_tmp2BC;_tmpFA=((_tmp2BC=_cycalloc(sizeof(*_tmp2BC)),((_tmp2BC[0]=*_tmpFA,_tmp2BC))));}
_tmpFA->initializer=0;}
# 586
switch(_tmpFA->sc){case Cyc_Absyn_Static: _LL69:
# 588
 if(_tmpEF  && Cyc_Tcutil_is_function_type(_tmpFA->type)){
const char*_tmp2BD;Cyc_Interface_body_err(((_tmp2BD="static",_tag_dyneither(_tmp2BD,sizeof(char),7))),_tmpFA->name);}
# 591
break;case Cyc_Absyn_Register: _LL6A:
 goto _LL6B;case Cyc_Absyn_Abstract: _LL6B: {
const char*_tmp2BE;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2BE="extract_ordinarie",_tag_dyneither(_tmp2BE,sizeof(char),18))));}case Cyc_Absyn_Public: _LL6C:
# 595
 if(_tmpEF  && Cyc_Tcutil_is_function_type(_tmpFA->type)){
const char*_tmp2BF;Cyc_Interface_body_err(((_tmp2BF="public",_tag_dyneither(_tmp2BF,sizeof(char),7))),_tmpFA->name);}
# 598
if(Cyc_Interface_check_public_vardecl(_tmpF0,_tmpF1,_tmpFA))
_tmpEE->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpEE->vardecls,x,_tmpFA);
break;case Cyc_Absyn_ExternC: _LL6D:
 goto _LL6E;case Cyc_Absyn_Extern: _LL6E:
# 603
 if(Cyc_Interface_check_public_vardecl(_tmpF0,_tmpF1,_tmpFA))
_tmpED->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpED->vardecls,x,_tmpFA);
break;}
# 607
goto _LL62;_LL67:;_LL68:
 goto _LL62;_LL62:;}
# 610
goto _LL5D;_LL60:;_LL61:
 goto _LL5D;_LL5D:;};}struct _tuple22{struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;};
# 615
static void Cyc_Interface_extract_f(struct _tuple22*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){
# 617
struct Cyc_Interface_I*_tmp102;int _tmp103;struct Cyc_Dict_Dict _tmp104;struct Cyc_Interface_Seen*_tmp105;struct _tuple22*_tmp101=env_f;_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;_tmp104=_tmp101->f3;_tmp105=_tmp101->f4;{
struct _tuple20 _tmp2C0;struct _tuple20 _tmp106=(_tmp2C0.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp102->imports,ns),((_tmp2C0.f2=
((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp102->exports,ns),((_tmp2C0.f3=_tmp103,((_tmp2C0.f4=_tmp104,((_tmp2C0.f5=_tmp105,((_tmp2C0.f6=_tmp102,_tmp2C0)))))))))));
# 623
((void(*)(void(*f)(struct _tuple20*,struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp106,ge->aggrdecls);
((void(*)(void(*f)(struct _tuple20*,struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl**),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmp106,ge->datatypedecls);
((void(*)(void(*f)(struct _tuple20*,struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmp106,ge->enumdecls);
((void(*)(void(*f)(struct _tuple20*,struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmp106,ge->typedefs);
((void(*)(void(*f)(struct _tuple20*,struct _dyneither_ptr*,struct _tuple21*),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp106,ge->ordinaries);};}
# 630
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Dict_Dict ae,int check_complete_defs){
# 632
struct _tuple22 _tmp2C1;struct _tuple22 env=
(_tmp2C1.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(ae),((_tmp2C1.f2=check_complete_defs,((_tmp2C1.f3=ae,((_tmp2C1.f4=Cyc_Interface_new_seen(),_tmp2C1)))))));
((void(*)(void(*f)(struct _tuple22*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),struct _tuple22*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,& env,ae);
return env.f1;}
# 638
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae){
return Cyc_Interface_gen_extract(ae,1);}
# 644
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
const char*_tmp2C5;void*_tmp2C4[1];struct Cyc_String_pa_PrintArg_struct _tmp2C3;(_tmp2C3.tag=0,((_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2),((_tmp2C4[0]=& _tmp2C3,Cyc_Tcdecl_merr(0,msg1,((_tmp2C5="%s",_tag_dyneither(_tmp2C5,sizeof(char),3))),_tag_dyneither(_tmp2C4,sizeof(void*),1)))))));}struct _tuple23{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 648
static void Cyc_Interface_incl_dict_f(struct _tuple23*env,struct _dyneither_ptr*x,void*y1){
# 655
int*_tmp10D;struct Cyc_Dict_Dict _tmp10E;int(*_tmp10F)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp110;struct _dyneither_ptr*_tmp111;struct _tuple23*_tmp10C=env;_tmp10D=(int*)& _tmp10C->f1;_tmp10E=_tmp10C->f2;_tmp10F=_tmp10C->f3;_tmp110=_tmp10C->f4;_tmp111=_tmp10C->f5;{
struct _handler_cons _tmp112;_push_handler(& _tmp112);{int _tmp114=0;if(setjmp(_tmp112.handler))_tmp114=1;if(!_tmp114){
{void*_tmp115=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp10E,x);
if(!_tmp10F(y1,_tmp115,_tmp111))*_tmp10D=0;}
# 657
;_pop_handler();}else{void*_tmp113=(void*)_exn_thrown;void*_tmp117=_tmp113;void*_tmp119;_LL71: {struct Cyc_Dict_Absent_exn_struct*_tmp118=(struct Cyc_Dict_Absent_exn_struct*)_tmp117;if(_tmp118->tag != Cyc_Dict_Absent)goto _LL73;}_LL72:
# 661
{const char*_tmp2CA;void*_tmp2C9[2];struct Cyc_String_pa_PrintArg_struct _tmp2C8;struct Cyc_String_pa_PrintArg_struct _tmp2C7;Cyc_Interface_check_err(_tmp111,(struct _dyneither_ptr)((_tmp2C7.tag=0,((_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp2C8.tag=0,((_tmp2C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp110),((_tmp2C9[0]=& _tmp2C8,((_tmp2C9[1]=& _tmp2C7,Cyc_aprintf(((_tmp2CA="%s %s is missing",_tag_dyneither(_tmp2CA,sizeof(char),17))),_tag_dyneither(_tmp2C9,sizeof(void*),2)))))))))))))));}
*_tmp10D=0;
goto _LL70;_LL73: _tmp119=_tmp117;_LL74:(void)_throw(_tmp119);_LL70:;}};};}
# 667
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 672
struct _tuple23 _tmp2CB;struct _tuple23 _tmp11E=(_tmp2CB.f1=1,((_tmp2CB.f2=dic2,((_tmp2CB.f3=incl_f,((_tmp2CB.f4=t,((_tmp2CB.f5=msg,_tmp2CB)))))))));
((void(*)(void(*f)(struct _tuple23*,struct _dyneither_ptr*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp11E,dic1);
return _tmp11E.f1;}
# 690 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp120=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp120 == 0)return 0;if(d0 != _tmp120){{const char*_tmp2CF;void*_tmp2CE[1];struct Cyc_String_pa_PrintArg_struct _tmp2CD;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2CD.tag=0,((_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2CE[0]=& _tmp2CD,Cyc_aprintf(((_tmp2CF="declaration of type %s discloses too much information",_tag_dyneither(_tmp2CF,sizeof(char),54))),_tag_dyneither(_tmp2CE,sizeof(void*),1)))))))));}return 0;}return 1;}
# 692
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp124=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp124 == 0)return 0;if(d0 != _tmp124){{const char*_tmp2D3;void*_tmp2D2[1];struct Cyc_String_pa_PrintArg_struct _tmp2D1;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2D1.tag=0,((_tmp2D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2D2[0]=& _tmp2D1,Cyc_aprintf(((_tmp2D3="declaration of datatype %s discloses too much information",_tag_dyneither(_tmp2D3,sizeof(char),58))),_tag_dyneither(_tmp2D2,sizeof(void*),1)))))))));}return 0;}return 1;}
# 694
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp128=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp128 == 0)return 0;if(d0 != _tmp128){{const char*_tmp2D7;void*_tmp2D6[1];struct Cyc_String_pa_PrintArg_struct _tmp2D5;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2D5.tag=0,((_tmp2D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2D6[0]=& _tmp2D5,Cyc_aprintf(((_tmp2D7="declaration of enum %s discloses too much information",_tag_dyneither(_tmp2D7,sizeof(char),54))),_tag_dyneither(_tmp2D6,sizeof(void*),1)))))))));}return 0;}return 1;}
# 696
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp12C=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12C == 0)return 0;if(d0 != _tmp12C){{const char*_tmp2DB;void*_tmp2DA[1];struct Cyc_String_pa_PrintArg_struct _tmp2D9;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2D9.tag=0,((_tmp2D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2DA[0]=& _tmp2D9,Cyc_aprintf(((_tmp2DB="declaration of variable %s discloses too much information",_tag_dyneither(_tmp2DB,sizeof(char),58))),_tag_dyneither(_tmp2DA,sizeof(void*),1)))))))));}return 0;}return 1;}
# 698
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp130=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp130 == 0)return 0;if(d0 != _tmp130){{const char*_tmp2DF;void*_tmp2DE[1];struct Cyc_String_pa_PrintArg_struct _tmp2DD;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2DD.tag=0,((_tmp2DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2DE[0]=& _tmp2DD,Cyc_aprintf(((_tmp2DF="declaration of typedef %s discloses too much information",_tag_dyneither(_tmp2DF,sizeof(char),57))),_tag_dyneither(_tmp2DE,sizeof(void*),1)))))))));}return 0;}return 1;}
# 700
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp134=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp134 == 0)return 0;if(d0 != _tmp134){{const char*_tmp2E3;void*_tmp2E2[1];struct Cyc_String_pa_PrintArg_struct _tmp2E1;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2E1.tag=0,((_tmp2E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name)),((_tmp2E2[0]=& _tmp2E1,Cyc_aprintf(((_tmp2E3="declaration of xdatatypefield %s discloses too much information",_tag_dyneither(_tmp2E3,sizeof(char),64))),_tag_dyneither(_tmp2E2,sizeof(void*),1)))))))));}return 0;}return 1;}
# 705
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 714
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
const char*_tmp2E4;int _tmp138=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,((_tmp2E4="type",_tag_dyneither(_tmp2E4,sizeof(char),5))),msg);
const char*_tmp2E5;int _tmp139=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,Cyc_Interface_incl_datatypedecl,((_tmp2E5="datatype",_tag_dyneither(_tmp2E5,sizeof(char),9))),msg);
const char*_tmp2E6;int _tmp13A=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,((_tmp2E6="enum",_tag_dyneither(_tmp2E6,sizeof(char),5))),msg);
# 719
const char*_tmp2E7;int _tmp13B=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,(
(_tmp2E7="typedef",_tag_dyneither(_tmp2E7,sizeof(char),8))),msg)!= 0;
const char*_tmp2E8;int _tmp13C=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,((_tmp2E8="variable",_tag_dyneither(_tmp2E8,sizeof(char),9))),msg);
const char*_tmp2E9;int _tmp13D=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,((_tmp2E9="xdatatypefield",_tag_dyneither(_tmp2E9,sizeof(char),15))),msg);
return((((_tmp138  && _tmp139) && _tmp13A) && _tmp13B) && _tmp13C) && _tmp13D;}struct _tuple24{int f1;struct Cyc_Dict_Dict f2;struct _dyneither_ptr*f3;};
# 727
static void Cyc_Interface_incl_ns_f(struct _tuple24*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){
# 732
int*_tmp145;struct Cyc_Dict_Dict _tmp146;struct _dyneither_ptr*_tmp147;struct _tuple24*_tmp144=env;_tmp145=(int*)& _tmp144->f1;_tmp146=_tmp144->f2;_tmp147=_tmp144->f3;{
# 734
struct Cyc_Interface_Ienv*ie2;
# 737
{struct _handler_cons _tmp148;_push_handler(& _tmp148);{int _tmp14A=0;if(setjmp(_tmp148.handler))_tmp14A=1;if(!_tmp14A){ie2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp146,ns);;_pop_handler();}else{void*_tmp149=(void*)_exn_thrown;void*_tmp14C=_tmp149;void*_tmp14E;_LL76: {struct Cyc_Dict_Absent_exn_struct*_tmp14D=(struct Cyc_Dict_Absent_exn_struct*)_tmp14C;if(_tmp14D->tag != Cyc_Dict_Absent)goto _LL78;}_LL77:
 ie2=Cyc_Interface_lazy_new_ienv();goto _LL75;_LL78: _tmp14E=_tmp14C;_LL79:(void)_throw(_tmp14E);_LL75:;}};}
# 740
if(!Cyc_Interface_incl_ienv(ie1,ie2,_tmp147))*_tmp145=0;};}
# 743
static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){
# 746
struct _tuple24 _tmp2EA;struct _tuple24 _tmp14F=(_tmp2EA.f1=1,((_tmp2EA.f2=dic2,((_tmp2EA.f3=msg,_tmp2EA)))));
((void(*)(void(*f)(struct _tuple24*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,& _tmp14F,dic1);
return _tmp14F.f1;}
# 751
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp151;_push_handler(& _tmp151);{int _tmp153=0;if(setjmp(_tmp151.handler))_tmp153=1;if(!_tmp153){
{int _tmp154=1;
struct _dyneither_ptr*msg=0;
# 756
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp2F4;struct Cyc_String_pa_PrintArg_struct _tmp2F3;void*_tmp2F2[2];const char*_tmp2F1;struct _dyneither_ptr*_tmp2F0;msg=((_tmp2F0=_cycalloc(sizeof(*_tmp2F0)),((_tmp2F0[0]=(struct _dyneither_ptr)((_tmp2F4.tag=0,((_tmp2F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp2F3.tag=0,((_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2F2[0]=& _tmp2F3,((_tmp2F2[1]=& _tmp2F4,Cyc_aprintf(((_tmp2F1="checking inclusion of %s exports into %s exports,",_tag_dyneither(_tmp2F1,sizeof(char),50))),_tag_dyneither(_tmp2F2,sizeof(void*),2)))))))))))))),_tmp2F0))));}
if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))_tmp154=0;
# 760
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp2FE;struct Cyc_String_pa_PrintArg_struct _tmp2FD;void*_tmp2FC[2];const char*_tmp2FB;struct _dyneither_ptr*_tmp2FA;msg=((_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((_tmp2FA[0]=(struct _dyneither_ptr)((_tmp2FE.tag=0,((_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2FD.tag=0,((_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp2FC[0]=& _tmp2FD,((_tmp2FC[1]=& _tmp2FE,Cyc_aprintf(((_tmp2FB="checking inclusion of %s imports into %s imports,",_tag_dyneither(_tmp2FB,sizeof(char),50))),_tag_dyneither(_tmp2FC,sizeof(void*),2)))))))))))))),_tmp2FA))));}
if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))_tmp154=0;{
# 764
int _tmp15F=_tmp154;_npop_handler(0);return _tmp15F;};}
# 753
;_pop_handler();}else{void*_tmp152=(void*)_exn_thrown;void*_tmp161=_tmp152;void*_tmp163;_LL7B: {struct Cyc_Tcdecl_Incompatible_exn_struct*_tmp162=(struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp161;if(_tmp162->tag != Cyc_Tcdecl_Incompatible)goto _LL7D;}_LL7C:
# 766
 return 0;_LL7D: _tmp163=_tmp161;_LL7E:(void)_throw(_tmp163);_LL7A:;}};}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 774
void Cyc_Interface_compat_merge_dict_f(struct _tuple25*env,struct _dyneither_ptr*x,void*y2){
# 781
int*_tmp165;struct Cyc_Dict_Dict*_tmp166;struct Cyc_Dict_Dict _tmp167;struct Cyc_Dict_Dict _tmp168;void*(*_tmp169)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr _tmp16A;struct _dyneither_ptr*_tmp16B;struct _tuple25*_tmp164=env;_tmp165=(int*)& _tmp164->f1;_tmp166=(struct Cyc_Dict_Dict*)& _tmp164->f2;_tmp167=_tmp164->f3;_tmp168=_tmp164->f4;_tmp169=_tmp164->f5;_tmp16A=_tmp164->f6;_tmp16B=_tmp164->f7;{
void*y;
{struct _handler_cons _tmp16C;_push_handler(& _tmp16C);{int _tmp16E=0;if(setjmp(_tmp16C.handler))_tmp16E=1;if(!_tmp16E){
{void*_tmp16F=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp167,x);
# 787
void*_tmp170=_tmp169(_tmp16F,y2,0,_tmp16B);
if(!((unsigned int)_tmp170)){
*_tmp165=0;
_npop_handler(0);return;}
# 792
y=_tmp170;}
# 784
;_pop_handler();}else{void*_tmp16D=(void*)_exn_thrown;void*_tmp172=_tmp16D;void*_tmp174;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmp173=(struct Cyc_Dict_Absent_exn_struct*)_tmp172;if(_tmp173->tag != Cyc_Dict_Absent)goto _LL82;}_LL81:
# 795
 y=y2;
goto _LL7F;_LL82: _tmp174=_tmp172;_LL83:(void)_throw(_tmp174);_LL7F:;}};}{
# 799
struct _handler_cons _tmp175;_push_handler(& _tmp175);{int _tmp177=0;if(setjmp(_tmp175.handler))_tmp177=1;if(!_tmp177){
{void*_tmp178=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp168,x);
# 804
void*_tmp179=_tmp169(_tmp178,y,0,_tmp16B);
if(_tmp179 != _tmp178){
if((unsigned int)_tmp179){
const char*_tmp303;void*_tmp302[2];struct Cyc_String_pa_PrintArg_struct _tmp301;struct Cyc_String_pa_PrintArg_struct _tmp300;Cyc_Interface_check_err(_tmp16B,(struct _dyneither_ptr)((_tmp300.tag=0,((_tmp300.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp301.tag=0,((_tmp301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A),((_tmp302[0]=& _tmp301,((_tmp302[1]=& _tmp300,Cyc_aprintf(((_tmp303="abstract %s %s is being imported as non-abstract",_tag_dyneither(_tmp303,sizeof(char),49))),_tag_dyneither(_tmp302,sizeof(void*),2)))))))))))))));}
# 809
*_tmp165=0;}}
# 800
;_pop_handler();}else{void*_tmp176=(void*)_exn_thrown;void*_tmp17F=_tmp176;void*_tmp181;_LL85: {struct Cyc_Dict_Absent_exn_struct*_tmp180=(struct Cyc_Dict_Absent_exn_struct*)_tmp17F;if(_tmp180->tag != Cyc_Dict_Absent)goto _LL87;}_LL86:
# 813
 if(*_tmp165)
*_tmp166=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp166,x,y);
goto _LL84;_LL87: _tmp181=_tmp17F;_LL88:(void)_throw(_tmp181);_LL84:;}};};};}
# 819
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 827
struct _tuple25 _tmp304;struct _tuple25 _tmp182=(_tmp304.f1=1,((_tmp304.f2=dic1,((_tmp304.f3=dic1,((_tmp304.f4=excl,((_tmp304.f5=merge_f,((_tmp304.f6=t,((_tmp304.f7=msg,_tmp304)))))))))))));
# 829
((void(*)(void(*f)(struct _tuple25*,struct _dyneither_ptr*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp182,dic2);
# 831
if(_tmp182.f1){
struct Cyc_Dict_Dict*_tmp305;return(_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=_tmp182.f2,_tmp305)));}
return 0;}
# 841
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
const char*_tmp306;struct Cyc_Dict_Dict*_tmp185=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,((_tmp306="type",_tag_dyneither(_tmp306,sizeof(char),5))),msg);
const char*_tmp307;struct Cyc_Dict_Dict*_tmp186=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,((_tmp307="datatype",_tag_dyneither(_tmp307,sizeof(char),9))),msg);
const char*_tmp308;struct Cyc_Dict_Dict*_tmp187=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,((_tmp308="enum",_tag_dyneither(_tmp308,sizeof(char),5))),msg);
const char*_tmp309;struct Cyc_Dict_Dict*_tmp188=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,((_tmp309="typedef",_tag_dyneither(_tmp309,sizeof(char),8))),msg);
const char*_tmp30A;struct Cyc_Dict_Dict*_tmp189=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,((_tmp30A="variable",_tag_dyneither(_tmp30A,sizeof(char),9))),msg);
const char*_tmp30B;struct Cyc_Dict_Dict*_tmp18A=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,((_tmp30B="xdatatypefield",_tag_dyneither(_tmp30B,sizeof(char),15))),msg);
if(((((!((unsigned int)_tmp185) || !((unsigned int)_tmp186)) || !((unsigned int)_tmp187)) || !((unsigned int)_tmp188)) || !((unsigned int)_tmp189)) || !((unsigned int)_tmp18A))
return 0;{
struct Cyc_Interface_Ienv*_tmp30C;return(_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C->aggrdecls=*_tmp185,((_tmp30C->datatypedecls=*_tmp186,((_tmp30C->enumdecls=*_tmp187,((_tmp30C->typedefdecls=*_tmp188,((_tmp30C->vardecls=*_tmp189,((_tmp30C->xdatatypefielddecls=*_tmp18A,_tmp30C)))))))))))));};}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;struct _dyneither_ptr*f5;};
# 853
void Cyc_Interface_compat_merge_ns_f(struct _tuple26*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){
# 859
int*_tmp193;struct Cyc_Dict_Dict*_tmp194;struct Cyc_Dict_Dict _tmp195;struct Cyc_Dict_Dict _tmp196;struct _dyneither_ptr*_tmp197;struct _tuple26*_tmp192=env;_tmp193=(int*)& _tmp192->f1;_tmp194=(struct Cyc_Dict_Dict*)& _tmp192->f2;_tmp195=_tmp192->f3;_tmp196=_tmp192->f4;_tmp197=_tmp192->f5;{
# 861
struct Cyc_Interface_Ienv*ie1;struct Cyc_Interface_Ienv*iexcl;
# 864
{struct _handler_cons _tmp198;_push_handler(& _tmp198);{int _tmp19A=0;if(setjmp(_tmp198.handler))_tmp19A=1;if(!_tmp19A){iexcl=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp196,ns);;_pop_handler();}else{void*_tmp199=(void*)_exn_thrown;void*_tmp19C=_tmp199;void*_tmp19E;_LL8A: {struct Cyc_Dict_Absent_exn_struct*_tmp19D=(struct Cyc_Dict_Absent_exn_struct*)_tmp19C;if(_tmp19D->tag != Cyc_Dict_Absent)goto _LL8C;}_LL8B:
 iexcl=Cyc_Interface_lazy_new_ienv();goto _LL89;_LL8C: _tmp19E=_tmp19C;_LL8D:(void)_throw(_tmp19E);_LL89:;}};}
# 867
{struct _handler_cons _tmp19F;_push_handler(& _tmp19F);{int _tmp1A1=0;if(setjmp(_tmp19F.handler))_tmp1A1=1;if(!_tmp1A1){
ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp195,ns);{
struct Cyc_Interface_Ienv*_tmp1A2=Cyc_Interface_compat_merge_ienv(Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp197);
if(_tmp1A2 == 0){
*_tmp193=0;
_npop_handler(0);return;}
# 874
ie1=_tmp1A2;};
# 868
;_pop_handler();}else{void*_tmp1A0=(void*)_exn_thrown;void*_tmp1A4=_tmp1A0;void*_tmp1A6;_LL8F: {struct Cyc_Dict_Absent_exn_struct*_tmp1A5=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A4;if(_tmp1A5->tag != Cyc_Dict_Absent)goto _LL91;}_LL90:
# 875
 ie1=Cyc_Interface_lazy_new_ienv();goto _LL8E;_LL91: _tmp1A6=_tmp1A4;_LL92:(void)_throw(_tmp1A6);_LL8E:;}};}{
# 877
struct Cyc_Interface_Ienv*_tmp1A7=Cyc_Interface_compat_merge_ienv(ie1,ie2,iexcl,_tmp197);
# 879
if(_tmp1A7 == 0)
*_tmp193=0;else{
# 882
if(*_tmp193)
*_tmp194=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp194,ns,_tmp1A7);}};};}
# 886
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg){
# 891
struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple26 _tmp30D;struct _tuple26 _tmp1A8=(_tmp30D.f1=1,((_tmp30D.f2=d,((_tmp30D.f3=dic1,((_tmp30D.f4=excl,((_tmp30D.f5=msg,_tmp30D)))))))));
# 894
((void(*)(void(*f)(struct _tuple26*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,& _tmp1A8,dic2);
# 896
if(_tmp1A8.f1){
struct Cyc_Dict_Dict*_tmp30E;return(_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E[0]=_tmp1A8.f2,_tmp30E)));}
return 0;}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 902
void Cyc_Interface_disj_merge_dict_f(struct _tuple27*env,struct _dyneither_ptr*x,void*y){
# 905
int*_tmp1AC;struct Cyc_Dict_Dict*_tmp1AD;struct Cyc_Dict_Dict _tmp1AE;struct _dyneither_ptr _tmp1AF;struct _dyneither_ptr*_tmp1B0;struct _tuple27*_tmp1AB=env;_tmp1AC=(int*)& _tmp1AB->f1;_tmp1AD=(struct Cyc_Dict_Dict*)& _tmp1AB->f2;_tmp1AE=_tmp1AB->f3;_tmp1AF=_tmp1AB->f4;_tmp1B0=_tmp1AB->f5;
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1AE,x)){
{const char*_tmp313;void*_tmp312[2];struct Cyc_String_pa_PrintArg_struct _tmp311;struct Cyc_String_pa_PrintArg_struct _tmp310;Cyc_Interface_check_err(_tmp1B0,(struct _dyneither_ptr)((_tmp310.tag=0,((_tmp310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp311.tag=0,((_tmp311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1AF),((_tmp312[0]=& _tmp311,((_tmp312[1]=& _tmp310,Cyc_aprintf(((_tmp313="%s %s is exported more than once",_tag_dyneither(_tmp313,sizeof(char),33))),_tag_dyneither(_tmp312,sizeof(void*),2)))))))))))))));}
*_tmp1AC=0;}else{
# 910
if(*_tmp1AC)*_tmp1AD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp1AD,x,y);}}
# 914
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 918
struct _tuple27 _tmp314;struct _tuple27 _tmp1B5=(_tmp314.f1=1,((_tmp314.f2=dic1,((_tmp314.f3=dic1,((_tmp314.f4=t,((_tmp314.f5=msg,_tmp314)))))))));
((void(*)(void(*f)(struct _tuple27*,struct _dyneither_ptr*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp1B5,dic2);
if(_tmp1B5.f1){
struct Cyc_Dict_Dict*_tmp315;return(_tmp315=_cycalloc(sizeof(*_tmp315)),((_tmp315[0]=_tmp1B5.f2,_tmp315)));}
return 0;}
# 925
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
const char*_tmp316;struct Cyc_Dict_Dict*_tmp1B8=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,(
(_tmp316="type",_tag_dyneither(_tmp316,sizeof(char),5))),msg);
const char*_tmp317;struct Cyc_Dict_Dict*_tmp1B9=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,((_tmp317="datatype",_tag_dyneither(_tmp317,sizeof(char),9))),msg);
const char*_tmp318;struct Cyc_Dict_Dict*_tmp1BA=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,ie2->enumdecls,((_tmp318="enum",_tag_dyneither(_tmp318,sizeof(char),5))),msg);
# 932
const char*_tmp319;struct Cyc_Dict_Dict*_tmp1BB=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,(
(_tmp319="typedef",_tag_dyneither(_tmp319,sizeof(char),8))),msg);
const char*_tmp31A;struct Cyc_Dict_Dict*_tmp1BC=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,((_tmp31A="variable",_tag_dyneither(_tmp31A,sizeof(char),9))),msg);
const char*_tmp31B;struct Cyc_Dict_Dict*_tmp1BD=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,(
(_tmp31B="xdatatypefield",_tag_dyneither(_tmp31B,sizeof(char),15))),msg);
# 939
if(((((!((unsigned int)_tmp1B8) || !((unsigned int)_tmp1B9)) || !((unsigned int)_tmp1BA)) || !((unsigned int)_tmp1BB)) || !((unsigned int)_tmp1BC)) || !((unsigned int)_tmp1BD))
return 0;{
struct Cyc_Interface_Ienv*_tmp31C;return(_tmp31C=_cycalloc(sizeof(*_tmp31C)),((_tmp31C->aggrdecls=*_tmp1B8,((_tmp31C->datatypedecls=*_tmp1B9,((_tmp31C->enumdecls=*_tmp1BA,((_tmp31C->typedefdecls=*_tmp1BB,((_tmp31C->vardecls=*_tmp1BC,((_tmp31C->xdatatypefielddecls=*_tmp1BD,_tmp31C)))))))))))));};}struct _tuple28{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*f4;};
# 944
void Cyc_Interface_disj_merge_ns_f(struct _tuple28*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){
# 947
int*_tmp1C6;struct Cyc_Dict_Dict*_tmp1C7;struct Cyc_Dict_Dict _tmp1C8;struct _dyneither_ptr*_tmp1C9;struct _tuple28*_tmp1C5=env;_tmp1C6=(int*)& _tmp1C5->f1;_tmp1C7=(struct Cyc_Dict_Dict*)& _tmp1C5->f2;_tmp1C8=_tmp1C5->f3;_tmp1C9=_tmp1C5->f4;{
# 949
struct Cyc_Interface_Ienv*ie1;
# 951
{struct _handler_cons _tmp1CA;_push_handler(& _tmp1CA);{int _tmp1CC=0;if(setjmp(_tmp1CA.handler))_tmp1CC=1;if(!_tmp1CC){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1C8,ns);;_pop_handler();}else{void*_tmp1CB=(void*)_exn_thrown;void*_tmp1CE=_tmp1CB;void*_tmp1D0;_LL94: {struct Cyc_Dict_Absent_exn_struct*_tmp1CF=(struct Cyc_Dict_Absent_exn_struct*)_tmp1CE;if(_tmp1CF->tag != Cyc_Dict_Absent)goto _LL96;}_LL95:
 ie1=Cyc_Interface_lazy_new_ienv();goto _LL93;_LL96: _tmp1D0=_tmp1CE;_LL97:(void)_throw(_tmp1D0);_LL93:;}};}{
# 954
struct Cyc_Interface_Ienv*_tmp1D1=Cyc_Interface_disj_merge_ienv(ie1,ie2,_tmp1C9);
# 956
if(_tmp1D1 == 0)
*_tmp1C6=0;else{
# 959
if(*_tmp1C6)
*_tmp1C7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp1C7,ns,_tmp1D1);}};};}
# 963
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){
# 967
struct _tuple28 _tmp31D;struct _tuple28 _tmp1D2=(_tmp31D.f1=1,((_tmp31D.f2=dic1,((_tmp31D.f3=dic1,((_tmp31D.f4=msg,_tmp31D)))))));
((void(*)(void(*f)(struct _tuple28*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple28*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,& _tmp1D2,dic2);
if(_tmp1D2.f1){
struct Cyc_Dict_Dict*_tmp31E;return(_tmp31E=_cycalloc(sizeof(*_tmp31E)),((_tmp31E[0]=_tmp1D2.f2,_tmp31E)));}
return 0;}
# 975
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp1D5;_push_handler(& _tmp1D5);{int _tmp1D7=0;if(setjmp(_tmp1D5.handler))_tmp1D7=1;if(!_tmp1D7){
{struct _dyneither_ptr*msg=0;
# 979
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp328;struct Cyc_String_pa_PrintArg_struct _tmp327;void*_tmp326[2];const char*_tmp325;struct _dyneither_ptr*_tmp324;msg=((_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324[0]=(struct _dyneither_ptr)((_tmp328.tag=0,((_tmp328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp327.tag=0,((_tmp327.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp326[0]=& _tmp327,((_tmp326[1]=& _tmp328,Cyc_aprintf(((_tmp325="merging exports of %s and %s,",_tag_dyneither(_tmp325,sizeof(char),30))),_tag_dyneither(_tmp326,sizeof(void*),2)))))))))))))),_tmp324))));}{
# 983
struct Cyc_Dict_Dict*_tmp1DD=Cyc_Interface_disj_merge_ns(i1->exports,i2->exports,msg);
if(_tmp1DD == 0){struct Cyc_Interface_I*_tmp1DE=0;_npop_handler(0);return _tmp1DE;}
# 986
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp332;struct Cyc_String_pa_PrintArg_struct _tmp331;void*_tmp330[2];const char*_tmp32F;struct _dyneither_ptr*_tmp32E;msg=((_tmp32E=_cycalloc(sizeof(*_tmp32E)),((_tmp32E[0]=(struct _dyneither_ptr)((_tmp332.tag=0,((_tmp332.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp331.tag=0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp330[0]=& _tmp331,((_tmp330[1]=& _tmp332,Cyc_aprintf(((_tmp32F="merging imports of %s and %s,",_tag_dyneither(_tmp32F,sizeof(char),30))),_tag_dyneither(_tmp330,sizeof(void*),2)))))))))))))),_tmp32E))));}{
# 990
struct Cyc_Dict_Dict*_tmp1E4=Cyc_Interface_compat_merge_ns(i1->imports,i2->imports,*_tmp1DD,msg);
if(_tmp1E4 == 0){struct Cyc_Interface_I*_tmp1E5=0;_npop_handler(0);return _tmp1E5;}{
# 993
struct Cyc_Interface_I*_tmp333;struct Cyc_Interface_I*_tmp1E7=(_tmp333=_cycalloc(sizeof(*_tmp333)),((_tmp333->imports=*_tmp1E4,((_tmp333->exports=*_tmp1DD,_tmp333)))));_npop_handler(0);return _tmp1E7;};};};}
# 977
;_pop_handler();}else{void*_tmp1D6=(void*)_exn_thrown;void*_tmp1E9=_tmp1D6;void*_tmp1EB;_LL99: {struct Cyc_Tcdecl_Incompatible_exn_struct*_tmp1EA=(struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp1E9;if(_tmp1EA->tag != Cyc_Tcdecl_Incompatible)goto _LL9B;}_LL9A:
# 994
 return 0;_LL9B: _tmp1EB=_tmp1E9;_LL9C:(void)_throw(_tmp1EB);_LL98:;}};}
# 997
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 1000
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 1005
for(0;li != 0;li=li->tl){
struct _tuple10*_tmp334;struct Cyc_Interface_I*_tmp1EC=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)li->hd,
curr_info != 0  && linfo != 0?(_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334->f1=*curr_info,((_tmp334->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp334))))): 0);
if(_tmp1EC == 0)return 0;
curr_i=_tmp1EC;
if(curr_info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp33E;struct Cyc_String_pa_PrintArg_struct _tmp33D;void*_tmp33C[2];const char*_tmp33B;struct _dyneither_ptr*_tmp33A;curr_info=linfo != 0?(_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A[0]=(struct _dyneither_ptr)((_tmp33E.tag=0,((_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((_tmp33D.tag=0,((_tmp33D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info),((_tmp33C[0]=& _tmp33D,((_tmp33C[1]=& _tmp33E,Cyc_aprintf(((_tmp33B="%s+%s",_tag_dyneither(_tmp33B,sizeof(char),6))),_tag_dyneither(_tmp33C,sizeof(void*),2)))))))))))))),_tmp33A))): 0;}
# 1013
if(linfo != 0)linfo=linfo->tl;}
# 1015
return curr_i;};}
# 1018
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 1021
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 1026
for(0;la != 0;la=la->tl){
struct _tuple10*_tmp33F;struct Cyc_Interface_I*_tmp1F3=Cyc_Interface_merge(curr_i,get(la->hd),
curr_info != 0  && linfo != 0?(_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F->f1=*curr_info,((_tmp33F->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp33F))))): 0);
if(_tmp1F3 == 0)return 0;
curr_i=_tmp1F3;
if(curr_info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp349;struct Cyc_String_pa_PrintArg_struct _tmp348;void*_tmp347[2];const char*_tmp346;struct _dyneither_ptr*_tmp345;curr_info=linfo != 0?(_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345[0]=(struct _dyneither_ptr)((_tmp349.tag=0,((_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((_tmp348.tag=0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info),((_tmp347[0]=& _tmp348,((_tmp347[1]=& _tmp349,Cyc_aprintf(((_tmp346="%s+%s",_tag_dyneither(_tmp346,sizeof(char),6))),_tag_dyneither(_tmp347,sizeof(void*),2)))))))))))))),_tmp345))): 0;}
# 1034
if(linfo != 0)linfo=linfo->tl;}
# 1036
return curr_i;};}
# 1041
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds,struct Cyc_List_List*ns){
if(ns == 0)return tds;{
struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp34F;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp34E;struct Cyc_List_List*_tmp34D;return(_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->hd=Cyc_Absyn_new_decl((void*)((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F[0]=((_tmp34E.tag=10,((_tmp34E.f1=(struct _dyneither_ptr*)ns->hd,((_tmp34E.f2=Cyc_Interface_add_namespace(tds,ns->tl),_tmp34E)))))),_tmp34F)))),0),((_tmp34D->tl=0,_tmp34D)))));};}
# 1047
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp355;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp354;struct Cyc_List_List*_tmp353;return(_tmp353=_cycalloc(sizeof(*_tmp353)),((_tmp353->hd=Cyc_Absyn_new_decl((void*)((_tmp355=_cycalloc(sizeof(*_tmp355)),((_tmp355[0]=((_tmp354.tag=6,((_tmp354.f1=d,_tmp354)))),_tmp355)))),0),((_tmp353->tl=tds,_tmp353)))));}
# 1051
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 1053
{struct Cyc_Absyn_Aggrdecl*_tmp356;d=((_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=*d,_tmp356))));}
d->impl=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp35C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp35B;struct Cyc_List_List*_tmp35A;return(_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A->hd=Cyc_Absyn_new_decl((void*)((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C[0]=((_tmp35B.tag=6,((_tmp35B.f1=d,_tmp35B)))),_tmp35C)))),0),((_tmp35A->tl=tds,_tmp35A)))));};}
# 1059
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp362;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp361;struct Cyc_List_List*_tmp360;return(_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360->hd=Cyc_Absyn_new_decl((void*)((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp361.tag=7,((_tmp361.f1=d,_tmp361)))),_tmp362)))),0),((_tmp360->tl=tds,_tmp360)))));}static char _tmp207[2]="_";
# 1063
static struct _dyneither_ptr Cyc_Interface_us={_tmp207,_tmp207,_tmp207 + 2};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1066
static struct _tuple19*Cyc_Interface_rewrite_datatypefield_type(struct _tuple19*x){
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp36C;struct Cyc_Absyn_Tvar*_tmp36B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp36A;struct _tuple19*_tmp369;return(_tmp369=_cycalloc(sizeof(*_tmp369)),((_tmp369->f1=Cyc_Absyn_empty_tqual(0),((_tmp369->f2=(void*)(
(_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=((_tmp36A.tag=2,((_tmp36A.f1=((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B->name=Cyc_Interface_us_p,((_tmp36B->identity=- 1,((_tmp36B->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp36B)))))))),_tmp36A)))),_tmp36C)))),_tmp369)))));}
# 1071
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
{struct Cyc_Absyn_Datatypefield*_tmp36D;f=((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=*f,_tmp36D))));}
f->typs=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);
return f;}
# 1077
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1079
{struct Cyc_Absyn_Datatypedecl*_tmp36E;d=((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E[0]=*d,_tmp36E))));}
# 1081
if(d->fields != 0){struct Cyc_Core_Opt*_tmp36F;d->fields=((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v),_tmp36F))));}
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp375;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp374;struct Cyc_List_List*_tmp373;return(_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->hd=Cyc_Absyn_new_decl((void*)((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375[0]=((_tmp374.tag=7,((_tmp374.f1=d,_tmp374)))),_tmp375)))),0),((_tmp373->tl=tds,_tmp373)))));};}
# 1086
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp37B;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp37A;struct Cyc_List_List*_tmp379;return(_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379->hd=Cyc_Absyn_new_decl((void*)((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B[0]=((_tmp37A.tag=8,((_tmp37A.f1=d,_tmp37A)))),_tmp37B)))),0),((_tmp379->tl=tds,_tmp379)))));}
# 1090
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1092
{struct Cyc_Absyn_Enumdecl*_tmp37C;d=((_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=*d,_tmp37C))));}
d->fields=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp382;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp381;struct Cyc_List_List*_tmp380;return(_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->hd=Cyc_Absyn_new_decl((void*)((_tmp382=_cycalloc(sizeof(*_tmp382)),((_tmp382[0]=((_tmp381.tag=8,((_tmp381.f1=d,_tmp381)))),_tmp382)))),0),((_tmp380->tl=tds,_tmp380)))));};}
# 1098
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1100
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp388;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp387;struct Cyc_List_List*_tmp386;return(_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->hd=Cyc_Absyn_new_decl((void*)((_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388[0]=((_tmp387.tag=9,((_tmp387.f1=d,_tmp387)))),_tmp388)))),0),((_tmp386->tl=tds,_tmp386)))));}
# 1103
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp38E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp38D;struct Cyc_List_List*_tmp38C;return(_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C->hd=Cyc_Absyn_new_decl((void*)((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E[0]=((_tmp38D.tag=0,((_tmp38D.f1=d,_tmp38D)))),_tmp38E)))),0),((_tmp38C->tl=tds,_tmp38C)))));}
# 1107
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1109
struct Cyc_Absyn_Datatypedecl*_tmp220;struct Cyc_Absyn_Datatypefield*_tmp221;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp21F=d;_tmp220=_tmp21F->base;_tmp221=_tmp21F->field;
{struct Cyc_Absyn_Datatypedecl*_tmp38F;_tmp220=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F[0]=*_tmp220,_tmp38F))));}
{struct Cyc_List_List*_tmp392;struct Cyc_Core_Opt*_tmp391;_tmp220->fields=((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391->v=((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392->hd=_tmp221,((_tmp392->tl=0,_tmp392)))))),_tmp391))));}
_tmp220->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp398;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp397;struct Cyc_List_List*_tmp396;return(_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396->hd=Cyc_Absyn_new_decl((void*)((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398[0]=((_tmp397.tag=7,((_tmp397.f1=_tmp220,_tmp397)))),_tmp398)))),0),((_tmp396->tl=tds,_tmp396)))));};}
# 1116
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1118
struct Cyc_Absyn_Datatypedecl*_tmp229;struct Cyc_Absyn_Datatypefield*_tmp22A;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp228=d;_tmp229=_tmp228->base;_tmp22A=_tmp228->field;
{struct Cyc_Absyn_Datatypedecl*_tmp399;_tmp229=((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=*_tmp229,_tmp399))));}
_tmp22A=Cyc_Interface_rewrite_datatypefield(_tmp22A);
_tmp22A->sc=Cyc_Absyn_Extern;
{struct Cyc_List_List*_tmp39C;struct Cyc_Core_Opt*_tmp39B;_tmp229->fields=((_tmp39B=_cycalloc(sizeof(*_tmp39B)),((_tmp39B->v=((_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->hd=_tmp22A,((_tmp39C->tl=0,_tmp39C)))))),_tmp39B))));}
_tmp229->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp3A2;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp3A1;struct Cyc_List_List*_tmp3A0;return(_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0->hd=Cyc_Absyn_new_decl((void*)((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2[0]=((_tmp3A1.tag=7,((_tmp3A1.f1=_tmp229,_tmp3A1)))),_tmp3A2)))),0),((_tmp3A0->tl=tds,_tmp3A0)))));};}
# 1127
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp231=0;
_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp231);
_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp231);
_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp231);
# 1133
if(_tmp231 != 0){
_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp231);
_tmp231=Cyc_Interface_add_namespace(_tmp231,ns);
Cyc_Absynpp_decllist2file(_tmp231,f);}}
# 1140
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
# 1142
struct Cyc_List_List*_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp232 != 0){
_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp232);
_tmp232=Cyc_Interface_add_namespace(_tmp232,ns);
Cyc_Absynpp_decllist2file(_tmp232,f);}}
# 1150
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp233 != 0){
_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp233);
_tmp233=Cyc_Interface_add_namespace(_tmp233,ns);
Cyc_Absynpp_decllist2file(_tmp233,f);}}
# 1159
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp234=0;
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp234);
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp234);
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp234);
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp234);
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp234);
# 1167
if(_tmp234 != 0){
_tmp234=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp234);
_tmp234=Cyc_Interface_add_namespace(_tmp234,ns);
Cyc_Absynpp_decllist2file(_tmp234,f);}}
# 1177
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_fprintf(f,((_tmp3A5="/****** needed (headers) ******/\n",_tag_dyneither(_tmp3A5,sizeof(char),34))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->imports);
# 1182
{const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_fprintf(f,((_tmp3A8="\n/****** provided (headers) ******/\n",_tag_dyneither(_tmp3A8,sizeof(char),37))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->exports);
# 1187
{const char*_tmp3AB;void*_tmp3AA;(_tmp3AA=0,Cyc_fprintf(f,((_tmp3AB="\n/****** needed (headers of xdatatypefielddecls) ******/\n",_tag_dyneither(_tmp3AB,sizeof(char),58))),_tag_dyneither(_tmp3AA,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,f,i->imports);
# 1190
{const char*_tmp3AE;void*_tmp3AD;(_tmp3AD=0,Cyc_fprintf(f,((_tmp3AE="\n/****** provided (headers of xdatatypefielddecls) ******/\n",_tag_dyneither(_tmp3AE,sizeof(char),60))),_tag_dyneither(_tmp3AD,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,f,i->exports);
# 1194
{const char*_tmp3B1;void*_tmp3B0;(_tmp3B0=0,Cyc_fprintf(f,((_tmp3B1="\n/****** provided (typedefs) ******/\n",_tag_dyneither(_tmp3B1,sizeof(char),38))),_tag_dyneither(_tmp3B0,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,f,i->exports);
# 1197
{const char*_tmp3B4;void*_tmp3B3;(_tmp3B3=0,Cyc_fprintf(f,((_tmp3B4="\n/****** needed (declarations) ******/\n",_tag_dyneither(_tmp3B4,sizeof(char),40))),_tag_dyneither(_tmp3B3,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->imports);
# 1200
{const char*_tmp3B7;void*_tmp3B6;(_tmp3B6=0,Cyc_fprintf(f,((_tmp3B7="\n/****** provided (declarations) ******/\n",_tag_dyneither(_tmp3B7,sizeof(char),42))),_tag_dyneither(_tmp3B6,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->exports);}
# 1207
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp243=Cyc_Parse_parse_file(f);
struct _RegionHandle _tmp244=_new_region("r");struct _RegionHandle*r=& _tmp244;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp245=Cyc_Tcenv_tc_init(r);
Cyc_Tc_tc(r,_tmp245,0,_tmp243);{
struct Cyc_Interface_I*_tmp246=Cyc_Interface_gen_extract(_tmp245->ae,0);_npop_handler(0);return _tmp246;};}
# 1211
;_pop_region(r);};}
# 1221
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1227
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
