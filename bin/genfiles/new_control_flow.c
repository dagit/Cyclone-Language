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
# 66
void*Cyc_Tcutil_compress(void*t);
# 100
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 160
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 173
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 182
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 246 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 278
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 344
int Cyc_Tcutil_is_noreturn(void*);
# 362
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 366
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 369
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp691;struct Cyc_NewControlFlow_CFStmtAnnot _tmp690;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp68F;enclosee->annot=(void*)((_tmp68F=_cycalloc(sizeof(*_tmp68F)),((_tmp68F[0]=((_tmp691.tag=Cyc_NewControlFlow_CFAnnot,((_tmp691.f1=((_tmp690.encloser=encloser,((_tmp690.visited=0,_tmp690)))),_tmp691)))),_tmp68F))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp697;const char*_tmp696;struct Cyc_Core_Impossible_exn_struct*_tmp695;(int)_throw((void*)((_tmp695=_cycalloc(sizeof(*_tmp695)),((_tmp695[0]=((_tmp697.tag=Cyc_Core_Impossible,((_tmp697.f1=((_tmp696="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp696,sizeof(char),37))),_tmp697)))),_tmp695)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp698;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp698=_region_malloc(env->r,sizeof(*_tmp698)),((_tmp698[0]=Cyc_CfFlowInfo_BottomFL(),_tmp698)));
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
struct _tuple19 _tmp699;return(_tmp699.f1=_tmpA,((_tmp699.f2=_tmpB,_tmp699)));};}
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
{const char*_tmp69D;void*_tmp69C[1];struct Cyc_String_pa_PrintArg_struct _tmp69B;(_tmp69B.tag=0,((_tmp69B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp69C[0]=& _tmp69B,Cyc_Tcutil_warn(_tmpF,((_tmp69D="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp69D,sizeof(char),60))),_tag_dyneither(_tmp69C,sizeof(void*),1)))))));}
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
struct _tuple18 _tmp69E;struct _tuple18 _stmttmp1=(_tmp69E.f1=flow,((_tmp69E.f2=_tmp1F,_tmp69E)));struct _tuple18 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp69F;struct _tuple20 _tmp23=(_tmp69F.f1=env,((_tmp69F.f2=s->loc,((_tmp69F.f3=_tmp22,_tmp69F)))));
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6A2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6A1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp6A1=_region_malloc(fenv->r,sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=0,((_tmp6A2.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp6A2)))),_tmp6A1)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6A4;_tmp2A=(void*)((_tmp6A4=_region_malloc(fenv->r,sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=8,((_tmp6A5.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp6A5.f2=_tmp2A,_tmp6A5)))))),_tmp6A4))));}
# 259
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp6A6;if(Cyc_Relations_consistent_relations(((_tmp6A6=_region_malloc(r,sizeof(*_tmp6A6)),((_tmp6A6->hd=_tmp2F,((_tmp6A6->tl=assume,_tmp6A6))))))))
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
{const char*_tmp6AB;void*_tmp6AA[2];struct Cyc_String_pa_PrintArg_struct _tmp6A9;struct Cyc_String_pa_PrintArg_struct _tmp6A8;(_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A9.tag=0,((_tmp6A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6AA[0]=& _tmp6A9,((_tmp6AA[1]=& _tmp6A8,Cyc_Tcutil_terr(loc,((_tmp6AB="unable to prove %s, required to access %s",_tag_dyneither(_tmp6AB,sizeof(char),42))),_tag_dyneither(_tmp6AA,sizeof(void*),2)))))))))))));}
{const char*_tmp6AE;void*_tmp6AD;(_tmp6AD=0,Cyc_fprintf(Cyc_stderr,((_tmp6AE="  [recorded facts on path: ",_tag_dyneither(_tmp6AE,sizeof(char),28))),_tag_dyneither(_tmp6AD,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_fprintf(Cyc_stderr,((_tmp6B1="]\n",_tag_dyneither(_tmp6B1,sizeof(char),3))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));};}}
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
{const char*_tmp6B6;void*_tmp6B5[2];struct Cyc_String_pa_PrintArg_struct _tmp6B4;struct Cyc_String_pa_PrintArg_struct _tmp6B3;(_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6B5[0]=& _tmp6B4,((_tmp6B5[1]=& _tmp6B3,Cyc_Tcutil_terr(loc,((_tmp6B6="unable to prove %s, required to access %s",_tag_dyneither(_tmp6B6,sizeof(char),42))),_tag_dyneither(_tmp6B5,sizeof(void*),2)))))))))))));}
{const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,Cyc_fprintf(Cyc_stderr,((_tmp6B9="  [recorded facts on path: ",_tag_dyneither(_tmp6B9,sizeof(char),28))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_fprintf(Cyc_stderr,((_tmp6BC="]\n",_tag_dyneither(_tmp6BC,sizeof(char),3))),_tag_dyneither(_tmp6BB,sizeof(void*),0)));};}
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
const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_Tcutil_terr(loc,((_tmp6BF="expression may not be fully initialized",_tag_dyneither(_tmp6BF,sizeof(char),40))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6C0;struct _tuple0 _stmttmp4=(_tmp6C0.f1=Cyc_Tcutil_compress(t),((_tmp6C0.f2=r,_tmp6C0)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
{const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_Tcutil_warn(loc,((_tmp6C3="argument may still contain unique pointers",_tag_dyneither(_tmp6C3,sizeof(char),43))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6C6;void*_tmp6C5;(_tmp6C5=0,Cyc_Tcutil_terr(loc,((_tmp6C6="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6C6,sizeof(char),47))),_tag_dyneither(_tmp6C5,sizeof(void*),0)));}
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
const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_Tcutil_warn(loc,((_tmp6C9="argument may contain live unique pointers",_tag_dyneither(_tmp6C9,sizeof(char),42))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
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
struct _tuple22 _tmp6CA;return(_tmp6CA.f1=inflow,((_tmp6CA.f2=0,_tmp6CA)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6CD;struct _tuple22 _tmp6CC;return(_tmp6CC.f1=_tmp8C,((_tmp6CC.f2=((_tmp6CD=_region_malloc(rgn,sizeof(*_tmp6CD)),((_tmp6CD->hd=_tmp8D,((_tmp6CD->tl=0,_tmp6CD)))))),_tmp6CC)));};}{
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
{struct Cyc_Dict_Dict*_tmp6CE;env->all_changed=((_tmp6CE=_region_malloc(env->r,sizeof(*_tmp6CE)),((_tmp6CE[0]=(env->fenv)->mt_place_set,_tmp6CE))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6CF;rvals=((_tmp6CF=_region_malloc(rgn,sizeof(*_tmp6CF)),((_tmp6CF->hd=_tmp93,((_tmp6CF->tl=0,_tmp6CF))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6D0;env->all_changed=((_tmp6D0=_region_malloc(env->r,sizeof(*_tmp6D0)),((_tmp6D0[0]=(env->fenv)->mt_place_set,_tmp6D0))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6D1;rvals=((_tmp6D1=_region_malloc(rgn,sizeof(*_tmp6D1)),((_tmp6D1->hd=_tmp98,((_tmp6D1->tl=rvals,_tmp6D1))))));}
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
struct Cyc_Dict_Dict*_tmp6D2;env->all_changed=((_tmp6D2=_region_malloc(env->r,sizeof(*_tmp6D2)),((_tmp6D2[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6D2))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp6D3;return(_tmp6D3.f1=outflow,((_tmp6D3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6D3)));};};}
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
const char*_tmp6D6;void*_tmp6D5;(_tmp6D5=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6D6="expression may not be initialized",_tag_dyneither(_tmp6D6,sizeof(char),34))),_tag_dyneither(_tmp6D5,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp6D7;struct _tuple15 _tmpA5=(_tmp6D7.f1=_tmp9E,((_tmp6D7.f2=(env->fenv)->unknown_all,_tmp6D7)));_npop_handler(0);return _tmpA5;};}
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
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DA;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D9;nzval=(void*)((_tmp6D9=_region_malloc((env->fenv)->r,sizeof(*_tmp6D9)),((_tmp6D9[0]=((_tmp6DA.tag=8,((_tmp6DA.f1=locname,((_tmp6DA.f2=nzval,_tmp6DA)))))),_tmp6D9))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 521
return _tmpAD;};}_LL60:;}_LL5B:;}
# 530
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 533
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp6DB;return(_tmp6DB.f1=outflow,((_tmp6DB.f2=outflow,_tmp6DB)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 536
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp6DC;return(_tmp6DC.f1=outflow,((_tmp6DC.f2=outflow,_tmp6DC)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DF;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6DE;nzval=(void*)((_tmp6DE=_region_malloc((env->fenv)->r,sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6DF.tag=8,((_tmp6DF.f1=locname,((_tmp6DF.f2=nzval,_tmp6DF)))))),_tmp6DE))));}{
struct _tuple18 _tmp6E0;return
(_tmp6E0.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6E0.f2=
# 545
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,(env->fenv)->zero),_tmpB0),_tmp6E0)));};}_LL6A:;}_LL65:;}
# 551
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 559
static struct _tuple15 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 563
void*_stmttmpB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpBA=_stmttmpB;void*_tmpBC;union Cyc_Absyn_Constraint*_tmpBD;union Cyc_Absyn_Constraint*_tmpBE;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBA;if(_tmpBB->tag != 5)goto _LL72;else{_tmpBC=(_tmpBB->f1).elt_typ;_tmpBD=((_tmpBB->f1).ptr_atts).bounds;_tmpBE=((_tmpBB->f1).ptr_atts).zero_term;}}_LL71: {
# 565
union Cyc_CfFlowInfo_FlowInfo _tmpBF=f;struct Cyc_Dict_Dict _tmpC0;struct Cyc_List_List*_tmpC1;_LL75: if((_tmpBF.BottomFL).tag != 1)goto _LL77;_LL76: {
# 567
struct _tuple15 _tmp6E1;return(_tmp6E1.f1=f,((_tmp6E1.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6E1)));}_LL77: if((_tmpBF.ReachableFL).tag != 2)goto _LL74;_tmpC0=((struct _tuple14)(_tmpBF.ReachableFL).val).f1;_tmpC1=((struct _tuple14)(_tmpBF.ReachableFL).val).f2;_LL78:
# 570
 if(Cyc_Tcutil_is_bound_one(_tmpBD)){
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmpC3=r;struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_CfFlowInfo_Place*_tmpCA;enum Cyc_CfFlowInfo_InitLevel _tmpCD;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC4->tag != 8)goto _LL7C;else{_tmpC5=_tmpC4->f1;_tmpC6=(void*)_tmpC4->f2;}}_LL7B:
 r=_tmpC6;locname=_tmpC5;goto retry;_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpC7=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC7->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC8->tag != 2)goto _LL80;}_LL7F:
# 576
{void*_stmttmpF=e->annot;void*_tmpCE=_stmttmpF;struct Cyc_List_List*_tmpD0;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpCF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpCE;if(_tmpCF->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpD0=_tmpCF->f1;}}_LL8A:
# 578
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpD0))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 582
{void*_stmttmp10=e->r;void*_tmpD1=_stmttmp10;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD1;if(_tmpD2->tag != 22)goto _LL90;}_LL8F:
# 584
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E4;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E3;e->annot=(void*)((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3[0]=((_tmp6E4.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E4.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E4)))),_tmp6E3))));}
goto _LL8D;_LL90:;_LL91:
# 587
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 590
goto _LL88;_LL88:;}
# 592
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpC9=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC9->tag != 5)goto _LL82;else{_tmpCA=_tmpC9->f1;}}_LL81:
# 594
{void*_stmttmpD=e->annot;void*_tmpD5=_stmttmpD;struct Cyc_List_List*_tmpD7;_LL93: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD6=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD5;if(_tmpD6->tag != Cyc_CfFlowInfo_NotZero)goto _LL95;else{_tmpD7=_tmpD6->f1;}}_LL94:
# 596
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpD7))goto _LL96;
goto _LL92;_LL95:;_LL96:
# 600
{void*_stmttmpE=e->r;void*_tmpD8=_stmttmpE;_LL98: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8;if(_tmpD9->tag != 22)goto _LL9A;}_LL99:
# 602
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E7;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E6;e->annot=(void*)((_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E7.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E7)))),_tmp6E6))));}
goto _LL97;_LL9A:;_LL9B:
# 605
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 608
goto _LL92;_LL92:;}{
# 610
struct _tuple15 _tmp6E8;return(_tmp6E8.f1=f,((_tmp6E8.f2=Cyc_CfFlowInfo_lookup_place(_tmpC0,_tmpCA),_tmp6E8)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpCB->tag != 0)goto _LL84;}_LL83:
# 612
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6E9;return(_tmp6E9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6E9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6E9)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpCC->tag != 3)goto _LL86;else{_tmpCD=_tmpCC->f1;}}_LL85:
# 615
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpCD,locname);goto _LL87;_LL86:;_LL87:
# 617
{void*_stmttmpC=e->r;void*_tmpDE=_stmttmpC;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 22)goto _LL9F;}_LL9E:
# 619
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EC;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EB;e->annot=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EC.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6EC)))),_tmp6EB))));}
goto _LL9C;_LL9F:;_LLA0:
# 622
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9C;_LL9C:;}
# 625
goto _LL79;_LL79:;}}else{
# 628
void*_stmttmp11=e->annot;void*_tmpE2=_stmttmp11;struct Cyc_List_List*_tmpE4;_LLA2: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE3=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE2;if(_tmpE3->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA4;else{_tmpE4=_tmpE3->f1;}}_LLA3:
# 630
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpE4))goto _LLA5;
goto _LLA1;_LLA4:;_LLA5:
# 633
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EF;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EE;e->annot=(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EF.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6EF)))),_tmp6EE))));}
goto _LLA1;_LLA1:;}{
# 637
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC0,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 639
{void*_tmpE7=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE7;if(_tmpE8->tag != 7)goto _LLAB;}_LLAA:
# 641
{const char*_tmp6F2;void*_tmp6F1;(_tmp6F1=0,Cyc_Tcutil_terr(e->loc,((_tmp6F2="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6F2,sizeof(char),61))),_tag_dyneither(_tmp6F1,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 644
const char*_tmp6F5;void*_tmp6F4;(_tmp6F4=0,Cyc_Tcutil_terr(e->loc,((_tmp6F5="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6F5,sizeof(char),46))),_tag_dyneither(_tmp6F4,sizeof(void*),0)));}_LLA8:;}
# 646
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 648
struct _tuple15 _tmp6F6;return(_tmp6F6.f1=f,((_tmp6F6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6F6)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 650
struct _tuple15 _tmp6F7;return(_tmp6F7.f1=f,((_tmp6F7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_none),_tmp6F7)));}}};_LL74:;}_LL72:;_LL73: {
# 653
struct Cyc_Core_Impossible_exn_struct _tmp6FD;const char*_tmp6FC;struct Cyc_Core_Impossible_exn_struct*_tmp6FB;(int)_throw((void*)((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB[0]=((_tmp6FD.tag=Cyc_Core_Impossible,((_tmp6FD.f1=((_tmp6FC="right deref of non-pointer-type",_tag_dyneither(_tmp6FC,sizeof(char),32))),_tmp6FD)))),_tmp6FB)))));}_LL6F:;}
# 660
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 663
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 666
{void*_stmttmp13=e1->r;void*_tmpF2=_stmttmp13;void*_tmpF4;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF2;if(_tmpF3->tag != 1)goto _LLB2;else{_tmpF4=(void*)_tmpF3->f2;}}_LLB1: {
# 668
struct Cyc_Absyn_Vardecl*_tmpF5=Cyc_Tcutil_nonesc_vardecl(_tmpF4);
if(_tmpF5 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF5);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 674
goto _LLAF;}_LLB2:;_LLB3:
 goto _LLAF;_LLAF:;}{
# 678
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 684
return relns;};}
# 692
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc){
# 696
struct _tuple17 _stmttmp14=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF7;struct _tuple17 _tmpF6=_stmttmp14;_tmpF7=_tmpF6.f2;
{struct _tuple17 _tmp6FE;struct _tuple17 _stmttmp15=(_tmp6FE.f1=inflow,((_tmp6FE.f2=_tmpF7,_tmp6FE)));struct _tuple17 _tmpF8=_stmttmp15;struct Cyc_Dict_Dict _tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_CfFlowInfo_Place*_tmpFB;_LLB5: if(((_tmpF8.f1).ReachableFL).tag != 2)goto _LLB7;_tmpF9=((struct _tuple14)((_tmpF8.f1).ReachableFL).val).f1;_tmpFA=((struct _tuple14)((_tmpF8.f1).ReachableFL).val).f2;if(((_tmpF8.f2).PlaceL).tag != 1)goto _LLB7;_tmpFB=(struct Cyc_CfFlowInfo_Place*)((_tmpF8.f2).PlaceL).val;_LLB6: {
# 699
void*_tmpFC=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 703
_tmpF9=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpF9,env->all_changed,_tmpFB,_tmpFC);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpF9,_tmpFA);
# 708
goto _LLB4;}_LLB7: if(((_tmpF8.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 711
{const char*_tmp701;void*_tmp700;(_tmp700=0,Cyc_Tcutil_terr(loc,((_tmp701="noconsume() parameters must be l-values",_tag_dyneither(_tmp701,sizeof(char),40))),_tag_dyneither(_tmp700,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 714
return inflow;}
# 719
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 727
union Cyc_CfFlowInfo_FlowInfo _tmp100=outflow;struct Cyc_Dict_Dict _tmp101;struct Cyc_List_List*_tmp102;_LLBC: if((_tmp100.BottomFL).tag != 1)goto _LLBE;_LLBD: {
# 730
struct _tuple15 _tmp702;return(_tmp702.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp702.f2=rval,_tmp702)));}_LLBE: if((_tmp100.ReachableFL).tag != 2)goto _LLBB;_tmp101=((struct _tuple14)(_tmp100.ReachableFL).val).f1;_tmp102=((struct _tuple14)(_tmp100.ReachableFL).val).f2;_LLBF: {
# 732
union Cyc_CfFlowInfo_AbsLVal _tmp104=lval;struct Cyc_CfFlowInfo_Place*_tmp105;_LLC1: if((_tmp104.PlaceL).tag != 1)goto _LLC3;_tmp105=(struct Cyc_CfFlowInfo_Place*)(_tmp104.PlaceL).val;_LLC2: {
# 734
struct Cyc_Dict_Dict _tmp106=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp101,env->all_changed,_tmp105,rval);
_tmp102=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp106,_tmp102);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 740
void*_stmttmp16=Cyc_CfFlowInfo_lookup_place(_tmp101,_tmp105);void*_tmp107=_stmttmp16;_LLC6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp108=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp108->tag != 3)goto _LLC8;else{if(_tmp108->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;}}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp109=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp109->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10A->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10B->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10B->f2);if(_tmp10C->tag != 3)goto _LLCE;else{if(_tmp10C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCE;}}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10D->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 7)goto _LLD0;}}_LLCF:
 goto _LLD1;_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10F->tag != 8)goto _LLD2;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 0)goto _LLD2;}}_LLD1:
 goto _LLC5;_LLD2:;_LLD3:
# 748
{const char*_tmp705;void*_tmp704;(_tmp704=0,Cyc_Tcutil_warn(lexp->loc,((_tmp705="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp705,sizeof(char),43))),_tag_dyneither(_tmp704,sizeof(void*),0)));}
goto _LLC5;_LLC5:;}
# 753
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 755
struct _tuple15 _tmp706;return(_tmp706.f1=outflow,((_tmp706.f2=rval,_tmp706)));};}_LLC3: if((_tmp104.UnknownL).tag != 2)goto _LLC0;_LLC4: {
# 758
struct _tuple15 _tmp707;return(_tmp707.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp707.f2=rval,_tmp707)));}_LLC0:;}_LLBB:;}
# 765
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 771
union Cyc_CfFlowInfo_FlowInfo _tmp115=f;struct Cyc_Dict_Dict _tmp116;struct Cyc_List_List*_tmp117;_LLD5: if((_tmp115.BottomFL).tag != 1)goto _LLD7;_LLD6:
 return Cyc_CfFlowInfo_BottomFL();_LLD7: if((_tmp115.ReachableFL).tag != 2)goto _LLD4;_tmp116=((struct _tuple14)(_tmp115.ReachableFL).val).f1;_tmp117=((struct _tuple14)(_tmp115.ReachableFL).val).f2;_LLD8:
# 774
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp70D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp70C;struct Cyc_CfFlowInfo_Place*_tmp70B;_tmp116=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp116,env->all_changed,(
(_tmp70B=_region_malloc(env->r,sizeof(*_tmp70B)),((_tmp70B->root=(void*)((_tmp70D=_region_malloc(env->r,sizeof(*_tmp70D)),((_tmp70D[0]=((_tmp70C.tag=0,((_tmp70C.f1=vd,_tmp70C)))),_tmp70D)))),((_tmp70B->fields=0,_tmp70B)))))),rval);}
# 777
_tmp117=Cyc_Relations_reln_assign_var(env->r,_tmp117,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11B=Cyc_CfFlowInfo_ReachableFL(_tmp116,_tmp117);
Cyc_NewControlFlow_update_tryflow(env,_tmp11B);
# 784
return _tmp11B;};_LLD4:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 788
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 795
if(vds == 0)return inflow;{
# 798
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp70E;es=((_tmp70E=_cycalloc(sizeof(*_tmp70E)),((_tmp70E->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp70E->tl=es,_tmp70E))))));}}}
# 804
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11C,fenv->unknown_all,pat_loc,name_locs);
# 806
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 810
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp11F;void*_tmp120;struct _tuple15 _tmp11E=_stmttmp17;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp11F,_tmp120);}}{
# 818
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp121 != 0;_tmp121=_tmp121->tl){
struct _tuple23*_stmttmp18=(struct _tuple23*)_tmp121->hd;struct Cyc_Absyn_Vardecl**_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct _tuple23*_tmp122=_stmttmp18;_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;
if(_tmp123 != 0  && _tmp124 != 0){
# 825
if(_tmp124->topt == 0){
const char*_tmp712;void*_tmp711[1];struct Cyc_String_pa_PrintArg_struct _tmp710;(_tmp710.tag=0,((_tmp710.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp124)),((_tmp711[0]=& _tmp710,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp712="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp712,sizeof(char),41))),_tag_dyneither(_tmp711,sizeof(void*),1)))))));}
# 830
Cyc_fflush(Cyc_stderr);{
# 832
struct Cyc_List_List _tmp713;struct Cyc_List_List l=(_tmp713.hd=_tmp124,((_tmp713.tl=0,_tmp713)));
union Cyc_CfFlowInfo_FlowInfo _tmp128=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple15 _stmttmp19=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp128,_tmp124);union Cyc_CfFlowInfo_FlowInfo _tmp12A;void*_tmp12B;struct _tuple15 _tmp129=_stmttmp19;_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp716;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp715;_tmp12B=(void*)((_tmp715=_region_malloc(fenv->r,sizeof(*_tmp715)),((_tmp715[0]=((_tmp716.tag=8,((_tmp716.f1=*_tmp123,((_tmp716.f2=_tmp12B,_tmp716)))))),_tmp715))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12A,*_tmp123,_tmp124,_tmp12B,pat_loc);};}}
# 841
return inflow;};};}
# 844
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 846
void*_stmttmp1A=e->r;void*_tmp12F=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp130->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp131=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp130->f2);if(_tmp131->tag != 4)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp132->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp133=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp132->f2);if(_tmp133->tag != 3)goto _LLDE;}}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp134->tag != 1)goto _LLE0;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp135=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp134->f2);if(_tmp135->tag != 5)goto _LLE0;}}_LLDF:
 return 1;_LLE0: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp136->tag != 19)goto _LLE2;else{_tmp137=_tmp136->f1;}}_LLE1:
 _tmp139=_tmp137;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp138->tag != 20)goto _LLE4;else{_tmp139=_tmp138->f1;}}_LLE3:
 _tmp13B=_tmp139;goto _LLE5;_LLE4: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp13A->tag != 21)goto _LLE6;else{_tmp13B=_tmp13A->f1;}}_LLE5:
# 853
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13B);_LLE6: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp13C->tag != 22)goto _LLE8;else{_tmp13D=_tmp13C->f1;}}_LLE7: {
# 855
void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp140=_stmttmp1B;_LLED: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp140;if(_tmp141->tag != 10)goto _LLEF;}_LLEE:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13D);_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLE8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12F;if(_tmp13E->tag != 13)goto _LLEA;else{_tmp13F=_tmp13E->f2;}}_LLE9:
# 859
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13F);_LLEA:;_LLEB:
 return 0;_LLD9:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7C3(unsigned int*_tmp7C2,unsigned int*_tmp7C1,void***_tmp7BF,struct Cyc_List_List**_tmp31C){for(*_tmp7C2=0;*_tmp7C2 < *_tmp7C1;(*_tmp7C2)++){
# 1433 "new_control_flow.cyc"
void*_tmp7BD;(*_tmp7BF)[*_tmp7C2]=(void*)((_tmp7BD=(void*)((struct Cyc_List_List*)_check_null(*_tmp31C))->hd,((*_tmp31C=(*_tmp31C)->tl,_tmp7BD))));}}
# 865 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 869
struct Cyc_CfFlowInfo_FlowEnv*_tmp142=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 882
{union Cyc_CfFlowInfo_FlowInfo _tmp143=inflow;struct Cyc_Dict_Dict _tmp144;struct Cyc_List_List*_tmp145;_LLF2: if((_tmp143.BottomFL).tag != 1)goto _LLF4;_LLF3: {
struct _tuple15 _tmp717;return(_tmp717.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp717.f2=_tmp142->unknown_all,_tmp717)));}_LLF4: if((_tmp143.ReachableFL).tag != 2)goto _LLF1;_tmp144=((struct _tuple14)(_tmp143.ReachableFL).val).f1;_tmp145=((struct _tuple14)(_tmp143.ReachableFL).val).f2;_LLF5:
 d=_tmp144;relns=_tmp145;_LLF1:;}
# 897 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 918 "new_control_flow.cyc"
struct _tuple17 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp148;union Cyc_CfFlowInfo_AbsLVal _tmp149;struct _tuple17 _tmp147=_stmttmp1C;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;{
struct _tuple17 _tmp718;struct _tuple17 _stmttmp1D=(_tmp718.f1=_tmp148,((_tmp718.f2=_tmp149,_tmp718)));struct _tuple17 _tmp14A=_stmttmp1D;struct Cyc_Dict_Dict _tmp14B;struct Cyc_List_List*_tmp14C;struct Cyc_CfFlowInfo_Place*_tmp14D;_LLF7: if(((_tmp14A.f1).ReachableFL).tag != 2)goto _LLF9;_tmp14B=((struct _tuple14)((_tmp14A.f1).ReachableFL).val).f1;_tmp14C=((struct _tuple14)((_tmp14A.f1).ReachableFL).val).f2;if(((_tmp14A.f2).PlaceL).tag != 1)goto _LLF9;_tmp14D=(struct Cyc_CfFlowInfo_Place*)((_tmp14A.f2).PlaceL).val;_LLF8: {
# 921
void*_tmp14E=Cyc_CfFlowInfo_lookup_place(_tmp14B,_tmp14D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp14E,& needs_unconsume)){
{const char*_tmp71B;void*_tmp71A;(_tmp71A=0,Cyc_Tcutil_terr(e->loc,((_tmp71B="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp71B,sizeof(char),71))),_tag_dyneither(_tmp71A,sizeof(void*),0)));}{
struct _tuple15 _tmp71C;return(_tmp71C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71C.f2=_tmp142->unknown_all,_tmp71C)));};}else{
# 927
if(needs_unconsume){
# 929
struct _tuple15 _tmp71D;return(_tmp71D.f1=_tmp148,((_tmp71D.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp142,_tmp14E),_tmp71D)));}else{
# 932
void*_tmp153=Cyc_CfFlowInfo_make_unique_consumed(_tmp142,(void*)_check_null(e->topt),e,env->iteration_num,_tmp14E);
struct Cyc_Dict_Dict _tmp154=Cyc_CfFlowInfo_assign_place(_tmp142,e->loc,_tmp14B,env->all_changed,_tmp14D,_tmp153);
# 943
struct _tuple15 _tmp71E;return(_tmp71E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp154,_tmp14C),((_tmp71E.f2=_tmp14E,_tmp71E)));}}}_LLF9:;_LLFA:
# 945
 goto _LLF6;_LLF6:;};}{
# 948
void*_stmttmp1E=e->r;void*_tmp157=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp170;struct Cyc_Absyn_Vardecl*_tmp173;struct Cyc_Absyn_Vardecl*_tmp176;enum Cyc_Absyn_Primop _tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_List_List*_tmp189;int _tmp18B;struct Cyc_Absyn_Exp*_tmp18C;void**_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;int _tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct _dyneither_ptr*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct _dyneither_ptr*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;struct _dyneither_ptr*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Datatypedecl*_tmp1B2;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B8;enum Cyc_Absyn_AggrKind _tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C2;struct Cyc_Absyn_Exp*_tmp1C4;void*_tmp1C5;int _tmp1C6;struct Cyc_Absyn_Stmt*_tmp1C8;_LLFC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp158->tag != 13)goto _LLFE;else{_tmp159=_tmp158->f2;if(_tmp158->f4 != Cyc_Absyn_NonNull_to_Null)goto _LLFE;}}_LLFD: {
# 952
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp159);union Cyc_CfFlowInfo_FlowInfo _tmp1D0;void*_tmp1D1;struct _tuple15 _tmp1CF=_stmttmp4F;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;{
struct _tuple15 _stmttmp50=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D0,_tmp159,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp1D3;void*_tmp1D4;struct _tuple15 _tmp1D2=_stmttmp50;_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;{
struct _tuple15 _tmp71F;return(_tmp71F.f1=_tmp1D3,((_tmp71F.f2=_tmp1D1,_tmp71F)));};};}_LLFE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15A->tag != 13)goto _LL100;else{_tmp15B=_tmp15A->f2;}}_LLFF:
# 958
 _tmp15D=_tmp15B;goto _LL101;_LL100: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15C->tag != 11)goto _LL102;else{_tmp15D=_tmp15C->f1;}}_LL101:
 _tmp15F=_tmp15D;goto _LL103;_LL102: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp15E->tag != 12)goto _LL104;else{_tmp15F=_tmp15E->f1;}}_LL103:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15F);_LL104: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp160->tag != 0)goto _LL106;else{if(((_tmp160->f1).Null_c).tag != 1)goto _LL106;}}_LL105:
# 962
 goto _LL107;_LL106: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp161->tag != 0)goto _LL108;else{if(((_tmp161->f1).Int_c).tag != 5)goto _LL108;if(((struct _tuple7)((_tmp161->f1).Int_c).val).f2 != 0)goto _LL108;}}_LL107: {
struct _tuple15 _tmp720;return(_tmp720.f1=inflow,((_tmp720.f2=_tmp142->zero,_tmp720)));}_LL108: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp162->tag != 0)goto _LL10A;else{if(((_tmp162->f1).Int_c).tag != 5)goto _LL10A;}}_LL109:
 goto _LL10B;_LL10A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp163->tag != 1)goto _LL10C;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp164=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp163->f2);if(_tmp164->tag != 2)goto _LL10C;}}_LL10B: {
struct _tuple15 _tmp721;return(_tmp721.f1=inflow,((_tmp721.f2=_tmp142->notzeroall,_tmp721)));}_LL10C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp165->tag != 30)goto _LL10E;else{if(_tmp165->f1 != 0)goto _LL10E;}}_LL10D:
# 967
 goto _LL10F;_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp166->tag != 0)goto _LL110;}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp167->tag != 17)goto _LL112;}_LL111:
 goto _LL113;_LL112: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp168->tag != 16)goto _LL114;}_LL113:
 goto _LL115;_LL114: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp169->tag != 18)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16A->tag != 32)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16B->tag != 31)goto _LL11A;}_LL119: {
struct _tuple15 _tmp722;return(_tmp722.f1=inflow,((_tmp722.f2=_tmp142->unknown_all,_tmp722)));}_LL11A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16C->tag != 1)goto _LL11C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp16C->f2);if(_tmp16D->tag != 1)goto _LL11C;}}_LL11B: {
# 976
struct _tuple15 _tmp723;return(_tmp723.f1=inflow,((_tmp723.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp723)));}_LL11C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp16E->tag != 1)goto _LL11E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 3)goto _LL11E;else{_tmp170=_tmp16F->f1;}}}_LL11D:
# 979
 _tmp173=_tmp170;goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp171->tag != 1)goto _LL120;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 4)goto _LL120;else{_tmp173=_tmp172->f1;}}}_LL11F:
 _tmp176=_tmp173;goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp174->tag != 1)goto _LL122;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp175=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp174->f2);if(_tmp175->tag != 5)goto _LL122;else{_tmp176=_tmp175->f1;}}}_LL121: {
# 983
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp724;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp724.tag=0,((_tmp724.f1=_tmp176,_tmp724)));
struct _tuple15 _tmp725;return(_tmp725.f1=inflow,((_tmp725.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp725)));}_LL122: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp177->tag != 2)goto _LL124;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}_LL123: {
# 990
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp179);union Cyc_CfFlowInfo_FlowInfo _tmp1DD;void*_tmp1DE;struct _tuple15 _tmp1DC=_stmttmp4E;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;{
struct _tuple15 _tmp726;return(_tmp726.f1=_tmp1DD,((_tmp726.f2=_tmp1DE,_tmp726)));};}_LL124: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17A->tag != 4)goto _LL126;else{_tmp17B=_tmp17A->f1;}}_LL125: {
# 994
struct Cyc_List_List _tmp727;struct Cyc_List_List _tmp1E0=(_tmp727.hd=_tmp17B,((_tmp727.tl=0,_tmp727)));
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1E0);union Cyc_CfFlowInfo_FlowInfo _tmp1E2;struct _tuple15 _tmp1E1=_stmttmp4B;_tmp1E2=_tmp1E1.f1;{
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E2,0,0,_tmp17B);union Cyc_CfFlowInfo_AbsLVal _tmp1E4;struct _tuple17 _tmp1E3=_stmttmp4C;_tmp1E4=_tmp1E3.f2;
{struct _tuple24 _tmp728;struct _tuple24 _stmttmp4D=(_tmp728.f1=_tmp1E4,((_tmp728.f2=_tmp1E2,_tmp728)));struct _tuple24 _tmp1E5=_stmttmp4D;struct Cyc_CfFlowInfo_Place*_tmp1E6;struct Cyc_Dict_Dict _tmp1E7;struct Cyc_List_List*_tmp1E8;_LL165: if(((_tmp1E5.f1).PlaceL).tag != 1)goto _LL167;_tmp1E6=(struct Cyc_CfFlowInfo_Place*)((_tmp1E5.f1).PlaceL).val;if(((_tmp1E5.f2).ReachableFL).tag != 2)goto _LL167;_tmp1E7=((struct _tuple14)((_tmp1E5.f2).ReachableFL).val).f1;_tmp1E8=((struct _tuple14)((_tmp1E5.f2).ReachableFL).val).f2;_LL166:
# 999
 _tmp1E8=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp1E8,_tmp17B);
_tmp1E2=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp142,_tmp17B->loc,_tmp1E7,env->all_changed,_tmp1E6,_tmp142->unknown_all),_tmp1E8);
# 1003
goto _LL164;_LL167:;_LL168:
 goto _LL164;_LL164:;}{
# 1006
struct _tuple15 _tmp729;return(_tmp729.f1=_tmp1E2,((_tmp729.f2=_tmp142->unknown_all,_tmp729)));};};}_LL126: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17C->tag != 3)goto _LL128;else{_tmp17D=_tmp17C->f1;if(_tmp17C->f2 == 0)goto _LL128;_tmp17E=_tmp17C->f3;}}_LL127:
# 1009
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_Tcutil_terr(e->loc,((_tmp72C="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72C,sizeof(char),58))),_tag_dyneither(_tmp72B,sizeof(void*),0)));}{
struct _tuple15 _tmp72D;return(_tmp72D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp72D.f2=_tmp142->unknown_all,_tmp72D)));};}{
# 1013
struct Cyc_List_List _tmp72E;struct Cyc_List_List _tmp1EF=(_tmp72E.hd=_tmp17E,((_tmp72E.tl=0,_tmp72E)));
struct Cyc_List_List _tmp72F;struct Cyc_List_List _tmp1F0=(_tmp72F.hd=_tmp17D,((_tmp72F.tl=& _tmp1EF,_tmp72F)));
struct _tuple15 _stmttmp49=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F0);union Cyc_CfFlowInfo_FlowInfo _tmp1F2;struct _tuple15 _tmp1F1=_stmttmp49;_tmp1F2=_tmp1F1.f1;{
struct _tuple17 _stmttmp4A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F2,0,0,_tmp17D);union Cyc_CfFlowInfo_AbsLVal _tmp1F4;struct _tuple17 _tmp1F3=_stmttmp4A;_tmp1F4=_tmp1F3.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1F5=_tmp1F2;struct Cyc_Dict_Dict _tmp1F6;struct Cyc_List_List*_tmp1F7;_LL16A: if((_tmp1F5.ReachableFL).tag != 2)goto _LL16C;_tmp1F6=((struct _tuple14)(_tmp1F5.ReachableFL).val).f1;_tmp1F7=((struct _tuple14)(_tmp1F5.ReachableFL).val).f2;_LL16B:
# 1019
{union Cyc_CfFlowInfo_AbsLVal _tmp1F8=_tmp1F4;struct Cyc_CfFlowInfo_Place*_tmp1F9;_LL16F: if((_tmp1F8.PlaceL).tag != 1)goto _LL171;_tmp1F9=(struct Cyc_CfFlowInfo_Place*)(_tmp1F8.PlaceL).val;_LL170:
# 1023
 _tmp1F7=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp1F7,_tmp17D);
_tmp1F6=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp17D->loc,_tmp1F6,env->all_changed,_tmp1F9,_tmp142->unknown_all);
# 1026
_tmp1F2=Cyc_CfFlowInfo_ReachableFL(_tmp1F6,_tmp1F7);
# 1029
goto _LL16E;_LL171: if((_tmp1F8.UnknownL).tag != 2)goto _LL16E;_LL172:
# 1032
 goto _LL16E;_LL16E:;}
# 1034
goto _LL169;_LL16C:;_LL16D:
 goto _LL169;_LL169:;}{
# 1037
struct _tuple15 _tmp730;return(_tmp730.f1=_tmp1F2,((_tmp730.f2=_tmp142->unknown_all,_tmp730)));};};};_LL128: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp17F->tag != 3)goto _LL12A;else{_tmp180=_tmp17F->f1;if(_tmp17F->f2 != 0)goto _LL12A;_tmp181=_tmp17F->f3;}}_LL129:
# 1041
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_terr(e->loc,((_tmp733="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp733,sizeof(char),58))),_tag_dyneither(_tmp732,sizeof(void*),0)));}{
struct _tuple15 _tmp734;return(_tmp734.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp734.f2=_tmp142->unknown_all,_tmp734)));};}{
# 1045
struct Cyc_Dict_Dict*_tmp200=env->all_changed;
# 1047
{struct Cyc_Absyn_Exp*_tmp735[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp735[1]=_tmp181,((_tmp735[0]=_tmp180,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp735,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp736;env->all_changed=((_tmp736=_region_malloc(env->r,sizeof(*_tmp736)),((_tmp736[0]=_tmp142->mt_place_set,_tmp736))));}{
struct _tuple17 _stmttmp47=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp180);union Cyc_CfFlowInfo_FlowInfo _tmp204;union Cyc_CfFlowInfo_AbsLVal _tmp205;struct _tuple17 _tmp203=_stmttmp47;_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;{
struct Cyc_Dict_Dict _tmp206=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp737;env->all_changed=((_tmp737=_region_malloc(env->r,sizeof(*_tmp737)),((_tmp737[0]=_tmp142->mt_place_set,_tmp737))));}{
struct _tuple15 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp181);union Cyc_CfFlowInfo_FlowInfo _tmp209;void*_tmp20A;struct _tuple15 _tmp208=_stmttmp48;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;{
struct Cyc_Dict_Dict _tmp20B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp20C=Cyc_CfFlowInfo_after_flow(_tmp142,& _tmp206,_tmp204,_tmp209,_tmp206,_tmp20B);
# 1058
union Cyc_CfFlowInfo_FlowInfo _tmp20D=Cyc_CfFlowInfo_join_flow(_tmp142,_tmp200,inflow,_tmp20C);
# 1061
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20D,inflow)){
if(_tmp200 == 0)
env->all_changed=0;else{
# 1065
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp200,
Cyc_CfFlowInfo_union_place_set(_tmp206,_tmp20B,0),0);}
# 1068
return Cyc_NewControlFlow_do_assign(_tmp142,env,_tmp20C,_tmp180,_tmp205,_tmp181,_tmp20A,e->loc);}
# 1071
inflow=_tmp20D;};};};};}};_LL12A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp182->tag != 8)goto _LL12C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL12B: {
# 1076
struct _tuple15 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp183);union Cyc_CfFlowInfo_FlowInfo _tmp20F;void*_tmp210;struct _tuple15 _tmp20E=_stmttmp46;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp20F,_tmp184);}_LL12C: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp185->tag != 10)goto _LL12E;else{_tmp186=_tmp185->f1;}}_LL12D: {
# 1080
struct _tuple15 _stmttmp45=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp186);union Cyc_CfFlowInfo_FlowInfo _tmp212;void*_tmp213;struct _tuple15 _tmp211=_stmttmp45;_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp186->loc,_tmp212,_tmp213);{
struct _tuple15 _tmp738;return(_tmp738.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp738.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp738)));};}_LL12E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp187->tag != 9)goto _LL130;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LL12F: {
# 1085
struct _RegionHandle _tmp215=_new_region("temp");struct _RegionHandle*temp=& _tmp215;_push_region(temp);
{struct Cyc_List_List*_tmp739;struct _tuple22 _stmttmp3F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp739=_region_malloc(temp,sizeof(*_tmp739)),((_tmp739->hd=_tmp188,((_tmp739->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp189),_tmp739)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp217;struct Cyc_List_List*_tmp218;struct _tuple22 _tmp216=_stmttmp3F;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;{
# 1088
union Cyc_CfFlowInfo_FlowInfo _tmp219=Cyc_NewControlFlow_use_Rval(env,_tmp188->loc,_tmp217,(void*)((struct Cyc_List_List*)_check_null(_tmp218))->hd);
_tmp218=_tmp218->tl;{
# 1091
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp188->topt));void*_tmp21A=_stmttmp40;void*_tmp21C;_LL174: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A;if(_tmp21B->tag != 5)goto _LL176;else{_tmp21C=(_tmp21B->f1).elt_typ;}}_LL175:
# 1096
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp21C);void*_tmp21D=_stmttmp41;struct Cyc_List_List*_tmp21F;_LL179: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 9)goto _LL17B;else{_tmp21F=(_tmp21E->f1).attributes;}}_LL17A:
# 1098
 for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
# 1100
void*_stmttmp42=(void*)_tmp21F->hd;void*_tmp220=_stmttmp42;int _tmp222;int _tmp224;int _tmp226;_LL17E: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp221=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp220;if(_tmp221->tag != 20)goto _LL180;else{_tmp222=_tmp221->f1;}}_LL17F:
# 1102
{struct Cyc_List_List*_tmp73A;init_params=((_tmp73A=_region_malloc(temp,sizeof(*_tmp73A)),((_tmp73A->hd=(void*)_tmp222,((_tmp73A->tl=init_params,_tmp73A))))));}goto _LL17D;_LL180: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp223=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp220;if(_tmp223->tag != 21)goto _LL182;else{_tmp224=_tmp223->f1;}}_LL181:
# 1104
{struct Cyc_List_List*_tmp73B;nolive_unique_params=((_tmp73B=_region_malloc(temp,sizeof(*_tmp73B)),((_tmp73B->hd=(void*)_tmp224,((_tmp73B->tl=nolive_unique_params,_tmp73B))))));}
goto _LL17D;_LL182: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp225=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp220;if(_tmp225->tag != 22)goto _LL184;else{_tmp226=_tmp225->f1;}}_LL183:
# 1108
{struct Cyc_List_List*_tmp73C;noconsume_params=((_tmp73C=_region_malloc(temp,sizeof(*_tmp73C)),((_tmp73C->hd=(void*)_tmp226,((_tmp73C->tl=noconsume_params,_tmp73C))))));}
goto _LL17D;_LL184:;_LL185:
 goto _LL17D;_LL17D:;}
