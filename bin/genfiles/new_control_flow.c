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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 137
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 183
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 186
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 192
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 195
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 217 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 220
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 227
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 231
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 236
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,struct _tuple14 pr2);
# 242
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256
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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp69A;struct Cyc_NewControlFlow_CFStmtAnnot _tmp699;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp698;enclosee->annot=(void*)((_tmp698=_cycalloc(sizeof(*_tmp698)),((_tmp698[0]=((_tmp69A.tag=Cyc_NewControlFlow_CFAnnot,((_tmp69A.f1=((_tmp699.encloser=encloser,((_tmp699.visited=0,_tmp699)))),_tmp69A)))),_tmp698))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp6A0;const char*_tmp69F;struct Cyc_Core_Impossible_exn_struct*_tmp69E;(int)_throw((void*)((_tmp69E=_cycalloc(sizeof(*_tmp69E)),((_tmp69E[0]=((_tmp6A0.tag=Cyc_Core_Impossible,((_tmp6A0.f1=((_tmp69F="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp69F,sizeof(char),37))),_tmp6A0)))),_tmp69E)))));}_LL0:;}
# 128
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp6A1;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp6A1=_region_malloc(env->r,sizeof(*_tmp6A1)),((_tmp6A1[0]=Cyc_CfFlowInfo_BottomFL(),_tmp6A1)));
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
struct _tuple17 _tmp6A2;return(_tmp6A2.f1=_tmpA,((_tmp6A2.f2=_tmpB,_tmp6A2)));};}
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
{void*_tmp14=rval;_LLB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp15=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp15->tag != 7)goto _LLD;}_LLC:
 goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp16=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp16->tag != 3)goto _LLF;else{if(_tmp16->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp14;if(_tmp17->tag != 0)goto _LL11;}_LL10:
 goto _LLA;_LL11:;_LL12:
# 184
{const char*_tmp6A6;void*_tmp6A5[1];struct Cyc_String_pa_PrintArg_struct _tmp6A4;(_tmp6A4.tag=0,((_tmp6A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6A5[0]=& _tmp6A4,Cyc_Tcutil_warn(_tmpF,((_tmp6A6="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp6A6,sizeof(char),60))),_tag_dyneither(_tmp6A5,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}
# 187
goto _LL5;}
# 189
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 197
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1B=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1C=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1D=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1C);
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple16 _tmp6A7;struct _tuple16 _tmp1F=(_tmp6A7.f1=flow,((_tmp6A7.f2=_tmp1D,_tmp6A7)));struct Cyc_Dict_Dict _tmp20;struct Cyc_Dict_Dict _tmp21;_LL14: if(((_tmp1F.f1).ReachableFL).tag != 2)goto _LL16;_tmp20=((struct _tuple13)((_tmp1F.f1).ReachableFL).val).f1;if(((_tmp1F.f2).ReachableFL).tag != 2)goto _LL16;_tmp21=((struct _tuple13)((_tmp1F.f2).ReachableFL).val).f1;_LL15: {
# 204
struct _tuple18 _tmp6A8;struct _tuple18 _tmp22=(_tmp6A8.f1=env,((_tmp6A8.f2=s->loc,((_tmp6A8.f3=_tmp21,_tmp6A8)))));
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp22,_tmp20);
goto _LL13;}_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 212
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1D,*_tmp1C)){
# 219
*_tmp1C=_tmp1D;
# 223
if(_tmp1B->visited == env->iteration_num)
# 225
env->iterate_again=1;}}
# 230
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc){
# 233
union Cyc_CfFlowInfo_FlowInfo _tmp24=inflow;struct Cyc_Dict_Dict _tmp25;struct Cyc_List_List*_tmp26;_LL19: if((_tmp24.BottomFL).tag != 1)goto _LL1B;_LL1A:
 return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp24.ReachableFL).tag != 2)goto _LL18;_tmp25=((struct _tuple13)(_tmp24.ReachableFL).val).f1;_tmp26=((struct _tuple13)(_tmp24.ReachableFL).val).f2;_LL1C:
# 236
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6AB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6AA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp27=(_tmp6AA=_region_malloc(fenv->r,sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=0,((_tmp6AB.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp6AB)))),_tmp6AA)));
# 239
_tmp25=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25,(void*)_tmp27,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval));}
# 241
return Cyc_CfFlowInfo_ReachableFL(_tmp25,_tmp26);_LL18:;}
# 245
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp2A=inflow;struct Cyc_Dict_Dict _tmp2B;struct Cyc_List_List*_tmp2C;_LL1E: if((_tmp2A.BottomFL).tag != 1)goto _LL20;_LL1F:
 return Cyc_CfFlowInfo_BottomFL();_LL20: if((_tmp2A.ReachableFL).tag != 2)goto _LL1D;_tmp2B=((struct _tuple13)(_tmp2A.ReachableFL).val).f1;_tmp2C=((struct _tuple13)(_tmp2A.ReachableFL).val).f2;_LL21:
# 249
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2B,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp6AE;void*_tmp6AD;(_tmp6AD=0,Cyc_Tcutil_terr(loc,((_tmp6AE="expression may not be fully initialized",_tag_dyneither(_tmp6AE,sizeof(char),40))),_tag_dyneither(_tmp6AD,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp2F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2B,env->all_changed,r);
if(_tmp2B.t == _tmp2F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp30=Cyc_CfFlowInfo_ReachableFL(_tmp2F,_tmp2C);
Cyc_NewControlFlow_update_tryflow(env,_tmp30);
return _tmp30;};};_LL1D:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 259
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6AF;struct _tuple0 _tmp32=(_tmp6AF.f1=Cyc_Tcutil_compress(t),((_tmp6AF.f2=r,_tmp6AF)));struct Cyc_Absyn_Datatypefield*_tmp37;struct _dyneither_ptr _tmp39;struct Cyc_List_List*_tmp3B;struct _dyneither_ptr _tmp3D;union Cyc_Absyn_AggrInfoU _tmp3F;struct Cyc_List_List*_tmp40;struct _dyneither_ptr _tmp42;enum Cyc_Absyn_AggrKind _tmp44;struct Cyc_List_List*_tmp45;struct _dyneither_ptr _tmp47;_LL23: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp33=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp33->tag != 3)goto _LL25;else{if(_tmp33->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL25;}}_LL24:
 return;_LL25: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp34=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp34->tag != 0)goto _LL27;}_LL26:
 return;_LL27: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp35=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp35->tag != 7)goto _LL29;}_LL28:
 return;_LL29:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp32.f1;if(_tmp36->tag != 4)goto _LL2B;else{if((((_tmp36->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B;_tmp37=((struct _tuple2)(((_tmp36->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp38=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp38->tag != 6)goto _LL2B;else{_tmp39=_tmp38->f2;}};_LL2A:
# 265
 if(_tmp37->typs == 0)
return;
_tmp3B=_tmp37->typs;_tmp3D=_tmp39;goto _LL2C;_LL2B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp32.f1;if(_tmp3A->tag != 10)goto _LL2D;else{_tmp3B=_tmp3A->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp3C->tag != 6)goto _LL2D;else{_tmp3D=_tmp3C->f2;}};_LL2C: {
# 269
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3B);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp3D,sizeof(void*),i)));}}
# 273
return;}_LL2D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp32.f1;if(_tmp3E->tag != 11)goto _LL2F;else{_tmp3F=(_tmp3E->f1).aggr_info;_tmp40=(_tmp3E->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp41=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp41->tag != 6)goto _LL2F;else{_tmp42=_tmp41->f2;}};_LL2E: {
# 276
struct Cyc_Absyn_Aggrdecl*_tmp49=Cyc_Absyn_get_known_aggrdecl(_tmp3F);
if(_tmp49->impl == 0)return;{
struct Cyc_List_List*_tmp4A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49->impl))->fields;
struct _RegionHandle _tmp4B=_new_region("temp");struct _RegionHandle*temp=& _tmp4B;_push_region(temp);
{struct Cyc_List_List*_tmp4C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp49->tvs,_tmp40);
{int i=0;for(0;i < _get_dyneither_size(_tmp42,sizeof(void*));(i ++,_tmp4A=_tmp4A->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp4A))->hd)->type;
if(_tmp4C != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp4C,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp42.curr)[i]);}}
# 286
_npop_handler(0);return;}
# 280
;_pop_region(temp);};}_LL2F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32.f1;if(_tmp43->tag != 12)goto _LL31;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp46=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32.f2;if(_tmp46->tag != 6)goto _LL31;else{_tmp47=_tmp46->f2;}};_LL30:
# 289
{int i=0;for(0;i < _get_dyneither_size(_tmp47,sizeof(void*));(i ++,_tmp45=_tmp45->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp45))->hd)->type,((void**)_tmp47.curr)[i]);}}
return;_LL31:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32.f1;if(_tmp48->tag != 5)goto _LL33;}if(!
Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:
{const char*_tmp6B2;void*_tmp6B1;(_tmp6B1=0,Cyc_Tcutil_warn(loc,((_tmp6B2="argument may still contain unique pointers",_tag_dyneither(_tmp6B2,sizeof(char),43))),_tag_dyneither(_tmp6B1,sizeof(void*),0)));}
return;_LL33:;_LL34:
 return;_LL22:;}
# 299
static void Cyc_NewControlFlow_check_unique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6B3;struct _tuple0 _tmp50=(_tmp6B3.f1=Cyc_Tcutil_compress(t),((_tmp6B3.f2=r,_tmp6B3)));struct Cyc_Absyn_Datatypefield*_tmp55;struct _dyneither_ptr _tmp57;struct Cyc_List_List*_tmp59;struct _dyneither_ptr _tmp5B;union Cyc_Absyn_AggrInfoU _tmp5D;struct Cyc_List_List*_tmp5E;struct _dyneither_ptr _tmp60;enum Cyc_Absyn_AggrKind _tmp62;struct Cyc_List_List*_tmp63;struct _dyneither_ptr _tmp65;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp51=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp51->tag != 3)goto _LL38;else{if(_tmp51->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp52=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp52->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp53=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp53->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp50.f1;if(_tmp54->tag != 4)goto _LL3E;else{if((((_tmp54->f1).field_info).KnownDatatypefield).tag != 2)goto _LL3E;_tmp55=((struct _tuple2)(((_tmp54->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp56=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp56->tag != 6)goto _LL3E;else{_tmp57=_tmp56->f2;}};_LL3D:
# 305
 if(_tmp55->typs == 0)
return;
_tmp59=_tmp55->typs;_tmp5B=_tmp57;goto _LL3F;_LL3E:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp58=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp50.f1;if(_tmp58->tag != 10)goto _LL40;else{_tmp59=_tmp58->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp5A->tag != 6)goto _LL40;else{_tmp5B=_tmp5A->f2;}};_LL3F: {
# 309
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp59);
{int i=0;for(0;i < sz;++ i){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp59))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp5B,sizeof(void*),i)));}}
# 313
return;}_LL40:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp50.f1;if(_tmp5C->tag != 11)goto _LL42;else{_tmp5D=(_tmp5C->f1).aggr_info;_tmp5E=(_tmp5C->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp5F->tag != 6)goto _LL42;else{_tmp60=_tmp5F->f2;}};_LL41: {
# 316
struct Cyc_Absyn_Aggrdecl*_tmp67=Cyc_Absyn_get_known_aggrdecl(_tmp5D);
if(_tmp67->impl == 0)return;{
struct Cyc_List_List*_tmp68=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->fields;
struct _RegionHandle _tmp69=_new_region("temp");struct _RegionHandle*temp=& _tmp69;_push_region(temp);
{struct Cyc_List_List*_tmp6A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp67->tvs,_tmp5E);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp68=_tmp68->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp68))->hd)->type;
if(_tmp6A != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp6A,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 326
_npop_handler(0);return;}
# 320
;_pop_region(temp);};}_LL42:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50.f1;if(_tmp61->tag != 12)goto _LL44;else{_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp50.f2;if(_tmp64->tag != 6)goto _LL44;else{_tmp65=_tmp64->f2;}};_LL43:
# 329
{int i=0;for(0;i < _get_dyneither_size(_tmp65,sizeof(void*));(i ++,_tmp63=_tmp63->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp63))->hd)->type,((void**)_tmp65.curr)[i]);}}
return;_LL44:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1;if(_tmp66->tag != 5)goto _LL46;}if(!
Cyc_Tcutil_is_noalias_pointer(t))goto _LL46;_LL45:
{const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,Cyc_Tcutil_warn(loc,((_tmp6B6="argument may still contain unique pointers",_tag_dyneither(_tmp6B6,sizeof(char),43))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}
return;_LL46:;_LL47:
 return;_LL35:;}
# 339
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp6D=inflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;_LL49: if((_tmp6D.BottomFL).tag != 1)goto _LL4B;_LL4A:
 return Cyc_CfFlowInfo_BottomFL();_LL4B: if((_tmp6D.ReachableFL).tag != 2)goto _LL48;_tmp6E=((struct _tuple13)(_tmp6D.ReachableFL).val).f1;_tmp6F=((struct _tuple13)(_tmp6D.ReachableFL).val).f2;_LL4C:
# 343
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,Cyc_Tcutil_terr(loc,((_tmp6B9="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp6B9,sizeof(char),47))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 347
void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);
{void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL4E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp74=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp73;if(_tmp74->tag != 5)goto _LL50;else{_tmp75=_tmp74->f1;}}_LL4F:
# 350
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));
goto _LL4D;_LL50:;_LL51:
# 353
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp72)){
const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_Tcutil_warn(loc,((_tmp6BC="argument may contain live unique pointers",_tag_dyneither(_tmp6BC,sizeof(char),42))),_tag_dyneither(_tmp6BB,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4D:;}{
# 357
struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6E,env->all_changed,r);
if(_tmp6E.t == _tmp78.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);
Cyc_NewControlFlow_update_tryflow(env,_tmp79);
return _tmp79;};};};_LL48:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 366
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 373
if(es == 0){
struct _tuple20 _tmp6BD;return(_tmp6BD.f1=inflow,((_tmp6BD.f2=0,_tmp6BD)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple14 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;{
struct Cyc_List_List*_tmp6C0;struct _tuple20 _tmp6BF;return(_tmp6BF.f1=_tmp7C,((_tmp6BF.f2=((_tmp6C0=_region_malloc(rgn,sizeof(*_tmp6C0)),((_tmp6C0->hd=_tmp7D,((_tmp6C0->tl=0,_tmp6C0)))))),_tmp6BF)));};}{
# 379
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 387
{struct Cyc_Dict_Dict*_tmp6C1;env->all_changed=((_tmp6C1=_region_malloc(env->r,sizeof(*_tmp6C1)),((_tmp6C1[0]=(env->fenv)->mt_place_set,_tmp6C1))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple14 _tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;
outflow=_tmp82;
{struct Cyc_List_List*_tmp6C2;rvals=((_tmp6C2=_region_malloc(rgn,sizeof(*_tmp6C2)),((_tmp6C2->hd=_tmp83,((_tmp6C2->tl=0,_tmp6C2))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 393
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6C3;env->all_changed=((_tmp6C3=_region_malloc(env->r,sizeof(*_tmp6C3)),((_tmp6C3[0]=(env->fenv)->mt_place_set,_tmp6C3))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp87;void*_tmp88;struct _tuple14 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;
{struct Cyc_List_List*_tmp6C4;rvals=((_tmp6C4=_region_malloc(rgn,sizeof(*_tmp6C4)),((_tmp6C4->hd=_tmp88,((_tmp6C4->tl=rvals,_tmp6C4))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 399
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 402
old_inflow=inflow;
# 405
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 410
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 414
struct Cyc_Dict_Dict*_tmp6C5;env->all_changed=((_tmp6C5=_region_malloc(env->r,sizeof(*_tmp6C5)),((_tmp6C5[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6C5))));}
# 416
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple20 _tmp6C6;return(_tmp6C6.f1=outflow,((_tmp6C6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6C6)));};};}
# 422
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 425
struct _RegionHandle _tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;_push_region(rgn);
{union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*_tmp8F;struct _tuple20 _tmp8D=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp90=_tmp8E;struct Cyc_Dict_Dict _tmp91;_LL53: if((_tmp90.ReachableFL).tag != 2)goto _LL55;_tmp91=((struct _tuple13)(_tmp90.ReachableFL).val).f1;_LL54:
# 430
 for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp91,(void*)_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6C9="expression may not be initialized",_tag_dyneither(_tmp6C9,sizeof(char),34))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}}
goto _LL52;_LL55: if((_tmp90.BottomFL).tag != 1)goto _LL52;_LL56:
 goto _LL52;_LL52:;}{
# 436
struct _tuple14 _tmp6CA;struct _tuple14 _tmp95=(_tmp6CA.f1=_tmp8E,((_tmp6CA.f2=(env->fenv)->unknown_all,_tmp6CA)));_npop_handler(0);return _tmp95;};}
# 426
;_pop_region(rgn);}
# 450 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){
# 456
union Cyc_CfFlowInfo_FlowInfo _tmp96=outflow;struct Cyc_Dict_Dict _tmp97;struct Cyc_List_List*_tmp98;_LL58: if((_tmp96.BottomFL).tag != 1)goto _LL5A;_LL59:
 return outflow;_LL5A: if((_tmp96.ReachableFL).tag != 2)goto _LL57;_tmp97=((struct _tuple13)(_tmp96.ReachableFL).val).f1;_tmp98=((struct _tuple13)(_tmp96.ReachableFL).val).f2;_LL5B: {
# 459
union Cyc_CfFlowInfo_AbsLVal _tmp99=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp9A;_LL5D: if((_tmp99.UnknownL).tag != 2)goto _LL5F;_LL5E:
# 463
 return outflow;_LL5F: if((_tmp99.PlaceL).tag != 1)goto _LL5C;_tmp9A=(struct Cyc_CfFlowInfo_Place*)(_tmp99.PlaceL).val;_LL60: {
# 467
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmp9B=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp97,env->all_changed,_tmp9A,nzval),_tmp98);
# 472
return _tmp9B;}_LL5C:;}_LL57:;}
# 481
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){
# 484
union Cyc_CfFlowInfo_FlowInfo _tmp9C=outflow;struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9E;_LL62: if((_tmp9C.BottomFL).tag != 1)goto _LL64;_LL63: {
struct _tuple16 _tmp6CB;return(_tmp6CB.f1=outflow,((_tmp6CB.f2=outflow,_tmp6CB)));}_LL64: if((_tmp9C.ReachableFL).tag != 2)goto _LL61;_tmp9D=((struct _tuple13)(_tmp9C.ReachableFL).val).f1;_tmp9E=((struct _tuple13)(_tmp9C.ReachableFL).val).f2;_LL65: {
# 487
union Cyc_CfFlowInfo_AbsLVal _tmpA0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmpA1;_LL67: if((_tmpA0.UnknownL).tag != 2)goto _LL69;_LL68: {
struct _tuple16 _tmp6CC;return(_tmp6CC.f1=outflow,((_tmp6CC.f2=outflow,_tmp6CC)));}_LL69: if((_tmpA0.PlaceL).tag != 1)goto _LL66;_tmpA1=(struct Cyc_CfFlowInfo_Place*)(_tmpA0.PlaceL).val;_LL6A: {
# 490
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple16 _tmp6CD;return
(_tmp6CD.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,env->all_changed,_tmpA1,nzval),_tmp9E),((_tmp6CD.f2=
# 494
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,env->all_changed,_tmpA1,(env->fenv)->zero),_tmp9E),_tmp6CD)));}_LL66:;}_LL61:;}
# 500
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 506
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 510
void*_tmpA6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA8;union Cyc_Absyn_Constraint*_tmpA9;union Cyc_Absyn_Constraint*_tmpAA;_LL6C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA6;if(_tmpA7->tag != 5)goto _LL6E;else{_tmpA8=(_tmpA7->f1).elt_typ;_tmpA9=((_tmpA7->f1).ptr_atts).bounds;_tmpAA=((_tmpA7->f1).ptr_atts).zero_term;}}_LL6D: {
# 512
union Cyc_CfFlowInfo_FlowInfo _tmpAB=f;struct Cyc_Dict_Dict _tmpAC;struct Cyc_List_List*_tmpAD;_LL71: if((_tmpAB.BottomFL).tag != 1)goto _LL73;_LL72: {
# 514
struct _tuple14 _tmp6CE;return(_tmp6CE.f1=f,((_tmp6CE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA8,0,(env->fenv)->unknown_all),_tmp6CE)));}_LL73: if((_tmpAB.ReachableFL).tag != 2)goto _LL70;_tmpAC=((struct _tuple13)(_tmpAB.ReachableFL).val).f1;_tmpAD=((struct _tuple13)(_tmpAB.ReachableFL).val).f2;_LL74:
# 517
 if(Cyc_Tcutil_is_bound_one(_tmpA9)){
void*_tmpAF=r;struct Cyc_CfFlowInfo_Place*_tmpB3;enum Cyc_CfFlowInfo_InitLevel _tmpB6;_LL76: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpB0=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpB1=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL7A;}_LL79:
# 521
{void*_tmpB7=e->annot;struct Cyc_List_List*_tmpB9;_LL83: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpB8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpB7;if(_tmpB8->tag != Cyc_CfFlowInfo_NotZero)goto _LL85;else{_tmpB9=_tmpB8->f1;}}_LL84:
# 523
 if(!Cyc_CfFlowInfo_same_relns(_tmpAD,_tmpB9))goto _LL86;
goto _LL82;_LL85:;_LL86:
# 527
{void*_tmpBA=e->r;_LL88: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBA;if(_tmpBB->tag != 22)goto _LL8A;}_LL89:
# 529
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6D1;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6D0;e->annot=(void*)((_tmp6D0=_cycalloc(sizeof(*_tmp6D0)),((_tmp6D0[0]=((_tmp6D1.tag=Cyc_CfFlowInfo_NotZero,((_tmp6D1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpAD),_tmp6D1)))),_tmp6D0))));}
goto _LL87;_LL8A:;_LL8B:
# 532
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL87;_LL87:;}
# 535
goto _LL82;_LL82:;}
# 537
goto _LL75;_LL7A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpB2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpAF;if(_tmpB2->tag != 5)goto _LL7C;else{_tmpB3=_tmpB2->f1;}}_LL7B:
# 539
{void*_tmpBE=e->annot;struct Cyc_List_List*_tmpC0;_LL8D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpBF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpBE;if(_tmpBF->tag != Cyc_CfFlowInfo_NotZero)goto _LL8F;else{_tmpC0=_tmpBF->f1;}}_LL8E:
# 541
 if(!Cyc_CfFlowInfo_same_relns(_tmpAD,_tmpC0))goto _LL90;
goto _LL8C;_LL8F:;_LL90:
# 545
{void*_tmpC1=e->r;_LL92: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC1;if(_tmpC2->tag != 22)goto _LL94;}_LL93:
# 547
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6D4;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6D3;e->annot=(void*)((_tmp6D3=_cycalloc(sizeof(*_tmp6D3)),((_tmp6D3[0]=((_tmp6D4.tag=Cyc_CfFlowInfo_NotZero,((_tmp6D4.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpAD),_tmp6D4)))),_tmp6D3))));}
goto _LL91;_LL94:;_LL95:
# 550
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL91;_LL91:;}
# 553
goto _LL8C;_LL8C:;}{
# 555
struct _tuple14 _tmp6D5;return(_tmp6D5.f1=f,((_tmp6D5.f2=Cyc_CfFlowInfo_lookup_place(_tmpAC,_tmpB3),_tmp6D5)));};_LL7C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpB4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpAF;if(_tmpB4->tag != 0)goto _LL7E;}_LL7D:
# 557
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp6D6;return(_tmp6D6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6D6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA8,0,(env->fenv)->unknown_all),_tmp6D6)));};_LL7E: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpB5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpAF;if(_tmpB5->tag != 3)goto _LL80;else{_tmpB6=_tmpB5->f1;}}_LL7F:
# 560
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpB6);goto _LL81;_LL80:;_LL81:
# 562
{void*_tmpC7=e->r;_LL97: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC7;if(_tmpC8->tag != 22)goto _LL99;}_LL98:
# 564
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6D9;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6D8;e->annot=(void*)((_tmp6D8=_cycalloc(sizeof(*_tmp6D8)),((_tmp6D8[0]=((_tmp6D9.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6D9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpAD),_tmp6D9)))),_tmp6D8))));}
goto _LL96;_LL99:;_LL9A:
# 567
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL96;_LL96:;}
# 570
goto _LL75;_LL75:;}else{
# 573
void*_tmpCB=e->annot;struct Cyc_List_List*_tmpCD;_LL9C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpCB;if(_tmpCC->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL9E;else{_tmpCD=_tmpCC->f1;}}_LL9D:
# 575
 if(!Cyc_CfFlowInfo_same_relns(_tmpAD,_tmpCD))goto _LL9F;
goto _LL9B;_LL9E:;_LL9F:
# 578
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6DC;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6DB;e->annot=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6DC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpAD),_tmp6DC)))),_tmp6DB))));}
goto _LL9B;_LL9B:;}
# 582
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAC,r)){case Cyc_CfFlowInfo_NoneIL: _LLA0:
# 584
{void*_tmpD0=r;_LLA3: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpD1=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD0;if(_tmpD1->tag != 7)goto _LLA5;}_LLA4:
# 586
{const char*_tmp6DF;void*_tmp6DE;(_tmp6DE=0,Cyc_Tcutil_terr(e->loc,((_tmp6DF="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp6DF,sizeof(char),61))),_tag_dyneither(_tmp6DE,sizeof(void*),0)));}
goto _LLA2;_LLA5:;_LLA6: {
# 589
const char*_tmp6E2;void*_tmp6E1;(_tmp6E1=0,Cyc_Tcutil_terr(e->loc,((_tmp6E2="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6E2,sizeof(char),46))),_tag_dyneither(_tmp6E1,sizeof(void*),0)));}_LLA2:;}
# 591
goto _LLA1;case Cyc_CfFlowInfo_AllIL: _LLA1: {
# 593
struct _tuple14 _tmp6E3;return(_tmp6E3.f1=f,((_tmp6E3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA8,0,(env->fenv)->unknown_all),_tmp6E3)));}case Cyc_CfFlowInfo_ThisIL: _LLA7: {
# 595
struct _tuple14 _tmp6E4;return(_tmp6E4.f1=f,((_tmp6E4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA8,0,(env->fenv)->unknown_none),_tmp6E4)));}}_LL70:;}_LL6E:;_LL6F: {
# 598
struct Cyc_Core_Impossible_exn_struct _tmp6EA;const char*_tmp6E9;struct Cyc_Core_Impossible_exn_struct*_tmp6E8;(int)_throw((void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6EA.tag=Cyc_Core_Impossible,((_tmp6EA.f1=((_tmp6E9="right deref of non-pointer-type",_tag_dyneither(_tmp6E9,sizeof(char),32))),_tmp6EA)))),_tmp6E8)))));}_LL6B:;}
# 604
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 606
void*_tmpDB=e1->r;struct Cyc_Absyn_Vardecl*_tmpDE;struct Cyc_Absyn_Vardecl*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Vardecl*_tmpE7;_LLAA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDC->tag != 1)goto _LLAC;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpDD=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpDC->f2);if(_tmpDD->tag != 5)goto _LLAC;else{_tmpDE=_tmpDD->f1;}}}_LLAB:
 _tmpE1=_tmpDE;goto _LLAD;_LLAC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDF->tag != 1)goto _LLAE;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpDF->f2);if(_tmpE0->tag != 4)goto _LLAE;else{_tmpE1=_tmpE0->f1;}}}_LLAD:
 _tmpE4=_tmpE1;goto _LLAF;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE2->tag != 1)goto _LLB0;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE3=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpE2->f2);if(_tmpE3->tag != 3)goto _LLB0;else{_tmpE4=_tmpE3->f1;}}}_LLAF:
 _tmpE7=_tmpE4;goto _LLB1;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE5->tag != 1)goto _LLB2;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE6=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpE5->f2);if(_tmpE6->tag != 1)goto _LLB2;else{_tmpE7=_tmpE6->f1;}}}_LLB1:
# 611
 if(!_tmpE7->escapes){
void*_tmpE8=e2->r;struct Cyc_Absyn_Vardecl*_tmpEB;struct Cyc_Absyn_Vardecl*_tmpEE;struct Cyc_Absyn_Vardecl*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF4;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpE9->tag != 1)goto _LLB7;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpEA=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpE9->f2);if(_tmpEA->tag != 5)goto _LLB7;else{_tmpEB=_tmpEA->f1;}}}_LLB6:
 _tmpEE=_tmpEB;goto _LLB8;_LLB7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpEC->tag != 1)goto _LLB9;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpED=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpEC->f2);if(_tmpED->tag != 4)goto _LLB9;else{_tmpEE=_tmpED->f1;}}}_LLB8:
 _tmpF1=_tmpEE;goto _LLBA;_LLB9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpEF->tag != 1)goto _LLBB;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEF->f2);if(_tmpF0->tag != 3)goto _LLBB;else{_tmpF1=_tmpF0->f1;}}}_LLBA:
 _tmpF4=_tmpF1;goto _LLBC;_LLBB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE8;if(_tmpF2->tag != 1)goto _LLBD;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF3=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpF2->f2);if(_tmpF3->tag != 1)goto _LLBD;else{_tmpF4=_tmpF3->f1;}}}_LLBC:
# 617
 if(!_tmpF4->escapes){
# 619
int found=0;
{struct Cyc_List_List*_tmpF5=relns;for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
struct Cyc_CfFlowInfo_Reln*_tmpF6=(struct Cyc_CfFlowInfo_Reln*)_tmpF5->hd;
if(_tmpF6->vd == _tmpF4){
union Cyc_CfFlowInfo_RelnOp _tmpF7=_tmpF6->rop;struct Cyc_Absyn_Vardecl*_tmpF8;_LLC0: if((_tmpF7.LessNumelts).tag != 3)goto _LLC2;_tmpF8=(struct Cyc_Absyn_Vardecl*)(_tmpF7.LessNumelts).val;if(!(_tmpF8 == _tmpE7))goto _LLC2;_LLC1:
 return relns;_LLC2:;_LLC3:
 continue;_LLBF:;}}}
# 628
if(!found){
struct Cyc_CfFlowInfo_Reln*_tmp6ED;struct Cyc_List_List*_tmp6EC;return(_tmp6EC=_region_malloc(rgn,sizeof(*_tmp6EC)),((_tmp6EC->hd=((_tmp6ED=_region_malloc(rgn,sizeof(*_tmp6ED)),((_tmp6ED->vd=_tmpF4,((_tmp6ED->rop=Cyc_CfFlowInfo_LessNumelts(_tmpE7),_tmp6ED)))))),((_tmp6EC->tl=relns,_tmp6EC)))));}}
# 631
return relns;_LLBD:;_LLBE:
 return relns;_LLB4:;}else{
# 634
return relns;}_LLB2:;_LLB3:
 return relns;_LLA9:;}
# 641
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 649
union Cyc_CfFlowInfo_FlowInfo _tmpFB=outflow;struct Cyc_Dict_Dict _tmpFC;struct Cyc_List_List*_tmpFD;_LLC5: if((_tmpFB.BottomFL).tag != 1)goto _LLC7;_LLC6: {
# 652
struct _tuple14 _tmp6EE;return(_tmp6EE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6EE.f2=rval,_tmp6EE)));}_LLC7: if((_tmpFB.ReachableFL).tag != 2)goto _LLC4;_tmpFC=((struct _tuple13)(_tmpFB.ReachableFL).val).f1;_tmpFD=((struct _tuple13)(_tmpFB.ReachableFL).val).f2;_LLC8: {
# 654
union Cyc_CfFlowInfo_AbsLVal _tmpFF=lval;struct Cyc_CfFlowInfo_Place*_tmp100;_LLCA: if((_tmpFF.PlaceL).tag != 1)goto _LLCC;_tmp100=(struct Cyc_CfFlowInfo_Place*)(_tmpFF.PlaceL).val;_LLCB: {
# 656
struct Cyc_Dict_Dict _tmp101=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpFC,env->all_changed,_tmp100,rval);
_tmpFD=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmpFD,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp101,_tmpFD);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 662
void*_tmp102=Cyc_CfFlowInfo_lookup_place(_tmpFC,_tmp100);_LLCF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp103=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp102;if(_tmp103->tag != 3)goto _LLD1;else{if(_tmp103->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLD1;}}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp104=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp102;if(_tmp104->tag != 7)goto _LLD3;}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp105=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp102;if(_tmp105->tag != 0)goto _LLD5;}_LLD4:
 goto _LLCE;_LLD5:;_LLD6:
# 667
{const char*_tmp6F1;void*_tmp6F0;(_tmp6F0=0,Cyc_Tcutil_warn(lexp->loc,((_tmp6F1="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp6F1,sizeof(char),43))),_tag_dyneither(_tmp6F0,sizeof(void*),0)));}
goto _LLCE;_LLCE:;}
# 672
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 674
struct _tuple14 _tmp6F2;return(_tmp6F2.f1=outflow,((_tmp6F2.f2=rval,_tmp6F2)));};}_LLCC: if((_tmpFF.UnknownL).tag != 2)goto _LLC9;_LLCD: {
# 677
struct _tuple14 _tmp6F3;return(_tmp6F3.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp6F3.f2=rval,_tmp6F3)));}_LLC9:;}_LLC4:;}
# 682
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
void*_tmp10A=e->r;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;_LLD8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp10B->tag != 1)goto _LLDA;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp10C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp10B->f2);if(_tmp10C->tag != 4)goto _LLDA;}}_LLD9:
 goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp10D->tag != 1)goto _LLDC;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp10E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp10D->f2);if(_tmp10E->tag != 3)goto _LLDC;}}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp10F->tag != 1)goto _LLDE;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp110=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp10F->f2);if(_tmp110->tag != 5)goto _LLDE;}}_LLDD:
 return 1;_LLDE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp111->tag != 19)goto _LLE0;else{_tmp112=_tmp111->f1;}}_LLDF:
 _tmp114=_tmp112;goto _LLE1;_LLE0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp113->tag != 20)goto _LLE2;else{_tmp114=_tmp113->f1;}}_LLE1:
 _tmp116=_tmp114;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp115->tag != 21)goto _LLE4;else{_tmp116=_tmp115->f1;}}_LLE3:
# 690
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp116);_LLE4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp117->tag != 22)goto _LLE6;else{_tmp118=_tmp117->f1;}}_LLE5: {
# 692
void*_tmp11B=Cyc_Tcutil_compress((void*)_check_null(_tmp118->topt));_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11B;if(_tmp11C->tag != 10)goto _LLED;}_LLEC:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp118);_LLED:;_LLEE:
 return 0;_LLEA:;}_LLE6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp119->tag != 13)goto _LLE8;else{_tmp11A=_tmp119->f2;}}_LLE7:
# 696
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp11A);_LLE8:;_LLE9:
 return 0;_LLD7:;}
# 701
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 703
struct Cyc_CfFlowInfo_FlowEnv*_tmp11D=env->fenv;
# 707
while(1){
union Cyc_CfFlowInfo_FlowInfo*_tmp11F;struct _tuple17 _tmp11E=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp11F=_tmp11E.f2;
inflow=*_tmp11F;{
void*_tmp120=s->r;struct Cyc_Absyn_Stmt*_tmp122;struct Cyc_Absyn_Stmt*_tmp123;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Tvar*_tmp127;struct Cyc_Absyn_Vardecl*_tmp128;struct Cyc_Absyn_Stmt*_tmp129;struct Cyc_Absyn_Decl*_tmp12B;struct Cyc_Absyn_Stmt*_tmp12C;struct Cyc_Absyn_Exp*_tmp12E;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp121=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp120;if(_tmp121->tag != 2)goto _LLF2;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LLF1:
# 712
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp122);
s=_tmp123;
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp124=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp120;if(_tmp124->tag != 12)goto _LLF4;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp125=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(*_tmp124->f1).r;if(_tmp125->tag != 5)goto _LLF4;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;_tmp128=_tmp125->f3;}}_tmp129=_tmp124->f2;}}_LLF3: {
# 718
union Cyc_CfFlowInfo_FlowInfo _tmp130;union Cyc_CfFlowInfo_AbsLVal _tmp131;struct _tuple15 _tmp12F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp126);_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;{
struct _tuple15 _tmp6F4;struct _tuple15 _tmp133=(_tmp6F4.f1=_tmp130,((_tmp6F4.f2=_tmp131,_tmp6F4)));struct Cyc_Dict_Dict _tmp134;struct Cyc_List_List*_tmp135;struct Cyc_CfFlowInfo_Place*_tmp136;_LLFB: if(((_tmp133.f1).ReachableFL).tag != 2)goto _LLFD;_tmp134=((struct _tuple13)((_tmp133.f1).ReachableFL).val).f1;_tmp135=((struct _tuple13)((_tmp133.f1).ReachableFL).val).f2;if(((_tmp133.f2).PlaceL).tag != 1)goto _LLFD;_tmp136=(struct Cyc_CfFlowInfo_Place*)((_tmp133.f2).PlaceL).val;_LLFC: {
# 721
void*_tmp137=Cyc_CfFlowInfo_lookup_place(_tmp134,_tmp136);
# 723
void*t=(void*)_check_null(_tmp126->topt);
{void*_tmp138=_tmp126->r;struct Cyc_Absyn_Exp*_tmp13A;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138;if(_tmp139->tag != 13)goto _LL104;else{_tmp13A=_tmp139->f2;}}_LL103:
 t=(void*)_check_null(_tmp13A->topt);goto _LL101;_LL104:;_LL105:
 goto _LL101;_LL101:;}{
# 728
void*_tmp13B=Cyc_CfFlowInfo_make_unique_consumed(_tmp11D,t,_tmp126,env->iteration_num,_tmp137);
_tmp134=Cyc_CfFlowInfo_assign_place(_tmp11D,_tmp126->loc,_tmp134,env->all_changed,_tmp136,_tmp13B);
_tmp130=Cyc_CfFlowInfo_ReachableFL(_tmp134,_tmp135);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp11D,_tmp134,_tmp137)){case Cyc_CfFlowInfo_AllIL: _LL106:
 leaf=_tmp11D->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL107:
 leaf=_tmp11D->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL108:
 leaf=_tmp11D->unknown_this;break;}{
# 737
void*_tmp13C=Cyc_CfFlowInfo_typ_to_absrval(_tmp11D,t,0,leaf);
_tmp130=Cyc_NewControlFlow_use_Rval(env,_tmp126->loc,_tmp130,_tmp137);{
struct Cyc_List_List _tmp6F5;struct Cyc_List_List _tmp13D=(_tmp6F5.hd=_tmp128,((_tmp6F5.tl=0,_tmp6F5)));
_tmp130=Cyc_NewControlFlow_add_vars(_tmp11D,_tmp130,(struct Cyc_List_List*)& _tmp13D,_tmp11D->unknown_all,s->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp13F;void*_tmp140;struct _tuple14 _tmp13E=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp130,_tmp129);_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp141=_tmp13F;struct Cyc_Dict_Dict _tmp142;struct Cyc_List_List*_tmp143;_LL10B: if((_tmp141.ReachableFL).tag != 2)goto _LL10D;_tmp142=((struct _tuple13)(_tmp141.ReachableFL).val).f1;_tmp143=((struct _tuple13)(_tmp141.ReachableFL).val).f2;_LL10C:
# 744
 _tmp142=Cyc_CfFlowInfo_assign_place(_tmp11D,s->loc,_tmp142,env->all_changed,_tmp136,_tmp13C);{
union Cyc_CfFlowInfo_FlowInfo _tmp144=Cyc_CfFlowInfo_ReachableFL(_tmp142,_tmp143);
struct _tuple14 _tmp6F6;return(_tmp6F6.f1=_tmp144,((_tmp6F6.f2=_tmp140,_tmp6F6)));};_LL10D:;_LL10E: {
struct _tuple14 _tmp6F7;return(_tmp6F7.f1=_tmp13F,((_tmp6F7.f2=_tmp140,_tmp6F7)));}_LL10A:;};};};};};};}_LLFD: if(((_tmp133.f1).BottomFL).tag != 1)goto _LLFF;_LLFE: {
# 749
struct _tuple14 _tmp6F8;return(_tmp6F8.f1=_tmp130,((_tmp6F8.f2=_tmp11D->unknown_all,_tmp6F8)));}_LLFF:;_LL100:
{const char*_tmp6FB;void*_tmp6FA;(_tmp6FA=0,Cyc_Tcutil_terr(_tmp126->loc,((_tmp6FB="bad alias expression--no unique path found",_tag_dyneither(_tmp6FB,sizeof(char),43))),_tag_dyneither(_tmp6FA,sizeof(void*),0)));}{
struct _tuple14 _tmp6FC;return(_tmp6FC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6FC.f2=_tmp11D->unknown_all,_tmp6FC)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp12A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp120;if(_tmp12A->tag != 12)goto _LLF6;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}_LLF5:
# 754
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp12B);
s=_tmp12C;
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp12D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp120;if(_tmp12D->tag != 1)goto _LLF8;else{_tmp12E=_tmp12D->f1;}}_LLF7:
# 758
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp12E);_LLF8:;_LLF9: {
struct Cyc_Core_Impossible_exn_struct _tmp702;const char*_tmp701;struct Cyc_Core_Impossible_exn_struct*_tmp700;(int)_throw((void*)((_tmp700=_cycalloc(sizeof(*_tmp700)),((_tmp700[0]=((_tmp702.tag=Cyc_Core_Impossible,((_tmp702.f1=((_tmp701="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp701,sizeof(char),33))),_tmp702)))),_tmp700)))));}_LLEF:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7A4(unsigned int*_tmp7A3,unsigned int*_tmp7A2,void***_tmp7A0,struct Cyc_List_List**_tmp311){for(*_tmp7A3=0;*_tmp7A3 < *_tmp7A2;(*_tmp7A3)++){
# 1307 "new_control_flow.cyc"
void*_tmp79E;(*_tmp7A0)[*_tmp7A3]=((_tmp79E=(void*)((struct Cyc_List_List*)_check_null(*_tmp311))->hd,((*_tmp311=(*_tmp311)->tl,_tmp79E))));}}
# 766 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 770
struct Cyc_CfFlowInfo_FlowEnv*_tmp14F=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 776
{union Cyc_CfFlowInfo_FlowInfo _tmp150=inflow;struct Cyc_Dict_Dict _tmp151;struct Cyc_List_List*_tmp152;_LL110: if((_tmp150.BottomFL).tag != 1)goto _LL112;_LL111: {
struct _tuple14 _tmp703;return(_tmp703.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp703.f2=_tmp14F->unknown_all,_tmp703)));}_LL112: if((_tmp150.ReachableFL).tag != 2)goto _LL10F;_tmp151=((struct _tuple13)(_tmp150.ReachableFL).val).f1;_tmp152=((struct _tuple13)(_tmp150.ReachableFL).val).f2;_LL113:
 d=_tmp151;relns=_tmp152;_LL10F:;}
# 785
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 806 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp156;struct _tuple15 _tmp154=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp155=_tmp154.f1;_tmp156=_tmp154.f2;{
struct _tuple15 _tmp704;struct _tuple15 _tmp158=(_tmp704.f1=_tmp155,((_tmp704.f2=_tmp156,_tmp704)));struct Cyc_Dict_Dict _tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_CfFlowInfo_Place*_tmp15B;_LL115: if(((_tmp158.f1).ReachableFL).tag != 2)goto _LL117;_tmp159=((struct _tuple13)((_tmp158.f1).ReachableFL).val).f1;_tmp15A=((struct _tuple13)((_tmp158.f1).ReachableFL).val).f2;if(((_tmp158.f2).PlaceL).tag != 1)goto _LL117;_tmp15B=(struct Cyc_CfFlowInfo_Place*)((_tmp158.f2).PlaceL).val;_LL116: {
# 809
void*_tmp15C=Cyc_CfFlowInfo_lookup_place(_tmp159,_tmp15B);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp15C,& needs_unconsume)){
{const char*_tmp707;void*_tmp706;(_tmp706=0,Cyc_Tcutil_terr(e->loc,((_tmp707="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp707,sizeof(char),71))),_tag_dyneither(_tmp706,sizeof(void*),0)));}{
struct _tuple14 _tmp708;return(_tmp708.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp708.f2=_tmp14F->unknown_all,_tmp708)));};}else{
# 815
if(needs_unconsume){
# 817
struct _tuple14 _tmp709;return(_tmp709.f1=_tmp155,((_tmp709.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14F,_tmp15C),_tmp709)));}else{
# 820
void*_tmp161=Cyc_CfFlowInfo_make_unique_consumed(_tmp14F,(void*)_check_null(e->topt),e,env->iteration_num,_tmp15C);
struct Cyc_Dict_Dict _tmp162=Cyc_CfFlowInfo_assign_place(_tmp14F,e->loc,_tmp159,env->all_changed,_tmp15B,_tmp161);
# 831
struct _tuple14 _tmp70A;return(_tmp70A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp162,_tmp15A),((_tmp70A.f2=_tmp15C,_tmp70A)));}}}_LL117:;_LL118:
# 833
 goto _LL114;_LL114:;};}{
# 836
void*_tmp164=e->r;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Vardecl*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp180;struct Cyc_Absyn_Vardecl*_tmp183;enum Cyc_Absyn_Primop _tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_List_List*_tmp196;int _tmp198;struct Cyc_Absyn_Exp*_tmp199;void**_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;int _tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct _dyneither_ptr*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct _dyneither_ptr*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;struct _dyneither_ptr*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Datatypedecl*_tmp1BF;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C5;enum Cyc_Absyn_AggrKind _tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Vardecl*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;int _tmp1CF;struct Cyc_Absyn_Stmt*_tmp1D1;_LL11A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp165->tag != 13)goto _LL11C;else{_tmp166=_tmp165->f2;if(_tmp165->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL11C;}}_LL11B: {
# 840
union Cyc_CfFlowInfo_FlowInfo _tmp1D9;void*_tmp1DA;struct _tuple14 _tmp1D8=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp166);_tmp1D9=_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1DC;void*_tmp1DD;struct _tuple14 _tmp1DB=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D9,_tmp166,_tmp1DA);_tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;{
struct _tuple14 _tmp70B;return(_tmp70B.f1=_tmp1DC,((_tmp70B.f2=_tmp1DA,_tmp70B)));};};}_LL11C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp167->tag != 13)goto _LL11E;else{_tmp168=_tmp167->f2;}}_LL11D:
# 846
 _tmp16A=_tmp168;goto _LL11F;_LL11E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp169->tag != 11)goto _LL120;else{_tmp16A=_tmp169->f1;}}_LL11F:
 _tmp16C=_tmp16A;goto _LL121;_LL120: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp16B->tag != 12)goto _LL122;else{_tmp16C=_tmp16B->f1;}}_LL121:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp16C);_LL122: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp16D->tag != 0)goto _LL124;else{if(((_tmp16D->f1).Null_c).tag != 1)goto _LL124;}}_LL123:
# 850
 goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp16E->tag != 0)goto _LL126;else{if(((_tmp16E->f1).Int_c).tag != 5)goto _LL126;if(((struct _tuple6)((_tmp16E->f1).Int_c).val).f2 != 0)goto _LL126;}}_LL125: {
struct _tuple14 _tmp70C;return(_tmp70C.f1=inflow,((_tmp70C.f2=_tmp14F->zero,_tmp70C)));}_LL126: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp16F->tag != 0)goto _LL128;else{if(((_tmp16F->f1).Int_c).tag != 5)goto _LL128;}}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp170->tag != 1)goto _LL12A;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp171=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp170->f2);if(_tmp171->tag != 2)goto _LL12A;}}_LL129: {
struct _tuple14 _tmp70D;return(_tmp70D.f1=inflow,((_tmp70D.f2=_tmp14F->notzeroall,_tmp70D)));}_LL12A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp172->tag != 29)goto _LL12C;else{if(_tmp172->f1 != 0)goto _LL12C;}}_LL12B:
# 855
 goto _LL12D;_LL12C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp173->tag != 0)goto _LL12E;}_LL12D:
 goto _LL12F;_LL12E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp174->tag != 17)goto _LL130;}_LL12F:
 goto _LL131;_LL130: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp175->tag != 16)goto _LL132;}_LL131:
 goto _LL133;_LL132: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp176->tag != 18)goto _LL134;}_LL133:
 goto _LL135;_LL134: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp177->tag != 31)goto _LL136;}_LL135:
 goto _LL137;_LL136: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp178->tag != 30)goto _LL138;}_LL137: {
struct _tuple14 _tmp70E;return(_tmp70E.f1=inflow,((_tmp70E.f2=_tmp14F->unknown_all,_tmp70E)));}_LL138: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp179->tag != 1)goto _LL13A;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp17A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp179->f2);if(_tmp17A->tag != 1)goto _LL13A;}}_LL139: {
# 864
struct _tuple14 _tmp70F;return(_tmp70F.f1=inflow,((_tmp70F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp70F)));}_LL13A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp17B->tag != 1)goto _LL13C;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp17C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp17B->f2);if(_tmp17C->tag != 3)goto _LL13C;else{_tmp17D=_tmp17C->f1;}}}_LL13B:
# 867
 _tmp180=_tmp17D;goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp17E->tag != 1)goto _LL13E;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17E->f2);if(_tmp17F->tag != 4)goto _LL13E;else{_tmp180=_tmp17F->f1;}}}_LL13D:
 _tmp183=_tmp180;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp181->tag != 1)goto _LL140;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp182=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp181->f2);if(_tmp182->tag != 5)goto _LL140;else{_tmp183=_tmp182->f1;}}}_LL13F: {
# 871
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp710;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp710.tag=0,((_tmp710.f1=_tmp183,_tmp710)));
struct _tuple14 _tmp711;return(_tmp711.f1=inflow,((_tmp711.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp711)));}_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp184->tag != 2)goto _LL142;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}_LL141: {
# 878
union Cyc_CfFlowInfo_FlowInfo _tmp1E6;void*_tmp1E7;struct _tuple14 _tmp1E5=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp186);_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;{
struct _tuple14 _tmp712;return(_tmp712.f1=_tmp1E6,((_tmp712.f2=_tmp1E7,_tmp712)));};}_LL142: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp187->tag != 4)goto _LL144;else{_tmp188=_tmp187->f1;}}_LL143: {
# 882
struct Cyc_List_List _tmp713;struct Cyc_List_List _tmp1E9=(_tmp713.hd=_tmp188,((_tmp713.tl=0,_tmp713)));
union Cyc_CfFlowInfo_FlowInfo _tmp1EB;struct _tuple14 _tmp1EA=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp1E9);_tmp1EB=_tmp1EA.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1ED;struct _tuple15 _tmp1EC=Cyc_NewControlFlow_anal_Lexp(env,_tmp1EB,0,_tmp188);_tmp1ED=_tmp1EC.f2;
{struct _tuple21 _tmp714;struct _tuple21 _tmp1EF=(_tmp714.f1=_tmp1ED,((_tmp714.f2=_tmp1EB,_tmp714)));struct Cyc_CfFlowInfo_Place*_tmp1F0;struct Cyc_Dict_Dict _tmp1F1;struct Cyc_List_List*_tmp1F2;_LL181: if(((_tmp1EF.f1).PlaceL).tag != 1)goto _LL183;_tmp1F0=(struct Cyc_CfFlowInfo_Place*)((_tmp1EF.f1).PlaceL).val;if(((_tmp1EF.f2).ReachableFL).tag != 2)goto _LL183;_tmp1F1=((struct _tuple13)((_tmp1EF.f2).ReachableFL).val).f1;_tmp1F2=((struct _tuple13)((_tmp1EF.f2).ReachableFL).val).f2;_LL182:
# 887
 _tmp1F2=Cyc_CfFlowInfo_reln_kill_exp(_tmp14F->r,_tmp1F2,_tmp188);
_tmp1EB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp188->loc,_tmp1F1,env->all_changed,_tmp1F0,_tmp14F->unknown_all),_tmp1F2);
# 891
goto _LL180;_LL183:;_LL184:
 goto _LL180;_LL180:;}{
# 894
struct _tuple14 _tmp715;return(_tmp715.f1=_tmp1EB,((_tmp715.f2=_tmp14F->unknown_all,_tmp715)));};};}_LL144: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp189->tag != 3)goto _LL146;else{_tmp18A=_tmp189->f1;if(_tmp189->f2 == 0)goto _LL146;_tmp18B=_tmp189->f3;}}_LL145:
# 897
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp718;void*_tmp717;(_tmp717=0,Cyc_Tcutil_terr(e->loc,((_tmp718="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp718,sizeof(char),58))),_tag_dyneither(_tmp717,sizeof(void*),0)));}{
struct _tuple14 _tmp719;return(_tmp719.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp719.f2=_tmp14F->unknown_all,_tmp719)));};}{
# 901
struct Cyc_List_List _tmp71A;struct Cyc_List_List _tmp1F8=(_tmp71A.hd=_tmp18B,((_tmp71A.tl=0,_tmp71A)));
struct Cyc_List_List _tmp71B;struct Cyc_List_List _tmp1F9=(_tmp71B.hd=_tmp18A,((_tmp71B.tl=(struct Cyc_List_List*)& _tmp1F8,_tmp71B)));
union Cyc_CfFlowInfo_FlowInfo _tmp1FB;struct _tuple14 _tmp1FA=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp1F9);_tmp1FB=_tmp1FA.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1FD;struct _tuple15 _tmp1FC=Cyc_NewControlFlow_anal_Lexp(env,_tmp1FB,0,_tmp18A);_tmp1FD=_tmp1FC.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1FE=_tmp1FB;struct Cyc_Dict_Dict _tmp1FF;struct Cyc_List_List*_tmp200;_LL186: if((_tmp1FE.ReachableFL).tag != 2)goto _LL188;_tmp1FF=((struct _tuple13)(_tmp1FE.ReachableFL).val).f1;_tmp200=((struct _tuple13)(_tmp1FE.ReachableFL).val).f2;_LL187:
# 907
{union Cyc_CfFlowInfo_AbsLVal _tmp201=_tmp1FD;struct Cyc_CfFlowInfo_Place*_tmp202;_LL18B: if((_tmp201.PlaceL).tag != 1)goto _LL18D;_tmp202=(struct Cyc_CfFlowInfo_Place*)(_tmp201.PlaceL).val;_LL18C:
# 911
 _tmp200=Cyc_CfFlowInfo_reln_kill_exp(_tmp14F->r,_tmp200,_tmp18A);
_tmp1FF=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp18A->loc,_tmp1FF,env->all_changed,_tmp202,_tmp14F->unknown_all);
# 914
_tmp1FB=Cyc_CfFlowInfo_ReachableFL(_tmp1FF,_tmp200);
# 917
goto _LL18A;_LL18D: if((_tmp201.UnknownL).tag != 2)goto _LL18A;_LL18E:
# 920
 goto _LL18A;_LL18A:;}
