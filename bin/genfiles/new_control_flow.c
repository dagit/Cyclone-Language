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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp68D;struct Cyc_NewControlFlow_CFStmtAnnot _tmp68C;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp68B;enclosee->annot=(void*)((_tmp68B=_cycalloc(sizeof(*_tmp68B)),((_tmp68B[0]=((_tmp68D.tag=Cyc_NewControlFlow_CFAnnot,((_tmp68D.f1=((_tmp68C.encloser=encloser,((_tmp68C.visited=0,_tmp68C)))),_tmp68D)))),_tmp68B))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp693;const char*_tmp692;struct Cyc_Core_Impossible_exn_struct*_tmp691;(int)_throw((void*)((_tmp691=_cycalloc(sizeof(*_tmp691)),((_tmp691[0]=((_tmp693.tag=Cyc_Core_Impossible,((_tmp693.f1=((_tmp692="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp692,sizeof(char),37))),_tmp693)))),_tmp691)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp694;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp694=_region_malloc(env->r,sizeof(*_tmp694)),((_tmp694[0]=Cyc_CfFlowInfo_BottomFL(),_tmp694)));
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
struct _tuple19 _tmp695;return(_tmp695.f1=_tmpA,((_tmp695.f2=_tmpB,_tmp695)));};}
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
{const char*_tmp699;void*_tmp698[1];struct Cyc_String_pa_PrintArg_struct _tmp697;(_tmp697.tag=0,((_tmp697.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp698[0]=& _tmp697,Cyc_Tcutil_warn(_tmpF,((_tmp699="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp699,sizeof(char),60))),_tag_dyneither(_tmp698,sizeof(void*),1)))))));}
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
struct _tuple18 _tmp69A;struct _tuple18 _stmttmp1=(_tmp69A.f1=flow,((_tmp69A.f2=_tmp1F,_tmp69A)));struct _tuple18 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp69B;struct _tuple20 _tmp23=(_tmp69B.f1=env,((_tmp69B.f2=s->loc,((_tmp69B.f3=_tmp22,_tmp69B)))));
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp69E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp69D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp69D=_region_malloc(fenv->r,sizeof(*_tmp69D)),((_tmp69D[0]=((_tmp69E.tag=0,((_tmp69E.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp69E)))),_tmp69D)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6A0;_tmp2A=(void*)((_tmp6A0=_region_malloc(fenv->r,sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=8,((_tmp6A1.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp6A1.f2=_tmp2A,_tmp6A1)))))),_tmp6A0))));}
# 259
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp6A2;if(Cyc_Relations_consistent_relations(((_tmp6A2=_region_malloc(r,sizeof(*_tmp6A2)),((_tmp6A2->hd=_tmp2F,((_tmp6A2->tl=assume,_tmp6A2))))))))
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
{const char*_tmp6A7;void*_tmp6A6[2];struct Cyc_String_pa_PrintArg_struct _tmp6A5;struct Cyc_String_pa_PrintArg_struct _tmp6A4;(_tmp6A4.tag=0,((_tmp6A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A5.tag=0,((_tmp6A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6A6[0]=& _tmp6A5,((_tmp6A6[1]=& _tmp6A4,Cyc_CfFlowInfo_aerr(loc,((_tmp6A7="unable to prove %s, required to access %s",_tag_dyneither(_tmp6A7,sizeof(char),42))),_tag_dyneither(_tmp6A6,sizeof(void*),2)))))))))))));}
{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="  [recorded facts on path: ",_tag_dyneither(_tmp6AA,sizeof(char),28))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6AD;void*_tmp6AC;(_tmp6AC=0,Cyc_fprintf(Cyc_stderr,((_tmp6AD="]\n",_tag_dyneither(_tmp6AD,sizeof(char),3))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));};}}
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
{const char*_tmp6B2;void*_tmp6B1[2];struct Cyc_String_pa_PrintArg_struct _tmp6B0;struct Cyc_String_pa_PrintArg_struct _tmp6AF;(_tmp6AF.tag=0,((_tmp6AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6B0.tag=0,((_tmp6B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6B1[0]=& _tmp6B0,((_tmp6B1[1]=& _tmp6AF,Cyc_CfFlowInfo_aerr(loc,((_tmp6B2="unable to prove %s, required to access %s",_tag_dyneither(_tmp6B2,sizeof(char),42))),_tag_dyneither(_tmp6B1,sizeof(void*),2)))))))))))));}
{const char*_tmp6B5;void*_tmp6B4;(_tmp6B4=0,Cyc_fprintf(Cyc_stderr,((_tmp6B5="  [recorded facts on path: ",_tag_dyneither(_tmp6B5,sizeof(char),28))),_tag_dyneither(_tmp6B4,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B8;void*_tmp6B7;(_tmp6B7=0,Cyc_fprintf(Cyc_stderr,((_tmp6B8="]\n",_tag_dyneither(_tmp6B8,sizeof(char),3))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));};}
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
const char*_tmp6BB;void*_tmp6BA;(_tmp6BA=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6BB="expression may not be fully initialized",_tag_dyneither(_tmp6BB,sizeof(char),40))),_tag_dyneither(_tmp6BA,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6BC;struct _tuple0 _stmttmp4=(_tmp6BC.f1=Cyc_Tcutil_compress(t),((_tmp6BC.f2=r,_tmp6BC)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
{const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_Tcutil_warn(loc,((_tmp6BF="argument may still contain unique pointers",_tag_dyneither(_tmp6BF,sizeof(char),43))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6C2;void*_tmp6C1;(_tmp6C1=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6C2="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6C2,sizeof(char),47))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}
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
const char*_tmp6C5;void*_tmp6C4;(_tmp6C4=0,Cyc_Tcutil_warn(loc,((_tmp6C5="argument may contain live unique pointers",_tag_dyneither(_tmp6C5,sizeof(char),42))),_tag_dyneither(_tmp6C4,sizeof(void*),0)));}
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
struct _tuple22 _tmp6C6;return(_tmp6C6.f1=inflow,((_tmp6C6.f2=0,_tmp6C6)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6C9;struct _tuple22 _tmp6C8;return(_tmp6C8.f1=_tmp8C,((_tmp6C8.f2=((_tmp6C9=_region_malloc(rgn,sizeof(*_tmp6C9)),((_tmp6C9->hd=_tmp8D,((_tmp6C9->tl=0,_tmp6C9)))))),_tmp6C8)));};}{
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
{struct Cyc_Dict_Dict*_tmp6CA;env->all_changed=((_tmp6CA=_region_malloc(env->r,sizeof(*_tmp6CA)),((_tmp6CA[0]=(env->fenv)->mt_place_set,_tmp6CA))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6CB;rvals=((_tmp6CB=_region_malloc(rgn,sizeof(*_tmp6CB)),((_tmp6CB->hd=_tmp93,((_tmp6CB->tl=0,_tmp6CB))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6CC;env->all_changed=((_tmp6CC=_region_malloc(env->r,sizeof(*_tmp6CC)),((_tmp6CC[0]=(env->fenv)->mt_place_set,_tmp6CC))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6CD;rvals=((_tmp6CD=_region_malloc(rgn,sizeof(*_tmp6CD)),((_tmp6CD->hd=_tmp98,((_tmp6CD->tl=rvals,_tmp6CD))))));}
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
struct Cyc_Dict_Dict*_tmp6CE;env->all_changed=((_tmp6CE=_region_malloc(env->r,sizeof(*_tmp6CE)),((_tmp6CE[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6CE))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp6CF;return(_tmp6CF.f1=outflow,((_tmp6CF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6CF)));};};}
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
const char*_tmp6D2;void*_tmp6D1;(_tmp6D1=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6D2="expression may not be initialized",_tag_dyneither(_tmp6D2,sizeof(char),34))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp6D3;struct _tuple15 _tmpA5=(_tmp6D3.f1=_tmp9E,((_tmp6D3.f2=(env->fenv)->unknown_all,_tmp6D3)));_npop_handler(0);return _tmpA5;};}
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
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D6;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D5;nzval=(void*)((_tmp6D5=_region_malloc((env->fenv)->r,sizeof(*_tmp6D5)),((_tmp6D5[0]=((_tmp6D6.tag=8,((_tmp6D6.f1=locname,((_tmp6D6.f2=nzval,_tmp6D6)))))),_tmp6D5))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 521
return _tmpAD;};}_LL60:;}_LL5B:;}
# 530
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 533
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp6D7;return(_tmp6D7.f1=outflow,((_tmp6D7.f2=outflow,_tmp6D7)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 536
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp6D8;return(_tmp6D8.f1=outflow,((_tmp6D8.f2=outflow,_tmp6D8)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
if(locname != 0){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DB;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6DA;nzval=(void*)((_tmp6DA=_region_malloc((env->fenv)->r,sizeof(*_tmp6DA)),((_tmp6DA[0]=((_tmp6DB.tag=8,((_tmp6DB.f1=locname,((_tmp6DB.f2=nzval,_tmp6DB)))))),_tmp6DA))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6DD;zval=(void*)((_tmp6DD=_region_malloc((env->fenv)->r,sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6DE.tag=8,((_tmp6DE.f1=locname,((_tmp6DE.f2=zval,_tmp6DE)))))),_tmp6DD))));};}{
# 545
struct _tuple18 _tmp6DF;return
(_tmp6DF.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6DF.f2=
# 548
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,zval),_tmpB0),_tmp6DF)));};}_LL6A:;}_LL65:;}
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
struct _tuple15 _tmp6E0;return(_tmp6E0.f1=f,((_tmp6E0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6E0)));}_LL77: if((_tmpC1.ReachableFL).tag != 2)goto _LL74;_tmpC2=((struct _tuple14)(_tmpC1.ReachableFL).val).f1;_tmpC3=((struct _tuple14)(_tmpC1.ReachableFL).val).f2;_LL78:
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E3;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E2;e->annot=(void*)((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2[0]=((_tmp6E3.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E3)))),_tmp6E2))));}
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E6;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E5;e->annot=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E6)))),_tmp6E5))));}
goto _LL97;_LL9A:;_LL9B:
# 609
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 612
goto _LL92;_LL92:;}{
# 614
struct _tuple15 _tmp6E7;return(_tmp6E7.f1=f,((_tmp6E7.f2=Cyc_CfFlowInfo_lookup_place(_tmpC2,_tmpCC),_tmp6E7)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCD->tag != 0)goto _LL84;}_LL83:
# 616
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6E8;return(_tmp6E8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6E8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6E8)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCE->tag != 3)goto _LL86;else{_tmpCF=_tmpCE->f1;}}_LL85:
# 619
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpCF,locname);goto _LL87;_LL86:;_LL87:
# 621
{void*_stmttmpC=e->r;void*_tmpE0=_stmttmpC;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 22)goto _LL9F;}_LL9E:
# 623
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EB;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EA;e->annot=(void*)((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EB.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6EB)))),_tmp6EA))));}
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
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6ED;e->annot=(void*)((_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED[0]=((_tmp6EE.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6EE)))),_tmp6ED))));}
goto _LLA1;_LLA1:;}{
# 641
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC2,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 643
{void*_tmpE9=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE9;if(_tmpEA->tag != 7)goto _LLAB;}_LLAA:
# 645
{const char*_tmp6F1;void*_tmp6F0;(_tmp6F0=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6F1="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6F1,sizeof(char),61))),_tag_dyneither(_tmp6F0,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 648
const char*_tmp6F4;void*_tmp6F3;(_tmp6F3=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6F4="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6F4,sizeof(char),46))),_tag_dyneither(_tmp6F3,sizeof(void*),0)));}_LLA8:;}
# 650
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 652
struct _tuple15 _tmp6F5;return(_tmp6F5.f1=f,((_tmp6F5.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6F5)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 654
struct _tuple15 _tmp6F6;return(_tmp6F6.f1=f,((_tmp6F6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_none),_tmp6F6)));}}};_LL74:;}_LL72:;_LL73: {
# 657
struct Cyc_Core_Impossible_exn_struct _tmp6FC;const char*_tmp6FB;struct Cyc_Core_Impossible_exn_struct*_tmp6FA;(int)_throw((void*)((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=((_tmp6FC.tag=Cyc_Core_Impossible,((_tmp6FC.f1=((_tmp6FB="right deref of non-pointer-type",_tag_dyneither(_tmp6FB,sizeof(char),32))),_tmp6FC)))),_tmp6FA)))));}_LL6F:;}
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
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 701
struct _tuple17 _stmttmp14=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF9;struct _tuple17 _tmpF8=_stmttmp14;_tmpF9=_tmpF8.f2;
{struct _tuple17 _tmp6FD;struct _tuple17 _stmttmp15=(_tmp6FD.f1=inflow,((_tmp6FD.f2=_tmpF9,_tmp6FD)));struct _tuple17 _tmpFA=_stmttmp15;struct Cyc_Dict_Dict _tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_CfFlowInfo_Place*_tmpFD;_LLB5: if(((_tmpFA.f1).ReachableFL).tag != 2)goto _LLB7;_tmpFB=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f1;_tmpFC=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f2;if(((_tmpFA.f2).PlaceL).tag != 1)goto _LLB7;_tmpFD=(struct Cyc_CfFlowInfo_Place*)((_tmpFA.f2).PlaceL).val;_LLB6: {
# 704
void*_tmpFE=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 706
{void*_tmpFF=old_rval;struct Cyc_Absyn_Vardecl*_tmp101;_LLBC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp100=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFF;if(_tmp100->tag != 8)goto _LLBE;else{_tmp101=_tmp100->f1;}}_LLBD:
# 708
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp700;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6FF;_tmpFE=(void*)((_tmp6FF=_region_malloc((env->fenv)->r,sizeof(*_tmp6FF)),((_tmp6FF[0]=((_tmp700.tag=8,((_tmp700.f1=_tmp101,((_tmp700.f2=_tmpFE,_tmp700)))))),_tmp6FF))));}
goto _LLBB;_LLBE:;_LLBF:
 goto _LLBB;_LLBB:;}
# 714
_tmpFB=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpFB,env->all_changed,_tmpFD,_tmpFE);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpFB,_tmpFC);
# 719
goto _LLB4;}_LLB7: if(((_tmpFA.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 722
{const char*_tmp703;void*_tmp702;(_tmp702=0,Cyc_CfFlowInfo_aerr(loc,((_tmp703="noconsume() parameters must be l-values",_tag_dyneither(_tmp703,sizeof(char),40))),_tag_dyneither(_tmp702,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 725
return inflow;}
# 730
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 738
union Cyc_CfFlowInfo_FlowInfo _tmp107=outflow;struct Cyc_Dict_Dict _tmp108;struct Cyc_List_List*_tmp109;_LLC1: if((_tmp107.BottomFL).tag != 1)goto _LLC3;_LLC2: {
# 741
struct _tuple15 _tmp704;return(_tmp704.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp704.f2=rval,_tmp704)));}_LLC3: if((_tmp107.ReachableFL).tag != 2)goto _LLC0;_tmp108=((struct _tuple14)(_tmp107.ReachableFL).val).f1;_tmp109=((struct _tuple14)(_tmp107.ReachableFL).val).f2;_LLC4: {
# 743
union Cyc_CfFlowInfo_AbsLVal _tmp10B=lval;struct Cyc_CfFlowInfo_Place*_tmp10C;_LLC6: if((_tmp10B.PlaceL).tag != 1)goto _LLC8;_tmp10C=(struct Cyc_CfFlowInfo_Place*)(_tmp10B.PlaceL).val;_LLC7: {
# 745
struct Cyc_Dict_Dict _tmp10D=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp108,env->all_changed,_tmp10C,rval);
_tmp109=Cyc_Relations_reln_assign_exp(fenv->r,_tmp109,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp10D,_tmp109);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 751
void*_stmttmp16=Cyc_CfFlowInfo_lookup_place(_tmp108,_tmp10C);void*_tmp10E=_stmttmp16;_LLCB: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp10F->tag != 3)goto _LLCD;else{if(_tmp10F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCD;}}_LLCC:
 goto _LLCE;_LLCD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp110->tag != 7)goto _LLCF;}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp111=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp111->tag != 0)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp112=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp112->tag != 8)goto _LLD3;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp113=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp112->f2);if(_tmp113->tag != 3)goto _LLD3;else{if(_tmp113->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLD3;}}}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp114=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp114->tag != 8)goto _LLD5;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp115=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp114->f2);if(_tmp115->tag != 7)goto _LLD5;}}_LLD4:
 goto _LLD6;_LLD5: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp116=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp116->tag != 8)goto _LLD7;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp117=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp116->f2);if(_tmp117->tag != 0)goto _LLD7;}}_LLD6:
 goto _LLCA;_LLD7:;_LLD8:
# 759
{const char*_tmp707;void*_tmp706;(_tmp706=0,Cyc_Tcutil_warn(lexp->loc,((_tmp707="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp707,sizeof(char),43))),_tag_dyneither(_tmp706,sizeof(void*),0)));}
goto _LLCA;_LLCA:;}
# 764
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 766
struct _tuple15 _tmp708;return(_tmp708.f1=outflow,((_tmp708.f2=rval,_tmp708)));};}_LLC8: if((_tmp10B.UnknownL).tag != 2)goto _LLC5;_LLC9: {
# 769
struct _tuple15 _tmp709;return(_tmp709.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp709.f2=rval,_tmp709)));}_LLC5:;}_LLC0:;}
# 776
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 782
union Cyc_CfFlowInfo_FlowInfo _tmp11C=f;struct Cyc_Dict_Dict _tmp11D;struct Cyc_List_List*_tmp11E;_LLDA: if((_tmp11C.BottomFL).tag != 1)goto _LLDC;_LLDB:
 return Cyc_CfFlowInfo_BottomFL();_LLDC: if((_tmp11C.ReachableFL).tag != 2)goto _LLD9;_tmp11D=((struct _tuple14)(_tmp11C.ReachableFL).val).f1;_tmp11E=((struct _tuple14)(_tmp11C.ReachableFL).val).f2;_LLDD:
# 785
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp70F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp70E;struct Cyc_CfFlowInfo_Place*_tmp70D;_tmp11D=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp11D,env->all_changed,(
(_tmp70D=_region_malloc(env->r,sizeof(*_tmp70D)),((_tmp70D->root=(void*)((_tmp70F=_region_malloc(env->r,sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp70E.tag=0,((_tmp70E.f1=vd,_tmp70E)))),_tmp70F)))),((_tmp70D->fields=0,_tmp70D)))))),rval);}
# 788
_tmp11E=Cyc_Relations_reln_assign_var(env->r,_tmp11E,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp122=Cyc_CfFlowInfo_ReachableFL(_tmp11D,_tmp11E);
Cyc_NewControlFlow_update_tryflow(env,_tmp122);
# 795
return _tmp122;};_LLD9:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 799
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 806
if(vds == 0)return inflow;{
# 809
struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp710;es=((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp710->tl=es,_tmp710))))));}}}
# 815
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp123,fenv->unknown_all,pat_loc,name_locs);
# 817
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 821
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp126;void*_tmp127;struct _tuple15 _tmp125=_stmttmp17;_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp126,_tmp127);}}{
# 829
struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp128 != 0;_tmp128=_tmp128->tl){
struct _tuple23*_stmttmp18=(struct _tuple23*)_tmp128->hd;struct Cyc_Absyn_Vardecl**_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct _tuple23*_tmp129=_stmttmp18;_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;
if(_tmp12A != 0  && _tmp12B != 0){
# 836
if(_tmp12B->topt == 0){
const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_PrintArg_struct _tmp712;(_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12B)),((_tmp713[0]=& _tmp712,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp714="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp714,sizeof(char),41))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}
# 841
Cyc_fflush(Cyc_stderr);{
# 843
struct Cyc_List_List _tmp715;struct Cyc_List_List l=(_tmp715.hd=_tmp12B,((_tmp715.tl=0,_tmp715)));
union Cyc_CfFlowInfo_FlowInfo _tmp12F=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple15 _stmttmp19=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp12F,_tmp12B);union Cyc_CfFlowInfo_FlowInfo _tmp131;void*_tmp132;struct _tuple15 _tmp130=_stmttmp19;_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp718;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp717;_tmp132=(void*)((_tmp717=_region_malloc(fenv->r,sizeof(*_tmp717)),((_tmp717[0]=((_tmp718.tag=8,((_tmp718.f1=*_tmp12A,((_tmp718.f2=_tmp132,_tmp718)))))),_tmp717))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp131,*_tmp12A,_tmp12B,_tmp132,pat_loc);};}}
# 852
return inflow;};};}
# 855
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 857
void*_stmttmp1A=e->r;void*_tmp136=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp144;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp137->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp138=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp137->f2);if(_tmp138->tag != 4)goto _LLE1;}}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp139->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp13A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp139->f2);if(_tmp13A->tag != 3)goto _LLE3;}}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp13B->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp13C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp13B->f2);if(_tmp13C->tag != 5)goto _LLE5;}}_LLE4:
 return 1;_LLE5: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp13D->tag != 19)goto _LLE7;else{_tmp13E=_tmp13D->f1;}}_LLE6:
 _tmp140=_tmp13E;goto _LLE8;_LLE7: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp13F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp13F->tag != 20)goto _LLE9;else{_tmp140=_tmp13F->f1;}}_LLE8:
 _tmp142=_tmp140;goto _LLEA;_LLE9: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp141->tag != 21)goto _LLEB;else{_tmp142=_tmp141->f1;}}_LLEA:
# 864
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp142);_LLEB: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp136;if(_tmp143->tag != 22)goto _LLED;else{_tmp144=_tmp143->f1;}}_LLEC: {
# 866
void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(_tmp144->topt));void*_tmp145=_stmttmp1B;_LLF0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp145;if(_tmp146->tag != 10)goto _LLF2;}_LLF1:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp144);_LLF2:;_LLF3:
 return 0;_LLEF:;}_LLED:;_LLEE:
# 871
 return 0;_LLDE:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7CB(unsigned int*_tmp7CA,unsigned int*_tmp7C9,void***_tmp7C7,struct Cyc_List_List**_tmp325){for(*_tmp7CA=0;*_tmp7CA < *_tmp7C9;(*_tmp7CA)++){
# 1454 "new_control_flow.cyc"
void*_tmp7C5;(*_tmp7C7)[*_tmp7CA]=(void*)((_tmp7C5=(void*)((struct Cyc_List_List*)_check_null(*_tmp325))->hd,((*_tmp325=(*_tmp325)->tl,_tmp7C5))));}}
# 876 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 880
struct Cyc_CfFlowInfo_FlowEnv*_tmp147=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 893
{union Cyc_CfFlowInfo_FlowInfo _tmp148=inflow;struct Cyc_Dict_Dict _tmp149;struct Cyc_List_List*_tmp14A;_LLF5: if((_tmp148.BottomFL).tag != 1)goto _LLF7;_LLF6: {
struct _tuple15 _tmp719;return(_tmp719.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp719.f2=_tmp147->unknown_all,_tmp719)));}_LLF7: if((_tmp148.ReachableFL).tag != 2)goto _LLF4;_tmp149=((struct _tuple14)(_tmp148.ReachableFL).val).f1;_tmp14A=((struct _tuple14)(_tmp148.ReachableFL).val).f2;_LLF8:
 d=_tmp149;relns=_tmp14A;_LLF4:;}