# 1112
goto _LL178;_LL17B:;_LL17C: {
const char*_tmp73F;void*_tmp73E;(_tmp73E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp73F="anal_Rexp: bad function type",_tag_dyneither(_tmp73F,sizeof(char),29))),_tag_dyneither(_tmp73E,sizeof(void*),0)));}_LL178:;}
# 1115
goto _LL173;_LL176:;_LL177: {
const char*_tmp742;void*_tmp741;(_tmp741=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp742="anal_Rexp: bad function type",_tag_dyneither(_tmp742,sizeof(char),29))),_tag_dyneither(_tmp741,sizeof(void*),0)));}_LL173:;}
# 1118
{int i=1;for(0;_tmp218 != 0;(((_tmp218=_tmp218->tl,_tmp189=((struct Cyc_List_List*)_check_null(_tmp189))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp22E=_tmp217;struct Cyc_Dict_Dict _tmp22F;_LL187: if((_tmp22E.BottomFL).tag != 1)goto _LL189;_LL188:
 goto _LL186;_LL189: if((_tmp22E.ReachableFL).tag != 2)goto _LL186;_tmp22F=((struct _tuple14)(_tmp22E.ReachableFL).val).f1;_LL18A:
# 1123
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp22F,(void*)_tmp218->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp745;void*_tmp744;(_tmp744=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,((_tmp745="expression may not be initialized",_tag_dyneither(_tmp745,sizeof(char),34))),_tag_dyneither(_tmp744,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp232=_tmp219;struct Cyc_Dict_Dict _tmp233;struct Cyc_List_List*_tmp234;_LL18C: if((_tmp232.BottomFL).tag != 1)goto _LL18E;_LL18D:
 goto _LL18B;_LL18E: if((_tmp232.ReachableFL).tag != 2)goto _LL18B;_tmp233=((struct _tuple14)(_tmp232.ReachableFL).val).f1;_tmp234=((struct _tuple14)(_tmp232.ReachableFL).val).f2;_LL18F: {
# 1130
struct Cyc_Dict_Dict _tmp235=Cyc_CfFlowInfo_escape_deref(_tmp142,_tmp233,env->all_changed,(void*)_tmp218->hd);
{void*_stmttmp43=(void*)_tmp218->hd;void*_tmp236=_stmttmp43;struct Cyc_CfFlowInfo_Place*_tmp238;_LL191: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp237=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp236;if(_tmp237->tag != 5)goto _LL193;else{_tmp238=_tmp237->f1;}}_LL192:
# 1133
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->topt));void*_tmp239=_stmttmp44;void*_tmp23B;_LL196: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239;if(_tmp23A->tag != 5)goto _LL198;else{_tmp23B=(_tmp23A->f1).elt_typ;}}_LL197:
# 1137
 _tmp235=Cyc_CfFlowInfo_assign_place(_tmp142,((struct Cyc_Absyn_Exp*)_tmp189->hd)->loc,_tmp235,env->all_changed,_tmp238,
# 1139
Cyc_CfFlowInfo_typ_to_absrval(_tmp142,_tmp23B,0,_tmp142->esc_all));
# 1141
goto _LL195;_LL198:;_LL199: {
const char*_tmp748;void*_tmp747;(_tmp747=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp748="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp748,sizeof(char),39))),_tag_dyneither(_tmp747,sizeof(void*),0)));}_LL195:;}
# 1144
goto _LL190;_LL193:;_LL194:
 goto _LL190;_LL190:;}
# 1147
_tmp219=Cyc_CfFlowInfo_ReachableFL(_tmp235,_tmp234);
goto _LL18B;}_LL18B:;}
# 1150
goto _LL186;_LL186:;}else{
# 1153
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1158
_tmp219=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->topt),_tmp219,(void*)_tmp218->hd);else{
# 1165
_tmp219=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->loc,_tmp219,(void*)_tmp218->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i))
_tmp219=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp219,(struct Cyc_Absyn_Exp*)_tmp189->hd,((struct Cyc_Absyn_Exp*)_tmp189->hd)->loc);}}}}
# 1171
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp188->topt))){
struct _tuple15 _tmp749;struct _tuple15 _tmp23F=(_tmp749.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp749.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp749)));_npop_handler(0);return _tmp23F;}else{
# 1174
struct _tuple15 _tmp74A;struct _tuple15 _tmp241=(_tmp74A.f1=_tmp219,((_tmp74A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp74A)));_npop_handler(0);return _tmp241;}};};}
# 1086
;_pop_region(temp);}_LL130: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp18A->tag != 33)goto _LL132;else{_tmp18B=(_tmp18A->f1).is_calloc;_tmp18C=(_tmp18A->f1).rgn;_tmp18D=(_tmp18A->f1).elt_type;_tmp18E=(_tmp18A->f1).num_elts;_tmp18F=(_tmp18A->f1).fat_result;}}_LL131: {
# 1177
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp74D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp74C;void*root=(void*)((_tmp74C=_region_malloc(_tmp142->r,sizeof(*_tmp74C)),((_tmp74C[0]=((_tmp74D.tag=1,((_tmp74D.f1=_tmp18E,((_tmp74D.f2=(void*)_check_null(e->topt),_tmp74D)))))),_tmp74C))));
struct Cyc_CfFlowInfo_Place*_tmp74E;struct Cyc_CfFlowInfo_Place*place=(_tmp74E=_region_malloc(_tmp142->r,sizeof(*_tmp74E)),((_tmp74E->root=root,((_tmp74E->fields=0,_tmp74E)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp751;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp750;void*rval=(void*)((_tmp750=_region_malloc(_tmp142->r,sizeof(*_tmp750)),((_tmp750[0]=((_tmp751.tag=5,((_tmp751.f1=place,_tmp751)))),_tmp750))));
void*place_val;
if(_tmp18F)place_val=_tmp142->notzeroall;else{
if(_tmp18B)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,*((void**)_check_null(_tmp18D)),0,_tmp142->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,*((void**)_check_null(_tmp18D)),0,_tmp142->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp18C != 0){
struct _RegionHandle _tmp243=_new_region("temp");struct _RegionHandle*temp=& _tmp243;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp752[2];struct _tuple22 _stmttmp3D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp752[1]=_tmp18E,((_tmp752[0]=_tmp18C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp752,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1188
union Cyc_CfFlowInfo_FlowInfo _tmp245;struct Cyc_List_List*_tmp246;struct _tuple22 _tmp244=_stmttmp3D;_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;
# 1190
outflow=_tmp245;}
# 1188
;_pop_region(temp);}else{
# 1193
struct _tuple15 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18E);union Cyc_CfFlowInfo_FlowInfo _tmp249;struct _tuple15 _tmp248=_stmttmp3E;_tmp249=_tmp248.f1;
outflow=_tmp249;}{
# 1196
union Cyc_CfFlowInfo_FlowInfo _tmp24A=outflow;struct Cyc_Dict_Dict _tmp24B;struct Cyc_List_List*_tmp24C;_LL19B: if((_tmp24A.BottomFL).tag != 1)goto _LL19D;_LL19C: {
struct _tuple15 _tmp753;return(_tmp753.f1=outflow,((_tmp753.f2=rval,_tmp753)));}_LL19D: if((_tmp24A.ReachableFL).tag != 2)goto _LL19A;_tmp24B=((struct _tuple14)(_tmp24A.ReachableFL).val).f1;_tmp24C=((struct _tuple14)(_tmp24A.ReachableFL).val).f2;_LL19E: {
# 1199
struct _tuple15 _tmp754;return(_tmp754.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp24B,root,place_val),_tmp24C),((_tmp754.f2=rval,_tmp754)));}_LL19A:;};};}_LL132: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp190->tag != 34)goto _LL134;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;}}_LL133: {
# 1203
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1209
struct _RegionHandle _tmp254=_new_region("temp");struct _RegionHandle*temp=& _tmp254;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp755[2];struct _tuple22 _stmttmp3A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp755[1]=_tmp192,((_tmp755[0]=_tmp191,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp755,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1210
union Cyc_CfFlowInfo_FlowInfo _tmp256;struct Cyc_List_List*_tmp257;struct _tuple22 _tmp255=_stmttmp3A;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;
# 1212
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp257))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp257->tl))->hd;
outflow=_tmp256;}{
# 1217
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp191);union Cyc_CfFlowInfo_AbsLVal _tmp25A;struct _tuple17 _tmp259=_stmttmp3B;_tmp25A=_tmp259.f2;{
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp192);union Cyc_CfFlowInfo_AbsLVal _tmp25C;struct _tuple17 _tmp25B=_stmttmp3C;_tmp25C=_tmp25B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp25D=outflow;struct Cyc_Dict_Dict _tmp25E;struct Cyc_List_List*_tmp25F;_LL1A0: if((_tmp25D.ReachableFL).tag != 2)goto _LL1A2;_tmp25E=((struct _tuple14)(_tmp25D.ReachableFL).val).f1;_tmp25F=((struct _tuple14)(_tmp25D.ReachableFL).val).f2;_LL1A1:
# 1221
{union Cyc_CfFlowInfo_AbsLVal _tmp260=_tmp25A;struct Cyc_CfFlowInfo_Place*_tmp261;_LL1A5: if((_tmp260.PlaceL).tag != 1)goto _LL1A7;_tmp261=(struct Cyc_CfFlowInfo_Place*)(_tmp260.PlaceL).val;_LL1A6:
# 1223
 _tmp25E=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp191->loc,_tmp25E,env->all_changed,_tmp261,right_rval);
# 1225
goto _LL1A4;_LL1A7: if((_tmp260.UnknownL).tag != 2)goto _LL1A4;_LL1A8:
 goto _LL1A4;_LL1A4:;}
