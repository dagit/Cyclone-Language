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
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 65
void*Cyc_Tcutil_compress(void*t);
# 99
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 172
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 181
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 245 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 277
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 287
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 340
int Cyc_Tcutil_is_noreturn(void*);
# 358
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 362
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 9 "stdio.h"
typedef long Cyc___off32_t;
# 13
typedef long Cyc_off_t;
# 17
typedef long Cyc__fpos_t;
# 21
typedef long Cyc_fpos_t;struct Cyc___cycFILE;
# 81 "stdio.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct  __abstractFILE;
# 108 "stdio.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 139
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 39
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;struct _tuple12{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 58
struct _tuple12 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
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
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
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
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,struct _tuple14 pr2);
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
# 64 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_vardecl_opt_t;
# 69
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 77
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 82
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp66C;struct Cyc_NewControlFlow_CFStmtAnnot _tmp66B;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp66A;enclosee->annot=(void*)((_tmp66A=_cycalloc(sizeof(*_tmp66A)),((_tmp66A[0]=((_tmp66C.tag=Cyc_NewControlFlow_CFAnnot,((_tmp66C.f1=((_tmp66B.encloser=encloser,((_tmp66B.visited=0,_tmp66B)))),_tmp66C)))),_tmp66A))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 108 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 116
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 125
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp672;const char*_tmp671;struct Cyc_Core_Impossible_exn_struct*_tmp670;(int)_throw((void*)((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670[0]=((_tmp672.tag=Cyc_Core_Impossible,((_tmp672.f1=((_tmp671="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp671,sizeof(char),37))),_tmp672)))),_tmp670)))));}_LL0:;}
# 132
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp673;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp673=_region_malloc(env->r,sizeof(*_tmp673)),((_tmp673[0]=Cyc_CfFlowInfo_BottomFL(),_tmp673)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 139
return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 142
static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 146
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 149
_tmpA->visited=env->iteration_num;{
struct _tuple17 _tmp674;return(_tmp674.f1=_tmpA,((_tmp674.f2=_tmpB,_tmp674)));};}
# 161 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 169
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 176
static void Cyc_NewControlFlow_check_unique_root(struct _tuple18*ckenv,void*root,void*rval){
# 178
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple18*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
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
{const char*_tmp678;void*_tmp677[1];struct Cyc_String_pa_PrintArg_struct _tmp676;(_tmp676.tag=0,((_tmp676.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp677[0]=& _tmp676,Cyc_Tcutil_warn(_tmpF,((_tmp678="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp678,sizeof(char),60))),_tag_dyneither(_tmp677,sizeof(void*),1)))))));}
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
struct _tuple16 _tmp679;struct _tuple16 _tmp21=(_tmp679.f1=flow,((_tmp679.f2=_tmp1F,_tmp679)));struct Cyc_Dict_Dict _tmp22;struct Cyc_Dict_Dict _tmp23;_LL16: if(((_tmp21.f1).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple13)((_tmp21.f1).ReachableFL).val).f1;if(((_tmp21.f2).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple13)((_tmp21.f2).ReachableFL).val).f1;_LL17: {
# 209
struct _tuple18 _tmp67A;struct _tuple18 _tmp24=(_tmp67A.f1=env,((_tmp67A.f2=s->loc,((_tmp67A.f3=_tmp23,_tmp67A)))));
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp24,_tmp22);
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
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple13)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple13)(_tmp26.ReachableFL).val).f2;_LL1E:
# 242
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp67D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp67C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp67C=_region_malloc(fenv->r,sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=0,((_tmp67D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp67D)))),_tmp67C)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp680;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp67F;_tmp2A=(void*)((_tmp67F=_region_malloc(fenv->r,sizeof(*_tmp67F)),((_tmp67F[0]=((_tmp680.tag=8,((_tmp680.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp680.f2=_tmp2A,_tmp680)))))),_tmp67F))));}
# 249
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 251
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 255
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 262
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2F=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp681;if(Cyc_Relations_consistent_relations(((_tmp681=_region_malloc(r,sizeof(*_tmp681)),((_tmp681->hd=_tmp2F,((_tmp681->tl=assume,_tmp681))))))))
return 0;}
# 267
return 1;}
# 270
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp31=e->r;struct Cyc_Absyn_Exp*_tmp33;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp32=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp31;if(_tmp32->tag != 13)goto _LL22;else{_tmp33=_tmp32->f2;}}_LL21:
 return _tmp33;_LL22:;_LL23:
 return e;_LL1F:;}
# 277
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 279
union Cyc_CfFlowInfo_FlowInfo _tmp34=inflow;struct Cyc_List_List*_tmp35;_LL25: if((_tmp34.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp34.ReachableFL).tag != 2)goto _LL24;_tmp35=((struct _tuple13)(_tmp34.ReachableFL).val).f2;_LL28:
# 282
{void*_tmp36=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp3B;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL2C;else{_tmp38=(_tmp37->f1).aggr_info;_tmp39=(_tmp37->f1).targs;}}_LL2B: {
# 284
struct Cyc_Absyn_Aggrdecl*_tmp3C=Cyc_Absyn_get_known_aggrdecl(_tmp38);
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_decl_field(_tmp3C,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp3C->tvs,_tmp39),_tmp3E);
# 291
if(!Cyc_NewControlFlow_relns_ok(r,_tmp35,Cyc_Relations_exp2relns(temp,_tmp40))){
{const char*_tmp686;void*_tmp685[2];struct Cyc_String_pa_PrintArg_struct _tmp684;struct Cyc_String_pa_PrintArg_struct _tmp683;(_tmp683.tag=0,((_tmp683.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp684.tag=0,((_tmp684.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40))),((_tmp685[0]=& _tmp684,((_tmp685[1]=& _tmp683,Cyc_Tcutil_terr(loc,((_tmp686="unable to prove %s, required to access %s",_tag_dyneither(_tmp686,sizeof(char),42))),_tag_dyneither(_tmp685,sizeof(void*),2)))))))))))));}
{const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="  [recorded facts on path: ",_tag_dyneither(_tmp689,sizeof(char),28))),_tag_dyneither(_tmp688,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="]\n",_tag_dyneither(_tmp68C,sizeof(char),3))),_tag_dyneither(_tmp68B,sizeof(void*),0)));};}}
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
{const char*_tmp691;void*_tmp690[2];struct Cyc_String_pa_PrintArg_struct _tmp68F;struct Cyc_String_pa_PrintArg_struct _tmp68E;(_tmp68E.tag=0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp68F.tag=0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4A))),((_tmp690[0]=& _tmp68F,((_tmp690[1]=& _tmp68E,Cyc_Tcutil_terr(loc,((_tmp691="unable to prove %s, required to access %s",_tag_dyneither(_tmp691,sizeof(char),42))),_tag_dyneither(_tmp690,sizeof(void*),2)))))))))))));}
{const char*_tmp694;void*_tmp693;(_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="  [recorded facts on path: ",_tag_dyneither(_tmp694,sizeof(char),28))),_tag_dyneither(_tmp693,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35);{
const char*_tmp697;void*_tmp696;(_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="]\n",_tag_dyneither(_tmp697,sizeof(char),3))),_tag_dyneither(_tmp696,sizeof(void*),0)));};}
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
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp54.ReachableFL).tag != 2)goto _LL30;_tmp55=((struct _tuple13)(_tmp54.ReachableFL).val).f1;_tmp56=((struct _tuple13)(_tmp54.ReachableFL).val).f2;_LL34:
# 322
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp55,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp69A;void*_tmp699;(_tmp699=0,Cyc_Tcutil_terr(loc,((_tmp69A="expression may not be fully initialized",_tag_dyneither(_tmp69A,sizeof(char),40))),_tag_dyneither(_tmp699,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp59=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp55,env->all_changed,r);
if(_tmp55.t == _tmp59.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A=Cyc_CfFlowInfo_ReachableFL(_tmp59,_tmp56);
Cyc_NewControlFlow_update_tryflow(env,_tmp5A);
return _tmp5A;};};_LL30:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 332
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp69B;struct _tuple0 _tmp5C=(_tmp69B.f1=Cyc_Tcutil_compress(t),((_tmp69B.f2=r,_tmp69B)));void*_tmp61;struct Cyc_Absyn_Datatypefield*_tmp63;struct _dyneither_ptr _tmp65;struct Cyc_List_List*_tmp67;struct _dyneither_ptr _tmp69;union Cyc_Absyn_AggrInfoU _tmp6B;struct Cyc_List_List*_tmp6C;struct _dyneither_ptr _tmp6E;enum Cyc_Absyn_AggrKind _tmp70;struct Cyc_List_List*_tmp71;struct _dyneither_ptr _tmp73;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp5D->tag != 3)goto _LL38;else{if(_tmp5D->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp5E->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp5F->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp60=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp60->tag != 8)goto _LL3E;else{_tmp61=(void*)_tmp60->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp61);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5C.f1;if(_tmp62->tag != 4)goto _LL40;else{if((((_tmp62->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp63=((struct _tuple2)(((_tmp62->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp64->tag != 6)goto _LL40;else{_tmp65=_tmp64->f2;}};_LL3F:
# 339
 if(_tmp63->typs == 0)
return;
_tmp67=_tmp63->typs;_tmp69=_tmp65;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C.f1;if(_tmp66->tag != 10)goto _LL42;else{_tmp67=_tmp66->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp68=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp68->tag != 6)goto _LL42;else{_tmp69=_tmp68->f2;}};_LL41: {
# 343
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp67))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp69,sizeof(void*),i)));}}
# 347
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5C.f1;if(_tmp6A->tag != 11)goto _LL44;else{_tmp6B=(_tmp6A->f1).aggr_info;_tmp6C=(_tmp6A->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp6D->tag != 6)goto _LL44;else{_tmp6E=_tmp6D->f2;}};_LL43: {
# 350
struct Cyc_Absyn_Aggrdecl*_tmp75=Cyc_Absyn_get_known_aggrdecl(_tmp6B);
if(_tmp75->impl == 0)return;{
struct Cyc_List_List*_tmp76=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp75->impl))->fields;
struct _RegionHandle _tmp77=_new_region("temp");struct _RegionHandle*temp=& _tmp77;_push_region(temp);
{struct Cyc_List_List*_tmp78=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp75->tvs,_tmp6C);
{int i=0;for(0;i < _get_dyneither_size(_tmp6E,sizeof(void*));(i ++,_tmp76=_tmp76->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp76))->hd)->type;
if(_tmp78 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp78,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp6E.curr)[i]);}}
# 360
_npop_handler(0);return;}
# 354
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C.f1;if(_tmp6F->tag != 12)goto _LL46;else{_tmp70=_tmp6F->f1;_tmp71=_tmp6F->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp72=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5C.f2;if(_tmp72->tag != 6)goto _LL46;else{_tmp73=_tmp72->f2;}};_LL45:
# 363
{int i=0;for(0;i < _get_dyneither_size(_tmp73,sizeof(void*));(i ++,_tmp71=_tmp71->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp71))->hd)->type,((void**)_tmp73.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C.f1;if(_tmp74->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t))goto _LL48;_LL47:
{const char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_Tcutil_warn(loc,((_tmp69E="argument may still contain unique pointers",_tag_dyneither(_tmp69E,sizeof(char),43))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 373
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp7B=inflow;struct Cyc_Dict_Dict _tmp7C;struct Cyc_List_List*_tmp7D;_LL4B: if((_tmp7B.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp7B.ReachableFL).tag != 2)goto _LL4A;_tmp7C=((struct _tuple13)(_tmp7B.ReachableFL).val).f1;_tmp7D=((struct _tuple13)(_tmp7B.ReachableFL).val).f2;_LL4E:
# 377
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_Tcutil_terr(loc,((_tmp6A1="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6A1,sizeof(char),47))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}
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
const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_Tcutil_warn(loc,((_tmp6A4="argument may contain live unique pointers",_tag_dyneither(_tmp6A4,sizeof(char),42))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 392
struct Cyc_Dict_Dict _tmp88=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp7C,env->all_changed,r);
if(_tmp7C.t == _tmp88.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp89=Cyc_CfFlowInfo_ReachableFL(_tmp88,_tmp7D);
Cyc_NewControlFlow_update_tryflow(env,_tmp89);
return _tmp89;};};};_LL4A:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 401
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 408
if(es == 0){
struct _tuple20 _tmp6A5;return(_tmp6A5.f1=inflow,((_tmp6A5.f2=0,_tmp6A5)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp8C;void*_tmp8D;struct _tuple14 _tmp8B=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{
struct Cyc_List_List*_tmp6A8;struct _tuple20 _tmp6A7;return(_tmp6A7.f1=_tmp8C,((_tmp6A7.f2=((_tmp6A8=_region_malloc(rgn,sizeof(*_tmp6A8)),((_tmp6A8->hd=_tmp8D,((_tmp6A8->tl=0,_tmp6A8)))))),_tmp6A7)));};}{
# 414
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 422
{struct Cyc_Dict_Dict*_tmp6A9;env->all_changed=((_tmp6A9=_region_malloc(env->r,sizeof(*_tmp6A9)),((_tmp6A9[0]=(env->fenv)->mt_place_set,_tmp6A9))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple14 _tmp91=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;
outflow=_tmp92;
{struct Cyc_List_List*_tmp6AA;rvals=((_tmp6AA=_region_malloc(rgn,sizeof(*_tmp6AA)),((_tmp6AA->hd=_tmp93,((_tmp6AA->tl=0,_tmp6AA))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 428
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6AB;env->all_changed=((_tmp6AB=_region_malloc(env->r,sizeof(*_tmp6AB)),((_tmp6AB[0]=(env->fenv)->mt_place_set,_tmp6AB))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp97;void*_tmp98;struct _tuple14 _tmp96=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp97=_tmp96.f1;_tmp98=_tmp96.f2;
{struct Cyc_List_List*_tmp6AC;rvals=((_tmp6AC=_region_malloc(rgn,sizeof(*_tmp6AC)),((_tmp6AC->hd=_tmp98,((_tmp6AC->tl=rvals,_tmp6AC))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp97,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 434
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 437
old_inflow=inflow;
# 440
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 445
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 449
struct Cyc_Dict_Dict*_tmp6AD;env->all_changed=((_tmp6AD=_region_malloc(env->r,sizeof(*_tmp6AD)),((_tmp6AD[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6AD))));}
# 451
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple20 _tmp6AE;return(_tmp6AE.f1=outflow,((_tmp6AE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6AE)));};};}
# 457
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 460
struct _RegionHandle _tmp9C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C;_push_region(rgn);
{union Cyc_CfFlowInfo_FlowInfo _tmp9E;struct Cyc_List_List*_tmp9F;struct _tuple20 _tmp9D=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmpA0=_tmp9E;struct Cyc_Dict_Dict _tmpA1;_LL57: if((_tmpA0.ReachableFL).tag != 2)goto _LL59;_tmpA1=((struct _tuple13)(_tmpA0.ReachableFL).val).f1;_LL58:
# 465
 for(0;_tmp9F != 0;(_tmp9F=_tmp9F->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpA1,(void*)_tmp9F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6B1="expression may not be initialized",_tag_dyneither(_tmp6B1,sizeof(char),34))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA0.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 471
struct _tuple14 _tmp6B2;struct _tuple14 _tmpA5=(_tmp6B2.f1=_tmp9E,((_tmp6B2.f2=(env->fenv)->unknown_all,_tmp6B2)));_npop_handler(0);return _tmpA5;};}
# 461
;_pop_region(rgn);}
# 485 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 491
union Cyc_CfFlowInfo_FlowInfo _tmpA6=outflow;struct Cyc_Dict_Dict _tmpA7;struct Cyc_List_List*_tmpA8;_LL5C: if((_tmpA6.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpA6.ReachableFL).tag != 2)goto _LL5B;_tmpA7=((struct _tuple13)(_tmpA6.ReachableFL).val).f1;_tmpA8=((struct _tuple13)(_tmpA6.ReachableFL).val).f2;_LL5F: {
# 494
union Cyc_CfFlowInfo_AbsLVal _tmpA9=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmpAA;_LL61: if((_tmpA9.UnknownL).tag != 2)goto _LL63;_LL62:
# 498
 return outflow;_LL63: if((_tmpA9.PlaceL).tag != 1)goto _LL60;_tmpAA=(struct Cyc_CfFlowInfo_Place*)(_tmpA9.PlaceL).val;_LL64: {
# 502
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6B5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6B4;nzval=(void*)((_tmp6B4=_region_malloc((env->fenv)->r,sizeof(*_tmp6B4)),((_tmp6B4[0]=((_tmp6B5.tag=8,((_tmp6B5.f1=locname,((_tmp6B5.f2=nzval,_tmp6B5)))))),_tmp6B4))));}{
union Cyc_CfFlowInfo_FlowInfo _tmpAD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA7,env->all_changed,_tmpAA,nzval),_tmpA8);
# 509
return _tmpAD;};}_LL60:;}_LL5B:;}
# 518
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 521
union Cyc_CfFlowInfo_FlowInfo _tmpAE=outflow;struct Cyc_Dict_Dict _tmpAF;struct Cyc_List_List*_tmpB0;_LL66: if((_tmpAE.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple16 _tmp6B6;return(_tmp6B6.f1=outflow,((_tmp6B6.f2=outflow,_tmp6B6)));}_LL68: if((_tmpAE.ReachableFL).tag != 2)goto _LL65;_tmpAF=((struct _tuple13)(_tmpAE.ReachableFL).val).f1;_tmpB0=((struct _tuple13)(_tmpAE.ReachableFL).val).f2;_LL69: {
# 524
union Cyc_CfFlowInfo_AbsLVal _tmpB2=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmpB3;_LL6B: if((_tmpB2.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple16 _tmp6B7;return(_tmp6B7.f1=outflow,((_tmp6B7.f2=outflow,_tmp6B7)));}_LL6D: if((_tmpB2.PlaceL).tag != 1)goto _LL6A;_tmpB3=(struct Cyc_CfFlowInfo_Place*)(_tmpB2.PlaceL).val;_LL6E: {
# 527
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6BA;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6B9;nzval=(void*)((_tmp6B9=_region_malloc((env->fenv)->r,sizeof(*_tmp6B9)),((_tmp6B9[0]=((_tmp6BA.tag=8,((_tmp6BA.f1=locname,((_tmp6BA.f2=nzval,_tmp6BA)))))),_tmp6B9))));}{
struct _tuple16 _tmp6BB;return
(_tmp6BB.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,nzval),_tmpB0),((_tmp6BB.f2=
# 533
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAF,env->all_changed,_tmpB3,(env->fenv)->zero),_tmpB0),_tmp6BB)));};}_LL6A:;}_LL65:;}
# 539
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 545
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 549
void*_tmpBA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpBC;union Cyc_Absyn_Constraint*_tmpBD;union Cyc_Absyn_Constraint*_tmpBE;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBA;if(_tmpBB->tag != 5)goto _LL72;else{_tmpBC=(_tmpBB->f1).elt_typ;_tmpBD=((_tmpBB->f1).ptr_atts).bounds;_tmpBE=((_tmpBB->f1).ptr_atts).zero_term;}}_LL71: {
# 551
union Cyc_CfFlowInfo_FlowInfo _tmpBF=f;struct Cyc_Dict_Dict _tmpC0;struct Cyc_List_List*_tmpC1;_LL75: if((_tmpBF.BottomFL).tag != 1)goto _LL77;_LL76: {
# 553
struct _tuple14 _tmp6BC;return(_tmp6BC.f1=f,((_tmp6BC.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6BC)));}_LL77: if((_tmpBF.ReachableFL).tag != 2)goto _LL74;_tmpC0=((struct _tuple13)(_tmpBF.ReachableFL).val).f1;_tmpC1=((struct _tuple13)(_tmpBF.ReachableFL).val).f2;_LL78: {
# 556
struct Cyc_Absyn_Vardecl*locname=0;
{void*_tmpC3=r;struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC3;if(_tmpC4->tag != 8)goto _LL7C;else{_tmpC5=_tmpC4->f1;_tmpC6=(void*)_tmpC4->f2;}}_LL7B:
 locname=_tmpC5;goto _LL79;_LL7C:;_LL7D:
 goto _LL79;_LL79:;}
# 561
if(Cyc_Tcutil_is_bound_one(_tmpBD)){
retry: {void*_tmpC7=r;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_CfFlowInfo_Place*_tmpCE;enum Cyc_CfFlowInfo_InitLevel _tmpD1;_LL7F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpC8->tag != 8)goto _LL81;else{_tmpC9=_tmpC8->f1;_tmpCA=(void*)_tmpC8->f2;}}_LL80:
 r=_tmpCA;goto retry;_LL81: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpCB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCB->tag != 1)goto _LL83;}_LL82:
 goto _LL84;_LL83: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCC->tag != 2)goto _LL85;}_LL84:
# 566
{void*_tmpD2=e->annot;struct Cyc_List_List*_tmpD4;_LL8E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD3=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD2;if(_tmpD3->tag != Cyc_CfFlowInfo_NotZero)goto _LL90;else{_tmpD4=_tmpD3->f1;}}_LL8F:
# 568
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpD4))goto _LL91;
goto _LL8D;_LL90:;_LL91:
# 572
{void*_tmpD5=e->r;_LL93: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD5;if(_tmpD6->tag != 22)goto _LL95;}_LL94:
# 574
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6BF;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6BE;e->annot=(void*)((_tmp6BE=_cycalloc(sizeof(*_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=Cyc_CfFlowInfo_NotZero,((_tmp6BF.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6BF)))),_tmp6BE))));}
goto _LL92;_LL95:;_LL96:
# 577
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL92;_LL92:;}
# 580
goto _LL8D;_LL8D:;}
# 582
goto _LL7E;_LL85: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCD->tag != 5)goto _LL87;else{_tmpCE=_tmpCD->f1;}}_LL86:
# 584
{void*_tmpD9=e->annot;struct Cyc_List_List*_tmpDB;_LL98: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpDA=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD9;if(_tmpDA->tag != Cyc_CfFlowInfo_NotZero)goto _LL9A;else{_tmpDB=_tmpDA->f1;}}_LL99:
# 586
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpDB))goto _LL9B;
goto _LL97;_LL9A:;_LL9B:
# 590
{void*_tmpDC=e->r;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpDD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC;if(_tmpDD->tag != 22)goto _LL9F;}_LL9E:
# 592
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6C2;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6C1;e->annot=(void*)((_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1[0]=((_tmp6C2.tag=Cyc_CfFlowInfo_NotZero,((_tmp6C2.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6C2)))),_tmp6C1))));}
goto _LL9C;_LL9F:;_LLA0:
# 595
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL9C;_LL9C:;}
# 598
goto _LL97;_LL97:;}{
# 600
struct _tuple14 _tmp6C3;return(_tmp6C3.f1=f,((_tmp6C3.f2=Cyc_CfFlowInfo_lookup_place(_tmpC0,_tmpCE),_tmp6C3)));};_LL87: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpCF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpCF->tag != 0)goto _LL89;}_LL88:
# 602
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp6C4;return(_tmp6C4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6C4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,(env->fenv)->unknown_all),_tmp6C4)));};_LL89: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpD0=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC7;if(_tmpD0->tag != 3)goto _LL8B;else{_tmpD1=_tmpD0->f1;}}_LL8A:
# 605
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpD1,locname);goto _LL8C;_LL8B:;_LL8C:
# 607
{void*_tmpE2=e->r;_LLA2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE2;if(_tmpE3->tag != 22)goto _LLA4;}_LLA3:
# 609
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6C7;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6C6;e->annot=(void*)((_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6C7.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6C7)))),_tmp6C6))));}
goto _LLA1;_LLA4:;_LLA5:
# 612
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLA1;_LLA1:;}
# 615
goto _LL7E;_LL7E:;}}else{
# 618
void*_tmpE6=e->annot;struct Cyc_List_List*_tmpE8;_LLA7: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE6;if(_tmpE7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA9;else{_tmpE8=_tmpE7->f1;}}_LLA8:
# 620
 if(!Cyc_Relations_same_relns(_tmpC1,_tmpE8))goto _LLAA;
goto _LLA6;_LLA9:;_LLAA:
# 623
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6CA;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6C9;e->annot=(void*)((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CA.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6CA.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1),_tmp6CA)))),_tmp6C9))));}
goto _LLA6;_LLA6:;}
# 627
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC0,r)){case Cyc_CfFlowInfo_NoneIL: _LLAB:
# 629
{void*_tmpEB=r;_LLAE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpEC=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpEB;if(_tmpEC->tag != 7)goto _LLB0;}_LLAF:
# 632
{const char*_tmp6CD;void*_tmp6CC;(_tmp6CC=0,Cyc_Tcutil_terr(e->loc,((_tmp6CD="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6CD,sizeof(char),61))),_tag_dyneither(_tmp6CC,sizeof(void*),0)));}
goto _LLAD;_LLB0:;_LLB1: {
# 635
const char*_tmp6D0;void*_tmp6CF;(_tmp6CF=0,Cyc_Tcutil_terr(e->loc,((_tmp6D0="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6D0,sizeof(char),46))),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}_LLAD:;}
# 637
goto _LLAC;case Cyc_CfFlowInfo_AllIL: _LLAC: {
# 639
void*_tmpF1=(env->fenv)->unknown_all;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D2;_tmpF1=(void*)((_tmp6D2=_region_malloc((env->fenv)->r,sizeof(*_tmp6D2)),((_tmp6D2[0]=((_tmp6D3.tag=8,((_tmp6D3.f1=locname,((_tmp6D3.f2=_tmpF1,_tmp6D3)))))),_tmp6D2))));}{
struct _tuple14 _tmp6D4;return(_tmp6D4.f1=f,((_tmp6D4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,_tmpF1),_tmp6D4)));};}case Cyc_CfFlowInfo_ThisIL: _LLB2: {
# 644
void*_tmpF5=(env->fenv)->unknown_none;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D6;_tmpF5=(void*)((_tmp6D6=_region_malloc((env->fenv)->r,sizeof(*_tmp6D6)),((_tmp6D6[0]=((_tmp6D7.tag=8,((_tmp6D7.f1=locname,((_tmp6D7.f2=_tmpF5,_tmp6D7)))))),_tmp6D6))));}{
struct _tuple14 _tmp6D8;return(_tmp6D8.f1=f,((_tmp6D8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBC,0,_tmpF5),_tmp6D8)));};}}}_LL74:;}_LL72:;_LL73: {
# 650
struct Cyc_Core_Impossible_exn_struct _tmp6DE;const char*_tmp6DD;struct Cyc_Core_Impossible_exn_struct*_tmp6DC;(int)_throw((void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DE.tag=Cyc_Core_Impossible,((_tmp6DE.f1=((_tmp6DD="right deref of non-pointer-type",_tag_dyneither(_tmp6DD,sizeof(char),32))),_tmp6DE)))),_tmp6DC)))));}_LL6F:;}
# 657
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 660
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 663
{void*_tmpFC=e1->r;void*_tmpFE;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFC;if(_tmpFD->tag != 1)goto _LLB7;else{_tmpFE=(void*)_tmpFD->f2;}}_LLB6: {
# 665
struct Cyc_Absyn_Vardecl*_tmpFF=Cyc_Tcutil_nonesc_vardecl(_tmpFE);
if(_tmpFF != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpFF);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 671
goto _LLB4;}_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}{
# 675
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 681
return relns;};}
# 686
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 694
union Cyc_CfFlowInfo_FlowInfo _tmp100=outflow;struct Cyc_Dict_Dict _tmp101;struct Cyc_List_List*_tmp102;_LLBA: if((_tmp100.BottomFL).tag != 1)goto _LLBC;_LLBB: {
# 697
struct _tuple14 _tmp6DF;return(_tmp6DF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6DF.f2=rval,_tmp6DF)));}_LLBC: if((_tmp100.ReachableFL).tag != 2)goto _LLB9;_tmp101=((struct _tuple13)(_tmp100.ReachableFL).val).f1;_tmp102=((struct _tuple13)(_tmp100.ReachableFL).val).f2;_LLBD: {
# 699
union Cyc_CfFlowInfo_AbsLVal _tmp104=lval;struct Cyc_CfFlowInfo_Place*_tmp105;_LLBF: if((_tmp104.PlaceL).tag != 1)goto _LLC1;_tmp105=(struct Cyc_CfFlowInfo_Place*)(_tmp104.PlaceL).val;_LLC0: {
# 701
struct Cyc_Dict_Dict _tmp106=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp101,env->all_changed,_tmp105,rval);
_tmp102=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp106,_tmp102);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 707
void*_tmp107=Cyc_CfFlowInfo_lookup_place(_tmp101,_tmp105);_LLC4: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp108=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp108->tag != 3)goto _LLC6;else{if(_tmp108->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC6;}}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp109=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp109->tag != 7)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp10A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10A->tag != 0)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10B->tag != 8)goto _LLCC;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10B->f2);if(_tmp10C->tag != 3)goto _LLCC;else{if(_tmp10C->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCC;}}}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10D->tag != 8)goto _LLCE;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 7)goto _LLCE;}}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp10F->tag != 8)goto _LLD0;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp110=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 0)goto _LLD0;}}_LLCF:
 goto _LLC3;_LLD0:;_LLD1:
# 715
{const char*_tmp6E2;void*_tmp6E1;(_tmp6E1=0,Cyc_Tcutil_warn(lexp->loc,((_tmp6E2="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp6E2,sizeof(char),43))),_tag_dyneither(_tmp6E1,sizeof(void*),0)));}
goto _LLC3;_LLC3:;}
# 720
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 722
struct _tuple14 _tmp6E3;return(_tmp6E3.f1=outflow,((_tmp6E3.f2=rval,_tmp6E3)));};}_LLC1: if((_tmp104.UnknownL).tag != 2)goto _LLBE;_LLC2: {
# 725
struct _tuple14 _tmp6E4;return(_tmp6E4.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp6E4.f2=rval,_tmp6E4)));}_LLBE:;}_LLB9:;}
# 730
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
void*_tmp115=e->r;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;_LLD3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp116->tag != 1)goto _LLD5;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp117=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp116->f2);if(_tmp117->tag != 4)goto _LLD5;}}_LLD4:
 goto _LLD6;_LLD5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp118=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp118->tag != 1)goto _LLD7;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp119=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp118->f2);if(_tmp119->tag != 3)goto _LLD7;}}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp11A->tag != 1)goto _LLD9;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp11B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp11A->f2);if(_tmp11B->tag != 5)goto _LLD9;}}_LLD8:
 return 1;_LLD9: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp11C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp11C->tag != 19)goto _LLDB;else{_tmp11D=_tmp11C->f1;}}_LLDA:
 _tmp11F=_tmp11D;goto _LLDC;_LLDB: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp11E->tag != 20)goto _LLDD;else{_tmp11F=_tmp11E->f1;}}_LLDC:
 _tmp121=_tmp11F;goto _LLDE;_LLDD: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp120=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp120->tag != 21)goto _LLDF;else{_tmp121=_tmp120->f1;}}_LLDE:
# 738
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp121);_LLDF: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp122->tag != 22)goto _LLE1;else{_tmp123=_tmp122->f1;}}_LLE0: {
# 740
void*_tmp126=Cyc_Tcutil_compress((void*)_check_null(_tmp123->topt));_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp127=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp126;if(_tmp127->tag != 10)goto _LLE8;}_LLE7:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp123);_LLE8:;_LLE9:
 return 0;_LLE5:;}_LLE1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp115;if(_tmp124->tag != 13)goto _LLE3;else{_tmp125=_tmp124->f2;}}_LLE2:
# 744
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp125);_LLE3:;_LLE4:
 return 0;_LLD2:;}
# 749
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 751
struct Cyc_CfFlowInfo_FlowEnv*_tmp128=env->fenv;
# 755
while(1){
union Cyc_CfFlowInfo_FlowInfo*_tmp12A;struct _tuple17 _tmp129=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp12A=_tmp129.f2;
inflow=*_tmp12A;{
void*_tmp12B=s->r;struct Cyc_Absyn_Stmt*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Absyn_Tvar*_tmp131;struct Cyc_Absyn_Vardecl*_tmp132;struct Cyc_Absyn_Stmt*_tmp133;struct Cyc_Absyn_Decl*_tmp135;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Absyn_Exp*_tmp138;_LLEB: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp12C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp12B;if(_tmp12C->tag != 2)goto _LLED;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LLEC:
# 760
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp12D);
s=_tmp12E;
goto _LLEA;_LLED: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp12B;if(_tmp12F->tag != 12)goto _LLEF;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp130=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp12F->f1)->r;if(_tmp130->tag != 5)goto _LLEF;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_tmp133=_tmp12F->f2;}}_LLEE: {
# 766
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp132->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp13A;union Cyc_CfFlowInfo_AbsLVal _tmp13B;struct _tuple15 _tmp139=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp13A=_tmp139.f1;_tmp13B=_tmp139.f2;{
struct _tuple15 _tmp6E5;struct _tuple15 _tmp13D=(_tmp6E5.f1=_tmp13A,((_tmp6E5.f2=_tmp13B,_tmp6E5)));struct Cyc_Dict_Dict _tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_CfFlowInfo_Place*_tmp140;_LLF6: if(((_tmp13D.f1).ReachableFL).tag != 2)goto _LLF8;_tmp13E=((struct _tuple13)((_tmp13D.f1).ReachableFL).val).f1;_tmp13F=((struct _tuple13)((_tmp13D.f1).ReachableFL).val).f2;if(((_tmp13D.f2).PlaceL).tag != 1)goto _LLF8;_tmp140=(struct Cyc_CfFlowInfo_Place*)((_tmp13D.f2).PlaceL).val;_LLF7: {
# 770
void*_tmp141=Cyc_CfFlowInfo_lookup_place(_tmp13E,_tmp140);
# 772
void*t=(void*)_check_null(e->topt);
# 779
void*_tmp142=Cyc_CfFlowInfo_make_unique_consumed(_tmp128,t,e,env->iteration_num,_tmp141);
_tmp13E=Cyc_CfFlowInfo_assign_place(_tmp128,e->loc,_tmp13E,env->all_changed,_tmp140,_tmp142);
_tmp13A=Cyc_CfFlowInfo_ReachableFL(_tmp13E,_tmp13F);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp128,_tmp13E,_tmp141)){case Cyc_CfFlowInfo_AllIL: _LLFC:
 leaf=_tmp128->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LLFD:
 leaf=_tmp128->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLFE:
 leaf=_tmp128->unknown_this;break;}{
# 790
void*_tmp143=Cyc_CfFlowInfo_typ_to_absrval(_tmp128,t,0,leaf);
_tmp13A=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp13A,_tmp141);{
struct Cyc_List_List _tmp6E6;struct Cyc_List_List _tmp144=(_tmp6E6.hd=_tmp132,((_tmp6E6.tl=0,_tmp6E6)));
_tmp13A=Cyc_NewControlFlow_add_vars(_tmp128,_tmp13A,& _tmp144,_tmp128->unknown_all,s->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp146;void*_tmp147;struct _tuple14 _tmp145=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp13A,_tmp133);_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp148=_tmp146;struct Cyc_Dict_Dict _tmp149;struct Cyc_List_List*_tmp14A;_LL101: if((_tmp148.ReachableFL).tag != 2)goto _LL103;_tmp149=((struct _tuple13)(_tmp148.ReachableFL).val).f1;_tmp14A=((struct _tuple13)(_tmp148.ReachableFL).val).f2;_LL102:
# 797
 _tmp149=Cyc_CfFlowInfo_assign_place(_tmp128,s->loc,_tmp149,env->all_changed,_tmp140,_tmp143);{
union Cyc_CfFlowInfo_FlowInfo _tmp14B=Cyc_CfFlowInfo_ReachableFL(_tmp149,_tmp14A);
struct _tuple14 _tmp6E7;return(_tmp6E7.f1=_tmp14B,((_tmp6E7.f2=_tmp147,_tmp6E7)));};_LL103:;_LL104: {
struct _tuple14 _tmp6E8;return(_tmp6E8.f1=_tmp146,((_tmp6E8.f2=_tmp147,_tmp6E8)));}_LL100:;};};};};};}_LLF8: if(((_tmp13D.f1).BottomFL).tag != 1)goto _LLFA;_LLF9: {
# 802
struct _tuple14 _tmp6E9;return(_tmp6E9.f1=_tmp13A,((_tmp6E9.f2=_tmp128->unknown_all,_tmp6E9)));}_LLFA:;_LLFB:
{const char*_tmp6EC;void*_tmp6EB;(_tmp6EB=0,Cyc_Tcutil_terr(e->loc,((_tmp6EC="bad alias expression--no unique path found",_tag_dyneither(_tmp6EC,sizeof(char),43))),_tag_dyneither(_tmp6EB,sizeof(void*),0)));}{
struct _tuple14 _tmp6ED;return(_tmp6ED.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6ED.f2=_tmp128->unknown_all,_tmp6ED)));};_LLF5:;};}_LLEF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp134=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp12B;if(_tmp134->tag != 12)goto _LLF1;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LLF0:
# 807
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp135);
s=_tmp136;
goto _LLEA;_LLF1: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp137=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp12B;if(_tmp137->tag != 1)goto _LLF3;else{_tmp138=_tmp137->f1;}}_LLF2:
# 811
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp138);_LLF3:;_LLF4: {
struct Cyc_Core_Impossible_exn_struct _tmp6F3;const char*_tmp6F2;struct Cyc_Core_Impossible_exn_struct*_tmp6F1;(int)_throw((void*)((_tmp6F1=_cycalloc(sizeof(*_tmp6F1)),((_tmp6F1[0]=((_tmp6F3.tag=Cyc_Core_Impossible,((_tmp6F3.f1=((_tmp6F2="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp6F2,sizeof(char),33))),_tmp6F3)))),_tmp6F1)))));}_LLEA:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp79B(unsigned int*_tmp79A,unsigned int*_tmp799,void***_tmp797,struct Cyc_List_List**_tmp333){for(*_tmp79A=0;*_tmp79A < *_tmp799;(*_tmp79A)++){
# 1386 "new_control_flow.cyc"
void*_tmp795;(*_tmp797)[*_tmp79A]=(void*)((_tmp795=(void*)((struct Cyc_List_List*)_check_null(*_tmp333))->hd,((*_tmp333=(*_tmp333)->tl,_tmp795))));}}
# 821 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 825
struct Cyc_CfFlowInfo_FlowEnv*_tmp156=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 831
{union Cyc_CfFlowInfo_FlowInfo _tmp157=inflow;struct Cyc_Dict_Dict _tmp158;struct Cyc_List_List*_tmp159;_LL106: if((_tmp157.BottomFL).tag != 1)goto _LL108;_LL107: {
struct _tuple14 _tmp6F4;return(_tmp6F4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6F4.f2=_tmp156->unknown_all,_tmp6F4)));}_LL108: if((_tmp157.ReachableFL).tag != 2)goto _LL105;_tmp158=((struct _tuple13)(_tmp157.ReachableFL).val).f1;_tmp159=((struct _tuple13)(_tmp157.ReachableFL).val).f2;_LL109:
 d=_tmp158;relns=_tmp159;_LL105:;}
# 840
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 861 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp15C;union Cyc_CfFlowInfo_AbsLVal _tmp15D;struct _tuple15 _tmp15B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp15C=_tmp15B.f1;_tmp15D=_tmp15B.f2;{
struct _tuple15 _tmp6F5;struct _tuple15 _tmp15F=(_tmp6F5.f1=_tmp15C,((_tmp6F5.f2=_tmp15D,_tmp6F5)));struct Cyc_Dict_Dict _tmp160;struct Cyc_List_List*_tmp161;struct Cyc_CfFlowInfo_Place*_tmp162;_LL10B: if(((_tmp15F.f1).ReachableFL).tag != 2)goto _LL10D;_tmp160=((struct _tuple13)((_tmp15F.f1).ReachableFL).val).f1;_tmp161=((struct _tuple13)((_tmp15F.f1).ReachableFL).val).f2;if(((_tmp15F.f2).PlaceL).tag != 1)goto _LL10D;_tmp162=(struct Cyc_CfFlowInfo_Place*)((_tmp15F.f2).PlaceL).val;_LL10C: {
# 864
void*_tmp163=Cyc_CfFlowInfo_lookup_place(_tmp160,_tmp162);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp163,& needs_unconsume)){
{const char*_tmp6F8;void*_tmp6F7;(_tmp6F7=0,Cyc_Tcutil_terr(e->loc,((_tmp6F8="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp6F8,sizeof(char),71))),_tag_dyneither(_tmp6F7,sizeof(void*),0)));}{
struct _tuple14 _tmp6F9;return(_tmp6F9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6F9.f2=_tmp156->unknown_all,_tmp6F9)));};}else{
# 870
if(needs_unconsume){
# 872
struct _tuple14 _tmp6FA;return(_tmp6FA.f1=_tmp15C,((_tmp6FA.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp156,_tmp163),_tmp6FA)));}else{
# 875
void*_tmp168=Cyc_CfFlowInfo_make_unique_consumed(_tmp156,(void*)_check_null(e->topt),e,env->iteration_num,_tmp163);
struct Cyc_Dict_Dict _tmp169=Cyc_CfFlowInfo_assign_place(_tmp156,e->loc,_tmp160,env->all_changed,_tmp162,_tmp168);
# 886
struct _tuple14 _tmp6FB;return(_tmp6FB.f1=Cyc_CfFlowInfo_ReachableFL(_tmp169,_tmp161),((_tmp6FB.f2=_tmp163,_tmp6FB)));}}}_LL10D:;_LL10E:
# 888
 goto _LL10A;_LL10A:;};}{
# 891
void*_tmp16B=e->r;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Vardecl*_tmp184;struct Cyc_Absyn_Vardecl*_tmp187;struct Cyc_Absyn_Vardecl*_tmp18A;enum Cyc_Absyn_Primop _tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_List_List*_tmp19D;int _tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;void**_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct _dyneither_ptr*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct _dyneither_ptr*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B5;struct _dyneither_ptr*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Datatypedecl*_tmp1C6;struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1CC;enum Cyc_Absyn_AggrKind _tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_Vardecl*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;int _tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;void*_tmp1D9;int _tmp1DA;struct Cyc_Absyn_Stmt*_tmp1DC;_LL110: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp16C->tag != 13)goto _LL112;else{_tmp16D=_tmp16C->f2;if(_tmp16C->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL112;}}_LL111: {
# 895
union Cyc_CfFlowInfo_FlowInfo _tmp1E4;void*_tmp1E5;struct _tuple14 _tmp1E3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp16D);_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1E7;void*_tmp1E8;struct _tuple14 _tmp1E6=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1E4,_tmp16D,_tmp1E5);_tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;{
struct _tuple14 _tmp6FC;return(_tmp6FC.f1=_tmp1E7,((_tmp6FC.f2=_tmp1E5,_tmp6FC)));};};}_LL112: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp16E->tag != 13)goto _LL114;else{_tmp16F=_tmp16E->f2;}}_LL113:
# 901
 _tmp171=_tmp16F;goto _LL115;_LL114: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp170->tag != 11)goto _LL116;else{_tmp171=_tmp170->f1;}}_LL115:
 _tmp173=_tmp171;goto _LL117;_LL116: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp172->tag != 12)goto _LL118;else{_tmp173=_tmp172->f1;}}_LL117:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp173);_LL118: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp174->tag != 0)goto _LL11A;else{if(((_tmp174->f1).Null_c).tag != 1)goto _LL11A;}}_LL119:
# 905
 goto _LL11B;_LL11A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp175->tag != 0)goto _LL11C;else{if(((_tmp175->f1).Int_c).tag != 5)goto _LL11C;if(((struct _tuple6)((_tmp175->f1).Int_c).val).f2 != 0)goto _LL11C;}}_LL11B: {
struct _tuple14 _tmp6FD;return(_tmp6FD.f1=inflow,((_tmp6FD.f2=_tmp156->zero,_tmp6FD)));}_LL11C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp176->tag != 0)goto _LL11E;else{if(((_tmp176->f1).Int_c).tag != 5)goto _LL11E;}}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp177->tag != 1)goto _LL120;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp178=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp177->f2);if(_tmp178->tag != 2)goto _LL120;}}_LL11F: {
struct _tuple14 _tmp6FE;return(_tmp6FE.f1=inflow,((_tmp6FE.f2=_tmp156->notzeroall,_tmp6FE)));}_LL120: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp179->tag != 30)goto _LL122;else{if(_tmp179->f1 != 0)goto _LL122;}}_LL121:
# 910
 goto _LL123;_LL122: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17A->tag != 0)goto _LL124;}_LL123:
 goto _LL125;_LL124: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17B->tag != 17)goto _LL126;}_LL125:
 goto _LL127;_LL126: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17C->tag != 16)goto _LL128;}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17D->tag != 18)goto _LL12A;}_LL129:
 goto _LL12B;_LL12A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17E->tag != 32)goto _LL12C;}_LL12B:
 goto _LL12D;_LL12C: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17F->tag != 31)goto _LL12E;}_LL12D: {
struct _tuple14 _tmp6FF;return(_tmp6FF.f1=inflow,((_tmp6FF.f2=_tmp156->unknown_all,_tmp6FF)));}_LL12E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp180->tag != 1)goto _LL130;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp181=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp180->f2);if(_tmp181->tag != 1)goto _LL130;}}_LL12F: {
# 919
struct _tuple14 _tmp700;return(_tmp700.f1=inflow,((_tmp700.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp700)));}_LL130: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp182->tag != 1)goto _LL132;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp183=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp182->f2);if(_tmp183->tag != 3)goto _LL132;else{_tmp184=_tmp183->f1;}}}_LL131:
# 922
 _tmp187=_tmp184;goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp185->tag != 1)goto _LL134;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp186=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp185->f2);if(_tmp186->tag != 4)goto _LL134;else{_tmp187=_tmp186->f1;}}}_LL133:
 _tmp18A=_tmp187;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp188->tag != 1)goto _LL136;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp189=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp188->f2);if(_tmp189->tag != 5)goto _LL136;else{_tmp18A=_tmp189->f1;}}}_LL135: {
# 926
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp701;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp701.tag=0,((_tmp701.f1=_tmp18A,_tmp701)));
struct _tuple14 _tmp702;return(_tmp702.f1=inflow,((_tmp702.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp702)));}_LL136: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp18B->tag != 2)goto _LL138;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LL137: {
# 933
union Cyc_CfFlowInfo_FlowInfo _tmp1F1;void*_tmp1F2;struct _tuple14 _tmp1F0=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp18D);_tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;{
struct _tuple14 _tmp703;return(_tmp703.f1=_tmp1F1,((_tmp703.f2=_tmp1F2,_tmp703)));};}_LL138: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp18E->tag != 4)goto _LL13A;else{_tmp18F=_tmp18E->f1;}}_LL139: {
# 937
struct Cyc_List_List _tmp704;struct Cyc_List_List _tmp1F4=(_tmp704.hd=_tmp18F,((_tmp704.tl=0,_tmp704)));
union Cyc_CfFlowInfo_FlowInfo _tmp1F6;struct _tuple14 _tmp1F5=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1F4);_tmp1F6=_tmp1F5.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1F8;struct _tuple15 _tmp1F7=Cyc_NewControlFlow_anal_Lexp(env,_tmp1F6,0,_tmp18F);_tmp1F8=_tmp1F7.f2;
{struct _tuple21 _tmp705;struct _tuple21 _tmp1FA=(_tmp705.f1=_tmp1F8,((_tmp705.f2=_tmp1F6,_tmp705)));struct Cyc_CfFlowInfo_Place*_tmp1FB;struct Cyc_Dict_Dict _tmp1FC;struct Cyc_List_List*_tmp1FD;_LL179: if(((_tmp1FA.f1).PlaceL).tag != 1)goto _LL17B;_tmp1FB=(struct Cyc_CfFlowInfo_Place*)((_tmp1FA.f1).PlaceL).val;if(((_tmp1FA.f2).ReachableFL).tag != 2)goto _LL17B;_tmp1FC=((struct _tuple13)((_tmp1FA.f2).ReachableFL).val).f1;_tmp1FD=((struct _tuple13)((_tmp1FA.f2).ReachableFL).val).f2;_LL17A:
# 942
 _tmp1FD=Cyc_Relations_reln_kill_exp(_tmp156->r,_tmp1FD,_tmp18F);
_tmp1F6=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp156,_tmp18F->loc,_tmp1FC,env->all_changed,_tmp1FB,_tmp156->unknown_all),_tmp1FD);
# 946
goto _LL178;_LL17B:;_LL17C:
 goto _LL178;_LL178:;}{
# 949
struct _tuple14 _tmp706;return(_tmp706.f1=_tmp1F6,((_tmp706.f2=_tmp156->unknown_all,_tmp706)));};};}_LL13A: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp190->tag != 3)goto _LL13C;else{_tmp191=_tmp190->f1;if(_tmp190->f2 == 0)goto _LL13C;_tmp192=_tmp190->f3;}}_LL13B:
# 952
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp709;void*_tmp708;(_tmp708=0,Cyc_Tcutil_terr(e->loc,((_tmp709="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp709,sizeof(char),58))),_tag_dyneither(_tmp708,sizeof(void*),0)));}{
struct _tuple14 _tmp70A;return(_tmp70A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp70A.f2=_tmp156->unknown_all,_tmp70A)));};}{
# 956
struct Cyc_List_List _tmp70B;struct Cyc_List_List _tmp203=(_tmp70B.hd=_tmp192,((_tmp70B.tl=0,_tmp70B)));
struct Cyc_List_List _tmp70C;struct Cyc_List_List _tmp204=(_tmp70C.hd=_tmp191,((_tmp70C.tl=& _tmp203,_tmp70C)));
union Cyc_CfFlowInfo_FlowInfo _tmp206;struct _tuple14 _tmp205=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp204);_tmp206=_tmp205.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp208;struct _tuple15 _tmp207=Cyc_NewControlFlow_anal_Lexp(env,_tmp206,0,_tmp191);_tmp208=_tmp207.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp209=_tmp206;struct Cyc_Dict_Dict _tmp20A;struct Cyc_List_List*_tmp20B;_LL17E: if((_tmp209.ReachableFL).tag != 2)goto _LL180;_tmp20A=((struct _tuple13)(_tmp209.ReachableFL).val).f1;_tmp20B=((struct _tuple13)(_tmp209.ReachableFL).val).f2;_LL17F:
# 962
{union Cyc_CfFlowInfo_AbsLVal _tmp20C=_tmp208;struct Cyc_CfFlowInfo_Place*_tmp20D;_LL183: if((_tmp20C.PlaceL).tag != 1)goto _LL185;_tmp20D=(struct Cyc_CfFlowInfo_Place*)(_tmp20C.PlaceL).val;_LL184:
# 966
 _tmp20B=Cyc_Relations_reln_kill_exp(_tmp156->r,_tmp20B,_tmp191);
_tmp20A=Cyc_CfFlowInfo_assign_place(_tmp156,_tmp191->loc,_tmp20A,env->all_changed,_tmp20D,_tmp156->unknown_all);
# 969
_tmp206=Cyc_CfFlowInfo_ReachableFL(_tmp20A,_tmp20B);
# 972
goto _LL182;_LL185: if((_tmp20C.UnknownL).tag != 2)goto _LL182;_LL186:
# 975
 goto _LL182;_LL182:;}