# 922
goto _LL185;_LL188:;_LL189:
 goto _LL185;_LL185:;}{
# 925
struct _tuple14 _tmp71C;return(_tmp71C.f1=_tmp1FB,((_tmp71C.f2=_tmp14F->unknown_all,_tmp71C)));};};};_LL146: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp18C->tag != 3)goto _LL148;else{_tmp18D=_tmp18C->f1;if(_tmp18C->f2 != 0)goto _LL148;_tmp18E=_tmp18C->f3;}}_LL147:
# 929
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp71F;void*_tmp71E;(_tmp71E=0,Cyc_Tcutil_terr(e->loc,((_tmp71F="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp71F,sizeof(char),58))),_tag_dyneither(_tmp71E,sizeof(void*),0)));}{
struct _tuple14 _tmp720;return(_tmp720.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp720.f2=_tmp14F->unknown_all,_tmp720)));};}{
# 933
struct Cyc_Dict_Dict*_tmp209=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp721;env->all_changed=((_tmp721=_region_malloc(env->r,sizeof(*_tmp721)),((_tmp721[0]=_tmp14F->mt_place_set,_tmp721))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp20C;union Cyc_CfFlowInfo_AbsLVal _tmp20D;struct _tuple15 _tmp20B=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp18D);_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;{
struct Cyc_Dict_Dict _tmp20E=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp722;env->all_changed=((_tmp722=_region_malloc(env->r,sizeof(*_tmp722)),((_tmp722[0]=_tmp14F->mt_place_set,_tmp722))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp211;void*_tmp212;struct _tuple14 _tmp210=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp18E);_tmp211=_tmp210.f1;_tmp212=_tmp210.f2;{
struct Cyc_Dict_Dict _tmp213=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp214=Cyc_CfFlowInfo_after_flow(_tmp14F,(struct Cyc_Dict_Dict*)& _tmp20E,_tmp20C,_tmp211,_tmp20E,_tmp213);
# 944
union Cyc_CfFlowInfo_FlowInfo _tmp215=Cyc_CfFlowInfo_join_flow(_tmp14F,_tmp209,inflow,_tmp214);
# 947
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp215,inflow)){
if(_tmp209 == 0)
env->all_changed=0;else{
# 951
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp209,
Cyc_CfFlowInfo_union_place_set(_tmp20E,_tmp213,0),0);}
# 954
return Cyc_NewControlFlow_do_assign(_tmp14F,env,_tmp214,_tmp18D,_tmp20D,_tmp18E,_tmp212,e->loc);}
# 957
inflow=_tmp215;};};};};}};_LL148: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp18F->tag != 8)goto _LL14A;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LL149: {
# 962
union Cyc_CfFlowInfo_FlowInfo _tmp217;void*_tmp218;struct _tuple14 _tmp216=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp190);_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp217,_tmp191);}_LL14A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp192->tag != 10)goto _LL14C;else{_tmp193=_tmp192->f1;}}_LL14B: {
# 966
union Cyc_CfFlowInfo_FlowInfo _tmp21A;void*_tmp21B;struct _tuple14 _tmp219=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp193);_tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp193->loc,_tmp21A,_tmp21B);{
struct _tuple14 _tmp723;return(_tmp723.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp723.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp723)));};}_LL14C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp194->tag != 9)goto _LL14E;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}_LL14D: {
# 971
struct _RegionHandle _tmp21D=_new_region("temp");struct _RegionHandle*temp=& _tmp21D;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp724;struct _tuple20 _tmp21F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp724=_region_malloc(temp,sizeof(*_tmp724)),((_tmp724->hd=_tmp195,((_tmp724->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp196),_tmp724)))))),0,1);_tmp220=_tmp21F.f1;_tmp221=_tmp21F.f2;{
# 974
union Cyc_CfFlowInfo_FlowInfo _tmp222=Cyc_NewControlFlow_use_Rval(env,_tmp195->loc,_tmp220,(void*)((struct Cyc_List_List*)_check_null(_tmp221))->hd);
_tmp221=_tmp221->tl;{
# 977
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp223=Cyc_Tcutil_compress((void*)_check_null(_tmp195->topt));void*_tmp225;_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp223;if(_tmp224->tag != 5)goto _LL192;else{_tmp225=(_tmp224->f1).elt_typ;}}_LL191:
# 982
{void*_tmp226=Cyc_Tcutil_compress(_tmp225);struct Cyc_List_List*_tmp228;_LL195: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp226;if(_tmp227->tag != 9)goto _LL197;else{_tmp228=(_tmp227->f1).attributes;}}_LL196:
# 984
 for(0;_tmp228 != 0;_tmp228=_tmp228->tl){
# 986
void*_tmp229=(void*)_tmp228->hd;int _tmp22B;int _tmp22D;int _tmp22F;_LL19A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp22A=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp229;if(_tmp22A->tag != 20)goto _LL19C;else{_tmp22B=_tmp22A->f1;}}_LL19B:
# 988
{struct Cyc_List_List*_tmp725;init_params=((_tmp725=_region_malloc(temp,sizeof(*_tmp725)),((_tmp725->hd=(void*)_tmp22B,((_tmp725->tl=init_params,_tmp725))))));}goto _LL199;_LL19C: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp22C=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp229;if(_tmp22C->tag != 21)goto _LL19E;else{_tmp22D=_tmp22C->f1;}}_LL19D:
# 990
{struct Cyc_List_List*_tmp726;nolive_unique_params=((_tmp726=_region_malloc(temp,sizeof(*_tmp726)),((_tmp726->hd=(void*)_tmp22D,((_tmp726->tl=nolive_unique_params,_tmp726))))));}
goto _LL199;_LL19E: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp22E=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp229;if(_tmp22E->tag != 22)goto _LL1A0;else{_tmp22F=_tmp22E->f1;}}_LL19F:
# 994
{struct Cyc_List_List*_tmp727;noconsume_params=((_tmp727=_region_malloc(temp,sizeof(*_tmp727)),((_tmp727->hd=(void*)_tmp22F,((_tmp727->tl=noconsume_params,_tmp727))))));}
goto _LL199;_LL1A0:;_LL1A1:
 goto _LL199;_LL199:;}
# 998
goto _LL194;_LL197:;_LL198: {
const char*_tmp72A;void*_tmp729;(_tmp729=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp72A="anal_Rexp: bad function type",_tag_dyneither(_tmp72A,sizeof(char),29))),_tag_dyneither(_tmp729,sizeof(void*),0)));}_LL194:;}
# 1001
goto _LL18F;_LL192:;_LL193: {
const char*_tmp72D;void*_tmp72C;(_tmp72C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp72D="anal_Rexp: bad function type",_tag_dyneither(_tmp72D,sizeof(char),29))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}_LL18F:;}
# 1004
{int i=1;for(0;_tmp221 != 0;(((_tmp221=_tmp221->tl,_tmp196=((struct Cyc_List_List*)_check_null(_tmp196))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp237=_tmp220;struct Cyc_Dict_Dict _tmp238;_LL1A3: if((_tmp237.BottomFL).tag != 1)goto _LL1A5;_LL1A4:
 goto _LL1A2;_LL1A5: if((_tmp237.ReachableFL).tag != 2)goto _LL1A2;_tmp238=((struct _tuple13)(_tmp237.ReachableFL).val).f1;_LL1A6:
# 1009
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp238,(void*)_tmp221->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp730;void*_tmp72F;(_tmp72F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp196))->hd)->loc,((_tmp730="expression may not be initialized",_tag_dyneither(_tmp730,sizeof(char),34))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp23B=_tmp222;struct Cyc_Dict_Dict _tmp23C;struct Cyc_List_List*_tmp23D;_LL1A8: if((_tmp23B.BottomFL).tag != 1)goto _LL1AA;_LL1A9:
 goto _LL1A7;_LL1AA: if((_tmp23B.ReachableFL).tag != 2)goto _LL1A7;_tmp23C=((struct _tuple13)(_tmp23B.ReachableFL).val).f1;_tmp23D=((struct _tuple13)(_tmp23B.ReachableFL).val).f2;_LL1AB: {
# 1016
struct Cyc_Dict_Dict _tmp23E=Cyc_CfFlowInfo_escape_deref(_tmp14F,_tmp23C,env->all_changed,(void*)_tmp221->hd);
{void*_tmp23F=(void*)_tmp221->hd;struct Cyc_CfFlowInfo_Place*_tmp241;_LL1AD: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp240=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp23F;if(_tmp240->tag != 5)goto _LL1AF;else{_tmp241=_tmp240->f1;}}_LL1AE:
# 1019
{void*_tmp242=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp196))->hd)->topt));void*_tmp244;_LL1B2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242;if(_tmp243->tag != 5)goto _LL1B4;else{_tmp244=(_tmp243->f1).elt_typ;}}_LL1B3:
# 1021
 _tmp23E=Cyc_CfFlowInfo_assign_place(_tmp14F,((struct Cyc_Absyn_Exp*)_tmp196->hd)->loc,_tmp23E,env->all_changed,_tmp241,
# 1023
Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,_tmp244,0,_tmp14F->esc_all));
# 1025
goto _LL1B1;_LL1B4:;_LL1B5: {
const char*_tmp733;void*_tmp732;(_tmp732=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp733="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp733,sizeof(char),39))),_tag_dyneither(_tmp732,sizeof(void*),0)));}_LL1B1:;}
# 1028
goto _LL1AC;_LL1AF:;_LL1B0:
 goto _LL1AC;_LL1AC:;}
# 1031
_tmp222=Cyc_CfFlowInfo_ReachableFL(_tmp23E,_tmp23D);
goto _LL1A7;}_LL1A7:;}
# 1034
goto _LL1A2;_LL1A2:;}else{
# 1037
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1040
_tmp222=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp196))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp196))->hd)->topt),_tmp222,(void*)_tmp221->hd);else{
# 1047
_tmp222=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp196))->hd)->loc,_tmp222,(void*)_tmp221->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1050
union Cyc_CfFlowInfo_AbsLVal _tmp248;struct _tuple15 _tmp247=Cyc_NewControlFlow_anal_Lexp(env,_tmp222,1,(struct Cyc_Absyn_Exp*)_tmp196->hd);_tmp248=_tmp247.f2;{
struct _tuple15 _tmp734;struct _tuple15 _tmp24A=(_tmp734.f1=_tmp222,((_tmp734.f2=_tmp248,_tmp734)));struct Cyc_Dict_Dict _tmp24B;struct Cyc_List_List*_tmp24C;struct Cyc_CfFlowInfo_Place*_tmp24D;_LL1B7: if(((_tmp24A.f1).ReachableFL).tag != 2)goto _LL1B9;_tmp24B=((struct _tuple13)((_tmp24A.f1).ReachableFL).val).f1;_tmp24C=((struct _tuple13)((_tmp24A.f1).ReachableFL).val).f2;if(((_tmp24A.f2).PlaceL).tag != 1)goto _LL1B9;_tmp24D=(struct Cyc_CfFlowInfo_Place*)((_tmp24A.f2).PlaceL).val;_LL1B8: {
# 1053
void*_tmp24E=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14F,(void*)_tmp221->hd);
# 1056
_tmp24B=Cyc_CfFlowInfo_assign_place(_tmp14F,((struct Cyc_Absyn_Exp*)_tmp196->hd)->loc,_tmp24B,env->all_changed,_tmp24D,_tmp24E);
_tmp222=Cyc_CfFlowInfo_ReachableFL(_tmp24B,_tmp24C);
# 1061
goto _LL1B6;}_LL1B9: if(((_tmp24A.f1).BottomFL).tag != 1)goto _LL1BB;_LL1BA:
 continue;_LL1BB:;_LL1BC: {
# 1064
const char*_tmp737;void*_tmp736;(_tmp736=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp196->hd)->loc,((_tmp737="noconsume() parameters must be l-values",_tag_dyneither(_tmp737,sizeof(char),40))),_tag_dyneither(_tmp736,sizeof(void*),0)));}_LL1B6:;};}}}}}
# 1071
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp195->topt))){
struct _tuple14 _tmp738;struct _tuple14 _tmp252=(_tmp738.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp738.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp738)));_npop_handler(0);return _tmp252;}else{
# 1074
struct _tuple14 _tmp739;struct _tuple14 _tmp254=(_tmp739.f1=_tmp222,((_tmp739.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp739)));_npop_handler(0);return _tmp254;}};};}
# 972
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp197->tag != 32)goto _LL150;else{_tmp198=(_tmp197->f1).is_calloc;_tmp199=(_tmp197->f1).rgn;_tmp19A=(_tmp197->f1).elt_type;_tmp19B=(_tmp197->f1).num_elts;_tmp19C=(_tmp197->f1).fat_result;}}_LL14F: {
# 1077
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp73C;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp73B;void*root=(void*)((_tmp73B=_region_malloc(_tmp14F->r,sizeof(*_tmp73B)),((_tmp73B[0]=((_tmp73C.tag=1,((_tmp73C.f1=_tmp19B,((_tmp73C.f2=(void*)((void*)_check_null(e->topt)),_tmp73C)))))),_tmp73B))));
struct Cyc_CfFlowInfo_Place*_tmp73D;struct Cyc_CfFlowInfo_Place*place=(_tmp73D=_region_malloc(_tmp14F->r,sizeof(*_tmp73D)),((_tmp73D->root=root,((_tmp73D->fields=0,_tmp73D)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp740;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp73F;void*rval=(void*)((_tmp73F=_region_malloc(_tmp14F->r,sizeof(*_tmp73F)),((_tmp73F[0]=((_tmp740.tag=5,((_tmp740.f1=place,_tmp740)))),_tmp73F))));
void*place_val;
if(_tmp19C)place_val=_tmp14F->notzeroall;else{
if(_tmp198)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,*((void**)_check_null(_tmp19A)),0,_tmp14F->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,*((void**)_check_null(_tmp19A)),0,_tmp14F->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp199 != 0){
struct _RegionHandle _tmp255=_new_region("temp");struct _RegionHandle*temp=& _tmp255;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp258;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Exp*_tmp741[2];struct _tuple20 _tmp257=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp741[1]=_tmp19B,((_tmp741[0]=(struct Cyc_Absyn_Exp*)_tmp199,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp741,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;
outflow=_tmp258;}
# 1088
;_pop_region(temp);}else{
# 1093
union Cyc_CfFlowInfo_FlowInfo _tmp25B;struct _tuple14 _tmp25A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19B);_tmp25B=_tmp25A.f1;
outflow=_tmp25B;}{
# 1096
union Cyc_CfFlowInfo_FlowInfo _tmp25C=outflow;struct Cyc_Dict_Dict _tmp25D;struct Cyc_List_List*_tmp25E;_LL1BE: if((_tmp25C.BottomFL).tag != 1)goto _LL1C0;_LL1BF: {
struct _tuple14 _tmp742;return(_tmp742.f1=outflow,((_tmp742.f2=rval,_tmp742)));}_LL1C0: if((_tmp25C.ReachableFL).tag != 2)goto _LL1BD;_tmp25D=((struct _tuple13)(_tmp25C.ReachableFL).val).f1;_tmp25E=((struct _tuple13)(_tmp25C.ReachableFL).val).f2;_LL1C1: {
# 1099
struct _tuple14 _tmp743;return(_tmp743.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp25D,root,place_val),_tmp25E),((_tmp743.f2=rval,_tmp743)));}_LL1BD:;};};}_LL150: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp19D->tag != 33)goto _LL152;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}_LL151: {
# 1103
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1109
struct _RegionHandle _tmp266=_new_region("temp");struct _RegionHandle*temp=& _tmp266;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp269;struct Cyc_List_List*_tmp26A;struct Cyc_Absyn_Exp*_tmp744[2];struct _tuple20 _tmp268=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp744[1]=_tmp19F,((_tmp744[0]=_tmp19E,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp744,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp269=_tmp268.f1;_tmp26A=_tmp268.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26A))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp26A->tl))->hd;
outflow=_tmp269;}{
# 1117
union Cyc_CfFlowInfo_AbsLVal _tmp26C;struct _tuple15 _tmp26B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19E);_tmp26C=_tmp26B.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp26E;struct _tuple15 _tmp26D=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp19F);_tmp26E=_tmp26D.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp26F=outflow;struct Cyc_Dict_Dict _tmp270;struct Cyc_List_List*_tmp271;_LL1C3: if((_tmp26F.ReachableFL).tag != 2)goto _LL1C5;_tmp270=((struct _tuple13)(_tmp26F.ReachableFL).val).f1;_tmp271=((struct _tuple13)(_tmp26F.ReachableFL).val).f2;_LL1C4:
# 1121
{union Cyc_CfFlowInfo_AbsLVal _tmp272=_tmp26C;struct Cyc_CfFlowInfo_Place*_tmp273;_LL1C8: if((_tmp272.PlaceL).tag != 1)goto _LL1CA;_tmp273=(struct Cyc_CfFlowInfo_Place*)(_tmp272.PlaceL).val;_LL1C9:
# 1123
 _tmp270=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp19E->loc,_tmp270,env->all_changed,_tmp273,right_rval);
# 1125
goto _LL1C7;_LL1CA: if((_tmp272.UnknownL).tag != 2)goto _LL1C7;_LL1CB:
 goto _LL1C7;_LL1C7:;}
# 1128
{union Cyc_CfFlowInfo_AbsLVal _tmp274=_tmp26E;struct Cyc_CfFlowInfo_Place*_tmp275;_LL1CD: if((_tmp274.PlaceL).tag != 1)goto _LL1CF;_tmp275=(struct Cyc_CfFlowInfo_Place*)(_tmp274.PlaceL).val;_LL1CE:
# 1130
 _tmp270=Cyc_CfFlowInfo_assign_place(_tmp14F,_tmp19F->loc,_tmp270,env->all_changed,_tmp275,left_rval);
# 1132
goto _LL1CC;_LL1CF: if((_tmp274.UnknownL).tag != 2)goto _LL1CC;_LL1D0:
 goto _LL1CC;_LL1CC:;}
