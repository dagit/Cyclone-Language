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
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 138
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 970 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1167
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1169
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 252 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 39
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 58
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 62
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 64
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 70
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 78
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 81
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 86
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 89
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 94
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 96
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 98
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 104
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 110
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 81
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 120
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 122
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 127
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 162
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 165
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 168
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 171
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 177
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 180
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 182
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 204 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 208
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 213
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple15 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple15 pr1,struct _tuple15 pr2);
# 219
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 224
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 62 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 66
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 74
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 79
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp688;struct Cyc_NewControlFlow_CFStmtAnnot _tmp687;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp686;enclosee->annot=(void*)((_tmp686=_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((_tmp688.tag=Cyc_NewControlFlow_CFAnnot,((_tmp688.f1=((_tmp687.encloser=encloser,((_tmp687.visited=0,_tmp687)))),_tmp688)))),_tmp686))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 110 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple16{void*f1;int f2;};
# 118
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple16*);
# 120
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 127
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 130
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_stmttmp0=s->annot;void*_tmp3=_stmttmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp68E;const char*_tmp68D;struct Cyc_Core_Impossible_exn_struct*_tmp68C;(int)_throw((void*)((_tmp68C=_cycalloc(sizeof(*_tmp68C)),((_tmp68C[0]=((_tmp68E.tag=Cyc_Core_Impossible,((_tmp68E.f1=((_tmp68D="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp68D,sizeof(char),37))),_tmp68E)))),_tmp68C)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp68F;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp68F=_region_malloc(env->r,sizeof(*_tmp68F)),((_tmp68F[0]=Cyc_CfFlowInfo_BottomFL(),_tmp68F)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 144
return*sflow;}struct _tuple19{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 147
static struct _tuple19 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 155
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 161
_tmpA->visited=env->iteration_num;{
struct _tuple19 _tmp690;return(_tmp690.f1=_tmpA,((_tmp690.f2=_tmpB,_tmp690)));};}
# 171
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 179
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple20{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 186
static void Cyc_NewControlFlow_check_unique_root(struct _tuple20*ckenv,void*root,void*rval){
# 188
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple20*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp13;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp12=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 191
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp13->type)){
retry: {void*_tmp14=rval;void*_tmp16;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 8)goto _LLD;else{_tmp16=(void*)_tmp15->f2;}}_LLC:
 rval=_tmp16;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp18->tag != 3)goto _LL11;else{if(_tmp18->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp19->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 200
{const char*_tmp694;void*_tmp693[1];struct Cyc_String_pa_PrintArg_struct _tmp692;(_tmp692.tag=0,((_tmp692.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp693[0]=& _tmp692,Cyc_Tcutil_warn(_tmpF,((_tmp694="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp694,sizeof(char),60))),_tag_dyneither(_tmp693,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 204
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 212
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1D=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1E=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1F=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1E);
# 218
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple18 _tmp695;struct _tuple18 _stmttmp1=(_tmp695.f1=flow,((_tmp695.f2=_tmp1F,_tmp695)));struct _tuple18 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp696;struct _tuple20 _tmp23=(_tmp696.f1=env,((_tmp696.f2=s->loc,((_tmp696.f3=_tmp22,_tmp696)))));
((void(*)(void(*f)(struct _tuple20*,void*,void*),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp23,_tmp21);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 227
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1F,*_tmp1E)){
# 234
*_tmp1E=_tmp1F;
# 238
if(_tmp1D->visited == env->iteration_num)
# 240
env->iterate_again=1;}}
# 245
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 249
union Cyc_CfFlowInfo_FlowInfo _tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;_LL1B: if((_tmp26.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple14)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple14)(_tmp26.ReachableFL).val).f2;_LL1E:
# 252
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp699;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp698;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp698=_region_malloc(fenv->r,sizeof(*_tmp698)),((_tmp698[0]=((_tmp699.tag=0,((_tmp699.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp699)))),_tmp698)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp69C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69B;_tmp2A=(void*)((_tmp69B=_region_malloc(fenv->r,sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=8,((_tmp69C.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp69C.f2=_tmp2A,_tmp69C)))))),_tmp69B))));}
# 259
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp69D;if(Cyc_Relations_consistent_relations(((_tmp69D=_region_malloc(r,sizeof(*_tmp69D)),((_tmp69D->hd=_tmp2F,((_tmp69D->tl=assume,_tmp69D))))))))
return 0;}
# 277
return 1;}
# 280
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_stmttmp2=e->r;void*_tmp31=_stmttmp2;struct Cyc_Absyn_Exp*_tmp33;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp32=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp31;if(_tmp32->tag != 13)goto _LL22;else{_tmp33=_tmp32->f2;}}_LL21:
 return _tmp33;_LL22:;_LL23:
 return e;_LL1F:;}
# 287
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 289
union Cyc_CfFlowInfo_FlowInfo _tmp34=inflow;struct Cyc_List_List*_tmp35;_LL25: if((_tmp34.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp34.ReachableFL).tag != 2)goto _LL24;_tmp35=((struct _tuple14)(_tmp34.ReachableFL).val).f2;_LL28:
# 292
{void*_stmttmp3=Cyc_Tcutil_compress(t);void*_tmp36=_stmttmp3;union Cyc_Absyn_AggrInfoU _tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp3B;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL2C;else{_tmp38=(_tmp37->f1).aggr_info;_tmp39=(_tmp37->f1).targs;}}_LL2B: {
# 294
struct Cyc_Absyn_Aggrdecl*_tmp3C=Cyc_Absyn_get_known_aggrdecl(_tmp38);
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_decl_field(_tmp3C,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp3C->tvs,_tmp39),_tmp3E);
# 301
if(!Cyc_NewControlFlow_relns_ok(r,_tmp35,Cyc_Relations_exp2relns(temp,_tmp40))){
{const char*_tmp6A2;void*_tmp6A1[2];struct Cyc_String_pa_PrintArg_struct _tmp6A0;struct Cyc_String_pa_PrintArg_struct _tmp69F;(_tmp69F.tag=0,((_tmp69F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A0.tag=0,((_tmp6A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6A1[0]=& _tmp6A0,((_tmp6A1[1]=& _tmp69F,Cyc_CfFlowInfo_aerr(loc,((_tmp6A2="unable to prove %s, required to access %s",_tag_dyneither(_tmp6A2,sizeof(char),42))),_tag_dyneither(_tmp6A1,sizeof(void*),2)))))))))))));}
{const char*_tmp6A5;void*_tmp6A4;(_tmp6A4=0,Cyc_fprintf(Cyc_stderr,((_tmp6A5="  [recorded facts on path: ",_tag_dyneither(_tmp6A5,sizeof(char),28))),_tag_dyneither(_tmp6A4,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6A8;void*_tmp6A7;(_tmp6A7=0,Cyc_fprintf(Cyc_stderr,((_tmp6A8="]\n",_tag_dyneither(_tmp6A8,sizeof(char),3))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));};}}
# 299
;_pop_region(temp);}
# 308
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3A->tag != 12)goto _LL2E;else{_tmp3B=_tmp3A->f2;}}_LL2D: {
# 310
struct Cyc_Absyn_Aggrfield*_tmp49=Cyc_Absyn_lookup_field(_tmp3B,f);
struct Cyc_Absyn_Exp*_tmp4A=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp49))->requires_clause;
if(_tmp4A != 0){
struct _RegionHandle _tmp4B=_new_region("temp");struct _RegionHandle*temp=& _tmp4B;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp35,Cyc_Relations_exp2relns(temp,_tmp4A))){
{const char*_tmp6AD;void*_tmp6AC[2];struct Cyc_String_pa_PrintArg_struct _tmp6AB;struct Cyc_String_pa_PrintArg_struct _tmp6AA;(_tmp6AA.tag=0,((_tmp6AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6AB.tag=0,((_tmp6AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6AC[0]=& _tmp6AB,((_tmp6AC[1]=& _tmp6AA,Cyc_CfFlowInfo_aerr(loc,((_tmp6AD="unable to prove %s, required to access %s",_tag_dyneither(_tmp6AD,sizeof(char),42))),_tag_dyneither(_tmp6AC,sizeof(void*),2)))))))))))));}
{const char*_tmp6B0;void*_tmp6AF;(_tmp6AF=0,Cyc_fprintf(Cyc_stderr,((_tmp6B0="  [recorded facts on path: ",_tag_dyneither(_tmp6B0,sizeof(char),28))),_tag_dyneither(_tmp6AF,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B3;void*_tmp6B2;(_tmp6B2=0,Cyc_fprintf(Cyc_stderr,((_tmp6B3="]\n",_tag_dyneither(_tmp6B3,sizeof(char),3))),_tag_dyneither(_tmp6B2,sizeof(void*),0)));};}
# 314
;_pop_region(temp);}
# 321
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 324
goto _LL24;_LL24:;}
# 328
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp54=inflow;struct Cyc_Dict_Dict _tmp55;struct Cyc_List_List*_tmp56;_LL31: if((_tmp54.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp54.ReachableFL).tag != 2)goto _LL30;_tmp55=((struct _tuple14)(_tmp54.ReachableFL).val).f1;_tmp56=((struct _tuple14)(_tmp54.ReachableFL).val).f2;_LL34:
# 332
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp55,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6B6="expression may not be fully initialized",_tag_dyneither(_tmp6B6,sizeof(char),40))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6B7;struct _tuple0 _stmttmp4=(_tmp6B7.f1=Cyc_Tcutil_compress(t),((_tmp6B7.f2=r,_tmp6B7)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5D->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5E->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5F->tag != 8)goto _LL3E;else{_tmp60=(void*)_tmp5F->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp60);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp61->tag != 4)goto _LL40;else{if((((_tmp61->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp62=((struct _tuple3)(((_tmp61->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp63->tag != 6)goto _LL40;else{_tmp64=_tmp63->f2;}};_LL3F:
# 349
 if(_tmp62->typs == 0)
return;
_tmp66=_tmp62->typs;_tmp68=_tmp64;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp65->tag != 10)goto _LL42;else{_tmp66=_tmp65->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp67=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp67->tag != 6)goto _LL42;else{_tmp68=_tmp67->f2;}};_LL41: {
# 353
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66);
{int i=0;for(0;i < sz;(i ++,_tmp66=_tmp66->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple21*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp68,sizeof(void*),i)));}}
# 357
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp69->tag != 11)goto _LL44;else{_tmp6A=(_tmp69->f1).aggr_info;_tmp6B=(_tmp69->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp6C->tag != 6)goto _LL44;else{_tmp6D=_tmp6C->f2;}};_LL43: {
# 360
struct Cyc_Absyn_Aggrdecl*_tmp74=Cyc_Absyn_get_known_aggrdecl(_tmp6A);
if(_tmp74->impl == 0)return;{
struct Cyc_List_List*_tmp75=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp74->impl))->fields;
struct _RegionHandle _tmp76=_new_region("temp");struct _RegionHandle*temp=& _tmp76;_push_region(temp);
{struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp74->tvs,_tmp6B);
{int i=0;for(0;i < _get_dyneither_size(_tmp6D,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type;
if(_tmp77 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp77,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp6D.curr)[i]);}}
# 370
_npop_handler(0);return;}
# 364
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp6E->tag != 12)goto _LL46;else{_tmp6F=_tmp6E->f1;_tmp70=_tmp6E->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp71->tag != 6)goto _LL46;else{_tmp72=_tmp71->f2;}};_LL45:
# 373
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp70=_tmp70->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp70))->hd)->type,((void**)_tmp72.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp73->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp6BA;void*_tmp6B9;(_tmp6B9=0,Cyc_Tcutil_warn(loc,((_tmp6BA="argument may still contain unique pointers",_tag_dyneither(_tmp6BA,sizeof(char),43))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6BD;void*_tmp6BC;(_tmp6BC=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6BD="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6BD,sizeof(char),47))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 391
void*_tmp80=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp81=r;void*_tmp83;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp81;if(_tmp82->tag != 8)goto _LL52;else{_tmp83=(void*)_tmp82->f2;}}_LL51:
 r=_tmp83;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp84=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp81;if(_tmp84->tag != 5)goto _LL54;else{_tmp85=_tmp84->f1;}}_LL53:
# 395
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp80,Cyc_CfFlowInfo_lookup_place(_tmp7C,_tmp85));
goto _LL4F;_LL54:;_LL55:
# 398
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp80)){
const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_Tcutil_warn(loc,((_tmp6C0="argument may contain live unique pointers",_tag_dyneither(_tmp6C0,sizeof(char),42))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 402
struct Cyc_Dict_Dict _tmp88=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp7C,env->all_changed,r);
if(_tmp7C.t == _tmp88.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp89=Cyc_CfFlowInfo_ReachableFL(_tmp88,_tmp7D);
Cyc_NewControlFlow_update_tryflow(env,_tmp89);
return _tmp89;};};};_LL4A:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 411
static struct _tuple22 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 418
if(es == 0){
struct _tuple22 _tmp6C1;return(_tmp6C1.f1=inflow,((_tmp6C1.f2=0,_tmp6C1)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6C4;struct _tuple22 _tmp6C3;return(_tmp6C3.f1=_tmp8C,((_tmp6C3.f2=((_tmp6C4=_region_malloc(rgn,sizeof(*_tmp6C4)),((_tmp6C4->hd=_tmp8D,((_tmp6C4->tl=0,_tmp6C4)))))),_tmp6C3)));};}{
# 424
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 430
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 434
{struct Cyc_Dict_Dict*_tmp6C5;env->all_changed=((_tmp6C5=_region_malloc(env->r,sizeof(*_tmp6C5)),((_tmp6C5[0]=(env->fenv)->mt_place_set,_tmp6C5))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6C6;rvals=((_tmp6C6=_region_malloc(rgn,sizeof(*_tmp6C6)),((_tmp6C6->hd=_tmp93,((_tmp6C6->tl=0,_tmp6C6))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6C7;env->all_changed=((_tmp6C7=_region_malloc(env->r,sizeof(*_tmp6C7)),((_tmp6C7[0]=(env->fenv)->mt_place_set,_tmp6C7))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6C8;rvals=((_tmp6C8=_region_malloc(rgn,sizeof(*_tmp6C8)),((_tmp6C8->hd=_tmp98,((_tmp6C8->tl=rvals,_tmp6C8))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp97,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 446
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 449
old_inflow=inflow;
# 452
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 457
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 461
struct Cyc_Dict_Dict*_tmp6C9;env->all_changed=((_tmp6C9=_region_malloc(env->r,sizeof(*_tmp6C9)),((_tmp6C9[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6C9))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp6CA;return(_tmp6CA.f1=outflow,((_tmp6CA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6CA)));};};}
# 469
static struct _tuple15 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 472
struct _RegionHandle _tmp9C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C;_push_region(rgn);
{struct _tuple22 _stmttmp8=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 473
union Cyc_CfFlowInfo_FlowInfo _tmp9E;struct Cyc_List_List*_tmp9F;struct _tuple22 _tmp9D=_stmttmp8;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;
# 475
{union Cyc_CfFlowInfo_FlowInfo _tmpA0=_tmp9E;struct Cyc_Dict_Dict _tmpA1;_LL57: if((_tmpA0.ReachableFL).tag != 2)goto _LL59;_tmpA1=((struct _tuple14)(_tmpA0.ReachableFL).val).f1;_LL58:
# 477
 for(0;_tmp9F != 0;(_tmp9F=_tmp9F->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpA1,(void*)_tmp9F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6CD;void*_tmp6CC;(_tmp6CC=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6CD="expression may not be initialized",_tag_dyneither(_tmp6CD,sizeof(char),34))),_tag_dyneither(_tmp6CC,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp6CE;struct _tuple15 _tmpA5=(_tmp6CE.f1=_tmp9E,((_tmp6CE.f2=(env->fenv)->unknown_all,_tmp6CE)));_npop_handler(0);return _tmpA5;};}
# 473
;_pop_region(rgn);}
# 497 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 503
union Cyc_CfFlowInfo_FlowInfo _tmpA6=outflow;struct Cyc_Dict_Dict _tmpA7;struct Cyc_List_List*_tmpA8;_LL5C: if((_tmpA6.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpA6.ReachableFL).tag != 2)goto _LL5B;_tmpA7=((struct _tuple14)(_tmpA6.ReachableFL).val).f1;_tmpA8=((struct _tuple14)(_tmpA6.ReachableFL).val).f2;_LL5F: {
# 506
union Cyc_CfFlowInfo_AbsLVal _stmttmp9=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpA9=_stmttmp9;struct Cyc_CfFlowInfo_Place*_tmpAA;_LL61: if((_tmpA9.UnknownL).tag != 2)goto _LL63;_LL62:
# 510
 return outflow;_LL63: if((_tmpA9.PlaceL).tag != 1)goto _LL60;_tmpAA=(struct Cyc_CfFlowInfo_Place*)(_tmpA9.PlaceL).val;_LL64: {
# 514
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D0;nzval=(void*)((_tmp6D0=_region_malloc((env->fenv)->r,sizeof(*_tmp6D0)),((_tmp6D0[0]=((_tmp6D1.tag=8,((_tmp6D1.f1=locname,((_tmp6D1.f2=nzval,_tmp6D1)))))),_tmp6D0))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 521
return _tmpAD;};}_LL60:;}_LL5B:;}
# 530
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 533
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp6D2;return(_tmp6D2.f1=outflow,((_tmp6D2.f2=outflow,_tmp6D2)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 536
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp6D3;return(_tmp6D3.f1=outflow,((_tmp6D3.f2=outflow,_tmp6D3)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
if(locname != 0){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D6;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D5;nzval=(void*)((_tmp6D5=_region_malloc((env->fenv)->r,sizeof(*_tmp6D5)),((_tmp6D5[0]=((_tmp6D6.tag=8,((_tmp6D6.f1=locname,((_tmp6D6.f2=nzval,_tmp6D6)))))),_tmp6D5))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D8;zval=(void*)((_tmp6D8=_region_malloc((env->fenv)->r,sizeof(*_tmp6D8)),((_tmp6D8[0]=((_tmp6D9.tag=8,((_tmp6D9.f1=locname,((_tmp6D9.f2=zval,_tmp6D9)))))),_tmp6D8))));};}{
# 545
struct _tuple18 _tmp6DA;return
(_tmp6DA.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6DA.f2=
# 548
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,zval),_tmpB0),_tmp6DA)));};}_LL6A:;}_LL65:;}
# 554
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 562
static struct _tuple15 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 566
void*_stmttmpB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpBC=_stmttmpB;void*_tmpBE;union Cyc_Absyn_Constraint*_tmpBF;union Cyc_Absyn_Constraint*_tmpC0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC;if(_tmpBD->tag != 5)goto _LL72;else{_tmpBE=(_tmpBD->f1).elt_typ;_tmpBF=((_tmpBD->f1).ptr_atts).bounds;_tmpC0=((_tmpBD->f1).ptr_atts).zero_term;}}_LL71: {
# 568
union Cyc_CfFlowInfo_FlowInfo _tmpC1=f;struct Cyc_Dict_Dict _tmpC2;struct Cyc_List_List*_tmpC3;_LL75: if((_tmpC1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 570
struct _tuple15 _tmp6DB;return(_tmp6DB.f1=f,((_tmp6DB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6DB)));}_LL77: if((_tmpC1.ReachableFL).tag != 2)goto _LL74;_tmpC2=((struct _tuple14)(_tmpC1.ReachableFL).val).f1;_tmpC3=((struct _tuple14)(_tmpC1.ReachableFL).val).f2;_LL78:
# 573
 if(Cyc_Tcutil_is_bound_one(_tmpBF)){
struct Cyc_Absyn_Vardecl*locname=0;
retry: {
void*_tmpC5=r;struct Cyc_Absyn_Vardecl*_tmpC7;void*_tmpC8;struct Cyc_CfFlowInfo_Place*_tmpCC;enum Cyc_CfFlowInfo_InitLevel _tmpCF;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpC6->tag != 8)goto _LL7C;else{_tmpC7=_tmpC6->f1;_tmpC8=(void*)_tmpC6->f2;}}_LL7B:
 r=_tmpC8;locname=_tmpC7;goto retry;_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpC9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpC9->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCA->tag != 2)goto _LL80;}_LL7F:
# 580
{void*_stmttmpF=e->annot;void*_tmpD0=_stmttmpF;struct Cyc_List_List*_tmpD2;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD1=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0;if(_tmpD1->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpD2=_tmpD1->f1;}}_LL8A:
# 582
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpD2))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 586
{void*_stmttmp10=e->r;void*_tmpD3=_stmttmp10;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD4->tag != 22)goto _LL90;}_LL8F:
# 588
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6DE;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6DD;e->annot=(void*)((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6DE.tag=Cyc_CfFlowInfo_NotZero,((_tmp6DE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6DE)))),_tmp6DD))));}
goto _LL8D;_LL90:;_LL91:
# 591
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 594
goto _LL88;_LL88:;}
# 596
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCB->tag != 5)goto _LL82;else{_tmpCC=_tmpCB->f1;}}_LL81:
# 598
{void*_stmttmpD=e->annot;void*_tmpD7=_stmttmpD;struct Cyc_List_List*_tmpD9;_LL93: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD7;if(_tmpD8->tag != Cyc_CfFlowInfo_NotZero)goto _LL95;else{_tmpD9=_tmpD8->f1;}}_LL94:
# 600
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpD9))goto _LL96;
goto _LL92;_LL95:;_LL96:
# 604
{void*_stmttmpE=e->r;void*_tmpDA=_stmttmpE;_LL98: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpDB->tag != 22)goto _LL9A;}_LL99:
# 606
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E1;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E0;e->annot=(void*)((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E1)))),_tmp6E0))));}
goto _LL97;_LL9A:;_LL9B:
# 609
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 612
goto _LL92;_LL92:;}{
# 614
struct _tuple15 _tmp6E2;return(_tmp6E2.f1=f,((_tmp6E2.f2=Cyc_CfFlowInfo_lookup_place(_tmpC2,_tmpCC),_tmp6E2)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCD->tag != 0)goto _LL84;}_LL83:
# 616
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6E3;return(_tmp6E3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6E3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6E3)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCE->tag != 3)goto _LL86;else{_tmpCF=_tmpCE->f1;}}_LL85:
# 619
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpCF,locname);goto _LL87;_LL86:;_LL87:
# 621
{void*_stmttmpC=e->r;void*_tmpE0=_stmttmpC;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 22)goto _LL9F;}_LL9E:
# 623
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E6;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E5;e->annot=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E6)))),_tmp6E5))));}
goto _LL9C;_LL9F:;_LLA0:
# 626
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9C;_LL9C:;}
# 629
goto _LL79;_LL79:;}}else{
# 632
void*_stmttmp11=e->annot;void*_tmpE4=_stmttmp11;struct Cyc_List_List*_tmpE6;_LLA2: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE5=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE4;if(_tmpE5->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA4;else{_tmpE6=_tmpE5->f1;}}_LLA3:
# 634
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpE6))goto _LLA5;
goto _LLA1;_LLA4:;_LLA5:
# 637
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E9;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E8;e->annot=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E9)))),_tmp6E8))));}
goto _LLA1;_LLA1:;}{
# 641
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC2,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 643
{void*_tmpE9=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE9;if(_tmpEA->tag != 7)goto _LLAB;}_LLAA:
# 645
{const char*_tmp6EC;void*_tmp6EB;(_tmp6EB=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6EC="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6EC,sizeof(char),61))),_tag_dyneither(_tmp6EB,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 648
const char*_tmp6EF;void*_tmp6EE;(_tmp6EE=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6EF="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6EF,sizeof(char),46))),_tag_dyneither(_tmp6EE,sizeof(void*),0)));}_LLA8:;}
# 650
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 652
struct _tuple15 _tmp6F0;return(_tmp6F0.f1=f,((_tmp6F0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6F0)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 654
struct _tuple15 _tmp6F1;return(_tmp6F1.f1=f,((_tmp6F1.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_none),_tmp6F1)));}}};_LL74:;}_LL72:;_LL73: {
# 657
struct Cyc_Core_Impossible_exn_struct _tmp6F7;const char*_tmp6F6;struct Cyc_Core_Impossible_exn_struct*_tmp6F5;(int)_throw((void*)((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5[0]=((_tmp6F7.tag=Cyc_Core_Impossible,((_tmp6F7.f1=((_tmp6F6="right deref of non-pointer-type",_tag_dyneither(_tmp6F6,sizeof(char),32))),_tmp6F7)))),_tmp6F5)))));}_LL6F:;}
# 664
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 667
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 670
{void*_stmttmp13=e1->r;void*_tmpF4=_stmttmp13;void*_tmpF6;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpF5->tag != 1)goto _LLB2;else{_tmpF6=(void*)_tmpF5->f2;}}_LLB1: {
# 672
struct Cyc_Absyn_Vardecl*_tmpF7=Cyc_Tcutil_nonesc_vardecl(_tmpF6);
if(_tmpF7 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF7);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 678
goto _LLAF;}_LLB2:;_LLB3:
 goto _LLAF;_LLAF:;}{
# 682
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 688
return relns;};}
# 696
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc){
# 700
struct _tuple17 _stmttmp14=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF9;struct _tuple17 _tmpF8=_stmttmp14;_tmpF9=_tmpF8.f2;
{struct _tuple17 _tmp6F8;struct _tuple17 _stmttmp15=(_tmp6F8.f1=inflow,((_tmp6F8.f2=_tmpF9,_tmp6F8)));struct _tuple17 _tmpFA=_stmttmp15;struct Cyc_Dict_Dict _tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_CfFlowInfo_Place*_tmpFD;_LLB5: if(((_tmpFA.f1).ReachableFL).tag != 2)goto _LLB7;_tmpFB=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f1;_tmpFC=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f2;if(((_tmpFA.f2).PlaceL).tag != 1)goto _LLB7;_tmpFD=(struct Cyc_CfFlowInfo_Place*)((_tmpFA.f2).PlaceL).val;_LLB6: {
# 703
void*_tmpFE=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 707
_tmpFB=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpFB,env->all_changed,_tmpFD,_tmpFE);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpFB,_tmpFC);
# 712
goto _LLB4;}_LLB7: if(((_tmpFA.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 715
{const char*_tmp6FB;void*_tmp6FA;(_tmp6FA=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6FB="noconsume() parameters must be l-values",_tag_dyneither(_tmp6FB,sizeof(char),40))),_tag_dyneither(_tmp6FA,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 718
return inflow;}
# 723
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 731
union Cyc_CfFlowInfo_FlowInfo _tmp102=outflow;struct Cyc_Dict_Dict _tmp103;struct Cyc_List_List*_tmp104;_LLBC: if((_tmp102.BottomFL).tag != 1)goto _LLBE;_LLBD: {
# 734
struct _tuple15 _tmp6FC;return(_tmp6FC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6FC.f2=rval,_tmp6FC)));}_LLBE: if((_tmp102.ReachableFL).tag != 2)goto _LLBB;_tmp103=((struct _tuple14)(_tmp102.ReachableFL).val).f1;_tmp104=((struct _tuple14)(_tmp102.ReachableFL).val).f2;_LLBF: {
# 736
union Cyc_CfFlowInfo_AbsLVal _tmp106=lval;struct Cyc_CfFlowInfo_Place*_tmp107;_LLC1: if((_tmp106.PlaceL).tag != 1)goto _LLC3;_tmp107=(struct Cyc_CfFlowInfo_Place*)(_tmp106.PlaceL).val;_LLC2: {
# 738
struct Cyc_Dict_Dict _tmp108=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp103,env->all_changed,_tmp107,rval);
_tmp104=Cyc_Relations_reln_assign_exp(fenv->r,_tmp104,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp108,_tmp104);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 744
void*_stmttmp16=Cyc_CfFlowInfo_lookup_place(_tmp103,_tmp107);void*_tmp109=_stmttmp16;_LLC6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10A->tag != 3)goto _LLC8;else{if(_tmp10A->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;}}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10B->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10C->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10D->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 3)goto _LLCE;else{if(_tmp10E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCE;}}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10F->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 7)goto _LLD0;}}_LLCF:
 goto _LLD1;_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp111=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp111->tag != 8)goto _LLD2;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp112=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp111->f2);if(_tmp112->tag != 0)goto _LLD2;}}_LLD1:
 goto _LLC5;_LLD2:;_LLD3:
