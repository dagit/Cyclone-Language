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
# 925 "absyn.h"
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 970
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
# 105
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 178
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 251 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 283
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 344
int Cyc_Tcutil_is_noreturn(void*);
# 362
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 366
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 369
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);struct _tuple12{unsigned int f1;int f2;};
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
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 50 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 61
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 63
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 68
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
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
# 123
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
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
# 167
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 173
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 176
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 178
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 200 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 204
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 209
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple15 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple15 pr1,struct _tuple15 pr2);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 220
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 229
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
typedef struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_vardecl_opt_t;
# 67
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 75
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 80
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp690;struct Cyc_NewControlFlow_CFStmtAnnot _tmp68F;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp68E;enclosee->annot=(void*)((_tmp68E=_cycalloc(sizeof(*_tmp68E)),((_tmp68E[0]=((_tmp690.tag=Cyc_NewControlFlow_CFAnnot,((_tmp690.f1=((_tmp68F.encloser=encloser,((_tmp68F.visited=0,_tmp68F)))),_tmp690)))),_tmp68E))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp696;const char*_tmp695;struct Cyc_Core_Impossible_exn_struct*_tmp694;(int)_throw((void*)((_tmp694=_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp696.tag=Cyc_Core_Impossible,((_tmp696.f1=((_tmp695="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp695,sizeof(char),37))),_tmp696)))),_tmp694)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp697;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp697=_region_malloc(env->r,sizeof(*_tmp697)),((_tmp697[0]=Cyc_CfFlowInfo_BottomFL(),_tmp697)));
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
struct _tuple19 _tmp698;return(_tmp698.f1=_tmpA,((_tmp698.f2=_tmpB,_tmp698)));};}
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
{const char*_tmp69C;void*_tmp69B[1];struct Cyc_String_pa_PrintArg_struct _tmp69A;(_tmp69A.tag=0,((_tmp69A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp69B[0]=& _tmp69A,Cyc_Tcutil_warn(_tmpF,((_tmp69C="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp69C,sizeof(char),60))),_tag_dyneither(_tmp69B,sizeof(void*),1)))))));}
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
struct _tuple18 _tmp69D;struct _tuple18 _stmttmp1=(_tmp69D.f1=flow,((_tmp69D.f2=_tmp1F,_tmp69D)));struct _tuple18 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp69E;struct _tuple20 _tmp23=(_tmp69E.f1=env,((_tmp69E.f2=s->loc,((_tmp69E.f3=_tmp22,_tmp69E)))));
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6A1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6A0;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp6A0=_region_malloc(fenv->r,sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=0,((_tmp6A1.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp6A1)))),_tmp6A0)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A4;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6A3;_tmp2A=(void*)((_tmp6A3=_region_malloc(fenv->r,sizeof(*_tmp6A3)),((_tmp6A3[0]=((_tmp6A4.tag=8,((_tmp6A4.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp6A4.f2=_tmp2A,_tmp6A4)))))),_tmp6A3))));}
# 259
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp6A5;if(Cyc_Relations_consistent_relations(((_tmp6A5=_region_malloc(r,sizeof(*_tmp6A5)),((_tmp6A5->hd=_tmp2F,((_tmp6A5->tl=assume,_tmp6A5))))))))
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
{const char*_tmp6AA;void*_tmp6A9[2];struct Cyc_String_pa_PrintArg_struct _tmp6A8;struct Cyc_String_pa_PrintArg_struct _tmp6A7;(_tmp6A7.tag=0,((_tmp6A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6A9[0]=& _tmp6A8,((_tmp6A9[1]=& _tmp6A7,Cyc_Tcutil_terr(loc,((_tmp6AA="unable to prove %s, required to access %s",_tag_dyneither(_tmp6AA,sizeof(char),42))),_tag_dyneither(_tmp6A9,sizeof(void*),2)))))))))))));}
{const char*_tmp6AD;void*_tmp6AC;(_tmp6AC=0,Cyc_fprintf(Cyc_stderr,((_tmp6AD="  [recorded facts on path: ",_tag_dyneither(_tmp6AD,sizeof(char),28))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B0;void*_tmp6AF;(_tmp6AF=0,Cyc_fprintf(Cyc_stderr,((_tmp6B0="]\n",_tag_dyneither(_tmp6B0,sizeof(char),3))),_tag_dyneither(_tmp6AF,sizeof(void*),0)));};}}
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
{const char*_tmp6B5;void*_tmp6B4[2];struct Cyc_String_pa_PrintArg_struct _tmp6B3;struct Cyc_String_pa_PrintArg_struct _tmp6B2;(_tmp6B2.tag=0,((_tmp6B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6B4[0]=& _tmp6B3,((_tmp6B4[1]=& _tmp6B2,Cyc_Tcutil_terr(loc,((_tmp6B5="unable to prove %s, required to access %s",_tag_dyneither(_tmp6B5,sizeof(char),42))),_tag_dyneither(_tmp6B4,sizeof(void*),2)))))))))))));}
{const char*_tmp6B8;void*_tmp6B7;(_tmp6B7=0,Cyc_fprintf(Cyc_stderr,((_tmp6B8="  [recorded facts on path: ",_tag_dyneither(_tmp6B8,sizeof(char),28))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6BB;void*_tmp6BA;(_tmp6BA=0,Cyc_fprintf(Cyc_stderr,((_tmp6BB="]\n",_tag_dyneither(_tmp6BB,sizeof(char),3))),_tag_dyneither(_tmp6BA,sizeof(void*),0)));};}
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
const char*_tmp6BE;void*_tmp6BD;(_tmp6BD=0,Cyc_Tcutil_terr(loc,((_tmp6BE="expression may not be fully initialized",_tag_dyneither(_tmp6BE,sizeof(char),40))),_tag_dyneither(_tmp6BD,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6BF;struct _tuple0 _stmttmp4=(_tmp6BF.f1=Cyc_Tcutil_compress(t),((_tmp6BF.f2=r,_tmp6BF)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
{int i=0;for(0;i < sz;++ i){
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
{const char*_tmp6C2;void*_tmp6C1;(_tmp6C1=0,Cyc_Tcutil_warn(loc,((_tmp6C2="argument may still contain unique pointers",_tag_dyneither(_tmp6C2,sizeof(char),43))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6C5;void*_tmp6C4;(_tmp6C4=0,Cyc_Tcutil_terr(loc,((_tmp6C5="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6C5,sizeof(char),47))),_tag_dyneither(_tmp6C4,sizeof(void*),0)));}
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
const char*_tmp6C8;void*_tmp6C7;(_tmp6C7=0,Cyc_Tcutil_warn(loc,((_tmp6C8="argument may contain live unique pointers",_tag_dyneither(_tmp6C8,sizeof(char),42))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));}
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
struct _tuple22 _tmp6C9;return(_tmp6C9.f1=inflow,((_tmp6C9.f2=0,_tmp6C9)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6CC;struct _tuple22 _tmp6CB;return(_tmp6CB.f1=_tmp8C,((_tmp6CB.f2=((_tmp6CC=_region_malloc(rgn,sizeof(*_tmp6CC)),((_tmp6CC->hd=_tmp8D,((_tmp6CC->tl=0,_tmp6CC)))))),_tmp6CB)));};}{
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
{struct Cyc_Dict_Dict*_tmp6CD;env->all_changed=((_tmp6CD=_region_malloc(env->r,sizeof(*_tmp6CD)),((_tmp6CD[0]=(env->fenv)->mt_place_set,_tmp6CD))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6CE;rvals=((_tmp6CE=_region_malloc(rgn,sizeof(*_tmp6CE)),((_tmp6CE->hd=_tmp93,((_tmp6CE->tl=0,_tmp6CE))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6CF;env->all_changed=((_tmp6CF=_region_malloc(env->r,sizeof(*_tmp6CF)),((_tmp6CF[0]=(env->fenv)->mt_place_set,_tmp6CF))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6D0;rvals=((_tmp6D0=_region_malloc(rgn,sizeof(*_tmp6D0)),((_tmp6D0->hd=_tmp98,((_tmp6D0->tl=rvals,_tmp6D0))))));}
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
struct Cyc_Dict_Dict*_tmp6D1;env->all_changed=((_tmp6D1=_region_malloc(env->r,sizeof(*_tmp6D1)),((_tmp6D1[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6D1))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp6D2;return(_tmp6D2.f1=outflow,((_tmp6D2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6D2)));};};}
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
const char*_tmp6D5;void*_tmp6D4;(_tmp6D4=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6D5="expression may not be initialized",_tag_dyneither(_tmp6D5,sizeof(char),34))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp6D6;struct _tuple15 _tmpA5=(_tmp6D6.f1=_tmp9E,((_tmp6D6.f2=(env->fenv)->unknown_all,_tmp6D6)));_npop_handler(0);return _tmpA5;};}
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
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D8;nzval=(void*)((_tmp6D8=_region_malloc((env->fenv)->r,sizeof(*_tmp6D8)),((_tmp6D8[0]=((_tmp6D9.tag=8,((_tmp6D9.f1=locname,((_tmp6D9.f2=nzval,_tmp6D9)))))),_tmp6D8))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 521
return _tmpAD;};}_LL60:;}_LL5B:;}
# 530
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 533
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp6DA;return(_tmp6DA.f1=outflow,((_tmp6DA.f2=outflow,_tmp6DA)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 536
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp6DB;return(_tmp6DB.f1=outflow,((_tmp6DB.f2=outflow,_tmp6DB)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
if(locname != 0){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6DD;nzval=(void*)((_tmp6DD=_region_malloc((env->fenv)->r,sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6DE.tag=8,((_tmp6DE.f1=locname,((_tmp6DE.f2=nzval,_tmp6DE)))))),_tmp6DD))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6E1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6E0;zval=(void*)((_tmp6E0=_region_malloc((env->fenv)->r,sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=8,((_tmp6E1.f1=locname,((_tmp6E1.f2=zval,_tmp6E1)))))),_tmp6E0))));};}{
# 545
struct _tuple18 _tmp6E2;return
(_tmp6E2.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6E2.f2=
# 548
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,zval),_tmpB0),_tmp6E2)));};}_LL6A:;}_LL65:;}
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
struct _tuple15 _tmp6E3;return(_tmp6E3.f1=f,((_tmp6E3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6E3)));}_LL77: if((_tmpC1.ReachableFL).tag != 2)goto _LL74;_tmpC2=((struct _tuple14)(_tmpC1.ReachableFL).val).f1;_tmpC3=((struct _tuple14)(_tmpC1.ReachableFL).val).f2;_LL78:
# 573
 if(Cyc_Tcutil_is_bound_one(_tmpBF)){
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmpC5=r;struct Cyc_Absyn_Vardecl*_tmpC7;void*_tmpC8;struct Cyc_CfFlowInfo_Place*_tmpCC;enum Cyc_CfFlowInfo_InitLevel _tmpCF;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpC6->tag != 8)goto _LL7C;else{_tmpC7=_tmpC6->f1;_tmpC8=(void*)_tmpC6->f2;}}_LL7B:
 r=_tmpC8;locname=_tmpC7;goto retry;_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpC9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpC9->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCA->tag != 2)goto _LL80;}_LL7F:
# 579
{void*_stmttmpF=e->annot;void*_tmpD0=_stmttmpF;struct Cyc_List_List*_tmpD2;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD1=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0;if(_tmpD1->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpD2=_tmpD1->f1;}}_LL8A:
# 581
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpD2))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 585
{void*_stmttmp10=e->r;void*_tmpD3=_stmttmp10;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD4->tag != 22)goto _LL90;}_LL8F:
# 587
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E6;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E5;e->annot=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E6)))),_tmp6E5))));}
goto _LL8D;_LL90:;_LL91:
# 590
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 593
goto _LL88;_LL88:;}
# 595
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCB->tag != 5)goto _LL82;else{_tmpCC=_tmpCB->f1;}}_LL81:
# 597
{void*_stmttmpD=e->annot;void*_tmpD7=_stmttmpD;struct Cyc_List_List*_tmpD9;_LL93: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD7;if(_tmpD8->tag != Cyc_CfFlowInfo_NotZero)goto _LL95;else{_tmpD9=_tmpD8->f1;}}_LL94:
# 599
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpD9))goto _LL96;
goto _LL92;_LL95:;_LL96:
# 603
{void*_stmttmpE=e->r;void*_tmpDA=_stmttmpE;_LL98: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpDB->tag != 22)goto _LL9A;}_LL99:
# 605
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E9;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E8;e->annot=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6E9)))),_tmp6E8))));}
goto _LL97;_LL9A:;_LL9B:
# 608
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 611
goto _LL92;_LL92:;}{
# 613
struct _tuple15 _tmp6EA;return(_tmp6EA.f1=f,((_tmp6EA.f2=Cyc_CfFlowInfo_lookup_place(_tmpC2,_tmpCC),_tmp6EA)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCD->tag != 0)goto _LL84;}_LL83:
# 615
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6EB;return(_tmp6EB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6EB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6EB)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC5;if(_tmpCE->tag != 3)goto _LL86;else{_tmpCF=_tmpCE->f1;}}_LL85:
# 618
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpCF,locname);goto _LL87;_LL86:;_LL87:
# 620
{void*_stmttmpC=e->r;void*_tmpE0=_stmttmpC;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 22)goto _LL9F;}_LL9E:
# 622
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6ED;e->annot=(void*)((_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED[0]=((_tmp6EE.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6EE)))),_tmp6ED))));}
goto _LL9C;_LL9F:;_LLA0:
# 625
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9C;_LL9C:;}
# 628
goto _LL79;_LL79:;}}else{
# 631
void*_stmttmp11=e->annot;void*_tmpE4=_stmttmp11;struct Cyc_List_List*_tmpE6;_LLA2: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE5=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE4;if(_tmpE5->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA4;else{_tmpE6=_tmpE5->f1;}}_LLA3:
# 633
 if(!Cyc_Relations_same_relns(_tmpC3,_tmpE6))goto _LLA5;
goto _LLA1;_LLA4:;_LLA5:
# 636
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6F1;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6F0;e->annot=(void*)((_tmp6F0=_cycalloc(sizeof(*_tmp6F0)),((_tmp6F0[0]=((_tmp6F1.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6F1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3),_tmp6F1)))),_tmp6F0))));}
goto _LLA1;_LLA1:;}{
# 640
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC2,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 642
{void*_tmpE9=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE9;if(_tmpEA->tag != 7)goto _LLAB;}_LLAA:
# 644
{const char*_tmp6F4;void*_tmp6F3;(_tmp6F3=0,Cyc_Tcutil_terr(e->loc,((_tmp6F4="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6F4,sizeof(char),61))),_tag_dyneither(_tmp6F3,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 647
const char*_tmp6F7;void*_tmp6F6;(_tmp6F6=0,Cyc_Tcutil_terr(e->loc,((_tmp6F7="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6F7,sizeof(char),46))),_tag_dyneither(_tmp6F6,sizeof(void*),0)));}_LLA8:;}
# 649
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 651
struct _tuple15 _tmp6F8;return(_tmp6F8.f1=f,((_tmp6F8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_all),_tmp6F8)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 653
struct _tuple15 _tmp6F9;return(_tmp6F9.f1=f,((_tmp6F9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBE,0,(env->fenv)->unknown_none),_tmp6F9)));}}};_LL74:;}_LL72:;_LL73: {
# 656
struct Cyc_Core_Impossible_exn_struct _tmp6FF;const char*_tmp6FE;struct Cyc_Core_Impossible_exn_struct*_tmp6FD;(int)_throw((void*)((_tmp6FD=_cycalloc(sizeof(*_tmp6FD)),((_tmp6FD[0]=((_tmp6FF.tag=Cyc_Core_Impossible,((_tmp6FF.f1=((_tmp6FE="right deref of non-pointer-type",_tag_dyneither(_tmp6FE,sizeof(char),32))),_tmp6FF)))),_tmp6FD)))));}_LL6F:;}
# 663
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 666
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 669
{void*_stmttmp13=e1->r;void*_tmpF4=_stmttmp13;void*_tmpF6;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpF5->tag != 1)goto _LLB2;else{_tmpF6=(void*)_tmpF5->f2;}}_LLB1: {
# 671
struct Cyc_Absyn_Vardecl*_tmpF7=Cyc_Tcutil_nonesc_vardecl(_tmpF6);
if(_tmpF7 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF7);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 677
goto _LLAF;}_LLB2:;_LLB3:
 goto _LLAF;_LLAF:;}{
# 681
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 687
return relns;};}
# 695
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc){
# 699
struct _tuple17 _stmttmp14=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF9;struct _tuple17 _tmpF8=_stmttmp14;_tmpF9=_tmpF8.f2;
{struct _tuple17 _tmp700;struct _tuple17 _stmttmp15=(_tmp700.f1=inflow,((_tmp700.f2=_tmpF9,_tmp700)));struct _tuple17 _tmpFA=_stmttmp15;struct Cyc_Dict_Dict _tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_CfFlowInfo_Place*_tmpFD;_LLB5: if(((_tmpFA.f1).ReachableFL).tag != 2)goto _LLB7;_tmpFB=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f1;_tmpFC=((struct _tuple14)((_tmpFA.f1).ReachableFL).val).f2;if(((_tmpFA.f2).PlaceL).tag != 1)goto _LLB7;_tmpFD=(struct Cyc_CfFlowInfo_Place*)((_tmpFA.f2).PlaceL).val;_LLB6: {
# 702
void*_tmpFE=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 706
_tmpFB=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpFB,env->all_changed,_tmpFD,_tmpFE);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpFB,_tmpFC);
# 711
goto _LLB4;}_LLB7: if(((_tmpFA.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 714
{const char*_tmp703;void*_tmp702;(_tmp702=0,Cyc_Tcutil_terr(loc,((_tmp703="noconsume() parameters must be l-values",_tag_dyneither(_tmp703,sizeof(char),40))),_tag_dyneither(_tmp702,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 717
return inflow;}
# 722
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 730
union Cyc_CfFlowInfo_FlowInfo _tmp102=outflow;struct Cyc_Dict_Dict _tmp103;struct Cyc_List_List*_tmp104;_LLBC: if((_tmp102.BottomFL).tag != 1)goto _LLBE;_LLBD: {
# 733
struct _tuple15 _tmp704;return(_tmp704.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp704.f2=rval,_tmp704)));}_LLBE: if((_tmp102.ReachableFL).tag != 2)goto _LLBB;_tmp103=((struct _tuple14)(_tmp102.ReachableFL).val).f1;_tmp104=((struct _tuple14)(_tmp102.ReachableFL).val).f2;_LLBF: {
# 735
union Cyc_CfFlowInfo_AbsLVal _tmp106=lval;struct Cyc_CfFlowInfo_Place*_tmp107;_LLC1: if((_tmp106.PlaceL).tag != 1)goto _LLC3;_tmp107=(struct Cyc_CfFlowInfo_Place*)(_tmp106.PlaceL).val;_LLC2: {
# 737
struct Cyc_Dict_Dict _tmp108=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp103,env->all_changed,_tmp107,rval);
_tmp104=Cyc_Relations_reln_assign_exp(fenv->r,_tmp104,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp108,_tmp104);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 743
void*_stmttmp16=Cyc_CfFlowInfo_lookup_place(_tmp103,_tmp107);void*_tmp109=_stmttmp16;_LLC6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10A->tag != 3)goto _LLC8;else{if(_tmp10A->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;}}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10B->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10C->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10D->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 3)goto _LLCE;else{if(_tmp10E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCE;}}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp10F->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 7)goto _LLD0;}}_LLCF:
 goto _LLD1;_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp111=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp109;if(_tmp111->tag != 8)goto _LLD2;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp112=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp111->f2);if(_tmp112->tag != 0)goto _LLD2;}}_LLD1:
 goto _LLC5;_LLD2:;_LLD3:
# 751
{const char*_tmp707;void*_tmp706;(_tmp706=0,Cyc_Tcutil_warn(lexp->loc,((_tmp707="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp707,sizeof(char),43))),_tag_dyneither(_tmp706,sizeof(void*),0)));}
goto _LLC5;_LLC5:;}
# 756
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 758
struct _tuple15 _tmp708;return(_tmp708.f1=outflow,((_tmp708.f2=rval,_tmp708)));};}_LLC3: if((_tmp106.UnknownL).tag != 2)goto _LLC0;_LLC4: {
# 761
struct _tuple15 _tmp709;return(_tmp709.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp709.f2=rval,_tmp709)));}_LLC0:;}_LLBB:;}
# 768
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 774
union Cyc_CfFlowInfo_FlowInfo _tmp117=f;struct Cyc_Dict_Dict _tmp118;struct Cyc_List_List*_tmp119;_LLD5: if((_tmp117.BottomFL).tag != 1)goto _LLD7;_LLD6:
 return Cyc_CfFlowInfo_BottomFL();_LLD7: if((_tmp117.ReachableFL).tag != 2)goto _LLD4;_tmp118=((struct _tuple14)(_tmp117.ReachableFL).val).f1;_tmp119=((struct _tuple14)(_tmp117.ReachableFL).val).f2;_LLD8:
# 777
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp70F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp70E;struct Cyc_CfFlowInfo_Place*_tmp70D;_tmp118=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp118,env->all_changed,(
(_tmp70D=_region_malloc(env->r,sizeof(*_tmp70D)),((_tmp70D->root=(void*)((_tmp70F=_region_malloc(env->r,sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp70E.tag=0,((_tmp70E.f1=vd,_tmp70E)))),_tmp70F)))),((_tmp70D->fields=0,_tmp70D)))))),rval);}
# 780
_tmp119=Cyc_Relations_reln_assign_var(env->r,_tmp119,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11D=Cyc_CfFlowInfo_ReachableFL(_tmp118,_tmp119);
Cyc_NewControlFlow_update_tryflow(env,_tmp11D);
# 787
return _tmp11D;};_LLD4:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 791
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 798
if(vds == 0)return inflow;{
# 801
struct Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp710;es=((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp710->tl=es,_tmp710))))));}}}
# 807
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11E,fenv->unknown_all,pat_loc,name_locs);
# 809
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 813
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp121;void*_tmp122;struct _tuple15 _tmp120=_stmttmp17;_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp121,_tmp122);}}{
# 821
struct Cyc_List_List*_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp123 != 0;_tmp123=_tmp123->tl){
struct _tuple23*_stmttmp18=(struct _tuple23*)_tmp123->hd;struct Cyc_Absyn_Vardecl**_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct _tuple23*_tmp124=_stmttmp18;_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;
if(_tmp125 != 0  && _tmp126 != 0){
# 828
if(_tmp126->topt == 0){
const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_PrintArg_struct _tmp712;(_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp126)),((_tmp713[0]=& _tmp712,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp714="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp714,sizeof(char),41))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}
# 833
Cyc_fflush(Cyc_stderr);{
# 835
struct Cyc_List_List _tmp715;struct Cyc_List_List l=(_tmp715.hd=_tmp126,((_tmp715.tl=0,_tmp715)));
union Cyc_CfFlowInfo_FlowInfo _tmp12A=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple15 _stmttmp19=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp12A,_tmp126);union Cyc_CfFlowInfo_FlowInfo _tmp12C;void*_tmp12D;struct _tuple15 _tmp12B=_stmttmp19;_tmp12C=_tmp12B.f1;_tmp12D=_tmp12B.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp718;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp717;_tmp12D=(void*)((_tmp717=_region_malloc(fenv->r,sizeof(*_tmp717)),((_tmp717[0]=((_tmp718.tag=8,((_tmp718.f1=*_tmp125,((_tmp718.f2=_tmp12D,_tmp718)))))),_tmp717))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12C,*_tmp125,_tmp126,_tmp12D,pat_loc);};}}
# 844
return inflow;};};}
# 847
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 849
void*_stmttmp1A=e->r;void*_tmp131=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp132->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp133=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp132->f2);if(_tmp133->tag != 4)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp134->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp135=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp134->f2);if(_tmp135->tag != 3)goto _LLDE;}}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp136->tag != 1)goto _LLE0;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp137=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp136->f2);if(_tmp137->tag != 5)goto _LLE0;}}_LLDF:
 return 1;_LLE0: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp138->tag != 19)goto _LLE2;else{_tmp139=_tmp138->f1;}}_LLE1:
 _tmp13B=_tmp139;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13A->tag != 20)goto _LLE4;else{_tmp13B=_tmp13A->f1;}}_LLE3:
 _tmp13D=_tmp13B;goto _LLE5;_LLE4: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13C->tag != 21)goto _LLE6;else{_tmp13D=_tmp13C->f1;}}_LLE5:
# 856
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13D);_LLE6: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp13E->tag != 22)goto _LLE8;else{_tmp13F=_tmp13E->f1;}}_LLE7: {
# 858
void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(_tmp13F->topt));void*_tmp142=_stmttmp1B;_LLED: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 10)goto _LLEF;}_LLEE:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13F);_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLE8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp131;if(_tmp140->tag != 13)goto _LLEA;else{_tmp141=_tmp140->f2;}}_LLE9:
# 862
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp141);_LLEA:;_LLEB:
 return 0;_LLD9:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7C5(unsigned int*_tmp7C4,unsigned int*_tmp7C3,void***_tmp7C1,struct Cyc_List_List**_tmp31E){for(*_tmp7C4=0;*_tmp7C4 < *_tmp7C3;(*_tmp7C4)++){
# 1436 "new_control_flow.cyc"
void*_tmp7BF;(*_tmp7C1)[*_tmp7C4]=(void*)((_tmp7BF=(void*)((struct Cyc_List_List*)_check_null(*_tmp31E))->hd,((*_tmp31E=(*_tmp31E)->tl,_tmp7BF))));}}
# 868 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 872
struct Cyc_CfFlowInfo_FlowEnv*_tmp144=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 885
{union Cyc_CfFlowInfo_FlowInfo _tmp145=inflow;struct Cyc_Dict_Dict _tmp146;struct Cyc_List_List*_tmp147;_LLF2: if((_tmp145.BottomFL).tag != 1)goto _LLF4;_LLF3: {
struct _tuple15 _tmp719;return(_tmp719.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp719.f2=_tmp144->unknown_all,_tmp719)));}_LLF4: if((_tmp145.ReachableFL).tag != 2)goto _LLF1;_tmp146=((struct _tuple14)(_tmp145.ReachableFL).val).f1;_tmp147=((struct _tuple14)(_tmp145.ReachableFL).val).f2;_LLF5:
 d=_tmp146;relns=_tmp147;_LLF1:;}
# 900 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 921 "new_control_flow.cyc"
struct _tuple17 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp14A;union Cyc_CfFlowInfo_AbsLVal _tmp14B;struct _tuple17 _tmp149=_stmttmp1C;_tmp14A=_tmp149.f1;_tmp14B=_tmp149.f2;{
struct _tuple17 _tmp71A;struct _tuple17 _stmttmp1D=(_tmp71A.f1=_tmp14A,((_tmp71A.f2=_tmp14B,_tmp71A)));struct _tuple17 _tmp14C=_stmttmp1D;struct Cyc_Dict_Dict _tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_CfFlowInfo_Place*_tmp14F;_LLF7: if(((_tmp14C.f1).ReachableFL).tag != 2)goto _LLF9;_tmp14D=((struct _tuple14)((_tmp14C.f1).ReachableFL).val).f1;_tmp14E=((struct _tuple14)((_tmp14C.f1).ReachableFL).val).f2;if(((_tmp14C.f2).PlaceL).tag != 1)goto _LLF9;_tmp14F=(struct Cyc_CfFlowInfo_Place*)((_tmp14C.f2).PlaceL).val;_LLF8: {
# 924
void*_tmp150=Cyc_CfFlowInfo_lookup_place(_tmp14D,_tmp14F);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp150,& needs_unconsume)){
{const char*_tmp71D;void*_tmp71C;(_tmp71C=0,Cyc_Tcutil_terr(e->loc,((_tmp71D="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp71D,sizeof(char),71))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}{
struct _tuple15 _tmp71E;return(_tmp71E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71E.f2=_tmp144->unknown_all,_tmp71E)));};}else{
# 930
if(needs_unconsume){
# 932
struct _tuple15 _tmp71F;return(_tmp71F.f1=_tmp14A,((_tmp71F.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp144,_tmp150),_tmp71F)));}else{
# 935
void*_tmp155=Cyc_CfFlowInfo_make_unique_consumed(_tmp144,(void*)_check_null(e->topt),e,env->iteration_num,_tmp150);
struct Cyc_Dict_Dict _tmp156=Cyc_CfFlowInfo_assign_place(_tmp144,e->loc,_tmp14D,env->all_changed,_tmp14F,_tmp155);
# 946
struct _tuple15 _tmp720;return(_tmp720.f1=Cyc_CfFlowInfo_ReachableFL(_tmp156,_tmp14E),((_tmp720.f2=_tmp150,_tmp720)));}}}_LLF9:;_LLFA:
# 948
 goto _LLF6;_LLF6:;};}{
# 951
void*_stmttmp1E=e->r;void*_tmp159=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Vardecl*_tmp172;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Vardecl*_tmp178;enum Cyc_Absyn_Primop _tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_List_List*_tmp18B;int _tmp18D;struct Cyc_Absyn_Exp*_tmp18E;void**_tmp18F;struct Cyc_Absyn_Exp*_tmp190;int _tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;struct _dyneither_ptr*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct _dyneither_ptr*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct _dyneither_ptr*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Datatypedecl*_tmp1B4;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B8;struct Cyc_List_List*_tmp1BA;enum Cyc_Absyn_AggrKind _tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BF;struct Cyc_Absyn_Vardecl*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;int _tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;void*_tmp1C7;int _tmp1C8;struct Cyc_Absyn_Stmt*_tmp1CA;_LLFC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp15A->tag != 13)goto _LLFE;else{_tmp15B=_tmp15A->f2;if(_tmp15A->f4 != Cyc_Absyn_NonNull_to_Null)goto _LLFE;}}_LLFD: {
# 955
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15B);union Cyc_CfFlowInfo_FlowInfo _tmp1D2;void*_tmp1D3;struct _tuple15 _tmp1D1=_stmttmp4F;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{
struct _tuple15 _stmttmp50=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D2,_tmp15B,_tmp1D3);union Cyc_CfFlowInfo_FlowInfo _tmp1D5;void*_tmp1D6;struct _tuple15 _tmp1D4=_stmttmp50;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;{
struct _tuple15 _tmp721;return(_tmp721.f1=_tmp1D5,((_tmp721.f2=_tmp1D3,_tmp721)));};};}_LLFE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp15C->tag != 13)goto _LL100;else{_tmp15D=_tmp15C->f2;}}_LLFF:
# 961
 _tmp15F=_tmp15D;goto _LL101;_LL100: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp15E->tag != 11)goto _LL102;else{_tmp15F=_tmp15E->f1;}}_LL101:
 _tmp161=_tmp15F;goto _LL103;_LL102: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp160->tag != 12)goto _LL104;else{_tmp161=_tmp160->f1;}}_LL103:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp161);_LL104: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp162->tag != 0)goto _LL106;else{if(((_tmp162->f1).Null_c).tag != 1)goto _LL106;}}_LL105:
# 965
 goto _LL107;_LL106: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp163->tag != 0)goto _LL108;else{if(((_tmp163->f1).Int_c).tag != 5)goto _LL108;if(((struct _tuple7)((_tmp163->f1).Int_c).val).f2 != 0)goto _LL108;}}_LL107: {
struct _tuple15 _tmp722;return(_tmp722.f1=inflow,((_tmp722.f2=_tmp144->zero,_tmp722)));}_LL108: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp164->tag != 0)goto _LL10A;else{if(((_tmp164->f1).Int_c).tag != 5)goto _LL10A;}}_LL109:
 goto _LL10B;_LL10A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp165->tag != 1)goto _LL10C;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp166=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp165->f2);if(_tmp166->tag != 2)goto _LL10C;}}_LL10B: {
struct _tuple15 _tmp723;return(_tmp723.f1=inflow,((_tmp723.f2=_tmp144->notzeroall,_tmp723)));}_LL10C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp167->tag != 30)goto _LL10E;else{if(_tmp167->f1 != 0)goto _LL10E;}}_LL10D:
# 970
 goto _LL10F;_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp168->tag != 0)goto _LL110;}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp169->tag != 17)goto _LL112;}_LL111:
 goto _LL113;_LL112: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp16A->tag != 16)goto _LL114;}_LL113:
 goto _LL115;_LL114: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp16B->tag != 18)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp16C->tag != 32)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp16D->tag != 31)goto _LL11A;}_LL119: {
struct _tuple15 _tmp724;return(_tmp724.f1=inflow,((_tmp724.f2=_tmp144->unknown_all,_tmp724)));}_LL11A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp16E->tag != 1)goto _LL11C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 1)goto _LL11C;}}_LL11B: {
# 979
struct _tuple15 _tmp725;return(_tmp725.f1=inflow,((_tmp725.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp725)));}_LL11C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp170->tag != 1)goto _LL11E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp171=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp170->f2);if(_tmp171->tag != 3)goto _LL11E;else{_tmp172=_tmp171->f1;}}}_LL11D:
# 982
 _tmp175=_tmp172;goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp173->tag != 1)goto _LL120;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 4)goto _LL120;else{_tmp175=_tmp174->f1;}}}_LL11F:
 _tmp178=_tmp175;goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp176->tag != 1)goto _LL122;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp177=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp176->f2);if(_tmp177->tag != 5)goto _LL122;else{_tmp178=_tmp177->f1;}}}_LL121: {
# 986
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp726;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp726.tag=0,((_tmp726.f1=_tmp178,_tmp726)));
struct _tuple15 _tmp727;return(_tmp727.f1=inflow,((_tmp727.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp727)));}_LL122: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp179->tag != 2)goto _LL124;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;}}_LL123: {
# 993
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp17B);union Cyc_CfFlowInfo_FlowInfo _tmp1DF;void*_tmp1E0;struct _tuple15 _tmp1DE=_stmttmp4E;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;{
struct _tuple15 _tmp728;return(_tmp728.f1=_tmp1DF,((_tmp728.f2=_tmp1E0,_tmp728)));};}_LL124: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp17C->tag != 4)goto _LL126;else{_tmp17D=_tmp17C->f1;}}_LL125: {
# 997
struct Cyc_List_List _tmp729;struct Cyc_List_List _tmp1E2=(_tmp729.hd=_tmp17D,((_tmp729.tl=0,_tmp729)));
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1E2);union Cyc_CfFlowInfo_FlowInfo _tmp1E4;struct _tuple15 _tmp1E3=_stmttmp4B;_tmp1E4=_tmp1E3.f1;{
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E4,0,0,_tmp17D);union Cyc_CfFlowInfo_AbsLVal _tmp1E6;struct _tuple17 _tmp1E5=_stmttmp4C;_tmp1E6=_tmp1E5.f2;
{struct _tuple24 _tmp72A;struct _tuple24 _stmttmp4D=(_tmp72A.f1=_tmp1E6,((_tmp72A.f2=_tmp1E4,_tmp72A)));struct _tuple24 _tmp1E7=_stmttmp4D;struct Cyc_CfFlowInfo_Place*_tmp1E8;struct Cyc_Dict_Dict _tmp1E9;struct Cyc_List_List*_tmp1EA;_LL165: if(((_tmp1E7.f1).PlaceL).tag != 1)goto _LL167;_tmp1E8=(struct Cyc_CfFlowInfo_Place*)((_tmp1E7.f1).PlaceL).val;if(((_tmp1E7.f2).ReachableFL).tag != 2)goto _LL167;_tmp1E9=((struct _tuple14)((_tmp1E7.f2).ReachableFL).val).f1;_tmp1EA=((struct _tuple14)((_tmp1E7.f2).ReachableFL).val).f2;_LL166:
# 1002
 _tmp1EA=Cyc_Relations_reln_kill_exp(_tmp144->r,_tmp1EA,_tmp17D);
_tmp1E4=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp144,_tmp17D->loc,_tmp1E9,env->all_changed,_tmp1E8,_tmp144->unknown_all),_tmp1EA);
# 1006
goto _LL164;_LL167:;_LL168:
 goto _LL164;_LL164:;}{
# 1009
struct _tuple15 _tmp72B;return(_tmp72B.f1=_tmp1E4,((_tmp72B.f2=_tmp144->unknown_all,_tmp72B)));};};}_LL126: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp17E->tag != 3)goto _LL128;else{_tmp17F=_tmp17E->f1;if(_tmp17E->f2 == 0)goto _LL128;_tmp180=_tmp17E->f3;}}_LL127:
# 1012
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72E;void*_tmp72D;(_tmp72D=0,Cyc_Tcutil_terr(e->loc,((_tmp72E="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72E,sizeof(char),58))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}{
struct _tuple15 _tmp72F;return(_tmp72F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp72F.f2=_tmp144->unknown_all,_tmp72F)));};}{
# 1016
struct Cyc_List_List _tmp730;struct Cyc_List_List _tmp1F1=(_tmp730.hd=_tmp180,((_tmp730.tl=0,_tmp730)));
struct Cyc_List_List _tmp731;struct Cyc_List_List _tmp1F2=(_tmp731.hd=_tmp17F,((_tmp731.tl=& _tmp1F1,_tmp731)));
struct _tuple15 _stmttmp49=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F2);union Cyc_CfFlowInfo_FlowInfo _tmp1F4;struct _tuple15 _tmp1F3=_stmttmp49;_tmp1F4=_tmp1F3.f1;{
struct _tuple17 _stmttmp4A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F4,0,0,_tmp17F);union Cyc_CfFlowInfo_AbsLVal _tmp1F6;struct _tuple17 _tmp1F5=_stmttmp4A;_tmp1F6=_tmp1F5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1F7=_tmp1F4;struct Cyc_Dict_Dict _tmp1F8;struct Cyc_List_List*_tmp1F9;_LL16A: if((_tmp1F7.ReachableFL).tag != 2)goto _LL16C;_tmp1F8=((struct _tuple14)(_tmp1F7.ReachableFL).val).f1;_tmp1F9=((struct _tuple14)(_tmp1F7.ReachableFL).val).f2;_LL16B:
# 1022
{union Cyc_CfFlowInfo_AbsLVal _tmp1FA=_tmp1F6;struct Cyc_CfFlowInfo_Place*_tmp1FB;_LL16F: if((_tmp1FA.PlaceL).tag != 1)goto _LL171;_tmp1FB=(struct Cyc_CfFlowInfo_Place*)(_tmp1FA.PlaceL).val;_LL170:
# 1026
 _tmp1F9=Cyc_Relations_reln_kill_exp(_tmp144->r,_tmp1F9,_tmp17F);
_tmp1F8=Cyc_CfFlowInfo_assign_place(_tmp144,_tmp17F->loc,_tmp1F8,env->all_changed,_tmp1FB,_tmp144->unknown_all);
# 1029
_tmp1F4=Cyc_CfFlowInfo_ReachableFL(_tmp1F8,_tmp1F9);
# 1032
goto _LL16E;_LL171: if((_tmp1FA.UnknownL).tag != 2)goto _LL16E;_LL172:
# 1035
 goto _LL16E;_LL16E:;}
# 1037
goto _LL169;_LL16C:;_LL16D:
 goto _LL169;_LL169:;}{
# 1040
struct _tuple15 _tmp732;return(_tmp732.f1=_tmp1F4,((_tmp732.f2=_tmp144->unknown_all,_tmp732)));};};};_LL128: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp181->tag != 3)goto _LL12A;else{_tmp182=_tmp181->f1;if(_tmp181->f2 != 0)goto _LL12A;_tmp183=_tmp181->f3;}}_LL129:
# 1044
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp735;void*_tmp734;(_tmp734=0,Cyc_Tcutil_terr(e->loc,((_tmp735="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp735,sizeof(char),58))),_tag_dyneither(_tmp734,sizeof(void*),0)));}{
struct _tuple15 _tmp736;return(_tmp736.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp736.f2=_tmp144->unknown_all,_tmp736)));};}{
# 1048
struct Cyc_Dict_Dict*_tmp202=env->all_changed;
# 1050
{struct Cyc_Absyn_Exp*_tmp737[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp737[1]=_tmp183,((_tmp737[0]=_tmp182,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp737,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp738;env->all_changed=((_tmp738=_region_malloc(env->r,sizeof(*_tmp738)),((_tmp738[0]=_tmp144->mt_place_set,_tmp738))));}{
struct _tuple17 _stmttmp47=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp182);union Cyc_CfFlowInfo_FlowInfo _tmp206;union Cyc_CfFlowInfo_AbsLVal _tmp207;struct _tuple17 _tmp205=_stmttmp47;_tmp206=_tmp205.f1;_tmp207=_tmp205.f2;{
struct Cyc_Dict_Dict _tmp208=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp739;env->all_changed=((_tmp739=_region_malloc(env->r,sizeof(*_tmp739)),((_tmp739[0]=_tmp144->mt_place_set,_tmp739))));}{
struct _tuple15 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp183);union Cyc_CfFlowInfo_FlowInfo _tmp20B;void*_tmp20C;struct _tuple15 _tmp20A=_stmttmp48;_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;{
struct Cyc_Dict_Dict _tmp20D=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp20E=Cyc_CfFlowInfo_after_flow(_tmp144,& _tmp208,_tmp206,_tmp20B,_tmp208,_tmp20D);
# 1061
union Cyc_CfFlowInfo_FlowInfo _tmp20F=Cyc_CfFlowInfo_join_flow(_tmp144,_tmp202,inflow,_tmp20E);
# 1064
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20F,inflow)){
if(_tmp202 == 0)
env->all_changed=0;else{
# 1068
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp202,
Cyc_CfFlowInfo_union_place_set(_tmp208,_tmp20D,0),0);}
# 1071
return Cyc_NewControlFlow_do_assign(_tmp144,env,_tmp20E,_tmp182,_tmp207,_tmp183,_tmp20C,e->loc);}
# 1074
inflow=_tmp20F;};};};};}};_LL12A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp184->tag != 8)goto _LL12C;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}_LL12B: {
# 1079
struct _tuple15 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp185);union Cyc_CfFlowInfo_FlowInfo _tmp211;void*_tmp212;struct _tuple15 _tmp210=_stmttmp46;_tmp211=_tmp210.f1;_tmp212=_tmp210.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp211,_tmp186);}_LL12C: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp187->tag != 10)goto _LL12E;else{_tmp188=_tmp187->f1;}}_LL12D: {
# 1083
struct _tuple15 _stmttmp45=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp188);union Cyc_CfFlowInfo_FlowInfo _tmp214;void*_tmp215;struct _tuple15 _tmp213=_stmttmp45;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp188->loc,_tmp214,_tmp215);{
struct _tuple15 _tmp73A;return(_tmp73A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp73A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp73A)));};}_LL12E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp189->tag != 9)goto _LL130;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LL12F: {
# 1088
struct _RegionHandle _tmp217=_new_region("temp");struct _RegionHandle*temp=& _tmp217;_push_region(temp);
{struct Cyc_List_List*_tmp73B;struct _tuple22 _stmttmp3F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp73B=_region_malloc(temp,sizeof(*_tmp73B)),((_tmp73B->hd=_tmp18A,((_tmp73B->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp18B),_tmp73B)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp219;struct Cyc_List_List*_tmp21A;struct _tuple22 _tmp218=_stmttmp3F;_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;{
# 1091
union Cyc_CfFlowInfo_FlowInfo _tmp21B=Cyc_NewControlFlow_use_Rval(env,_tmp18A->loc,_tmp219,(void*)((struct Cyc_List_List*)_check_null(_tmp21A))->hd);
_tmp21A=_tmp21A->tl;{
# 1094
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp18A->topt));void*_tmp21C=_stmttmp40;void*_tmp21E;_LL174: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C;if(_tmp21D->tag != 5)goto _LL176;else{_tmp21E=(_tmp21D->f1).elt_typ;}}_LL175:
# 1099
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp21E);void*_tmp21F=_stmttmp41;struct Cyc_List_List*_tmp221;_LL179: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21F;if(_tmp220->tag != 9)goto _LL17B;else{_tmp221=(_tmp220->f1).attributes;}}_LL17A:
# 1101
 for(0;_tmp221 != 0;_tmp221=_tmp221->tl){
# 1103
void*_stmttmp42=(void*)_tmp221->hd;void*_tmp222=_stmttmp42;int _tmp224;int _tmp226;int _tmp228;_LL17E: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp223=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp222;if(_tmp223->tag != 20)goto _LL180;else{_tmp224=_tmp223->f1;}}_LL17F:
# 1105
{struct Cyc_List_List*_tmp73C;init_params=((_tmp73C=_region_malloc(temp,sizeof(*_tmp73C)),((_tmp73C->hd=(void*)_tmp224,((_tmp73C->tl=init_params,_tmp73C))))));}goto _LL17D;_LL180: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp225=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp222;if(_tmp225->tag != 21)goto _LL182;else{_tmp226=_tmp225->f1;}}_LL181:
# 1107
{struct Cyc_List_List*_tmp73D;nolive_unique_params=((_tmp73D=_region_malloc(temp,sizeof(*_tmp73D)),((_tmp73D->hd=(void*)_tmp226,((_tmp73D->tl=nolive_unique_params,_tmp73D))))));}
goto _LL17D;_LL182: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp227=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp222;if(_tmp227->tag != 22)goto _LL184;else{_tmp228=_tmp227->f1;}}_LL183:
# 1111
{struct Cyc_List_List*_tmp73E;noconsume_params=((_tmp73E=_region_malloc(temp,sizeof(*_tmp73E)),((_tmp73E->hd=(void*)_tmp228,((_tmp73E->tl=noconsume_params,_tmp73E))))));}
goto _LL17D;_LL184:;_LL185:
 goto _LL17D;_LL17D:;}
