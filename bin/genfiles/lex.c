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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;
# 169
void Cyc_Core_ufree(struct _dyneither_ptr ptr);
# 197
struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(const char*file,int lineno);extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 214 "core.h"
void Cyc_Core_free_dynregion(struct _DynRegionHandle*);
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err_lex(unsigned int,struct _dyneither_ptr);
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
# 906 "absyn.h"
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_FlatList{struct Cyc_FlatList*tl;};
# 36 "parse.h"
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple16 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;
# 80
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 342 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 43 "lex.cyl"
void Cyc_yyerror(struct _dyneither_ptr s){
Cyc_Position_post_error(Cyc_Position_mk_err_parse((unsigned int)Cyc_yylloc.first_line,s));}
# 49
int Cyc_Lex_compile_for_boot_r=0;struct Cyc_Lex_Trie;
# 52
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple18{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple18 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 58
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren _tmp14D;return((_tmp14D.Zero).val=0,(((_tmp14D.Zero).tag=1,_tmp14D)));}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
struct _tuple18 _tmp150;union Cyc_Lex_TrieChildren _tmp14F;return((_tmp14F.One).val=((_tmp150.f1=i,((_tmp150.f2=t,_tmp150)))),(((_tmp14F.One).tag=2,_tmp14F)));}
# 62
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
union Cyc_Lex_TrieChildren _tmp151;return((_tmp151.Many).val=ts,(((_tmp151.Many).tag=3,_tmp151)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};
# 73
static int Cyc_Lex_num_kws=0;
static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct _DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;};
# 81
typedef struct Cyc_Lex_DynTrie*Cyc_Lex_dyn_trie_t;struct Cyc_Lex_DynTrieSymbols{struct _DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 88
typedef struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_dyn_trie_symbols_t;
typedef struct Cyc_Lex_DynSymbols*Cyc_Lex_dyn_symbols_t;
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 93
static int Cyc_Lex_comment_depth=0;
# 96
static struct _tuple5 Cyc_Lex_token_int={Cyc_Absyn_Signed,0};static char _tmp4[8]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp4,_tmp4,_tmp4 + 8};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 101
static char Cyc_Lex_token_char='\000';static char _tmp5[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmp5,_tmp5,_tmp5 + 1};
static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp6[1]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmp6,_tmp6,_tmp6 + 1}};
# 106
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 109
static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}
# 113
static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}struct _tuple19{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp7[14]="__attribute__";static char _tmp8[9]="abstract";static char _tmp9[6]="alias";static char _tmpA[5]="auto";static char _tmpB[6]="break";static char _tmpC[7]="calloc";static char _tmpD[5]="case";static char _tmpE[6]="catch";static char _tmpF[5]="char";static char _tmp10[6]="const";static char _tmp11[9]="continue";static char _tmp12[9]="datatype";static char _tmp13[8]="default";static char _tmp14[3]="do";static char _tmp15[7]="double";static char _tmp16[12]="dynregion_t";static char _tmp17[5]="else";static char _tmp18[5]="enum";static char _tmp19[7]="export";static char _tmp1A[7]="extern";static char _tmp1B[9]="fallthru";static char _tmp1C[6]="float";static char _tmp1D[4]="for";static char _tmp1E[5]="goto";static char _tmp1F[3]="if";static char _tmp20[7]="inline";static char _tmp21[11]="__inline__";static char _tmp22[4]="int";static char _tmp23[4]="let";static char _tmp24[5]="long";static char _tmp25[7]="malloc";static char _tmp26[10]="namespace";static char _tmp27[4]="new";static char _tmp28[11]="NOZEROTERM";static char _tmp29[5]="NULL";static char _tmp2A[8]="numelts";static char _tmp2B[9]="offsetof";static char _tmp2C[20]="__cyclone_port_on__";static char _tmp2D[21]="__cyclone_port_off__";static char _tmp2E[8]="rcalloc";static char _tmp2F[9]="region_t";static char _tmp30[7]="region";static char _tmp31[8]="regions";static char _tmp32[9]="register";static char _tmp33[13]="reset_region";static char _tmp34[9]="restrict";static char _tmp35[7]="return";static char _tmp36[8]="rmalloc";static char _tmp37[5]="rnew";static char _tmp38[6]="short";static char _tmp39[7]="signed";static char _tmp3A[7]="sizeof";static char _tmp3B[7]="static";static char _tmp3C[7]="struct";static char _tmp3D[7]="switch";static char _tmp3E[9]="tagcheck";static char _tmp3F[6]="tag_t";static char _tmp40[6]="throw";static char _tmp41[4]="try";static char _tmp42[7]="tunion";static char _tmp43[8]="typedef";static char _tmp44[6]="union";static char _tmp45[9]="unsigned";static char _tmp46[6]="using";static char _tmp47[8]="valueof";static char _tmp48[10]="valueof_t";static char _tmp49[5]="void";static char _tmp4A[9]="volatile";static char _tmp4B[6]="while";static char _tmp4C[8]="xtunion";static char _tmp4D[9]="ZEROTERM";
# 121
static struct _tuple19 Cyc_Lex_rw_array[71]={{{_tmp7,_tmp7,_tmp7 + 14},372,1},{{_tmp8,_tmp8,_tmp8 + 9},300,0},{{_tmp9,_tmp9,_tmp9 + 6},324,0},{{_tmpA,_tmpA,_tmpA + 5},258,1},{{_tmpB,_tmpB,_tmpB + 6},290,1},{{_tmpC,_tmpC,_tmpC + 7},309,0},{{_tmpD,_tmpD,_tmpD + 5},277,1},{{_tmpE,_tmpE,_tmpE + 6},297,1},{{_tmpF,_tmpF,_tmpF + 5},264,1},{{_tmp10,_tmp10,_tmp10 + 6},272,1},{{_tmp11,_tmp11,_tmp11 + 9},289,1},{{_tmp12,_tmp12,_tmp12 + 9},304,0},{{_tmp13,_tmp13,_tmp13 + 8},278,1},{{_tmp14,_tmp14,_tmp14 + 3},286,1},{{_tmp15,_tmp15,_tmp15 + 7},269,1},{{_tmp16,_tmp16,_tmp16 + 12},323,0},{{_tmp17,_tmp17,_tmp17 + 5},283,1},{{_tmp18,_tmp18,_tmp18 + 5},292,1},{{_tmp19,_tmp19,_tmp19 + 7},298,0},{{_tmp1A,_tmp1A,_tmp1A + 7},261,1},{{_tmp1B,_tmp1B,_tmp1B + 9},301,0},{{_tmp1C,_tmp1C,_tmp1C + 6},268,1},{{_tmp1D,_tmp1D,_tmp1D + 4},287,1},{{_tmp1E,_tmp1E,_tmp1E + 5},288,1},{{_tmp1F,_tmp1F,_tmp1F + 3},282,1},{{_tmp20,_tmp20,_tmp20 + 7},279,1},{{_tmp21,_tmp21,_tmp21 + 11},279,1},{{_tmp22,_tmp22,_tmp22 + 4},266,1},{{_tmp23,_tmp23,_tmp23 + 4},294,0},{{_tmp24,_tmp24,_tmp24 + 5},267,1},{{_tmp25,_tmp25,_tmp25 + 7},307,0},{{_tmp26,_tmp26,_tmp26 + 10},303,0},{{_tmp27,_tmp27,_tmp27 + 4},299,0},{{_tmp28,_tmp28,_tmp28 + 11},318,0},{{_tmp29,_tmp29,_tmp29 + 5},293,0},{{_tmp2A,_tmp2A,_tmp2A + 8},325,0},{{_tmp2B,_tmp2B,_tmp2B + 9},281,1},{{_tmp2C,_tmp2C,_tmp2C + 20},321,0},{{_tmp2D,_tmp2D,_tmp2D + 21},322,0},{{_tmp2E,_tmp2E,_tmp2E + 8},310,0},{{_tmp2F,_tmp2F,_tmp2F + 9},312,0},{{_tmp30,_tmp30,_tmp30 + 7},314,0},{{_tmp31,_tmp31,_tmp31 + 8},316,0},{{_tmp32,_tmp32,_tmp32 + 9},259,1},{{_tmp33,_tmp33,_tmp33 + 13},317,0},{{_tmp34,_tmp34,_tmp34 + 9},274,1},{{_tmp35,_tmp35,_tmp35 + 7},291,1},{{_tmp36,_tmp36,_tmp36 + 8},308,0},{{_tmp37,_tmp37,_tmp37 + 5},315,0},{{_tmp38,_tmp38,_tmp38 + 6},265,1},{{_tmp39,_tmp39,_tmp39 + 7},270,1},{{_tmp3A,_tmp3A,_tmp3A + 7},280,1},{{_tmp3B,_tmp3B,_tmp3B + 7},260,1},{{_tmp3C,_tmp3C,_tmp3C + 7},275,1},{{_tmp3D,_tmp3D,_tmp3D + 7},284,1},{{_tmp3E,_tmp3E,_tmp3E + 9},328,0},{{_tmp3F,_tmp3F,_tmp3F + 6},313,0},{{_tmp40,_tmp40,_tmp40 + 6},295,0},{{_tmp41,_tmp41,_tmp41 + 4},296,0},{{_tmp42,_tmp42,_tmp42 + 7},306,0},{{_tmp43,_tmp43,_tmp43 + 8},262,1},{{_tmp44,_tmp44,_tmp44 + 6},276,1},{{_tmp45,_tmp45,_tmp45 + 9},271,1},{{_tmp46,_tmp46,_tmp46 + 6},302,0},{{_tmp47,_tmp47,_tmp47 + 8},326,0},{{_tmp48,_tmp48,_tmp48 + 10},327,0},{{_tmp49,_tmp49,_tmp49 + 5},263,1},{{_tmp4A,_tmp4A,_tmp4A + 9},273,1},{{_tmp4B,_tmp4B,_tmp4B + 6},285,1},{{_tmp4C,_tmp4C,_tmp4C + 8},305,0},{{_tmp4D,_tmp4D,_tmp4D + 9},319,0}};
# 195
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned int i=0;for(0;i < 71;++ i){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 204
static int Cyc_Lex_trie_char(int c){
# 206
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void _tmp157(unsigned int*_tmp156,unsigned int*_tmp155,struct Cyc_Lex_Trie***_tmp154){for(*_tmp156=0;*_tmp156 < *_tmp155;(*_tmp156)++){(*_tmp154)[*_tmp156]=0;}}
# 211
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 213
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp4E=((struct Cyc_Lex_Trie*)_check_null(t))->children;struct Cyc_Lex_Trie**_tmp4F;int _tmp50;struct Cyc_Lex_Trie*_tmp51;_LL1: if((_tmp4E.Many).tag != 3)goto _LL3;_tmp4F=(struct Cyc_Lex_Trie**)(_tmp4E.Many).val;_LL2: {
# 219
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp4F[_check_known_subscript_notnull(64,ch)]== 0){
struct Cyc_Lex_Trie*_tmp152;_tmp4F[_check_known_subscript_notnull(64,ch)]=((_tmp152=_region_malloc(r,sizeof(*_tmp152)),((_tmp152->children=Cyc_Lex_Zero(),((_tmp152->shared_str=0,_tmp152))))));}
t=_tmp4F[_check_known_subscript_notnull(64,ch)];
++ i;
goto _LL0;}_LL3: if((_tmp4E.One).tag != 2)goto _LL5;_tmp50=((struct _tuple18)(_tmp4E.One).val).f1;_tmp51=((struct _tuple18)(_tmp4E.One).val).f2;_LL4:
# 227
 if(_tmp50 == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp51;else{
# 230
unsigned int _tmp156;unsigned int _tmp155;struct Cyc_Lex_Trie**_tmp154;unsigned int _tmp153;struct Cyc_Lex_Trie**_tmp53=(_tmp153=64,((_tmp154=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp153)),((((_tmp155=_tmp153,_tmp157(& _tmp156,& _tmp155,& _tmp154))),_tmp154)))));
_tmp53[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp50))]=_tmp51;{
int _tmp54=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
{struct Cyc_Lex_Trie*_tmp158;_tmp53[_check_known_subscript_notnull(64,_tmp54)]=((_tmp158=_region_malloc(r,sizeof(*_tmp158)),((_tmp158->children=Cyc_Lex_Zero(),((_tmp158->shared_str=0,_tmp158))))));}
t->children=Cyc_Lex_Many(_tmp53);
t=_tmp53[_check_known_subscript_notnull(64,_tmp54)];};}
# 237
++ i;
goto _LL0;_LL5: if((_tmp4E.Zero).tag != 1)goto _LL0;_LL6:
# 241
 while(i <= last){
struct Cyc_Lex_Trie*_tmp159;struct Cyc_Lex_Trie*_tmp59=(_tmp159=_region_malloc(r,sizeof(*_tmp159)),((_tmp159->children=Cyc_Lex_Zero(),((_tmp159->shared_str=0,_tmp159)))));
((struct Cyc_Lex_Trie*)_check_null(t))->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp59);
t=_tmp59;}
# 246
return t;_LL0:;}
# 249
return t;}
# 252
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct _DynRegionHandle*_tmp5C;struct Cyc_Lex_Trie*_tmp5D;struct Cyc_Xarray_Xarray*_tmp5E;struct Cyc_Lex_DynTrieSymbols _tmp5B=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp5C=_tmp5B.dyn;_tmp5D=_tmp5B.t;_tmp5E=_tmp5B.symbols;{
struct _DynRegionFrame _tmp5F;struct _RegionHandle*d=_open_dynregion(& _tmp5F,_tmp5C);
{struct Cyc_Lex_Trie*_tmp60=Cyc_Lex_trie_lookup(d,_tmp5D,buff,offset,len);
# 257
if(_tmp60->shared_str == 0){
struct _dyneither_ptr _tmp61=Cyc_Core_new_string((unsigned int)(len + 1));
Cyc_zstrncpy(_dyneither_ptr_decrease_size(_tmp61,sizeof(char),1),(struct _dyneither_ptr)_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned long)len);{
struct _dyneither_ptr*_tmp15A;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp5E,((_tmp15A=_cycalloc(sizeof(*_tmp15A)),((_tmp15A[0]=(struct _dyneither_ptr)_tmp61,_tmp15A)))));
_tmp60->shared_str=ans;};}{
# 263
int _tmp63=_tmp60->shared_str;_npop_handler(0);return _tmp63;};}
# 255
;_pop_dynregion(d);};}
# 267
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}
# 273
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp64=*sptr;
struct _DynRegionHandle*_tmp66;struct Cyc_Lex_Trie*_tmp67;struct Cyc_Lex_DynTrie _tmp65=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp66=_tmp65.dyn;_tmp67=_tmp65.t;{
struct _DynRegionFrame _tmp68;struct _RegionHandle*d=_open_dynregion(& _tmp68,_tmp66);{
struct Cyc_Lex_Trie*_tmp69=Cyc_Lex_trie_lookup(d,_tmp67,_tmp64,0,(int)(_get_dyneither_size(_tmp64,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp69))->shared_str=1;}
# 280
_npop_handler(0);return;
# 276
;_pop_dynregion(d);};}
# 283
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct _DynRegionHandle*_tmp6B;struct Cyc_Lex_Trie*_tmp6C;struct Cyc_Xarray_Xarray*_tmp6D;struct Cyc_Lex_DynTrieSymbols _tmp6A=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp6B=_tmp6A.dyn;_tmp6C=_tmp6A.t;_tmp6D=_tmp6A.symbols;{
struct _DynRegionFrame _tmp6E;struct _RegionHandle*d=_open_dynregion(& _tmp6E,_tmp6B);
{struct _dyneither_ptr*_tmp6F=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp6D,symbol_num);_npop_handler(0);return _tmp6F;};_pop_dynregion(d);};}
# 298 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
struct Cyc_Core_Invalid_argument_exn_struct _tmp160;const char*_tmp15F;struct Cyc_Core_Invalid_argument_exn_struct*_tmp15E;(int)_throw((void*)((_tmp15E=_cycalloc(sizeof(*_tmp15E)),((_tmp15E[0]=((_tmp160.tag=Cyc_Core_Invalid_argument,((_tmp160.f1=((_tmp15F="string to integer conversion",_tag_dyneither(_tmp15F,sizeof(char),29))),_tmp160)))),_tmp15E)))));}}}}
# 308
static struct _tuple5 Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
unsigned int n=0;
int end2=lbuf->lex_curr_pos - end;
struct _dyneither_ptr buff=lbuf->lex_buffer;
int i=start + lbuf->lex_start_pos;
{int i=start + lbuf->lex_start_pos;for(0;i < end2;++ i){
char c=*((char*)_check_dyneither_subscript(buff,sizeof(char),i));
switch(c){case 'u': _LL7:
# 317
 goto _LL8;case 'U': _LL8: {
struct _tuple5 _tmp161;return(_tmp161.f1=Cyc_Absyn_Unsigned,((_tmp161.f2=(int)n,_tmp161)));}case 'l': _LL9:
 break;case 'L': _LLA:
 break;default: _LLB:
 n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}{
# 324
struct _tuple5 _tmp162;return(_tmp162.f1=Cyc_Absyn_Signed,((_tmp162.f2=(int)n,_tmp162)));};}
# 329
char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};
int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c);static void _tmp168(int*sz,unsigned int*_tmp167,unsigned int*_tmp166,char**_tmp164){for(*_tmp167=0;*_tmp167 < *_tmp166;(*_tmp167)++){(*_tmp164)[*_tmp167]=
# 337
*_tmp167 < *sz?*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*_tmp167)):'\000';}}
# 332
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
unsigned int _tmp167;unsigned int _tmp166;struct _dyneither_ptr _tmp165;char*_tmp164;unsigned int _tmp163;struct _dyneither_ptr str=(_tmp163=(unsigned int)newsz,((_tmp164=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp163 + 1)),((_tmp165=_tag_dyneither(_tmp164,sizeof(char),_tmp163 + 1),((((_tmp166=_tmp163,((_tmp168(& sz,& _tmp167,& _tmp166,& _tmp164),_tmp164[_tmp166]=(char)0)))),_tmp165)))))));
Cyc_Lex_string_buffer=str;};}
# 340
{char _tmp16B;char _tmp16A;struct _dyneither_ptr _tmp169;(_tmp169=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos),((_tmp16A=*((char*)_check_dyneither_subscript(_tmp169,sizeof(char),0)),((_tmp16B=c,((_get_dyneither_size(_tmp169,sizeof(char))== 1  && (_tmp16A == '\000'  && _tmp16B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp169.curr)=_tmp16B)))))));}
++ Cyc_Lex_string_pos;}
# 343
void Cyc_Lex_store_string(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));}}
# 348
struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};
# 361
typedef struct Cyc_Lex_Ldecls*Cyc_Lex_ldecls_t;struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct Cyc_List_List*imported_namespaces;};
# 368
typedef struct Cyc_Lex_Lvis*Cyc_Lex_lvis_t;struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
# 376
typedef struct Cyc_Lex_Lstate*Cyc_Lex_lstate_t;
# 378
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 380
static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp16C;struct Cyc_Lex_Lvis*_tmp7C=(_tmp16C=_cycalloc(sizeof(*_tmp16C)),((_tmp16C->current_namespace=0,((_tmp16C->imported_namespaces=0,_tmp16C)))));
# 383
struct Cyc_List_List*_tmp16D;struct Cyc_List_List*_tmp7D=(_tmp16D=_cycalloc(sizeof(*_tmp16D)),((_tmp16D->hd=_tmp7C,((_tmp16D->tl=0,_tmp16D)))));
struct Cyc_Lex_Ldecls*_tmp16E;struct Cyc_Dict_Dict init_decls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,(
# 387
(_tmp16E=_cycalloc(sizeof(*_tmp16E)),((_tmp16E->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp16E->namespaces=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp16E)))))));
struct Cyc_Lex_Lstate*_tmp171;struct Cyc_Core_Opt*_tmp170;Cyc_Lex_lstate=((_tmp170=_cycalloc(sizeof(*_tmp170)),((_tmp170->v=((_tmp171=_cycalloc(sizeof(*_tmp171)),((_tmp171->lstack=_tmp7D,((_tmp171->decls=init_decls,_tmp171)))))),_tmp170))));}
# 392
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
# 394
struct _dyneither_ptr*n=(struct _dyneither_ptr*)ns->hd;
# 396
{struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
# 399
struct Cyc_List_List*_tmp172;struct Cyc_List_List*x=(_tmp172=_cycalloc(sizeof(*_tmp172)),((_tmp172->hd=lv->current_namespace,((_tmp172->tl=lv->imported_namespaces,_tmp172)))));
# 401
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
# 404
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))
# 406
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,ns);}}}
# 409
{const char*_tmp178;void*_tmp177[1];const char*_tmp176;struct Cyc_String_pa_PrintArg_struct _tmp175;Cyc_yyerror((struct _dyneither_ptr)((_tmp175.tag=0,((_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(ns,((_tmp176="::",_tag_dyneither(_tmp176,sizeof(char),3))))),((_tmp177[0]=& _tmp175,Cyc_aprintf(((_tmp178="undeclared namespace %s",_tag_dyneither(_tmp178,sizeof(char),24))),_tag_dyneither(_tmp177,sizeof(void*),1)))))))));}
return 0;}
# 413
static void Cyc_Lex_recompute_typedefs(){
# 415
struct _DynRegionHandle*_tmp89;struct Cyc_Lex_Trie*_tmp8A;struct Cyc_Lex_DynTrie _tmp88=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp89=_tmp88.dyn;_tmp8A=_tmp88.t;
Cyc_Core_free_dynregion(_tmp89);{
struct _DynRegionHandle*_tmp8C;struct Cyc_Core_NewRegion _tmp8B=Cyc_Core__new_dynregion("internal-error",0);_tmp8C=_tmp8B.dynregion;{
struct _DynRegionFrame _tmp8D;struct _RegionHandle*d2=_open_dynregion(& _tmp8D,_tmp8C);{
struct Cyc_Lex_Trie*_tmp17B;struct Cyc_Lex_DynTrie*_tmp17A;Cyc_Lex_typedefs_trie=((_tmp17A=_cycalloc(sizeof(*_tmp17A)),((_tmp17A->dyn=(struct _DynRegionHandle*)_tmp8C,((_tmp17A->t=(struct Cyc_Lex_Trie*)((_tmp17B=_region_malloc(d2,sizeof(*_tmp17B)),((_tmp17B->children=Cyc_Lex_Zero(),((_tmp17B->shared_str=0,_tmp17B)))))),(struct Cyc_Lex_DynTrie*)_tmp17A))))));}{
# 423
struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
struct Cyc_List_List*_tmp17C;struct Cyc_List_List*x=(_tmp17C=_cycalloc(sizeof(*_tmp17C)),((_tmp17C->hd=lv->current_namespace,((_tmp17C->tl=lv->imported_namespaces,_tmp17C)))));
# 427
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};
# 418
;_pop_dynregion(d2);};};}
# 435
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmp91;_push_handler(& _tmp91);{int _tmp93=0;if(setjmp(_tmp91.handler))_tmp93=1;if(!_tmp93){
{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);
int _tmp94=((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);_npop_handler(0);return _tmp94;}
# 438
;_pop_handler();}else{void*_tmp92=(void*)_exn_thrown;void*_tmp96=_tmp92;void*_tmp98;_LLE: {struct Cyc_Dict_Absent_exn_struct*_tmp97=(struct Cyc_Dict_Absent_exn_struct*)_tmp96;if(_tmp97->tag != Cyc_Dict_Absent)goto _LL10;}_LLF:
# 441
 return 0;_LL10: _tmp98=_tmp96;_LL11:(void)_throw(_tmp98);_LLD:;}};}