# 1228
{union Cyc_CfFlowInfo_AbsLVal _tmp262=_tmp25C;struct Cyc_CfFlowInfo_Place*_tmp263;_LL1AA: if((_tmp262.PlaceL).tag != 1)goto _LL1AC;_tmp263=(struct Cyc_CfFlowInfo_Place*)(_tmp262.PlaceL).val;_LL1AB:
# 1230
 _tmp25E=Cyc_CfFlowInfo_assign_place(_tmp142,_tmp192->loc,_tmp25E,env->all_changed,_tmp263,left_rval);
# 1232
goto _LL1A9;_LL1AC: if((_tmp262.UnknownL).tag != 2)goto _LL1A9;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1236
_tmp25F=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp25F,_tmp191);
_tmp25F=Cyc_Relations_reln_kill_exp(_tmp142->r,_tmp25F,_tmp192);
# 1239
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp25E,_tmp25F);
goto _LL19F;_LL1A2:;_LL1A3:
 goto _LL19F;_LL19F:;}{
# 1245
struct _tuple15 _tmp756;struct _tuple15 _tmp265=(_tmp756.f1=outflow,((_tmp756.f2=_tmp142->unknown_all,_tmp756)));_npop_handler(0);return _tmp265;};};};
# 1209
;_pop_region(temp);}_LL134: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp193->tag != 15)goto _LL136;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;}}_LL135: {
# 1248
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp759;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp758;void*root=(void*)((_tmp758=_region_malloc(_tmp142->r,sizeof(*_tmp758)),((_tmp758[0]=((_tmp759.tag=1,((_tmp759.f1=_tmp195,((_tmp759.f2=(void*)_check_null(e->topt),_tmp759)))))),_tmp758))));
struct Cyc_CfFlowInfo_Place*_tmp75A;struct Cyc_CfFlowInfo_Place*place=(_tmp75A=_region_malloc(_tmp142->r,sizeof(*_tmp75A)),((_tmp75A->root=root,((_tmp75A->fields=0,_tmp75A)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp75D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp75C;void*rval=(void*)((_tmp75C=_region_malloc(_tmp142->r,sizeof(*_tmp75C)),((_tmp75C[0]=((_tmp75D.tag=5,((_tmp75D.f1=place,_tmp75D)))),_tmp75C))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp194 != 0){
struct _RegionHandle _tmp266=_new_region("temp");struct _RegionHandle*temp=& _tmp266;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp75E[2];struct _tuple22 _stmttmp38=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp75E[1]=_tmp195,((_tmp75E[0]=_tmp194,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp75E,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1256
union Cyc_CfFlowInfo_FlowInfo _tmp268;struct Cyc_List_List*_tmp269;struct _tuple22 _tmp267=_stmttmp38;_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;
# 1258
outflow=_tmp268;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp269))->tl))->hd;}
# 1256
;_pop_region(temp);}else{
# 1262
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp195);union Cyc_CfFlowInfo_FlowInfo _tmp26C;void*_tmp26D;struct _tuple15 _tmp26B=_stmttmp39;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;
outflow=_tmp26C;
place_val=_tmp26D;}{
# 1266
union Cyc_CfFlowInfo_FlowInfo _tmp26E=outflow;struct Cyc_Dict_Dict _tmp26F;struct Cyc_List_List*_tmp270;_LL1AF: if((_tmp26E.BottomFL).tag != 1)goto _LL1B1;_LL1B0: {
struct _tuple15 _tmp75F;return(_tmp75F.f1=outflow,((_tmp75F.f2=rval,_tmp75F)));}_LL1B1: if((_tmp26E.ReachableFL).tag != 2)goto _LL1AE;_tmp26F=((struct _tuple14)(_tmp26E.ReachableFL).val).f1;_tmp270=((struct _tuple14)(_tmp26E.ReachableFL).val).f2;_LL1B2: {
# 1269
struct _tuple15 _tmp760;return(_tmp760.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp26F,root,place_val),_tmp270),((_tmp760.f2=rval,_tmp760)));}_LL1AE:;};};}_LL136: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp196->tag != 14)goto _LL138;else{_tmp197=_tmp196->f1;}}_LL137: {
# 1273
struct _tuple17 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp197);union Cyc_CfFlowInfo_FlowInfo _tmp279;union Cyc_CfFlowInfo_AbsLVal _tmp27A;struct _tuple17 _tmp278=_stmttmp37;_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp27B=_tmp27A;struct Cyc_CfFlowInfo_Place*_tmp27C;_LL1B4: if((_tmp27B.UnknownL).tag != 2)goto _LL1B6;_LL1B5: {
struct _tuple15 _tmp761;return(_tmp761.f1=_tmp279,((_tmp761.f2=_tmp142->notzeroall,_tmp761)));}_LL1B6: if((_tmp27B.PlaceL).tag != 1)goto _LL1B3;_tmp27C=(struct Cyc_CfFlowInfo_Place*)(_tmp27B.PlaceL).val;_LL1B7: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp767;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp766;struct _tuple15 _tmp765;return(_tmp765.f1=_tmp279,((_tmp765.f2=(void*)((_tmp767=_region_malloc(env->r,sizeof(*_tmp767)),((_tmp767[0]=((_tmp766.tag=5,((_tmp766.f1=_tmp27C,_tmp766)))),_tmp767)))),_tmp765)));}_LL1B3:;};}_LL138: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp198->tag != 19)goto _LL13A;else{_tmp199=_tmp198->f1;}}_LL139: {
# 1280
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp282;void*_tmp283;struct _tuple15 _tmp281=_stmttmp36;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp282,_tmp199,_tmp283);}_LL13A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp19A->tag != 20)goto _LL13C;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_LL13B: {
# 1284
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19B);union Cyc_CfFlowInfo_FlowInfo _tmp285;void*_tmp286;struct _tuple15 _tmp284=_stmttmp35;_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;
if(_tmp19B->topt == 0){
{const char*_tmp76B;void*_tmp76A[1];struct Cyc_String_pa_PrintArg_struct _tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp76A[0]=& _tmp769,Cyc_fprintf(Cyc_stderr,((_tmp76B="aggrmember exp %s\n",_tag_dyneither(_tmp76B,sizeof(char),19))),_tag_dyneither(_tmp76A,sizeof(void*),1)))))));}{
const char*_tmp76F;void*_tmp76E[1];struct Cyc_String_pa_PrintArg_struct _tmp76D;(_tmp76D.tag=0,((_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp19B)),((_tmp76E[0]=& _tmp76D,Cyc_fprintf(Cyc_stderr,((_tmp76F="oops! %s.topt = null!\n",_tag_dyneither(_tmp76F,sizeof(char),23))),_tag_dyneither(_tmp76E,sizeof(void*),1)))))));};}{
# 1289
void*_tmp28D=(void*)_check_null(_tmp19B->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp28D)){
# 1292
struct _tuple15 _tmp770;return(_tmp770.f1=_tmp285,((_tmp770.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp770)));}
# 1294
if(Cyc_Absyn_is_require_union_type(_tmp28D))
Cyc_NewControlFlow_check_requires(_tmp19B->loc,_tmp142->r,_tmp28D,_tmp19C,_tmp285);{
# 1297
void*_tmp28F=_tmp286;int _tmp292;int _tmp293;struct _dyneither_ptr _tmp294;int _tmp296;int _tmp297;struct _dyneither_ptr _tmp298;_LL1B9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp290=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28F;if(_tmp290->tag != 8)goto _LL1BB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp291=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp290->f2);if(_tmp291->tag != 6)goto _LL1BB;else{_tmp292=(_tmp291->f1).is_union;_tmp293=(_tmp291->f1).fieldnum;_tmp294=_tmp291->f2;}}}_LL1BA:
# 1299
 _tmp296=_tmp292;_tmp297=_tmp293;_tmp298=_tmp294;goto _LL1BC;_LL1BB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp295=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F;if(_tmp295->tag != 6)goto _LL1BD;else{_tmp296=(_tmp295->f1).is_union;_tmp297=(_tmp295->f1).fieldnum;_tmp298=_tmp295->f2;}}_LL1BC: {
# 1301
int _tmp299=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19B->topt),_tmp19C);
if((_tmp296  && _tmp297 != - 1) && _tmp297 != _tmp299){
struct _tuple15 _tmp771;return(_tmp771.f1=_tmp285,((_tmp771.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),1,_tmp142->unknown_none),_tmp771)));}{
struct _tuple15 _tmp772;return(_tmp772.f1=_tmp285,((_tmp772.f2=*((void**)_check_dyneither_subscript(_tmp298,sizeof(void*),_tmp299)),_tmp772)));};}_LL1BD:;_LL1BE:
# 1306
{const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_fprintf(Cyc_stderr,((_tmp775="the bad rexp is :",_tag_dyneither(_tmp775,sizeof(char),18))),_tag_dyneither(_tmp774,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp286);
{const char*_tmp778;void*_tmp777;(_tmp777=0,Cyc_fprintf(Cyc_stderr,((_tmp778="\n",_tag_dyneither(_tmp778,sizeof(char),2))),_tag_dyneither(_tmp777,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp785;const char*_tmp784;void*_tmp783[1];struct Cyc_String_pa_PrintArg_struct _tmp782;struct Cyc_Core_Impossible_exn_struct*_tmp781;(int)_throw((void*)((_tmp781=_cycalloc(sizeof(*_tmp781)),((_tmp781[0]=((_tmp785.tag=Cyc_Core_Impossible,((_tmp785.f1=(struct _dyneither_ptr)((_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp783[0]=& _tmp782,Cyc_aprintf(((_tmp784="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp784,sizeof(char),26))),_tag_dyneither(_tmp783,sizeof(void*),1)))))))),_tmp785)))),_tmp781)))));};_LL1B8:;};};}_LL13C: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp19D->tag != 37)goto _LL13E;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}_LL13D: {
# 1316
struct _tuple15 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19E);union Cyc_CfFlowInfo_FlowInfo _tmp2A6;void*_tmp2A7;struct _tuple15 _tmp2A5=_stmttmp34;_tmp2A6=_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;
# 1318
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp19E->topt))){
struct _tuple15 _tmp786;return(_tmp786.f1=_tmp2A6,((_tmp786.f2=_tmp142->unknown_all,_tmp786)));}{
void*_tmp2A9=_tmp2A7;int _tmp2AC;int _tmp2AD;struct _dyneither_ptr _tmp2AE;int _tmp2B0;int _tmp2B1;struct _dyneither_ptr _tmp2B2;_LL1C0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2AA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A9;if(_tmp2AA->tag != 8)goto _LL1C2;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2AA->f2);if(_tmp2AB->tag != 6)goto _LL1C2;else{_tmp2AC=(_tmp2AB->f1).is_union;_tmp2AD=(_tmp2AB->f1).fieldnum;_tmp2AE=_tmp2AB->f2;}}}_LL1C1:
# 1322
 _tmp2B0=_tmp2AC;_tmp2B1=_tmp2AD;_tmp2B2=_tmp2AE;goto _LL1C3;_LL1C2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2A9;if(_tmp2AF->tag != 6)goto _LL1C4;else{_tmp2B0=(_tmp2AF->f1).is_union;_tmp2B1=(_tmp2AF->f1).fieldnum;_tmp2B2=_tmp2AF->f2;}}_LL1C3: {
# 1324
int _tmp2B3=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19E->topt),_tmp19F);
if(_tmp2B0  && _tmp2B1 != - 1){
if(_tmp2B1 != _tmp2B3){
struct _tuple15 _tmp787;return(_tmp787.f1=_tmp2A6,((_tmp787.f2=_tmp142->zero,_tmp787)));}else{
# 1329
struct _tuple15 _tmp788;return(_tmp788.f1=_tmp2A6,((_tmp788.f2=_tmp142->notzeroall,_tmp788)));}}else{
# 1331
struct _tuple15 _tmp789;return(_tmp789.f1=_tmp2A6,((_tmp789.f2=_tmp142->unknown_all,_tmp789)));}}_LL1C4:;_LL1C5: {
# 1333
struct Cyc_Core_Impossible_exn_struct _tmp796;const char*_tmp795;void*_tmp794[1];struct Cyc_String_pa_PrintArg_struct _tmp793;struct Cyc_Core_Impossible_exn_struct*_tmp792;(int)_throw((void*)((_tmp792=_cycalloc(sizeof(*_tmp792)),((_tmp792[0]=((_tmp796.tag=Cyc_Core_Impossible,((_tmp796.f1=(struct _dyneither_ptr)((_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp794[0]=& _tmp793,Cyc_aprintf(((_tmp795="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp795,sizeof(char),26))),_tag_dyneither(_tmp794,sizeof(void*),1)))))))),_tmp796)))),_tmp792)))));}_LL1BF:;};}_LL13E: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A0->tag != 21)goto _LL140;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}}_LL13F: {
# 1338
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A1);union Cyc_CfFlowInfo_FlowInfo _tmp2BD;void*_tmp2BE;struct _tuple15 _tmp2BC=_stmttmp31;_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{
# 1341
struct _tuple15 _stmttmp32=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BD,_tmp1A1,_tmp2BE);union Cyc_CfFlowInfo_FlowInfo _tmp2C0;void*_tmp2C1;struct _tuple15 _tmp2BF=_stmttmp32;_tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;{
# 1344
void*_stmttmp33=Cyc_Tcutil_compress((void*)_check_null(_tmp1A1->topt));void*_tmp2C2=_stmttmp33;void*_tmp2C4;_LL1C7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C2;if(_tmp2C3->tag != 5)goto _LL1C9;else{_tmp2C4=(_tmp2C3->f1).elt_typ;}}_LL1C8:
# 1346
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C4)){
# 1348
struct _tuple15 _tmp797;return(_tmp797.f1=_tmp2C0,((_tmp797.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp797)));}
# 1350
if(Cyc_Absyn_is_require_union_type(_tmp2C4))
Cyc_NewControlFlow_check_requires(_tmp1A1->loc,_tmp142->r,_tmp2C4,_tmp1A2,_tmp2BD);{
# 1353
void*_tmp2C6=_tmp2C1;int _tmp2C9;int _tmp2CA;struct _dyneither_ptr _tmp2CB;int _tmp2CD;int _tmp2CE;struct _dyneither_ptr _tmp2CF;_LL1CC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2C7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2C6;if(_tmp2C7->tag != 8)goto _LL1CE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2C7->f2);if(_tmp2C8->tag != 6)goto _LL1CE;else{_tmp2C9=(_tmp2C8->f1).is_union;_tmp2CA=(_tmp2C8->f1).fieldnum;_tmp2CB=_tmp2C8->f2;}}}_LL1CD:
# 1355
 _tmp2CD=_tmp2C9;_tmp2CE=_tmp2CA;_tmp2CF=_tmp2CB;goto _LL1CF;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C6;if(_tmp2CC->tag != 6)goto _LL1D0;else{_tmp2CD=(_tmp2CC->f1).is_union;_tmp2CE=(_tmp2CC->f1).fieldnum;_tmp2CF=_tmp2CC->f2;}}_LL1CF: {
# 1357
int _tmp2D0=Cyc_CfFlowInfo_get_field_index(_tmp2C4,_tmp1A2);
if((_tmp2CD  && _tmp2CE != - 1) && _tmp2CE != _tmp2D0){
struct _tuple15 _tmp798;return(_tmp798.f1=_tmp2C0,((_tmp798.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),1,_tmp142->unknown_none),_tmp798)));}{
struct _tuple15 _tmp799;return(_tmp799.f1=_tmp2C0,((_tmp799.f2=*((void**)_check_dyneither_subscript(_tmp2CF,sizeof(void*),_tmp2D0)),_tmp799)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp79F;const char*_tmp79E;struct Cyc_Core_Impossible_exn_struct*_tmp79D;(int)_throw((void*)((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=((_tmp79F.tag=Cyc_Core_Impossible,((_tmp79F.f1=((_tmp79E="anal_Rexp: AggrArrow",_tag_dyneither(_tmp79E,sizeof(char),21))),_tmp79F)))),_tmp79D)))));}_LL1CB:;};_LL1C9:;_LL1CA: {
# 1363
struct Cyc_Core_Impossible_exn_struct _tmp7A5;const char*_tmp7A4;struct Cyc_Core_Impossible_exn_struct*_tmp7A3;(int)_throw((void*)((_tmp7A3=_cycalloc(sizeof(*_tmp7A3)),((_tmp7A3[0]=((_tmp7A5.tag=Cyc_Core_Impossible,((_tmp7A5.f1=((_tmp7A4="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7A4,sizeof(char),25))),_tmp7A5)))),_tmp7A3)))));}_LL1C6:;};};}_LL140: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A3->tag != 5)goto _LL142;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;_tmp1A6=_tmp1A3->f3;}}_LL141: {
# 1367
struct _tuple18 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A4);union Cyc_CfFlowInfo_FlowInfo _tmp2DA;union Cyc_CfFlowInfo_FlowInfo _tmp2DB;struct _tuple18 _tmp2D9=_stmttmp30;_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;{
struct _tuple15 _tmp2DC=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DA,_tmp1A5);
struct _tuple15 _tmp2DD=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DB,_tmp1A6);
# 1371
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2DC,_tmp2DD);};}_LL142: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1A7->tag != 6)goto _LL144;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL143: {
# 1374
struct _tuple18 _stmttmp2E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A8);union Cyc_CfFlowInfo_FlowInfo _tmp2DF;union Cyc_CfFlowInfo_FlowInfo _tmp2E0;struct _tuple18 _tmp2DE=_stmttmp2E;_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;{
struct _tuple15 _stmttmp2F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DF,_tmp1A9);union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;struct _tuple15 _tmp2E1=_stmttmp2F;_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{
struct _tuple15 _tmp7A6;struct _tuple15 _tmp2E4=(_tmp7A6.f1=_tmp2E2,((_tmp7A6.f2=_tmp2E3,_tmp7A6)));
struct _tuple15 _tmp7A7;struct _tuple15 _tmp2E5=(_tmp7A7.f1=_tmp2E0,((_tmp7A7.f2=_tmp142->zero,_tmp7A7)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2E4,_tmp2E5);};};}_LL144: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1AA->tag != 7)goto _LL146;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}_LL145: {
# 1381
struct _tuple18 _stmttmp2C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1AB);union Cyc_CfFlowInfo_FlowInfo _tmp2E9;union Cyc_CfFlowInfo_FlowInfo _tmp2EA;struct _tuple18 _tmp2E8=_stmttmp2C;_tmp2E9=_tmp2E8.f1;_tmp2EA=_tmp2E8.f2;{
struct _tuple15 _stmttmp2D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EA,_tmp1AC);union Cyc_CfFlowInfo_FlowInfo _tmp2EC;void*_tmp2ED;struct _tuple15 _tmp2EB=_stmttmp2D;_tmp2EC=_tmp2EB.f1;_tmp2ED=_tmp2EB.f2;{
struct _tuple15 _tmp7A8;struct _tuple15 _tmp2EE=(_tmp7A8.f1=_tmp2EC,((_tmp7A8.f2=_tmp2ED,_tmp7A8)));
struct _tuple15 _tmp7A9;struct _tuple15 _tmp2EF=(_tmp7A9.f1=_tmp2E9,((_tmp7A9.f2=_tmp142->notzeroall,_tmp7A9)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp142,env->all_changed,_tmp2EE,_tmp2EF);};};}_LL146: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1AD->tag != 22)goto _LL148;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LL147: {
# 1388
struct _RegionHandle _tmp2F2=_new_region("temp");struct _RegionHandle*temp=& _tmp2F2;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7AA[2];struct _tuple22 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7AA[1]=_tmp1AF,((_tmp7AA[0]=_tmp1AE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7AA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1389
union Cyc_CfFlowInfo_FlowInfo _tmp2F4;struct Cyc_List_List*_tmp2F5;struct _tuple22 _tmp2F3=_stmttmp28;_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{
# 1393
union Cyc_CfFlowInfo_FlowInfo _tmp2F6=_tmp2F4;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F7=_tmp2F4;struct Cyc_Dict_Dict _tmp2F8;struct Cyc_List_List*_tmp2F9;_LL1D3: if((_tmp2F7.ReachableFL).tag != 2)goto _LL1D5;_tmp2F8=((struct _tuple14)(_tmp2F7.ReachableFL).val).f1;_tmp2F9=((struct _tuple14)(_tmp2F7.ReachableFL).val).f2;_LL1D4:
# 1398
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2F8,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F5))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_Tcutil_terr(_tmp1AF->loc,((_tmp7AD="expression may not be initialized",_tag_dyneither(_tmp7AD,sizeof(char),34))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2FC=Cyc_NewControlFlow_add_subscript_reln(_tmp142->r,_tmp2F9,_tmp1AE,_tmp1AF);
if(_tmp2F9 != _tmp2FC)
_tmp2F6=Cyc_CfFlowInfo_ReachableFL(_tmp2F8,_tmp2FC);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1406
void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp1AE->topt));void*_tmp2FD=_stmttmp29;struct Cyc_List_List*_tmp2FF;union Cyc_Absyn_Constraint*_tmp301;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FD;if(_tmp2FE->tag != 10)goto _LL1DA;else{_tmp2FF=_tmp2FE->f1;}}_LL1D9: {
# 1408
void*_stmttmp2B=(void*)((struct Cyc_List_List*)_check_null(_tmp2F5))->hd;void*_tmp302=_stmttmp2B;struct _dyneither_ptr _tmp305;struct _dyneither_ptr _tmp307;_LL1DF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp303=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp302;if(_tmp303->tag != 8)goto _LL1E1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp304=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp303->f2);if(_tmp304->tag != 6)goto _LL1E1;else{_tmp305=_tmp304->f2;}}}_LL1E0:
# 1410
 _tmp307=_tmp305;goto _LL1E2;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp306=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp302;if(_tmp306->tag != 6)goto _LL1E3;else{_tmp307=_tmp306->f2;}}_LL1E2: {
# 1412
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1AF)).f1;
struct _tuple15 _tmp7AE;struct _tuple15 _tmp309=(_tmp7AE.f1=_tmp2F6,((_tmp7AE.f2=*((void**)_check_dyneither_subscript(_tmp307,sizeof(void*),(int)i)),_tmp7AE)));_npop_handler(0);return _tmp309;}_LL1E3:;_LL1E4: {
struct Cyc_Core_Impossible_exn_struct _tmp7B4;const char*_tmp7B3;struct Cyc_Core_Impossible_exn_struct*_tmp7B2;(int)_throw((void*)((_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B4.tag=Cyc_Core_Impossible,((_tmp7B4.f1=((_tmp7B3="anal_Rexp: Subscript",_tag_dyneither(_tmp7B3,sizeof(char),21))),_tmp7B4)))),_tmp7B2)))));}_LL1DE:;}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD;if(_tmp300->tag != 5)goto _LL1DC;else{_tmp301=((_tmp300->f1).ptr_atts).bounds;}}_LL1DB: {
# 1417
struct _tuple15 _stmttmp2A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F4,_tmp1AE,(void*)((struct Cyc_List_List*)_check_null(_tmp2F5))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp30E;void*_tmp30F;struct _tuple15 _tmp30D=_stmttmp2A;_tmp30E=_tmp30D.f1;_tmp30F=_tmp30D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp310=_tmp30E;_LL1E6: if((_tmp310.BottomFL).tag != 1)goto _LL1E8;_LL1E7: {
struct _tuple15 _tmp7B5;struct _tuple15 _tmp312=(_tmp7B5.f1=_tmp30E,((_tmp7B5.f2=_tmp30F,_tmp7B5)));_npop_handler(0);return _tmp312;}_LL1E8:;_LL1E9: {
struct _tuple15 _tmp7B6;struct _tuple15 _tmp314=(_tmp7B6.f1=_tmp2F6,((_tmp7B6.f2=_tmp30F,_tmp7B6)));_npop_handler(0);return _tmp314;}_LL1E5:;};}_LL1DC:;_LL1DD: {
# 1422
struct Cyc_Core_Impossible_exn_struct _tmp7BC;const char*_tmp7BB;struct Cyc_Core_Impossible_exn_struct*_tmp7BA;(int)_throw((void*)((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BC.tag=Cyc_Core_Impossible,((_tmp7BC.f1=((_tmp7BB="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7BB,sizeof(char),33))),_tmp7BC)))),_tmp7BA)))));}_LL1D7:;};};}
# 1389
;_pop_region(temp);}_LL148: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1B0=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B0->tag != 30)goto _LL14A;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL149:
# 1427
 _tmp1B4=_tmp1B1;goto _LL14B;_LL14A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B3->tag != 23)goto _LL14C;else{_tmp1B4=_tmp1B3->f1;}}_LL14B: {
# 1429
struct _RegionHandle _tmp319=_new_region("temp");struct _RegionHandle*temp=& _tmp319;_push_region(temp);
{struct _tuple22 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1B4,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp31B;struct Cyc_List_List*_tmp31C;struct _tuple22 _tmp31A=_stmttmp27;_tmp31B=_tmp31A.f1;_tmp31C=_tmp31A.f2;{
unsigned int _tmp7C2;unsigned int _tmp7C1;struct _dyneither_ptr _tmp7C0;void**_tmp7BF;unsigned int _tmp7BE;struct _dyneither_ptr aggrdict=
(_tmp7BE=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B4),((_tmp7BF=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7BE)),((_tmp7C0=_tag_dyneither(_tmp7BF,sizeof(void*),_tmp7BE),((((_tmp7C1=_tmp7BE,_tmp7C3(& _tmp7C2,& _tmp7C1,& _tmp7BF,& _tmp31C))),_tmp7C0)))))));
# 1437
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7CD;struct Cyc_CfFlowInfo_UnionRInfo _tmp7CC;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7CB;struct _tuple15 _tmp7CA;struct _tuple15 _tmp321=(_tmp7CA.f1=_tmp31B,((_tmp7CA.f2=(void*)((_tmp7CD=_region_malloc(env->r,sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CB.tag=6,((_tmp7CB.f1=((_tmp7CC.is_union=0,((_tmp7CC.fieldnum=- 1,_tmp7CC)))),((_tmp7CB.f2=aggrdict,_tmp7CB)))))),_tmp7CD)))),_tmp7CA)));_npop_handler(0);return _tmp321;};}
# 1430
;_pop_region(temp);}_LL14C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B5->tag != 29)goto _LL14E;else{_tmp1B6=_tmp1B5->f2;}}_LL14D: {
# 1440
struct Cyc_List_List*fs;
{void*_stmttmp26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp327=_stmttmp26;struct Cyc_List_List*_tmp329;_LL1EB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327;if(_tmp328->tag != 12)goto _LL1ED;else{_tmp329=_tmp328->f2;}}_LL1EC:
 fs=_tmp329;goto _LL1EA;_LL1ED:;_LL1EE: {
struct Cyc_Core_Impossible_exn_struct _tmp7D3;const char*_tmp7D2;struct Cyc_Core_Impossible_exn_struct*_tmp7D1;(int)_throw((void*)((_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D3.tag=Cyc_Core_Impossible,((_tmp7D3.f1=((_tmp7D2="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7D2,sizeof(char),35))),_tmp7D3)))),_tmp7D1)))));}_LL1EA:;}
# 1445
_tmp1B8=_tmp1B6;_tmp1B9=Cyc_Absyn_StructA;_tmp1BA=fs;goto _LL14F;}_LL14E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1B7->tag != 28)goto _LL150;else{_tmp1B8=_tmp1B7->f3;if(_tmp1B7->f4 == 0)goto _LL150;_tmp1B9=(_tmp1B7->f4)->kind;if((_tmp1B7->f4)->impl == 0)goto _LL150;_tmp1BA=((_tmp1B7->f4)->impl)->fields;}}_LL14F: {
# 1447
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp32D=_new_region("temp");struct _RegionHandle*temp=& _tmp32D;_push_region(temp);
{struct _tuple22 _stmttmp24=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1B8),1,1);
# 1449
union Cyc_CfFlowInfo_FlowInfo _tmp32F;struct Cyc_List_List*_tmp330;struct _tuple22 _tmp32E=_stmttmp24;_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;{
# 1451
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp142,_tmp1BA,_tmp1B9 == Cyc_Absyn_UnionA,_tmp142->unknown_all);
# 1454
int field_no=-1;
{int i=0;for(0;_tmp330 != 0;(((_tmp330=_tmp330->tl,_tmp1B8=_tmp1B8->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1B8))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp25=(void*)ds->hd;void*_tmp331=_stmttmp25;struct _dyneither_ptr*_tmp334;_LL1F0: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp332=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp331;if(_tmp332->tag != 0)goto _LL1F2;}_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7D9;const char*_tmp7D8;struct Cyc_Core_Impossible_exn_struct*_tmp7D7;(int)_throw((void*)((_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7[0]=((_tmp7D9.tag=Cyc_Core_Impossible,((_tmp7D9.f1=((_tmp7D8="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7D8,sizeof(char),22))),_tmp7D9)))),_tmp7D7)))));}_LL1F2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp333=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp331;if(_tmp333->tag != 1)goto _LL1EF;else{_tmp334=_tmp333->f1;}}_LL1F3:
# 1461
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1BA,_tmp334);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp330->hd;
# 1464
if(_tmp1B9 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp338=(*((struct _tuple25*)_tmp1B8->hd)).f2;
_tmp32F=Cyc_NewControlFlow_use_Rval(env,_tmp338->loc,_tmp32F,(void*)_tmp330->hd);
# 1468
Cyc_NewControlFlow_check_requires(_tmp338->loc,_tmp142->r,exp_type,_tmp334,_tmp32F);}_LL1EF:;}}}{
# 1471
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7DF;struct Cyc_CfFlowInfo_UnionRInfo _tmp7DE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7DD;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp339=(_tmp7DD=_region_malloc(env->r,sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=6,((_tmp7DF.f1=((_tmp7DE.is_union=_tmp1B9 == Cyc_Absyn_UnionA,((_tmp7DE.fieldnum=field_no,_tmp7DE)))),((_tmp7DF.f2=aggrdict,_tmp7DF)))))),_tmp7DD)));
struct _tuple15 _tmp7E0;struct _tuple15 _tmp33B=(_tmp7E0.f1=_tmp32F,((_tmp7E0.f2=(void*)_tmp339,_tmp7E0)));_npop_handler(0);return _tmp33B;};};}
# 1449
;_pop_region(temp);}_LL150: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BB->tag != 28)goto _LL152;}_LL151: {
# 1475
struct Cyc_Core_Impossible_exn_struct _tmp7E6;const char*_tmp7E5;struct Cyc_Core_Impossible_exn_struct*_tmp7E4;(int)_throw((void*)((_tmp7E4=_cycalloc(sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E6.tag=Cyc_Core_Impossible,((_tmp7E6.f1=((_tmp7E5="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7E5,sizeof(char),31))),_tmp7E6)))),_tmp7E4)))));}_LL152: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BC->tag != 25)goto _LL154;else{_tmp1BD=_tmp1BC->f1;}}_LL153: {
# 1477
struct _RegionHandle _tmp342=_new_region("temp");struct _RegionHandle*temp=& _tmp342;_push_region(temp);
{struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BD);
struct _tuple22 _stmttmp23=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp343,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp345;struct Cyc_List_List*_tmp346;struct _tuple22 _tmp344=_stmttmp23;_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;
for(0;_tmp346 != 0;(_tmp346=_tmp346->tl,_tmp343=_tmp343->tl)){
_tmp345=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp343))->hd)->loc,_tmp345,(void*)_tmp346->hd);}{
struct _tuple15 _tmp7E7;struct _tuple15 _tmp348=(_tmp7E7.f1=_tmp345,((_tmp7E7.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,(void*)_check_null(e->topt),0,_tmp142->unknown_all),_tmp7E7)));_npop_handler(0);return _tmp348;};}
# 1478
;_pop_region(temp);}_LL154: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1BE->tag != 26)goto _LL156;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;_tmp1C1=_tmp1BE->f3;_tmp1C2=_tmp1BE->f4;}}_LL155: {
# 1486
struct _tuple15 _stmttmp20=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C0);union Cyc_CfFlowInfo_FlowInfo _tmp34A;void*_tmp34B;struct _tuple15 _tmp349=_stmttmp20;_tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp34C=_tmp34A;struct Cyc_Dict_Dict _tmp34D;struct Cyc_List_List*_tmp34E;_LL1F5: if((_tmp34C.BottomFL).tag != 1)goto _LL1F7;_LL1F6: {
struct _tuple15 _tmp7E8;return(_tmp7E8.f1=_tmp34A,((_tmp7E8.f2=_tmp142->unknown_all,_tmp7E8)));}_LL1F7: if((_tmp34C.ReachableFL).tag != 2)goto _LL1F4;_tmp34D=((struct _tuple14)(_tmp34C.ReachableFL).val).f1;_tmp34E=((struct _tuple14)(_tmp34C.ReachableFL).val).f2;_LL1F8:
# 1490
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp34D,_tmp34B)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7EB;void*_tmp7EA;(_tmp7EA=0,Cyc_Tcutil_terr(_tmp1C0->loc,((_tmp7EB="expression may not be initialized",_tag_dyneither(_tmp7EB,sizeof(char),34))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}{
# 1494
struct Cyc_List_List*new_relns=_tmp34E;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1BF);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1C0,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp34E);
# 1500
if(_tmp34E != new_relns)
_tmp34A=Cyc_CfFlowInfo_ReachableFL(_tmp34D,new_relns);{
# 1504
void*_tmp352=_tmp34B;_LL1FA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp353=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp352;if(_tmp353->tag != 0)goto _LL1FC;}_LL1FB: {
struct _tuple15 _tmp7EC;return(_tmp7EC.f1=_tmp34A,((_tmp7EC.f2=_tmp142->unknown_all,_tmp7EC)));}_LL1FC: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp354=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp352;if(_tmp354->tag != 2)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp355=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp352;if(_tmp355->tag != 1)goto _LL200;}_LL1FF:
 goto _LL201;_LL200: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp356=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp352;if(_tmp356->tag != 5)goto _LL202;}_LL201: {
# 1509
struct Cyc_List_List _tmp7ED;struct Cyc_List_List _tmp358=(_tmp7ED.hd=_tmp1BF,((_tmp7ED.tl=0,_tmp7ED)));
_tmp34A=Cyc_NewControlFlow_add_vars(_tmp142,_tmp34A,& _tmp358,_tmp142->unknown_all,e->loc,0);{
# 1512
struct _tuple15 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34A,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp35A;void*_tmp35B;struct _tuple15 _tmp359=_stmttmp22;_tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp35C=_tmp35A;struct Cyc_Dict_Dict _tmp35D;_LL205: if((_tmp35C.BottomFL).tag != 1)goto _LL207;_LL206: {
struct _tuple15 _tmp7EE;return(_tmp7EE.f1=_tmp35A,((_tmp7EE.f2=_tmp142->unknown_all,_tmp7EE)));}_LL207: if((_tmp35C.ReachableFL).tag != 2)goto _LL204;_tmp35D=((struct _tuple14)(_tmp35C.ReachableFL).val).f1;_LL208:
# 1516
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp35D,_tmp35B)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_Tcutil_terr(_tmp1C0->loc,((_tmp7F1="expression may not be initialized",_tag_dyneither(_tmp7F1,sizeof(char),34))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}{
struct _tuple15 _tmp7F2;return(_tmp7F2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F2.f2=_tmp142->unknown_all,_tmp7F2)));};}_LL204:;}
# 1521
_tmp34A=_tmp35A;
goto _LL203;};}_LL202:;_LL203:
# 1524
 while(1){
struct Cyc_List_List _tmp7F3;struct Cyc_List_List _tmp363=(_tmp7F3.hd=_tmp1BF,((_tmp7F3.tl=0,_tmp7F3)));
_tmp34A=Cyc_NewControlFlow_add_vars(_tmp142,_tmp34A,& _tmp363,_tmp142->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp21=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34A,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp365;void*_tmp366;struct _tuple15 _tmp364=_stmttmp21;_tmp365=_tmp364.f1;_tmp366=_tmp364.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp367=_tmp365;struct Cyc_Dict_Dict _tmp368;_LL20A: if((_tmp367.BottomFL).tag != 1)goto _LL20C;_LL20B:
 goto _LL209;_LL20C: if((_tmp367.ReachableFL).tag != 2)goto _LL209;_tmp368=((struct _tuple14)(_tmp367.ReachableFL).val).f1;_LL20D:
# 1531
 if(Cyc_CfFlowInfo_initlevel(_tmp142,_tmp368,_tmp366)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,Cyc_Tcutil_terr(_tmp1C0->loc,((_tmp7F6="expression may not be initialized",_tag_dyneither(_tmp7F6,sizeof(char),34))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}{
struct _tuple15 _tmp7F7;return(_tmp7F7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F7.f2=_tmp142->unknown_all,_tmp7F7)));};}_LL209:;}{
# 1536
union Cyc_CfFlowInfo_FlowInfo _tmp36C=Cyc_CfFlowInfo_join_flow(_tmp142,env->all_changed,_tmp34A,_tmp365);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp36C,_tmp34A))
break;
_tmp34A=_tmp36C;};};}{
# 1541
struct _tuple15 _tmp7F8;return(_tmp7F8.f1=_tmp34A,((_tmp7F8.f2=_tmp142->unknown_all,_tmp7F8)));};_LL1F9:;};};_LL1F4:;};}_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1C3=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C3->tag != 27)goto _LL158;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=(void*)_tmp1C3->f2;_tmp1C6=_tmp1C3->f3;}}_LL157: {
# 1547
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7FB;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7FA;void*root=(void*)((_tmp7FA=_region_malloc(_tmp142->r,sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FB.tag=1,((_tmp7FB.f1=_tmp1C4,((_tmp7FB.f2=(void*)_check_null(e->topt),_tmp7FB)))))),_tmp7FA))));
struct Cyc_CfFlowInfo_Place*_tmp7FC;struct Cyc_CfFlowInfo_Place*place=(_tmp7FC=_region_malloc(_tmp142->r,sizeof(*_tmp7FC)),((_tmp7FC->root=root,((_tmp7FC->fields=0,_tmp7FC)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7FF;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7FE;void*rval=(void*)((_tmp7FE=_region_malloc(_tmp142->r,sizeof(*_tmp7FE)),((_tmp7FE[0]=((_tmp7FF.tag=5,((_tmp7FF.f1=place,_tmp7FF)))),_tmp7FE))));
void*place_val;
# 1555
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp142,_tmp1C5,0,_tmp142->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C4);union Cyc_CfFlowInfo_FlowInfo _tmp370;struct _tuple15 _tmp36F=_stmttmp1F;_tmp370=_tmp36F.f1;
outflow=_tmp370;{
union Cyc_CfFlowInfo_FlowInfo _tmp371=outflow;struct Cyc_Dict_Dict _tmp372;struct Cyc_List_List*_tmp373;_LL20F: if((_tmp371.BottomFL).tag != 1)goto _LL211;_LL210: {
struct _tuple15 _tmp800;return(_tmp800.f1=outflow,((_tmp800.f2=rval,_tmp800)));}_LL211: if((_tmp371.ReachableFL).tag != 2)goto _LL20E;_tmp372=((struct _tuple14)(_tmp371.ReachableFL).val).f1;_tmp373=((struct _tuple14)(_tmp371.ReachableFL).val).f2;_LL212: {
# 1563
struct _tuple15 _tmp801;return(_tmp801.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp372,root,place_val),_tmp373),((_tmp801.f2=rval,_tmp801)));}_LL20E:;};};};}_LL158: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C7->tag != 36)goto _LL15A;else{_tmp1C8=_tmp1C7->f1;}}_LL159: {
# 1567
struct _tuple16 _tmp802;struct _tuple16 _tmp37B=(_tmp802.f1=(env->fenv)->unknown_all,((_tmp802.f2=copy_ctxt,_tmp802)));
union Cyc_CfFlowInfo_FlowInfo _tmp37C=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1C8,& _tmp37B);
struct _tuple15 _tmp803;return(_tmp803.f1=_tmp37C,((_tmp803.f2=_tmp37B.f1,_tmp803)));}_LL15A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1C9->tag != 1)goto _LL15C;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1CA=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1C9->f2);if(_tmp1CA->tag != 0)goto _LL15C;}}_LL15B:
# 1571
 goto _LL15D;_LL15C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CB->tag != 35)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CC->tag != 24)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CD->tag != 38)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp157;if(_tmp1CE->tag != 39)goto _LLFB;}_LL163: {
# 1576
struct Cyc_Core_Impossible_exn_struct _tmp809;const char*_tmp808;struct Cyc_Core_Impossible_exn_struct*_tmp807;(int)_throw((void*)((_tmp807=_cycalloc(sizeof(*_tmp807)),((_tmp807[0]=((_tmp809.tag=Cyc_Core_Impossible,((_tmp809.f1=((_tmp808="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp808,sizeof(char),31))),_tmp809)))),_tmp807)))));}_LLFB:;};}
# 1586
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1594
struct Cyc_CfFlowInfo_FlowEnv*_tmp382=env->fenv;
void*_stmttmp51=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp383=_stmttmp51;void*_tmp385;union Cyc_Absyn_Constraint*_tmp386;union Cyc_Absyn_Constraint*_tmp387;_LL214: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 5)goto _LL216;else{_tmp385=(_tmp384->f1).elt_typ;_tmp386=((_tmp384->f1).ptr_atts).bounds;_tmp387=((_tmp384->f1).ptr_atts).zero_term;}}_LL215: {
# 1597
union Cyc_CfFlowInfo_FlowInfo _tmp388=f;struct Cyc_Dict_Dict _tmp389;struct Cyc_List_List*_tmp38A;_LL219: if((_tmp388.BottomFL).tag != 1)goto _LL21B;_LL21A: {
struct _tuple17 _tmp80A;return(_tmp80A.f1=f,((_tmp80A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp80A)));}_LL21B: if((_tmp388.ReachableFL).tag != 2)goto _LL218;_tmp389=((struct _tuple14)(_tmp388.ReachableFL).val).f1;_tmp38A=((struct _tuple14)(_tmp388.ReachableFL).val).f2;_LL21C: {
# 1601
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp386)){
retry: {void*_tmp38C=r;struct Cyc_Absyn_Vardecl*_tmp38E;void*_tmp38F;void*_tmp393;struct Cyc_List_List*_tmp394;enum Cyc_CfFlowInfo_InitLevel _tmp397;_LL21E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp38D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp38D->tag != 8)goto _LL220;else{_tmp38E=_tmp38D->f1;_tmp38F=(void*)_tmp38D->f2;}}_LL21F:
 r=_tmp38F;locname=_tmp38E;goto retry;_LL220: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp390=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp390->tag != 1)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp391=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp391->tag != 2)goto _LL224;}_LL223:
# 1607
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp80D;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80C;e->annot=(void*)((_tmp80C=_cycalloc(sizeof(*_tmp80C)),((_tmp80C[0]=((_tmp80D.tag=Cyc_CfFlowInfo_NotZero,((_tmp80D.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38A),_tmp80D)))),_tmp80C))));}goto _LL21D;_LL224: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp392=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp392->tag != 5)goto _LL226;else{_tmp393=(_tmp392->f1)->root;_tmp394=(_tmp392->f1)->fields;}}_LL225:
# 1609
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp810;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80F;e->annot=(void*)((_tmp80F=_cycalloc(sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=Cyc_CfFlowInfo_NotZero,((_tmp810.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38A),_tmp810)))),_tmp80F))));}{
struct Cyc_CfFlowInfo_Place*_tmp813;struct _tuple17 _tmp812;return(_tmp812.f1=f,((_tmp812.f2=Cyc_CfFlowInfo_PlaceL(((_tmp813=_region_malloc(_tmp382->r,sizeof(*_tmp813)),((_tmp813->root=_tmp393,((_tmp813->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp382->r,_tmp394,flds),_tmp813))))))),_tmp812)));};_LL226: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp395=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp395->tag != 0)goto _LL228;}_LL227:
# 1612
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp814;return(_tmp814.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp814.f2=Cyc_CfFlowInfo_UnknownL(),_tmp814)));};_LL228: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp396=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38C;if(_tmp396->tag != 3)goto _LL22A;else{_tmp397=_tmp396->f1;}}_LL229:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp397,locname);goto _LL22B;_LL22A:;_LL22B:
# 1617
 if(passthrough_consumes){
void*_tmp39F=r;void*_tmp3A1;_LL22D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3A0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A0->tag != 7)goto _LL22F;else{_tmp3A1=(void*)_tmp3A0->f3;}}_LL22E:
 r=_tmp3A1;goto retry;_LL22F:;_LL230:
 goto _LL22C;_LL22C:;}{
# 1623
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp817;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp816;e->annot=(void*)((_tmp816=_cycalloc(sizeof(*_tmp816)),((_tmp816[0]=((_tmp817.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp817.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38A),_tmp817)))),_tmp816))));};_LL21D:;}}else{
# 1626
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81A;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp819;e->annot=(void*)((_tmp819=_cycalloc(sizeof(*_tmp819)),((_tmp819[0]=((_tmp81A.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81A.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38A),_tmp81A)))),_tmp819))));}
if(Cyc_CfFlowInfo_initlevel(_tmp382,_tmp389,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(e->loc,((_tmp81D="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp81D,sizeof(char),46))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}{
struct _tuple17 _tmp81E;return(_tmp81E.f1=f,((_tmp81E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp81E)));};}_LL218:;}_LL216:;_LL217: {
# 1631
struct Cyc_Core_Impossible_exn_struct _tmp824;const char*_tmp823;struct Cyc_Core_Impossible_exn_struct*_tmp822;(int)_throw((void*)((_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=((_tmp824.tag=Cyc_Core_Impossible,((_tmp824.f1=((_tmp823="left deref of non-pointer-type",_tag_dyneither(_tmp823,sizeof(char),31))),_tmp824)))),_tmp822)))));}_LL213:;}
# 1639
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1642
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3AC=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3AD=inflow;struct Cyc_Dict_Dict _tmp3AE;struct Cyc_List_List*_tmp3AF;_LL232: if((_tmp3AD.BottomFL).tag != 1)goto _LL234;_LL233: {
struct _tuple17 _tmp825;return(_tmp825.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp825.f2=Cyc_CfFlowInfo_UnknownL(),_tmp825)));}_LL234: if((_tmp3AD.ReachableFL).tag != 2)goto _LL231;_tmp3AE=((struct _tuple14)(_tmp3AD.ReachableFL).val).f1;_tmp3AF=((struct _tuple14)(_tmp3AD.ReachableFL).val).f2;_LL235:
# 1647
 d=_tmp3AE;_LL231:;}{
# 1651
void*_stmttmp52=e->r;void*_tmp3B1=_stmttmp52;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3BA;struct Cyc_Absyn_Vardecl*_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;struct _dyneither_ptr*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct _dyneither_ptr*_tmp3CE;_LL237: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3B2->tag != 13)goto _LL239;else{_tmp3B3=_tmp3B2->f2;}}_LL238:
 _tmp3B5=_tmp3B3;goto _LL23A;_LL239: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3B4->tag != 11)goto _LL23B;else{_tmp3B5=_tmp3B4->f1;}}_LL23A:
 _tmp3B7=_tmp3B5;goto _LL23C;_LL23B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3B6->tag != 12)goto _LL23D;else{_tmp3B7=_tmp3B6->f1;}}_LL23C:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3B7,flds);_LL23D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3B8->tag != 1)goto _LL23F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3B9=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3B8->f2);if(_tmp3B9->tag != 3)goto _LL23F;else{_tmp3BA=_tmp3B9->f1;}}}_LL23E:
# 1656
 _tmp3BD=_tmp3BA;goto _LL240;_LL23F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3BB->tag != 1)goto _LL241;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3BC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3BB->f2);if(_tmp3BC->tag != 4)goto _LL241;else{_tmp3BD=_tmp3BC->f1;}}}_LL240:
 _tmp3C0=_tmp3BD;goto _LL242;_LL241: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3BE->tag != 1)goto _LL243;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3BF=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3BE->f2);if(_tmp3BF->tag != 5)goto _LL243;else{_tmp3C0=_tmp3BF->f1;}}}_LL242: {
# 1659
struct Cyc_CfFlowInfo_Place*_tmp82F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp82E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp82D;struct _tuple17 _tmp82C;return(_tmp82C.f1=inflow,((_tmp82C.f2=Cyc_CfFlowInfo_PlaceL(((_tmp82F=_region_malloc(env->r,sizeof(*_tmp82F)),((_tmp82F->root=(void*)((_tmp82D=_region_malloc(env->r,sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82E.tag=0,((_tmp82E.f1=_tmp3C0,_tmp82E)))),_tmp82D)))),((_tmp82F->fields=flds,_tmp82F))))))),_tmp82C)));}_LL243: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3C1->tag != 1)goto _LL245;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3C2=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3C1->f2);if(_tmp3C2->tag != 1)goto _LL245;else{_tmp3C3=_tmp3C2->f1;}}}_LL244: {
# 1661
struct _tuple17 _tmp830;return(_tmp830.f1=inflow,((_tmp830.f2=Cyc_CfFlowInfo_UnknownL(),_tmp830)));}_LL245: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3C4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3C4->tag != 21)goto _LL247;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL246:
# 1664
{void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3C5->topt));void*_tmp3D4=_stmttmp59;void*_tmp3D6;_LL250: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D4;if(_tmp3D5->tag != 5)goto _LL252;else{_tmp3D6=(_tmp3D5->f1).elt_typ;}}_LL251:
# 1666
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D6)){
Cyc_NewControlFlow_check_requires(_tmp3C5->loc,_tmp3AC->r,_tmp3D6,_tmp3C6,inflow);{
struct Cyc_List_List*_tmp831;flds=((_tmp831=_region_malloc(env->r,sizeof(*_tmp831)),((_tmp831->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3D6,_tmp3C6),((_tmp831->tl=flds,_tmp831))))));};}
# 1670
goto _LL24F;_LL252:;_LL253: {
struct Cyc_Core_Impossible_exn_struct _tmp837;const char*_tmp836;struct Cyc_Core_Impossible_exn_struct*_tmp835;(int)_throw((void*)((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835[0]=((_tmp837.tag=Cyc_Core_Impossible,((_tmp837.f1=((_tmp836="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp836,sizeof(char),25))),_tmp837)))),_tmp835)))));}_LL24F:;}
# 1673
_tmp3C8=_tmp3C5;goto _LL248;_LL247: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3C7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3C7->tag != 19)goto _LL249;else{_tmp3C8=_tmp3C7->f1;}}_LL248:
# 1676
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C8->topt),1)){
# 1678
struct _tuple17 _stmttmp56=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C8->topt),1),passthrough_consumes,_tmp3C8);
# 1678
union Cyc_CfFlowInfo_FlowInfo _tmp3DC;union Cyc_CfFlowInfo_AbsLVal _tmp3DD;struct _tuple17 _tmp3DB=_stmttmp56;_tmp3DC=_tmp3DB.f1;_tmp3DD=_tmp3DB.f2;{
# 1682
struct _tuple17 _tmp838;struct _tuple17 _stmttmp57=(_tmp838.f1=_tmp3DC,((_tmp838.f2=_tmp3DD,_tmp838)));struct _tuple17 _tmp3DE=_stmttmp57;struct Cyc_Dict_Dict _tmp3DF;struct Cyc_List_List*_tmp3E0;struct Cyc_CfFlowInfo_Place*_tmp3E1;_LL255: if(((_tmp3DE.f1).ReachableFL).tag != 2)goto _LL257;_tmp3DF=((struct _tuple14)((_tmp3DE.f1).ReachableFL).val).f1;_tmp3E0=((struct _tuple14)((_tmp3DE.f1).ReachableFL).val).f2;if(((_tmp3DE.f2).PlaceL).tag != 1)goto _LL257;_tmp3E1=(struct Cyc_CfFlowInfo_Place*)((_tmp3DE.f2).PlaceL).val;_LL256: {
# 1684
void*_tmp3E2=Cyc_CfFlowInfo_lookup_place(_tmp3DF,_tmp3E1);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp3E3=_tmp3E2;struct Cyc_Absyn_Vardecl*_tmp3E5;void*_tmp3E6;_LL25A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E3;if(_tmp3E4->tag != 8)goto _LL25C;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=(void*)_tmp3E4->f2;}}_LL25B:
 _tmp3E2=_tmp3E6;locname=_tmp3E5;goto retry;_LL25C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3E7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E3;if(_tmp3E7->tag != 7)goto _LL25E;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3E8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp3E7->f3);if(_tmp3E8->tag != 5)goto _LL25E;}}_LL25D:
 goto _LL25F;_LL25E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3E9=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3E3;if(_tmp3E9->tag != 5)goto _LL260;}_LL25F:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3DC,_tmp3C8,_tmp3DC,_tmp3E2,passthrough_consumes,flds);_LL260:;_LL261: {
# 1696
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3AC,_tmp3DF,_tmp3E2);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3AC->unknown_all: _tmp3AC->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3AC,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3C8->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp83B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp83A;void*new_root=(void*)((_tmp83A=_region_malloc(_tmp3AC->r,sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=1,((_tmp83B.f1=e,((_tmp83B.f2=(void*)_check_null(e->topt),_tmp83B)))))),_tmp83A))));
struct Cyc_CfFlowInfo_Place*_tmp83C;struct Cyc_CfFlowInfo_Place*place=(_tmp83C=_region_malloc(_tmp3AC->r,sizeof(*_tmp83C)),((_tmp83C->root=new_root,((_tmp83C->fields=0,_tmp83C)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp83F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp83E;void*res=(void*)((_tmp83E=_region_malloc(_tmp3AC->r,sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp83F.tag=5,((_tmp83F.f1=place,_tmp83F)))),_tmp83E))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp842;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp841;res=(void*)((_tmp841=_region_malloc(_tmp3AC->r,sizeof(*_tmp841)),((_tmp841[0]=((_tmp842.tag=8,((_tmp842.f1=locname,((_tmp842.f2=res,_tmp842)))))),_tmp841))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3DF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3DF,new_root,new_rval);
_tmp3DF=Cyc_CfFlowInfo_assign_place(_tmp3AC,e->loc,_tmp3DF,env->all_changed,_tmp3E1,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3EC=Cyc_CfFlowInfo_ReachableFL(_tmp3DF,_tmp3E0);
# 1711
return Cyc_NewControlFlow_anal_derefL(env,_tmp3EC,_tmp3C8,_tmp3EC,res,passthrough_consumes,flds);};}_LL259:;}}_LL257:;_LL258:
# 1713
 goto _LL254;_LL254:;};}{
# 1716
struct _tuple15 _stmttmp58=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3C8);union Cyc_CfFlowInfo_FlowInfo _tmp3F4;void*_tmp3F5;struct _tuple15 _tmp3F3=_stmttmp58;_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C8,_tmp3F4,_tmp3F5,passthrough_consumes,flds);};_LL249: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3C9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3C9->tag != 22)goto _LL24B;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;}}_LL24A: {
# 1720
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(_tmp3CA->topt));void*_tmp3F6=_stmttmp53;union Cyc_Absyn_Constraint*_tmp3F9;_LL263: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F6;if(_tmp3F7->tag != 10)goto _LL265;}_LL264: {
# 1722
unsigned int _tmp3FA=(Cyc_Evexp_eval_const_uint_exp(_tmp3CB)).f1;
struct Cyc_List_List*_tmp843;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CA,((_tmp843=_region_malloc(env->r,sizeof(*_tmp843)),((_tmp843->hd=(void*)((int)_tmp3FA),((_tmp843->tl=flds,_tmp843)))))));}_LL265: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6;if(_tmp3F8->tag != 5)goto _LL267;else{_tmp3F9=((_tmp3F8->f1).ptr_atts).bounds;}}_LL266: {
# 1725
struct _RegionHandle _tmp3FC=_new_region("temp");struct _RegionHandle*temp=& _tmp3FC;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp844[2];struct _tuple22 _stmttmp54=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp844[1]=_tmp3CB,((_tmp844[0]=_tmp3CA,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp844,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1726
union Cyc_CfFlowInfo_FlowInfo _tmp3FE;struct Cyc_List_List*_tmp3FF;struct _tuple22 _tmp3FD=_stmttmp54;_tmp3FE=_tmp3FD.f1;_tmp3FF=_tmp3FD.f2;{
# 1728
union Cyc_CfFlowInfo_FlowInfo _tmp400=_tmp3FE;
{union Cyc_CfFlowInfo_FlowInfo _tmp401=_tmp3FE;struct Cyc_Dict_Dict _tmp402;struct Cyc_List_List*_tmp403;_LL26A: if((_tmp401.ReachableFL).tag != 2)goto _LL26C;_tmp402=((struct _tuple14)(_tmp401.ReachableFL).val).f1;_tmp403=((struct _tuple14)(_tmp401.ReachableFL).val).f2;_LL26B:
# 1731
 if(Cyc_CfFlowInfo_initlevel(_tmp3AC,_tmp402,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3FF))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp847;void*_tmp846;(_tmp846=0,Cyc_Tcutil_terr(_tmp3CB->loc,((_tmp847="expression may not be initialized",_tag_dyneither(_tmp847,sizeof(char),34))),_tag_dyneither(_tmp846,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp406=Cyc_NewControlFlow_add_subscript_reln(_tmp3AC->r,_tmp403,_tmp3CA,_tmp3CB);
if(_tmp403 != _tmp406)
_tmp400=Cyc_CfFlowInfo_ReachableFL(_tmp402,_tmp406);
goto _LL269;};_LL26C:;_LL26D:
 goto _LL269;_LL269:;}{
# 1739
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CA,_tmp3FE,(void*)((struct Cyc_List_List*)_check_null(_tmp3FF))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp408;union Cyc_CfFlowInfo_AbsLVal _tmp409;struct _tuple17 _tmp407=_stmttmp55;_tmp408=_tmp407.f1;_tmp409=_tmp407.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp40A=_tmp408;_LL26F: if((_tmp40A.BottomFL).tag != 1)goto _LL271;_LL270: {
struct _tuple17 _tmp848;struct _tuple17 _tmp40C=(_tmp848.f1=_tmp408,((_tmp848.f2=_tmp409,_tmp848)));_npop_handler(0);return _tmp40C;}_LL271:;_LL272: {
struct _tuple17 _tmp849;struct _tuple17 _tmp40E=(_tmp849.f1=_tmp400,((_tmp849.f2=_tmp409,_tmp849)));_npop_handler(0);return _tmp40E;}_LL26E:;};};};}
# 1726
;_pop_region(temp);}_LL267:;_LL268: {
# 1745
struct Cyc_Core_Impossible_exn_struct _tmp84F;const char*_tmp84E;struct Cyc_Core_Impossible_exn_struct*_tmp84D;(int)_throw((void*)((_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D[0]=((_tmp84F.tag=Cyc_Core_Impossible,((_tmp84F.f1=((_tmp84E="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp84E,sizeof(char),33))),_tmp84F)))),_tmp84D)))));}_LL262:;}_LL24B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3CC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3B1;if(_tmp3CC->tag != 20)goto _LL24D;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL24C: {
# 1749
void*_tmp413=(void*)_check_null(_tmp3CD->topt);
if(Cyc_Absyn_is_require_union_type(_tmp413))
Cyc_NewControlFlow_check_requires(_tmp3CD->loc,_tmp3AC->r,_tmp413,_tmp3CE,inflow);
# 1753
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp413)){
struct _tuple17 _tmp850;return(_tmp850.f1=inflow,((_tmp850.f2=Cyc_CfFlowInfo_UnknownL(),_tmp850)));}{
# 1756
struct Cyc_List_List*_tmp851;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CD,(
(_tmp851=_region_malloc(env->r,sizeof(*_tmp851)),((_tmp851->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp413,_tmp3CE),((_tmp851->tl=flds,_tmp851)))))));};}_LL24D:;_LL24E: {
# 1759
struct _tuple17 _tmp852;return(_tmp852.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp852.f2=Cyc_CfFlowInfo_UnknownL(),_tmp852)));}_LL236:;};}
# 1763
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1766
struct _tuple17 _stmttmp5A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp418;union Cyc_CfFlowInfo_AbsLVal _tmp419;struct _tuple17 _tmp417=_stmttmp5A;_tmp418=_tmp417.f1;_tmp419=_tmp417.f2;{
struct _tuple17 _tmp853;return(_tmp853.f1=_tmp418,((_tmp853.f2=_tmp419,_tmp853)));};}
# 1773
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1775
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _stmttmp5B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp41C;struct _tuple17 _tmp41B=_stmttmp5B;_tmp41C=_tmp41B.f1;
inflow=_tmp41C;}}}
# 1782
return inflow;}
# 1788
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1791
struct Cyc_CfFlowInfo_FlowEnv*_tmp41D=env->fenv;
# 1794
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp41E=_new_region("temp");struct _RegionHandle*temp=& _tmp41E;_push_region(temp);{
struct _tuple22 _stmttmp5C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp420;struct Cyc_List_List*_tmp421;struct _tuple22 _tmp41F=_stmttmp5C;_tmp420=_tmp41F.f1;_tmp421=_tmp41F.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp421))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp421->tl))->hd;
f=_tmp420;}{
# 1804
union Cyc_CfFlowInfo_FlowInfo _tmp422=f;struct Cyc_Dict_Dict _tmp423;struct Cyc_List_List*_tmp424;_LL274: if((_tmp422.BottomFL).tag != 1)goto _LL276;_LL275: {
struct _tuple18 _tmp854;struct _tuple18 _tmp426=(_tmp854.f1=f,((_tmp854.f2=f,_tmp854)));_npop_handler(0);return _tmp426;}_LL276: if((_tmp422.ReachableFL).tag != 2)goto _LL273;_tmp423=((struct _tuple14)(_tmp422.ReachableFL).val).f1;_tmp424=((struct _tuple14)(_tmp422.ReachableFL).val).f2;_LL277: {
# 1807
struct Cyc_Absyn_Exp*_tmp427=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp428=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp423,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp857;void*_tmp856;(_tmp856=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp857="expression may not be initialized",_tag_dyneither(_tmp857,sizeof(char),34))),_tag_dyneither(_tmp856,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp423,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp85A="expression may not be initialized",_tag_dyneither(_tmp85A,sizeof(char),34))),_tag_dyneither(_tmp859,sizeof(void*),0)));}{
# 1814
union Cyc_CfFlowInfo_FlowInfo _tmp42D=f;
union Cyc_CfFlowInfo_FlowInfo _tmp42E=f;
# 1819
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp42F=r1;struct Cyc_Absyn_Vardecl*_tmp431;void*_tmp432;_LL279: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp430=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42F;if(_tmp430->tag != 8)goto _LL27B;else{_tmp431=_tmp430->f1;_tmp432=(void*)_tmp430->f2;}}_LL27A:
 r1=_tmp432;r1n=_tmp431;goto _LL278;_LL27B:;_LL27C:
 goto _LL278;_LL278:;}