# 1115
goto _LL178;_LL17B:;_LL17C: {
const char*_tmp741;void*_tmp740;(_tmp740=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp741="anal_Rexp: bad function type",_tag_dyneither(_tmp741,sizeof(char),29))),_tag_dyneither(_tmp740,sizeof(void*),0)));}_LL178:;}
# 1118
goto _LL173;_LL176:;_LL177: {
const char*_tmp744;void*_tmp743;(_tmp743=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp744="anal_Rexp: bad function type",_tag_dyneither(_tmp744,sizeof(char),29))),_tag_dyneither(_tmp743,sizeof(void*),0)));}_LL173:;}
# 1121
{int i=1;for(0;_tmp21A != 0;(((_tmp21A=_tmp21A->tl,_tmp18B=((struct Cyc_List_List*)_check_null(_tmp18B))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp230=_tmp219;struct Cyc_Dict_Dict _tmp231;_LL187: if((_tmp230.BottomFL).tag != 1)goto _LL189;_LL188:
 goto _LL186;_LL189: if((_tmp230.ReachableFL).tag != 2)goto _LL186;_tmp231=((struct _tuple14)(_tmp230.ReachableFL).val).f1;_LL18A:
# 1126
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp231,(void*)_tmp21A->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp747;void*_tmp746;(_tmp746=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18B))->hd)->loc,((_tmp747="expression may not be initialized",_tag_dyneither(_tmp747,sizeof(char),34))),_tag_dyneither(_tmp746,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp234=_tmp21B;struct Cyc_Dict_Dict _tmp235;struct Cyc_List_List*_tmp236;_LL18C: if((_tmp234.BottomFL).tag != 1)goto _LL18E;_LL18D:
 goto _LL18B;_LL18E: if((_tmp234.ReachableFL).tag != 2)goto _LL18B;_tmp235=((struct _tuple14)(_tmp234.ReachableFL).val).f1;_tmp236=((struct _tuple14)(_tmp234.ReachableFL).val).f2;_LL18F: {
# 1133
struct Cyc_Dict_Dict _tmp237=Cyc_CfFlowInfo_escape_deref(_tmp144,_tmp235,env->all_changed,(void*)_tmp21A->hd);
{void*_stmttmp43=(void*)_tmp21A->hd;void*_tmp238=_stmttmp43;struct Cyc_CfFlowInfo_Place*_tmp23A;_LL191: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp239=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp238;if(_tmp239->tag != 5)goto _LL193;else{_tmp23A=_tmp239->f1;}}_LL192:
# 1136
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18B))->hd)->topt));void*_tmp23B=_stmttmp44;void*_tmp23D;_LL196: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B;if(_tmp23C->tag != 5)goto _LL198;else{_tmp23D=(_tmp23C->f1).elt_typ;}}_LL197:
# 1140
 _tmp237=Cyc_CfFlowInfo_assign_place(_tmp144,((struct Cyc_Absyn_Exp*)_tmp18B->hd)->loc,_tmp237,env->all_changed,_tmp23A,
# 1142
Cyc_CfFlowInfo_typ_to_absrval(_tmp144,_tmp23D,0,_tmp144->esc_all));
# 1144
goto _LL195;_LL198:;_LL199: {
const char*_tmp74A;void*_tmp749;(_tmp749=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp74A="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp74A,sizeof(char),39))),_tag_dyneither(_tmp749,sizeof(void*),0)));}_LL195:;}
# 1147
goto _LL190;_LL193:;_LL194:
 goto _LL190;_LL190:;}
# 1150
_tmp21B=Cyc_CfFlowInfo_ReachableFL(_tmp237,_tmp236);
goto _LL18B;}_LL18B:;}
# 1153
goto _LL186;_LL186:;}else{
# 1156
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1161
_tmp21B=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18B))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18B))->hd)->topt),_tmp21B,(void*)_tmp21A->hd);else{
# 1168
_tmp21B=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp18B))->hd)->loc,_tmp21B,(void*)_tmp21A->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i))
_tmp21B=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp21B,(struct Cyc_Absyn_Exp*)_tmp18B->hd,((struct Cyc_Absyn_Exp*)_tmp18B->hd)->loc);}}}}
# 1174
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp18A->topt))){
struct _tuple15 _tmp74B;struct _tuple15 _tmp241=(_tmp74B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp74B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp74B)));_npop_handler(0);return _tmp241;}else{
# 1177
struct _tuple15 _tmp74C;struct _tuple15 _tmp243=(_tmp74C.f1=_tmp21B,((_tmp74C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp74C)));_npop_handler(0);return _tmp243;}};};}
# 1089
;_pop_region(temp);}_LL130: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp18C->tag != 33)goto _LL132;else{_tmp18D=(_tmp18C->f1).is_calloc;_tmp18E=(_tmp18C->f1).rgn;_tmp18F=(_tmp18C->f1).elt_type;_tmp190=(_tmp18C->f1).num_elts;_tmp191=(_tmp18C->f1).fat_result;}}_LL131: {
# 1180
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp74F;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp74E;void*root=(void*)((_tmp74E=_region_malloc(_tmp144->r,sizeof(*_tmp74E)),((_tmp74E[0]=((_tmp74F.tag=1,((_tmp74F.f1=_tmp190,((_tmp74F.f2=(void*)_check_null(e->topt),_tmp74F)))))),_tmp74E))));
struct Cyc_CfFlowInfo_Place*_tmp750;struct Cyc_CfFlowInfo_Place*place=(_tmp750=_region_malloc(_tmp144->r,sizeof(*_tmp750)),((_tmp750->root=root,((_tmp750->fields=0,_tmp750)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp753;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp752;void*rval=(void*)((_tmp752=_region_malloc(_tmp144->r,sizeof(*_tmp752)),((_tmp752[0]=((_tmp753.tag=5,((_tmp753.f1=place,_tmp753)))),_tmp752))));
void*place_val;
if(_tmp191)place_val=_tmp144->notzeroall;else{
if(_tmp18D)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,*((void**)_check_null(_tmp18F)),0,_tmp144->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,*((void**)_check_null(_tmp18F)),0,_tmp144->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp18E != 0){
struct _RegionHandle _tmp245=_new_region("temp");struct _RegionHandle*temp=& _tmp245;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp754[2];struct _tuple22 _stmttmp3D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp754[1]=_tmp190,((_tmp754[0]=_tmp18E,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp754,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1191
union Cyc_CfFlowInfo_FlowInfo _tmp247;struct Cyc_List_List*_tmp248;struct _tuple22 _tmp246=_stmttmp3D;_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;
# 1193
outflow=_tmp247;}
# 1191
;_pop_region(temp);}else{
# 1196
struct _tuple15 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp190);union Cyc_CfFlowInfo_FlowInfo _tmp24B;struct _tuple15 _tmp24A=_stmttmp3E;_tmp24B=_tmp24A.f1;
outflow=_tmp24B;}{
# 1199
union Cyc_CfFlowInfo_FlowInfo _tmp24C=outflow;struct Cyc_Dict_Dict _tmp24D;struct Cyc_List_List*_tmp24E;_LL19B: if((_tmp24C.BottomFL).tag != 1)goto _LL19D;_LL19C: {
struct _tuple15 _tmp755;return(_tmp755.f1=outflow,((_tmp755.f2=rval,_tmp755)));}_LL19D: if((_tmp24C.ReachableFL).tag != 2)goto _LL19A;_tmp24D=((struct _tuple14)(_tmp24C.ReachableFL).val).f1;_tmp24E=((struct _tuple14)(_tmp24C.ReachableFL).val).f2;_LL19E: {
# 1202
struct _tuple15 _tmp756;return(_tmp756.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp24D,root,place_val),_tmp24E),((_tmp756.f2=rval,_tmp756)));}_LL19A:;};};}_LL132: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp192->tag != 34)goto _LL134;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LL133: {
# 1206
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1212
struct _RegionHandle _tmp256=_new_region("temp");struct _RegionHandle*temp=& _tmp256;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp757[2];struct _tuple22 _stmttmp3A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp757[1]=_tmp194,((_tmp757[0]=_tmp193,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp757,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1213
union Cyc_CfFlowInfo_FlowInfo _tmp258;struct Cyc_List_List*_tmp259;struct _tuple22 _tmp257=_stmttmp3A;_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;
# 1215
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp259))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp259->tl))->hd;
outflow=_tmp258;}{
# 1220
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp193);union Cyc_CfFlowInfo_AbsLVal _tmp25C;struct _tuple17 _tmp25B=_stmttmp3B;_tmp25C=_tmp25B.f2;{
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp194);union Cyc_CfFlowInfo_AbsLVal _tmp25E;struct _tuple17 _tmp25D=_stmttmp3C;_tmp25E=_tmp25D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp25F=outflow;struct Cyc_Dict_Dict _tmp260;struct Cyc_List_List*_tmp261;_LL1A0: if((_tmp25F.ReachableFL).tag != 2)goto _LL1A2;_tmp260=((struct _tuple14)(_tmp25F.ReachableFL).val).f1;_tmp261=((struct _tuple14)(_tmp25F.ReachableFL).val).f2;_LL1A1:
# 1224
{union Cyc_CfFlowInfo_AbsLVal _tmp262=_tmp25C;struct Cyc_CfFlowInfo_Place*_tmp263;_LL1A5: if((_tmp262.PlaceL).tag != 1)goto _LL1A7;_tmp263=(struct Cyc_CfFlowInfo_Place*)(_tmp262.PlaceL).val;_LL1A6:
# 1226
 _tmp260=Cyc_CfFlowInfo_assign_place(_tmp144,_tmp193->loc,_tmp260,env->all_changed,_tmp263,right_rval);
# 1228
goto _LL1A4;_LL1A7: if((_tmp262.UnknownL).tag != 2)goto _LL1A4;_LL1A8:
 goto _LL1A4;_LL1A4:;}
