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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 95
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 115
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 117
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 122
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 157
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 160
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 163
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 166
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 172
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 175
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 177
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 199 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 203
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 208
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple15 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple15 pr1,struct _tuple15 pr2);
# 214
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 219
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 228
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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp687;struct Cyc_NewControlFlow_CFStmtAnnot _tmp686;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp685;enclosee->annot=(void*)((_tmp685=_cycalloc(sizeof(*_tmp685)),((_tmp685[0]=((_tmp687.tag=Cyc_NewControlFlow_CFAnnot,((_tmp687.f1=((_tmp686.encloser=encloser,((_tmp686.visited=0,_tmp686)))),_tmp687)))),_tmp685))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 106 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 114
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 122
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 125
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_stmttmp0=s->annot;void*_tmp3=_stmttmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp68D;const char*_tmp68C;struct Cyc_Core_Impossible_exn_struct*_tmp68B;(int)_throw((void*)((_tmp68B=_cycalloc(sizeof(*_tmp68B)),((_tmp68B[0]=((_tmp68D.tag=Cyc_Core_Impossible,((_tmp68D.f1=((_tmp68C="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp68C,sizeof(char),37))),_tmp68D)))),_tmp68B)))));}_LL0:;}
# 132
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp68E;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp68E=_region_malloc(env->r,sizeof(*_tmp68E)),((_tmp68E[0]=Cyc_CfFlowInfo_BottomFL(),_tmp68E)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 139
return*sflow;}struct _tuple18{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 142
static struct _tuple18 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 146
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 149
_tmpA->visited=env->iteration_num;{
struct _tuple18 _tmp68F;return(_tmp68F.f1=_tmpA,((_tmp68F.f2=_tmpB,_tmp68F)));};}
# 161 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 169
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple19{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 176
static void Cyc_NewControlFlow_check_unique_root(struct _tuple19*ckenv,void*root,void*rval){
# 178
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple19*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp13;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp12=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 181
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp13->type)){
retry: {void*_tmp14=rval;void*_tmp16;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 8)goto _LLD;else{_tmp16=(void*)_tmp15->f2;}}_LLC:
 rval=_tmp16;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp18->tag != 3)goto _LL11;else{if(_tmp18->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp19->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 190
{const char*_tmp693;void*_tmp692[1];struct Cyc_String_pa_PrintArg_struct _tmp691;(_tmp691.tag=0,((_tmp691.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp692[0]=& _tmp691,Cyc_Tcutil_warn(_tmpF,((_tmp693="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp693,sizeof(char),60))),_tag_dyneither(_tmp692,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 194
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 202
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1D=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1E=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1F=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1E);
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple17 _tmp694;struct _tuple17 _stmttmp1=(_tmp694.f1=flow,((_tmp694.f2=_tmp1F,_tmp694)));struct _tuple17 _tmp20=_stmttmp1;struct Cyc_Dict_Dict _tmp21;struct Cyc_Dict_Dict _tmp22;_LL16: if(((_tmp20.f1).ReachableFL).tag != 2)goto _LL18;_tmp21=((struct _tuple14)((_tmp20.f1).ReachableFL).val).f1;if(((_tmp20.f2).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple14)((_tmp20.f2).ReachableFL).val).f1;_LL17: {
# 209
struct _tuple19 _tmp695;struct _tuple19 _tmp23=(_tmp695.f1=env,((_tmp695.f2=s->loc,((_tmp695.f3=_tmp22,_tmp695)))));
((void(*)(void(*f)(struct _tuple19*,void*,void*),struct _tuple19*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp23,_tmp21);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 217
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1F,*_tmp1E)){
# 224
*_tmp1E=_tmp1F;
# 228
if(_tmp1D->visited == env->iteration_num)
# 230
env->iterate_again=1;}}
# 235
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 239
union Cyc_CfFlowInfo_FlowInfo _tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;_LL1B: if((_tmp26.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple14)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple14)(_tmp26.ReachableFL).val).f2;_LL1E:
# 242
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp698;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp697;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp697=_region_malloc(fenv->r,sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=0,((_tmp698.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp698)))),_tmp697)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp69B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69A;_tmp2A=(void*)((_tmp69A=_region_malloc(fenv->r,sizeof(*_tmp69A)),((_tmp69A[0]=((_tmp69B.tag=8,((_tmp69B.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp69B.f2=_tmp2A,_tmp69B)))))),_tmp69A))));}
# 249
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 251
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 255
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 262
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp69C;if(Cyc_Relations_consistent_relations(((_tmp69C=_region_malloc(r,sizeof(*_tmp69C)),((_tmp69C->hd=_tmp2F,((_tmp69C->tl=assume,_tmp69C))))))))
return 0;}
# 267
return 1;}
# 270
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_stmttmp2=e->r;void*_tmp31=_stmttmp2;struct Cyc_Absyn_Exp*_tmp33;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp32=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp31;if(_tmp32->tag != 13)goto _LL22;else{_tmp33=_tmp32->f2;}}_LL21:
 return _tmp33;_LL22:;_LL23:
 return e;_LL1F:;}
# 277
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 279
union Cyc_CfFlowInfo_FlowInfo _tmp34=inflow;struct Cyc_List_List*_tmp35;_LL25: if((_tmp34.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp34.ReachableFL).tag != 2)goto _LL24;_tmp35=((struct _tuple14)(_tmp34.ReachableFL).val).f2;_LL28:
# 282
{void*_stmttmp3=Cyc_Tcutil_compress(t);void*_tmp36=_stmttmp3;union Cyc_Absyn_AggrInfoU _tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp3B;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL2C;else{_tmp38=(_tmp37->f1).aggr_info;_tmp39=(_tmp37->f1).targs;}}_LL2B: {
# 284
struct Cyc_Absyn_Aggrdecl*_tmp3C=Cyc_Absyn_get_known_aggrdecl(_tmp38);
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_decl_field(_tmp3C,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp3C->tvs,_tmp39),_tmp3E);
# 291
if(!Cyc_NewControlFlow_relns_ok(r,_tmp35,Cyc_Relations_exp2relns(temp,_tmp40))){
{const char*_tmp6A1;void*_tmp6A0[2];struct Cyc_String_pa_PrintArg_struct _tmp69F;struct Cyc_String_pa_PrintArg_struct _tmp69E;(_tmp69E.tag=0,((_tmp69E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp69F.tag=0,((_tmp69F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp6A0[0]=& _tmp69F,((_tmp6A0[1]=& _tmp69E,Cyc_Tcutil_terr(loc,((_tmp6A1="unable to prove %s, required to access %s",_tag_dyneither(_tmp6A1,sizeof(char),42))),_tag_dyneither(_tmp6A0,sizeof(void*),2)))))))))))));}
{const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="  [recorded facts on path: ",_tag_dyneither(_tmp6A4,sizeof(char),28))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="]\n",_tag_dyneither(_tmp6A7,sizeof(char),3))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));};}}
# 289
;_pop_region(temp);}
# 298
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3A->tag != 12)goto _LL2E;else{_tmp3B=_tmp3A->f2;}}_LL2D: {
# 300
struct Cyc_Absyn_Aggrfield*_tmp49=Cyc_Absyn_lookup_field(_tmp3B,f);
struct Cyc_Absyn_Exp*_tmp4A=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp49))->requires_clause;
if(_tmp4A != 0){
struct _RegionHandle _tmp4B=_new_region("temp");struct _RegionHandle*temp=& _tmp4B;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp35,Cyc_Relations_exp2relns(temp,_tmp4A))){
{const char*_tmp6AC;void*_tmp6AB[2];struct Cyc_String_pa_PrintArg_struct _tmp6AA;struct Cyc_String_pa_PrintArg_struct _tmp6A9;(_tmp6A9.tag=0,((_tmp6A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6AA.tag=0,((_tmp6AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp6AB[0]=& _tmp6AA,((_tmp6AB[1]=& _tmp6A9,Cyc_Tcutil_terr(loc,((_tmp6AC="unable to prove %s, required to access %s",_tag_dyneither(_tmp6AC,sizeof(char),42))),_tag_dyneither(_tmp6AB,sizeof(void*),2)))))))))))));}
{const char*_tmp6AF;void*_tmp6AE;(_tmp6AE=0,Cyc_fprintf(Cyc_stderr,((_tmp6AF="  [recorded facts on path: ",_tag_dyneither(_tmp6AF,sizeof(char),28))),_tag_dyneither(_tmp6AE,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp6B2;void*_tmp6B1;(_tmp6B1=0,Cyc_fprintf(Cyc_stderr,((_tmp6B2="]\n",_tag_dyneither(_tmp6B2,sizeof(char),3))),_tag_dyneither(_tmp6B1,sizeof(void*),0)));};}
# 304
;_pop_region(temp);}
# 311
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 314
goto _LL24;_LL24:;}
# 318
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp54=inflow;struct Cyc_Dict_Dict _tmp55;struct Cyc_List_List*_tmp56;_LL31: if((_tmp54.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp54.ReachableFL).tag != 2)goto _LL30;_tmp55=((struct _tuple14)(_tmp54.ReachableFL).val).f1;_tmp56=((struct _tuple14)(_tmp54.ReachableFL).val).f2;_LL34:
# 322
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp55,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp6B5;void*_tmp6B4;(_tmp6B4=0,Cyc_Tcutil_terr(loc,((_tmp6B5="expression may not be fully initialized",_tag_dyneither(_tmp6B5,sizeof(char),40))),_tag_dyneither(_tmp6B4,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 332
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6B6;struct _tuple0 _stmttmp4=(_tmp6B6.f1=Cyc_Tcutil_compress(t),((_tmp6B6.f2=r,_tmp6B6)));struct _tuple0 _tmp5B=_stmttmp4;void*_tmp60;struct Cyc_Absyn_Datatypefield*_tmp62;struct _dyneither_ptr _tmp64;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp68;union Cyc_Absyn_AggrInfoU _tmp6A;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;enum Cyc_Absyn_AggrKind _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5C->tag != 3)goto _LL38;else{if(_tmp5C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5D->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5E->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp5F->tag != 8)goto _LL3E;else{_tmp60=(void*)_tmp5F->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp60);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp61->tag != 4)goto _LL40;else{if((((_tmp61->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp62=((struct _tuple3)(((_tmp61->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp63->tag != 6)goto _LL40;else{_tmp64=_tmp63->f2;}};_LL3F:
# 339
 if(_tmp62->typs == 0)
return;
_tmp66=_tmp62->typs;_tmp68=_tmp64;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp65->tag != 10)goto _LL42;else{_tmp66=_tmp65->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp67=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp67->tag != 6)goto _LL42;else{_tmp68=_tmp67->f2;}};_LL41: {
# 343
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple20*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp68,sizeof(void*),i)));}}
# 347
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp69->tag != 11)goto _LL44;else{_tmp6A=(_tmp69->f1).aggr_info;_tmp6B=(_tmp69->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp6C->tag != 6)goto _LL44;else{_tmp6D=_tmp6C->f2;}};_LL43: {
# 350
struct Cyc_Absyn_Aggrdecl*_tmp74=Cyc_Absyn_get_known_aggrdecl(_tmp6A);
if(_tmp74->impl == 0)return;{
struct Cyc_List_List*_tmp75=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp74->impl))->fields;
struct _RegionHandle _tmp76=_new_region("temp");struct _RegionHandle*temp=& _tmp76;_push_region(temp);
{struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp74->tvs,_tmp6B);
{int i=0;for(0;i < _get_dyneither_size(_tmp6D,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type;
if(_tmp77 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp77,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp6D.curr)[i]);}}
# 360
_npop_handler(0);return;}
# 354
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp6E->tag != 12)goto _LL46;else{_tmp6F=_tmp6E->f1;_tmp70=_tmp6E->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5B.f2;if(_tmp71->tag != 6)goto _LL46;else{_tmp72=_tmp71->f2;}};_LL45:
# 363
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp70=_tmp70->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp70))->hd)->type,((void**)_tmp72.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B.f1;if(_tmp73->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,Cyc_Tcutil_warn(loc,((_tmp6B9="argument may still contain unique pointers",_tag_dyneither(_tmp6B9,sizeof(char),43))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 373
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple14)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple14)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 377
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_Tcutil_terr(loc,((_tmp6BC="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6BC,sizeof(char),47))),_tag_dyneither(_tmp6BB,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 381
void*_tmp80=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp81=r;void*_tmp83;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp81;if(_tmp82->tag != 8)goto _LL52;else{_tmp83=(void*)_tmp82->f2;}}_LL51:
 r=_tmp83;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp84=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp81;if(_tmp84->tag != 5)goto _LL54;else{_tmp85=_tmp84->f1;}}_LL53:
# 385
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp80,Cyc_CfFlowInfo_lookup_place(_tmp7C,_tmp85));
goto _LL4F;_LL54:;_LL55:
# 388
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp80)){
const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_Tcutil_warn(loc,((_tmp6BF="argument may contain live unique pointers",_tag_dyneither(_tmp6BF,sizeof(char),42))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 392
struct Cyc_Dict_Dict _tmp88=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp7C,env->all_changed,r);
if(_tmp7C.t == _tmp88.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp89=Cyc_CfFlowInfo_ReachableFL(_tmp88,_tmp7D);
Cyc_NewControlFlow_update_tryflow(env,_tmp89);
return _tmp89;};};};_LL4A:;}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 401
static struct _tuple21 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 408
if(es == 0){
struct _tuple21 _tmp6C0;return(_tmp6C0.f1=inflow,((_tmp6C0.f2=0,_tmp6C0)));}
if(es->tl == 0){
struct _tuple15 _stmttmp5=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple15 _tmp8B=_stmttmp5;_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6C3;struct _tuple21 _tmp6C2;return(_tmp6C2.f1=_tmp8C,((_tmp6C2.f2=((_tmp6C3=_region_malloc(rgn,sizeof(*_tmp6C3)),((_tmp6C3->hd=_tmp8D,((_tmp6C3->tl=0,_tmp6C3)))))),_tmp6C2)));};}{
# 414
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 420
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 424
{struct Cyc_Dict_Dict*_tmp6C4;env->all_changed=((_tmp6C4=_region_malloc(env->r,sizeof(*_tmp6C4)),((_tmp6C4[0]=(env->fenv)->mt_place_set,_tmp6C4))));}{
struct _tuple15 _stmttmp6=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_stmttmp6;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6C5;rvals=((_tmp6C5=_region_malloc(rgn,sizeof(*_tmp6C5)),((_tmp6C5->hd=_tmp93,((_tmp6C5->tl=0,_tmp6C5))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 430
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6C6;env->all_changed=((_tmp6C6=_region_malloc(env->r,sizeof(*_tmp6C6)),((_tmp6C6[0]=(env->fenv)->mt_place_set,_tmp6C6))));}{
struct _tuple15 _stmttmp7=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple15 _tmp96=_stmttmp7;_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6C7;rvals=((_tmp6C7=_region_malloc(rgn,sizeof(*_tmp6C7)),((_tmp6C7->hd=_tmp98,((_tmp6C7->tl=rvals,_tmp6C7))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp97,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 436
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 439
old_inflow=inflow;
# 442
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 447
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 451
struct Cyc_Dict_Dict*_tmp6C8;env->all_changed=((_tmp6C8=_region_malloc(env->r,sizeof(*_tmp6C8)),((_tmp6C8[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6C8))));}
# 453
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple21 _tmp6C9;return(_tmp6C9.f1=outflow,((_tmp6C9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6C9)));};};}
# 459
static struct _tuple15 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 462
struct _RegionHandle _tmp9C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C;_push_region(rgn);
{struct _tuple21 _stmttmp8=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 463
union Cyc_CfFlowInfo_FlowInfo _tmp9E;struct Cyc_List_List*_tmp9F;struct _tuple21 _tmp9D=_stmttmp8;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;
# 465
{union Cyc_CfFlowInfo_FlowInfo _tmpA0=_tmp9E;struct Cyc_Dict_Dict _tmpA1;_LL57: if((_tmpA0.ReachableFL).tag != 2)goto _LL59;_tmpA1=((struct _tuple14)(_tmpA0.ReachableFL).val).f1;_LL58:
# 467
 for(0;_tmp9F != 0;(_tmp9F=_tmp9F->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpA1,(void*)_tmp9F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6CC;void*_tmp6CB;(_tmp6CB=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6CC="expression may not be initialized",_tag_dyneither(_tmp6CC,sizeof(char),34))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 473
struct _tuple15 _tmp6CD;struct _tuple15 _tmpA5=(_tmp6CD.f1=_tmp9E,((_tmp6CD.f2=(env->fenv)->unknown_all,_tmp6CD)));_npop_handler(0);return _tmpA5;};}
# 463
;_pop_region(rgn);}
# 487 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 493
union Cyc_CfFlowInfo_FlowInfo _tmpA6=outflow;struct Cyc_Dict_Dict _tmpA7;struct Cyc_List_List*_tmpA8;_LL5C: if((_tmpA6.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpA6.ReachableFL).tag != 2)goto _LL5B;_tmpA7=((struct _tuple14)(_tmpA6.ReachableFL).val).f1;_tmpA8=((struct _tuple14)(_tmpA6.ReachableFL).val).f2;_LL5F: {
# 496
union Cyc_CfFlowInfo_AbsLVal _stmttmp9=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpA9=_stmttmp9;struct Cyc_CfFlowInfo_Place*_tmpAA;_LL61: if((_tmpA9.UnknownL).tag != 2)goto _LL63;_LL62:
# 500
 return outflow;_LL63: if((_tmpA9.PlaceL).tag != 1)goto _LL60;_tmpAA=(struct Cyc_CfFlowInfo_Place*)(_tmpA9.PlaceL).val;_LL64: {
# 504
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D0;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6CF;nzval=(void*)((_tmp6CF=_region_malloc((env->fenv)->r,sizeof(*_tmp6CF)),((_tmp6CF[0]=((_tmp6D0.tag=8,((_tmp6D0.f1=locname,((_tmp6D0.f2=nzval,_tmp6D0)))))),_tmp6CF))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 511
return _tmpAD;};}_LL60:;}_LL5B:;}
# 520
static struct _tuple17 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 523
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple17 _tmp6D1;return(_tmp6D1.f1=outflow,((_tmp6D1.f2=outflow,_tmp6D1)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple14)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple14)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 526
union Cyc_CfFlowInfo_AbsLVal _stmttmpA=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB2=_stmttmpA;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple17 _tmp6D2;return(_tmp6D2.f1=outflow,((_tmp6D2.f2=outflow,_tmp6D2)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 529
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D4;nzval=(void*)((_tmp6D4=_region_malloc((env->fenv)->r,sizeof(*_tmp6D4)),((_tmp6D4[0]=((_tmp6D5.tag=8,((_tmp6D5.f1=locname,((_tmp6D5.f2=nzval,_tmp6D5)))))),_tmp6D4))));}{
struct _tuple17 _tmp6D6;return
(_tmp6D6.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6D6.f2=
# 535
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,(env->fenv)->zero),_tmpB0),_tmp6D6)));};}_LL6A:;}_LL65:;}
# 541
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 547
static struct _tuple15 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 551
void*_stmttmpB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpBA=_stmttmpB;void*_tmpBC;union Cyc_Absyn_Constraint*_tmpBD;union Cyc_Absyn_Constraint*_tmpBE;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBA;if(_tmpBB->tag != 5)goto _LL72;else{_tmpBC=(_tmpBB->f1).elt_typ;_tmpBD=((_tmpBB->f1).ptr_atts).bounds;_tmpBE=((_tmpBB->f1).ptr_atts).zero_term;}}_LL71: {
# 553
union Cyc_CfFlowInfo_FlowInfo _tmpBF=f;struct Cyc_Dict_Dict _tmpC0;struct Cyc_List_List*_tmpC1;_LL75: if((_tmpBF.BottomFL).tag != 1)goto _LL77;_LL76: {
# 555
struct _tuple15 _tmp6D7;return(_tmp6D7.f1=f,((_tmp6D7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6D7)));}_LL77: if((_tmpBF.ReachableFL).tag != 2)goto _LL74;_tmpC0=((struct _tuple14)(_tmpBF.ReachableFL).val).f1;_tmpC1=((struct _tuple14)(_tmpBF.ReachableFL).val).f2;_LL78: {
# 558
struct Cyc_Absyn_Vardecl*locname=0;
{void*_tmpC3=r;struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC4->tag != 8)goto _LL7C;else{_tmpC5=_tmpC4->f1;_tmpC6=(void*)_tmpC4->f2;}}_LL7B:
 locname=_tmpC5;goto _LL79;_LL7C:;_LL7D:
 goto _LL79;_LL79:;}
# 563
if(Cyc_Tcutil_is_bound_one(_tmpBD)){
retry: {void*_tmpC7=r;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_CfFlowInfo_Place*_tmpCE;enum Cyc_CfFlowInfo_InitLevel _tmpD1;_LL7F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpC8->tag != 8)goto _LL81;else{_tmpC9=_tmpC8->f1;_tmpCA=(void*)_tmpC8->f2;}}_LL80:
 r=_tmpCA;goto retry;_LL81: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCB->tag != 1)goto _LL83;}_LL82:
 goto _LL84;_LL83: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCC->tag != 2)goto _LL85;}_LL84:
# 568
{void*_stmttmpF=e->annot;void*_tmpD2=_stmttmpF;struct Cyc_List_List*_tmpD4;_LL8E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD3=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD2;if(_tmpD3->tag != Cyc_CfFlowInfo_NotZero)goto _LL90;else{_tmpD4=_tmpD3->f1;}}_LL8F:
# 570
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpD4))goto _LL91;
goto _LL8D;_LL90:;_LL91:
# 574
{void*_stmttmp10=e->r;void*_tmpD5=_stmttmp10;_LL93: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD5;if(_tmpD6->tag != 22)goto _LL95;}_LL94:
# 576
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6DA;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6D9;e->annot=(void*)((_tmp6D9=_cycalloc(sizeof(*_tmp6D9)),((_tmp6D9[0]=((_tmp6DA.tag=Cyc_CfFlowInfo_NotZero,((_tmp6DA.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6DA)))),_tmp6D9))));}
goto _LL92;_LL95:;_LL96:
# 579
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL92;_LL92:;}
# 582
goto _LL8D;_LL8D:;}
# 584
goto _LL7E;_LL85: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCD->tag != 5)goto _LL87;else{_tmpCE=_tmpCD->f1;}}_LL86:
# 586
{void*_stmttmpD=e->annot;void*_tmpD9=_stmttmpD;struct Cyc_List_List*_tmpDB;_LL98: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpDA=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD9;if(_tmpDA->tag != Cyc_CfFlowInfo_NotZero)goto _LL9A;else{_tmpDB=_tmpDA->f1;}}_LL99:
# 588
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpDB))goto _LL9B;
goto _LL97;_LL9A:;_LL9B:
# 592
{void*_stmttmpE=e->r;void*_tmpDC=_stmttmpE;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC;if(_tmpDD->tag != 22)goto _LL9F;}_LL9E:
# 594
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6DD;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6DC;e->annot=(void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DD.tag=Cyc_CfFlowInfo_NotZero,((_tmp6DD.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6DD)))),_tmp6DC))));}
goto _LL9C;_LL9F:;_LLA0:
# 597
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL9C;_LL9C:;}
# 600
goto _LL97;_LL97:;}{
# 602
struct _tuple15 _tmp6DE;return(_tmp6DE.f1=f,((_tmp6DE.f2=Cyc_CfFlowInfo_lookup_place(_tmpC0,_tmpCE),_tmp6DE)));};_LL87: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCF->tag != 0)goto _LL89;}_LL88:
# 604
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp6DF;return(_tmp6DF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6DF.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6DF)));};_LL89: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpD0=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpD0->tag != 3)goto _LL8B;else{_tmpD1=_tmpD0->f1;}}_LL8A:
# 607
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpD1,locname);goto _LL8C;_LL8B:;_LL8C:
# 609
{void*_stmttmpC=e->r;void*_tmpE2=_stmttmpC;_LLA2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE2;if(_tmpE3->tag != 22)goto _LLA4;}_LLA3:
# 611
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E2;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E1;e->annot=(void*)((_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1[0]=((_tmp6E2.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E2.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E2)))),_tmp6E1))));}
goto _LLA1;_LLA4:;_LLA5:
# 614
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLA1;_LLA1:;}
# 617
goto _LL7E;_LL7E:;}}else{
# 620
void*_stmttmp11=e->annot;void*_tmpE6=_stmttmp11;struct Cyc_List_List*_tmpE8;_LLA7: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE6;if(_tmpE7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA9;else{_tmpE8=_tmpE7->f1;}}_LLA8:
# 622
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpE8))goto _LLAA;
goto _LLA6;_LLA9:;_LLAA:
# 625
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E5;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E4;e->annot=(void*)((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4[0]=((_tmp6E5.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E5.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6E5)))),_tmp6E4))));}
goto _LLA6;_LLA6:;}{
# 629
enum Cyc_CfFlowInfo_InitLevel _stmttmp12=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC0,r);switch(_stmttmp12){case Cyc_CfFlowInfo_NoneIL: _LLAB:
# 631
{void*_tmpEB=r;_LLAE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpEC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpEB;if(_tmpEC->tag != 7)goto _LLB0;}_LLAF:
# 634
{const char*_tmp6E8;void*_tmp6E7;(_tmp6E7=0,Cyc_Tcutil_terr(e->loc,((_tmp6E8="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6E8,sizeof(char),61))),_tag_dyneither(_tmp6E7,sizeof(void*),0)));}
goto _LLAD;_LLB0:;_LLB1: {
# 637
const char*_tmp6EB;void*_tmp6EA;(_tmp6EA=0,Cyc_Tcutil_terr(e->loc,((_tmp6EB="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6EB,sizeof(char),46))),_tag_dyneither(_tmp6EA,sizeof(void*),0)));}_LLAD:;}
# 639
goto _LLAC;case Cyc_CfFlowInfo_AllIL: _LLAC: {
# 641
void*_tmpF1=(env->fenv)->unknown_all;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6EE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6ED;_tmpF1=(void*)((_tmp6ED=_region_malloc((env->fenv)->r,sizeof(*_tmp6ED)),((_tmp6ED[0]=((_tmp6EE.tag=8,((_tmp6EE.f1=locname,((_tmp6EE.f2=_tmpF1,_tmp6EE)))))),_tmp6ED))));}{
struct _tuple15 _tmp6EF;return(_tmp6EF.f1=f,((_tmp6EF.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,_tmpF1),_tmp6EF)));};}case Cyc_CfFlowInfo_ThisIL: _LLB2: {
# 646
void*_tmpF5=(env->fenv)->unknown_none;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6F2;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6F1;_tmpF5=(void*)((_tmp6F1=_region_malloc((env->fenv)->r,sizeof(*_tmp6F1)),((_tmp6F1[0]=((_tmp6F2.tag=8,((_tmp6F2.f1=locname,((_tmp6F2.f2=_tmpF5,_tmp6F2)))))),_tmp6F1))));}{
struct _tuple15 _tmp6F3;return(_tmp6F3.f1=f,((_tmp6F3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,_tmpF5),_tmp6F3)));};}}};}_LL74:;}_LL72:;_LL73: {
# 652
struct Cyc_Core_Impossible_exn_struct _tmp6F9;const char*_tmp6F8;struct Cyc_Core_Impossible_exn_struct*_tmp6F7;(int)_throw((void*)((_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7[0]=((_tmp6F9.tag=Cyc_Core_Impossible,((_tmp6F9.f1=((_tmp6F8="right deref of non-pointer-type",_tag_dyneither(_tmp6F8,sizeof(char),32))),_tmp6F9)))),_tmp6F7)))));}_LL6F:;}
# 659
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 662
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 665
{void*_stmttmp13=e1->r;void*_tmpFC=_stmttmp13;void*_tmpFE;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFC;if(_tmpFD->tag != 1)goto _LLB7;else{_tmpFE=(void*)_tmpFD->f2;}}_LLB6: {
# 667
struct Cyc_Absyn_Vardecl*_tmpFF=Cyc_Tcutil_nonesc_vardecl(_tmpFE);
if(_tmpFF != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpFF);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 673
goto _LLB4;}_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}{
# 677
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 683
return relns;};}
# 688
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 696
union Cyc_CfFlowInfo_FlowInfo _tmp100=outflow;struct Cyc_Dict_Dict _tmp101;struct Cyc_List_List*_tmp102;_LLBA: if((_tmp100.BottomFL).tag != 1)goto _LLBC;_LLBB: {
# 699
struct _tuple15 _tmp6FA;return(_tmp6FA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6FA.f2=rval,_tmp6FA)));}_LLBC: if((_tmp100.ReachableFL).tag != 2)goto _LLB9;_tmp101=((struct _tuple14)(_tmp100.ReachableFL).val).f1;_tmp102=((struct _tuple14)(_tmp100.ReachableFL).val).f2;_LLBD: {
# 701
union Cyc_CfFlowInfo_AbsLVal _tmp104=lval;struct Cyc_CfFlowInfo_Place*_tmp105;_LLBF: if((_tmp104.PlaceL).tag != 1)goto _LLC1;_tmp105=(struct Cyc_CfFlowInfo_Place*)(_tmp104.PlaceL).val;_LLC0: {
# 703
struct Cyc_Dict_Dict _tmp106=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp101,env->all_changed,_tmp105,rval);
_tmp102=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp106,_tmp102);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 709
void*_stmttmp14=Cyc_CfFlowInfo_lookup_place(_tmp101,_tmp105);void*_tmp107=_stmttmp14;_LLC4: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp108=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp108->tag != 3)goto _LLC6;else{if(_tmp108->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC6;}}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp109=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp109->tag != 7)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10A->tag != 0)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10B->tag != 8)goto _LLCC;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10B->f2);if(_tmp10C->tag != 3)goto _LLCC;else{if(_tmp10C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCC;}}}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10D->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 7)goto _LLCE;}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10F->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 0)goto _LLD0;}}_LLCF:
 goto _LLC3;_LLD0:;_LLD1:
# 717
{const char*_tmp6FD;void*_tmp6FC;(_tmp6FC=0,Cyc_Tcutil_warn(lexp->loc,((_tmp6FD="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp6FD,sizeof(char),43))),_tag_dyneither(_tmp6FC,sizeof(void*),0)));}
goto _LLC3;_LLC3:;}
# 722
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 724
struct _tuple15 _tmp6FE;return(_tmp6FE.f1=outflow,((_tmp6FE.f2=rval,_tmp6FE)));};}_LLC1: if((_tmp104.UnknownL).tag != 2)goto _LLBE;_LLC2: {
# 727
struct _tuple15 _tmp6FF;return(_tmp6FF.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp6FF.f2=rval,_tmp6FF)));}_LLBE:;}_LLB9:;}
# 734
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 740
union Cyc_CfFlowInfo_FlowInfo _tmp115=f;struct Cyc_Dict_Dict _tmp116;struct Cyc_List_List*_tmp117;_LLD3: if((_tmp115.BottomFL).tag != 1)goto _LLD5;_LLD4:
 return Cyc_CfFlowInfo_BottomFL();_LLD5: if((_tmp115.ReachableFL).tag != 2)goto _LLD2;_tmp116=((struct _tuple14)(_tmp115.ReachableFL).val).f1;_tmp117=((struct _tuple14)(_tmp115.ReachableFL).val).f2;_LLD6:
# 743
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp705;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp704;struct Cyc_CfFlowInfo_Place*_tmp703;_tmp116=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp116,env->all_changed,(
(_tmp703=_region_malloc(env->r,sizeof(*_tmp703)),((_tmp703->root=(void*)((_tmp705=_region_malloc(env->r,sizeof(*_tmp705)),((_tmp705[0]=((_tmp704.tag=0,((_tmp704.f1=vd,_tmp704)))),_tmp705)))),((_tmp703->fields=0,_tmp703)))))),rval);}
# 746
_tmp117=Cyc_Relations_reln_assign_var(env->r,_tmp117,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11B=Cyc_CfFlowInfo_ReachableFL(_tmp116,_tmp117);
Cyc_NewControlFlow_update_tryflow(env,_tmp11B);
# 753
return _tmp11B;};_LLD2:;}struct _tuple22{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 757
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,unsigned int pat_loc,unsigned int exp_loc){
# 763
if(vds == 0)return inflow;{
# 766
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple22*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp706;es=((_tmp706=_cycalloc(sizeof(*_tmp706)),((_tmp706->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple22*)x->hd)).f2),((_tmp706->tl=es,_tmp706))))));}}}
# 772
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11C,fenv->unknown_all,pat_loc,0);
# 774
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 777
struct _tuple15 _stmttmp15=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp11F;void*_tmp120;struct _tuple15 _tmp11E=_stmttmp15;_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp11F,_tmp120);}}{
# 785
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp121 != 0;_tmp121=_tmp121->tl){
struct _tuple22*_stmttmp16=(struct _tuple22*)_tmp121->hd;struct Cyc_Absyn_Vardecl**_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct _tuple22*_tmp122=_stmttmp16;_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;
if(_tmp123 != 0  && _tmp124 != 0){
# 792
if(_tmp124->topt == 0){
const char*_tmp70A;void*_tmp709[1];struct Cyc_String_pa_PrintArg_struct _tmp708;(_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp124)),((_tmp709[0]=& _tmp708,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp70A="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp70A,sizeof(char),41))),_tag_dyneither(_tmp709,sizeof(void*),1)))))));}
# 797
Cyc_fflush(Cyc_stderr);{
struct _tuple15 _stmttmp17=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp124);union Cyc_CfFlowInfo_FlowInfo _tmp129;void*_tmp12A;struct _tuple15 _tmp128=_stmttmp17;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp129,*_tmp123,_tmp124,_tmp12A,pat_loc);};}}
# 803
return inflow;};};}
# 806
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 808
void*_stmttmp18=e->r;void*_tmp12B=_stmttmp18;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;_LLD8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp12C->tag != 1)goto _LLDA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp12C->f2);if(_tmp12D->tag != 4)goto _LLDA;}}_LLD9:
 goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp12E->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp12F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp12E->f2);if(_tmp12F->tag != 3)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp130->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp131=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp130->f2);if(_tmp131->tag != 5)goto _LLDE;}}_LLDD:
 return 1;_LLDE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp132->tag != 19)goto _LLE0;else{_tmp133=_tmp132->f1;}}_LLDF:
 _tmp135=_tmp133;goto _LLE1;_LLE0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp134->tag != 20)goto _LLE2;else{_tmp135=_tmp134->f1;}}_LLE1:
 _tmp137=_tmp135;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp136->tag != 21)goto _LLE4;else{_tmp137=_tmp136->f1;}}_LLE3:
# 815
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp137);_LLE4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp138->tag != 22)goto _LLE6;else{_tmp139=_tmp138->f1;}}_LLE5: {
# 817
void*_stmttmp19=Cyc_Tcutil_compress((void*)_check_null(_tmp139->topt));void*_tmp13C=_stmttmp19;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13C;if(_tmp13D->tag != 10)goto _LLED;}_LLEC:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp139);_LLED:;_LLEE:
 return 0;_LLEA:;}_LLE6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12B;if(_tmp13A->tag != 13)goto _LLE8;else{_tmp13B=_tmp13A->f2;}}_LLE7:
# 821
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp13B);_LLE8:;_LLE9:
 return 0;_LLD7:;}
# 826
static struct _tuple15 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 828
struct Cyc_CfFlowInfo_FlowEnv*_tmp13E=env->fenv;
# 832
while(1){
struct _tuple18 _stmttmp1A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);union Cyc_CfFlowInfo_FlowInfo*_tmp140;struct _tuple18 _tmp13F=_stmttmp1A;_tmp140=_tmp13F.f2;
inflow=*_tmp140;{
void*_stmttmp1B=s->r;void*_tmp141=_stmttmp1B;struct Cyc_Absyn_Stmt*_tmp143;struct Cyc_Absyn_Stmt*_tmp144;struct Cyc_Absyn_Tvar*_tmp147;struct Cyc_Absyn_Vardecl*_tmp148;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Absyn_Decl*_tmp14B;struct Cyc_Absyn_Stmt*_tmp14C;struct Cyc_Absyn_Exp*_tmp14E;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp142=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp141;if(_tmp142->tag != 2)goto _LLF2;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}_LLF1:
# 837
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp143);
s=_tmp144;
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp145=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp141;if(_tmp145->tag != 12)goto _LLF4;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp146=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp145->f1)->r;if(_tmp146->tag != 5)goto _LLF4;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;}}_tmp149=_tmp145->f2;}}_LLF3: {
# 843
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp148->initializer);
struct _tuple16 _stmttmp1C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);union Cyc_CfFlowInfo_FlowInfo _tmp150;union Cyc_CfFlowInfo_AbsLVal _tmp151;struct _tuple16 _tmp14F=_stmttmp1C;_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;{
struct _tuple16 _tmp70B;struct _tuple16 _stmttmp1D=(_tmp70B.f1=_tmp150,((_tmp70B.f2=_tmp151,_tmp70B)));struct _tuple16 _tmp152=_stmttmp1D;struct Cyc_Dict_Dict _tmp153;struct Cyc_List_List*_tmp154;struct Cyc_CfFlowInfo_Place*_tmp155;_LLFB: if(((_tmp152.f1).ReachableFL).tag != 2)goto _LLFD;_tmp153=((struct _tuple14)((_tmp152.f1).ReachableFL).val).f1;_tmp154=((struct _tuple14)((_tmp152.f1).ReachableFL).val).f2;if(((_tmp152.f2).PlaceL).tag != 1)goto _LLFD;_tmp155=(struct Cyc_CfFlowInfo_Place*)((_tmp152.f2).PlaceL).val;_LLFC: {
# 847
void*_tmp156=Cyc_CfFlowInfo_lookup_place(_tmp153,_tmp155);
# 849
void*t=(void*)_check_null(e->topt);
# 856
void*_tmp157=Cyc_CfFlowInfo_make_unique_consumed(_tmp13E,t,e,env->iteration_num,_tmp156);
_tmp153=Cyc_CfFlowInfo_assign_place(_tmp13E,e->loc,_tmp153,env->all_changed,_tmp155,_tmp157);
_tmp150=Cyc_CfFlowInfo_ReachableFL(_tmp153,_tmp154);{
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp1E=Cyc_CfFlowInfo_initlevel(_tmp13E,_tmp153,_tmp156);switch(_stmttmp1E){case Cyc_CfFlowInfo_AllIL: _LL101:
 leaf=_tmp13E->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL102:
 leaf=_tmp13E->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL103:
 leaf=_tmp13E->unknown_this;break;}}{
# 867
void*_tmp158=Cyc_CfFlowInfo_typ_to_absrval(_tmp13E,t,0,leaf);
_tmp150=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp150,_tmp156);{
struct Cyc_List_List _tmp70C;struct Cyc_List_List _tmp159=(_tmp70C.hd=_tmp148,((_tmp70C.tl=0,_tmp70C)));
_tmp150=Cyc_NewControlFlow_add_vars(_tmp13E,_tmp150,& _tmp159,_tmp13E->unknown_all,s->loc,0);{
struct _tuple15 _stmttmp1F=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp150,_tmp149);union Cyc_CfFlowInfo_FlowInfo _tmp15B;void*_tmp15C;struct _tuple15 _tmp15A=_stmttmp1F;_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp15D=_tmp15B;struct Cyc_Dict_Dict _tmp15E;struct Cyc_List_List*_tmp15F;_LL106: if((_tmp15D.ReachableFL).tag != 2)goto _LL108;_tmp15E=((struct _tuple14)(_tmp15D.ReachableFL).val).f1;_tmp15F=((struct _tuple14)(_tmp15D.ReachableFL).val).f2;_LL107:
# 874
 _tmp15E=Cyc_CfFlowInfo_assign_place(_tmp13E,s->loc,_tmp15E,env->all_changed,_tmp155,_tmp158);{
union Cyc_CfFlowInfo_FlowInfo _tmp160=Cyc_CfFlowInfo_ReachableFL(_tmp15E,_tmp15F);
struct _tuple15 _tmp70D;return(_tmp70D.f1=_tmp160,((_tmp70D.f2=_tmp15C,_tmp70D)));};_LL108:;_LL109: {
struct _tuple15 _tmp70E;return(_tmp70E.f1=_tmp15B,((_tmp70E.f2=_tmp15C,_tmp70E)));}_LL105:;};};};};};}_LLFD: if(((_tmp152.f1).BottomFL).tag != 1)goto _LLFF;_LLFE: {
# 879
struct _tuple15 _tmp70F;return(_tmp70F.f1=_tmp150,((_tmp70F.f2=_tmp13E->unknown_all,_tmp70F)));}_LLFF:;_LL100:
{const char*_tmp712;void*_tmp711;(_tmp711=0,Cyc_Tcutil_terr(e->loc,((_tmp712="bad alias expression--no unique path found",_tag_dyneither(_tmp712,sizeof(char),43))),_tag_dyneither(_tmp711,sizeof(void*),0)));}{
struct _tuple15 _tmp713;return(_tmp713.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp713.f2=_tmp13E->unknown_all,_tmp713)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp14A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp141;if(_tmp14A->tag != 12)goto _LLF6;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;}}_LLF5:
# 884
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp14B);
s=_tmp14C;
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp14D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp141;if(_tmp14D->tag != 1)goto _LLF8;else{_tmp14E=_tmp14D->f1;}}_LLF7:
# 888
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp14E);_LLF8:;_LLF9: {
struct Cyc_Core_Impossible_exn_struct _tmp719;const char*_tmp718;struct Cyc_Core_Impossible_exn_struct*_tmp717;(int)_throw((void*)((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717[0]=((_tmp719.tag=Cyc_Core_Impossible,((_tmp719.f1=((_tmp718="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp718,sizeof(char),33))),_tmp719)))),_tmp717)))));}_LLEF:;};}}struct _tuple23{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7CA(unsigned int*_tmp7C9,unsigned int*_tmp7C8,void***_tmp7C6,struct Cyc_List_List**_tmp350){for(*_tmp7C9=0;*_tmp7C9 < *_tmp7C8;(*_tmp7C9)++){
# 1484 "new_control_flow.cyc"
void*_tmp7C4;(*_tmp7C6)[*_tmp7C9]=(void*)((_tmp7C4=(void*)((struct Cyc_List_List*)_check_null(*_tmp350))->hd,((*_tmp350=(*_tmp350)->tl,_tmp7C4))));}}
# 898 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 902
struct Cyc_CfFlowInfo_FlowEnv*_tmp16C=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 915
{union Cyc_CfFlowInfo_FlowInfo _tmp16D=inflow;struct Cyc_Dict_Dict _tmp16E;struct Cyc_List_List*_tmp16F;_LL10B: if((_tmp16D.BottomFL).tag != 1)goto _LL10D;_LL10C: {
struct _tuple15 _tmp71A;return(_tmp71A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71A.f2=_tmp16C->unknown_all,_tmp71A)));}_LL10D: if((_tmp16D.ReachableFL).tag != 2)goto _LL10A;_tmp16E=((struct _tuple14)(_tmp16D.ReachableFL).val).f1;_tmp16F=((struct _tuple14)(_tmp16D.ReachableFL).val).f2;_LL10E:
 d=_tmp16E;relns=_tmp16F;_LL10A:;}
# 930 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 951 "new_control_flow.cyc"
struct _tuple16 _stmttmp20=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);union Cyc_CfFlowInfo_FlowInfo _tmp172;union Cyc_CfFlowInfo_AbsLVal _tmp173;struct _tuple16 _tmp171=_stmttmp20;_tmp172=_tmp171.f1;_tmp173=_tmp171.f2;{
struct _tuple16 _tmp71B;struct _tuple16 _stmttmp21=(_tmp71B.f1=_tmp172,((_tmp71B.f2=_tmp173,_tmp71B)));struct _tuple16 _tmp174=_stmttmp21;struct Cyc_Dict_Dict _tmp175;struct Cyc_List_List*_tmp176;struct Cyc_CfFlowInfo_Place*_tmp177;_LL110: if(((_tmp174.f1).ReachableFL).tag != 2)goto _LL112;_tmp175=((struct _tuple14)((_tmp174.f1).ReachableFL).val).f1;_tmp176=((struct _tuple14)((_tmp174.f1).ReachableFL).val).f2;if(((_tmp174.f2).PlaceL).tag != 1)goto _LL112;_tmp177=(struct Cyc_CfFlowInfo_Place*)((_tmp174.f2).PlaceL).val;_LL111: {
# 954
void*_tmp178=Cyc_CfFlowInfo_lookup_place(_tmp175,_tmp177);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp178,& needs_unconsume)){
{const char*_tmp71E;void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_terr(e->loc,((_tmp71E="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp71E,sizeof(char),71))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}{
struct _tuple15 _tmp71F;return(_tmp71F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp71F.f2=_tmp16C->unknown_all,_tmp71F)));};}else{
# 960
if(needs_unconsume){
# 962
struct _tuple15 _tmp720;return(_tmp720.f1=_tmp172,((_tmp720.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp16C,_tmp178),_tmp720)));}else{
# 965
void*_tmp17D=Cyc_CfFlowInfo_make_unique_consumed(_tmp16C,(void*)_check_null(e->topt),e,env->iteration_num,_tmp178);
struct Cyc_Dict_Dict _tmp17E=Cyc_CfFlowInfo_assign_place(_tmp16C,e->loc,_tmp175,env->all_changed,_tmp177,_tmp17D);
# 976
struct _tuple15 _tmp721;return(_tmp721.f1=Cyc_CfFlowInfo_ReachableFL(_tmp17E,_tmp176),((_tmp721.f2=_tmp178,_tmp721)));}}}_LL112:;_LL113:
# 978
 goto _LL10F;_LL10F:;};}{
# 981
void*_stmttmp22=e->r;void*_tmp181=_stmttmp22;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Vardecl*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp1A0;enum Cyc_Absyn_Primop _tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_List_List*_tmp1B3;int _tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;void**_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;int _tmp1B9;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C8;struct _dyneither_ptr*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct _dyneither_ptr*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Datatypedecl*_tmp1DC;struct Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1E0;struct Cyc_List_List*_tmp1E2;enum Cyc_Absyn_AggrKind _tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EB;int _tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;void*_tmp1EF;int _tmp1F0;struct Cyc_Absyn_Stmt*_tmp1F2;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp182->tag != 13)goto _LL117;else{_tmp183=_tmp182->f2;if(_tmp182->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL117;}}_LL116: {
# 985
struct _tuple15 _stmttmp55=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp183);union Cyc_CfFlowInfo_FlowInfo _tmp1FA;void*_tmp1FB;struct _tuple15 _tmp1F9=_stmttmp55;_tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;{
struct _tuple15 _stmttmp56=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FA,_tmp183,_tmp1FB);union Cyc_CfFlowInfo_FlowInfo _tmp1FD;void*_tmp1FE;struct _tuple15 _tmp1FC=_stmttmp56;_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;{
struct _tuple15 _tmp722;return(_tmp722.f1=_tmp1FD,((_tmp722.f2=_tmp1FB,_tmp722)));};};}_LL117: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp184->tag != 13)goto _LL119;else{_tmp185=_tmp184->f2;}}_LL118:
# 991
 _tmp187=_tmp185;goto _LL11A;_LL119: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp186->tag != 11)goto _LL11B;else{_tmp187=_tmp186->f1;}}_LL11A:
 _tmp189=_tmp187;goto _LL11C;_LL11B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp188->tag != 12)goto _LL11D;else{_tmp189=_tmp188->f1;}}_LL11C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp189);_LL11D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp18A->tag != 0)goto _LL11F;else{if(((_tmp18A->f1).Null_c).tag != 1)goto _LL11F;}}_LL11E:
