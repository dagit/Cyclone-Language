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
# 264
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 307
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 924 "absyn.h"
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 969
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1162
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1166
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
# 130 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 99
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 234 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 329
int Cyc_Tcutil_is_noreturn(void*);
# 344
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
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
typedef struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_vardecl_opt_t;
# 66
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 74
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 79
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp705;struct Cyc_NewControlFlow_CFStmtAnnot _tmp704;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp703;enclosee->annot=(void*)((_tmp703=_cycalloc(sizeof(*_tmp703)),((_tmp703[0]=((_tmp705.tag=Cyc_NewControlFlow_CFAnnot,((_tmp705.f1=((_tmp704.encloser=encloser,((_tmp704.visited=0,_tmp704)))),_tmp705)))),_tmp703))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 105 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 113
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 122
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp70B;const char*_tmp70A;struct Cyc_Core_Impossible_exn_struct*_tmp709;(int)_throw((void*)((_tmp709=_cycalloc(sizeof(*_tmp709)),((_tmp709[0]=((_tmp70B.tag=Cyc_Core_Impossible,((_tmp70B.f1=((_tmp70A="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp70A,sizeof(char),37))),_tmp70B)))),_tmp709)))));}_LL0:;}
# 129
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp70C;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp70C=_region_malloc(env->r,sizeof(*_tmp70C)),((_tmp70C[0]=Cyc_CfFlowInfo_BottomFL(),_tmp70C)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 136
return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 139
static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 143
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 146
_tmpA->visited=env->iteration_num;{
struct _tuple17 _tmp70D;return(_tmp70D.f1=_tmpA,((_tmp70D.f2=_tmpB,_tmp70D)));};}
# 158 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 166
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 173
static void Cyc_NewControlFlow_check_unique_root(struct _tuple18*ckenv,void*root,void*rval){
# 175
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple18*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp13;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp12=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 178
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp13->type)){
retry: {void*_tmp14=rval;void*_tmp16;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 8)goto _LLD;else{_tmp16=(void*)_tmp15->f2;}}_LLC:
 rval=_tmp16;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp18->tag != 3)goto _LL11;else{if(_tmp18->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp19->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 187
{const char*_tmp711;void*_tmp710[1];struct Cyc_String_pa_PrintArg_struct _tmp70F;(_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp710[0]=& _tmp70F,Cyc_Tcutil_warn(_tmpF,((_tmp711="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp711,sizeof(char),60))),_tag_dyneither(_tmp710,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 191
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 199
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1D=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1E=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1F=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1E);
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple16 _tmp712;struct _tuple16 _tmp21=(_tmp712.f1=flow,((_tmp712.f2=_tmp1F,_tmp712)));struct Cyc_Dict_Dict _tmp22;struct Cyc_Dict_Dict _tmp23;_LL16: if(((_tmp21.f1).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple13)((_tmp21.f1).ReachableFL).val).f1;if(((_tmp21.f2).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple13)((_tmp21.f2).ReachableFL).val).f1;_LL17: {
# 206
struct _tuple18 _tmp713;struct _tuple18 _tmp24=(_tmp713.f1=env,((_tmp713.f2=s->loc,((_tmp713.f3=_tmp23,_tmp713)))));
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp24,_tmp22);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 214
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1F,*_tmp1E)){
# 221
*_tmp1E=_tmp1F;
# 225
if(_tmp1D->visited == env->iteration_num)
# 227
env->iterate_again=1;}}
# 232
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 236
union Cyc_CfFlowInfo_FlowInfo _tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;_LL1B: if((_tmp26.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple13)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple13)(_tmp26.ReachableFL).val).f2;_LL1E:
# 239
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp716;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp715;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp715=_region_malloc(fenv->r,sizeof(*_tmp715)),((_tmp715[0]=((_tmp716.tag=0,((_tmp716.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp716)))),_tmp715)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp719;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp718;_tmp2A=(void*)((_tmp718=_region_malloc(fenv->r,sizeof(*_tmp718)),((_tmp718[0]=((_tmp719.tag=8,((_tmp719.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp719.f2=_tmp2A,_tmp719)))))),_tmp718))));}
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
const char*_tmp71C;void*_tmp71B;(_tmp71B=0,Cyc_Tcutil_terr(loc,((_tmp71C="expression may not be fully initialized",_tag_dyneither(_tmp71C,sizeof(char),40))),_tag_dyneither(_tmp71B,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp34=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp30,env->all_changed,r);
if(_tmp30.t == _tmp34.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp35=Cyc_CfFlowInfo_ReachableFL(_tmp34,_tmp31);
Cyc_NewControlFlow_update_tryflow(env,_tmp35);
return _tmp35;};};_LL1F:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 266
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp71D;struct _tuple0 _tmp37=(_tmp71D.f1=Cyc_Tcutil_compress(t),((_tmp71D.f2=r,_tmp71D)));void*_tmp3C;struct Cyc_Absyn_Datatypefield*_tmp3E;struct _dyneither_ptr _tmp40;struct Cyc_List_List*_tmp42;struct _dyneither_ptr _tmp44;union Cyc_Absyn_AggrInfoU _tmp46;struct Cyc_List_List*_tmp47;struct _dyneither_ptr _tmp49;enum Cyc_Absyn_AggrKind _tmp4B;struct Cyc_List_List*_tmp4C;struct _dyneither_ptr _tmp4E;_LL25: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp38=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp38->tag != 3)goto _LL27;else{if(_tmp38->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL27;}}_LL26:
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
{const char*_tmp720;void*_tmp71F;(_tmp71F=0,Cyc_Tcutil_warn(loc,((_tmp720="argument may still contain unique pointers",_tag_dyneither(_tmp720,sizeof(char),43))),_tag_dyneither(_tmp71F,sizeof(void*),0)));}
return;_LL37:;_LL38:
 return;_LL24:;}
# 349 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp56=inflow;struct Cyc_Dict_Dict _tmp57;struct Cyc_List_List*_tmp58;_LL3A: if((_tmp56.BottomFL).tag != 1)goto _LL3C;_LL3B:
 return Cyc_CfFlowInfo_BottomFL();_LL3C: if((_tmp56.ReachableFL).tag != 2)goto _LL39;_tmp57=((struct _tuple13)(_tmp56.ReachableFL).val).f1;_tmp58=((struct _tuple13)(_tmp56.ReachableFL).val).f2;_LL3D:
# 353
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp723;void*_tmp722;(_tmp722=0,Cyc_Tcutil_terr(loc,((_tmp723="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp723,sizeof(char),47))),_tag_dyneither(_tmp722,sizeof(void*),0)));}
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
const char*_tmp726;void*_tmp725;(_tmp725=0,Cyc_Tcutil_warn(loc,((_tmp726="argument may contain live unique pointers",_tag_dyneither(_tmp726,sizeof(char),42))),_tag_dyneither(_tmp725,sizeof(void*),0)));}
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
struct _tuple20 _tmp727;return(_tmp727.f1=inflow,((_tmp727.f2=0,_tmp727)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp67;void*_tmp68;struct _tuple14 _tmp66=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp72A;struct _tuple20 _tmp729;return(_tmp729.f1=_tmp67,((_tmp729.f2=((_tmp72A=_region_malloc(rgn,sizeof(*_tmp72A)),((_tmp72A->hd=_tmp68,((_tmp72A->tl=0,_tmp72A)))))),_tmp729)));};}{
# 390
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 398
{struct Cyc_Dict_Dict*_tmp72B;env->all_changed=((_tmp72B=_region_malloc(env->r,sizeof(*_tmp72B)),((_tmp72B[0]=(env->fenv)->mt_place_set,_tmp72B))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp6D;void*_tmp6E;struct _tuple14 _tmp6C=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
outflow=_tmp6D;
{struct Cyc_List_List*_tmp72C;rvals=((_tmp72C=_region_malloc(rgn,sizeof(*_tmp72C)),((_tmp72C->hd=_tmp6E,((_tmp72C->tl=0,_tmp72C))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 404
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp72D;env->all_changed=((_tmp72D=_region_malloc(env->r,sizeof(*_tmp72D)),((_tmp72D[0]=(env->fenv)->mt_place_set,_tmp72D))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp72;void*_tmp73;struct _tuple14 _tmp71=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;
{struct Cyc_List_List*_tmp72E;rvals=((_tmp72E=_region_malloc(rgn,sizeof(*_tmp72E)),((_tmp72E->hd=_tmp73,((_tmp72E->tl=rvals,_tmp72E))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp72,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
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
struct Cyc_Dict_Dict*_tmp72F;env->all_changed=((_tmp72F=_region_malloc(env->r,sizeof(*_tmp72F)),((_tmp72F[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp72F))));}
# 427
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple20 _tmp730;return(_tmp730.f1=outflow,((_tmp730.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp730)));};};}
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
const char*_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp733="expression may not be initialized",_tag_dyneither(_tmp733,sizeof(char),34))),_tag_dyneither(_tmp732,sizeof(void*),0)));}}
goto _LL45;_LL48: if((_tmp7B.BottomFL).tag != 1)goto _LL45;_LL49:
 goto _LL45;_LL45:;}{
# 447
struct _tuple14 _tmp734;struct _tuple14 _tmp80=(_tmp734.f1=_tmp79,((_tmp734.f2=(env->fenv)->unknown_all,_tmp734)));_npop_handler(0);return _tmp80;};}
# 437
;_pop_region(rgn);}
# 461 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 467
union Cyc_CfFlowInfo_FlowInfo _tmp81=outflow;struct Cyc_Dict_Dict _tmp82;struct Cyc_List_List*_tmp83;_LL4B: if((_tmp81.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return outflow;_LL4D: if((_tmp81.ReachableFL).tag != 2)goto _LL4A;_tmp82=((struct _tuple13)(_tmp81.ReachableFL).val).f1;_tmp83=((struct _tuple13)(_tmp81.ReachableFL).val).f2;_LL4E: {
# 470
union Cyc_CfFlowInfo_AbsLVal _tmp84=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: if((_tmp84.UnknownL).tag != 2)goto _LL52;_LL51:
# 474
 return outflow;_LL52: if((_tmp84.PlaceL).tag != 1)goto _LL4F;_tmp85=(struct Cyc_CfFlowInfo_Place*)(_tmp84.PlaceL).val;_LL53: {
# 478
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp737;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp736;nzval=(void*)((_tmp736=_region_malloc((env->fenv)->r,sizeof(*_tmp736)),((_tmp736[0]=((_tmp737.tag=8,((_tmp737.f1=locname,((_tmp737.f2=nzval,_tmp737)))))),_tmp736))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp88=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp82,env->all_changed,_tmp85,nzval),_tmp83);
# 485
return _tmp88;};}_LL4F:;}_LL4A:;}
# 494
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp89=outflow;struct Cyc_Dict_Dict _tmp8A;struct Cyc_List_List*_tmp8B;_LL55: if((_tmp89.BottomFL).tag != 1)goto _LL57;_LL56: {
struct _tuple16 _tmp738;return(_tmp738.f1=outflow,((_tmp738.f2=outflow,_tmp738)));}_LL57: if((_tmp89.ReachableFL).tag != 2)goto _LL54;_tmp8A=((struct _tuple13)(_tmp89.ReachableFL).val).f1;_tmp8B=((struct _tuple13)(_tmp89.ReachableFL).val).f2;_LL58: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp8D=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp8E;_LL5A: if((_tmp8D.UnknownL).tag != 2)goto _LL5C;_LL5B: {
struct _tuple16 _tmp739;return(_tmp739.f1=outflow,((_tmp739.f2=outflow,_tmp739)));}_LL5C: if((_tmp8D.PlaceL).tag != 1)goto _LL59;_tmp8E=(struct Cyc_CfFlowInfo_Place*)(_tmp8D.PlaceL).val;_LL5D: {
# 503
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp73C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp73B;nzval=(void*)((_tmp73B=_region_malloc((env->fenv)->r,sizeof(*_tmp73B)),((_tmp73B[0]=((_tmp73C.tag=8,((_tmp73C.f1=locname,((_tmp73C.f2=nzval,_tmp73C)))))),_tmp73B))));}{
struct _tuple16 _tmp73D;return
(_tmp73D.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,nzval),_tmp8B),((_tmp73D.f2=
# 509
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,(env->fenv)->zero),_tmp8B),_tmp73D)));};}_LL59:;}_LL54:;}
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
struct _tuple14 _tmp73E;return(_tmp73E.f1=f,((_tmp73E.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp73E)));}_LL66: if((_tmp9A.ReachableFL).tag != 2)goto _LL63;_tmp9B=((struct _tuple13)(_tmp9A.ReachableFL).val).f1;_tmp9C=((struct _tuple13)(_tmp9A.ReachableFL).val).f2;_LL67: {
# 532
struct Cyc_Absyn_Vardecl*locname=0;
{void*_tmp9E=r;struct Cyc_Absyn_Vardecl*_tmpA0;void*_tmpA1;_LL69: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9E;if(_tmp9F->tag != 8)goto _LL6B;else{_tmpA0=_tmp9F->f1;_tmpA1=(void*)_tmp9F->f2;}}_LL6A:
 locname=_tmpA0;goto _LL68;_LL6B:;_LL6C:
 goto _LL68;_LL68:;}
# 537
if(Cyc_Tcutil_is_bound_one(_tmp98)){
retry: {void*_tmpA2=r;struct Cyc_Absyn_Vardecl*_tmpA4;void*_tmpA5;struct Cyc_CfFlowInfo_Place*_tmpA9;enum Cyc_CfFlowInfo_InitLevel _tmpAC;_LL6E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA3->tag != 8)goto _LL70;else{_tmpA4=_tmpA3->f1;_tmpA5=(void*)_tmpA3->f2;}}_LL6F:
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp741;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp740;e->annot=(void*)((_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740[0]=((_tmp741.tag=Cyc_CfFlowInfo_NotZero,((_tmp741.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp741)))),_tmp740))));}
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp744;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp743;e->annot=(void*)((_tmp743=_cycalloc(sizeof(*_tmp743)),((_tmp743[0]=((_tmp744.tag=Cyc_CfFlowInfo_NotZero,((_tmp744.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp744)))),_tmp743))));}
goto _LL8B;_LL8E:;_LL8F:
# 571
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8B;_LL8B:;}
# 574
goto _LL86;_LL86:;}{
# 576
struct _tuple14 _tmp745;return(_tmp745.f1=f,((_tmp745.f2=Cyc_CfFlowInfo_lookup_place(_tmp9B,_tmpA9),_tmp745)));};_LL76: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpAA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAA->tag != 0)goto _LL78;}_LL77:
# 578
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp746;return(_tmp746.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp746.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp746)));};_LL78: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpAB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAB->tag != 3)goto _LL7A;else{_tmpAC=_tmpAB->f1;}}_LL79:
# 581
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpAC,locname);goto _LL7B;_LL7A:;_LL7B:
# 583
{void*_tmpBD=e->r;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD;if(_tmpBE->tag != 22)goto _LL93;}_LL92:
# 585
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp749;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp748;e->annot=(void*)((_tmp748=_cycalloc(sizeof(*_tmp748)),((_tmp748[0]=((_tmp749.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp749.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp749)))),_tmp748))));}
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
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp74C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp74B;e->annot=(void*)((_tmp74B=_cycalloc(sizeof(*_tmp74B)),((_tmp74B[0]=((_tmp74C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp74C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp74C)))),_tmp74B))));}
goto _LL95;_LL95:;}
# 603
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp9B,r)){case Cyc_CfFlowInfo_NoneIL: _LL9A:
# 605
{void*_tmpC6=r;_LL9D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC6;if(_tmpC7->tag != 7)goto _LL9F;}_LL9E:
# 608
{const char*_tmp74F;void*_tmp74E;(_tmp74E=0,Cyc_Tcutil_terr(e->loc,((_tmp74F="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp74F,sizeof(char),61))),_tag_dyneither(_tmp74E,sizeof(void*),0)));}
goto _LL9C;_LL9F:;_LLA0: {
# 611
const char*_tmp752;void*_tmp751;(_tmp751=0,Cyc_Tcutil_terr(e->loc,((_tmp752="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp752,sizeof(char),46))),_tag_dyneither(_tmp751,sizeof(void*),0)));}_LL9C:;}
# 613
goto _LL9B;case Cyc_CfFlowInfo_AllIL: _LL9B: {
# 615
void*_tmpCC=(env->fenv)->unknown_all;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp755;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp754;_tmpCC=(void*)((_tmp754=_region_malloc((env->fenv)->r,sizeof(*_tmp754)),((_tmp754[0]=((_tmp755.tag=8,((_tmp755.f1=locname,((_tmp755.f2=_tmpCC,_tmp755)))))),_tmp754))));}{
struct _tuple14 _tmp756;return(_tmp756.f1=f,((_tmp756.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpCC),_tmp756)));};}case Cyc_CfFlowInfo_ThisIL: _LLA1: {
# 620
void*_tmpD0=(env->fenv)->unknown_none;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp759;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp758;_tmpD0=(void*)((_tmp758=_region_malloc((env->fenv)->r,sizeof(*_tmp758)),((_tmp758[0]=((_tmp759.tag=8,((_tmp759.f1=locname,((_tmp759.f2=_tmpD0,_tmp759)))))),_tmp758))));}{
struct _tuple14 _tmp75A;return(_tmp75A.f1=f,((_tmp75A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpD0),_tmp75A)));};}}}_LL63:;}_LL61:;_LL62: {
# 626
struct Cyc_Core_Impossible_exn_struct _tmp760;const char*_tmp75F;struct Cyc_Core_Impossible_exn_struct*_tmp75E;(int)_throw((void*)((_tmp75E=_cycalloc(sizeof(*_tmp75E)),((_tmp75E[0]=((_tmp760.tag=Cyc_Core_Impossible,((_tmp760.f1=((_tmp75F="right deref of non-pointer-type",_tag_dyneither(_tmp75F,sizeof(char),32))),_tmp760)))),_tmp75E)))));}_LL5E:;}
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
struct Cyc_CfFlowInfo_Reln*_tmp763;struct Cyc_List_List*_tmp762;return(_tmp762=_region_malloc(rgn,sizeof(*_tmp762)),((_tmp762->hd=((_tmp763=_region_malloc(rgn,sizeof(*_tmp763)),((_tmp763->vd=_tmpF0,((_tmp763->rop=Cyc_CfFlowInfo_LessNumelts(_tmpE3),_tmp763)))))),((_tmp762->tl=relns,_tmp762)))));}}
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
struct _tuple14 _tmp764;return(_tmp764.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp764.f2=rval,_tmp764)));}_LLC1: if((_tmpF7.ReachableFL).tag != 2)goto _LLBE;_tmpF8=((struct _tuple13)(_tmpF7.ReachableFL).val).f1;_tmpF9=((struct _tuple13)(_tmpF7.ReachableFL).val).f2;_LLC2: {
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
{const char*_tmp767;void*_tmp766;(_tmp766=0,Cyc_Tcutil_warn(lexp->loc,((_tmp767="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp767,sizeof(char),43))),_tag_dyneither(_tmp766,sizeof(void*),0)));}
goto _LLC8;_LLC8:;}
# 703
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 705
struct _tuple14 _tmp768;return(_tmp768.f1=outflow,((_tmp768.f2=rval,_tmp768)));};}_LLC6: if((_tmpFB.UnknownL).tag != 2)goto _LLC3;_LLC7: {
# 708
struct _tuple14 _tmp769;return(_tmp769.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp769.f2=rval,_tmp769)));}_LLC3:;}_LLBE:;}
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
void*_tmp122=s->r;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Absyn_Tvar*_tmp128;struct Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Stmt*_tmp12A;struct Cyc_Absyn_Decl*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp123=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp123->tag != 2)goto _LLF2;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LLF1:
# 743
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp124);
s=_tmp125;
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp126=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp126->tag != 12)goto _LLF4;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp127=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp126->f1)->r;if(_tmp127->tag != 5)goto _LLF4;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;}}_tmp12A=_tmp126->f2;}}_LLF3: {
# 749
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp129->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp131;union Cyc_CfFlowInfo_AbsLVal _tmp132;struct _tuple15 _tmp130=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{
struct _tuple15 _tmp76A;struct _tuple15 _tmp134=(_tmp76A.f1=_tmp131,((_tmp76A.f2=_tmp132,_tmp76A)));struct Cyc_Dict_Dict _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_CfFlowInfo_Place*_tmp137;_LLFB: if(((_tmp134.f1).ReachableFL).tag != 2)goto _LLFD;_tmp135=((struct _tuple13)((_tmp134.f1).ReachableFL).val).f1;_tmp136=((struct _tuple13)((_tmp134.f1).ReachableFL).val).f2;if(((_tmp134.f2).PlaceL).tag != 1)goto _LLFD;_tmp137=(struct Cyc_CfFlowInfo_Place*)((_tmp134.f2).PlaceL).val;_LLFC: {
# 753
void*_tmp138=Cyc_CfFlowInfo_lookup_place(_tmp135,_tmp137);
# 755
void*t=(void*)_check_null(e->topt);
# 762
void*_tmp139=Cyc_CfFlowInfo_make_unique_consumed(_tmp11F,t,e,env->iteration_num,_tmp138);
_tmp135=Cyc_CfFlowInfo_assign_place(_tmp11F,e->loc,_tmp135,env->all_changed,_tmp137,_tmp139);
_tmp131=Cyc_CfFlowInfo_ReachableFL(_tmp135,_tmp136);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp11F,_tmp135,_tmp138)){case Cyc_CfFlowInfo_AllIL: _LL101:
 leaf=_tmp11F->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL102:
 leaf=_tmp11F->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL103:
 leaf=_tmp11F->unknown_this;break;}{
# 773
void*_tmp13A=Cyc_CfFlowInfo_typ_to_absrval(_tmp11F,t,0,leaf);
_tmp131=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp131,_tmp138);{
struct Cyc_List_List _tmp76B;struct Cyc_List_List _tmp13B=(_tmp76B.hd=_tmp129,((_tmp76B.tl=0,_tmp76B)));
_tmp131=Cyc_NewControlFlow_add_vars(_tmp11F,_tmp131,& _tmp13B,_tmp11F->unknown_all,s->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp13D;void*_tmp13E;struct _tuple14 _tmp13C=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp131,_tmp12A);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp13F=_tmp13D;struct Cyc_Dict_Dict _tmp140;struct Cyc_List_List*_tmp141;_LL106: if((_tmp13F.ReachableFL).tag != 2)goto _LL108;_tmp140=((struct _tuple13)(_tmp13F.ReachableFL).val).f1;_tmp141=((struct _tuple13)(_tmp13F.ReachableFL).val).f2;_LL107:
# 780
 _tmp140=Cyc_CfFlowInfo_assign_place(_tmp11F,s->loc,_tmp140,env->all_changed,_tmp137,_tmp13A);{
union Cyc_CfFlowInfo_FlowInfo _tmp142=Cyc_CfFlowInfo_ReachableFL(_tmp140,_tmp141);
struct _tuple14 _tmp76C;return(_tmp76C.f1=_tmp142,((_tmp76C.f2=_tmp13E,_tmp76C)));};_LL108:;_LL109: {
struct _tuple14 _tmp76D;return(_tmp76D.f1=_tmp13D,((_tmp76D.f2=_tmp13E,_tmp76D)));}_LL105:;};};};};};}_LLFD: if(((_tmp134.f1).BottomFL).tag != 1)goto _LLFF;_LLFE: {
# 785
struct _tuple14 _tmp76E;return(_tmp76E.f1=_tmp131,((_tmp76E.f2=_tmp11F->unknown_all,_tmp76E)));}_LLFF:;_LL100:
{const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(e->loc,((_tmp771="bad alias expression--no unique path found",_tag_dyneither(_tmp771,sizeof(char),43))),_tag_dyneither(_tmp770,sizeof(void*),0)));}{
struct _tuple14 _tmp772;return(_tmp772.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp772.f2=_tmp11F->unknown_all,_tmp772)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12B->tag != 12)goto _LLF6;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LLF5:
# 790
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp12C);
s=_tmp12D;
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp12E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp122;if(_tmp12E->tag != 1)goto _LLF8;else{_tmp12F=_tmp12E->f1;}}_LLF7:
# 794
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp12F);_LLF8:;_LLF9: {
struct Cyc_Core_Impossible_exn_struct _tmp778;const char*_tmp777;struct Cyc_Core_Impossible_exn_struct*_tmp776;(int)_throw((void*)((_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776[0]=((_tmp778.tag=Cyc_Core_Impossible,((_tmp778.f1=((_tmp777="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp777,sizeof(char),33))),_tmp778)))),_tmp776)))));}_LLEF:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp81A(unsigned int*_tmp819,unsigned int*_tmp818,void***_tmp816,struct Cyc_List_List**_tmp325){for(*_tmp819=0;*_tmp819 < *_tmp818;(*_tmp819)++){
# 1353 "new_control_flow.cyc"
void*_tmp814;(*_tmp816)[*_tmp819]=(void*)((_tmp814=(void*)((struct Cyc_List_List*)_check_null(*_tmp325))->hd,((*_tmp325=(*_tmp325)->tl,_tmp814))));}}
# 802 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 806
struct Cyc_CfFlowInfo_FlowEnv*_tmp14D=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 812
{union Cyc_CfFlowInfo_FlowInfo _tmp14E=inflow;struct Cyc_Dict_Dict _tmp14F;struct Cyc_List_List*_tmp150;_LL10B: if((_tmp14E.BottomFL).tag != 1)goto _LL10D;_LL10C: {
struct _tuple14 _tmp779;return(_tmp779.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp779.f2=_tmp14D->unknown_all,_tmp779)));}_LL10D: if((_tmp14E.ReachableFL).tag != 2)goto _LL10A;_tmp14F=((struct _tuple13)(_tmp14E.ReachableFL).val).f1;_tmp150=((struct _tuple13)(_tmp14E.ReachableFL).val).f2;_LL10E:
 d=_tmp14F;relns=_tmp150;_LL10A:;}
# 821
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 842 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp153;union Cyc_CfFlowInfo_AbsLVal _tmp154;struct _tuple15 _tmp152=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{
struct _tuple15 _tmp77A;struct _tuple15 _tmp156=(_tmp77A.f1=_tmp153,((_tmp77A.f2=_tmp154,_tmp77A)));struct Cyc_Dict_Dict _tmp157;struct Cyc_List_List*_tmp158;struct Cyc_CfFlowInfo_Place*_tmp159;_LL110: if(((_tmp156.f1).ReachableFL).tag != 2)goto _LL112;_tmp157=((struct _tuple13)((_tmp156.f1).ReachableFL).val).f1;_tmp158=((struct _tuple13)((_tmp156.f1).ReachableFL).val).f2;if(((_tmp156.f2).PlaceL).tag != 1)goto _LL112;_tmp159=(struct Cyc_CfFlowInfo_Place*)((_tmp156.f2).PlaceL).val;_LL111: {
# 845
void*_tmp15A=Cyc_CfFlowInfo_lookup_place(_tmp157,_tmp159);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp15A,& needs_unconsume)){
{const char*_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(e->loc,((_tmp77D="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp77D,sizeof(char),71))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}{
struct _tuple14 _tmp77E;return(_tmp77E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp77E.f2=_tmp14D->unknown_all,_tmp77E)));};}else{
# 851
if(needs_unconsume){
# 853
struct _tuple14 _tmp77F;return(_tmp77F.f1=_tmp153,((_tmp77F.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14D,_tmp15A),_tmp77F)));}else{
# 856
void*_tmp15F=Cyc_CfFlowInfo_make_unique_consumed(_tmp14D,(void*)_check_null(e->topt),e,env->iteration_num,_tmp15A);
struct Cyc_Dict_Dict _tmp160=Cyc_CfFlowInfo_assign_place(_tmp14D,e->loc,_tmp157,env->all_changed,_tmp159,_tmp15F);
# 867
struct _tuple14 _tmp780;return(_tmp780.f1=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp158),((_tmp780.f2=_tmp15A,_tmp780)));}}}_LL112:;_LL113:
# 869
 goto _LL10F;_LL10F:;};}{
# 872
void*_tmp162=e->r;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17E;struct Cyc_Absyn_Vardecl*_tmp181;enum Cyc_Absyn_Primop _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_List_List*_tmp194;int _tmp196;struct Cyc_Absyn_Exp*_tmp197;void**_tmp198;struct Cyc_Absyn_Exp*_tmp199;int _tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct _dyneither_ptr*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;struct _dyneither_ptr*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;struct _dyneither_ptr*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Datatypedecl*_tmp1BD;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C3;enum Cyc_Absyn_AggrKind _tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;int _tmp1CD;struct Cyc_Absyn_Exp*_tmp1CF;void*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_Stmt*_tmp1D3;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp163->tag != 13)goto _LL117;else{_tmp164=_tmp163->f2;if(_tmp163->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL117;}}_LL116: {
# 876
union Cyc_CfFlowInfo_FlowInfo _tmp1DB;void*_tmp1DC;struct _tuple14 _tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp164);_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1DE;void*_tmp1DF;struct _tuple14 _tmp1DD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1DB,_tmp164,_tmp1DC);_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;{
struct _tuple14 _tmp781;return(_tmp781.f1=_tmp1DE,((_tmp781.f2=_tmp1DC,_tmp781)));};};}_LL117: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp165->tag != 13)goto _LL119;else{_tmp166=_tmp165->f2;}}_LL118:
# 882
 _tmp168=_tmp166;goto _LL11A;_LL119: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp167->tag != 11)goto _LL11B;else{_tmp168=_tmp167->f1;}}_LL11A:
 _tmp16A=_tmp168;goto _LL11C;_LL11B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp169->tag != 12)goto _LL11D;else{_tmp16A=_tmp169->f1;}}_LL11C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp16A);_LL11D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16B->tag != 0)goto _LL11F;else{if(((_tmp16B->f1).Null_c).tag != 1)goto _LL11F;}}_LL11E:
# 886
 goto _LL120;_LL11F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16C->tag != 0)goto _LL121;else{if(((_tmp16C->f1).Int_c).tag != 5)goto _LL121;if(((struct _tuple6)((_tmp16C->f1).Int_c).val).f2 != 0)goto _LL121;}}_LL120: {
struct _tuple14 _tmp782;return(_tmp782.f1=inflow,((_tmp782.f2=_tmp14D->zero,_tmp782)));}_LL121: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16D->tag != 0)goto _LL123;else{if(((_tmp16D->f1).Int_c).tag != 5)goto _LL123;}}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp16E->tag != 1)goto _LL125;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 2)goto _LL125;}}_LL124: {
struct _tuple14 _tmp783;return(_tmp783.f1=inflow,((_tmp783.f2=_tmp14D->notzeroall,_tmp783)));}_LL125: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp170->tag != 30)goto _LL127;else{if(_tmp170->f1 != 0)goto _LL127;}}_LL126:
# 891
 goto _LL128;_LL127: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp171->tag != 0)goto _LL129;}_LL128:
 goto _LL12A;_LL129: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp172->tag != 17)goto _LL12B;}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp173->tag != 16)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp174->tag != 18)goto _LL12F;}_LL12E:
 goto _LL130;_LL12F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp175->tag != 32)goto _LL131;}_LL130:
 goto _LL132;_LL131: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp176->tag != 31)goto _LL133;}_LL132: {
struct _tuple14 _tmp784;return(_tmp784.f1=inflow,((_tmp784.f2=_tmp14D->unknown_all,_tmp784)));}_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp177->tag != 1)goto _LL135;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp178=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp177->f2);if(_tmp178->tag != 1)goto _LL135;}}_LL134: {
# 900
struct _tuple14 _tmp785;return(_tmp785.f1=inflow,((_tmp785.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp785)));}_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp179->tag != 1)goto _LL137;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp17A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp179->f2);if(_tmp17A->tag != 3)goto _LL137;else{_tmp17B=_tmp17A->f1;}}}_LL136:
# 903
 _tmp17E=_tmp17B;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp17C->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17C->f2);if(_tmp17D->tag != 4)goto _LL139;else{_tmp17E=_tmp17D->f1;}}}_LL138:
 _tmp181=_tmp17E;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp17F->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp180=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp17F->f2);if(_tmp180->tag != 5)goto _LL13B;else{_tmp181=_tmp180->f1;}}}_LL13A: {
# 907
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp786;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp786.tag=0,((_tmp786.f1=_tmp181,_tmp786)));
struct _tuple14 _tmp787;return(_tmp787.f1=inflow,((_tmp787.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp787)));}_LL13B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp182->tag != 2)goto _LL13D;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL13C: {
# 914
union Cyc_CfFlowInfo_FlowInfo _tmp1E8;void*_tmp1E9;struct _tuple14 _tmp1E7=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp184);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;{
struct _tuple14 _tmp788;return(_tmp788.f1=_tmp1E8,((_tmp788.f2=_tmp1E9,_tmp788)));};}_LL13D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp185->tag != 4)goto _LL13F;else{_tmp186=_tmp185->f1;}}_LL13E: {
# 918
struct Cyc_List_List _tmp789;struct Cyc_List_List _tmp1EB=(_tmp789.hd=_tmp186,((_tmp789.tl=0,_tmp789)));
union Cyc_CfFlowInfo_FlowInfo _tmp1ED;struct _tuple14 _tmp1EC=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1EB);_tmp1ED=_tmp1EC.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1EF;struct _tuple15 _tmp1EE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1ED,0,_tmp186);_tmp1EF=_tmp1EE.f2;
{struct _tuple21 _tmp78A;struct _tuple21 _tmp1F1=(_tmp78A.f1=_tmp1EF,((_tmp78A.f2=_tmp1ED,_tmp78A)));struct Cyc_CfFlowInfo_Place*_tmp1F2;struct Cyc_Dict_Dict _tmp1F3;struct Cyc_List_List*_tmp1F4;_LL17E: if(((_tmp1F1.f1).PlaceL).tag != 1)goto _LL180;_tmp1F2=(struct Cyc_CfFlowInfo_Place*)((_tmp1F1.f1).PlaceL).val;if(((_tmp1F1.f2).ReachableFL).tag != 2)goto _LL180;_tmp1F3=((struct _tuple13)((_tmp1F1.f2).ReachableFL).val).f1;_tmp1F4=((struct _tuple13)((_tmp1F1.f2).ReachableFL).val).f2;_LL17F:
# 923
 _tmp1F4=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp1F4,_tmp186);
_tmp1ED=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp186->loc,_tmp1F3,env->all_changed,_tmp1F2,_tmp14D->unknown_all),_tmp1F4);
# 927
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}{
# 930
struct _tuple14 _tmp78B;return(_tmp78B.f1=_tmp1ED,((_tmp78B.f2=_tmp14D->unknown_all,_tmp78B)));};};}_LL13F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp187->tag != 3)goto _LL141;else{_tmp188=_tmp187->f1;if(_tmp187->f2 == 0)goto _LL141;_tmp189=_tmp187->f3;}}_LL140:
# 933
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(e->loc,((_tmp78E="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp78E,sizeof(char),58))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}{
struct _tuple14 _tmp78F;return(_tmp78F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp78F.f2=_tmp14D->unknown_all,_tmp78F)));};}{
# 937
struct Cyc_List_List _tmp790;struct Cyc_List_List _tmp1FA=(_tmp790.hd=_tmp189,((_tmp790.tl=0,_tmp790)));
struct Cyc_List_List _tmp791;struct Cyc_List_List _tmp1FB=(_tmp791.hd=_tmp188,((_tmp791.tl=& _tmp1FA,_tmp791)));
union Cyc_CfFlowInfo_FlowInfo _tmp1FD;struct _tuple14 _tmp1FC=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1FB);_tmp1FD=_tmp1FC.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1FF;struct _tuple15 _tmp1FE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1FD,0,_tmp188);_tmp1FF=_tmp1FE.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp200=_tmp1FD;struct Cyc_Dict_Dict _tmp201;struct Cyc_List_List*_tmp202;_LL183: if((_tmp200.ReachableFL).tag != 2)goto _LL185;_tmp201=((struct _tuple13)(_tmp200.ReachableFL).val).f1;_tmp202=((struct _tuple13)(_tmp200.ReachableFL).val).f2;_LL184:
# 943
{union Cyc_CfFlowInfo_AbsLVal _tmp203=_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp204;_LL188: if((_tmp203.PlaceL).tag != 1)goto _LL18A;_tmp204=(struct Cyc_CfFlowInfo_Place*)(_tmp203.PlaceL).val;_LL189:
# 947
 _tmp202=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp202,_tmp188);
_tmp201=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp188->loc,_tmp201,env->all_changed,_tmp204,_tmp14D->unknown_all);
# 950
_tmp1FD=Cyc_CfFlowInfo_ReachableFL(_tmp201,_tmp202);
# 953
goto _LL187;_LL18A: if((_tmp203.UnknownL).tag != 2)goto _LL187;_LL18B:
# 956
 goto _LL187;_LL187:;}
# 958
goto _LL182;_LL185:;_LL186:
 goto _LL182;_LL182:;}{
# 961
struct _tuple14 _tmp792;return(_tmp792.f1=_tmp1FD,((_tmp792.f2=_tmp14D->unknown_all,_tmp792)));};};};_LL141: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp18A->tag != 3)goto _LL143;else{_tmp18B=_tmp18A->f1;if(_tmp18A->f2 != 0)goto _LL143;_tmp18C=_tmp18A->f3;}}_LL142:
# 965
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_Tcutil_terr(e->loc,((_tmp795="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp795,sizeof(char),58))),_tag_dyneither(_tmp794,sizeof(void*),0)));}{
struct _tuple14 _tmp796;return(_tmp796.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp796.f2=_tmp14D->unknown_all,_tmp796)));};}{
# 969
struct Cyc_Dict_Dict*_tmp20B=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp797;env->all_changed=((_tmp797=_region_malloc(env->r,sizeof(*_tmp797)),((_tmp797[0]=_tmp14D->mt_place_set,_tmp797))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp20E;union Cyc_CfFlowInfo_AbsLVal _tmp20F;struct _tuple15 _tmp20D=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp18B);_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;{
struct Cyc_Dict_Dict _tmp210=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp798;env->all_changed=((_tmp798=_region_malloc(env->r,sizeof(*_tmp798)),((_tmp798[0]=_tmp14D->mt_place_set,_tmp798))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp213;void*_tmp214;struct _tuple14 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18C);_tmp213=_tmp212.f1;_tmp214=_tmp212.f2;{
struct Cyc_Dict_Dict _tmp215=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp216=Cyc_CfFlowInfo_after_flow(_tmp14D,& _tmp210,_tmp20E,_tmp213,_tmp210,_tmp215);
# 980
union Cyc_CfFlowInfo_FlowInfo _tmp217=Cyc_CfFlowInfo_join_flow(_tmp14D,_tmp20B,inflow,_tmp216);
# 983
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp217,inflow)){
if(_tmp20B == 0)
env->all_changed=0;else{
# 987
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp20B,
Cyc_CfFlowInfo_union_place_set(_tmp210,_tmp215,0),0);}
# 990
return Cyc_NewControlFlow_do_assign(_tmp14D,env,_tmp216,_tmp18B,_tmp20F,_tmp18C,_tmp214,e->loc);}
# 993
inflow=_tmp217;};};};};}};_LL143: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp18D->tag != 8)goto _LL145;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LL144: {
# 998
union Cyc_CfFlowInfo_FlowInfo _tmp219;void*_tmp21A;struct _tuple14 _tmp218=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp18E);_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp219,_tmp18F);}_LL145: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp190->tag != 10)goto _LL147;else{_tmp191=_tmp190->f1;}}_LL146: {
# 1002
union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*_tmp21D;struct _tuple14 _tmp21B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp191);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp191->loc,_tmp21C,_tmp21D);{
struct _tuple14 _tmp799;return(_tmp799.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp799.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp799)));};}_LL147: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp192->tag != 9)goto _LL149;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LL148: {
# 1007
struct _RegionHandle _tmp21F=_new_region("temp");struct _RegionHandle*temp=& _tmp21F;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp222;struct Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp79A;struct _tuple20 _tmp221=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp79A=_region_malloc(temp,sizeof(*_tmp79A)),((_tmp79A->hd=_tmp193,((_tmp79A->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp194),_tmp79A)))))),0,1);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;{
# 1010
union Cyc_CfFlowInfo_FlowInfo _tmp224=Cyc_NewControlFlow_use_Rval(env,_tmp193->loc,_tmp222,(void*)((struct Cyc_List_List*)_check_null(_tmp223))->hd);
_tmp223=_tmp223->tl;{
# 1013
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp225=Cyc_Tcutil_compress((void*)_check_null(_tmp193->topt));void*_tmp227;_LL18D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225;if(_tmp226->tag != 5)goto _LL18F;else{_tmp227=(_tmp226->f1).elt_typ;}}_LL18E:
# 1018
{void*_tmp228=Cyc_Tcutil_compress(_tmp227);struct Cyc_List_List*_tmp22A;_LL192: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228;if(_tmp229->tag != 9)goto _LL194;else{_tmp22A=(_tmp229->f1).attributes;}}_LL193:
# 1020
 for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){
# 1022
void*_tmp22B=(void*)_tmp22A->hd;int _tmp22D;int _tmp22F;int _tmp231;_LL197: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp22C=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp22C->tag != 20)goto _LL199;else{_tmp22D=_tmp22C->f1;}}_LL198:
# 1024
{struct Cyc_List_List*_tmp79B;init_params=((_tmp79B=_region_malloc(temp,sizeof(*_tmp79B)),((_tmp79B->hd=(void*)_tmp22D,((_tmp79B->tl=init_params,_tmp79B))))));}goto _LL196;_LL199: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp22E=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp22E->tag != 21)goto _LL19B;else{_tmp22F=_tmp22E->f1;}}_LL19A:
# 1026
{struct Cyc_List_List*_tmp79C;nolive_unique_params=((_tmp79C=_region_malloc(temp,sizeof(*_tmp79C)),((_tmp79C->hd=(void*)_tmp22F,((_tmp79C->tl=nolive_unique_params,_tmp79C))))));}
goto _LL196;_LL19B: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp230=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp22B;if(_tmp230->tag != 22)goto _LL19D;else{_tmp231=_tmp230->f1;}}_LL19C:
# 1030
{struct Cyc_List_List*_tmp79D;noconsume_params=((_tmp79D=_region_malloc(temp,sizeof(*_tmp79D)),((_tmp79D->hd=(void*)_tmp231,((_tmp79D->tl=noconsume_params,_tmp79D))))));}
goto _LL196;_LL19D:;_LL19E:
 goto _LL196;_LL196:;}
# 1034
goto _LL191;_LL194:;_LL195: {
const char*_tmp7A0;void*_tmp79F;(_tmp79F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A0="anal_Rexp: bad function type",_tag_dyneither(_tmp7A0,sizeof(char),29))),_tag_dyneither(_tmp79F,sizeof(void*),0)));}_LL191:;}
# 1037
goto _LL18C;_LL18F:;_LL190: {
const char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A3="anal_Rexp: bad function type",_tag_dyneither(_tmp7A3,sizeof(char),29))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}_LL18C:;}
# 1040
{int i=1;for(0;_tmp223 != 0;(((_tmp223=_tmp223->tl,_tmp194=((struct Cyc_List_List*)_check_null(_tmp194))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp239=_tmp222;struct Cyc_Dict_Dict _tmp23A;_LL1A0: if((_tmp239.BottomFL).tag != 1)goto _LL1A2;_LL1A1:
 goto _LL19F;_LL1A2: if((_tmp239.ReachableFL).tag != 2)goto _LL19F;_tmp23A=((struct _tuple13)(_tmp239.ReachableFL).val).f1;_LL1A3:
# 1045
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp23A,(void*)_tmp223->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,((_tmp7A6="expression may not be initialized",_tag_dyneither(_tmp7A6,sizeof(char),34))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp23D=_tmp224;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23F;_LL1A5: if((_tmp23D.BottomFL).tag != 1)goto _LL1A7;_LL1A6:
 goto _LL1A4;_LL1A7: if((_tmp23D.ReachableFL).tag != 2)goto _LL1A4;_tmp23E=((struct _tuple13)(_tmp23D.ReachableFL).val).f1;_tmp23F=((struct _tuple13)(_tmp23D.ReachableFL).val).f2;_LL1A8: {
# 1052
struct Cyc_Dict_Dict _tmp240=Cyc_CfFlowInfo_escape_deref(_tmp14D,_tmp23E,env->all_changed,(void*)_tmp223->hd);
{void*_tmp241=(void*)_tmp223->hd;struct Cyc_CfFlowInfo_Place*_tmp243;_LL1AA: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp242=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241;if(_tmp242->tag != 5)goto _LL1AC;else{_tmp243=_tmp242->f1;}}_LL1AB:
# 1055
{void*_tmp244=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->topt));void*_tmp246;_LL1AF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244;if(_tmp245->tag != 5)goto _LL1B1;else{_tmp246=(_tmp245->f1).elt_typ;}}_LL1B0:
# 1059
 _tmp240=Cyc_CfFlowInfo_assign_place(_tmp14D,((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,_tmp240,env->all_changed,_tmp243,
# 1061
Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,_tmp246,0,_tmp14D->esc_all));
# 1063
goto _LL1AE;_LL1B1:;_LL1B2: {
const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A9="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp7A9,sizeof(char),39))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}_LL1AE:;}
# 1066
goto _LL1A9;_LL1AC:;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1069
_tmp224=Cyc_CfFlowInfo_ReachableFL(_tmp240,_tmp23F);
goto _LL1A4;}_LL1A4:;}
# 1072
goto _LL19F;_LL19F:;}else{
# 1075
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1078
_tmp224=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->topt),_tmp224,(void*)_tmp223->hd);else{
# 1085
_tmp224=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp194))->hd)->loc,_tmp224,(void*)_tmp223->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1088
union Cyc_CfFlowInfo_AbsLVal _tmp24A;struct _tuple15 _tmp249=Cyc_NewControlFlow_anal_Lexp(env,_tmp224,1,(struct Cyc_Absyn_Exp*)_tmp194->hd);_tmp24A=_tmp249.f2;{
struct _tuple15 _tmp7AA;struct _tuple15 _tmp24C=(_tmp7AA.f1=_tmp224,((_tmp7AA.f2=_tmp24A,_tmp7AA)));struct Cyc_Dict_Dict _tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_CfFlowInfo_Place*_tmp24F;_LL1B4: if(((_tmp24C.f1).ReachableFL).tag != 2)goto _LL1B6;_tmp24D=((struct _tuple13)((_tmp24C.f1).ReachableFL).val).f1;_tmp24E=((struct _tuple13)((_tmp24C.f1).ReachableFL).val).f2;if(((_tmp24C.f2).PlaceL).tag != 1)goto _LL1B6;_tmp24F=(struct Cyc_CfFlowInfo_Place*)((_tmp24C.f2).PlaceL).val;_LL1B5: {
# 1091
void*_tmp250=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14D,(void*)_tmp223->hd);
# 1094
_tmp24D=Cyc_CfFlowInfo_assign_place(_tmp14D,((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,_tmp24D,env->all_changed,_tmp24F,_tmp250);
_tmp224=Cyc_CfFlowInfo_ReachableFL(_tmp24D,_tmp24E);
# 1099
goto _LL1B3;}_LL1B6: if(((_tmp24C.f1).BottomFL).tag != 1)goto _LL1B8;_LL1B7:
 continue;_LL1B8:;_LL1B9: {
# 1102
const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp194->hd)->loc,((_tmp7AD="noconsume() parameters must be l-values",_tag_dyneither(_tmp7AD,sizeof(char),40))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}_LL1B3:;};}}}}}
# 1109
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp193->topt))){
struct _tuple14 _tmp7AE;struct _tuple14 _tmp254=(_tmp7AE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7AE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7AE)));_npop_handler(0);return _tmp254;}else{
# 1112
struct _tuple14 _tmp7AF;struct _tuple14 _tmp256=(_tmp7AF.f1=_tmp224,((_tmp7AF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7AF)));_npop_handler(0);return _tmp256;}};};}
# 1008
;_pop_region(temp);}_LL149: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp195->tag != 33)goto _LL14B;else{_tmp196=(_tmp195->f1).is_calloc;_tmp197=(_tmp195->f1).rgn;_tmp198=(_tmp195->f1).elt_type;_tmp199=(_tmp195->f1).num_elts;_tmp19A=(_tmp195->f1).fat_result;}}_LL14A: {
# 1115
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7B2;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7B1;void*root=(void*)((_tmp7B1=_region_malloc(_tmp14D->r,sizeof(*_tmp7B1)),((_tmp7B1[0]=((_tmp7B2.tag=1,((_tmp7B2.f1=_tmp199,((_tmp7B2.f2=(void*)_check_null(e->topt),_tmp7B2)))))),_tmp7B1))));
struct Cyc_CfFlowInfo_Place*_tmp7B3;struct Cyc_CfFlowInfo_Place*place=(_tmp7B3=_region_malloc(_tmp14D->r,sizeof(*_tmp7B3)),((_tmp7B3->root=root,((_tmp7B3->fields=0,_tmp7B3)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7B6;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7B5;void*rval=(void*)((_tmp7B5=_region_malloc(_tmp14D->r,sizeof(*_tmp7B5)),((_tmp7B5[0]=((_tmp7B6.tag=5,((_tmp7B6.f1=place,_tmp7B6)))),_tmp7B5))));
void*place_val;
if(_tmp19A)place_val=_tmp14D->notzeroall;else{
if(_tmp196)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,*((void**)_check_null(_tmp198)),0,_tmp14D->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,*((void**)_check_null(_tmp198)),0,_tmp14D->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp197 != 0){
struct _RegionHandle _tmp257=_new_region("temp");struct _RegionHandle*temp=& _tmp257;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_Absyn_Exp*_tmp7B7[2];struct _tuple20 _tmp259=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7B7[1]=_tmp199,((_tmp7B7[0]=_tmp197,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7B7,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;
outflow=_tmp25A;}
# 1126
;_pop_region(temp);}else{
# 1131
union Cyc_CfFlowInfo_FlowInfo _tmp25D;struct _tuple14 _tmp25C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp199);_tmp25D=_tmp25C.f1;
outflow=_tmp25D;}{
# 1134
union Cyc_CfFlowInfo_FlowInfo _tmp25E=outflow;struct Cyc_Dict_Dict _tmp25F;struct Cyc_List_List*_tmp260;_LL1BB: if((_tmp25E.BottomFL).tag != 1)goto _LL1BD;_LL1BC: {
struct _tuple14 _tmp7B8;return(_tmp7B8.f1=outflow,((_tmp7B8.f2=rval,_tmp7B8)));}_LL1BD: if((_tmp25E.ReachableFL).tag != 2)goto _LL1BA;_tmp25F=((struct _tuple13)(_tmp25E.ReachableFL).val).f1;_tmp260=((struct _tuple13)(_tmp25E.ReachableFL).val).f2;_LL1BE: {
# 1137
struct _tuple14 _tmp7B9;return(_tmp7B9.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25F,root,place_val),_tmp260),((_tmp7B9.f2=rval,_tmp7B9)));}_LL1BA:;};};}_LL14B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp19B->tag != 34)goto _LL14D;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL14C: {
# 1141
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1147
struct _RegionHandle _tmp268=_new_region("temp");struct _RegionHandle*temp=& _tmp268;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*_tmp7BA[2];struct _tuple20 _tmp26A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7BA[1]=_tmp19D,((_tmp7BA[0]=_tmp19C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7BA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26C->tl))->hd;
outflow=_tmp26B;}{
# 1155
union Cyc_CfFlowInfo_AbsLVal _tmp26E;struct _tuple15 _tmp26D=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19C);_tmp26E=_tmp26D.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp270;struct _tuple15 _tmp26F=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19D);_tmp270=_tmp26F.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp271=outflow;struct Cyc_Dict_Dict _tmp272;struct Cyc_List_List*_tmp273;_LL1C0: if((_tmp271.ReachableFL).tag != 2)goto _LL1C2;_tmp272=((struct _tuple13)(_tmp271.ReachableFL).val).f1;_tmp273=((struct _tuple13)(_tmp271.ReachableFL).val).f2;_LL1C1:
# 1159
{union Cyc_CfFlowInfo_AbsLVal _tmp274=_tmp26E;struct Cyc_CfFlowInfo_Place*_tmp275;_LL1C5: if((_tmp274.PlaceL).tag != 1)goto _LL1C7;_tmp275=(struct Cyc_CfFlowInfo_Place*)(_tmp274.PlaceL).val;_LL1C6:
# 1161
 _tmp272=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp19C->loc,_tmp272,env->all_changed,_tmp275,right_rval);
