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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 302
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922 "absyn.h"
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 967
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1164
int Cyc_Absyn_is_nontagged_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 228 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 323
int Cyc_Tcutil_is_noreturn(void*);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 102
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct _dyneither_ptr f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 185
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 188
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 191
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 197
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 200
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 222 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 225
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 232
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 236
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 241
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,struct _tuple14 pr2);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 252
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 61 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 65
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 73
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 78
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp6F6;struct Cyc_NewControlFlow_CFStmtAnnot _tmp6F5;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp6F4;enclosee->annot=(void*)((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F6.tag=Cyc_NewControlFlow_CFAnnot,((_tmp6F6.f1=((_tmp6F5.encloser=encloser,((_tmp6F5.visited=0,_tmp6F5)))),_tmp6F6)))),_tmp6F4))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 104 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 112
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 121
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp6FC;const char*_tmp6FB;struct Cyc_Core_Impossible_exn_struct*_tmp6FA;(int)_throw((void*)((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=((_tmp6FC.tag=Cyc_Core_Impossible,((_tmp6FC.f1=((_tmp6FB="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp6FB,sizeof(char),37))),_tmp6FC)))),_tmp6FA)))));}_LL0:;}
# 128
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp6FD;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp6FD=_region_malloc(env->r,sizeof(*_tmp6FD)),((_tmp6FD[0]=Cyc_CfFlowInfo_BottomFL(),_tmp6FD)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 135
return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 138
static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 142
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 145
_tmpA->visited=env->iteration_num;{
struct _tuple17 _tmp6FE;return(_tmp6FE.f1=_tmpA,((_tmp6FE.f2=_tmpB,_tmp6FE)));};}
# 157 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 165
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 172
static void Cyc_NewControlFlow_check_unique_root(struct _tuple18*ckenv,void*root,void*rval){
# 174
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple18*_tmpD=ckenv;_tmpE=(*_tmpD).f1;_tmpF=(*_tmpD).f2;_tmp10=(*_tmpD).f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp13;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp12=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 177
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp13->type)){
retry: {void*_tmp14=rval;void*_tmp16;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 8)goto _LLD;else{_tmp16=(void*)_tmp15->f2;}}_LLC:
 rval=_tmp16;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp18->tag != 3)goto _LL11;else{if(_tmp18->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp19->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 186
{const char*_tmp702;void*_tmp701[1];struct Cyc_String_pa_PrintArg_struct _tmp700;(_tmp700.tag=0,((_tmp700.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp701[0]=& _tmp700,Cyc_Tcutil_warn(_tmpF,((_tmp702="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp702,sizeof(char),60))),_tag_dyneither(_tmp701,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 190
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 198
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1D=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1E=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1F=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1E);
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple16 _tmp703;struct _tuple16 _tmp21=(_tmp703.f1=flow,((_tmp703.f2=_tmp1F,_tmp703)));struct Cyc_Dict_Dict _tmp22;struct Cyc_Dict_Dict _tmp23;_LL16: if(((_tmp21.f1).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple13)((_tmp21.f1).ReachableFL).val).f1;if(((_tmp21.f2).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple13)((_tmp21.f2).ReachableFL).val).f1;_LL17: {
# 205
struct _tuple18 _tmp704;struct _tuple18 _tmp24=(_tmp704.f1=env,((_tmp704.f2=s->loc,((_tmp704.f3=_tmp23,_tmp704)))));
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp24,_tmp22);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 213
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1F,*_tmp1E)){
# 220
*_tmp1E=_tmp1F;
# 224
if(_tmp1D->visited == env->iteration_num)
# 226
env->iterate_again=1;}}
# 231
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 235
union Cyc_CfFlowInfo_FlowInfo _tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;_LL1B: if((_tmp26.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple13)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple13)(_tmp26.ReachableFL).val).f2;_LL1E:
# 238
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp707;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp706;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp706=_region_malloc(fenv->r,sizeof(*_tmp706)),((_tmp706[0]=((_tmp707.tag=0,((_tmp707.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp707)))),_tmp706)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct _dyneither_ptr locname=(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp70A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp709;_tmp2A=(void*)((_tmp709=_region_malloc(fenv->r,sizeof(*_tmp709)),((_tmp709[0]=((_tmp70A.tag=8,((_tmp70A.f1=locname,((_tmp70A.f2=(void*)_tmp2A,_tmp70A)))))),_tmp709))));}
# 246
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 248
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp2F=inflow;struct Cyc_Dict_Dict _tmp30;struct Cyc_List_List*_tmp31;_LL20: if((_tmp2F.BottomFL).tag != 1)goto _LL22;_LL21:
 return Cyc_CfFlowInfo_BottomFL();_LL22: if((_tmp2F.ReachableFL).tag != 2)goto _LL1F;_tmp30=((struct _tuple13)(_tmp2F.ReachableFL).val).f1;_tmp31=((struct _tuple13)(_tmp2F.ReachableFL).val).f2;_LL23:
# 256
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp70D;void*_tmp70C;(_tmp70C=0,Cyc_Tcutil_terr(loc,((_tmp70D="expression may not be fully initialized",_tag_dyneither(_tmp70D,sizeof(char),40))),_tag_dyneither(_tmp70C,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp34=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp30,env->all_changed,r);
if(_tmp30.t == _tmp34.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp35=Cyc_CfFlowInfo_ReachableFL(_tmp34,_tmp31);
Cyc_NewControlFlow_update_tryflow(env,_tmp35);
return _tmp35;};};_LL1F:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 266
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp70E;struct _tuple0 _tmp37=(_tmp70E.f1=Cyc_Tcutil_compress(t),((_tmp70E.f2=r,_tmp70E)));void*_tmp3C;struct Cyc_Absyn_Datatypefield*_tmp3E;struct _dyneither_ptr _tmp40;struct Cyc_List_List*_tmp42;struct _dyneither_ptr _tmp44;union Cyc_Absyn_AggrInfoU _tmp46;struct Cyc_List_List*_tmp47;struct _dyneither_ptr _tmp49;enum Cyc_Absyn_AggrKind _tmp4B;struct Cyc_List_List*_tmp4C;struct _dyneither_ptr _tmp4E;_LL25: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp38=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp38->tag != 3)goto _LL27;else{if(_tmp38->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL27;}}_LL26:
 return;_LL27: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp39=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp39->tag != 0)goto _LL29;}_LL28:
 return;_LL29: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3A->tag != 7)goto _LL2B;}_LL2A:
 return;_LL2B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3B->tag != 8)goto _LL2D;else{_tmp3C=(void*)_tmp3B->f2;}}_LL2C:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp3C);return;_LL2D:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp37.f1;if(_tmp3D->tag != 4)goto _LL2F;else{if((((_tmp3D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F;_tmp3E=((struct _tuple2)(((_tmp3D->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp3F->tag != 6)goto _LL2F;else{_tmp40=_tmp3F->f2;}};_LL2E:
# 273
 if(_tmp3E->typs == 0)
return;
_tmp42=_tmp3E->typs;_tmp44=_tmp40;goto _LL30;_LL2F:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37.f1;if(_tmp41->tag != 10)goto _LL31;else{_tmp42=_tmp41->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp43=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp43->tag != 6)goto _LL31;else{_tmp44=_tmp43->f2;}};_LL30: {
# 277
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp42);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp42))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp44,sizeof(void*),i)));}}
# 281
return;}_LL31:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37.f1;if(_tmp45->tag != 11)goto _LL33;else{_tmp46=(_tmp45->f1).aggr_info;_tmp47=(_tmp45->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp48=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp48->tag != 6)goto _LL33;else{_tmp49=_tmp48->f2;}};_LL32: {
# 284
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp46);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp47);
{int i=0;for(0;i < _get_dyneither_size(_tmp49,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp49.curr)[i]);}}
# 294
_npop_handler(0);return;}
# 288
;_pop_region(temp);};}_LL33:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37.f1;if(_tmp4A->tag != 12)goto _LL35;else{_tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp4D->tag != 6)goto _LL35;else{_tmp4E=_tmp4D->f2;}};_LL34:
# 297
{int i=0;for(0;i < _get_dyneither_size(_tmp4E,sizeof(void*));(i ++,_tmp4C=_tmp4C->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp4C))->hd)->type,((void**)_tmp4E.curr)[i]);}}
return;_LL35:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37.f1;if(_tmp4F->tag != 5)goto _LL37;}if(!
Cyc_Tcutil_is_noalias_pointer(t))goto _LL37;_LL36:
{const char*_tmp711;void*_tmp710;(_tmp710=0,Cyc_Tcutil_warn(loc,((_tmp711="argument may still contain unique pointers",_tag_dyneither(_tmp711,sizeof(char),43))),_tag_dyneither(_tmp710,sizeof(void*),0)));}
return;_LL37:;_LL38:
 return;_LL24:;}
# 349 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp56=inflow;struct Cyc_Dict_Dict _tmp57;struct Cyc_List_List*_tmp58;_LL3A: if((_tmp56.BottomFL).tag != 1)goto _LL3C;_LL3B:
 return Cyc_CfFlowInfo_BottomFL();_LL3C: if((_tmp56.ReachableFL).tag != 2)goto _LL39;_tmp57=((struct _tuple13)(_tmp56.ReachableFL).val).f1;_tmp58=((struct _tuple13)(_tmp56.ReachableFL).val).f2;_LL3D:
# 353
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp714;void*_tmp713;(_tmp713=0,Cyc_Tcutil_terr(loc,((_tmp714="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp714,sizeof(char),47))),_tag_dyneither(_tmp713,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 357
void*_tmp5B=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp5C=r;void*_tmp5E;struct Cyc_CfFlowInfo_Place*_tmp60;_LL3F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C;if(_tmp5D->tag != 8)goto _LL41;else{_tmp5E=(void*)_tmp5D->f2;}}_LL40:
 r=_tmp5E;goto retry;_LL41: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp5C;if(_tmp5F->tag != 5)goto _LL43;else{_tmp60=_tmp5F->f1;}}_LL42:
# 361
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp5B,Cyc_CfFlowInfo_lookup_place(_tmp57,_tmp60));
goto _LL3E;_LL43:;_LL44:
# 364
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp5B)){
const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_warn(loc,((_tmp717="argument may contain live unique pointers",_tag_dyneither(_tmp717,sizeof(char),42))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL3E:;}{
# 368
struct Cyc_Dict_Dict _tmp63=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp57,env->all_changed,r);
if(_tmp57.t == _tmp63.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp64=Cyc_CfFlowInfo_ReachableFL(_tmp63,_tmp58);
Cyc_NewControlFlow_update_tryflow(env,_tmp64);
return _tmp64;};};};_LL39:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 377
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 384
if(es == 0){
struct _tuple20 _tmp718;return(_tmp718.f1=inflow,((_tmp718.f2=0,_tmp718)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp67;void*_tmp68;struct _tuple14 _tmp66=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp71B;struct _tuple20 _tmp71A;return(_tmp71A.f1=_tmp67,((_tmp71A.f2=((_tmp71B=_region_malloc(rgn,sizeof(*_tmp71B)),((_tmp71B->hd=_tmp68,((_tmp71B->tl=0,_tmp71B)))))),_tmp71A)));};}{
# 390
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 398
{struct Cyc_Dict_Dict*_tmp71C;env->all_changed=((_tmp71C=_region_malloc(env->r,sizeof(*_tmp71C)),((_tmp71C[0]=(env->fenv)->mt_place_set,_tmp71C))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp6D;void*_tmp6E;struct _tuple14 _tmp6C=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
outflow=_tmp6D;
{struct Cyc_List_List*_tmp71D;rvals=((_tmp71D=_region_malloc(rgn,sizeof(*_tmp71D)),((_tmp71D->hd=_tmp6E,((_tmp71D->tl=0,_tmp71D))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 404
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp71E;env->all_changed=((_tmp71E=_region_malloc(env->r,sizeof(*_tmp71E)),((_tmp71E[0]=(env->fenv)->mt_place_set,_tmp71E))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp72;void*_tmp73;struct _tuple14 _tmp71=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;
{struct Cyc_List_List*_tmp71F;rvals=((_tmp71F=_region_malloc(rgn,sizeof(*_tmp71F)),((_tmp71F->hd=_tmp73,((_tmp71F->tl=rvals,_tmp71F))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp72,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 410
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 413
old_inflow=inflow;
# 416
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 421
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 425
struct Cyc_Dict_Dict*_tmp720;env->all_changed=((_tmp720=_region_malloc(env->r,sizeof(*_tmp720)),((_tmp720[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp720))));}
# 427
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple20 _tmp721;return(_tmp721.f1=outflow,((_tmp721.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp721)));};};}
# 433
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 436
struct _RegionHandle _tmp77=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77;_push_region(rgn);
{union Cyc_CfFlowInfo_FlowInfo _tmp79;struct Cyc_List_List*_tmp7A;struct _tuple20 _tmp78=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp7B=_tmp79;struct Cyc_Dict_Dict _tmp7C;_LL46: if((_tmp7B.ReachableFL).tag != 2)goto _LL48;_tmp7C=((struct _tuple13)(_tmp7B.ReachableFL).val).f1;_LL47:
# 441
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp7C,(void*)_tmp7A->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp724;void*_tmp723;(_tmp723=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp724="expression may not be initialized",_tag_dyneither(_tmp724,sizeof(char),34))),_tag_dyneither(_tmp723,sizeof(void*),0)));}}
goto _LL45;_LL48: if((_tmp7B.BottomFL).tag != 1)goto _LL45;_LL49:
 goto _LL45;_LL45:;}{
# 447
struct _tuple14 _tmp725;struct _tuple14 _tmp80=(_tmp725.f1=_tmp79,((_tmp725.f2=(env->fenv)->unknown_all,_tmp725)));_npop_handler(0);return _tmp80;};}
# 437
;_pop_region(rgn);}
# 461 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct _dyneither_ptr locname){
# 467
union Cyc_CfFlowInfo_FlowInfo _tmp81=outflow;struct Cyc_Dict_Dict _tmp82;struct Cyc_List_List*_tmp83;_LL4B: if((_tmp81.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return outflow;_LL4D: if((_tmp81.ReachableFL).tag != 2)goto _LL4A;_tmp82=((struct _tuple13)(_tmp81.ReachableFL).val).f1;_tmp83=((struct _tuple13)(_tmp81.ReachableFL).val).f2;_LL4E: {
# 470
union Cyc_CfFlowInfo_AbsLVal _tmp84=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: if((_tmp84.UnknownL).tag != 2)goto _LL52;_LL51:
# 474
 return outflow;_LL52: if((_tmp84.PlaceL).tag != 1)goto _LL4F;_tmp85=(struct Cyc_CfFlowInfo_Place*)(_tmp84.PlaceL).val;_LL53: {
# 478
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if((char*)locname.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp728;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp727;nzval=(void*)((_tmp727=_region_malloc((env->fenv)->r,sizeof(*_tmp727)),((_tmp727[0]=((_tmp728.tag=8,((_tmp728.f1=locname,((_tmp728.f2=(void*)nzval,_tmp728)))))),_tmp727))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp88=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp82,env->all_changed,_tmp85,nzval),_tmp83);
# 485
return _tmp88;};}_LL4F:;}_LL4A:;}
# 494
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct _dyneither_ptr locname){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp89=outflow;struct Cyc_Dict_Dict _tmp8A;struct Cyc_List_List*_tmp8B;_LL55: if((_tmp89.BottomFL).tag != 1)goto _LL57;_LL56: {
struct _tuple16 _tmp729;return(_tmp729.f1=outflow,((_tmp729.f2=outflow,_tmp729)));}_LL57: if((_tmp89.ReachableFL).tag != 2)goto _LL54;_tmp8A=((struct _tuple13)(_tmp89.ReachableFL).val).f1;_tmp8B=((struct _tuple13)(_tmp89.ReachableFL).val).f2;_LL58: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp8D=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp8E;_LL5A: if((_tmp8D.UnknownL).tag != 2)goto _LL5C;_LL5B: {
struct _tuple16 _tmp72A;return(_tmp72A.f1=outflow,((_tmp72A.f2=outflow,_tmp72A)));}_LL5C: if((_tmp8D.PlaceL).tag != 1)goto _LL59;_tmp8E=(struct Cyc_CfFlowInfo_Place*)(_tmp8D.PlaceL).val;_LL5D: {
# 503
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if((char*)locname.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp72D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp72C;nzval=(void*)((_tmp72C=_region_malloc((env->fenv)->r,sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp72D.tag=8,((_tmp72D.f1=locname,((_tmp72D.f2=(void*)nzval,_tmp72D)))))),_tmp72C))));}{
struct _tuple16 _tmp72E;return
(_tmp72E.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,nzval),_tmp8B),((_tmp72E.f2=
# 509
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,(env->fenv)->zero),_tmp8B),_tmp72E)));};}_LL59:;}_LL54:;}
# 515
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 521
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 525
void*_tmp95=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp97;union Cyc_Absyn_Constraint*_tmp98;union Cyc_Absyn_Constraint*_tmp99;_LL5F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95;if(_tmp96->tag != 5)goto _LL61;else{_tmp97=(_tmp96->f1).elt_typ;_tmp98=((_tmp96->f1).ptr_atts).bounds;_tmp99=((_tmp96->f1).ptr_atts).zero_term;}}_LL60: {
# 527
union Cyc_CfFlowInfo_FlowInfo _tmp9A=f;struct Cyc_Dict_Dict _tmp9B;struct Cyc_List_List*_tmp9C;_LL64: if((_tmp9A.BottomFL).tag != 1)goto _LL66;_LL65: {
# 529
struct _tuple14 _tmp72F;return(_tmp72F.f1=f,((_tmp72F.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp72F)));}_LL66: if((_tmp9A.ReachableFL).tag != 2)goto _LL63;_tmp9B=((struct _tuple13)(_tmp9A.ReachableFL).val).f1;_tmp9C=((struct _tuple13)(_tmp9A.ReachableFL).val).f2;_LL67: {
# 532
struct _dyneither_ptr locname=_tag_dyneither(0,0,0);
{void*_tmp9E=r;struct _dyneither_ptr _tmpA0;void*_tmpA1;_LL69: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9E;if(_tmp9F->tag != 8)goto _LL6B;else{_tmpA0=_tmp9F->f1;_tmpA1=(void*)_tmp9F->f2;}}_LL6A:
 locname=_tmpA0;goto _LL68;_LL6B:;_LL6C:
 goto _LL68;_LL68:;}
# 537
if(Cyc_Tcutil_is_bound_one(_tmp98)){
retry: {void*_tmpA2=r;struct _dyneither_ptr _tmpA4;void*_tmpA5;struct Cyc_CfFlowInfo_Place*_tmpA9;enum Cyc_CfFlowInfo_InitLevel _tmpAC;_LL6E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA3->tag != 8)goto _LL70;else{_tmpA4=_tmpA3->f1;_tmpA5=(void*)_tmpA3->f2;}}_LL6F:
 r=_tmpA5;goto retry;_LL70: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpA6=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA6->tag != 1)goto _LL72;}_LL71:
 goto _LL73;_LL72: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpA7=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA7->tag != 2)goto _LL74;}_LL73:
# 542
{void*_tmpAD=e->annot;struct Cyc_List_List*_tmpAF;_LL7D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpAE=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpAD;if(_tmpAE->tag != Cyc_CfFlowInfo_NotZero)goto _LL7F;else{_tmpAF=_tmpAE->f1;}}_LL7E:
# 544
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpAF))goto _LL80;
goto _LL7C;_LL7F:;_LL80:
# 548
{void*_tmpB0=e->r;_LL82: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpB1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB0;if(_tmpB1->tag != 22)goto _LL84;}_LL83:
# 550
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp732;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp731;e->annot=(void*)((_tmp731=_cycalloc(sizeof(*_tmp731)),((_tmp731[0]=((_tmp732.tag=Cyc_CfFlowInfo_NotZero,((_tmp732.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp732)))),_tmp731))));}
goto _LL81;_LL84:;_LL85:
# 553
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL81;_LL81:;}
# 556
goto _LL7C;_LL7C:;}
# 558
goto _LL6D;_LL74: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA8->tag != 5)goto _LL76;else{_tmpA9=_tmpA8->f1;}}_LL75:
# 560
{void*_tmpB4=e->annot;struct Cyc_List_List*_tmpB6;_LL87: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpB5=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpB4;if(_tmpB5->tag != Cyc_CfFlowInfo_NotZero)goto _LL89;else{_tmpB6=_tmpB5->f1;}}_LL88:
# 562
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpB6))goto _LL8A;
goto _LL86;_LL89:;_LL8A:
# 566
{void*_tmpB7=e->r;_LL8C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpB8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB7;if(_tmpB8->tag != 22)goto _LL8E;}_LL8D:
# 568
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp735;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp734;e->annot=(void*)((_tmp734=_cycalloc(sizeof(*_tmp734)),((_tmp734[0]=((_tmp735.tag=Cyc_CfFlowInfo_NotZero,((_tmp735.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp735)))),_tmp734))));}
goto _LL8B;_LL8E:;_LL8F:
# 571
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8B;_LL8B:;}
# 574
goto _LL86;_LL86:;}{
# 576
struct _tuple14 _tmp736;return(_tmp736.f1=f,((_tmp736.f2=Cyc_CfFlowInfo_lookup_place(_tmp9B,_tmpA9),_tmp736)));};_LL76: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpAA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAA->tag != 0)goto _LL78;}_LL77:
# 578
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp737;return(_tmp737.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp737.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp737)));};_LL78: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpAB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAB->tag != 3)goto _LL7A;else{_tmpAC=_tmpAB->f1;}}_LL79:
# 581
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpAC,locname);goto _LL7B;_LL7A:;_LL7B:
# 583
{void*_tmpBD=e->r;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD;if(_tmpBE->tag != 22)goto _LL93;}_LL92:
# 585
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp73A;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp739;e->annot=(void*)((_tmp739=_cycalloc(sizeof(*_tmp739)),((_tmp739[0]=((_tmp73A.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp73A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp73A)))),_tmp739))));}
goto _LL90;_LL93:;_LL94:
# 588
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL90;_LL90:;}
# 591
goto _LL6D;_LL6D:;}}else{
# 594
void*_tmpC1=e->annot;struct Cyc_List_List*_tmpC3;_LL96: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC1;if(_tmpC2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL98;else{_tmpC3=_tmpC2->f1;}}_LL97:
# 596
 if(!Cyc_CfFlowInfo_same_relns(_tmp9C,_tmpC3))goto _LL99;
goto _LL95;_LL98:;_LL99:
# 599
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp73D;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp73C;e->annot=(void*)((_tmp73C=_cycalloc(sizeof(*_tmp73C)),((_tmp73C[0]=((_tmp73D.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp73D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp73D)))),_tmp73C))));}
goto _LL95;_LL95:;}
# 603
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp9B,r)){case Cyc_CfFlowInfo_NoneIL: _LL9A:
# 605
{void*_tmpC6=r;_LL9D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC6;if(_tmpC7->tag != 7)goto _LL9F;}_LL9E:
# 608
{const char*_tmp740;void*_tmp73F;(_tmp73F=0,Cyc_Tcutil_terr(e->loc,((_tmp740="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp740,sizeof(char),61))),_tag_dyneither(_tmp73F,sizeof(void*),0)));}
goto _LL9C;_LL9F:;_LLA0: {
# 611
const char*_tmp743;void*_tmp742;(_tmp742=0,Cyc_Tcutil_terr(e->loc,((_tmp743="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp743,sizeof(char),46))),_tag_dyneither(_tmp742,sizeof(void*),0)));}_LL9C:;}
# 613
goto _LL9B;case Cyc_CfFlowInfo_AllIL: _LL9B: {
# 615
void*_tmpCC=(env->fenv)->unknown_all;
if((char*)locname.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp746;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp745;_tmpCC=(void*)((_tmp745=_region_malloc((env->fenv)->r,sizeof(*_tmp745)),((_tmp745[0]=((_tmp746.tag=8,((_tmp746.f1=locname,((_tmp746.f2=(void*)_tmpCC,_tmp746)))))),_tmp745))));}{
struct _tuple14 _tmp747;return(_tmp747.f1=f,((_tmp747.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpCC),_tmp747)));};}case Cyc_CfFlowInfo_ThisIL: _LLA1: {
# 620
void*_tmpD0=(env->fenv)->unknown_none;
if((char*)locname.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp74A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp749;_tmpD0=(void*)((_tmp749=_region_malloc((env->fenv)->r,sizeof(*_tmp749)),((_tmp749[0]=((_tmp74A.tag=8,((_tmp74A.f1=locname,((_tmp74A.f2=(void*)_tmpD0,_tmp74A)))))),_tmp749))));}{
struct _tuple14 _tmp74B;return(_tmp74B.f1=f,((_tmp74B.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpD0),_tmp74B)));};}}}_LL63:;}_LL61:;_LL62: {
# 626
struct Cyc_Core_Impossible_exn_struct _tmp751;const char*_tmp750;struct Cyc_Core_Impossible_exn_struct*_tmp74F;(int)_throw((void*)((_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((_tmp751.tag=Cyc_Core_Impossible,((_tmp751.f1=((_tmp750="right deref of non-pointer-type",_tag_dyneither(_tmp750,sizeof(char),32))),_tmp751)))),_tmp74F)))));}_LL5E:;}
# 632
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
void*_tmpD7=e1->r;struct Cyc_Absyn_Vardecl*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpDD;struct Cyc_Absyn_Vardecl*_tmpE0;struct Cyc_Absyn_Vardecl*_tmpE3;_LLA4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpD8->tag != 1)goto _LLA6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpD8->f2);if(_tmpD9->tag != 5)goto _LLA6;else{_tmpDA=_tmpD9->f1;}}}_LLA5:
 _tmpDD=_tmpDA;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpDB->tag != 1)goto _LLA8;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpDB->f2);if(_tmpDC->tag != 4)goto _LLA8;else{_tmpDD=_tmpDC->f1;}}}_LLA7:
 _tmpE0=_tmpDD;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpDE->tag != 1)goto _LLAA;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpDF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpDE->f2);if(_tmpDF->tag != 3)goto _LLAA;else{_tmpE0=_tmpDF->f1;}}}_LLA9:
 _tmpE3=_tmpE0;goto _LLAB;_LLAA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpE1->tag != 1)goto _LLAC;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE2=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpE1->f2);if(_tmpE2->tag != 1)goto _LLAC;else{_tmpE3=_tmpE2->f1;}}}_LLAB:
# 639
 if(!_tmpE3->escapes){
void*_tmpE4=e2->r;struct Cyc_Absyn_Vardecl*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpED;struct Cyc_Absyn_Vardecl*_tmpF0;_LLAF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpE5->tag != 1)goto _LLB1;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpE5->f2);if(_tmpE6->tag != 5)goto _LLB1;else{_tmpE7=_tmpE6->f1;}}}_LLB0:
 _tmpEA=_tmpE7;goto _LLB2;_LLB1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpE8->tag != 1)goto _LLB3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpE8->f2);if(_tmpE9->tag != 4)goto _LLB3;else{_tmpEA=_tmpE9->f1;}}}_LLB2:
 _tmpED=_tmpEA;goto _LLB4;_LLB3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpEB->tag != 1)goto _LLB5;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpEC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEB->f2);if(_tmpEC->tag != 3)goto _LLB5;else{_tmpED=_tmpEC->f1;}}}_LLB4:
 _tmpF0=_tmpED;goto _LLB6;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE4;if(_tmpEE->tag != 1)goto _LLB7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpEF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpEE->f2);if(_tmpEF->tag != 1)goto _LLB7;else{_tmpF0=_tmpEF->f1;}}}_LLB6:
# 645
 if(!_tmpF0->escapes){
# 647
int found=0;
{struct Cyc_List_List*_tmpF1=relns;for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_CfFlowInfo_Reln*_tmpF2=(struct Cyc_CfFlowInfo_Reln*)_tmpF1->hd;
if(_tmpF2->vd == _tmpF0){
union Cyc_CfFlowInfo_RelnOp _tmpF3=_tmpF2->rop;struct Cyc_Absyn_Vardecl*_tmpF4;_LLBA: if((_tmpF3.LessNumelts).tag != 3)goto _LLBC;_tmpF4=(struct Cyc_Absyn_Vardecl*)(_tmpF3.LessNumelts).val;if(!(_tmpF4 == _tmpE3))goto _LLBC;_LLBB:
 return relns;_LLBC:;_LLBD:
 continue;_LLB9:;}}}
# 656
if(!found){
struct Cyc_CfFlowInfo_Reln*_tmp754;struct Cyc_List_List*_tmp753;return(_tmp753=_region_malloc(rgn,sizeof(*_tmp753)),((_tmp753->hd=((_tmp754=_region_malloc(rgn,sizeof(*_tmp754)),((_tmp754->vd=_tmpF0,((_tmp754->rop=Cyc_CfFlowInfo_LessNumelts(_tmpE3),_tmp754)))))),((_tmp753->tl=relns,_tmp753)))));}}
# 659
return relns;_LLB7:;_LLB8:
 return relns;_LLAE:;}else{
# 662
return relns;}_LLAC:;_LLAD:
 return relns;_LLA3:;}
# 669
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 677
union Cyc_CfFlowInfo_FlowInfo _tmpF7=outflow;struct Cyc_Dict_Dict _tmpF8;struct Cyc_List_List*_tmpF9;_LLBF: if((_tmpF7.BottomFL).tag != 1)goto _LLC1;_LLC0: {
# 680
struct _tuple14 _tmp755;return(_tmp755.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp755.f2=rval,_tmp755)));}_LLC1: if((_tmpF7.ReachableFL).tag != 2)goto _LLBE;_tmpF8=((struct _tuple13)(_tmpF7.ReachableFL).val).f1;_tmpF9=((struct _tuple13)(_tmpF7.ReachableFL).val).f2;_LLC2: {
# 682
union Cyc_CfFlowInfo_AbsLVal _tmpFB=lval;struct Cyc_CfFlowInfo_Place*_tmpFC;_LLC4: if((_tmpFB.PlaceL).tag != 1)goto _LLC6;_tmpFC=(struct Cyc_CfFlowInfo_Place*)(_tmpFB.PlaceL).val;_LLC5: {
# 684
struct Cyc_Dict_Dict _tmpFD=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpF8,env->all_changed,_tmpFC,rval);
_tmpF9=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmpF9,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmpFD,_tmpF9);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 690
void*_tmpFE=Cyc_CfFlowInfo_lookup_place(_tmpF8,_tmpFC);_LLC9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpFF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmpFF->tag != 3)goto _LLCB;else{if(_tmpFF->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCB;}}_LLCA:
 goto _LLCC;_LLCB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp100=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp100->tag != 7)goto _LLCD;}_LLCC:
 goto _LLCE;_LLCD: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp101=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp101->tag != 0)goto _LLCF;}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp102=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp102->tag != 8)goto _LLD1;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp103=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp102->f2);if(_tmp103->tag != 3)goto _LLD1;else{if(_tmp103->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLD1;}}}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp104=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp104->tag != 8)goto _LLD3;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp105=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp104->f2);if(_tmp105->tag != 7)goto _LLD3;}}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp106=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFE;if(_tmp106->tag != 8)goto _LLD5;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp107=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp106->f2);if(_tmp107->tag != 0)goto _LLD5;}}_LLD4:
 goto _LLC8;_LLD5:;_LLD6:
# 698
{const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_warn(lexp->loc,((_tmp758="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp758,sizeof(char),43))),_tag_dyneither(_tmp757,sizeof(void*),0)));}
goto _LLC8;_LLC8:;}
# 703
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 705
struct _tuple14 _tmp759;return(_tmp759.f1=outflow,((_tmp759.f2=rval,_tmp759)));};}_LLC6: if((_tmpFB.UnknownL).tag != 2)goto _LLC3;_LLC7: {
# 708
struct _tuple14 _tmp75A;return(_tmp75A.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp75A.f2=rval,_tmp75A)));}_LLC3:;}_LLBE:;}
# 713
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
void*_tmp10C=e->r;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;_LLD8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10D->tag != 1)goto _LLDA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp10E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 4)goto _LLDA;}}_LLD9:
 goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10F->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp110=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 3)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp111->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp112=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp111->f2);if(_tmp112->tag != 5)goto _LLDE;}}_LLDD:
 return 1;_LLDE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp113->tag != 19)goto _LLE0;else{_tmp114=_tmp113->f1;}}_LLDF:
 _tmp116=_tmp114;goto _LLE1;_LLE0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp115->tag != 20)goto _LLE2;else{_tmp116=_tmp115->f1;}}_LLE1:
 _tmp118=_tmp116;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp117->tag != 21)goto _LLE4;else{_tmp118=_tmp117->f1;}}_LLE3:
# 721
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp118);_LLE4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp119->tag != 22)goto _LLE6;else{_tmp11A=_tmp119->f1;}}_LLE5: {
# 723
void*_tmp11D=Cyc_Tcutil_compress((void*)_check_null(_tmp11A->topt));_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11D;if(_tmp11E->tag != 10)goto _LLED;}_LLEC:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp11A);_LLED:;_LLEE:
 return 0;_LLEA:;}_LLE6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp11B->tag != 13)goto _LLE8;else{_tmp11C=_tmp11B->f2;}}_LLE7:
# 727
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp11C);_LLE8:;_LLE9:
 return 0;_LLD7:;}
# 732
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 734
struct Cyc_CfFlowInfo_FlowEnv*_tmp11F=env->fenv;
# 738
while(1){
union Cyc_CfFlowInfo_FlowInfo*_tmp121;struct _tuple17 _tmp120=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp121=_tmp120.f2;
inflow=*_tmp121;{
void*_tmp122=s->r;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Tvar*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Decl*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Absyn_Exp*_tmp130;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp123=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp123->tag != 2)goto _LLF2;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LLF1:
# 743
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp124);
s=_tmp125;
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp126=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp126->tag != 12)goto _LLF4;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp127=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(*_tmp126->f1).r;if(_tmp127->tag != 5)goto _LLF4;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_tmp12A=_tmp127->f3;}}_tmp12B=_tmp126->f2;}}_LLF3: {
# 749
union Cyc_CfFlowInfo_FlowInfo _tmp132;union Cyc_CfFlowInfo_AbsLVal _tmp133;struct _tuple15 _tmp131=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp128);_tmp132=_tmp131.f1;_tmp133=_tmp131.f2;{
struct _tuple15 _tmp75B;struct _tuple15 _tmp135=(_tmp75B.f1=_tmp132,((_tmp75B.f2=_tmp133,_tmp75B)));struct Cyc_Dict_Dict _tmp136;struct Cyc_List_List*_tmp137;struct Cyc_CfFlowInfo_Place*_tmp138;_LLFB: if(((_tmp135.f1).ReachableFL).tag != 2)goto _LLFD;_tmp136=((struct _tuple13)((_tmp135.f1).ReachableFL).val).f1;_tmp137=((struct _tuple13)((_tmp135.f1).ReachableFL).val).f2;if(((_tmp135.f2).PlaceL).tag != 1)goto _LLFD;_tmp138=(struct Cyc_CfFlowInfo_Place*)((_tmp135.f2).PlaceL).val;_LLFC: {
# 752
void*_tmp139=Cyc_CfFlowInfo_lookup_place(_tmp136,_tmp138);
# 754
void*t=(void*)_check_null(_tmp128->topt);
{void*_tmp13A=_tmp128->r;struct Cyc_Absyn_Exp*_tmp13C;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp13A;if(_tmp13B->tag != 13)goto _LL104;else{_tmp13C=_tmp13B->f2;}}_LL103:
 t=(void*)_check_null(_tmp13C->topt);goto _LL101;_LL104:;_LL105:
 goto _LL101;_LL101:;}{
# 759
void*_tmp13D=Cyc_CfFlowInfo_make_unique_consumed(_tmp11F,t,_tmp128,env->iteration_num,_tmp139);
_tmp136=Cyc_CfFlowInfo_assign_place(_tmp11F,_tmp128->loc,_tmp136,env->all_changed,_tmp138,_tmp13D);
_tmp132=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp137);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp11F,_tmp136,_tmp139)){case Cyc_CfFlowInfo_AllIL: _LL106:
 leaf=_tmp11F->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL107:
 leaf=_tmp11F->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL108:
 leaf=_tmp11F->unknown_this;break;}{
# 770
void*_tmp13E=Cyc_CfFlowInfo_typ_to_absrval(_tmp11F,t,0,leaf);
_tmp132=Cyc_NewControlFlow_use_Rval(env,_tmp128->loc,_tmp132,_tmp139);{
struct Cyc_List_List _tmp75C;struct Cyc_List_List _tmp13F=(_tmp75C.hd=_tmp12A,((_tmp75C.tl=0,_tmp75C)));
_tmp132=Cyc_NewControlFlow_add_vars(_tmp11F,_tmp132,(struct Cyc_List_List*)& _tmp13F,_tmp11F->unknown_all,s->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp141;void*_tmp142;struct _tuple14 _tmp140=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp132,_tmp12B);_tmp141=_tmp140.f1;_tmp142=_tmp140.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp143=_tmp141;struct Cyc_Dict_Dict _tmp144;struct Cyc_List_List*_tmp145;_LL10B: if((_tmp143.ReachableFL).tag != 2)goto _LL10D;_tmp144=((struct _tuple13)(_tmp143.ReachableFL).val).f1;_tmp145=((struct _tuple13)(_tmp143.ReachableFL).val).f2;_LL10C:
# 777
 _tmp144=Cyc_CfFlowInfo_assign_place(_tmp11F,s->loc,_tmp144,env->all_changed,_tmp138,_tmp13E);{
union Cyc_CfFlowInfo_FlowInfo _tmp146=Cyc_CfFlowInfo_ReachableFL(_tmp144,_tmp145);
struct _tuple14 _tmp75D;return(_tmp75D.f1=_tmp146,((_tmp75D.f2=_tmp142,_tmp75D)));};_LL10D:;_LL10E: {
struct _tuple14 _tmp75E;return(_tmp75E.f1=_tmp141,((_tmp75E.f2=_tmp142,_tmp75E)));}_LL10A:;};};};};};};}_LLFD: if(((_tmp135.f1).BottomFL).tag != 1)goto _LLFF;_LLFE: {
# 782
struct _tuple14 _tmp75F;return(_tmp75F.f1=_tmp132,((_tmp75F.f2=_tmp11F->unknown_all,_tmp75F)));}_LLFF:;_LL100:
{const char*_tmp762;void*_tmp761;(_tmp761=0,Cyc_Tcutil_terr(_tmp128->loc,((_tmp762="bad alias expression--no unique path found",_tag_dyneither(_tmp762,sizeof(char),43))),_tag_dyneither(_tmp761,sizeof(void*),0)));}{
struct _tuple14 _tmp763;return(_tmp763.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp763.f2=_tmp11F->unknown_all,_tmp763)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12C->tag != 12)goto _LLF6;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LLF5:
# 787
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp12D);
s=_tmp12E;
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp12F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12F->tag != 1)goto _LLF8;else{_tmp130=_tmp12F->f1;}}_LLF7:
# 791
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp130);_LLF8:;_LLF9: {
struct Cyc_Core_Impossible_exn_struct _tmp769;const char*_tmp768;struct Cyc_Core_Impossible_exn_struct*_tmp767;(int)_throw((void*)((_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((_tmp769.tag=Cyc_Core_Impossible,((_tmp769.f1=((_tmp768="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp768,sizeof(char),33))),_tmp769)))),_tmp767)))));}_LLEF:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp80B(unsigned int*_tmp80A,unsigned int*_tmp809,void***_tmp807,struct Cyc_List_List**_tmp325){for(*_tmp80A=0;*_tmp80A < *_tmp809;(*_tmp80A)++){
# 1350 "new_control_flow.cyc"
void*_tmp805;(*_tmp807)[*_tmp80A]=((_tmp805=(void*)((struct Cyc_List_List*)_check_null(*_tmp325))->hd,((*_tmp325=(*_tmp325)->tl,_tmp805))));}}
# 799 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 803
struct Cyc_CfFlowInfo_FlowEnv*_tmp151=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 809
{union Cyc_CfFlowInfo_FlowInfo _tmp152=inflow;struct Cyc_Dict_Dict _tmp153;struct Cyc_List_List*_tmp154;_LL110: if((_tmp152.BottomFL).tag != 1)goto _LL112;_LL111: {
struct _tuple14 _tmp76A;return(_tmp76A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp76A.f2=_tmp151->unknown_all,_tmp76A)));}_LL112: if((_tmp152.ReachableFL).tag != 2)goto _LL10F;_tmp153=((struct _tuple13)(_tmp152.ReachableFL).val).f1;_tmp154=((struct _tuple13)(_tmp152.ReachableFL).val).f2;_LL113:
 d=_tmp153;relns=_tmp154;_LL10F:;}
# 818
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 839 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp157;union Cyc_CfFlowInfo_AbsLVal _tmp158;struct _tuple15 _tmp156=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp157=_tmp156.f1;_tmp158=_tmp156.f2;{
struct _tuple15 _tmp76B;struct _tuple15 _tmp15A=(_tmp76B.f1=_tmp157,((_tmp76B.f2=_tmp158,_tmp76B)));struct Cyc_Dict_Dict _tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_CfFlowInfo_Place*_tmp15D;_LL115: if(((_tmp15A.f1).ReachableFL).tag != 2)goto _LL117;_tmp15B=((struct _tuple13)((_tmp15A.f1).ReachableFL).val).f1;_tmp15C=((struct _tuple13)((_tmp15A.f1).ReachableFL).val).f2;if(((_tmp15A.f2).PlaceL).tag != 1)goto _LL117;_tmp15D=(struct Cyc_CfFlowInfo_Place*)((_tmp15A.f2).PlaceL).val;_LL116: {
# 842
void*_tmp15E=Cyc_CfFlowInfo_lookup_place(_tmp15B,_tmp15D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp15E,& needs_unconsume)){
{const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_terr(e->loc,((_tmp76E="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp76E,sizeof(char),71))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}{
struct _tuple14 _tmp76F;return(_tmp76F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp76F.f2=_tmp151->unknown_all,_tmp76F)));};}else{
# 848
if(needs_unconsume){
# 850
struct _tuple14 _tmp770;return(_tmp770.f1=_tmp157,((_tmp770.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp151,_tmp15E),_tmp770)));}else{
# 853
void*_tmp163=Cyc_CfFlowInfo_make_unique_consumed(_tmp151,(void*)_check_null(e->topt),e,env->iteration_num,_tmp15E);
struct Cyc_Dict_Dict _tmp164=Cyc_CfFlowInfo_assign_place(_tmp151,e->loc,_tmp15B,env->all_changed,_tmp15D,_tmp163);
# 864
struct _tuple14 _tmp771;return(_tmp771.f1=Cyc_CfFlowInfo_ReachableFL(_tmp164,_tmp15C),((_tmp771.f2=_tmp15E,_tmp771)));}}}_LL117:;_LL118:
# 866
 goto _LL114;_LL114:;};}{
# 869
void*_tmp166=e->r;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Vardecl*_tmp185;enum Cyc_Absyn_Primop _tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_List_List*_tmp198;int _tmp19A;struct Cyc_Absyn_Exp*_tmp19B;void**_tmp19C;struct Cyc_Absyn_Exp*_tmp19D;int _tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct _dyneither_ptr*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct _dyneither_ptr*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct _dyneither_ptr*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1C1;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C7;enum Cyc_Absyn_AggrKind _tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_Vardecl*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_Stmt*_tmp1D3;_LL11A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp167->tag != 13)goto _LL11C;else{_tmp168=_tmp167->f2;if(_tmp167->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL11C;}}_LL11B: {
# 873
union Cyc_CfFlowInfo_FlowInfo _tmp1DB;void*_tmp1DC;struct _tuple14 _tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp168);_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1DE;void*_tmp1DF;struct _tuple14 _tmp1DD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1DB,_tmp168,_tmp1DC);_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;{
struct _tuple14 _tmp772;return(_tmp772.f1=_tmp1DE,((_tmp772.f2=_tmp1DC,_tmp772)));};};}_LL11C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp169->tag != 13)goto _LL11E;else{_tmp16A=_tmp169->f2;}}_LL11D:
# 879
 _tmp16C=_tmp16A;goto _LL11F;_LL11E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp16B->tag != 11)goto _LL120;else{_tmp16C=_tmp16B->f1;}}_LL11F:
 _tmp16E=_tmp16C;goto _LL121;_LL120: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp16D->tag != 12)goto _LL122;else{_tmp16E=_tmp16D->f1;}}_LL121:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp16E);_LL122: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp16F->tag != 0)goto _LL124;else{if(((_tmp16F->f1).Null_c).tag != 1)goto _LL124;}}_LL123:
# 883
 goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp170->tag != 0)goto _LL126;else{if(((_tmp170->f1).Int_c).tag != 5)goto _LL126;if(((struct _tuple6)((_tmp170->f1).Int_c).val).f2 != 0)goto _LL126;}}_LL125: {
struct _tuple14 _tmp773;return(_tmp773.f1=inflow,((_tmp773.f2=_tmp151->zero,_tmp773)));}_LL126: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp171->tag != 0)goto _LL128;else{if(((_tmp171->f1).Int_c).tag != 5)goto _LL128;}}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp172->tag != 1)goto _LL12A;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp173=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp172->f2);if(_tmp173->tag != 2)goto _LL12A;}}_LL129: {
struct _tuple14 _tmp774;return(_tmp774.f1=inflow,((_tmp774.f2=_tmp151->notzeroall,_tmp774)));}_LL12A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp174->tag != 29)goto _LL12C;else{if(_tmp174->f1 != 0)goto _LL12C;}}_LL12B:
# 888
 goto _LL12D;_LL12C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp175->tag != 0)goto _LL12E;}_LL12D:
 goto _LL12F;_LL12E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp176->tag != 17)goto _LL130;}_LL12F:
 goto _LL131;_LL130: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp177->tag != 16)goto _LL132;}_LL131:
 goto _LL133;_LL132: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp178->tag != 18)goto _LL134;}_LL133:
 goto _LL135;_LL134: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp179->tag != 31)goto _LL136;}_LL135:
 goto _LL137;_LL136: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp17A->tag != 30)goto _LL138;}_LL137: {
struct _tuple14 _tmp775;return(_tmp775.f1=inflow,((_tmp775.f2=_tmp151->unknown_all,_tmp775)));}_LL138: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp17B->tag != 1)goto _LL13A;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp17C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp17B->f2);if(_tmp17C->tag != 1)goto _LL13A;}}_LL139: {
# 897
struct _tuple14 _tmp776;return(_tmp776.f1=inflow,((_tmp776.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp776)));}_LL13A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp17D->tag != 1)goto _LL13C;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp17E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp17D->f2);if(_tmp17E->tag != 3)goto _LL13C;else{_tmp17F=_tmp17E->f1;}}}_LL13B:
# 900
 _tmp182=_tmp17F;goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp180->tag != 1)goto _LL13E;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp181=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp180->f2);if(_tmp181->tag != 4)goto _LL13E;else{_tmp182=_tmp181->f1;}}}_LL13D:
 _tmp185=_tmp182;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp183->tag != 1)goto _LL140;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp184=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp183->f2);if(_tmp184->tag != 5)goto _LL140;else{_tmp185=_tmp184->f1;}}}_LL13F: {
# 904
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp777;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp777.tag=0,((_tmp777.f1=_tmp185,_tmp777)));
struct _tuple14 _tmp778;return(_tmp778.f1=inflow,((_tmp778.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp778)));}_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp186->tag != 2)goto _LL142;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LL141: {
# 911
union Cyc_CfFlowInfo_FlowInfo _tmp1E8;void*_tmp1E9;struct _tuple14 _tmp1E7=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp188);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;{
struct _tuple14 _tmp779;return(_tmp779.f1=_tmp1E8,((_tmp779.f2=_tmp1E9,_tmp779)));};}_LL142: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp189->tag != 4)goto _LL144;else{_tmp18A=_tmp189->f1;}}_LL143: {
# 915
struct Cyc_List_List _tmp77A;struct Cyc_List_List _tmp1EB=(_tmp77A.hd=_tmp18A,((_tmp77A.tl=0,_tmp77A)));
union Cyc_CfFlowInfo_FlowInfo _tmp1ED;struct _tuple14 _tmp1EC=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp1EB);_tmp1ED=_tmp1EC.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1EF;struct _tuple15 _tmp1EE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1ED,0,_tmp18A);_tmp1EF=_tmp1EE.f2;
{struct _tuple21 _tmp77B;struct _tuple21 _tmp1F1=(_tmp77B.f1=_tmp1EF,((_tmp77B.f2=_tmp1ED,_tmp77B)));struct Cyc_CfFlowInfo_Place*_tmp1F2;struct Cyc_Dict_Dict _tmp1F3;struct Cyc_List_List*_tmp1F4;_LL181: if(((_tmp1F1.f1).PlaceL).tag != 1)goto _LL183;_tmp1F2=(struct Cyc_CfFlowInfo_Place*)((_tmp1F1.f1).PlaceL).val;if(((_tmp1F1.f2).ReachableFL).tag != 2)goto _LL183;_tmp1F3=((struct _tuple13)((_tmp1F1.f2).ReachableFL).val).f1;_tmp1F4=((struct _tuple13)((_tmp1F1.f2).ReachableFL).val).f2;_LL182:
# 920
 _tmp1F4=Cyc_CfFlowInfo_reln_kill_exp(_tmp151->r,_tmp1F4,_tmp18A);
_tmp1ED=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp151,_tmp18A->loc,_tmp1F3,env->all_changed,_tmp1F2,_tmp151->unknown_all),_tmp1F4);
# 924
goto _LL180;_LL183:;_LL184:
 goto _LL180;_LL180:;}{
# 927
struct _tuple14 _tmp77C;return(_tmp77C.f1=_tmp1ED,((_tmp77C.f2=_tmp151->unknown_all,_tmp77C)));};};}_LL144: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp18B->tag != 3)goto _LL146;else{_tmp18C=_tmp18B->f1;if(_tmp18B->f2 == 0)goto _LL146;_tmp18D=_tmp18B->f3;}}_LL145:
# 930
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_terr(e->loc,((_tmp77F="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp77F,sizeof(char),58))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}{
struct _tuple14 _tmp780;return(_tmp780.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp780.f2=_tmp151->unknown_all,_tmp780)));};}{
# 934
struct Cyc_List_List _tmp781;struct Cyc_List_List _tmp1FA=(_tmp781.hd=_tmp18D,((_tmp781.tl=0,_tmp781)));
struct Cyc_List_List _tmp782;struct Cyc_List_List _tmp1FB=(_tmp782.hd=_tmp18C,((_tmp782.tl=(struct Cyc_List_List*)& _tmp1FA,_tmp782)));
union Cyc_CfFlowInfo_FlowInfo _tmp1FD;struct _tuple14 _tmp1FC=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp1FB);_tmp1FD=_tmp1FC.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1FF;struct _tuple15 _tmp1FE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1FD,0,_tmp18C);_tmp1FF=_tmp1FE.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp200=_tmp1FD;struct Cyc_Dict_Dict _tmp201;struct Cyc_List_List*_tmp202;_LL186: if((_tmp200.ReachableFL).tag != 2)goto _LL188;_tmp201=((struct _tuple13)(_tmp200.ReachableFL).val).f1;_tmp202=((struct _tuple13)(_tmp200.ReachableFL).val).f2;_LL187:
# 940
{union Cyc_CfFlowInfo_AbsLVal _tmp203=_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp204;_LL18B: if((_tmp203.PlaceL).tag != 1)goto _LL18D;_tmp204=(struct Cyc_CfFlowInfo_Place*)(_tmp203.PlaceL).val;_LL18C:
# 944
 _tmp202=Cyc_CfFlowInfo_reln_kill_exp(_tmp151->r,_tmp202,_tmp18C);
_tmp201=Cyc_CfFlowInfo_assign_place(_tmp151,_tmp18C->loc,_tmp201,env->all_changed,_tmp204,_tmp151->unknown_all);
# 947
_tmp1FD=Cyc_CfFlowInfo_ReachableFL(_tmp201,_tmp202);
# 950
goto _LL18A;_LL18D: if((_tmp203.UnknownL).tag != 2)goto _LL18A;_LL18E:
# 953
 goto _LL18A;_LL18A:;}