# 908 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 929 "new_control_flow.cyc"
struct _tuple17 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp14D;union Cyc_CfFlowInfo_AbsLVal _tmp14E;struct _tuple17 _tmp14C=_stmttmp1C;_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{
struct _tuple17 _tmp71A;struct _tuple17 _stmttmp1D=(_tmp71A.f1=_tmp14D,((_tmp71A.f2=_tmp14E,_tmp71A)));struct _tuple17 _tmp14F=_stmttmp1D;struct Cyc_Dict_Dict _tmp150;struct Cyc_List_List*_tmp151;struct Cyc_CfFlowInfo_Place*_tmp152;_LLFA: if(((_tmp14F.f1).ReachableFL).tag != 2)goto _LLFC;_tmp150=((struct _tuple14)((_tmp14F.f1).ReachableFL).val).f1;_tmp151=((struct _tuple14)((_tmp14F.f1).ReachableFL).val).f2;if(((_tmp14F.f2).PlaceL).tag != 1)goto _LLFC;_tmp152=(struct Cyc_CfFlowInfo_Place*)((_tmp14F.f2).PlaceL).val;_LLFB: {
# 932
void*_tmp153=Cyc_CfFlowInfo_lookup_place(_tmp150,_tmp152);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp153,& needs_unconsume)){
{const char*_tmp71D;void*_tmp71C;(_tmp71C=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp71D="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp71D,sizeof(char),71))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}{
struct _tuple15 _tmp71E;return(_tmp71E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71E.f2=_tmp147->unknown_all,_tmp71E)));};}else{
# 938
if(needs_unconsume){
# 940
struct _tuple15 _tmp71F;return(_tmp71F.f1=_tmp14D,((_tmp71F.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp147,_tmp153),_tmp71F)));}else{
# 943
void*_tmp158=Cyc_CfFlowInfo_make_unique_consumed(_tmp147,(void*)_check_null(e->topt),e,env->iteration_num,_tmp153);
struct Cyc_Dict_Dict _tmp159=Cyc_CfFlowInfo_assign_place(_tmp147,e->loc,_tmp150,env->all_changed,_tmp152,_tmp158);
# 954
struct _tuple15 _tmp720;return(_tmp720.f1=Cyc_CfFlowInfo_ReachableFL(_tmp159,_tmp151),((_tmp720.f2=_tmp153,_tmp720)));}}}_LLFC:;_LLFD:
# 956
 goto _LLF9;_LLF9:;};}{
# 959
void*_stmttmp1E=e->r;void*_tmp15C=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Vardecl*_tmp178;struct Cyc_Absyn_Vardecl*_tmp17B;enum Cyc_Absyn_Primop _tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_List_List*_tmp18E;int _tmp190;struct Cyc_Absyn_Exp*_tmp191;void**_tmp192;struct Cyc_Absyn_Exp*_tmp193;int _tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct _dyneither_ptr*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct _dyneither_ptr*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct _dyneither_ptr*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_List_List*_tmp1B6;struct Cyc_Absyn_Datatypedecl*_tmp1B7;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BD;enum Cyc_Absyn_AggrKind _tmp1BE;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Vardecl*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C6;int _tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;void*_tmp1CA;int _tmp1CB;struct Cyc_Absyn_Stmt*_tmp1CD;_LLFF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp15D->tag != 13)goto _LL101;else{_tmp15E=_tmp15D->f2;if(_tmp15D->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL101;}}_LL100: {
# 963
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15E);union Cyc_CfFlowInfo_FlowInfo _tmp1D5;void*_tmp1D6;struct _tuple15 _tmp1D4=_stmttmp4F;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;{
struct _tuple15 _stmttmp50=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D5,_tmp15E,_tmp1D6);union Cyc_CfFlowInfo_FlowInfo _tmp1D8;void*_tmp1D9;struct _tuple15 _tmp1D7=_stmttmp50;_tmp1D8=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;{
struct _tuple15 _tmp721;return(_tmp721.f1=_tmp1D8,((_tmp721.f2=_tmp1D6,_tmp721)));};};}_LL101: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp15F->tag != 13)goto _LL103;else{_tmp160=_tmp15F->f2;}}_LL102:
# 969
 _tmp162=_tmp160;goto _LL104;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp161->tag != 11)goto _LL105;else{_tmp162=_tmp161->f1;}}_LL104:
 _tmp164=_tmp162;goto _LL106;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp163->tag != 12)goto _LL107;else{_tmp164=_tmp163->f1;}}_LL106:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp164);_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp165->tag != 0)goto _LL109;else{if(((_tmp165->f1).Null_c).tag != 1)goto _LL109;}}_LL108:
# 973
 goto _LL10A;_LL109: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp166->tag != 0)goto _LL10B;else{if(((_tmp166->f1).Int_c).tag != 5)goto _LL10B;if(((struct _tuple7)((_tmp166->f1).Int_c).val).f2 != 0)goto _LL10B;}}_LL10A: {
struct _tuple15 _tmp722;return(_tmp722.f1=inflow,((_tmp722.f2=_tmp147->zero,_tmp722)));}_LL10B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp167->tag != 0)goto _LL10D;else{if(((_tmp167->f1).Int_c).tag != 5)goto _LL10D;}}_LL10C:
 goto _LL10E;_LL10D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp168->tag != 1)goto _LL10F;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp169=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp168->f2);if(_tmp169->tag != 2)goto _LL10F;}}_LL10E: {
struct _tuple15 _tmp723;return(_tmp723.f1=inflow,((_tmp723.f2=_tmp147->notzeroall,_tmp723)));}_LL10F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16A->tag != 30)goto _LL111;else{if(_tmp16A->f1 != 0)goto _LL111;}}_LL110:
# 978
 goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16B->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16C->tag != 17)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16D->tag != 16)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16E->tag != 18)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp16F->tag != 32)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp170->tag != 31)goto _LL11D;}_LL11C: {
struct _tuple15 _tmp724;return(_tmp724.f1=inflow,((_tmp724.f2=_tmp147->unknown_all,_tmp724)));}_LL11D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp171->tag != 1)goto _LL11F;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 1)goto _LL11F;}}_LL11E: {
# 987
struct _tuple15 _tmp725;return(_tmp725.f1=inflow,((_tmp725.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp725)));}_LL11F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp173->tag != 1)goto _LL121;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 3)goto _LL121;else{_tmp175=_tmp174->f1;}}}_LL120:
# 990
 _tmp178=_tmp175;goto _LL122;_LL121: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp176->tag != 1)goto _LL123;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp177=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp176->f2);if(_tmp177->tag != 4)goto _LL123;else{_tmp178=_tmp177->f1;}}}_LL122:
 _tmp17B=_tmp178;goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp179->tag != 1)goto _LL125;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp17A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp179->f2);if(_tmp17A->tag != 5)goto _LL125;else{_tmp17B=_tmp17A->f1;}}}_LL124: {
# 994
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp726;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp726.tag=0,((_tmp726.f1=_tmp17B,_tmp726)));
struct _tuple15 _tmp727;return(_tmp727.f1=inflow,((_tmp727.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp727)));}_LL125: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp17C->tag != 2)goto _LL127;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LL126: {
# 1001
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp17E);union Cyc_CfFlowInfo_FlowInfo _tmp1E2;void*_tmp1E3;struct _tuple15 _tmp1E1=_stmttmp4E;_tmp1E2=_tmp1E1.f1;_tmp1E3=_tmp1E1.f2;{
struct _tuple15 _tmp728;return(_tmp728.f1=_tmp1E2,((_tmp728.f2=_tmp1E3,_tmp728)));};}_LL127: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp17F->tag != 4)goto _LL129;else{_tmp180=_tmp17F->f1;}}_LL128: {
# 1005
struct Cyc_List_List _tmp729;struct Cyc_List_List _tmp1E5=(_tmp729.hd=_tmp180,((_tmp729.tl=0,_tmp729)));
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1E5);union Cyc_CfFlowInfo_FlowInfo _tmp1E7;struct _tuple15 _tmp1E6=_stmttmp4B;_tmp1E7=_tmp1E6.f1;{
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E7,0,0,_tmp180);union Cyc_CfFlowInfo_AbsLVal _tmp1E9;struct _tuple17 _tmp1E8=_stmttmp4C;_tmp1E9=_tmp1E8.f2;
{struct _tuple24 _tmp72A;struct _tuple24 _stmttmp4D=(_tmp72A.f1=_tmp1E9,((_tmp72A.f2=_tmp1E7,_tmp72A)));struct _tuple24 _tmp1EA=_stmttmp4D;struct Cyc_CfFlowInfo_Place*_tmp1EB;struct Cyc_Dict_Dict _tmp1EC;struct Cyc_List_List*_tmp1ED;_LL168: if(((_tmp1EA.f1).PlaceL).tag != 1)goto _LL16A;_tmp1EB=(struct Cyc_CfFlowInfo_Place*)((_tmp1EA.f1).PlaceL).val;if(((_tmp1EA.f2).ReachableFL).tag != 2)goto _LL16A;_tmp1EC=((struct _tuple14)((_tmp1EA.f2).ReachableFL).val).f1;_tmp1ED=((struct _tuple14)((_tmp1EA.f2).ReachableFL).val).f2;_LL169:
# 1010
 _tmp1ED=Cyc_Relations_reln_kill_exp(_tmp147->r,_tmp1ED,_tmp180);
_tmp1E7=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp147,_tmp180->loc,_tmp1EC,env->all_changed,_tmp1EB,_tmp147->unknown_all),_tmp1ED);
# 1014
goto _LL167;_LL16A:;_LL16B:
 goto _LL167;_LL167:;}{
# 1017
struct _tuple15 _tmp72B;return(_tmp72B.f1=_tmp1E7,((_tmp72B.f2=_tmp147->unknown_all,_tmp72B)));};};}_LL129: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp181->tag != 3)goto _LL12B;else{_tmp182=_tmp181->f1;if(_tmp181->f2 == 0)goto _LL12B;_tmp183=_tmp181->f3;}}_LL12A:
# 1020
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72E;void*_tmp72D;(_tmp72D=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp72E="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72E,sizeof(char),58))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}{
struct _tuple15 _tmp72F;return(_tmp72F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp72F.f2=_tmp147->unknown_all,_tmp72F)));};}{
# 1024
struct Cyc_List_List _tmp730;struct Cyc_List_List _tmp1F4=(_tmp730.hd=_tmp183,((_tmp730.tl=0,_tmp730)));
struct Cyc_List_List _tmp731;struct Cyc_List_List _tmp1F5=(_tmp731.hd=_tmp182,((_tmp731.tl=& _tmp1F4,_tmp731)));
struct _tuple15 _stmttmp49=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F5);union Cyc_CfFlowInfo_FlowInfo _tmp1F7;struct _tuple15 _tmp1F6=_stmttmp49;_tmp1F7=_tmp1F6.f1;{
struct _tuple17 _stmttmp4A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F7,0,0,_tmp182);union Cyc_CfFlowInfo_AbsLVal _tmp1F9;struct _tuple17 _tmp1F8=_stmttmp4A;_tmp1F9=_tmp1F8.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1FA=_tmp1F7;struct Cyc_Dict_Dict _tmp1FB;struct Cyc_List_List*_tmp1FC;_LL16D: if((_tmp1FA.ReachableFL).tag != 2)goto _LL16F;_tmp1FB=((struct _tuple14)(_tmp1FA.ReachableFL).val).f1;_tmp1FC=((struct _tuple14)(_tmp1FA.ReachableFL).val).f2;_LL16E:
# 1030
{union Cyc_CfFlowInfo_AbsLVal _tmp1FD=_tmp1F9;struct Cyc_CfFlowInfo_Place*_tmp1FE;_LL172: if((_tmp1FD.PlaceL).tag != 1)goto _LL174;_tmp1FE=(struct Cyc_CfFlowInfo_Place*)(_tmp1FD.PlaceL).val;_LL173:
# 1034
 _tmp1FC=Cyc_Relations_reln_kill_exp(_tmp147->r,_tmp1FC,_tmp182);
_tmp1FB=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp182->loc,_tmp1FB,env->all_changed,_tmp1FE,_tmp147->unknown_all);
# 1037
_tmp1F7=Cyc_CfFlowInfo_ReachableFL(_tmp1FB,_tmp1FC);
# 1040
goto _LL171;_LL174: if((_tmp1FD.UnknownL).tag != 2)goto _LL171;_LL175:
# 1043
 goto _LL171;_LL171:;}
# 1045
goto _LL16C;_LL16F:;_LL170:
 goto _LL16C;_LL16C:;}{
# 1048
struct _tuple15 _tmp732;return(_tmp732.f1=_tmp1F7,((_tmp732.f2=_tmp147->unknown_all,_tmp732)));};};};_LL12B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp184->tag != 3)goto _LL12D;else{_tmp185=_tmp184->f1;if(_tmp184->f2 != 0)goto _LL12D;_tmp186=_tmp184->f3;}}_LL12C:
# 1052
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp735;void*_tmp734;(_tmp734=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp735="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp735,sizeof(char),58))),_tag_dyneither(_tmp734,sizeof(void*),0)));}{
struct _tuple15 _tmp736;return(_tmp736.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp736.f2=_tmp147->unknown_all,_tmp736)));};}{
# 1056
struct Cyc_Dict_Dict*_tmp205=env->all_changed;
# 1058
{struct Cyc_Absyn_Exp*_tmp737[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp737[1]=_tmp186,((_tmp737[0]=_tmp185,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp737,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp738;env->all_changed=((_tmp738=_region_malloc(env->r,sizeof(*_tmp738)),((_tmp738[0]=_tmp147->mt_place_set,_tmp738))));}{
struct _tuple17 _stmttmp47=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp185);union Cyc_CfFlowInfo_FlowInfo _tmp209;union Cyc_CfFlowInfo_AbsLVal _tmp20A;struct _tuple17 _tmp208=_stmttmp47;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;{
struct Cyc_Dict_Dict _tmp20B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp739;env->all_changed=((_tmp739=_region_malloc(env->r,sizeof(*_tmp739)),((_tmp739[0]=_tmp147->mt_place_set,_tmp739))));}{
struct _tuple15 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp186);union Cyc_CfFlowInfo_FlowInfo _tmp20E;void*_tmp20F;struct _tuple15 _tmp20D=_stmttmp48;_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;{
struct Cyc_Dict_Dict _tmp210=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp211=Cyc_CfFlowInfo_after_flow(_tmp147,& _tmp20B,_tmp209,_tmp20E,_tmp20B,_tmp210);
# 1069
union Cyc_CfFlowInfo_FlowInfo _tmp212=Cyc_CfFlowInfo_join_flow(_tmp147,_tmp205,inflow,_tmp211);
# 1072
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp212,inflow)){
if(_tmp205 == 0)
env->all_changed=0;else{
# 1076
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp205,
Cyc_CfFlowInfo_union_place_set(_tmp20B,_tmp210,0),0);}
# 1079
return Cyc_NewControlFlow_do_assign(_tmp147,env,_tmp211,_tmp185,_tmp20A,_tmp186,_tmp20F,e->loc);}
# 1082
inflow=_tmp212;};};};};}};_LL12D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp187->tag != 8)goto _LL12F;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LL12E: {
# 1087
struct _tuple15 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp188);union Cyc_CfFlowInfo_FlowInfo _tmp214;void*_tmp215;struct _tuple15 _tmp213=_stmttmp46;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp214,_tmp189);}_LL12F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp18A->tag != 10)goto _LL131;else{_tmp18B=_tmp18A->f1;}}_LL130: {
# 1091
struct _tuple15 _stmttmp45=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18B);union Cyc_CfFlowInfo_FlowInfo _tmp217;void*_tmp218;struct _tuple15 _tmp216=_stmttmp45;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp18B->loc,_tmp217,_tmp218);{
struct _tuple15 _tmp73A;return(_tmp73A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp73A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp73A)));};}_LL131: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp18C->tag != 9)goto _LL133;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL132: {
# 1096
struct _RegionHandle _tmp21A=_new_region("temp");struct _RegionHandle*temp=& _tmp21A;_push_region(temp);
{struct Cyc_List_List*_tmp73B;struct _tuple22 _stmttmp3F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp73B=_region_malloc(temp,sizeof(*_tmp73B)),((_tmp73B->hd=_tmp18D,((_tmp73B->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp18E),_tmp73B)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp21C;struct Cyc_List_List*_tmp21D;struct _tuple22 _tmp21B=_stmttmp3F;_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;{
# 1099
union Cyc_CfFlowInfo_FlowInfo _tmp21E=Cyc_NewControlFlow_use_Rval(env,_tmp18D->loc,_tmp21C,(void*)((struct Cyc_List_List*)_check_null(_tmp21D))->hd);
_tmp21D=_tmp21D->tl;{
# 1102
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp18D->topt));void*_tmp21F=_stmttmp40;void*_tmp221;_LL177: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F;if(_tmp220->tag != 5)goto _LL179;else{_tmp221=(_tmp220->f1).elt_typ;}}_LL178:
# 1107
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp221);void*_tmp222=_stmttmp41;struct Cyc_List_List*_tmp224;_LL17C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp222;if(_tmp223->tag != 9)goto _LL17E;else{_tmp224=(_tmp223->f1).attributes;}}_LL17D:
# 1109
 for(0;_tmp224 != 0;_tmp224=_tmp224->tl){
# 1111
void*_stmttmp42=(void*)_tmp224->hd;void*_tmp225=_stmttmp42;int _tmp227;int _tmp229;int _tmp22B;_LL181: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp226=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp225;if(_tmp226->tag != 20)goto _LL183;else{_tmp227=_tmp226->f1;}}_LL182:
# 1113
{struct Cyc_List_List*_tmp73C;init_params=((_tmp73C=_region_malloc(temp,sizeof(*_tmp73C)),((_tmp73C->hd=(void*)_tmp227,((_tmp73C->tl=init_params,_tmp73C))))));}goto _LL180;_LL183: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp228=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp225;if(_tmp228->tag != 21)goto _LL185;else{_tmp229=_tmp228->f1;}}_LL184:
# 1115
{struct Cyc_List_List*_tmp73D;nolive_unique_params=((_tmp73D=_region_malloc(temp,sizeof(*_tmp73D)),((_tmp73D->hd=(void*)_tmp229,((_tmp73D->tl=nolive_unique_params,_tmp73D))))));}
goto _LL180;_LL185: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp22A=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp225;if(_tmp22A->tag != 22)goto _LL187;else{_tmp22B=_tmp22A->f1;}}_LL186:
# 1119
{struct Cyc_List_List*_tmp73E;noconsume_params=((_tmp73E=_region_malloc(temp,sizeof(*_tmp73E)),((_tmp73E->hd=(void*)_tmp22B,((_tmp73E->tl=noconsume_params,_tmp73E))))));}
goto _LL180;_LL187:;_LL188:
 goto _LL180;_LL180:;}
# 1123
goto _LL17B;_LL17E:;_LL17F: {
const char*_tmp741;void*_tmp740;(_tmp740=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp741="anal_Rexp: bad function type",_tag_dyneither(_tmp741,sizeof(char),29))),_tag_dyneither(_tmp740,sizeof(void*),0)));}_LL17B:;}
# 1126
goto _LL176;_LL179:;_LL17A: {
const char*_tmp744;void*_tmp743;(_tmp743=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp744="anal_Rexp: bad function type",_tag_dyneither(_tmp744,sizeof(char),29))),_tag_dyneither(_tmp743,sizeof(void*),0)));}_LL176:;}
# 1129
{int i=1;for(0;_tmp21D != 0;(((_tmp21D=_tmp21D->tl,_tmp18E=((struct Cyc_List_List*)_check_null(_tmp18E))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp233=_tmp21C;struct Cyc_Dict_Dict _tmp234;_LL18A: if((_tmp233.BottomFL).tag != 1)goto _LL18C;_LL18B:
 goto _LL189;_LL18C: if((_tmp233.ReachableFL).tag != 2)goto _LL189;_tmp234=((struct _tuple14)(_tmp233.ReachableFL).val).f1;_LL18D:
# 1134
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp234,(void*)_tmp21D->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp747;void*_tmp746;(_tmp746=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18E))->hd)->loc,((_tmp747="expression may not be initialized",_tag_dyneither(_tmp747,sizeof(char),34))),_tag_dyneither(_tmp746,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp237=_tmp21E;struct Cyc_Dict_Dict _tmp238;struct Cyc_List_List*_tmp239;_LL18F: if((_tmp237.BottomFL).tag != 1)goto _LL191;_LL190:
 goto _LL18E;_LL191: if((_tmp237.ReachableFL).tag != 2)goto _LL18E;_tmp238=((struct _tuple14)(_tmp237.ReachableFL).val).f1;_tmp239=((struct _tuple14)(_tmp237.ReachableFL).val).f2;_LL192: {
# 1141
struct Cyc_Dict_Dict _tmp23A=Cyc_CfFlowInfo_escape_deref(_tmp147,_tmp238,env->all_changed,(void*)_tmp21D->hd);
{void*_stmttmp43=(void*)_tmp21D->hd;void*_tmp23B=_stmttmp43;struct Cyc_CfFlowInfo_Place*_tmp23D;_LL194: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp23B;if(_tmp23C->tag != 5)goto _LL196;else{_tmp23D=_tmp23C->f1;}}_LL195:
# 1144
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18E))->hd)->topt));void*_tmp23E=_stmttmp44;void*_tmp240;_LL199: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E;if(_tmp23F->tag != 5)goto _LL19B;else{_tmp240=(_tmp23F->f1).elt_typ;}}_LL19A:
# 1148
 _tmp23A=Cyc_CfFlowInfo_assign_place(_tmp147,((struct Cyc_Absyn_Exp*)_tmp18E->hd)->loc,_tmp23A,env->all_changed,_tmp23D,
# 1150
Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp240,0,_tmp147->esc_all));
# 1152
goto _LL198;_LL19B:;_LL19C: {
const char*_tmp74A;void*_tmp749;(_tmp749=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp74A="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp74A,sizeof(char),39))),_tag_dyneither(_tmp749,sizeof(void*),0)));}_LL198:;}
# 1155
goto _LL193;_LL196:;_LL197:
 goto _LL193;_LL193:;}
# 1158
_tmp21E=Cyc_CfFlowInfo_ReachableFL(_tmp23A,_tmp239);
goto _LL18E;}_LL18E:;}
# 1161
goto _LL189;_LL189:;}else{
# 1164
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1169
_tmp21E=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18E))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18E))->hd)->topt),_tmp21E,(void*)_tmp21D->hd);else{
# 1176
_tmp21E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18E))->hd)->loc,_tmp21E,(void*)_tmp21D->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp18E->hd)->topt),0))
_tmp21E=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp21E,(struct Cyc_Absyn_Exp*)_tmp18E->hd,((struct Cyc_Absyn_Exp*)_tmp18E->hd)->loc,(void*)_tmp21D->hd);}}}}
# 1183
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp18D->topt))){
struct _tuple15 _tmp74B;struct _tuple15 _tmp244=(_tmp74B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp74B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp74B)));_npop_handler(0);return _tmp244;}else{
# 1186
struct _tuple15 _tmp74C;struct _tuple15 _tmp246=(_tmp74C.f1=_tmp21E,((_tmp74C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp74C)));_npop_handler(0);return _tmp246;}};};}
# 1097
;_pop_region(temp);}_LL133: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp18F->tag != 33)goto _LL135;else{_tmp190=(_tmp18F->f1).is_calloc;_tmp191=(_tmp18F->f1).rgn;_tmp192=(_tmp18F->f1).elt_type;_tmp193=(_tmp18F->f1).num_elts;_tmp194=(_tmp18F->f1).fat_result;}}_LL134: {
# 1189
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp74F;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp74E;void*root=(void*)((_tmp74E=_region_malloc(_tmp147->r,sizeof(*_tmp74E)),((_tmp74E[0]=((_tmp74F.tag=1,((_tmp74F.f1=_tmp193,((_tmp74F.f2=(void*)_check_null(e->topt),_tmp74F)))))),_tmp74E))));
struct Cyc_CfFlowInfo_Place*_tmp750;struct Cyc_CfFlowInfo_Place*place=(_tmp750=_region_malloc(_tmp147->r,sizeof(*_tmp750)),((_tmp750->root=root,((_tmp750->fields=0,_tmp750)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp753;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp752;void*rval=(void*)((_tmp752=_region_malloc(_tmp147->r,sizeof(*_tmp752)),((_tmp752[0]=((_tmp753.tag=5,((_tmp753.f1=place,_tmp753)))),_tmp752))));
void*place_val;
if(_tmp194)place_val=_tmp147->notzeroall;else{
if(_tmp190)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp192)),0,_tmp147->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp192)),0,_tmp147->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp191 != 0){
struct _RegionHandle _tmp248=_new_region("temp");struct _RegionHandle*temp=& _tmp248;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp754[2];struct _tuple22 _stmttmp3D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp754[1]=_tmp193,((_tmp754[0]=_tmp191,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp754,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1200
union Cyc_CfFlowInfo_FlowInfo _tmp24A;struct Cyc_List_List*_tmp24B;struct _tuple22 _tmp249=_stmttmp3D;_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;
# 1202
outflow=_tmp24A;}
# 1200
;_pop_region(temp);}else{
# 1205
struct _tuple15 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp193);union Cyc_CfFlowInfo_FlowInfo _tmp24E;struct _tuple15 _tmp24D=_stmttmp3E;_tmp24E=_tmp24D.f1;
outflow=_tmp24E;}{
# 1208
union Cyc_CfFlowInfo_FlowInfo _tmp24F=outflow;struct Cyc_Dict_Dict _tmp250;struct Cyc_List_List*_tmp251;_LL19E: if((_tmp24F.BottomFL).tag != 1)goto _LL1A0;_LL19F: {
struct _tuple15 _tmp755;return(_tmp755.f1=outflow,((_tmp755.f2=rval,_tmp755)));}_LL1A0: if((_tmp24F.ReachableFL).tag != 2)goto _LL19D;_tmp250=((struct _tuple14)(_tmp24F.ReachableFL).val).f1;_tmp251=((struct _tuple14)(_tmp24F.ReachableFL).val).f2;_LL1A1: {
# 1211
struct _tuple15 _tmp756;return(_tmp756.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp250,root,place_val),_tmp251),((_tmp756.f2=rval,_tmp756)));}_LL19D:;};};}_LL135: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp195->tag != 34)goto _LL137;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LL136: {
# 1215
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1221
struct _RegionHandle _tmp259=_new_region("temp");struct _RegionHandle*temp=& _tmp259;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp757[2];struct _tuple22 _stmttmp3A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp757[1]=_tmp197,((_tmp757[0]=_tmp196,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp757,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1222
union Cyc_CfFlowInfo_FlowInfo _tmp25B;struct Cyc_List_List*_tmp25C;struct _tuple22 _tmp25A=_stmttmp3A;_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;
# 1224
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp25C))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp25C->tl))->hd;
outflow=_tmp25B;}{
# 1229
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp196);union Cyc_CfFlowInfo_AbsLVal _tmp25F;struct _tuple17 _tmp25E=_stmttmp3B;_tmp25F=_tmp25E.f2;{
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp197);union Cyc_CfFlowInfo_AbsLVal _tmp261;struct _tuple17 _tmp260=_stmttmp3C;_tmp261=_tmp260.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp262=outflow;struct Cyc_Dict_Dict _tmp263;struct Cyc_List_List*_tmp264;_LL1A3: if((_tmp262.ReachableFL).tag != 2)goto _LL1A5;_tmp263=((struct _tuple14)(_tmp262.ReachableFL).val).f1;_tmp264=((struct _tuple14)(_tmp262.ReachableFL).val).f2;_LL1A4:
# 1233
{union Cyc_CfFlowInfo_AbsLVal _tmp265=_tmp25F;struct Cyc_CfFlowInfo_Place*_tmp266;_LL1A8: if((_tmp265.PlaceL).tag != 1)goto _LL1AA;_tmp266=(struct Cyc_CfFlowInfo_Place*)(_tmp265.PlaceL).val;_LL1A9:
# 1235
 _tmp263=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp196->loc,_tmp263,env->all_changed,_tmp266,right_rval);
