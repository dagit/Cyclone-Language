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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp728;struct Cyc_NewControlFlow_CFStmtAnnot _tmp727;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp726;enclosee->annot=(void*)((_tmp726=_cycalloc(sizeof(*_tmp726)),((_tmp726[0]=((_tmp728.tag=Cyc_NewControlFlow_CFAnnot,((_tmp728.f1=((_tmp727.encloser=encloser,((_tmp727.visited=0,_tmp727)))),_tmp728)))),_tmp726))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp5=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4;if(_tmp5->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp5->f1;}}_LL2:
 return _tmp6;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp72E;const char*_tmp72D;struct Cyc_Core_Impossible_exn_struct*_tmp72C;(int)_throw((void*)((_tmp72C=_cycalloc(sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp72E.tag=Cyc_Core_Impossible,((_tmp72E.f1=((_tmp72D="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp72D,sizeof(char),37))),_tmp72E)))),_tmp72C)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp72F;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp72F=_region_malloc(env->r,sizeof(*_tmp72F)),((_tmp72F[0]=Cyc_CfFlowInfo_BottomFL(),_tmp72F)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 144
return*sflow;}struct _tuple19{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 147
static struct _tuple19 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 155
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 161
_tmpB->visited=env->iteration_num;{
struct _tuple19 _tmp730;return(_tmp730.f1=_tmpB,((_tmp730.f2=_tmpC,_tmp730)));};}
# 171
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 179
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple20{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 186
static void Cyc_NewControlFlow_check_unique_root(struct _tuple20*ckenv,void*root,void*rval){
# 188
struct Cyc_NewControlFlow_AnalEnv*_tmpF;unsigned int _tmp10;struct Cyc_Dict_Dict _tmp11;struct _tuple20*_tmpE=ckenv;_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;{
void*_tmp12=root;struct Cyc_Absyn_Vardecl*_tmp14;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp13=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp12;if(_tmp13->tag != 0)goto _LL8;else{_tmp14=_tmp13->f1;}}_LL7:
# 191
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpF->fenv)->r,_tmp14->type)){
retry: {void*_tmp15=rval;void*_tmp17;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp16->tag != 8)goto _LLD;else{_tmp17=(void*)_tmp16->f2;}}_LLC:
 rval=_tmp17;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp18->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp19->tag != 3)goto _LL11;else{if(_tmp19->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp1A->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 200
{const char*_tmp734;void*_tmp733[1];struct Cyc_String_pa_PrintArg_struct _tmp732;(_tmp732.tag=0,((_tmp732.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp733[0]=& _tmp732,Cyc_Tcutil_warn(_tmp10,((_tmp734="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp734,sizeof(char),60))),_tag_dyneither(_tmp733,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 204
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 212
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1F);
# 218
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple18 _tmp735;struct _tuple18 _tmp21=(_tmp735.f1=flow,((_tmp735.f2=_tmp20,_tmp735)));struct _tuple18 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple20 _tmp736;struct _tuple20 _tmp25=(_tmp736.f1=env,((_tmp736.f2=s->loc,((_tmp736.f3=_tmp24,_tmp736)))));
((void(*)(void(*f)(struct _tuple20*,void*,void*),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp25,_tmp23);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 227
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){
# 234
*_tmp1F=_tmp20;
# 238
if(_tmp1E->visited == env->iteration_num)
# 240
env->iterate_again=1;}}
# 245
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 249
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_Dict_Dict _tmp29;struct Cyc_List_List*_tmp2A;_LL1B: if((_tmp28.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp28.ReachableFL).tag != 2)goto _LL1A;_tmp29=((struct _tuple14)(_tmp28.ReachableFL).val).f1;_tmp2A=((struct _tuple14)(_tmp28.ReachableFL).val).f2;_LL1E:
# 252
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp739;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp738;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp738=_region_malloc(fenv->r,sizeof(*_tmp738)),((_tmp738[0]=((_tmp739.tag=0,((_tmp739.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp739)))),_tmp738)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp73C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp73B;_tmp2C=(void*)((_tmp73B=_region_malloc(fenv->r,sizeof(*_tmp73B)),((_tmp73B[0]=((_tmp73C.tag=8,((_tmp73C.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp73C.f2=_tmp2C,_tmp73C)))))),_tmp73B))));}
# 259
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp73D;if(Cyc_Relations_consistent_relations(((_tmp73D=_region_malloc(r,sizeof(*_tmp73D)),((_tmp73D->hd=_tmp31,((_tmp73D->tl=assume,_tmp73D))))))))
return 0;}
# 277
return 1;}
# 280
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp33=e->r;void*_tmp34=_tmp33;struct Cyc_Absyn_Exp*_tmp36;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34;if(_tmp35->tag != 13)goto _LL22;else{_tmp36=_tmp35->f2;}}_LL21:
 return _tmp36;_LL22:;_LL23:
 return e;_LL1F:;}
# 287
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 289
union Cyc_CfFlowInfo_FlowInfo _tmp37=inflow;struct Cyc_List_List*_tmp38;_LL25: if((_tmp37.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp37.ReachableFL).tag != 2)goto _LL24;_tmp38=((struct _tuple14)(_tmp37.ReachableFL).val).f2;_LL28:
# 292
{void*_tmp39=Cyc_Tcutil_compress(t);void*_tmp3A=_tmp39;union Cyc_Absyn_AggrInfoU _tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3F;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3B->tag != 11)goto _LL2C;else{_tmp3C=(_tmp3B->f1).aggr_info;_tmp3D=(_tmp3B->f1).targs;}}_LL2B: {
# 294
struct Cyc_Absyn_Aggrdecl*_tmp40=Cyc_Absyn_get_known_aggrdecl(_tmp3C);
struct Cyc_Absyn_Aggrfield*_tmp41=Cyc_Absyn_lookup_decl_field(_tmp40,f);
struct Cyc_Absyn_Exp*_tmp42=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp41))->requires_clause;
if(_tmp42 != 0){
struct _RegionHandle _tmp43=_new_region("temp");struct _RegionHandle*temp=& _tmp43;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp44=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp40->tvs,_tmp3D),_tmp42);
# 301
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp44))){
{const char*_tmp742;void*_tmp741[2];struct Cyc_String_pa_PrintArg_struct _tmp740;struct Cyc_String_pa_PrintArg_struct _tmp73F;(_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp740.tag=0,((_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp741[0]=& _tmp740,((_tmp741[1]=& _tmp73F,Cyc_CfFlowInfo_aerr(loc,((_tmp742="unable to prove %s, required to access %s",_tag_dyneither(_tmp742,sizeof(char),42))),_tag_dyneither(_tmp741,sizeof(void*),2)))))))))))));}
{const char*_tmp745;void*_tmp744;(_tmp744=0,Cyc_fprintf(Cyc_stderr,((_tmp745="  [recorded facts on path: ",_tag_dyneither(_tmp745,sizeof(char),28))),_tag_dyneither(_tmp744,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp748;void*_tmp747;(_tmp747=0,Cyc_fprintf(Cyc_stderr,((_tmp748="]\n",_tag_dyneither(_tmp748,sizeof(char),3))),_tag_dyneither(_tmp747,sizeof(void*),0)));};}}
# 299
;_pop_region(temp);}
# 308
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3E->tag != 12)goto _LL2E;else{_tmp3F=_tmp3E->f2;}}_LL2D: {
# 310
struct Cyc_Absyn_Aggrfield*_tmp4D=Cyc_Absyn_lookup_field(_tmp3F,f);
struct Cyc_Absyn_Exp*_tmp4E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp4D))->requires_clause;
if(_tmp4E != 0){
struct _RegionHandle _tmp4F=_new_region("temp");struct _RegionHandle*temp=& _tmp4F;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp4E))){
{const char*_tmp74D;void*_tmp74C[2];struct Cyc_String_pa_PrintArg_struct _tmp74B;struct Cyc_String_pa_PrintArg_struct _tmp74A;(_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp74B.tag=0,((_tmp74B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp74C[0]=& _tmp74B,((_tmp74C[1]=& _tmp74A,Cyc_CfFlowInfo_aerr(loc,((_tmp74D="unable to prove %s, required to access %s",_tag_dyneither(_tmp74D,sizeof(char),42))),_tag_dyneither(_tmp74C,sizeof(void*),2)))))))))))));}
{const char*_tmp750;void*_tmp74F;(_tmp74F=0,Cyc_fprintf(Cyc_stderr,((_tmp750="  [recorded facts on path: ",_tag_dyneither(_tmp750,sizeof(char),28))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_fprintf(Cyc_stderr,((_tmp753="]\n",_tag_dyneither(_tmp753,sizeof(char),3))),_tag_dyneither(_tmp752,sizeof(void*),0)));};}
# 314
;_pop_region(temp);}
# 321
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 324
goto _LL24;_LL24:;}
# 328
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp58=inflow;struct Cyc_Dict_Dict _tmp59;struct Cyc_List_List*_tmp5A;_LL31: if((_tmp58.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp58.ReachableFL).tag != 2)goto _LL30;_tmp59=((struct _tuple14)(_tmp58.ReachableFL).val).f1;_tmp5A=((struct _tuple14)(_tmp58.ReachableFL).val).f2;_LL34:
# 332
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp59,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_CfFlowInfo_aerr(loc,((_tmp756="expression may not be fully initialized",_tag_dyneither(_tmp756,sizeof(char),40))),_tag_dyneither(_tmp755,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp757;struct _tuple0 _tmp5F=(_tmp757.f1=Cyc_Tcutil_compress(t),((_tmp757.f2=r,_tmp757)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp62=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp62->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp63->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp64->tag != 8)goto _LL3E;else{_tmp65=(void*)_tmp64->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp65);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60.f1;if(_tmp66->tag != 4)goto _LL40;else{if((((_tmp66->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp67=((struct _tuple3)(((_tmp66->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp68=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp68->tag != 6)goto _LL40;else{_tmp69=_tmp68->f2;}};_LL3F:
# 349
 if(_tmp67->typs == 0)
return;
_tmp6B=_tmp67->typs;_tmp6D=_tmp69;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6A->tag != 10)goto _LL42;else{_tmp6B=_tmp6A->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp6C->tag != 6)goto _LL42;else{_tmp6D=_tmp6C->f2;}};_LL41: {
# 353
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B);
{int i=0;for(0;i < sz;(i ++,_tmp6B=_tmp6B->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple21*)((struct Cyc_List_List*)_check_null(_tmp6B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp6D,sizeof(void*),i)));}}
# 357
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6E->tag != 11)goto _LL44;else{_tmp6F=(_tmp6E->f1).aggr_info;_tmp70=(_tmp6E->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp71->tag != 6)goto _LL44;else{_tmp72=_tmp71->f2;}};_LL43: {
# 360
struct Cyc_Absyn_Aggrdecl*_tmp79=Cyc_Absyn_get_known_aggrdecl(_tmp6F);
if(_tmp79->impl == 0)return;{
struct Cyc_List_List*_tmp7A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->fields;
struct _RegionHandle _tmp7B=_new_region("temp");struct _RegionHandle*temp=& _tmp7B;_push_region(temp);
{struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp79->tvs,_tmp70);
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp7A=_tmp7A->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp7A))->hd)->type;
if(_tmp7C != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp7C,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp72.curr)[i]);}}
# 370
_npop_handler(0);return;}
# 364
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp73->tag != 12)goto _LL46;else{_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp76=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp76->tag != 6)goto _LL46;else{_tmp77=_tmp76->f2;}};_LL45:
# 373
{int i=0;for(0;i < _get_dyneither_size(_tmp77,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type,((void**)_tmp77.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60.f1;if(_tmp78->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp75A;void*_tmp759;(_tmp759=0,Cyc_Tcutil_warn(loc,((_tmp75A="argument may still contain unique pointers",_tag_dyneither(_tmp75A,sizeof(char),43))),_tag_dyneither(_tmp759,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp75D;void*_tmp75C;(_tmp75C=0,Cyc_CfFlowInfo_aerr(loc,((_tmp75D="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp75D,sizeof(char),47))),_tag_dyneither(_tmp75C,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 391
void*_tmp85=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp86=r;void*_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8A;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp87=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp87->tag != 8)goto _LL52;else{_tmp88=(void*)_tmp87->f2;}}_LL51:
 r=_tmp88;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp89=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp89->tag != 5)goto _LL54;else{_tmp8A=_tmp89->f1;}}_LL53:
# 395
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp85,Cyc_CfFlowInfo_lookup_place(_tmp81,_tmp8A));
goto _LL4F;_LL54:;_LL55:
# 398
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp85)){
const char*_tmp760;void*_tmp75F;(_tmp75F=0,Cyc_Tcutil_warn(loc,((_tmp760="argument may contain live unique pointers",_tag_dyneither(_tmp760,sizeof(char),42))),_tag_dyneither(_tmp75F,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 402
struct Cyc_Dict_Dict _tmp8D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp81,env->all_changed,r);
if(_tmp81.t == _tmp8D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp8E=Cyc_CfFlowInfo_ReachableFL(_tmp8D,_tmp82);
Cyc_NewControlFlow_update_tryflow(env,_tmp8E);
return _tmp8E;};};};_LL4A:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 411
static struct _tuple22 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 418
if(es == 0){
struct _tuple22 _tmp761;return(_tmp761.f1=inflow,((_tmp761.f2=0,_tmp761)));}
if(es->tl == 0){
struct _tuple15 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple15 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp764;struct _tuple22 _tmp763;return(_tmp763.f1=_tmp92,((_tmp763.f2=((_tmp764=_region_malloc(rgn,sizeof(*_tmp764)),((_tmp764->hd=_tmp93,((_tmp764->tl=0,_tmp764)))))),_tmp763)));};}{
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
{struct Cyc_Dict_Dict*_tmp765;env->all_changed=((_tmp765=_region_malloc(env->r,sizeof(*_tmp765)),((_tmp765[0]=(env->fenv)->mt_place_set,_tmp765))));}{
struct _tuple15 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple15 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp766;rvals=((_tmp766=_region_malloc(rgn,sizeof(*_tmp766)),((_tmp766->hd=_tmp9A,((_tmp766->tl=0,_tmp766))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp767;env->all_changed=((_tmp767=_region_malloc(env->r,sizeof(*_tmp767)),((_tmp767[0]=(env->fenv)->mt_place_set,_tmp767))));}{
struct _tuple15 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple15 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp768;rvals=((_tmp768=_region_malloc(rgn,sizeof(*_tmp768)),((_tmp768->hd=_tmpA0,((_tmp768->tl=rvals,_tmp768))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp9F,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
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
struct Cyc_Dict_Dict*_tmp769;env->all_changed=((_tmp769=_region_malloc(env->r,sizeof(*_tmp769)),((_tmp769[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp769))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple22 _tmp76A;return(_tmp76A.f1=outflow,((_tmp76A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp76A)));};};}
# 469
static struct _tuple15 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 472
struct _RegionHandle _tmpA4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA4;_push_region(rgn);
{struct _tuple22 _tmpA5=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 473
union Cyc_CfFlowInfo_FlowInfo _tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple22 _tmpA6=_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
# 475
{union Cyc_CfFlowInfo_FlowInfo _tmpA9=_tmpA7;struct Cyc_Dict_Dict _tmpAA;_LL57: if((_tmpA9.ReachableFL).tag != 2)goto _LL59;_tmpAA=((struct _tuple14)(_tmpA9.ReachableFL).val).f1;_LL58:
# 477
 for(0;_tmpA8 != 0;(_tmpA8=_tmpA8->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAA,(void*)_tmpA8->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp76D;void*_tmp76C;(_tmp76C=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp76D="expression may not be initialized",_tag_dyneither(_tmp76D,sizeof(char),34))),_tag_dyneither(_tmp76C,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple15 _tmp76E;struct _tuple15 _tmpAE=(_tmp76E.f1=_tmpA7,((_tmp76E.f2=(env->fenv)->unknown_all,_tmp76E)));_npop_handler(0);return _tmpAE;};}
# 473
;_pop_region(rgn);}
# 491
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 498
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp774;const char*_tmp773;struct Cyc_Core_Impossible_exn_struct*_tmp772;(int)_throw((void*)((_tmp772=_cycalloc(sizeof(*_tmp772)),((_tmp772[0]=((_tmp774.tag=Cyc_Core_Impossible,((_tmp774.f1=((_tmp773="consume_zero_ral",_tag_dyneither(_tmp773,sizeof(char),17))),_tmp774)))),_tmp772)))));}else{
# 503
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 507
return new_rval;}}}
# 520 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 526
union Cyc_CfFlowInfo_FlowInfo _tmpB3=outflow;struct Cyc_Dict_Dict _tmpB4;struct Cyc_List_List*_tmpB5;_LL5C: if((_tmpB3.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpB3.ReachableFL).tag != 2)goto _LL5B;_tmpB4=((struct _tuple14)(_tmpB3.ReachableFL).val).f1;_tmpB5=((struct _tuple14)(_tmpB3.ReachableFL).val).f2;_LL5F: {
# 529
union Cyc_CfFlowInfo_AbsLVal _tmpB6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB7=_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL61: if((_tmpB7.UnknownL).tag != 2)goto _LL63;_LL62:
# 533
 return outflow;_LL63: if((_tmpB7.PlaceL).tag != 1)goto _LL60;_tmpB8=(struct Cyc_CfFlowInfo_Place*)(_tmpB7.PlaceL).val;_LL64: {
# 537
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp777;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp776;nzval=(void*)((_tmp776=_region_malloc((env->fenv)->r,sizeof(*_tmp776)),((_tmp776[0]=((_tmp777.tag=8,((_tmp777.f1=locname,((_tmp777.f2=nzval,_tmp777)))))),_tmp776))));}
# 541
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 546
return _tmpBB;};}_LL60:;}_LL5B:;}
# 555
static struct _tuple18 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 558
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple18 _tmp778;return(_tmp778.f1=outflow,((_tmp778.f2=outflow,_tmp778)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 561
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple18 _tmp779;return(_tmp779.f1=outflow,((_tmp779.f2=outflow,_tmp779)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 564
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
if(locname != 0){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp77C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp77B;nzval=(void*)((_tmp77B=_region_malloc((env->fenv)->r,sizeof(*_tmp77B)),((_tmp77B[0]=((_tmp77C.tag=8,((_tmp77C.f1=locname,((_tmp77C.f2=nzval,_tmp77C)))))),_tmp77B))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp77F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp77E;zval=(void*)((_tmp77E=_region_malloc((env->fenv)->r,sizeof(*_tmp77E)),((_tmp77E[0]=((_tmp77F.tag=8,((_tmp77F.f1=locname,((_tmp77F.f2=zval,_tmp77F)))))),_tmp77E))));};}
# 570
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple18 _tmp780;return
(_tmp780.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp780.f2=
# 575
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp780)));};}_LL6A:;}_LL65:;}
# 581
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 589
static struct _tuple15 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 593
void*_tmpCB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCC=_tmpCB;void*_tmpCE;union Cyc_Absyn_Constraint*_tmpCF;union Cyc_Absyn_Constraint*_tmpD0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 5)goto _LL72;else{_tmpCE=(_tmpCD->f1).elt_typ;_tmpCF=((_tmpCD->f1).ptr_atts).bounds;_tmpD0=((_tmpCD->f1).ptr_atts).zero_term;}}_LL71: {
# 595
union Cyc_CfFlowInfo_FlowInfo _tmpD1=f;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD3;_LL75: if((_tmpD1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 597
struct _tuple15 _tmp781;return(_tmp781.f1=f,((_tmp781.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp781)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 600
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct Cyc_Absyn_Vardecl*locname=0;
retry: {
void*_tmpD5=r;struct Cyc_Absyn_Vardecl*_tmpD7;void*_tmpD8;struct Cyc_CfFlowInfo_Place*_tmpDC;enum Cyc_CfFlowInfo_InitLevel _tmpDF;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpD6->tag != 8)goto _LL7C;else{_tmpD7=_tmpD6->f1;_tmpD8=(void*)_tmpD6->f2;}}_LL7B:
 r=_tmpD8;locname=_tmpD7;goto retry;_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpD9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpD9->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpDA->tag != 2)goto _LL80;}_LL7F:
# 607
{void*_tmpE0=e->annot;void*_tmpE1=_tmpE0;struct Cyc_List_List*_tmpE3;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE2=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE1;if(_tmpE2->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpE3=_tmpE2->f1;}}_LL8A:
# 609
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpE3))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 613
{void*_tmpE4=e->r;void*_tmpE5=_tmpE4;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpE6->tag != 22)goto _LL90;}_LL8F:
# 615
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp784;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp783;e->annot=(void*)((_tmp783=_cycalloc(sizeof(*_tmp783)),((_tmp783[0]=((_tmp784.tag=Cyc_CfFlowInfo_NotZero,((_tmp784.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp784)))),_tmp783))));}
goto _LL8D;_LL90:;_LL91:
# 618
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 621
goto _LL88;_LL88:;}
# 623
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpDB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpDB->tag != 5)goto _LL82;else{_tmpDC=_tmpDB->f1;}}_LL81:
# 625
{void*_tmpE9=e->annot;void*_tmpEA=_tmpE9;struct Cyc_List_List*_tmpEC;_LL93: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpEB=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpEA;if(_tmpEB->tag != Cyc_CfFlowInfo_NotZero)goto _LL95;else{_tmpEC=_tmpEB->f1;}}_LL94:
# 627
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEC))goto _LL96;
goto _LL92;_LL95:;_LL96:
# 631
{void*_tmpED=e->r;void*_tmpEE=_tmpED;_LL98: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEE;if(_tmpEF->tag != 22)goto _LL9A;}_LL99:
# 633
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp787;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp786;e->annot=(void*)((_tmp786=_cycalloc(sizeof(*_tmp786)),((_tmp786[0]=((_tmp787.tag=Cyc_CfFlowInfo_NotZero,((_tmp787.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp787)))),_tmp786))));}
goto _LL97;_LL9A:;_LL9B:
# 636
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 639
goto _LL92;_LL92:;}{
# 641
struct _tuple15 _tmp788;return(_tmp788.f1=f,((_tmp788.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpDC),_tmp788)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpDD->tag != 0)goto _LL84;}_LL83:
# 643
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp789;return(_tmp789.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp789.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp789)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD5;if(_tmpDE->tag != 3)goto _LL86;else{_tmpDF=_tmpDE->f1;}}_LL85:
# 646
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpDF,locname);goto _LL87;_LL86:;_LL87:
# 648
{void*_tmpF4=e->r;void*_tmpF5=_tmpF4;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpF6->tag != 22)goto _LL9F;}_LL9E:
# 650
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp78C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp78B;e->annot=(void*)((_tmp78B=_cycalloc(sizeof(*_tmp78B)),((_tmp78B[0]=((_tmp78C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp78C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp78C)))),_tmp78B))));}
goto _LL9C;_LL9F:;_LLA0:
# 653
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9C;_LL9C:;}
# 656
goto _LL79;_LL79:;}}else{
# 659
void*_tmpF9=e->annot;void*_tmpFA=_tmpF9;struct Cyc_List_List*_tmpFC;_LLA2: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpFB=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpFA;if(_tmpFB->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA4;else{_tmpFC=_tmpFB->f1;}}_LLA3:
# 661
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpFC))goto _LLA5;
goto _LLA1;_LLA4:;_LLA5:
# 664
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp78F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp78E;e->annot=(void*)((_tmp78E=_cycalloc(sizeof(*_tmp78E)),((_tmp78E[0]=((_tmp78F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp78F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp78F)))),_tmp78E))));}
goto _LLA1;_LLA1:;}{
# 668
enum Cyc_CfFlowInfo_InitLevel _tmpFF=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmpFF){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 670
{void*_tmp100=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp101=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp100;if(_tmp101->tag != 7)goto _LLAB;}_LLAA:
# 672
{const char*_tmp792;void*_tmp791;(_tmp791=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp792="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp792,sizeof(char),61))),_tag_dyneither(_tmp791,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 675
const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp795="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp795,sizeof(char),46))),_tag_dyneither(_tmp794,sizeof(void*),0)));}_LLA8:;}
# 677
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 679
struct _tuple15 _tmp796;return(_tmp796.f1=f,((_tmp796.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp796)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 681
struct _tuple15 _tmp797;return(_tmp797.f1=f,((_tmp797.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp797)));}}};_LL74:;}_LL72:;_LL73: {
# 684
struct Cyc_Core_Impossible_exn_struct _tmp79D;const char*_tmp79C;struct Cyc_Core_Impossible_exn_struct*_tmp79B;(int)_throw((void*)((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B[0]=((_tmp79D.tag=Cyc_Core_Impossible,((_tmp79D.f1=((_tmp79C="right deref of non-pointer-type",_tag_dyneither(_tmp79C,sizeof(char),32))),_tmp79D)))),_tmp79B)))));}_LL6F:;}
# 691
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 694
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 697
{void*_tmp10B=e1->r;void*_tmp10C=_tmp10B;void*_tmp10E;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10D->tag != 1)goto _LLB2;else{_tmp10E=(void*)_tmp10D->f2;}}_LLB1: {
# 699
struct Cyc_Absyn_Vardecl*_tmp10F=Cyc_Tcutil_nonesc_vardecl(_tmp10E);
if(_tmp10F != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp10F);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 705
goto _LLAF;}_LLB2:;_LLB3:
 goto _LLAF;_LLAF:;}{
# 709
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 715
return relns;};}
# 723
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 728
struct _tuple17 _tmp110=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp112;struct _tuple17 _tmp111=_tmp110;_tmp112=_tmp111.f2;
{struct _tuple17 _tmp79E;struct _tuple17 _tmp113=(_tmp79E.f1=inflow,((_tmp79E.f2=_tmp112,_tmp79E)));struct _tuple17 _tmp114=_tmp113;struct Cyc_Dict_Dict _tmp115;struct Cyc_List_List*_tmp116;struct Cyc_CfFlowInfo_Place*_tmp117;_LLB5: if(((_tmp114.f1).ReachableFL).tag != 2)goto _LLB7;_tmp115=((struct _tuple14)((_tmp114.f1).ReachableFL).val).f1;_tmp116=((struct _tuple14)((_tmp114.f1).ReachableFL).val).f2;if(((_tmp114.f2).PlaceL).tag != 1)goto _LLB7;_tmp117=(struct Cyc_CfFlowInfo_Place*)((_tmp114.f2).PlaceL).val;_LLB6: {
# 731
void*_tmp118=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 733
{void*_tmp119=old_rval;struct Cyc_Absyn_Vardecl*_tmp11B;_LLBC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp119;if(_tmp11A->tag != 8)goto _LLBE;else{_tmp11B=_tmp11A->f1;}}_LLBD:
# 735
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7A1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7A0;_tmp118=(void*)((_tmp7A0=_region_malloc((env->fenv)->r,sizeof(*_tmp7A0)),((_tmp7A0[0]=((_tmp7A1.tag=8,((_tmp7A1.f1=_tmp11B,((_tmp7A1.f2=_tmp118,_tmp7A1)))))),_tmp7A0))));}
goto _LLBB;_LLBE:;_LLBF:
 goto _LLBB;_LLBB:;}