# 955
goto _LL185;_LL188:;_LL189:
 goto _LL185;_LL185:;}{
# 958
struct _tuple14 _tmp783;return(_tmp783.f1=_tmp1FD,((_tmp783.f2=_tmp151->unknown_all,_tmp783)));};};};_LL146: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp18E->tag != 3)goto _LL148;else{_tmp18F=_tmp18E->f1;if(_tmp18E->f2 != 0)goto _LL148;_tmp190=_tmp18E->f3;}}_LL147:
# 962
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp786;void*_tmp785;(_tmp785=0,Cyc_Tcutil_terr(e->loc,((_tmp786="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp786,sizeof(char),58))),_tag_dyneither(_tmp785,sizeof(void*),0)));}{
struct _tuple14 _tmp787;return(_tmp787.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp787.f2=_tmp151->unknown_all,_tmp787)));};}{
# 966
struct Cyc_Dict_Dict*_tmp20B=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp788;env->all_changed=((_tmp788=_region_malloc(env->r,sizeof(*_tmp788)),((_tmp788[0]=_tmp151->mt_place_set,_tmp788))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp20E;union Cyc_CfFlowInfo_AbsLVal _tmp20F;struct _tuple15 _tmp20D=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp18F);_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;{
struct Cyc_Dict_Dict _tmp210=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp789;env->all_changed=((_tmp789=_region_malloc(env->r,sizeof(*_tmp789)),((_tmp789[0]=_tmp151->mt_place_set,_tmp789))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp213;void*_tmp214;struct _tuple14 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp190);_tmp213=_tmp212.f1;_tmp214=_tmp212.f2;{
struct Cyc_Dict_Dict _tmp215=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp216=Cyc_CfFlowInfo_after_flow(_tmp151,(struct Cyc_Dict_Dict*)& _tmp210,_tmp20E,_tmp213,_tmp210,_tmp215);
# 977
union Cyc_CfFlowInfo_FlowInfo _tmp217=Cyc_CfFlowInfo_join_flow(_tmp151,_tmp20B,inflow,_tmp216);
# 980
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp217,inflow)){
if(_tmp20B == 0)
env->all_changed=0;else{
# 984
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp20B,
Cyc_CfFlowInfo_union_place_set(_tmp210,_tmp215,0),0);}
# 987
return Cyc_NewControlFlow_do_assign(_tmp151,env,_tmp216,_tmp18F,_tmp20F,_tmp190,_tmp214,e->loc);}
# 990
inflow=_tmp217;};};};};}};_LL148: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp191->tag != 8)goto _LL14A;else{_tmp192=_tmp191->f1;_tmp193=_tmp191->f2;}}_LL149: {
# 995
union Cyc_CfFlowInfo_FlowInfo _tmp219;void*_tmp21A;struct _tuple14 _tmp218=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp192);_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp219,_tmp193);}_LL14A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp194->tag != 10)goto _LL14C;else{_tmp195=_tmp194->f1;}}_LL14B: {
# 999
union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*_tmp21D;struct _tuple14 _tmp21B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp195);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp195->loc,_tmp21C,_tmp21D);{
struct _tuple14 _tmp78A;return(_tmp78A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp78A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp78A)));};}_LL14C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp196->tag != 9)goto _LL14E;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LL14D: {
# 1004
struct _RegionHandle _tmp21F=_new_region("temp");struct _RegionHandle*temp=& _tmp21F;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp222;struct Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp78B;struct _tuple20 _tmp221=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp78B=_region_malloc(temp,sizeof(*_tmp78B)),((_tmp78B->hd=_tmp197,((_tmp78B->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp198),_tmp78B)))))),0,1);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;{
# 1007
union Cyc_CfFlowInfo_FlowInfo _tmp224=Cyc_NewControlFlow_use_Rval(env,_tmp197->loc,_tmp222,(void*)((struct Cyc_List_List*)_check_null(_tmp223))->hd);
_tmp223=_tmp223->tl;{
# 1010
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp225=Cyc_Tcutil_compress((void*)_check_null(_tmp197->topt));void*_tmp227;_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225;if(_tmp226->tag != 5)goto _LL192;else{_tmp227=(_tmp226->f1).elt_typ;}}_LL191:
# 1015
{void*_tmp228=Cyc_Tcutil_compress(_tmp227);struct Cyc_List_List*_tmp22A;_LL195: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228;if(_tmp229->tag != 9)goto _LL197;else{_tmp22A=(_tmp229->f1).attributes;}}_LL196:
# 1017
 for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){
# 1019
void*_tmp22B=(void*)_tmp22A->hd;int _tmp22D;int _tmp22F;int _tmp231;_LL19A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp22C=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp22C->tag != 20)goto _LL19C;else{_tmp22D=_tmp22C->f1;}}_LL19B:
# 1021
{struct Cyc_List_List*_tmp78C;init_params=((_tmp78C=_region_malloc(temp,sizeof(*_tmp78C)),((_tmp78C->hd=(void*)_tmp22D,((_tmp78C->tl=init_params,_tmp78C))))));}goto _LL199;_LL19C: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp22E=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp22E->tag != 21)goto _LL19E;else{_tmp22F=_tmp22E->f1;}}_LL19D:
# 1023
{struct Cyc_List_List*_tmp78D;nolive_unique_params=((_tmp78D=_region_malloc(temp,sizeof(*_tmp78D)),((_tmp78D->hd=(void*)_tmp22F,((_tmp78D->tl=nolive_unique_params,_tmp78D))))));}
goto _LL199;_LL19E: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp230=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp230->tag != 22)goto _LL1A0;else{_tmp231=_tmp230->f1;}}_LL19F:
# 1027
{struct Cyc_List_List*_tmp78E;noconsume_params=((_tmp78E=_region_malloc(temp,sizeof(*_tmp78E)),((_tmp78E->hd=(void*)_tmp231,((_tmp78E->tl=noconsume_params,_tmp78E))))));}
goto _LL199;_LL1A0:;_LL1A1:
 goto _LL199;_LL199:;}
# 1031
goto _LL194;_LL197:;_LL198: {
const char*_tmp791;void*_tmp790;(_tmp790=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp791="anal_Rexp: bad function type",_tag_dyneither(_tmp791,sizeof(char),29))),_tag_dyneither(_tmp790,sizeof(void*),0)));}_LL194:;}
# 1034
goto _LL18F;_LL192:;_LL193: {
const char*_tmp794;void*_tmp793;(_tmp793=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp794="anal_Rexp: bad function type",_tag_dyneither(_tmp794,sizeof(char),29))),_tag_dyneither(_tmp793,sizeof(void*),0)));}_LL18F:;}
# 1037
{int i=1;for(0;_tmp223 != 0;(((_tmp223=_tmp223->tl,_tmp198=((struct Cyc_List_List*)_check_null(_tmp198))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp239=_tmp222;struct Cyc_Dict_Dict _tmp23A;_LL1A3: if((_tmp239.BottomFL).tag != 1)goto _LL1A5;_LL1A4:
 goto _LL1A2;_LL1A5: if((_tmp239.ReachableFL).tag != 2)goto _LL1A2;_tmp23A=((struct _tuple13)(_tmp239.ReachableFL).val).f1;_LL1A6:
# 1042
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp23A,(void*)_tmp223->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp797;void*_tmp796;(_tmp796=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp198))->hd)->loc,((_tmp797="expression may not be initialized",_tag_dyneither(_tmp797,sizeof(char),34))),_tag_dyneither(_tmp796,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp23D=_tmp224;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23F;_LL1A8: if((_tmp23D.BottomFL).tag != 1)goto _LL1AA;_LL1A9:
 goto _LL1A7;_LL1AA: if((_tmp23D.ReachableFL).tag != 2)goto _LL1A7;_tmp23E=((struct _tuple13)(_tmp23D.ReachableFL).val).f1;_tmp23F=((struct _tuple13)(_tmp23D.ReachableFL).val).f2;_LL1AB: {
# 1049
struct Cyc_Dict_Dict _tmp240=Cyc_CfFlowInfo_escape_deref(_tmp151,_tmp23E,env->all_changed,(void*)_tmp223->hd);
{void*_tmp241=(void*)_tmp223->hd;struct Cyc_CfFlowInfo_Place*_tmp243;_LL1AD: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp242=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241;if(_tmp242->tag != 5)goto _LL1AF;else{_tmp243=_tmp242->f1;}}_LL1AE:
# 1052
{void*_tmp244=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp198))->hd)->topt));void*_tmp246;_LL1B2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244;if(_tmp245->tag != 5)goto _LL1B4;else{_tmp246=(_tmp245->f1).elt_typ;}}_LL1B3:
# 1056
 _tmp240=Cyc_CfFlowInfo_assign_place(_tmp151,((struct Cyc_Absyn_Exp*)_tmp198->hd)->loc,_tmp240,env->all_changed,_tmp243,
# 1058
Cyc_CfFlowInfo_typ_to_absrval(_tmp151,_tmp246,0,_tmp151->esc_all));
# 1060
goto _LL1B1;_LL1B4:;_LL1B5: {
const char*_tmp79A;void*_tmp799;(_tmp799=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp79A="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp79A,sizeof(char),39))),_tag_dyneither(_tmp799,sizeof(void*),0)));}_LL1B1:;}
# 1063
goto _LL1AC;_LL1AF:;_LL1B0:
 goto _LL1AC;_LL1AC:;}
# 1066
_tmp224=Cyc_CfFlowInfo_ReachableFL(_tmp240,_tmp23F);
goto _LL1A7;}_LL1A7:;}
# 1069
goto _LL1A2;_LL1A2:;}else{
# 1072
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1075
_tmp224=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp198))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp198))->hd)->topt),_tmp224,(void*)_tmp223->hd);else{
# 1082
_tmp224=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp198))->hd)->loc,_tmp224,(void*)_tmp223->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1085
union Cyc_CfFlowInfo_AbsLVal _tmp24A;struct _tuple15 _tmp249=Cyc_NewControlFlow_anal_Lexp(env,_tmp224,1,(struct Cyc_Absyn_Exp*)_tmp198->hd);_tmp24A=_tmp249.f2;{
struct _tuple15 _tmp79B;struct _tuple15 _tmp24C=(_tmp79B.f1=_tmp224,((_tmp79B.f2=_tmp24A,_tmp79B)));struct Cyc_Dict_Dict _tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_CfFlowInfo_Place*_tmp24F;_LL1B7: if(((_tmp24C.f1).ReachableFL).tag != 2)goto _LL1B9;_tmp24D=((struct _tuple13)((_tmp24C.f1).ReachableFL).val).f1;_tmp24E=((struct _tuple13)((_tmp24C.f1).ReachableFL).val).f2;if(((_tmp24C.f2).PlaceL).tag != 1)goto _LL1B9;_tmp24F=(struct Cyc_CfFlowInfo_Place*)((_tmp24C.f2).PlaceL).val;_LL1B8: {
# 1088
void*_tmp250=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp151,(void*)_tmp223->hd);
# 1091
_tmp24D=Cyc_CfFlowInfo_assign_place(_tmp151,((struct Cyc_Absyn_Exp*)_tmp198->hd)->loc,_tmp24D,env->all_changed,_tmp24F,_tmp250);
_tmp224=Cyc_CfFlowInfo_ReachableFL(_tmp24D,_tmp24E);
# 1096
goto _LL1B6;}_LL1B9: if(((_tmp24C.f1).BottomFL).tag != 1)goto _LL1BB;_LL1BA:
 continue;_LL1BB:;_LL1BC: {
# 1099
const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp198->hd)->loc,((_tmp79E="noconsume() parameters must be l-values",_tag_dyneither(_tmp79E,sizeof(char),40))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}_LL1B6:;};}}}}}
# 1106
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp197->topt))){
struct _tuple14 _tmp79F;struct _tuple14 _tmp254=(_tmp79F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp79F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp79F)));_npop_handler(0);return _tmp254;}else{
# 1109
struct _tuple14 _tmp7A0;struct _tuple14 _tmp256=(_tmp7A0.f1=_tmp224,((_tmp7A0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp7A0)));_npop_handler(0);return _tmp256;}};};}
# 1005
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp199->tag != 32)goto _LL150;else{_tmp19A=(_tmp199->f1).is_calloc;_tmp19B=(_tmp199->f1).rgn;_tmp19C=(_tmp199->f1).elt_type;_tmp19D=(_tmp199->f1).num_elts;_tmp19E=(_tmp199->f1).fat_result;}}_LL14F: {
# 1112
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7A3;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7A2;void*root=(void*)((_tmp7A2=_region_malloc(_tmp151->r,sizeof(*_tmp7A2)),((_tmp7A2[0]=((_tmp7A3.tag=1,((_tmp7A3.f1=_tmp19D,((_tmp7A3.f2=(void*)((void*)_check_null(e->topt)),_tmp7A3)))))),_tmp7A2))));
struct Cyc_CfFlowInfo_Place*_tmp7A4;struct Cyc_CfFlowInfo_Place*place=(_tmp7A4=_region_malloc(_tmp151->r,sizeof(*_tmp7A4)),((_tmp7A4->root=root,((_tmp7A4->fields=0,_tmp7A4)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7A7;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7A6;void*rval=(void*)((_tmp7A6=_region_malloc(_tmp151->r,sizeof(*_tmp7A6)),((_tmp7A6[0]=((_tmp7A7.tag=5,((_tmp7A7.f1=place,_tmp7A7)))),_tmp7A6))));
void*place_val;
if(_tmp19E)place_val=_tmp151->notzeroall;else{
if(_tmp19A)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,*((void**)_check_null(_tmp19C)),0,_tmp151->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,*((void**)_check_null(_tmp19C)),0,_tmp151->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp19B != 0){
struct _RegionHandle _tmp257=_new_region("temp");struct _RegionHandle*temp=& _tmp257;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_Absyn_Exp*_tmp7A8[2];struct _tuple20 _tmp259=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7A8[1]=_tmp19D,((_tmp7A8[0]=(struct Cyc_Absyn_Exp*)_tmp19B,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7A8,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;
outflow=_tmp25A;}
# 1123
;_pop_region(temp);}else{
# 1128
union Cyc_CfFlowInfo_FlowInfo _tmp25D;struct _tuple14 _tmp25C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19D);_tmp25D=_tmp25C.f1;
outflow=_tmp25D;}{
# 1131
union Cyc_CfFlowInfo_FlowInfo _tmp25E=outflow;struct Cyc_Dict_Dict _tmp25F;struct Cyc_List_List*_tmp260;_LL1BE: if((_tmp25E.BottomFL).tag != 1)goto _LL1C0;_LL1BF: {
struct _tuple14 _tmp7A9;return(_tmp7A9.f1=outflow,((_tmp7A9.f2=rval,_tmp7A9)));}_LL1C0: if((_tmp25E.ReachableFL).tag != 2)goto _LL1BD;_tmp25F=((struct _tuple13)(_tmp25E.ReachableFL).val).f1;_tmp260=((struct _tuple13)(_tmp25E.ReachableFL).val).f2;_LL1C1: {
# 1134
struct _tuple14 _tmp7AA;return(_tmp7AA.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25F,root,place_val),_tmp260),((_tmp7AA.f2=rval,_tmp7AA)));}_LL1BD:;};};}_LL150: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp19F->tag != 33)goto _LL152;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL151: {
# 1138
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1144
struct _RegionHandle _tmp268=_new_region("temp");struct _RegionHandle*temp=& _tmp268;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*_tmp7AB[2];struct _tuple20 _tmp26A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7AB[1]=_tmp1A1,((_tmp7AB[0]=_tmp1A0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7AB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C->tl))->hd;
outflow=_tmp26B;}{
# 1152
union Cyc_CfFlowInfo_AbsLVal _tmp26E;struct _tuple15 _tmp26D=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1A0);_tmp26E=_tmp26D.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp270;struct _tuple15 _tmp26F=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1A1);_tmp270=_tmp26F.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp271=outflow;struct Cyc_Dict_Dict _tmp272;struct Cyc_List_List*_tmp273;_LL1C3: if((_tmp271.ReachableFL).tag != 2)goto _LL1C5;_tmp272=((struct _tuple13)(_tmp271.ReachableFL).val).f1;_tmp273=((struct _tuple13)(_tmp271.ReachableFL).val).f2;_LL1C4:
# 1156
{union Cyc_CfFlowInfo_AbsLVal _tmp274=_tmp26E;struct Cyc_CfFlowInfo_Place*_tmp275;_LL1C8: if((_tmp274.PlaceL).tag != 1)goto _LL1CA;_tmp275=(struct Cyc_CfFlowInfo_Place*)(_tmp274.PlaceL).val;_LL1C9:
# 1158
 _tmp272=Cyc_CfFlowInfo_assign_place(_tmp151,_tmp1A0->loc,_tmp272,env->all_changed,_tmp275,right_rval);
# 1160
goto _LL1C7;_LL1CA: if((_tmp274.UnknownL).tag != 2)goto _LL1C7;_LL1CB:
 goto _LL1C7;_LL1C7:;}
# 1163
{union Cyc_CfFlowInfo_AbsLVal _tmp276=_tmp270;struct Cyc_CfFlowInfo_Place*_tmp277;_LL1CD: if((_tmp276.PlaceL).tag != 1)goto _LL1CF;_tmp277=(struct Cyc_CfFlowInfo_Place*)(_tmp276.PlaceL).val;_LL1CE:
# 1165
 _tmp272=Cyc_CfFlowInfo_assign_place(_tmp151,_tmp1A1->loc,_tmp272,env->all_changed,_tmp277,left_rval);
# 1167
goto _LL1CC;_LL1CF: if((_tmp276.UnknownL).tag != 2)goto _LL1CC;_LL1D0:
 goto _LL1CC;_LL1CC:;}
