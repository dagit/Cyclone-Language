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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp68E;struct Cyc_NewControlFlow_CFStmtAnnot _tmp68D;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp68C;enclosee->annot=(void*)((_tmp68C=_cycalloc(sizeof(*_tmp68C)),((_tmp68C[0]=((_tmp68E.tag=Cyc_NewControlFlow_CFAnnot,((_tmp68E.f1=((_tmp68D.encloser=encloser,((_tmp68D.visited=0,_tmp68D)))),_tmp68E)))),_tmp68C))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp694;const char*_tmp693;struct Cyc_Core_Impossible_exn_struct*_tmp692;(int)_throw((void*)((_tmp692=_cycalloc(sizeof(*_tmp692)),((_tmp692[0]=((_tmp694.tag=Cyc_Core_Impossible,((_tmp694.f1=((_tmp693="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp693,sizeof(char),37))),_tmp694)))),_tmp692)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp695;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp695=_region_malloc(env->r,sizeof(*_tmp695)),((_tmp695[0]=Cyc_CfFlowInfo_BottomFL(),_tmp695)));
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
struct _tuple19 _tmp696;return(_tmp696.f1=_tmpA,((_tmp696.f2=_tmpB,_tmp696)));};}
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
{const char*_tmp69A;void*_tmp699[1];struct Cyc_String_pa_PrintArg_struct _tmp698;(_tmp698.tag=0,((_tmp698.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp699[0]=& _tmp698,Cyc_Tcutil_warn(_tmpF,((_tmp69A="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp69A,sizeof(char),60))),_tag_dyneither(_tmp699,sizeof(void*),1)))))));}
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
struct _tuple18 _tmp69B;struct _tuple18 _stmttmp1=(_tmp69B.f1=flow,((_tmp69B.f2=_tmp1F,_tmp69B)));struct _tuple18 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp69C;struct _tuple20 _tmp23=(_tmp69C.f1=env,((_tmp69C.f2=s->loc,((_tmp69C.f3=_tmp22,_tmp69C)))));
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp69F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp69E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp69E=_region_malloc(fenv->r,sizeof(*_tmp69E)),((_tmp69E[0]=((_tmp69F.tag=0,((_tmp69F.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp69F)))),_tmp69E)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A2;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6A1;_tmp2A=(void*)((_tmp6A1=_region_malloc(fenv->r,sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=8,((_tmp6A2.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp6A2.f2=_tmp2A,_tmp6A2)))))),_tmp6A1))));}
# 259
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp6A3;if(Cyc_Relations_consistent_relations(((_tmp6A3=_region_malloc(r,sizeof(*_tmp6A3)),((_tmp6A3->hd=_tmp2F,((_tmp6A3->tl=assume,_tmp6A3))))))))
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
{const char*_tmp6A8;void*_tmp6A7[2];struct Cyc_String_pa_PrintArg_struct _tmp6A6;struct Cyc_String_pa_PrintArg_struct _tmp6A5;(_tmp6A5.tag=0,((_tmp6A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A6.tag=0,((_tmp6A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6A7[0]=& _tmp6A6,((_tmp6A7[1]=& _tmp6A5,Cyc_Tcutil_terr(loc,((_tmp6A8="unable to prove %s, required to access %s",_tag_dyneither(_tmp6A8,sizeof(char),42))),_tag_dyneither(_tmp6A7,sizeof(void*),2)))))))))))));}
{const char*_tmp6AB;void*_tmp6AA;(_tmp6AA=0,Cyc_fprintf(Cyc_stderr,((_tmp6AB="  [recorded facts on path: ",_tag_dyneither(_tmp6AB,sizeof(char),28))),_tag_dyneither(_tmp6AA,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6AE;void*_tmp6AD;(_tmp6AD=0,Cyc_fprintf(Cyc_stderr,((_tmp6AE="]\n",_tag_dyneither(_tmp6AE,sizeof(char),3))),_tag_dyneither(_tmp6AD,sizeof(void*),0)));};}}
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
{const char*_tmp6B3;void*_tmp6B2[2];struct Cyc_String_pa_PrintArg_struct _tmp6B1;struct Cyc_String_pa_PrintArg_struct _tmp6B0;(_tmp6B0.tag=0,((_tmp6B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6B1.tag=0,((_tmp6B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6B2[0]=& _tmp6B1,((_tmp6B2[1]=& _tmp6B0,Cyc_Tcutil_terr(loc,((_tmp6B3="unable to prove %s, required to access %s",_tag_dyneither(_tmp6B3,sizeof(char),42))),_tag_dyneither(_tmp6B2,sizeof(void*),2)))))))))))));}
{const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,Cyc_fprintf(Cyc_stderr,((_tmp6B6="  [recorded facts on path: ",_tag_dyneither(_tmp6B6,sizeof(char),28))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,Cyc_fprintf(Cyc_stderr,((_tmp6B9="]\n",_tag_dyneither(_tmp6B9,sizeof(char),3))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));};}
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
const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_Tcutil_terr(loc,((_tmp6BC="expression may not be fully initialized",_tag_dyneither(_tmp6BC,sizeof(char),40))),_tag_dyneither(_tmp6BB,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6BD;struct _tuple0 _stmttmp4=(_tmp6BD.f1=Cyc_Tcutil_compress(t),((_tmp6BD.f2=r,_tmp6BD)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
{const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_Tcutil_warn(loc,((_tmp6C0="argument may still contain unique pointers",_tag_dyneither(_tmp6C0,sizeof(char),43))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_Tcutil_terr(loc,((_tmp6C3="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6C3,sizeof(char),47))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}
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
const char*_tmp6C6;void*_tmp6C5;(_tmp6C5=0,Cyc_Tcutil_warn(loc,((_tmp6C6="argument may contain live unique pointers",_tag_dyneither(_tmp6C6,sizeof(char),42))),_tag_dyneither(_tmp6C5,sizeof(void*),0)));}
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
struct _tuple22 _tmp6C7;return(_tmp6C7.f1=inflow,((_tmp6C7.f2=0,_tmp6C7)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6CA;struct _tuple22 _tmp6C9;return(_tmp6C9.f1=_tmp8C,((_tmp6C9.f2=((_tmp6CA=_region_malloc(rgn,sizeof(*_tmp6CA)),((_tmp6CA->hd=_tmp8D,((_tmp6CA->tl=0,_tmp6CA)))))),_tmp6C9)));};}{
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
{struct Cyc_Dict_Dict*_tmp6CB;env->all_changed=((_tmp6CB=_region_malloc(env->r,sizeof(*_tmp6CB)),((_tmp6CB[0]=(env->fenv)->mt_place_set,_tmp6CB))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6CC;rvals=((_tmp6CC=_region_malloc(rgn,sizeof(*_tmp6CC)),((_tmp6CC->hd=_tmp93,((_tmp6CC->tl=0,_tmp6CC))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6CD;env->all_changed=((_tmp6CD=_region_malloc(env->r,sizeof(*_tmp6CD)),((_tmp6CD[0]=(env->fenv)->mt_place_set,_tmp6CD))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6CE;rvals=((_tmp6CE=_region_malloc(rgn,sizeof(*_tmp6CE)),((_tmp6CE->hd=_tmp98,((_tmp6CE->tl=rvals,_tmp6CE))))));}
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
struct Cyc_Dict_Dict*_tmp6CF;env->all_changed=((_tmp6CF=_region_malloc(env->r,sizeof(*_tmp6CF)),((_tmp6CF[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6CF))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp6D0;return(_tmp6D0.f1=outflow,((_tmp6D0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6D0)));};};}
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
const char*_tmp6D3;void*_tmp6D2;(_tmp6D2=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6D3="expression may not be initialized",_tag_dyneither(_tmp6D3,sizeof(char),34))),_tag_dyneither(_tmp6D2,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp6D4;struct _tuple15 _tmpA5=(_tmp6D4.f1=_tmp9E,((_tmp6D4.f2=(env->fenv)->unknown_all,_tmp6D4)));_npop_handler(0);return _tmpA5;};}
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
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D6;nzval=(void*)((_tmp6D6=_region_malloc((env->fenv)->r,sizeof(*_tmp6D6)),((_tmp6D6[0]=((_tmp6D7.tag=8,((_tmp6D7.f1=locname,((_tmp6D7.f2=nzval,_tmp6D7)))))),_tmp6D6))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 521
return _tmpAD;};}_LL60:;}_LL5B:;}
# 530
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 533
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp6D8;return(_tmp6D8.f1=outflow,((_tmp6D8.f2=outflow,_tmp6D8)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 536
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp6D9;return(_tmp6D9.f1=outflow,((_tmp6D9.f2=outflow,_tmp6D9)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6DC;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6DB;nzval=(void*)((_tmp6DB=_region_malloc((env->fenv)->r,sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=8,((_tmp6DC.f1=locname,((_tmp6DC.f2=nzval,_tmp6DC)))))),_tmp6DB))));}{
struct _tuple18 _tmp6DD;return
(_tmp6DD.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6DD.f2=
# 545
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,(env->fenv)->zero),_tmpB0),_tmp6DD)));};}_LL6A:;}_LL65:;}
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
struct _tuple15 _tmp6DE;return(_tmp6DE.f1=f,((_tmp6DE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6DE)));}_LL77: if((_tmpBF.ReachableFL).tag != 2)goto _LL74;_tmpC0=((struct _tuple14)(_tmpBF.ReachableFL).val).f1;_tmpC1=((struct _tuple14)(_tmpBF.ReachableFL).val).f2;_LL78:
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E1;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E0;e->annot=(void*)((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E1)))),_tmp6E0))));}
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E4;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E3;e->annot=(void*)((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3[0]=((_tmp6E4.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E4.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E4)))),_tmp6E3))));}
goto _LL97;_LL9A:;_LL9B:
# 605
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 608
goto _LL92;_LL92:;}{
# 610
struct _tuple15 _tmp6E5;return(_tmp6E5.f1=f,((_tmp6E5.f2=Cyc_CfFlowInfo_lookup_place(_tmpC0,_tmpCA),_tmp6E5)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpCB->tag != 0)goto _LL84;}_LL83:
# 612
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6E6;return(_tmp6E6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6E6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6E6)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpCC->tag != 3)goto _LL86;else{_tmpCD=_tmpCC->f1;}}_LL85:
# 615
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpCD,locname);goto _LL87;_LL86:;_LL87:
# 617
{void*_stmttmpC=e->r;void*_tmpDE=_stmttmpC;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 22)goto _LL9F;}_LL9E:
# 619
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E9;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E8;e->annot=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E9)))),_tmp6E8))));}
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
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EC;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EB;e->annot=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EC.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6EC)))),_tmp6EB))));}
goto _LLA1;_LLA1:;}{
# 637
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC0,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 639
{void*_tmpE7=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE7;if(_tmpE8->tag != 7)goto _LLAB;}_LLAA:
# 641
{const char*_tmp6EF;void*_tmp6EE;(_tmp6EE=0,Cyc_Tcutil_terr(e->loc,((_tmp6EF="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6EF,sizeof(char),61))),_tag_dyneither(_tmp6EE,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 644
const char*_tmp6F2;void*_tmp6F1;(_tmp6F1=0,Cyc_Tcutil_terr(e->loc,((_tmp6F2="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6F2,sizeof(char),46))),_tag_dyneither(_tmp6F1,sizeof(void*),0)));}_LLA8:;}
# 646
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 648
struct _tuple15 _tmp6F3;return(_tmp6F3.f1=f,((_tmp6F3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6F3)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 650
struct _tuple15 _tmp6F4;return(_tmp6F4.f1=f,((_tmp6F4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_none),_tmp6F4)));}}};_LL74:;}_LL72:;_LL73: {
# 653
struct Cyc_Core_Impossible_exn_struct _tmp6FA;const char*_tmp6F9;struct Cyc_Core_Impossible_exn_struct*_tmp6F8;(int)_throw((void*)((_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8[0]=((_tmp6FA.tag=Cyc_Core_Impossible,((_tmp6FA.f1=((_tmp6F9="right deref of non-pointer-type",_tag_dyneither(_tmp6F9,sizeof(char),32))),_tmp6FA)))),_tmp6F8)))));}_LL6F:;}
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
# 690
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,void*rval,unsigned int loc){
# 695
struct _tuple17 _stmttmp14=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF7;struct _tuple17 _tmpF6=_stmttmp14;_tmpF7=_tmpF6.f2;
{struct _tuple17 _tmp6FB;struct _tuple17 _stmttmp15=(_tmp6FB.f1=inflow,((_tmp6FB.f2=_tmpF7,_tmp6FB)));struct _tuple17 _tmpF8=_stmttmp15;struct Cyc_Dict_Dict _tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_CfFlowInfo_Place*_tmpFB;_LLB5: if(((_tmpF8.f1).ReachableFL).tag != 2)goto _LLB7;_tmpF9=((struct _tuple14)((_tmpF8.f1).ReachableFL).val).f1;_tmpFA=((struct _tuple14)((_tmpF8.f1).ReachableFL).val).f2;if(((_tmpF8.f2).PlaceL).tag != 1)goto _LLB7;_tmpFB=(struct Cyc_CfFlowInfo_Place*)((_tmpF8.f2).PlaceL).val;_LLB6:
# 698
 if(rval == 0)
rval=Cyc_CfFlowInfo_lookup_place(_tmpF9,_tmpFB);{
# 701
void*_tmpFC=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,rval);
# 706
_tmpF9=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpF9,env->all_changed,_tmpFB,_tmpFC);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpF9,_tmpFA);
# 711
goto _LLB4;};_LLB7: if(((_tmpF8.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 714
{const char*_tmp6FE;void*_tmp6FD;(_tmp6FD=0,Cyc_Tcutil_terr(loc,((_tmp6FE="noconsume() parameters must be l-values",_tag_dyneither(_tmp6FE,sizeof(char),40))),_tag_dyneither(_tmp6FD,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 717
return inflow;}
# 722
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 730
union Cyc_CfFlowInfo_FlowInfo _tmp100=outflow;struct Cyc_Dict_Dict _tmp101;struct Cyc_List_List*_tmp102;_LLBC: if((_tmp100.BottomFL).tag != 1)goto _LLBE;_LLBD: {
# 733
struct _tuple15 _tmp6FF;return(_tmp6FF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6FF.f2=rval,_tmp6FF)));}_LLBE: if((_tmp100.ReachableFL).tag != 2)goto _LLBB;_tmp101=((struct _tuple14)(_tmp100.ReachableFL).val).f1;_tmp102=((struct _tuple14)(_tmp100.ReachableFL).val).f2;_LLBF: {
# 735
union Cyc_CfFlowInfo_AbsLVal _tmp104=lval;struct Cyc_CfFlowInfo_Place*_tmp105;_LLC1: if((_tmp104.PlaceL).tag != 1)goto _LLC3;_tmp105=(struct Cyc_CfFlowInfo_Place*)(_tmp104.PlaceL).val;_LLC2: {
# 737
struct Cyc_Dict_Dict _tmp106=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp101,env->all_changed,_tmp105,rval);
_tmp102=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp106,_tmp102);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 743
void*_stmttmp16=Cyc_CfFlowInfo_lookup_place(_tmp101,_tmp105);void*_tmp107=_stmttmp16;_LLC6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp108=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp108->tag != 3)goto _LLC8;else{if(_tmp108->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;}}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp109=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp109->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10A->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10B->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10B->f2);if(_tmp10C->tag != 3)goto _LLCE;else{if(_tmp10C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCE;}}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10D->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 7)goto _LLD0;}}_LLCF:
 goto _LLD1;_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10F->tag != 8)goto _LLD2;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 0)goto _LLD2;}}_LLD1:
 goto _LLC5;_LLD2:;_LLD3:
# 751
{const char*_tmp702;void*_tmp701;(_tmp701=0,Cyc_Tcutil_warn(lexp->loc,((_tmp702="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp702,sizeof(char),43))),_tag_dyneither(_tmp701,sizeof(void*),0)));}
goto _LLC5;_LLC5:;}
# 756
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 758
struct _tuple15 _tmp703;return(_tmp703.f1=outflow,((_tmp703.f2=rval,_tmp703)));};}_LLC3: if((_tmp104.UnknownL).tag != 2)goto _LLC0;_LLC4: {
# 761
struct _tuple15 _tmp704;return(_tmp704.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp704.f2=rval,_tmp704)));}_LLC0:;}_LLBB:;}
# 768
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 774
union Cyc_CfFlowInfo_FlowInfo _tmp115=f;struct Cyc_Dict_Dict _tmp116;struct Cyc_List_List*_tmp117;_LLD5: if((_tmp115.BottomFL).tag != 1)goto _LLD7;_LLD6:
 return Cyc_CfFlowInfo_BottomFL();_LLD7: if((_tmp115.ReachableFL).tag != 2)goto _LLD4;_tmp116=((struct _tuple14)(_tmp115.ReachableFL).val).f1;_tmp117=((struct _tuple14)(_tmp115.ReachableFL).val).f2;_LLD8:
# 777
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp70A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp709;struct Cyc_CfFlowInfo_Place*_tmp708;_tmp116=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp116,env->all_changed,(
(_tmp708=_region_malloc(env->r,sizeof(*_tmp708)),((_tmp708->root=(void*)((_tmp70A=_region_malloc(env->r,sizeof(*_tmp70A)),((_tmp70A[0]=((_tmp709.tag=0,((_tmp709.f1=vd,_tmp709)))),_tmp70A)))),((_tmp708->fields=0,_tmp708)))))),rval);}
# 780
_tmp117=Cyc_Relations_reln_assign_var(env->r,_tmp117,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11B=Cyc_CfFlowInfo_ReachableFL(_tmp116,_tmp117);
Cyc_NewControlFlow_update_tryflow(env,_tmp11B);
# 787
return _tmp11B;};_LLD4:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 791
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 798
if(vds == 0)return inflow;{
# 801
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp70B;es=((_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp70B->tl=es,_tmp70B))))));}}}
# 807
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11C,fenv->unknown_all,pat_loc,name_locs);
# 809
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 812
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp11F;void*_tmp120;struct _tuple15 _tmp11E=_stmttmp17;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp11F,_tmp120);}}{
# 820
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp121 != 0;_tmp121=_tmp121->tl){
struct _tuple23*_stmttmp18=(struct _tuple23*)_tmp121->hd;struct Cyc_Absyn_Vardecl**_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct _tuple23*_tmp122=_stmttmp18;_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;
if(_tmp123 != 0  && _tmp124 != 0){
# 827
if(_tmp124->topt == 0){
const char*_tmp70F;void*_tmp70E[1];struct Cyc_String_pa_PrintArg_struct _tmp70D;(_tmp70D.tag=0,((_tmp70D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp124)),((_tmp70E[0]=& _tmp70D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp70F="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp70F,sizeof(char),41))),_tag_dyneither(_tmp70E,sizeof(void*),1)))))));}
# 832
Cyc_fflush(Cyc_stderr);{
struct _tuple15 _stmttmp19=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp124);union Cyc_CfFlowInfo_FlowInfo _tmp129;void*_tmp12A;struct _tuple15 _tmp128=_stmttmp19;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp712;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp711;_tmp12A=(void*)((_tmp711=_region_malloc(fenv->r,sizeof(*_tmp711)),((_tmp711[0]=((_tmp712.tag=8,((_tmp712.f1=*_tmp123,((_tmp712.f2=_tmp12A,_tmp712)))))),_tmp711))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp129,*_tmp123,_tmp124,_tmp12A,pat_loc);};}}
# 840
return inflow;};};}
# 843
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 845
void*_stmttmp1A=e->r;void*_tmp12D=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp12E->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp12E->f2);if(_tmp12F->tag != 4)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp130->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp131=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp130->f2);if(_tmp131->tag != 3)goto _LLDE;}}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp132->tag != 1)goto _LLE0;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp133=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp132->f2);if(_tmp133->tag != 5)goto _LLE0;}}_LLDF:
 return 1;_LLE0: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp134->tag != 19)goto _LLE2;else{_tmp135=_tmp134->f1;}}_LLE1:
 _tmp137=_tmp135;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp136->tag != 20)goto _LLE4;else{_tmp137=_tmp136->f1;}}_LLE3:
 _tmp139=_tmp137;goto _LLE5;_LLE4: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp138->tag != 21)goto _LLE6;else{_tmp139=_tmp138->f1;}}_LLE5:
# 852
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp139);_LLE6: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp13A->tag != 22)goto _LLE8;else{_tmp13B=_tmp13A->f1;}}_LLE7: {
# 854
void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(_tmp13B->topt));void*_tmp13E=_stmttmp1B;_LLED: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13E;if(_tmp13F->tag != 10)goto _LLEF;}_LLEE:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13B);_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLE8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12D;if(_tmp13C->tag != 13)goto _LLEA;else{_tmp13D=_tmp13C->f2;}}_LLE9:
# 858
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13D);_LLEA:;_LLEB:
 return 0;_LLD9:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7BF(unsigned int*_tmp7BE,unsigned int*_tmp7BD,void***_tmp7BB,struct Cyc_List_List**_tmp31A){for(*_tmp7BE=0;*_tmp7BE < *_tmp7BD;(*_tmp7BE)++){
# 1432 "new_control_flow.cyc"
void*_tmp7B9;(*_tmp7BB)[*_tmp7BE]=(void*)((_tmp7B9=(void*)((struct Cyc_List_List*)_check_null(*_tmp31A))->hd,((*_tmp31A=(*_tmp31A)->tl,_tmp7B9))));}}
# 864 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 868
struct Cyc_CfFlowInfo_FlowEnv*_tmp140=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 881
{union Cyc_CfFlowInfo_FlowInfo _tmp141=inflow;struct Cyc_Dict_Dict _tmp142;struct Cyc_List_List*_tmp143;_LLF2: if((_tmp141.BottomFL).tag != 1)goto _LLF4;_LLF3: {
struct _tuple15 _tmp713;return(_tmp713.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp713.f2=_tmp140->unknown_all,_tmp713)));}_LLF4: if((_tmp141.ReachableFL).tag != 2)goto _LLF1;_tmp142=((struct _tuple14)(_tmp141.ReachableFL).val).f1;_tmp143=((struct _tuple14)(_tmp141.ReachableFL).val).f2;_LLF5:
 d=_tmp142;relns=_tmp143;_LLF1:;}