# 1237
goto _LL1A7;_LL1AA: if((_tmp265.UnknownL).tag != 2)goto _LL1A7;_LL1AB:
# 1242
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp263,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp75A;void*_tmp759;(_tmp759=0,Cyc_Tcutil_terr(_tmp197->loc,((_tmp75A="expression may not be fully initialized",_tag_dyneither(_tmp75A,sizeof(char),40))),_tag_dyneither(_tmp759,sizeof(void*),0)));}
goto _LL1A7;_LL1A7:;}
# 1246
{union Cyc_CfFlowInfo_AbsLVal _tmp269=_tmp261;struct Cyc_CfFlowInfo_Place*_tmp26A;_LL1AD: if((_tmp269.PlaceL).tag != 1)goto _LL1AF;_tmp26A=(struct Cyc_CfFlowInfo_Place*)(_tmp269.PlaceL).val;_LL1AE:
# 1248
 _tmp263=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp197->loc,_tmp263,env->all_changed,_tmp26A,left_rval);
# 1250
goto _LL1AC;_LL1AF: if((_tmp269.UnknownL).tag != 2)goto _LL1AC;_LL1B0:
# 1252
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp263,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp75D;void*_tmp75C;(_tmp75C=0,Cyc_Tcutil_terr(_tmp196->loc,((_tmp75D="expression may not be fully initialized",_tag_dyneither(_tmp75D,sizeof(char),40))),_tag_dyneither(_tmp75C,sizeof(void*),0)));}
goto _LL1AC;_LL1AC:;}
# 1257
_tmp264=Cyc_Relations_reln_kill_exp(_tmp147->r,_tmp264,_tmp196);
_tmp264=Cyc_Relations_reln_kill_exp(_tmp147->r,_tmp264,_tmp197);
# 1260
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp263,_tmp264);
goto _LL1A2;_LL1A5:;_LL1A6:
 goto _LL1A2;_LL1A2:;}{
# 1266
struct _tuple15 _tmp75E;struct _tuple15 _tmp26E=(_tmp75E.f1=outflow,((_tmp75E.f2=_tmp147->unknown_all,_tmp75E)));_npop_handler(0);return _tmp26E;};};};
# 1221
;_pop_region(temp);}_LL137: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp198->tag != 15)goto _LL139;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL138: {
# 1269
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp761;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp760;void*root=(void*)((_tmp760=_region_malloc(_tmp147->r,sizeof(*_tmp760)),((_tmp760[0]=((_tmp761.tag=1,((_tmp761.f1=_tmp19A,((_tmp761.f2=(void*)_check_null(e->topt),_tmp761)))))),_tmp760))));
struct Cyc_CfFlowInfo_Place*_tmp762;struct Cyc_CfFlowInfo_Place*place=(_tmp762=_region_malloc(_tmp147->r,sizeof(*_tmp762)),((_tmp762->root=root,((_tmp762->fields=0,_tmp762)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp765;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp764;void*rval=(void*)((_tmp764=_region_malloc(_tmp147->r,sizeof(*_tmp764)),((_tmp764[0]=((_tmp765.tag=5,((_tmp765.f1=place,_tmp765)))),_tmp764))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp199 != 0){
struct _RegionHandle _tmp26F=_new_region("temp");struct _RegionHandle*temp=& _tmp26F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp766[2];struct _tuple22 _stmttmp38=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp766[1]=_tmp19A,((_tmp766[0]=_tmp199,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp766,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1277
union Cyc_CfFlowInfo_FlowInfo _tmp271;struct Cyc_List_List*_tmp272;struct _tuple22 _tmp270=_stmttmp38;_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;
# 1279
outflow=_tmp271;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp272))->tl))->hd;}
# 1277
;_pop_region(temp);}else{
# 1283
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19A);union Cyc_CfFlowInfo_FlowInfo _tmp275;void*_tmp276;struct _tuple15 _tmp274=_stmttmp39;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;
outflow=_tmp275;
place_val=_tmp276;}{
# 1287
union Cyc_CfFlowInfo_FlowInfo _tmp277=outflow;struct Cyc_Dict_Dict _tmp278;struct Cyc_List_List*_tmp279;_LL1B2: if((_tmp277.BottomFL).tag != 1)goto _LL1B4;_LL1B3: {
struct _tuple15 _tmp767;return(_tmp767.f1=outflow,((_tmp767.f2=rval,_tmp767)));}_LL1B4: if((_tmp277.ReachableFL).tag != 2)goto _LL1B1;_tmp278=((struct _tuple14)(_tmp277.ReachableFL).val).f1;_tmp279=((struct _tuple14)(_tmp277.ReachableFL).val).f2;_LL1B5: {
# 1290
struct _tuple15 _tmp768;return(_tmp768.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp278,root,place_val),_tmp279),((_tmp768.f2=rval,_tmp768)));}_LL1B1:;};};}_LL139: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp19B->tag != 14)goto _LL13B;else{_tmp19C=_tmp19B->f1;}}_LL13A: {
# 1294
struct _tuple17 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp19C);union Cyc_CfFlowInfo_FlowInfo _tmp282;union Cyc_CfFlowInfo_AbsLVal _tmp283;struct _tuple17 _tmp281=_stmttmp37;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp284=_tmp283;struct Cyc_CfFlowInfo_Place*_tmp285;_LL1B7: if((_tmp284.UnknownL).tag != 2)goto _LL1B9;_LL1B8: {
struct _tuple15 _tmp769;return(_tmp769.f1=_tmp282,((_tmp769.f2=_tmp147->notzeroall,_tmp769)));}_LL1B9: if((_tmp284.PlaceL).tag != 1)goto _LL1B6;_tmp285=(struct Cyc_CfFlowInfo_Place*)(_tmp284.PlaceL).val;_LL1BA: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp76F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp76E;struct _tuple15 _tmp76D;return(_tmp76D.f1=_tmp282,((_tmp76D.f2=(void*)((_tmp76F=_region_malloc(env->r,sizeof(*_tmp76F)),((_tmp76F[0]=((_tmp76E.tag=5,((_tmp76E.f1=_tmp285,_tmp76E)))),_tmp76F)))),_tmp76D)));}_LL1B6:;};}_LL13B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp19D->tag != 19)goto _LL13D;else{_tmp19E=_tmp19D->f1;}}_LL13C: {
# 1301
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19E);union Cyc_CfFlowInfo_FlowInfo _tmp28B;void*_tmp28C;struct _tuple15 _tmp28A=_stmttmp36;_tmp28B=_tmp28A.f1;_tmp28C=_tmp28A.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp28B,_tmp19E,_tmp28C);}_LL13D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp19F->tag != 20)goto _LL13F;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL13E: {
# 1305
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A0);union Cyc_CfFlowInfo_FlowInfo _tmp28E;void*_tmp28F;struct _tuple15 _tmp28D=_stmttmp35;_tmp28E=_tmp28D.f1;_tmp28F=_tmp28D.f2;
if(_tmp1A0->topt == 0){
{const char*_tmp773;void*_tmp772[1];struct Cyc_String_pa_PrintArg_struct _tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp772[0]=& _tmp771,Cyc_fprintf(Cyc_stderr,((_tmp773="aggrmember exp %s\n",_tag_dyneither(_tmp773,sizeof(char),19))),_tag_dyneither(_tmp772,sizeof(void*),1)))))));}{
const char*_tmp777;void*_tmp776[1];struct Cyc_String_pa_PrintArg_struct _tmp775;(_tmp775.tag=0,((_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1A0)),((_tmp776[0]=& _tmp775,Cyc_fprintf(Cyc_stderr,((_tmp777="oops! %s.topt = null!\n",_tag_dyneither(_tmp777,sizeof(char),23))),_tag_dyneither(_tmp776,sizeof(void*),1)))))));};}{
# 1310
void*_tmp296=(void*)_check_null(_tmp1A0->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp296)){
# 1313
struct _tuple15 _tmp778;return(_tmp778.f1=_tmp28E,((_tmp778.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp778)));}
# 1315
if(Cyc_Absyn_is_require_union_type(_tmp296))
Cyc_NewControlFlow_check_requires(_tmp1A0->loc,_tmp147->r,_tmp296,_tmp1A1,_tmp28E);{
# 1318
void*_tmp298=_tmp28F;int _tmp29B;int _tmp29C;struct _dyneither_ptr _tmp29D;int _tmp29F;int _tmp2A0;struct _dyneither_ptr _tmp2A1;_LL1BC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp299=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp298;if(_tmp299->tag != 8)goto _LL1BE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp299->f2);if(_tmp29A->tag != 6)goto _LL1BE;else{_tmp29B=(_tmp29A->f1).is_union;_tmp29C=(_tmp29A->f1).fieldnum;_tmp29D=_tmp29A->f2;}}}_LL1BD:
# 1320
 _tmp29F=_tmp29B;_tmp2A0=_tmp29C;_tmp2A1=_tmp29D;goto _LL1BF;_LL1BE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp298;if(_tmp29E->tag != 6)goto _LL1C0;else{_tmp29F=(_tmp29E->f1).is_union;_tmp2A0=(_tmp29E->f1).fieldnum;_tmp2A1=_tmp29E->f2;}}_LL1BF: {
# 1322
int _tmp2A2=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A0->topt),_tmp1A1);
if((_tmp29F  && _tmp2A0 != - 1) && _tmp2A0 != _tmp2A2){
struct _tuple15 _tmp779;return(_tmp779.f1=_tmp28E,((_tmp779.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none),_tmp779)));}{
struct _tuple15 _tmp77A;return(_tmp77A.f1=_tmp28E,((_tmp77A.f2=*((void**)_check_dyneither_subscript(_tmp2A1,sizeof(void*),_tmp2A2)),_tmp77A)));};}_LL1C0:;_LL1C1:
# 1327
{const char*_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_fprintf(Cyc_stderr,((_tmp77D="the bad rexp is :",_tag_dyneither(_tmp77D,sizeof(char),18))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp28F);
{const char*_tmp780;void*_tmp77F;(_tmp77F=0,Cyc_fprintf(Cyc_stderr,((_tmp780="\n",_tag_dyneither(_tmp780,sizeof(char),2))),_tag_dyneither(_tmp77F,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp78D;const char*_tmp78C;void*_tmp78B[1];struct Cyc_String_pa_PrintArg_struct _tmp78A;struct Cyc_Core_Impossible_exn_struct*_tmp789;(int)_throw((void*)((_tmp789=_cycalloc(sizeof(*_tmp789)),((_tmp789[0]=((_tmp78D.tag=Cyc_Core_Impossible,((_tmp78D.f1=(struct _dyneither_ptr)((_tmp78A.tag=0,((_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp78B[0]=& _tmp78A,Cyc_aprintf(((_tmp78C="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp78C,sizeof(char),26))),_tag_dyneither(_tmp78B,sizeof(void*),1)))))))),_tmp78D)))),_tmp789)))));};_LL1BB:;};};}_LL13F: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1A2->tag != 37)goto _LL141;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LL140: {
# 1337
struct _tuple15 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A3);union Cyc_CfFlowInfo_FlowInfo _tmp2AF;void*_tmp2B0;struct _tuple15 _tmp2AE=_stmttmp34;_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;
# 1339
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1A3->topt))){
struct _tuple15 _tmp78E;return(_tmp78E.f1=_tmp2AF,((_tmp78E.f2=_tmp147->unknown_all,_tmp78E)));}{
void*_tmp2B2=_tmp2B0;int _tmp2B5;int _tmp2B6;struct _dyneither_ptr _tmp2B7;int _tmp2B9;int _tmp2BA;struct _dyneither_ptr _tmp2BB;_LL1C3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B3->tag != 8)goto _LL1C5;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2B3->f2);if(_tmp2B4->tag != 6)goto _LL1C5;else{_tmp2B5=(_tmp2B4->f1).is_union;_tmp2B6=(_tmp2B4->f1).fieldnum;_tmp2B7=_tmp2B4->f2;}}}_LL1C4:
# 1343
 _tmp2B9=_tmp2B5;_tmp2BA=_tmp2B6;_tmp2BB=_tmp2B7;goto _LL1C6;_LL1C5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B8->tag != 6)goto _LL1C7;else{_tmp2B9=(_tmp2B8->f1).is_union;_tmp2BA=(_tmp2B8->f1).fieldnum;_tmp2BB=_tmp2B8->f2;}}_LL1C6: {
# 1345
int _tmp2BC=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A3->topt),_tmp1A4);
if(_tmp2B9  && _tmp2BA != - 1){
if(_tmp2BA != _tmp2BC){
struct _tuple15 _tmp78F;return(_tmp78F.f1=_tmp2AF,((_tmp78F.f2=_tmp147->zero,_tmp78F)));}else{
# 1350
struct _tuple15 _tmp790;return(_tmp790.f1=_tmp2AF,((_tmp790.f2=_tmp147->notzeroall,_tmp790)));}}else{
# 1352
struct _tuple15 _tmp791;return(_tmp791.f1=_tmp2AF,((_tmp791.f2=_tmp147->unknown_all,_tmp791)));}}_LL1C7:;_LL1C8: {
# 1354
struct Cyc_Core_Impossible_exn_struct _tmp79E;const char*_tmp79D;void*_tmp79C[1];struct Cyc_String_pa_PrintArg_struct _tmp79B;struct Cyc_Core_Impossible_exn_struct*_tmp79A;(int)_throw((void*)((_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A[0]=((_tmp79E.tag=Cyc_Core_Impossible,((_tmp79E.f1=(struct _dyneither_ptr)((_tmp79B.tag=0,((_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp79C[0]=& _tmp79B,Cyc_aprintf(((_tmp79D="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp79D,sizeof(char),26))),_tag_dyneither(_tmp79C,sizeof(void*),1)))))))),_tmp79E)))),_tmp79A)))));}_LL1C2:;};}_LL141: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1A5->tag != 21)goto _LL143;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LL142: {
# 1359
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A6);union Cyc_CfFlowInfo_FlowInfo _tmp2C6;void*_tmp2C7;struct _tuple15 _tmp2C5=_stmttmp31;_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;{
# 1362
struct _tuple15 _stmttmp32=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C6,_tmp1A6,_tmp2C7);union Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple15 _tmp2C8=_stmttmp32;_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;{
# 1365
void*_stmttmp33=Cyc_Tcutil_compress((void*)_check_null(_tmp1A6->topt));void*_tmp2CB=_stmttmp33;void*_tmp2CD;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB;if(_tmp2CC->tag != 5)goto _LL1CC;else{_tmp2CD=(_tmp2CC->f1).elt_typ;}}_LL1CB:
# 1367
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2CD)){
# 1369
struct _tuple15 _tmp79F;return(_tmp79F.f1=_tmp2C9,((_tmp79F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp79F)));}
# 1371
if(Cyc_Absyn_is_require_union_type(_tmp2CD))
Cyc_NewControlFlow_check_requires(_tmp1A6->loc,_tmp147->r,_tmp2CD,_tmp1A7,_tmp2C6);{
# 1374
void*_tmp2CF=_tmp2CA;int _tmp2D2;int _tmp2D3;struct _dyneither_ptr _tmp2D4;int _tmp2D6;int _tmp2D7;struct _dyneither_ptr _tmp2D8;_LL1CF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2D0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D0->tag != 8)goto _LL1D1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2D0->f2);if(_tmp2D1->tag != 6)goto _LL1D1;else{_tmp2D2=(_tmp2D1->f1).is_union;_tmp2D3=(_tmp2D1->f1).fieldnum;_tmp2D4=_tmp2D1->f2;}}}_LL1D0:
# 1376
 _tmp2D6=_tmp2D2;_tmp2D7=_tmp2D3;_tmp2D8=_tmp2D4;goto _LL1D2;_LL1D1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D5->tag != 6)goto _LL1D3;else{_tmp2D6=(_tmp2D5->f1).is_union;_tmp2D7=(_tmp2D5->f1).fieldnum;_tmp2D8=_tmp2D5->f2;}}_LL1D2: {
# 1378
int _tmp2D9=Cyc_CfFlowInfo_get_field_index(_tmp2CD,_tmp1A7);
if((_tmp2D6  && _tmp2D7 != - 1) && _tmp2D7 != _tmp2D9){
struct _tuple15 _tmp7A0;return(_tmp7A0.f1=_tmp2C9,((_tmp7A0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none),_tmp7A0)));}{
struct _tuple15 _tmp7A1;return(_tmp7A1.f1=_tmp2C9,((_tmp7A1.f2=*((void**)_check_dyneither_subscript(_tmp2D8,sizeof(void*),_tmp2D9)),_tmp7A1)));};}_LL1D3:;_LL1D4: {
struct Cyc_Core_Impossible_exn_struct _tmp7A7;const char*_tmp7A6;struct Cyc_Core_Impossible_exn_struct*_tmp7A5;(int)_throw((void*)((_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A7.tag=Cyc_Core_Impossible,((_tmp7A7.f1=((_tmp7A6="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7A6,sizeof(char),21))),_tmp7A7)))),_tmp7A5)))));}_LL1CE:;};_LL1CC:;_LL1CD: {
# 1384
struct Cyc_Core_Impossible_exn_struct _tmp7AD;const char*_tmp7AC;struct Cyc_Core_Impossible_exn_struct*_tmp7AB;(int)_throw((void*)((_tmp7AB=_cycalloc(sizeof(*_tmp7AB)),((_tmp7AB[0]=((_tmp7AD.tag=Cyc_Core_Impossible,((_tmp7AD.f1=((_tmp7AC="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7AC,sizeof(char),25))),_tmp7AD)))),_tmp7AB)))));}_LL1C9:;};};}_LL143: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1A8->tag != 5)goto _LL145;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;_tmp1AB=_tmp1A8->f3;}}_LL144: {
# 1388
struct _tuple18 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A9);union Cyc_CfFlowInfo_FlowInfo _tmp2E3;union Cyc_CfFlowInfo_FlowInfo _tmp2E4;struct _tuple18 _tmp2E2=_stmttmp30;_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;{
struct _tuple15 _tmp2E5=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E3,_tmp1AA);
struct _tuple15 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E4,_tmp1AB);
# 1392
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,env->all_changed,_tmp2E5,_tmp2E6);};}_LL145: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1AC->tag != 6)goto _LL147;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL146: {
# 1395
struct _tuple18 _stmttmp2E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AD);union Cyc_CfFlowInfo_FlowInfo _tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp2E9;struct _tuple18 _tmp2E7=_stmttmp2E;_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;{
struct _tuple15 _stmttmp2F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E8,_tmp1AE);union Cyc_CfFlowInfo_FlowInfo _tmp2EB;void*_tmp2EC;struct _tuple15 _tmp2EA=_stmttmp2F;_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;{
struct _tuple15 _tmp7AE;struct _tuple15 _tmp2ED=(_tmp7AE.f1=_tmp2EB,((_tmp7AE.f2=_tmp2EC,_tmp7AE)));
struct _tuple15 _tmp7AF;struct _tuple15 _tmp2EE=(_tmp7AF.f1=_tmp2E9,((_tmp7AF.f2=_tmp147->zero,_tmp7AF)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,env->all_changed,_tmp2ED,_tmp2EE);};};}_LL147: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1AF->tag != 7)goto _LL149;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL148: {
# 1402
struct _tuple18 _stmttmp2C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B0);union Cyc_CfFlowInfo_FlowInfo _tmp2F2;union Cyc_CfFlowInfo_FlowInfo _tmp2F3;struct _tuple18 _tmp2F1=_stmttmp2C;_tmp2F2=_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;{
struct _tuple15 _stmttmp2D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F3,_tmp1B1);union Cyc_CfFlowInfo_FlowInfo _tmp2F5;void*_tmp2F6;struct _tuple15 _tmp2F4=_stmttmp2D;_tmp2F5=_tmp2F4.f1;_tmp2F6=_tmp2F4.f2;{
struct _tuple15 _tmp7B0;struct _tuple15 _tmp2F7=(_tmp7B0.f1=_tmp2F5,((_tmp7B0.f2=_tmp2F6,_tmp7B0)));
struct _tuple15 _tmp7B1;struct _tuple15 _tmp2F8=(_tmp7B1.f1=_tmp2F2,((_tmp7B1.f2=_tmp147->notzeroall,_tmp7B1)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,env->all_changed,_tmp2F7,_tmp2F8);};};}_LL149: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1B2->tag != 22)goto _LL14B;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL14A: {
# 1409
struct _RegionHandle _tmp2FB=_new_region("temp");struct _RegionHandle*temp=& _tmp2FB;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7B2[2];struct _tuple22 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7B2[1]=_tmp1B4,((_tmp7B2[0]=_tmp1B3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7B2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1410
union Cyc_CfFlowInfo_FlowInfo _tmp2FD;struct Cyc_List_List*_tmp2FE;struct _tuple22 _tmp2FC=_stmttmp28;_tmp2FD=_tmp2FC.f1;_tmp2FE=_tmp2FC.f2;{
# 1414
union Cyc_CfFlowInfo_FlowInfo _tmp2FF=_tmp2FD;
{union Cyc_CfFlowInfo_FlowInfo _tmp300=_tmp2FD;struct Cyc_Dict_Dict _tmp301;struct Cyc_List_List*_tmp302;_LL1D6: if((_tmp300.ReachableFL).tag != 2)goto _LL1D8;_tmp301=((struct _tuple14)(_tmp300.ReachableFL).val).f1;_tmp302=((struct _tuple14)(_tmp300.ReachableFL).val).f2;_LL1D7:
# 1419
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp301,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2FE))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7B5;void*_tmp7B4;(_tmp7B4=0,Cyc_CfFlowInfo_aerr(_tmp1B4->loc,((_tmp7B5="expression may not be initialized",_tag_dyneither(_tmp7B5,sizeof(char),34))),_tag_dyneither(_tmp7B4,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp305=Cyc_NewControlFlow_add_subscript_reln(_tmp147->r,_tmp302,_tmp1B3,_tmp1B4);
if(_tmp302 != _tmp305)
_tmp2FF=Cyc_CfFlowInfo_ReachableFL(_tmp301,_tmp305);
goto _LL1D5;};_LL1D8:;_LL1D9:
 goto _LL1D5;_LL1D5:;}{
# 1427
void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp1B3->topt));void*_tmp306=_stmttmp29;struct Cyc_List_List*_tmp308;union Cyc_Absyn_Constraint*_tmp30A;_LL1DB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp307=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp306;if(_tmp307->tag != 10)goto _LL1DD;else{_tmp308=_tmp307->f1;}}_LL1DC: {
# 1429
void*_stmttmp2B=(void*)((struct Cyc_List_List*)_check_null(_tmp2FE))->hd;void*_tmp30B=_stmttmp2B;struct _dyneither_ptr _tmp30E;struct _dyneither_ptr _tmp310;_LL1E2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp30C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp30B;if(_tmp30C->tag != 8)goto _LL1E4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp30C->f2);if(_tmp30D->tag != 6)goto _LL1E4;else{_tmp30E=_tmp30D->f2;}}}_LL1E3:
# 1431
 _tmp310=_tmp30E;goto _LL1E5;_LL1E4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30B;if(_tmp30F->tag != 6)goto _LL1E6;else{_tmp310=_tmp30F->f2;}}_LL1E5: {
# 1433
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1B4)).f1;
struct _tuple15 _tmp7B6;struct _tuple15 _tmp312=(_tmp7B6.f1=_tmp2FF,((_tmp7B6.f2=*((void**)_check_dyneither_subscript(_tmp310,sizeof(void*),(int)i)),_tmp7B6)));_npop_handler(0);return _tmp312;}_LL1E6:;_LL1E7: {
struct Cyc_Core_Impossible_exn_struct _tmp7BC;const char*_tmp7BB;struct Cyc_Core_Impossible_exn_struct*_tmp7BA;(int)_throw((void*)((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BC.tag=Cyc_Core_Impossible,((_tmp7BC.f1=((_tmp7BB="anal_Rexp: Subscript",_tag_dyneither(_tmp7BB,sizeof(char),21))),_tmp7BC)))),_tmp7BA)))));}_LL1E1:;}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp306;if(_tmp309->tag != 5)goto _LL1DF;else{_tmp30A=((_tmp309->f1).ptr_atts).bounds;}}_LL1DE: {
# 1438
struct _tuple15 _stmttmp2A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2FD,_tmp1B3,(void*)((struct Cyc_List_List*)_check_null(_tmp2FE))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp317;void*_tmp318;struct _tuple15 _tmp316=_stmttmp2A;_tmp317=_tmp316.f1;_tmp318=_tmp316.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp319=_tmp317;_LL1E9: if((_tmp319.BottomFL).tag != 1)goto _LL1EB;_LL1EA: {
struct _tuple15 _tmp7BD;struct _tuple15 _tmp31B=(_tmp7BD.f1=_tmp317,((_tmp7BD.f2=_tmp318,_tmp7BD)));_npop_handler(0);return _tmp31B;}_LL1EB:;_LL1EC: {
struct _tuple15 _tmp7BE;struct _tuple15 _tmp31D=(_tmp7BE.f1=_tmp2FF,((_tmp7BE.f2=_tmp318,_tmp7BE)));_npop_handler(0);return _tmp31D;}_LL1E8:;};}_LL1DF:;_LL1E0: {
# 1443
struct Cyc_Core_Impossible_exn_struct _tmp7C4;const char*_tmp7C3;struct Cyc_Core_Impossible_exn_struct*_tmp7C2;(int)_throw((void*)((_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((_tmp7C2[0]=((_tmp7C4.tag=Cyc_Core_Impossible,((_tmp7C4.f1=((_tmp7C3="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7C3,sizeof(char),33))),_tmp7C4)))),_tmp7C2)))));}_LL1DA:;};};}
# 1410
;_pop_region(temp);}_LL14B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1B5->tag != 30)goto _LL14D;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL14C:
# 1448
 _tmp1B9=_tmp1B6;goto _LL14E;_LL14D: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1B8->tag != 23)goto _LL14F;else{_tmp1B9=_tmp1B8->f1;}}_LL14E: {
# 1450
struct _RegionHandle _tmp322=_new_region("temp");struct _RegionHandle*temp=& _tmp322;_push_region(temp);
{struct _tuple22 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1B9,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp324;struct Cyc_List_List*_tmp325;struct _tuple22 _tmp323=_stmttmp27;_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;{
unsigned int _tmp7CA;unsigned int _tmp7C9;struct _dyneither_ptr _tmp7C8;void**_tmp7C7;unsigned int _tmp7C6;struct _dyneither_ptr aggrdict=
(_tmp7C6=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B9),((_tmp7C7=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7C6)),((_tmp7C8=_tag_dyneither(_tmp7C7,sizeof(void*),_tmp7C6),((((_tmp7C9=_tmp7C6,_tmp7CB(& _tmp7CA,& _tmp7C9,& _tmp7C7,& _tmp325))),_tmp7C8)))))));
# 1458
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7D5;struct Cyc_CfFlowInfo_UnionRInfo _tmp7D4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7D3;struct _tuple15 _tmp7D2;struct _tuple15 _tmp32A=(_tmp7D2.f1=_tmp324,((_tmp7D2.f2=(void*)((_tmp7D5=_region_malloc(env->r,sizeof(*_tmp7D5)),((_tmp7D5[0]=((_tmp7D3.tag=6,((_tmp7D3.f1=((_tmp7D4.is_union=0,((_tmp7D4.fieldnum=- 1,_tmp7D4)))),((_tmp7D3.f2=aggrdict,_tmp7D3)))))),_tmp7D5)))),_tmp7D2)));_npop_handler(0);return _tmp32A;};}
# 1451
;_pop_region(temp);}_LL14F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1BA->tag != 29)goto _LL151;else{_tmp1BB=_tmp1BA->f2;}}_LL150: {
# 1461
struct Cyc_List_List*fs;
{void*_stmttmp26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp330=_stmttmp26;struct Cyc_List_List*_tmp332;_LL1EE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp330;if(_tmp331->tag != 12)goto _LL1F0;else{_tmp332=_tmp331->f2;}}_LL1EF:
 fs=_tmp332;goto _LL1ED;_LL1F0:;_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7DB;const char*_tmp7DA;struct Cyc_Core_Impossible_exn_struct*_tmp7D9;(int)_throw((void*)((_tmp7D9=_cycalloc(sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DB.tag=Cyc_Core_Impossible,((_tmp7DB.f1=((_tmp7DA="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7DA,sizeof(char),35))),_tmp7DB)))),_tmp7D9)))));}_LL1ED:;}
# 1466
_tmp1BD=_tmp1BB;_tmp1BE=Cyc_Absyn_StructA;_tmp1BF=fs;goto _LL152;}_LL151: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1BC->tag != 28)goto _LL153;else{_tmp1BD=_tmp1BC->f3;if(_tmp1BC->f4 == 0)goto _LL153;_tmp1BE=(_tmp1BC->f4)->kind;if((_tmp1BC->f4)->impl == 0)goto _LL153;_tmp1BF=((_tmp1BC->f4)->impl)->fields;}}_LL152: {
# 1468
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);
{struct _tuple22 _stmttmp24=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BD),1,1);
# 1470
union Cyc_CfFlowInfo_FlowInfo _tmp338;struct Cyc_List_List*_tmp339;struct _tuple22 _tmp337=_stmttmp24;_tmp338=_tmp337.f1;_tmp339=_tmp337.f2;{
# 1472
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp147,_tmp1BF,_tmp1BE == Cyc_Absyn_UnionA,_tmp147->unknown_all);
# 1475
int field_no=-1;
{int i=0;for(0;_tmp339 != 0;(((_tmp339=_tmp339->tl,_tmp1BD=_tmp1BD->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1BD))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp25=(void*)ds->hd;void*_tmp33A=_stmttmp25;struct _dyneither_ptr*_tmp33D;_LL1F3: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33A;if(_tmp33B->tag != 0)goto _LL1F5;}_LL1F4: {
struct Cyc_Core_Impossible_exn_struct _tmp7E1;const char*_tmp7E0;struct Cyc_Core_Impossible_exn_struct*_tmp7DF;(int)_throw((void*)((_tmp7DF=_cycalloc(sizeof(*_tmp7DF)),((_tmp7DF[0]=((_tmp7E1.tag=Cyc_Core_Impossible,((_tmp7E1.f1=((_tmp7E0="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7E0,sizeof(char),22))),_tmp7E1)))),_tmp7DF)))));}_LL1F5: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33A;if(_tmp33C->tag != 1)goto _LL1F2;else{_tmp33D=_tmp33C->f1;}}_LL1F6:
# 1482
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1BF,_tmp33D);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp339->hd;
# 1485
if(_tmp1BE == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp341=(*((struct _tuple25*)_tmp1BD->hd)).f2;
_tmp338=Cyc_NewControlFlow_use_Rval(env,_tmp341->loc,_tmp338,(void*)_tmp339->hd);
# 1489
Cyc_NewControlFlow_check_requires(_tmp341->loc,_tmp147->r,exp_type,_tmp33D,_tmp338);}_LL1F2:;}}}{
# 1492
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7E7;struct Cyc_CfFlowInfo_UnionRInfo _tmp7E6;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7E5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp342=(_tmp7E5=_region_malloc(env->r,sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E7.tag=6,((_tmp7E7.f1=((_tmp7E6.is_union=_tmp1BE == Cyc_Absyn_UnionA,((_tmp7E6.fieldnum=field_no,_tmp7E6)))),((_tmp7E7.f2=aggrdict,_tmp7E7)))))),_tmp7E5)));
struct _tuple15 _tmp7E8;struct _tuple15 _tmp344=(_tmp7E8.f1=_tmp338,((_tmp7E8.f2=(void*)_tmp342,_tmp7E8)));_npop_handler(0);return _tmp344;};};}
# 1470
;_pop_region(temp);}_LL153: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1C0->tag != 28)goto _LL155;}_LL154: {
# 1496
struct Cyc_Core_Impossible_exn_struct _tmp7EE;const char*_tmp7ED;struct Cyc_Core_Impossible_exn_struct*_tmp7EC;(int)_throw((void*)((_tmp7EC=_cycalloc(sizeof(*_tmp7EC)),((_tmp7EC[0]=((_tmp7EE.tag=Cyc_Core_Impossible,((_tmp7EE.f1=((_tmp7ED="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7ED,sizeof(char),31))),_tmp7EE)))),_tmp7EC)))));}_LL155: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1C1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1C1->tag != 25)goto _LL157;else{_tmp1C2=_tmp1C1->f1;}}_LL156: {
# 1498
struct _RegionHandle _tmp34B=_new_region("temp");struct _RegionHandle*temp=& _tmp34B;_push_region(temp);
{struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1C2);
struct _tuple22 _stmttmp23=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp34C,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp34E;struct Cyc_List_List*_tmp34F;struct _tuple22 _tmp34D=_stmttmp23;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;
for(0;_tmp34F != 0;(_tmp34F=_tmp34F->tl,_tmp34C=_tmp34C->tl)){
_tmp34E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp34C))->hd)->loc,_tmp34E,(void*)_tmp34F->hd);}{
struct _tuple15 _tmp7EF;struct _tuple15 _tmp351=(_tmp7EF.f1=_tmp34E,((_tmp7EF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all),_tmp7EF)));_npop_handler(0);return _tmp351;};}
# 1499
;_pop_region(temp);}_LL157: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1C3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1C3->tag != 26)goto _LL159;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;_tmp1C6=_tmp1C3->f3;_tmp1C7=_tmp1C3->f4;}}_LL158: {
# 1507
struct _tuple15 _stmttmp20=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C5);union Cyc_CfFlowInfo_FlowInfo _tmp353;void*_tmp354;struct _tuple15 _tmp352=_stmttmp20;_tmp353=_tmp352.f1;_tmp354=_tmp352.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp355=_tmp353;struct Cyc_Dict_Dict _tmp356;struct Cyc_List_List*_tmp357;_LL1F8: if((_tmp355.BottomFL).tag != 1)goto _LL1FA;_LL1F9: {
struct _tuple15 _tmp7F0;return(_tmp7F0.f1=_tmp353,((_tmp7F0.f2=_tmp147->unknown_all,_tmp7F0)));}_LL1FA: if((_tmp355.ReachableFL).tag != 2)goto _LL1F7;_tmp356=((struct _tuple14)(_tmp355.ReachableFL).val).f1;_tmp357=((struct _tuple14)(_tmp355.ReachableFL).val).f2;_LL1FB:
# 1511
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp356,_tmp354)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_CfFlowInfo_aerr(_tmp1C5->loc,((_tmp7F3="expression may not be initialized",_tag_dyneither(_tmp7F3,sizeof(char),34))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}{
# 1515
struct Cyc_List_List*new_relns=_tmp357;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1C4);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1C5,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp357);
# 1521
if(_tmp357 != new_relns)
_tmp353=Cyc_CfFlowInfo_ReachableFL(_tmp356,new_relns);{
# 1525
void*_tmp35B=_tmp354;_LL1FD: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp35C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp35B;if(_tmp35C->tag != 0)goto _LL1FF;}_LL1FE: {
struct _tuple15 _tmp7F4;return(_tmp7F4.f1=_tmp353,((_tmp7F4.f2=_tmp147->unknown_all,_tmp7F4)));}_LL1FF: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp35D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp35B;if(_tmp35D->tag != 2)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp35E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp35B;if(_tmp35E->tag != 1)goto _LL203;}_LL202:
 goto _LL204;_LL203: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp35F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp35B;if(_tmp35F->tag != 5)goto _LL205;}_LL204: {
# 1530
struct Cyc_List_List _tmp7F5;struct Cyc_List_List _tmp361=(_tmp7F5.hd=_tmp1C4,((_tmp7F5.tl=0,_tmp7F5)));
_tmp353=Cyc_NewControlFlow_add_vars(_tmp147,_tmp353,& _tmp361,_tmp147->unknown_all,e->loc,0);{
# 1533
struct _tuple15 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp353,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp363;void*_tmp364;struct _tuple15 _tmp362=_stmttmp22;_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp365=_tmp363;struct Cyc_Dict_Dict _tmp366;_LL208: if((_tmp365.BottomFL).tag != 1)goto _LL20A;_LL209: {
struct _tuple15 _tmp7F6;return(_tmp7F6.f1=_tmp363,((_tmp7F6.f2=_tmp147->unknown_all,_tmp7F6)));}_LL20A: if((_tmp365.ReachableFL).tag != 2)goto _LL207;_tmp366=((struct _tuple14)(_tmp365.ReachableFL).val).f1;_LL20B:
# 1537
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp366,_tmp364)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,Cyc_CfFlowInfo_aerr(_tmp1C5->loc,((_tmp7F9="expression may not be initialized",_tag_dyneither(_tmp7F9,sizeof(char),34))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}{
struct _tuple15 _tmp7FA;return(_tmp7FA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FA.f2=_tmp147->unknown_all,_tmp7FA)));};}_LL207:;}
# 1542
_tmp353=_tmp363;
goto _LL206;};}_LL205:;_LL206:
# 1545
 while(1){
struct Cyc_List_List _tmp7FB;struct Cyc_List_List _tmp36C=(_tmp7FB.hd=_tmp1C4,((_tmp7FB.tl=0,_tmp7FB)));
_tmp353=Cyc_NewControlFlow_add_vars(_tmp147,_tmp353,& _tmp36C,_tmp147->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp21=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp353,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp36E;void*_tmp36F;struct _tuple15 _tmp36D=_stmttmp21;_tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp370=_tmp36E;struct Cyc_Dict_Dict _tmp371;_LL20D: if((_tmp370.BottomFL).tag != 1)goto _LL20F;_LL20E:
 goto _LL20C;_LL20F: if((_tmp370.ReachableFL).tag != 2)goto _LL20C;_tmp371=((struct _tuple14)(_tmp370.ReachableFL).val).f1;_LL210:
# 1552
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp371,_tmp36F)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7FE;void*_tmp7FD;(_tmp7FD=0,Cyc_CfFlowInfo_aerr(_tmp1C5->loc,((_tmp7FE="expression may not be initialized",_tag_dyneither(_tmp7FE,sizeof(char),34))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}{
struct _tuple15 _tmp7FF;return(_tmp7FF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FF.f2=_tmp147->unknown_all,_tmp7FF)));};}_LL20C:;}{
# 1557
union Cyc_CfFlowInfo_FlowInfo _tmp375=Cyc_CfFlowInfo_join_flow(_tmp147,env->all_changed,_tmp353,_tmp36E);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp375,_tmp353))
break;
_tmp353=_tmp375;};};}{
# 1562
struct _tuple15 _tmp800;return(_tmp800.f1=_tmp353,((_tmp800.f2=_tmp147->unknown_all,_tmp800)));};_LL1FC:;};};_LL1F7:;};}_LL159: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1C8->tag != 27)goto _LL15B;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=(void*)_tmp1C8->f2;_tmp1CB=_tmp1C8->f3;}}_LL15A: {
# 1568
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp803;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp802;void*root=(void*)((_tmp802=_region_malloc(_tmp147->r,sizeof(*_tmp802)),((_tmp802[0]=((_tmp803.tag=1,((_tmp803.f1=_tmp1C9,((_tmp803.f2=(void*)_check_null(e->topt),_tmp803)))))),_tmp802))));
struct Cyc_CfFlowInfo_Place*_tmp804;struct Cyc_CfFlowInfo_Place*place=(_tmp804=_region_malloc(_tmp147->r,sizeof(*_tmp804)),((_tmp804->root=root,((_tmp804->fields=0,_tmp804)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp807;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp806;void*rval=(void*)((_tmp806=_region_malloc(_tmp147->r,sizeof(*_tmp806)),((_tmp806[0]=((_tmp807.tag=5,((_tmp807.f1=place,_tmp807)))),_tmp806))));
void*place_val;
# 1576
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp1CA,0,_tmp147->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C9);union Cyc_CfFlowInfo_FlowInfo _tmp379;struct _tuple15 _tmp378=_stmttmp1F;_tmp379=_tmp378.f1;
outflow=_tmp379;{
union Cyc_CfFlowInfo_FlowInfo _tmp37A=outflow;struct Cyc_Dict_Dict _tmp37B;struct Cyc_List_List*_tmp37C;_LL212: if((_tmp37A.BottomFL).tag != 1)goto _LL214;_LL213: {
struct _tuple15 _tmp808;return(_tmp808.f1=outflow,((_tmp808.f2=rval,_tmp808)));}_LL214: if((_tmp37A.ReachableFL).tag != 2)goto _LL211;_tmp37B=((struct _tuple14)(_tmp37A.ReachableFL).val).f1;_tmp37C=((struct _tuple14)(_tmp37A.ReachableFL).val).f2;_LL215: {
# 1584
struct _tuple15 _tmp809;return(_tmp809.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp37B,root,place_val),_tmp37C),((_tmp809.f2=rval,_tmp809)));}_LL211:;};};};}_LL15B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1CC->tag != 36)goto _LL15D;else{_tmp1CD=_tmp1CC->f1;}}_LL15C: {
# 1588
struct _tuple16 _tmp80A;struct _tuple16 _tmp384=(_tmp80A.f1=(env->fenv)->unknown_all,((_tmp80A.f2=copy_ctxt,_tmp80A)));
union Cyc_CfFlowInfo_FlowInfo _tmp385=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1CD,& _tmp384);
struct _tuple15 _tmp80B;return(_tmp80B.f1=_tmp385,((_tmp80B.f2=_tmp384.f1,_tmp80B)));}_LL15D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1CE->tag != 1)goto _LL15F;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1CF=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1CE->f2);if(_tmp1CF->tag != 0)goto _LL15F;}}_LL15E:
# 1592
 goto _LL160;_LL15F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1D0->tag != 35)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1D1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1D1->tag != 24)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1D2->tag != 38)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp15C;if(_tmp1D3->tag != 39)goto _LLFE;}_LL166: {
# 1597
struct Cyc_Core_Impossible_exn_struct _tmp811;const char*_tmp810;struct Cyc_Core_Impossible_exn_struct*_tmp80F;(int)_throw((void*)((_tmp80F=_cycalloc(sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp811.tag=Cyc_Core_Impossible,((_tmp811.f1=((_tmp810="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp810,sizeof(char),31))),_tmp811)))),_tmp80F)))));}_LLFE:;};}
# 1607
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1615
struct Cyc_CfFlowInfo_FlowEnv*_tmp38B=env->fenv;
void*_stmttmp51=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp38C=_stmttmp51;void*_tmp38E;union Cyc_Absyn_Constraint*_tmp38F;union Cyc_Absyn_Constraint*_tmp390;_LL217: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C;if(_tmp38D->tag != 5)goto _LL219;else{_tmp38E=(_tmp38D->f1).elt_typ;_tmp38F=((_tmp38D->f1).ptr_atts).bounds;_tmp390=((_tmp38D->f1).ptr_atts).zero_term;}}_LL218: {
# 1618
union Cyc_CfFlowInfo_FlowInfo _tmp391=f;struct Cyc_Dict_Dict _tmp392;struct Cyc_List_List*_tmp393;_LL21C: if((_tmp391.BottomFL).tag != 1)goto _LL21E;_LL21D: {
struct _tuple17 _tmp812;return(_tmp812.f1=f,((_tmp812.f2=Cyc_CfFlowInfo_UnknownL(),_tmp812)));}_LL21E: if((_tmp391.ReachableFL).tag != 2)goto _LL21B;_tmp392=((struct _tuple14)(_tmp391.ReachableFL).val).f1;_tmp393=((struct _tuple14)(_tmp391.ReachableFL).val).f2;_LL21F: {
# 1622
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp38F)){
retry: {void*_tmp395=r;struct Cyc_Absyn_Vardecl*_tmp397;void*_tmp398;void*_tmp39C;struct Cyc_List_List*_tmp39D;enum Cyc_CfFlowInfo_InitLevel _tmp3A0;_LL221: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp396=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp396->tag != 8)goto _LL223;else{_tmp397=_tmp396->f1;_tmp398=(void*)_tmp396->f2;}}_LL222:
 r=_tmp398;locname=_tmp397;goto retry;_LL223: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp399=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp399->tag != 1)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp39A=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp39A->tag != 2)goto _LL227;}_LL226:
# 1628
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp815;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp814;e->annot=(void*)((_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=Cyc_CfFlowInfo_NotZero,((_tmp815.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp393),_tmp815)))),_tmp814))));}goto _LL220;_LL227: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp39B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp39B->tag != 5)goto _LL229;else{_tmp39C=(_tmp39B->f1)->root;_tmp39D=(_tmp39B->f1)->fields;}}_LL228:
# 1630
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp818;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp817;e->annot=(void*)((_tmp817=_cycalloc(sizeof(*_tmp817)),((_tmp817[0]=((_tmp818.tag=Cyc_CfFlowInfo_NotZero,((_tmp818.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp393),_tmp818)))),_tmp817))));}{
struct Cyc_CfFlowInfo_Place*_tmp81B;struct _tuple17 _tmp81A;return(_tmp81A.f1=f,((_tmp81A.f2=Cyc_CfFlowInfo_PlaceL(((_tmp81B=_region_malloc(_tmp38B->r,sizeof(*_tmp81B)),((_tmp81B->root=_tmp39C,((_tmp81B->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp38B->r,_tmp39D,flds),_tmp81B))))))),_tmp81A)));};_LL229: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp39E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp39E->tag != 0)goto _LL22B;}_LL22A:
# 1633
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp81C;return(_tmp81C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp81C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp81C)));};_LL22B: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp39F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp395;if(_tmp39F->tag != 3)goto _LL22D;else{_tmp3A0=_tmp39F->f1;}}_LL22C:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3A0,locname);goto _LL22E;_LL22D:;_LL22E:
# 1638
 if(passthrough_consumes){
void*_tmp3A8=r;void*_tmp3AA;_LL230: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8;if(_tmp3A9->tag != 7)goto _LL232;else{_tmp3AA=(void*)_tmp3A9->f3;}}_LL231:
 r=_tmp3AA;goto retry;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 1644
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp81E;e->annot=(void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp393),_tmp81F)))),_tmp81E))));};_LL220:;}}else{
# 1647
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp822;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp821;e->annot=(void*)((_tmp821=_cycalloc(sizeof(*_tmp821)),((_tmp821[0]=((_tmp822.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp822.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp393),_tmp822)))),_tmp821))));}
if(Cyc_CfFlowInfo_initlevel(_tmp38B,_tmp392,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp825;void*_tmp824;(_tmp824=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp825="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp825,sizeof(char),46))),_tag_dyneither(_tmp824,sizeof(void*),0)));}{
struct _tuple17 _tmp826;return(_tmp826.f1=f,((_tmp826.f2=Cyc_CfFlowInfo_UnknownL(),_tmp826)));};}_LL21B:;}_LL219:;_LL21A: {
# 1652
struct Cyc_Core_Impossible_exn_struct _tmp82C;const char*_tmp82B;struct Cyc_Core_Impossible_exn_struct*_tmp82A;(int)_throw((void*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82C.tag=Cyc_Core_Impossible,((_tmp82C.f1=((_tmp82B="left deref of non-pointer-type",_tag_dyneither(_tmp82B,sizeof(char),31))),_tmp82C)))),_tmp82A)))));}_LL216:;}
# 1660
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1663
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B5=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3B6=inflow;struct Cyc_Dict_Dict _tmp3B7;struct Cyc_List_List*_tmp3B8;_LL235: if((_tmp3B6.BottomFL).tag != 1)goto _LL237;_LL236: {
struct _tuple17 _tmp82D;return(_tmp82D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp82D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82D)));}_LL237: if((_tmp3B6.ReachableFL).tag != 2)goto _LL234;_tmp3B7=((struct _tuple14)(_tmp3B6.ReachableFL).val).f1;_tmp3B8=((struct _tuple14)(_tmp3B6.ReachableFL).val).f2;_LL238:
# 1668
 d=_tmp3B7;_LL234:;}{
# 1672
void*_stmttmp52=e->r;void*_tmp3BA=_stmttmp52;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C3;struct Cyc_Absyn_Vardecl*_tmp3C6;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Vardecl*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CE;struct _dyneither_ptr*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;struct _dyneither_ptr*_tmp3D7;_LL23A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3BB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3BB->tag != 13)goto _LL23C;else{_tmp3BC=_tmp3BB->f2;}}_LL23B:
 _tmp3BE=_tmp3BC;goto _LL23D;_LL23C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3BD=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3BD->tag != 11)goto _LL23E;else{_tmp3BE=_tmp3BD->f1;}}_LL23D:
 _tmp3C0=_tmp3BE;goto _LL23F;_LL23E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3BF=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3BF->tag != 12)goto _LL240;else{_tmp3C0=_tmp3BF->f1;}}_LL23F:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C0,flds);_LL240: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3C1->tag != 1)goto _LL242;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3C2=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3C1->f2);if(_tmp3C2->tag != 3)goto _LL242;else{_tmp3C3=_tmp3C2->f1;}}}_LL241:
# 1677
 _tmp3C6=_tmp3C3;goto _LL243;_LL242: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3C4->tag != 1)goto _LL244;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3C5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3C4->f2);if(_tmp3C5->tag != 4)goto _LL244;else{_tmp3C6=_tmp3C5->f1;}}}_LL243:
 _tmp3C9=_tmp3C6;goto _LL245;_LL244: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3C7->tag != 1)goto _LL246;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3C8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3C7->f2);if(_tmp3C8->tag != 5)goto _LL246;else{_tmp3C9=_tmp3C8->f1;}}}_LL245: {
# 1680
struct Cyc_CfFlowInfo_Place*_tmp837;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp836;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp835;struct _tuple17 _tmp834;return(_tmp834.f1=inflow,((_tmp834.f2=Cyc_CfFlowInfo_PlaceL(((_tmp837=_region_malloc(env->r,sizeof(*_tmp837)),((_tmp837->root=(void*)((_tmp835=_region_malloc(env->r,sizeof(*_tmp835)),((_tmp835[0]=((_tmp836.tag=0,((_tmp836.f1=_tmp3C9,_tmp836)))),_tmp835)))),((_tmp837->fields=flds,_tmp837))))))),_tmp834)));}_LL246: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3CA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3CA->tag != 1)goto _LL248;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3CB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3CA->f2);if(_tmp3CB->tag != 1)goto _LL248;else{_tmp3CC=_tmp3CB->f1;}}}_LL247: {
# 1682
struct _tuple17 _tmp838;return(_tmp838.f1=inflow,((_tmp838.f2=Cyc_CfFlowInfo_UnknownL(),_tmp838)));}_LL248: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3CD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3CD->tag != 21)goto _LL24A;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;}}_LL249:
# 1685
{void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3CE->topt));void*_tmp3DD=_stmttmp59;void*_tmp3DF;_LL253: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD;if(_tmp3DE->tag != 5)goto _LL255;else{_tmp3DF=(_tmp3DE->f1).elt_typ;}}_LL254:
# 1687
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3DF)){
Cyc_NewControlFlow_check_requires(_tmp3CE->loc,_tmp3B5->r,_tmp3DF,_tmp3CF,inflow);{
struct Cyc_List_List*_tmp839;flds=((_tmp839=_region_malloc(env->r,sizeof(*_tmp839)),((_tmp839->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3DF,_tmp3CF),((_tmp839->tl=flds,_tmp839))))));};}
# 1691
goto _LL252;_LL255:;_LL256: {
struct Cyc_Core_Impossible_exn_struct _tmp83F;const char*_tmp83E;struct Cyc_Core_Impossible_exn_struct*_tmp83D;(int)_throw((void*)((_tmp83D=_cycalloc(sizeof(*_tmp83D)),((_tmp83D[0]=((_tmp83F.tag=Cyc_Core_Impossible,((_tmp83F.f1=((_tmp83E="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp83E,sizeof(char),25))),_tmp83F)))),_tmp83D)))));}_LL252:;}
# 1694
_tmp3D1=_tmp3CE;goto _LL24B;_LL24A: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3D0->tag != 19)goto _LL24C;else{_tmp3D1=_tmp3D0->f1;}}_LL24B:
# 1697
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3D1->topt),1)){
# 1699
struct _tuple17 _stmttmp56=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3D1->topt),1),passthrough_consumes,_tmp3D1);
# 1699
union Cyc_CfFlowInfo_FlowInfo _tmp3E5;union Cyc_CfFlowInfo_AbsLVal _tmp3E6;struct _tuple17 _tmp3E4=_stmttmp56;_tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;{
# 1703
struct _tuple17 _tmp840;struct _tuple17 _stmttmp57=(_tmp840.f1=_tmp3E5,((_tmp840.f2=_tmp3E6,_tmp840)));struct _tuple17 _tmp3E7=_stmttmp57;struct Cyc_Dict_Dict _tmp3E8;struct Cyc_List_List*_tmp3E9;struct Cyc_CfFlowInfo_Place*_tmp3EA;_LL258: if(((_tmp3E7.f1).ReachableFL).tag != 2)goto _LL25A;_tmp3E8=((struct _tuple14)((_tmp3E7.f1).ReachableFL).val).f1;_tmp3E9=((struct _tuple14)((_tmp3E7.f1).ReachableFL).val).f2;if(((_tmp3E7.f2).PlaceL).tag != 1)goto _LL25A;_tmp3EA=(struct Cyc_CfFlowInfo_Place*)((_tmp3E7.f2).PlaceL).val;_LL259: {
# 1705
void*_tmp3EB=Cyc_CfFlowInfo_lookup_place(_tmp3E8,_tmp3EA);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp3EC=_tmp3EB;struct Cyc_Absyn_Vardecl*_tmp3EE;void*_tmp3EF;_LL25D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3ED=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EC;if(_tmp3ED->tag != 8)goto _LL25F;else{_tmp3EE=_tmp3ED->f1;_tmp3EF=(void*)_tmp3ED->f2;}}_LL25E:
 _tmp3EB=_tmp3EF;locname=_tmp3EE;goto retry;_LL25F: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3F0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3EC;if(_tmp3F0->tag != 7)goto _LL261;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3F1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp3F0->f3);if(_tmp3F1->tag != 5)goto _LL261;}}_LL260:
 goto _LL262;_LL261: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3F2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3EC;if(_tmp3F2->tag != 5)goto _LL263;}_LL262:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3E5,_tmp3D1,_tmp3E5,_tmp3EB,passthrough_consumes,flds);_LL263:;_LL264: {
# 1717
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3B5,_tmp3E8,_tmp3EB);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3B5->unknown_all: _tmp3B5->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B5,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3D1->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp843;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp842;void*new_root=(void*)((_tmp842=_region_malloc(_tmp3B5->r,sizeof(*_tmp842)),((_tmp842[0]=((_tmp843.tag=1,((_tmp843.f1=e,((_tmp843.f2=(void*)_check_null(e->topt),_tmp843)))))),_tmp842))));
struct Cyc_CfFlowInfo_Place*_tmp844;struct Cyc_CfFlowInfo_Place*place=(_tmp844=_region_malloc(_tmp3B5->r,sizeof(*_tmp844)),((_tmp844->root=new_root,((_tmp844->fields=0,_tmp844)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp847;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp846;void*res=(void*)((_tmp846=_region_malloc(_tmp3B5->r,sizeof(*_tmp846)),((_tmp846[0]=((_tmp847.tag=5,((_tmp847.f1=place,_tmp847)))),_tmp846))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp84A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp849;res=(void*)((_tmp849=_region_malloc(_tmp3B5->r,sizeof(*_tmp849)),((_tmp849[0]=((_tmp84A.tag=8,((_tmp84A.f1=locname,((_tmp84A.f2=res,_tmp84A)))))),_tmp849))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E8,new_root,new_rval);
_tmp3E8=Cyc_CfFlowInfo_assign_place(_tmp3B5,e->loc,_tmp3E8,env->all_changed,_tmp3EA,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3F5=Cyc_CfFlowInfo_ReachableFL(_tmp3E8,_tmp3E9);
# 1732
return Cyc_NewControlFlow_anal_derefL(env,_tmp3F5,_tmp3D1,_tmp3F5,res,passthrough_consumes,flds);};}_LL25C:;}}_LL25A:;_LL25B:
# 1734
 goto _LL257;_LL257:;};}{
# 1737
struct _tuple15 _stmttmp58=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3D1);union Cyc_CfFlowInfo_FlowInfo _tmp3FD;void*_tmp3FE;struct _tuple15 _tmp3FC=_stmttmp58;_tmp3FD=_tmp3FC.f1;_tmp3FE=_tmp3FC.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3D1,_tmp3FD,_tmp3FE,passthrough_consumes,flds);};_LL24C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3D2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3D2->tag != 22)goto _LL24E;else{_tmp3D3=_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;}}_LL24D: {
# 1741
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(_tmp3D3->topt));void*_tmp3FF=_stmttmp53;union Cyc_Absyn_Constraint*_tmp402;_LL266: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3FF;if(_tmp400->tag != 10)goto _LL268;}_LL267: {
# 1743
unsigned int _tmp403=(Cyc_Evexp_eval_const_uint_exp(_tmp3D4)).f1;
struct Cyc_List_List*_tmp84B;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3D3,((_tmp84B=_region_malloc(env->r,sizeof(*_tmp84B)),((_tmp84B->hd=(void*)((int)_tmp403),((_tmp84B->tl=flds,_tmp84B)))))));}_LL268: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FF;if(_tmp401->tag != 5)goto _LL26A;else{_tmp402=((_tmp401->f1).ptr_atts).bounds;}}_LL269: {
# 1746
struct _RegionHandle _tmp405=_new_region("temp");struct _RegionHandle*temp=& _tmp405;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp84C[2];struct _tuple22 _stmttmp54=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp84C[1]=_tmp3D4,((_tmp84C[0]=_tmp3D3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp84C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1747
union Cyc_CfFlowInfo_FlowInfo _tmp407;struct Cyc_List_List*_tmp408;struct _tuple22 _tmp406=_stmttmp54;_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;{
# 1749
union Cyc_CfFlowInfo_FlowInfo _tmp409=_tmp407;
{union Cyc_CfFlowInfo_FlowInfo _tmp40A=_tmp407;struct Cyc_Dict_Dict _tmp40B;struct Cyc_List_List*_tmp40C;_LL26D: if((_tmp40A.ReachableFL).tag != 2)goto _LL26F;_tmp40B=((struct _tuple14)(_tmp40A.ReachableFL).val).f1;_tmp40C=((struct _tuple14)(_tmp40A.ReachableFL).val).f2;_LL26E:
# 1752
 if(Cyc_CfFlowInfo_initlevel(_tmp3B5,_tmp40B,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp408))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp84F;void*_tmp84E;(_tmp84E=0,Cyc_CfFlowInfo_aerr(_tmp3D4->loc,((_tmp84F="expression may not be initialized",_tag_dyneither(_tmp84F,sizeof(char),34))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp40F=Cyc_NewControlFlow_add_subscript_reln(_tmp3B5->r,_tmp40C,_tmp3D3,_tmp3D4);
if(_tmp40C != _tmp40F)
_tmp409=Cyc_CfFlowInfo_ReachableFL(_tmp40B,_tmp40F);
goto _LL26C;};_LL26F:;_LL270:
 goto _LL26C;_LL26C:;}{
# 1760
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3D3,_tmp407,(void*)((struct Cyc_List_List*)_check_null(_tmp408))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp411;union Cyc_CfFlowInfo_AbsLVal _tmp412;struct _tuple17 _tmp410=_stmttmp55;_tmp411=_tmp410.f1;_tmp412=_tmp410.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp413=_tmp411;_LL272: if((_tmp413.BottomFL).tag != 1)goto _LL274;_LL273: {
struct _tuple17 _tmp850;struct _tuple17 _tmp415=(_tmp850.f1=_tmp411,((_tmp850.f2=_tmp412,_tmp850)));_npop_handler(0);return _tmp415;}_LL274:;_LL275: {
struct _tuple17 _tmp851;struct _tuple17 _tmp417=(_tmp851.f1=_tmp409,((_tmp851.f2=_tmp412,_tmp851)));_npop_handler(0);return _tmp417;}_LL271:;};};};}
# 1747
;_pop_region(temp);}_LL26A:;_LL26B: {
# 1766
struct Cyc_Core_Impossible_exn_struct _tmp857;const char*_tmp856;struct Cyc_Core_Impossible_exn_struct*_tmp855;(int)_throw((void*)((_tmp855=_cycalloc(sizeof(*_tmp855)),((_tmp855[0]=((_tmp857.tag=Cyc_Core_Impossible,((_tmp857.f1=((_tmp856="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp856,sizeof(char),33))),_tmp857)))),_tmp855)))));}_LL265:;}_LL24E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3D5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3BA;if(_tmp3D5->tag != 20)goto _LL250;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;}}_LL24F: {
# 1770
void*_tmp41C=(void*)_check_null(_tmp3D6->topt);
if(Cyc_Absyn_is_require_union_type(_tmp41C))
Cyc_NewControlFlow_check_requires(_tmp3D6->loc,_tmp3B5->r,_tmp41C,_tmp3D7,inflow);
# 1774
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp41C)){
struct _tuple17 _tmp858;return(_tmp858.f1=inflow,((_tmp858.f2=Cyc_CfFlowInfo_UnknownL(),_tmp858)));}{
# 1777
struct Cyc_List_List*_tmp859;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3D6,(
(_tmp859=_region_malloc(env->r,sizeof(*_tmp859)),((_tmp859->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp41C,_tmp3D7),((_tmp859->tl=flds,_tmp859)))))));};}_LL250:;_LL251: {
# 1780
struct _tuple17 _tmp85A;return(_tmp85A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp85A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp85A)));}_LL239:;};}
# 1784
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1787
struct _tuple17 _stmttmp5A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp421;union Cyc_CfFlowInfo_AbsLVal _tmp422;struct _tuple17 _tmp420=_stmttmp5A;_tmp421=_tmp420.f1;_tmp422=_tmp420.f2;{
struct _tuple17 _tmp85B;return(_tmp85B.f1=_tmp421,((_tmp85B.f2=_tmp422,_tmp85B)));};}
# 1794
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1796
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _stmttmp5B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp425;struct _tuple17 _tmp424=_stmttmp5B;_tmp425=_tmp424.f1;
inflow=_tmp425;}}}
# 1803
return inflow;}
# 1809
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1812
struct Cyc_CfFlowInfo_FlowEnv*_tmp426=env->fenv;
# 1815
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp427=_new_region("temp");struct _RegionHandle*temp=& _tmp427;_push_region(temp);{
struct _tuple22 _stmttmp5C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp429;struct Cyc_List_List*_tmp42A;struct _tuple22 _tmp428=_stmttmp5C;_tmp429=_tmp428.f1;_tmp42A=_tmp428.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp42A))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp42A->tl))->hd;
f=_tmp429;}{
# 1825
union Cyc_CfFlowInfo_FlowInfo _tmp42B=f;struct Cyc_Dict_Dict _tmp42C;struct Cyc_List_List*_tmp42D;_LL277: if((_tmp42B.BottomFL).tag != 1)goto _LL279;_LL278: {
struct _tuple18 _tmp85C;struct _tuple18 _tmp42F=(_tmp85C.f1=f,((_tmp85C.f2=f,_tmp85C)));_npop_handler(0);return _tmp42F;}_LL279: if((_tmp42B.ReachableFL).tag != 2)goto _LL276;_tmp42C=((struct _tuple14)(_tmp42B.ReachableFL).val).f1;_tmp42D=((struct _tuple14)(_tmp42B.ReachableFL).val).f2;_LL27A: {
# 1828
struct Cyc_Absyn_Exp*_tmp430=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp431=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42C,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp85F="expression may not be initialized",_tag_dyneither(_tmp85F,sizeof(char),34))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42C,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp862;void*_tmp861;(_tmp861=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp862="expression may not be initialized",_tag_dyneither(_tmp862,sizeof(char),34))),_tag_dyneither(_tmp861,sizeof(void*),0)));}{
# 1835
union Cyc_CfFlowInfo_FlowInfo _tmp436=f;
union Cyc_CfFlowInfo_FlowInfo _tmp437=f;
# 1840
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp438=r1;struct Cyc_Absyn_Vardecl*_tmp43A;void*_tmp43B;_LL27C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp439=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp438;if(_tmp439->tag != 8)goto _LL27E;else{_tmp43A=_tmp439->f1;_tmp43B=(void*)_tmp439->f2;}}_LL27D:
 r1=_tmp43B;r1n=_tmp43A;goto _LL27B;_LL27E:;_LL27F:
 goto _LL27B;_LL27B:;}
