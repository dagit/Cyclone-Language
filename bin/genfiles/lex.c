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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
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
# 909 "absyn.h"
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;
# 83
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
# 134 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 43 "lex.cyl"
void Cyc_yyerror(struct _dyneither_ptr s){
Cyc_Position_post_error(Cyc_Position_mk_err_parse((unsigned int)Cyc_yylloc.first_line,s));}
# 49
int Cyc_Lex_compile_for_boot_r=0;struct Cyc_Lex_Trie;
# 52
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 58
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren _tmp15B;return((_tmp15B.Zero).val=0,(((_tmp15B.Zero).tag=1,_tmp15B)));}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
struct _tuple19 _tmp15E;union Cyc_Lex_TrieChildren _tmp15D;return((_tmp15D.One).val=((_tmp15E.f1=i,((_tmp15E.f2=t,_tmp15E)))),(((_tmp15D.One).tag=2,_tmp15D)));}
# 62
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
union Cyc_Lex_TrieChildren _tmp15F;return((_tmp15F.Many).val=ts,(((_tmp15F.Many).tag=3,_tmp15F)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};
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
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}struct _tuple20{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp7[14]="__attribute__";static char _tmp8[9]="abstract";static char _tmp9[6]="alias";static char _tmpA[5]="auto";static char _tmpB[6]="break";static char _tmpC[7]="calloc";static char _tmpD[5]="case";static char _tmpE[6]="catch";static char _tmpF[5]="char";static char _tmp10[6]="const";static char _tmp11[9]="continue";static char _tmp12[9]="datatype";static char _tmp13[8]="default";static char _tmp14[3]="do";static char _tmp15[7]="double";static char _tmp16[12]="dynregion_t";static char _tmp17[5]="else";static char _tmp18[5]="enum";static char _tmp19[7]="export";static char _tmp1A[7]="extern";static char _tmp1B[9]="fallthru";static char _tmp1C[6]="float";static char _tmp1D[4]="for";static char _tmp1E[5]="goto";static char _tmp1F[3]="if";static char _tmp20[7]="inline";static char _tmp21[11]="__inline__";static char _tmp22[4]="int";static char _tmp23[4]="let";static char _tmp24[5]="long";static char _tmp25[7]="malloc";static char _tmp26[10]="namespace";static char _tmp27[4]="new";static char _tmp28[11]="NOZEROTERM";static char _tmp29[5]="NULL";static char _tmp2A[8]="numelts";static char _tmp2B[9]="offsetof";static char _tmp2C[20]="__cyclone_port_on__";static char _tmp2D[21]="__cyclone_port_off__";static char _tmp2E[8]="rcalloc";static char _tmp2F[9]="region_t";static char _tmp30[7]="region";static char _tmp31[8]="regions";static char _tmp32[9]="register";static char _tmp33[13]="reset_region";static char _tmp34[9]="restrict";static char _tmp35[7]="return";static char _tmp36[8]="rmalloc";static char _tmp37[5]="rnew";static char _tmp38[6]="short";static char _tmp39[7]="signed";static char _tmp3A[7]="sizeof";static char _tmp3B[7]="static";static char _tmp3C[7]="struct";static char _tmp3D[7]="switch";static char _tmp3E[9]="tagcheck";static char _tmp3F[6]="tag_t";static char _tmp40[6]="throw";static char _tmp41[4]="try";static char _tmp42[7]="tunion";static char _tmp43[8]="typedef";static char _tmp44[6]="union";static char _tmp45[9]="unsigned";static char _tmp46[6]="using";static char _tmp47[8]="valueof";static char _tmp48[10]="valueof_t";static char _tmp49[5]="void";static char _tmp4A[9]="volatile";static char _tmp4B[6]="while";static char _tmp4C[8]="xtunion";static char _tmp4D[9]="ZEROTERM";
# 121
static struct _tuple20 Cyc_Lex_rw_array[71]={{{_tmp7,_tmp7,_tmp7 + 14},373,1},{{_tmp8,_tmp8,_tmp8 + 9},300,0},{{_tmp9,_tmp9,_tmp9 + 6},324,0},{{_tmpA,_tmpA,_tmpA + 5},258,1},{{_tmpB,_tmpB,_tmpB + 6},290,1},{{_tmpC,_tmpC,_tmpC + 7},309,0},{{_tmpD,_tmpD,_tmpD + 5},277,1},{{_tmpE,_tmpE,_tmpE + 6},297,1},{{_tmpF,_tmpF,_tmpF + 5},264,1},{{_tmp10,_tmp10,_tmp10 + 6},272,1},{{_tmp11,_tmp11,_tmp11 + 9},289,1},{{_tmp12,_tmp12,_tmp12 + 9},304,0},{{_tmp13,_tmp13,_tmp13 + 8},278,1},{{_tmp14,_tmp14,_tmp14 + 3},286,1},{{_tmp15,_tmp15,_tmp15 + 7},269,1},{{_tmp16,_tmp16,_tmp16 + 12},323,0},{{_tmp17,_tmp17,_tmp17 + 5},283,1},{{_tmp18,_tmp18,_tmp18 + 5},292,1},{{_tmp19,_tmp19,_tmp19 + 7},298,0},{{_tmp1A,_tmp1A,_tmp1A + 7},261,1},{{_tmp1B,_tmp1B,_tmp1B + 9},301,0},{{_tmp1C,_tmp1C,_tmp1C + 6},268,1},{{_tmp1D,_tmp1D,_tmp1D + 4},287,1},{{_tmp1E,_tmp1E,_tmp1E + 5},288,1},{{_tmp1F,_tmp1F,_tmp1F + 3},282,1},{{_tmp20,_tmp20,_tmp20 + 7},279,1},{{_tmp21,_tmp21,_tmp21 + 11},279,1},{{_tmp22,_tmp22,_tmp22 + 4},266,1},{{_tmp23,_tmp23,_tmp23 + 4},294,0},{{_tmp24,_tmp24,_tmp24 + 5},267,1},{{_tmp25,_tmp25,_tmp25 + 7},307,0},{{_tmp26,_tmp26,_tmp26 + 10},303,0},{{_tmp27,_tmp27,_tmp27 + 4},299,0},{{_tmp28,_tmp28,_tmp28 + 11},318,0},{{_tmp29,_tmp29,_tmp29 + 5},293,0},{{_tmp2A,_tmp2A,_tmp2A + 8},325,0},{{_tmp2B,_tmp2B,_tmp2B + 9},281,1},{{_tmp2C,_tmp2C,_tmp2C + 20},321,0},{{_tmp2D,_tmp2D,_tmp2D + 21},322,0},{{_tmp2E,_tmp2E,_tmp2E + 8},310,0},{{_tmp2F,_tmp2F,_tmp2F + 9},312,0},{{_tmp30,_tmp30,_tmp30 + 7},314,0},{{_tmp31,_tmp31,_tmp31 + 8},316,0},{{_tmp32,_tmp32,_tmp32 + 9},259,1},{{_tmp33,_tmp33,_tmp33 + 13},317,0},{{_tmp34,_tmp34,_tmp34 + 9},274,1},{{_tmp35,_tmp35,_tmp35 + 7},291,1},{{_tmp36,_tmp36,_tmp36 + 8},308,0},{{_tmp37,_tmp37,_tmp37 + 5},315,0},{{_tmp38,_tmp38,_tmp38 + 6},265,1},{{_tmp39,_tmp39,_tmp39 + 7},270,1},{{_tmp3A,_tmp3A,_tmp3A + 7},280,1},{{_tmp3B,_tmp3B,_tmp3B + 7},260,1},{{_tmp3C,_tmp3C,_tmp3C + 7},275,1},{{_tmp3D,_tmp3D,_tmp3D + 7},284,1},{{_tmp3E,_tmp3E,_tmp3E + 9},328,0},{{_tmp3F,_tmp3F,_tmp3F + 6},313,0},{{_tmp40,_tmp40,_tmp40 + 6},295,0},{{_tmp41,_tmp41,_tmp41 + 4},296,0},{{_tmp42,_tmp42,_tmp42 + 7},306,0},{{_tmp43,_tmp43,_tmp43 + 8},262,1},{{_tmp44,_tmp44,_tmp44 + 6},276,1},{{_tmp45,_tmp45,_tmp45 + 9},271,1},{{_tmp46,_tmp46,_tmp46 + 6},302,0},{{_tmp47,_tmp47,_tmp47 + 8},326,0},{{_tmp48,_tmp48,_tmp48 + 10},327,0},{{_tmp49,_tmp49,_tmp49 + 5},263,1},{{_tmp4A,_tmp4A,_tmp4A + 9},273,1},{{_tmp4B,_tmp4B,_tmp4B + 6},285,1},{{_tmp4C,_tmp4C,_tmp4C + 8},305,0},{{_tmp4D,_tmp4D,_tmp4D + 9},319,0}};
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
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void _tmp165(unsigned int*_tmp164,unsigned int*_tmp163,struct Cyc_Lex_Trie***_tmp162){for(*_tmp164=0;*_tmp164 < *_tmp163;(*_tmp164)++){(*_tmp162)[*_tmp164]=(struct Cyc_Lex_Trie*)0;}}
# 211
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 213
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp4E=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp4F=_tmp4E;struct Cyc_Lex_Trie**_tmp50;int _tmp51;struct Cyc_Lex_Trie*_tmp52;_LL1: if((_tmp4F.Many).tag != 3)goto _LL3;_tmp50=(struct Cyc_Lex_Trie**)(_tmp4F.Many).val;_LL2: {
# 219
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp50[_check_known_subscript_notnull(64,ch)]== 0){
struct Cyc_Lex_Trie*_tmp160;_tmp50[_check_known_subscript_notnull(64,ch)]=((_tmp160=_region_malloc(r,sizeof(*_tmp160)),((_tmp160->children=Cyc_Lex_Zero(),((_tmp160->shared_str=0,_tmp160))))));}
t=_tmp50[_check_known_subscript_notnull(64,ch)];
++ i;
goto _LL0;}_LL3: if((_tmp4F.One).tag != 2)goto _LL5;_tmp51=((struct _tuple19)(_tmp4F.One).val).f1;_tmp52=((struct _tuple19)(_tmp4F.One).val).f2;_LL4:
# 227
 if(_tmp51 == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp52;else{
# 230
unsigned int _tmp164;unsigned int _tmp163;struct Cyc_Lex_Trie**_tmp162;unsigned int _tmp161;struct Cyc_Lex_Trie**_tmp54=(_tmp161=64,((_tmp162=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp161)),((((_tmp163=_tmp161,_tmp165(& _tmp164,& _tmp163,& _tmp162))),_tmp162)))));
_tmp54[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp51))]=_tmp52;{
int _tmp55=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
{struct Cyc_Lex_Trie*_tmp166;_tmp54[_check_known_subscript_notnull(64,_tmp55)]=((_tmp166=_region_malloc(r,sizeof(*_tmp166)),((_tmp166->children=Cyc_Lex_Zero(),((_tmp166->shared_str=0,_tmp166))))));}
t->children=Cyc_Lex_Many(_tmp54);
t=_tmp54[_check_known_subscript_notnull(64,_tmp55)];};}
# 237
++ i;
goto _LL0;_LL5: if((_tmp4F.Zero).tag != 1)goto _LL0;_LL6:
# 241
 while(i <= last){
struct Cyc_Lex_Trie*_tmp167;struct Cyc_Lex_Trie*_tmp5A=(_tmp167=_region_malloc(r,sizeof(*_tmp167)),((_tmp167->children=Cyc_Lex_Zero(),((_tmp167->shared_str=0,_tmp167)))));
t->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp5A);
t=_tmp5A;}
# 246
return t;_LL0:;}
# 249
return t;}
# 252
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols _tmp5C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));struct _DynRegionHandle*_tmp5E;struct Cyc_Lex_Trie*_tmp5F;struct Cyc_Xarray_Xarray*_tmp60;struct Cyc_Lex_DynTrieSymbols _tmp5D=_tmp5C;_tmp5E=_tmp5D.dyn;_tmp5F=_tmp5D.t;_tmp60=_tmp5D.symbols;{
struct _DynRegionFrame _tmp61;struct _RegionHandle*d=_open_dynregion(& _tmp61,_tmp5E);
{struct Cyc_Lex_Trie*_tmp62=Cyc_Lex_trie_lookup(d,_tmp5F,buff,offset,len);
# 257
if(_tmp62->shared_str == 0){
struct _dyneither_ptr _tmp63=Cyc_Core_new_string((unsigned int)(len + 1));
Cyc_zstrncpy(_dyneither_ptr_decrease_size(_tmp63,sizeof(char),1),(struct _dyneither_ptr)_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned long)len);{
struct _dyneither_ptr*_tmp168;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp60,((_tmp168=_cycalloc(sizeof(*_tmp168)),((_tmp168[0]=(struct _dyneither_ptr)_tmp63,_tmp168)))));
_tmp62->shared_str=ans;};}{
# 263
int _tmp65=_tmp62->shared_str;_npop_handler(0);return _tmp65;};}
# 255
;_pop_dynregion(d);};}
# 267
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}
# 273
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp66=*sptr;
struct Cyc_Lex_DynTrie _tmp67=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));struct _DynRegionHandle*_tmp69;struct Cyc_Lex_Trie*_tmp6A;struct Cyc_Lex_DynTrie _tmp68=_tmp67;_tmp69=_tmp68.dyn;_tmp6A=_tmp68.t;{
struct _DynRegionFrame _tmp6B;struct _RegionHandle*d=_open_dynregion(& _tmp6B,_tmp69);{
struct Cyc_Lex_Trie*_tmp6C=Cyc_Lex_trie_lookup(d,_tmp6A,_tmp66,0,(int)(_get_dyneither_size(_tmp66,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp6C))->shared_str=1;}
# 280
_npop_handler(0);return;
# 276
;_pop_dynregion(d);};}
# 283
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols _tmp6D=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));struct _DynRegionHandle*_tmp6F;struct Cyc_Lex_Trie*_tmp70;struct Cyc_Xarray_Xarray*_tmp71;struct Cyc_Lex_DynTrieSymbols _tmp6E=_tmp6D;_tmp6F=_tmp6E.dyn;_tmp70=_tmp6E.t;_tmp71=_tmp6E.symbols;{
struct _DynRegionFrame _tmp72;struct _RegionHandle*d=_open_dynregion(& _tmp72,_tmp6F);
{struct _dyneither_ptr*_tmp73=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp71,symbol_num);_npop_handler(0);return _tmp73;};_pop_dynregion(d);};}
# 298 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
struct Cyc_Core_Invalid_argument_exn_struct _tmp16E;const char*_tmp16D;struct Cyc_Core_Invalid_argument_exn_struct*_tmp16C;(int)_throw((void*)((_tmp16C=_cycalloc(sizeof(*_tmp16C)),((_tmp16C[0]=((_tmp16E.tag=Cyc_Core_Invalid_argument,((_tmp16E.f1=((_tmp16D="string to integer conversion",_tag_dyneither(_tmp16D,sizeof(char),29))),_tmp16E)))),_tmp16C)))));}}}}
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
struct _tuple5 _tmp16F;return(_tmp16F.f1=Cyc_Absyn_Unsigned,((_tmp16F.f2=(int)n,_tmp16F)));}case 'l': _LL9:
 break;case 'L': _LLA:
 break;default: _LLB:
 n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}{
# 324
struct _tuple5 _tmp170;return(_tmp170.f1=Cyc_Absyn_Signed,((_tmp170.f2=(int)n,_tmp170)));};}
# 329
char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};
int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c);static void _tmp176(int*sz,unsigned int*_tmp175,unsigned int*_tmp174,char**_tmp172){for(*_tmp175=0;*_tmp175 < *_tmp174;(*_tmp175)++){(*_tmp172)[*_tmp175]=(char)(
# 337
*_tmp175 < *sz?*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*_tmp175)):'\000');}}
# 332
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
unsigned int _tmp175;unsigned int _tmp174;struct _dyneither_ptr _tmp173;char*_tmp172;unsigned int _tmp171;struct _dyneither_ptr str=(_tmp171=(unsigned int)newsz,((_tmp172=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp171 + 1)),((_tmp173=_tag_dyneither(_tmp172,sizeof(char),_tmp171 + 1),((((_tmp174=_tmp171,((_tmp176(& sz,& _tmp175,& _tmp174,& _tmp172),_tmp172[_tmp174]=(char)0)))),_tmp173)))))));
Cyc_Lex_string_buffer=str;};}
# 340
{char _tmp179;char _tmp178;struct _dyneither_ptr _tmp177;(_tmp177=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos),((_tmp178=*((char*)_check_dyneither_subscript(_tmp177,sizeof(char),0)),((_tmp179=c,((_get_dyneither_size(_tmp177,sizeof(char))== 1  && (_tmp178 == '\000'  && _tmp179 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp177.curr)=_tmp179)))))));}
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
struct Cyc_Lex_Lvis*_tmp17A;struct Cyc_Lex_Lvis*_tmp80=(_tmp17A=_cycalloc(sizeof(*_tmp17A)),((_tmp17A->current_namespace=0,((_tmp17A->imported_namespaces=0,_tmp17A)))));
# 383
struct Cyc_List_List*_tmp17B;struct Cyc_List_List*_tmp81=(_tmp17B=_cycalloc(sizeof(*_tmp17B)),((_tmp17B->hd=_tmp80,((_tmp17B->tl=0,_tmp17B)))));
struct Cyc_Lex_Ldecls*_tmp17C;struct Cyc_Dict_Dict init_decls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,(
# 387
(_tmp17C=_cycalloc(sizeof(*_tmp17C)),((_tmp17C->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp17C->namespaces=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp17C)))))));
struct Cyc_Lex_Lstate*_tmp17F;struct Cyc_Core_Opt*_tmp17E;Cyc_Lex_lstate=((_tmp17E=_cycalloc(sizeof(*_tmp17E)),((_tmp17E->v=((_tmp17F=_cycalloc(sizeof(*_tmp17F)),((_tmp17F->lstack=_tmp81,((_tmp17F->decls=init_decls,_tmp17F)))))),_tmp17E))));}
# 392
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
# 394
struct _dyneither_ptr*n=(struct _dyneither_ptr*)ns->hd;
# 396
{struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
# 399
struct Cyc_List_List*_tmp180;struct Cyc_List_List*x=(_tmp180=_cycalloc(sizeof(*_tmp180)),((_tmp180->hd=lv->current_namespace,((_tmp180->tl=lv->imported_namespaces,_tmp180)))));
# 401
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
# 404
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))
# 406
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,ns);}}}
# 409
{const char*_tmp186;void*_tmp185[1];const char*_tmp184;struct Cyc_String_pa_PrintArg_struct _tmp183;Cyc_yyerror((struct _dyneither_ptr)((_tmp183.tag=0,((_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(ns,((_tmp184="::",_tag_dyneither(_tmp184,sizeof(char),3))))),((_tmp185[0]=& _tmp183,Cyc_aprintf(((_tmp186="undeclared namespace %s",_tag_dyneither(_tmp186,sizeof(char),24))),_tag_dyneither(_tmp185,sizeof(void*),1)))))))));}
return 0;}
# 413
static void Cyc_Lex_recompute_typedefs(){
# 415
struct Cyc_Lex_DynTrie _tmp8C=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));struct _DynRegionHandle*_tmp8E;struct Cyc_Lex_Trie*_tmp8F;struct Cyc_Lex_DynTrie _tmp8D=_tmp8C;_tmp8E=_tmp8D.dyn;_tmp8F=_tmp8D.t;
Cyc_Core_free_dynregion(_tmp8E);{
struct Cyc_Core_NewRegion _tmp90=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmp92;struct Cyc_Core_NewRegion _tmp91=_tmp90;_tmp92=_tmp91.dynregion;{
struct _DynRegionFrame _tmp93;struct _RegionHandle*d2=_open_dynregion(& _tmp93,_tmp92);{
struct Cyc_Lex_Trie*_tmp189;struct Cyc_Lex_DynTrie*_tmp188;Cyc_Lex_typedefs_trie=((_tmp188=_cycalloc(sizeof(*_tmp188)),((_tmp188->dyn=(struct _DynRegionHandle*)_tmp92,((_tmp188->t=(struct Cyc_Lex_Trie*)((_tmp189=_region_malloc(d2,sizeof(*_tmp189)),((_tmp189->children=Cyc_Lex_Zero(),((_tmp189->shared_str=0,_tmp189)))))),_tmp188))))));}{
# 423
struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
struct Cyc_List_List*_tmp18A;struct Cyc_List_List*x=(_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A->hd=lv->current_namespace,((_tmp18A->tl=lv->imported_namespaces,_tmp18A)))));
# 427
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};
# 418
;_pop_dynregion(d2);};};}
# 435
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmp97;_push_handler(& _tmp97);{int _tmp99=0;if(setjmp(_tmp97.handler))_tmp99=1;if(!_tmp99){
{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);
int _tmp9A=((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);_npop_handler(0);return _tmp9A;}
# 438
;_pop_handler();}else{void*_tmp98=(void*)_exn_thrown;void*_tmp9C=_tmp98;void*_tmp9E;_LLE: {struct Cyc_Dict_Absent_exn_struct*_tmp9D=(struct Cyc_Dict_Absent_exn_struct*)_tmp9C;if(_tmp9D->tag != Cyc_Dict_Absent)goto _LL10;}_LLF:
# 441
 return 0;_LL10: _tmp9E=_tmp9C;_LL11:(void)_throw(_tmp9E);_LLD:;}};}