# 1171
_tmp273=Cyc_CfFlowInfo_reln_kill_exp(_tmp151->r,_tmp273,_tmp1A0);
_tmp273=Cyc_CfFlowInfo_reln_kill_exp(_tmp151->r,_tmp273,_tmp1A1);
# 1174
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp272,_tmp273);
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}{
# 1180
struct _tuple14 _tmp7AC;struct _tuple14 _tmp279=(_tmp7AC.f1=outflow,((_tmp7AC.f2=_tmp151->unknown_all,_tmp7AC)));_npop_handler(0);return _tmp279;};};};
# 1144
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1A2->tag != 15)goto _LL154;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LL153: {
# 1183
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7AF;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7AE;void*root=(void*)((_tmp7AE=_region_malloc(_tmp151->r,sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7AF.tag=1,((_tmp7AF.f1=_tmp1A4,((_tmp7AF.f2=(void*)((void*)_check_null(e->topt)),_tmp7AF)))))),_tmp7AE))));
struct Cyc_CfFlowInfo_Place*_tmp7B0;struct Cyc_CfFlowInfo_Place*place=(_tmp7B0=_region_malloc(_tmp151->r,sizeof(*_tmp7B0)),((_tmp7B0->root=root,((_tmp7B0->fields=0,_tmp7B0)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7B3;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7B2;void*rval=(void*)((_tmp7B2=_region_malloc(_tmp151->r,sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B3.tag=5,((_tmp7B3.f1=place,_tmp7B3)))),_tmp7B2))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1A3 != 0){
struct _RegionHandle _tmp27A=_new_region("temp");struct _RegionHandle*temp=& _tmp27A;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_Absyn_Exp*_tmp7B4[2];struct _tuple20 _tmp27C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7B4[1]=_tmp1A4,((_tmp7B4[0]=(struct Cyc_Absyn_Exp*)_tmp1A3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7B4,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;
outflow=_tmp27D;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp27E))->tl))->hd;}
# 1191
;_pop_region(temp);}else{
# 1197
union Cyc_CfFlowInfo_FlowInfo _tmp280;void*_tmp281;struct _tuple14 _tmp27F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A4);_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;
outflow=_tmp280;
place_val=_tmp281;}{
# 1201
union Cyc_CfFlowInfo_FlowInfo _tmp282=outflow;struct Cyc_Dict_Dict _tmp283;struct Cyc_List_List*_tmp284;_LL1D2: if((_tmp282.BottomFL).tag != 1)goto _LL1D4;_LL1D3: {
struct _tuple14 _tmp7B5;return(_tmp7B5.f1=outflow,((_tmp7B5.f2=rval,_tmp7B5)));}_LL1D4: if((_tmp282.ReachableFL).tag != 2)goto _LL1D1;_tmp283=((struct _tuple13)(_tmp282.ReachableFL).val).f1;_tmp284=((struct _tuple13)(_tmp282.ReachableFL).val).f2;_LL1D5: {
# 1204
struct _tuple14 _tmp7B6;return(_tmp7B6.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp283,root,place_val),_tmp284),((_tmp7B6.f2=rval,_tmp7B6)));}_LL1D1:;};};}_LL154: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1A5->tag != 14)goto _LL156;else{_tmp1A6=_tmp1A5->f1;}}_LL155: {
# 1208
union Cyc_CfFlowInfo_FlowInfo _tmp28D;union Cyc_CfFlowInfo_AbsLVal _tmp28E;struct _tuple15 _tmp28C=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1A6);_tmp28D=_tmp28C.f1;_tmp28E=_tmp28C.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp28F=_tmp28E;struct Cyc_CfFlowInfo_Place*_tmp290;_LL1D7: if((_tmp28F.UnknownL).tag != 2)goto _LL1D9;_LL1D8: {
struct _tuple14 _tmp7B7;return(_tmp7B7.f1=_tmp28D,((_tmp7B7.f2=_tmp151->notzeroall,_tmp7B7)));}_LL1D9: if((_tmp28F.PlaceL).tag != 1)goto _LL1D6;_tmp290=(struct Cyc_CfFlowInfo_Place*)(_tmp28F.PlaceL).val;_LL1DA: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7BD;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7BC;struct _tuple14 _tmp7BB;return(_tmp7BB.f1=_tmp28D,((_tmp7BB.f2=(void*)((_tmp7BD=_region_malloc(env->r,sizeof(*_tmp7BD)),((_tmp7BD[0]=((_tmp7BC.tag=5,((_tmp7BC.f1=_tmp290,_tmp7BC)))),_tmp7BD)))),_tmp7BB)));}_LL1D6:;};}_LL156: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1A7->tag != 19)goto _LL158;else{_tmp1A8=_tmp1A7->f1;}}_LL157: {
# 1215
union Cyc_CfFlowInfo_FlowInfo _tmp296;void*_tmp297;struct _tuple14 _tmp295=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A8);_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp296,_tmp1A8,_tmp297);}_LL158: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1A9->tag != 20)goto _LL15A;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LL159: {
# 1220
union Cyc_CfFlowInfo_FlowInfo _tmp299;void*_tmp29A;struct _tuple14 _tmp298=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AA);_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1AA->topt))){
struct _tuple14 _tmp7BE;return(_tmp7BE.f1=_tmp299,((_tmp7BE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp7BE)));}{
void*_tmp29C=_tmp29A;int _tmp29F;int _tmp2A0;struct _dyneither_ptr _tmp2A1;int _tmp2A3;int _tmp2A4;struct _dyneither_ptr _tmp2A5;_LL1DC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp29D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp29C;if(_tmp29D->tag != 8)goto _LL1DE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp29D->f2);if(_tmp29E->tag != 6)goto _LL1DE;else{_tmp29F=(_tmp29E->f1).is_union;_tmp2A0=(_tmp29E->f1).fieldnum;_tmp2A1=_tmp29E->f2;}}}_LL1DD:
# 1225
 _tmp2A3=_tmp29F;_tmp2A4=_tmp2A0;_tmp2A5=_tmp2A1;goto _LL1DF;_LL1DE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp29C;if(_tmp2A2->tag != 6)goto _LL1E0;else{_tmp2A3=(_tmp2A2->f1).is_union;_tmp2A4=(_tmp2A2->f1).fieldnum;_tmp2A5=_tmp2A2->f2;}}_LL1DF: {
# 1227
int _tmp2A6=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1AA->topt),_tmp1AB);
if((_tmp2A3  && _tmp2A4 != - 1) && _tmp2A4 != _tmp2A6){
struct _tuple14 _tmp7BF;return(_tmp7BF.f1=_tmp299,((_tmp7BF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),1,_tmp151->unknown_none),_tmp7BF)));}{
struct _tuple14 _tmp7C0;return(_tmp7C0.f1=_tmp299,((_tmp7C0.f2=*((void**)_check_dyneither_subscript(_tmp2A5,sizeof(void*),_tmp2A6)),_tmp7C0)));};}_LL1E0:;_LL1E1: {
# 1232
struct Cyc_Core_Impossible_exn_struct _tmp7CD;const char*_tmp7CC;void*_tmp7CB[1];struct Cyc_String_pa_PrintArg_struct _tmp7CA;struct Cyc_Core_Impossible_exn_struct*_tmp7C9;(int)_throw((void*)((_tmp7C9=_cycalloc(sizeof(*_tmp7C9)),((_tmp7C9[0]=((_tmp7CD.tag=Cyc_Core_Impossible,((_tmp7CD.f1=(struct _dyneither_ptr)((_tmp7CA.tag=0,((_tmp7CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7CB[0]=& _tmp7CA,Cyc_aprintf(((_tmp7CC="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp7CC,sizeof(char),26))),_tag_dyneither(_tmp7CB,sizeof(void*),1)))))))),_tmp7CD)))),_tmp7C9)))));}_LL1DB:;};}_LL15A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1AC->tag != 36)goto _LL15C;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL15B: {
# 1239
union Cyc_CfFlowInfo_FlowInfo _tmp2AF;void*_tmp2B0;struct _tuple14 _tmp2AE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AD);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1AD->topt))){
struct _tuple14 _tmp7CE;return(_tmp7CE.f1=_tmp2AF,((_tmp7CE.f2=_tmp151->unknown_all,_tmp7CE)));}{
void*_tmp2B2=_tmp2B0;int _tmp2B5;int _tmp2B6;struct _dyneither_ptr _tmp2B7;int _tmp2B9;int _tmp2BA;struct _dyneither_ptr _tmp2BB;_LL1E3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B3->tag != 8)goto _LL1E5;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2B3->f2);if(_tmp2B4->tag != 6)goto _LL1E5;else{_tmp2B5=(_tmp2B4->f1).is_union;_tmp2B6=(_tmp2B4->f1).fieldnum;_tmp2B7=_tmp2B4->f2;}}}_LL1E4:
# 1244
 _tmp2B9=_tmp2B5;_tmp2BA=_tmp2B6;_tmp2BB=_tmp2B7;goto _LL1E6;_LL1E5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B8->tag != 6)goto _LL1E7;else{_tmp2B9=(_tmp2B8->f1).is_union;_tmp2BA=(_tmp2B8->f1).fieldnum;_tmp2BB=_tmp2B8->f2;}}_LL1E6: {
# 1246
int _tmp2BC=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1AD->topt),_tmp1AE);
if(_tmp2B9  && _tmp2BA != - 1){
if(_tmp2BA != _tmp2BC){
struct _tuple14 _tmp7CF;return(_tmp7CF.f1=_tmp2AF,((_tmp7CF.f2=_tmp151->zero,_tmp7CF)));}else{
# 1251
struct _tuple14 _tmp7D0;return(_tmp7D0.f1=_tmp2AF,((_tmp7D0.f2=_tmp151->notzeroall,_tmp7D0)));}}else{
# 1253
struct _tuple14 _tmp7D1;return(_tmp7D1.f1=_tmp2AF,((_tmp7D1.f2=_tmp151->unknown_all,_tmp7D1)));}}_LL1E7:;_LL1E8: {
# 1255
struct Cyc_Core_Impossible_exn_struct _tmp7DE;const char*_tmp7DD;void*_tmp7DC[1];struct Cyc_String_pa_PrintArg_struct _tmp7DB;struct Cyc_Core_Impossible_exn_struct*_tmp7DA;(int)_throw((void*)((_tmp7DA=_cycalloc(sizeof(*_tmp7DA)),((_tmp7DA[0]=((_tmp7DE.tag=Cyc_Core_Impossible,((_tmp7DE.f1=(struct _dyneither_ptr)((_tmp7DB.tag=0,((_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DC[0]=& _tmp7DB,Cyc_aprintf(((_tmp7DD="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp7DD,sizeof(char),26))),_tag_dyneither(_tmp7DC,sizeof(void*),1)))))))),_tmp7DE)))),_tmp7DA)))));}_LL1E2:;};}_LL15C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1AF->tag != 21)goto _LL15E;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL15D: {
# 1260
union Cyc_CfFlowInfo_FlowInfo _tmp2C6;void*_tmp2C7;struct _tuple14 _tmp2C5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1B0);_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;{
# 1263
union Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple14 _tmp2C8=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C6,_tmp1B0,_tmp2C7);_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;{
# 1266
void*_tmp2CB=Cyc_Tcutil_compress((void*)_check_null(_tmp1B0->topt));void*_tmp2CD;_LL1EA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB;if(_tmp2CC->tag != 5)goto _LL1EC;else{_tmp2CD=(_tmp2CC->f1).elt_typ;}}_LL1EB:
# 1268
 if(Cyc_Absyn_is_nontagged_union_type(_tmp2CD)){
struct _tuple14 _tmp7DF;return(_tmp7DF.f1=_tmp2C9,((_tmp7DF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp7DF)));}{
void*_tmp2CF=_tmp2CA;int _tmp2D2;int _tmp2D3;struct _dyneither_ptr _tmp2D4;int _tmp2D6;int _tmp2D7;struct _dyneither_ptr _tmp2D8;_LL1EF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2D0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D0->tag != 8)goto _LL1F1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2D0->f2);if(_tmp2D1->tag != 6)goto _LL1F1;else{_tmp2D2=(_tmp2D1->f1).is_union;_tmp2D3=(_tmp2D1->f1).fieldnum;_tmp2D4=_tmp2D1->f2;}}}_LL1F0:
# 1272
 _tmp2D6=_tmp2D2;_tmp2D7=_tmp2D3;_tmp2D8=_tmp2D4;goto _LL1F2;_LL1F1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D5->tag != 6)goto _LL1F3;else{_tmp2D6=(_tmp2D5->f1).is_union;_tmp2D7=(_tmp2D5->f1).fieldnum;_tmp2D8=_tmp2D5->f2;}}_LL1F2: {
# 1274
int _tmp2D9=Cyc_CfFlowInfo_get_field_index(_tmp2CD,_tmp1B1);
if((_tmp2D6  && _tmp2D7 != - 1) && _tmp2D7 != _tmp2D9){
struct _tuple14 _tmp7E0;return(_tmp7E0.f1=_tmp2C9,((_tmp7E0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),1,_tmp151->unknown_none),_tmp7E0)));}{
struct _tuple14 _tmp7E1;return(_tmp7E1.f1=_tmp2C9,((_tmp7E1.f2=*((void**)_check_dyneither_subscript(_tmp2D8,sizeof(void*),_tmp2D9)),_tmp7E1)));};}_LL1F3:;_LL1F4: {
struct Cyc_Core_Impossible_exn_struct _tmp7E7;const char*_tmp7E6;struct Cyc_Core_Impossible_exn_struct*_tmp7E5;(int)_throw((void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E7.tag=Cyc_Core_Impossible,((_tmp7E7.f1=((_tmp7E6="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7E6,sizeof(char),21))),_tmp7E7)))),_tmp7E5)))));}_LL1EE:;};_LL1EC:;_LL1ED: {
# 1280
struct Cyc_Core_Impossible_exn_struct _tmp7ED;const char*_tmp7EC;struct Cyc_Core_Impossible_exn_struct*_tmp7EB;(int)_throw((void*)((_tmp7EB=_cycalloc(sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7ED.tag=Cyc_Core_Impossible,((_tmp7ED.f1=((_tmp7EC="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7EC,sizeof(char),25))),_tmp7ED)))),_tmp7EB)))));}_LL1E9:;};};}_LL15E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1B2->tag != 5)goto _LL160;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;}}_LL15F: {
# 1284
union Cyc_CfFlowInfo_FlowInfo _tmp2E3;union Cyc_CfFlowInfo_FlowInfo _tmp2E4;struct _tuple16 _tmp2E2=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B3);_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;{
struct _tuple14 _tmp2E5=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E3,_tmp1B4);
struct _tuple14 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E4,_tmp1B5);
# 1288
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp151,env->all_changed,_tmp2E5,_tmp2E6);};}_LL160: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1B6=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1B6->tag != 6)goto _LL162;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LL161: {
# 1291
union Cyc_CfFlowInfo_FlowInfo _tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp2E9;struct _tuple16 _tmp2E7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B7);_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2EB;void*_tmp2EC;struct _tuple14 _tmp2EA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E8,_tmp1B8);_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;{
struct _tuple14 _tmp7EE;struct _tuple14 _tmp2ED=(_tmp7EE.f1=_tmp2EB,((_tmp7EE.f2=_tmp2EC,_tmp7EE)));
struct _tuple14 _tmp7EF;struct _tuple14 _tmp2EE=(_tmp7EF.f1=_tmp2E9,((_tmp7EF.f2=_tmp151->zero,_tmp7EF)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp151,env->all_changed,_tmp2ED,_tmp2EE);};};}_LL162: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1B9=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1B9->tag != 7)goto _LL164;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LL163: {
# 1298
union Cyc_CfFlowInfo_FlowInfo _tmp2F2;union Cyc_CfFlowInfo_FlowInfo _tmp2F3;struct _tuple16 _tmp2F1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1BA);_tmp2F2=_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F5;void*_tmp2F6;struct _tuple14 _tmp2F4=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F3,_tmp1BB);_tmp2F5=_tmp2F4.f1;_tmp2F6=_tmp2F4.f2;{
struct _tuple14 _tmp7F0;struct _tuple14 _tmp2F7=(_tmp7F0.f1=_tmp2F5,((_tmp7F0.f2=_tmp2F6,_tmp7F0)));
struct _tuple14 _tmp7F1;struct _tuple14 _tmp2F8=(_tmp7F1.f1=_tmp2F2,((_tmp7F1.f2=_tmp151->notzeroall,_tmp7F1)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp151,env->all_changed,_tmp2F7,_tmp2F8);};};}_LL164: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1BC->tag != 22)goto _LL166;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}}_LL165: {
# 1305
struct _RegionHandle _tmp2FB=_new_region("temp");struct _RegionHandle*temp=& _tmp2FB;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp2FE;struct Cyc_List_List*_tmp2FF;struct Cyc_Absyn_Exp*_tmp7F2[2];struct _tuple20 _tmp2FD=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7F2[1]=_tmp1BE,((_tmp7F2[0]=_tmp1BD,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7F2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;{
# 1310
union Cyc_CfFlowInfo_FlowInfo _tmp300=_tmp2FE;
{union Cyc_CfFlowInfo_FlowInfo _tmp301=_tmp2FE;struct Cyc_Dict_Dict _tmp302;struct Cyc_List_List*_tmp303;_LL1F6: if((_tmp301.ReachableFL).tag != 2)goto _LL1F8;_tmp302=((struct _tuple13)(_tmp301.ReachableFL).val).f1;_tmp303=((struct _tuple13)(_tmp301.ReachableFL).val).f2;_LL1F7:
# 1315
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp302,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2FF))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7F5;void*_tmp7F4;(_tmp7F4=0,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp7F5="expression may not be initialized",_tag_dyneither(_tmp7F5,sizeof(char),34))),_tag_dyneither(_tmp7F4,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp306=Cyc_NewControlFlow_add_subscript_reln(_tmp151->r,_tmp303,_tmp1BD,_tmp1BE);
if(_tmp303 != _tmp306)
_tmp300=Cyc_CfFlowInfo_ReachableFL(_tmp302,_tmp306);
goto _LL1F5;};_LL1F8:;_LL1F9:
 goto _LL1F5;_LL1F5:;}{
# 1323
void*_tmp307=Cyc_Tcutil_compress((void*)_check_null(_tmp1BD->topt));struct Cyc_List_List*_tmp309;union Cyc_Absyn_Constraint*_tmp30B;_LL1FB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp308=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp307;if(_tmp308->tag != 10)goto _LL1FD;else{_tmp309=_tmp308->f1;}}_LL1FC: {
# 1325
void*_tmp30C=(void*)((struct Cyc_List_List*)_check_null(_tmp2FF))->hd;struct _dyneither_ptr _tmp30F;struct _dyneither_ptr _tmp311;_LL202: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp30D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp30D->tag != 8)goto _LL204;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp30D->f2);if(_tmp30E->tag != 6)goto _LL204;else{_tmp30F=_tmp30E->f2;}}}_LL203:
# 1327
 _tmp311=_tmp30F;goto _LL205;_LL204: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp310=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp310->tag != 6)goto _LL206;else{_tmp311=_tmp310->f2;}}_LL205: {
# 1329
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1BE)).f1;
struct _tuple14 _tmp7F6;struct _tuple14 _tmp313=(_tmp7F6.f1=_tmp300,((_tmp7F6.f2=*((void**)_check_dyneither_subscript(_tmp311,sizeof(void*),(int)i)),_tmp7F6)));_npop_handler(0);return _tmp313;}_LL206:;_LL207: {
struct Cyc_Core_Impossible_exn_struct _tmp7FC;const char*_tmp7FB;struct Cyc_Core_Impossible_exn_struct*_tmp7FA;(int)_throw((void*)((_tmp7FA=_cycalloc(sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FC.tag=Cyc_Core_Impossible,((_tmp7FC.f1=((_tmp7FB="anal_Rexp: Subscript",_tag_dyneither(_tmp7FB,sizeof(char),21))),_tmp7FC)))),_tmp7FA)))));}_LL201:;}_LL1FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307;if(_tmp30A->tag != 5)goto _LL1FF;else{_tmp30B=((_tmp30A->f1).ptr_atts).bounds;}}_LL1FE: {
# 1334
union Cyc_CfFlowInfo_FlowInfo _tmp318;void*_tmp319;struct _tuple14 _tmp317=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2FE,_tmp1BD,(void*)((struct Cyc_List_List*)_check_null(_tmp2FF))->hd);_tmp318=_tmp317.f1;_tmp319=_tmp317.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp31A=_tmp318;_LL209: if((_tmp31A.BottomFL).tag != 1)goto _LL20B;_LL20A: {
struct _tuple14 _tmp7FD;struct _tuple14 _tmp31C=(_tmp7FD.f1=_tmp318,((_tmp7FD.f2=_tmp319,_tmp7FD)));_npop_handler(0);return _tmp31C;}_LL20B:;_LL20C: {
struct _tuple14 _tmp7FE;struct _tuple14 _tmp31E=(_tmp7FE.f1=_tmp300,((_tmp7FE.f2=_tmp319,_tmp7FE)));_npop_handler(0);return _tmp31E;}_LL208:;};}_LL1FF:;_LL200: {
# 1339
struct Cyc_Core_Impossible_exn_struct _tmp804;const char*_tmp803;struct Cyc_Core_Impossible_exn_struct*_tmp802;(int)_throw((void*)((_tmp802=_cycalloc(sizeof(*_tmp802)),((_tmp802[0]=((_tmp804.tag=Cyc_Core_Impossible,((_tmp804.f1=((_tmp803="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp803,sizeof(char),33))),_tmp804)))),_tmp802)))));}_LL1FA:;};};}
# 1306
;_pop_region(temp);}_LL166: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1BF=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1BF->tag != 29)goto _LL168;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;}}_LL167:
# 1344
 _tmp1C3=_tmp1C0;goto _LL169;_LL168: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1C2->tag != 23)goto _LL16A;else{_tmp1C3=_tmp1C2->f1;}}_LL169: {
# 1346
struct _RegionHandle _tmp322=_new_region("temp");struct _RegionHandle*temp=& _tmp322;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp324;struct Cyc_List_List*_tmp325;struct _tuple20 _tmp323=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1C3,1,1);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;{
unsigned int _tmp80A;unsigned int _tmp809;struct _dyneither_ptr _tmp808;void**_tmp807;unsigned int _tmp806;struct _dyneither_ptr aggrdict=
(_tmp806=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C3),((_tmp807=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp806)),((_tmp808=_tag_dyneither(_tmp807,sizeof(void*),_tmp806),((((_tmp809=_tmp806,_tmp80B(& _tmp80A,& _tmp809,& _tmp807,& _tmp325))),_tmp808)))))));
# 1354
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp815;struct Cyc_CfFlowInfo_UnionRInfo _tmp814;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp813;struct _tuple14 _tmp812;struct _tuple14 _tmp32A=(_tmp812.f1=_tmp324,((_tmp812.f2=(void*)((_tmp815=_region_malloc(env->r,sizeof(*_tmp815)),((_tmp815[0]=((_tmp813.tag=6,((_tmp813.f1=((_tmp814.is_union=0,((_tmp814.fieldnum=- 1,_tmp814)))),((_tmp813.f2=aggrdict,_tmp813)))))),_tmp815)))),_tmp812)));_npop_handler(0);return _tmp32A;};}
# 1347
;_pop_region(temp);}_LL16A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1C4->tag != 28)goto _LL16C;else{_tmp1C5=_tmp1C4->f2;}}_LL16B: {
# 1357
struct Cyc_List_List*fs;
{void*_tmp330=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp332;_LL20E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp330;if(_tmp331->tag != 12)goto _LL210;else{_tmp332=_tmp331->f2;}}_LL20F:
 fs=_tmp332;goto _LL20D;_LL210:;_LL211: {
struct Cyc_Core_Impossible_exn_struct _tmp81B;const char*_tmp81A;struct Cyc_Core_Impossible_exn_struct*_tmp819;(int)_throw((void*)((_tmp819=_cycalloc(sizeof(*_tmp819)),((_tmp819[0]=((_tmp81B.tag=Cyc_Core_Impossible,((_tmp81B.f1=((_tmp81A="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp81A,sizeof(char),35))),_tmp81B)))),_tmp819)))));}_LL20D:;}
# 1362
_tmp1C7=_tmp1C5;_tmp1C8=Cyc_Absyn_StructA;_tmp1C9=fs;goto _LL16D;}_LL16C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1C6->tag != 27)goto _LL16E;else{_tmp1C7=_tmp1C6->f3;if(_tmp1C6->f4 == 0)goto _LL16E;_tmp1C8=(*_tmp1C6->f4).kind;if((*_tmp1C6->f4).impl == 0)goto _LL16E;_tmp1C9=(*(*_tmp1C6->f4).impl).fields;}}_LL16D: {
# 1364
struct _RegionHandle _tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp338;struct Cyc_List_List*_tmp339;struct _tuple20 _tmp337=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C7),1,1);_tmp338=_tmp337.f1;_tmp339=_tmp337.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp151,_tmp1C9,_tmp1C8 == Cyc_Absyn_UnionA,_tmp1C8 == Cyc_Absyn_UnionA?_tmp151->unknown_none: _tmp151->unknown_all);
int field_no=- 1;
{int i=0;for(0;_tmp339 != 0;(((_tmp339=_tmp339->tl,_tmp1C7=_tmp1C7->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp1C7))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp33A=(void*)ds->hd;struct _dyneither_ptr*_tmp33D;_LL213: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33A;if(_tmp33B->tag != 0)goto _LL215;}_LL214: {
struct Cyc_Core_Impossible_exn_struct _tmp821;const char*_tmp820;struct Cyc_Core_Impossible_exn_struct*_tmp81F;(int)_throw((void*)((_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F[0]=((_tmp821.tag=Cyc_Core_Impossible,((_tmp821.f1=((_tmp820="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp820,sizeof(char),22))),_tmp821)))),_tmp81F)))));}_LL215: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33A;if(_tmp33C->tag != 1)goto _LL212;else{_tmp33D=_tmp33C->f1;}}_LL216:
# 1376
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1C9,_tmp33D);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp339->hd;
# 1379
if(_tmp1C8 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp341=(*((struct _tuple22*)_tmp1C7->hd)).f2;
_tmp338=Cyc_NewControlFlow_use_Rval(env,_tmp341->loc,_tmp338,(void*)_tmp339->hd);}_LL212:;}}}{
# 1384
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp82B;struct Cyc_CfFlowInfo_UnionRInfo _tmp82A;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp829;struct _tuple14 _tmp828;struct _tuple14 _tmp346=(_tmp828.f1=_tmp338,((_tmp828.f2=(void*)((_tmp82B=_region_malloc(env->r,sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp829.tag=6,((_tmp829.f1=((_tmp82A.is_union=_tmp1C8 == Cyc_Absyn_UnionA,((_tmp82A.fieldnum=field_no,_tmp82A)))),((_tmp829.f2=aggrdict,_tmp829)))))),_tmp82B)))),_tmp828)));_npop_handler(0);return _tmp346;};};}
# 1365
;_pop_region(temp);}_LL16E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1CA->tag != 27)goto _LL170;}_LL16F: {
# 1387
struct Cyc_Core_Impossible_exn_struct _tmp831;const char*_tmp830;struct Cyc_Core_Impossible_exn_struct*_tmp82F;(int)_throw((void*)((_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F[0]=((_tmp831.tag=Cyc_Core_Impossible,((_tmp831.f1=((_tmp830="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp830,sizeof(char),31))),_tmp831)))),_tmp82F)))));}_LL170: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1CB->tag != 25)goto _LL172;else{_tmp1CC=_tmp1CB->f1;}}_LL171: {
# 1389
struct _RegionHandle _tmp34A=_new_region("temp");struct _RegionHandle*temp=& _tmp34A;_push_region(temp);
{struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1CC);
union Cyc_CfFlowInfo_FlowInfo _tmp34D;struct Cyc_List_List*_tmp34E;struct _tuple20 _tmp34C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp34B,1,1);_tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;
for(0;_tmp34E != 0;(_tmp34E=_tmp34E->tl,_tmp34B=_tmp34B->tl)){
_tmp34D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp34B))->hd)->loc,_tmp34D,(void*)_tmp34E->hd);}{
struct _tuple14 _tmp832;struct _tuple14 _tmp350=(_tmp832.f1=_tmp34D,((_tmp832.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp151,(void*)_check_null(e->topt),0,_tmp151->unknown_all),_tmp832)));_npop_handler(0);return _tmp350;};}
# 1390
;_pop_region(temp);}_LL172: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1CD->tag != 26)goto _LL174;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;_tmp1D0=_tmp1CD->f3;_tmp1D1=_tmp1CD->f4;}}_LL173: {
# 1398
union Cyc_CfFlowInfo_FlowInfo _tmp352;void*_tmp353;struct _tuple14 _tmp351=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CF);_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp354=_tmp352;struct Cyc_Dict_Dict _tmp355;struct Cyc_List_List*_tmp356;_LL218: if((_tmp354.BottomFL).tag != 1)goto _LL21A;_LL219: {
struct _tuple14 _tmp833;return(_tmp833.f1=_tmp352,((_tmp833.f2=_tmp151->unknown_all,_tmp833)));}_LL21A: if((_tmp354.ReachableFL).tag != 2)goto _LL217;_tmp355=((struct _tuple13)(_tmp354.ReachableFL).val).f1;_tmp356=((struct _tuple13)(_tmp354.ReachableFL).val).f2;_LL21B:
# 1402
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp355,_tmp353)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp836;void*_tmp835;(_tmp835=0,Cyc_Tcutil_terr(_tmp1CF->loc,((_tmp836="expression may not be initialized",_tag_dyneither(_tmp836,sizeof(char),34))),_tag_dyneither(_tmp835,sizeof(void*),0)));}{
# 1406
struct Cyc_List_List*new_relns=_tmp356;
comp_loop: {
void*_tmp35A=_tmp1CF->r;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Vardecl*_tmp35F;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Vardecl*_tmp368;int _tmp36A;enum Cyc_Absyn_Primop _tmp36C;struct Cyc_Absyn_Exp*_tmp36D;_LL21D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp35B->tag != 13)goto _LL21F;else{_tmp35C=_tmp35B->f2;}}_LL21E:
 _tmp1CF=_tmp35C;goto comp_loop;_LL21F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp35D->tag != 1)goto _LL221;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp35E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp35D->f2);if(_tmp35E->tag != 1)goto _LL221;else{_tmp35F=_tmp35E->f1;}}}if(!(!_tmp35F->escapes))goto _LL221;_LL220:
 _tmp362=_tmp35F;goto _LL222;_LL221:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp360=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp360->tag != 1)goto _LL223;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp361=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp360->f2);if(_tmp361->tag != 4)goto _LL223;else{_tmp362=_tmp361->f1;}}}if(!(!_tmp362->escapes))goto _LL223;_LL222:
 _tmp365=_tmp362;goto _LL224;_LL223:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp363->tag != 1)goto _LL225;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp364=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp363->f2);if(_tmp364->tag != 5)goto _LL225;else{_tmp365=_tmp364->f1;}}}if(!(!_tmp365->escapes))goto _LL225;_LL224:
 _tmp368=_tmp365;goto _LL226;_LL225:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp366->tag != 1)goto _LL227;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp367=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp366->f2);if(_tmp367->tag != 3)goto _LL227;else{_tmp368=_tmp367->f1;}}}if(!(!_tmp368->escapes))goto _LL227;_LL226:
# 1414
{struct Cyc_CfFlowInfo_Reln*_tmp839;struct Cyc_List_List*_tmp838;new_relns=((_tmp838=_region_malloc(env->r,sizeof(*_tmp838)),((_tmp838->hd=((_tmp839=_region_malloc(env->r,sizeof(*_tmp839)),((_tmp839->vd=_tmp1CE,((_tmp839->rop=Cyc_CfFlowInfo_LessVar(_tmp368,_tmp368->type),_tmp839)))))),((_tmp838->tl=_tmp356,_tmp838))))));}
goto _LL21C;_LL227: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp369=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp369->tag != 0)goto _LL229;else{if(((_tmp369->f1).Int_c).tag != 5)goto _LL229;_tmp36A=((struct _tuple6)((_tmp369->f1).Int_c).val).f2;}}_LL228:
# 1417
{struct Cyc_CfFlowInfo_Reln*_tmp83C;struct Cyc_List_List*_tmp83B;new_relns=((_tmp83B=_region_malloc(env->r,sizeof(*_tmp83B)),((_tmp83B->hd=((_tmp83C=_region_malloc(env->r,sizeof(*_tmp83C)),((_tmp83C->vd=_tmp1CE,((_tmp83C->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp36A),_tmp83C)))))),((_tmp83B->tl=_tmp356,_tmp83B))))));}
goto _LL21C;_LL229: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp36B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp36B->tag != 2)goto _LL22B;else{_tmp36C=_tmp36B->f1;if(_tmp36B->f2 == 0)goto _LL22B;_tmp36D=(struct Cyc_Absyn_Exp*)(*_tmp36B->f2).hd;}}_LL22A:
# 1420
{void*_tmp372=_tmp36D->r;struct Cyc_Absyn_Vardecl*_tmp375;struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_Absyn_Vardecl*_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37E;_LL22E:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp373->tag != 1)goto _LL230;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp374=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp373->f2);if(_tmp374->tag != 1)goto _LL230;else{_tmp375=_tmp374->f1;}}}if(!(!_tmp375->escapes))goto _LL230;_LL22F:
 _tmp378=_tmp375;goto _LL231;_LL230:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp376=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp376->tag != 1)goto _LL232;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp377=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp376->f2);if(_tmp377->tag != 4)goto _LL232;else{_tmp378=_tmp377->f1;}}}if(!(!_tmp378->escapes))goto _LL232;_LL231:
 _tmp37B=_tmp378;goto _LL233;_LL232:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp379=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp379->tag != 1)goto _LL234;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp37A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp379->f2);if(_tmp37A->tag != 5)goto _LL234;else{_tmp37B=_tmp37A->f1;}}}if(!(!_tmp37B->escapes))goto _LL234;_LL233:
 _tmp37E=_tmp37B;goto _LL235;_LL234:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp37C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp37C->tag != 1)goto _LL236;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp37C->f2);if(_tmp37D->tag != 3)goto _LL236;else{_tmp37E=_tmp37D->f1;}}}if(!(!_tmp37E->escapes))goto _LL236;_LL235:
# 1426
{struct Cyc_CfFlowInfo_Reln*_tmp83F;struct Cyc_List_List*_tmp83E;new_relns=((_tmp83E=_region_malloc(env->r,sizeof(*_tmp83E)),((_tmp83E->hd=((_tmp83F=_region_malloc(env->r,sizeof(*_tmp83F)),((_tmp83F->vd=_tmp1CE,((_tmp83F->rop=Cyc_CfFlowInfo_LessNumelts(_tmp37E),_tmp83F)))))),((_tmp83E->tl=_tmp356,_tmp83E))))));}
goto _LL22D;_LL236:;_LL237:
 goto _LL22D;_LL22D:;}
# 1430
goto _LL21C;_LL22B:;_LL22C:
 goto _LL21C;_LL21C:;}