# 444
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 449
struct _dyneither_ptr _tmp99=*v;
int len=(int)(_get_dyneither_size(_tmp99,sizeof(char))- 1);
struct _DynRegionHandle*_tmp9B;struct Cyc_Lex_Trie*_tmp9C;struct Cyc_Lex_DynTrie _tmp9A=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp9B=_tmp9A.dyn;_tmp9C=_tmp9A.t;{
struct _DynRegionFrame _tmp9D;struct _RegionHandle*d=_open_dynregion(& _tmp9D,_tmp9B);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmp9E=((struct Cyc_Lex_Trie*)_check_null(_tmp9C))->children;int _tmp9F;struct Cyc_Lex_Trie*_tmpA0;struct Cyc_Lex_Trie*_tmpA1;struct Cyc_Lex_Trie**_tmpA2;_LL13: if((_tmp9E.Zero).tag != 1)goto _LL15;_LL14: {
int _tmpA3=0;_npop_handler(0);return _tmpA3;}_LL15: if((_tmp9E.One).tag != 2)goto _LL17;_tmp9F=((struct _tuple18)(_tmp9E.One).val).f1;_tmpA0=((struct _tuple18)(_tmp9E.One).val).f2;if(!(_tmp9F != *((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),i))))goto _LL17;_LL16: {
int _tmpA4=0;_npop_handler(0);return _tmpA4;}_LL17: if((_tmp9E.One).tag != 2)goto _LL19;_tmpA1=((struct _tuple18)(_tmp9E.One).val).f2;_LL18:
 _tmp9C=_tmpA1;goto _LL12;_LL19: if((_tmp9E.Many).tag != 3)goto _LL12;_tmpA2=(struct Cyc_Lex_Trie**)(_tmp9E.Many).val;_LL1A: {
# 459
struct Cyc_Lex_Trie*_tmpA5=_tmpA2[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(_tmp99,sizeof(char),i))))];
if(_tmpA5 == 0){
int _tmpA6=0;_npop_handler(0);return _tmpA6;}
_tmp9C=_tmpA5;
goto _LL12;}_LL12:;}}{
# 465
int _tmpA7=((struct Cyc_Lex_Trie*)_check_null(_tmp9C))->shared_str;_npop_handler(0);return _tmpA7;};
# 453
;_pop_dynregion(d);};};}
# 469
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp17D;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp17D=_cycalloc(sizeof(*_tmp17D)),((_tmp17D->hd=s,((_tmp17D->tl=0,_tmp17D)))))));
# 473
{struct Cyc_Lex_Lvis*_tmp180;struct Cyc_List_List*_tmp17F;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
(_tmp17F=_cycalloc(sizeof(*_tmp17F)),((_tmp17F->hd=((_tmp180=_cycalloc(sizeof(*_tmp180)),((_tmp180->current_namespace=new_ns,((_tmp180->imported_namespaces=0,_tmp180)))))),((_tmp17F->tl=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,_tmp17F))))));}{
# 479
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns);
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){
{struct Cyc_Lex_Ldecls*_tmp181;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns,(
# 483
(_tmp181=_cycalloc(sizeof(*_tmp181)),((_tmp181->typedefs=ld->typedefs,((_tmp181->namespaces=
# 485
((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s),_tmp181)))))));}{
# 487
struct Cyc_Lex_Ldecls*_tmp182;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns,(
# 489
(_tmp182=_cycalloc(sizeof(*_tmp182)),((_tmp182->typedefs=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp182->namespaces=
# 492
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp182)))))));};}
# 498
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);};}
# 500
void Cyc_Lex_leave_namespace(){
# 502
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}
# 505
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmpAD=(*q).f1;struct Cyc_List_List*_tmpAE;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpB0;_LL1C: if((_tmpAD.Loc_n).tag != 4)goto _LL1E;_LL1D:
# 509
{struct Cyc_List_List*_tmp183;ns=((_tmp183=_cycalloc(sizeof(*_tmp183)),((_tmp183->hd=(*q).f2,((_tmp183->tl=0,_tmp183))))));}
goto _LL1B;_LL1E: if((_tmpAD.Rel_n).tag != 1)goto _LL20;_tmpAE=(struct Cyc_List_List*)(_tmpAD.Rel_n).val;_LL1F:
 _tmpAF=_tmpAE;goto _LL21;_LL20: if((_tmpAD.C_n).tag != 3)goto _LL22;_tmpAF=(struct Cyc_List_List*)(_tmpAD.C_n).val;_LL21:
 _tmpB0=_tmpAF;goto _LL23;_LL22: if((_tmpAD.Abs_n).tag != 2)goto _LL1B;_tmpB0=(struct Cyc_List_List*)(_tmpAD.Abs_n).val;_LL23:
# 514
{struct Cyc_List_List*_tmp184;ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB0,((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184->hd=(*q).f2,((_tmp184->tl=0,_tmp184))))))));}
goto _LL1B;_LL1B:;}{
# 518
struct Cyc_List_List*_tmpB3=Cyc_Lex_get_absolute_namespace(ns);
struct Cyc_List_List*_tmpB4=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
# 521
{struct Cyc_List_List*_tmp185;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp185=_cycalloc(sizeof(*_tmp185)),((_tmp185->hd=_tmpB3,((_tmp185->tl=_tmpB4,_tmp185))))));}
# 525
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,_tmpB3))->typedefs);};}
# 527
void Cyc_Lex_leave_using(){
struct Cyc_List_List*_tmpB6=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)_check_null(_tmpB6))->tl;
# 532
Cyc_Lex_recompute_typedefs();}
# 535
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 537
struct Cyc_List_List*_tmpB7=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpB8=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls*_tmpB9=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpB8,_tmpB7);
struct Cyc_Lex_Ldecls*_tmp186;struct Cyc_Lex_Ldecls*_tmpBA=(_tmp186=_cycalloc(sizeof(*_tmp186)),((_tmp186->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpB9->typedefs,(*q).f2),((_tmp186->namespaces=_tmpB9->namespaces,_tmp186)))));
# 542
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpB8,_tmpB7,_tmpBA);
# 546
Cyc_Lex_insert_typedef((*q).f2);}
# 549
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 553
if(symbol_num <= Cyc_Lex_num_kws){
short res=(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),symbol_num - 1));
switch(res){case 319: _LL24:
# 557
{const char*_tmp189;void*_tmp188;(_tmp188=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp189="deprecated ZEROTERM, use @zeroterm",_tag_dyneither(_tmp189,sizeof(char),35))),_tag_dyneither(_tmp188,sizeof(void*),0)));}
break;case 318: _LL25:
# 560
{const char*_tmp18C;void*_tmp18B;(_tmp18B=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp18C="deprecated NOZEROTERM, use @nozeroterm",_tag_dyneither(_tmp18C,sizeof(char),39))),_tag_dyneither(_tmp18B,sizeof(void*),0)));}
break;case 306: _LL26:
# 563
{const char*_tmp18F;void*_tmp18E;(_tmp18E=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp18F="deprecated tunion, use datatype",_tag_dyneither(_tmp18F,sizeof(char),32))),_tag_dyneither(_tmp18E,sizeof(void*),0)));}
break;case 305: _LL27:
# 566
{const char*_tmp192;void*_tmp191;(_tmp191=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp192="deprecated xtunion, use @extensible datatype",_tag_dyneither(_tmp192,sizeof(char),45))),_tag_dyneither(_tmp191,sizeof(void*),0)));}
break;default: _LL28:
 break;}