# 741
_tmp115=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp115,env->all_changed,_tmp117,_tmp118);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp115,_tmp116);
# 746
goto _LLB4;}_LLB7: if(((_tmp114.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 749
{const char*_tmp7A4;void*_tmp7A3;(_tmp7A3=0,Cyc_CfFlowInfo_aerr(loc,((_tmp7A4="noconsume() parameters must be l-values",_tag_dyneither(_tmp7A4,sizeof(char),40))),_tag_dyneither(_tmp7A3,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 752
return inflow;}
# 757
static struct _tuple15 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 765
union Cyc_CfFlowInfo_FlowInfo _tmp121=outflow;struct Cyc_Dict_Dict _tmp122;struct Cyc_List_List*_tmp123;_LLC1: if((_tmp121.BottomFL).tag != 1)goto _LLC3;_LLC2: {
# 768
struct _tuple15 _tmp7A5;return(_tmp7A5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A5.f2=rval,_tmp7A5)));}_LLC3: if((_tmp121.ReachableFL).tag != 2)goto _LLC0;_tmp122=((struct _tuple14)(_tmp121.ReachableFL).val).f1;_tmp123=((struct _tuple14)(_tmp121.ReachableFL).val).f2;_LLC4: {
# 770
union Cyc_CfFlowInfo_AbsLVal _tmp125=lval;struct Cyc_CfFlowInfo_Place*_tmp126;_LLC6: if((_tmp125.PlaceL).tag != 1)goto _LLC8;_tmp126=(struct Cyc_CfFlowInfo_Place*)(_tmp125.PlaceL).val;_LLC7: {
# 772
struct Cyc_Dict_Dict _tmp127=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp122,env->all_changed,_tmp126,rval);
_tmp123=Cyc_Relations_reln_assign_exp(fenv->r,_tmp123,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp127,_tmp123);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 778
void*_tmp128=Cyc_CfFlowInfo_lookup_place(_tmp122,_tmp126);void*_tmp129=_tmp128;_LLCB: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp12A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12A->tag != 3)goto _LLCD;else{if(_tmp12A->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCD;}}_LLCC:
 goto _LLCE;_LLCD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12B->tag != 7)goto _LLCF;}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp12C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12C->tag != 0)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12D->tag != 8)goto _LLD3;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp12E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp12D->f2);if(_tmp12E->tag != 3)goto _LLD3;else{if(_tmp12E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLD3;}}}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp12F->tag != 8)goto _LLD5;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp130=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp12F->f2);if(_tmp130->tag != 7)goto _LLD5;}}_LLD4:
 goto _LLD6;_LLD5: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp129;if(_tmp131->tag != 8)goto _LLD7;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp132=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp131->f2);if(_tmp132->tag != 0)goto _LLD7;}}_LLD6:
 goto _LLCA;_LLD7:;_LLD8:
# 786
{const char*_tmp7A8;void*_tmp7A7;(_tmp7A7=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7A8="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp7A8,sizeof(char),43))),_tag_dyneither(_tmp7A7,sizeof(void*),0)));}
goto _LLCA;_LLCA:;}
# 791
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 793
struct _tuple15 _tmp7A9;return(_tmp7A9.f1=outflow,((_tmp7A9.f2=rval,_tmp7A9)));};}_LLC8: if((_tmp125.UnknownL).tag != 2)goto _LLC5;_LLC9: {
# 796
struct _tuple15 _tmp7AA;return(_tmp7AA.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp7AA.f2=rval,_tmp7AA)));}_LLC5:;}_LLC0:;}
# 803
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 809
union Cyc_CfFlowInfo_FlowInfo _tmp137=f;struct Cyc_Dict_Dict _tmp138;struct Cyc_List_List*_tmp139;_LLDA: if((_tmp137.BottomFL).tag != 1)goto _LLDC;_LLDB:
 return Cyc_CfFlowInfo_BottomFL();_LLDC: if((_tmp137.ReachableFL).tag != 2)goto _LLD9;_tmp138=((struct _tuple14)(_tmp137.ReachableFL).val).f1;_tmp139=((struct _tuple14)(_tmp137.ReachableFL).val).f2;_LLDD:
# 812
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7B0;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7AF;struct Cyc_CfFlowInfo_Place*_tmp7AE;_tmp138=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp138,env->all_changed,(
(_tmp7AE=_region_malloc(env->r,sizeof(*_tmp7AE)),((_tmp7AE->root=(void*)((_tmp7B0=_region_malloc(env->r,sizeof(*_tmp7B0)),((_tmp7B0[0]=((_tmp7AF.tag=0,((_tmp7AF.f1=vd,_tmp7AF)))),_tmp7B0)))),((_tmp7AE->fields=0,_tmp7AE)))))),rval);}
# 815
_tmp139=Cyc_Relations_reln_assign_var(env->r,_tmp139,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp13D=Cyc_CfFlowInfo_ReachableFL(_tmp138,_tmp139);
Cyc_NewControlFlow_update_tryflow(env,_tmp13D);
# 822
return _tmp13D;};_LLD9:;}struct _tuple23{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 826
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 833
if(vds == 0)return inflow;{
# 836
struct Cyc_List_List*_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple23*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp7B1;es=((_tmp7B1=_cycalloc(sizeof(*_tmp7B1)),((_tmp7B1->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple23*)x->hd)).f2),((_tmp7B1->tl=es,_tmp7B1))))));}}}
# 842
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp13E,fenv->unknown_all,pat_loc,name_locs);
# 844
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 848
struct _tuple15 _tmp140=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp142;void*_tmp143;struct _tuple15 _tmp141=_tmp140;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp142,_tmp143);}}{
# 856
struct Cyc_List_List*_tmp144=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp144 != 0;_tmp144=_tmp144->tl){
struct _tuple23*_tmp145=(struct _tuple23*)_tmp144->hd;struct Cyc_Absyn_Vardecl**_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct _tuple23*_tmp146=_tmp145;_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;
if(_tmp147 != 0  && _tmp148 != 0){
# 863
if(_tmp148->topt == 0){
const char*_tmp7B5;void*_tmp7B4[1];struct Cyc_String_pa_PrintArg_struct _tmp7B3;(_tmp7B3.tag=0,((_tmp7B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp148)),((_tmp7B4[0]=& _tmp7B3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B5="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp7B5,sizeof(char),41))),_tag_dyneither(_tmp7B4,sizeof(void*),1)))))));}
# 868
Cyc_fflush(Cyc_stderr);{
# 870
struct Cyc_List_List _tmp7B6;struct Cyc_List_List l=(_tmp7B6.hd=_tmp148,((_tmp7B6.tl=0,_tmp7B6)));
union Cyc_CfFlowInfo_FlowInfo _tmp14C=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple15 _tmp14D=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp14C,_tmp148);union Cyc_CfFlowInfo_FlowInfo _tmp14F;void*_tmp150;struct _tuple15 _tmp14E=_tmp14D;_tmp14F=_tmp14E.f1;_tmp150=_tmp14E.f2;
if(name_locs){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B8;_tmp150=(void*)((_tmp7B8=_region_malloc(fenv->r,sizeof(*_tmp7B8)),((_tmp7B8[0]=((_tmp7B9.tag=8,((_tmp7B9.f1=*_tmp147,((_tmp7B9.f2=_tmp150,_tmp7B9)))))),_tmp7B8))));}
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp14F,*_tmp147,_tmp148,_tmp150,pat_loc);};}}
# 879
return inflow;};};}
# 882
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 884
void*_tmp154=e->r;void*_tmp155=_tmp154;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp163;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp156->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp157=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp156->f2);if(_tmp157->tag != 4)goto _LLE1;}}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp158->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp159=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp158->f2);if(_tmp159->tag != 3)goto _LLE3;}}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15A->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp15B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp15A->f2);if(_tmp15B->tag != 5)goto _LLE5;}}_LLE4:
 return 1;_LLE5: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15C->tag != 19)goto _LLE7;else{_tmp15D=_tmp15C->f1;}}_LLE6:
 _tmp15F=_tmp15D;goto _LLE8;_LLE7: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp15E->tag != 20)goto _LLE9;else{_tmp15F=_tmp15E->f1;}}_LLE8:
 _tmp161=_tmp15F;goto _LLEA;_LLE9: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp160->tag != 21)goto _LLEB;else{_tmp161=_tmp160->f1;}}_LLEA:
# 891
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp161);_LLEB: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp162->tag != 22)goto _LLED;else{_tmp163=_tmp162->f1;}}_LLEC: {
# 893
void*_tmp164=Cyc_Tcutil_compress((void*)_check_null(_tmp163->topt));void*_tmp165=_tmp164;_LLF0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp165;if(_tmp166->tag != 10)goto _LLF2;}_LLF1:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp163);_LLF2:;_LLF3:
 return 0;_LLEF:;}_LLED:;_LLEE:
# 898
 return 0;_LLDE:;}struct _tuple24{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp86C(unsigned int*_tmp86B,unsigned int*_tmp86A,void***_tmp868,struct Cyc_List_List**_tmp372){for(*_tmp86B=0;*_tmp86B < *_tmp86A;(*_tmp86B)++){
# 1481 "new_control_flow.cyc"
void*_tmp866;(*_tmp868)[*_tmp86B]=(void*)((_tmp866=(void*)((struct Cyc_List_List*)_check_null(*_tmp372))->hd,((*_tmp372=(*_tmp372)->tl,_tmp866))));}}
# 903 "new_control_flow.cyc"
static struct _tuple15 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 907
struct Cyc_CfFlowInfo_FlowEnv*_tmp167=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 920
{union Cyc_CfFlowInfo_FlowInfo _tmp168=inflow;struct Cyc_Dict_Dict _tmp169;struct Cyc_List_List*_tmp16A;_LLF5: if((_tmp168.BottomFL).tag != 1)goto _LLF7;_LLF6: {
struct _tuple15 _tmp7BA;return(_tmp7BA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7BA.f2=_tmp167->unknown_all,_tmp7BA)));}_LLF7: if((_tmp168.ReachableFL).tag != 2)goto _LLF4;_tmp169=((struct _tuple14)(_tmp168.ReachableFL).val).f1;_tmp16A=((struct _tuple14)(_tmp168.ReachableFL).val).f2;_LLF8:
 d=_tmp169;relns=_tmp16A;_LLF4:;}
# 935 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 956 "new_control_flow.cyc"
struct _tuple17 _tmp16C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp16E;union Cyc_CfFlowInfo_AbsLVal _tmp16F;struct _tuple17 _tmp16D=_tmp16C;_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;{
struct _tuple17 _tmp7BB;struct _tuple17 _tmp170=(_tmp7BB.f1=_tmp16E,((_tmp7BB.f2=_tmp16F,_tmp7BB)));struct _tuple17 _tmp171=_tmp170;struct Cyc_Dict_Dict _tmp172;struct Cyc_List_List*_tmp173;struct Cyc_CfFlowInfo_Place*_tmp174;_LLFA: if(((_tmp171.f1).ReachableFL).tag != 2)goto _LLFC;_tmp172=((struct _tuple14)((_tmp171.f1).ReachableFL).val).f1;_tmp173=((struct _tuple14)((_tmp171.f1).ReachableFL).val).f2;if(((_tmp171.f2).PlaceL).tag != 1)goto _LLFC;_tmp174=(struct Cyc_CfFlowInfo_Place*)((_tmp171.f2).PlaceL).val;_LLFB: {
# 959
void*_tmp175=Cyc_CfFlowInfo_lookup_place(_tmp172,_tmp174);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp175,& needs_unconsume)){
{const char*_tmp7BE;void*_tmp7BD;(_tmp7BD=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7BE="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp7BE,sizeof(char),71))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));}{
struct _tuple15 _tmp7BF;return(_tmp7BF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7BF.f2=_tmp167->unknown_all,_tmp7BF)));};}else{
# 965
if(needs_unconsume){
# 967
struct _tuple15 _tmp7C0;return(_tmp7C0.f1=_tmp16E,((_tmp7C0.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp167,_tmp175),_tmp7C0)));}else{
# 970
void*_tmp17A=Cyc_CfFlowInfo_make_unique_consumed(_tmp167,(void*)_check_null(e->topt),e,env->iteration_num,_tmp175);
struct Cyc_Dict_Dict _tmp17B=Cyc_CfFlowInfo_assign_place(_tmp167,e->loc,_tmp172,env->all_changed,_tmp174,_tmp17A);
# 981
struct _tuple15 _tmp7C1;return(_tmp7C1.f1=Cyc_CfFlowInfo_ReachableFL(_tmp17B,_tmp173),((_tmp7C1.f2=_tmp175,_tmp7C1)));}}}_LLFC:;_LLFD:
# 983
 goto _LLF9;_LLF9:;};}{
# 986
void*_tmp17E=e->r;void*_tmp17F=_tmp17E;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Vardecl*_tmp198;struct Cyc_Absyn_Vardecl*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19E;enum Cyc_Absyn_Primop _tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_List_List*_tmp1B1;int _tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;void**_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;int _tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct _dyneither_ptr*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct _dyneither_ptr*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct _dyneither_ptr*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Datatypedecl*_tmp1DA;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1E0;enum Cyc_Absyn_AggrKind _tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;int _tmp1EA;struct Cyc_Absyn_Exp*_tmp1EC;void*_tmp1ED;int _tmp1EE;struct Cyc_Absyn_Stmt*_tmp1F0;_LLFF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp180->tag != 13)goto _LL101;else{_tmp181=_tmp180->f2;if(_tmp180->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL101;}}_LL100: {
# 990
struct _tuple15 _tmp1F7=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp181);union Cyc_CfFlowInfo_FlowInfo _tmp1F9;void*_tmp1FA;struct _tuple15 _tmp1F8=_tmp1F7;_tmp1F9=_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;{
struct _tuple15 _tmp1FB=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1F9,_tmp181,_tmp1FA);union Cyc_CfFlowInfo_FlowInfo _tmp1FD;void*_tmp1FE;struct _tuple15 _tmp1FC=_tmp1FB;_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;{
struct _tuple15 _tmp7C2;return(_tmp7C2.f1=_tmp1FD,((_tmp7C2.f2=_tmp1FA,_tmp7C2)));};};}_LL101: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp182->tag != 13)goto _LL103;else{_tmp183=_tmp182->f2;}}_LL102:
# 996
 _tmp185=_tmp183;goto _LL104;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp184->tag != 11)goto _LL105;else{_tmp185=_tmp184->f1;}}_LL104:
 _tmp187=_tmp185;goto _LL106;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp186->tag != 12)goto _LL107;else{_tmp187=_tmp186->f1;}}_LL106:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp187);_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp188->tag != 0)goto _LL109;else{if(((_tmp188->f1).Null_c).tag != 1)goto _LL109;}}_LL108:
# 1000
 goto _LL10A;_LL109: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp189->tag != 0)goto _LL10B;else{if(((_tmp189->f1).Int_c).tag != 5)goto _LL10B;if(((struct _tuple7)((_tmp189->f1).Int_c).val).f2 != 0)goto _LL10B;}}_LL10A: {
struct _tuple15 _tmp7C3;return(_tmp7C3.f1=inflow,((_tmp7C3.f2=_tmp167->zero,_tmp7C3)));}_LL10B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18A->tag != 0)goto _LL10D;else{if(((_tmp18A->f1).Int_c).tag != 5)goto _LL10D;}}_LL10C:
 goto _LL10E;_LL10D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18B->tag != 1)goto _LL10F;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp18C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp18B->f2);if(_tmp18C->tag != 2)goto _LL10F;}}_LL10E: {
struct _tuple15 _tmp7C4;return(_tmp7C4.f1=inflow,((_tmp7C4.f2=_tmp167->notzeroall,_tmp7C4)));}_LL10F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18D->tag != 30)goto _LL111;else{if(_tmp18D->f1 != 0)goto _LL111;}}_LL110:
# 1005
 goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18E->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18F->tag != 17)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp190->tag != 16)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp191->tag != 18)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp192->tag != 32)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp193->tag != 31)goto _LL11D;}_LL11C: {
struct _tuple15 _tmp7C5;return(_tmp7C5.f1=inflow,((_tmp7C5.f2=_tmp167->unknown_all,_tmp7C5)));}_LL11D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp194->tag != 1)goto _LL11F;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp195=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp194->f2);if(_tmp195->tag != 1)goto _LL11F;}}_LL11E: {
# 1014
struct _tuple15 _tmp7C6;return(_tmp7C6.f1=inflow,((_tmp7C6.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp7C6)));}_LL11F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp196->tag != 1)goto _LL121;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp197=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp196->f2);if(_tmp197->tag != 3)goto _LL121;else{_tmp198=_tmp197->f1;}}}_LL120:
# 1017
 _tmp19B=_tmp198;goto _LL122;_LL121: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp199->tag != 1)goto _LL123;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp19A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp199->f2);if(_tmp19A->tag != 4)goto _LL123;else{_tmp19B=_tmp19A->f1;}}}_LL122:
 _tmp19E=_tmp19B;goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp19C->tag != 1)goto _LL125;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp19C->f2);if(_tmp19D->tag != 5)goto _LL125;else{_tmp19E=_tmp19D->f1;}}}_LL124: {
# 1021
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7C7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp7C7.tag=0,((_tmp7C7.f1=_tmp19E,_tmp7C7)));
struct _tuple15 _tmp7C8;return(_tmp7C8.f1=inflow,((_tmp7C8.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp7C8)));}_LL125: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp19F->tag != 2)goto _LL127;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL126: {
# 1028
struct _tuple15 _tmp206=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1A1);union Cyc_CfFlowInfo_FlowInfo _tmp208;void*_tmp209;struct _tuple15 _tmp207=_tmp206;_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;{
struct _tuple15 _tmp7C9;return(_tmp7C9.f1=_tmp208,((_tmp7C9.f2=_tmp209,_tmp7C9)));};}_LL127: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1A2->tag != 4)goto _LL129;else{_tmp1A3=_tmp1A2->f1;}}_LL128: {
# 1032
struct Cyc_List_List _tmp7CA;struct Cyc_List_List _tmp20B=(_tmp7CA.hd=_tmp1A3,((_tmp7CA.tl=0,_tmp7CA)));
struct _tuple15 _tmp20C=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp20B);union Cyc_CfFlowInfo_FlowInfo _tmp20E;struct _tuple15 _tmp20D=_tmp20C;_tmp20E=_tmp20D.f1;{
struct _tuple17 _tmp20F=Cyc_NewControlFlow_anal_Lexp(env,_tmp20E,0,0,_tmp1A3);union Cyc_CfFlowInfo_AbsLVal _tmp211;struct _tuple17 _tmp210=_tmp20F;_tmp211=_tmp210.f2;
{struct _tuple24 _tmp7CB;struct _tuple24 _tmp212=(_tmp7CB.f1=_tmp211,((_tmp7CB.f2=_tmp20E,_tmp7CB)));struct _tuple24 _tmp213=_tmp212;struct Cyc_CfFlowInfo_Place*_tmp214;struct Cyc_Dict_Dict _tmp215;struct Cyc_List_List*_tmp216;_LL168: if(((_tmp213.f1).PlaceL).tag != 1)goto _LL16A;_tmp214=(struct Cyc_CfFlowInfo_Place*)((_tmp213.f1).PlaceL).val;if(((_tmp213.f2).ReachableFL).tag != 2)goto _LL16A;_tmp215=((struct _tuple14)((_tmp213.f2).ReachableFL).val).f1;_tmp216=((struct _tuple14)((_tmp213.f2).ReachableFL).val).f2;_LL169:
# 1037
 _tmp216=Cyc_Relations_reln_kill_exp(_tmp167->r,_tmp216,_tmp1A3);
_tmp20E=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1A3->loc,_tmp215,env->all_changed,_tmp214,_tmp167->unknown_all),_tmp216);
# 1041
goto _LL167;_LL16A:;_LL16B:
 goto _LL167;_LL167:;}{
# 1044
struct _tuple15 _tmp7CC;return(_tmp7CC.f1=_tmp20E,((_tmp7CC.f2=_tmp167->unknown_all,_tmp7CC)));};};}_LL129: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1A4->tag != 3)goto _LL12B;else{_tmp1A5=_tmp1A4->f1;if(_tmp1A4->f2 == 0)goto _LL12B;_tmp1A6=_tmp1A4->f3;}}_LL12A:
# 1047
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7CF="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7CF,sizeof(char),58))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}{
struct _tuple15 _tmp7D0;return(_tmp7D0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D0.f2=_tmp167->unknown_all,_tmp7D0)));};}{
# 1051
struct Cyc_List_List _tmp7D1;struct Cyc_List_List _tmp21D=(_tmp7D1.hd=_tmp1A6,((_tmp7D1.tl=0,_tmp7D1)));
struct Cyc_List_List _tmp7D2;struct Cyc_List_List _tmp21E=(_tmp7D2.hd=_tmp1A5,((_tmp7D2.tl=& _tmp21D,_tmp7D2)));
struct _tuple15 _tmp21F=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp21E);union Cyc_CfFlowInfo_FlowInfo _tmp221;struct _tuple15 _tmp220=_tmp21F;_tmp221=_tmp220.f1;{
struct _tuple17 _tmp222=Cyc_NewControlFlow_anal_Lexp(env,_tmp221,0,0,_tmp1A5);union Cyc_CfFlowInfo_AbsLVal _tmp224;struct _tuple17 _tmp223=_tmp222;_tmp224=_tmp223.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp225=_tmp221;struct Cyc_Dict_Dict _tmp226;struct Cyc_List_List*_tmp227;_LL16D: if((_tmp225.ReachableFL).tag != 2)goto _LL16F;_tmp226=((struct _tuple14)(_tmp225.ReachableFL).val).f1;_tmp227=((struct _tuple14)(_tmp225.ReachableFL).val).f2;_LL16E:
# 1057
{union Cyc_CfFlowInfo_AbsLVal _tmp228=_tmp224;struct Cyc_CfFlowInfo_Place*_tmp229;_LL172: if((_tmp228.PlaceL).tag != 1)goto _LL174;_tmp229=(struct Cyc_CfFlowInfo_Place*)(_tmp228.PlaceL).val;_LL173:
# 1061
 _tmp227=Cyc_Relations_reln_kill_exp(_tmp167->r,_tmp227,_tmp1A5);
_tmp226=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1A5->loc,_tmp226,env->all_changed,_tmp229,_tmp167->unknown_all);
# 1064
_tmp221=Cyc_CfFlowInfo_ReachableFL(_tmp226,_tmp227);
# 1067
goto _LL171;_LL174: if((_tmp228.UnknownL).tag != 2)goto _LL171;_LL175:
# 1070
 goto _LL171;_LL171:;}