# 1231
{union Cyc_CfFlowInfo_AbsLVal _tmp264=_tmp25E;struct Cyc_CfFlowInfo_Place*_tmp265;_LL1AA: if((_tmp264.PlaceL).tag != 1)goto _LL1AC;_tmp265=(struct Cyc_CfFlowInfo_Place*)(_tmp264.PlaceL).val;_LL1AB:
# 1233
 _tmp260=Cyc_CfFlowInfo_assign_place(_tmp144,_tmp194->loc,_tmp260,env->all_changed,_tmp265,left_rval);
# 1235
goto _LL1A9;_LL1AC: if((_tmp264.UnknownL).tag != 2)goto _LL1A9;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1239
_tmp261=Cyc_Relations_reln_kill_exp(_tmp144->r,_tmp261,_tmp193);
_tmp261=Cyc_Relations_reln_kill_exp(_tmp144->r,_tmp261,_tmp194);
# 1242
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp260,_tmp261);
goto _LL19F;_LL1A2:;_LL1A3:
 goto _LL19F;_LL19F:;}{
# 1248
struct _tuple15 _tmp758;struct _tuple15 _tmp267=(_tmp758.f1=outflow,((_tmp758.f2=_tmp144->unknown_all,_tmp758)));_npop_handler(0);return _tmp267;};};};
# 1212
;_pop_region(temp);}_LL134: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp195->tag != 15)goto _LL136;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LL135: {
# 1251
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp75B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp75A;void*root=(void*)((_tmp75A=_region_malloc(_tmp144->r,sizeof(*_tmp75A)),((_tmp75A[0]=((_tmp75B.tag=1,((_tmp75B.f1=_tmp197,((_tmp75B.f2=(void*)_check_null(e->topt),_tmp75B)))))),_tmp75A))));
struct Cyc_CfFlowInfo_Place*_tmp75C;struct Cyc_CfFlowInfo_Place*place=(_tmp75C=_region_malloc(_tmp144->r,sizeof(*_tmp75C)),((_tmp75C->root=root,((_tmp75C->fields=0,_tmp75C)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp75F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp75E;void*rval=(void*)((_tmp75E=_region_malloc(_tmp144->r,sizeof(*_tmp75E)),((_tmp75E[0]=((_tmp75F.tag=5,((_tmp75F.f1=place,_tmp75F)))),_tmp75E))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp196 != 0){
struct _RegionHandle _tmp268=_new_region("temp");struct _RegionHandle*temp=& _tmp268;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp760[2];struct _tuple22 _stmttmp38=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp760[1]=_tmp197,((_tmp760[0]=_tmp196,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp760,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1259
union Cyc_CfFlowInfo_FlowInfo _tmp26A;struct Cyc_List_List*_tmp26B;struct _tuple22 _tmp269=_stmttmp38;_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;
# 1261
outflow=_tmp26A;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp26B))->tl))->hd;}
# 1259
;_pop_region(temp);}else{
# 1265
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp197);union Cyc_CfFlowInfo_FlowInfo _tmp26E;void*_tmp26F;struct _tuple15 _tmp26D=_stmttmp39;_tmp26E=_tmp26D.f1;_tmp26F=_tmp26D.f2;
outflow=_tmp26E;
place_val=_tmp26F;}{
# 1269
union Cyc_CfFlowInfo_FlowInfo _tmp270=outflow;struct Cyc_Dict_Dict _tmp271;struct Cyc_List_List*_tmp272;_LL1AF: if((_tmp270.BottomFL).tag != 1)goto _LL1B1;_LL1B0: {
struct _tuple15 _tmp761;return(_tmp761.f1=outflow,((_tmp761.f2=rval,_tmp761)));}_LL1B1: if((_tmp270.ReachableFL).tag != 2)goto _LL1AE;_tmp271=((struct _tuple14)(_tmp270.ReachableFL).val).f1;_tmp272=((struct _tuple14)(_tmp270.ReachableFL).val).f2;_LL1B2: {
# 1272
struct _tuple15 _tmp762;return(_tmp762.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp271,root,place_val),_tmp272),((_tmp762.f2=rval,_tmp762)));}_LL1AE:;};};}_LL136: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp198->tag != 14)goto _LL138;else{_tmp199=_tmp198->f1;}}_LL137: {
# 1276
struct _tuple17 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp27B;union Cyc_CfFlowInfo_AbsLVal _tmp27C;struct _tuple17 _tmp27A=_stmttmp37;_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp27D=_tmp27C;struct Cyc_CfFlowInfo_Place*_tmp27E;_LL1B4: if((_tmp27D.UnknownL).tag != 2)goto _LL1B6;_LL1B5: {
struct _tuple15 _tmp763;return(_tmp763.f1=_tmp27B,((_tmp763.f2=_tmp144->notzeroall,_tmp763)));}_LL1B6: if((_tmp27D.PlaceL).tag != 1)goto _LL1B3;_tmp27E=(struct Cyc_CfFlowInfo_Place*)(_tmp27D.PlaceL).val;_LL1B7: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp769;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp768;struct _tuple15 _tmp767;return(_tmp767.f1=_tmp27B,((_tmp767.f2=(void*)((_tmp769=_region_malloc(env->r,sizeof(*_tmp769)),((_tmp769[0]=((_tmp768.tag=5,((_tmp768.f1=_tmp27E,_tmp768)))),_tmp769)))),_tmp767)));}_LL1B3:;};}_LL138: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp19A->tag != 19)goto _LL13A;else{_tmp19B=_tmp19A->f1;}}_LL139: {
# 1283
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19B);union Cyc_CfFlowInfo_FlowInfo _tmp284;void*_tmp285;struct _tuple15 _tmp283=_stmttmp36;_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp284,_tmp19B,_tmp285);}_LL13A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp19C->tag != 20)goto _LL13C;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}_LL13B: {
# 1287
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19D);union Cyc_CfFlowInfo_FlowInfo _tmp287;void*_tmp288;struct _tuple15 _tmp286=_stmttmp35;_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;
if(_tmp19D->topt == 0){
{const char*_tmp76D;void*_tmp76C[1];struct Cyc_String_pa_PrintArg_struct _tmp76B;(_tmp76B.tag=0,((_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp76C[0]=& _tmp76B,Cyc_fprintf(Cyc_stderr,((_tmp76D="aggrmember exp %s\n",_tag_dyneither(_tmp76D,sizeof(char),19))),_tag_dyneither(_tmp76C,sizeof(void*),1)))))));}{
const char*_tmp771;void*_tmp770[1];struct Cyc_String_pa_PrintArg_struct _tmp76F;(_tmp76F.tag=0,((_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp19D)),((_tmp770[0]=& _tmp76F,Cyc_fprintf(Cyc_stderr,((_tmp771="oops! %s.topt = null!\n",_tag_dyneither(_tmp771,sizeof(char),23))),_tag_dyneither(_tmp770,sizeof(void*),1)))))));};}{
# 1292
void*_tmp28F=(void*)_check_null(_tmp19D->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp28F)){
# 1295
struct _tuple15 _tmp772;return(_tmp772.f1=_tmp287,((_tmp772.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp772)));}
# 1297
if(Cyc_Absyn_is_require_union_type(_tmp28F))
Cyc_NewControlFlow_check_requires(_tmp19D->loc,_tmp144->r,_tmp28F,_tmp19E,_tmp287);{
# 1300
void*_tmp291=_tmp288;int _tmp294;int _tmp295;struct _dyneither_ptr _tmp296;int _tmp298;int _tmp299;struct _dyneither_ptr _tmp29A;_LL1B9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp292=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp291;if(_tmp292->tag != 8)goto _LL1BB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp293=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp292->f2);if(_tmp293->tag != 6)goto _LL1BB;else{_tmp294=(_tmp293->f1).is_union;_tmp295=(_tmp293->f1).fieldnum;_tmp296=_tmp293->f2;}}}_LL1BA:
# 1302
 _tmp298=_tmp294;_tmp299=_tmp295;_tmp29A=_tmp296;goto _LL1BC;_LL1BB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp297=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp291;if(_tmp297->tag != 6)goto _LL1BD;else{_tmp298=(_tmp297->f1).is_union;_tmp299=(_tmp297->f1).fieldnum;_tmp29A=_tmp297->f2;}}_LL1BC: {
# 1304
int _tmp29B=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19D->topt),_tmp19E);
if((_tmp298  && _tmp299 != - 1) && _tmp299 != _tmp29B){
struct _tuple15 _tmp773;return(_tmp773.f1=_tmp287,((_tmp773.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),1,_tmp144->unknown_none),_tmp773)));}{
struct _tuple15 _tmp774;return(_tmp774.f1=_tmp287,((_tmp774.f2=*((void**)_check_dyneither_subscript(_tmp29A,sizeof(void*),_tmp29B)),_tmp774)));};}_LL1BD:;_LL1BE:
# 1309
{const char*_tmp777;void*_tmp776;(_tmp776=0,Cyc_fprintf(Cyc_stderr,((_tmp777="the bad rexp is :",_tag_dyneither(_tmp777,sizeof(char),18))),_tag_dyneither(_tmp776,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp288);
{const char*_tmp77A;void*_tmp779;(_tmp779=0,Cyc_fprintf(Cyc_stderr,((_tmp77A="\n",_tag_dyneither(_tmp77A,sizeof(char),2))),_tag_dyneither(_tmp779,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp787;const char*_tmp786;void*_tmp785[1];struct Cyc_String_pa_PrintArg_struct _tmp784;struct Cyc_Core_Impossible_exn_struct*_tmp783;(int)_throw((void*)((_tmp783=_cycalloc(sizeof(*_tmp783)),((_tmp783[0]=((_tmp787.tag=Cyc_Core_Impossible,((_tmp787.f1=(struct _dyneither_ptr)((_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp785[0]=& _tmp784,Cyc_aprintf(((_tmp786="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp786,sizeof(char),26))),_tag_dyneither(_tmp785,sizeof(void*),1)))))))),_tmp787)))),_tmp783)))));};_LL1B8:;};};}_LL13C: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp19F->tag != 37)goto _LL13E;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL13D: {
# 1319
struct _tuple15 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A0);union Cyc_CfFlowInfo_FlowInfo _tmp2A8;void*_tmp2A9;struct _tuple15 _tmp2A7=_stmttmp34;_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;
# 1321
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1A0->topt))){
struct _tuple15 _tmp788;return(_tmp788.f1=_tmp2A8,((_tmp788.f2=_tmp144->unknown_all,_tmp788)));}{
void*_tmp2AB=_tmp2A9;int _tmp2AE;int _tmp2AF;struct _dyneither_ptr _tmp2B0;int _tmp2B2;int _tmp2B3;struct _dyneither_ptr _tmp2B4;_LL1C0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2AB;if(_tmp2AC->tag != 8)goto _LL1C2;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2AC->f2);if(_tmp2AD->tag != 6)goto _LL1C2;else{_tmp2AE=(_tmp2AD->f1).is_union;_tmp2AF=(_tmp2AD->f1).fieldnum;_tmp2B0=_tmp2AD->f2;}}}_LL1C1:
# 1325
 _tmp2B2=_tmp2AE;_tmp2B3=_tmp2AF;_tmp2B4=_tmp2B0;goto _LL1C3;_LL1C2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AB;if(_tmp2B1->tag != 6)goto _LL1C4;else{_tmp2B2=(_tmp2B1->f1).is_union;_tmp2B3=(_tmp2B1->f1).fieldnum;_tmp2B4=_tmp2B1->f2;}}_LL1C3: {
# 1327
int _tmp2B5=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A0->topt),_tmp1A1);
if(_tmp2B2  && _tmp2B3 != - 1){
if(_tmp2B3 != _tmp2B5){
struct _tuple15 _tmp789;return(_tmp789.f1=_tmp2A8,((_tmp789.f2=_tmp144->zero,_tmp789)));}else{
# 1332
struct _tuple15 _tmp78A;return(_tmp78A.f1=_tmp2A8,((_tmp78A.f2=_tmp144->notzeroall,_tmp78A)));}}else{
# 1334
struct _tuple15 _tmp78B;return(_tmp78B.f1=_tmp2A8,((_tmp78B.f2=_tmp144->unknown_all,_tmp78B)));}}_LL1C4:;_LL1C5: {
# 1336
struct Cyc_Core_Impossible_exn_struct _tmp798;const char*_tmp797;void*_tmp796[1];struct Cyc_String_pa_PrintArg_struct _tmp795;struct Cyc_Core_Impossible_exn_struct*_tmp794;(int)_throw((void*)((_tmp794=_cycalloc(sizeof(*_tmp794)),((_tmp794[0]=((_tmp798.tag=Cyc_Core_Impossible,((_tmp798.f1=(struct _dyneither_ptr)((_tmp795.tag=0,((_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp796[0]=& _tmp795,Cyc_aprintf(((_tmp797="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp797,sizeof(char),26))),_tag_dyneither(_tmp796,sizeof(void*),1)))))))),_tmp798)))),_tmp794)))));}_LL1BF:;};}_LL13E: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1A2->tag != 21)goto _LL140;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LL13F: {
# 1341
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A3);union Cyc_CfFlowInfo_FlowInfo _tmp2BF;void*_tmp2C0;struct _tuple15 _tmp2BE=_stmttmp31;_tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;{
# 1344
struct _tuple15 _stmttmp32=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BF,_tmp1A3,_tmp2C0);union Cyc_CfFlowInfo_FlowInfo _tmp2C2;void*_tmp2C3;struct _tuple15 _tmp2C1=_stmttmp32;_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;{
# 1347
void*_stmttmp33=Cyc_Tcutil_compress((void*)_check_null(_tmp1A3->topt));void*_tmp2C4=_stmttmp33;void*_tmp2C6;_LL1C7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4;if(_tmp2C5->tag != 5)goto _LL1C9;else{_tmp2C6=(_tmp2C5->f1).elt_typ;}}_LL1C8:
# 1349
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C6)){
# 1351
struct _tuple15 _tmp799;return(_tmp799.f1=_tmp2C2,((_tmp799.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp799)));}
# 1353
if(Cyc_Absyn_is_require_union_type(_tmp2C6))
Cyc_NewControlFlow_check_requires(_tmp1A3->loc,_tmp144->r,_tmp2C6,_tmp1A4,_tmp2BF);{
# 1356
void*_tmp2C8=_tmp2C3;int _tmp2CB;int _tmp2CC;struct _dyneither_ptr _tmp2CD;int _tmp2CF;int _tmp2D0;struct _dyneither_ptr _tmp2D1;_LL1CC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2C8;if(_tmp2C9->tag != 8)goto _LL1CE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2C9->f2);if(_tmp2CA->tag != 6)goto _LL1CE;else{_tmp2CB=(_tmp2CA->f1).is_union;_tmp2CC=(_tmp2CA->f1).fieldnum;_tmp2CD=_tmp2CA->f2;}}}_LL1CD:
# 1358
 _tmp2CF=_tmp2CB;_tmp2D0=_tmp2CC;_tmp2D1=_tmp2CD;goto _LL1CF;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CE=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C8;if(_tmp2CE->tag != 6)goto _LL1D0;else{_tmp2CF=(_tmp2CE->f1).is_union;_tmp2D0=(_tmp2CE->f1).fieldnum;_tmp2D1=_tmp2CE->f2;}}_LL1CF: {
# 1360
int _tmp2D2=Cyc_CfFlowInfo_get_field_index(_tmp2C6,_tmp1A4);
if((_tmp2CF  && _tmp2D0 != - 1) && _tmp2D0 != _tmp2D2){
struct _tuple15 _tmp79A;return(_tmp79A.f1=_tmp2C2,((_tmp79A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),1,_tmp144->unknown_none),_tmp79A)));}{
struct _tuple15 _tmp79B;return(_tmp79B.f1=_tmp2C2,((_tmp79B.f2=*((void**)_check_dyneither_subscript(_tmp2D1,sizeof(void*),_tmp2D2)),_tmp79B)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp7A1;const char*_tmp7A0;struct Cyc_Core_Impossible_exn_struct*_tmp79F;(int)_throw((void*)((_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F[0]=((_tmp7A1.tag=Cyc_Core_Impossible,((_tmp7A1.f1=((_tmp7A0="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7A0,sizeof(char),21))),_tmp7A1)))),_tmp79F)))));}_LL1CB:;};_LL1C9:;_LL1CA: {
# 1366
struct Cyc_Core_Impossible_exn_struct _tmp7A7;const char*_tmp7A6;struct Cyc_Core_Impossible_exn_struct*_tmp7A5;(int)_throw((void*)((_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A7.tag=Cyc_Core_Impossible,((_tmp7A7.f1=((_tmp7A6="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7A6,sizeof(char),25))),_tmp7A7)))),_tmp7A5)))));}_LL1C6:;};};}_LL140: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1A5->tag != 5)goto _LL142;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;_tmp1A8=_tmp1A5->f3;}}_LL141: {
# 1370
struct _tuple18 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A6);union Cyc_CfFlowInfo_FlowInfo _tmp2DC;union Cyc_CfFlowInfo_FlowInfo _tmp2DD;struct _tuple18 _tmp2DB=_stmttmp30;_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;{
struct _tuple15 _tmp2DE=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DC,_tmp1A7);
struct _tuple15 _tmp2DF=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DD,_tmp1A8);
# 1374
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp144,env->all_changed,_tmp2DE,_tmp2DF);};}_LL142: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1A9->tag != 6)goto _LL144;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LL143: {
# 1377
struct _tuple18 _stmttmp2E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AA);union Cyc_CfFlowInfo_FlowInfo _tmp2E1;union Cyc_CfFlowInfo_FlowInfo _tmp2E2;struct _tuple18 _tmp2E0=_stmttmp2E;_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;{
struct _tuple15 _stmttmp2F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E1,_tmp1AB);union Cyc_CfFlowInfo_FlowInfo _tmp2E4;void*_tmp2E5;struct _tuple15 _tmp2E3=_stmttmp2F;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;{
struct _tuple15 _tmp7A8;struct _tuple15 _tmp2E6=(_tmp7A8.f1=_tmp2E4,((_tmp7A8.f2=_tmp2E5,_tmp7A8)));
struct _tuple15 _tmp7A9;struct _tuple15 _tmp2E7=(_tmp7A9.f1=_tmp2E2,((_tmp7A9.f2=_tmp144->zero,_tmp7A9)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp144,env->all_changed,_tmp2E6,_tmp2E7);};};}_LL144: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1AC->tag != 7)goto _LL146;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL145: {
# 1384
struct _tuple18 _stmttmp2C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AD);union Cyc_CfFlowInfo_FlowInfo _tmp2EB;union Cyc_CfFlowInfo_FlowInfo _tmp2EC;struct _tuple18 _tmp2EA=_stmttmp2C;_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;{
struct _tuple15 _stmttmp2D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EC,_tmp1AE);union Cyc_CfFlowInfo_FlowInfo _tmp2EE;void*_tmp2EF;struct _tuple15 _tmp2ED=_stmttmp2D;_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{
struct _tuple15 _tmp7AA;struct _tuple15 _tmp2F0=(_tmp7AA.f1=_tmp2EE,((_tmp7AA.f2=_tmp2EF,_tmp7AA)));
struct _tuple15 _tmp7AB;struct _tuple15 _tmp2F1=(_tmp7AB.f1=_tmp2EB,((_tmp7AB.f2=_tmp144->notzeroall,_tmp7AB)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp144,env->all_changed,_tmp2F0,_tmp2F1);};};}_LL146: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1AF->tag != 22)goto _LL148;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL147: {
# 1391
struct _RegionHandle _tmp2F4=_new_region("temp");struct _RegionHandle*temp=& _tmp2F4;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7AC[2];struct _tuple22 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7AC[1]=_tmp1B1,((_tmp7AC[0]=_tmp1B0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7AC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1392
union Cyc_CfFlowInfo_FlowInfo _tmp2F6;struct Cyc_List_List*_tmp2F7;struct _tuple22 _tmp2F5=_stmttmp28;_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;{
# 1396
union Cyc_CfFlowInfo_FlowInfo _tmp2F8=_tmp2F6;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F9=_tmp2F6;struct Cyc_Dict_Dict _tmp2FA;struct Cyc_List_List*_tmp2FB;_LL1D3: if((_tmp2F9.ReachableFL).tag != 2)goto _LL1D5;_tmp2FA=((struct _tuple14)(_tmp2F9.ReachableFL).val).f1;_tmp2FB=((struct _tuple14)(_tmp2F9.ReachableFL).val).f2;_LL1D4:
# 1401
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2FA,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F7))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(_tmp1B1->loc,((_tmp7AF="expression may not be initialized",_tag_dyneither(_tmp7AF,sizeof(char),34))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2FE=Cyc_NewControlFlow_add_subscript_reln(_tmp144->r,_tmp2FB,_tmp1B0,_tmp1B1);
if(_tmp2FB != _tmp2FE)
_tmp2F8=Cyc_CfFlowInfo_ReachableFL(_tmp2FA,_tmp2FE);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1409
void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp1B0->topt));void*_tmp2FF=_stmttmp29;struct Cyc_List_List*_tmp301;union Cyc_Absyn_Constraint*_tmp303;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FF;if(_tmp300->tag != 10)goto _LL1DA;else{_tmp301=_tmp300->f1;}}_LL1D9: {
# 1411
void*_stmttmp2B=(void*)((struct Cyc_List_List*)_check_null(_tmp2F7))->hd;void*_tmp304=_stmttmp2B;struct _dyneither_ptr _tmp307;struct _dyneither_ptr _tmp309;_LL1DF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp305=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp304;if(_tmp305->tag != 8)goto _LL1E1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp306=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp305->f2);if(_tmp306->tag != 6)goto _LL1E1;else{_tmp307=_tmp306->f2;}}}_LL1E0:
# 1413
 _tmp309=_tmp307;goto _LL1E2;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp308=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp304;if(_tmp308->tag != 6)goto _LL1E3;else{_tmp309=_tmp308->f2;}}_LL1E2: {
# 1415
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1B1)).f1;
struct _tuple15 _tmp7B0;struct _tuple15 _tmp30B=(_tmp7B0.f1=_tmp2F8,((_tmp7B0.f2=*((void**)_check_dyneither_subscript(_tmp309,sizeof(void*),(int)i)),_tmp7B0)));_npop_handler(0);return _tmp30B;}_LL1E3:;_LL1E4: {
struct Cyc_Core_Impossible_exn_struct _tmp7B6;const char*_tmp7B5;struct Cyc_Core_Impossible_exn_struct*_tmp7B4;(int)_throw((void*)((_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B6.tag=Cyc_Core_Impossible,((_tmp7B6.f1=((_tmp7B5="anal_Rexp: Subscript",_tag_dyneither(_tmp7B5,sizeof(char),21))),_tmp7B6)))),_tmp7B4)))));}_LL1DE:;}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF;if(_tmp302->tag != 5)goto _LL1DC;else{_tmp303=((_tmp302->f1).ptr_atts).bounds;}}_LL1DB: {
# 1420
struct _tuple15 _stmttmp2A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F6,_tmp1B0,(void*)((struct Cyc_List_List*)_check_null(_tmp2F7))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp310;void*_tmp311;struct _tuple15 _tmp30F=_stmttmp2A;_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp312=_tmp310;_LL1E6: if((_tmp312.BottomFL).tag != 1)goto _LL1E8;_LL1E7: {
struct _tuple15 _tmp7B7;struct _tuple15 _tmp314=(_tmp7B7.f1=_tmp310,((_tmp7B7.f2=_tmp311,_tmp7B7)));_npop_handler(0);return _tmp314;}_LL1E8:;_LL1E9: {
struct _tuple15 _tmp7B8;struct _tuple15 _tmp316=(_tmp7B8.f1=_tmp2F8,((_tmp7B8.f2=_tmp311,_tmp7B8)));_npop_handler(0);return _tmp316;}_LL1E5:;};}_LL1DC:;_LL1DD: {
# 1425
struct Cyc_Core_Impossible_exn_struct _tmp7BE;const char*_tmp7BD;struct Cyc_Core_Impossible_exn_struct*_tmp7BC;(int)_throw((void*)((_tmp7BC=_cycalloc(sizeof(*_tmp7BC)),((_tmp7BC[0]=((_tmp7BE.tag=Cyc_Core_Impossible,((_tmp7BE.f1=((_tmp7BD="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7BD,sizeof(char),33))),_tmp7BE)))),_tmp7BC)))));}_LL1D7:;};};}
# 1392
;_pop_region(temp);}_LL148: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1B2->tag != 30)goto _LL14A;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL149:
# 1430
 _tmp1B6=_tmp1B3;goto _LL14B;_LL14A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1B5->tag != 23)goto _LL14C;else{_tmp1B6=_tmp1B5->f1;}}_LL14B: {
# 1432
struct _RegionHandle _tmp31B=_new_region("temp");struct _RegionHandle*temp=& _tmp31B;_push_region(temp);
{struct _tuple22 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1B6,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp31D;struct Cyc_List_List*_tmp31E;struct _tuple22 _tmp31C=_stmttmp27;_tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;{
unsigned int _tmp7C4;unsigned int _tmp7C3;struct _dyneither_ptr _tmp7C2;void**_tmp7C1;unsigned int _tmp7C0;struct _dyneither_ptr aggrdict=
(_tmp7C0=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B6),((_tmp7C1=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7C0)),((_tmp7C2=_tag_dyneither(_tmp7C1,sizeof(void*),_tmp7C0),((((_tmp7C3=_tmp7C0,_tmp7C5(& _tmp7C4,& _tmp7C3,& _tmp7C1,& _tmp31E))),_tmp7C2)))))));
# 1440
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7CF;struct Cyc_CfFlowInfo_UnionRInfo _tmp7CE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7CD;struct _tuple15 _tmp7CC;struct _tuple15 _tmp323=(_tmp7CC.f1=_tmp31D,((_tmp7CC.f2=(void*)((_tmp7CF=_region_malloc(env->r,sizeof(*_tmp7CF)),((_tmp7CF[0]=((_tmp7CD.tag=6,((_tmp7CD.f1=((_tmp7CE.is_union=0,((_tmp7CE.fieldnum=- 1,_tmp7CE)))),((_tmp7CD.f2=aggrdict,_tmp7CD)))))),_tmp7CF)))),_tmp7CC)));_npop_handler(0);return _tmp323;};}
# 1433
;_pop_region(temp);}_LL14C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1B7->tag != 29)goto _LL14E;else{_tmp1B8=_tmp1B7->f2;}}_LL14D: {
# 1443
struct Cyc_List_List*fs;
{void*_stmttmp26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp329=_stmttmp26;struct Cyc_List_List*_tmp32B;_LL1EB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp329;if(_tmp32A->tag != 12)goto _LL1ED;else{_tmp32B=_tmp32A->f2;}}_LL1EC:
 fs=_tmp32B;goto _LL1EA;_LL1ED:;_LL1EE: {
struct Cyc_Core_Impossible_exn_struct _tmp7D5;const char*_tmp7D4;struct Cyc_Core_Impossible_exn_struct*_tmp7D3;(int)_throw((void*)((_tmp7D3=_cycalloc(sizeof(*_tmp7D3)),((_tmp7D3[0]=((_tmp7D5.tag=Cyc_Core_Impossible,((_tmp7D5.f1=((_tmp7D4="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7D4,sizeof(char),35))),_tmp7D5)))),_tmp7D3)))));}_LL1EA:;}
# 1448
_tmp1BA=_tmp1B8;_tmp1BB=Cyc_Absyn_StructA;_tmp1BC=fs;goto _LL14F;}_LL14E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1B9=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1B9->tag != 28)goto _LL150;else{_tmp1BA=_tmp1B9->f3;if(_tmp1B9->f4 == 0)goto _LL150;_tmp1BB=(_tmp1B9->f4)->kind;if((_tmp1B9->f4)->impl == 0)goto _LL150;_tmp1BC=((_tmp1B9->f4)->impl)->fields;}}_LL14F: {
# 1450
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp32F=_new_region("temp");struct _RegionHandle*temp=& _tmp32F;_push_region(temp);
{struct _tuple22 _stmttmp24=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BA),1,1);
# 1452
union Cyc_CfFlowInfo_FlowInfo _tmp331;struct Cyc_List_List*_tmp332;struct _tuple22 _tmp330=_stmttmp24;_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;{
# 1454
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp144,_tmp1BC,_tmp1BB == Cyc_Absyn_UnionA,_tmp144->unknown_all);
# 1457
int field_no=-1;
{int i=0;for(0;_tmp332 != 0;(((_tmp332=_tmp332->tl,_tmp1BA=_tmp1BA->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1BA))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp25=(void*)ds->hd;void*_tmp333=_stmttmp25;struct _dyneither_ptr*_tmp336;_LL1F0: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp334=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp333;if(_tmp334->tag != 0)goto _LL1F2;}_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7DB;const char*_tmp7DA;struct Cyc_Core_Impossible_exn_struct*_tmp7D9;(int)_throw((void*)((_tmp7D9=_cycalloc(sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DB.tag=Cyc_Core_Impossible,((_tmp7DB.f1=((_tmp7DA="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7DA,sizeof(char),22))),_tmp7DB)))),_tmp7D9)))));}_LL1F2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp335=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp333;if(_tmp335->tag != 1)goto _LL1EF;else{_tmp336=_tmp335->f1;}}_LL1F3:
# 1464
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1BC,_tmp336);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp332->hd;
# 1467
if(_tmp1BB == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp33A=(*((struct _tuple25*)_tmp1BA->hd)).f2;
_tmp331=Cyc_NewControlFlow_use_Rval(env,_tmp33A->loc,_tmp331,(void*)_tmp332->hd);
# 1471
Cyc_NewControlFlow_check_requires(_tmp33A->loc,_tmp144->r,exp_type,_tmp336,_tmp331);}_LL1EF:;}}}{
# 1474
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7E1;struct Cyc_CfFlowInfo_UnionRInfo _tmp7E0;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7DF;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33B=(_tmp7DF=_region_malloc(env->r,sizeof(*_tmp7DF)),((_tmp7DF[0]=((_tmp7E1.tag=6,((_tmp7E1.f1=((_tmp7E0.is_union=_tmp1BB == Cyc_Absyn_UnionA,((_tmp7E0.fieldnum=field_no,_tmp7E0)))),((_tmp7E1.f2=aggrdict,_tmp7E1)))))),_tmp7DF)));
struct _tuple15 _tmp7E2;struct _tuple15 _tmp33D=(_tmp7E2.f1=_tmp331,((_tmp7E2.f2=(void*)_tmp33B,_tmp7E2)));_npop_handler(0);return _tmp33D;};};}
# 1452
;_pop_region(temp);}_LL150: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1BD->tag != 28)goto _LL152;}_LL151: {
# 1478
struct Cyc_Core_Impossible_exn_struct _tmp7E8;const char*_tmp7E7;struct Cyc_Core_Impossible_exn_struct*_tmp7E6;(int)_throw((void*)((_tmp7E6=_cycalloc(sizeof(*_tmp7E6)),((_tmp7E6[0]=((_tmp7E8.tag=Cyc_Core_Impossible,((_tmp7E8.f1=((_tmp7E7="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7E7,sizeof(char),31))),_tmp7E8)))),_tmp7E6)))));}_LL152: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1BE->tag != 25)goto _LL154;else{_tmp1BF=_tmp1BE->f1;}}_LL153: {
# 1480
struct _RegionHandle _tmp344=_new_region("temp");struct _RegionHandle*temp=& _tmp344;_push_region(temp);
{struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BF);
struct _tuple22 _stmttmp23=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp345,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp347;struct Cyc_List_List*_tmp348;struct _tuple22 _tmp346=_stmttmp23;_tmp347=_tmp346.f1;_tmp348=_tmp346.f2;
for(0;_tmp348 != 0;(_tmp348=_tmp348->tl,_tmp345=_tmp345->tl)){
_tmp347=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp345))->hd)->loc,_tmp347,(void*)_tmp348->hd);}{
struct _tuple15 _tmp7E9;struct _tuple15 _tmp34A=(_tmp7E9.f1=_tmp347,((_tmp7E9.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,(void*)_check_null(e->topt),0,_tmp144->unknown_all),_tmp7E9)));_npop_handler(0);return _tmp34A;};}
# 1481
;_pop_region(temp);}_LL154: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1C0->tag != 26)goto _LL156;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;_tmp1C3=_tmp1C0->f3;_tmp1C4=_tmp1C0->f4;}}_LL155: {
# 1489
struct _tuple15 _stmttmp20=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C2);union Cyc_CfFlowInfo_FlowInfo _tmp34C;void*_tmp34D;struct _tuple15 _tmp34B=_stmttmp20;_tmp34C=_tmp34B.f1;_tmp34D=_tmp34B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp34E=_tmp34C;struct Cyc_Dict_Dict _tmp34F;struct Cyc_List_List*_tmp350;_LL1F5: if((_tmp34E.BottomFL).tag != 1)goto _LL1F7;_LL1F6: {
struct _tuple15 _tmp7EA;return(_tmp7EA.f1=_tmp34C,((_tmp7EA.f2=_tmp144->unknown_all,_tmp7EA)));}_LL1F7: if((_tmp34E.ReachableFL).tag != 2)goto _LL1F4;_tmp34F=((struct _tuple14)(_tmp34E.ReachableFL).val).f1;_tmp350=((struct _tuple14)(_tmp34E.ReachableFL).val).f2;_LL1F8:
# 1493
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp34F,_tmp34D)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_Tcutil_terr(_tmp1C2->loc,((_tmp7ED="expression may not be initialized",_tag_dyneither(_tmp7ED,sizeof(char),34))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}{
# 1497
struct Cyc_List_List*new_relns=_tmp350;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1C1);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1C2,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp350);
# 1503
if(_tmp350 != new_relns)
_tmp34C=Cyc_CfFlowInfo_ReachableFL(_tmp34F,new_relns);{
# 1507
void*_tmp354=_tmp34D;_LL1FA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp355=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp354;if(_tmp355->tag != 0)goto _LL1FC;}_LL1FB: {
struct _tuple15 _tmp7EE;return(_tmp7EE.f1=_tmp34C,((_tmp7EE.f2=_tmp144->unknown_all,_tmp7EE)));}_LL1FC: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp356=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp354;if(_tmp356->tag != 2)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp357=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp354;if(_tmp357->tag != 1)goto _LL200;}_LL1FF:
 goto _LL201;_LL200: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp358=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp354;if(_tmp358->tag != 5)goto _LL202;}_LL201: {
# 1512
struct Cyc_List_List _tmp7EF;struct Cyc_List_List _tmp35A=(_tmp7EF.hd=_tmp1C1,((_tmp7EF.tl=0,_tmp7EF)));
_tmp34C=Cyc_NewControlFlow_add_vars(_tmp144,_tmp34C,& _tmp35A,_tmp144->unknown_all,e->loc,0);{
# 1515
struct _tuple15 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34C,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp35C;void*_tmp35D;struct _tuple15 _tmp35B=_stmttmp22;_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp35E=_tmp35C;struct Cyc_Dict_Dict _tmp35F;_LL205: if((_tmp35E.BottomFL).tag != 1)goto _LL207;_LL206: {
struct _tuple15 _tmp7F0;return(_tmp7F0.f1=_tmp35C,((_tmp7F0.f2=_tmp144->unknown_all,_tmp7F0)));}_LL207: if((_tmp35E.ReachableFL).tag != 2)goto _LL204;_tmp35F=((struct _tuple14)(_tmp35E.ReachableFL).val).f1;_LL208:
# 1519
 if(Cyc_CfFlowInfo_initlevel(_tmp144,_tmp35F,_tmp35D)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_Tcutil_terr(_tmp1C2->loc,((_tmp7F3="expression may not be initialized",_tag_dyneither(_tmp7F3,sizeof(char),34))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}{
struct _tuple15 _tmp7F4;return(_tmp7F4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F4.f2=_tmp144->unknown_all,_tmp7F4)));};}_LL204:;}
# 1524
_tmp34C=_tmp35C;
goto _LL203;};}_LL202:;_LL203:
# 1527
 while(1){
struct Cyc_List_List _tmp7F5;struct Cyc_List_List _tmp365=(_tmp7F5.hd=_tmp1C1,((_tmp7F5.tl=0,_tmp7F5)));
_tmp34C=Cyc_NewControlFlow_add_vars(_tmp144,_tmp34C,& _tmp365,_tmp144->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp21=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34C,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp367;void*_tmp368;struct _tuple15 _tmp366=_stmttmp21;_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp369=_tmp367;struct Cyc_Dict_Dict _tmp36A;_LL20A: if((_tmp369.BottomFL).tag != 1)goto _LL20C;_LL20B:
 goto _LL209;_LL20C: if((_tmp369.ReachableFL).tag != 2)goto _LL209;_tmp36A=((struct _tuple14)(_tmp369.ReachableFL).val).f1;_LL20D:
# 1534
 if(Cyc_CfFlowInfo_initlevel(_tmp144,_tmp36A,_tmp368)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F8;void*_tmp7F7;(_tmp7F7=0,Cyc_Tcutil_terr(_tmp1C2->loc,((_tmp7F8="expression may not be initialized",_tag_dyneither(_tmp7F8,sizeof(char),34))),_tag_dyneither(_tmp7F7,sizeof(void*),0)));}{
struct _tuple15 _tmp7F9;return(_tmp7F9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F9.f2=_tmp144->unknown_all,_tmp7F9)));};}_LL209:;}{
# 1539
union Cyc_CfFlowInfo_FlowInfo _tmp36E=Cyc_CfFlowInfo_join_flow(_tmp144,env->all_changed,_tmp34C,_tmp367);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp36E,_tmp34C))
break;
_tmp34C=_tmp36E;};};}{
# 1544
struct _tuple15 _tmp7FA;return(_tmp7FA.f1=_tmp34C,((_tmp7FA.f2=_tmp144->unknown_all,_tmp7FA)));};_LL1F9:;};};_LL1F4:;};}_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1C5->tag != 27)goto _LL158;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=(void*)_tmp1C5->f2;_tmp1C8=_tmp1C5->f3;}}_LL157: {
# 1550
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7FD;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7FC;void*root=(void*)((_tmp7FC=_region_malloc(_tmp144->r,sizeof(*_tmp7FC)),((_tmp7FC[0]=((_tmp7FD.tag=1,((_tmp7FD.f1=_tmp1C6,((_tmp7FD.f2=(void*)_check_null(e->topt),_tmp7FD)))))),_tmp7FC))));
struct Cyc_CfFlowInfo_Place*_tmp7FE;struct Cyc_CfFlowInfo_Place*place=(_tmp7FE=_region_malloc(_tmp144->r,sizeof(*_tmp7FE)),((_tmp7FE->root=root,((_tmp7FE->fields=0,_tmp7FE)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp801;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp800;void*rval=(void*)((_tmp800=_region_malloc(_tmp144->r,sizeof(*_tmp800)),((_tmp800[0]=((_tmp801.tag=5,((_tmp801.f1=place,_tmp801)))),_tmp800))));
void*place_val;
# 1558
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp144,_tmp1C7,0,_tmp144->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp372;struct _tuple15 _tmp371=_stmttmp1F;_tmp372=_tmp371.f1;
outflow=_tmp372;{
union Cyc_CfFlowInfo_FlowInfo _tmp373=outflow;struct Cyc_Dict_Dict _tmp374;struct Cyc_List_List*_tmp375;_LL20F: if((_tmp373.BottomFL).tag != 1)goto _LL211;_LL210: {
struct _tuple15 _tmp802;return(_tmp802.f1=outflow,((_tmp802.f2=rval,_tmp802)));}_LL211: if((_tmp373.ReachableFL).tag != 2)goto _LL20E;_tmp374=((struct _tuple14)(_tmp373.ReachableFL).val).f1;_tmp375=((struct _tuple14)(_tmp373.ReachableFL).val).f2;_LL212: {
# 1566
struct _tuple15 _tmp803;return(_tmp803.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp374,root,place_val),_tmp375),((_tmp803.f2=rval,_tmp803)));}_LL20E:;};};};}_LL158: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1C9->tag != 36)goto _LL15A;else{_tmp1CA=_tmp1C9->f1;}}_LL159: {
# 1570
struct _tuple16 _tmp804;struct _tuple16 _tmp37D=(_tmp804.f1=(env->fenv)->unknown_all,((_tmp804.f2=copy_ctxt,_tmp804)));
union Cyc_CfFlowInfo_FlowInfo _tmp37E=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1CA,& _tmp37D);
struct _tuple15 _tmp805;return(_tmp805.f1=_tmp37E,((_tmp805.f2=_tmp37D.f1,_tmp805)));}_LL15A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1CB->tag != 1)goto _LL15C;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1CC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1CB->f2);if(_tmp1CC->tag != 0)goto _LL15C;}}_LL15B:
# 1574
 goto _LL15D;_LL15C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1CD->tag != 35)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1CE->tag != 24)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1CF=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1CF->tag != 38)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp1D0->tag != 39)goto _LLFB;}_LL163: {
# 1579
struct Cyc_Core_Impossible_exn_struct _tmp80B;const char*_tmp80A;struct Cyc_Core_Impossible_exn_struct*_tmp809;(int)_throw((void*)((_tmp809=_cycalloc(sizeof(*_tmp809)),((_tmp809[0]=((_tmp80B.tag=Cyc_Core_Impossible,((_tmp80B.f1=((_tmp80A="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp80A,sizeof(char),31))),_tmp80B)))),_tmp809)))));}_LLFB:;};}
# 1589
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1597
struct Cyc_CfFlowInfo_FlowEnv*_tmp384=env->fenv;
void*_stmttmp51=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp385=_stmttmp51;void*_tmp387;union Cyc_Absyn_Constraint*_tmp388;union Cyc_Absyn_Constraint*_tmp389;_LL214: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp385;if(_tmp386->tag != 5)goto _LL216;else{_tmp387=(_tmp386->f1).elt_typ;_tmp388=((_tmp386->f1).ptr_atts).bounds;_tmp389=((_tmp386->f1).ptr_atts).zero_term;}}_LL215: {
# 1600
union Cyc_CfFlowInfo_FlowInfo _tmp38A=f;struct Cyc_Dict_Dict _tmp38B;struct Cyc_List_List*_tmp38C;_LL219: if((_tmp38A.BottomFL).tag != 1)goto _LL21B;_LL21A: {
struct _tuple17 _tmp80C;return(_tmp80C.f1=f,((_tmp80C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp80C)));}_LL21B: if((_tmp38A.ReachableFL).tag != 2)goto _LL218;_tmp38B=((struct _tuple14)(_tmp38A.ReachableFL).val).f1;_tmp38C=((struct _tuple14)(_tmp38A.ReachableFL).val).f2;_LL21C: {
# 1604
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp388)){
retry: {void*_tmp38E=r;struct Cyc_Absyn_Vardecl*_tmp390;void*_tmp391;void*_tmp395;struct Cyc_List_List*_tmp396;enum Cyc_CfFlowInfo_InitLevel _tmp399;_LL21E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp38F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp38F->tag != 8)goto _LL220;else{_tmp390=_tmp38F->f1;_tmp391=(void*)_tmp38F->f2;}}_LL21F:
 r=_tmp391;locname=_tmp390;goto retry;_LL220: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp392=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp392->tag != 1)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp393=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp393->tag != 2)goto _LL224;}_LL223:
# 1610
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp80F;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80E;e->annot=(void*)((_tmp80E=_cycalloc(sizeof(*_tmp80E)),((_tmp80E[0]=((_tmp80F.tag=Cyc_CfFlowInfo_NotZero,((_tmp80F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38C),_tmp80F)))),_tmp80E))));}goto _LL21D;_LL224: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp394=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp394->tag != 5)goto _LL226;else{_tmp395=(_tmp394->f1)->root;_tmp396=(_tmp394->f1)->fields;}}_LL225:
# 1612
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp812;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp811;e->annot=(void*)((_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811[0]=((_tmp812.tag=Cyc_CfFlowInfo_NotZero,((_tmp812.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38C),_tmp812)))),_tmp811))));}{
struct Cyc_CfFlowInfo_Place*_tmp815;struct _tuple17 _tmp814;return(_tmp814.f1=f,((_tmp814.f2=Cyc_CfFlowInfo_PlaceL(((_tmp815=_region_malloc(_tmp384->r,sizeof(*_tmp815)),((_tmp815->root=_tmp395,((_tmp815->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp384->r,_tmp396,flds),_tmp815))))))),_tmp814)));};_LL226: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp397=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp397->tag != 0)goto _LL228;}_LL227:
# 1615
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp816;return(_tmp816.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp816.f2=Cyc_CfFlowInfo_UnknownL(),_tmp816)));};_LL228: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp398=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38E;if(_tmp398->tag != 3)goto _LL22A;else{_tmp399=_tmp398->f1;}}_LL229:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp399,locname);goto _LL22B;_LL22A:;_LL22B:
# 1620
 if(passthrough_consumes){
void*_tmp3A1=r;void*_tmp3A3;_LL22D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A1;if(_tmp3A2->tag != 7)goto _LL22F;else{_tmp3A3=(void*)_tmp3A2->f3;}}_LL22E:
 r=_tmp3A3;goto retry;_LL22F:;_LL230:
 goto _LL22C;_LL22C:;}{
# 1626
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp819;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp818;e->annot=(void*)((_tmp818=_cycalloc(sizeof(*_tmp818)),((_tmp818[0]=((_tmp819.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp819.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38C),_tmp819)))),_tmp818))));};_LL21D:;}}else{
# 1629
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp81B;e->annot=(void*)((_tmp81B=_cycalloc(sizeof(*_tmp81B)),((_tmp81B[0]=((_tmp81C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38C),_tmp81C)))),_tmp81B))));}
if(Cyc_CfFlowInfo_initlevel(_tmp384,_tmp38B,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp81F;void*_tmp81E;(_tmp81E=0,Cyc_Tcutil_terr(e->loc,((_tmp81F="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp81F,sizeof(char),46))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}{
struct _tuple17 _tmp820;return(_tmp820.f1=f,((_tmp820.f2=Cyc_CfFlowInfo_UnknownL(),_tmp820)));};}_LL218:;}_LL216:;_LL217: {
# 1634
struct Cyc_Core_Impossible_exn_struct _tmp826;const char*_tmp825;struct Cyc_Core_Impossible_exn_struct*_tmp824;(int)_throw((void*)((_tmp824=_cycalloc(sizeof(*_tmp824)),((_tmp824[0]=((_tmp826.tag=Cyc_Core_Impossible,((_tmp826.f1=((_tmp825="left deref of non-pointer-type",_tag_dyneither(_tmp825,sizeof(char),31))),_tmp826)))),_tmp824)))));}_LL213:;}
# 1642
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1645
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3AE=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3AF=inflow;struct Cyc_Dict_Dict _tmp3B0;struct Cyc_List_List*_tmp3B1;_LL232: if((_tmp3AF.BottomFL).tag != 1)goto _LL234;_LL233: {
struct _tuple17 _tmp827;return(_tmp827.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp827.f2=Cyc_CfFlowInfo_UnknownL(),_tmp827)));}_LL234: if((_tmp3AF.ReachableFL).tag != 2)goto _LL231;_tmp3B0=((struct _tuple14)(_tmp3AF.ReachableFL).val).f1;_tmp3B1=((struct _tuple14)(_tmp3AF.ReachableFL).val).f2;_LL235:
# 1650
 d=_tmp3B0;_LL231:;}{
# 1654
void*_stmttmp52=e->r;void*_tmp3B3=_stmttmp52;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BC;struct Cyc_Absyn_Vardecl*_tmp3BF;struct Cyc_Absyn_Vardecl*_tmp3C2;struct Cyc_Absyn_Vardecl*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;struct _dyneither_ptr*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct _dyneither_ptr*_tmp3D0;_LL237: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3B4->tag != 13)goto _LL239;else{_tmp3B5=_tmp3B4->f2;}}_LL238:
 _tmp3B7=_tmp3B5;goto _LL23A;_LL239: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3B6->tag != 11)goto _LL23B;else{_tmp3B7=_tmp3B6->f1;}}_LL23A:
 _tmp3B9=_tmp3B7;goto _LL23C;_LL23B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3B8->tag != 12)goto _LL23D;else{_tmp3B9=_tmp3B8->f1;}}_LL23C:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3B9,flds);_LL23D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3BA->tag != 1)goto _LL23F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3BB=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3BA->f2);if(_tmp3BB->tag != 3)goto _LL23F;else{_tmp3BC=_tmp3BB->f1;}}}_LL23E:
# 1659
 _tmp3BF=_tmp3BC;goto _LL240;_LL23F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3BD->tag != 1)goto _LL241;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3BE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3BD->f2);if(_tmp3BE->tag != 4)goto _LL241;else{_tmp3BF=_tmp3BE->f1;}}}_LL240:
 _tmp3C2=_tmp3BF;goto _LL242;_LL241: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3C0->tag != 1)goto _LL243;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3C1=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3C0->f2);if(_tmp3C1->tag != 5)goto _LL243;else{_tmp3C2=_tmp3C1->f1;}}}_LL242: {
# 1662
struct Cyc_CfFlowInfo_Place*_tmp831;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp830;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp82F;struct _tuple17 _tmp82E;return(_tmp82E.f1=inflow,((_tmp82E.f2=Cyc_CfFlowInfo_PlaceL(((_tmp831=_region_malloc(env->r,sizeof(*_tmp831)),((_tmp831->root=(void*)((_tmp82F=_region_malloc(env->r,sizeof(*_tmp82F)),((_tmp82F[0]=((_tmp830.tag=0,((_tmp830.f1=_tmp3C2,_tmp830)))),_tmp82F)))),((_tmp831->fields=flds,_tmp831))))))),_tmp82E)));}_LL243: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3C3->tag != 1)goto _LL245;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3C4=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3C3->f2);if(_tmp3C4->tag != 1)goto _LL245;else{_tmp3C5=_tmp3C4->f1;}}}_LL244: {
# 1664
struct _tuple17 _tmp832;return(_tmp832.f1=inflow,((_tmp832.f2=Cyc_CfFlowInfo_UnknownL(),_tmp832)));}_LL245: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3C6->tag != 21)goto _LL247;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}}_LL246:
# 1667
{void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3C7->topt));void*_tmp3D6=_stmttmp59;void*_tmp3D8;_LL250: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag != 5)goto _LL252;else{_tmp3D8=(_tmp3D7->f1).elt_typ;}}_LL251:
# 1669
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D8)){
Cyc_NewControlFlow_check_requires(_tmp3C7->loc,_tmp3AE->r,_tmp3D8,_tmp3C8,inflow);{
struct Cyc_List_List*_tmp833;flds=((_tmp833=_region_malloc(env->r,sizeof(*_tmp833)),((_tmp833->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3D8,_tmp3C8),((_tmp833->tl=flds,_tmp833))))));};}
# 1673
goto _LL24F;_LL252:;_LL253: {
struct Cyc_Core_Impossible_exn_struct _tmp839;const char*_tmp838;struct Cyc_Core_Impossible_exn_struct*_tmp837;(int)_throw((void*)((_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837[0]=((_tmp839.tag=Cyc_Core_Impossible,((_tmp839.f1=((_tmp838="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp838,sizeof(char),25))),_tmp839)))),_tmp837)))));}_LL24F:;}
# 1676
_tmp3CA=_tmp3C7;goto _LL248;_LL247: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3C9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3C9->tag != 19)goto _LL249;else{_tmp3CA=_tmp3C9->f1;}}_LL248:
# 1679
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3CA->topt),1)){
# 1681
struct _tuple17 _stmttmp56=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3CA->topt),1),passthrough_consumes,_tmp3CA);
# 1681
union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_AbsLVal _tmp3DF;struct _tuple17 _tmp3DD=_stmttmp56;_tmp3DE=_tmp3DD.f1;_tmp3DF=_tmp3DD.f2;{
# 1685
struct _tuple17 _tmp83A;struct _tuple17 _stmttmp57=(_tmp83A.f1=_tmp3DE,((_tmp83A.f2=_tmp3DF,_tmp83A)));struct _tuple17 _tmp3E0=_stmttmp57;struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_CfFlowInfo_Place*_tmp3E3;_LL255: if(((_tmp3E0.f1).ReachableFL).tag != 2)goto _LL257;_tmp3E1=((struct _tuple14)((_tmp3E0.f1).ReachableFL).val).f1;_tmp3E2=((struct _tuple14)((_tmp3E0.f1).ReachableFL).val).f2;if(((_tmp3E0.f2).PlaceL).tag != 1)goto _LL257;_tmp3E3=(struct Cyc_CfFlowInfo_Place*)((_tmp3E0.f2).PlaceL).val;_LL256: {
# 1687
void*_tmp3E4=Cyc_CfFlowInfo_lookup_place(_tmp3E1,_tmp3E3);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp3E5=_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E7;void*_tmp3E8;_LL25A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E5;if(_tmp3E6->tag != 8)goto _LL25C;else{_tmp3E7=_tmp3E6->f1;_tmp3E8=(void*)_tmp3E6->f2;}}_LL25B:
 _tmp3E4=_tmp3E8;locname=_tmp3E7;goto retry;_LL25C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3E9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E5;if(_tmp3E9->tag != 7)goto _LL25E;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3EA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp3E9->f3);if(_tmp3EA->tag != 5)goto _LL25E;}}_LL25D:
 goto _LL25F;_LL25E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3EB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3E5;if(_tmp3EB->tag != 5)goto _LL260;}_LL25F:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3DE,_tmp3CA,_tmp3DE,_tmp3E4,passthrough_consumes,flds);_LL260:;_LL261: {
# 1699
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3AE,_tmp3E1,_tmp3E4);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3AE->unknown_all: _tmp3AE->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3AE,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3CA->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp83D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp83C;void*new_root=(void*)((_tmp83C=_region_malloc(_tmp3AE->r,sizeof(*_tmp83C)),((_tmp83C[0]=((_tmp83D.tag=1,((_tmp83D.f1=e,((_tmp83D.f2=(void*)_check_null(e->topt),_tmp83D)))))),_tmp83C))));
struct Cyc_CfFlowInfo_Place*_tmp83E;struct Cyc_CfFlowInfo_Place*place=(_tmp83E=_region_malloc(_tmp3AE->r,sizeof(*_tmp83E)),((_tmp83E->root=new_root,((_tmp83E->fields=0,_tmp83E)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp841;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp840;void*res=(void*)((_tmp840=_region_malloc(_tmp3AE->r,sizeof(*_tmp840)),((_tmp840[0]=((_tmp841.tag=5,((_tmp841.f1=place,_tmp841)))),_tmp840))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp844;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp843;res=(void*)((_tmp843=_region_malloc(_tmp3AE->r,sizeof(*_tmp843)),((_tmp843[0]=((_tmp844.tag=8,((_tmp844.f1=locname,((_tmp844.f2=res,_tmp844)))))),_tmp843))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E1,new_root,new_rval);
_tmp3E1=Cyc_CfFlowInfo_assign_place(_tmp3AE,e->loc,_tmp3E1,env->all_changed,_tmp3E3,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3EE=Cyc_CfFlowInfo_ReachableFL(_tmp3E1,_tmp3E2);
# 1714
return Cyc_NewControlFlow_anal_derefL(env,_tmp3EE,_tmp3CA,_tmp3EE,res,passthrough_consumes,flds);};}_LL259:;}}_LL257:;_LL258:
# 1716
 goto _LL254;_LL254:;};}{
# 1719
struct _tuple15 _stmttmp58=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3CA);union Cyc_CfFlowInfo_FlowInfo _tmp3F6;void*_tmp3F7;struct _tuple15 _tmp3F5=_stmttmp58;_tmp3F6=_tmp3F5.f1;_tmp3F7=_tmp3F5.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CA,_tmp3F6,_tmp3F7,passthrough_consumes,flds);};_LL249: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3CB->tag != 22)goto _LL24B;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;}}_LL24A: {
# 1723
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(_tmp3CC->topt));void*_tmp3F8=_stmttmp53;union Cyc_Absyn_Constraint*_tmp3FB;_LL263: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F8;if(_tmp3F9->tag != 10)goto _LL265;}_LL264: {
# 1725
unsigned int _tmp3FC=(Cyc_Evexp_eval_const_uint_exp(_tmp3CD)).f1;
struct Cyc_List_List*_tmp845;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CC,((_tmp845=_region_malloc(env->r,sizeof(*_tmp845)),((_tmp845->hd=(void*)((int)_tmp3FC),((_tmp845->tl=flds,_tmp845)))))));}_LL265: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F8;if(_tmp3FA->tag != 5)goto _LL267;else{_tmp3FB=((_tmp3FA->f1).ptr_atts).bounds;}}_LL266: {
# 1728
struct _RegionHandle _tmp3FE=_new_region("temp");struct _RegionHandle*temp=& _tmp3FE;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp846[2];struct _tuple22 _stmttmp54=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp846[1]=_tmp3CD,((_tmp846[0]=_tmp3CC,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp846,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1729
union Cyc_CfFlowInfo_FlowInfo _tmp400;struct Cyc_List_List*_tmp401;struct _tuple22 _tmp3FF=_stmttmp54;_tmp400=_tmp3FF.f1;_tmp401=_tmp3FF.f2;{
# 1731
union Cyc_CfFlowInfo_FlowInfo _tmp402=_tmp400;
{union Cyc_CfFlowInfo_FlowInfo _tmp403=_tmp400;struct Cyc_Dict_Dict _tmp404;struct Cyc_List_List*_tmp405;_LL26A: if((_tmp403.ReachableFL).tag != 2)goto _LL26C;_tmp404=((struct _tuple14)(_tmp403.ReachableFL).val).f1;_tmp405=((struct _tuple14)(_tmp403.ReachableFL).val).f2;_LL26B:
# 1734
 if(Cyc_CfFlowInfo_initlevel(_tmp3AE,_tmp404,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp401))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp849;void*_tmp848;(_tmp848=0,Cyc_Tcutil_terr(_tmp3CD->loc,((_tmp849="expression may not be initialized",_tag_dyneither(_tmp849,sizeof(char),34))),_tag_dyneither(_tmp848,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp408=Cyc_NewControlFlow_add_subscript_reln(_tmp3AE->r,_tmp405,_tmp3CC,_tmp3CD);
if(_tmp405 != _tmp408)
_tmp402=Cyc_CfFlowInfo_ReachableFL(_tmp404,_tmp408);
goto _LL269;};_LL26C:;_LL26D:
 goto _LL269;_LL269:;}{
# 1742
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CC,_tmp400,(void*)((struct Cyc_List_List*)_check_null(_tmp401))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp40A;union Cyc_CfFlowInfo_AbsLVal _tmp40B;struct _tuple17 _tmp409=_stmttmp55;_tmp40A=_tmp409.f1;_tmp40B=_tmp409.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp40C=_tmp40A;_LL26F: if((_tmp40C.BottomFL).tag != 1)goto _LL271;_LL270: {
struct _tuple17 _tmp84A;struct _tuple17 _tmp40E=(_tmp84A.f1=_tmp40A,((_tmp84A.f2=_tmp40B,_tmp84A)));_npop_handler(0);return _tmp40E;}_LL271:;_LL272: {
struct _tuple17 _tmp84B;struct _tuple17 _tmp410=(_tmp84B.f1=_tmp402,((_tmp84B.f2=_tmp40B,_tmp84B)));_npop_handler(0);return _tmp410;}_LL26E:;};};};}
# 1729
;_pop_region(temp);}_LL267:;_LL268: {
# 1748
struct Cyc_Core_Impossible_exn_struct _tmp851;const char*_tmp850;struct Cyc_Core_Impossible_exn_struct*_tmp84F;(int)_throw((void*)((_tmp84F=_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp851.tag=Cyc_Core_Impossible,((_tmp851.f1=((_tmp850="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp850,sizeof(char),33))),_tmp851)))),_tmp84F)))));}_LL262:;}_LL24B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3CE->tag != 20)goto _LL24D;else{_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;}}_LL24C: {
# 1752
void*_tmp415=(void*)_check_null(_tmp3CF->topt);
if(Cyc_Absyn_is_require_union_type(_tmp415))
Cyc_NewControlFlow_check_requires(_tmp3CF->loc,_tmp3AE->r,_tmp415,_tmp3D0,inflow);
# 1756
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp415)){
struct _tuple17 _tmp852;return(_tmp852.f1=inflow,((_tmp852.f2=Cyc_CfFlowInfo_UnknownL(),_tmp852)));}{
# 1759
struct Cyc_List_List*_tmp853;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CF,(
(_tmp853=_region_malloc(env->r,sizeof(*_tmp853)),((_tmp853->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp415,_tmp3D0),((_tmp853->tl=flds,_tmp853)))))));};}_LL24D:;_LL24E: {
# 1762
struct _tuple17 _tmp854;return(_tmp854.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp854.f2=Cyc_CfFlowInfo_UnknownL(),_tmp854)));}_LL236:;};}
# 1766
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1769
struct _tuple17 _stmttmp5A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp41A;union Cyc_CfFlowInfo_AbsLVal _tmp41B;struct _tuple17 _tmp419=_stmttmp5A;_tmp41A=_tmp419.f1;_tmp41B=_tmp419.f2;{
struct _tuple17 _tmp855;return(_tmp855.f1=_tmp41A,((_tmp855.f2=_tmp41B,_tmp855)));};}
# 1776
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1778
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _stmttmp5B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp41E;struct _tuple17 _tmp41D=_stmttmp5B;_tmp41E=_tmp41D.f1;
inflow=_tmp41E;}}}
# 1785
return inflow;}
# 1791
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1794
struct Cyc_CfFlowInfo_FlowEnv*_tmp41F=env->fenv;
# 1797
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp420=_new_region("temp");struct _RegionHandle*temp=& _tmp420;_push_region(temp);{
struct _tuple22 _stmttmp5C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp422;struct Cyc_List_List*_tmp423;struct _tuple22 _tmp421=_stmttmp5C;_tmp422=_tmp421.f1;_tmp423=_tmp421.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp423))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp423->tl))->hd;
f=_tmp422;}{
# 1807
union Cyc_CfFlowInfo_FlowInfo _tmp424=f;struct Cyc_Dict_Dict _tmp425;struct Cyc_List_List*_tmp426;_LL274: if((_tmp424.BottomFL).tag != 1)goto _LL276;_LL275: {
struct _tuple18 _tmp856;struct _tuple18 _tmp428=(_tmp856.f1=f,((_tmp856.f2=f,_tmp856)));_npop_handler(0);return _tmp428;}_LL276: if((_tmp424.ReachableFL).tag != 2)goto _LL273;_tmp425=((struct _tuple14)(_tmp424.ReachableFL).val).f1;_tmp426=((struct _tuple14)(_tmp424.ReachableFL).val).f2;_LL277: {
# 1810
struct Cyc_Absyn_Exp*_tmp429=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp42A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp425,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp859;void*_tmp858;(_tmp858=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp859="expression may not be initialized",_tag_dyneither(_tmp859,sizeof(char),34))),_tag_dyneither(_tmp858,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp425,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85C;void*_tmp85B;(_tmp85B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp85C="expression may not be initialized",_tag_dyneither(_tmp85C,sizeof(char),34))),_tag_dyneither(_tmp85B,sizeof(void*),0)));}{
# 1817
union Cyc_CfFlowInfo_FlowInfo _tmp42F=f;
union Cyc_CfFlowInfo_FlowInfo _tmp430=f;
# 1822
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp431=r1;struct Cyc_Absyn_Vardecl*_tmp433;void*_tmp434;_LL279: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp432=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp431;if(_tmp432->tag != 8)goto _LL27B;else{_tmp433=_tmp432->f1;_tmp434=(void*)_tmp432->f2;}}_LL27A:
 r1=_tmp434;r1n=_tmp433;goto _LL278;_LL27B:;_LL27C:
 goto _LL278;_LL278:;}