# 995
 goto _LL120;_LL11F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp18B->tag != 0)goto _LL121;else{if(((_tmp18B->f1).Int_c).tag != 5)goto _LL121;if(((struct _tuple7)((_tmp18B->f1).Int_c).val).f2 != 0)goto _LL121;}}_LL120: {
struct _tuple15 _tmp723;return(_tmp723.f1=inflow,((_tmp723.f2=_tmp16C->zero,_tmp723)));}_LL121: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp18C->tag != 0)goto _LL123;else{if(((_tmp18C->f1).Int_c).tag != 5)goto _LL123;}}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp18D->tag != 1)goto _LL125;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp18E=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp18D->f2);if(_tmp18E->tag != 2)goto _LL125;}}_LL124: {
struct _tuple15 _tmp724;return(_tmp724.f1=inflow,((_tmp724.f2=_tmp16C->notzeroall,_tmp724)));}_LL125: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp18F->tag != 30)goto _LL127;else{if(_tmp18F->f1 != 0)goto _LL127;}}_LL126:
# 1000
 goto _LL128;_LL127: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp190->tag != 0)goto _LL129;}_LL128:
 goto _LL12A;_LL129: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp191->tag != 17)goto _LL12B;}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp192->tag != 16)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp193->tag != 18)goto _LL12F;}_LL12E:
 goto _LL130;_LL12F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp194->tag != 32)goto _LL131;}_LL130:
 goto _LL132;_LL131: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp195->tag != 31)goto _LL133;}_LL132: {
struct _tuple15 _tmp725;return(_tmp725.f1=inflow,((_tmp725.f2=_tmp16C->unknown_all,_tmp725)));}_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp196->tag != 1)goto _LL135;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp197=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp196->f2);if(_tmp197->tag != 1)goto _LL135;}}_LL134: {
# 1009
struct _tuple15 _tmp726;return(_tmp726.f1=inflow,((_tmp726.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp726)));}_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp198->tag != 1)goto _LL137;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp198->f2);if(_tmp199->tag != 3)goto _LL137;else{_tmp19A=_tmp199->f1;}}}_LL136:
# 1012
 _tmp19D=_tmp19A;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp19B->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp19C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp19B->f2);if(_tmp19C->tag != 4)goto _LL139;else{_tmp19D=_tmp19C->f1;}}}_LL138:
 _tmp1A0=_tmp19D;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp19E->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp19E->f2);if(_tmp19F->tag != 5)goto _LL13B;else{_tmp1A0=_tmp19F->f1;}}}_LL13A: {
# 1016
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp727;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp727.tag=0,((_tmp727.f1=_tmp1A0,_tmp727)));
struct _tuple15 _tmp728;return(_tmp728.f1=inflow,((_tmp728.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp728)));}_LL13B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1A1->tag != 2)goto _LL13D;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LL13C: {
# 1023
struct _tuple15 _stmttmp54=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1A3);union Cyc_CfFlowInfo_FlowInfo _tmp207;void*_tmp208;struct _tuple15 _tmp206=_stmttmp54;_tmp207=_tmp206.f1;_tmp208=_tmp206.f2;{
struct _tuple15 _tmp729;return(_tmp729.f1=_tmp207,((_tmp729.f2=_tmp208,_tmp729)));};}_LL13D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1A4->tag != 4)goto _LL13F;else{_tmp1A5=_tmp1A4->f1;}}_LL13E: {
# 1027
struct Cyc_List_List _tmp72A;struct Cyc_List_List _tmp20A=(_tmp72A.hd=_tmp1A5,((_tmp72A.tl=0,_tmp72A)));
struct _tuple15 _stmttmp51=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp20A);union Cyc_CfFlowInfo_FlowInfo _tmp20C;struct _tuple15 _tmp20B=_stmttmp51;_tmp20C=_tmp20B.f1;{
struct _tuple16 _stmttmp52=Cyc_NewControlFlow_anal_Lexp(env,_tmp20C,0,_tmp1A5);union Cyc_CfFlowInfo_AbsLVal _tmp20E;struct _tuple16 _tmp20D=_stmttmp52;_tmp20E=_tmp20D.f2;
{struct _tuple23 _tmp72B;struct _tuple23 _stmttmp53=(_tmp72B.f1=_tmp20E,((_tmp72B.f2=_tmp20C,_tmp72B)));struct _tuple23 _tmp20F=_stmttmp53;struct Cyc_CfFlowInfo_Place*_tmp210;struct Cyc_Dict_Dict _tmp211;struct Cyc_List_List*_tmp212;_LL17E: if(((_tmp20F.f1).PlaceL).tag != 1)goto _LL180;_tmp210=(struct Cyc_CfFlowInfo_Place*)((_tmp20F.f1).PlaceL).val;if(((_tmp20F.f2).ReachableFL).tag != 2)goto _LL180;_tmp211=((struct _tuple14)((_tmp20F.f2).ReachableFL).val).f1;_tmp212=((struct _tuple14)((_tmp20F.f2).ReachableFL).val).f2;_LL17F:
# 1032
 _tmp212=Cyc_Relations_reln_kill_exp(_tmp16C->r,_tmp212,_tmp1A5);
_tmp20C=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp16C,_tmp1A5->loc,_tmp211,env->all_changed,_tmp210,_tmp16C->unknown_all),_tmp212);
# 1036
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}{
# 1039
struct _tuple15 _tmp72C;return(_tmp72C.f1=_tmp20C,((_tmp72C.f2=_tmp16C->unknown_all,_tmp72C)));};};}_LL13F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1A6->tag != 3)goto _LL141;else{_tmp1A7=_tmp1A6->f1;if(_tmp1A6->f2 == 0)goto _LL141;_tmp1A8=_tmp1A6->f3;}}_LL140:
# 1042
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp72F;void*_tmp72E;(_tmp72E=0,Cyc_Tcutil_terr(e->loc,((_tmp72F="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp72F,sizeof(char),58))),_tag_dyneither(_tmp72E,sizeof(void*),0)));}{
struct _tuple15 _tmp730;return(_tmp730.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp730.f2=_tmp16C->unknown_all,_tmp730)));};}{
# 1046
struct Cyc_List_List _tmp731;struct Cyc_List_List _tmp219=(_tmp731.hd=_tmp1A8,((_tmp731.tl=0,_tmp731)));
struct Cyc_List_List _tmp732;struct Cyc_List_List _tmp21A=(_tmp732.hd=_tmp1A7,((_tmp732.tl=& _tmp219,_tmp732)));
struct _tuple15 _stmttmp4F=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp21A);union Cyc_CfFlowInfo_FlowInfo _tmp21C;struct _tuple15 _tmp21B=_stmttmp4F;_tmp21C=_tmp21B.f1;{
struct _tuple16 _stmttmp50=Cyc_NewControlFlow_anal_Lexp(env,_tmp21C,0,_tmp1A7);union Cyc_CfFlowInfo_AbsLVal _tmp21E;struct _tuple16 _tmp21D=_stmttmp50;_tmp21E=_tmp21D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp21F=_tmp21C;struct Cyc_Dict_Dict _tmp220;struct Cyc_List_List*_tmp221;_LL183: if((_tmp21F.ReachableFL).tag != 2)goto _LL185;_tmp220=((struct _tuple14)(_tmp21F.ReachableFL).val).f1;_tmp221=((struct _tuple14)(_tmp21F.ReachableFL).val).f2;_LL184:
# 1052
{union Cyc_CfFlowInfo_AbsLVal _tmp222=_tmp21E;struct Cyc_CfFlowInfo_Place*_tmp223;_LL188: if((_tmp222.PlaceL).tag != 1)goto _LL18A;_tmp223=(struct Cyc_CfFlowInfo_Place*)(_tmp222.PlaceL).val;_LL189:
# 1056
 _tmp221=Cyc_Relations_reln_kill_exp(_tmp16C->r,_tmp221,_tmp1A7);
_tmp220=Cyc_CfFlowInfo_assign_place(_tmp16C,_tmp1A7->loc,_tmp220,env->all_changed,_tmp223,_tmp16C->unknown_all);
# 1059
_tmp21C=Cyc_CfFlowInfo_ReachableFL(_tmp220,_tmp221);
# 1062
goto _LL187;_LL18A: if((_tmp222.UnknownL).tag != 2)goto _LL187;_LL18B:
# 1065
 goto _LL187;_LL187:;}
# 1067
goto _LL182;_LL185:;_LL186:
 goto _LL182;_LL182:;}{
# 1070
struct _tuple15 _tmp733;return(_tmp733.f1=_tmp21C,((_tmp733.f2=_tmp16C->unknown_all,_tmp733)));};};};_LL141: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1A9->tag != 3)goto _LL143;else{_tmp1AA=_tmp1A9->f1;if(_tmp1A9->f2 != 0)goto _LL143;_tmp1AB=_tmp1A9->f3;}}_LL142:
# 1074
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp736;void*_tmp735;(_tmp735=0,Cyc_Tcutil_terr(e->loc,((_tmp736="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp736,sizeof(char),58))),_tag_dyneither(_tmp735,sizeof(void*),0)));}{
struct _tuple15 _tmp737;return(_tmp737.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp737.f2=_tmp16C->unknown_all,_tmp737)));};}{
# 1078
struct Cyc_Dict_Dict*_tmp22A=env->all_changed;
# 1080
{struct Cyc_Absyn_Exp*_tmp738[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp738[1]=_tmp1AB,((_tmp738[0]=_tmp1AA,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp738,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp739;env->all_changed=((_tmp739=_region_malloc(env->r,sizeof(*_tmp739)),((_tmp739[0]=_tmp16C->mt_place_set,_tmp739))));}{
struct _tuple16 _stmttmp4D=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1AA);union Cyc_CfFlowInfo_FlowInfo _tmp22E;union Cyc_CfFlowInfo_AbsLVal _tmp22F;struct _tuple16 _tmp22D=_stmttmp4D;_tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;{
struct Cyc_Dict_Dict _tmp230=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp73A;env->all_changed=((_tmp73A=_region_malloc(env->r,sizeof(*_tmp73A)),((_tmp73A[0]=_tmp16C->mt_place_set,_tmp73A))));}{
struct _tuple15 _stmttmp4E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1AB);union Cyc_CfFlowInfo_FlowInfo _tmp233;void*_tmp234;struct _tuple15 _tmp232=_stmttmp4E;_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;{
struct Cyc_Dict_Dict _tmp235=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp236=Cyc_CfFlowInfo_after_flow(_tmp16C,& _tmp230,_tmp22E,_tmp233,_tmp230,_tmp235);
# 1091
union Cyc_CfFlowInfo_FlowInfo _tmp237=Cyc_CfFlowInfo_join_flow(_tmp16C,_tmp22A,inflow,_tmp236);
# 1094
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp237,inflow)){
if(_tmp22A == 0)
env->all_changed=0;else{
# 1098
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp22A,
Cyc_CfFlowInfo_union_place_set(_tmp230,_tmp235,0),0);}
# 1101
return Cyc_NewControlFlow_do_assign(_tmp16C,env,_tmp236,_tmp1AA,_tmp22F,_tmp1AB,_tmp234,e->loc);}
# 1104
inflow=_tmp237;};};};};}};_LL143: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1AC->tag != 8)goto _LL145;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL144: {
# 1109
struct _tuple15 _stmttmp4C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AD);union Cyc_CfFlowInfo_FlowInfo _tmp239;void*_tmp23A;struct _tuple15 _tmp238=_stmttmp4C;_tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp239,_tmp1AE);}_LL145: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1AF->tag != 10)goto _LL147;else{_tmp1B0=_tmp1AF->f1;}}_LL146: {
# 1113
struct _tuple15 _stmttmp4B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1B0);union Cyc_CfFlowInfo_FlowInfo _tmp23C;void*_tmp23D;struct _tuple15 _tmp23B=_stmttmp4B;_tmp23C=_tmp23B.f1;_tmp23D=_tmp23B.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1B0->loc,_tmp23C,_tmp23D);{
struct _tuple15 _tmp73B;return(_tmp73B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp73B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp73B)));};}_LL147: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1B1->tag != 9)goto _LL149;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LL148: {
# 1118
struct _RegionHandle _tmp23F=_new_region("temp");struct _RegionHandle*temp=& _tmp23F;_push_region(temp);
{struct Cyc_List_List*_tmp73C;struct _tuple21 _stmttmp43=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp73C=_region_malloc(temp,sizeof(*_tmp73C)),((_tmp73C->hd=_tmp1B2,((_tmp73C->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1B3),_tmp73C)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp241;struct Cyc_List_List*_tmp242;struct _tuple21 _tmp240=_stmttmp43;_tmp241=_tmp240.f1;_tmp242=_tmp240.f2;{
# 1121
union Cyc_CfFlowInfo_FlowInfo _tmp243=Cyc_NewControlFlow_use_Rval(env,_tmp1B2->loc,_tmp241,(void*)((struct Cyc_List_List*)_check_null(_tmp242))->hd);
_tmp242=_tmp242->tl;{
# 1124
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_stmttmp44=Cyc_Tcutil_compress((void*)_check_null(_tmp1B2->topt));void*_tmp244=_stmttmp44;void*_tmp246;_LL18D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244;if(_tmp245->tag != 5)goto _LL18F;else{_tmp246=(_tmp245->f1).elt_typ;}}_LL18E:
# 1129
{void*_stmttmp45=Cyc_Tcutil_compress(_tmp246);void*_tmp247=_stmttmp45;struct Cyc_List_List*_tmp249;_LL192: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp247;if(_tmp248->tag != 9)goto _LL194;else{_tmp249=(_tmp248->f1).attributes;}}_LL193:
# 1131
 for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
# 1133
void*_stmttmp46=(void*)_tmp249->hd;void*_tmp24A=_stmttmp46;int _tmp24C;int _tmp24E;int _tmp250;_LL197: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp24B=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp24A;if(_tmp24B->tag != 20)goto _LL199;else{_tmp24C=_tmp24B->f1;}}_LL198:
# 1135
{struct Cyc_List_List*_tmp73D;init_params=((_tmp73D=_region_malloc(temp,sizeof(*_tmp73D)),((_tmp73D->hd=(void*)_tmp24C,((_tmp73D->tl=init_params,_tmp73D))))));}goto _LL196;_LL199: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp24D=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp24A;if(_tmp24D->tag != 21)goto _LL19B;else{_tmp24E=_tmp24D->f1;}}_LL19A:
# 1137
{struct Cyc_List_List*_tmp73E;nolive_unique_params=((_tmp73E=_region_malloc(temp,sizeof(*_tmp73E)),((_tmp73E->hd=(void*)_tmp24E,((_tmp73E->tl=nolive_unique_params,_tmp73E))))));}
goto _LL196;_LL19B: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp24F=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp24A;if(_tmp24F->tag != 22)goto _LL19D;else{_tmp250=_tmp24F->f1;}}_LL19C:
# 1141
{struct Cyc_List_List*_tmp73F;noconsume_params=((_tmp73F=_region_malloc(temp,sizeof(*_tmp73F)),((_tmp73F->hd=(void*)_tmp250,((_tmp73F->tl=noconsume_params,_tmp73F))))));}
goto _LL196;_LL19D:;_LL19E:
 goto _LL196;_LL196:;}
# 1145
goto _LL191;_LL194:;_LL195: {
const char*_tmp742;void*_tmp741;(_tmp741=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp742="anal_Rexp: bad function type",_tag_dyneither(_tmp742,sizeof(char),29))),_tag_dyneither(_tmp741,sizeof(void*),0)));}_LL191:;}
# 1148
goto _LL18C;_LL18F:;_LL190: {
const char*_tmp745;void*_tmp744;(_tmp744=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp745="anal_Rexp: bad function type",_tag_dyneither(_tmp745,sizeof(char),29))),_tag_dyneither(_tmp744,sizeof(void*),0)));}_LL18C:;}
# 1151
{int i=1;for(0;_tmp242 != 0;(((_tmp242=_tmp242->tl,_tmp1B3=((struct Cyc_List_List*)_check_null(_tmp1B3))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp258=_tmp241;struct Cyc_Dict_Dict _tmp259;_LL1A0: if((_tmp258.BottomFL).tag != 1)goto _LL1A2;_LL1A1:
 goto _LL19F;_LL1A2: if((_tmp258.ReachableFL).tag != 2)goto _LL19F;_tmp259=((struct _tuple14)(_tmp258.ReachableFL).val).f1;_LL1A3:
# 1156
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp259,(void*)_tmp242->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp748;void*_tmp747;(_tmp747=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->loc,((_tmp748="expression may not be initialized",_tag_dyneither(_tmp748,sizeof(char),34))),_tag_dyneither(_tmp747,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp25C=_tmp243;struct Cyc_Dict_Dict _tmp25D;struct Cyc_List_List*_tmp25E;_LL1A5: if((_tmp25C.BottomFL).tag != 1)goto _LL1A7;_LL1A6:
 goto _LL1A4;_LL1A7: if((_tmp25C.ReachableFL).tag != 2)goto _LL1A4;_tmp25D=((struct _tuple14)(_tmp25C.ReachableFL).val).f1;_tmp25E=((struct _tuple14)(_tmp25C.ReachableFL).val).f2;_LL1A8: {
# 1163
struct Cyc_Dict_Dict _tmp25F=Cyc_CfFlowInfo_escape_deref(_tmp16C,_tmp25D,env->all_changed,(void*)_tmp242->hd);
{void*_stmttmp47=(void*)_tmp242->hd;void*_tmp260=_stmttmp47;struct Cyc_CfFlowInfo_Place*_tmp262;_LL1AA: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp261=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp260;if(_tmp261->tag != 5)goto _LL1AC;else{_tmp262=_tmp261->f1;}}_LL1AB:
# 1166
{void*_stmttmp48=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->topt));void*_tmp263=_stmttmp48;void*_tmp265;_LL1AF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263;if(_tmp264->tag != 5)goto _LL1B1;else{_tmp265=(_tmp264->f1).elt_typ;}}_LL1B0:
# 1170
 _tmp25F=Cyc_CfFlowInfo_assign_place(_tmp16C,((struct Cyc_Absyn_Exp*)_tmp1B3->hd)->loc,_tmp25F,env->all_changed,_tmp262,
# 1172
Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,_tmp265,0,_tmp16C->esc_all));
# 1174
goto _LL1AE;_LL1B1:;_LL1B2: {
const char*_tmp74B;void*_tmp74A;(_tmp74A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp74B="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp74B,sizeof(char),39))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}_LL1AE:;}
# 1177
goto _LL1A9;_LL1AC:;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1180
_tmp243=Cyc_CfFlowInfo_ReachableFL(_tmp25F,_tmp25E);
goto _LL1A4;}_LL1A4:;}
# 1183
goto _LL19F;_LL19F:;}else{
# 1186
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1191
_tmp243=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->topt),_tmp243,(void*)_tmp242->hd);else{
# 1198
_tmp243=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd)->loc,_tmp243,(void*)_tmp242->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1201
struct _tuple16 _stmttmp49=Cyc_NewControlFlow_anal_Lexp(env,_tmp243,1,(struct Cyc_Absyn_Exp*)_tmp1B3->hd);union Cyc_CfFlowInfo_AbsLVal _tmp269;struct _tuple16 _tmp268=_stmttmp49;_tmp269=_tmp268.f2;{
struct _tuple16 _tmp74C;struct _tuple16 _stmttmp4A=(_tmp74C.f1=_tmp243,((_tmp74C.f2=_tmp269,_tmp74C)));struct _tuple16 _tmp26A=_stmttmp4A;struct Cyc_Dict_Dict _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_CfFlowInfo_Place*_tmp26D;_LL1B4: if(((_tmp26A.f1).ReachableFL).tag != 2)goto _LL1B6;_tmp26B=((struct _tuple14)((_tmp26A.f1).ReachableFL).val).f1;_tmp26C=((struct _tuple14)((_tmp26A.f1).ReachableFL).val).f2;if(((_tmp26A.f2).PlaceL).tag != 1)goto _LL1B6;_tmp26D=(struct Cyc_CfFlowInfo_Place*)((_tmp26A.f2).PlaceL).val;_LL1B5: {
# 1204
void*_tmp26E=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp16C,(void*)_tmp242->hd);
# 1207
_tmp26B=Cyc_CfFlowInfo_assign_place(_tmp16C,((struct Cyc_Absyn_Exp*)_tmp1B3->hd)->loc,_tmp26B,env->all_changed,_tmp26D,_tmp26E);
_tmp243=Cyc_CfFlowInfo_ReachableFL(_tmp26B,_tmp26C);
# 1212
goto _LL1B3;}_LL1B6: if(((_tmp26A.f1).BottomFL).tag != 1)goto _LL1B8;_LL1B7:
 continue;_LL1B8:;_LL1B9: {
# 1215
const char*_tmp74F;void*_tmp74E;(_tmp74E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp1B3->hd)->loc,((_tmp74F="noconsume() parameters must be l-values",_tag_dyneither(_tmp74F,sizeof(char),40))),_tag_dyneither(_tmp74E,sizeof(void*),0)));}_LL1B3:;};}}}}}
# 1222
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1B2->topt))){
struct _tuple15 _tmp750;struct _tuple15 _tmp273=(_tmp750.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp750.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp750)));_npop_handler(0);return _tmp273;}else{
# 1225
struct _tuple15 _tmp751;struct _tuple15 _tmp275=(_tmp751.f1=_tmp243,((_tmp751.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp751)));_npop_handler(0);return _tmp275;}};};}
# 1119
;_pop_region(temp);}_LL149: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1B4->tag != 33)goto _LL14B;else{_tmp1B5=(_tmp1B4->f1).is_calloc;_tmp1B6=(_tmp1B4->f1).rgn;_tmp1B7=(_tmp1B4->f1).elt_type;_tmp1B8=(_tmp1B4->f1).num_elts;_tmp1B9=(_tmp1B4->f1).fat_result;}}_LL14A: {
# 1228
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp754;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp753;void*root=(void*)((_tmp753=_region_malloc(_tmp16C->r,sizeof(*_tmp753)),((_tmp753[0]=((_tmp754.tag=1,((_tmp754.f1=_tmp1B8,((_tmp754.f2=(void*)_check_null(e->topt),_tmp754)))))),_tmp753))));
struct Cyc_CfFlowInfo_Place*_tmp755;struct Cyc_CfFlowInfo_Place*place=(_tmp755=_region_malloc(_tmp16C->r,sizeof(*_tmp755)),((_tmp755->root=root,((_tmp755->fields=0,_tmp755)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp758;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp757;void*rval=(void*)((_tmp757=_region_malloc(_tmp16C->r,sizeof(*_tmp757)),((_tmp757[0]=((_tmp758.tag=5,((_tmp758.f1=place,_tmp758)))),_tmp757))));
void*place_val;
if(_tmp1B9)place_val=_tmp16C->notzeroall;else{
if(_tmp1B5)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,*((void**)_check_null(_tmp1B7)),0,_tmp16C->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,*((void**)_check_null(_tmp1B7)),0,_tmp16C->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1B6 != 0){
struct _RegionHandle _tmp277=_new_region("temp");struct _RegionHandle*temp=& _tmp277;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp759[2];struct _tuple21 _stmttmp41=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp759[1]=_tmp1B8,((_tmp759[0]=_tmp1B6,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp759,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1239
union Cyc_CfFlowInfo_FlowInfo _tmp279;struct Cyc_List_List*_tmp27A;struct _tuple21 _tmp278=_stmttmp41;_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;
# 1241
outflow=_tmp279;}
# 1239
;_pop_region(temp);}else{
# 1244
struct _tuple15 _stmttmp42=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1B8);union Cyc_CfFlowInfo_FlowInfo _tmp27D;struct _tuple15 _tmp27C=_stmttmp42;_tmp27D=_tmp27C.f1;
outflow=_tmp27D;}{
# 1247
union Cyc_CfFlowInfo_FlowInfo _tmp27E=outflow;struct Cyc_Dict_Dict _tmp27F;struct Cyc_List_List*_tmp280;_LL1BB: if((_tmp27E.BottomFL).tag != 1)goto _LL1BD;_LL1BC: {
struct _tuple15 _tmp75A;return(_tmp75A.f1=outflow,((_tmp75A.f2=rval,_tmp75A)));}_LL1BD: if((_tmp27E.ReachableFL).tag != 2)goto _LL1BA;_tmp27F=((struct _tuple14)(_tmp27E.ReachableFL).val).f1;_tmp280=((struct _tuple14)(_tmp27E.ReachableFL).val).f2;_LL1BE: {
# 1250
struct _tuple15 _tmp75B;return(_tmp75B.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27F,root,place_val),_tmp280),((_tmp75B.f2=rval,_tmp75B)));}_LL1BA:;};};}_LL14B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1BA->tag != 34)goto _LL14D;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL14C: {
# 1254
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1260
struct _RegionHandle _tmp288=_new_region("temp");struct _RegionHandle*temp=& _tmp288;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp75C[2];struct _tuple21 _stmttmp3E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp75C[1]=_tmp1BC,((_tmp75C[0]=_tmp1BB,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp75C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1261
union Cyc_CfFlowInfo_FlowInfo _tmp28A;struct Cyc_List_List*_tmp28B;struct _tuple21 _tmp289=_stmttmp3E;_tmp28A=_tmp289.f1;_tmp28B=_tmp289.f2;
# 1263
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp28B))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp28B->tl))->hd;
outflow=_tmp28A;}{
# 1268
struct _tuple16 _stmttmp3F=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1BB);union Cyc_CfFlowInfo_AbsLVal _tmp28E;struct _tuple16 _tmp28D=_stmttmp3F;_tmp28E=_tmp28D.f2;{
struct _tuple16 _stmttmp40=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1BC);union Cyc_CfFlowInfo_AbsLVal _tmp290;struct _tuple16 _tmp28F=_stmttmp40;_tmp290=_tmp28F.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp291=outflow;struct Cyc_Dict_Dict _tmp292;struct Cyc_List_List*_tmp293;_LL1C0: if((_tmp291.ReachableFL).tag != 2)goto _LL1C2;_tmp292=((struct _tuple14)(_tmp291.ReachableFL).val).f1;_tmp293=((struct _tuple14)(_tmp291.ReachableFL).val).f2;_LL1C1:
# 1272
{union Cyc_CfFlowInfo_AbsLVal _tmp294=_tmp28E;struct Cyc_CfFlowInfo_Place*_tmp295;_LL1C5: if((_tmp294.PlaceL).tag != 1)goto _LL1C7;_tmp295=(struct Cyc_CfFlowInfo_Place*)(_tmp294.PlaceL).val;_LL1C6:
# 1274
 _tmp292=Cyc_CfFlowInfo_assign_place(_tmp16C,_tmp1BB->loc,_tmp292,env->all_changed,_tmp295,right_rval);
# 1276
goto _LL1C4;_LL1C7: if((_tmp294.UnknownL).tag != 2)goto _LL1C4;_LL1C8:
 goto _LL1C4;_LL1C4:;}
# 1279
{union Cyc_CfFlowInfo_AbsLVal _tmp296=_tmp290;struct Cyc_CfFlowInfo_Place*_tmp297;_LL1CA: if((_tmp296.PlaceL).tag != 1)goto _LL1CC;_tmp297=(struct Cyc_CfFlowInfo_Place*)(_tmp296.PlaceL).val;_LL1CB:
# 1281
 _tmp292=Cyc_CfFlowInfo_assign_place(_tmp16C,_tmp1BC->loc,_tmp292,env->all_changed,_tmp297,left_rval);
# 1283
goto _LL1C9;_LL1CC: if((_tmp296.UnknownL).tag != 2)goto _LL1C9;_LL1CD:
 goto _LL1C9;_LL1C9:;}