# 444
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 449
struct _dyneither_ptr _tmp9F=*v;
int len=(int)(_get_dyneither_size(_tmp9F,sizeof(char))- 1);
struct Cyc_Lex_DynTrie _tmpA0=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));struct _DynRegionHandle*_tmpA2;struct Cyc_Lex_Trie*_tmpA3;struct Cyc_Lex_DynTrie _tmpA1=_tmpA0;_tmpA2=_tmpA1.dyn;_tmpA3=_tmpA1.t;{
struct _DynRegionFrame _tmpA4;struct _RegionHandle*d=_open_dynregion(& _tmpA4,_tmpA2);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpA5=((struct Cyc_Lex_Trie*)_check_null(_tmpA3))->children;union Cyc_Lex_TrieChildren _tmpA6=_tmpA5;int _tmpA7;struct Cyc_Lex_Trie*_tmpA8;struct Cyc_Lex_Trie*_tmpA9;struct Cyc_Lex_Trie**_tmpAA;_LL13: if((_tmpA6.Zero).tag != 1)goto _LL15;_LL14: {
int _tmpAB=0;_npop_handler(0);return _tmpAB;}_LL15: if((_tmpA6.One).tag != 2)goto _LL17;_tmpA7=((struct _tuple19)(_tmpA6.One).val).f1;_tmpA8=((struct _tuple19)(_tmpA6.One).val).f2;if(!(_tmpA7 != *((const char*)_check_dyneither_subscript(_tmp9F,sizeof(char),i))))goto _LL17;_LL16: {
int _tmpAC=0;_npop_handler(0);return _tmpAC;}_LL17: if((_tmpA6.One).tag != 2)goto _LL19;_tmpA9=((struct _tuple19)(_tmpA6.One).val).f2;_LL18:
 _tmpA3=_tmpA9;goto _LL12;_LL19: if((_tmpA6.Many).tag != 3)goto _LL12;_tmpAA=(struct Cyc_Lex_Trie**)(_tmpA6.Many).val;_LL1A: {
# 459
struct Cyc_Lex_Trie*_tmpAD=_tmpAA[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(_tmp9F,sizeof(char),i))))];
if(_tmpAD == 0){
int _tmpAE=0;_npop_handler(0);return _tmpAE;}
_tmpA3=_tmpAD;
goto _LL12;}_LL12:;}}{
# 465
int _tmpAF=((struct Cyc_Lex_Trie*)_check_null(_tmpA3))->shared_str;_npop_handler(0);return _tmpAF;};
# 453
;_pop_dynregion(d);};};}
# 469
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp18B;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp18B=_cycalloc(sizeof(*_tmp18B)),((_tmp18B->hd=s,((_tmp18B->tl=0,_tmp18B)))))));
# 473
{struct Cyc_Lex_Lvis*_tmp18E;struct Cyc_List_List*_tmp18D;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
(_tmp18D=_cycalloc(sizeof(*_tmp18D)),((_tmp18D->hd=((_tmp18E=_cycalloc(sizeof(*_tmp18E)),((_tmp18E->current_namespace=new_ns,((_tmp18E->imported_namespaces=0,_tmp18E)))))),((_tmp18D->tl=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,_tmp18D))))));}{
# 479
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns);
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){
{struct Cyc_Lex_Ldecls*_tmp18F;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns,(
# 483
(_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F->typedefs=ld->typedefs,((_tmp18F->namespaces=
# 485
((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s),_tmp18F)))))));}{
# 487
struct Cyc_Lex_Ldecls*_tmp190;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns,(
# 489
(_tmp190=_cycalloc(sizeof(*_tmp190)),((_tmp190->typedefs=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp190->namespaces=
# 492
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp190)))))));};}
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
{union Cyc_Absyn_Nmspace _tmpB5=(*q).f1;union Cyc_Absyn_Nmspace _tmpB6=_tmpB5;struct Cyc_List_List*_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB9;_LL1C: if((_tmpB6.Loc_n).tag != 4)goto _LL1E;_LL1D:
# 509
{struct Cyc_List_List*_tmp191;ns=((_tmp191=_cycalloc(sizeof(*_tmp191)),((_tmp191->hd=(*q).f2,((_tmp191->tl=0,_tmp191))))));}
goto _LL1B;_LL1E: if((_tmpB6.Rel_n).tag != 1)goto _LL20;_tmpB7=(struct Cyc_List_List*)(_tmpB6.Rel_n).val;_LL1F:
 _tmpB8=_tmpB7;goto _LL21;_LL20: if((_tmpB6.C_n).tag != 3)goto _LL22;_tmpB8=(struct Cyc_List_List*)(_tmpB6.C_n).val;_LL21:
 _tmpB9=_tmpB8;goto _LL23;_LL22: if((_tmpB6.Abs_n).tag != 2)goto _LL1B;_tmpB9=(struct Cyc_List_List*)(_tmpB6.Abs_n).val;_LL23:
# 514
{struct Cyc_List_List*_tmp192;ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB9,((_tmp192=_cycalloc(sizeof(*_tmp192)),((_tmp192->hd=(*q).f2,((_tmp192->tl=0,_tmp192))))))));}
goto _LL1B;_LL1B:;}{
# 518
struct Cyc_List_List*_tmpBC=Cyc_Lex_get_absolute_namespace(ns);
struct Cyc_List_List*_tmpBD=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
# 521
{struct Cyc_List_List*_tmp193;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193->hd=_tmpBC,((_tmp193->tl=_tmpBD,_tmp193))))));}
# 525
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,_tmpBC))->typedefs);};}
# 527
void Cyc_Lex_leave_using(){
struct Cyc_List_List*_tmpBF=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)_check_null(_tmpBF))->tl;
# 532
Cyc_Lex_recompute_typedefs();}
# 535
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 537
struct Cyc_List_List*_tmpC0=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpC1=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls*_tmpC2=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpC1,_tmpC0);
struct Cyc_Lex_Ldecls*_tmp194;struct Cyc_Lex_Ldecls*_tmpC3=(_tmp194=_cycalloc(sizeof(*_tmp194)),((_tmp194->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpC2->typedefs,(*q).f2),((_tmp194->namespaces=_tmpC2->namespaces,_tmp194)))));
# 542
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpC1,_tmpC0,_tmpC3);
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
{const char*_tmp197;void*_tmp196;(_tmp196=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp197="deprecated ZEROTERM, use @zeroterm",_tag_dyneither(_tmp197,sizeof(char),35))),_tag_dyneither(_tmp196,sizeof(void*),0)));}
break;case 318: _LL25:
# 560
{const char*_tmp19A;void*_tmp199;(_tmp199=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp19A="deprecated NOZEROTERM, use @nozeroterm",_tag_dyneither(_tmp19A,sizeof(char),39))),_tag_dyneither(_tmp199,sizeof(void*),0)));}
break;case 306: _LL26:
# 563
{const char*_tmp19D;void*_tmp19C;(_tmp19C=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp19D="deprecated tunion, use datatype",_tag_dyneither(_tmp19D,sizeof(char),32))),_tag_dyneither(_tmp19C,sizeof(void*),0)));}
break;case 305: _LL27:
# 566
{const char*_tmp1A0;void*_tmp19F;(_tmp19F=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp1A0="deprecated xtunion, use @extensible datatype",_tag_dyneither(_tmp1A0,sizeof(char),45))),_tag_dyneither(_tmp19F,sizeof(void*),0)));}
break;default: _LL28:
 break;}