# 1433
if(_tmp356 != new_relns)
_tmp352=Cyc_CfFlowInfo_ReachableFL(_tmp355,new_relns);{
# 1437
void*_tmp381=_tmp353;_LL239: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp382=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp382->tag != 0)goto _LL23B;}_LL23A: {
struct _tuple14 _tmp840;return(_tmp840.f1=_tmp352,((_tmp840.f2=_tmp151->unknown_all,_tmp840)));}_LL23B: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp383=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp383->tag != 2)goto _LL23D;}_LL23C:
 goto _LL23E;_LL23D: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp384=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp384->tag != 1)goto _LL23F;}_LL23E:
 goto _LL240;_LL23F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp385=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp385->tag != 5)goto _LL241;}_LL240: {
# 1442
struct Cyc_List_List _tmp841;struct Cyc_List_List _tmp387=(_tmp841.hd=_tmp1CE,((_tmp841.tl=0,_tmp841)));
_tmp352=Cyc_NewControlFlow_add_vars(_tmp151,_tmp352,(struct Cyc_List_List*)& _tmp387,_tmp151->unknown_all,e->loc,0);{
# 1445
union Cyc_CfFlowInfo_FlowInfo _tmp389;void*_tmp38A;struct _tuple14 _tmp388=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp352,_tmp1D0);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp38B=_tmp389;struct Cyc_Dict_Dict _tmp38C;_LL244: if((_tmp38B.BottomFL).tag != 1)goto _LL246;_LL245: {
struct _tuple14 _tmp842;return(_tmp842.f1=_tmp389,((_tmp842.f2=_tmp151->unknown_all,_tmp842)));}_LL246: if((_tmp38B.ReachableFL).tag != 2)goto _LL243;_tmp38C=((struct _tuple13)(_tmp38B.ReachableFL).val).f1;_LL247:
# 1449
 if(Cyc_CfFlowInfo_initlevel(_tmp151,_tmp38C,_tmp38A)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp845;void*_tmp844;(_tmp844=0,Cyc_Tcutil_terr(_tmp1CF->loc,((_tmp845="expression may not be initialized",_tag_dyneither(_tmp845,sizeof(char),34))),_tag_dyneither(_tmp844,sizeof(void*),0)));}{
struct _tuple14 _tmp846;return(_tmp846.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp846.f2=_tmp151->unknown_all,_tmp846)));};}_LL243:;}
# 1454
_tmp352=_tmp389;
goto _LL242;};}_LL241:;_LL242:
# 1457
 while(1){
struct Cyc_List_List _tmp847;struct Cyc_List_List _tmp392=(_tmp847.hd=_tmp1CE,((_tmp847.tl=0,_tmp847)));
_tmp352=Cyc_NewControlFlow_add_vars(_tmp151,_tmp352,(struct Cyc_List_List*)& _tmp392,_tmp151->unknown_all,e->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp394;void*_tmp395;struct _tuple14 _tmp393=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp352,_tmp1D0);_tmp394=_tmp393.f1;_tmp395=_tmp393.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp396=_tmp394;struct Cyc_Dict_Dict _tmp397;_LL249: if((_tmp396.BottomFL).tag != 1)goto _LL24B;_LL24A:
 goto _LL248;_LL24B: if((_tmp396.ReachableFL).tag != 2)goto _LL248;_tmp397=((struct _tuple13)(_tmp396.ReachableFL).val).f1;_LL24C:
# 1464
 if(Cyc_CfFlowInfo_initlevel(_tmp151,_tmp397,_tmp395)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp84A;void*_tmp849;(_tmp849=0,Cyc_Tcutil_terr(_tmp1CF->loc,((_tmp84A="expression may not be initialized",_tag_dyneither(_tmp84A,sizeof(char),34))),_tag_dyneither(_tmp849,sizeof(void*),0)));}{
struct _tuple14 _tmp84B;return(_tmp84B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84B.f2=_tmp151->unknown_all,_tmp84B)));};}_LL248:;}{
# 1469
union Cyc_CfFlowInfo_FlowInfo _tmp39B=Cyc_CfFlowInfo_join_flow(_tmp151,env->all_changed,_tmp352,_tmp394);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp39B,_tmp352))
break;
_tmp352=_tmp39B;};};}{
# 1474
struct _tuple14 _tmp84C;return(_tmp84C.f1=_tmp352,((_tmp84C.f2=_tmp151->unknown_all,_tmp84C)));};_LL238:;};};_LL217:;};}_LL174: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D2->tag != 35)goto _LL176;else{_tmp1D3=_tmp1D2->f1;}}_LL175:
# 1477
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1D3);_LL176: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D4->tag != 1)goto _LL178;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1D5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1D4->f2);if(_tmp1D5->tag != 0)goto _LL178;}}_LL177:
 goto _LL179;_LL178: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D6->tag != 34)goto _LL17A;}_LL179:
 goto _LL17B;_LL17A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D7->tag != 24)goto _LL17C;}_LL17B:
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D8->tag != 37)goto _LL17E;}_LL17D:
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp166;if(_tmp1D9->tag != 38)goto _LL119;}_LL17F: {
# 1483
struct Cyc_Core_Impossible_exn_struct _tmp852;const char*_tmp851;struct Cyc_Core_Impossible_exn_struct*_tmp850;(int)_throw((void*)((_tmp850=_cycalloc(sizeof(*_tmp850)),((_tmp850[0]=((_tmp852.tag=Cyc_Core_Impossible,((_tmp852.f1=((_tmp851="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp851,sizeof(char),31))),_tmp852)))),_tmp850)))));}_LL119:;};}
# 1492
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1499
struct Cyc_CfFlowInfo_FlowEnv*_tmp3A1=env->fenv;
void*_tmp3A2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3A4;union Cyc_Absyn_Constraint*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A6;_LL24E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A2;if(_tmp3A3->tag != 5)goto _LL250;else{_tmp3A4=(_tmp3A3->f1).elt_typ;_tmp3A5=((_tmp3A3->f1).ptr_atts).bounds;_tmp3A6=((_tmp3A3->f1).ptr_atts).zero_term;}}_LL24F: {
# 1502
union Cyc_CfFlowInfo_FlowInfo _tmp3A7=f;struct Cyc_Dict_Dict _tmp3A8;struct Cyc_List_List*_tmp3A9;_LL253: if((_tmp3A7.BottomFL).tag != 1)goto _LL255;_LL254: {
struct _tuple15 _tmp853;return(_tmp853.f1=f,((_tmp853.f2=Cyc_CfFlowInfo_UnknownL(),_tmp853)));}_LL255: if((_tmp3A7.ReachableFL).tag != 2)goto _LL252;_tmp3A8=((struct _tuple13)(_tmp3A7.ReachableFL).val).f1;_tmp3A9=((struct _tuple13)(_tmp3A7.ReachableFL).val).f2;_LL256: {
# 1506
struct _dyneither_ptr locname=_tag_dyneither(0,0,0);
if(Cyc_Tcutil_is_bound_one(_tmp3A5)){
retry: {void*_tmp3AB=r;struct _dyneither_ptr _tmp3AD;void*_tmp3AE;void*_tmp3B2;struct Cyc_List_List*_tmp3B3;enum Cyc_CfFlowInfo_InitLevel _tmp3B6;_LL258: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3AC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3AC->tag != 8)goto _LL25A;else{_tmp3AD=_tmp3AC->f1;_tmp3AE=(void*)_tmp3AC->f2;}}_LL259:
 r=_tmp3AE;locname=_tmp3AD;goto retry;_LL25A: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3AF=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3AF->tag != 1)goto _LL25C;}_LL25B:
 goto _LL25D;_LL25C: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3B0=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3B0->tag != 2)goto _LL25E;}_LL25D:
# 1512
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp856;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp855;e->annot=(void*)((_tmp855=_cycalloc(sizeof(*_tmp855)),((_tmp855[0]=((_tmp856.tag=Cyc_CfFlowInfo_NotZero,((_tmp856.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A9),_tmp856)))),_tmp855))));}goto _LL257;_LL25E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3B1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3B1->tag != 5)goto _LL260;else{_tmp3B2=(*_tmp3B1->f1).root;_tmp3B3=(*_tmp3B1->f1).fields;}}_LL25F:
# 1514
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp859;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp858;e->annot=(void*)((_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858[0]=((_tmp859.tag=Cyc_CfFlowInfo_NotZero,((_tmp859.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A9),_tmp859)))),_tmp858))));}{
struct Cyc_CfFlowInfo_Place*_tmp85C;struct _tuple15 _tmp85B;return(_tmp85B.f1=f,((_tmp85B.f2=Cyc_CfFlowInfo_PlaceL(((_tmp85C=_region_malloc(_tmp3A1->r,sizeof(*_tmp85C)),((_tmp85C->root=_tmp3B2,((_tmp85C->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3A1->r,_tmp3B3,flds),_tmp85C))))))),_tmp85B)));};_LL260: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3B4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3B4->tag != 0)goto _LL262;}_LL261:
# 1517
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp85D;return(_tmp85D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp85D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp85D)));};_LL262: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3B5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3AB;if(_tmp3B5->tag != 3)goto _LL264;else{_tmp3B6=_tmp3B5->f1;}}_LL263:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3B6,locname);goto _LL265;_LL264:;_LL265: {
# 1521
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp860;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp85F;e->annot=(void*)((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp860.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp860.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A9),_tmp860)))),_tmp85F))));}_LL257:;}}else{
# 1524
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp863;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp862;e->annot=(void*)((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862[0]=((_tmp863.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp863.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3A9),_tmp863)))),_tmp862))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3A1,_tmp3A8,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp866;void*_tmp865;(_tmp865=0,Cyc_Tcutil_terr(e->loc,((_tmp866="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp866,sizeof(char),46))),_tag_dyneither(_tmp865,sizeof(void*),0)));}{
struct _tuple15 _tmp867;return(_tmp867.f1=f,((_tmp867.f2=Cyc_CfFlowInfo_UnknownL(),_tmp867)));};}_LL252:;}_LL250:;_LL251: {
# 1529
struct Cyc_Core_Impossible_exn_struct _tmp86D;const char*_tmp86C;struct Cyc_Core_Impossible_exn_struct*_tmp86B;(int)_throw((void*)((_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86D.tag=Cyc_Core_Impossible,((_tmp86D.f1=((_tmp86C="left deref of non-pointer-type",_tag_dyneither(_tmp86C,sizeof(char),31))),_tmp86D)))),_tmp86B)))));}_LL24D:;}
# 1537
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1540
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C8=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C9=inflow;struct Cyc_Dict_Dict _tmp3CA;struct Cyc_List_List*_tmp3CB;_LL267: if((_tmp3C9.BottomFL).tag != 1)goto _LL269;_LL268: {
struct _tuple15 _tmp86E;return(_tmp86E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp86E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp86E)));}_LL269: if((_tmp3C9.ReachableFL).tag != 2)goto _LL266;_tmp3CA=((struct _tuple13)(_tmp3C9.ReachableFL).val).f1;_tmp3CB=((struct _tuple13)(_tmp3C9.ReachableFL).val).f2;_LL26A:
# 1545
 d=_tmp3CA;_LL266:;}{
# 1549
void*_tmp3CD=e->r;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D3;struct Cyc_Absyn_Vardecl*_tmp3D6;struct Cyc_Absyn_Vardecl*_tmp3D9;struct Cyc_Absyn_Vardecl*_tmp3DC;struct Cyc_Absyn_Vardecl*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E1;struct _dyneither_ptr*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E9;struct _dyneither_ptr*_tmp3EA;_LL26C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3CE->tag != 13)goto _LL26E;else{_tmp3CF=_tmp3CE->f2;}}_LL26D:
 _tmp3D1=_tmp3CF;goto _LL26F;_LL26E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D0->tag != 11)goto _LL270;else{_tmp3D1=_tmp3D0->f1;}}_LL26F:
 _tmp3D3=_tmp3D1;goto _LL271;_LL270: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3D2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D2->tag != 12)goto _LL272;else{_tmp3D3=_tmp3D2->f1;}}_LL271:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3D3,flds);_LL272: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D4->tag != 1)goto _LL274;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3D5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3D4->f2);if(_tmp3D5->tag != 3)goto _LL274;else{_tmp3D6=_tmp3D5->f1;}}}_LL273:
# 1554
 _tmp3D9=_tmp3D6;goto _LL275;_LL274: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D7->tag != 1)goto _LL276;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3D8=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3D7->f2);if(_tmp3D8->tag != 4)goto _LL276;else{_tmp3D9=_tmp3D8->f1;}}}_LL275:
 _tmp3DC=_tmp3D9;goto _LL277;_LL276: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3DA->tag != 1)goto _LL278;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3DB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3DA->f2);if(_tmp3DB->tag != 5)goto _LL278;else{_tmp3DC=_tmp3DB->f1;}}}_LL277: {
# 1557
struct Cyc_CfFlowInfo_Place*_tmp878;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp877;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp876;struct _tuple15 _tmp875;return(_tmp875.f1=inflow,((_tmp875.f2=Cyc_CfFlowInfo_PlaceL(((_tmp878=_region_malloc(env->r,sizeof(*_tmp878)),((_tmp878->root=(void*)((_tmp876=_region_malloc(env->r,sizeof(*_tmp876)),((_tmp876[0]=((_tmp877.tag=0,((_tmp877.f1=_tmp3DC,_tmp877)))),_tmp876)))),((_tmp878->fields=flds,_tmp878))))))),_tmp875)));}_LL278: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3DD->tag != 1)goto _LL27A;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3DE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3DD->f2);if(_tmp3DE->tag != 1)goto _LL27A;else{_tmp3DF=_tmp3DE->f1;}}}_LL279: {
# 1559
struct _tuple15 _tmp879;return(_tmp879.f1=inflow,((_tmp879.f2=Cyc_CfFlowInfo_UnknownL(),_tmp879)));}_LL27A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3E0->tag != 21)goto _LL27C;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}}_LL27B:
# 1562
{void*_tmp3F0=Cyc_Tcutil_compress((void*)_check_null(_tmp3E1->topt));void*_tmp3F2;_LL285: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0;if(_tmp3F1->tag != 5)goto _LL287;else{_tmp3F2=(_tmp3F1->f1).elt_typ;}}_LL286:
# 1564
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp3F2)){
struct Cyc_List_List*_tmp87A;flds=((_tmp87A=_region_malloc(env->r,sizeof(*_tmp87A)),((_tmp87A->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3F2,_tmp3E2),((_tmp87A->tl=flds,_tmp87A))))));}
goto _LL284;_LL287:;_LL288: {
struct Cyc_Core_Impossible_exn_struct _tmp880;const char*_tmp87F;struct Cyc_Core_Impossible_exn_struct*_tmp87E;(int)_throw((void*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp880.tag=Cyc_Core_Impossible,((_tmp880.f1=((_tmp87F="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp87F,sizeof(char),25))),_tmp880)))),_tmp87E)))));}_LL284:;}
# 1569
_tmp3E4=_tmp3E1;goto _LL27D;_LL27C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3E3->tag != 19)goto _LL27E;else{_tmp3E4=_tmp3E3->f1;}}_LL27D:
# 1571
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3E4->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp3F8;union Cyc_CfFlowInfo_AbsLVal _tmp3F9;struct _tuple15 _tmp3F7=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3E4);_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;{
struct _tuple15 _tmp881;struct _tuple15 _tmp3FB=(_tmp881.f1=_tmp3F8,((_tmp881.f2=_tmp3F9,_tmp881)));struct Cyc_Dict_Dict _tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_CfFlowInfo_Place*_tmp3FE;_LL28A: if(((_tmp3FB.f1).ReachableFL).tag != 2)goto _LL28C;_tmp3FC=((struct _tuple13)((_tmp3FB.f1).ReachableFL).val).f1;_tmp3FD=((struct _tuple13)((_tmp3FB.f1).ReachableFL).val).f2;if(((_tmp3FB.f2).PlaceL).tag != 1)goto _LL28C;_tmp3FE=(struct Cyc_CfFlowInfo_Place*)((_tmp3FB.f2).PlaceL).val;_LL28B: {
# 1575
void*_tmp3FF=Cyc_CfFlowInfo_lookup_place(_tmp3FC,_tmp3FE);
struct _dyneither_ptr locname=_tag_dyneither(0,0,0);
void*_tmp400=_tmp3FF;struct _dyneither_ptr _tmp403;void*_tmp404;_LL28F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp401=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp401->tag != 5)goto _LL291;}_LL290:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3F8,_tmp3E4,_tmp3F8,_tmp3FF,flds);_LL291: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp402=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp402->tag != 8)goto _LL293;else{_tmp403=_tmp402->f1;_tmp404=(void*)_tmp402->f2;}}_LL292:
 _tmp3FF=_tmp404;locname=_tmp403;goto _LL294;_LL293:;_LL294: {
# 1586
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3C8,_tmp3FC,_tmp3FF);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3C8->unknown_all: _tmp3C8->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3C8,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3E4->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp884;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp883;void*new_root=(void*)((_tmp883=_region_malloc(_tmp3C8->r,sizeof(*_tmp883)),((_tmp883[0]=((_tmp884.tag=1,((_tmp884.f1=e,((_tmp884.f2=(void*)((void*)_check_null(e->topt)),_tmp884)))))),_tmp883))));
struct Cyc_CfFlowInfo_Place*_tmp885;struct Cyc_CfFlowInfo_Place*place=(_tmp885=_region_malloc(_tmp3C8->r,sizeof(*_tmp885)),((_tmp885->root=new_root,((_tmp885->fields=0,_tmp885)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp888;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp887;void*res=(void*)((_tmp887=_region_malloc(_tmp3C8->r,sizeof(*_tmp887)),((_tmp887[0]=((_tmp888.tag=5,((_tmp888.f1=place,_tmp888)))),_tmp887))));
if((char*)locname.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp88B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp88A;res=(void*)((_tmp88A=_region_malloc(_tmp3C8->r,sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=8,((_tmp88B.f1=locname,((_tmp88B.f2=(void*)res,_tmp88B)))))),_tmp88A))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3FC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3FC,new_root,new_rval);
_tmp3FC=Cyc_CfFlowInfo_assign_place(_tmp3C8,e->loc,_tmp3FC,env->all_changed,_tmp3FE,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp407=Cyc_CfFlowInfo_ReachableFL(_tmp3FC,_tmp3FD);
# 1601
return Cyc_NewControlFlow_anal_derefL(env,_tmp407,_tmp3E4,_tmp407,res,flds);};}_LL28E:;}_LL28C:;_LL28D:
# 1603
 goto _LL289;_LL289:;};}{
# 1606
union Cyc_CfFlowInfo_FlowInfo _tmp40E;void*_tmp40F;struct _tuple14 _tmp40D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3E4);_tmp40E=_tmp40D.f1;_tmp40F=_tmp40D.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3E4,_tmp40E,_tmp40F,flds);};_LL27E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3E5->tag != 22)goto _LL280;else{_tmp3E6=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;}}_LL27F: {
# 1610
void*_tmp410=Cyc_Tcutil_compress((void*)_check_null(_tmp3E6->topt));union Cyc_Absyn_Constraint*_tmp413;_LL296: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp411=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp410;if(_tmp411->tag != 10)goto _LL298;}_LL297: {
# 1612
unsigned int _tmp414=(Cyc_Evexp_eval_const_uint_exp(_tmp3E7)).f1;
struct Cyc_List_List*_tmp88C;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E6,((_tmp88C=_region_malloc(env->r,sizeof(*_tmp88C)),((_tmp88C->hd=(void*)((int)_tmp414),((_tmp88C->tl=flds,_tmp88C)))))));}_LL298: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp412=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp410;if(_tmp412->tag != 5)goto _LL29A;else{_tmp413=((_tmp412->f1).ptr_atts).bounds;}}_LL299: {
# 1615
struct _RegionHandle _tmp416=_new_region("temp");struct _RegionHandle*temp=& _tmp416;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp419;struct Cyc_List_List*_tmp41A;struct Cyc_Absyn_Exp*_tmp88D[2];struct _tuple20 _tmp418=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp88D[1]=_tmp3E7,((_tmp88D[0]=_tmp3E6,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp88D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp419=_tmp418.f1;_tmp41A=_tmp418.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp41B=_tmp419;
{union Cyc_CfFlowInfo_FlowInfo _tmp41C=_tmp419;struct Cyc_Dict_Dict _tmp41D;struct Cyc_List_List*_tmp41E;_LL29D: if((_tmp41C.ReachableFL).tag != 2)goto _LL29F;_tmp41D=((struct _tuple13)(_tmp41C.ReachableFL).val).f1;_tmp41E=((struct _tuple13)(_tmp41C.ReachableFL).val).f2;_LL29E:
# 1621
 if(Cyc_CfFlowInfo_initlevel(_tmp3C8,_tmp41D,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp41A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(_tmp3E7->loc,((_tmp890="expression may not be initialized",_tag_dyneither(_tmp890,sizeof(char),34))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp421=Cyc_NewControlFlow_add_subscript_reln(_tmp3C8->r,_tmp41E,_tmp3E6,_tmp3E7);
if(_tmp41E != _tmp421)
_tmp41B=Cyc_CfFlowInfo_ReachableFL(_tmp41D,_tmp421);
goto _LL29C;};_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}{
# 1629
union Cyc_CfFlowInfo_FlowInfo _tmp423;union Cyc_CfFlowInfo_AbsLVal _tmp424;struct _tuple15 _tmp422=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3E6,_tmp419,(void*)((struct Cyc_List_List*)_check_null(_tmp41A))->hd,flds);_tmp423=_tmp422.f1;_tmp424=_tmp422.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp425=_tmp423;_LL2A2: if((_tmp425.BottomFL).tag != 1)goto _LL2A4;_LL2A3: {
struct _tuple15 _tmp891;struct _tuple15 _tmp427=(_tmp891.f1=_tmp423,((_tmp891.f2=_tmp424,_tmp891)));_npop_handler(0);return _tmp427;}_LL2A4:;_LL2A5: {
struct _tuple15 _tmp892;struct _tuple15 _tmp429=(_tmp892.f1=_tmp41B,((_tmp892.f2=_tmp424,_tmp892)));_npop_handler(0);return _tmp429;}_LL2A1:;};};};}
# 1616
;_pop_region(temp);}_LL29A:;_LL29B: {
# 1635
struct Cyc_Core_Impossible_exn_struct _tmp898;const char*_tmp897;struct Cyc_Core_Impossible_exn_struct*_tmp896;(int)_throw((void*)((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp898.tag=Cyc_Core_Impossible,((_tmp898.f1=((_tmp897="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp897,sizeof(char),33))),_tmp898)))),_tmp896)))));}_LL295:;}_LL280: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3E8=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3E8->tag != 20)goto _LL282;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;}}_LL281:
# 1639
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp3E9->topt))){
struct _tuple15 _tmp899;return(_tmp899.f1=inflow,((_tmp899.f2=Cyc_CfFlowInfo_UnknownL(),_tmp899)));}{
struct Cyc_List_List*_tmp89A;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E9,(
(_tmp89A=_region_malloc(env->r,sizeof(*_tmp89A)),((_tmp89A->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp3E9->topt),_tmp3EA),((_tmp89A->tl=flds,_tmp89A)))))));};_LL282:;_LL283: {
# 1644
struct _tuple15 _tmp89B;return(_tmp89B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp89B.f2=Cyc_CfFlowInfo_UnknownL(),_tmp89B)));}_LL26B:;};}
# 1648
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1650
union Cyc_CfFlowInfo_FlowInfo _tmp431;union Cyc_CfFlowInfo_AbsLVal _tmp432;struct _tuple15 _tmp430=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp431=_tmp430.f1;_tmp432=_tmp430.f2;{
struct _tuple15 _tmp89C;return(_tmp89C.f1=_tmp431,((_tmp89C.f2=_tmp432,_tmp89C)));};}
# 1656
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1658
struct Cyc_CfFlowInfo_FlowEnv*_tmp434=env->fenv;
{void*_tmp435=e->r;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*_tmp439;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Exp*_tmp444;enum Cyc_Absyn_Primop _tmp446;struct Cyc_List_List*_tmp447;_LL2A7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp436->tag != 5)goto _LL2A9;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;_tmp439=_tmp436->f3;}}_LL2A8: {
# 1661
union Cyc_CfFlowInfo_FlowInfo _tmp449;union Cyc_CfFlowInfo_FlowInfo _tmp44A;struct _tuple16 _tmp448=Cyc_NewControlFlow_anal_test(env,inflow,_tmp437);_tmp449=_tmp448.f1;_tmp44A=_tmp448.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp44C;union Cyc_CfFlowInfo_FlowInfo _tmp44D;struct _tuple16 _tmp44B=Cyc_NewControlFlow_anal_test(env,_tmp449,_tmp438);_tmp44C=_tmp44B.f1;_tmp44D=_tmp44B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_FlowInfo _tmp450;struct _tuple16 _tmp44E=Cyc_NewControlFlow_anal_test(env,_tmp44A,_tmp439);_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;{
struct _tuple16 _tmp89D;return(_tmp89D.f1=Cyc_CfFlowInfo_join_flow(_tmp434,env->all_changed,_tmp44C,_tmp44F),((_tmp89D.f2=
Cyc_CfFlowInfo_join_flow(_tmp434,env->all_changed,_tmp44D,_tmp450),_tmp89D)));};};};}_LL2A9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp43A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp43A->tag != 6)goto _LL2AB;else{_tmp43B=_tmp43A->f1;_tmp43C=_tmp43A->f2;}}_LL2AA: {
# 1667
union Cyc_CfFlowInfo_FlowInfo _tmp453;union Cyc_CfFlowInfo_FlowInfo _tmp454;struct _tuple16 _tmp452=Cyc_NewControlFlow_anal_test(env,inflow,_tmp43B);_tmp453=_tmp452.f1;_tmp454=_tmp452.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp456;union Cyc_CfFlowInfo_FlowInfo _tmp457;struct _tuple16 _tmp455=Cyc_NewControlFlow_anal_test(env,_tmp453,_tmp43C);_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;{
struct _tuple16 _tmp89E;return(_tmp89E.f1=_tmp456,((_tmp89E.f2=Cyc_CfFlowInfo_join_flow(_tmp434,env->all_changed,_tmp454,_tmp457),_tmp89E)));};};}_LL2AB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp43D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp43D->tag != 7)goto _LL2AD;else{_tmp43E=_tmp43D->f1;_tmp43F=_tmp43D->f2;}}_LL2AC: {
# 1671
union Cyc_CfFlowInfo_FlowInfo _tmp45A;union Cyc_CfFlowInfo_FlowInfo _tmp45B;struct _tuple16 _tmp459=Cyc_NewControlFlow_anal_test(env,inflow,_tmp43E);_tmp45A=_tmp459.f1;_tmp45B=_tmp459.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp45D;union Cyc_CfFlowInfo_FlowInfo _tmp45E;struct _tuple16 _tmp45C=Cyc_NewControlFlow_anal_test(env,_tmp45B,_tmp43F);_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;{
struct _tuple16 _tmp89F;return(_tmp89F.f1=Cyc_CfFlowInfo_join_flow(_tmp434,env->all_changed,_tmp45A,_tmp45D),((_tmp89F.f2=_tmp45E,_tmp89F)));};};}_LL2AD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp440=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp440->tag != 8)goto _LL2AF;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;}}_LL2AE: {
# 1675
union Cyc_CfFlowInfo_FlowInfo _tmp461;void*_tmp462;struct _tuple14 _tmp460=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp441);_tmp461=_tmp460.f1;_tmp462=_tmp460.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp461,_tmp442);}_LL2AF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp443=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp443->tag != 2)goto _LL2B1;else{if(_tmp443->f1 != Cyc_Absyn_Not)goto _LL2B1;if(_tmp443->f2 == 0)goto _LL2B1;_tmp444=(struct Cyc_Absyn_Exp*)(*_tmp443->f2).hd;if((*_tmp443->f2).tl != 0)goto _LL2B1;}}_LL2B0: {
# 1678
union Cyc_CfFlowInfo_FlowInfo _tmp464;union Cyc_CfFlowInfo_FlowInfo _tmp465;struct _tuple16 _tmp463=Cyc_NewControlFlow_anal_test(env,inflow,_tmp444);_tmp464=_tmp463.f1;_tmp465=_tmp463.f2;{
struct _tuple16 _tmp8A0;return(_tmp8A0.f1=_tmp465,((_tmp8A0.f2=_tmp464,_tmp8A0)));};}_LL2B1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp445=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp435;if(_tmp445->tag != 2)goto _LL2B3;else{_tmp446=_tmp445->f1;_tmp447=_tmp445->f2;}}_LL2B2: {
# 1683
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp467=_new_region("temp");struct _RegionHandle*temp=& _tmp467;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp469;struct Cyc_List_List*_tmp46A;struct _tuple20 _tmp468=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp447,0,0);_tmp469=_tmp468.f1;_tmp46A=_tmp468.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp46A))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp46A->tl))->hd;
f=_tmp469;}{
# 1693
union Cyc_CfFlowInfo_FlowInfo _tmp46B=f;struct Cyc_Dict_Dict _tmp46C;struct Cyc_List_List*_tmp46D;_LL2B6: if((_tmp46B.BottomFL).tag != 1)goto _LL2B8;_LL2B7: {
struct _tuple16 _tmp8A1;struct _tuple16 _tmp46F=(_tmp8A1.f1=f,((_tmp8A1.f2=f,_tmp8A1)));_npop_handler(0);return _tmp46F;}_LL2B8: if((_tmp46B.ReachableFL).tag != 2)goto _LL2B5;_tmp46C=((struct _tuple13)(_tmp46B.ReachableFL).val).f1;_tmp46D=((struct _tuple13)(_tmp46B.ReachableFL).val).f2;_LL2B9: {
# 1696
struct Cyc_Absyn_Exp*_tmp470=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447))->hd;
struct Cyc_Absyn_Exp*_tmp471=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp46C,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A4;void*_tmp8A3;(_tmp8A3=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp447->hd)->loc,((_tmp8A4="expression may not be initialized",_tag_dyneither(_tmp8A4,sizeof(char),34))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp46C,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A7;void*_tmp8A6;(_tmp8A6=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp447->tl))->hd)->loc,((_tmp8A7="expression may not be initialized",_tag_dyneither(_tmp8A7,sizeof(char),34))),_tag_dyneither(_tmp8A6,sizeof(void*),0)));}
# 1705
if(_tmp446 == Cyc_Absyn_Eq  || _tmp446 == Cyc_Absyn_Neq){
struct _dyneither_ptr r1n=_tag_dyneither(0,0,0);struct _dyneither_ptr r2n=_tag_dyneither(0,0,0);
{void*_tmp476=r1;struct _dyneither_ptr _tmp478;void*_tmp479;_LL2BB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp477=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp476;if(_tmp477->tag != 8)goto _LL2BD;else{_tmp478=_tmp477->f1;_tmp479=(void*)_tmp477->f2;}}_LL2BC:
 r1=_tmp479;r1n=_tmp478;goto _LL2BA;_LL2BD:;_LL2BE:
 goto _LL2BA;_LL2BA:;}