# 1828
{void*_tmp435=r2;struct Cyc_Absyn_Vardecl*_tmp437;void*_tmp438;_LL27E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp436=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp435;if(_tmp436->tag != 8)goto _LL280;else{_tmp437=_tmp436->f1;_tmp438=(void*)_tmp436->f2;}}_LL27F:
 r2=_tmp438;r2n=_tmp437;goto _LL27D;_LL280:;_LL281:
 goto _LL27D;_LL27D:;}{
# 1833
struct _tuple0 _tmp85D;struct _tuple0 _stmttmp5D=(_tmp85D.f1=r1,((_tmp85D.f2=r2,_tmp85D)));struct _tuple0 _tmp439=_stmttmp5D;enum Cyc_CfFlowInfo_InitLevel _tmp43B;enum Cyc_CfFlowInfo_InitLevel _tmp43F;_LL283:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp43A->tag != 3)goto _LL285;else{_tmp43B=_tmp43A->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp43C->tag != 0)goto _LL285;};_LL284: {
# 1837
struct _tuple18 _stmttmp5F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp429,_tmp43B,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_FlowInfo _tmp450;struct _tuple18 _tmp44E=_stmttmp5F;_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;
switch(p){case Cyc_Absyn_Eq: _LL297:
 _tmp42F=_tmp450;_tmp430=_tmp44F;break;case Cyc_Absyn_Neq: _LL298:
 _tmp42F=_tmp44F;_tmp430=_tmp450;break;default: _LL299: {
struct Cyc_Core_Impossible_exn_struct _tmp863;const char*_tmp862;struct Cyc_Core_Impossible_exn_struct*_tmp861;(int)_throw((void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp863.tag=Cyc_Core_Impossible,((_tmp863.f1=((_tmp862="anal_test, zero-split",_tag_dyneither(_tmp862,sizeof(char),22))),_tmp863)))),_tmp861)))));}}
# 1843
goto _LL282;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp43D->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp43E->tag != 3)goto _LL287;else{_tmp43F=_tmp43E->f1;}};_LL286: {
# 1845
struct _tuple18 _stmttmp5E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp42A,_tmp43F,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp455;union Cyc_CfFlowInfo_FlowInfo _tmp456;struct _tuple18 _tmp454=_stmttmp5E;_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;
switch(p){case Cyc_Absyn_Eq: _LL29B:
 _tmp42F=_tmp456;_tmp430=_tmp455;break;case Cyc_Absyn_Neq: _LL29C:
 _tmp42F=_tmp455;_tmp430=_tmp456;break;default: _LL29D: {
struct Cyc_Core_Impossible_exn_struct _tmp869;const char*_tmp868;struct Cyc_Core_Impossible_exn_struct*_tmp867;(int)_throw((void*)((_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((_tmp869.tag=Cyc_Core_Impossible,((_tmp869.f1=((_tmp868="anal_test, zero-split",_tag_dyneither(_tmp868,sizeof(char),22))),_tmp869)))),_tmp867)))));}}
# 1851
goto _LL282;}_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp440->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp441=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp441->tag != 0)goto _LL289;};_LL288:
# 1853
 if(p == Cyc_Absyn_Eq)_tmp430=Cyc_CfFlowInfo_BottomFL();else{
_tmp42F=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp442=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp442->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp443=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp443->tag != 1)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp444=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp444->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp445->tag != 2)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp446=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp446->tag != 0)goto _LL28F;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp447->tag != 5)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp448->tag != 1)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp449->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp44A=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp44A->tag != 2)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp44B->tag != 0)goto _LL293;};_LL292:
 goto _LL294;_LL293:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp439.f1;if(_tmp44C->tag != 5)goto _LL295;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2;if(_tmp44D->tag != 0)goto _LL295;};_LL294:
# 1862
 if(p == Cyc_Absyn_Neq)_tmp430=Cyc_CfFlowInfo_BottomFL();else{
_tmp42F=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL295:;_LL296:
 goto _LL282;_LL282:;};}
# 1873
{struct _tuple0 _tmp86A;struct _tuple0 _stmttmp60=(_tmp86A.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp429->topt)),((_tmp86A.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp42A->topt)),_tmp86A)));struct _tuple0 _tmp45B=_stmttmp60;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45B.f1;if(_tmp45C->tag != 6)goto _LL2A2;else{if(_tmp45C->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45B.f2;if(_tmp45D->tag != 6)goto _LL2A4;else{if(_tmp45D->f1 != Cyc_Absyn_Unsigned)goto _LL2A4;}}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp45B.f1;if(_tmp45E->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL2A7;_LL2A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp45B.f2;if(_tmp45F->tag != 19)goto _LL2A8;}_LL2A7:
 goto _LL29F;_LL2A8:;_LL2A9: {
struct _tuple18 _tmp86B;struct _tuple18 _tmp461=(_tmp86B.f1=_tmp42F,((_tmp86B.f2=_tmp430,_tmp86B)));_npop_handler(0);return _tmp461;}_LL29F:;}
# 1882
switch(p){case Cyc_Absyn_Eq: _LL2AA:
 goto _LL2AB;case Cyc_Absyn_Neq: _LL2AB: goto _LL2AC;case Cyc_Absyn_Gt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Gte: _LL2AD: goto _LL2AE;case Cyc_Absyn_Lt: _LL2AE: goto _LL2AF;case Cyc_Absyn_Lte: _LL2AF: break;default: _LL2B0: {
struct _tuple18 _tmp86C;struct _tuple18 _tmp464=(_tmp86C.f1=_tmp42F,((_tmp86C.f2=_tmp430,_tmp86C)));_npop_handler(0);return _tmp464;}}{
# 1887
struct _RegionHandle*_tmp465=(env->fenv)->r;
struct _tuple13 _stmttmp61=Cyc_Relations_primop2relation(_tmp429,p,_tmp42A);struct Cyc_Absyn_Exp*_tmp467;enum Cyc_Relations_Relation _tmp468;struct Cyc_Absyn_Exp*_tmp469;struct _tuple13 _tmp466=_stmttmp61;_tmp467=_tmp466.f1;_tmp468=_tmp466.f2;_tmp469=_tmp466.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1892
if(Cyc_Relations_exp2relnop(_tmp467,& n1) && Cyc_Relations_exp2relnop(_tmp469,& n2)){
# 1894
struct Cyc_List_List*_tmp46A=Cyc_Relations_add_relation(_tmp465,n1,_tmp468,n2,_tmp426);
# 1898
struct Cyc_List_List*_tmp46B=Cyc_Relations_add_relation(_tmp465,n2,Cyc_Relations_flip_relation(_tmp468),n1,_tmp426);
struct _tuple18 _tmp86D;struct _tuple18 _stmttmp62=(_tmp86D.f1=_tmp42F,((_tmp86D.f2=_tmp430,_tmp86D)));struct _tuple18 _tmp46C=_stmttmp62;struct Cyc_Dict_Dict _tmp46D;struct Cyc_Dict_Dict _tmp46E;struct Cyc_Dict_Dict _tmp46F;struct Cyc_Dict_Dict _tmp470;_LL2B3: if(((_tmp46C.f1).ReachableFL).tag != 2)goto _LL2B5;_tmp46D=((struct _tuple14)((_tmp46C.f1).ReachableFL).val).f1;if(((_tmp46C.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp46E=((struct _tuple14)((_tmp46C.f2).ReachableFL).val).f1;_LL2B4: {
# 1901
struct _tuple18 _tmp86E;struct _tuple18 _tmp472=(_tmp86E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46D,_tmp46A),((_tmp86E.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46E,_tmp46B),_tmp86E)));_npop_handler(0);return _tmp472;}_LL2B5: if(((_tmp46C.f2).ReachableFL).tag != 2)goto _LL2B7;_tmp46F=((struct _tuple14)((_tmp46C.f2).ReachableFL).val).f1;_LL2B6: {
# 1903
struct _tuple18 _tmp86F;struct _tuple18 _tmp474=(_tmp86F.f1=_tmp42F,((_tmp86F.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46F,_tmp46B),_tmp86F)));_npop_handler(0);return _tmp474;}_LL2B7: if(((_tmp46C.f1).ReachableFL).tag != 2)goto _LL2B9;_tmp470=((struct _tuple14)((_tmp46C.f1).ReachableFL).val).f1;_LL2B8: {
# 1905
struct _tuple18 _tmp870;struct _tuple18 _tmp476=(_tmp870.f1=Cyc_CfFlowInfo_ReachableFL(_tmp470,_tmp46A),((_tmp870.f2=_tmp430,_tmp870)));_npop_handler(0);return _tmp476;}_LL2B9:;_LL2BA: {
# 1907
struct _tuple18 _tmp871;struct _tuple18 _tmp478=(_tmp871.f1=_tmp42F,((_tmp871.f2=_tmp430,_tmp871)));_npop_handler(0);return _tmp478;}_LL2B2:;}else{
# 1910
struct _tuple18 _tmp872;struct _tuple18 _tmp47B=(_tmp872.f1=_tmp42F,((_tmp872.f2=_tmp430,_tmp872)));_npop_handler(0);return _tmp47B;}};};};}_LL273:;};
# 1799
;_pop_region(temp);}
# 1916
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1918
struct Cyc_CfFlowInfo_FlowEnv*_tmp47C=env->fenv;
void*_stmttmp63=e->r;void*_tmp47D=_stmttmp63;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp48C;enum Cyc_Absyn_Primop _tmp48E;struct Cyc_List_List*_tmp48F;_LL2BC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp47E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp47E->tag != 5)goto _LL2BE;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;_tmp481=_tmp47E->f3;}}_LL2BD: {
# 1921
struct _tuple18 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47F);union Cyc_CfFlowInfo_FlowInfo _tmp491;union Cyc_CfFlowInfo_FlowInfo _tmp492;struct _tuple18 _tmp490=_stmttmp6B;_tmp491=_tmp490.f1;_tmp492=_tmp490.f2;{
struct _tuple18 _stmttmp6C=Cyc_NewControlFlow_anal_test(env,_tmp491,_tmp480);union Cyc_CfFlowInfo_FlowInfo _tmp494;union Cyc_CfFlowInfo_FlowInfo _tmp495;struct _tuple18 _tmp493=_stmttmp6C;_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;{
struct _tuple18 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,_tmp492,_tmp481);union Cyc_CfFlowInfo_FlowInfo _tmp497;union Cyc_CfFlowInfo_FlowInfo _tmp498;struct _tuple18 _tmp496=_stmttmp6D;_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;{
struct _tuple18 _tmp873;return(_tmp873.f1=Cyc_CfFlowInfo_join_flow(_tmp47C,env->all_changed,_tmp494,_tmp497),((_tmp873.f2=
Cyc_CfFlowInfo_join_flow(_tmp47C,env->all_changed,_tmp495,_tmp498),_tmp873)));};};};}_LL2BE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp482=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp482->tag != 6)goto _LL2C0;else{_tmp483=_tmp482->f1;_tmp484=_tmp482->f2;}}_LL2BF: {
# 1927
struct _tuple18 _stmttmp69=Cyc_NewControlFlow_anal_test(env,inflow,_tmp483);union Cyc_CfFlowInfo_FlowInfo _tmp49B;union Cyc_CfFlowInfo_FlowInfo _tmp49C;struct _tuple18 _tmp49A=_stmttmp69;_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;{
struct _tuple18 _stmttmp6A=Cyc_NewControlFlow_anal_test(env,_tmp49B,_tmp484);union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple18 _tmp49D=_stmttmp6A;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{
struct _tuple18 _tmp874;return(_tmp874.f1=_tmp49E,((_tmp874.f2=Cyc_CfFlowInfo_join_flow(_tmp47C,env->all_changed,_tmp49C,_tmp49F),_tmp874)));};};}_LL2C0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp485=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp485->tag != 7)goto _LL2C2;else{_tmp486=_tmp485->f1;_tmp487=_tmp485->f2;}}_LL2C1: {
# 1931
struct _tuple18 _stmttmp67=Cyc_NewControlFlow_anal_test(env,inflow,_tmp486);union Cyc_CfFlowInfo_FlowInfo _tmp4A2;union Cyc_CfFlowInfo_FlowInfo _tmp4A3;struct _tuple18 _tmp4A1=_stmttmp67;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;{
struct _tuple18 _stmttmp68=Cyc_NewControlFlow_anal_test(env,_tmp4A3,_tmp487);union Cyc_CfFlowInfo_FlowInfo _tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;struct _tuple18 _tmp4A4=_stmttmp68;_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;{
struct _tuple18 _tmp875;return(_tmp875.f1=Cyc_CfFlowInfo_join_flow(_tmp47C,env->all_changed,_tmp4A2,_tmp4A5),((_tmp875.f2=_tmp4A6,_tmp875)));};};}_LL2C2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp488=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp488->tag != 8)goto _LL2C4;else{_tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;}}_LL2C3: {
# 1935
struct _tuple15 _stmttmp66=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp489);union Cyc_CfFlowInfo_FlowInfo _tmp4A9;void*_tmp4AA;struct _tuple15 _tmp4A8=_stmttmp66;_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4A9,_tmp48A);}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp48B->tag != 2)goto _LL2C6;else{if(_tmp48B->f1 != Cyc_Absyn_Not)goto _LL2C6;if(_tmp48B->f2 == 0)goto _LL2C6;_tmp48C=(struct Cyc_Absyn_Exp*)(_tmp48B->f2)->hd;if((_tmp48B->f2)->tl != 0)goto _LL2C6;}}_LL2C5: {
# 1938
struct _tuple18 _stmttmp65=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48C);union Cyc_CfFlowInfo_FlowInfo _tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;struct _tuple18 _tmp4AB=_stmttmp65;_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{
struct _tuple18 _tmp876;return(_tmp876.f1=_tmp4AD,((_tmp876.f2=_tmp4AC,_tmp876)));};}_LL2C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47D;if(_tmp48D->tag != 2)goto _LL2C8;else{_tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;}}_LL2C7:
# 1941
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp48E,_tmp48F);_LL2C8:;_LL2C9: {
# 1945
struct _tuple15 _stmttmp64=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4B0;void*_tmp4B1;struct _tuple15 _tmp4AF=_stmttmp64;_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B2=_tmp4B0;struct Cyc_Dict_Dict _tmp4B3;_LL2CB: if((_tmp4B2.BottomFL).tag != 1)goto _LL2CD;_LL2CC: {
struct _tuple18 _tmp877;return(_tmp877.f1=_tmp4B0,((_tmp877.f2=_tmp4B0,_tmp877)));}_LL2CD: if((_tmp4B2.ReachableFL).tag != 2)goto _LL2CA;_tmp4B3=((struct _tuple14)(_tmp4B2.ReachableFL).val).f1;_LL2CE: {
# 1949
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4B5=_tmp4B1;struct Cyc_Absyn_Vardecl*_tmp4B7;void*_tmp4B8;enum Cyc_CfFlowInfo_InitLevel _tmp4C1;_LL2D0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4B6->tag != 8)goto _LL2D2;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=(void*)_tmp4B6->f2;}}_LL2D1:
 _tmp4B1=_tmp4B8;locname=_tmp4B7;goto retry;_LL2D2: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4B9->tag != 0)goto _LL2D4;}_LL2D3: {
struct _tuple18 _tmp878;return(_tmp878.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp878.f2=_tmp4B0,_tmp878)));}_LL2D4: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4BA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BA->tag != 2)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4BB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BB->tag != 1)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4BC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BC->tag != 5)goto _LL2DA;}_LL2D9: {
struct _tuple18 _tmp879;return(_tmp879.f1=_tmp4B0,((_tmp879.f2=Cyc_CfFlowInfo_BottomFL(),_tmp879)));}_LL2DA: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BD=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BD->tag != 3)goto _LL2DC;else{if(_tmp4BD->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BE->tag != 4)goto _LL2DE;else{if(_tmp4BE->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DE;}}_LL2DD:
 goto _LL2DF;_LL2DE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4BF=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4BF->tag != 7)goto _LL2E0;}_LL2DF:
# 1959
{const char*_tmp87C;void*_tmp87B;(_tmp87B=0,Cyc_Tcutil_terr(e->loc,((_tmp87C="expression may not be initialized",_tag_dyneither(_tmp87C,sizeof(char),34))),_tag_dyneither(_tmp87B,sizeof(void*),0)));}{
struct _tuple18 _tmp87D;return(_tmp87D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87D.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87D)));};_LL2E0: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4C0->tag != 3)goto _LL2E2;else{_tmp4C1=_tmp4C0->f1;}}_LL2E1:
# 1962
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B0,e,_tmp4C1,locname);_LL2E2: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C2=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4C2->tag != 4)goto _LL2E4;}_LL2E3: {
struct _tuple18 _tmp87E;return(_tmp87E.f1=_tmp4B0,((_tmp87E.f2=_tmp4B0,_tmp87E)));}_LL2E4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B5;if(_tmp4C3->tag != 6)goto _LL2CF;}_LL2E5: {
struct Cyc_Core_Impossible_exn_struct _tmp884;const char*_tmp883;struct Cyc_Core_Impossible_exn_struct*_tmp882;(int)_throw((void*)((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=((_tmp884.tag=Cyc_Core_Impossible,((_tmp884.f1=((_tmp883="anal_test",_tag_dyneither(_tmp883,sizeof(char),10))),_tmp884)))),_tmp882)))));}_LL2CF:;}}_LL2CA:;};}_LL2BB:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1970
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 1972
unsigned int _tmp4CE;struct Cyc_NewControlFlow_AnalEnv*_tmp4CF;struct Cyc_Dict_Dict _tmp4D0;struct _tuple26*_tmp4CD=ckenv;_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;_tmp4D0=_tmp4CD->f3;{
void*_tmp4D1=root;struct Cyc_Absyn_Vardecl*_tmp4D3;_LL2E7: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D2=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4D1;if(_tmp4D2->tag != 0)goto _LL2E9;else{_tmp4D3=_tmp4D2->f1;}}_LL2E8:
# 1975
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4CF->fenv)->r,_tmp4D3->type)){
void*_tmp4D4=rval;_LL2EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4D5->tag != 8)goto _LL2EE;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D6=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D5->f2);if(_tmp4D6->tag != 7)goto _LL2EE;}}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4D7->tag != 8)goto _LL2F0;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D7->f2);if(_tmp4D8->tag != 0)goto _LL2F0;}}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4D9->tag != 8)goto _LL2F2;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D9->f2);if(_tmp4DA->tag != 3)goto _LL2F2;else{if(_tmp4DA->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F2;}}}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4DB=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4DB->tag != 7)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DC=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4DC->tag != 0)goto _LL2F6;}_LL2F5:
 goto _LL2F7;_LL2F6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4DD=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D4;if(_tmp4DD->tag != 3)goto _LL2F8;else{if(_tmp4DD->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F8;}}_LL2F7:
 goto _LL2EB;_LL2F8:;_LL2F9:
# 1984
{const char*_tmp888;void*_tmp887[1];struct Cyc_String_pa_PrintArg_struct _tmp886;(_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D3->name)),((_tmp887[0]=& _tmp886,Cyc_Tcutil_warn(_tmp4CE,((_tmp888="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp888,sizeof(char),57))),_tag_dyneither(_tmp887,sizeof(void*),1)))))));}
goto _LL2EB;_LL2EB:;}
# 1987
goto _LL2E6;_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;};}
# 1992
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4E1=flow;struct Cyc_Dict_Dict _tmp4E2;_LL2FB: if((_tmp4E1.BottomFL).tag != 1)goto _LL2FD;_LL2FC:
 return;_LL2FD: if((_tmp4E1.ReachableFL).tag != 2)goto _LL2FA;_tmp4E2=((struct _tuple14)(_tmp4E1.ReachableFL).val).f1;_LL2FE:
# 1996
{struct Cyc_List_List*_tmp4E3=env->param_roots;for(0;_tmp4E3 != 0;_tmp4E3=_tmp4E3->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E2,Cyc_CfFlowInfo_lookup_place(_tmp4E2,(struct Cyc_CfFlowInfo_Place*)_tmp4E3->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp88B;void*_tmp88A;(_tmp88A=0,Cyc_Tcutil_terr(loc,((_tmp88B="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp88B,sizeof(char),76))),_tag_dyneither(_tmp88A,sizeof(void*),0)));}}}
# 2000
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp88C;struct _tuple26 _tmp4E6=(_tmp88C.f1=loc,((_tmp88C.f2=env,((_tmp88C.f3=_tmp4E2,_tmp88C)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4E6,_tmp4E2);}
# 2004
return;_LL2FA:;}
# 2008
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2011
struct Cyc_List_List*_tmp4E8=0;
struct Cyc_List_List*_tmp4E9=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp6E=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;struct _tuple23*_tmp4EA=_stmttmp6E;_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;
if((_tmp4EB != 0  && 
# 2018
Cyc_Tcutil_is_noalias_pointer((*_tmp4EB)->type,0)) && _tmp4EC != 0){
# 2020
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp88F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp88E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4ED=(_tmp88E=_region_malloc(frgn,sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=0,((_tmp88F.f1=*_tmp4EB,_tmp88F)))),_tmp88E)));
struct Cyc_CfFlowInfo_Place*_tmp890;struct Cyc_CfFlowInfo_Place*_tmp4EE=(_tmp890=_region_malloc(frgn,sizeof(*_tmp890)),((_tmp890->root=(void*)_tmp4ED,((_tmp890->fields=0,_tmp890)))));
{struct Cyc_List_List*_tmp891;_tmp4E8=((_tmp891=_region_malloc(frgn,sizeof(*_tmp891)),((_tmp891->hd=_tmp4EE,((_tmp891->tl=_tmp4E8,_tmp891))))));}{
struct Cyc_List_List*_tmp892;_tmp4E9=((_tmp892=_region_malloc(frgn,sizeof(*_tmp892)),((_tmp892->hd=_tmp4EC,((_tmp892->tl=_tmp4E9,_tmp892))))));};}}}{
# 2026
struct _tuple1 _tmp893;return(_tmp893.f1=_tmp4E8,((_tmp893.f2=_tmp4E9,_tmp893)));};}
# 2035
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2040
{union Cyc_CfFlowInfo_FlowInfo _tmp4F5=flow;_LL300: if((_tmp4F5.BottomFL).tag != 1)goto _LL302;_LL301:
 return flow;_LL302:;_LL303:
 goto _LL2FF;_LL2FF:;}{
# 2044
int _tmp4F6=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp4F7=Cyc_Absyn_exp_unsigned_one;
int _tmp4F8=0;
int _tmp4F9=1;
# 2051
{struct Cyc_List_List*_tmp4FA=consumed_vals;for(0;_tmp4FA != 0;
(_tmp4FA=_tmp4FA->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp4FB=flow;struct Cyc_Dict_Dict _tmp4FC;_LL305: if((_tmp4FB.ReachableFL).tag != 2)goto _LL307;_tmp4FC=((struct _tuple14)(_tmp4FB.ReachableFL).val).f1;_LL306: {
# 2061
void*_tmp4FD=Cyc_CfFlowInfo_lookup_place(_tmp4FC,(struct Cyc_CfFlowInfo_Place*)_tmp4FA->hd);
struct Cyc_CfFlowInfo_Place*_stmttmp6F=(struct Cyc_CfFlowInfo_Place*)_tmp4FA->hd;void*_tmp4FF;struct Cyc_CfFlowInfo_Place*_tmp4FE=_stmttmp6F;_tmp4FF=_tmp4FE->root;
{struct _tuple0 _tmp894;struct _tuple0 _stmttmp70=(_tmp894.f1=_tmp4FF,((_tmp894.f2=_tmp4FD,_tmp894)));struct _tuple0 _tmp500=_stmttmp70;struct Cyc_Absyn_Vardecl*_tmp502;struct Cyc_Absyn_Vardecl*_tmp504;void*_tmp505;struct Cyc_Absyn_Vardecl*_tmp507;_LL30A:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp501=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp500.f1;if(_tmp501->tag != 0)goto _LL30C;else{_tmp502=_tmp501->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp503=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp500.f2;if(_tmp503->tag != 8)goto _LL30C;else{_tmp504=_tmp503->f1;_tmp505=(void*)_tmp503->f2;}};_LL30B:
# 2065
 if(_tmp502 == _tmp504){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4F7,_tmp4F9,_tmp505,& _tmp4F8)){
if(!_tmp4F6){
const char*_tmp898;void*_tmp897[1];struct Cyc_String_pa_PrintArg_struct _tmp896;(_tmp896.tag=0,((_tmp896.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2070
Cyc_Absynpp_qvar2string(_tmp502->name)),((_tmp897[0]=& _tmp896,Cyc_Tcutil_terr(loc,((_tmp898="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp898,sizeof(char),58))),_tag_dyneither(_tmp897,sizeof(void*),1)))))));}}else{
# 2072
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4FC,_tmp505)!= Cyc_CfFlowInfo_AllIL  && !_tmp4F6){
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_PrintArg_struct _tmp89A;(_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2075
Cyc_Absynpp_qvar2string(_tmp502->name)),((_tmp89B[0]=& _tmp89A,Cyc_Tcutil_terr(loc,((_tmp89C="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp89C,sizeof(char),84))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))));}else{
# 2077
if(_tmp4F6){
# 2083
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp50F;union Cyc_CfFlowInfo_AbsLVal _tmp510;struct _tuple17 _tmp50E=_stmttmp71;_tmp50F=_tmp50E.f1;_tmp510=_tmp50E.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp511=_tmp510;struct Cyc_CfFlowInfo_Place*_tmp512;_LL311: if((_tmp511.PlaceL).tag != 1)goto _LL313;_tmp512=(struct Cyc_CfFlowInfo_Place*)(_tmp511.PlaceL).val;_LL312: {
# 2087
void*_tmp513=Cyc_CfFlowInfo_lookup_place(_tmp4FC,_tmp512);
_tmp513=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp513);{
# 2095
struct _tuple15 _tmp89E;struct _tuple15 _tmp89D;struct _tuple15 _stmttmp72=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2098
(_tmp89D.f1=_tmp50F,((_tmp89D.f2=_tmp513,_tmp89D)))),(
(_tmp89E.f1=_tmp50F,((_tmp89E.f2=_tmp505,_tmp89E)))));
# 2095
union Cyc_CfFlowInfo_FlowInfo _tmp515;void*_tmp516;struct _tuple15 _tmp514=_stmttmp72;_tmp515=_tmp514.f1;_tmp516=_tmp514.f2;
# 2100
{union Cyc_CfFlowInfo_FlowInfo _tmp517=_tmp515;struct Cyc_Dict_Dict _tmp518;struct Cyc_List_List*_tmp519;_LL316: if((_tmp517.ReachableFL).tag != 2)goto _LL318;_tmp518=((struct _tuple14)(_tmp517.ReachableFL).val).f1;_tmp519=((struct _tuple14)(_tmp517.ReachableFL).val).f2;_LL317:
# 2102
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp518,env->all_changed,_tmp512,_tmp516),_tmp519);
# 2106
goto _LL315;_LL318:;_LL319: {
# 2108
const char*_tmp8A1;void*_tmp8A0;(_tmp8A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A1="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp8A1,sizeof(char),42))),_tag_dyneither(_tmp8A0,sizeof(void*),0)));}_LL315:;}
# 2110
goto _LL310;};}_LL313:;_LL314:
# 2115
 goto _LL310;_LL310:;};}}}}else{
# 2119
_tmp507=_tmp502;goto _LL30D;}
goto _LL309;_LL30C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp506=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp500.f1;if(_tmp506->tag != 0)goto _LL30E;else{_tmp507=_tmp506->f1;}}_LL30D:
# 2122
 if(!_tmp4F6){
const char*_tmp8A5;void*_tmp8A4[1];struct Cyc_String_pa_PrintArg_struct _tmp8A3;(_tmp8A3.tag=0,((_tmp8A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2125
Cyc_Absynpp_qvar2string(_tmp507->name)),((_tmp8A4[0]=& _tmp8A3,Cyc_Tcutil_terr(loc,((_tmp8A5="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8A5,sizeof(char),85))),_tag_dyneither(_tmp8A4,sizeof(void*),1)))))));}
goto _LL309;_LL30E:;_LL30F: {
# 2128
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A8="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8A8,sizeof(char),40))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}_LL309:;}
# 2130
goto _LL304;}_LL307: if((_tmp4FB.BottomFL).tag != 1)goto _LL304;_LL308: {
# 2132
const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8AB="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp8AB,sizeof(char),38))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}_LL304:;}}
# 2135
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2138
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2141
struct Cyc_CfFlowInfo_FlowEnv*_tmp526=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp73=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Stmt*_tmp52A;unsigned int _tmp52B;struct Cyc_Absyn_Switch_clause*_tmp527=_stmttmp73;_tmp528=_tmp527->pat_vars;_tmp529=_tmp527->where_clause;_tmp52A=_tmp527->body;_tmp52B=_tmp527->loc;{
struct _tuple1 _stmttmp74=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp528))->v);struct Cyc_List_List*_tmp52D;struct Cyc_List_List*_tmp52E;struct _tuple1 _tmp52C=_stmttmp74;_tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp528->v,_tmp52D != 0,_tmp52B,exp_loc);
# 2149
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp52F=env->unique_pat_vars;
{struct _tuple27*_tmp8AE;struct Cyc_List_List*_tmp8AD;env->unique_pat_vars=(
(_tmp8AD=_region_malloc(env->r,sizeof(*_tmp8AD)),((_tmp8AD->hd=((_tmp8AE=_region_malloc(env->r,sizeof(*_tmp8AE)),((_tmp8AE->f1=_tmp52A,((_tmp8AE->f2=_tmp52D,((_tmp8AE->f3=_tmp52E,_tmp8AE)))))))),((_tmp8AD->tl=_tmp52F,_tmp8AD))))));}
# 2154
if(_tmp529 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp529;
struct _tuple18 _stmttmp75=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp533;union Cyc_CfFlowInfo_FlowInfo _tmp534;struct _tuple18 _tmp532=_stmttmp75;_tmp533=_tmp532.f1;_tmp534=_tmp532.f2;
inflow=_tmp534;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp533,_tmp52A,0);}else{
# 2160
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp52A,0);}
# 2162
env->unique_pat_vars=_tmp52F;{
union Cyc_CfFlowInfo_FlowInfo _tmp535=clause_outflow;_LL31B: if((_tmp535.BottomFL).tag != 1)goto _LL31D;_LL31C:
 goto _LL31A;_LL31D:;_LL31E:
# 2167
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp52D,_tmp52E,clause_outflow,_tmp52B);
# 2169
if(scs->tl == 0)
return clause_outflow;else{
# 2174
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,Cyc_Tcutil_terr(_tmp52A->loc,((_tmp8B1="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B1,sizeof(char),38))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}else{
# 2177
const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,Cyc_Tcutil_warn(_tmp52A->loc,((_tmp8B4="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B4,sizeof(char),38))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}
# 2179
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2181
goto _LL31A;_LL31A:;};};};}
# 2184
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2189
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2191
struct Cyc_NewControlFlow_AnalEnv*_tmp53B;struct Cyc_Dict_Dict _tmp53C;unsigned int _tmp53D;struct _tuple28*_tmp53A=vdenv;_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;_tmp53D=_tmp53A->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp53B->fenv)->r,vd->type)){
# 2194
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8B5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8B5.tag=0,((_tmp8B5.f1=vd,_tmp8B5)));
# 2196
void*_tmp53E=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp53C,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp53F=_tmp53E;_LL320: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp540=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp540->tag != 8)goto _LL322;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp541=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp540->f2);if(_tmp541->tag != 7)goto _LL322;}}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp542=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp542->tag != 8)goto _LL324;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp543=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp542->f2);if(_tmp543->tag != 0)goto _LL324;}}_LL323:
 goto _LL325;_LL324: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp544=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp544->tag != 8)goto _LL326;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp545=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp544->f2);if(_tmp545->tag != 3)goto _LL326;else{if(_tmp545->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}}_LL325:
 goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp546=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp546->tag != 7)goto _LL328;}_LL327:
 goto _LL329;_LL328: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp547=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp547->tag != 0)goto _LL32A;}_LL329:
 goto _LL32B;_LL32A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp548=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp53F;if(_tmp548->tag != 3)goto _LL32C;else{if(_tmp548->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32C;}}_LL32B:
 goto _LL31F;_LL32C:;_LL32D:
# 2205
{const char*_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_PrintArg_struct _tmp8B7;(_tmp8B7.tag=0,((_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8B8[0]=& _tmp8B7,Cyc_Tcutil_warn(_tmp53D,((_tmp8B9="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8B9,sizeof(char),68))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));}
# 2207
goto _LL31F;_LL31F:;}}
# 2212
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2214
{union Cyc_CfFlowInfo_FlowInfo _tmp54D=inflow;struct Cyc_Dict_Dict _tmp54E;_LL32F: if((_tmp54D.ReachableFL).tag != 2)goto _LL331;_tmp54E=((struct _tuple14)(_tmp54D.ReachableFL).val).f1;_LL330: {
# 2216
struct _tuple28 _tmp8BA;struct _tuple28 _tmp54F=(_tmp8BA.f1=env,((_tmp8BA.f2=_tmp54E,((_tmp8BA.f3=decl->loc,_tmp8BA)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp550=env->fenv;
{void*_stmttmp76=decl->r;void*_tmp551=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp553;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp557;_LL334: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp552=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp551;if(_tmp552->tag != 0)goto _LL336;else{_tmp553=_tmp552->f1;}}_LL335:
# 2220
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp54F,_tmp553);
goto _LL333;_LL336: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp554=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp551;if(_tmp554->tag != 2)goto _LL338;else{if(_tmp554->f2 == 0)goto _LL338;_tmp555=(struct Cyc_List_List*)(_tmp554->f2)->v;}}_LL337: {
# 2223
struct _tuple1 _stmttmp77=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp555);struct Cyc_List_List*_tmp559;struct _tuple1 _tmp558=_stmttmp77;_tmp559=_tmp558.f1;{
struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp559);
_tmp557=_tmp55A;goto _LL339;};}_LL338: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp556=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp551;if(_tmp556->tag != 3)goto _LL33A;else{_tmp557=_tmp556->f1;}}_LL339:
# 2227
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp54F,_tmp557);
goto _LL333;_LL33A:;_LL33B:
 goto _LL333;_LL333:;}
# 2231
goto _LL32E;}_LL331:;_LL332:
 goto _LL32E;_LL32E:;}
# 2234
return;}
# 2240
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2243
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_stmttmp78=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp55D;struct Cyc_List_List*_tmp55E;struct Cyc_List_List*_tmp55F;struct _tuple27*_tmp55C=_stmttmp78;_tmp55D=_tmp55C->f1;_tmp55E=_tmp55C->f2;_tmp55F=_tmp55C->f3;
keep_going=0;{
# 2251
struct Cyc_Absyn_Stmt*_tmp560=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp560 != _tmp55D){
struct Cyc_Absyn_Stmt*_tmp561=(Cyc_NewControlFlow_get_stmt_annot(_tmp560))->encloser;
if(_tmp561 == _tmp560){
# 2257
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp55E,_tmp55F,inflow,dest->loc);
keep_going=1;
break;}
# 2261
_tmp560=_tmp561;}};}}
# 2264
return inflow;}
# 2270
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2273
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _stmttmp79=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp563;union Cyc_CfFlowInfo_FlowInfo*_tmp564;struct _tuple19 _tmp562=_stmttmp79;_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;
inflow=*_tmp564;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp565=env->fenv;
# 2280
void*_stmttmp7A=s->r;void*_tmp566=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Stmt*_tmp56E;struct Cyc_Absyn_Stmt*_tmp56F;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Stmt*_tmp572;struct Cyc_Absyn_Stmt*_tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Stmt*_tmp576;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_Absyn_Exp*_tmp57A;struct Cyc_Absyn_Stmt*_tmp57B;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Stmt*_tmp581;struct Cyc_Absyn_Stmt*_tmp582;struct Cyc_List_List*_tmp585;struct Cyc_Absyn_Switch_clause*_tmp586;struct Cyc_Absyn_Stmt*_tmp588;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Absyn_Stmt*_tmp58C;struct Cyc_Absyn_Exp*_tmp58E;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_Stmt*_tmp591;struct Cyc_List_List*_tmp592;struct Cyc_List_List*_tmp595;struct Cyc_Absyn_Exp*_tmp596;unsigned int _tmp597;struct Cyc_Absyn_Stmt*_tmp598;struct Cyc_Absyn_Tvar*_tmp59B;struct Cyc_Absyn_Vardecl*_tmp59C;struct Cyc_Absyn_Stmt*_tmp59D;struct Cyc_Absyn_Decl*_tmp59F;struct Cyc_Absyn_Stmt*_tmp5A0;struct Cyc_Absyn_Stmt*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A4;_LL33D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp567=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp567->tag != 0)goto _LL33F;}_LL33E:
 return inflow;_LL33F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp568=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp568->tag != 3)goto _LL341;else{if(_tmp568->f1 != 0)goto _LL341;}}_LL340:
# 2283
 if(env->noreturn){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,Cyc_Tcutil_terr(s->loc,((_tmp8BD="`noreturn' function might return",_tag_dyneither(_tmp8BD,sizeof(char),33))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL341: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp569->tag != 3)goto _LL343;else{_tmp56A=_tmp569->f1;}}_LL342:
# 2289
 if(env->noreturn){
const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,Cyc_Tcutil_terr(s->loc,((_tmp8C0="`noreturn' function might return",_tag_dyneither(_tmp8C0,sizeof(char),33))),_tag_dyneither(_tmp8BF,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp56A));union Cyc_CfFlowInfo_FlowInfo _tmp5AA;void*_tmp5AB;struct _tuple15 _tmp5A9=_stmttmp8D;_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;
_tmp5AA=Cyc_NewControlFlow_use_Rval(env,_tmp56A->loc,_tmp5AA,_tmp5AB);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5AA);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp5AA,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL343: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp56B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp56B->tag != 1)goto _LL345;else{_tmp56C=_tmp56B->f1;}}_LL344: {
# 2298
struct _tuple16*_tmp5AC=rval_opt;void**_tmp5AD;int _tmp5AE;_LL36A: if(_tmp5AC == 0)goto _LL36C;_tmp5AD=(void**)& _tmp5AC->f1;_tmp5AE=_tmp5AC->f2;_LL36B: {
# 2300
struct _tuple15 _stmttmp8C=Cyc_NewControlFlow_anal_Rexp(env,_tmp5AE,inflow,_tmp56C);union Cyc_CfFlowInfo_FlowInfo _tmp5B0;void*_tmp5B1;struct _tuple15 _tmp5AF=_stmttmp8C;_tmp5B0=_tmp5AF.f1;_tmp5B1=_tmp5AF.f2;
*_tmp5AD=_tmp5B1;
return _tmp5B0;}_LL36C: if(_tmp5AC != 0)goto _LL369;_LL36D:
# 2304
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp56C)).f1;_LL369:;}_LL345: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp56D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp56D->tag != 2)goto _LL347;else{_tmp56E=_tmp56D->f1;_tmp56F=_tmp56D->f2;}}_LL346:
# 2308
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp56E,0),_tmp56F,rval_opt);_LL347: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp570=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp570->tag != 4)goto _LL349;else{_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;_tmp573=_tmp570->f3;}}_LL348: {
# 2311
struct _tuple18 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp571);union Cyc_CfFlowInfo_FlowInfo _tmp5B3;union Cyc_CfFlowInfo_FlowInfo _tmp5B4;struct _tuple18 _tmp5B2=_stmttmp8B;_tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;{
# 2318
union Cyc_CfFlowInfo_FlowInfo _tmp5B5=Cyc_NewControlFlow_anal_stmt(env,_tmp5B4,_tmp573,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=Cyc_NewControlFlow_anal_stmt(env,_tmp5B3,_tmp572,0);
return Cyc_CfFlowInfo_join_flow(_tmp565,env->all_changed,_tmp5B6,_tmp5B5);};}_LL349: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp574=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp574->tag != 5)goto _LL34B;else{_tmp575=(_tmp574->f1).f1;_tmp576=(_tmp574->f1).f2;_tmp577=_tmp574->f2;}}_LL34A: {
# 2326
struct _tuple19 _stmttmp89=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp576);union Cyc_CfFlowInfo_FlowInfo*_tmp5B8;struct _tuple19 _tmp5B7=_stmttmp89;_tmp5B8=_tmp5B7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B9=*_tmp5B8;
struct _tuple18 _stmttmp8A=Cyc_NewControlFlow_anal_test(env,_tmp5B9,_tmp575);union Cyc_CfFlowInfo_FlowInfo _tmp5BB;union Cyc_CfFlowInfo_FlowInfo _tmp5BC;struct _tuple18 _tmp5BA=_stmttmp8A;_tmp5BB=_tmp5BA.f1;_tmp5BC=_tmp5BA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BD=Cyc_NewControlFlow_anal_stmt(env,_tmp5BB,_tmp577,0);
Cyc_NewControlFlow_update_flow(env,_tmp576,_tmp5BD);
return _tmp5BC;};};}_LL34B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp578=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp578->tag != 14)goto _LL34D;else{_tmp579=_tmp578->f1;_tmp57A=(_tmp578->f2).f1;_tmp57B=(_tmp578->f2).f2;}}_LL34C: {
# 2336
union Cyc_CfFlowInfo_FlowInfo _tmp5BE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp579,0);
struct _tuple19 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BE,_tmp57B);union Cyc_CfFlowInfo_FlowInfo*_tmp5C0;struct _tuple19 _tmp5BF=_stmttmp87;_tmp5C0=_tmp5BF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C1=*_tmp5C0;
struct _tuple18 _stmttmp88=Cyc_NewControlFlow_anal_test(env,_tmp5C1,_tmp57A);union Cyc_CfFlowInfo_FlowInfo _tmp5C3;union Cyc_CfFlowInfo_FlowInfo _tmp5C4;struct _tuple18 _tmp5C2=_stmttmp88;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;
Cyc_NewControlFlow_update_flow(env,_tmp579,_tmp5C3);
return _tmp5C4;};}_LL34D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp57C=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp57C->tag != 9)goto _LL34F;else{_tmp57D=_tmp57C->f1;_tmp57E=(_tmp57C->f2).f1;_tmp57F=(_tmp57C->f2).f2;_tmp580=(_tmp57C->f3).f1;_tmp581=(_tmp57C->f3).f2;_tmp582=_tmp57C->f4;}}_LL34E: {
# 2345
union Cyc_CfFlowInfo_FlowInfo _tmp5C5=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp57D)).f1;
struct _tuple19 _stmttmp84=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C5,_tmp57F);union Cyc_CfFlowInfo_FlowInfo*_tmp5C7;struct _tuple19 _tmp5C6=_stmttmp84;_tmp5C7=_tmp5C6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C8=*_tmp5C7;
struct _tuple18 _stmttmp85=Cyc_NewControlFlow_anal_test(env,_tmp5C8,_tmp57E);union Cyc_CfFlowInfo_FlowInfo _tmp5CA;union Cyc_CfFlowInfo_FlowInfo _tmp5CB;struct _tuple18 _tmp5C9=_stmttmp85;_tmp5CA=_tmp5C9.f1;_tmp5CB=_tmp5C9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CC=Cyc_NewControlFlow_anal_stmt(env,_tmp5CA,_tmp582,0);
struct _tuple19 _stmttmp86=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CC,_tmp581);union Cyc_CfFlowInfo_FlowInfo*_tmp5CE;struct _tuple19 _tmp5CD=_stmttmp86;_tmp5CE=_tmp5CD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CF=*_tmp5CE;
union Cyc_CfFlowInfo_FlowInfo _tmp5D0=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5CF,_tmp580)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp57F,_tmp5D0);
return _tmp5CB;};};};}_LL34F: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp583=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp583->tag != 6)goto _LL351;else{if(_tmp583->f1 != 0)goto _LL351;}}_LL350:
# 2357
 return Cyc_CfFlowInfo_BottomFL();_LL351: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp584=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp584->tag != 11)goto _LL353;else{_tmp585=_tmp584->f1;if(_tmp584->f2 == 0)goto _LL353;_tmp586=*_tmp584->f2;}}_LL352: {
# 2359
struct _RegionHandle _tmp5D1=_new_region("temp");struct _RegionHandle*temp=& _tmp5D1;_push_region(temp);
{struct _tuple22 _stmttmp82=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp585,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5D3;struct Cyc_List_List*_tmp5D4;struct _tuple22 _tmp5D2=_stmttmp82;_tmp5D3=_tmp5D2.f1;_tmp5D4=_tmp5D2.f2;
# 2362
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp586->body);{
# 2364
struct Cyc_List_List*_tmp5D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp586->pat_vars))->v)).f1);
_tmp5D3=Cyc_NewControlFlow_add_vars(_tmp565,_tmp5D3,_tmp5D5,_tmp565->unknown_all,s->loc,0);
# 2367
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp586->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp83=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D8;struct _tuple23*_tmp5D6=_stmttmp83;_tmp5D7=_tmp5D6->f1;_tmp5D8=_tmp5D6->f2;
if(_tmp5D7 != 0){
_tmp5D3=Cyc_NewControlFlow_do_initialize_var(_tmp565,env,_tmp5D3,*_tmp5D7,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp585))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5D4))->hd,s->loc);
_tmp5D4=_tmp5D4->tl;
_tmp585=_tmp585->tl;}}}
# 2375
Cyc_NewControlFlow_update_flow(env,_tmp586->body,_tmp5D3);{
union Cyc_CfFlowInfo_FlowInfo _tmp5D9=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5D9;};};}
# 2360
;_pop_region(temp);}_LL353: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp587=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp587->tag != 6)goto _LL355;else{_tmp588=_tmp587->f1;}}_LL354:
# 2378
 _tmp58A=_tmp588;goto _LL356;_LL355: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp589=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp589->tag != 7)goto _LL357;else{_tmp58A=_tmp589->f1;}}_LL356:
 _tmp58C=_tmp58A;goto _LL358;_LL357: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp58B->tag != 8)goto _LL359;else{_tmp58C=_tmp58B->f2;}}_LL358:
# 2383
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5DA=_tmp563->encloser;
struct Cyc_Absyn_Stmt*_tmp5DB=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp58C)))->encloser;
while(_tmp5DB != _tmp5DA){
struct Cyc_Absyn_Stmt*_tmp5DC=(Cyc_NewControlFlow_get_stmt_annot(_tmp5DA))->encloser;
if(_tmp5DC == _tmp5DA){
{const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_Tcutil_terr(s->loc,((_tmp8C3="goto enters local scope or exception handler",_tag_dyneither(_tmp8C3,sizeof(char),45))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}
break;}
# 2392
_tmp5DA=_tmp5DC;}}
# 2396
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp58C));
# 2398
Cyc_NewControlFlow_update_flow(env,_tmp58C,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL359: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp58D=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp58D->tag != 10)goto _LL35B;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58D->f2;}}_LL35A:
# 2404
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp58F,_tmp58E->loc);_LL35B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp590=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp590->tag != 15)goto _LL35D;else{_tmp591=_tmp590->f1;_tmp592=_tmp590->f2;}}_LL35C: {
# 2409
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp591,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2417
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2420
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp592,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5DF=scs_outflow;_LL36F: if((_tmp5DF.BottomFL).tag != 1)goto _LL371;_LL370:
 goto _LL36E;_LL371:;_LL372: {
const char*_tmp8C6;void*_tmp8C5;(_tmp8C5=0,Cyc_Tcutil_terr(s->loc,((_tmp8C6="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8C6,sizeof(char),42))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}_LL36E:;}
# 2426
outflow=s1_outflow;
# 2428
return outflow;};};}_LL35D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp593=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp593->tag != 12)goto _LL35F;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp594=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp593->f1)->r;if(_tmp594->tag != 2)goto _LL35F;else{if(_tmp594->f2 == 0)goto _LL35F;_tmp595=(struct Cyc_List_List*)(_tmp594->f2)->v;_tmp596=_tmp594->f3;}}_tmp597=(_tmp593->f1)->loc;_tmp598=_tmp593->f2;}}_LL35E: {
# 2438
struct _tuple1 _stmttmp81=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp595);struct Cyc_List_List*_tmp5E3;struct Cyc_List_List*_tmp5E4;struct _tuple1 _tmp5E2=_stmttmp81;_tmp5E3=_tmp5E2.f1;_tmp5E4=_tmp5E2.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp565,env,inflow,_tmp595,_tmp5E3 != 0,_tmp597,_tmp596->loc);{
struct Cyc_List_List*_tmp5E5=env->unique_pat_vars;
{struct _tuple27*_tmp8C9;struct Cyc_List_List*_tmp8C8;env->unique_pat_vars=(
(_tmp8C8=_region_malloc(env->r,sizeof(*_tmp8C8)),((_tmp8C8->hd=((_tmp8C9=_region_malloc(env->r,sizeof(*_tmp8C9)),((_tmp8C9->f1=_tmp598,((_tmp8C9->f2=_tmp5E3,((_tmp8C9->f3=_tmp5E4,_tmp8C9)))))))),((_tmp8C8->tl=_tmp5E5,_tmp8C8))))));}
# 2446
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp598,rval_opt);
env->unique_pat_vars=_tmp5E5;
# 2450
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E3,_tmp5E4,inflow,_tmp597);
# 2454
return inflow;};}_LL35F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp599=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp599->tag != 12)goto _LL361;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp59A=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp599->f1)->r;if(_tmp59A->tag != 5)goto _LL361;else{_tmp59B=_tmp59A->f1;_tmp59C=_tmp59A->f2;}}_tmp59D=_tmp599->f2;}}_LL360: {
# 2458
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp59C->initializer);
struct _tuple17 _stmttmp7D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp5E9;union Cyc_CfFlowInfo_AbsLVal _tmp5EA;struct _tuple17 _tmp5E8=_stmttmp7D;_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;{
# 2463
struct _tuple17 _tmp8CA;struct _tuple17 _stmttmp7E=(_tmp8CA.f1=_tmp5E9,((_tmp8CA.f2=_tmp5EA,_tmp8CA)));struct _tuple17 _tmp5EB=_stmttmp7E;struct Cyc_Dict_Dict _tmp5EC;struct Cyc_List_List*_tmp5ED;struct Cyc_CfFlowInfo_Place*_tmp5EE;_LL374: if(((_tmp5EB.f1).ReachableFL).tag != 2)goto _LL376;_tmp5EC=((struct _tuple14)((_tmp5EB.f1).ReachableFL).val).f1;_tmp5ED=((struct _tuple14)((_tmp5EB.f1).ReachableFL).val).f2;if(((_tmp5EB.f2).PlaceL).tag != 1)goto _LL376;_tmp5EE=(struct Cyc_CfFlowInfo_Place*)((_tmp5EB.f2).PlaceL).val;_LL375: {
# 2465
void*_tmp5EF=Cyc_CfFlowInfo_lookup_place(_tmp5EC,_tmp5EE);
void*t=(void*)_check_null(e->topt);
void*_tmp5F0=Cyc_CfFlowInfo_make_unique_consumed(_tmp565,t,e,env->iteration_num,_tmp5EF);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8CD;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8CC;_tmp5F0=(void*)((_tmp8CC=_region_malloc(_tmp565->r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=8,((_tmp8CD.f1=_tmp59C,((_tmp8CD.f2=_tmp5F0,_tmp8CD)))))),_tmp8CC))));}
_tmp5EC=Cyc_CfFlowInfo_assign_place(_tmp565,e->loc,_tmp5EC,env->all_changed,_tmp5EE,_tmp5F0);
_tmp5E9=Cyc_CfFlowInfo_ReachableFL(_tmp5EC,_tmp5ED);{
# 2478
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp7F=Cyc_CfFlowInfo_initlevel(_tmp565,_tmp5EC,_tmp5EF);switch(_stmttmp7F){case Cyc_CfFlowInfo_AllIL: _LL37A:
 leaf=_tmp565->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL37B:
 leaf=_tmp565->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL37C:
 leaf=_tmp565->unknown_this;break;}}{
# 2484
void*_tmp5F3=Cyc_CfFlowInfo_typ_to_absrval(_tmp565,t,0,leaf);
_tmp5E9=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5E9,_tmp5EF);{
struct Cyc_List_List _tmp8CE;struct Cyc_List_List _tmp5F4=(_tmp8CE.hd=_tmp59C,((_tmp8CE.tl=0,_tmp8CE)));
# 2488
_tmp5E9=Cyc_NewControlFlow_add_vars(_tmp565,_tmp5E9,& _tmp5F4,_tmp565->unknown_all,s->loc,0);
# 2492
_tmp5E9=Cyc_NewControlFlow_anal_stmt(env,_tmp5E9,_tmp59D,rval_opt);{
# 2494
union Cyc_CfFlowInfo_FlowInfo _tmp5F5=_tmp5E9;struct Cyc_Dict_Dict _tmp5F6;struct Cyc_List_List*_tmp5F7;_LL37F: if((_tmp5F5.ReachableFL).tag != 2)goto _LL381;_tmp5F6=((struct _tuple14)(_tmp5F5.ReachableFL).val).f1;_tmp5F7=((struct _tuple14)(_tmp5F5.ReachableFL).val).f2;_LL380:
# 2497
{void*_stmttmp80=Cyc_CfFlowInfo_lookup_place(_tmp5F6,_tmp5EE);void*_tmp5F8=_stmttmp80;struct Cyc_Absyn_Vardecl*_tmp5FA;_LL384: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5F9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F8;if(_tmp5F9->tag != 8)goto _LL386;else{_tmp5FA=_tmp5F9->f1;}}_LL385:
# 2499
 if(_tmp5FA != _tmp59C)goto _LL387;
goto _LL383;_LL386:;_LL387:
# 2502
{const char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(s->loc,((_tmp8D1="aliased location was overwritten",_tag_dyneither(_tmp8D1,sizeof(char),33))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}
goto _LL383;_LL383:;}
# 2505
_tmp5F6=Cyc_CfFlowInfo_assign_place(_tmp565,s->loc,_tmp5F6,env->all_changed,_tmp5EE,_tmp5F3);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5F6,_tmp5F7);
# 2510
return outflow;_LL381:;_LL382:
 return _tmp5E9;_LL37E:;};};};};}_LL376: if(((_tmp5EB.f1).BottomFL).tag != 1)goto _LL378;_LL377:
# 2513
 return _tmp5E9;_LL378:;_LL379:
{const char*_tmp8D4;void*_tmp8D3;(_tmp8D3=0,Cyc_Tcutil_terr(e->loc,((_tmp8D4="bad alias expression--no unique path found",_tag_dyneither(_tmp8D4,sizeof(char),43))),_tag_dyneither(_tmp8D3,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL373:;};}_LL361: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp59E->tag != 12)goto _LL363;else{_tmp59F=_tmp59E->f1;_tmp5A0=_tmp59E->f2;}}_LL362:
# 2519
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp59F),_tmp5A0,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp59F);
return outflow;_LL363: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5A1=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp5A1->tag != 13)goto _LL365;else{_tmp5A2=_tmp5A1->f2;}}_LL364:
# 2524
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A2,rval_opt);_LL365: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5A3=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp5A3->tag != 16)goto _LL367;else{_tmp5A4=_tmp5A3->f1;}}_LL366: {
# 2527
struct _tuple15 _stmttmp7B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A4);union Cyc_CfFlowInfo_FlowInfo _tmp602;void*_tmp603;struct _tuple15 _tmp601=_stmttmp7B;_tmp602=_tmp601.f1;_tmp603=_tmp601.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp604=Cyc_NewControlFlow_use_Rval(env,_tmp5A4->loc,_tmp602,_tmp603);
void*_stmttmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp5A4->topt));void*_tmp605=_stmttmp7C;void*_tmp607;_LL389: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp605;if(_tmp606->tag != 15)goto _LL38B;else{_tmp607=(void*)_tmp606->f1;}}_LL38A:
# 2531
 return Cyc_CfFlowInfo_kill_flow_region(_tmp565,_tmp602,_tmp607);_LL38B:;_LL38C: {
struct Cyc_Core_Impossible_exn_struct _tmp8DA;const char*_tmp8D9;struct Cyc_Core_Impossible_exn_struct*_tmp8D8;(int)_throw((void*)((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8DA.tag=Cyc_Core_Impossible,((_tmp8DA.f1=((_tmp8D9="anal_stmt -- reset_region",_tag_dyneither(_tmp8D9,sizeof(char),26))),_tmp8DA)))),_tmp8D8)))));}_LL388:;};}_LL367:;_LL368: {
# 2535
struct Cyc_Core_Impossible_exn_struct _tmp8E0;const char*_tmp8DF;struct Cyc_Core_Impossible_exn_struct*_tmp8DE;(int)_throw((void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8E0.tag=Cyc_Core_Impossible,((_tmp8E0.f1=((_tmp8DF="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8DF,sizeof(char),56))),_tmp8E0)))),_tmp8DE)))));}_LL33C:;};}
# 2540
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2543
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp60E=env->fenv;
void*_stmttmp8E=decl->r;void*_tmp60F=_stmttmp8E;struct Cyc_Absyn_Vardecl*_tmp611;struct Cyc_List_List*_tmp613;struct Cyc_Absyn_Fndecl*_tmp615;struct Cyc_Absyn_Tvar*_tmp617;struct Cyc_Absyn_Vardecl*_tmp618;int _tmp619;struct Cyc_Absyn_Exp*_tmp61A;_LL38E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp610=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60F;if(_tmp610->tag != 0)goto _LL390;else{_tmp611=_tmp610->f1;}}_LL38F: {
# 2553
struct Cyc_List_List _tmp8E1;struct Cyc_List_List _tmp61B=(_tmp8E1.hd=_tmp611,((_tmp8E1.tl=0,_tmp8E1)));
inflow=Cyc_NewControlFlow_add_vars(_tmp60E,inflow,& _tmp61B,_tmp60E->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp61C=_tmp611->initializer;
if(_tmp61C == 0)
return inflow;{
struct _tuple15 _stmttmp90=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp61C);union Cyc_CfFlowInfo_FlowInfo _tmp61E;void*_tmp61F;struct _tuple15 _tmp61D=_stmttmp90;_tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp60E,env,_tmp61E,_tmp611,_tmp61C,_tmp61F,decl->loc);};};}_LL390: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp612=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60F;if(_tmp612->tag != 3)goto _LL392;else{_tmp613=_tmp612->f1;}}_LL391:
# 2562
 return Cyc_NewControlFlow_add_vars(_tmp60E,inflow,_tmp613,_tmp60E->unknown_none,decl->loc,0);_LL392: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp614=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60F;if(_tmp614->tag != 1)goto _LL394;else{_tmp615=_tmp614->f1;}}_LL393:
# 2565
 Cyc_NewControlFlow_check_nested_fun(_tmp60E,inflow,_tmp615);{
void*t=(void*)_check_null(_tmp615->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp621=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp615->fn_vardecl);
# 2571
struct Cyc_List_List*_tmp8E2;return Cyc_NewControlFlow_add_vars(_tmp60E,inflow,((_tmp8E2=_region_malloc(env->r,sizeof(*_tmp8E2)),((_tmp8E2->hd=_tmp621,((_tmp8E2->tl=0,_tmp8E2)))))),_tmp60E->unknown_all,decl->loc,0);};_LL394: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp616=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60F;if(_tmp616->tag != 4)goto _LL396;else{_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;_tmp619=_tmp616->f3;_tmp61A=_tmp616->f4;}}_LL395:
# 2574
 if(_tmp61A != 0){
struct Cyc_Absyn_Exp*_tmp623=_tmp61A;
struct _tuple15 _stmttmp8F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp623);union Cyc_CfFlowInfo_FlowInfo _tmp625;void*_tmp626;struct _tuple15 _tmp624=_stmttmp8F;_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp623->loc,_tmp625,_tmp626);}{
# 2579
struct Cyc_List_List _tmp8E3;struct Cyc_List_List _tmp627=(_tmp8E3.hd=_tmp618,((_tmp8E3.tl=0,_tmp8E3)));
return Cyc_NewControlFlow_add_vars(_tmp60E,inflow,& _tmp627,_tmp60E->unknown_all,decl->loc,0);};_LL396:;_LL397: {
# 2583
struct Cyc_Core_Impossible_exn_struct _tmp8E9;const char*_tmp8E8;struct Cyc_Core_Impossible_exn_struct*_tmp8E7;(int)_throw((void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E9.tag=Cyc_Core_Impossible,((_tmp8E9.f1=((_tmp8E8="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8E8,sizeof(char),35))),_tmp8E9)))),_tmp8E7)))));}_LL38D:;}
# 2591
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp62C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp62C;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2593
;_pop_region(frgn);}
# 2598
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2602
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2606
struct _RegionHandle*_tmp62D=fenv->r;
unsigned int _tmp62E=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp62E,1);{
# 2612
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp62F=inflow;struct Cyc_Dict_Dict _tmp630;struct Cyc_List_List*_tmp631;_LL399: if((_tmp62F.BottomFL).tag != 1)goto _LL39B;_LL39A: {
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8EC="check_fun",_tag_dyneither(_tmp8EC,sizeof(char),10))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}_LL39B: if((_tmp62F.ReachableFL).tag != 2)goto _LL398;_tmp630=((struct _tuple14)(_tmp62F.ReachableFL).val).f1;_tmp631=((struct _tuple14)(_tmp62F.ReachableFL).val).f2;_LL39C: {
# 2619
struct Cyc_List_List*atts;
{void*_stmttmp91=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp634=_stmttmp91;struct Cyc_List_List*_tmp636;_LL39E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp635=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp634;if(_tmp635->tag != 9)goto _LL3A0;else{_tmp636=(_tmp635->f1).attributes;}}_LL39F:
 atts=_tmp636;goto _LL39D;_LL3A0:;_LL3A1: {
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8EF="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8EF,sizeof(char),50))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}_LL39D:;}
# 2624
for(0;atts != 0;atts=atts->tl){
void*_stmttmp92=(void*)atts->hd;void*_tmp639=_stmttmp92;int _tmp63B;int _tmp63D;int _tmp63F;_LL3A3: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp63A=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp639;if(_tmp63A->tag != 21)goto _LL3A5;else{_tmp63B=_tmp63A->f1;}}_LL3A4: {
# 2627
unsigned int j=(unsigned int)_tmp63B;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,Cyc_Tcutil_terr(_tmp62E,((_tmp8F2="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8F2,sizeof(char),52))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}
continue;}{
# 2633
struct Cyc_Absyn_Vardecl*_tmp642=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp642->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,Cyc_Tcutil_terr(_tmp62E,((_tmp8F5="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8F5,sizeof(char),52))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}
continue;}{
# 2639
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp645=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8F8;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8F7;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp646=(_tmp8F7=_region_malloc(_tmp62D,sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=2,((_tmp8F8.f1=(int)j,((_tmp8F8.f2=t,_tmp8F8)))))),_tmp8F7)));
struct Cyc_CfFlowInfo_Place*_tmp8F9;struct Cyc_CfFlowInfo_Place*_tmp647=(_tmp8F9=_region_malloc(_tmp62D,sizeof(*_tmp8F9)),((_tmp8F9->root=(void*)_tmp646,((_tmp8F9->fields=0,_tmp8F9)))));
_tmp630=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp630,(void*)_tmp646,_tmp645);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8FF;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8FE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8FC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8FB;_tmp630=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp630,(void*)((_tmp8FB=_region_malloc(_tmp62D,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=0,((_tmp8FC.f1=_tmp642,_tmp8FC)))),_tmp8FB)))),(void*)((_tmp8FE=_region_malloc(_tmp62D,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=5,((_tmp8FF.f1=_tmp647,_tmp8FF)))),_tmp8FE)))));}
goto _LL3A2;};};}_LL3A5: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp63C=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp639;if(_tmp63C->tag != 20)goto _LL3A7;else{_tmp63D=_tmp63C->f1;}}_LL3A6: {
# 2649
unsigned int j=(unsigned int)_tmp63D;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp902;void*_tmp901;(_tmp901=0,Cyc_Tcutil_terr(_tmp62E,((_tmp902="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp902,sizeof(char),51))),_tag_dyneither(_tmp901,sizeof(void*),0)));}
# 2653
continue;}{
# 2655
struct Cyc_Absyn_Vardecl*_tmp651=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_stmttmp93=Cyc_Tcutil_compress(_tmp651->type);void*_tmp652=_stmttmp93;void*_tmp654;union Cyc_Absyn_Constraint*_tmp655;union Cyc_Absyn_Constraint*_tmp656;union Cyc_Absyn_Constraint*_tmp657;_LL3AC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp653=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp652;if(_tmp653->tag != 5)goto _LL3AE;else{_tmp654=(_tmp653->f1).elt_typ;_tmp655=((_tmp653->f1).ptr_atts).nullable;_tmp656=((_tmp653->f1).ptr_atts).bounds;_tmp657=((_tmp653->f1).ptr_atts).zero_term;}}_LL3AD:
# 2658
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp655)){
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(_tmp62E,((_tmp905="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp905,sizeof(char),55))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
# 2661
if(!Cyc_Tcutil_is_bound_one(_tmp656)){
const char*_tmp908;void*_tmp907;(_tmp907=0,Cyc_Tcutil_terr(_tmp62E,((_tmp908="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp908,sizeof(char),57))),_tag_dyneither(_tmp907,sizeof(void*),0)));}
# 2664
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp657)){
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_terr(_tmp62E,((_tmp90B="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp90B,sizeof(char),77))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}{
# 2667
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp90E;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp90D;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65E=(_tmp90D=_region_malloc(_tmp62D,sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=2,((_tmp90E.f1=(int)j,((_tmp90E.f2=_tmp654,_tmp90E)))))),_tmp90D)));
struct Cyc_CfFlowInfo_Place*_tmp90F;struct Cyc_CfFlowInfo_Place*_tmp65F=(_tmp90F=_region_malloc(_tmp62D,sizeof(*_tmp90F)),((_tmp90F->root=(void*)_tmp65E,((_tmp90F->fields=0,_tmp90F)))));
_tmp630=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp630,(void*)_tmp65E,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp654,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp915;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp914;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp912;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp911;_tmp630=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp630,(void*)((_tmp911=_region_malloc(_tmp62D,sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=0,((_tmp912.f1=_tmp651,_tmp912)))),_tmp911)))),(void*)((_tmp914=_region_malloc(_tmp62D,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=5,((_tmp915.f1=_tmp65F,_tmp915)))),_tmp914)))));}
{struct Cyc_List_List*_tmp916;param_roots=((_tmp916=_region_malloc(_tmp62D,sizeof(*_tmp916)),((_tmp916->hd=_tmp65F,((_tmp916->tl=param_roots,_tmp916))))));}
goto _LL3AB;};_LL3AE:;_LL3AF: {
const char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_Tcutil_terr(_tmp62E,((_tmp919="initializes attribute on non-pointer",_tag_dyneither(_tmp919,sizeof(char),37))),_tag_dyneither(_tmp918,sizeof(void*),0)));}_LL3AB:;}
# 2675
goto _LL3A2;};}_LL3A7: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp63E=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp639;if(_tmp63E->tag != 22)goto _LL3A9;else{_tmp63F=_tmp63E->f1;}}_LL3A8: {
# 2677
unsigned int j=(unsigned int)_tmp63F;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp91C;void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_terr(_tmp62E,((_tmp91C="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp91C,sizeof(char),49))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}
continue;}{
# 2682
struct Cyc_Absyn_Vardecl*_tmp66C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp66C->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp91F;void*_tmp91E;(_tmp91E=0,Cyc_Tcutil_terr(_tmp62E,((_tmp91F="noconsume attribute requires no-alias pointer",_tag_dyneither(_tmp91F,sizeof(char),46))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
continue;}{
# 2689
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp922;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp921;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66F=(_tmp921=_region_malloc(_tmp62D,sizeof(*_tmp921)),((_tmp921[0]=((_tmp922.tag=0,((_tmp922.f1=_tmp66C,_tmp922)))),_tmp921)));
struct Cyc_CfFlowInfo_Place*_tmp923;struct Cyc_CfFlowInfo_Place*_tmp670=(_tmp923=_region_malloc(_tmp62D,sizeof(*_tmp923)),((_tmp923->root=(void*)_tmp66F,((_tmp923->fields=0,_tmp923)))));
{struct Cyc_List_List*_tmp924;noconsume_roots=((_tmp924=_region_malloc(_tmp62D,sizeof(*_tmp924)),((_tmp924->hd=_tmp670,((_tmp924->tl=noconsume_roots,_tmp924))))));}
goto _LL3A2;};};}_LL3A9:;_LL3AA:
 goto _LL3A2;_LL3A2:;}
# 2695
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp630,_tmp631);}_LL398:;}{
# 2698
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp62D,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp925;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp925=_region_malloc(_tmp62D,sizeof(*_tmp925)),((_tmp925->r=_tmp62D,((_tmp925->fenv=fenv,((_tmp925->iterate_again=1,((_tmp925->iteration_num=0,((_tmp925->in_try=0,((_tmp925->tryflow=inflow,((_tmp925->all_changed=0,((_tmp925->noreturn=noreturn,((_tmp925->unique_pat_vars=0,((_tmp925->param_roots=param_roots,((_tmp925->noconsume_params=noconsume_roots,((_tmp925->flow_table=flow_table,_tmp925)))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2708
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2711
union Cyc_CfFlowInfo_FlowInfo _tmp675=outflow;_LL3B1: if((_tmp675.BottomFL).tag != 1)goto _LL3B3;_LL3B2:
 goto _LL3B0;_LL3B3:;_LL3B4:
# 2714
 Cyc_NewControlFlow_check_init_params(_tmp62E,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp62E);
# 2718
if(noreturn){
const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_terr(_tmp62E,((_tmp928="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp928,sizeof(char),46))),_tag_dyneither(_tmp927,sizeof(void*),0)));}else{
# 2721
void*_stmttmp94=Cyc_Tcutil_compress(fd->ret_type);void*_tmp678=_stmttmp94;_LL3B6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp679=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp678;if(_tmp679->tag != 0)goto _LL3B8;}_LL3B7:
 goto _LL3B5;_LL3B8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp678;if(_tmp67A->tag != 7)goto _LL3BA;}_LL3B9:
 goto _LL3BB;_LL3BA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp678;if(_tmp67B->tag != 6)goto _LL3BC;}_LL3BB:
# 2725
{const char*_tmp92B;void*_tmp92A;(_tmp92A=0,Cyc_Tcutil_warn(_tmp62E,((_tmp92B="function may not return a value",_tag_dyneither(_tmp92B,sizeof(char),32))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}goto _LL3B5;_LL3BC:;_LL3BD:
# 2727
{const char*_tmp92E;void*_tmp92D;(_tmp92D=0,Cyc_Tcutil_terr(_tmp62E,((_tmp92E="function may not return a value",_tag_dyneither(_tmp92E,sizeof(char),32))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}goto _LL3B5;_LL3B5:;}
# 2729
goto _LL3B0;_LL3B0:;};};};}
# 2733
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_stmttmp95=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp681=_stmttmp95;struct Cyc_Absyn_Fndecl*_tmp683;struct Cyc_List_List*_tmp685;struct Cyc_List_List*_tmp687;struct Cyc_List_List*_tmp689;_LL3BF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp682=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp681;if(_tmp682->tag != 1)goto _LL3C1;else{_tmp683=_tmp682->f1;}}_LL3C0:
 Cyc_NewControlFlow_check_fun(_tmp683);goto _LL3BE;_LL3C1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp684=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp681;if(_tmp684->tag != 12)goto _LL3C3;else{_tmp685=_tmp684->f1;}}_LL3C2:
 _tmp687=_tmp685;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp686=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp681;if(_tmp686->tag != 11)goto _LL3C5;else{_tmp687=_tmp686->f2;}}_LL3C4:
 _tmp689=_tmp687;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp688=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp681;if(_tmp688->tag != 10)goto _LL3C7;else{_tmp689=_tmp688->f2;}}_LL3C6:
 Cyc_NewControlFlow_cf_check(_tmp689);goto _LL3BE;_LL3C7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp68A=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp681;if(_tmp68A->tag != 13)goto _LL3C9;}_LL3C8:
 goto _LL3BE;_LL3C9:;_LL3CA:
 goto _LL3BE;_LL3BE:;}}