# 752
{const char*_tmp6FF;void*_tmp6FE;(_tmp6FE=0,Cyc_Tcutil_warn(lexp->loc,((_tmp6FF="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp6FF,sizeof(char),43))),_tag_dyneither(_tmp6FE,sizeof(void*),0)));}
goto _LLC5;_LLC5:;}
# 757
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 759
struct _tuple15 _tmp700;return(_tmp700.f1=outflow,((_tmp700.f2=rval,_tmp700)));};}_LLC3: if((_tmp106.UnknownL).tag != 2)goto _LLC0;_LLC4: {
# 762
struct _tuple15 _tmp701;return(_tmp701.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp701.f2=rval,_tmp701)));}_LLC0:;}_LLBB:;}
# 769
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 775
union Cyc_CfFlowInfo_FlowInfo _tmp117=f;struct Cyc_Dict_Dict _tmp118;struct Cyc_List_List*_tmp119;_LLD5: if((_tmp117.BottomFL).tag != 1)goto _LLD7;_LLD6:
 return Cyc_CfFlowInfo_BottomFL();_LLD7: if((_tmp117.ReachableFL).tag != 2)goto _LLD4;_tmp118=((struct _tuple14)(_tmp117.ReachableFL).val).f1;_tmp119=((struct _tuple14)(_tmp117.ReachableFL).val).f2;_LLD8:
# 778
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp707;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp706;struct Cyc_CfFlowInfo_Place*_tmp705;_tmp118=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp118,env->all_changed,(
(_tmp705=_region_malloc(env->r,sizeof(*_tmp705)),((_tmp705->root=(void*)((_tmp707=_region_malloc(env->r,sizeof(*_tmp707)),((_tmp707[0]=((_tmp706.tag=0,((_tmp706.f1=vd,_tmp706)))),_tmp707)))),((_tmp705->fields=0,_tmp705)))))),rval);}
# 781
_tmp119=Cyc_Relations_reln_assign_var(env->r,_tmp119,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11D=Cyc_CfFlowInfo_ReachableFL(_tmp118,_tmp119);
Cyc_NewControlFlow_update_tryflow(env,_tmp11D);
# 788
return _tmp11D;};_LLD4:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 792
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 799
if(vds == 0)return inflow;{
# 802
struct Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp708;es=((_tmp708=_cycalloc(sizeof(*_tmp708)),((_tmp708->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp708->tl=es,_tmp708))))));}}}
# 808
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11E,fenv->unknown_all,pat_loc,name_locs);
# 810
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 814
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp121;void*_tmp122;struct _tuple15 _tmp120=_stmttmp17;_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp121,_tmp122);}}{
# 822
struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp123 != 0;_tmp123=_tmp123->tl){
struct _tuple23*_stmttmp18=(struct _tuple23*)_tmp123->hd;struct Cyc_Absyn_Vardecl**_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct _tuple23*_tmp124=_stmttmp18;_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;
if(_tmp125 != 0  && _tmp126 != 0){
# 829
if(_tmp126->topt == 0){
const char*_tmp70C;void*_tmp70B[1];struct Cyc_String_pa_PrintArg_struct _tmp70A;(_tmp70A.tag=0,((_tmp70A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp126)),((_tmp70B[0]=& _tmp70A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp70C="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp70C,sizeof(char),41))),_tag_dyneither(_tmp70B,sizeof(void*),1)))))));}
# 834
Cyc_fflush(Cyc_stderr);{
# 836
struct Cyc_List_List _tmp70D;struct Cyc_List_List l=(_tmp70D.hd=_tmp126,((_tmp70D.tl=0,_tmp70D)));
union Cyc_CfFlowInfo_FlowInfo _tmp12A=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple15 _stmttmp19=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp12A,_tmp126);union Cyc_CfFlowInfo_FlowInfo _tmp12C;void*_tmp12D;struct _tuple15 _tmp12B=_stmttmp19;_tmp12C=_tmp12B.f1;_tmp12D=_tmp12B.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp710;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp70F;_tmp12D=(void*)((_tmp70F=_region_malloc(fenv->r,sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp710.tag=8,((_tmp710.f1=*_tmp125,((_tmp710.f2=_tmp12D,_tmp710)))))),_tmp70F))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12C,*_tmp125,_tmp126,_tmp12D,pat_loc);};}}
# 845
return inflow;};};}
# 848
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 850
void*_stmttmp1A=e->r;void*_tmp131=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp132->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp133=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp132->f2);if(_tmp133->tag != 4)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp134->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp135=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp134->f2);if(_tmp135->tag != 3)goto _LLDE;}}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp136->tag != 1)goto _LLE0;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp137=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp136->f2);if(_tmp137->tag != 5)goto _LLE0;}}_LLDF:
 return 1;_LLE0: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp138->tag != 19)goto _LLE2;else{_tmp139=_tmp138->f1;}}_LLE1:
 _tmp13B=_tmp139;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13A->tag != 20)goto _LLE4;else{_tmp13B=_tmp13A->f1;}}_LLE3:
 _tmp13D=_tmp13B;goto _LLE5;_LLE4: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13C->tag != 21)goto _LLE6;else{_tmp13D=_tmp13C->f1;}}_LLE5:
# 857
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13D);_LLE6: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13E->tag != 22)goto _LLE8;else{_tmp13F=_tmp13E->f1;}}_LLE7: {
# 859
void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(_tmp13F->topt));void*_tmp140=_stmttmp1B;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp140;if(_tmp141->tag != 10)goto _LLED;}_LLEC:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13F);_LLED:;_LLEE:
 return 0;_LLEA:;}_LLE8:;_LLE9:
# 864
 return 0;_LLD9:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7C3(unsigned int*_tmp7C2,unsigned int*_tmp7C1,void***_tmp7BF,struct Cyc_List_List**_tmp320){for(*_tmp7C2=0;*_tmp7C2 < *_tmp7C1;(*_tmp7C2)++){
# 1447 "new_control_flow.cyc"
void*_tmp7BD;(*_tmp7BF)[*_tmp7C2]=(void*)((_tmp7BD=(void*)((struct Cyc_List_List*)_check_null(*_tmp320))->hd,((*_tmp320=(*_tmp320)->tl,_tmp7BD))));}}
# 869 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 873
struct Cyc_CfFlowInfo_FlowEnv*_tmp142=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 886
{union Cyc_CfFlowInfo_FlowInfo _tmp143=inflow;struct Cyc_Dict_Dict _tmp144;struct Cyc_List_List*_tmp145;_LLF0: if((_tmp143.BottomFL).tag != 1)goto _LLF2;_LLF1: {
struct _tuple15 _tmp711;return(_tmp711.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp711.f2=_tmp142->unknown_all,_tmp711)));}_LLF2: if((_tmp143.ReachableFL).tag != 2)goto _LLEF;_tmp144=((struct _tuple14)(_tmp143.ReachableFL).val).f1;_tmp145=((struct _tuple14)(_tmp143.ReachableFL).val).f2;_LLF3:
 d=_tmp144;relns=_tmp145;_LLEF:;}
# 901 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 922 "new_control_flow.cyc"
struct _tuple17 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp148;union Cyc_CfFlowInfo_AbsLVal _tmp149;struct _tuple17 _tmp147=_stmttmp1C;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;{
struct _tuple17 _tmp712;struct _tuple17 _stmttmp1D=(_tmp712.f1=_tmp148,((_tmp712.f2=_tmp149,_tmp712)));struct _tuple17 _tmp14A=_stmttmp1D;struct Cyc_Dict_Dict _tmp14B;struct Cyc_List_List*_tmp14C;struct Cyc_CfFlowInfo_Place*_tmp14D;_LLF5: if(((_tmp14A.f1).ReachableFL).tag != 2)goto _LLF7;_tmp14B=((struct _tuple14)((_tmp14A.f1).ReachableFL).val).f1;_tmp14C=((struct _tuple14)((_tmp14A.f1).ReachableFL).val).f2;if(((_tmp14A.f2).PlaceL).tag != 1)goto _LLF7;_tmp14D=(struct Cyc_CfFlowInfo_Place*)((_tmp14A.f2).PlaceL).val;_LLF6: {
# 925
void*_tmp14E=Cyc_CfFlowInfo_lookup_place(_tmp14B,_tmp14D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp14E,& needs_unconsume)){
{const char*_tmp715;void*_tmp714;(_tmp714=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp715="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp715,sizeof(char),71))),_tag_dyneither(_tmp714,sizeof(void*),0)));}{
struct _tuple15 _tmp716;return(_tmp716.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp716.f2=_tmp142->unknown_all,_tmp716)));};}else{
# 931
if(needs_unconsume){
# 933
struct _tuple15 _tmp717;return(_tmp717.f1=_tmp148,((_tmp717.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp142,_tmp14E),_tmp717)));}else{
# 936
void*_tmp153=Cyc_CfFlowInfo_make_unique_consumed(_tmp142,(void*)_check_null(e->topt),e,env->iteration_num,_tmp14E);
struct Cyc_Dict_Dict _tmp154=Cyc_CfFlowInfo_assign_place(_tmp142,e->loc,_tmp14B,env->all_changed,_tmp14D,_tmp153);
# 947
struct _tuple15 _tmp718;return(_tmp718.f1=Cyc_CfFlowInfo_ReachableFL(_tmp154,_tmp14C),((_tmp718.f2=_tmp14E,_tmp718)));}}}_LLF7:;_LLF8:
# 949
 goto _LLF4;_LLF4:;};}{
# 952
void*_stmttmp1E=e->r;void*_tmp157=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp170;struct Cyc_Absyn_Vardecl*_tmp173;struct Cyc_Absyn_Vardecl*_tmp176;enum Cyc_Absyn_Primop _tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_List_List*_tmp189;int _tmp18B;struct Cyc_Absyn_Exp*_tmp18C;void**_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;int _tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct _dyneither_ptr*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct _dyneither_ptr*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;struct _dyneither_ptr*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Datatypedecl*_tmp1B2;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B8;enum Cyc_Absyn_AggrKind _tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C2;struct Cyc_Absyn_Exp*_tmp1C4;void*_tmp1C5;int _tmp1C6;struct Cyc_Absyn_Stmt*_tmp1C8;_LLFA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp158->tag != 13)goto _LLFC;else{_tmp159=_tmp158->f2;if(_tmp158->f4 != Cyc_Absyn_NonNull_to_Null)goto _LLFC;}}_LLFB: {
# 956
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp159);union Cyc_CfFlowInfo_FlowInfo _tmp1D0;void*_tmp1D1;struct _tuple15 _tmp1CF=_stmttmp4F;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;{
struct _tuple15 _stmttmp50=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D0,_tmp159,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp1D3;void*_tmp1D4;struct _tuple15 _tmp1D2=_stmttmp50;_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;{
struct _tuple15 _tmp719;return(_tmp719.f1=_tmp1D3,((_tmp719.f2=_tmp1D1,_tmp719)));};};}_LLFC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15A->tag != 13)goto _LLFE;else{_tmp15B=_tmp15A->f2;}}_LLFD:
# 962
 _tmp15D=_tmp15B;goto _LLFF;_LLFE: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15C->tag != 11)goto _LL100;else{_tmp15D=_tmp15C->f1;}}_LLFF:
 _tmp15F=_tmp15D;goto _LL101;_LL100: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15E->tag != 12)goto _LL102;else{_tmp15F=_tmp15E->f1;}}_LL101:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15F);_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp160->tag != 0)goto _LL104;else{if(((_tmp160->f1).Null_c).tag != 1)goto _LL104;}}_LL103:
# 966
 goto _LL105;_LL104: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp161->tag != 0)goto _LL106;else{if(((_tmp161->f1).Int_c).tag != 5)goto _LL106;if(((struct _tuple7)((_tmp161->f1).Int_c).val).f2 != 0)goto _LL106;}}_LL105: {
struct _tuple15 _tmp71A;return(_tmp71A.f1=inflow,((_tmp71A.f2=_tmp142->zero,_tmp71A)));}_LL106: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp162->tag != 0)goto _LL108;else{if(((_tmp162->f1).Int_c).tag != 5)goto _LL108;}}_LL107:
 goto _LL109;_LL108: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp163->tag != 1)goto _LL10A;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp164=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp163->f2);if(_tmp164->tag != 2)goto _LL10A;}}_LL109: {
struct _tuple15 _tmp71B;return(_tmp71B.f1=inflow,((_tmp71B.f2=_tmp142->notzeroall,_tmp71B)));}_LL10A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp165->tag != 30)goto _LL10C;else{if(_tmp165->f1 != 0)goto _LL10C;}}_LL10B:
# 971
 goto _LL10D;_LL10C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp166->tag != 0)goto _LL10E;}_LL10D:
 goto _LL10F;_LL10E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp167->tag != 17)goto _LL110;}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp168->tag != 16)goto _LL112;}_LL111:
 goto _LL113;_LL112: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp169->tag != 18)goto _LL114;}_LL113:
 goto _LL115;_LL114: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16A->tag != 32)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16B->tag != 31)goto _LL118;}_LL117: {
struct _tuple15 _tmp71C;return(_tmp71C.f1=inflow,((_tmp71C.f2=_tmp142->unknown_all,_tmp71C)));}_LL118: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16C->tag != 1)goto _LL11A;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp16C->f2);if(_tmp16D->tag != 1)goto _LL11A;}}_LL119: {
# 980
struct _tuple15 _tmp71D;return(_tmp71D.f1=inflow,((_tmp71D.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp71D)));}_LL11A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16E->tag != 1)goto _LL11C;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 3)goto _LL11C;else{_tmp170=_tmp16F->f1;}}}_LL11B:
# 983
 _tmp173=_tmp170;goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp171->tag != 1)goto _LL11E;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 4)goto _LL11E;else{_tmp173=_tmp172->f1;}}}_LL11D:
 _tmp176=_tmp173;goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp174->tag != 1)goto _LL120;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp175=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp174->f2);if(_tmp175->tag != 5)goto _LL120;else{_tmp176=_tmp175->f1;}}}_LL11F: {
# 987
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp71E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp71E.tag=0,((_tmp71E.f1=_tmp176,_tmp71E)));
struct _tuple15 _tmp71F;return(_tmp71F.f1=inflow,((_tmp71F.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp71F)));}_LL120: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp177->tag != 2)goto _LL122;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}_LL121: {
# 994
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp179);union Cyc_CfFlowInfo_FlowInfo _tmp1DD;void*_tmp1DE;struct _tuple15 _tmp1DC=_stmttmp4E;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;{
struct _tuple15 _tmp720;return(_tmp720.f1=_tmp1DD,((_tmp720.f2=_tmp1DE,_tmp720)));};}_LL122: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17A->tag != 4)goto _LL124;else{_tmp17B=_tmp17A->f1;}}_LL123: {
# 998
struct Cyc_List_List _tmp721;struct Cyc_List_List _tmp1E0=(_tmp721.hd=_tmp17B,((_tmp721.tl=0,_tmp721)));
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1E0);union Cyc_CfFlowInfo_FlowInfo _tmp1E2;struct _tuple15 _tmp1E1=_stmttmp4B;_tmp1E2=_tmp1E1.f1;{
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E2,0,0,_tmp17B);union Cyc_CfFlowInfo_AbsLVal _tmp1E4;struct _tuple17 _tmp1E3=_stmttmp4C;_tmp1E4=_tmp1E3.f2;
{struct _tuple24 _tmp722;struct _tuple24 _stmttmp4D=(_tmp722.f1=_tmp1E4,((_tmp722.f2=_tmp1E2,_tmp722)));struct _tuple24 _tmp1E5=_stmttmp4D;struct Cyc_CfFlowInfo_Place*_tmp1E6;struct Cyc_Dict_Dict _tmp1E7;struct Cyc_List_List*_tmp1E8;_LL163: if(((_tmp1E5.f1).PlaceL).tag != 1)goto _LL165;_tmp1E6=(struct Cyc_CfFlowInfo_Place*)((_tmp1E5.f1).PlaceL).val;if(((_tmp1E5.f2).ReachableFL).tag != 2)goto _LL165;_tmp1E7=((struct _tuple14)((_tmp1E5.f2).ReachableFL).val).f1;_tmp1E8=((struct _tuple14)((_tmp1E5.f2).ReachableFL).val).f2;_LL164:
# 1003
 _tmp1E8=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp1E8,_tmp17B);
_tmp1E2=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp142,_tmp17B->loc,_tmp1E7,env->all_changed,_tmp1E6,_tmp142->unknown_all),_tmp1E8);
# 1007
goto _LL162;_LL165:;_LL166:
 goto _LL162;_LL162:;}{
# 1010
struct _tuple15 _tmp723;return(_tmp723.f1=_tmp1E2,((_tmp723.f2=_tmp142->unknown_all,_tmp723)));};};}_LL124: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17C->tag != 3)goto _LL126;else{_tmp17D=_tmp17C->f1;if(_tmp17C->f2 == 0)goto _LL126;_tmp17E=_tmp17C->f3;}}_LL125:
# 1013
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp726;void*_tmp725;(_tmp725=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp726="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp726,sizeof(char),58))),_tag_dyneither(_tmp725,sizeof(void*),0)));}{
struct _tuple15 _tmp727;return(_tmp727.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp727.f2=_tmp142->unknown_all,_tmp727)));};}{
# 1017
struct Cyc_List_List _tmp728;struct Cyc_List_List _tmp1EF=(_tmp728.hd=_tmp17E,((_tmp728.tl=0,_tmp728)));
struct Cyc_List_List _tmp729;struct Cyc_List_List _tmp1F0=(_tmp729.hd=_tmp17D,((_tmp729.tl=& _tmp1EF,_tmp729)));
struct _tuple15 _stmttmp49=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F0);union Cyc_CfFlowInfo_FlowInfo _tmp1F2;struct _tuple15 _tmp1F1=_stmttmp49;_tmp1F2=_tmp1F1.f1;{
struct _tuple17 _stmttmp4A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F2,0,0,_tmp17D);union Cyc_CfFlowInfo_AbsLVal _tmp1F4;struct _tuple17 _tmp1F3=_stmttmp4A;_tmp1F4=_tmp1F3.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1F5=_tmp1F2;struct Cyc_Dict_Dict _tmp1F6;struct Cyc_List_List*_tmp1F7;_LL168: if((_tmp1F5.ReachableFL).tag != 2)goto _LL16A;_tmp1F6=((struct _tuple14)(_tmp1F5.ReachableFL).val).f1;_tmp1F7=((struct _tuple14)(_tmp1F5.ReachableFL).val).f2;_LL169:
# 1023
{union Cyc_CfFlowInfo_AbsLVal _tmp1F8=_tmp1F4;struct Cyc_CfFlowInfo_Place*_tmp1F9;_LL16D: if((_tmp1F8.PlaceL).tag != 1)goto _LL16F;_tmp1F9=(struct Cyc_CfFlowInfo_Place*)(_tmp1F8.PlaceL).val;_LL16E:
# 1027
 _tmp1F7=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp1F7,_tmp17D);
_tmp1F6=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp17D->loc,_tmp1F6,env->all_changed,_tmp1F9,_tmp142->unknown_all);
# 1030
_tmp1F2=Cyc_CfFlowInfo_ReachableFL(_tmp1F6,_tmp1F7);
# 1033
goto _LL16C;_LL16F: if((_tmp1F8.UnknownL).tag != 2)goto _LL16C;_LL170:
# 1036
 goto _LL16C;_LL16C:;}