# 1287
_tmp293=Cyc_Relations_reln_kill_exp(_tmp16C->r,_tmp293,_tmp1BB);
_tmp293=Cyc_Relations_reln_kill_exp(_tmp16C->r,_tmp293,_tmp1BC);
# 1290
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp292,_tmp293);
goto _LL1BF;_LL1C2:;_LL1C3:
 goto _LL1BF;_LL1BF:;}{
# 1296
struct _tuple15 _tmp75D;struct _tuple15 _tmp299=(_tmp75D.f1=outflow,((_tmp75D.f2=_tmp16C->unknown_all,_tmp75D)));_npop_handler(0);return _tmp299;};};};
# 1260
;_pop_region(temp);}_LL14D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1BD->tag != 15)goto _LL14F;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;}}_LL14E: {
# 1299
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp760;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp75F;void*root=(void*)((_tmp75F=_region_malloc(_tmp16C->r,sizeof(*_tmp75F)),((_tmp75F[0]=((_tmp760.tag=1,((_tmp760.f1=_tmp1BF,((_tmp760.f2=(void*)_check_null(e->topt),_tmp760)))))),_tmp75F))));
struct Cyc_CfFlowInfo_Place*_tmp761;struct Cyc_CfFlowInfo_Place*place=(_tmp761=_region_malloc(_tmp16C->r,sizeof(*_tmp761)),((_tmp761->root=root,((_tmp761->fields=0,_tmp761)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp764;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp763;void*rval=(void*)((_tmp763=_region_malloc(_tmp16C->r,sizeof(*_tmp763)),((_tmp763[0]=((_tmp764.tag=5,((_tmp764.f1=place,_tmp764)))),_tmp763))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1BE != 0){
struct _RegionHandle _tmp29A=_new_region("temp");struct _RegionHandle*temp=& _tmp29A;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp765[2];struct _tuple21 _stmttmp3C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp765[1]=_tmp1BF,((_tmp765[0]=_tmp1BE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp765,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1307
union Cyc_CfFlowInfo_FlowInfo _tmp29C;struct Cyc_List_List*_tmp29D;struct _tuple21 _tmp29B=_stmttmp3C;_tmp29C=_tmp29B.f1;_tmp29D=_tmp29B.f2;
# 1309
outflow=_tmp29C;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp29D))->tl))->hd;}
# 1307
;_pop_region(temp);}else{
# 1313
struct _tuple15 _stmttmp3D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1BF);union Cyc_CfFlowInfo_FlowInfo _tmp2A0;void*_tmp2A1;struct _tuple15 _tmp29F=_stmttmp3D;_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;
outflow=_tmp2A0;
place_val=_tmp2A1;}{
# 1317
union Cyc_CfFlowInfo_FlowInfo _tmp2A2=outflow;struct Cyc_Dict_Dict _tmp2A3;struct Cyc_List_List*_tmp2A4;_LL1CF: if((_tmp2A2.BottomFL).tag != 1)goto _LL1D1;_LL1D0: {
struct _tuple15 _tmp766;return(_tmp766.f1=outflow,((_tmp766.f2=rval,_tmp766)));}_LL1D1: if((_tmp2A2.ReachableFL).tag != 2)goto _LL1CE;_tmp2A3=((struct _tuple14)(_tmp2A2.ReachableFL).val).f1;_tmp2A4=((struct _tuple14)(_tmp2A2.ReachableFL).val).f2;_LL1D2: {
# 1320
struct _tuple15 _tmp767;return(_tmp767.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A3,root,place_val),_tmp2A4),((_tmp767.f2=rval,_tmp767)));}_LL1CE:;};};}_LL14F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1C0->tag != 14)goto _LL151;else{_tmp1C1=_tmp1C0->f1;}}_LL150: {
# 1324
struct _tuple16 _stmttmp3B=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp2AD;union Cyc_CfFlowInfo_AbsLVal _tmp2AE;struct _tuple16 _tmp2AC=_stmttmp3B;_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2AF=_tmp2AE;struct Cyc_CfFlowInfo_Place*_tmp2B0;_LL1D4: if((_tmp2AF.UnknownL).tag != 2)goto _LL1D6;_LL1D5: {
struct _tuple15 _tmp768;return(_tmp768.f1=_tmp2AD,((_tmp768.f2=_tmp16C->notzeroall,_tmp768)));}_LL1D6: if((_tmp2AF.PlaceL).tag != 1)goto _LL1D3;_tmp2B0=(struct Cyc_CfFlowInfo_Place*)(_tmp2AF.PlaceL).val;_LL1D7: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp76E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp76D;struct _tuple15 _tmp76C;return(_tmp76C.f1=_tmp2AD,((_tmp76C.f2=(void*)((_tmp76E=_region_malloc(env->r,sizeof(*_tmp76E)),((_tmp76E[0]=((_tmp76D.tag=5,((_tmp76D.f1=_tmp2B0,_tmp76D)))),_tmp76E)))),_tmp76C)));}_LL1D3:;};}_LL151: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1C2->tag != 19)goto _LL153;else{_tmp1C3=_tmp1C2->f1;}}_LL152: {
# 1331
struct _tuple15 _stmttmp3A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp2B6;void*_tmp2B7;struct _tuple15 _tmp2B5=_stmttmp3A;_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2B6,_tmp1C3,_tmp2B7);}_LL153: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1C4->tag != 20)goto _LL155;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}_LL154: {
# 1335
struct _tuple15 _stmttmp39=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C5);union Cyc_CfFlowInfo_FlowInfo _tmp2B9;void*_tmp2BA;struct _tuple15 _tmp2B8=_stmttmp39;_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;
if(_tmp1C5->topt == 0){
{const char*_tmp772;void*_tmp771[1];struct Cyc_String_pa_PrintArg_struct _tmp770;(_tmp770.tag=0,((_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp771[0]=& _tmp770,Cyc_fprintf(Cyc_stderr,((_tmp772="aggrmember exp %s\n",_tag_dyneither(_tmp772,sizeof(char),19))),_tag_dyneither(_tmp771,sizeof(void*),1)))))));}{
const char*_tmp776;void*_tmp775[1];struct Cyc_String_pa_PrintArg_struct _tmp774;(_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1C5)),((_tmp775[0]=& _tmp774,Cyc_fprintf(Cyc_stderr,((_tmp776="oops! %s.topt = null!\n",_tag_dyneither(_tmp776,sizeof(char),23))),_tag_dyneither(_tmp775,sizeof(void*),1)))))));};}{
# 1340
void*_tmp2C1=(void*)_check_null(_tmp1C5->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C1)){
# 1343
struct _tuple15 _tmp777;return(_tmp777.f1=_tmp2B9,((_tmp777.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp777)));}
# 1345
if(Cyc_Absyn_is_require_union_type(_tmp2C1))
Cyc_NewControlFlow_check_requires(_tmp1C5->loc,_tmp16C->r,_tmp2C1,_tmp1C6,_tmp2B9);{
# 1348
void*_tmp2C3=_tmp2BA;int _tmp2C6;int _tmp2C7;struct _dyneither_ptr _tmp2C8;int _tmp2CA;int _tmp2CB;struct _dyneither_ptr _tmp2CC;_LL1D9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2C4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2C3;if(_tmp2C4->tag != 8)goto _LL1DB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2C4->f2);if(_tmp2C5->tag != 6)goto _LL1DB;else{_tmp2C6=(_tmp2C5->f1).is_union;_tmp2C7=(_tmp2C5->f1).fieldnum;_tmp2C8=_tmp2C5->f2;}}}_LL1DA:
# 1350
 _tmp2CA=_tmp2C6;_tmp2CB=_tmp2C7;_tmp2CC=_tmp2C8;goto _LL1DC;_LL1DB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C3;if(_tmp2C9->tag != 6)goto _LL1DD;else{_tmp2CA=(_tmp2C9->f1).is_union;_tmp2CB=(_tmp2C9->f1).fieldnum;_tmp2CC=_tmp2C9->f2;}}_LL1DC: {
# 1352
int _tmp2CD=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1C5->topt),_tmp1C6);
if((_tmp2CA  && _tmp2CB != - 1) && _tmp2CB != _tmp2CD){
struct _tuple15 _tmp778;return(_tmp778.f1=_tmp2B9,((_tmp778.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),1,_tmp16C->unknown_none),_tmp778)));}{
struct _tuple15 _tmp779;return(_tmp779.f1=_tmp2B9,((_tmp779.f2=*((void**)_check_dyneither_subscript(_tmp2CC,sizeof(void*),_tmp2CD)),_tmp779)));};}_LL1DD:;_LL1DE:
# 1357
{const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_fprintf(Cyc_stderr,((_tmp77C="the bad rexp is :",_tag_dyneither(_tmp77C,sizeof(char),18))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2BA);
{const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_fprintf(Cyc_stderr,((_tmp77F="\n",_tag_dyneither(_tmp77F,sizeof(char),2))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp78C;const char*_tmp78B;void*_tmp78A[1];struct Cyc_String_pa_PrintArg_struct _tmp789;struct Cyc_Core_Impossible_exn_struct*_tmp788;(int)_throw((void*)((_tmp788=_cycalloc(sizeof(*_tmp788)),((_tmp788[0]=((_tmp78C.tag=Cyc_Core_Impossible,((_tmp78C.f1=(struct _dyneither_ptr)((_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp78A[0]=& _tmp789,Cyc_aprintf(((_tmp78B="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp78B,sizeof(char),26))),_tag_dyneither(_tmp78A,sizeof(void*),1)))))))),_tmp78C)))),_tmp788)))));};_LL1D8:;};};}_LL155: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1C7->tag != 37)goto _LL157;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL156: {
# 1367
struct _tuple15 _stmttmp38=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C8);union Cyc_CfFlowInfo_FlowInfo _tmp2DA;void*_tmp2DB;struct _tuple15 _tmp2D9=_stmttmp38;_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;
# 1369
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1C8->topt))){
struct _tuple15 _tmp78D;return(_tmp78D.f1=_tmp2DA,((_tmp78D.f2=_tmp16C->unknown_all,_tmp78D)));}{
void*_tmp2DD=_tmp2DB;int _tmp2E0;int _tmp2E1;struct _dyneither_ptr _tmp2E2;int _tmp2E4;int _tmp2E5;struct _dyneither_ptr _tmp2E6;_LL1E0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2DE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2DD;if(_tmp2DE->tag != 8)goto _LL1E2;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2DE->f2);if(_tmp2DF->tag != 6)goto _LL1E2;else{_tmp2E0=(_tmp2DF->f1).is_union;_tmp2E1=(_tmp2DF->f1).fieldnum;_tmp2E2=_tmp2DF->f2;}}}_LL1E1:
# 1373
 _tmp2E4=_tmp2E0;_tmp2E5=_tmp2E1;_tmp2E6=_tmp2E2;goto _LL1E3;_LL1E2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2DD;if(_tmp2E3->tag != 6)goto _LL1E4;else{_tmp2E4=(_tmp2E3->f1).is_union;_tmp2E5=(_tmp2E3->f1).fieldnum;_tmp2E6=_tmp2E3->f2;}}_LL1E3: {
# 1375
int _tmp2E7=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1C8->topt),_tmp1C9);
if(_tmp2E4  && _tmp2E5 != - 1){
if(_tmp2E5 != _tmp2E7){
struct _tuple15 _tmp78E;return(_tmp78E.f1=_tmp2DA,((_tmp78E.f2=_tmp16C->zero,_tmp78E)));}else{
# 1380
struct _tuple15 _tmp78F;return(_tmp78F.f1=_tmp2DA,((_tmp78F.f2=_tmp16C->notzeroall,_tmp78F)));}}else{
# 1382
struct _tuple15 _tmp790;return(_tmp790.f1=_tmp2DA,((_tmp790.f2=_tmp16C->unknown_all,_tmp790)));}}_LL1E4:;_LL1E5: {
# 1384
struct Cyc_Core_Impossible_exn_struct _tmp79D;const char*_tmp79C;void*_tmp79B[1];struct Cyc_String_pa_PrintArg_struct _tmp79A;struct Cyc_Core_Impossible_exn_struct*_tmp799;(int)_throw((void*)((_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799[0]=((_tmp79D.tag=Cyc_Core_Impossible,((_tmp79D.f1=(struct _dyneither_ptr)((_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp79B[0]=& _tmp79A,Cyc_aprintf(((_tmp79C="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp79C,sizeof(char),26))),_tag_dyneither(_tmp79B,sizeof(void*),1)))))))),_tmp79D)))),_tmp799)))));}_LL1DF:;};}_LL157: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1CA->tag != 21)goto _LL159;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}_LL158: {
# 1389
struct _tuple15 _stmttmp35=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1CB);union Cyc_CfFlowInfo_FlowInfo _tmp2F1;void*_tmp2F2;struct _tuple15 _tmp2F0=_stmttmp35;_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;{
# 1392
struct _tuple15 _stmttmp36=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F1,_tmp1CB,_tmp2F2);union Cyc_CfFlowInfo_FlowInfo _tmp2F4;void*_tmp2F5;struct _tuple15 _tmp2F3=_stmttmp36;_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{
# 1395
void*_stmttmp37=Cyc_Tcutil_compress((void*)_check_null(_tmp1CB->topt));void*_tmp2F6=_stmttmp37;void*_tmp2F8;_LL1E7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6;if(_tmp2F7->tag != 5)goto _LL1E9;else{_tmp2F8=(_tmp2F7->f1).elt_typ;}}_LL1E8:
# 1397
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2F8)){
# 1399
struct _tuple15 _tmp79E;return(_tmp79E.f1=_tmp2F4,((_tmp79E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp79E)));}
# 1401
if(Cyc_Absyn_is_require_union_type(_tmp2F8))
Cyc_NewControlFlow_check_requires(_tmp1CB->loc,_tmp16C->r,_tmp2F8,_tmp1CC,_tmp2F1);{
# 1404
void*_tmp2FA=_tmp2F5;int _tmp2FD;int _tmp2FE;struct _dyneither_ptr _tmp2FF;int _tmp301;int _tmp302;struct _dyneither_ptr _tmp303;_LL1EC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2FB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2FA;if(_tmp2FB->tag != 8)goto _LL1EE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2FB->f2);if(_tmp2FC->tag != 6)goto _LL1EE;else{_tmp2FD=(_tmp2FC->f1).is_union;_tmp2FE=(_tmp2FC->f1).fieldnum;_tmp2FF=_tmp2FC->f2;}}}_LL1ED:
# 1406
 _tmp301=_tmp2FD;_tmp302=_tmp2FE;_tmp303=_tmp2FF;goto _LL1EF;_LL1EE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp300=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2FA;if(_tmp300->tag != 6)goto _LL1F0;else{_tmp301=(_tmp300->f1).is_union;_tmp302=(_tmp300->f1).fieldnum;_tmp303=_tmp300->f2;}}_LL1EF: {
# 1408
int _tmp304=Cyc_CfFlowInfo_get_field_index(_tmp2F8,_tmp1CC);
if((_tmp301  && _tmp302 != - 1) && _tmp302 != _tmp304){
struct _tuple15 _tmp79F;return(_tmp79F.f1=_tmp2F4,((_tmp79F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),1,_tmp16C->unknown_none),_tmp79F)));}{
struct _tuple15 _tmp7A0;return(_tmp7A0.f1=_tmp2F4,((_tmp7A0.f2=*((void**)_check_dyneither_subscript(_tmp303,sizeof(void*),_tmp304)),_tmp7A0)));};}_LL1F0:;_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp7A6;const char*_tmp7A5;struct Cyc_Core_Impossible_exn_struct*_tmp7A4;(int)_throw((void*)((_tmp7A4=_cycalloc(sizeof(*_tmp7A4)),((_tmp7A4[0]=((_tmp7A6.tag=Cyc_Core_Impossible,((_tmp7A6.f1=((_tmp7A5="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7A5,sizeof(char),21))),_tmp7A6)))),_tmp7A4)))));}_LL1EB:;};_LL1E9:;_LL1EA: {
# 1414
struct Cyc_Core_Impossible_exn_struct _tmp7AC;const char*_tmp7AB;struct Cyc_Core_Impossible_exn_struct*_tmp7AA;(int)_throw((void*)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=((_tmp7AC.tag=Cyc_Core_Impossible,((_tmp7AC.f1=((_tmp7AB="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7AB,sizeof(char),25))),_tmp7AC)))),_tmp7AA)))));}_LL1E6:;};};}_LL159: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1CD->tag != 5)goto _LL15B;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;_tmp1D0=_tmp1CD->f3;}}_LL15A: {
# 1418
struct _tuple17 _stmttmp34=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1CE);union Cyc_CfFlowInfo_FlowInfo _tmp30E;union Cyc_CfFlowInfo_FlowInfo _tmp30F;struct _tuple17 _tmp30D=_stmttmp34;_tmp30E=_tmp30D.f1;_tmp30F=_tmp30D.f2;{
struct _tuple15 _tmp310=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp30E,_tmp1CF);
struct _tuple15 _tmp311=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp30F,_tmp1D0);
# 1422
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp16C,env->all_changed,_tmp310,_tmp311);};}_LL15B: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1D1=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1D1->tag != 6)goto _LL15D;else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D1->f2;}}_LL15C: {
# 1425
struct _tuple17 _stmttmp32=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D2);union Cyc_CfFlowInfo_FlowInfo _tmp313;union Cyc_CfFlowInfo_FlowInfo _tmp314;struct _tuple17 _tmp312=_stmttmp32;_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;{
struct _tuple15 _stmttmp33=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp313,_tmp1D3);union Cyc_CfFlowInfo_FlowInfo _tmp316;void*_tmp317;struct _tuple15 _tmp315=_stmttmp33;_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;{
struct _tuple15 _tmp7AD;struct _tuple15 _tmp318=(_tmp7AD.f1=_tmp316,((_tmp7AD.f2=_tmp317,_tmp7AD)));
struct _tuple15 _tmp7AE;struct _tuple15 _tmp319=(_tmp7AE.f1=_tmp314,((_tmp7AE.f2=_tmp16C->zero,_tmp7AE)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp16C,env->all_changed,_tmp318,_tmp319);};};}_LL15D: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1D4->tag != 7)goto _LL15F;else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D4->f2;}}_LL15E: {
# 1432
struct _tuple17 _stmttmp30=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D5);union Cyc_CfFlowInfo_FlowInfo _tmp31D;union Cyc_CfFlowInfo_FlowInfo _tmp31E;struct _tuple17 _tmp31C=_stmttmp30;_tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;{
struct _tuple15 _stmttmp31=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp31E,_tmp1D6);union Cyc_CfFlowInfo_FlowInfo _tmp320;void*_tmp321;struct _tuple15 _tmp31F=_stmttmp31;_tmp320=_tmp31F.f1;_tmp321=_tmp31F.f2;{
struct _tuple15 _tmp7AF;struct _tuple15 _tmp322=(_tmp7AF.f1=_tmp320,((_tmp7AF.f2=_tmp321,_tmp7AF)));
struct _tuple15 _tmp7B0;struct _tuple15 _tmp323=(_tmp7B0.f1=_tmp31D,((_tmp7B0.f2=_tmp16C->notzeroall,_tmp7B0)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp16C,env->all_changed,_tmp322,_tmp323);};};}_LL15F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1D7->tag != 22)goto _LL161;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;}}_LL160: {
# 1439
struct _RegionHandle _tmp326=_new_region("temp");struct _RegionHandle*temp=& _tmp326;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7B1[2];struct _tuple21 _stmttmp2C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7B1[1]=_tmp1D9,((_tmp7B1[0]=_tmp1D8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7B1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1440
union Cyc_CfFlowInfo_FlowInfo _tmp328;struct Cyc_List_List*_tmp329;struct _tuple21 _tmp327=_stmttmp2C;_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;{
# 1444
union Cyc_CfFlowInfo_FlowInfo _tmp32A=_tmp328;
{union Cyc_CfFlowInfo_FlowInfo _tmp32B=_tmp328;struct Cyc_Dict_Dict _tmp32C;struct Cyc_List_List*_tmp32D;_LL1F3: if((_tmp32B.ReachableFL).tag != 2)goto _LL1F5;_tmp32C=((struct _tuple14)(_tmp32B.ReachableFL).val).f1;_tmp32D=((struct _tuple14)(_tmp32B.ReachableFL).val).f2;_LL1F4:
# 1449
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp32C,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp329))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_Tcutil_terr(_tmp1D9->loc,((_tmp7B4="expression may not be initialized",_tag_dyneither(_tmp7B4,sizeof(char),34))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp330=Cyc_NewControlFlow_add_subscript_reln(_tmp16C->r,_tmp32D,_tmp1D8,_tmp1D9);
if(_tmp32D != _tmp330)
_tmp32A=Cyc_CfFlowInfo_ReachableFL(_tmp32C,_tmp330);
goto _LL1F2;};_LL1F5:;_LL1F6:
 goto _LL1F2;_LL1F2:;}{
# 1457
void*_stmttmp2D=Cyc_Tcutil_compress((void*)_check_null(_tmp1D8->topt));void*_tmp331=_stmttmp2D;struct Cyc_List_List*_tmp333;union Cyc_Absyn_Constraint*_tmp335;_LL1F8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp331;if(_tmp332->tag != 10)goto _LL1FA;else{_tmp333=_tmp332->f1;}}_LL1F9: {
# 1459
void*_stmttmp2F=(void*)((struct Cyc_List_List*)_check_null(_tmp329))->hd;void*_tmp336=_stmttmp2F;struct _dyneither_ptr _tmp339;struct _dyneither_ptr _tmp33B;_LL1FF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp337=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp336;if(_tmp337->tag != 8)goto _LL201;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp338=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp337->f2);if(_tmp338->tag != 6)goto _LL201;else{_tmp339=_tmp338->f2;}}}_LL200:
# 1461
 _tmp33B=_tmp339;goto _LL202;_LL201: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp336;if(_tmp33A->tag != 6)goto _LL203;else{_tmp33B=_tmp33A->f2;}}_LL202: {
# 1463
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1D9)).f1;
struct _tuple15 _tmp7B5;struct _tuple15 _tmp33D=(_tmp7B5.f1=_tmp32A,((_tmp7B5.f2=*((void**)_check_dyneither_subscript(_tmp33B,sizeof(void*),(int)i)),_tmp7B5)));_npop_handler(0);return _tmp33D;}_LL203:;_LL204: {
struct Cyc_Core_Impossible_exn_struct _tmp7BB;const char*_tmp7BA;struct Cyc_Core_Impossible_exn_struct*_tmp7B9;(int)_throw((void*)((_tmp7B9=_cycalloc(sizeof(*_tmp7B9)),((_tmp7B9[0]=((_tmp7BB.tag=Cyc_Core_Impossible,((_tmp7BB.f1=((_tmp7BA="anal_Rexp: Subscript",_tag_dyneither(_tmp7BA,sizeof(char),21))),_tmp7BB)))),_tmp7B9)))));}_LL1FE:;}_LL1FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp331;if(_tmp334->tag != 5)goto _LL1FC;else{_tmp335=((_tmp334->f1).ptr_atts).bounds;}}_LL1FB: {
# 1468
struct _tuple15 _stmttmp2E=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp328,_tmp1D8,(void*)((struct Cyc_List_List*)_check_null(_tmp329))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp342;void*_tmp343;struct _tuple15 _tmp341=_stmttmp2E;_tmp342=_tmp341.f1;_tmp343=_tmp341.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp344=_tmp342;_LL206: if((_tmp344.BottomFL).tag != 1)goto _LL208;_LL207: {
struct _tuple15 _tmp7BC;struct _tuple15 _tmp346=(_tmp7BC.f1=_tmp342,((_tmp7BC.f2=_tmp343,_tmp7BC)));_npop_handler(0);return _tmp346;}_LL208:;_LL209: {
struct _tuple15 _tmp7BD;struct _tuple15 _tmp348=(_tmp7BD.f1=_tmp32A,((_tmp7BD.f2=_tmp343,_tmp7BD)));_npop_handler(0);return _tmp348;}_LL205:;};}_LL1FC:;_LL1FD: {
# 1473
struct Cyc_Core_Impossible_exn_struct _tmp7C3;const char*_tmp7C2;struct Cyc_Core_Impossible_exn_struct*_tmp7C1;(int)_throw((void*)((_tmp7C1=_cycalloc(sizeof(*_tmp7C1)),((_tmp7C1[0]=((_tmp7C3.tag=Cyc_Core_Impossible,((_tmp7C3.f1=((_tmp7C2="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7C2,sizeof(char),33))),_tmp7C3)))),_tmp7C1)))));}_LL1F7:;};};}
# 1440
;_pop_region(temp);}_LL161: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1DA=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1DA->tag != 30)goto _LL163;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}_LL162:
# 1478
 _tmp1DE=_tmp1DB;goto _LL164;_LL163: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1DD->tag != 23)goto _LL165;else{_tmp1DE=_tmp1DD->f1;}}_LL164: {
# 1480
struct _RegionHandle _tmp34D=_new_region("temp");struct _RegionHandle*temp=& _tmp34D;_push_region(temp);
{struct _tuple21 _stmttmp2B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1DE,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp34F;struct Cyc_List_List*_tmp350;struct _tuple21 _tmp34E=_stmttmp2B;_tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;{
unsigned int _tmp7C9;unsigned int _tmp7C8;struct _dyneither_ptr _tmp7C7;void**_tmp7C6;unsigned int _tmp7C5;struct _dyneither_ptr aggrdict=
(_tmp7C5=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1DE),((_tmp7C6=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7C5)),((_tmp7C7=_tag_dyneither(_tmp7C6,sizeof(void*),_tmp7C5),((((_tmp7C8=_tmp7C5,_tmp7CA(& _tmp7C9,& _tmp7C8,& _tmp7C6,& _tmp350))),_tmp7C7)))))));
# 1488
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7D4;struct Cyc_CfFlowInfo_UnionRInfo _tmp7D3;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7D2;struct _tuple15 _tmp7D1;struct _tuple15 _tmp355=(_tmp7D1.f1=_tmp34F,((_tmp7D1.f2=(void*)((_tmp7D4=_region_malloc(env->r,sizeof(*_tmp7D4)),((_tmp7D4[0]=((_tmp7D2.tag=6,((_tmp7D2.f1=((_tmp7D3.is_union=0,((_tmp7D3.fieldnum=- 1,_tmp7D3)))),((_tmp7D2.f2=aggrdict,_tmp7D2)))))),_tmp7D4)))),_tmp7D1)));_npop_handler(0);return _tmp355;};}
# 1481
;_pop_region(temp);}_LL165: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1DF=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1DF->tag != 29)goto _LL167;else{_tmp1E0=_tmp1DF->f2;}}_LL166: {
# 1491
struct Cyc_List_List*fs;
{void*_stmttmp2A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp35B=_stmttmp2A;struct Cyc_List_List*_tmp35D;_LL20B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35B;if(_tmp35C->tag != 12)goto _LL20D;else{_tmp35D=_tmp35C->f2;}}_LL20C:
 fs=_tmp35D;goto _LL20A;_LL20D:;_LL20E: {
struct Cyc_Core_Impossible_exn_struct _tmp7DA;const char*_tmp7D9;struct Cyc_Core_Impossible_exn_struct*_tmp7D8;(int)_throw((void*)((_tmp7D8=_cycalloc(sizeof(*_tmp7D8)),((_tmp7D8[0]=((_tmp7DA.tag=Cyc_Core_Impossible,((_tmp7DA.f1=((_tmp7D9="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7D9,sizeof(char),35))),_tmp7DA)))),_tmp7D8)))));}_LL20A:;}
# 1496
_tmp1E2=_tmp1E0;_tmp1E3=Cyc_Absyn_StructA;_tmp1E4=fs;goto _LL168;}_LL167: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1E1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1E1->tag != 28)goto _LL169;else{_tmp1E2=_tmp1E1->f3;if(_tmp1E1->f4 == 0)goto _LL169;_tmp1E3=(_tmp1E1->f4)->kind;if((_tmp1E1->f4)->impl == 0)goto _LL169;_tmp1E4=((_tmp1E1->f4)->impl)->fields;}}_LL168: {
# 1498
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp361=_new_region("temp");struct _RegionHandle*temp=& _tmp361;_push_region(temp);
{struct _tuple21 _stmttmp28=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple24*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))Cyc_Core_snd,_tmp1E2),1,1);
# 1500
union Cyc_CfFlowInfo_FlowInfo _tmp363;struct Cyc_List_List*_tmp364;struct _tuple21 _tmp362=_stmttmp28;_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;{
# 1502
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp16C,_tmp1E4,_tmp1E3 == Cyc_Absyn_UnionA,_tmp16C->unknown_all);
# 1505
int field_no=-1;
{int i=0;for(0;_tmp364 != 0;(((_tmp364=_tmp364->tl,_tmp1E2=_tmp1E2->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple24*)((struct Cyc_List_List*)_check_null(_tmp1E2))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_stmttmp29=(void*)ds->hd;void*_tmp365=_stmttmp29;struct _dyneither_ptr*_tmp368;_LL210: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp366=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp365;if(_tmp366->tag != 0)goto _LL212;}_LL211: {
struct Cyc_Core_Impossible_exn_struct _tmp7E0;const char*_tmp7DF;struct Cyc_Core_Impossible_exn_struct*_tmp7DE;(int)_throw((void*)((_tmp7DE=_cycalloc(sizeof(*_tmp7DE)),((_tmp7DE[0]=((_tmp7E0.tag=Cyc_Core_Impossible,((_tmp7E0.f1=((_tmp7DF="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7DF,sizeof(char),22))),_tmp7E0)))),_tmp7DE)))));}_LL212: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp367=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp365;if(_tmp367->tag != 1)goto _LL20F;else{_tmp368=_tmp367->f1;}}_LL213:
# 1512
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1E4,_tmp368);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp364->hd;
# 1515
if(_tmp1E3 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp36C=(*((struct _tuple24*)_tmp1E2->hd)).f2;
_tmp363=Cyc_NewControlFlow_use_Rval(env,_tmp36C->loc,_tmp363,(void*)_tmp364->hd);
# 1519
Cyc_NewControlFlow_check_requires(_tmp36C->loc,_tmp16C->r,exp_type,_tmp368,_tmp363);}_LL20F:;}}}{
# 1522
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7E6;struct Cyc_CfFlowInfo_UnionRInfo _tmp7E5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7E4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp36D=(_tmp7E4=_region_malloc(env->r,sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E6.tag=6,((_tmp7E6.f1=((_tmp7E5.is_union=_tmp1E3 == Cyc_Absyn_UnionA,((_tmp7E5.fieldnum=field_no,_tmp7E5)))),((_tmp7E6.f2=aggrdict,_tmp7E6)))))),_tmp7E4)));
struct _tuple15 _tmp7E7;struct _tuple15 _tmp36F=(_tmp7E7.f1=_tmp363,((_tmp7E7.f2=(void*)_tmp36D,_tmp7E7)));_npop_handler(0);return _tmp36F;};};}
# 1500
;_pop_region(temp);}_LL169: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1E5=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1E5->tag != 28)goto _LL16B;}_LL16A: {
# 1526
struct Cyc_Core_Impossible_exn_struct _tmp7ED;const char*_tmp7EC;struct Cyc_Core_Impossible_exn_struct*_tmp7EB;(int)_throw((void*)((_tmp7EB=_cycalloc(sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7ED.tag=Cyc_Core_Impossible,((_tmp7ED.f1=((_tmp7EC="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7EC,sizeof(char),31))),_tmp7ED)))),_tmp7EB)))));}_LL16B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1E6->tag != 25)goto _LL16D;else{_tmp1E7=_tmp1E6->f1;}}_LL16C: {
# 1528
struct _RegionHandle _tmp376=_new_region("temp");struct _RegionHandle*temp=& _tmp376;_push_region(temp);
{struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple24*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple24*))Cyc_Core_snd,_tmp1E7);
struct _tuple21 _stmttmp27=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp377,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp379;struct Cyc_List_List*_tmp37A;struct _tuple21 _tmp378=_stmttmp27;_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;
for(0;_tmp37A != 0;(_tmp37A=_tmp37A->tl,_tmp377=_tmp377->tl)){
_tmp379=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp377))->hd)->loc,_tmp379,(void*)_tmp37A->hd);}{
struct _tuple15 _tmp7EE;struct _tuple15 _tmp37C=(_tmp7EE.f1=_tmp379,((_tmp7EE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,(void*)_check_null(e->topt),0,_tmp16C->unknown_all),_tmp7EE)));_npop_handler(0);return _tmp37C;};}
# 1529
;_pop_region(temp);}_LL16D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1E8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1E8->tag != 26)goto _LL16F;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;_tmp1EB=_tmp1E8->f3;_tmp1EC=_tmp1E8->f4;}}_LL16E: {
# 1537
struct _tuple15 _stmttmp24=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1EA);union Cyc_CfFlowInfo_FlowInfo _tmp37E;void*_tmp37F;struct _tuple15 _tmp37D=_stmttmp24;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp380=_tmp37E;struct Cyc_Dict_Dict _tmp381;struct Cyc_List_List*_tmp382;_LL215: if((_tmp380.BottomFL).tag != 1)goto _LL217;_LL216: {
struct _tuple15 _tmp7EF;return(_tmp7EF.f1=_tmp37E,((_tmp7EF.f2=_tmp16C->unknown_all,_tmp7EF)));}_LL217: if((_tmp380.ReachableFL).tag != 2)goto _LL214;_tmp381=((struct _tuple14)(_tmp380.ReachableFL).val).f1;_tmp382=((struct _tuple14)(_tmp380.ReachableFL).val).f2;_LL218:
# 1541
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp381,_tmp37F)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7F2;void*_tmp7F1;(_tmp7F1=0,Cyc_Tcutil_terr(_tmp1EA->loc,((_tmp7F2="expression may not be initialized",_tag_dyneither(_tmp7F2,sizeof(char),34))),_tag_dyneither(_tmp7F1,sizeof(void*),0)));}{
# 1545
struct Cyc_List_List*new_relns=_tmp382;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1E9);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1EA,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp382);
# 1551
if(_tmp382 != new_relns)
_tmp37E=Cyc_CfFlowInfo_ReachableFL(_tmp381,new_relns);{
# 1555
void*_tmp386=_tmp37F;_LL21A: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp387=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp386;if(_tmp387->tag != 0)goto _LL21C;}_LL21B: {
struct _tuple15 _tmp7F3;return(_tmp7F3.f1=_tmp37E,((_tmp7F3.f2=_tmp16C->unknown_all,_tmp7F3)));}_LL21C: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp388=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp386;if(_tmp388->tag != 2)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp389=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp386;if(_tmp389->tag != 1)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp38A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp386;if(_tmp38A->tag != 5)goto _LL222;}_LL221: {
# 1560
struct Cyc_List_List _tmp7F4;struct Cyc_List_List _tmp38C=(_tmp7F4.hd=_tmp1E9,((_tmp7F4.tl=0,_tmp7F4)));
_tmp37E=Cyc_NewControlFlow_add_vars(_tmp16C,_tmp37E,& _tmp38C,_tmp16C->unknown_all,e->loc,0);{
# 1563
struct _tuple15 _stmttmp26=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp37E,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp38E;void*_tmp38F;struct _tuple15 _tmp38D=_stmttmp26;_tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp390=_tmp38E;struct Cyc_Dict_Dict _tmp391;_LL225: if((_tmp390.BottomFL).tag != 1)goto _LL227;_LL226: {
struct _tuple15 _tmp7F5;return(_tmp7F5.f1=_tmp38E,((_tmp7F5.f2=_tmp16C->unknown_all,_tmp7F5)));}_LL227: if((_tmp390.ReachableFL).tag != 2)goto _LL224;_tmp391=((struct _tuple14)(_tmp390.ReachableFL).val).f1;_LL228:
# 1567
 if(Cyc_CfFlowInfo_initlevel(_tmp16C,_tmp391,_tmp38F)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7F8;void*_tmp7F7;(_tmp7F7=0,Cyc_Tcutil_terr(_tmp1EA->loc,((_tmp7F8="expression may not be initialized",_tag_dyneither(_tmp7F8,sizeof(char),34))),_tag_dyneither(_tmp7F7,sizeof(void*),0)));}{
struct _tuple15 _tmp7F9;return(_tmp7F9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F9.f2=_tmp16C->unknown_all,_tmp7F9)));};}_LL224:;}
# 1572
_tmp37E=_tmp38E;
goto _LL223;};}_LL222:;_LL223:
# 1575
 while(1){
struct Cyc_List_List _tmp7FA;struct Cyc_List_List _tmp397=(_tmp7FA.hd=_tmp1E9,((_tmp7FA.tl=0,_tmp7FA)));
_tmp37E=Cyc_NewControlFlow_add_vars(_tmp16C,_tmp37E,& _tmp397,_tmp16C->unknown_all,e->loc,0);{
struct _tuple15 _stmttmp25=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp37E,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp399;void*_tmp39A;struct _tuple15 _tmp398=_stmttmp25;_tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp39B=_tmp399;struct Cyc_Dict_Dict _tmp39C;_LL22A: if((_tmp39B.BottomFL).tag != 1)goto _LL22C;_LL22B:
 goto _LL229;_LL22C: if((_tmp39B.ReachableFL).tag != 2)goto _LL229;_tmp39C=((struct _tuple14)(_tmp39B.ReachableFL).val).f1;_LL22D:
# 1582
 if(Cyc_CfFlowInfo_initlevel(_tmp16C,_tmp39C,_tmp39A)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7FD;void*_tmp7FC;(_tmp7FC=0,Cyc_Tcutil_terr(_tmp1EA->loc,((_tmp7FD="expression may not be initialized",_tag_dyneither(_tmp7FD,sizeof(char),34))),_tag_dyneither(_tmp7FC,sizeof(void*),0)));}{
struct _tuple15 _tmp7FE;return(_tmp7FE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FE.f2=_tmp16C->unknown_all,_tmp7FE)));};}_LL229:;}{
# 1587
union Cyc_CfFlowInfo_FlowInfo _tmp3A0=Cyc_CfFlowInfo_join_flow(_tmp16C,env->all_changed,_tmp37E,_tmp399);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3A0,_tmp37E))
break;
_tmp37E=_tmp3A0;};};}{
# 1592
struct _tuple15 _tmp7FF;return(_tmp7FF.f1=_tmp37E,((_tmp7FF.f2=_tmp16C->unknown_all,_tmp7FF)));};_LL219:;};};_LL214:;};}_LL16F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1ED->tag != 27)goto _LL171;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=(void*)_tmp1ED->f2;_tmp1F0=_tmp1ED->f3;}}_LL170: {
# 1598
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp802;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp801;void*root=(void*)((_tmp801=_region_malloc(_tmp16C->r,sizeof(*_tmp801)),((_tmp801[0]=((_tmp802.tag=1,((_tmp802.f1=_tmp1EE,((_tmp802.f2=(void*)_check_null(e->topt),_tmp802)))))),_tmp801))));
struct Cyc_CfFlowInfo_Place*_tmp803;struct Cyc_CfFlowInfo_Place*place=(_tmp803=_region_malloc(_tmp16C->r,sizeof(*_tmp803)),((_tmp803->root=root,((_tmp803->fields=0,_tmp803)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp806;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp805;void*rval=(void*)((_tmp805=_region_malloc(_tmp16C->r,sizeof(*_tmp805)),((_tmp805[0]=((_tmp806.tag=5,((_tmp806.f1=place,_tmp806)))),_tmp805))));
void*place_val;
# 1606
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp16C,_tmp1EF,0,_tmp16C->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _stmttmp23=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1EE);union Cyc_CfFlowInfo_FlowInfo _tmp3A4;struct _tuple15 _tmp3A3=_stmttmp23;_tmp3A4=_tmp3A3.f1;
outflow=_tmp3A4;{
union Cyc_CfFlowInfo_FlowInfo _tmp3A5=outflow;struct Cyc_Dict_Dict _tmp3A6;struct Cyc_List_List*_tmp3A7;_LL22F: if((_tmp3A5.BottomFL).tag != 1)goto _LL231;_LL230: {
struct _tuple15 _tmp807;return(_tmp807.f1=outflow,((_tmp807.f2=rval,_tmp807)));}_LL231: if((_tmp3A5.ReachableFL).tag != 2)goto _LL22E;_tmp3A6=((struct _tuple14)(_tmp3A5.ReachableFL).val).f1;_tmp3A7=((struct _tuple14)(_tmp3A5.ReachableFL).val).f2;_LL232: {
# 1614
struct _tuple15 _tmp808;return(_tmp808.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3A6,root,place_val),_tmp3A7),((_tmp808.f2=rval,_tmp808)));}_LL22E:;};};};}_LL171: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1F1=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F1->tag != 36)goto _LL173;else{_tmp1F2=_tmp1F1->f1;}}_LL172:
# 1617
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1F2);_LL173: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F3->tag != 1)goto _LL175;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1F4=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1F3->f2);if(_tmp1F4->tag != 0)goto _LL175;}}_LL174:
 goto _LL176;_LL175: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1F5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F5->tag != 35)goto _LL177;}_LL176:
 goto _LL178;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1F6=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F6->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1F7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F7->tag != 38)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1F8=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp1F8->tag != 39)goto _LL114;}_LL17C: {
# 1623
struct Cyc_Core_Impossible_exn_struct _tmp80E;const char*_tmp80D;struct Cyc_Core_Impossible_exn_struct*_tmp80C;(int)_throw((void*)((_tmp80C=_cycalloc(sizeof(*_tmp80C)),((_tmp80C[0]=((_tmp80E.tag=Cyc_Core_Impossible,((_tmp80E.f1=((_tmp80D="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp80D,sizeof(char),31))),_tmp80E)))),_tmp80C)))));}_LL114:;};}
# 1632
static struct _tuple16 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1639
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B2=env->fenv;
void*_stmttmp57=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3B3=_stmttmp57;void*_tmp3B5;union Cyc_Absyn_Constraint*_tmp3B6;union Cyc_Absyn_Constraint*_tmp3B7;_LL234: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 5)goto _LL236;else{_tmp3B5=(_tmp3B4->f1).elt_typ;_tmp3B6=((_tmp3B4->f1).ptr_atts).bounds;_tmp3B7=((_tmp3B4->f1).ptr_atts).zero_term;}}_LL235: {
# 1642
union Cyc_CfFlowInfo_FlowInfo _tmp3B8=f;struct Cyc_Dict_Dict _tmp3B9;struct Cyc_List_List*_tmp3BA;_LL239: if((_tmp3B8.BottomFL).tag != 1)goto _LL23B;_LL23A: {
struct _tuple16 _tmp80F;return(_tmp80F.f1=f,((_tmp80F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp80F)));}_LL23B: if((_tmp3B8.ReachableFL).tag != 2)goto _LL238;_tmp3B9=((struct _tuple14)(_tmp3B8.ReachableFL).val).f1;_tmp3BA=((struct _tuple14)(_tmp3B8.ReachableFL).val).f2;_LL23C: {
# 1646
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp3B6)){
retry: {void*_tmp3BC=r;struct Cyc_Absyn_Vardecl*_tmp3BE;void*_tmp3BF;void*_tmp3C3;struct Cyc_List_List*_tmp3C4;enum Cyc_CfFlowInfo_InitLevel _tmp3C7;_LL23E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3BD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3BD->tag != 8)goto _LL240;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=(void*)_tmp3BD->f2;}}_LL23F:
 r=_tmp3BF;locname=_tmp3BE;goto retry;_LL240: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3C0=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3C0->tag != 1)goto _LL242;}_LL241:
 goto _LL243;_LL242: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3C1=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3C1->tag != 2)goto _LL244;}_LL243:
# 1652
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp812;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp811;e->annot=(void*)((_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811[0]=((_tmp812.tag=Cyc_CfFlowInfo_NotZero,((_tmp812.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3BA),_tmp812)))),_tmp811))));}goto _LL23D;_LL244: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3C2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3C2->tag != 5)goto _LL246;else{_tmp3C3=(_tmp3C2->f1)->root;_tmp3C4=(_tmp3C2->f1)->fields;}}_LL245:
# 1654
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp815;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp814;e->annot=(void*)((_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=Cyc_CfFlowInfo_NotZero,((_tmp815.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3BA),_tmp815)))),_tmp814))));}{
struct Cyc_CfFlowInfo_Place*_tmp818;struct _tuple16 _tmp817;return(_tmp817.f1=f,((_tmp817.f2=Cyc_CfFlowInfo_PlaceL(((_tmp818=_region_malloc(_tmp3B2->r,sizeof(*_tmp818)),((_tmp818->root=_tmp3C3,((_tmp818->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3B2->r,_tmp3C4,flds),_tmp818))))))),_tmp817)));};_LL246: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3C5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3C5->tag != 0)goto _LL248;}_LL247:
# 1657
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp819;return(_tmp819.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp819.f2=Cyc_CfFlowInfo_UnknownL(),_tmp819)));};_LL248: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3C6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3BC;if(_tmp3C6->tag != 3)goto _LL24A;else{_tmp3C7=_tmp3C6->f1;}}_LL249:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3C7,locname);goto _LL24B;_LL24A:;_LL24B: {
# 1661
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp81B;e->annot=(void*)((_tmp81B=_cycalloc(sizeof(*_tmp81B)),((_tmp81B[0]=((_tmp81C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3BA),_tmp81C)))),_tmp81B))));}_LL23D:;}}else{
# 1664
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp81F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp81E;e->annot=(void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp81F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3BA),_tmp81F)))),_tmp81E))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3B2,_tmp3B9,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp822;void*_tmp821;(_tmp821=0,Cyc_Tcutil_terr(e->loc,((_tmp822="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp822,sizeof(char),46))),_tag_dyneither(_tmp821,sizeof(void*),0)));}{
struct _tuple16 _tmp823;return(_tmp823.f1=f,((_tmp823.f2=Cyc_CfFlowInfo_UnknownL(),_tmp823)));};}_LL238:;}_LL236:;_LL237: {
# 1669
struct Cyc_Core_Impossible_exn_struct _tmp829;const char*_tmp828;struct Cyc_Core_Impossible_exn_struct*_tmp827;(int)_throw((void*)((_tmp827=_cycalloc(sizeof(*_tmp827)),((_tmp827[0]=((_tmp829.tag=Cyc_Core_Impossible,((_tmp829.f1=((_tmp828="left deref of non-pointer-type",_tag_dyneither(_tmp828,sizeof(char),31))),_tmp829)))),_tmp827)))));}_LL233:;}
# 1677
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1680
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3D9=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3DA=inflow;struct Cyc_Dict_Dict _tmp3DB;struct Cyc_List_List*_tmp3DC;_LL24D: if((_tmp3DA.BottomFL).tag != 1)goto _LL24F;_LL24E: {
struct _tuple16 _tmp82A;return(_tmp82A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp82A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82A)));}_LL24F: if((_tmp3DA.ReachableFL).tag != 2)goto _LL24C;_tmp3DB=((struct _tuple14)(_tmp3DA.ReachableFL).val).f1;_tmp3DC=((struct _tuple14)(_tmp3DA.ReachableFL).val).f2;_LL250:
# 1685
 d=_tmp3DB;_LL24C:;}{
# 1689
void*_stmttmp58=e->r;void*_tmp3DE=_stmttmp58;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E7;struct Cyc_Absyn_Vardecl*_tmp3EA;struct Cyc_Absyn_Vardecl*_tmp3ED;struct Cyc_Absyn_Vardecl*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F2;struct _dyneither_ptr*_tmp3F3;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FA;struct _dyneither_ptr*_tmp3FB;_LL252: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3DF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3DF->tag != 13)goto _LL254;else{_tmp3E0=_tmp3DF->f2;}}_LL253:
 _tmp3E2=_tmp3E0;goto _LL255;_LL254: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3E1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3E1->tag != 11)goto _LL256;else{_tmp3E2=_tmp3E1->f1;}}_LL255:
 _tmp3E4=_tmp3E2;goto _LL257;_LL256: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3E3->tag != 12)goto _LL258;else{_tmp3E4=_tmp3E3->f1;}}_LL257:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3E4,flds);_LL258: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3E5->tag != 1)goto _LL25A;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3E6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3E5->f2);if(_tmp3E6->tag != 3)goto _LL25A;else{_tmp3E7=_tmp3E6->f1;}}}_LL259:
# 1694
 _tmp3EA=_tmp3E7;goto _LL25B;_LL25A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3E8->tag != 1)goto _LL25C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3E9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3E8->f2);if(_tmp3E9->tag != 4)goto _LL25C;else{_tmp3EA=_tmp3E9->f1;}}}_LL25B:
 _tmp3ED=_tmp3EA;goto _LL25D;_LL25C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3EB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3EB->tag != 1)goto _LL25E;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3EC=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3EB->f2);if(_tmp3EC->tag != 5)goto _LL25E;else{_tmp3ED=_tmp3EC->f1;}}}_LL25D: {
# 1697
struct Cyc_CfFlowInfo_Place*_tmp834;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp833;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp832;struct _tuple16 _tmp831;return(_tmp831.f1=inflow,((_tmp831.f2=Cyc_CfFlowInfo_PlaceL(((_tmp834=_region_malloc(env->r,sizeof(*_tmp834)),((_tmp834->root=(void*)((_tmp832=_region_malloc(env->r,sizeof(*_tmp832)),((_tmp832[0]=((_tmp833.tag=0,((_tmp833.f1=_tmp3ED,_tmp833)))),_tmp832)))),((_tmp834->fields=flds,_tmp834))))))),_tmp831)));}_LL25E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3EE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3EE->tag != 1)goto _LL260;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3EF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3EE->f2);if(_tmp3EF->tag != 1)goto _LL260;else{_tmp3F0=_tmp3EF->f1;}}}_LL25F: {
# 1699
struct _tuple16 _tmp835;return(_tmp835.f1=inflow,((_tmp835.f2=Cyc_CfFlowInfo_UnknownL(),_tmp835)));}_LL260: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3F1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3F1->tag != 21)goto _LL262;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;}}_LL261:
# 1702
{void*_stmttmp5F=Cyc_Tcutil_compress((void*)_check_null(_tmp3F2->topt));void*_tmp401=_stmttmp5F;void*_tmp403;_LL26B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp401;if(_tmp402->tag != 5)goto _LL26D;else{_tmp403=(_tmp402->f1).elt_typ;}}_LL26C:
# 1704
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp403)){
Cyc_NewControlFlow_check_requires(_tmp3F2->loc,_tmp3D9->r,_tmp403,_tmp3F3,inflow);{
struct Cyc_List_List*_tmp836;flds=((_tmp836=_region_malloc(env->r,sizeof(*_tmp836)),((_tmp836->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp403,_tmp3F3),((_tmp836->tl=flds,_tmp836))))));};}
# 1708
goto _LL26A;_LL26D:;_LL26E: {
struct Cyc_Core_Impossible_exn_struct _tmp83C;const char*_tmp83B;struct Cyc_Core_Impossible_exn_struct*_tmp83A;(int)_throw((void*)((_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83C.tag=Cyc_Core_Impossible,((_tmp83C.f1=((_tmp83B="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp83B,sizeof(char),25))),_tmp83C)))),_tmp83A)))));}_LL26A:;}
# 1711
_tmp3F5=_tmp3F2;goto _LL263;_LL262: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3F4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3F4->tag != 19)goto _LL264;else{_tmp3F5=_tmp3F4->f1;}}_LL263:
# 1713
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3F5->topt),0)){
struct _tuple16 _stmttmp5C=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3F5->topt),1),_tmp3F5);
# 1714
union Cyc_CfFlowInfo_FlowInfo _tmp409;union Cyc_CfFlowInfo_AbsLVal _tmp40A;struct _tuple16 _tmp408=_stmttmp5C;_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;{
# 1717
struct _tuple16 _tmp83D;struct _tuple16 _stmttmp5D=(_tmp83D.f1=_tmp409,((_tmp83D.f2=_tmp40A,_tmp83D)));struct _tuple16 _tmp40B=_stmttmp5D;struct Cyc_Dict_Dict _tmp40C;struct Cyc_List_List*_tmp40D;struct Cyc_CfFlowInfo_Place*_tmp40E;_LL270: if(((_tmp40B.f1).ReachableFL).tag != 2)goto _LL272;_tmp40C=((struct _tuple14)((_tmp40B.f1).ReachableFL).val).f1;_tmp40D=((struct _tuple14)((_tmp40B.f1).ReachableFL).val).f2;if(((_tmp40B.f2).PlaceL).tag != 1)goto _LL272;_tmp40E=(struct Cyc_CfFlowInfo_Place*)((_tmp40B.f2).PlaceL).val;_LL271: {
# 1719
void*_tmp40F=Cyc_CfFlowInfo_lookup_place(_tmp40C,_tmp40E);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp410=_tmp40F;struct Cyc_Absyn_Vardecl*_tmp412;void*_tmp413;_LL275: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp411=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp410;if(_tmp411->tag != 8)goto _LL277;else{_tmp412=_tmp411->f1;_tmp413=(void*)_tmp411->f2;}}_LL276:
 _tmp40F=_tmp413;locname=_tmp412;goto retry;_LL277: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp414=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp410;if(_tmp414->tag != 7)goto _LL279;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp415=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp414->f3);if(_tmp415->tag != 5)goto _LL279;}}_LL278:
 goto _LL27A;_LL279: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp416=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp410;if(_tmp416->tag != 5)goto _LL27B;}_LL27A:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp409,_tmp3F5,_tmp409,_tmp40F,flds);_LL27B:;_LL27C: {
# 1731
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3D9,_tmp40C,_tmp40F);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3D9->unknown_all: _tmp3D9->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3D9,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3F5->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp840;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp83F;void*new_root=(void*)((_tmp83F=_region_malloc(_tmp3D9->r,sizeof(*_tmp83F)),((_tmp83F[0]=((_tmp840.tag=1,((_tmp840.f1=e,((_tmp840.f2=(void*)_check_null(e->topt),_tmp840)))))),_tmp83F))));
struct Cyc_CfFlowInfo_Place*_tmp841;struct Cyc_CfFlowInfo_Place*place=(_tmp841=_region_malloc(_tmp3D9->r,sizeof(*_tmp841)),((_tmp841->root=new_root,((_tmp841->fields=0,_tmp841)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp844;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp843;void*res=(void*)((_tmp843=_region_malloc(_tmp3D9->r,sizeof(*_tmp843)),((_tmp843[0]=((_tmp844.tag=5,((_tmp844.f1=place,_tmp844)))),_tmp843))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp847;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp846;res=(void*)((_tmp846=_region_malloc(_tmp3D9->r,sizeof(*_tmp846)),((_tmp846[0]=((_tmp847.tag=8,((_tmp847.f1=locname,((_tmp847.f2=res,_tmp847)))))),_tmp846))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp40C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp40C,new_root,new_rval);
_tmp40C=Cyc_CfFlowInfo_assign_place(_tmp3D9,e->loc,_tmp40C,env->all_changed,_tmp40E,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp419=Cyc_CfFlowInfo_ReachableFL(_tmp40C,_tmp40D);
# 1746
return Cyc_NewControlFlow_anal_derefL(env,_tmp419,_tmp3F5,_tmp419,res,flds);};}_LL274:;}}_LL272:;_LL273:
# 1748
 goto _LL26F;_LL26F:;};}{
# 1751
struct _tuple15 _stmttmp5E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3F5);union Cyc_CfFlowInfo_FlowInfo _tmp421;void*_tmp422;struct _tuple15 _tmp420=_stmttmp5E;_tmp421=_tmp420.f1;_tmp422=_tmp420.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F5,_tmp421,_tmp422,flds);};_LL264: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3F6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3F6->tag != 22)goto _LL266;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;}}_LL265: {
# 1755
void*_stmttmp59=Cyc_Tcutil_compress((void*)_check_null(_tmp3F7->topt));void*_tmp423=_stmttmp59;union Cyc_Absyn_Constraint*_tmp426;_LL27E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp423;if(_tmp424->tag != 10)goto _LL280;}_LL27F: {
# 1757
unsigned int _tmp427=(Cyc_Evexp_eval_const_uint_exp(_tmp3F8)).f1;
struct Cyc_List_List*_tmp848;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3F7,((_tmp848=_region_malloc(env->r,sizeof(*_tmp848)),((_tmp848->hd=(void*)((int)_tmp427),((_tmp848->tl=flds,_tmp848)))))));}_LL280: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp425=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp423;if(_tmp425->tag != 5)goto _LL282;else{_tmp426=((_tmp425->f1).ptr_atts).bounds;}}_LL281: {
# 1760
struct _RegionHandle _tmp429=_new_region("temp");struct _RegionHandle*temp=& _tmp429;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp849[2];struct _tuple21 _stmttmp5A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp849[1]=_tmp3F8,((_tmp849[0]=_tmp3F7,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp849,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1761
union Cyc_CfFlowInfo_FlowInfo _tmp42B;struct Cyc_List_List*_tmp42C;struct _tuple21 _tmp42A=_stmttmp5A;_tmp42B=_tmp42A.f1;_tmp42C=_tmp42A.f2;{
# 1763
union Cyc_CfFlowInfo_FlowInfo _tmp42D=_tmp42B;
{union Cyc_CfFlowInfo_FlowInfo _tmp42E=_tmp42B;struct Cyc_Dict_Dict _tmp42F;struct Cyc_List_List*_tmp430;_LL285: if((_tmp42E.ReachableFL).tag != 2)goto _LL287;_tmp42F=((struct _tuple14)(_tmp42E.ReachableFL).val).f1;_tmp430=((struct _tuple14)(_tmp42E.ReachableFL).val).f2;_LL286:
# 1766
 if(Cyc_CfFlowInfo_initlevel(_tmp3D9,_tmp42F,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp42C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp84C;void*_tmp84B;(_tmp84B=0,Cyc_Tcutil_terr(_tmp3F8->loc,((_tmp84C="expression may not be initialized",_tag_dyneither(_tmp84C,sizeof(char),34))),_tag_dyneither(_tmp84B,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp433=Cyc_NewControlFlow_add_subscript_reln(_tmp3D9->r,_tmp430,_tmp3F7,_tmp3F8);
if(_tmp430 != _tmp433)
_tmp42D=Cyc_CfFlowInfo_ReachableFL(_tmp42F,_tmp433);
goto _LL284;};_LL287:;_LL288:
 goto _LL284;_LL284:;}{
# 1774
struct _tuple16 _stmttmp5B=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3F7,_tmp42B,(void*)((struct Cyc_List_List*)_check_null(_tmp42C))->hd,flds);union Cyc_CfFlowInfo_FlowInfo _tmp435;union Cyc_CfFlowInfo_AbsLVal _tmp436;struct _tuple16 _tmp434=_stmttmp5B;_tmp435=_tmp434.f1;_tmp436=_tmp434.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp437=_tmp435;_LL28A: if((_tmp437.BottomFL).tag != 1)goto _LL28C;_LL28B: {
struct _tuple16 _tmp84D;struct _tuple16 _tmp439=(_tmp84D.f1=_tmp435,((_tmp84D.f2=_tmp436,_tmp84D)));_npop_handler(0);return _tmp439;}_LL28C:;_LL28D: {
struct _tuple16 _tmp84E;struct _tuple16 _tmp43B=(_tmp84E.f1=_tmp42D,((_tmp84E.f2=_tmp436,_tmp84E)));_npop_handler(0);return _tmp43B;}_LL289:;};};};}
# 1761
;_pop_region(temp);}_LL282:;_LL283: {
# 1780
struct Cyc_Core_Impossible_exn_struct _tmp854;const char*_tmp853;struct Cyc_Core_Impossible_exn_struct*_tmp852;(int)_throw((void*)((_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852[0]=((_tmp854.tag=Cyc_Core_Impossible,((_tmp854.f1=((_tmp853="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp853,sizeof(char),33))),_tmp854)))),_tmp852)))));}_LL27D:;}_LL266: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3F9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3DE;if(_tmp3F9->tag != 20)goto _LL268;else{_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3F9->f2;}}_LL267: {
# 1784
void*_tmp440=(void*)_check_null(_tmp3FA->topt);
if(Cyc_Absyn_is_require_union_type(_tmp440))
Cyc_NewControlFlow_check_requires(_tmp3FA->loc,_tmp3D9->r,_tmp440,_tmp3FB,inflow);
# 1788
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp440)){
struct _tuple16 _tmp855;return(_tmp855.f1=inflow,((_tmp855.f2=Cyc_CfFlowInfo_UnknownL(),_tmp855)));}{
# 1791
struct Cyc_List_List*_tmp856;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3FA,(
(_tmp856=_region_malloc(env->r,sizeof(*_tmp856)),((_tmp856->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp440,_tmp3FB),((_tmp856->tl=flds,_tmp856)))))));};}_LL268:;_LL269: {
# 1794
struct _tuple16 _tmp857;return(_tmp857.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp857.f2=Cyc_CfFlowInfo_UnknownL(),_tmp857)));}_LL251:;};}
# 1798
static struct _tuple16 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1800
struct _tuple16 _stmttmp60=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp445;union Cyc_CfFlowInfo_AbsLVal _tmp446;struct _tuple16 _tmp444=_stmttmp60;_tmp445=_tmp444.f1;_tmp446=_tmp444.f2;{
struct _tuple16 _tmp858;return(_tmp858.f1=_tmp445,((_tmp858.f2=_tmp446,_tmp858)));};}
# 1807
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1809
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple16 _stmttmp61=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp449;struct _tuple16 _tmp448=_stmttmp61;_tmp449=_tmp448.f1;
inflow=_tmp449;}}}
# 1816
return inflow;}
# 1822
static struct _tuple17 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1825
struct Cyc_CfFlowInfo_FlowEnv*_tmp44A=env->fenv;
# 1828
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp44B=_new_region("temp");struct _RegionHandle*temp=& _tmp44B;_push_region(temp);{
struct _tuple21 _stmttmp62=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp44D;struct Cyc_List_List*_tmp44E;struct _tuple21 _tmp44C=_stmttmp62;_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp44E))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp44E->tl))->hd;
f=_tmp44D;}{
# 1838
union Cyc_CfFlowInfo_FlowInfo _tmp44F=f;struct Cyc_Dict_Dict _tmp450;struct Cyc_List_List*_tmp451;_LL28F: if((_tmp44F.BottomFL).tag != 1)goto _LL291;_LL290: {
struct _tuple17 _tmp859;struct _tuple17 _tmp453=(_tmp859.f1=f,((_tmp859.f2=f,_tmp859)));_npop_handler(0);return _tmp453;}_LL291: if((_tmp44F.ReachableFL).tag != 2)goto _LL28E;_tmp450=((struct _tuple14)(_tmp44F.ReachableFL).val).f1;_tmp451=((struct _tuple14)(_tmp44F.ReachableFL).val).f2;_LL292: {
# 1841
struct Cyc_Absyn_Exp*_tmp454=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp455=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp450,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85C;void*_tmp85B;(_tmp85B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp85C="expression may not be initialized",_tag_dyneither(_tmp85C,sizeof(char),34))),_tag_dyneither(_tmp85B,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp450,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp85F="expression may not be initialized",_tag_dyneither(_tmp85F,sizeof(char),34))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}{
# 1848
union Cyc_CfFlowInfo_FlowInfo _tmp45A=f;
union Cyc_CfFlowInfo_FlowInfo _tmp45B=f;
# 1853
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp45C=r1;struct Cyc_Absyn_Vardecl*_tmp45E;void*_tmp45F;_LL294: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp45D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp45C;if(_tmp45D->tag != 8)goto _LL296;else{_tmp45E=_tmp45D->f1;_tmp45F=(void*)_tmp45D->f2;}}_LL295:
 r1=_tmp45F;r1n=_tmp45E;goto _LL293;_LL296:;_LL297:
 goto _LL293;_LL293:;}
