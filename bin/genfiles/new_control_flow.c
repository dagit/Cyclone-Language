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
# 347
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 80
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RNumelts(struct Cyc_Absyn_Vardecl*);
# 88
enum Cyc_CfFlowInfo_Relation{Cyc_CfFlowInfo_Req  = 0,Cyc_CfFlowInfo_Rneq  = 1,Cyc_CfFlowInfo_Rlte  = 2,Cyc_CfFlowInfo_Rlt  = 3};
typedef enum Cyc_CfFlowInfo_Relation Cyc_CfFlowInfo_relation_t;struct Cyc_CfFlowInfo_Reln{union Cyc_CfFlowInfo_RelnOp rop1;enum Cyc_CfFlowInfo_Relation relation;union Cyc_CfFlowInfo_RelnOp rop2;};
# 95
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 104
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 110
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 115
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 122
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 144
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 149
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 165 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 184
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 187
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 190
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 193
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 199
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 202
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 228 "cf_flowinfo.h"
struct Cyc_Absyn_Vardecl*Cyc_CfFlowInfo_nonesc_vardecl(void*b);
# 231
int Cyc_CfFlowInfo_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_RelnOp*p);
# 233
struct Cyc_List_List*Cyc_CfFlowInfo_add_relation(struct _RegionHandle*rgn,union Cyc_CfFlowInfo_RelnOp rop1,enum Cyc_CfFlowInfo_Relation r,union Cyc_CfFlowInfo_RelnOp rop2,struct Cyc_List_List*relns);
# 236
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 238
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 242
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 255
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 259
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 264
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 270
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 275
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 284
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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp64B;struct Cyc_NewControlFlow_CFStmtAnnot _tmp64A;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp649;enclosee->annot=(void*)((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649[0]=((_tmp64B.tag=Cyc_NewControlFlow_CFAnnot,((_tmp64B.f1=((_tmp64A.encloser=encloser,((_tmp64A.visited=0,_tmp64A)))),_tmp64B)))),_tmp649))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 105 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 113
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 122
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
 return _tmp5;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp651;const char*_tmp650;struct Cyc_Core_Impossible_exn_struct*_tmp64F;(int)_throw((void*)((_tmp64F=_cycalloc(sizeof(*_tmp64F)),((_tmp64F[0]=((_tmp651.tag=Cyc_Core_Impossible,((_tmp651.f1=((_tmp650="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp650,sizeof(char),37))),_tmp651)))),_tmp64F)))));}_LL0:;}
# 129
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp652;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp652=_region_malloc(env->r,sizeof(*_tmp652)),((_tmp652[0]=Cyc_CfFlowInfo_BottomFL(),_tmp652)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 136
return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 139
static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 143
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 146
_tmpA->visited=env->iteration_num;{
struct _tuple16 _tmp653;return(_tmp653.f1=_tmpA,((_tmp653.f2=_tmpB,_tmp653)));};}
# 158 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 166
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple17{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 173
static void Cyc_NewControlFlow_check_unique_root(struct _tuple17*ckenv,void*root,void*rval){
# 175
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple17*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
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
{const char*_tmp657;void*_tmp656[1];struct Cyc_String_pa_PrintArg_struct _tmp655;(_tmp655.tag=0,((_tmp655.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp656[0]=& _tmp655,Cyc_Tcutil_warn(_tmpF,((_tmp657="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp657,sizeof(char),60))),_tag_dyneither(_tmp656,sizeof(void*),1)))))));}
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
struct _tuple15 _tmp658;struct _tuple15 _tmp21=(_tmp658.f1=flow,((_tmp658.f2=_tmp1F,_tmp658)));struct Cyc_Dict_Dict _tmp22;struct Cyc_Dict_Dict _tmp23;_LL16: if(((_tmp21.f1).ReachableFL).tag != 2)goto _LL18;_tmp22=((struct _tuple12)((_tmp21.f1).ReachableFL).val).f1;if(((_tmp21.f2).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple12)((_tmp21.f2).ReachableFL).val).f1;_LL17: {
# 206
struct _tuple17 _tmp659;struct _tuple17 _tmp24=(_tmp659.f1=env,((_tmp659.f2=s->loc,((_tmp659.f3=_tmp23,_tmp659)))));
((void(*)(void(*f)(struct _tuple17*,void*,void*),struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp24,_tmp22);
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
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp26.ReachableFL).tag != 2)goto _LL1A;_tmp27=((struct _tuple12)(_tmp26.ReachableFL).val).f1;_tmp28=((struct _tuple12)(_tmp26.ReachableFL).val).f2;_LL1E:
# 239
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp65C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=(_tmp65B=_region_malloc(fenv->r,sizeof(*_tmp65B)),((_tmp65B[0]=((_tmp65C.tag=0,((_tmp65C.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp65C)))),_tmp65B)));
void*_tmp2A=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp65F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp65E;_tmp2A=(void*)((_tmp65E=_region_malloc(fenv->r,sizeof(*_tmp65E)),((_tmp65E[0]=((_tmp65F.tag=8,((_tmp65F.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp65F.f2=_tmp2A,_tmp65F)))))),_tmp65E))));}
# 246
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp29,_tmp2A);}
# 248
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp28);_LL1A:;}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp2F=inflow;struct Cyc_Dict_Dict _tmp30;struct Cyc_List_List*_tmp31;_LL20: if((_tmp2F.BottomFL).tag != 1)goto _LL22;_LL21:
 return Cyc_CfFlowInfo_BottomFL();_LL22: if((_tmp2F.ReachableFL).tag != 2)goto _LL1F;_tmp30=((struct _tuple12)(_tmp2F.ReachableFL).val).f1;_tmp31=((struct _tuple12)(_tmp2F.ReachableFL).val).f2;_LL23:
# 256
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp662;void*_tmp661;(_tmp661=0,Cyc_Tcutil_terr(loc,((_tmp662="expression may not be fully initialized",_tag_dyneither(_tmp662,sizeof(char),40))),_tag_dyneither(_tmp661,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp34=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp30,env->all_changed,r);
if(_tmp30.t == _tmp34.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp35=Cyc_CfFlowInfo_ReachableFL(_tmp34,_tmp31);
Cyc_NewControlFlow_update_tryflow(env,_tmp35);
return _tmp35;};};_LL1F:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 266
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp663;struct _tuple0 _tmp37=(_tmp663.f1=Cyc_Tcutil_compress(t),((_tmp663.f2=r,_tmp663)));void*_tmp3C;struct Cyc_Absyn_Datatypefield*_tmp3E;struct _dyneither_ptr _tmp40;struct Cyc_List_List*_tmp42;struct _dyneither_ptr _tmp44;union Cyc_Absyn_AggrInfoU _tmp46;struct Cyc_List_List*_tmp47;struct _dyneither_ptr _tmp49;enum Cyc_Absyn_AggrKind _tmp4B;struct Cyc_List_List*_tmp4C;struct _dyneither_ptr _tmp4E;_LL25: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp38=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp37.f2;if(_tmp38->tag != 3)goto _LL27;else{if(_tmp38->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL27;}}_LL26:
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
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp42))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp44,sizeof(void*),i)));}}
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
{const char*_tmp666;void*_tmp665;(_tmp665=0,Cyc_Tcutil_warn(loc,((_tmp666="argument may still contain unique pointers",_tag_dyneither(_tmp666,sizeof(char),43))),_tag_dyneither(_tmp665,sizeof(void*),0)));}
return;_LL37:;_LL38:
 return;_LL24:;}
# 349 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp56=inflow;struct Cyc_Dict_Dict _tmp57;struct Cyc_List_List*_tmp58;_LL3A: if((_tmp56.BottomFL).tag != 1)goto _LL3C;_LL3B:
 return Cyc_CfFlowInfo_BottomFL();_LL3C: if((_tmp56.ReachableFL).tag != 2)goto _LL39;_tmp57=((struct _tuple12)(_tmp56.ReachableFL).val).f1;_tmp58=((struct _tuple12)(_tmp56.ReachableFL).val).f2;_LL3D:
# 353
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_Tcutil_terr(loc,((_tmp669="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp669,sizeof(char),47))),_tag_dyneither(_tmp668,sizeof(void*),0)));}
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
const char*_tmp66C;void*_tmp66B;(_tmp66B=0,Cyc_Tcutil_warn(loc,((_tmp66C="argument may contain live unique pointers",_tag_dyneither(_tmp66C,sizeof(char),42))),_tag_dyneither(_tmp66B,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL3E:;}{
# 368
struct Cyc_Dict_Dict _tmp63=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp57,env->all_changed,r);
if(_tmp57.t == _tmp63.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp64=Cyc_CfFlowInfo_ReachableFL(_tmp63,_tmp58);
Cyc_NewControlFlow_update_tryflow(env,_tmp64);
return _tmp64;};};};_LL39:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 377
static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 384
if(es == 0){
struct _tuple19 _tmp66D;return(_tmp66D.f1=inflow,((_tmp66D.f2=0,_tmp66D)));}
if(es->tl == 0){
union Cyc_CfFlowInfo_FlowInfo _tmp67;void*_tmp68;struct _tuple13 _tmp66=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp670;struct _tuple19 _tmp66F;return(_tmp66F.f1=_tmp67,((_tmp66F.f2=((_tmp670=_region_malloc(rgn,sizeof(*_tmp670)),((_tmp670->hd=_tmp68,((_tmp670->tl=0,_tmp670)))))),_tmp66F)));};}{
# 390
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
do{
this_all_changed=(env->fenv)->mt_place_set;
# 398
{struct Cyc_Dict_Dict*_tmp671;env->all_changed=((_tmp671=_region_malloc(env->r,sizeof(*_tmp671)),((_tmp671[0]=(env->fenv)->mt_place_set,_tmp671))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp6D;void*_tmp6E;struct _tuple13 _tmp6C=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;
outflow=_tmp6D;
{struct Cyc_List_List*_tmp672;rvals=((_tmp672=_region_malloc(rgn,sizeof(*_tmp672)),((_tmp672->hd=_tmp6E,((_tmp672->tl=0,_tmp672))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 404
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp673;env->all_changed=((_tmp673=_region_malloc(env->r,sizeof(*_tmp673)),((_tmp673[0]=(env->fenv)->mt_place_set,_tmp673))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp72;void*_tmp73;struct _tuple13 _tmp71=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;
{struct Cyc_List_List*_tmp674;rvals=((_tmp674=_region_malloc(rgn,sizeof(*_tmp674)),((_tmp674->hd=_tmp73,((_tmp674->tl=rvals,_tmp674))))));}
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
struct Cyc_Dict_Dict*_tmp675;env->all_changed=((_tmp675=_region_malloc(env->r,sizeof(*_tmp675)),((_tmp675[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp675))));}
# 427
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple19 _tmp676;return(_tmp676.f1=outflow,((_tmp676.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp676)));};};}
# 433
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 436
struct _RegionHandle _tmp77=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77;_push_region(rgn);
{union Cyc_CfFlowInfo_FlowInfo _tmp79;struct Cyc_List_List*_tmp7A;struct _tuple19 _tmp78=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp7B=_tmp79;struct Cyc_Dict_Dict _tmp7C;_LL46: if((_tmp7B.ReachableFL).tag != 2)goto _LL48;_tmp7C=((struct _tuple12)(_tmp7B.ReachableFL).val).f1;_LL47:
# 441
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp7C,(void*)_tmp7A->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp679="expression may not be initialized",_tag_dyneither(_tmp679,sizeof(char),34))),_tag_dyneither(_tmp678,sizeof(void*),0)));}}
goto _LL45;_LL48: if((_tmp7B.BottomFL).tag != 1)goto _LL45;_LL49:
 goto _LL45;_LL45:;}{
# 447
struct _tuple13 _tmp67A;struct _tuple13 _tmp80=(_tmp67A.f1=_tmp79,((_tmp67A.f2=(env->fenv)->unknown_all,_tmp67A)));_npop_handler(0);return _tmp80;};}
# 437
;_pop_region(rgn);}
# 461 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 467
union Cyc_CfFlowInfo_FlowInfo _tmp81=outflow;struct Cyc_Dict_Dict _tmp82;struct Cyc_List_List*_tmp83;_LL4B: if((_tmp81.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return outflow;_LL4D: if((_tmp81.ReachableFL).tag != 2)goto _LL4A;_tmp82=((struct _tuple12)(_tmp81.ReachableFL).val).f1;_tmp83=((struct _tuple12)(_tmp81.ReachableFL).val).f2;_LL4E: {
# 470
union Cyc_CfFlowInfo_AbsLVal _tmp84=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp85;_LL50: if((_tmp84.UnknownL).tag != 2)goto _LL52;_LL51:
# 474
 return outflow;_LL52: if((_tmp84.PlaceL).tag != 1)goto _LL4F;_tmp85=(struct Cyc_CfFlowInfo_Place*)(_tmp84.PlaceL).val;_LL53: {
# 478
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp67D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp67C;nzval=(void*)((_tmp67C=_region_malloc((env->fenv)->r,sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=8,((_tmp67D.f1=locname,((_tmp67D.f2=nzval,_tmp67D)))))),_tmp67C))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp88=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp82,env->all_changed,_tmp85,nzval),_tmp83);
# 485
return _tmp88;};}_LL4F:;}_LL4A:;}
# 494
static struct _tuple15 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_Absyn_Vardecl*locname){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp89=outflow;struct Cyc_Dict_Dict _tmp8A;struct Cyc_List_List*_tmp8B;_LL55: if((_tmp89.BottomFL).tag != 1)goto _LL57;_LL56: {
struct _tuple15 _tmp67E;return(_tmp67E.f1=outflow,((_tmp67E.f2=outflow,_tmp67E)));}_LL57: if((_tmp89.ReachableFL).tag != 2)goto _LL54;_tmp8A=((struct _tuple12)(_tmp89.ReachableFL).val).f1;_tmp8B=((struct _tuple12)(_tmp89.ReachableFL).val).f2;_LL58: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp8D=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp8E;_LL5A: if((_tmp8D.UnknownL).tag != 2)goto _LL5C;_LL5B: {
struct _tuple15 _tmp67F;return(_tmp67F.f1=outflow,((_tmp67F.f2=outflow,_tmp67F)));}_LL5C: if((_tmp8D.PlaceL).tag != 1)goto _LL59;_tmp8E=(struct Cyc_CfFlowInfo_Place*)(_tmp8D.PlaceL).val;_LL5D: {
# 503
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp682;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp681;nzval=(void*)((_tmp681=_region_malloc((env->fenv)->r,sizeof(*_tmp681)),((_tmp681[0]=((_tmp682.tag=8,((_tmp682.f1=locname,((_tmp682.f2=nzval,_tmp682)))))),_tmp681))));}{
struct _tuple15 _tmp683;return
(_tmp683.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,nzval),_tmp8B),((_tmp683.f2=
# 509
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8A,env->all_changed,_tmp8E,(env->fenv)->zero),_tmp8B),_tmp683)));};}_LL59:;}_LL54:;}
# 515
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 521
static struct _tuple13 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 525
void*_tmp95=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp97;union Cyc_Absyn_Constraint*_tmp98;union Cyc_Absyn_Constraint*_tmp99;_LL5F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95;if(_tmp96->tag != 5)goto _LL61;else{_tmp97=(_tmp96->f1).elt_typ;_tmp98=((_tmp96->f1).ptr_atts).bounds;_tmp99=((_tmp96->f1).ptr_atts).zero_term;}}_LL60: {
# 527
union Cyc_CfFlowInfo_FlowInfo _tmp9A=f;struct Cyc_Dict_Dict _tmp9B;struct Cyc_List_List*_tmp9C;_LL64: if((_tmp9A.BottomFL).tag != 1)goto _LL66;_LL65: {
# 529
struct _tuple13 _tmp684;return(_tmp684.f1=f,((_tmp684.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp684)));}_LL66: if((_tmp9A.ReachableFL).tag != 2)goto _LL63;_tmp9B=((struct _tuple12)(_tmp9A.ReachableFL).val).f1;_tmp9C=((struct _tuple12)(_tmp9A.ReachableFL).val).f2;_LL67: {
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp687;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp686;e->annot=(void*)((_tmp686=_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((_tmp687.tag=Cyc_CfFlowInfo_NotZero,((_tmp687.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp687)))),_tmp686))));}
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
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp68A;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp689;e->annot=(void*)((_tmp689=_cycalloc(sizeof(*_tmp689)),((_tmp689[0]=((_tmp68A.tag=Cyc_CfFlowInfo_NotZero,((_tmp68A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp68A)))),_tmp689))));}
goto _LL8B;_LL8E:;_LL8F:
# 571
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8B;_LL8B:;}
# 574
goto _LL86;_LL86:;}{
# 576
struct _tuple13 _tmp68B;return(_tmp68B.f1=f,((_tmp68B.f2=Cyc_CfFlowInfo_lookup_place(_tmp9B,_tmpA9),_tmp68B)));};_LL76: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpAA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAA->tag != 0)goto _LL78;}_LL77:
# 578
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp68C;return(_tmp68C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp68C.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,(env->fenv)->unknown_all),_tmp68C)));};_LL78: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpAB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpAB->tag != 3)goto _LL7A;else{_tmpAC=_tmpAB->f1;}}_LL79:
# 581
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpAC,locname);goto _LL7B;_LL7A:;_LL7B:
# 583
{void*_tmpBD=e->r;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD;if(_tmpBE->tag != 22)goto _LL93;}_LL92:
# 585
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp68F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp68E;e->annot=(void*)((_tmp68E=_cycalloc(sizeof(*_tmp68E)),((_tmp68E[0]=((_tmp68F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp68F.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp68F)))),_tmp68E))));}
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
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp692;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp691;e->annot=(void*)((_tmp691=_cycalloc(sizeof(*_tmp691)),((_tmp691[0]=((_tmp692.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp692.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9C),_tmp692)))),_tmp691))));}
goto _LL95;_LL95:;}
# 603
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp9B,r)){case Cyc_CfFlowInfo_NoneIL: _LL9A:
# 605
{void*_tmpC6=r;_LL9D: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpC7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpC6;if(_tmpC7->tag != 7)goto _LL9F;}_LL9E:
# 608
{const char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_Tcutil_terr(e->loc,((_tmp695="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp695,sizeof(char),61))),_tag_dyneither(_tmp694,sizeof(void*),0)));}
goto _LL9C;_LL9F:;_LLA0: {
# 611
const char*_tmp698;void*_tmp697;(_tmp697=0,Cyc_Tcutil_terr(e->loc,((_tmp698="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp698,sizeof(char),46))),_tag_dyneither(_tmp697,sizeof(void*),0)));}_LL9C:;}
# 613
goto _LL9B;case Cyc_CfFlowInfo_AllIL: _LL9B: {
# 615
void*_tmpCC=(env->fenv)->unknown_all;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp69B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69A;_tmpCC=(void*)((_tmp69A=_region_malloc((env->fenv)->r,sizeof(*_tmp69A)),((_tmp69A[0]=((_tmp69B.tag=8,((_tmp69B.f1=locname,((_tmp69B.f2=_tmpCC,_tmp69B)))))),_tmp69A))));}{
struct _tuple13 _tmp69C;return(_tmp69C.f1=f,((_tmp69C.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpCC),_tmp69C)));};}case Cyc_CfFlowInfo_ThisIL: _LLA1: {
# 620
void*_tmpD0=(env->fenv)->unknown_none;
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp69F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp69E;_tmpD0=(void*)((_tmp69E=_region_malloc((env->fenv)->r,sizeof(*_tmp69E)),((_tmp69E[0]=((_tmp69F.tag=8,((_tmp69F.f1=locname,((_tmp69F.f2=_tmpD0,_tmp69F)))))),_tmp69E))));}{
struct _tuple13 _tmp6A0;return(_tmp6A0.f1=f,((_tmp6A0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp97,0,_tmpD0),_tmp6A0)));};}}}_LL63:;}_LL61:;_LL62: {
# 626
struct Cyc_Core_Impossible_exn_struct _tmp6A6;const char*_tmp6A5;struct Cyc_Core_Impossible_exn_struct*_tmp6A4;(int)_throw((void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A6.tag=Cyc_Core_Impossible,((_tmp6A6.f1=((_tmp6A5="right deref of non-pointer-type",_tag_dyneither(_tmp6A5,sizeof(char),32))),_tmp6A6)))),_tmp6A4)))));}_LL5E:;}
# 633
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 636
union Cyc_CfFlowInfo_RelnOp n2=Cyc_CfFlowInfo_RConst(0);
int e2_valid_op=Cyc_CfFlowInfo_exp2relnop(e2,& n2);
# 639
{void*_tmpD7=e1->r;void*_tmpD9;_LLA4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7;if(_tmpD8->tag != 1)goto _LLA6;else{_tmpD9=(void*)_tmpD8->f2;}}_LLA5: {
# 641
struct Cyc_Absyn_Vardecl*_tmpDA=Cyc_CfFlowInfo_nonesc_vardecl(_tmpD9);
if(_tmpDA != 0){
union Cyc_CfFlowInfo_RelnOp n1=Cyc_CfFlowInfo_RNumelts(_tmpDA);
if(e2_valid_op)
relns=Cyc_CfFlowInfo_add_relation(rgn,n2,Cyc_CfFlowInfo_Rlt,n1,relns);}
# 647
goto _LLA3;}_LLA6:;_LLA7:
 goto _LLA3;_LLA3:;}{
# 651
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_CfFlowInfo_RelnOp rbound=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(bound,& rbound))
relns=Cyc_CfFlowInfo_add_relation(rgn,n2,Cyc_CfFlowInfo_Rlt,rbound,relns);}
# 657
return relns;};}
# 662
static struct _tuple13 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 670
union Cyc_CfFlowInfo_FlowInfo _tmpDB=outflow;struct Cyc_Dict_Dict _tmpDC;struct Cyc_List_List*_tmpDD;_LLA9: if((_tmpDB.BottomFL).tag != 1)goto _LLAB;_LLAA: {
# 673
struct _tuple13 _tmp6A7;return(_tmp6A7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6A7.f2=rval,_tmp6A7)));}_LLAB: if((_tmpDB.ReachableFL).tag != 2)goto _LLA8;_tmpDC=((struct _tuple12)(_tmpDB.ReachableFL).val).f1;_tmpDD=((struct _tuple12)(_tmpDB.ReachableFL).val).f2;_LLAC: {
# 675
union Cyc_CfFlowInfo_AbsLVal _tmpDF=lval;struct Cyc_CfFlowInfo_Place*_tmpE0;_LLAE: if((_tmpDF.PlaceL).tag != 1)goto _LLB0;_tmpE0=(struct Cyc_CfFlowInfo_Place*)(_tmpDF.PlaceL).val;_LLAF: {
# 677
struct Cyc_Dict_Dict _tmpE1=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpDC,env->all_changed,_tmpE0,rval);
_tmpDD=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmpDD,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmpE1,_tmpDD);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 683
void*_tmpE2=Cyc_CfFlowInfo_lookup_place(_tmpDC,_tmpE0);_LLB3: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE3=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpE3->tag != 3)goto _LLB5;else{if(_tmpE3->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLB5;}}_LLB4:
 goto _LLB6;_LLB5: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpE4->tag != 7)goto _LLB7;}_LLB6:
 goto _LLB8;_LLB7: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpE5->tag != 0)goto _LLB9;}_LLB8:
 goto _LLBA;_LLB9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpE6->tag != 8)goto _LLBB;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmpE6->f2);if(_tmpE7->tag != 3)goto _LLBB;else{if(_tmpE7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLBB;}}}_LLBA:
 goto _LLBC;_LLBB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpE8->tag != 8)goto _LLBD;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE9=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmpE8->f2);if(_tmpE9->tag != 7)goto _LLBD;}}_LLBC:
 goto _LLBE;_LLBD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE2;if(_tmpEA->tag != 8)goto _LLBF;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpEB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmpEA->f2);if(_tmpEB->tag != 0)goto _LLBF;}}_LLBE:
 goto _LLB2;_LLBF:;_LLC0:
# 691
{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_Tcutil_warn(lexp->loc,((_tmp6AA="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp6AA,sizeof(char),43))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
goto _LLB2;_LLB2:;}
# 696
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 698
struct _tuple13 _tmp6AB;return(_tmp6AB.f1=outflow,((_tmp6AB.f2=rval,_tmp6AB)));};}_LLB0: if((_tmpDF.UnknownL).tag != 2)goto _LLAD;_LLB1: {
# 701
struct _tuple13 _tmp6AC;return(_tmp6AC.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp6AC.f2=rval,_tmp6AC)));}_LLAD:;}_LLA8:;}
# 706
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
void*_tmpF0=e->r;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmp100;_LLC2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF1->tag != 1)goto _LLC4;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpF1->f2);if(_tmpF2->tag != 4)goto _LLC4;}}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF3->tag != 1)goto _LLC6;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF4=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpF3->f2);if(_tmpF4->tag != 3)goto _LLC6;}}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF5->tag != 1)goto _LLC8;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpF6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpF5->f2);if(_tmpF6->tag != 5)goto _LLC8;}}_LLC7:
 return 1;_LLC8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF7->tag != 19)goto _LLCA;else{_tmpF8=_tmpF7->f1;}}_LLC9:
 _tmpFA=_tmpF8;goto _LLCB;_LLCA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF9->tag != 20)goto _LLCC;else{_tmpFA=_tmpF9->f1;}}_LLCB:
 _tmpFC=_tmpFA;goto _LLCD;_LLCC: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpFB->tag != 21)goto _LLCE;else{_tmpFC=_tmpFB->f1;}}_LLCD:
# 714
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmpFC);_LLCE: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpFD->tag != 22)goto _LLD0;else{_tmpFE=_tmpFD->f1;}}_LLCF: {
# 716
void*_tmp101=Cyc_Tcutil_compress((void*)_check_null(_tmpFE->topt));_LLD5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp102=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp101;if(_tmp102->tag != 10)goto _LLD7;}_LLD6:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmpFE);_LLD7:;_LLD8:
 return 0;_LLD4:;}_LLD0: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpFF->tag != 13)goto _LLD2;else{_tmp100=_tmpFF->f2;}}_LLD1:
# 720
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp100);_LLD2:;_LLD3:
 return 0;_LLC1:;}
# 725
static struct _tuple13 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 727
struct Cyc_CfFlowInfo_FlowEnv*_tmp103=env->fenv;
# 731
while(1){
union Cyc_CfFlowInfo_FlowInfo*_tmp105;struct _tuple16 _tmp104=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp105=_tmp104.f2;
inflow=*_tmp105;{
void*_tmp106=s->r;struct Cyc_Absyn_Stmt*_tmp108;struct Cyc_Absyn_Stmt*_tmp109;struct Cyc_Absyn_Tvar*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10D;struct Cyc_Absyn_Stmt*_tmp10E;struct Cyc_Absyn_Decl*_tmp110;struct Cyc_Absyn_Stmt*_tmp111;struct Cyc_Absyn_Exp*_tmp113;_LLDA: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp107=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp106;if(_tmp107->tag != 2)goto _LLDC;else{_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}}_LLDB:
# 736
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp108);
s=_tmp109;
goto _LLD9;_LLDC: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp10A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp106;if(_tmp10A->tag != 12)goto _LLDE;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp10B=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp10A->f1)->r;if(_tmp10B->tag != 5)goto _LLDE;else{_tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;}}_tmp10E=_tmp10A->f2;}}_LLDD: {
# 742
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp10D->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp115;union Cyc_CfFlowInfo_AbsLVal _tmp116;struct _tuple14 _tmp114=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;{
struct _tuple14 _tmp6AD;struct _tuple14 _tmp118=(_tmp6AD.f1=_tmp115,((_tmp6AD.f2=_tmp116,_tmp6AD)));struct Cyc_Dict_Dict _tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_CfFlowInfo_Place*_tmp11B;_LLE5: if(((_tmp118.f1).ReachableFL).tag != 2)goto _LLE7;_tmp119=((struct _tuple12)((_tmp118.f1).ReachableFL).val).f1;_tmp11A=((struct _tuple12)((_tmp118.f1).ReachableFL).val).f2;if(((_tmp118.f2).PlaceL).tag != 1)goto _LLE7;_tmp11B=(struct Cyc_CfFlowInfo_Place*)((_tmp118.f2).PlaceL).val;_LLE6: {
# 746
void*_tmp11C=Cyc_CfFlowInfo_lookup_place(_tmp119,_tmp11B);
# 748
void*t=(void*)_check_null(e->topt);
# 755
void*_tmp11D=Cyc_CfFlowInfo_make_unique_consumed(_tmp103,t,e,env->iteration_num,_tmp11C);
_tmp119=Cyc_CfFlowInfo_assign_place(_tmp103,e->loc,_tmp119,env->all_changed,_tmp11B,_tmp11D);
_tmp115=Cyc_CfFlowInfo_ReachableFL(_tmp119,_tmp11A);{
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp103,_tmp119,_tmp11C)){case Cyc_CfFlowInfo_AllIL: _LLEB:
 leaf=_tmp103->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LLEC:
 leaf=_tmp103->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLED:
 leaf=_tmp103->unknown_this;break;}{
# 766
void*_tmp11E=Cyc_CfFlowInfo_typ_to_absrval(_tmp103,t,0,leaf);
_tmp115=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp115,_tmp11C);{
struct Cyc_List_List _tmp6AE;struct Cyc_List_List _tmp11F=(_tmp6AE.hd=_tmp10D,((_tmp6AE.tl=0,_tmp6AE)));
_tmp115=Cyc_NewControlFlow_add_vars(_tmp103,_tmp115,& _tmp11F,_tmp103->unknown_all,s->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp121;void*_tmp122;struct _tuple13 _tmp120=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp115,_tmp10E);_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp123=_tmp121;struct Cyc_Dict_Dict _tmp124;struct Cyc_List_List*_tmp125;_LLF0: if((_tmp123.ReachableFL).tag != 2)goto _LLF2;_tmp124=((struct _tuple12)(_tmp123.ReachableFL).val).f1;_tmp125=((struct _tuple12)(_tmp123.ReachableFL).val).f2;_LLF1:
# 773
 _tmp124=Cyc_CfFlowInfo_assign_place(_tmp103,s->loc,_tmp124,env->all_changed,_tmp11B,_tmp11E);{
union Cyc_CfFlowInfo_FlowInfo _tmp126=Cyc_CfFlowInfo_ReachableFL(_tmp124,_tmp125);
struct _tuple13 _tmp6AF;return(_tmp6AF.f1=_tmp126,((_tmp6AF.f2=_tmp122,_tmp6AF)));};_LLF2:;_LLF3: {
struct _tuple13 _tmp6B0;return(_tmp6B0.f1=_tmp121,((_tmp6B0.f2=_tmp122,_tmp6B0)));}_LLEF:;};};};};};}_LLE7: if(((_tmp118.f1).BottomFL).tag != 1)goto _LLE9;_LLE8: {
# 778
struct _tuple13 _tmp6B1;return(_tmp6B1.f1=_tmp115,((_tmp6B1.f2=_tmp103->unknown_all,_tmp6B1)));}_LLE9:;_LLEA:
{const char*_tmp6B4;void*_tmp6B3;(_tmp6B3=0,Cyc_Tcutil_terr(e->loc,((_tmp6B4="bad alias expression--no unique path found",_tag_dyneither(_tmp6B4,sizeof(char),43))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}{
struct _tuple13 _tmp6B5;return(_tmp6B5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6B5.f2=_tmp103->unknown_all,_tmp6B5)));};_LLE4:;};}_LLDE: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp10F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp106;if(_tmp10F->tag != 12)goto _LLE0;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}}_LLDF:
# 783
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp110);
s=_tmp111;
goto _LLD9;_LLE0: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp112=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp106;if(_tmp112->tag != 1)goto _LLE2;else{_tmp113=_tmp112->f1;}}_LLE1:
# 787
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp113);_LLE2:;_LLE3: {
struct Cyc_Core_Impossible_exn_struct _tmp6BB;const char*_tmp6BA;struct Cyc_Core_Impossible_exn_struct*_tmp6B9;(int)_throw((void*)((_tmp6B9=_cycalloc(sizeof(*_tmp6B9)),((_tmp6B9[0]=((_tmp6BB.tag=Cyc_Core_Impossible,((_tmp6BB.f1=((_tmp6BA="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp6BA,sizeof(char),33))),_tmp6BB)))),_tmp6B9)))));}_LLD9:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp75D(unsigned int*_tmp75C,unsigned int*_tmp75B,void***_tmp759,struct Cyc_List_List**_tmp309){for(*_tmp75C=0;*_tmp75C < *_tmp75B;(*_tmp75C)++){
# 1348 "new_control_flow.cyc"
void*_tmp757;(*_tmp759)[*_tmp75C]=(void*)((_tmp757=(void*)((struct Cyc_List_List*)_check_null(*_tmp309))->hd,((*_tmp309=(*_tmp309)->tl,_tmp757))));}}
# 797 "new_control_flow.cyc"
static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 801
struct Cyc_CfFlowInfo_FlowEnv*_tmp131=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 807
{union Cyc_CfFlowInfo_FlowInfo _tmp132=inflow;struct Cyc_Dict_Dict _tmp133;struct Cyc_List_List*_tmp134;_LLF5: if((_tmp132.BottomFL).tag != 1)goto _LLF7;_LLF6: {
struct _tuple13 _tmp6BC;return(_tmp6BC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6BC.f2=_tmp131->unknown_all,_tmp6BC)));}_LLF7: if((_tmp132.ReachableFL).tag != 2)goto _LLF4;_tmp133=((struct _tuple12)(_tmp132.ReachableFL).val).f1;_tmp134=((struct _tuple12)(_tmp132.ReachableFL).val).f2;_LLF8:
 d=_tmp133;relns=_tmp134;_LLF4:;}
# 816
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 837 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp137;union Cyc_CfFlowInfo_AbsLVal _tmp138;struct _tuple14 _tmp136=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp137=_tmp136.f1;_tmp138=_tmp136.f2;{
struct _tuple14 _tmp6BD;struct _tuple14 _tmp13A=(_tmp6BD.f1=_tmp137,((_tmp6BD.f2=_tmp138,_tmp6BD)));struct Cyc_Dict_Dict _tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_CfFlowInfo_Place*_tmp13D;_LLFA: if(((_tmp13A.f1).ReachableFL).tag != 2)goto _LLFC;_tmp13B=((struct _tuple12)((_tmp13A.f1).ReachableFL).val).f1;_tmp13C=((struct _tuple12)((_tmp13A.f1).ReachableFL).val).f2;if(((_tmp13A.f2).PlaceL).tag != 1)goto _LLFC;_tmp13D=(struct Cyc_CfFlowInfo_Place*)((_tmp13A.f2).PlaceL).val;_LLFB: {
# 840
void*_tmp13E=Cyc_CfFlowInfo_lookup_place(_tmp13B,_tmp13D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp13E,& needs_unconsume)){
{const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_Tcutil_terr(e->loc,((_tmp6C0="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp6C0,sizeof(char),71))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}{
struct _tuple13 _tmp6C1;return(_tmp6C1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6C1.f2=_tmp131->unknown_all,_tmp6C1)));};}else{
# 846
if(needs_unconsume){
# 848
struct _tuple13 _tmp6C2;return(_tmp6C2.f1=_tmp137,((_tmp6C2.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp131,_tmp13E),_tmp6C2)));}else{
# 851
void*_tmp143=Cyc_CfFlowInfo_make_unique_consumed(_tmp131,(void*)_check_null(e->topt),e,env->iteration_num,_tmp13E);
struct Cyc_Dict_Dict _tmp144=Cyc_CfFlowInfo_assign_place(_tmp131,e->loc,_tmp13B,env->all_changed,_tmp13D,_tmp143);
# 862
struct _tuple13 _tmp6C3;return(_tmp6C3.f1=Cyc_CfFlowInfo_ReachableFL(_tmp144,_tmp13C),((_tmp6C3.f2=_tmp13E,_tmp6C3)));}}}_LLFC:;_LLFD:
# 864
 goto _LLF9;_LLF9:;};}{
# 867
void*_tmp146=e->r;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Vardecl*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp162;struct Cyc_Absyn_Vardecl*_tmp165;enum Cyc_Absyn_Primop _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_List_List*_tmp178;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct _dyneither_ptr*_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_Datatypedecl*_tmp1A1;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AC;struct Cyc_Absyn_Vardecl*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B0;int _tmp1B1;struct Cyc_Absyn_Exp*_tmp1B3;void*_tmp1B4;int _tmp1B5;struct Cyc_Absyn_Stmt*_tmp1B7;_LLFF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp147->tag != 13)goto _LL101;else{_tmp148=_tmp147->f2;if(_tmp147->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL101;}}_LL100: {
# 871
union Cyc_CfFlowInfo_FlowInfo _tmp1BF;void*_tmp1C0;struct _tuple13 _tmp1BE=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp148);_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp1C2;void*_tmp1C3;struct _tuple13 _tmp1C1=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1BF,_tmp148,_tmp1C0);_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;{
struct _tuple13 _tmp6C4;return(_tmp6C4.f1=_tmp1C2,((_tmp6C4.f2=_tmp1C0,_tmp6C4)));};};}_LL101: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp149=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp149->tag != 13)goto _LL103;else{_tmp14A=_tmp149->f2;}}_LL102:
# 877
 _tmp14C=_tmp14A;goto _LL104;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp14B->tag != 11)goto _LL105;else{_tmp14C=_tmp14B->f1;}}_LL104:
 _tmp14E=_tmp14C;goto _LL106;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp14D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp14D->tag != 12)goto _LL107;else{_tmp14E=_tmp14D->f1;}}_LL106:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp14E);_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp14F->tag != 0)goto _LL109;else{if(((_tmp14F->f1).Null_c).tag != 1)goto _LL109;}}_LL108:
# 881
 goto _LL10A;_LL109: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp150->tag != 0)goto _LL10B;else{if(((_tmp150->f1).Int_c).tag != 5)goto _LL10B;if(((struct _tuple6)((_tmp150->f1).Int_c).val).f2 != 0)goto _LL10B;}}_LL10A: {
struct _tuple13 _tmp6C5;return(_tmp6C5.f1=inflow,((_tmp6C5.f2=_tmp131->zero,_tmp6C5)));}_LL10B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp151->tag != 0)goto _LL10D;else{if(((_tmp151->f1).Int_c).tag != 5)goto _LL10D;}}_LL10C:
 goto _LL10E;_LL10D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp152->tag != 1)goto _LL10F;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp153=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp152->f2);if(_tmp153->tag != 2)goto _LL10F;}}_LL10E: {
struct _tuple13 _tmp6C6;return(_tmp6C6.f1=inflow,((_tmp6C6.f2=_tmp131->notzeroall,_tmp6C6)));}_LL10F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp154->tag != 30)goto _LL111;else{if(_tmp154->f1 != 0)goto _LL111;}}_LL110:
# 886
 goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp155->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp156->tag != 17)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp157->tag != 16)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp158->tag != 18)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp159->tag != 32)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp15A->tag != 31)goto _LL11D;}_LL11C: {
struct _tuple13 _tmp6C7;return(_tmp6C7.f1=inflow,((_tmp6C7.f2=_tmp131->unknown_all,_tmp6C7)));}_LL11D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp15B->tag != 1)goto _LL11F;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp15C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp15B->f2);if(_tmp15C->tag != 1)goto _LL11F;}}_LL11E: {
# 895
struct _tuple13 _tmp6C8;return(_tmp6C8.f1=inflow,((_tmp6C8.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp6C8)));}_LL11F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp15D->tag != 1)goto _LL121;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp15E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp15D->f2);if(_tmp15E->tag != 3)goto _LL121;else{_tmp15F=_tmp15E->f1;}}}_LL120:
# 898
 _tmp162=_tmp15F;goto _LL122;_LL121: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp160->tag != 1)goto _LL123;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp161=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp160->f2);if(_tmp161->tag != 4)goto _LL123;else{_tmp162=_tmp161->f1;}}}_LL122:
 _tmp165=_tmp162;goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp163->tag != 1)goto _LL125;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp164=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp163->f2);if(_tmp164->tag != 5)goto _LL125;else{_tmp165=_tmp164->f1;}}}_LL124: {
# 902
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6C9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp6C9.tag=0,((_tmp6C9.f1=_tmp165,_tmp6C9)));
struct _tuple13 _tmp6CA;return(_tmp6CA.f1=inflow,((_tmp6CA.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp6CA)));}_LL125: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp166->tag != 2)goto _LL127;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LL126: {
# 909
union Cyc_CfFlowInfo_FlowInfo _tmp1CC;void*_tmp1CD;struct _tuple13 _tmp1CB=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp168);_tmp1CC=_tmp1CB.f1;_tmp1CD=_tmp1CB.f2;{
struct _tuple13 _tmp6CB;return(_tmp6CB.f1=_tmp1CC,((_tmp6CB.f2=_tmp1CD,_tmp6CB)));};}_LL127: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp169->tag != 4)goto _LL129;else{_tmp16A=_tmp169->f1;}}_LL128: {
# 913
struct Cyc_List_List _tmp6CC;struct Cyc_List_List _tmp1CF=(_tmp6CC.hd=_tmp16A,((_tmp6CC.tl=0,_tmp6CC)));
union Cyc_CfFlowInfo_FlowInfo _tmp1D1;struct _tuple13 _tmp1D0=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1CF);_tmp1D1=_tmp1D0.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1D3;struct _tuple14 _tmp1D2=Cyc_NewControlFlow_anal_Lexp(env,_tmp1D1,0,_tmp16A);_tmp1D3=_tmp1D2.f2;
{struct _tuple20 _tmp6CD;struct _tuple20 _tmp1D5=(_tmp6CD.f1=_tmp1D3,((_tmp6CD.f2=_tmp1D1,_tmp6CD)));struct Cyc_CfFlowInfo_Place*_tmp1D6;struct Cyc_Dict_Dict _tmp1D7;struct Cyc_List_List*_tmp1D8;_LL168: if(((_tmp1D5.f1).PlaceL).tag != 1)goto _LL16A;_tmp1D6=(struct Cyc_CfFlowInfo_Place*)((_tmp1D5.f1).PlaceL).val;if(((_tmp1D5.f2).ReachableFL).tag != 2)goto _LL16A;_tmp1D7=((struct _tuple12)((_tmp1D5.f2).ReachableFL).val).f1;_tmp1D8=((struct _tuple12)((_tmp1D5.f2).ReachableFL).val).f2;_LL169:
# 918
 _tmp1D8=Cyc_CfFlowInfo_reln_kill_exp(_tmp131->r,_tmp1D8,_tmp16A);
_tmp1D1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp131,_tmp16A->loc,_tmp1D7,env->all_changed,_tmp1D6,_tmp131->unknown_all),_tmp1D8);
# 922
goto _LL167;_LL16A:;_LL16B:
 goto _LL167;_LL167:;}{
# 925
struct _tuple13 _tmp6CE;return(_tmp6CE.f1=_tmp1D1,((_tmp6CE.f2=_tmp131->unknown_all,_tmp6CE)));};};}_LL129: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp16B->tag != 3)goto _LL12B;else{_tmp16C=_tmp16B->f1;if(_tmp16B->f2 == 0)goto _LL12B;_tmp16D=_tmp16B->f3;}}_LL12A:
# 928
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_Tcutil_terr(e->loc,((_tmp6D1="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp6D1,sizeof(char),58))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}{
struct _tuple13 _tmp6D2;return(_tmp6D2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6D2.f2=_tmp131->unknown_all,_tmp6D2)));};}{
# 932
struct Cyc_List_List _tmp6D3;struct Cyc_List_List _tmp1DE=(_tmp6D3.hd=_tmp16D,((_tmp6D3.tl=0,_tmp6D3)));
struct Cyc_List_List _tmp6D4;struct Cyc_List_List _tmp1DF=(_tmp6D4.hd=_tmp16C,((_tmp6D4.tl=& _tmp1DE,_tmp6D4)));
union Cyc_CfFlowInfo_FlowInfo _tmp1E1;struct _tuple13 _tmp1E0=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1DF);_tmp1E1=_tmp1E0.f1;{
union Cyc_CfFlowInfo_AbsLVal _tmp1E3;struct _tuple14 _tmp1E2=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E1,0,_tmp16C);_tmp1E3=_tmp1E2.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1E4=_tmp1E1;struct Cyc_Dict_Dict _tmp1E5;struct Cyc_List_List*_tmp1E6;_LL16D: if((_tmp1E4.ReachableFL).tag != 2)goto _LL16F;_tmp1E5=((struct _tuple12)(_tmp1E4.ReachableFL).val).f1;_tmp1E6=((struct _tuple12)(_tmp1E4.ReachableFL).val).f2;_LL16E:
# 938
{union Cyc_CfFlowInfo_AbsLVal _tmp1E7=_tmp1E3;struct Cyc_CfFlowInfo_Place*_tmp1E8;_LL172: if((_tmp1E7.PlaceL).tag != 1)goto _LL174;_tmp1E8=(struct Cyc_CfFlowInfo_Place*)(_tmp1E7.PlaceL).val;_LL173:
# 942
 _tmp1E6=Cyc_CfFlowInfo_reln_kill_exp(_tmp131->r,_tmp1E6,_tmp16C);
_tmp1E5=Cyc_CfFlowInfo_assign_place(_tmp131,_tmp16C->loc,_tmp1E5,env->all_changed,_tmp1E8,_tmp131->unknown_all);
# 945
_tmp1E1=Cyc_CfFlowInfo_ReachableFL(_tmp1E5,_tmp1E6);
# 948
goto _LL171;_LL174: if((_tmp1E7.UnknownL).tag != 2)goto _LL171;_LL175:
# 951
 goto _LL171;_LL171:;}