# 1038
goto _LL167;_LL16A:;_LL16B:
 goto _LL167;_LL167:;}{
# 1041
struct _tuple15 _tmp72A;return(_tmp72A.f1=_tmp1F2,((_tmp72A.f2=_tmp142->unknown_all,_tmp72A)));};};};_LL126: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17F->tag != 3)goto _LL128;else{_tmp180=_tmp17F->f1;if(_tmp17F->f2 != 0)goto _LL128;_tmp181=_tmp17F->f3;}}_LL127:
# 1045
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72D;void*_tmp72C;(_tmp72C=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp72D="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72D,sizeof(char),58))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}{
struct _tuple15 _tmp72E;return(_tmp72E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp72E.f2=_tmp142->unknown_all,_tmp72E)));};}{
# 1049
struct Cyc_Dict_Dict*_tmp200=env->all_changed;
# 1051
{struct Cyc_Absyn_Exp*_tmp72F[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp72F[1]=_tmp181,((_tmp72F[0]=_tmp180,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp72F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp730;env->all_changed=((_tmp730=_region_malloc(env->r,sizeof(*_tmp730)),((_tmp730[0]=_tmp142->mt_place_set,_tmp730))));}{
struct _tuple17 _stmttmp47=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp180);union Cyc_CfFlowInfo_FlowInfo _tmp204;union Cyc_CfFlowInfo_AbsLVal _tmp205;struct _tuple17 _tmp203=_stmttmp47;_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;{
struct Cyc_Dict_Dict _tmp206=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp731;env->all_changed=((_tmp731=_region_malloc(env->r,sizeof(*_tmp731)),((_tmp731[0]=_tmp142->mt_place_set,_tmp731))));}{
struct _tuple15 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp181);union Cyc_CfFlowInfo_FlowInfo _tmp209;void*_tmp20A;struct _tuple15 _tmp208=_stmttmp48;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;{
struct Cyc_Dict_Dict _tmp20B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp20C=Cyc_CfFlowInfo_after_flow(_tmp142,& _tmp206,_tmp204,_tmp209,_tmp206,_tmp20B);
# 1062
union Cyc_CfFlowInfo_FlowInfo _tmp20D=Cyc_CfFlowInfo_join_flow(_tmp142,_tmp200,inflow,_tmp20C);
# 1065
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20D,inflow)){
if(_tmp200 == 0)
env->all_changed=0;else{
# 1069
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp200,
Cyc_CfFlowInfo_union_place_set(_tmp206,_tmp20B,0),0);}
# 1072
return Cyc_NewControlFlow_do_assign(_tmp142,env,_tmp20C,_tmp180,_tmp205,_tmp181,_tmp20A,e->loc);}
# 1075
inflow=_tmp20D;};};};};}};_LL128: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp182->tag != 8)goto _LL12A;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL129: {
# 1080
struct _tuple15 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp183);union Cyc_CfFlowInfo_FlowInfo _tmp20F;void*_tmp210;struct _tuple15 _tmp20E=_stmttmp46;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp20F,_tmp184);}_LL12A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp185->tag != 10)goto _LL12C;else{_tmp186=_tmp185->f1;}}_LL12B: {
# 1084
struct _tuple15 _stmttmp45=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp186);union Cyc_CfFlowInfo_FlowInfo _tmp212;void*_tmp213;struct _tuple15 _tmp211=_stmttmp45;_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp186->loc,_tmp212,_tmp213);{
struct _tuple15 _tmp732;return(_tmp732.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp732.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp732)));};}_LL12C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp187->tag != 9)goto _LL12E;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LL12D: {
# 1089
struct _RegionHandle _tmp215=_new_region("temp");struct _RegionHandle*temp=& _tmp215;_push_region(temp);
{struct Cyc_List_List*_tmp733;struct _tuple22 _stmttmp3F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp733=_region_malloc(temp,sizeof(*_tmp733)),((_tmp733->hd=_tmp188,((_tmp733->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp189),_tmp733)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp217;struct Cyc_List_List*_tmp218;struct _tuple22 _tmp216=_stmttmp3F;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;{
# 1092
union Cyc_CfFlowInfo_FlowInfo _tmp219=Cyc_NewControlFlow_use_Rval(env,_tmp188->loc,_tmp217,(void*)((struct Cyc_List_List*)_check_null(_tmp218))->hd);
_tmp218=_tmp218->tl;{
# 1095
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp188->topt));void*_tmp21A=_stmttmp40;void*_tmp21C;_LL172: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A;if(_tmp21B->tag != 5)goto _LL174;else{_tmp21C=(_tmp21B->f1).elt_typ;}}_LL173:
# 1100
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp21C);void*_tmp21D=_stmttmp41;struct Cyc_List_List*_tmp21F;_LL177: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 9)goto _LL179;else{_tmp21F=(_tmp21E->f1).attributes;}}_LL178:
# 1102
 for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
# 1104
void*_stmttmp42=(void*)_tmp21F->hd;void*_tmp220=_stmttmp42;int _tmp222;int _tmp224;int _tmp226;_LL17C: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp221=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp220;if(_tmp221->tag != 20)goto _LL17E;else{_tmp222=_tmp221->f1;}}_LL17D:
# 1106
{struct Cyc_List_List*_tmp734;init_params=((_tmp734=_region_malloc(temp,sizeof(*_tmp734)),((_tmp734->hd=(void*)_tmp222,((_tmp734->tl=init_params,_tmp734))))));}goto _LL17B;_LL17E: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp223=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp220;if(_tmp223->tag != 21)goto _LL180;else{_tmp224=_tmp223->f1;}}_LL17F:
# 1108
{struct Cyc_List_List*_tmp735;nolive_unique_params=((_tmp735=_region_malloc(temp,sizeof(*_tmp735)),((_tmp735->hd=(void*)_tmp224,((_tmp735->tl=nolive_unique_params,_tmp735))))));}
goto _LL17B;_LL180: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp225=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp220;if(_tmp225->tag != 22)goto _LL182;else{_tmp226=_tmp225->f1;}}_LL181:
# 1112
{struct Cyc_List_List*_tmp736;noconsume_params=((_tmp736=_region_malloc(temp,sizeof(*_tmp736)),((_tmp736->hd=(void*)_tmp226,((_tmp736->tl=noconsume_params,_tmp736))))));}
goto _LL17B;_LL182:;_LL183:
 goto _LL17B;_LL17B:;}
# 1116
goto _LL176;_LL179:;_LL17A: {
const char*_tmp739;void*_tmp738;(_tmp738=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp739="anal_Rexp: bad function type",_tag_dyneither(_tmp739,sizeof(char),29))),_tag_dyneither(_tmp738,sizeof(void*),0)));}_LL176:;}
# 1119
goto _LL171;_LL174:;_LL175: {
const char*_tmp73C;void*_tmp73B;(_tmp73B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp73C="anal_Rexp: bad function type",_tag_dyneither(_tmp73C,sizeof(char),29))),_tag_dyneither(_tmp73B,sizeof(void*),0)));}_LL171:;}
# 1122
{int i=1;for(0;_tmp218 != 0;(((_tmp218=_tmp218->tl,_tmp189=((struct Cyc_List_List*)_check_null(_tmp189))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp22E=_tmp217;struct Cyc_Dict_Dict _tmp22F;_LL185: if((_tmp22E.BottomFL).tag != 1)goto _LL187;_LL186:
 goto _LL184;_LL187: if((_tmp22E.ReachableFL).tag != 2)goto _LL184;_tmp22F=((struct _tuple14)(_tmp22E.ReachableFL).val).f1;_LL188:
# 1127
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp22F,(void*)_tmp218->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp73F;void*_tmp73E;(_tmp73E=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,((_tmp73F="expression may not be initialized",_tag_dyneither(_tmp73F,sizeof(char),34))),_tag_dyneither(_tmp73E,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp232=_tmp219;struct Cyc_Dict_Dict _tmp233;struct Cyc_List_List*_tmp234;_LL18A: if((_tmp232.BottomFL).tag != 1)goto _LL18C;_LL18B:
 goto _LL189;_LL18C: if((_tmp232.ReachableFL).tag != 2)goto _LL189;_tmp233=((struct _tuple14)(_tmp232.ReachableFL).val).f1;_tmp234=((struct _tuple14)(_tmp232.ReachableFL).val).f2;_LL18D: {
# 1134
struct Cyc_Dict_Dict _tmp235=Cyc_CfFlowInfo_escape_deref(_tmp142,_tmp233,env->all_changed,(void*)_tmp218->hd);
{void*_stmttmp43=(void*)_tmp218->hd;void*_tmp236=_stmttmp43;struct Cyc_CfFlowInfo_Place*_tmp238;_LL18F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp237=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp236;if(_tmp237->tag != 5)goto _LL191;else{_tmp238=_tmp237->f1;}}_LL190:
# 1137
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->topt));void*_tmp239=_stmttmp44;void*_tmp23B;_LL194: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239;if(_tmp23A->tag != 5)goto _LL196;else{_tmp23B=(_tmp23A->f1).elt_typ;}}_LL195:
# 1141
 _tmp235=Cyc_CfFlowInfo_assign_place(_tmp142,((struct Cyc_Absyn_Exp*)_tmp189->hd)->loc,_tmp235,env->all_changed,_tmp238,
# 1143
Cyc_CfFlowInfo_typ_to_absrval(_tmp142,_tmp23B,0,_tmp142->esc_all));
# 1145
goto _LL193;_LL196:;_LL197: {
const char*_tmp742;void*_tmp741;(_tmp741=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp742="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp742,sizeof(char),39))),_tag_dyneither(_tmp741,sizeof(void*),0)));}_LL193:;}
# 1148
goto _LL18E;_LL191:;_LL192:
 goto _LL18E;_LL18E:;}
# 1151
_tmp219=Cyc_CfFlowInfo_ReachableFL(_tmp235,_tmp234);
goto _LL189;}_LL189:;}
# 1154
goto _LL184;_LL184:;}else{
# 1157
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1162
_tmp219=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->topt),_tmp219,(void*)_tmp218->hd);else{
# 1169
_tmp219=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,_tmp219,(void*)_tmp218->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp189->hd)->topt),0))
_tmp219=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp219,(struct Cyc_Absyn_Exp*)_tmp189->hd,((struct Cyc_Absyn_Exp*)_tmp189->hd)->loc);}}}}
# 1176
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp188->topt))){
struct _tuple15 _tmp743;struct _tuple15 _tmp23F=(_tmp743.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp743.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp743)));_npop_handler(0);return _tmp23F;}else{
# 1179
struct _tuple15 _tmp744;struct _tuple15 _tmp241=(_tmp744.f1=_tmp219,((_tmp744.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp744)));_npop_handler(0);return _tmp241;}};};}
# 1090
;_pop_region(temp);}_LL12E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp18A->tag != 33)goto _LL130;else{_tmp18B=(_tmp18A->f1).is_calloc;_tmp18C=(_tmp18A->f1).rgn;_tmp18D=(_tmp18A->f1).elt_type;_tmp18E=(_tmp18A->f1).num_elts;_tmp18F=(_tmp18A->f1).fat_result;}}_LL12F: {
# 1182
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp747;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp746;void*root=(void*)((_tmp746=_region_malloc(_tmp142->r,sizeof(*_tmp746)),((_tmp746[0]=((_tmp747.tag=1,((_tmp747.f1=_tmp18E,((_tmp747.f2=(void*)_check_null(e->topt),_tmp747)))))),_tmp746))));
struct Cyc_CfFlowInfo_Place*_tmp748;struct Cyc_CfFlowInfo_Place*place=(_tmp748=_region_malloc(_tmp142->r,sizeof(*_tmp748)),((_tmp748->root=root,((_tmp748->fields=0,_tmp748)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp74B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp74A;void*rval=(void*)((_tmp74A=_region_malloc(_tmp142->r,sizeof(*_tmp74A)),((_tmp74A[0]=((_tmp74B.tag=5,((_tmp74B.f1=place,_tmp74B)))),_tmp74A))));
void*place_val;
if(_tmp18F)place_val=_tmp142->notzeroall;else{
if(_tmp18B)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,*((void**)_check_null(_tmp18D)),0,_tmp142->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,*((void**)_check_null(_tmp18D)),0,_tmp142->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp18C != 0){
struct _RegionHandle _tmp243=_new_region("temp");struct _RegionHandle*temp=& _tmp243;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp74C[2];struct _tuple22 _stmttmp3D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp74C[1]=_tmp18E,((_tmp74C[0]=_tmp18C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp74C,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1193
union Cyc_CfFlowInfo_FlowInfo _tmp245;struct Cyc_List_List*_tmp246;struct _tuple22 _tmp244=_stmttmp3D;_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;
# 1195
outflow=_tmp245;}
# 1193
;_pop_region(temp);}else{
# 1198
struct _tuple15 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18E);union Cyc_CfFlowInfo_FlowInfo _tmp249;struct _tuple15 _tmp248=_stmttmp3E;_tmp249=_tmp248.f1;
outflow=_tmp249;}{
# 1201
union Cyc_CfFlowInfo_FlowInfo _tmp24A=outflow;struct Cyc_Dict_Dict _tmp24B;struct Cyc_List_List*_tmp24C;_LL199: if((_tmp24A.BottomFL).tag != 1)goto _LL19B;_LL19A: {
struct _tuple15 _tmp74D;return(_tmp74D.f1=outflow,((_tmp74D.f2=rval,_tmp74D)));}_LL19B: if((_tmp24A.ReachableFL).tag != 2)goto _LL198;_tmp24B=((struct _tuple14)(_tmp24A.ReachableFL).val).f1;_tmp24C=((struct _tuple14)(_tmp24A.ReachableFL).val).f2;_LL19C: {
# 1204
struct _tuple15 _tmp74E;return(_tmp74E.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp24B,root,place_val),_tmp24C),((_tmp74E.f2=rval,_tmp74E)));}_LL198:;};};}_LL130: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp190->tag != 34)goto _LL132;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;}}_LL131: {
# 1208
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1214
struct _RegionHandle _tmp254=_new_region("temp");struct _RegionHandle*temp=& _tmp254;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp74F[2];struct _tuple22 _stmttmp3A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp74F[1]=_tmp192,((_tmp74F[0]=_tmp191,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp74F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1215
union Cyc_CfFlowInfo_FlowInfo _tmp256;struct Cyc_List_List*_tmp257;struct _tuple22 _tmp255=_stmttmp3A;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;
# 1217
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp257))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp257->tl))->hd;
outflow=_tmp256;}{
# 1222
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp191);union Cyc_CfFlowInfo_AbsLVal _tmp25A;struct _tuple17 _tmp259=_stmttmp3B;_tmp25A=_tmp259.f2;{
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp192);union Cyc_CfFlowInfo_AbsLVal _tmp25C;struct _tuple17 _tmp25B=_stmttmp3C;_tmp25C=_tmp25B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp25D=outflow;struct Cyc_Dict_Dict _tmp25E;struct Cyc_List_List*_tmp25F;_LL19E: if((_tmp25D.ReachableFL).tag != 2)goto _LL1A0;_tmp25E=((struct _tuple14)(_tmp25D.ReachableFL).val).f1;_tmp25F=((struct _tuple14)(_tmp25D.ReachableFL).val).f2;_LL19F:
# 1226
{union Cyc_CfFlowInfo_AbsLVal _tmp260=_tmp25A;struct Cyc_CfFlowInfo_Place*_tmp261;_LL1A3: if((_tmp260.PlaceL).tag != 1)goto _LL1A5;_tmp261=(struct Cyc_CfFlowInfo_Place*)(_tmp260.PlaceL).val;_LL1A4:
# 1228
 _tmp25E=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp191->loc,_tmp25E,env->all_changed,_tmp261,right_rval);
# 1230
goto _LL1A2;_LL1A5: if((_tmp260.UnknownL).tag != 2)goto _LL1A2;_LL1A6:
# 1235
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp25E,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp752;void*_tmp751;(_tmp751=0,Cyc_Tcutil_terr(_tmp192->loc,((_tmp752="expression may not be fully initialized",_tag_dyneither(_tmp752,sizeof(char),40))),_tag_dyneither(_tmp751,sizeof(void*),0)));}
goto _LL1A2;_LL1A2:;}
# 1239
{union Cyc_CfFlowInfo_AbsLVal _tmp264=_tmp25C;struct Cyc_CfFlowInfo_Place*_tmp265;_LL1A8: if((_tmp264.PlaceL).tag != 1)goto _LL1AA;_tmp265=(struct Cyc_CfFlowInfo_Place*)(_tmp264.PlaceL).val;_LL1A9:
# 1241
 _tmp25E=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp192->loc,_tmp25E,env->all_changed,_tmp265,left_rval);