# 977
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}{
# 980
struct _tuple14 _tmp70D;return(_tmp70D.f1=_tmp206,((_tmp70D.f2=_tmp156->unknown_all,_tmp70D)));};};};_LL13C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp193->tag != 3)goto _LL13E;else{_tmp194=_tmp193->f1;if(_tmp193->f2 != 0)goto _LL13E;_tmp195=_tmp193->f3;}}_LL13D:
# 984
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp710;void*_tmp70F;(_tmp70F=0,Cyc_Tcutil_terr(e->loc,((_tmp710="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp710,sizeof(char),58))),_tag_dyneither(_tmp70F,sizeof(void*),0)));}{
struct _tuple14 _tmp711;return(_tmp711.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp711.f2=_tmp156->unknown_all,_tmp711)));};}{
# 988
struct Cyc_Dict_Dict*_tmp214=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp712;env->all_changed=((_tmp712=_region_malloc(env->r,sizeof(*_tmp712)),((_tmp712[0]=_tmp156->mt_place_set,_tmp712))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp217;union Cyc_CfFlowInfo_AbsLVal _tmp218;struct _tuple15 _tmp216=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp194);_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;{
struct Cyc_Dict_Dict _tmp219=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp713;env->all_changed=((_tmp713=_region_malloc(env->r,sizeof(*_tmp713)),((_tmp713[0]=_tmp156->mt_place_set,_tmp713))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*_tmp21D;struct _tuple14 _tmp21B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp195);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;{
struct Cyc_Dict_Dict _tmp21E=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp21F=Cyc_CfFlowInfo_after_flow(_tmp156,& _tmp219,_tmp217,_tmp21C,_tmp219,_tmp21E);
# 999
union Cyc_CfFlowInfo_FlowInfo _tmp220=Cyc_CfFlowInfo_join_flow(_tmp156,_tmp214,inflow,_tmp21F);
# 1002
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp220,inflow)){
if(_tmp214 == 0)
env->all_changed=0;else{
# 1006
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp214,
Cyc_CfFlowInfo_union_place_set(_tmp219,_tmp21E,0),0);}
# 1009
return Cyc_NewControlFlow_do_assign(_tmp156,env,_tmp21F,_tmp194,_tmp218,_tmp195,_tmp21D,e->loc);}
# 1012
inflow=_tmp220;};};};};}};_LL13E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp196->tag != 8)goto _LL140;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LL13F: {
# 1017
union Cyc_CfFlowInfo_FlowInfo _tmp222;void*_tmp223;struct _tuple14 _tmp221=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp197);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp222,_tmp198);}_LL140: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp199->tag != 10)goto _LL142;else{_tmp19A=_tmp199->f1;}}_LL141: {
# 1021
union Cyc_CfFlowInfo_FlowInfo _tmp225;void*_tmp226;struct _tuple14 _tmp224=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19A);_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp19A->loc,_tmp225,_tmp226);{
struct _tuple14 _tmp714;return(_tmp714.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp714.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp714)));};}_LL142: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp19B->tag != 9)goto _LL144;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL143: {
# 1026
struct _RegionHandle _tmp228=_new_region("temp");struct _RegionHandle*temp=& _tmp228;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp715;struct _tuple20 _tmp22A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp715=_region_malloc(temp,sizeof(*_tmp715)),((_tmp715->hd=_tmp19C,((_tmp715->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp19D),_tmp715)))))),0,1);_tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;{
# 1029
union Cyc_CfFlowInfo_FlowInfo _tmp22D=Cyc_NewControlFlow_use_Rval(env,_tmp19C->loc,_tmp22B,(void*)((struct Cyc_List_List*)_check_null(_tmp22C))->hd);
_tmp22C=_tmp22C->tl;{
# 1032
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(_tmp19C->topt));void*_tmp230;_LL188: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LL18A;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LL189:
# 1037
{void*_tmp231=Cyc_Tcutil_compress(_tmp230);struct Cyc_List_List*_tmp233;_LL18D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 9)goto _LL18F;else{_tmp233=(_tmp232->f1).attributes;}}_LL18E:
# 1039
 for(0;_tmp233 != 0;_tmp233=_tmp233->tl){
# 1041
void*_tmp234=(void*)_tmp233->hd;int _tmp236;int _tmp238;int _tmp23A;_LL192: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp235=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp234;if(_tmp235->tag != 20)goto _LL194;else{_tmp236=_tmp235->f1;}}_LL193:
# 1043
{struct Cyc_List_List*_tmp716;init_params=((_tmp716=_region_malloc(temp,sizeof(*_tmp716)),((_tmp716->hd=(void*)_tmp236,((_tmp716->tl=init_params,_tmp716))))));}goto _LL191;_LL194: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp237=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp234;if(_tmp237->tag != 21)goto _LL196;else{_tmp238=_tmp237->f1;}}_LL195:
# 1045
{struct Cyc_List_List*_tmp717;nolive_unique_params=((_tmp717=_region_malloc(temp,sizeof(*_tmp717)),((_tmp717->hd=(void*)_tmp238,((_tmp717->tl=nolive_unique_params,_tmp717))))));}
goto _LL191;_LL196: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp239=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp234;if(_tmp239->tag != 22)goto _LL198;else{_tmp23A=_tmp239->f1;}}_LL197:
# 1049
{struct Cyc_List_List*_tmp718;noconsume_params=((_tmp718=_region_malloc(temp,sizeof(*_tmp718)),((_tmp718->hd=(void*)_tmp23A,((_tmp718->tl=noconsume_params,_tmp718))))));}
goto _LL191;_LL198:;_LL199:
 goto _LL191;_LL191:;}
# 1053
goto _LL18C;_LL18F:;_LL190: {
const char*_tmp71B;void*_tmp71A;(_tmp71A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp71B="anal_Rexp: bad function type",_tag_dyneither(_tmp71B,sizeof(char),29))),_tag_dyneither(_tmp71A,sizeof(void*),0)));}_LL18C:;}
# 1056
goto _LL187;_LL18A:;_LL18B: {
const char*_tmp71E;void*_tmp71D;(_tmp71D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp71E="anal_Rexp: bad function type",_tag_dyneither(_tmp71E,sizeof(char),29))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}_LL187:;}
# 1059
{int i=1;for(0;_tmp22C != 0;(((_tmp22C=_tmp22C->tl,_tmp19D=((struct Cyc_List_List*)_check_null(_tmp19D))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp242=_tmp22B;struct Cyc_Dict_Dict _tmp243;_LL19B: if((_tmp242.BottomFL).tag != 1)goto _LL19D;_LL19C:
 goto _LL19A;_LL19D: if((_tmp242.ReachableFL).tag != 2)goto _LL19A;_tmp243=((struct _tuple13)(_tmp242.ReachableFL).val).f1;_LL19E:
# 1064
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp243,(void*)_tmp22C->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp721;void*_tmp720;(_tmp720=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd)->loc,((_tmp721="expression may not be initialized",_tag_dyneither(_tmp721,sizeof(char),34))),_tag_dyneither(_tmp720,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp246=_tmp22D;struct Cyc_Dict_Dict _tmp247;struct Cyc_List_List*_tmp248;_LL1A0: if((_tmp246.BottomFL).tag != 1)goto _LL1A2;_LL1A1:
 goto _LL19F;_LL1A2: if((_tmp246.ReachableFL).tag != 2)goto _LL19F;_tmp247=((struct _tuple13)(_tmp246.ReachableFL).val).f1;_tmp248=((struct _tuple13)(_tmp246.ReachableFL).val).f2;_LL1A3: {
# 1071
struct Cyc_Dict_Dict _tmp249=Cyc_CfFlowInfo_escape_deref(_tmp156,_tmp247,env->all_changed,(void*)_tmp22C->hd);
{void*_tmp24A=(void*)_tmp22C->hd;struct Cyc_CfFlowInfo_Place*_tmp24C;_LL1A5: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp24B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp24A;if(_tmp24B->tag != 5)goto _LL1A7;else{_tmp24C=_tmp24B->f1;}}_LL1A6:
# 1074
{void*_tmp24D=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd)->topt));void*_tmp24F;_LL1AA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24D;if(_tmp24E->tag != 5)goto _LL1AC;else{_tmp24F=(_tmp24E->f1).elt_typ;}}_LL1AB:
# 1078
 _tmp249=Cyc_CfFlowInfo_assign_place(_tmp156,((struct Cyc_Absyn_Exp*)_tmp19D->hd)->loc,_tmp249,env->all_changed,_tmp24C,
# 1080
Cyc_CfFlowInfo_typ_to_absrval(_tmp156,_tmp24F,0,_tmp156->esc_all));
# 1082
goto _LL1A9;_LL1AC:;_LL1AD: {
const char*_tmp724;void*_tmp723;(_tmp723=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp724="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp724,sizeof(char),39))),_tag_dyneither(_tmp723,sizeof(void*),0)));}_LL1A9:;}
# 1085
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}
# 1088
_tmp22D=Cyc_CfFlowInfo_ReachableFL(_tmp249,_tmp248);
goto _LL19F;}_LL19F:;}
# 1091
goto _LL19A;_LL19A:;}else{
# 1094
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1097
_tmp22D=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd)->topt),_tmp22D,(void*)_tmp22C->hd);else{
# 1104
_tmp22D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd)->loc,_tmp22D,(void*)_tmp22C->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1107
union Cyc_CfFlowInfo_AbsLVal _tmp253;struct _tuple15 _tmp252=Cyc_NewControlFlow_anal_Lexp(env,_tmp22D,1,(struct Cyc_Absyn_Exp*)_tmp19D->hd);_tmp253=_tmp252.f2;{
struct _tuple15 _tmp725;struct _tuple15 _tmp255=(_tmp725.f1=_tmp22D,((_tmp725.f2=_tmp253,_tmp725)));struct Cyc_Dict_Dict _tmp256;struct Cyc_List_List*_tmp257;struct Cyc_CfFlowInfo_Place*_tmp258;_LL1AF: if(((_tmp255.f1).ReachableFL).tag != 2)goto _LL1B1;_tmp256=((struct _tuple13)((_tmp255.f1).ReachableFL).val).f1;_tmp257=((struct _tuple13)((_tmp255.f1).ReachableFL).val).f2;if(((_tmp255.f2).PlaceL).tag != 1)goto _LL1B1;_tmp258=(struct Cyc_CfFlowInfo_Place*)((_tmp255.f2).PlaceL).val;_LL1B0: {
# 1110
void*_tmp259=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp156,(void*)_tmp22C->hd);
# 1113
_tmp256=Cyc_CfFlowInfo_assign_place(_tmp156,((struct Cyc_Absyn_Exp*)_tmp19D->hd)->loc,_tmp256,env->all_changed,_tmp258,_tmp259);
_tmp22D=Cyc_CfFlowInfo_ReachableFL(_tmp256,_tmp257);
# 1118
goto _LL1AE;}_LL1B1: if(((_tmp255.f1).BottomFL).tag != 1)goto _LL1B3;_LL1B2:
 continue;_LL1B3:;_LL1B4: {
# 1121
const char*_tmp728;void*_tmp727;(_tmp727=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp19D->hd)->loc,((_tmp728="noconsume() parameters must be l-values",_tag_dyneither(_tmp728,sizeof(char),40))),_tag_dyneither(_tmp727,sizeof(void*),0)));}_LL1AE:;};}}}}}
# 1128
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp19C->topt))){
struct _tuple14 _tmp729;struct _tuple14 _tmp25D=(_tmp729.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp729.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp729)));_npop_handler(0);return _tmp25D;}else{
# 1131
struct _tuple14 _tmp72A;struct _tuple14 _tmp25F=(_tmp72A.f1=_tmp22D,((_tmp72A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp72A)));_npop_handler(0);return _tmp25F;}};};}
# 1027
;_pop_region(temp);}_LL144: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp19E->tag != 33)goto _LL146;else{_tmp19F=(_tmp19E->f1).is_calloc;_tmp1A0=(_tmp19E->f1).rgn;_tmp1A1=(_tmp19E->f1).elt_type;_tmp1A2=(_tmp19E->f1).num_elts;_tmp1A3=(_tmp19E->f1).fat_result;}}_LL145: {
# 1134
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp72D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp72C;void*root=(void*)((_tmp72C=_region_malloc(_tmp156->r,sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp72D.tag=1,((_tmp72D.f1=_tmp1A2,((_tmp72D.f2=(void*)_check_null(e->topt),_tmp72D)))))),_tmp72C))));
struct Cyc_CfFlowInfo_Place*_tmp72E;struct Cyc_CfFlowInfo_Place*place=(_tmp72E=_region_malloc(_tmp156->r,sizeof(*_tmp72E)),((_tmp72E->root=root,((_tmp72E->fields=0,_tmp72E)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp731;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp730;void*rval=(void*)((_tmp730=_region_malloc(_tmp156->r,sizeof(*_tmp730)),((_tmp730[0]=((_tmp731.tag=5,((_tmp731.f1=place,_tmp731)))),_tmp730))));
void*place_val;
if(_tmp1A3)place_val=_tmp156->notzeroall;else{
if(_tmp19F)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,*((void**)_check_null(_tmp1A1)),0,_tmp156->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,*((void**)_check_null(_tmp1A1)),0,_tmp156->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1A0 != 0){
struct _RegionHandle _tmp260=_new_region("temp");struct _RegionHandle*temp=& _tmp260;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp263;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_Exp*_tmp732[2];struct _tuple20 _tmp262=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp732[1]=_tmp1A2,((_tmp732[0]=_tmp1A0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp732,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;
outflow=_tmp263;}
# 1145
;_pop_region(temp);}else{
# 1150
union Cyc_CfFlowInfo_FlowInfo _tmp266;struct _tuple14 _tmp265=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A2);_tmp266=_tmp265.f1;
outflow=_tmp266;}{
# 1153
union Cyc_CfFlowInfo_FlowInfo _tmp267=outflow;struct Cyc_Dict_Dict _tmp268;struct Cyc_List_List*_tmp269;_LL1B6: if((_tmp267.BottomFL).tag != 1)goto _LL1B8;_LL1B7: {
struct _tuple14 _tmp733;return(_tmp733.f1=outflow,((_tmp733.f2=rval,_tmp733)));}_LL1B8: if((_tmp267.ReachableFL).tag != 2)goto _LL1B5;_tmp268=((struct _tuple13)(_tmp267.ReachableFL).val).f1;_tmp269=((struct _tuple13)(_tmp267.ReachableFL).val).f2;_LL1B9: {
# 1156
struct _tuple14 _tmp734;return(_tmp734.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp268,root,place_val),_tmp269),((_tmp734.f2=rval,_tmp734)));}_LL1B5:;};};}_LL146: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1A4->tag != 34)goto _LL148;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LL147: {
# 1160
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1166
struct _RegionHandle _tmp271=_new_region("temp");struct _RegionHandle*temp=& _tmp271;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp274;struct Cyc_List_List*_tmp275;struct Cyc_Absyn_Exp*_tmp735[2];struct _tuple20 _tmp273=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp735[1]=_tmp1A6,((_tmp735[0]=_tmp1A5,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp735,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp275))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp275->tl))->hd;
outflow=_tmp274;}{
# 1174
union Cyc_CfFlowInfo_AbsLVal _tmp277;struct _tuple15 _tmp276=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1A5);_tmp277=_tmp276.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp279;struct _tuple15 _tmp278=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1A6);_tmp279=_tmp278.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp27A=outflow;struct Cyc_Dict_Dict _tmp27B;struct Cyc_List_List*_tmp27C;_LL1BB: if((_tmp27A.ReachableFL).tag != 2)goto _LL1BD;_tmp27B=((struct _tuple13)(_tmp27A.ReachableFL).val).f1;_tmp27C=((struct _tuple13)(_tmp27A.ReachableFL).val).f2;_LL1BC:
# 1178
{union Cyc_CfFlowInfo_AbsLVal _tmp27D=_tmp277;struct Cyc_CfFlowInfo_Place*_tmp27E;_LL1C0: if((_tmp27D.PlaceL).tag != 1)goto _LL1C2;_tmp27E=(struct Cyc_CfFlowInfo_Place*)(_tmp27D.PlaceL).val;_LL1C1:
# 1180
 _tmp27B=Cyc_CfFlowInfo_assign_place(_tmp156,_tmp1A5->loc,_tmp27B,env->all_changed,_tmp27E,right_rval);
# 1182
goto _LL1BF;_LL1C2: if((_tmp27D.UnknownL).tag != 2)goto _LL1BF;_LL1C3:
 goto _LL1BF;_LL1BF:;}
# 1185
{union Cyc_CfFlowInfo_AbsLVal _tmp27F=_tmp279;struct Cyc_CfFlowInfo_Place*_tmp280;_LL1C5: if((_tmp27F.PlaceL).tag != 1)goto _LL1C7;_tmp280=(struct Cyc_CfFlowInfo_Place*)(_tmp27F.PlaceL).val;_LL1C6:
# 1187
 _tmp27B=Cyc_CfFlowInfo_assign_place(_tmp156,_tmp1A6->loc,_tmp27B,env->all_changed,_tmp280,left_rval);
# 1189
goto _LL1C4;_LL1C7: if((_tmp27F.UnknownL).tag != 2)goto _LL1C4;_LL1C8:
 goto _LL1C4;_LL1C4:;}