# 1825
{void*_tmp433=r2;struct Cyc_Absyn_Vardecl*_tmp435;void*_tmp436;_LL27E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp434=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp433;if(_tmp434->tag != 8)goto _LL280;else{_tmp435=_tmp434->f1;_tmp436=(void*)_tmp434->f2;}}_LL27F:
 r2=_tmp436;r2n=_tmp435;goto _LL27D;_LL280:;_LL281:
 goto _LL27D;_LL27D:;}{
# 1830
struct _tuple0 _tmp85B;struct _tuple0 _stmttmp5D=(_tmp85B.f1=r1,((_tmp85B.f2=r2,_tmp85B)));struct _tuple0 _tmp437=_stmttmp5D;enum Cyc_CfFlowInfo_InitLevel _tmp439;enum Cyc_CfFlowInfo_InitLevel _tmp43D;_LL283:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp438=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp438->tag != 3)goto _LL285;else{_tmp439=_tmp438->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp43A->tag != 0)goto _LL285;};_LL284: {
# 1834
struct _tuple18 _stmttmp5F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp427,_tmp439,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp44D;union Cyc_CfFlowInfo_FlowInfo _tmp44E;struct _tuple18 _tmp44C=_stmttmp5F;_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;
switch(p){case Cyc_Absyn_Eq: _LL297:
 _tmp42D=_tmp44E;_tmp42E=_tmp44D;break;case Cyc_Absyn_Neq: _LL298:
 _tmp42D=_tmp44D;_tmp42E=_tmp44E;break;default: _LL299: {
struct Cyc_Core_Impossible_exn_struct _tmp861;const char*_tmp860;struct Cyc_Core_Impossible_exn_struct*_tmp85F;(int)_throw((void*)((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp861.tag=Cyc_Core_Impossible,((_tmp861.f1=((_tmp860="anal_test, zero-split",_tag_dyneither(_tmp860,sizeof(char),22))),_tmp861)))),_tmp85F)))));}}
# 1840
goto _LL282;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp43B->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp43C->tag != 3)goto _LL287;else{_tmp43D=_tmp43C->f1;}};_LL286: {
# 1842
struct _tuple18 _stmttmp5E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp428,_tmp43D,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp453;union Cyc_CfFlowInfo_FlowInfo _tmp454;struct _tuple18 _tmp452=_stmttmp5E;_tmp453=_tmp452.f1;_tmp454=_tmp452.f2;
switch(p){case Cyc_Absyn_Eq: _LL29B:
 _tmp42D=_tmp454;_tmp42E=_tmp453;break;case Cyc_Absyn_Neq: _LL29C:
 _tmp42D=_tmp453;_tmp42E=_tmp454;break;default: _LL29D: {
struct Cyc_Core_Impossible_exn_struct _tmp867;const char*_tmp866;struct Cyc_Core_Impossible_exn_struct*_tmp865;(int)_throw((void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp867.tag=Cyc_Core_Impossible,((_tmp867.f1=((_tmp866="anal_test, zero-split",_tag_dyneither(_tmp866,sizeof(char),22))),_tmp867)))),_tmp865)))));}}
# 1848
goto _LL282;}_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp43E->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp43F->tag != 0)goto _LL289;};_LL288:
# 1850
 if(p == Cyc_Absyn_Eq)_tmp42E=Cyc_CfFlowInfo_BottomFL();else{
_tmp42D=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp440->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp441=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp441->tag != 1)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp442=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp442->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp443=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp443->tag != 2)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp444=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp444->tag != 0)goto _LL28F;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp445->tag != 5)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp446=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp446->tag != 1)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp447->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp448->tag != 2)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp449->tag != 0)goto _LL293;};_LL292:
 goto _LL294;_LL293:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp437.f1;if(_tmp44A->tag != 5)goto _LL295;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp437.f2;if(_tmp44B->tag != 0)goto _LL295;};_LL294:
# 1859
 if(p == Cyc_Absyn_Neq)_tmp42E=Cyc_CfFlowInfo_BottomFL();else{
_tmp42D=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL295:;_LL296:
 goto _LL282;_LL282:;};}
# 1870
{struct _tuple0 _tmp868;struct _tuple0 _stmttmp60=(_tmp868.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp427->topt)),((_tmp868.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp428->topt)),_tmp868)));struct _tuple0 _tmp459=_stmttmp60;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp459.f1;if(_tmp45A->tag != 6)goto _LL2A2;else{if(_tmp45A->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp459.f2;if(_tmp45B->tag != 6)goto _LL2A4;else{if(_tmp45B->f1 != Cyc_Absyn_Unsigned)goto _LL2A4;}}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp459.f1;if(_tmp45C->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL2A7;_LL2A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp459.f2;if(_tmp45D->tag != 19)goto _LL2A8;}_LL2A7:
 goto _LL29F;_LL2A8:;_LL2A9: {
struct _tuple18 _tmp869;struct _tuple18 _tmp45F=(_tmp869.f1=_tmp42D,((_tmp869.f2=_tmp42E,_tmp869)));_npop_handler(0);return _tmp45F;}_LL29F:;}
# 1879
switch(p){case Cyc_Absyn_Eq: _LL2AA:
 goto _LL2AB;case Cyc_Absyn_Neq: _LL2AB: goto _LL2AC;case Cyc_Absyn_Gt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Gte: _LL2AD: goto _LL2AE;case Cyc_Absyn_Lt: _LL2AE: goto _LL2AF;case Cyc_Absyn_Lte: _LL2AF: break;default: _LL2B0: {
struct _tuple18 _tmp86A;struct _tuple18 _tmp462=(_tmp86A.f1=_tmp42D,((_tmp86A.f2=_tmp42E,_tmp86A)));_npop_handler(0);return _tmp462;}}{
# 1884
struct _RegionHandle*_tmp463=(env->fenv)->r;
struct _tuple13 _stmttmp61=Cyc_Relations_primop2relation(_tmp427,p,_tmp428);struct Cyc_Absyn_Exp*_tmp465;enum Cyc_Relations_Relation _tmp466;struct Cyc_Absyn_Exp*_tmp467;struct _tuple13 _tmp464=_stmttmp61;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;_tmp467=_tmp464.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1889
if(Cyc_Relations_exp2relnop(_tmp465,& n1) && Cyc_Relations_exp2relnop(_tmp467,& n2)){
# 1891
struct Cyc_List_List*_tmp468=Cyc_Relations_add_relation(_tmp463,n1,_tmp466,n2,_tmp424);
# 1895
struct Cyc_List_List*_tmp469=Cyc_Relations_add_relation(_tmp463,n2,Cyc_Relations_flip_relation(_tmp466),n1,_tmp424);
struct _tuple18 _tmp86B;struct _tuple18 _stmttmp62=(_tmp86B.f1=_tmp42D,((_tmp86B.f2=_tmp42E,_tmp86B)));struct _tuple18 _tmp46A=_stmttmp62;struct Cyc_Dict_Dict _tmp46B;struct Cyc_Dict_Dict _tmp46C;struct Cyc_Dict_Dict _tmp46D;struct Cyc_Dict_Dict _tmp46E;_LL2B3: if(((_tmp46A.f1).ReachableFL).tag != 2)goto _LL2B5;_tmp46B=((struct _tuple14)((_tmp46A.f1).ReachableFL).val).f1;if(((_tmp46A.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp46C=((struct _tuple14)((_tmp46A.f2).ReachableFL).val).f1;_LL2B4: {
# 1898
struct _tuple18 _tmp86C;struct _tuple18 _tmp470=(_tmp86C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46B,_tmp468),((_tmp86C.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp469),_tmp86C)));_npop_handler(0);return _tmp470;}_LL2B5: if(((_tmp46A.f2).ReachableFL).tag != 2)goto _LL2B7;_tmp46D=((struct _tuple14)((_tmp46A.f2).ReachableFL).val).f1;_LL2B6: {
# 1900
struct _tuple18 _tmp86D;struct _tuple18 _tmp472=(_tmp86D.f1=_tmp42D,((_tmp86D.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46D,_tmp469),_tmp86D)));_npop_handler(0);return _tmp472;}_LL2B7: if(((_tmp46A.f1).ReachableFL).tag != 2)goto _LL2B9;_tmp46E=((struct _tuple14)((_tmp46A.f1).ReachableFL).val).f1;_LL2B8: {
# 1902
struct _tuple18 _tmp86E;struct _tuple18 _tmp474=(_tmp86E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46E,_tmp468),((_tmp86E.f2=_tmp42E,_tmp86E)));_npop_handler(0);return _tmp474;}_LL2B9:;_LL2BA: {
# 1904
struct _tuple18 _tmp86F;struct _tuple18 _tmp476=(_tmp86F.f1=_tmp42D,((_tmp86F.f2=_tmp42E,_tmp86F)));_npop_handler(0);return _tmp476;}_LL2B2:;}else{
# 1907
struct _tuple18 _tmp870;struct _tuple18 _tmp479=(_tmp870.f1=_tmp42D,((_tmp870.f2=_tmp42E,_tmp870)));_npop_handler(0);return _tmp479;}};};};}_LL273:;};
# 1796
;_pop_region(temp);}
# 1913
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1915
struct Cyc_CfFlowInfo_FlowEnv*_tmp47A=env->fenv;
void*_stmttmp63=e->r;void*_tmp47B=_stmttmp63;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp48A;enum Cyc_Absyn_Primop _tmp48C;struct Cyc_List_List*_tmp48D;_LL2BC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp47C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp47C->tag != 5)goto _LL2BE;else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;_tmp47F=_tmp47C->f3;}}_LL2BD: {
# 1918
struct _tuple18 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47D);union Cyc_CfFlowInfo_FlowInfo _tmp48F;union Cyc_CfFlowInfo_FlowInfo _tmp490;struct _tuple18 _tmp48E=_stmttmp6B;_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;{
struct _tuple18 _stmttmp6C=Cyc_NewControlFlow_anal_test(env,_tmp48F,_tmp47E);union Cyc_CfFlowInfo_FlowInfo _tmp492;union Cyc_CfFlowInfo_FlowInfo _tmp493;struct _tuple18 _tmp491=_stmttmp6C;_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{
struct _tuple18 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,_tmp490,_tmp47F);union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple18 _tmp494=_stmttmp6D;_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;{
struct _tuple18 _tmp871;return(_tmp871.f1=Cyc_CfFlowInfo_join_flow(_tmp47A,env->all_changed,_tmp492,_tmp495),((_tmp871.f2=
Cyc_CfFlowInfo_join_flow(_tmp47A,env->all_changed,_tmp493,_tmp496),_tmp871)));};};};}_LL2BE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp480=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp480->tag != 6)goto _LL2C0;else{_tmp481=_tmp480->f1;_tmp482=_tmp480->f2;}}_LL2BF: {
# 1924
struct _tuple18 _stmttmp69=Cyc_NewControlFlow_anal_test(env,inflow,_tmp481);union Cyc_CfFlowInfo_FlowInfo _tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;struct _tuple18 _tmp498=_stmttmp69;_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{
struct _tuple18 _stmttmp6A=Cyc_NewControlFlow_anal_test(env,_tmp499,_tmp482);union Cyc_CfFlowInfo_FlowInfo _tmp49C;union Cyc_CfFlowInfo_FlowInfo _tmp49D;struct _tuple18 _tmp49B=_stmttmp6A;_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;{
struct _tuple18 _tmp872;return(_tmp872.f1=_tmp49C,((_tmp872.f2=Cyc_CfFlowInfo_join_flow(_tmp47A,env->all_changed,_tmp49A,_tmp49D),_tmp872)));};};}_LL2C0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp483=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp483->tag != 7)goto _LL2C2;else{_tmp484=_tmp483->f1;_tmp485=_tmp483->f2;}}_LL2C1: {
# 1928
struct _tuple18 _stmttmp67=Cyc_NewControlFlow_anal_test(env,inflow,_tmp484);union Cyc_CfFlowInfo_FlowInfo _tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp4A1;struct _tuple18 _tmp49F=_stmttmp67;_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;{
struct _tuple18 _stmttmp68=Cyc_NewControlFlow_anal_test(env,_tmp4A1,_tmp485);union Cyc_CfFlowInfo_FlowInfo _tmp4A3;union Cyc_CfFlowInfo_FlowInfo _tmp4A4;struct _tuple18 _tmp4A2=_stmttmp68;_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;{
struct _tuple18 _tmp873;return(_tmp873.f1=Cyc_CfFlowInfo_join_flow(_tmp47A,env->all_changed,_tmp4A0,_tmp4A3),((_tmp873.f2=_tmp4A4,_tmp873)));};};}_LL2C2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp486=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp486->tag != 8)goto _LL2C4;else{_tmp487=_tmp486->f1;_tmp488=_tmp486->f2;}}_LL2C3: {
# 1932
struct _tuple15 _stmttmp66=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp487);union Cyc_CfFlowInfo_FlowInfo _tmp4A7;void*_tmp4A8;struct _tuple15 _tmp4A6=_stmttmp66;_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4A7,_tmp488);}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp489=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp489->tag != 2)goto _LL2C6;else{if(_tmp489->f1 != Cyc_Absyn_Not)goto _LL2C6;if(_tmp489->f2 == 0)goto _LL2C6;_tmp48A=(struct Cyc_Absyn_Exp*)(_tmp489->f2)->hd;if((_tmp489->f2)->tl != 0)goto _LL2C6;}}_LL2C5: {
# 1935
struct _tuple18 _stmttmp65=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48A);union Cyc_CfFlowInfo_FlowInfo _tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;struct _tuple18 _tmp4A9=_stmttmp65;_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{
struct _tuple18 _tmp874;return(_tmp874.f1=_tmp4AB,((_tmp874.f2=_tmp4AA,_tmp874)));};}_LL2C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47B;if(_tmp48B->tag != 2)goto _LL2C8;else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48B->f2;}}_LL2C7:
# 1938
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp48C,_tmp48D);_LL2C8:;_LL2C9: {
# 1942
struct _tuple15 _stmttmp64=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4AE;void*_tmp4AF;struct _tuple15 _tmp4AD=_stmttmp64;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B0=_tmp4AE;struct Cyc_Dict_Dict _tmp4B1;_LL2CB: if((_tmp4B0.BottomFL).tag != 1)goto _LL2CD;_LL2CC: {
struct _tuple18 _tmp875;return(_tmp875.f1=_tmp4AE,((_tmp875.f2=_tmp4AE,_tmp875)));}_LL2CD: if((_tmp4B0.ReachableFL).tag != 2)goto _LL2CA;_tmp4B1=((struct _tuple14)(_tmp4B0.ReachableFL).val).f1;_LL2CE: {
# 1946
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4B3=_tmp4AF;struct Cyc_Absyn_Vardecl*_tmp4B5;void*_tmp4B6;enum Cyc_CfFlowInfo_InitLevel _tmp4BF;_LL2D0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4B4->tag != 8)goto _LL2D2;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=(void*)_tmp4B4->f2;}}_LL2D1:
 _tmp4AF=_tmp4B6;locname=_tmp4B5;goto retry;_LL2D2: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B7=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4B7->tag != 0)goto _LL2D4;}_LL2D3: {
struct _tuple18 _tmp876;return(_tmp876.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp876.f2=_tmp4AE,_tmp876)));}_LL2D4: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4B8=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4B8->tag != 2)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4B9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4B9->tag != 1)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4BA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4BA->tag != 5)goto _LL2DA;}_LL2D9: {
struct _tuple18 _tmp877;return(_tmp877.f1=_tmp4AE,((_tmp877.f2=Cyc_CfFlowInfo_BottomFL(),_tmp877)));}_LL2DA: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4BB->tag != 3)goto _LL2DC;else{if(_tmp4BB->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4BC=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4BC->tag != 4)goto _LL2DE;else{if(_tmp4BC->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DE;}}_LL2DD:
 goto _LL2DF;_LL2DE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4BD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4BD->tag != 7)goto _LL2E0;}_LL2DF:
# 1956
{const char*_tmp87A;void*_tmp879;(_tmp879=0,Cyc_Tcutil_terr(e->loc,((_tmp87A="expression may not be initialized",_tag_dyneither(_tmp87A,sizeof(char),34))),_tag_dyneither(_tmp879,sizeof(void*),0)));}{
struct _tuple18 _tmp87B;return(_tmp87B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87B.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87B)));};_LL2E0: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4BE->tag != 3)goto _LL2E2;else{_tmp4BF=_tmp4BE->f1;}}_LL2E1:
# 1959
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4AE,e,_tmp4BF,locname);_LL2E2: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4C0->tag != 4)goto _LL2E4;}_LL2E3: {
struct _tuple18 _tmp87C;return(_tmp87C.f1=_tmp4AE,((_tmp87C.f2=_tmp4AE,_tmp87C)));}_LL2E4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B3;if(_tmp4C1->tag != 6)goto _LL2CF;}_LL2E5: {
struct Cyc_Core_Impossible_exn_struct _tmp882;const char*_tmp881;struct Cyc_Core_Impossible_exn_struct*_tmp880;(int)_throw((void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp882.tag=Cyc_Core_Impossible,((_tmp882.f1=((_tmp881="anal_test",_tag_dyneither(_tmp881,sizeof(char),10))),_tmp882)))),_tmp880)))));}_LL2CF:;}}_LL2CA:;};}_LL2BB:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1967
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 1969
unsigned int _tmp4CC;struct Cyc_NewControlFlow_AnalEnv*_tmp4CD;struct Cyc_Dict_Dict _tmp4CE;struct _tuple26*_tmp4CB=ckenv;_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;_tmp4CE=_tmp4CB->f3;{
void*_tmp4CF=root;struct Cyc_Absyn_Vardecl*_tmp4D1;_LL2E7: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D0=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CF;if(_tmp4D0->tag != 0)goto _LL2E9;else{_tmp4D1=_tmp4D0->f1;}}_LL2E8:
# 1972
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4CD->fenv)->r,_tmp4D1->type)){
void*_tmp4D2=rval;_LL2EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4D3->tag != 8)goto _LL2EE;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D3->f2);if(_tmp4D4->tag != 7)goto _LL2EE;}}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4D5->tag != 8)goto _LL2F0;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D5->f2);if(_tmp4D6->tag != 0)goto _LL2F0;}}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4D7->tag != 8)goto _LL2F2;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D7->f2);if(_tmp4D8->tag != 3)goto _LL2F2;else{if(_tmp4D8->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F2;}}}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4D9->tag != 7)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4DA->tag != 0)goto _LL2F6;}_LL2F5:
 goto _LL2F7;_LL2F6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4DB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D2;if(_tmp4DB->tag != 3)goto _LL2F8;else{if(_tmp4DB->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F8;}}_LL2F7:
 goto _LL2EB;_LL2F8:;_LL2F9:
# 1981
{const char*_tmp886;void*_tmp885[1];struct Cyc_String_pa_PrintArg_struct _tmp884;(_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4D1->name)),((_tmp885[0]=& _tmp884,Cyc_Tcutil_warn(_tmp4CC,((_tmp886="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp886,sizeof(char),57))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));}
goto _LL2EB;_LL2EB:;}
# 1984
goto _LL2E6;_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;};}
# 1989
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4DF=flow;struct Cyc_Dict_Dict _tmp4E0;_LL2FB: if((_tmp4DF.BottomFL).tag != 1)goto _LL2FD;_LL2FC:
 return;_LL2FD: if((_tmp4DF.ReachableFL).tag != 2)goto _LL2FA;_tmp4E0=((struct _tuple14)(_tmp4DF.ReachableFL).val).f1;_LL2FE:
# 1993
{struct Cyc_List_List*_tmp4E1=env->param_roots;for(0;_tmp4E1 != 0;_tmp4E1=_tmp4E1->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E0,Cyc_CfFlowInfo_lookup_place(_tmp4E0,(struct Cyc_CfFlowInfo_Place*)_tmp4E1->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp889;void*_tmp888;(_tmp888=0,Cyc_Tcutil_terr(loc,((_tmp889="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp889,sizeof(char),76))),_tag_dyneither(_tmp888,sizeof(void*),0)));}}}
# 1997
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp88A;struct _tuple26 _tmp4E4=(_tmp88A.f1=loc,((_tmp88A.f2=env,((_tmp88A.f3=_tmp4E0,_tmp88A)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4E4,_tmp4E0);}
# 2001
return;_LL2FA:;}
# 2005
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2008
struct Cyc_List_List*_tmp4E6=0;
struct Cyc_List_List*_tmp4E7=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp6E=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;struct _tuple23*_tmp4E8=_stmttmp6E;_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;
if((_tmp4E9 != 0  && 
# 2015
Cyc_Tcutil_is_noalias_pointer((*_tmp4E9)->type,0)) && _tmp4EA != 0){
# 2017
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp88D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp88C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4EB=(_tmp88C=_region_malloc(frgn,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88D.tag=0,((_tmp88D.f1=*_tmp4E9,_tmp88D)))),_tmp88C)));
struct Cyc_CfFlowInfo_Place*_tmp88E;struct Cyc_CfFlowInfo_Place*_tmp4EC=(_tmp88E=_region_malloc(frgn,sizeof(*_tmp88E)),((_tmp88E->root=(void*)_tmp4EB,((_tmp88E->fields=0,_tmp88E)))));
{struct Cyc_List_List*_tmp88F;_tmp4E6=((_tmp88F=_region_malloc(frgn,sizeof(*_tmp88F)),((_tmp88F->hd=_tmp4EC,((_tmp88F->tl=_tmp4E6,_tmp88F))))));}{
struct Cyc_List_List*_tmp890;_tmp4E7=((_tmp890=_region_malloc(frgn,sizeof(*_tmp890)),((_tmp890->hd=_tmp4EA,((_tmp890->tl=_tmp4E7,_tmp890))))));};}}}{
# 2023
struct _tuple1 _tmp891;return(_tmp891.f1=_tmp4E6,((_tmp891.f2=_tmp4E7,_tmp891)));};}
# 2032
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2037
{union Cyc_CfFlowInfo_FlowInfo _tmp4F3=flow;_LL300: if((_tmp4F3.BottomFL).tag != 1)goto _LL302;_LL301:
 return flow;_LL302:;_LL303:
 goto _LL2FF;_LL2FF:;}{
# 2041
int _tmp4F4=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp4F5=Cyc_Absyn_exp_unsigned_one;
int _tmp4F6=0;
int _tmp4F7=1;
# 2048
{struct Cyc_List_List*_tmp4F8=consumed_vals;for(0;_tmp4F8 != 0;
(_tmp4F8=_tmp4F8->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp4F9=flow;struct Cyc_Dict_Dict _tmp4FA;_LL305: if((_tmp4F9.ReachableFL).tag != 2)goto _LL307;_tmp4FA=((struct _tuple14)(_tmp4F9.ReachableFL).val).f1;_LL306: {
# 2058
void*_tmp4FB=Cyc_CfFlowInfo_lookup_place(_tmp4FA,(struct Cyc_CfFlowInfo_Place*)_tmp4F8->hd);
struct Cyc_CfFlowInfo_Place*_stmttmp6F=(struct Cyc_CfFlowInfo_Place*)_tmp4F8->hd;void*_tmp4FD;struct Cyc_CfFlowInfo_Place*_tmp4FC=_stmttmp6F;_tmp4FD=_tmp4FC->root;
{struct _tuple0 _tmp892;struct _tuple0 _stmttmp70=(_tmp892.f1=_tmp4FD,((_tmp892.f2=_tmp4FB,_tmp892)));struct _tuple0 _tmp4FE=_stmttmp70;struct Cyc_Absyn_Vardecl*_tmp500;struct Cyc_Absyn_Vardecl*_tmp502;void*_tmp503;struct Cyc_Absyn_Vardecl*_tmp505;_LL30A:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FF=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FE.f1;if(_tmp4FF->tag != 0)goto _LL30C;else{_tmp500=_tmp4FF->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp501=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FE.f2;if(_tmp501->tag != 8)goto _LL30C;else{_tmp502=_tmp501->f1;_tmp503=(void*)_tmp501->f2;}};_LL30B:
# 2062
 if(_tmp500 == _tmp502){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4F5,_tmp4F7,_tmp503,& _tmp4F6)){
if(!_tmp4F4){
const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_PrintArg_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_qvar2string(_tmp500->name)),((_tmp895[0]=& _tmp894,Cyc_Tcutil_terr(loc,((_tmp896="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp896,sizeof(char),58))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}}else{
# 2069
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4FA,_tmp503)!= Cyc_CfFlowInfo_AllIL  && !_tmp4F4){
const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_PrintArg_struct _tmp898;(_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2072
Cyc_Absynpp_qvar2string(_tmp500->name)),((_tmp899[0]=& _tmp898,Cyc_Tcutil_terr(loc,((_tmp89A="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp89A,sizeof(char),84))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));}else{
# 2074
if(_tmp4F4){
# 2080
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp50D;union Cyc_CfFlowInfo_AbsLVal _tmp50E;struct _tuple17 _tmp50C=_stmttmp71;_tmp50D=_tmp50C.f1;_tmp50E=_tmp50C.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp50F=_tmp50E;struct Cyc_CfFlowInfo_Place*_tmp510;_LL311: if((_tmp50F.PlaceL).tag != 1)goto _LL313;_tmp510=(struct Cyc_CfFlowInfo_Place*)(_tmp50F.PlaceL).val;_LL312: {
# 2084
void*_tmp511=Cyc_CfFlowInfo_lookup_place(_tmp4FA,_tmp510);
_tmp511=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp511);{
# 2092
struct _tuple15 _tmp89C;struct _tuple15 _tmp89B;struct _tuple15 _stmttmp72=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2095
(_tmp89B.f1=_tmp50D,((_tmp89B.f2=_tmp511,_tmp89B)))),(
(_tmp89C.f1=_tmp50D,((_tmp89C.f2=_tmp503,_tmp89C)))));
# 2092
union Cyc_CfFlowInfo_FlowInfo _tmp513;void*_tmp514;struct _tuple15 _tmp512=_stmttmp72;_tmp513=_tmp512.f1;_tmp514=_tmp512.f2;
# 2097
{union Cyc_CfFlowInfo_FlowInfo _tmp515=_tmp513;struct Cyc_Dict_Dict _tmp516;struct Cyc_List_List*_tmp517;_LL316: if((_tmp515.ReachableFL).tag != 2)goto _LL318;_tmp516=((struct _tuple14)(_tmp515.ReachableFL).val).f1;_tmp517=((struct _tuple14)(_tmp515.ReachableFL).val).f2;_LL317:
# 2099
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp516,env->all_changed,_tmp510,_tmp514),_tmp517);
# 2103
goto _LL315;_LL318:;_LL319: {
# 2105
const char*_tmp89F;void*_tmp89E;(_tmp89E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp89F="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp89F,sizeof(char),42))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}_LL315:;}
# 2107
goto _LL310;};}_LL313:;_LL314: {
# 2109
const char*_tmp8A3;void*_tmp8A2[1];struct Cyc_String_pa_PrintArg_struct _tmp8A1;(_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)unconsume_exps->hd)),((_tmp8A2[0]=& _tmp8A1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A3="unconsume_params: |%s| not an lval!",_tag_dyneither(_tmp8A3,sizeof(char),36))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))));}_LL310:;};}}}}else{
# 2114
_tmp505=_tmp500;goto _LL30D;}
goto _LL309;_LL30C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp504=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FE.f1;if(_tmp504->tag != 0)goto _LL30E;else{_tmp505=_tmp504->f1;}}_LL30D:
# 2117
 if(!_tmp4F4){
const char*_tmp8A7;void*_tmp8A6[1];struct Cyc_String_pa_PrintArg_struct _tmp8A5;(_tmp8A5.tag=0,((_tmp8A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_qvar2string(_tmp505->name)),((_tmp8A6[0]=& _tmp8A5,Cyc_Tcutil_terr(loc,((_tmp8A7="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8A7,sizeof(char),85))),_tag_dyneither(_tmp8A6,sizeof(void*),1)))))));}
goto _LL309;_LL30E:;_LL30F: {
# 2123
const char*_tmp8AA;void*_tmp8A9;(_tmp8A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8AA="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8AA,sizeof(char),40))),_tag_dyneither(_tmp8A9,sizeof(void*),0)));}_LL309:;}
# 2125
goto _LL304;}_LL307: if((_tmp4F9.BottomFL).tag != 1)goto _LL304;_LL308: {
# 2127
const char*_tmp8AD;void*_tmp8AC;(_tmp8AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8AD="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp8AD,sizeof(char),38))),_tag_dyneither(_tmp8AC,sizeof(void*),0)));}_LL304:;}}
# 2130
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2133
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2136
struct Cyc_CfFlowInfo_FlowEnv*_tmp527=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp73=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Stmt*_tmp52B;unsigned int _tmp52C;struct Cyc_Absyn_Switch_clause*_tmp528=_stmttmp73;_tmp529=_tmp528->pat_vars;_tmp52A=_tmp528->where_clause;_tmp52B=_tmp528->body;_tmp52C=_tmp528->loc;{
struct _tuple1 _stmttmp74=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp529))->v);struct Cyc_List_List*_tmp52E;struct Cyc_List_List*_tmp52F;struct _tuple1 _tmp52D=_stmttmp74;_tmp52E=_tmp52D.f1;_tmp52F=_tmp52D.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp529->v,_tmp52E != 0,_tmp52C,exp_loc);
# 2144
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp530=env->unique_pat_vars;
{struct _tuple27*_tmp8B0;struct Cyc_List_List*_tmp8AF;env->unique_pat_vars=(
(_tmp8AF=_region_malloc(env->r,sizeof(*_tmp8AF)),((_tmp8AF->hd=((_tmp8B0=_region_malloc(env->r,sizeof(*_tmp8B0)),((_tmp8B0->f1=_tmp52B,((_tmp8B0->f2=_tmp52E,((_tmp8B0->f3=_tmp52F,_tmp8B0)))))))),((_tmp8AF->tl=_tmp530,_tmp8AF))))));}
# 2149
if(_tmp52A != 0){
struct Cyc_Absyn_Exp*wexp=_tmp52A;
struct _tuple18 _stmttmp75=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp534;union Cyc_CfFlowInfo_FlowInfo _tmp535;struct _tuple18 _tmp533=_stmttmp75;_tmp534=_tmp533.f1;_tmp535=_tmp533.f2;
inflow=_tmp535;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp534,_tmp52B,0);}else{
# 2155
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp52B,0);}
# 2157
env->unique_pat_vars=_tmp530;{
union Cyc_CfFlowInfo_FlowInfo _tmp536=clause_outflow;_LL31B: if((_tmp536.BottomFL).tag != 1)goto _LL31D;_LL31C:
 goto _LL31A;_LL31D:;_LL31E:
# 2162
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp52E,_tmp52F,clause_outflow,_tmp52C);
# 2164
if(scs->tl == 0)
return clause_outflow;else{
# 2169
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8B3;void*_tmp8B2;(_tmp8B2=0,Cyc_Tcutil_terr(_tmp52B->loc,((_tmp8B3="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B3,sizeof(char),38))),_tag_dyneither(_tmp8B2,sizeof(void*),0)));}else{
# 2172
const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_warn(_tmp52B->loc,((_tmp8B6="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B6,sizeof(char),38))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}
# 2174
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2176
goto _LL31A;_LL31A:;};};};}
# 2179
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2184
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2186
struct Cyc_NewControlFlow_AnalEnv*_tmp53C;struct Cyc_Dict_Dict _tmp53D;unsigned int _tmp53E;struct _tuple28*_tmp53B=vdenv;_tmp53C=_tmp53B->f1;_tmp53D=_tmp53B->f2;_tmp53E=_tmp53B->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp53C->fenv)->r,vd->type)){
# 2189
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8B7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8B7.tag=0,((_tmp8B7.f1=vd,_tmp8B7)));
# 2191
void*_tmp53F=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp53D,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp540=_tmp53F;_LL320: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp541=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp541->tag != 8)goto _LL322;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp542=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp541->f2);if(_tmp542->tag != 7)goto _LL322;}}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp543=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp543->tag != 8)goto _LL324;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp544=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp543->f2);if(_tmp544->tag != 0)goto _LL324;}}_LL323:
 goto _LL325;_LL324: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp545=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp545->tag != 8)goto _LL326;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp546=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp545->f2);if(_tmp546->tag != 3)goto _LL326;else{if(_tmp546->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}}_LL325:
 goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp547=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp547->tag != 7)goto _LL328;}_LL327:
 goto _LL329;_LL328: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp548=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp548->tag != 0)goto _LL32A;}_LL329:
 goto _LL32B;_LL32A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp549=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp540;if(_tmp549->tag != 3)goto _LL32C;else{if(_tmp549->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32C;}}_LL32B:
 goto _LL31F;_LL32C:;_LL32D:
# 2200
{const char*_tmp8BB;void*_tmp8BA[1];struct Cyc_String_pa_PrintArg_struct _tmp8B9;(_tmp8B9.tag=0,((_tmp8B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8BA[0]=& _tmp8B9,Cyc_Tcutil_warn(_tmp53E,((_tmp8BB="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8BB,sizeof(char),68))),_tag_dyneither(_tmp8BA,sizeof(void*),1)))))));}
# 2202
goto _LL31F;_LL31F:;}}
# 2207
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2209
{union Cyc_CfFlowInfo_FlowInfo _tmp54E=inflow;struct Cyc_Dict_Dict _tmp54F;_LL32F: if((_tmp54E.ReachableFL).tag != 2)goto _LL331;_tmp54F=((struct _tuple14)(_tmp54E.ReachableFL).val).f1;_LL330: {
# 2211
struct _tuple28 _tmp8BC;struct _tuple28 _tmp550=(_tmp8BC.f1=env,((_tmp8BC.f2=_tmp54F,((_tmp8BC.f3=decl->loc,_tmp8BC)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp551=env->fenv;
{void*_stmttmp76=decl->r;void*_tmp552=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp558;_LL334: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp553=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp552;if(_tmp553->tag != 0)goto _LL336;else{_tmp554=_tmp553->f1;}}_LL335:
# 2215
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp550,_tmp554);
goto _LL333;_LL336: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp555=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp552;if(_tmp555->tag != 2)goto _LL338;else{if(_tmp555->f2 == 0)goto _LL338;_tmp556=(struct Cyc_List_List*)(_tmp555->f2)->v;}}_LL337: {
# 2218
struct _tuple1 _stmttmp77=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp556);struct Cyc_List_List*_tmp55A;struct _tuple1 _tmp559=_stmttmp77;_tmp55A=_tmp559.f1;{
struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp55A);
_tmp558=_tmp55B;goto _LL339;};}_LL338: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp557=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp552;if(_tmp557->tag != 3)goto _LL33A;else{_tmp558=_tmp557->f1;}}_LL339:
# 2222
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp550,_tmp558);
goto _LL333;_LL33A:;_LL33B:
 goto _LL333;_LL333:;}
# 2226
goto _LL32E;}_LL331:;_LL332:
 goto _LL32E;_LL32E:;}
# 2229
return;}
# 2235
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2238
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_stmttmp78=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp560;struct _tuple27*_tmp55D=_stmttmp78;_tmp55E=_tmp55D->f1;_tmp55F=_tmp55D->f2;_tmp560=_tmp55D->f3;
keep_going=0;{
# 2246
struct Cyc_Absyn_Stmt*_tmp561=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp561 != _tmp55E){
struct Cyc_Absyn_Stmt*_tmp562=(Cyc_NewControlFlow_get_stmt_annot(_tmp561))->encloser;
if(_tmp562 == _tmp561){
# 2252
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp55F,_tmp560,inflow,dest->loc);
keep_going=1;
break;}
# 2256
_tmp561=_tmp562;}};}}
# 2259
return inflow;}
# 2265
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2268
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _stmttmp79=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp564;union Cyc_CfFlowInfo_FlowInfo*_tmp565;struct _tuple19 _tmp563=_stmttmp79;_tmp564=_tmp563.f1;_tmp565=_tmp563.f2;
inflow=*_tmp565;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp566=env->fenv;
# 2275
void*_stmttmp7A=s->r;void*_tmp567=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Stmt*_tmp56F;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Stmt*_tmp573;struct Cyc_Absyn_Stmt*_tmp574;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Stmt*_tmp578;struct Cyc_Absyn_Stmt*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;struct Cyc_Absyn_Stmt*_tmp57C;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Stmt*_tmp580;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Stmt*_tmp582;struct Cyc_Absyn_Stmt*_tmp583;struct Cyc_List_List*_tmp586;struct Cyc_Absyn_Switch_clause*_tmp587;struct Cyc_Absyn_Stmt*_tmp589;struct Cyc_Absyn_Stmt*_tmp58B;struct Cyc_Absyn_Stmt*_tmp58D;struct Cyc_Absyn_Exp*_tmp58F;struct Cyc_List_List*_tmp590;struct Cyc_Absyn_Stmt*_tmp592;struct Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp596;struct Cyc_Absyn_Exp*_tmp597;unsigned int _tmp598;struct Cyc_Absyn_Stmt*_tmp599;struct Cyc_Absyn_Tvar*_tmp59C;struct Cyc_Absyn_Vardecl*_tmp59D;struct Cyc_Absyn_Stmt*_tmp59E;struct Cyc_Absyn_Decl*_tmp5A0;struct Cyc_Absyn_Stmt*_tmp5A1;struct Cyc_Absyn_Stmt*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;_LL33D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp568=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp568->tag != 0)goto _LL33F;}_LL33E:
 return inflow;_LL33F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp569->tag != 3)goto _LL341;else{if(_tmp569->f1 != 0)goto _LL341;}}_LL340:
# 2278
 if(env->noreturn){
const char*_tmp8BF;void*_tmp8BE;(_tmp8BE=0,Cyc_Tcutil_terr(s->loc,((_tmp8BF="`noreturn' function might return",_tag_dyneither(_tmp8BF,sizeof(char),33))),_tag_dyneither(_tmp8BE,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL341: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp56A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp56A->tag != 3)goto _LL343;else{_tmp56B=_tmp56A->f1;}}_LL342:
# 2284
 if(env->noreturn){
const char*_tmp8C2;void*_tmp8C1;(_tmp8C1=0,Cyc_Tcutil_terr(s->loc,((_tmp8C2="`noreturn' function might return",_tag_dyneither(_tmp8C2,sizeof(char),33))),_tag_dyneither(_tmp8C1,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp56B));union Cyc_CfFlowInfo_FlowInfo _tmp5AB;void*_tmp5AC;struct _tuple15 _tmp5AA=_stmttmp8D;_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;
_tmp5AB=Cyc_NewControlFlow_use_Rval(env,_tmp56B->loc,_tmp5AB,_tmp5AC);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5AB);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp5AB,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL343: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp56C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp56C->tag != 1)goto _LL345;else{_tmp56D=_tmp56C->f1;}}_LL344: {
# 2293
struct _tuple16*_tmp5AD=rval_opt;void**_tmp5AE;int _tmp5AF;_LL36A: if(_tmp5AD == 0)goto _LL36C;_tmp5AE=(void**)& _tmp5AD->f1;_tmp5AF=_tmp5AD->f2;_LL36B: {
# 2295
struct _tuple15 _stmttmp8C=Cyc_NewControlFlow_anal_Rexp(env,_tmp5AF,inflow,_tmp56D);union Cyc_CfFlowInfo_FlowInfo _tmp5B1;void*_tmp5B2;struct _tuple15 _tmp5B0=_stmttmp8C;_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;
*_tmp5AE=_tmp5B2;
return _tmp5B1;}_LL36C: if(_tmp5AD != 0)goto _LL369;_LL36D:
# 2299
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp56D)).f1;_LL369:;}_LL345: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp56E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp56E->tag != 2)goto _LL347;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;}}_LL346:
# 2303
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp56F,0),_tmp570,rval_opt);_LL347: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp571=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp571->tag != 4)goto _LL349;else{_tmp572=_tmp571->f1;_tmp573=_tmp571->f2;_tmp574=_tmp571->f3;}}_LL348: {
# 2306
struct _tuple18 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp572);union Cyc_CfFlowInfo_FlowInfo _tmp5B4;union Cyc_CfFlowInfo_FlowInfo _tmp5B5;struct _tuple18 _tmp5B3=_stmttmp8B;_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;{
# 2313
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=Cyc_NewControlFlow_anal_stmt(env,_tmp5B5,_tmp574,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5B7=Cyc_NewControlFlow_anal_stmt(env,_tmp5B4,_tmp573,0);
return Cyc_CfFlowInfo_join_flow(_tmp566,env->all_changed,_tmp5B7,_tmp5B6);};}_LL349: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp575=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp575->tag != 5)goto _LL34B;else{_tmp576=(_tmp575->f1).f1;_tmp577=(_tmp575->f1).f2;_tmp578=_tmp575->f2;}}_LL34A: {
# 2321
struct _tuple19 _stmttmp89=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp577);union Cyc_CfFlowInfo_FlowInfo*_tmp5B9;struct _tuple19 _tmp5B8=_stmttmp89;_tmp5B9=_tmp5B8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BA=*_tmp5B9;
struct _tuple18 _stmttmp8A=Cyc_NewControlFlow_anal_test(env,_tmp5BA,_tmp576);union Cyc_CfFlowInfo_FlowInfo _tmp5BC;union Cyc_CfFlowInfo_FlowInfo _tmp5BD;struct _tuple18 _tmp5BB=_stmttmp8A;_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BE=Cyc_NewControlFlow_anal_stmt(env,_tmp5BC,_tmp578,0);
Cyc_NewControlFlow_update_flow(env,_tmp577,_tmp5BE);
return _tmp5BD;};};}_LL34B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp579=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp579->tag != 14)goto _LL34D;else{_tmp57A=_tmp579->f1;_tmp57B=(_tmp579->f2).f1;_tmp57C=(_tmp579->f2).f2;}}_LL34C: {
# 2331
union Cyc_CfFlowInfo_FlowInfo _tmp5BF=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp57A,0);
struct _tuple19 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BF,_tmp57C);union Cyc_CfFlowInfo_FlowInfo*_tmp5C1;struct _tuple19 _tmp5C0=_stmttmp87;_tmp5C1=_tmp5C0.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C2=*_tmp5C1;
struct _tuple18 _stmttmp88=Cyc_NewControlFlow_anal_test(env,_tmp5C2,_tmp57B);union Cyc_CfFlowInfo_FlowInfo _tmp5C4;union Cyc_CfFlowInfo_FlowInfo _tmp5C5;struct _tuple18 _tmp5C3=_stmttmp88;_tmp5C4=_tmp5C3.f1;_tmp5C5=_tmp5C3.f2;
Cyc_NewControlFlow_update_flow(env,_tmp57A,_tmp5C4);
return _tmp5C5;};}_LL34D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp57D=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp57D->tag != 9)goto _LL34F;else{_tmp57E=_tmp57D->f1;_tmp57F=(_tmp57D->f2).f1;_tmp580=(_tmp57D->f2).f2;_tmp581=(_tmp57D->f3).f1;_tmp582=(_tmp57D->f3).f2;_tmp583=_tmp57D->f4;}}_LL34E: {
# 2340
union Cyc_CfFlowInfo_FlowInfo _tmp5C6=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp57E)).f1;
struct _tuple19 _stmttmp84=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C6,_tmp580);union Cyc_CfFlowInfo_FlowInfo*_tmp5C8;struct _tuple19 _tmp5C7=_stmttmp84;_tmp5C8=_tmp5C7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=*_tmp5C8;
struct _tuple18 _stmttmp85=Cyc_NewControlFlow_anal_test(env,_tmp5C9,_tmp57F);union Cyc_CfFlowInfo_FlowInfo _tmp5CB;union Cyc_CfFlowInfo_FlowInfo _tmp5CC;struct _tuple18 _tmp5CA=_stmttmp85;_tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CD=Cyc_NewControlFlow_anal_stmt(env,_tmp5CB,_tmp583,0);
struct _tuple19 _stmttmp86=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CD,_tmp582);union Cyc_CfFlowInfo_FlowInfo*_tmp5CF;struct _tuple19 _tmp5CE=_stmttmp86;_tmp5CF=_tmp5CE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D0=*_tmp5CF;
union Cyc_CfFlowInfo_FlowInfo _tmp5D1=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5D0,_tmp581)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp580,_tmp5D1);
return _tmp5CC;};};};}_LL34F: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp584=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp584->tag != 6)goto _LL351;else{if(_tmp584->f1 != 0)goto _LL351;}}_LL350:
# 2352
 return Cyc_CfFlowInfo_BottomFL();_LL351: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp585=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp585->tag != 11)goto _LL353;else{_tmp586=_tmp585->f1;if(_tmp585->f2 == 0)goto _LL353;_tmp587=*_tmp585->f2;}}_LL352: {
# 2354
struct _RegionHandle _tmp5D2=_new_region("temp");struct _RegionHandle*temp=& _tmp5D2;_push_region(temp);
{struct _tuple22 _stmttmp82=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp586,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5D4;struct Cyc_List_List*_tmp5D5;struct _tuple22 _tmp5D3=_stmttmp82;_tmp5D4=_tmp5D3.f1;_tmp5D5=_tmp5D3.f2;
# 2357
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp587->body);{
# 2359
struct Cyc_List_List*_tmp5D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp587->pat_vars))->v)).f1);
_tmp5D4=Cyc_NewControlFlow_add_vars(_tmp566,_tmp5D4,_tmp5D6,_tmp566->unknown_all,s->loc,0);
# 2362
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp587->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp83=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;struct _tuple23*_tmp5D7=_stmttmp83;_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;
if(_tmp5D8 != 0){
_tmp5D4=Cyc_NewControlFlow_do_initialize_var(_tmp566,env,_tmp5D4,*_tmp5D8,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp586))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5D5))->hd,s->loc);
_tmp5D5=_tmp5D5->tl;
_tmp586=_tmp586->tl;}}}
# 2370
Cyc_NewControlFlow_update_flow(env,_tmp587->body,_tmp5D4);{
union Cyc_CfFlowInfo_FlowInfo _tmp5DA=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5DA;};};}
# 2355
;_pop_region(temp);}_LL353: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp588=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp588->tag != 6)goto _LL355;else{_tmp589=_tmp588->f1;}}_LL354:
# 2373
 _tmp58B=_tmp589;goto _LL356;_LL355: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp58A=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp58A->tag != 7)goto _LL357;else{_tmp58B=_tmp58A->f1;}}_LL356:
 _tmp58D=_tmp58B;goto _LL358;_LL357: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp58C=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp58C->tag != 8)goto _LL359;else{_tmp58D=_tmp58C->f2;}}_LL358:
# 2378
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5DB=_tmp564->encloser;
struct Cyc_Absyn_Stmt*_tmp5DC=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp58D)))->encloser;
while(_tmp5DC != _tmp5DB){
struct Cyc_Absyn_Stmt*_tmp5DD=(Cyc_NewControlFlow_get_stmt_annot(_tmp5DB))->encloser;
if(_tmp5DD == _tmp5DB){
{const char*_tmp8C5;void*_tmp8C4;(_tmp8C4=0,Cyc_Tcutil_terr(s->loc,((_tmp8C5="goto enters local scope or exception handler",_tag_dyneither(_tmp8C5,sizeof(char),45))),_tag_dyneither(_tmp8C4,sizeof(void*),0)));}
break;}
# 2387
_tmp5DB=_tmp5DD;}}
# 2391
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp58D));
# 2393
Cyc_NewControlFlow_update_flow(env,_tmp58D,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL359: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp58E=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp58E->tag != 10)goto _LL35B;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58E->f2;}}_LL35A:
# 2399
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp590,_tmp58F->loc);_LL35B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp591=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp591->tag != 15)goto _LL35D;else{_tmp592=_tmp591->f1;_tmp593=_tmp591->f2;}}_LL35C: {
# 2404
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp592,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2412
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2415
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp593,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5E0=scs_outflow;_LL36F: if((_tmp5E0.BottomFL).tag != 1)goto _LL371;_LL370:
 goto _LL36E;_LL371:;_LL372: {
const char*_tmp8C8;void*_tmp8C7;(_tmp8C7=0,Cyc_Tcutil_terr(s->loc,((_tmp8C8="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8C8,sizeof(char),42))),_tag_dyneither(_tmp8C7,sizeof(void*),0)));}_LL36E:;}
# 2421
outflow=s1_outflow;
# 2423
return outflow;};};}_LL35D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp594=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp594->tag != 12)goto _LL35F;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp595=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp594->f1)->r;if(_tmp595->tag != 2)goto _LL35F;else{if(_tmp595->f2 == 0)goto _LL35F;_tmp596=(struct Cyc_List_List*)(_tmp595->f2)->v;_tmp597=_tmp595->f3;}}_tmp598=(_tmp594->f1)->loc;_tmp599=_tmp594->f2;}}_LL35E: {
# 2433
struct _tuple1 _stmttmp81=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp596);struct Cyc_List_List*_tmp5E4;struct Cyc_List_List*_tmp5E5;struct _tuple1 _tmp5E3=_stmttmp81;_tmp5E4=_tmp5E3.f1;_tmp5E5=_tmp5E3.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp566,env,inflow,_tmp596,_tmp5E4 != 0,_tmp598,_tmp597->loc);{
struct Cyc_List_List*_tmp5E6=env->unique_pat_vars;
{struct _tuple27*_tmp8CB;struct Cyc_List_List*_tmp8CA;env->unique_pat_vars=(
(_tmp8CA=_region_malloc(env->r,sizeof(*_tmp8CA)),((_tmp8CA->hd=((_tmp8CB=_region_malloc(env->r,sizeof(*_tmp8CB)),((_tmp8CB->f1=_tmp599,((_tmp8CB->f2=_tmp5E4,((_tmp8CB->f3=_tmp5E5,_tmp8CB)))))))),((_tmp8CA->tl=_tmp5E6,_tmp8CA))))));}
# 2441
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp599,rval_opt);
env->unique_pat_vars=_tmp5E6;
# 2445
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E4,_tmp5E5,inflow,_tmp598);
# 2449
return inflow;};}_LL35F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp59A->tag != 12)goto _LL361;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp59B=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp59A->f1)->r;if(_tmp59B->tag != 5)goto _LL361;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59B->f2;}}_tmp59E=_tmp59A->f2;}}_LL360: {
# 2453
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp59D->initializer);
struct _tuple17 _stmttmp7D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp5EA;union Cyc_CfFlowInfo_AbsLVal _tmp5EB;struct _tuple17 _tmp5E9=_stmttmp7D;_tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;{
# 2458
struct _tuple17 _tmp8CC;struct _tuple17 _stmttmp7E=(_tmp8CC.f1=_tmp5EA,((_tmp8CC.f2=_tmp5EB,_tmp8CC)));struct _tuple17 _tmp5EC=_stmttmp7E;struct Cyc_Dict_Dict _tmp5ED;struct Cyc_List_List*_tmp5EE;struct Cyc_CfFlowInfo_Place*_tmp5EF;_LL374: if(((_tmp5EC.f1).ReachableFL).tag != 2)goto _LL376;_tmp5ED=((struct _tuple14)((_tmp5EC.f1).ReachableFL).val).f1;_tmp5EE=((struct _tuple14)((_tmp5EC.f1).ReachableFL).val).f2;if(((_tmp5EC.f2).PlaceL).tag != 1)goto _LL376;_tmp5EF=(struct Cyc_CfFlowInfo_Place*)((_tmp5EC.f2).PlaceL).val;_LL375: {
# 2460
void*_tmp5F0=Cyc_CfFlowInfo_lookup_place(_tmp5ED,_tmp5EF);
void*t=(void*)_check_null(e->topt);
void*_tmp5F1=Cyc_CfFlowInfo_make_unique_consumed(_tmp566,t,e,env->iteration_num,_tmp5F0);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8CF;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8CE;_tmp5F1=(void*)((_tmp8CE=_region_malloc(_tmp566->r,sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CF.tag=8,((_tmp8CF.f1=_tmp59D,((_tmp8CF.f2=_tmp5F1,_tmp8CF)))))),_tmp8CE))));}
_tmp5ED=Cyc_CfFlowInfo_assign_place(_tmp566,e->loc,_tmp5ED,env->all_changed,_tmp5EF,_tmp5F1);
_tmp5EA=Cyc_CfFlowInfo_ReachableFL(_tmp5ED,_tmp5EE);{
# 2473
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp7F=Cyc_CfFlowInfo_initlevel(_tmp566,_tmp5ED,_tmp5F0);switch(_stmttmp7F){case Cyc_CfFlowInfo_AllIL: _LL37A:
 leaf=_tmp566->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL37B:
 leaf=_tmp566->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL37C:
 leaf=_tmp566->unknown_this;break;}}{
# 2479
void*_tmp5F4=Cyc_CfFlowInfo_typ_to_absrval(_tmp566,t,0,leaf);
_tmp5EA=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5EA,_tmp5F0);{
struct Cyc_List_List _tmp8D0;struct Cyc_List_List _tmp5F5=(_tmp8D0.hd=_tmp59D,((_tmp8D0.tl=0,_tmp8D0)));
# 2483
_tmp5EA=Cyc_NewControlFlow_add_vars(_tmp566,_tmp5EA,& _tmp5F5,_tmp566->unknown_all,s->loc,0);
# 2487
_tmp5EA=Cyc_NewControlFlow_anal_stmt(env,_tmp5EA,_tmp59E,rval_opt);{
# 2489
union Cyc_CfFlowInfo_FlowInfo _tmp5F6=_tmp5EA;struct Cyc_Dict_Dict _tmp5F7;struct Cyc_List_List*_tmp5F8;_LL37F: if((_tmp5F6.ReachableFL).tag != 2)goto _LL381;_tmp5F7=((struct _tuple14)(_tmp5F6.ReachableFL).val).f1;_tmp5F8=((struct _tuple14)(_tmp5F6.ReachableFL).val).f2;_LL380:
# 2492
{void*_stmttmp80=Cyc_CfFlowInfo_lookup_place(_tmp5F7,_tmp5EF);void*_tmp5F9=_stmttmp80;struct Cyc_Absyn_Vardecl*_tmp5FB;_LL384: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5FA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F9;if(_tmp5FA->tag != 8)goto _LL386;else{_tmp5FB=_tmp5FA->f1;}}_LL385:
# 2494
 if(_tmp5FB != _tmp59D)goto _LL387;
goto _LL383;_LL386:;_LL387:
# 2497
{const char*_tmp8D3;void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(s->loc,((_tmp8D3="aliased location was overwritten",_tag_dyneither(_tmp8D3,sizeof(char),33))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}
goto _LL383;_LL383:;}
# 2500
_tmp5F7=Cyc_CfFlowInfo_assign_place(_tmp566,s->loc,_tmp5F7,env->all_changed,_tmp5EF,_tmp5F4);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5F7,_tmp5F8);
# 2505
return outflow;_LL381:;_LL382:
 return _tmp5EA;_LL37E:;};};};};}_LL376: if(((_tmp5EC.f1).BottomFL).tag != 1)goto _LL378;_LL377:
# 2508
 return _tmp5EA;_LL378:;_LL379:
{const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_terr(e->loc,((_tmp8D6="bad alias expression--no unique path found",_tag_dyneither(_tmp8D6,sizeof(char),43))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL373:;};}_LL361: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp59F->tag != 12)goto _LL363;else{_tmp5A0=_tmp59F->f1;_tmp5A1=_tmp59F->f2;}}_LL362:
# 2514
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5A0),_tmp5A1,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5A0);
return outflow;_LL363: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5A2=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp5A2->tag != 13)goto _LL365;else{_tmp5A3=_tmp5A2->f2;}}_LL364:
# 2519
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A3,rval_opt);_LL365: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5A4=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp567;if(_tmp5A4->tag != 16)goto _LL367;else{_tmp5A5=_tmp5A4->f1;}}_LL366: {
# 2522
struct _tuple15 _stmttmp7B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A5);union Cyc_CfFlowInfo_FlowInfo _tmp603;void*_tmp604;struct _tuple15 _tmp602=_stmttmp7B;_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp605=Cyc_NewControlFlow_use_Rval(env,_tmp5A5->loc,_tmp603,_tmp604);
void*_stmttmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp5A5->topt));void*_tmp606=_stmttmp7C;void*_tmp608;_LL389: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 15)goto _LL38B;else{_tmp608=(void*)_tmp607->f1;}}_LL38A:
# 2526
 return Cyc_CfFlowInfo_kill_flow_region(_tmp566,_tmp603,_tmp608);_LL38B:;_LL38C: {
struct Cyc_Core_Impossible_exn_struct _tmp8DC;const char*_tmp8DB;struct Cyc_Core_Impossible_exn_struct*_tmp8DA;(int)_throw((void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DC.tag=Cyc_Core_Impossible,((_tmp8DC.f1=((_tmp8DB="anal_stmt -- reset_region",_tag_dyneither(_tmp8DB,sizeof(char),26))),_tmp8DC)))),_tmp8DA)))));}_LL388:;};}_LL367:;_LL368: {
# 2530
struct Cyc_Core_Impossible_exn_struct _tmp8E2;const char*_tmp8E1;struct Cyc_Core_Impossible_exn_struct*_tmp8E0;(int)_throw((void*)((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E2.tag=Cyc_Core_Impossible,((_tmp8E2.f1=((_tmp8E1="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8E1,sizeof(char),56))),_tmp8E2)))),_tmp8E0)))));}_LL33C:;};}
# 2535
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2538
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp60F=env->fenv;
void*_stmttmp8E=decl->r;void*_tmp610=_stmttmp8E;struct Cyc_Absyn_Vardecl*_tmp612;struct Cyc_List_List*_tmp614;struct Cyc_Absyn_Fndecl*_tmp616;struct Cyc_Absyn_Tvar*_tmp618;struct Cyc_Absyn_Vardecl*_tmp619;int _tmp61A;struct Cyc_Absyn_Exp*_tmp61B;_LL38E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp611=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp611->tag != 0)goto _LL390;else{_tmp612=_tmp611->f1;}}_LL38F: {
# 2548
struct Cyc_List_List _tmp8E3;struct Cyc_List_List _tmp61C=(_tmp8E3.hd=_tmp612,((_tmp8E3.tl=0,_tmp8E3)));
inflow=Cyc_NewControlFlow_add_vars(_tmp60F,inflow,& _tmp61C,_tmp60F->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp61D=_tmp612->initializer;
if(_tmp61D == 0)
return inflow;{
struct _tuple15 _stmttmp90=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp61D);union Cyc_CfFlowInfo_FlowInfo _tmp61F;void*_tmp620;struct _tuple15 _tmp61E=_stmttmp90;_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp60F,env,_tmp61F,_tmp612,_tmp61D,_tmp620,decl->loc);};};}_LL390: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp613=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp613->tag != 3)goto _LL392;else{_tmp614=_tmp613->f1;}}_LL391:
# 2557
 return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,_tmp614,_tmp60F->unknown_none,decl->loc,0);_LL392: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp615=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp615->tag != 1)goto _LL394;else{_tmp616=_tmp615->f1;}}_LL393:
# 2560
 Cyc_NewControlFlow_check_nested_fun(_tmp60F,inflow,_tmp616);{
void*t=(void*)_check_null(_tmp616->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp622=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp616->fn_vardecl);
# 2566
struct Cyc_List_List*_tmp8E4;return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,((_tmp8E4=_region_malloc(env->r,sizeof(*_tmp8E4)),((_tmp8E4->hd=_tmp622,((_tmp8E4->tl=0,_tmp8E4)))))),_tmp60F->unknown_all,decl->loc,0);};_LL394: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp617=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp617->tag != 4)goto _LL396;else{_tmp618=_tmp617->f1;_tmp619=_tmp617->f2;_tmp61A=_tmp617->f3;_tmp61B=_tmp617->f4;}}_LL395:
# 2569
 if(_tmp61B != 0){
struct Cyc_Absyn_Exp*_tmp624=_tmp61B;
struct _tuple15 _stmttmp8F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp624);union Cyc_CfFlowInfo_FlowInfo _tmp626;void*_tmp627;struct _tuple15 _tmp625=_stmttmp8F;_tmp626=_tmp625.f1;_tmp627=_tmp625.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp624->loc,_tmp626,_tmp627);}{
# 2574
struct Cyc_List_List _tmp8E5;struct Cyc_List_List _tmp628=(_tmp8E5.hd=_tmp619,((_tmp8E5.tl=0,_tmp8E5)));
return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,& _tmp628,_tmp60F->unknown_all,decl->loc,0);};_LL396:;_LL397: {
# 2578
struct Cyc_Core_Impossible_exn_struct _tmp8EB;const char*_tmp8EA;struct Cyc_Core_Impossible_exn_struct*_tmp8E9;(int)_throw((void*)((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EB.tag=Cyc_Core_Impossible,((_tmp8EB.f1=((_tmp8EA="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8EA,sizeof(char),35))),_tmp8EB)))),_tmp8E9)))));}_LL38D:;}
# 2586
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp62D=_new_region("frgn");struct _RegionHandle*frgn=& _tmp62D;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2588
;_pop_region(frgn);}
# 2593
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2597
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2601
struct _RegionHandle*_tmp62E=fenv->r;
unsigned int _tmp62F=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp62F,1);{
# 2607
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp630=inflow;struct Cyc_Dict_Dict _tmp631;struct Cyc_List_List*_tmp632;_LL399: if((_tmp630.BottomFL).tag != 1)goto _LL39B;_LL39A: {
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8EE="check_fun",_tag_dyneither(_tmp8EE,sizeof(char),10))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}_LL39B: if((_tmp630.ReachableFL).tag != 2)goto _LL398;_tmp631=((struct _tuple14)(_tmp630.ReachableFL).val).f1;_tmp632=((struct _tuple14)(_tmp630.ReachableFL).val).f2;_LL39C: {
# 2614
struct Cyc_List_List*atts;
{void*_stmttmp91=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp635=_stmttmp91;struct Cyc_List_List*_tmp637;_LL39E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp636=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp635;if(_tmp636->tag != 9)goto _LL3A0;else{_tmp637=(_tmp636->f1).attributes;}}_LL39F:
 atts=_tmp637;goto _LL39D;_LL3A0:;_LL3A1: {
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8F1="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8F1,sizeof(char),50))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}_LL39D:;}
# 2619
for(0;atts != 0;atts=atts->tl){
void*_stmttmp92=(void*)atts->hd;void*_tmp63A=_stmttmp92;int _tmp63C;int _tmp63E;int _tmp640;_LL3A3: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp63B=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp63A;if(_tmp63B->tag != 21)goto _LL3A5;else{_tmp63C=_tmp63B->f1;}}_LL3A4: {
# 2622
unsigned int j=(unsigned int)_tmp63C;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8F4;void*_tmp8F3;(_tmp8F3=0,Cyc_Tcutil_terr(_tmp62F,((_tmp8F4="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8F4,sizeof(char),52))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}
continue;}{
# 2628
struct Cyc_Absyn_Vardecl*_tmp643=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp643->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,Cyc_Tcutil_terr(_tmp62F,((_tmp8F7="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8F7,sizeof(char),52))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}
continue;}{
# 2634
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp646=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8FA;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8F9;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp647=(_tmp8F9=_region_malloc(_tmp62E,sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FA.tag=2,((_tmp8FA.f1=(int)j,((_tmp8FA.f2=t,_tmp8FA)))))),_tmp8F9)));
struct Cyc_CfFlowInfo_Place*_tmp8FB;struct Cyc_CfFlowInfo_Place*_tmp648=(_tmp8FB=_region_malloc(_tmp62E,sizeof(*_tmp8FB)),((_tmp8FB->root=(void*)_tmp647,((_tmp8FB->fields=0,_tmp8FB)))));
_tmp631=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp631,(void*)_tmp647,_tmp646);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp901;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp900;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8FE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8FD;_tmp631=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp631,(void*)((_tmp8FD=_region_malloc(_tmp62E,sizeof(*_tmp8FD)),((_tmp8FD[0]=((_tmp8FE.tag=0,((_tmp8FE.f1=_tmp643,_tmp8FE)))),_tmp8FD)))),(void*)((_tmp900=_region_malloc(_tmp62E,sizeof(*_tmp900)),((_tmp900[0]=((_tmp901.tag=5,((_tmp901.f1=_tmp648,_tmp901)))),_tmp900)))));}
goto _LL3A2;};};}_LL3A5: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp63D=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp63A;if(_tmp63D->tag != 20)goto _LL3A7;else{_tmp63E=_tmp63D->f1;}}_LL3A6: {
# 2644
unsigned int j=(unsigned int)_tmp63E;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp904;void*_tmp903;(_tmp903=0,Cyc_Tcutil_terr(_tmp62F,((_tmp904="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp904,sizeof(char),51))),_tag_dyneither(_tmp903,sizeof(void*),0)));}
# 2648
continue;}{
# 2650
struct Cyc_Absyn_Vardecl*_tmp652=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_stmttmp93=Cyc_Tcutil_compress(_tmp652->type);void*_tmp653=_stmttmp93;void*_tmp655;union Cyc_Absyn_Constraint*_tmp656;union Cyc_Absyn_Constraint*_tmp657;union Cyc_Absyn_Constraint*_tmp658;_LL3AC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp654=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp653;if(_tmp654->tag != 5)goto _LL3AE;else{_tmp655=(_tmp654->f1).elt_typ;_tmp656=((_tmp654->f1).ptr_atts).nullable;_tmp657=((_tmp654->f1).ptr_atts).bounds;_tmp658=((_tmp654->f1).ptr_atts).zero_term;}}_LL3AD:
# 2653
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp656)){
const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_Tcutil_terr(_tmp62F,((_tmp907="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp907,sizeof(char),55))),_tag_dyneither(_tmp906,sizeof(void*),0)));}
# 2656
if(!Cyc_Tcutil_is_bound_one(_tmp657)){
const char*_tmp90A;void*_tmp909;(_tmp909=0,Cyc_Tcutil_terr(_tmp62F,((_tmp90A="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp90A,sizeof(char),57))),_tag_dyneither(_tmp909,sizeof(void*),0)));}
# 2659
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp658)){
const char*_tmp90D;void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_terr(_tmp62F,((_tmp90D="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp90D,sizeof(char),77))),_tag_dyneither(_tmp90C,sizeof(void*),0)));}{
# 2662
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp910;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp90F;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65F=(_tmp90F=_region_malloc(_tmp62E,sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp910.tag=2,((_tmp910.f1=(int)j,((_tmp910.f2=_tmp655,_tmp910)))))),_tmp90F)));
struct Cyc_CfFlowInfo_Place*_tmp911;struct Cyc_CfFlowInfo_Place*_tmp660=(_tmp911=_region_malloc(_tmp62E,sizeof(*_tmp911)),((_tmp911->root=(void*)_tmp65F,((_tmp911->fields=0,_tmp911)))));
_tmp631=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp631,(void*)_tmp65F,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp655,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp917;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp916;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp914;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp913;_tmp631=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp631,(void*)((_tmp913=_region_malloc(_tmp62E,sizeof(*_tmp913)),((_tmp913[0]=((_tmp914.tag=0,((_tmp914.f1=_tmp652,_tmp914)))),_tmp913)))),(void*)((_tmp916=_region_malloc(_tmp62E,sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=5,((_tmp917.f1=_tmp660,_tmp917)))),_tmp916)))));}
{struct Cyc_List_List*_tmp918;param_roots=((_tmp918=_region_malloc(_tmp62E,sizeof(*_tmp918)),((_tmp918->hd=_tmp660,((_tmp918->tl=param_roots,_tmp918))))));}
goto _LL3AB;};_LL3AE:;_LL3AF: {
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_terr(_tmp62F,((_tmp91B="initializes attribute on non-pointer",_tag_dyneither(_tmp91B,sizeof(char),37))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}_LL3AB:;}
# 2670
goto _LL3A2;};}_LL3A7: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp63F=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp63A;if(_tmp63F->tag != 22)goto _LL3A9;else{_tmp640=_tmp63F->f1;}}_LL3A8: {
# 2672
unsigned int j=(unsigned int)_tmp640;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp91E;void*_tmp91D;(_tmp91D=0,Cyc_Tcutil_terr(_tmp62F,((_tmp91E="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp91E,sizeof(char),49))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}
continue;}{
# 2677
struct Cyc_Absyn_Vardecl*_tmp66D=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp66D->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp921;void*_tmp920;(_tmp920=0,Cyc_Tcutil_terr(_tmp62F,((_tmp921="noconsume attribute requires no-alias pointer",_tag_dyneither(_tmp921,sizeof(char),46))),_tag_dyneither(_tmp920,sizeof(void*),0)));}
continue;}{
# 2684
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp924;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp923;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp670=(_tmp923=_region_malloc(_tmp62E,sizeof(*_tmp923)),((_tmp923[0]=((_tmp924.tag=0,((_tmp924.f1=_tmp66D,_tmp924)))),_tmp923)));
struct Cyc_CfFlowInfo_Place*_tmp925;struct Cyc_CfFlowInfo_Place*_tmp671=(_tmp925=_region_malloc(_tmp62E,sizeof(*_tmp925)),((_tmp925->root=(void*)_tmp670,((_tmp925->fields=0,_tmp925)))));
{struct Cyc_List_List*_tmp926;noconsume_roots=((_tmp926=_region_malloc(_tmp62E,sizeof(*_tmp926)),((_tmp926->hd=_tmp671,((_tmp926->tl=noconsume_roots,_tmp926))))));}
goto _LL3A2;};};}_LL3A9:;_LL3AA:
 goto _LL3A2;_LL3A2:;}
# 2690
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp631,_tmp632);}_LL398:;}{
# 2693
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp62E,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp927;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp927=_region_malloc(_tmp62E,sizeof(*_tmp927)),((_tmp927->r=_tmp62E,((_tmp927->fenv=fenv,((_tmp927->iterate_again=1,((_tmp927->iteration_num=0,((_tmp927->in_try=0,((_tmp927->tryflow=inflow,((_tmp927->all_changed=0,((_tmp927->noreturn=noreturn,((_tmp927->unique_pat_vars=0,((_tmp927->param_roots=param_roots,((_tmp927->noconsume_params=noconsume_roots,((_tmp927->flow_table=flow_table,_tmp927)))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2703
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2706
union Cyc_CfFlowInfo_FlowInfo _tmp676=outflow;_LL3B1: if((_tmp676.BottomFL).tag != 1)goto _LL3B3;_LL3B2:
 goto _LL3B0;_LL3B3:;_LL3B4:
# 2709
 Cyc_NewControlFlow_check_init_params(_tmp62F,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp62F);
# 2713
if(noreturn){
const char*_tmp92A;void*_tmp929;(_tmp929=0,Cyc_Tcutil_terr(_tmp62F,((_tmp92A="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp92A,sizeof(char),46))),_tag_dyneither(_tmp929,sizeof(void*),0)));}else{
# 2716
void*_stmttmp94=Cyc_Tcutil_compress(fd->ret_type);void*_tmp679=_stmttmp94;_LL3B6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp67A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp679;if(_tmp67A->tag != 0)goto _LL3B8;}_LL3B7:
 goto _LL3B5;_LL3B8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp679;if(_tmp67B->tag != 7)goto _LL3BA;}_LL3B9:
 goto _LL3BB;_LL3BA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp679;if(_tmp67C->tag != 6)goto _LL3BC;}_LL3BB:
# 2720
{const char*_tmp92D;void*_tmp92C;(_tmp92C=0,Cyc_Tcutil_warn(_tmp62F,((_tmp92D="function may not return a value",_tag_dyneither(_tmp92D,sizeof(char),32))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}goto _LL3B5;_LL3BC:;_LL3BD:
# 2722
{const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_terr(_tmp62F,((_tmp930="function may not return a value",_tag_dyneither(_tmp930,sizeof(char),32))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}goto _LL3B5;_LL3B5:;}
# 2724
goto _LL3B0;_LL3B0:;};};};}
# 2728
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_stmttmp95=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp682=_stmttmp95;struct Cyc_Absyn_Fndecl*_tmp684;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp68A;_LL3BF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp683=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp682;if(_tmp683->tag != 1)goto _LL3C1;else{_tmp684=_tmp683->f1;}}_LL3C0:
 Cyc_NewControlFlow_check_fun(_tmp684);goto _LL3BE;_LL3C1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp685=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp682;if(_tmp685->tag != 12)goto _LL3C3;else{_tmp686=_tmp685->f1;}}_LL3C2:
 _tmp688=_tmp686;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp687=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp682;if(_tmp687->tag != 11)goto _LL3C5;else{_tmp688=_tmp687->f2;}}_LL3C4:
 _tmp68A=_tmp688;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp689=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp682;if(_tmp689->tag != 10)goto _LL3C7;else{_tmp68A=_tmp689->f2;}}_LL3C6:
 Cyc_NewControlFlow_cf_check(_tmp68A);goto _LL3BE;_LL3C7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp68B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp682;if(_tmp68B->tag != 13)goto _LL3C9;}_LL3C8:
 goto _LL3BE;_LL3C9:;_LL3CA:
 goto _LL3BE;_LL3BE:;}}