# 953
goto _LL16C;_LL16F:;_LL170:
 goto _LL16C;_LL16C:;}{
# 956
struct _tuple13 _tmp6D5;return(_tmp6D5.f1=_tmp1E1,((_tmp6D5.f2=_tmp131->unknown_all,_tmp6D5)));};};};_LL12B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp16E->tag != 3)goto _LL12D;else{_tmp16F=_tmp16E->f1;if(_tmp16E->f2 != 0)goto _LL12D;_tmp170=_tmp16E->f3;}}_LL12C:
# 960
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp6D8;void*_tmp6D7;(_tmp6D7=0,Cyc_Tcutil_terr(e->loc,((_tmp6D8="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp6D8,sizeof(char),58))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));}{
struct _tuple13 _tmp6D9;return(_tmp6D9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6D9.f2=_tmp131->unknown_all,_tmp6D9)));};}{
# 964
struct Cyc_Dict_Dict*_tmp1EF=env->all_changed;
while(1){
{struct Cyc_Dict_Dict*_tmp6DA;env->all_changed=((_tmp6DA=_region_malloc(env->r,sizeof(*_tmp6DA)),((_tmp6DA[0]=_tmp131->mt_place_set,_tmp6DA))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp1F2;union Cyc_CfFlowInfo_AbsLVal _tmp1F3;struct _tuple14 _tmp1F1=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp16F);_tmp1F2=_tmp1F1.f1;_tmp1F3=_tmp1F1.f2;{
struct Cyc_Dict_Dict _tmp1F4=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp6DB;env->all_changed=((_tmp6DB=_region_malloc(env->r,sizeof(*_tmp6DB)),((_tmp6DB[0]=_tmp131->mt_place_set,_tmp6DB))));}{
union Cyc_CfFlowInfo_FlowInfo _tmp1F7;void*_tmp1F8;struct _tuple13 _tmp1F6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp170);_tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;{
struct Cyc_Dict_Dict _tmp1F9=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp1FA=Cyc_CfFlowInfo_after_flow(_tmp131,& _tmp1F4,_tmp1F2,_tmp1F7,_tmp1F4,_tmp1F9);
# 975
union Cyc_CfFlowInfo_FlowInfo _tmp1FB=Cyc_CfFlowInfo_join_flow(_tmp131,_tmp1EF,inflow,_tmp1FA);
# 978
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1FB,inflow)){
if(_tmp1EF == 0)
env->all_changed=0;else{
# 982
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1EF,
Cyc_CfFlowInfo_union_place_set(_tmp1F4,_tmp1F9,0),0);}
# 985
return Cyc_NewControlFlow_do_assign(_tmp131,env,_tmp1FA,_tmp16F,_tmp1F3,_tmp170,_tmp1F8,e->loc);}
# 988
inflow=_tmp1FB;};};};};}};_LL12D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp171->tag != 8)goto _LL12F;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;}}_LL12E: {
# 993
union Cyc_CfFlowInfo_FlowInfo _tmp1FD;void*_tmp1FE;struct _tuple13 _tmp1FC=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp172);_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1FD,_tmp173);}_LL12F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp174->tag != 10)goto _LL131;else{_tmp175=_tmp174->f1;}}_LL130: {
# 997
union Cyc_CfFlowInfo_FlowInfo _tmp200;void*_tmp201;struct _tuple13 _tmp1FF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp175);_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp175->loc,_tmp200,_tmp201);{
struct _tuple13 _tmp6DC;return(_tmp6DC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6DC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp6DC)));};}_LL131: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp176->tag != 9)goto _LL133;else{_tmp177=_tmp176->f1;_tmp178=_tmp176->f2;}}_LL132: {
# 1002
struct _RegionHandle _tmp203=_new_region("temp");struct _RegionHandle*temp=& _tmp203;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp6DD;struct _tuple19 _tmp205=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp6DD=_region_malloc(temp,sizeof(*_tmp6DD)),((_tmp6DD->hd=_tmp177,((_tmp6DD->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp178),_tmp6DD)))))),0,1);_tmp206=_tmp205.f1;_tmp207=_tmp205.f2;{
# 1005
union Cyc_CfFlowInfo_FlowInfo _tmp208=Cyc_NewControlFlow_use_Rval(env,_tmp177->loc,_tmp206,(void*)((struct Cyc_List_List*)_check_null(_tmp207))->hd);
_tmp207=_tmp207->tl;{
# 1008
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp209=Cyc_Tcutil_compress((void*)_check_null(_tmp177->topt));void*_tmp20B;_LL177: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LL179;else{_tmp20B=(_tmp20A->f1).elt_typ;}}_LL178:
# 1013
{void*_tmp20C=Cyc_Tcutil_compress(_tmp20B);struct Cyc_List_List*_tmp20E;_LL17C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp20C;if(_tmp20D->tag != 9)goto _LL17E;else{_tmp20E=(_tmp20D->f1).attributes;}}_LL17D:
# 1015
 for(0;_tmp20E != 0;_tmp20E=_tmp20E->tl){
# 1017
void*_tmp20F=(void*)_tmp20E->hd;int _tmp211;int _tmp213;int _tmp215;_LL181: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp210=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp20F;if(_tmp210->tag != 20)goto _LL183;else{_tmp211=_tmp210->f1;}}_LL182:
# 1019
{struct Cyc_List_List*_tmp6DE;init_params=((_tmp6DE=_region_malloc(temp,sizeof(*_tmp6DE)),((_tmp6DE->hd=(void*)_tmp211,((_tmp6DE->tl=init_params,_tmp6DE))))));}goto _LL180;_LL183: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp212=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp20F;if(_tmp212->tag != 21)goto _LL185;else{_tmp213=_tmp212->f1;}}_LL184:
# 1021
{struct Cyc_List_List*_tmp6DF;nolive_unique_params=((_tmp6DF=_region_malloc(temp,sizeof(*_tmp6DF)),((_tmp6DF->hd=(void*)_tmp213,((_tmp6DF->tl=nolive_unique_params,_tmp6DF))))));}
goto _LL180;_LL185: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp214=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp20F;if(_tmp214->tag != 22)goto _LL187;else{_tmp215=_tmp214->f1;}}_LL186:
# 1025
{struct Cyc_List_List*_tmp6E0;noconsume_params=((_tmp6E0=_region_malloc(temp,sizeof(*_tmp6E0)),((_tmp6E0->hd=(void*)_tmp215,((_tmp6E0->tl=noconsume_params,_tmp6E0))))));}
goto _LL180;_LL187:;_LL188:
 goto _LL180;_LL180:;}
# 1029
goto _LL17B;_LL17E:;_LL17F: {
const char*_tmp6E3;void*_tmp6E2;(_tmp6E2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6E3="anal_Rexp: bad function type",_tag_dyneither(_tmp6E3,sizeof(char),29))),_tag_dyneither(_tmp6E2,sizeof(void*),0)));}_LL17B:;}
# 1032
goto _LL176;_LL179:;_LL17A: {
const char*_tmp6E6;void*_tmp6E5;(_tmp6E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6E6="anal_Rexp: bad function type",_tag_dyneither(_tmp6E6,sizeof(char),29))),_tag_dyneither(_tmp6E5,sizeof(void*),0)));}_LL176:;}
# 1035
{int i=1;for(0;_tmp207 != 0;(((_tmp207=_tmp207->tl,_tmp178=((struct Cyc_List_List*)_check_null(_tmp178))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp21D=_tmp206;struct Cyc_Dict_Dict _tmp21E;_LL18A: if((_tmp21D.BottomFL).tag != 1)goto _LL18C;_LL18B:
 goto _LL189;_LL18C: if((_tmp21D.ReachableFL).tag != 2)goto _LL189;_tmp21E=((struct _tuple12)(_tmp21D.ReachableFL).val).f1;_LL18D:
# 1040
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp21E,(void*)_tmp207->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6E9;void*_tmp6E8;(_tmp6E8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp178))->hd)->loc,((_tmp6E9="expression may not be initialized",_tag_dyneither(_tmp6E9,sizeof(char),34))),_tag_dyneither(_tmp6E8,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp221=_tmp208;struct Cyc_Dict_Dict _tmp222;struct Cyc_List_List*_tmp223;_LL18F: if((_tmp221.BottomFL).tag != 1)goto _LL191;_LL190:
 goto _LL18E;_LL191: if((_tmp221.ReachableFL).tag != 2)goto _LL18E;_tmp222=((struct _tuple12)(_tmp221.ReachableFL).val).f1;_tmp223=((struct _tuple12)(_tmp221.ReachableFL).val).f2;_LL192: {
# 1047
struct Cyc_Dict_Dict _tmp224=Cyc_CfFlowInfo_escape_deref(_tmp131,_tmp222,env->all_changed,(void*)_tmp207->hd);
{void*_tmp225=(void*)_tmp207->hd;struct Cyc_CfFlowInfo_Place*_tmp227;_LL194: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp226=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp225;if(_tmp226->tag != 5)goto _LL196;else{_tmp227=_tmp226->f1;}}_LL195:
# 1050
{void*_tmp228=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp178))->hd)->topt));void*_tmp22A;_LL199: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228;if(_tmp229->tag != 5)goto _LL19B;else{_tmp22A=(_tmp229->f1).elt_typ;}}_LL19A:
# 1054
 _tmp224=Cyc_CfFlowInfo_assign_place(_tmp131,((struct Cyc_Absyn_Exp*)_tmp178->hd)->loc,_tmp224,env->all_changed,_tmp227,
# 1056
Cyc_CfFlowInfo_typ_to_absrval(_tmp131,_tmp22A,0,_tmp131->esc_all));
# 1058
goto _LL198;_LL19B:;_LL19C: {
const char*_tmp6EC;void*_tmp6EB;(_tmp6EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6EC="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp6EC,sizeof(char),39))),_tag_dyneither(_tmp6EB,sizeof(void*),0)));}_LL198:;}
# 1061
goto _LL193;_LL196:;_LL197:
 goto _LL193;_LL193:;}
# 1064
_tmp208=Cyc_CfFlowInfo_ReachableFL(_tmp224,_tmp223);
goto _LL18E;}_LL18E:;}
# 1067
goto _LL189;_LL189:;}else{
# 1070
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1073
_tmp208=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp178))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp178))->hd)->topt),_tmp208,(void*)_tmp207->hd);else{
# 1080
_tmp208=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp178))->hd)->loc,_tmp208,(void*)_tmp207->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1083
union Cyc_CfFlowInfo_AbsLVal _tmp22E;struct _tuple14 _tmp22D=Cyc_NewControlFlow_anal_Lexp(env,_tmp208,1,(struct Cyc_Absyn_Exp*)_tmp178->hd);_tmp22E=_tmp22D.f2;{
struct _tuple14 _tmp6ED;struct _tuple14 _tmp230=(_tmp6ED.f1=_tmp208,((_tmp6ED.f2=_tmp22E,_tmp6ED)));struct Cyc_Dict_Dict _tmp231;struct Cyc_List_List*_tmp232;struct Cyc_CfFlowInfo_Place*_tmp233;_LL19E: if(((_tmp230.f1).ReachableFL).tag != 2)goto _LL1A0;_tmp231=((struct _tuple12)((_tmp230.f1).ReachableFL).val).f1;_tmp232=((struct _tuple12)((_tmp230.f1).ReachableFL).val).f2;if(((_tmp230.f2).PlaceL).tag != 1)goto _LL1A0;_tmp233=(struct Cyc_CfFlowInfo_Place*)((_tmp230.f2).PlaceL).val;_LL19F: {
# 1086
void*_tmp234=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp131,(void*)_tmp207->hd);
# 1089
_tmp231=Cyc_CfFlowInfo_assign_place(_tmp131,((struct Cyc_Absyn_Exp*)_tmp178->hd)->loc,_tmp231,env->all_changed,_tmp233,_tmp234);
_tmp208=Cyc_CfFlowInfo_ReachableFL(_tmp231,_tmp232);
# 1094
goto _LL19D;}_LL1A0: if(((_tmp230.f1).BottomFL).tag != 1)goto _LL1A2;_LL1A1:
 continue;_LL1A2:;_LL1A3: {
# 1097
const char*_tmp6F0;void*_tmp6EF;(_tmp6EF=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp178->hd)->loc,((_tmp6F0="noconsume() parameters must be l-values",_tag_dyneither(_tmp6F0,sizeof(char),40))),_tag_dyneither(_tmp6EF,sizeof(void*),0)));}_LL19D:;};}}}}}
# 1104
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp177->topt))){
struct _tuple13 _tmp6F1;struct _tuple13 _tmp238=(_tmp6F1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6F1.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp6F1)));_npop_handler(0);return _tmp238;}else{
# 1107
struct _tuple13 _tmp6F2;struct _tuple13 _tmp23A=(_tmp6F2.f1=_tmp208,((_tmp6F2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp6F2)));_npop_handler(0);return _tmp23A;}};};}
# 1003
;_pop_region(temp);}_LL133: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp179->tag != 33)goto _LL135;else{_tmp17A=(_tmp179->f1).is_calloc;_tmp17B=(_tmp179->f1).rgn;_tmp17C=(_tmp179->f1).elt_type;_tmp17D=(_tmp179->f1).num_elts;_tmp17E=(_tmp179->f1).fat_result;}}_LL134: {
# 1110
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp6F5;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6F4;void*root=(void*)((_tmp6F4=_region_malloc(_tmp131->r,sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F5.tag=1,((_tmp6F5.f1=_tmp17D,((_tmp6F5.f2=(void*)_check_null(e->topt),_tmp6F5)))))),_tmp6F4))));
struct Cyc_CfFlowInfo_Place*_tmp6F6;struct Cyc_CfFlowInfo_Place*place=(_tmp6F6=_region_malloc(_tmp131->r,sizeof(*_tmp6F6)),((_tmp6F6->root=root,((_tmp6F6->fields=0,_tmp6F6)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp6F9;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp6F8;void*rval=(void*)((_tmp6F8=_region_malloc(_tmp131->r,sizeof(*_tmp6F8)),((_tmp6F8[0]=((_tmp6F9.tag=5,((_tmp6F9.f1=place,_tmp6F9)))),_tmp6F8))));
void*place_val;
if(_tmp17E)place_val=_tmp131->notzeroall;else{
if(_tmp17A)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,*((void**)_check_null(_tmp17C)),0,_tmp131->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,*((void**)_check_null(_tmp17C)),0,_tmp131->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp17B != 0){
struct _RegionHandle _tmp23B=_new_region("temp");struct _RegionHandle*temp=& _tmp23B;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp23E;struct Cyc_List_List*_tmp23F;struct Cyc_Absyn_Exp*_tmp6FA[2];struct _tuple19 _tmp23D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp6FA[1]=_tmp17D,((_tmp6FA[0]=_tmp17B,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp6FA,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp23E=_tmp23D.f1;_tmp23F=_tmp23D.f2;
outflow=_tmp23E;}
# 1121
;_pop_region(temp);}else{
# 1126
union Cyc_CfFlowInfo_FlowInfo _tmp241;struct _tuple13 _tmp240=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17D);_tmp241=_tmp240.f1;
outflow=_tmp241;}{
# 1129
union Cyc_CfFlowInfo_FlowInfo _tmp242=outflow;struct Cyc_Dict_Dict _tmp243;struct Cyc_List_List*_tmp244;_LL1A5: if((_tmp242.BottomFL).tag != 1)goto _LL1A7;_LL1A6: {
struct _tuple13 _tmp6FB;return(_tmp6FB.f1=outflow,((_tmp6FB.f2=rval,_tmp6FB)));}_LL1A7: if((_tmp242.ReachableFL).tag != 2)goto _LL1A4;_tmp243=((struct _tuple12)(_tmp242.ReachableFL).val).f1;_tmp244=((struct _tuple12)(_tmp242.ReachableFL).val).f2;_LL1A8: {
# 1132
struct _tuple13 _tmp6FC;return(_tmp6FC.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp243,root,place_val),_tmp244),((_tmp6FC.f2=rval,_tmp6FC)));}_LL1A4:;};};}_LL135: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp17F->tag != 34)goto _LL137;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LL136: {
# 1136
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1142
struct _RegionHandle _tmp24C=_new_region("temp");struct _RegionHandle*temp=& _tmp24C;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_Absyn_Exp*_tmp6FD[2];struct _tuple19 _tmp24E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp6FD[1]=_tmp181,((_tmp6FD[0]=_tmp180,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp6FD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp250))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp250->tl))->hd;
outflow=_tmp24F;}{
# 1150
union Cyc_CfFlowInfo_AbsLVal _tmp252;struct _tuple14 _tmp251=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp180);_tmp252=_tmp251.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp254;struct _tuple14 _tmp253=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp181);_tmp254=_tmp253.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp255=outflow;struct Cyc_Dict_Dict _tmp256;struct Cyc_List_List*_tmp257;_LL1AA: if((_tmp255.ReachableFL).tag != 2)goto _LL1AC;_tmp256=((struct _tuple12)(_tmp255.ReachableFL).val).f1;_tmp257=((struct _tuple12)(_tmp255.ReachableFL).val).f2;_LL1AB:
# 1154
{union Cyc_CfFlowInfo_AbsLVal _tmp258=_tmp252;struct Cyc_CfFlowInfo_Place*_tmp259;_LL1AF: if((_tmp258.PlaceL).tag != 1)goto _LL1B1;_tmp259=(struct Cyc_CfFlowInfo_Place*)(_tmp258.PlaceL).val;_LL1B0:
# 1156
 _tmp256=Cyc_CfFlowInfo_assign_place(_tmp131,_tmp180->loc,_tmp256,env->all_changed,_tmp259,right_rval);
# 1158
goto _LL1AE;_LL1B1: if((_tmp258.UnknownL).tag != 2)goto _LL1AE;_LL1B2:
 goto _LL1AE;_LL1AE:;}
# 1161
{union Cyc_CfFlowInfo_AbsLVal _tmp25A=_tmp254;struct Cyc_CfFlowInfo_Place*_tmp25B;_LL1B4: if((_tmp25A.PlaceL).tag != 1)goto _LL1B6;_tmp25B=(struct Cyc_CfFlowInfo_Place*)(_tmp25A.PlaceL).val;_LL1B5:
# 1163
 _tmp256=Cyc_CfFlowInfo_assign_place(_tmp131,_tmp181->loc,_tmp256,env->all_changed,_tmp25B,left_rval);
# 1165
goto _LL1B3;_LL1B6: if((_tmp25A.UnknownL).tag != 2)goto _LL1B3;_LL1B7:
 goto _LL1B3;_LL1B3:;}