# 896 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 917 "new_control_flow.cyc"
struct _tuple17 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp146;union Cyc_CfFlowInfo_AbsLVal _tmp147;struct _tuple17 _tmp145=_stmttmp1C;_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;{
struct _tuple17 _tmp714;struct _tuple17 _stmttmp1D=(_tmp714.f1=_tmp146,((_tmp714.f2=_tmp147,_tmp714)));struct _tuple17 _tmp148=_stmttmp1D;struct Cyc_Dict_Dict _tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_CfFlowInfo_Place*_tmp14B;_LLF7: if(((_tmp148.f1).ReachableFL).tag != 2)goto _LLF9;_tmp149=((struct _tuple14)((_tmp148.f1).ReachableFL).val).f1;_tmp14A=((struct _tuple14)((_tmp148.f1).ReachableFL).val).f2;if(((_tmp148.f2).PlaceL).tag != 1)goto _LLF9;_tmp14B=(struct Cyc_CfFlowInfo_Place*)((_tmp148.f2).PlaceL).val;_LLF8: {
# 920
void*_tmp14C=Cyc_CfFlowInfo_lookup_place(_tmp149,_tmp14B);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp14C,& needs_unconsume)){
{const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_terr(e->loc,((_tmp717="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp717,sizeof(char),71))),_tag_dyneither(_tmp716,sizeof(void*),0)));}{
struct _tuple15 _tmp718;return(_tmp718.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp718.f2=_tmp140->unknown_all,_tmp718)));};}else{
# 926
if(needs_unconsume){
# 928
struct _tuple15 _tmp719;return(_tmp719.f1=_tmp146,((_tmp719.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp140,_tmp14C),_tmp719)));}else{
# 931
void*_tmp151=Cyc_CfFlowInfo_make_unique_consumed(_tmp140,(void*)_check_null(e->topt),e,env->iteration_num,_tmp14C);
struct Cyc_Dict_Dict _tmp152=Cyc_CfFlowInfo_assign_place(_tmp140,e->loc,_tmp149,env->all_changed,_tmp14B,_tmp151);
# 942
struct _tuple15 _tmp71A;return(_tmp71A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp152,_tmp14A),((_tmp71A.f2=_tmp14C,_tmp71A)));}}}_LLF9:;_LLFA:
# 944
 goto _LLF6;_LLF6:;};}{
# 947
void*_stmttmp1E=e->r;void*_tmp155=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp171;struct Cyc_Absyn_Vardecl*_tmp174;enum Cyc_Absyn_Primop _tmp176;struct Cyc_List_List*_tmp177;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_List_List*_tmp187;int _tmp189;struct Cyc_Absyn_Exp*_tmp18A;void**_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;int _tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct _dyneither_ptr*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct _dyneither_ptr*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct _dyneither_ptr*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Datatypedecl*_tmp1B0;struct Cyc_List_List*_tmp1B2;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B6;enum Cyc_Absyn_AggrKind _tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_List_List*_tmp1BB;struct Cyc_Absyn_Vardecl*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;int _tmp1C0;struct Cyc_Absyn_Exp*_tmp1C2;void*_tmp1C3;int _tmp1C4;struct Cyc_Absyn_Stmt*_tmp1C6;_LLFC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp156->tag != 13)goto _LLFE;else{_tmp157=_tmp156->f2;if(_tmp156->f4 != Cyc_Absyn_NonNull_to_Null)goto _LLFE;}}_LLFD: {
# 951
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp157);union Cyc_CfFlowInfo_FlowInfo _tmp1CE;void*_tmp1CF;struct _tuple15 _tmp1CD=_stmttmp4F;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;{
struct _tuple15 _stmttmp50=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1CE,_tmp157,_tmp1CF);union Cyc_CfFlowInfo_FlowInfo _tmp1D1;void*_tmp1D2;struct _tuple15 _tmp1D0=_stmttmp50;_tmp1D1=_tmp1D0.f1;_tmp1D2=_tmp1D0.f2;{
struct _tuple15 _tmp71B;return(_tmp71B.f1=_tmp1D1,((_tmp71B.f2=_tmp1CF,_tmp71B)));};};}_LLFE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp158->tag != 13)goto _LL100;else{_tmp159=_tmp158->f2;}}_LLFF:
# 957
 _tmp15B=_tmp159;goto _LL101;_LL100: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15A->tag != 11)goto _LL102;else{_tmp15B=_tmp15A->f1;}}_LL101:
 _tmp15D=_tmp15B;goto _LL103;_LL102: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15C->tag != 12)goto _LL104;else{_tmp15D=_tmp15C->f1;}}_LL103:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp15D);_LL104: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15E->tag != 0)goto _LL106;else{if(((_tmp15E->f1).Null_c).tag != 1)goto _LL106;}}_LL105:
# 961
 goto _LL107;_LL106: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15F->tag != 0)goto _LL108;else{if(((_tmp15F->f1).Int_c).tag != 5)goto _LL108;if(((struct _tuple7)((_tmp15F->f1).Int_c).val).f2 != 0)goto _LL108;}}_LL107: {
struct _tuple15 _tmp71C;return(_tmp71C.f1=inflow,((_tmp71C.f2=_tmp140->zero,_tmp71C)));}_LL108: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp160->tag != 0)goto _LL10A;else{if(((_tmp160->f1).Int_c).tag != 5)goto _LL10A;}}_LL109:
 goto _LL10B;_LL10A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp161->tag != 1)goto _LL10C;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp162=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp161->f2);if(_tmp162->tag != 2)goto _LL10C;}}_LL10B: {
struct _tuple15 _tmp71D;return(_tmp71D.f1=inflow,((_tmp71D.f2=_tmp140->notzeroall,_tmp71D)));}_LL10C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp163->tag != 30)goto _LL10E;else{if(_tmp163->f1 != 0)goto _LL10E;}}_LL10D:
# 966
 goto _LL10F;_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp164->tag != 0)goto _LL110;}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp165->tag != 17)goto _LL112;}_LL111:
 goto _LL113;_LL112: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp166->tag != 16)goto _LL114;}_LL113:
 goto _LL115;_LL114: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp167->tag != 18)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp168->tag != 32)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp169->tag != 31)goto _LL11A;}_LL119: {
struct _tuple15 _tmp71E;return(_tmp71E.f1=inflow,((_tmp71E.f2=_tmp140->unknown_all,_tmp71E)));}_LL11A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp16A->tag != 1)goto _LL11C;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp16B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp16A->f2);if(_tmp16B->tag != 1)goto _LL11C;}}_LL11B: {
# 975
struct _tuple15 _tmp71F;return(_tmp71F.f1=inflow,((_tmp71F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp71F)));}_LL11C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp16C->tag != 1)goto _LL11E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16C->f2);if(_tmp16D->tag != 3)goto _LL11E;else{_tmp16E=_tmp16D->f1;}}}_LL11D:
# 978
 _tmp171=_tmp16E;goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp16F->tag != 1)goto _LL120;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp170=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp16F->f2);if(_tmp170->tag != 4)goto _LL120;else{_tmp171=_tmp170->f1;}}}_LL11F:
 _tmp174=_tmp171;goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp172->tag != 1)goto _LL122;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp173=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp172->f2);if(_tmp173->tag != 5)goto _LL122;else{_tmp174=_tmp173->f1;}}}_LL121: {
# 982
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp720;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp720.tag=0,((_tmp720.f1=_tmp174,_tmp720)));
struct _tuple15 _tmp721;return(_tmp721.f1=inflow,((_tmp721.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp721)));}_LL122: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp175->tag != 2)goto _LL124;else{_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;}}_LL123: {
# 989
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp177);union Cyc_CfFlowInfo_FlowInfo _tmp1DB;void*_tmp1DC;struct _tuple15 _tmp1DA=_stmttmp4E;_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;{
struct _tuple15 _tmp722;return(_tmp722.f1=_tmp1DB,((_tmp722.f2=_tmp1DC,_tmp722)));};}_LL124: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp178->tag != 4)goto _LL126;else{_tmp179=_tmp178->f1;}}_LL125: {
# 993
struct Cyc_List_List _tmp723;struct Cyc_List_List _tmp1DE=(_tmp723.hd=_tmp179,((_tmp723.tl=0,_tmp723)));
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1DE);union Cyc_CfFlowInfo_FlowInfo _tmp1E0;struct _tuple15 _tmp1DF=_stmttmp4B;_tmp1E0=_tmp1DF.f1;{
struct _tuple17 _stmttmp4C=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E0,0,0,_tmp179);union Cyc_CfFlowInfo_AbsLVal _tmp1E2;struct _tuple17 _tmp1E1=_stmttmp4C;_tmp1E2=_tmp1E1.f2;
{struct _tuple24 _tmp724;struct _tuple24 _stmttmp4D=(_tmp724.f1=_tmp1E2,((_tmp724.f2=_tmp1E0,_tmp724)));struct _tuple24 _tmp1E3=_stmttmp4D;struct Cyc_CfFlowInfo_Place*_tmp1E4;struct Cyc_Dict_Dict _tmp1E5;struct Cyc_List_List*_tmp1E6;_LL165: if(((_tmp1E3.f1).PlaceL).tag != 1)goto _LL167;_tmp1E4=(struct Cyc_CfFlowInfo_Place*)((_tmp1E3.f1).PlaceL).val;if(((_tmp1E3.f2).ReachableFL).tag != 2)goto _LL167;_tmp1E5=((struct _tuple14)((_tmp1E3.f2).ReachableFL).val).f1;_tmp1E6=((struct _tuple14)((_tmp1E3.f2).ReachableFL).val).f2;_LL166:
# 998
 _tmp1E6=Cyc_Relations_reln_kill_exp(_tmp140->r,_tmp1E6,_tmp179);
_tmp1E0=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp140,_tmp179->loc,_tmp1E5,env->all_changed,_tmp1E4,_tmp140->unknown_all),_tmp1E6);
# 1002
goto _LL164;_LL167:;_LL168:
 goto _LL164;_LL164:;}{
# 1005
struct _tuple15 _tmp725;return(_tmp725.f1=_tmp1E0,((_tmp725.f2=_tmp140->unknown_all,_tmp725)));};};}_LL126: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp17A->tag != 3)goto _LL128;else{_tmp17B=_tmp17A->f1;if(_tmp17A->f2 == 0)goto _LL128;_tmp17C=_tmp17A->f3;}}_LL127:
# 1008
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp728;void*_tmp727;(_tmp727=0,Cyc_Tcutil_terr(e->loc,((_tmp728="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp728,sizeof(char),58))),_tag_dyneither(_tmp727,sizeof(void*),0)));}{
struct _tuple15 _tmp729;return(_tmp729.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp729.f2=_tmp140->unknown_all,_tmp729)));};}{
# 1012
struct Cyc_List_List _tmp72A;struct Cyc_List_List _tmp1ED=(_tmp72A.hd=_tmp17C,((_tmp72A.tl=0,_tmp72A)));
struct Cyc_List_List _tmp72B;struct Cyc_List_List _tmp1EE=(_tmp72B.hd=_tmp17B,((_tmp72B.tl=& _tmp1ED,_tmp72B)));
struct _tuple15 _stmttmp49=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1EE);union Cyc_CfFlowInfo_FlowInfo _tmp1F0;struct _tuple15 _tmp1EF=_stmttmp49;_tmp1F0=_tmp1EF.f1;{
struct _tuple17 _stmttmp4A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F0,0,0,_tmp17B);union Cyc_CfFlowInfo_AbsLVal _tmp1F2;struct _tuple17 _tmp1F1=_stmttmp4A;_tmp1F2=_tmp1F1.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1F3=_tmp1F0;struct Cyc_Dict_Dict _tmp1F4;struct Cyc_List_List*_tmp1F5;_LL16A: if((_tmp1F3.ReachableFL).tag != 2)goto _LL16C;_tmp1F4=((struct _tuple14)(_tmp1F3.ReachableFL).val).f1;_tmp1F5=((struct _tuple14)(_tmp1F3.ReachableFL).val).f2;_LL16B:
# 1018
{union Cyc_CfFlowInfo_AbsLVal _tmp1F6=_tmp1F2;struct Cyc_CfFlowInfo_Place*_tmp1F7;_LL16F: if((_tmp1F6.PlaceL).tag != 1)goto _LL171;_tmp1F7=(struct Cyc_CfFlowInfo_Place*)(_tmp1F6.PlaceL).val;_LL170:
# 1022
 _tmp1F5=Cyc_Relations_reln_kill_exp(_tmp140->r,_tmp1F5,_tmp17B);
_tmp1F4=Cyc_CfFlowInfo_assign_place(_tmp140,_tmp17B->loc,_tmp1F4,env->all_changed,_tmp1F7,_tmp140->unknown_all);
# 1025
_tmp1F0=Cyc_CfFlowInfo_ReachableFL(_tmp1F4,_tmp1F5);
# 1028
goto _LL16E;_LL171: if((_tmp1F6.UnknownL).tag != 2)goto _LL16E;_LL172:
# 1031
 goto _LL16E;_LL16E:;}
# 1033
goto _LL169;_LL16C:;_LL16D:
 goto _LL169;_LL169:;}{
# 1036
struct _tuple15 _tmp72C;return(_tmp72C.f1=_tmp1F0,((_tmp72C.f2=_tmp140->unknown_all,_tmp72C)));};};};_LL128: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp17D->tag != 3)goto _LL12A;else{_tmp17E=_tmp17D->f1;if(_tmp17D->f2 != 0)goto _LL12A;_tmp17F=_tmp17D->f3;}}_LL129:
# 1040
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72F;void*_tmp72E;(_tmp72E=0,Cyc_Tcutil_terr(e->loc,((_tmp72F="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72F,sizeof(char),58))),_tag_dyneither(_tmp72E,sizeof(void*),0)));}{
struct _tuple15 _tmp730;return(_tmp730.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp730.f2=_tmp140->unknown_all,_tmp730)));};}{
# 1044
struct Cyc_Dict_Dict*_tmp1FE=env->all_changed;
# 1046
{struct Cyc_Absyn_Exp*_tmp731[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp731[1]=_tmp17F,((_tmp731[0]=_tmp17E,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp731,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp732;env->all_changed=((_tmp732=_region_malloc(env->r,sizeof(*_tmp732)),((_tmp732[0]=_tmp140->mt_place_set,_tmp732))));}{
struct _tuple17 _stmttmp47=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp17E);union Cyc_CfFlowInfo_FlowInfo _tmp202;union Cyc_CfFlowInfo_AbsLVal _tmp203;struct _tuple17 _tmp201=_stmttmp47;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;{
struct Cyc_Dict_Dict _tmp204=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp733;env->all_changed=((_tmp733=_region_malloc(env->r,sizeof(*_tmp733)),((_tmp733[0]=_tmp140->mt_place_set,_tmp733))));}{
struct _tuple15 _stmttmp48=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17F);union Cyc_CfFlowInfo_FlowInfo _tmp207;void*_tmp208;struct _tuple15 _tmp206=_stmttmp48;_tmp207=_tmp206.f1;_tmp208=_tmp206.f2;{
struct Cyc_Dict_Dict _tmp209=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp20A=Cyc_CfFlowInfo_after_flow(_tmp140,& _tmp204,_tmp202,_tmp207,_tmp204,_tmp209);
# 1057
union Cyc_CfFlowInfo_FlowInfo _tmp20B=Cyc_CfFlowInfo_join_flow(_tmp140,_tmp1FE,inflow,_tmp20A);
# 1060
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20B,inflow)){
if(_tmp1FE == 0)
env->all_changed=0;else{
# 1064
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1FE,
Cyc_CfFlowInfo_union_place_set(_tmp204,_tmp209,0),0);}
# 1067
return Cyc_NewControlFlow_do_assign(_tmp140,env,_tmp20A,_tmp17E,_tmp203,_tmp17F,_tmp208,e->loc);}
# 1070
inflow=_tmp20B;};};};};}};_LL12A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp180->tag != 8)goto _LL12C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}_LL12B: {
# 1075
struct _tuple15 _stmttmp46=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp181);union Cyc_CfFlowInfo_FlowInfo _tmp20D;void*_tmp20E;struct _tuple15 _tmp20C=_stmttmp46;_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp20D,_tmp182);}_LL12C: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp183->tag != 10)goto _LL12E;else{_tmp184=_tmp183->f1;}}_LL12D: {
# 1079
struct _tuple15 _stmttmp45=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp184);union Cyc_CfFlowInfo_FlowInfo _tmp210;void*_tmp211;struct _tuple15 _tmp20F=_stmttmp45;_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp184->loc,_tmp210,_tmp211);{
struct _tuple15 _tmp734;return(_tmp734.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp734.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp734)));};}_LL12E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp185->tag != 9)goto _LL130;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LL12F: {
# 1084
struct _RegionHandle _tmp213=_new_region("temp");struct _RegionHandle*temp=& _tmp213;_push_region(temp);
{struct Cyc_List_List*_tmp735;struct _tuple22 _stmttmp3F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp735=_region_malloc(temp,sizeof(*_tmp735)),((_tmp735->hd=_tmp186,((_tmp735->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp187),_tmp735)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp215;struct Cyc_List_List*_tmp216;struct _tuple22 _tmp214=_stmttmp3F;_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;{
# 1087
union Cyc_CfFlowInfo_FlowInfo _tmp217=Cyc_NewControlFlow_use_Rval(env,_tmp186->loc,_tmp215,(void*)((struct Cyc_List_List*)_check_null(_tmp216))->hd);
_tmp216=_tmp216->tl;{
# 1090
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp186->topt));void*_tmp218=_stmttmp40;void*_tmp21A;_LL174: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 5)goto _LL176;else{_tmp21A=(_tmp219->f1).elt_typ;}}_LL175:
# 1095
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp21A);void*_tmp21B=_stmttmp41;struct Cyc_List_List*_tmp21D;_LL179: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21B;if(_tmp21C->tag != 9)goto _LL17B;else{_tmp21D=(_tmp21C->f1).attributes;}}_LL17A:
# 1097
 for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){
# 1099
void*_stmttmp42=(void*)_tmp21D->hd;void*_tmp21E=_stmttmp42;int _tmp220;int _tmp222;int _tmp224;_LL17E: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp21F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp21E;if(_tmp21F->tag != 20)goto _LL180;else{_tmp220=_tmp21F->f1;}}_LL17F:
# 1101
{struct Cyc_List_List*_tmp736;init_params=((_tmp736=_region_malloc(temp,sizeof(*_tmp736)),((_tmp736->hd=(void*)_tmp220,((_tmp736->tl=init_params,_tmp736))))));}goto _LL17D;_LL180: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp221=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp21E;if(_tmp221->tag != 21)goto _LL182;else{_tmp222=_tmp221->f1;}}_LL181:
# 1103
{struct Cyc_List_List*_tmp737;nolive_unique_params=((_tmp737=_region_malloc(temp,sizeof(*_tmp737)),((_tmp737->hd=(void*)_tmp222,((_tmp737->tl=nolive_unique_params,_tmp737))))));}
goto _LL17D;_LL182: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp223=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp21E;if(_tmp223->tag != 22)goto _LL184;else{_tmp224=_tmp223->f1;}}_LL183:
# 1107
{struct Cyc_List_List*_tmp738;noconsume_params=((_tmp738=_region_malloc(temp,sizeof(*_tmp738)),((_tmp738->hd=(void*)_tmp224,((_tmp738->tl=noconsume_params,_tmp738))))));}
goto _LL17D;_LL184:;_LL185:
 goto _LL17D;_LL17D:;}
# 1111
goto _LL178;_LL17B:;_LL17C: {
const char*_tmp73B;void*_tmp73A;(_tmp73A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp73B="anal_Rexp: bad function type",_tag_dyneither(_tmp73B,sizeof(char),29))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}_LL178:;}
# 1114
goto _LL173;_LL176:;_LL177: {
const char*_tmp73E;void*_tmp73D;(_tmp73D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp73E="anal_Rexp: bad function type",_tag_dyneither(_tmp73E,sizeof(char),29))),_tag_dyneither(_tmp73D,sizeof(void*),0)));}_LL173:;}
# 1117
{int i=1;for(0;_tmp216 != 0;(((_tmp216=_tmp216->tl,_tmp187=((struct Cyc_List_List*)_check_null(_tmp187))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp22C=_tmp215;struct Cyc_Dict_Dict _tmp22D;_LL187: if((_tmp22C.BottomFL).tag != 1)goto _LL189;_LL188:
 goto _LL186;_LL189: if((_tmp22C.ReachableFL).tag != 2)goto _LL186;_tmp22D=((struct _tuple14)(_tmp22C.ReachableFL).val).f1;_LL18A:
# 1122
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp22D,(void*)_tmp216->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp741;void*_tmp740;(_tmp740=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->loc,((_tmp741="expression may not be initialized",_tag_dyneither(_tmp741,sizeof(char),34))),_tag_dyneither(_tmp740,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp230=_tmp217;struct Cyc_Dict_Dict _tmp231;struct Cyc_List_List*_tmp232;_LL18C: if((_tmp230.BottomFL).tag != 1)goto _LL18E;_LL18D:
 goto _LL18B;_LL18E: if((_tmp230.ReachableFL).tag != 2)goto _LL18B;_tmp231=((struct _tuple14)(_tmp230.ReachableFL).val).f1;_tmp232=((struct _tuple14)(_tmp230.ReachableFL).val).f2;_LL18F: {
# 1129
struct Cyc_Dict_Dict _tmp233=Cyc_CfFlowInfo_escape_deref(_tmp140,_tmp231,env->all_changed,(void*)_tmp216->hd);
{void*_stmttmp43=(void*)_tmp216->hd;void*_tmp234=_stmttmp43;struct Cyc_CfFlowInfo_Place*_tmp236;_LL191: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp235=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp234;if(_tmp235->tag != 5)goto _LL193;else{_tmp236=_tmp235->f1;}}_LL192:
# 1132
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->topt));void*_tmp237=_stmttmp44;void*_tmp239;_LL196: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp237;if(_tmp238->tag != 5)goto _LL198;else{_tmp239=(_tmp238->f1).elt_typ;}}_LL197:
# 1136
 _tmp233=Cyc_CfFlowInfo_assign_place(_tmp140,((struct Cyc_Absyn_Exp*)_tmp187->hd)->loc,_tmp233,env->all_changed,_tmp236,
# 1138
Cyc_CfFlowInfo_typ_to_absrval(_tmp140,_tmp239,0,_tmp140->esc_all));
# 1140
goto _LL195;_LL198:;_LL199: {
const char*_tmp744;void*_tmp743;(_tmp743=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp744="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp744,sizeof(char),39))),_tag_dyneither(_tmp743,sizeof(void*),0)));}_LL195:;}
# 1143
goto _LL190;_LL193:;_LL194:
 goto _LL190;_LL190:;}
# 1146
_tmp217=Cyc_CfFlowInfo_ReachableFL(_tmp233,_tmp232);
goto _LL18B;}_LL18B:;}
# 1149
goto _LL186;_LL186:;}else{
# 1152
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1157
_tmp217=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->topt),_tmp217,(void*)_tmp216->hd);else{
# 1164
_tmp217=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->loc,_tmp217,(void*)_tmp216->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i))
_tmp217=Cyc_NewControlFlow_unconsume_exp(env,_tmp217,(struct Cyc_Absyn_Exp*)_tmp187->hd,(void*)_tmp216->hd,((struct Cyc_Absyn_Exp*)_tmp187->hd)->loc);}}}}
# 1170
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp186->topt))){
struct _tuple15 _tmp745;struct _tuple15 _tmp23D=(_tmp745.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp745.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp745)));_npop_handler(0);return _tmp23D;}else{
# 1173
struct _tuple15 _tmp746;struct _tuple15 _tmp23F=(_tmp746.f1=_tmp217,((_tmp746.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp746)));_npop_handler(0);return _tmp23F;}};};}
# 1085
;_pop_region(temp);}_LL130: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp188->tag != 33)goto _LL132;else{_tmp189=(_tmp188->f1).is_calloc;_tmp18A=(_tmp188->f1).rgn;_tmp18B=(_tmp188->f1).elt_type;_tmp18C=(_tmp188->f1).num_elts;_tmp18D=(_tmp188->f1).fat_result;}}_LL131: {
# 1176
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp749;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp748;void*root=(void*)((_tmp748=_region_malloc(_tmp140->r,sizeof(*_tmp748)),((_tmp748[0]=((_tmp749.tag=1,((_tmp749.f1=_tmp18C,((_tmp749.f2=(void*)_check_null(e->topt),_tmp749)))))),_tmp748))));
struct Cyc_CfFlowInfo_Place*_tmp74A;struct Cyc_CfFlowInfo_Place*place=(_tmp74A=_region_malloc(_tmp140->r,sizeof(*_tmp74A)),((_tmp74A->root=root,((_tmp74A->fields=0,_tmp74A)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp74D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp74C;void*rval=(void*)((_tmp74C=_region_malloc(_tmp140->r,sizeof(*_tmp74C)),((_tmp74C[0]=((_tmp74D.tag=5,((_tmp74D.f1=place,_tmp74D)))),_tmp74C))));
void*place_val;
if(_tmp18D)place_val=_tmp140->notzeroall;else{
if(_tmp189)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,*((void**)_check_null(_tmp18B)),0,_tmp140->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,*((void**)_check_null(_tmp18B)),0,_tmp140->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp18A != 0){
struct _RegionHandle _tmp241=_new_region("temp");struct _RegionHandle*temp=& _tmp241;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp74E[2];struct _tuple22 _stmttmp3D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp74E[1]=_tmp18C,((_tmp74E[0]=_tmp18A,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp74E,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1187
union Cyc_CfFlowInfo_FlowInfo _tmp243;struct Cyc_List_List*_tmp244;struct _tuple22 _tmp242=_stmttmp3D;_tmp243=_tmp242.f1;_tmp244=_tmp242.f2;
# 1189
outflow=_tmp243;}
# 1187
;_pop_region(temp);}else{
# 1192
struct _tuple15 _stmttmp3E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18C);union Cyc_CfFlowInfo_FlowInfo _tmp247;struct _tuple15 _tmp246=_stmttmp3E;_tmp247=_tmp246.f1;
outflow=_tmp247;}{
# 1195
union Cyc_CfFlowInfo_FlowInfo _tmp248=outflow;struct Cyc_Dict_Dict _tmp249;struct Cyc_List_List*_tmp24A;_LL19B: if((_tmp248.BottomFL).tag != 1)goto _LL19D;_LL19C: {
struct _tuple15 _tmp74F;return(_tmp74F.f1=outflow,((_tmp74F.f2=rval,_tmp74F)));}_LL19D: if((_tmp248.ReachableFL).tag != 2)goto _LL19A;_tmp249=((struct _tuple14)(_tmp248.ReachableFL).val).f1;_tmp24A=((struct _tuple14)(_tmp248.ReachableFL).val).f2;_LL19E: {
# 1198
struct _tuple15 _tmp750;return(_tmp750.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp249,root,place_val),_tmp24A),((_tmp750.f2=rval,_tmp750)));}_LL19A:;};};}_LL132: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp18E->tag != 34)goto _LL134;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LL133: {
# 1202
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1208
struct _RegionHandle _tmp252=_new_region("temp");struct _RegionHandle*temp=& _tmp252;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp751[2];struct _tuple22 _stmttmp3A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp751[1]=_tmp190,((_tmp751[0]=_tmp18F,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp751,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1209
union Cyc_CfFlowInfo_FlowInfo _tmp254;struct Cyc_List_List*_tmp255;struct _tuple22 _tmp253=_stmttmp3A;_tmp254=_tmp253.f1;_tmp255=_tmp253.f2;
# 1211
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp255))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp255->tl))->hd;
outflow=_tmp254;}{
# 1216
struct _tuple17 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp18F);union Cyc_CfFlowInfo_AbsLVal _tmp258;struct _tuple17 _tmp257=_stmttmp3B;_tmp258=_tmp257.f2;{
struct _tuple17 _stmttmp3C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp190);union Cyc_CfFlowInfo_AbsLVal _tmp25A;struct _tuple17 _tmp259=_stmttmp3C;_tmp25A=_tmp259.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp25B=outflow;struct Cyc_Dict_Dict _tmp25C;struct Cyc_List_List*_tmp25D;_LL1A0: if((_tmp25B.ReachableFL).tag != 2)goto _LL1A2;_tmp25C=((struct _tuple14)(_tmp25B.ReachableFL).val).f1;_tmp25D=((struct _tuple14)(_tmp25B.ReachableFL).val).f2;_LL1A1:
# 1220
{union Cyc_CfFlowInfo_AbsLVal _tmp25E=_tmp258;struct Cyc_CfFlowInfo_Place*_tmp25F;_LL1A5: if((_tmp25E.PlaceL).tag != 1)goto _LL1A7;_tmp25F=(struct Cyc_CfFlowInfo_Place*)(_tmp25E.PlaceL).val;_LL1A6:
# 1222
 _tmp25C=Cyc_CfFlowInfo_assign_place(_tmp140,_tmp18F->loc,_tmp25C,env->all_changed,_tmp25F,right_rval);
# 1224
goto _LL1A4;_LL1A7: if((_tmp25E.UnknownL).tag != 2)goto _LL1A4;_LL1A8:
 goto _LL1A4;_LL1A4:;}