# 1193
_tmp27C=Cyc_Relations_reln_kill_exp(_tmp156->r,_tmp27C,_tmp1A5);
_tmp27C=Cyc_Relations_reln_kill_exp(_tmp156->r,_tmp27C,_tmp1A6);
# 1196
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp27B,_tmp27C);
goto _LL1BA;_LL1BD:;_LL1BE:
 goto _LL1BA;_LL1BA:;}{
# 1202
struct _tuple14 _tmp736;struct _tuple14 _tmp282=(_tmp736.f1=outflow,((_tmp736.f2=_tmp156->unknown_all,_tmp736)));_npop_handler(0);return _tmp282;};};};
# 1166
;_pop_region(temp);}_LL148: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1A7->tag != 15)goto _LL14A;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL149: {
# 1205
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp739;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp738;void*root=(void*)((_tmp738=_region_malloc(_tmp156->r,sizeof(*_tmp738)),((_tmp738[0]=((_tmp739.tag=1,((_tmp739.f1=_tmp1A9,((_tmp739.f2=(void*)_check_null(e->topt),_tmp739)))))),_tmp738))));
struct Cyc_CfFlowInfo_Place*_tmp73A;struct Cyc_CfFlowInfo_Place*place=(_tmp73A=_region_malloc(_tmp156->r,sizeof(*_tmp73A)),((_tmp73A->root=root,((_tmp73A->fields=0,_tmp73A)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp73D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp73C;void*rval=(void*)((_tmp73C=_region_malloc(_tmp156->r,sizeof(*_tmp73C)),((_tmp73C[0]=((_tmp73D.tag=5,((_tmp73D.f1=place,_tmp73D)))),_tmp73C))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1A8 != 0){
struct _RegionHandle _tmp283=_new_region("temp");struct _RegionHandle*temp=& _tmp283;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp286;struct Cyc_List_List*_tmp287;struct Cyc_Absyn_Exp*_tmp73E[2];struct _tuple20 _tmp285=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp73E[1]=_tmp1A9,((_tmp73E[0]=_tmp1A8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp73E,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;
outflow=_tmp286;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp287))->tl))->hd;}
# 1213
;_pop_region(temp);}else{
# 1219
union Cyc_CfFlowInfo_FlowInfo _tmp289;void*_tmp28A;struct _tuple14 _tmp288=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A9);_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;
outflow=_tmp289;
place_val=_tmp28A;}{
# 1223
union Cyc_CfFlowInfo_FlowInfo _tmp28B=outflow;struct Cyc_Dict_Dict _tmp28C;struct Cyc_List_List*_tmp28D;_LL1CA: if((_tmp28B.BottomFL).tag != 1)goto _LL1CC;_LL1CB: {
struct _tuple14 _tmp73F;return(_tmp73F.f1=outflow,((_tmp73F.f2=rval,_tmp73F)));}_LL1CC: if((_tmp28B.ReachableFL).tag != 2)goto _LL1C9;_tmp28C=((struct _tuple13)(_tmp28B.ReachableFL).val).f1;_tmp28D=((struct _tuple13)(_tmp28B.ReachableFL).val).f2;_LL1CD: {
# 1226
struct _tuple14 _tmp740;return(_tmp740.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp28C,root,place_val),_tmp28D),((_tmp740.f2=rval,_tmp740)));}_LL1C9:;};};}_LL14A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1AA->tag != 14)goto _LL14C;else{_tmp1AB=_tmp1AA->f1;}}_LL14B: {
# 1230
union Cyc_CfFlowInfo_FlowInfo _tmp296;union Cyc_CfFlowInfo_AbsLVal _tmp297;struct _tuple15 _tmp295=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1AB);_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp298=_tmp297;struct Cyc_CfFlowInfo_Place*_tmp299;_LL1CF: if((_tmp298.UnknownL).tag != 2)goto _LL1D1;_LL1D0: {
struct _tuple14 _tmp741;return(_tmp741.f1=_tmp296,((_tmp741.f2=_tmp156->notzeroall,_tmp741)));}_LL1D1: if((_tmp298.PlaceL).tag != 1)goto _LL1CE;_tmp299=(struct Cyc_CfFlowInfo_Place*)(_tmp298.PlaceL).val;_LL1D2: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp747;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp746;struct _tuple14 _tmp745;return(_tmp745.f1=_tmp296,((_tmp745.f2=(void*)((_tmp747=_region_malloc(env->r,sizeof(*_tmp747)),((_tmp747[0]=((_tmp746.tag=5,((_tmp746.f1=_tmp299,_tmp746)))),_tmp747)))),_tmp745)));}_LL1CE:;};}_LL14C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1AC->tag != 19)goto _LL14E;else{_tmp1AD=_tmp1AC->f1;}}_LL14D: {
# 1237
union Cyc_CfFlowInfo_FlowInfo _tmp29F;void*_tmp2A0;struct _tuple14 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AD);_tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp29F,_tmp1AD,_tmp2A0);}_LL14E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1AE->tag != 20)goto _LL150;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LL14F: {
# 1241
union Cyc_CfFlowInfo_FlowInfo _tmp2A2;void*_tmp2A3;struct _tuple14 _tmp2A1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AF);_tmp2A2=_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;{
void*_tmp2A4=(void*)_check_null(_tmp1AF->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2A4)){
# 1245
struct _tuple14 _tmp748;return(_tmp748.f1=_tmp2A2,((_tmp748.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp748)));}
# 1247
if(Cyc_Absyn_is_require_union_type(_tmp2A4))
Cyc_NewControlFlow_check_requires(_tmp1AF->loc,_tmp156->r,_tmp2A4,_tmp1B0,_tmp2A2);{
# 1250
void*_tmp2A6=_tmp2A3;int _tmp2A9;int _tmp2AA;struct _dyneither_ptr _tmp2AB;int _tmp2AD;int _tmp2AE;struct _dyneither_ptr _tmp2AF;_LL1D4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2A7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2A6;if(_tmp2A7->tag != 8)goto _LL1D6;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2A7->f2);if(_tmp2A8->tag != 6)goto _LL1D6;else{_tmp2A9=(_tmp2A8->f1).is_union;_tmp2AA=(_tmp2A8->f1).fieldnum;_tmp2AB=_tmp2A8->f2;}}}_LL1D5:
# 1252
 _tmp2AD=_tmp2A9;_tmp2AE=_tmp2AA;_tmp2AF=_tmp2AB;goto _LL1D7;_LL1D6: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2A6;if(_tmp2AC->tag != 6)goto _LL1D8;else{_tmp2AD=(_tmp2AC->f1).is_union;_tmp2AE=(_tmp2AC->f1).fieldnum;_tmp2AF=_tmp2AC->f2;}}_LL1D7: {
# 1254
int _tmp2B0=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1AF->topt),_tmp1B0);
if((_tmp2AD  && _tmp2AE != - 1) && _tmp2AE != _tmp2B0){
struct _tuple14 _tmp749;return(_tmp749.f1=_tmp2A2,((_tmp749.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),1,_tmp156->unknown_none),_tmp749)));}{
struct _tuple14 _tmp74A;return(_tmp74A.f1=_tmp2A2,((_tmp74A.f2=*((void**)_check_dyneither_subscript(_tmp2AF,sizeof(void*),_tmp2B0)),_tmp74A)));};}_LL1D8:;_LL1D9:
# 1259
{const char*_tmp74D;void*_tmp74C;(_tmp74C=0,Cyc_fprintf(Cyc_stderr,((_tmp74D="the bad rexp is :",_tag_dyneither(_tmp74D,sizeof(char),18))),_tag_dyneither(_tmp74C,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2A3);
{const char*_tmp750;void*_tmp74F;(_tmp74F=0,Cyc_fprintf(Cyc_stderr,((_tmp750="\n",_tag_dyneither(_tmp750,sizeof(char),2))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp75D;const char*_tmp75C;void*_tmp75B[1];struct Cyc_String_pa_PrintArg_struct _tmp75A;struct Cyc_Core_Impossible_exn_struct*_tmp759;(int)_throw((void*)((_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759[0]=((_tmp75D.tag=Cyc_Core_Impossible,((_tmp75D.f1=(struct _dyneither_ptr)((_tmp75A.tag=0,((_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp75B[0]=& _tmp75A,Cyc_aprintf(((_tmp75C="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp75C,sizeof(char),26))),_tag_dyneither(_tmp75B,sizeof(void*),1)))))))),_tmp75D)))),_tmp759)))));};_LL1D3:;};};}_LL150: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1B1->tag != 37)goto _LL152;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LL151: {
# 1269
union Cyc_CfFlowInfo_FlowInfo _tmp2BD;void*_tmp2BE;struct _tuple14 _tmp2BC=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1B2);_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;
# 1271
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1B2->topt))){
struct _tuple14 _tmp75E;return(_tmp75E.f1=_tmp2BD,((_tmp75E.f2=_tmp156->unknown_all,_tmp75E)));}{
void*_tmp2C0=_tmp2BE;int _tmp2C3;int _tmp2C4;struct _dyneither_ptr _tmp2C5;int _tmp2C7;int _tmp2C8;struct _dyneither_ptr _tmp2C9;_LL1DB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2C1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2C0;if(_tmp2C1->tag != 8)goto _LL1DD;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2C1->f2);if(_tmp2C2->tag != 6)goto _LL1DD;else{_tmp2C3=(_tmp2C2->f1).is_union;_tmp2C4=(_tmp2C2->f1).fieldnum;_tmp2C5=_tmp2C2->f2;}}}_LL1DC:
# 1275
 _tmp2C7=_tmp2C3;_tmp2C8=_tmp2C4;_tmp2C9=_tmp2C5;goto _LL1DE;_LL1DD: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C0;if(_tmp2C6->tag != 6)goto _LL1DF;else{_tmp2C7=(_tmp2C6->f1).is_union;_tmp2C8=(_tmp2C6->f1).fieldnum;_tmp2C9=_tmp2C6->f2;}}_LL1DE: {
# 1277
int _tmp2CA=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1B2->topt),_tmp1B3);
if(_tmp2C7  && _tmp2C8 != - 1){
if(_tmp2C8 != _tmp2CA){
struct _tuple14 _tmp75F;return(_tmp75F.f1=_tmp2BD,((_tmp75F.f2=_tmp156->zero,_tmp75F)));}else{
# 1282
struct _tuple14 _tmp760;return(_tmp760.f1=_tmp2BD,((_tmp760.f2=_tmp156->notzeroall,_tmp760)));}}else{
# 1284
struct _tuple14 _tmp761;return(_tmp761.f1=_tmp2BD,((_tmp761.f2=_tmp156->unknown_all,_tmp761)));}}_LL1DF:;_LL1E0: {
# 1286
struct Cyc_Core_Impossible_exn_struct _tmp76E;const char*_tmp76D;void*_tmp76C[1];struct Cyc_String_pa_PrintArg_struct _tmp76B;struct Cyc_Core_Impossible_exn_struct*_tmp76A;(int)_throw((void*)((_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A[0]=((_tmp76E.tag=Cyc_Core_Impossible,((_tmp76E.f1=(struct _dyneither_ptr)((_tmp76B.tag=0,((_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp76C[0]=& _tmp76B,Cyc_aprintf(((_tmp76D="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp76D,sizeof(char),26))),_tag_dyneither(_tmp76C,sizeof(void*),1)))))))),_tmp76E)))),_tmp76A)))));}_LL1DA:;};}_LL152: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1B4->tag != 21)goto _LL154;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;}}_LL153: {
# 1291
union Cyc_CfFlowInfo_FlowInfo _tmp2D4;void*_tmp2D5;struct _tuple14 _tmp2D3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1B5);_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;{
# 1294
union Cyc_CfFlowInfo_FlowInfo _tmp2D7;void*_tmp2D8;struct _tuple14 _tmp2D6=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2D4,_tmp1B5,_tmp2D5);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{
# 1297
void*_tmp2D9=Cyc_Tcutil_compress((void*)_check_null(_tmp1B5->topt));void*_tmp2DB;_LL1E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9;if(_tmp2DA->tag != 5)goto _LL1E4;else{_tmp2DB=(_tmp2DA->f1).elt_typ;}}_LL1E3:
# 1299
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2DB)){
# 1301
struct _tuple14 _tmp76F;return(_tmp76F.f1=_tmp2D7,((_tmp76F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp76F)));}
# 1303
if(Cyc_Absyn_is_require_union_type(_tmp2DB))
Cyc_NewControlFlow_check_requires(_tmp1B5->loc,_tmp156->r,_tmp2DB,_tmp1B6,_tmp2D4);{
# 1306
void*_tmp2DD=_tmp2D8;int _tmp2E0;int _tmp2E1;struct _dyneither_ptr _tmp2E2;int _tmp2E4;int _tmp2E5;struct _dyneither_ptr _tmp2E6;_LL1E7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2DE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2DD;if(_tmp2DE->tag != 8)goto _LL1E9;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2DE->f2);if(_tmp2DF->tag != 6)goto _LL1E9;else{_tmp2E0=(_tmp2DF->f1).is_union;_tmp2E1=(_tmp2DF->f1).fieldnum;_tmp2E2=_tmp2DF->f2;}}}_LL1E8:
# 1308
 _tmp2E4=_tmp2E0;_tmp2E5=_tmp2E1;_tmp2E6=_tmp2E2;goto _LL1EA;_LL1E9: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2DD;if(_tmp2E3->tag != 6)goto _LL1EB;else{_tmp2E4=(_tmp2E3->f1).is_union;_tmp2E5=(_tmp2E3->f1).fieldnum;_tmp2E6=_tmp2E3->f2;}}_LL1EA: {
# 1310
int _tmp2E7=Cyc_CfFlowInfo_get_field_index(_tmp2DB,_tmp1B6);
if((_tmp2E4  && _tmp2E5 != - 1) && _tmp2E5 != _tmp2E7){
struct _tuple14 _tmp770;return(_tmp770.f1=_tmp2D7,((_tmp770.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),1,_tmp156->unknown_none),_tmp770)));}{
struct _tuple14 _tmp771;return(_tmp771.f1=_tmp2D7,((_tmp771.f2=*((void**)_check_dyneither_subscript(_tmp2E6,sizeof(void*),_tmp2E7)),_tmp771)));};}_LL1EB:;_LL1EC: {
struct Cyc_Core_Impossible_exn_struct _tmp777;const char*_tmp776;struct Cyc_Core_Impossible_exn_struct*_tmp775;(int)_throw((void*)((_tmp775=_cycalloc(sizeof(*_tmp775)),((_tmp775[0]=((_tmp777.tag=Cyc_Core_Impossible,((_tmp777.f1=((_tmp776="anal_Rexp: AggrArrow",_tag_dyneither(_tmp776,sizeof(char),21))),_tmp777)))),_tmp775)))));}_LL1E6:;};_LL1E4:;_LL1E5: {
# 1316
struct Cyc_Core_Impossible_exn_struct _tmp77D;const char*_tmp77C;struct Cyc_Core_Impossible_exn_struct*_tmp77B;(int)_throw((void*)((_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B[0]=((_tmp77D.tag=Cyc_Core_Impossible,((_tmp77D.f1=((_tmp77C="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp77C,sizeof(char),25))),_tmp77D)))),_tmp77B)))));}_LL1E1:;};};}_LL154: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1B7->tag != 5)goto _LL156;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;_tmp1BA=_tmp1B7->f3;}}_LL155: {
# 1320
union Cyc_CfFlowInfo_FlowInfo _tmp2F1;union Cyc_CfFlowInfo_FlowInfo _tmp2F2;struct _tuple16 _tmp2F0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B8);_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;{
struct _tuple14 _tmp2F3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F1,_tmp1B9);
struct _tuple14 _tmp2F4=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F2,_tmp1BA);
# 1324
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp156,env->all_changed,_tmp2F3,_tmp2F4);};}_LL156: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1BB->tag != 6)goto _LL158;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL157: {
# 1327
union Cyc_CfFlowInfo_FlowInfo _tmp2F6;union Cyc_CfFlowInfo_FlowInfo _tmp2F7;struct _tuple16 _tmp2F5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1BC);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F9;void*_tmp2FA;struct _tuple14 _tmp2F8=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2F6,_tmp1BD);_tmp2F9=_tmp2F8.f1;_tmp2FA=_tmp2F8.f2;{
struct _tuple14 _tmp77E;struct _tuple14 _tmp2FB=(_tmp77E.f1=_tmp2F9,((_tmp77E.f2=_tmp2FA,_tmp77E)));
struct _tuple14 _tmp77F;struct _tuple14 _tmp2FC=(_tmp77F.f1=_tmp2F7,((_tmp77F.f2=_tmp156->zero,_tmp77F)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp156,env->all_changed,_tmp2FB,_tmp2FC);};};}_LL158: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1BE->tag != 7)goto _LL15A;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;}}_LL159: {
# 1334
union Cyc_CfFlowInfo_FlowInfo _tmp300;union Cyc_CfFlowInfo_FlowInfo _tmp301;struct _tuple16 _tmp2FF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1BF);_tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp303;void*_tmp304;struct _tuple14 _tmp302=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp301,_tmp1C0);_tmp303=_tmp302.f1;_tmp304=_tmp302.f2;{
struct _tuple14 _tmp780;struct _tuple14 _tmp305=(_tmp780.f1=_tmp303,((_tmp780.f2=_tmp304,_tmp780)));
struct _tuple14 _tmp781;struct _tuple14 _tmp306=(_tmp781.f1=_tmp300,((_tmp781.f2=_tmp156->notzeroall,_tmp781)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp156,env->all_changed,_tmp305,_tmp306);};};}_LL15A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1C1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1C1->tag != 22)goto _LL15C;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;}}_LL15B: {
# 1341
struct _RegionHandle _tmp309=_new_region("temp");struct _RegionHandle*temp=& _tmp309;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp30C;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Exp*_tmp782[2];struct _tuple20 _tmp30B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp782[1]=_tmp1C3,((_tmp782[0]=_tmp1C2,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp782,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;{
# 1346
union Cyc_CfFlowInfo_FlowInfo _tmp30E=_tmp30C;
{union Cyc_CfFlowInfo_FlowInfo _tmp30F=_tmp30C;struct Cyc_Dict_Dict _tmp310;struct Cyc_List_List*_tmp311;_LL1EE: if((_tmp30F.ReachableFL).tag != 2)goto _LL1F0;_tmp310=((struct _tuple13)(_tmp30F.ReachableFL).val).f1;_tmp311=((struct _tuple13)(_tmp30F.ReachableFL).val).f2;_LL1EF:
# 1351
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp310,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp30D))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp785;void*_tmp784;(_tmp784=0,Cyc_Tcutil_terr(_tmp1C3->loc,((_tmp785="expression may not be initialized",_tag_dyneither(_tmp785,sizeof(char),34))),_tag_dyneither(_tmp784,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp314=Cyc_NewControlFlow_add_subscript_reln(_tmp156->r,_tmp311,_tmp1C2,_tmp1C3);
if(_tmp311 != _tmp314)
_tmp30E=Cyc_CfFlowInfo_ReachableFL(_tmp310,_tmp314);
goto _LL1ED;};_LL1F0:;_LL1F1:
 goto _LL1ED;_LL1ED:;}{
# 1359
void*_tmp315=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));struct Cyc_List_List*_tmp317;union Cyc_Absyn_Constraint*_tmp319;_LL1F3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp315;if(_tmp316->tag != 10)goto _LL1F5;else{_tmp317=_tmp316->f1;}}_LL1F4: {
# 1361
void*_tmp31A=(void*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd;struct _dyneither_ptr _tmp31D;struct _dyneither_ptr _tmp31F;_LL1FA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp31B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp31A;if(_tmp31B->tag != 8)goto _LL1FC;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp31C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp31B->f2);if(_tmp31C->tag != 6)goto _LL1FC;else{_tmp31D=_tmp31C->f2;}}}_LL1FB:
# 1363
 _tmp31F=_tmp31D;goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp31E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp31A;if(_tmp31E->tag != 6)goto _LL1FE;else{_tmp31F=_tmp31E->f2;}}_LL1FD: {
# 1365
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1C3)).f1;
struct _tuple14 _tmp786;struct _tuple14 _tmp321=(_tmp786.f1=_tmp30E,((_tmp786.f2=*((void**)_check_dyneither_subscript(_tmp31F,sizeof(void*),(int)i)),_tmp786)));_npop_handler(0);return _tmp321;}_LL1FE:;_LL1FF: {
struct Cyc_Core_Impossible_exn_struct _tmp78C;const char*_tmp78B;struct Cyc_Core_Impossible_exn_struct*_tmp78A;(int)_throw((void*)((_tmp78A=_cycalloc(sizeof(*_tmp78A)),((_tmp78A[0]=((_tmp78C.tag=Cyc_Core_Impossible,((_tmp78C.f1=((_tmp78B="anal_Rexp: Subscript",_tag_dyneither(_tmp78B,sizeof(char),21))),_tmp78C)))),_tmp78A)))));}_LL1F9:;}_LL1F5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315;if(_tmp318->tag != 5)goto _LL1F7;else{_tmp319=((_tmp318->f1).ptr_atts).bounds;}}_LL1F6: {
# 1370
union Cyc_CfFlowInfo_FlowInfo _tmp326;void*_tmp327;struct _tuple14 _tmp325=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp30C,_tmp1C2,(void*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd);_tmp326=_tmp325.f1;_tmp327=_tmp325.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp328=_tmp326;_LL201: if((_tmp328.BottomFL).tag != 1)goto _LL203;_LL202: {
struct _tuple14 _tmp78D;struct _tuple14 _tmp32A=(_tmp78D.f1=_tmp326,((_tmp78D.f2=_tmp327,_tmp78D)));_npop_handler(0);return _tmp32A;}_LL203:;_LL204: {
struct _tuple14 _tmp78E;struct _tuple14 _tmp32C=(_tmp78E.f1=_tmp30E,((_tmp78E.f2=_tmp327,_tmp78E)));_npop_handler(0);return _tmp32C;}_LL200:;};}_LL1F7:;_LL1F8: {
# 1375
struct Cyc_Core_Impossible_exn_struct _tmp794;const char*_tmp793;struct Cyc_Core_Impossible_exn_struct*_tmp792;(int)_throw((void*)((_tmp792=_cycalloc(sizeof(*_tmp792)),((_tmp792[0]=((_tmp794.tag=Cyc_Core_Impossible,((_tmp794.f1=((_tmp793="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp793,sizeof(char),33))),_tmp794)))),_tmp792)))));}_LL1F2:;};};}
# 1342
;_pop_region(temp);}_LL15C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1C4->tag != 30)goto _LL15E;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}_LL15D:
# 1380
 _tmp1C8=_tmp1C5;goto _LL15F;_LL15E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1C7->tag != 23)goto _LL160;else{_tmp1C8=_tmp1C7->f1;}}_LL15F: {
# 1382
struct _RegionHandle _tmp330=_new_region("temp");struct _RegionHandle*temp=& _tmp330;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp332;struct Cyc_List_List*_tmp333;struct _tuple20 _tmp331=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1C8,1,1);_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{
unsigned int _tmp79A;unsigned int _tmp799;struct _dyneither_ptr _tmp798;void**_tmp797;unsigned int _tmp796;struct _dyneither_ptr aggrdict=
(_tmp796=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C8),((_tmp797=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp796)),((_tmp798=_tag_dyneither(_tmp797,sizeof(void*),_tmp796),((((_tmp799=_tmp796,_tmp79B(& _tmp79A,& _tmp799,& _tmp797,& _tmp333))),_tmp798)))))));
# 1390
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7A5;struct Cyc_CfFlowInfo_UnionRInfo _tmp7A4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7A3;struct _tuple14 _tmp7A2;struct _tuple14 _tmp338=(_tmp7A2.f1=_tmp332,((_tmp7A2.f2=(void*)((_tmp7A5=_region_malloc(env->r,sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A3.tag=6,((_tmp7A3.f1=((_tmp7A4.is_union=0,((_tmp7A4.fieldnum=- 1,_tmp7A4)))),((_tmp7A3.f2=aggrdict,_tmp7A3)))))),_tmp7A5)))),_tmp7A2)));_npop_handler(0);return _tmp338;};}
# 1383
;_pop_region(temp);}_LL160: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1C9->tag != 29)goto _LL162;else{_tmp1CA=_tmp1C9->f2;}}_LL161: {
# 1393
struct Cyc_List_List*fs;
{void*_tmp33E=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp340;_LL206: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 12)goto _LL208;else{_tmp340=_tmp33F->f2;}}_LL207:
 fs=_tmp340;goto _LL205;_LL208:;_LL209: {
struct Cyc_Core_Impossible_exn_struct _tmp7AB;const char*_tmp7AA;struct Cyc_Core_Impossible_exn_struct*_tmp7A9;(int)_throw((void*)((_tmp7A9=_cycalloc(sizeof(*_tmp7A9)),((_tmp7A9[0]=((_tmp7AB.tag=Cyc_Core_Impossible,((_tmp7AB.f1=((_tmp7AA="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7AA,sizeof(char),35))),_tmp7AB)))),_tmp7A9)))));}_LL205:;}
# 1398
_tmp1CC=_tmp1CA;_tmp1CD=Cyc_Absyn_StructA;_tmp1CE=fs;goto _LL163;}_LL162: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1CB->tag != 28)goto _LL164;else{_tmp1CC=_tmp1CB->f3;if(_tmp1CB->f4 == 0)goto _LL164;_tmp1CD=(_tmp1CB->f4)->kind;if((_tmp1CB->f4)->impl == 0)goto _LL164;_tmp1CE=((_tmp1CB->f4)->impl)->fields;}}_LL163: {
# 1400
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp344=_new_region("temp");struct _RegionHandle*temp=& _tmp344;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp346;struct Cyc_List_List*_tmp347;struct _tuple20 _tmp345=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1CC),1,1);_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp156,_tmp1CE,_tmp1CD == Cyc_Absyn_UnionA,_tmp156->unknown_all);
# 1407
int field_no=-1;
{int i=0;for(0;_tmp347 != 0;(((_tmp347=_tmp347->tl,_tmp1CC=_tmp1CC->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp348=(void*)ds->hd;struct _dyneither_ptr*_tmp34B;_LL20B: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp349=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp348;if(_tmp349->tag != 0)goto _LL20D;}_LL20C: {
struct Cyc_Core_Impossible_exn_struct _tmp7B1;const char*_tmp7B0;struct Cyc_Core_Impossible_exn_struct*_tmp7AF;(int)_throw((void*)((_tmp7AF=_cycalloc(sizeof(*_tmp7AF)),((_tmp7AF[0]=((_tmp7B1.tag=Cyc_Core_Impossible,((_tmp7B1.f1=((_tmp7B0="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7B0,sizeof(char),22))),_tmp7B1)))),_tmp7AF)))));}_LL20D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp34A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp348;if(_tmp34A->tag != 1)goto _LL20A;else{_tmp34B=_tmp34A->f1;}}_LL20E:
# 1414
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1CE,_tmp34B);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp347->hd;
# 1417
if(_tmp1CD == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp34F=(*((struct _tuple22*)_tmp1CC->hd)).f2;
_tmp346=Cyc_NewControlFlow_use_Rval(env,_tmp34F->loc,_tmp346,(void*)_tmp347->hd);
# 1421
Cyc_NewControlFlow_check_requires(_tmp34F->loc,_tmp156->r,exp_type,_tmp34B,_tmp346);}_LL20A:;}}}{
# 1424
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7B7;struct Cyc_CfFlowInfo_UnionRInfo _tmp7B6;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7B5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp350=(_tmp7B5=_region_malloc(env->r,sizeof(*_tmp7B5)),((_tmp7B5[0]=((_tmp7B7.tag=6,((_tmp7B7.f1=((_tmp7B6.is_union=_tmp1CD == Cyc_Absyn_UnionA,((_tmp7B6.fieldnum=field_no,_tmp7B6)))),((_tmp7B7.f2=aggrdict,_tmp7B7)))))),_tmp7B5)));
struct _tuple14 _tmp7B8;struct _tuple14 _tmp352=(_tmp7B8.f1=_tmp346,((_tmp7B8.f2=(void*)_tmp350,_tmp7B8)));_npop_handler(0);return _tmp352;};};}
# 1402
;_pop_region(temp);}_LL164: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1CF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1CF->tag != 28)goto _LL166;}_LL165: {
# 1428
struct Cyc_Core_Impossible_exn_struct _tmp7BE;const char*_tmp7BD;struct Cyc_Core_Impossible_exn_struct*_tmp7BC;(int)_throw((void*)((_tmp7BC=_cycalloc(sizeof(*_tmp7BC)),((_tmp7BC[0]=((_tmp7BE.tag=Cyc_Core_Impossible,((_tmp7BE.f1=((_tmp7BD="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7BD,sizeof(char),31))),_tmp7BE)))),_tmp7BC)))));}_LL166: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1D0->tag != 25)goto _LL168;else{_tmp1D1=_tmp1D0->f1;}}_LL167: {
# 1430
struct _RegionHandle _tmp359=_new_region("temp");struct _RegionHandle*temp=& _tmp359;_push_region(temp);
{struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1D1);
union Cyc_CfFlowInfo_FlowInfo _tmp35C;struct Cyc_List_List*_tmp35D;struct _tuple20 _tmp35B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp35A,1,1);_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;
for(0;_tmp35D != 0;(_tmp35D=_tmp35D->tl,_tmp35A=_tmp35A->tl)){
_tmp35C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35A))->hd)->loc,_tmp35C,(void*)_tmp35D->hd);}{
struct _tuple14 _tmp7BF;struct _tuple14 _tmp35F=(_tmp7BF.f1=_tmp35C,((_tmp7BF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,(void*)_check_null(e->topt),0,_tmp156->unknown_all),_tmp7BF)));_npop_handler(0);return _tmp35F;};}
# 1431
;_pop_region(temp);}_LL168: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1D2->tag != 26)goto _LL16A;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;_tmp1D5=_tmp1D2->f3;_tmp1D6=_tmp1D2->f4;}}_LL169: {
# 1439
union Cyc_CfFlowInfo_FlowInfo _tmp361;void*_tmp362;struct _tuple14 _tmp360=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D4);_tmp361=_tmp360.f1;_tmp362=_tmp360.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp363=_tmp361;struct Cyc_Dict_Dict _tmp364;struct Cyc_List_List*_tmp365;_LL210: if((_tmp363.BottomFL).tag != 1)goto _LL212;_LL211: {
struct _tuple14 _tmp7C0;return(_tmp7C0.f1=_tmp361,((_tmp7C0.f2=_tmp156->unknown_all,_tmp7C0)));}_LL212: if((_tmp363.ReachableFL).tag != 2)goto _LL20F;_tmp364=((struct _tuple13)(_tmp363.ReachableFL).val).f1;_tmp365=((struct _tuple13)(_tmp363.ReachableFL).val).f2;_LL213:
# 1443
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp364,_tmp362)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_Tcutil_terr(_tmp1D4->loc,((_tmp7C3="expression may not be initialized",_tag_dyneither(_tmp7C3,sizeof(char),34))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}{
# 1447
struct Cyc_List_List*new_relns=_tmp365;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1D3);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1D4,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp365);
# 1453
if(_tmp365 != new_relns)
_tmp361=Cyc_CfFlowInfo_ReachableFL(_tmp364,new_relns);{
# 1457
void*_tmp369=_tmp362;_LL215: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp36A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp369;if(_tmp36A->tag != 0)goto _LL217;}_LL216: {
struct _tuple14 _tmp7C4;return(_tmp7C4.f1=_tmp361,((_tmp7C4.f2=_tmp156->unknown_all,_tmp7C4)));}_LL217: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp36B=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp369;if(_tmp36B->tag != 2)goto _LL219;}_LL218:
 goto _LL21A;_LL219: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp36C=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp369;if(_tmp36C->tag != 1)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp36D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp369;if(_tmp36D->tag != 5)goto _LL21D;}_LL21C: {
# 1462
struct Cyc_List_List _tmp7C5;struct Cyc_List_List _tmp36F=(_tmp7C5.hd=_tmp1D3,((_tmp7C5.tl=0,_tmp7C5)));
_tmp361=Cyc_NewControlFlow_add_vars(_tmp156,_tmp361,& _tmp36F,_tmp156->unknown_all,e->loc,0);{
# 1465
union Cyc_CfFlowInfo_FlowInfo _tmp371;void*_tmp372;struct _tuple14 _tmp370=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp361,_tmp1D5);_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp373=_tmp371;struct Cyc_Dict_Dict _tmp374;_LL220: if((_tmp373.BottomFL).tag != 1)goto _LL222;_LL221: {
struct _tuple14 _tmp7C6;return(_tmp7C6.f1=_tmp371,((_tmp7C6.f2=_tmp156->unknown_all,_tmp7C6)));}_LL222: if((_tmp373.ReachableFL).tag != 2)goto _LL21F;_tmp374=((struct _tuple13)(_tmp373.ReachableFL).val).f1;_LL223:
# 1469
 if(Cyc_CfFlowInfo_initlevel(_tmp156,_tmp374,_tmp372)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7C9;void*_tmp7C8;(_tmp7C8=0,Cyc_Tcutil_terr(_tmp1D4->loc,((_tmp7C9="expression may not be initialized",_tag_dyneither(_tmp7C9,sizeof(char),34))),_tag_dyneither(_tmp7C8,sizeof(void*),0)));}{
struct _tuple14 _tmp7CA;return(_tmp7CA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CA.f2=_tmp156->unknown_all,_tmp7CA)));};}_LL21F:;}
# 1474
_tmp361=_tmp371;
goto _LL21E;};}_LL21D:;_LL21E:
# 1477
 while(1){
struct Cyc_List_List _tmp7CB;struct Cyc_List_List _tmp37A=(_tmp7CB.hd=_tmp1D3,((_tmp7CB.tl=0,_tmp7CB)));
_tmp361=Cyc_NewControlFlow_add_vars(_tmp156,_tmp361,& _tmp37A,_tmp156->unknown_all,e->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp37C;void*_tmp37D;struct _tuple14 _tmp37B=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp361,_tmp1D5);_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp37E=_tmp37C;struct Cyc_Dict_Dict _tmp37F;_LL225: if((_tmp37E.BottomFL).tag != 1)goto _LL227;_LL226:
 goto _LL224;_LL227: if((_tmp37E.ReachableFL).tag != 2)goto _LL224;_tmp37F=((struct _tuple13)(_tmp37E.ReachableFL).val).f1;_LL228:
# 1484
 if(Cyc_CfFlowInfo_initlevel(_tmp156,_tmp37F,_tmp37D)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7CE;void*_tmp7CD;(_tmp7CD=0,Cyc_Tcutil_terr(_tmp1D4->loc,((_tmp7CE="expression may not be initialized",_tag_dyneither(_tmp7CE,sizeof(char),34))),_tag_dyneither(_tmp7CD,sizeof(void*),0)));}{
struct _tuple14 _tmp7CF;return(_tmp7CF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CF.f2=_tmp156->unknown_all,_tmp7CF)));};}_LL224:;}{
# 1489
union Cyc_CfFlowInfo_FlowInfo _tmp383=Cyc_CfFlowInfo_join_flow(_tmp156,env->all_changed,_tmp361,_tmp37C);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp383,_tmp361))
break;
_tmp361=_tmp383;};};}{
# 1494
struct _tuple14 _tmp7D0;return(_tmp7D0.f1=_tmp361,((_tmp7D0.f2=_tmp156->unknown_all,_tmp7D0)));};_LL214:;};};_LL20F:;};}_LL16A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1D7->tag != 27)goto _LL16C;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=(void*)_tmp1D7->f2;_tmp1DA=_tmp1D7->f3;}}_LL16B: {
# 1500
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7D3;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7D2;void*root=(void*)((_tmp7D2=_region_malloc(_tmp156->r,sizeof(*_tmp7D2)),((_tmp7D2[0]=((_tmp7D3.tag=1,((_tmp7D3.f1=_tmp1D8,((_tmp7D3.f2=(void*)_check_null(e->topt),_tmp7D3)))))),_tmp7D2))));
struct Cyc_CfFlowInfo_Place*_tmp7D4;struct Cyc_CfFlowInfo_Place*place=(_tmp7D4=_region_malloc(_tmp156->r,sizeof(*_tmp7D4)),((_tmp7D4->root=root,((_tmp7D4->fields=0,_tmp7D4)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7D7;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7D6;void*rval=(void*)((_tmp7D6=_region_malloc(_tmp156->r,sizeof(*_tmp7D6)),((_tmp7D6[0]=((_tmp7D7.tag=5,((_tmp7D7.f1=place,_tmp7D7)))),_tmp7D6))));
void*place_val;
# 1508
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp156,_tmp1D9,0,_tmp156->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp387;struct _tuple14 _tmp386=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D8);_tmp387=_tmp386.f1;
outflow=_tmp387;{
union Cyc_CfFlowInfo_FlowInfo _tmp388=outflow;struct Cyc_Dict_Dict _tmp389;struct Cyc_List_List*_tmp38A;_LL22A: if((_tmp388.BottomFL).tag != 1)goto _LL22C;_LL22B: {
struct _tuple14 _tmp7D8;return(_tmp7D8.f1=outflow,((_tmp7D8.f2=rval,_tmp7D8)));}_LL22C: if((_tmp388.ReachableFL).tag != 2)goto _LL229;_tmp389=((struct _tuple13)(_tmp388.ReachableFL).val).f1;_tmp38A=((struct _tuple13)(_tmp388.ReachableFL).val).f2;_LL22D: {
# 1516
struct _tuple14 _tmp7D9;return(_tmp7D9.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp389,root,place_val),_tmp38A),((_tmp7D9.f2=rval,_tmp7D9)));}_LL229:;};};};}_LL16C: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1DB=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1DB->tag != 36)goto _LL16E;else{_tmp1DC=_tmp1DB->f1;}}_LL16D:
# 1519
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1DC);_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1DD->tag != 1)goto _LL170;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1DE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1DD->f2);if(_tmp1DE->tag != 0)goto _LL170;}}_LL16F:
 goto _LL171;_LL170: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1DF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1DF->tag != 35)goto _LL172;}_LL171:
 goto _LL173;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1E0=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1E0->tag != 24)goto _LL174;}_LL173:
 goto _LL175;_LL174: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1E1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1E1->tag != 38)goto _LL176;}_LL175:
 goto _LL177;_LL176: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1E2=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp1E2->tag != 39)goto _LL10F;}_LL177: {
# 1525
struct Cyc_Core_Impossible_exn_struct _tmp7DF;const char*_tmp7DE;struct Cyc_Core_Impossible_exn_struct*_tmp7DD;(int)_throw((void*)((_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=Cyc_Core_Impossible,((_tmp7DF.f1=((_tmp7DE="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp7DE,sizeof(char),31))),_tmp7DF)))),_tmp7DD)))));}_LL10F:;};}
# 1534
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1541
struct Cyc_CfFlowInfo_FlowEnv*_tmp395=env->fenv;
void*_tmp396=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp398;union Cyc_Absyn_Constraint*_tmp399;union Cyc_Absyn_Constraint*_tmp39A;_LL22F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396;if(_tmp397->tag != 5)goto _LL231;else{_tmp398=(_tmp397->f1).elt_typ;_tmp399=((_tmp397->f1).ptr_atts).bounds;_tmp39A=((_tmp397->f1).ptr_atts).zero_term;}}_LL230: {
# 1544
union Cyc_CfFlowInfo_FlowInfo _tmp39B=f;struct Cyc_Dict_Dict _tmp39C;struct Cyc_List_List*_tmp39D;_LL234: if((_tmp39B.BottomFL).tag != 1)goto _LL236;_LL235: {
struct _tuple15 _tmp7E0;return(_tmp7E0.f1=f,((_tmp7E0.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7E0)));}_LL236: if((_tmp39B.ReachableFL).tag != 2)goto _LL233;_tmp39C=((struct _tuple13)(_tmp39B.ReachableFL).val).f1;_tmp39D=((struct _tuple13)(_tmp39B.ReachableFL).val).f2;_LL237: {
# 1548
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp399)){
retry: {void*_tmp39F=r;struct Cyc_Absyn_Vardecl*_tmp3A1;void*_tmp3A2;void*_tmp3A6;struct Cyc_List_List*_tmp3A7;enum Cyc_CfFlowInfo_InitLevel _tmp3AA;_LL239: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3A0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A0->tag != 8)goto _LL23B;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=(void*)_tmp3A0->f2;}}_LL23A:
 r=_tmp3A2;locname=_tmp3A1;goto retry;_LL23B: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3A3=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A3->tag != 1)goto _LL23D;}_LL23C:
 goto _LL23E;_LL23D: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3A4=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A4->tag != 2)goto _LL23F;}_LL23E:
# 1554
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7E3;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7E2;e->annot=(void*)((_tmp7E2=_cycalloc(sizeof(*_tmp7E2)),((_tmp7E2[0]=((_tmp7E3.tag=Cyc_CfFlowInfo_NotZero,((_tmp7E3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp39D),_tmp7E3)))),_tmp7E2))));}goto _LL238;_LL23F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3A5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A5->tag != 5)goto _LL241;else{_tmp3A6=(_tmp3A5->f1)->root;_tmp3A7=(_tmp3A5->f1)->fields;}}_LL240:
# 1556
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7E6;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7E5;e->annot=(void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E6.tag=Cyc_CfFlowInfo_NotZero,((_tmp7E6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp39D),_tmp7E6)))),_tmp7E5))));}{
struct Cyc_CfFlowInfo_Place*_tmp7E9;struct _tuple15 _tmp7E8;return(_tmp7E8.f1=f,((_tmp7E8.f2=Cyc_CfFlowInfo_PlaceL(((_tmp7E9=_region_malloc(_tmp395->r,sizeof(*_tmp7E9)),((_tmp7E9->root=_tmp3A6,((_tmp7E9->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp395->r,_tmp3A7,flds),_tmp7E9))))))),_tmp7E8)));};_LL241: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3A8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A8->tag != 0)goto _LL243;}_LL242:
# 1559
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp7EA;return(_tmp7EA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EA.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7EA)));};_LL243: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3A9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp39F;if(_tmp3A9->tag != 3)goto _LL245;else{_tmp3AA=_tmp3A9->f1;}}_LL244:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3AA,locname);goto _LL246;_LL245:;_LL246: {
# 1563
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7ED;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7EC;e->annot=(void*)((_tmp7EC=_cycalloc(sizeof(*_tmp7EC)),((_tmp7EC[0]=((_tmp7ED.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7ED.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp39D),_tmp7ED)))),_tmp7EC))));}_LL238:;}}else{
# 1566
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7F0;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7EF;e->annot=(void*)((_tmp7EF=_cycalloc(sizeof(*_tmp7EF)),((_tmp7EF[0]=((_tmp7F0.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7F0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp39D),_tmp7F0)))),_tmp7EF))));}
if(Cyc_CfFlowInfo_initlevel(_tmp395,_tmp39C,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_Tcutil_terr(e->loc,((_tmp7F3="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7F3,sizeof(char),46))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}{
struct _tuple15 _tmp7F4;return(_tmp7F4.f1=f,((_tmp7F4.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7F4)));};}_LL233:;}_LL231:;_LL232: {
# 1571
struct Cyc_Core_Impossible_exn_struct _tmp7FA;const char*_tmp7F9;struct Cyc_Core_Impossible_exn_struct*_tmp7F8;(int)_throw((void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7FA.tag=Cyc_Core_Impossible,((_tmp7FA.f1=((_tmp7F9="left deref of non-pointer-type",_tag_dyneither(_tmp7F9,sizeof(char),31))),_tmp7FA)))),_tmp7F8)))));}_LL22E:;}
# 1579
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1582
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3BC=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3BD=inflow;struct Cyc_Dict_Dict _tmp3BE;struct Cyc_List_List*_tmp3BF;_LL248: if((_tmp3BD.BottomFL).tag != 1)goto _LL24A;_LL249: {
struct _tuple15 _tmp7FB;return(_tmp7FB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7FB)));}_LL24A: if((_tmp3BD.ReachableFL).tag != 2)goto _LL247;_tmp3BE=((struct _tuple13)(_tmp3BD.ReachableFL).val).f1;_tmp3BF=((struct _tuple13)(_tmp3BD.ReachableFL).val).f2;_LL24B:
# 1587
 d=_tmp3BE;_LL247:;}{
# 1591
void*_tmp3C1=e->r;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CD;struct Cyc_Absyn_Vardecl*_tmp3D0;struct Cyc_Absyn_Vardecl*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D5;struct _dyneither_ptr*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;struct _dyneither_ptr*_tmp3DE;_LL24D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3C2->tag != 13)goto _LL24F;else{_tmp3C3=_tmp3C2->f2;}}_LL24E:
 _tmp3C5=_tmp3C3;goto _LL250;_LL24F: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3C4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3C4->tag != 11)goto _LL251;else{_tmp3C5=_tmp3C4->f1;}}_LL250:
 _tmp3C7=_tmp3C5;goto _LL252;_LL251: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3C6->tag != 12)goto _LL253;else{_tmp3C7=_tmp3C6->f1;}}_LL252:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3C7,flds);_LL253: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3C8->tag != 1)goto _LL255;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3C9=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3C8->f2);if(_tmp3C9->tag != 3)goto _LL255;else{_tmp3CA=_tmp3C9->f1;}}}_LL254:
# 1596
 _tmp3CD=_tmp3CA;goto _LL256;_LL255: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3CB->tag != 1)goto _LL257;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3CC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3CB->f2);if(_tmp3CC->tag != 4)goto _LL257;else{_tmp3CD=_tmp3CC->f1;}}}_LL256:
 _tmp3D0=_tmp3CD;goto _LL258;_LL257: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3CE->tag != 1)goto _LL259;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3CF=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3CE->f2);if(_tmp3CF->tag != 5)goto _LL259;else{_tmp3D0=_tmp3CF->f1;}}}_LL258: {
# 1599
struct Cyc_CfFlowInfo_Place*_tmp805;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp804;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp803;struct _tuple15 _tmp802;return(_tmp802.f1=inflow,((_tmp802.f2=Cyc_CfFlowInfo_PlaceL(((_tmp805=_region_malloc(env->r,sizeof(*_tmp805)),((_tmp805->root=(void*)((_tmp803=_region_malloc(env->r,sizeof(*_tmp803)),((_tmp803[0]=((_tmp804.tag=0,((_tmp804.f1=_tmp3D0,_tmp804)))),_tmp803)))),((_tmp805->fields=flds,_tmp805))))))),_tmp802)));}_LL259: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3D1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3D1->tag != 1)goto _LL25B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3D2=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3D1->f2);if(_tmp3D2->tag != 1)goto _LL25B;else{_tmp3D3=_tmp3D2->f1;}}}_LL25A: {
# 1601
struct _tuple15 _tmp806;return(_tmp806.f1=inflow,((_tmp806.f2=Cyc_CfFlowInfo_UnknownL(),_tmp806)));}_LL25B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3D4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3D4->tag != 21)goto _LL25D;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;}}_LL25C:
# 1604
{void*_tmp3E4=Cyc_Tcutil_compress((void*)_check_null(_tmp3D5->topt));void*_tmp3E6;_LL266: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E4;if(_tmp3E5->tag != 5)goto _LL268;else{_tmp3E6=(_tmp3E5->f1).elt_typ;}}_LL267:
# 1606
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3E6)){
Cyc_NewControlFlow_check_requires(_tmp3D5->loc,_tmp3BC->r,_tmp3E6,_tmp3D6,inflow);{
struct Cyc_List_List*_tmp807;flds=((_tmp807=_region_malloc(env->r,sizeof(*_tmp807)),((_tmp807->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3E6,_tmp3D6),((_tmp807->tl=flds,_tmp807))))));};}
# 1610
goto _LL265;_LL268:;_LL269: {
struct Cyc_Core_Impossible_exn_struct _tmp80D;const char*_tmp80C;struct Cyc_Core_Impossible_exn_struct*_tmp80B;(int)_throw((void*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B[0]=((_tmp80D.tag=Cyc_Core_Impossible,((_tmp80D.f1=((_tmp80C="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp80C,sizeof(char),25))),_tmp80D)))),_tmp80B)))));}_LL265:;}
# 1613
_tmp3D8=_tmp3D5;goto _LL25E;_LL25D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3D7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3D7->tag != 19)goto _LL25F;else{_tmp3D8=_tmp3D7->f1;}}_LL25E:
# 1615
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3D8->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp3EC;union Cyc_CfFlowInfo_AbsLVal _tmp3ED;struct _tuple15 _tmp3EB=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3D8);_tmp3EC=_tmp3EB.f1;_tmp3ED=_tmp3EB.f2;{
struct _tuple15 _tmp80E;struct _tuple15 _tmp3EF=(_tmp80E.f1=_tmp3EC,((_tmp80E.f2=_tmp3ED,_tmp80E)));struct Cyc_Dict_Dict _tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_CfFlowInfo_Place*_tmp3F2;_LL26B: if(((_tmp3EF.f1).ReachableFL).tag != 2)goto _LL26D;_tmp3F0=((struct _tuple13)((_tmp3EF.f1).ReachableFL).val).f1;_tmp3F1=((struct _tuple13)((_tmp3EF.f1).ReachableFL).val).f2;if(((_tmp3EF.f2).PlaceL).tag != 1)goto _LL26D;_tmp3F2=(struct Cyc_CfFlowInfo_Place*)((_tmp3EF.f2).PlaceL).val;_LL26C: {
# 1619
void*_tmp3F3=Cyc_CfFlowInfo_lookup_place(_tmp3F0,_tmp3F2);
struct Cyc_Absyn_Vardecl*locname=0;
void*_tmp3F4=_tmp3F3;struct Cyc_Absyn_Vardecl*_tmp3F7;void*_tmp3F8;_LL270: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3F5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3F4;if(_tmp3F5->tag != 5)goto _LL272;}_LL271:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3EC,_tmp3D8,_tmp3EC,_tmp3F3,flds);_LL272: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3F6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3F4;if(_tmp3F6->tag != 8)goto _LL274;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=(void*)_tmp3F6->f2;}}_LL273:
 _tmp3F3=_tmp3F8;locname=_tmp3F7;goto _LL275;_LL274:;_LL275: {
# 1630
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3BC,_tmp3F0,_tmp3F3);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3BC->unknown_all: _tmp3BC->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3BC,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3D8->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp811;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp810;void*new_root=(void*)((_tmp810=_region_malloc(_tmp3BC->r,sizeof(*_tmp810)),((_tmp810[0]=((_tmp811.tag=1,((_tmp811.f1=e,((_tmp811.f2=(void*)_check_null(e->topt),_tmp811)))))),_tmp810))));
struct Cyc_CfFlowInfo_Place*_tmp812;struct Cyc_CfFlowInfo_Place*place=(_tmp812=_region_malloc(_tmp3BC->r,sizeof(*_tmp812)),((_tmp812->root=new_root,((_tmp812->fields=0,_tmp812)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp815;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp814;void*res=(void*)((_tmp814=_region_malloc(_tmp3BC->r,sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=5,((_tmp815.f1=place,_tmp815)))),_tmp814))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp818;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp817;res=(void*)((_tmp817=_region_malloc(_tmp3BC->r,sizeof(*_tmp817)),((_tmp817[0]=((_tmp818.tag=8,((_tmp818.f1=locname,((_tmp818.f2=res,_tmp818)))))),_tmp817))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3F0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3F0,new_root,new_rval);
_tmp3F0=Cyc_CfFlowInfo_assign_place(_tmp3BC,e->loc,_tmp3F0,env->all_changed,_tmp3F2,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3FB=Cyc_CfFlowInfo_ReachableFL(_tmp3F0,_tmp3F1);
# 1645
return Cyc_NewControlFlow_anal_derefL(env,_tmp3FB,_tmp3D8,_tmp3FB,res,flds);};}_LL26F:;}_LL26D:;_LL26E:
# 1647
 goto _LL26A;_LL26A:;};}{
# 1650
union Cyc_CfFlowInfo_FlowInfo _tmp402;void*_tmp403;struct _tuple14 _tmp401=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3D8);_tmp402=_tmp401.f1;_tmp403=_tmp401.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3D8,_tmp402,_tmp403,flds);};_LL25F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3D9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3D9->tag != 22)goto _LL261;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL260: {
# 1654
void*_tmp404=Cyc_Tcutil_compress((void*)_check_null(_tmp3DA->topt));union Cyc_Absyn_Constraint*_tmp407;_LL277: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404;if(_tmp405->tag != 10)goto _LL279;}_LL278: {
# 1656
unsigned int _tmp408=(Cyc_Evexp_eval_const_uint_exp(_tmp3DB)).f1;
struct Cyc_List_List*_tmp819;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3DA,((_tmp819=_region_malloc(env->r,sizeof(*_tmp819)),((_tmp819->hd=(void*)((int)_tmp408),((_tmp819->tl=flds,_tmp819)))))));}_LL279: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404;if(_tmp406->tag != 5)goto _LL27B;else{_tmp407=((_tmp406->f1).ptr_atts).bounds;}}_LL27A: {
# 1659
struct _RegionHandle _tmp40A=_new_region("temp");struct _RegionHandle*temp=& _tmp40A;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp40D;struct Cyc_List_List*_tmp40E;struct Cyc_Absyn_Exp*_tmp81A[2];struct _tuple20 _tmp40C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp81A[1]=_tmp3DB,((_tmp81A[0]=_tmp3DA,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp81A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp40F=_tmp40D;
{union Cyc_CfFlowInfo_FlowInfo _tmp410=_tmp40D;struct Cyc_Dict_Dict _tmp411;struct Cyc_List_List*_tmp412;_LL27E: if((_tmp410.ReachableFL).tag != 2)goto _LL280;_tmp411=((struct _tuple13)(_tmp410.ReachableFL).val).f1;_tmp412=((struct _tuple13)(_tmp410.ReachableFL).val).f2;_LL27F:
# 1665
 if(Cyc_CfFlowInfo_initlevel(_tmp3BC,_tmp411,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp40E))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(_tmp3DB->loc,((_tmp81D="expression may not be initialized",_tag_dyneither(_tmp81D,sizeof(char),34))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp415=Cyc_NewControlFlow_add_subscript_reln(_tmp3BC->r,_tmp412,_tmp3DA,_tmp3DB);
if(_tmp412 != _tmp415)
_tmp40F=Cyc_CfFlowInfo_ReachableFL(_tmp411,_tmp415);
goto _LL27D;};_LL280:;_LL281:
 goto _LL27D;_LL27D:;}{
# 1673
union Cyc_CfFlowInfo_FlowInfo _tmp417;union Cyc_CfFlowInfo_AbsLVal _tmp418;struct _tuple15 _tmp416=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3DA,_tmp40D,(void*)((struct Cyc_List_List*)_check_null(_tmp40E))->hd,flds);_tmp417=_tmp416.f1;_tmp418=_tmp416.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp419=_tmp417;_LL283: if((_tmp419.BottomFL).tag != 1)goto _LL285;_LL284: {
struct _tuple15 _tmp81E;struct _tuple15 _tmp41B=(_tmp81E.f1=_tmp417,((_tmp81E.f2=_tmp418,_tmp81E)));_npop_handler(0);return _tmp41B;}_LL285:;_LL286: {
struct _tuple15 _tmp81F;struct _tuple15 _tmp41D=(_tmp81F.f1=_tmp40F,((_tmp81F.f2=_tmp418,_tmp81F)));_npop_handler(0);return _tmp41D;}_LL282:;};};};}
# 1660
;_pop_region(temp);}_LL27B:;_LL27C: {
# 1679
struct Cyc_Core_Impossible_exn_struct _tmp825;const char*_tmp824;struct Cyc_Core_Impossible_exn_struct*_tmp823;(int)_throw((void*)((_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((_tmp825.tag=Cyc_Core_Impossible,((_tmp825.f1=((_tmp824="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp824,sizeof(char),33))),_tmp825)))),_tmp823)))));}_LL276:;}_LL261: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3DC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C1;if(_tmp3DC->tag != 20)goto _LL263;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DC->f2;}}_LL262: {
# 1683
void*_tmp421=(void*)_check_null(_tmp3DD->topt);
if(Cyc_Absyn_is_require_union_type(_tmp421))
Cyc_NewControlFlow_check_requires(_tmp3DD->loc,_tmp3BC->r,_tmp421,_tmp3DE,inflow);
# 1687
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp421)){
struct _tuple15 _tmp826;return(_tmp826.f1=inflow,((_tmp826.f2=Cyc_CfFlowInfo_UnknownL(),_tmp826)));}{
# 1690
struct Cyc_List_List*_tmp827;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3DD,(
(_tmp827=_region_malloc(env->r,sizeof(*_tmp827)),((_tmp827->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp421,_tmp3DE),((_tmp827->tl=flds,_tmp827)))))));};}_LL263:;_LL264: {
# 1693
struct _tuple15 _tmp828;return(_tmp828.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp828.f2=Cyc_CfFlowInfo_UnknownL(),_tmp828)));}_LL24C:;};}
# 1697
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1699
union Cyc_CfFlowInfo_FlowInfo _tmp426;union Cyc_CfFlowInfo_AbsLVal _tmp427;struct _tuple15 _tmp425=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp426=_tmp425.f1;_tmp427=_tmp425.f2;{
struct _tuple15 _tmp829;return(_tmp829.f1=_tmp426,((_tmp829.f2=_tmp427,_tmp829)));};}
# 1706
static struct _tuple16 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1709
struct Cyc_CfFlowInfo_FlowEnv*_tmp429=env->fenv;
# 1712
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp42A=_new_region("temp");struct _RegionHandle*temp=& _tmp42A;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp42C;struct Cyc_List_List*_tmp42D;struct _tuple20 _tmp42B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);_tmp42C=_tmp42B.f1;_tmp42D=_tmp42B.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp42D))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp42D->tl))->hd;
f=_tmp42C;}{
# 1722
union Cyc_CfFlowInfo_FlowInfo _tmp42E=f;struct Cyc_Dict_Dict _tmp42F;struct Cyc_List_List*_tmp430;_LL288: if((_tmp42E.BottomFL).tag != 1)goto _LL28A;_LL289: {
struct _tuple16 _tmp82A;struct _tuple16 _tmp432=(_tmp82A.f1=f,((_tmp82A.f2=f,_tmp82A)));_npop_handler(0);return _tmp432;}_LL28A: if((_tmp42E.ReachableFL).tag != 2)goto _LL287;_tmp42F=((struct _tuple13)(_tmp42E.ReachableFL).val).f1;_tmp430=((struct _tuple13)(_tmp42E.ReachableFL).val).f2;_LL28B: {
# 1725
struct Cyc_Absyn_Exp*_tmp433=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp434=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42F,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp82D;void*_tmp82C;(_tmp82C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp82D="expression may not be initialized",_tag_dyneither(_tmp82D,sizeof(char),34))),_tag_dyneither(_tmp82C,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42F,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp830;void*_tmp82F;(_tmp82F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp830="expression may not be initialized",_tag_dyneither(_tmp830,sizeof(char),34))),_tag_dyneither(_tmp82F,sizeof(void*),0)));}{
# 1732
union Cyc_CfFlowInfo_FlowInfo _tmp439=f;
union Cyc_CfFlowInfo_FlowInfo _tmp43A=f;
# 1737
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp43B=r1;struct Cyc_Absyn_Vardecl*_tmp43D;void*_tmp43E;_LL28D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp43C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp43B;if(_tmp43C->tag != 8)goto _LL28F;else{_tmp43D=_tmp43C->f1;_tmp43E=(void*)_tmp43C->f2;}}_LL28E:
 r1=_tmp43E;r1n=_tmp43D;goto _LL28C;_LL28F:;_LL290:
 goto _LL28C;_LL28C:;}