# 1072
goto _LL16C;_LL16F:;_LL170:
 goto _LL16C;_LL16C:;}{
# 1075
struct _tuple15 _tmp7D3;return(_tmp7D3.f1=_tmp221,((_tmp7D3.f2=_tmp167->unknown_all,_tmp7D3)));};};};_LL12B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1A7->tag != 3)goto _LL12D;else{_tmp1A8=_tmp1A7->f1;if(_tmp1A7->f2 != 0)goto _LL12D;_tmp1A9=_tmp1A7->f3;}}_LL12C:
# 1079
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7D6="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7D6,sizeof(char),58))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}{
struct _tuple15 _tmp7D7;return(_tmp7D7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D7.f2=_tmp167->unknown_all,_tmp7D7)));};}{
# 1083
struct Cyc_Dict_Dict*_tmp230=env->all_changed;
# 1085
{struct Cyc_Absyn_Exp*_tmp7D8[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp7D8[1]=_tmp1A9,((_tmp7D8[0]=_tmp1A8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp7D8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp7D9;env->all_changed=((_tmp7D9=_region_malloc(env->r,sizeof(*_tmp7D9)),((_tmp7D9[0]=_tmp167->mt_place_set,_tmp7D9))));}{
struct _tuple17 _tmp233=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1A8);union Cyc_CfFlowInfo_FlowInfo _tmp235;union Cyc_CfFlowInfo_AbsLVal _tmp236;struct _tuple17 _tmp234=_tmp233;_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;{
struct Cyc_Dict_Dict _tmp237=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp7DA;env->all_changed=((_tmp7DA=_region_malloc(env->r,sizeof(*_tmp7DA)),((_tmp7DA[0]=_tmp167->mt_place_set,_tmp7DA))));}{
struct _tuple15 _tmp239=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A9);union Cyc_CfFlowInfo_FlowInfo _tmp23B;void*_tmp23C;struct _tuple15 _tmp23A=_tmp239;_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;{
struct Cyc_Dict_Dict _tmp23D=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp23E=Cyc_CfFlowInfo_after_flow(_tmp167,& _tmp237,_tmp235,_tmp23B,_tmp237,_tmp23D);
# 1096
union Cyc_CfFlowInfo_FlowInfo _tmp23F=Cyc_CfFlowInfo_join_flow(_tmp167,_tmp230,inflow,_tmp23E);
# 1099
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp23F,inflow)){
if(_tmp230 == 0)
env->all_changed=0;else{
# 1103
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp230,
Cyc_CfFlowInfo_union_place_set(_tmp237,_tmp23D,0),0);}
# 1106
return Cyc_NewControlFlow_do_assign(_tmp167,env,_tmp23E,_tmp1A8,_tmp236,_tmp1A9,_tmp23C,e->loc);}
# 1109
inflow=_tmp23F;};};};};}};_LL12D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1AA->tag != 8)goto _LL12F;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}_LL12E: {
# 1114
struct _tuple15 _tmp240=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1AB);union Cyc_CfFlowInfo_FlowInfo _tmp242;void*_tmp243;struct _tuple15 _tmp241=_tmp240;_tmp242=_tmp241.f1;_tmp243=_tmp241.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp242,_tmp1AC);}_LL12F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1AD->tag != 10)goto _LL131;else{_tmp1AE=_tmp1AD->f1;}}_LL130: {
# 1118
struct _tuple15 _tmp244=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1AE);union Cyc_CfFlowInfo_FlowInfo _tmp246;void*_tmp247;struct _tuple15 _tmp245=_tmp244;_tmp246=_tmp245.f1;_tmp247=_tmp245.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1AE->loc,_tmp246,_tmp247);{
struct _tuple15 _tmp7DB;return(_tmp7DB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DB.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp7DB)));};}_LL131: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1AF->tag != 9)goto _LL133;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;}}_LL132: {
# 1123
struct _RegionHandle _tmp249=_new_region("temp");struct _RegionHandle*temp=& _tmp249;_push_region(temp);
{struct Cyc_List_List*_tmp7DC;struct _tuple22 _tmp24A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp7DC=_region_malloc(temp,sizeof(*_tmp7DC)),((_tmp7DC->hd=_tmp1B0,((_tmp7DC->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1B1),_tmp7DC)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp24C;struct Cyc_List_List*_tmp24D;struct _tuple22 _tmp24B=_tmp24A;_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;{
# 1126
union Cyc_CfFlowInfo_FlowInfo _tmp24E=Cyc_NewControlFlow_use_Rval(env,_tmp1B0->loc,_tmp24C,(void*)((struct Cyc_List_List*)_check_null(_tmp24D))->hd);
_tmp24D=_tmp24D->tl;{
# 1129
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp24F=Cyc_Tcutil_compress((void*)_check_null(_tmp1B0->topt));void*_tmp250=_tmp24F;void*_tmp252;_LL177: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp251=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp250;if(_tmp251->tag != 5)goto _LL179;else{_tmp252=(_tmp251->f1).elt_typ;}}_LL178:
# 1134
{void*_tmp253=Cyc_Tcutil_compress(_tmp252);void*_tmp254=_tmp253;struct Cyc_List_List*_tmp256;_LL17C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254;if(_tmp255->tag != 9)goto _LL17E;else{_tmp256=(_tmp255->f1).attributes;}}_LL17D:
# 1136
 for(0;_tmp256 != 0;_tmp256=_tmp256->tl){
# 1138
void*_tmp257=(void*)_tmp256->hd;void*_tmp258=_tmp257;int _tmp25A;int _tmp25C;int _tmp25E;_LL181: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp259=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp258;if(_tmp259->tag != 20)goto _LL183;else{_tmp25A=_tmp259->f1;}}_LL182:
# 1140
{struct Cyc_List_List*_tmp7DD;init_params=((_tmp7DD=_region_malloc(temp,sizeof(*_tmp7DD)),((_tmp7DD->hd=(void*)_tmp25A,((_tmp7DD->tl=init_params,_tmp7DD))))));}goto _LL180;_LL183: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp25B=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp258;if(_tmp25B->tag != 21)goto _LL185;else{_tmp25C=_tmp25B->f1;}}_LL184:
# 1142
{struct Cyc_List_List*_tmp7DE;nolive_unique_params=((_tmp7DE=_region_malloc(temp,sizeof(*_tmp7DE)),((_tmp7DE->hd=(void*)_tmp25C,((_tmp7DE->tl=nolive_unique_params,_tmp7DE))))));}
goto _LL180;_LL185: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp25D=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp258;if(_tmp25D->tag != 22)goto _LL187;else{_tmp25E=_tmp25D->f1;}}_LL186:
# 1146
{struct Cyc_List_List*_tmp7DF;noconsume_params=((_tmp7DF=_region_malloc(temp,sizeof(*_tmp7DF)),((_tmp7DF->hd=(void*)_tmp25E,((_tmp7DF->tl=noconsume_params,_tmp7DF))))));}
goto _LL180;_LL187:;_LL188:
 goto _LL180;_LL180:;}
# 1150
goto _LL17B;_LL17E:;_LL17F: {
const char*_tmp7E2;void*_tmp7E1;(_tmp7E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E2="anal_Rexp: bad function type",_tag_dyneither(_tmp7E2,sizeof(char),29))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}_LL17B:;}
# 1153
goto _LL176;_LL179:;_LL17A: {
const char*_tmp7E5;void*_tmp7E4;(_tmp7E4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E5="anal_Rexp: bad function type",_tag_dyneither(_tmp7E5,sizeof(char),29))),_tag_dyneither(_tmp7E4,sizeof(void*),0)));}_LL176:;}
# 1156
{int i=1;for(0;_tmp24D != 0;(((_tmp24D=_tmp24D->tl,_tmp1B1=((struct Cyc_List_List*)_check_null(_tmp1B1))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp266=_tmp24C;struct Cyc_Dict_Dict _tmp267;_LL18A: if((_tmp266.BottomFL).tag != 1)goto _LL18C;_LL18B:
 goto _LL189;_LL18C: if((_tmp266.ReachableFL).tag != 2)goto _LL189;_tmp267=((struct _tuple14)(_tmp266.ReachableFL).val).f1;_LL18D:
# 1161
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp267,(void*)_tmp24D->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7E8;void*_tmp7E7;(_tmp7E7=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->loc,((_tmp7E8="expression may not be initialized",_tag_dyneither(_tmp7E8,sizeof(char),34))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp26A=_tmp24E;struct Cyc_Dict_Dict _tmp26B;struct Cyc_List_List*_tmp26C;_LL18F: if((_tmp26A.BottomFL).tag != 1)goto _LL191;_LL190:
 goto _LL18E;_LL191: if((_tmp26A.ReachableFL).tag != 2)goto _LL18E;_tmp26B=((struct _tuple14)(_tmp26A.ReachableFL).val).f1;_tmp26C=((struct _tuple14)(_tmp26A.ReachableFL).val).f2;_LL192: {
# 1168
struct Cyc_Dict_Dict _tmp26D=Cyc_CfFlowInfo_escape_deref(_tmp167,_tmp26B,env->all_changed,(void*)_tmp24D->hd);
{void*_tmp26E=(void*)_tmp24D->hd;void*_tmp26F=_tmp26E;struct Cyc_CfFlowInfo_Place*_tmp271;_LL194: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp270=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp26F;if(_tmp270->tag != 5)goto _LL196;else{_tmp271=_tmp270->f1;}}_LL195:
# 1171
{void*_tmp272=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->topt));void*_tmp273=_tmp272;void*_tmp275;_LL199: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp274=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp273;if(_tmp274->tag != 5)goto _LL19B;else{_tmp275=(_tmp274->f1).elt_typ;}}_LL19A:
# 1175
 _tmp26D=Cyc_CfFlowInfo_assign_place(_tmp167,((struct Cyc_Absyn_Exp*)_tmp1B1->hd)->loc,_tmp26D,env->all_changed,_tmp271,
# 1177
Cyc_CfFlowInfo_typ_to_absrval(_tmp167,_tmp275,0,_tmp167->esc_all));
# 1179
goto _LL198;_LL19B:;_LL19C: {
const char*_tmp7EB;void*_tmp7EA;(_tmp7EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7EB="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp7EB,sizeof(char),39))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}_LL198:;}
# 1182
goto _LL193;_LL196:;_LL197:
 goto _LL193;_LL193:;}
# 1185
_tmp24E=Cyc_CfFlowInfo_ReachableFL(_tmp26D,_tmp26C);
goto _LL18E;}_LL18E:;}
# 1188
goto _LL189;_LL189:;}else{
# 1191
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1196
_tmp24E=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->topt),_tmp24E,(void*)_tmp24D->hd);else{
# 1203
_tmp24E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd)->loc,_tmp24E,(void*)_tmp24D->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1B1->hd)->topt),0))
_tmp24E=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp24E,(struct Cyc_Absyn_Exp*)_tmp1B1->hd,((struct Cyc_Absyn_Exp*)_tmp1B1->hd)->loc,(void*)_tmp24D->hd);}}}}
# 1210
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1B0->topt))){
struct _tuple15 _tmp7EC;struct _tuple15 _tmp279=(_tmp7EC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp7EC)));_npop_handler(0);return _tmp279;}else{
# 1213
struct _tuple15 _tmp7ED;struct _tuple15 _tmp27B=(_tmp7ED.f1=_tmp24E,((_tmp7ED.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp7ED)));_npop_handler(0);return _tmp27B;}};};}
# 1124
;_pop_region(temp);}_LL133: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1B2->tag != 33)goto _LL135;else{_tmp1B3=(_tmp1B2->f1).is_calloc;_tmp1B4=(_tmp1B2->f1).rgn;_tmp1B5=(_tmp1B2->f1).elt_type;_tmp1B6=(_tmp1B2->f1).num_elts;_tmp1B7=(_tmp1B2->f1).fat_result;}}_LL134: {
# 1216
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7F0;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7EF;void*root=(void*)((_tmp7EF=_region_malloc(_tmp167->r,sizeof(*_tmp7EF)),((_tmp7EF[0]=((_tmp7F0.tag=1,((_tmp7F0.f1=_tmp1B6,((_tmp7F0.f2=(void*)_check_null(e->topt),_tmp7F0)))))),_tmp7EF))));
struct Cyc_CfFlowInfo_Place*_tmp7F1;struct Cyc_CfFlowInfo_Place*place=(_tmp7F1=_region_malloc(_tmp167->r,sizeof(*_tmp7F1)),((_tmp7F1->root=root,((_tmp7F1->fields=0,_tmp7F1)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7F4;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7F3;void*rval=(void*)((_tmp7F3=_region_malloc(_tmp167->r,sizeof(*_tmp7F3)),((_tmp7F3[0]=((_tmp7F4.tag=5,((_tmp7F4.f1=place,_tmp7F4)))),_tmp7F3))));
void*place_val;
if(_tmp1B7)place_val=_tmp167->notzeroall;else{
if(_tmp1B3)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,*((void**)_check_null(_tmp1B5)),0,_tmp167->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,*((void**)_check_null(_tmp1B5)),0,_tmp167->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1B4 != 0){
struct _RegionHandle _tmp27D=_new_region("temp");struct _RegionHandle*temp=& _tmp27D;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7F5[2];struct _tuple22 _tmp27E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7F5[1]=_tmp1B6,((_tmp7F5[0]=_tmp1B4,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7F5,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1227
union Cyc_CfFlowInfo_FlowInfo _tmp280;struct Cyc_List_List*_tmp281;struct _tuple22 _tmp27F=_tmp27E;_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;
# 1229
outflow=_tmp280;}
# 1227
;_pop_region(temp);}else{
# 1232
struct _tuple15 _tmp283=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1B6);union Cyc_CfFlowInfo_FlowInfo _tmp285;struct _tuple15 _tmp284=_tmp283;_tmp285=_tmp284.f1;
outflow=_tmp285;}{
# 1235
union Cyc_CfFlowInfo_FlowInfo _tmp286=outflow;struct Cyc_Dict_Dict _tmp287;struct Cyc_List_List*_tmp288;_LL19E: if((_tmp286.BottomFL).tag != 1)goto _LL1A0;_LL19F: {
struct _tuple15 _tmp7F6;return(_tmp7F6.f1=outflow,((_tmp7F6.f2=rval,_tmp7F6)));}_LL1A0: if((_tmp286.ReachableFL).tag != 2)goto _LL19D;_tmp287=((struct _tuple14)(_tmp286.ReachableFL).val).f1;_tmp288=((struct _tuple14)(_tmp286.ReachableFL).val).f2;_LL1A1: {
# 1238
struct _tuple15 _tmp7F7;return(_tmp7F7.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp287,root,place_val),_tmp288),((_tmp7F7.f2=rval,_tmp7F7)));}_LL19D:;};};}_LL135: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1B8->tag != 34)goto _LL137;else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;}}_LL136: {
# 1242
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1248
struct _RegionHandle _tmp290=_new_region("temp");struct _RegionHandle*temp=& _tmp290;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp7F8[2];struct _tuple22 _tmp291=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7F8[1]=_tmp1BA,((_tmp7F8[0]=_tmp1B9,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7F8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1249
union Cyc_CfFlowInfo_FlowInfo _tmp293;struct Cyc_List_List*_tmp294;struct _tuple22 _tmp292=_tmp291;_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;
# 1251
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp294))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp294->tl))->hd;
outflow=_tmp293;}{
# 1256
struct _tuple17 _tmp296=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1B9);union Cyc_CfFlowInfo_AbsLVal _tmp298;struct _tuple17 _tmp297=_tmp296;_tmp298=_tmp297.f2;{
struct _tuple17 _tmp299=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1BA);union Cyc_CfFlowInfo_AbsLVal _tmp29B;struct _tuple17 _tmp29A=_tmp299;_tmp29B=_tmp29A.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp29C=outflow;struct Cyc_Dict_Dict _tmp29D;struct Cyc_List_List*_tmp29E;_LL1A3: if((_tmp29C.ReachableFL).tag != 2)goto _LL1A5;_tmp29D=((struct _tuple14)(_tmp29C.ReachableFL).val).f1;_tmp29E=((struct _tuple14)(_tmp29C.ReachableFL).val).f2;_LL1A4:
# 1260
{union Cyc_CfFlowInfo_AbsLVal _tmp29F=_tmp298;struct Cyc_CfFlowInfo_Place*_tmp2A0;_LL1A8: if((_tmp29F.PlaceL).tag != 1)goto _LL1AA;_tmp2A0=(struct Cyc_CfFlowInfo_Place*)(_tmp29F.PlaceL).val;_LL1A9:
# 1262
 _tmp29D=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1B9->loc,_tmp29D,env->all_changed,_tmp2A0,right_rval);
# 1264
goto _LL1A7;_LL1AA: if((_tmp29F.UnknownL).tag != 2)goto _LL1A7;_LL1AB:
# 1269
 if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp29D,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,Cyc_Tcutil_terr(_tmp1BA->loc,((_tmp7FB="expression may not be fully initialized",_tag_dyneither(_tmp7FB,sizeof(char),40))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}
goto _LL1A7;_LL1A7:;}
# 1273
{union Cyc_CfFlowInfo_AbsLVal _tmp2A3=_tmp29B;struct Cyc_CfFlowInfo_Place*_tmp2A4;_LL1AD: if((_tmp2A3.PlaceL).tag != 1)goto _LL1AF;_tmp2A4=(struct Cyc_CfFlowInfo_Place*)(_tmp2A3.PlaceL).val;_LL1AE:
# 1275
 _tmp29D=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp1BA->loc,_tmp29D,env->all_changed,_tmp2A4,left_rval);