# 1227
{union Cyc_CfFlowInfo_AbsLVal _tmp260=_tmp25A;struct Cyc_CfFlowInfo_Place*_tmp261;_LL1AA: if((_tmp260.PlaceL).tag != 1)goto _LL1AC;_tmp261=(struct Cyc_CfFlowInfo_Place*)(_tmp260.PlaceL).val;_LL1AB:
# 1229
 _tmp25C=Cyc_CfFlowInfo_assign_place(_tmp140,_tmp190->loc,_tmp25C,env->all_changed,_tmp261,left_rval);
# 1231
goto _LL1A9;_LL1AC: if((_tmp260.UnknownL).tag != 2)goto _LL1A9;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1235
_tmp25D=Cyc_Relations_reln_kill_exp(_tmp140->r,_tmp25D,_tmp18F);
_tmp25D=Cyc_Relations_reln_kill_exp(_tmp140->r,_tmp25D,_tmp190);
# 1238
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp25C,_tmp25D);
goto _LL19F;_LL1A2:;_LL1A3:
 goto _LL19F;_LL19F:;}{
# 1244
struct _tuple15 _tmp752;struct _tuple15 _tmp263=(_tmp752.f1=outflow,((_tmp752.f2=_tmp140->unknown_all,_tmp752)));_npop_handler(0);return _tmp263;};};};
# 1208
;_pop_region(temp);}_LL134: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp191->tag != 15)goto _LL136;else{_tmp192=_tmp191->f1;_tmp193=_tmp191->f2;}}_LL135: {
# 1247
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp755;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp754;void*root=(void*)((_tmp754=_region_malloc(_tmp140->r,sizeof(*_tmp754)),((_tmp754[0]=((_tmp755.tag=1,((_tmp755.f1=_tmp193,((_tmp755.f2=(void*)_check_null(e->topt),_tmp755)))))),_tmp754))));
struct Cyc_CfFlowInfo_Place*_tmp756;struct Cyc_CfFlowInfo_Place*place=(_tmp756=_region_malloc(_tmp140->r,sizeof(*_tmp756)),((_tmp756->root=root,((_tmp756->fields=0,_tmp756)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp759;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp758;void*rval=(void*)((_tmp758=_region_malloc(_tmp140->r,sizeof(*_tmp758)),((_tmp758[0]=((_tmp759.tag=5,((_tmp759.f1=place,_tmp759)))),_tmp758))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp192 != 0){
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp75A[2];struct _tuple22 _stmttmp38=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp75A[1]=_tmp193,((_tmp75A[0]=_tmp192,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp75A,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1255
union Cyc_CfFlowInfo_FlowInfo _tmp266;struct Cyc_List_List*_tmp267;struct _tuple22 _tmp265=_stmttmp38;_tmp266=_tmp265.f1;_tmp267=_tmp265.f2;
# 1257
outflow=_tmp266;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp267))->tl))->hd;}
# 1255
;_pop_region(temp);}else{
# 1261
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp193);union Cyc_CfFlowInfo_FlowInfo _tmp26A;void*_tmp26B;struct _tuple15 _tmp269=_stmttmp39;_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;
outflow=_tmp26A;
place_val=_tmp26B;}{
# 1265
union Cyc_CfFlowInfo_FlowInfo _tmp26C=outflow;struct Cyc_Dict_Dict _tmp26D;struct Cyc_List_List*_tmp26E;_LL1AF: if((_tmp26C.BottomFL).tag != 1)goto _LL1B1;_LL1B0: {
struct _tuple15 _tmp75B;return(_tmp75B.f1=outflow,((_tmp75B.f2=rval,_tmp75B)));}_LL1B1: if((_tmp26C.ReachableFL).tag != 2)goto _LL1AE;_tmp26D=((struct _tuple14)(_tmp26C.ReachableFL).val).f1;_tmp26E=((struct _tuple14)(_tmp26C.ReachableFL).val).f2;_LL1B2: {
# 1268
struct _tuple15 _tmp75C;return(_tmp75C.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp26D,root,place_val),_tmp26E),((_tmp75C.f2=rval,_tmp75C)));}_LL1AE:;};};}_LL136: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp194->tag != 14)goto _LL138;else{_tmp195=_tmp194->f1;}}_LL137: {
# 1272
struct _tuple17 _stmttmp37=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp195);union Cyc_CfFlowInfo_FlowInfo _tmp277;union Cyc_CfFlowInfo_AbsLVal _tmp278;struct _tuple17 _tmp276=_stmttmp37;_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp279=_tmp278;struct Cyc_CfFlowInfo_Place*_tmp27A;_LL1B4: if((_tmp279.UnknownL).tag != 2)goto _LL1B6;_LL1B5: {
struct _tuple15 _tmp75D;return(_tmp75D.f1=_tmp277,((_tmp75D.f2=_tmp140->notzeroall,_tmp75D)));}_LL1B6: if((_tmp279.PlaceL).tag != 1)goto _LL1B3;_tmp27A=(struct Cyc_CfFlowInfo_Place*)(_tmp279.PlaceL).val;_LL1B7: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp763;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp762;struct _tuple15 _tmp761;return(_tmp761.f1=_tmp277,((_tmp761.f2=(void*)((_tmp763=_region_malloc(env->r,sizeof(*_tmp763)),((_tmp763[0]=((_tmp762.tag=5,((_tmp762.f1=_tmp27A,_tmp762)))),_tmp763)))),_tmp761)));}_LL1B3:;};}_LL138: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp196->tag != 19)goto _LL13A;else{_tmp197=_tmp196->f1;}}_LL139: {
# 1279
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp197);union Cyc_CfFlowInfo_FlowInfo _tmp280;void*_tmp281;struct _tuple15 _tmp27F=_stmttmp36;_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp280,_tmp197,_tmp281);}_LL13A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp198->tag != 20)goto _LL13C;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL13B: {
# 1283
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp283;void*_tmp284;struct _tuple15 _tmp282=_stmttmp35;_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;
if(_tmp199->topt == 0){
{const char*_tmp767;void*_tmp766[1];struct Cyc_String_pa_PrintArg_struct _tmp765;(_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp766[0]=& _tmp765,Cyc_fprintf(Cyc_stderr,((_tmp767="aggrmember exp %s\n",_tag_dyneither(_tmp767,sizeof(char),19))),_tag_dyneither(_tmp766,sizeof(void*),1)))))));}{
const char*_tmp76B;void*_tmp76A[1];struct Cyc_String_pa_PrintArg_struct _tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp199)),((_tmp76A[0]=& _tmp769,Cyc_fprintf(Cyc_stderr,((_tmp76B="oops! %s.topt = null!\n",_tag_dyneither(_tmp76B,sizeof(char),23))),_tag_dyneither(_tmp76A,sizeof(void*),1)))))));};}{
# 1288
void*_tmp28B=(void*)_check_null(_tmp199->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp28B)){
# 1291
struct _tuple15 _tmp76C;return(_tmp76C.f1=_tmp283,((_tmp76C.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp76C)));}
# 1293
if(Cyc_Absyn_is_require_union_type(_tmp28B))
Cyc_NewControlFlow_check_requires(_tmp199->loc,_tmp140->r,_tmp28B,_tmp19A,_tmp283);{
# 1296
void*_tmp28D=_tmp284;int _tmp290;int _tmp291;struct _dyneither_ptr _tmp292;int _tmp294;int _tmp295;struct _dyneither_ptr _tmp296;_LL1B9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp28E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28D;if(_tmp28E->tag != 8)goto _LL1BB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp28F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp28E->f2);if(_tmp28F->tag != 6)goto _LL1BB;else{_tmp290=(_tmp28F->f1).is_union;_tmp291=(_tmp28F->f1).fieldnum;_tmp292=_tmp28F->f2;}}}_LL1BA:
# 1298
 _tmp294=_tmp290;_tmp295=_tmp291;_tmp296=_tmp292;goto _LL1BC;_LL1BB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp293=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28D;if(_tmp293->tag != 6)goto _LL1BD;else{_tmp294=(_tmp293->f1).is_union;_tmp295=(_tmp293->f1).fieldnum;_tmp296=_tmp293->f2;}}_LL1BC: {
# 1300
int _tmp297=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp199->topt),_tmp19A);
if((_tmp294  && _tmp295 != - 1) && _tmp295 != _tmp297){
struct _tuple15 _tmp76D;return(_tmp76D.f1=_tmp283,((_tmp76D.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),1,_tmp140->unknown_none),_tmp76D)));}{
struct _tuple15 _tmp76E;return(_tmp76E.f1=_tmp283,((_tmp76E.f2=*((void**)_check_dyneither_subscript(_tmp296,sizeof(void*),_tmp297)),_tmp76E)));};}_LL1BD:;_LL1BE:
# 1305
{const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_fprintf(Cyc_stderr,((_tmp771="the bad rexp is :",_tag_dyneither(_tmp771,sizeof(char),18))),_tag_dyneither(_tmp770,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp284);
{const char*_tmp774;void*_tmp773;(_tmp773=0,Cyc_fprintf(Cyc_stderr,((_tmp774="\n",_tag_dyneither(_tmp774,sizeof(char),2))),_tag_dyneither(_tmp773,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp781;const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_PrintArg_struct _tmp77E;struct Cyc_Core_Impossible_exn_struct*_tmp77D;(int)_throw((void*)((_tmp77D=_cycalloc(sizeof(*_tmp77D)),((_tmp77D[0]=((_tmp781.tag=Cyc_Core_Impossible,((_tmp781.f1=(struct _dyneither_ptr)((_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp77F[0]=& _tmp77E,Cyc_aprintf(((_tmp780="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp780,sizeof(char),26))),_tag_dyneither(_tmp77F,sizeof(void*),1)))))))),_tmp781)))),_tmp77D)))));};_LL1B8:;};};}_LL13C: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp19B->tag != 37)goto _LL13E;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL13D: {
# 1315
struct _tuple15 _stmttmp34=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19C);union Cyc_CfFlowInfo_FlowInfo _tmp2A4;void*_tmp2A5;struct _tuple15 _tmp2A3=_stmttmp34;_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;
# 1317
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp19C->topt))){
struct _tuple15 _tmp782;return(_tmp782.f1=_tmp2A4,((_tmp782.f2=_tmp140->unknown_all,_tmp782)));}{
void*_tmp2A7=_tmp2A5;int _tmp2AA;int _tmp2AB;struct _dyneither_ptr _tmp2AC;int _tmp2AE;int _tmp2AF;struct _dyneither_ptr _tmp2B0;_LL1C0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A7;if(_tmp2A8->tag != 8)goto _LL1C2;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2A8->f2);if(_tmp2A9->tag != 6)goto _LL1C2;else{_tmp2AA=(_tmp2A9->f1).is_union;_tmp2AB=(_tmp2A9->f1).fieldnum;_tmp2AC=_tmp2A9->f2;}}}_LL1C1:
# 1321
 _tmp2AE=_tmp2AA;_tmp2AF=_tmp2AB;_tmp2B0=_tmp2AC;goto _LL1C3;_LL1C2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2A7;if(_tmp2AD->tag != 6)goto _LL1C4;else{_tmp2AE=(_tmp2AD->f1).is_union;_tmp2AF=(_tmp2AD->f1).fieldnum;_tmp2B0=_tmp2AD->f2;}}_LL1C3: {
# 1323
int _tmp2B1=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp19C->topt),_tmp19D);
if(_tmp2AE  && _tmp2AF != - 1){
if(_tmp2AF != _tmp2B1){
struct _tuple15 _tmp783;return(_tmp783.f1=_tmp2A4,((_tmp783.f2=_tmp140->zero,_tmp783)));}else{
# 1328
struct _tuple15 _tmp784;return(_tmp784.f1=_tmp2A4,((_tmp784.f2=_tmp140->notzeroall,_tmp784)));}}else{
# 1330
struct _tuple15 _tmp785;return(_tmp785.f1=_tmp2A4,((_tmp785.f2=_tmp140->unknown_all,_tmp785)));}}_LL1C4:;_LL1C5: {
# 1332
struct Cyc_Core_Impossible_exn_struct _tmp792;const char*_tmp791;void*_tmp790[1];struct Cyc_String_pa_PrintArg_struct _tmp78F;struct Cyc_Core_Impossible_exn_struct*_tmp78E;(int)_throw((void*)((_tmp78E=_cycalloc(sizeof(*_tmp78E)),((_tmp78E[0]=((_tmp792.tag=Cyc_Core_Impossible,((_tmp792.f1=(struct _dyneither_ptr)((_tmp78F.tag=0,((_tmp78F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp790[0]=& _tmp78F,Cyc_aprintf(((_tmp791="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp791,sizeof(char),26))),_tag_dyneither(_tmp790,sizeof(void*),1)))))))),_tmp792)))),_tmp78E)))));}_LL1BF:;};}_LL13E: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp19E->tag != 21)goto _LL140;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;}}_LL13F: {
# 1337
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp19F);union Cyc_CfFlowInfo_FlowInfo _tmp2BB;void*_tmp2BC;struct _tuple15 _tmp2BA=_stmttmp31;_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;{
# 1340
struct _tuple15 _stmttmp32=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BB,_tmp19F,_tmp2BC);union Cyc_CfFlowInfo_FlowInfo _tmp2BE;void*_tmp2BF;struct _tuple15 _tmp2BD=_stmttmp32;_tmp2BE=_tmp2BD.f1;_tmp2BF=_tmp2BD.f2;{
# 1343
void*_stmttmp33=Cyc_Tcutil_compress((void*)_check_null(_tmp19F->topt));void*_tmp2C0=_stmttmp33;void*_tmp2C2;_LL1C7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0;if(_tmp2C1->tag != 5)goto _LL1C9;else{_tmp2C2=(_tmp2C1->f1).elt_typ;}}_LL1C8:
# 1345
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C2)){
# 1347
struct _tuple15 _tmp793;return(_tmp793.f1=_tmp2BE,((_tmp793.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp793)));}
# 1349
if(Cyc_Absyn_is_require_union_type(_tmp2C2))
Cyc_NewControlFlow_check_requires(_tmp19F->loc,_tmp140->r,_tmp2C2,_tmp1A0,_tmp2BB);{
# 1352
void*_tmp2C4=_tmp2BF;int _tmp2C7;int _tmp2C8;struct _dyneither_ptr _tmp2C9;int _tmp2CB;int _tmp2CC;struct _dyneither_ptr _tmp2CD;_LL1CC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2C5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2C4;if(_tmp2C5->tag != 8)goto _LL1CE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2C5->f2);if(_tmp2C6->tag != 6)goto _LL1CE;else{_tmp2C7=(_tmp2C6->f1).is_union;_tmp2C8=(_tmp2C6->f1).fieldnum;_tmp2C9=_tmp2C6->f2;}}}_LL1CD:
# 1354
 _tmp2CB=_tmp2C7;_tmp2CC=_tmp2C8;_tmp2CD=_tmp2C9;goto _LL1CF;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2CA=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C4;if(_tmp2CA->tag != 6)goto _LL1D0;else{_tmp2CB=(_tmp2CA->f1).is_union;_tmp2CC=(_tmp2CA->f1).fieldnum;_tmp2CD=_tmp2CA->f2;}}_LL1CF: {
# 1356
int _tmp2CE=Cyc_CfFlowInfo_get_field_index(_tmp2C2,_tmp1A0);
if((_tmp2CB  && _tmp2CC != - 1) && _tmp2CC != _tmp2CE){
struct _tuple15 _tmp794;return(_tmp794.f1=_tmp2BE,((_tmp794.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),1,_tmp140->unknown_none),_tmp794)));}{
struct _tuple15 _tmp795;return(_tmp795.f1=_tmp2BE,((_tmp795.f2=*((void**)_check_dyneither_subscript(_tmp2CD,sizeof(void*),_tmp2CE)),_tmp795)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp79B;const char*_tmp79A;struct Cyc_Core_Impossible_exn_struct*_tmp799;(int)_throw((void*)((_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799[0]=((_tmp79B.tag=Cyc_Core_Impossible,((_tmp79B.f1=((_tmp79A="anal_Rexp: AggrArrow",_tag_dyneither(_tmp79A,sizeof(char),21))),_tmp79B)))),_tmp799)))));}_LL1CB:;};_LL1C9:;_LL1CA: {
# 1362
struct Cyc_Core_Impossible_exn_struct _tmp7A1;const char*_tmp7A0;struct Cyc_Core_Impossible_exn_struct*_tmp79F;(int)_throw((void*)((_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F[0]=((_tmp7A1.tag=Cyc_Core_Impossible,((_tmp7A1.f1=((_tmp7A0="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7A0,sizeof(char),25))),_tmp7A1)))),_tmp79F)))));}_LL1C6:;};};}_LL140: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1A1->tag != 5)goto _LL142;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;_tmp1A4=_tmp1A1->f3;}}_LL141: {
# 1366
struct _tuple18 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A2);union Cyc_CfFlowInfo_FlowInfo _tmp2D8;union Cyc_CfFlowInfo_FlowInfo _tmp2D9;struct _tuple18 _tmp2D7=_stmttmp30;_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;{
struct _tuple15 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D8,_tmp1A3);
struct _tuple15 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D9,_tmp1A4);
# 1370
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp140,env->all_changed,_tmp2DA,_tmp2DB);};}_LL142: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1A5->tag != 6)goto _LL144;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LL143: {
# 1373
struct _tuple18 _stmttmp2E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A6);union Cyc_CfFlowInfo_FlowInfo _tmp2DD;union Cyc_CfFlowInfo_FlowInfo _tmp2DE;struct _tuple18 _tmp2DC=_stmttmp2E;_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;{
struct _tuple15 _stmttmp2F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DD,_tmp1A7);union Cyc_CfFlowInfo_FlowInfo _tmp2E0;void*_tmp2E1;struct _tuple15 _tmp2DF=_stmttmp2F;_tmp2E0=_tmp2DF.f1;_tmp2E1=_tmp2DF.f2;{
struct _tuple15 _tmp7A2;struct _tuple15 _tmp2E2=(_tmp7A2.f1=_tmp2E0,((_tmp7A2.f2=_tmp2E1,_tmp7A2)));
struct _tuple15 _tmp7A3;struct _tuple15 _tmp2E3=(_tmp7A3.f1=_tmp2DE,((_tmp7A3.f2=_tmp140->zero,_tmp7A3)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp140,env->all_changed,_tmp2E2,_tmp2E3);};};}_LL144: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1A8->tag != 7)goto _LL146;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL145: {
# 1380
struct _tuple18 _stmttmp2C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1A9);union Cyc_CfFlowInfo_FlowInfo _tmp2E7;union Cyc_CfFlowInfo_FlowInfo _tmp2E8;struct _tuple18 _tmp2E6=_stmttmp2C;_tmp2E7=_tmp2E6.f1;_tmp2E8=_tmp2E6.f2;{
struct _tuple15 _stmttmp2D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E8,_tmp1AA);union Cyc_CfFlowInfo_FlowInfo _tmp2EA;void*_tmp2EB;struct _tuple15 _tmp2E9=_stmttmp2D;_tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;{
struct _tuple15 _tmp7A4;struct _tuple15 _tmp2EC=(_tmp7A4.f1=_tmp2EA,((_tmp7A4.f2=_tmp2EB,_tmp7A4)));
struct _tuple15 _tmp7A5;struct _tuple15 _tmp2ED=(_tmp7A5.f1=_tmp2E7,((_tmp7A5.f2=_tmp140->notzeroall,_tmp7A5)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp140,env->all_changed,_tmp2EC,_tmp2ED);};};}_LL146: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1AB->tag != 22)goto _LL148;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL147: {
# 1387
struct _RegionHandle _tmp2F0=_new_region("temp");struct _RegionHandle*temp=& _tmp2F0;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7A6[2];struct _tuple22 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7A6[1]=_tmp1AD,((_tmp7A6[0]=_tmp1AC,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7A6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1388
union Cyc_CfFlowInfo_FlowInfo _tmp2F2;struct Cyc_List_List*_tmp2F3;struct _tuple22 _tmp2F1=_stmttmp28;_tmp2F2=_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;{
# 1392
union Cyc_CfFlowInfo_FlowInfo _tmp2F4=_tmp2F2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F5=_tmp2F2;struct Cyc_Dict_Dict _tmp2F6;struct Cyc_List_List*_tmp2F7;_LL1D3: if((_tmp2F5.ReachableFL).tag != 2)goto _LL1D5;_tmp2F6=((struct _tuple14)(_tmp2F5.ReachableFL).val).f1;_tmp2F7=((struct _tuple14)(_tmp2F5.ReachableFL).val).f2;_LL1D4:
# 1397
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2F6,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F3))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_Tcutil_terr(_tmp1AD->loc,((_tmp7A9="expression may not be initialized",_tag_dyneither(_tmp7A9,sizeof(char),34))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2FA=Cyc_NewControlFlow_add_subscript_reln(_tmp140->r,_tmp2F7,_tmp1AC,_tmp1AD);
if(_tmp2F7 != _tmp2FA)
_tmp2F4=Cyc_CfFlowInfo_ReachableFL(_tmp2F6,_tmp2FA);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1405
void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp1AC->topt));void*_tmp2FB=_stmttmp29;struct Cyc_List_List*_tmp2FD;union Cyc_Absyn_Constraint*_tmp2FF;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FB;if(_tmp2FC->tag != 10)goto _LL1DA;else{_tmp2FD=_tmp2FC->f1;}}_LL1D9: {
# 1407
void*_stmttmp2B=(void*)((struct Cyc_List_List*)_check_null(_tmp2F3))->hd;void*_tmp300=_stmttmp2B;struct _dyneither_ptr _tmp303;struct _dyneither_ptr _tmp305;_LL1DF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp301=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp300;if(_tmp301->tag != 8)goto _LL1E1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp302=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp301->f2);if(_tmp302->tag != 6)goto _LL1E1;else{_tmp303=_tmp302->f2;}}}_LL1E0:
# 1409
 _tmp305=_tmp303;goto _LL1E2;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp304=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp300;if(_tmp304->tag != 6)goto _LL1E3;else{_tmp305=_tmp304->f2;}}_LL1E2: {
# 1411
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1AD)).f1;
struct _tuple15 _tmp7AA;struct _tuple15 _tmp307=(_tmp7AA.f1=_tmp2F4,((_tmp7AA.f2=*((void**)_check_dyneither_subscript(_tmp305,sizeof(void*),(int)i)),_tmp7AA)));_npop_handler(0);return _tmp307;}_LL1E3:;_LL1E4: {
struct Cyc_Core_Impossible_exn_struct _tmp7B0;const char*_tmp7AF;struct Cyc_Core_Impossible_exn_struct*_tmp7AE;(int)_throw((void*)((_tmp7AE=_cycalloc(sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7B0.tag=Cyc_Core_Impossible,((_tmp7B0.f1=((_tmp7AF="anal_Rexp: Subscript",_tag_dyneither(_tmp7AF,sizeof(char),21))),_tmp7B0)))),_tmp7AE)))));}_LL1DE:;}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB;if(_tmp2FE->tag != 5)goto _LL1DC;else{_tmp2FF=((_tmp2FE->f1).ptr_atts).bounds;}}_LL1DB: {
# 1416
struct _tuple15 _stmttmp2A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F2,_tmp1AC,(void*)((struct Cyc_List_List*)_check_null(_tmp2F3))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp30C;void*_tmp30D;struct _tuple15 _tmp30B=_stmttmp2A;_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp30E=_tmp30C;_LL1E6: if((_tmp30E.BottomFL).tag != 1)goto _LL1E8;_LL1E7: {
struct _tuple15 _tmp7B1;struct _tuple15 _tmp310=(_tmp7B1.f1=_tmp30C,((_tmp7B1.f2=_tmp30D,_tmp7B1)));_npop_handler(0);return _tmp310;}_LL1E8:;_LL1E9: {
struct _tuple15 _tmp7B2;struct _tuple15 _tmp312=(_tmp7B2.f1=_tmp2F4,((_tmp7B2.f2=_tmp30D,_tmp7B2)));_npop_handler(0);return _tmp312;}_LL1E5:;};}_LL1DC:;_LL1DD: {
# 1421
struct Cyc_Core_Impossible_exn_struct _tmp7B8;const char*_tmp7B7;struct Cyc_Core_Impossible_exn_struct*_tmp7B6;(int)_throw((void*)((_tmp7B6=_cycalloc(sizeof(*_tmp7B6)),((_tmp7B6[0]=((_tmp7B8.tag=Cyc_Core_Impossible,((_tmp7B8.f1=((_tmp7B7="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7B7,sizeof(char),33))),_tmp7B8)))),_tmp7B6)))));}_LL1D7:;};};}
# 1388
;_pop_region(temp);}_LL148: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1AE->tag != 30)goto _LL14A;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LL149:
# 1426
 _tmp1B2=_tmp1AF;goto _LL14B;_LL14A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1B1->tag != 23)goto _LL14C;else{_tmp1B2=_tmp1B1->f1;}}_LL14B: {
# 1428
struct _RegionHandle _tmp317=_new_region("temp");struct _RegionHandle*temp=& _tmp317;_push_region(temp);
{struct _tuple22 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1B2,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp319;struct Cyc_List_List*_tmp31A;struct _tuple22 _tmp318=_stmttmp27;_tmp319=_tmp318.f1;_tmp31A=_tmp318.f2;{
unsigned int _tmp7BE;unsigned int _tmp7BD;struct _dyneither_ptr _tmp7BC;void**_tmp7BB;unsigned int _tmp7BA;struct _dyneither_ptr aggrdict=
(_tmp7BA=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1B2),((_tmp7BB=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7BA)),((_tmp7BC=_tag_dyneither(_tmp7BB,sizeof(void*),_tmp7BA),((((_tmp7BD=_tmp7BA,_tmp7BF(& _tmp7BE,& _tmp7BD,& _tmp7BB,& _tmp31A))),_tmp7BC)))))));
# 1436
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7C9;struct Cyc_CfFlowInfo_UnionRInfo _tmp7C8;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7C7;struct _tuple15 _tmp7C6;struct _tuple15 _tmp31F=(_tmp7C6.f1=_tmp319,((_tmp7C6.f2=(void*)((_tmp7C9=_region_malloc(env->r,sizeof(*_tmp7C9)),((_tmp7C9[0]=((_tmp7C7.tag=6,((_tmp7C7.f1=((_tmp7C8.is_union=0,((_tmp7C8.fieldnum=- 1,_tmp7C8)))),((_tmp7C7.f2=aggrdict,_tmp7C7)))))),_tmp7C9)))),_tmp7C6)));_npop_handler(0);return _tmp31F;};}
# 1429
;_pop_region(temp);}_LL14C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1B3->tag != 29)goto _LL14E;else{_tmp1B4=_tmp1B3->f2;}}_LL14D: {
# 1439
struct Cyc_List_List*fs;
{void*_stmttmp26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp325=_stmttmp26;struct Cyc_List_List*_tmp327;_LL1EB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 12)goto _LL1ED;else{_tmp327=_tmp326->f2;}}_LL1EC:
 fs=_tmp327;goto _LL1EA;_LL1ED:;_LL1EE: {
struct Cyc_Core_Impossible_exn_struct _tmp7CF;const char*_tmp7CE;struct Cyc_Core_Impossible_exn_struct*_tmp7CD;(int)_throw((void*)((_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CF.tag=Cyc_Core_Impossible,((_tmp7CF.f1=((_tmp7CE="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7CE,sizeof(char),35))),_tmp7CF)))),_tmp7CD)))));}_LL1EA:;}
# 1444
_tmp1B6=_tmp1B4;_tmp1B7=Cyc_Absyn_StructA;_tmp1B8=fs;goto _LL14F;}_LL14E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1B5->tag != 28)goto _LL150;else{_tmp1B6=_tmp1B5->f3;if(_tmp1B5->f4 == 0)goto _LL150;_tmp1B7=(_tmp1B5->f4)->kind;if((_tmp1B5->f4)->impl == 0)goto _LL150;_tmp1B8=((_tmp1B5->f4)->impl)->fields;}}_LL14F: {
# 1446
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp32B=_new_region("temp");struct _RegionHandle*temp=& _tmp32B;_push_region(temp);
{struct _tuple22 _stmttmp24=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1B6),1,1);
# 1448
union Cyc_CfFlowInfo_FlowInfo _tmp32D;struct Cyc_List_List*_tmp32E;struct _tuple22 _tmp32C=_stmttmp24;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;{
# 1450
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp140,_tmp1B8,_tmp1B7 == Cyc_Absyn_UnionA,_tmp140->unknown_all);
# 1453
int field_no=-1;
{int i=0;for(0;_tmp32E != 0;(((_tmp32E=_tmp32E->tl,_tmp1B6=_tmp1B6->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1B6))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp25=(void*)ds->hd;void*_tmp32F=_stmttmp25;struct _dyneither_ptr*_tmp332;_LL1F0: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp330=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32F;if(_tmp330->tag != 0)goto _LL1F2;}_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7D5;const char*_tmp7D4;struct Cyc_Core_Impossible_exn_struct*_tmp7D3;(int)_throw((void*)((_tmp7D3=_cycalloc(sizeof(*_tmp7D3)),((_tmp7D3[0]=((_tmp7D5.tag=Cyc_Core_Impossible,((_tmp7D5.f1=((_tmp7D4="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7D4,sizeof(char),22))),_tmp7D5)))),_tmp7D3)))));}_LL1F2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp331=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32F;if(_tmp331->tag != 1)goto _LL1EF;else{_tmp332=_tmp331->f1;}}_LL1F3:
# 1460
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1B8,_tmp332);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp32E->hd;
# 1463
if(_tmp1B7 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp336=(*((struct _tuple25*)_tmp1B6->hd)).f2;
_tmp32D=Cyc_NewControlFlow_use_Rval(env,_tmp336->loc,_tmp32D,(void*)_tmp32E->hd);
# 1467
Cyc_NewControlFlow_check_requires(_tmp336->loc,_tmp140->r,exp_type,_tmp332,_tmp32D);}_LL1EF:;}}}{
# 1470
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7DB;struct Cyc_CfFlowInfo_UnionRInfo _tmp7DA;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7D9;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp337=(_tmp7D9=_region_malloc(env->r,sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DB.tag=6,((_tmp7DB.f1=((_tmp7DA.is_union=_tmp1B7 == Cyc_Absyn_UnionA,((_tmp7DA.fieldnum=field_no,_tmp7DA)))),((_tmp7DB.f2=aggrdict,_tmp7DB)))))),_tmp7D9)));
struct _tuple15 _tmp7DC;struct _tuple15 _tmp339=(_tmp7DC.f1=_tmp32D,((_tmp7DC.f2=(void*)_tmp337,_tmp7DC)));_npop_handler(0);return _tmp339;};};}
# 1448
;_pop_region(temp);}_LL150: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1B9=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1B9->tag != 28)goto _LL152;}_LL151: {
# 1474
struct Cyc_Core_Impossible_exn_struct _tmp7E2;const char*_tmp7E1;struct Cyc_Core_Impossible_exn_struct*_tmp7E0;(int)_throw((void*)((_tmp7E0=_cycalloc(sizeof(*_tmp7E0)),((_tmp7E0[0]=((_tmp7E2.tag=Cyc_Core_Impossible,((_tmp7E2.f1=((_tmp7E1="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7E1,sizeof(char),31))),_tmp7E2)))),_tmp7E0)))));}_LL152: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1BA->tag != 25)goto _LL154;else{_tmp1BB=_tmp1BA->f1;}}_LL153: {
# 1476
struct _RegionHandle _tmp340=_new_region("temp");struct _RegionHandle*temp=& _tmp340;_push_region(temp);
{struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1BB);
struct _tuple22 _stmttmp23=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp341,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp343;struct Cyc_List_List*_tmp344;struct _tuple22 _tmp342=_stmttmp23;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;
for(0;_tmp344 != 0;(_tmp344=_tmp344->tl,_tmp341=_tmp341->tl)){
_tmp343=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp341))->hd)->loc,_tmp343,(void*)_tmp344->hd);}{
struct _tuple15 _tmp7E3;struct _tuple15 _tmp346=(_tmp7E3.f1=_tmp343,((_tmp7E3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,(void*)_check_null(e->topt),0,_tmp140->unknown_all),_tmp7E3)));_npop_handler(0);return _tmp346;};}
# 1477
;_pop_region(temp);}_LL154: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1BC->tag != 26)goto _LL156;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;_tmp1BF=_tmp1BC->f3;_tmp1C0=_tmp1BC->f4;}}_LL155: {
# 1485
struct _tuple15 _stmttmp20=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1BE);union Cyc_CfFlowInfo_FlowInfo _tmp348;void*_tmp349;struct _tuple15 _tmp347=_stmttmp20;_tmp348=_tmp347.f1;_tmp349=_tmp347.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp34A=_tmp348;struct Cyc_Dict_Dict _tmp34B;struct Cyc_List_List*_tmp34C;_LL1F5: if((_tmp34A.BottomFL).tag != 1)goto _LL1F7;_LL1F6: {
struct _tuple15 _tmp7E4;return(_tmp7E4.f1=_tmp348,((_tmp7E4.f2=_tmp140->unknown_all,_tmp7E4)));}_LL1F7: if((_tmp34A.ReachableFL).tag != 2)goto _LL1F4;_tmp34B=((struct _tuple14)(_tmp34A.ReachableFL).val).f1;_tmp34C=((struct _tuple14)(_tmp34A.ReachableFL).val).f2;_LL1F8:
# 1489
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp34B,_tmp349)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=0,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp7E7="expression may not be initialized",_tag_dyneither(_tmp7E7,sizeof(char),34))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}{
# 1493
struct Cyc_List_List*new_relns=_tmp34C;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1BD);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1BE,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp34C);
# 1499
if(_tmp34C != new_relns)
_tmp348=Cyc_CfFlowInfo_ReachableFL(_tmp34B,new_relns);{
# 1503
void*_tmp350=_tmp349;_LL1FA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp351=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp350;if(_tmp351->tag != 0)goto _LL1FC;}_LL1FB: {
struct _tuple15 _tmp7E8;return(_tmp7E8.f1=_tmp348,((_tmp7E8.f2=_tmp140->unknown_all,_tmp7E8)));}_LL1FC: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp352=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp350;if(_tmp352->tag != 2)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp353=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp350;if(_tmp353->tag != 1)goto _LL200;}_LL1FF:
 goto _LL201;_LL200: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp354=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp350;if(_tmp354->tag != 5)goto _LL202;}_LL201: {
# 1508
struct Cyc_List_List _tmp7E9;struct Cyc_List_List _tmp356=(_tmp7E9.hd=_tmp1BD,((_tmp7E9.tl=0,_tmp7E9)));
_tmp348=Cyc_NewControlFlow_add_vars(_tmp140,_tmp348,& _tmp356,_tmp140->unknown_all,e->loc,0);{
# 1511
struct _tuple15 _stmttmp22=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp348,_tmp1BF);union Cyc_CfFlowInfo_FlowInfo _tmp358;void*_tmp359;struct _tuple15 _tmp357=_stmttmp22;_tmp358=_tmp357.f1;_tmp359=_tmp357.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp35A=_tmp358;struct Cyc_Dict_Dict _tmp35B;_LL205: if((_tmp35A.BottomFL).tag != 1)goto _LL207;_LL206: {
struct _tuple15 _tmp7EA;return(_tmp7EA.f1=_tmp358,((_tmp7EA.f2=_tmp140->unknown_all,_tmp7EA)));}_LL207: if((_tmp35A.ReachableFL).tag != 2)goto _LL204;_tmp35B=((struct _tuple14)(_tmp35A.ReachableFL).val).f1;_LL208:
# 1515
 if(Cyc_CfFlowInfo_initlevel(_tmp140,_tmp35B,_tmp359)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp7ED="expression may not be initialized",_tag_dyneither(_tmp7ED,sizeof(char),34))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}{
struct _tuple15 _tmp7EE;return(_tmp7EE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EE.f2=_tmp140->unknown_all,_tmp7EE)));};}_LL204:;}
# 1520
_tmp348=_tmp358;
goto _LL203;};}_LL202:;_LL203:
# 1523
 while(1){
struct Cyc_List_List _tmp7EF;struct Cyc_List_List _tmp361=(_tmp7EF.hd=_tmp1BD,((_tmp7EF.tl=0,_tmp7EF)));
_tmp348=Cyc_NewControlFlow_add_vars(_tmp140,_tmp348,& _tmp361,_tmp140->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp21=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp348,_tmp1BF);union Cyc_CfFlowInfo_FlowInfo _tmp363;void*_tmp364;struct _tuple15 _tmp362=_stmttmp21;_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp365=_tmp363;struct Cyc_Dict_Dict _tmp366;_LL20A: if((_tmp365.BottomFL).tag != 1)goto _LL20C;_LL20B:
 goto _LL209;_LL20C: if((_tmp365.ReachableFL).tag != 2)goto _LL209;_tmp366=((struct _tuple14)(_tmp365.ReachableFL).val).f1;_LL20D:
# 1530
 if(Cyc_CfFlowInfo_initlevel(_tmp140,_tmp366,_tmp364)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F2;void*_tmp7F1;(_tmp7F1=0,Cyc_Tcutil_terr(_tmp1BE->loc,((_tmp7F2="expression may not be initialized",_tag_dyneither(_tmp7F2,sizeof(char),34))),_tag_dyneither(_tmp7F1,sizeof(void*),0)));}{
struct _tuple15 _tmp7F3;return(_tmp7F3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F3.f2=_tmp140->unknown_all,_tmp7F3)));};}_LL209:;}{
# 1535
union Cyc_CfFlowInfo_FlowInfo _tmp36A=Cyc_CfFlowInfo_join_flow(_tmp140,env->all_changed,_tmp348,_tmp363);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp36A,_tmp348))
break;
_tmp348=_tmp36A;};};}{
# 1540
struct _tuple15 _tmp7F4;return(_tmp7F4.f1=_tmp348,((_tmp7F4.f2=_tmp140->unknown_all,_tmp7F4)));};_LL1F9:;};};_LL1F4:;};}_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1C1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1C1->tag != 27)goto _LL158;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=(void*)_tmp1C1->f2;_tmp1C4=_tmp1C1->f3;}}_LL157: {
# 1546
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7F7;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7F6;void*root=(void*)((_tmp7F6=_region_malloc(_tmp140->r,sizeof(*_tmp7F6)),((_tmp7F6[0]=((_tmp7F7.tag=1,((_tmp7F7.f1=_tmp1C2,((_tmp7F7.f2=(void*)_check_null(e->topt),_tmp7F7)))))),_tmp7F6))));
struct Cyc_CfFlowInfo_Place*_tmp7F8;struct Cyc_CfFlowInfo_Place*place=(_tmp7F8=_region_malloc(_tmp140->r,sizeof(*_tmp7F8)),((_tmp7F8->root=root,((_tmp7F8->fields=0,_tmp7F8)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7FB;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7FA;void*rval=(void*)((_tmp7FA=_region_malloc(_tmp140->r,sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FB.tag=5,((_tmp7FB.f1=place,_tmp7FB)))),_tmp7FA))));
void*place_val;
# 1554
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp140,_tmp1C3,0,_tmp140->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C2);union Cyc_CfFlowInfo_FlowInfo _tmp36E;struct _tuple15 _tmp36D=_stmttmp1F;_tmp36E=_tmp36D.f1;
outflow=_tmp36E;{
union Cyc_CfFlowInfo_FlowInfo _tmp36F=outflow;struct Cyc_Dict_Dict _tmp370;struct Cyc_List_List*_tmp371;_LL20F: if((_tmp36F.BottomFL).tag != 1)goto _LL211;_LL210: {
struct _tuple15 _tmp7FC;return(_tmp7FC.f1=outflow,((_tmp7FC.f2=rval,_tmp7FC)));}_LL211: if((_tmp36F.ReachableFL).tag != 2)goto _LL20E;_tmp370=((struct _tuple14)(_tmp36F.ReachableFL).val).f1;_tmp371=((struct _tuple14)(_tmp36F.ReachableFL).val).f2;_LL212: {
# 1562
struct _tuple15 _tmp7FD;return(_tmp7FD.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp370,root,place_val),_tmp371),((_tmp7FD.f2=rval,_tmp7FD)));}_LL20E:;};};};}_LL158: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1C5->tag != 36)goto _LL15A;else{_tmp1C6=_tmp1C5->f1;}}_LL159: {
# 1566
struct _tuple16 _tmp7FE;struct _tuple16 _tmp379=(_tmp7FE.f1=(env->fenv)->unknown_all,((_tmp7FE.f2=copy_ctxt,_tmp7FE)));
union Cyc_CfFlowInfo_FlowInfo _tmp37A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1C6,& _tmp379);
struct _tuple15 _tmp7FF;return(_tmp7FF.f1=_tmp37A,((_tmp7FF.f2=_tmp379.f1,_tmp7FF)));}_LL15A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1C7->tag != 1)goto _LL15C;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1C8=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1C7->f2);if(_tmp1C8->tag != 0)goto _LL15C;}}_LL15B:
# 1570
 goto _LL15D;_LL15C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1C9->tag != 35)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1CA->tag != 24)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1CB->tag != 38)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp1CC->tag != 39)goto _LLFB;}_LL163: {
# 1575
struct Cyc_Core_Impossible_exn_struct _tmp805;const char*_tmp804;struct Cyc_Core_Impossible_exn_struct*_tmp803;(int)_throw((void*)((_tmp803=_cycalloc(sizeof(*_tmp803)),((_tmp803[0]=((_tmp805.tag=Cyc_Core_Impossible,((_tmp805.f1=((_tmp804="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp804,sizeof(char),31))),_tmp805)))),_tmp803)))));}_LLFB:;};}
# 1585
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1593
struct Cyc_CfFlowInfo_FlowEnv*_tmp380=env->fenv;
void*_stmttmp51=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp381=_stmttmp51;void*_tmp383;union Cyc_Absyn_Constraint*_tmp384;union Cyc_Absyn_Constraint*_tmp385;_LL214: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381;if(_tmp382->tag != 5)goto _LL216;else{_tmp383=(_tmp382->f1).elt_typ;_tmp384=((_tmp382->f1).ptr_atts).bounds;_tmp385=((_tmp382->f1).ptr_atts).zero_term;}}_LL215: {
# 1596
union Cyc_CfFlowInfo_FlowInfo _tmp386=f;struct Cyc_Dict_Dict _tmp387;struct Cyc_List_List*_tmp388;_LL219: if((_tmp386.BottomFL).tag != 1)goto _LL21B;_LL21A: {
struct _tuple17 _tmp806;return(_tmp806.f1=f,((_tmp806.f2=Cyc_CfFlowInfo_UnknownL(),_tmp806)));}_LL21B: if((_tmp386.ReachableFL).tag != 2)goto _LL218;_tmp387=((struct _tuple14)(_tmp386.ReachableFL).val).f1;_tmp388=((struct _tuple14)(_tmp386.ReachableFL).val).f2;_LL21C: {
# 1600
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp384)){
retry: {void*_tmp38A=r;struct Cyc_Absyn_Vardecl*_tmp38C;void*_tmp38D;void*_tmp391;struct Cyc_List_List*_tmp392;enum Cyc_CfFlowInfo_InitLevel _tmp395;_LL21E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp38B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp38B->tag != 8)goto _LL220;else{_tmp38C=_tmp38B->f1;_tmp38D=(void*)_tmp38B->f2;}}_LL21F:
 r=_tmp38D;locname=_tmp38C;goto retry;_LL220: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp38E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp38E->tag != 1)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp38F=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp38F->tag != 2)goto _LL224;}_LL223:
# 1606
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp809;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp808;e->annot=(void*)((_tmp808=_cycalloc(sizeof(*_tmp808)),((_tmp808[0]=((_tmp809.tag=Cyc_CfFlowInfo_NotZero,((_tmp809.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp388),_tmp809)))),_tmp808))));}goto _LL21D;_LL224: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp390=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp390->tag != 5)goto _LL226;else{_tmp391=(_tmp390->f1)->root;_tmp392=(_tmp390->f1)->fields;}}_LL225:
# 1608
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp80C;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp80B;e->annot=(void*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B[0]=((_tmp80C.tag=Cyc_CfFlowInfo_NotZero,((_tmp80C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp388),_tmp80C)))),_tmp80B))));}{
struct Cyc_CfFlowInfo_Place*_tmp80F;struct _tuple17 _tmp80E;return(_tmp80E.f1=f,((_tmp80E.f2=Cyc_CfFlowInfo_PlaceL(((_tmp80F=_region_malloc(_tmp380->r,sizeof(*_tmp80F)),((_tmp80F->root=_tmp391,((_tmp80F->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp380->r,_tmp392,flds),_tmp80F))))))),_tmp80E)));};_LL226: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp393=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp393->tag != 0)goto _LL228;}_LL227:
# 1611
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp810;return(_tmp810.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp810.f2=Cyc_CfFlowInfo_UnknownL(),_tmp810)));};_LL228: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp394=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp394->tag != 3)goto _LL22A;else{_tmp395=_tmp394->f1;}}_LL229:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp395,locname);goto _LL22B;_LL22A:;_LL22B:
# 1616
 if(passthrough_consumes){
void*_tmp39D=r;void*_tmp39F;_LL22D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp39E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp39D;if(_tmp39E->tag != 7)goto _LL22F;else{_tmp39F=(void*)_tmp39E->f3;}}_LL22E:
 r=_tmp39F;goto retry;_LL22F:;_LL230:
 goto _LL22C;_LL22C:;}{
# 1622
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp813;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp812;e->annot=(void*)((_tmp812=_cycalloc(sizeof(*_tmp812)),((_tmp812[0]=((_tmp813.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp813.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp388),_tmp813)))),_tmp812))));};_LL21D:;}}else{
# 1625
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp816;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp815;e->annot=(void*)((_tmp815=_cycalloc(sizeof(*_tmp815)),((_tmp815[0]=((_tmp816.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp816.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp388),_tmp816)))),_tmp815))));}
if(Cyc_CfFlowInfo_initlevel(_tmp380,_tmp387,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_terr(e->loc,((_tmp819="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp819,sizeof(char),46))),_tag_dyneither(_tmp818,sizeof(void*),0)));}{
struct _tuple17 _tmp81A;return(_tmp81A.f1=f,((_tmp81A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp81A)));};}_LL218:;}_LL216:;_LL217: {
# 1630
struct Cyc_Core_Impossible_exn_struct _tmp820;const char*_tmp81F;struct Cyc_Core_Impossible_exn_struct*_tmp81E;(int)_throw((void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp820.tag=Cyc_Core_Impossible,((_tmp820.f1=((_tmp81F="left deref of non-pointer-type",_tag_dyneither(_tmp81F,sizeof(char),31))),_tmp820)))),_tmp81E)))));}_LL213:;}
# 1638
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1641
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3AA=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3AB=inflow;struct Cyc_Dict_Dict _tmp3AC;struct Cyc_List_List*_tmp3AD;_LL232: if((_tmp3AB.BottomFL).tag != 1)goto _LL234;_LL233: {
struct _tuple17 _tmp821;return(_tmp821.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp821.f2=Cyc_CfFlowInfo_UnknownL(),_tmp821)));}_LL234: if((_tmp3AB.ReachableFL).tag != 2)goto _LL231;_tmp3AC=((struct _tuple14)(_tmp3AB.ReachableFL).val).f1;_tmp3AD=((struct _tuple14)(_tmp3AB.ReachableFL).val).f2;_LL235:
# 1646
 d=_tmp3AC;_LL231:;}{
# 1650
void*_stmttmp52=e->r;void*_tmp3AF=_stmttmp52;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Vardecl*_tmp3B8;struct Cyc_Absyn_Vardecl*_tmp3BB;struct Cyc_Absyn_Vardecl*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C3;struct _dyneither_ptr*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3CB;struct _dyneither_ptr*_tmp3CC;_LL237: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B0->tag != 13)goto _LL239;else{_tmp3B1=_tmp3B0->f2;}}_LL238:
 _tmp3B3=_tmp3B1;goto _LL23A;_LL239: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3B2=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B2->tag != 11)goto _LL23B;else{_tmp3B3=_tmp3B2->f1;}}_LL23A:
 _tmp3B5=_tmp3B3;goto _LL23C;_LL23B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B4->tag != 12)goto _LL23D;else{_tmp3B5=_tmp3B4->f1;}}_LL23C:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3B5,flds);_LL23D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B6->tag != 1)goto _LL23F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3B7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3B6->f2);if(_tmp3B7->tag != 3)goto _LL23F;else{_tmp3B8=_tmp3B7->f1;}}}_LL23E:
# 1655
 _tmp3BB=_tmp3B8;goto _LL240;_LL23F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3B9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B9->tag != 1)goto _LL241;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3BA=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3B9->f2);if(_tmp3BA->tag != 4)goto _LL241;else{_tmp3BB=_tmp3BA->f1;}}}_LL240:
 _tmp3BE=_tmp3BB;goto _LL242;_LL241: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3BC->tag != 1)goto _LL243;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3BD=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3BC->f2);if(_tmp3BD->tag != 5)goto _LL243;else{_tmp3BE=_tmp3BD->f1;}}}_LL242: {
# 1658
struct Cyc_CfFlowInfo_Place*_tmp82B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp82A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp829;struct _tuple17 _tmp828;return(_tmp828.f1=inflow,((_tmp828.f2=Cyc_CfFlowInfo_PlaceL(((_tmp82B=_region_malloc(env->r,sizeof(*_tmp82B)),((_tmp82B->root=(void*)((_tmp829=_region_malloc(env->r,sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=0,((_tmp82A.f1=_tmp3BE,_tmp82A)))),_tmp829)))),((_tmp82B->fields=flds,_tmp82B))))))),_tmp828)));}_LL243: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3BF->tag != 1)goto _LL245;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3C0=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3BF->f2);if(_tmp3C0->tag != 1)goto _LL245;else{_tmp3C1=_tmp3C0->f1;}}}_LL244: {
# 1660
struct _tuple17 _tmp82C;return(_tmp82C.f1=inflow,((_tmp82C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82C)));}_LL245: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3C2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3C2->tag != 21)goto _LL247;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL246:
# 1663
{void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3C3->topt));void*_tmp3D2=_stmttmp59;void*_tmp3D4;_LL250: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D2;if(_tmp3D3->tag != 5)goto _LL252;else{_tmp3D4=(_tmp3D3->f1).elt_typ;}}_LL251:
# 1665
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D4)){
Cyc_NewControlFlow_check_requires(_tmp3C3->loc,_tmp3AA->r,_tmp3D4,_tmp3C4,inflow);{
struct Cyc_List_List*_tmp82D;flds=((_tmp82D=_region_malloc(env->r,sizeof(*_tmp82D)),((_tmp82D->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3D4,_tmp3C4),((_tmp82D->tl=flds,_tmp82D))))));};}
# 1669
goto _LL24F;_LL252:;_LL253: {
struct Cyc_Core_Impossible_exn_struct _tmp833;const char*_tmp832;struct Cyc_Core_Impossible_exn_struct*_tmp831;(int)_throw((void*)((_tmp831=_cycalloc(sizeof(*_tmp831)),((_tmp831[0]=((_tmp833.tag=Cyc_Core_Impossible,((_tmp833.f1=((_tmp832="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp832,sizeof(char),25))),_tmp833)))),_tmp831)))));}_LL24F:;}
# 1672
_tmp3C6=_tmp3C3;goto _LL248;_LL247: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3C5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3C5->tag != 19)goto _LL249;else{_tmp3C6=_tmp3C5->f1;}}_LL248:
# 1674
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C6->topt),0)){
struct _tuple17 _stmttmp56=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C6->topt),1),passthrough_consumes,_tmp3C6);
# 1675
union Cyc_CfFlowInfo_FlowInfo _tmp3DA;union Cyc_CfFlowInfo_AbsLVal _tmp3DB;struct _tuple17 _tmp3D9=_stmttmp56;_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;{
# 1679
struct _tuple17 _tmp834;struct _tuple17 _stmttmp57=(_tmp834.f1=_tmp3DA,((_tmp834.f2=_tmp3DB,_tmp834)));struct _tuple17 _tmp3DC=_stmttmp57;struct Cyc_Dict_Dict _tmp3DD;struct Cyc_List_List*_tmp3DE;struct Cyc_CfFlowInfo_Place*_tmp3DF;_LL255: if(((_tmp3DC.f1).ReachableFL).tag != 2)goto _LL257;_tmp3DD=((struct _tuple14)((_tmp3DC.f1).ReachableFL).val).f1;_tmp3DE=((struct _tuple14)((_tmp3DC.f1).ReachableFL).val).f2;if(((_tmp3DC.f2).PlaceL).tag != 1)goto _LL257;_tmp3DF=(struct Cyc_CfFlowInfo_Place*)((_tmp3DC.f2).PlaceL).val;_LL256: {
# 1681
void*_tmp3E0=Cyc_CfFlowInfo_lookup_place(_tmp3DD,_tmp3DF);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp3E1=_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3E3;void*_tmp3E4;_LL25A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E1;if(_tmp3E2->tag != 8)goto _LL25C;else{_tmp3E3=_tmp3E2->f1;_tmp3E4=(void*)_tmp3E2->f2;}}_LL25B:
 _tmp3E0=_tmp3E4;locname=_tmp3E3;goto retry;_LL25C: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3E5=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E1;if(_tmp3E5->tag != 7)goto _LL25E;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3E6=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp3E5->f3);if(_tmp3E6->tag != 5)goto _LL25E;}}_LL25D:
 goto _LL25F;_LL25E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3E7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3E1;if(_tmp3E7->tag != 5)goto _LL260;}_LL25F:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3DA,_tmp3C6,_tmp3DA,_tmp3E0,passthrough_consumes,flds);_LL260:;_LL261: {
# 1693
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3AA,_tmp3DD,_tmp3E0);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3AA->unknown_all: _tmp3AA->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3AA,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3C6->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp837;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp836;void*new_root=(void*)((_tmp836=_region_malloc(_tmp3AA->r,sizeof(*_tmp836)),((_tmp836[0]=((_tmp837.tag=1,((_tmp837.f1=e,((_tmp837.f2=(void*)_check_null(e->topt),_tmp837)))))),_tmp836))));
struct Cyc_CfFlowInfo_Place*_tmp838;struct Cyc_CfFlowInfo_Place*place=(_tmp838=_region_malloc(_tmp3AA->r,sizeof(*_tmp838)),((_tmp838->root=new_root,((_tmp838->fields=0,_tmp838)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp83B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp83A;void*res=(void*)((_tmp83A=_region_malloc(_tmp3AA->r,sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=5,((_tmp83B.f1=place,_tmp83B)))),_tmp83A))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp83E;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp83D;res=(void*)((_tmp83D=_region_malloc(_tmp3AA->r,sizeof(*_tmp83D)),((_tmp83D[0]=((_tmp83E.tag=8,((_tmp83E.f1=locname,((_tmp83E.f2=res,_tmp83E)))))),_tmp83D))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3DD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3DD,new_root,new_rval);
_tmp3DD=Cyc_CfFlowInfo_assign_place(_tmp3AA,e->loc,_tmp3DD,env->all_changed,_tmp3DF,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3EA=Cyc_CfFlowInfo_ReachableFL(_tmp3DD,_tmp3DE);
# 1708
return Cyc_NewControlFlow_anal_derefL(env,_tmp3EA,_tmp3C6,_tmp3EA,res,passthrough_consumes,flds);};}_LL259:;}}_LL257:;_LL258:
# 1710
 goto _LL254;_LL254:;};}{
# 1713
struct _tuple15 _stmttmp58=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3C6);union Cyc_CfFlowInfo_FlowInfo _tmp3F2;void*_tmp3F3;struct _tuple15 _tmp3F1=_stmttmp58;_tmp3F2=_tmp3F1.f1;_tmp3F3=_tmp3F1.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C6,_tmp3F2,_tmp3F3,passthrough_consumes,flds);};_LL249: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3C7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3C7->tag != 22)goto _LL24B;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=_tmp3C7->f2;}}_LL24A: {
# 1717
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(_tmp3C8->topt));void*_tmp3F4=_stmttmp53;union Cyc_Absyn_Constraint*_tmp3F7;_LL263: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F5->tag != 10)goto _LL265;}_LL264: {
# 1719
unsigned int _tmp3F8=(Cyc_Evexp_eval_const_uint_exp(_tmp3C9)).f1;
struct Cyc_List_List*_tmp83F;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C8,((_tmp83F=_region_malloc(env->r,sizeof(*_tmp83F)),((_tmp83F->hd=(void*)((int)_tmp3F8),((_tmp83F->tl=flds,_tmp83F)))))));}_LL265: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F6->tag != 5)goto _LL267;else{_tmp3F7=((_tmp3F6->f1).ptr_atts).bounds;}}_LL266: {
# 1722
struct _RegionHandle _tmp3FA=_new_region("temp");struct _RegionHandle*temp=& _tmp3FA;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp840[2];struct _tuple22 _stmttmp54=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp840[1]=_tmp3C9,((_tmp840[0]=_tmp3C8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp840,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1723
union Cyc_CfFlowInfo_FlowInfo _tmp3FC;struct Cyc_List_List*_tmp3FD;struct _tuple22 _tmp3FB=_stmttmp54;_tmp3FC=_tmp3FB.f1;_tmp3FD=_tmp3FB.f2;{
# 1725
union Cyc_CfFlowInfo_FlowInfo _tmp3FE=_tmp3FC;
{union Cyc_CfFlowInfo_FlowInfo _tmp3FF=_tmp3FC;struct Cyc_Dict_Dict _tmp400;struct Cyc_List_List*_tmp401;_LL26A: if((_tmp3FF.ReachableFL).tag != 2)goto _LL26C;_tmp400=((struct _tuple14)(_tmp3FF.ReachableFL).val).f1;_tmp401=((struct _tuple14)(_tmp3FF.ReachableFL).val).f2;_LL26B:
# 1728
 if(Cyc_CfFlowInfo_initlevel(_tmp3AA,_tmp400,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3FD))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp843;void*_tmp842;(_tmp842=0,Cyc_Tcutil_terr(_tmp3C9->loc,((_tmp843="expression may not be initialized",_tag_dyneither(_tmp843,sizeof(char),34))),_tag_dyneither(_tmp842,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp404=Cyc_NewControlFlow_add_subscript_reln(_tmp3AA->r,_tmp401,_tmp3C8,_tmp3C9);
if(_tmp401 != _tmp404)
_tmp3FE=Cyc_CfFlowInfo_ReachableFL(_tmp400,_tmp404);
goto _LL269;};_LL26C:;_LL26D:
 goto _LL269;_LL269:;}{
# 1736
struct _tuple17 _stmttmp55=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C8,_tmp3FC,(void*)((struct Cyc_List_List*)_check_null(_tmp3FD))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp406;union Cyc_CfFlowInfo_AbsLVal _tmp407;struct _tuple17 _tmp405=_stmttmp55;_tmp406=_tmp405.f1;_tmp407=_tmp405.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp408=_tmp406;_LL26F: if((_tmp408.BottomFL).tag != 1)goto _LL271;_LL270: {
struct _tuple17 _tmp844;struct _tuple17 _tmp40A=(_tmp844.f1=_tmp406,((_tmp844.f2=_tmp407,_tmp844)));_npop_handler(0);return _tmp40A;}_LL271:;_LL272: {
struct _tuple17 _tmp845;struct _tuple17 _tmp40C=(_tmp845.f1=_tmp3FE,((_tmp845.f2=_tmp407,_tmp845)));_npop_handler(0);return _tmp40C;}_LL26E:;};};};}
# 1723
;_pop_region(temp);}_LL267:;_LL268: {
# 1742
struct Cyc_Core_Impossible_exn_struct _tmp84B;const char*_tmp84A;struct Cyc_Core_Impossible_exn_struct*_tmp849;(int)_throw((void*)((_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849[0]=((_tmp84B.tag=Cyc_Core_Impossible,((_tmp84B.f1=((_tmp84A="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp84A,sizeof(char),33))),_tmp84B)))),_tmp849)))));}_LL262:;}_LL24B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3CA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3CA->tag != 20)goto _LL24D;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL24C: {
# 1746
void*_tmp411=(void*)_check_null(_tmp3CB->topt);
if(Cyc_Absyn_is_require_union_type(_tmp411))
Cyc_NewControlFlow_check_requires(_tmp3CB->loc,_tmp3AA->r,_tmp411,_tmp3CC,inflow);
# 1750
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp411)){
struct _tuple17 _tmp84C;return(_tmp84C.f1=inflow,((_tmp84C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp84C)));}{
# 1753
struct Cyc_List_List*_tmp84D;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CB,(
(_tmp84D=_region_malloc(env->r,sizeof(*_tmp84D)),((_tmp84D->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp411,_tmp3CC),((_tmp84D->tl=flds,_tmp84D)))))));};}_LL24D:;_LL24E: {
# 1756
struct _tuple17 _tmp84E;return(_tmp84E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp84E)));}_LL236:;};}
# 1760
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1763
struct _tuple17 _stmttmp5A=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp416;union Cyc_CfFlowInfo_AbsLVal _tmp417;struct _tuple17 _tmp415=_stmttmp5A;_tmp416=_tmp415.f1;_tmp417=_tmp415.f2;{
struct _tuple17 _tmp84F;return(_tmp84F.f1=_tmp416,((_tmp84F.f2=_tmp417,_tmp84F)));};}
# 1770
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1772
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _stmttmp5B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp41A;struct _tuple17 _tmp419=_stmttmp5B;_tmp41A=_tmp419.f1;
inflow=_tmp41A;}}}
# 1779
return inflow;}
# 1785
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1788
struct Cyc_CfFlowInfo_FlowEnv*_tmp41B=env->fenv;
# 1791
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp41C=_new_region("temp");struct _RegionHandle*temp=& _tmp41C;_push_region(temp);{
struct _tuple22 _stmttmp5C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp41E;struct Cyc_List_List*_tmp41F;struct _tuple22 _tmp41D=_stmttmp5C;_tmp41E=_tmp41D.f1;_tmp41F=_tmp41D.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp41F))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp41F->tl))->hd;
f=_tmp41E;}{
# 1801
union Cyc_CfFlowInfo_FlowInfo _tmp420=f;struct Cyc_Dict_Dict _tmp421;struct Cyc_List_List*_tmp422;_LL274: if((_tmp420.BottomFL).tag != 1)goto _LL276;_LL275: {
struct _tuple18 _tmp850;struct _tuple18 _tmp424=(_tmp850.f1=f,((_tmp850.f2=f,_tmp850)));_npop_handler(0);return _tmp424;}_LL276: if((_tmp420.ReachableFL).tag != 2)goto _LL273;_tmp421=((struct _tuple14)(_tmp420.ReachableFL).val).f1;_tmp422=((struct _tuple14)(_tmp420.ReachableFL).val).f2;_LL277: {
# 1804
struct Cyc_Absyn_Exp*_tmp425=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp426=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp421,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp853;void*_tmp852;(_tmp852=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp853="expression may not be initialized",_tag_dyneither(_tmp853,sizeof(char),34))),_tag_dyneither(_tmp852,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp421,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp856="expression may not be initialized",_tag_dyneither(_tmp856,sizeof(char),34))),_tag_dyneither(_tmp855,sizeof(void*),0)));}{
# 1811
union Cyc_CfFlowInfo_FlowInfo _tmp42B=f;
union Cyc_CfFlowInfo_FlowInfo _tmp42C=f;
# 1816
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp42D=r1;struct Cyc_Absyn_Vardecl*_tmp42F;void*_tmp430;_LL279: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp42E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42D;if(_tmp42E->tag != 8)goto _LL27B;else{_tmp42F=_tmp42E->f1;_tmp430=(void*)_tmp42E->f2;}}_LL27A:
 r1=_tmp430;r1n=_tmp42F;goto _LL278;_LL27B:;_LL27C:
 goto _LL278;_LL278:;}