# 1711
{void*_tmp47A=r2;struct _dyneither_ptr _tmp47C;void*_tmp47D;_LL2C0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp47B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp47A;if(_tmp47B->tag != 8)goto _LL2C2;else{_tmp47C=_tmp47B->f1;_tmp47D=(void*)_tmp47B->f2;}}_LL2C1:
 r2=_tmp47D;r2n=_tmp47C;goto _LL2BF;_LL2C2:;_LL2C3:
 goto _LL2BF;_LL2BF:;}{
# 1716
struct _tuple0 _tmp8A8;struct _tuple0 _tmp47F=(_tmp8A8.f1=r1,((_tmp8A8.f2=r2,_tmp8A8)));enum Cyc_CfFlowInfo_InitLevel _tmp481;enum Cyc_CfFlowInfo_InitLevel _tmp485;_LL2C5:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp480=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp480->tag != 3)goto _LL2C7;else{_tmp481=_tmp480->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp482=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp482->tag != 0)goto _LL2C7;};_LL2C6: {
# 1720
union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple16 _tmp494=Cyc_NewControlFlow_splitzero(env,f,f,_tmp470,_tmp481,r1n);_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;
switch(_tmp446){case Cyc_Absyn_Eq: _LL2D9: {
struct _tuple16 _tmp8A9;struct _tuple16 _tmp498=(_tmp8A9.f1=_tmp496,((_tmp8A9.f2=_tmp495,_tmp8A9)));_npop_handler(0);return _tmp498;}case Cyc_Absyn_Neq: _LL2DA: {
struct _tuple16 _tmp8AA;struct _tuple16 _tmp49A=(_tmp8AA.f1=_tmp495,((_tmp8AA.f2=_tmp496,_tmp8AA)));_npop_handler(0);return _tmp49A;}default: _LL2DB: {
struct Cyc_Core_Impossible_exn_struct _tmp8B0;const char*_tmp8AF;struct Cyc_Core_Impossible_exn_struct*_tmp8AE;(int)_throw((void*)((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8B0.tag=Cyc_Core_Impossible,((_tmp8B0.f1=((_tmp8AF="anal_test, zero-split",_tag_dyneither(_tmp8AF,sizeof(char),22))),_tmp8B0)))),_tmp8AE)))));}}}_LL2C7:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp483=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp483->tag != 0)goto _LL2C9;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp484=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp484->tag != 3)goto _LL2C9;else{_tmp485=_tmp484->f1;}};_LL2C8: {
# 1727
union Cyc_CfFlowInfo_FlowInfo _tmp49F;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;struct _tuple16 _tmp49E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp471,_tmp485,r2n);_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;
switch(_tmp446){case Cyc_Absyn_Eq: _LL2DD: {
struct _tuple16 _tmp8B1;struct _tuple16 _tmp4A2=(_tmp8B1.f1=_tmp4A0,((_tmp8B1.f2=_tmp49F,_tmp8B1)));_npop_handler(0);return _tmp4A2;}case Cyc_Absyn_Neq: _LL2DE: {
struct _tuple16 _tmp8B2;struct _tuple16 _tmp4A4=(_tmp8B2.f1=_tmp49F,((_tmp8B2.f2=_tmp4A0,_tmp8B2)));_npop_handler(0);return _tmp4A4;}default: _LL2DF: {
struct Cyc_Core_Impossible_exn_struct _tmp8B8;const char*_tmp8B7;struct Cyc_Core_Impossible_exn_struct*_tmp8B6;(int)_throw((void*)((_tmp8B6=_cycalloc(sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B8.tag=Cyc_Core_Impossible,((_tmp8B8.f1=((_tmp8B7="anal_test, zero-split",_tag_dyneither(_tmp8B7,sizeof(char),22))),_tmp8B8)))),_tmp8B6)))));}}}_LL2C9:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp486=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp486->tag != 0)goto _LL2CB;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp487=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp487->tag != 0)goto _LL2CB;};_LL2CA:
# 1734
 if(_tmp446 == Cyc_Absyn_Eq){struct _tuple16 _tmp8B9;struct _tuple16 _tmp4A9=(_tmp8B9.f1=f,((_tmp8B9.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8B9)));_npop_handler(0);return _tmp4A9;}else{
struct _tuple16 _tmp8BA;struct _tuple16 _tmp4AB=(_tmp8BA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BA.f2=f,_tmp8BA)));_npop_handler(0);return _tmp4AB;}_LL2CB:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp488=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp488->tag != 0)goto _LL2CD;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp489=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp489->tag != 1)goto _LL2CD;};_LL2CC:
 goto _LL2CE;_LL2CD:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp48A->tag != 0)goto _LL2CF;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp48B=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp48B->tag != 2)goto _LL2CF;};_LL2CE:
 goto _LL2D0;_LL2CF:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp48C->tag != 0)goto _LL2D1;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp48D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp48D->tag != 5)goto _LL2D1;};_LL2D0:
 goto _LL2D2;_LL2D1:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp48E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp48E->tag != 1)goto _LL2D3;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp48F->tag != 0)goto _LL2D3;};_LL2D2:
 goto _LL2D4;_LL2D3:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp490=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp490->tag != 2)goto _LL2D5;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp491=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp491->tag != 0)goto _LL2D5;};_LL2D4:
 goto _LL2D6;_LL2D5:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp492=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp47F.f1;if(_tmp492->tag != 5)goto _LL2D7;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp493=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp47F.f2;if(_tmp493->tag != 0)goto _LL2D7;};_LL2D6:
# 1742
 if(_tmp446 == Cyc_Absyn_Neq){struct _tuple16 _tmp8BB;struct _tuple16 _tmp4AD=(_tmp8BB.f1=f,((_tmp8BB.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8BB)));_npop_handler(0);return _tmp4AD;}else{
struct _tuple16 _tmp8BC;struct _tuple16 _tmp4AF=(_tmp8BC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BC.f2=f,_tmp8BC)));_npop_handler(0);return _tmp4AF;}_LL2D7:;_LL2D8:
 goto _LL2C4;_LL2C4:;};}
# 1752
{struct _tuple0 _tmp8BD;struct _tuple0 _tmp4B1=(_tmp8BD.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp470->topt)),((_tmp8BD.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp471->topt)),_tmp8BD)));_LL2E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4B1.f1;if(_tmp4B2->tag != 6)goto _LL2E4;else{if(_tmp4B2->f1 != Cyc_Absyn_Unsigned)goto _LL2E4;}}_LL2E3:
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4B1.f2;if(_tmp4B3->tag != 6)goto _LL2E6;else{if(_tmp4B3->f1 != Cyc_Absyn_Unsigned)goto _LL2E6;}}_LL2E5:
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B1.f1;if(_tmp4B4->tag != 19)goto _LL2E8;}_LL2E7:
 goto _LL2E9;_LL2E8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B1.f2;if(_tmp4B5->tag != 19)goto _LL2EA;}_LL2E9:
 goto _LL2E1;_LL2EA:;_LL2EB: {
struct _tuple16 _tmp8BE;struct _tuple16 _tmp4B7=(_tmp8BE.f1=f,((_tmp8BE.f2=f,_tmp8BE)));_npop_handler(0);return _tmp4B7;}_LL2E1:;}
# 1760
switch(_tmp446){case Cyc_Absyn_Eq: _LL2EC:
 goto _LL2ED;case Cyc_Absyn_Neq: _LL2ED:
 goto _LL2EE;case Cyc_Absyn_Gt: _LL2EE:
 goto _LL2EF;case Cyc_Absyn_Gte: _LL2EF: {
struct _tuple16 _tmp8BF;struct _tuple16 _tmp4B9=(_tmp8BF.f1=f,((_tmp8BF.f2=f,_tmp8BF)));_npop_handler(0);return _tmp4B9;}case Cyc_Absyn_Lt: _LL2F0: {
# 1766
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4BB=f;
# 1769
{union Cyc_CfFlowInfo_FlowInfo _tmp4BC=_tmp4BA;struct Cyc_Dict_Dict _tmp4BD;_LL2F3: if((_tmp4BC.BottomFL).tag != 1)goto _LL2F5;_LL2F4: {
struct Cyc_Core_Impossible_exn_struct _tmp8C5;const char*_tmp8C4;struct Cyc_Core_Impossible_exn_struct*_tmp8C3;(int)_throw((void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C5.tag=Cyc_Core_Impossible,((_tmp8C5.f1=((_tmp8C4="anal_test, Lt",_tag_dyneither(_tmp8C4,sizeof(char),14))),_tmp8C5)))),_tmp8C3)))));}_LL2F5: if((_tmp4BC.ReachableFL).tag != 2)goto _LL2F2;_tmp4BD=((struct _tuple13)(_tmp4BC.ReachableFL).val).f1;_LL2F6:
 _tmp46C=_tmp4BD;_LL2F2:;}{
# 1775
void*_tmp4C1=_tmp470->r;struct Cyc_Absyn_Vardecl*_tmp4C4;struct Cyc_Absyn_Vardecl*_tmp4C7;struct Cyc_Absyn_Vardecl*_tmp4CA;struct Cyc_Absyn_Vardecl*_tmp4CD;_LL2F8:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C1;if(_tmp4C2->tag != 1)goto _LL2FA;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4C3=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4C2->f2);if(_tmp4C3->tag != 1)goto _LL2FA;else{_tmp4C4=_tmp4C3->f1;}}}if(!(!_tmp4C4->escapes))goto _LL2FA;_LL2F9:
# 1777
 _tmp4C7=_tmp4C4;goto _LL2FB;_LL2FA:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C1;if(_tmp4C5->tag != 1)goto _LL2FC;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4C6=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4C5->f2);if(_tmp4C6->tag != 4)goto _LL2FC;else{_tmp4C7=_tmp4C6->f1;}}}if(!(!_tmp4C7->escapes))goto _LL2FC;_LL2FB:
 _tmp4CA=_tmp4C7;goto _LL2FD;_LL2FC:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C1;if(_tmp4C8->tag != 1)goto _LL2FE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4C9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4C8->f2);if(_tmp4C9->tag != 5)goto _LL2FE;else{_tmp4CA=_tmp4C9->f1;}}}if(!(!_tmp4CA->escapes))goto _LL2FE;_LL2FD:
 _tmp4CD=_tmp4CA;goto _LL2FF;_LL2FE:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C1;if(_tmp4CB->tag != 1)goto _LL300;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4CC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4CB->f2);if(_tmp4CC->tag != 3)goto _LL300;else{_tmp4CD=_tmp4CC->f1;}}}if(!(!_tmp4CD->escapes))goto _LL300;_LL2FF: {
# 1781
void*_tmp4CE=_tmp471->r;struct Cyc_Absyn_Vardecl*_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D4;struct Cyc_Absyn_Vardecl*_tmp4D7;struct Cyc_Absyn_Vardecl*_tmp4DA;int _tmp4DC;int _tmp4DF;enum Cyc_Absyn_Primop _tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;_LL303:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4CF->tag != 1)goto _LL305;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4D0=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4CF->f2);if(_tmp4D0->tag != 1)goto _LL305;else{_tmp4D1=_tmp4D0->f1;}}}if(!(!_tmp4D1->escapes))goto _LL305;_LL304:
 _tmp4D4=_tmp4D1;goto _LL306;_LL305:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4D2->tag != 1)goto _LL307;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D3=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4D2->f2);if(_tmp4D3->tag != 4)goto _LL307;else{_tmp4D4=_tmp4D3->f1;}}}if(!(!_tmp4D4->escapes))goto _LL307;_LL306:
 _tmp4D7=_tmp4D4;goto _LL308;_LL307:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4D5->tag != 1)goto _LL309;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4D6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4D5->f2);if(_tmp4D6->tag != 5)goto _LL309;else{_tmp4D7=_tmp4D6->f1;}}}if(!(!_tmp4D7->escapes))goto _LL309;_LL308:
 _tmp4DA=_tmp4D7;goto _LL30A;_LL309:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4D8->tag != 1)goto _LL30B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4D9=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4D8->f2);if(_tmp4D9->tag != 3)goto _LL30B;else{_tmp4DA=_tmp4D9->f1;}}}if(!(!_tmp4DA->escapes))goto _LL30B;_LL30A: {
# 1787
struct _RegionHandle*_tmp4E3=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8C8;struct Cyc_List_List*_tmp8C7;struct Cyc_List_List*_tmp4E4=(_tmp8C7=_region_malloc(_tmp4E3,sizeof(*_tmp8C7)),((_tmp8C7->hd=((_tmp8C8=_region_malloc(_tmp4E3,sizeof(*_tmp8C8)),((_tmp8C8->vd=_tmp4CD,((_tmp8C8->rop=Cyc_CfFlowInfo_LessVar(_tmp4DA,_tmp4DA->type),_tmp8C8)))))),((_tmp8C7->tl=_tmp46D,_tmp8C7)))));
struct _tuple16 _tmp8C9;struct _tuple16 _tmp4E6=(_tmp8C9.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp4E4),((_tmp8C9.f2=_tmp4BB,_tmp8C9)));_npop_handler(0);return _tmp4E6;}_LL30B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4DB->tag != 0)goto _LL30D;else{if(((_tmp4DB->f1).Int_c).tag != 5)goto _LL30D;_tmp4DC=((struct _tuple6)((_tmp4DB->f1).Int_c).val).f2;}}_LL30C:
 _tmp4DF=_tmp4DC;goto _LL30E;_LL30D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4DD->tag != 13)goto _LL30F;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(*_tmp4DD->f2).r;if(_tmp4DE->tag != 0)goto _LL30F;else{if(((_tmp4DE->f1).Int_c).tag != 5)goto _LL30F;_tmp4DF=((struct _tuple6)((_tmp4DE->f1).Int_c).val).f2;}}}_LL30E: {
# 1792
struct _RegionHandle*_tmp4E9=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8CC;struct Cyc_List_List*_tmp8CB;struct Cyc_List_List*_tmp4EA=(_tmp8CB=_region_malloc(_tmp4E9,sizeof(*_tmp8CB)),((_tmp8CB->hd=((_tmp8CC=_region_malloc(_tmp4E9,sizeof(*_tmp8CC)),((_tmp8CC->vd=_tmp4CD,((_tmp8CC->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp4DF),_tmp8CC)))))),((_tmp8CB->tl=_tmp46D,_tmp8CB)))));
struct _tuple16 _tmp8CD;struct _tuple16 _tmp4EC=(_tmp8CD.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp4EA),((_tmp8CD.f2=_tmp4BB,_tmp8CD)));_npop_handler(0);return _tmp4EC;}_LL30F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4E0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4CE;if(_tmp4E0->tag != 2)goto _LL311;else{_tmp4E1=_tmp4E0->f1;if(_tmp4E0->f2 == 0)goto _LL311;_tmp4E2=(struct Cyc_Absyn_Exp*)(*_tmp4E0->f2).hd;}}_LL310: {
# 1796
void*_tmp4EF=_tmp4E2->r;struct Cyc_Absyn_Vardecl*_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp4F5;struct Cyc_Absyn_Vardecl*_tmp4F8;struct Cyc_Absyn_Vardecl*_tmp4FB;_LL314:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EF;if(_tmp4F0->tag != 1)goto _LL316;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4F1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4F0->f2);if(_tmp4F1->tag != 1)goto _LL316;else{_tmp4F2=_tmp4F1->f1;}}}if(!(!_tmp4F2->escapes))goto _LL316;_LL315:
 _tmp4F5=_tmp4F2;goto _LL317;_LL316:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EF;if(_tmp4F3->tag != 1)goto _LL318;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F4=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4F3->f2);if(_tmp4F4->tag != 4)goto _LL318;else{_tmp4F5=_tmp4F4->f1;}}}if(!(!_tmp4F5->escapes))goto _LL318;_LL317:
 _tmp4F8=_tmp4F5;goto _LL319;_LL318:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EF;if(_tmp4F6->tag != 1)goto _LL31A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4F7=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4F6->f2);if(_tmp4F7->tag != 5)goto _LL31A;else{_tmp4F8=_tmp4F7->f1;}}}if(!(!_tmp4F8->escapes))goto _LL31A;_LL319:
 _tmp4FB=_tmp4F8;goto _LL31B;_LL31A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EF;if(_tmp4F9->tag != 1)goto _LL31C;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4FA=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4F9->f2);if(_tmp4FA->tag != 3)goto _LL31C;else{_tmp4FB=_tmp4FA->f1;}}}if(!(!_tmp4FB->escapes))goto _LL31C;_LL31B: {
# 1802
struct _RegionHandle*_tmp4FC=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8D0;struct Cyc_List_List*_tmp8CF;struct Cyc_List_List*_tmp4FD=(_tmp8CF=_region_malloc(_tmp4FC,sizeof(*_tmp8CF)),((_tmp8CF->hd=((_tmp8D0=_region_malloc(_tmp4FC,sizeof(*_tmp8D0)),((_tmp8D0->vd=_tmp4CD,((_tmp8D0->rop=Cyc_CfFlowInfo_LessNumelts(_tmp4FB),_tmp8D0)))))),((_tmp8CF->tl=_tmp46D,_tmp8CF)))));
struct _tuple16 _tmp8D1;struct _tuple16 _tmp4FF=(_tmp8D1.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp4FD),((_tmp8D1.f2=_tmp4BB,_tmp8D1)));_npop_handler(0);return _tmp4FF;}_LL31C:;_LL31D: {
struct _tuple16 _tmp8D2;struct _tuple16 _tmp503=(_tmp8D2.f1=_tmp4BA,((_tmp8D2.f2=_tmp4BB,_tmp8D2)));_npop_handler(0);return _tmp503;}_LL313:;}_LL311:;_LL312: {
# 1807
struct _tuple16 _tmp8D3;struct _tuple16 _tmp505=(_tmp8D3.f1=_tmp4BA,((_tmp8D3.f2=_tmp4BB,_tmp8D3)));_npop_handler(0);return _tmp505;}_LL302:;}_LL300:;_LL301: {
# 1809
struct _tuple16 _tmp8D4;struct _tuple16 _tmp507=(_tmp8D4.f1=_tmp4BA,((_tmp8D4.f2=_tmp4BB,_tmp8D4)));_npop_handler(0);return _tmp507;}_LL2F7:;};}case Cyc_Absyn_Lte: _LL2F1: {
# 1813
union Cyc_CfFlowInfo_FlowInfo _tmp508=f;
union Cyc_CfFlowInfo_FlowInfo _tmp509=f;
# 1816
{union Cyc_CfFlowInfo_FlowInfo _tmp50A=_tmp508;struct Cyc_Dict_Dict _tmp50B;_LL320: if((_tmp50A.BottomFL).tag != 1)goto _LL322;_LL321: {
struct Cyc_Core_Impossible_exn_struct _tmp8DA;const char*_tmp8D9;struct Cyc_Core_Impossible_exn_struct*_tmp8D8;(int)_throw((void*)((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8DA.tag=Cyc_Core_Impossible,((_tmp8DA.f1=((_tmp8D9="anal_test, Lte",_tag_dyneither(_tmp8D9,sizeof(char),15))),_tmp8DA)))),_tmp8D8)))));}_LL322: if((_tmp50A.ReachableFL).tag != 2)goto _LL31F;_tmp50B=((struct _tuple13)(_tmp50A.ReachableFL).val).f1;_LL323:
 _tmp46C=_tmp50B;_LL31F:;}{
# 1822
void*_tmp50F=_tmp470->r;struct Cyc_Absyn_Vardecl*_tmp512;struct Cyc_Absyn_Vardecl*_tmp515;struct Cyc_Absyn_Vardecl*_tmp518;struct Cyc_Absyn_Vardecl*_tmp51B;_LL325:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp510->tag != 1)goto _LL327;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp511=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp510->f2);if(_tmp511->tag != 1)goto _LL327;else{_tmp512=_tmp511->f1;}}}if(!(!_tmp512->escapes))goto _LL327;_LL326:
# 1824
 _tmp515=_tmp512;goto _LL328;_LL327:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp513=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp513->tag != 1)goto _LL329;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp514=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp513->f2);if(_tmp514->tag != 4)goto _LL329;else{_tmp515=_tmp514->f1;}}}if(!(!_tmp515->escapes))goto _LL329;_LL328:
 _tmp518=_tmp515;goto _LL32A;_LL329:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp516=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp516->tag != 1)goto _LL32B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp517=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp516->f2);if(_tmp517->tag != 5)goto _LL32B;else{_tmp518=_tmp517->f1;}}}if(!(!_tmp518->escapes))goto _LL32B;_LL32A:
 _tmp51B=_tmp518;goto _LL32C;_LL32B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp519->tag != 1)goto _LL32D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp51A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp519->f2);if(_tmp51A->tag != 3)goto _LL32D;else{_tmp51B=_tmp51A->f1;}}}if(!(!_tmp51B->escapes))goto _LL32D;_LL32C: {
# 1828
void*_tmp51C=_tmp471->r;int _tmp51E;int _tmp521;enum Cyc_Absyn_Primop _tmp523;struct Cyc_Absyn_Exp*_tmp524;_LL330: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51C;if(_tmp51D->tag != 0)goto _LL332;else{if(((_tmp51D->f1).Int_c).tag != 5)goto _LL332;_tmp51E=((struct _tuple6)((_tmp51D->f1).Int_c).val).f2;}}_LL331:
 _tmp521=_tmp51E;goto _LL333;_LL332: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp51C;if(_tmp51F->tag != 13)goto _LL334;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(*_tmp51F->f2).r;if(_tmp520->tag != 0)goto _LL334;else{if(((_tmp520->f1).Int_c).tag != 5)goto _LL334;_tmp521=((struct _tuple6)((_tmp520->f1).Int_c).val).f2;}}}_LL333: {
# 1831
struct _RegionHandle*_tmp525=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8DD;struct Cyc_List_List*_tmp8DC;struct Cyc_List_List*_tmp526=(_tmp8DC=_region_malloc(_tmp525,sizeof(*_tmp8DC)),((_tmp8DC->hd=(
(_tmp8DD=_region_malloc(_tmp525,sizeof(*_tmp8DD)),((_tmp8DD->vd=_tmp51B,((_tmp8DD->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)_tmp521),_tmp8DD)))))),((_tmp8DC->tl=_tmp46D,_tmp8DC)))));
struct _tuple16 _tmp8DE;struct _tuple16 _tmp528=(_tmp8DE.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp526),((_tmp8DE.f2=_tmp509,_tmp8DE)));_npop_handler(0);return _tmp528;}_LL334: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp51C;if(_tmp522->tag != 2)goto _LL336;else{_tmp523=_tmp522->f1;if(_tmp522->f2 == 0)goto _LL336;_tmp524=(struct Cyc_Absyn_Exp*)(*_tmp522->f2).hd;}}_LL335: {
# 1836
void*_tmp52B=_tmp524->r;struct Cyc_Absyn_Vardecl*_tmp52E;struct Cyc_Absyn_Vardecl*_tmp531;struct Cyc_Absyn_Vardecl*_tmp534;struct Cyc_Absyn_Vardecl*_tmp537;_LL339:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52B;if(_tmp52C->tag != 1)goto _LL33B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp52D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp52C->f2);if(_tmp52D->tag != 1)goto _LL33B;else{_tmp52E=_tmp52D->f1;}}}if(!(!_tmp52E->escapes))goto _LL33B;_LL33A:
 _tmp531=_tmp52E;goto _LL33C;_LL33B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52B;if(_tmp52F->tag != 1)goto _LL33D;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp530=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp52F->f2);if(_tmp530->tag != 4)goto _LL33D;else{_tmp531=_tmp530->f1;}}}if(!(!_tmp531->escapes))goto _LL33D;_LL33C:
 _tmp534=_tmp531;goto _LL33E;_LL33D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52B;if(_tmp532->tag != 1)goto _LL33F;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp533=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp532->f2);if(_tmp533->tag != 5)goto _LL33F;else{_tmp534=_tmp533->f1;}}}if(!(!_tmp534->escapes))goto _LL33F;_LL33E:
 _tmp537=_tmp534;goto _LL340;_LL33F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52B;if(_tmp535->tag != 1)goto _LL341;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp536=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp535->f2);if(_tmp536->tag != 3)goto _LL341;else{_tmp537=_tmp536->f1;}}}if(!(!_tmp537->escapes))goto _LL341;_LL340: {
# 1842
struct Cyc_CfFlowInfo_FlowEnv*_tmp538=env->fenv;
struct Cyc_CfFlowInfo_Reln*_tmp8E1;struct Cyc_List_List*_tmp8E0;struct Cyc_List_List*_tmp539=(_tmp8E0=_region_malloc(_tmp538->r,sizeof(*_tmp8E0)),((_tmp8E0->hd=((_tmp8E1=_region_malloc(_tmp538->r,sizeof(*_tmp8E1)),((_tmp8E1->vd=_tmp51B,((_tmp8E1->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp537),_tmp8E1)))))),((_tmp8E0->tl=_tmp46D,_tmp8E0)))));
struct _tuple16 _tmp8E2;struct _tuple16 _tmp53B=(_tmp8E2.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp539),((_tmp8E2.f2=_tmp509,_tmp8E2)));_npop_handler(0);return _tmp53B;}_LL341:;_LL342: {
struct _tuple16 _tmp8E3;struct _tuple16 _tmp53F=(_tmp8E3.f1=_tmp508,((_tmp8E3.f2=_tmp509,_tmp8E3)));_npop_handler(0);return _tmp53F;}_LL338:;}_LL336:;_LL337: {
# 1847
struct _tuple16 _tmp8E4;struct _tuple16 _tmp541=(_tmp8E4.f1=_tmp508,((_tmp8E4.f2=_tmp509,_tmp8E4)));_npop_handler(0);return _tmp541;}_LL32F:;}_LL32D:;_LL32E: {
# 1849
struct _tuple16 _tmp8E5;struct _tuple16 _tmp543=(_tmp8E5.f1=_tmp508,((_tmp8E5.f2=_tmp509,_tmp8E5)));_npop_handler(0);return _tmp543;}_LL324:;};}default: _LL31E: {
# 1851
struct _tuple16 _tmp8E6;struct _tuple16 _tmp545=(_tmp8E6.f1=f,((_tmp8E6.f2=f,_tmp8E6)));_npop_handler(0);return _tmp545;}}}_LL2B5:;};
# 1685
;_pop_region(temp);}_LL2B3:;_LL2B4:
# 1854
 goto _LL2A6;_LL2A6:;}{
# 1858
union Cyc_CfFlowInfo_FlowInfo _tmp547;void*_tmp548;struct _tuple14 _tmp546=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp547=_tmp546.f1;_tmp548=_tmp546.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp549=_tmp547;struct Cyc_Dict_Dict _tmp54A;_LL345: if((_tmp549.BottomFL).tag != 1)goto _LL347;_LL346: {
struct _tuple16 _tmp8E7;return(_tmp8E7.f1=_tmp547,((_tmp8E7.f2=_tmp547,_tmp8E7)));}_LL347: if((_tmp549.ReachableFL).tag != 2)goto _LL344;_tmp54A=((struct _tuple13)(_tmp549.ReachableFL).val).f1;_LL348: {
# 1862
struct _dyneither_ptr locname=_tag_dyneither(0,0,0);
retry: {void*_tmp54C=_tmp548;struct _dyneither_ptr _tmp54E;void*_tmp54F;enum Cyc_CfFlowInfo_InitLevel _tmp558;_LL34A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp54D->tag != 8)goto _LL34C;else{_tmp54E=_tmp54D->f1;_tmp54F=(void*)_tmp54D->f2;}}_LL34B:
 _tmp548=_tmp54F;locname=_tmp54E;goto retry;_LL34C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp550=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp550->tag != 0)goto _LL34E;}_LL34D: {
struct _tuple16 _tmp8E8;return(_tmp8E8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E8.f2=_tmp547,_tmp8E8)));}_LL34E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp551=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp551->tag != 2)goto _LL350;}_LL34F:
 goto _LL351;_LL350: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp552=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp552->tag != 1)goto _LL352;}_LL351:
 goto _LL353;_LL352: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp553=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp553->tag != 5)goto _LL354;}_LL353: {
struct _tuple16 _tmp8E9;return(_tmp8E9.f1=_tmp547,((_tmp8E9.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8E9)));}_LL354: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp554=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp554->tag != 3)goto _LL356;else{if(_tmp554->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL356;}}_LL355:
 goto _LL357;_LL356: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp555=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp555->tag != 4)goto _LL358;else{if(_tmp555->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL358;}}_LL357:
 goto _LL359;_LL358: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp556=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp556->tag != 7)goto _LL35A;}_LL359:
# 1872
{const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_terr(e->loc,((_tmp8EC="expression may not be initialized",_tag_dyneither(_tmp8EC,sizeof(char),34))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}{
struct _tuple16 _tmp8ED;return(_tmp8ED.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8ED.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8ED)));};_LL35A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp557=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp557->tag != 3)goto _LL35C;else{_tmp558=_tmp557->f1;}}_LL35B:
# 1875
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp547,e,_tmp558,locname);_LL35C: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp559=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp559->tag != 4)goto _LL35E;}_LL35D: {
struct _tuple16 _tmp8EE;return(_tmp8EE.f1=_tmp547,((_tmp8EE.f2=_tmp547,_tmp8EE)));}_LL35E: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp55A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54C;if(_tmp55A->tag != 6)goto _LL349;}_LL35F: {
struct Cyc_Core_Impossible_exn_struct _tmp8F4;const char*_tmp8F3;struct Cyc_Core_Impossible_exn_struct*_tmp8F2;(int)_throw((void*)((_tmp8F2=_cycalloc(sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F4.tag=Cyc_Core_Impossible,((_tmp8F4.f1=((_tmp8F3="anal_test",_tag_dyneither(_tmp8F3,sizeof(char),10))),_tmp8F4)))),_tmp8F2)))));}_LL349:;}}_LL344:;};};}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1882
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1884
unsigned int _tmp565;struct Cyc_NewControlFlow_AnalEnv*_tmp566;struct Cyc_Dict_Dict _tmp567;struct _tuple23*_tmp564=ckenv;_tmp565=(*_tmp564).f1;_tmp566=(*_tmp564).f2;_tmp567=(*_tmp564).f3;{
void*_tmp568=root;struct Cyc_Absyn_Vardecl*_tmp56A;_LL361: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp569=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp568;if(_tmp569->tag != 0)goto _LL363;else{_tmp56A=_tmp569->f1;}}_LL362:
# 1887
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp566->fenv)->r,_tmp56A->type)){
void*_tmp56B=rval;_LL366: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp56C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp56C->tag != 8)goto _LL368;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp56D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp56C->f2);if(_tmp56D->tag != 7)goto _LL368;}}_LL367:
 goto _LL369;_LL368: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp56E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp56E->tag != 8)goto _LL36A;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp56F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp56E->f2);if(_tmp56F->tag != 0)goto _LL36A;}}_LL369:
 goto _LL36B;_LL36A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp570=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp570->tag != 8)goto _LL36C;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp571=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp570->f2);if(_tmp571->tag != 3)goto _LL36C;else{if(_tmp571->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL36C;}}}_LL36B:
 goto _LL36D;_LL36C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp572=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp572->tag != 7)goto _LL36E;}_LL36D:
 goto _LL36F;_LL36E: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp573=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp573->tag != 0)goto _LL370;}_LL36F:
 goto _LL371;_LL370: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp574=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp56B;if(_tmp574->tag != 3)goto _LL372;else{if(_tmp574->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL372;}}_LL371:
 goto _LL365;_LL372:;_LL373:
# 1896
{const char*_tmp8F8;void*_tmp8F7[1];struct Cyc_String_pa_PrintArg_struct _tmp8F6;(_tmp8F6.tag=0,((_tmp8F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp56A->name)),((_tmp8F7[0]=& _tmp8F6,Cyc_Tcutil_warn(_tmp565,((_tmp8F8="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp8F8,sizeof(char),57))),_tag_dyneither(_tmp8F7,sizeof(void*),1)))))));}
goto _LL365;_LL365:;}
# 1899
goto _LL360;_LL363:;_LL364:
 goto _LL360;_LL360:;};}
# 1904
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp578=flow;struct Cyc_Dict_Dict _tmp579;_LL375: if((_tmp578.BottomFL).tag != 1)goto _LL377;_LL376:
 return;_LL377: if((_tmp578.ReachableFL).tag != 2)goto _LL374;_tmp579=((struct _tuple13)(_tmp578.ReachableFL).val).f1;_LL378:
# 1908
{struct Cyc_List_List*_tmp57A=env->param_roots;for(0;_tmp57A != 0;_tmp57A=_tmp57A->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp579,Cyc_CfFlowInfo_lookup_place(_tmp579,(struct Cyc_CfFlowInfo_Place*)_tmp57A->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,Cyc_Tcutil_terr(loc,((_tmp8FB="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp8FB,sizeof(char),76))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}}}
# 1912
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp8FC;struct _tuple23 _tmp57D=(_tmp8FC.f1=loc,((_tmp8FC.f2=env,((_tmp8FC.f3=_tmp579,_tmp8FC)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp57D,_tmp579);}
# 1916
return;_LL374:;}struct _tuple24{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple25{void*f1;void*f2;void*f3;};
# 1920
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1922
union Cyc_CfFlowInfo_FlowInfo _tmp57F=flow;struct Cyc_Dict_Dict _tmp580;_LL37A: if((_tmp57F.BottomFL).tag != 1)goto _LL37C;_LL37B:
 return;_LL37C: if((_tmp57F.ReachableFL).tag != 2)goto _LL379;_tmp580=((struct _tuple13)(_tmp57F.ReachableFL).val).f1;_LL37D: {
# 1925
struct Cyc_Absyn_Exp*_tmp581=Cyc_Absyn_exp_unsigned_one;
int _tmp582=0;
int _tmp583=1;
{struct Cyc_List_List*_tmp584=env->noconsume_params;for(0;_tmp584 != 0;_tmp584=_tmp584->tl){
# 1931
struct Cyc_CfFlowInfo_Place*_tmp586;void*_tmp587;struct _tuple24*_tmp585=(struct _tuple24*)_tmp584->hd;_tmp586=(*_tmp585).f1;_tmp587=(*_tmp585).f2;{
void*_tmp588=Cyc_CfFlowInfo_lookup_place(_tmp580,_tmp586);
void*_tmp58A;struct Cyc_CfFlowInfo_Place*_tmp589=_tmp586;_tmp58A=(*_tmp589).root;{
struct _tuple25 _tmp8FD;struct _tuple25 _tmp58C=(_tmp8FD.f1=_tmp58A,((_tmp8FD.f2=_tmp587,((_tmp8FD.f3=_tmp588,_tmp8FD)))));struct Cyc_Absyn_Vardecl*_tmp58E;struct _dyneither_ptr _tmp590;void*_tmp591;struct _dyneither_ptr _tmp593;void*_tmp594;struct Cyc_Absyn_Vardecl*_tmp596;_LL37F:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp58D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp58C.f1;if(_tmp58D->tag != 0)goto _LL381;else{_tmp58E=_tmp58D->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp58F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp58C.f2;if(_tmp58F->tag != 8)goto _LL381;else{_tmp590=_tmp58F->f1;_tmp591=(void*)_tmp58F->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp592=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp58C.f3;if(_tmp592->tag != 8)goto _LL381;else{_tmp593=_tmp592->f1;_tmp594=(void*)_tmp592->f2;}};_LL380:
# 1936
 if((char*)_tmp590.curr == (char*)_tmp593.curr){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp581,_tmp583,_tmp594,& _tmp582)){
const char*_tmp901;void*_tmp900[1];struct Cyc_String_pa_PrintArg_struct _tmp8FF;(_tmp8FF.tag=0,((_tmp8FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1940
Cyc_Absynpp_qvar2string(_tmp58E->name)),((_tmp900[0]=& _tmp8FF,Cyc_Tcutil_terr(loc,((_tmp901="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp901,sizeof(char),58))),_tag_dyneither(_tmp900,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp580,_tmp594)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp905;void*_tmp904[1];struct Cyc_String_pa_PrintArg_struct _tmp903;(_tmp903.tag=0,((_tmp903.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1944
Cyc_Absynpp_qvar2string(_tmp58E->name)),((_tmp904[0]=& _tmp903,Cyc_Tcutil_terr(loc,((_tmp905="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp905,sizeof(char),84))),_tag_dyneither(_tmp904,sizeof(void*),1)))))));}}}else{
# 1948
_tmp596=_tmp58E;goto _LL382;}
goto _LL37E;_LL381: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp595=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp58C.f1;if(_tmp595->tag != 0)goto _LL383;else{_tmp596=_tmp595->f1;}}_LL382:
# 1951
{const char*_tmp909;void*_tmp908[1];struct Cyc_String_pa_PrintArg_struct _tmp907;(_tmp907.tag=0,((_tmp907.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1953
Cyc_Absynpp_qvar2string(_tmp596->name)),((_tmp908[0]=& _tmp907,Cyc_Tcutil_terr(loc,((_tmp909="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp909,sizeof(char),85))),_tag_dyneither(_tmp908,sizeof(void*),1)))))));}
goto _LL37E;_LL383:;_LL384: {
# 1956
const char*_tmp90C;void*_tmp90B;(_tmp90B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp90C="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp90C,sizeof(char),46))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}_LL37E:;};};}}
# 1959
return;}_LL379:;}
# 1966
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1968
struct Cyc_CfFlowInfo_FlowEnv*_tmp5A2=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Stmt*_tmp5A6;unsigned int _tmp5A7;struct Cyc_Absyn_Switch_clause*_tmp5A3=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp5A4=(*_tmp5A3).pat_vars;_tmp5A5=(*_tmp5A3).where_clause;_tmp5A6=(*_tmp5A3).body;_tmp5A7=(*_tmp5A3).loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5A2,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A4))->v,_tmp5A2->unknown_all,_tmp5A7,0);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp5A5 != 0){
struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp5A5;
union Cyc_CfFlowInfo_FlowInfo _tmp5A9;union Cyc_CfFlowInfo_FlowInfo _tmp5AA;struct _tuple16 _tmp5A8=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp5A9=_tmp5A8.f1;_tmp5AA=_tmp5A8.f2;
inflow=_tmp5AA;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5A9,_tmp5A6);}else{
# 1979
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5A6);}{
# 1981
union Cyc_CfFlowInfo_FlowInfo _tmp5AB=clause_outflow;_LL386: if((_tmp5AB.BottomFL).tag != 1)goto _LL388;_LL387:
 goto _LL385;_LL388:;_LL389:
# 1985
 if(scs->tl == 0)
return clause_outflow;else{
# 1990
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp90F;void*_tmp90E;(_tmp90E=0,Cyc_Tcutil_terr(_tmp5A6->loc,((_tmp90F="switch clause may implicitly fallthru",_tag_dyneither(_tmp90F,sizeof(char),38))),_tag_dyneither(_tmp90E,sizeof(void*),0)));}else{
# 1993
const char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_warn(_tmp5A6->loc,((_tmp912="switch clause may implicitly fallthru",_tag_dyneither(_tmp912,sizeof(char),38))),_tag_dyneither(_tmp911,sizeof(void*),0)));}
# 1995
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 1997
goto _LL385;_LL385:;};};}
# 2000
return Cyc_CfFlowInfo_BottomFL();}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2005
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple26*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2007
struct Cyc_NewControlFlow_AnalEnv*_tmp5B1;struct Cyc_Dict_Dict _tmp5B2;unsigned int _tmp5B3;struct _tuple26*_tmp5B0=vdenv;_tmp5B1=(*_tmp5B0).f1;_tmp5B2=(*_tmp5B0).f2;_tmp5B3=(*_tmp5B0).f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5B1->fenv)->r,vd->type)){
# 2010
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp913;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp913.tag=0,((_tmp913.f1=vd,_tmp913)));
# 2012
void*_tmp5B4=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5B2,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp5B5=_tmp5B4;_LL38B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5B6->tag != 8)goto _LL38D;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5B7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5B6->f2);if(_tmp5B7->tag != 7)goto _LL38D;}}_LL38C:
 goto _LL38E;_LL38D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5B8->tag != 8)goto _LL38F;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5B9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5B8->f2);if(_tmp5B9->tag != 0)goto _LL38F;}}_LL38E:
 goto _LL390;_LL38F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5BA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5BA->tag != 8)goto _LL391;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5BB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5BA->f2);if(_tmp5BB->tag != 3)goto _LL391;else{if(_tmp5BB->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL391;}}}_LL390:
 goto _LL392;_LL391: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5BC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5BC->tag != 7)goto _LL393;}_LL392:
 goto _LL394;_LL393: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5BD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5BD->tag != 0)goto _LL395;}_LL394:
 goto _LL396;_LL395: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5BE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B5;if(_tmp5BE->tag != 3)goto _LL397;else{if(_tmp5BE->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL397;}}_LL396:
 goto _LL38A;_LL397:;_LL398:
# 2021
{const char*_tmp917;void*_tmp916[1];struct Cyc_String_pa_PrintArg_struct _tmp915;(_tmp915.tag=0,((_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp916[0]=& _tmp915,Cyc_Tcutil_warn(_tmp5B3,((_tmp917="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp917,sizeof(char),68))),_tag_dyneither(_tmp916,sizeof(void*),1)))))));}
# 2023
goto _LL38A;_LL38A:;}}
# 2028
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2030
{union Cyc_CfFlowInfo_FlowInfo _tmp5C3=inflow;struct Cyc_Dict_Dict _tmp5C4;_LL39A: if((_tmp5C3.ReachableFL).tag != 2)goto _LL39C;_tmp5C4=((struct _tuple13)(_tmp5C3.ReachableFL).val).f1;_LL39B: {
# 2032
struct _tuple26 _tmp918;struct _tuple26 _tmp5C5=(_tmp918.f1=env,((_tmp918.f2=_tmp5C4,((_tmp918.f3=decl->loc,_tmp918)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5C6=env->fenv;
{void*_tmp5C7=decl->r;struct Cyc_Absyn_Vardecl*_tmp5C9;struct Cyc_List_List*_tmp5CB;struct Cyc_List_List*_tmp5CD;_LL39F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5C8=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5C7;if(_tmp5C8->tag != 0)goto _LL3A1;else{_tmp5C9=_tmp5C8->f1;}}_LL3A0:
# 2036
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5C5,_tmp5C9);
goto _LL39E;_LL3A1: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5CA=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5C7;if(_tmp5CA->tag != 2)goto _LL3A3;else{if(_tmp5CA->f2 == 0)goto _LL3A3;_tmp5CB=(struct Cyc_List_List*)(*_tmp5CA->f2).v;}}_LL3A2:
 _tmp5CD=_tmp5CB;goto _LL3A4;_LL3A3: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5CC=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5C7;if(_tmp5CC->tag != 3)goto _LL3A5;else{_tmp5CD=_tmp5CC->f1;}}_LL3A4:
# 2040
((void(*)(void(*f)(struct _tuple26*,struct Cyc_Absyn_Vardecl*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5C5,_tmp5CD);
goto _LL39E;_LL3A5:;_LL3A6:
 goto _LL39E;_LL39E:;}
# 2044
goto _LL399;}_LL39C:;_LL39D:
 goto _LL399;_LL399:;}
# 2047
return;}
# 2050
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2055
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5D0;union Cyc_CfFlowInfo_FlowInfo*_tmp5D1;struct _tuple17 _tmp5CF=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp5D0=_tmp5CF.f1;_tmp5D1=_tmp5CF.f2;
inflow=*_tmp5D1;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp5D2=env->fenv;
void*_tmp5D3=s->r;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_Absyn_Stmt*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Stmt*_tmp5DF;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Stmt*_tmp5E3;struct Cyc_Absyn_Stmt*_tmp5E4;struct Cyc_Absyn_Stmt*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E7;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_List_List*_tmp5F2;struct Cyc_Absyn_Switch_clause*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_List_List*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_List_List*_tmp5FF;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_Absyn_Tvar*_tmp603;struct Cyc_Absyn_Vardecl*_tmp604;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Decl*_tmp607;struct Cyc_Absyn_Stmt*_tmp608;struct Cyc_Absyn_Stmt*_tmp60A;struct Cyc_Absyn_Exp*_tmp60C;_LL3A8: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp5D4=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5D4->tag != 0)goto _LL3AA;}_LL3A9:
 return inflow;_LL3AA: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5D5=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5D5->tag != 3)goto _LL3AC;else{if(_tmp5D5->f1 != 0)goto _LL3AC;}}_LL3AB:
# 2061
 if(env->noreturn){
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_terr(s->loc,((_tmp91B="`noreturn' function might return",_tag_dyneither(_tmp91B,sizeof(char),33))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3AC: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5D6=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5D6->tag != 3)goto _LL3AE;else{_tmp5D7=_tmp5D6->f1;}}_LL3AD:
# 2067
 if(env->noreturn){
const char*_tmp91E;void*_tmp91D;(_tmp91D=0,Cyc_Tcutil_terr(s->loc,((_tmp91E="`noreturn' function might return",_tag_dyneither(_tmp91E,sizeof(char),33))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp612;void*_tmp613;struct _tuple14 _tmp611=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5D7));_tmp612=_tmp611.f1;_tmp613=_tmp611.f2;
_tmp612=Cyc_NewControlFlow_use_Rval(env,_tmp5D7->loc,_tmp612,_tmp613);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp612);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp612);
return Cyc_CfFlowInfo_BottomFL();};_LL3AE: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp5D8=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5D8->tag != 1)goto _LL3B0;else{_tmp5D9=_tmp5D8->f1;}}_LL3AF:
# 2075
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5D9)).f1;_LL3B0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp5DA=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5DA->tag != 2)goto _LL3B2;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;}}_LL3B1:
# 2078
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5DB),_tmp5DC);_LL3B2: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp5DD=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5DD->tag != 4)goto _LL3B4;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;_tmp5E0=_tmp5DD->f3;}}_LL3B3: {
# 2081
union Cyc_CfFlowInfo_FlowInfo _tmp615;union Cyc_CfFlowInfo_FlowInfo _tmp616;struct _tuple16 _tmp614=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5DE);_tmp615=_tmp614.f1;_tmp616=_tmp614.f2;{
# 2088
union Cyc_CfFlowInfo_FlowInfo _tmp617=Cyc_NewControlFlow_anal_stmt(env,_tmp616,_tmp5E0);
union Cyc_CfFlowInfo_FlowInfo _tmp618=Cyc_NewControlFlow_anal_stmt(env,_tmp615,_tmp5DF);
return Cyc_CfFlowInfo_join_flow(_tmp5D2,env->all_changed,_tmp618,_tmp617);};}_LL3B4: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp5E1=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5E1->tag != 5)goto _LL3B6;else{_tmp5E2=(_tmp5E1->f1).f1;_tmp5E3=(_tmp5E1->f1).f2;_tmp5E4=_tmp5E1->f2;}}_LL3B5: {
# 2096
union Cyc_CfFlowInfo_FlowInfo*_tmp61A;struct _tuple17 _tmp619=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5E3);_tmp61A=_tmp619.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp61B=*_tmp61A;
union Cyc_CfFlowInfo_FlowInfo _tmp61D;union Cyc_CfFlowInfo_FlowInfo _tmp61E;struct _tuple16 _tmp61C=Cyc_NewControlFlow_anal_test(env,_tmp61B,_tmp5E2);_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp61F=Cyc_NewControlFlow_anal_stmt(env,_tmp61D,_tmp5E4);
Cyc_NewControlFlow_update_flow(env,_tmp5E3,_tmp61F);
return _tmp61E;};};}_LL3B6: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp5E5=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5E5->tag != 14)goto _LL3B8;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=(_tmp5E5->f2).f1;_tmp5E8=(_tmp5E5->f2).f2;}}_LL3B7: {
# 2106
union Cyc_CfFlowInfo_FlowInfo _tmp620=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E6);
union Cyc_CfFlowInfo_FlowInfo*_tmp622;struct _tuple17 _tmp621=Cyc_NewControlFlow_pre_stmt_check(env,_tmp620,_tmp5E8);_tmp622=_tmp621.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp623=*_tmp622;
union Cyc_CfFlowInfo_FlowInfo _tmp625;union Cyc_CfFlowInfo_FlowInfo _tmp626;struct _tuple16 _tmp624=Cyc_NewControlFlow_anal_test(env,_tmp623,_tmp5E7);_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;
Cyc_NewControlFlow_update_flow(env,_tmp5E6,_tmp625);
return _tmp626;};}_LL3B8: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp5E9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5E9->tag != 9)goto _LL3BA;else{_tmp5EA=_tmp5E9->f1;_tmp5EB=(_tmp5E9->f2).f1;_tmp5EC=(_tmp5E9->f2).f2;_tmp5ED=(_tmp5E9->f3).f1;_tmp5EE=(_tmp5E9->f3).f2;_tmp5EF=_tmp5E9->f4;}}_LL3B9: {
# 2115
union Cyc_CfFlowInfo_FlowInfo _tmp627=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5EA)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp629;struct _tuple17 _tmp628=Cyc_NewControlFlow_pre_stmt_check(env,_tmp627,_tmp5EC);_tmp629=_tmp628.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp62A=*_tmp629;
union Cyc_CfFlowInfo_FlowInfo _tmp62C;union Cyc_CfFlowInfo_FlowInfo _tmp62D;struct _tuple16 _tmp62B=Cyc_NewControlFlow_anal_test(env,_tmp62A,_tmp5EB);_tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp62E=Cyc_NewControlFlow_anal_stmt(env,_tmp62C,_tmp5EF);
union Cyc_CfFlowInfo_FlowInfo*_tmp630;struct _tuple17 _tmp62F=Cyc_NewControlFlow_pre_stmt_check(env,_tmp62E,_tmp5EE);_tmp630=_tmp62F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp631=*_tmp630;
union Cyc_CfFlowInfo_FlowInfo _tmp632=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp631,_tmp5ED)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5EC,_tmp632);
return _tmp62D;};};};}_LL3BA: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5F0=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5F0->tag != 6)goto _LL3BC;else{if(_tmp5F0->f1 != 0)goto _LL3BC;}}_LL3BB:
# 2127
 return Cyc_CfFlowInfo_BottomFL();_LL3BC: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp5F1=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5F1->tag != 11)goto _LL3BE;else{_tmp5F2=_tmp5F1->f1;if(_tmp5F1->f2 == 0)goto _LL3BE;_tmp5F3=*_tmp5F1->f2;}}_LL3BD: {
# 2129
struct _RegionHandle _tmp633=_new_region("temp");struct _RegionHandle*temp=& _tmp633;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp635;struct Cyc_List_List*_tmp636;struct _tuple20 _tmp634=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp5F2,1,1);_tmp635=_tmp634.f1;_tmp636=_tmp634.f2;
# 2132
for(0;_tmp636 != 0;(_tmp636=_tmp636->tl,_tmp5F2=_tmp5F2->tl)){
_tmp635=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F2))->hd)->loc,_tmp635,(void*)_tmp636->hd);}
# 2135
_tmp635=Cyc_NewControlFlow_add_vars(_tmp5D2,_tmp635,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F3->pat_vars))->v,_tmp5D2->unknown_all,s->loc,0);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp5F3->body,_tmp635);{
union Cyc_CfFlowInfo_FlowInfo _tmp637=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp637;};}
# 2130
;_pop_region(temp);}_LL3BE: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5F4=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5F4->tag != 6)goto _LL3C0;else{_tmp5F5=_tmp5F4->f1;}}_LL3BF:
# 2139
 _tmp5F7=_tmp5F5;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp5F6=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5F6->tag != 7)goto _LL3C2;else{_tmp5F7=_tmp5F6->f1;}}_LL3C1:
 _tmp5F9=_tmp5F7;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp5F8=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5F8->tag != 8)goto _LL3C4;else{_tmp5F9=_tmp5F8->f2;}}_LL3C3:
# 2144
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp638=_tmp5D0->encloser;
struct Cyc_Absyn_Stmt*_tmp639=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp5F9)))->encloser;
while(_tmp639 != _tmp638){
struct Cyc_Absyn_Stmt*_tmp63A=(Cyc_NewControlFlow_get_stmt_annot(_tmp638))->encloser;
if(_tmp63A == _tmp638){
{const char*_tmp921;void*_tmp920;(_tmp920=0,Cyc_Tcutil_terr(s->loc,((_tmp921="goto enters local scope or exception handler",_tag_dyneither(_tmp921,sizeof(char),45))),_tag_dyneither(_tmp920,sizeof(void*),0)));}
break;}
# 2153
_tmp638=_tmp63A;}}
# 2158
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5F9),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3C4: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5FA=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5FA->tag != 10)goto _LL3C6;else{_tmp5FB=_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}_LL3C5: {
# 2162
union Cyc_CfFlowInfo_FlowInfo _tmp63E;void*_tmp63F;struct _tuple14 _tmp63D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5FB);_tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;
_tmp63E=Cyc_NewControlFlow_use_Rval(env,_tmp5FB->loc,_tmp63E,_tmp63F);
return Cyc_NewControlFlow_anal_scs(env,_tmp63E,_tmp5FC);}_LL3C6: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp5FD=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp5FD->tag != 15)goto _LL3C8;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}_LL3C7: {
# 2169
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5FE);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2177
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2180
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5FF);
{union Cyc_CfFlowInfo_FlowInfo _tmp640=scs_outflow;_LL3D3: if((_tmp640.BottomFL).tag != 1)goto _LL3D5;_LL3D4:
 goto _LL3D2;_LL3D5:;_LL3D6: {
const char*_tmp924;void*_tmp923;(_tmp923=0,Cyc_Tcutil_terr(s->loc,((_tmp924="last catch clause may implicitly fallthru",_tag_dyneither(_tmp924,sizeof(char),42))),_tag_dyneither(_tmp923,sizeof(void*),0)));}_LL3D2:;}
# 2186
outflow=s1_outflow;
# 2188
return outflow;};};}_LL3C8: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp600=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp600->tag != 12)goto _LL3CA;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp601=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(*_tmp600->f1).r;if(_tmp601->tag != 5)goto _LL3CA;else{_tmp602=_tmp601->f1;_tmp603=_tmp601->f2;_tmp604=_tmp601->f3;}}_tmp605=_tmp600->f2;}}_LL3C9: {
# 2194
union Cyc_CfFlowInfo_FlowInfo _tmp644;union Cyc_CfFlowInfo_AbsLVal _tmp645;struct _tuple15 _tmp643=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp602);_tmp644=_tmp643.f1;_tmp645=_tmp643.f2;{
# 2198
struct _tuple15 _tmp925;struct _tuple15 _tmp647=(_tmp925.f1=_tmp644,((_tmp925.f2=_tmp645,_tmp925)));struct Cyc_Dict_Dict _tmp648;struct Cyc_List_List*_tmp649;struct Cyc_CfFlowInfo_Place*_tmp64A;_LL3D8: if(((_tmp647.f1).ReachableFL).tag != 2)goto _LL3DA;_tmp648=((struct _tuple13)((_tmp647.f1).ReachableFL).val).f1;_tmp649=((struct _tuple13)((_tmp647.f1).ReachableFL).val).f2;if(((_tmp647.f2).PlaceL).tag != 1)goto _LL3DA;_tmp64A=(struct Cyc_CfFlowInfo_Place*)((_tmp647.f2).PlaceL).val;_LL3D9: {
# 2200
void*_tmp64B=Cyc_CfFlowInfo_lookup_place(_tmp648,_tmp64A);
# 2202
void*t=(void*)_check_null(_tmp602->topt);
{void*_tmp64C=_tmp602->r;struct Cyc_Absyn_Exp*_tmp64E;_LL3DF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp64C;if(_tmp64D->tag != 13)goto _LL3E1;else{_tmp64E=_tmp64D->f2;}}_LL3E0:
 t=(void*)_check_null(_tmp64E->topt);goto _LL3DE;_LL3E1:;_LL3E2:
 goto _LL3DE;_LL3DE:;}{
# 2207
void*_tmp64F=Cyc_CfFlowInfo_make_unique_consumed(_tmp5D2,t,_tmp602,env->iteration_num,_tmp64B);
_tmp648=Cyc_CfFlowInfo_assign_place(_tmp5D2,_tmp602->loc,_tmp648,env->all_changed,_tmp64A,_tmp64F);
_tmp644=Cyc_CfFlowInfo_ReachableFL(_tmp648,_tmp649);{
# 2213
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp5D2,_tmp648,_tmp64B)){case Cyc_CfFlowInfo_AllIL: _LL3E3:
 leaf=_tmp5D2->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL3E4:
 leaf=_tmp5D2->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL3E5:
 leaf=_tmp5D2->unknown_this;break;}{
# 2221
void*_tmp650=Cyc_CfFlowInfo_typ_to_absrval(_tmp5D2,t,0,leaf);
_tmp644=Cyc_NewControlFlow_use_Rval(env,_tmp602->loc,_tmp644,_tmp64B);{
struct Cyc_List_List _tmp926;struct Cyc_List_List _tmp651=(_tmp926.hd=_tmp604,((_tmp926.tl=0,_tmp926)));
_tmp644=Cyc_NewControlFlow_add_vars(_tmp5D2,_tmp644,(struct Cyc_List_List*)& _tmp651,_tmp5D2->unknown_all,s->loc,0);
# 2228
_tmp644=Cyc_NewControlFlow_anal_stmt(env,_tmp644,_tmp605);{
# 2233
union Cyc_CfFlowInfo_FlowInfo _tmp652=_tmp644;struct Cyc_Dict_Dict _tmp653;struct Cyc_List_List*_tmp654;_LL3E8: if((_tmp652.ReachableFL).tag != 2)goto _LL3EA;_tmp653=((struct _tuple13)(_tmp652.ReachableFL).val).f1;_tmp654=((struct _tuple13)(_tmp652.ReachableFL).val).f2;_LL3E9:
# 2235
 _tmp653=Cyc_CfFlowInfo_assign_place(_tmp5D2,s->loc,_tmp653,env->all_changed,_tmp64A,_tmp650);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp653,_tmp654);
# 2240
return outflow;_LL3EA:;_LL3EB:
 return _tmp644;_LL3E7:;};};};};};}_LL3DA: if(((_tmp647.f1).BottomFL).tag != 1)goto _LL3DC;_LL3DB:
# 2243
 return _tmp644;_LL3DC:;_LL3DD:
{const char*_tmp929;void*_tmp928;(_tmp928=0,Cyc_Tcutil_terr(_tmp602->loc,((_tmp929="bad alias expression--no unique path found",_tag_dyneither(_tmp929,sizeof(char),43))),_tag_dyneither(_tmp928,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL3D7:;};}_LL3CA: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp606=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp606->tag != 12)goto _LL3CC;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL3CB:
# 2249
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp607),_tmp608);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp607);
return outflow;_LL3CC: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp609=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp609->tag != 13)goto _LL3CE;else{_tmp60A=_tmp609->f2;}}_LL3CD:
# 2254
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp60A);_LL3CE: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp60B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp5D3;if(_tmp60B->tag != 16)goto _LL3D0;else{_tmp60C=_tmp60B->f1;}}_LL3CF: {
# 2257
union Cyc_CfFlowInfo_FlowInfo _tmp659;void*_tmp65A;struct _tuple14 _tmp658=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp60C);_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp65B=Cyc_NewControlFlow_use_Rval(env,_tmp60C->loc,_tmp659,_tmp65A);
void*_tmp65C=Cyc_Tcutil_compress((void*)_check_null(_tmp60C->topt));void*_tmp65E;_LL3ED: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 15)goto _LL3EF;else{_tmp65E=(void*)_tmp65D->f1;}}_LL3EE:
# 2261
 return Cyc_CfFlowInfo_kill_flow_region(_tmp5D2,_tmp659,_tmp65E);_LL3EF:;_LL3F0: {
struct Cyc_Core_Impossible_exn_struct _tmp92F;const char*_tmp92E;struct Cyc_Core_Impossible_exn_struct*_tmp92D;(int)_throw((void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92F.tag=Cyc_Core_Impossible,((_tmp92F.f1=((_tmp92E="anal_stmt -- reset_region",_tag_dyneither(_tmp92E,sizeof(char),26))),_tmp92F)))),_tmp92D)))));}_LL3EC:;};}_LL3D0:;_LL3D1: {
# 2265
struct Cyc_Core_Impossible_exn_struct _tmp935;const char*_tmp934;struct Cyc_Core_Impossible_exn_struct*_tmp933;(int)_throw((void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp935.tag=Cyc_Core_Impossible,((_tmp935.f1=((_tmp934="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp934,sizeof(char),56))),_tmp935)))),_tmp933)))));}_LL3A7:;};}
# 2270
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2272
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp665=env->fenv;
void*_tmp666=decl->r;struct Cyc_Absyn_Vardecl*_tmp668;struct Cyc_List_List*_tmp66A;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_List_List*_tmp66D;struct Cyc_Absyn_Fndecl*_tmp66F;struct Cyc_Absyn_Tvar*_tmp671;struct Cyc_Absyn_Vardecl*_tmp672;int _tmp673;struct Cyc_Absyn_Exp*_tmp674;_LL3F2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp667=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp666;if(_tmp667->tag != 0)goto _LL3F4;else{_tmp668=_tmp667->f1;}}_LL3F3: {
# 2282
struct Cyc_List_List _tmp936;struct Cyc_List_List _tmp675=(_tmp936.hd=_tmp668,((_tmp936.tl=0,_tmp936)));
inflow=Cyc_NewControlFlow_add_vars(_tmp665,inflow,(struct Cyc_List_List*)& _tmp675,_tmp665->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp676=_tmp668->initializer;
if(_tmp676 == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp678;void*_tmp679;struct _tuple14 _tmp677=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_tmp676);_tmp678=_tmp677.f1;_tmp679=_tmp677.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp67A=_tmp678;struct Cyc_Dict_Dict _tmp67B;struct Cyc_List_List*_tmp67C;_LL3FF: if((_tmp67A.BottomFL).tag != 1)goto _LL401;_LL400:
 return Cyc_CfFlowInfo_BottomFL();_LL401: if((_tmp67A.ReachableFL).tag != 2)goto _LL3FE;_tmp67B=((struct _tuple13)(_tmp67A.ReachableFL).val).f1;_tmp67C=((struct _tuple13)(_tmp67A.ReachableFL).val).f2;_LL402:
# 2291
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp93C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp93B;struct Cyc_CfFlowInfo_Place*_tmp93A;_tmp67B=Cyc_CfFlowInfo_assign_place(_tmp665,decl->loc,_tmp67B,env->all_changed,(
(_tmp93A=_region_malloc(env->r,sizeof(*_tmp93A)),((_tmp93A->root=(void*)((_tmp93C=_region_malloc(env->r,sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp93B.tag=0,((_tmp93B.f1=_tmp668,_tmp93B)))),_tmp93C)))),((_tmp93A->fields=0,_tmp93A)))))),_tmp679);}
# 2294
_tmp67C=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp67C,_tmp668,(struct Cyc_Absyn_Exp*)_check_null(_tmp668->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp680=Cyc_CfFlowInfo_ReachableFL(_tmp67B,_tmp67C);
Cyc_NewControlFlow_update_tryflow(env,_tmp680);
# 2299
return _tmp680;};_LL3FE:;};};};}_LL3F4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp669=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp666;if(_tmp669->tag != 2)goto _LL3F6;else{if(_tmp669->f2 == 0)goto _LL3F6;_tmp66A=(struct Cyc_List_List*)(*_tmp669->f2).v;_tmp66B=_tmp669->f3;}}_LL3F5: {
# 2304
union Cyc_CfFlowInfo_FlowInfo _tmp683;void*_tmp684;struct _tuple14 _tmp682=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp66B);_tmp683=_tmp682.f1;_tmp684=_tmp682.f2;
_tmp683=Cyc_NewControlFlow_use_Rval(env,_tmp66B->loc,_tmp683,_tmp684);
return Cyc_NewControlFlow_add_vars(_tmp665,_tmp683,_tmp66A,_tmp665->unknown_all,decl->loc,0);}_LL3F6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp66C=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp666;if(_tmp66C->tag != 3)goto _LL3F8;else{_tmp66D=_tmp66C->f1;}}_LL3F7:
# 2309
 return Cyc_NewControlFlow_add_vars(_tmp665,inflow,_tmp66D,_tmp665->unknown_none,decl->loc,0);_LL3F8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp66E=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp666;if(_tmp66E->tag != 1)goto _LL3FA;else{_tmp66F=_tmp66E->f1;}}_LL3F9:
# 2312
 Cyc_NewControlFlow_check_nested_fun(_tmp665,inflow,_tmp66F);{
void*t=(void*)_check_null(_tmp66F->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp685=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp66F->fn_vardecl);
# 2318
struct Cyc_List_List*_tmp93D;return Cyc_NewControlFlow_add_vars(_tmp665,inflow,((_tmp93D=_region_malloc(env->r,sizeof(*_tmp93D)),((_tmp93D->hd=_tmp685,((_tmp93D->tl=0,_tmp93D)))))),_tmp665->unknown_all,decl->loc,0);};_LL3FA: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp670=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp666;if(_tmp670->tag != 4)goto _LL3FC;else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_tmp673=_tmp670->f3;_tmp674=_tmp670->f4;}}_LL3FB:
# 2321
 if(_tmp674 != 0){
struct Cyc_Absyn_Exp*_tmp687=(struct Cyc_Absyn_Exp*)_tmp674;
union Cyc_CfFlowInfo_FlowInfo _tmp689;void*_tmp68A;struct _tuple14 _tmp688=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp687);_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp687->loc,_tmp689,_tmp68A);}{
# 2326
struct Cyc_List_List _tmp93E;struct Cyc_List_List _tmp68B=(_tmp93E.hd=_tmp672,((_tmp93E.tl=0,_tmp93E)));
return Cyc_NewControlFlow_add_vars(_tmp665,inflow,(struct Cyc_List_List*)& _tmp68B,_tmp665->unknown_all,decl->loc,0);};_LL3FC:;_LL3FD: {
# 2329
struct Cyc_Core_Impossible_exn_struct _tmp944;const char*_tmp943;struct Cyc_Core_Impossible_exn_struct*_tmp942;(int)_throw((void*)((_tmp942=_cycalloc(sizeof(*_tmp942)),((_tmp942[0]=((_tmp944.tag=Cyc_Core_Impossible,((_tmp944.f1=((_tmp943="anal_decl: unexpected decl variant",_tag_dyneither(_tmp943,sizeof(char),35))),_tmp944)))),_tmp942)))));}_LL3F1:;}
# 2369 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp690=_new_region("frgn");struct _RegionHandle*frgn=& _tmp690;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2371
;_pop_region(frgn);}
# 2376
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2380
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2384
struct _RegionHandle*_tmp691=fenv->r;
unsigned int _tmp692=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp692,1);{
# 2390
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp693=inflow;struct Cyc_Dict_Dict _tmp694;struct Cyc_List_List*_tmp695;_LL404: if((_tmp693.BottomFL).tag != 1)goto _LL406;_LL405: {
const char*_tmp947;void*_tmp946;(_tmp946=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp947="check_fun",_tag_dyneither(_tmp947,sizeof(char),10))),_tag_dyneither(_tmp946,sizeof(void*),0)));}_LL406: if((_tmp693.ReachableFL).tag != 2)goto _LL403;_tmp694=((struct _tuple13)(_tmp693.ReachableFL).val).f1;_tmp695=((struct _tuple13)(_tmp693.ReachableFL).val).f2;_LL407: {
# 2397
struct Cyc_List_List*atts;
{void*_tmp698=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp69A;_LL409: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp699=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp698;if(_tmp699->tag != 9)goto _LL40B;else{_tmp69A=(_tmp699->f1).attributes;}}_LL40A:
 atts=_tmp69A;goto _LL408;_LL40B:;_LL40C: {
const char*_tmp94A;void*_tmp949;(_tmp949=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp94A="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp94A,sizeof(char),50))),_tag_dyneither(_tmp949,sizeof(void*),0)));}_LL408:;}
# 2402
for(0;atts != 0;atts=atts->tl){
void*_tmp69D=(void*)atts->hd;int _tmp69F;int _tmp6A1;int _tmp6A3;_LL40E: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp69E=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp69D;if(_tmp69E->tag != 21)goto _LL410;else{_tmp69F=_tmp69E->f1;}}_LL40F: {
# 2405
unsigned int j=(unsigned int)_tmp69F;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp94D;void*_tmp94C;(_tmp94C=0,Cyc_Tcutil_terr(_tmp692,((_tmp94D="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp94D,sizeof(char),52))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}
continue;}{
# 2411
struct Cyc_Absyn_Vardecl*_tmp6A6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6A6->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_terr(_tmp692,((_tmp950="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp950,sizeof(char),52))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}
continue;}{
# 2417
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6A9=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp953;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp952;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6AA=(_tmp952=_region_malloc(_tmp691,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=2,((_tmp953.f1=(int)j,((_tmp953.f2=(void*)t,_tmp953)))))),_tmp952)));
struct Cyc_CfFlowInfo_Place*_tmp954;struct Cyc_CfFlowInfo_Place*_tmp6AB=(_tmp954=_region_malloc(_tmp691,sizeof(*_tmp954)),((_tmp954->root=(void*)_tmp6AA,((_tmp954->fields=0,_tmp954)))));
_tmp694=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp694,(void*)_tmp6AA,_tmp6A9);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp95A;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp959;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp957;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp956;_tmp694=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp694,(void*)((_tmp956=_region_malloc(_tmp691,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=0,((_tmp957.f1=_tmp6A6,_tmp957)))),_tmp956)))),(void*)((_tmp959=_region_malloc(_tmp691,sizeof(*_tmp959)),((_tmp959[0]=((_tmp95A.tag=5,((_tmp95A.f1=_tmp6AB,_tmp95A)))),_tmp959)))));}
goto _LL40D;};};}_LL410: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6A0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp69D;if(_tmp6A0->tag != 20)goto _LL412;else{_tmp6A1=_tmp6A0->f1;}}_LL411: {
# 2427
unsigned int j=(unsigned int)_tmp6A1;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_terr(_tmp692,((_tmp95D="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp95D,sizeof(char),51))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}
# 2431
continue;}{
# 2433
struct Cyc_Absyn_Vardecl*_tmp6B5=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp6B6=Cyc_Tcutil_compress(_tmp6B5->type);void*_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;union Cyc_Absyn_Constraint*_tmp6BB;_LL417: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6;if(_tmp6B7->tag != 5)goto _LL419;else{_tmp6B8=(_tmp6B7->f1).elt_typ;_tmp6B9=((_tmp6B7->f1).ptr_atts).nullable;_tmp6BA=((_tmp6B7->f1).ptr_atts).bounds;_tmp6BB=((_tmp6B7->f1).ptr_atts).zero_term;}}_LL418:
# 2436
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B9)){
const char*_tmp960;void*_tmp95F;(_tmp95F=0,Cyc_Tcutil_terr(_tmp692,((_tmp960="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp960,sizeof(char),55))),_tag_dyneither(_tmp95F,sizeof(void*),0)));}
# 2439
if(!Cyc_Tcutil_is_bound_one(_tmp6BA)){
const char*_tmp963;void*_tmp962;(_tmp962=0,Cyc_Tcutil_terr(_tmp692,((_tmp963="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp963,sizeof(char),57))),_tag_dyneither(_tmp962,sizeof(void*),0)));}
# 2442
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BB)){
const char*_tmp966;void*_tmp965;(_tmp965=0,Cyc_Tcutil_terr(_tmp692,((_tmp966="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp966,sizeof(char),77))),_tag_dyneither(_tmp965,sizeof(void*),0)));}{
# 2445
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp969;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp968;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6C2=(_tmp968=_region_malloc(_tmp691,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=2,((_tmp969.f1=(int)j,((_tmp969.f2=(void*)_tmp6B8,_tmp969)))))),_tmp968)));
struct Cyc_CfFlowInfo_Place*_tmp96A;struct Cyc_CfFlowInfo_Place*_tmp6C3=(_tmp96A=_region_malloc(_tmp691,sizeof(*_tmp96A)),((_tmp96A->root=(void*)_tmp6C2,((_tmp96A->fields=0,_tmp96A)))));
_tmp694=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp694,(void*)_tmp6C2,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6B8,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp970;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp96F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp96D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp96C;_tmp694=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp694,(void*)((_tmp96C=_region_malloc(_tmp691,sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96D.tag=0,((_tmp96D.f1=_tmp6B5,_tmp96D)))),_tmp96C)))),(void*)((_tmp96F=_region_malloc(_tmp691,sizeof(*_tmp96F)),((_tmp96F[0]=((_tmp970.tag=5,((_tmp970.f1=_tmp6C3,_tmp970)))),_tmp96F)))));}
{struct Cyc_List_List*_tmp971;param_roots=((_tmp971=_region_malloc(_tmp691,sizeof(*_tmp971)),((_tmp971->hd=_tmp6C3,((_tmp971->tl=param_roots,_tmp971))))));}
goto _LL416;};_LL419:;_LL41A: {
const char*_tmp974;void*_tmp973;(_tmp973=0,Cyc_Tcutil_terr(_tmp692,((_tmp974="initializes attribute on non-pointer",_tag_dyneither(_tmp974,sizeof(char),37))),_tag_dyneither(_tmp973,sizeof(void*),0)));}_LL416:;}
# 2453
goto _LL40D;};}_LL412: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6A2=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp69D;if(_tmp6A2->tag != 22)goto _LL414;else{_tmp6A3=_tmp6A2->f1;}}_LL413: {
# 2455
unsigned int j=(unsigned int)_tmp6A3;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp977;void*_tmp976;(_tmp976=0,Cyc_Tcutil_terr(_tmp692,((_tmp977="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp977,sizeof(char),49))),_tag_dyneither(_tmp976,sizeof(void*),0)));}
continue;}{
# 2460
struct Cyc_Absyn_Vardecl*_tmp6D0=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6D0->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp97A;void*_tmp979;(_tmp979=0,Cyc_Tcutil_terr(_tmp692,((_tmp97A="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp97A,sizeof(char),49))),_tag_dyneither(_tmp979,sizeof(void*),0)));}
continue;}{
# 2467
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp97D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp97C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6D3=(_tmp97C=_region_malloc(_tmp691,sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=0,((_tmp97D.f1=_tmp6D0,_tmp97D)))),_tmp97C)));
struct Cyc_CfFlowInfo_Place*_tmp97E;struct Cyc_CfFlowInfo_Place*_tmp6D4=(_tmp97E=_region_malloc(_tmp691,sizeof(*_tmp97E)),((_tmp97E->root=(void*)_tmp6D3,((_tmp97E->fields=0,_tmp97E)))));
void*_tmp6D5=Cyc_CfFlowInfo_lookup_place(_tmp694,_tmp6D4);
{struct _tuple24*_tmp981;struct Cyc_List_List*_tmp980;noconsume_rvals=((_tmp980=_region_malloc(_tmp691,sizeof(*_tmp980)),((_tmp980->hd=((_tmp981=_region_malloc(_tmp691,sizeof(*_tmp981)),((_tmp981->f1=_tmp6D4,((_tmp981->f2=_tmp6D5,_tmp981)))))),((_tmp980->tl=noconsume_rvals,_tmp980))))));}
# 2472
goto _LL40D;};};}_LL414:;_LL415:
 goto _LL40D;_LL40D:;}
# 2475
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp694,_tmp695);}_LL403:;}{
# 2478
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp691,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp982;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp982=_region_malloc(_tmp691,sizeof(*_tmp982)),((_tmp982->r=_tmp691,((_tmp982->fenv=fenv,((_tmp982->iterate_again=1,((_tmp982->iteration_num=0,((_tmp982->in_try=0,((_tmp982->tryflow=inflow,((_tmp982->all_changed=0,((_tmp982->noreturn=noreturn,((_tmp982->param_roots=param_roots,((_tmp982->noconsume_params=noconsume_rvals,((_tmp982->flow_table=flow_table,_tmp982)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2488
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2491
union Cyc_CfFlowInfo_FlowInfo _tmp6DB=outflow;_LL41C: if((_tmp6DB.BottomFL).tag != 1)goto _LL41E;_LL41D:
 goto _LL41B;_LL41E:;_LL41F:
# 2494
 Cyc_NewControlFlow_check_init_params(_tmp692,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp692,env,outflow);
# 2498
if(noreturn){
const char*_tmp985;void*_tmp984;(_tmp984=0,Cyc_Tcutil_terr(_tmp692,((_tmp985="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp985,sizeof(char),46))),_tag_dyneither(_tmp984,sizeof(void*),0)));}else{
# 2501
void*_tmp6DE=Cyc_Tcutil_compress(fd->ret_type);_LL421: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6DF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6DE;if(_tmp6DF->tag != 0)goto _LL423;}_LL422:
 goto _LL420;_LL423: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DE;if(_tmp6E0->tag != 7)goto _LL425;}_LL424:
 goto _LL426;_LL425: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6DE;if(_tmp6E1->tag != 6)goto _LL427;}_LL426:
# 2505
{const char*_tmp988;void*_tmp987;(_tmp987=0,Cyc_Tcutil_warn(_tmp692,((_tmp988="function may not return a value",_tag_dyneither(_tmp988,sizeof(char),32))),_tag_dyneither(_tmp987,sizeof(void*),0)));}goto _LL420;_LL427:;_LL428:
# 2507
{const char*_tmp98B;void*_tmp98A;(_tmp98A=0,Cyc_Tcutil_terr(_tmp692,((_tmp98B="function may not return a value",_tag_dyneither(_tmp98B,sizeof(char),32))),_tag_dyneither(_tmp98A,sizeof(void*),0)));}goto _LL420;_LL420:;}
# 2509
goto _LL41B;_LL41B:;};};};}
# 2514
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp6E7=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp6E9;struct Cyc_List_List*_tmp6EB;struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*_tmp6EF;_LL42A: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6E8=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6E7;if(_tmp6E8->tag != 1)goto _LL42C;else{_tmp6E9=_tmp6E8->f1;}}_LL42B:
 Cyc_NewControlFlow_check_fun(_tmp6E9);goto _LL429;_LL42C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp6EA=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6E7;if(_tmp6EA->tag != 12)goto _LL42E;else{_tmp6EB=_tmp6EA->f1;}}_LL42D:
 _tmp6ED=_tmp6EB;goto _LL42F;_LL42E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp6EC=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6E7;if(_tmp6EC->tag != 11)goto _LL430;else{_tmp6ED=_tmp6EC->f2;}}_LL42F:
 _tmp6EF=_tmp6ED;goto _LL431;_LL430: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp6EE=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6E7;if(_tmp6EE->tag != 10)goto _LL432;else{_tmp6EF=_tmp6EE->f2;}}_LL431:
 Cyc_NewControlFlow_cf_check(_tmp6EF);goto _LL429;_LL432: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp6F0=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6E7;if(_tmp6F0->tag != 13)goto _LL434;}_LL433:
 goto _LL429;_LL434:;_LL435:
 goto _LL429;_LL429:;}}