# 1277
goto _LL1AC;_LL1AF: if((_tmp2A3.UnknownL).tag != 2)goto _LL1AC;_LL1B0:
# 1279
 if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp29D,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp7FE;void*_tmp7FD;(_tmp7FD=0,Cyc_Tcutil_terr(_tmp1B9->loc,((_tmp7FE="expression may not be fully initialized",_tag_dyneither(_tmp7FE,sizeof(char),40))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}
goto _LL1AC;_LL1AC:;}
# 1284
_tmp29E=Cyc_Relations_reln_kill_exp(_tmp167->r,_tmp29E,_tmp1B9);
_tmp29E=Cyc_Relations_reln_kill_exp(_tmp167->r,_tmp29E,_tmp1BA);
# 1287
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp29D,_tmp29E);
goto _LL1A2;_LL1A5:;_LL1A6:
 goto _LL1A2;_LL1A2:;}{
# 1293
struct _tuple15 _tmp7FF;struct _tuple15 _tmp2A8=(_tmp7FF.f1=outflow,((_tmp7FF.f2=_tmp167->unknown_all,_tmp7FF)));_npop_handler(0);return _tmp2A8;};};};
# 1248
;_pop_region(temp);}_LL137: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1BB->tag != 15)goto _LL139;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL138: {
# 1296
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp802;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp801;void*root=(void*)((_tmp801=_region_malloc(_tmp167->r,sizeof(*_tmp801)),((_tmp801[0]=((_tmp802.tag=1,((_tmp802.f1=_tmp1BD,((_tmp802.f2=(void*)_check_null(e->topt),_tmp802)))))),_tmp801))));
struct Cyc_CfFlowInfo_Place*_tmp803;struct Cyc_CfFlowInfo_Place*place=(_tmp803=_region_malloc(_tmp167->r,sizeof(*_tmp803)),((_tmp803->root=root,((_tmp803->fields=0,_tmp803)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp806;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp805;void*rval=(void*)((_tmp805=_region_malloc(_tmp167->r,sizeof(*_tmp805)),((_tmp805[0]=((_tmp806.tag=5,((_tmp806.f1=place,_tmp806)))),_tmp805))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1BC != 0){
struct _RegionHandle _tmp2A9=_new_region("temp");struct _RegionHandle*temp=& _tmp2A9;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp807[2];struct _tuple22 _tmp2AA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp807[1]=_tmp1BD,((_tmp807[0]=_tmp1BC,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp807,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1304
union Cyc_CfFlowInfo_FlowInfo _tmp2AC;struct Cyc_List_List*_tmp2AD;struct _tuple22 _tmp2AB=_tmp2AA;_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;
# 1306
outflow=_tmp2AC;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2AD))->tl))->hd;}
# 1304
;_pop_region(temp);}else{
# 1310
struct _tuple15 _tmp2AF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1BD);union Cyc_CfFlowInfo_FlowInfo _tmp2B1;void*_tmp2B2;struct _tuple15 _tmp2B0=_tmp2AF;_tmp2B1=_tmp2B0.f1;_tmp2B2=_tmp2B0.f2;
outflow=_tmp2B1;
place_val=_tmp2B2;}{
# 1314
union Cyc_CfFlowInfo_FlowInfo _tmp2B3=outflow;struct Cyc_Dict_Dict _tmp2B4;struct Cyc_List_List*_tmp2B5;_LL1B2: if((_tmp2B3.BottomFL).tag != 1)goto _LL1B4;_LL1B3: {
struct _tuple15 _tmp808;return(_tmp808.f1=outflow,((_tmp808.f2=rval,_tmp808)));}_LL1B4: if((_tmp2B3.ReachableFL).tag != 2)goto _LL1B1;_tmp2B4=((struct _tuple14)(_tmp2B3.ReachableFL).val).f1;_tmp2B5=((struct _tuple14)(_tmp2B3.ReachableFL).val).f2;_LL1B5: {
# 1317
struct _tuple15 _tmp809;return(_tmp809.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2B4,root,place_val),_tmp2B5),((_tmp809.f2=rval,_tmp809)));}_LL1B1:;};};}_LL139: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1BE->tag != 14)goto _LL13B;else{_tmp1BF=_tmp1BE->f1;}}_LL13A: {
# 1321
struct _tuple17 _tmp2BD=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1BF);union Cyc_CfFlowInfo_FlowInfo _tmp2BF;union Cyc_CfFlowInfo_AbsLVal _tmp2C0;struct _tuple17 _tmp2BE=_tmp2BD;_tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2C1=_tmp2C0;struct Cyc_CfFlowInfo_Place*_tmp2C2;_LL1B7: if((_tmp2C1.UnknownL).tag != 2)goto _LL1B9;_LL1B8: {
struct _tuple15 _tmp80A;return(_tmp80A.f1=_tmp2BF,((_tmp80A.f2=_tmp167->notzeroall,_tmp80A)));}_LL1B9: if((_tmp2C1.PlaceL).tag != 1)goto _LL1B6;_tmp2C2=(struct Cyc_CfFlowInfo_Place*)(_tmp2C1.PlaceL).val;_LL1BA: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp810;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp80F;struct _tuple15 _tmp80E;return(_tmp80E.f1=_tmp2BF,((_tmp80E.f2=(void*)((_tmp810=_region_malloc(env->r,sizeof(*_tmp810)),((_tmp810[0]=((_tmp80F.tag=5,((_tmp80F.f1=_tmp2C2,_tmp80F)))),_tmp810)))),_tmp80E)));}_LL1B6:;};}_LL13B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1C0->tag != 19)goto _LL13D;else{_tmp1C1=_tmp1C0->f1;}}_LL13C: {
# 1328
struct _tuple15 _tmp2C7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple15 _tmp2C8=_tmp2C7;_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C9,_tmp1C1,_tmp2CA);}_LL13D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1C2->tag != 20)goto _LL13F;else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL13E: {
# 1332
struct _tuple15 _tmp2CB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp2CD;void*_tmp2CE;struct _tuple15 _tmp2CC=_tmp2CB;_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;
if(_tmp1C3->topt == 0){
{const char*_tmp814;void*_tmp813[1];struct Cyc_String_pa_PrintArg_struct _tmp812;(_tmp812.tag=0,((_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp813[0]=& _tmp812,Cyc_fprintf(Cyc_stderr,((_tmp814="aggrmember exp %s\n",_tag_dyneither(_tmp814,sizeof(char),19))),_tag_dyneither(_tmp813,sizeof(void*),1)))))));}{
const char*_tmp818;void*_tmp817[1];struct Cyc_String_pa_PrintArg_struct _tmp816;(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1C3)),((_tmp817[0]=& _tmp816,Cyc_fprintf(Cyc_stderr,((_tmp818="oops! %s.topt = null!\n",_tag_dyneither(_tmp818,sizeof(char),23))),_tag_dyneither(_tmp817,sizeof(void*),1)))))));};}{
# 1337
void*_tmp2D5=(void*)_check_null(_tmp1C3->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2D5)){
# 1340
struct _tuple15 _tmp819;return(_tmp819.f1=_tmp2CD,((_tmp819.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp819)));}
# 1342
if(Cyc_Absyn_is_require_union_type(_tmp2D5))
Cyc_NewControlFlow_check_requires(_tmp1C3->loc,_tmp167->r,_tmp2D5,_tmp1C4,_tmp2CD);{
# 1345
void*_tmp2D7=_tmp2CE;int _tmp2DA;int _tmp2DB;struct _dyneither_ptr _tmp2DC;int _tmp2DE;int _tmp2DF;struct _dyneither_ptr _tmp2E0;_LL1BC: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2D8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2D7;if(_tmp2D8->tag != 8)goto _LL1BE;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2D9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2D8->f2);if(_tmp2D9->tag != 6)goto _LL1BE;else{_tmp2DA=(_tmp2D9->f1).is_union;_tmp2DB=(_tmp2D9->f1).fieldnum;_tmp2DC=_tmp2D9->f2;}}}_LL1BD:
# 1347
 _tmp2DE=_tmp2DA;_tmp2DF=_tmp2DB;_tmp2E0=_tmp2DC;goto _LL1BF;_LL1BE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DD=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2D7;if(_tmp2DD->tag != 6)goto _LL1C0;else{_tmp2DE=(_tmp2DD->f1).is_union;_tmp2DF=(_tmp2DD->f1).fieldnum;_tmp2E0=_tmp2DD->f2;}}_LL1BF: {
# 1349
int _tmp2E1=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1C3->topt),_tmp1C4);
if((_tmp2DE  && _tmp2DF != - 1) && _tmp2DF != _tmp2E1){
struct _tuple15 _tmp81A;return(_tmp81A.f1=_tmp2CD,((_tmp81A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),1,_tmp167->unknown_none),_tmp81A)));}{
struct _tuple15 _tmp81B;return(_tmp81B.f1=_tmp2CD,((_tmp81B.f2=*((void**)_check_dyneither_subscript(_tmp2E0,sizeof(void*),_tmp2E1)),_tmp81B)));};}_LL1C0:;_LL1C1:
# 1354
{const char*_tmp81E;void*_tmp81D;(_tmp81D=0,Cyc_fprintf(Cyc_stderr,((_tmp81E="the bad rexp is :",_tag_dyneither(_tmp81E,sizeof(char),18))),_tag_dyneither(_tmp81D,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2CE);
{const char*_tmp821;void*_tmp820;(_tmp820=0,Cyc_fprintf(Cyc_stderr,((_tmp821="\n",_tag_dyneither(_tmp821,sizeof(char),2))),_tag_dyneither(_tmp820,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp82E;const char*_tmp82D;void*_tmp82C[1];struct Cyc_String_pa_PrintArg_struct _tmp82B;struct Cyc_Core_Impossible_exn_struct*_tmp82A;(int)_throw((void*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82E.tag=Cyc_Core_Impossible,((_tmp82E.f1=(struct _dyneither_ptr)((_tmp82B.tag=0,((_tmp82B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp82C[0]=& _tmp82B,Cyc_aprintf(((_tmp82D="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp82D,sizeof(char),26))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))))),_tmp82E)))),_tmp82A)))));};_LL1BB:;};};}_LL13F: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1C5->tag != 37)goto _LL141;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}_LL140: {
# 1364
struct _tuple15 _tmp2ED=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp2EF;void*_tmp2F0;struct _tuple15 _tmp2EE=_tmp2ED;_tmp2EF=_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;
# 1366
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1C6->topt))){
struct _tuple15 _tmp82F;return(_tmp82F.f1=_tmp2EF,((_tmp82F.f2=_tmp167->unknown_all,_tmp82F)));}{
void*_tmp2F2=_tmp2F0;int _tmp2F5;int _tmp2F6;struct _dyneither_ptr _tmp2F7;int _tmp2F9;int _tmp2FA;struct _dyneither_ptr _tmp2FB;_LL1C3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2F3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2F2;if(_tmp2F3->tag != 8)goto _LL1C5;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2F3->f2);if(_tmp2F4->tag != 6)goto _LL1C5;else{_tmp2F5=(_tmp2F4->f1).is_union;_tmp2F6=(_tmp2F4->f1).fieldnum;_tmp2F7=_tmp2F4->f2;}}}_LL1C4:
# 1370
 _tmp2F9=_tmp2F5;_tmp2FA=_tmp2F6;_tmp2FB=_tmp2F7;goto _LL1C6;_LL1C5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F2;if(_tmp2F8->tag != 6)goto _LL1C7;else{_tmp2F9=(_tmp2F8->f1).is_union;_tmp2FA=(_tmp2F8->f1).fieldnum;_tmp2FB=_tmp2F8->f2;}}_LL1C6: {
# 1372
int _tmp2FC=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1C6->topt),_tmp1C7);
if(_tmp2F9  && _tmp2FA != - 1){
if(_tmp2FA != _tmp2FC){
struct _tuple15 _tmp830;return(_tmp830.f1=_tmp2EF,((_tmp830.f2=_tmp167->zero,_tmp830)));}else{
# 1377
struct _tuple15 _tmp831;return(_tmp831.f1=_tmp2EF,((_tmp831.f2=_tmp167->notzeroall,_tmp831)));}}else{
# 1379
struct _tuple15 _tmp832;return(_tmp832.f1=_tmp2EF,((_tmp832.f2=_tmp167->unknown_all,_tmp832)));}}_LL1C7:;_LL1C8: {
# 1381
struct Cyc_Core_Impossible_exn_struct _tmp83F;const char*_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_PrintArg_struct _tmp83C;struct Cyc_Core_Impossible_exn_struct*_tmp83B;(int)_throw((void*)((_tmp83B=_cycalloc(sizeof(*_tmp83B)),((_tmp83B[0]=((_tmp83F.tag=Cyc_Core_Impossible,((_tmp83F.f1=(struct _dyneither_ptr)((_tmp83C.tag=0,((_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp83D[0]=& _tmp83C,Cyc_aprintf(((_tmp83E="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp83E,sizeof(char),26))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))))),_tmp83F)))),_tmp83B)))));}_LL1C2:;};}_LL141: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1C8->tag != 21)goto _LL143;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;}}_LL142: {
# 1386
struct _tuple15 _tmp305=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C9);union Cyc_CfFlowInfo_FlowInfo _tmp307;void*_tmp308;struct _tuple15 _tmp306=_tmp305;_tmp307=_tmp306.f1;_tmp308=_tmp306.f2;{
# 1389
struct _tuple15 _tmp309=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp307,_tmp1C9,_tmp308);union Cyc_CfFlowInfo_FlowInfo _tmp30B;void*_tmp30C;struct _tuple15 _tmp30A=_tmp309;_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;{
# 1392
void*_tmp30D=Cyc_Tcutil_compress((void*)_check_null(_tmp1C9->topt));void*_tmp30E=_tmp30D;void*_tmp310;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30E;if(_tmp30F->tag != 5)goto _LL1CC;else{_tmp310=(_tmp30F->f1).elt_typ;}}_LL1CB:
# 1394
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp310)){
# 1396
struct _tuple15 _tmp840;return(_tmp840.f1=_tmp30B,((_tmp840.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp840)));}
# 1398
if(Cyc_Absyn_is_require_union_type(_tmp310))
Cyc_NewControlFlow_check_requires(_tmp1C9->loc,_tmp167->r,_tmp310,_tmp1CA,_tmp307);{
# 1401
void*_tmp312=_tmp30C;int _tmp315;int _tmp316;struct _dyneither_ptr _tmp317;int _tmp319;int _tmp31A;struct _dyneither_ptr _tmp31B;_LL1CF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp313=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp312;if(_tmp313->tag != 8)goto _LL1D1;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp314=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp313->f2);if(_tmp314->tag != 6)goto _LL1D1;else{_tmp315=(_tmp314->f1).is_union;_tmp316=(_tmp314->f1).fieldnum;_tmp317=_tmp314->f2;}}}_LL1D0:
# 1403
 _tmp319=_tmp315;_tmp31A=_tmp316;_tmp31B=_tmp317;goto _LL1D2;_LL1D1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp318=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp312;if(_tmp318->tag != 6)goto _LL1D3;else{_tmp319=(_tmp318->f1).is_union;_tmp31A=(_tmp318->f1).fieldnum;_tmp31B=_tmp318->f2;}}_LL1D2: {
# 1405
int _tmp31C=Cyc_CfFlowInfo_get_field_index(_tmp310,_tmp1CA);
if((_tmp319  && _tmp31A != - 1) && _tmp31A != _tmp31C){
struct _tuple15 _tmp841;return(_tmp841.f1=_tmp30B,((_tmp841.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),1,_tmp167->unknown_none),_tmp841)));}{
struct _tuple15 _tmp842;return(_tmp842.f1=_tmp30B,((_tmp842.f2=*((void**)_check_dyneither_subscript(_tmp31B,sizeof(void*),_tmp31C)),_tmp842)));};}_LL1D3:;_LL1D4: {
struct Cyc_Core_Impossible_exn_struct _tmp848;const char*_tmp847;struct Cyc_Core_Impossible_exn_struct*_tmp846;(int)_throw((void*)((_tmp846=_cycalloc(sizeof(*_tmp846)),((_tmp846[0]=((_tmp848.tag=Cyc_Core_Impossible,((_tmp848.f1=((_tmp847="anal_Rexp: AggrArrow",_tag_dyneither(_tmp847,sizeof(char),21))),_tmp848)))),_tmp846)))));}_LL1CE:;};_LL1CC:;_LL1CD: {
# 1411
struct Cyc_Core_Impossible_exn_struct _tmp84E;const char*_tmp84D;struct Cyc_Core_Impossible_exn_struct*_tmp84C;(int)_throw((void*)((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84E.tag=Cyc_Core_Impossible,((_tmp84E.f1=((_tmp84D="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp84D,sizeof(char),25))),_tmp84E)))),_tmp84C)))));}_LL1C9:;};};}_LL143: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1CB->tag != 5)goto _LL145;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CB->f2;_tmp1CE=_tmp1CB->f3;}}_LL144: {
# 1415
struct _tuple18 _tmp325=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1CC);union Cyc_CfFlowInfo_FlowInfo _tmp327;union Cyc_CfFlowInfo_FlowInfo _tmp328;struct _tuple18 _tmp326=_tmp325;_tmp327=_tmp326.f1;_tmp328=_tmp326.f2;{
struct _tuple15 _tmp329=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp327,_tmp1CD);
struct _tuple15 _tmp32A=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp328,_tmp1CE);
# 1419
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp167,env->all_changed,_tmp329,_tmp32A);};}_LL145: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1CF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1CF->tag != 6)goto _LL147;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;}}_LL146: {
# 1422
struct _tuple18 _tmp32B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D0);union Cyc_CfFlowInfo_FlowInfo _tmp32D;union Cyc_CfFlowInfo_FlowInfo _tmp32E;struct _tuple18 _tmp32C=_tmp32B;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;{
struct _tuple15 _tmp32F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp32D,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp331;void*_tmp332;struct _tuple15 _tmp330=_tmp32F;_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;{
struct _tuple15 _tmp84F;struct _tuple15 _tmp333=(_tmp84F.f1=_tmp331,((_tmp84F.f2=_tmp332,_tmp84F)));
struct _tuple15 _tmp850;struct _tuple15 _tmp334=(_tmp850.f1=_tmp32E,((_tmp850.f2=_tmp167->zero,_tmp850)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp167,env->all_changed,_tmp333,_tmp334);};};}_LL147: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1D2->tag != 7)goto _LL149;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}_LL148: {
# 1429
struct _tuple18 _tmp337=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1D3);union Cyc_CfFlowInfo_FlowInfo _tmp339;union Cyc_CfFlowInfo_FlowInfo _tmp33A;struct _tuple18 _tmp338=_tmp337;_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;{
struct _tuple15 _tmp33B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33A,_tmp1D4);union Cyc_CfFlowInfo_FlowInfo _tmp33D;void*_tmp33E;struct _tuple15 _tmp33C=_tmp33B;_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;{
struct _tuple15 _tmp851;struct _tuple15 _tmp33F=(_tmp851.f1=_tmp33D,((_tmp851.f2=_tmp33E,_tmp851)));
struct _tuple15 _tmp852;struct _tuple15 _tmp340=(_tmp852.f1=_tmp339,((_tmp852.f2=_tmp167->notzeroall,_tmp852)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp167,env->all_changed,_tmp33F,_tmp340);};};}_LL149: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1D5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1D5->tag != 22)goto _LL14B;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}_LL14A: {
# 1436
struct _RegionHandle _tmp343=_new_region("temp");struct _RegionHandle*temp=& _tmp343;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp853[2];struct _tuple22 _tmp344=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp853[1]=_tmp1D7,((_tmp853[0]=_tmp1D6,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp853,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1437
union Cyc_CfFlowInfo_FlowInfo _tmp346;struct Cyc_List_List*_tmp347;struct _tuple22 _tmp345=_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{
# 1441
union Cyc_CfFlowInfo_FlowInfo _tmp348=_tmp346;
{union Cyc_CfFlowInfo_FlowInfo _tmp349=_tmp346;struct Cyc_Dict_Dict _tmp34A;struct Cyc_List_List*_tmp34B;_LL1D6: if((_tmp349.ReachableFL).tag != 2)goto _LL1D8;_tmp34A=((struct _tuple14)(_tmp349.ReachableFL).val).f1;_tmp34B=((struct _tuple14)(_tmp349.ReachableFL).val).f2;_LL1D7:
# 1446
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp34A,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp347))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_CfFlowInfo_aerr(_tmp1D7->loc,((_tmp856="expression may not be initialized",_tag_dyneither(_tmp856,sizeof(char),34))),_tag_dyneither(_tmp855,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp34E=Cyc_NewControlFlow_add_subscript_reln(_tmp167->r,_tmp34B,_tmp1D6,_tmp1D7);
if(_tmp34B != _tmp34E)
_tmp348=Cyc_CfFlowInfo_ReachableFL(_tmp34A,_tmp34E);
goto _LL1D5;};_LL1D8:;_LL1D9:
 goto _LL1D5;_LL1D5:;}{
# 1454
void*_tmp34F=Cyc_Tcutil_compress((void*)_check_null(_tmp1D6->topt));void*_tmp350=_tmp34F;struct Cyc_List_List*_tmp352;union Cyc_Absyn_Constraint*_tmp354;_LL1DB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp350;if(_tmp351->tag != 10)goto _LL1DD;else{_tmp352=_tmp351->f1;}}_LL1DC: {
# 1456
void*_tmp355=(void*)((struct Cyc_List_List*)_check_null(_tmp347))->hd;void*_tmp356=_tmp355;struct _dyneither_ptr _tmp359;struct _dyneither_ptr _tmp35B;_LL1E2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp357=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp357->tag != 8)goto _LL1E4;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp358=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp357->f2);if(_tmp358->tag != 6)goto _LL1E4;else{_tmp359=_tmp358->f2;}}}_LL1E3:
# 1458
 _tmp35B=_tmp359;goto _LL1E5;_LL1E4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp35A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp356;if(_tmp35A->tag != 6)goto _LL1E6;else{_tmp35B=_tmp35A->f2;}}_LL1E5: {
# 1460
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1D7)).f1;
struct _tuple15 _tmp857;struct _tuple15 _tmp35D=(_tmp857.f1=_tmp348,((_tmp857.f2=*((void**)_check_dyneither_subscript(_tmp35B,sizeof(void*),(int)i)),_tmp857)));_npop_handler(0);return _tmp35D;}_LL1E6:;_LL1E7: {
struct Cyc_Core_Impossible_exn_struct _tmp85D;const char*_tmp85C;struct Cyc_Core_Impossible_exn_struct*_tmp85B;(int)_throw((void*)((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85D.tag=Cyc_Core_Impossible,((_tmp85D.f1=((_tmp85C="anal_Rexp: Subscript",_tag_dyneither(_tmp85C,sizeof(char),21))),_tmp85D)))),_tmp85B)))));}_LL1E1:;}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp353=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp350;if(_tmp353->tag != 5)goto _LL1DF;else{_tmp354=((_tmp353->f1).ptr_atts).bounds;}}_LL1DE: {
# 1465
struct _tuple15 _tmp361=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp346,_tmp1D6,(void*)((struct Cyc_List_List*)_check_null(_tmp347))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp363;void*_tmp364;struct _tuple15 _tmp362=_tmp361;_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp365=_tmp363;_LL1E9: if((_tmp365.BottomFL).tag != 1)goto _LL1EB;_LL1EA: {
struct _tuple15 _tmp85E;struct _tuple15 _tmp367=(_tmp85E.f1=_tmp363,((_tmp85E.f2=_tmp364,_tmp85E)));_npop_handler(0);return _tmp367;}_LL1EB:;_LL1EC: {
struct _tuple15 _tmp85F;struct _tuple15 _tmp369=(_tmp85F.f1=_tmp348,((_tmp85F.f2=_tmp364,_tmp85F)));_npop_handler(0);return _tmp369;}_LL1E8:;};}_LL1DF:;_LL1E0: {
# 1470
struct Cyc_Core_Impossible_exn_struct _tmp865;const char*_tmp864;struct Cyc_Core_Impossible_exn_struct*_tmp863;(int)_throw((void*)((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((_tmp865.tag=Cyc_Core_Impossible,((_tmp865.f1=((_tmp864="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp864,sizeof(char),33))),_tmp865)))),_tmp863)))));}_LL1DA:;};};}
# 1437
;_pop_region(temp);}_LL14B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1D8->tag != 30)goto _LL14D;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL14C:
# 1475
 _tmp1DC=_tmp1D9;goto _LL14E;_LL14D: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1DB=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1DB->tag != 23)goto _LL14F;else{_tmp1DC=_tmp1DB->f1;}}_LL14E: {
# 1477
struct _RegionHandle _tmp36E=_new_region("temp");struct _RegionHandle*temp=& _tmp36E;_push_region(temp);
{struct _tuple22 _tmp36F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1DC,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp371;struct Cyc_List_List*_tmp372;struct _tuple22 _tmp370=_tmp36F;_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;{
unsigned int _tmp86B;unsigned int _tmp86A;struct _dyneither_ptr _tmp869;void**_tmp868;unsigned int _tmp867;struct _dyneither_ptr aggrdict=
(_tmp867=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1DC),((_tmp868=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp867)),((_tmp869=_tag_dyneither(_tmp868,sizeof(void*),_tmp867),((((_tmp86A=_tmp867,_tmp86C(& _tmp86B,& _tmp86A,& _tmp868,& _tmp372))),_tmp869)))))));
# 1485
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp876;struct Cyc_CfFlowInfo_UnionRInfo _tmp875;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp874;struct _tuple15 _tmp873;struct _tuple15 _tmp377=(_tmp873.f1=_tmp371,((_tmp873.f2=(void*)((_tmp876=_region_malloc(env->r,sizeof(*_tmp876)),((_tmp876[0]=((_tmp874.tag=6,((_tmp874.f1=((_tmp875.is_union=0,((_tmp875.fieldnum=- 1,_tmp875)))),((_tmp874.f2=aggrdict,_tmp874)))))),_tmp876)))),_tmp873)));_npop_handler(0);return _tmp377;};}
# 1478
;_pop_region(temp);}_LL14F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1DD->tag != 29)goto _LL151;else{_tmp1DE=_tmp1DD->f2;}}_LL150: {
# 1488
struct Cyc_List_List*fs;
{void*_tmp37D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp37E=_tmp37D;struct Cyc_List_List*_tmp380;_LL1EE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37E;if(_tmp37F->tag != 12)goto _LL1F0;else{_tmp380=_tmp37F->f2;}}_LL1EF:
 fs=_tmp380;goto _LL1ED;_LL1F0:;_LL1F1: {
struct Cyc_Core_Impossible_exn_struct _tmp87C;const char*_tmp87B;struct Cyc_Core_Impossible_exn_struct*_tmp87A;(int)_throw((void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87C.tag=Cyc_Core_Impossible,((_tmp87C.f1=((_tmp87B="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp87B,sizeof(char),35))),_tmp87C)))),_tmp87A)))));}_LL1ED:;}
# 1493
_tmp1E0=_tmp1DE;_tmp1E1=Cyc_Absyn_StructA;_tmp1E2=fs;goto _LL152;}_LL151: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1DF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1DF->tag != 28)goto _LL153;else{_tmp1E0=_tmp1DF->f3;if(_tmp1DF->f4 == 0)goto _LL153;_tmp1E1=(_tmp1DF->f4)->kind;if((_tmp1DF->f4)->impl == 0)goto _LL153;_tmp1E2=((_tmp1DF->f4)->impl)->fields;}}_LL152: {
# 1495
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp384=_new_region("temp");struct _RegionHandle*temp=& _tmp384;_push_region(temp);
{struct _tuple22 _tmp385=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1E0),1,1);
# 1497
union Cyc_CfFlowInfo_FlowInfo _tmp387;struct Cyc_List_List*_tmp388;struct _tuple22 _tmp386=_tmp385;_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;{
# 1499
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp167,_tmp1E2,_tmp1E1 == Cyc_Absyn_UnionA,_tmp167->unknown_all);
# 1502
int field_no=-1;
{int i=0;for(0;_tmp388 != 0;(((_tmp388=_tmp388->tl,_tmp1E0=_tmp1E0->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp389=(void*)ds->hd;void*_tmp38A=_tmp389;struct _dyneither_ptr*_tmp38D;_LL1F3: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp38B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp38A;if(_tmp38B->tag != 0)goto _LL1F5;}_LL1F4: {
struct Cyc_Core_Impossible_exn_struct _tmp882;const char*_tmp881;struct Cyc_Core_Impossible_exn_struct*_tmp880;(int)_throw((void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp882.tag=Cyc_Core_Impossible,((_tmp882.f1=((_tmp881="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp881,sizeof(char),22))),_tmp882)))),_tmp880)))));}_LL1F5: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp38C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38A;if(_tmp38C->tag != 1)goto _LL1F2;else{_tmp38D=_tmp38C->f1;}}_LL1F6:
# 1509
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1E2,_tmp38D);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp388->hd;
# 1512
if(_tmp1E1 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp391=(*((struct _tuple25*)_tmp1E0->hd)).f2;
_tmp387=Cyc_NewControlFlow_use_Rval(env,_tmp391->loc,_tmp387,(void*)_tmp388->hd);
# 1516
Cyc_NewControlFlow_check_requires(_tmp391->loc,_tmp167->r,exp_type,_tmp38D,_tmp387);}_LL1F2:;}}}{
# 1519
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp888;struct Cyc_CfFlowInfo_UnionRInfo _tmp887;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp886;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp392=(_tmp886=_region_malloc(env->r,sizeof(*_tmp886)),((_tmp886[0]=((_tmp888.tag=6,((_tmp888.f1=((_tmp887.is_union=_tmp1E1 == Cyc_Absyn_UnionA,((_tmp887.fieldnum=field_no,_tmp887)))),((_tmp888.f2=aggrdict,_tmp888)))))),_tmp886)));
struct _tuple15 _tmp889;struct _tuple15 _tmp394=(_tmp889.f1=_tmp387,((_tmp889.f2=(void*)_tmp392,_tmp889)));_npop_handler(0);return _tmp394;};};}
# 1497
;_pop_region(temp);}_LL153: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1E3->tag != 28)goto _LL155;}_LL154: {
# 1523
struct Cyc_Core_Impossible_exn_struct _tmp88F;const char*_tmp88E;struct Cyc_Core_Impossible_exn_struct*_tmp88D;(int)_throw((void*)((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88F.tag=Cyc_Core_Impossible,((_tmp88F.f1=((_tmp88E="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp88E,sizeof(char),31))),_tmp88F)))),_tmp88D)))));}_LL155: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1E4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1E4->tag != 25)goto _LL157;else{_tmp1E5=_tmp1E4->f1;}}_LL156: {
# 1525
struct _RegionHandle _tmp39B=_new_region("temp");struct _RegionHandle*temp=& _tmp39B;_push_region(temp);
{struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple25*))Cyc_Core_snd,_tmp1E5);
struct _tuple22 _tmp39D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp39C,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp39F;struct Cyc_List_List*_tmp3A0;struct _tuple22 _tmp39E=_tmp39D;_tmp39F=_tmp39E.f1;_tmp3A0=_tmp39E.f2;
for(0;_tmp3A0 != 0;(_tmp3A0=_tmp3A0->tl,_tmp39C=_tmp39C->tl)){
_tmp39F=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp39C))->hd)->loc,_tmp39F,(void*)_tmp3A0->hd);}{
struct _tuple15 _tmp890;struct _tuple15 _tmp3A2=(_tmp890.f1=_tmp39F,((_tmp890.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,(void*)_check_null(e->topt),0,_tmp167->unknown_all),_tmp890)));_npop_handler(0);return _tmp3A2;};}
# 1526
;_pop_region(temp);}_LL157: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1E6->tag != 26)goto _LL159;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;_tmp1E9=_tmp1E6->f3;_tmp1EA=_tmp1E6->f4;}}_LL158: {
# 1534
struct _tuple15 _tmp3A3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1E8);union Cyc_CfFlowInfo_FlowInfo _tmp3A5;void*_tmp3A6;struct _tuple15 _tmp3A4=_tmp3A3;_tmp3A5=_tmp3A4.f1;_tmp3A6=_tmp3A4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3A7=_tmp3A5;struct Cyc_Dict_Dict _tmp3A8;struct Cyc_List_List*_tmp3A9;_LL1F8: if((_tmp3A7.BottomFL).tag != 1)goto _LL1FA;_LL1F9: {
struct _tuple15 _tmp891;return(_tmp891.f1=_tmp3A5,((_tmp891.f2=_tmp167->unknown_all,_tmp891)));}_LL1FA: if((_tmp3A7.ReachableFL).tag != 2)goto _LL1F7;_tmp3A8=((struct _tuple14)(_tmp3A7.ReachableFL).val).f1;_tmp3A9=((struct _tuple14)(_tmp3A7.ReachableFL).val).f2;_LL1FB:
# 1538
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3A8,_tmp3A6)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp894;void*_tmp893;(_tmp893=0,Cyc_CfFlowInfo_aerr(_tmp1E8->loc,((_tmp894="expression may not be initialized",_tag_dyneither(_tmp894,sizeof(char),34))),_tag_dyneither(_tmp893,sizeof(void*),0)));}{
# 1542
struct Cyc_List_List*new_relns=_tmp3A9;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1E7);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1E8,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3A9);
# 1548
if(_tmp3A9 != new_relns)
_tmp3A5=Cyc_CfFlowInfo_ReachableFL(_tmp3A8,new_relns);{
# 1552
void*_tmp3AD=_tmp3A6;_LL1FD: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3AE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3AD;if(_tmp3AE->tag != 0)goto _LL1FF;}_LL1FE: {
struct _tuple15 _tmp895;return(_tmp895.f1=_tmp3A5,((_tmp895.f2=_tmp167->unknown_all,_tmp895)));}_LL1FF: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3AF=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3AD;if(_tmp3AF->tag != 2)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3B0=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3AD;if(_tmp3B0->tag != 1)goto _LL203;}_LL202:
 goto _LL204;_LL203: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3B1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3AD;if(_tmp3B1->tag != 5)goto _LL205;}_LL204: {
# 1557
struct Cyc_List_List _tmp896;struct Cyc_List_List _tmp3B3=(_tmp896.hd=_tmp1E7,((_tmp896.tl=0,_tmp896)));
_tmp3A5=Cyc_NewControlFlow_add_vars(_tmp167,_tmp3A5,& _tmp3B3,_tmp167->unknown_all,e->loc,0);{
# 1560
struct _tuple15 _tmp3B4=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3A5,_tmp1E9);union Cyc_CfFlowInfo_FlowInfo _tmp3B6;void*_tmp3B7;struct _tuple15 _tmp3B5=_tmp3B4;_tmp3B6=_tmp3B5.f1;_tmp3B7=_tmp3B5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3B8=_tmp3B6;struct Cyc_Dict_Dict _tmp3B9;_LL208: if((_tmp3B8.BottomFL).tag != 1)goto _LL20A;_LL209: {
struct _tuple15 _tmp897;return(_tmp897.f1=_tmp3B6,((_tmp897.f2=_tmp167->unknown_all,_tmp897)));}_LL20A: if((_tmp3B8.ReachableFL).tag != 2)goto _LL207;_tmp3B9=((struct _tuple14)(_tmp3B8.ReachableFL).val).f1;_LL20B:
# 1564
 if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp3B9,_tmp3B7)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp89A;void*_tmp899;(_tmp899=0,Cyc_CfFlowInfo_aerr(_tmp1E8->loc,((_tmp89A="expression may not be initialized",_tag_dyneither(_tmp89A,sizeof(char),34))),_tag_dyneither(_tmp899,sizeof(void*),0)));}{
struct _tuple15 _tmp89B;return(_tmp89B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp89B.f2=_tmp167->unknown_all,_tmp89B)));};}_LL207:;}
# 1569
_tmp3A5=_tmp3B6;
goto _LL206;};}_LL205:;_LL206:
# 1572
 while(1){
struct Cyc_List_List _tmp89C;struct Cyc_List_List _tmp3BF=(_tmp89C.hd=_tmp1E7,((_tmp89C.tl=0,_tmp89C)));
_tmp3A5=Cyc_NewControlFlow_add_vars(_tmp167,_tmp3A5,& _tmp3BF,_tmp167->unknown_all,e->loc,0);{
struct _tuple15 _tmp3C0=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3A5,_tmp1E9);union Cyc_CfFlowInfo_FlowInfo _tmp3C2;void*_tmp3C3;struct _tuple15 _tmp3C1=_tmp3C0;_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C4=_tmp3C2;struct Cyc_Dict_Dict _tmp3C5;_LL20D: if((_tmp3C4.BottomFL).tag != 1)goto _LL20F;_LL20E:
 goto _LL20C;_LL20F: if((_tmp3C4.ReachableFL).tag != 2)goto _LL20C;_tmp3C5=((struct _tuple14)(_tmp3C4.ReachableFL).val).f1;_LL210:
# 1579
 if(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp3C5,_tmp3C3)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp89F;void*_tmp89E;(_tmp89E=0,Cyc_CfFlowInfo_aerr(_tmp1E8->loc,((_tmp89F="expression may not be initialized",_tag_dyneither(_tmp89F,sizeof(char),34))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}{
struct _tuple15 _tmp8A0;return(_tmp8A0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A0.f2=_tmp167->unknown_all,_tmp8A0)));};}_LL20C:;}{
# 1584
union Cyc_CfFlowInfo_FlowInfo _tmp3C9=Cyc_CfFlowInfo_join_flow(_tmp167,env->all_changed,_tmp3A5,_tmp3C2);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3C9,_tmp3A5))
break;
_tmp3A5=_tmp3C9;};};}{
# 1589
struct _tuple15 _tmp8A1;return(_tmp8A1.f1=_tmp3A5,((_tmp8A1.f2=_tmp167->unknown_all,_tmp8A1)));};_LL1FC:;};};_LL1F7:;};}_LL159: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1EB=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1EB->tag != 27)goto _LL15B;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=(void*)_tmp1EB->f2;_tmp1EE=_tmp1EB->f3;}}_LL15A: {
# 1595
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8A4;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8A3;void*root=(void*)((_tmp8A3=_region_malloc(_tmp167->r,sizeof(*_tmp8A3)),((_tmp8A3[0]=((_tmp8A4.tag=1,((_tmp8A4.f1=_tmp1EC,((_tmp8A4.f2=(void*)_check_null(e->topt),_tmp8A4)))))),_tmp8A3))));
struct Cyc_CfFlowInfo_Place*_tmp8A5;struct Cyc_CfFlowInfo_Place*place=(_tmp8A5=_region_malloc(_tmp167->r,sizeof(*_tmp8A5)),((_tmp8A5->root=root,((_tmp8A5->fields=0,_tmp8A5)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8A8;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8A7;void*rval=(void*)((_tmp8A7=_region_malloc(_tmp167->r,sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A8.tag=5,((_tmp8A8.f1=place,_tmp8A8)))),_tmp8A7))));
void*place_val;
# 1603
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,_tmp1ED,0,_tmp167->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple15 _tmp3CC=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp3CE;struct _tuple15 _tmp3CD=_tmp3CC;_tmp3CE=_tmp3CD.f1;
outflow=_tmp3CE;{
union Cyc_CfFlowInfo_FlowInfo _tmp3CF=outflow;struct Cyc_Dict_Dict _tmp3D0;struct Cyc_List_List*_tmp3D1;_LL212: if((_tmp3CF.BottomFL).tag != 1)goto _LL214;_LL213: {
struct _tuple15 _tmp8A9;return(_tmp8A9.f1=outflow,((_tmp8A9.f2=rval,_tmp8A9)));}_LL214: if((_tmp3CF.ReachableFL).tag != 2)goto _LL211;_tmp3D0=((struct _tuple14)(_tmp3CF.ReachableFL).val).f1;_tmp3D1=((struct _tuple14)(_tmp3CF.ReachableFL).val).f2;_LL215: {
# 1611
struct _tuple15 _tmp8AA;return(_tmp8AA.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3D0,root,place_val),_tmp3D1),((_tmp8AA.f2=rval,_tmp8AA)));}_LL211:;};};};}_LL15B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1EF=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1EF->tag != 36)goto _LL15D;else{_tmp1F0=_tmp1EF->f1;}}_LL15C: {
# 1615
struct _tuple16 _tmp8AB;struct _tuple16 _tmp3D9=(_tmp8AB.f1=(env->fenv)->unknown_all,((_tmp8AB.f2=copy_ctxt,_tmp8AB)));
union Cyc_CfFlowInfo_FlowInfo _tmp3DA=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1F0,& _tmp3D9);
struct _tuple15 _tmp8AC;return(_tmp8AC.f1=_tmp3DA,((_tmp8AC.f2=_tmp3D9.f1,_tmp8AC)));}_LL15D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1F1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1F1->tag != 1)goto _LL15F;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1F2=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1F1->f2);if(_tmp1F2->tag != 0)goto _LL15F;}}_LL15E:
# 1619
 goto _LL160;_LL15F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1F3->tag != 35)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1F4=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1F4->tag != 24)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1F5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1F5->tag != 38)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1F6=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp1F6->tag != 39)goto _LLFE;}_LL166: {
# 1624
struct Cyc_Core_Impossible_exn_struct _tmp8B2;const char*_tmp8B1;struct Cyc_Core_Impossible_exn_struct*_tmp8B0;(int)_throw((void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B2.tag=Cyc_Core_Impossible,((_tmp8B2.f1=((_tmp8B1="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8B1,sizeof(char),31))),_tmp8B2)))),_tmp8B0)))));}_LLFE:;};}
# 1634
static struct _tuple17 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1642
struct Cyc_CfFlowInfo_FlowEnv*_tmp3E0=env->fenv;
void*_tmp3E1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3E2=_tmp3E1;void*_tmp3E4;union Cyc_Absyn_Constraint*_tmp3E5;union Cyc_Absyn_Constraint*_tmp3E6;_LL217: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2;if(_tmp3E3->tag != 5)goto _LL219;else{_tmp3E4=(_tmp3E3->f1).elt_typ;_tmp3E5=((_tmp3E3->f1).ptr_atts).bounds;_tmp3E6=((_tmp3E3->f1).ptr_atts).zero_term;}}_LL218: {
# 1645
union Cyc_CfFlowInfo_FlowInfo _tmp3E7=f;struct Cyc_Dict_Dict _tmp3E8;struct Cyc_List_List*_tmp3E9;_LL21C: if((_tmp3E7.BottomFL).tag != 1)goto _LL21E;_LL21D: {
struct _tuple17 _tmp8B3;return(_tmp8B3.f1=f,((_tmp8B3.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8B3)));}_LL21E: if((_tmp3E7.ReachableFL).tag != 2)goto _LL21B;_tmp3E8=((struct _tuple14)(_tmp3E7.ReachableFL).val).f1;_tmp3E9=((struct _tuple14)(_tmp3E7.ReachableFL).val).f2;_LL21F: {
# 1649
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp3E5)){
retry: {void*_tmp3EB=r;struct Cyc_Absyn_Vardecl*_tmp3ED;void*_tmp3EE;void*_tmp3F2;struct Cyc_List_List*_tmp3F3;enum Cyc_CfFlowInfo_InitLevel _tmp3F6;_LL221: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3EC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3EC->tag != 8)goto _LL223;else{_tmp3ED=_tmp3EC->f1;_tmp3EE=(void*)_tmp3EC->f2;}}_LL222:
 r=_tmp3EE;locname=_tmp3ED;goto retry;_LL223: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3EF=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3EF->tag != 1)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3F0=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3F0->tag != 2)goto _LL227;}_LL226:
# 1655
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8B6;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8B5;e->annot=(void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B6.tag=Cyc_CfFlowInfo_NotZero,((_tmp8B6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3E9),_tmp8B6)))),_tmp8B5))));}goto _LL220;_LL227: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3F1=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3F1->tag != 5)goto _LL229;else{_tmp3F2=(_tmp3F1->f1)->root;_tmp3F3=(_tmp3F1->f1)->fields;}}_LL228:
# 1657
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8B9;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8B8;e->annot=(void*)((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8B9.tag=Cyc_CfFlowInfo_NotZero,((_tmp8B9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3E9),_tmp8B9)))),_tmp8B8))));}{
struct Cyc_CfFlowInfo_Place*_tmp8BC;struct _tuple17 _tmp8BB;return(_tmp8BB.f1=f,((_tmp8BB.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8BC=_region_malloc(_tmp3E0->r,sizeof(*_tmp8BC)),((_tmp8BC->root=_tmp3F2,((_tmp8BC->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3E0->r,_tmp3F3,flds),_tmp8BC))))))),_tmp8BB)));};_LL229: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3F4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3F4->tag != 0)goto _LL22B;}_LL22A:
# 1660
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple17 _tmp8BD;return(_tmp8BD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BD.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8BD)));};_LL22B: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3F5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EB;if(_tmp3F5->tag != 3)goto _LL22D;else{_tmp3F6=_tmp3F5->f1;}}_LL22C:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3F6,locname);goto _LL22E;_LL22D:;_LL22E:
# 1665
 if(passthrough_consumes){
void*_tmp3FE=r;void*_tmp400;_LL230: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3FF=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3FE;if(_tmp3FF->tag != 7)goto _LL232;else{_tmp400=(void*)_tmp3FF->f3;}}_LL231:
 r=_tmp400;goto retry;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 1671
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8C0;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8BF;e->annot=(void*)((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF[0]=((_tmp8C0.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8C0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3E9),_tmp8C0)))),_tmp8BF))));};_LL220:;}}else{
# 1674
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8C3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8C2;e->annot=(void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8C3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3E9),_tmp8C3)))),_tmp8C2))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3E0,_tmp3E8,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8C6;void*_tmp8C5;(_tmp8C5=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8C6="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp8C6,sizeof(char),46))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}{
struct _tuple17 _tmp8C7;return(_tmp8C7.f1=f,((_tmp8C7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C7)));};}_LL21B:;}_LL219:;_LL21A: {
# 1679
struct Cyc_Core_Impossible_exn_struct _tmp8CD;const char*_tmp8CC;struct Cyc_Core_Impossible_exn_struct*_tmp8CB;(int)_throw((void*)((_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CD.tag=Cyc_Core_Impossible,((_tmp8CD.f1=((_tmp8CC="left deref of non-pointer-type",_tag_dyneither(_tmp8CC,sizeof(char),31))),_tmp8CD)))),_tmp8CB)))));}_LL216:;}
# 1687
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1690
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp40B=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp40C=inflow;struct Cyc_Dict_Dict _tmp40D;struct Cyc_List_List*_tmp40E;_LL235: if((_tmp40C.BottomFL).tag != 1)goto _LL237;_LL236: {
struct _tuple17 _tmp8CE;return(_tmp8CE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8CE.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8CE)));}_LL237: if((_tmp40C.ReachableFL).tag != 2)goto _LL234;_tmp40D=((struct _tuple14)(_tmp40C.ReachableFL).val).f1;_tmp40E=((struct _tuple14)(_tmp40C.ReachableFL).val).f2;_LL238:
# 1695
 d=_tmp40D;_LL234:;}{
# 1699
void*_tmp410=e->r;void*_tmp411=_tmp410;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Vardecl*_tmp41A;struct Cyc_Absyn_Vardecl*_tmp41D;struct Cyc_Absyn_Vardecl*_tmp420;struct Cyc_Absyn_Vardecl*_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct _dyneither_ptr*_tmp426;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;struct _dyneither_ptr*_tmp42E;_LL23A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp412=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp412->tag != 13)goto _LL23C;else{_tmp413=_tmp412->f2;}}_LL23B:
 _tmp415=_tmp413;goto _LL23D;_LL23C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp414=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp414->tag != 11)goto _LL23E;else{_tmp415=_tmp414->f1;}}_LL23D:
 _tmp417=_tmp415;goto _LL23F;_LL23E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp416=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp416->tag != 12)goto _LL240;else{_tmp417=_tmp416->f1;}}_LL23F:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp417,flds);_LL240: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp418=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp418->tag != 1)goto _LL242;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp419=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp418->f2);if(_tmp419->tag != 3)goto _LL242;else{_tmp41A=_tmp419->f1;}}}_LL241:
# 1704
 _tmp41D=_tmp41A;goto _LL243;_LL242: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp41B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp41B->tag != 1)goto _LL244;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp41B->f2);if(_tmp41C->tag != 4)goto _LL244;else{_tmp41D=_tmp41C->f1;}}}_LL243:
 _tmp420=_tmp41D;goto _LL245;_LL244: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp41E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp41E->tag != 1)goto _LL246;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp41F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp41E->f2);if(_tmp41F->tag != 5)goto _LL246;else{_tmp420=_tmp41F->f1;}}}_LL245: {
# 1707
struct Cyc_CfFlowInfo_Place*_tmp8D8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8D7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8D6;struct _tuple17 _tmp8D5;return(_tmp8D5.f1=inflow,((_tmp8D5.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8D8=_region_malloc(env->r,sizeof(*_tmp8D8)),((_tmp8D8->root=(void*)((_tmp8D6=_region_malloc(env->r,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=0,((_tmp8D7.f1=_tmp420,_tmp8D7)))),_tmp8D6)))),((_tmp8D8->fields=flds,_tmp8D8))))))),_tmp8D5)));}_LL246: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp421=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp421->tag != 1)goto _LL248;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp422=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp421->f2);if(_tmp422->tag != 1)goto _LL248;else{_tmp423=_tmp422->f1;}}}_LL247: {
# 1709
struct _tuple17 _tmp8D9;return(_tmp8D9.f1=inflow,((_tmp8D9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8D9)));}_LL248: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp424=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp424->tag != 21)goto _LL24A;else{_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;}}_LL249:
# 1712
{void*_tmp434=Cyc_Tcutil_compress((void*)_check_null(_tmp425->topt));void*_tmp435=_tmp434;void*_tmp437;_LL253: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp436=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435;if(_tmp436->tag != 5)goto _LL255;else{_tmp437=(_tmp436->f1).elt_typ;}}_LL254:
# 1714
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp437)){
Cyc_NewControlFlow_check_requires(_tmp425->loc,_tmp40B->r,_tmp437,_tmp426,inflow);{
struct Cyc_List_List*_tmp8DA;flds=((_tmp8DA=_region_malloc(env->r,sizeof(*_tmp8DA)),((_tmp8DA->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp437,_tmp426),((_tmp8DA->tl=flds,_tmp8DA))))));};}
# 1718
goto _LL252;_LL255:;_LL256: {
struct Cyc_Core_Impossible_exn_struct _tmp8E0;const char*_tmp8DF;struct Cyc_Core_Impossible_exn_struct*_tmp8DE;(int)_throw((void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8E0.tag=Cyc_Core_Impossible,((_tmp8E0.f1=((_tmp8DF="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp8DF,sizeof(char),25))),_tmp8E0)))),_tmp8DE)))));}_LL252:;}
# 1721
_tmp428=_tmp425;goto _LL24B;_LL24A: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp427->tag != 19)goto _LL24C;else{_tmp428=_tmp427->f1;}}_LL24B:
# 1724
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp428->topt),1)){
# 1726
struct _tuple17 _tmp43C=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp428->topt),1),passthrough_consumes,_tmp428);
# 1726
union Cyc_CfFlowInfo_FlowInfo _tmp43E;union Cyc_CfFlowInfo_AbsLVal _tmp43F;struct _tuple17 _tmp43D=_tmp43C;_tmp43E=_tmp43D.f1;_tmp43F=_tmp43D.f2;{
# 1730
struct _tuple17 _tmp8E1;struct _tuple17 _tmp440=(_tmp8E1.f1=_tmp43E,((_tmp8E1.f2=_tmp43F,_tmp8E1)));struct _tuple17 _tmp441=_tmp440;struct Cyc_Dict_Dict _tmp442;struct Cyc_List_List*_tmp443;struct Cyc_CfFlowInfo_Place*_tmp444;_LL258: if(((_tmp441.f1).ReachableFL).tag != 2)goto _LL25A;_tmp442=((struct _tuple14)((_tmp441.f1).ReachableFL).val).f1;_tmp443=((struct _tuple14)((_tmp441.f1).ReachableFL).val).f2;if(((_tmp441.f2).PlaceL).tag != 1)goto _LL25A;_tmp444=(struct Cyc_CfFlowInfo_Place*)((_tmp441.f2).PlaceL).val;_LL259: {
# 1732
void*_tmp445=Cyc_CfFlowInfo_lookup_place(_tmp442,_tmp444);
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp446=_tmp445;struct Cyc_Absyn_Vardecl*_tmp448;void*_tmp449;_LL25D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp447=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp446;if(_tmp447->tag != 8)goto _LL25F;else{_tmp448=_tmp447->f1;_tmp449=(void*)_tmp447->f2;}}_LL25E:
 _tmp445=_tmp449;locname=_tmp448;goto retry;_LL25F: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp44A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp446;if(_tmp44A->tag != 7)goto _LL261;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp44A->f3);if(_tmp44B->tag != 5)goto _LL261;}}_LL260:
 goto _LL262;_LL261: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp44C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp446;if(_tmp44C->tag != 5)goto _LL263;}_LL262:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp43E,_tmp428,_tmp43E,_tmp445,passthrough_consumes,flds);_LL263:;_LL264: {
# 1744
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp40B,_tmp442,_tmp445);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp40B->unknown_all: _tmp40B->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp40B,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp428->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8E4;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8E3;void*new_root=(void*)((_tmp8E3=_region_malloc(_tmp40B->r,sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=1,((_tmp8E4.f1=e,((_tmp8E4.f2=(void*)_check_null(e->topt),_tmp8E4)))))),_tmp8E3))));
struct Cyc_CfFlowInfo_Place*_tmp8E5;struct Cyc_CfFlowInfo_Place*place=(_tmp8E5=_region_malloc(_tmp40B->r,sizeof(*_tmp8E5)),((_tmp8E5->root=new_root,((_tmp8E5->fields=0,_tmp8E5)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8E8;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8E7;void*res=(void*)((_tmp8E7=_region_malloc(_tmp40B->r,sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=5,((_tmp8E8.f1=place,_tmp8E8)))),_tmp8E7))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8EB;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8EA;res=(void*)((_tmp8EA=_region_malloc(_tmp40B->r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=8,((_tmp8EB.f1=locname,((_tmp8EB.f2=res,_tmp8EB)))))),_tmp8EA))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp442=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp442,new_root,new_rval);
_tmp442=Cyc_CfFlowInfo_assign_place(_tmp40B,e->loc,_tmp442,env->all_changed,_tmp444,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp44F=Cyc_CfFlowInfo_ReachableFL(_tmp442,_tmp443);
# 1759
return Cyc_NewControlFlow_anal_derefL(env,_tmp44F,_tmp428,_tmp44F,res,passthrough_consumes,flds);};}_LL25C:;}}_LL25A:;_LL25B:
# 1761
 goto _LL257;_LL257:;};}{
# 1764
struct _tuple15 _tmp456=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp428);union Cyc_CfFlowInfo_FlowInfo _tmp458;void*_tmp459;struct _tuple15 _tmp457=_tmp456;_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp428,_tmp458,_tmp459,passthrough_consumes,flds);};_LL24C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp429=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp429->tag != 22)goto _LL24E;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}_LL24D: {
# 1768
void*_tmp45A=Cyc_Tcutil_compress((void*)_check_null(_tmp42A->topt));void*_tmp45B=_tmp45A;union Cyc_Absyn_Constraint*_tmp45E;_LL266: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp45B;if(_tmp45C->tag != 10)goto _LL268;}_LL267: {
# 1770
unsigned int _tmp45F=(Cyc_Evexp_eval_const_uint_exp(_tmp42B)).f1;
struct Cyc_List_List*_tmp8EC;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp42A,((_tmp8EC=_region_malloc(env->r,sizeof(*_tmp8EC)),((_tmp8EC->hd=(void*)((int)_tmp45F),((_tmp8EC->tl=flds,_tmp8EC)))))));}_LL268: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45B;if(_tmp45D->tag != 5)goto _LL26A;else{_tmp45E=((_tmp45D->f1).ptr_atts).bounds;}}_LL269: {
# 1773
struct _RegionHandle _tmp461=_new_region("temp");struct _RegionHandle*temp=& _tmp461;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp8ED[2];struct _tuple22 _tmp462=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp8ED[1]=_tmp42B,((_tmp8ED[0]=_tmp42A,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8ED,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1774
union Cyc_CfFlowInfo_FlowInfo _tmp464;struct Cyc_List_List*_tmp465;struct _tuple22 _tmp463=_tmp462;_tmp464=_tmp463.f1;_tmp465=_tmp463.f2;{
# 1776
union Cyc_CfFlowInfo_FlowInfo _tmp466=_tmp464;
{union Cyc_CfFlowInfo_FlowInfo _tmp467=_tmp464;struct Cyc_Dict_Dict _tmp468;struct Cyc_List_List*_tmp469;_LL26D: if((_tmp467.ReachableFL).tag != 2)goto _LL26F;_tmp468=((struct _tuple14)(_tmp467.ReachableFL).val).f1;_tmp469=((struct _tuple14)(_tmp467.ReachableFL).val).f2;_LL26E:
# 1779
 if(Cyc_CfFlowInfo_initlevel(_tmp40B,_tmp468,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp465))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_CfFlowInfo_aerr(_tmp42B->loc,((_tmp8F0="expression may not be initialized",_tag_dyneither(_tmp8F0,sizeof(char),34))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp46C=Cyc_NewControlFlow_add_subscript_reln(_tmp40B->r,_tmp469,_tmp42A,_tmp42B);
if(_tmp469 != _tmp46C)
_tmp466=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp46C);
goto _LL26C;};_LL26F:;_LL270:
 goto _LL26C;_LL26C:;}{
# 1787
struct _tuple17 _tmp46D=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp42A,_tmp464,(void*)((struct Cyc_List_List*)_check_null(_tmp465))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp46F;union Cyc_CfFlowInfo_AbsLVal _tmp470;struct _tuple17 _tmp46E=_tmp46D;_tmp46F=_tmp46E.f1;_tmp470=_tmp46E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp471=_tmp46F;_LL272: if((_tmp471.BottomFL).tag != 1)goto _LL274;_LL273: {
struct _tuple17 _tmp8F1;struct _tuple17 _tmp473=(_tmp8F1.f1=_tmp46F,((_tmp8F1.f2=_tmp470,_tmp8F1)));_npop_handler(0);return _tmp473;}_LL274:;_LL275: {
struct _tuple17 _tmp8F2;struct _tuple17 _tmp475=(_tmp8F2.f1=_tmp466,((_tmp8F2.f2=_tmp470,_tmp8F2)));_npop_handler(0);return _tmp475;}_LL271:;};};};}
# 1774
;_pop_region(temp);}_LL26A:;_LL26B: {
# 1793
struct Cyc_Core_Impossible_exn_struct _tmp8F8;const char*_tmp8F7;struct Cyc_Core_Impossible_exn_struct*_tmp8F6;(int)_throw((void*)((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F8.tag=Cyc_Core_Impossible,((_tmp8F8.f1=((_tmp8F7="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp8F7,sizeof(char),33))),_tmp8F8)))),_tmp8F6)))));}_LL265:;}_LL24E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp42C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp42C->tag != 20)goto _LL250;else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}}_LL24F: {
# 1797
void*_tmp47A=(void*)_check_null(_tmp42D->topt);
if(Cyc_Absyn_is_require_union_type(_tmp47A))
Cyc_NewControlFlow_check_requires(_tmp42D->loc,_tmp40B->r,_tmp47A,_tmp42E,inflow);
# 1801
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp47A)){
struct _tuple17 _tmp8F9;return(_tmp8F9.f1=inflow,((_tmp8F9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F9)));}{
# 1804
struct Cyc_List_List*_tmp8FA;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp42D,(
(_tmp8FA=_region_malloc(env->r,sizeof(*_tmp8FA)),((_tmp8FA->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp47A,_tmp42E),((_tmp8FA->tl=flds,_tmp8FA)))))));};}_LL250:;_LL251: {
# 1807
struct _tuple17 _tmp8FB;return(_tmp8FB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8FB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8FB)));}_LL239:;};}
# 1811
static struct _tuple17 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1814
struct _tuple17 _tmp47E=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp480;union Cyc_CfFlowInfo_AbsLVal _tmp481;struct _tuple17 _tmp47F=_tmp47E;_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;{
struct _tuple17 _tmp8FC;return(_tmp8FC.f1=_tmp480,((_tmp8FC.f2=_tmp481,_tmp8FC)));};}
# 1821
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1823
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple17 _tmp483=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp485;struct _tuple17 _tmp484=_tmp483;_tmp485=_tmp484.f1;
inflow=_tmp485;}}}
# 1830
return inflow;}
# 1836
static struct _tuple18 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1839
struct Cyc_CfFlowInfo_FlowEnv*_tmp486=env->fenv;
# 1842
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp487=_new_region("temp");struct _RegionHandle*temp=& _tmp487;_push_region(temp);{
struct _tuple22 _tmp488=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp48A;struct Cyc_List_List*_tmp48B;struct _tuple22 _tmp489=_tmp488;_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp48B))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp48B->tl))->hd;
f=_tmp48A;}{
# 1852
union Cyc_CfFlowInfo_FlowInfo _tmp48C=f;struct Cyc_Dict_Dict _tmp48D;struct Cyc_List_List*_tmp48E;_LL277: if((_tmp48C.BottomFL).tag != 1)goto _LL279;_LL278: {
struct _tuple18 _tmp8FD;struct _tuple18 _tmp490=(_tmp8FD.f1=f,((_tmp8FD.f2=f,_tmp8FD)));_npop_handler(0);return _tmp490;}_LL279: if((_tmp48C.ReachableFL).tag != 2)goto _LL276;_tmp48D=((struct _tuple14)(_tmp48C.ReachableFL).val).f1;_tmp48E=((struct _tuple14)(_tmp48C.ReachableFL).val).f2;_LL27A: {
# 1855
struct Cyc_Absyn_Exp*_tmp491=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp492=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp48D,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp900="expression may not be initialized",_tag_dyneither(_tmp900,sizeof(char),34))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp48D,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp903="expression may not be initialized",_tag_dyneither(_tmp903,sizeof(char),34))),_tag_dyneither(_tmp902,sizeof(void*),0)));}{
# 1862
union Cyc_CfFlowInfo_FlowInfo _tmp497=f;
union Cyc_CfFlowInfo_FlowInfo _tmp498=f;
# 1867
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp499=r1;struct Cyc_Absyn_Vardecl*_tmp49B;void*_tmp49C;_LL27C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp49A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp499;if(_tmp49A->tag != 8)goto _LL27E;else{_tmp49B=_tmp49A->f1;_tmp49C=(void*)_tmp49A->f2;}}_LL27D:
 r1=_tmp49C;r1n=_tmp49B;goto _LL27B;_LL27E:;_LL27F:
 goto _LL27B;_LL27B:;}