# 1846
{void*_tmp43C=r2;struct Cyc_Absyn_Vardecl*_tmp43E;void*_tmp43F;_LL281: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp43D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp43C;if(_tmp43D->tag != 8)goto _LL283;else{_tmp43E=_tmp43D->f1;_tmp43F=(void*)_tmp43D->f2;}}_LL282:
 r2=_tmp43F;r2n=_tmp43E;goto _LL280;_LL283:;_LL284:
 goto _LL280;_LL280:;}{
# 1851
struct _tuple0 _tmp863;struct _tuple0 _stmttmp5D=(_tmp863.f1=r1,((_tmp863.f2=r2,_tmp863)));struct _tuple0 _tmp440=_stmttmp5D;enum Cyc_CfFlowInfo_InitLevel _tmp442;enum Cyc_CfFlowInfo_InitLevel _tmp446;_LL286:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp441=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp441->tag != 3)goto _LL288;else{_tmp442=_tmp441->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp443=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp443->tag != 0)goto _LL288;};_LL287: {
# 1855
struct _tuple18 _stmttmp5F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp430,_tmp442,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp456;union Cyc_CfFlowInfo_FlowInfo _tmp457;struct _tuple18 _tmp455=_stmttmp5F;_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;
switch(p){case Cyc_Absyn_Eq: _LL29A:
 _tmp436=_tmp457;_tmp437=_tmp456;break;case Cyc_Absyn_Neq: _LL29B:
 _tmp436=_tmp456;_tmp437=_tmp457;break;default: _LL29C: {
struct Cyc_Core_Impossible_exn_struct _tmp869;const char*_tmp868;struct Cyc_Core_Impossible_exn_struct*_tmp867;(int)_throw((void*)((_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((_tmp869.tag=Cyc_Core_Impossible,((_tmp869.f1=((_tmp868="anal_test, zero-split",_tag_dyneither(_tmp868,sizeof(char),22))),_tmp869)))),_tmp867)))));}}
# 1861
goto _LL285;}_LL288:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp444=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp444->tag != 0)goto _LL28A;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp445->tag != 3)goto _LL28A;else{_tmp446=_tmp445->f1;}};_LL289: {
# 1863
struct _tuple18 _stmttmp5E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp431,_tmp446,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp45C;union Cyc_CfFlowInfo_FlowInfo _tmp45D;struct _tuple18 _tmp45B=_stmttmp5E;_tmp45C=_tmp45B.f1;_tmp45D=_tmp45B.f2;
switch(p){case Cyc_Absyn_Eq: _LL29E:
 _tmp436=_tmp45D;_tmp437=_tmp45C;break;case Cyc_Absyn_Neq: _LL29F:
 _tmp436=_tmp45C;_tmp437=_tmp45D;break;default: _LL2A0: {
struct Cyc_Core_Impossible_exn_struct _tmp86F;const char*_tmp86E;struct Cyc_Core_Impossible_exn_struct*_tmp86D;(int)_throw((void*)((_tmp86D=_cycalloc(sizeof(*_tmp86D)),((_tmp86D[0]=((_tmp86F.tag=Cyc_Core_Impossible,((_tmp86F.f1=((_tmp86E="anal_test, zero-split",_tag_dyneither(_tmp86E,sizeof(char),22))),_tmp86F)))),_tmp86D)))));}}
# 1869
goto _LL285;}_LL28A:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp447->tag != 0)goto _LL28C;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp448->tag != 0)goto _LL28C;};_LL28B:
# 1871
 if(p == Cyc_Absyn_Eq)_tmp437=Cyc_CfFlowInfo_BottomFL();else{
_tmp436=Cyc_CfFlowInfo_BottomFL();}
goto _LL285;_LL28C:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp449->tag != 0)goto _LL28E;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp44A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp44A->tag != 1)goto _LL28E;};_LL28D:
 goto _LL28F;_LL28E:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp44B->tag != 0)goto _LL290;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp44C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp44C->tag != 2)goto _LL290;};_LL28F:
 goto _LL291;_LL290:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp44D->tag != 0)goto _LL292;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp44E->tag != 5)goto _LL292;};_LL291:
 goto _LL293;_LL292:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp44F=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp44F->tag != 1)goto _LL294;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp450=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp450->tag != 0)goto _LL294;};_LL293:
 goto _LL295;_LL294:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp451=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp451->tag != 2)goto _LL296;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp452=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp452->tag != 0)goto _LL296;};_LL295:
 goto _LL297;_LL296:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp453=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp440.f1;if(_tmp453->tag != 5)goto _LL298;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp454=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp440.f2;if(_tmp454->tag != 0)goto _LL298;};_LL297:
# 1880
 if(p == Cyc_Absyn_Neq)_tmp437=Cyc_CfFlowInfo_BottomFL();else{
_tmp436=Cyc_CfFlowInfo_BottomFL();}
goto _LL285;_LL298:;_LL299:
 goto _LL285;_LL285:;};}
# 1891
{struct _tuple0 _tmp870;struct _tuple0 _stmttmp60=(_tmp870.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp430->topt)),((_tmp870.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp431->topt)),_tmp870)));struct _tuple0 _tmp462=_stmttmp60;_LL2A3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp462.f1;if(_tmp463->tag != 6)goto _LL2A5;else{if(_tmp463->f1 != Cyc_Absyn_Unsigned)goto _LL2A5;}}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp462.f2;if(_tmp464->tag != 6)goto _LL2A7;else{if(_tmp464->f1 != Cyc_Absyn_Unsigned)goto _LL2A7;}}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp462.f1;if(_tmp465->tag != 19)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp462.f2;if(_tmp466->tag != 19)goto _LL2AB;}_LL2AA:
 goto _LL2A2;_LL2AB:;_LL2AC: {
struct _tuple18 _tmp871;struct _tuple18 _tmp468=(_tmp871.f1=_tmp436,((_tmp871.f2=_tmp437,_tmp871)));_npop_handler(0);return _tmp468;}_LL2A2:;}
# 1900
switch(p){case Cyc_Absyn_Eq: _LL2AD:
 goto _LL2AE;case Cyc_Absyn_Neq: _LL2AE: goto _LL2AF;case Cyc_Absyn_Gt: _LL2AF: goto _LL2B0;case Cyc_Absyn_Gte: _LL2B0: goto _LL2B1;case Cyc_Absyn_Lt: _LL2B1: goto _LL2B2;case Cyc_Absyn_Lte: _LL2B2: break;default: _LL2B3: {
struct _tuple18 _tmp872;struct _tuple18 _tmp46B=(_tmp872.f1=_tmp436,((_tmp872.f2=_tmp437,_tmp872)));_npop_handler(0);return _tmp46B;}}{
# 1905
struct _RegionHandle*_tmp46C=(env->fenv)->r;
struct _tuple13 _stmttmp61=Cyc_Relations_primop2relation(_tmp430,p,_tmp431);struct Cyc_Absyn_Exp*_tmp46E;enum Cyc_Relations_Relation _tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct _tuple13 _tmp46D=_stmttmp61;_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;_tmp470=_tmp46D.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1910
if(Cyc_Relations_exp2relnop(_tmp46E,& n1) && Cyc_Relations_exp2relnop(_tmp470,& n2)){
# 1912
struct Cyc_List_List*_tmp471=Cyc_Relations_add_relation(_tmp46C,n1,_tmp46F,n2,_tmp42D);
# 1916
struct Cyc_List_List*_tmp472=Cyc_Relations_add_relation(_tmp46C,n2,Cyc_Relations_flip_relation(_tmp46F),n1,_tmp42D);
struct _tuple18 _tmp873;struct _tuple18 _stmttmp62=(_tmp873.f1=_tmp436,((_tmp873.f2=_tmp437,_tmp873)));struct _tuple18 _tmp473=_stmttmp62;struct Cyc_Dict_Dict _tmp474;struct Cyc_Dict_Dict _tmp475;struct Cyc_Dict_Dict _tmp476;struct Cyc_Dict_Dict _tmp477;_LL2B6: if(((_tmp473.f1).ReachableFL).tag != 2)goto _LL2B8;_tmp474=((struct _tuple14)((_tmp473.f1).ReachableFL).val).f1;if(((_tmp473.f2).ReachableFL).tag != 2)goto _LL2B8;_tmp475=((struct _tuple14)((_tmp473.f2).ReachableFL).val).f1;_LL2B7: {
# 1919
struct _tuple18 _tmp874;struct _tuple18 _tmp479=(_tmp874.f1=Cyc_CfFlowInfo_ReachableFL(_tmp474,_tmp471),((_tmp874.f2=Cyc_CfFlowInfo_ReachableFL(_tmp475,_tmp472),_tmp874)));_npop_handler(0);return _tmp479;}_LL2B8: if(((_tmp473.f2).ReachableFL).tag != 2)goto _LL2BA;_tmp476=((struct _tuple14)((_tmp473.f2).ReachableFL).val).f1;_LL2B9: {
# 1921
struct _tuple18 _tmp875;struct _tuple18 _tmp47B=(_tmp875.f1=_tmp436,((_tmp875.f2=Cyc_CfFlowInfo_ReachableFL(_tmp476,_tmp472),_tmp875)));_npop_handler(0);return _tmp47B;}_LL2BA: if(((_tmp473.f1).ReachableFL).tag != 2)goto _LL2BC;_tmp477=((struct _tuple14)((_tmp473.f1).ReachableFL).val).f1;_LL2BB: {
# 1923
struct _tuple18 _tmp876;struct _tuple18 _tmp47D=(_tmp876.f1=Cyc_CfFlowInfo_ReachableFL(_tmp477,_tmp471),((_tmp876.f2=_tmp437,_tmp876)));_npop_handler(0);return _tmp47D;}_LL2BC:;_LL2BD: {
# 1925
struct _tuple18 _tmp877;struct _tuple18 _tmp47F=(_tmp877.f1=_tmp436,((_tmp877.f2=_tmp437,_tmp877)));_npop_handler(0);return _tmp47F;}_LL2B5:;}else{
# 1928
struct _tuple18 _tmp878;struct _tuple18 _tmp482=(_tmp878.f1=_tmp436,((_tmp878.f2=_tmp437,_tmp878)));_npop_handler(0);return _tmp482;}};};};}_LL276:;};
# 1817
;_pop_region(temp);}
# 1934
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1936
struct Cyc_CfFlowInfo_FlowEnv*_tmp483=env->fenv;
void*_stmttmp63=e->r;void*_tmp484=_stmttmp63;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp493;enum Cyc_Absyn_Primop _tmp495;struct Cyc_List_List*_tmp496;_LL2BF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp485=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp485->tag != 5)goto _LL2C1;else{_tmp486=_tmp485->f1;_tmp487=_tmp485->f2;_tmp488=_tmp485->f3;}}_LL2C0: {
# 1939
struct _tuple18 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp486);union Cyc_CfFlowInfo_FlowInfo _tmp498;union Cyc_CfFlowInfo_FlowInfo _tmp499;struct _tuple18 _tmp497=_stmttmp6B;_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;{
struct _tuple18 _stmttmp6C=Cyc_NewControlFlow_anal_test(env,_tmp498,_tmp487);union Cyc_CfFlowInfo_FlowInfo _tmp49B;union Cyc_CfFlowInfo_FlowInfo _tmp49C;struct _tuple18 _tmp49A=_stmttmp6C;_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;{
struct _tuple18 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,_tmp499,_tmp488);union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple18 _tmp49D=_stmttmp6D;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{
struct _tuple18 _tmp879;return(_tmp879.f1=Cyc_CfFlowInfo_join_flow(_tmp483,env->all_changed,_tmp49B,_tmp49E),((_tmp879.f2=
Cyc_CfFlowInfo_join_flow(_tmp483,env->all_changed,_tmp49C,_tmp49F),_tmp879)));};};};}_LL2C1: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp489=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp489->tag != 6)goto _LL2C3;else{_tmp48A=_tmp489->f1;_tmp48B=_tmp489->f2;}}_LL2C2: {
# 1945
struct _tuple18 _stmttmp69=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48A);union Cyc_CfFlowInfo_FlowInfo _tmp4A2;union Cyc_CfFlowInfo_FlowInfo _tmp4A3;struct _tuple18 _tmp4A1=_stmttmp69;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;{
struct _tuple18 _stmttmp6A=Cyc_NewControlFlow_anal_test(env,_tmp4A2,_tmp48B);union Cyc_CfFlowInfo_FlowInfo _tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;struct _tuple18 _tmp4A4=_stmttmp6A;_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;{
struct _tuple18 _tmp87A;return(_tmp87A.f1=_tmp4A5,((_tmp87A.f2=Cyc_CfFlowInfo_join_flow(_tmp483,env->all_changed,_tmp4A3,_tmp4A6),_tmp87A)));};};}_LL2C3: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp48C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp48C->tag != 7)goto _LL2C5;else{_tmp48D=_tmp48C->f1;_tmp48E=_tmp48C->f2;}}_LL2C4: {
# 1949
struct _tuple18 _stmttmp67=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48D);union Cyc_CfFlowInfo_FlowInfo _tmp4A9;union Cyc_CfFlowInfo_FlowInfo _tmp4AA;struct _tuple18 _tmp4A8=_stmttmp67;_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;{
struct _tuple18 _stmttmp68=Cyc_NewControlFlow_anal_test(env,_tmp4AA,_tmp48E);union Cyc_CfFlowInfo_FlowInfo _tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;struct _tuple18 _tmp4AB=_stmttmp68;_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{
struct _tuple18 _tmp87B;return(_tmp87B.f1=Cyc_CfFlowInfo_join_flow(_tmp483,env->all_changed,_tmp4A9,_tmp4AC),((_tmp87B.f2=_tmp4AD,_tmp87B)));};};}_LL2C5: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp48F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp48F->tag != 8)goto _LL2C7;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;}}_LL2C6: {
# 1953
struct _tuple15 _stmttmp66=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp490);union Cyc_CfFlowInfo_FlowInfo _tmp4B0;void*_tmp4B1;struct _tuple15 _tmp4AF=_stmttmp66;_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4B0,_tmp491);}_LL2C7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp492=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp492->tag != 2)goto _LL2C9;else{if(_tmp492->f1 != Cyc_Absyn_Not)goto _LL2C9;if(_tmp492->f2 == 0)goto _LL2C9;_tmp493=(struct Cyc_Absyn_Exp*)(_tmp492->f2)->hd;if((_tmp492->f2)->tl != 0)goto _LL2C9;}}_LL2C8: {
# 1956
struct _tuple18 _stmttmp65=Cyc_NewControlFlow_anal_test(env,inflow,_tmp493);union Cyc_CfFlowInfo_FlowInfo _tmp4B3;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;struct _tuple18 _tmp4B2=_stmttmp65;_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{
struct _tuple18 _tmp87C;return(_tmp87C.f1=_tmp4B4,((_tmp87C.f2=_tmp4B3,_tmp87C)));};}_LL2C9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp494=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp484;if(_tmp494->tag != 2)goto _LL2CB;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;}}_LL2CA:
# 1959
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp495,_tmp496);_LL2CB:;_LL2CC: {
# 1963
struct _tuple15 _stmttmp64=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4B7;void*_tmp4B8;struct _tuple15 _tmp4B6=_stmttmp64;_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B9=_tmp4B7;struct Cyc_Dict_Dict _tmp4BA;_LL2CE: if((_tmp4B9.BottomFL).tag != 1)goto _LL2D0;_LL2CF: {
struct _tuple18 _tmp87D;return(_tmp87D.f1=_tmp4B7,((_tmp87D.f2=_tmp4B7,_tmp87D)));}_LL2D0: if((_tmp4B9.ReachableFL).tag != 2)goto _LL2CD;_tmp4BA=((struct _tuple14)(_tmp4B9.ReachableFL).val).f1;_LL2D1: {
# 1967
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4BC=_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4BE;void*_tmp4BF;enum Cyc_CfFlowInfo_InitLevel _tmp4C8;_LL2D3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4BD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4BD->tag != 8)goto _LL2D5;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=(void*)_tmp4BD->f2;}}_LL2D4:
 _tmp4B8=_tmp4BF;locname=_tmp4BE;goto retry;_LL2D5: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C0->tag != 0)goto _LL2D7;}_LL2D6: {
struct _tuple18 _tmp87E;return(_tmp87E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87E.f2=_tmp4B7,_tmp87E)));}_LL2D7: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4C1=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C1->tag != 2)goto _LL2D9;}_LL2D8:
 goto _LL2DA;_LL2D9: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4C2=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C2->tag != 1)goto _LL2DB;}_LL2DA:
 goto _LL2DC;_LL2DB: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4C3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C3->tag != 5)goto _LL2DD;}_LL2DC: {
struct _tuple18 _tmp87F;return(_tmp87F.f1=_tmp4B7,((_tmp87F.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87F)));}_LL2DD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C4=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C4->tag != 3)goto _LL2DF;else{if(_tmp4C4->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DF;}}_LL2DE:
 goto _LL2E0;_LL2DF: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C5=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C5->tag != 4)goto _LL2E1;else{if(_tmp4C5->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2E1;}}_LL2E0:
 goto _LL2E2;_LL2E1: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4C6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C6->tag != 7)goto _LL2E3;}_LL2E2:
# 1977
{const char*_tmp882;void*_tmp881;(_tmp881=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp882="expression may not be initialized",_tag_dyneither(_tmp882,sizeof(char),34))),_tag_dyneither(_tmp881,sizeof(void*),0)));}{
struct _tuple18 _tmp883;return(_tmp883.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp883.f2=Cyc_CfFlowInfo_BottomFL(),_tmp883)));};_LL2E3: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C7->tag != 3)goto _LL2E5;else{_tmp4C8=_tmp4C7->f1;}}_LL2E4:
# 1980
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B7,e,_tmp4C8,locname);_LL2E5: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C9=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4C9->tag != 4)goto _LL2E7;}_LL2E6: {
struct _tuple18 _tmp884;return(_tmp884.f1=_tmp4B7,((_tmp884.f2=_tmp4B7,_tmp884)));}_LL2E7: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4CA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4BC;if(_tmp4CA->tag != 6)goto _LL2D2;}_LL2E8: {
struct Cyc_Core_Impossible_exn_struct _tmp88A;const char*_tmp889;struct Cyc_Core_Impossible_exn_struct*_tmp888;(int)_throw((void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp88A.tag=Cyc_Core_Impossible,((_tmp88A.f1=((_tmp889="anal_test",_tag_dyneither(_tmp889,sizeof(char),10))),_tmp88A)))),_tmp888)))));}_LL2D2:;}}_LL2CD:;};}_LL2BE:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1988
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 1990
unsigned int _tmp4D5;struct Cyc_NewControlFlow_AnalEnv*_tmp4D6;struct Cyc_Dict_Dict _tmp4D7;struct _tuple26*_tmp4D4=ckenv;_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;_tmp4D7=_tmp4D4->f3;{
void*_tmp4D8=root;struct Cyc_Absyn_Vardecl*_tmp4DA;_LL2EA: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D9=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4D8;if(_tmp4D9->tag != 0)goto _LL2EC;else{_tmp4DA=_tmp4D9->f1;}}_LL2EB:
# 1993
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4D6->fenv)->r,_tmp4DA->type)){
void*_tmp4DB=rval;_LL2EF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4DC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4DC->tag != 8)goto _LL2F1;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4DD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4DC->f2);if(_tmp4DD->tag != 7)goto _LL2F1;}}_LL2F0:
 goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4DE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4DE->tag != 8)goto _LL2F3;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4DE->f2);if(_tmp4DF->tag != 0)goto _LL2F3;}}_LL2F2:
 goto _LL2F4;_LL2F3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4E0->tag != 8)goto _LL2F5;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4E0->f2);if(_tmp4E1->tag != 3)goto _LL2F5;else{if(_tmp4E1->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F5;}}}_LL2F4:
 goto _LL2F6;_LL2F5: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4E2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4E2->tag != 7)goto _LL2F7;}_LL2F6:
 goto _LL2F8;_LL2F7: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4E3->tag != 0)goto _LL2F9;}_LL2F8:
 goto _LL2FA;_LL2F9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4DB;if(_tmp4E4->tag != 3)goto _LL2FB;else{if(_tmp4E4->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2FB;}}_LL2FA:
 goto _LL2EE;_LL2FB:;_LL2FC:
# 2002
{const char*_tmp88E;void*_tmp88D[1];struct Cyc_String_pa_PrintArg_struct _tmp88C;(_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4DA->name)),((_tmp88D[0]=& _tmp88C,Cyc_Tcutil_warn(_tmp4D5,((_tmp88E="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp88E,sizeof(char),57))),_tag_dyneither(_tmp88D,sizeof(void*),1)))))));}
goto _LL2EE;_LL2EE:;}
# 2005
goto _LL2E9;_LL2EC:;_LL2ED:
 goto _LL2E9;_LL2E9:;};}
# 2010
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4E8=flow;struct Cyc_Dict_Dict _tmp4E9;_LL2FE: if((_tmp4E8.BottomFL).tag != 1)goto _LL300;_LL2FF:
 return;_LL300: if((_tmp4E8.ReachableFL).tag != 2)goto _LL2FD;_tmp4E9=((struct _tuple14)(_tmp4E8.ReachableFL).val).f1;_LL301:
# 2014
{struct Cyc_List_List*_tmp4EA=env->param_roots;for(0;_tmp4EA != 0;_tmp4EA=_tmp4EA->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E9,Cyc_CfFlowInfo_lookup_place(_tmp4E9,(struct Cyc_CfFlowInfo_Place*)_tmp4EA->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_CfFlowInfo_aerr(loc,((_tmp891="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp891,sizeof(char),76))),_tag_dyneither(_tmp890,sizeof(void*),0)));}}}
# 2018
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp892;struct _tuple26 _tmp4ED=(_tmp892.f1=loc,((_tmp892.f2=env,((_tmp892.f3=_tmp4E9,_tmp892)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4ED,_tmp4E9);}
# 2022
return;_LL2FD:;}
# 2026
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2029
struct Cyc_List_List*_tmp4EF=0;
struct Cyc_List_List*_tmp4F0=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp6E=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct _tuple23*_tmp4F1=_stmttmp6E;_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;
if((_tmp4F2 != 0  && 
# 2036
Cyc_Tcutil_is_noalias_pointer((*_tmp4F2)->type,0)) && _tmp4F3 != 0){
# 2038
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp895;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp894;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F4=(_tmp894=_region_malloc(frgn,sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=0,((_tmp895.f1=*_tmp4F2,_tmp895)))),_tmp894)));
struct Cyc_CfFlowInfo_Place*_tmp896;struct Cyc_CfFlowInfo_Place*_tmp4F5=(_tmp896=_region_malloc(frgn,sizeof(*_tmp896)),((_tmp896->root=(void*)_tmp4F4,((_tmp896->fields=0,_tmp896)))));
{struct Cyc_List_List*_tmp897;_tmp4EF=((_tmp897=_region_malloc(frgn,sizeof(*_tmp897)),((_tmp897->hd=_tmp4F5,((_tmp897->tl=_tmp4EF,_tmp897))))));}{
struct Cyc_List_List*_tmp898;_tmp4F0=((_tmp898=_region_malloc(frgn,sizeof(*_tmp898)),((_tmp898->hd=_tmp4F3,((_tmp898->tl=_tmp4F0,_tmp898))))));};}}}{
# 2044
struct _tuple1 _tmp899;return(_tmp899.f1=_tmp4EF,((_tmp899.f2=_tmp4F0,_tmp899)));};}
# 2053
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2058
{union Cyc_CfFlowInfo_FlowInfo _tmp4FC=flow;_LL303: if((_tmp4FC.BottomFL).tag != 1)goto _LL305;_LL304:
 return flow;_LL305:;_LL306:
 goto _LL302;_LL302:;}{
# 2062
int _tmp4FD=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp4FE=Cyc_Absyn_exp_unsigned_one;
int _tmp4FF=0;
int _tmp500=1;
# 2069
{struct Cyc_List_List*_tmp501=consumed_vals;for(0;_tmp501 != 0;
(_tmp501=_tmp501->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp502=flow;struct Cyc_Dict_Dict _tmp503;_LL308: if((_tmp502.ReachableFL).tag != 2)goto _LL30A;_tmp503=((struct _tuple14)(_tmp502.ReachableFL).val).f1;_LL309: {
# 2079
void*_tmp504=Cyc_CfFlowInfo_lookup_place(_tmp503,(struct Cyc_CfFlowInfo_Place*)_tmp501->hd);
struct Cyc_CfFlowInfo_Place*_stmttmp6F=(struct Cyc_CfFlowInfo_Place*)_tmp501->hd;void*_tmp506;struct Cyc_CfFlowInfo_Place*_tmp505=_stmttmp6F;_tmp506=_tmp505->root;
{struct _tuple0 _tmp89A;struct _tuple0 _stmttmp70=(_tmp89A.f1=_tmp506,((_tmp89A.f2=_tmp504,_tmp89A)));struct _tuple0 _tmp507=_stmttmp70;struct Cyc_Absyn_Vardecl*_tmp509;struct Cyc_Absyn_Vardecl*_tmp50B;void*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50E;_LL30D:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp508=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp507.f1;if(_tmp508->tag != 0)goto _LL30F;else{_tmp509=_tmp508->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp50A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp507.f2;if(_tmp50A->tag != 8)goto _LL30F;else{_tmp50B=_tmp50A->f1;_tmp50C=(void*)_tmp50A->f2;}};_LL30E:
# 2083
 if(_tmp509 == _tmp50B){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4FE,_tmp500,_tmp50C,& _tmp4FF)){
if(!_tmp4FD){
const char*_tmp89E;void*_tmp89D[1];struct Cyc_String_pa_PrintArg_struct _tmp89C;(_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2088
Cyc_Absynpp_qvar2string(_tmp509->name)),((_tmp89D[0]=& _tmp89C,Cyc_CfFlowInfo_aerr(loc,((_tmp89E="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp89E,sizeof(char),58))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))));}}else{
# 2090
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp503,_tmp50C)!= Cyc_CfFlowInfo_AllIL  && !_tmp4FD){
const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_PrintArg_struct _tmp8A0;(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2093
Cyc_Absynpp_qvar2string(_tmp509->name)),((_tmp8A1[0]=& _tmp8A0,Cyc_CfFlowInfo_aerr(loc,((_tmp8A2="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp8A2,sizeof(char),84))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));}else{
# 2095
if(_tmp4FD){
# 2101
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp516;union Cyc_CfFlowInfo_AbsLVal _tmp517;struct _tuple17 _tmp515=_stmttmp71;_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp518=_tmp517;struct Cyc_CfFlowInfo_Place*_tmp519;_LL314: if((_tmp518.PlaceL).tag != 1)goto _LL316;_tmp519=(struct Cyc_CfFlowInfo_Place*)(_tmp518.PlaceL).val;_LL315: {
# 2105
void*_tmp51A=Cyc_CfFlowInfo_lookup_place(_tmp503,_tmp519);
_tmp51A=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp51A);{
# 2113
struct _tuple15 _tmp8A4;struct _tuple15 _tmp8A3;struct _tuple15 _stmttmp72=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2116
(_tmp8A3.f1=_tmp516,((_tmp8A3.f2=_tmp51A,_tmp8A3)))),(
(_tmp8A4.f1=_tmp516,((_tmp8A4.f2=_tmp50C,_tmp8A4)))));
# 2113
union Cyc_CfFlowInfo_FlowInfo _tmp51C;void*_tmp51D;struct _tuple15 _tmp51B=_stmttmp72;_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;
# 2118
{union Cyc_CfFlowInfo_FlowInfo _tmp51E=_tmp51C;struct Cyc_Dict_Dict _tmp51F;struct Cyc_List_List*_tmp520;_LL319: if((_tmp51E.ReachableFL).tag != 2)goto _LL31B;_tmp51F=((struct _tuple14)(_tmp51E.ReachableFL).val).f1;_tmp520=((struct _tuple14)(_tmp51E.ReachableFL).val).f2;_LL31A:
# 2120
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp51F,env->all_changed,_tmp519,_tmp51D),_tmp520);
# 2124
goto _LL318;_LL31B:;_LL31C: {
# 2126
const char*_tmp8A7;void*_tmp8A6;(_tmp8A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A7="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp8A7,sizeof(char),42))),_tag_dyneither(_tmp8A6,sizeof(void*),0)));}_LL318:;}
# 2128
goto _LL313;};}_LL316:;_LL317:
# 2133
 goto _LL313;_LL313:;};}}}}else{
# 2137
_tmp50E=_tmp509;goto _LL310;}
goto _LL30C;_LL30F: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp50D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp507.f1;if(_tmp50D->tag != 0)goto _LL311;else{_tmp50E=_tmp50D->f1;}}_LL310:
# 2140
 if(!_tmp4FD){
const char*_tmp8AB;void*_tmp8AA[1];struct Cyc_String_pa_PrintArg_struct _tmp8A9;(_tmp8A9.tag=0,((_tmp8A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2143
Cyc_Absynpp_qvar2string(_tmp50E->name)),((_tmp8AA[0]=& _tmp8A9,Cyc_CfFlowInfo_aerr(loc,((_tmp8AB="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8AB,sizeof(char),85))),_tag_dyneither(_tmp8AA,sizeof(void*),1)))))));}
goto _LL30C;_LL311:;_LL312: {
# 2146
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8AE="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8AE,sizeof(char),40))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}_LL30C:;}
# 2148
goto _LL307;}_LL30A: if((_tmp502.BottomFL).tag != 1)goto _LL307;_LL30B: {
# 2150
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8B1="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp8B1,sizeof(char),38))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}_LL307:;}}
# 2153
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2156
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2159
struct Cyc_CfFlowInfo_FlowEnv*_tmp52D=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp73=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp52F;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Stmt*_tmp531;unsigned int _tmp532;struct Cyc_Absyn_Switch_clause*_tmp52E=_stmttmp73;_tmp52F=_tmp52E->pat_vars;_tmp530=_tmp52E->where_clause;_tmp531=_tmp52E->body;_tmp532=_tmp52E->loc;{
struct _tuple1 _stmttmp74=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp52F))->v);struct Cyc_List_List*_tmp534;struct Cyc_List_List*_tmp535;struct _tuple1 _tmp533=_stmttmp74;_tmp534=_tmp533.f1;_tmp535=_tmp533.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp52F->v,_tmp534 != 0,_tmp532,exp_loc);
# 2167
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp536=env->unique_pat_vars;
{struct _tuple27*_tmp8B4;struct Cyc_List_List*_tmp8B3;env->unique_pat_vars=(
(_tmp8B3=_region_malloc(env->r,sizeof(*_tmp8B3)),((_tmp8B3->hd=((_tmp8B4=_region_malloc(env->r,sizeof(*_tmp8B4)),((_tmp8B4->f1=_tmp531,((_tmp8B4->f2=_tmp534,((_tmp8B4->f3=_tmp535,_tmp8B4)))))))),((_tmp8B3->tl=_tmp536,_tmp8B3))))));}
# 2172
if(_tmp530 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp530;
struct _tuple18 _stmttmp75=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp53A;union Cyc_CfFlowInfo_FlowInfo _tmp53B;struct _tuple18 _tmp539=_stmttmp75;_tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;
inflow=_tmp53B;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53A,_tmp531,0);}else{
# 2178
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp531,0);}
# 2180
env->unique_pat_vars=_tmp536;{
union Cyc_CfFlowInfo_FlowInfo _tmp53C=clause_outflow;_LL31E: if((_tmp53C.BottomFL).tag != 1)goto _LL320;_LL31F:
 goto _LL31D;_LL320:;_LL321:
# 2185
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp534,_tmp535,clause_outflow,_tmp532);
# 2187
if(scs->tl == 0)
return clause_outflow;else{
# 2192
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,Cyc_CfFlowInfo_aerr(_tmp531->loc,((_tmp8B7="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B7,sizeof(char),38))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}else{
# 2195
const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,Cyc_Tcutil_warn(_tmp531->loc,((_tmp8BA="switch clause may implicitly fallthru",_tag_dyneither(_tmp8BA,sizeof(char),38))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
# 2197
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2199
goto _LL31D;_LL31D:;};};};}
# 2202
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2207
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2209
struct Cyc_NewControlFlow_AnalEnv*_tmp542;struct Cyc_Dict_Dict _tmp543;unsigned int _tmp544;struct _tuple28*_tmp541=vdenv;_tmp542=_tmp541->f1;_tmp543=_tmp541->f2;_tmp544=_tmp541->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp542->fenv)->r,vd->type)){
# 2212
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8BB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8BB.tag=0,((_tmp8BB.f1=vd,_tmp8BB)));
# 2214
void*_tmp545=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp543,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp546=_tmp545;_LL323: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp547=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp547->tag != 8)goto _LL325;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp548=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp547->f2);if(_tmp548->tag != 7)goto _LL325;}}_LL324:
 goto _LL326;_LL325: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp549=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp549->tag != 8)goto _LL327;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp54A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp549->f2);if(_tmp54A->tag != 0)goto _LL327;}}_LL326:
 goto _LL328;_LL327: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp54B->tag != 8)goto _LL329;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp54C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp54B->f2);if(_tmp54C->tag != 3)goto _LL329;else{if(_tmp54C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL329;}}}_LL328:
 goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp54D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp54D->tag != 7)goto _LL32B;}_LL32A:
 goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp54E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp54E->tag != 0)goto _LL32D;}_LL32C:
 goto _LL32E;_LL32D: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp54F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp546;if(_tmp54F->tag != 3)goto _LL32F;else{if(_tmp54F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32F;}}_LL32E:
 goto _LL322;_LL32F:;_LL330:
# 2223
{const char*_tmp8BF;void*_tmp8BE[1];struct Cyc_String_pa_PrintArg_struct _tmp8BD;(_tmp8BD.tag=0,((_tmp8BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8BE[0]=& _tmp8BD,Cyc_Tcutil_warn(_tmp544,((_tmp8BF="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8BF,sizeof(char),68))),_tag_dyneither(_tmp8BE,sizeof(void*),1)))))));}
# 2225
goto _LL322;_LL322:;}}
# 2230
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2232
{union Cyc_CfFlowInfo_FlowInfo _tmp554=inflow;struct Cyc_Dict_Dict _tmp555;_LL332: if((_tmp554.ReachableFL).tag != 2)goto _LL334;_tmp555=((struct _tuple14)(_tmp554.ReachableFL).val).f1;_LL333: {
# 2234
struct _tuple28 _tmp8C0;struct _tuple28 _tmp556=(_tmp8C0.f1=env,((_tmp8C0.f2=_tmp555,((_tmp8C0.f3=decl->loc,_tmp8C0)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp557=env->fenv;
{void*_stmttmp76=decl->r;void*_tmp558=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp55A;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*_tmp55E;_LL337: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp559=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp558;if(_tmp559->tag != 0)goto _LL339;else{_tmp55A=_tmp559->f1;}}_LL338:
# 2238
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp556,_tmp55A);
goto _LL336;_LL339: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp55B=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp558;if(_tmp55B->tag != 2)goto _LL33B;else{if(_tmp55B->f2 == 0)goto _LL33B;_tmp55C=(struct Cyc_List_List*)(_tmp55B->f2)->v;}}_LL33A: {
# 2241
struct _tuple1 _stmttmp77=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp55C);struct Cyc_List_List*_tmp560;struct _tuple1 _tmp55F=_stmttmp77;_tmp560=_tmp55F.f1;{
struct Cyc_List_List*_tmp561=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp560);
_tmp55E=_tmp561;goto _LL33C;};}_LL33B: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp55D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp558;if(_tmp55D->tag != 3)goto _LL33D;else{_tmp55E=_tmp55D->f1;}}_LL33C:
# 2245
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp556,_tmp55E);
goto _LL336;_LL33D:;_LL33E:
 goto _LL336;_LL336:;}
# 2249
goto _LL331;}_LL334:;_LL335:
 goto _LL331;_LL331:;}
# 2252
return;}
# 2258
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2261
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_stmttmp78=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp564;struct Cyc_List_List*_tmp565;struct Cyc_List_List*_tmp566;struct _tuple27*_tmp563=_stmttmp78;_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;_tmp566=_tmp563->f3;
keep_going=0;{
# 2269
struct Cyc_Absyn_Stmt*_tmp567=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp567 != _tmp564){
struct Cyc_Absyn_Stmt*_tmp568=(Cyc_NewControlFlow_get_stmt_annot(_tmp567))->encloser;
if(_tmp568 == _tmp567){
# 2275
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp565,_tmp566,inflow,dest->loc);
keep_going=1;
break;}
# 2279
_tmp567=_tmp568;}};}}
# 2282
return inflow;}
# 2288
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2291
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _stmttmp79=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp56A;union Cyc_CfFlowInfo_FlowInfo*_tmp56B;struct _tuple19 _tmp569=_stmttmp79;_tmp56A=_tmp569.f1;_tmp56B=_tmp569.f2;
inflow=*_tmp56B;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp56C=env->fenv;
# 2298
void*_stmttmp7A=s->r;void*_tmp56D=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Stmt*_tmp576;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Stmt*_tmp57A;struct Cyc_Absyn_Stmt*_tmp57B;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Stmt*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Stmt*_tmp581;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Stmt*_tmp583;struct Cyc_Absyn_Exp*_tmp585;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Stmt*_tmp587;struct Cyc_Absyn_Exp*_tmp588;struct Cyc_Absyn_Stmt*_tmp589;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_List_List*_tmp58C;struct Cyc_Absyn_Switch_clause*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58F;struct Cyc_Absyn_Stmt*_tmp591;struct Cyc_Absyn_Stmt*_tmp593;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_List_List*_tmp596;struct Cyc_Absyn_Stmt*_tmp598;struct Cyc_List_List*_tmp599;struct Cyc_List_List*_tmp59C;struct Cyc_Absyn_Exp*_tmp59D;unsigned int _tmp59E;struct Cyc_Absyn_Stmt*_tmp59F;struct Cyc_Absyn_Tvar*_tmp5A2;struct Cyc_Absyn_Vardecl*_tmp5A3;struct Cyc_Absyn_Stmt*_tmp5A4;struct Cyc_Absyn_Decl*_tmp5A6;struct Cyc_Absyn_Stmt*_tmp5A7;struct Cyc_Absyn_Stmt*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AB;_LL340: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp56E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp56E->tag != 0)goto _LL342;}_LL341:
 return inflow;_LL342: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp56F=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp56F->tag != 6)goto _LL344;else{if(_tmp56F->f1 != 0)goto _LL344;}}_LL343:
# 2304
{union Cyc_CfFlowInfo_FlowInfo _tmp5AC=inflow;_LL36D: if((_tmp5AC.ReachableFL).tag != 2)goto _LL36F;_LL36E:
# 2306
{void*_stmttmp8F=Cyc_Tcutil_compress(env->return_type);void*_tmp5AD=_stmttmp8F;_LL372: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5AD;if(_tmp5AE->tag != 0)goto _LL374;}_LL373:
 goto _LL371;_LL374: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AD;if(_tmp5AF->tag != 7)goto _LL376;}_LL375:
 goto _LL377;_LL376: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AD;if(_tmp5B0->tag != 6)goto _LL378;}_LL377:
# 2310
{const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_Tcutil_warn(s->loc,((_tmp8C3="break may cause function not to return a value",_tag_dyneither(_tmp8C3,sizeof(char),47))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}
goto _LL371;_LL378:;_LL379:
# 2313
{const char*_tmp8C6;void*_tmp8C5;(_tmp8C5=0,Cyc_Tcutil_terr(s->loc,((_tmp8C6="break may cause function not to return a value",_tag_dyneither(_tmp8C6,sizeof(char),47))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}
goto _LL371;_LL371:;}
# 2316
goto _LL36C;_LL36F:;_LL370:
 goto _LL36C;_LL36C:;}
# 2319
goto _LL345;_LL344: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp570=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp570->tag != 3)goto _LL346;else{if(_tmp570->f1 != 0)goto _LL346;}}_LL345:
# 2321
 if(env->noreturn){
const char*_tmp8C9;void*_tmp8C8;(_tmp8C8=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8C9="`noreturn' function might return",_tag_dyneither(_tmp8C9,sizeof(char),33))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL346: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp571=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp571->tag != 3)goto _LL348;else{_tmp572=_tmp571->f1;}}_LL347:
# 2327
 if(env->noreturn){
const char*_tmp8CC;void*_tmp8CB;(_tmp8CB=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8CC="`noreturn' function might return",_tag_dyneither(_tmp8CC,sizeof(char),33))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp572));union Cyc_CfFlowInfo_FlowInfo _tmp5BA;void*_tmp5BB;struct _tuple15 _tmp5B9=_stmttmp8E;_tmp5BA=_tmp5B9.f1;_tmp5BB=_tmp5B9.f2;
_tmp5BA=Cyc_NewControlFlow_use_Rval(env,_tmp572->loc,_tmp5BA,_tmp5BB);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5BA);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp5BA,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL348: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp573=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp573->tag != 1)goto _LL34A;else{_tmp574=_tmp573->f1;}}_LL349: {
# 2336
struct _tuple16*_tmp5BC=rval_opt;void**_tmp5BD;int _tmp5BE;_LL37B: if(_tmp5BC == 0)goto _LL37D;_tmp5BD=(void**)& _tmp5BC->f1;_tmp5BE=_tmp5BC->f2;_LL37C: {
# 2338
struct _tuple15 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,_tmp5BE,inflow,_tmp574);union Cyc_CfFlowInfo_FlowInfo _tmp5C0;void*_tmp5C1;struct _tuple15 _tmp5BF=_stmttmp8D;_tmp5C0=_tmp5BF.f1;_tmp5C1=_tmp5BF.f2;
*_tmp5BD=_tmp5C1;
return _tmp5C0;}_LL37D: if(_tmp5BC != 0)goto _LL37A;_LL37E:
# 2342
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp574)).f1;_LL37A:;}_LL34A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp575=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp575->tag != 2)goto _LL34C;else{_tmp576=_tmp575->f1;_tmp577=_tmp575->f2;}}_LL34B:
# 2346
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp576,0),_tmp577,rval_opt);_LL34C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp578=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp578->tag != 4)goto _LL34E;else{_tmp579=_tmp578->f1;_tmp57A=_tmp578->f2;_tmp57B=_tmp578->f3;}}_LL34D: {
# 2349
struct _tuple18 _stmttmp8C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp579);union Cyc_CfFlowInfo_FlowInfo _tmp5C3;union Cyc_CfFlowInfo_FlowInfo _tmp5C4;struct _tuple18 _tmp5C2=_stmttmp8C;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;{
# 2356
union Cyc_CfFlowInfo_FlowInfo _tmp5C5=Cyc_NewControlFlow_anal_stmt(env,_tmp5C4,_tmp57B,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5C6=Cyc_NewControlFlow_anal_stmt(env,_tmp5C3,_tmp57A,0);
return Cyc_CfFlowInfo_join_flow(_tmp56C,env->all_changed,_tmp5C6,_tmp5C5);};}_LL34E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp57C=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp57C->tag != 5)goto _LL350;else{_tmp57D=(_tmp57C->f1).f1;_tmp57E=(_tmp57C->f1).f2;_tmp57F=_tmp57C->f2;}}_LL34F: {
# 2364
struct _tuple19 _stmttmp8A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp57E);union Cyc_CfFlowInfo_FlowInfo*_tmp5C8;struct _tuple19 _tmp5C7=_stmttmp8A;_tmp5C8=_tmp5C7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=*_tmp5C8;
struct _tuple18 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,_tmp5C9,_tmp57D);union Cyc_CfFlowInfo_FlowInfo _tmp5CB;union Cyc_CfFlowInfo_FlowInfo _tmp5CC;struct _tuple18 _tmp5CA=_stmttmp8B;_tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CD=Cyc_NewControlFlow_anal_stmt(env,_tmp5CB,_tmp57F,0);
Cyc_NewControlFlow_update_flow(env,_tmp57E,_tmp5CD);
return _tmp5CC;};};}_LL350: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp580=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp580->tag != 14)goto _LL352;else{_tmp581=_tmp580->f1;_tmp582=(_tmp580->f2).f1;_tmp583=(_tmp580->f2).f2;}}_LL351: {
# 2374
union Cyc_CfFlowInfo_FlowInfo _tmp5CE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp581,0);
struct _tuple19 _stmttmp88=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CE,_tmp583);union Cyc_CfFlowInfo_FlowInfo*_tmp5D0;struct _tuple19 _tmp5CF=_stmttmp88;_tmp5D0=_tmp5CF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D1=*_tmp5D0;
struct _tuple18 _stmttmp89=Cyc_NewControlFlow_anal_test(env,_tmp5D1,_tmp582);union Cyc_CfFlowInfo_FlowInfo _tmp5D3;union Cyc_CfFlowInfo_FlowInfo _tmp5D4;struct _tuple18 _tmp5D2=_stmttmp89;_tmp5D3=_tmp5D2.f1;_tmp5D4=_tmp5D2.f2;
Cyc_NewControlFlow_update_flow(env,_tmp581,_tmp5D3);
return _tmp5D4;};}_LL352: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp584=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp584->tag != 9)goto _LL354;else{_tmp585=_tmp584->f1;_tmp586=(_tmp584->f2).f1;_tmp587=(_tmp584->f2).f2;_tmp588=(_tmp584->f3).f1;_tmp589=(_tmp584->f3).f2;_tmp58A=_tmp584->f4;}}_LL353: {
# 2383
union Cyc_CfFlowInfo_FlowInfo _tmp5D5=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp585)).f1;
struct _tuple19 _stmttmp85=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5D5,_tmp587);union Cyc_CfFlowInfo_FlowInfo*_tmp5D7;struct _tuple19 _tmp5D6=_stmttmp85;_tmp5D7=_tmp5D6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D8=*_tmp5D7;
struct _tuple18 _stmttmp86=Cyc_NewControlFlow_anal_test(env,_tmp5D8,_tmp586);union Cyc_CfFlowInfo_FlowInfo _tmp5DA;union Cyc_CfFlowInfo_FlowInfo _tmp5DB;struct _tuple18 _tmp5D9=_stmttmp86;_tmp5DA=_tmp5D9.f1;_tmp5DB=_tmp5D9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DC=Cyc_NewControlFlow_anal_stmt(env,_tmp5DA,_tmp58A,0);
struct _tuple19 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5DC,_tmp589);union Cyc_CfFlowInfo_FlowInfo*_tmp5DE;struct _tuple19 _tmp5DD=_stmttmp87;_tmp5DE=_tmp5DD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DF=*_tmp5DE;
union Cyc_CfFlowInfo_FlowInfo _tmp5E0=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5DF,_tmp588)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp587,_tmp5E0);
return _tmp5DB;};};};}_LL354: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp58B->tag != 11)goto _LL356;else{_tmp58C=_tmp58B->f1;if(_tmp58B->f2 == 0)goto _LL356;_tmp58D=*_tmp58B->f2;}}_LL355: {
# 2395
struct _RegionHandle _tmp5E1=_new_region("temp");struct _RegionHandle*temp=& _tmp5E1;_push_region(temp);
{struct _tuple22 _stmttmp83=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp58C,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5E3;struct Cyc_List_List*_tmp5E4;struct _tuple22 _tmp5E2=_stmttmp83;_tmp5E3=_tmp5E2.f1;_tmp5E4=_tmp5E2.f2;
# 2398
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp58D->body);{
# 2400
struct Cyc_List_List*_tmp5E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp58D->pat_vars))->v)).f1);
_tmp5E3=Cyc_NewControlFlow_add_vars(_tmp56C,_tmp5E3,_tmp5E5,_tmp56C->unknown_all,s->loc,0);
# 2403
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp58D->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp84=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E8;struct _tuple23*_tmp5E6=_stmttmp84;_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;
if(_tmp5E7 != 0){
_tmp5E3=Cyc_NewControlFlow_do_initialize_var(_tmp56C,env,_tmp5E3,*_tmp5E7,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5E4))->hd,s->loc);
_tmp5E4=_tmp5E4->tl;
_tmp58C=_tmp58C->tl;}}}
# 2411
Cyc_NewControlFlow_update_flow(env,_tmp58D->body,_tmp5E3);{
union Cyc_CfFlowInfo_FlowInfo _tmp5E9=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5E9;};};}
# 2396
;_pop_region(temp);}_LL356: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp58E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp58E->tag != 6)goto _LL358;else{_tmp58F=_tmp58E->f1;}}_LL357:
# 2414
 _tmp591=_tmp58F;goto _LL359;_LL358: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp590=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp590->tag != 7)goto _LL35A;else{_tmp591=_tmp590->f1;}}_LL359:
 _tmp593=_tmp591;goto _LL35B;_LL35A: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp592=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp592->tag != 8)goto _LL35C;else{_tmp593=_tmp592->f2;}}_LL35B:
# 2419
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5EA=_tmp56A->encloser;
struct Cyc_Absyn_Stmt*_tmp5EB=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp593)))->encloser;
while(_tmp5EB != _tmp5EA){
struct Cyc_Absyn_Stmt*_tmp5EC=(Cyc_NewControlFlow_get_stmt_annot(_tmp5EA))->encloser;
if(_tmp5EC == _tmp5EA){
{const char*_tmp8CF;void*_tmp8CE;(_tmp8CE=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8CF="goto enters local scope or exception handler",_tag_dyneither(_tmp8CF,sizeof(char),45))),_tag_dyneither(_tmp8CE,sizeof(void*),0)));}
break;}
# 2428
_tmp5EA=_tmp5EC;}}
# 2432
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp593));
# 2434
Cyc_NewControlFlow_update_flow(env,_tmp593,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL35C: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp594=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp594->tag != 10)goto _LL35E;else{_tmp595=_tmp594->f1;_tmp596=_tmp594->f2;}}_LL35D:
# 2440
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp596,_tmp595->loc);_LL35E: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp597=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp597->tag != 15)goto _LL360;else{_tmp598=_tmp597->f1;_tmp599=_tmp597->f2;}}_LL35F: {
# 2445
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp598,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2453
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2456
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp599,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5EF=scs_outflow;_LL380: if((_tmp5EF.BottomFL).tag != 1)goto _LL382;_LL381:
 goto _LL37F;_LL382:;_LL383: {
const char*_tmp8D2;void*_tmp8D1;(_tmp8D1=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8D2="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8D2,sizeof(char),42))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}_LL37F:;}
# 2462
outflow=s1_outflow;
# 2464
return outflow;};};}_LL360: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp59A->tag != 12)goto _LL362;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp59B=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp59A->f1)->r;if(_tmp59B->tag != 2)goto _LL362;else{if(_tmp59B->f2 == 0)goto _LL362;_tmp59C=(struct Cyc_List_List*)(_tmp59B->f2)->v;_tmp59D=_tmp59B->f3;}}_tmp59E=(_tmp59A->f1)->loc;_tmp59F=_tmp59A->f2;}}_LL361: {
# 2474
struct _tuple1 _stmttmp82=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp59C);struct Cyc_List_List*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct _tuple1 _tmp5F2=_stmttmp82;_tmp5F3=_tmp5F2.f1;_tmp5F4=_tmp5F2.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp56C,env,inflow,_tmp59C,_tmp5F3 != 0,_tmp59E,_tmp59D->loc);{
struct Cyc_List_List*_tmp5F5=env->unique_pat_vars;
{struct _tuple27*_tmp8D5;struct Cyc_List_List*_tmp8D4;env->unique_pat_vars=(
(_tmp8D4=_region_malloc(env->r,sizeof(*_tmp8D4)),((_tmp8D4->hd=((_tmp8D5=_region_malloc(env->r,sizeof(*_tmp8D5)),((_tmp8D5->f1=_tmp59F,((_tmp8D5->f2=_tmp5F3,((_tmp8D5->f3=_tmp5F4,_tmp8D5)))))))),((_tmp8D4->tl=_tmp5F5,_tmp8D4))))));}
# 2482
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp59F,rval_opt);
env->unique_pat_vars=_tmp5F5;
# 2486
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5F3,_tmp5F4,inflow,_tmp59E);
# 2490
return inflow;};}_LL362: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5A0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp5A0->tag != 12)goto _LL364;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp5A1=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp5A0->f1)->r;if(_tmp5A1->tag != 5)goto _LL364;else{_tmp5A2=_tmp5A1->f1;_tmp5A3=_tmp5A1->f2;}}_tmp5A4=_tmp5A0->f2;}}_LL363: {
# 2494
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp5A3->initializer);
struct _tuple17 _stmttmp7D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp5F9;union Cyc_CfFlowInfo_AbsLVal _tmp5FA;struct _tuple17 _tmp5F8=_stmttmp7D;_tmp5F9=_tmp5F8.f1;_tmp5FA=_tmp5F8.f2;{
# 2499
struct _tuple17 _tmp8D6;struct _tuple17 _stmttmp7E=(_tmp8D6.f1=_tmp5F9,((_tmp8D6.f2=_tmp5FA,_tmp8D6)));struct _tuple17 _tmp5FB=_stmttmp7E;struct Cyc_Dict_Dict _tmp5FC;struct Cyc_List_List*_tmp5FD;struct Cyc_CfFlowInfo_Place*_tmp5FE;_LL385: if(((_tmp5FB.f1).ReachableFL).tag != 2)goto _LL387;_tmp5FC=((struct _tuple14)((_tmp5FB.f1).ReachableFL).val).f1;_tmp5FD=((struct _tuple14)((_tmp5FB.f1).ReachableFL).val).f2;if(((_tmp5FB.f2).PlaceL).tag != 1)goto _LL387;_tmp5FE=(struct Cyc_CfFlowInfo_Place*)((_tmp5FB.f2).PlaceL).val;_LL386: {
# 2501
void*_tmp5FF=Cyc_CfFlowInfo_lookup_place(_tmp5FC,_tmp5FE);
void*t=(void*)_check_null(e->topt);
void*_tmp600=Cyc_CfFlowInfo_make_unique_consumed(_tmp56C,t,e,env->iteration_num,_tmp5FF);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8D9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8D8;_tmp600=(void*)((_tmp8D8=_region_malloc(_tmp56C->r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=8,((_tmp8D9.f1=_tmp5A3,((_tmp8D9.f2=_tmp600,_tmp8D9)))))),_tmp8D8))));}
_tmp5FC=Cyc_CfFlowInfo_assign_place(_tmp56C,e->loc,_tmp5FC,env->all_changed,_tmp5FE,_tmp600);
_tmp5F9=Cyc_CfFlowInfo_ReachableFL(_tmp5FC,_tmp5FD);{
# 2514
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp7F=Cyc_CfFlowInfo_initlevel(_tmp56C,_tmp5FC,_tmp5FF);switch(_stmttmp7F){case Cyc_CfFlowInfo_AllIL: _LL38B:
 leaf=_tmp56C->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL38C:
 leaf=_tmp56C->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL38D:
 leaf=_tmp56C->unknown_this;break;}}{
# 2520
void*_tmp603=Cyc_CfFlowInfo_typ_to_absrval(_tmp56C,t,0,leaf);
_tmp5F9=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5F9,_tmp5FF);{
struct Cyc_List_List _tmp8DA;struct Cyc_List_List _tmp604=(_tmp8DA.hd=_tmp5A3,((_tmp8DA.tl=0,_tmp8DA)));
# 2524
_tmp5F9=Cyc_NewControlFlow_add_vars(_tmp56C,_tmp5F9,& _tmp604,_tmp56C->unknown_all,s->loc,0);
# 2528
_tmp5F9=Cyc_NewControlFlow_anal_stmt(env,_tmp5F9,_tmp5A4,rval_opt);{
# 2530
union Cyc_CfFlowInfo_FlowInfo _tmp605=_tmp5F9;struct Cyc_Dict_Dict _tmp606;struct Cyc_List_List*_tmp607;_LL390: if((_tmp605.ReachableFL).tag != 2)goto _LL392;_tmp606=((struct _tuple14)(_tmp605.ReachableFL).val).f1;_tmp607=((struct _tuple14)(_tmp605.ReachableFL).val).f2;_LL391:
# 2533
{void*_stmttmp80=Cyc_CfFlowInfo_lookup_place(_tmp606,_tmp5FE);void*_tmp608=_stmttmp80;struct Cyc_Absyn_Vardecl*_tmp60A;void*_tmp60B;_LL395: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp609=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp608;if(_tmp609->tag != 8)goto _LL397;else{_tmp60A=_tmp609->f1;_tmp60B=(void*)_tmp609->f2;}}_LL396:
# 2535
 if(_tmp60A != _tmp5A3)goto _LL398;{
# 2538
struct _tuple15 _tmp8DC;struct _tuple15 _tmp8DB;struct _tuple15 _stmttmp81=Cyc_CfFlowInfo_join_flow_and_rval(_tmp56C,env->all_changed,(
(_tmp8DB.f1=_tmp5F9,((_tmp8DB.f2=_tmp5FF,_tmp8DB)))),((_tmp8DC.f1=_tmp5F9,((_tmp8DC.f2=_tmp60B,_tmp8DC)))));
# 2538
void*_tmp60D;struct _tuple15 _tmp60C=_stmttmp81;_tmp60D=_tmp60C.f2;
# 2540
_tmp603=_tmp60D;
goto _LL394;};_LL397:;_LL398:
# 2543
{const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8DF="aliased location was overwritten",_tag_dyneither(_tmp8DF,sizeof(char),33))),_tag_dyneither(_tmp8DE,sizeof(void*),0)));}
goto _LL394;_LL394:;}
# 2546
_tmp606=Cyc_CfFlowInfo_assign_place(_tmp56C,s->loc,_tmp606,env->all_changed,_tmp5FE,_tmp603);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp606,_tmp607);
# 2551
return outflow;_LL392:;_LL393:
 return _tmp5F9;_LL38F:;};};};};}_LL387: if(((_tmp5FB.f1).BottomFL).tag != 1)goto _LL389;_LL388:
# 2554
 return _tmp5F9;_LL389:;_LL38A:
{const char*_tmp8E2;void*_tmp8E1;(_tmp8E1=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8E2="bad alias expression--no unique path found",_tag_dyneither(_tmp8E2,sizeof(char),43))),_tag_dyneither(_tmp8E1,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL384:;};}_LL364: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5A5=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp5A5->tag != 12)goto _LL366;else{_tmp5A6=_tmp5A5->f1;_tmp5A7=_tmp5A5->f2;}}_LL365:
# 2560
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5A6),_tmp5A7,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5A6);
return outflow;_LL366: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5A8=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp5A8->tag != 13)goto _LL368;else{_tmp5A9=_tmp5A8->f2;}}_LL367:
# 2565
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A9,rval_opt);_LL368: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5AA=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp56D;if(_tmp5AA->tag != 16)goto _LL36A;else{_tmp5AB=_tmp5AA->f1;}}_LL369: {
# 2568
struct _tuple15 _stmttmp7B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5AB);union Cyc_CfFlowInfo_FlowInfo _tmp617;void*_tmp618;struct _tuple15 _tmp616=_stmttmp7B;_tmp617=_tmp616.f1;_tmp618=_tmp616.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp619=Cyc_NewControlFlow_use_Rval(env,_tmp5AB->loc,_tmp617,_tmp618);
void*_stmttmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp5AB->topt));void*_tmp61A=_stmttmp7C;void*_tmp61C;_LL39A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp61B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp61A;if(_tmp61B->tag != 15)goto _LL39C;else{_tmp61C=(void*)_tmp61B->f1;}}_LL39B:
# 2572
 return Cyc_CfFlowInfo_kill_flow_region(_tmp56C,_tmp617,_tmp61C);_LL39C:;_LL39D: {
struct Cyc_Core_Impossible_exn_struct _tmp8E8;const char*_tmp8E7;struct Cyc_Core_Impossible_exn_struct*_tmp8E6;(int)_throw((void*)((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E8.tag=Cyc_Core_Impossible,((_tmp8E8.f1=((_tmp8E7="anal_stmt -- reset_region",_tag_dyneither(_tmp8E7,sizeof(char),26))),_tmp8E8)))),_tmp8E6)))));}_LL399:;};}_LL36A:;_LL36B: {
# 2576
struct Cyc_Core_Impossible_exn_struct _tmp8EE;const char*_tmp8ED;struct Cyc_Core_Impossible_exn_struct*_tmp8EC;(int)_throw((void*)((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8EE.tag=Cyc_Core_Impossible,((_tmp8EE.f1=((_tmp8ED="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8ED,sizeof(char),56))),_tmp8EE)))),_tmp8EC)))));}_LL33F:;};}
# 2581
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2584
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp623=env->fenv;
void*_stmttmp90=decl->r;void*_tmp624=_stmttmp90;struct Cyc_Absyn_Vardecl*_tmp626;struct Cyc_List_List*_tmp628;struct Cyc_Absyn_Fndecl*_tmp62A;struct Cyc_Absyn_Tvar*_tmp62C;struct Cyc_Absyn_Vardecl*_tmp62D;int _tmp62E;struct Cyc_Absyn_Exp*_tmp62F;_LL39F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp625=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp624;if(_tmp625->tag != 0)goto _LL3A1;else{_tmp626=_tmp625->f1;}}_LL3A0: {
# 2594
struct Cyc_List_List _tmp8EF;struct Cyc_List_List _tmp630=(_tmp8EF.hd=_tmp626,((_tmp8EF.tl=0,_tmp8EF)));
inflow=Cyc_NewControlFlow_add_vars(_tmp623,inflow,& _tmp630,_tmp623->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp631=_tmp626->initializer;
if(_tmp631 == 0)
return inflow;{
struct _tuple15 _stmttmp92=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp631);union Cyc_CfFlowInfo_FlowInfo _tmp633;void*_tmp634;struct _tuple15 _tmp632=_stmttmp92;_tmp633=_tmp632.f1;_tmp634=_tmp632.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp623,env,_tmp633,_tmp626,_tmp631,_tmp634,decl->loc);};};}_LL3A1: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp627=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp624;if(_tmp627->tag != 3)goto _LL3A3;else{_tmp628=_tmp627->f1;}}_LL3A2:
# 2603
 return Cyc_NewControlFlow_add_vars(_tmp623,inflow,_tmp628,_tmp623->unknown_none,decl->loc,0);_LL3A3: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp629=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp624;if(_tmp629->tag != 1)goto _LL3A5;else{_tmp62A=_tmp629->f1;}}_LL3A4:
# 2606
 Cyc_NewControlFlow_check_nested_fun(_tmp623,inflow,_tmp62A);{
void*t=(void*)_check_null(_tmp62A->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp636=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp62A->fn_vardecl);
# 2612
struct Cyc_List_List*_tmp8F0;return Cyc_NewControlFlow_add_vars(_tmp623,inflow,((_tmp8F0=_region_malloc(env->r,sizeof(*_tmp8F0)),((_tmp8F0->hd=_tmp636,((_tmp8F0->tl=0,_tmp8F0)))))),_tmp623->unknown_all,decl->loc,0);};_LL3A5: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp62B=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp624;if(_tmp62B->tag != 4)goto _LL3A7;else{_tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;_tmp62E=_tmp62B->f3;_tmp62F=_tmp62B->f4;}}_LL3A6:
# 2615
 if(_tmp62F != 0){
struct Cyc_Absyn_Exp*_tmp638=_tmp62F;
struct _tuple15 _stmttmp91=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp638);union Cyc_CfFlowInfo_FlowInfo _tmp63A;void*_tmp63B;struct _tuple15 _tmp639=_stmttmp91;_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp638->loc,_tmp63A,_tmp63B);}{
# 2620
struct Cyc_List_List _tmp8F1;struct Cyc_List_List _tmp63C=(_tmp8F1.hd=_tmp62D,((_tmp8F1.tl=0,_tmp8F1)));
return Cyc_NewControlFlow_add_vars(_tmp623,inflow,& _tmp63C,_tmp623->unknown_all,decl->loc,0);};_LL3A7:;_LL3A8: {
# 2624
struct Cyc_Core_Impossible_exn_struct _tmp8F7;const char*_tmp8F6;struct Cyc_Core_Impossible_exn_struct*_tmp8F5;(int)_throw((void*)((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F7.tag=Cyc_Core_Impossible,((_tmp8F7.f1=((_tmp8F6="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8F6,sizeof(char),35))),_tmp8F7)))),_tmp8F5)))));}_LL39E:;}
# 2632
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp641=_new_region("frgn");struct _RegionHandle*frgn=& _tmp641;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2634
;_pop_region(frgn);}
# 2639
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2643
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2647
struct _RegionHandle*_tmp642=fenv->r;
unsigned int _tmp643=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp643,1);{
# 2653
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp644=inflow;struct Cyc_Dict_Dict _tmp645;struct Cyc_List_List*_tmp646;_LL3AA: if((_tmp644.BottomFL).tag != 1)goto _LL3AC;_LL3AB: {
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8FA="check_fun",_tag_dyneither(_tmp8FA,sizeof(char),10))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}_LL3AC: if((_tmp644.ReachableFL).tag != 2)goto _LL3A9;_tmp645=((struct _tuple14)(_tmp644.ReachableFL).val).f1;_tmp646=((struct _tuple14)(_tmp644.ReachableFL).val).f2;_LL3AD: {
# 2660
struct Cyc_List_List*atts;
{void*_stmttmp93=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp649=_stmttmp93;struct Cyc_List_List*_tmp64B;_LL3AF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp64A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp649;if(_tmp64A->tag != 9)goto _LL3B1;else{_tmp64B=(_tmp64A->f1).attributes;}}_LL3B0:
 atts=_tmp64B;goto _LL3AE;_LL3B1:;_LL3B2: {
const char*_tmp8FD;void*_tmp8FC;(_tmp8FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8FD="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8FD,sizeof(char),50))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}_LL3AE:;}
# 2667
for(0;atts != 0;atts=atts->tl){
void*_stmttmp94=(void*)atts->hd;void*_tmp64E=_stmttmp94;int _tmp650;int _tmp652;int _tmp654;_LL3B4: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp64F=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp64E;if(_tmp64F->tag != 21)goto _LL3B6;else{_tmp650=_tmp64F->f1;}}_LL3B5: {
# 2670
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp655=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp650 - 1);
void*t=Cyc_Tcutil_compress(_tmp655->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp656=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp900;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8FF;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp657=(_tmp8FF=_region_malloc(_tmp642,sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=2,((_tmp900.f1=_tmp650,((_tmp900.f2=t,_tmp900)))))),_tmp8FF)));
struct Cyc_CfFlowInfo_Place*_tmp901;struct Cyc_CfFlowInfo_Place*_tmp658=(_tmp901=_region_malloc(_tmp642,sizeof(*_tmp901)),((_tmp901->root=(void*)_tmp657,((_tmp901->fields=0,_tmp901)))));
_tmp645=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp645,(void*)_tmp657,_tmp656);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp907;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp906;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp904;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp903;_tmp645=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp645,(void*)((_tmp903=_region_malloc(_tmp642,sizeof(*_tmp903)),((_tmp903[0]=((_tmp904.tag=0,((_tmp904.f1=_tmp655,_tmp904)))),_tmp903)))),(void*)((_tmp906=_region_malloc(_tmp642,sizeof(*_tmp906)),((_tmp906[0]=((_tmp907.tag=5,((_tmp907.f1=_tmp658,_tmp907)))),_tmp906)))));}
goto _LL3B3;}_LL3B6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp651=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp64E;if(_tmp651->tag != 20)goto _LL3B8;else{_tmp652=_tmp651->f1;}}_LL3B7: {
# 2683
struct Cyc_Absyn_Vardecl*_tmp660=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp652 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp660->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp90A;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp909;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp661=(_tmp909=_region_malloc(_tmp642,sizeof(*_tmp909)),((_tmp909[0]=((_tmp90A.tag=2,((_tmp90A.f1=_tmp652,((_tmp90A.f2=elttype,_tmp90A)))))),_tmp909)));
struct Cyc_CfFlowInfo_Place*_tmp90B;struct Cyc_CfFlowInfo_Place*_tmp662=(_tmp90B=_region_malloc(_tmp642,sizeof(*_tmp90B)),((_tmp90B->root=(void*)_tmp661,((_tmp90B->fields=0,_tmp90B)))));
_tmp645=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp645,(void*)_tmp661,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp911;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp910;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp90E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp90D;_tmp645=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp645,(void*)((_tmp90D=_region_malloc(_tmp642,sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=0,((_tmp90E.f1=_tmp660,_tmp90E)))),_tmp90D)))),(void*)((_tmp910=_region_malloc(_tmp642,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=5,((_tmp911.f1=_tmp662,_tmp911)))),_tmp910)))));}
{struct Cyc_List_List*_tmp912;param_roots=((_tmp912=_region_malloc(_tmp642,sizeof(*_tmp912)),((_tmp912->hd=_tmp662,((_tmp912->tl=param_roots,_tmp912))))));}
goto _LL3B3;}_LL3B8: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp653=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp64E;if(_tmp653->tag != 22)goto _LL3BA;else{_tmp654=_tmp653->f1;}}_LL3B9: {
# 2692
struct Cyc_Absyn_Vardecl*_tmp66B=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp654 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp66B->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp915;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp914;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66C=(_tmp914=_region_malloc(_tmp642,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=0,((_tmp915.f1=_tmp66B,_tmp915)))),_tmp914)));
struct Cyc_CfFlowInfo_Place*_tmp916;struct Cyc_CfFlowInfo_Place*_tmp66D=(_tmp916=_region_malloc(_tmp642,sizeof(*_tmp916)),((_tmp916->root=(void*)_tmp66C,((_tmp916->fields=0,_tmp916)))));
struct Cyc_List_List*_tmp917;noconsume_roots=((_tmp917=_region_malloc(_tmp642,sizeof(*_tmp917)),((_tmp917->hd=_tmp66D,((_tmp917->tl=noconsume_roots,_tmp917))))));}
# 2698
goto _LL3B3;}_LL3BA:;_LL3BB:
 goto _LL3B3;_LL3B3:;}
# 2701
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp645,_tmp646);}_LL3A9:;}{
# 2704
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp642,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp918;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp918=_region_malloc(_tmp642,sizeof(*_tmp918)),((_tmp918->r=_tmp642,((_tmp918->fenv=fenv,((_tmp918->iterate_again=1,((_tmp918->iteration_num=0,((_tmp918->in_try=0,((_tmp918->tryflow=inflow,((_tmp918->all_changed=0,((_tmp918->noreturn=noreturn,((_tmp918->return_type=fd->ret_type,((_tmp918->unique_pat_vars=0,((_tmp918->param_roots=param_roots,((_tmp918->noconsume_params=noconsume_roots,((_tmp918->flow_table=flow_table,_tmp918)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2714
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2717
union Cyc_CfFlowInfo_FlowInfo _tmp672=outflow;_LL3BD: if((_tmp672.BottomFL).tag != 1)goto _LL3BF;_LL3BE:
 goto _LL3BC;_LL3BF:;_LL3C0:
# 2720
 Cyc_NewControlFlow_check_init_params(_tmp643,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp643);
# 2724
if(noreturn){
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_CfFlowInfo_aerr(_tmp643,((_tmp91B="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp91B,sizeof(char),46))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}else{
# 2727
void*_stmttmp95=Cyc_Tcutil_compress(fd->ret_type);void*_tmp675=_stmttmp95;_LL3C2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp676=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp675;if(_tmp676->tag != 0)goto _LL3C4;}_LL3C3:
 goto _LL3C1;_LL3C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp675;if(_tmp677->tag != 7)goto _LL3C6;}_LL3C5:
 goto _LL3C7;_LL3C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp678=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp675;if(_tmp678->tag != 6)goto _LL3C8;}_LL3C7:
# 2731
{const char*_tmp91E;void*_tmp91D;(_tmp91D=0,Cyc_Tcutil_warn(_tmp643,((_tmp91E="function may not return a value",_tag_dyneither(_tmp91E,sizeof(char),32))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}goto _LL3C1;_LL3C8:;_LL3C9:
# 2733
{const char*_tmp921;void*_tmp920;(_tmp920=0,Cyc_CfFlowInfo_aerr(_tmp643,((_tmp921="function may not return a value",_tag_dyneither(_tmp921,sizeof(char),32))),_tag_dyneither(_tmp920,sizeof(void*),0)));}goto _LL3C1;_LL3C1:;}
# 2735
goto _LL3BC;_LL3BC:;};};};}
# 2739
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_stmttmp96=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp67E=_stmttmp96;struct Cyc_Absyn_Fndecl*_tmp680;struct Cyc_List_List*_tmp682;struct Cyc_List_List*_tmp684;struct Cyc_List_List*_tmp686;_LL3CB: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp67F=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp67E;if(_tmp67F->tag != 1)goto _LL3CD;else{_tmp680=_tmp67F->f1;}}_LL3CC:
 Cyc_NewControlFlow_check_fun(_tmp680);goto _LL3CA;_LL3CD: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp681=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp67E;if(_tmp681->tag != 12)goto _LL3CF;else{_tmp682=_tmp681->f1;}}_LL3CE:
 _tmp684=_tmp682;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp683=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp67E;if(_tmp683->tag != 11)goto _LL3D1;else{_tmp684=_tmp683->f2;}}_LL3D0:
 _tmp686=_tmp684;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp685=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp67E;if(_tmp685->tag != 10)goto _LL3D3;else{_tmp686=_tmp685->f2;}}_LL3D2:
 Cyc_NewControlFlow_cf_check(_tmp686);goto _LL3CA;_LL3D3: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp687=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp67E;if(_tmp687->tag != 13)goto _LL3D5;}_LL3D4:
 goto _LL3CA;_LL3D5:;_LL3D6:
 goto _LL3CA;_LL3CA:;};}}