# 570
return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),symbol_num - 1));}{
# 573
struct _dyneither_ptr*_tmpC4=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpC4;
# 577
if(Cyc_Lex_is_typedef(0,_tmpC4))
return 369;
# 581
return 361;};}
# 584
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;
# 589
struct _dyneither_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 592
while(i < end){
int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){
;}
if(start == i){
struct Cyc_Core_Impossible_exn_struct _tmp198;const char*_tmp197;struct Cyc_Core_Impossible_exn_struct*_tmp196;(int)_throw((void*)((_tmp196=_cycalloc(sizeof(*_tmp196)),((_tmp196[0]=((_tmp198.tag=Cyc_Core_Impossible,((_tmp198.f1=((_tmp197="bad namespace",_tag_dyneither(_tmp197,sizeof(char),14))),_tmp198)))),_tmp196)))));}{
int vlen=i - start;
if(v != 0){
struct Cyc_List_List*_tmp199;rev_vs=((_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199->hd=v,((_tmp199->tl=rev_vs,_tmp199))))));}
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));
i +=2;};}
# 604
if(v == 0){
struct Cyc_Core_Impossible_exn_struct _tmp19F;const char*_tmp19E;struct Cyc_Core_Impossible_exn_struct*_tmp19D;(int)_throw((void*)((_tmp19D=_cycalloc(sizeof(*_tmp19D)),((_tmp19D[0]=((_tmp19F.tag=Cyc_Core_Impossible,((_tmp19F.f1=((_tmp19E="bad namespace",_tag_dyneither(_tmp19E,sizeof(char),14))),_tmp19F)))),_tmp19D)))));}{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 609
{const char*_tmp1A0;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1A0="Cyc",_tag_dyneither(_tmp1A0,sizeof(char),4))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1A1;Cyc_Lex_token_qvar=((_tmp1A1=_cycalloc(sizeof(*_tmp1A1)),((_tmp1A1->f1=Cyc_Absyn_Abs_n(vs,0),((_tmp1A1->f2=v,_tmp1A1))))));};}else{
# 613
const char*_tmp1A2;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1A2="C",_tag_dyneither(_tmp1A2,sizeof(char),2))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1A3;Cyc_Lex_token_qvar=((_tmp1A3=_cycalloc(sizeof(*_tmp1A3)),((_tmp1A3->f1=Cyc_Absyn_Abs_n(vs,1),((_tmp1A3->f2=v,_tmp1A3))))));};}else{
# 617
struct _tuple0*_tmp1A4;Cyc_Lex_token_qvar=((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4->f1=Cyc_Absyn_Rel_n(vs),((_tmp1A4->f2=v,_tmp1A4))))));}}}
if(Cyc_Lex_is_typedef(vs,v))
return 371;
return 370;};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
# 633
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_t;
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_opt_t;
# 636
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 638
static int Cyc_Lex_linenumber=1;
# 641
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _dyneither_ptr filename,unsigned int starting_line,struct Cyc_Lex_PosInfo*next){
# 645
unsigned int*_tmp1A6;unsigned int _tmp1A5;struct _dyneither_ptr linenumpos=(_tmp1A5=10,((_tmp1A6=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1A5),_tag_dyneither(_tmp1A6,sizeof(unsigned int),_tmp1A5))));
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;{
struct Cyc_Lex_PosInfo*_tmp1A7;return(_tmp1A7=_region_malloc(r,sizeof(*_tmp1A7)),((_tmp1A7->next=next,((_tmp1A7->starting_line=starting_line,((_tmp1A7->filename=(struct _dyneither_ptr)filename,((_tmp1A7->linenumpos=linenumpos,((_tmp1A7->linenumpos_offset=1,_tmp1A7)))))))))));};}
# 654
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0){struct Cyc_Core_Impossible_exn_struct _tmp1AD;const char*_tmp1AC;struct Cyc_Core_Impossible_exn_struct*_tmp1AB;(int)_throw((void*)((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB[0]=((_tmp1AD.tag=Cyc_Core_Impossible,((_tmp1AD.f1=((_tmp1AC="empty position info!",_tag_dyneither(_tmp1AC,sizeof(char),21))),_tmp1AD)))),_tmp1AB)))));}{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
_swap_dyneither(& p->linenumpos,& linenums);{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 662
if(offset >= n){
# 664
unsigned int*_tmp1AF;unsigned int _tmp1AE;struct _dyneither_ptr newlinenums=(_tmp1AE=n * 2,((_tmp1AF=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1AE),_tag_dyneither(_tmp1AF,sizeof(unsigned int),_tmp1AE))));
{unsigned int i=0;for(0;i < n;++ i){
*((unsigned int*)_check_dyneither_subscript(newlinenums,sizeof(unsigned int),(int)i))=((unsigned int*)linenums.curr)[(int)i];}}
_swap_dyneither(& linenums,& newlinenums);
# 669
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(newlinenums);}
# 672
*((unsigned int*)_check_dyneither_subscript(linenums,sizeof(unsigned int),(int)offset))=(unsigned int)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1;
++ Cyc_Lex_linenumber;
_swap_dyneither(& p->linenumpos,& linenums);};};}
# 679
static void Cyc_Lex_process_directive(struct _dyneither_ptr line){
int i;
char buf[100];
const char*_tmp1B0;struct _dyneither_ptr filename=(_tmp1B0="",_tag_dyneither(_tmp1B0,sizeof(char),1));
{const char*_tmp1B5;void*_tmp1B4[2];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1B3;struct Cyc_CharPtr_sa_ScanfArg_struct _tmp1B2;if(((_tmp1B2.tag=7,((_tmp1B2.f1=_tag_dyneither(buf,sizeof(char),100),((_tmp1B3.tag=2,((_tmp1B3.f1=& i,((_tmp1B4[0]=& _tmp1B3,((_tmp1B4[1]=& _tmp1B2,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1B5="# %d \"%s",_tag_dyneither(_tmp1B5,sizeof(char),9))),_tag_dyneither(_tmp1B4,sizeof(void*),2))))))))))))))== 2){
if(Cyc_Lex_compile_for_boot_r){
# 687
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100))- 1);
int last_slash=- 1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100,i)]== '/'){last_slash=i;break;}
-- i;}{
# 693
const char*_tmp1B9;void*_tmp1B8[1];struct Cyc_String_pa_PrintArg_struct _tmp1B7;filename=(struct _dyneither_ptr)((_tmp1B7.tag=0,((_tmp1B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(_tag_dyneither(buf,sizeof(char),100),sizeof(char),last_slash),sizeof(char),1)),((_tmp1B8[0]=& _tmp1B7,Cyc_aprintf(((_tmp1B9="\"%s",_tag_dyneither(_tmp1B9,sizeof(char),4))),_tag_dyneither(_tmp1B8,sizeof(void*),1))))))));};}else{
# 695
const char*_tmp1BD;void*_tmp1BC[1];struct Cyc_String_pa_PrintArg_struct _tmp1BB;filename=(struct _dyneither_ptr)((_tmp1BB.tag=0,((_tmp1BB.f1=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100),((_tmp1BC[0]=& _tmp1BB,Cyc_aprintf(((_tmp1BD="\"%s",_tag_dyneither(_tmp1BD,sizeof(char),4))),_tag_dyneither(_tmp1BC,sizeof(void*),1))))))));}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
const char*_tmp1C1;void*_tmp1C0[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1BF;if(((_tmp1BF.tag=2,((_tmp1BF.f1=& i,((_tmp1C0[0]=& _tmp1BF,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1C1="# %d",_tag_dyneither(_tmp1C1,sizeof(char),5))),_tag_dyneither(_tmp1C0,sizeof(void*),1))))))))== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 703
++ Cyc_Lex_linenumber;
return;}}}
# 706
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple20{struct _dyneither_ptr f1;unsigned int f2;};
# 713
struct _tuple20 Cyc_Lex_xlate_pos(unsigned int char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned int first_char_offset=*((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),0));
if(char_offset < first_char_offset  && p->next != 0)continue;{
# 719
unsigned int base=0;
unsigned int size=p->linenumpos_offset;
while(size > 1){
int half=(int)(size / 2);
int mid=(int)(base + half);
if(char_offset > *((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),mid))){
base=base + half;
size=size - half;}else{
# 728
size=(unsigned int)half;}}{
# 731
struct _tuple20 _tmp1C2;return(_tmp1C2.f1=p->filename,((_tmp1C2.f2=p->starting_line + base,_tmp1C2)));};};}}{
# 733
struct _tuple20 _tmp1C3;return(_tmp1C3.f1=_tag_dyneither(0,0,0),((_tmp1C3.f2=0,_tmp1C3)));};}
# 738
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 751
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 753
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
Cyc_yylloc.first_line=(yyllocptr->first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf));
Cyc_yylloc.last_line=(yyllocptr->last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf));
switch(ans){case 361: _LL2A:
# 758
{union Cyc_YYSTYPE _tmp1C4;*yylval=(((_tmp1C4.String_tok).val=Cyc_Lex_token_string,(((_tmp1C4.String_tok).tag=3,_tmp1C4))));}break;case 369: _LL2B:
# 760
{union Cyc_YYSTYPE _tmp1C5;*yylval=(((_tmp1C5.String_tok).val=Cyc_Lex_token_string,(((_tmp1C5.String_tok).tag=3,_tmp1C5))));}break;case 370: _LL2C:
# 762
{union Cyc_YYSTYPE _tmp1C6;*yylval=(((_tmp1C6.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1C6.QualId_tok).tag=5,_tmp1C6))));}break;case 371: _LL2D:
# 764
{union Cyc_YYSTYPE _tmp1C7;*yylval=(((_tmp1C7.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1C7.QualId_tok).tag=5,_tmp1C7))));}break;case 368: _LL2E:
# 766
{union Cyc_YYSTYPE _tmp1C8;*yylval=(((_tmp1C8.String_tok).val=Cyc_Lex_token_string,(((_tmp1C8.String_tok).tag=3,_tmp1C8))));}break;case 362: _LL2F:
# 768
{union Cyc_YYSTYPE _tmp1C9;*yylval=(((_tmp1C9.Int_tok).val=Cyc_Lex_token_int,(((_tmp1C9.Int_tok).tag=1,_tmp1C9))));}break;case 365: _LL30:
# 770
{union Cyc_YYSTYPE _tmp1CA;*yylval=(((_tmp1CA.Char_tok).val=Cyc_Lex_token_char,(((_tmp1CA.Char_tok).tag=2,_tmp1CA))));}break;case 366: _LL31:
# 772
{union Cyc_YYSTYPE _tmp1CB;*yylval=(((_tmp1CB.String_tok).val=Cyc_Lex_token_string,(((_tmp1CB.String_tok).tag=3,_tmp1CB))));}break;case 367: _LL32:
# 774
{union Cyc_YYSTYPE _tmp1CC;*yylval=(((_tmp1CC.String_tok).val=Cyc_Lex_token_string,(((_tmp1CC.String_tok).tag=3,_tmp1CC))));}break;case 363: _LL33:
# 776
{union Cyc_YYSTYPE _tmp1CD;*yylval=(((_tmp1CD.String_tok).val=Cyc_Lex_token_string,(((_tmp1CD.String_tok).tag=3,_tmp1CD))));}break;case 364: _LL34:
# 778
{union Cyc_YYSTYPE _tmp1CE;*yylval=(((_tmp1CE.String_tok).val=Cyc_Lex_token_string,(((_tmp1CE.String_tok).tag=3,_tmp1CE))));}break;case 373: _LL35:
# 780
{union Cyc_YYSTYPE _tmp1CF;*yylval=(((_tmp1CF.Asm_tok).val=Cyc_Lex_token_asm,(((_tmp1CF.Asm_tok).tag=6,_tmp1CF))));}break;default: _LL36:
 break;}