# 1169
_tmp257=Cyc_CfFlowInfo_reln_kill_exp(_tmp131->r,_tmp257,_tmp180);
_tmp257=Cyc_CfFlowInfo_reln_kill_exp(_tmp131->r,_tmp257,_tmp181);
# 1172
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp256,_tmp257);
goto _LL1A9;_LL1AC:;_LL1AD:
 goto _LL1A9;_LL1A9:;}{
# 1178
struct _tuple13 _tmp6FE;struct _tuple13 _tmp25D=(_tmp6FE.f1=outflow,((_tmp6FE.f2=_tmp131->unknown_all,_tmp6FE)));_npop_handler(0);return _tmp25D;};};};
# 1142
;_pop_region(temp);}_LL137: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp182->tag != 15)goto _LL139;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LL138: {
# 1181
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp701;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp700;void*root=(void*)((_tmp700=_region_malloc(_tmp131->r,sizeof(*_tmp700)),((_tmp700[0]=((_tmp701.tag=1,((_tmp701.f1=_tmp184,((_tmp701.f2=(void*)_check_null(e->topt),_tmp701)))))),_tmp700))));
struct Cyc_CfFlowInfo_Place*_tmp702;struct Cyc_CfFlowInfo_Place*place=(_tmp702=_region_malloc(_tmp131->r,sizeof(*_tmp702)),((_tmp702->root=root,((_tmp702->fields=0,_tmp702)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp705;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp704;void*rval=(void*)((_tmp704=_region_malloc(_tmp131->r,sizeof(*_tmp704)),((_tmp704[0]=((_tmp705.tag=5,((_tmp705.f1=place,_tmp705)))),_tmp704))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp183 != 0){
struct _RegionHandle _tmp25E=_new_region("temp");struct _RegionHandle*temp=& _tmp25E;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp261;struct Cyc_List_List*_tmp262;struct Cyc_Absyn_Exp*_tmp706[2];struct _tuple19 _tmp260=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp706[1]=_tmp184,((_tmp706[0]=_tmp183,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp706,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;
outflow=_tmp261;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp262))->tl))->hd;}
# 1189
;_pop_region(temp);}else{
# 1195
union Cyc_CfFlowInfo_FlowInfo _tmp264;void*_tmp265;struct _tuple13 _tmp263=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp184);_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;
outflow=_tmp264;
place_val=_tmp265;}{
# 1199
union Cyc_CfFlowInfo_FlowInfo _tmp266=outflow;struct Cyc_Dict_Dict _tmp267;struct Cyc_List_List*_tmp268;_LL1B9: if((_tmp266.BottomFL).tag != 1)goto _LL1BB;_LL1BA: {
struct _tuple13 _tmp707;return(_tmp707.f1=outflow,((_tmp707.f2=rval,_tmp707)));}_LL1BB: if((_tmp266.ReachableFL).tag != 2)goto _LL1B8;_tmp267=((struct _tuple12)(_tmp266.ReachableFL).val).f1;_tmp268=((struct _tuple12)(_tmp266.ReachableFL).val).f2;_LL1BC: {
# 1202
struct _tuple13 _tmp708;return(_tmp708.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp267,root,place_val),_tmp268),((_tmp708.f2=rval,_tmp708)));}_LL1B8:;};};}_LL139: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp185->tag != 14)goto _LL13B;else{_tmp186=_tmp185->f1;}}_LL13A: {
# 1206
union Cyc_CfFlowInfo_FlowInfo _tmp271;union Cyc_CfFlowInfo_AbsLVal _tmp272;struct _tuple14 _tmp270=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp186);_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp273=_tmp272;struct Cyc_CfFlowInfo_Place*_tmp274;_LL1BE: if((_tmp273.UnknownL).tag != 2)goto _LL1C0;_LL1BF: {
struct _tuple13 _tmp709;return(_tmp709.f1=_tmp271,((_tmp709.f2=_tmp131->notzeroall,_tmp709)));}_LL1C0: if((_tmp273.PlaceL).tag != 1)goto _LL1BD;_tmp274=(struct Cyc_CfFlowInfo_Place*)(_tmp273.PlaceL).val;_LL1C1: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp70F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp70E;struct _tuple13 _tmp70D;return(_tmp70D.f1=_tmp271,((_tmp70D.f2=(void*)((_tmp70F=_region_malloc(env->r,sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp70E.tag=5,((_tmp70E.f1=_tmp274,_tmp70E)))),_tmp70F)))),_tmp70D)));}_LL1BD:;};}_LL13B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp187->tag != 19)goto _LL13D;else{_tmp188=_tmp187->f1;}}_LL13C: {
# 1213
union Cyc_CfFlowInfo_FlowInfo _tmp27A;void*_tmp27B;struct _tuple13 _tmp279=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp188);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp27A,_tmp188,_tmp27B);}_LL13D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp189->tag != 20)goto _LL13F;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LL13E: {
# 1218
union Cyc_CfFlowInfo_FlowInfo _tmp27D;void*_tmp27E;struct _tuple13 _tmp27C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp18A);_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp18A->topt))){
struct _tuple13 _tmp710;return(_tmp710.f1=_tmp27D,((_tmp710.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp710)));}{
void*_tmp280=_tmp27E;int _tmp283;int _tmp284;struct _dyneither_ptr _tmp285;int _tmp287;int _tmp288;struct _dyneither_ptr _tmp289;_LL1C3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp281=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp280;if(_tmp281->tag != 8)goto _LL1C5;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp282=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp281->f2);if(_tmp282->tag != 6)goto _LL1C5;else{_tmp283=(_tmp282->f1).is_union;_tmp284=(_tmp282->f1).fieldnum;_tmp285=_tmp282->f2;}}}_LL1C4:
# 1223
 _tmp287=_tmp283;_tmp288=_tmp284;_tmp289=_tmp285;goto _LL1C6;_LL1C5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp286=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp280;if(_tmp286->tag != 6)goto _LL1C7;else{_tmp287=(_tmp286->f1).is_union;_tmp288=(_tmp286->f1).fieldnum;_tmp289=_tmp286->f2;}}_LL1C6: {
# 1225
int _tmp28A=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp18A->topt),_tmp18B);
if((_tmp287  && _tmp288 != - 1) && _tmp288 != _tmp28A){
struct _tuple13 _tmp711;return(_tmp711.f1=_tmp27D,((_tmp711.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),1,_tmp131->unknown_none),_tmp711)));}{
struct _tuple13 _tmp712;return(_tmp712.f1=_tmp27D,((_tmp712.f2=*((void**)_check_dyneither_subscript(_tmp289,sizeof(void*),_tmp28A)),_tmp712)));};}_LL1C7:;_LL1C8: {
# 1230
struct Cyc_Core_Impossible_exn_struct _tmp71F;const char*_tmp71E;void*_tmp71D[1];struct Cyc_String_pa_PrintArg_struct _tmp71C;struct Cyc_Core_Impossible_exn_struct*_tmp71B;(int)_throw((void*)((_tmp71B=_cycalloc(sizeof(*_tmp71B)),((_tmp71B[0]=((_tmp71F.tag=Cyc_Core_Impossible,((_tmp71F.f1=(struct _dyneither_ptr)((_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp71D[0]=& _tmp71C,Cyc_aprintf(((_tmp71E="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp71E,sizeof(char),26))),_tag_dyneither(_tmp71D,sizeof(void*),1)))))))),_tmp71F)))),_tmp71B)))));}_LL1C2:;};}_LL13F: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp18C->tag != 37)goto _LL141;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL140: {
# 1237
union Cyc_CfFlowInfo_FlowInfo _tmp293;void*_tmp294;struct _tuple13 _tmp292=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp18D);_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp18D->topt))){
struct _tuple13 _tmp720;return(_tmp720.f1=_tmp293,((_tmp720.f2=_tmp131->unknown_all,_tmp720)));}{
void*_tmp296=_tmp294;int _tmp299;int _tmp29A;struct _dyneither_ptr _tmp29B;int _tmp29D;int _tmp29E;struct _dyneither_ptr _tmp29F;_LL1CA: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp297=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp296;if(_tmp297->tag != 8)goto _LL1CC;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp298=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp297->f2);if(_tmp298->tag != 6)goto _LL1CC;else{_tmp299=(_tmp298->f1).is_union;_tmp29A=(_tmp298->f1).fieldnum;_tmp29B=_tmp298->f2;}}}_LL1CB:
# 1242
 _tmp29D=_tmp299;_tmp29E=_tmp29A;_tmp29F=_tmp29B;goto _LL1CD;_LL1CC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp29C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp296;if(_tmp29C->tag != 6)goto _LL1CE;else{_tmp29D=(_tmp29C->f1).is_union;_tmp29E=(_tmp29C->f1).fieldnum;_tmp29F=_tmp29C->f2;}}_LL1CD: {
# 1244
int _tmp2A0=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp18D->topt),_tmp18E);
if(_tmp29D  && _tmp29E != - 1){
if(_tmp29E != _tmp2A0){
struct _tuple13 _tmp721;return(_tmp721.f1=_tmp293,((_tmp721.f2=_tmp131->zero,_tmp721)));}else{
# 1249
struct _tuple13 _tmp722;return(_tmp722.f1=_tmp293,((_tmp722.f2=_tmp131->notzeroall,_tmp722)));}}else{
# 1251
struct _tuple13 _tmp723;return(_tmp723.f1=_tmp293,((_tmp723.f2=_tmp131->unknown_all,_tmp723)));}}_LL1CE:;_LL1CF: {
# 1253
struct Cyc_Core_Impossible_exn_struct _tmp730;const char*_tmp72F;void*_tmp72E[1];struct Cyc_String_pa_PrintArg_struct _tmp72D;struct Cyc_Core_Impossible_exn_struct*_tmp72C;(int)_throw((void*)((_tmp72C=_cycalloc(sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp730.tag=Cyc_Core_Impossible,((_tmp730.f1=(struct _dyneither_ptr)((_tmp72D.tag=0,((_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp72E[0]=& _tmp72D,Cyc_aprintf(((_tmp72F="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp72F,sizeof(char),26))),_tag_dyneither(_tmp72E,sizeof(void*),1)))))))),_tmp730)))),_tmp72C)))));}_LL1C9:;};}_LL141: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp18F->tag != 21)goto _LL143;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LL142: {
# 1258
union Cyc_CfFlowInfo_FlowInfo _tmp2AA;void*_tmp2AB;struct _tuple13 _tmp2A9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp190);_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;{
# 1261
union Cyc_CfFlowInfo_FlowInfo _tmp2AD;void*_tmp2AE;struct _tuple13 _tmp2AC=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2AA,_tmp190,_tmp2AB);_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;{
# 1264
void*_tmp2AF=Cyc_Tcutil_compress((void*)_check_null(_tmp190->topt));void*_tmp2B1;_LL1D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2B0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AF;if(_tmp2B0->tag != 5)goto _LL1D3;else{_tmp2B1=(_tmp2B0->f1).elt_typ;}}_LL1D2:
# 1266
 if(Cyc_Absyn_is_nontagged_union_type(_tmp2B1)){
struct _tuple13 _tmp731;return(_tmp731.f1=_tmp2AD,((_tmp731.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp731)));}{
void*_tmp2B3=_tmp2AE;int _tmp2B6;int _tmp2B7;struct _dyneither_ptr _tmp2B8;int _tmp2BA;int _tmp2BB;struct _dyneither_ptr _tmp2BC;_LL1D6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B3;if(_tmp2B4->tag != 8)goto _LL1D8;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2B4->f2);if(_tmp2B5->tag != 6)goto _LL1D8;else{_tmp2B6=(_tmp2B5->f1).is_union;_tmp2B7=(_tmp2B5->f1).fieldnum;_tmp2B8=_tmp2B5->f2;}}}_LL1D7:
# 1270
 _tmp2BA=_tmp2B6;_tmp2BB=_tmp2B7;_tmp2BC=_tmp2B8;goto _LL1D9;_LL1D8: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2B9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B3;if(_tmp2B9->tag != 6)goto _LL1DA;else{_tmp2BA=(_tmp2B9->f1).is_union;_tmp2BB=(_tmp2B9->f1).fieldnum;_tmp2BC=_tmp2B9->f2;}}_LL1D9: {
# 1272
int _tmp2BD=Cyc_CfFlowInfo_get_field_index(_tmp2B1,_tmp191);
if((_tmp2BA  && _tmp2BB != - 1) && _tmp2BB != _tmp2BD){
struct _tuple13 _tmp732;return(_tmp732.f1=_tmp2AD,((_tmp732.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),1,_tmp131->unknown_none),_tmp732)));}{
struct _tuple13 _tmp733;return(_tmp733.f1=_tmp2AD,((_tmp733.f2=*((void**)_check_dyneither_subscript(_tmp2BC,sizeof(void*),_tmp2BD)),_tmp733)));};}_LL1DA:;_LL1DB: {
struct Cyc_Core_Impossible_exn_struct _tmp739;const char*_tmp738;struct Cyc_Core_Impossible_exn_struct*_tmp737;(int)_throw((void*)((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737[0]=((_tmp739.tag=Cyc_Core_Impossible,((_tmp739.f1=((_tmp738="anal_Rexp: AggrArrow",_tag_dyneither(_tmp738,sizeof(char),21))),_tmp739)))),_tmp737)))));}_LL1D5:;};_LL1D3:;_LL1D4: {
# 1278
struct Cyc_Core_Impossible_exn_struct _tmp73F;const char*_tmp73E;struct Cyc_Core_Impossible_exn_struct*_tmp73D;(int)_throw((void*)((_tmp73D=_cycalloc(sizeof(*_tmp73D)),((_tmp73D[0]=((_tmp73F.tag=Cyc_Core_Impossible,((_tmp73F.f1=((_tmp73E="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp73E,sizeof(char),25))),_tmp73F)))),_tmp73D)))));}_LL1D0:;};};}_LL143: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp192->tag != 5)goto _LL145;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;_tmp195=_tmp192->f3;}}_LL144: {
# 1282
union Cyc_CfFlowInfo_FlowInfo _tmp2C7;union Cyc_CfFlowInfo_FlowInfo _tmp2C8;struct _tuple15 _tmp2C6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp193);_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;{
struct _tuple13 _tmp2C9=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2C7,_tmp194);
struct _tuple13 _tmp2CA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2C8,_tmp195);
# 1286
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp131,env->all_changed,_tmp2C9,_tmp2CA);};}_LL145: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp196->tag != 6)goto _LL147;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LL146: {
# 1289
union Cyc_CfFlowInfo_FlowInfo _tmp2CC;union Cyc_CfFlowInfo_FlowInfo _tmp2CD;struct _tuple15 _tmp2CB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp197);_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2CF;void*_tmp2D0;struct _tuple13 _tmp2CE=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2CC,_tmp198);_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;{
struct _tuple13 _tmp740;struct _tuple13 _tmp2D1=(_tmp740.f1=_tmp2CF,((_tmp740.f2=_tmp2D0,_tmp740)));
struct _tuple13 _tmp741;struct _tuple13 _tmp2D2=(_tmp741.f1=_tmp2CD,((_tmp741.f2=_tmp131->zero,_tmp741)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp131,env->all_changed,_tmp2D1,_tmp2D2);};};}_LL147: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp199->tag != 7)goto _LL149;else{_tmp19A=_tmp199->f1;_tmp19B=_tmp199->f2;}}_LL148: {
# 1296
union Cyc_CfFlowInfo_FlowInfo _tmp2D6;union Cyc_CfFlowInfo_FlowInfo _tmp2D7;struct _tuple15 _tmp2D5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp19A);_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2D9;void*_tmp2DA;struct _tuple13 _tmp2D8=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D7,_tmp19B);_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;{
struct _tuple13 _tmp742;struct _tuple13 _tmp2DB=(_tmp742.f1=_tmp2D9,((_tmp742.f2=_tmp2DA,_tmp742)));
struct _tuple13 _tmp743;struct _tuple13 _tmp2DC=(_tmp743.f1=_tmp2D6,((_tmp743.f2=_tmp131->notzeroall,_tmp743)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp131,env->all_changed,_tmp2DB,_tmp2DC);};};}_LL149: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp19C->tag != 22)goto _LL14B;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}_LL14A: {
# 1303
struct _RegionHandle _tmp2DF=_new_region("temp");struct _RegionHandle*temp=& _tmp2DF;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_Absyn_Exp*_tmp744[2];struct _tuple19 _tmp2E1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp744[1]=_tmp19E,((_tmp744[0]=_tmp19D,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp744,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{
# 1308
union Cyc_CfFlowInfo_FlowInfo _tmp2E4=_tmp2E2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2E5=_tmp2E2;struct Cyc_Dict_Dict _tmp2E6;struct Cyc_List_List*_tmp2E7;_LL1DD: if((_tmp2E5.ReachableFL).tag != 2)goto _LL1DF;_tmp2E6=((struct _tuple12)(_tmp2E5.ReachableFL).val).f1;_tmp2E7=((struct _tuple12)(_tmp2E5.ReachableFL).val).f2;_LL1DE:
# 1313
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2E6,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2E3))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp747;void*_tmp746;(_tmp746=0,Cyc_Tcutil_terr(_tmp19E->loc,((_tmp747="expression may not be initialized",_tag_dyneither(_tmp747,sizeof(char),34))),_tag_dyneither(_tmp746,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2EA=Cyc_NewControlFlow_add_subscript_reln(_tmp131->r,_tmp2E7,_tmp19D,_tmp19E);
if(_tmp2E7 != _tmp2EA)
_tmp2E4=Cyc_CfFlowInfo_ReachableFL(_tmp2E6,_tmp2EA);
goto _LL1DC;};_LL1DF:;_LL1E0:
 goto _LL1DC;_LL1DC:;}{
# 1321
void*_tmp2EB=Cyc_Tcutil_compress((void*)_check_null(_tmp19D->topt));struct Cyc_List_List*_tmp2ED;union Cyc_Absyn_Constraint*_tmp2EF;_LL1E2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2EB;if(_tmp2EC->tag != 10)goto _LL1E4;else{_tmp2ED=_tmp2EC->f1;}}_LL1E3: {
# 1323
void*_tmp2F0=(void*)((struct Cyc_List_List*)_check_null(_tmp2E3))->hd;struct _dyneither_ptr _tmp2F3;struct _dyneither_ptr _tmp2F5;_LL1E9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2F1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2F0;if(_tmp2F1->tag != 8)goto _LL1EB;else{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F2=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)((void*)_tmp2F1->f2);if(_tmp2F2->tag != 6)goto _LL1EB;else{_tmp2F3=_tmp2F2->f2;}}}_LL1EA:
# 1325
 _tmp2F5=_tmp2F3;goto _LL1EC;_LL1EB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F0;if(_tmp2F4->tag != 6)goto _LL1ED;else{_tmp2F5=_tmp2F4->f2;}}_LL1EC: {
# 1327
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp19E)).f1;
struct _tuple13 _tmp748;struct _tuple13 _tmp2F7=(_tmp748.f1=_tmp2E4,((_tmp748.f2=*((void**)_check_dyneither_subscript(_tmp2F5,sizeof(void*),(int)i)),_tmp748)));_npop_handler(0);return _tmp2F7;}_LL1ED:;_LL1EE: {
struct Cyc_Core_Impossible_exn_struct _tmp74E;const char*_tmp74D;struct Cyc_Core_Impossible_exn_struct*_tmp74C;(int)_throw((void*)((_tmp74C=_cycalloc(sizeof(*_tmp74C)),((_tmp74C[0]=((_tmp74E.tag=Cyc_Core_Impossible,((_tmp74E.f1=((_tmp74D="anal_Rexp: Subscript",_tag_dyneither(_tmp74D,sizeof(char),21))),_tmp74E)))),_tmp74C)))));}_LL1E8:;}_LL1E4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB;if(_tmp2EE->tag != 5)goto _LL1E6;else{_tmp2EF=((_tmp2EE->f1).ptr_atts).bounds;}}_LL1E5: {
# 1332
union Cyc_CfFlowInfo_FlowInfo _tmp2FC;void*_tmp2FD;struct _tuple13 _tmp2FB=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E2,_tmp19D,(void*)((struct Cyc_List_List*)_check_null(_tmp2E3))->hd);_tmp2FC=_tmp2FB.f1;_tmp2FD=_tmp2FB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp2FE=_tmp2FC;_LL1F0: if((_tmp2FE.BottomFL).tag != 1)goto _LL1F2;_LL1F1: {
struct _tuple13 _tmp74F;struct _tuple13 _tmp300=(_tmp74F.f1=_tmp2FC,((_tmp74F.f2=_tmp2FD,_tmp74F)));_npop_handler(0);return _tmp300;}_LL1F2:;_LL1F3: {
struct _tuple13 _tmp750;struct _tuple13 _tmp302=(_tmp750.f1=_tmp2E4,((_tmp750.f2=_tmp2FD,_tmp750)));_npop_handler(0);return _tmp302;}_LL1EF:;};}_LL1E6:;_LL1E7: {
# 1337
struct Cyc_Core_Impossible_exn_struct _tmp756;const char*_tmp755;struct Cyc_Core_Impossible_exn_struct*_tmp754;(int)_throw((void*)((_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754[0]=((_tmp756.tag=Cyc_Core_Impossible,((_tmp756.f1=((_tmp755="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp755,sizeof(char),33))),_tmp756)))),_tmp754)))));}_LL1E1:;};};}
# 1304
;_pop_region(temp);}_LL14B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp19F->tag != 30)goto _LL14D;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL14C:
# 1342
 _tmp1A3=_tmp1A0;goto _LL14E;_LL14D: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1A2->tag != 23)goto _LL14F;else{_tmp1A3=_tmp1A2->f1;}}_LL14E: {
# 1344
struct _RegionHandle _tmp306=_new_region("temp");struct _RegionHandle*temp=& _tmp306;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp308;struct Cyc_List_List*_tmp309;struct _tuple19 _tmp307=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1A3,1,1);_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;{
unsigned int _tmp75C;unsigned int _tmp75B;struct _dyneither_ptr _tmp75A;void**_tmp759;unsigned int _tmp758;struct _dyneither_ptr aggrdict=
(_tmp758=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1A3),((_tmp759=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp758)),((_tmp75A=_tag_dyneither(_tmp759,sizeof(void*),_tmp758),((((_tmp75B=_tmp758,_tmp75D(& _tmp75C,& _tmp75B,& _tmp759,& _tmp309))),_tmp75A)))))));
# 1352
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp767;struct Cyc_CfFlowInfo_UnionRInfo _tmp766;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp765;struct _tuple13 _tmp764;struct _tuple13 _tmp30E=(_tmp764.f1=_tmp308,((_tmp764.f2=(void*)((_tmp767=_region_malloc(env->r,sizeof(*_tmp767)),((_tmp767[0]=((_tmp765.tag=6,((_tmp765.f1=((_tmp766.is_union=0,((_tmp766.fieldnum=- 1,_tmp766)))),((_tmp765.f2=aggrdict,_tmp765)))))),_tmp767)))),_tmp764)));_npop_handler(0);return _tmp30E;};}
# 1345
;_pop_region(temp);}_LL14F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1A4->tag != 29)goto _LL151;else{_tmp1A5=_tmp1A4->f2;}}_LL150: {
# 1355
struct Cyc_List_List*fs;
{void*_tmp314=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp316;_LL1F5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp314;if(_tmp315->tag != 12)goto _LL1F7;else{_tmp316=_tmp315->f2;}}_LL1F6:
 fs=_tmp316;goto _LL1F4;_LL1F7:;_LL1F8: {
struct Cyc_Core_Impossible_exn_struct _tmp76D;const char*_tmp76C;struct Cyc_Core_Impossible_exn_struct*_tmp76B;(int)_throw((void*)((_tmp76B=_cycalloc(sizeof(*_tmp76B)),((_tmp76B[0]=((_tmp76D.tag=Cyc_Core_Impossible,((_tmp76D.f1=((_tmp76C="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp76C,sizeof(char),35))),_tmp76D)))),_tmp76B)))));}_LL1F4:;}
# 1360
_tmp1A7=_tmp1A5;_tmp1A8=Cyc_Absyn_StructA;_tmp1A9=fs;goto _LL152;}_LL151: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1A6->tag != 28)goto _LL153;else{_tmp1A7=_tmp1A6->f3;if(_tmp1A6->f4 == 0)goto _LL153;_tmp1A8=(_tmp1A6->f4)->kind;if((_tmp1A6->f4)->impl == 0)goto _LL153;_tmp1A9=((_tmp1A6->f4)->impl)->fields;}}_LL152: {
# 1362
struct _RegionHandle _tmp31A=_new_region("temp");struct _RegionHandle*temp=& _tmp31A;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp31C;struct Cyc_List_List*_tmp31D;struct _tuple19 _tmp31B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple21*))Cyc_Core_snd,_tmp1A7),1,1);_tmp31C=_tmp31B.f1;_tmp31D=_tmp31B.f2;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp131,_tmp1A9,_tmp1A8 == Cyc_Absyn_UnionA,_tmp1A8 == Cyc_Absyn_UnionA?_tmp131->unknown_none: _tmp131->unknown_all);
int field_no=- 1;
{int i=0;for(0;_tmp31D != 0;(((_tmp31D=_tmp31D->tl,_tmp1A7=_tmp1A7->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple21*)((struct Cyc_List_List*)_check_null(_tmp1A7))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp31E=(void*)ds->hd;struct _dyneither_ptr*_tmp321;_LL1FA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp31F=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp31E;if(_tmp31F->tag != 0)goto _LL1FC;}_LL1FB: {
struct Cyc_Core_Impossible_exn_struct _tmp773;const char*_tmp772;struct Cyc_Core_Impossible_exn_struct*_tmp771;(int)_throw((void*)((_tmp771=_cycalloc(sizeof(*_tmp771)),((_tmp771[0]=((_tmp773.tag=Cyc_Core_Impossible,((_tmp773.f1=((_tmp772="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp772,sizeof(char),22))),_tmp773)))),_tmp771)))));}_LL1FC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp320=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp31E;if(_tmp320->tag != 1)goto _LL1F9;else{_tmp321=_tmp320->f1;}}_LL1FD:
# 1374
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1A9,_tmp321);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp31D->hd;
# 1377
if(_tmp1A8 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp325=(*((struct _tuple21*)_tmp1A7->hd)).f2;
_tmp31C=Cyc_NewControlFlow_use_Rval(env,_tmp325->loc,_tmp31C,(void*)_tmp31D->hd);}_LL1F9:;}}}{
# 1382
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp77D;struct Cyc_CfFlowInfo_UnionRInfo _tmp77C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp77B;struct _tuple13 _tmp77A;struct _tuple13 _tmp32A=(_tmp77A.f1=_tmp31C,((_tmp77A.f2=(void*)((_tmp77D=_region_malloc(env->r,sizeof(*_tmp77D)),((_tmp77D[0]=((_tmp77B.tag=6,((_tmp77B.f1=((_tmp77C.is_union=_tmp1A8 == Cyc_Absyn_UnionA,((_tmp77C.fieldnum=field_no,_tmp77C)))),((_tmp77B.f2=aggrdict,_tmp77B)))))),_tmp77D)))),_tmp77A)));_npop_handler(0);return _tmp32A;};};}
# 1363
;_pop_region(temp);}_LL153: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1AA->tag != 28)goto _LL155;}_LL154: {
# 1385
struct Cyc_Core_Impossible_exn_struct _tmp783;const char*_tmp782;struct Cyc_Core_Impossible_exn_struct*_tmp781;(int)_throw((void*)((_tmp781=_cycalloc(sizeof(*_tmp781)),((_tmp781[0]=((_tmp783.tag=Cyc_Core_Impossible,((_tmp783.f1=((_tmp782="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp782,sizeof(char),31))),_tmp783)))),_tmp781)))));}_LL155: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1AB->tag != 25)goto _LL157;else{_tmp1AC=_tmp1AB->f1;}}_LL156: {
# 1387
struct _RegionHandle _tmp32E=_new_region("temp");struct _RegionHandle*temp=& _tmp32E;_push_region(temp);
{struct Cyc_List_List*_tmp32F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple21*))Cyc_Core_snd,_tmp1AC);
union Cyc_CfFlowInfo_FlowInfo _tmp331;struct Cyc_List_List*_tmp332;struct _tuple19 _tmp330=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp32F,1,1);_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;
for(0;_tmp332 != 0;(_tmp332=_tmp332->tl,_tmp32F=_tmp32F->tl)){
_tmp331=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp32F))->hd)->loc,_tmp331,(void*)_tmp332->hd);}{
struct _tuple13 _tmp784;struct _tuple13 _tmp334=(_tmp784.f1=_tmp331,((_tmp784.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,(void*)_check_null(e->topt),0,_tmp131->unknown_all),_tmp784)));_npop_handler(0);return _tmp334;};}
# 1388
;_pop_region(temp);}_LL157: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1AD->tag != 26)goto _LL159;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;_tmp1B0=_tmp1AD->f3;_tmp1B1=_tmp1AD->f4;}}_LL158: {
# 1396
union Cyc_CfFlowInfo_FlowInfo _tmp336;void*_tmp337;struct _tuple13 _tmp335=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1AF);_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp338=_tmp336;struct Cyc_Dict_Dict _tmp339;struct Cyc_List_List*_tmp33A;_LL1FF: if((_tmp338.BottomFL).tag != 1)goto _LL201;_LL200: {
struct _tuple13 _tmp785;return(_tmp785.f1=_tmp336,((_tmp785.f2=_tmp131->unknown_all,_tmp785)));}_LL201: if((_tmp338.ReachableFL).tag != 2)goto _LL1FE;_tmp339=((struct _tuple12)(_tmp338.ReachableFL).val).f1;_tmp33A=((struct _tuple12)(_tmp338.ReachableFL).val).f2;_LL202:
# 1400
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp339,_tmp337)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp788;void*_tmp787;(_tmp787=0,Cyc_Tcutil_terr(_tmp1AF->loc,((_tmp788="expression may not be initialized",_tag_dyneither(_tmp788,sizeof(char),34))),_tag_dyneither(_tmp787,sizeof(void*),0)));}{
# 1404
struct Cyc_List_List*new_relns=_tmp33A;
union Cyc_CfFlowInfo_RelnOp n1=Cyc_CfFlowInfo_RVar(_tmp1AE);
union Cyc_CfFlowInfo_RelnOp n2=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(_tmp1AF,& n2))
new_relns=Cyc_CfFlowInfo_add_relation(env->r,n1,Cyc_CfFlowInfo_Rlt,n2,_tmp33A);
# 1410
if(_tmp33A != new_relns)
_tmp336=Cyc_CfFlowInfo_ReachableFL(_tmp339,new_relns);{
# 1414
void*_tmp33E=_tmp337;_LL204: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp33F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp33E;if(_tmp33F->tag != 0)goto _LL206;}_LL205: {
struct _tuple13 _tmp789;return(_tmp789.f1=_tmp336,((_tmp789.f2=_tmp131->unknown_all,_tmp789)));}_LL206: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp340=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp33E;if(_tmp340->tag != 2)goto _LL208;}_LL207:
 goto _LL209;_LL208: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp341=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp33E;if(_tmp341->tag != 1)goto _LL20A;}_LL209:
 goto _LL20B;_LL20A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp342=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp33E;if(_tmp342->tag != 5)goto _LL20C;}_LL20B: {
# 1419
struct Cyc_List_List _tmp78A;struct Cyc_List_List _tmp344=(_tmp78A.hd=_tmp1AE,((_tmp78A.tl=0,_tmp78A)));
_tmp336=Cyc_NewControlFlow_add_vars(_tmp131,_tmp336,& _tmp344,_tmp131->unknown_all,e->loc,0);{
# 1422
union Cyc_CfFlowInfo_FlowInfo _tmp346;void*_tmp347;struct _tuple13 _tmp345=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp336,_tmp1B0);_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp348=_tmp346;struct Cyc_Dict_Dict _tmp349;_LL20F: if((_tmp348.BottomFL).tag != 1)goto _LL211;_LL210: {
struct _tuple13 _tmp78B;return(_tmp78B.f1=_tmp346,((_tmp78B.f2=_tmp131->unknown_all,_tmp78B)));}_LL211: if((_tmp348.ReachableFL).tag != 2)goto _LL20E;_tmp349=((struct _tuple12)(_tmp348.ReachableFL).val).f1;_LL212:
# 1426
 if(Cyc_CfFlowInfo_initlevel(_tmp131,_tmp349,_tmp347)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(_tmp1AF->loc,((_tmp78E="expression may not be initialized",_tag_dyneither(_tmp78E,sizeof(char),34))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}{
struct _tuple13 _tmp78F;return(_tmp78F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp78F.f2=_tmp131->unknown_all,_tmp78F)));};}_LL20E:;}
# 1431
_tmp336=_tmp346;
goto _LL20D;};}_LL20C:;_LL20D:
# 1434
 while(1){
struct Cyc_List_List _tmp790;struct Cyc_List_List _tmp34F=(_tmp790.hd=_tmp1AE,((_tmp790.tl=0,_tmp790)));
_tmp336=Cyc_NewControlFlow_add_vars(_tmp131,_tmp336,& _tmp34F,_tmp131->unknown_all,e->loc,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp351;void*_tmp352;struct _tuple13 _tmp350=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp336,_tmp1B0);_tmp351=_tmp350.f1;_tmp352=_tmp350.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp353=_tmp351;struct Cyc_Dict_Dict _tmp354;_LL214: if((_tmp353.BottomFL).tag != 1)goto _LL216;_LL215:
 goto _LL213;_LL216: if((_tmp353.ReachableFL).tag != 2)goto _LL213;_tmp354=((struct _tuple12)(_tmp353.ReachableFL).val).f1;_LL217:
# 1441
 if(Cyc_CfFlowInfo_initlevel(_tmp131,_tmp354,_tmp352)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp793;void*_tmp792;(_tmp792=0,Cyc_Tcutil_terr(_tmp1AF->loc,((_tmp793="expression may not be initialized",_tag_dyneither(_tmp793,sizeof(char),34))),_tag_dyneither(_tmp792,sizeof(void*),0)));}{
struct _tuple13 _tmp794;return(_tmp794.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp794.f2=_tmp131->unknown_all,_tmp794)));};}_LL213:;}{
# 1446
union Cyc_CfFlowInfo_FlowInfo _tmp358=Cyc_CfFlowInfo_join_flow(_tmp131,env->all_changed,_tmp336,_tmp351);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp358,_tmp336))
break;
_tmp336=_tmp358;};};}{
# 1451
struct _tuple13 _tmp795;return(_tmp795.f1=_tmp336,((_tmp795.f2=_tmp131->unknown_all,_tmp795)));};_LL203:;};};_LL1FE:;};}_LL159: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1B2->tag != 27)goto _LL15B;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=(void*)_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;}}_LL15A: {
# 1457
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp798;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp797;void*root=(void*)((_tmp797=_region_malloc(_tmp131->r,sizeof(*_tmp797)),((_tmp797[0]=((_tmp798.tag=1,((_tmp798.f1=_tmp1B3,((_tmp798.f2=(void*)_check_null(e->topt),_tmp798)))))),_tmp797))));
struct Cyc_CfFlowInfo_Place*_tmp799;struct Cyc_CfFlowInfo_Place*place=(_tmp799=_region_malloc(_tmp131->r,sizeof(*_tmp799)),((_tmp799->root=root,((_tmp799->fields=0,_tmp799)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp79C;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp79B;void*rval=(void*)((_tmp79B=_region_malloc(_tmp131->r,sizeof(*_tmp79B)),((_tmp79B[0]=((_tmp79C.tag=5,((_tmp79C.f1=place,_tmp79C)))),_tmp79B))));
void*place_val;
# 1465
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp131,_tmp1B4,0,_tmp131->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo _tmp35C;struct _tuple13 _tmp35B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1B3);_tmp35C=_tmp35B.f1;
outflow=_tmp35C;{
union Cyc_CfFlowInfo_FlowInfo _tmp35D=outflow;struct Cyc_Dict_Dict _tmp35E;struct Cyc_List_List*_tmp35F;_LL219: if((_tmp35D.BottomFL).tag != 1)goto _LL21B;_LL21A: {
struct _tuple13 _tmp79D;return(_tmp79D.f1=outflow,((_tmp79D.f2=rval,_tmp79D)));}_LL21B: if((_tmp35D.ReachableFL).tag != 2)goto _LL218;_tmp35E=((struct _tuple12)(_tmp35D.ReachableFL).val).f1;_tmp35F=((struct _tuple12)(_tmp35D.ReachableFL).val).f2;_LL21C: {
# 1473
struct _tuple13 _tmp79E;return(_tmp79E.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp35E,root,place_val),_tmp35F),((_tmp79E.f2=rval,_tmp79E)));}_LL218:;};};};}_LL15B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1B6=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1B6->tag != 36)goto _LL15D;else{_tmp1B7=_tmp1B6->f1;}}_LL15C:
# 1476
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp1B7);_LL15D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1B8->tag != 1)goto _LL15F;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1B9=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp1B8->f2);if(_tmp1B9->tag != 0)goto _LL15F;}}_LL15E:
 goto _LL160;_LL15F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1BA->tag != 35)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1BB->tag != 24)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1BC->tag != 38)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp146;if(_tmp1BD->tag != 39)goto _LLFE;}_LL166: {
# 1482
struct Cyc_Core_Impossible_exn_struct _tmp7A4;const char*_tmp7A3;struct Cyc_Core_Impossible_exn_struct*_tmp7A2;(int)_throw((void*)((_tmp7A2=_cycalloc(sizeof(*_tmp7A2)),((_tmp7A2[0]=((_tmp7A4.tag=Cyc_Core_Impossible,((_tmp7A4.f1=((_tmp7A3="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp7A3,sizeof(char),31))),_tmp7A4)))),_tmp7A2)))));}_LLFE:;};}
# 1491
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1498
struct Cyc_CfFlowInfo_FlowEnv*_tmp36A=env->fenv;
void*_tmp36B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp36D;union Cyc_Absyn_Constraint*_tmp36E;union Cyc_Absyn_Constraint*_tmp36F;_LL21E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36B;if(_tmp36C->tag != 5)goto _LL220;else{_tmp36D=(_tmp36C->f1).elt_typ;_tmp36E=((_tmp36C->f1).ptr_atts).bounds;_tmp36F=((_tmp36C->f1).ptr_atts).zero_term;}}_LL21F: {
# 1501
union Cyc_CfFlowInfo_FlowInfo _tmp370=f;struct Cyc_Dict_Dict _tmp371;struct Cyc_List_List*_tmp372;_LL223: if((_tmp370.BottomFL).tag != 1)goto _LL225;_LL224: {
struct _tuple14 _tmp7A5;return(_tmp7A5.f1=f,((_tmp7A5.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7A5)));}_LL225: if((_tmp370.ReachableFL).tag != 2)goto _LL222;_tmp371=((struct _tuple12)(_tmp370.ReachableFL).val).f1;_tmp372=((struct _tuple12)(_tmp370.ReachableFL).val).f2;_LL226: {
# 1505
struct Cyc_Absyn_Vardecl*locname=0;
if(Cyc_Tcutil_is_bound_one(_tmp36E)){
retry: {void*_tmp374=r;struct Cyc_Absyn_Vardecl*_tmp376;void*_tmp377;void*_tmp37B;struct Cyc_List_List*_tmp37C;enum Cyc_CfFlowInfo_InitLevel _tmp37F;_LL228: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp375=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp375->tag != 8)goto _LL22A;else{_tmp376=_tmp375->f1;_tmp377=(void*)_tmp375->f2;}}_LL229:
 r=_tmp377;locname=_tmp376;goto retry;_LL22A: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp378=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp378->tag != 1)goto _LL22C;}_LL22B:
 goto _LL22D;_LL22C: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp379=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp379->tag != 2)goto _LL22E;}_LL22D:
# 1511
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7A8;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7A7;e->annot=(void*)((_tmp7A7=_cycalloc(sizeof(*_tmp7A7)),((_tmp7A7[0]=((_tmp7A8.tag=Cyc_CfFlowInfo_NotZero,((_tmp7A8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp372),_tmp7A8)))),_tmp7A7))));}goto _LL227;_LL22E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp37A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp37A->tag != 5)goto _LL230;else{_tmp37B=(_tmp37A->f1)->root;_tmp37C=(_tmp37A->f1)->fields;}}_LL22F:
# 1513
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7AB;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7AA;e->annot=(void*)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=((_tmp7AB.tag=Cyc_CfFlowInfo_NotZero,((_tmp7AB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp372),_tmp7AB)))),_tmp7AA))));}{
struct Cyc_CfFlowInfo_Place*_tmp7AE;struct _tuple14 _tmp7AD;return(_tmp7AD.f1=f,((_tmp7AD.f2=Cyc_CfFlowInfo_PlaceL(((_tmp7AE=_region_malloc(_tmp36A->r,sizeof(*_tmp7AE)),((_tmp7AE->root=_tmp37B,((_tmp7AE->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp36A->r,_tmp37C,flds),_tmp7AE))))))),_tmp7AD)));};_LL230: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp37D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp37D->tag != 0)goto _LL232;}_LL231:
# 1516
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp7AF;return(_tmp7AF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7AF.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7AF)));};_LL232: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp37E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp374;if(_tmp37E->tag != 3)goto _LL234;else{_tmp37F=_tmp37E->f1;}}_LL233:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp37F,locname);goto _LL235;_LL234:;_LL235: {
# 1520
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7B2;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7B1;e->annot=(void*)((_tmp7B1=_cycalloc(sizeof(*_tmp7B1)),((_tmp7B1[0]=((_tmp7B2.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7B2.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp372),_tmp7B2)))),_tmp7B1))));}_LL227:;}}else{
# 1523
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7B5;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7B4;e->annot=(void*)((_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B5.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7B5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp372),_tmp7B5)))),_tmp7B4))));}
if(Cyc_CfFlowInfo_initlevel(_tmp36A,_tmp371,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7B8;void*_tmp7B7;(_tmp7B7=0,Cyc_Tcutil_terr(e->loc,((_tmp7B8="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7B8,sizeof(char),46))),_tag_dyneither(_tmp7B7,sizeof(void*),0)));}{
struct _tuple14 _tmp7B9;return(_tmp7B9.f1=f,((_tmp7B9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7B9)));};}_LL222:;}_LL220:;_LL221: {
# 1528
struct Cyc_Core_Impossible_exn_struct _tmp7BF;const char*_tmp7BE;struct Cyc_Core_Impossible_exn_struct*_tmp7BD;(int)_throw((void*)((_tmp7BD=_cycalloc(sizeof(*_tmp7BD)),((_tmp7BD[0]=((_tmp7BF.tag=Cyc_Core_Impossible,((_tmp7BF.f1=((_tmp7BE="left deref of non-pointer-type",_tag_dyneither(_tmp7BE,sizeof(char),31))),_tmp7BF)))),_tmp7BD)))));}_LL21D:;}
# 1536
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1539
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp391=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp392=inflow;struct Cyc_Dict_Dict _tmp393;struct Cyc_List_List*_tmp394;_LL237: if((_tmp392.BottomFL).tag != 1)goto _LL239;_LL238: {
struct _tuple14 _tmp7C0;return(_tmp7C0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C0.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7C0)));}_LL239: if((_tmp392.ReachableFL).tag != 2)goto _LL236;_tmp393=((struct _tuple12)(_tmp392.ReachableFL).val).f1;_tmp394=((struct _tuple12)(_tmp392.ReachableFL).val).f2;_LL23A:
# 1544
 d=_tmp393;_LL236:;}{
# 1548
void*_tmp396=e->r;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39F;struct Cyc_Absyn_Vardecl*_tmp3A2;struct Cyc_Absyn_Vardecl*_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct _dyneither_ptr*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B2;struct _dyneither_ptr*_tmp3B3;_LL23C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp397=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp397->tag != 13)goto _LL23E;else{_tmp398=_tmp397->f2;}}_LL23D:
 _tmp39A=_tmp398;goto _LL23F;_LL23E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp399=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp399->tag != 11)goto _LL240;else{_tmp39A=_tmp399->f1;}}_LL23F:
 _tmp39C=_tmp39A;goto _LL241;_LL240: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp39B=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp39B->tag != 12)goto _LL242;else{_tmp39C=_tmp39B->f1;}}_LL241:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp39C,flds);_LL242: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp39D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp39D->tag != 1)goto _LL244;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp39E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp39D->f2);if(_tmp39E->tag != 3)goto _LL244;else{_tmp39F=_tmp39E->f1;}}}_LL243:
# 1553
 _tmp3A2=_tmp39F;goto _LL245;_LL244: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3A0->tag != 1)goto _LL246;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3A1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp3A0->f2);if(_tmp3A1->tag != 4)goto _LL246;else{_tmp3A2=_tmp3A1->f1;}}}_LL245:
 _tmp3A5=_tmp3A2;goto _LL247;_LL246: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3A3->tag != 1)goto _LL248;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3A4=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp3A3->f2);if(_tmp3A4->tag != 5)goto _LL248;else{_tmp3A5=_tmp3A4->f1;}}}_LL247: {
# 1556
struct Cyc_CfFlowInfo_Place*_tmp7CA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7C9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7C8;struct _tuple14 _tmp7C7;return(_tmp7C7.f1=inflow,((_tmp7C7.f2=Cyc_CfFlowInfo_PlaceL(((_tmp7CA=_region_malloc(env->r,sizeof(*_tmp7CA)),((_tmp7CA->root=(void*)((_tmp7C8=_region_malloc(env->r,sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7C9.tag=0,((_tmp7C9.f1=_tmp3A5,_tmp7C9)))),_tmp7C8)))),((_tmp7CA->fields=flds,_tmp7CA))))))),_tmp7C7)));}_LL248: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3A6->tag != 1)goto _LL24A;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3A7=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp3A6->f2);if(_tmp3A7->tag != 1)goto _LL24A;else{_tmp3A8=_tmp3A7->f1;}}}_LL249: {
# 1558
struct _tuple14 _tmp7CB;return(_tmp7CB.f1=inflow,((_tmp7CB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7CB)));}_LL24A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3A9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3A9->tag != 21)goto _LL24C;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;}}_LL24B:
# 1561
{void*_tmp3B9=Cyc_Tcutil_compress((void*)_check_null(_tmp3AA->topt));void*_tmp3BB;_LL255: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9;if(_tmp3BA->tag != 5)goto _LL257;else{_tmp3BB=(_tmp3BA->f1).elt_typ;}}_LL256:
# 1563
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp3BB)){
struct Cyc_List_List*_tmp7CC;flds=((_tmp7CC=_region_malloc(env->r,sizeof(*_tmp7CC)),((_tmp7CC->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3BB,_tmp3AB),((_tmp7CC->tl=flds,_tmp7CC))))));}
goto _LL254;_LL257:;_LL258: {
struct Cyc_Core_Impossible_exn_struct _tmp7D2;const char*_tmp7D1;struct Cyc_Core_Impossible_exn_struct*_tmp7D0;(int)_throw((void*)((_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0[0]=((_tmp7D2.tag=Cyc_Core_Impossible,((_tmp7D2.f1=((_tmp7D1="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7D1,sizeof(char),25))),_tmp7D2)))),_tmp7D0)))));}_LL254:;}
# 1568
_tmp3AD=_tmp3AA;goto _LL24D;_LL24C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3AC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3AC->tag != 19)goto _LL24E;else{_tmp3AD=_tmp3AC->f1;}}_LL24D:
# 1570
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3AD->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp3C1;union Cyc_CfFlowInfo_AbsLVal _tmp3C2;struct _tuple14 _tmp3C0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp3AD);_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;{
struct _tuple14 _tmp7D3;struct _tuple14 _tmp3C4=(_tmp7D3.f1=_tmp3C1,((_tmp7D3.f2=_tmp3C2,_tmp7D3)));struct Cyc_Dict_Dict _tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_CfFlowInfo_Place*_tmp3C7;_LL25A: if(((_tmp3C4.f1).ReachableFL).tag != 2)goto _LL25C;_tmp3C5=((struct _tuple12)((_tmp3C4.f1).ReachableFL).val).f1;_tmp3C6=((struct _tuple12)((_tmp3C4.f1).ReachableFL).val).f2;if(((_tmp3C4.f2).PlaceL).tag != 1)goto _LL25C;_tmp3C7=(struct Cyc_CfFlowInfo_Place*)((_tmp3C4.f2).PlaceL).val;_LL25B: {
# 1574
void*_tmp3C8=Cyc_CfFlowInfo_lookup_place(_tmp3C5,_tmp3C7);
struct Cyc_Absyn_Vardecl*locname=0;
void*_tmp3C9=_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3CC;void*_tmp3CD;_LL25F: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3CA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3C9;if(_tmp3CA->tag != 5)goto _LL261;}_LL260:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3C1,_tmp3AD,_tmp3C1,_tmp3C8,flds);_LL261: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3CB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3C9;if(_tmp3CB->tag != 8)goto _LL263;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=(void*)_tmp3CB->f2;}}_LL262:
 _tmp3C8=_tmp3CD;locname=_tmp3CC;goto _LL264;_LL263:;_LL264: {
# 1585
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp391,_tmp3C5,_tmp3C8);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp391->unknown_all: _tmp391->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp391,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3AD->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7D6;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7D5;void*new_root=(void*)((_tmp7D5=_region_malloc(_tmp391->r,sizeof(*_tmp7D5)),((_tmp7D5[0]=((_tmp7D6.tag=1,((_tmp7D6.f1=e,((_tmp7D6.f2=(void*)_check_null(e->topt),_tmp7D6)))))),_tmp7D5))));
struct Cyc_CfFlowInfo_Place*_tmp7D7;struct Cyc_CfFlowInfo_Place*place=(_tmp7D7=_region_malloc(_tmp391->r,sizeof(*_tmp7D7)),((_tmp7D7->root=new_root,((_tmp7D7->fields=0,_tmp7D7)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7DA;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7D9;void*res=(void*)((_tmp7D9=_region_malloc(_tmp391->r,sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DA.tag=5,((_tmp7DA.f1=place,_tmp7DA)))),_tmp7D9))));
if(locname != 0){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7DD;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7DC;res=(void*)((_tmp7DC=_region_malloc(_tmp391->r,sizeof(*_tmp7DC)),((_tmp7DC[0]=((_tmp7DD.tag=8,((_tmp7DD.f1=locname,((_tmp7DD.f2=res,_tmp7DD)))))),_tmp7DC))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3C5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3C5,new_root,new_rval);
_tmp3C5=Cyc_CfFlowInfo_assign_place(_tmp391,e->loc,_tmp3C5,env->all_changed,_tmp3C7,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3D0=Cyc_CfFlowInfo_ReachableFL(_tmp3C5,_tmp3C6);
# 1600
return Cyc_NewControlFlow_anal_derefL(env,_tmp3D0,_tmp3AD,_tmp3D0,res,flds);};}_LL25E:;}_LL25C:;_LL25D:
# 1602
 goto _LL259;_LL259:;};}{
# 1605
union Cyc_CfFlowInfo_FlowInfo _tmp3D7;void*_tmp3D8;struct _tuple13 _tmp3D6=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3AD);_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3AD,_tmp3D7,_tmp3D8,flds);};_LL24E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3AE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3AE->tag != 22)goto _LL250;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;}}_LL24F: {
# 1609
void*_tmp3D9=Cyc_Tcutil_compress((void*)_check_null(_tmp3AF->topt));union Cyc_Absyn_Constraint*_tmp3DC;_LL266: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3DA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D9;if(_tmp3DA->tag != 10)goto _LL268;}_LL267: {
# 1611
unsigned int _tmp3DD=(Cyc_Evexp_eval_const_uint_exp(_tmp3B0)).f1;
struct Cyc_List_List*_tmp7DE;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3AF,((_tmp7DE=_region_malloc(env->r,sizeof(*_tmp7DE)),((_tmp7DE->hd=(void*)((int)_tmp3DD),((_tmp7DE->tl=flds,_tmp7DE)))))));}_LL268: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9;if(_tmp3DB->tag != 5)goto _LL26A;else{_tmp3DC=((_tmp3DB->f1).ptr_atts).bounds;}}_LL269: {
# 1614
struct _RegionHandle _tmp3DF=_new_region("temp");struct _RegionHandle*temp=& _tmp3DF;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp3E2;struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_Exp*_tmp7DF[2];struct _tuple19 _tmp3E1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7DF[1]=_tmp3B0,((_tmp7DF[0]=_tmp3AF,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7DF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E4=_tmp3E2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3E5=_tmp3E2;struct Cyc_Dict_Dict _tmp3E6;struct Cyc_List_List*_tmp3E7;_LL26D: if((_tmp3E5.ReachableFL).tag != 2)goto _LL26F;_tmp3E6=((struct _tuple12)(_tmp3E5.ReachableFL).val).f1;_tmp3E7=((struct _tuple12)(_tmp3E5.ReachableFL).val).f2;_LL26E:
# 1620
 if(Cyc_CfFlowInfo_initlevel(_tmp391,_tmp3E6,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3E3))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7E2;void*_tmp7E1;(_tmp7E1=0,Cyc_Tcutil_terr(_tmp3B0->loc,((_tmp7E2="expression may not be initialized",_tag_dyneither(_tmp7E2,sizeof(char),34))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp3EA=Cyc_NewControlFlow_add_subscript_reln(_tmp391->r,_tmp3E7,_tmp3AF,_tmp3B0);
if(_tmp3E7 != _tmp3EA)
_tmp3E4=Cyc_CfFlowInfo_ReachableFL(_tmp3E6,_tmp3EA);
goto _LL26C;};_LL26F:;_LL270:
 goto _LL26C;_LL26C:;}{
# 1628
union Cyc_CfFlowInfo_FlowInfo _tmp3EC;union Cyc_CfFlowInfo_AbsLVal _tmp3ED;struct _tuple14 _tmp3EB=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3AF,_tmp3E2,(void*)((struct Cyc_List_List*)_check_null(_tmp3E3))->hd,flds);_tmp3EC=_tmp3EB.f1;_tmp3ED=_tmp3EB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3EE=_tmp3EC;_LL272: if((_tmp3EE.BottomFL).tag != 1)goto _LL274;_LL273: {
struct _tuple14 _tmp7E3;struct _tuple14 _tmp3F0=(_tmp7E3.f1=_tmp3EC,((_tmp7E3.f2=_tmp3ED,_tmp7E3)));_npop_handler(0);return _tmp3F0;}_LL274:;_LL275: {
struct _tuple14 _tmp7E4;struct _tuple14 _tmp3F2=(_tmp7E4.f1=_tmp3E4,((_tmp7E4.f2=_tmp3ED,_tmp7E4)));_npop_handler(0);return _tmp3F2;}_LL271:;};};};}
# 1615
;_pop_region(temp);}_LL26A:;_LL26B: {
# 1634
struct Cyc_Core_Impossible_exn_struct _tmp7EA;const char*_tmp7E9;struct Cyc_Core_Impossible_exn_struct*_tmp7E8;(int)_throw((void*)((_tmp7E8=_cycalloc(sizeof(*_tmp7E8)),((_tmp7E8[0]=((_tmp7EA.tag=Cyc_Core_Impossible,((_tmp7EA.f1=((_tmp7E9="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp7E9,sizeof(char),33))),_tmp7EA)))),_tmp7E8)))));}_LL265:;}_LL250: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3B1=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp396;if(_tmp3B1->tag != 20)goto _LL252;else{_tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;}}_LL251:
# 1638
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp3B2->topt))){
struct _tuple14 _tmp7EB;return(_tmp7EB.f1=inflow,((_tmp7EB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7EB)));}{
struct Cyc_List_List*_tmp7EC;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp3B2,(
(_tmp7EC=_region_malloc(env->r,sizeof(*_tmp7EC)),((_tmp7EC->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp3B2->topt),_tmp3B3),((_tmp7EC->tl=flds,_tmp7EC)))))));};_LL252:;_LL253: {
# 1643
struct _tuple14 _tmp7ED;return(_tmp7ED.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7ED.f2=Cyc_CfFlowInfo_UnknownL(),_tmp7ED)));}_LL23B:;};}
# 1647
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1649
union Cyc_CfFlowInfo_FlowInfo _tmp3FA;union Cyc_CfFlowInfo_AbsLVal _tmp3FB;struct _tuple14 _tmp3F9=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;{
struct _tuple14 _tmp7EE;return(_tmp7EE.f1=_tmp3FA,((_tmp7EE.f2=_tmp3FB,_tmp7EE)));};}struct _tuple22{struct Cyc_Absyn_Exp*f1;enum Cyc_CfFlowInfo_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 1655
static struct _tuple22 Cyc_NewControlFlow_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 1657
switch(p){case Cyc_Absyn_Eq: _LL276: {
struct _tuple22 _tmp7EF;return(_tmp7EF.f1=e1,((_tmp7EF.f2=Cyc_CfFlowInfo_Req,((_tmp7EF.f3=e2,_tmp7EF)))));}case Cyc_Absyn_Neq: _LL277: {
struct _tuple22 _tmp7F0;return(_tmp7F0.f1=e1,((_tmp7F0.f2=Cyc_CfFlowInfo_Rneq,((_tmp7F0.f3=e2,_tmp7F0)))));}case Cyc_Absyn_Lt: _LL278: {
struct _tuple22 _tmp7F1;return(_tmp7F1.f1=e1,((_tmp7F1.f2=Cyc_CfFlowInfo_Rlt,((_tmp7F1.f3=e2,_tmp7F1)))));}case Cyc_Absyn_Lte: _LL279: {
struct _tuple22 _tmp7F2;return(_tmp7F2.f1=e1,((_tmp7F2.f2=Cyc_CfFlowInfo_Rlte,((_tmp7F2.f3=e2,_tmp7F2)))));}case Cyc_Absyn_Gt: _LL27A: {
struct _tuple22 _tmp7F3;return(_tmp7F3.f1=e2,((_tmp7F3.f2=Cyc_CfFlowInfo_Rlt,((_tmp7F3.f3=e1,_tmp7F3)))));}case Cyc_Absyn_Gte: _LL27B: {
struct _tuple22 _tmp7F4;return(_tmp7F4.f1=e2,((_tmp7F4.f2=Cyc_CfFlowInfo_Rlte,((_tmp7F4.f3=e1,_tmp7F4)))));}default: _LL27C: {
struct Cyc_Core_Impossible_exn_struct _tmp7FA;const char*_tmp7F9;struct Cyc_Core_Impossible_exn_struct*_tmp7F8;(int)_throw((void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7FA.tag=Cyc_Core_Impossible,((_tmp7FA.f1=((_tmp7F9="primop2relation",_tag_dyneither(_tmp7F9,sizeof(char),16))),_tmp7FA)))),_tmp7F8)))));}}}
# 1669
static enum Cyc_CfFlowInfo_Relation Cyc_NewControlFlow_flip_relation(enum Cyc_CfFlowInfo_Relation r){
switch(r){case Cyc_CfFlowInfo_Req: _LL27E:
 return Cyc_CfFlowInfo_Rneq;case Cyc_CfFlowInfo_Rneq: _LL27F:
 return Cyc_CfFlowInfo_Req;case Cyc_CfFlowInfo_Rlt: _LL280:
 return Cyc_CfFlowInfo_Rlte;case Cyc_CfFlowInfo_Rlte: _LL281:
 return Cyc_CfFlowInfo_Rlt;}}