# 570
return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),symbol_num - 1));}{
# 573
struct _dyneither_ptr*_tmpCD=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpCD;
# 577
if(Cyc_Lex_is_typedef(0,_tmpCD))
return 370;
# 581
return 362;};}
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
struct Cyc_Core_Impossible_exn_struct _tmp1A6;const char*_tmp1A5;struct Cyc_Core_Impossible_exn_struct*_tmp1A4;(int)_throw((void*)((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4[0]=((_tmp1A6.tag=Cyc_Core_Impossible,((_tmp1A6.f1=((_tmp1A5="bad namespace",_tag_dyneither(_tmp1A5,sizeof(char),14))),_tmp1A6)))),_tmp1A4)))));}{
int vlen=i - start;
if(v != 0){
struct Cyc_List_List*_tmp1A7;rev_vs=((_tmp1A7=_cycalloc(sizeof(*_tmp1A7)),((_tmp1A7->hd=v,((_tmp1A7->tl=rev_vs,_tmp1A7))))));}
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));
i +=2;};}
# 604
if(v == 0){
struct Cyc_Core_Impossible_exn_struct _tmp1AD;const char*_tmp1AC;struct Cyc_Core_Impossible_exn_struct*_tmp1AB;(int)_throw((void*)((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB[0]=((_tmp1AD.tag=Cyc_Core_Impossible,((_tmp1AD.f1=((_tmp1AC="bad namespace",_tag_dyneither(_tmp1AC,sizeof(char),14))),_tmp1AD)))),_tmp1AB)))));}{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 609
{const char*_tmp1AE;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1AE="Cyc",_tag_dyneither(_tmp1AE,sizeof(char),4))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1AF;Cyc_Lex_token_qvar=((_tmp1AF=_cycalloc(sizeof(*_tmp1AF)),((_tmp1AF->f1=Cyc_Absyn_Abs_n(vs,0),((_tmp1AF->f2=v,_tmp1AF))))));};}else{
# 613
const char*_tmp1B0;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1B0="C",_tag_dyneither(_tmp1B0,sizeof(char),2))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1B1;Cyc_Lex_token_qvar=((_tmp1B1=_cycalloc(sizeof(*_tmp1B1)),((_tmp1B1->f1=Cyc_Absyn_Abs_n(vs,1),((_tmp1B1->f2=v,_tmp1B1))))));};}else{
# 617
struct _tuple0*_tmp1B2;Cyc_Lex_token_qvar=((_tmp1B2=_cycalloc(sizeof(*_tmp1B2)),((_tmp1B2->f1=Cyc_Absyn_Rel_n(vs),((_tmp1B2->f2=v,_tmp1B2))))));}}}
if(Cyc_Lex_is_typedef(vs,v))
return 372;
return 371;};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
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
unsigned int*_tmp1B4;unsigned int _tmp1B3;struct _dyneither_ptr linenumpos=(_tmp1B3=10,((_tmp1B4=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1B3),_tag_dyneither(_tmp1B4,sizeof(unsigned int),_tmp1B3))));
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;{
struct Cyc_Lex_PosInfo*_tmp1B5;return(_tmp1B5=_region_malloc(r,sizeof(*_tmp1B5)),((_tmp1B5->next=next,((_tmp1B5->starting_line=starting_line,((_tmp1B5->filename=(struct _dyneither_ptr)filename,((_tmp1B5->linenumpos=linenumpos,((_tmp1B5->linenumpos_offset=1,_tmp1B5)))))))))));};}
# 654
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0){struct Cyc_Core_Impossible_exn_struct _tmp1BB;const char*_tmp1BA;struct Cyc_Core_Impossible_exn_struct*_tmp1B9;(int)_throw((void*)((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((_tmp1B9[0]=((_tmp1BB.tag=Cyc_Core_Impossible,((_tmp1BB.f1=((_tmp1BA="empty position info!",_tag_dyneither(_tmp1BA,sizeof(char),21))),_tmp1BB)))),_tmp1B9)))));}{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
_swap_dyneither(& p->linenumpos,& linenums);{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 662
if(offset >= n){
# 664
unsigned int*_tmp1BD;unsigned int _tmp1BC;struct _dyneither_ptr newlinenums=(_tmp1BC=n * 2,((_tmp1BD=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1BC),_tag_dyneither(_tmp1BD,sizeof(unsigned int),_tmp1BC))));
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
const char*_tmp1BE;struct _dyneither_ptr filename=(_tmp1BE="",_tag_dyneither(_tmp1BE,sizeof(char),1));
{const char*_tmp1C3;void*_tmp1C2[2];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1C1;struct Cyc_CharPtr_sa_ScanfArg_struct _tmp1C0;if(((_tmp1C0.tag=7,((_tmp1C0.f1=_tag_dyneither(buf,sizeof(char),100),((_tmp1C1.tag=2,((_tmp1C1.f1=& i,((_tmp1C2[0]=& _tmp1C1,((_tmp1C2[1]=& _tmp1C0,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1C3="# %d \"%s",_tag_dyneither(_tmp1C3,sizeof(char),9))),_tag_dyneither(_tmp1C2,sizeof(void*),2))))))))))))))== 2){
if(Cyc_Lex_compile_for_boot_r){
# 687
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100))- 1);
int last_slash=-1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100,i)]== '/'){last_slash=i;break;}
-- i;}{
# 693
const char*_tmp1C7;void*_tmp1C6[1];struct Cyc_String_pa_PrintArg_struct _tmp1C5;filename=(struct _dyneither_ptr)((_tmp1C5.tag=0,((_tmp1C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(_tag_dyneither(buf,sizeof(char),100),sizeof(char),last_slash),sizeof(char),1)),((_tmp1C6[0]=& _tmp1C5,Cyc_aprintf(((_tmp1C7="\"%s",_tag_dyneither(_tmp1C7,sizeof(char),4))),_tag_dyneither(_tmp1C6,sizeof(void*),1))))))));};}else{
# 695
const char*_tmp1CB;void*_tmp1CA[1];struct Cyc_String_pa_PrintArg_struct _tmp1C9;filename=(struct _dyneither_ptr)((_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100),((_tmp1CA[0]=& _tmp1C9,Cyc_aprintf(((_tmp1CB="\"%s",_tag_dyneither(_tmp1CB,sizeof(char),4))),_tag_dyneither(_tmp1CA,sizeof(void*),1))))))));}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
const char*_tmp1CF;void*_tmp1CE[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1CD;if(((_tmp1CD.tag=2,((_tmp1CD.f1=& i,((_tmp1CE[0]=& _tmp1CD,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1CF="# %d",_tag_dyneither(_tmp1CF,sizeof(char),5))),_tag_dyneither(_tmp1CE,sizeof(void*),1))))))))== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 703
++ Cyc_Lex_linenumber;
return;}}}
# 706
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple21{struct _dyneither_ptr f1;unsigned int f2;};
# 713
struct _tuple21 Cyc_Lex_xlate_pos(unsigned int char_offset){
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
struct _tuple21 _tmp1D0;return(_tmp1D0.f1=p->filename,((_tmp1D0.f2=p->starting_line + base,_tmp1D0)));};};}}{
# 733
struct _tuple21 _tmp1D1;return(_tmp1D1.f1=_tag_dyneither(0,0,0),((_tmp1D1.f2=0,_tmp1D1)));};}
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
switch(ans){case 362: _LL2A:
# 758
{union Cyc_YYSTYPE _tmp1D2;*yylval=(((_tmp1D2.String_tok).val=Cyc_Lex_token_string,(((_tmp1D2.String_tok).tag=3,_tmp1D2))));}break;case 370: _LL2B:
# 760
{union Cyc_YYSTYPE _tmp1D3;*yylval=(((_tmp1D3.String_tok).val=Cyc_Lex_token_string,(((_tmp1D3.String_tok).tag=3,_tmp1D3))));}break;case 371: _LL2C:
# 762
{union Cyc_YYSTYPE _tmp1D4;*yylval=(((_tmp1D4.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1D4.QualId_tok).tag=5,_tmp1D4))));}break;case 372: _LL2D:
# 764
{union Cyc_YYSTYPE _tmp1D5;*yylval=(((_tmp1D5.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1D5.QualId_tok).tag=5,_tmp1D5))));}break;case 369: _LL2E:
# 766
{union Cyc_YYSTYPE _tmp1D6;*yylval=(((_tmp1D6.String_tok).val=Cyc_Lex_token_string,(((_tmp1D6.String_tok).tag=3,_tmp1D6))));}break;case 363: _LL2F:
# 768
{union Cyc_YYSTYPE _tmp1D7;*yylval=(((_tmp1D7.Int_tok).val=Cyc_Lex_token_int,(((_tmp1D7.Int_tok).tag=1,_tmp1D7))));}break;case 366: _LL30:
# 770
{union Cyc_YYSTYPE _tmp1D8;*yylval=(((_tmp1D8.Char_tok).val=Cyc_Lex_token_char,(((_tmp1D8.Char_tok).tag=2,_tmp1D8))));}break;case 367: _LL31:
# 772
{union Cyc_YYSTYPE _tmp1D9;*yylval=(((_tmp1D9.String_tok).val=Cyc_Lex_token_string,(((_tmp1D9.String_tok).tag=3,_tmp1D9))));}break;case 368: _LL32:
# 774
{union Cyc_YYSTYPE _tmp1DA;*yylval=(((_tmp1DA.String_tok).val=Cyc_Lex_token_string,(((_tmp1DA.String_tok).tag=3,_tmp1DA))));}break;case 364: _LL33:
# 776
{union Cyc_YYSTYPE _tmp1DB;*yylval=(((_tmp1DB.String_tok).val=Cyc_Lex_token_string,(((_tmp1DB.String_tok).tag=3,_tmp1DB))));}break;case 365: _LL34:
# 778
{union Cyc_YYSTYPE _tmp1DC;*yylval=(((_tmp1DC.String_tok).val=Cyc_Lex_token_string,(((_tmp1DC.String_tok).tag=3,_tmp1DC))));}break;case 374: _LL35:
# 780
{union Cyc_YYSTYPE _tmp1DD;*yylval=(((_tmp1DD.Asm_tok).val=Cyc_Lex_token_asm,(((_tmp1DD.Asm_tok).tag=6,_tmp1DD))));}break;default: _LL36:
 break;}