# 783
return ans;}
# 790
const int Cyc_Lex_lex_base[287]={0,24,117,119,123,132,134,139,150,157,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,163,- 1,12,23,38,21,34,35,47,201,174,10,12,21,- 15,- 16,11,- 1,195,- 17,- 11,- 10,215,- 12,- 9,- 4,- 5,268,349,223,- 13,- 16,- 3,- 2,12,2,- 16,- 17,13,- 1,373,- 18,14,291,446,469,299,- 14,15,- 1,16,524,307,597,672,389,- 23,30,57,54,54,81,57,52,58,61,78,66,74,747,85,84,83,822,91,95,99,897,102,95,100,86,109,109,110,118,972,109,112,1047,108,119,115,124,123,116,120,1122,139,139,130,138,1197,130,141,137,141,137,153,141,148,1272,142,152,171,1347,166,1422,167,183,175,171,183,191,182,190,1497,- 62,295,- 41,17,30,- 44,285,31,265,353,32,251,269,406,330,1574,1617,238,240,247,267,- 7,1687,1776,249,1851,1909,1984,310,- 61,- 25,- 31,2059,274,2134,2209,2267,2342,2417,306,2492,2567,2642,2717,2792,2867,2942,3017,- 26,- 45,- 46,313,- 29,315,- 34,- 28,- 37,320,- 38,3092,3121,540,347,359,550,3131,3161,1655,1584,3194,3225,3263,368,396,3333,3371,388,398,391,401,393,451,- 9,- 43,371,- 42,18,334,3303,- 36,- 19,- 21,- 35,- 20,- 22,1,3411,2,343,501,346,348,350,354,355,357,369,370,371,3484,3568,- 59,- 53,- 52,- 51,- 50,- 49,- 48,- 47,- 54,- 57,- 58,1603,372,- 55,- 56,- 60,- 30,- 27,- 24,551,- 39,19,- 15,- 41,599};
const int Cyc_Lex_lex_backtrk[287]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,- 1,- 1,14,- 1,17,- 1,- 1,- 1,12,- 1,- 1,- 1,- 1,- 1,13,12,- 1,- 1,- 1,- 1,1,3,- 1,- 1,15,- 1,18,- 1,1,13,- 1,14,13,- 1,1,- 1,14,17,12,- 1,13,12,- 1,22,22,22,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,12,- 1,- 1,- 1,18,- 1,- 1,- 1,11,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,- 1,- 1,- 1,20,- 1,19,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,39,- 1,40,61,- 1,61,61,61,61,61,61,61,61,61,8,10,61,61,61,61,- 1,2,2,61,61,61,2,61,- 1,- 1,- 1,2,- 1,2,0,- 1,4,4,- 1,5,3,3,3,3,3,3,1,- 1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,- 1,11,12,- 1,- 1,12,8,9,8,8,- 1,7,7,7,9,9,8,8,- 1,- 1,- 1,- 1,41,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,39};
const int Cyc_Lex_lex_default[287]={151,80,38,49,60,49,38,34,- 1,10,0,0,- 1,0,20,0,0,- 1,0,0,20,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,34,- 1,- 1,- 1,0,0,- 1,0,- 1,0,0,0,- 1,0,0,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,0,281,- 1,- 1,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,235,0,- 1,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,281,0,- 1,0,0,- 1};
const int Cyc_Lex_lex_trans[3825]={0,0,0,0,0,0,0,0,0,152,153,152,152,154,23,21,21,21,22,22,18,55,19,43,19,19,55,285,236,282,0,0,152,155,156,157,25,158,159,160,277,276,161,162,19,163,164,165,166,167,167,167,167,167,167,167,167,167,168,19,169,170,171,25,172,173,173,173,173,173,173,173,173,173,173,173,174,173,173,173,173,173,173,173,173,173,173,173,173,173,173,284,280,245,175,176,177,178,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,27,179,81,82,39,56,57,74,28,72,61,83,29,62,30,84,31,85,56,57,32,39,58,86,40,33,18,142,56,35,73,140,102,41,63,87,24,24,24,24,24,56,114,88,11,41,89,12,115,24,24,24,24,24,90,94,91,92,36,24,93,- 1,95,37,- 1,98,96,25,13,14,97,99,24,100,15,16,101,111,105,106,25,17,103,107,108,59,75,33,33,33,33,33,64,- 1,104,109,110,112,- 1,113,122,127,118,42,116,117,44,128,119,120,33,45,121,123,124,125,126,136,19,129,46,46,46,46,46,46,46,46,130,131,132,133,134,180,135,47,137,138,- 1,- 1,53,53,53,53,53,53,53,53,54,54,54,54,54,54,54,54,139,- 1,26,141,143,144,145,146,48,147,148,149,150,19,21,65,282,207,49,283,208,204,55,205,278,286,50,286,286,71,18,199,23,244,10,241,51,52,52,52,52,52,52,52,52,52,52,279,286,43,202,242,243,187,52,52,52,52,52,52,70,70,70,70,70,70,70,70,71,71,71,71,71,71,71,71,79,79,79,79,79,79,79,79,- 1,187,52,52,52,52,52,52,181,234,43,203,- 1,206,235,209,65,240,236,275,19,237,271,66,270,- 1,269,43,80,- 1,268,267,21,266,52,52,52,52,52,52,52,52,52,52,47,265,264,263,272,44,18,52,52,52,52,52,52,0,67,67,67,67,67,67,67,67,0,- 1,0,44,0,182,44,54,54,54,54,54,54,54,54,54,247,52,52,52,52,52,52,238,48,239,239,239,239,239,239,239,239,239,239,44,45,0,44,0,0,21,49,48,10,10,50,45,45,233,0,0,0,0,18,0,48,0,23,0,10,0,48,0,68,69,69,69,69,69,69,69,69,69,69,48,10,10,0,45,45,233,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,233,0,0,0,0,0,0,69,69,69,69,69,69,272,- 1,0,69,69,69,69,69,69,273,273,273,273,273,273,273,273,0,44,233,0,282,0,45,283,0,69,69,69,69,69,69,76,76,76,76,76,76,76,76,0,0,0,215,0,215,0,47,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,286,- 1,286,286,0,0,0,0,48,0,0,0,0,19,21,0,0,0,49,- 1,0,0,0,286,0,0,50,0,0,0,18,0,23,0,10,0,77,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,19,19,19,19,19,19,19,19,19,19,0,0,- 1,0,0,0,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,10,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,25,25,25,25,25,25,25,25,25,25,0,0,0,0,0,0,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,0,0,0,0,25,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,50,50,50,50,50,50,50,50,50,50,0,0,0,0,0,0,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,0,0,0,0,50,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,18,18,18,18,18,18,18,18,18,18,0,0,0,0,0,0,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,0,0,0,0,18,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,0,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,0,0,0,0,23,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,44,44,44,44,44,44,44,44,44,44,0,0,0,0,0,0,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,0,0,0,0,44,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,21,21,21,21,21,21,21,21,21,21,0,0,0,0,0,0,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,0,0,0,0,21,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,45,45,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,0,0,0,0,45,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,48,48,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,0,0,0,0,48,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49,49,49,0,0,0,0,0,0,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,0,0,0,0,49,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,210,0,221,221,221,221,221,221,221,221,222,222,220,220,220,220,220,220,220,220,220,220,272,212,0,0,0,0,0,0,223,274,274,274,274,274,274,274,274,224,0,0,225,210,0,211,211,211,211,211,211,211,211,211,211,212,0,0,0,0,0,0,223,0,0,0,212,0,0,0,0,224,0,213,225,0,0,0,219,0,219,0,214,220,220,220,220,220,220,220,220,220,220,0,0,0,0,0,212,0,0,0,0,0,0,213,0,0,0,0,0,0,0,0,214,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,200,0,0,0,0,201,0,0,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,193,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,0,0,0,0,191,0,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,185,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,186,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,0,0,0,0,0,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,0,0,0,0,183,0,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,189,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,190,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,189,189,189,189,189,189,189,189,189,0,0,0,0,0,0,0,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,0,0,0,0,189,0,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,191,191,191,191,191,191,191,191,191,191,0,0,0,0,0,0,0,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,0,0,0,0,191,0,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,192,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,192,0,194,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,192,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,195,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,192,0,192,192,192,192,192,192,192,192,192,192,192,192,196,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,197,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,198,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,0,0,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,0,0,0,0,192,0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,217,217,217,217,217,217,217,217,217,217,0,0,0,0,0,0,0,0,0,0,0,218,54,0,0,0,0,210,54,211,211,211,211,211,211,211,211,211,211,216,216,216,216,216,216,216,216,216,216,0,212,0,0,218,54,0,0,213,0,0,54,47,0,0,0,0,214,47,0,217,217,217,217,217,217,217,217,217,217,0,0,0,212,0,0,0,0,0,0,213,218,54,0,47,0,0,0,54,214,47,0,0,220,220,220,220,220,220,220,220,220,220,0,0,0,0,0,0,0,0,0,0,218,54,54,0,0,0,0,54,54,210,0,221,221,221,221,221,221,221,221,222,222,0,0,0,0,0,0,0,0,0,0,0,212,0,54,0,0,0,0,231,54,0,0,0,0,0,0,210,232,222,222,222,222,222,222,222,222,222,222,0,0,0,0,0,212,0,0,0,0,0,212,231,0,0,0,0,0,229,0,0,232,0,0,0,0,0,230,0,0,239,239,239,239,239,239,239,239,239,239,0,0,0,212,0,0,0,0,0,0,229,218,54,0,0,0,0,0,54,230,226,226,226,226,226,226,226,226,226,226,0,0,0,0,0,0,0,226,226,226,226,226,226,218,54,0,0,0,0,0,54,0,0,0,0,0,0,0,226,226,226,226,226,226,226,226,226,226,0,226,226,226,226,226,226,226,226,226,226,226,226,0,0,0,248,0,227,0,0,249,0,0,0,0,0,228,0,0,250,250,250,250,250,250,250,250,0,226,226,226,226,226,226,251,0,0,0,0,227,0,0,0,0,0,0,0,0,228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,0,0,0,0,253,254,0,0,0,255,0,0,0,0,0,0,0,256,0,0,0,257,0,258,0,259,0,260,261,261,261,261,261,261,261,261,261,261,0,0,0,0,0,0,0,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,0,0,0,0,0,0,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,262,0,0,0,0,0,0,0,0,261,261,261,261,261,261,261,261,261,261,0,0,0,0,0,0,0,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,0,0,0,0,0,0,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3825]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,12,14,20,22,14,20,35,40,58,62,66,72,74,154,237,283,- 1,- 1,0,0,0,0,59,0,0,0,246,248,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,155,158,161,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,1,1,2,3,3,2,27,3,4,1,28,4,29,1,30,1,5,5,31,6,5,1,6,32,7,81,3,7,3,82,84,2,4,86,8,8,8,8,8,5,83,87,9,6,88,9,83,24,24,24,24,24,89,85,90,91,7,8,92,34,85,7,34,94,95,8,9,9,96,98,24,99,9,9,100,103,104,105,24,9,102,106,107,5,2,33,33,33,33,33,4,34,102,108,109,111,34,112,116,114,117,6,115,115,42,114,118,119,33,42,120,122,123,124,125,127,33,128,42,42,42,42,42,42,42,42,129,130,131,132,133,0,134,42,136,137,14,20,46,46,46,46,46,46,46,46,53,53,53,53,53,53,53,53,138,1,24,140,142,143,144,145,42,146,147,148,149,42,42,162,157,168,42,157,168,169,169,169,159,152,42,152,152,170,42,175,42,162,42,163,42,51,51,51,51,51,51,51,51,51,51,159,152,171,171,163,163,184,51,51,51,51,51,51,67,67,67,67,67,67,67,67,70,70,70,70,70,70,70,70,76,76,76,76,76,76,76,76,160,190,51,51,51,51,51,51,179,165,2,202,3,204,165,208,4,238,235,249,64,235,251,64,252,5,253,6,165,160,254,255,7,256,52,52,52,52,52,52,52,52,52,52,64,257,258,259,274,64,9,52,52,52,52,52,52,- 1,64,64,64,64,64,64,64,64,- 1,34,- 1,213,- 1,179,214,64,79,79,79,79,79,79,79,79,160,52,52,52,52,52,52,164,223,164,164,164,164,164,164,164,164,164,164,213,64,- 1,214,- 1,- 1,64,64,224,227,228,64,229,230,231,- 1,- 1,- 1,- 1,64,- 1,223,- 1,64,- 1,64,- 1,64,- 1,64,68,68,68,68,68,68,68,68,68,68,224,227,228,- 1,229,230,231,68,68,68,68,68,68,69,69,69,69,69,69,69,69,69,69,232,- 1,- 1,- 1,- 1,- 1,- 1,69,69,69,69,69,69,250,157,- 1,68,68,68,68,68,68,250,250,250,250,250,250,250,250,- 1,75,232,- 1,281,- 1,75,281,- 1,69,69,69,69,69,69,75,75,75,75,75,75,75,75,- 1,- 1,- 1,212,- 1,212,- 1,75,212,212,212,212,212,212,212,212,212,212,215,215,215,215,215,215,215,215,215,215,286,160,286,286,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,75,75,- 1,- 1,- 1,75,235,- 1,- 1,- 1,286,- 1,- 1,75,- 1,- 1,- 1,75,- 1,75,- 1,75,- 1,75,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,93,93,93,93,93,93,93,93,93,93,- 1,- 1,281,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,121,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,126,- 1,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,135,135,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,135,- 1,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,150,150,150,150,150,150,150,150,150,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,- 1,- 1,- 1,- 1,150,- 1,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,166,- 1,166,166,166,166,166,166,166,166,166,166,219,219,219,219,219,219,219,219,219,219,273,166,- 1,- 1,- 1,- 1,- 1,- 1,166,273,273,273,273,273,273,273,273,166,- 1,- 1,166,167,- 1,167,167,167,167,167,167,167,167,167,167,166,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,- 1,- 1,167,- 1,- 1,- 1,- 1,166,- 1,167,166,- 1,- 1,- 1,218,- 1,218,- 1,167,218,218,218,218,218,218,218,218,218,218,- 1,- 1,- 1,- 1,- 1,167,- 1,- 1,- 1,- 1,- 1,- 1,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,173,173,173,173,173,173,173,173,173,173,173,- 1,- 1,- 1,- 1,- 1,- 1,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,- 1,- 1,- 1,- 1,173,- 1,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,174,- 1,- 1,- 1,- 1,174,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,174,174,174,174,174,174,174,174,174,174,174,- 1,- 1,- 1,- 1,- 1,- 1,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,- 1,- 1,- 1,- 1,174,- 1,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,176,- 1,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,- 1,- 1,- 1,- 1,177,- 1,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,178,178,178,178,178,178,178,178,178,178,178,- 1,- 1,- 1,- 1,- 1,- 1,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,- 1,- 1,- 1,- 1,178,- 1,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,178,183,183,183,183,183,183,183,183,183,183,183,- 1,- 1,- 1,- 1,- 1,- 1,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,- 1,- 1,- 1,- 1,183,- 1,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,- 1,- 1,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,185,- 1,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,186,186,186,186,186,186,186,186,186,186,186,- 1,- 1,- 1,- 1,- 1,- 1,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,- 1,- 1,- 1,- 1,186,- 1,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,187,- 1,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,188,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,- 1,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,188,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,189,189,189,189,189,189,189,189,189,- 1,- 1,- 1,- 1,- 1,- 1,- 1,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,- 1,- 1,- 1,- 1,189,- 1,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,191,191,191,191,191,191,191,191,191,191,- 1,- 1,- 1,- 1,- 1,- 1,- 1,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,- 1,- 1,- 1,- 1,191,- 1,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,192,- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,- 1,- 1,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,193,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,194,194,194,194,194,194,194,194,194,194,- 1,- 1,- 1,- 1,- 1,- 1,- 1,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,- 1,- 1,- 1,- 1,194,- 1,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,195,195,195,195,195,195,195,195,195,195,- 1,- 1,- 1,- 1,- 1,- 1,- 1,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,- 1,- 1,- 1,- 1,195,- 1,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,- 1,- 1,- 1,- 1,- 1,- 1,- 1,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,- 1,- 1,- 1,- 1,196,- 1,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,- 1,- 1,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,197,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,198,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,210,210,- 1,- 1,- 1,- 1,211,210,211,211,211,211,211,211,211,211,211,211,216,216,216,216,216,216,216,216,216,216,- 1,211,- 1,- 1,210,210,- 1,- 1,211,- 1,- 1,210,216,- 1,- 1,- 1,- 1,211,216,- 1,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,211,- 1,- 1,- 1,- 1,- 1,- 1,211,217,217,- 1,216,- 1,- 1,- 1,217,211,216,- 1,- 1,220,220,220,220,220,220,220,220,220,220,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,220,- 1,- 1,- 1,- 1,217,220,221,- 1,221,221,221,221,221,221,221,221,221,221,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,221,- 1,220,- 1,- 1,- 1,- 1,221,220,- 1,- 1,- 1,- 1,- 1,- 1,222,221,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,- 1,221,- 1,- 1,- 1,- 1,- 1,222,221,- 1,- 1,- 1,- 1,- 1,222,- 1,- 1,221,- 1,- 1,- 1,- 1,- 1,222,- 1,- 1,239,239,239,239,239,239,239,239,239,239,- 1,- 1,- 1,222,- 1,- 1,- 1,- 1,- 1,- 1,222,239,239,- 1,- 1,- 1,- 1,- 1,239,222,225,225,225,225,225,225,225,225,225,225,- 1,- 1,- 1,- 1,- 1,- 1,- 1,225,225,225,225,225,225,239,239,- 1,- 1,- 1,- 1,- 1,239,- 1,- 1,- 1,- 1,- 1,- 1,- 1,226,226,226,226,226,226,226,226,226,226,- 1,225,225,225,225,225,225,226,226,226,226,226,226,- 1,- 1,- 1,247,- 1,226,- 1,- 1,247,- 1,- 1,- 1,- 1,- 1,226,- 1,- 1,247,247,247,247,247,247,247,247,- 1,226,226,226,226,226,226,247,- 1,- 1,- 1,- 1,226,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,226,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,247,- 1,- 1,- 1,- 1,247,247,- 1,- 1,- 1,247,- 1,- 1,- 1,- 1,- 1,- 1,- 1,247,- 1,- 1,- 1,247,- 1,247,- 1,247,- 1,247,260,260,260,260,260,260,260,260,260,260,- 1,- 1,- 1,- 1,- 1,- 1,- 1,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,- 1,- 1,- 1,- 1,- 1,- 1,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,261,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,261,261,261,261,261,261,261,261,261,261,- 1,- 1,- 1,- 1,- 1,- 1,- 1,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,- 1,- 1,- 1,- 1,- 1,- 1,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 797
int state;int base;int backtrk;
int c;
state=start_state;
# 801
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 805
state=(- state)- 1;}
# 807
while(1){
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(287,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(287,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 815
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 819
c=256;}}else{
# 821
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 824
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(3825,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(3825,base + c)];else{
# 827
state=Cyc_Lex_lex_default[_check_known_subscript_notnull(287,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp1D5;const char*_tmp1D4;struct Cyc_Lexing_Error_exn_struct*_tmp1D3;(int)_throw((void*)((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3[0]=((_tmp1D5.tag=Cyc_Lexing_Error,((_tmp1D5.f1=((_tmp1D4="empty token",_tag_dyneither(_tmp1D4,sizeof(char),12))),_tmp1D5)))),_tmp1D3)))));}else{
# 833
return lbuf->lex_last_action;}}else{
# 836
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 840
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL38:
# 793 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL39:
# 794 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 2: _LL3A:
# 797 "lex.cyl"
 return(int)Cyc_Lex_process_id(lexbuf);case 3: _LL3B:
# 799
 return(int)Cyc_Lex_process_id(lexbuf);case 4: _LL3C:
# 803 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 5: _LL3D:
# 806 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 368;case 6: _LL3E:
# 809
 return Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3F:
# 812 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 362;case 8: _LL40:
# 815
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 362;case 9: _LL41:
# 821 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 362;case 10: _LL42:
# 824
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 362;case 11: _LL43:
# 828 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 367;case 12: _LL44:
# 832 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 367;case 13: _LL45:
# 835
 return 344;case 14: _LL46:
# 836 "lex.cyl"
 return 345;case 15: _LL47:
# 837 "lex.cyl"
 return 342;case 16: _LL48:
# 838 "lex.cyl"
 return 343;case 17: _LL49:
# 839 "lex.cyl"
 return 338;case 18: _LL4A:
# 840 "lex.cyl"
 return 339;case 19: _LL4B:
# 841 "lex.cyl"
 return 351;case 20: _LL4C:
# 842 "lex.cyl"
 return 352;case 21: _LL4D:
# 843 "lex.cyl"
 return 348;case 22: _LL4E:
# 844 "lex.cyl"
 return 349;case 23: _LL4F:
# 845 "lex.cyl"
 return 350;case 24: _LL50:
# 846 "lex.cyl"
 return 357;case 25: _LL51:
# 847 "lex.cyl"
 return 356;case 26: _LL52:
# 848 "lex.cyl"
 return 355;case 27: _LL53:
# 849 "lex.cyl"
 return 353;case 28: _LL54:
# 850 "lex.cyl"
 return 354;case 29: _LL55:
# 851 "lex.cyl"
 return 346;case 30: _LL56:
# 852 "lex.cyl"
 return 347;case 31: _LL57:
# 853 "lex.cyl"
 return 340;case 32: _LL58:
# 855
 return 341;case 33: _LL59:
# 856 "lex.cyl"
 return 359;case 34: _LL5A:
# 857 "lex.cyl"
 return 337;case 35: _LL5B:
# 858 "lex.cyl"
 return 358;case 36: _LL5C:
# 859 "lex.cyl"
 return 360;case 37: _LL5D:
# 861
 return 311;case 38: _LL5E:
# 864 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 39: _LL5F:
# 865 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 40: _LL60:
# 866 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 41: _LL61:
# 867 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 42: _LL62:
# 868 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 43: _LL63:
# 874 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 363;case 44: _LL64:
# 882 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 364;case 45: _LL65:
# 890 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 366;case 46: _LL66:
# 897
 Cyc_Lex_token_char='\a';return 365;case 47: _LL67:
# 898 "lex.cyl"
 Cyc_Lex_token_char='\b';return 365;case 48: _LL68:
# 899 "lex.cyl"
 Cyc_Lex_token_char='\f';return 365;case 49: _LL69:
# 900 "lex.cyl"
 Cyc_Lex_token_char='\n';return 365;case 50: _LL6A:
# 901 "lex.cyl"
 Cyc_Lex_token_char='\r';return 365;case 51: _LL6B:
# 902 "lex.cyl"
 Cyc_Lex_token_char='\t';return 365;case 52: _LL6C:
# 903 "lex.cyl"
 Cyc_Lex_token_char='\v';return 365;case 53: _LL6D:
# 904 "lex.cyl"
 Cyc_Lex_token_char='\\';return 365;case 54: _LL6E:
# 905 "lex.cyl"
 Cyc_Lex_token_char='\'';return 365;case 55: _LL6F:
# 906 "lex.cyl"
 Cyc_Lex_token_char='"';return 365;case 56: _LL70:
# 907 "lex.cyl"
 Cyc_Lex_token_char='?';return 365;case 57: _LL71:
# 910 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;
return 365;case 58: _LL72:
# 915 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;
return 365;case 59: _LL73:
# 919
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 365;case 60: _LL74:
# 923
 return - 1;case 61: _LL75:
# 925
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL76:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}{
# 929
struct Cyc_Lexing_Error_exn_struct _tmp1DB;const char*_tmp1DA;struct Cyc_Lexing_Error_exn_struct*_tmp1D9;(int)_throw((void*)((_tmp1D9=_cycalloc(sizeof(*_tmp1D9)),((_tmp1D9[0]=((_tmp1DB.tag=Cyc_Lexing_Error,((_tmp1DB.f1=((_tmp1DA="some action didn't return!",_tag_dyneither(_tmp1DA,sizeof(char),27))),_tmp1DB)))),_tmp1D9)))));};}
# 931
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_cyc_qual_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL78:
# 932 "lex.cyl"
{const char*_tmp1DC;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1DC="taggedZ",_tag_dyneither(_tmp1DC,sizeof(char),8))));}return(int)'@';case 1: _LL79:
# 934
{const char*_tmp1DD;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1DD="zerotermZ",_tag_dyneither(_tmp1DD,sizeof(char),10))));}return(int)'@';case 2: _LL7A:
# 936
{const char*_tmp1DE;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1DE="nozerotermZ",_tag_dyneither(_tmp1DE,sizeof(char),12))));}return(int)'@';case 3: _LL7B:
# 938
{const char*_tmp1DF;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1DF="extensibleZ",_tag_dyneither(_tmp1DF,sizeof(char),12))));}return(int)'@';case 4: _LL7C:
# 940
{const char*_tmp1E0;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E0="resetableZ",_tag_dyneither(_tmp1E0,sizeof(char),11))));}return(int)'@';case 5: _LL7D:
# 942
{const char*_tmp1E1;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E1="regionZ",_tag_dyneither(_tmp1E1,sizeof(char),8))));}return(int)'@';case 6: _LL7E:
# 944
{const char*_tmp1E2;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E2="numeltsZ",_tag_dyneither(_tmp1E2,sizeof(char),9))));}return(int)'@';case 7: _LL7F:
# 946
{const char*_tmp1E3;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E3="thinZ",_tag_dyneither(_tmp1E3,sizeof(char),6))));}return(int)'@';case 8: _LL80:
# 948
{const char*_tmp1E4;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E4="fatZ",_tag_dyneither(_tmp1E4,sizeof(char),5))));}return(int)'@';case 9: _LL81:
# 950
{const char*_tmp1E5;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E5="notnullZ",_tag_dyneither(_tmp1E5,sizeof(char),9))));}return(int)'@';case 10: _LL82:
# 952
{const char*_tmp1E6;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1E6="nullableZ",_tag_dyneither(_tmp1E6,sizeof(char),10))));}return(int)'@';case 11: _LL83:
# 953 "lex.cyl"
 return 334;case 12: _LL84:
# 954 "lex.cyl"
 return 319;case 13: _LL85:
# 955 "lex.cyl"
 return 318;case 14: _LL86:
# 956 "lex.cyl"
 return 335;case 15: _LL87:
# 957 "lex.cyl"
 return 336;case 16: _LL88:
# 958 "lex.cyl"
 return 320;case 17: _LL89:
# 959 "lex.cyl"
 return 329;case 18: _LL8A:
# 960 "lex.cyl"
 return 330;case 19: _LL8B:
# 961 "lex.cyl"
 return 331;case 20: _LL8C:
# 962 "lex.cyl"
 return 332;case 21: _LL8D:
# 963 "lex.cyl"
 return 333;case 22: _LL8E:
# 964 "lex.cyl"
 lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL8F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}{
# 968
struct Cyc_Lexing_Error_exn_struct _tmp1EC;const char*_tmp1EB;struct Cyc_Lexing_Error_exn_struct*_tmp1EA;(int)_throw((void*)((_tmp1EA=_cycalloc(sizeof(*_tmp1EA)),((_tmp1EA[0]=((_tmp1EC.tag=Cyc_Lexing_Error,((_tmp1EC.f1=((_tmp1EB="some action didn't return!",_tag_dyneither(_tmp1EB,sizeof(char),27))),_tmp1EC)))),_tmp1EA)))));};}
# 970
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL91:
# 967 "lex.cyl"
 return 0;case 1: _LL92:
# 969
 Cyc_Lex_store_string_char('\a');return 1;case 2: _LL93:
# 970 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3: _LL94:
# 971 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4: _LL95:
# 972 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5: _LL96:
# 973 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6: _LL97:
# 974 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7: _LL98:
# 975 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8: _LL99:
# 976 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9: _LL9A:
# 977 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10: _LL9B:
# 978 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11: _LL9C:
# 979 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12: _LL9D:
# 982 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13: _LL9E:
# 985 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LL9F:
# 987
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15: _LLA0:
# 989
 Cyc_Lex_inc_linenumber();{const char*_tmp1ED;Cyc_Lex_runaway_err(((_tmp1ED="wide character ends in newline",_tag_dyneither(_tmp1ED,sizeof(char),31))),lexbuf);}return 0;case 16: _LLA1:
# 990 "lex.cyl"
{const char*_tmp1EE;Cyc_Lex_runaway_err(((_tmp1EE="unterminated wide character",_tag_dyneither(_tmp1EE,sizeof(char),28))),lexbuf);}return 0;case 17: _LLA2:
# 991 "lex.cyl"
{const char*_tmp1EF;Cyc_Lex_err(((_tmp1EF="bad character following backslash in wide character",_tag_dyneither(_tmp1EF,sizeof(char),52))),lexbuf);}return 1;default: _LLA3:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}{
# 995
struct Cyc_Lexing_Error_exn_struct _tmp1F5;const char*_tmp1F4;struct Cyc_Lexing_Error_exn_struct*_tmp1F3;(int)_throw((void*)((_tmp1F3=_cycalloc(sizeof(*_tmp1F3)),((_tmp1F3[0]=((_tmp1F5.tag=Cyc_Lexing_Error,((_tmp1F5.f1=((_tmp1F4="some action didn't return!",_tag_dyneither(_tmp1F4,sizeof(char),27))),_tmp1F5)))),_tmp1F3)))));};}
# 997
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,2);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA5:
# 996 "lex.cyl"
 return 1;case 1: _LLA6:
# 997 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2: _LLA7:
# 998 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3: _LLA8:
# 1000
 lexbuf->lex_curr_pos -=1;return 0;default: _LLA9:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}{
# 1004
struct Cyc_Lexing_Error_exn_struct _tmp1FB;const char*_tmp1FA;struct Cyc_Lexing_Error_exn_struct*_tmp1F9;(int)_throw((void*)((_tmp1F9=_cycalloc(sizeof(*_tmp1F9)),((_tmp1F9[0]=((_tmp1FB.tag=Cyc_Lexing_Error,((_tmp1FB.f1=((_tmp1FA="some action didn't return!",_tag_dyneither(_tmp1FA,sizeof(char),27))),_tmp1FB)))),_tmp1F9)))));};}
# 1006
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,3);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAB:
# 1004 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1: _LLAC:
# 1005 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2: _LLAD:
# 1006 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3: _LLAE:
# 1007 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4: _LLAF:
# 1008 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5: _LLB0:
# 1009 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 6: _LLB1:
# 1010 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7: _LLB2:
# 1011 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 8: _LLB3:
# 1012 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9: _LLB4:
# 1013 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10: _LLB5:
# 1014 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11: _LLB6:
# 1015 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12: _LLB7:
# 1016 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13: _LLB8:
# 1019 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);
return 1;case 14: _LLB9:
# 1024 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);
return 1;case 15: _LLBA:
# 1029
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 16: _LLBB:
# 1031 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp1FC;Cyc_Lex_runaway_err(((_tmp1FC="string ends in newline",_tag_dyneither(_tmp1FC,sizeof(char),23))),lexbuf);}
return 0;case 17: _LLBC:
# 1035 "lex.cyl"
{const char*_tmp1FD;Cyc_Lex_runaway_err(((_tmp1FD="unterminated string",_tag_dyneither(_tmp1FD,sizeof(char),20))),lexbuf);}
return 0;case 18: _LLBD:
# 1038 "lex.cyl"
{const char*_tmp1FE;Cyc_Lex_err(((_tmp1FE="bad character following backslash in string",_tag_dyneither(_tmp1FE,sizeof(char),44))),lexbuf);}
return 1;default: _LLBE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}{
# 1043
struct Cyc_Lexing_Error_exn_struct _tmp204;const char*_tmp203;struct Cyc_Lexing_Error_exn_struct*_tmp202;(int)_throw((void*)((_tmp202=_cycalloc(sizeof(*_tmp202)),((_tmp202[0]=((_tmp204.tag=Cyc_Lexing_Error,((_tmp204.f1=((_tmp203="some action didn't return!",_tag_dyneither(_tmp203,sizeof(char),27))),_tmp204)))),_tmp202)))));};}
# 1045
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,4);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC0:
# 1048 "lex.cyl"
{const char*_tmp205;Cyc_Lex_store_string(((_tmp205="\" L\"",_tag_dyneither(_tmp205,sizeof(char),5))));}return 1;case 1: _LLC1:
# 1049 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2: _LLC2:
# 1050 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3: _LLC3:
# 1052
 lexbuf->lex_curr_pos -=1;return 0;default: _LLC4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}{
# 1056
struct Cyc_Lexing_Error_exn_struct _tmp20B;const char*_tmp20A;struct Cyc_Lexing_Error_exn_struct*_tmp209;(int)_throw((void*)((_tmp209=_cycalloc(sizeof(*_tmp209)),((_tmp209[0]=((_tmp20B.tag=Cyc_Lexing_Error,((_tmp20B.f1=((_tmp20A="some action didn't return!",_tag_dyneither(_tmp20A,sizeof(char),27))),_tmp20B)))),_tmp209)))));};}
# 1058
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,5);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC6:
# 1055 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1: _LLC7:
# 1056 "lex.cyl"
{const char*_tmp20C;Cyc_Lex_store_string(((_tmp20C="\\a",_tag_dyneither(_tmp20C,sizeof(char),3))));}return 1;case 2: _LLC8:
# 1057 "lex.cyl"
{const char*_tmp20D;Cyc_Lex_store_string(((_tmp20D="\\b",_tag_dyneither(_tmp20D,sizeof(char),3))));}return 1;case 3: _LLC9:
# 1058 "lex.cyl"
{const char*_tmp20E;Cyc_Lex_store_string(((_tmp20E="\\f",_tag_dyneither(_tmp20E,sizeof(char),3))));}return 1;case 4: _LLCA:
# 1059 "lex.cyl"
{const char*_tmp20F;Cyc_Lex_store_string(((_tmp20F="\\n",_tag_dyneither(_tmp20F,sizeof(char),3))));}return 1;case 5: _LLCB:
# 1060 "lex.cyl"
{const char*_tmp210;Cyc_Lex_store_string(((_tmp210="\\r",_tag_dyneither(_tmp210,sizeof(char),3))));}return 1;case 6: _LLCC:
# 1061 "lex.cyl"
{const char*_tmp211;Cyc_Lex_store_string(((_tmp211="\\t",_tag_dyneither(_tmp211,sizeof(char),3))));}return 1;case 7: _LLCD:
# 1062 "lex.cyl"
{const char*_tmp212;Cyc_Lex_store_string(((_tmp212="\\v",_tag_dyneither(_tmp212,sizeof(char),3))));}return 1;case 8: _LLCE:
# 1063 "lex.cyl"
{const char*_tmp213;Cyc_Lex_store_string(((_tmp213="\\\\",_tag_dyneither(_tmp213,sizeof(char),3))));}return 1;case 9: _LLCF:
# 1064 "lex.cyl"
{const char*_tmp214;Cyc_Lex_store_string(((_tmp214="\\'",_tag_dyneither(_tmp214,sizeof(char),3))));}return 1;case 10: _LLD0:
# 1065 "lex.cyl"
{const char*_tmp215;Cyc_Lex_store_string(((_tmp215="\\\"",_tag_dyneither(_tmp215,sizeof(char),3))));}return 1;case 11: _LLD1:
# 1066 "lex.cyl"
{const char*_tmp216;Cyc_Lex_store_string(((_tmp216="\\?",_tag_dyneither(_tmp216,sizeof(char),3))));}return 1;case 12: _LLD2:
# 1069 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 13: _LLD3:
# 1074 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 14: _LLD4:
# 1079
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 15: _LLD5:
# 1081 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp217;Cyc_Lex_runaway_err(((_tmp217="string ends in newline",_tag_dyneither(_tmp217,sizeof(char),23))),lexbuf);}
return 0;case 16: _LLD6:
# 1085 "lex.cyl"
{const char*_tmp218;Cyc_Lex_runaway_err(((_tmp218="unterminated string",_tag_dyneither(_tmp218,sizeof(char),20))),lexbuf);}
return 0;case 17: _LLD7:
# 1088 "lex.cyl"
{const char*_tmp219;Cyc_Lex_err(((_tmp219="bad character following backslash in string",_tag_dyneither(_tmp219,sizeof(char),44))),lexbuf);}
return 1;default: _LLD8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}{
# 1093
struct Cyc_Lexing_Error_exn_struct _tmp21F;const char*_tmp21E;struct Cyc_Lexing_Error_exn_struct*_tmp21D;(int)_throw((void*)((_tmp21D=_cycalloc(sizeof(*_tmp21D)),((_tmp21D[0]=((_tmp21F.tag=Cyc_Lexing_Error,((_tmp21F.f1=((_tmp21E="some action didn't return!",_tag_dyneither(_tmp21E,sizeof(char),27))),_tmp21F)))),_tmp21D)))));};}
# 1095
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,6);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLDA:
# 1091 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LLDB:
# 1092 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LLDC:
# 1097 "lex.cyl"
{const char*_tmp220;Cyc_Lex_runaway_err(((_tmp220="unterminated comment",_tag_dyneither(_tmp220,sizeof(char),21))),lexbuf);}
return 0;case 3: _LLDD:
# 1100 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4: _LLDE:
# 1101 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5: _LLDF:
# 1102 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6: _LLE0:
# 1103 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLE1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}{
# 1107
struct Cyc_Lexing_Error_exn_struct _tmp226;const char*_tmp225;struct Cyc_Lexing_Error_exn_struct*_tmp224;(int)_throw((void*)((_tmp224=_cycalloc(sizeof(*_tmp224)),((_tmp224[0]=((_tmp226.tag=Cyc_Lexing_Error,((_tmp226.f1=((_tmp225="some action didn't return!",_tag_dyneither(_tmp225,sizeof(char),27))),_tmp226)))),_tmp224)))));};}
# 1109
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE3:
# 1110 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 373;case 1: _LLE4:
# 1120 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 373;default: _LLE5:
# 1128
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}{
# 1131
struct Cyc_Lexing_Error_exn_struct _tmp22C;const char*_tmp22B;struct Cyc_Lexing_Error_exn_struct*_tmp22A;(int)_throw((void*)((_tmp22A=_cycalloc(sizeof(*_tmp22A)),((_tmp22A[0]=((_tmp22C.tag=Cyc_Lexing_Error,((_tmp22C.f1=((_tmp22B="some action didn't return!",_tag_dyneither(_tmp22B,sizeof(char),27))),_tmp22C)))),_tmp22A)))));};}
# 1133
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,8);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE7:
# 1130 "lex.cyl"
 Cyc_Lex_store_string_char('"');
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_store_string_char('"');
return 1;case 1: _LLE8:
# 1134 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2: _LLE9:
# 1138 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3: _LLEA:
# 1140 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LLEB:
# 1141 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1144
Cyc_Lex_store_string_char(')');
return 1;}case 5: _LLEC:
# 1147 "lex.cyl"
{const char*_tmp22D;Cyc_Lex_runaway_err(((_tmp22D="unterminated asm",_tag_dyneither(_tmp22D,sizeof(char),17))),lexbuf);}return 0;case 6: _LLED:
# 1148 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7: _LLEE:
# 1149 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLEF:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{
# 1153
struct Cyc_Lexing_Error_exn_struct _tmp233;const char*_tmp232;struct Cyc_Lexing_Error_exn_struct*_tmp231;(int)_throw((void*)((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231[0]=((_tmp233.tag=Cyc_Lexing_Error,((_tmp233.f1=((_tmp232="some action didn't return!",_tag_dyneither(_tmp232,sizeof(char),27))),_tmp233)))),_tmp231)))));};}
# 1155
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,9);}
# 1158 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}void Cyc_Lex_lex_init(int include_cyclone_keywords);static void _tmp242(unsigned int*_tmp241,unsigned int*_tmp240,int**_tmp23E){for(*_tmp241=0;*_tmp241 < *_tmp240;(*_tmp241)++){(*_tmp23E)[*_tmp241]=0;}}
# 1163
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1165
if(Cyc_Lex_ids_trie != 0){
struct _DynRegionHandle*_tmp139;struct Cyc_Lex_DynTrieSymbols _tmp138=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp139=_tmp138.dyn;
Cyc_Core_free_dynregion(_tmp139);}
# 1169
if(Cyc_Lex_typedefs_trie != 0){
struct _DynRegionHandle*_tmp13B;struct Cyc_Lex_DynTrie _tmp13A=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp13B=_tmp13A.dyn;
Cyc_Core_free_dynregion(_tmp13B);}{
# 1174
struct _DynRegionHandle*_tmp13D;struct Cyc_Core_NewRegion _tmp13C=Cyc_Core__new_dynregion("internal-error",0);_tmp13D=_tmp13C.dynregion;{
struct _DynRegionFrame _tmp13E;struct _RegionHandle*id_rgn=_open_dynregion(& _tmp13E,_tmp13D);{
const char*_tmp236;struct _dyneither_ptr*_tmp235;struct Cyc_Xarray_Xarray*symbols=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((_tmp235=_cycalloc(sizeof(*_tmp235)),((_tmp235[0]=(struct _dyneither_ptr)((_tmp236="",_tag_dyneither(_tmp236,sizeof(char),1))),_tmp235)))));
# 1179
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);{
struct Cyc_Lex_Trie*_tmp239;struct Cyc_Lex_DynTrieSymbols*_tmp238;Cyc_Lex_ids_trie=((_tmp238=_cycalloc(sizeof(*_tmp238)),((_tmp238->dyn=(struct _DynRegionHandle*)_tmp13D,((_tmp238->t=(struct Cyc_Lex_Trie*)((_tmp239=_region_malloc(id_rgn,sizeof(*_tmp239)),((_tmp239->children=Cyc_Lex_Zero(),((_tmp239->shared_str=0,_tmp239)))))),((_tmp238->symbols=(struct Cyc_Xarray_Xarray*)symbols,(struct Cyc_Lex_DynTrieSymbols*)_tmp238))))))));};}{
# 1183
struct _DynRegionHandle*_tmp144;struct Cyc_Core_NewRegion _tmp143=Cyc_Core__new_dynregion("internal-error",0);_tmp144=_tmp143.dynregion;{
struct _DynRegionFrame _tmp145;struct _RegionHandle*typedefs_rgn=_open_dynregion(& _tmp145,_tmp144);{
struct Cyc_Lex_Trie*_tmp23C;struct Cyc_Lex_DynTrie*_tmp23B;Cyc_Lex_typedefs_trie=((_tmp23B=_cycalloc(sizeof(*_tmp23B)),((_tmp23B->dyn=(struct _DynRegionHandle*)_tmp144,((_tmp23B->t=(struct Cyc_Lex_Trie*)(
(_tmp23C=_region_malloc(typedefs_rgn,sizeof(*_tmp23C)),((_tmp23C->children=Cyc_Lex_Zero(),((_tmp23C->shared_str=0,_tmp23C)))))),(struct Cyc_Lex_DynTrie*)_tmp23B))))));}
# 1188
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
{unsigned int _tmp241;unsigned int _tmp240;struct _dyneither_ptr _tmp23F;int*_tmp23E;unsigned int _tmp23D;Cyc_Lex_kw_nums=((_tmp23D=(unsigned int)Cyc_Lex_num_kws,((_tmp23E=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp23D)),((_tmp23F=_tag_dyneither(_tmp23E,sizeof(int),_tmp23D),((((_tmp240=_tmp23D,_tmp242(& _tmp241,& _tmp240,& _tmp23E))),_tmp23F))))))));}{
unsigned int i=0;
unsigned int rwsze=71;
{unsigned int j=0;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp14C=(Cyc_Lex_rw_array[(int)j]).f1;
Cyc_Lex_str_index(_tmp14C,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp14C));
*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;
++ i;}}}
# 1200
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};
# 1184
;_pop_dynregion(typedefs_rgn);};};
# 1175
;_pop_dynregion(id_rgn);};};}