# 1822
{void*_tmp431=r2;struct Cyc_Absyn_Vardecl*_tmp433;void*_tmp434;_LL27E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp432=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp431;if(_tmp432->tag != 8)goto _LL280;else{_tmp433=_tmp432->f1;_tmp434=(void*)_tmp432->f2;}}_LL27F:
 r2=_tmp434;r2n=_tmp433;goto _LL27D;_LL280:;_LL281:
 goto _LL27D;_LL27D:;}{
# 1827
struct _tuple0 _tmp857;struct _tuple0 _stmttmp5D=(_tmp857.f1=r1,((_tmp857.f2=r2,_tmp857)));struct _tuple0 _tmp435=_stmttmp5D;enum Cyc_CfFlowInfo_InitLevel _tmp437;enum Cyc_CfFlowInfo_InitLevel _tmp43B;_LL283:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp436=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp436->tag != 3)goto _LL285;else{_tmp437=_tmp436->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp438=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp438->tag != 0)goto _LL285;};_LL284: {
# 1831
struct _tuple18 _stmttmp5F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp425,_tmp437,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp44B;union Cyc_CfFlowInfo_FlowInfo _tmp44C;struct _tuple18 _tmp44A=_stmttmp5F;_tmp44B=_tmp44A.f1;_tmp44C=_tmp44A.f2;
switch(p){case Cyc_Absyn_Eq: _LL297:
 _tmp42B=_tmp44C;_tmp42C=_tmp44B;break;case Cyc_Absyn_Neq: _LL298:
 _tmp42B=_tmp44B;_tmp42C=_tmp44C;break;default: _LL299: {
struct Cyc_Core_Impossible_exn_struct _tmp85D;const char*_tmp85C;struct Cyc_Core_Impossible_exn_struct*_tmp85B;(int)_throw((void*)((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85D.tag=Cyc_Core_Impossible,((_tmp85D.f1=((_tmp85C="anal_test, zero-split",_tag_dyneither(_tmp85C,sizeof(char),22))),_tmp85D)))),_tmp85B)))));}}
# 1837
goto _LL282;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp439=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp439->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp43A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp43A->tag != 3)goto _LL287;else{_tmp43B=_tmp43A->f1;}};_LL286: {
# 1839
struct _tuple18 _stmttmp5E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp426,_tmp43B,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp451;union Cyc_CfFlowInfo_FlowInfo _tmp452;struct _tuple18 _tmp450=_stmttmp5E;_tmp451=_tmp450.f1;_tmp452=_tmp450.f2;
switch(p){case Cyc_Absyn_Eq: _LL29B:
 _tmp42B=_tmp452;_tmp42C=_tmp451;break;case Cyc_Absyn_Neq: _LL29C:
 _tmp42B=_tmp451;_tmp42C=_tmp452;break;default: _LL29D: {
struct Cyc_Core_Impossible_exn_struct _tmp863;const char*_tmp862;struct Cyc_Core_Impossible_exn_struct*_tmp861;(int)_throw((void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp863.tag=Cyc_Core_Impossible,((_tmp863.f1=((_tmp862="anal_test, zero-split",_tag_dyneither(_tmp862,sizeof(char),22))),_tmp863)))),_tmp861)))));}}
# 1845
goto _LL282;}_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp43C->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp43D->tag != 0)goto _LL289;};_LL288:
# 1847
 if(p == Cyc_Absyn_Eq)_tmp42C=Cyc_CfFlowInfo_BottomFL();else{
_tmp42B=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp43E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp43E->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp43F=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp43F->tag != 1)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp440->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp441=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp441->tag != 2)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp442=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp442->tag != 0)goto _LL28F;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp443=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp443->tag != 5)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp444=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp444->tag != 1)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp445->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp446=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp446->tag != 2)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp447->tag != 0)goto _LL293;};_LL292:
 goto _LL294;_LL293:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp435.f1;if(_tmp448->tag != 5)goto _LL295;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp435.f2;if(_tmp449->tag != 0)goto _LL295;};_LL294:
# 1856
 if(p == Cyc_Absyn_Neq)_tmp42C=Cyc_CfFlowInfo_BottomFL();else{
_tmp42B=Cyc_CfFlowInfo_BottomFL();}
goto _LL282;_LL295:;_LL296:
 goto _LL282;_LL282:;};}
# 1867
{struct _tuple0 _tmp864;struct _tuple0 _stmttmp60=(_tmp864.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp425->topt)),((_tmp864.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp426->topt)),_tmp864)));struct _tuple0 _tmp457=_stmttmp60;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1;if(_tmp458->tag != 6)goto _LL2A2;else{if(_tmp458->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2;if(_tmp459->tag != 6)goto _LL2A4;else{if(_tmp459->f1 != Cyc_Absyn_Unsigned)goto _LL2A4;}}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f1;if(_tmp45A->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL2A7;_LL2A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f2;if(_tmp45B->tag != 19)goto _LL2A8;}_LL2A7:
 goto _LL29F;_LL2A8:;_LL2A9: {
struct _tuple18 _tmp865;struct _tuple18 _tmp45D=(_tmp865.f1=_tmp42B,((_tmp865.f2=_tmp42C,_tmp865)));_npop_handler(0);return _tmp45D;}_LL29F:;}
# 1876
switch(p){case Cyc_Absyn_Eq: _LL2AA:
 goto _LL2AB;case Cyc_Absyn_Neq: _LL2AB: goto _LL2AC;case Cyc_Absyn_Gt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Gte: _LL2AD: goto _LL2AE;case Cyc_Absyn_Lt: _LL2AE: goto _LL2AF;case Cyc_Absyn_Lte: _LL2AF: break;default: _LL2B0: {
struct _tuple18 _tmp866;struct _tuple18 _tmp460=(_tmp866.f1=_tmp42B,((_tmp866.f2=_tmp42C,_tmp866)));_npop_handler(0);return _tmp460;}}{
# 1881
struct _RegionHandle*_tmp461=(env->fenv)->r;
struct _tuple13 _stmttmp61=Cyc_Relations_primop2relation(_tmp425,p,_tmp426);struct Cyc_Absyn_Exp*_tmp463;enum Cyc_Relations_Relation _tmp464;struct Cyc_Absyn_Exp*_tmp465;struct _tuple13 _tmp462=_stmttmp61;_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_tmp465=_tmp462.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1886
if(Cyc_Relations_exp2relnop(_tmp463,& n1) && Cyc_Relations_exp2relnop(_tmp465,& n2)){
# 1888
struct Cyc_List_List*_tmp466=Cyc_Relations_add_relation(_tmp461,n1,_tmp464,n2,_tmp422);
# 1892
struct Cyc_List_List*_tmp467=Cyc_Relations_add_relation(_tmp461,n2,Cyc_Relations_flip_relation(_tmp464),n1,_tmp422);
struct _tuple18 _tmp867;struct _tuple18 _stmttmp62=(_tmp867.f1=_tmp42B,((_tmp867.f2=_tmp42C,_tmp867)));struct _tuple18 _tmp468=_stmttmp62;struct Cyc_Dict_Dict _tmp469;struct Cyc_Dict_Dict _tmp46A;struct Cyc_Dict_Dict _tmp46B;struct Cyc_Dict_Dict _tmp46C;_LL2B3: if(((_tmp468.f1).ReachableFL).tag != 2)goto _LL2B5;_tmp469=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f1;if(((_tmp468.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp46A=((struct _tuple14)((_tmp468.f2).ReachableFL).val).f1;_LL2B4: {
# 1895
struct _tuple18 _tmp868;struct _tuple18 _tmp46E=(_tmp868.f1=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp466),((_tmp868.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46A,_tmp467),_tmp868)));_npop_handler(0);return _tmp46E;}_LL2B5: if(((_tmp468.f2).ReachableFL).tag != 2)goto _LL2B7;_tmp46B=((struct _tuple14)((_tmp468.f2).ReachableFL).val).f1;_LL2B6: {
# 1897
struct _tuple18 _tmp869;struct _tuple18 _tmp470=(_tmp869.f1=_tmp42B,((_tmp869.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46B,_tmp467),_tmp869)));_npop_handler(0);return _tmp470;}_LL2B7: if(((_tmp468.f1).ReachableFL).tag != 2)goto _LL2B9;_tmp46C=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f1;_LL2B8: {
# 1899
struct _tuple18 _tmp86A;struct _tuple18 _tmp472=(_tmp86A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46C,_tmp466),((_tmp86A.f2=_tmp42C,_tmp86A)));_npop_handler(0);return _tmp472;}_LL2B9:;_LL2BA: {
# 1901
struct _tuple18 _tmp86B;struct _tuple18 _tmp474=(_tmp86B.f1=_tmp42B,((_tmp86B.f2=_tmp42C,_tmp86B)));_npop_handler(0);return _tmp474;}_LL2B2:;}else{
# 1904
struct _tuple18 _tmp86C;struct _tuple18 _tmp477=(_tmp86C.f1=_tmp42B,((_tmp86C.f2=_tmp42C,_tmp86C)));_npop_handler(0);return _tmp477;}};};};}_LL273:;};
# 1793
;_pop_region(temp);}
# 1910
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1912
struct Cyc_CfFlowInfo_FlowEnv*_tmp478=env->fenv;
void*_stmttmp63=e->r;void*_tmp479=_stmttmp63;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp488;enum Cyc_Absyn_Primop _tmp48A;struct Cyc_List_List*_tmp48B;_LL2BC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp47A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp47A->tag != 5)goto _LL2BE;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;_tmp47D=_tmp47A->f3;}}_LL2BD: {
# 1915
struct _tuple18 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47B);union Cyc_CfFlowInfo_FlowInfo _tmp48D;union Cyc_CfFlowInfo_FlowInfo _tmp48E;struct _tuple18 _tmp48C=_stmttmp6B;_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;{
struct _tuple18 _stmttmp6C=Cyc_NewControlFlow_anal_test(env,_tmp48D,_tmp47C);union Cyc_CfFlowInfo_FlowInfo _tmp490;union Cyc_CfFlowInfo_FlowInfo _tmp491;struct _tuple18 _tmp48F=_stmttmp6C;_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;{
struct _tuple18 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,_tmp48E,_tmp47D);union Cyc_CfFlowInfo_FlowInfo _tmp493;union Cyc_CfFlowInfo_FlowInfo _tmp494;struct _tuple18 _tmp492=_stmttmp6D;_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;{
struct _tuple18 _tmp86D;return(_tmp86D.f1=Cyc_CfFlowInfo_join_flow(_tmp478,env->all_changed,_tmp490,_tmp493),((_tmp86D.f2=
Cyc_CfFlowInfo_join_flow(_tmp478,env->all_changed,_tmp491,_tmp494),_tmp86D)));};};};}_LL2BE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp47E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp47E->tag != 6)goto _LL2C0;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;}}_LL2BF: {
# 1921
struct _tuple18 _stmttmp69=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47F);union Cyc_CfFlowInfo_FlowInfo _tmp497;union Cyc_CfFlowInfo_FlowInfo _tmp498;struct _tuple18 _tmp496=_stmttmp69;_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;{
struct _tuple18 _stmttmp6A=Cyc_NewControlFlow_anal_test(env,_tmp497,_tmp480);union Cyc_CfFlowInfo_FlowInfo _tmp49A;union Cyc_CfFlowInfo_FlowInfo _tmp49B;struct _tuple18 _tmp499=_stmttmp6A;_tmp49A=_tmp499.f1;_tmp49B=_tmp499.f2;{
struct _tuple18 _tmp86E;return(_tmp86E.f1=_tmp49A,((_tmp86E.f2=Cyc_CfFlowInfo_join_flow(_tmp478,env->all_changed,_tmp498,_tmp49B),_tmp86E)));};};}_LL2C0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp481=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp481->tag != 7)goto _LL2C2;else{_tmp482=_tmp481->f1;_tmp483=_tmp481->f2;}}_LL2C1: {
# 1925
struct _tuple18 _stmttmp67=Cyc_NewControlFlow_anal_test(env,inflow,_tmp482);union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple18 _tmp49D=_stmttmp67;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{
struct _tuple18 _stmttmp68=Cyc_NewControlFlow_anal_test(env,_tmp49F,_tmp483);union Cyc_CfFlowInfo_FlowInfo _tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A2;struct _tuple18 _tmp4A0=_stmttmp68;_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;{
struct _tuple18 _tmp86F;return(_tmp86F.f1=Cyc_CfFlowInfo_join_flow(_tmp478,env->all_changed,_tmp49E,_tmp4A1),((_tmp86F.f2=_tmp4A2,_tmp86F)));};};}_LL2C2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp484=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp484->tag != 8)goto _LL2C4;else{_tmp485=_tmp484->f1;_tmp486=_tmp484->f2;}}_LL2C3: {
# 1929
struct _tuple15 _stmttmp66=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp485);union Cyc_CfFlowInfo_FlowInfo _tmp4A5;void*_tmp4A6;struct _tuple15 _tmp4A4=_stmttmp66;_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4A5,_tmp486);}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp487=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp487->tag != 2)goto _LL2C6;else{if(_tmp487->f1 != Cyc_Absyn_Not)goto _LL2C6;if(_tmp487->f2 == 0)goto _LL2C6;_tmp488=(struct Cyc_Absyn_Exp*)(_tmp487->f2)->hd;if((_tmp487->f2)->tl != 0)goto _LL2C6;}}_LL2C5: {
# 1932
struct _tuple18 _stmttmp65=Cyc_NewControlFlow_anal_test(env,inflow,_tmp488);union Cyc_CfFlowInfo_FlowInfo _tmp4A8;union Cyc_CfFlowInfo_FlowInfo _tmp4A9;struct _tuple18 _tmp4A7=_stmttmp65;_tmp4A8=_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;{
struct _tuple18 _tmp870;return(_tmp870.f1=_tmp4A9,((_tmp870.f2=_tmp4A8,_tmp870)));};}_LL2C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp489=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479;if(_tmp489->tag != 2)goto _LL2C8;else{_tmp48A=_tmp489->f1;_tmp48B=_tmp489->f2;}}_LL2C7:
# 1935
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp48A,_tmp48B);_LL2C8:;_LL2C9: {
# 1939
struct _tuple15 _stmttmp64=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4AC;void*_tmp4AD;struct _tuple15 _tmp4AB=_stmttmp64;_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4AE=_tmp4AC;struct Cyc_Dict_Dict _tmp4AF;_LL2CB: if((_tmp4AE.BottomFL).tag != 1)goto _LL2CD;_LL2CC: {
struct _tuple18 _tmp871;return(_tmp871.f1=_tmp4AC,((_tmp871.f2=_tmp4AC,_tmp871)));}_LL2CD: if((_tmp4AE.ReachableFL).tag != 2)goto _LL2CA;_tmp4AF=((struct _tuple14)(_tmp4AE.ReachableFL).val).f1;_LL2CE: {
# 1943
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4B1=_tmp4AD;struct Cyc_Absyn_Vardecl*_tmp4B3;void*_tmp4B4;enum Cyc_CfFlowInfo_InitLevel _tmp4BD;_LL2D0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B2->tag != 8)goto _LL2D2;else{_tmp4B3=_tmp4B2->f1;_tmp4B4=(void*)_tmp4B2->f2;}}_LL2D1:
 _tmp4AD=_tmp4B4;locname=_tmp4B3;goto retry;_LL2D2: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B5->tag != 0)goto _LL2D4;}_LL2D3: {
struct _tuple18 _tmp872;return(_tmp872.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp872.f2=_tmp4AC,_tmp872)));}_LL2D4: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4B6=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B6->tag != 2)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4B7=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B7->tag != 1)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4B8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B8->tag != 5)goto _LL2DA;}_LL2D9: {
struct _tuple18 _tmp873;return(_tmp873.f1=_tmp4AC,((_tmp873.f2=Cyc_CfFlowInfo_BottomFL(),_tmp873)));}_LL2DA: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4B9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4B9->tag != 3)goto _LL2DC;else{if(_tmp4B9->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4BA=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4BA->tag != 4)goto _LL2DE;else{if(_tmp4BA->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DE;}}_LL2DD:
 goto _LL2DF;_LL2DE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4BB=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4BB->tag != 7)goto _LL2E0;}_LL2DF:
# 1953
{const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_terr(e->loc,((_tmp876="expression may not be initialized",_tag_dyneither(_tmp876,sizeof(char),34))),_tag_dyneither(_tmp875,sizeof(void*),0)));}{
struct _tuple18 _tmp877;return(_tmp877.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp877.f2=Cyc_CfFlowInfo_BottomFL(),_tmp877)));};_LL2E0: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4BC->tag != 3)goto _LL2E2;else{_tmp4BD=_tmp4BC->f1;}}_LL2E1:
# 1956
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4AC,e,_tmp4BD,locname);_LL2E2: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4BE->tag != 4)goto _LL2E4;}_LL2E3: {
struct _tuple18 _tmp878;return(_tmp878.f1=_tmp4AC,((_tmp878.f2=_tmp4AC,_tmp878)));}_LL2E4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4BF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B1;if(_tmp4BF->tag != 6)goto _LL2CF;}_LL2E5: {
struct Cyc_Core_Impossible_exn_struct _tmp87E;const char*_tmp87D;struct Cyc_Core_Impossible_exn_struct*_tmp87C;(int)_throw((void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp87E.tag=Cyc_Core_Impossible,((_tmp87E.f1=((_tmp87D="anal_test",_tag_dyneither(_tmp87D,sizeof(char),10))),_tmp87E)))),_tmp87C)))));}_LL2CF:;}}_LL2CA:;};}_LL2BB:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1964
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 1966
unsigned int _tmp4CA;struct Cyc_NewControlFlow_AnalEnv*_tmp4CB;struct Cyc_Dict_Dict _tmp4CC;struct _tuple26*_tmp4C9=ckenv;_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;_tmp4CC=_tmp4C9->f3;{
void*_tmp4CD=root;struct Cyc_Absyn_Vardecl*_tmp4CF;_LL2E7: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4CE=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CD;if(_tmp4CE->tag != 0)goto _LL2E9;else{_tmp4CF=_tmp4CE->f1;}}_LL2E8:
# 1969
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4CB->fenv)->r,_tmp4CF->type)){
void*_tmp4D0=rval;_LL2EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D1->tag != 8)goto _LL2EE;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D1->f2);if(_tmp4D2->tag != 7)goto _LL2EE;}}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D3->tag != 8)goto _LL2F0;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D3->f2);if(_tmp4D4->tag != 0)goto _LL2F0;}}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D5->tag != 8)goto _LL2F2;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4D5->f2);if(_tmp4D6->tag != 3)goto _LL2F2;else{if(_tmp4D6->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F2;}}}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D7->tag != 7)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D8->tag != 0)goto _LL2F6;}_LL2F5:
 goto _LL2F7;_LL2F6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D0;if(_tmp4D9->tag != 3)goto _LL2F8;else{if(_tmp4D9->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F8;}}_LL2F7:
 goto _LL2EB;_LL2F8:;_LL2F9:
# 1978
{const char*_tmp882;void*_tmp881[1];struct Cyc_String_pa_PrintArg_struct _tmp880;(_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4CF->name)),((_tmp881[0]=& _tmp880,Cyc_Tcutil_warn(_tmp4CA,((_tmp882="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp882,sizeof(char),57))),_tag_dyneither(_tmp881,sizeof(void*),1)))))));}
goto _LL2EB;_LL2EB:;}
# 1981
goto _LL2E6;_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;};}
# 1986
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4DD=flow;struct Cyc_Dict_Dict _tmp4DE;_LL2FB: if((_tmp4DD.BottomFL).tag != 1)goto _LL2FD;_LL2FC:
 return;_LL2FD: if((_tmp4DD.ReachableFL).tag != 2)goto _LL2FA;_tmp4DE=((struct _tuple14)(_tmp4DD.ReachableFL).val).f1;_LL2FE:
# 1990
{struct Cyc_List_List*_tmp4DF=env->param_roots;for(0;_tmp4DF != 0;_tmp4DF=_tmp4DF->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4DE,Cyc_CfFlowInfo_lookup_place(_tmp4DE,(struct Cyc_CfFlowInfo_Place*)_tmp4DF->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp885;void*_tmp884;(_tmp884=0,Cyc_Tcutil_terr(loc,((_tmp885="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp885,sizeof(char),76))),_tag_dyneither(_tmp884,sizeof(void*),0)));}}}
# 1994
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp886;struct _tuple26 _tmp4E2=(_tmp886.f1=loc,((_tmp886.f2=env,((_tmp886.f3=_tmp4DE,_tmp886)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4E2,_tmp4DE);}
# 1998
return;_LL2FA:;}
# 2002
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2005
struct Cyc_List_List*_tmp4E4=0;
struct Cyc_List_List*_tmp4E5=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp6E=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct _tuple23*_tmp4E6=_stmttmp6E;_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;
if((_tmp4E7 != 0  && 
# 2012
Cyc_Tcutil_is_noalias_pointer((*_tmp4E7)->type,0)) && _tmp4E8 != 0){
# 2014
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp889;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp888;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E9=(_tmp888=_region_malloc(frgn,sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=0,((_tmp889.f1=*_tmp4E7,_tmp889)))),_tmp888)));
struct Cyc_CfFlowInfo_Place*_tmp88A;struct Cyc_CfFlowInfo_Place*_tmp4EA=(_tmp88A=_region_malloc(frgn,sizeof(*_tmp88A)),((_tmp88A->root=(void*)_tmp4E9,((_tmp88A->fields=0,_tmp88A)))));
{struct Cyc_List_List*_tmp88B;_tmp4E4=((_tmp88B=_region_malloc(frgn,sizeof(*_tmp88B)),((_tmp88B->hd=_tmp4EA,((_tmp88B->tl=_tmp4E4,_tmp88B))))));}{
struct Cyc_List_List*_tmp88C;_tmp4E5=((_tmp88C=_region_malloc(frgn,sizeof(*_tmp88C)),((_tmp88C->hd=_tmp4E8,((_tmp88C->tl=_tmp4E5,_tmp88C))))));};}}}{
# 2020
struct _tuple1 _tmp88D;return(_tmp88D.f1=_tmp4E4,((_tmp88D.f2=_tmp4E5,_tmp88D)));};}
# 2029
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2034
{union Cyc_CfFlowInfo_FlowInfo _tmp4F1=flow;_LL300: if((_tmp4F1.BottomFL).tag != 1)goto _LL302;_LL301:
 return flow;_LL302:;_LL303:
 goto _LL2FF;_LL2FF:;}{
# 2038
int _tmp4F2=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp4F3=Cyc_Absyn_exp_unsigned_one;
int _tmp4F4=0;
int _tmp4F5=1;
# 2045
{struct Cyc_List_List*_tmp4F6=consumed_vals;for(0;_tmp4F6 != 0;
(_tmp4F6=_tmp4F6->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp4F7=flow;struct Cyc_Dict_Dict _tmp4F8;_LL305: if((_tmp4F7.ReachableFL).tag != 2)goto _LL307;_tmp4F8=((struct _tuple14)(_tmp4F7.ReachableFL).val).f1;_LL306: {
# 2052
void*_tmp4F9=Cyc_CfFlowInfo_lookup_place(_tmp4F8,(struct Cyc_CfFlowInfo_Place*)_tmp4F6->hd);
struct Cyc_CfFlowInfo_Place*_stmttmp6F=(struct Cyc_CfFlowInfo_Place*)_tmp4F6->hd;void*_tmp4FB;struct Cyc_CfFlowInfo_Place*_tmp4FA=_stmttmp6F;_tmp4FB=_tmp4FA->root;
{struct _tuple0 _tmp88E;struct _tuple0 _stmttmp70=(_tmp88E.f1=_tmp4FB,((_tmp88E.f2=_tmp4F9,_tmp88E)));struct _tuple0 _tmp4FC=_stmttmp70;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp500;void*_tmp501;struct Cyc_Absyn_Vardecl*_tmp503;_LL30A:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FD=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FC.f1;if(_tmp4FD->tag != 0)goto _LL30C;else{_tmp4FE=_tmp4FD->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4FF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FC.f2;if(_tmp4FF->tag != 8)goto _LL30C;else{_tmp500=_tmp4FF->f1;_tmp501=(void*)_tmp4FF->f2;}};_LL30B:
# 2058
 if(_tmp4FE == _tmp500){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4F3,_tmp4F5,_tmp501,& _tmp4F4)){
if(!_tmp4F2){
const char*_tmp892;void*_tmp891[1];struct Cyc_String_pa_PrintArg_struct _tmp890;(_tmp890.tag=0,((_tmp890.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2063
Cyc_Absynpp_qvar2string(_tmp4FE->name)),((_tmp891[0]=& _tmp890,Cyc_Tcutil_terr(loc,((_tmp892="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp892,sizeof(char),58))),_tag_dyneither(_tmp891,sizeof(void*),1)))))));}}else{
# 2065
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4F8,_tmp501)!= Cyc_CfFlowInfo_AllIL  && !_tmp4F2){
const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_PrintArg_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2068
Cyc_Absynpp_qvar2string(_tmp4FE->name)),((_tmp895[0]=& _tmp894,Cyc_Tcutil_terr(loc,((_tmp896="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp896,sizeof(char),84))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}else{
# 2070
if(_tmp4F2){
# 2076
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_Lexp(env,flow,1,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp50B;union Cyc_CfFlowInfo_AbsLVal _tmp50C;struct _tuple17 _tmp50A=_stmttmp71;_tmp50B=_tmp50A.f1;_tmp50C=_tmp50A.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp50D=_tmp50C;struct Cyc_CfFlowInfo_Place*_tmp50E;_LL311: if((_tmp50D.PlaceL).tag != 1)goto _LL313;_tmp50E=(struct Cyc_CfFlowInfo_Place*)(_tmp50D.PlaceL).val;_LL312: {
# 2080
void*_tmp50F=Cyc_CfFlowInfo_lookup_place(_tmp4F8,_tmp50E);
_tmp50F=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp50F);{
# 2088
struct _tuple15 _tmp898;struct _tuple15 _tmp897;struct _tuple15 _stmttmp72=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2091
(_tmp897.f1=_tmp50B,((_tmp897.f2=_tmp50F,_tmp897)))),(
(_tmp898.f1=_tmp50B,((_tmp898.f2=_tmp501,_tmp898)))));
# 2088
union Cyc_CfFlowInfo_FlowInfo _tmp511;void*_tmp512;struct _tuple15 _tmp510=_stmttmp72;_tmp511=_tmp510.f1;_tmp512=_tmp510.f2;
# 2093
{union Cyc_CfFlowInfo_FlowInfo _tmp513=_tmp511;struct Cyc_Dict_Dict _tmp514;struct Cyc_List_List*_tmp515;_LL316: if((_tmp513.ReachableFL).tag != 2)goto _LL318;_tmp514=((struct _tuple14)(_tmp513.ReachableFL).val).f1;_tmp515=((struct _tuple14)(_tmp513.ReachableFL).val).f2;_LL317:
# 2095
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp514,env->all_changed,_tmp50E,_tmp512),_tmp515);
# 2099
goto _LL315;_LL318:;_LL319: {
# 2101
const char*_tmp89B;void*_tmp89A;(_tmp89A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp89B="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp89B,sizeof(char),42))),_tag_dyneither(_tmp89A,sizeof(void*),0)));}_LL315:;}
# 2103
goto _LL310;};}_LL313:;_LL314: {
# 2105
const char*_tmp89E;void*_tmp89D;(_tmp89D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp89E="unconsume_params: unconsume_exp not an lval!",_tag_dyneither(_tmp89E,sizeof(char),45))),_tag_dyneither(_tmp89D,sizeof(void*),0)));}_LL310:;};}}}}else{
# 2109
_tmp503=_tmp4FE;goto _LL30D;}
goto _LL309;_LL30C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp502=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FC.f1;if(_tmp502->tag != 0)goto _LL30E;else{_tmp503=_tmp502->f1;}}_LL30D:
# 2112
 if(!_tmp4F2){
const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_PrintArg_struct _tmp8A0;(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2115
Cyc_Absynpp_qvar2string(_tmp503->name)),((_tmp8A1[0]=& _tmp8A0,Cyc_Tcutil_terr(loc,((_tmp8A2="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8A2,sizeof(char),85))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));}
goto _LL309;_LL30E:;_LL30F: {
# 2118
const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A5="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8A5,sizeof(char),40))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}_LL309:;}
# 2120
goto _LL304;}_LL307: if((_tmp4F7.BottomFL).tag != 1)goto _LL304;_LL308: {
# 2122
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A8="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp8A8,sizeof(char),38))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}_LL304:;}}
# 2125
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2128
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2131
struct Cyc_CfFlowInfo_FlowEnv*_tmp524=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp73=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Stmt*_tmp528;unsigned int _tmp529;struct Cyc_Absyn_Switch_clause*_tmp525=_stmttmp73;_tmp526=_tmp525->pat_vars;_tmp527=_tmp525->where_clause;_tmp528=_tmp525->body;_tmp529=_tmp525->loc;{
struct _tuple1 _stmttmp74=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp526))->v);struct Cyc_List_List*_tmp52B;struct Cyc_List_List*_tmp52C;struct _tuple1 _tmp52A=_stmttmp74;_tmp52B=_tmp52A.f1;_tmp52C=_tmp52A.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp526->v,_tmp52B != 0,_tmp529,exp_loc);
# 2139
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp52D=env->unique_pat_vars;
{struct _tuple27*_tmp8AB;struct Cyc_List_List*_tmp8AA;env->unique_pat_vars=(
(_tmp8AA=_region_malloc(env->r,sizeof(*_tmp8AA)),((_tmp8AA->hd=((_tmp8AB=_region_malloc(env->r,sizeof(*_tmp8AB)),((_tmp8AB->f1=_tmp528,((_tmp8AB->f2=_tmp52B,((_tmp8AB->f3=_tmp52C,_tmp8AB)))))))),((_tmp8AA->tl=_tmp52D,_tmp8AA))))));}
# 2144
if(_tmp527 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp527;
struct _tuple18 _stmttmp75=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp531;union Cyc_CfFlowInfo_FlowInfo _tmp532;struct _tuple18 _tmp530=_stmttmp75;_tmp531=_tmp530.f1;_tmp532=_tmp530.f2;
inflow=_tmp532;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp531,_tmp528,0);}else{
# 2150
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp528,0);}
# 2152
env->unique_pat_vars=_tmp52D;{
union Cyc_CfFlowInfo_FlowInfo _tmp533=clause_outflow;_LL31B: if((_tmp533.BottomFL).tag != 1)goto _LL31D;_LL31C:
 goto _LL31A;_LL31D:;_LL31E:
# 2157
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp52B,_tmp52C,clause_outflow,_tmp529);
# 2159
if(scs->tl == 0)
return clause_outflow;else{
# 2164
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,Cyc_Tcutil_terr(_tmp528->loc,((_tmp8AE="switch clause may implicitly fallthru",_tag_dyneither(_tmp8AE,sizeof(char),38))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}else{
# 2167
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,Cyc_Tcutil_warn(_tmp528->loc,((_tmp8B1="switch clause may implicitly fallthru",_tag_dyneither(_tmp8B1,sizeof(char),38))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}
# 2169
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2171
goto _LL31A;_LL31A:;};};};}
# 2174
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2179
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2181
struct Cyc_NewControlFlow_AnalEnv*_tmp539;struct Cyc_Dict_Dict _tmp53A;unsigned int _tmp53B;struct _tuple28*_tmp538=vdenv;_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;_tmp53B=_tmp538->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp539->fenv)->r,vd->type)){
# 2184
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8B2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8B2.tag=0,((_tmp8B2.f1=vd,_tmp8B2)));
# 2186
void*_tmp53C=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp53A,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp53D=_tmp53C;_LL320: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp53E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp53E->tag != 8)goto _LL322;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp53F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp53E->f2);if(_tmp53F->tag != 7)goto _LL322;}}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp540=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp540->tag != 8)goto _LL324;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp541=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp540->f2);if(_tmp541->tag != 0)goto _LL324;}}_LL323:
 goto _LL325;_LL324: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp542=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp542->tag != 8)goto _LL326;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp543=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp542->f2);if(_tmp543->tag != 3)goto _LL326;else{if(_tmp543->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}}_LL325:
 goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp544=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp544->tag != 7)goto _LL328;}_LL327:
 goto _LL329;_LL328: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp545=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp545->tag != 0)goto _LL32A;}_LL329:
 goto _LL32B;_LL32A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp546=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp53D;if(_tmp546->tag != 3)goto _LL32C;else{if(_tmp546->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32C;}}_LL32B:
 goto _LL31F;_LL32C:;_LL32D:
# 2195
{const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_String_pa_PrintArg_struct _tmp8B4;(_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8B5[0]=& _tmp8B4,Cyc_Tcutil_warn(_tmp53B,((_tmp8B6="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8B6,sizeof(char),68))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))));}
# 2197
goto _LL31F;_LL31F:;}}
# 2202
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2204
{union Cyc_CfFlowInfo_FlowInfo _tmp54B=inflow;struct Cyc_Dict_Dict _tmp54C;_LL32F: if((_tmp54B.ReachableFL).tag != 2)goto _LL331;_tmp54C=((struct _tuple14)(_tmp54B.ReachableFL).val).f1;_LL330: {
# 2206
struct _tuple28 _tmp8B7;struct _tuple28 _tmp54D=(_tmp8B7.f1=env,((_tmp8B7.f2=_tmp54C,((_tmp8B7.f3=decl->loc,_tmp8B7)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp54E=env->fenv;
{void*_stmttmp76=decl->r;void*_tmp54F=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp551;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp555;_LL334: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp550=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp54F;if(_tmp550->tag != 0)goto _LL336;else{_tmp551=_tmp550->f1;}}_LL335:
# 2210
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp54D,_tmp551);
goto _LL333;_LL336: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp552=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp54F;if(_tmp552->tag != 2)goto _LL338;else{if(_tmp552->f2 == 0)goto _LL338;_tmp553=(struct Cyc_List_List*)(_tmp552->f2)->v;}}_LL337: {
# 2213
struct _tuple1 _stmttmp77=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp553);struct Cyc_List_List*_tmp557;struct _tuple1 _tmp556=_stmttmp77;_tmp557=_tmp556.f1;{
struct Cyc_List_List*_tmp558=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp557);
_tmp555=_tmp558;goto _LL339;};}_LL338: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp554=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp54F;if(_tmp554->tag != 3)goto _LL33A;else{_tmp555=_tmp554->f1;}}_LL339:
# 2217
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp54D,_tmp555);
goto _LL333;_LL33A:;_LL33B:
 goto _LL333;_LL333:;}
# 2221
goto _LL32E;}_LL331:;_LL332:
 goto _LL32E;_LL32E:;}
# 2224
return;}
# 2230
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2233
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_stmttmp78=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp55B;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*_tmp55D;struct _tuple27*_tmp55A=_stmttmp78;_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;_tmp55D=_tmp55A->f3;
keep_going=0;{
# 2241
struct Cyc_Absyn_Stmt*_tmp55E=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp55E != _tmp55B){
struct Cyc_Absyn_Stmt*_tmp55F=(Cyc_NewControlFlow_get_stmt_annot(_tmp55E))->encloser;
if(_tmp55F == _tmp55E){
# 2247
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp55C,_tmp55D,inflow,dest->loc);
keep_going=1;
break;}
# 2251
_tmp55E=_tmp55F;}};}}
# 2254
return inflow;}
# 2260
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2263
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _stmttmp79=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp561;union Cyc_CfFlowInfo_FlowInfo*_tmp562;struct _tuple19 _tmp560=_stmttmp79;_tmp561=_tmp560.f1;_tmp562=_tmp560.f2;
inflow=*_tmp562;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp563=env->fenv;
# 2270
void*_stmttmp7A=s->r;void*_tmp564=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Stmt*_tmp56C;struct Cyc_Absyn_Stmt*_tmp56D;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Stmt*_tmp571;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Stmt*_tmp574;struct Cyc_Absyn_Stmt*_tmp575;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Exp*_tmp578;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_Absyn_Exp*_tmp57B;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_Absyn_Stmt*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Stmt*_tmp580;struct Cyc_List_List*_tmp583;struct Cyc_Absyn_Switch_clause*_tmp584;struct Cyc_Absyn_Stmt*_tmp586;struct Cyc_Absyn_Stmt*_tmp588;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_List_List*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58F;struct Cyc_List_List*_tmp590;struct Cyc_List_List*_tmp593;struct Cyc_Absyn_Exp*_tmp594;unsigned int _tmp595;struct Cyc_Absyn_Stmt*_tmp596;struct Cyc_Absyn_Tvar*_tmp599;struct Cyc_Absyn_Vardecl*_tmp59A;struct Cyc_Absyn_Stmt*_tmp59B;struct Cyc_Absyn_Decl*_tmp59D;struct Cyc_Absyn_Stmt*_tmp59E;struct Cyc_Absyn_Stmt*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A2;_LL33D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp565=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp565->tag != 0)goto _LL33F;}_LL33E:
 return inflow;_LL33F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp566=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp566->tag != 3)goto _LL341;else{if(_tmp566->f1 != 0)goto _LL341;}}_LL340:
# 2273
 if(env->noreturn){
const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,Cyc_Tcutil_terr(s->loc,((_tmp8BA="`noreturn' function might return",_tag_dyneither(_tmp8BA,sizeof(char),33))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL341: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp567=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp567->tag != 3)goto _LL343;else{_tmp568=_tmp567->f1;}}_LL342:
# 2279
 if(env->noreturn){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,Cyc_Tcutil_terr(s->loc,((_tmp8BD="`noreturn' function might return",_tag_dyneither(_tmp8BD,sizeof(char),33))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp568));union Cyc_CfFlowInfo_FlowInfo _tmp5A8;void*_tmp5A9;struct _tuple15 _tmp5A7=_stmttmp8D;_tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;
_tmp5A8=Cyc_NewControlFlow_use_Rval(env,_tmp568->loc,_tmp5A8,_tmp5A9);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5A8);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp5A8,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL343: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp569->tag != 1)goto _LL345;else{_tmp56A=_tmp569->f1;}}_LL344: {
# 2288
struct _tuple16*_tmp5AA=rval_opt;void**_tmp5AB;int _tmp5AC;_LL36A: if(_tmp5AA == 0)goto _LL36C;_tmp5AB=(void**)& _tmp5AA->f1;_tmp5AC=_tmp5AA->f2;_LL36B: {
# 2290
struct _tuple15 _stmttmp8C=Cyc_NewControlFlow_anal_Rexp(env,_tmp5AC,inflow,_tmp56A);union Cyc_CfFlowInfo_FlowInfo _tmp5AE;void*_tmp5AF;struct _tuple15 _tmp5AD=_stmttmp8C;_tmp5AE=_tmp5AD.f1;_tmp5AF=_tmp5AD.f2;
*_tmp5AB=_tmp5AF;
return _tmp5AE;}_LL36C: if(_tmp5AA != 0)goto _LL369;_LL36D:
# 2294
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp56A)).f1;_LL369:;}_LL345: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp56B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp56B->tag != 2)goto _LL347;else{_tmp56C=_tmp56B->f1;_tmp56D=_tmp56B->f2;}}_LL346:
# 2299
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp56C,0),_tmp56D,rval_opt);_LL347: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp56E=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp56E->tag != 4)goto _LL349;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;_tmp571=_tmp56E->f3;}}_LL348: {
# 2302
struct _tuple18 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp56F);union Cyc_CfFlowInfo_FlowInfo _tmp5B1;union Cyc_CfFlowInfo_FlowInfo _tmp5B2;struct _tuple18 _tmp5B0=_stmttmp8B;_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;{
# 2309
union Cyc_CfFlowInfo_FlowInfo _tmp5B3=Cyc_NewControlFlow_anal_stmt(env,_tmp5B2,_tmp571,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5B4=Cyc_NewControlFlow_anal_stmt(env,_tmp5B1,_tmp570,0);
return Cyc_CfFlowInfo_join_flow(_tmp563,env->all_changed,_tmp5B4,_tmp5B3);};}_LL349: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp572=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp572->tag != 5)goto _LL34B;else{_tmp573=(_tmp572->f1).f1;_tmp574=(_tmp572->f1).f2;_tmp575=_tmp572->f2;}}_LL34A: {
# 2317
struct _tuple19 _stmttmp89=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp574);union Cyc_CfFlowInfo_FlowInfo*_tmp5B6;struct _tuple19 _tmp5B5=_stmttmp89;_tmp5B6=_tmp5B5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B7=*_tmp5B6;
struct _tuple18 _stmttmp8A=Cyc_NewControlFlow_anal_test(env,_tmp5B7,_tmp573);union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_FlowInfo _tmp5BA;struct _tuple18 _tmp5B8=_stmttmp8A;_tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BB=Cyc_NewControlFlow_anal_stmt(env,_tmp5B9,_tmp575,0);
Cyc_NewControlFlow_update_flow(env,_tmp574,_tmp5BB);
return _tmp5BA;};};}_LL34B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp576=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp576->tag != 14)goto _LL34D;else{_tmp577=_tmp576->f1;_tmp578=(_tmp576->f2).f1;_tmp579=(_tmp576->f2).f2;}}_LL34C: {
# 2327
union Cyc_CfFlowInfo_FlowInfo _tmp5BC=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp577,0);
struct _tuple19 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BC,_tmp579);union Cyc_CfFlowInfo_FlowInfo*_tmp5BE;struct _tuple19 _tmp5BD=_stmttmp87;_tmp5BE=_tmp5BD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BF=*_tmp5BE;
struct _tuple18 _stmttmp88=Cyc_NewControlFlow_anal_test(env,_tmp5BF,_tmp578);union Cyc_CfFlowInfo_FlowInfo _tmp5C1;union Cyc_CfFlowInfo_FlowInfo _tmp5C2;struct _tuple18 _tmp5C0=_stmttmp88;_tmp5C1=_tmp5C0.f1;_tmp5C2=_tmp5C0.f2;
Cyc_NewControlFlow_update_flow(env,_tmp577,_tmp5C1);
return _tmp5C2;};}_LL34D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp57A=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp57A->tag != 9)goto _LL34F;else{_tmp57B=_tmp57A->f1;_tmp57C=(_tmp57A->f2).f1;_tmp57D=(_tmp57A->f2).f2;_tmp57E=(_tmp57A->f3).f1;_tmp57F=(_tmp57A->f3).f2;_tmp580=_tmp57A->f4;}}_LL34E: {
# 2336
union Cyc_CfFlowInfo_FlowInfo _tmp5C3=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp57B)).f1;
struct _tuple19 _stmttmp84=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C3,_tmp57D);union Cyc_CfFlowInfo_FlowInfo*_tmp5C5;struct _tuple19 _tmp5C4=_stmttmp84;_tmp5C5=_tmp5C4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C6=*_tmp5C5;
struct _tuple18 _stmttmp85=Cyc_NewControlFlow_anal_test(env,_tmp5C6,_tmp57C);union Cyc_CfFlowInfo_FlowInfo _tmp5C8;union Cyc_CfFlowInfo_FlowInfo _tmp5C9;struct _tuple18 _tmp5C7=_stmttmp85;_tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CA=Cyc_NewControlFlow_anal_stmt(env,_tmp5C8,_tmp580,0);
struct _tuple19 _stmttmp86=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CA,_tmp57F);union Cyc_CfFlowInfo_FlowInfo*_tmp5CC;struct _tuple19 _tmp5CB=_stmttmp86;_tmp5CC=_tmp5CB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CD=*_tmp5CC;
union Cyc_CfFlowInfo_FlowInfo _tmp5CE=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5CD,_tmp57E)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp57D,_tmp5CE);
return _tmp5C9;};};};}_LL34F: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp581=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp581->tag != 6)goto _LL351;else{if(_tmp581->f1 != 0)goto _LL351;}}_LL350:
# 2348
 return Cyc_CfFlowInfo_BottomFL();_LL351: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp582=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp582->tag != 11)goto _LL353;else{_tmp583=_tmp582->f1;if(_tmp582->f2 == 0)goto _LL353;_tmp584=*_tmp582->f2;}}_LL352: {
# 2350
struct _RegionHandle _tmp5CF=_new_region("temp");struct _RegionHandle*temp=& _tmp5CF;_push_region(temp);
{struct _tuple22 _stmttmp82=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp583,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5D1;struct Cyc_List_List*_tmp5D2;struct _tuple22 _tmp5D0=_stmttmp82;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;
# 2353
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp584->body);{
# 2355
struct Cyc_List_List*_tmp5D3=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp584->pat_vars))->v)).f1);
_tmp5D1=Cyc_NewControlFlow_add_vars(_tmp563,_tmp5D1,_tmp5D3,_tmp563->unknown_all,s->loc,0);
# 2358
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp584->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_stmttmp83=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D6;struct _tuple23*_tmp5D4=_stmttmp83;_tmp5D5=_tmp5D4->f1;_tmp5D6=_tmp5D4->f2;
if(_tmp5D5 != 0){
_tmp5D1=Cyc_NewControlFlow_do_initialize_var(_tmp563,env,_tmp5D1,*_tmp5D5,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp583))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5D2))->hd,s->loc);
_tmp5D2=_tmp5D2->tl;
_tmp583=_tmp583->tl;}}}
# 2366
Cyc_NewControlFlow_update_flow(env,_tmp584->body,_tmp5D1);{
union Cyc_CfFlowInfo_FlowInfo _tmp5D7=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5D7;};};}
# 2351
;_pop_region(temp);}_LL353: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp585=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp585->tag != 6)goto _LL355;else{_tmp586=_tmp585->f1;}}_LL354:
# 2371
 _tmp588=_tmp586;goto _LL356;_LL355: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp587=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp587->tag != 7)goto _LL357;else{_tmp588=_tmp587->f1;}}_LL356:
 _tmp58A=_tmp588;goto _LL358;_LL357: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp589=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp589->tag != 8)goto _LL359;else{_tmp58A=_tmp589->f2;}}_LL358:
# 2376
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5D8=_tmp561->encloser;
struct Cyc_Absyn_Stmt*_tmp5D9=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp58A)))->encloser;
while(_tmp5D9 != _tmp5D8){
struct Cyc_Absyn_Stmt*_tmp5DA=(Cyc_NewControlFlow_get_stmt_annot(_tmp5D8))->encloser;
if(_tmp5DA == _tmp5D8){
{const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,Cyc_Tcutil_terr(s->loc,((_tmp8C0="goto enters local scope or exception handler",_tag_dyneither(_tmp8C0,sizeof(char),45))),_tag_dyneither(_tmp8BF,sizeof(void*),0)));}
break;}
# 2385
_tmp5D8=_tmp5DA;}}
# 2389
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp58A));
# 2391
Cyc_NewControlFlow_update_flow(env,_tmp58A,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL359: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp58B->tag != 10)goto _LL35B;else{_tmp58C=_tmp58B->f1;_tmp58D=_tmp58B->f2;}}_LL35A:
# 2397
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp58D,_tmp58C->loc);_LL35B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp58E=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp58E->tag != 15)goto _LL35D;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58E->f2;}}_LL35C: {
# 2402
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp58F,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2410
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2413
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp590,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5DD=scs_outflow;_LL36F: if((_tmp5DD.BottomFL).tag != 1)goto _LL371;_LL370:
 goto _LL36E;_LL371:;_LL372: {
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_Tcutil_terr(s->loc,((_tmp8C3="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8C3,sizeof(char),42))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}_LL36E:;}
# 2419
outflow=s1_outflow;
# 2421
return outflow;};};}_LL35D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp591=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp591->tag != 12)goto _LL35F;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp592=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp591->f1)->r;if(_tmp592->tag != 2)goto _LL35F;else{if(_tmp592->f2 == 0)goto _LL35F;_tmp593=(struct Cyc_List_List*)(_tmp592->f2)->v;_tmp594=_tmp592->f3;}}_tmp595=(_tmp591->f1)->loc;_tmp596=_tmp591->f2;}}_LL35E: {
# 2431
struct _tuple1 _stmttmp81=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp593);struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*_tmp5E2;struct _tuple1 _tmp5E0=_stmttmp81;_tmp5E1=_tmp5E0.f1;_tmp5E2=_tmp5E0.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp563,env,inflow,_tmp593,_tmp5E1 != 0,_tmp595,_tmp594->loc);{
struct Cyc_List_List*_tmp5E3=env->unique_pat_vars;
{struct _tuple27*_tmp8C6;struct Cyc_List_List*_tmp8C5;env->unique_pat_vars=(
(_tmp8C5=_region_malloc(env->r,sizeof(*_tmp8C5)),((_tmp8C5->hd=((_tmp8C6=_region_malloc(env->r,sizeof(*_tmp8C6)),((_tmp8C6->f1=_tmp596,((_tmp8C6->f2=_tmp5E1,((_tmp8C6->f3=_tmp5E2,_tmp8C6)))))))),((_tmp8C5->tl=_tmp5E3,_tmp8C5))))));}
# 2439
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp596,rval_opt);
env->unique_pat_vars=_tmp5E3;
# 2443
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E1,_tmp5E2,inflow,_tmp595);
# 2447
return inflow;};}_LL35F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp597=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp597->tag != 12)goto _LL361;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp598=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp597->f1)->r;if(_tmp598->tag != 5)goto _LL361;else{_tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;}}_tmp59B=_tmp597->f2;}}_LL360: {
# 2451
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp59A->initializer);
struct _tuple17 _stmttmp7D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp5E7;union Cyc_CfFlowInfo_AbsLVal _tmp5E8;struct _tuple17 _tmp5E6=_stmttmp7D;_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;{
# 2456
struct _tuple17 _tmp8C7;struct _tuple17 _stmttmp7E=(_tmp8C7.f1=_tmp5E7,((_tmp8C7.f2=_tmp5E8,_tmp8C7)));struct _tuple17 _tmp5E9=_stmttmp7E;struct Cyc_Dict_Dict _tmp5EA;struct Cyc_List_List*_tmp5EB;struct Cyc_CfFlowInfo_Place*_tmp5EC;_LL374: if(((_tmp5E9.f1).ReachableFL).tag != 2)goto _LL376;_tmp5EA=((struct _tuple14)((_tmp5E9.f1).ReachableFL).val).f1;_tmp5EB=((struct _tuple14)((_tmp5E9.f1).ReachableFL).val).f2;if(((_tmp5E9.f2).PlaceL).tag != 1)goto _LL376;_tmp5EC=(struct Cyc_CfFlowInfo_Place*)((_tmp5E9.f2).PlaceL).val;_LL375: {
# 2458
void*_tmp5ED=Cyc_CfFlowInfo_lookup_place(_tmp5EA,_tmp5EC);
void*t=(void*)_check_null(e->topt);
void*_tmp5EE=Cyc_CfFlowInfo_make_unique_consumed(_tmp563,t,e,env->iteration_num,_tmp5ED);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8CA;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8C9;_tmp5EE=(void*)((_tmp8C9=_region_malloc(_tmp563->r,sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CA.tag=8,((_tmp8CA.f1=_tmp59A,((_tmp8CA.f2=_tmp5EE,_tmp8CA)))))),_tmp8C9))));}
_tmp5EA=Cyc_CfFlowInfo_assign_place(_tmp563,e->loc,_tmp5EA,env->all_changed,_tmp5EC,_tmp5EE);
_tmp5E7=Cyc_CfFlowInfo_ReachableFL(_tmp5EA,_tmp5EB);{
# 2471
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp7F=Cyc_CfFlowInfo_initlevel(_tmp563,_tmp5EA,_tmp5ED);switch(_stmttmp7F){case Cyc_CfFlowInfo_AllIL: _LL37A:
 leaf=_tmp563->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL37B:
 leaf=_tmp563->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL37C:
 leaf=_tmp563->unknown_this;break;}}{
# 2477
void*_tmp5F1=Cyc_CfFlowInfo_typ_to_absrval(_tmp563,t,0,leaf);
_tmp5E7=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5E7,_tmp5ED);{
struct Cyc_List_List _tmp8CB;struct Cyc_List_List _tmp5F2=(_tmp8CB.hd=_tmp59A,((_tmp8CB.tl=0,_tmp8CB)));
# 2481
_tmp5E7=Cyc_NewControlFlow_add_vars(_tmp563,_tmp5E7,& _tmp5F2,_tmp563->unknown_all,s->loc,0);
# 2485
_tmp5E7=Cyc_NewControlFlow_anal_stmt(env,_tmp5E7,_tmp59B,rval_opt);{
# 2487
union Cyc_CfFlowInfo_FlowInfo _tmp5F3=_tmp5E7;struct Cyc_Dict_Dict _tmp5F4;struct Cyc_List_List*_tmp5F5;_LL37F: if((_tmp5F3.ReachableFL).tag != 2)goto _LL381;_tmp5F4=((struct _tuple14)(_tmp5F3.ReachableFL).val).f1;_tmp5F5=((struct _tuple14)(_tmp5F3.ReachableFL).val).f2;_LL380:
# 2490
{void*_stmttmp80=Cyc_CfFlowInfo_lookup_place(_tmp5F4,_tmp5EC);void*_tmp5F6=_stmttmp80;struct Cyc_Absyn_Vardecl*_tmp5F8;_LL384: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5F7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F6;if(_tmp5F7->tag != 8)goto _LL386;else{_tmp5F8=_tmp5F7->f1;}}_LL385:
# 2492
 if(_tmp5F8 != _tmp59A)goto _LL387;
goto _LL383;_LL386:;_LL387:
# 2495
{const char*_tmp8CE;void*_tmp8CD;(_tmp8CD=0,Cyc_Tcutil_terr(s->loc,((_tmp8CE="aliased location was overwritten",_tag_dyneither(_tmp8CE,sizeof(char),33))),_tag_dyneither(_tmp8CD,sizeof(void*),0)));}
goto _LL383;_LL383:;}
# 2498
_tmp5F4=Cyc_CfFlowInfo_assign_place(_tmp563,s->loc,_tmp5F4,env->all_changed,_tmp5EC,_tmp5F1);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5F4,_tmp5F5);
# 2503
return outflow;_LL381:;_LL382:
 return _tmp5E7;_LL37E:;};};};};}_LL376: if(((_tmp5E9.f1).BottomFL).tag != 1)goto _LL378;_LL377:
# 2506
 return _tmp5E7;_LL378:;_LL379:
{const char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(e->loc,((_tmp8D1="bad alias expression--no unique path found",_tag_dyneither(_tmp8D1,sizeof(char),43))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL373:;};}_LL361: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp59C->tag != 12)goto _LL363;else{_tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;}}_LL362:
# 2512
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp59D),_tmp59E,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp59D);
return outflow;_LL363: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp59F=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp59F->tag != 13)goto _LL365;else{_tmp5A0=_tmp59F->f2;}}_LL364:
# 2517
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A0,rval_opt);_LL365: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5A1=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp564;if(_tmp5A1->tag != 16)goto _LL367;else{_tmp5A2=_tmp5A1->f1;}}_LL366: {
# 2520
struct _tuple15 _stmttmp7B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A2);union Cyc_CfFlowInfo_FlowInfo _tmp600;void*_tmp601;struct _tuple15 _tmp5FF=_stmttmp7B;_tmp600=_tmp5FF.f1;_tmp601=_tmp5FF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp602=Cyc_NewControlFlow_use_Rval(env,_tmp5A2->loc,_tmp600,_tmp601);
void*_stmttmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp5A2->topt));void*_tmp603=_stmttmp7C;void*_tmp605;_LL389: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp603;if(_tmp604->tag != 15)goto _LL38B;else{_tmp605=(void*)_tmp604->f1;}}_LL38A:
# 2524
 return Cyc_CfFlowInfo_kill_flow_region(_tmp563,_tmp600,_tmp605);_LL38B:;_LL38C: {
struct Cyc_Core_Impossible_exn_struct _tmp8D7;const char*_tmp8D6;struct Cyc_Core_Impossible_exn_struct*_tmp8D5;(int)_throw((void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D7.tag=Cyc_Core_Impossible,((_tmp8D7.f1=((_tmp8D6="anal_stmt -- reset_region",_tag_dyneither(_tmp8D6,sizeof(char),26))),_tmp8D7)))),_tmp8D5)))));}_LL388:;};}_LL367:;_LL368: {
# 2528
struct Cyc_Core_Impossible_exn_struct _tmp8DD;const char*_tmp8DC;struct Cyc_Core_Impossible_exn_struct*_tmp8DB;(int)_throw((void*)((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DD.tag=Cyc_Core_Impossible,((_tmp8DD.f1=((_tmp8DC="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8DC,sizeof(char),56))),_tmp8DD)))),_tmp8DB)))));}_LL33C:;};}
# 2533
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2536
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp60C=env->fenv;
void*_stmttmp8E=decl->r;void*_tmp60D=_stmttmp8E;struct Cyc_Absyn_Vardecl*_tmp60F;struct Cyc_List_List*_tmp611;struct Cyc_Absyn_Fndecl*_tmp613;struct Cyc_Absyn_Tvar*_tmp615;struct Cyc_Absyn_Vardecl*_tmp616;int _tmp617;struct Cyc_Absyn_Exp*_tmp618;_LL38E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp60E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60D;if(_tmp60E->tag != 0)goto _LL390;else{_tmp60F=_tmp60E->f1;}}_LL38F: {
# 2546
struct Cyc_List_List _tmp8DE;struct Cyc_List_List _tmp619=(_tmp8DE.hd=_tmp60F,((_tmp8DE.tl=0,_tmp8DE)));
inflow=Cyc_NewControlFlow_add_vars(_tmp60C,inflow,& _tmp619,_tmp60C->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp61A=_tmp60F->initializer;
if(_tmp61A == 0)
return inflow;{
struct _tuple15 _stmttmp90=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp61A);union Cyc_CfFlowInfo_FlowInfo _tmp61C;void*_tmp61D;struct _tuple15 _tmp61B=_stmttmp90;_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp60C,env,_tmp61C,_tmp60F,_tmp61A,_tmp61D,decl->loc);};};}_LL390: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp610=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60D;if(_tmp610->tag != 3)goto _LL392;else{_tmp611=_tmp610->f1;}}_LL391:
# 2555
 return Cyc_NewControlFlow_add_vars(_tmp60C,inflow,_tmp611,_tmp60C->unknown_none,decl->loc,0);_LL392: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp612=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60D;if(_tmp612->tag != 1)goto _LL394;else{_tmp613=_tmp612->f1;}}_LL393:
# 2558
 Cyc_NewControlFlow_check_nested_fun(_tmp60C,inflow,_tmp613);{
void*t=(void*)_check_null(_tmp613->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp61F=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp613->fn_vardecl);
# 2564
struct Cyc_List_List*_tmp8DF;return Cyc_NewControlFlow_add_vars(_tmp60C,inflow,((_tmp8DF=_region_malloc(env->r,sizeof(*_tmp8DF)),((_tmp8DF->hd=_tmp61F,((_tmp8DF->tl=0,_tmp8DF)))))),_tmp60C->unknown_all,decl->loc,0);};_LL394: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp614=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60D;if(_tmp614->tag != 4)goto _LL396;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;_tmp617=_tmp614->f3;_tmp618=_tmp614->f4;}}_LL395:
# 2567
 if(_tmp618 != 0){
struct Cyc_Absyn_Exp*_tmp621=_tmp618;
struct _tuple15 _stmttmp8F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp621);union Cyc_CfFlowInfo_FlowInfo _tmp623;void*_tmp624;struct _tuple15 _tmp622=_stmttmp8F;_tmp623=_tmp622.f1;_tmp624=_tmp622.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp621->loc,_tmp623,_tmp624);}{
# 2572
struct Cyc_List_List _tmp8E0;struct Cyc_List_List _tmp625=(_tmp8E0.hd=_tmp616,((_tmp8E0.tl=0,_tmp8E0)));
return Cyc_NewControlFlow_add_vars(_tmp60C,inflow,& _tmp625,_tmp60C->unknown_all,decl->loc,0);};_LL396:;_LL397: {
# 2576
struct Cyc_Core_Impossible_exn_struct _tmp8E6;const char*_tmp8E5;struct Cyc_Core_Impossible_exn_struct*_tmp8E4;(int)_throw((void*)((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E6.tag=Cyc_Core_Impossible,((_tmp8E6.f1=((_tmp8E5="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8E5,sizeof(char),35))),_tmp8E6)))),_tmp8E4)))));}_LL38D:;}
# 2584
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp62A=_new_region("frgn");struct _RegionHandle*frgn=& _tmp62A;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2586
;_pop_region(frgn);}
# 2591
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2595
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2599
struct _RegionHandle*_tmp62B=fenv->r;
unsigned int _tmp62C=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp62C,1);{
# 2605
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp62D=inflow;struct Cyc_Dict_Dict _tmp62E;struct Cyc_List_List*_tmp62F;_LL399: if((_tmp62D.BottomFL).tag != 1)goto _LL39B;_LL39A: {
const char*_tmp8E9;void*_tmp8E8;(_tmp8E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8E9="check_fun",_tag_dyneither(_tmp8E9,sizeof(char),10))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}_LL39B: if((_tmp62D.ReachableFL).tag != 2)goto _LL398;_tmp62E=((struct _tuple14)(_tmp62D.ReachableFL).val).f1;_tmp62F=((struct _tuple14)(_tmp62D.ReachableFL).val).f2;_LL39C: {
# 2612
struct Cyc_List_List*atts;
{void*_stmttmp91=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp632=_stmttmp91;struct Cyc_List_List*_tmp634;_LL39E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp633=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp632;if(_tmp633->tag != 9)goto _LL3A0;else{_tmp634=(_tmp633->f1).attributes;}}_LL39F:
 atts=_tmp634;goto _LL39D;_LL3A0:;_LL3A1: {
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8EC="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8EC,sizeof(char),50))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}_LL39D:;}
# 2617
for(0;atts != 0;atts=atts->tl){
void*_stmttmp92=(void*)atts->hd;void*_tmp637=_stmttmp92;int _tmp639;int _tmp63B;int _tmp63D;_LL3A3: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp638=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp637;if(_tmp638->tag != 21)goto _LL3A5;else{_tmp639=_tmp638->f1;}}_LL3A4: {
# 2620
unsigned int j=(unsigned int)_tmp639;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,Cyc_Tcutil_terr(_tmp62C,((_tmp8EF="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8EF,sizeof(char),52))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}
continue;}{
# 2626
struct Cyc_Absyn_Vardecl*_tmp640=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp640->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,Cyc_Tcutil_terr(_tmp62C,((_tmp8F2="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8F2,sizeof(char),52))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}
continue;}{
# 2632
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp643=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8F5;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8F4;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp644=(_tmp8F4=_region_malloc(_tmp62B,sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=2,((_tmp8F5.f1=(int)j,((_tmp8F5.f2=t,_tmp8F5)))))),_tmp8F4)));
struct Cyc_CfFlowInfo_Place*_tmp8F6;struct Cyc_CfFlowInfo_Place*_tmp645=(_tmp8F6=_region_malloc(_tmp62B,sizeof(*_tmp8F6)),((_tmp8F6->root=(void*)_tmp644,((_tmp8F6->fields=0,_tmp8F6)))));
_tmp62E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp62E,(void*)_tmp644,_tmp643);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8FC;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8FB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8F9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8F8;_tmp62E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp62E,(void*)((_tmp8F8=_region_malloc(_tmp62B,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=0,((_tmp8F9.f1=_tmp640,_tmp8F9)))),_tmp8F8)))),(void*)((_tmp8FB=_region_malloc(_tmp62B,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=5,((_tmp8FC.f1=_tmp645,_tmp8FC)))),_tmp8FB)))));}
goto _LL3A2;};};}_LL3A5: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp63A=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp637;if(_tmp63A->tag != 20)goto _LL3A7;else{_tmp63B=_tmp63A->f1;}}_LL3A6: {
# 2642
unsigned int j=(unsigned int)_tmp63B;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8FF;void*_tmp8FE;(_tmp8FE=0,Cyc_Tcutil_terr(_tmp62C,((_tmp8FF="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp8FF,sizeof(char),51))),_tag_dyneither(_tmp8FE,sizeof(void*),0)));}
# 2646
continue;}{
# 2648
struct Cyc_Absyn_Vardecl*_tmp64F=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_stmttmp93=Cyc_Tcutil_compress(_tmp64F->type);void*_tmp650=_stmttmp93;void*_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;_LL3AC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp651=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp650;if(_tmp651->tag != 5)goto _LL3AE;else{_tmp652=(_tmp651->f1).elt_typ;_tmp653=((_tmp651->f1).ptr_atts).nullable;_tmp654=((_tmp651->f1).ptr_atts).bounds;_tmp655=((_tmp651->f1).ptr_atts).zero_term;}}_LL3AD:
# 2651
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp653)){
const char*_tmp902;void*_tmp901;(_tmp901=0,Cyc_Tcutil_terr(_tmp62C,((_tmp902="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp902,sizeof(char),55))),_tag_dyneither(_tmp901,sizeof(void*),0)));}
# 2654
if(!Cyc_Tcutil_is_bound_one(_tmp654)){
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(_tmp62C,((_tmp905="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp905,sizeof(char),57))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
# 2657
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp655)){
const char*_tmp908;void*_tmp907;(_tmp907=0,Cyc_Tcutil_terr(_tmp62C,((_tmp908="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp908,sizeof(char),77))),_tag_dyneither(_tmp907,sizeof(void*),0)));}{
# 2660
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp90B;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp90A;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65C=(_tmp90A=_region_malloc(_tmp62B,sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=2,((_tmp90B.f1=(int)j,((_tmp90B.f2=_tmp652,_tmp90B)))))),_tmp90A)));
struct Cyc_CfFlowInfo_Place*_tmp90C;struct Cyc_CfFlowInfo_Place*_tmp65D=(_tmp90C=_region_malloc(_tmp62B,sizeof(*_tmp90C)),((_tmp90C->root=(void*)_tmp65C,((_tmp90C->fields=0,_tmp90C)))));
_tmp62E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp62E,(void*)_tmp65C,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp652,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp912;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp911;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp90F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp90E;_tmp62E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp62E,(void*)((_tmp90E=_region_malloc(_tmp62B,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=0,((_tmp90F.f1=_tmp64F,_tmp90F)))),_tmp90E)))),(void*)((_tmp911=_region_malloc(_tmp62B,sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=5,((_tmp912.f1=_tmp65D,_tmp912)))),_tmp911)))));}
{struct Cyc_List_List*_tmp913;param_roots=((_tmp913=_region_malloc(_tmp62B,sizeof(*_tmp913)),((_tmp913->hd=_tmp65D,((_tmp913->tl=param_roots,_tmp913))))));}
goto _LL3AB;};_LL3AE:;_LL3AF: {
const char*_tmp916;void*_tmp915;(_tmp915=0,Cyc_Tcutil_terr(_tmp62C,((_tmp916="initializes attribute on non-pointer",_tag_dyneither(_tmp916,sizeof(char),37))),_tag_dyneither(_tmp915,sizeof(void*),0)));}_LL3AB:;}
# 2668
goto _LL3A2;};}_LL3A7: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp63C=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp637;if(_tmp63C->tag != 22)goto _LL3A9;else{_tmp63D=_tmp63C->f1;}}_LL3A8: {
# 2670
unsigned int j=(unsigned int)_tmp63D;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_Tcutil_terr(_tmp62C,((_tmp919="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp919,sizeof(char),49))),_tag_dyneither(_tmp918,sizeof(void*),0)));}
continue;}{
# 2675
struct Cyc_Absyn_Vardecl*_tmp66A=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp66A->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp91C;void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_terr(_tmp62C,((_tmp91C="noconsume attribute requires no-alias pointer",_tag_dyneither(_tmp91C,sizeof(char),46))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}
continue;}{
# 2682
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp91F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp91E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66D=(_tmp91E=_region_malloc(_tmp62B,sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91F.tag=0,((_tmp91F.f1=_tmp66A,_tmp91F)))),_tmp91E)));
struct Cyc_CfFlowInfo_Place*_tmp920;struct Cyc_CfFlowInfo_Place*_tmp66E=(_tmp920=_region_malloc(_tmp62B,sizeof(*_tmp920)),((_tmp920->root=(void*)_tmp66D,((_tmp920->fields=0,_tmp920)))));
{struct Cyc_List_List*_tmp921;noconsume_roots=((_tmp921=_region_malloc(_tmp62B,sizeof(*_tmp921)),((_tmp921->hd=_tmp66E,((_tmp921->tl=noconsume_roots,_tmp921))))));}
goto _LL3A2;};};}_LL3A9:;_LL3AA:
 goto _LL3A2;_LL3A2:;}
# 2688
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp62E,_tmp62F);}_LL398:;}{
# 2691
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp62B,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp922;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp922=_region_malloc(_tmp62B,sizeof(*_tmp922)),((_tmp922->r=_tmp62B,((_tmp922->fenv=fenv,((_tmp922->iterate_again=1,((_tmp922->iteration_num=0,((_tmp922->in_try=0,((_tmp922->tryflow=inflow,((_tmp922->all_changed=0,((_tmp922->noreturn=noreturn,((_tmp922->unique_pat_vars=0,((_tmp922->param_roots=param_roots,((_tmp922->noconsume_params=noconsume_roots,((_tmp922->flow_table=flow_table,_tmp922)))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2701
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2704
union Cyc_CfFlowInfo_FlowInfo _tmp673=outflow;_LL3B1: if((_tmp673.BottomFL).tag != 1)goto _LL3B3;_LL3B2:
 goto _LL3B0;_LL3B3:;_LL3B4:
# 2707
 Cyc_NewControlFlow_check_init_params(_tmp62C,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp62C);
# 2711
if(noreturn){
const char*_tmp925;void*_tmp924;(_tmp924=0,Cyc_Tcutil_terr(_tmp62C,((_tmp925="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp925,sizeof(char),46))),_tag_dyneither(_tmp924,sizeof(void*),0)));}else{
# 2714
void*_stmttmp94=Cyc_Tcutil_compress(fd->ret_type);void*_tmp676=_stmttmp94;_LL3B6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp676;if(_tmp677->tag != 0)goto _LL3B8;}_LL3B7:
 goto _LL3B5;_LL3B8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp678=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp676;if(_tmp678->tag != 7)goto _LL3BA;}_LL3B9:
 goto _LL3BB;_LL3BA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp679=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp676;if(_tmp679->tag != 6)goto _LL3BC;}_LL3BB:
# 2718
{const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_warn(_tmp62C,((_tmp928="function may not return a value",_tag_dyneither(_tmp928,sizeof(char),32))),_tag_dyneither(_tmp927,sizeof(void*),0)));}goto _LL3B5;_LL3BC:;_LL3BD:
# 2720
{const char*_tmp92B;void*_tmp92A;(_tmp92A=0,Cyc_Tcutil_terr(_tmp62C,((_tmp92B="function may not return a value",_tag_dyneither(_tmp92B,sizeof(char),32))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}goto _LL3B5;_LL3B5:;}
# 2722
goto _LL3B0;_LL3B0:;};};};}
# 2727
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_stmttmp95=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp67F=_stmttmp95;struct Cyc_Absyn_Fndecl*_tmp681;struct Cyc_List_List*_tmp683;struct Cyc_List_List*_tmp685;struct Cyc_List_List*_tmp687;_LL3BF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp680=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp67F;if(_tmp680->tag != 1)goto _LL3C1;else{_tmp681=_tmp680->f1;}}_LL3C0:
 Cyc_NewControlFlow_check_fun(_tmp681);goto _LL3BE;_LL3C1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp682=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp67F;if(_tmp682->tag != 12)goto _LL3C3;else{_tmp683=_tmp682->f1;}}_LL3C2:
 _tmp685=_tmp683;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp684=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp67F;if(_tmp684->tag != 11)goto _LL3C5;else{_tmp685=_tmp684->f2;}}_LL3C4:
 _tmp687=_tmp685;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp686=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp67F;if(_tmp686->tag != 10)goto _LL3C7;else{_tmp687=_tmp686->f2;}}_LL3C6:
 Cyc_NewControlFlow_cf_check(_tmp687);goto _LL3BE;_LL3C7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp688=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp67F;if(_tmp688->tag != 13)goto _LL3C9;}_LL3C8:
 goto _LL3BE;_LL3C9:;_LL3CA:
 goto _LL3BE;_LL3BE:;}}