# 1873
{void*_tmp49D=r2;struct Cyc_Absyn_Vardecl*_tmp49F;void*_tmp4A0;_LL281: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp49E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp49D;if(_tmp49E->tag != 8)goto _LL283;else{_tmp49F=_tmp49E->f1;_tmp4A0=(void*)_tmp49E->f2;}}_LL282:
 r2=_tmp4A0;r2n=_tmp49F;goto _LL280;_LL283:;_LL284:
 goto _LL280;_LL280:;}{
# 1878
struct _tuple0 _tmp904;struct _tuple0 _tmp4A1=(_tmp904.f1=r1,((_tmp904.f2=r2,_tmp904)));struct _tuple0 _tmp4A2=_tmp4A1;enum Cyc_CfFlowInfo_InitLevel _tmp4A4;enum Cyc_CfFlowInfo_InitLevel _tmp4A8;_LL286:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4A3=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4A3->tag != 3)goto _LL288;else{_tmp4A4=_tmp4A3->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4A5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4A5->tag != 0)goto _LL288;};_LL287: {
# 1882
struct _tuple18 _tmp4B7=Cyc_NewControlFlow_splitzero(env,f,f,_tmp491,_tmp4A4,r1n);union Cyc_CfFlowInfo_FlowInfo _tmp4B9;union Cyc_CfFlowInfo_FlowInfo _tmp4BA;struct _tuple18 _tmp4B8=_tmp4B7;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
switch(p){case Cyc_Absyn_Eq: _LL29A:
 _tmp497=_tmp4BA;_tmp498=_tmp4B9;break;case Cyc_Absyn_Neq: _LL29B:
 _tmp497=_tmp4B9;_tmp498=_tmp4BA;break;default: _LL29C: {
struct Cyc_Core_Impossible_exn_struct _tmp90A;const char*_tmp909;struct Cyc_Core_Impossible_exn_struct*_tmp908;(int)_throw((void*)((_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((_tmp90A.tag=Cyc_Core_Impossible,((_tmp90A.f1=((_tmp909="anal_test, zero-split",_tag_dyneither(_tmp909,sizeof(char),22))),_tmp90A)))),_tmp908)))));}}
# 1888
goto _LL285;}_LL288:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4A6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4A6->tag != 0)goto _LL28A;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4A7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4A7->tag != 3)goto _LL28A;else{_tmp4A8=_tmp4A7->f1;}};_LL289: {
# 1890
struct _tuple18 _tmp4BE=Cyc_NewControlFlow_splitzero(env,f,f,_tmp492,_tmp4A8,r2n);union Cyc_CfFlowInfo_FlowInfo _tmp4C0;union Cyc_CfFlowInfo_FlowInfo _tmp4C1;struct _tuple18 _tmp4BF=_tmp4BE;_tmp4C0=_tmp4BF.f1;_tmp4C1=_tmp4BF.f2;
switch(p){case Cyc_Absyn_Eq: _LL29E:
 _tmp497=_tmp4C1;_tmp498=_tmp4C0;break;case Cyc_Absyn_Neq: _LL29F:
 _tmp497=_tmp4C0;_tmp498=_tmp4C1;break;default: _LL2A0: {
struct Cyc_Core_Impossible_exn_struct _tmp910;const char*_tmp90F;struct Cyc_Core_Impossible_exn_struct*_tmp90E;(int)_throw((void*)((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp910.tag=Cyc_Core_Impossible,((_tmp910.f1=((_tmp90F="anal_test, zero-split",_tag_dyneither(_tmp90F,sizeof(char),22))),_tmp910)))),_tmp90E)))));}}
# 1896
goto _LL285;}_LL28A:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4A9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4A9->tag != 0)goto _LL28C;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4AA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4AA->tag != 0)goto _LL28C;};_LL28B:
# 1898
 if(p == Cyc_Absyn_Eq)_tmp498=Cyc_CfFlowInfo_BottomFL();else{
_tmp497=Cyc_CfFlowInfo_BottomFL();}
goto _LL285;_LL28C:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4AB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4AB->tag != 0)goto _LL28E;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4AC=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4AC->tag != 1)goto _LL28E;};_LL28D:
 goto _LL28F;_LL28E:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4AD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4AD->tag != 0)goto _LL290;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4AE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4AE->tag != 2)goto _LL290;};_LL28F:
 goto _LL291;_LL290:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4AF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4AF->tag != 0)goto _LL292;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4B0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4B0->tag != 5)goto _LL292;};_LL291:
 goto _LL293;_LL292:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4B1=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4B1->tag != 1)goto _LL294;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4B2->tag != 0)goto _LL294;};_LL293:
 goto _LL295;_LL294:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4B3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4B3->tag != 2)goto _LL296;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4B4->tag != 0)goto _LL296;};_LL295:
 goto _LL297;_LL296:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4B5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f1;if(_tmp4B5->tag != 5)goto _LL298;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4B6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4A2.f2;if(_tmp4B6->tag != 0)goto _LL298;};_LL297:
# 1907
 if(p == Cyc_Absyn_Neq)_tmp498=Cyc_CfFlowInfo_BottomFL();else{
_tmp497=Cyc_CfFlowInfo_BottomFL();}
goto _LL285;_LL298:;_LL299:
 goto _LL285;_LL285:;};}
# 1918
{struct _tuple0 _tmp911;struct _tuple0 _tmp4C6=(_tmp911.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp491->topt)),((_tmp911.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp492->topt)),_tmp911)));struct _tuple0 _tmp4C7=_tmp4C6;_LL2A3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4C8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4C7.f1;if(_tmp4C8->tag != 6)goto _LL2A5;else{if(_tmp4C8->f1 != Cyc_Absyn_Unsigned)goto _LL2A5;}}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4C7.f2;if(_tmp4C9->tag != 6)goto _LL2A7;else{if(_tmp4C9->f1 != Cyc_Absyn_Unsigned)goto _LL2A7;}}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C7.f1;if(_tmp4CA->tag != 19)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C7.f2;if(_tmp4CB->tag != 19)goto _LL2AB;}_LL2AA:
 goto _LL2A2;_LL2AB:;_LL2AC: {
struct _tuple18 _tmp912;struct _tuple18 _tmp4CD=(_tmp912.f1=_tmp497,((_tmp912.f2=_tmp498,_tmp912)));_npop_handler(0);return _tmp4CD;}_LL2A2:;}
# 1927
switch(p){case Cyc_Absyn_Eq: _LL2AD:
 goto _LL2AE;case Cyc_Absyn_Neq: _LL2AE: goto _LL2AF;case Cyc_Absyn_Gt: _LL2AF: goto _LL2B0;case Cyc_Absyn_Gte: _LL2B0: goto _LL2B1;case Cyc_Absyn_Lt: _LL2B1: goto _LL2B2;case Cyc_Absyn_Lte: _LL2B2: break;default: _LL2B3: {
struct _tuple18 _tmp913;struct _tuple18 _tmp4D0=(_tmp913.f1=_tmp497,((_tmp913.f2=_tmp498,_tmp913)));_npop_handler(0);return _tmp4D0;}}{
# 1932
struct _RegionHandle*_tmp4D1=(env->fenv)->r;
struct _tuple13 _tmp4D2=Cyc_Relations_primop2relation(_tmp491,p,_tmp492);struct Cyc_Absyn_Exp*_tmp4D4;enum Cyc_Relations_Relation _tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct _tuple13 _tmp4D3=_tmp4D2;_tmp4D4=_tmp4D3.f1;_tmp4D5=_tmp4D3.f2;_tmp4D6=_tmp4D3.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1937
if(Cyc_Relations_exp2relnop(_tmp4D4,& n1) && Cyc_Relations_exp2relnop(_tmp4D6,& n2)){
# 1939
struct Cyc_List_List*_tmp4D7=Cyc_Relations_add_relation(_tmp4D1,n1,_tmp4D5,n2,_tmp48E);
# 1943
struct Cyc_List_List*_tmp4D8=Cyc_Relations_add_relation(_tmp4D1,n2,Cyc_Relations_flip_relation(_tmp4D5),n1,_tmp48E);
struct _tuple18 _tmp914;struct _tuple18 _tmp4D9=(_tmp914.f1=_tmp497,((_tmp914.f2=_tmp498,_tmp914)));struct _tuple18 _tmp4DA=_tmp4D9;struct Cyc_Dict_Dict _tmp4DB;struct Cyc_Dict_Dict _tmp4DC;struct Cyc_Dict_Dict _tmp4DD;struct Cyc_Dict_Dict _tmp4DE;_LL2B6: if(((_tmp4DA.f1).ReachableFL).tag != 2)goto _LL2B8;_tmp4DB=((struct _tuple14)((_tmp4DA.f1).ReachableFL).val).f1;if(((_tmp4DA.f2).ReachableFL).tag != 2)goto _LL2B8;_tmp4DC=((struct _tuple14)((_tmp4DA.f2).ReachableFL).val).f1;_LL2B7: {
# 1946
struct _tuple18 _tmp915;struct _tuple18 _tmp4E0=(_tmp915.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4DB,_tmp4D7),((_tmp915.f2=Cyc_CfFlowInfo_ReachableFL(_tmp4DC,_tmp4D8),_tmp915)));_npop_handler(0);return _tmp4E0;}_LL2B8: if(((_tmp4DA.f2).ReachableFL).tag != 2)goto _LL2BA;_tmp4DD=((struct _tuple14)((_tmp4DA.f2).ReachableFL).val).f1;_LL2B9: {
# 1948
struct _tuple18 _tmp916;struct _tuple18 _tmp4E2=(_tmp916.f1=_tmp497,((_tmp916.f2=Cyc_CfFlowInfo_ReachableFL(_tmp4DD,_tmp4D8),_tmp916)));_npop_handler(0);return _tmp4E2;}_LL2BA: if(((_tmp4DA.f1).ReachableFL).tag != 2)goto _LL2BC;_tmp4DE=((struct _tuple14)((_tmp4DA.f1).ReachableFL).val).f1;_LL2BB: {
# 1950
struct _tuple18 _tmp917;struct _tuple18 _tmp4E4=(_tmp917.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4DE,_tmp4D7),((_tmp917.f2=_tmp498,_tmp917)));_npop_handler(0);return _tmp4E4;}_LL2BC:;_LL2BD: {
# 1952
struct _tuple18 _tmp918;struct _tuple18 _tmp4E6=(_tmp918.f1=_tmp497,((_tmp918.f2=_tmp498,_tmp918)));_npop_handler(0);return _tmp4E6;}_LL2B5:;}else{
# 1955
struct _tuple18 _tmp919;struct _tuple18 _tmp4E9=(_tmp919.f1=_tmp497,((_tmp919.f2=_tmp498,_tmp919)));_npop_handler(0);return _tmp4E9;}};};};}_LL276:;};
# 1844
;_pop_region(temp);}
# 1961
static struct _tuple18 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1963
struct Cyc_CfFlowInfo_FlowEnv*_tmp4EA=env->fenv;
void*_tmp4EB=e->r;void*_tmp4EC=_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;enum Cyc_Absyn_Primop _tmp4FD;struct Cyc_List_List*_tmp4FE;_LL2BF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4ED->tag != 5)goto _LL2C1;else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;_tmp4F0=_tmp4ED->f3;}}_LL2C0: {
# 1966
struct _tuple18 _tmp4FF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4EE);union Cyc_CfFlowInfo_FlowInfo _tmp501;union Cyc_CfFlowInfo_FlowInfo _tmp502;struct _tuple18 _tmp500=_tmp4FF;_tmp501=_tmp500.f1;_tmp502=_tmp500.f2;{
struct _tuple18 _tmp503=Cyc_NewControlFlow_anal_test(env,_tmp501,_tmp4EF);union Cyc_CfFlowInfo_FlowInfo _tmp505;union Cyc_CfFlowInfo_FlowInfo _tmp506;struct _tuple18 _tmp504=_tmp503;_tmp505=_tmp504.f1;_tmp506=_tmp504.f2;{
struct _tuple18 _tmp507=Cyc_NewControlFlow_anal_test(env,_tmp502,_tmp4F0);union Cyc_CfFlowInfo_FlowInfo _tmp509;union Cyc_CfFlowInfo_FlowInfo _tmp50A;struct _tuple18 _tmp508=_tmp507;_tmp509=_tmp508.f1;_tmp50A=_tmp508.f2;{
struct _tuple18 _tmp91A;return(_tmp91A.f1=Cyc_CfFlowInfo_join_flow(_tmp4EA,env->all_changed,_tmp505,_tmp509),((_tmp91A.f2=
Cyc_CfFlowInfo_join_flow(_tmp4EA,env->all_changed,_tmp506,_tmp50A),_tmp91A)));};};};}_LL2C1: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4F1->tag != 6)goto _LL2C3;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;}}_LL2C2: {
# 1972
struct _tuple18 _tmp50C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4F2);union Cyc_CfFlowInfo_FlowInfo _tmp50E;union Cyc_CfFlowInfo_FlowInfo _tmp50F;struct _tuple18 _tmp50D=_tmp50C;_tmp50E=_tmp50D.f1;_tmp50F=_tmp50D.f2;{
struct _tuple18 _tmp510=Cyc_NewControlFlow_anal_test(env,_tmp50E,_tmp4F3);union Cyc_CfFlowInfo_FlowInfo _tmp512;union Cyc_CfFlowInfo_FlowInfo _tmp513;struct _tuple18 _tmp511=_tmp510;_tmp512=_tmp511.f1;_tmp513=_tmp511.f2;{
struct _tuple18 _tmp91B;return(_tmp91B.f1=_tmp512,((_tmp91B.f2=Cyc_CfFlowInfo_join_flow(_tmp4EA,env->all_changed,_tmp50F,_tmp513),_tmp91B)));};};}_LL2C3: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4F4->tag != 7)goto _LL2C5;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;}}_LL2C4: {
# 1976
struct _tuple18 _tmp515=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4F5);union Cyc_CfFlowInfo_FlowInfo _tmp517;union Cyc_CfFlowInfo_FlowInfo _tmp518;struct _tuple18 _tmp516=_tmp515;_tmp517=_tmp516.f1;_tmp518=_tmp516.f2;{
struct _tuple18 _tmp519=Cyc_NewControlFlow_anal_test(env,_tmp518,_tmp4F6);union Cyc_CfFlowInfo_FlowInfo _tmp51B;union Cyc_CfFlowInfo_FlowInfo _tmp51C;struct _tuple18 _tmp51A=_tmp519;_tmp51B=_tmp51A.f1;_tmp51C=_tmp51A.f2;{
struct _tuple18 _tmp91C;return(_tmp91C.f1=Cyc_CfFlowInfo_join_flow(_tmp4EA,env->all_changed,_tmp517,_tmp51B),((_tmp91C.f2=_tmp51C,_tmp91C)));};};}_LL2C5: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4F7->tag != 8)goto _LL2C7;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;}}_LL2C6: {
# 1980
struct _tuple15 _tmp51E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4F8);union Cyc_CfFlowInfo_FlowInfo _tmp520;void*_tmp521;struct _tuple15 _tmp51F=_tmp51E;_tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp520,_tmp4F9);}_LL2C7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4FA->tag != 2)goto _LL2C9;else{if(_tmp4FA->f1 != Cyc_Absyn_Not)goto _LL2C9;if(_tmp4FA->f2 == 0)goto _LL2C9;_tmp4FB=(struct Cyc_Absyn_Exp*)(_tmp4FA->f2)->hd;if((_tmp4FA->f2)->tl != 0)goto _LL2C9;}}_LL2C8: {
# 1983
struct _tuple18 _tmp522=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4FB);union Cyc_CfFlowInfo_FlowInfo _tmp524;union Cyc_CfFlowInfo_FlowInfo _tmp525;struct _tuple18 _tmp523=_tmp522;_tmp524=_tmp523.f1;_tmp525=_tmp523.f2;{
struct _tuple18 _tmp91D;return(_tmp91D.f1=_tmp525,((_tmp91D.f2=_tmp524,_tmp91D)));};}_LL2C9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4FC->tag != 2)goto _LL2CB;else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FC->f2;}}_LL2CA:
# 1986
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4FD,_tmp4FE);_LL2CB:;_LL2CC: {
# 1990
struct _tuple15 _tmp527=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp529;void*_tmp52A;struct _tuple15 _tmp528=_tmp527;_tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp52B=_tmp529;struct Cyc_Dict_Dict _tmp52C;_LL2CE: if((_tmp52B.BottomFL).tag != 1)goto _LL2D0;_LL2CF: {
struct _tuple18 _tmp91E;return(_tmp91E.f1=_tmp529,((_tmp91E.f2=_tmp529,_tmp91E)));}_LL2D0: if((_tmp52B.ReachableFL).tag != 2)goto _LL2CD;_tmp52C=((struct _tuple14)(_tmp52B.ReachableFL).val).f1;_LL2D1: {
# 1994
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp52E=_tmp52A;struct Cyc_Absyn_Vardecl*_tmp530;void*_tmp531;enum Cyc_CfFlowInfo_InitLevel _tmp53A;_LL2D3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp52F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp52F->tag != 8)goto _LL2D5;else{_tmp530=_tmp52F->f1;_tmp531=(void*)_tmp52F->f2;}}_LL2D4:
 _tmp52A=_tmp531;locname=_tmp530;goto retry;_LL2D5: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp532=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp532->tag != 0)goto _LL2D7;}_LL2D6: {
struct _tuple18 _tmp91F;return(_tmp91F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp91F.f2=_tmp529,_tmp91F)));}_LL2D7: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp533=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp533->tag != 2)goto _LL2D9;}_LL2D8:
 goto _LL2DA;_LL2D9: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp534=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp534->tag != 1)goto _LL2DB;}_LL2DA:
 goto _LL2DC;_LL2DB: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp535=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp535->tag != 5)goto _LL2DD;}_LL2DC: {
struct _tuple18 _tmp920;return(_tmp920.f1=_tmp529,((_tmp920.f2=Cyc_CfFlowInfo_BottomFL(),_tmp920)));}_LL2DD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp536=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp536->tag != 3)goto _LL2DF;else{if(_tmp536->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DF;}}_LL2DE:
 goto _LL2E0;_LL2DF: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp537=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp537->tag != 4)goto _LL2E1;else{if(_tmp537->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2E1;}}_LL2E0:
 goto _LL2E2;_LL2E1: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp538=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp538->tag != 7)goto _LL2E3;}_LL2E2:
# 2004
{const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp923="expression may not be initialized",_tag_dyneither(_tmp923,sizeof(char),34))),_tag_dyneither(_tmp922,sizeof(void*),0)));}{
struct _tuple18 _tmp924;return(_tmp924.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp924.f2=Cyc_CfFlowInfo_BottomFL(),_tmp924)));};_LL2E3: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp539=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp539->tag != 3)goto _LL2E5;else{_tmp53A=_tmp539->f1;}}_LL2E4:
# 2007
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp529,e,_tmp53A,locname);_LL2E5: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp53B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp53B->tag != 4)goto _LL2E7;}_LL2E6: {
struct _tuple18 _tmp925;return(_tmp925.f1=_tmp529,((_tmp925.f2=_tmp529,_tmp925)));}_LL2E7: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp53C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp53C->tag != 6)goto _LL2D2;}_LL2E8: {
struct Cyc_Core_Impossible_exn_struct _tmp92B;const char*_tmp92A;struct Cyc_Core_Impossible_exn_struct*_tmp929;(int)_throw((void*)((_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929[0]=((_tmp92B.tag=Cyc_Core_Impossible,((_tmp92B.f1=((_tmp92A="anal_test",_tag_dyneither(_tmp92A,sizeof(char),10))),_tmp92B)))),_tmp929)))));}_LL2D2:;}}_LL2CD:;};}_LL2BE:;}struct _tuple26{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2015
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple26*ckenv,void*root,void*rval){
# 2017
unsigned int _tmp547;struct Cyc_NewControlFlow_AnalEnv*_tmp548;struct Cyc_Dict_Dict _tmp549;struct _tuple26*_tmp546=ckenv;_tmp547=_tmp546->f1;_tmp548=_tmp546->f2;_tmp549=_tmp546->f3;{
void*_tmp54A=root;struct Cyc_Absyn_Vardecl*_tmp54C;_LL2EA: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp54B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp54A;if(_tmp54B->tag != 0)goto _LL2EC;else{_tmp54C=_tmp54B->f1;}}_LL2EB:
# 2020
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp548->fenv)->r,_tmp54C->type)){
void*_tmp54D=rval;_LL2EF: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp54E->tag != 8)goto _LL2F1;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp54F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp54E->f2);if(_tmp54F->tag != 7)goto _LL2F1;}}_LL2F0:
 goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp550=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp550->tag != 8)goto _LL2F3;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp551=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp550->f2);if(_tmp551->tag != 0)goto _LL2F3;}}_LL2F2:
 goto _LL2F4;_LL2F3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp552=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp552->tag != 8)goto _LL2F5;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp553=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp552->f2);if(_tmp553->tag != 3)goto _LL2F5;else{if(_tmp553->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F5;}}}_LL2F4:
 goto _LL2F6;_LL2F5: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp554=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp554->tag != 7)goto _LL2F7;}_LL2F6:
 goto _LL2F8;_LL2F7: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp555=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp555->tag != 0)goto _LL2F9;}_LL2F8:
 goto _LL2FA;_LL2F9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp556=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54D;if(_tmp556->tag != 3)goto _LL2FB;else{if(_tmp556->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2FB;}}_LL2FA:
 goto _LL2EE;_LL2FB:;_LL2FC:
# 2029
{const char*_tmp92F;void*_tmp92E[1];struct Cyc_String_pa_PrintArg_struct _tmp92D;(_tmp92D.tag=0,((_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp54C->name)),((_tmp92E[0]=& _tmp92D,Cyc_Tcutil_warn(_tmp547,((_tmp92F="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp92F,sizeof(char),57))),_tag_dyneither(_tmp92E,sizeof(void*),1)))))));}
goto _LL2EE;_LL2EE:;}
# 2032
goto _LL2E9;_LL2EC:;_LL2ED:
 goto _LL2E9;_LL2E9:;};}
# 2037
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp55A=flow;struct Cyc_Dict_Dict _tmp55B;_LL2FE: if((_tmp55A.BottomFL).tag != 1)goto _LL300;_LL2FF:
 return;_LL300: if((_tmp55A.ReachableFL).tag != 2)goto _LL2FD;_tmp55B=((struct _tuple14)(_tmp55A.ReachableFL).val).f1;_LL301:
# 2041
{struct Cyc_List_List*_tmp55C=env->param_roots;for(0;_tmp55C != 0;_tmp55C=_tmp55C->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp55B,Cyc_CfFlowInfo_lookup_place(_tmp55B,(struct Cyc_CfFlowInfo_Place*)_tmp55C->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp932;void*_tmp931;(_tmp931=0,Cyc_CfFlowInfo_aerr(loc,((_tmp932="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp932,sizeof(char),76))),_tag_dyneither(_tmp931,sizeof(void*),0)));}}}
# 2045
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple26 _tmp933;struct _tuple26 _tmp55F=(_tmp933.f1=loc,((_tmp933.f2=env,((_tmp933.f3=_tmp55B,_tmp933)))));
((void(*)(void(*f)(struct _tuple26*,void*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp55F,_tmp55B);}
# 2049
return;_LL2FD:;}
# 2053
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2056
struct Cyc_List_List*_tmp561=0;
struct Cyc_List_List*_tmp562=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple23*_tmp563=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp565;struct Cyc_Absyn_Exp*_tmp566;struct _tuple23*_tmp564=_tmp563;_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;
if((_tmp565 != 0  && 
# 2063
Cyc_Tcutil_is_noalias_pointer((*_tmp565)->type,0)) && _tmp566 != 0){
# 2065
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp936;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp935;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp567=(_tmp935=_region_malloc(frgn,sizeof(*_tmp935)),((_tmp935[0]=((_tmp936.tag=0,((_tmp936.f1=*_tmp565,_tmp936)))),_tmp935)));
struct Cyc_CfFlowInfo_Place*_tmp937;struct Cyc_CfFlowInfo_Place*_tmp568=(_tmp937=_region_malloc(frgn,sizeof(*_tmp937)),((_tmp937->root=(void*)_tmp567,((_tmp937->fields=0,_tmp937)))));
{struct Cyc_List_List*_tmp938;_tmp561=((_tmp938=_region_malloc(frgn,sizeof(*_tmp938)),((_tmp938->hd=_tmp568,((_tmp938->tl=_tmp561,_tmp938))))));}{
struct Cyc_List_List*_tmp939;_tmp562=((_tmp939=_region_malloc(frgn,sizeof(*_tmp939)),((_tmp939->hd=_tmp566,((_tmp939->tl=_tmp562,_tmp939))))));};}}}{
# 2071
struct _tuple1 _tmp93A;return(_tmp93A.f1=_tmp561,((_tmp93A.f2=_tmp562,_tmp93A)));};}
# 2080
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2085
{union Cyc_CfFlowInfo_FlowInfo _tmp56F=flow;_LL303: if((_tmp56F.BottomFL).tag != 1)goto _LL305;_LL304:
 return flow;_LL305:;_LL306:
 goto _LL302;_LL302:;}{
# 2089
int _tmp570=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp571=Cyc_Absyn_exp_unsigned_one;
int _tmp572=0;
int _tmp573=1;
# 2096
{struct Cyc_List_List*_tmp574=consumed_vals;for(0;_tmp574 != 0;
(_tmp574=_tmp574->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp575=flow;struct Cyc_Dict_Dict _tmp576;_LL308: if((_tmp575.ReachableFL).tag != 2)goto _LL30A;_tmp576=((struct _tuple14)(_tmp575.ReachableFL).val).f1;_LL309: {
# 2106
void*_tmp577=Cyc_CfFlowInfo_lookup_place(_tmp576,(struct Cyc_CfFlowInfo_Place*)_tmp574->hd);
struct Cyc_CfFlowInfo_Place*_tmp578=(struct Cyc_CfFlowInfo_Place*)_tmp574->hd;void*_tmp57A;struct Cyc_CfFlowInfo_Place*_tmp579=_tmp578;_tmp57A=_tmp579->root;
{struct _tuple0 _tmp93B;struct _tuple0 _tmp57B=(_tmp93B.f1=_tmp57A,((_tmp93B.f2=_tmp577,_tmp93B)));struct _tuple0 _tmp57C=_tmp57B;struct Cyc_Absyn_Vardecl*_tmp57E;struct Cyc_Absyn_Vardecl*_tmp580;void*_tmp581;struct Cyc_Absyn_Vardecl*_tmp583;_LL30D:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp57C.f1;if(_tmp57D->tag != 0)goto _LL30F;else{_tmp57E=_tmp57D->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp57F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp57C.f2;if(_tmp57F->tag != 8)goto _LL30F;else{_tmp580=_tmp57F->f1;_tmp581=(void*)_tmp57F->f2;}};_LL30E:
# 2110
 if(_tmp57E == _tmp580){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp571,_tmp573,_tmp581,& _tmp572)){
if(!_tmp570){
const char*_tmp93F;void*_tmp93E[1];struct Cyc_String_pa_PrintArg_struct _tmp93D;(_tmp93D.tag=0,((_tmp93D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2115
Cyc_Absynpp_qvar2string(_tmp57E->name)),((_tmp93E[0]=& _tmp93D,Cyc_CfFlowInfo_aerr(loc,((_tmp93F="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp93F,sizeof(char),58))),_tag_dyneither(_tmp93E,sizeof(void*),1)))))));}}else{
# 2117
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp576,_tmp581)!= Cyc_CfFlowInfo_AllIL  && !_tmp570){
const char*_tmp943;void*_tmp942[1];struct Cyc_String_pa_PrintArg_struct _tmp941;(_tmp941.tag=0,((_tmp941.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_qvar2string(_tmp57E->name)),((_tmp942[0]=& _tmp941,Cyc_CfFlowInfo_aerr(loc,((_tmp943="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp943,sizeof(char),84))),_tag_dyneither(_tmp942,sizeof(void*),1)))))));}else{
# 2122
if(_tmp570){
# 2128
struct _tuple17 _tmp58A=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp58C;union Cyc_CfFlowInfo_AbsLVal _tmp58D;struct _tuple17 _tmp58B=_tmp58A;_tmp58C=_tmp58B.f1;_tmp58D=_tmp58B.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp58E=_tmp58D;struct Cyc_CfFlowInfo_Place*_tmp58F;_LL314: if((_tmp58E.PlaceL).tag != 1)goto _LL316;_tmp58F=(struct Cyc_CfFlowInfo_Place*)(_tmp58E.PlaceL).val;_LL315: {
# 2132
void*_tmp590=Cyc_CfFlowInfo_lookup_place(_tmp576,_tmp58F);
_tmp590=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp590);{
# 2140
struct _tuple15 _tmp945;struct _tuple15 _tmp944;struct _tuple15 _tmp591=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2143
(_tmp944.f1=_tmp58C,((_tmp944.f2=_tmp590,_tmp944)))),(
(_tmp945.f1=_tmp58C,((_tmp945.f2=_tmp581,_tmp945)))));
# 2140
union Cyc_CfFlowInfo_FlowInfo _tmp593;void*_tmp594;struct _tuple15 _tmp592=_tmp591;_tmp593=_tmp592.f1;_tmp594=_tmp592.f2;
# 2145
{union Cyc_CfFlowInfo_FlowInfo _tmp595=_tmp593;struct Cyc_Dict_Dict _tmp596;struct Cyc_List_List*_tmp597;_LL319: if((_tmp595.ReachableFL).tag != 2)goto _LL31B;_tmp596=((struct _tuple14)(_tmp595.ReachableFL).val).f1;_tmp597=((struct _tuple14)(_tmp595.ReachableFL).val).f2;_LL31A:
# 2147
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp596,env->all_changed,_tmp58F,_tmp594),_tmp597);
# 2151
goto _LL318;_LL31B:;_LL31C: {
# 2153
const char*_tmp948;void*_tmp947;(_tmp947=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp948="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp948,sizeof(char),42))),_tag_dyneither(_tmp947,sizeof(void*),0)));}_LL318:;}
# 2155
goto _LL313;};}_LL316:;_LL317:
# 2160
 goto _LL313;_LL313:;};}}}}else{
# 2164
_tmp583=_tmp57E;goto _LL310;}
goto _LL30C;_LL30F: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp582=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp57C.f1;if(_tmp582->tag != 0)goto _LL311;else{_tmp583=_tmp582->f1;}}_LL310:
# 2167
 if(!_tmp570){
const char*_tmp94C;void*_tmp94B[1];struct Cyc_String_pa_PrintArg_struct _tmp94A;(_tmp94A.tag=0,((_tmp94A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2170
Cyc_Absynpp_qvar2string(_tmp583->name)),((_tmp94B[0]=& _tmp94A,Cyc_CfFlowInfo_aerr(loc,((_tmp94C="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp94C,sizeof(char),85))),_tag_dyneither(_tmp94B,sizeof(void*),1)))))));}
goto _LL30C;_LL311:;_LL312: {
# 2173
const char*_tmp94F;void*_tmp94E;(_tmp94E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp94F="unconsume_params: root is not a varroot",_tag_dyneither(_tmp94F,sizeof(char),40))),_tag_dyneither(_tmp94E,sizeof(void*),0)));}_LL30C:;}
# 2175
goto _LL307;}_LL30A: if((_tmp575.BottomFL).tag != 1)goto _LL307;_LL30B: {
# 2177
const char*_tmp952;void*_tmp951;(_tmp951=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp952="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp952,sizeof(char),38))),_tag_dyneither(_tmp951,sizeof(void*),0)));}_LL307:;}}
# 2180
return flow;};}struct _tuple27{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2183
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2186
struct Cyc_CfFlowInfo_FlowEnv*_tmp5A4=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5A5=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp5A7;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Stmt*_tmp5A9;unsigned int _tmp5AA;struct Cyc_Absyn_Switch_clause*_tmp5A6=_tmp5A5;_tmp5A7=_tmp5A6->pat_vars;_tmp5A8=_tmp5A6->where_clause;_tmp5A9=_tmp5A6->body;_tmp5AA=_tmp5A6->loc;{
struct _tuple1 _tmp5AB=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A7))->v);struct Cyc_List_List*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct _tuple1 _tmp5AC=_tmp5AB;_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp5A7->v,_tmp5AD != 0,_tmp5AA,exp_loc);
# 2194
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp5AF=env->unique_pat_vars;
{struct _tuple27*_tmp955;struct Cyc_List_List*_tmp954;env->unique_pat_vars=(
(_tmp954=_region_malloc(env->r,sizeof(*_tmp954)),((_tmp954->hd=((_tmp955=_region_malloc(env->r,sizeof(*_tmp955)),((_tmp955->f1=_tmp5A9,((_tmp955->f2=_tmp5AD,((_tmp955->f3=_tmp5AE,_tmp955)))))))),((_tmp954->tl=_tmp5AF,_tmp954))))));}
# 2199
if(_tmp5A8 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5A8;
struct _tuple18 _tmp5B2=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp5B4;union Cyc_CfFlowInfo_FlowInfo _tmp5B5;struct _tuple18 _tmp5B3=_tmp5B2;_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;
inflow=_tmp5B5;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5B4,_tmp5A9,0);}else{
# 2205
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5A9,0);}
# 2207
env->unique_pat_vars=_tmp5AF;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=clause_outflow;_LL31E: if((_tmp5B6.BottomFL).tag != 1)goto _LL320;_LL31F:
 goto _LL31D;_LL320:;_LL321:
# 2212
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5AD,_tmp5AE,clause_outflow,_tmp5AA);
# 2214
if(scs->tl == 0)
return clause_outflow;else{
# 2219
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp958;void*_tmp957;(_tmp957=0,Cyc_CfFlowInfo_aerr(_tmp5A9->loc,((_tmp958="switch clause may implicitly fallthru",_tag_dyneither(_tmp958,sizeof(char),38))),_tag_dyneither(_tmp957,sizeof(void*),0)));}else{
# 2222
const char*_tmp95B;void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_warn(_tmp5A9->loc,((_tmp95B="switch clause may implicitly fallthru",_tag_dyneither(_tmp95B,sizeof(char),38))),_tag_dyneither(_tmp95A,sizeof(void*),0)));}
# 2224
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2226
goto _LL31D;_LL31D:;};};};}
# 2229
return Cyc_CfFlowInfo_BottomFL();}struct _tuple28{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2234
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple28*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2236
struct Cyc_NewControlFlow_AnalEnv*_tmp5BC;struct Cyc_Dict_Dict _tmp5BD;unsigned int _tmp5BE;struct _tuple28*_tmp5BB=vdenv;_tmp5BC=_tmp5BB->f1;_tmp5BD=_tmp5BB->f2;_tmp5BE=_tmp5BB->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5BC->fenv)->r,vd->type)){
# 2239
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp95C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp95C.tag=0,((_tmp95C.f1=vd,_tmp95C)));
# 2241
void*_tmp5BF=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5BD,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp5C0=_tmp5BF;_LL323: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C1->tag != 8)goto _LL325;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5C2=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C1->f2);if(_tmp5C2->tag != 7)goto _LL325;}}_LL324:
 goto _LL326;_LL325: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C3->tag != 8)goto _LL327;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5C4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C3->f2);if(_tmp5C4->tag != 0)goto _LL327;}}_LL326:
 goto _LL328;_LL327: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C5->tag != 8)goto _LL329;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp5C5->f2);if(_tmp5C6->tag != 3)goto _LL329;else{if(_tmp5C6->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL329;}}}_LL328:
 goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5C7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C7->tag != 7)goto _LL32B;}_LL32A:
 goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5C8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C8->tag != 0)goto _LL32D;}_LL32C:
 goto _LL32E;_LL32D: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5C9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5C0;if(_tmp5C9->tag != 3)goto _LL32F;else{if(_tmp5C9->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32F;}}_LL32E:
 goto _LL322;_LL32F:;_LL330:
# 2250
{const char*_tmp960;void*_tmp95F[1];struct Cyc_String_pa_PrintArg_struct _tmp95E;(_tmp95E.tag=0,((_tmp95E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp95F[0]=& _tmp95E,Cyc_Tcutil_warn(_tmp5BE,((_tmp960="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp960,sizeof(char),68))),_tag_dyneither(_tmp95F,sizeof(void*),1)))))));}
# 2252
goto _LL322;_LL322:;}}
# 2257
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2259
{union Cyc_CfFlowInfo_FlowInfo _tmp5CE=inflow;struct Cyc_Dict_Dict _tmp5CF;_LL332: if((_tmp5CE.ReachableFL).tag != 2)goto _LL334;_tmp5CF=((struct _tuple14)(_tmp5CE.ReachableFL).val).f1;_LL333: {
# 2261
struct _tuple28 _tmp961;struct _tuple28 _tmp5D0=(_tmp961.f1=env,((_tmp961.f2=_tmp5CF,((_tmp961.f3=decl->loc,_tmp961)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5D1=env->fenv;
{void*_tmp5D2=decl->r;void*_tmp5D3=_tmp5D2;struct Cyc_Absyn_Vardecl*_tmp5D5;struct Cyc_List_List*_tmp5D7;struct Cyc_List_List*_tmp5D9;_LL337: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5D4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D4->tag != 0)goto _LL339;else{_tmp5D5=_tmp5D4->f1;}}_LL338:
# 2265
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5D0,_tmp5D5);
goto _LL336;_LL339: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5D6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D6->tag != 2)goto _LL33B;else{if(_tmp5D6->f2 == 0)goto _LL33B;_tmp5D7=(struct Cyc_List_List*)(_tmp5D6->f2)->v;}}_LL33A: {
# 2268
struct _tuple1 _tmp5DA=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp5D7);struct Cyc_List_List*_tmp5DC;struct _tuple1 _tmp5DB=_tmp5DA;_tmp5DC=_tmp5DB.f1;{
struct Cyc_List_List*_tmp5DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp5DC);
_tmp5D9=_tmp5DD;goto _LL33C;};}_LL33B: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5D8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5D3;if(_tmp5D8->tag != 3)goto _LL33D;else{_tmp5D9=_tmp5D8->f1;}}_LL33C:
# 2272
((void(*)(void(*f)(struct _tuple28*,struct Cyc_Absyn_Vardecl*),struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5D0,_tmp5D9);
goto _LL336;_LL33D:;_LL33E:
 goto _LL336;_LL336:;}
# 2276
goto _LL331;}_LL334:;_LL335:
 goto _LL331;_LL331:;}
# 2279
return;}
# 2285
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2288
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple27*_tmp5DF=(struct _tuple27*)x->hd;struct Cyc_Absyn_Stmt*_tmp5E1;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp5E3;struct _tuple27*_tmp5E0=_tmp5DF;_tmp5E1=_tmp5E0->f1;_tmp5E2=_tmp5E0->f2;_tmp5E3=_tmp5E0->f3;
keep_going=0;{
# 2296
struct Cyc_Absyn_Stmt*_tmp5E4=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp5E4 != _tmp5E1){
struct Cyc_Absyn_Stmt*_tmp5E5=(Cyc_NewControlFlow_get_stmt_annot(_tmp5E4))->encloser;
if(_tmp5E5 == _tmp5E4){
# 2302
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E2,_tmp5E3,inflow,dest->loc);
keep_going=1;
break;}
# 2306
_tmp5E4=_tmp5E5;}};}}
# 2309
return inflow;}
# 2315
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple16*rval_opt){
# 2318
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple19 _tmp5E6=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5E8;union Cyc_CfFlowInfo_FlowInfo*_tmp5E9;struct _tuple19 _tmp5E7=_tmp5E6;_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;
inflow=*_tmp5E9;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp5EA=env->fenv;
# 2325
void*_tmp5EB=s->r;void*_tmp5EC=_tmp5EB;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Stmt*_tmp602;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Stmt*_tmp606;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_Absyn_Stmt*_tmp608;struct Cyc_Absyn_Stmt*_tmp609;struct Cyc_List_List*_tmp60B;struct Cyc_Absyn_Switch_clause*_tmp60C;struct Cyc_Absyn_Stmt*_tmp60E;struct Cyc_Absyn_Stmt*_tmp610;struct Cyc_Absyn_Stmt*_tmp612;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_List_List*_tmp615;struct Cyc_Absyn_Stmt*_tmp617;struct Cyc_List_List*_tmp618;struct Cyc_List_List*_tmp61B;struct Cyc_Absyn_Exp*_tmp61C;unsigned int _tmp61D;struct Cyc_Absyn_Stmt*_tmp61E;struct Cyc_Absyn_Tvar*_tmp621;struct Cyc_Absyn_Vardecl*_tmp622;struct Cyc_Absyn_Stmt*_tmp623;struct Cyc_Absyn_Decl*_tmp625;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Stmt*_tmp628;struct Cyc_Absyn_Exp*_tmp62A;_LL340: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp5ED=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5ED->tag != 0)goto _LL342;}_LL341:
 return inflow;_LL342: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5EE=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5EE->tag != 6)goto _LL344;else{if(_tmp5EE->f1 != 0)goto _LL344;}}_LL343:
# 2331
{union Cyc_CfFlowInfo_FlowInfo _tmp62B=inflow;_LL36D: if((_tmp62B.ReachableFL).tag != 2)goto _LL36F;_LL36E:
# 2333
{void*_tmp62C=Cyc_Tcutil_compress(env->return_type);void*_tmp62D=_tmp62C;_LL372: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp62E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp62D;if(_tmp62E->tag != 0)goto _LL374;}_LL373:
 goto _LL371;_LL374: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp62F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp62D;if(_tmp62F->tag != 7)goto _LL376;}_LL375:
 goto _LL377;_LL376: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp630=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp62D;if(_tmp630->tag != 6)goto _LL378;}_LL377:
# 2337
{const char*_tmp964;void*_tmp963;(_tmp963=0,Cyc_Tcutil_warn(s->loc,((_tmp964="break may cause function not to return a value",_tag_dyneither(_tmp964,sizeof(char),47))),_tag_dyneither(_tmp963,sizeof(void*),0)));}
goto _LL371;_LL378:;_LL379:
# 2340
{const char*_tmp967;void*_tmp966;(_tmp966=0,Cyc_Tcutil_terr(s->loc,((_tmp967="break may cause function not to return a value",_tag_dyneither(_tmp967,sizeof(char),47))),_tag_dyneither(_tmp966,sizeof(void*),0)));}
goto _LL371;_LL371:;}
# 2343
goto _LL36C;_LL36F:;_LL370:
 goto _LL36C;_LL36C:;}
# 2346
goto _LL345;_LL344: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5EF=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5EF->tag != 3)goto _LL346;else{if(_tmp5EF->f1 != 0)goto _LL346;}}_LL345:
# 2348
 if(env->noreturn){
const char*_tmp96A;void*_tmp969;(_tmp969=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp96A="`noreturn' function might return",_tag_dyneither(_tmp96A,sizeof(char),33))),_tag_dyneither(_tmp969,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL346: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp5F0=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5F0->tag != 3)goto _LL348;else{_tmp5F1=_tmp5F0->f1;}}_LL347:
# 2354
 if(env->noreturn){
const char*_tmp96D;void*_tmp96C;(_tmp96C=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp96D="`noreturn' function might return",_tag_dyneither(_tmp96D,sizeof(char),33))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}{
struct _tuple15 _tmp639=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F1));union Cyc_CfFlowInfo_FlowInfo _tmp63B;void*_tmp63C;struct _tuple15 _tmp63A=_tmp639;_tmp63B=_tmp63A.f1;_tmp63C=_tmp63A.f2;
_tmp63B=Cyc_NewControlFlow_use_Rval(env,_tmp5F1->loc,_tmp63B,_tmp63C);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp63B);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp63B,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL348: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp5F2=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5F2->tag != 1)goto _LL34A;else{_tmp5F3=_tmp5F2->f1;}}_LL349: {
# 2363
struct _tuple16*_tmp63D=rval_opt;void**_tmp63E;int _tmp63F;_LL37B: if(_tmp63D == 0)goto _LL37D;_tmp63E=(void**)& _tmp63D->f1;_tmp63F=_tmp63D->f2;_LL37C: {
# 2365
struct _tuple15 _tmp640=Cyc_NewControlFlow_anal_Rexp(env,_tmp63F,inflow,_tmp5F3);union Cyc_CfFlowInfo_FlowInfo _tmp642;void*_tmp643;struct _tuple15 _tmp641=_tmp640;_tmp642=_tmp641.f1;_tmp643=_tmp641.f2;
*_tmp63E=_tmp643;
return _tmp642;}_LL37D: if(_tmp63D != 0)goto _LL37A;_LL37E:
# 2369
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5F3)).f1;_LL37A:;}_LL34A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp5F4=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5F4->tag != 2)goto _LL34C;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;}}_LL34B:
# 2373
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5F5,0),_tmp5F6,rval_opt);_LL34C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp5F7=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5F7->tag != 4)goto _LL34E;else{_tmp5F8=_tmp5F7->f1;_tmp5F9=_tmp5F7->f2;_tmp5FA=_tmp5F7->f3;}}_LL34D: {
# 2376
struct _tuple18 _tmp644=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5F8);union Cyc_CfFlowInfo_FlowInfo _tmp646;union Cyc_CfFlowInfo_FlowInfo _tmp647;struct _tuple18 _tmp645=_tmp644;_tmp646=_tmp645.f1;_tmp647=_tmp645.f2;{
# 2383
union Cyc_CfFlowInfo_FlowInfo _tmp648=Cyc_NewControlFlow_anal_stmt(env,_tmp647,_tmp5FA,0);
union Cyc_CfFlowInfo_FlowInfo _tmp649=Cyc_NewControlFlow_anal_stmt(env,_tmp646,_tmp5F9,0);
return Cyc_CfFlowInfo_join_flow(_tmp5EA,env->all_changed,_tmp649,_tmp648);};}_LL34E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp5FB=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5FB->tag != 5)goto _LL350;else{_tmp5FC=(_tmp5FB->f1).f1;_tmp5FD=(_tmp5FB->f1).f2;_tmp5FE=_tmp5FB->f2;}}_LL34F: {
# 2391
struct _tuple19 _tmp64A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5FD);union Cyc_CfFlowInfo_FlowInfo*_tmp64C;struct _tuple19 _tmp64B=_tmp64A;_tmp64C=_tmp64B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp64D=*_tmp64C;
struct _tuple18 _tmp64E=Cyc_NewControlFlow_anal_test(env,_tmp64D,_tmp5FC);union Cyc_CfFlowInfo_FlowInfo _tmp650;union Cyc_CfFlowInfo_FlowInfo _tmp651;struct _tuple18 _tmp64F=_tmp64E;_tmp650=_tmp64F.f1;_tmp651=_tmp64F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp652=Cyc_NewControlFlow_anal_stmt(env,_tmp650,_tmp5FE,0);
Cyc_NewControlFlow_update_flow(env,_tmp5FD,_tmp652);
return _tmp651;};};}_LL350: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp5FF=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp5FF->tag != 14)goto _LL352;else{_tmp600=_tmp5FF->f1;_tmp601=(_tmp5FF->f2).f1;_tmp602=(_tmp5FF->f2).f2;}}_LL351: {
# 2401
union Cyc_CfFlowInfo_FlowInfo _tmp653=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp600,0);
struct _tuple19 _tmp654=Cyc_NewControlFlow_pre_stmt_check(env,_tmp653,_tmp602);union Cyc_CfFlowInfo_FlowInfo*_tmp656;struct _tuple19 _tmp655=_tmp654;_tmp656=_tmp655.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp657=*_tmp656;
struct _tuple18 _tmp658=Cyc_NewControlFlow_anal_test(env,_tmp657,_tmp601);union Cyc_CfFlowInfo_FlowInfo _tmp65A;union Cyc_CfFlowInfo_FlowInfo _tmp65B;struct _tuple18 _tmp659=_tmp658;_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;
Cyc_NewControlFlow_update_flow(env,_tmp600,_tmp65A);
return _tmp65B;};}_LL352: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp603=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp603->tag != 9)goto _LL354;else{_tmp604=_tmp603->f1;_tmp605=(_tmp603->f2).f1;_tmp606=(_tmp603->f2).f2;_tmp607=(_tmp603->f3).f1;_tmp608=(_tmp603->f3).f2;_tmp609=_tmp603->f4;}}_LL353: {
# 2410
union Cyc_CfFlowInfo_FlowInfo _tmp65C=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp604)).f1;
struct _tuple19 _tmp65D=Cyc_NewControlFlow_pre_stmt_check(env,_tmp65C,_tmp606);union Cyc_CfFlowInfo_FlowInfo*_tmp65F;struct _tuple19 _tmp65E=_tmp65D;_tmp65F=_tmp65E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp660=*_tmp65F;
struct _tuple18 _tmp661=Cyc_NewControlFlow_anal_test(env,_tmp660,_tmp605);union Cyc_CfFlowInfo_FlowInfo _tmp663;union Cyc_CfFlowInfo_FlowInfo _tmp664;struct _tuple18 _tmp662=_tmp661;_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp665=Cyc_NewControlFlow_anal_stmt(env,_tmp663,_tmp609,0);
struct _tuple19 _tmp666=Cyc_NewControlFlow_pre_stmt_check(env,_tmp665,_tmp608);union Cyc_CfFlowInfo_FlowInfo*_tmp668;struct _tuple19 _tmp667=_tmp666;_tmp668=_tmp667.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp669=*_tmp668;
union Cyc_CfFlowInfo_FlowInfo _tmp66A=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp669,_tmp607)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp606,_tmp66A);
return _tmp664;};};};}_LL354: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp60A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp60A->tag != 11)goto _LL356;else{_tmp60B=_tmp60A->f1;if(_tmp60A->f2 == 0)goto _LL356;_tmp60C=*_tmp60A->f2;}}_LL355: {
# 2422
struct _RegionHandle _tmp66B=_new_region("temp");struct _RegionHandle*temp=& _tmp66B;_push_region(temp);
{struct _tuple22 _tmp66C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp60B,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp66E;struct Cyc_List_List*_tmp66F;struct _tuple22 _tmp66D=_tmp66C;_tmp66E=_tmp66D.f1;_tmp66F=_tmp66D.f2;
# 2425
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp60C->body);{
# 2427
struct Cyc_List_List*_tmp670=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60C->pat_vars))->v)).f1);
_tmp66E=Cyc_NewControlFlow_add_vars(_tmp5EA,_tmp66E,_tmp670,_tmp5EA->unknown_all,s->loc,0);
# 2430
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60C->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple23*_tmp671=(struct _tuple23*)x->hd;struct Cyc_Absyn_Vardecl**_tmp673;struct Cyc_Absyn_Exp*_tmp674;struct _tuple23*_tmp672=_tmp671;_tmp673=_tmp672->f1;_tmp674=_tmp672->f2;
if(_tmp673 != 0){
_tmp66E=Cyc_NewControlFlow_do_initialize_var(_tmp5EA,env,_tmp66E,*_tmp673,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60B))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp66F))->hd,s->loc);
_tmp66F=_tmp66F->tl;
_tmp60B=_tmp60B->tl;}}}
# 2438
Cyc_NewControlFlow_update_flow(env,_tmp60C->body,_tmp66E);{
union Cyc_CfFlowInfo_FlowInfo _tmp675=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp675;};};}
# 2423
;_pop_region(temp);}_LL356: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp60D=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp60D->tag != 6)goto _LL358;else{_tmp60E=_tmp60D->f1;}}_LL357:
# 2441
 _tmp610=_tmp60E;goto _LL359;_LL358: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp60F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp60F->tag != 7)goto _LL35A;else{_tmp610=_tmp60F->f1;}}_LL359:
 _tmp612=_tmp610;goto _LL35B;_LL35A: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp611=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp611->tag != 8)goto _LL35C;else{_tmp612=_tmp611->f2;}}_LL35B:
# 2446
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp676=_tmp5E8->encloser;
struct Cyc_Absyn_Stmt*_tmp677=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp612)))->encloser;
while(_tmp677 != _tmp676){
struct Cyc_Absyn_Stmt*_tmp678=(Cyc_NewControlFlow_get_stmt_annot(_tmp676))->encloser;
if(_tmp678 == _tmp676){
{const char*_tmp970;void*_tmp96F;(_tmp96F=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp970="goto enters local scope or exception handler",_tag_dyneither(_tmp970,sizeof(char),45))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}
break;}
# 2455
_tmp676=_tmp678;}}
# 2459
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp612));
# 2461
Cyc_NewControlFlow_update_flow(env,_tmp612,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL35C: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp613=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp613->tag != 10)goto _LL35E;else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;}}_LL35D:
# 2467
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp615,_tmp614->loc);_LL35E: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp616=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp616->tag != 15)goto _LL360;else{_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;}}_LL35F: {
# 2472
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp617,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2480
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2483
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp618,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp67B=scs_outflow;_LL380: if((_tmp67B.BottomFL).tag != 1)goto _LL382;_LL381:
 goto _LL37F;_LL382:;_LL383: {
const char*_tmp973;void*_tmp972;(_tmp972=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp973="last catch clause may implicitly fallthru",_tag_dyneither(_tmp973,sizeof(char),42))),_tag_dyneither(_tmp972,sizeof(void*),0)));}_LL37F:;}
# 2489
outflow=s1_outflow;
# 2491
return outflow;};};}_LL360: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp619=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp619->tag != 12)goto _LL362;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp61A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp619->f1)->r;if(_tmp61A->tag != 2)goto _LL362;else{if(_tmp61A->f2 == 0)goto _LL362;_tmp61B=(struct Cyc_List_List*)(_tmp61A->f2)->v;_tmp61C=_tmp61A->f3;}}_tmp61D=(_tmp619->f1)->loc;_tmp61E=_tmp619->f2;}}_LL361: {
# 2501
struct _tuple1 _tmp67E=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp61B);struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp681;struct _tuple1 _tmp67F=_tmp67E;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp5EA,env,inflow,_tmp61B,_tmp680 != 0,_tmp61D,_tmp61C->loc);{
struct Cyc_List_List*_tmp682=env->unique_pat_vars;
{struct _tuple27*_tmp976;struct Cyc_List_List*_tmp975;env->unique_pat_vars=(
(_tmp975=_region_malloc(env->r,sizeof(*_tmp975)),((_tmp975->hd=((_tmp976=_region_malloc(env->r,sizeof(*_tmp976)),((_tmp976->f1=_tmp61E,((_tmp976->f2=_tmp680,((_tmp976->f3=_tmp681,_tmp976)))))))),((_tmp975->tl=_tmp682,_tmp975))))));}
# 2509
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp61E,rval_opt);
env->unique_pat_vars=_tmp682;
# 2513
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp680,_tmp681,inflow,_tmp61D);
# 2517
return inflow;};}_LL362: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp61F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp61F->tag != 12)goto _LL364;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp620=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp61F->f1)->r;if(_tmp620->tag != 5)goto _LL364;else{_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;}}_tmp623=_tmp61F->f2;}}_LL363: {
# 2521
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp622->initializer);
struct _tuple17 _tmp685=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp687;union Cyc_CfFlowInfo_AbsLVal _tmp688;struct _tuple17 _tmp686=_tmp685;_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;{
# 2526
struct _tuple17 _tmp977;struct _tuple17 _tmp689=(_tmp977.f1=_tmp687,((_tmp977.f2=_tmp688,_tmp977)));struct _tuple17 _tmp68A=_tmp689;struct Cyc_Dict_Dict _tmp68B;struct Cyc_List_List*_tmp68C;struct Cyc_CfFlowInfo_Place*_tmp68D;_LL385: if(((_tmp68A.f1).ReachableFL).tag != 2)goto _LL387;_tmp68B=((struct _tuple14)((_tmp68A.f1).ReachableFL).val).f1;_tmp68C=((struct _tuple14)((_tmp68A.f1).ReachableFL).val).f2;if(((_tmp68A.f2).PlaceL).tag != 1)goto _LL387;_tmp68D=(struct Cyc_CfFlowInfo_Place*)((_tmp68A.f2).PlaceL).val;_LL386: {
# 2528
void*_tmp68E=Cyc_CfFlowInfo_lookup_place(_tmp68B,_tmp68D);
void*t=(void*)_check_null(e->topt);
void*_tmp68F=Cyc_CfFlowInfo_make_unique_consumed(_tmp5EA,t,e,env->iteration_num,_tmp68E);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp97A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp979;_tmp68F=(void*)((_tmp979=_region_malloc(_tmp5EA->r,sizeof(*_tmp979)),((_tmp979[0]=((_tmp97A.tag=8,((_tmp97A.f1=_tmp622,((_tmp97A.f2=_tmp68F,_tmp97A)))))),_tmp979))));}
_tmp68B=Cyc_CfFlowInfo_assign_place(_tmp5EA,e->loc,_tmp68B,env->all_changed,_tmp68D,_tmp68F);
_tmp687=Cyc_CfFlowInfo_ReachableFL(_tmp68B,_tmp68C);{
# 2541
void*leaf;
{enum Cyc_CfFlowInfo_InitLevel _tmp692=Cyc_CfFlowInfo_initlevel(_tmp5EA,_tmp68B,_tmp68E);switch(_tmp692){case Cyc_CfFlowInfo_AllIL: _LL38B:
 leaf=_tmp5EA->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL38C:
 leaf=_tmp5EA->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL38D:
 leaf=_tmp5EA->unknown_this;break;}}{
# 2547
void*_tmp693=Cyc_CfFlowInfo_typ_to_absrval(_tmp5EA,t,0,leaf);
_tmp687=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp687,_tmp68E);{
struct Cyc_List_List _tmp97B;struct Cyc_List_List _tmp694=(_tmp97B.hd=_tmp622,((_tmp97B.tl=0,_tmp97B)));
# 2551
_tmp687=Cyc_NewControlFlow_add_vars(_tmp5EA,_tmp687,& _tmp694,_tmp5EA->unknown_all,s->loc,0);
# 2555
_tmp687=Cyc_NewControlFlow_anal_stmt(env,_tmp687,_tmp623,rval_opt);{
# 2557
union Cyc_CfFlowInfo_FlowInfo _tmp695=_tmp687;struct Cyc_Dict_Dict _tmp696;struct Cyc_List_List*_tmp697;_LL390: if((_tmp695.ReachableFL).tag != 2)goto _LL392;_tmp696=((struct _tuple14)(_tmp695.ReachableFL).val).f1;_tmp697=((struct _tuple14)(_tmp695.ReachableFL).val).f2;_LL391:
# 2560
{void*_tmp698=Cyc_CfFlowInfo_lookup_place(_tmp696,_tmp68D);void*_tmp699=_tmp698;struct Cyc_Absyn_Vardecl*_tmp69B;void*_tmp69C;_LL395: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp699;if(_tmp69A->tag != 8)goto _LL397;else{_tmp69B=_tmp69A->f1;_tmp69C=(void*)_tmp69A->f2;}}_LL396:
# 2562
 if(_tmp69B != _tmp622)goto _LL398;{
# 2565
struct _tuple15 _tmp97D;struct _tuple15 _tmp97C;struct _tuple15 _tmp69D=Cyc_CfFlowInfo_join_flow_and_rval(_tmp5EA,env->all_changed,(
(_tmp97C.f1=_tmp687,((_tmp97C.f2=_tmp68E,_tmp97C)))),((_tmp97D.f1=_tmp687,((_tmp97D.f2=_tmp69C,_tmp97D)))));
# 2565
void*_tmp69F;struct _tuple15 _tmp69E=_tmp69D;_tmp69F=_tmp69E.f2;
# 2567
_tmp693=_tmp69F;
goto _LL394;};_LL397:;_LL398:
# 2570
{const char*_tmp980;void*_tmp97F;(_tmp97F=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp980="aliased location was overwritten",_tag_dyneither(_tmp980,sizeof(char),33))),_tag_dyneither(_tmp97F,sizeof(void*),0)));}
goto _LL394;_LL394:;}
# 2573
_tmp696=Cyc_CfFlowInfo_assign_place(_tmp5EA,s->loc,_tmp696,env->all_changed,_tmp68D,_tmp693);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp696,_tmp697);
# 2578
return outflow;_LL392:;_LL393:
 return _tmp687;_LL38F:;};};};};}_LL387: if(((_tmp68A.f1).BottomFL).tag != 1)goto _LL389;_LL388:
# 2581
 return _tmp687;_LL389:;_LL38A:
{const char*_tmp983;void*_tmp982;(_tmp982=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp983="bad alias expression--no unique path found",_tag_dyneither(_tmp983,sizeof(char),43))),_tag_dyneither(_tmp982,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL384:;};}_LL364: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp624=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp624->tag != 12)goto _LL366;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;}}_LL365:
# 2587
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp625),_tmp626,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp625);
return outflow;_LL366: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp627=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp627->tag != 13)goto _LL368;else{_tmp628=_tmp627->f2;}}_LL367:
# 2592
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp628,rval_opt);_LL368: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp629=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp5EC;if(_tmp629->tag != 16)goto _LL36A;else{_tmp62A=_tmp629->f1;}}_LL369: {
# 2595
struct _tuple15 _tmp6A8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp62A);union Cyc_CfFlowInfo_FlowInfo _tmp6AA;void*_tmp6AB;struct _tuple15 _tmp6A9=_tmp6A8;_tmp6AA=_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6AC=Cyc_NewControlFlow_use_Rval(env,_tmp62A->loc,_tmp6AA,_tmp6AB);
void*_tmp6AD=Cyc_Tcutil_compress((void*)_check_null(_tmp62A->topt));void*_tmp6AE=_tmp6AD;void*_tmp6B0;_LL39A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6AF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6AE;if(_tmp6AF->tag != 15)goto _LL39C;else{_tmp6B0=(void*)_tmp6AF->f1;}}_LL39B:
# 2599
 return Cyc_CfFlowInfo_kill_flow_region(_tmp5EA,_tmp6AA,_tmp6B0);_LL39C:;_LL39D: {
struct Cyc_Core_Impossible_exn_struct _tmp989;const char*_tmp988;struct Cyc_Core_Impossible_exn_struct*_tmp987;(int)_throw((void*)((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp989.tag=Cyc_Core_Impossible,((_tmp989.f1=((_tmp988="anal_stmt -- reset_region",_tag_dyneither(_tmp988,sizeof(char),26))),_tmp989)))),_tmp987)))));}_LL399:;};}_LL36A:;_LL36B: {
# 2603
struct Cyc_Core_Impossible_exn_struct _tmp98F;const char*_tmp98E;struct Cyc_Core_Impossible_exn_struct*_tmp98D;(int)_throw((void*)((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98F.tag=Cyc_Core_Impossible,((_tmp98F.f1=((_tmp98E="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp98E,sizeof(char),56))),_tmp98F)))),_tmp98D)))));}_LL33F:;};}
# 2608
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2611
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp6B7=env->fenv;
void*_tmp6B8=decl->r;void*_tmp6B9=_tmp6B8;struct Cyc_Absyn_Vardecl*_tmp6BB;struct Cyc_List_List*_tmp6BD;struct Cyc_Absyn_Fndecl*_tmp6BF;struct Cyc_Absyn_Tvar*_tmp6C1;struct Cyc_Absyn_Vardecl*_tmp6C2;int _tmp6C3;struct Cyc_Absyn_Exp*_tmp6C4;_LL39F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BA=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6B9;if(_tmp6BA->tag != 0)goto _LL3A1;else{_tmp6BB=_tmp6BA->f1;}}_LL3A0: {
# 2621
struct Cyc_List_List _tmp990;struct Cyc_List_List _tmp6C5=(_tmp990.hd=_tmp6BB,((_tmp990.tl=0,_tmp990)));
inflow=Cyc_NewControlFlow_add_vars(_tmp6B7,inflow,& _tmp6C5,_tmp6B7->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp6C6=_tmp6BB->initializer;
if(_tmp6C6 == 0)
return inflow;{
struct _tuple15 _tmp6C7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6C6);union Cyc_CfFlowInfo_FlowInfo _tmp6C9;void*_tmp6CA;struct _tuple15 _tmp6C8=_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp6B7,env,_tmp6C9,_tmp6BB,_tmp6C6,_tmp6CA,decl->loc);};};}_LL3A1: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp6BC=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6B9;if(_tmp6BC->tag != 3)goto _LL3A3;else{_tmp6BD=_tmp6BC->f1;}}_LL3A2:
# 2630
 return Cyc_NewControlFlow_add_vars(_tmp6B7,inflow,_tmp6BD,_tmp6B7->unknown_none,decl->loc,0);_LL3A3: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6BE=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6B9;if(_tmp6BE->tag != 1)goto _LL3A5;else{_tmp6BF=_tmp6BE->f1;}}_LL3A4:
# 2633
 Cyc_NewControlFlow_check_nested_fun(_tmp6B7,inflow,_tmp6BF);{
void*t=(void*)_check_null(_tmp6BF->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp6CC=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6BF->fn_vardecl);
# 2639
struct Cyc_List_List*_tmp991;return Cyc_NewControlFlow_add_vars(_tmp6B7,inflow,((_tmp991=_region_malloc(env->r,sizeof(*_tmp991)),((_tmp991->hd=_tmp6CC,((_tmp991->tl=0,_tmp991)))))),_tmp6B7->unknown_all,decl->loc,0);};_LL3A5: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp6C0=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6B9;if(_tmp6C0->tag != 4)goto _LL3A7;else{_tmp6C1=_tmp6C0->f1;_tmp6C2=_tmp6C0->f2;_tmp6C3=_tmp6C0->f3;_tmp6C4=_tmp6C0->f4;}}_LL3A6:
# 2642
 if(_tmp6C4 != 0){
struct Cyc_Absyn_Exp*_tmp6CE=_tmp6C4;
struct _tuple15 _tmp6CF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6CE);union Cyc_CfFlowInfo_FlowInfo _tmp6D1;void*_tmp6D2;struct _tuple15 _tmp6D0=_tmp6CF;_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp6CE->loc,_tmp6D1,_tmp6D2);}{
# 2647
struct Cyc_List_List _tmp992;struct Cyc_List_List _tmp6D3=(_tmp992.hd=_tmp6C2,((_tmp992.tl=0,_tmp992)));
return Cyc_NewControlFlow_add_vars(_tmp6B7,inflow,& _tmp6D3,_tmp6B7->unknown_all,decl->loc,0);};_LL3A7:;_LL3A8: {
# 2651
struct Cyc_Core_Impossible_exn_struct _tmp998;const char*_tmp997;struct Cyc_Core_Impossible_exn_struct*_tmp996;(int)_throw((void*)((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996[0]=((_tmp998.tag=Cyc_Core_Impossible,((_tmp998.f1=((_tmp997="anal_decl: unexpected decl variant",_tag_dyneither(_tmp997,sizeof(char),35))),_tmp998)))),_tmp996)))));}_LL39E:;}
# 2659
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp6D8=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6D8;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2661
;_pop_region(frgn);}
# 2666
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2670
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2674
struct _RegionHandle*_tmp6D9=fenv->r;
unsigned int _tmp6DA=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6DA,1);{
# 2680
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6DB=inflow;struct Cyc_Dict_Dict _tmp6DC;struct Cyc_List_List*_tmp6DD;_LL3AA: if((_tmp6DB.BottomFL).tag != 1)goto _LL3AC;_LL3AB: {
const char*_tmp99B;void*_tmp99A;(_tmp99A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp99B="check_fun",_tag_dyneither(_tmp99B,sizeof(char),10))),_tag_dyneither(_tmp99A,sizeof(void*),0)));}_LL3AC: if((_tmp6DB.ReachableFL).tag != 2)goto _LL3A9;_tmp6DC=((struct _tuple14)(_tmp6DB.ReachableFL).val).f1;_tmp6DD=((struct _tuple14)(_tmp6DB.ReachableFL).val).f2;_LL3AD: {
# 2687
struct Cyc_List_List*atts;
{void*_tmp6E0=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp6E1=_tmp6E0;struct Cyc_List_List*_tmp6E3;_LL3AF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6E2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E1;if(_tmp6E2->tag != 9)goto _LL3B1;else{_tmp6E3=(_tmp6E2->f1).attributes;}}_LL3B0:
 atts=_tmp6E3;goto _LL3AE;_LL3B1:;_LL3B2: {
const char*_tmp99E;void*_tmp99D;(_tmp99D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp99E="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp99E,sizeof(char),50))),_tag_dyneither(_tmp99D,sizeof(void*),0)));}_LL3AE:;}
# 2694
for(0;atts != 0;atts=atts->tl){
void*_tmp6E6=(void*)atts->hd;void*_tmp6E7=_tmp6E6;int _tmp6E9;int _tmp6EB;int _tmp6ED;_LL3B4: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp6E8=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6E7;if(_tmp6E8->tag != 21)goto _LL3B6;else{_tmp6E9=_tmp6E8->f1;}}_LL3B5: {
# 2697
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp6EE=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6E9 - 1);
void*t=Cyc_Tcutil_compress(_tmp6EE->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6EF=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9A1;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9A0;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6F0=(_tmp9A0=_region_malloc(_tmp6D9,sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=2,((_tmp9A1.f1=_tmp6E9,((_tmp9A1.f2=t,_tmp9A1)))))),_tmp9A0)));
struct Cyc_CfFlowInfo_Place*_tmp9A2;struct Cyc_CfFlowInfo_Place*_tmp6F1=(_tmp9A2=_region_malloc(_tmp6D9,sizeof(*_tmp9A2)),((_tmp9A2->root=(void*)_tmp6F0,((_tmp9A2->fields=0,_tmp9A2)))));
_tmp6DC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6DC,(void*)_tmp6F0,_tmp6EF);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9A8;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9A7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9A5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9A4;_tmp6DC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6DC,(void*)((_tmp9A4=_region_malloc(_tmp6D9,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=0,((_tmp9A5.f1=_tmp6EE,_tmp9A5)))),_tmp9A4)))),(void*)((_tmp9A7=_region_malloc(_tmp6D9,sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A8.tag=5,((_tmp9A8.f1=_tmp6F1,_tmp9A8)))),_tmp9A7)))));}
goto _LL3B3;}_LL3B6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6EA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6E7;if(_tmp6EA->tag != 20)goto _LL3B8;else{_tmp6EB=_tmp6EA->f1;}}_LL3B7: {
# 2710
struct Cyc_Absyn_Vardecl*_tmp6F9=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6EB - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp6F9->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9AB;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9AA;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6FA=(_tmp9AA=_region_malloc(_tmp6D9,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=2,((_tmp9AB.f1=_tmp6EB,((_tmp9AB.f2=elttype,_tmp9AB)))))),_tmp9AA)));
struct Cyc_CfFlowInfo_Place*_tmp9AC;struct Cyc_CfFlowInfo_Place*_tmp6FB=(_tmp9AC=_region_malloc(_tmp6D9,sizeof(*_tmp9AC)),((_tmp9AC->root=(void*)_tmp6FA,((_tmp9AC->fields=0,_tmp9AC)))));
_tmp6DC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6DC,(void*)_tmp6FA,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9B2;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9B1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9AF;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9AE;_tmp6DC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6DC,(void*)((_tmp9AE=_region_malloc(_tmp6D9,sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=0,((_tmp9AF.f1=_tmp6F9,_tmp9AF)))),_tmp9AE)))),(void*)((_tmp9B1=_region_malloc(_tmp6D9,sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B2.tag=5,((_tmp9B2.f1=_tmp6FB,_tmp9B2)))),_tmp9B1)))));}
{struct Cyc_List_List*_tmp9B3;param_roots=((_tmp9B3=_region_malloc(_tmp6D9,sizeof(*_tmp9B3)),((_tmp9B3->hd=_tmp6FB,((_tmp9B3->tl=param_roots,_tmp9B3))))));}
goto _LL3B3;}_LL3B8: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6EC=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6E7;if(_tmp6EC->tag != 22)goto _LL3BA;else{_tmp6ED=_tmp6EC->f1;}}_LL3B9: {
# 2719
struct Cyc_Absyn_Vardecl*_tmp704=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6ED - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp704->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9B6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9B5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp705=(_tmp9B5=_region_malloc(_tmp6D9,sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=0,((_tmp9B6.f1=_tmp704,_tmp9B6)))),_tmp9B5)));
struct Cyc_CfFlowInfo_Place*_tmp9B7;struct Cyc_CfFlowInfo_Place*_tmp706=(_tmp9B7=_region_malloc(_tmp6D9,sizeof(*_tmp9B7)),((_tmp9B7->root=(void*)_tmp705,((_tmp9B7->fields=0,_tmp9B7)))));
struct Cyc_List_List*_tmp9B8;noconsume_roots=((_tmp9B8=_region_malloc(_tmp6D9,sizeof(*_tmp9B8)),((_tmp9B8->hd=_tmp706,((_tmp9B8->tl=noconsume_roots,_tmp9B8))))));}
# 2725
goto _LL3B3;}_LL3BA:;_LL3BB:
 goto _LL3B3;_LL3B3:;}
# 2728
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp6DC,_tmp6DD);}_LL3A9:;}{
# 2731
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6D9,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp9B9;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp9B9=_region_malloc(_tmp6D9,sizeof(*_tmp9B9)),((_tmp9B9->r=_tmp6D9,((_tmp9B9->fenv=fenv,((_tmp9B9->iterate_again=1,((_tmp9B9->iteration_num=0,((_tmp9B9->in_try=0,((_tmp9B9->tryflow=inflow,((_tmp9B9->all_changed=0,((_tmp9B9->noreturn=noreturn,((_tmp9B9->return_type=fd->ret_type,((_tmp9B9->unique_pat_vars=0,((_tmp9B9->param_roots=param_roots,((_tmp9B9->noconsume_params=noconsume_roots,((_tmp9B9->flow_table=flow_table,_tmp9B9)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2741
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2744
union Cyc_CfFlowInfo_FlowInfo _tmp70B=outflow;_LL3BD: if((_tmp70B.BottomFL).tag != 1)goto _LL3BF;_LL3BE:
 goto _LL3BC;_LL3BF:;_LL3C0:
# 2747
 Cyc_NewControlFlow_check_init_params(_tmp6DA,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp6DA);
# 2751
if(noreturn){
const char*_tmp9BC;void*_tmp9BB;(_tmp9BB=0,Cyc_CfFlowInfo_aerr(_tmp6DA,((_tmp9BC="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp9BC,sizeof(char),46))),_tag_dyneither(_tmp9BB,sizeof(void*),0)));}else{
# 2754
void*_tmp70E=Cyc_Tcutil_compress(fd->ret_type);void*_tmp70F=_tmp70E;_LL3C2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp70F;if(_tmp710->tag != 0)goto _LL3C4;}_LL3C3:
 goto _LL3C1;_LL3C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70F;if(_tmp711->tag != 7)goto _LL3C6;}_LL3C5:
 goto _LL3C7;_LL3C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp712=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70F;if(_tmp712->tag != 6)goto _LL3C8;}_LL3C7:
# 2758
{const char*_tmp9BF;void*_tmp9BE;(_tmp9BE=0,Cyc_Tcutil_warn(_tmp6DA,((_tmp9BF="function may not return a value",_tag_dyneither(_tmp9BF,sizeof(char),32))),_tag_dyneither(_tmp9BE,sizeof(void*),0)));}goto _LL3C1;_LL3C8:;_LL3C9:
# 2760
{const char*_tmp9C2;void*_tmp9C1;(_tmp9C1=0,Cyc_CfFlowInfo_aerr(_tmp6DA,((_tmp9C2="function may not return a value",_tag_dyneither(_tmp9C2,sizeof(char),32))),_tag_dyneither(_tmp9C1,sizeof(void*),0)));}goto _LL3C1;_LL3C1:;}
# 2762
goto _LL3BC;_LL3BC:;};};};}
# 2766
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp718=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp719=_tmp718;struct Cyc_Absyn_Fndecl*_tmp71B;struct Cyc_List_List*_tmp71D;struct Cyc_List_List*_tmp71F;struct Cyc_List_List*_tmp721;_LL3CB: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp71A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp719;if(_tmp71A->tag != 1)goto _LL3CD;else{_tmp71B=_tmp71A->f1;}}_LL3CC:
 Cyc_NewControlFlow_check_fun(_tmp71B);goto _LL3CA;_LL3CD: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp71C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp719;if(_tmp71C->tag != 12)goto _LL3CF;else{_tmp71D=_tmp71C->f1;}}_LL3CE:
 _tmp71F=_tmp71D;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp71E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp719;if(_tmp71E->tag != 11)goto _LL3D1;else{_tmp71F=_tmp71E->f2;}}_LL3D0:
 _tmp721=_tmp71F;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp720=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp719;if(_tmp720->tag != 10)goto _LL3D3;else{_tmp721=_tmp720->f2;}}_LL3D2:
 Cyc_NewControlFlow_cf_check(_tmp721);goto _LL3CA;_LL3D3: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp722=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp719;if(_tmp722->tag != 13)goto _LL3D5;}_LL3D4:
 goto _LL3CA;_LL3D5:;_LL3D6:
 goto _LL3CA;_LL3CA:;};}}