# 1243
goto _LL1A7;_LL1AA: if((_tmp264.UnknownL).tag != 2)goto _LL1A7;_LL1AB:
# 1245
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp25E,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp755;void*_tmp754;(_tmp754=0,Cyc_Tcutil_terr(_tmp191->loc,((_tmp755="expression may not be fully initialized",_tag_dyneither(_tmp755,sizeof(char),40))),_tag_dyneither(_tmp754,sizeof(void*),0)));}
goto _LL1A7;_LL1A7:;}
# 1250
_tmp25F=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp25F,_tmp191);
_tmp25F=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp25F,_tmp192);
# 1253
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp25E,_tmp25F);
goto _LL19D;_LL1A0:;_LL1A1:
 goto _LL19D;_LL19D:;}{
# 1259
struct _tuple15 _tmp756;struct _tuple15 _tmp269=(_tmp756.f1=outflow,((_tmp756.f2=_tmp142->unknown_all,_tmp756)));_npop_handler(0);return _tmp269;};};};
# 1214
;_pop_region(temp);}_LL132: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp193->tag != 15)goto _LL134;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;}}_LL133: {
# 1262
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp759;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp758;void*root=(void*)((_tmp758=_region_malloc(_tmp142->r,sizeof(*_tmp758)),((_tmp758[0]=((_tmp759.tag=1,((_tmp759.f1=_tmp195,((_tmp759.f2=(void*)_check_null(e->topt),_tmp759)))))),_tmp758))));
struct Cyc_CfFlowInfo_Place*_tmp75A;struct Cyc_CfFlowInfo_Place*place=(_tmp75A=_region_malloc(_tmp142->r,sizeof(*_tmp75A)),((_tmp75A->root=root,((_tmp75A->fields=0,_tmp75A)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp75D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp75C;void*rval=(void*)((_tmp75C=_region_malloc(_tmp142->r,sizeof(*_tmp75C)),((_tmp75C[0]=((_tmp75D.tag=5,((_tmp75D.f1=place,_tmp75D)))),_tmp75C))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp194 != 0){
struct _RegionHandle _tmp26A=_new_region("temp");struct _RegionHandle*temp=& _tmp26A;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp75E[2];struct _tuple22 _stmttmp38=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp75E[1]=_tmp195,((_tmp75E[0]=_tmp194,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp75E,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1270
union Cyc_CfFlowInfo_FlowInfo _tmp26C;struct Cyc_List_List*_tmp26D;struct _tuple22 _tmp26B=_stmttmp38;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;
# 1272
outflow=_tmp26C;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp26D))->tl))->hd;}
# 1270
;_pop_region(temp);}else{
# 1276
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp195);union Cyc_CfFlowInfo_FlowInfo _tmp270;void*_tmp271;struct _tuple15 _tmp26F=_stmttmp39;_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;
outflow=_tmp270;
place_val=_tmp271;}{
# 1280
union Cyc_CfFlowInfo_FlowInfo _tmp272=outflow;struct Cyc_Dict_Dict _tmp273;struct Cyc_List_List*_tmp274;_LL1AD: if((_tmp272.BottomFL).tag != 1)goto _LL1AF;_LL1AE: {
struct _tuple15 _tmp75F;return(_tmp75F.f1=outflow,((_tmp75F.f2=rval,_tmp75F)));}_LL1AF: if((_tmp272.ReachableFL).tag != 2)goto _LL1AC;_tmp273=((struct _tuple14)(_tmp272.ReachableFL).val).f1;_tmp274=((struct _tuple14)(_tmp272.ReachableFL).val).f2;_LL1B0: {
# 1283
struct _tuple15 _tmp760;return(_tmp760.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp273,root,place_val),_tmp274),((_tmp760.f2=rval,_tmp760)));}_LL1AC:;};};}_LL134: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp196->tag != 14)goto _LL136;else{_tmp197=_tmp196->f1;}}_LL135: {
# 1287
struct _tuple17 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp197);union Cyc_CfFlowInfo_FlowInfo _tmp27D;union Cyc_CfFlowInfo_AbsLVal _tmp27E;struct _tuple17 _tmp27C=_stmttmp37;_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp27F=_tmp27E;struct Cyc_CfFlowInfo_Place*_tmp280;_LL1B2: if((_tmp27F.UnknownL).tag != 2)goto _LL1B4;_LL1B3: {
struct _tuple15 _tmp761;return(_tmp761.f1=_tmp27D,((_tmp761.f2=_tmp142->notzeroall,_tmp761)));}_LL1B4: if((_tmp27F.PlaceL).tag != 1)goto _LL1B1;_tmp280=(struct Cyc_CfFlowInfo_Place*)(_tmp27F.PlaceL).val;_LL1B5: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp767;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp766;struct _tuple15 _tmp765;return(_tmp765.f1=_tmp27D,((_tmp765.f2=(void*)((_tmp767=_region_malloc(env->r,sizeof(*_tmp767)),((_tmp767[0]=((_tmp766.tag=5,((_tmp766.f1=_tmp280,_tmp766)))),_tmp767)))),_tmp765)));}_LL1B1:;};}_LL136: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp198->tag != 19)goto _LL138;else{_tmp199=_tmp198->f1;}}_LL137: {
# 1294
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp286;void*_tmp287;struct _tuple15 _tmp285=_stmttmp36;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp286,_tmp199,_tmp287);}_LL138: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp19A->tag != 20)goto _LL13A;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_LL139: {
# 1298
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19B);union Cyc_CfFlowInfo_FlowInfo _tmp289;void*_tmp28A;struct _tuple15 _tmp288=_stmttmp35;_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;
if(_tmp19B->topt == 0){
{const char*_tmp76B;void*_tmp76A[1];struct Cyc_String_pa_PrintArg_struct _tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp76A[0]=& _tmp769,Cyc_fprintf(Cyc_stderr,((_tmp76B="aggrmember exp %s\n",_tag_dyneither(_tmp76B,sizeof(char),19))),_tag_dyneither(_tmp76A,sizeof(void*),1)))))));}{
const char*_tmp76F;void*_tmp76E[1];struct Cyc_String_pa_PrintArg_struct _tmp76D;(_tmp76D.tag=0,((_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp19B)),((_tmp76E[0]=& _tmp76D,Cyc_fprintf(Cyc_stderr,((_tmp76F="oops! %s.topt = null!\n",_tag_dyneither(_tmp76F,sizeof(char),23))),_tag_dyneither(_tmp76E,sizeof(void*),1)))))));};}{
# 1303
void*_tmp291=(void*)_check_null(_tmp19B->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp291)){
# 1306
struct _tuple15 _tmp770;return(_tmp770.f1=_tmp289,((_tmp770.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp770)));}
# 1308
if(Cyc_Absyn_is_require_union_type(_tmp291))
Cyc_NewControlFlow_check_requires(_tmp19B->loc,_tmp142->r,_tmp291,_tmp19C,_tmp289);{
# 1311
void*_tmp293=_tmp28A;int _tmp296;int _tmp297;struct _dyneither_ptr _tmp298;int _tmp29A;int _tmp29B;struct _dyneither_ptr _tmp29C;_LL1B7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp294=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp293;if(_tmp294->tag != 8)goto _LL1B9;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp295=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp294->f2);if(_tmp295->tag != 6)goto _LL1B9;else{_tmp296=(_tmp295->f1).is_union;_tmp297=(_tmp295->f1).fieldnum;_tmp298=_tmp295->f2;}}}_LL1B8:
# 1313
 _tmp29A=_tmp296;_tmp29B=_tmp297;_tmp29C=_tmp298;goto _LL1BA;_LL1B9: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp299=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp293;if(_tmp299->tag != 6)goto _LL1BB;else{_tmp29A=(_tmp299->f1).is_union;_tmp29B=(_tmp299->f1).fieldnum;_tmp29C=_tmp299->f2;}}_LL1BA: {
# 1315
int _tmp29D=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19B->topt),_tmp19C);
if((_tmp29A  && _tmp29B != - 1) && _tmp29B != _tmp29D){
struct _tuple15 _tmp771;return(_tmp771.f1=_tmp289,((_tmp771.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),1,_tmp142->unknown_none),_tmp771)));}{
struct _tuple15 _tmp772;return(_tmp772.f1=_tmp289,((_tmp772.f2=*((void**)_check_dyneither_subscript(_tmp29C,sizeof(void*),_tmp29D)),_tmp772)));};}_LL1BB:;_LL1BC:
# 1320
{const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_fprintf(Cyc_stderr,((_tmp775="the bad rexp is :",_tag_dyneither(_tmp775,sizeof(char),18))),_tag_dyneither(_tmp774,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp28A);
{const char*_tmp778;void*_tmp777;(_tmp777=0,Cyc_fprintf(Cyc_stderr,((_tmp778="\n",_tag_dyneither(_tmp778,sizeof(char),2))),_tag_dyneither(_tmp777,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp785;const char*_tmp784;void*_tmp783[1];struct Cyc_String_pa_PrintArg_struct _tmp782;struct Cyc_Core_Impossible_exn_struct*_tmp781;(int)_throw((void*)((_tmp781=_cycalloc(sizeof(*_tmp781)),((_tmp781[0]=((_tmp785.tag=Cyc_Core_Impossible,((_tmp785.f1=(struct _dyneither_ptr)((_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp783[0]=& _tmp782,Cyc_aprintf(((_tmp784="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp784,sizeof(char),26))),_tag_dyneither(_tmp783,sizeof(void*),1)))))))),_tmp785)))),_tmp781)))));};_LL1B6:;};};}_LL13A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp19D->tag != 37)goto _LL13C;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}_LL13B: {
# 1330
struct _tuple15 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19E);union Cyc_CfFlowInfo_FlowInfo _tmp2AA;void*_tmp2AB;struct _tuple15 _tmp2A9=_stmttmp34;_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;
# 1332
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp19E->topt))){
struct _tuple15 _tmp786;return(_tmp786.f1=_tmp2AA,((_tmp786.f2=_tmp142->unknown_all,_tmp786)));}{
void*_tmp2AD=_tmp2AB;int _tmp2B0;int _tmp2B1;struct _dyneither_ptr _tmp2B2;int _tmp2B4;int _tmp2B5;struct _dyneither_ptr _tmp2B6;_LL1BE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AD;if(_tmp2AE->tag != 8)goto _LL1C0;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2AE->f2);if(_tmp2AF->tag != 6)goto _LL1C0;else{_tmp2B0=(_tmp2AF->f1).is_union;_tmp2B1=(_tmp2AF->f1).fieldnum;_tmp2B2=_tmp2AF->f2;}}}_LL1BF:
# 1336
 _tmp2B4=_tmp2B0;_tmp2B5=_tmp2B1;_tmp2B6=_tmp2B2;goto _LL1C1;_LL1C0: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD;if(_tmp2B3->tag != 6)goto _LL1C2;else{_tmp2B4=(_tmp2B3->f1).is_union;_tmp2B5=(_tmp2B3->f1).fieldnum;_tmp2B6=_tmp2B3->f2;}}_LL1C1: {
# 1338
int _tmp2B7=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19E->topt),_tmp19F);
if(_tmp2B4  && _tmp2B5 != - 1){
if(_tmp2B5 != _tmp2B7){
struct _tuple15 _tmp787;return(_tmp787.f1=_tmp2AA,((_tmp787.f2=_tmp142->zero,_tmp787)));}else{
# 1343
struct _tuple15 _tmp788;return(_tmp788.f1=_tmp2AA,((_tmp788.f2=_tmp142->notzeroall,_tmp788)));}}else{
# 1345
struct _tuple15 _tmp789;return(_tmp789.f1=_tmp2AA,((_tmp789.f2=_tmp142->unknown_all,_tmp789)));}}_LL1C2:;_LL1C3: {
# 1347
struct Cyc_Core_Impossible_exn_struct _tmp796;const char*_tmp795;void*_tmp794[1];struct Cyc_String_pa_PrintArg_struct _tmp793;struct Cyc_Core_Impossible_exn_struct*_tmp792;(int)_throw((void*)((_tmp792=_cycalloc(sizeof(*_tmp792)),((_tmp792[0]=((_tmp796.tag=Cyc_Core_Impossible,((_tmp796.f1=(struct _dyneither_ptr)((_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp794[0]=& _tmp793,Cyc_aprintf(((_tmp795="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp795,sizeof(char),26))),_tag_dyneither(_tmp794,sizeof(void*),1)))))))),_tmp796)))),_tmp792)))));}_LL1BD:;};}_LL13C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A0->tag != 21)goto _LL13E;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}}_LL13D: {
# 1352
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A1);union Cyc_CfFlowInfo_FlowInfo _tmp2C1;void*_tmp2C2;struct _tuple15 _tmp2C0=_stmttmp31;_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;{
# 1355
struct _tuple15 _stmttmp32=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C1,_tmp1A1,_tmp2C2);union Cyc_CfFlowInfo_FlowInfo _tmp2C4;void*_tmp2C5;struct _tuple15 _tmp2C3=_stmttmp32;_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;{
# 1358
void*_stmttmp33=Cyc_Tcutil_compress((void*)_check_null(_tmp1A1->topt));void*_tmp2C6=_stmttmp33;void*_tmp2C8;_LL1C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6;if(_tmp2C7->tag != 5)goto _LL1C7;else{_tmp2C8=(_tmp2C7->f1).elt_typ;}}_LL1C6:
# 1360
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C8)){
# 1362
struct _tuple15 _tmp797;return(_tmp797.f1=_tmp2C4,((_tmp797.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp797)));}
# 1364
if(Cyc_Absyn_is_require_union_type(_tmp2C8))
Cyc_NewControlFlow_check_requires(_tmp1A1->loc,_tmp142->r,_tmp2C8,_tmp1A2,_tmp2C1);{
# 1367
void*_tmp2CA=_tmp2C5;int _tmp2CD;int _tmp2CE;struct _dyneither_ptr _tmp2CF;int _tmp2D1;int _tmp2D2;struct _dyneither_ptr _tmp2D3;_LL1CA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2CB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2CA;if(_tmp2CB->tag != 8)goto _LL1CC;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2CB->f2);if(_tmp2CC->tag != 6)goto _LL1CC;else{_tmp2CD=(_tmp2CC->f1).is_union;_tmp2CE=(_tmp2CC->f1).fieldnum;_tmp2CF=_tmp2CC->f2;}}}_LL1CB:
# 1369
 _tmp2D1=_tmp2CD;_tmp2D2=_tmp2CE;_tmp2D3=_tmp2CF;goto _LL1CD;_LL1CC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D0=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2CA;if(_tmp2D0->tag != 6)goto _LL1CE;else{_tmp2D1=(_tmp2D0->f1).is_union;_tmp2D2=(_tmp2D0->f1).fieldnum;_tmp2D3=_tmp2D0->f2;}}_LL1CD: {
# 1371
int _tmp2D4=Cyc_CfFlowInfo_get_field_index(_tmp2C8,_tmp1A2);
if((_tmp2D1  && _tmp2D2 != - 1) && _tmp2D2 != _tmp2D4){
struct _tuple15 _tmp798;return(_tmp798.f1=_tmp2C4,((_tmp798.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),1,_tmp142->unknown_none),_tmp798)));}{
struct _tuple15 _tmp799;return(_tmp799.f1=_tmp2C4,((_tmp799.f2=*((void**)_check_dyneither_subscript(_tmp2D3,sizeof(void*),_tmp2D4)),_tmp799)));};}_LL1CE:;_LL1CF: {
struct Cyc_Core_Impossible_exn_struct _tmp79F;const char*_tmp79E;struct Cyc_Core_Impossible_exn_struct*_tmp79D;(int)_throw((void*)((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=((_tmp79F.tag=Cyc_Core_Impossible,((_tmp79F.f1=((_tmp79E="anal_Rexp: AggrArrow",_tag_dyneither(_tmp79E,sizeof(char),21))),_tmp79F)))),_tmp79D)))));}_LL1C9:;};_LL1C7:;_LL1C8: {
# 1377
struct Cyc_Core_Impossible_exn_struct _tmp7A5;const char*_tmp7A4;struct Cyc_Core_Impossible_exn_struct*_tmp7A3;(int)_throw((void*)((_tmp7A3=_cycalloc(sizeof(*_tmp7A3)),((_tmp7A3[0]=((_tmp7A5.tag=Cyc_Core_Impossible,((_tmp7A5.f1=((_tmp7A4="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7A4,sizeof(char),25))),_tmp7A5)))),_tmp7A3)))));}_LL1C4:;};};}_LL13E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A3->tag != 5)goto _LL140;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;_tmp1A6=_tmp1A3->f3;}}_LL13F: {
# 1381
struct _tuple18 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A4);union Cyc_CfFlowInfo_FlowInfo _tmp2DE;union Cyc_CfFlowInfo_FlowInfo _tmp2DF;struct _tuple18 _tmp2DD=_stmttmp30;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;{
struct _tuple15 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DE,_tmp1A5);
struct _tuple15 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DF,_tmp1A6);
# 1385
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2E0,_tmp2E1);};}_LL140: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A7->tag != 6)goto _LL142;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL141: {
# 1388
struct _tuple18 _stmttmp2E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A8);union Cyc_CfFlowInfo_FlowInfo _tmp2E3;union Cyc_CfFlowInfo_FlowInfo _tmp2E4;struct _tuple18 _tmp2E2=_stmttmp2E;_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;{
struct _tuple15 _stmttmp2F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E3,_tmp1A9);union Cyc_CfFlowInfo_FlowInfo _tmp2E6;void*_tmp2E7;struct _tuple15 _tmp2E5=_stmttmp2F;_tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;{
struct _tuple15 _tmp7A6;struct _tuple15 _tmp2E8=(_tmp7A6.f1=_tmp2E6,((_tmp7A6.f2=_tmp2E7,_tmp7A6)));
struct _tuple15 _tmp7A7;struct _tuple15 _tmp2E9=(_tmp7A7.f1=_tmp2E4,((_tmp7A7.f2=_tmp142->zero,_tmp7A7)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2E8,_tmp2E9);};};}_LL142: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1AA->tag != 7)goto _LL144;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}_LL143: {
# 1395
struct _tuple18 _stmttmp2C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AB);union Cyc_CfFlowInfo_FlowInfo _tmp2ED;union Cyc_CfFlowInfo_FlowInfo _tmp2EE;struct _tuple18 _tmp2EC=_stmttmp2C;_tmp2ED=_tmp2EC.f1;_tmp2EE=_tmp2EC.f2;{
struct _tuple15 _stmttmp2D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EE,_tmp1AC);union Cyc_CfFlowInfo_FlowInfo _tmp2F0;void*_tmp2F1;struct _tuple15 _tmp2EF=_stmttmp2D;_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;{
struct _tuple15 _tmp7A8;struct _tuple15 _tmp2F2=(_tmp7A8.f1=_tmp2F0,((_tmp7A8.f2=_tmp2F1,_tmp7A8)));
struct _tuple15 _tmp7A9;struct _tuple15 _tmp2F3=(_tmp7A9.f1=_tmp2ED,((_tmp7A9.f2=_tmp142->notzeroall,_tmp7A9)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2F2,_tmp2F3);};};}_LL144: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1AD->tag != 22)goto _LL146;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LL145: {
# 1402
struct _RegionHandle _tmp2F6=_new_region("temp");struct _RegionHandle*temp=& _tmp2F6;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7AA[2];struct _tuple22 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7AA[1]=_tmp1AF,((_tmp7AA[0]=_tmp1AE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7AA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1403
union Cyc_CfFlowInfo_FlowInfo _tmp2F8;struct Cyc_List_List*_tmp2F9;struct _tuple22 _tmp2F7=_stmttmp28;_tmp2F8=_tmp2F7.f1;_tmp2F9=_tmp2F7.f2;{
# 1407
union Cyc_CfFlowInfo_FlowInfo _tmp2FA=_tmp2F8;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FB=_tmp2F8;struct Cyc_Dict_Dict _tmp2FC;struct Cyc_List_List*_tmp2FD;_LL1D1: if((_tmp2FB.ReachableFL).tag != 2)goto _LL1D3;_tmp2FC=((struct _tuple14)(_tmp2FB.ReachableFL).val).f1;_tmp2FD=((struct _tuple14)(_tmp2FB.ReachableFL).val).f2;_LL1D2:
# 1412
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2FC,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F9))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_CfFlowInfo_aerr(_tmp1AF->loc,((_tmp7AD="expression may not be initialized",_tag_dyneither(_tmp7AD,sizeof(char),34))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp300=Cyc_NewControlFlow_add_subscript_reln(_tmp142->r,_tmp2FD,_tmp1AE,_tmp1AF);
if(_tmp2FD != _tmp300)
_tmp2FA=Cyc_CfFlowInfo_ReachableFL(_tmp2FC,_tmp300);
goto _LL1D0;};_LL1D3:;_LL1D4:
 goto _LL1D0;_LL1D0:;}{
# 1420
void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp1AE->topt));void*_tmp301=_stmttmp29;struct Cyc_List_List*_tmp303;union Cyc_Absyn_Constraint*_tmp305;_LL1D6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp301;if(_tmp302->tag != 10)goto _LL1D8;else{_tmp303=_tmp302->f1;}}_LL1D7: {
# 1422
void*_stmttmp2B=(void*)((struct Cyc_List_List*)_check_null(_tmp2F9))->hd;void*_tmp306=_stmttmp2B;struct _dyneither_ptr _tmp309;struct _dyneither_ptr _tmp30B;_LL1DD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp307=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp306;if(_tmp307->tag != 8)goto _LL1DF;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp308=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp307->f2);if(_tmp308->tag != 6)goto _LL1DF;else{_tmp309=_tmp308->f2;}}}_LL1DE:
# 1424
 _tmp30B=_tmp309;goto _LL1E0;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp306;if(_tmp30A->tag != 6)goto _LL1E1;else{_tmp30B=_tmp30A->f2;}}_LL1E0: {
# 1426
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1AF)).f1;
struct _tuple15 _tmp7AE;struct _tuple15 _tmp30D=(_tmp7AE.f1=_tmp2FA,((_tmp7AE.f2=*((void**)_check_dyneither_subscript(_tmp30B,sizeof(void*),(int)i)),_tmp7AE)));_npop_handler(0);return _tmp30D;}_LL1E1:;_LL1E2: {
struct Cyc_Core_Impossible_exn_struct _tmp7B4;const char*_tmp7B3;struct Cyc_Core_Impossible_exn_struct*_tmp7B2;(int)_throw((void*)((_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B4.tag=Cyc_Core_Impossible,((_tmp7B4.f1=((_tmp7B3="anal_Rexp: Subscript",_tag_dyneither(_tmp7B3,sizeof(char),21))),_tmp7B4)))),_tmp7B2)))));}_LL1DC:;}_LL1D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301;if(_tmp304->tag != 5)goto _LL1DA;else{_tmp305=((_tmp304->f1).ptr_atts).bounds;}}_LL1D9: {
# 1431
struct _tuple15 _stmttmp2A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F8,_tmp1AE,(void*)((struct Cyc_List_List*)_check_null(_tmp2F9))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp312;void*_tmp313;struct _tuple15 _tmp311=_stmttmp2A;_tmp312=_tmp311.f1;_tmp313=_tmp311.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp314=_tmp312;_LL1E4: if((_tmp314.BottomFL).tag != 1)goto _LL1E6;_LL1E5: {
struct _tuple15 _tmp7B5;struct _tuple15 _tmp316=(_tmp7B5.f1=_tmp312,((_tmp7B5.f2=_tmp313,_tmp7B5)));_npop_handler(0);return _tmp316;}_LL1E6:;_LL1E7: {
struct _tuple15 _tmp7B6;struct _tuple15 _tmp318=(_tmp7B6.f1=_tmp2FA,((_tmp7B6.f2=_tmp313,_tmp7B6)));_npop_handler(0);return _tmp318;}_LL1E3:;};}_LL1DA:;_LL1DB: {
# 1436
struct Cyc_Core_Impossible_exn_struct _tmp7BC;const char*_tmp7BB;struct Cyc_Core_Impossible_exn_struct*_tmp7BA;(int)_throw((void*)((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BC.tag=Cyc_Core_Impossible,((_tmp7BC.f1=((_tmp7BB="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7BB,sizeof(char),33))),_tmp7BC)))),_tmp7BA)))));}_LL1D5:;};};}
# 1403
;_pop_region(temp);}_LL146: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1B0=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B0->tag != 30)goto _LL148;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL147:
# 1441
 _tmp1B4=_tmp1B1;goto _LL149;_LL148: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B3->tag != 23)goto _LL14A;else{_tmp1B4=_tmp1B3->f1;}}_LL149: {
# 1443
struct _RegionHandle _tmp31D=_new_region("temp");struct _RegionHandle*temp=& _tmp31D;_push_region(temp);
{struct _tuple22 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1B4,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp31F;struct Cyc_List_List*_tmp320;struct _tuple22 _tmp31E=_stmttmp27;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;{
unsigned int _tmp7C2;unsigned int _tmp7C1;struct _dyneither_ptr _tmp7C0;void**_tmp7BF;unsigned int _tmp7BE;struct _dyneither_ptr aggrdict=
(_tmp7BE=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B4),((_tmp7BF=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7BE)),((_tmp7C0=_tag_dyneither(_tmp7BF,sizeof(void*),_tmp7BE),((((_tmp7C1=_tmp7BE,_tmp7C3(& _tmp7C2,& _tmp7C1,& _tmp7BF,& _tmp320))),_tmp7C0)))))));
# 1451
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7CD;struct Cyc_CfFlowInfo_UnionRInfo _tmp7CC;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7CB;struct _tuple15 _tmp7CA;struct _tuple15 _tmp325=(_tmp7CA.f1=_tmp31F,((_tmp7CA.f2=(void*)((_tmp7CD=_region_malloc(env->r,sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CB.tag=6,((_tmp7CB.f1=((_tmp7CC.is_union=0,((_tmp7CC.fieldnum=- 1,_tmp7CC)))),((_tmp7CB.f2=aggrdict,_tmp7CB)))))),_tmp7CD)))),_tmp7CA)));_npop_handler(0);return _tmp325;};}
# 1444
;_pop_region(temp);}_LL14A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B5->tag != 29)goto _LL14C;else{_tmp1B6=_tmp1B5->f2;}}_LL14B: {
# 1454
struct Cyc_List_List*fs;
{void*_stmttmp26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32B=_stmttmp26;struct Cyc_List_List*_tmp32D;_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 12)goto _LL1EB;else{_tmp32D=_tmp32C->f2;}}_LL1EA:
 fs=_tmp32D;goto _LL1E8;_LL1EB:;_LL1EC: {
struct Cyc_Core_Impossible_exn_struct _tmp7D3;const char*_tmp7D2;struct Cyc_Core_Impossible_exn_struct*_tmp7D1;(int)_throw((void*)((_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D3.tag=Cyc_Core_Impossible,((_tmp7D3.f1=((_tmp7D2="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7D2,sizeof(char),35))),_tmp7D3)))),_tmp7D1)))));}_LL1E8:;}
# 1459
_tmp1B8=_tmp1B6;_tmp1B9=Cyc_Absyn_StructA;_tmp1BA=fs;goto _LL14D;}_LL14C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B7->tag != 28)goto _LL14E;else{_tmp1B8=_tmp1B7->f3;if(_tmp1B7->f4 == 0)goto _LL14E;_tmp1B9=(_tmp1B7->f4)->kind;if((_tmp1B7->f4)->impl == 0)goto _LL14E;_tmp1BA=((_tmp1B7->f4)->impl)->fields;}}_LL14D: {
# 1461
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp331=_new_region("temp");struct _RegionHandle*temp=& _tmp331;_push_region(temp);
{struct _tuple22 _stmttmp24=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1B8),1,1);
# 1463
union Cyc_CfFlowInfo_FlowInfo _tmp333;struct Cyc_List_List*_tmp334;struct _tuple22 _tmp332=_stmttmp24;_tmp333=_tmp332.f1;_tmp334=_tmp332.f2;{
# 1465
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp142,_tmp1BA,_tmp1B9 == Cyc_Absyn_UnionA,_tmp142->unknown_all);
# 1468
int field_no=-1;
{int i=0;for(0;_tmp334 != 0;(((_tmp334=_tmp334->tl,_tmp1B8=_tmp1B8->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1B8))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp25=(void*)ds->hd;void*_tmp335=_stmttmp25;struct _dyneither_ptr*_tmp338;_LL1EE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp336=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp335;if(_tmp336->tag != 0)goto _LL1F0;}_LL1EF: {
struct Cyc_Core_Impossible_exn_struct _tmp7D9;const char*_tmp7D8;struct Cyc_Core_Impossible_exn_struct*_tmp7D7;(int)_throw((void*)((_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7[0]=((_tmp7D9.tag=Cyc_Core_Impossible,((_tmp7D9.f1=((_tmp7D8="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7D8,sizeof(char),22))),_tmp7D9)))),_tmp7D7)))));}_LL1F0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp337=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp335;if(_tmp337->tag != 1)goto _LL1ED;else{_tmp338=_tmp337->f1;}}_LL1F1:
# 1475
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1BA,_tmp338);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp334->hd;
# 1478
if(_tmp1B9 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp33C=(*((struct _tuple25*)_tmp1B8->hd)).f2;
_tmp333=Cyc_NewControlFlow_use_Rval(env,_tmp33C->loc,_tmp333,(void*)_tmp334->hd);
# 1482
Cyc_NewControlFlow_check_requires(_tmp33C->loc,_tmp142->r,exp_type,_tmp338,_tmp333);}_LL1ED:;}}}{
# 1485
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7DF;struct Cyc_CfFlowInfo_UnionRInfo _tmp7DE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7DD;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33D=(_tmp7DD=_region_malloc(env->r,sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=6,((_tmp7DF.f1=((_tmp7DE.is_union=_tmp1B9 == Cyc_Absyn_UnionA,((_tmp7DE.fieldnum=field_no,_tmp7DE)))),((_tmp7DF.f2=aggrdict,_tmp7DF)))))),_tmp7DD)));
struct _tuple15 _tmp7E0;struct _tuple15 _tmp33F=(_tmp7E0.f1=_tmp333,((_tmp7E0.f2=(void*)_tmp33D,_tmp7E0)));_npop_handler(0);return _tmp33F;};};}
# 1463
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BB->tag != 28)goto _LL150;}_LL14F: {
# 1489
struct Cyc_Core_Impossible_exn_struct _tmp7E6;const char*_tmp7E5;struct Cyc_Core_Impossible_exn_struct*_tmp7E4;(int)_throw((void*)((_tmp7E4=_cycalloc(sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E6.tag=Cyc_Core_Impossible,((_tmp7E6.f1=((_tmp7E5="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7E5,sizeof(char),31))),_tmp7E6)))),_tmp7E4)))));}_LL150: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BC->tag != 25)goto _LL152;else{_tmp1BD=_tmp1BC->f1;}}_LL151: {
# 1491
struct _RegionHandle _tmp346=_new_region("temp");struct _RegionHandle*temp=& _tmp346;_push_region(temp);
{struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BD);
struct _tuple22 _stmttmp23=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp347,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp349;struct Cyc_List_List*_tmp34A;struct _tuple22 _tmp348=_stmttmp23;_tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;
for(0;_tmp34A != 0;(_tmp34A=_tmp34A->tl,_tmp347=_tmp347->tl)){
_tmp349=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp347))->hd)->loc,_tmp349,(void*)_tmp34A->hd);}{
struct _tuple15 _tmp7E7;struct _tuple15 _tmp34C=(_tmp7E7.f1=_tmp349,((_tmp7E7.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp7E7)));_npop_handler(0);return _tmp34C;};}
# 1492
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BE->tag != 26)goto _LL154;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;_tmp1C1=_tmp1BE->f3;_tmp1C2=_tmp1BE->f4;}}_LL153: {
# 1500
struct _tuple15 _stmttmp20=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C0);union Cyc_CfFlowInfo_FlowInfo _tmp34E;void*_tmp34F;struct _tuple15 _tmp34D=_stmttmp20;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp350=_tmp34E;struct Cyc_Dict_Dict _tmp351;struct Cyc_List_List*_tmp352;_LL1F3: if((_tmp350.BottomFL).tag != 1)goto _LL1F5;_LL1F4: {
struct _tuple15 _tmp7E8;return(_tmp7E8.f1=_tmp34E,((_tmp7E8.f2=_tmp142->unknown_all,_tmp7E8)));}_LL1F5: if((_tmp350.ReachableFL).tag != 2)goto _LL1F2;_tmp351=((struct _tuple14)(_tmp350.ReachableFL).val).f1;_tmp352=((struct _tuple14)(_tmp350.ReachableFL).val).f2;_LL1F6:
# 1504
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp351,_tmp34F)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7EB;void*_tmp7EA;(_tmp7EA=0,Cyc_CfFlowInfo_aerr(_tmp1C0->loc,((_tmp7EB="expression may not be initialized",_tag_dyneither(_tmp7EB,sizeof(char),34))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}{
# 1508
struct Cyc_List_List*new_relns=_tmp352;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1BF);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1C0,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp352);
# 1514
if(_tmp352 != new_relns)
_tmp34E=Cyc_CfFlowInfo_ReachableFL(_tmp351,new_relns);{
# 1518
void*_tmp356=_tmp34F;_LL1F8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp357=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp357->tag != 0)goto _LL1FA;}_LL1F9: {
struct _tuple15 _tmp7EC;return(_tmp7EC.f1=_tmp34E,((_tmp7EC.f2=_tmp142->unknown_all,_tmp7EC)));}_LL1FA: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp358=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp358->tag != 2)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp359=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp359->tag != 1)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp35A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp35A->tag != 5)goto _LL200;}_LL1FF: {
# 1523
struct Cyc_List_List _tmp7ED;struct Cyc_List_List _tmp35C=(_tmp7ED.hd=_tmp1BF,((_tmp7ED.tl=0,_tmp7ED)));
_tmp34E=Cyc_NewControlFlow_add_vars(_tmp142,_tmp34E,& _tmp35C,_tmp142->unknown_all,e->loc,0);{
# 1526
struct _tuple15 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34E,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp35E;void*_tmp35F;struct _tuple15 _tmp35D=_stmttmp22;_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp360=_tmp35E;struct Cyc_Dict_Dict _tmp361;_LL203: if((_tmp360.BottomFL).tag != 1)goto _LL205;_LL204: {
struct _tuple15 _tmp7EE;return(_tmp7EE.f1=_tmp35E,((_tmp7EE.f2=_tmp142->unknown_all,_tmp7EE)));}_LL205: if((_tmp360.ReachableFL).tag != 2)goto _LL202;_tmp361=((struct _tuple14)(_tmp360.ReachableFL).val).f1;_LL206:
# 1530
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp361,_tmp35F)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_CfFlowInfo_aerr(_tmp1C0->loc,((_tmp7F1="expression may not be initialized",_tag_dyneither(_tmp7F1,sizeof(char),34))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}{
struct _tuple15 _tmp7F2;return(_tmp7F2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F2.f2=_tmp142->unknown_all,_tmp7F2)));};}_LL202:;}
# 1535
_tmp34E=_tmp35E;
goto _LL201;};}_LL200:;_LL201:
# 1538
 while(1){
struct Cyc_List_List _tmp7F3;struct Cyc_List_List _tmp367=(_tmp7F3.hd=_tmp1BF,((_tmp7F3.tl=0,_tmp7F3)));
_tmp34E=Cyc_NewControlFlow_add_vars(_tmp142,_tmp34E,& _tmp367,_tmp142->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp21=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34E,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp369;void*_tmp36A;struct _tuple15 _tmp368=_stmttmp21;_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp36B=_tmp369;struct Cyc_Dict_Dict _tmp36C;_LL208: if((_tmp36B.BottomFL).tag != 1)goto _LL20A;_LL209:
 goto _LL207;_LL20A: if((_tmp36B.ReachableFL).tag != 2)goto _LL207;_tmp36C=((struct _tuple14)(_tmp36B.ReachableFL).val).f1;_LL20B:
# 1545
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp36C,_tmp36A)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,Cyc_CfFlowInfo_aerr(_tmp1C0->loc,((_tmp7F6="expression may not be initialized",_tag_dyneither(_tmp7F6,sizeof(char),34))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}{
struct _tuple15 _tmp7F7;return(_tmp7F7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F7.f2=_tmp142->unknown_all,_tmp7F7)));};}_LL207:;}{
# 1550
union Cyc_CfFlowInfo_FlowInfo _tmp370=Cyc_CfFlowInfo_join_flow(_tmp142,env->all_changed,_tmp34E,_tmp369);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp370,_tmp34E))
break;
_tmp34E=_tmp370;};};}{
# 1555
struct _tuple15 _tmp7F8;return(_tmp7F8.f1=_tmp34E,((_tmp7F8.f2=_tmp142->unknown_all,_tmp7F8)));};_LL1F7:;};};_LL1F2:;};}_LL154: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1C3=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C3->tag != 27)goto _LL156;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=(void*)_tmp1C3->f2;_tmp1C6=_tmp1C3->f3;}}_LL155: {
# 1561
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7FB;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7FA;void*root=(void*)((_tmp7FA=_region_malloc(_tmp142->r,sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FB.tag=1,((_tmp7FB.f1=_tmp1C4,((_tmp7FB.f2=(void*)_check_null(e->topt),_tmp7FB)))))),_tmp7FA))));
struct Cyc_CfFlowInfo_Place*_tmp7FC;struct Cyc_CfFlowInfo_Place*place=(_tmp7FC=_region_malloc(_tmp142->r,sizeof(*_tmp7FC)),((_tmp7FC->root=root,((_tmp7FC->fields=0,_tmp7FC)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7FF;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7FE;void*rval=(void*)((_tmp7FE=_region_malloc(_tmp142->r,sizeof(*_tmp7FE)),((_tmp7FE[0]=((_tmp7FF.tag=5,((_tmp7FF.f1=place,_tmp7FF)))),_tmp7FE))));
void*place_val;
# 1569
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,_tmp1C5,0,_tmp142->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C4);union Cyc_CfFlowInfo_FlowInfo _tmp374;struct _tuple15 _tmp373=_stmttmp1F;_tmp374=_tmp373.f1;
outflow=_tmp374;{
union Cyc_CfFlowInfo_FlowInfo _tmp375=outflow;struct Cyc_Dict_Dict _tmp376;struct Cyc_List_List*_tmp377;_LL20D: if((_tmp375.BottomFL).tag != 1)goto _LL20F;_LL20E: {
struct _tuple15 _tmp800;return(_tmp800.f1=outflow,((_tmp800.f2=rval,_tmp800)));}_LL20F: if((_tmp375.ReachableFL).tag != 2)goto _LL20C;_tmp376=((struct _tuple14)(_tmp375.ReachableFL).val).f1;_tmp377=((struct _tuple14)(_tmp375.ReachableFL).val).f2;_LL210: {
# 1577
struct _tuple15 _tmp801;return(_tmp801.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp376,root,place_val),_tmp377),((_tmp801.f2=rval,_tmp801)));}_LL20C:;};};};}_LL156: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C7->tag != 36)goto _LL158;else{_tmp1C8=_tmp1C7->f1;}}_LL157: {
# 1581
struct _tuple16 _tmp802;struct _tuple16 _tmp37F=(_tmp802.f1=(env->fenv)->unknown_all,((_tmp802.f2=copy_ctxt,_tmp802)));
union Cyc_CfFlowInfo_FlowInfo _tmp380=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1C8,& _tmp37F);
struct _tuple15 _tmp803;return(_tmp803.f1=_tmp380,((_tmp803.f2=_tmp37F.f1,_tmp803)));}_LL158: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C9->tag != 1)goto _LL15A;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1CA=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1C9->f2);if(_tmp1CA->tag != 0)goto _LL15A;}}_LL159:
# 1585
 goto _LL15B;_LL15A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CB->tag != 35)goto _LL15C;}_LL15B:
 goto _LL15D;_LL15C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CC->tag != 24)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CD->tag != 38)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CE->tag != 39)goto _LLF9;}_LL161: {
# 1590
struct Cyc_Core_Impossible_exn_struct _tmp809;const char*_tmp808;struct Cyc_Core_Impossible_exn_struct*_tmp807;(int)_throw((void*)((_tmp807=_cycalloc(sizeof(*_tmp807)),((_tmp807[0]=((_tmp809.tag=Cyc_Core_Impossible,((_tmp809.f1=((_tmp808="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp808,sizeof(char),31))),_tmp809)))),_tmp807)))));}_LLF9:;};}
# 1600
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1608
struct Cyc_CfFlowInfo_FlowEnv*_tmp386=env->fenv;
void*_stmttmp51=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp387=_stmttmp51;void*_tmp389;union Cyc_Absyn_Constraint*_tmp38A;union Cyc_Absyn_Constraint*_tmp38B;_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp387;if(_tmp388->tag != 5)goto _LL214;else{_tmp389=(_tmp388->f1).elt_typ;_tmp38A=((_tmp388->f1).ptr_atts).bounds;_tmp38B=((_tmp388->f1).ptr_atts).zero_term;}}_LL213: {
# 1611
union Cyc_CfFlowInfo_FlowInfo _tmp38C=f;struct Cyc_Dict_Dict _tmp38D;struct Cyc_List_List*_tmp38E;_LL217: if((_tmp38C.BottomFL).tag != 1)goto _LL219;_LL218: {
struct _tuple17 _tmp80A;return(_tmp80A.f1=f,((_tmp80A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp80A)));}_LL219: if((_tmp38C.ReachableFL).tag != 2)goto _LL216;_tmp38D=((struct _tuple14)(_tmp38C.ReachableFL).val).f1;_tmp38E=((struct _tuple14)(_tmp38C.ReachableFL).val).f2;_LL21A: {
# 1615
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp38A)){
retry: {void*_tmp390=r;struct Cyc_Absyn_Vardecl*_tmp392;void*_tmp393;void*_tmp397;struct Cyc_List_List*_tmp398;enum Cyc_CfFlowInfo_InitLevel _tmp39B;_LL21C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp391=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp391->tag != 8)goto _LL21E;else{_tmp392=_tmp391->f1;_tmp393=(void*)_tmp391->f2;}}_LL21D:
 r=_tmp393;locname=_tmp392;goto retry;_LL21E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp394=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp394->tag != 1)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp395=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp395->tag != 2)goto _LL222;}_LL221:
# 1621
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp80D;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80C;e->annot=(void*)((_tmp80C=_cycalloc(sizeof(*_tmp80C)),((_tmp80C[0]=((_tmp80D.tag=Cyc_CfFlowInfo_NotZero,((_tmp80D.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp80D)))),_tmp80C))));}goto _LL21B;_LL222: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp396=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp396->tag != 5)goto _LL224;else{_tmp397=(_tmp396->f1)->root;_tmp398=(_tmp396->f1)->fields;}}_LL223:
# 1623
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp810;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80F;e->annot=(void*)((_tmp80F=_cycalloc(sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=Cyc_CfFlowInfo_NotZero,((_tmp810.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp810)))),_tmp80F))));}{
struct Cyc_CfFlowInfo_Place*_tmp813;struct _tuple17 _tmp812;return(_tmp812.f1=f,((_tmp812.f2=Cyc_CfFlowInfo_PlaceL(((_tmp813=_region_malloc(_tmp386->r,sizeof(*_tmp813)),((_tmp813->root=_tmp397,((_tmp813->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp386->r,_tmp398,flds),_tmp813))))))),_tmp812)));};_LL224: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp399=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp399->tag != 0)goto _LL226;}_LL225:
# 1626
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp814;return(_tmp814.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp814.f2=Cyc_CfFlowInfo_UnknownL(),_tmp814)));};_LL226: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp39A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp390;if(_tmp39A->tag != 3)goto _LL228;else{_tmp39B=_tmp39A->f1;}}_LL227:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp39B,locname);goto _LL229;_LL228:;_LL229:
# 1631
 if(passthrough_consumes){
void*_tmp3A3=r;void*_tmp3A5;_LL22B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A3;if(_tmp3A4->tag != 7)goto _LL22D;else{_tmp3A5=(void*)_tmp3A4->f3;}}_LL22C:
 r=_tmp3A5;goto retry;_LL22D:;_LL22E:
 goto _LL22A;_LL22A:;}{
# 1637
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp817;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp816;e->annot=(void*)((_tmp816=_cycalloc(sizeof(*_tmp816)),((_tmp816[0]=((_tmp817.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp817.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp817)))),_tmp816))));};_LL21B:;}}else{
# 1640
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81A;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp819;e->annot=(void*)((_tmp819=_cycalloc(sizeof(*_tmp819)),((_tmp819[0]=((_tmp81A.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81A.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp81A)))),_tmp819))));}
if(Cyc_CfFlowInfo_initlevel(_tmp386,_tmp38D,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp81D="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp81D,sizeof(char),46))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}{
struct _tuple17 _tmp81E;return(_tmp81E.f1=f,((_tmp81E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp81E)));};}_LL216:;}_LL214:;_LL215: {
# 1645
struct Cyc_Core_Impossible_exn_struct _tmp824;const char*_tmp823;struct Cyc_Core_Impossible_exn_struct*_tmp822;(int)_throw((void*)((_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=((_tmp824.tag=Cyc_Core_Impossible,((_tmp824.f1=((_tmp823="left deref of non-pointer-type",_tag_dyneither(_tmp823,sizeof(char),31))),_tmp824)))),_tmp822)))));}_LL211:;}
# 1653
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1656
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B0=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3B1=inflow;struct Cyc_Dict_Dict _tmp3B2;struct Cyc_List_List*_tmp3B3;_LL230: if((_tmp3B1.BottomFL).tag != 1)goto _LL232;_LL231: {
struct _tuple17 _tmp825;return(_tmp825.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp825.f2=Cyc_CfFlowInfo_UnknownL(),_tmp825)));}_LL232: if((_tmp3B1.ReachableFL).tag != 2)goto _LL22F;_tmp3B2=((struct _tuple14)(_tmp3B1.ReachableFL).val).f1;_tmp3B3=((struct _tuple14)(_tmp3B1.ReachableFL).val).f2;_LL233:
# 1661
 d=_tmp3B2;_LL22F:;}{
# 1665
void*_stmttmp52=e->r;void*_tmp3B5=_stmttmp52;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Vardecl*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C1;struct Cyc_Absyn_Vardecl*_tmp3C4;struct Cyc_Absyn_Vardecl*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C9;struct _dyneither_ptr*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct _dyneither_ptr*_tmp3D2;_LL235: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3B6->tag != 13)goto _LL237;else{_tmp3B7=_tmp3B6->f2;}}_LL236:
 _tmp3B9=_tmp3B7;goto _LL238;_LL237: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3B8->tag != 11)goto _LL239;else{_tmp3B9=_tmp3B8->f1;}}_LL238:
 _tmp3BB=_tmp3B9;goto _LL23A;_LL239: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3BA=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3BA->tag != 12)goto _LL23B;else{_tmp3BB=_tmp3BA->f1;}}_LL23A:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3BB,flds);_LL23B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3BC->tag != 1)goto _LL23D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3BD=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3BC->f2);if(_tmp3BD->tag != 3)goto _LL23D;else{_tmp3BE=_tmp3BD->f1;}}}_LL23C:
# 1670
 _tmp3C1=_tmp3BE;goto _LL23E;_LL23D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3BF->tag != 1)goto _LL23F;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3C0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3BF->f2);if(_tmp3C0->tag != 4)goto _LL23F;else{_tmp3C1=_tmp3C0->f1;}}}_LL23E:
 _tmp3C4=_tmp3C1;goto _LL240;_LL23F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3C2->tag != 1)goto _LL241;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3C3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3C2->f2);if(_tmp3C3->tag != 5)goto _LL241;else{_tmp3C4=_tmp3C3->f1;}}}_LL240: {
# 1673
struct Cyc_CfFlowInfo_Place*_tmp82F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp82E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp82D;struct _tuple17 _tmp82C;return(_tmp82C.f1=inflow,((_tmp82C.f2=Cyc_CfFlowInfo_PlaceL(((_tmp82F=_region_malloc(env->r,sizeof(*_tmp82F)),((_tmp82F->root=(void*)((_tmp82D=_region_malloc(env->r,sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82E.tag=0,((_tmp82E.f1=_tmp3C4,_tmp82E)))),_tmp82D)))),((_tmp82F->fields=flds,_tmp82F))))))),_tmp82C)));}_LL241: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3C5->tag != 1)goto _LL243;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3C6=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3C5->f2);if(_tmp3C6->tag != 1)goto _LL243;else{_tmp3C7=_tmp3C6->f1;}}}_LL242: {
# 1675
struct _tuple17 _tmp830;return(_tmp830.f1=inflow,((_tmp830.f2=Cyc_CfFlowInfo_UnknownL(),_tmp830)));}_LL243: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3C8=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3C8->tag != 21)goto _LL245;else{_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;}}_LL244:
# 1678
{void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3C9->topt));void*_tmp3D8=_stmttmp59;void*_tmp3DA;_LL24E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D8;if(_tmp3D9->tag != 5)goto _LL250;else{_tmp3DA=(_tmp3D9->f1).elt_typ;}}_LL24F:
# 1680
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3DA)){
Cyc_NewControlFlow_check_requires(_tmp3C9->loc,_tmp3B0->r,_tmp3DA,_tmp3CA,inflow);{
struct Cyc_List_List*_tmp831;flds=((_tmp831=_region_malloc(env->r,sizeof(*_tmp831)),((_tmp831->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3DA,_tmp3CA),((_tmp831->tl=flds,_tmp831))))));};}
# 1684
goto _LL24D;_LL250:;_LL251: {
struct Cyc_Core_Impossible_exn_struct _tmp837;const char*_tmp836;struct Cyc_Core_Impossible_exn_struct*_tmp835;(int)_throw((void*)((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835[0]=((_tmp837.tag=Cyc_Core_Impossible,((_tmp837.f1=((_tmp836="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp836,sizeof(char),25))),_tmp837)))),_tmp835)))));}_LL24D:;}
# 1687
_tmp3CC=_tmp3C9;goto _LL246;_LL245: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3CB->tag != 19)goto _LL247;else{_tmp3CC=_tmp3CB->f1;}}_LL246:
# 1690
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3CC->topt),1)){
# 1692
struct _tuple17 _stmttmp56=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3CC->topt),1),passthrough_consumes,_tmp3CC);
# 1692
union Cyc_CfFlowInfo_FlowInfo _tmp3E0;union Cyc_CfFlowInfo_AbsLVal _tmp3E1;struct _tuple17 _tmp3DF=_stmttmp56;_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;{
# 1696
struct _tuple17 _tmp838;struct _tuple17 _stmttmp57=(_tmp838.f1=_tmp3E0,((_tmp838.f2=_tmp3E1,_tmp838)));struct _tuple17 _tmp3E2=_stmttmp57;struct Cyc_Dict_Dict _tmp3E3;struct Cyc_List_List*_tmp3E4;struct Cyc_CfFlowInfo_Place*_tmp3E5;_LL253: if(((_tmp3E2.f1).ReachableFL).tag != 2)goto _LL255;_tmp3E3=((struct _tuple14)((_tmp3E2.f1).ReachableFL).val).f1;_tmp3E4=((struct _tuple14)((_tmp3E2.f1).ReachableFL).val).f2;if(((_tmp3E2.f2).PlaceL).tag != 1)goto _LL255;_tmp3E5=(struct Cyc_CfFlowInfo_Place*)((_tmp3E2.f2).PlaceL).val;_LL254: {
# 1698
void*_tmp3E6=Cyc_CfFlowInfo_lookup_place(_tmp3E3,_tmp3E5);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp3E7=_tmp3E6;struct Cyc_Absyn_Vardecl*_tmp3E9;void*_tmp3EA;_LL258: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E7;if(_tmp3E8->tag != 8)goto _LL25A;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=(void*)_tmp3E8->f2;}}_LL259:
 _tmp3E6=_tmp3EA;locname=_tmp3E9;goto retry;_LL25A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3EB=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E7;if(_tmp3EB->tag != 7)goto _LL25C;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3EC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp3EB->f3);if(_tmp3EC->tag != 5)goto _LL25C;}}_LL25B:
 goto _LL25D;_LL25C: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3ED=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3E7;if(_tmp3ED->tag != 5)goto _LL25E;}_LL25D:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3E0,_tmp3CC,_tmp3E0,_tmp3E6,passthrough_consumes,flds);_LL25E:;_LL25F: {
# 1710
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3B0,_tmp3E3,_tmp3E6);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3B0->unknown_all: _tmp3B0->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B0,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3CC->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp83B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp83A;void*new_root=(void*)((_tmp83A=_region_malloc(_tmp3B0->r,sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=1,((_tmp83B.f1=e,((_tmp83B.f2=(void*)_check_null(e->topt),_tmp83B)))))),_tmp83A))));
struct Cyc_CfFlowInfo_Place*_tmp83C;struct Cyc_CfFlowInfo_Place*place=(_tmp83C=_region_malloc(_tmp3B0->r,sizeof(*_tmp83C)),((_tmp83C->root=new_root,((_tmp83C->fields=0,_tmp83C)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp83F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp83E;void*res=(void*)((_tmp83E=_region_malloc(_tmp3B0->r,sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp83F.tag=5,((_tmp83F.f1=place,_tmp83F)))),_tmp83E))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp842;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp841;res=(void*)((_tmp841=_region_malloc(_tmp3B0->r,sizeof(*_tmp841)),((_tmp841[0]=((_tmp842.tag=8,((_tmp842.f1=locname,((_tmp842.f2=res,_tmp842)))))),_tmp841))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E3,new_root,new_rval);
_tmp3E3=Cyc_CfFlowInfo_assign_place(_tmp3B0,e->loc,_tmp3E3,env->all_changed,_tmp3E5,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3F0=Cyc_CfFlowInfo_ReachableFL(_tmp3E3,_tmp3E4);
# 1725
return Cyc_NewControlFlow_anal_derefL(env,_tmp3F0,_tmp3CC,_tmp3F0,res,passthrough_consumes,flds);};}_LL257:;}}_LL255:;_LL256:
# 1727
 goto _LL252;_LL252:;};}{
# 1730
struct _tuple15 _stmttmp58=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3CC);union Cyc_CfFlowInfo_FlowInfo _tmp3F8;void*_tmp3F9;struct _tuple15 _tmp3F7=_stmttmp58;_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CC,_tmp3F8,_tmp3F9,passthrough_consumes,flds);};_LL247: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3CD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3CD->tag != 22)goto _LL249;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;}}_LL248: {
# 1734
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(_tmp3CE->topt));void*_tmp3FA=_stmttmp53;union Cyc_Absyn_Constraint*_tmp3FD;_LL261: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3FA;if(_tmp3FB->tag != 10)goto _LL263;}_LL262: {
# 1736
unsigned int _tmp3FE=(Cyc_Evexp_eval_const_uint_exp(_tmp3CF)).f1;
struct Cyc_List_List*_tmp843;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CE,((_tmp843=_region_malloc(env->r,sizeof(*_tmp843)),((_tmp843->hd=(void*)((int)_tmp3FE),((_tmp843->tl=flds,_tmp843)))))));}_LL263: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FA;if(_tmp3FC->tag != 5)goto _LL265;else{_tmp3FD=((_tmp3FC->f1).ptr_atts).bounds;}}_LL264: {
# 1739
struct _RegionHandle _tmp400=_new_region("temp");struct _RegionHandle*temp=& _tmp400;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp844[2];struct _tuple22 _stmttmp54=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp844[1]=_tmp3CF,((_tmp844[0]=_tmp3CE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp844,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1740
union Cyc_CfFlowInfo_FlowInfo _tmp402;struct Cyc_List_List*_tmp403;struct _tuple22 _tmp401=_stmttmp54;_tmp402=_tmp401.f1;_tmp403=_tmp401.f2;{
# 1742
union Cyc_CfFlowInfo_FlowInfo _tmp404=_tmp402;
{union Cyc_CfFlowInfo_FlowInfo _tmp405=_tmp402;struct Cyc_Dict_Dict _tmp406;struct Cyc_List_List*_tmp407;_LL268: if((_tmp405.ReachableFL).tag != 2)goto _LL26A;_tmp406=((struct _tuple14)(_tmp405.ReachableFL).val).f1;_tmp407=((struct _tuple14)(_tmp405.ReachableFL).val).f2;_LL269:
# 1745
 if(Cyc_CfFlowInfo_initlevel(_tmp3B0,_tmp406,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp403))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp847;void*_tmp846;(_tmp846=0,Cyc_CfFlowInfo_aerr(_tmp3CF->loc,((_tmp847="expression may not be initialized",_tag_dyneither(_tmp847,sizeof(char),34))),_tag_dyneither(_tmp846,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp40A=Cyc_NewControlFlow_add_subscript_reln(_tmp3B0->r,_tmp407,_tmp3CE,_tmp3CF);
if(_tmp407 != _tmp40A)
_tmp404=Cyc_CfFlowInfo_ReachableFL(_tmp406,_tmp40A);
goto _LL267;};_LL26A:;_LL26B:
 goto _LL267;_LL267:;}{
# 1753
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CE,_tmp402,(void*)((struct Cyc_List_List*)_check_null(_tmp403))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp40C;union Cyc_CfFlowInfo_AbsLVal _tmp40D;struct _tuple17 _tmp40B=_stmttmp55;_tmp40C=_tmp40B.f1;_tmp40D=_tmp40B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp40E=_tmp40C;_LL26D: if((_tmp40E.BottomFL).tag != 1)goto _LL26F;_LL26E: {
struct _tuple17 _tmp848;struct _tuple17 _tmp410=(_tmp848.f1=_tmp40C,((_tmp848.f2=_tmp40D,_tmp848)));_npop_handler(0);return _tmp410;}_LL26F:;_LL270: {
struct _tuple17 _tmp849;struct _tuple17 _tmp412=(_tmp849.f1=_tmp404,((_tmp849.f2=_tmp40D,_tmp849)));_npop_handler(0);return _tmp412;}_LL26C:;};};};}
# 1740
;_pop_region(temp);}_LL265:;_LL266: {
# 1759
struct Cyc_Core_Impossible_exn_struct _tmp84F;const char*_tmp84E;struct Cyc_Core_Impossible_exn_struct*_tmp84D;(int)_throw((void*)((_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D[0]=((_tmp84F.tag=Cyc_Core_Impossible,((_tmp84F.f1=((_tmp84E="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp84E,sizeof(char),33))),_tmp84F)))),_tmp84D)))));}_LL260:;}_LL249: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3D0->tag != 20)goto _LL24B;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;}}_LL24A: {
# 1763
void*_tmp417=(void*)_check_null(_tmp3D1->topt);
if(Cyc_Absyn_is_require_union_type(_tmp417))
Cyc_NewControlFlow_check_requires(_tmp3D1->loc,_tmp3B0->r,_tmp417,_tmp3D2,inflow);
# 1767
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp417)){
struct _tuple17 _tmp850;return(_tmp850.f1=inflow,((_tmp850.f2=Cyc_CfFlowInfo_UnknownL(),_tmp850)));}{
# 1770
struct Cyc_List_List*_tmp851;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3D1,(
(_tmp851=_region_malloc(env->r,sizeof(*_tmp851)),((_tmp851->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp417,_tmp3D2),((_tmp851->tl=flds,_tmp851)))))));};}_LL24B:;_LL24C: {
# 1773
struct _tuple17 _tmp852;return(_tmp852.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp852.f2=Cyc_CfFlowInfo_UnknownL(),_tmp852)));}_LL234:;};}
# 1777
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1780
struct _tuple17 _stmttmp5A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp41C;union Cyc_CfFlowInfo_AbsLVal _tmp41D;struct _tuple17 _tmp41B=_stmttmp5A;_tmp41C=_tmp41B.f1;_tmp41D=_tmp41B.f2;{
struct _tuple17 _tmp853;return(_tmp853.f1=_tmp41C,((_tmp853.f2=_tmp41D,_tmp853)));};}
# 1787
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1789
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _stmttmp5B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp420;struct _tuple17 _tmp41F=_stmttmp5B;_tmp420=_tmp41F.f1;
inflow=_tmp420;}}}
# 1796
return inflow;}
# 1802
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1805
struct Cyc_CfFlowInfo_FlowEnv*_tmp421=env->fenv;
# 1808
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp422=_new_region("temp");struct _RegionHandle*temp=& _tmp422;_push_region(temp);{
struct _tuple22 _stmttmp5C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp424;struct Cyc_List_List*_tmp425;struct _tuple22 _tmp423=_stmttmp5C;_tmp424=_tmp423.f1;_tmp425=_tmp423.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp425))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp425->tl))->hd;
f=_tmp424;}{
# 1818
union Cyc_CfFlowInfo_FlowInfo _tmp426=f;struct Cyc_Dict_Dict _tmp427;struct Cyc_List_List*_tmp428;_LL272: if((_tmp426.BottomFL).tag != 1)goto _LL274;_LL273: {
struct _tuple18 _tmp854;struct _tuple18 _tmp42A=(_tmp854.f1=f,((_tmp854.f2=f,_tmp854)));_npop_handler(0);return _tmp42A;}_LL274: if((_tmp426.ReachableFL).tag != 2)goto _LL271;_tmp427=((struct _tuple14)(_tmp426.ReachableFL).val).f1;_tmp428=((struct _tuple14)(_tmp426.ReachableFL).val).f2;_LL275: {
# 1821
struct Cyc_Absyn_Exp*_tmp42B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp42C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp427,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp857;void*_tmp856;(_tmp856=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp857="expression may not be initialized",_tag_dyneither(_tmp857,sizeof(char),34))),_tag_dyneither(_tmp856,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp427,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp85A="expression may not be initialized",_tag_dyneither(_tmp85A,sizeof(char),34))),_tag_dyneither(_tmp859,sizeof(void*),0)));}{
# 1828
union Cyc_CfFlowInfo_FlowInfo _tmp431=f;
union Cyc_CfFlowInfo_FlowInfo _tmp432=f;
# 1833
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp433=r1;struct Cyc_Absyn_Vardecl*_tmp435;void*_tmp436;_LL277: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp434=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp433;if(_tmp434->tag != 8)goto _LL279;else{_tmp435=_tmp434->f1;_tmp436=(void*)_tmp434->f2;}}_LL278:
 r1=_tmp436;r1n=_tmp435;goto _LL276;_LL279:;_LL27A:
 goto _LL276;_LL276:;}