# 1136
_tmp271=Cyc_CfFlowInfo_reln_kill_exp(_tmp14F->r,_tmp271,_tmp19E);
_tmp271=Cyc_CfFlowInfo_reln_kill_exp(_tmp14F->r,_tmp271,_tmp19F);
# 1139
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp270,_tmp271);
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}{
# 1145
struct _tuple14 _tmp745;struct _tuple14 _tmp277=(_tmp745.f1=outflow,((_tmp745.f2=_tmp14F->unknown_all,_tmp745)));_npop_handler(0);return _tmp277;};};};
# 1109
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1A0->tag != 15)goto _LL154;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}}_LL153: {
# 1148
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp748;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp747;void*root=(void*)((_tmp747=_region_malloc(_tmp14F->r,sizeof(*_tmp747)),((_tmp747[0]=((_tmp748.tag=1,((_tmp748.f1=_tmp1A2,((_tmp748.f2=(void*)((void*)_check_null(e->topt)),_tmp748)))))),_tmp747))));
struct Cyc_CfFlowInfo_Place*_tmp749;struct Cyc_CfFlowInfo_Place*place=(_tmp749=_region_malloc(_tmp14F->r,sizeof(*_tmp749)),((_tmp749->root=root,((_tmp749->fields=0,_tmp749)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp74C;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp74B;void*rval=(void*)((_tmp74B=_region_malloc(_tmp14F->r,sizeof(*_tmp74B)),((_tmp74B[0]=((_tmp74C.tag=5,((_tmp74C.f1=place,_tmp74C)))),_tmp74B))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1A1 != 0){
struct _RegionHandle _tmp278=_new_region("temp");struct _RegionHandle*temp=& _tmp278;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp27B;struct Cyc_List_List*_tmp27C;struct Cyc_Absyn_Exp*_tmp74D[2];struct _tuple20 _tmp27A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp74D[1]=_tmp1A2,((_tmp74D[0]=(struct Cyc_Absyn_Exp*)_tmp1A1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp74D,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;
outflow=_tmp27B;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp27C))->tl))->hd;}
# 1156
;_pop_region(temp);}else{
# 1162
union Cyc_CfFlowInfo_FlowInfo _tmp27E;void*_tmp27F;struct _tuple14 _tmp27D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A2);_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;
outflow=_tmp27E;
place_val=_tmp27F;}{
# 1166
union Cyc_CfFlowInfo_FlowInfo _tmp280=outflow;struct Cyc_Dict_Dict _tmp281;struct Cyc_List_List*_tmp282;_LL1D2: if((_tmp280.BottomFL).tag != 1)goto _LL1D4;_LL1D3: {
struct _tuple14 _tmp74E;return(_tmp74E.f1=outflow,((_tmp74E.f2=rval,_tmp74E)));}_LL1D4: if((_tmp280.ReachableFL).tag != 2)goto _LL1D1;_tmp281=((struct _tuple13)(_tmp280.ReachableFL).val).f1;_tmp282=((struct _tuple13)(_tmp280.ReachableFL).val).f2;_LL1D5: {
# 1169
struct _tuple14 _tmp74F;return(_tmp74F.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp281,root,place_val),_tmp282),((_tmp74F.f2=rval,_tmp74F)));}_LL1D1:;};};}_LL154: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1A3->tag != 14)goto _LL156;else{_tmp1A4=_tmp1A3->f1;}}_LL155: {
# 1173
union Cyc_CfFlowInfo_FlowInfo _tmp28B;union Cyc_CfFlowInfo_AbsLVal _tmp28C;struct _tuple15 _tmp28A=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1A4);_tmp28B=_tmp28A.f1;_tmp28C=_tmp28A.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp28D=_tmp28C;struct Cyc_CfFlowInfo_Place*_tmp28E;_LL1D7: if((_tmp28D.UnknownL).tag != 2)goto _LL1D9;_LL1D8: {
struct _tuple14 _tmp750;return(_tmp750.f1=_tmp28B,((_tmp750.f2=_tmp14F->notzeroall,_tmp750)));}_LL1D9: if((_tmp28D.PlaceL).tag != 1)goto _LL1D6;_tmp28E=(struct Cyc_CfFlowInfo_Place*)(_tmp28D.PlaceL).val;_LL1DA: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp756;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp755;struct _tuple14 _tmp754;return(_tmp754.f1=_tmp28B,((_tmp754.f2=(void*)((_tmp756=_region_malloc(env->r,sizeof(*_tmp756)),((_tmp756[0]=((_tmp755.tag=5,((_tmp755.f1=_tmp28E,_tmp755)))),_tmp756)))),_tmp754)));}_LL1D6:;};}_LL156: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1A5->tag != 19)goto _LL158;else{_tmp1A6=_tmp1A5->f1;}}_LL157: {
# 1180
union Cyc_CfFlowInfo_FlowInfo _tmp294;void*_tmp295;struct _tuple14 _tmp293=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A6);_tmp294=_tmp293.f1;_tmp295=_tmp293.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp294,_tmp1A6,_tmp295);}_LL158: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1A7->tag != 20)goto _LL15A;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL159: {
# 1185
union Cyc_CfFlowInfo_FlowInfo _tmp297;void*_tmp298;struct _tuple14 _tmp296=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A8);_tmp297=_tmp296.f1;_tmp298=_tmp296.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1A8->topt))){
struct _tuple14 _tmp757;return(_tmp757.f1=_tmp297,((_tmp757.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp757)));}{
void*_tmp29A=_tmp298;int _tmp29C;int _tmp29D;struct _dyneither_ptr _tmp29E;_LL1DC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp29A;if(_tmp29B->tag != 6)goto _LL1DE;else{_tmp29C=(_tmp29B->f1).is_union;_tmp29D=(_tmp29B->f1).fieldnum;_tmp29E=_tmp29B->f2;}}_LL1DD: {
# 1190
int _tmp29F=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1A8->topt),_tmp1A9);
if((_tmp29C  && _tmp29D != - 1) && _tmp29D != _tmp29F){
struct _tuple14 _tmp758;return(_tmp758.f1=_tmp297,((_tmp758.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),1,_tmp14F->unknown_none),_tmp758)));}{
struct _tuple14 _tmp759;return(_tmp759.f1=_tmp297,((_tmp759.f2=*((void**)_check_dyneither_subscript(_tmp29E,sizeof(void*),_tmp29F)),_tmp759)));};}_LL1DE:;_LL1DF: {
# 1195
struct Cyc_Core_Impossible_exn_struct _tmp766;const char*_tmp765;void*_tmp764[1];struct Cyc_String_pa_PrintArg_struct _tmp763;struct Cyc_Core_Impossible_exn_struct*_tmp762;(int)_throw((void*)((_tmp762=_cycalloc(sizeof(*_tmp762)),((_tmp762[0]=((_tmp766.tag=Cyc_Core_Impossible,((_tmp766.f1=(struct _dyneither_ptr)((_tmp763.tag=0,((_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp764[0]=& _tmp763,Cyc_aprintf(((_tmp765="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp765,sizeof(char),26))),_tag_dyneither(_tmp764,sizeof(void*),1)))))))),_tmp766)))),_tmp762)))));}_LL1DB:;};}_LL15A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1AA->tag != 36)goto _LL15C;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}_LL15B: {
# 1202
union Cyc_CfFlowInfo_FlowInfo _tmp2A8;void*_tmp2A9;struct _tuple14 _tmp2A7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AB);_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp1AB->topt))){
struct _tuple14 _tmp767;return(_tmp767.f1=_tmp2A8,((_tmp767.f2=_tmp14F->unknown_all,_tmp767)));}{
void*_tmp2AB=_tmp2A9;int _tmp2AD;int _tmp2AE;struct _dyneither_ptr _tmp2AF;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2AC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AB;if(_tmp2AC->tag != 6)goto _LL1E3;else{_tmp2AD=(_tmp2AC->f1).is_union;_tmp2AE=(_tmp2AC->f1).fieldnum;_tmp2AF=_tmp2AC->f2;}}_LL1E2: {
# 1207
int _tmp2B0=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1AB->topt),_tmp1AC);
if(_tmp2AD  && _tmp2AE != - 1){
if(_tmp2AE != _tmp2B0){
struct _tuple14 _tmp768;return(_tmp768.f1=_tmp2A8,((_tmp768.f2=_tmp14F->zero,_tmp768)));}else{
# 1212
struct _tuple14 _tmp769;return(_tmp769.f1=_tmp2A8,((_tmp769.f2=_tmp14F->notzeroall,_tmp769)));}}else{
# 1214
struct _tuple14 _tmp76A;return(_tmp76A.f1=_tmp2A8,((_tmp76A.f2=_tmp14F->unknown_all,_tmp76A)));}}_LL1E3:;_LL1E4: {
# 1216
struct Cyc_Core_Impossible_exn_struct _tmp777;const char*_tmp776;void*_tmp775[1];struct Cyc_String_pa_PrintArg_struct _tmp774;struct Cyc_Core_Impossible_exn_struct*_tmp773;(int)_throw((void*)((_tmp773=_cycalloc(sizeof(*_tmp773)),((_tmp773[0]=((_tmp777.tag=Cyc_Core_Impossible,((_tmp777.f1=(struct _dyneither_ptr)((_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp775[0]=& _tmp774,Cyc_aprintf(((_tmp776="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp776,sizeof(char),26))),_tag_dyneither(_tmp775,sizeof(void*),1)))))))),_tmp777)))),_tmp773)))));}_LL1E0:;};}_LL15C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1AD->tag != 21)goto _LL15E;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LL15D: {
# 1221
union Cyc_CfFlowInfo_FlowInfo _tmp2BA;void*_tmp2BB;struct _tuple14 _tmp2B9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AE);_tmp2BA=_tmp2B9.f1;_tmp2BB=_tmp2B9.f2;{
# 1224
union Cyc_CfFlowInfo_FlowInfo _tmp2BD;void*_tmp2BE;struct _tuple14 _tmp2BC=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BA,_tmp1AE,_tmp2BB);_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;{
# 1227
void*_tmp2BF=Cyc_Tcutil_compress((void*)_check_null(_tmp1AE->topt));void*_tmp2C1;_LL1E6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF;if(_tmp2C0->tag != 5)goto _LL1E8;else{_tmp2C1=(_tmp2C0->f1).elt_typ;}}_LL1E7:
# 1229
 if(Cyc_Absyn_is_nontagged_union_type(_tmp2C1)){
struct _tuple14 _tmp778;return(_tmp778.f1=_tmp2BD,((_tmp778.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp778)));}{
void*_tmp2C3=_tmp2BE;int _tmp2C5;int _tmp2C6;struct _dyneither_ptr _tmp2C7;_LL1EB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C3;if(_tmp2C4->tag != 6)goto _LL1ED;else{_tmp2C5=(_tmp2C4->f1).is_union;_tmp2C6=(_tmp2C4->f1).fieldnum;_tmp2C7=_tmp2C4->f2;}}_LL1EC: {
# 1233
int _tmp2C8=Cyc_CfFlowInfo_get_field_index(_tmp2C1,_tmp1AF);
if((_tmp2C5  && _tmp2C6 != - 1) && _tmp2C6 != _tmp2C8){
struct _tuple14 _tmp779;return(_tmp779.f1=_tmp2BD,((_tmp779.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),1,_tmp14F->unknown_none),_tmp779)));}{
struct _tuple14 _tmp77A;return(_tmp77A.f1=_tmp2BD,((_tmp77A.f2=*((void**)_check_dyneither_subscript(_tmp2C7,sizeof(void*),_tmp2C8)),_tmp77A)));};}_LL1ED:;_LL1EE: {
struct Cyc_Core_Impossible_exn_struct _tmp780;const char*_tmp77F;struct Cyc_Core_Impossible_exn_struct*_tmp77E;(int)_throw((void*)((_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E[0]=((_tmp780.tag=Cyc_Core_Impossible,((_tmp780.f1=((_tmp77F="anal_Rexp: AggrArrow",_tag_dyneither(_tmp77F,sizeof(char),21))),_tmp780)))),_tmp77E)))));}_LL1EA:;};_LL1E8:;_LL1E9: {
# 1239
struct Cyc_Core_Impossible_exn_struct _tmp786;const char*_tmp785;struct Cyc_Core_Impossible_exn_struct*_tmp784;(int)_throw((void*)((_tmp784=_cycalloc(sizeof(*_tmp784)),((_tmp784[0]=((_tmp786.tag=Cyc_Core_Impossible,((_tmp786.f1=((_tmp785="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp785,sizeof(char),25))),_tmp786)))),_tmp784)))));}_LL1E5:;};};}_LL15E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1B0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1B0->tag != 5)goto _LL160;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;_tmp1B3=_tmp1B0->f3;}}_LL15F: {
# 1243
union Cyc_CfFlowInfo_FlowInfo _tmp2D2;union Cyc_CfFlowInfo_FlowInfo _tmp2D3;struct _tuple16 _tmp2D1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B1);_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;{
struct _tuple14 _tmp2D4=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D2,_tmp1B2);
struct _tuple14 _tmp2D5=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D3,_tmp1B3);
# 1247
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,env->all_changed,_tmp2D4,_tmp2D5);};}_LL160: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1B4->tag != 6)goto _LL162;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;}}_LL161: {
# 1250
union Cyc_CfFlowInfo_FlowInfo _tmp2D7;union Cyc_CfFlowInfo_FlowInfo _tmp2D8;struct _tuple16 _tmp2D6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B5);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2DA;void*_tmp2DB;struct _tuple14 _tmp2D9=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D7,_tmp1B6);_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;{
struct _tuple14 _tmp787;struct _tuple14 _tmp2DC=(_tmp787.f1=_tmp2DA,((_tmp787.f2=_tmp2DB,_tmp787)));
struct _tuple14 _tmp788;struct _tuple14 _tmp2DD=(_tmp788.f1=_tmp2D8,((_tmp788.f2=_tmp14F->zero,_tmp788)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,env->all_changed,_tmp2DC,_tmp2DD);};};}_LL162: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1B7->tag != 7)goto _LL164;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}_LL163: {
# 1257
union Cyc_CfFlowInfo_FlowInfo _tmp2E1;union Cyc_CfFlowInfo_FlowInfo _tmp2E2;struct _tuple16 _tmp2E0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1B8);_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2E4;void*_tmp2E5;struct _tuple14 _tmp2E3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2E2,_tmp1B9);_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;{
struct _tuple14 _tmp789;struct _tuple14 _tmp2E6=(_tmp789.f1=_tmp2E4,((_tmp789.f2=_tmp2E5,_tmp789)));
struct _tuple14 _tmp78A;struct _tuple14 _tmp2E7=(_tmp78A.f1=_tmp2E1,((_tmp78A.f2=_tmp14F->notzeroall,_tmp78A)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14F,env->all_changed,_tmp2E6,_tmp2E7);};};}_LL164: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1BA->tag != 22)goto _LL166;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL165: {
# 1264
struct _RegionHandle _tmp2EA=_new_region("temp");struct _RegionHandle*temp=& _tmp2EA;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Exp*_tmp78B[2];struct _tuple20 _tmp2EC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp78B[1]=_tmp1BC,((_tmp78B[0]=_tmp1BB,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp78B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2ED=_tmp2EC.f1;_tmp2EE=_tmp2EC.f2;{
# 1269
union Cyc_CfFlowInfo_FlowInfo _tmp2EF=_tmp2ED;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F0=_tmp2ED;struct Cyc_Dict_Dict _tmp2F1;struct Cyc_List_List*_tmp2F2;_LL1F0: if((_tmp2F0.ReachableFL).tag != 2)goto _LL1F2;_tmp2F1=((struct _tuple13)(_tmp2F0.ReachableFL).val).f1;_tmp2F2=((struct _tuple13)(_tmp2F0.ReachableFL).val).f2;_LL1F1:
# 1274
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2F1,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2EE))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(_tmp1BC->loc,((_tmp78E="expression may not be initialized",_tag_dyneither(_tmp78E,sizeof(char),34))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2F5=Cyc_NewControlFlow_add_subscript_reln(_tmp14F->r,_tmp2F2,_tmp1BB,_tmp1BC);
if(_tmp2F2 != _tmp2F5)
_tmp2EF=Cyc_CfFlowInfo_ReachableFL(_tmp2F1,_tmp2F5);
goto _LL1EF;};_LL1F2:;_LL1F3:
 goto _LL1EF;_LL1EF:;}{
# 1282
void*_tmp2F6=Cyc_Tcutil_compress((void*)_check_null(_tmp1BB->topt));struct Cyc_List_List*_tmp2F8;union Cyc_Absyn_Constraint*_tmp2FA;_LL1F5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6;if(_tmp2F7->tag != 10)goto _LL1F7;else{_tmp2F8=_tmp2F7->f1;}}_LL1F6: {
# 1284
void*_tmp2FB=(void*)((struct Cyc_List_List*)_check_null(_tmp2EE))->hd;struct _dyneither_ptr _tmp2FD;_LL1FC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2FB;if(_tmp2FC->tag != 6)goto _LL1FE;else{_tmp2FD=_tmp2FC->f2;}}_LL1FD: {
# 1286
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1BC)).f1;
struct _tuple14 _tmp78F;struct _tuple14 _tmp2FF=(_tmp78F.f1=_tmp2EF,((_tmp78F.f2=*((void**)_check_dyneither_subscript(_tmp2FD,sizeof(void*),(int)i)),_tmp78F)));_npop_handler(0);return _tmp2FF;}_LL1FE:;_LL1FF: {
struct Cyc_Core_Impossible_exn_struct _tmp795;const char*_tmp794;struct Cyc_Core_Impossible_exn_struct*_tmp793;(int)_throw((void*)((_tmp793=_cycalloc(sizeof(*_tmp793)),((_tmp793[0]=((_tmp795.tag=Cyc_Core_Impossible,((_tmp795.f1=((_tmp794="anal_Rexp: Subscript",_tag_dyneither(_tmp794,sizeof(char),21))),_tmp795)))),_tmp793)))));}_LL1FB:;}_LL1F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6;if(_tmp2F9->tag != 5)goto _LL1F9;else{_tmp2FA=((_tmp2F9->f1).ptr_atts).bounds;}}_LL1F8: {
# 1291
union Cyc_CfFlowInfo_FlowInfo _tmp304;void*_tmp305;struct _tuple14 _tmp303=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2ED,_tmp1BB,(void*)((struct Cyc_List_List*)_check_null(_tmp2EE))->hd);_tmp304=_tmp303.f1;_tmp305=_tmp303.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp306=_tmp304;_LL201: if((_tmp306.BottomFL).tag != 1)goto _LL203;_LL202: {
struct _tuple14 _tmp796;struct _tuple14 _tmp308=(_tmp796.f1=_tmp304,((_tmp796.f2=_tmp305,_tmp796)));_npop_handler(0);return _tmp308;}_LL203:;_LL204: {
struct _tuple14 _tmp797;struct _tuple14 _tmp30A=(_tmp797.f1=_tmp2EF,((_tmp797.f2=_tmp305,_tmp797)));_npop_handler(0);return _tmp30A;}_LL200:;};}_LL1F9:;_LL1FA: {
# 1296
struct Cyc_Core_Impossible_exn_struct _tmp79D;const char*_tmp79C;struct Cyc_Core_Impossible_exn_struct*_tmp79B;(int)_throw((void*)((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B[0]=((_tmp79D.tag=Cyc_Core_Impossible,((_tmp79D.f1=((_tmp79C="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp79C,sizeof(char),33))),_tmp79D)))),_tmp79B)))));}_LL1F4:;};};}
# 1265
;_pop_region(temp);}_LL166: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1BD->tag != 29)goto _LL168;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;}}_LL167:
# 1301
 _tmp1C1=_tmp1BE;goto _LL169;_LL168: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1C0->tag != 23)goto _LL16A;else{_tmp1C1=_tmp1C0->f1;}}_LL169: {
# 1303
struct _RegionHandle _tmp30E=_new_region("temp");struct _RegionHandle*temp=& _tmp30E;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp310;struct Cyc_List_List*_tmp311;struct _tuple20 _tmp30F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1C1,1,1);_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{
unsigned int _tmp7A3;unsigned int _tmp7A2;struct _dyneither_ptr _tmp7A1;void**_tmp7A0;unsigned int _tmp79F;struct _dyneither_ptr aggrdict=
(_tmp79F=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1C1),((_tmp7A0=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp79F)),((_tmp7A1=_tag_dyneither(_tmp7A0,sizeof(void*),_tmp79F),((((_tmp7A2=_tmp79F,_tmp7A4(& _tmp7A3,& _tmp7A2,& _tmp7A0,& _tmp311))),_tmp7A1)))))));
# 1311
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7AE;struct Cyc_CfFlowInfo_UnionRInfo _tmp7AD;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7AC;struct _tuple14 _tmp7AB;struct _tuple14 _tmp316=(_tmp7AB.f1=_tmp310,((_tmp7AB.f2=(void*)((_tmp7AE=_region_malloc(env->r,sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7AC.tag=6,((_tmp7AC.f1=((_tmp7AD.is_union=0,((_tmp7AD.fieldnum=- 1,_tmp7AD)))),((_tmp7AC.f2=aggrdict,_tmp7AC)))))),_tmp7AE)))),_tmp7AB)));_npop_handler(0);return _tmp316;};}
# 1304
;_pop_region(temp);}_LL16A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1C2->tag != 28)goto _LL16C;else{_tmp1C3=_tmp1C2->f2;}}_LL16B: {
# 1314
struct Cyc_List_List*fs;
{void*_tmp31C=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp31E;_LL206: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31D->tag != 12)goto _LL208;else{_tmp31E=_tmp31D->f2;}}_LL207:
 fs=_tmp31E;goto _LL205;_LL208:;_LL209: {
struct Cyc_Core_Impossible_exn_struct _tmp7B4;const char*_tmp7B3;struct Cyc_Core_Impossible_exn_struct*_tmp7B2;(int)_throw((void*)((_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B4.tag=Cyc_Core_Impossible,((_tmp7B4.f1=((_tmp7B3="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7B3,sizeof(char),35))),_tmp7B4)))),_tmp7B2)))));}_LL205:;}
# 1319
_tmp1C5=_tmp1C3;_tmp1C6=Cyc_Absyn_StructA;_tmp1C7=fs;goto _LL16D;}_LL16C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1C4->tag != 27)goto _LL16E;else{_tmp1C5=_tmp1C4->f3;if(_tmp1C4->f4 == 0)goto _LL16E;_tmp1C6=(*_tmp1C4->f4).kind;if((*_tmp1C4->f4).impl == 0)goto _LL16E;_tmp1C7=(*(*_tmp1C4->f4).impl).fields;}}_LL16D: {
# 1321
struct _RegionHandle _tmp322=_new_region("temp");struct _RegionHandle*temp=& _tmp322;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp324;struct Cyc_List_List*_tmp325;struct _tuple20 _tmp323=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1C5),1,1);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14F,_tmp1C7,_tmp1C6 == Cyc_Absyn_UnionA,_tmp1C6 == Cyc_Absyn_UnionA?_tmp14F->unknown_none: _tmp14F->unknown_all);
int field_no=- 1;
{int i=0;for(0;_tmp325 != 0;(((_tmp325=_tmp325->tl,_tmp1C5=_tmp1C5->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp326=(void*)ds->hd;struct _dyneither_ptr*_tmp329;_LL20B: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp327=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp326;if(_tmp327->tag != 0)goto _LL20D;}_LL20C: {
struct Cyc_Core_Impossible_exn_struct _tmp7BA;const char*_tmp7B9;struct Cyc_Core_Impossible_exn_struct*_tmp7B8;(int)_throw((void*)((_tmp7B8=_cycalloc(sizeof(*_tmp7B8)),((_tmp7B8[0]=((_tmp7BA.tag=Cyc_Core_Impossible,((_tmp7BA.f1=((_tmp7B9="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7B9,sizeof(char),22))),_tmp7BA)))),_tmp7B8)))));}_LL20D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp328=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp326;if(_tmp328->tag != 1)goto _LL20A;else{_tmp329=_tmp328->f1;}}_LL20E:
# 1333
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1C7,_tmp329);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp325->hd;
# 1336
if(_tmp1C6 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp32D=(*((struct _tuple22*)_tmp1C5->hd)).f2;
_tmp324=Cyc_NewControlFlow_use_Rval(env,_tmp32D->loc,_tmp324,(void*)_tmp325->hd);}_LL20A:;}}}{
# 1341
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7C4;struct Cyc_CfFlowInfo_UnionRInfo _tmp7C3;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7C2;struct _tuple14 _tmp7C1;struct _tuple14 _tmp332=(_tmp7C1.f1=_tmp324,((_tmp7C1.f2=(void*)((_tmp7C4=_region_malloc(env->r,sizeof(*_tmp7C4)),((_tmp7C4[0]=((_tmp7C2.tag=6,((_tmp7C2.f1=((_tmp7C3.is_union=_tmp1C6 == Cyc_Absyn_UnionA,((_tmp7C3.fieldnum=field_no,_tmp7C3)))),((_tmp7C2.f2=aggrdict,_tmp7C2)))))),_tmp7C4)))),_tmp7C1)));_npop_handler(0);return _tmp332;};};}
# 1322
;_pop_region(temp);}_LL16E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1C8->tag != 27)goto _LL170;}_LL16F: {
# 1344
struct Cyc_Core_Impossible_exn_struct _tmp7CA;const char*_tmp7C9;struct Cyc_Core_Impossible_exn_struct*_tmp7C8;(int)_throw((void*)((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CA.tag=Cyc_Core_Impossible,((_tmp7CA.f1=((_tmp7C9="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp7C9,sizeof(char),31))),_tmp7CA)))),_tmp7C8)))));}_LL170: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1C9->tag != 25)goto _LL172;else{_tmp1CA=_tmp1C9->f1;}}_LL171: {
# 1346
struct _RegionHandle _tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);
{struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp1CA);
union Cyc_CfFlowInfo_FlowInfo _tmp339;struct Cyc_List_List*_tmp33A;struct _tuple20 _tmp338=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp337,1,1);_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;
for(0;_tmp33A != 0;(_tmp33A=_tmp33A->tl,_tmp337=_tmp337->tl)){
_tmp339=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp337))->hd)->loc,_tmp339,(void*)_tmp33A->hd);}{
struct _tuple14 _tmp7CB;struct _tuple14 _tmp33C=(_tmp7CB.f1=_tmp339,((_tmp7CB.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp14F,(void*)_check_null(e->topt),0,_tmp14F->unknown_all),_tmp7CB)));_npop_handler(0);return _tmp33C;};}
# 1347
;_pop_region(temp);}_LL172: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1CB->tag != 26)goto _LL174;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CB->f2;_tmp1CE=_tmp1CB->f3;_tmp1CF=_tmp1CB->f4;}}_LL173: {
# 1355
union Cyc_CfFlowInfo_FlowInfo _tmp33E;void*_tmp33F;struct _tuple14 _tmp33D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CD);_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp340=_tmp33E;struct Cyc_Dict_Dict _tmp341;struct Cyc_List_List*_tmp342;_LL210: if((_tmp340.BottomFL).tag != 1)goto _LL212;_LL211: {
struct _tuple14 _tmp7CC;return(_tmp7CC.f1=_tmp33E,((_tmp7CC.f2=_tmp14F->unknown_all,_tmp7CC)));}_LL212: if((_tmp340.ReachableFL).tag != 2)goto _LL20F;_tmp341=((struct _tuple13)(_tmp340.ReachableFL).val).f1;_tmp342=((struct _tuple13)(_tmp340.ReachableFL).val).f2;_LL213:
# 1359
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp341,_tmp33F)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp7CF="expression may not be initialized",_tag_dyneither(_tmp7CF,sizeof(char),34))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}{
# 1363
struct Cyc_List_List*new_relns=_tmp342;
comp_loop: {
void*_tmp346=_tmp1CD->r;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp351;struct Cyc_Absyn_Vardecl*_tmp354;int _tmp356;enum Cyc_Absyn_Primop _tmp358;struct Cyc_Absyn_Exp*_tmp359;_LL215: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp347=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp347->tag != 13)goto _LL217;else{_tmp348=_tmp347->f2;}}_LL216:
 _tmp1CD=_tmp348;goto comp_loop;_LL217:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp349=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp349->tag != 1)goto _LL219;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp34A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp349->f2);if(_tmp34A->tag != 1)goto _LL219;else{_tmp34B=_tmp34A->f1;}}}if(!(!_tmp34B->escapes))goto _LL219;_LL218:
 _tmp34E=_tmp34B;goto _LL21A;_LL219:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp34C->tag != 1)goto _LL21B;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp34D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp34C->f2);if(_tmp34D->tag != 4)goto _LL21B;else{_tmp34E=_tmp34D->f1;}}}if(!(!_tmp34E->escapes))goto _LL21B;_LL21A:
 _tmp351=_tmp34E;goto _LL21C;_LL21B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp34F->tag != 1)goto _LL21D;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp350=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp34F->f2);if(_tmp350->tag != 5)goto _LL21D;else{_tmp351=_tmp350->f1;}}}if(!(!_tmp351->escapes))goto _LL21D;_LL21C:
 _tmp354=_tmp351;goto _LL21E;_LL21D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp352=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp352->tag != 1)goto _LL21F;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp353=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp352->f2);if(_tmp353->tag != 3)goto _LL21F;else{_tmp354=_tmp353->f1;}}}if(!(!_tmp354->escapes))goto _LL21F;_LL21E:
# 1371
{struct Cyc_CfFlowInfo_Reln*_tmp7D2;struct Cyc_List_List*_tmp7D1;new_relns=((_tmp7D1=_region_malloc(env->r,sizeof(*_tmp7D1)),((_tmp7D1->hd=((_tmp7D2=_region_malloc(env->r,sizeof(*_tmp7D2)),((_tmp7D2->vd=_tmp1CC,((_tmp7D2->rop=Cyc_CfFlowInfo_LessVar(_tmp354,_tmp354->type),_tmp7D2)))))),((_tmp7D1->tl=_tmp342,_tmp7D1))))));}
goto _LL214;_LL21F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp355=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp355->tag != 0)goto _LL221;else{if(((_tmp355->f1).Int_c).tag != 5)goto _LL221;_tmp356=((struct _tuple6)((_tmp355->f1).Int_c).val).f2;}}_LL220:
# 1374
{struct Cyc_CfFlowInfo_Reln*_tmp7D5;struct Cyc_List_List*_tmp7D4;new_relns=((_tmp7D4=_region_malloc(env->r,sizeof(*_tmp7D4)),((_tmp7D4->hd=((_tmp7D5=_region_malloc(env->r,sizeof(*_tmp7D5)),((_tmp7D5->vd=_tmp1CC,((_tmp7D5->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp356),_tmp7D5)))))),((_tmp7D4->tl=_tmp342,_tmp7D4))))));}
goto _LL214;_LL221: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp357=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp357->tag != 2)goto _LL223;else{_tmp358=_tmp357->f1;if(_tmp357->f2 == 0)goto _LL223;_tmp359=(struct Cyc_Absyn_Exp*)(*_tmp357->f2).hd;}}_LL222:
# 1377
{void*_tmp35E=_tmp359->r;struct Cyc_Absyn_Vardecl*_tmp361;struct Cyc_Absyn_Vardecl*_tmp364;struct Cyc_Absyn_Vardecl*_tmp367;struct Cyc_Absyn_Vardecl*_tmp36A;_LL226:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp35F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp35F->tag != 1)goto _LL228;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp360=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp35F->f2);if(_tmp360->tag != 1)goto _LL228;else{_tmp361=_tmp360->f1;}}}if(!(!_tmp361->escapes))goto _LL228;_LL227:
 _tmp364=_tmp361;goto _LL229;_LL228:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp362=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp362->tag != 1)goto _LL22A;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp363=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp362->f2);if(_tmp363->tag != 4)goto _LL22A;else{_tmp364=_tmp363->f1;}}}if(!(!_tmp364->escapes))goto _LL22A;_LL229:
 _tmp367=_tmp364;goto _LL22B;_LL22A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp365=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp365->tag != 1)goto _LL22C;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp366=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp365->f2);if(_tmp366->tag != 5)goto _LL22C;else{_tmp367=_tmp366->f1;}}}if(!(!_tmp367->escapes))goto _LL22C;_LL22B:
 _tmp36A=_tmp367;goto _LL22D;_LL22C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp368=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp368->tag != 1)goto _LL22E;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp369=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp368->f2);if(_tmp369->tag != 3)goto _LL22E;else{_tmp36A=_tmp369->f1;}}}if(!(!_tmp36A->escapes))goto _LL22E;_LL22D:
# 1383
{struct Cyc_CfFlowInfo_Reln*_tmp7D8;struct Cyc_List_List*_tmp7D7;new_relns=((_tmp7D7=_region_malloc(env->r,sizeof(*_tmp7D7)),((_tmp7D7->hd=((_tmp7D8=_region_malloc(env->r,sizeof(*_tmp7D8)),((_tmp7D8->vd=_tmp1CC,((_tmp7D8->rop=Cyc_CfFlowInfo_LessNumelts(_tmp36A),_tmp7D8)))))),((_tmp7D7->tl=_tmp342,_tmp7D7))))));}
goto _LL225;_LL22E:;_LL22F:
 goto _LL225;_LL225:;}
# 1387
goto _LL214;_LL223:;_LL224:
 goto _LL214;_LL214:;}
# 1390
if(_tmp342 != new_relns)
_tmp33E=Cyc_CfFlowInfo_ReachableFL(_tmp341,new_relns);{
# 1394
void*_tmp36D=_tmp33F;_LL231: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp36E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp36E->tag != 0)goto _LL233;}_LL232: {
struct _tuple14 _tmp7D9;return(_tmp7D9.f1=_tmp33E,((_tmp7D9.f2=_tmp14F->unknown_all,_tmp7D9)));}_LL233: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp36F=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp36F->tag != 2)goto _LL235;}_LL234:
 goto _LL236;_LL235: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp370=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp370->tag != 1)goto _LL237;}_LL236:
 goto _LL238;_LL237: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp371=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp371->tag != 5)goto _LL239;}_LL238: {
# 1399
struct Cyc_List_List _tmp7DA;struct Cyc_List_List _tmp373=(_tmp7DA.hd=_tmp1CC,((_tmp7DA.tl=0,_tmp7DA)));
_tmp33E=Cyc_NewControlFlow_add_vars(_tmp14F,_tmp33E,(struct Cyc_List_List*)& _tmp373,_tmp14F->unknown_all,e->loc);{
# 1402
union Cyc_CfFlowInfo_FlowInfo _tmp375;void*_tmp376;struct _tuple14 _tmp374=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp33E,_tmp1CE);_tmp375=_tmp374.f1;_tmp376=_tmp374.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp377=_tmp375;struct Cyc_Dict_Dict _tmp378;_LL23C: if((_tmp377.BottomFL).tag != 1)goto _LL23E;_LL23D: {
struct _tuple14 _tmp7DB;return(_tmp7DB.f1=_tmp375,((_tmp7DB.f2=_tmp14F->unknown_all,_tmp7DB)));}_LL23E: if((_tmp377.ReachableFL).tag != 2)goto _LL23B;_tmp378=((struct _tuple13)(_tmp377.ReachableFL).val).f1;_LL23F:
# 1406
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp378,_tmp376)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7DE;void*_tmp7DD;(_tmp7DD=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp7DE="expression may not be initialized",_tag_dyneither(_tmp7DE,sizeof(char),34))),_tag_dyneither(_tmp7DD,sizeof(void*),0)));}{
struct _tuple14 _tmp7DF;return(_tmp7DF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DF.f2=_tmp14F->unknown_all,_tmp7DF)));};}_LL23B:;}
# 1411
_tmp33E=_tmp375;
goto _LL23A;};}_LL239:;_LL23A:
# 1414
 while(1){
struct Cyc_List_List _tmp7E0;struct Cyc_List_List _tmp37E=(_tmp7E0.hd=_tmp1CC,((_tmp7E0.tl=0,_tmp7E0)));
_tmp33E=Cyc_NewControlFlow_add_vars(_tmp14F,_tmp33E,(struct Cyc_List_List*)& _tmp37E,_tmp14F->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp380;void*_tmp381;struct _tuple14 _tmp37F=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp33E,_tmp1CE);_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp382=_tmp380;struct Cyc_Dict_Dict _tmp383;_LL241: if((_tmp382.BottomFL).tag != 1)goto _LL243;_LL242:
 goto _LL240;_LL243: if((_tmp382.ReachableFL).tag != 2)goto _LL240;_tmp383=((struct _tuple13)(_tmp382.ReachableFL).val).f1;_LL244:
# 1421
 if(Cyc_CfFlowInfo_initlevel(_tmp14F,_tmp383,_tmp381)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp7E3;void*_tmp7E2;(_tmp7E2=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp7E3="expression may not be initialized",_tag_dyneither(_tmp7E3,sizeof(char),34))),_tag_dyneither(_tmp7E2,sizeof(void*),0)));}{
struct _tuple14 _tmp7E4;return(_tmp7E4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E4.f2=_tmp14F->unknown_all,_tmp7E4)));};}_LL240:;}{
# 1426
union Cyc_CfFlowInfo_FlowInfo _tmp387=Cyc_CfFlowInfo_join_flow(_tmp14F,env->all_changed,_tmp33E,_tmp380);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp387,_tmp33E))
break;
_tmp33E=_tmp387;};};}{
# 1431
struct _tuple14 _tmp7E5;return(_tmp7E5.f1=_tmp33E,((_tmp7E5.f2=_tmp14F->unknown_all,_tmp7E5)));};_LL230:;};};_LL20F:;};}_LL174: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D0->tag != 35)goto _LL176;else{_tmp1D1=_tmp1D0->f1;}}_LL175:
# 1434
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1D1);_LL176: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D2->tag != 1)goto _LL178;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1D3=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1D2->f2);if(_tmp1D3->tag != 0)goto _LL178;}}_LL177:
 goto _LL179;_LL178: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D4->tag != 34)goto _LL17A;}_LL179:
 goto _LL17B;_LL17A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1D5=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D5->tag != 24)goto _LL17C;}_LL17B:
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D6->tag != 37)goto _LL17E;}_LL17D:
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp1D7->tag != 38)goto _LL119;}_LL17F: {
# 1440
struct Cyc_Core_Impossible_exn_struct _tmp7EB;const char*_tmp7EA;struct Cyc_Core_Impossible_exn_struct*_tmp7E9;(int)_throw((void*)((_tmp7E9=_cycalloc(sizeof(*_tmp7E9)),((_tmp7E9[0]=((_tmp7EB.tag=Cyc_Core_Impossible,((_tmp7EB.f1=((_tmp7EA="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp7EA,sizeof(char),31))),_tmp7EB)))),_tmp7E9)))));}_LL119:;};}
# 1449
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1456
struct Cyc_CfFlowInfo_FlowEnv*_tmp38D=env->fenv;
void*_tmp38E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp390;union Cyc_Absyn_Constraint*_tmp391;union Cyc_Absyn_Constraint*_tmp392;_LL246: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E;if(_tmp38F->tag != 5)goto _LL248;else{_tmp390=(_tmp38F->f1).elt_typ;_tmp391=((_tmp38F->f1).ptr_atts).bounds;_tmp392=((_tmp38F->f1).ptr_atts).zero_term;}}_LL247: {
# 1459
union Cyc_CfFlowInfo_FlowInfo _tmp393=f;struct Cyc_Dict_Dict _tmp394;struct Cyc_List_List*_tmp395;_LL24B: if((_tmp393.BottomFL).tag != 1)goto _LL24D;_LL24C: {
struct _tuple15 _tmp7EC;return(_tmp7EC.f1=f,((_tmp7EC.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7EC)));}_LL24D: if((_tmp393.ReachableFL).tag != 2)goto _LL24A;_tmp394=((struct _tuple13)(_tmp393.ReachableFL).val).f1;_tmp395=((struct _tuple13)(_tmp393.ReachableFL).val).f2;_LL24E:
# 1463
 if(Cyc_Tcutil_is_bound_one(_tmp391)){
void*_tmp397=r;void*_tmp39B;struct Cyc_List_List*_tmp39C;enum Cyc_CfFlowInfo_InitLevel _tmp39F;_LL250: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp398=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp397;if(_tmp398->tag != 1)goto _LL252;}_LL251:
 goto _LL253;_LL252: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp399=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp397;if(_tmp399->tag != 2)goto _LL254;}_LL253:
# 1467
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7EF;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7EE;e->annot=(void*)((_tmp7EE=_cycalloc(sizeof(*_tmp7EE)),((_tmp7EE[0]=((_tmp7EF.tag=Cyc_CfFlowInfo_NotZero,((_tmp7EF.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp395),_tmp7EF)))),_tmp7EE))));}goto _LL24F;_LL254: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp39A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp397;if(_tmp39A->tag != 5)goto _LL256;else{_tmp39B=(*_tmp39A->f1).root;_tmp39C=(*_tmp39A->f1).fields;}}_LL255:
# 1469
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7F2;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7F1;e->annot=(void*)((_tmp7F1=_cycalloc(sizeof(*_tmp7F1)),((_tmp7F1[0]=((_tmp7F2.tag=Cyc_CfFlowInfo_NotZero,((_tmp7F2.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp395),_tmp7F2)))),_tmp7F1))));}{
struct Cyc_CfFlowInfo_Place*_tmp7F5;struct _tuple15 _tmp7F4;return(_tmp7F4.f1=f,((_tmp7F4.f2=Cyc_CfFlowInfo_PlaceL(((_tmp7F5=_region_malloc(_tmp38D->r,sizeof(*_tmp7F5)),((_tmp7F5->root=_tmp39B,((_tmp7F5->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp38D->r,_tmp39C,flds),_tmp7F5))))))),_tmp7F4)));};_LL256: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp39D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp397;if(_tmp39D->tag != 0)goto _LL258;}_LL257:
# 1472
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp7F6;return(_tmp7F6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F6.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7F6)));};_LL258: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp39E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp397;if(_tmp39E->tag != 3)goto _LL25A;else{_tmp39F=_tmp39E->f1;}}_LL259:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp39F);goto _LL25B;_LL25A:;_LL25B: {
# 1476
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7F9;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7F8;e->annot=(void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7F9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp395),_tmp7F9)))),_tmp7F8))));}_LL24F:;}else{
# 1479
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7FC;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7FB;e->annot=(void*)((_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((_tmp7FB[0]=((_tmp7FC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7FC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp395),_tmp7FC)))),_tmp7FB))));}
if(Cyc_CfFlowInfo_initlevel(_tmp38D,_tmp394,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7FF;void*_tmp7FE;(_tmp7FE=0,Cyc_Tcutil_terr(e->loc,((_tmp7FF="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7FF,sizeof(char),46))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}{
struct _tuple15 _tmp800;return(_tmp800.f1=f,((_tmp800.f2=Cyc_CfFlowInfo_UnknownL(),_tmp800)));};_LL24A:;}_LL248:;_LL249: {
# 1484
struct Cyc_Core_Impossible_exn_struct _tmp806;const char*_tmp805;struct Cyc_Core_Impossible_exn_struct*_tmp804;(int)_throw((void*)((_tmp804=_cycalloc(sizeof(*_tmp804)),((_tmp804[0]=((_tmp806.tag=Cyc_Core_Impossible,((_tmp806.f1=((_tmp805="left deref of non-pointer-type",_tag_dyneither(_tmp805,sizeof(char),31))),_tmp806)))),_tmp804)))));}_LL245:;}
# 1492
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1495
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B1=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3B2=inflow;struct Cyc_Dict_Dict _tmp3B3;struct Cyc_List_List*_tmp3B4;_LL25D: if((_tmp3B2.BottomFL).tag != 1)goto _LL25F;_LL25E: {
struct _tuple15 _tmp807;return(_tmp807.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp807.f2=Cyc_CfFlowInfo_UnknownL(),_tmp807)));}_LL25F: if((_tmp3B2.ReachableFL).tag != 2)goto _LL25C;_tmp3B3=((struct _tuple13)(_tmp3B2.ReachableFL).val).f1;_tmp3B4=((struct _tuple13)(_tmp3B2.ReachableFL).val).f2;_LL260:
# 1500
 d=_tmp3B3;_LL25C:;}{
# 1504
void*_tmp3B6=e->r;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Vardecl*_tmp3BF;struct Cyc_Absyn_Vardecl*_tmp3C2;struct Cyc_Absyn_Vardecl*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CA;struct _dyneither_ptr*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct _dyneither_ptr*_tmp3D3;_LL262: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3B7->tag != 13)goto _LL264;else{_tmp3B8=_tmp3B7->f2;}}_LL263:
 _tmp3BA=_tmp3B8;goto _LL265;_LL264: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3B9=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3B9->tag != 11)goto _LL266;else{_tmp3BA=_tmp3B9->f1;}}_LL265:
 _tmp3BC=_tmp3BA;goto _LL267;_LL266: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp3BB=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3BB->tag != 12)goto _LL268;else{_tmp3BC=_tmp3BB->f1;}}_LL267:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3BC,flds);_LL268: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3BD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3BD->tag != 1)goto _LL26A;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3BE=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp3BD->f2);if(_tmp3BE->tag != 3)goto _LL26A;else{_tmp3BF=_tmp3BE->f1;}}}_LL269:
# 1509
 _tmp3C2=_tmp3BF;goto _LL26B;_LL26A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3C0->tag != 1)goto _LL26C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3C1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3C0->f2);if(_tmp3C1->tag != 4)goto _LL26C;else{_tmp3C2=_tmp3C1->f1;}}}_LL26B:
 _tmp3C5=_tmp3C2;goto _LL26D;_LL26C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3C3->tag != 1)goto _LL26E;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3C4=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3C3->f2);if(_tmp3C4->tag != 5)goto _LL26E;else{_tmp3C5=_tmp3C4->f1;}}}_LL26D: {
# 1512
struct Cyc_CfFlowInfo_Place*_tmp811;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp810;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp80F;struct _tuple15 _tmp80E;return(_tmp80E.f1=inflow,((_tmp80E.f2=Cyc_CfFlowInfo_PlaceL(((_tmp811=_region_malloc(env->r,sizeof(*_tmp811)),((_tmp811->root=(void*)((_tmp80F=_region_malloc(env->r,sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=0,((_tmp810.f1=_tmp3C5,_tmp810)))),_tmp80F)))),((_tmp811->fields=flds,_tmp811))))))),_tmp80E)));}_LL26E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3C6->tag != 1)goto _LL270;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3C7=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3C6->f2);if(_tmp3C7->tag != 1)goto _LL270;else{_tmp3C8=_tmp3C7->f1;}}}_LL26F: {
# 1514
struct _tuple15 _tmp812;return(_tmp812.f1=inflow,((_tmp812.f2=Cyc_CfFlowInfo_UnknownL(),_tmp812)));}_LL270: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3C9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3C9->tag != 21)goto _LL272;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;}}_LL271:
# 1517
{void*_tmp3D9=Cyc_Tcutil_compress((void*)_check_null(_tmp3CA->topt));void*_tmp3DB;_LL27B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9;if(_tmp3DA->tag != 5)goto _LL27D;else{_tmp3DB=(_tmp3DA->f1).elt_typ;}}_LL27C:
# 1519
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp3DB)){
struct Cyc_List_List*_tmp813;flds=((_tmp813=_region_malloc(env->r,sizeof(*_tmp813)),((_tmp813->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3DB,_tmp3CB),((_tmp813->tl=flds,_tmp813))))));}
goto _LL27A;_LL27D:;_LL27E: {
struct Cyc_Core_Impossible_exn_struct _tmp819;const char*_tmp818;struct Cyc_Core_Impossible_exn_struct*_tmp817;(int)_throw((void*)((_tmp817=_cycalloc(sizeof(*_tmp817)),((_tmp817[0]=((_tmp819.tag=Cyc_Core_Impossible,((_tmp819.f1=((_tmp818="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp818,sizeof(char),25))),_tmp819)))),_tmp817)))));}_LL27A:;}
# 1524
_tmp3CD=_tmp3CA;goto _LL273;_LL272: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3CC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3CC->tag != 19)goto _LL274;else{_tmp3CD=_tmp3CC->f1;}}_LL273:
# 1526
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3CD->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp3E1;union Cyc_CfFlowInfo_AbsLVal _tmp3E2;struct _tuple15 _tmp3E0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3CD);_tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;{
struct _tuple15 _tmp81A;struct _tuple15 _tmp3E4=(_tmp81A.f1=_tmp3E1,((_tmp81A.f2=_tmp3E2,_tmp81A)));struct Cyc_Dict_Dict _tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_CfFlowInfo_Place*_tmp3E7;_LL280: if(((_tmp3E4.f1).ReachableFL).tag != 2)goto _LL282;_tmp3E5=((struct _tuple13)((_tmp3E4.f1).ReachableFL).val).f1;_tmp3E6=((struct _tuple13)((_tmp3E4.f1).ReachableFL).val).f2;if(((_tmp3E4.f2).PlaceL).tag != 1)goto _LL282;_tmp3E7=(struct Cyc_CfFlowInfo_Place*)((_tmp3E4.f2).PlaceL).val;_LL281: {
# 1530
void*_tmp3E8=Cyc_CfFlowInfo_lookup_place(_tmp3E5,_tmp3E7);
void*_tmp3E9=_tmp3E8;_LL285: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3EA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3E9;if(_tmp3EA->tag != 5)goto _LL287;}_LL286:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3E1,_tmp3CD,_tmp3E1,_tmp3E8,flds);_LL287:;_LL288: {
# 1539
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3B1,_tmp3E5,_tmp3E8);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3B1->unknown_all: _tmp3B1->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B1,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3CD->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp81D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp81C;void*new_root=(void*)((_tmp81C=_region_malloc(_tmp3B1->r,sizeof(*_tmp81C)),((_tmp81C[0]=((_tmp81D.tag=1,((_tmp81D.f1=e,((_tmp81D.f2=(void*)((void*)_check_null(e->topt)),_tmp81D)))))),_tmp81C))));
struct Cyc_CfFlowInfo_Place*_tmp81E;struct Cyc_CfFlowInfo_Place*place=(_tmp81E=_region_malloc(_tmp3B1->r,sizeof(*_tmp81E)),((_tmp81E->root=new_root,((_tmp81E->fields=0,_tmp81E)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp821;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp820;void*res=(void*)((_tmp820=_region_malloc(_tmp3B1->r,sizeof(*_tmp820)),((_tmp820[0]=((_tmp821.tag=5,((_tmp821.f1=place,_tmp821)))),_tmp820))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E5,new_root,new_rval);
_tmp3E5=Cyc_CfFlowInfo_assign_place(_tmp3B1,e->loc,_tmp3E5,env->all_changed,_tmp3E7,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3EB=Cyc_CfFlowInfo_ReachableFL(_tmp3E5,_tmp3E6);
# 1552
return Cyc_NewControlFlow_anal_derefL(env,_tmp3EB,_tmp3CD,_tmp3EB,res,flds);};}_LL284:;}_LL282:;_LL283:
# 1554
 goto _LL27F;_LL27F:;};}{
# 1557
union Cyc_CfFlowInfo_FlowInfo _tmp3F2;void*_tmp3F3;struct _tuple14 _tmp3F1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3CD);_tmp3F2=_tmp3F1.f1;_tmp3F3=_tmp3F1.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CD,_tmp3F2,_tmp3F3,flds);};_LL274: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3CE->tag != 22)goto _LL276;else{_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;}}_LL275: {
# 1561
void*_tmp3F4=Cyc_Tcutil_compress((void*)_check_null(_tmp3CF->topt));union Cyc_Absyn_Constraint*_tmp3F7;_LL28A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F5->tag != 10)goto _LL28C;}_LL28B: {
# 1563
unsigned int _tmp3F8=(Cyc_Evexp_eval_const_uint_exp(_tmp3D0)).f1;
struct Cyc_List_List*_tmp822;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3CF,((_tmp822=_region_malloc(env->r,sizeof(*_tmp822)),((_tmp822->hd=(void*)((int)_tmp3F8),((_tmp822->tl=flds,_tmp822)))))));}_LL28C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F6->tag != 5)goto _LL28E;else{_tmp3F7=((_tmp3F6->f1).ptr_atts).bounds;}}_LL28D: {
# 1566
struct _RegionHandle _tmp3FA=_new_region("temp");struct _RegionHandle*temp=& _tmp3FA;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp3FD;struct Cyc_List_List*_tmp3FE;struct Cyc_Absyn_Exp*_tmp823[2];struct _tuple20 _tmp3FC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp823[1]=_tmp3D0,((_tmp823[0]=_tmp3CF,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp823,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp3FD=_tmp3FC.f1;_tmp3FE=_tmp3FC.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3FF=_tmp3FD;
{union Cyc_CfFlowInfo_FlowInfo _tmp400=_tmp3FD;struct Cyc_Dict_Dict _tmp401;struct Cyc_List_List*_tmp402;_LL291: if((_tmp400.ReachableFL).tag != 2)goto _LL293;_tmp401=((struct _tuple13)(_tmp400.ReachableFL).val).f1;_tmp402=((struct _tuple13)(_tmp400.ReachableFL).val).f2;_LL292:
# 1572
 if(Cyc_CfFlowInfo_initlevel(_tmp3B1,_tmp401,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3FE))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp826;void*_tmp825;(_tmp825=0,Cyc_Tcutil_terr(_tmp3D0->loc,((_tmp826="expression may not be initialized",_tag_dyneither(_tmp826,sizeof(char),34))),_tag_dyneither(_tmp825,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp405=Cyc_NewControlFlow_add_subscript_reln(_tmp3B1->r,_tmp402,_tmp3CF,_tmp3D0);
if(_tmp402 != _tmp405)
_tmp3FF=Cyc_CfFlowInfo_ReachableFL(_tmp401,_tmp405);
goto _LL290;};_LL293:;_LL294:
 goto _LL290;_LL290:;}{
# 1580
union Cyc_CfFlowInfo_FlowInfo _tmp407;union Cyc_CfFlowInfo_AbsLVal _tmp408;struct _tuple15 _tmp406=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3CF,_tmp3FD,(void*)((struct Cyc_List_List*)_check_null(_tmp3FE))->hd,flds);_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp409=_tmp407;_LL296: if((_tmp409.BottomFL).tag != 1)goto _LL298;_LL297: {
struct _tuple15 _tmp827;struct _tuple15 _tmp40B=(_tmp827.f1=_tmp407,((_tmp827.f2=_tmp408,_tmp827)));_npop_handler(0);return _tmp40B;}_LL298:;_LL299: {
struct _tuple15 _tmp828;struct _tuple15 _tmp40D=(_tmp828.f1=_tmp3FF,((_tmp828.f2=_tmp408,_tmp828)));_npop_handler(0);return _tmp40D;}_LL295:;};};};}
# 1567
;_pop_region(temp);}_LL28E:;_LL28F: {
# 1586
struct Cyc_Core_Impossible_exn_struct _tmp82E;const char*_tmp82D;struct Cyc_Core_Impossible_exn_struct*_tmp82C;(int)_throw((void*)((_tmp82C=_cycalloc(sizeof(*_tmp82C)),((_tmp82C[0]=((_tmp82E.tag=Cyc_Core_Impossible,((_tmp82E.f1=((_tmp82D="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp82D,sizeof(char),33))),_tmp82E)))),_tmp82C)))));}_LL289:;}_LL276: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3D1=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3B6;if(_tmp3D1->tag != 20)goto _LL278;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;}}_LL277:
# 1590
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp3D2->topt))){
struct _tuple15 _tmp82F;return(_tmp82F.f1=inflow,((_tmp82F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp82F)));}{
struct Cyc_List_List*_tmp830;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3D2,(
(_tmp830=_region_malloc(env->r,sizeof(*_tmp830)),((_tmp830->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp3D2->topt),_tmp3D3),((_tmp830->tl=flds,_tmp830)))))));};_LL278:;_LL279: {
# 1595
struct _tuple15 _tmp831;return(_tmp831.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp831.f2=Cyc_CfFlowInfo_UnknownL(),_tmp831)));}_LL261:;};}
# 1599
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1601
union Cyc_CfFlowInfo_FlowInfo _tmp415;union Cyc_CfFlowInfo_AbsLVal _tmp416;struct _tuple15 _tmp414=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;{
struct _tuple15 _tmp832;return(_tmp832.f1=_tmp415,((_tmp832.f2=_tmp416,_tmp832)));};}
# 1607
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1609
struct Cyc_CfFlowInfo_FlowEnv*_tmp418=env->fenv;
{void*_tmp419=e->r;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp428;enum Cyc_Absyn_Primop _tmp42A;struct Cyc_List_List*_tmp42B;_LL29B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp41A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41A->tag != 5)goto _LL29D;else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;_tmp41D=_tmp41A->f3;}}_LL29C: {
# 1612
union Cyc_CfFlowInfo_FlowInfo _tmp42D;union Cyc_CfFlowInfo_FlowInfo _tmp42E;struct _tuple16 _tmp42C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp41B);_tmp42D=_tmp42C.f1;_tmp42E=_tmp42C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp430;union Cyc_CfFlowInfo_FlowInfo _tmp431;struct _tuple16 _tmp42F=Cyc_NewControlFlow_anal_test(env,_tmp42D,_tmp41C);_tmp430=_tmp42F.f1;_tmp431=_tmp42F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp433;union Cyc_CfFlowInfo_FlowInfo _tmp434;struct _tuple16 _tmp432=Cyc_NewControlFlow_anal_test(env,_tmp42E,_tmp41D);_tmp433=_tmp432.f1;_tmp434=_tmp432.f2;{
struct _tuple16 _tmp833;return(_tmp833.f1=Cyc_CfFlowInfo_join_flow(_tmp418,env->all_changed,_tmp430,_tmp433),((_tmp833.f2=
Cyc_CfFlowInfo_join_flow(_tmp418,env->all_changed,_tmp431,_tmp434),_tmp833)));};};};}_LL29D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp41E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41E->tag != 6)goto _LL29F;else{_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;}}_LL29E: {
# 1618
union Cyc_CfFlowInfo_FlowInfo _tmp437;union Cyc_CfFlowInfo_FlowInfo _tmp438;struct _tuple16 _tmp436=Cyc_NewControlFlow_anal_test(env,inflow,_tmp41F);_tmp437=_tmp436.f1;_tmp438=_tmp436.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp43A;union Cyc_CfFlowInfo_FlowInfo _tmp43B;struct _tuple16 _tmp439=Cyc_NewControlFlow_anal_test(env,_tmp437,_tmp420);_tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;{
# 1621
struct _tuple16 _tmp834;return(_tmp834.f1=_tmp43A,((_tmp834.f2=Cyc_CfFlowInfo_join_flow(_tmp418,env->all_changed,_tmp438,_tmp43B),_tmp834)));};};}_LL29F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp421=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp421->tag != 7)goto _LL2A1;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;}}_LL2A0: {
# 1623
union Cyc_CfFlowInfo_FlowInfo _tmp43E;union Cyc_CfFlowInfo_FlowInfo _tmp43F;struct _tuple16 _tmp43D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp422);_tmp43E=_tmp43D.f1;_tmp43F=_tmp43D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp441;union Cyc_CfFlowInfo_FlowInfo _tmp442;struct _tuple16 _tmp440=Cyc_NewControlFlow_anal_test(env,_tmp43F,_tmp423);_tmp441=_tmp440.f1;_tmp442=_tmp440.f2;{
# 1626
struct _tuple16 _tmp835;return(_tmp835.f1=Cyc_CfFlowInfo_join_flow(_tmp418,env->all_changed,_tmp43E,_tmp441),((_tmp835.f2=_tmp442,_tmp835)));};};}_LL2A1: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp424=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp424->tag != 8)goto _LL2A3;else{_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;}}_LL2A2: {
# 1628
union Cyc_CfFlowInfo_FlowInfo _tmp445;void*_tmp446;struct _tuple14 _tmp444=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp425);_tmp445=_tmp444.f1;_tmp446=_tmp444.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp445,_tmp426);}_LL2A3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp427->tag != 2)goto _LL2A5;else{if(_tmp427->f1 != Cyc_Absyn_Not)goto _LL2A5;if(_tmp427->f2 == 0)goto _LL2A5;_tmp428=(struct Cyc_Absyn_Exp*)(*_tmp427->f2).hd;if((*_tmp427->f2).tl != 0)goto _LL2A5;}}_LL2A4: {
# 1631
union Cyc_CfFlowInfo_FlowInfo _tmp448;union Cyc_CfFlowInfo_FlowInfo _tmp449;struct _tuple16 _tmp447=Cyc_NewControlFlow_anal_test(env,inflow,_tmp428);_tmp448=_tmp447.f1;_tmp449=_tmp447.f2;{
struct _tuple16 _tmp836;return(_tmp836.f1=_tmp449,((_tmp836.f2=_tmp448,_tmp836)));};}_LL2A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp429=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp429->tag != 2)goto _LL2A7;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}_LL2A6: {
# 1636
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp44B=_new_region("temp");struct _RegionHandle*temp=& _tmp44B;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp44D;struct Cyc_List_List*_tmp44E;struct _tuple20 _tmp44C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp42B,0,0);_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp44E))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp44E->tl))->hd;
f=_tmp44D;}{
# 1646
union Cyc_CfFlowInfo_FlowInfo _tmp44F=f;struct Cyc_Dict_Dict _tmp450;struct Cyc_List_List*_tmp451;_LL2AA: if((_tmp44F.BottomFL).tag != 1)goto _LL2AC;_LL2AB: {
struct _tuple16 _tmp837;struct _tuple16 _tmp453=(_tmp837.f1=f,((_tmp837.f2=f,_tmp837)));_npop_handler(0);return _tmp453;}_LL2AC: if((_tmp44F.ReachableFL).tag != 2)goto _LL2A9;_tmp450=((struct _tuple13)(_tmp44F.ReachableFL).val).f1;_tmp451=((struct _tuple13)(_tmp44F.ReachableFL).val).f2;_LL2AD: {
# 1649
struct Cyc_Absyn_Exp*_tmp454=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42B))->hd;
struct Cyc_Absyn_Exp*_tmp455=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42B->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp450,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp83A;void*_tmp839;(_tmp839=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp42B->hd)->loc,((_tmp83A="expression may not be initialized",_tag_dyneither(_tmp83A,sizeof(char),34))),_tag_dyneither(_tmp839,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp450,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp83D;void*_tmp83C;(_tmp83C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42B->tl))->hd)->loc,((_tmp83D="expression may not be initialized",_tag_dyneither(_tmp83D,sizeof(char),34))),_tag_dyneither(_tmp83C,sizeof(void*),0)));}
# 1658
if(_tmp42A == Cyc_Absyn_Eq  || _tmp42A == Cyc_Absyn_Neq){
struct _tuple0 _tmp83E;struct _tuple0 _tmp45B=(_tmp83E.f1=r1,((_tmp83E.f2=r2,_tmp83E)));enum Cyc_CfFlowInfo_InitLevel _tmp45D;enum Cyc_CfFlowInfo_InitLevel _tmp461;_LL2AF:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp45C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp45C->tag != 3)goto _LL2B1;else{_tmp45D=_tmp45C->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp45E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp45E->tag != 0)goto _LL2B1;};_LL2B0: {
# 1663
union Cyc_CfFlowInfo_FlowInfo _tmp471;union Cyc_CfFlowInfo_FlowInfo _tmp472;struct _tuple16 _tmp470=Cyc_NewControlFlow_splitzero(env,f,f,_tmp454,_tmp45D);_tmp471=_tmp470.f1;_tmp472=_tmp470.f2;
switch(_tmp42A){case Cyc_Absyn_Eq: _LL2C3: {
struct _tuple16 _tmp83F;struct _tuple16 _tmp474=(_tmp83F.f1=_tmp472,((_tmp83F.f2=_tmp471,_tmp83F)));_npop_handler(0);return _tmp474;}case Cyc_Absyn_Neq: _LL2C4: {
struct _tuple16 _tmp840;struct _tuple16 _tmp476=(_tmp840.f1=_tmp471,((_tmp840.f2=_tmp472,_tmp840)));_npop_handler(0);return _tmp476;}default: _LL2C5: {
struct Cyc_Core_Impossible_exn_struct _tmp846;const char*_tmp845;struct Cyc_Core_Impossible_exn_struct*_tmp844;(int)_throw((void*)((_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844[0]=((_tmp846.tag=Cyc_Core_Impossible,((_tmp846.f1=((_tmp845="anal_test, zero-split",_tag_dyneither(_tmp845,sizeof(char),22))),_tmp846)))),_tmp844)))));}}}_LL2B1:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp45F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp45F->tag != 0)goto _LL2B3;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp460=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp460->tag != 3)goto _LL2B3;else{_tmp461=_tmp460->f1;}};_LL2B2: {
# 1670
union Cyc_CfFlowInfo_FlowInfo _tmp47B;union Cyc_CfFlowInfo_FlowInfo _tmp47C;struct _tuple16 _tmp47A=Cyc_NewControlFlow_splitzero(env,f,f,_tmp455,_tmp461);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;
switch(_tmp42A){case Cyc_Absyn_Eq: _LL2C7: {
struct _tuple16 _tmp847;struct _tuple16 _tmp47E=(_tmp847.f1=_tmp47C,((_tmp847.f2=_tmp47B,_tmp847)));_npop_handler(0);return _tmp47E;}case Cyc_Absyn_Neq: _LL2C8: {
struct _tuple16 _tmp848;struct _tuple16 _tmp480=(_tmp848.f1=_tmp47B,((_tmp848.f2=_tmp47C,_tmp848)));_npop_handler(0);return _tmp480;}default: _LL2C9: {
struct Cyc_Core_Impossible_exn_struct _tmp84E;const char*_tmp84D;struct Cyc_Core_Impossible_exn_struct*_tmp84C;(int)_throw((void*)((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84E.tag=Cyc_Core_Impossible,((_tmp84E.f1=((_tmp84D="anal_test, zero-split",_tag_dyneither(_tmp84D,sizeof(char),22))),_tmp84E)))),_tmp84C)))));}}}_LL2B3:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp462=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp462->tag != 0)goto _LL2B5;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp463=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp463->tag != 0)goto _LL2B5;};_LL2B4:
# 1677
 if(_tmp42A == Cyc_Absyn_Eq){struct _tuple16 _tmp84F;struct _tuple16 _tmp485=(_tmp84F.f1=f,((_tmp84F.f2=Cyc_CfFlowInfo_BottomFL(),_tmp84F)));_npop_handler(0);return _tmp485;}else{
struct _tuple16 _tmp850;struct _tuple16 _tmp487=(_tmp850.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp850.f2=f,_tmp850)));_npop_handler(0);return _tmp487;}_LL2B5:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp464=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp464->tag != 0)goto _LL2B7;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp465=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp465->tag != 1)goto _LL2B7;};_LL2B6:
 goto _LL2B8;_LL2B7:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp466=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp466->tag != 0)goto _LL2B9;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp467=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp467->tag != 2)goto _LL2B9;};_LL2B8:
 goto _LL2BA;_LL2B9:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp468=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp468->tag != 0)goto _LL2BB;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp469=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp469->tag != 5)goto _LL2BB;};_LL2BA:
 goto _LL2BC;_LL2BB:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp46A=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp46A->tag != 1)goto _LL2BD;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp46B->tag != 0)goto _LL2BD;};_LL2BC:
 goto _LL2BE;_LL2BD:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp46C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp46C->tag != 2)goto _LL2BF;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp46D->tag != 0)goto _LL2BF;};_LL2BE:
 goto _LL2C0;_LL2BF:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp46E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp45B.f1;if(_tmp46E->tag != 5)goto _LL2C1;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp46F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp45B.f2;if(_tmp46F->tag != 0)goto _LL2C1;};_LL2C0:
# 1685
 if(_tmp42A == Cyc_Absyn_Neq){struct _tuple16 _tmp851;struct _tuple16 _tmp489=(_tmp851.f1=f,((_tmp851.f2=Cyc_CfFlowInfo_BottomFL(),_tmp851)));_npop_handler(0);return _tmp489;}else{
struct _tuple16 _tmp852;struct _tuple16 _tmp48B=(_tmp852.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp852.f2=f,_tmp852)));_npop_handler(0);return _tmp48B;}_LL2C1:;_LL2C2:
 goto _LL2AE;_LL2AE:;}
# 1694
{struct _tuple0 _tmp853;struct _tuple0 _tmp48D=(_tmp853.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp454->topt)),((_tmp853.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp455->topt)),_tmp853)));_LL2CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp48D.f1;if(_tmp48E->tag != 6)goto _LL2CE;else{if(_tmp48E->f1 != Cyc_Absyn_Unsigned)goto _LL2CE;}}_LL2CD:
 goto _LL2CF;_LL2CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp48F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp48D.f2;if(_tmp48F->tag != 6)goto _LL2D0;else{if(_tmp48F->f1 != Cyc_Absyn_Unsigned)goto _LL2D0;}}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp48D.f1;if(_tmp490->tag != 19)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp491=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp48D.f2;if(_tmp491->tag != 19)goto _LL2D4;}_LL2D3:
 goto _LL2CB;_LL2D4:;_LL2D5: {
struct _tuple16 _tmp854;struct _tuple16 _tmp493=(_tmp854.f1=f,((_tmp854.f2=f,_tmp854)));_npop_handler(0);return _tmp493;}_LL2CB:;}
# 1702
switch(_tmp42A){case Cyc_Absyn_Eq: _LL2D6:
 goto _LL2D7;case Cyc_Absyn_Neq: _LL2D7:
 goto _LL2D8;case Cyc_Absyn_Gt: _LL2D8:
 goto _LL2D9;case Cyc_Absyn_Gte: _LL2D9: {
struct _tuple16 _tmp855;struct _tuple16 _tmp495=(_tmp855.f1=f,((_tmp855.f2=f,_tmp855)));_npop_handler(0);return _tmp495;}case Cyc_Absyn_Lt: _LL2DA: {
# 1708
union Cyc_CfFlowInfo_FlowInfo _tmp496=f;
union Cyc_CfFlowInfo_FlowInfo _tmp497=f;
# 1711
{union Cyc_CfFlowInfo_FlowInfo _tmp498=_tmp496;struct Cyc_Dict_Dict _tmp499;_LL2DD: if((_tmp498.BottomFL).tag != 1)goto _LL2DF;_LL2DE: {
struct Cyc_Core_Impossible_exn_struct _tmp85B;const char*_tmp85A;struct Cyc_Core_Impossible_exn_struct*_tmp859;(int)_throw((void*)((_tmp859=_cycalloc(sizeof(*_tmp859)),((_tmp859[0]=((_tmp85B.tag=Cyc_Core_Impossible,((_tmp85B.f1=((_tmp85A="anal_test, Lt",_tag_dyneither(_tmp85A,sizeof(char),14))),_tmp85B)))),_tmp859)))));}_LL2DF: if((_tmp498.ReachableFL).tag != 2)goto _LL2DC;_tmp499=((struct _tuple13)(_tmp498.ReachableFL).val).f1;_LL2E0:
 _tmp450=_tmp499;_LL2DC:;}{
# 1717
void*_tmp49D=_tmp454->r;struct Cyc_Absyn_Vardecl*_tmp4A0;struct Cyc_Absyn_Vardecl*_tmp4A3;struct Cyc_Absyn_Vardecl*_tmp4A6;struct Cyc_Absyn_Vardecl*_tmp4A9;_LL2E2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp49E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp49E->tag != 1)goto _LL2E4;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp49F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp49E->f2);if(_tmp49F->tag != 1)goto _LL2E4;else{_tmp4A0=_tmp49F->f1;}}}if(!(!_tmp4A0->escapes))goto _LL2E4;_LL2E3:
# 1719
 _tmp4A3=_tmp4A0;goto _LL2E5;_LL2E4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4A1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A1->tag != 1)goto _LL2E6;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4A1->f2);if(_tmp4A2->tag != 4)goto _LL2E6;else{_tmp4A3=_tmp4A2->f1;}}}if(!(!_tmp4A3->escapes))goto _LL2E6;_LL2E5:
 _tmp4A6=_tmp4A3;goto _LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4A4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A4->tag != 1)goto _LL2E8;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4A5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4A4->f2);if(_tmp4A5->tag != 5)goto _LL2E8;else{_tmp4A6=_tmp4A5->f1;}}}if(!(!_tmp4A6->escapes))goto _LL2E8;_LL2E7:
 _tmp4A9=_tmp4A6;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4A7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A7->tag != 1)goto _LL2EA;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4A8=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4A7->f2);if(_tmp4A8->tag != 3)goto _LL2EA;else{_tmp4A9=_tmp4A8->f1;}}}if(!(!_tmp4A9->escapes))goto _LL2EA;_LL2E9: {
# 1723
void*_tmp4AA=_tmp455->r;struct Cyc_Absyn_Vardecl*_tmp4AD;struct Cyc_Absyn_Vardecl*_tmp4B0;struct Cyc_Absyn_Vardecl*_tmp4B3;struct Cyc_Absyn_Vardecl*_tmp4B6;int _tmp4B8;int _tmp4BB;enum Cyc_Absyn_Primop _tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;_LL2ED:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4AB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4AB->tag != 1)goto _LL2EF;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4AC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4AB->f2);if(_tmp4AC->tag != 1)goto _LL2EF;else{_tmp4AD=_tmp4AC->f1;}}}if(!(!_tmp4AD->escapes))goto _LL2EF;_LL2EE:
 _tmp4B0=_tmp4AD;goto _LL2F0;_LL2EF:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4AE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4AE->tag != 1)goto _LL2F1;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4AF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4AE->f2);if(_tmp4AF->tag != 4)goto _LL2F1;else{_tmp4B0=_tmp4AF->f1;}}}if(!(!_tmp4B0->escapes))goto _LL2F1;_LL2F0:
 _tmp4B3=_tmp4B0;goto _LL2F2;_LL2F1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4B1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B1->tag != 1)goto _LL2F3;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4B2=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4B1->f2);if(_tmp4B2->tag != 5)goto _LL2F3;else{_tmp4B3=_tmp4B2->f1;}}}if(!(!_tmp4B3->escapes))goto _LL2F3;_LL2F2:
 _tmp4B6=_tmp4B3;goto _LL2F4;_LL2F3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4B4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B4->tag != 1)goto _LL2F5;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4B5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4B4->f2);if(_tmp4B5->tag != 3)goto _LL2F5;else{_tmp4B6=_tmp4B5->f1;}}}if(!(!_tmp4B6->escapes))goto _LL2F5;_LL2F4: {
# 1730
struct _RegionHandle*_tmp4BF=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp85E;struct Cyc_List_List*_tmp85D;struct Cyc_List_List*_tmp4C0=(_tmp85D=_region_malloc(_tmp4BF,sizeof(*_tmp85D)),((_tmp85D->hd=((_tmp85E=_region_malloc(_tmp4BF,sizeof(*_tmp85E)),((_tmp85E->vd=_tmp4A9,((_tmp85E->rop=Cyc_CfFlowInfo_LessVar(_tmp4B6,_tmp4B6->type),_tmp85E)))))),((_tmp85D->tl=_tmp451,_tmp85D)))));
struct _tuple16 _tmp85F;struct _tuple16 _tmp4C2=(_tmp85F.f1=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp4C0),((_tmp85F.f2=_tmp497,_tmp85F)));_npop_handler(0);return _tmp4C2;}_LL2F5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B7->tag != 0)goto _LL2F7;else{if(((_tmp4B7->f1).Int_c).tag != 5)goto _LL2F7;_tmp4B8=((struct _tuple6)((_tmp4B7->f1).Int_c).val).f2;}}_LL2F6:
 _tmp4BB=_tmp4B8;goto _LL2F8;_LL2F7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4B9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B9->tag != 13)goto _LL2F9;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4BA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(*_tmp4B9->f2).r;if(_tmp4BA->tag != 0)goto _LL2F9;else{if(((_tmp4BA->f1).Int_c).tag != 5)goto _LL2F9;_tmp4BB=((struct _tuple6)((_tmp4BA->f1).Int_c).val).f2;}}}_LL2F8: {
# 1735
struct _RegionHandle*_tmp4C5=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp862;struct Cyc_List_List*_tmp861;struct Cyc_List_List*_tmp4C6=(_tmp861=_region_malloc(_tmp4C5,sizeof(*_tmp861)),((_tmp861->hd=((_tmp862=_region_malloc(_tmp4C5,sizeof(*_tmp862)),((_tmp862->vd=_tmp4A9,((_tmp862->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp4BB),_tmp862)))))),((_tmp861->tl=_tmp451,_tmp861)))));
struct _tuple16 _tmp863;struct _tuple16 _tmp4C8=(_tmp863.f1=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp4C6),((_tmp863.f2=_tmp497,_tmp863)));_npop_handler(0);return _tmp4C8;}_LL2F9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4BC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4BC->tag != 2)goto _LL2FB;else{_tmp4BD=_tmp4BC->f1;if(_tmp4BC->f2 == 0)goto _LL2FB;_tmp4BE=(struct Cyc_Absyn_Exp*)(*_tmp4BC->f2).hd;}}_LL2FA: {
# 1739
void*_tmp4CB=_tmp4BE->r;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Vardecl*_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D4;struct Cyc_Absyn_Vardecl*_tmp4D7;_LL2FE:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CB;if(_tmp4CC->tag != 1)goto _LL300;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4CD=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4CC->f2);if(_tmp4CD->tag != 1)goto _LL300;else{_tmp4CE=_tmp4CD->f1;}}}if(!(!_tmp4CE->escapes))goto _LL300;_LL2FF:
 _tmp4D1=_tmp4CE;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CB;if(_tmp4CF->tag != 1)goto _LL302;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4CF->f2);if(_tmp4D0->tag != 4)goto _LL302;else{_tmp4D1=_tmp4D0->f1;}}}if(!(!_tmp4D1->escapes))goto _LL302;_LL301:
 _tmp4D4=_tmp4D1;goto _LL303;_LL302:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CB;if(_tmp4D2->tag != 1)goto _LL304;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4D3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4D2->f2);if(_tmp4D3->tag != 5)goto _LL304;else{_tmp4D4=_tmp4D3->f1;}}}if(!(!_tmp4D4->escapes))goto _LL304;_LL303:
 _tmp4D7=_tmp4D4;goto _LL305;_LL304:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4CB;if(_tmp4D5->tag != 1)goto _LL306;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4D6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4D5->f2);if(_tmp4D6->tag != 3)goto _LL306;else{_tmp4D7=_tmp4D6->f1;}}}if(!(!_tmp4D7->escapes))goto _LL306;_LL305: {
# 1745
struct _RegionHandle*_tmp4D8=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp866;struct Cyc_List_List*_tmp865;struct Cyc_List_List*_tmp4D9=(_tmp865=_region_malloc(_tmp4D8,sizeof(*_tmp865)),((_tmp865->hd=((_tmp866=_region_malloc(_tmp4D8,sizeof(*_tmp866)),((_tmp866->vd=_tmp4A9,((_tmp866->rop=Cyc_CfFlowInfo_LessNumelts(_tmp4D7),_tmp866)))))),((_tmp865->tl=_tmp451,_tmp865)))));
struct _tuple16 _tmp867;struct _tuple16 _tmp4DB=(_tmp867.f1=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp4D9),((_tmp867.f2=_tmp497,_tmp867)));_npop_handler(0);return _tmp4DB;}_LL306:;_LL307: {
struct _tuple16 _tmp868;struct _tuple16 _tmp4DF=(_tmp868.f1=_tmp496,((_tmp868.f2=_tmp497,_tmp868)));_npop_handler(0);return _tmp4DF;}_LL2FD:;}_LL2FB:;_LL2FC: {
# 1750
struct _tuple16 _tmp869;struct _tuple16 _tmp4E1=(_tmp869.f1=_tmp496,((_tmp869.f2=_tmp497,_tmp869)));_npop_handler(0);return _tmp4E1;}_LL2EC:;}_LL2EA:;_LL2EB: {
# 1752
struct _tuple16 _tmp86A;struct _tuple16 _tmp4E3=(_tmp86A.f1=_tmp496,((_tmp86A.f2=_tmp497,_tmp86A)));_npop_handler(0);return _tmp4E3;}_LL2E1:;};}case Cyc_Absyn_Lte: _LL2DB: {
# 1756
union Cyc_CfFlowInfo_FlowInfo _tmp4E4=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4E5=f;
# 1759
{union Cyc_CfFlowInfo_FlowInfo _tmp4E6=_tmp4E4;struct Cyc_Dict_Dict _tmp4E7;_LL30A: if((_tmp4E6.BottomFL).tag != 1)goto _LL30C;_LL30B: {
struct Cyc_Core_Impossible_exn_struct _tmp870;const char*_tmp86F;struct Cyc_Core_Impossible_exn_struct*_tmp86E;(int)_throw((void*)((_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp870.tag=Cyc_Core_Impossible,((_tmp870.f1=((_tmp86F="anal_test, Lte",_tag_dyneither(_tmp86F,sizeof(char),15))),_tmp870)))),_tmp86E)))));}_LL30C: if((_tmp4E6.ReachableFL).tag != 2)goto _LL309;_tmp4E7=((struct _tuple13)(_tmp4E6.ReachableFL).val).f1;_LL30D:
 _tmp450=_tmp4E7;_LL309:;}{
# 1765
void*_tmp4EB=_tmp454->r;struct Cyc_Absyn_Vardecl*_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4F1;struct Cyc_Absyn_Vardecl*_tmp4F4;struct Cyc_Absyn_Vardecl*_tmp4F7;_LL30F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4EC->tag != 1)goto _LL311;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4ED=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp4EC->f2);if(_tmp4ED->tag != 1)goto _LL311;else{_tmp4EE=_tmp4ED->f1;}}}if(!(!_tmp4EE->escapes))goto _LL311;_LL310:
# 1767
 _tmp4F1=_tmp4EE;goto _LL312;_LL311:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4EF->tag != 1)goto _LL313;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp4EF->f2);if(_tmp4F0->tag != 4)goto _LL313;else{_tmp4F1=_tmp4F0->f1;}}}if(!(!_tmp4F1->escapes))goto _LL313;_LL312:
 _tmp4F4=_tmp4F1;goto _LL314;_LL313:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4F2->tag != 1)goto _LL315;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp4F3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp4F2->f2);if(_tmp4F3->tag != 5)goto _LL315;else{_tmp4F4=_tmp4F3->f1;}}}if(!(!_tmp4F4->escapes))goto _LL315;_LL314:
 _tmp4F7=_tmp4F4;goto _LL316;_LL315:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4EB;if(_tmp4F5->tag != 1)goto _LL317;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4F6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp4F5->f2);if(_tmp4F6->tag != 3)goto _LL317;else{_tmp4F7=_tmp4F6->f1;}}}if(!(!_tmp4F7->escapes))goto _LL317;_LL316: {
# 1771
void*_tmp4F8=_tmp455->r;int _tmp4FA;int _tmp4FD;enum Cyc_Absyn_Primop _tmp4FF;struct Cyc_Absyn_Exp*_tmp500;_LL31A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4F9->tag != 0)goto _LL31C;else{if(((_tmp4F9->f1).Int_c).tag != 5)goto _LL31C;_tmp4FA=((struct _tuple6)((_tmp4F9->f1).Int_c).val).f2;}}_LL31B:
 _tmp4FD=_tmp4FA;goto _LL31D;_LL31C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4FB->tag != 13)goto _LL31E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(*_tmp4FB->f2).r;if(_tmp4FC->tag != 0)goto _LL31E;else{if(((_tmp4FC->f1).Int_c).tag != 5)goto _LL31E;_tmp4FD=((struct _tuple6)((_tmp4FC->f1).Int_c).val).f2;}}}_LL31D: {
# 1774
struct _RegionHandle*_tmp501=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp873;struct Cyc_List_List*_tmp872;struct Cyc_List_List*_tmp502=(_tmp872=_region_malloc(_tmp501,sizeof(*_tmp872)),((_tmp872->hd=(
(_tmp873=_region_malloc(_tmp501,sizeof(*_tmp873)),((_tmp873->vd=_tmp4F7,((_tmp873->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)_tmp4FD),_tmp873)))))),((_tmp872->tl=_tmp451,_tmp872)))));
struct _tuple16 _tmp874;struct _tuple16 _tmp504=(_tmp874.f1=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp502),((_tmp874.f2=_tmp4E5,_tmp874)));_npop_handler(0);return _tmp504;}_LL31E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4FE->tag != 2)goto _LL320;else{_tmp4FF=_tmp4FE->f1;if(_tmp4FE->f2 == 0)goto _LL320;_tmp500=(struct Cyc_Absyn_Exp*)(*_tmp4FE->f2).hd;}}_LL31F: {
# 1779
void*_tmp507=_tmp500->r;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp510;struct Cyc_Absyn_Vardecl*_tmp513;_LL323:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp507;if(_tmp508->tag != 1)goto _LL325;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp509=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp508->f2);if(_tmp509->tag != 1)goto _LL325;else{_tmp50A=_tmp509->f1;}}}if(!(!_tmp50A->escapes))goto _LL325;_LL324:
 _tmp50D=_tmp50A;goto _LL326;_LL325:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp507;if(_tmp50B->tag != 1)goto _LL327;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp50C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp50B->f2);if(_tmp50C->tag != 4)goto _LL327;else{_tmp50D=_tmp50C->f1;}}}if(!(!_tmp50D->escapes))goto _LL327;_LL326:
 _tmp510=_tmp50D;goto _LL328;_LL327:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp507;if(_tmp50E->tag != 1)goto _LL329;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp50F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp50E->f2);if(_tmp50F->tag != 5)goto _LL329;else{_tmp510=_tmp50F->f1;}}}if(!(!_tmp510->escapes))goto _LL329;_LL328:
 _tmp513=_tmp510;goto _LL32A;_LL329:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp507;if(_tmp511->tag != 1)goto _LL32B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp512=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp511->f2);if(_tmp512->tag != 3)goto _LL32B;else{_tmp513=_tmp512->f1;}}}if(!(!_tmp513->escapes))goto _LL32B;_LL32A: {
# 1785
struct Cyc_CfFlowInfo_FlowEnv*_tmp514=env->fenv;
struct Cyc_CfFlowInfo_Reln*_tmp877;struct Cyc_List_List*_tmp876;struct Cyc_List_List*_tmp515=(_tmp876=_region_malloc(_tmp514->r,sizeof(*_tmp876)),((_tmp876->hd=((_tmp877=_region_malloc(_tmp514->r,sizeof(*_tmp877)),((_tmp877->vd=_tmp4F7,((_tmp877->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp513),_tmp877)))))),((_tmp876->tl=_tmp451,_tmp876)))));
struct _tuple16 _tmp878;struct _tuple16 _tmp517=(_tmp878.f1=Cyc_CfFlowInfo_ReachableFL(_tmp450,_tmp515),((_tmp878.f2=_tmp4E5,_tmp878)));_npop_handler(0);return _tmp517;}_LL32B:;_LL32C: {
struct _tuple16 _tmp879;struct _tuple16 _tmp51B=(_tmp879.f1=_tmp4E4,((_tmp879.f2=_tmp4E5,_tmp879)));_npop_handler(0);return _tmp51B;}_LL322:;}_LL320:;_LL321: {
# 1790
struct _tuple16 _tmp87A;struct _tuple16 _tmp51D=(_tmp87A.f1=_tmp4E4,((_tmp87A.f2=_tmp4E5,_tmp87A)));_npop_handler(0);return _tmp51D;}_LL319:;}_LL317:;_LL318: {
# 1792
struct _tuple16 _tmp87B;struct _tuple16 _tmp51F=(_tmp87B.f1=_tmp4E4,((_tmp87B.f2=_tmp4E5,_tmp87B)));_npop_handler(0);return _tmp51F;}_LL30E:;};}default: _LL308: {
# 1794
struct _tuple16 _tmp87C;struct _tuple16 _tmp521=(_tmp87C.f1=f,((_tmp87C.f2=f,_tmp87C)));_npop_handler(0);return _tmp521;}}}_LL2A9:;};
# 1638
;_pop_region(temp);}_LL2A7:;_LL2A8:
# 1797
 goto _LL29A;_LL29A:;}{
# 1801
union Cyc_CfFlowInfo_FlowInfo _tmp523;void*_tmp524;struct _tuple14 _tmp522=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp523=_tmp522.f1;_tmp524=_tmp522.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp525=_tmp523;struct Cyc_Dict_Dict _tmp526;_LL32F: if((_tmp525.BottomFL).tag != 1)goto _LL331;_LL330: {
struct _tuple16 _tmp87D;return(_tmp87D.f1=_tmp523,((_tmp87D.f2=_tmp523,_tmp87D)));}_LL331: if((_tmp525.ReachableFL).tag != 2)goto _LL32E;_tmp526=((struct _tuple13)(_tmp525.ReachableFL).val).f1;_LL332: {
# 1805
void*_tmp528=_tmp524;enum Cyc_CfFlowInfo_InitLevel _tmp531;_LL334: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp529=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp529->tag != 0)goto _LL336;}_LL335: {
struct _tuple16 _tmp87E;return(_tmp87E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87E.f2=_tmp523,_tmp87E)));}_LL336: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp52A=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52A->tag != 2)goto _LL338;}_LL337:
 goto _LL339;_LL338: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp52B=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52B->tag != 1)goto _LL33A;}_LL339:
 goto _LL33B;_LL33A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp52C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52C->tag != 5)goto _LL33C;}_LL33B: {
struct _tuple16 _tmp87F;return(_tmp87F.f1=_tmp523,((_tmp87F.f2=Cyc_CfFlowInfo_BottomFL(),_tmp87F)));}_LL33C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp52D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52D->tag != 3)goto _LL33E;else{if(_tmp52D->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL33E;}}_LL33D:
 goto _LL33F;_LL33E: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp52E=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52E->tag != 4)goto _LL340;else{if(_tmp52E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL340;}}_LL33F:
 goto _LL341;_LL340: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp52F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp52F->tag != 7)goto _LL342;}_LL341:
# 1813
{const char*_tmp882;void*_tmp881;(_tmp881=0,Cyc_Tcutil_terr(e->loc,((_tmp882="expression may not be initialized",_tag_dyneither(_tmp882,sizeof(char),34))),_tag_dyneither(_tmp881,sizeof(void*),0)));}{
struct _tuple16 _tmp883;return(_tmp883.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp883.f2=Cyc_CfFlowInfo_BottomFL(),_tmp883)));};_LL342: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp530=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp530->tag != 3)goto _LL344;else{_tmp531=_tmp530->f1;}}_LL343:
# 1816
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp523,e,_tmp531);_LL344: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp532=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp532->tag != 4)goto _LL346;}_LL345: {
struct _tuple16 _tmp884;return(_tmp884.f1=_tmp523,((_tmp884.f2=_tmp523,_tmp884)));}_LL346: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp533=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp533->tag != 6)goto _LL333;}_LL347: {
struct Cyc_Core_Impossible_exn_struct _tmp88A;const char*_tmp889;struct Cyc_Core_Impossible_exn_struct*_tmp888;(int)_throw((void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp88A.tag=Cyc_Core_Impossible,((_tmp88A.f1=((_tmp889="anal_test",_tag_dyneither(_tmp889,sizeof(char),10))),_tmp88A)))),_tmp888)))));}_LL333:;}_LL32E:;};};}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1823
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1825
unsigned int _tmp53E;struct Cyc_NewControlFlow_AnalEnv*_tmp53F;struct Cyc_Dict_Dict _tmp540;struct _tuple23*_tmp53D=ckenv;_tmp53E=(*_tmp53D).f1;_tmp53F=(*_tmp53D).f2;_tmp540=(*_tmp53D).f3;{
void*_tmp541=root;struct Cyc_Absyn_Vardecl*_tmp543;_LL349: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp542=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp541;if(_tmp542->tag != 0)goto _LL34B;else{_tmp543=_tmp542->f1;}}_LL34A:
# 1828
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp53F->fenv)->r,_tmp543->type)){
void*_tmp544=rval;_LL34E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp545=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp544;if(_tmp545->tag != 7)goto _LL350;}_LL34F:
 goto _LL351;_LL350: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp546=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp544;if(_tmp546->tag != 0)goto _LL352;}_LL351:
 goto _LL353;_LL352: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp547=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp544;if(_tmp547->tag != 3)goto _LL354;else{if(_tmp547->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL354;}}_LL353:
 goto _LL34D;_LL354:;_LL355:
# 1834
{const char*_tmp88E;void*_tmp88D[1];struct Cyc_String_pa_PrintArg_struct _tmp88C;(_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp543->name)),((_tmp88D[0]=& _tmp88C,Cyc_Tcutil_warn(_tmp53E,((_tmp88E="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp88E,sizeof(char),57))),_tag_dyneither(_tmp88D,sizeof(void*),1)))))));}
goto _LL34D;_LL34D:;}
# 1837
goto _LL348;_LL34B:;_LL34C:
 goto _LL348;_LL348:;};}
# 1842
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp54B=flow;struct Cyc_Dict_Dict _tmp54C;_LL357: if((_tmp54B.BottomFL).tag != 1)goto _LL359;_LL358:
 return;_LL359: if((_tmp54B.ReachableFL).tag != 2)goto _LL356;_tmp54C=((struct _tuple13)(_tmp54B.ReachableFL).val).f1;_LL35A:
# 1846
{struct Cyc_List_List*_tmp54D=env->param_roots;for(0;_tmp54D != 0;_tmp54D=_tmp54D->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp54C,Cyc_CfFlowInfo_lookup_place(_tmp54C,(struct Cyc_CfFlowInfo_Place*)_tmp54D->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_Tcutil_terr(loc,((_tmp891="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp891,sizeof(char),76))),_tag_dyneither(_tmp890,sizeof(void*),0)));}}}
# 1850
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp892;struct _tuple23 _tmp550=(_tmp892.f1=loc,((_tmp892.f2=env,((_tmp892.f3=_tmp54C,_tmp892)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp550,_tmp54C);}
# 1854
return;_LL356:;}
# 1861
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1863
struct Cyc_CfFlowInfo_FlowEnv*_tmp552=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Stmt*_tmp556;unsigned int _tmp557;struct Cyc_Absyn_Switch_clause*_tmp553=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp554=(*_tmp553).pat_vars;_tmp555=(*_tmp553).where_clause;_tmp556=(*_tmp553).body;_tmp557=(*_tmp553).loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp552,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp554))->v,_tmp552->unknown_all,_tmp557);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp555 != 0){
struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp555;
union Cyc_CfFlowInfo_FlowInfo _tmp559;union Cyc_CfFlowInfo_FlowInfo _tmp55A;struct _tuple16 _tmp558=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp559=_tmp558.f1;_tmp55A=_tmp558.f2;
inflow=_tmp55A;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp559,_tmp556);}else{
# 1874
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp556);}{
# 1876
union Cyc_CfFlowInfo_FlowInfo _tmp55B=clause_outflow;_LL35C: if((_tmp55B.BottomFL).tag != 1)goto _LL35E;_LL35D:
 goto _LL35B;_LL35E:;_LL35F:
# 1880
 if(scs->tl == 0)
return clause_outflow;else{
# 1885
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp895;void*_tmp894;(_tmp894=0,Cyc_Tcutil_terr(_tmp556->loc,((_tmp895="switch clause may implicitly fallthru",_tag_dyneither(_tmp895,sizeof(char),38))),_tag_dyneither(_tmp894,sizeof(void*),0)));}else{
# 1888
const char*_tmp898;void*_tmp897;(_tmp897=0,Cyc_Tcutil_warn(_tmp556->loc,((_tmp898="switch clause may implicitly fallthru",_tag_dyneither(_tmp898,sizeof(char),38))),_tag_dyneither(_tmp897,sizeof(void*),0)));}
# 1890
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 1892
goto _LL35B;_LL35B:;};};}
# 1895
return Cyc_CfFlowInfo_BottomFL();}struct _tuple24{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 1900
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple24*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 1902
struct Cyc_NewControlFlow_AnalEnv*_tmp561;struct Cyc_Dict_Dict _tmp562;unsigned int _tmp563;struct _tuple24*_tmp560=vdenv;_tmp561=(*_tmp560).f1;_tmp562=(*_tmp560).f2;_tmp563=(*_tmp560).f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp561->fenv)->r,vd->type)){
# 1905
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp899;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp899.tag=0,((_tmp899.f1=vd,_tmp899)));
# 1907
void*_tmp564=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp562,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp565=_tmp564;_LL361: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp566=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp565;if(_tmp566->tag != 7)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp567=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp565;if(_tmp567->tag != 0)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp568=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp565;if(_tmp568->tag != 3)goto _LL367;else{if(_tmp568->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL367;}}_LL366:
 goto _LL360;_LL367:;_LL368:
# 1913
{const char*_tmp89D;void*_tmp89C[1];struct Cyc_String_pa_PrintArg_struct _tmp89B;(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp89C[0]=& _tmp89B,Cyc_Tcutil_warn(_tmp563,((_tmp89D="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp89D,sizeof(char),68))),_tag_dyneither(_tmp89C,sizeof(void*),1)))))));}
# 1915
goto _LL360;_LL360:;}}
# 1920
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 1922
{union Cyc_CfFlowInfo_FlowInfo _tmp56D=inflow;struct Cyc_Dict_Dict _tmp56E;_LL36A: if((_tmp56D.ReachableFL).tag != 2)goto _LL36C;_tmp56E=((struct _tuple13)(_tmp56D.ReachableFL).val).f1;_LL36B: {
# 1924
struct _tuple24 _tmp89E;struct _tuple24 _tmp56F=(_tmp89E.f1=env,((_tmp89E.f2=_tmp56E,((_tmp89E.f3=decl->loc,_tmp89E)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp570=env->fenv;
{void*_tmp571=decl->r;struct Cyc_Absyn_Vardecl*_tmp573;struct Cyc_List_List*_tmp575;struct Cyc_List_List*_tmp577;_LL36F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp572=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp571;if(_tmp572->tag != 0)goto _LL371;else{_tmp573=_tmp572->f1;}}_LL370:
# 1928
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp56F,_tmp573);
goto _LL36E;_LL371: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp574=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp571;if(_tmp574->tag != 2)goto _LL373;else{if(_tmp574->f2 == 0)goto _LL373;_tmp575=(struct Cyc_List_List*)(*_tmp574->f2).v;}}_LL372:
 _tmp577=_tmp575;goto _LL374;_LL373: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp576=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp571;if(_tmp576->tag != 3)goto _LL375;else{_tmp577=_tmp576->f1;}}_LL374:
# 1932
((void(*)(void(*f)(struct _tuple24*,struct Cyc_Absyn_Vardecl*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp56F,_tmp577);
goto _LL36E;_LL375:;_LL376:
 goto _LL36E;_LL36E:;}
# 1936
goto _LL369;}_LL36C:;_LL36D:
 goto _LL369;_LL369:;}
# 1939
return;}
# 1942
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1947
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp57A;union Cyc_CfFlowInfo_FlowInfo*_tmp57B;struct _tuple17 _tmp579=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp57A=_tmp579.f1;_tmp57B=_tmp579.f2;
inflow=*_tmp57B;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp57C=env->fenv;
void*_tmp57D=s->r;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Exp*_tmp583;struct Cyc_Absyn_Stmt*_tmp585;struct Cyc_Absyn_Stmt*_tmp586;struct Cyc_Absyn_Exp*_tmp588;struct Cyc_Absyn_Stmt*_tmp589;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Stmt*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Stmt*_tmp590;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Stmt*_tmp592;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Stmt*_tmp596;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Stmt*_tmp598;struct Cyc_Absyn_Stmt*_tmp599;struct Cyc_List_List*_tmp59C;struct Cyc_Absyn_Switch_clause*_tmp59D;struct Cyc_Absyn_Stmt*_tmp59F;struct Cyc_Absyn_Stmt*_tmp5A1;struct Cyc_Absyn_Stmt*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_Absyn_Stmt*_tmp5A8;struct Cyc_List_List*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Tvar*_tmp5AD;struct Cyc_Absyn_Vardecl*_tmp5AE;struct Cyc_Absyn_Stmt*_tmp5AF;struct Cyc_Absyn_Decl*_tmp5B1;struct Cyc_Absyn_Stmt*_tmp5B2;struct Cyc_Absyn_Stmt*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B6;_LL378: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp57E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp57E->tag != 0)goto _LL37A;}_LL379:
 return inflow;_LL37A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp57F=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp57F->tag != 3)goto _LL37C;else{if(_tmp57F->f1 != 0)goto _LL37C;}}_LL37B:
# 1953
 if(env->noreturn){
const char*_tmp8A1;void*_tmp8A0;(_tmp8A0=0,Cyc_Tcutil_terr(s->loc,((_tmp8A1="`noreturn' function might return",_tag_dyneither(_tmp8A1,sizeof(char),33))),_tag_dyneither(_tmp8A0,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
# 1957
return Cyc_CfFlowInfo_BottomFL();_LL37C: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp580=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp580->tag != 3)goto _LL37E;else{_tmp581=_tmp580->f1;}}_LL37D:
# 1959
 if(env->noreturn){
const char*_tmp8A4;void*_tmp8A3;(_tmp8A3=0,Cyc_Tcutil_terr(s->loc,((_tmp8A4="`noreturn' function might return",_tag_dyneither(_tmp8A4,sizeof(char),33))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp5BC;void*_tmp5BD;struct _tuple14 _tmp5BB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp581));_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;
_tmp5BC=Cyc_NewControlFlow_use_Rval(env,_tmp581->loc,_tmp5BC,_tmp5BD);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5BC);
return Cyc_CfFlowInfo_BottomFL();};_LL37E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp582=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp582->tag != 1)goto _LL380;else{_tmp583=_tmp582->f1;}}_LL37F:
# 1966
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp583)).f1;_LL380: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp584=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp584->tag != 2)goto _LL382;else{_tmp585=_tmp584->f1;_tmp586=_tmp584->f2;}}_LL381:
# 1969
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp585),_tmp586);_LL382: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp587=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp587->tag != 4)goto _LL384;else{_tmp588=_tmp587->f1;_tmp589=_tmp587->f2;_tmp58A=_tmp587->f3;}}_LL383: {
# 1972
union Cyc_CfFlowInfo_FlowInfo _tmp5BF;union Cyc_CfFlowInfo_FlowInfo _tmp5C0;struct _tuple16 _tmp5BE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp588);_tmp5BF=_tmp5BE.f1;_tmp5C0=_tmp5BE.f2;{
# 1979
union Cyc_CfFlowInfo_FlowInfo _tmp5C1=Cyc_NewControlFlow_anal_stmt(env,_tmp5C0,_tmp58A);
union Cyc_CfFlowInfo_FlowInfo _tmp5C2=Cyc_NewControlFlow_anal_stmt(env,_tmp5BF,_tmp589);
return Cyc_CfFlowInfo_join_flow(_tmp57C,env->all_changed,_tmp5C2,_tmp5C1);};}_LL384: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp58B=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp58B->tag != 5)goto _LL386;else{_tmp58C=(_tmp58B->f1).f1;_tmp58D=(_tmp58B->f1).f2;_tmp58E=_tmp58B->f2;}}_LL385: {
# 1987
union Cyc_CfFlowInfo_FlowInfo*_tmp5C4;struct _tuple17 _tmp5C3=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp58D);_tmp5C4=_tmp5C3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C5=*_tmp5C4;
union Cyc_CfFlowInfo_FlowInfo _tmp5C7;union Cyc_CfFlowInfo_FlowInfo _tmp5C8;struct _tuple16 _tmp5C6=Cyc_NewControlFlow_anal_test(env,_tmp5C5,_tmp58C);_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=Cyc_NewControlFlow_anal_stmt(env,_tmp5C7,_tmp58E);
Cyc_NewControlFlow_update_flow(env,_tmp58D,_tmp5C9);
return _tmp5C8;};};}_LL386: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp58F=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp58F->tag != 14)goto _LL388;else{_tmp590=_tmp58F->f1;_tmp591=(_tmp58F->f2).f1;_tmp592=(_tmp58F->f2).f2;}}_LL387: {
# 1997
union Cyc_CfFlowInfo_FlowInfo _tmp5CA=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp590);
union Cyc_CfFlowInfo_FlowInfo*_tmp5CC;struct _tuple17 _tmp5CB=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CA,_tmp592);_tmp5CC=_tmp5CB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CD=*_tmp5CC;
union Cyc_CfFlowInfo_FlowInfo _tmp5CF;union Cyc_CfFlowInfo_FlowInfo _tmp5D0;struct _tuple16 _tmp5CE=Cyc_NewControlFlow_anal_test(env,_tmp5CD,_tmp591);_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;
Cyc_NewControlFlow_update_flow(env,_tmp590,_tmp5CF);
return _tmp5D0;};}_LL388: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp593=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp593->tag != 9)goto _LL38A;else{_tmp594=_tmp593->f1;_tmp595=(_tmp593->f2).f1;_tmp596=(_tmp593->f2).f2;_tmp597=(_tmp593->f3).f1;_tmp598=(_tmp593->f3).f2;_tmp599=_tmp593->f4;}}_LL389: {
# 2006
union Cyc_CfFlowInfo_FlowInfo _tmp5D1=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp594)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp5D3;struct _tuple17 _tmp5D2=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5D1,_tmp596);_tmp5D3=_tmp5D2.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D4=*_tmp5D3;
union Cyc_CfFlowInfo_FlowInfo _tmp5D6;union Cyc_CfFlowInfo_FlowInfo _tmp5D7;struct _tuple16 _tmp5D5=Cyc_NewControlFlow_anal_test(env,_tmp5D4,_tmp595);_tmp5D6=_tmp5D5.f1;_tmp5D7=_tmp5D5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D8=Cyc_NewControlFlow_anal_stmt(env,_tmp5D6,_tmp599);
union Cyc_CfFlowInfo_FlowInfo*_tmp5DA;struct _tuple17 _tmp5D9=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5D8,_tmp598);_tmp5DA=_tmp5D9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DB=*_tmp5DA;
union Cyc_CfFlowInfo_FlowInfo _tmp5DC=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5DB,_tmp597)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp596,_tmp5DC);
return _tmp5D7;};};};}_LL38A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp59A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp59A->tag != 6)goto _LL38C;else{if(_tmp59A->f1 != 0)goto _LL38C;}}_LL38B:
# 2018
 return Cyc_CfFlowInfo_BottomFL();_LL38C: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp59B=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp59B->tag != 11)goto _LL38E;else{_tmp59C=_tmp59B->f1;if(_tmp59B->f2 == 0)goto _LL38E;_tmp59D=*_tmp59B->f2;}}_LL38D: {
# 2020
struct _RegionHandle _tmp5DD=_new_region("temp");struct _RegionHandle*temp=& _tmp5DD;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp5DF;struct Cyc_List_List*_tmp5E0;struct _tuple20 _tmp5DE=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp59C,1,1);_tmp5DF=_tmp5DE.f1;_tmp5E0=_tmp5DE.f2;
# 2023
for(0;_tmp5E0 != 0;(_tmp5E0=_tmp5E0->tl,_tmp59C=_tmp59C->tl)){
_tmp5DF=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp59C))->hd)->loc,_tmp5DF,(void*)_tmp5E0->hd);}
# 2026
_tmp5DF=Cyc_NewControlFlow_add_vars(_tmp57C,_tmp5DF,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp59D->pat_vars))->v,_tmp57C->unknown_all,s->loc);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp59D->body,_tmp5DF);{
union Cyc_CfFlowInfo_FlowInfo _tmp5E1=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5E1;};}
# 2021
;_pop_region(temp);}_LL38E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp59E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp59E->tag != 6)goto _LL390;else{_tmp59F=_tmp59E->f1;}}_LL38F:
# 2030
 _tmp5A1=_tmp59F;goto _LL391;_LL390: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp5A0=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5A0->tag != 7)goto _LL392;else{_tmp5A1=_tmp5A0->f1;}}_LL391:
 _tmp5A3=_tmp5A1;goto _LL393;_LL392: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp5A2=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5A2->tag != 8)goto _LL394;else{_tmp5A3=_tmp5A2->f2;}}_LL393:
# 2035
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5E2=_tmp57A->encloser;
struct Cyc_Absyn_Stmt*_tmp5E3=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp5A3)))->encloser;
while(_tmp5E3 != _tmp5E2){
struct Cyc_Absyn_Stmt*_tmp5E4=(Cyc_NewControlFlow_get_stmt_annot(_tmp5E2))->encloser;
if(_tmp5E4 == _tmp5E2){
{const char*_tmp8A7;void*_tmp8A6;(_tmp8A6=0,Cyc_Tcutil_terr(s->loc,((_tmp8A7="goto enters local scope or exception handler",_tag_dyneither(_tmp8A7,sizeof(char),45))),_tag_dyneither(_tmp8A6,sizeof(void*),0)));}
break;}
# 2044
_tmp5E2=_tmp5E4;}}
# 2049
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5A3),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL394: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A4=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5A4->tag != 10)goto _LL396;else{_tmp5A5=_tmp5A4->f1;_tmp5A6=_tmp5A4->f2;}}_LL395: {
# 2053
union Cyc_CfFlowInfo_FlowInfo _tmp5E8;void*_tmp5E9;struct _tuple14 _tmp5E7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5A5);_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;
_tmp5E8=Cyc_NewControlFlow_use_Rval(env,_tmp5A5->loc,_tmp5E8,_tmp5E9);
return Cyc_NewControlFlow_anal_scs(env,_tmp5E8,_tmp5A6);}_LL396: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp5A7=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5A7->tag != 15)goto _LL398;else{_tmp5A8=_tmp5A7->f1;_tmp5A9=_tmp5A7->f2;}}_LL397: {
# 2060
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5A8);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2068
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2071
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5A9);
{union Cyc_CfFlowInfo_FlowInfo _tmp5EA=scs_outflow;_LL3A3: if((_tmp5EA.BottomFL).tag != 1)goto _LL3A5;_LL3A4:
 goto _LL3A2;_LL3A5:;_LL3A6: {
const char*_tmp8AA;void*_tmp8A9;(_tmp8A9=0,Cyc_Tcutil_terr(s->loc,((_tmp8AA="last catch clause may implicitly fallthru",_tag_dyneither(_tmp8AA,sizeof(char),42))),_tag_dyneither(_tmp8A9,sizeof(void*),0)));}_LL3A2:;}
# 2077
outflow=s1_outflow;
# 2079
return outflow;};};}_LL398: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5AA=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5AA->tag != 12)goto _LL39A;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp5AB=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(*_tmp5AA->f1).r;if(_tmp5AB->tag != 5)goto _LL39A;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AB->f2;_tmp5AE=_tmp5AB->f3;}}_tmp5AF=_tmp5AA->f2;}}_LL399: {
# 2085
union Cyc_CfFlowInfo_FlowInfo _tmp5EE;union Cyc_CfFlowInfo_AbsLVal _tmp5EF;struct _tuple15 _tmp5ED=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp5AC);_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;{
# 2089
struct _tuple15 _tmp8AB;struct _tuple15 _tmp5F1=(_tmp8AB.f1=_tmp5EE,((_tmp8AB.f2=_tmp5EF,_tmp8AB)));struct Cyc_Dict_Dict _tmp5F2;struct Cyc_List_List*_tmp5F3;struct Cyc_CfFlowInfo_Place*_tmp5F4;_LL3A8: if(((_tmp5F1.f1).ReachableFL).tag != 2)goto _LL3AA;_tmp5F2=((struct _tuple13)((_tmp5F1.f1).ReachableFL).val).f1;_tmp5F3=((struct _tuple13)((_tmp5F1.f1).ReachableFL).val).f2;if(((_tmp5F1.f2).PlaceL).tag != 1)goto _LL3AA;_tmp5F4=(struct Cyc_CfFlowInfo_Place*)((_tmp5F1.f2).PlaceL).val;_LL3A9: {
# 2091
void*_tmp5F5=Cyc_CfFlowInfo_lookup_place(_tmp5F2,_tmp5F4);
# 2093
void*t=(void*)_check_null(_tmp5AC->topt);
{void*_tmp5F6=_tmp5AC->r;struct Cyc_Absyn_Exp*_tmp5F8;_LL3AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5F7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5F6;if(_tmp5F7->tag != 13)goto _LL3B1;else{_tmp5F8=_tmp5F7->f2;}}_LL3B0:
 t=(void*)_check_null(_tmp5F8->topt);goto _LL3AE;_LL3B1:;_LL3B2:
 goto _LL3AE;_LL3AE:;}{
# 2098
void*_tmp5F9=Cyc_CfFlowInfo_make_unique_consumed(_tmp57C,t,_tmp5AC,env->iteration_num,_tmp5F5);
_tmp5F2=Cyc_CfFlowInfo_assign_place(_tmp57C,_tmp5AC->loc,_tmp5F2,env->all_changed,_tmp5F4,_tmp5F9);
_tmp5EE=Cyc_CfFlowInfo_ReachableFL(_tmp5F2,_tmp5F3);{
# 2104
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp57C,_tmp5F2,_tmp5F5)){case Cyc_CfFlowInfo_AllIL: _LL3B3:
 leaf=_tmp57C->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL3B4:
 leaf=_tmp57C->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL3B5:
 leaf=_tmp57C->unknown_this;break;}{
# 2110
void*_tmp5FA=Cyc_CfFlowInfo_typ_to_absrval(_tmp57C,t,0,leaf);
_tmp5EE=Cyc_NewControlFlow_use_Rval(env,_tmp5AC->loc,_tmp5EE,_tmp5F5);{
struct Cyc_List_List _tmp8AC;struct Cyc_List_List _tmp5FB=(_tmp8AC.hd=_tmp5AE,((_tmp8AC.tl=0,_tmp8AC)));
_tmp5EE=Cyc_NewControlFlow_add_vars(_tmp57C,_tmp5EE,(struct Cyc_List_List*)& _tmp5FB,_tmp57C->unknown_all,s->loc);
# 2117
_tmp5EE=Cyc_NewControlFlow_anal_stmt(env,_tmp5EE,_tmp5AF);{
# 2122
union Cyc_CfFlowInfo_FlowInfo _tmp5FC=_tmp5EE;struct Cyc_Dict_Dict _tmp5FD;struct Cyc_List_List*_tmp5FE;_LL3B8: if((_tmp5FC.ReachableFL).tag != 2)goto _LL3BA;_tmp5FD=((struct _tuple13)(_tmp5FC.ReachableFL).val).f1;_tmp5FE=((struct _tuple13)(_tmp5FC.ReachableFL).val).f2;_LL3B9:
# 2124
 _tmp5FD=Cyc_CfFlowInfo_assign_place(_tmp57C,s->loc,_tmp5FD,env->all_changed,_tmp5F4,_tmp5FA);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5FD,_tmp5FE);
# 2129
return outflow;_LL3BA:;_LL3BB:
 return _tmp5EE;_LL3B7:;};};};};};}_LL3AA: if(((_tmp5F1.f1).BottomFL).tag != 1)goto _LL3AC;_LL3AB:
# 2132
 return _tmp5EE;_LL3AC:;_LL3AD:
{const char*_tmp8AF;void*_tmp8AE;(_tmp8AE=0,Cyc_Tcutil_terr(_tmp5AC->loc,((_tmp8AF="bad alias expression--no unique path found",_tag_dyneither(_tmp8AF,sizeof(char),43))),_tag_dyneither(_tmp8AE,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL3A7:;};}_LL39A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5B0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5B0->tag != 12)goto _LL39C;else{_tmp5B1=_tmp5B0->f1;_tmp5B2=_tmp5B0->f2;}}_LL39B:
# 2138
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5B1),_tmp5B2);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5B1);
return outflow;_LL39C: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B3=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5B3->tag != 13)goto _LL39E;else{_tmp5B4=_tmp5B3->f2;}}_LL39D:
# 2143
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5B4);_LL39E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5B5=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp57D;if(_tmp5B5->tag != 16)goto _LL3A0;else{_tmp5B6=_tmp5B5->f1;}}_LL39F: {
# 2146
union Cyc_CfFlowInfo_FlowInfo _tmp603;void*_tmp604;struct _tuple14 _tmp602=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5B6);_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp605=Cyc_NewControlFlow_use_Rval(env,_tmp5B6->loc,_tmp603,_tmp604);
void*_tmp606=Cyc_Tcutil_compress((void*)_check_null(_tmp5B6->topt));void*_tmp608;_LL3BD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 15)goto _LL3BF;else{_tmp608=(void*)_tmp607->f1;}}_LL3BE:
# 2150
 return Cyc_CfFlowInfo_kill_flow_region(_tmp57C,_tmp603,_tmp608);_LL3BF:;_LL3C0: {
struct Cyc_Core_Impossible_exn_struct _tmp8B5;const char*_tmp8B4;struct Cyc_Core_Impossible_exn_struct*_tmp8B3;(int)_throw((void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B5.tag=Cyc_Core_Impossible,((_tmp8B5.f1=((_tmp8B4="anal_stmt -- reset_region",_tag_dyneither(_tmp8B4,sizeof(char),26))),_tmp8B5)))),_tmp8B3)))));}_LL3BC:;};}_LL3A0:;_LL3A1: {
# 2154
struct Cyc_Core_Impossible_exn_struct _tmp8BB;const char*_tmp8BA;struct Cyc_Core_Impossible_exn_struct*_tmp8B9;(int)_throw((void*)((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9[0]=((_tmp8BB.tag=Cyc_Core_Impossible,((_tmp8BB.f1=((_tmp8BA="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp8BA,sizeof(char),56))),_tmp8BB)))),_tmp8B9)))));}_LL377:;};}
# 2159
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2161
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp60F=env->fenv;
void*_tmp610=decl->r;struct Cyc_Absyn_Vardecl*_tmp612;struct Cyc_List_List*_tmp614;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_List_List*_tmp617;struct Cyc_Absyn_Fndecl*_tmp619;struct Cyc_Absyn_Tvar*_tmp61B;struct Cyc_Absyn_Vardecl*_tmp61C;int _tmp61D;struct Cyc_Absyn_Exp*_tmp61E;_LL3C2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp611=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp611->tag != 0)goto _LL3C4;else{_tmp612=_tmp611->f1;}}_LL3C3: {
# 2171
struct Cyc_List_List _tmp8BC;struct Cyc_List_List _tmp61F=(_tmp8BC.hd=_tmp612,((_tmp8BC.tl=0,_tmp8BC)));
inflow=Cyc_NewControlFlow_add_vars(_tmp60F,inflow,(struct Cyc_List_List*)& _tmp61F,_tmp60F->unknown_none,decl->loc);{
struct Cyc_Absyn_Exp*_tmp620=_tmp612->initializer;
if(_tmp620 == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp622;void*_tmp623;struct _tuple14 _tmp621=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_tmp620);_tmp622=_tmp621.f1;_tmp623=_tmp621.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp624=_tmp622;struct Cyc_Dict_Dict _tmp625;struct Cyc_List_List*_tmp626;_LL3CF: if((_tmp624.BottomFL).tag != 1)goto _LL3D1;_LL3D0:
 return Cyc_CfFlowInfo_BottomFL();_LL3D1: if((_tmp624.ReachableFL).tag != 2)goto _LL3CE;_tmp625=((struct _tuple13)(_tmp624.ReachableFL).val).f1;_tmp626=((struct _tuple13)(_tmp624.ReachableFL).val).f2;_LL3D2:
# 2180
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8C2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8C1;struct Cyc_CfFlowInfo_Place*_tmp8C0;_tmp625=Cyc_CfFlowInfo_assign_place(_tmp60F,decl->loc,_tmp625,env->all_changed,(
(_tmp8C0=_region_malloc(env->r,sizeof(*_tmp8C0)),((_tmp8C0->root=(void*)((_tmp8C2=_region_malloc(env->r,sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C1.tag=0,((_tmp8C1.f1=_tmp612,_tmp8C1)))),_tmp8C2)))),((_tmp8C0->fields=0,_tmp8C0)))))),_tmp623);}
# 2183
_tmp626=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp626,_tmp612,(struct Cyc_Absyn_Exp*)_check_null(_tmp612->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp62A=Cyc_CfFlowInfo_ReachableFL(_tmp625,_tmp626);
Cyc_NewControlFlow_update_tryflow(env,_tmp62A);
# 2188
return _tmp62A;};_LL3CE:;};};};}_LL3C4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp613=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp613->tag != 2)goto _LL3C6;else{if(_tmp613->f2 == 0)goto _LL3C6;_tmp614=(struct Cyc_List_List*)(*_tmp613->f2).v;_tmp615=_tmp613->f3;}}_LL3C5: {
# 2193
union Cyc_CfFlowInfo_FlowInfo _tmp62D;void*_tmp62E;struct _tuple14 _tmp62C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp615);_tmp62D=_tmp62C.f1;_tmp62E=_tmp62C.f2;
_tmp62D=Cyc_NewControlFlow_use_Rval(env,_tmp615->loc,_tmp62D,_tmp62E);
return Cyc_NewControlFlow_add_vars(_tmp60F,_tmp62D,_tmp614,_tmp60F->unknown_all,decl->loc);}_LL3C6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp616=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp616->tag != 3)goto _LL3C8;else{_tmp617=_tmp616->f1;}}_LL3C7:
# 2199
 return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,_tmp617,_tmp60F->unknown_none,decl->loc);_LL3C8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp618=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp618->tag != 1)goto _LL3CA;else{_tmp619=_tmp618->f1;}}_LL3C9:
# 2202
 Cyc_NewControlFlow_check_nested_fun(_tmp60F,inflow,_tmp619);{
void*t=(void*)_check_null(_tmp619->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp62F=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp619->fn_vardecl);
# 2208
struct Cyc_List_List*_tmp8C3;return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,((_tmp8C3=_region_malloc(env->r,sizeof(*_tmp8C3)),((_tmp8C3->hd=_tmp62F,((_tmp8C3->tl=0,_tmp8C3)))))),_tmp60F->unknown_all,decl->loc);};_LL3CA: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp61A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp610;if(_tmp61A->tag != 4)goto _LL3CC;else{_tmp61B=_tmp61A->f1;_tmp61C=_tmp61A->f2;_tmp61D=_tmp61A->f3;_tmp61E=_tmp61A->f4;}}_LL3CB:
# 2211
 if(_tmp61E != 0){
struct Cyc_Absyn_Exp*_tmp631=(struct Cyc_Absyn_Exp*)_tmp61E;
union Cyc_CfFlowInfo_FlowInfo _tmp633;void*_tmp634;struct _tuple14 _tmp632=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp631);_tmp633=_tmp632.f1;_tmp634=_tmp632.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp631->loc,_tmp633,_tmp634);}{
# 2216
struct Cyc_List_List _tmp8C4;struct Cyc_List_List _tmp635=(_tmp8C4.hd=_tmp61C,((_tmp8C4.tl=0,_tmp8C4)));
return Cyc_NewControlFlow_add_vars(_tmp60F,inflow,(struct Cyc_List_List*)& _tmp635,_tmp60F->unknown_all,decl->loc);};_LL3CC:;_LL3CD: {
# 2220
struct Cyc_Core_Impossible_exn_struct _tmp8CA;const char*_tmp8C9;struct Cyc_Core_Impossible_exn_struct*_tmp8C8;(int)_throw((void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8CA.tag=Cyc_Core_Impossible,((_tmp8CA.f1=((_tmp8C9="anal_decl: unexpected decl variant",_tag_dyneither(_tmp8C9,sizeof(char),35))),_tmp8CA)))),_tmp8C8)))));}_LL3C1:;}
# 2260 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp63A=_new_region("frgn");struct _RegionHandle*frgn=& _tmp63A;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2262
;_pop_region(frgn);}
# 2267
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2271
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2275
struct _RegionHandle*_tmp63B=fenv->r;
unsigned int _tmp63C=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp63C);{
# 2281
struct Cyc_List_List*param_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp63D=inflow;struct Cyc_Dict_Dict _tmp63E;struct Cyc_List_List*_tmp63F;_LL3D4: if((_tmp63D.BottomFL).tag != 1)goto _LL3D6;_LL3D5: {
const char*_tmp8CD;void*_tmp8CC;(_tmp8CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8CD="check_fun",_tag_dyneither(_tmp8CD,sizeof(char),10))),_tag_dyneither(_tmp8CC,sizeof(void*),0)));}_LL3D6: if((_tmp63D.ReachableFL).tag != 2)goto _LL3D3;_tmp63E=((struct _tuple13)(_tmp63D.ReachableFL).val).f1;_tmp63F=((struct _tuple13)(_tmp63D.ReachableFL).val).f2;_LL3D7: {
# 2287
struct Cyc_List_List*atts;
{void*_tmp642=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp644;_LL3D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp643=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp642;if(_tmp643->tag != 9)goto _LL3DB;else{_tmp644=(_tmp643->f1).attributes;}}_LL3DA:
 atts=_tmp644;goto _LL3D8;_LL3DB:;_LL3DC: {
const char*_tmp8D0;void*_tmp8CF;(_tmp8CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8D0="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp8D0,sizeof(char),50))),_tag_dyneither(_tmp8CF,sizeof(void*),0)));}_LL3D8:;}
# 2292
for(0;atts != 0;atts=atts->tl){
void*_tmp647=(void*)atts->hd;int _tmp649;int _tmp64B;int _tmp64D;_LL3DE: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp648=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp647;if(_tmp648->tag != 21)goto _LL3E0;else{_tmp649=_tmp648->f1;}}_LL3DF: {
# 2295
unsigned int j=(unsigned int)_tmp649;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8D3;void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8D3="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp8D3,sizeof(char),52))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}
continue;}{
# 2301
struct Cyc_Absyn_Vardecl*_tmp650=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp650->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8D6="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp8D6,sizeof(char),52))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
continue;}{
# 2307
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp653=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8D9;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8D8;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp654=(_tmp8D8=_region_malloc(_tmp63B,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=2,((_tmp8D9.f1=(int)j,((_tmp8D9.f2=(void*)t,_tmp8D9)))))),_tmp8D8)));
struct Cyc_CfFlowInfo_Place*_tmp8DA;struct Cyc_CfFlowInfo_Place*_tmp655=(_tmp8DA=_region_malloc(_tmp63B,sizeof(*_tmp8DA)),((_tmp8DA->root=(void*)_tmp654,((_tmp8DA->fields=0,_tmp8DA)))));
_tmp63E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63E,(void*)_tmp654,_tmp653);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8E0;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8DF;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8DD;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8DC;_tmp63E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63E,(void*)((_tmp8DC=_region_malloc(_tmp63B,sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=0,((_tmp8DD.f1=_tmp650,_tmp8DD)))),_tmp8DC)))),(void*)((_tmp8DF=_region_malloc(_tmp63B,sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E0.tag=5,((_tmp8E0.f1=_tmp655,_tmp8E0)))),_tmp8DF)))));}
goto _LL3DD;};};}_LL3E0: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp64A=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp647;if(_tmp64A->tag != 20)goto _LL3E2;else{_tmp64B=_tmp64A->f1;}}_LL3E1: {
# 2317
unsigned int j=(unsigned int)_tmp64B;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8E3="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp8E3,sizeof(char),51))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}
# 2321
continue;}{
# 2323
struct Cyc_Absyn_Vardecl*_tmp65F=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp660=Cyc_Tcutil_compress(_tmp65F->type);void*_tmp662;union Cyc_Absyn_Constraint*_tmp663;union Cyc_Absyn_Constraint*_tmp664;union Cyc_Absyn_Constraint*_tmp665;_LL3E7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp661=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp660;if(_tmp661->tag != 5)goto _LL3E9;else{_tmp662=(_tmp661->f1).elt_typ;_tmp663=((_tmp661->f1).ptr_atts).nullable;_tmp664=((_tmp661->f1).ptr_atts).bounds;_tmp665=((_tmp661->f1).ptr_atts).zero_term;}}_LL3E8:
# 2326
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp663)){
const char*_tmp8E6;void*_tmp8E5;(_tmp8E5=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8E6="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp8E6,sizeof(char),55))),_tag_dyneither(_tmp8E5,sizeof(void*),0)));}
# 2329
if(!Cyc_Tcutil_is_bound_one(_tmp664)){
const char*_tmp8E9;void*_tmp8E8;(_tmp8E8=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8E9="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp8E9,sizeof(char),57))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}
# 2332
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp665)){
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8EC="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp8EC,sizeof(char),77))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}{
# 2335
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8EF;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8EE;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp66C=(_tmp8EE=_region_malloc(_tmp63B,sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8EF.tag=2,((_tmp8EF.f1=(int)j,((_tmp8EF.f2=(void*)_tmp662,_tmp8EF)))))),_tmp8EE)));
struct Cyc_CfFlowInfo_Place*_tmp8F0;struct Cyc_CfFlowInfo_Place*_tmp66D=(_tmp8F0=_region_malloc(_tmp63B,sizeof(*_tmp8F0)),((_tmp8F0->root=(void*)_tmp66C,((_tmp8F0->fields=0,_tmp8F0)))));
_tmp63E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63E,(void*)_tmp66C,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp662,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8F6;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8F5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8F3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8F2;_tmp63E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63E,(void*)((_tmp8F2=_region_malloc(_tmp63B,sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=0,((_tmp8F3.f1=_tmp65F,_tmp8F3)))),_tmp8F2)))),(void*)((_tmp8F5=_region_malloc(_tmp63B,sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F6.tag=5,((_tmp8F6.f1=_tmp66D,_tmp8F6)))),_tmp8F5)))));}
{struct Cyc_List_List*_tmp8F7;param_roots=((_tmp8F7=_region_malloc(_tmp63B,sizeof(*_tmp8F7)),((_tmp8F7->hd=_tmp66D,((_tmp8F7->tl=param_roots,_tmp8F7))))));}
goto _LL3E6;};_LL3E9:;_LL3EA: {
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8FA="initializes attribute on non-pointer",_tag_dyneither(_tmp8FA,sizeof(char),37))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}_LL3E6:;}
# 2343
goto _LL3DD;};}_LL3E2: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp64C=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp647;if(_tmp64C->tag != 22)goto _LL3E4;else{_tmp64D=_tmp64C->f1;}}_LL3E3: {
# 2345
unsigned int j=(unsigned int)_tmp64D;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8FD;void*_tmp8FC;(_tmp8FC=0,Cyc_Tcutil_terr(_tmp63C,((_tmp8FD="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp8FD,sizeof(char),49))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}
continue;}{
# 2350
struct Cyc_Absyn_Vardecl*_tmp67A=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp67A->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_Tcutil_terr(_tmp63C,((_tmp900="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp900,sizeof(char),49))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}
continue;}
# 2357
{const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_Tcutil_terr(_tmp63C,((_tmp903="noconsume attribute not yet supported on fn defs",_tag_dyneither(_tmp903,sizeof(char),49))),_tag_dyneither(_tmp902,sizeof(void*),0)));}
continue;};}_LL3E4:;_LL3E5:
 goto _LL3DD;_LL3DD:;}
# 2361
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp63E,_tmp63F);}_LL3D3:;}{
# 2364
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp63B,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp904;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp904=_region_malloc(_tmp63B,sizeof(*_tmp904)),((_tmp904->r=_tmp63B,((_tmp904->fenv=fenv,((_tmp904->iterate_again=1,((_tmp904->iteration_num=0,((_tmp904->in_try=0,((_tmp904->tryflow=inflow,((_tmp904->all_changed=0,((_tmp904->noreturn=noreturn,((_tmp904->param_roots=param_roots,((_tmp904->flow_table=flow_table,_tmp904)))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2374
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2377
union Cyc_CfFlowInfo_FlowInfo _tmp67F=outflow;_LL3EC: if((_tmp67F.BottomFL).tag != 1)goto _LL3EE;_LL3ED:
 goto _LL3EB;_LL3EE:;_LL3EF:
# 2380
 Cyc_NewControlFlow_check_init_params(_tmp63C,env,outflow);
# 2383
if(noreturn){
const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_Tcutil_terr(_tmp63C,((_tmp907="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp907,sizeof(char),46))),_tag_dyneither(_tmp906,sizeof(void*),0)));}else{
# 2386
void*_tmp682=Cyc_Tcutil_compress(fd->ret_type);_LL3F1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp683=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp682;if(_tmp683->tag != 0)goto _LL3F3;}_LL3F2:
 goto _LL3F0;_LL3F3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp684=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp682;if(_tmp684->tag != 7)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp685=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp682;if(_tmp685->tag != 6)goto _LL3F7;}_LL3F6:
# 2390
{const char*_tmp90A;void*_tmp909;(_tmp909=0,Cyc_Tcutil_warn(_tmp63C,((_tmp90A="function may not return a value",_tag_dyneither(_tmp90A,sizeof(char),32))),_tag_dyneither(_tmp909,sizeof(void*),0)));}goto _LL3F0;_LL3F7:;_LL3F8:
# 2392
{const char*_tmp90D;void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_terr(_tmp63C,((_tmp90D="function may not return a value",_tag_dyneither(_tmp90D,sizeof(char),32))),_tag_dyneither(_tmp90C,sizeof(void*),0)));}goto _LL3F0;_LL3F0:;}
# 2394
goto _LL3EB;_LL3EB:;};};};}
# 2399
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp68B=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp68D;struct Cyc_List_List*_tmp68F;struct Cyc_List_List*_tmp691;struct Cyc_List_List*_tmp693;_LL3FA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp68C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp68B;if(_tmp68C->tag != 1)goto _LL3FC;else{_tmp68D=_tmp68C->f1;}}_LL3FB:
 Cyc_NewControlFlow_check_fun(_tmp68D);goto _LL3F9;_LL3FC: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp68E=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp68B;if(_tmp68E->tag != 12)goto _LL3FE;else{_tmp68F=_tmp68E->f1;}}_LL3FD:
 _tmp691=_tmp68F;goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp690=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp68B;if(_tmp690->tag != 11)goto _LL400;else{_tmp691=_tmp690->f2;}}_LL3FF:
 _tmp693=_tmp691;goto _LL401;_LL400: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp692=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp68B;if(_tmp692->tag != 10)goto _LL402;else{_tmp693=_tmp692->f2;}}_LL401:
 Cyc_NewControlFlow_cf_check(_tmp693);goto _LL3F9;_LL402: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp694=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp68B;if(_tmp694->tag != 13)goto _LL404;}_LL403:
 goto _LL3F9;_LL404:;_LL405:
 goto _LL3F9;_LL3F9:;}}