# 1681
static struct _tuple15 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1684
struct Cyc_CfFlowInfo_FlowEnv*_tmp406=env->fenv;
# 1687
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp407=_new_region("temp");struct _RegionHandle*temp=& _tmp407;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp409;struct Cyc_List_List*_tmp40A;struct _tuple19 _tmp408=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp40A))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp40A->tl))->hd;
f=_tmp409;}{
# 1697
union Cyc_CfFlowInfo_FlowInfo _tmp40B=f;struct Cyc_Dict_Dict _tmp40C;struct Cyc_List_List*_tmp40D;_LL284: if((_tmp40B.BottomFL).tag != 1)goto _LL286;_LL285: {
struct _tuple15 _tmp7FB;struct _tuple15 _tmp40F=(_tmp7FB.f1=f,((_tmp7FB.f2=f,_tmp7FB)));_npop_handler(0);return _tmp40F;}_LL286: if((_tmp40B.ReachableFL).tag != 2)goto _LL283;_tmp40C=((struct _tuple12)(_tmp40B.ReachableFL).val).f1;_tmp40D=((struct _tuple12)(_tmp40B.ReachableFL).val).f2;_LL287: {
# 1700
struct Cyc_Absyn_Exp*_tmp410=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp411=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp40C,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7FE;void*_tmp7FD;(_tmp7FD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp7FE="expression may not be initialized",_tag_dyneither(_tmp7FE,sizeof(char),34))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp40C,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp801;void*_tmp800;(_tmp800=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp801="expression may not be initialized",_tag_dyneither(_tmp801,sizeof(char),34))),_tag_dyneither(_tmp800,sizeof(void*),0)));}
# 1709
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct Cyc_Absyn_Vardecl*r1n=0;struct Cyc_Absyn_Vardecl*r2n=0;
{void*_tmp416=r1;struct Cyc_Absyn_Vardecl*_tmp418;void*_tmp419;_LL289: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp417=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp416;if(_tmp417->tag != 8)goto _LL28B;else{_tmp418=_tmp417->f1;_tmp419=(void*)_tmp417->f2;}}_LL28A:
 r1=_tmp419;r1n=_tmp418;goto _LL288;_LL28B:;_LL28C:
 goto _LL288;_LL288:;}
# 1715
{void*_tmp41A=r2;struct Cyc_Absyn_Vardecl*_tmp41C;void*_tmp41D;_LL28E: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp41B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp41B->tag != 8)goto _LL290;else{_tmp41C=_tmp41B->f1;_tmp41D=(void*)_tmp41B->f2;}}_LL28F:
 r2=_tmp41D;r2n=_tmp41C;goto _LL28D;_LL290:;_LL291:
 goto _LL28D;_LL28D:;}{
# 1720
struct _tuple0 _tmp802;struct _tuple0 _tmp41F=(_tmp802.f1=r1,((_tmp802.f2=r2,_tmp802)));enum Cyc_CfFlowInfo_InitLevel _tmp421;enum Cyc_CfFlowInfo_InitLevel _tmp425;_LL293:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp420=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp420->tag != 3)goto _LL295;else{_tmp421=_tmp420->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp422=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp422->tag != 0)goto _LL295;};_LL294: {
# 1724
union Cyc_CfFlowInfo_FlowInfo _tmp435;union Cyc_CfFlowInfo_FlowInfo _tmp436;struct _tuple15 _tmp434=Cyc_NewControlFlow_splitzero(env,f,f,_tmp410,_tmp421,r1n);_tmp435=_tmp434.f1;_tmp436=_tmp434.f2;
switch(p){case Cyc_Absyn_Eq: _LL2A7: {
struct _tuple15 _tmp803;struct _tuple15 _tmp438=(_tmp803.f1=_tmp436,((_tmp803.f2=_tmp435,_tmp803)));_npop_handler(0);return _tmp438;}case Cyc_Absyn_Neq: _LL2A8: {
struct _tuple15 _tmp804;struct _tuple15 _tmp43A=(_tmp804.f1=_tmp435,((_tmp804.f2=_tmp436,_tmp804)));_npop_handler(0);return _tmp43A;}default: _LL2A9: {
struct Cyc_Core_Impossible_exn_struct _tmp80A;const char*_tmp809;struct Cyc_Core_Impossible_exn_struct*_tmp808;(int)_throw((void*)((_tmp808=_cycalloc(sizeof(*_tmp808)),((_tmp808[0]=((_tmp80A.tag=Cyc_Core_Impossible,((_tmp80A.f1=((_tmp809="anal_test, zero-split",_tag_dyneither(_tmp809,sizeof(char),22))),_tmp80A)))),_tmp808)))));}}}_LL295:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp423=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp423->tag != 0)goto _LL297;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp424=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp424->tag != 3)goto _LL297;else{_tmp425=_tmp424->f1;}};_LL296: {
# 1731
union Cyc_CfFlowInfo_FlowInfo _tmp43F;union Cyc_CfFlowInfo_FlowInfo _tmp440;struct _tuple15 _tmp43E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp411,_tmp425,r2n);_tmp43F=_tmp43E.f1;_tmp440=_tmp43E.f2;
switch(p){case Cyc_Absyn_Eq: _LL2AB: {
struct _tuple15 _tmp80B;struct _tuple15 _tmp442=(_tmp80B.f1=_tmp440,((_tmp80B.f2=_tmp43F,_tmp80B)));_npop_handler(0);return _tmp442;}case Cyc_Absyn_Neq: _LL2AC: {
struct _tuple15 _tmp80C;struct _tuple15 _tmp444=(_tmp80C.f1=_tmp43F,((_tmp80C.f2=_tmp440,_tmp80C)));_npop_handler(0);return _tmp444;}default: _LL2AD: {
struct Cyc_Core_Impossible_exn_struct _tmp812;const char*_tmp811;struct Cyc_Core_Impossible_exn_struct*_tmp810;(int)_throw((void*)((_tmp810=_cycalloc(sizeof(*_tmp810)),((_tmp810[0]=((_tmp812.tag=Cyc_Core_Impossible,((_tmp812.f1=((_tmp811="anal_test, zero-split",_tag_dyneither(_tmp811,sizeof(char),22))),_tmp812)))),_tmp810)))));}}}_LL297:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp426=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp426->tag != 0)goto _LL299;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp427=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp427->tag != 0)goto _LL299;};_LL298:
# 1738
 if(p == Cyc_Absyn_Eq){struct _tuple15 _tmp813;struct _tuple15 _tmp449=(_tmp813.f1=f,((_tmp813.f2=Cyc_CfFlowInfo_BottomFL(),_tmp813)));_npop_handler(0);return _tmp449;}else{
struct _tuple15 _tmp814;struct _tuple15 _tmp44B=(_tmp814.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp814.f2=f,_tmp814)));_npop_handler(0);return _tmp44B;}_LL299:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp428=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp428->tag != 0)goto _LL29B;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp429=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp429->tag != 1)goto _LL29B;};_LL29A:
 goto _LL29C;_LL29B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp42A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp42A->tag != 0)goto _LL29D;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp42B=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp42B->tag != 2)goto _LL29D;};_LL29C:
 goto _LL29E;_LL29D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp42C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp42C->tag != 0)goto _LL29F;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp42D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp42D->tag != 5)goto _LL29F;};_LL29E:
 goto _LL2A0;_LL29F:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp42E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp42E->tag != 1)goto _LL2A1;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp42F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp42F->tag != 0)goto _LL2A1;};_LL2A0:
 goto _LL2A2;_LL2A1:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp430=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp430->tag != 2)goto _LL2A3;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp431=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp431->tag != 0)goto _LL2A3;};_LL2A2:
 goto _LL2A4;_LL2A3:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp432=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp41F.f1;if(_tmp432->tag != 5)goto _LL2A5;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp433=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41F.f2;if(_tmp433->tag != 0)goto _LL2A5;};_LL2A4:
# 1746
 if(p == Cyc_Absyn_Neq){struct _tuple15 _tmp815;struct _tuple15 _tmp44D=(_tmp815.f1=f,((_tmp815.f2=Cyc_CfFlowInfo_BottomFL(),_tmp815)));_npop_handler(0);return _tmp44D;}else{
struct _tuple15 _tmp816;struct _tuple15 _tmp44F=(_tmp816.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp816.f2=f,_tmp816)));_npop_handler(0);return _tmp44F;}_LL2A5:;_LL2A6:
 goto _LL292;_LL292:;};}
# 1756
{struct _tuple0 _tmp817;struct _tuple0 _tmp451=(_tmp817.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp410->topt)),((_tmp817.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp411->topt)),_tmp817)));_LL2B0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp451.f1;if(_tmp452->tag != 6)goto _LL2B2;else{if(_tmp452->f1 != Cyc_Absyn_Unsigned)goto _LL2B2;}}_LL2B1:
 goto _LL2B3;_LL2B2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp451.f2;if(_tmp453->tag != 6)goto _LL2B4;else{if(_tmp453->f1 != Cyc_Absyn_Unsigned)goto _LL2B4;}}_LL2B3:
 goto _LL2B5;_LL2B4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp451.f1;if(_tmp454->tag != 19)goto _LL2B6;}_LL2B5:
 goto _LL2B7;_LL2B6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp451.f2;if(_tmp455->tag != 19)goto _LL2B8;}_LL2B7:
 goto _LL2AF;_LL2B8:;_LL2B9: {
struct _tuple15 _tmp818;struct _tuple15 _tmp457=(_tmp818.f1=f,((_tmp818.f2=f,_tmp818)));_npop_handler(0);return _tmp457;}_LL2AF:;}
# 1765
switch(p){case Cyc_Absyn_Eq: _LL2BA:
 goto _LL2BB;case Cyc_Absyn_Neq: _LL2BB: goto _LL2BC;case Cyc_Absyn_Gt: _LL2BC: goto _LL2BD;case Cyc_Absyn_Gte: _LL2BD: goto _LL2BE;case Cyc_Absyn_Lt: _LL2BE: goto _LL2BF;case Cyc_Absyn_Lte: _LL2BF: break;default: _LL2C0: {
struct _tuple15 _tmp819;struct _tuple15 _tmp459=(_tmp819.f1=f,((_tmp819.f2=f,_tmp819)));_npop_handler(0);return _tmp459;}}{
# 1770
struct _RegionHandle*_tmp45A=(env->fenv)->r;
struct Cyc_Absyn_Exp*_tmp45C;enum Cyc_CfFlowInfo_Relation _tmp45D;struct Cyc_Absyn_Exp*_tmp45E;struct _tuple22 _tmp45B=Cyc_NewControlFlow_primop2relation(_tmp410,p,_tmp411);_tmp45C=_tmp45B.f1;_tmp45D=_tmp45B.f2;_tmp45E=_tmp45B.f3;{
union Cyc_CfFlowInfo_RelnOp n1=Cyc_CfFlowInfo_RConst(0);
union Cyc_CfFlowInfo_RelnOp n2=Cyc_CfFlowInfo_RConst(0);
# 1775
if(Cyc_CfFlowInfo_exp2relnop(_tmp45C,& n1) && Cyc_CfFlowInfo_exp2relnop(_tmp45E,& n2)){
# 1777
struct Cyc_List_List*_tmp45F=Cyc_CfFlowInfo_add_relation(_tmp45A,n1,_tmp45D,n2,_tmp40D);
# 1781
struct Cyc_List_List*_tmp460=Cyc_CfFlowInfo_add_relation(_tmp45A,n2,Cyc_NewControlFlow_flip_relation(_tmp45D),n1,_tmp40D);
struct _tuple15 _tmp81A;struct _tuple15 _tmp462=(_tmp81A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp40C,_tmp45F),((_tmp81A.f2=Cyc_CfFlowInfo_ReachableFL(_tmp40C,_tmp460),_tmp81A)));_npop_handler(0);return _tmp462;}else{
# 1784
struct _tuple15 _tmp81B;struct _tuple15 _tmp464=(_tmp81B.f1=f,((_tmp81B.f2=f,_tmp81B)));_npop_handler(0);return _tmp464;}};};}_LL283:;};
# 1689
;_pop_region(temp);}
# 1790
static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1792
struct Cyc_CfFlowInfo_FlowEnv*_tmp465=env->fenv;
void*_tmp466=e->r;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_Absyn_Exp*_tmp46C;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp475;enum Cyc_Absyn_Primop _tmp477;struct Cyc_List_List*_tmp478;_LL2C3: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp467=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp467->tag != 5)goto _LL2C5;else{_tmp468=_tmp467->f1;_tmp469=_tmp467->f2;_tmp46A=_tmp467->f3;}}_LL2C4: {
# 1795
union Cyc_CfFlowInfo_FlowInfo _tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp47B;struct _tuple15 _tmp479=Cyc_NewControlFlow_anal_test(env,inflow,_tmp468);_tmp47A=_tmp479.f1;_tmp47B=_tmp479.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp47D;union Cyc_CfFlowInfo_FlowInfo _tmp47E;struct _tuple15 _tmp47C=Cyc_NewControlFlow_anal_test(env,_tmp47A,_tmp469);_tmp47D=_tmp47C.f1;_tmp47E=_tmp47C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp480;union Cyc_CfFlowInfo_FlowInfo _tmp481;struct _tuple15 _tmp47F=Cyc_NewControlFlow_anal_test(env,_tmp47B,_tmp46A);_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;{
struct _tuple15 _tmp81C;return(_tmp81C.f1=Cyc_CfFlowInfo_join_flow(_tmp465,env->all_changed,_tmp47D,_tmp480),((_tmp81C.f2=
Cyc_CfFlowInfo_join_flow(_tmp465,env->all_changed,_tmp47E,_tmp481),_tmp81C)));};};};}_LL2C5: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp46B=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp46B->tag != 6)goto _LL2C7;else{_tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;}}_LL2C6: {
# 1801
union Cyc_CfFlowInfo_FlowInfo _tmp484;union Cyc_CfFlowInfo_FlowInfo _tmp485;struct _tuple15 _tmp483=Cyc_NewControlFlow_anal_test(env,inflow,_tmp46C);_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp487;union Cyc_CfFlowInfo_FlowInfo _tmp488;struct _tuple15 _tmp486=Cyc_NewControlFlow_anal_test(env,_tmp484,_tmp46D);_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;{
struct _tuple15 _tmp81D;return(_tmp81D.f1=_tmp487,((_tmp81D.f2=Cyc_CfFlowInfo_join_flow(_tmp465,env->all_changed,_tmp485,_tmp488),_tmp81D)));};};}_LL2C7: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp46E=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp46E->tag != 7)goto _LL2C9;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46E->f2;}}_LL2C8: {
# 1805
union Cyc_CfFlowInfo_FlowInfo _tmp48B;union Cyc_CfFlowInfo_FlowInfo _tmp48C;struct _tuple15 _tmp48A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp46F);_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp48E;union Cyc_CfFlowInfo_FlowInfo _tmp48F;struct _tuple15 _tmp48D=Cyc_NewControlFlow_anal_test(env,_tmp48C,_tmp470);_tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f2;{
struct _tuple15 _tmp81E;return(_tmp81E.f1=Cyc_CfFlowInfo_join_flow(_tmp465,env->all_changed,_tmp48B,_tmp48E),((_tmp81E.f2=_tmp48F,_tmp81E)));};};}_LL2C9: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp471=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp471->tag != 8)goto _LL2CB;else{_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;}}_LL2CA: {
# 1809
union Cyc_CfFlowInfo_FlowInfo _tmp492;void*_tmp493;struct _tuple13 _tmp491=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp472);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp492,_tmp473);}_LL2CB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp474=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp474->tag != 2)goto _LL2CD;else{if(_tmp474->f1 != Cyc_Absyn_Not)goto _LL2CD;if(_tmp474->f2 == 0)goto _LL2CD;_tmp475=(struct Cyc_Absyn_Exp*)(_tmp474->f2)->hd;if((_tmp474->f2)->tl != 0)goto _LL2CD;}}_LL2CC: {
# 1812
union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple15 _tmp494=Cyc_NewControlFlow_anal_test(env,inflow,_tmp475);_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;{
struct _tuple15 _tmp81F;return(_tmp81F.f1=_tmp496,((_tmp81F.f2=_tmp495,_tmp81F)));};}_LL2CD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp476=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp466;if(_tmp476->tag != 2)goto _LL2CF;else{_tmp477=_tmp476->f1;_tmp478=_tmp476->f2;}}_LL2CE:
# 1815
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp477,_tmp478);_LL2CF:;_LL2D0: {
# 1819
union Cyc_CfFlowInfo_FlowInfo _tmp499;void*_tmp49A;struct _tuple13 _tmp498=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp49B=_tmp499;struct Cyc_Dict_Dict _tmp49C;_LL2D2: if((_tmp49B.BottomFL).tag != 1)goto _LL2D4;_LL2D3: {
struct _tuple15 _tmp820;return(_tmp820.f1=_tmp499,((_tmp820.f2=_tmp499,_tmp820)));}_LL2D4: if((_tmp49B.ReachableFL).tag != 2)goto _LL2D1;_tmp49C=((struct _tuple12)(_tmp49B.ReachableFL).val).f1;_LL2D5: {
# 1823
struct Cyc_Absyn_Vardecl*locname=0;
retry: {void*_tmp49E=_tmp49A;struct Cyc_Absyn_Vardecl*_tmp4A0;void*_tmp4A1;enum Cyc_CfFlowInfo_InitLevel _tmp4AA;_LL2D7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp49F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp49F->tag != 8)goto _LL2D9;else{_tmp4A0=_tmp49F->f1;_tmp4A1=(void*)_tmp49F->f2;}}_LL2D8:
 _tmp49A=_tmp4A1;locname=_tmp4A0;goto retry;_LL2D9: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4A2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A2->tag != 0)goto _LL2DB;}_LL2DA: {
struct _tuple15 _tmp821;return(_tmp821.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp821.f2=_tmp499,_tmp821)));}_LL2DB: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4A3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A3->tag != 2)goto _LL2DD;}_LL2DC:
 goto _LL2DE;_LL2DD: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4A4=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A4->tag != 1)goto _LL2DF;}_LL2DE:
 goto _LL2E0;_LL2DF: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4A5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A5->tag != 5)goto _LL2E1;}_LL2E0: {
struct _tuple15 _tmp822;return(_tmp822.f1=_tmp499,((_tmp822.f2=Cyc_CfFlowInfo_BottomFL(),_tmp822)));}_LL2E1: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4A6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A6->tag != 3)goto _LL2E3;else{if(_tmp4A6->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2E3;}}_LL2E2:
 goto _LL2E4;_LL2E3: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4A7=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A7->tag != 4)goto _LL2E5;else{if(_tmp4A7->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2E5;}}_LL2E4:
 goto _LL2E6;_LL2E5: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4A8=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A8->tag != 7)goto _LL2E7;}_LL2E6:
# 1833
{const char*_tmp825;void*_tmp824;(_tmp824=0,Cyc_Tcutil_terr(e->loc,((_tmp825="expression may not be initialized",_tag_dyneither(_tmp825,sizeof(char),34))),_tag_dyneither(_tmp824,sizeof(void*),0)));}{
struct _tuple15 _tmp826;return(_tmp826.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp826.f2=Cyc_CfFlowInfo_BottomFL(),_tmp826)));};_LL2E7: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4A9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4A9->tag != 3)goto _LL2E9;else{_tmp4AA=_tmp4A9->f1;}}_LL2E8:
# 1836
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp499,e,_tmp4AA,locname);_LL2E9: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4AB=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4AB->tag != 4)goto _LL2EB;}_LL2EA: {
struct _tuple15 _tmp827;return(_tmp827.f1=_tmp499,((_tmp827.f2=_tmp499,_tmp827)));}_LL2EB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4AC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp49E;if(_tmp4AC->tag != 6)goto _LL2D6;}_LL2EC: {
struct Cyc_Core_Impossible_exn_struct _tmp82D;const char*_tmp82C;struct Cyc_Core_Impossible_exn_struct*_tmp82B;(int)_throw((void*)((_tmp82B=_cycalloc(sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp82D.tag=Cyc_Core_Impossible,((_tmp82D.f1=((_tmp82C="anal_test",_tag_dyneither(_tmp82C,sizeof(char),10))),_tmp82D)))),_tmp82B)))));}_LL2D6:;}}_LL2D1:;};}_LL2C2:;}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1844
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1846
unsigned int _tmp4B7;struct Cyc_NewControlFlow_AnalEnv*_tmp4B8;struct Cyc_Dict_Dict _tmp4B9;struct _tuple23*_tmp4B6=ckenv;_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;_tmp4B9=_tmp4B6->f3;{
void*_tmp4BA=root;struct Cyc_Absyn_Vardecl*_tmp4BC;_LL2EE: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4BB=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4BA;if(_tmp4BB->tag != 0)goto _LL2F0;else{_tmp4BC=_tmp4BB->f1;}}_LL2EF:
# 1849
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4B8->fenv)->r,_tmp4BC->type)){
void*_tmp4BD=rval;_LL2F3: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4BE->tag != 8)goto _LL2F5;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4BF=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4BE->f2);if(_tmp4BF->tag != 7)goto _LL2F5;}}_LL2F4:
 goto _LL2F6;_LL2F5: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4C0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4C0->tag != 8)goto _LL2F7;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4C0->f2);if(_tmp4C1->tag != 0)goto _LL2F7;}}_LL2F6:
 goto _LL2F8;_LL2F7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4C2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4C2->tag != 8)goto _LL2F9;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C3=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp4C2->f2);if(_tmp4C3->tag != 3)goto _LL2F9;else{if(_tmp4C3->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F9;}}}_LL2F8:
 goto _LL2FA;_LL2F9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4C4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4C4->tag != 7)goto _LL2FB;}_LL2FA:
 goto _LL2FC;_LL2FB: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4C5->tag != 0)goto _LL2FD;}_LL2FC:
 goto _LL2FE;_LL2FD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C6=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BD;if(_tmp4C6->tag != 3)goto _LL2FF;else{if(_tmp4C6->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2FF;}}_LL2FE:
 goto _LL2F2;_LL2FF:;_LL300:
# 1858
{const char*_tmp831;void*_tmp830[1];struct Cyc_String_pa_PrintArg_struct _tmp82F;(_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4BC->name)),((_tmp830[0]=& _tmp82F,Cyc_Tcutil_warn(_tmp4B7,((_tmp831="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp831,sizeof(char),57))),_tag_dyneither(_tmp830,sizeof(void*),1)))))));}
goto _LL2F2;_LL2F2:;}
# 1861
goto _LL2ED;_LL2F0:;_LL2F1:
 goto _LL2ED;_LL2ED:;};}
# 1866
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4CA=flow;struct Cyc_Dict_Dict _tmp4CB;_LL302: if((_tmp4CA.BottomFL).tag != 1)goto _LL304;_LL303:
 return;_LL304: if((_tmp4CA.ReachableFL).tag != 2)goto _LL301;_tmp4CB=((struct _tuple12)(_tmp4CA.ReachableFL).val).f1;_LL305:
# 1870
{struct Cyc_List_List*_tmp4CC=env->param_roots;for(0;_tmp4CC != 0;_tmp4CC=_tmp4CC->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CB,Cyc_CfFlowInfo_lookup_place(_tmp4CB,(struct Cyc_CfFlowInfo_Place*)_tmp4CC->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp834;void*_tmp833;(_tmp833=0,Cyc_Tcutil_terr(loc,((_tmp834="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp834,sizeof(char),76))),_tag_dyneither(_tmp833,sizeof(void*),0)));}}}
# 1874
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple23 _tmp835;struct _tuple23 _tmp4CF=(_tmp835.f1=loc,((_tmp835.f2=env,((_tmp835.f3=_tmp4CB,_tmp835)))));
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4CF,_tmp4CB);}
# 1878
return;_LL301:;}struct _tuple24{struct Cyc_CfFlowInfo_Place*f1;void*f2;};struct _tuple25{void*f1;void*f2;void*f3;};
# 1882
static void Cyc_NewControlFlow_check_noconsume_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1884
union Cyc_CfFlowInfo_FlowInfo _tmp4D1=flow;struct Cyc_Dict_Dict _tmp4D2;_LL307: if((_tmp4D1.BottomFL).tag != 1)goto _LL309;_LL308:
 return;_LL309: if((_tmp4D1.ReachableFL).tag != 2)goto _LL306;_tmp4D2=((struct _tuple12)(_tmp4D1.ReachableFL).val).f1;_LL30A: {
# 1887
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Absyn_exp_unsigned_one;
int _tmp4D4=0;
int _tmp4D5=1;
{struct Cyc_List_List*_tmp4D6=env->noconsume_params;for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){
# 1893
struct Cyc_CfFlowInfo_Place*_tmp4D8;void*_tmp4D9;struct _tuple24*_tmp4D7=(struct _tuple24*)_tmp4D6->hd;_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;{
void*_tmp4DA=Cyc_CfFlowInfo_lookup_place(_tmp4D2,_tmp4D8);
void*_tmp4DC;struct Cyc_CfFlowInfo_Place*_tmp4DB=_tmp4D8;_tmp4DC=_tmp4DB->root;{
struct _tuple25 _tmp836;struct _tuple25 _tmp4DE=(_tmp836.f1=_tmp4DC,((_tmp836.f2=_tmp4D9,((_tmp836.f3=_tmp4DA,_tmp836)))));struct Cyc_Absyn_Vardecl*_tmp4E0;struct Cyc_Absyn_Vardecl*_tmp4E2;void*_tmp4E3;struct Cyc_Absyn_Vardecl*_tmp4E5;void*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;_LL30C:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4DF=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4DE.f1;if(_tmp4DF->tag != 0)goto _LL30E;else{_tmp4E0=_tmp4DF->f1;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DE.f2;if(_tmp4E1->tag != 8)goto _LL30E;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=(void*)_tmp4E1->f2;}};{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DE.f3;if(_tmp4E4->tag != 8)goto _LL30E;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=(void*)_tmp4E4->f2;}};_LL30D:
# 1898
 if(_tmp4E2 == _tmp4E5){
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4D3,_tmp4D5,_tmp4E6,& _tmp4D4)){
const char*_tmp83A;void*_tmp839[1];struct Cyc_String_pa_PrintArg_struct _tmp838;(_tmp838.tag=0,((_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1902
Cyc_Absynpp_qvar2string(_tmp4E0->name)),((_tmp839[0]=& _tmp838,Cyc_Tcutil_terr(loc,((_tmp83A="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp83A,sizeof(char),58))),_tag_dyneither(_tmp839,sizeof(void*),1)))))));}else{
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D2,_tmp4E6)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_PrintArg_struct _tmp83C;(_tmp83C.tag=0,((_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1906
Cyc_Absynpp_qvar2string(_tmp4E0->name)),((_tmp83D[0]=& _tmp83C,Cyc_Tcutil_terr(loc,((_tmp83E="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp83E,sizeof(char),84))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))));}}}else{
# 1910
_tmp4E8=_tmp4E0;goto _LL30F;}
goto _LL30B;_LL30E: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E7=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4DE.f1;if(_tmp4E7->tag != 0)goto _LL310;else{_tmp4E8=_tmp4E7->f1;}}_LL30F:
# 1913
{const char*_tmp842;void*_tmp841[1];struct Cyc_String_pa_PrintArg_struct _tmp840;(_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1915
Cyc_Absynpp_qvar2string(_tmp4E8->name)),((_tmp841[0]=& _tmp840,Cyc_Tcutil_terr(loc,((_tmp842="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp842,sizeof(char),85))),_tag_dyneither(_tmp841,sizeof(void*),1)))))));}
goto _LL30B;_LL310:;_LL311: {
# 1918
const char*_tmp845;void*_tmp844;(_tmp844=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp845="check_noconsume_params: root is not a varroot",_tag_dyneither(_tmp845,sizeof(char),46))),_tag_dyneither(_tmp844,sizeof(void*),0)));}_LL30B:;};};}}
# 1921
return;}_LL306:;}
# 1928
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1930
struct Cyc_CfFlowInfo_FlowEnv*_tmp4F4=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Core_Opt*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Stmt*_tmp4F8;unsigned int _tmp4F9;struct Cyc_Absyn_Switch_clause*_tmp4F5=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp4F6=_tmp4F5->pat_vars;_tmp4F7=_tmp4F5->where_clause;_tmp4F8=_tmp4F5->body;_tmp4F9=_tmp4F5->loc;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4F4,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4F6))->v,_tmp4F4->unknown_all,_tmp4F9,0);
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
if(_tmp4F7 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp4F7;
union Cyc_CfFlowInfo_FlowInfo _tmp4FB;union Cyc_CfFlowInfo_FlowInfo _tmp4FC;struct _tuple15 _tmp4FA=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;
inflow=_tmp4FC;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4FB,_tmp4F8);}else{
# 1941
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp4F8);}{
# 1943
union Cyc_CfFlowInfo_FlowInfo _tmp4FD=clause_outflow;_LL313: if((_tmp4FD.BottomFL).tag != 1)goto _LL315;_LL314:
 goto _LL312;_LL315:;_LL316:
# 1947
 if(scs->tl == 0)
return clause_outflow;else{
# 1952
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp848;void*_tmp847;(_tmp847=0,Cyc_Tcutil_terr(_tmp4F8->loc,((_tmp848="switch clause may implicitly fallthru",_tag_dyneither(_tmp848,sizeof(char),38))),_tag_dyneither(_tmp847,sizeof(void*),0)));}else{
# 1955
const char*_tmp84B;void*_tmp84A;(_tmp84A=0,Cyc_Tcutil_warn(_tmp4F8->loc,((_tmp84B="switch clause may implicitly fallthru",_tag_dyneither(_tmp84B,sizeof(char),38))),_tag_dyneither(_tmp84A,sizeof(void*),0)));}
# 1957
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 1959
goto _LL312;_LL312:;};};}
# 1962
return Cyc_CfFlowInfo_BottomFL();}struct _tuple26{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 1967
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple26*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 1969
struct Cyc_NewControlFlow_AnalEnv*_tmp503;struct Cyc_Dict_Dict _tmp504;unsigned int _tmp505;struct _tuple26*_tmp502=vdenv;_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;_tmp505=_tmp502->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp503->fenv)->r,vd->type)){
# 1972
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp84C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp84C.tag=0,((_tmp84C.f1=vd,_tmp84C)));
# 1974
void*_tmp506=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp504,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
void*_tmp507=_tmp506;_LL318: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp508=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp508->tag != 8)goto _LL31A;else{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp509=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)((void*)_tmp508->f2);if(_tmp509->tag != 7)goto _LL31A;}}_LL319:
 goto _LL31B;_LL31A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp50A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp50A->tag != 8)goto _LL31C;else{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp50B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)((void*)_tmp50A->f2);if(_tmp50B->tag != 0)goto _LL31C;}}_LL31B:
 goto _LL31D;_LL31C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp50C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp50C->tag != 8)goto _LL31E;else{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp50D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)((void*)_tmp50C->f2);if(_tmp50D->tag != 3)goto _LL31E;else{if(_tmp50D->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL31E;}}}_LL31D:
 goto _LL31F;_LL31E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp50E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp50E->tag != 7)goto _LL320;}_LL31F:
 goto _LL321;_LL320: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp50F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp50F->tag != 0)goto _LL322;}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp510=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp507;if(_tmp510->tag != 3)goto _LL324;else{if(_tmp510->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL324;}}_LL323:
 goto _LL317;_LL324:;_LL325:
# 1983
{const char*_tmp850;void*_tmp84F[1];struct Cyc_String_pa_PrintArg_struct _tmp84E;(_tmp84E.tag=0,((_tmp84E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp84F[0]=& _tmp84E,Cyc_Tcutil_warn(_tmp505,((_tmp850="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp850,sizeof(char),68))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))));}
# 1985
goto _LL317;_LL317:;}}
# 1990
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 1992
{union Cyc_CfFlowInfo_FlowInfo _tmp515=inflow;struct Cyc_Dict_Dict _tmp516;_LL327: if((_tmp515.ReachableFL).tag != 2)goto _LL329;_tmp516=((struct _tuple12)(_tmp515.ReachableFL).val).f1;_LL328: {
# 1994
struct _tuple26 _tmp851;struct _tuple26 _tmp517=(_tmp851.f1=env,((_tmp851.f2=_tmp516,((_tmp851.f3=decl->loc,_tmp851)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp518=env->fenv;
{void*_tmp519=decl->r;struct Cyc_Absyn_Vardecl*_tmp51B;struct Cyc_List_List*_tmp51D;struct Cyc_List_List*_tmp51F;_LL32C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp51A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp519;if(_tmp51A->tag != 0)goto _LL32E;else{_tmp51B=_tmp51A->f1;}}_LL32D:
# 1998
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp517,_tmp51B);
goto _LL32B;_LL32E: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp51C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp519;if(_tmp51C->tag != 2)goto _LL330;else{if(_tmp51C->f2 == 0)goto _LL330;_tmp51D=(struct Cyc_List_List*)(_tmp51C->f2)->v;}}_LL32F:
 _tmp51F=_tmp51D;goto _LL331;_LL330: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp51E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp519;if(_tmp51E->tag != 3)goto _LL332;else{_tmp51F=_tmp51E->f1;}}_LL331:
# 2002
((void(*)(void(*f)(struct _tuple26*,struct Cyc_Absyn_Vardecl*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp517,_tmp51F);
goto _LL32B;_LL332:;_LL333:
 goto _LL32B;_LL32B:;}
# 2006
goto _LL326;}_LL329:;_LL32A:
 goto _LL326;_LL326:;}
# 2009
return;}
# 2012
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo outflow;
# 2017
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp522;union Cyc_CfFlowInfo_FlowInfo*_tmp523;struct _tuple16 _tmp521=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp522=_tmp521.f1;_tmp523=_tmp521.f2;
inflow=*_tmp523;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp524=env->fenv;
void*_tmp525=s->r;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Stmt*_tmp52D;struct Cyc_Absyn_Stmt*_tmp52E;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Stmt*_tmp531;struct Cyc_Absyn_Stmt*_tmp532;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Stmt*_tmp535;struct Cyc_Absyn_Stmt*_tmp536;struct Cyc_Absyn_Stmt*_tmp538;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Stmt*_tmp53A;struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Stmt*_tmp53E;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Stmt*_tmp540;struct Cyc_Absyn_Stmt*_tmp541;struct Cyc_List_List*_tmp544;struct Cyc_Absyn_Switch_clause*_tmp545;struct Cyc_Absyn_Stmt*_tmp547;struct Cyc_Absyn_Stmt*_tmp549;struct Cyc_Absyn_Stmt*_tmp54B;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_Absyn_Stmt*_tmp550;struct Cyc_List_List*_tmp551;struct Cyc_Absyn_Tvar*_tmp554;struct Cyc_Absyn_Vardecl*_tmp555;struct Cyc_Absyn_Stmt*_tmp556;struct Cyc_Absyn_Decl*_tmp558;struct Cyc_Absyn_Stmt*_tmp559;struct Cyc_Absyn_Stmt*_tmp55B;struct Cyc_Absyn_Exp*_tmp55D;_LL335: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp526=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp526->tag != 0)goto _LL337;}_LL336:
 return inflow;_LL337: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp527=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp527->tag != 3)goto _LL339;else{if(_tmp527->f1 != 0)goto _LL339;}}_LL338:
# 2023
 if(env->noreturn){
const char*_tmp854;void*_tmp853;(_tmp853=0,Cyc_Tcutil_terr(s->loc,((_tmp854="`noreturn' function might return",_tag_dyneither(_tmp854,sizeof(char),33))),_tag_dyneither(_tmp853,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL339: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp528=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp528->tag != 3)goto _LL33B;else{_tmp529=_tmp528->f1;}}_LL33A:
# 2029
 if(env->noreturn){
const char*_tmp857;void*_tmp856;(_tmp856=0,Cyc_Tcutil_terr(s->loc,((_tmp857="`noreturn' function might return",_tag_dyneither(_tmp857,sizeof(char),33))),_tag_dyneither(_tmp856,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp563;void*_tmp564;struct _tuple13 _tmp562=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp529));_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;
_tmp563=Cyc_NewControlFlow_use_Rval(env,_tmp529->loc,_tmp563,_tmp564);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp563);
Cyc_NewControlFlow_check_noconsume_params(s->loc,env,_tmp563);
return Cyc_CfFlowInfo_BottomFL();};_LL33B: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp52A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp52A->tag != 1)goto _LL33D;else{_tmp52B=_tmp52A->f1;}}_LL33C:
# 2037
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp52B)).f1;_LL33D: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp52C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp52C->tag != 2)goto _LL33F;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}_LL33E:
# 2040
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52D),_tmp52E);_LL33F: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp52F=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp52F->tag != 4)goto _LL341;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;_tmp532=_tmp52F->f3;}}_LL340: {
# 2043
union Cyc_CfFlowInfo_FlowInfo _tmp566;union Cyc_CfFlowInfo_FlowInfo _tmp567;struct _tuple15 _tmp565=Cyc_NewControlFlow_anal_test(env,inflow,_tmp530);_tmp566=_tmp565.f1;_tmp567=_tmp565.f2;{
# 2050
union Cyc_CfFlowInfo_FlowInfo _tmp568=Cyc_NewControlFlow_anal_stmt(env,_tmp567,_tmp532);
union Cyc_CfFlowInfo_FlowInfo _tmp569=Cyc_NewControlFlow_anal_stmt(env,_tmp566,_tmp531);
return Cyc_CfFlowInfo_join_flow(_tmp524,env->all_changed,_tmp569,_tmp568);};}_LL341: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp533=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp533->tag != 5)goto _LL343;else{_tmp534=(_tmp533->f1).f1;_tmp535=(_tmp533->f1).f2;_tmp536=_tmp533->f2;}}_LL342: {
# 2058
union Cyc_CfFlowInfo_FlowInfo*_tmp56B;struct _tuple16 _tmp56A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp535);_tmp56B=_tmp56A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp56C=*_tmp56B;
union Cyc_CfFlowInfo_FlowInfo _tmp56E;union Cyc_CfFlowInfo_FlowInfo _tmp56F;struct _tuple15 _tmp56D=Cyc_NewControlFlow_anal_test(env,_tmp56C,_tmp534);_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp570=Cyc_NewControlFlow_anal_stmt(env,_tmp56E,_tmp536);
Cyc_NewControlFlow_update_flow(env,_tmp535,_tmp570);
return _tmp56F;};};}_LL343: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp537=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp537->tag != 14)goto _LL345;else{_tmp538=_tmp537->f1;_tmp539=(_tmp537->f2).f1;_tmp53A=(_tmp537->f2).f2;}}_LL344: {
# 2068
union Cyc_CfFlowInfo_FlowInfo _tmp571=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp538);
union Cyc_CfFlowInfo_FlowInfo*_tmp573;struct _tuple16 _tmp572=Cyc_NewControlFlow_pre_stmt_check(env,_tmp571,_tmp53A);_tmp573=_tmp572.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp574=*_tmp573;
union Cyc_CfFlowInfo_FlowInfo _tmp576;union Cyc_CfFlowInfo_FlowInfo _tmp577;struct _tuple15 _tmp575=Cyc_NewControlFlow_anal_test(env,_tmp574,_tmp539);_tmp576=_tmp575.f1;_tmp577=_tmp575.f2;
Cyc_NewControlFlow_update_flow(env,_tmp538,_tmp576);
return _tmp577;};}_LL345: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp53B=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp53B->tag != 9)goto _LL347;else{_tmp53C=_tmp53B->f1;_tmp53D=(_tmp53B->f2).f1;_tmp53E=(_tmp53B->f2).f2;_tmp53F=(_tmp53B->f3).f1;_tmp540=(_tmp53B->f3).f2;_tmp541=_tmp53B->f4;}}_LL346: {
# 2077
union Cyc_CfFlowInfo_FlowInfo _tmp578=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp53C)).f1;
union Cyc_CfFlowInfo_FlowInfo*_tmp57A;struct _tuple16 _tmp579=Cyc_NewControlFlow_pre_stmt_check(env,_tmp578,_tmp53E);_tmp57A=_tmp579.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp57B=*_tmp57A;
union Cyc_CfFlowInfo_FlowInfo _tmp57D;union Cyc_CfFlowInfo_FlowInfo _tmp57E;struct _tuple15 _tmp57C=Cyc_NewControlFlow_anal_test(env,_tmp57B,_tmp53D);_tmp57D=_tmp57C.f1;_tmp57E=_tmp57C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp57F=Cyc_NewControlFlow_anal_stmt(env,_tmp57D,_tmp541);
union Cyc_CfFlowInfo_FlowInfo*_tmp581;struct _tuple16 _tmp580=Cyc_NewControlFlow_pre_stmt_check(env,_tmp57F,_tmp540);_tmp581=_tmp580.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp582=*_tmp581;
union Cyc_CfFlowInfo_FlowInfo _tmp583=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp582,_tmp53F)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp53E,_tmp583);
return _tmp57E;};};};}_LL347: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp542=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp542->tag != 6)goto _LL349;else{if(_tmp542->f1 != 0)goto _LL349;}}_LL348:
# 2089
 return Cyc_CfFlowInfo_BottomFL();_LL349: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp543=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp543->tag != 11)goto _LL34B;else{_tmp544=_tmp543->f1;if(_tmp543->f2 == 0)goto _LL34B;_tmp545=*_tmp543->f2;}}_LL34A: {
# 2091
struct _RegionHandle _tmp584=_new_region("temp");struct _RegionHandle*temp=& _tmp584;_push_region(temp);
{union Cyc_CfFlowInfo_FlowInfo _tmp586;struct Cyc_List_List*_tmp587;struct _tuple19 _tmp585=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp544,1,1);_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;
# 2094
for(0;_tmp587 != 0;(_tmp587=_tmp587->tl,_tmp544=_tmp544->tl)){
_tmp586=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp544))->hd)->loc,_tmp586,(void*)_tmp587->hd);}
# 2097
_tmp586=Cyc_NewControlFlow_add_vars(_tmp524,_tmp586,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp545->pat_vars))->v,_tmp524->unknown_all,s->loc,0);
Cyc_NewControlFlow_update_flow(env,_tmp545->body,_tmp586);{
union Cyc_CfFlowInfo_FlowInfo _tmp588=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp588;};}
# 2092
;_pop_region(temp);}_LL34B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp546=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp546->tag != 6)goto _LL34D;else{_tmp547=_tmp546->f1;}}_LL34C:
# 2101
 _tmp549=_tmp547;goto _LL34E;_LL34D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp548=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp548->tag != 7)goto _LL34F;else{_tmp549=_tmp548->f1;}}_LL34E:
 _tmp54B=_tmp549;goto _LL350;_LL34F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp54A=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp54A->tag != 8)goto _LL351;else{_tmp54B=_tmp54A->f2;}}_LL350:
# 2106
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp589=_tmp522->encloser;
struct Cyc_Absyn_Stmt*_tmp58A=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp54B)))->encloser;
while(_tmp58A != _tmp589){
struct Cyc_Absyn_Stmt*_tmp58B=(Cyc_NewControlFlow_get_stmt_annot(_tmp589))->encloser;
if(_tmp58B == _tmp589){
{const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_terr(s->loc,((_tmp85A="goto enters local scope or exception handler",_tag_dyneither(_tmp85A,sizeof(char),45))),_tag_dyneither(_tmp859,sizeof(void*),0)));}
break;}
# 2115
_tmp589=_tmp58B;}}
# 2120
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp54B),inflow);
return Cyc_CfFlowInfo_BottomFL();_LL351: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp54C=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp54C->tag != 10)goto _LL353;else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;}}_LL352: {
# 2124
union Cyc_CfFlowInfo_FlowInfo _tmp58F;void*_tmp590;struct _tuple13 _tmp58E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp54D);_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;
_tmp58F=Cyc_NewControlFlow_use_Rval(env,_tmp54D->loc,_tmp58F,_tmp590);
return Cyc_NewControlFlow_anal_scs(env,_tmp58F,_tmp54E);}_LL353: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp54F=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp54F->tag != 15)goto _LL355;else{_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;}}_LL354: {
# 2131
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp550);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2139
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2142
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp551);
{union Cyc_CfFlowInfo_FlowInfo _tmp591=scs_outflow;_LL360: if((_tmp591.BottomFL).tag != 1)goto _LL362;_LL361:
 goto _LL35F;_LL362:;_LL363: {
const char*_tmp85D;void*_tmp85C;(_tmp85C=0,Cyc_Tcutil_terr(s->loc,((_tmp85D="last catch clause may implicitly fallthru",_tag_dyneither(_tmp85D,sizeof(char),42))),_tag_dyneither(_tmp85C,sizeof(void*),0)));}_LL35F:;}
# 2148
outflow=s1_outflow;
# 2150
return outflow;};};}_LL355: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp552=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp552->tag != 12)goto _LL357;else{{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp553=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)(_tmp552->f1)->r;if(_tmp553->tag != 5)goto _LL357;else{_tmp554=_tmp553->f1;_tmp555=_tmp553->f2;}}_tmp556=_tmp552->f2;}}_LL356: {
# 2156
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp555->initializer);
union Cyc_CfFlowInfo_FlowInfo _tmp595;union Cyc_CfFlowInfo_AbsLVal _tmp596;struct _tuple14 _tmp594=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp595=_tmp594.f1;_tmp596=_tmp594.f2;{
# 2161
struct _tuple14 _tmp85E;struct _tuple14 _tmp598=(_tmp85E.f1=_tmp595,((_tmp85E.f2=_tmp596,_tmp85E)));struct Cyc_Dict_Dict _tmp599;struct Cyc_List_List*_tmp59A;struct Cyc_CfFlowInfo_Place*_tmp59B;_LL365: if(((_tmp598.f1).ReachableFL).tag != 2)goto _LL367;_tmp599=((struct _tuple12)((_tmp598.f1).ReachableFL).val).f1;_tmp59A=((struct _tuple12)((_tmp598.f1).ReachableFL).val).f2;if(((_tmp598.f2).PlaceL).tag != 1)goto _LL367;_tmp59B=(struct Cyc_CfFlowInfo_Place*)((_tmp598.f2).PlaceL).val;_LL366: {
# 2163
void*_tmp59C=Cyc_CfFlowInfo_lookup_place(_tmp599,_tmp59B);
# 2165
void*t=(void*)_check_null(e->topt);
# 2172
void*_tmp59D=Cyc_CfFlowInfo_make_unique_consumed(_tmp524,t,e,env->iteration_num,_tmp59C);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp861;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp860;_tmp59D=(void*)((_tmp860=_region_malloc(_tmp524->r,sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=8,((_tmp861.f1=_tmp555,((_tmp861.f2=_tmp59D,_tmp861)))))),_tmp860))));}
_tmp599=Cyc_CfFlowInfo_assign_place(_tmp524,e->loc,_tmp599,env->all_changed,_tmp59B,_tmp59D);
_tmp595=Cyc_CfFlowInfo_ReachableFL(_tmp599,_tmp59A);{
# 2179
void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp524,_tmp599,_tmp59C)){case Cyc_CfFlowInfo_AllIL: _LL36B:
 leaf=_tmp524->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL36C:
 leaf=_tmp524->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL36D:
 leaf=_tmp524->unknown_this;break;}{
# 2187
void*_tmp5A0=Cyc_CfFlowInfo_typ_to_absrval(_tmp524,t,0,leaf);
_tmp595=Cyc_NewControlFlow_use_Rval(env,e->loc,_tmp595,_tmp59C);{
struct Cyc_List_List _tmp862;struct Cyc_List_List _tmp5A1=(_tmp862.hd=_tmp555,((_tmp862.tl=0,_tmp862)));
_tmp595=Cyc_NewControlFlow_add_vars(_tmp524,_tmp595,& _tmp5A1,_tmp524->unknown_all,s->loc,0);
# 2194
_tmp595=Cyc_NewControlFlow_anal_stmt(env,_tmp595,_tmp556);{
# 2199
union Cyc_CfFlowInfo_FlowInfo _tmp5A2=_tmp595;struct Cyc_Dict_Dict _tmp5A3;struct Cyc_List_List*_tmp5A4;_LL370: if((_tmp5A2.ReachableFL).tag != 2)goto _LL372;_tmp5A3=((struct _tuple12)(_tmp5A2.ReachableFL).val).f1;_tmp5A4=((struct _tuple12)(_tmp5A2.ReachableFL).val).f2;_LL371:
# 2202
{void*_tmp5A5=Cyc_CfFlowInfo_lookup_place(_tmp5A3,_tmp59B);struct Cyc_Absyn_Vardecl*_tmp5A7;_LL375: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5A6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5A5;if(_tmp5A6->tag != 8)goto _LL377;else{_tmp5A7=_tmp5A6->f1;}}_LL376:
# 2204
 if(_tmp5A7 != _tmp555)goto _LL378;
goto _LL374;_LL377:;_LL378:
# 2207
{const char*_tmp865;void*_tmp864;(_tmp864=0,Cyc_Tcutil_terr(s->loc,((_tmp865="aliased location was overwritten",_tag_dyneither(_tmp865,sizeof(char),33))),_tag_dyneither(_tmp864,sizeof(void*),0)));}
goto _LL374;_LL374:;}
# 2210
_tmp5A3=Cyc_CfFlowInfo_assign_place(_tmp524,s->loc,_tmp5A3,env->all_changed,_tmp59B,_tmp5A0);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp5A3,_tmp5A4);
# 2215
return outflow;_LL372:;_LL373:
 return _tmp595;_LL36F:;};};};};}_LL367: if(((_tmp598.f1).BottomFL).tag != 1)goto _LL369;_LL368:
# 2218
 return _tmp595;_LL369:;_LL36A:
{const char*_tmp868;void*_tmp867;(_tmp867=0,Cyc_Tcutil_terr(e->loc,((_tmp868="bad alias expression--no unique path found",_tag_dyneither(_tmp868,sizeof(char),43))),_tag_dyneither(_tmp867,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL364:;};}_LL357: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp557=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp557->tag != 12)goto _LL359;else{_tmp558=_tmp557->f1;_tmp559=_tmp557->f2;}}_LL358:
# 2224
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp558),_tmp559);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp558);
return outflow;_LL359: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp55A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp55A->tag != 13)goto _LL35B;else{_tmp55B=_tmp55A->f2;}}_LL35A:
# 2229
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp55B);_LL35B: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp55C=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp525;if(_tmp55C->tag != 16)goto _LL35D;else{_tmp55D=_tmp55C->f1;}}_LL35C: {
# 2232
union Cyc_CfFlowInfo_FlowInfo _tmp5AE;void*_tmp5AF;struct _tuple13 _tmp5AD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp55D);_tmp5AE=_tmp5AD.f1;_tmp5AF=_tmp5AD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B0=Cyc_NewControlFlow_use_Rval(env,_tmp55D->loc,_tmp5AE,_tmp5AF);
void*_tmp5B1=Cyc_Tcutil_compress((void*)_check_null(_tmp55D->topt));void*_tmp5B3;_LL37A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5B1;if(_tmp5B2->tag != 15)goto _LL37C;else{_tmp5B3=(void*)_tmp5B2->f1;}}_LL37B:
# 2236
 return Cyc_CfFlowInfo_kill_flow_region(_tmp524,_tmp5AE,_tmp5B3);_LL37C:;_LL37D: {
struct Cyc_Core_Impossible_exn_struct _tmp86E;const char*_tmp86D;struct Cyc_Core_Impossible_exn_struct*_tmp86C;(int)_throw((void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86E.tag=Cyc_Core_Impossible,((_tmp86E.f1=((_tmp86D="anal_stmt -- reset_region",_tag_dyneither(_tmp86D,sizeof(char),26))),_tmp86E)))),_tmp86C)))));}_LL379:;};}_LL35D:;_LL35E: {
# 2240
struct Cyc_Core_Impossible_exn_struct _tmp874;const char*_tmp873;struct Cyc_Core_Impossible_exn_struct*_tmp872;(int)_throw((void*)((_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp874.tag=Cyc_Core_Impossible,((_tmp874.f1=((_tmp873="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp873,sizeof(char),56))),_tmp874)))),_tmp872)))));}_LL334:;};}
# 2245
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2247
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5BA=env->fenv;
void*_tmp5BB=decl->r;struct Cyc_Absyn_Vardecl*_tmp5BD;struct Cyc_List_List*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_List_List*_tmp5C2;struct Cyc_Absyn_Fndecl*_tmp5C4;struct Cyc_Absyn_Tvar*_tmp5C6;struct Cyc_Absyn_Vardecl*_tmp5C7;int _tmp5C8;struct Cyc_Absyn_Exp*_tmp5C9;_LL37F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BC=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5BB;if(_tmp5BC->tag != 0)goto _LL381;else{_tmp5BD=_tmp5BC->f1;}}_LL380: {
# 2257
struct Cyc_List_List _tmp875;struct Cyc_List_List _tmp5CA=(_tmp875.hd=_tmp5BD,((_tmp875.tl=0,_tmp875)));
inflow=Cyc_NewControlFlow_add_vars(_tmp5BA,inflow,& _tmp5CA,_tmp5BA->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp5CB=_tmp5BD->initializer;
if(_tmp5CB == 0)
return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CD;void*_tmp5CE;struct _tuple13 _tmp5CC=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5CB);_tmp5CD=_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CF=_tmp5CD;struct Cyc_Dict_Dict _tmp5D0;struct Cyc_List_List*_tmp5D1;_LL38C: if((_tmp5CF.BottomFL).tag != 1)goto _LL38E;_LL38D:
 return Cyc_CfFlowInfo_BottomFL();_LL38E: if((_tmp5CF.ReachableFL).tag != 2)goto _LL38B;_tmp5D0=((struct _tuple12)(_tmp5CF.ReachableFL).val).f1;_tmp5D1=((struct _tuple12)(_tmp5CF.ReachableFL).val).f2;_LL38F:
# 2266
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp87B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp87A;struct Cyc_CfFlowInfo_Place*_tmp879;_tmp5D0=Cyc_CfFlowInfo_assign_place(_tmp5BA,decl->loc,_tmp5D0,env->all_changed,(
(_tmp879=_region_malloc(env->r,sizeof(*_tmp879)),((_tmp879->root=(void*)((_tmp87B=_region_malloc(env->r,sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp87A.tag=0,((_tmp87A.f1=_tmp5BD,_tmp87A)))),_tmp87B)))),((_tmp879->fields=0,_tmp879)))))),_tmp5CE);}
# 2269
_tmp5D1=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp5D1,_tmp5BD,(struct Cyc_Absyn_Exp*)_check_null(_tmp5BD->initializer));{
union Cyc_CfFlowInfo_FlowInfo _tmp5D5=Cyc_CfFlowInfo_ReachableFL(_tmp5D0,_tmp5D1);
Cyc_NewControlFlow_update_tryflow(env,_tmp5D5);
# 2274
return _tmp5D5;};_LL38B:;};};};}_LL381: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5BE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5BB;if(_tmp5BE->tag != 2)goto _LL383;else{if(_tmp5BE->f2 == 0)goto _LL383;_tmp5BF=(struct Cyc_List_List*)(_tmp5BE->f2)->v;_tmp5C0=_tmp5BE->f3;}}_LL382: {
# 2279
union Cyc_CfFlowInfo_FlowInfo _tmp5D8;void*_tmp5D9;struct _tuple13 _tmp5D7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5C0);_tmp5D8=_tmp5D7.f1;_tmp5D9=_tmp5D7.f2;
_tmp5D8=Cyc_NewControlFlow_use_Rval(env,_tmp5C0->loc,_tmp5D8,_tmp5D9);
return Cyc_NewControlFlow_add_vars(_tmp5BA,_tmp5D8,_tmp5BF,_tmp5BA->unknown_all,decl->loc,0);}_LL383: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5C1=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5BB;if(_tmp5C1->tag != 3)goto _LL385;else{_tmp5C2=_tmp5C1->f1;}}_LL384:
# 2284
 return Cyc_NewControlFlow_add_vars(_tmp5BA,inflow,_tmp5C2,_tmp5BA->unknown_none,decl->loc,0);_LL385: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5C3=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5BB;if(_tmp5C3->tag != 1)goto _LL387;else{_tmp5C4=_tmp5C3->f1;}}_LL386:
# 2287
 Cyc_NewControlFlow_check_nested_fun(_tmp5BA,inflow,_tmp5C4);{
void*t=(void*)_check_null(_tmp5C4->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5DA=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp5C4->fn_vardecl);
# 2293
struct Cyc_List_List*_tmp87C;return Cyc_NewControlFlow_add_vars(_tmp5BA,inflow,((_tmp87C=_region_malloc(env->r,sizeof(*_tmp87C)),((_tmp87C->hd=_tmp5DA,((_tmp87C->tl=0,_tmp87C)))))),_tmp5BA->unknown_all,decl->loc,0);};_LL387: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp5C5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5BB;if(_tmp5C5->tag != 4)goto _LL389;else{_tmp5C6=_tmp5C5->f1;_tmp5C7=_tmp5C5->f2;_tmp5C8=_tmp5C5->f3;_tmp5C9=_tmp5C5->f4;}}_LL388:
# 2296
 if(_tmp5C9 != 0){
struct Cyc_Absyn_Exp*_tmp5DC=_tmp5C9;
union Cyc_CfFlowInfo_FlowInfo _tmp5DE;void*_tmp5DF;struct _tuple13 _tmp5DD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5DC);_tmp5DE=_tmp5DD.f1;_tmp5DF=_tmp5DD.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp5DC->loc,_tmp5DE,_tmp5DF);}{
# 2301
struct Cyc_List_List _tmp87D;struct Cyc_List_List _tmp5E0=(_tmp87D.hd=_tmp5C7,((_tmp87D.tl=0,_tmp87D)));
return Cyc_NewControlFlow_add_vars(_tmp5BA,inflow,& _tmp5E0,_tmp5BA->unknown_all,decl->loc,0);};_LL389:;_LL38A: {
# 2304
struct Cyc_Core_Impossible_exn_struct _tmp883;const char*_tmp882;struct Cyc_Core_Impossible_exn_struct*_tmp881;(int)_throw((void*)((_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881[0]=((_tmp883.tag=Cyc_Core_Impossible,((_tmp883.f1=((_tmp882="anal_decl: unexpected decl variant",_tag_dyneither(_tmp882,sizeof(char),35))),_tmp883)))),_tmp881)))));}_LL37E:;}
# 2344 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp5E5=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5E5;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2346
;_pop_region(frgn);}
# 2351
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2355
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2359
struct _RegionHandle*_tmp5E6=fenv->r;
unsigned int _tmp5E7=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp5E7,1);{
# 2365
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_rvals=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5E8=inflow;struct Cyc_Dict_Dict _tmp5E9;struct Cyc_List_List*_tmp5EA;_LL391: if((_tmp5E8.BottomFL).tag != 1)goto _LL393;_LL392: {
const char*_tmp886;void*_tmp885;(_tmp885=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp886="check_fun",_tag_dyneither(_tmp886,sizeof(char),10))),_tag_dyneither(_tmp885,sizeof(void*),0)));}_LL393: if((_tmp5E8.ReachableFL).tag != 2)goto _LL390;_tmp5E9=((struct _tuple12)(_tmp5E8.ReachableFL).val).f1;_tmp5EA=((struct _tuple12)(_tmp5E8.ReachableFL).val).f2;_LL394: {
# 2372
struct Cyc_List_List*atts;
{void*_tmp5ED=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_List_List*_tmp5EF;_LL396: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5ED;if(_tmp5EE->tag != 9)goto _LL398;else{_tmp5EF=(_tmp5EE->f1).attributes;}}_LL397:
 atts=_tmp5EF;goto _LL395;_LL398:;_LL399: {
const char*_tmp889;void*_tmp888;(_tmp888=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp889="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp889,sizeof(char),50))),_tag_dyneither(_tmp888,sizeof(void*),0)));}_LL395:;}
# 2377
for(0;atts != 0;atts=atts->tl){
void*_tmp5F2=(void*)atts->hd;int _tmp5F4;int _tmp5F6;int _tmp5F8;_LL39B: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp5F3=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp5F2;if(_tmp5F3->tag != 21)goto _LL39D;else{_tmp5F4=_tmp5F3->f1;}}_LL39C: {
# 2380
unsigned int j=(unsigned int)_tmp5F4;
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp88C;void*_tmp88B;(_tmp88B=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp88C="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp88C,sizeof(char),52))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}
continue;}{
# 2386
struct Cyc_Absyn_Vardecl*_tmp5FB=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp5FB->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp88F="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp88F,sizeof(char),52))),_tag_dyneither(_tmp88E,sizeof(void*),0)));}
continue;}{
# 2392
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp5FE=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp892;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp891;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp5FF=(_tmp891=_region_malloc(_tmp5E6,sizeof(*_tmp891)),((_tmp891[0]=((_tmp892.tag=2,((_tmp892.f1=(int)j,((_tmp892.f2=t,_tmp892)))))),_tmp891)));
struct Cyc_CfFlowInfo_Place*_tmp893;struct Cyc_CfFlowInfo_Place*_tmp600=(_tmp893=_region_malloc(_tmp5E6,sizeof(*_tmp893)),((_tmp893->root=(void*)_tmp5FF,((_tmp893->fields=0,_tmp893)))));
_tmp5E9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp5E9,(void*)_tmp5FF,_tmp5FE);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp899;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp898;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp896;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp895;_tmp5E9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp5E9,(void*)((_tmp895=_region_malloc(_tmp5E6,sizeof(*_tmp895)),((_tmp895[0]=((_tmp896.tag=0,((_tmp896.f1=_tmp5FB,_tmp896)))),_tmp895)))),(void*)((_tmp898=_region_malloc(_tmp5E6,sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=5,((_tmp899.f1=_tmp600,_tmp899)))),_tmp898)))));}
goto _LL39A;};};}_LL39D: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp5F5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp5F2;if(_tmp5F5->tag != 20)goto _LL39F;else{_tmp5F6=_tmp5F5->f1;}}_LL39E: {
# 2402
unsigned int j=(unsigned int)_tmp5F6;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp89C;void*_tmp89B;(_tmp89B=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp89C="initializes attribute exceeds number of parameters",_tag_dyneither(_tmp89C,sizeof(char),51))),_tag_dyneither(_tmp89B,sizeof(void*),0)));}
# 2406
continue;}{
# 2408
struct Cyc_Absyn_Vardecl*_tmp60A=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
{void*_tmp60B=Cyc_Tcutil_compress(_tmp60A->type);void*_tmp60D;union Cyc_Absyn_Constraint*_tmp60E;union Cyc_Absyn_Constraint*_tmp60F;union Cyc_Absyn_Constraint*_tmp610;_LL3A4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 5)goto _LL3A6;else{_tmp60D=(_tmp60C->f1).elt_typ;_tmp60E=((_tmp60C->f1).ptr_atts).nullable;_tmp60F=((_tmp60C->f1).ptr_atts).bounds;_tmp610=((_tmp60C->f1).ptr_atts).zero_term;}}_LL3A5:
# 2411
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp60E)){
const char*_tmp89F;void*_tmp89E;(_tmp89E=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp89F="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmp89F,sizeof(char),55))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}
# 2414
if(!Cyc_Tcutil_is_bound_one(_tmp60F)){
const char*_tmp8A2;void*_tmp8A1;(_tmp8A1=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8A2="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmp8A2,sizeof(char),57))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}
# 2417
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp610)){
const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8A5="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmp8A5,sizeof(char),77))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}{
# 2420
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8A8;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp8A7;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp617=(_tmp8A7=_region_malloc(_tmp5E6,sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A8.tag=2,((_tmp8A8.f1=(int)j,((_tmp8A8.f2=_tmp60D,_tmp8A8)))))),_tmp8A7)));
struct Cyc_CfFlowInfo_Place*_tmp8A9;struct Cyc_CfFlowInfo_Place*_tmp618=(_tmp8A9=_region_malloc(_tmp5E6,sizeof(*_tmp8A9)),((_tmp8A9->root=(void*)_tmp617,((_tmp8A9->fields=0,_tmp8A9)))));
_tmp5E9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp5E9,(void*)_tmp617,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp60D,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8AF;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8AE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8AC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8AB;_tmp5E9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp5E9,(void*)((_tmp8AB=_region_malloc(_tmp5E6,sizeof(*_tmp8AB)),((_tmp8AB[0]=((_tmp8AC.tag=0,((_tmp8AC.f1=_tmp60A,_tmp8AC)))),_tmp8AB)))),(void*)((_tmp8AE=_region_malloc(_tmp5E6,sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=5,((_tmp8AF.f1=_tmp618,_tmp8AF)))),_tmp8AE)))));}
{struct Cyc_List_List*_tmp8B0;param_roots=((_tmp8B0=_region_malloc(_tmp5E6,sizeof(*_tmp8B0)),((_tmp8B0->hd=_tmp618,((_tmp8B0->tl=param_roots,_tmp8B0))))));}
goto _LL3A3;};_LL3A6:;_LL3A7: {
const char*_tmp8B3;void*_tmp8B2;(_tmp8B2=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8B3="initializes attribute on non-pointer",_tag_dyneither(_tmp8B3,sizeof(char),37))),_tag_dyneither(_tmp8B2,sizeof(void*),0)));}_LL3A3:;}
# 2428
goto _LL39A;};}_LL39F: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp5F7=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp5F2;if(_tmp5F7->tag != 22)goto _LL3A1;else{_tmp5F8=_tmp5F7->f1;}}_LL3A0: {
# 2430
unsigned int j=(unsigned int)_tmp5F8;
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
{const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8B6="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmp8B6,sizeof(char),49))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}
continue;}{
# 2435
struct Cyc_Absyn_Vardecl*_tmp625=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
void*t=Cyc_Tcutil_compress(_tmp625->type);
if(!Cyc_Tcutil_is_noalias_pointer(t)){
{const char*_tmp8B9;void*_tmp8B8;(_tmp8B8=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8B9="noconsume attribute requires unique pointer type",_tag_dyneither(_tmp8B9,sizeof(char),49))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}
continue;}{
# 2442
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8BC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8BB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp628=(_tmp8BB=_region_malloc(_tmp5E6,sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BC.tag=0,((_tmp8BC.f1=_tmp625,_tmp8BC)))),_tmp8BB)));
struct Cyc_CfFlowInfo_Place*_tmp8BD;struct Cyc_CfFlowInfo_Place*_tmp629=(_tmp8BD=_region_malloc(_tmp5E6,sizeof(*_tmp8BD)),((_tmp8BD->root=(void*)_tmp628,((_tmp8BD->fields=0,_tmp8BD)))));
void*_tmp62A=Cyc_CfFlowInfo_lookup_place(_tmp5E9,_tmp629);
{struct _tuple24*_tmp8C0;struct Cyc_List_List*_tmp8BF;noconsume_rvals=((_tmp8BF=_region_malloc(_tmp5E6,sizeof(*_tmp8BF)),((_tmp8BF->hd=((_tmp8C0=_region_malloc(_tmp5E6,sizeof(*_tmp8C0)),((_tmp8C0->f1=_tmp629,((_tmp8C0->f2=_tmp62A,_tmp8C0)))))),((_tmp8BF->tl=noconsume_rvals,_tmp8BF))))));}
# 2447
goto _LL39A;};};}_LL3A1:;_LL3A2:
 goto _LL39A;_LL39A:;}
# 2450
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp5E9,_tmp5EA);}_LL390:;}{
# 2453
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp5E6,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp8C1;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp8C1=_region_malloc(_tmp5E6,sizeof(*_tmp8C1)),((_tmp8C1->r=_tmp5E6,((_tmp8C1->fenv=fenv,((_tmp8C1->iterate_again=1,((_tmp8C1->iteration_num=0,((_tmp8C1->in_try=0,((_tmp8C1->tryflow=inflow,((_tmp8C1->all_changed=0,((_tmp8C1->noreturn=noreturn,((_tmp8C1->param_roots=param_roots,((_tmp8C1->noconsume_params=noconsume_rvals,((_tmp8C1->flow_table=flow_table,_tmp8C1)))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_Position_error_p()){
++ env->iteration_num;
# 2463
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2466
union Cyc_CfFlowInfo_FlowInfo _tmp630=outflow;_LL3A9: if((_tmp630.BottomFL).tag != 1)goto _LL3AB;_LL3AA:
 goto _LL3A8;_LL3AB:;_LL3AC:
# 2469
 Cyc_NewControlFlow_check_init_params(_tmp5E7,env,outflow);
Cyc_NewControlFlow_check_noconsume_params(_tmp5E7,env,outflow);
# 2473
if(noreturn){
const char*_tmp8C4;void*_tmp8C3;(_tmp8C3=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8C4="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp8C4,sizeof(char),46))),_tag_dyneither(_tmp8C3,sizeof(void*),0)));}else{
# 2476
void*_tmp633=Cyc_Tcutil_compress(fd->ret_type);_LL3AE: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp634=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp633;if(_tmp634->tag != 0)goto _LL3B0;}_LL3AF:
 goto _LL3AD;_LL3B0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp635=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp633;if(_tmp635->tag != 7)goto _LL3B2;}_LL3B1:
 goto _LL3B3;_LL3B2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp636=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp633;if(_tmp636->tag != 6)goto _LL3B4;}_LL3B3:
# 2480
{const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,Cyc_Tcutil_warn(_tmp5E7,((_tmp8C7="function may not return a value",_tag_dyneither(_tmp8C7,sizeof(char),32))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}goto _LL3AD;_LL3B4:;_LL3B5:
# 2482
{const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,Cyc_Tcutil_terr(_tmp5E7,((_tmp8CA="function may not return a value",_tag_dyneither(_tmp8CA,sizeof(char),32))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}goto _LL3AD;_LL3AD:;}
# 2484
goto _LL3A8;_LL3A8:;};};};}
# 2489
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp63C=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp63E;struct Cyc_List_List*_tmp640;struct Cyc_List_List*_tmp642;struct Cyc_List_List*_tmp644;_LL3B7: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp63D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp63C;if(_tmp63D->tag != 1)goto _LL3B9;else{_tmp63E=_tmp63D->f1;}}_LL3B8:
 Cyc_NewControlFlow_check_fun(_tmp63E);goto _LL3B6;_LL3B9: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp63F=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp63C;if(_tmp63F->tag != 12)goto _LL3BB;else{_tmp640=_tmp63F->f1;}}_LL3BA:
 _tmp642=_tmp640;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp641=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp63C;if(_tmp641->tag != 11)goto _LL3BD;else{_tmp642=_tmp641->f2;}}_LL3BC:
 _tmp644=_tmp642;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp643=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp63C;if(_tmp643->tag != 10)goto _LL3BF;else{_tmp644=_tmp643->f2;}}_LL3BE:
 Cyc_NewControlFlow_cf_check(_tmp644);goto _LL3B6;_LL3BF: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp645=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp63C;if(_tmp645->tag != 13)goto _LL3C1;}_LL3C0:
 goto _LL3B6;_LL3C1:;_LL3C2:
 goto _LL3B6;_LL3B6:;}}