# 1859
{void*_tmp460=r2;struct Cyc_Absyn_Vardecl*_tmp462;void*_tmp463;_LL299: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp461=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp460;if(_tmp461->tag != 8)goto _LL29B;else{_tmp462=_tmp461->f1;_tmp463=(void*)_tmp461->f2;}}_LL29A:
 r2=_tmp463;r2n=_tmp462;goto _LL298;_LL29B:;_LL29C:
 goto _LL298;_LL298:;}{
# 1864
struct _tuple0 _tmp860;struct _tuple0 _stmttmp63=(_tmp860.f1=r1,((_tmp860.f2=r2,_tmp860)));struct _tuple0 _tmp464=_stmttmp63;enum Cyc_CfFlowInfo_InitLevel _tmp466;enum Cyc_CfFlowInfo_InitLevel _tmp46A;_LL29E:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp465=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp465->tag != 3)goto _LL2A0;else{_tmp466=_tmp465->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp467=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp467->tag != 0)goto _LL2A0;};_LL29F: {
# 1868
struct _tuple17 _stmttmp65=Cyc_NewControlFlow_splitzero(env,f,f,_tmp454,_tmp466,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp47B;struct _tuple17 _tmp479=_stmttmp65;_tmp47A=_tmp479.f1;_tmp47B=_tmp479.f2;
switch(p){case Cyc_Absyn_Eq: _LL2B2:
 _tmp45A=_tmp47B;_tmp45B=_tmp47A;break;case Cyc_Absyn_Neq: _LL2B3:
 _tmp45A=_tmp47A;_tmp45B=_tmp47B;break;default: _LL2B4: {
struct Cyc_Core_Impossible_exn_struct _tmp866;const char*_tmp865;struct Cyc_Core_Impossible_exn_struct*_tmp864;(int)_throw((void*)((_tmp864=_cycalloc(sizeof(*_tmp864)),((_tmp864[0]=((_tmp866.tag=Cyc_Core_Impossible,((_tmp866.f1=((_tmp865="anal_test, zero-split",_tag_dyneither(_tmp865,sizeof(char),22))),_tmp866)))),_tmp864)))));}}
# 1874
goto _LL29D;}_LL2A0:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp468=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp468->tag != 0)goto _LL2A2;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp469=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp469->tag != 3)goto _LL2A2;else{_tmp46A=_tmp469->f1;}};_LL2A1: {
# 1876
struct _tuple17 _stmttmp64=Cyc_NewControlFlow_splitzero(env,f,f,_tmp455,_tmp46A,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp480;union Cyc_CfFlowInfo_FlowInfo _tmp481;struct _tuple17 _tmp47F=_stmttmp64;_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;
switch(p){case Cyc_Absyn_Eq: _LL2B6:
 _tmp45A=_tmp481;_tmp45B=_tmp480;break;case Cyc_Absyn_Neq: _LL2B7:
 _tmp45A=_tmp480;_tmp45B=_tmp481;break;default: _LL2B8: {
struct Cyc_Core_Impossible_exn_struct _tmp86C;const char*_tmp86B;struct Cyc_Core_Impossible_exn_struct*_tmp86A;(int)_throw((void*)((_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86C.tag=Cyc_Core_Impossible,((_tmp86C.f1=((_tmp86B="anal_test, zero-split",_tag_dyneither(_tmp86B,sizeof(char),22))),_tmp86C)))),_tmp86A)))));}}
# 1882
goto _LL29D;}_LL2A2:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp46B->tag != 0)goto _LL2A4;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp46C->tag != 0)goto _LL2A4;};_LL2A3:
# 1884
 if(p == Cyc_Absyn_Eq)_tmp45B=Cyc_CfFlowInfo_BottomFL();else{
_tmp45A=Cyc_CfFlowInfo_BottomFL();}
goto _LL29D;_LL2A4:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp46D->tag != 0)goto _LL2A6;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp46E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp46E->tag != 1)goto _LL2A6;};_LL2A5:
 goto _LL2A7;_LL2A6:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp46F->tag != 0)goto _LL2A8;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp470=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp470->tag != 2)goto _LL2A8;};_LL2A7:
 goto _LL2A9;_LL2A8:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp471=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp471->tag != 0)goto _LL2AA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp472=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp472->tag != 5)goto _LL2AA;};_LL2A9:
 goto _LL2AB;_LL2AA:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp473=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp473->tag != 1)goto _LL2AC;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp474=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp474->tag != 0)goto _LL2AC;};_LL2AB:
 goto _LL2AD;_LL2AC:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp475=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp475->tag != 2)goto _LL2AE;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp476=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp476->tag != 0)goto _LL2AE;};_LL2AD:
 goto _LL2AF;_LL2AE:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp477=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp464.f1;if(_tmp477->tag != 5)goto _LL2B0;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp478=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp464.f2;if(_tmp478->tag != 0)goto _LL2B0;};_LL2AF:
# 1893
 if(p == Cyc_Absyn_Neq)_tmp45B=Cyc_CfFlowInfo_BottomFL();else{
_tmp45A=Cyc_CfFlowInfo_BottomFL();}
goto _LL29D;_LL2B0:;_LL2B1:
 goto _LL29D;_LL29D:;};}
# 1904
{struct _tuple0 _tmp86D;struct _tuple0 _stmttmp66=(_tmp86D.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp454->topt)),((_tmp86D.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp455->topt)),_tmp86D)));struct _tuple0 _tmp486=_stmttmp66;_LL2BB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp486.f1;if(_tmp487->tag != 6)goto _LL2BD;else{if(_tmp487->f1 != Cyc_Absyn_Unsigned)goto _LL2BD;}}_LL2BC:
 goto _LL2BE;_LL2BD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp488=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp486.f2;if(_tmp488->tag != 6)goto _LL2BF;else{if(_tmp488->f1 != Cyc_Absyn_Unsigned)goto _LL2BF;}}_LL2BE:
 goto _LL2C0;_LL2BF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp489=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp486.f1;if(_tmp489->tag != 19)goto _LL2C1;}_LL2C0:
 goto _LL2C2;_LL2C1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp486.f2;if(_tmp48A->tag != 19)goto _LL2C3;}_LL2C2:
 goto _LL2BA;_LL2C3:;_LL2C4: {
struct _tuple17 _tmp86E;struct _tuple17 _tmp48C=(_tmp86E.f1=_tmp45A,((_tmp86E.f2=_tmp45B,_tmp86E)));_npop_handler(0);return _tmp48C;}_LL2BA:;}
# 1913
switch(p){case Cyc_Absyn_Eq: _LL2C5:
 goto _LL2C6;case Cyc_Absyn_Neq: _LL2C6: goto _LL2C7;case Cyc_Absyn_Gt: _LL2C7: goto _LL2C8;case Cyc_Absyn_Gte: _LL2C8: goto _LL2C9;case Cyc_Absyn_Lt: _LL2C9: goto _LL2CA;case Cyc_Absyn_Lte: _LL2CA: break;default: _LL2CB: {
struct _tuple17 _tmp86F;struct _tuple17 _tmp48F=(_tmp86F.f1=_tmp45A,((_tmp86F.f2=_tmp45B,_tmp86F)));_npop_handler(0);return _tmp48F;}}{
# 1918
struct _RegionHandle*_tmp490=(env->fenv)->r;
struct _tuple13 _stmttmp67=Cyc_Relations_primop2relation(_tmp454,p,_tmp455);struct Cyc_Absyn_Exp*_tmp492;enum Cyc_Relations_Relation _tmp493;struct Cyc_Absyn_Exp*_tmp494;struct _tuple13 _tmp491=_stmttmp67;_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;_tmp494=_tmp491.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1923
if(Cyc_Relations_exp2relnop(_tmp492,& n1) && Cyc_Relations_exp2relnop(_tmp494,& n2)){
# 1925
struct Cyc_List_List*_tmp495=Cyc_Relations_add_relation(_tmp490,n1,_tmp493,n2,_tmp451);
# 1929
struct Cyc_List_List*_tmp496=Cyc_Relations_add_relation(_tmp490,n2,Cyc_Relations_flip_relation(_tmp493),n1,_tmp451);
struct _tuple17 _tmp870;struct _tuple17 _stmttmp68=(_tmp870.f1=_tmp45A,((_tmp870.f2=_tmp45B,_tmp870)));struct _tuple17 _tmp497=_stmttmp68;struct Cyc_Dict_Dict _tmp498;struct Cyc_Dict_Dict _tmp499;struct Cyc_Dict_Dict _tmp49A;struct Cyc_Dict_Dict _tmp49B;_LL2CE: if(((_tmp497.f1).ReachableFL).tag != 2)goto _LL2D0;_tmp498=((struct _tuple14)((_tmp497.f1).ReachableFL).val).f1;if(((_tmp497.f2).ReachableFL).tag != 2)goto _LL2D0;_tmp499=((struct _tuple14)((_tmp497.f2).ReachableFL).val).f1;_LL2CF: {
# 1932
struct _tuple17 _tmp871;struct _tuple17 _tmp49D=(_tmp871.f1=Cyc_CfFlowInfo_ReachableFL(_tmp498,_tmp495),((_tmp871.f2=Cyc_CfFlowInfo_ReachableFL(_tmp499,_tmp496),_tmp871)));_npop_handler(0);return _tmp49D;}_LL2D0: if(((_tmp497.f2).ReachableFL).tag != 2)goto _LL2D2;_tmp49A=((struct _tuple14)((_tmp497.f2).ReachableFL).val).f1;_LL2D1: {
# 1934
struct _tuple17 _tmp872;struct _tuple17 _tmp49F=(_tmp872.f1=_tmp45A,((_tmp872.f2=Cyc_CfFlowInfo_ReachableFL(_tmp49A,_tmp496),_tmp872)));_npop_handler(0);return _tmp49F;}_LL2D2: if(((_tmp497.f1).ReachableFL).tag != 2)goto _LL2D4;_tmp49B=((struct _tuple14)((_tmp497.f1).ReachableFL).val).f1;_LL2D3: {
# 1936
struct _tuple17 _tmp873;struct _tuple17 _tmp4A1=(_tmp873.f1=Cyc_CfFlowInfo_ReachableFL(_tmp49B,_tmp495),((_tmp873.f2=_tmp45B,_tmp873)));_npop_handler(0);return _tmp4A1;}_LL2D4:;_LL2D5: {
# 1938
struct _tuple17 _tmp874;struct _tuple17 _tmp4A3=(_tmp874.f1=_tmp45A,((_tmp874.f2=_tmp45B,_tmp874)));_npop_handler(0);return _tmp4A3;}_LL2CD:;}else{
# 1941
struct _tuple17 _tmp875;struct _tuple17 _tmp4A6=(_tmp875.f1=_tmp45A,((_tmp875.f2=_tmp45B,_tmp875)));_npop_handler(0);return _tmp4A6;}};};};}_LL28E:;};
# 1830
;_pop_region(temp);}
# 1947
static struct _tuple17 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1949
struct Cyc_CfFlowInfo_FlowEnv*_tmp4A7=env->fenv;
void*_stmttmp69=e->r;void*_tmp4A8=_stmttmp69;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;enum Cyc_Absyn_Primop _tmp4B9;struct Cyc_List_List*_tmp4BA;_LL2D7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4A9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4A9->tag != 5)goto _LL2D9;else{_tmp4AA=_tmp4A9->f1;_tmp4AB=_tmp4A9->f2;_tmp4AC=_tmp4A9->f3;}}_LL2D8: {
# 1952
struct _tuple17 _stmttmp71=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4AA);union Cyc_CfFlowInfo_FlowInfo _tmp4BC;union Cyc_CfFlowInfo_FlowInfo _tmp4BD;struct _tuple17 _tmp4BB=_stmttmp71;_tmp4BC=_tmp4BB.f1;_tmp4BD=_tmp4BB.f2;{
struct _tuple17 _stmttmp72=Cyc_NewControlFlow_anal_test(env,_tmp4BC,_tmp4AB);union Cyc_CfFlowInfo_FlowInfo _tmp4BF;union Cyc_CfFlowInfo_FlowInfo _tmp4C0;struct _tuple17 _tmp4BE=_stmttmp72;_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;{
struct _tuple17 _stmttmp73=Cyc_NewControlFlow_anal_test(env,_tmp4BD,_tmp4AC);union Cyc_CfFlowInfo_FlowInfo _tmp4C2;union Cyc_CfFlowInfo_FlowInfo _tmp4C3;struct _tuple17 _tmp4C1=_stmttmp73;_tmp4C2=_tmp4C1.f1;_tmp4C3=_tmp4C1.f2;{
struct _tuple17 _tmp876;return(_tmp876.f1=Cyc_CfFlowInfo_join_flow(_tmp4A7,env->all_changed,_tmp4BF,_tmp4C2),((_tmp876.f2=
Cyc_CfFlowInfo_join_flow(_tmp4A7,env->all_changed,_tmp4C0,_tmp4C3),_tmp876)));};};};}_LL2D9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4AD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4AD->tag != 6)goto _LL2DB;else{_tmp4AE=_tmp4AD->f1;_tmp4AF=_tmp4AD->f2;}}_LL2DA: {
# 1958
struct _tuple17 _stmttmp6F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4AE);union Cyc_CfFlowInfo_FlowInfo _tmp4C6;union Cyc_CfFlowInfo_FlowInfo _tmp4C7;struct _tuple17 _tmp4C5=_stmttmp6F;_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;{
struct _tuple17 _stmttmp70=Cyc_NewControlFlow_anal_test(env,_tmp4C6,_tmp4AF);union Cyc_CfFlowInfo_FlowInfo _tmp4C9;union Cyc_CfFlowInfo_FlowInfo _tmp4CA;struct _tuple17 _tmp4C8=_stmttmp70;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;{
struct _tuple17 _tmp877;return(_tmp877.f1=_tmp4C9,((_tmp877.f2=Cyc_CfFlowInfo_join_flow(_tmp4A7,env->all_changed,_tmp4C7,_tmp4CA),_tmp877)));};};}_LL2DB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4B0=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4B0->tag != 7)goto _LL2DD;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B0->f2;}}_LL2DC: {
# 1962
struct _tuple17 _stmttmp6D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4B1);union Cyc_CfFlowInfo_FlowInfo _tmp4CD;union Cyc_CfFlowInfo_FlowInfo _tmp4CE;struct _tuple17 _tmp4CC=_stmttmp6D;_tmp4CD=_tmp4CC.f1;_tmp4CE=_tmp4CC.f2;{
struct _tuple17 _stmttmp6E=Cyc_NewControlFlow_anal_test(env,_tmp4CE,_tmp4B2);union Cyc_CfFlowInfo_FlowInfo _tmp4D0;union Cyc_CfFlowInfo_FlowInfo _tmp4D1;struct _tuple17 _tmp4CF=_stmttmp6E;_tmp4D0=_tmp4CF.f1;_tmp4D1=_tmp4CF.f2;{
struct _tuple17 _tmp878;return(_tmp878.f1=Cyc_CfFlowInfo_join_flow(_tmp4A7,env->all_changed,_tmp4CD,_tmp4D0),((_tmp878.f2=_tmp4D1,_tmp878)));};};}_LL2DD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4B3->tag != 8)goto _LL2DF;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;}}_LL2DE: {
# 1966
struct _tuple15 _stmttmp6C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4B4);union Cyc_CfFlowInfo_FlowInfo _tmp4D4;void*_tmp4D5;struct _tuple15 _tmp4D3=_stmttmp6C;_tmp4D4=_tmp4D3.f1;_tmp4D5=_tmp4D3.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4D4,_tmp4B5);}_LL2DF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4B6->tag != 2)goto _LL2E1;else{if(_tmp4B6->f1 != Cyc_Absyn_Not)goto _LL2E1;if(_tmp4B6->f2 == 0)goto _LL2E1;_tmp4B7=(struct Cyc_Absyn_Exp*)(_tmp4B6->f2)->hd;if((_tmp4B6->f2)->tl != 0)goto _LL2E1;}}_LL2E0: {
# 1969
struct _tuple17 _stmttmp6B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4B7);union Cyc_CfFlowInfo_FlowInfo _tmp4D7;union Cyc_CfFlowInfo_FlowInfo _tmp4D8;struct _tuple17 _tmp4D6=_stmttmp6B;_tmp4D7=_tmp4D6.f1;_tmp4D8=_tmp4D6.f2;{
struct _tuple17 _tmp879;return(_tmp879.f1=_tmp4D8,((_tmp879.f2=_tmp4D7,_tmp879)));};}_LL2E1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A8;if(_tmp4B8->tag != 2)goto _LL2E3;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;}}_LL2E2:
# 1972
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4B9,_tmp4BA);_LL2E3:;_LL2E4: {
# 1976
struct _tuple15 _stmttmp6A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4DB;void*_tmp4DC;struct _tuple15 _tmp4DA=_stmttmp6A;_tmp4DB=_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4DD=_tmp4DB;struct Cyc_Dict_Dict _tmp4DE;_LL2E6: if((_tmp4DD.BottomFL).tag != 1)goto _LL2E8;_LL2E7: {
struct _tuple17 _tmp87A;return(_tmp87A.f1=_tmp4DB,((_tmp87A.f2=_tmp4DB,_tmp87A)));}_LL2E8: if((_tmp4DD.ReachableFL).tag != 2)goto _LL2E5;_tmp4DE=((struct _tuple14)(_tmp4DD.ReachableFL).val).f1;_LL2E9: {
# 1980
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4E0=_tmp4DC;struct Cyc_Absyn_Vardecl*_tmp4E2;void*_tmp4E3;enum Cyc_CfFlowInfo_InitLevel _tmp4EC;_LL2EB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E1->tag != 8)goto _LL2ED;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=(void*)_tmp4E1->f2;}}_LL2EC:
 _tmp4DC=_tmp4E3;locname=_tmp4E2;goto retry;_LL2ED: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E4->tag != 0)goto _LL2EF;}_LL2EE: {
struct _tuple17 _tmp87B;return(_tmp87B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87B.f2=_tmp4DB,_tmp87B)));}_LL2EF: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4E5=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E5->tag != 2)goto _LL2F1;}_LL2F0:
 goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4E6=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E6->tag != 1)goto _LL2F3;}_LL2F2:
 goto _LL2F4;_LL2F3: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4E7=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E7->tag != 5)goto _LL2F5;}_LL2F4: {
struct _tuple17 _tmp87C;return(_tmp87C.f1=_tmp4DB,((_tmp87C.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87C)));}_LL2F5: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E8=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E8->tag != 3)goto _LL2F7;else{if(_tmp4E8->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F7;}}_LL2F6:
 goto _LL2F8;_LL2F7: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4E9=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4E9->tag != 4)goto _LL2F9;else{if(_tmp4E9->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F9;}}_LL2F8:
 goto _LL2FA;_LL2F9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4EA=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4EA->tag != 7)goto _LL2FB;}_LL2FA:
# 1990
{const char*_tmp87F;void*_tmp87E;(_tmp87E=0,Cyc_Tcutil_terr(e->loc,((_tmp87F="expression may not be initialized",_tag_dyneither(_tmp87F,sizeof(char),34))),_tag_dyneither(_tmp87E,sizeof(void*),0)));}{
struct _tuple17 _tmp880;return(_tmp880.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp880.f2=Cyc_CfFlowInfo_BottomFL(),_tmp880)));};_LL2FB: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4EB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4EB->tag != 3)goto _LL2FD;else{_tmp4EC=_tmp4EB->f1;}}_LL2FC:
# 1993
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4DB,e,_tmp4EC,locname);_LL2FD: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4ED=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4ED->tag != 4)goto _LL2FF;}_LL2FE: {
struct _tuple17 _tmp881;return(_tmp881.f1=_tmp4DB,((_tmp881.f2=_tmp4DB,_tmp881)));}_LL2FF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4EE=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E0;if(_tmp4EE->tag != 6)goto _LL2EA;}_LL300: {
struct Cyc_Core_Impossible_exn_struct _tmp887;const char*_tmp886;struct Cyc_Core_Impossible_exn_struct*_tmp885;(int)_throw((void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp887.tag=Cyc_Core_Impossible,((_tmp887.f1=((_tmp886="anal_test",_tag_dyneither(_tmp886,sizeof(char),10))),_tmp887)))),_tmp885)))));}_LL2EA:;}}_LL2E5:;};}_LL2D6:;}struct _tuple25{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2001
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple25*ckenv,void*root,void*rval){
# 2003
unsigned int _tmp4F9;struct Cyc_NewControlFlow_AnalEnv*_tmp4FA;struct Cyc_Dict_Dict _tmp4FB;struct _tuple25*_tmp4F8=ckenv;_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F8->f2;_tmp4FB=_tmp4F8->f3;{
void*_tmp4FC=root;struct Cyc_Absyn_Vardecl*_tmp4FE;_LL302: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FD=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FC;if(_tmp4FD->tag != 0)goto _LL304;else{_tmp4FE=_tmp4FD->f1;}}_LL303:
# 2006
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4FA->fenv)->r,_tmp4FE->type)){
void*_tmp4FF=rval;_LL307: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp500=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp500->tag != 8)goto _LL309;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp501=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp500->f2);if(_tmp501->tag != 7)goto _LL309;}}_LL308:
 goto _LL30A;_LL309: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp502=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp502->tag != 8)goto _LL30B;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp503=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp502->f2);if(_tmp503->tag != 0)goto _LL30B;}}_LL30A:
 goto _LL30C;_LL30B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp504=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp504->tag != 8)goto _LL30D;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp505=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp504->f2);if(_tmp505->tag != 3)goto _LL30D;else{if(_tmp505->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL30D;}}}_LL30C:
 goto _LL30E;_LL30D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp506=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp506->tag != 7)goto _LL30F;}_LL30E:
 goto _LL310;_LL30F: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp507=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp507->tag != 0)goto _LL311;}_LL310:
 goto _LL312;_LL311: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp508=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4FF;if(_tmp508->tag != 3)goto _LL313;else{if(_tmp508->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL313;}}_LL312:
 goto _LL306;_LL313:;_LL314:
# 2015
{const char*_tmp88B;void*_tmp88A[1];struct Cyc_String_pa_PrintArg_struct _tmp889;(_tmp889.tag=0,((_tmp889.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4FE->name)),((_tmp88A[0]=& _tmp889,Cyc_Tcutil_warn(_tmp4F9,((_tmp88B="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp88B,sizeof(char),57))),_tag_dyneither(_tmp88A,sizeof(void*),1)))))));}
goto _LL306;_LL306:;}
# 2018
goto _LL301;_LL304:;_LL305:
 goto _LL301;_LL301:;};}
# 2023
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp50C=flow;struct Cyc_Dict_Dict _tmp50D;_LL316: if((_tmp50C.BottomFL).tag != 1)goto _LL318;_LL317:
 return;_LL318: if((_tmp50C.ReachableFL).tag != 2)goto _LL315;_tmp50D=((struct _tuple14)(_tmp50C.ReachableFL).val).f1;_LL319:
# 2027
{struct Cyc_List_List*_tmp50E=env->param_roots;for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp50D,Cyc_CfFlowInfo_lookup_place(_tmp50D,(struct Cyc_CfFlowInfo_Place*)_tmp50E->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp88E;void*_tmp88D;(_tmp88D=0,Cyc_Tcutil_terr(loc,((_tmp88E="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp88E,sizeof(char),76))),_tag_dyneither(_tmp88D,sizeof(void*),0)));}}}
# 2031
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple25 _tmp88F;struct _tuple25 _tmp511=(_tmp88F.f1=loc,((_tmp88F.f2=env,((_tmp88F.f3=_tmp50D,_tmp88F)))));
((void(*)(void(*f)(struct _tuple25*,void*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp511,_tmp50D);}
# 2035
return;_LL315:;}struct _tuple26{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple27{void*f1;void*f2;void*f3;};
# 2039
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 2041
union Cyc_CfFlowInfo_FlowInfo _tmp513=flow;struct Cyc_Dict_Dict _tmp514;_LL31B: if((_tmp513.BottomFL).tag != 1)goto _LL31D;_LL31C:
 return;_LL31D: if((_tmp513.ReachableFL).tag != 2)goto _LL31A;_tmp514=((struct _tuple14)(_tmp513.ReachableFL).val).f1;_LL31E: {
# 2044
struct Cyc_Absyn_Exp*_tmp515=Cyc_Absyn_exp_unsigned_one;
int _tmp516=0;
int _tmp517=1;
{struct Cyc_List_List*_tmp518=env->noconsume_params;for(0;_tmp518 != 0;_tmp518=_tmp518->tl){
# 2050
struct _tuple26*_stmttmp74=(struct _tuple26*)_tmp518->hd;struct Cyc_CfFlowInfo_Place*_tmp51A;void*_tmp51B;struct _tuple26*_tmp519=_stmttmp74;_tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;{
void*_tmp51C=Cyc_CfFlowInfo_lookup_place(_tmp514,_tmp51A);
void*_tmp51E;struct Cyc_CfFlowInfo_Place*_tmp51D=_tmp51A;_tmp51E=_tmp51D->root;{
struct _tuple27 _tmp890;struct _tuple27 _stmttmp75=(_tmp890.f1=_tmp51E,((_tmp890.f2=_tmp51B,((_tmp890.f3=_tmp51C,_tmp890)))));struct _tuple27 _tmp51F=_stmttmp75;struct Cyc_Absyn_Vardecl*_tmp521;struct Cyc_Absyn_Vardecl*_tmp523;void*_tmp524;struct Cyc_Absyn_Vardecl*_tmp526;void*_tmp527;struct Cyc_Absyn_Vardecl*_tmp529;_LL320:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp520=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51F.f1;if(_tmp520->tag != 0)goto _LL322;else{_tmp521=_tmp520->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp522=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp51F.f2;if(_tmp522->tag != 8)goto _LL322;else{_tmp523=_tmp522->f1;_tmp524=(void*)_tmp522->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp525=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp51F.f3;if(_tmp525->tag != 8)goto _LL322;else{_tmp526=_tmp525->f1;_tmp527=(void*)_tmp525->f2;}};_LL321:
# 2055
 if(_tmp523 == _tmp526){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp515,_tmp517,_tmp527,& _tmp516)){
const char*_tmp894;void*_tmp893[1];struct Cyc_String_pa_PrintArg_struct _tmp892;(_tmp892.tag=0,((_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2059
Cyc_Absynpp_qvar2string(_tmp521->name)),((_tmp893[0]=& _tmp892,Cyc_Tcutil_terr(loc,((_tmp894="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp894,sizeof(char),58))),_tag_dyneither(_tmp893,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp514,_tmp527)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp898;void*_tmp897[1];struct Cyc_String_pa_PrintArg_struct _tmp896;(_tmp896.tag=0,((_tmp896.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2063
Cyc_Absynpp_qvar2string(_tmp521->name)),((_tmp897[0]=& _tmp896,Cyc_Tcutil_terr(loc,((_tmp898="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp898,sizeof(char),84))),_tag_dyneither(_tmp897,sizeof(void*),1)))))));}}}else{
# 2067
_tmp529=_tmp521;goto _LL323;}
goto _LL31F;_LL322: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp528=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51F.f1;if(_tmp528->tag != 0)goto _LL324;else{_tmp529=_tmp528->f1;}}_LL323:
# 2070
{const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_PrintArg_struct _tmp89A;(_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2072
Cyc_Absynpp_qvar2string(_tmp529->name)),((_tmp89B[0]=& _tmp89A,Cyc_Tcutil_terr(loc,((_tmp89C="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp89C,sizeof(char),85))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))));}
goto _LL31F;_LL324:;_LL325: {
# 2075
const char*_tmp89F;void*_tmp89E;(_tmp89E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp89F="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp89F,sizeof(char),46))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}_LL31F:;};};}}
# 2078
return;}_LL31A:;}
# 2082
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2085
struct Cyc_CfFlowInfo_FlowEnv*_tmp536=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_stmttmp76=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp538;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Stmt*_tmp53A;unsigned int _tmp53B;struct Cyc_Absyn_Switch_clause*_tmp537=_stmttmp76;_tmp538=_tmp537->pat_vars;_tmp539=_tmp537->where_clause;_tmp53A=_tmp537->body;_tmp53B=_tmp537->loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp538))->v,_tmp53B,exp_loc);
# 2091
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp539 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp539;
struct _tuple17 _stmttmp77=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp53D;union Cyc_CfFlowInfo_FlowInfo _tmp53E;struct _tuple17 _tmp53C=_stmttmp77;_tmp53D=_tmp53C.f1;_tmp53E=_tmp53C.f2;
inflow=_tmp53E;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53D,_tmp53A);}else{
# 2098
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp53A);}{
# 2100
union Cyc_CfFlowInfo_FlowInfo _tmp53F=clause_outflow;_LL327: if((_tmp53F.BottomFL).tag != 1)goto _LL329;_LL328:
 goto _LL326;_LL329:;_LL32A:
# 2104
 if(scs->tl == 0)
return clause_outflow;else{
# 2109
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8A2;void*_tmp8A1;(_tmp8A1=0,Cyc_Tcutil_terr(_tmp53A->loc,((_tmp8A2="switch clause may implicitly fallthru",_tag_dyneither(_tmp8A2,sizeof(char),38))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}else{
# 2112
const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,Cyc_Tcutil_warn(_tmp53A->loc,((_tmp8A5="switch clause may implicitly fallthru",_tag_dyneither(_tmp8A5,sizeof(char),38))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}
# 2114
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2116
goto _LL326;_LL326:;};};}
# 2119
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2124
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2126
struct Cyc_NewControlFlow_AnalEnv*_tmp545;struct Cyc_Dict_Dict _tmp546;unsigned int _tmp547;struct _tuple28*_tmp544=vdenv;_tmp545=_tmp544->f1;_tmp546=_tmp544->f2;_tmp547=_tmp544->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp545->fenv)->r,vd->type)){
# 2129
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8A6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8A6.tag=0,((_tmp8A6.f1=vd,_tmp8A6)));
# 2131
void*_tmp548=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp546,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp549=_tmp548;_LL32C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp54A->tag != 8)goto _LL32E;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp54B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp54A->f2);if(_tmp54B->tag != 7)goto _LL32E;}}_LL32D:
 goto _LL32F;_LL32E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp54C->tag != 8)goto _LL330;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp54D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp54C->f2);if(_tmp54D->tag != 0)goto _LL330;}}_LL32F:
 goto _LL331;_LL330: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp54E->tag != 8)goto _LL332;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp54F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp54E->f2);if(_tmp54F->tag != 3)goto _LL332;else{if(_tmp54F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL332;}}}_LL331:
 goto _LL333;_LL332: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp550=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp550->tag != 7)goto _LL334;}_LL333:
 goto _LL335;_LL334: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp551=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp551->tag != 0)goto _LL336;}_LL335:
 goto _LL337;_LL336: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp552=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp549;if(_tmp552->tag != 3)goto _LL338;else{if(_tmp552->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL338;}}_LL337:
 goto _LL32B;_LL338:;_LL339:
# 2140
{const char*_tmp8AA;void*_tmp8A9[1];struct Cyc_String_pa_PrintArg_struct _tmp8A8;(_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8A9[0]=& _tmp8A8,Cyc_Tcutil_warn(_tmp547,((_tmp8AA="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8AA,sizeof(char),68))),_tag_dyneither(_tmp8A9,sizeof(void*),1)))))));}
# 2142
goto _LL32B;_LL32B:;}}
# 2147
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2149
{union Cyc_CfFlowInfo_FlowInfo _tmp557=inflow;struct Cyc_Dict_Dict _tmp558;_LL33B: if((_tmp557.ReachableFL).tag != 2)goto _LL33D;_tmp558=((struct _tuple14)(_tmp557.ReachableFL).val).f1;_LL33C: {
# 2151
struct _tuple28 _tmp8AB;struct _tuple28 _tmp559=(_tmp8AB.f1=env,((_tmp8AB.f2=_tmp558,((_tmp8AB.f3=decl->loc,_tmp8AB)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp55A=env->fenv;
{void*_stmttmp78=decl->r;void*_tmp55B=_stmttmp78;struct Cyc_Absyn_Vardecl*_tmp55D;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp561;_LL340: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp55C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp55B;if(_tmp55C->tag != 0)goto _LL342;else{_tmp55D=_tmp55C->f1;}}_LL341:
# 2155
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp559,_tmp55D);
goto _LL33F;_LL342: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp55E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp55B;if(_tmp55E->tag != 2)goto _LL344;else{if(_tmp55E->f2 == 0)goto _LL344;_tmp55F=(struct Cyc_List_List*)(_tmp55E->f2)->v;}}_LL343: {
# 2158
struct _tuple1 _stmttmp79=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp55F);struct Cyc_List_List*_tmp563;struct _tuple1 _tmp562=_stmttmp79;_tmp563=_tmp562.f1;{
struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp563);
_tmp561=_tmp564;goto _LL345;};}_LL344: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp560=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp55B;if(_tmp560->tag != 3)goto _LL346;else{_tmp561=_tmp560->f1;}}_LL345:
# 2162
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp559,_tmp561);
goto _LL33F;_LL346:;_LL347:
 goto _LL33F;_LL33F:;}
# 2166
goto _LL33A;}_LL33D:;_LL33E:
 goto _LL33A;_LL33A:;}
# 2169
return;}
# 2172
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2177
struct _tuple18 _stmttmp7A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp567;union Cyc_CfFlowInfo_FlowInfo*_tmp568;struct _tuple18 _tmp566=_stmttmp7A;_tmp567=_tmp566.f1;_tmp568=_tmp566.f2;
inflow=*_tmp568;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp569=env->fenv;
void*_stmttmp7B=s->r;void*_tmp56A=_stmttmp7B;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Stmt*_tmp572;struct Cyc_Absyn_Stmt*_tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Stmt*_tmp576;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Stmt*_tmp57A;struct Cyc_Absyn_Stmt*_tmp57B;struct Cyc_Absyn_Stmt*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Stmt*_tmp583;struct Cyc_Absyn_Exp*_tmp584;struct Cyc_Absyn_Stmt*_tmp585;struct Cyc_Absyn_Stmt*_tmp586;struct Cyc_List_List*_tmp589;struct Cyc_Absyn_Switch_clause*_tmp58A;struct Cyc_Absyn_Stmt*_tmp58C;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Stmt*_tmp590;struct Cyc_Absyn_Exp*_tmp592;struct Cyc_List_List*_tmp593;struct Cyc_Absyn_Stmt*_tmp595;struct Cyc_List_List*_tmp596;struct Cyc_Absyn_Tvar*_tmp599;struct Cyc_Absyn_Vardecl*_tmp59A;struct Cyc_Absyn_Stmt*_tmp59B;struct Cyc_Absyn_Decl*_tmp59D;struct Cyc_Absyn_Stmt*_tmp59E;struct Cyc_Absyn_Stmt*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A2;_LL349: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp56B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp56B->tag != 0)goto _LL34B;}_LL34A:
 return inflow;_LL34B: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp56C=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp56C->tag != 3)goto _LL34D;else{if(_tmp56C->f1 != 0)goto _LL34D;}}_LL34C:
# 2183
 if(env->noreturn){
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,Cyc_Tcutil_terr(s->loc,((_tmp8AE="`noreturn' function might return",_tag_dyneither(_tmp8AE,sizeof(char),33))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL34D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp56D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp56D->tag != 3)goto _LL34F;else{_tmp56E=_tmp56D->f1;}}_LL34E:
# 2189
 if(env->noreturn){
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,Cyc_Tcutil_terr(s->loc,((_tmp8B1="`noreturn' function might return",_tag_dyneither(_tmp8B1,sizeof(char),33))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}{
struct _tuple15 _stmttmp8C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp56E));union Cyc_CfFlowInfo_FlowInfo _tmp5A8;void*_tmp5A9;struct _tuple15 _tmp5A7=_stmttmp8C;_tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;
_tmp5A8=Cyc_NewControlFlow_use_Rval(env,_tmp56E->loc,_tmp5A8,_tmp5A9);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5A8);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp5A8);
return Cyc_CfFlowInfo_BottomFL();};_LL34F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp56F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp56F->tag != 1)goto _LL351;else{_tmp570=_tmp56F->f1;}}_LL350:
# 2197
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp570)).f1;_LL351: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp571=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp571->tag != 2)goto _LL353;else{_tmp572=_tmp571->f1;_tmp573=_tmp571->f2;}}_LL352:
# 2200
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp572),_tmp573);_LL353: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp574=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp574->tag != 4)goto _LL355;else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;_tmp577=_tmp574->f3;}}_LL354: {
# 2203
struct _tuple17 _stmttmp8B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp575);union Cyc_CfFlowInfo_FlowInfo _tmp5AB;union Cyc_CfFlowInfo_FlowInfo _tmp5AC;struct _tuple17 _tmp5AA=_stmttmp8B;_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;{
# 2210
union Cyc_CfFlowInfo_FlowInfo _tmp5AD=Cyc_NewControlFlow_anal_stmt(env,_tmp5AC,_tmp577);
union Cyc_CfFlowInfo_FlowInfo _tmp5AE=Cyc_NewControlFlow_anal_stmt(env,_tmp5AB,_tmp576);
return Cyc_CfFlowInfo_join_flow(_tmp569,env->all_changed,_tmp5AE,_tmp5AD);};}_LL355: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp578=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp578->tag != 5)goto _LL357;else{_tmp579=(_tmp578->f1).f1;_tmp57A=(_tmp578->f1).f2;_tmp57B=_tmp578->f2;}}_LL356: {
# 2218
struct _tuple18 _stmttmp89=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp57A);union Cyc_CfFlowInfo_FlowInfo*_tmp5B0;struct _tuple18 _tmp5AF=_stmttmp89;_tmp5B0=_tmp5AF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B1=*_tmp5B0;
struct _tuple17 _stmttmp8A=Cyc_NewControlFlow_anal_test(env,_tmp5B1,_tmp579);union Cyc_CfFlowInfo_FlowInfo _tmp5B3;union Cyc_CfFlowInfo_FlowInfo _tmp5B4;struct _tuple17 _tmp5B2=_stmttmp8A;_tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B5=Cyc_NewControlFlow_anal_stmt(env,_tmp5B3,_tmp57B);
Cyc_NewControlFlow_update_flow(env,_tmp57A,_tmp5B5);
return _tmp5B4;};};}_LL357: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp57C=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp57C->tag != 14)goto _LL359;else{_tmp57D=_tmp57C->f1;_tmp57E=(_tmp57C->f2).f1;_tmp57F=(_tmp57C->f2).f2;}}_LL358: {
# 2228
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp57D);
struct _tuple18 _stmttmp87=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5B6,_tmp57F);union Cyc_CfFlowInfo_FlowInfo*_tmp5B8;struct _tuple18 _tmp5B7=_stmttmp87;_tmp5B8=_tmp5B7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B9=*_tmp5B8;
struct _tuple17 _stmttmp88=Cyc_NewControlFlow_anal_test(env,_tmp5B9,_tmp57E);union Cyc_CfFlowInfo_FlowInfo _tmp5BB;union Cyc_CfFlowInfo_FlowInfo _tmp5BC;struct _tuple17 _tmp5BA=_stmttmp88;_tmp5BB=_tmp5BA.f1;_tmp5BC=_tmp5BA.f2;
Cyc_NewControlFlow_update_flow(env,_tmp57D,_tmp5BB);
return _tmp5BC;};}_LL359: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp580=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp580->tag != 9)goto _LL35B;else{_tmp581=_tmp580->f1;_tmp582=(_tmp580->f2).f1;_tmp583=(_tmp580->f2).f2;_tmp584=(_tmp580->f3).f1;_tmp585=(_tmp580->f3).f2;_tmp586=_tmp580->f4;}}_LL35A: {
# 2237
union Cyc_CfFlowInfo_FlowInfo _tmp5BD=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp581)).f1;
struct _tuple18 _stmttmp84=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BD,_tmp583);union Cyc_CfFlowInfo_FlowInfo*_tmp5BF;struct _tuple18 _tmp5BE=_stmttmp84;_tmp5BF=_tmp5BE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C0=*_tmp5BF;
struct _tuple17 _stmttmp85=Cyc_NewControlFlow_anal_test(env,_tmp5C0,_tmp582);union Cyc_CfFlowInfo_FlowInfo _tmp5C2;union Cyc_CfFlowInfo_FlowInfo _tmp5C3;struct _tuple17 _tmp5C1=_stmttmp85;_tmp5C2=_tmp5C1.f1;_tmp5C3=_tmp5C1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C4=Cyc_NewControlFlow_anal_stmt(env,_tmp5C2,_tmp586);
struct _tuple18 _stmttmp86=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C4,_tmp585);union Cyc_CfFlowInfo_FlowInfo*_tmp5C6;struct _tuple18 _tmp5C5=_stmttmp86;_tmp5C6=_tmp5C5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C7=*_tmp5C6;
union Cyc_CfFlowInfo_FlowInfo _tmp5C8=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5C7,_tmp584)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp583,_tmp5C8);
return _tmp5C3;};};};}_LL35B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp587=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp587->tag != 6)goto _LL35D;else{if(_tmp587->f1 != 0)goto _LL35D;}}_LL35C:
# 2249
 return Cyc_CfFlowInfo_BottomFL();_LL35D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp588=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp588->tag != 11)goto _LL35F;else{_tmp589=_tmp588->f1;if(_tmp588->f2 == 0)goto _LL35F;_tmp58A=*_tmp588->f2;}}_LL35E: {
# 2251
struct _RegionHandle _tmp5C9=_new_region("temp");struct _RegionHandle*temp=& _tmp5C9;_push_region(temp);
{struct _tuple21 _stmttmp82=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp589,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5CB;struct Cyc_List_List*_tmp5CC;struct _tuple21 _tmp5CA=_stmttmp82;_tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;{
# 2256
struct Cyc_List_List*_tmp5CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp58A->pat_vars))->v)).f1);
_tmp5CB=Cyc_NewControlFlow_add_vars(_tmp569,_tmp5CB,_tmp5CD,_tmp569->unknown_all,s->loc,0);
# 2259
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp58A->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple22*_stmttmp83=(struct _tuple22*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct _tuple22*_tmp5CE=_stmttmp83;_tmp5CF=_tmp5CE->f1;_tmp5D0=_tmp5CE->f2;
if(_tmp5CF != 0){
_tmp5CB=Cyc_NewControlFlow_do_initialize_var(_tmp569,env,_tmp5CB,*_tmp5CF,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp589))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5CC))->hd,s->loc);
_tmp5CC=_tmp5CC->tl;
_tmp589=_tmp589->tl;}}}
# 2267
Cyc_NewControlFlow_update_flow(env,_tmp58A->body,_tmp5CB);{
union Cyc_CfFlowInfo_FlowInfo _tmp5D1=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5D1;};};}
# 2252
;_pop_region(temp);}_LL35F: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp58B->tag != 6)goto _LL361;else{_tmp58C=_tmp58B->f1;}}_LL360:
# 2270
 _tmp58E=_tmp58C;goto _LL362;_LL361: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp58D=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp58D->tag != 7)goto _LL363;else{_tmp58E=_tmp58D->f1;}}_LL362:
 _tmp590=_tmp58E;goto _LL364;_LL363: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp58F=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp58F->tag != 8)goto _LL365;else{_tmp590=_tmp58F->f2;}}_LL364:
# 2275
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5D2=_tmp567->encloser;
struct Cyc_Absyn_Stmt*_tmp5D3=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp590)))->encloser;
while(_tmp5D3 != _tmp5D2){
struct Cyc_Absyn_Stmt*_tmp5D4=(Cyc_NewControlFlow_get_stmt_annot(_tmp5D2))->encloser;
if(_tmp5D4 == _tmp5D2){
{const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,Cyc_Tcutil_terr(s->loc,((_tmp8B4="goto enters local scope or exception handler",_tag_dyneither(_tmp8B4,sizeof(char),45))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}
break;}
# 2284
_tmp5D2=_tmp5D4;}}
# 2289
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp590),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL365: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp591=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp591->tag != 10)goto _LL367;else{_tmp592=_tmp591->f1;_tmp593=_tmp591->f2;}}_LL366:
# 2295
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp593,_tmp592->loc);_LL367: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp594=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp594->tag != 15)goto _LL369;else{_tmp595=_tmp594->f1;_tmp596=_tmp594->f2;}}_LL368: {
# 2300
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp595);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2308
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2311
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp596,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5D7=scs_outflow;_LL374: if((_tmp5D7.BottomFL).tag != 1)goto _LL376;_LL375:
 goto _LL373;_LL376:;_LL377: {
const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,Cyc_Tcutil_terr(s->loc,((_tmp8B7="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8B7,sizeof(char),42))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}_LL373:;}
# 2317
outflow=s1_outflow;
# 2319
return outflow;};};}_LL369: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp597=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp597->tag != 12)goto _LL36B;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp598=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp597->f1)->r;if(_tmp598->tag != 5)goto _LL36B;else{_tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;}}_tmp59B=_tmp597->f2;}}_LL36A: {
# 2325
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp59A->initializer);
struct _tuple16 _stmttmp7E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);union Cyc_CfFlowInfo_FlowInfo _tmp5DB;union Cyc_CfFlowInfo_AbsLVal _tmp5DC;struct _tuple16 _tmp5DA=_stmttmp7E;_tmp5DB=_tmp5DA.f1;_tmp5DC=_tmp5DA.f2;{
# 2330
struct _tuple16 _tmp8B8;struct _tuple16 _stmttmp7F=(_tmp8B8.f1=_tmp5DB,((_tmp8B8.f2=_tmp5DC,_tmp8B8)));struct _tuple16 _tmp5DD=_stmttmp7F;struct Cyc_Dict_Dict _tmp5DE;struct Cyc_List_List*_tmp5DF;struct Cyc_CfFlowInfo_Place*_tmp5E0;_LL379: if(((_tmp5DD.f1).ReachableFL).tag != 2)goto _LL37B;_tmp5DE=((struct _tuple14)((_tmp5DD.f1).ReachableFL).val).f1;_tmp5DF=((struct _tuple14)((_tmp5DD.f1).ReachableFL).val).f2;if(((_tmp5DD.f2).PlaceL).tag != 1)goto _LL37B;_tmp5E0=(struct Cyc_CfFlowInfo_Place*)((_tmp5DD.f2).PlaceL).val;_LL37A: {
# 2332
void*_tmp5E1=Cyc_CfFlowInfo_lookup_place(_tmp5DE,_tmp5E0);
# 2334
void*t=(void*)_check_null(e->topt);
# 2341
void*_tmp5E2=Cyc_CfFlowInfo_make_unique_consumed(_tmp569,t,e,env->iteration_num,_tmp5E1);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8BB;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8BA;_tmp5E2=(void*)((_tmp8BA=_region_malloc(_tmp569->r,sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BB.tag=8,((_tmp8BB.f1=_tmp59A,((_tmp8BB.f2=_tmp5E2,_tmp8BB)))))),_tmp8BA))));}
_tmp5DE=Cyc_CfFlowInfo_assign_place(_tmp569,e->loc,_tmp5DE,env->all_changed,_tmp5E0,_tmp5E2);
_tmp5DB=Cyc_CfFlowInfo_ReachableFL(_tmp5DE,_tmp5DF);{
# 2348
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _stmttmp80=Cyc_CfFlowInfo_initlevel(_tmp569,_tmp5DE,_tmp5E1);switch(_stmttmp80){case Cyc_CfFlowInfo_AllIL: _LL37F:
 leaf=_tmp569->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL380:
 leaf=_tmp569->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL381:
 leaf=_tmp569->unknown_this;break;}}{
# 2356
void*_tmp5E5=Cyc_CfFlowInfo_typ_to_absrval(_tmp569,t,0,leaf);
_tmp5DB=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5DB,_tmp5E1);{
struct Cyc_List_List _tmp8BC;struct Cyc_List_List _tmp5E6=(_tmp8BC.hd=_tmp59A,((_tmp8BC.tl=0,_tmp8BC)));
_tmp5DB=Cyc_NewControlFlow_add_vars(_tmp569,_tmp5DB,& _tmp5E6,_tmp569->unknown_all,s->loc,0);
# 2363
_tmp5DB=Cyc_NewControlFlow_anal_stmt(env,_tmp5DB,_tmp59B);{
# 2368
union Cyc_CfFlowInfo_FlowInfo _tmp5E7=_tmp5DB;struct Cyc_Dict_Dict _tmp5E8;struct Cyc_List_List*_tmp5E9;_LL384: if((_tmp5E7.ReachableFL).tag != 2)goto _LL386;_tmp5E8=((struct _tuple14)(_tmp5E7.ReachableFL).val).f1;_tmp5E9=((struct _tuple14)(_tmp5E7.ReachableFL).val).f2;_LL385:
# 2371
{void*_stmttmp81=Cyc_CfFlowInfo_lookup_place(_tmp5E8,_tmp5E0);void*_tmp5EA=_stmttmp81;struct Cyc_Absyn_Vardecl*_tmp5EC;_LL389: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5EB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5EA;if(_tmp5EB->tag != 8)goto _LL38B;else{_tmp5EC=_tmp5EB->f1;}}_LL38A:
# 2373
 if(_tmp5EC != _tmp59A)goto _LL38C;
goto _LL388;_LL38B:;_LL38C:
# 2376
{const char*_tmp8BF;void*_tmp8BE;(_tmp8BE=0,Cyc_Tcutil_terr(s->loc,((_tmp8BF="aliased location was overwritten",_tag_dyneither(_tmp8BF,sizeof(char),33))),_tag_dyneither(_tmp8BE,sizeof(void*),0)));}
goto _LL388;_LL388:;}
# 2379
_tmp5E8=Cyc_CfFlowInfo_assign_place(_tmp569,s->loc,_tmp5E8,env->all_changed,_tmp5E0,_tmp5E5);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5E8,_tmp5E9);
# 2384
return outflow;_LL386:;_LL387:
 return _tmp5DB;_LL383:;};};};};}_LL37B: if(((_tmp5DD.f1).BottomFL).tag != 1)goto _LL37D;_LL37C:
# 2387
 return _tmp5DB;_LL37D:;_LL37E:
{const char*_tmp8C2;void*_tmp8C1;(_tmp8C1=0,Cyc_Tcutil_terr(e->loc,((_tmp8C2="bad alias expression--no unique path found",_tag_dyneither(_tmp8C2,sizeof(char),43))),_tag_dyneither(_tmp8C1,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL378:;};}_LL36B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp59C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp59C->tag != 12)goto _LL36D;else{_tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;}}_LL36C:
# 2393
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp59D),_tmp59E);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp59D);
return outflow;_LL36D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp59F=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp59F->tag != 13)goto _LL36F;else{_tmp5A0=_tmp59F->f2;}}_LL36E:
# 2398
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A0);_LL36F: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5A1=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp56A;if(_tmp5A1->tag != 16)goto _LL371;else{_tmp5A2=_tmp5A1->f1;}}_LL370: {
# 2401
struct _tuple15 _stmttmp7C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A2);union Cyc_CfFlowInfo_FlowInfo _tmp5F4;void*_tmp5F5;struct _tuple15 _tmp5F3=_stmttmp7C;_tmp5F4=_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5F6=Cyc_NewControlFlow_use_Rval(env,_tmp5A2->loc,_tmp5F4,_tmp5F5);
void*_stmttmp7D=Cyc_Tcutil_compress((void*)_check_null(_tmp5A2->topt));void*_tmp5F7=_stmttmp7D;void*_tmp5F9;_LL38E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 15)goto _LL390;else{_tmp5F9=(void*)_tmp5F8->f1;}}_LL38F:
# 2405
 return Cyc_CfFlowInfo_kill_flow_region(_tmp569,_tmp5F4,_tmp5F9);_LL390:;_LL391: {
struct Cyc_Core_Impossible_exn_struct _tmp8C8;const char*_tmp8C7;struct Cyc_Core_Impossible_exn_struct*_tmp8C6;(int)_throw((void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C8.tag=Cyc_Core_Impossible,((_tmp8C8.f1=((_tmp8C7="anal_stmt -- reset_region",_tag_dyneither(_tmp8C7,sizeof(char),26))),_tmp8C8)))),_tmp8C6)))));}_LL38D:;};}_LL371:;_LL372: {
# 2409
struct Cyc_Core_Impossible_exn_struct _tmp8CE;const char*_tmp8CD;struct Cyc_Core_Impossible_exn_struct*_tmp8CC;(int)_throw((void*)((_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CE.tag=Cyc_Core_Impossible,((_tmp8CE.f1=((_tmp8CD="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8CD,sizeof(char),56))),_tmp8CE)))),_tmp8CC)))));}_LL348:;};}
# 2414
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2416
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp600=env->fenv;
void*_stmttmp8D=decl->r;void*_tmp601=_stmttmp8D;struct Cyc_Absyn_Vardecl*_tmp603;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_List_List*_tmp608;struct Cyc_Absyn_Fndecl*_tmp60A;struct Cyc_Absyn_Tvar*_tmp60C;struct Cyc_Absyn_Vardecl*_tmp60D;int _tmp60E;struct Cyc_Absyn_Exp*_tmp60F;_LL393: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp602=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp601;if(_tmp602->tag != 0)goto _LL395;else{_tmp603=_tmp602->f1;}}_LL394: {
# 2426
struct Cyc_List_List _tmp8CF;struct Cyc_List_List _tmp610=(_tmp8CF.hd=_tmp603,((_tmp8CF.tl=0,_tmp8CF)));
inflow=Cyc_NewControlFlow_add_vars(_tmp600,inflow,& _tmp610,_tmp600->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp611=_tmp603->initializer;
if(_tmp611 == 0)
return inflow;{
struct _tuple15 _stmttmp8F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp611);union Cyc_CfFlowInfo_FlowInfo _tmp613;void*_tmp614;struct _tuple15 _tmp612=_stmttmp8F;_tmp613=_tmp612.f1;_tmp614=_tmp612.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp600,env,_tmp613,_tmp603,_tmp611,_tmp614,decl->loc);};};}_LL395: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp604=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp601;if(_tmp604->tag != 2)goto _LL397;else{if(_tmp604->f2 == 0)goto _LL397;_tmp605=(struct Cyc_List_List*)(_tmp604->f2)->v;_tmp606=_tmp604->f3;}}_LL396:
# 2440
 inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp600,env,inflow,_tmp605,decl->loc,_tmp606->loc);
# 2444
return inflow;_LL397: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp607=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp601;if(_tmp607->tag != 3)goto _LL399;else{_tmp608=_tmp607->f1;}}_LL398:
# 2447
 return Cyc_NewControlFlow_add_vars(_tmp600,inflow,_tmp608,_tmp600->unknown_none,decl->loc,0);_LL399: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp609=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp601;if(_tmp609->tag != 1)goto _LL39B;else{_tmp60A=_tmp609->f1;}}_LL39A:
# 2450
 Cyc_NewControlFlow_check_nested_fun(_tmp600,inflow,_tmp60A);{
void*t=(void*)_check_null(_tmp60A->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp616=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp60A->fn_vardecl);
# 2456
struct Cyc_List_List*_tmp8D0;return Cyc_NewControlFlow_add_vars(_tmp600,inflow,((_tmp8D0=_region_malloc(env->r,sizeof(*_tmp8D0)),((_tmp8D0->hd=_tmp616,((_tmp8D0->tl=0,_tmp8D0)))))),_tmp600->unknown_all,decl->loc,0);};_LL39B: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp60B=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp601;if(_tmp60B->tag != 4)goto _LL39D;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60B->f2;_tmp60E=_tmp60B->f3;_tmp60F=_tmp60B->f4;}}_LL39C:
# 2459
 if(_tmp60F != 0){
struct Cyc_Absyn_Exp*_tmp618=_tmp60F;
struct _tuple15 _stmttmp8E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp618);union Cyc_CfFlowInfo_FlowInfo _tmp61A;void*_tmp61B;struct _tuple15 _tmp619=_stmttmp8E;_tmp61A=_tmp619.f1;_tmp61B=_tmp619.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp618->loc,_tmp61A,_tmp61B);}{
# 2464
struct Cyc_List_List _tmp8D1;struct Cyc_List_List _tmp61C=(_tmp8D1.hd=_tmp60D,((_tmp8D1.tl=0,_tmp8D1)));
return Cyc_NewControlFlow_add_vars(_tmp600,inflow,& _tmp61C,_tmp600->unknown_all,decl->loc,0);};_LL39D:;_LL39E: {
# 2467
struct Cyc_Core_Impossible_exn_struct _tmp8D7;const char*_tmp8D6;struct Cyc_Core_Impossible_exn_struct*_tmp8D5;(int)_throw((void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D7.tag=Cyc_Core_Impossible,((_tmp8D7.f1=((_tmp8D6="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8D6,sizeof(char),35))),_tmp8D7)))),_tmp8D5)))));}_LL392:;}
# 2507 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp621=_new_region("frgn");struct _RegionHandle*frgn=& _tmp621;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2509
;_pop_region(frgn);}
# 2514
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2518
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2522
struct _RegionHandle*_tmp622=fenv->r;
unsigned int _tmp623=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp623,1);{
# 2528
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp624=inflow;struct Cyc_Dict_Dict _tmp625;struct Cyc_List_List*_tmp626;_LL3A0: if((_tmp624.BottomFL).tag != 1)goto _LL3A2;_LL3A1: {
const char*_tmp8DA;void*_tmp8D9;(_tmp8D9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8DA="check_fun",_tag_dyneither(_tmp8DA,sizeof(char),10))),_tag_dyneither(_tmp8D9,sizeof(void*),0)));}_LL3A2: if((_tmp624.ReachableFL).tag != 2)goto _LL39F;_tmp625=((struct _tuple14)(_tmp624.ReachableFL).val).f1;_tmp626=((struct _tuple14)(_tmp624.ReachableFL).val).f2;_LL3A3: {
# 2535
struct Cyc_List_List*atts;
{void*_stmttmp90=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp629=_stmttmp90;struct Cyc_List_List*_tmp62B;_LL3A5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp62A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp629;if(_tmp62A->tag != 9)goto _LL3A7;else{_tmp62B=(_tmp62A->f1).attributes;}}_LL3A6:
 atts=_tmp62B;goto _LL3A4;_LL3A7:;_LL3A8: {
const char*_tmp8DD;void*_tmp8DC;(_tmp8DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8DD="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8DD,sizeof(char),50))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}_LL3A4:;}
# 2540
for(0;atts != 0;atts=atts->tl){
void*_stmttmp91=(void*)atts->hd;void*_tmp62E=_stmttmp91;int _tmp630;int _tmp632;int _tmp634;_LL3AA: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp62F=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp62E;if(_tmp62F->tag != 21)goto _LL3AC;else{_tmp630=_tmp62F->f1;}}_LL3AB: {
# 2543
unsigned int j=(unsigned int)_tmp630;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8E0;void*_tmp8DF;(_tmp8DF=0,Cyc_Tcutil_terr(_tmp623,((_tmp8E0="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8E0,sizeof(char),52))),_tag_dyneither(_tmp8DF,sizeof(void*),0)));}
continue;}{
# 2549
struct Cyc_Absyn_Vardecl*_tmp637=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp637->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,Cyc_Tcutil_terr(_tmp623,((_tmp8E3="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8E3,sizeof(char),52))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}
continue;}{
# 2555
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp63A=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8E6;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8E5;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp63B=(_tmp8E5=_region_malloc(_tmp622,sizeof(*_tmp8E5)),((_tmp8E5[0]=((_tmp8E6.tag=2,((_tmp8E6.f1=(int)j,((_tmp8E6.f2=t,_tmp8E6)))))),_tmp8E5)));
struct Cyc_CfFlowInfo_Place*_tmp8E7;struct Cyc_CfFlowInfo_Place*_tmp63C=(_tmp8E7=_region_malloc(_tmp622,sizeof(*_tmp8E7)),((_tmp8E7->root=(void*)_tmp63B,((_tmp8E7->fields=0,_tmp8E7)))));
_tmp625=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp625,(void*)_tmp63B,_tmp63A);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8ED;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8EC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8EA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8E9;_tmp625=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp625,(void*)((_tmp8E9=_region_malloc(_tmp622,sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EA.tag=0,((_tmp8EA.f1=_tmp637,_tmp8EA)))),_tmp8E9)))),(void*)((_tmp8EC=_region_malloc(_tmp622,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=5,((_tmp8ED.f1=_tmp63C,_tmp8ED)))),_tmp8EC)))));}
goto _LL3A9;};};}_LL3AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp631=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp62E;if(_tmp631->tag != 20)goto _LL3AE;else{_tmp632=_tmp631->f1;}}_LL3AD: {
# 2565
unsigned int j=(unsigned int)_tmp632;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_terr(_tmp623,((_tmp8F0="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp8F0,sizeof(char),51))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
# 2569
continue;}{
# 2571
struct Cyc_Absyn_Vardecl*_tmp646=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_stmttmp92=Cyc_Tcutil_compress(_tmp646->type);void*_tmp647=_stmttmp92;void*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;_LL3B3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 5)goto _LL3B5;else{_tmp649=(_tmp648->f1).elt_typ;_tmp64A=((_tmp648->f1).ptr_atts).nullable;_tmp64B=((_tmp648->f1).ptr_atts).bounds;_tmp64C=((_tmp648->f1).ptr_atts).zero_term;}}_LL3B4:
# 2574
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp64A)){
const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_Tcutil_terr(_tmp623,((_tmp8F3="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp8F3,sizeof(char),55))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
# 2577
if(!Cyc_Tcutil_is_bound_one(_tmp64B)){
const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_terr(_tmp623,((_tmp8F6="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp8F6,sizeof(char),57))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}
# 2580
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp64C)){
const char*_tmp8F9;void*_tmp8F8;(_tmp8F8=0,Cyc_Tcutil_terr(_tmp623,((_tmp8F9="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp8F9,sizeof(char),77))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}{
# 2583
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8FC;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8FB;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp653=(_tmp8FB=_region_malloc(_tmp622,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=2,((_tmp8FC.f1=(int)j,((_tmp8FC.f2=_tmp649,_tmp8FC)))))),_tmp8FB)));
struct Cyc_CfFlowInfo_Place*_tmp8FD;struct Cyc_CfFlowInfo_Place*_tmp654=(_tmp8FD=_region_malloc(_tmp622,sizeof(*_tmp8FD)),((_tmp8FD->root=(void*)_tmp653,((_tmp8FD->fields=0,_tmp8FD)))));
_tmp625=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp625,(void*)_tmp653,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp649,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp903;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp902;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp900;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8FF;_tmp625=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp625,(void*)((_tmp8FF=_region_malloc(_tmp622,sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=0,((_tmp900.f1=_tmp646,_tmp900)))),_tmp8FF)))),(void*)((_tmp902=_region_malloc(_tmp622,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=5,((_tmp903.f1=_tmp654,_tmp903)))),_tmp902)))));}
{struct Cyc_List_List*_tmp904;param_roots=((_tmp904=_region_malloc(_tmp622,sizeof(*_tmp904)),((_tmp904->hd=_tmp654,((_tmp904->tl=param_roots,_tmp904))))));}
goto _LL3B2;};_LL3B5:;_LL3B6: {
const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_Tcutil_terr(_tmp623,((_tmp907="initializes attribute on non-pointer",_tag_dyneither(_tmp907,sizeof(char),37))),_tag_dyneither(_tmp906,sizeof(void*),0)));}_LL3B2:;}
# 2591
goto _LL3A9;};}_LL3AE: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp633=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp62E;if(_tmp633->tag != 22)goto _LL3B0;else{_tmp634=_tmp633->f1;}}_LL3AF: {
# 2593
unsigned int j=(unsigned int)_tmp634;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp90A;void*_tmp909;(_tmp909=0,Cyc_Tcutil_terr(_tmp623,((_tmp90A="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp90A,sizeof(char),49))),_tag_dyneither(_tmp909,sizeof(void*),0)));}
continue;}{
# 2598
struct Cyc_Absyn_Vardecl*_tmp661=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp661->type);
if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp90D;void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_terr(_tmp623,((_tmp90D="noconsume attribute requires no-alias pointer",_tag_dyneither(_tmp90D,sizeof(char),46))),_tag_dyneither(_tmp90C,sizeof(void*),0)));}
continue;}{
# 2605
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp910;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp90F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp664=(_tmp90F=_region_malloc(_tmp622,sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp910.tag=0,((_tmp910.f1=_tmp661,_tmp910)))),_tmp90F)));
struct Cyc_CfFlowInfo_Place*_tmp911;struct Cyc_CfFlowInfo_Place*_tmp665=(_tmp911=_region_malloc(_tmp622,sizeof(*_tmp911)),((_tmp911->root=(void*)_tmp664,((_tmp911->fields=0,_tmp911)))));
void*_tmp666=Cyc_CfFlowInfo_lookup_place(_tmp625,_tmp665);
{struct _tuple26*_tmp914;struct Cyc_List_List*_tmp913;noconsume_rvals=((_tmp913=_region_malloc(_tmp622,sizeof(*_tmp913)),((_tmp913->hd=((_tmp914=_region_malloc(_tmp622,sizeof(*_tmp914)),((_tmp914->f1=_tmp665,((_tmp914->f2=_tmp666,_tmp914)))))),((_tmp913->tl=noconsume_rvals,_tmp913))))));}
# 2610
goto _LL3A9;};};}_LL3B0:;_LL3B1:
 goto _LL3A9;_LL3A9:;}
# 2613
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp625,_tmp626);}_LL39F:;}{
# 2616
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp622,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp915;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp915=_region_malloc(_tmp622,sizeof(*_tmp915)),((_tmp915->r=_tmp622,((_tmp915->fenv=fenv,((_tmp915->iterate_again=1,((_tmp915->iteration_num=0,((_tmp915->in_try=0,((_tmp915->tryflow=inflow,((_tmp915->all_changed=0,((_tmp915->noreturn=noreturn,((_tmp915->param_roots=param_roots,((_tmp915->noconsume_params=noconsume_rvals,((_tmp915->flow_table=flow_table,_tmp915)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2626
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2629
union Cyc_CfFlowInfo_FlowInfo _tmp66C=outflow;_LL3B8: if((_tmp66C.BottomFL).tag != 1)goto _LL3BA;_LL3B9:
 goto _LL3B7;_LL3BA:;_LL3BB:
# 2632
 Cyc_NewControlFlow_check_init_params(_tmp623,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp623,env,outflow);
# 2636
if(noreturn){
const char*_tmp918;void*_tmp917;(_tmp917=0,Cyc_Tcutil_terr(_tmp623,((_tmp918="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp918,sizeof(char),46))),_tag_dyneither(_tmp917,sizeof(void*),0)));}else{
# 2639
void*_stmttmp93=Cyc_Tcutil_compress(fd->ret_type);void*_tmp66F=_stmttmp93;_LL3BD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp670=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp66F;if(_tmp670->tag != 0)goto _LL3BF;}_LL3BE:
 goto _LL3BC;_LL3BF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp671=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp66F;if(_tmp671->tag != 7)goto _LL3C1;}_LL3C0:
 goto _LL3C2;_LL3C1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp672=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66F;if(_tmp672->tag != 6)goto _LL3C3;}_LL3C2:
# 2643
{const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_warn(_tmp623,((_tmp91B="function may not return a value",_tag_dyneither(_tmp91B,sizeof(char),32))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}goto _LL3BC;_LL3C3:;_LL3C4:
# 2645
{const char*_tmp91E;void*_tmp91D;(_tmp91D=0,Cyc_Tcutil_terr(_tmp623,((_tmp91E="function may not return a value",_tag_dyneither(_tmp91E,sizeof(char),32))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}goto _LL3BC;_LL3BC:;}
# 2647
goto _LL3B7;_LL3B7:;};};};}
# 2652
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_stmttmp94=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp678=_stmttmp94;struct Cyc_Absyn_Fndecl*_tmp67A;struct Cyc_List_List*_tmp67C;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp680;_LL3C6: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp679=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp678;if(_tmp679->tag != 1)goto _LL3C8;else{_tmp67A=_tmp679->f1;}}_LL3C7:
 Cyc_NewControlFlow_check_fun(_tmp67A);goto _LL3C5;_LL3C8: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp67B=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp678;if(_tmp67B->tag != 12)goto _LL3CA;else{_tmp67C=_tmp67B->f1;}}_LL3C9:
 _tmp67E=_tmp67C;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp67D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp678;if(_tmp67D->tag != 11)goto _LL3CC;else{_tmp67E=_tmp67D->f2;}}_LL3CB:
 _tmp680=_tmp67E;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp67F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp678;if(_tmp67F->tag != 10)goto _LL3CE;else{_tmp680=_tmp67F->f2;}}_LL3CD:
 Cyc_NewControlFlow_cf_check(_tmp680);goto _LL3C5;_LL3CE: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp681=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp678;if(_tmp681->tag != 13)goto _LL3D0;}_LL3CF:
 goto _LL3C5;_LL3D0:;_LL3D1:
 goto _LL3C5;_LL3C5:;}}