# 1743
{void*_tmp43F=r2;struct Cyc_Absyn_Vardecl*_tmp441;void*_tmp442;_LL292: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp43F;if(_tmp440->tag != 8)goto _LL294;else{_tmp441=_tmp440->f1;_tmp442=(void*)_tmp440->f2;}}_LL293:
 r2=_tmp442;r2n=_tmp441;goto _LL291;_LL294:;_LL295:
 goto _LL291;_LL291:;}{
# 1748
struct _tuple0 _tmp831;struct _tuple0 _tmp444=(_tmp831.f1=r1,((_tmp831.f2=r2,_tmp831)));enum Cyc_CfFlowInfo_InitLevel _tmp446;enum Cyc_CfFlowInfo_InitLevel _tmp44A;_LL297:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp445->tag != 3)goto _LL299;else{_tmp446=_tmp445->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp447->tag != 0)goto _LL299;};_LL298: {
# 1752
union Cyc_CfFlowInfo_FlowInfo _tmp45A;union Cyc_CfFlowInfo_FlowInfo _tmp45B;struct _tuple16 _tmp459=Cyc_NewControlFlow_splitzero(env,f,f,_tmp433,_tmp446,r1n);_tmp45A=_tmp459.f1;_tmp45B=_tmp459.f2;
switch(p){case Cyc_Absyn_Eq: _LL2AB:
 _tmp439=_tmp45B;_tmp43A=_tmp45A;break;case Cyc_Absyn_Neq: _LL2AC:
 _tmp439=_tmp45A;_tmp43A=_tmp45B;break;default: _LL2AD: {
struct Cyc_Core_Impossible_exn_struct _tmp837;const char*_tmp836;struct Cyc_Core_Impossible_exn_struct*_tmp835;(int)_throw((void*)((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835[0]=((_tmp837.tag=Cyc_Core_Impossible,((_tmp837.f1=((_tmp836="anal_test, zero-split",_tag_dyneither(_tmp836,sizeof(char),22))),_tmp837)))),_tmp835)))));}}
# 1758
goto _LL296;}_LL299:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp448=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp448->tag != 0)goto _LL29B;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp449=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp449->tag != 3)goto _LL29B;else{_tmp44A=_tmp449->f1;}};_LL29A: {
# 1760
union Cyc_CfFlowInfo_FlowInfo _tmp460;union Cyc_CfFlowInfo_FlowInfo _tmp461;struct _tuple16 _tmp45F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp434,_tmp44A,r2n);_tmp460=_tmp45F.f1;_tmp461=_tmp45F.f2;
switch(p){case Cyc_Absyn_Eq: _LL2AF:
 _tmp439=_tmp461;_tmp43A=_tmp460;break;case Cyc_Absyn_Neq: _LL2B0:
 _tmp439=_tmp460;_tmp43A=_tmp461;break;default: _LL2B1: {
struct Cyc_Core_Impossible_exn_struct _tmp83D;const char*_tmp83C;struct Cyc_Core_Impossible_exn_struct*_tmp83B;(int)_throw((void*)((_tmp83B=_cycalloc(sizeof(*_tmp83B)),((_tmp83B[0]=((_tmp83D.tag=Cyc_Core_Impossible,((_tmp83D.f1=((_tmp83C="anal_test, zero-split",_tag_dyneither(_tmp83C,sizeof(char),22))),_tmp83D)))),_tmp83B)))));}}
# 1766
goto _LL296;}_LL29B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp44B->tag != 0)goto _LL29D;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp44C->tag != 0)goto _LL29D;};_LL29C:
# 1768
 if(p == Cyc_Absyn_Eq)_tmp43A=Cyc_CfFlowInfo_BottomFL();else{
_tmp439=Cyc_CfFlowInfo_BottomFL();}
goto _LL296;_LL29D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp44D->tag != 0)goto _LL29F;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp44E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp44E->tag != 1)goto _LL29F;};_LL29E:
 goto _LL2A0;_LL29F:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp44F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp44F->tag != 0)goto _LL2A1;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp450=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp450->tag != 2)goto _LL2A1;};_LL2A0:
 goto _LL2A2;_LL2A1:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp451=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp451->tag != 0)goto _LL2A3;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp452=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp452->tag != 5)goto _LL2A3;};_LL2A2:
 goto _LL2A4;_LL2A3:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp453=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp453->tag != 1)goto _LL2A5;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp454=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp454->tag != 0)goto _LL2A5;};_LL2A4:
 goto _LL2A6;_LL2A5:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp455=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp455->tag != 2)goto _LL2A7;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp456=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp456->tag != 0)goto _LL2A7;};_LL2A6:
 goto _LL2A8;_LL2A7:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp457=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp444.f1;if(_tmp457->tag != 5)goto _LL2A9;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp458=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp444.f2;if(_tmp458->tag != 0)goto _LL2A9;};_LL2A8:
# 1777
 if(p == Cyc_Absyn_Neq)_tmp43A=Cyc_CfFlowInfo_BottomFL();else{
_tmp439=Cyc_CfFlowInfo_BottomFL();}
goto _LL296;_LL2A9:;_LL2AA:
 goto _LL296;_LL296:;};}
# 1788
{struct _tuple0 _tmp83E;struct _tuple0 _tmp466=(_tmp83E.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp433->topt)),((_tmp83E.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp434->topt)),_tmp83E)));_LL2B4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp466.f1;if(_tmp467->tag != 6)goto _LL2B6;else{if(_tmp467->f1 != Cyc_Absyn_Unsigned)goto _LL2B6;}}_LL2B5:
 goto _LL2B7;_LL2B6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp466.f2;if(_tmp468->tag != 6)goto _LL2B8;else{if(_tmp468->f1 != Cyc_Absyn_Unsigned)goto _LL2B8;}}_LL2B7:
 goto _LL2B9;_LL2B8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp466.f1;if(_tmp469->tag != 19)goto _LL2BA;}_LL2B9:
 goto _LL2BB;_LL2BA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp466.f2;if(_tmp46A->tag != 19)goto _LL2BC;}_LL2BB:
 goto _LL2B3;_LL2BC:;_LL2BD: {
struct _tuple16 _tmp83F;struct _tuple16 _tmp46C=(_tmp83F.f1=_tmp439,((_tmp83F.f2=_tmp43A,_tmp83F)));_npop_handler(0);return _tmp46C;}_LL2B3:;}
# 1797
switch(p){case Cyc_Absyn_Eq: _LL2BE:
 goto _LL2BF;case Cyc_Absyn_Neq: _LL2BF: goto _LL2C0;case Cyc_Absyn_Gt: _LL2C0: goto _LL2C1;case Cyc_Absyn_Gte: _LL2C1: goto _LL2C2;case Cyc_Absyn_Lt: _LL2C2: goto _LL2C3;case Cyc_Absyn_Lte: _LL2C3: break;default: _LL2C4: {
struct _tuple16 _tmp840;struct _tuple16 _tmp46E=(_tmp840.f1=_tmp439,((_tmp840.f2=_tmp43A,_tmp840)));_npop_handler(0);return _tmp46E;}}{
# 1802
struct _RegionHandle*_tmp46F=(env->fenv)->r;
struct Cyc_Absyn_Exp*_tmp471;enum Cyc_Relations_Relation _tmp472;struct Cyc_Absyn_Exp*_tmp473;struct _tuple12 _tmp470=Cyc_Relations_primop2relation(_tmp433,p,_tmp434);_tmp471=_tmp470.f1;_tmp472=_tmp470.f2;_tmp473=_tmp470.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1807
if(Cyc_Relations_exp2relnop(_tmp471,& n1) && Cyc_Relations_exp2relnop(_tmp473,& n2)){
# 1809
struct Cyc_List_List*_tmp474=Cyc_Relations_add_relation(_tmp46F,n1,_tmp472,n2,_tmp430);
# 1813
struct Cyc_List_List*_tmp475=Cyc_Relations_add_relation(_tmp46F,n2,Cyc_Relations_flip_relation(_tmp472),n1,_tmp430);
struct _tuple16 _tmp841;struct _tuple16 _tmp477=(_tmp841.f1=_tmp439,((_tmp841.f2=_tmp43A,_tmp841)));struct Cyc_Dict_Dict _tmp478;struct Cyc_Dict_Dict _tmp479;struct Cyc_Dict_Dict _tmp47A;struct Cyc_Dict_Dict _tmp47B;_LL2C7: if(((_tmp477.f1).ReachableFL).tag != 2)goto _LL2C9;_tmp478=((struct _tuple13)((_tmp477.f1).ReachableFL).val).f1;if(((_tmp477.f2).ReachableFL).tag != 2)goto _LL2C9;_tmp479=((struct _tuple13)((_tmp477.f2).ReachableFL).val).f1;_LL2C8: {
# 1816
struct _tuple16 _tmp842;struct _tuple16 _tmp47D=(_tmp842.f1=Cyc_CfFlowInfo_ReachableFL(_tmp478,_tmp474),((_tmp842.f2=Cyc_CfFlowInfo_ReachableFL(_tmp479,_tmp475),_tmp842)));_npop_handler(0);return _tmp47D;}_LL2C9: if(((_tmp477.f2).ReachableFL).tag != 2)goto _LL2CB;_tmp47A=((struct _tuple13)((_tmp477.f2).ReachableFL).val).f1;_LL2CA: {
# 1818
struct _tuple16 _tmp843;struct _tuple16 _tmp47F=(_tmp843.f1=_tmp439,((_tmp843.f2=Cyc_CfFlowInfo_ReachableFL(_tmp47A,_tmp475),_tmp843)));_npop_handler(0);return _tmp47F;}_LL2CB: if(((_tmp477.f1).ReachableFL).tag != 2)goto _LL2CD;_tmp47B=((struct _tuple13)((_tmp477.f1).ReachableFL).val).f1;_LL2CC: {
# 1820
struct _tuple16 _tmp844;struct _tuple16 _tmp481=(_tmp844.f1=Cyc_CfFlowInfo_ReachableFL(_tmp47B,_tmp474),((_tmp844.f2=_tmp43A,_tmp844)));_npop_handler(0);return _tmp481;}_LL2CD:;_LL2CE: {
# 1822
struct _tuple16 _tmp845;struct _tuple16 _tmp483=(_tmp845.f1=_tmp439,((_tmp845.f2=_tmp43A,_tmp845)));_npop_handler(0);return _tmp483;}_LL2C6:;}else{
# 1825
struct _tuple16 _tmp846;struct _tuple16 _tmp485=(_tmp846.f1=_tmp439,((_tmp846.f2=_tmp43A,_tmp846)));_npop_handler(0);return _tmp485;}};};};}_LL287:;};
# 1714
;_pop_region(temp);}
# 1831
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1833
struct Cyc_CfFlowInfo_FlowEnv*_tmp486=env->fenv;
void*_tmp487=e->r;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp496;enum Cyc_Absyn_Primop _tmp498;struct Cyc_List_List*_tmp499;_LL2D0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp488=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp488->tag != 5)goto _LL2D2;else{_tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;_tmp48B=_tmp488->f3;}}_LL2D1: {
# 1836
union Cyc_CfFlowInfo_FlowInfo _tmp49B;union Cyc_CfFlowInfo_FlowInfo _tmp49C;struct _tuple16 _tmp49A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp489);_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple16 _tmp49D=Cyc_NewControlFlow_anal_test(env,_tmp49B,_tmp48A);_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A2;struct _tuple16 _tmp4A0=Cyc_NewControlFlow_anal_test(env,_tmp49C,_tmp48B);_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;{
struct _tuple16 _tmp847;return(_tmp847.f1=Cyc_CfFlowInfo_join_flow(_tmp486,env->all_changed,_tmp49E,_tmp4A1),((_tmp847.f2=
Cyc_CfFlowInfo_join_flow(_tmp486,env->all_changed,_tmp49F,_tmp4A2),_tmp847)));};};};}_LL2D2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp48C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp48C->tag != 6)goto _LL2D4;else{_tmp48D=_tmp48C->f1;_tmp48E=_tmp48C->f2;}}_LL2D3: {
# 1842
union Cyc_CfFlowInfo_FlowInfo _tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;struct _tuple16 _tmp4A4=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48D);_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4A8;union Cyc_CfFlowInfo_FlowInfo _tmp4A9;struct _tuple16 _tmp4A7=Cyc_NewControlFlow_anal_test(env,_tmp4A5,_tmp48E);_tmp4A8=_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;{
struct _tuple16 _tmp848;return(_tmp848.f1=_tmp4A8,((_tmp848.f2=Cyc_CfFlowInfo_join_flow(_tmp486,env->all_changed,_tmp4A6,_tmp4A9),_tmp848)));};};}_LL2D4: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp48F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp48F->tag != 7)goto _LL2D6;else{_tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;}}_LL2D5: {
# 1846
union Cyc_CfFlowInfo_FlowInfo _tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;struct _tuple16 _tmp4AB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp490);_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4AF;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;struct _tuple16 _tmp4AE=Cyc_NewControlFlow_anal_test(env,_tmp4AD,_tmp491);_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;{
struct _tuple16 _tmp849;return(_tmp849.f1=Cyc_CfFlowInfo_join_flow(_tmp486,env->all_changed,_tmp4AC,_tmp4AF),((_tmp849.f2=_tmp4B0,_tmp849)));};};}_LL2D6: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp492=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp492->tag != 8)goto _LL2D8;else{_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;}}_LL2D7: {
# 1850
union Cyc_CfFlowInfo_FlowInfo _tmp4B3;void*_tmp4B4;struct _tuple14 _tmp4B2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp493);_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4B3,_tmp494);}_LL2D8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp495=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp495->tag != 2)goto _LL2DA;else{if(_tmp495->f1 != Cyc_Absyn_Not)goto _LL2DA;if(_tmp495->f2 == 0)goto _LL2DA;_tmp496=(struct Cyc_Absyn_Exp*)(_tmp495->f2)->hd;if((_tmp495->f2)->tl != 0)goto _LL2DA;}}_LL2D9: {
# 1853
union Cyc_CfFlowInfo_FlowInfo _tmp4B6;union Cyc_CfFlowInfo_FlowInfo _tmp4B7;struct _tuple16 _tmp4B5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp496);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{
struct _tuple16 _tmp84A;return(_tmp84A.f1=_tmp4B7,((_tmp84A.f2=_tmp4B6,_tmp84A)));};}_LL2DA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp487;if(_tmp497->tag != 2)goto _LL2DC;else{_tmp498=_tmp497->f1;_tmp499=_tmp497->f2;}}_LL2DB:
# 1856
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp498,_tmp499);_LL2DC:;_LL2DD: {
# 1860
union Cyc_CfFlowInfo_FlowInfo _tmp4BA;void*_tmp4BB;struct _tuple14 _tmp4B9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp4BA=_tmp4B9.f1;_tmp4BB=_tmp4B9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BC=_tmp4BA;struct Cyc_Dict_Dict _tmp4BD;_LL2DF: if((_tmp4BC.BottomFL).tag != 1)goto _LL2E1;_LL2E0: {
struct _tuple16 _tmp84B;return(_tmp84B.f1=_tmp4BA,((_tmp84B.f2=_tmp4BA,_tmp84B)));}_LL2E1: if((_tmp4BC.ReachableFL).tag != 2)goto _LL2DE;_tmp4BD=((struct _tuple13)(_tmp4BC.ReachableFL).val).f1;_LL2E2: {
# 1864
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp4BF=_tmp4BB;struct Cyc_Absyn_Vardecl*_tmp4C1;void*_tmp4C2;enum Cyc_CfFlowInfo_InitLevel _tmp4CB;_LL2E4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C0->tag != 8)goto _LL2E6;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=(void*)_tmp4C0->f2;}}_LL2E5:
 _tmp4BB=_tmp4C2;locname=_tmp4C1;goto retry;_LL2E6: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C3->tag != 0)goto _LL2E8;}_LL2E7: {
struct _tuple16 _tmp84C;return(_tmp84C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84C.f2=_tmp4BA,_tmp84C)));}_LL2E8: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4C4=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C4->tag != 2)goto _LL2EA;}_LL2E9:
 goto _LL2EB;_LL2EA: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4C5=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C5->tag != 1)goto _LL2EC;}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4C6=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C6->tag != 5)goto _LL2EE;}_LL2ED: {
struct _tuple16 _tmp84D;return(_tmp84D.f1=_tmp4BA,((_tmp84D.f2=Cyc_CfFlowInfo_BottomFL(),_tmp84D)));}_LL2EE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C7->tag != 3)goto _LL2F0;else{if(_tmp4C7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F0;}}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4C8=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C8->tag != 4)goto _LL2F2;else{if(_tmp4C8->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F2;}}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4C9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4C9->tag != 7)goto _LL2F4;}_LL2F3:
# 1874
{const char*_tmp850;void*_tmp84F;(_tmp84F=0,Cyc_Tcutil_terr(e->loc,((_tmp850="expression may not be initialized",_tag_dyneither(_tmp850,sizeof(char),34))),_tag_dyneither(_tmp84F,sizeof(void*),0)));}{
struct _tuple16 _tmp851;return(_tmp851.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp851.f2=Cyc_CfFlowInfo_BottomFL(),_tmp851)));};_LL2F4: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4CA=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4CA->tag != 3)goto _LL2F6;else{_tmp4CB=_tmp4CA->f1;}}_LL2F5:
# 1877
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4BA,e,_tmp4CB,locname);_LL2F6: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4CC=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4CC->tag != 4)goto _LL2F8;}_LL2F7: {
struct _tuple16 _tmp852;return(_tmp852.f1=_tmp4BA,((_tmp852.f2=_tmp4BA,_tmp852)));}_LL2F8: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4CD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4BF;if(_tmp4CD->tag != 6)goto _LL2E3;}_LL2F9: {
struct Cyc_Core_Impossible_exn_struct _tmp858;const char*_tmp857;struct Cyc_Core_Impossible_exn_struct*_tmp856;(int)_throw((void*)((_tmp856=_cycalloc(sizeof(*_tmp856)),((_tmp856[0]=((_tmp858.tag=Cyc_Core_Impossible,((_tmp858.f1=((_tmp857="anal_test",_tag_dyneither(_tmp857,sizeof(char),10))),_tmp858)))),_tmp856)))));}_LL2E3:;}}_LL2DE:;};}_LL2CF:;}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1885
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1887
unsigned int _tmp4D8;struct Cyc_NewControlFlow_AnalEnv*_tmp4D9;struct Cyc_Dict_Dict _tmp4DA;struct _tuple23*_tmp4D7=ckenv;_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;_tmp4DA=_tmp4D7->f3;{
void*_tmp4DB=root;struct Cyc_Absyn_Vardecl*_tmp4DD;_LL2FB: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4DC=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4DB;if(_tmp4DC->tag != 0)goto _LL2FD;else{_tmp4DD=_tmp4DC->f1;}}_LL2FC:
# 1890
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4D9->fenv)->r,_tmp4DD->type)){
void*_tmp4DE=rval;_LL300: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4DF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4DF->tag != 8)goto _LL302;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4E0=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4DF->f2);if(_tmp4E0->tag != 7)goto _LL302;}}_LL301:
 goto _LL303;_LL302: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4E1->tag != 8)goto _LL304;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4E1->f2);if(_tmp4E2->tag != 0)goto _LL304;}}_LL303:
 goto _LL305;_LL304: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4E3->tag != 8)goto _LL306;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4E3->f2);if(_tmp4E4->tag != 3)goto _LL306;else{if(_tmp4E4->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL306;}}}_LL305:
 goto _LL307;_LL306: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4E5=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4E5->tag != 7)goto _LL308;}_LL307:
 goto _LL309;_LL308: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4E6->tag != 0)goto _LL30A;}_LL309:
 goto _LL30B;_LL30A: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4DE;if(_tmp4E7->tag != 3)goto _LL30C;else{if(_tmp4E7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL30C;}}_LL30B:
 goto _LL2FF;_LL30C:;_LL30D:
# 1899
{const char*_tmp85C;void*_tmp85B[1];struct Cyc_String_pa_PrintArg_struct _tmp85A;(_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4DD->name)),((_tmp85B[0]=& _tmp85A,Cyc_Tcutil_warn(_tmp4D8,((_tmp85C="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp85C,sizeof(char),57))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))));}
goto _LL2FF;_LL2FF:;}
# 1902
goto _LL2FA;_LL2FD:;_LL2FE:
 goto _LL2FA;_LL2FA:;};}
# 1907
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4EB=flow;struct Cyc_Dict_Dict _tmp4EC;_LL30F: if((_tmp4EB.BottomFL).tag != 1)goto _LL311;_LL310:
 return;_LL311: if((_tmp4EB.ReachableFL).tag != 2)goto _LL30E;_tmp4EC=((struct _tuple13)(_tmp4EB.ReachableFL).val).f1;_LL312:
# 1911
{struct Cyc_List_List*_tmp4ED=env->param_roots;for(0;_tmp4ED != 0;_tmp4ED=_tmp4ED->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4EC,Cyc_CfFlowInfo_lookup_place(_tmp4EC,(struct Cyc_CfFlowInfo_Place*)_tmp4ED->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_terr(loc,((_tmp85F="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp85F,sizeof(char),76))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}}}
# 1915
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp860;struct _tuple23 _tmp4F0=(_tmp860.f1=loc,((_tmp860.f2=env,((_tmp860.f3=_tmp4EC,_tmp860)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4F0,_tmp4EC);}
# 1919
return;_LL30E:;}struct _tuple24{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple25{void*f1;void*f2;void*f3;};
# 1923
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1925
union Cyc_CfFlowInfo_FlowInfo _tmp4F2=flow;struct Cyc_Dict_Dict _tmp4F3;_LL314: if((_tmp4F2.BottomFL).tag != 1)goto _LL316;_LL315:
 return;_LL316: if((_tmp4F2.ReachableFL).tag != 2)goto _LL313;_tmp4F3=((struct _tuple13)(_tmp4F2.ReachableFL).val).f1;_LL317: {
# 1928
struct Cyc_Absyn_Exp*_tmp4F4=Cyc_Absyn_exp_unsigned_one;
int _tmp4F5=0;
int _tmp4F6=1;
{struct Cyc_List_List*_tmp4F7=env->noconsume_params;for(0;_tmp4F7 != 0;_tmp4F7=_tmp4F7->tl){
# 1934
struct Cyc_CfFlowInfo_Place*_tmp4F9;void*_tmp4FA;struct _tuple24*_tmp4F8=(struct _tuple24*)_tmp4F7->hd;_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F8->f2;{
void*_tmp4FB=Cyc_CfFlowInfo_lookup_place(_tmp4F3,_tmp4F9);
void*_tmp4FD;struct Cyc_CfFlowInfo_Place*_tmp4FC=_tmp4F9;_tmp4FD=_tmp4FC->root;{
struct _tuple25 _tmp861;struct _tuple25 _tmp4FF=(_tmp861.f1=_tmp4FD,((_tmp861.f2=_tmp4FA,((_tmp861.f3=_tmp4FB,_tmp861)))));struct Cyc_Absyn_Vardecl*_tmp501;struct Cyc_Absyn_Vardecl*_tmp503;void*_tmp504;struct Cyc_Absyn_Vardecl*_tmp506;void*_tmp507;struct Cyc_Absyn_Vardecl*_tmp509;_LL319:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp500=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FF.f1;if(_tmp500->tag != 0)goto _LL31B;else{_tmp501=_tmp500->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp502=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FF.f2;if(_tmp502->tag != 8)goto _LL31B;else{_tmp503=_tmp502->f1;_tmp504=(void*)_tmp502->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp505=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4FF.f3;if(_tmp505->tag != 8)goto _LL31B;else{_tmp506=_tmp505->f1;_tmp507=(void*)_tmp505->f2;}};_LL31A:
# 1939
 if(_tmp503 == _tmp506){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4F4,_tmp4F6,_tmp507,& _tmp4F5)){
const char*_tmp865;void*_tmp864[1];struct Cyc_String_pa_PrintArg_struct _tmp863;(_tmp863.tag=0,((_tmp863.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1943
Cyc_Absynpp_qvar2string(_tmp501->name)),((_tmp864[0]=& _tmp863,Cyc_Tcutil_terr(loc,((_tmp865="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp865,sizeof(char),58))),_tag_dyneither(_tmp864,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4F3,_tmp507)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp869;void*_tmp868[1];struct Cyc_String_pa_PrintArg_struct _tmp867;(_tmp867.tag=0,((_tmp867.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1947
Cyc_Absynpp_qvar2string(_tmp501->name)),((_tmp868[0]=& _tmp867,Cyc_Tcutil_terr(loc,((_tmp869="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp869,sizeof(char),84))),_tag_dyneither(_tmp868,sizeof(void*),1)))))));}}}else{
# 1951
_tmp509=_tmp501;goto _LL31C;}
goto _LL318;_LL31B: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp508=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4FF.f1;if(_tmp508->tag != 0)goto _LL31D;else{_tmp509=_tmp508->f1;}}_LL31C:
# 1954
{const char*_tmp86D;void*_tmp86C[1];struct Cyc_String_pa_PrintArg_struct _tmp86B;(_tmp86B.tag=0,((_tmp86B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1956
Cyc_Absynpp_qvar2string(_tmp509->name)),((_tmp86C[0]=& _tmp86B,Cyc_Tcutil_terr(loc,((_tmp86D="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp86D,sizeof(char),85))),_tag_dyneither(_tmp86C,sizeof(void*),1)))))));}
goto _LL318;_LL31D:;_LL31E: {
# 1959
const char*_tmp870;void*_tmp86F;(_tmp86F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp870="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp870,sizeof(char),46))),_tag_dyneither(_tmp86F,sizeof(void*),0)));}_LL318:;};};}}
# 1962
return;}_LL313:;}
# 1969
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1971
struct Cyc_CfFlowInfo_FlowEnv*_tmp515=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp517;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Stmt*_tmp519;unsigned int _tmp51A;struct Cyc_Absyn_Switch_clause*_tmp516=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp517=_tmp516->pat_vars;_tmp518=_tmp516->where_clause;_tmp519=_tmp516->body;_tmp51A=_tmp516->loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp515,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp517))->v,_tmp515->unknown_all,_tmp51A,0);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp518 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp518;
union Cyc_CfFlowInfo_FlowInfo _tmp51C;union Cyc_CfFlowInfo_FlowInfo _tmp51D;struct _tuple16 _tmp51B=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;
inflow=_tmp51D;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp51C,_tmp519);}else{
# 1982
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp519);}{
# 1984
union Cyc_CfFlowInfo_FlowInfo _tmp51E=clause_outflow;_LL320: if((_tmp51E.BottomFL).tag != 1)goto _LL322;_LL321:
 goto _LL31F;_LL322:;_LL323:
# 1988
 if(scs->tl == 0)
return clause_outflow;else{
# 1993
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp873;void*_tmp872;(_tmp872=0,Cyc_Tcutil_terr(_tmp519->loc,((_tmp873="switch clause may implicitly fallthru",_tag_dyneither(_tmp873,sizeof(char),38))),_tag_dyneither(_tmp872,sizeof(void*),0)));}else{
# 1996
const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_warn(_tmp519->loc,((_tmp876="switch clause may implicitly fallthru",_tag_dyneither(_tmp876,sizeof(char),38))),_tag_dyneither(_tmp875,sizeof(void*),0)));}
# 1998
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2000
goto _LL31F;_LL31F:;};};}
# 2003
return Cyc_CfFlowInfo_BottomFL();}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2008
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple26*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2010
struct Cyc_NewControlFlow_AnalEnv*_tmp524;struct Cyc_Dict_Dict _tmp525;unsigned int _tmp526;struct _tuple26*_tmp523=vdenv;_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;_tmp526=_tmp523->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp524->fenv)->r,vd->type)){
# 2013
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp877;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp877.tag=0,((_tmp877.f1=vd,_tmp877)));
# 2015
void*_tmp527=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp525,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp528=_tmp527;_LL325: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp529=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp529->tag != 8)goto _LL327;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp52A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp529->f2);if(_tmp52A->tag != 7)goto _LL327;}}_LL326:
 goto _LL328;_LL327: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp52B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52B->tag != 8)goto _LL329;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp52C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp52B->f2);if(_tmp52C->tag != 0)goto _LL329;}}_LL328:
 goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp52D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52D->tag != 8)goto _LL32B;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp52E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp52D->f2);if(_tmp52E->tag != 3)goto _LL32B;else{if(_tmp52E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32B;}}}_LL32A:
 goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp52F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52F->tag != 7)goto _LL32D;}_LL32C:
 goto _LL32E;_LL32D: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp530=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp530->tag != 0)goto _LL32F;}_LL32E:
 goto _LL330;_LL32F: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp531=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp531->tag != 3)goto _LL331;else{if(_tmp531->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL331;}}_LL330:
 goto _LL324;_LL331:;_LL332:
# 2024
{const char*_tmp87B;void*_tmp87A[1];struct Cyc_String_pa_PrintArg_struct _tmp879;(_tmp879.tag=0,((_tmp879.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp87A[0]=& _tmp879,Cyc_Tcutil_warn(_tmp526,((_tmp87B="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp87B,sizeof(char),68))),_tag_dyneither(_tmp87A,sizeof(void*),1)))))));}
# 2026
goto _LL324;_LL324:;}}
# 2031
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2033
{union Cyc_CfFlowInfo_FlowInfo _tmp536=inflow;struct Cyc_Dict_Dict _tmp537;_LL334: if((_tmp536.ReachableFL).tag != 2)goto _LL336;_tmp537=((struct _tuple13)(_tmp536.ReachableFL).val).f1;_LL335: {
# 2035
struct _tuple26 _tmp87C;struct _tuple26 _tmp538=(_tmp87C.f1=env,((_tmp87C.f2=_tmp537,((_tmp87C.f3=decl->loc,_tmp87C)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp539=env->fenv;
{void*_tmp53A=decl->r;struct Cyc_Absyn_Vardecl*_tmp53C;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp540;_LL339: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp53B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53A;if(_tmp53B->tag != 0)goto _LL33B;else{_tmp53C=_tmp53B->f1;}}_LL33A:
# 2039
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp538,_tmp53C);
goto _LL338;_LL33B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp53D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp53A;if(_tmp53D->tag != 2)goto _LL33D;else{if(_tmp53D->f2 == 0)goto _LL33D;_tmp53E=(struct Cyc_List_List*)(_tmp53D->f2)->v;}}_LL33C:
 _tmp540=_tmp53E;goto _LL33E;_LL33D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp53F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53A;if(_tmp53F->tag != 3)goto _LL33F;else{_tmp540=_tmp53F->f1;}}_LL33E:
# 2043
((void(*)(void(*f)(struct _tuple26*,struct Cyc_Absyn_Vardecl*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp538,_tmp540);
goto _LL338;_LL33F:;_LL340:
 goto _LL338;_LL338:;}
# 2047
goto _LL333;}_LL336:;_LL337:
 goto _LL333;_LL333:;}
# 2050
return;}
# 2053
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2058
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp543;union Cyc_CfFlowInfo_FlowInfo*_tmp544;struct _tuple17 _tmp542=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp543=_tmp542.f1;_tmp544=_tmp542.f2;
inflow=*_tmp544;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp545=env->fenv;
void*_tmp546=s->r;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Stmt*_tmp54E;struct Cyc_Absyn_Stmt*_tmp54F;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Stmt*_tmp552;struct Cyc_Absyn_Stmt*_tmp553;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Stmt*_tmp556;struct Cyc_Absyn_Stmt*_tmp557;struct Cyc_Absyn_Stmt*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Stmt*_tmp55B;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Stmt*_tmp55F;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Stmt*_tmp561;struct Cyc_Absyn_Stmt*_tmp562;struct Cyc_List_List*_tmp565;struct Cyc_Absyn_Switch_clause*_tmp566;struct Cyc_Absyn_Stmt*_tmp568;struct Cyc_Absyn_Stmt*_tmp56A;struct Cyc_Absyn_Stmt*_tmp56C;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_List_List*_tmp56F;struct Cyc_Absyn_Stmt*_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_Absyn_Tvar*_tmp575;struct Cyc_Absyn_Vardecl*_tmp576;struct Cyc_Absyn_Stmt*_tmp577;struct Cyc_Absyn_Decl*_tmp579;struct Cyc_Absyn_Stmt*_tmp57A;struct Cyc_Absyn_Stmt*_tmp57C;struct Cyc_Absyn_Exp*_tmp57E;_LL342: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp547=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp547->tag != 0)goto _LL344;}_LL343:
 return inflow;_LL344: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp548=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp548->tag != 3)goto _LL346;else{if(_tmp548->f1 != 0)goto _LL346;}}_LL345:
# 2064
 if(env->noreturn){
const char*_tmp87F;void*_tmp87E;(_tmp87E=0,Cyc_Tcutil_terr(s->loc,((_tmp87F="`noreturn' function might return",_tag_dyneither(_tmp87F,sizeof(char),33))),_tag_dyneither(_tmp87E,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL346: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp549=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp549->tag != 3)goto _LL348;else{_tmp54A=_tmp549->f1;}}_LL347:
# 2070
 if(env->noreturn){
const char*_tmp882;void*_tmp881;(_tmp881=0,Cyc_Tcutil_terr(s->loc,((_tmp882="`noreturn' function might return",_tag_dyneither(_tmp882,sizeof(char),33))),_tag_dyneither(_tmp881,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp584;void*_tmp585;struct _tuple14 _tmp583=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp54A));_tmp584=_tmp583.f1;_tmp585=_tmp583.f2;
_tmp584=Cyc_NewControlFlow_use_Rval(env,_tmp54A->loc,_tmp584,_tmp585);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp584);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp584);
return Cyc_CfFlowInfo_BottomFL();};_LL348: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp54B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp54B->tag != 1)goto _LL34A;else{_tmp54C=_tmp54B->f1;}}_LL349:
# 2078
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp54C)).f1;_LL34A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp54D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp54D->tag != 2)goto _LL34C;else{_tmp54E=_tmp54D->f1;_tmp54F=_tmp54D->f2;}}_LL34B:
# 2081
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp54E),_tmp54F);_LL34C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp550=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp550->tag != 4)goto _LL34E;else{_tmp551=_tmp550->f1;_tmp552=_tmp550->f2;_tmp553=_tmp550->f3;}}_LL34D: {
# 2084
union Cyc_CfFlowInfo_FlowInfo _tmp587;union Cyc_CfFlowInfo_FlowInfo _tmp588;struct _tuple16 _tmp586=Cyc_NewControlFlow_anal_test(env,inflow,_tmp551);_tmp587=_tmp586.f1;_tmp588=_tmp586.f2;{
# 2091
union Cyc_CfFlowInfo_FlowInfo _tmp589=Cyc_NewControlFlow_anal_stmt(env,_tmp588,_tmp553);
union Cyc_CfFlowInfo_FlowInfo _tmp58A=Cyc_NewControlFlow_anal_stmt(env,_tmp587,_tmp552);
return Cyc_CfFlowInfo_join_flow(_tmp545,env->all_changed,_tmp58A,_tmp589);};}_LL34E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp554=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp554->tag != 5)goto _LL350;else{_tmp555=(_tmp554->f1).f1;_tmp556=(_tmp554->f1).f2;_tmp557=_tmp554->f2;}}_LL34F: {
# 2099
union Cyc_CfFlowInfo_FlowInfo*_tmp58C;struct _tuple17 _tmp58B=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp556);_tmp58C=_tmp58B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp58D=*_tmp58C;
union Cyc_CfFlowInfo_FlowInfo _tmp58F;union Cyc_CfFlowInfo_FlowInfo _tmp590;struct _tuple16 _tmp58E=Cyc_NewControlFlow_anal_test(env,_tmp58D,_tmp555);_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp591=Cyc_NewControlFlow_anal_stmt(env,_tmp58F,_tmp557);
Cyc_NewControlFlow_update_flow(env,_tmp556,_tmp591);
return _tmp590;};};}_LL350: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp558=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp558->tag != 14)goto _LL352;else{_tmp559=_tmp558->f1;_tmp55A=(_tmp558->f2).f1;_tmp55B=(_tmp558->f2).f2;}}_LL351: {
# 2109
union Cyc_CfFlowInfo_FlowInfo _tmp592=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp559);
union Cyc_CfFlowInfo_FlowInfo*_tmp594;struct _tuple17 _tmp593=Cyc_NewControlFlow_pre_stmt_check(env,_tmp592,_tmp55B);_tmp594=_tmp593.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp595=*_tmp594;
union Cyc_CfFlowInfo_FlowInfo _tmp597;union Cyc_CfFlowInfo_FlowInfo _tmp598;struct _tuple16 _tmp596=Cyc_NewControlFlow_anal_test(env,_tmp595,_tmp55A);_tmp597=_tmp596.f1;_tmp598=_tmp596.f2;
Cyc_NewControlFlow_update_flow(env,_tmp559,_tmp597);
return _tmp598;};}_LL352: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp55C=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp55C->tag != 9)goto _LL354;else{_tmp55D=_tmp55C->f1;_tmp55E=(_tmp55C->f2).f1;_tmp55F=(_tmp55C->f2).f2;_tmp560=(_tmp55C->f3).f1;_tmp561=(_tmp55C->f3).f2;_tmp562=_tmp55C->f4;}}_LL353: {
# 2118
union Cyc_CfFlowInfo_FlowInfo _tmp599=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp55D)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp59B;struct _tuple17 _tmp59A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp599,_tmp55F);_tmp59B=_tmp59A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp59C=*_tmp59B;
union Cyc_CfFlowInfo_FlowInfo _tmp59E;union Cyc_CfFlowInfo_FlowInfo _tmp59F;struct _tuple16 _tmp59D=Cyc_NewControlFlow_anal_test(env,_tmp59C,_tmp55E);_tmp59E=_tmp59D.f1;_tmp59F=_tmp59D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A0=Cyc_NewControlFlow_anal_stmt(env,_tmp59E,_tmp562);
union Cyc_CfFlowInfo_FlowInfo*_tmp5A2;struct _tuple17 _tmp5A1=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5A0,_tmp561);_tmp5A2=_tmp5A1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A3=*_tmp5A2;
union Cyc_CfFlowInfo_FlowInfo _tmp5A4=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5A3,_tmp560)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp55F,_tmp5A4);
return _tmp59F;};};};}_LL354: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp563=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp563->tag != 6)goto _LL356;else{if(_tmp563->f1 != 0)goto _LL356;}}_LL355:
# 2130
 return Cyc_CfFlowInfo_BottomFL();_LL356: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp564=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp564->tag != 11)goto _LL358;else{_tmp565=_tmp564->f1;if(_tmp564->f2 == 0)goto _LL358;_tmp566=*_tmp564->f2;}}_LL357: {
# 2132
struct _RegionHandle _tmp5A5=_new_region("temp");struct _RegionHandle*temp=& _tmp5A5;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp5A7;struct Cyc_List_List*_tmp5A8;struct _tuple20 _tmp5A6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp565,1,1);_tmp5A7=_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;
# 2135
for(0;_tmp5A8 != 0;(_tmp5A8=_tmp5A8->tl,_tmp565=_tmp565->tl)){
_tmp5A7=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp565))->hd)->loc,_tmp5A7,(void*)_tmp5A8->hd);}
# 2138
_tmp5A7=Cyc_NewControlFlow_add_vars(_tmp545,_tmp5A7,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp566->pat_vars))->v,_tmp545->unknown_all,s->loc,0);
Cyc_NewControlFlow_update_flow(env,_tmp566->body,_tmp5A7);{
union Cyc_CfFlowInfo_FlowInfo _tmp5A9=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5A9;};}
# 2133
;_pop_region(temp);}_LL358: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp567=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp567->tag != 6)goto _LL35A;else{_tmp568=_tmp567->f1;}}_LL359:
# 2142
 _tmp56A=_tmp568;goto _LL35B;_LL35A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp569->tag != 7)goto _LL35C;else{_tmp56A=_tmp569->f1;}}_LL35B:
 _tmp56C=_tmp56A;goto _LL35D;_LL35C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp56B=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp56B->tag != 8)goto _LL35E;else{_tmp56C=_tmp56B->f2;}}_LL35D:
# 2147
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5AA=_tmp543->encloser;
struct Cyc_Absyn_Stmt*_tmp5AB=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp56C)))->encloser;
while(_tmp5AB != _tmp5AA){
struct Cyc_Absyn_Stmt*_tmp5AC=(Cyc_NewControlFlow_get_stmt_annot(_tmp5AA))->encloser;
if(_tmp5AC == _tmp5AA){
{const char*_tmp885;void*_tmp884;(_tmp884=0,Cyc_Tcutil_terr(s->loc,((_tmp885="goto enters local scope or exception handler",_tag_dyneither(_tmp885,sizeof(char),45))),_tag_dyneither(_tmp884,sizeof(void*),0)));}
break;}
# 2156
_tmp5AA=_tmp5AC;}}
# 2161
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp56C),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL35E: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp56D=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp56D->tag != 10)goto _LL360;else{_tmp56E=_tmp56D->f1;_tmp56F=_tmp56D->f2;}}_LL35F: {
# 2165
union Cyc_CfFlowInfo_FlowInfo _tmp5B0;void*_tmp5B1;struct _tuple14 _tmp5AF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp56E);_tmp5B0=_tmp5AF.f1;_tmp5B1=_tmp5AF.f2;
_tmp5B0=Cyc_NewControlFlow_use_Rval(env,_tmp56E->loc,_tmp5B0,_tmp5B1);
return Cyc_NewControlFlow_anal_scs(env,_tmp5B0,_tmp56F);}_LL360: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp570=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp570->tag != 15)goto _LL362;else{_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;}}_LL361: {
# 2172
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp571);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2180
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2183
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp572);
{union Cyc_CfFlowInfo_FlowInfo _tmp5B2=scs_outflow;_LL36D: if((_tmp5B2.BottomFL).tag != 1)goto _LL36F;_LL36E:
 goto _LL36C;_LL36F:;_LL370: {
const char*_tmp888;void*_tmp887;(_tmp887=0,Cyc_Tcutil_terr(s->loc,((_tmp888="last catch clause may implicitly fallthru",_tag_dyneither(_tmp888,sizeof(char),42))),_tag_dyneither(_tmp887,sizeof(void*),0)));}_LL36C:;}
# 2189
outflow=s1_outflow;
# 2191
return outflow;};};}_LL362: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp573=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp573->tag != 12)goto _LL364;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp574=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp573->f1)->r;if(_tmp574->tag != 5)goto _LL364;else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;}}_tmp577=_tmp573->f2;}}_LL363: {
# 2197
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp576->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp5B6;union Cyc_CfFlowInfo_AbsLVal _tmp5B7;struct _tuple15 _tmp5B5=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp5B6=_tmp5B5.f1;_tmp5B7=_tmp5B5.f2;{
# 2202
struct _tuple15 _tmp889;struct _tuple15 _tmp5B9=(_tmp889.f1=_tmp5B6,((_tmp889.f2=_tmp5B7,_tmp889)));struct Cyc_Dict_Dict _tmp5BA;struct Cyc_List_List*_tmp5BB;struct Cyc_CfFlowInfo_Place*_tmp5BC;_LL372: if(((_tmp5B9.f1).ReachableFL).tag != 2)goto _LL374;_tmp5BA=((struct _tuple13)((_tmp5B9.f1).ReachableFL).val).f1;_tmp5BB=((struct _tuple13)((_tmp5B9.f1).ReachableFL).val).f2;if(((_tmp5B9.f2).PlaceL).tag != 1)goto _LL374;_tmp5BC=(struct Cyc_CfFlowInfo_Place*)((_tmp5B9.f2).PlaceL).val;_LL373: {
# 2204
void*_tmp5BD=Cyc_CfFlowInfo_lookup_place(_tmp5BA,_tmp5BC);
# 2206
void*t=(void*)_check_null(e->topt);
# 2213
void*_tmp5BE=Cyc_CfFlowInfo_make_unique_consumed(_tmp545,t,e,env->iteration_num,_tmp5BD);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp88C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp88B;_tmp5BE=(void*)((_tmp88B=_region_malloc(_tmp545->r,sizeof(*_tmp88B)),((_tmp88B[0]=((_tmp88C.tag=8,((_tmp88C.f1=_tmp576,((_tmp88C.f2=_tmp5BE,_tmp88C)))))),_tmp88B))));}
_tmp5BA=Cyc_CfFlowInfo_assign_place(_tmp545,e->loc,_tmp5BA,env->all_changed,_tmp5BC,_tmp5BE);
_tmp5B6=Cyc_CfFlowInfo_ReachableFL(_tmp5BA,_tmp5BB);{
# 2220
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp545,_tmp5BA,_tmp5BD)){case Cyc_CfFlowInfo_AllIL: _LL378:
 leaf=_tmp545->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL379:
 leaf=_tmp545->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL37A:
 leaf=_tmp545->unknown_this;break;}{
# 2228
void*_tmp5C1=Cyc_CfFlowInfo_typ_to_absrval(_tmp545,t,0,leaf);
_tmp5B6=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp5B6,_tmp5BD);{
struct Cyc_List_List _tmp88D;struct Cyc_List_List _tmp5C2=(_tmp88D.hd=_tmp576,((_tmp88D.tl=0,_tmp88D)));
_tmp5B6=Cyc_NewControlFlow_add_vars(_tmp545,_tmp5B6,& _tmp5C2,_tmp545->unknown_all,s->loc,0);
# 2235
_tmp5B6=Cyc_NewControlFlow_anal_stmt(env,_tmp5B6,_tmp577);{
# 2240
union Cyc_CfFlowInfo_FlowInfo _tmp5C3=_tmp5B6;struct Cyc_Dict_Dict _tmp5C4;struct Cyc_List_List*_tmp5C5;_LL37D: if((_tmp5C3.ReachableFL).tag != 2)goto _LL37F;_tmp5C4=((struct _tuple13)(_tmp5C3.ReachableFL).val).f1;_tmp5C5=((struct _tuple13)(_tmp5C3.ReachableFL).val).f2;_LL37E:
# 2243
{void*_tmp5C6=Cyc_CfFlowInfo_lookup_place(_tmp5C4,_tmp5BC);struct Cyc_Absyn_Vardecl*_tmp5C8;_LL382: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C6;if(_tmp5C7->tag != 8)goto _LL384;else{_tmp5C8=_tmp5C7->f1;}}_LL383:
# 2245
 if(_tmp5C8 != _tmp576)goto _LL385;
goto _LL381;_LL384:;_LL385:
# 2248
{const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(s->loc,((_tmp890="aliased location was overwritten",_tag_dyneither(_tmp890,sizeof(char),33))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}
goto _LL381;_LL381:;}
# 2251
_tmp5C4=Cyc_CfFlowInfo_assign_place(_tmp545,s->loc,_tmp5C4,env->all_changed,_tmp5BC,_tmp5C1);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5C4,_tmp5C5);
# 2256
return outflow;_LL37F:;_LL380:
 return _tmp5B6;_LL37C:;};};};};}_LL374: if(((_tmp5B9.f1).BottomFL).tag != 1)goto _LL376;_LL375:
# 2259
 return _tmp5B6;_LL376:;_LL377:
{const char*_tmp893;void*_tmp892;(_tmp892=0,Cyc_Tcutil_terr(e->loc,((_tmp893="bad alias expression--no unique path found",_tag_dyneither(_tmp893,sizeof(char),43))),_tag_dyneither(_tmp892,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL371:;};}_LL364: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp578=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp578->tag != 12)goto _LL366;else{_tmp579=_tmp578->f1;_tmp57A=_tmp578->f2;}}_LL365:
# 2265
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp579),_tmp57A);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp579);
return outflow;_LL366: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp57B=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp57B->tag != 13)goto _LL368;else{_tmp57C=_tmp57B->f2;}}_LL367:
# 2270
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp57C);_LL368: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp57D=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp546;if(_tmp57D->tag != 16)goto _LL36A;else{_tmp57E=_tmp57D->f1;}}_LL369: {
# 2273
union Cyc_CfFlowInfo_FlowInfo _tmp5CF;void*_tmp5D0;struct _tuple14 _tmp5CE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp57E);_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D1=Cyc_NewControlFlow_use_Rval(env,_tmp57E->loc,_tmp5CF,_tmp5D0);
void*_tmp5D2=Cyc_Tcutil_compress((void*)_check_null(_tmp57E->topt));void*_tmp5D4;_LL387: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5D2;if(_tmp5D3->tag != 15)goto _LL389;else{_tmp5D4=(void*)_tmp5D3->f1;}}_LL388:
# 2277
 return Cyc_CfFlowInfo_kill_flow_region(_tmp545,_tmp5CF,_tmp5D4);_LL389:;_LL38A: {
struct Cyc_Core_Impossible_exn_struct _tmp899;const char*_tmp898;struct Cyc_Core_Impossible_exn_struct*_tmp897;(int)_throw((void*)((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp899.tag=Cyc_Core_Impossible,((_tmp899.f1=((_tmp898="anal_stmt -- reset_region",_tag_dyneither(_tmp898,sizeof(char),26))),_tmp899)))),_tmp897)))));}_LL386:;};}_LL36A:;_LL36B: {
# 2281
struct Cyc_Core_Impossible_exn_struct _tmp89F;const char*_tmp89E;struct Cyc_Core_Impossible_exn_struct*_tmp89D;(int)_throw((void*)((_tmp89D=_cycalloc(sizeof(*_tmp89D)),((_tmp89D[0]=((_tmp89F.tag=Cyc_Core_Impossible,((_tmp89F.f1=((_tmp89E="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp89E,sizeof(char),56))),_tmp89F)))),_tmp89D)))));}_LL341:;};}
# 2286
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2288
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5DB=env->fenv;
void*_tmp5DC=decl->r;struct Cyc_Absyn_Vardecl*_tmp5DE;struct Cyc_List_List*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_List_List*_tmp5E3;struct Cyc_Absyn_Fndecl*_tmp5E5;struct Cyc_Absyn_Tvar*_tmp5E7;struct Cyc_Absyn_Vardecl*_tmp5E8;int _tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;_LL38C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5DD=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5DC;if(_tmp5DD->tag != 0)goto _LL38E;else{_tmp5DE=_tmp5DD->f1;}}_LL38D: {
# 2298
struct Cyc_List_List _tmp8A0;struct Cyc_List_List _tmp5EB=(_tmp8A0.hd=_tmp5DE,((_tmp8A0.tl=0,_tmp8A0)));
inflow=Cyc_NewControlFlow_add_vars(_tmp5DB,inflow,& _tmp5EB,_tmp5DB->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp5EC=_tmp5DE->initializer;
if(_tmp5EC == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5EE;void*_tmp5EF;struct _tuple14 _tmp5ED=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5EC);_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5F0=_tmp5EE;struct Cyc_Dict_Dict _tmp5F1;struct Cyc_List_List*_tmp5F2;_LL399: if((_tmp5F0.BottomFL).tag != 1)goto _LL39B;_LL39A:
 return Cyc_CfFlowInfo_BottomFL();_LL39B: if((_tmp5F0.ReachableFL).tag != 2)goto _LL398;_tmp5F1=((struct _tuple13)(_tmp5F0.ReachableFL).val).f1;_tmp5F2=((struct _tuple13)(_tmp5F0.ReachableFL).val).f2;_LL39C:
# 2307
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8A6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8A5;struct Cyc_CfFlowInfo_Place*_tmp8A4;_tmp5F1=Cyc_CfFlowInfo_assign_place(_tmp5DB,decl->loc,_tmp5F1,env->all_changed,(
(_tmp8A4=_region_malloc(env->r,sizeof(*_tmp8A4)),((_tmp8A4->root=(void*)((_tmp8A6=_region_malloc(env->r,sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A5.tag=0,((_tmp8A5.f1=_tmp5DE,_tmp8A5)))),_tmp8A6)))),((_tmp8A4->fields=0,_tmp8A4)))))),_tmp5EF);}
# 2310
_tmp5F2=Cyc_Relations_reln_assign_var(env->r,_tmp5F2,_tmp5DE,(struct Cyc_Absyn_Exp*)_check_null(_tmp5DE->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp5F6=Cyc_CfFlowInfo_ReachableFL(_tmp5F1,_tmp5F2);
Cyc_NewControlFlow_update_tryflow(env,_tmp5F6);
# 2315
return _tmp5F6;};_LL398:;};};};}_LL38E: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5DF=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5DC;if(_tmp5DF->tag != 2)goto _LL390;else{if(_tmp5DF->f2 == 0)goto _LL390;_tmp5E0=(struct Cyc_List_List*)(_tmp5DF->f2)->v;_tmp5E1=_tmp5DF->f3;}}_LL38F: {
# 2320
union Cyc_CfFlowInfo_FlowInfo _tmp5F9;void*_tmp5FA;struct _tuple14 _tmp5F8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5E1);_tmp5F9=_tmp5F8.f1;_tmp5FA=_tmp5F8.f2;
_tmp5F9=Cyc_NewControlFlow_use_Rval(env,_tmp5E1->loc,_tmp5F9,_tmp5FA);
return Cyc_NewControlFlow_add_vars(_tmp5DB,_tmp5F9,_tmp5E0,_tmp5DB->unknown_all,decl->loc,0);}_LL390: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5E2=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5DC;if(_tmp5E2->tag != 3)goto _LL392;else{_tmp5E3=_tmp5E2->f1;}}_LL391:
# 2325
 return Cyc_NewControlFlow_add_vars(_tmp5DB,inflow,_tmp5E3,_tmp5DB->unknown_none,decl->loc,0);_LL392: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5E4=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5DC;if(_tmp5E4->tag != 1)goto _LL394;else{_tmp5E5=_tmp5E4->f1;}}_LL393:
# 2328
 Cyc_NewControlFlow_check_nested_fun(_tmp5DB,inflow,_tmp5E5);{
void*t=(void*)_check_null(_tmp5E5->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5FB=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp5E5->fn_vardecl);
# 2334
struct Cyc_List_List*_tmp8A7;return Cyc_NewControlFlow_add_vars(_tmp5DB,inflow,((_tmp8A7=_region_malloc(env->r,sizeof(*_tmp8A7)),((_tmp8A7->hd=_tmp5FB,((_tmp8A7->tl=0,_tmp8A7)))))),_tmp5DB->unknown_all,decl->loc,0);};_LL394: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp5E6=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5DC;if(_tmp5E6->tag != 4)goto _LL396;else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;_tmp5E9=_tmp5E6->f3;_tmp5EA=_tmp5E6->f4;}}_LL395:
# 2337
 if(_tmp5EA != 0){
struct Cyc_Absyn_Exp*_tmp5FD=_tmp5EA;
union Cyc_CfFlowInfo_FlowInfo _tmp5FF;void*_tmp600;struct _tuple14 _tmp5FE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5FD);_tmp5FF=_tmp5FE.f1;_tmp600=_tmp5FE.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp5FD->loc,_tmp5FF,_tmp600);}{
# 2342
struct Cyc_List_List _tmp8A8;struct Cyc_List_List _tmp601=(_tmp8A8.hd=_tmp5E8,((_tmp8A8.tl=0,_tmp8A8)));
return Cyc_NewControlFlow_add_vars(_tmp5DB,inflow,& _tmp601,_tmp5DB->unknown_all,decl->loc,0);};_LL396:;_LL397: {
# 2345
struct Cyc_Core_Impossible_exn_struct _tmp8AE;const char*_tmp8AD;struct Cyc_Core_Impossible_exn_struct*_tmp8AC;(int)_throw((void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AE.tag=Cyc_Core_Impossible,((_tmp8AE.f1=((_tmp8AD="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8AD,sizeof(char),35))),_tmp8AE)))),_tmp8AC)))));}_LL38B:;}
# 2385 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp606=_new_region("frgn");struct _RegionHandle*frgn=& _tmp606;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2387
;_pop_region(frgn);}
# 2392
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2396
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2400
struct _RegionHandle*_tmp607=fenv->r;
unsigned int _tmp608=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp608,1);{
# 2406
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp609=inflow;struct Cyc_Dict_Dict _tmp60A;struct Cyc_List_List*_tmp60B;_LL39E: if((_tmp609.BottomFL).tag != 1)goto _LL3A0;_LL39F: {
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8B1="check_fun",_tag_dyneither(_tmp8B1,sizeof(char),10))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}_LL3A0: if((_tmp609.ReachableFL).tag != 2)goto _LL39D;_tmp60A=((struct _tuple13)(_tmp609.ReachableFL).val).f1;_tmp60B=((struct _tuple13)(_tmp609.ReachableFL).val).f2;_LL3A1: {
# 2413
struct Cyc_List_List*atts;
{void*_tmp60E=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp610;_LL3A3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 9)goto _LL3A5;else{_tmp610=(_tmp60F->f1).attributes;}}_LL3A4:
 atts=_tmp610;goto _LL3A2;_LL3A5:;_LL3A6: {
const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8B4="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8B4,sizeof(char),50))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}_LL3A2:;}
# 2418
for(0;atts != 0;atts=atts->tl){
void*_tmp613=(void*)atts->hd;int _tmp615;int _tmp617;int _tmp619;_LL3A8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp614=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp613;if(_tmp614->tag != 21)goto _LL3AA;else{_tmp615=_tmp614->f1;}}_LL3A9: {
# 2421
unsigned int j=(unsigned int)_tmp615;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,Cyc_Tcutil_terr(_tmp608,((_tmp8B7="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8B7,sizeof(char),52))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}
continue;}{
# 2427
struct Cyc_Absyn_Vardecl*_tmp61C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp61C->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,Cyc_Tcutil_terr(_tmp608,((_tmp8BA="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8BA,sizeof(char),52))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
continue;}{
# 2433
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp61F=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8BD;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8BC;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp620=(_tmp8BC=_region_malloc(_tmp607,sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=2,((_tmp8BD.f1=(int)j,((_tmp8BD.f2=t,_tmp8BD)))))),_tmp8BC)));
struct Cyc_CfFlowInfo_Place*_tmp8BE;struct Cyc_CfFlowInfo_Place*_tmp621=(_tmp8BE=_region_malloc(_tmp607,sizeof(*_tmp8BE)),((_tmp8BE->root=(void*)_tmp620,((_tmp8BE->fields=0,_tmp8BE)))));
_tmp60A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp60A,(void*)_tmp620,_tmp61F);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8C4;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8C3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8C1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8C0;_tmp60A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp60A,(void*)((_tmp8C0=_region_malloc(_tmp607,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=0,((_tmp8C1.f1=_tmp61C,_tmp8C1)))),_tmp8C0)))),(void*)((_tmp8C3=_region_malloc(_tmp607,sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C4.tag=5,((_tmp8C4.f1=_tmp621,_tmp8C4)))),_tmp8C3)))));}
goto _LL3A7;};};}_LL3AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp616=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp613;if(_tmp616->tag != 20)goto _LL3AC;else{_tmp617=_tmp616->f1;}}_LL3AB: {
# 2443
unsigned int j=(unsigned int)_tmp617;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,Cyc_Tcutil_terr(_tmp608,((_tmp8C7="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp8C7,sizeof(char),51))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}
# 2447
continue;}{
# 2449
struct Cyc_Absyn_Vardecl*_tmp62B=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp62C=Cyc_Tcutil_compress(_tmp62B->type);void*_tmp62E;union Cyc_Absyn_Constraint*_tmp62F;union Cyc_Absyn_Constraint*_tmp630;union Cyc_Absyn_Constraint*_tmp631;_LL3B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp62D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C;if(_tmp62D->tag != 5)goto _LL3B3;else{_tmp62E=(_tmp62D->f1).elt_typ;_tmp62F=((_tmp62D->f1).ptr_atts).nullable;_tmp630=((_tmp62D->f1).ptr_atts).bounds;_tmp631=((_tmp62D->f1).ptr_atts).zero_term;}}_LL3B2:
# 2452
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp62F)){
const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,Cyc_Tcutil_terr(_tmp608,((_tmp8CA="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp8CA,sizeof(char),55))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}
# 2455
if(!Cyc_Tcutil_is_bound_one(_tmp630)){
const char*_tmp8CD;void*_tmp8CC;(_tmp8CC=0,Cyc_Tcutil_terr(_tmp608,((_tmp8CD="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp8CD,sizeof(char),57))),_tag_dyneither(_tmp8CC,sizeof(void*),0)));}
# 2458
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp631)){
const char*_tmp8D0;void*_tmp8CF;(_tmp8CF=0,Cyc_Tcutil_terr(_tmp608,((_tmp8D0="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp8D0,sizeof(char),77))),_tag_dyneither(_tmp8CF,sizeof(void*),0)));}{
# 2461
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8D3;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8D2;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp638=(_tmp8D2=_region_malloc(_tmp607,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=2,((_tmp8D3.f1=(int)j,((_tmp8D3.f2=_tmp62E,_tmp8D3)))))),_tmp8D2)));
struct Cyc_CfFlowInfo_Place*_tmp8D4;struct Cyc_CfFlowInfo_Place*_tmp639=(_tmp8D4=_region_malloc(_tmp607,sizeof(*_tmp8D4)),((_tmp8D4->root=(void*)_tmp638,((_tmp8D4->fields=0,_tmp8D4)))));
_tmp60A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp60A,(void*)_tmp638,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp62E,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8DA;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8D9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8D7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8D6;_tmp60A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp60A,(void*)((_tmp8D6=_region_malloc(_tmp607,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=0,((_tmp8D7.f1=_tmp62B,_tmp8D7)))),_tmp8D6)))),(void*)((_tmp8D9=_region_malloc(_tmp607,sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=5,((_tmp8DA.f1=_tmp639,_tmp8DA)))),_tmp8D9)))));}
{struct Cyc_List_List*_tmp8DB;param_roots=((_tmp8DB=_region_malloc(_tmp607,sizeof(*_tmp8DB)),((_tmp8DB->hd=_tmp639,((_tmp8DB->tl=param_roots,_tmp8DB))))));}
goto _LL3B0;};_LL3B3:;_LL3B4: {
const char*_tmp8DE;void*_tmp8DD;(_tmp8DD=0,Cyc_Tcutil_terr(_tmp608,((_tmp8DE="initializes attribute on non-pointer",_tag_dyneither(_tmp8DE,sizeof(char),37))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}_LL3B0:;}
# 2469
goto _LL3A7;};}_LL3AC: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp618=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp613;if(_tmp618->tag != 22)goto _LL3AE;else{_tmp619=_tmp618->f1;}}_LL3AD: {
# 2471
unsigned int j=(unsigned int)_tmp619;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8E1;void*_tmp8E0;(_tmp8E0=0,Cyc_Tcutil_terr(_tmp608,((_tmp8E1="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp8E1,sizeof(char),49))),_tag_dyneither(_tmp8E0,sizeof(void*),0)));}
continue;}{
# 2476
struct Cyc_Absyn_Vardecl*_tmp646=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp646->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp8E4;void*_tmp8E3;(_tmp8E3=0,Cyc_Tcutil_terr(_tmp608,((_tmp8E4="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp8E4,sizeof(char),49))),_tag_dyneither(_tmp8E3,sizeof(void*),0)));}
continue;}{
# 2483
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8E7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8E6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp649=(_tmp8E6=_region_malloc(_tmp607,sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=0,((_tmp8E7.f1=_tmp646,_tmp8E7)))),_tmp8E6)));
struct Cyc_CfFlowInfo_Place*_tmp8E8;struct Cyc_CfFlowInfo_Place*_tmp64A=(_tmp8E8=_region_malloc(_tmp607,sizeof(*_tmp8E8)),((_tmp8E8->root=(void*)_tmp649,((_tmp8E8->fields=0,_tmp8E8)))));
void*_tmp64B=Cyc_CfFlowInfo_lookup_place(_tmp60A,_tmp64A);
{struct _tuple24*_tmp8EB;struct Cyc_List_List*_tmp8EA;noconsume_rvals=((_tmp8EA=_region_malloc(_tmp607,sizeof(*_tmp8EA)),((_tmp8EA->hd=((_tmp8EB=_region_malloc(_tmp607,sizeof(*_tmp8EB)),((_tmp8EB->f1=_tmp64A,((_tmp8EB->f2=_tmp64B,_tmp8EB)))))),((_tmp8EA->tl=noconsume_rvals,_tmp8EA))))));}
# 2488
goto _LL3A7;};};}_LL3AE:;_LL3AF:
 goto _LL3A7;_LL3A7:;}
# 2491
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp60A,_tmp60B);}_LL39D:;}{
# 2494
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp607,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp8EC;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp8EC=_region_malloc(_tmp607,sizeof(*_tmp8EC)),((_tmp8EC->r=_tmp607,((_tmp8EC->fenv=fenv,((_tmp8EC->iterate_again=1,((_tmp8EC->iteration_num=0,((_tmp8EC->in_try=0,((_tmp8EC->tryflow=inflow,((_tmp8EC->all_changed=0,((_tmp8EC->noreturn=noreturn,((_tmp8EC->param_roots=param_roots,((_tmp8EC->noconsume_params=noconsume_rvals,((_tmp8EC->flow_table=flow_table,_tmp8EC)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2504
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2507
union Cyc_CfFlowInfo_FlowInfo _tmp651=outflow;_LL3B6: if((_tmp651.BottomFL).tag != 1)goto _LL3B8;_LL3B7:
 goto _LL3B5;_LL3B8:;_LL3B9:
# 2510
 Cyc_NewControlFlow_check_init_params(_tmp608,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp608,env,outflow);
# 2514
if(noreturn){
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,Cyc_Tcutil_terr(_tmp608,((_tmp8EF="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp8EF,sizeof(char),46))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}else{
# 2517
void*_tmp654=Cyc_Tcutil_compress(fd->ret_type);_LL3BB: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp655=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp654;if(_tmp655->tag != 0)goto _LL3BD;}_LL3BC:
 goto _LL3BA;_LL3BD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp656=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp654;if(_tmp656->tag != 7)goto _LL3BF;}_LL3BE:
 goto _LL3C0;_LL3BF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp654;if(_tmp657->tag != 6)goto _LL3C1;}_LL3C0:
# 2521
{const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,Cyc_Tcutil_warn(_tmp608,((_tmp8F2="function may not return a value",_tag_dyneither(_tmp8F2,sizeof(char),32))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}goto _LL3BA;_LL3C1:;_LL3C2:
# 2523
{const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,Cyc_Tcutil_terr(_tmp608,((_tmp8F5="function may not return a value",_tag_dyneither(_tmp8F5,sizeof(char),32))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}goto _LL3BA;_LL3BA:;}
# 2525
goto _LL3B5;_LL3B5:;};};};}
# 2530
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp65D=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp65F;struct Cyc_List_List*_tmp661;struct Cyc_List_List*_tmp663;struct Cyc_List_List*_tmp665;_LL3C4: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp65E=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp65D;if(_tmp65E->tag != 1)goto _LL3C6;else{_tmp65F=_tmp65E->f1;}}_LL3C5:
 Cyc_NewControlFlow_check_fun(_tmp65F);goto _LL3C3;_LL3C6: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp660=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp65D;if(_tmp660->tag != 12)goto _LL3C8;else{_tmp661=_tmp660->f1;}}_LL3C7:
 _tmp663=_tmp661;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp662=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp65D;if(_tmp662->tag != 11)goto _LL3CA;else{_tmp663=_tmp662->f2;}}_LL3C9:
 _tmp665=_tmp663;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp664=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp65D;if(_tmp664->tag != 10)goto _LL3CC;else{_tmp665=_tmp664->f2;}}_LL3CB:
 Cyc_NewControlFlow_cf_check(_tmp665);goto _LL3C3;_LL3CC: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp666=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp65D;if(_tmp666->tag != 13)goto _LL3CE;}_LL3CD:
 goto _LL3C3;_LL3CE:;_LL3CF:
 goto _LL3C3;_LL3C3:;}}