# 1163
goto _LL1C4;_LL1C7: if((_tmp274.UnknownL).tag != 2)goto _LL1C4;_LL1C8:
 goto _LL1C4;_LL1C4:;}
# 1166
{union Cyc_CfFlowInfo_AbsLVal _tmp276=_tmp270;struct Cyc_CfFlowInfo_Place*_tmp277;_LL1CA: if((_tmp276.PlaceL).tag != 1)goto _LL1CC;_tmp277=(struct Cyc_CfFlowInfo_Place*)(_tmp276.PlaceL).val;_LL1CB:
# 1168
 _tmp272=Cyc_CfFlowInfo_assign_place(_tmp14D,_tmp19D->loc,_tmp272,env->all_changed,_tmp277,left_rval);
# 1170
goto _LL1C9;_LL1CC: if((_tmp276.UnknownL).tag != 2)goto _LL1C9;_LL1CD:
 goto _LL1C9;_LL1C9:;}
# 1174
_tmp273=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp273,_tmp19C);
_tmp273=Cyc_CfFlowInfo_reln_kill_exp(_tmp14D->r,_tmp273,_tmp19D);
# 1177
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp272,_tmp273);
goto _LL1BF;_LL1C2:;_LL1C3:
 goto _LL1BF;_LL1BF:;}{
# 1183
struct _tuple14 _tmp7BB;struct _tuple14 _tmp279=(_tmp7BB.f1=outflow,((_tmp7BB.f2=_tmp14D->unknown_all,_tmp7BB)));_npop_handler(0);return _tmp279;};};};
# 1147
;_pop_region(temp);}_LL14D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp19E->tag != 15)goto _LL14F;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;}}_LL14E: {
# 1186
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7BE;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7BD;void*root=(void*)((_tmp7BD=_region_malloc(_tmp14D->r,sizeof(*_tmp7BD)),((_tmp7BD[0]=((_tmp7BE.tag=1,((_tmp7BE.f1=_tmp1A0,((_tmp7BE.f2=(void*)_check_null(e->topt),_tmp7BE)))))),_tmp7BD))));
struct Cyc_CfFlowInfo_Place*_tmp7BF;struct Cyc_CfFlowInfo_Place*place=(_tmp7BF=_region_malloc(_tmp14D->r,sizeof(*_tmp7BF)),((_tmp7BF->root=root,((_tmp7BF->fields=0,_tmp7BF)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7C2;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7C1;void*rval=(void*)((_tmp7C1=_region_malloc(_tmp14D->r,sizeof(*_tmp7C1)),((_tmp7C1[0]=((_tmp7C2.tag=5,((_tmp7C2.f1=place,_tmp7C2)))),_tmp7C1))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp19F != 0){
struct _RegionHandle _tmp27A=_new_region("temp");struct _RegionHandle*temp=& _tmp27A;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_Absyn_Exp*_tmp7C3[2];struct _tuple20 _tmp27C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7C3[1]=_tmp1A0,((_tmp7C3[0]=_tmp19F,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7C3,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;
outflow=_tmp27D;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp27E))->tl))->hd;}
# 1194
;_pop_region(temp);}else{
# 1200
union Cyc_CfFlowInfo_FlowInfo _tmp280;void*_tmp281;struct _tuple14 _tmp27F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A0);_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;
outflow=_tmp280;
place_val=_tmp281;}{
# 1204
union Cyc_CfFlowInfo_FlowInfo _tmp282=outflow;struct Cyc_Dict_Dict _tmp283;struct Cyc_List_List*_tmp284;_LL1CF: if((_tmp282.BottomFL).tag != 1)goto _LL1D1;_LL1D0: {
struct _tuple14 _tmp7C4;return(_tmp7C4.f1=outflow,((_tmp7C4.f2=rval,_tmp7C4)));}_LL1D1: if((_tmp282.ReachableFL).tag != 2)goto _LL1CE;_tmp283=((struct _tuple13)(_tmp282.ReachableFL).val).f1;_tmp284=((struct _tuple13)(_tmp282.ReachableFL).val).f2;_LL1D2: {
# 1207
struct _tuple14 _tmp7C5;return(_tmp7C5.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp283,root,place_val),_tmp284),((_tmp7C5.f2=rval,_tmp7C5)));}_LL1CE:;};};}_LL14F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A1->tag != 14)goto _LL151;else{_tmp1A2=_tmp1A1->f1;}}_LL150: {
# 1211
union Cyc_CfFlowInfo_FlowInfo _tmp28D;union Cyc_CfFlowInfo_AbsLVal _tmp28E;struct _tuple15 _tmp28C=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1A2);_tmp28D=_tmp28C.f1;_tmp28E=_tmp28C.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp28F=_tmp28E;struct Cyc_CfFlowInfo_Place*_tmp290;_LL1D4: if((_tmp28F.UnknownL).tag != 2)goto _LL1D6;_LL1D5: {
struct _tuple14 _tmp7C6;return(_tmp7C6.f1=_tmp28D,((_tmp7C6.f2=_tmp14D->notzeroall,_tmp7C6)));}_LL1D6: if((_tmp28F.PlaceL).tag != 1)goto _LL1D3;_tmp290=(struct Cyc_CfFlowInfo_Place*)(_tmp28F.PlaceL).val;_LL1D7: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7CC;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7CB;struct _tuple14 _tmp7CA;return(_tmp7CA.f1=_tmp28D,((_tmp7CA.f2=(void*)((_tmp7CC=_region_malloc(env->r,sizeof(*_tmp7CC)),((_tmp7CC[0]=((_tmp7CB.tag=5,((_tmp7CB.f1=_tmp290,_tmp7CB)))),_tmp7CC)))),_tmp7CA)));}_LL1D3:;};}_LL151: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A3->tag != 19)goto _LL153;else{_tmp1A4=_tmp1A3->f1;}}_LL152: {
# 1218
union Cyc_CfFlowInfo_FlowInfo _tmp296;void*_tmp297;struct _tuple14 _tmp295=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A4);_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp296,_tmp1A4,_tmp297);}_LL153: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A5->tag != 20)goto _LL155;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LL154: {
# 1223
union Cyc_CfFlowInfo_FlowInfo _tmp299;void*_tmp29A;struct _tuple14 _tmp298=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A6);_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1A6->topt))){
struct _tuple14 _tmp7CD;return(_tmp7CD.f1=_tmp299,((_tmp7CD.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7CD)));}{
void*_tmp29C=_tmp29A;int _tmp29F;int _tmp2A0;struct _dyneither_ptr _tmp2A1;int _tmp2A3;int _tmp2A4;struct _dyneither_ptr _tmp2A5;_LL1D9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp29D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp29C;if(_tmp29D->tag != 8)goto _LL1DB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp29D->f2);if(_tmp29E->tag != 6)goto _LL1DB;else{_tmp29F=(_tmp29E->f1).is_union;_tmp2A0=(_tmp29E->f1).fieldnum;_tmp2A1=_tmp29E->f2;}}}_LL1DA:
# 1228
 _tmp2A3=_tmp29F;_tmp2A4=_tmp2A0;_tmp2A5=_tmp2A1;goto _LL1DC;_LL1DB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp29C;if(_tmp2A2->tag != 6)goto _LL1DD;else{_tmp2A3=(_tmp2A2->f1).is_union;_tmp2A4=(_tmp2A2->f1).fieldnum;_tmp2A5=_tmp2A2->f2;}}_LL1DC: {
# 1230
int _tmp2A6=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A6->topt),_tmp1A7);
if((_tmp2A3  && _tmp2A4 != - 1) && _tmp2A4 != _tmp2A6){
struct _tuple14 _tmp7CE;return(_tmp7CE.f1=_tmp299,((_tmp7CE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),1,_tmp14D->unknown_none),_tmp7CE)));}{
struct _tuple14 _tmp7CF;return(_tmp7CF.f1=_tmp299,((_tmp7CF.f2=*((void**)_check_dyneither_subscript(_tmp2A5,sizeof(void*),_tmp2A6)),_tmp7CF)));};}_LL1DD:;_LL1DE: {
# 1235
struct Cyc_Core_Impossible_exn_struct _tmp7DC;const char*_tmp7DB;void*_tmp7DA[1];struct Cyc_String_pa_PrintArg_struct _tmp7D9;struct Cyc_Core_Impossible_exn_struct*_tmp7D8;(int)_throw((void*)((_tmp7D8=_cycalloc(sizeof(*_tmp7D8)),((_tmp7D8[0]=((_tmp7DC.tag=Cyc_Core_Impossible,((_tmp7DC.f1=(struct _dyneither_ptr)((_tmp7D9.tag=0,((_tmp7D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DA[0]=& _tmp7D9,Cyc_aprintf(((_tmp7DB="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp7DB,sizeof(char),26))),_tag_dyneither(_tmp7DA,sizeof(void*),1)))))))),_tmp7DC)))),_tmp7D8)))));}_LL1D8:;};}_LL155: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1A8->tag != 37)goto _LL157;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL156: {
# 1242
union Cyc_CfFlowInfo_FlowInfo _tmp2AF;void*_tmp2B0;struct _tuple14 _tmp2AE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A9);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1A9->topt))){
struct _tuple14 _tmp7DD;return(_tmp7DD.f1=_tmp2AF,((_tmp7DD.f2=_tmp14D->unknown_all,_tmp7DD)));}{
void*_tmp2B2=_tmp2B0;int _tmp2B5;int _tmp2B6;struct _dyneither_ptr _tmp2B7;int _tmp2B9;int _tmp2BA;struct _dyneither_ptr _tmp2BB;_LL1E0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B3->tag != 8)goto _LL1E2;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2B3->f2);if(_tmp2B4->tag != 6)goto _LL1E2;else{_tmp2B5=(_tmp2B4->f1).is_union;_tmp2B6=(_tmp2B4->f1).fieldnum;_tmp2B7=_tmp2B4->f2;}}}_LL1E1:
# 1247
 _tmp2B9=_tmp2B5;_tmp2BA=_tmp2B6;_tmp2BB=_tmp2B7;goto _LL1E3;_LL1E2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2;if(_tmp2B8->tag != 6)goto _LL1E4;else{_tmp2B9=(_tmp2B8->f1).is_union;_tmp2BA=(_tmp2B8->f1).fieldnum;_tmp2BB=_tmp2B8->f2;}}_LL1E3: {
# 1249
int _tmp2BC=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A9->topt),_tmp1AA);
if(_tmp2B9  && _tmp2BA != - 1){
if(_tmp2BA != _tmp2BC){
struct _tuple14 _tmp7DE;return(_tmp7DE.f1=_tmp2AF,((_tmp7DE.f2=_tmp14D->zero,_tmp7DE)));}else{
# 1254
struct _tuple14 _tmp7DF;return(_tmp7DF.f1=_tmp2AF,((_tmp7DF.f2=_tmp14D->notzeroall,_tmp7DF)));}}else{
# 1256
struct _tuple14 _tmp7E0;return(_tmp7E0.f1=_tmp2AF,((_tmp7E0.f2=_tmp14D->unknown_all,_tmp7E0)));}}_LL1E4:;_LL1E5: {
# 1258
struct Cyc_Core_Impossible_exn_struct _tmp7ED;const char*_tmp7EC;void*_tmp7EB[1];struct Cyc_String_pa_PrintArg_struct _tmp7EA;struct Cyc_Core_Impossible_exn_struct*_tmp7E9;(int)_throw((void*)((_tmp7E9=_cycalloc(sizeof(*_tmp7E9)),((_tmp7E9[0]=((_tmp7ED.tag=Cyc_Core_Impossible,((_tmp7ED.f1=(struct _dyneither_ptr)((_tmp7EA.tag=0,((_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7EB[0]=& _tmp7EA,Cyc_aprintf(((_tmp7EC="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp7EC,sizeof(char),26))),_tag_dyneither(_tmp7EB,sizeof(void*),1)))))))),_tmp7ED)))),_tmp7E9)))));}_LL1DF:;};}_LL157: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1AB->tag != 21)goto _LL159;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL158: {
# 1263
union Cyc_CfFlowInfo_FlowInfo _tmp2C6;void*_tmp2C7;struct _tuple14 _tmp2C5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AC);_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;{
# 1266
union Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple14 _tmp2C8=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C6,_tmp1AC,_tmp2C7);_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;{
# 1269
void*_tmp2CB=Cyc_Tcutil_compress((void*)_check_null(_tmp1AC->topt));void*_tmp2CD;_LL1E7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB;if(_tmp2CC->tag != 5)goto _LL1E9;else{_tmp2CD=(_tmp2CC->f1).elt_typ;}}_LL1E8:
# 1271
 if(Cyc_Absyn_is_nontagged_union_type(_tmp2CD)){
struct _tuple14 _tmp7EE;return(_tmp7EE.f1=_tmp2C9,((_tmp7EE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp7EE)));}{
void*_tmp2CF=_tmp2CA;int _tmp2D2;int _tmp2D3;struct _dyneither_ptr _tmp2D4;int _tmp2D6;int _tmp2D7;struct _dyneither_ptr _tmp2D8;_LL1EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2D0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D0->tag != 8)goto _LL1EE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2D0->f2);if(_tmp2D1->tag != 6)goto _LL1EE;else{_tmp2D2=(_tmp2D1->f1).is_union;_tmp2D3=(_tmp2D1->f1).fieldnum;_tmp2D4=_tmp2D1->f2;}}}_LL1ED:
# 1275
 _tmp2D6=_tmp2D2;_tmp2D7=_tmp2D3;_tmp2D8=_tmp2D4;goto _LL1EF;_LL1EE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2CF;if(_tmp2D5->tag != 6)goto _LL1F0;else{_tmp2D6=(_tmp2D5->f1).is_union;_tmp2D7=(_tmp2D5->f1).fieldnum;_tmp2D8=_tmp2D5->f2;}}_LL1EF: {
# 1277
int _tmp2D9=Cyc_CfFlowInfo_get_field_index(_tmp2CD,_tmp1AD);
if((_tmp2D6  && _tmp2D7 != - 1) && _tmp2D7 != _tmp2D9){
struct _tuple14 _tmp7EF;return(_tmp7EF.f1=_tmp2C9,((_tmp7EF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),1,_tmp14D->unknown_none),_tmp7EF)));}{
struct _tuple14 _tmp7F0;return(_tmp7F0.f1=_tmp2C9,((_tmp7F0.f2=*((void**)_check_dyneither_subscript(_tmp2D8,sizeof(void*),_tmp2D9)),_tmp7F0)));};}_LL1F0:;_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7F6;const char*_tmp7F5;struct Cyc_Core_Impossible_exn_struct*_tmp7F4;(int)_throw((void*)((_tmp7F4=_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F6.tag=Cyc_Core_Impossible,((_tmp7F6.f1=((_tmp7F5="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7F5,sizeof(char),21))),_tmp7F6)))),_tmp7F4)))));}_LL1EB:;};_LL1E9:;_LL1EA: {
# 1283
struct Cyc_Core_Impossible_exn_struct _tmp7FC;const char*_tmp7FB;struct Cyc_Core_Impossible_exn_struct*_tmp7FA;(int)_throw((void*)((_tmp7FA=_cycalloc(sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FC.tag=Cyc_Core_Impossible,((_tmp7FC.f1=((_tmp7FB="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7FB,sizeof(char),25))),_tmp7FC)))),_tmp7FA)))));}_LL1E6:;};};}_LL159: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1AE->tag != 5)goto _LL15B;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;_tmp1B1=_tmp1AE->f3;}}_LL15A: {
# 1287
union Cyc_CfFlowInfo_FlowInfo _tmp2E3;union Cyc_CfFlowInfo_FlowInfo _tmp2E4;struct _tuple16 _tmp2E2=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AF);_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;{
struct _tuple14 _tmp2E5=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E3,_tmp1B0);
struct _tuple14 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E4,_tmp1B1);
# 1291
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2E5,_tmp2E6);};}_LL15B: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B2->tag != 6)goto _LL15D;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL15C: {
# 1294
union Cyc_CfFlowInfo_FlowInfo _tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp2E9;struct _tuple16 _tmp2E7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B3);_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2EB;void*_tmp2EC;struct _tuple14 _tmp2EA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E8,_tmp1B4);_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;{
struct _tuple14 _tmp7FD;struct _tuple14 _tmp2ED=(_tmp7FD.f1=_tmp2EB,((_tmp7FD.f2=_tmp2EC,_tmp7FD)));
struct _tuple14 _tmp7FE;struct _tuple14 _tmp2EE=(_tmp7FE.f1=_tmp2E9,((_tmp7FE.f2=_tmp14D->zero,_tmp7FE)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2ED,_tmp2EE);};};}_LL15D: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B5->tag != 7)goto _LL15F;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL15E: {
# 1301
union Cyc_CfFlowInfo_FlowInfo _tmp2F2;union Cyc_CfFlowInfo_FlowInfo _tmp2F3;struct _tuple16 _tmp2F1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B6);_tmp2F2=_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F5;void*_tmp2F6;struct _tuple14 _tmp2F4=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F3,_tmp1B7);_tmp2F5=_tmp2F4.f1;_tmp2F6=_tmp2F4.f2;{
struct _tuple14 _tmp7FF;struct _tuple14 _tmp2F7=(_tmp7FF.f1=_tmp2F5,((_tmp7FF.f2=_tmp2F6,_tmp7FF)));
struct _tuple14 _tmp800;struct _tuple14 _tmp2F8=(_tmp800.f1=_tmp2F2,((_tmp800.f2=_tmp14D->notzeroall,_tmp800)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14D,env->all_changed,_tmp2F7,_tmp2F8);};};}_LL15F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1B8->tag != 22)goto _LL161;else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;}}_LL160: {
# 1308
struct _RegionHandle _tmp2FB=_new_region("temp");struct _RegionHandle*temp=& _tmp2FB;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp2FE;struct Cyc_List_List*_tmp2FF;struct Cyc_Absyn_Exp*_tmp801[2];struct _tuple20 _tmp2FD=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp801[1]=_tmp1BA,((_tmp801[0]=_tmp1B9,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp801,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;{
# 1313
union Cyc_CfFlowInfo_FlowInfo _tmp300=_tmp2FE;
{union Cyc_CfFlowInfo_FlowInfo _tmp301=_tmp2FE;struct Cyc_Dict_Dict _tmp302;struct Cyc_List_List*_tmp303;_LL1F3: if((_tmp301.ReachableFL).tag != 2)goto _LL1F5;_tmp302=((struct _tuple13)(_tmp301.ReachableFL).val).f1;_tmp303=((struct _tuple13)(_tmp301.ReachableFL).val).f2;_LL1F4:
# 1318
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp302,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2FF))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp804;void*_tmp803;(_tmp803=0,Cyc_Tcutil_terr(_tmp1BA->loc,((_tmp804="expression may not be initialized",_tag_dyneither(_tmp804,sizeof(char),34))),_tag_dyneither(_tmp803,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp306=Cyc_NewControlFlow_add_subscript_reln(_tmp14D->r,_tmp303,_tmp1B9,_tmp1BA);
if(_tmp303 != _tmp306)
_tmp300=Cyc_CfFlowInfo_ReachableFL(_tmp302,_tmp306);
goto _LL1F2;};_LL1F5:;_LL1F6:
 goto _LL1F2;_LL1F2:;}{
# 1326
void*_tmp307=Cyc_Tcutil_compress((void*)_check_null(_tmp1B9->topt));struct Cyc_List_List*_tmp309;union Cyc_Absyn_Constraint*_tmp30B;_LL1F8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp308=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp307;if(_tmp308->tag != 10)goto _LL1FA;else{_tmp309=_tmp308->f1;}}_LL1F9: {
# 1328
void*_tmp30C=(void*)((struct Cyc_List_List*)_check_null(_tmp2FF))->hd;struct _dyneither_ptr _tmp30F;struct _dyneither_ptr _tmp311;_LL1FF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp30D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp30D->tag != 8)goto _LL201;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp30D->f2);if(_tmp30E->tag != 6)goto _LL201;else{_tmp30F=_tmp30E->f2;}}}_LL200:
# 1330
 _tmp311=_tmp30F;goto _LL202;_LL201: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp310=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp310->tag != 6)goto _LL203;else{_tmp311=_tmp310->f2;}}_LL202: {
# 1332
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1BA)).f1;
struct _tuple14 _tmp805;struct _tuple14 _tmp313=(_tmp805.f1=_tmp300,((_tmp805.f2=*((void**)_check_dyneither_subscript(_tmp311,sizeof(void*),(int)i)),_tmp805)));_npop_handler(0);return _tmp313;}_LL203:;_LL204: {
struct Cyc_Core_Impossible_exn_struct _tmp80B;const char*_tmp80A;struct Cyc_Core_Impossible_exn_struct*_tmp809;(int)_throw((void*)((_tmp809=_cycalloc(sizeof(*_tmp809)),((_tmp809[0]=((_tmp80B.tag=Cyc_Core_Impossible,((_tmp80B.f1=((_tmp80A="anal_Rexp: Subscript",_tag_dyneither(_tmp80A,sizeof(char),21))),_tmp80B)))),_tmp809)))));}_LL1FE:;}_LL1FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307;if(_tmp30A->tag != 5)goto _LL1FC;else{_tmp30B=((_tmp30A->f1).ptr_atts).bounds;}}_LL1FB: {
# 1337
union Cyc_CfFlowInfo_FlowInfo _tmp318;void*_tmp319;struct _tuple14 _tmp317=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2FE,_tmp1B9,(void*)((struct Cyc_List_List*)_check_null(_tmp2FF))->hd);_tmp318=_tmp317.f1;_tmp319=_tmp317.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp31A=_tmp318;_LL206: if((_tmp31A.BottomFL).tag != 1)goto _LL208;_LL207: {
struct _tuple14 _tmp80C;struct _tuple14 _tmp31C=(_tmp80C.f1=_tmp318,((_tmp80C.f2=_tmp319,_tmp80C)));_npop_handler(0);return _tmp31C;}_LL208:;_LL209: {
struct _tuple14 _tmp80D;struct _tuple14 _tmp31E=(_tmp80D.f1=_tmp300,((_tmp80D.f2=_tmp319,_tmp80D)));_npop_handler(0);return _tmp31E;}_LL205:;};}_LL1FC:;_LL1FD: {
# 1342
struct Cyc_Core_Impossible_exn_struct _tmp813;const char*_tmp812;struct Cyc_Core_Impossible_exn_struct*_tmp811;(int)_throw((void*)((_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811[0]=((_tmp813.tag=Cyc_Core_Impossible,((_tmp813.f1=((_tmp812="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp812,sizeof(char),33))),_tmp813)))),_tmp811)))));}_LL1F7:;};};}
# 1309
;_pop_region(temp);}_LL161: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1BB->tag != 30)goto _LL163;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL162:
# 1347
 _tmp1BF=_tmp1BC;goto _LL164;_LL163: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1BE->tag != 23)goto _LL165;else{_tmp1BF=_tmp1BE->f1;}}_LL164: {
# 1349
struct _RegionHandle _tmp322=_new_region("temp");struct _RegionHandle*temp=& _tmp322;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp324;struct Cyc_List_List*_tmp325;struct _tuple20 _tmp323=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1BF,1,1);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;{
unsigned int _tmp819;unsigned int _tmp818;struct _dyneither_ptr _tmp817;void**_tmp816;unsigned int _tmp815;struct _dyneither_ptr aggrdict=
(_tmp815=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BF),((_tmp816=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp815)),((_tmp817=_tag_dyneither(_tmp816,sizeof(void*),_tmp815),((((_tmp818=_tmp815,_tmp81A(& _tmp819,& _tmp818,& _tmp816,& _tmp325))),_tmp817)))))));
# 1357
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp824;struct Cyc_CfFlowInfo_UnionRInfo _tmp823;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp822;struct _tuple14 _tmp821;struct _tuple14 _tmp32A=(_tmp821.f1=_tmp324,((_tmp821.f2=(void*)((_tmp824=_region_malloc(env->r,sizeof(*_tmp824)),((_tmp824[0]=((_tmp822.tag=6,((_tmp822.f1=((_tmp823.is_union=0,((_tmp823.fieldnum=- 1,_tmp823)))),((_tmp822.f2=aggrdict,_tmp822)))))),_tmp824)))),_tmp821)));_npop_handler(0);return _tmp32A;};}
# 1350
;_pop_region(temp);}_LL165: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C0->tag != 29)goto _LL167;else{_tmp1C1=_tmp1C0->f2;}}_LL166: {
# 1360
struct Cyc_List_List*fs;
{void*_tmp330=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp332;_LL20B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp330;if(_tmp331->tag != 12)goto _LL20D;else{_tmp332=_tmp331->f2;}}_LL20C:
 fs=_tmp332;goto _LL20A;_LL20D:;_LL20E: {
struct Cyc_Core_Impossible_exn_struct _tmp82A;const char*_tmp829;struct Cyc_Core_Impossible_exn_struct*_tmp828;(int)_throw((void*)((_tmp828=_cycalloc(sizeof(*_tmp828)),((_tmp828[0]=((_tmp82A.tag=Cyc_Core_Impossible,((_tmp82A.f1=((_tmp829="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp829,sizeof(char),35))),_tmp82A)))),_tmp828)))));}_LL20A:;}
# 1365
_tmp1C3=_tmp1C1;_tmp1C4=Cyc_Absyn_StructA;_tmp1C5=fs;goto _LL168;}_LL167: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C2->tag != 28)goto _LL169;else{_tmp1C3=_tmp1C2->f3;if(_tmp1C2->f4 == 0)goto _LL169;_tmp1C4=(_tmp1C2->f4)->kind;if((_tmp1C2->f4)->impl == 0)goto _LL169;_tmp1C5=((_tmp1C2->f4)->impl)->fields;}}_LL168: {
# 1367
struct _RegionHandle _tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp338;struct Cyc_List_List*_tmp339;struct _tuple20 _tmp337=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C3),1,1);_tmp338=_tmp337.f1;_tmp339=_tmp337.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14D,_tmp1C5,_tmp1C4 == Cyc_Absyn_UnionA,_tmp1C4 == Cyc_Absyn_UnionA?_tmp14D->unknown_none: _tmp14D->unknown_all);
int field_no=- 1;
{int i=0;for(0;_tmp339 != 0;(((_tmp339=_tmp339->tl,_tmp1C3=_tmp1C3->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp1C3))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp33A=(void*)ds->hd;struct _dyneither_ptr*_tmp33D;_LL210: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33A;if(_tmp33B->tag != 0)goto _LL212;}_LL211: {
struct Cyc_Core_Impossible_exn_struct _tmp830;const char*_tmp82F;struct Cyc_Core_Impossible_exn_struct*_tmp82E;(int)_throw((void*)((_tmp82E=_cycalloc(sizeof(*_tmp82E)),((_tmp82E[0]=((_tmp830.tag=Cyc_Core_Impossible,((_tmp830.f1=((_tmp82F="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp82F,sizeof(char),22))),_tmp830)))),_tmp82E)))));}_LL212: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33A;if(_tmp33C->tag != 1)goto _LL20F;else{_tmp33D=_tmp33C->f1;}}_LL213:
# 1379
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1C5,_tmp33D);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp339->hd;
# 1382
if(_tmp1C4 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp341=(*((struct _tuple22*)_tmp1C3->hd)).f2;
_tmp338=Cyc_NewControlFlow_use_Rval(env,_tmp341->loc,_tmp338,(void*)_tmp339->hd);}_LL20F:;}}}{
# 1387
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp83A;struct Cyc_CfFlowInfo_UnionRInfo _tmp839;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp838;struct _tuple14 _tmp837;struct _tuple14 _tmp346=(_tmp837.f1=_tmp338,((_tmp837.f2=(void*)((_tmp83A=_region_malloc(env->r,sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp838.tag=6,((_tmp838.f1=((_tmp839.is_union=_tmp1C4 == Cyc_Absyn_UnionA,((_tmp839.fieldnum=field_no,_tmp839)))),((_tmp838.f2=aggrdict,_tmp838)))))),_tmp83A)))),_tmp837)));_npop_handler(0);return _tmp346;};};}
# 1368
;_pop_region(temp);}_LL169: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C6->tag != 28)goto _LL16B;}_LL16A: {
# 1390
struct Cyc_Core_Impossible_exn_struct _tmp840;const char*_tmp83F;struct Cyc_Core_Impossible_exn_struct*_tmp83E;(int)_throw((void*)((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp840.tag=Cyc_Core_Impossible,((_tmp840.f1=((_tmp83F="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp83F,sizeof(char),31))),_tmp840)))),_tmp83E)))));}_LL16B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C7->tag != 25)goto _LL16D;else{_tmp1C8=_tmp1C7->f1;}}_LL16C: {
# 1392
struct _RegionHandle _tmp34A=_new_region("temp");struct _RegionHandle*temp=& _tmp34A;_push_region(temp);
{struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C8);
union Cyc_CfFlowInfo_FlowInfo _tmp34D;struct Cyc_List_List*_tmp34E;struct _tuple20 _tmp34C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp34B,1,1);_tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;
for(0;_tmp34E != 0;(_tmp34E=_tmp34E->tl,_tmp34B=_tmp34B->tl)){
_tmp34D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp34B))->hd)->loc,_tmp34D,(void*)_tmp34E->hd);}{
struct _tuple14 _tmp841;struct _tuple14 _tmp350=(_tmp841.f1=_tmp34D,((_tmp841.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,(void*)_check_null(e->topt),0,_tmp14D->unknown_all),_tmp841)));_npop_handler(0);return _tmp350;};}
# 1393
;_pop_region(temp);}_LL16D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1C9->tag != 26)goto _LL16F;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;_tmp1CC=_tmp1C9->f3;_tmp1CD=_tmp1C9->f4;}}_LL16E: {
# 1401
union Cyc_CfFlowInfo_FlowInfo _tmp352;void*_tmp353;struct _tuple14 _tmp351=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CB);_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp354=_tmp352;struct Cyc_Dict_Dict _tmp355;struct Cyc_List_List*_tmp356;_LL215: if((_tmp354.BottomFL).tag != 1)goto _LL217;_LL216: {
struct _tuple14 _tmp842;return(_tmp842.f1=_tmp352,((_tmp842.f2=_tmp14D->unknown_all,_tmp842)));}_LL217: if((_tmp354.ReachableFL).tag != 2)goto _LL214;_tmp355=((struct _tuple13)(_tmp354.ReachableFL).val).f1;_tmp356=((struct _tuple13)(_tmp354.ReachableFL).val).f2;_LL218:
# 1405
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp355,_tmp353)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp845;void*_tmp844;(_tmp844=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp845="expression may not be initialized",_tag_dyneither(_tmp845,sizeof(char),34))),_tag_dyneither(_tmp844,sizeof(void*),0)));}{
# 1409
struct Cyc_List_List*new_relns=_tmp356;
comp_loop: {
void*_tmp35A=_tmp1CB->r;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Vardecl*_tmp35F;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Vardecl*_tmp368;int _tmp36A;enum Cyc_Absyn_Primop _tmp36C;struct Cyc_Absyn_Exp*_tmp36D;_LL21A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp35B->tag != 13)goto _LL21C;else{_tmp35C=_tmp35B->f2;}}_LL21B:
 _tmp1CB=_tmp35C;goto comp_loop;_LL21C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp35D->tag != 1)goto _LL21E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp35E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp35D->f2);if(_tmp35E->tag != 1)goto _LL21E;else{_tmp35F=_tmp35E->f1;}}}if(!(!_tmp35F->escapes))goto _LL21E;_LL21D:
 _tmp362=_tmp35F;goto _LL21F;_LL21E:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp360=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp360->tag != 1)goto _LL220;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp361=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp360->f2);if(_tmp361->tag != 4)goto _LL220;else{_tmp362=_tmp361->f1;}}}if(!(!_tmp362->escapes))goto _LL220;_LL21F:
 _tmp365=_tmp362;goto _LL221;_LL220:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp363->tag != 1)goto _LL222;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp364=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp363->f2);if(_tmp364->tag != 5)goto _LL222;else{_tmp365=_tmp364->f1;}}}if(!(!_tmp365->escapes))goto _LL222;_LL221:
 _tmp368=_tmp365;goto _LL223;_LL222:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp366->tag != 1)goto _LL224;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp367=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp366->f2);if(_tmp367->tag != 3)goto _LL224;else{_tmp368=_tmp367->f1;}}}if(!(!_tmp368->escapes))goto _LL224;_LL223:
# 1417
{struct Cyc_CfFlowInfo_Reln*_tmp848;struct Cyc_List_List*_tmp847;new_relns=((_tmp847=_region_malloc(env->r,sizeof(*_tmp847)),((_tmp847->hd=((_tmp848=_region_malloc(env->r,sizeof(*_tmp848)),((_tmp848->vd=_tmp1CA,((_tmp848->rop=Cyc_CfFlowInfo_LessVar(_tmp368,_tmp368->type),_tmp848)))))),((_tmp847->tl=_tmp356,_tmp847))))));}
goto _LL219;_LL224: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp369=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp369->tag != 0)goto _LL226;else{if(((_tmp369->f1).Int_c).tag != 5)goto _LL226;_tmp36A=((struct _tuple6)((_tmp369->f1).Int_c).val).f2;}}_LL225:
# 1420
{struct Cyc_CfFlowInfo_Reln*_tmp84B;struct Cyc_List_List*_tmp84A;new_relns=((_tmp84A=_region_malloc(env->r,sizeof(*_tmp84A)),((_tmp84A->hd=((_tmp84B=_region_malloc(env->r,sizeof(*_tmp84B)),((_tmp84B->vd=_tmp1CA,((_tmp84B->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp36A),_tmp84B)))))),((_tmp84A->tl=_tmp356,_tmp84A))))));}
goto _LL219;_LL226: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp36B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35A;if(_tmp36B->tag != 2)goto _LL228;else{_tmp36C=_tmp36B->f1;if(_tmp36B->f2 == 0)goto _LL228;_tmp36D=(struct Cyc_Absyn_Exp*)(_tmp36B->f2)->hd;}}_LL227:
# 1423
{void*_tmp372=_tmp36D->r;struct Cyc_Absyn_Vardecl*_tmp375;struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_Absyn_Vardecl*_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37E;_LL22B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp373->tag != 1)goto _LL22D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp374=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp373->f2);if(_tmp374->tag != 1)goto _LL22D;else{_tmp375=_tmp374->f1;}}}if(!(!_tmp375->escapes))goto _LL22D;_LL22C:
 _tmp378=_tmp375;goto _LL22E;_LL22D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp376=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp376->tag != 1)goto _LL22F;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp377=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp376->f2);if(_tmp377->tag != 4)goto _LL22F;else{_tmp378=_tmp377->f1;}}}if(!(!_tmp378->escapes))goto _LL22F;_LL22E:
 _tmp37B=_tmp378;goto _LL230;_LL22F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp379=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp379->tag != 1)goto _LL231;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp37A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp379->f2);if(_tmp37A->tag != 5)goto _LL231;else{_tmp37B=_tmp37A->f1;}}}if(!(!_tmp37B->escapes))goto _LL231;_LL230:
 _tmp37E=_tmp37B;goto _LL232;_LL231:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp37C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp372;if(_tmp37C->tag != 1)goto _LL233;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp37C->f2);if(_tmp37D->tag != 3)goto _LL233;else{_tmp37E=_tmp37D->f1;}}}if(!(!_tmp37E->escapes))goto _LL233;_LL232:
# 1429
{struct Cyc_CfFlowInfo_Reln*_tmp84E;struct Cyc_List_List*_tmp84D;new_relns=((_tmp84D=_region_malloc(env->r,sizeof(*_tmp84D)),((_tmp84D->hd=((_tmp84E=_region_malloc(env->r,sizeof(*_tmp84E)),((_tmp84E->vd=_tmp1CA,((_tmp84E->rop=Cyc_CfFlowInfo_LessNumelts(_tmp37E),_tmp84E)))))),((_tmp84D->tl=_tmp356,_tmp84D))))));}
goto _LL22A;_LL233:;_LL234:
 goto _LL22A;_LL22A:;}
# 1433
goto _LL219;_LL228:;_LL229:
 goto _LL219;_LL219:;}
# 1436
if(_tmp356 != new_relns)
_tmp352=Cyc_CfFlowInfo_ReachableFL(_tmp355,new_relns);{
# 1440
void*_tmp381=_tmp353;_LL236: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp382=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp382->tag != 0)goto _LL238;}_LL237: {
struct _tuple14 _tmp84F;return(_tmp84F.f1=_tmp352,((_tmp84F.f2=_tmp14D->unknown_all,_tmp84F)));}_LL238: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp383=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp383->tag != 2)goto _LL23A;}_LL239:
 goto _LL23B;_LL23A: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp384=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp384->tag != 1)goto _LL23C;}_LL23B:
 goto _LL23D;_LL23C: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp385=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp381;if(_tmp385->tag != 5)goto _LL23E;}_LL23D: {
# 1445
struct Cyc_List_List _tmp850;struct Cyc_List_List _tmp387=(_tmp850.hd=_tmp1CA,((_tmp850.tl=0,_tmp850)));
_tmp352=Cyc_NewControlFlow_add_vars(_tmp14D,_tmp352,& _tmp387,_tmp14D->unknown_all,e->loc,0);{
# 1448
union Cyc_CfFlowInfo_FlowInfo _tmp389;void*_tmp38A;struct _tuple14 _tmp388=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp352,_tmp1CC);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp38B=_tmp389;struct Cyc_Dict_Dict _tmp38C;_LL241: if((_tmp38B.BottomFL).tag != 1)goto _LL243;_LL242: {
struct _tuple14 _tmp851;return(_tmp851.f1=_tmp389,((_tmp851.f2=_tmp14D->unknown_all,_tmp851)));}_LL243: if((_tmp38B.ReachableFL).tag != 2)goto _LL240;_tmp38C=((struct _tuple13)(_tmp38B.ReachableFL).val).f1;_LL244:
# 1452
 if(Cyc_CfFlowInfo_initlevel(_tmp14D,_tmp38C,_tmp38A)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp854;void*_tmp853;(_tmp853=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp854="expression may not be initialized",_tag_dyneither(_tmp854,sizeof(char),34))),_tag_dyneither(_tmp853,sizeof(void*),0)));}{
struct _tuple14 _tmp855;return(_tmp855.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp855.f2=_tmp14D->unknown_all,_tmp855)));};}_LL240:;}
# 1457
_tmp352=_tmp389;
goto _LL23F;};}_LL23E:;_LL23F:
# 1460
 while(1){
struct Cyc_List_List _tmp856;struct Cyc_List_List _tmp392=(_tmp856.hd=_tmp1CA,((_tmp856.tl=0,_tmp856)));
_tmp352=Cyc_NewControlFlow_add_vars(_tmp14D,_tmp352,& _tmp392,_tmp14D->unknown_all,e->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp394;void*_tmp395;struct _tuple14 _tmp393=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp352,_tmp1CC);_tmp394=_tmp393.f1;_tmp395=_tmp393.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp396=_tmp394;struct Cyc_Dict_Dict _tmp397;_LL246: if((_tmp396.BottomFL).tag != 1)goto _LL248;_LL247:
 goto _LL245;_LL248: if((_tmp396.ReachableFL).tag != 2)goto _LL245;_tmp397=((struct _tuple13)(_tmp396.ReachableFL).val).f1;_LL249:
# 1467
 if(Cyc_CfFlowInfo_initlevel(_tmp14D,_tmp397,_tmp395)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp859;void*_tmp858;(_tmp858=0,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp859="expression may not be initialized",_tag_dyneither(_tmp859,sizeof(char),34))),_tag_dyneither(_tmp858,sizeof(void*),0)));}{
struct _tuple14 _tmp85A;return(_tmp85A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp85A.f2=_tmp14D->unknown_all,_tmp85A)));};}_LL245:;}{
# 1472
union Cyc_CfFlowInfo_FlowInfo _tmp39B=Cyc_CfFlowInfo_join_flow(_tmp14D,env->all_changed,_tmp352,_tmp394);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp39B,_tmp352))
break;
_tmp352=_tmp39B;};};}{
# 1477
struct _tuple14 _tmp85B;return(_tmp85B.f1=_tmp352,((_tmp85B.f2=_tmp14D->unknown_all,_tmp85B)));};_LL235:;};};_LL214:;};}_LL16F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1CE->tag != 27)goto _LL171;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=(void*)_tmp1CE->f2;_tmp1D1=_tmp1CE->f3;}}_LL170: {
# 1483
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp85E;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp85D;void*root=(void*)((_tmp85D=_region_malloc(_tmp14D->r,sizeof(*_tmp85D)),((_tmp85D[0]=((_tmp85E.tag=1,((_tmp85E.f1=_tmp1CF,((_tmp85E.f2=(void*)_check_null(e->topt),_tmp85E)))))),_tmp85D))));
struct Cyc_CfFlowInfo_Place*_tmp85F;struct Cyc_CfFlowInfo_Place*place=(_tmp85F=_region_malloc(_tmp14D->r,sizeof(*_tmp85F)),((_tmp85F->root=root,((_tmp85F->fields=0,_tmp85F)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp862;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp861;void*rval=(void*)((_tmp861=_region_malloc(_tmp14D->r,sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=5,((_tmp862.f1=place,_tmp862)))),_tmp861))));
void*place_val;
# 1491
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14D,_tmp1D0,0,_tmp14D->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp39F;struct _tuple14 _tmp39E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CF);_tmp39F=_tmp39E.f1;
outflow=_tmp39F;{
union Cyc_CfFlowInfo_FlowInfo _tmp3A0=outflow;struct Cyc_Dict_Dict _tmp3A1;struct Cyc_List_List*_tmp3A2;_LL24B: if((_tmp3A0.BottomFL).tag != 1)goto _LL24D;_LL24C: {
struct _tuple14 _tmp863;return(_tmp863.f1=outflow,((_tmp863.f2=rval,_tmp863)));}_LL24D: if((_tmp3A0.ReachableFL).tag != 2)goto _LL24A;_tmp3A1=((struct _tuple13)(_tmp3A0.ReachableFL).val).f1;_tmp3A2=((struct _tuple13)(_tmp3A0.ReachableFL).val).f2;_LL24E: {
# 1499
struct _tuple14 _tmp864;return(_tmp864.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3A1,root,place_val),_tmp3A2),((_tmp864.f2=rval,_tmp864)));}_LL24A:;};};};}_LL171: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D2->tag != 36)goto _LL173;else{_tmp1D3=_tmp1D2->f1;}}_LL172:
# 1502
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1D3);_LL173: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D4->tag != 1)goto _LL175;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1D5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1D4->f2);if(_tmp1D5->tag != 0)goto _LL175;}}_LL174:
 goto _LL176;_LL175: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D6->tag != 35)goto _LL177;}_LL176:
 goto _LL178;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D7->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D8->tag != 38)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp162;if(_tmp1D9->tag != 39)goto _LL114;}_LL17C: {
# 1508
struct Cyc_Core_Impossible_exn_struct _tmp86A;const char*_tmp869;struct Cyc_Core_Impossible_exn_struct*_tmp868;(int)_throw((void*)((_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((_tmp86A.tag=Cyc_Core_Impossible,((_tmp86A.f1=((_tmp869="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp869,sizeof(char),31))),_tmp86A)))),_tmp868)))));}_LL114:;};}
# 1517
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1524
struct Cyc_CfFlowInfo_FlowEnv*_tmp3AD=env->fenv;
void*_tmp3AE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3B0;union Cyc_Absyn_Constraint*_tmp3B1;union Cyc_Absyn_Constraint*_tmp3B2;_LL250: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AE;if(_tmp3AF->tag != 5)goto _LL252;else{_tmp3B0=(_tmp3AF->f1).elt_typ;_tmp3B1=((_tmp3AF->f1).ptr_atts).bounds;_tmp3B2=((_tmp3AF->f1).ptr_atts).zero_term;}}_LL251: {
# 1527
union Cyc_CfFlowInfo_FlowInfo _tmp3B3=f;struct Cyc_Dict_Dict _tmp3B4;struct Cyc_List_List*_tmp3B5;_LL255: if((_tmp3B3.BottomFL).tag != 1)goto _LL257;_LL256: {
struct _tuple15 _tmp86B;return(_tmp86B.f1=f,((_tmp86B.f2=Cyc_CfFlowInfo_UnknownL(),_tmp86B)));}_LL257: if((_tmp3B3.ReachableFL).tag != 2)goto _LL254;_tmp3B4=((struct _tuple13)(_tmp3B3.ReachableFL).val).f1;_tmp3B5=((struct _tuple13)(_tmp3B3.ReachableFL).val).f2;_LL258: {
# 1531
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp3B1)){
retry: {void*_tmp3B7=r;struct Cyc_Absyn_Vardecl*_tmp3B9;void*_tmp3BA;void*_tmp3BE;struct Cyc_List_List*_tmp3BF;enum Cyc_CfFlowInfo_InitLevel _tmp3C2;_LL25A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3B8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3B8->tag != 8)goto _LL25C;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=(void*)_tmp3B8->f2;}}_LL25B:
 r=_tmp3BA;locname=_tmp3B9;goto retry;_LL25C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3BB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3BB->tag != 1)goto _LL25E;}_LL25D:
 goto _LL25F;_LL25E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3BC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3BC->tag != 2)goto _LL260;}_LL25F:
# 1537
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp86E;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp86D;e->annot=(void*)((_tmp86D=_cycalloc(sizeof(*_tmp86D)),((_tmp86D[0]=((_tmp86E.tag=Cyc_CfFlowInfo_NotZero,((_tmp86E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3B5),_tmp86E)))),_tmp86D))));}goto _LL259;_LL260: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3BD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3BD->tag != 5)goto _LL262;else{_tmp3BE=(_tmp3BD->f1)->root;_tmp3BF=(_tmp3BD->f1)->fields;}}_LL261:
# 1539
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp871;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp870;e->annot=(void*)((_tmp870=_cycalloc(sizeof(*_tmp870)),((_tmp870[0]=((_tmp871.tag=Cyc_CfFlowInfo_NotZero,((_tmp871.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3B5),_tmp871)))),_tmp870))));}{
struct Cyc_CfFlowInfo_Place*_tmp874;struct _tuple15 _tmp873;return(_tmp873.f1=f,((_tmp873.f2=Cyc_CfFlowInfo_PlaceL(((_tmp874=_region_malloc(_tmp3AD->r,sizeof(*_tmp874)),((_tmp874->root=_tmp3BE,((_tmp874->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3AD->r,_tmp3BF,flds),_tmp874))))))),_tmp873)));};_LL262: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3C0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3C0->tag != 0)goto _LL264;}_LL263:
# 1542
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp875;return(_tmp875.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp875.f2=Cyc_CfFlowInfo_UnknownL(),_tmp875)));};_LL264: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3C1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3B7;if(_tmp3C1->tag != 3)goto _LL266;else{_tmp3C2=_tmp3C1->f1;}}_LL265:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3C2,locname);goto _LL267;_LL266:;_LL267: {
# 1546
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp878;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp877;e->annot=(void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp878.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp878.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3B5),_tmp878)))),_tmp877))));}_LL259:;}}else{
# 1549
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp87B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp87A;e->annot=(void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp87B.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3B5),_tmp87B)))),_tmp87A))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3AD,_tmp3B4,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp87E;void*_tmp87D;(_tmp87D=0,Cyc_Tcutil_terr(e->loc,((_tmp87E="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp87E,sizeof(char),46))),_tag_dyneither(_tmp87D,sizeof(void*),0)));}{
struct _tuple15 _tmp87F;return(_tmp87F.f1=f,((_tmp87F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp87F)));};}_LL254:;}_LL252:;_LL253: {
# 1554
struct Cyc_Core_Impossible_exn_struct _tmp885;const char*_tmp884;struct Cyc_Core_Impossible_exn_struct*_tmp883;(int)_throw((void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp885.tag=Cyc_Core_Impossible,((_tmp885.f1=((_tmp884="left deref of non-pointer-type",_tag_dyneither(_tmp884,sizeof(char),31))),_tmp885)))),_tmp883)))));}_LL24F:;}
# 1562
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1565
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3D4=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3D5=inflow;struct Cyc_Dict_Dict _tmp3D6;struct Cyc_List_List*_tmp3D7;_LL269: if((_tmp3D5.BottomFL).tag != 1)goto _LL26B;_LL26A: {
struct _tuple15 _tmp886;return(_tmp886.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp886.f2=Cyc_CfFlowInfo_UnknownL(),_tmp886)));}_LL26B: if((_tmp3D5.ReachableFL).tag != 2)goto _LL268;_tmp3D6=((struct _tuple13)(_tmp3D5.ReachableFL).val).f1;_tmp3D7=((struct _tuple13)(_tmp3D5.ReachableFL).val).f2;_LL26C:
# 1570
 d=_tmp3D6;_LL268:;}{
# 1574
void*_tmp3D9=e->r;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DF;struct Cyc_Absyn_Vardecl*_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E5;struct Cyc_Absyn_Vardecl*_tmp3E8;struct Cyc_Absyn_Vardecl*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3ED;struct _dyneither_ptr*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Exp*_tmp3F5;struct _dyneither_ptr*_tmp3F6;_LL26E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3DA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3DA->tag != 13)goto _LL270;else{_tmp3DB=_tmp3DA->f2;}}_LL26F:
 _tmp3DD=_tmp3DB;goto _LL271;_LL270: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3DC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3DC->tag != 11)goto _LL272;else{_tmp3DD=_tmp3DC->f1;}}_LL271:
 _tmp3DF=_tmp3DD;goto _LL273;_LL272: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3DE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3DE->tag != 12)goto _LL274;else{_tmp3DF=_tmp3DE->f1;}}_LL273:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3DF,flds);_LL274: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3E0->tag != 1)goto _LL276;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3E1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3E0->f2);if(_tmp3E1->tag != 3)goto _LL276;else{_tmp3E2=_tmp3E1->f1;}}}_LL275:
# 1579
 _tmp3E5=_tmp3E2;goto _LL277;_LL276: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3E3->tag != 1)goto _LL278;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3E4=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3E3->f2);if(_tmp3E4->tag != 4)goto _LL278;else{_tmp3E5=_tmp3E4->f1;}}}_LL277:
 _tmp3E8=_tmp3E5;goto _LL279;_LL278: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3E6->tag != 1)goto _LL27A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3E7=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3E6->f2);if(_tmp3E7->tag != 5)goto _LL27A;else{_tmp3E8=_tmp3E7->f1;}}}_LL279: {
# 1582
struct Cyc_CfFlowInfo_Place*_tmp890;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp88F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp88E;struct _tuple15 _tmp88D;return(_tmp88D.f1=inflow,((_tmp88D.f2=Cyc_CfFlowInfo_PlaceL(((_tmp890=_region_malloc(env->r,sizeof(*_tmp890)),((_tmp890->root=(void*)((_tmp88E=_region_malloc(env->r,sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=0,((_tmp88F.f1=_tmp3E8,_tmp88F)))),_tmp88E)))),((_tmp890->fields=flds,_tmp890))))))),_tmp88D)));}_LL27A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3E9->tag != 1)goto _LL27C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3EA=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3E9->f2);if(_tmp3EA->tag != 1)goto _LL27C;else{_tmp3EB=_tmp3EA->f1;}}}_LL27B: {
# 1584
struct _tuple15 _tmp891;return(_tmp891.f1=inflow,((_tmp891.f2=Cyc_CfFlowInfo_UnknownL(),_tmp891)));}_LL27C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3EC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3EC->tag != 21)goto _LL27E;else{_tmp3ED=_tmp3EC->f1;_tmp3EE=_tmp3EC->f2;}}_LL27D:
# 1587
{void*_tmp3FC=Cyc_Tcutil_compress((void*)_check_null(_tmp3ED->topt));void*_tmp3FE;_LL287: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FC;if(_tmp3FD->tag != 5)goto _LL289;else{_tmp3FE=(_tmp3FD->f1).elt_typ;}}_LL288:
# 1589
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp3FE)){
struct Cyc_List_List*_tmp892;flds=((_tmp892=_region_malloc(env->r,sizeof(*_tmp892)),((_tmp892->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3FE,_tmp3EE),((_tmp892->tl=flds,_tmp892))))));}
goto _LL286;_LL289:;_LL28A: {
struct Cyc_Core_Impossible_exn_struct _tmp898;const char*_tmp897;struct Cyc_Core_Impossible_exn_struct*_tmp896;(int)_throw((void*)((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp898.tag=Cyc_Core_Impossible,((_tmp898.f1=((_tmp897="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp897,sizeof(char),25))),_tmp898)))),_tmp896)))));}_LL286:;}
# 1594
_tmp3F0=_tmp3ED;goto _LL27F;_LL27E: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3EF=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3EF->tag != 19)goto _LL280;else{_tmp3F0=_tmp3EF->f1;}}_LL27F:
# 1596
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3F0->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp404;union Cyc_CfFlowInfo_AbsLVal _tmp405;struct _tuple15 _tmp403=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3F0);_tmp404=_tmp403.f1;_tmp405=_tmp403.f2;{
struct _tuple15 _tmp899;struct _tuple15 _tmp407=(_tmp899.f1=_tmp404,((_tmp899.f2=_tmp405,_tmp899)));struct Cyc_Dict_Dict _tmp408;struct Cyc_List_List*_tmp409;struct Cyc_CfFlowInfo_Place*_tmp40A;_LL28C: if(((_tmp407.f1).ReachableFL).tag != 2)goto _LL28E;_tmp408=((struct _tuple13)((_tmp407.f1).ReachableFL).val).f1;_tmp409=((struct _tuple13)((_tmp407.f1).ReachableFL).val).f2;if(((_tmp407.f2).PlaceL).tag != 1)goto _LL28E;_tmp40A=(struct Cyc_CfFlowInfo_Place*)((_tmp407.f2).PlaceL).val;_LL28D: {
# 1600
void*_tmp40B=Cyc_CfFlowInfo_lookup_place(_tmp408,_tmp40A);
struct Cyc_Absyn_Vardecl*locname=0;
void*_tmp40C=_tmp40B;struct Cyc_Absyn_Vardecl*_tmp40F;void*_tmp410;_LL291: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp40D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp40C;if(_tmp40D->tag != 5)goto _LL293;}_LL292:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp404,_tmp3F0,_tmp404,_tmp40B,flds);_LL293: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp40E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp40C;if(_tmp40E->tag != 8)goto _LL295;else{_tmp40F=_tmp40E->f1;_tmp410=(void*)_tmp40E->f2;}}_LL294:
 _tmp40B=_tmp410;locname=_tmp40F;goto _LL296;_LL295:;_LL296: {
# 1611
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3D4,_tmp408,_tmp40B);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3D4->unknown_all: _tmp3D4->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3D4,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3F0->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp89C;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp89B;void*new_root=(void*)((_tmp89B=_region_malloc(_tmp3D4->r,sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89C.tag=1,((_tmp89C.f1=e,((_tmp89C.f2=(void*)_check_null(e->topt),_tmp89C)))))),_tmp89B))));
struct Cyc_CfFlowInfo_Place*_tmp89D;struct Cyc_CfFlowInfo_Place*place=(_tmp89D=_region_malloc(_tmp3D4->r,sizeof(*_tmp89D)),((_tmp89D->root=new_root,((_tmp89D->fields=0,_tmp89D)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8A0;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp89F;void*res=(void*)((_tmp89F=_region_malloc(_tmp3D4->r,sizeof(*_tmp89F)),((_tmp89F[0]=((_tmp8A0.tag=5,((_tmp8A0.f1=place,_tmp8A0)))),_tmp89F))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8A3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8A2;res=(void*)((_tmp8A2=_region_malloc(_tmp3D4->r,sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=8,((_tmp8A3.f1=locname,((_tmp8A3.f2=res,_tmp8A3)))))),_tmp8A2))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp408=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp408,new_root,new_rval);
_tmp408=Cyc_CfFlowInfo_assign_place(_tmp3D4,e->loc,_tmp408,env->all_changed,_tmp40A,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp413=Cyc_CfFlowInfo_ReachableFL(_tmp408,_tmp409);
# 1626
return Cyc_NewControlFlow_anal_derefL(env,_tmp413,_tmp3F0,_tmp413,res,flds);};}_LL290:;}_LL28E:;_LL28F:
# 1628
 goto _LL28B;_LL28B:;};}{
# 1631
union Cyc_CfFlowInfo_FlowInfo _tmp41A;void*_tmp41B;struct _tuple14 _tmp419=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3F0);_tmp41A=_tmp419.f1;_tmp41B=_tmp419.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F0,_tmp41A,_tmp41B,flds);};_LL280: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3F1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3F1->tag != 22)goto _LL282;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;}}_LL281: {
# 1635
void*_tmp41C=Cyc_Tcutil_compress((void*)_check_null(_tmp3F2->topt));union Cyc_Absyn_Constraint*_tmp41F;_LL298: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp41C;if(_tmp41D->tag != 10)goto _LL29A;}_LL299: {
# 1637
unsigned int _tmp420=(Cyc_Evexp_eval_const_uint_exp(_tmp3F3)).f1;
struct Cyc_List_List*_tmp8A4;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3F2,((_tmp8A4=_region_malloc(env->r,sizeof(*_tmp8A4)),((_tmp8A4->hd=(void*)((int)_tmp420),((_tmp8A4->tl=flds,_tmp8A4)))))));}_LL29A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41C;if(_tmp41E->tag != 5)goto _LL29C;else{_tmp41F=((_tmp41E->f1).ptr_atts).bounds;}}_LL29B: {
# 1640
struct _RegionHandle _tmp422=_new_region("temp");struct _RegionHandle*temp=& _tmp422;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp425;struct Cyc_List_List*_tmp426;struct Cyc_Absyn_Exp*_tmp8A5[2];struct _tuple20 _tmp424=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp8A5[1]=_tmp3F3,((_tmp8A5[0]=_tmp3F2,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8A5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp425=_tmp424.f1;_tmp426=_tmp424.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp427=_tmp425;
{union Cyc_CfFlowInfo_FlowInfo _tmp428=_tmp425;struct Cyc_Dict_Dict _tmp429;struct Cyc_List_List*_tmp42A;_LL29F: if((_tmp428.ReachableFL).tag != 2)goto _LL2A1;_tmp429=((struct _tuple13)(_tmp428.ReachableFL).val).f1;_tmp42A=((struct _tuple13)(_tmp428.ReachableFL).val).f2;_LL2A0:
# 1646
 if(Cyc_CfFlowInfo_initlevel(_tmp3D4,_tmp429,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp426))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,Cyc_Tcutil_terr(_tmp3F3->loc,((_tmp8A8="expression may not be initialized",_tag_dyneither(_tmp8A8,sizeof(char),34))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp42D=Cyc_NewControlFlow_add_subscript_reln(_tmp3D4->r,_tmp42A,_tmp3F2,_tmp3F3);
if(_tmp42A != _tmp42D)
_tmp427=Cyc_CfFlowInfo_ReachableFL(_tmp429,_tmp42D);
goto _LL29E;};_LL2A1:;_LL2A2:
 goto _LL29E;_LL29E:;}{
# 1654
union Cyc_CfFlowInfo_FlowInfo _tmp42F;union Cyc_CfFlowInfo_AbsLVal _tmp430;struct _tuple15 _tmp42E=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F2,_tmp425,(void*)((struct Cyc_List_List*)_check_null(_tmp426))->hd,flds);_tmp42F=_tmp42E.f1;_tmp430=_tmp42E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp431=_tmp42F;_LL2A4: if((_tmp431.BottomFL).tag != 1)goto _LL2A6;_LL2A5: {
struct _tuple15 _tmp8A9;struct _tuple15 _tmp433=(_tmp8A9.f1=_tmp42F,((_tmp8A9.f2=_tmp430,_tmp8A9)));_npop_handler(0);return _tmp433;}_LL2A6:;_LL2A7: {
struct _tuple15 _tmp8AA;struct _tuple15 _tmp435=(_tmp8AA.f1=_tmp427,((_tmp8AA.f2=_tmp430,_tmp8AA)));_npop_handler(0);return _tmp435;}_LL2A3:;};};};}
# 1641
;_pop_region(temp);}_LL29C:;_LL29D: {
# 1660
struct Cyc_Core_Impossible_exn_struct _tmp8B0;const char*_tmp8AF;struct Cyc_Core_Impossible_exn_struct*_tmp8AE;(int)_throw((void*)((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8B0.tag=Cyc_Core_Impossible,((_tmp8B0.f1=((_tmp8AF="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp8AF,sizeof(char),33))),_tmp8B0)))),_tmp8AE)))));}_LL297:;}_LL282: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3F4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D9;if(_tmp3F4->tag != 20)goto _LL284;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;}}_LL283:
# 1664
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp3F5->topt))){
struct _tuple15 _tmp8B1;return(_tmp8B1.f1=inflow,((_tmp8B1.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8B1)));}{
struct Cyc_List_List*_tmp8B2;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3F5,(
(_tmp8B2=_region_malloc(env->r,sizeof(*_tmp8B2)),((_tmp8B2->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp3F5->topt),_tmp3F6),((_tmp8B2->tl=flds,_tmp8B2)))))));};_LL284:;_LL285: {
# 1669
struct _tuple15 _tmp8B3;return(_tmp8B3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B3.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8B3)));}_LL26D:;};}
# 1673
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1675
union Cyc_CfFlowInfo_FlowInfo _tmp43D;union Cyc_CfFlowInfo_AbsLVal _tmp43E;struct _tuple15 _tmp43C=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp43D=_tmp43C.f1;_tmp43E=_tmp43C.f2;{
struct _tuple15 _tmp8B4;return(_tmp8B4.f1=_tmp43D,((_tmp8B4.f2=_tmp43E,_tmp8B4)));};}
# 1681
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1683
struct Cyc_CfFlowInfo_FlowEnv*_tmp440=env->fenv;
{void*_tmp441=e->r;struct Cyc_Absyn_Exp*_tmp443;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp450;enum Cyc_Absyn_Primop _tmp452;struct Cyc_List_List*_tmp453;_LL2A9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp442=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp442->tag != 5)goto _LL2AB;else{_tmp443=_tmp442->f1;_tmp444=_tmp442->f2;_tmp445=_tmp442->f3;}}_LL2AA: {
# 1686
union Cyc_CfFlowInfo_FlowInfo _tmp455;union Cyc_CfFlowInfo_FlowInfo _tmp456;struct _tuple16 _tmp454=Cyc_NewControlFlow_anal_test(env,inflow,_tmp443);_tmp455=_tmp454.f1;_tmp456=_tmp454.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp458;union Cyc_CfFlowInfo_FlowInfo _tmp459;struct _tuple16 _tmp457=Cyc_NewControlFlow_anal_test(env,_tmp455,_tmp444);_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp45B;union Cyc_CfFlowInfo_FlowInfo _tmp45C;struct _tuple16 _tmp45A=Cyc_NewControlFlow_anal_test(env,_tmp456,_tmp445);_tmp45B=_tmp45A.f1;_tmp45C=_tmp45A.f2;{
struct _tuple16 _tmp8B5;return(_tmp8B5.f1=Cyc_CfFlowInfo_join_flow(_tmp440,env->all_changed,_tmp458,_tmp45B),((_tmp8B5.f2=
Cyc_CfFlowInfo_join_flow(_tmp440,env->all_changed,_tmp459,_tmp45C),_tmp8B5)));};};};}_LL2AB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp446=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp446->tag != 6)goto _LL2AD;else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL2AC: {
# 1692
union Cyc_CfFlowInfo_FlowInfo _tmp45F;union Cyc_CfFlowInfo_FlowInfo _tmp460;struct _tuple16 _tmp45E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp447);_tmp45F=_tmp45E.f1;_tmp460=_tmp45E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp462;union Cyc_CfFlowInfo_FlowInfo _tmp463;struct _tuple16 _tmp461=Cyc_NewControlFlow_anal_test(env,_tmp45F,_tmp448);_tmp462=_tmp461.f1;_tmp463=_tmp461.f2;{
struct _tuple16 _tmp8B6;return(_tmp8B6.f1=_tmp462,((_tmp8B6.f2=Cyc_CfFlowInfo_join_flow(_tmp440,env->all_changed,_tmp460,_tmp463),_tmp8B6)));};};}_LL2AD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp449=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp449->tag != 7)goto _LL2AF;else{_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;}}_LL2AE: {
# 1696
union Cyc_CfFlowInfo_FlowInfo _tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp467;struct _tuple16 _tmp465=Cyc_NewControlFlow_anal_test(env,inflow,_tmp44A);_tmp466=_tmp465.f1;_tmp467=_tmp465.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp469;union Cyc_CfFlowInfo_FlowInfo _tmp46A;struct _tuple16 _tmp468=Cyc_NewControlFlow_anal_test(env,_tmp467,_tmp44B);_tmp469=_tmp468.f1;_tmp46A=_tmp468.f2;{
struct _tuple16 _tmp8B7;return(_tmp8B7.f1=Cyc_CfFlowInfo_join_flow(_tmp440,env->all_changed,_tmp466,_tmp469),((_tmp8B7.f2=_tmp46A,_tmp8B7)));};};}_LL2AF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp44C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp44C->tag != 8)goto _LL2B1;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}_LL2B0: {
# 1700
union Cyc_CfFlowInfo_FlowInfo _tmp46D;void*_tmp46E;struct _tuple14 _tmp46C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp44D);_tmp46D=_tmp46C.f1;_tmp46E=_tmp46C.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp46D,_tmp44E);}_LL2B1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp44F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp44F->tag != 2)goto _LL2B3;else{if(_tmp44F->f1 != Cyc_Absyn_Not)goto _LL2B3;if(_tmp44F->f2 == 0)goto _LL2B3;_tmp450=(struct Cyc_Absyn_Exp*)(_tmp44F->f2)->hd;if((_tmp44F->f2)->tl != 0)goto _LL2B3;}}_LL2B2: {
# 1703
union Cyc_CfFlowInfo_FlowInfo _tmp470;union Cyc_CfFlowInfo_FlowInfo _tmp471;struct _tuple16 _tmp46F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp450);_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
struct _tuple16 _tmp8B8;return(_tmp8B8.f1=_tmp471,((_tmp8B8.f2=_tmp470,_tmp8B8)));};}_LL2B3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp451=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp441;if(_tmp451->tag != 2)goto _LL2B5;else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;}}_LL2B4: {
# 1708
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp473=_new_region("temp");struct _RegionHandle*temp=& _tmp473;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp475;struct Cyc_List_List*_tmp476;struct _tuple20 _tmp474=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp453,0,0);_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp476))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp476->tl))->hd;
f=_tmp475;}{
# 1718
union Cyc_CfFlowInfo_FlowInfo _tmp477=f;struct Cyc_Dict_Dict _tmp478;struct Cyc_List_List*_tmp479;_LL2B8: if((_tmp477.BottomFL).tag != 1)goto _LL2BA;_LL2B9: {
struct _tuple16 _tmp8B9;struct _tuple16 _tmp47B=(_tmp8B9.f1=f,((_tmp8B9.f2=f,_tmp8B9)));_npop_handler(0);return _tmp47B;}_LL2BA: if((_tmp477.ReachableFL).tag != 2)goto _LL2B7;_tmp478=((struct _tuple13)(_tmp477.ReachableFL).val).f1;_tmp479=((struct _tuple13)(_tmp477.ReachableFL).val).f2;_LL2BB: {
# 1721
struct Cyc_Absyn_Exp*_tmp47C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp453))->hd;
struct Cyc_Absyn_Exp*_tmp47D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp453->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp478,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8BC;void*_tmp8BB;(_tmp8BB=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp453->hd)->loc,((_tmp8BC="expression may not be initialized",_tag_dyneither(_tmp8BC,sizeof(char),34))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp478,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8BF;void*_tmp8BE;(_tmp8BE=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp453->tl))->hd)->loc,((_tmp8BF="expression may not be initialized",_tag_dyneither(_tmp8BF,sizeof(char),34))),_tag_dyneither(_tmp8BE,sizeof(void*),0)));}
# 1730
if(_tmp452 == Cyc_Absyn_Eq  || _tmp452 == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp482=r1;struct Cyc_Absyn_Vardecl*_tmp484;void*_tmp485;_LL2BD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp483=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp482;if(_tmp483->tag != 8)goto _LL2BF;else{_tmp484=_tmp483->f1;_tmp485=(void*)_tmp483->f2;}}_LL2BE:
 r1=_tmp485;r1n=_tmp484;goto _LL2BC;_LL2BF:;_LL2C0:
 goto _LL2BC;_LL2BC:;}
# 1736
{void*_tmp486=r2;struct Cyc_Absyn_Vardecl*_tmp488;void*_tmp489;_LL2C2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp487=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp486;if(_tmp487->tag != 8)goto _LL2C4;else{_tmp488=_tmp487->f1;_tmp489=(void*)_tmp487->f2;}}_LL2C3:
 r2=_tmp489;r2n=_tmp488;goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}{
# 1741
struct _tuple0 _tmp8C0;struct _tuple0 _tmp48B=(_tmp8C0.f1=r1,((_tmp8C0.f2=r2,_tmp8C0)));enum Cyc_CfFlowInfo_InitLevel _tmp48D;enum Cyc_CfFlowInfo_InitLevel _tmp491;_LL2C7:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp48C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp48C->tag != 3)goto _LL2C9;else{_tmp48D=_tmp48C->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp48E->tag != 0)goto _LL2C9;};_LL2C8: {
# 1745
union Cyc_CfFlowInfo_FlowInfo _tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A2;struct _tuple16 _tmp4A0=Cyc_NewControlFlow_splitzero(env,f,f,_tmp47C,_tmp48D,r1n);_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;
switch(_tmp452){case Cyc_Absyn_Eq: _LL2DB: {
struct _tuple16 _tmp8C1;struct _tuple16 _tmp4A4=(_tmp8C1.f1=_tmp4A2,((_tmp8C1.f2=_tmp4A1,_tmp8C1)));_npop_handler(0);return _tmp4A4;}case Cyc_Absyn_Neq: _LL2DC: {
struct _tuple16 _tmp8C2;struct _tuple16 _tmp4A6=(_tmp8C2.f1=_tmp4A1,((_tmp8C2.f2=_tmp4A2,_tmp8C2)));_npop_handler(0);return _tmp4A6;}default: _LL2DD: {
struct Cyc_Core_Impossible_exn_struct _tmp8C8;const char*_tmp8C7;struct Cyc_Core_Impossible_exn_struct*_tmp8C6;(int)_throw((void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C8.tag=Cyc_Core_Impossible,((_tmp8C8.f1=((_tmp8C7="anal_test, zero-split",_tag_dyneither(_tmp8C7,sizeof(char),22))),_tmp8C8)))),_tmp8C6)))));}}}_LL2C9:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp48F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp48F->tag != 0)goto _LL2CB;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp490=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp490->tag != 3)goto _LL2CB;else{_tmp491=_tmp490->f1;}};_LL2CA: {
# 1752
union Cyc_CfFlowInfo_FlowInfo _tmp4AB;union Cyc_CfFlowInfo_FlowInfo _tmp4AC;struct _tuple16 _tmp4AA=Cyc_NewControlFlow_splitzero(env,f,f,_tmp47D,_tmp491,r2n);_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;
switch(_tmp452){case Cyc_Absyn_Eq: _LL2DF: {
struct _tuple16 _tmp8C9;struct _tuple16 _tmp4AE=(_tmp8C9.f1=_tmp4AC,((_tmp8C9.f2=_tmp4AB,_tmp8C9)));_npop_handler(0);return _tmp4AE;}case Cyc_Absyn_Neq: _LL2E0: {
struct _tuple16 _tmp8CA;struct _tuple16 _tmp4B0=(_tmp8CA.f1=_tmp4AB,((_tmp8CA.f2=_tmp4AC,_tmp8CA)));_npop_handler(0);return _tmp4B0;}default: _LL2E1: {
struct Cyc_Core_Impossible_exn_struct _tmp8D0;const char*_tmp8CF;struct Cyc_Core_Impossible_exn_struct*_tmp8CE;(int)_throw((void*)((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8D0.tag=Cyc_Core_Impossible,((_tmp8D0.f1=((_tmp8CF="anal_test, zero-split",_tag_dyneither(_tmp8CF,sizeof(char),22))),_tmp8D0)))),_tmp8CE)))));}}}_LL2CB:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp492=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp492->tag != 0)goto _LL2CD;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp493=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp493->tag != 0)goto _LL2CD;};_LL2CC:
# 1759
 if(_tmp452 == Cyc_Absyn_Eq){struct _tuple16 _tmp8D1;struct _tuple16 _tmp4B5=(_tmp8D1.f1=f,((_tmp8D1.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8D1)));_npop_handler(0);return _tmp4B5;}else{
struct _tuple16 _tmp8D2;struct _tuple16 _tmp4B7=(_tmp8D2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D2.f2=f,_tmp8D2)));_npop_handler(0);return _tmp4B7;}_LL2CD:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp494=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp494->tag != 0)goto _LL2CF;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp495=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp495->tag != 1)goto _LL2CF;};_LL2CE:
 goto _LL2D0;_LL2CF:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp496=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp496->tag != 0)goto _LL2D1;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp497=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp497->tag != 2)goto _LL2D1;};_LL2D0:
 goto _LL2D2;_LL2D1:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp498=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp498->tag != 0)goto _LL2D3;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp499=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp499->tag != 5)goto _LL2D3;};_LL2D2:
 goto _LL2D4;_LL2D3:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp49A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp49A->tag != 1)goto _LL2D5;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp49B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp49B->tag != 0)goto _LL2D5;};_LL2D4:
 goto _LL2D6;_LL2D5:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp49C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp49C->tag != 2)goto _LL2D7;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp49D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp49D->tag != 0)goto _LL2D7;};_LL2D6:
 goto _LL2D8;_LL2D7:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp49E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp48B.f1;if(_tmp49E->tag != 5)goto _LL2D9;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp49F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp48B.f2;if(_tmp49F->tag != 0)goto _LL2D9;};_LL2D8:
# 1767
 if(_tmp452 == Cyc_Absyn_Neq){struct _tuple16 _tmp8D3;struct _tuple16 _tmp4B9=(_tmp8D3.f1=f,((_tmp8D3.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8D3)));_npop_handler(0);return _tmp4B9;}else{
struct _tuple16 _tmp8D4;struct _tuple16 _tmp4BB=(_tmp8D4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D4.f2=f,_tmp8D4)));_npop_handler(0);return _tmp4BB;}_LL2D9:;_LL2DA:
 goto _LL2C6;_LL2C6:;};}
# 1777
{struct _tuple0 _tmp8D5;struct _tuple0 _tmp4BD=(_tmp8D5.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp47C->topt)),((_tmp8D5.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp47D->topt)),_tmp8D5)));_LL2E4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f1;if(_tmp4BE->tag != 6)goto _LL2E6;else{if(_tmp4BE->f1 != Cyc_Absyn_Unsigned)goto _LL2E6;}}_LL2E5:
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f2;if(_tmp4BF->tag != 6)goto _LL2E8;else{if(_tmp4BF->f1 != Cyc_Absyn_Unsigned)goto _LL2E8;}}_LL2E7:
 goto _LL2E9;_LL2E8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4BD.f1;if(_tmp4C0->tag != 19)goto _LL2EA;}_LL2E9:
 goto _LL2EB;_LL2EA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4BD.f2;if(_tmp4C1->tag != 19)goto _LL2EC;}_LL2EB:
 goto _LL2E3;_LL2EC:;_LL2ED: {
struct _tuple16 _tmp8D6;struct _tuple16 _tmp4C3=(_tmp8D6.f1=f,((_tmp8D6.f2=f,_tmp8D6)));_npop_handler(0);return _tmp4C3;}_LL2E3:;}
# 1785
switch(_tmp452){case Cyc_Absyn_Eq: _LL2EE:
 goto _LL2EF;case Cyc_Absyn_Neq: _LL2EF:
 goto _LL2F0;case Cyc_Absyn_Gt: _LL2F0:
 goto _LL2F1;case Cyc_Absyn_Gte: _LL2F1: {
struct _tuple16 _tmp8D7;struct _tuple16 _tmp4C5=(_tmp8D7.f1=f,((_tmp8D7.f2=f,_tmp8D7)));_npop_handler(0);return _tmp4C5;}case Cyc_Absyn_Lt: _LL2F2: {
# 1791
union Cyc_CfFlowInfo_FlowInfo _tmp4C6=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4C7=f;
# 1794
{union Cyc_CfFlowInfo_FlowInfo _tmp4C8=_tmp4C6;struct Cyc_Dict_Dict _tmp4C9;_LL2F5: if((_tmp4C8.BottomFL).tag != 1)goto _LL2F7;_LL2F6: {
struct Cyc_Core_Impossible_exn_struct _tmp8DD;const char*_tmp8DC;struct Cyc_Core_Impossible_exn_struct*_tmp8DB;(int)_throw((void*)((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DD.tag=Cyc_Core_Impossible,((_tmp8DD.f1=((_tmp8DC="anal_test, Lt",_tag_dyneither(_tmp8DC,sizeof(char),14))),_tmp8DD)))),_tmp8DB)))));}_LL2F7: if((_tmp4C8.ReachableFL).tag != 2)goto _LL2F4;_tmp4C9=((struct _tuple13)(_tmp4C8.ReachableFL).val).f1;_LL2F8:
 _tmp478=_tmp4C9;_LL2F4:;}{
# 1800
void*_tmp4CD=_tmp47C->r;struct Cyc_Absyn_Vardecl*_tmp4D0;struct Cyc_Absyn_Vardecl*_tmp4D3;struct Cyc_Absyn_Vardecl*_tmp4D6;struct Cyc_Absyn_Vardecl*_tmp4D9;_LL2FA:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CD;if(_tmp4CE->tag != 1)goto _LL2FC;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4CF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4CE->f2);if(_tmp4CF->tag != 1)goto _LL2FC;else{_tmp4D0=_tmp4CF->f1;}}}if(!(!_tmp4D0->escapes))goto _LL2FC;_LL2FB:
# 1802
 _tmp4D3=_tmp4D0;goto _LL2FD;_LL2FC:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CD;if(_tmp4D1->tag != 1)goto _LL2FE;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4D1->f2);if(_tmp4D2->tag != 4)goto _LL2FE;else{_tmp4D3=_tmp4D2->f1;}}}if(!(!_tmp4D3->escapes))goto _LL2FE;_LL2FD:
 _tmp4D6=_tmp4D3;goto _LL2FF;_LL2FE:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CD;if(_tmp4D4->tag != 1)goto _LL300;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4D5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4D4->f2);if(_tmp4D5->tag != 5)goto _LL300;else{_tmp4D6=_tmp4D5->f1;}}}if(!(!_tmp4D6->escapes))goto _LL300;_LL2FF:
 _tmp4D9=_tmp4D6;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CD;if(_tmp4D7->tag != 1)goto _LL302;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4D8=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4D7->f2);if(_tmp4D8->tag != 3)goto _LL302;else{_tmp4D9=_tmp4D8->f1;}}}if(!(!_tmp4D9->escapes))goto _LL302;_LL301: {
# 1806
void*_tmp4DA=_tmp47D->r;struct Cyc_Absyn_Vardecl*_tmp4DD;struct Cyc_Absyn_Vardecl*_tmp4E0;struct Cyc_Absyn_Vardecl*_tmp4E3;struct Cyc_Absyn_Vardecl*_tmp4E6;int _tmp4E8;int _tmp4EB;enum Cyc_Absyn_Primop _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;_LL305:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4DB->tag != 1)goto _LL307;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4DC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4DB->f2);if(_tmp4DC->tag != 1)goto _LL307;else{_tmp4DD=_tmp4DC->f1;}}}if(!(!_tmp4DD->escapes))goto _LL307;_LL306:
 _tmp4E0=_tmp4DD;goto _LL308;_LL307:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4DE->tag != 1)goto _LL309;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4DE->f2);if(_tmp4DF->tag != 4)goto _LL309;else{_tmp4E0=_tmp4DF->f1;}}}if(!(!_tmp4E0->escapes))goto _LL309;_LL308:
 _tmp4E3=_tmp4E0;goto _LL30A;_LL309:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4E1->tag != 1)goto _LL30B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4E2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4E1->f2);if(_tmp4E2->tag != 5)goto _LL30B;else{_tmp4E3=_tmp4E2->f1;}}}if(!(!_tmp4E3->escapes))goto _LL30B;_LL30A:
 _tmp4E6=_tmp4E3;goto _LL30C;_LL30B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4E4->tag != 1)goto _LL30D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4E5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4E4->f2);if(_tmp4E5->tag != 3)goto _LL30D;else{_tmp4E6=_tmp4E5->f1;}}}if(!(!_tmp4E6->escapes))goto _LL30D;_LL30C: {
# 1812
struct _RegionHandle*_tmp4EF=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8E0;struct Cyc_List_List*_tmp8DF;struct Cyc_List_List*_tmp4F0=(_tmp8DF=_region_malloc(_tmp4EF,sizeof(*_tmp8DF)),((_tmp8DF->hd=((_tmp8E0=_region_malloc(_tmp4EF,sizeof(*_tmp8E0)),((_tmp8E0->vd=_tmp4D9,((_tmp8E0->rop=Cyc_CfFlowInfo_LessVar(_tmp4E6,_tmp4E6->type),_tmp8E0)))))),((_tmp8DF->tl=_tmp479,_tmp8DF)))));
struct _tuple16 _tmp8E1;struct _tuple16 _tmp4F2=(_tmp8E1.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp4F0),((_tmp8E1.f2=_tmp4C7,_tmp8E1)));_npop_handler(0);return _tmp4F2;}_LL30D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4E7->tag != 0)goto _LL30F;else{if(((_tmp4E7->f1).Int_c).tag != 5)goto _LL30F;_tmp4E8=((struct _tuple6)((_tmp4E7->f1).Int_c).val).f2;}}_LL30E:
 _tmp4EB=_tmp4E8;goto _LL310;_LL30F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4E9->tag != 13)goto _LL311;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4EA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmp4E9->f2)->r;if(_tmp4EA->tag != 0)goto _LL311;else{if(((_tmp4EA->f1).Int_c).tag != 5)goto _LL311;_tmp4EB=((struct _tuple6)((_tmp4EA->f1).Int_c).val).f2;}}}_LL310: {
# 1817
struct _RegionHandle*_tmp4F5=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8E4;struct Cyc_List_List*_tmp8E3;struct Cyc_List_List*_tmp4F6=(_tmp8E3=_region_malloc(_tmp4F5,sizeof(*_tmp8E3)),((_tmp8E3->hd=((_tmp8E4=_region_malloc(_tmp4F5,sizeof(*_tmp8E4)),((_tmp8E4->vd=_tmp4D9,((_tmp8E4->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp4EB),_tmp8E4)))))),((_tmp8E3->tl=_tmp479,_tmp8E3)))));
struct _tuple16 _tmp8E5;struct _tuple16 _tmp4F8=(_tmp8E5.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp4F6),((_tmp8E5.f2=_tmp4C7,_tmp8E5)));_npop_handler(0);return _tmp4F8;}_LL311: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4DA;if(_tmp4EC->tag != 2)goto _LL313;else{_tmp4ED=_tmp4EC->f1;if(_tmp4EC->f2 == 0)goto _LL313;_tmp4EE=(struct Cyc_Absyn_Exp*)(_tmp4EC->f2)->hd;}}_LL312: {
# 1821
void*_tmp4FB=_tmp4EE->r;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp501;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Vardecl*_tmp507;_LL316:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp4FC->tag != 1)goto _LL318;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4FD=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4FC->f2);if(_tmp4FD->tag != 1)goto _LL318;else{_tmp4FE=_tmp4FD->f1;}}}if(!(!_tmp4FE->escapes))goto _LL318;_LL317:
 _tmp501=_tmp4FE;goto _LL319;_LL318:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp4FF->tag != 1)goto _LL31A;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp500=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4FF->f2);if(_tmp500->tag != 4)goto _LL31A;else{_tmp501=_tmp500->f1;}}}if(!(!_tmp501->escapes))goto _LL31A;_LL319:
 _tmp504=_tmp501;goto _LL31B;_LL31A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp502->tag != 1)goto _LL31C;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp503=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp502->f2);if(_tmp503->tag != 5)goto _LL31C;else{_tmp504=_tmp503->f1;}}}if(!(!_tmp504->escapes))goto _LL31C;_LL31B:
 _tmp507=_tmp504;goto _LL31D;_LL31C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp505->tag != 1)goto _LL31E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp506=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp505->f2);if(_tmp506->tag != 3)goto _LL31E;else{_tmp507=_tmp506->f1;}}}if(!(!_tmp507->escapes))goto _LL31E;_LL31D: {
# 1827
struct _RegionHandle*_tmp508=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8E8;struct Cyc_List_List*_tmp8E7;struct Cyc_List_List*_tmp509=(_tmp8E7=_region_malloc(_tmp508,sizeof(*_tmp8E7)),((_tmp8E7->hd=((_tmp8E8=_region_malloc(_tmp508,sizeof(*_tmp8E8)),((_tmp8E8->vd=_tmp4D9,((_tmp8E8->rop=Cyc_CfFlowInfo_LessNumelts(_tmp507),_tmp8E8)))))),((_tmp8E7->tl=_tmp479,_tmp8E7)))));
struct _tuple16 _tmp8E9;struct _tuple16 _tmp50B=(_tmp8E9.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp509),((_tmp8E9.f2=_tmp4C7,_tmp8E9)));_npop_handler(0);return _tmp50B;}_LL31E:;_LL31F: {
struct _tuple16 _tmp8EA;struct _tuple16 _tmp50F=(_tmp8EA.f1=_tmp4C6,((_tmp8EA.f2=_tmp4C7,_tmp8EA)));_npop_handler(0);return _tmp50F;}_LL315:;}_LL313:;_LL314: {
# 1832
struct _tuple16 _tmp8EB;struct _tuple16 _tmp511=(_tmp8EB.f1=_tmp4C6,((_tmp8EB.f2=_tmp4C7,_tmp8EB)));_npop_handler(0);return _tmp511;}_LL304:;}_LL302:;_LL303: {
# 1834
struct _tuple16 _tmp8EC;struct _tuple16 _tmp513=(_tmp8EC.f1=_tmp4C6,((_tmp8EC.f2=_tmp4C7,_tmp8EC)));_npop_handler(0);return _tmp513;}_LL2F9:;};}case Cyc_Absyn_Lte: _LL2F3: {
# 1838
union Cyc_CfFlowInfo_FlowInfo _tmp514=f;
union Cyc_CfFlowInfo_FlowInfo _tmp515=f;
# 1841
{union Cyc_CfFlowInfo_FlowInfo _tmp516=_tmp514;struct Cyc_Dict_Dict _tmp517;_LL322: if((_tmp516.BottomFL).tag != 1)goto _LL324;_LL323: {
struct Cyc_Core_Impossible_exn_struct _tmp8F2;const char*_tmp8F1;struct Cyc_Core_Impossible_exn_struct*_tmp8F0;(int)_throw((void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F2.tag=Cyc_Core_Impossible,((_tmp8F2.f1=((_tmp8F1="anal_test, Lte",_tag_dyneither(_tmp8F1,sizeof(char),15))),_tmp8F2)))),_tmp8F0)))));}_LL324: if((_tmp516.ReachableFL).tag != 2)goto _LL321;_tmp517=((struct _tuple13)(_tmp516.ReachableFL).val).f1;_LL325:
 _tmp478=_tmp517;_LL321:;}{
# 1847
void*_tmp51B=_tmp47C->r;struct Cyc_Absyn_Vardecl*_tmp51E;struct Cyc_Absyn_Vardecl*_tmp521;struct Cyc_Absyn_Vardecl*_tmp524;struct Cyc_Absyn_Vardecl*_tmp527;_LL327:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51B;if(_tmp51C->tag != 1)goto _LL329;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp51D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp51C->f2);if(_tmp51D->tag != 1)goto _LL329;else{_tmp51E=_tmp51D->f1;}}}if(!(!_tmp51E->escapes))goto _LL329;_LL328:
# 1849
 _tmp521=_tmp51E;goto _LL32A;_LL329:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51B;if(_tmp51F->tag != 1)goto _LL32B;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp520=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp51F->f2);if(_tmp520->tag != 4)goto _LL32B;else{_tmp521=_tmp520->f1;}}}if(!(!_tmp521->escapes))goto _LL32B;_LL32A:
 _tmp524=_tmp521;goto _LL32C;_LL32B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51B;if(_tmp522->tag != 1)goto _LL32D;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp523=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp522->f2);if(_tmp523->tag != 5)goto _LL32D;else{_tmp524=_tmp523->f1;}}}if(!(!_tmp524->escapes))goto _LL32D;_LL32C:
 _tmp527=_tmp524;goto _LL32E;_LL32D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51B;if(_tmp525->tag != 1)goto _LL32F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp526=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp525->f2);if(_tmp526->tag != 3)goto _LL32F;else{_tmp527=_tmp526->f1;}}}if(!(!_tmp527->escapes))goto _LL32F;_LL32E: {
# 1853
void*_tmp528=_tmp47D->r;int _tmp52A;int _tmp52D;enum Cyc_Absyn_Primop _tmp52F;struct Cyc_Absyn_Exp*_tmp530;_LL332: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp529=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp528;if(_tmp529->tag != 0)goto _LL334;else{if(((_tmp529->f1).Int_c).tag != 5)goto _LL334;_tmp52A=((struct _tuple6)((_tmp529->f1).Int_c).val).f2;}}_LL333:
 _tmp52D=_tmp52A;goto _LL335;_LL334: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp528;if(_tmp52B->tag != 13)goto _LL336;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmp52B->f2)->r;if(_tmp52C->tag != 0)goto _LL336;else{if(((_tmp52C->f1).Int_c).tag != 5)goto _LL336;_tmp52D=((struct _tuple6)((_tmp52C->f1).Int_c).val).f2;}}}_LL335: {
# 1856
struct _RegionHandle*_tmp531=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8F5;struct Cyc_List_List*_tmp8F4;struct Cyc_List_List*_tmp532=(_tmp8F4=_region_malloc(_tmp531,sizeof(*_tmp8F4)),((_tmp8F4->hd=(
(_tmp8F5=_region_malloc(_tmp531,sizeof(*_tmp8F5)),((_tmp8F5->vd=_tmp527,((_tmp8F5->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)_tmp52D),_tmp8F5)))))),((_tmp8F4->tl=_tmp479,_tmp8F4)))));
struct _tuple16 _tmp8F6;struct _tuple16 _tmp534=(_tmp8F6.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp532),((_tmp8F6.f2=_tmp515,_tmp8F6)));_npop_handler(0);return _tmp534;}_LL336: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp528;if(_tmp52E->tag != 2)goto _LL338;else{_tmp52F=_tmp52E->f1;if(_tmp52E->f2 == 0)goto _LL338;_tmp530=(struct Cyc_Absyn_Exp*)(_tmp52E->f2)->hd;}}_LL337: {
# 1861
void*_tmp537=_tmp530->r;struct Cyc_Absyn_Vardecl*_tmp53A;struct Cyc_Absyn_Vardecl*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp540;struct Cyc_Absyn_Vardecl*_tmp543;_LL33B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp538=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp537;if(_tmp538->tag != 1)goto _LL33D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp539=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp538->f2);if(_tmp539->tag != 1)goto _LL33D;else{_tmp53A=_tmp539->f1;}}}if(!(!_tmp53A->escapes))goto _LL33D;_LL33C:
 _tmp53D=_tmp53A;goto _LL33E;_LL33D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp537;if(_tmp53B->tag != 1)goto _LL33F;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp53C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp53B->f2);if(_tmp53C->tag != 4)goto _LL33F;else{_tmp53D=_tmp53C->f1;}}}if(!(!_tmp53D->escapes))goto _LL33F;_LL33E:
 _tmp540=_tmp53D;goto _LL340;_LL33F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp537;if(_tmp53E->tag != 1)goto _LL341;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp53F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp53E->f2);if(_tmp53F->tag != 5)goto _LL341;else{_tmp540=_tmp53F->f1;}}}if(!(!_tmp540->escapes))goto _LL341;_LL340:
 _tmp543=_tmp540;goto _LL342;_LL341:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp537;if(_tmp541->tag != 1)goto _LL343;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp542=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp541->f2);if(_tmp542->tag != 3)goto _LL343;else{_tmp543=_tmp542->f1;}}}if(!(!_tmp543->escapes))goto _LL343;_LL342: {
# 1867
struct Cyc_CfFlowInfo_FlowEnv*_tmp544=env->fenv;
struct Cyc_CfFlowInfo_Reln*_tmp8F9;struct Cyc_List_List*_tmp8F8;struct Cyc_List_List*_tmp545=(_tmp8F8=_region_malloc(_tmp544->r,sizeof(*_tmp8F8)),((_tmp8F8->hd=((_tmp8F9=_region_malloc(_tmp544->r,sizeof(*_tmp8F9)),((_tmp8F9->vd=_tmp527,((_tmp8F9->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp543),_tmp8F9)))))),((_tmp8F8->tl=_tmp479,_tmp8F8)))));
struct _tuple16 _tmp8FA;struct _tuple16 _tmp547=(_tmp8FA.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp545),((_tmp8FA.f2=_tmp515,_tmp8FA)));_npop_handler(0);return _tmp547;}_LL343:;_LL344: {
struct _tuple16 _tmp8FB;struct _tuple16 _tmp54B=(_tmp8FB.f1=_tmp514,((_tmp8FB.f2=_tmp515,_tmp8FB)));_npop_handler(0);return _tmp54B;}_LL33A:;}_LL338:;_LL339: {
# 1872
struct _tuple16 _tmp8FC;struct _tuple16 _tmp54D=(_tmp8FC.f1=_tmp514,((_tmp8FC.f2=_tmp515,_tmp8FC)));_npop_handler(0);return _tmp54D;}_LL331:;}_LL32F:;_LL330: {
# 1874
struct _tuple16 _tmp8FD;struct _tuple16 _tmp54F=(_tmp8FD.f1=_tmp514,((_tmp8FD.f2=_tmp515,_tmp8FD)));_npop_handler(0);return _tmp54F;}_LL326:;};}default: _LL320: {
# 1876
struct _tuple16 _tmp8FE;struct _tuple16 _tmp551=(_tmp8FE.f1=f,((_tmp8FE.f2=f,_tmp8FE)));_npop_handler(0);return _tmp551;}}}_LL2B7:;};
# 1710
;_pop_region(temp);}_LL2B5:;_LL2B6:
# 1879
 goto _LL2A8;_LL2A8:;}{
# 1883
union Cyc_CfFlowInfo_FlowInfo _tmp553;void*_tmp554;struct _tuple14 _tmp552=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp553=_tmp552.f1;_tmp554=_tmp552.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp555=_tmp553;struct Cyc_Dict_Dict _tmp556;_LL347: if((_tmp555.BottomFL).tag != 1)goto _LL349;_LL348: {
struct _tuple16 _tmp8FF;return(_tmp8FF.f1=_tmp553,((_tmp8FF.f2=_tmp553,_tmp8FF)));}_LL349: if((_tmp555.ReachableFL).tag != 2)goto _LL346;_tmp556=((struct _tuple13)(_tmp555.ReachableFL).val).f1;_LL34A: {
# 1887
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp558=_tmp554;struct Cyc_Absyn_Vardecl*_tmp55A;void*_tmp55B;enum Cyc_CfFlowInfo_InitLevel _tmp564;_LL34C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp559=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp559->tag != 8)goto _LL34E;else{_tmp55A=_tmp559->f1;_tmp55B=(void*)_tmp559->f2;}}_LL34D:
 _tmp554=_tmp55B;locname=_tmp55A;goto retry;_LL34E: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp55C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp55C->tag != 0)goto _LL350;}_LL34F: {
struct _tuple16 _tmp900;return(_tmp900.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp900.f2=_tmp553,_tmp900)));}_LL350: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp55D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp55D->tag != 2)goto _LL352;}_LL351:
 goto _LL353;_LL352: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp55E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp55E->tag != 1)goto _LL354;}_LL353:
 goto _LL355;_LL354: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp55F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp55F->tag != 5)goto _LL356;}_LL355: {
struct _tuple16 _tmp901;return(_tmp901.f1=_tmp553,((_tmp901.f2=Cyc_CfFlowInfo_BottomFL(),_tmp901)));}_LL356: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp560=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp560->tag != 3)goto _LL358;else{if(_tmp560->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL358;}}_LL357:
 goto _LL359;_LL358: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp561=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp561->tag != 4)goto _LL35A;else{if(_tmp561->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL35A;}}_LL359:
 goto _LL35B;_LL35A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp562=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp562->tag != 7)goto _LL35C;}_LL35B:
# 1897
{const char*_tmp904;void*_tmp903;(_tmp903=0,Cyc_Tcutil_terr(e->loc,((_tmp904="expression may not be initialized",_tag_dyneither(_tmp904,sizeof(char),34))),_tag_dyneither(_tmp903,sizeof(void*),0)));}{
struct _tuple16 _tmp905;return(_tmp905.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp905.f2=Cyc_CfFlowInfo_BottomFL(),_tmp905)));};_LL35C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp563=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp563->tag != 3)goto _LL35E;else{_tmp564=_tmp563->f1;}}_LL35D:
# 1900
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp553,e,_tmp564,locname);_LL35E: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp565=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp565->tag != 4)goto _LL360;}_LL35F: {
struct _tuple16 _tmp906;return(_tmp906.f1=_tmp553,((_tmp906.f2=_tmp553,_tmp906)));}_LL360: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp566=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp558;if(_tmp566->tag != 6)goto _LL34B;}_LL361: {
struct Cyc_Core_Impossible_exn_struct _tmp90C;const char*_tmp90B;struct Cyc_Core_Impossible_exn_struct*_tmp90A;(int)_throw((void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90C.tag=Cyc_Core_Impossible,((_tmp90C.f1=((_tmp90B="anal_test",_tag_dyneither(_tmp90B,sizeof(char),10))),_tmp90C)))),_tmp90A)))));}_LL34B:;}}_LL346:;};};}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1907
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1909
unsigned int _tmp571;struct Cyc_NewControlFlow_AnalEnv*_tmp572;struct Cyc_Dict_Dict _tmp573;struct _tuple23*_tmp570=ckenv;_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;_tmp573=_tmp570->f3;{
void*_tmp574=root;struct Cyc_Absyn_Vardecl*_tmp576;_LL363: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp575=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp574;if(_tmp575->tag != 0)goto _LL365;else{_tmp576=_tmp575->f1;}}_LL364:
# 1912
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp572->fenv)->r,_tmp576->type)){
void*_tmp577=rval;_LL368: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp578=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp578->tag != 8)goto _LL36A;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp579=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp578->f2);if(_tmp579->tag != 7)goto _LL36A;}}_LL369:
 goto _LL36B;_LL36A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp57A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57A->tag != 8)goto _LL36C;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp57B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp57A->f2);if(_tmp57B->tag != 0)goto _LL36C;}}_LL36B:
 goto _LL36D;_LL36C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp57C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57C->tag != 8)goto _LL36E;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp57D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp57C->f2);if(_tmp57D->tag != 3)goto _LL36E;else{if(_tmp57D->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL36E;}}}_LL36D:
 goto _LL36F;_LL36E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp57E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57E->tag != 7)goto _LL370;}_LL36F:
 goto _LL371;_LL370: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp57F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57F->tag != 0)goto _LL372;}_LL371:
 goto _LL373;_LL372: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp580=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp580->tag != 3)goto _LL374;else{if(_tmp580->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL374;}}_LL373:
 goto _LL367;_LL374:;_LL375:
# 1921
{const char*_tmp910;void*_tmp90F[1];struct Cyc_String_pa_PrintArg_struct _tmp90E;(_tmp90E.tag=0,((_tmp90E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp576->name)),((_tmp90F[0]=& _tmp90E,Cyc_Tcutil_warn(_tmp571,((_tmp910="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp910,sizeof(char),57))),_tag_dyneither(_tmp90F,sizeof(void*),1)))))));}
goto _LL367;_LL367:;}
# 1924
goto _LL362;_LL365:;_LL366:
 goto _LL362;_LL362:;};}
# 1929
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp584=flow;struct Cyc_Dict_Dict _tmp585;_LL377: if((_tmp584.BottomFL).tag != 1)goto _LL379;_LL378:
 return;_LL379: if((_tmp584.ReachableFL).tag != 2)goto _LL376;_tmp585=((struct _tuple13)(_tmp584.ReachableFL).val).f1;_LL37A:
# 1933
{struct Cyc_List_List*_tmp586=env->param_roots;for(0;_tmp586 != 0;_tmp586=_tmp586->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp585,Cyc_CfFlowInfo_lookup_place(_tmp585,(struct Cyc_CfFlowInfo_Place*)_tmp586->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp913;void*_tmp912;(_tmp912=0,Cyc_Tcutil_terr(loc,((_tmp913="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp913,sizeof(char),76))),_tag_dyneither(_tmp912,sizeof(void*),0)));}}}
# 1937
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp914;struct _tuple23 _tmp589=(_tmp914.f1=loc,((_tmp914.f2=env,((_tmp914.f3=_tmp585,_tmp914)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp589,_tmp585);}
# 1941
return;_LL376:;}struct _tuple24{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple25{void*f1;void*f2;void*f3;};
# 1945
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1947
union Cyc_CfFlowInfo_FlowInfo _tmp58B=flow;struct Cyc_Dict_Dict _tmp58C;_LL37C: if((_tmp58B.BottomFL).tag != 1)goto _LL37E;_LL37D:
 return;_LL37E: if((_tmp58B.ReachableFL).tag != 2)goto _LL37B;_tmp58C=((struct _tuple13)(_tmp58B.ReachableFL).val).f1;_LL37F: {
# 1950
struct Cyc_Absyn_Exp*_tmp58D=Cyc_Absyn_exp_unsigned_one;
int _tmp58E=0;
int _tmp58F=1;
{struct Cyc_List_List*_tmp590=env->noconsume_params;for(0;_tmp590 != 0;_tmp590=_tmp590->tl){
# 1956
struct Cyc_CfFlowInfo_Place*_tmp592;void*_tmp593;struct _tuple24*_tmp591=(struct _tuple24*)_tmp590->hd;_tmp592=_tmp591->f1;_tmp593=_tmp591->f2;{
void*_tmp594=Cyc_CfFlowInfo_lookup_place(_tmp58C,_tmp592);
void*_tmp596;struct Cyc_CfFlowInfo_Place*_tmp595=_tmp592;_tmp596=_tmp595->root;{
struct _tuple25 _tmp915;struct _tuple25 _tmp598=(_tmp915.f1=_tmp596,((_tmp915.f2=_tmp593,((_tmp915.f3=_tmp594,_tmp915)))));struct Cyc_Absyn_Vardecl*_tmp59A;struct Cyc_Absyn_Vardecl*_tmp59C;void*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59F;void*_tmp5A0;struct Cyc_Absyn_Vardecl*_tmp5A2;_LL381:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp599=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp598.f1;if(_tmp599->tag != 0)goto _LL383;else{_tmp59A=_tmp599->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp59B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp598.f2;if(_tmp59B->tag != 8)goto _LL383;else{_tmp59C=_tmp59B->f1;_tmp59D=(void*)_tmp59B->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp59E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp598.f3;if(_tmp59E->tag != 8)goto _LL383;else{_tmp59F=_tmp59E->f1;_tmp5A0=(void*)_tmp59E->f2;}};_LL382:
# 1961
 if(_tmp59C == _tmp59F){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp58D,_tmp58F,_tmp5A0,& _tmp58E)){
const char*_tmp919;void*_tmp918[1];struct Cyc_String_pa_PrintArg_struct _tmp917;(_tmp917.tag=0,((_tmp917.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1965
Cyc_Absynpp_qvar2string(_tmp59A->name)),((_tmp918[0]=& _tmp917,Cyc_Tcutil_terr(loc,((_tmp919="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp919,sizeof(char),58))),_tag_dyneither(_tmp918,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp58C,_tmp5A0)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp91D;void*_tmp91C[1];struct Cyc_String_pa_PrintArg_struct _tmp91B;(_tmp91B.tag=0,((_tmp91B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1969
Cyc_Absynpp_qvar2string(_tmp59A->name)),((_tmp91C[0]=& _tmp91B,Cyc_Tcutil_terr(loc,((_tmp91D="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp91D,sizeof(char),84))),_tag_dyneither(_tmp91C,sizeof(void*),1)))))));}}}else{
# 1973
_tmp5A2=_tmp59A;goto _LL384;}
goto _LL380;_LL383: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5A1=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp598.f1;if(_tmp5A1->tag != 0)goto _LL385;else{_tmp5A2=_tmp5A1->f1;}}_LL384:
# 1976
{const char*_tmp921;void*_tmp920[1];struct Cyc_String_pa_PrintArg_struct _tmp91F;(_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1978
Cyc_Absynpp_qvar2string(_tmp5A2->name)),((_tmp920[0]=& _tmp91F,Cyc_Tcutil_terr(loc,((_tmp921="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp921,sizeof(char),85))),_tag_dyneither(_tmp920,sizeof(void*),1)))))));}
goto _LL380;_LL385:;_LL386: {
# 1981
const char*_tmp924;void*_tmp923;(_tmp923=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp924="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp924,sizeof(char),46))),_tag_dyneither(_tmp923,sizeof(void*),0)));}_LL380:;};};}}
# 1984
return;}_LL37B:;}
# 1991
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1993
struct Cyc_CfFlowInfo_FlowEnv*_tmp5AE=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Stmt*_tmp5B2;unsigned int _tmp5B3;struct Cyc_Absyn_Switch_clause*_tmp5AF=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp5B0=_tmp5AF->pat_vars;_tmp5B1=_tmp5AF->where_clause;_tmp5B2=_tmp5AF->body;_tmp5B3=_tmp5AF->loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5AE,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5B0))->v,_tmp5AE->unknown_all,_tmp5B3,0);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp5B1 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5B1;
union Cyc_CfFlowInfo_FlowInfo _tmp5B5;union Cyc_CfFlowInfo_FlowInfo _tmp5B6;struct _tuple16 _tmp5B4=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;
inflow=_tmp5B6;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5B5,_tmp5B2);}else{
# 2004
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5B2);}{
# 2006
union Cyc_CfFlowInfo_FlowInfo _tmp5B7=clause_outflow;_LL388: if((_tmp5B7.BottomFL).tag != 1)goto _LL38A;_LL389:
 goto _LL387;_LL38A:;_LL38B:
# 2010
 if(scs->tl == 0)
return clause_outflow;else{
# 2015
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_Tcutil_terr(_tmp5B2->loc,((_tmp927="switch clause may implicitly fallthru",_tag_dyneither(_tmp927,sizeof(char),38))),_tag_dyneither(_tmp926,sizeof(void*),0)));}else{
# 2018
const char*_tmp92A;void*_tmp929;(_tmp929=0,Cyc_Tcutil_warn(_tmp5B2->loc,((_tmp92A="switch clause may implicitly fallthru",_tag_dyneither(_tmp92A,sizeof(char),38))),_tag_dyneither(_tmp929,sizeof(void*),0)));}
# 2020
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2022
goto _LL387;_LL387:;};};}
# 2025
return Cyc_CfFlowInfo_BottomFL();}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2030
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple26*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2032
struct Cyc_NewControlFlow_AnalEnv*_tmp5BD;struct Cyc_Dict_Dict _tmp5BE;unsigned int _tmp5BF;struct _tuple26*_tmp5BC=vdenv;_tmp5BD=_tmp5BC->f1;_tmp5BE=_tmp5BC->f2;_tmp5BF=_tmp5BC->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5BD->fenv)->r,vd->type)){
# 2035
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp92B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp92B.tag=0,((_tmp92B.f1=vd,_tmp92B)));
# 2037
void*_tmp5C0=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5BE,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp5C1=_tmp5C0;_LL38D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C2->tag != 8)goto _LL38F;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5C3=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C2->f2);if(_tmp5C3->tag != 7)goto _LL38F;}}_LL38E:
 goto _LL390;_LL38F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C4->tag != 8)goto _LL391;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5C5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C4->f2);if(_tmp5C5->tag != 0)goto _LL391;}}_LL390:
 goto _LL392;_LL391: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C6->tag != 8)goto _LL393;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C6->f2);if(_tmp5C7->tag != 3)goto _LL393;else{if(_tmp5C7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL393;}}}_LL392:
 goto _LL394;_LL393: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5C8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C8->tag != 7)goto _LL395;}_LL394:
 goto _LL396;_LL395: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5C9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C9->tag != 0)goto _LL397;}_LL396:
 goto _LL398;_LL397: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5CA=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5CA->tag != 3)goto _LL399;else{if(_tmp5CA->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL399;}}_LL398:
 goto _LL38C;_LL399:;_LL39A:
# 2046
{const char*_tmp92F;void*_tmp92E[1];struct Cyc_String_pa_PrintArg_struct _tmp92D;(_tmp92D.tag=0,((_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp92E[0]=& _tmp92D,Cyc_Tcutil_warn(_tmp5BF,((_tmp92F="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp92F,sizeof(char),68))),_tag_dyneither(_tmp92E,sizeof(void*),1)))))));}
# 2048
goto _LL38C;_LL38C:;}}
# 2053
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2055
{union Cyc_CfFlowInfo_FlowInfo _tmp5CF=inflow;struct Cyc_Dict_Dict _tmp5D0;_LL39C: if((_tmp5CF.ReachableFL).tag != 2)goto _LL39E;_tmp5D0=((struct _tuple13)(_tmp5CF.ReachableFL).val).f1;_LL39D: {
# 2057
struct _tuple26 _tmp930;struct _tuple26 _tmp5D1=(_tmp930.f1=env,((_tmp930.f2=_tmp5D0,((_tmp930.f3=decl->loc,_tmp930)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5D2=env->fenv;
{void*_tmp5D3=decl->r;struct Cyc_Absyn_Vardecl*_tmp5D5;struct Cyc_List_List*_tmp5D7;struct Cyc_List_List*_tmp5D9;_LL3A1: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5D4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D4->tag != 0)goto _LL3A3;else{_tmp5D5=_tmp5D4->f1;}}_LL3A2:
# 2061
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5D1,_tmp5D5);
goto _LL3A0;_LL3A3: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5D6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D6->tag != 2)goto _LL3A5;else{if(_tmp5D6->f2 == 0)goto _LL3A5;_tmp5D7=(struct Cyc_List_List*)(_tmp5D6->f2)->v;}}_LL3A4:
 _tmp5D9=_tmp5D7;goto _LL3A6;_LL3A5: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5D8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D8->tag != 3)goto _LL3A7;else{_tmp5D9=_tmp5D8->f1;}}_LL3A6:
# 2065
((void(*)(void(*f)(struct _tuple26*,struct Cyc_Absyn_Vardecl*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5D1,_tmp5D9);
goto _LL3A0;_LL3A7:;_LL3A8:
 goto _LL3A0;_LL3A0:;}
# 2069
goto _LL39B;}_LL39E:;_LL39F:
 goto _LL39B;_LL39B:;}
# 2072
return;}
# 2075
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2080
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5DC;union Cyc_CfFlowInfo_FlowInfo*_tmp5DD;struct _tuple17 _tmp5DB=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp5DC=_tmp5DB.f1;_tmp5DD=_tmp5DB.f2;
inflow=*_tmp5DD;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp5DE=env->fenv;
void*_tmp5DF=s->r;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_Stmt*_tmp5E7;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5EE;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_Absyn_Stmt*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FA;struct Cyc_Absyn_Stmt*_tmp5FB;struct Cyc_List_List*_tmp5FE;struct Cyc_Absyn_Switch_clause*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_List_List*_tmp608;struct Cyc_Absyn_Stmt*_tmp60A;struct Cyc_List_List*_tmp60B;struct Cyc_Absyn_Tvar*_tmp60E;struct Cyc_Absyn_Vardecl*_tmp60F;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_Absyn_Decl*_tmp612;struct Cyc_Absyn_Stmt*_tmp613;struct Cyc_Absyn_Stmt*_tmp615;struct Cyc_Absyn_Exp*_tmp617;_LL3AA: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp5E0=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E0->tag != 0)goto _LL3AC;}_LL3AB:
 return inflow;_LL3AC: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5E1=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E1->tag != 3)goto _LL3AE;else{if(_tmp5E1->f1 != 0)goto _LL3AE;}}_LL3AD:
# 2086
 if(env->noreturn){
const char*_tmp933;void*_tmp932;(_tmp932=0,Cyc_Tcutil_terr(s->loc,((_tmp933="`noreturn' function might return",_tag_dyneither(_tmp933,sizeof(char),33))),_tag_dyneither(_tmp932,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3AE: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5E2=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E2->tag != 3)goto _LL3B0;else{_tmp5E3=_tmp5E2->f1;}}_LL3AF:
# 2092
 if(env->noreturn){
const char*_tmp936;void*_tmp935;(_tmp935=0,Cyc_Tcutil_terr(s->loc,((_tmp936="`noreturn' function might return",_tag_dyneither(_tmp936,sizeof(char),33))),_tag_dyneither(_tmp935,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp61D;void*_tmp61E;struct _tuple14 _tmp61C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5E3));_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;
_tmp61D=Cyc_NewControlFlow_use_Rval(env,_tmp5E3->loc,_tmp61D,_tmp61E);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp61D);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp61D);
return Cyc_CfFlowInfo_BottomFL();};_LL3B0: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp5E4=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E4->tag != 1)goto _LL3B2;else{_tmp5E5=_tmp5E4->f1;}}_LL3B1:
# 2100
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5E5)).f1;_LL3B2: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp5E6=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E6->tag != 2)goto _LL3B4;else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;}}_LL3B3:
# 2103
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E7),_tmp5E8);_LL3B4: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp5E9=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5E9->tag != 4)goto _LL3B6;else{_tmp5EA=_tmp5E9->f1;_tmp5EB=_tmp5E9->f2;_tmp5EC=_tmp5E9->f3;}}_LL3B5: {
# 2106
union Cyc_CfFlowInfo_FlowInfo _tmp620;union Cyc_CfFlowInfo_FlowInfo _tmp621;struct _tuple16 _tmp61F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5EA);_tmp620=_tmp61F.f1;_tmp621=_tmp61F.f2;{
# 2113
union Cyc_CfFlowInfo_FlowInfo _tmp622=Cyc_NewControlFlow_anal_stmt(env,_tmp621,_tmp5EC);
union Cyc_CfFlowInfo_FlowInfo _tmp623=Cyc_NewControlFlow_anal_stmt(env,_tmp620,_tmp5EB);
return Cyc_CfFlowInfo_join_flow(_tmp5DE,env->all_changed,_tmp623,_tmp622);};}_LL3B6: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp5ED=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5ED->tag != 5)goto _LL3B8;else{_tmp5EE=(_tmp5ED->f1).f1;_tmp5EF=(_tmp5ED->f1).f2;_tmp5F0=_tmp5ED->f2;}}_LL3B7: {
# 2121
union Cyc_CfFlowInfo_FlowInfo*_tmp625;struct _tuple17 _tmp624=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5EF);_tmp625=_tmp624.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp626=*_tmp625;
union Cyc_CfFlowInfo_FlowInfo _tmp628;union Cyc_CfFlowInfo_FlowInfo _tmp629;struct _tuple16 _tmp627=Cyc_NewControlFlow_anal_test(env,_tmp626,_tmp5EE);_tmp628=_tmp627.f1;_tmp629=_tmp627.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp62A=Cyc_NewControlFlow_anal_stmt(env,_tmp628,_tmp5F0);
Cyc_NewControlFlow_update_flow(env,_tmp5EF,_tmp62A);
return _tmp629;};};}_LL3B8: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp5F1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5F1->tag != 14)goto _LL3BA;else{_tmp5F2=_tmp5F1->f1;_tmp5F3=(_tmp5F1->f2).f1;_tmp5F4=(_tmp5F1->f2).f2;}}_LL3B9: {
# 2131
union Cyc_CfFlowInfo_FlowInfo _tmp62B=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5F2);
union Cyc_CfFlowInfo_FlowInfo*_tmp62D;struct _tuple17 _tmp62C=Cyc_NewControlFlow_pre_stmt_check(env,_tmp62B,_tmp5F4);_tmp62D=_tmp62C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp62E=*_tmp62D;
union Cyc_CfFlowInfo_FlowInfo _tmp630;union Cyc_CfFlowInfo_FlowInfo _tmp631;struct _tuple16 _tmp62F=Cyc_NewControlFlow_anal_test(env,_tmp62E,_tmp5F3);_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;
Cyc_NewControlFlow_update_flow(env,_tmp5F2,_tmp630);
return _tmp631;};}_LL3BA: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp5F5=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5F5->tag != 9)goto _LL3BC;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=(_tmp5F5->f2).f1;_tmp5F8=(_tmp5F5->f2).f2;_tmp5F9=(_tmp5F5->f3).f1;_tmp5FA=(_tmp5F5->f3).f2;_tmp5FB=_tmp5F5->f4;}}_LL3BB: {
# 2140
union Cyc_CfFlowInfo_FlowInfo _tmp632=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5F6)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp634;struct _tuple17 _tmp633=Cyc_NewControlFlow_pre_stmt_check(env,_tmp632,_tmp5F8);_tmp634=_tmp633.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp635=*_tmp634;
union Cyc_CfFlowInfo_FlowInfo _tmp637;union Cyc_CfFlowInfo_FlowInfo _tmp638;struct _tuple16 _tmp636=Cyc_NewControlFlow_anal_test(env,_tmp635,_tmp5F7);_tmp637=_tmp636.f1;_tmp638=_tmp636.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp639=Cyc_NewControlFlow_anal_stmt(env,_tmp637,_tmp5FB);
union Cyc_CfFlowInfo_FlowInfo*_tmp63B;struct _tuple17 _tmp63A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp639,_tmp5FA);_tmp63B=_tmp63A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp63C=*_tmp63B;
union Cyc_CfFlowInfo_FlowInfo _tmp63D=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp63C,_tmp5F9)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5F8,_tmp63D);
return _tmp638;};};};}_LL3BC: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5FC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5FC->tag != 6)goto _LL3BE;else{if(_tmp5FC->f1 != 0)goto _LL3BE;}}_LL3BD:
# 2152
 return Cyc_CfFlowInfo_BottomFL();_LL3BE: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp5FD=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp5FD->tag != 11)goto _LL3C0;else{_tmp5FE=_tmp5FD->f1;if(_tmp5FD->f2 == 0)goto _LL3C0;_tmp5FF=*_tmp5FD->f2;}}_LL3BF: {
# 2154
struct _RegionHandle _tmp63E=_new_region("temp");struct _RegionHandle*temp=& _tmp63E;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp640;struct Cyc_List_List*_tmp641;struct _tuple20 _tmp63F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp5FE,1,1);_tmp640=_tmp63F.f1;_tmp641=_tmp63F.f2;
# 2157
for(0;_tmp641 != 0;(_tmp641=_tmp641->tl,_tmp5FE=_tmp5FE->tl)){
_tmp640=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5FE))->hd)->loc,_tmp640,(void*)_tmp641->hd);}
# 2160
_tmp640=Cyc_NewControlFlow_add_vars(_tmp5DE,_tmp640,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5FF->pat_vars))->v,_tmp5DE->unknown_all,s->loc,0);
Cyc_NewControlFlow_update_flow(env,_tmp5FF->body,_tmp640);{
union Cyc_CfFlowInfo_FlowInfo _tmp642=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp642;};}
# 2155
;_pop_region(temp);}_LL3C0: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp600=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp600->tag != 6)goto _LL3C2;else{_tmp601=_tmp600->f1;}}_LL3C1:
# 2164
 _tmp603=_tmp601;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp602=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp602->tag != 7)goto _LL3C4;else{_tmp603=_tmp602->f1;}}_LL3C3:
 _tmp605=_tmp603;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp604=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp604->tag != 8)goto _LL3C6;else{_tmp605=_tmp604->f2;}}_LL3C5:
# 2169
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp643=_tmp5DC->encloser;
struct Cyc_Absyn_Stmt*_tmp644=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp605)))->encloser;
while(_tmp644 != _tmp643){
struct Cyc_Absyn_Stmt*_tmp645=(Cyc_NewControlFlow_get_stmt_annot(_tmp643))->encloser;
if(_tmp645 == _tmp643){
{const char*_tmp939;void*_tmp938;(_tmp938=0,Cyc_Tcutil_terr(s->loc,((_tmp939="goto enters local scope or exception handler",_tag_dyneither(_tmp939,sizeof(char),45))),_tag_dyneither(_tmp938,sizeof(void*),0)));}
break;}
# 2178
_tmp643=_tmp645;}}
# 2183
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp605),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL3C6: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp606=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp606->tag != 10)goto _LL3C8;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL3C7: {
# 2187
union Cyc_CfFlowInfo_FlowInfo _tmp649;void*_tmp64A;struct _tuple14 _tmp648=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp607);_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;
_tmp649=Cyc_NewControlFlow_use_Rval(env,_tmp607->loc,_tmp649,_tmp64A);
return Cyc_NewControlFlow_anal_scs(env,_tmp649,_tmp608);}_LL3C8: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp609=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp609->tag != 15)goto _LL3CA;else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;}}_LL3C9: {
# 2194
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp60A);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2202
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2205
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp60B);
{union Cyc_CfFlowInfo_FlowInfo _tmp64B=scs_outflow;_LL3D5: if((_tmp64B.BottomFL).tag != 1)goto _LL3D7;_LL3D6:
 goto _LL3D4;_LL3D7:;_LL3D8: {
const char*_tmp93C;void*_tmp93B;(_tmp93B=0,Cyc_Tcutil_terr(s->loc,((_tmp93C="last catch clause may implicitly fallthru",_tag_dyneither(_tmp93C,sizeof(char),42))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}_LL3D4:;}
# 2211
outflow=s1_outflow;
# 2213
return outflow;};};}_LL3CA: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp60C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp60C->tag != 12)goto _LL3CC;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp60D=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp60C->f1)->r;if(_tmp60D->tag != 5)goto _LL3CC;else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60D->f2;}}_tmp610=_tmp60C->f2;}}_LL3CB: {
# 2219
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp60F->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp64F;union Cyc_CfFlowInfo_AbsLVal _tmp650;struct _tuple15 _tmp64E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp64F=_tmp64E.f1;_tmp650=_tmp64E.f2;{
# 2224
struct _tuple15 _tmp93D;struct _tuple15 _tmp652=(_tmp93D.f1=_tmp64F,((_tmp93D.f2=_tmp650,_tmp93D)));struct Cyc_Dict_Dict _tmp653;struct Cyc_List_List*_tmp654;struct Cyc_CfFlowInfo_Place*_tmp655;_LL3DA: if(((_tmp652.f1).ReachableFL).tag != 2)goto _LL3DC;_tmp653=((struct _tuple13)((_tmp652.f1).ReachableFL).val).f1;_tmp654=((struct _tuple13)((_tmp652.f1).ReachableFL).val).f2;if(((_tmp652.f2).PlaceL).tag != 1)goto _LL3DC;_tmp655=(struct Cyc_CfFlowInfo_Place*)((_tmp652.f2).PlaceL).val;_LL3DB: {
# 2226
void*_tmp656=Cyc_CfFlowInfo_lookup_place(_tmp653,_tmp655);
# 2228
void*t=(void*)_check_null(e->topt);
# 2235
void*_tmp657=Cyc_CfFlowInfo_make_unique_consumed(_tmp5DE,t,e,env->iteration_num,_tmp656);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp940;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp93F;_tmp657=(void*)((_tmp93F=_region_malloc(_tmp5DE->r,sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp940.tag=8,((_tmp940.f1=_tmp60F,((_tmp940.f2=_tmp657,_tmp940)))))),_tmp93F))));}
_tmp653=Cyc_CfFlowInfo_assign_place(_tmp5DE,e->loc,_tmp653,env->all_changed,_tmp655,_tmp657);
_tmp64F=Cyc_CfFlowInfo_ReachableFL(_tmp653,_tmp654);{
# 2242
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp5DE,_tmp653,_tmp656)){case Cyc_CfFlowInfo_AllIL: _LL3E0:
 leaf=_tmp5DE->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL3E1:
 leaf=_tmp5DE->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL3E2:
 leaf=_tmp5DE->unknown_this;break;}{
# 2250
void*_tmp65A=Cyc_CfFlowInfo_typ_to_absrval(_tmp5DE,t,0,leaf);
_tmp64F=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp64F,_tmp656);{
struct Cyc_List_List _tmp941;struct Cyc_List_List _tmp65B=(_tmp941.hd=_tmp60F,((_tmp941.tl=0,_tmp941)));
_tmp64F=Cyc_NewControlFlow_add_vars(_tmp5DE,_tmp64F,& _tmp65B,_tmp5DE->unknown_all,s->loc,0);
# 2257
_tmp64F=Cyc_NewControlFlow_anal_stmt(env,_tmp64F,_tmp610);{
# 2262
union Cyc_CfFlowInfo_FlowInfo _tmp65C=_tmp64F;struct Cyc_Dict_Dict _tmp65D;struct Cyc_List_List*_tmp65E;_LL3E5: if((_tmp65C.ReachableFL).tag != 2)goto _LL3E7;_tmp65D=((struct _tuple13)(_tmp65C.ReachableFL).val).f1;_tmp65E=((struct _tuple13)(_tmp65C.ReachableFL).val).f2;_LL3E6:
# 2265
{void*_tmp65F=Cyc_CfFlowInfo_lookup_place(_tmp65D,_tmp655);struct Cyc_Absyn_Vardecl*_tmp661;_LL3EA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp660=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp65F;if(_tmp660->tag != 8)goto _LL3EC;else{_tmp661=_tmp660->f1;}}_LL3EB:
# 2267
 if(_tmp661 != _tmp60F)goto _LL3ED;
goto _LL3E9;_LL3EC:;_LL3ED:
# 2270
{const char*_tmp944;void*_tmp943;(_tmp943=0,Cyc_Tcutil_terr(s->loc,((_tmp944="aliased location was overwritten",_tag_dyneither(_tmp944,sizeof(char),33))),_tag_dyneither(_tmp943,sizeof(void*),0)));}
goto _LL3E9;_LL3E9:;}
# 2273
_tmp65D=Cyc_CfFlowInfo_assign_place(_tmp5DE,s->loc,_tmp65D,env->all_changed,_tmp655,_tmp65A);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp65D,_tmp65E);
# 2278
return outflow;_LL3E7:;_LL3E8:
 return _tmp64F;_LL3E4:;};};};};}_LL3DC: if(((_tmp652.f1).BottomFL).tag != 1)goto _LL3DE;_LL3DD:
# 2281
 return _tmp64F;_LL3DE:;_LL3DF:
{const char*_tmp947;void*_tmp946;(_tmp946=0,Cyc_Tcutil_terr(e->loc,((_tmp947="bad alias expression--no unique path found",_tag_dyneither(_tmp947,sizeof(char),43))),_tag_dyneither(_tmp946,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL3D9:;};}_LL3CC: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp611=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp611->tag != 12)goto _LL3CE;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;}}_LL3CD:
# 2287
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp612),_tmp613);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp612);
return outflow;_LL3CE: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp614=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp614->tag != 13)goto _LL3D0;else{_tmp615=_tmp614->f2;}}_LL3CF:
# 2292
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp615);_LL3D0: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp616=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp5DF;if(_tmp616->tag != 16)goto _LL3D2;else{_tmp617=_tmp616->f1;}}_LL3D1: {
# 2295
union Cyc_CfFlowInfo_FlowInfo _tmp668;void*_tmp669;struct _tuple14 _tmp667=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp617);_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp66A=Cyc_NewControlFlow_use_Rval(env,_tmp617->loc,_tmp668,_tmp669);
void*_tmp66B=Cyc_Tcutil_compress((void*)_check_null(_tmp617->topt));void*_tmp66D;_LL3EF: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp66B;if(_tmp66C->tag != 15)goto _LL3F1;else{_tmp66D=(void*)_tmp66C->f1;}}_LL3F0:
# 2299
 return Cyc_CfFlowInfo_kill_flow_region(_tmp5DE,_tmp668,_tmp66D);_LL3F1:;_LL3F2: {
struct Cyc_Core_Impossible_exn_struct _tmp94D;const char*_tmp94C;struct Cyc_Core_Impossible_exn_struct*_tmp94B;(int)_throw((void*)((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94D.tag=Cyc_Core_Impossible,((_tmp94D.f1=((_tmp94C="anal_stmt -- reset_region",_tag_dyneither(_tmp94C,sizeof(char),26))),_tmp94D)))),_tmp94B)))));}_LL3EE:;};}_LL3D2:;_LL3D3: {
# 2303
struct Cyc_Core_Impossible_exn_struct _tmp953;const char*_tmp952;struct Cyc_Core_Impossible_exn_struct*_tmp951;(int)_throw((void*)((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=((_tmp953.tag=Cyc_Core_Impossible,((_tmp953.f1=((_tmp952="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp952,sizeof(char),56))),_tmp953)))),_tmp951)))));}_LL3A9:;};}
# 2308
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2310
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp674=env->fenv;
void*_tmp675=decl->r;struct Cyc_Absyn_Vardecl*_tmp677;struct Cyc_List_List*_tmp679;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_List_List*_tmp67C;struct Cyc_Absyn_Fndecl*_tmp67E;struct Cyc_Absyn_Tvar*_tmp680;struct Cyc_Absyn_Vardecl*_tmp681;int _tmp682;struct Cyc_Absyn_Exp*_tmp683;_LL3F4: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp676=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp675;if(_tmp676->tag != 0)goto _LL3F6;else{_tmp677=_tmp676->f1;}}_LL3F5: {
# 2320
struct Cyc_List_List _tmp954;struct Cyc_List_List _tmp684=(_tmp954.hd=_tmp677,((_tmp954.tl=0,_tmp954)));
inflow=Cyc_NewControlFlow_add_vars(_tmp674,inflow,& _tmp684,_tmp674->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp685=_tmp677->initializer;
if(_tmp685 == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp687;void*_tmp688;struct _tuple14 _tmp686=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp685);_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp689=_tmp687;struct Cyc_Dict_Dict _tmp68A;struct Cyc_List_List*_tmp68B;_LL401: if((_tmp689.BottomFL).tag != 1)goto _LL403;_LL402:
 return Cyc_CfFlowInfo_BottomFL();_LL403: if((_tmp689.ReachableFL).tag != 2)goto _LL400;_tmp68A=((struct _tuple13)(_tmp689.ReachableFL).val).f1;_tmp68B=((struct _tuple13)(_tmp689.ReachableFL).val).f2;_LL404:
# 2329
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp95A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp959;struct Cyc_CfFlowInfo_Place*_tmp958;_tmp68A=Cyc_CfFlowInfo_assign_place(_tmp674,decl->loc,_tmp68A,env->all_changed,(
(_tmp958=_region_malloc(env->r,sizeof(*_tmp958)),((_tmp958->root=(void*)((_tmp95A=_region_malloc(env->r,sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp959.tag=0,((_tmp959.f1=_tmp677,_tmp959)))),_tmp95A)))),((_tmp958->fields=0,_tmp958)))))),_tmp688);}
# 2332
_tmp68B=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp68B,_tmp677,(struct Cyc_Absyn_Exp*)_check_null(_tmp677->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp68F=Cyc_CfFlowInfo_ReachableFL(_tmp68A,_tmp68B);
Cyc_NewControlFlow_update_tryflow(env,_tmp68F);
# 2337
return _tmp68F;};_LL400:;};};};}_LL3F6: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp678=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp675;if(_tmp678->tag != 2)goto _LL3F8;else{if(_tmp678->f2 == 0)goto _LL3F8;_tmp679=(struct Cyc_List_List*)(_tmp678->f2)->v;_tmp67A=_tmp678->f3;}}_LL3F7: {
# 2342
union Cyc_CfFlowInfo_FlowInfo _tmp692;void*_tmp693;struct _tuple14 _tmp691=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp67A);_tmp692=_tmp691.f1;_tmp693=_tmp691.f2;
_tmp692=Cyc_NewControlFlow_use_Rval(env,_tmp67A->loc,_tmp692,_tmp693);
return Cyc_NewControlFlow_add_vars(_tmp674,_tmp692,_tmp679,_tmp674->unknown_all,decl->loc,0);}_LL3F8: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp67B=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp675;if(_tmp67B->tag != 3)goto _LL3FA;else{_tmp67C=_tmp67B->f1;}}_LL3F9:
# 2347
 return Cyc_NewControlFlow_add_vars(_tmp674,inflow,_tmp67C,_tmp674->unknown_none,decl->loc,0);_LL3FA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp67D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp675;if(_tmp67D->tag != 1)goto _LL3FC;else{_tmp67E=_tmp67D->f1;}}_LL3FB:
# 2350
 Cyc_NewControlFlow_check_nested_fun(_tmp674,inflow,_tmp67E);{
void*t=(void*)_check_null(_tmp67E->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp694=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp67E->fn_vardecl);
# 2356
struct Cyc_List_List*_tmp95B;return Cyc_NewControlFlow_add_vars(_tmp674,inflow,((_tmp95B=_region_malloc(env->r,sizeof(*_tmp95B)),((_tmp95B->hd=_tmp694,((_tmp95B->tl=0,_tmp95B)))))),_tmp674->unknown_all,decl->loc,0);};_LL3FC: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp67F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp675;if(_tmp67F->tag != 4)goto _LL3FE;else{_tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;_tmp682=_tmp67F->f3;_tmp683=_tmp67F->f4;}}_LL3FD:
# 2359
 if(_tmp683 != 0){
struct Cyc_Absyn_Exp*_tmp696=_tmp683;
union Cyc_CfFlowInfo_FlowInfo _tmp698;void*_tmp699;struct _tuple14 _tmp697=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp696);_tmp698=_tmp697.f1;_tmp699=_tmp697.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp696->loc,_tmp698,_tmp699);}{
# 2364
struct Cyc_List_List _tmp95C;struct Cyc_List_List _tmp69A=(_tmp95C.hd=_tmp681,((_tmp95C.tl=0,_tmp95C)));
return Cyc_NewControlFlow_add_vars(_tmp674,inflow,& _tmp69A,_tmp674->unknown_all,decl->loc,0);};_LL3FE:;_LL3FF: {
# 2367
struct Cyc_Core_Impossible_exn_struct _tmp962;const char*_tmp961;struct Cyc_Core_Impossible_exn_struct*_tmp960;(int)_throw((void*)((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960[0]=((_tmp962.tag=Cyc_Core_Impossible,((_tmp962.f1=((_tmp961="anal_decl: unexpected decl variant",_tag_dyneither(_tmp961,sizeof(char),35))),_tmp962)))),_tmp960)))));}_LL3F3:;}
# 2407 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp69F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp69F;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2409
;_pop_region(frgn);}
# 2414
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2418
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2422
struct _RegionHandle*_tmp6A0=fenv->r;
unsigned int _tmp6A1=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6A1,1);{
# 2428
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6A2=inflow;struct Cyc_Dict_Dict _tmp6A3;struct Cyc_List_List*_tmp6A4;_LL406: if((_tmp6A2.BottomFL).tag != 1)goto _LL408;_LL407: {
const char*_tmp965;void*_tmp964;(_tmp964=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp965="check_fun",_tag_dyneither(_tmp965,sizeof(char),10))),_tag_dyneither(_tmp964,sizeof(void*),0)));}_LL408: if((_tmp6A2.ReachableFL).tag != 2)goto _LL405;_tmp6A3=((struct _tuple13)(_tmp6A2.ReachableFL).val).f1;_tmp6A4=((struct _tuple13)(_tmp6A2.ReachableFL).val).f2;_LL409: {
# 2435
struct Cyc_List_List*atts;
{void*_tmp6A7=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp6A9;_LL40B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6A8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6A7;if(_tmp6A8->tag != 9)goto _LL40D;else{_tmp6A9=(_tmp6A8->f1).attributes;}}_LL40C:
 atts=_tmp6A9;goto _LL40A;_LL40D:;_LL40E: {
const char*_tmp968;void*_tmp967;(_tmp967=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp968="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp968,sizeof(char),50))),_tag_dyneither(_tmp967,sizeof(void*),0)));}_LL40A:;}
# 2440
for(0;atts != 0;atts=atts->tl){
void*_tmp6AC=(void*)atts->hd;int _tmp6AE;int _tmp6B0;int _tmp6B2;_LL410: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp6AD=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6AC;if(_tmp6AD->tag != 21)goto _LL412;else{_tmp6AE=_tmp6AD->f1;}}_LL411: {
# 2443
unsigned int j=(unsigned int)_tmp6AE;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp96B;void*_tmp96A;(_tmp96A=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp96B="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp96B,sizeof(char),52))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}
continue;}{
# 2449
struct Cyc_Absyn_Vardecl*_tmp6B5=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6B5->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp96E;void*_tmp96D;(_tmp96D=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp96E="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp96E,sizeof(char),52))),_tag_dyneither(_tmp96D,sizeof(void*),0)));}
continue;}{
# 2455
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6B8=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp971;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp970;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6B9=(_tmp970=_region_malloc(_tmp6A0,sizeof(*_tmp970)),((_tmp970[0]=((_tmp971.tag=2,((_tmp971.f1=(int)j,((_tmp971.f2=t,_tmp971)))))),_tmp970)));
struct Cyc_CfFlowInfo_Place*_tmp972;struct Cyc_CfFlowInfo_Place*_tmp6BA=(_tmp972=_region_malloc(_tmp6A0,sizeof(*_tmp972)),((_tmp972->root=(void*)_tmp6B9,((_tmp972->fields=0,_tmp972)))));
_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)_tmp6B9,_tmp6B8);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp978;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp977;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp975;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp974;_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)((_tmp974=_region_malloc(_tmp6A0,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=0,((_tmp975.f1=_tmp6B5,_tmp975)))),_tmp974)))),(void*)((_tmp977=_region_malloc(_tmp6A0,sizeof(*_tmp977)),((_tmp977[0]=((_tmp978.tag=5,((_tmp978.f1=_tmp6BA,_tmp978)))),_tmp977)))));}
goto _LL40F;};};}_LL412: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6AF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6AC;if(_tmp6AF->tag != 20)goto _LL414;else{_tmp6B0=_tmp6AF->f1;}}_LL413: {
# 2465
unsigned int j=(unsigned int)_tmp6B0;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp97B;void*_tmp97A;(_tmp97A=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp97B="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp97B,sizeof(char),51))),_tag_dyneither(_tmp97A,sizeof(void*),0)));}
# 2469
continue;}{
# 2471
struct Cyc_Absyn_Vardecl*_tmp6C4=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp6C5=Cyc_Tcutil_compress(_tmp6C4->type);void*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;_LL419: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C5;if(_tmp6C6->tag != 5)goto _LL41B;else{_tmp6C7=(_tmp6C6->f1).elt_typ;_tmp6C8=((_tmp6C6->f1).ptr_atts).nullable;_tmp6C9=((_tmp6C6->f1).ptr_atts).bounds;_tmp6CA=((_tmp6C6->f1).ptr_atts).zero_term;}}_LL41A:
# 2474
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C8)){
const char*_tmp97E;void*_tmp97D;(_tmp97D=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp97E="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp97E,sizeof(char),55))),_tag_dyneither(_tmp97D,sizeof(void*),0)));}
# 2477
if(!Cyc_Tcutil_is_bound_one(_tmp6C9)){
const char*_tmp981;void*_tmp980;(_tmp980=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp981="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp981,sizeof(char),57))),_tag_dyneither(_tmp980,sizeof(void*),0)));}
# 2480
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA)){
const char*_tmp984;void*_tmp983;(_tmp983=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp984="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp984,sizeof(char),77))),_tag_dyneither(_tmp983,sizeof(void*),0)));}{
# 2483
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp987;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp986;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6D1=(_tmp986=_region_malloc(_tmp6A0,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=2,((_tmp987.f1=(int)j,((_tmp987.f2=_tmp6C7,_tmp987)))))),_tmp986)));
struct Cyc_CfFlowInfo_Place*_tmp988;struct Cyc_CfFlowInfo_Place*_tmp6D2=(_tmp988=_region_malloc(_tmp6A0,sizeof(*_tmp988)),((_tmp988->root=(void*)_tmp6D1,((_tmp988->fields=0,_tmp988)))));
_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)_tmp6D1,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6C7,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp98E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp98D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp98B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp98A;_tmp6A3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6A3,(void*)((_tmp98A=_region_malloc(_tmp6A0,sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=0,((_tmp98B.f1=_tmp6C4,_tmp98B)))),_tmp98A)))),(void*)((_tmp98D=_region_malloc(_tmp6A0,sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98E.tag=5,((_tmp98E.f1=_tmp6D2,_tmp98E)))),_tmp98D)))));}
{struct Cyc_List_List*_tmp98F;param_roots=((_tmp98F=_region_malloc(_tmp6A0,sizeof(*_tmp98F)),((_tmp98F->hd=_tmp6D2,((_tmp98F->tl=param_roots,_tmp98F))))));}
goto _LL418;};_LL41B:;_LL41C: {
const char*_tmp992;void*_tmp991;(_tmp991=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp992="initializes attribute on non-pointer",_tag_dyneither(_tmp992,sizeof(char),37))),_tag_dyneither(_tmp991,sizeof(void*),0)));}_LL418:;}
# 2491
goto _LL40F;};}_LL414: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6B1=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6AC;if(_tmp6B1->tag != 22)goto _LL416;else{_tmp6B2=_tmp6B1->f1;}}_LL415: {
# 2493
unsigned int j=(unsigned int)_tmp6B2;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp995;void*_tmp994;(_tmp994=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp995="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp995,sizeof(char),49))),_tag_dyneither(_tmp994,sizeof(void*),0)));}
continue;}{
# 2498
struct Cyc_Absyn_Vardecl*_tmp6DF=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp6DF->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp998;void*_tmp997;(_tmp997=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp998="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp998,sizeof(char),49))),_tag_dyneither(_tmp997,sizeof(void*),0)));}
continue;}{
# 2505
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp99B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp99A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6E2=(_tmp99A=_region_malloc(_tmp6A0,sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=0,((_tmp99B.f1=_tmp6DF,_tmp99B)))),_tmp99A)));
struct Cyc_CfFlowInfo_Place*_tmp99C;struct Cyc_CfFlowInfo_Place*_tmp6E3=(_tmp99C=_region_malloc(_tmp6A0,sizeof(*_tmp99C)),((_tmp99C->root=(void*)_tmp6E2,((_tmp99C->fields=0,_tmp99C)))));
void*_tmp6E4=Cyc_CfFlowInfo_lookup_place(_tmp6A3,_tmp6E3);
{struct _tuple24*_tmp99F;struct Cyc_List_List*_tmp99E;noconsume_rvals=((_tmp99E=_region_malloc(_tmp6A0,sizeof(*_tmp99E)),((_tmp99E->hd=((_tmp99F=_region_malloc(_tmp6A0,sizeof(*_tmp99F)),((_tmp99F->f1=_tmp6E3,((_tmp99F->f2=_tmp6E4,_tmp99F)))))),((_tmp99E->tl=noconsume_rvals,_tmp99E))))));}
# 2510
goto _LL40F;};};}_LL416:;_LL417:
 goto _LL40F;_LL40F:;}
# 2513
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp6A3,_tmp6A4);}_LL405:;}{
# 2516
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6A0,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp9A0;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp9A0=_region_malloc(_tmp6A0,sizeof(*_tmp9A0)),((_tmp9A0->r=_tmp6A0,((_tmp9A0->fenv=fenv,((_tmp9A0->iterate_again=1,((_tmp9A0->iteration_num=0,((_tmp9A0->in_try=0,((_tmp9A0->tryflow=inflow,((_tmp9A0->all_changed=0,((_tmp9A0->noreturn=noreturn,((_tmp9A0->param_roots=param_roots,((_tmp9A0->noconsume_params=noconsume_rvals,((_tmp9A0->flow_table=flow_table,_tmp9A0)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2526
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2529
union Cyc_CfFlowInfo_FlowInfo _tmp6EA=outflow;_LL41E: if((_tmp6EA.BottomFL).tag != 1)goto _LL420;_LL41F:
 goto _LL41D;_LL420:;_LL421:
# 2532
 Cyc_NewControlFlow_check_init_params(_tmp6A1,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp6A1,env,outflow);
# 2536
if(noreturn){
const char*_tmp9A3;void*_tmp9A2;(_tmp9A2=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp9A3="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp9A3,sizeof(char),46))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}else{
# 2539
void*_tmp6ED=Cyc_Tcutil_compress(fd->ret_type);_LL423: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6EE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6ED;if(_tmp6EE->tag != 0)goto _LL425;}_LL424:
 goto _LL422;_LL425: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6EF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6ED;if(_tmp6EF->tag != 7)goto _LL427;}_LL426:
 goto _LL428;_LL427: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6ED;if(_tmp6F0->tag != 6)goto _LL429;}_LL428:
# 2543
{const char*_tmp9A6;void*_tmp9A5;(_tmp9A5=0,Cyc_Tcutil_warn(_tmp6A1,((_tmp9A6="function may not return a value",_tag_dyneither(_tmp9A6,sizeof(char),32))),_tag_dyneither(_tmp9A5,sizeof(void*),0)));}goto _LL422;_LL429:;_LL42A:
# 2545
{const char*_tmp9A9;void*_tmp9A8;(_tmp9A8=0,Cyc_Tcutil_terr(_tmp6A1,((_tmp9A9="function may not return a value",_tag_dyneither(_tmp9A9,sizeof(char),32))),_tag_dyneither(_tmp9A8,sizeof(void*),0)));}goto _LL422;_LL422:;}
# 2547
goto _LL41D;_LL41D:;};};};}
# 2552
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp6F6=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp6F8;struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*_tmp6FC;struct Cyc_List_List*_tmp6FE;_LL42C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6F7=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6F6;if(_tmp6F7->tag != 1)goto _LL42E;else{_tmp6F8=_tmp6F7->f1;}}_LL42D:
 Cyc_NewControlFlow_check_fun(_tmp6F8);goto _LL42B;_LL42E: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp6F9=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6F6;if(_tmp6F9->tag != 12)goto _LL430;else{_tmp6FA=_tmp6F9->f1;}}_LL42F:
 _tmp6FC=_tmp6FA;goto _LL431;_LL430: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp6FB=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6F6;if(_tmp6FB->tag != 11)goto _LL432;else{_tmp6FC=_tmp6FB->f2;}}_LL431:
 _tmp6FE=_tmp6FC;goto _LL433;_LL432: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp6FD=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6F6;if(_tmp6FD->tag != 10)goto _LL434;else{_tmp6FE=_tmp6FD->f2;}}_LL433:
 Cyc_NewControlFlow_cf_check(_tmp6FE);goto _LL42B;_LL434: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp6FF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6F6;if(_tmp6FF->tag != 13)goto _LL436;}_LL435:
 goto _LL42B;_LL436:;_LL437:
 goto _LL42B;_LL42B:;}}