# 1839
{void*_tmp437=r2;struct Cyc_Absyn_Vardecl*_tmp439;void*_tmp43A;_LL27C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp438=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp437;if(_tmp438->tag != 8)goto _LL27E;else{_tmp439=_tmp438->f1;_tmp43A=(void*)_tmp438->f2;}}_LL27D:
 r2=_tmp43A;r2n=_tmp439;goto _LL27B;_LL27E:;_LL27F:
 goto _LL27B;_LL27B:;}{
# 1844
struct _tuple0 _tmp85B;struct _tuple0 _stmttmp5D=(_tmp85B.f1=r1,((_tmp85B.f2=r2,_tmp85B)));struct _tuple0 _tmp43B=_stmttmp5D;enum Cyc_CfFlowInfo_InitLevel _tmp43D;enum Cyc_CfFlowInfo_InitLevel _tmp441;_LL281:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp43C->tag != 3)goto _LL283;else{_tmp43D=_tmp43C->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp43E->tag != 0)goto _LL283;};_LL282: {
# 1848
struct _tuple18 _stmttmp5F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp42B,_tmp43D,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp451;union Cyc_CfFlowInfo_FlowInfo _tmp452;struct _tuple18 _tmp450=_stmttmp5F;_tmp451=_tmp450.f1;_tmp452=_tmp450.f2;
switch(p){case Cyc_Absyn_Eq: _LL295:
 _tmp431=_tmp452;_tmp432=_tmp451;break;case Cyc_Absyn_Neq: _LL296:
 _tmp431=_tmp451;_tmp432=_tmp452;break;default: _LL297: {
struct Cyc_Core_Impossible_exn_struct _tmp861;const char*_tmp860;struct Cyc_Core_Impossible_exn_struct*_tmp85F;(int)_throw((void*)((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp861.tag=Cyc_Core_Impossible,((_tmp861.f1=((_tmp860="anal_test, zero-split",_tag_dyneither(_tmp860,sizeof(char),22))),_tmp861)))),_tmp85F)))));}}
# 1854
goto _LL280;}_LL283:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp43F->tag != 0)goto _LL285;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp440->tag != 3)goto _LL285;else{_tmp441=_tmp440->f1;}};_LL284: {
# 1856
struct _tuple18 _stmttmp5E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp42C,_tmp441,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp457;union Cyc_CfFlowInfo_FlowInfo _tmp458;struct _tuple18 _tmp456=_stmttmp5E;_tmp457=_tmp456.f1;_tmp458=_tmp456.f2;
switch(p){case Cyc_Absyn_Eq: _LL299:
 _tmp431=_tmp458;_tmp432=_tmp457;break;case Cyc_Absyn_Neq: _LL29A:
 _tmp431=_tmp457;_tmp432=_tmp458;break;default: _LL29B: {
struct Cyc_Core_Impossible_exn_struct _tmp867;const char*_tmp866;struct Cyc_Core_Impossible_exn_struct*_tmp865;(int)_throw((void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp867.tag=Cyc_Core_Impossible,((_tmp867.f1=((_tmp866="anal_test, zero-split",_tag_dyneither(_tmp866,sizeof(char),22))),_tmp867)))),_tmp865)))));}}
# 1862
goto _LL280;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp442=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp442->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp443=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp443->tag != 0)goto _LL287;};_LL286:
# 1864
 if(p == Cyc_Absyn_Eq)_tmp432=Cyc_CfFlowInfo_BottomFL();else{
_tmp431=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp444=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp444->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp445->tag != 1)goto _LL289;};_LL288:
 goto _LL28A;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp446=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp446->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp447->tag != 2)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp448->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp449->tag != 5)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp44A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp44A->tag != 1)goto _LL28F;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp44B->tag != 0)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp44C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp44C->tag != 2)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp44D->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp43B.f1;if(_tmp44E->tag != 5)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43B.f2;if(_tmp44F->tag != 0)goto _LL293;};_LL292:
# 1873
 if(p == Cyc_Absyn_Neq)_tmp432=Cyc_CfFlowInfo_BottomFL();else{
_tmp431=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL293:;_LL294:
 goto _LL280;_LL280:;};}
# 1884
{struct _tuple0 _tmp868;struct _tuple0 _stmttmp60=(_tmp868.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp42B->topt)),((_tmp868.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp42C->topt)),_tmp868)));struct _tuple0 _tmp45D=_stmttmp60;_LL29E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D.f1;if(_tmp45E->tag != 6)goto _LL2A0;else{if(_tmp45E->f1 != Cyc_Absyn_Unsigned)goto _LL2A0;}}_LL29F:
 goto _LL2A1;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D.f2;if(_tmp45F->tag != 6)goto _LL2A2;else{if(_tmp45F->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp45D.f1;if(_tmp460->tag != 19)goto _LL2A4;}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp45D.f2;if(_tmp461->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL29D;_LL2A6:;_LL2A7: {
struct _tuple18 _tmp869;struct _tuple18 _tmp463=(_tmp869.f1=_tmp431,((_tmp869.f2=_tmp432,_tmp869)));_npop_handler(0);return _tmp463;}_LL29D:;}
# 1893
switch(p){case Cyc_Absyn_Eq: _LL2A8:
 goto _LL2A9;case Cyc_Absyn_Neq: _LL2A9: goto _LL2AA;case Cyc_Absyn_Gt: _LL2AA: goto _LL2AB;case Cyc_Absyn_Gte: _LL2AB: goto _LL2AC;case Cyc_Absyn_Lt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Lte: _LL2AD: break;default: _LL2AE: {
struct _tuple18 _tmp86A;struct _tuple18 _tmp466=(_tmp86A.f1=_tmp431,((_tmp86A.f2=_tmp432,_tmp86A)));_npop_handler(0);return _tmp466;}}{
# 1898
struct _RegionHandle*_tmp467=(env->fenv)->r;
struct _tuple13 _stmttmp61=Cyc_Relations_primop2relation(_tmp42B,p,_tmp42C);struct Cyc_Absyn_Exp*_tmp469;enum Cyc_Relations_Relation _tmp46A;struct Cyc_Absyn_Exp*_tmp46B;struct _tuple13 _tmp468=_stmttmp61;_tmp469=_tmp468.f1;_tmp46A=_tmp468.f2;_tmp46B=_tmp468.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1903
if(Cyc_Relations_exp2relnop(_tmp469,& n1) && Cyc_Relations_exp2relnop(_tmp46B,& n2)){
# 1905
struct Cyc_List_List*_tmp46C=Cyc_Relations_add_relation(_tmp467,n1,_tmp46A,n2,_tmp428);
# 1909
struct Cyc_List_List*_tmp46D=Cyc_Relations_add_relation(_tmp467,n2,Cyc_Relations_flip_relation(_tmp46A),n1,_tmp428);
struct _tuple18 _tmp86B;struct _tuple18 _stmttmp62=(_tmp86B.f1=_tmp431,((_tmp86B.f2=_tmp432,_tmp86B)));struct _tuple18 _tmp46E=_stmttmp62;struct Cyc_Dict_Dict _tmp46F;struct Cyc_Dict_Dict _tmp470;struct Cyc_Dict_Dict _tmp471;struct Cyc_Dict_Dict _tmp472;_LL2B1: if(((_tmp46E.f1).ReachableFL).tag != 2)goto _LL2B3;_tmp46F=((struct _tuple14)((_tmp46E.f1).ReachableFL).val).f1;if(((_tmp46E.f2).ReachableFL).tag != 2)goto _LL2B3;_tmp470=((struct _tuple14)((_tmp46E.f2).ReachableFL).val).f1;_LL2B2: {
# 1912
struct _tuple18 _tmp86C;struct _tuple18 _tmp474=(_tmp86C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46F,_tmp46C),((_tmp86C.f2=Cyc_CfFlowInfo_ReachableFL(_tmp470,_tmp46D),_tmp86C)));_npop_handler(0);return _tmp474;}_LL2B3: if(((_tmp46E.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp471=((struct _tuple14)((_tmp46E.f2).ReachableFL).val).f1;_LL2B4: {
# 1914
struct _tuple18 _tmp86D;struct _tuple18 _tmp476=(_tmp86D.f1=_tmp431,((_tmp86D.f2=Cyc_CfFlowInfo_ReachableFL(_tmp471,_tmp46D),_tmp86D)));_npop_handler(0);return _tmp476;}_LL2B5: if(((_tmp46E.f1).ReachableFL).tag != 2)goto _LL2B7;_tmp472=((struct _tuple14)((_tmp46E.f1).ReachableFL).val).f1;_LL2B6: {
# 1916
struct _tuple18 _tmp86E;struct _tuple18 _tmp478=(_tmp86E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp472,_tmp46C),((_tmp86E.f2=_tmp432,_tmp86E)));_npop_handler(0);return _tmp478;}_LL2B7:;_LL2B8: {
# 1918
struct _tuple18 _tmp86F;struct _tuple18 _tmp47A=(_tmp86F.f1=_tmp431,((_tmp86F.f2=_tmp432,_tmp86F)));_npop_handler(0);return _tmp47A;}_LL2B0:;}else{
# 1921
struct _tuple18 _tmp870;struct _tuple18 _tmp47D=(_tmp870.f1=_tmp431,((_tmp870.f2=_tmp432,_tmp870)));_npop_handler(0);return _tmp47D;}};};};}_LL271:;};
# 1810
;_pop_region(temp);}
# 1927
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1929
struct Cyc_CfFlowInfo_FlowEnv*_tmp47E=env->fenv;
void*_stmttmp63=e->r;void*_tmp47F=_stmttmp63;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48E;enum Cyc_Absyn_Primop _tmp490;struct Cyc_List_List*_tmp491;_LL2BA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp480=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp480->tag != 5)goto _LL2BC;else{_tmp481=_tmp480->f1;_tmp482=_tmp480->f2;_tmp483=_tmp480->f3;}}_LL2BB: {
# 1932
struct _tuple18 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp481);union Cyc_CfFlowInfo_FlowInfo _tmp493;union Cyc_CfFlowInfo_FlowInfo _tmp494;struct _tuple18 _tmp492=_stmttmp6B;_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;{
struct _tuple18 _stmttmp6C=Cyc_NewControlFlow_anal_test(env,_tmp493,_tmp482);union Cyc_CfFlowInfo_FlowInfo _tmp496;union Cyc_CfFlowInfo_FlowInfo _tmp497;struct _tuple18 _tmp495=_stmttmp6C;_tmp496=_tmp495.f1;_tmp497=_tmp495.f2;{
struct _tuple18 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,_tmp494,_tmp483);union Cyc_CfFlowInfo_FlowInfo _tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;struct _tuple18 _tmp498=_stmttmp6D;_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{
struct _tuple18 _tmp871;return(_tmp871.f1=Cyc_CfFlowInfo_join_flow(_tmp47E,env->all_changed,_tmp496,_tmp499),((_tmp871.f2=
Cyc_CfFlowInfo_join_flow(_tmp47E,env->all_changed,_tmp497,_tmp49A),_tmp871)));};};};}_LL2BC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp484=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp484->tag != 6)goto _LL2BE;else{_tmp485=_tmp484->f1;_tmp486=_tmp484->f2;}}_LL2BD: {
# 1938
struct _tuple18 _stmttmp69=Cyc_NewControlFlow_anal_test(env,inflow,_tmp485);union Cyc_CfFlowInfo_FlowInfo _tmp49D;union Cyc_CfFlowInfo_FlowInfo _tmp49E;struct _tuple18 _tmp49C=_stmttmp69;_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;{
struct _tuple18 _stmttmp6A=Cyc_NewControlFlow_anal_test(env,_tmp49D,_tmp486);union Cyc_CfFlowInfo_FlowInfo _tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp4A1;struct _tuple18 _tmp49F=_stmttmp6A;_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;{
struct _tuple18 _tmp872;return(_tmp872.f1=_tmp4A0,((_tmp872.f2=Cyc_CfFlowInfo_join_flow(_tmp47E,env->all_changed,_tmp49E,_tmp4A1),_tmp872)));};};}_LL2BE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp487=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp487->tag != 7)goto _LL2C0;else{_tmp488=_tmp487->f1;_tmp489=_tmp487->f2;}}_LL2BF: {
# 1942
struct _tuple18 _stmttmp67=Cyc_NewControlFlow_anal_test(env,inflow,_tmp488);union Cyc_CfFlowInfo_FlowInfo _tmp4A4;union Cyc_CfFlowInfo_FlowInfo _tmp4A5;struct _tuple18 _tmp4A3=_stmttmp67;_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;{
struct _tuple18 _stmttmp68=Cyc_NewControlFlow_anal_test(env,_tmp4A5,_tmp489);union Cyc_CfFlowInfo_FlowInfo _tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;struct _tuple18 _tmp4A6=_stmttmp68;_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{
struct _tuple18 _tmp873;return(_tmp873.f1=Cyc_CfFlowInfo_join_flow(_tmp47E,env->all_changed,_tmp4A4,_tmp4A7),((_tmp873.f2=_tmp4A8,_tmp873)));};};}_LL2C0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp48A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp48A->tag != 8)goto _LL2C2;else{_tmp48B=_tmp48A->f1;_tmp48C=_tmp48A->f2;}}_LL2C1: {
# 1946
struct _tuple15 _stmttmp66=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp48B);union Cyc_CfFlowInfo_FlowInfo _tmp4AB;void*_tmp4AC;struct _tuple15 _tmp4AA=_stmttmp66;_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4AB,_tmp48C);}_LL2C2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp48D->tag != 2)goto _LL2C4;else{if(_tmp48D->f1 != Cyc_Absyn_Not)goto _LL2C4;if(_tmp48D->f2 == 0)goto _LL2C4;_tmp48E=(struct Cyc_Absyn_Exp*)(_tmp48D->f2)->hd;if((_tmp48D->f2)->tl != 0)goto _LL2C4;}}_LL2C3: {
# 1949
struct _tuple18 _stmttmp65=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48E);union Cyc_CfFlowInfo_FlowInfo _tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;struct _tuple18 _tmp4AD=_stmttmp65;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{
struct _tuple18 _tmp874;return(_tmp874.f1=_tmp4AF,((_tmp874.f2=_tmp4AE,_tmp874)));};}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47F;if(_tmp48F->tag != 2)goto _LL2C6;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;}}_LL2C5:
# 1952
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp490,_tmp491);_LL2C6:;_LL2C7: {
# 1956
struct _tuple15 _stmttmp64=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4B2;void*_tmp4B3;struct _tuple15 _tmp4B1=_stmttmp64;_tmp4B2=_tmp4B1.f1;_tmp4B3=_tmp4B1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B4=_tmp4B2;struct Cyc_Dict_Dict _tmp4B5;_LL2C9: if((_tmp4B4.BottomFL).tag != 1)goto _LL2CB;_LL2CA: {
struct _tuple18 _tmp875;return(_tmp875.f1=_tmp4B2,((_tmp875.f2=_tmp4B2,_tmp875)));}_LL2CB: if((_tmp4B4.ReachableFL).tag != 2)goto _LL2C8;_tmp4B5=((struct _tuple14)(_tmp4B4.ReachableFL).val).f1;_LL2CC: {
# 1960
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4B7=_tmp4B3;struct Cyc_Absyn_Vardecl*_tmp4B9;void*_tmp4BA;enum Cyc_CfFlowInfo_InitLevel _tmp4C3;_LL2CE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4B8->tag != 8)goto _LL2D0;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=(void*)_tmp4B8->f2;}}_LL2CF:
 _tmp4B3=_tmp4BA;locname=_tmp4B9;goto retry;_LL2D0: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4BB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4BB->tag != 0)goto _LL2D2;}_LL2D1: {
struct _tuple18 _tmp876;return(_tmp876.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp876.f2=_tmp4B2,_tmp876)));}_LL2D2: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4BC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4BC->tag != 2)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4BD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4BD->tag != 1)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4BE->tag != 5)goto _LL2D8;}_LL2D7: {
struct _tuple18 _tmp877;return(_tmp877.f1=_tmp4B2,((_tmp877.f2=Cyc_CfFlowInfo_BottomFL(),_tmp877)));}_LL2D8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4BF->tag != 3)goto _LL2DA;else{if(_tmp4BF->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DA;}}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4C0->tag != 4)goto _LL2DC;else{if(_tmp4C0->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4C1=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4C1->tag != 7)goto _LL2DE;}_LL2DD:
# 1970
{const char*_tmp87A;void*_tmp879;(_tmp879=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp87A="expression may not be initialized",_tag_dyneither(_tmp87A,sizeof(char),34))),_tag_dyneither(_tmp879,sizeof(void*),0)));}{
struct _tuple18 _tmp87B;return(_tmp87B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87B.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87B)));};_LL2DE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4C2->tag != 3)goto _LL2E0;else{_tmp4C3=_tmp4C2->f1;}}_LL2DF:
# 1973
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B2,e,_tmp4C3,locname);_LL2E0: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C4=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4C4->tag != 4)goto _LL2E2;}_LL2E1: {
struct _tuple18 _tmp87C;return(_tmp87C.f1=_tmp4B2,((_tmp87C.f2=_tmp4B2,_tmp87C)));}_LL2E2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B7;if(_tmp4C5->tag != 6)goto _LL2CD;}_LL2E3: {
struct Cyc_Core_Impossible_exn_struct _tmp882;const char*_tmp881;struct Cyc_Core_Impossible_exn_struct*_tmp880;(int)_throw((void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp882.tag=Cyc_Core_Impossible,((_tmp882.f1=((_tmp881="anal_test",_tag_dyneither(_tmp881,sizeof(char),10))),_tmp882)))),_tmp880)))));}_LL2CD:;}}_LL2C8:;};}_LL2B9:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1981
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 1983
unsigned int _tmp4D0;struct Cyc_NewControlFlow_AnalEnv*_tmp4D1;struct Cyc_Dict_Dict _tmp4D2;struct _tuple26*_tmp4CF=ckenv;_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f2;_tmp4D2=_tmp4CF->f3;{
void*_tmp4D3=root;struct Cyc_Absyn_Vardecl*_tmp4D5;_LL2E5: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D4=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4D3;if(_tmp4D4->tag != 0)goto _LL2E7;else{_tmp4D5=_tmp4D4->f1;}}_LL2E6:
# 1986
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4D1->fenv)->r,_tmp4D5->type)){
void*_tmp4D6=rval;_LL2EA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4D7->tag != 8)goto _LL2EC;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D7->f2);if(_tmp4D8->tag != 7)goto _LL2EC;}}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4D9->tag != 8)goto _LL2EE;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D9->f2);if(_tmp4DA->tag != 0)goto _LL2EE;}}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4DB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4DB->tag != 8)goto _LL2F0;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4DC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4DB->f2);if(_tmp4DC->tag != 3)goto _LL2F0;else{if(_tmp4DC->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F0;}}}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4DD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4DD->tag != 7)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4DE->tag != 0)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4DF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D6;if(_tmp4DF->tag != 3)goto _LL2F6;else{if(_tmp4DF->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F6;}}_LL2F5:
 goto _LL2E9;_LL2F6:;_LL2F7:
# 1995
{const char*_tmp886;void*_tmp885[1];struct Cyc_String_pa_PrintArg_struct _tmp884;(_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D5->name)),((_tmp885[0]=& _tmp884,Cyc_Tcutil_warn(_tmp4D0,((_tmp886="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp886,sizeof(char),57))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));}
goto _LL2E9;_LL2E9:;}
# 1998
goto _LL2E4;_LL2E7:;_LL2E8:
 goto _LL2E4;_LL2E4:;};}
# 2003
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4E3=flow;struct Cyc_Dict_Dict _tmp4E4;_LL2F9: if((_tmp4E3.BottomFL).tag != 1)goto _LL2FB;_LL2FA:
 return;_LL2FB: if((_tmp4E3.ReachableFL).tag != 2)goto _LL2F8;_tmp4E4=((struct _tuple14)(_tmp4E3.ReachableFL).val).f1;_LL2FC:
# 2007
{struct Cyc_List_List*_tmp4E5=env->param_roots;for(0;_tmp4E5 != 0;_tmp4E5=_tmp4E5->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E4,Cyc_CfFlowInfo_lookup_place(_tmp4E4,(struct Cyc_CfFlowInfo_Place*)_tmp4E5->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp889;void*_tmp888;(_tmp888=0,Cyc_CfFlowInfo_aerr(loc,((_tmp889="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp889,sizeof(char),76))),_tag_dyneither(_tmp888,sizeof(void*),0)));}}}
# 2011
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp88A;struct _tuple26 _tmp4E8=(_tmp88A.f1=loc,((_tmp88A.f2=env,((_tmp88A.f3=_tmp4E4,_tmp88A)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4E8,_tmp4E4);}
# 2015
return;_LL2F8:;}
# 2019
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2022
struct Cyc_List_List*_tmp4EA=0;
struct Cyc_List_List*_tmp4EB=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp6E=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct _tuple23*_tmp4EC=_stmttmp6E;_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;
if((_tmp4ED != 0  && 
# 2029
Cyc_Tcutil_is_noalias_pointer((*_tmp4ED)->type,0)) && _tmp4EE != 0){
# 2031
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp88D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp88C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4EF=(_tmp88C=_region_malloc(frgn,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88D.tag=0,((_tmp88D.f1=*_tmp4ED,_tmp88D)))),_tmp88C)));
struct Cyc_CfFlowInfo_Place*_tmp88E;struct Cyc_CfFlowInfo_Place*_tmp4F0=(_tmp88E=_region_malloc(frgn,sizeof(*_tmp88E)),((_tmp88E->root=(void*)_tmp4EF,((_tmp88E->fields=0,_tmp88E)))));
{struct Cyc_List_List*_tmp88F;_tmp4EA=((_tmp88F=_region_malloc(frgn,sizeof(*_tmp88F)),((_tmp88F->hd=_tmp4F0,((_tmp88F->tl=_tmp4EA,_tmp88F))))));}{
struct Cyc_List_List*_tmp890;_tmp4EB=((_tmp890=_region_malloc(frgn,sizeof(*_tmp890)),((_tmp890->hd=_tmp4EE,((_tmp890->tl=_tmp4EB,_tmp890))))));};}}}{
# 2037
struct _tuple1 _tmp891;return(_tmp891.f1=_tmp4EA,((_tmp891.f2=_tmp4EB,_tmp891)));};}
# 2046
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2051
{union Cyc_CfFlowInfo_FlowInfo _tmp4F7=flow;_LL2FE: if((_tmp4F7.BottomFL).tag != 1)goto _LL300;_LL2FF:
 return flow;_LL300:;_LL301:
 goto _LL2FD;_LL2FD:;}{
# 2055
int _tmp4F8=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp4F9=Cyc_Absyn_exp_unsigned_one;
int _tmp4FA=0;
int _tmp4FB=1;
# 2062
{struct Cyc_List_List*_tmp4FC=consumed_vals;for(0;_tmp4FC != 0;
(_tmp4FC=_tmp4FC->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp4FD=flow;struct Cyc_Dict_Dict _tmp4FE;_LL303: if((_tmp4FD.ReachableFL).tag != 2)goto _LL305;_tmp4FE=((struct _tuple14)(_tmp4FD.ReachableFL).val).f1;_LL304: {
# 2072
void*_tmp4FF=Cyc_CfFlowInfo_lookup_place(_tmp4FE,(struct Cyc_CfFlowInfo_Place*)_tmp4FC->hd);
struct Cyc_CfFlowInfo_Place*_stmttmp6F=(struct Cyc_CfFlowInfo_Place*)_tmp4FC->hd;void*_tmp501;struct Cyc_CfFlowInfo_Place*_tmp500=_stmttmp6F;_tmp501=_tmp500->root;
{struct _tuple0 _tmp892;struct _tuple0 _stmttmp70=(_tmp892.f1=_tmp501,((_tmp892.f2=_tmp4FF,_tmp892)));struct _tuple0 _tmp502=_stmttmp70;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Vardecl*_tmp506;void*_tmp507;struct Cyc_Absyn_Vardecl*_tmp509;_LL308:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp503=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp502.f1;if(_tmp503->tag != 0)goto _LL30A;else{_tmp504=_tmp503->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp505=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp502.f2;if(_tmp505->tag != 8)goto _LL30A;else{_tmp506=_tmp505->f1;_tmp507=(void*)_tmp505->f2;}};_LL309:
# 2076
 if(_tmp504 == _tmp506){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4F9,_tmp4FB,_tmp507,& _tmp4FA)){
if(!_tmp4F8){
const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_PrintArg_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2081
Cyc_Absynpp_qvar2string(_tmp504->name)),((_tmp895[0]=& _tmp894,Cyc_CfFlowInfo_aerr(loc,((_tmp896="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp896,sizeof(char),58))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}}else{
# 2083
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4FE,_tmp507)!= Cyc_CfFlowInfo_AllIL  && !_tmp4F8){
const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_PrintArg_struct _tmp898;(_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2086
Cyc_Absynpp_qvar2string(_tmp504->name)),((_tmp899[0]=& _tmp898,Cyc_CfFlowInfo_aerr(loc,((_tmp89A="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp89A,sizeof(char),84))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));}else{
# 2088
if(_tmp4F8){
# 2094
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp511;union Cyc_CfFlowInfo_AbsLVal _tmp512;struct _tuple17 _tmp510=_stmttmp71;_tmp511=_tmp510.f1;_tmp512=_tmp510.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp513=_tmp512;struct Cyc_CfFlowInfo_Place*_tmp514;_LL30F: if((_tmp513.PlaceL).tag != 1)goto _LL311;_tmp514=(struct Cyc_CfFlowInfo_Place*)(_tmp513.PlaceL).val;_LL310: {
# 2098
void*_tmp515=Cyc_CfFlowInfo_lookup_place(_tmp4FE,_tmp514);
_tmp515=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp515);{
# 2106
struct _tuple15 _tmp89C;struct _tuple15 _tmp89B;struct _tuple15 _stmttmp72=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2109
(_tmp89B.f1=_tmp511,((_tmp89B.f2=_tmp515,_tmp89B)))),(
(_tmp89C.f1=_tmp511,((_tmp89C.f2=_tmp507,_tmp89C)))));
# 2106
union Cyc_CfFlowInfo_FlowInfo _tmp517;void*_tmp518;struct _tuple15 _tmp516=_stmttmp72;_tmp517=_tmp516.f1;_tmp518=_tmp516.f2;
# 2111
{union Cyc_CfFlowInfo_FlowInfo _tmp519=_tmp517;struct Cyc_Dict_Dict _tmp51A;struct Cyc_List_List*_tmp51B;_LL314: if((_tmp519.ReachableFL).tag != 2)goto _LL316;_tmp51A=((struct _tuple14)(_tmp519.ReachableFL).val).f1;_tmp51B=((struct _tuple14)(_tmp519.ReachableFL).val).f2;_LL315:
# 2113
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp51A,env->all_changed,_tmp514,_tmp518),_tmp51B);
# 2117
goto _LL313;_LL316:;_LL317: {
# 2119
const char*_tmp89F;void*_tmp89E;(_tmp89E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp89F="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp89F,sizeof(char),42))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}_LL313:;}
# 2121
goto _LL30E;};}_LL311:;_LL312:
# 2126
 goto _LL30E;_LL30E:;};}}}}else{
# 2130
_tmp509=_tmp504;goto _LL30B;}
goto _LL307;_LL30A: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp508=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp502.f1;if(_tmp508->tag != 0)goto _LL30C;else{_tmp509=_tmp508->f1;}}_LL30B:
# 2133
 if(!_tmp4F8){
const char*_tmp8A3;void*_tmp8A2[1];struct Cyc_String_pa_PrintArg_struct _tmp8A1;(_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2136
Cyc_Absynpp_qvar2string(_tmp509->name)),((_tmp8A2[0]=& _tmp8A1,Cyc_CfFlowInfo_aerr(loc,((_tmp8A3="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8A3,sizeof(char),85))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))));}
goto _LL307;_LL30C:;_LL30D: {
# 2139
const char*_tmp8A6;void*_tmp8A5;(_tmp8A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A6="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8A6,sizeof(char),40))),_tag_dyneither(_tmp8A5,sizeof(void*),0)));}_LL307:;}
# 2141
goto _LL302;}_LL305: if((_tmp4FD.BottomFL).tag != 1)goto _LL302;_LL306: {
# 2143
const char*_tmp8A9;void*_tmp8A8;(_tmp8A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A9="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp8A9,sizeof(char),38))),_tag_dyneither(_tmp8A8,sizeof(void*),0)));}_LL302:;}}
# 2146
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2149
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2152
struct Cyc_CfFlowInfo_FlowEnv*_tmp528=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp73=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Stmt*_tmp52C;unsigned int _tmp52D;struct Cyc_Absyn_Switch_clause*_tmp529=_stmttmp73;_tmp52A=_tmp529->pat_vars;_tmp52B=_tmp529->where_clause;_tmp52C=_tmp529->body;_tmp52D=_tmp529->loc;{
struct _tuple1 _stmttmp74=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp52A))->v);struct Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp530;struct _tuple1 _tmp52E=_stmttmp74;_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp52A->v,_tmp52F != 0,_tmp52D,exp_loc);
# 2160
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp531=env->unique_pat_vars;
{struct _tuple27*_tmp8AC;struct Cyc_List_List*_tmp8AB;env->unique_pat_vars=(
(_tmp8AB=_region_malloc(env->r,sizeof(*_tmp8AB)),((_tmp8AB->hd=((_tmp8AC=_region_malloc(env->r,sizeof(*_tmp8AC)),((_tmp8AC->f1=_tmp52C,((_tmp8AC->f2=_tmp52F,((_tmp8AC->f3=_tmp530,_tmp8AC)))))))),((_tmp8AB->tl=_tmp531,_tmp8AB))))));}
# 2165
if(_tmp52B != 0){
struct Cyc_Absyn_Exp*wexp=_tmp52B;
struct _tuple18 _stmttmp75=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp535;union Cyc_CfFlowInfo_FlowInfo _tmp536;struct _tuple18 _tmp534=_stmttmp75;_tmp535=_tmp534.f1;_tmp536=_tmp534.f2;
inflow=_tmp536;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp535,_tmp52C,0);}else{
# 2171
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp52C,0);}
# 2173
env->unique_pat_vars=_tmp531;{
union Cyc_CfFlowInfo_FlowInfo _tmp537=clause_outflow;_LL319: if((_tmp537.BottomFL).tag != 1)goto _LL31B;_LL31A:
 goto _LL318;_LL31B:;_LL31C:
# 2178
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp52F,_tmp530,clause_outflow,_tmp52D);
# 2180
if(scs->tl == 0)
return clause_outflow;else{
# 2185
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8AF;void*_tmp8AE;(_tmp8AE=0,Cyc_CfFlowInfo_aerr(_tmp52C->loc,((_tmp8AF="switch clause may implicitly fallthru",_tag_dyneither(_tmp8AF,sizeof(char),38))),_tag_dyneither(_tmp8AE,sizeof(void*),0)));}else{
# 2188
const char*_tmp8B2;void*_tmp8B1;(_tmp8B1=0,Cyc_Tcutil_warn(_tmp52C->loc,((_tmp8B2="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B2,sizeof(char),38))),_tag_dyneither(_tmp8B1,sizeof(void*),0)));}
# 2190
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2192
goto _LL318;_LL318:;};};};}
# 2195
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2200
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2202
struct Cyc_NewControlFlow_AnalEnv*_tmp53D;struct Cyc_Dict_Dict _tmp53E;unsigned int _tmp53F;struct _tuple28*_tmp53C=vdenv;_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;_tmp53F=_tmp53C->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp53D->fenv)->r,vd->type)){
# 2205
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8B3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8B3.tag=0,((_tmp8B3.f1=vd,_tmp8B3)));
# 2207
void*_tmp540=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp53E,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp541=_tmp540;_LL31E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp542=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp542->tag != 8)goto _LL320;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp543=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp542->f2);if(_tmp543->tag != 7)goto _LL320;}}_LL31F:
 goto _LL321;_LL320: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp544=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp544->tag != 8)goto _LL322;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp545=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp544->f2);if(_tmp545->tag != 0)goto _LL322;}}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp546=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp546->tag != 8)goto _LL324;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp547=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp546->f2);if(_tmp547->tag != 3)goto _LL324;else{if(_tmp547->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL324;}}}_LL323:
 goto _LL325;_LL324: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp548=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp548->tag != 7)goto _LL326;}_LL325:
 goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp549=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp549->tag != 0)goto _LL328;}_LL327:
 goto _LL329;_LL328: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp54A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp541;if(_tmp54A->tag != 3)goto _LL32A;else{if(_tmp54A->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32A;}}_LL329:
 goto _LL31D;_LL32A:;_LL32B:
# 2216
{const char*_tmp8B7;void*_tmp8B6[1];struct Cyc_String_pa_PrintArg_struct _tmp8B5;(_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8B6[0]=& _tmp8B5,Cyc_Tcutil_warn(_tmp53F,((_tmp8B7="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8B7,sizeof(char),68))),_tag_dyneither(_tmp8B6,sizeof(void*),1)))))));}
# 2218
goto _LL31D;_LL31D:;}}
# 2223
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2225
{union Cyc_CfFlowInfo_FlowInfo _tmp54F=inflow;struct Cyc_Dict_Dict _tmp550;_LL32D: if((_tmp54F.ReachableFL).tag != 2)goto _LL32F;_tmp550=((struct _tuple14)(_tmp54F.ReachableFL).val).f1;_LL32E: {
# 2227
struct _tuple28 _tmp8B8;struct _tuple28 _tmp551=(_tmp8B8.f1=env,((_tmp8B8.f2=_tmp550,((_tmp8B8.f3=decl->loc,_tmp8B8)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp552=env->fenv;
{void*_stmttmp76=decl->r;void*_tmp553=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp555;struct Cyc_List_List*_tmp557;struct Cyc_List_List*_tmp559;_LL332: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp554=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp553;if(_tmp554->tag != 0)goto _LL334;else{_tmp555=_tmp554->f1;}}_LL333:
# 2231
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp551,_tmp555);
goto _LL331;_LL334: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp556=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp553;if(_tmp556->tag != 2)goto _LL336;else{if(_tmp556->f2 == 0)goto _LL336;_tmp557=(struct Cyc_List_List*)(_tmp556->f2)->v;}}_LL335: {
# 2234
struct _tuple1 _stmttmp77=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp557);struct Cyc_List_List*_tmp55B;struct _tuple1 _tmp55A=_stmttmp77;_tmp55B=_tmp55A.f1;{
struct Cyc_List_List*_tmp55C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp55B);
_tmp559=_tmp55C;goto _LL337;};}_LL336: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp558=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp553;if(_tmp558->tag != 3)goto _LL338;else{_tmp559=_tmp558->f1;}}_LL337:
# 2238
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp551,_tmp559);
goto _LL331;_LL338:;_LL339:
 goto _LL331;_LL331:;}
# 2242
goto _LL32C;}_LL32F:;_LL330:
 goto _LL32C;_LL32C:;}
# 2245
return;}
# 2251
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2254
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_stmttmp78=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp55F;struct Cyc_List_List*_tmp560;struct Cyc_List_List*_tmp561;struct _tuple27*_tmp55E=_stmttmp78;_tmp55F=_tmp55E->f1;_tmp560=_tmp55E->f2;_tmp561=_tmp55E->f3;
keep_going=0;{
# 2262
struct Cyc_Absyn_Stmt*_tmp562=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp562 != _tmp55F){
struct Cyc_Absyn_Stmt*_tmp563=(Cyc_NewControlFlow_get_stmt_annot(_tmp562))->encloser;
if(_tmp563 == _tmp562){
# 2268
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp560,_tmp561,inflow,dest->loc);
keep_going=1;
break;}
# 2272
_tmp562=_tmp563;}};}}
# 2275
return inflow;}
# 2281
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2284
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _stmttmp79=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp565;union Cyc_CfFlowInfo_FlowInfo*_tmp566;struct _tuple19 _tmp564=_stmttmp79;_tmp565=_tmp564.f1;_tmp566=_tmp564.f2;
inflow=*_tmp566;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp567=env->fenv;
# 2291
void*_stmttmp7A=s->r;void*_tmp568=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Stmt*_tmp571;struct Cyc_Absyn_Stmt*_tmp572;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Stmt*_tmp575;struct Cyc_Absyn_Stmt*_tmp576;struct Cyc_Absyn_Exp*_tmp578;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_Absyn_Stmt*_tmp57A;struct Cyc_Absyn_Stmt*_tmp57C;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Stmt*_tmp57E;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Stmt*_tmp582;struct Cyc_Absyn_Exp*_tmp583;struct Cyc_Absyn_Stmt*_tmp584;struct Cyc_Absyn_Stmt*_tmp585;struct Cyc_List_List*_tmp587;struct Cyc_Absyn_Switch_clause*_tmp588;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Absyn_Stmt*_tmp58C;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Exp*_tmp590;struct Cyc_List_List*_tmp591;struct Cyc_Absyn_Stmt*_tmp593;struct Cyc_List_List*_tmp594;struct Cyc_List_List*_tmp597;struct Cyc_Absyn_Exp*_tmp598;unsigned int _tmp599;struct Cyc_Absyn_Stmt*_tmp59A;struct Cyc_Absyn_Tvar*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59E;struct Cyc_Absyn_Stmt*_tmp59F;struct Cyc_Absyn_Decl*_tmp5A1;struct Cyc_Absyn_Stmt*_tmp5A2;struct Cyc_Absyn_Stmt*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;_LL33B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp569->tag != 0)goto _LL33D;}_LL33C:
 return inflow;_LL33D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp56A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp56A->tag != 6)goto _LL33F;else{if(_tmp56A->f1 != 0)goto _LL33F;}}_LL33E:
# 2297
{union Cyc_CfFlowInfo_FlowInfo _tmp5A7=inflow;_LL368: if((_tmp5A7.ReachableFL).tag != 2)goto _LL36A;_LL369:
# 2299
{void*_stmttmp8F=Cyc_Tcutil_compress(env->return_type);void*_tmp5A8=_stmttmp8F;_LL36D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5A8;if(_tmp5A9->tag != 0)goto _LL36F;}_LL36E:
 goto _LL36C;_LL36F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A8;if(_tmp5AA->tag != 7)goto _LL371;}_LL370:
 goto _LL372;_LL371: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A8;if(_tmp5AB->tag != 6)goto _LL373;}_LL372:
# 2303
{const char*_tmp8BB;void*_tmp8BA;(_tmp8BA=0,Cyc_Tcutil_warn(s->loc,((_tmp8BB="break may cause function not to return a value",_tag_dyneither(_tmp8BB,sizeof(char),47))),_tag_dyneither(_tmp8BA,sizeof(void*),0)));}
goto _LL36C;_LL373:;_LL374:
# 2306
{const char*_tmp8BE;void*_tmp8BD;(_tmp8BD=0,Cyc_Tcutil_terr(s->loc,((_tmp8BE="break may cause function not to return a value",_tag_dyneither(_tmp8BE,sizeof(char),47))),_tag_dyneither(_tmp8BD,sizeof(void*),0)));}
goto _LL36C;_LL36C:;}
# 2309
goto _LL367;_LL36A:;_LL36B:
 goto _LL367;_LL367:;}
# 2312
goto _LL340;_LL33F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp56B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp56B->tag != 3)goto _LL341;else{if(_tmp56B->f1 != 0)goto _LL341;}}_LL340:
# 2314
 if(env->noreturn){
const char*_tmp8C1;void*_tmp8C0;(_tmp8C0=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8C1="`noreturn' function might return",_tag_dyneither(_tmp8C1,sizeof(char),33))),_tag_dyneither(_tmp8C0,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL341: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp56C=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp56C->tag != 3)goto _LL343;else{_tmp56D=_tmp56C->f1;}}_LL342:
# 2320
 if(env->noreturn){
const char*_tmp8C4;void*_tmp8C3;(_tmp8C3=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8C4="`noreturn' function might return",_tag_dyneither(_tmp8C4,sizeof(char),33))),_tag_dyneither(_tmp8C3,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp56D));union Cyc_CfFlowInfo_FlowInfo _tmp5B5;void*_tmp5B6;struct _tuple15 _tmp5B4=_stmttmp8E;_tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;
_tmp5B5=Cyc_NewControlFlow_use_Rval(env,_tmp56D->loc,_tmp5B5,_tmp5B6);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5B5);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp5B5,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL343: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp56E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp56E->tag != 1)goto _LL345;else{_tmp56F=_tmp56E->f1;}}_LL344: {
# 2329
struct _tuple16*_tmp5B7=rval_opt;void**_tmp5B8;int _tmp5B9;_LL376: if(_tmp5B7 == 0)goto _LL378;_tmp5B8=(void**)& _tmp5B7->f1;_tmp5B9=_tmp5B7->f2;_LL377: {
# 2331
struct _tuple15 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,_tmp5B9,inflow,_tmp56F);union Cyc_CfFlowInfo_FlowInfo _tmp5BB;void*_tmp5BC;struct _tuple15 _tmp5BA=_stmttmp8D;_tmp5BB=_tmp5BA.f1;_tmp5BC=_tmp5BA.f2;
*_tmp5B8=_tmp5BC;
return _tmp5BB;}_LL378: if(_tmp5B7 != 0)goto _LL375;_LL379:
# 2335
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp56F)).f1;_LL375:;}_LL345: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp570=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp570->tag != 2)goto _LL347;else{_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;}}_LL346:
# 2339
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp571,0),_tmp572,rval_opt);_LL347: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp573=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp573->tag != 4)goto _LL349;else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;_tmp576=_tmp573->f3;}}_LL348: {
# 2342
struct _tuple18 _stmttmp8C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp574);union Cyc_CfFlowInfo_FlowInfo _tmp5BE;union Cyc_CfFlowInfo_FlowInfo _tmp5BF;struct _tuple18 _tmp5BD=_stmttmp8C;_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;{
# 2349
union Cyc_CfFlowInfo_FlowInfo _tmp5C0=Cyc_NewControlFlow_anal_stmt(env,_tmp5BF,_tmp576,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5C1=Cyc_NewControlFlow_anal_stmt(env,_tmp5BE,_tmp575,0);
return Cyc_CfFlowInfo_join_flow(_tmp567,env->all_changed,_tmp5C1,_tmp5C0);};}_LL349: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp577=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp577->tag != 5)goto _LL34B;else{_tmp578=(_tmp577->f1).f1;_tmp579=(_tmp577->f1).f2;_tmp57A=_tmp577->f2;}}_LL34A: {
# 2357
struct _tuple19 _stmttmp8A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp579);union Cyc_CfFlowInfo_FlowInfo*_tmp5C3;struct _tuple19 _tmp5C2=_stmttmp8A;_tmp5C3=_tmp5C2.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C4=*_tmp5C3;
struct _tuple18 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,_tmp5C4,_tmp578);union Cyc_CfFlowInfo_FlowInfo _tmp5C6;union Cyc_CfFlowInfo_FlowInfo _tmp5C7;struct _tuple18 _tmp5C5=_stmttmp8B;_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C8=Cyc_NewControlFlow_anal_stmt(env,_tmp5C6,_tmp57A,0);
Cyc_NewControlFlow_update_flow(env,_tmp579,_tmp5C8);
return _tmp5C7;};};}_LL34B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp57B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp57B->tag != 14)goto _LL34D;else{_tmp57C=_tmp57B->f1;_tmp57D=(_tmp57B->f2).f1;_tmp57E=(_tmp57B->f2).f2;}}_LL34C: {
# 2367
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp57C,0);
struct _tuple19 _stmttmp88=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C9,_tmp57E);union Cyc_CfFlowInfo_FlowInfo*_tmp5CB;struct _tuple19 _tmp5CA=_stmttmp88;_tmp5CB=_tmp5CA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CC=*_tmp5CB;
struct _tuple18 _stmttmp89=Cyc_NewControlFlow_anal_test(env,_tmp5CC,_tmp57D);union Cyc_CfFlowInfo_FlowInfo _tmp5CE;union Cyc_CfFlowInfo_FlowInfo _tmp5CF;struct _tuple18 _tmp5CD=_stmttmp89;_tmp5CE=_tmp5CD.f1;_tmp5CF=_tmp5CD.f2;
Cyc_NewControlFlow_update_flow(env,_tmp57C,_tmp5CE);
return _tmp5CF;};}_LL34D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp57F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp57F->tag != 9)goto _LL34F;else{_tmp580=_tmp57F->f1;_tmp581=(_tmp57F->f2).f1;_tmp582=(_tmp57F->f2).f2;_tmp583=(_tmp57F->f3).f1;_tmp584=(_tmp57F->f3).f2;_tmp585=_tmp57F->f4;}}_LL34E: {
# 2376
union Cyc_CfFlowInfo_FlowInfo _tmp5D0=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp580)).f1;
struct _tuple19 _stmttmp85=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5D0,_tmp582);union Cyc_CfFlowInfo_FlowInfo*_tmp5D2;struct _tuple19 _tmp5D1=_stmttmp85;_tmp5D2=_tmp5D1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D3=*_tmp5D2;
struct _tuple18 _stmttmp86=Cyc_NewControlFlow_anal_test(env,_tmp5D3,_tmp581);union Cyc_CfFlowInfo_FlowInfo _tmp5D5;union Cyc_CfFlowInfo_FlowInfo _tmp5D6;struct _tuple18 _tmp5D4=_stmttmp86;_tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D7=Cyc_NewControlFlow_anal_stmt(env,_tmp5D5,_tmp585,0);
struct _tuple19 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5D7,_tmp584);union Cyc_CfFlowInfo_FlowInfo*_tmp5D9;struct _tuple19 _tmp5D8=_stmttmp87;_tmp5D9=_tmp5D8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DA=*_tmp5D9;
union Cyc_CfFlowInfo_FlowInfo _tmp5DB=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5DA,_tmp583)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp582,_tmp5DB);
return _tmp5D6;};};};}_LL34F: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp586=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp586->tag != 11)goto _LL351;else{_tmp587=_tmp586->f1;if(_tmp586->f2 == 0)goto _LL351;_tmp588=*_tmp586->f2;}}_LL350: {
# 2388
struct _RegionHandle _tmp5DC=_new_region("temp");struct _RegionHandle*temp=& _tmp5DC;_push_region(temp);
{struct _tuple22 _stmttmp83=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp587,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5DE;struct Cyc_List_List*_tmp5DF;struct _tuple22 _tmp5DD=_stmttmp83;_tmp5DE=_tmp5DD.f1;_tmp5DF=_tmp5DD.f2;
# 2391
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp588->body);{
# 2393
struct Cyc_List_List*_tmp5E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp588->pat_vars))->v)).f1);
_tmp5DE=Cyc_NewControlFlow_add_vars(_tmp567,_tmp5DE,_tmp5E0,_tmp567->unknown_all,s->loc,0);
# 2396
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp588->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp84=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct _tuple23*_tmp5E1=_stmttmp84;_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;
if(_tmp5E2 != 0){
_tmp5DE=Cyc_NewControlFlow_do_initialize_var(_tmp567,env,_tmp5DE,*_tmp5E2,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp587))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5DF))->hd,s->loc);
_tmp5DF=_tmp5DF->tl;
_tmp587=_tmp587->tl;}}}
# 2404
Cyc_NewControlFlow_update_flow(env,_tmp588->body,_tmp5DE);{
union Cyc_CfFlowInfo_FlowInfo _tmp5E4=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5E4;};};}
# 2389
;_pop_region(temp);}_LL351: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp589=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp589->tag != 6)goto _LL353;else{_tmp58A=_tmp589->f1;}}_LL352:
# 2407
 _tmp58C=_tmp58A;goto _LL354;_LL353: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp58B->tag != 7)goto _LL355;else{_tmp58C=_tmp58B->f1;}}_LL354:
 _tmp58E=_tmp58C;goto _LL356;_LL355: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp58D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp58D->tag != 8)goto _LL357;else{_tmp58E=_tmp58D->f2;}}_LL356:
# 2412
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5E5=_tmp565->encloser;
struct Cyc_Absyn_Stmt*_tmp5E6=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp58E)))->encloser;
while(_tmp5E6 != _tmp5E5){
struct Cyc_Absyn_Stmt*_tmp5E7=(Cyc_NewControlFlow_get_stmt_annot(_tmp5E5))->encloser;
if(_tmp5E7 == _tmp5E5){
{const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8C7="goto enters local scope or exception handler",_tag_dyneither(_tmp8C7,sizeof(char),45))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}
break;}
# 2421
_tmp5E5=_tmp5E7;}}
# 2425
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp58E));
# 2427
Cyc_NewControlFlow_update_flow(env,_tmp58E,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL357: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp58F=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp58F->tag != 10)goto _LL359;else{_tmp590=_tmp58F->f1;_tmp591=_tmp58F->f2;}}_LL358:
# 2433
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp591,_tmp590->loc);_LL359: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp592=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp592->tag != 15)goto _LL35B;else{_tmp593=_tmp592->f1;_tmp594=_tmp592->f2;}}_LL35A: {
# 2438
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp593,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2446
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2449
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp594,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5EA=scs_outflow;_LL37B: if((_tmp5EA.BottomFL).tag != 1)goto _LL37D;_LL37C:
 goto _LL37A;_LL37D:;_LL37E: {
const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8CA="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8CA,sizeof(char),42))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}_LL37A:;}
# 2455
outflow=s1_outflow;
# 2457
return outflow;};};}_LL35B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp595=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp595->tag != 12)goto _LL35D;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp596=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp595->f1)->r;if(_tmp596->tag != 2)goto _LL35D;else{if(_tmp596->f2 == 0)goto _LL35D;_tmp597=(struct Cyc_List_List*)(_tmp596->f2)->v;_tmp598=_tmp596->f3;}}_tmp599=(_tmp595->f1)->loc;_tmp59A=_tmp595->f2;}}_LL35C: {
# 2467
struct _tuple1 _stmttmp82=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp597);struct Cyc_List_List*_tmp5EE;struct Cyc_List_List*_tmp5EF;struct _tuple1 _tmp5ED=_stmttmp82;_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp567,env,inflow,_tmp597,_tmp5EE != 0,_tmp599,_tmp598->loc);{
struct Cyc_List_List*_tmp5F0=env->unique_pat_vars;
{struct _tuple27*_tmp8CD;struct Cyc_List_List*_tmp8CC;env->unique_pat_vars=(
(_tmp8CC=_region_malloc(env->r,sizeof(*_tmp8CC)),((_tmp8CC->hd=((_tmp8CD=_region_malloc(env->r,sizeof(*_tmp8CD)),((_tmp8CD->f1=_tmp59A,((_tmp8CD->f2=_tmp5EE,((_tmp8CD->f3=_tmp5EF,_tmp8CD)))))))),((_tmp8CC->tl=_tmp5F0,_tmp8CC))))));}
# 2475
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp59A,rval_opt);
env->unique_pat_vars=_tmp5F0;
# 2479
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5EE,_tmp5EF,inflow,_tmp599);
# 2483
return inflow;};}_LL35D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp59B->tag != 12)goto _LL35F;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp59C=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp59B->f1)->r;if(_tmp59C->tag != 5)goto _LL35F;else{_tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;}}_tmp59F=_tmp59B->f2;}}_LL35E: {
# 2487
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp59E->initializer);
struct _tuple17 _stmttmp7D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp5F4;union Cyc_CfFlowInfo_AbsLVal _tmp5F5;struct _tuple17 _tmp5F3=_stmttmp7D;_tmp5F4=_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;{
# 2492
struct _tuple17 _tmp8CE;struct _tuple17 _stmttmp7E=(_tmp8CE.f1=_tmp5F4,((_tmp8CE.f2=_tmp5F5,_tmp8CE)));struct _tuple17 _tmp5F6=_stmttmp7E;struct Cyc_Dict_Dict _tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_CfFlowInfo_Place*_tmp5F9;_LL380: if(((_tmp5F6.f1).ReachableFL).tag != 2)goto _LL382;_tmp5F7=((struct _tuple14)((_tmp5F6.f1).ReachableFL).val).f1;_tmp5F8=((struct _tuple14)((_tmp5F6.f1).ReachableFL).val).f2;if(((_tmp5F6.f2).PlaceL).tag != 1)goto _LL382;_tmp5F9=(struct Cyc_CfFlowInfo_Place*)((_tmp5F6.f2).PlaceL).val;_LL381: {
# 2494
void*_tmp5FA=Cyc_CfFlowInfo_lookup_place(_tmp5F7,_tmp5F9);
void*t=(void*)_check_null(e->topt);
void*_tmp5FB=Cyc_CfFlowInfo_make_unique_consumed(_tmp567,t,e,env->iteration_num,_tmp5FA);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8D1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8D0;_tmp5FB=(void*)((_tmp8D0=_region_malloc(_tmp567->r,sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8D1.tag=8,((_tmp8D1.f1=_tmp59E,((_tmp8D1.f2=_tmp5FB,_tmp8D1)))))),_tmp8D0))));}
_tmp5F7=Cyc_CfFlowInfo_assign_place(_tmp567,e->loc,_tmp5F7,env->all_changed,_tmp5F9,_tmp5FB);
_tmp5F4=Cyc_CfFlowInfo_ReachableFL(_tmp5F7,_tmp5F8);{
# 2507
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp7F=Cyc_CfFlowInfo_initlevel(_tmp567,_tmp5F7,_tmp5FA);switch(_stmttmp7F){case Cyc_CfFlowInfo_AllIL: _LL386:
 leaf=_tmp567->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL387:
 leaf=_tmp567->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL388:
 leaf=_tmp567->unknown_this;break;}}{
# 2513
void*_tmp5FE=Cyc_CfFlowInfo_typ_to_absrval(_tmp567,t,0,leaf);
_tmp5F4=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5F4,_tmp5FA);{
struct Cyc_List_List _tmp8D2;struct Cyc_List_List _tmp5FF=(_tmp8D2.hd=_tmp59E,((_tmp8D2.tl=0,_tmp8D2)));
# 2517
_tmp5F4=Cyc_NewControlFlow_add_vars(_tmp567,_tmp5F4,& _tmp5FF,_tmp567->unknown_all,s->loc,0);
# 2521
_tmp5F4=Cyc_NewControlFlow_anal_stmt(env,_tmp5F4,_tmp59F,rval_opt);{
# 2523
union Cyc_CfFlowInfo_FlowInfo _tmp600=_tmp5F4;struct Cyc_Dict_Dict _tmp601;struct Cyc_List_List*_tmp602;_LL38B: if((_tmp600.ReachableFL).tag != 2)goto _LL38D;_tmp601=((struct _tuple14)(_tmp600.ReachableFL).val).f1;_tmp602=((struct _tuple14)(_tmp600.ReachableFL).val).f2;_LL38C:
# 2526
{void*_stmttmp80=Cyc_CfFlowInfo_lookup_place(_tmp601,_tmp5F9);void*_tmp603=_stmttmp80;struct Cyc_Absyn_Vardecl*_tmp605;void*_tmp606;_LL390: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp604=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp603;if(_tmp604->tag != 8)goto _LL392;else{_tmp605=_tmp604->f1;_tmp606=(void*)_tmp604->f2;}}_LL391:
# 2528
 if(_tmp605 != _tmp59E)goto _LL393;{
# 2531
struct _tuple15 _tmp8D4;struct _tuple15 _tmp8D3;struct _tuple15 _stmttmp81=Cyc_CfFlowInfo_join_flow_and_rval(_tmp567,env->all_changed,(
(_tmp8D3.f1=_tmp5F4,((_tmp8D3.f2=_tmp5FA,_tmp8D3)))),((_tmp8D4.f1=_tmp5F4,((_tmp8D4.f2=_tmp606,_tmp8D4)))));
# 2531
void*_tmp608;struct _tuple15 _tmp607=_stmttmp81;_tmp608=_tmp607.f2;
# 2533
_tmp5FE=_tmp608;
goto _LL38F;};_LL392:;_LL393:
# 2536
{const char*_tmp8D7;void*_tmp8D6;(_tmp8D6=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8D7="aliased location was overwritten",_tag_dyneither(_tmp8D7,sizeof(char),33))),_tag_dyneither(_tmp8D6,sizeof(void*),0)));}
goto _LL38F;_LL38F:;}
# 2539
_tmp601=Cyc_CfFlowInfo_assign_place(_tmp567,s->loc,_tmp601,env->all_changed,_tmp5F9,_tmp5FE);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp601,_tmp602);
# 2544
return outflow;_LL38D:;_LL38E:
 return _tmp5F4;_LL38A:;};};};};}_LL382: if(((_tmp5F6.f1).BottomFL).tag != 1)goto _LL384;_LL383:
# 2547
 return _tmp5F4;_LL384:;_LL385:
{const char*_tmp8DA;void*_tmp8D9;(_tmp8D9=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8DA="bad alias expression--no unique path found",_tag_dyneither(_tmp8DA,sizeof(char),43))),_tag_dyneither(_tmp8D9,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL37F:;};}_LL35F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5A0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp5A0->tag != 12)goto _LL361;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A0->f2;}}_LL360:
# 2553
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5A1),_tmp5A2,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5A1);
return outflow;_LL361: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5A3=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp5A3->tag != 13)goto _LL363;else{_tmp5A4=_tmp5A3->f2;}}_LL362:
# 2558
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A4,rval_opt);_LL363: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5A5=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp568;if(_tmp5A5->tag != 16)goto _LL365;else{_tmp5A6=_tmp5A5->f1;}}_LL364: {
# 2561
struct _tuple15 _stmttmp7B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A6);union Cyc_CfFlowInfo_FlowInfo _tmp612;void*_tmp613;struct _tuple15 _tmp611=_stmttmp7B;_tmp612=_tmp611.f1;_tmp613=_tmp611.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp614=Cyc_NewControlFlow_use_Rval(env,_tmp5A6->loc,_tmp612,_tmp613);
void*_stmttmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp5A6->topt));void*_tmp615=_stmttmp7C;void*_tmp617;_LL395: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp615;if(_tmp616->tag != 15)goto _LL397;else{_tmp617=(void*)_tmp616->f1;}}_LL396:
# 2565
 return Cyc_CfFlowInfo_kill_flow_region(_tmp567,_tmp612,_tmp617);_LL397:;_LL398: {
struct Cyc_Core_Impossible_exn_struct _tmp8E0;const char*_tmp8DF;struct Cyc_Core_Impossible_exn_struct*_tmp8DE;(int)_throw((void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8E0.tag=Cyc_Core_Impossible,((_tmp8E0.f1=((_tmp8DF="anal_stmt -- reset_region",_tag_dyneither(_tmp8DF,sizeof(char),26))),_tmp8E0)))),_tmp8DE)))));}_LL394:;};}_LL365:;_LL366: {
# 2569
struct Cyc_Core_Impossible_exn_struct _tmp8E6;const char*_tmp8E5;struct Cyc_Core_Impossible_exn_struct*_tmp8E4;(int)_throw((void*)((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E6.tag=Cyc_Core_Impossible,((_tmp8E6.f1=((_tmp8E5="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8E5,sizeof(char),56))),_tmp8E6)))),_tmp8E4)))));}_LL33A:;};}
# 2574
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2577
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp61E=env->fenv;
void*_stmttmp90=decl->r;void*_tmp61F=_stmttmp90;struct Cyc_Absyn_Vardecl*_tmp621;struct Cyc_List_List*_tmp623;struct Cyc_Absyn_Fndecl*_tmp625;struct Cyc_Absyn_Tvar*_tmp627;struct Cyc_Absyn_Vardecl*_tmp628;int _tmp629;struct Cyc_Absyn_Exp*_tmp62A;_LL39A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp620=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp61F;if(_tmp620->tag != 0)goto _LL39C;else{_tmp621=_tmp620->f1;}}_LL39B: {
# 2587
struct Cyc_List_List _tmp8E7;struct Cyc_List_List _tmp62B=(_tmp8E7.hd=_tmp621,((_tmp8E7.tl=0,_tmp8E7)));
inflow=Cyc_NewControlFlow_add_vars(_tmp61E,inflow,& _tmp62B,_tmp61E->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp62C=_tmp621->initializer;
if(_tmp62C == 0)
return inflow;{
struct _tuple15 _stmttmp92=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp62C);union Cyc_CfFlowInfo_FlowInfo _tmp62E;void*_tmp62F;struct _tuple15 _tmp62D=_stmttmp92;_tmp62E=_tmp62D.f1;_tmp62F=_tmp62D.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp61E,env,_tmp62E,_tmp621,_tmp62C,_tmp62F,decl->loc);};};}_LL39C: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp622=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp61F;if(_tmp622->tag != 3)goto _LL39E;else{_tmp623=_tmp622->f1;}}_LL39D:
# 2596
 return Cyc_NewControlFlow_add_vars(_tmp61E,inflow,_tmp623,_tmp61E->unknown_none,decl->loc,0);_LL39E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp624=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp61F;if(_tmp624->tag != 1)goto _LL3A0;else{_tmp625=_tmp624->f1;}}_LL39F:
# 2599
 Cyc_NewControlFlow_check_nested_fun(_tmp61E,inflow,_tmp625);{
void*t=(void*)_check_null(_tmp625->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp631=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp625->fn_vardecl);
# 2605
struct Cyc_List_List*_tmp8E8;return Cyc_NewControlFlow_add_vars(_tmp61E,inflow,((_tmp8E8=_region_malloc(env->r,sizeof(*_tmp8E8)),((_tmp8E8->hd=_tmp631,((_tmp8E8->tl=0,_tmp8E8)))))),_tmp61E->unknown_all,decl->loc,0);};_LL3A0: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp626=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61F;if(_tmp626->tag != 4)goto _LL3A2;else{_tmp627=_tmp626->f1;_tmp628=_tmp626->f2;_tmp629=_tmp626->f3;_tmp62A=_tmp626->f4;}}_LL3A1:
# 2608
 if(_tmp62A != 0){
struct Cyc_Absyn_Exp*_tmp633=_tmp62A;
struct _tuple15 _stmttmp91=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp633);union Cyc_CfFlowInfo_FlowInfo _tmp635;void*_tmp636;struct _tuple15 _tmp634=_stmttmp91;_tmp635=_tmp634.f1;_tmp636=_tmp634.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp633->loc,_tmp635,_tmp636);}{
# 2613
struct Cyc_List_List _tmp8E9;struct Cyc_List_List _tmp637=(_tmp8E9.hd=_tmp628,((_tmp8E9.tl=0,_tmp8E9)));
return Cyc_NewControlFlow_add_vars(_tmp61E,inflow,& _tmp637,_tmp61E->unknown_all,decl->loc,0);};_LL3A2:;_LL3A3: {
# 2617
struct Cyc_Core_Impossible_exn_struct _tmp8EF;const char*_tmp8EE;struct Cyc_Core_Impossible_exn_struct*_tmp8ED;(int)_throw((void*)((_tmp8ED=_cycalloc(sizeof(*_tmp8ED)),((_tmp8ED[0]=((_tmp8EF.tag=Cyc_Core_Impossible,((_tmp8EF.f1=((_tmp8EE="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8EE,sizeof(char),35))),_tmp8EF)))),_tmp8ED)))));}_LL399:;}
# 2625
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp63C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp63C;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2627
;_pop_region(frgn);}
# 2632
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2636
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2640
struct _RegionHandle*_tmp63D=fenv->r;
unsigned int _tmp63E=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp63E,1);{
# 2646
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp63F=inflow;struct Cyc_Dict_Dict _tmp640;struct Cyc_List_List*_tmp641;_LL3A5: if((_tmp63F.BottomFL).tag != 1)goto _LL3A7;_LL3A6: {
const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8F2="check_fun",_tag_dyneither(_tmp8F2,sizeof(char),10))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}_LL3A7: if((_tmp63F.ReachableFL).tag != 2)goto _LL3A4;_tmp640=((struct _tuple14)(_tmp63F.ReachableFL).val).f1;_tmp641=((struct _tuple14)(_tmp63F.ReachableFL).val).f2;_LL3A8: {
# 2653
struct Cyc_List_List*atts;
{void*_stmttmp93=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp644=_stmttmp93;struct Cyc_List_List*_tmp646;_LL3AA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp645=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp644;if(_tmp645->tag != 9)goto _LL3AC;else{_tmp646=(_tmp645->f1).attributes;}}_LL3AB:
 atts=_tmp646;goto _LL3A9;_LL3AC:;_LL3AD: {
const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8F5="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8F5,sizeof(char),50))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}_LL3A9:;}
# 2660
for(0;atts != 0;atts=atts->tl){
void*_stmttmp94=(void*)atts->hd;void*_tmp649=_stmttmp94;int _tmp64B;int _tmp64D;int _tmp64F;_LL3AF: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp64A=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp649;if(_tmp64A->tag != 21)goto _LL3B1;else{_tmp64B=_tmp64A->f1;}}_LL3B0: {
# 2663
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp650=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp64B - 1);
void*t=Cyc_Tcutil_compress(_tmp650->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp651=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8F8;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8F7;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp652=(_tmp8F7=_region_malloc(_tmp63D,sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=2,((_tmp8F8.f1=_tmp64B,((_tmp8F8.f2=t,_tmp8F8)))))),_tmp8F7)));
struct Cyc_CfFlowInfo_Place*_tmp8F9;struct Cyc_CfFlowInfo_Place*_tmp653=(_tmp8F9=_region_malloc(_tmp63D,sizeof(*_tmp8F9)),((_tmp8F9->root=(void*)_tmp652,((_tmp8F9->fields=0,_tmp8F9)))));
_tmp640=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp640,(void*)_tmp652,_tmp651);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8FF;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8FE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8FC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8FB;_tmp640=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp640,(void*)((_tmp8FB=_region_malloc(_tmp63D,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=0,((_tmp8FC.f1=_tmp650,_tmp8FC)))),_tmp8FB)))),(void*)((_tmp8FE=_region_malloc(_tmp63D,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=5,((_tmp8FF.f1=_tmp653,_tmp8FF)))),_tmp8FE)))));}
goto _LL3AE;}_LL3B1: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp64C=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp649;if(_tmp64C->tag != 20)goto _LL3B3;else{_tmp64D=_tmp64C->f1;}}_LL3B2: {
# 2676
struct Cyc_Absyn_Vardecl*_tmp65B=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp64D - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp65B->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp902;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp901;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65C=(_tmp901=_region_malloc(_tmp63D,sizeof(*_tmp901)),((_tmp901[0]=((_tmp902.tag=2,((_tmp902.f1=_tmp64D,((_tmp902.f2=elttype,_tmp902)))))),_tmp901)));
struct Cyc_CfFlowInfo_Place*_tmp903;struct Cyc_CfFlowInfo_Place*_tmp65D=(_tmp903=_region_malloc(_tmp63D,sizeof(*_tmp903)),((_tmp903->root=(void*)_tmp65C,((_tmp903->fields=0,_tmp903)))));
_tmp640=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp640,(void*)_tmp65C,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp909;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp908;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp906;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp905;_tmp640=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp640,(void*)((_tmp905=_region_malloc(_tmp63D,sizeof(*_tmp905)),((_tmp905[0]=((_tmp906.tag=0,((_tmp906.f1=_tmp65B,_tmp906)))),_tmp905)))),(void*)((_tmp908=_region_malloc(_tmp63D,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=5,((_tmp909.f1=_tmp65D,_tmp909)))),_tmp908)))));}
{struct Cyc_List_List*_tmp90A;param_roots=((_tmp90A=_region_malloc(_tmp63D,sizeof(*_tmp90A)),((_tmp90A->hd=_tmp65D,((_tmp90A->tl=param_roots,_tmp90A))))));}
goto _LL3AE;}_LL3B3: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp64E=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp649;if(_tmp64E->tag != 22)goto _LL3B5;else{_tmp64F=_tmp64E->f1;}}_LL3B4: {
# 2685
struct Cyc_Absyn_Vardecl*_tmp666=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp64F - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp666->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp90D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp90C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp667=(_tmp90C=_region_malloc(_tmp63D,sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp90D.tag=0,((_tmp90D.f1=_tmp666,_tmp90D)))),_tmp90C)));
struct Cyc_CfFlowInfo_Place*_tmp90E;struct Cyc_CfFlowInfo_Place*_tmp668=(_tmp90E=_region_malloc(_tmp63D,sizeof(*_tmp90E)),((_tmp90E->root=(void*)_tmp667,((_tmp90E->fields=0,_tmp90E)))));
struct Cyc_List_List*_tmp90F;noconsume_roots=((_tmp90F=_region_malloc(_tmp63D,sizeof(*_tmp90F)),((_tmp90F->hd=_tmp668,((_tmp90F->tl=noconsume_roots,_tmp90F))))));}
# 2691
goto _LL3AE;}_LL3B5:;_LL3B6:
 goto _LL3AE;_LL3AE:;}
# 2694
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp640,_tmp641);}_LL3A4:;}{
# 2697
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp63D,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp910;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp910=_region_malloc(_tmp63D,sizeof(*_tmp910)),((_tmp910->r=_tmp63D,((_tmp910->fenv=fenv,((_tmp910->iterate_again=1,((_tmp910->iteration_num=0,((_tmp910->in_try=0,((_tmp910->tryflow=inflow,((_tmp910->all_changed=0,((_tmp910->noreturn=noreturn,((_tmp910->return_type=fd->ret_type,((_tmp910->unique_pat_vars=0,((_tmp910->param_roots=param_roots,((_tmp910->noconsume_params=noconsume_roots,((_tmp910->flow_table=flow_table,_tmp910)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2707
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2710
union Cyc_CfFlowInfo_FlowInfo _tmp66D=outflow;_LL3B8: if((_tmp66D.BottomFL).tag != 1)goto _LL3BA;_LL3B9:
 goto _LL3B7;_LL3BA:;_LL3BB:
# 2713
 Cyc_NewControlFlow_check_init_params(_tmp63E,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp63E);
# 2717
if(noreturn){
const char*_tmp913;void*_tmp912;(_tmp912=0,Cyc_CfFlowInfo_aerr(_tmp63E,((_tmp913="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp913,sizeof(char),46))),_tag_dyneither(_tmp912,sizeof(void*),0)));}else{
# 2720
void*_stmttmp95=Cyc_Tcutil_compress(fd->ret_type);void*_tmp670=_stmttmp95;_LL3BD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp671=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp670;if(_tmp671->tag != 0)goto _LL3BF;}_LL3BE:
 goto _LL3BC;_LL3BF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp672=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp670;if(_tmp672->tag != 7)goto _LL3C1;}_LL3C0:
 goto _LL3C2;_LL3C1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp673=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp670;if(_tmp673->tag != 6)goto _LL3C3;}_LL3C2:
# 2724
{const char*_tmp916;void*_tmp915;(_tmp915=0,Cyc_Tcutil_warn(_tmp63E,((_tmp916="function may not return a value",_tag_dyneither(_tmp916,sizeof(char),32))),_tag_dyneither(_tmp915,sizeof(void*),0)));}goto _LL3BC;_LL3C3:;_LL3C4:
# 2726
{const char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_CfFlowInfo_aerr(_tmp63E,((_tmp919="function may not return a value",_tag_dyneither(_tmp919,sizeof(char),32))),_tag_dyneither(_tmp918,sizeof(void*),0)));}goto _LL3BC;_LL3BC:;}
# 2728
goto _LL3B7;_LL3B7:;};};};}
# 2732
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_stmttmp96=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp679=_stmttmp96;struct Cyc_Absyn_Fndecl*_tmp67B;struct Cyc_List_List*_tmp67D;struct Cyc_List_List*_tmp67F;struct Cyc_List_List*_tmp681;_LL3C6: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp67A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp679;if(_tmp67A->tag != 1)goto _LL3C8;else{_tmp67B=_tmp67A->f1;}}_LL3C7:
 Cyc_NewControlFlow_check_fun(_tmp67B);goto _LL3C5;_LL3C8: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp67C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp679;if(_tmp67C->tag != 12)goto _LL3CA;else{_tmp67D=_tmp67C->f1;}}_LL3C9:
 _tmp67F=_tmp67D;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp67E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp679;if(_tmp67E->tag != 11)goto _LL3CC;else{_tmp67F=_tmp67E->f2;}}_LL3CB:
 _tmp681=_tmp67F;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp680=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp679;if(_tmp680->tag != 10)goto _LL3CE;else{_tmp681=_tmp680->f2;}}_LL3CD:
 Cyc_NewControlFlow_cf_check(_tmp681);goto _LL3C5;_LL3CE: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp682=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp679;if(_tmp682->tag != 13)goto _LL3D0;}_LL3CF:
 goto _LL3C5;_LL3D0:;_LL3D1:
 goto _LL3C5;_LL3C5:;};}}