# 783
return ans;}
# 790
const int Cyc_Lex_lex_base[293]={0,24,117,119,123,132,134,139,150,157,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,163,- 1,12,23,38,21,34,35,47,201,174,10,12,21,- 15,- 16,11,- 1,195,- 17,- 11,- 10,215,- 12,- 9,- 4,- 5,268,349,223,- 13,- 16,- 3,- 2,12,2,- 16,- 17,13,- 1,373,- 18,14,291,446,469,299,- 14,15,- 1,16,524,307,597,672,389,- 25,30,57,54,54,81,57,52,58,61,78,66,74,747,85,84,83,822,91,95,99,897,104,95,84,101,89,109,120,112,121,972,118,110,124,112,1047,117,120,1122,115,127,124,137,131,124,127,1197,154,154,145,153,1272,145,156,145,149,163,181,169,175,1347,168,178,180,1422,173,1497,174,190,184,180,191,200,191,199,1572,- 62,295,- 41,17,30,- 44,361,31,265,353,32,267,269,406,330,1649,1692,271,320,240,323,- 7,1762,1851,241,1926,1984,2059,348,- 61,- 31,2134,250,2209,2284,2342,2417,2492,254,2567,2642,2717,2792,2867,2942,3017,3092,- 26,- 45,- 46,303,- 29,315,- 34,- 28,- 37,320,- 38,3167,3196,540,347,358,550,3206,3236,1730,1659,3269,3300,3338,350,377,3408,3446,388,398,391,401,393,451,- 9,- 43,383,- 23,- 42,18,341,3378,- 36,- 19,- 21,- 35,- 20,- 22,1,3486,2,350,501,355,369,371,372,381,390,392,394,429,3559,3643,- 59,- 53,- 52,- 51,- 50,- 49,- 48,- 47,- 54,- 57,- 58,1678,430,- 55,- 56,- 60,- 30,- 27,- 24,551,- 39,19,- 15,- 41,599};
const int Cyc_Lex_lex_backtrk[293]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,- 1,- 1,14,- 1,17,- 1,- 1,- 1,12,- 1,- 1,- 1,- 1,- 1,13,12,- 1,- 1,- 1,- 1,1,3,- 1,- 1,15,- 1,18,- 1,1,13,- 1,14,13,- 1,1,- 1,14,17,12,- 1,13,12,- 1,24,24,24,24,24,24,- 1,- 1,- 1,- 1,- 1,- 1,13,- 1,- 1,- 1,19,- 1,- 1,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,23,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,18,- 1,- 1,- 1,- 1,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,- 1,- 1,21,- 1,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,39,- 1,40,61,- 1,61,61,61,61,61,61,61,61,61,8,10,61,61,61,61,- 1,2,2,61,61,61,2,61,- 1,- 1,2,- 1,2,0,- 1,4,4,- 1,5,3,3,3,3,3,3,1,- 1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,- 1,11,12,- 1,- 1,12,8,9,8,8,- 1,7,7,7,9,9,8,8,- 1,- 1,- 1,- 1,- 1,41,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,39};
const int Cyc_Lex_lex_default[293]={157,80,38,49,60,49,38,34,- 1,10,0,0,- 1,0,20,0,0,- 1,0,0,20,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,34,- 1,- 1,- 1,0,0,- 1,0,- 1,0,0,0,- 1,0,0,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,0,287,- 1,- 1,252,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,240,0,0,- 1,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,287,0,- 1,0,0,- 1};
const int Cyc_Lex_lex_trans[3900]={0,0,0,0,0,0,0,0,0,158,159,158,158,160,23,21,21,21,22,22,18,55,19,43,19,19,55,291,242,288,0,0,158,161,162,163,25,164,165,166,283,282,167,168,19,169,170,171,172,173,173,173,173,173,173,173,173,173,174,19,175,176,177,25,178,179,179,179,179,179,179,179,179,179,179,179,180,179,179,179,179,179,179,179,179,179,179,179,179,179,179,290,286,251,181,182,183,184,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,27,185,81,82,39,56,57,74,28,72,61,83,29,62,30,84,31,85,56,57,32,39,58,86,40,33,18,148,56,35,73,146,102,41,63,87,24,24,24,24,24,56,120,88,11,41,89,12,121,24,24,24,24,24,90,94,91,92,36,24,93,- 1,95,37,- 1,98,96,25,13,14,97,99,24,100,15,16,101,117,112,106,25,17,107,108,103,59,75,33,33,33,33,33,64,- 1,104,109,105,110,- 1,111,113,114,115,42,116,118,44,119,133,128,33,45,122,123,134,124,125,126,19,127,46,46,46,46,46,46,46,46,129,130,131,132,142,186,135,47,136,137,- 1,- 1,53,53,53,53,53,53,53,53,54,54,54,54,54,54,54,54,138,- 1,26,139,140,141,143,144,48,145,147,149,150,19,21,151,152,153,49,154,155,156,71,204,284,292,50,292,292,192,18,65,23,192,10,247,51,52,52,52,52,52,52,52,52,52,52,285,292,250,212,248,249,213,52,52,52,52,52,52,70,70,70,70,70,70,70,70,71,71,71,71,71,71,71,71,79,79,79,79,79,79,79,79,- 1,208,52,52,52,52,52,52,288,239,43,289,- 1,211,240,214,65,209,55,210,19,43,207,66,246,- 1,281,43,241,- 1,242,277,21,243,52,52,52,52,52,52,52,52,52,52,47,276,80,275,274,44,18,52,52,52,52,52,52,273,67,67,67,67,67,67,67,67,272,- 1,271,44,270,44,48,54,54,54,54,54,54,54,54,54,253,52,52,52,52,52,52,244,48,245,245,245,245,245,245,245,245,245,245,44,45,44,48,269,278,21,49,187,10,10,50,45,45,238,0,0,0,0,18,0,48,0,23,0,10,0,48,0,68,69,69,69,69,69,69,69,69,69,69,0,10,10,0,45,45,238,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,238,0,0,0,0,0,0,69,69,69,69,69,69,278,0,0,69,69,69,69,69,69,279,279,279,279,279,279,279,279,0,44,238,0,288,0,45,289,0,69,69,69,69,69,69,76,76,76,76,76,76,76,76,0,0,0,220,0,220,0,47,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,292,- 1,292,292,0,0,0,0,48,- 1,0,0,0,19,21,0,0,0,49,0,0,0,0,292,0,0,50,0,0,0,18,- 1,23,0,10,0,77,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,19,19,19,19,19,19,19,19,19,19,0,0,- 1,0,0,0,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,10,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,25,25,25,25,25,25,25,25,25,25,0,0,0,0,0,0,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,0,0,0,0,25,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,50,50,50,50,50,50,50,50,50,50,0,0,0,0,0,0,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,0,0,0,0,50,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,47,47,47,47,47,47,47,47,47,47,0,0,0,0,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,0,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,18,18,18,18,18,18,18,18,18,18,0,0,0,0,0,0,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,0,0,0,0,18,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,0,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,0,0,0,0,23,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,44,44,44,44,44,44,44,44,44,44,0,0,0,0,0,0,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,0,0,0,0,44,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,21,21,21,21,21,21,21,21,21,21,0,0,0,0,0,0,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,0,0,0,0,21,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,45,45,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,0,0,0,0,45,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,48,48,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,0,0,0,0,48,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49,49,49,0,0,0,0,0,0,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,0,0,0,0,49,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,215,0,226,226,226,226,226,226,226,226,227,227,225,225,225,225,225,225,225,225,225,225,278,217,0,0,0,0,0,0,228,280,280,280,280,280,280,280,280,229,0,0,230,215,0,216,216,216,216,216,216,216,216,216,216,217,0,0,0,0,0,0,228,0,0,0,217,0,0,0,0,229,0,218,230,0,0,0,224,0,224,0,219,225,225,225,225,225,225,225,225,225,225,0,0,0,0,0,217,0,0,0,0,0,0,218,0,0,0,0,0,0,0,0,219,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,205,0,0,0,0,206,0,0,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,198,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,0,0,0,0,196,0,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,190,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,191,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,0,0,0,0,0,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,0,0,0,0,188,0,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,0,0,0,0,194,0,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,195,0,0,0,0,0,0,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,0,0,0,0,193,0,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,194,194,194,194,194,194,194,194,194,194,0,0,0,0,0,0,0,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,0,0,0,0,194,0,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,196,196,196,196,196,196,196,196,196,196,0,0,0,0,0,0,0,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,0,0,0,0,196,0,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,197,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,197,0,199,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,197,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,200,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,197,0,197,197,197,197,197,197,197,197,197,197,197,197,201,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,202,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,203,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,0,0,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,0,0,0,0,197,0,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,222,222,222,222,222,222,222,222,222,222,0,0,0,0,0,0,0,0,0,0,0,223,54,0,0,0,0,215,54,216,216,216,216,216,216,216,216,216,216,221,221,221,221,221,221,221,221,221,221,0,217,0,0,223,54,0,0,218,0,0,54,47,0,0,0,0,219,47,0,222,222,222,222,222,222,222,222,222,222,0,0,0,217,0,0,0,0,0,0,218,223,54,0,47,0,0,0,54,219,47,0,0,225,225,225,225,225,225,225,225,225,225,0,0,0,0,0,0,0,0,0,0,223,54,54,0,0,0,0,54,54,215,0,226,226,226,226,226,226,226,226,227,227,0,0,0,0,0,0,0,0,0,0,0,217,0,54,0,0,0,0,236,54,0,0,0,0,0,0,215,237,227,227,227,227,227,227,227,227,227,227,0,0,0,0,0,217,0,0,0,0,0,217,236,0,0,0,0,0,234,0,0,237,0,0,0,0,0,235,0,0,245,245,245,245,245,245,245,245,245,245,0,0,0,217,0,0,0,0,0,0,234,223,54,0,0,0,0,0,54,235,231,231,231,231,231,231,231,231,231,231,0,0,0,0,0,0,0,231,231,231,231,231,231,223,54,0,0,0,0,0,54,0,0,0,0,0,0,0,231,231,231,231,231,231,231,231,231,231,0,231,231,231,231,231,231,231,231,231,231,231,231,0,0,0,254,0,232,0,0,255,0,0,0,0,0,233,0,0,256,256,256,256,256,256,256,256,0,231,231,231,231,231,231,257,0,0,0,0,232,0,0,0,0,0,0,0,0,233,0,0,0,0,0,0,0,0,0,0,0,0,0,0,258,0,0,0,0,259,260,0,0,0,261,0,0,0,0,0,0,0,262,0,0,0,263,0,264,0,265,0,266,267,267,267,267,267,267,267,267,267,267,0,0,0,0,0,0,0,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,0,0,0,0,0,0,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,268,0,0,0,0,0,0,0,0,267,267,267,267,267,267,267,267,267,267,0,0,0,0,0,0,0,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,0,0,0,0,0,0,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3900]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,12,14,20,22,14,20,35,40,58,62,66,72,74,160,243,289,- 1,- 1,0,0,0,0,59,0,0,0,252,254,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,161,164,167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,1,1,2,3,3,2,27,3,4,1,28,4,29,1,30,1,5,5,31,6,5,1,6,32,7,81,3,7,3,82,84,2,4,86,8,8,8,8,8,5,83,87,9,6,88,9,83,24,24,24,24,24,89,85,90,91,7,8,92,34,85,7,34,94,95,8,9,9,96,98,24,99,9,9,100,103,104,105,24,9,106,107,102,5,2,33,33,33,33,33,4,34,102,108,102,109,34,110,112,113,114,6,115,117,42,118,120,122,33,42,121,121,120,123,124,125,33,126,42,42,42,42,42,42,42,42,128,129,130,131,133,0,134,42,135,136,14,20,46,46,46,46,46,46,46,46,53,53,53,53,53,53,53,53,137,1,24,138,139,140,142,143,42,144,146,148,149,42,42,150,151,152,42,153,154,155,176,181,165,158,42,158,158,189,42,168,42,195,42,169,42,51,51,51,51,51,51,51,51,51,51,165,158,168,174,169,169,174,51,51,51,51,51,51,67,67,67,67,67,67,67,67,70,70,70,70,70,70,70,70,76,76,76,76,76,76,76,76,166,207,51,51,51,51,51,51,163,171,2,163,3,209,171,213,4,175,175,175,64,177,177,64,244,5,255,6,171,166,240,257,7,240,52,52,52,52,52,52,52,52,52,52,64,258,185,259,260,64,9,52,52,52,52,52,52,261,64,64,64,64,64,64,64,64,262,34,263,218,264,219,228,64,79,79,79,79,79,79,79,79,166,52,52,52,52,52,52,170,229,170,170,170,170,170,170,170,170,170,170,218,64,219,228,265,280,64,64,185,232,233,64,234,235,236,- 1,- 1,- 1,- 1,64,- 1,229,- 1,64,- 1,64,- 1,64,- 1,64,68,68,68,68,68,68,68,68,68,68,- 1,232,233,- 1,234,235,236,68,68,68,68,68,68,69,69,69,69,69,69,69,69,69,69,237,- 1,- 1,- 1,- 1,- 1,- 1,69,69,69,69,69,69,256,- 1,- 1,68,68,68,68,68,68,256,256,256,256,256,256,256,256,- 1,75,237,- 1,287,- 1,75,287,- 1,69,69,69,69,69,69,75,75,75,75,75,75,75,75,- 1,- 1,- 1,217,- 1,217,- 1,75,217,217,217,217,217,217,217,217,217,217,220,220,220,220,220,220,220,220,220,220,292,166,292,292,- 1,- 1,- 1,- 1,75,163,- 1,- 1,- 1,75,75,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,292,- 1,- 1,75,- 1,- 1,- 1,75,240,75,- 1,75,- 1,75,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,93,93,93,93,93,93,93,93,93,93,- 1,- 1,287,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,127,- 1,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,145,- 1,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,147,- 1,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,156,156,156,156,156,156,156,156,156,156,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,- 1,- 1,- 1,- 1,156,- 1,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,172,- 1,172,172,172,172,172,172,172,172,172,172,224,224,224,224,224,224,224,224,224,224,279,172,- 1,- 1,- 1,- 1,- 1,- 1,172,279,279,279,279,279,279,279,279,172,- 1,- 1,172,173,- 1,173,173,173,173,173,173,173,173,173,173,172,- 1,- 1,- 1,- 1,- 1,- 1,172,- 1,- 1,- 1,173,- 1,- 1,- 1,- 1,172,- 1,173,172,- 1,- 1,- 1,223,- 1,223,- 1,173,223,223,223,223,223,223,223,223,223,223,- 1,- 1,- 1,- 1,- 1,173,- 1,- 1,- 1,- 1,- 1,- 1,173,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,173,179,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,- 1,- 1,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,179,- 1,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,180,- 1,- 1,- 1,- 1,180,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,180,180,180,180,180,180,180,180,180,180,180,- 1,- 1,- 1,- 1,- 1,- 1,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,- 1,- 1,- 1,- 1,180,- 1,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,182,182,182,182,182,182,182,182,182,182,- 1,- 1,- 1,- 1,- 1,- 1,- 1,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,- 1,- 1,- 1,- 1,182,- 1,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,- 1,- 1,- 1,- 1,183,- 1,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,184,184,184,184,184,184,184,184,184,184,- 1,- 1,- 1,- 1,- 1,- 1,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,- 1,- 1,- 1,- 1,184,- 1,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,188,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,- 1,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,188,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,190,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,- 1,- 1,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,190,- 1,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,191,191,191,191,191,191,191,191,191,191,191,- 1,- 1,- 1,- 1,- 1,- 1,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,- 1,- 1,- 1,- 1,191,- 1,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,191,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,192,- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,- 1,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,- 1,- 1,- 1,- 1,193,- 1,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,194,194,194,194,194,194,194,194,194,194,- 1,- 1,- 1,- 1,- 1,- 1,- 1,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,- 1,- 1,- 1,- 1,194,- 1,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,196,196,196,196,196,196,196,196,196,196,- 1,- 1,- 1,- 1,- 1,- 1,- 1,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,- 1,- 1,- 1,- 1,196,- 1,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,- 1,- 1,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,197,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,198,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,- 1,- 1,- 1,- 1,- 1,- 1,- 1,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,- 1,- 1,- 1,- 1,199,- 1,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,200,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,201,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,202,202,202,- 1,- 1,- 1,- 1,- 1,- 1,- 1,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,- 1,- 1,- 1,- 1,202,- 1,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,203,203,203,203,203,203,203,203,203,203,- 1,- 1,- 1,- 1,- 1,- 1,- 1,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,- 1,- 1,- 1,- 1,203,- 1,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,215,215,215,215,215,215,215,215,215,215,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,215,215,- 1,- 1,- 1,- 1,216,215,216,216,216,216,216,216,216,216,216,216,221,221,221,221,221,221,221,221,221,221,- 1,216,- 1,- 1,215,215,- 1,- 1,216,- 1,- 1,215,221,- 1,- 1,- 1,- 1,216,221,- 1,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,- 1,216,222,222,- 1,221,- 1,- 1,- 1,222,216,221,- 1,- 1,225,225,225,225,225,225,225,225,225,225,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,222,222,225,- 1,- 1,- 1,- 1,222,225,226,- 1,226,226,226,226,226,226,226,226,226,226,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,226,- 1,225,- 1,- 1,- 1,- 1,226,225,- 1,- 1,- 1,- 1,- 1,- 1,227,226,227,227,227,227,227,227,227,227,227,227,- 1,- 1,- 1,- 1,- 1,226,- 1,- 1,- 1,- 1,- 1,227,226,- 1,- 1,- 1,- 1,- 1,227,- 1,- 1,226,- 1,- 1,- 1,- 1,- 1,227,- 1,- 1,245,245,245,245,245,245,245,245,245,245,- 1,- 1,- 1,227,- 1,- 1,- 1,- 1,- 1,- 1,227,245,245,- 1,- 1,- 1,- 1,- 1,245,227,230,230,230,230,230,230,230,230,230,230,- 1,- 1,- 1,- 1,- 1,- 1,- 1,230,230,230,230,230,230,245,245,- 1,- 1,- 1,- 1,- 1,245,- 1,- 1,- 1,- 1,- 1,- 1,- 1,231,231,231,231,231,231,231,231,231,231,- 1,230,230,230,230,230,230,231,231,231,231,231,231,- 1,- 1,- 1,253,- 1,231,- 1,- 1,253,- 1,- 1,- 1,- 1,- 1,231,- 1,- 1,253,253,253,253,253,253,253,253,- 1,231,231,231,231,231,231,253,- 1,- 1,- 1,- 1,231,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,231,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,253,- 1,- 1,- 1,- 1,253,253,- 1,- 1,- 1,253,- 1,- 1,- 1,- 1,- 1,- 1,- 1,253,- 1,- 1,- 1,253,- 1,253,- 1,253,- 1,253,266,266,266,266,266,266,266,266,266,266,- 1,- 1,- 1,- 1,- 1,- 1,- 1,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,- 1,- 1,- 1,- 1,- 1,- 1,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,267,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,267,267,267,267,267,267,267,267,267,267,- 1,- 1,- 1,- 1,- 1,- 1,- 1,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,- 1,- 1,- 1,- 1,- 1,- 1,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,267,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
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
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(293,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(293,state)];
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
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(3900,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(3900,base + c)];else{
# 827
state=Cyc_Lex_lex_default[_check_known_subscript_notnull(293,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp1E3;const char*_tmp1E2;struct Cyc_Lexing_Error_exn_struct*_tmp1E1;(int)_throw((void*)((_tmp1E1=_cycalloc(sizeof(*_tmp1E1)),((_tmp1E1[0]=((_tmp1E3.tag=Cyc_Lexing_Error,((_tmp1E3.f1=((_tmp1E2="empty token",_tag_dyneither(_tmp1E2,sizeof(char),12))),_tmp1E3)))),_tmp1E1)))));}else{
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
return 369;case 6: _LL3E:
# 809
 return Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3F:
# 812 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 363;case 8: _LL40:
# 815
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 363;case 9: _LL41:
# 821 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 363;case 10: _LL42:
# 824
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 363;case 11: _LL43:
# 828 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 368;case 12: _LL44:
# 832 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 368;case 13: _LL45:
# 835
 return 345;case 14: _LL46:
# 836 "lex.cyl"
 return 346;case 15: _LL47:
# 837 "lex.cyl"
 return 343;case 16: _LL48:
# 838 "lex.cyl"
 return 344;case 17: _LL49:
# 839 "lex.cyl"
 return 339;case 18: _LL4A:
# 840 "lex.cyl"
 return 340;case 19: _LL4B:
# 841 "lex.cyl"
 return 352;case 20: _LL4C:
# 842 "lex.cyl"
 return 353;case 21: _LL4D:
# 843 "lex.cyl"
 return 349;case 22: _LL4E:
# 844 "lex.cyl"
 return 350;case 23: _LL4F:
# 845 "lex.cyl"
 return 351;case 24: _LL50:
# 846 "lex.cyl"
 return 358;case 25: _LL51:
# 847 "lex.cyl"
 return 357;case 26: _LL52:
# 848 "lex.cyl"
 return 356;case 27: _LL53:
# 849 "lex.cyl"
 return 354;case 28: _LL54:
# 850 "lex.cyl"
 return 355;case 29: _LL55:
# 851 "lex.cyl"
 return 347;case 30: _LL56:
# 852 "lex.cyl"
 return 348;case 31: _LL57:
# 853 "lex.cyl"
 return 341;case 32: _LL58:
# 855
 return 342;case 33: _LL59:
# 856 "lex.cyl"
 return 360;case 34: _LL5A:
# 857 "lex.cyl"
 return 338;case 35: _LL5B:
# 858 "lex.cyl"
 return 359;case 36: _LL5C:
# 859 "lex.cyl"
 return 361;case 37: _LL5D:
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
return 364;case 44: _LL64:
# 882 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 365;case 45: _LL65:
# 890 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 367;case 46: _LL66:
# 897
 Cyc_Lex_token_char='\a';return 366;case 47: _LL67:
# 898 "lex.cyl"
 Cyc_Lex_token_char='\b';return 366;case 48: _LL68:
# 899 "lex.cyl"
 Cyc_Lex_token_char='\f';return 366;case 49: _LL69:
# 900 "lex.cyl"
 Cyc_Lex_token_char='\n';return 366;case 50: _LL6A:
# 901 "lex.cyl"
 Cyc_Lex_token_char='\r';return 366;case 51: _LL6B:
# 902 "lex.cyl"
 Cyc_Lex_token_char='\t';return 366;case 52: _LL6C:
# 903 "lex.cyl"
 Cyc_Lex_token_char='\v';return 366;case 53: _LL6D:
# 904 "lex.cyl"
 Cyc_Lex_token_char='\\';return 366;case 54: _LL6E:
# 905 "lex.cyl"
 Cyc_Lex_token_char='\'';return 366;case 55: _LL6F:
# 906 "lex.cyl"
 Cyc_Lex_token_char='"';return 366;case 56: _LL70:
# 907 "lex.cyl"
 Cyc_Lex_token_char='?';return 366;case 57: _LL71:
# 910 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;
return 366;case 58: _LL72:
# 915 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;
return 366;case 59: _LL73:
# 919
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 366;case 60: _LL74:
# 923
 return - 1;case 61: _LL75:
# 925
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL76:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}{
# 929
struct Cyc_Lexing_Error_exn_struct _tmp1E9;const char*_tmp1E8;struct Cyc_Lexing_Error_exn_struct*_tmp1E7;(int)_throw((void*)((_tmp1E7=_cycalloc(sizeof(*_tmp1E7)),((_tmp1E7[0]=((_tmp1E9.tag=Cyc_Lexing_Error,((_tmp1E9.f1=((_tmp1E8="some action didn't return!",_tag_dyneither(_tmp1E8,sizeof(char),27))),_tmp1E9)))),_tmp1E7)))));};}
# 931
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_cyc_qual_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL78:
# 932 "lex.cyl"
{const char*_tmp1EA;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1EA="taggedZ",_tag_dyneither(_tmp1EA,sizeof(char),8))));}return(int)'@';case 1: _LL79:
# 934
{const char*_tmp1EB;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1EB="zerotermZ",_tag_dyneither(_tmp1EB,sizeof(char),10))));}return(int)'@';case 2: _LL7A:
# 936
{const char*_tmp1EC;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1EC="nozerotermZ",_tag_dyneither(_tmp1EC,sizeof(char),12))));}return(int)'@';case 3: _LL7B:
# 938
{const char*_tmp1ED;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1ED="extensibleZ",_tag_dyneither(_tmp1ED,sizeof(char),12))));}return(int)'@';case 4: _LL7C:
# 940
{const char*_tmp1EE;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1EE="resetableZ",_tag_dyneither(_tmp1EE,sizeof(char),11))));}return(int)'@';case 5: _LL7D:
# 942
{const char*_tmp1EF;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1EF="regionZ",_tag_dyneither(_tmp1EF,sizeof(char),8))));}return(int)'@';case 6: _LL7E:
# 944
{const char*_tmp1F0;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F0="numeltsZ",_tag_dyneither(_tmp1F0,sizeof(char),9))));}return(int)'@';case 7: _LL7F:
# 946
{const char*_tmp1F1;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F1="thinZ",_tag_dyneither(_tmp1F1,sizeof(char),6))));}return(int)'@';case 8: _LL80:
# 948
{const char*_tmp1F2;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F2="fatZ",_tag_dyneither(_tmp1F2,sizeof(char),5))));}return(int)'@';case 9: _LL81:
# 950
{const char*_tmp1F3;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F3="notnullZ",_tag_dyneither(_tmp1F3,sizeof(char),9))));}return(int)'@';case 10: _LL82:
# 952
{const char*_tmp1F4;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F4="nullableZ",_tag_dyneither(_tmp1F4,sizeof(char),10))));}return(int)'@';case 11: _LL83:
# 954
{const char*_tmp1F5;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1F5="requiresZ",_tag_dyneither(_tmp1F5,sizeof(char),10))));}return(int)'@';case 12: _LL84:
# 955 "lex.cyl"
 return 335;case 13: _LL85:
# 956 "lex.cyl"
 return 319;case 14: _LL86:
# 957 "lex.cyl"
 return 318;case 15: _LL87:
# 958 "lex.cyl"
 return 336;case 16: _LL88:
# 959 "lex.cyl"
 return 337;case 17: _LL89:
# 960 "lex.cyl"
 return 320;case 18: _LL8A:
# 961 "lex.cyl"
 return 329;case 19: _LL8B:
# 962 "lex.cyl"
 return 330;case 20: _LL8C:
# 963 "lex.cyl"
 return 331;case 21: _LL8D:
# 964 "lex.cyl"
 return 332;case 22: _LL8E:
# 965 "lex.cyl"
 return 333;case 23: _LL8F:
# 966 "lex.cyl"
 return 334;case 24: _LL90:
# 967 "lex.cyl"
 lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL91:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}{
# 971
struct Cyc_Lexing_Error_exn_struct _tmp1FB;const char*_tmp1FA;struct Cyc_Lexing_Error_exn_struct*_tmp1F9;(int)_throw((void*)((_tmp1F9=_cycalloc(sizeof(*_tmp1F9)),((_tmp1F9[0]=((_tmp1FB.tag=Cyc_Lexing_Error,((_tmp1FB.f1=((_tmp1FA="some action didn't return!",_tag_dyneither(_tmp1FA,sizeof(char),27))),_tmp1FB)))),_tmp1F9)))));};}
# 973
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL93:
# 970 "lex.cyl"
 return 0;case 1: _LL94:
# 972
 Cyc_Lex_store_string_char('\a');return 1;case 2: _LL95:
# 973 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3: _LL96:
# 974 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4: _LL97:
# 975 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5: _LL98:
# 976 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6: _LL99:
# 977 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7: _LL9A:
# 978 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8: _LL9B:
# 979 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9: _LL9C:
# 980 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10: _LL9D:
# 981 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11: _LL9E:
# 982 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12: _LL9F:
# 985 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13: _LLA0:
# 988 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LLA1:
# 990
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15: _LLA2:
# 992
 Cyc_Lex_inc_linenumber();{const char*_tmp1FC;Cyc_Lex_runaway_err(((_tmp1FC="wide character ends in newline",_tag_dyneither(_tmp1FC,sizeof(char),31))),lexbuf);}return 0;case 16: _LLA3:
# 993 "lex.cyl"
{const char*_tmp1FD;Cyc_Lex_runaway_err(((_tmp1FD="unterminated wide character",_tag_dyneither(_tmp1FD,sizeof(char),28))),lexbuf);}return 0;case 17: _LLA4:
# 994 "lex.cyl"
{const char*_tmp1FE;Cyc_Lex_err(((_tmp1FE="bad character following backslash in wide character",_tag_dyneither(_tmp1FE,sizeof(char),52))),lexbuf);}return 1;default: _LLA5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}{
# 998
struct Cyc_Lexing_Error_exn_struct _tmp204;const char*_tmp203;struct Cyc_Lexing_Error_exn_struct*_tmp202;(int)_throw((void*)((_tmp202=_cycalloc(sizeof(*_tmp202)),((_tmp202[0]=((_tmp204.tag=Cyc_Lexing_Error,((_tmp204.f1=((_tmp203="some action didn't return!",_tag_dyneither(_tmp203,sizeof(char),27))),_tmp204)))),_tmp202)))));};}
# 1000
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,2);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA7:
# 999 "lex.cyl"
 return 1;case 1: _LLA8:
# 1000 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2: _LLA9:
# 1001 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3: _LLAA:
# 1003
 lexbuf->lex_curr_pos -=1;return 0;default: _LLAB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}{
# 1007
struct Cyc_Lexing_Error_exn_struct _tmp20A;const char*_tmp209;struct Cyc_Lexing_Error_exn_struct*_tmp208;(int)_throw((void*)((_tmp208=_cycalloc(sizeof(*_tmp208)),((_tmp208[0]=((_tmp20A.tag=Cyc_Lexing_Error,((_tmp20A.f1=((_tmp209="some action didn't return!",_tag_dyneither(_tmp209,sizeof(char),27))),_tmp20A)))),_tmp208)))));};}
# 1009
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,3);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAD:
# 1007 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1: _LLAE:
# 1008 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2: _LLAF:
# 1009 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3: _LLB0:
# 1010 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4: _LLB1:
# 1011 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5: _LLB2:
# 1012 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 6: _LLB3:
# 1013 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7: _LLB4:
# 1014 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 8: _LLB5:
# 1015 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9: _LLB6:
# 1016 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10: _LLB7:
# 1017 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11: _LLB8:
# 1018 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12: _LLB9:
# 1019 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13: _LLBA:
# 1022 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);
return 1;case 14: _LLBB:
# 1027 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);
return 1;case 15: _LLBC:
# 1032
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 16: _LLBD:
# 1034 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp20B;Cyc_Lex_runaway_err(((_tmp20B="string ends in newline",_tag_dyneither(_tmp20B,sizeof(char),23))),lexbuf);}
return 0;case 17: _LLBE:
# 1038 "lex.cyl"
{const char*_tmp20C;Cyc_Lex_runaway_err(((_tmp20C="unterminated string",_tag_dyneither(_tmp20C,sizeof(char),20))),lexbuf);}
return 0;case 18: _LLBF:
# 1041 "lex.cyl"
{const char*_tmp20D;Cyc_Lex_err(((_tmp20D="bad character following backslash in string",_tag_dyneither(_tmp20D,sizeof(char),44))),lexbuf);}
return 1;default: _LLC0:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}{
# 1046
struct Cyc_Lexing_Error_exn_struct _tmp213;const char*_tmp212;struct Cyc_Lexing_Error_exn_struct*_tmp211;(int)_throw((void*)((_tmp211=_cycalloc(sizeof(*_tmp211)),((_tmp211[0]=((_tmp213.tag=Cyc_Lexing_Error,((_tmp213.f1=((_tmp212="some action didn't return!",_tag_dyneither(_tmp212,sizeof(char),27))),_tmp213)))),_tmp211)))));};}
# 1048
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,4);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC2:
# 1051 "lex.cyl"
{const char*_tmp214;Cyc_Lex_store_string(((_tmp214="\" L\"",_tag_dyneither(_tmp214,sizeof(char),5))));}return 1;case 1: _LLC3:
# 1052 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2: _LLC4:
# 1053 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3: _LLC5:
# 1055
 lexbuf->lex_curr_pos -=1;return 0;default: _LLC6:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}{
# 1059
struct Cyc_Lexing_Error_exn_struct _tmp21A;const char*_tmp219;struct Cyc_Lexing_Error_exn_struct*_tmp218;(int)_throw((void*)((_tmp218=_cycalloc(sizeof(*_tmp218)),((_tmp218[0]=((_tmp21A.tag=Cyc_Lexing_Error,((_tmp21A.f1=((_tmp219="some action didn't return!",_tag_dyneither(_tmp219,sizeof(char),27))),_tmp21A)))),_tmp218)))));};}
# 1061
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,5);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC8:
# 1058 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1: _LLC9:
# 1059 "lex.cyl"
{const char*_tmp21B;Cyc_Lex_store_string(((_tmp21B="\\a",_tag_dyneither(_tmp21B,sizeof(char),3))));}return 1;case 2: _LLCA:
# 1060 "lex.cyl"
{const char*_tmp21C;Cyc_Lex_store_string(((_tmp21C="\\b",_tag_dyneither(_tmp21C,sizeof(char),3))));}return 1;case 3: _LLCB:
# 1061 "lex.cyl"
{const char*_tmp21D;Cyc_Lex_store_string(((_tmp21D="\\f",_tag_dyneither(_tmp21D,sizeof(char),3))));}return 1;case 4: _LLCC:
# 1062 "lex.cyl"
{const char*_tmp21E;Cyc_Lex_store_string(((_tmp21E="\\n",_tag_dyneither(_tmp21E,sizeof(char),3))));}return 1;case 5: _LLCD:
# 1063 "lex.cyl"
{const char*_tmp21F;Cyc_Lex_store_string(((_tmp21F="\\r",_tag_dyneither(_tmp21F,sizeof(char),3))));}return 1;case 6: _LLCE:
# 1064 "lex.cyl"
{const char*_tmp220;Cyc_Lex_store_string(((_tmp220="\\t",_tag_dyneither(_tmp220,sizeof(char),3))));}return 1;case 7: _LLCF:
# 1065 "lex.cyl"
{const char*_tmp221;Cyc_Lex_store_string(((_tmp221="\\v",_tag_dyneither(_tmp221,sizeof(char),3))));}return 1;case 8: _LLD0:
# 1066 "lex.cyl"
{const char*_tmp222;Cyc_Lex_store_string(((_tmp222="\\\\",_tag_dyneither(_tmp222,sizeof(char),3))));}return 1;case 9: _LLD1:
# 1067 "lex.cyl"
{const char*_tmp223;Cyc_Lex_store_string(((_tmp223="\\'",_tag_dyneither(_tmp223,sizeof(char),3))));}return 1;case 10: _LLD2:
# 1068 "lex.cyl"
{const char*_tmp224;Cyc_Lex_store_string(((_tmp224="\\\"",_tag_dyneither(_tmp224,sizeof(char),3))));}return 1;case 11: _LLD3:
# 1069 "lex.cyl"
{const char*_tmp225;Cyc_Lex_store_string(((_tmp225="\\?",_tag_dyneither(_tmp225,sizeof(char),3))));}return 1;case 12: _LLD4:
# 1072 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 13: _LLD5:
# 1077 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 14: _LLD6:
# 1082
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 15: _LLD7:
# 1084 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp226;Cyc_Lex_runaway_err(((_tmp226="string ends in newline",_tag_dyneither(_tmp226,sizeof(char),23))),lexbuf);}
return 0;case 16: _LLD8:
# 1088 "lex.cyl"
{const char*_tmp227;Cyc_Lex_runaway_err(((_tmp227="unterminated string",_tag_dyneither(_tmp227,sizeof(char),20))),lexbuf);}
return 0;case 17: _LLD9:
# 1091 "lex.cyl"
{const char*_tmp228;Cyc_Lex_err(((_tmp228="bad character following backslash in string",_tag_dyneither(_tmp228,sizeof(char),44))),lexbuf);}
return 1;default: _LLDA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}{
# 1096
struct Cyc_Lexing_Error_exn_struct _tmp22E;const char*_tmp22D;struct Cyc_Lexing_Error_exn_struct*_tmp22C;(int)_throw((void*)((_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C[0]=((_tmp22E.tag=Cyc_Lexing_Error,((_tmp22E.f1=((_tmp22D="some action didn't return!",_tag_dyneither(_tmp22D,sizeof(char),27))),_tmp22E)))),_tmp22C)))));};}
# 1098
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,6);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLDC:
# 1094 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LLDD:
# 1095 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LLDE:
# 1100 "lex.cyl"
{const char*_tmp22F;Cyc_Lex_runaway_err(((_tmp22F="unterminated comment",_tag_dyneither(_tmp22F,sizeof(char),21))),lexbuf);}
return 0;case 3: _LLDF:
# 1103 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4: _LLE0:
# 1104 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5: _LLE1:
# 1105 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6: _LLE2:
# 1106 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLE3:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}{
# 1110
struct Cyc_Lexing_Error_exn_struct _tmp235;const char*_tmp234;struct Cyc_Lexing_Error_exn_struct*_tmp233;(int)_throw((void*)((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233[0]=((_tmp235.tag=Cyc_Lexing_Error,((_tmp235.f1=((_tmp234="some action didn't return!",_tag_dyneither(_tmp234,sizeof(char),27))),_tmp235)))),_tmp233)))));};}
# 1112
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE5:
# 1113 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 374;case 1: _LLE6:
# 1123 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 374;default: _LLE7:
# 1131
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}{
# 1134
struct Cyc_Lexing_Error_exn_struct _tmp23B;const char*_tmp23A;struct Cyc_Lexing_Error_exn_struct*_tmp239;(int)_throw((void*)((_tmp239=_cycalloc(sizeof(*_tmp239)),((_tmp239[0]=((_tmp23B.tag=Cyc_Lexing_Error,((_tmp23B.f1=((_tmp23A="some action didn't return!",_tag_dyneither(_tmp23A,sizeof(char),27))),_tmp23B)))),_tmp239)))));};}
# 1136
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,8);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE9:
# 1133 "lex.cyl"
 Cyc_Lex_store_string_char('"');
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_store_string_char('"');
return 1;case 1: _LLEA:
# 1137 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2: _LLEB:
# 1141 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3: _LLEC:
# 1143 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LLED:
# 1144 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1147
Cyc_Lex_store_string_char(')');
return 1;}case 5: _LLEE:
# 1150 "lex.cyl"
{const char*_tmp23C;Cyc_Lex_runaway_err(((_tmp23C="unterminated asm",_tag_dyneither(_tmp23C,sizeof(char),17))),lexbuf);}return 0;case 6: _LLEF:
# 1151 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7: _LLF0:
# 1152 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLF1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{
# 1156
struct Cyc_Lexing_Error_exn_struct _tmp242;const char*_tmp241;struct Cyc_Lexing_Error_exn_struct*_tmp240;(int)_throw((void*)((_tmp240=_cycalloc(sizeof(*_tmp240)),((_tmp240[0]=((_tmp242.tag=Cyc_Lexing_Error,((_tmp242.f1=((_tmp241="some action didn't return!",_tag_dyneither(_tmp241,sizeof(char),27))),_tmp242)))),_tmp240)))));};}
# 1158
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,9);}
# 1161 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}void Cyc_Lex_lex_init(int include_cyclone_keywords);static void _tmp251(unsigned int*_tmp250,unsigned int*_tmp24F,int**_tmp24D){for(*_tmp250=0;*_tmp250 < *_tmp24F;(*_tmp250)++){(*_tmp24D)[*_tmp250]=(int)0;}}
# 1166
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1168
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols _tmp142=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));struct _DynRegionHandle*_tmp144;struct Cyc_Lex_DynTrieSymbols _tmp143=_tmp142;_tmp144=_tmp143.dyn;
Cyc_Core_free_dynregion(_tmp144);}
# 1172
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie _tmp145=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));struct _DynRegionHandle*_tmp147;struct Cyc_Lex_DynTrie _tmp146=_tmp145;_tmp147=_tmp146.dyn;
Cyc_Core_free_dynregion(_tmp147);}{
# 1177
struct Cyc_Core_NewRegion _tmp148=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmp14A;struct Cyc_Core_NewRegion _tmp149=_tmp148;_tmp14A=_tmp149.dynregion;{
struct _DynRegionFrame _tmp14B;struct _RegionHandle*id_rgn=_open_dynregion(& _tmp14B,_tmp14A);{
const char*_tmp245;struct _dyneither_ptr*_tmp244;struct Cyc_Xarray_Xarray*symbols=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((_tmp244=_cycalloc(sizeof(*_tmp244)),((_tmp244[0]=(struct _dyneither_ptr)((_tmp245="",_tag_dyneither(_tmp245,sizeof(char),1))),_tmp244)))));
# 1182
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);{
struct Cyc_Lex_Trie*_tmp248;struct Cyc_Lex_DynTrieSymbols*_tmp247;Cyc_Lex_ids_trie=((_tmp247=_cycalloc(sizeof(*_tmp247)),((_tmp247->dyn=(struct _DynRegionHandle*)_tmp14A,((_tmp247->t=(struct Cyc_Lex_Trie*)((_tmp248=_region_malloc(id_rgn,sizeof(*_tmp248)),((_tmp248->children=Cyc_Lex_Zero(),((_tmp248->shared_str=0,_tmp248)))))),((_tmp247->symbols=(struct Cyc_Xarray_Xarray*)symbols,_tmp247))))))));};}{
# 1186
struct Cyc_Core_NewRegion _tmp150=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmp152;struct Cyc_Core_NewRegion _tmp151=_tmp150;_tmp152=_tmp151.dynregion;{
struct _DynRegionFrame _tmp153;struct _RegionHandle*typedefs_rgn=_open_dynregion(& _tmp153,_tmp152);{
struct Cyc_Lex_Trie*_tmp24B;struct Cyc_Lex_DynTrie*_tmp24A;Cyc_Lex_typedefs_trie=((_tmp24A=_cycalloc(sizeof(*_tmp24A)),((_tmp24A->dyn=(struct _DynRegionHandle*)_tmp152,((_tmp24A->t=(struct Cyc_Lex_Trie*)(
(_tmp24B=_region_malloc(typedefs_rgn,sizeof(*_tmp24B)),((_tmp24B->children=Cyc_Lex_Zero(),((_tmp24B->shared_str=0,_tmp24B)))))),_tmp24A))))));}
# 1191
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
{unsigned int _tmp250;unsigned int _tmp24F;struct _dyneither_ptr _tmp24E;int*_tmp24D;unsigned int _tmp24C;Cyc_Lex_kw_nums=((_tmp24C=(unsigned int)Cyc_Lex_num_kws,((_tmp24D=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp24C)),((_tmp24E=_tag_dyneither(_tmp24D,sizeof(int),_tmp24C),((((_tmp24F=_tmp24C,_tmp251(& _tmp250,& _tmp24F,& _tmp24D))),_tmp24E))))))));}{
unsigned int i=0;
unsigned int rwsze=71;
{unsigned int j=0;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp15A=(Cyc_Lex_rw_array[(int)j]).f1;
Cyc_Lex_str_index(_tmp15A,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp15A));
*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;
++ i;}}}
# 1203
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};
# 1187
;_pop_dynregion(typedefs_rgn);};};
# 1178
;_pop_dynregion(id_rgn);};};}
