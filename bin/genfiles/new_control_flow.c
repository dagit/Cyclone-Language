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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 971 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1150
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1152
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1164
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1168
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1170
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
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 179
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 182
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 184
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 205 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 209
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 214
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple16 pr1,struct _tuple16 pr2);
# 220
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 225
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 234
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp720;struct Cyc_NewControlFlow_CFStmtAnnot _tmp71F;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp71E;enclosee->annot=(void*)((_tmp71E=_cycalloc(sizeof(*_tmp71E)),((_tmp71E[0]=((_tmp720.tag=Cyc_NewControlFlow_CFAnnot,((_tmp720.f1=((_tmp71F.encloser=encloser,((_tmp71F.visited=0,_tmp71F)))),_tmp720)))),_tmp71E))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 110 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 118
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 120
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 127
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 130
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp5=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4;if(_tmp5->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp5->f1;}}_LL2:
 return _tmp6;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp726;const char*_tmp725;struct Cyc_Core_Impossible_exn_struct*_tmp724;(int)_throw((void*)((_tmp724=_cycalloc(sizeof(*_tmp724)),((_tmp724[0]=((_tmp726.tag=Cyc_Core_Impossible,((_tmp726.f1=((_tmp725="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp725,sizeof(char),37))),_tmp726)))),_tmp724)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp727;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp727=_region_malloc(env->r,sizeof(*_tmp727)),((_tmp727[0]=Cyc_CfFlowInfo_BottomFL(),_tmp727)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 144
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 147
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 155
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 161
_tmpB->visited=env->iteration_num;{
struct _tuple20 _tmp728;return(_tmp728.f1=_tmpB,((_tmp728.f2=_tmpC,_tmp728)));};}
# 171
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 179
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 186
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 188
struct Cyc_NewControlFlow_AnalEnv*_tmpF;unsigned int _tmp10;struct Cyc_Dict_Dict _tmp11;struct _tuple21*_tmpE=ckenv;_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;{
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
{const char*_tmp72C;void*_tmp72B[1];struct Cyc_String_pa_PrintArg_struct _tmp72A;(_tmp72A.tag=0,((_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp72B[0]=& _tmp72A,Cyc_Tcutil_warn(_tmp10,((_tmp72C="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp72C,sizeof(char),60))),_tag_dyneither(_tmp72B,sizeof(void*),1)))))));}
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
struct _tuple19 _tmp72D;struct _tuple19 _tmp21=(_tmp72D.f1=flow,((_tmp72D.f2=_tmp20,_tmp72D)));struct _tuple19 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple21 _tmp72E;struct _tuple21 _tmp25=(_tmp72E.f1=env,((_tmp72E.f2=s->loc,((_tmp72E.f3=_tmp24,_tmp72E)))));
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp25,_tmp23);
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp731;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp730;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp730=_region_malloc(fenv->r,sizeof(*_tmp730)),((_tmp730[0]=((_tmp731.tag=0,((_tmp731.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp731)))),_tmp730)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp734;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp733;_tmp2C=(void*)((_tmp733=_region_malloc(fenv->r,sizeof(*_tmp733)),((_tmp733[0]=((_tmp734.tag=8,((_tmp734.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp734.f2=_tmp2C,_tmp734)))))),_tmp733))));}
# 259
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp735;if(Cyc_Relations_consistent_relations(((_tmp735=_region_malloc(r,sizeof(*_tmp735)),((_tmp735->hd=_tmp31,((_tmp735->tl=assume,_tmp735))))))))
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
{const char*_tmp73A;void*_tmp739[2];struct Cyc_String_pa_PrintArg_struct _tmp738;struct Cyc_String_pa_PrintArg_struct _tmp737;(_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp738.tag=0,((_tmp738.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp739[0]=& _tmp738,((_tmp739[1]=& _tmp737,Cyc_CfFlowInfo_aerr(loc,((_tmp73A="unable to prove %s, required to access %s",_tag_dyneither(_tmp73A,sizeof(char),42))),_tag_dyneither(_tmp739,sizeof(void*),2)))))))))))));}
{const char*_tmp73D;void*_tmp73C;(_tmp73C=0,Cyc_fprintf(Cyc_stderr,((_tmp73D="  [recorded facts on path: ",_tag_dyneither(_tmp73D,sizeof(char),28))),_tag_dyneither(_tmp73C,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp740;void*_tmp73F;(_tmp73F=0,Cyc_fprintf(Cyc_stderr,((_tmp740="]\n",_tag_dyneither(_tmp740,sizeof(char),3))),_tag_dyneither(_tmp73F,sizeof(void*),0)));};}}
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
{const char*_tmp745;void*_tmp744[2];struct Cyc_String_pa_PrintArg_struct _tmp743;struct Cyc_String_pa_PrintArg_struct _tmp742;(_tmp742.tag=0,((_tmp742.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp744[0]=& _tmp743,((_tmp744[1]=& _tmp742,Cyc_CfFlowInfo_aerr(loc,((_tmp745="unable to prove %s, required to access %s",_tag_dyneither(_tmp745,sizeof(char),42))),_tag_dyneither(_tmp744,sizeof(void*),2)))))))))))));}
{const char*_tmp748;void*_tmp747;(_tmp747=0,Cyc_fprintf(Cyc_stderr,((_tmp748="  [recorded facts on path: ",_tag_dyneither(_tmp748,sizeof(char),28))),_tag_dyneither(_tmp747,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_fprintf(Cyc_stderr,((_tmp74B="]\n",_tag_dyneither(_tmp74B,sizeof(char),3))),_tag_dyneither(_tmp74A,sizeof(void*),0)));};}
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
const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_CfFlowInfo_aerr(loc,((_tmp74E="expression may not be fully initialized",_tag_dyneither(_tmp74E,sizeof(char),40))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp74F;struct _tuple0 _tmp5F=(_tmp74F.f1=Cyc_Tcutil_compress(t),((_tmp74F.f2=r,_tmp74F)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp6D,sizeof(void*),i)));}}
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
{const char*_tmp752;void*_tmp751;(_tmp751=0,Cyc_Tcutil_warn(loc,((_tmp752="argument may still contain unique pointers",_tag_dyneither(_tmp752,sizeof(char),43))),_tag_dyneither(_tmp751,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp755;void*_tmp754;(_tmp754=0,Cyc_CfFlowInfo_aerr(loc,((_tmp755="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp755,sizeof(char),47))),_tag_dyneither(_tmp754,sizeof(void*),0)));}
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
const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_warn(loc,((_tmp758="argument may contain live unique pointers",_tag_dyneither(_tmp758,sizeof(char),42))),_tag_dyneither(_tmp757,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 402
struct Cyc_Dict_Dict _tmp8D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp81,env->all_changed,r);
if(_tmp81.t == _tmp8D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp8E=Cyc_CfFlowInfo_ReachableFL(_tmp8D,_tmp82);
Cyc_NewControlFlow_update_tryflow(env,_tmp8E);
return _tmp8E;};};};_LL4A:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 411
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 418
if(es == 0){
struct _tuple23 _tmp759;return(_tmp759.f1=inflow,((_tmp759.f2=0,_tmp759)));}
if(es->tl == 0){
struct _tuple16 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple16 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp75C;struct _tuple23 _tmp75B;return(_tmp75B.f1=_tmp92,((_tmp75B.f2=((_tmp75C=_region_malloc(rgn,sizeof(*_tmp75C)),((_tmp75C->hd=_tmp93,((_tmp75C->tl=0,_tmp75C)))))),_tmp75B)));};}{
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
{struct Cyc_Dict_Dict*_tmp75D;env->all_changed=((_tmp75D=_region_malloc(env->r,sizeof(*_tmp75D)),((_tmp75D[0]=(env->fenv)->mt_place_set,_tmp75D))));}{
struct _tuple16 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple16 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp75E;rvals=((_tmp75E=_region_malloc(rgn,sizeof(*_tmp75E)),((_tmp75E->hd=_tmp9A,((_tmp75E->tl=0,_tmp75E))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 440
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp75F;env->all_changed=((_tmp75F=_region_malloc(env->r,sizeof(*_tmp75F)),((_tmp75F[0]=(env->fenv)->mt_place_set,_tmp75F))));}{
struct _tuple16 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple16 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp760;rvals=((_tmp760=_region_malloc(rgn,sizeof(*_tmp760)),((_tmp760->hd=_tmpA0,((_tmp760->tl=rvals,_tmp760))))));}
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
struct Cyc_Dict_Dict*_tmp761;env->all_changed=((_tmp761=_region_malloc(env->r,sizeof(*_tmp761)),((_tmp761[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp761))));}
# 463
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple23 _tmp762;return(_tmp762.f1=outflow,((_tmp762.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp762)));};};}
# 469
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 472
struct _RegionHandle _tmpA4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA4;_push_region(rgn);
{struct _tuple23 _tmpA5=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 473
union Cyc_CfFlowInfo_FlowInfo _tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple23 _tmpA6=_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
# 475
{union Cyc_CfFlowInfo_FlowInfo _tmpA9=_tmpA7;struct Cyc_Dict_Dict _tmpAA;_LL57: if((_tmpA9.ReachableFL).tag != 2)goto _LL59;_tmpAA=((struct _tuple14)(_tmpA9.ReachableFL).val).f1;_LL58:
# 477
 for(0;_tmpA8 != 0;(_tmpA8=_tmpA8->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAA,(void*)_tmpA8->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp765;void*_tmp764;(_tmp764=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp765="expression may not be initialized",_tag_dyneither(_tmp765,sizeof(char),34))),_tag_dyneither(_tmp764,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 483
struct _tuple16 _tmp766;struct _tuple16 _tmpAE=(_tmp766.f1=_tmpA7,((_tmp766.f2=(env->fenv)->unknown_all,_tmp766)));_npop_handler(0);return _tmpAE;};}
# 473
;_pop_region(rgn);}
# 491
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 498
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp76C;const char*_tmp76B;struct Cyc_Core_Impossible_exn_struct*_tmp76A;(int)_throw((void*)((_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A[0]=((_tmp76C.tag=Cyc_Core_Impossible,((_tmp76C.f1=((_tmp76B="consume_zero_ral",_tag_dyneither(_tmp76B,sizeof(char),17))),_tmp76C)))),_tmp76A)))));}else{
# 503
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 507
return new_rval;}}}
# 520 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 526
union Cyc_CfFlowInfo_FlowInfo _tmpB3=outflow;struct Cyc_Dict_Dict _tmpB4;struct Cyc_List_List*_tmpB5;_LL5C: if((_tmpB3.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpB3.ReachableFL).tag != 2)goto _LL5B;_tmpB4=((struct _tuple14)(_tmpB3.ReachableFL).val).f1;_tmpB5=((struct _tuple14)(_tmpB3.ReachableFL).val).f2;_LL5F: {
# 529
union Cyc_CfFlowInfo_AbsLVal _tmpB6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB7=_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL61: if((_tmpB7.UnknownL).tag != 2)goto _LL63;_LL62:
# 533
 return outflow;_LL63: if((_tmpB7.PlaceL).tag != 1)goto _LL60;_tmpB8=(struct Cyc_CfFlowInfo_Place*)(_tmpB7.PlaceL).val;_LL64: {
# 537
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp76F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp76E;nzval=(void*)((_tmp76E=_region_malloc((env->fenv)->r,sizeof(*_tmp76E)),((_tmp76E[0]=((_tmp76F.tag=8,((_tmp76F.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp76F.f2=nzval,_tmp76F)))))),_tmp76E))));}
# 541
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 546
return _tmpBB;};}_LL60:;}_LL5B:;}
# 555
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 558
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple19 _tmp770;return(_tmp770.f1=outflow,((_tmp770.f2=outflow,_tmp770)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 561
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple19 _tmp771;return(_tmp771.f1=outflow,((_tmp771.f2=outflow,_tmp771)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 564
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp774;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp773;nzval=(void*)((_tmp773=_region_malloc((env->fenv)->r,sizeof(*_tmp773)),((_tmp773[0]=((_tmp774.tag=8,((_tmp774.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp774.f2=nzval,_tmp774)))))),_tmp773))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp777;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp776;zval=(void*)((_tmp776=_region_malloc((env->fenv)->r,sizeof(*_tmp776)),((_tmp776[0]=((_tmp777.tag=8,((_tmp777.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp777.f2=zval,_tmp777)))))),_tmp776))));};}
# 570
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple19 _tmp778;return
(_tmp778.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp778.f2=
# 575
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp778)));};}_LL6A:;}_LL65:;}
# 581
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 589
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 593
void*_tmpCB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCC=_tmpCB;void*_tmpCE;union Cyc_Absyn_Constraint*_tmpCF;union Cyc_Absyn_Constraint*_tmpD0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 5)goto _LL72;else{_tmpCE=(_tmpCD->f1).elt_typ;_tmpCF=((_tmpCD->f1).ptr_atts).bounds;_tmpD0=((_tmpCD->f1).ptr_atts).zero_term;}}_LL71: {
# 595
union Cyc_CfFlowInfo_FlowInfo _tmpD1=f;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD3;_LL75: if((_tmpD1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 597
struct _tuple16 _tmp779;return(_tmp779.f1=f,((_tmp779.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp779)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 600
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct _tuple15 _tmpD5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple15 _tmpD6=_tmpD5;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
void*_tmpD9=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpE0;enum Cyc_CfFlowInfo_InitLevel _tmpE3;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDA->tag != 8)goto _LL7C;else{_tmpDB=_tmpDA->f1;_tmpDC=(void*)_tmpDA->f2;}}_LL7B: {
# 604
struct Cyc_Core_Impossible_exn_struct _tmp77F;const char*_tmp77E;struct Cyc_Core_Impossible_exn_struct*_tmp77D;(int)_throw((void*)((_tmp77D=_cycalloc(sizeof(*_tmp77D)),((_tmp77D[0]=((_tmp77F.tag=Cyc_Core_Impossible,((_tmp77F.f1=((_tmp77E="named location in anal_derefR",_tag_dyneither(_tmp77E,sizeof(char),30))),_tmp77F)))),_tmp77D)))));}_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDD->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDE->tag != 2)goto _LL80;}_LL7F:
# 607
{void*_tmpE7=e->annot;void*_tmpE8=_tmpE7;struct Cyc_List_List*_tmpEA;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE8;if(_tmpE9->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpEA=_tmpE9->f1;}}_LL8A:
# 609
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEA))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 613
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC;if(_tmpED->tag != 22)goto _LL90;}_LL8F:
# 615
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp782;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp781;e->annot=(void*)((_tmp781=_cycalloc(sizeof(*_tmp781)),((_tmp781[0]=((_tmp782.tag=Cyc_CfFlowInfo_NotZero,((_tmp782.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp782)))),_tmp781))));}
goto _LL8D;_LL90:;_LL91:
# 618
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 621
goto _LL88;_LL88:;}
# 623
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpDF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDF->tag != 5)goto _LL82;else{_tmpE0=_tmpDF->f1;}}_LL81:
# 625
{void*_tmpF0=e->annot;void*_tmpF1=_tmpF0;struct Cyc_List_List*_tmpF3;_LL93: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpF2=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF2->tag != Cyc_CfFlowInfo_NotZero)goto _LL95;else{_tmpF3=_tmpF2->f1;}}_LL94:
# 627
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpF3))goto _LL96;
goto _LL92;_LL95:;_LL96:
# 631
{void*_tmpF4=e->r;void*_tmpF5=_tmpF4;_LL98: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpF6->tag != 22)goto _LL9A;}_LL99:
# 633
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp785;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp784;e->annot=(void*)((_tmp784=_cycalloc(sizeof(*_tmp784)),((_tmp784[0]=((_tmp785.tag=Cyc_CfFlowInfo_NotZero,((_tmp785.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp785)))),_tmp784))));}
goto _LL97;_LL9A:;_LL9B:
# 636
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL97;_LL97:;}
# 639
goto _LL92;_LL92:;}{
# 641
struct _tuple16 _tmp786;return(_tmp786.f1=f,((_tmp786.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpE0),_tmp786)));};_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE1->tag != 0)goto _LL84;}_LL83:
# 643
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp787;return(_tmp787.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp787.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp787)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE2->tag != 3)goto _LL86;else{_tmpE3=_tmpE2->f1;}}_LL85:
# 646
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpE3,_tmpD8);goto _LL87;_LL86:;_LL87:
# 648
{void*_tmpFB=e->r;void*_tmpFC=_tmpFB;_LL9D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFC;if(_tmpFD->tag != 22)goto _LL9F;}_LL9E:
# 650
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp78A;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp789;e->annot=(void*)((_tmp789=_cycalloc(sizeof(*_tmp789)),((_tmp789[0]=((_tmp78A.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp78A.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp78A)))),_tmp789))));}
goto _LL9C;_LL9F:;_LLA0:
# 653
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9C;_LL9C:;}
# 656
goto _LL79;_LL79:;};}else{
# 659
void*_tmp100=e->annot;void*_tmp101=_tmp100;struct Cyc_List_List*_tmp103;_LLA2: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp102=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp101;if(_tmp102->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA4;else{_tmp103=_tmp102->f1;}}_LLA3:
# 661
 if(!Cyc_Relations_same_relns(_tmpD3,_tmp103))goto _LLA5;
goto _LLA1;_LLA4:;_LLA5:
# 664
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp78D;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp78C;e->annot=(void*)((_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C[0]=((_tmp78D.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp78D.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp78D)))),_tmp78C))));}
goto _LLA1;_LLA1:;}{
# 668
enum Cyc_CfFlowInfo_InitLevel _tmp106=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmp106){case Cyc_CfFlowInfo_NoneIL: _LLA6:
# 670
{void*_tmp107=r;_LLA9: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp108=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp107;if(_tmp108->tag != 7)goto _LLAB;}_LLAA:
# 672
{const char*_tmp790;void*_tmp78F;(_tmp78F=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp790="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp790,sizeof(char),61))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}
goto _LLA8;_LLAB:;_LLAC: {
# 675
const char*_tmp793;void*_tmp792;(_tmp792=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp793="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp793,sizeof(char),46))),_tag_dyneither(_tmp792,sizeof(void*),0)));}_LLA8:;}
# 677
goto _LLA7;case Cyc_CfFlowInfo_AllIL: _LLA7: {
# 679
struct _tuple16 _tmp794;return(_tmp794.f1=f,((_tmp794.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp794)));}case Cyc_CfFlowInfo_ThisIL: _LLAD: {
# 681
struct _tuple16 _tmp795;return(_tmp795.f1=f,((_tmp795.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp795)));}}};_LL74:;}_LL72:;_LL73: {
# 684
struct Cyc_Core_Impossible_exn_struct _tmp79B;const char*_tmp79A;struct Cyc_Core_Impossible_exn_struct*_tmp799;(int)_throw((void*)((_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799[0]=((_tmp79B.tag=Cyc_Core_Impossible,((_tmp79B.f1=((_tmp79A="right deref of non-pointer-type",_tag_dyneither(_tmp79A,sizeof(char),32))),_tmp79B)))),_tmp799)))));}_LL6F:;}
# 691
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 694
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 697
{void*_tmp112=e1->r;void*_tmp113=_tmp112;void*_tmp115;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113;if(_tmp114->tag != 1)goto _LLB2;else{_tmp115=(void*)_tmp114->f2;}}_LLB1: {
# 699
struct Cyc_Absyn_Vardecl*_tmp116=Cyc_Tcutil_nonesc_vardecl(_tmp115);
if(_tmp116 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp116);
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
struct _tuple18 _tmp117=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp119;struct _tuple18 _tmp118=_tmp117;_tmp119=_tmp118.f2;
{struct _tuple18 _tmp79C;struct _tuple18 _tmp11A=(_tmp79C.f1=inflow,((_tmp79C.f2=_tmp119,_tmp79C)));struct _tuple18 _tmp11B=_tmp11A;struct Cyc_Dict_Dict _tmp11C;struct Cyc_List_List*_tmp11D;struct Cyc_CfFlowInfo_Place*_tmp11E;_LLB5: if(((_tmp11B.f1).ReachableFL).tag != 2)goto _LLB7;_tmp11C=((struct _tuple14)((_tmp11B.f1).ReachableFL).val).f1;_tmp11D=((struct _tuple14)((_tmp11B.f1).ReachableFL).val).f2;if(((_tmp11B.f2).PlaceL).tag != 1)goto _LLB7;_tmp11E=(struct Cyc_CfFlowInfo_Place*)((_tmp11B.f2).PlaceL).val;_LLB6: {
# 731
void*_tmp11F=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 733
struct _tuple15 _tmp120=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp122;struct Cyc_List_List*_tmp123;struct _tuple15 _tmp121=_tmp120;_tmp122=_tmp121.f1;_tmp123=_tmp121.f2;
for(0;_tmp123 != 0;_tmp123=_tmp123->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp79F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp79E;_tmp11F=(void*)((_tmp79E=_region_malloc((env->fenv)->r,sizeof(*_tmp79E)),((_tmp79E[0]=((_tmp79F.tag=8,((_tmp79F.f1=(struct Cyc_Absyn_Vardecl*)_tmp123->hd,((_tmp79F.f2=_tmp11F,_tmp79F)))))),_tmp79E))));}
# 738
_tmp11C=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp11C,env->all_changed,_tmp11E,_tmp11F);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp11C,_tmp11D);
# 743
goto _LLB4;}_LLB7: if(((_tmp11B.f1).BottomFL).tag != 1)goto _LLB9;_LLB8:
 goto _LLB4;_LLB9:;_LLBA:
# 746
{const char*_tmp7A2;void*_tmp7A1;(_tmp7A1=0,Cyc_CfFlowInfo_aerr(loc,((_tmp7A2="noconsume() parameters must be l-values",_tag_dyneither(_tmp7A2,sizeof(char),40))),_tag_dyneither(_tmp7A1,sizeof(void*),0)));}
goto _LLB4;_LLB4:;}
# 749
return inflow;}
# 754
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 762
union Cyc_CfFlowInfo_FlowInfo _tmp129=outflow;struct Cyc_Dict_Dict _tmp12A;struct Cyc_List_List*_tmp12B;_LLBC: if((_tmp129.BottomFL).tag != 1)goto _LLBE;_LLBD: {
# 765
struct _tuple16 _tmp7A3;return(_tmp7A3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A3.f2=rval,_tmp7A3)));}_LLBE: if((_tmp129.ReachableFL).tag != 2)goto _LLBB;_tmp12A=((struct _tuple14)(_tmp129.ReachableFL).val).f1;_tmp12B=((struct _tuple14)(_tmp129.ReachableFL).val).f2;_LLBF: {
# 767
union Cyc_CfFlowInfo_AbsLVal _tmp12D=lval;struct Cyc_CfFlowInfo_Place*_tmp12E;_LLC1: if((_tmp12D.PlaceL).tag != 1)goto _LLC3;_tmp12E=(struct Cyc_CfFlowInfo_Place*)(_tmp12D.PlaceL).val;_LLC2: {
# 769
struct Cyc_Dict_Dict _tmp12F=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp12A,env->all_changed,_tmp12E,rval);
_tmp12B=Cyc_Relations_reln_assign_exp(fenv->r,_tmp12B,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp12F,_tmp12B);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 775
struct _tuple15 _tmp130=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp12A,_tmp12E));void*_tmp132;struct _tuple15 _tmp131=_tmp130;_tmp132=_tmp131.f1;{
void*_tmp133=_tmp132;_LLC6: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp134=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp133;if(_tmp134->tag != 3)goto _LLC8;else{if(_tmp134->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;}}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp135=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp133;if(_tmp135->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp136=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp133;if(_tmp136->tag != 0)goto _LLCC;}_LLCB:
 goto _LLC5;_LLCC:;_LLCD:
# 781
{const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7A6="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp7A6,sizeof(char),43))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}
goto _LLC5;_LLC5:;};}
# 786
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 788
struct _tuple16 _tmp7A7;return(_tmp7A7.f1=outflow,((_tmp7A7.f2=rval,_tmp7A7)));};}_LLC3: if((_tmp12D.UnknownL).tag != 2)goto _LLC0;_LLC4: {
# 791
struct _tuple16 _tmp7A8;return(_tmp7A8.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp7A8.f2=rval,_tmp7A8)));}_LLC0:;}_LLBB:;}
# 798
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 804
union Cyc_CfFlowInfo_FlowInfo _tmp13B=f;struct Cyc_Dict_Dict _tmp13C;struct Cyc_List_List*_tmp13D;_LLCF: if((_tmp13B.BottomFL).tag != 1)goto _LLD1;_LLD0:
 return Cyc_CfFlowInfo_BottomFL();_LLD1: if((_tmp13B.ReachableFL).tag != 2)goto _LLCE;_tmp13C=((struct _tuple14)(_tmp13B.ReachableFL).val).f1;_tmp13D=((struct _tuple14)(_tmp13B.ReachableFL).val).f2;_LLD2:
# 807
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7AE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7AD;struct Cyc_CfFlowInfo_Place*_tmp7AC;_tmp13C=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp13C,env->all_changed,(
(_tmp7AC=_region_malloc(env->r,sizeof(*_tmp7AC)),((_tmp7AC->root=(void*)((_tmp7AE=_region_malloc(env->r,sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7AD.tag=0,((_tmp7AD.f1=vd,_tmp7AD)))),_tmp7AE)))),((_tmp7AC->fields=0,_tmp7AC)))))),rval);}
# 810
_tmp13D=Cyc_Relations_reln_assign_var(env->r,_tmp13D,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp141=Cyc_CfFlowInfo_ReachableFL(_tmp13C,_tmp13D);
Cyc_NewControlFlow_update_tryflow(env,_tmp141);
# 817
return _tmp141;};_LLCE:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 821
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 828
if(vds == 0)return inflow;{
# 831
struct Cyc_List_List*_tmp142=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp7AF;es=((_tmp7AF=_cycalloc(sizeof(*_tmp7AF)),((_tmp7AF->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp7AF->tl=es,_tmp7AF))))));}}}
# 837
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp142,fenv->unknown_all,pat_loc,name_locs);
# 839
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 843
struct _tuple16 _tmp144=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp146;void*_tmp147;struct _tuple16 _tmp145=_tmp144;_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp146,_tmp147);}}{
# 851
struct Cyc_List_List*_tmp148=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
struct _tuple24*_tmp149=(struct _tuple24*)_tmp148->hd;struct Cyc_Absyn_Vardecl**_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct _tuple24*_tmp14A=_tmp149;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;
if(_tmp14B != 0  && _tmp14C != 0){
if(_tmp14C->topt == 0){
const char*_tmp7B3;void*_tmp7B2[1];struct Cyc_String_pa_PrintArg_struct _tmp7B1;(_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp14C)),((_tmp7B2[0]=& _tmp7B1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B3="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp7B3,sizeof(char),41))),_tag_dyneither(_tmp7B2,sizeof(void*),1)))))));}{
# 865
struct Cyc_List_List _tmp7B4;struct Cyc_List_List l=(_tmp7B4.hd=_tmp14C,((_tmp7B4.tl=0,_tmp7B4)));
union Cyc_CfFlowInfo_FlowInfo _tmp150=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp151=Cyc_NewControlFlow_anal_Lexp(env,_tmp150,0,0,_tmp14C);union Cyc_CfFlowInfo_AbsLVal _tmp153;struct _tuple18 _tmp152=_tmp151;_tmp153=_tmp152.f2;{
struct _tuple16 _tmp154=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp150,_tmp14C);union Cyc_CfFlowInfo_FlowInfo _tmp156;void*_tmp157;struct _tuple16 _tmp155=_tmp154;_tmp156=_tmp155.f1;_tmp157=_tmp155.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp158=_tmp156;struct Cyc_Dict_Dict _tmp159;struct Cyc_List_List*_tmp15A;_LLD4: if((_tmp158.ReachableFL).tag != 2)goto _LLD6;_tmp159=((struct _tuple14)(_tmp158.ReachableFL).val).f1;_tmp15A=((struct _tuple14)(_tmp158.ReachableFL).val).f2;_LLD5:
# 871
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp15B=_tmp153;struct Cyc_CfFlowInfo_Place*_tmp15C;_LLD9: if((_tmp15B.PlaceL).tag != 1)goto _LLDB;_tmp15C=(struct Cyc_CfFlowInfo_Place*)(_tmp15B.PlaceL).val;_LLDA:
# 874
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B6;_tmp157=(void*)((_tmp7B6=_region_malloc(fenv->r,sizeof(*_tmp7B6)),((_tmp7B6[0]=((_tmp7B7.tag=8,((_tmp7B7.f1=*_tmp14B,((_tmp7B7.f2=_tmp157,_tmp7B7)))))),_tmp7B6))));}{
# 877
void*_tmp15F=Cyc_CfFlowInfo_lookup_place(_tmp159,_tmp15C);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7BA;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B9;_tmp15F=(void*)((_tmp7B9=_region_malloc(fenv->r,sizeof(*_tmp7B9)),((_tmp7B9[0]=((_tmp7BA.tag=8,((_tmp7BA.f1=*_tmp14B,((_tmp7BA.f2=_tmp15F,_tmp7BA)))))),_tmp7B9))));}
_tmp159=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp159,env->all_changed,_tmp15C,_tmp15F);
_tmp156=Cyc_CfFlowInfo_ReachableFL(_tmp159,_tmp15A);
Cyc_NewControlFlow_update_tryflow(env,_tmp156);
goto _LLD8;};_LLDB:;_LLDC:
# 885
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp14C->topt)) && !
# 887
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp14B)->type)){
# 889
const char*_tmp7BE;void*_tmp7BD[1];struct Cyc_String_pa_PrintArg_struct _tmp7BC;(_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp14C)),((_tmp7BD[0]=& _tmp7BC,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp7BE="aliased pattern expression |%s| not an l-value",_tag_dyneither(_tmp7BE,sizeof(char),47))),_tag_dyneither(_tmp7BD,sizeof(void*),1)))))));}_LLD8:;}
# 900
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp156,*_tmp14B,_tmp14C,_tmp157,pat_loc);
goto _LLD3;_LLD6:;_LLD7:
# 903
 goto _LLD3;_LLD3:;};};};}}
# 908
return inflow;};};}
# 911
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 913
void*_tmp166=e->r;void*_tmp167=_tmp166;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp177;_LLDE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp168->tag != 1)goto _LLE0;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp169=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp168->f2);if(_tmp169->tag != 4)goto _LLE0;}}_LLDF:
 goto _LLE1;_LLE0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp16A->tag != 1)goto _LLE2;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16A->f2);if(_tmp16B->tag != 3)goto _LLE2;}}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp16C->tag != 1)goto _LLE4;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp16C->f2);if(_tmp16D->tag != 5)goto _LLE4;}}_LLE3:
 return 1;_LLE4: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp16E->tag != 19)goto _LLE6;else{_tmp16F=_tmp16E->f1;}}_LLE5:
 _tmp171=_tmp16F;goto _LLE7;_LLE6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp170->tag != 20)goto _LLE8;else{_tmp171=_tmp170->f1;}}_LLE7:
 _tmp173=_tmp171;goto _LLE9;_LLE8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp172->tag != 21)goto _LLEA;else{_tmp173=_tmp172->f1;}}_LLE9:
# 920
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp173,cast_ok);_LLEA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp174->tag != 22)goto _LLEC;else{_tmp175=_tmp174->f1;}}_LLEB: {
# 922
void*_tmp178=Cyc_Tcutil_compress((void*)_check_null(_tmp175->topt));void*_tmp179=_tmp178;_LLF1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp179;if(_tmp17A->tag != 10)goto _LLF3;}_LLF2:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp175,cast_ok);_LLF3:;_LLF4:
 return 0;_LLF0:;}_LLEC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp176->tag != 13)goto _LLEE;else{_tmp177=_tmp176->f2;}}_LLED:
# 927
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp177,cast_ok);else{
return 0;}_LLEE:;_LLEF:
 return 0;_LLDD:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp871(unsigned int*_tmp870,unsigned int*_tmp86F,void***_tmp86D,struct Cyc_List_List**_tmp37F){for(*_tmp870=0;*_tmp870 < *_tmp86F;(*_tmp870)++){
# 1508 "new_control_flow.cyc"
void*_tmp86B;(*_tmp86D)[*_tmp870]=(void*)((_tmp86B=(void*)((struct Cyc_List_List*)_check_null(*_tmp37F))->hd,((*_tmp37F=(*_tmp37F)->tl,_tmp86B))));}}
# 934 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 938
struct Cyc_CfFlowInfo_FlowEnv*_tmp17B=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 951
{union Cyc_CfFlowInfo_FlowInfo _tmp17C=inflow;struct Cyc_Dict_Dict _tmp17D;struct Cyc_List_List*_tmp17E;_LLF6: if((_tmp17C.BottomFL).tag != 1)goto _LLF8;_LLF7: {
struct _tuple16 _tmp7BF;return(_tmp7BF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7BF.f2=_tmp17B->unknown_all,_tmp7BF)));}_LLF8: if((_tmp17C.ReachableFL).tag != 2)goto _LLF5;_tmp17D=((struct _tuple14)(_tmp17C.ReachableFL).val).f1;_tmp17E=((struct _tuple14)(_tmp17C.ReachableFL).val).f2;_LLF9:
 d=_tmp17D;relns=_tmp17E;_LLF5:;}
# 966 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 987 "new_control_flow.cyc"
struct _tuple18 _tmp180=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp182;union Cyc_CfFlowInfo_AbsLVal _tmp183;struct _tuple18 _tmp181=_tmp180;_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;{
struct _tuple18 _tmp7C0;struct _tuple18 _tmp184=(_tmp7C0.f1=_tmp182,((_tmp7C0.f2=_tmp183,_tmp7C0)));struct _tuple18 _tmp185=_tmp184;struct Cyc_Dict_Dict _tmp186;struct Cyc_List_List*_tmp187;struct Cyc_CfFlowInfo_Place*_tmp188;_LLFB: if(((_tmp185.f1).ReachableFL).tag != 2)goto _LLFD;_tmp186=((struct _tuple14)((_tmp185.f1).ReachableFL).val).f1;_tmp187=((struct _tuple14)((_tmp185.f1).ReachableFL).val).f2;if(((_tmp185.f2).PlaceL).tag != 1)goto _LLFD;_tmp188=(struct Cyc_CfFlowInfo_Place*)((_tmp185.f2).PlaceL).val;_LLFC: {
# 990
void*_tmp189=Cyc_CfFlowInfo_lookup_place(_tmp186,_tmp188);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp189,& needs_unconsume)){
{const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7C3="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp7C3,sizeof(char),71))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}{
struct _tuple16 _tmp7C4;return(_tmp7C4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C4.f2=_tmp17B->unknown_all,_tmp7C4)));};}else{
# 996
if(needs_unconsume){
# 998
struct _tuple16 _tmp7C5;return(_tmp7C5.f1=_tmp182,((_tmp7C5.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp17B,_tmp189),_tmp7C5)));}else{
# 1001
void*_tmp18E=Cyc_CfFlowInfo_make_unique_consumed(_tmp17B,(void*)_check_null(e->topt),e,env->iteration_num,_tmp189);
struct Cyc_Dict_Dict _tmp18F=Cyc_CfFlowInfo_assign_place(_tmp17B,e->loc,_tmp186,env->all_changed,_tmp188,_tmp18E);
# 1012
struct _tuple16 _tmp7C6;return(_tmp7C6.f1=Cyc_CfFlowInfo_ReachableFL(_tmp18F,_tmp187),((_tmp7C6.f2=_tmp189,_tmp7C6)));}}}_LLFD:;_LLFE:
# 1014
 goto _LLFA;_LLFA:;};}{
# 1017
void*_tmp192=e->r;void*_tmp193=_tmp192;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp1AC;struct Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B2;enum Cyc_Absyn_Primop _tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_List_List*_tmp1C5;int _tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;void**_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;int _tmp1CB;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct _dyneither_ptr*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1DA;struct _dyneither_ptr*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;struct _dyneither_ptr*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Datatypedecl*_tmp1EE;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F4;enum Cyc_Absyn_AggrKind _tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F9;struct Cyc_Absyn_Vardecl*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;int _tmp1FE;struct Cyc_Absyn_Exp*_tmp200;void*_tmp201;int _tmp202;struct Cyc_Absyn_Stmt*_tmp204;_LL100: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp194->tag != 13)goto _LL102;else{_tmp195=_tmp194->f2;if(_tmp194->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL102;}}_LL101: {
# 1021
struct _tuple16 _tmp20B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp195);union Cyc_CfFlowInfo_FlowInfo _tmp20D;void*_tmp20E;struct _tuple16 _tmp20C=_tmp20B;_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;{
struct _tuple16 _tmp20F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp20D,_tmp195,_tmp20E);union Cyc_CfFlowInfo_FlowInfo _tmp211;void*_tmp212;struct _tuple16 _tmp210=_tmp20F;_tmp211=_tmp210.f1;_tmp212=_tmp210.f2;{
struct _tuple16 _tmp7C7;return(_tmp7C7.f1=_tmp211,((_tmp7C7.f2=_tmp20E,_tmp7C7)));};};}_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp196->tag != 13)goto _LL104;else{_tmp197=_tmp196->f2;}}_LL103:
# 1027
 _tmp199=_tmp197;goto _LL105;_LL104: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp198->tag != 11)goto _LL106;else{_tmp199=_tmp198->f1;}}_LL105:
 _tmp19B=_tmp199;goto _LL107;_LL106: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp19A->tag != 12)goto _LL108;else{_tmp19B=_tmp19A->f1;}}_LL107:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19B);_LL108: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp19C->tag != 0)goto _LL10A;else{if(((_tmp19C->f1).Null_c).tag != 1)goto _LL10A;}}_LL109:
# 1031
 goto _LL10B;_LL10A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp19D->tag != 0)goto _LL10C;else{if(((_tmp19D->f1).Int_c).tag != 5)goto _LL10C;if(((struct _tuple7)((_tmp19D->f1).Int_c).val).f2 != 0)goto _LL10C;}}_LL10B: {
struct _tuple16 _tmp7C8;return(_tmp7C8.f1=inflow,((_tmp7C8.f2=_tmp17B->zero,_tmp7C8)));}_LL10C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp19E->tag != 0)goto _LL10E;else{if(((_tmp19E->f1).Int_c).tag != 5)goto _LL10E;}}_LL10D:
 goto _LL10F;_LL10E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp19F->tag != 1)goto _LL110;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1A0=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp19F->f2);if(_tmp1A0->tag != 2)goto _LL110;}}_LL10F: {
struct _tuple16 _tmp7C9;return(_tmp7C9.f1=inflow,((_tmp7C9.f2=_tmp17B->notzeroall,_tmp7C9)));}_LL110: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A1->tag != 30)goto _LL112;else{if(_tmp1A1->f1 != 0)goto _LL112;}}_LL111:
# 1036
 goto _LL113;_LL112: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A2->tag != 0)goto _LL114;}_LL113:
 goto _LL115;_LL114: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A3->tag != 17)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A4->tag != 16)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A5->tag != 18)goto _LL11A;}_LL119:
 goto _LL11B;_LL11A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A6->tag != 32)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A7->tag != 31)goto _LL11E;}_LL11D: {
struct _tuple16 _tmp7CA;return(_tmp7CA.f1=inflow,((_tmp7CA.f2=_tmp17B->unknown_all,_tmp7CA)));}_LL11E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1A8->tag != 1)goto _LL120;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1A9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp1A8->f2);if(_tmp1A9->tag != 1)goto _LL120;}}_LL11F: {
# 1045
struct _tuple16 _tmp7CB;return(_tmp7CB.f1=inflow,((_tmp7CB.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp7CB)));}_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1AA->tag != 1)goto _LL122;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1AB=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp1AA->f2);if(_tmp1AB->tag != 3)goto _LL122;else{_tmp1AC=_tmp1AB->f1;}}}_LL121:
# 1048
 _tmp1AF=_tmp1AC;goto _LL123;_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1AD->tag != 1)goto _LL124;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1AE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp1AD->f2);if(_tmp1AE->tag != 4)goto _LL124;else{_tmp1AF=_tmp1AE->f1;}}}_LL123:
 _tmp1B2=_tmp1AF;goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1B0->tag != 1)goto _LL126;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B1=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp1B0->f2);if(_tmp1B1->tag != 5)goto _LL126;else{_tmp1B2=_tmp1B1->f1;}}}_LL125: {
# 1052
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7CC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp7CC.tag=0,((_tmp7CC.f1=_tmp1B2,_tmp7CC)));
struct _tuple16 _tmp7CD;return(_tmp7CD.f1=inflow,((_tmp7CD.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp7CD)));}_LL126: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1B3->tag != 2)goto _LL128;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B3->f2;}}_LL127: {
# 1059
struct _tuple16 _tmp21A=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1B5);union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*_tmp21D;struct _tuple16 _tmp21B=_tmp21A;_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;{
struct _tuple16 _tmp7CE;return(_tmp7CE.f1=_tmp21C,((_tmp7CE.f2=_tmp21D,_tmp7CE)));};}_LL128: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1B6=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1B6->tag != 4)goto _LL12A;else{_tmp1B7=_tmp1B6->f1;}}_LL129: {
# 1063
struct Cyc_List_List _tmp7CF;struct Cyc_List_List _tmp21F=(_tmp7CF.hd=_tmp1B7,((_tmp7CF.tl=0,_tmp7CF)));
struct _tuple16 _tmp220=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp21F);union Cyc_CfFlowInfo_FlowInfo _tmp222;struct _tuple16 _tmp221=_tmp220;_tmp222=_tmp221.f1;{
struct _tuple18 _tmp223=Cyc_NewControlFlow_anal_Lexp(env,_tmp222,0,0,_tmp1B7);union Cyc_CfFlowInfo_AbsLVal _tmp225;struct _tuple18 _tmp224=_tmp223;_tmp225=_tmp224.f2;
{struct _tuple25 _tmp7D0;struct _tuple25 _tmp226=(_tmp7D0.f1=_tmp225,((_tmp7D0.f2=_tmp222,_tmp7D0)));struct _tuple25 _tmp227=_tmp226;struct Cyc_CfFlowInfo_Place*_tmp228;struct Cyc_Dict_Dict _tmp229;struct Cyc_List_List*_tmp22A;_LL169: if(((_tmp227.f1).PlaceL).tag != 1)goto _LL16B;_tmp228=(struct Cyc_CfFlowInfo_Place*)((_tmp227.f1).PlaceL).val;if(((_tmp227.f2).ReachableFL).tag != 2)goto _LL16B;_tmp229=((struct _tuple14)((_tmp227.f2).ReachableFL).val).f1;_tmp22A=((struct _tuple14)((_tmp227.f2).ReachableFL).val).f2;_LL16A:
# 1068
 _tmp22A=Cyc_Relations_reln_kill_exp(_tmp17B->r,_tmp22A,_tmp1B7);
_tmp222=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp17B,_tmp1B7->loc,_tmp229,env->all_changed,_tmp228,_tmp17B->unknown_all),_tmp22A);
# 1072
goto _LL168;_LL16B:;_LL16C:
 goto _LL168;_LL168:;}{
# 1075
struct _tuple16 _tmp7D1;return(_tmp7D1.f1=_tmp222,((_tmp7D1.f2=_tmp17B->unknown_all,_tmp7D1)));};};}_LL12A: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1B8->tag != 3)goto _LL12C;else{_tmp1B9=_tmp1B8->f1;if(_tmp1B8->f2 == 0)goto _LL12C;_tmp1BA=_tmp1B8->f3;}}_LL12B:
# 1078
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7D4;void*_tmp7D3;(_tmp7D3=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7D4="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7D4,sizeof(char),58))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));}{
struct _tuple16 _tmp7D5;return(_tmp7D5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D5.f2=_tmp17B->unknown_all,_tmp7D5)));};}{
# 1082
struct Cyc_List_List _tmp7D6;struct Cyc_List_List _tmp231=(_tmp7D6.hd=_tmp1BA,((_tmp7D6.tl=0,_tmp7D6)));
struct Cyc_List_List _tmp7D7;struct Cyc_List_List _tmp232=(_tmp7D7.hd=_tmp1B9,((_tmp7D7.tl=& _tmp231,_tmp7D7)));
struct _tuple16 _tmp233=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp232);union Cyc_CfFlowInfo_FlowInfo _tmp235;struct _tuple16 _tmp234=_tmp233;_tmp235=_tmp234.f1;{
struct _tuple18 _tmp236=Cyc_NewControlFlow_anal_Lexp(env,_tmp235,0,0,_tmp1B9);union Cyc_CfFlowInfo_AbsLVal _tmp238;struct _tuple18 _tmp237=_tmp236;_tmp238=_tmp237.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp239=_tmp235;struct Cyc_Dict_Dict _tmp23A;struct Cyc_List_List*_tmp23B;_LL16E: if((_tmp239.ReachableFL).tag != 2)goto _LL170;_tmp23A=((struct _tuple14)(_tmp239.ReachableFL).val).f1;_tmp23B=((struct _tuple14)(_tmp239.ReachableFL).val).f2;_LL16F:
# 1088
{union Cyc_CfFlowInfo_AbsLVal _tmp23C=_tmp238;struct Cyc_CfFlowInfo_Place*_tmp23D;_LL173: if((_tmp23C.PlaceL).tag != 1)goto _LL175;_tmp23D=(struct Cyc_CfFlowInfo_Place*)(_tmp23C.PlaceL).val;_LL174:
# 1092
 _tmp23B=Cyc_Relations_reln_kill_exp(_tmp17B->r,_tmp23B,_tmp1B9);
_tmp23A=Cyc_CfFlowInfo_assign_place(_tmp17B,_tmp1B9->loc,_tmp23A,env->all_changed,_tmp23D,_tmp17B->unknown_all);
# 1095
_tmp235=Cyc_CfFlowInfo_ReachableFL(_tmp23A,_tmp23B);
# 1098
goto _LL172;_LL175: if((_tmp23C.UnknownL).tag != 2)goto _LL172;_LL176:
# 1101
 goto _LL172;_LL172:;}
# 1103
goto _LL16D;_LL170:;_LL171:
 goto _LL16D;_LL16D:;}{
# 1106
struct _tuple16 _tmp7D8;return(_tmp7D8.f1=_tmp235,((_tmp7D8.f2=_tmp17B->unknown_all,_tmp7D8)));};};};_LL12C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1BB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1BB->tag != 3)goto _LL12E;else{_tmp1BC=_tmp1BB->f1;if(_tmp1BB->f2 != 0)goto _LL12E;_tmp1BD=_tmp1BB->f3;}}_LL12D:
# 1110
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7DB;void*_tmp7DA;(_tmp7DA=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7DB="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7DB,sizeof(char),58))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}{
struct _tuple16 _tmp7DC;return(_tmp7DC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DC.f2=_tmp17B->unknown_all,_tmp7DC)));};}{
# 1114
struct Cyc_Dict_Dict*_tmp244=env->all_changed;
# 1116
{struct Cyc_Absyn_Exp*_tmp7DD[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp7DD[1]=_tmp1BD,((_tmp7DD[0]=_tmp1BC,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp7DD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp7DE;env->all_changed=((_tmp7DE=_region_malloc(env->r,sizeof(*_tmp7DE)),((_tmp7DE[0]=_tmp17B->mt_place_set,_tmp7DE))));}{
struct _tuple18 _tmp247=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1BC);union Cyc_CfFlowInfo_FlowInfo _tmp249;union Cyc_CfFlowInfo_AbsLVal _tmp24A;struct _tuple18 _tmp248=_tmp247;_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{
struct Cyc_Dict_Dict _tmp24B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp7DF;env->all_changed=((_tmp7DF=_region_malloc(env->r,sizeof(*_tmp7DF)),((_tmp7DF[0]=_tmp17B->mt_place_set,_tmp7DF))));}{
struct _tuple16 _tmp24D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1BD);union Cyc_CfFlowInfo_FlowInfo _tmp24F;void*_tmp250;struct _tuple16 _tmp24E=_tmp24D;_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;{
struct Cyc_Dict_Dict _tmp251=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp252=Cyc_CfFlowInfo_after_flow(_tmp17B,& _tmp24B,_tmp249,_tmp24F,_tmp24B,_tmp251);
# 1127
union Cyc_CfFlowInfo_FlowInfo _tmp253=Cyc_CfFlowInfo_join_flow(_tmp17B,_tmp244,inflow,_tmp252);
# 1130
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp253,inflow)){
if(_tmp244 == 0)
env->all_changed=0;else{
# 1134
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp244,
Cyc_CfFlowInfo_union_place_set(_tmp24B,_tmp251,0),0);}
# 1137
return Cyc_NewControlFlow_do_assign(_tmp17B,env,_tmp252,_tmp1BC,_tmp24A,_tmp1BD,_tmp250,e->loc);}
# 1140
inflow=_tmp253;};};};};}};_LL12E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1BE->tag != 8)goto _LL130;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;}}_LL12F: {
# 1145
struct _tuple16 _tmp254=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1BF);union Cyc_CfFlowInfo_FlowInfo _tmp256;void*_tmp257;struct _tuple16 _tmp255=_tmp254;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp256,_tmp1C0);}_LL130: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1C1=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1C1->tag != 10)goto _LL132;else{_tmp1C2=_tmp1C1->f1;}}_LL131: {
# 1149
struct _tuple16 _tmp258=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C2);union Cyc_CfFlowInfo_FlowInfo _tmp25A;void*_tmp25B;struct _tuple16 _tmp259=_tmp258;_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1C2->loc,_tmp25A,_tmp25B);{
struct _tuple16 _tmp7E0;return(_tmp7E0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp7E0)));};}_LL132: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1C3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1C3->tag != 9)goto _LL134;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;}}_LL133: {
# 1154
struct _RegionHandle _tmp25D=_new_region("temp");struct _RegionHandle*temp=& _tmp25D;_push_region(temp);
{struct Cyc_List_List*_tmp7E1;struct _tuple23 _tmp25E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp7E1=_region_malloc(temp,sizeof(*_tmp7E1)),((_tmp7E1->hd=_tmp1C4,((_tmp7E1->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1C5),_tmp7E1)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp260;struct Cyc_List_List*_tmp261;struct _tuple23 _tmp25F=_tmp25E;_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;{
# 1157
union Cyc_CfFlowInfo_FlowInfo _tmp262=Cyc_NewControlFlow_use_Rval(env,_tmp1C4->loc,_tmp260,(void*)((struct Cyc_List_List*)_check_null(_tmp261))->hd);
_tmp261=_tmp261->tl;{
# 1160
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp263=Cyc_Tcutil_compress((void*)_check_null(_tmp1C4->topt));void*_tmp264=_tmp263;void*_tmp266;_LL178: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264;if(_tmp265->tag != 5)goto _LL17A;else{_tmp266=(_tmp265->f1).elt_typ;}}_LL179:
# 1165
{void*_tmp267=Cyc_Tcutil_compress(_tmp266);void*_tmp268=_tmp267;struct Cyc_List_List*_tmp26A;_LL17D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp268;if(_tmp269->tag != 9)goto _LL17F;else{_tmp26A=(_tmp269->f1).attributes;}}_LL17E:
# 1167
 for(0;_tmp26A != 0;_tmp26A=_tmp26A->tl){
# 1169
void*_tmp26B=(void*)_tmp26A->hd;void*_tmp26C=_tmp26B;int _tmp26E;int _tmp270;int _tmp272;_LL182: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp26D=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp26C;if(_tmp26D->tag != 20)goto _LL184;else{_tmp26E=_tmp26D->f1;}}_LL183:
# 1171
{struct Cyc_List_List*_tmp7E2;init_params=((_tmp7E2=_region_malloc(temp,sizeof(*_tmp7E2)),((_tmp7E2->hd=(void*)_tmp26E,((_tmp7E2->tl=init_params,_tmp7E2))))));}goto _LL181;_LL184: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp26F=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp26C;if(_tmp26F->tag != 21)goto _LL186;else{_tmp270=_tmp26F->f1;}}_LL185:
# 1173
{struct Cyc_List_List*_tmp7E3;nolive_unique_params=((_tmp7E3=_region_malloc(temp,sizeof(*_tmp7E3)),((_tmp7E3->hd=(void*)_tmp270,((_tmp7E3->tl=nolive_unique_params,_tmp7E3))))));}
goto _LL181;_LL186: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp271=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp26C;if(_tmp271->tag != 22)goto _LL188;else{_tmp272=_tmp271->f1;}}_LL187:
# 1177
{struct Cyc_List_List*_tmp7E4;noconsume_params=((_tmp7E4=_region_malloc(temp,sizeof(*_tmp7E4)),((_tmp7E4->hd=(void*)_tmp272,((_tmp7E4->tl=noconsume_params,_tmp7E4))))));}
goto _LL181;_LL188:;_LL189:
 goto _LL181;_LL181:;}
# 1181
goto _LL17C;_LL17F:;_LL180: {
const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E7="anal_Rexp: bad function type",_tag_dyneither(_tmp7E7,sizeof(char),29))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}_LL17C:;}
# 1184
goto _LL177;_LL17A:;_LL17B: {
const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7EA="anal_Rexp: bad function type",_tag_dyneither(_tmp7EA,sizeof(char),29))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}_LL177:;}
# 1187
{int i=1;for(0;_tmp261 != 0;(((_tmp261=_tmp261->tl,_tmp1C5=((struct Cyc_List_List*)_check_null(_tmp1C5))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp27A=_tmp260;struct Cyc_Dict_Dict _tmp27B;_LL18B: if((_tmp27A.BottomFL).tag != 1)goto _LL18D;_LL18C:
 goto _LL18A;_LL18D: if((_tmp27A.ReachableFL).tag != 2)goto _LL18A;_tmp27B=((struct _tuple14)(_tmp27A.ReachableFL).val).f1;_LL18E:
# 1192
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp27B,(void*)_tmp261->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)->loc,((_tmp7ED="expression may not be initialized",_tag_dyneither(_tmp7ED,sizeof(char),34))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp27E=_tmp262;struct Cyc_Dict_Dict _tmp27F;struct Cyc_List_List*_tmp280;_LL190: if((_tmp27E.BottomFL).tag != 1)goto _LL192;_LL191:
 goto _LL18F;_LL192: if((_tmp27E.ReachableFL).tag != 2)goto _LL18F;_tmp27F=((struct _tuple14)(_tmp27E.ReachableFL).val).f1;_tmp280=((struct _tuple14)(_tmp27E.ReachableFL).val).f2;_LL193: {
# 1199
struct Cyc_Dict_Dict _tmp281=Cyc_CfFlowInfo_escape_deref(_tmp17B,_tmp27F,env->all_changed,(void*)_tmp261->hd);
{void*_tmp282=(void*)_tmp261->hd;void*_tmp283=_tmp282;struct Cyc_CfFlowInfo_Place*_tmp285;_LL195: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp284=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp283;if(_tmp284->tag != 5)goto _LL197;else{_tmp285=_tmp284->f1;}}_LL196:
# 1202
{void*_tmp286=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)->topt));void*_tmp287=_tmp286;void*_tmp289;_LL19A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp288=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp287;if(_tmp288->tag != 5)goto _LL19C;else{_tmp289=(_tmp288->f1).elt_typ;}}_LL19B:
# 1206
 _tmp281=Cyc_CfFlowInfo_assign_place(_tmp17B,((struct Cyc_Absyn_Exp*)_tmp1C5->hd)->loc,_tmp281,env->all_changed,_tmp285,
# 1208
Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,_tmp289,0,_tmp17B->esc_all));
# 1210
goto _LL199;_LL19C:;_LL19D: {
const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F0="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp7F0,sizeof(char),39))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}_LL199:;}
# 1213
goto _LL194;_LL197:;_LL198:
 goto _LL194;_LL194:;}
# 1216
_tmp262=Cyc_CfFlowInfo_ReachableFL(_tmp281,_tmp280);
goto _LL18F;}_LL18F:;}
# 1219
goto _LL18A;_LL18A:;}else{
# 1222
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1227
_tmp262=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)->topt),_tmp262,(void*)_tmp261->hd);else{
# 1234
_tmp262=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd)->loc,_tmp262,(void*)_tmp261->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1C5->hd)->topt),0))
_tmp262=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp262,(struct Cyc_Absyn_Exp*)_tmp1C5->hd,((struct Cyc_Absyn_Exp*)_tmp1C5->hd)->loc,(void*)_tmp261->hd);}}}}
# 1241
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1C4->topt))){
struct _tuple16 _tmp7F1;struct _tuple16 _tmp28D=(_tmp7F1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F1.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp7F1)));_npop_handler(0);return _tmp28D;}else{
# 1244
struct _tuple16 _tmp7F2;struct _tuple16 _tmp28F=(_tmp7F2.f1=_tmp262,((_tmp7F2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp7F2)));_npop_handler(0);return _tmp28F;}};};}
# 1155
;_pop_region(temp);}_LL134: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1C6=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1C6->tag != 33)goto _LL136;else{_tmp1C7=(_tmp1C6->f1).is_calloc;_tmp1C8=(_tmp1C6->f1).rgn;_tmp1C9=(_tmp1C6->f1).elt_type;_tmp1CA=(_tmp1C6->f1).num_elts;_tmp1CB=(_tmp1C6->f1).fat_result;}}_LL135: {
# 1247
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7F5;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp7F4;void*root=(void*)((_tmp7F4=_region_malloc(_tmp17B->r,sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F5.tag=1,((_tmp7F5.f1=_tmp1CA,((_tmp7F5.f2=(void*)_check_null(e->topt),_tmp7F5)))))),_tmp7F4))));
struct Cyc_CfFlowInfo_Place*_tmp7F6;struct Cyc_CfFlowInfo_Place*place=(_tmp7F6=_region_malloc(_tmp17B->r,sizeof(*_tmp7F6)),((_tmp7F6->root=root,((_tmp7F6->fields=0,_tmp7F6)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7F9;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp7F8;void*rval=(void*)((_tmp7F8=_region_malloc(_tmp17B->r,sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=5,((_tmp7F9.f1=place,_tmp7F9)))),_tmp7F8))));
void*place_val;
if(_tmp1CB)place_val=_tmp17B->notzeroall;else{
if(_tmp1C7)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,*((void**)_check_null(_tmp1C9)),0,_tmp17B->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,*((void**)_check_null(_tmp1C9)),0,_tmp17B->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1C8 != 0){
struct _RegionHandle _tmp291=_new_region("temp");struct _RegionHandle*temp=& _tmp291;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7FA[2];struct _tuple23 _tmp292=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7FA[1]=_tmp1CA,((_tmp7FA[0]=_tmp1C8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7FA,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1258
union Cyc_CfFlowInfo_FlowInfo _tmp294;struct Cyc_List_List*_tmp295;struct _tuple23 _tmp293=_tmp292;_tmp294=_tmp293.f1;_tmp295=_tmp293.f2;
# 1260
outflow=_tmp294;}
# 1258
;_pop_region(temp);}else{
# 1263
struct _tuple16 _tmp297=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CA);union Cyc_CfFlowInfo_FlowInfo _tmp299;struct _tuple16 _tmp298=_tmp297;_tmp299=_tmp298.f1;
outflow=_tmp299;}{
# 1266
union Cyc_CfFlowInfo_FlowInfo _tmp29A=outflow;struct Cyc_Dict_Dict _tmp29B;struct Cyc_List_List*_tmp29C;_LL19F: if((_tmp29A.BottomFL).tag != 1)goto _LL1A1;_LL1A0: {
struct _tuple16 _tmp7FB;return(_tmp7FB.f1=outflow,((_tmp7FB.f2=rval,_tmp7FB)));}_LL1A1: if((_tmp29A.ReachableFL).tag != 2)goto _LL19E;_tmp29B=((struct _tuple14)(_tmp29A.ReachableFL).val).f1;_tmp29C=((struct _tuple14)(_tmp29A.ReachableFL).val).f2;_LL1A2: {
# 1269
struct _tuple16 _tmp7FC;return(_tmp7FC.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29B,root,place_val),_tmp29C),((_tmp7FC.f2=rval,_tmp7FC)));}_LL19E:;};};}_LL136: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1CC=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1CC->tag != 34)goto _LL138;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;}}_LL137: {
# 1273
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1279
struct _RegionHandle _tmp2A4=_new_region("temp");struct _RegionHandle*temp=& _tmp2A4;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp7FD[2];struct _tuple23 _tmp2A5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7FD[1]=_tmp1CE,((_tmp7FD[0]=_tmp1CD,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7FD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1280
union Cyc_CfFlowInfo_FlowInfo _tmp2A7;struct Cyc_List_List*_tmp2A8;struct _tuple23 _tmp2A6=_tmp2A5;_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;
# 1282
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2A8))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2A8->tl))->hd;
outflow=_tmp2A7;}{
# 1287
struct _tuple18 _tmp2AA=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1CD);union Cyc_CfFlowInfo_AbsLVal _tmp2AC;struct _tuple18 _tmp2AB=_tmp2AA;_tmp2AC=_tmp2AB.f2;{
struct _tuple18 _tmp2AD=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1CE);union Cyc_CfFlowInfo_AbsLVal _tmp2AF;struct _tuple18 _tmp2AE=_tmp2AD;_tmp2AF=_tmp2AE.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2B0=outflow;struct Cyc_Dict_Dict _tmp2B1;struct Cyc_List_List*_tmp2B2;_LL1A4: if((_tmp2B0.ReachableFL).tag != 2)goto _LL1A6;_tmp2B1=((struct _tuple14)(_tmp2B0.ReachableFL).val).f1;_tmp2B2=((struct _tuple14)(_tmp2B0.ReachableFL).val).f2;_LL1A5:
# 1291
{union Cyc_CfFlowInfo_AbsLVal _tmp2B3=_tmp2AC;struct Cyc_CfFlowInfo_Place*_tmp2B4;_LL1A9: if((_tmp2B3.PlaceL).tag != 1)goto _LL1AB;_tmp2B4=(struct Cyc_CfFlowInfo_Place*)(_tmp2B3.PlaceL).val;_LL1AA:
# 1293
 _tmp2B1=Cyc_CfFlowInfo_assign_place(_tmp17B,_tmp1CD->loc,_tmp2B1,env->all_changed,_tmp2B4,right_rval);
# 1295
goto _LL1A8;_LL1AB: if((_tmp2B3.UnknownL).tag != 2)goto _LL1A8;_LL1AC:
# 1300
 if(Cyc_CfFlowInfo_initlevel(_tmp17B,_tmp2B1,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,Cyc_Tcutil_terr(_tmp1CE->loc,((_tmp800="expression may not be fully initialized",_tag_dyneither(_tmp800,sizeof(char),40))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}
goto _LL1A8;_LL1A8:;}
# 1304
{union Cyc_CfFlowInfo_AbsLVal _tmp2B7=_tmp2AF;struct Cyc_CfFlowInfo_Place*_tmp2B8;_LL1AE: if((_tmp2B7.PlaceL).tag != 1)goto _LL1B0;_tmp2B8=(struct Cyc_CfFlowInfo_Place*)(_tmp2B7.PlaceL).val;_LL1AF:
# 1306
 _tmp2B1=Cyc_CfFlowInfo_assign_place(_tmp17B,_tmp1CE->loc,_tmp2B1,env->all_changed,_tmp2B8,left_rval);
# 1308
goto _LL1AD;_LL1B0: if((_tmp2B7.UnknownL).tag != 2)goto _LL1AD;_LL1B1:
# 1310
 if(Cyc_CfFlowInfo_initlevel(_tmp17B,_tmp2B1,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(_tmp1CD->loc,((_tmp803="expression may not be fully initialized",_tag_dyneither(_tmp803,sizeof(char),40))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
goto _LL1AD;_LL1AD:;}
# 1315
_tmp2B2=Cyc_Relations_reln_kill_exp(_tmp17B->r,_tmp2B2,_tmp1CD);
_tmp2B2=Cyc_Relations_reln_kill_exp(_tmp17B->r,_tmp2B2,_tmp1CE);
# 1318
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B1,_tmp2B2);
goto _LL1A3;_LL1A6:;_LL1A7:
 goto _LL1A3;_LL1A3:;}{
# 1324
struct _tuple16 _tmp804;struct _tuple16 _tmp2BC=(_tmp804.f1=outflow,((_tmp804.f2=_tmp17B->unknown_all,_tmp804)));_npop_handler(0);return _tmp2BC;};};};
# 1279
;_pop_region(temp);}_LL138: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1CF=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1CF->tag != 15)goto _LL13A;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;}}_LL139: {
# 1327
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp807;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp806;void*root=(void*)((_tmp806=_region_malloc(_tmp17B->r,sizeof(*_tmp806)),((_tmp806[0]=((_tmp807.tag=1,((_tmp807.f1=_tmp1D1,((_tmp807.f2=(void*)_check_null(e->topt),_tmp807)))))),_tmp806))));
struct Cyc_CfFlowInfo_Place*_tmp808;struct Cyc_CfFlowInfo_Place*place=(_tmp808=_region_malloc(_tmp17B->r,sizeof(*_tmp808)),((_tmp808->root=root,((_tmp808->fields=0,_tmp808)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp80B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp80A;void*rval=(void*)((_tmp80A=_region_malloc(_tmp17B->r,sizeof(*_tmp80A)),((_tmp80A[0]=((_tmp80B.tag=5,((_tmp80B.f1=place,_tmp80B)))),_tmp80A))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1D0 != 0){
struct _RegionHandle _tmp2BD=_new_region("temp");struct _RegionHandle*temp=& _tmp2BD;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp80C[2];struct _tuple23 _tmp2BE=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp80C[1]=_tmp1D1,((_tmp80C[0]=_tmp1D0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp80C,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1335
union Cyc_CfFlowInfo_FlowInfo _tmp2C0;struct Cyc_List_List*_tmp2C1;struct _tuple23 _tmp2BF=_tmp2BE;_tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;
# 1337
outflow=_tmp2C0;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C1))->tl))->hd;}
# 1335
;_pop_region(temp);}else{
# 1341
struct _tuple16 _tmp2C3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp2C5;void*_tmp2C6;struct _tuple16 _tmp2C4=_tmp2C3;_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;
outflow=_tmp2C5;
place_val=_tmp2C6;}{
# 1345
union Cyc_CfFlowInfo_FlowInfo _tmp2C7=outflow;struct Cyc_Dict_Dict _tmp2C8;struct Cyc_List_List*_tmp2C9;_LL1B3: if((_tmp2C7.BottomFL).tag != 1)goto _LL1B5;_LL1B4: {
struct _tuple16 _tmp80D;return(_tmp80D.f1=outflow,((_tmp80D.f2=rval,_tmp80D)));}_LL1B5: if((_tmp2C7.ReachableFL).tag != 2)goto _LL1B2;_tmp2C8=((struct _tuple14)(_tmp2C7.ReachableFL).val).f1;_tmp2C9=((struct _tuple14)(_tmp2C7.ReachableFL).val).f2;_LL1B6: {
# 1348
struct _tuple16 _tmp80E;return(_tmp80E.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2C8,root,place_val),_tmp2C9),((_tmp80E.f2=rval,_tmp80E)));}_LL1B2:;};};}_LL13A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1D2->tag != 14)goto _LL13C;else{_tmp1D3=_tmp1D2->f1;}}_LL13B: {
# 1352
struct _tuple18 _tmp2D1=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1D3);union Cyc_CfFlowInfo_FlowInfo _tmp2D3;union Cyc_CfFlowInfo_AbsLVal _tmp2D4;struct _tuple18 _tmp2D2=_tmp2D1;_tmp2D3=_tmp2D2.f1;_tmp2D4=_tmp2D2.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2D5=_tmp2D4;struct Cyc_CfFlowInfo_Place*_tmp2D6;_LL1B8: if((_tmp2D5.UnknownL).tag != 2)goto _LL1BA;_LL1B9: {
struct _tuple16 _tmp80F;return(_tmp80F.f1=_tmp2D3,((_tmp80F.f2=_tmp17B->notzeroall,_tmp80F)));}_LL1BA: if((_tmp2D5.PlaceL).tag != 1)goto _LL1B7;_tmp2D6=(struct Cyc_CfFlowInfo_Place*)(_tmp2D5.PlaceL).val;_LL1BB: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp815;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp814;struct _tuple16 _tmp813;return(_tmp813.f1=_tmp2D3,((_tmp813.f2=(void*)((_tmp815=_region_malloc(env->r,sizeof(*_tmp815)),((_tmp815[0]=((_tmp814.tag=5,((_tmp814.f1=_tmp2D6,_tmp814)))),_tmp815)))),_tmp813)));}_LL1B7:;};}_LL13C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1D4->tag != 19)goto _LL13E;else{_tmp1D5=_tmp1D4->f1;}}_LL13D: {
# 1359
struct _tuple16 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1D5);union Cyc_CfFlowInfo_FlowInfo _tmp2DD;void*_tmp2DE;struct _tuple16 _tmp2DC=_tmp2DB;_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2DD,_tmp1D5,_tmp2DE);}_LL13E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1D6->tag != 20)goto _LL140;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL13F: {
# 1363
struct _tuple16 _tmp2DF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1D7);union Cyc_CfFlowInfo_FlowInfo _tmp2E1;void*_tmp2E2;struct _tuple16 _tmp2E0=_tmp2DF;_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;
if(_tmp1D7->topt == 0){
{const char*_tmp819;void*_tmp818[1];struct Cyc_String_pa_PrintArg_struct _tmp817;(_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp818[0]=& _tmp817,Cyc_fprintf(Cyc_stderr,((_tmp819="aggrmember exp %s\n",_tag_dyneither(_tmp819,sizeof(char),19))),_tag_dyneither(_tmp818,sizeof(void*),1)))))));}{
const char*_tmp81D;void*_tmp81C[1];struct Cyc_String_pa_PrintArg_struct _tmp81B;(_tmp81B.tag=0,((_tmp81B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1D7)),((_tmp81C[0]=& _tmp81B,Cyc_fprintf(Cyc_stderr,((_tmp81D="oops! %s.topt = null!\n",_tag_dyneither(_tmp81D,sizeof(char),23))),_tag_dyneither(_tmp81C,sizeof(void*),1)))))));};}{
# 1368
void*_tmp2E9=(void*)_check_null(_tmp1D7->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2E9)){
# 1371
struct _tuple16 _tmp81E;return(_tmp81E.f1=_tmp2E1,((_tmp81E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp81E)));}
# 1373
if(Cyc_Absyn_is_require_union_type(_tmp2E9))
Cyc_NewControlFlow_check_requires(_tmp1D7->loc,_tmp17B->r,_tmp2E9,_tmp1D8,_tmp2E1);{
# 1376
struct _tuple15 _tmp2EB=Cyc_CfFlowInfo_unname_rval(_tmp17B->r,_tmp2E2);void*_tmp2ED;struct _tuple15 _tmp2EC=_tmp2EB;_tmp2ED=_tmp2EC.f1;{
void*_tmp2EE=_tmp2ED;int _tmp2F0;int _tmp2F1;struct _dyneither_ptr _tmp2F2;_LL1BD: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2EF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2EE;if(_tmp2EF->tag != 6)goto _LL1BF;else{_tmp2F0=(_tmp2EF->f1).is_union;_tmp2F1=(_tmp2EF->f1).fieldnum;_tmp2F2=_tmp2EF->f2;}}_LL1BE: {
# 1379
int _tmp2F3=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1D7->topt),_tmp1D8);
if((_tmp2F0  && _tmp2F1 != - 1) && _tmp2F1 != _tmp2F3){
struct _tuple16 _tmp81F;return(_tmp81F.f1=_tmp2E1,((_tmp81F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),1,_tmp17B->unknown_none),_tmp81F)));}{
struct _tuple16 _tmp820;return(_tmp820.f1=_tmp2E1,((_tmp820.f2=*((void**)_check_dyneither_subscript(_tmp2F2,sizeof(void*),_tmp2F3)),_tmp820)));};}_LL1BF:;_LL1C0:
# 1384
{const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_fprintf(Cyc_stderr,((_tmp823="the bad rexp is :",_tag_dyneither(_tmp823,sizeof(char),18))),_tag_dyneither(_tmp822,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2ED);
{const char*_tmp826;void*_tmp825;(_tmp825=0,Cyc_fprintf(Cyc_stderr,((_tmp826="\n",_tag_dyneither(_tmp826,sizeof(char),2))),_tag_dyneither(_tmp825,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp833;const char*_tmp832;void*_tmp831[1];struct Cyc_String_pa_PrintArg_struct _tmp830;struct Cyc_Core_Impossible_exn_struct*_tmp82F;(int)_throw((void*)((_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F[0]=((_tmp833.tag=Cyc_Core_Impossible,((_tmp833.f1=(struct _dyneither_ptr)((_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp831[0]=& _tmp830,Cyc_aprintf(((_tmp832="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp832,sizeof(char),26))),_tag_dyneither(_tmp831,sizeof(void*),1)))))))),_tmp833)))),_tmp82F)))));};_LL1BC:;};};};}_LL140: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1D9->tag != 37)goto _LL142;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LL141: {
# 1394
struct _tuple16 _tmp2FF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DA);union Cyc_CfFlowInfo_FlowInfo _tmp301;void*_tmp302;struct _tuple16 _tmp300=_tmp2FF;_tmp301=_tmp300.f1;_tmp302=_tmp300.f2;
# 1396
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1DA->topt))){
struct _tuple16 _tmp834;return(_tmp834.f1=_tmp301,((_tmp834.f2=_tmp17B->unknown_all,_tmp834)));}{
struct _tuple15 _tmp304=Cyc_CfFlowInfo_unname_rval(_tmp17B->r,_tmp302);void*_tmp306;struct _tuple15 _tmp305=_tmp304;_tmp306=_tmp305.f1;{
void*_tmp307=_tmp306;int _tmp309;int _tmp30A;struct _dyneither_ptr _tmp30B;_LL1C2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp308=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp307;if(_tmp308->tag != 6)goto _LL1C4;else{_tmp309=(_tmp308->f1).is_union;_tmp30A=(_tmp308->f1).fieldnum;_tmp30B=_tmp308->f2;}}_LL1C3: {
# 1401
int _tmp30C=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1DA->topt),_tmp1DB);
if(_tmp309  && _tmp30A != - 1){
if(_tmp30A != _tmp30C){
struct _tuple16 _tmp835;return(_tmp835.f1=_tmp301,((_tmp835.f2=_tmp17B->zero,_tmp835)));}else{
# 1406
struct _tuple16 _tmp836;return(_tmp836.f1=_tmp301,((_tmp836.f2=_tmp17B->notzeroall,_tmp836)));}}else{
# 1408
struct _tuple16 _tmp837;return(_tmp837.f1=_tmp301,((_tmp837.f2=_tmp17B->unknown_all,_tmp837)));}}_LL1C4:;_LL1C5: {
# 1410
struct Cyc_Core_Impossible_exn_struct _tmp844;const char*_tmp843;void*_tmp842[1];struct Cyc_String_pa_PrintArg_struct _tmp841;struct Cyc_Core_Impossible_exn_struct*_tmp840;(int)_throw((void*)((_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((_tmp844.tag=Cyc_Core_Impossible,((_tmp844.f1=(struct _dyneither_ptr)((_tmp841.tag=0,((_tmp841.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp842[0]=& _tmp841,Cyc_aprintf(((_tmp843="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp843,sizeof(char),26))),_tag_dyneither(_tmp842,sizeof(void*),1)))))))),_tmp844)))),_tmp840)))));}_LL1C1:;};};}_LL142: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1DC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1DC->tag != 21)goto _LL144;else{_tmp1DD=_tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}}_LL143: {
# 1415
struct _tuple16 _tmp315=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DD);union Cyc_CfFlowInfo_FlowInfo _tmp317;void*_tmp318;struct _tuple16 _tmp316=_tmp315;_tmp317=_tmp316.f1;_tmp318=_tmp316.f2;{
# 1418
struct _tuple16 _tmp319=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp317,_tmp1DD,_tmp318);union Cyc_CfFlowInfo_FlowInfo _tmp31B;void*_tmp31C;struct _tuple16 _tmp31A=_tmp319;_tmp31B=_tmp31A.f1;_tmp31C=_tmp31A.f2;{
# 1421
void*_tmp31D=Cyc_Tcutil_compress((void*)_check_null(_tmp1DD->topt));void*_tmp31E=_tmp31D;void*_tmp320;_LL1C7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31E;if(_tmp31F->tag != 5)goto _LL1C9;else{_tmp320=(_tmp31F->f1).elt_typ;}}_LL1C8:
# 1423
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp320)){
# 1425
struct _tuple16 _tmp845;return(_tmp845.f1=_tmp31B,((_tmp845.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp845)));}
# 1427
if(Cyc_Absyn_is_require_union_type(_tmp320))
Cyc_NewControlFlow_check_requires(_tmp1DD->loc,_tmp17B->r,_tmp320,_tmp1DE,_tmp317);{
# 1430
struct _tuple15 _tmp322=Cyc_CfFlowInfo_unname_rval(_tmp17B->r,_tmp31C);void*_tmp324;struct _tuple15 _tmp323=_tmp322;_tmp324=_tmp323.f1;{
void*_tmp325=_tmp324;int _tmp327;int _tmp328;struct _dyneither_ptr _tmp329;_LL1CC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp326=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp325;if(_tmp326->tag != 6)goto _LL1CE;else{_tmp327=(_tmp326->f1).is_union;_tmp328=(_tmp326->f1).fieldnum;_tmp329=_tmp326->f2;}}_LL1CD: {
# 1433
int _tmp32A=Cyc_CfFlowInfo_get_field_index(_tmp320,_tmp1DE);
if((_tmp327  && _tmp328 != - 1) && _tmp328 != _tmp32A){
struct _tuple16 _tmp846;return(_tmp846.f1=_tmp31B,((_tmp846.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),1,_tmp17B->unknown_none),_tmp846)));}{
struct _tuple16 _tmp847;return(_tmp847.f1=_tmp31B,((_tmp847.f2=*((void**)_check_dyneither_subscript(_tmp329,sizeof(void*),_tmp32A)),_tmp847)));};}_LL1CE:;_LL1CF: {
struct Cyc_Core_Impossible_exn_struct _tmp84D;const char*_tmp84C;struct Cyc_Core_Impossible_exn_struct*_tmp84B;(int)_throw((void*)((_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84D.tag=Cyc_Core_Impossible,((_tmp84D.f1=((_tmp84C="anal_Rexp: AggrArrow",_tag_dyneither(_tmp84C,sizeof(char),21))),_tmp84D)))),_tmp84B)))));}_LL1CB:;};};_LL1C9:;_LL1CA: {
# 1439
struct Cyc_Core_Impossible_exn_struct _tmp853;const char*_tmp852;struct Cyc_Core_Impossible_exn_struct*_tmp851;(int)_throw((void*)((_tmp851=_cycalloc(sizeof(*_tmp851)),((_tmp851[0]=((_tmp853.tag=Cyc_Core_Impossible,((_tmp853.f1=((_tmp852="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp852,sizeof(char),25))),_tmp853)))),_tmp851)))));}_LL1C6:;};};}_LL144: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1DF=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1DF->tag != 5)goto _LL146;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;_tmp1E2=_tmp1DF->f3;}}_LL145: {
# 1443
struct _tuple19 _tmp333=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E0);union Cyc_CfFlowInfo_FlowInfo _tmp335;union Cyc_CfFlowInfo_FlowInfo _tmp336;struct _tuple19 _tmp334=_tmp333;_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;{
struct _tuple16 _tmp337=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp335,_tmp1E1);
struct _tuple16 _tmp338=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp336,_tmp1E2);
# 1447
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17B,env->all_changed,_tmp337,_tmp338);};}_LL146: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1E3->tag != 6)goto _LL148;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}}_LL147: {
# 1450
struct _tuple19 _tmp339=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E4);union Cyc_CfFlowInfo_FlowInfo _tmp33B;union Cyc_CfFlowInfo_FlowInfo _tmp33C;struct _tuple19 _tmp33A=_tmp339;_tmp33B=_tmp33A.f1;_tmp33C=_tmp33A.f2;{
struct _tuple16 _tmp33D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33B,_tmp1E5);union Cyc_CfFlowInfo_FlowInfo _tmp33F;void*_tmp340;struct _tuple16 _tmp33E=_tmp33D;_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;{
struct _tuple16 _tmp854;struct _tuple16 _tmp341=(_tmp854.f1=_tmp33F,((_tmp854.f2=_tmp340,_tmp854)));
struct _tuple16 _tmp855;struct _tuple16 _tmp342=(_tmp855.f1=_tmp33C,((_tmp855.f2=_tmp17B->zero,_tmp855)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17B,env->all_changed,_tmp341,_tmp342);};};}_LL148: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1E6->tag != 7)goto _LL14A;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;}}_LL149: {
# 1457
struct _tuple19 _tmp345=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E7);union Cyc_CfFlowInfo_FlowInfo _tmp347;union Cyc_CfFlowInfo_FlowInfo _tmp348;struct _tuple19 _tmp346=_tmp345;_tmp347=_tmp346.f1;_tmp348=_tmp346.f2;{
struct _tuple16 _tmp349=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp348,_tmp1E8);union Cyc_CfFlowInfo_FlowInfo _tmp34B;void*_tmp34C;struct _tuple16 _tmp34A=_tmp349;_tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;{
struct _tuple16 _tmp856;struct _tuple16 _tmp34D=(_tmp856.f1=_tmp34B,((_tmp856.f2=_tmp34C,_tmp856)));
struct _tuple16 _tmp857;struct _tuple16 _tmp34E=(_tmp857.f1=_tmp347,((_tmp857.f2=_tmp17B->notzeroall,_tmp857)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17B,env->all_changed,_tmp34D,_tmp34E);};};}_LL14A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1E9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1E9->tag != 22)goto _LL14C;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=_tmp1E9->f2;}}_LL14B: {
# 1464
struct _RegionHandle _tmp351=_new_region("temp");struct _RegionHandle*temp=& _tmp351;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp858[2];struct _tuple23 _tmp352=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp858[1]=_tmp1EB,((_tmp858[0]=_tmp1EA,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp858,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1465
union Cyc_CfFlowInfo_FlowInfo _tmp354;struct Cyc_List_List*_tmp355;struct _tuple23 _tmp353=_tmp352;_tmp354=_tmp353.f1;_tmp355=_tmp353.f2;{
# 1469
union Cyc_CfFlowInfo_FlowInfo _tmp356=_tmp354;
{union Cyc_CfFlowInfo_FlowInfo _tmp357=_tmp354;struct Cyc_Dict_Dict _tmp358;struct Cyc_List_List*_tmp359;_LL1D1: if((_tmp357.ReachableFL).tag != 2)goto _LL1D3;_tmp358=((struct _tuple14)(_tmp357.ReachableFL).val).f1;_tmp359=((struct _tuple14)(_tmp357.ReachableFL).val).f2;_LL1D2:
# 1474
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp358,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp355))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp85B;void*_tmp85A;(_tmp85A=0,Cyc_CfFlowInfo_aerr(_tmp1EB->loc,((_tmp85B="expression may not be initialized",_tag_dyneither(_tmp85B,sizeof(char),34))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp35C=Cyc_NewControlFlow_add_subscript_reln(_tmp17B->r,_tmp359,_tmp1EA,_tmp1EB);
if(_tmp359 != _tmp35C)
_tmp356=Cyc_CfFlowInfo_ReachableFL(_tmp358,_tmp35C);
goto _LL1D0;};_LL1D3:;_LL1D4:
 goto _LL1D0;_LL1D0:;}{
# 1482
void*_tmp35D=Cyc_Tcutil_compress((void*)_check_null(_tmp1EA->topt));void*_tmp35E=_tmp35D;struct Cyc_List_List*_tmp360;union Cyc_Absyn_Constraint*_tmp362;_LL1D6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35E;if(_tmp35F->tag != 10)goto _LL1D8;else{_tmp360=_tmp35F->f1;}}_LL1D7: {
# 1484
struct _tuple15 _tmp363=Cyc_CfFlowInfo_unname_rval(_tmp17B->r,(void*)((struct Cyc_List_List*)_check_null(_tmp355))->hd);void*_tmp365;struct _tuple15 _tmp364=_tmp363;_tmp365=_tmp364.f1;{
void*_tmp366=_tmp365;struct _dyneither_ptr _tmp368;_LL1DD: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp367=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp366;if(_tmp367->tag != 6)goto _LL1DF;else{_tmp368=_tmp367->f2;}}_LL1DE: {
# 1487
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1EB)).f1;
struct _tuple16 _tmp85C;struct _tuple16 _tmp36A=(_tmp85C.f1=_tmp356,((_tmp85C.f2=*((void**)_check_dyneither_subscript(_tmp368,sizeof(void*),(int)i)),_tmp85C)));_npop_handler(0);return _tmp36A;}_LL1DF:;_LL1E0: {
struct Cyc_Core_Impossible_exn_struct _tmp862;const char*_tmp861;struct Cyc_Core_Impossible_exn_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp862.tag=Cyc_Core_Impossible,((_tmp862.f1=((_tmp861="anal_Rexp: Subscript",_tag_dyneither(_tmp861,sizeof(char),21))),_tmp862)))),_tmp860)))));}_LL1DC:;};}_LL1D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E;if(_tmp361->tag != 5)goto _LL1DA;else{_tmp362=((_tmp361->f1).ptr_atts).bounds;}}_LL1D9: {
# 1492
struct _tuple16 _tmp36E=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp354,_tmp1EA,(void*)((struct Cyc_List_List*)_check_null(_tmp355))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp370;void*_tmp371;struct _tuple16 _tmp36F=_tmp36E;_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp372=_tmp370;_LL1E2: if((_tmp372.BottomFL).tag != 1)goto _LL1E4;_LL1E3: {
struct _tuple16 _tmp863;struct _tuple16 _tmp374=(_tmp863.f1=_tmp370,((_tmp863.f2=_tmp371,_tmp863)));_npop_handler(0);return _tmp374;}_LL1E4:;_LL1E5: {
struct _tuple16 _tmp864;struct _tuple16 _tmp376=(_tmp864.f1=_tmp356,((_tmp864.f2=_tmp371,_tmp864)));_npop_handler(0);return _tmp376;}_LL1E1:;};}_LL1DA:;_LL1DB: {
# 1497
struct Cyc_Core_Impossible_exn_struct _tmp86A;const char*_tmp869;struct Cyc_Core_Impossible_exn_struct*_tmp868;(int)_throw((void*)((_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((_tmp86A.tag=Cyc_Core_Impossible,((_tmp86A.f1=((_tmp869="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp869,sizeof(char),33))),_tmp86A)))),_tmp868)))));}_LL1D5:;};};}
# 1465
;_pop_region(temp);}_LL14C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1EC=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1EC->tag != 30)goto _LL14E;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LL14D:
# 1502
 _tmp1F0=_tmp1ED;goto _LL14F;_LL14E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1EF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1EF->tag != 23)goto _LL150;else{_tmp1F0=_tmp1EF->f1;}}_LL14F: {
# 1504
struct _RegionHandle _tmp37B=_new_region("temp");struct _RegionHandle*temp=& _tmp37B;_push_region(temp);
{struct _tuple23 _tmp37C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1F0,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp37E;struct Cyc_List_List*_tmp37F;struct _tuple23 _tmp37D=_tmp37C;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;{
unsigned int _tmp870;unsigned int _tmp86F;struct _dyneither_ptr _tmp86E;void**_tmp86D;unsigned int _tmp86C;struct _dyneither_ptr aggrdict=
(_tmp86C=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1F0),((_tmp86D=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp86C)),((_tmp86E=_tag_dyneither(_tmp86D,sizeof(void*),_tmp86C),((((_tmp86F=_tmp86C,_tmp871(& _tmp870,& _tmp86F,& _tmp86D,& _tmp37F))),_tmp86E)))))));
# 1512
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp87B;struct Cyc_CfFlowInfo_UnionRInfo _tmp87A;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp879;struct _tuple16 _tmp878;struct _tuple16 _tmp384=(_tmp878.f1=_tmp37E,((_tmp878.f2=(void*)((_tmp87B=_region_malloc(env->r,sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp879.tag=6,((_tmp879.f1=((_tmp87A.is_union=0,((_tmp87A.fieldnum=- 1,_tmp87A)))),((_tmp879.f2=aggrdict,_tmp879)))))),_tmp87B)))),_tmp878)));_npop_handler(0);return _tmp384;};}
# 1505
;_pop_region(temp);}_LL150: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1F1=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1F1->tag != 29)goto _LL152;else{_tmp1F2=_tmp1F1->f2;}}_LL151: {
# 1515
struct Cyc_List_List*fs;
{void*_tmp38A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp38B=_tmp38A;struct Cyc_List_List*_tmp38D;_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B;if(_tmp38C->tag != 12)goto _LL1E9;else{_tmp38D=_tmp38C->f2;}}_LL1E8:
 fs=_tmp38D;goto _LL1E6;_LL1E9:;_LL1EA: {
struct Cyc_Core_Impossible_exn_struct _tmp881;const char*_tmp880;struct Cyc_Core_Impossible_exn_struct*_tmp87F;(int)_throw((void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((_tmp881.tag=Cyc_Core_Impossible,((_tmp881.f1=((_tmp880="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp880,sizeof(char),35))),_tmp881)))),_tmp87F)))));}_LL1E6:;}
# 1520
_tmp1F4=_tmp1F2;_tmp1F5=Cyc_Absyn_StructA;_tmp1F6=fs;goto _LL153;}_LL152: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1F3->tag != 28)goto _LL154;else{_tmp1F4=_tmp1F3->f3;if(_tmp1F3->f4 == 0)goto _LL154;_tmp1F5=(_tmp1F3->f4)->kind;if((_tmp1F3->f4)->impl == 0)goto _LL154;_tmp1F6=((_tmp1F3->f4)->impl)->fields;}}_LL153: {
# 1522
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp391=_new_region("temp");struct _RegionHandle*temp=& _tmp391;_push_region(temp);
{struct _tuple23 _tmp392=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1F4),1,1);
# 1524
union Cyc_CfFlowInfo_FlowInfo _tmp394;struct Cyc_List_List*_tmp395;struct _tuple23 _tmp393=_tmp392;_tmp394=_tmp393.f1;_tmp395=_tmp393.f2;{
# 1526
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp17B,_tmp1F6,_tmp1F5 == Cyc_Absyn_UnionA,_tmp17B->unknown_all);
# 1529
int field_no=-1;
{int i=0;for(0;_tmp395 != 0;(((_tmp395=_tmp395->tl,_tmp1F4=_tmp1F4->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp1F4))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp396=(void*)ds->hd;void*_tmp397=_tmp396;struct _dyneither_ptr*_tmp39A;_LL1EC: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp398=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp397;if(_tmp398->tag != 0)goto _LL1EE;}_LL1ED: {
struct Cyc_Core_Impossible_exn_struct _tmp887;const char*_tmp886;struct Cyc_Core_Impossible_exn_struct*_tmp885;(int)_throw((void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp887.tag=Cyc_Core_Impossible,((_tmp887.f1=((_tmp886="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp886,sizeof(char),22))),_tmp887)))),_tmp885)))));}_LL1EE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp399=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp397;if(_tmp399->tag != 1)goto _LL1EB;else{_tmp39A=_tmp399->f1;}}_LL1EF:
# 1536
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1F6,_tmp39A);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp395->hd;
# 1539
if(_tmp1F5 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp39E=(*((struct _tuple26*)_tmp1F4->hd)).f2;
_tmp394=Cyc_NewControlFlow_use_Rval(env,_tmp39E->loc,_tmp394,(void*)_tmp395->hd);
# 1543
Cyc_NewControlFlow_check_requires(_tmp39E->loc,_tmp17B->r,exp_type,_tmp39A,_tmp394);}_LL1EB:;}}}{
# 1546
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp88D;struct Cyc_CfFlowInfo_UnionRInfo _tmp88C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp88B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp39F=(_tmp88B=_region_malloc(env->r,sizeof(*_tmp88B)),((_tmp88B[0]=((_tmp88D.tag=6,((_tmp88D.f1=((_tmp88C.is_union=_tmp1F5 == Cyc_Absyn_UnionA,((_tmp88C.fieldnum=field_no,_tmp88C)))),((_tmp88D.f2=aggrdict,_tmp88D)))))),_tmp88B)));
struct _tuple16 _tmp88E;struct _tuple16 _tmp3A1=(_tmp88E.f1=_tmp394,((_tmp88E.f2=(void*)_tmp39F,_tmp88E)));_npop_handler(0);return _tmp3A1;};};}
# 1524
;_pop_region(temp);}_LL154: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1F7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1F7->tag != 28)goto _LL156;}_LL155: {
# 1550
struct Cyc_Core_Impossible_exn_struct _tmp894;const char*_tmp893;struct Cyc_Core_Impossible_exn_struct*_tmp892;(int)_throw((void*)((_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892[0]=((_tmp894.tag=Cyc_Core_Impossible,((_tmp894.f1=((_tmp893="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp893,sizeof(char),31))),_tmp894)))),_tmp892)))));}_LL156: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1F8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1F8->tag != 25)goto _LL158;else{_tmp1F9=_tmp1F8->f1;}}_LL157: {
# 1552
struct _RegionHandle _tmp3A8=_new_region("temp");struct _RegionHandle*temp=& _tmp3A8;_push_region(temp);
{struct Cyc_List_List*_tmp3A9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1F9);
struct _tuple23 _tmp3AA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3A9,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp3AC;struct Cyc_List_List*_tmp3AD;struct _tuple23 _tmp3AB=_tmp3AA;_tmp3AC=_tmp3AB.f1;_tmp3AD=_tmp3AB.f2;
for(0;_tmp3AD != 0;(_tmp3AD=_tmp3AD->tl,_tmp3A9=_tmp3A9->tl)){
_tmp3AC=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3A9))->hd)->loc,_tmp3AC,(void*)_tmp3AD->hd);}{
struct _tuple16 _tmp895;struct _tuple16 _tmp3AF=(_tmp895.f1=_tmp3AC,((_tmp895.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,(void*)_check_null(e->topt),0,_tmp17B->unknown_all),_tmp895)));_npop_handler(0);return _tmp3AF;};}
# 1553
;_pop_region(temp);}_LL158: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1FA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1FA->tag != 26)goto _LL15A;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;_tmp1FD=_tmp1FA->f3;_tmp1FE=_tmp1FA->f4;}}_LL159: {
# 1561
struct _tuple16 _tmp3B0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1FC);union Cyc_CfFlowInfo_FlowInfo _tmp3B2;void*_tmp3B3;struct _tuple16 _tmp3B1=_tmp3B0;_tmp3B2=_tmp3B1.f1;_tmp3B3=_tmp3B1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3B4=_tmp3B2;struct Cyc_Dict_Dict _tmp3B5;struct Cyc_List_List*_tmp3B6;_LL1F1: if((_tmp3B4.BottomFL).tag != 1)goto _LL1F3;_LL1F2: {
struct _tuple16 _tmp896;return(_tmp896.f1=_tmp3B2,((_tmp896.f2=_tmp17B->unknown_all,_tmp896)));}_LL1F3: if((_tmp3B4.ReachableFL).tag != 2)goto _LL1F0;_tmp3B5=((struct _tuple14)(_tmp3B4.ReachableFL).val).f1;_tmp3B6=((struct _tuple14)(_tmp3B4.ReachableFL).val).f2;_LL1F4:
# 1565
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3B5,_tmp3B3)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp899;void*_tmp898;(_tmp898=0,Cyc_CfFlowInfo_aerr(_tmp1FC->loc,((_tmp899="expression may not be initialized",_tag_dyneither(_tmp899,sizeof(char),34))),_tag_dyneither(_tmp898,sizeof(void*),0)));}{
# 1569
struct Cyc_List_List*new_relns=_tmp3B6;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1FB);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp1FC,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3B6);
# 1575
if(_tmp3B6 != new_relns)
_tmp3B2=Cyc_CfFlowInfo_ReachableFL(_tmp3B5,new_relns);{
# 1579
void*_tmp3BA=_tmp3B3;_LL1F6: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3BB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3BA;if(_tmp3BB->tag != 0)goto _LL1F8;}_LL1F7: {
struct _tuple16 _tmp89A;return(_tmp89A.f1=_tmp3B2,((_tmp89A.f2=_tmp17B->unknown_all,_tmp89A)));}_LL1F8: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3BC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3BA;if(_tmp3BC->tag != 2)goto _LL1FA;}_LL1F9:
 goto _LL1FB;_LL1FA: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3BD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3BA;if(_tmp3BD->tag != 1)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3BE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3BA;if(_tmp3BE->tag != 5)goto _LL1FE;}_LL1FD: {
# 1584
struct Cyc_List_List _tmp89B;struct Cyc_List_List _tmp3C0=(_tmp89B.hd=_tmp1FB,((_tmp89B.tl=0,_tmp89B)));
_tmp3B2=Cyc_NewControlFlow_add_vars(_tmp17B,_tmp3B2,& _tmp3C0,_tmp17B->unknown_all,e->loc,0);{
# 1587
struct _tuple16 _tmp3C1=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B2,_tmp1FD);union Cyc_CfFlowInfo_FlowInfo _tmp3C3;void*_tmp3C4;struct _tuple16 _tmp3C2=_tmp3C1;_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C5=_tmp3C3;struct Cyc_Dict_Dict _tmp3C6;_LL201: if((_tmp3C5.BottomFL).tag != 1)goto _LL203;_LL202: {
struct _tuple16 _tmp89C;return(_tmp89C.f1=_tmp3C3,((_tmp89C.f2=_tmp17B->unknown_all,_tmp89C)));}_LL203: if((_tmp3C5.ReachableFL).tag != 2)goto _LL200;_tmp3C6=((struct _tuple14)(_tmp3C5.ReachableFL).val).f1;_LL204:
# 1591
 if(Cyc_CfFlowInfo_initlevel(_tmp17B,_tmp3C6,_tmp3C4)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp89F;void*_tmp89E;(_tmp89E=0,Cyc_CfFlowInfo_aerr(_tmp1FC->loc,((_tmp89F="expression may not be initialized",_tag_dyneither(_tmp89F,sizeof(char),34))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}{
struct _tuple16 _tmp8A0;return(_tmp8A0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A0.f2=_tmp17B->unknown_all,_tmp8A0)));};}_LL200:;}
# 1596
_tmp3B2=_tmp3C3;
goto _LL1FF;};}_LL1FE:;_LL1FF:
# 1599
 while(1){
struct Cyc_List_List _tmp8A1;struct Cyc_List_List _tmp3CC=(_tmp8A1.hd=_tmp1FB,((_tmp8A1.tl=0,_tmp8A1)));
_tmp3B2=Cyc_NewControlFlow_add_vars(_tmp17B,_tmp3B2,& _tmp3CC,_tmp17B->unknown_all,e->loc,0);{
struct _tuple16 _tmp3CD=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B2,_tmp1FD);union Cyc_CfFlowInfo_FlowInfo _tmp3CF;void*_tmp3D0;struct _tuple16 _tmp3CE=_tmp3CD;_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3D1=_tmp3CF;struct Cyc_Dict_Dict _tmp3D2;_LL206: if((_tmp3D1.BottomFL).tag != 1)goto _LL208;_LL207:
 goto _LL205;_LL208: if((_tmp3D1.ReachableFL).tag != 2)goto _LL205;_tmp3D2=((struct _tuple14)(_tmp3D1.ReachableFL).val).f1;_LL209:
# 1606
 if(Cyc_CfFlowInfo_initlevel(_tmp17B,_tmp3D2,_tmp3D0)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8A4;void*_tmp8A3;(_tmp8A3=0,Cyc_CfFlowInfo_aerr(_tmp1FC->loc,((_tmp8A4="expression may not be initialized",_tag_dyneither(_tmp8A4,sizeof(char),34))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}{
struct _tuple16 _tmp8A5;return(_tmp8A5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A5.f2=_tmp17B->unknown_all,_tmp8A5)));};}_LL205:;}{
# 1611
union Cyc_CfFlowInfo_FlowInfo _tmp3D6=Cyc_CfFlowInfo_join_flow(_tmp17B,env->all_changed,_tmp3B2,_tmp3CF);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3D6,_tmp3B2))
break;
_tmp3B2=_tmp3D6;};};}{
# 1616
struct _tuple16 _tmp8A6;return(_tmp8A6.f1=_tmp3B2,((_tmp8A6.f2=_tmp17B->unknown_all,_tmp8A6)));};_LL1F5:;};};_LL1F0:;};}_LL15A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp1FF=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp1FF->tag != 27)goto _LL15C;else{_tmp200=_tmp1FF->f1;_tmp201=(void*)_tmp1FF->f2;_tmp202=_tmp1FF->f3;}}_LL15B: {
# 1622
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8A9;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8A8;void*root=(void*)((_tmp8A8=_region_malloc(_tmp17B->r,sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=1,((_tmp8A9.f1=_tmp200,((_tmp8A9.f2=(void*)_check_null(e->topt),_tmp8A9)))))),_tmp8A8))));
struct Cyc_CfFlowInfo_Place*_tmp8AA;struct Cyc_CfFlowInfo_Place*place=(_tmp8AA=_region_malloc(_tmp17B->r,sizeof(*_tmp8AA)),((_tmp8AA->root=root,((_tmp8AA->fields=0,_tmp8AA)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8AD;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8AC;void*rval=(void*)((_tmp8AC=_region_malloc(_tmp17B->r,sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AD.tag=5,((_tmp8AD.f1=place,_tmp8AD)))),_tmp8AC))));
void*place_val;
# 1630
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17B,_tmp201,0,_tmp17B->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp3D9=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp200);union Cyc_CfFlowInfo_FlowInfo _tmp3DB;struct _tuple16 _tmp3DA=_tmp3D9;_tmp3DB=_tmp3DA.f1;
outflow=_tmp3DB;{
union Cyc_CfFlowInfo_FlowInfo _tmp3DC=outflow;struct Cyc_Dict_Dict _tmp3DD;struct Cyc_List_List*_tmp3DE;_LL20B: if((_tmp3DC.BottomFL).tag != 1)goto _LL20D;_LL20C: {
struct _tuple16 _tmp8AE;return(_tmp8AE.f1=outflow,((_tmp8AE.f2=rval,_tmp8AE)));}_LL20D: if((_tmp3DC.ReachableFL).tag != 2)goto _LL20A;_tmp3DD=((struct _tuple14)(_tmp3DC.ReachableFL).val).f1;_tmp3DE=((struct _tuple14)(_tmp3DC.ReachableFL).val).f2;_LL20E: {
# 1638
struct _tuple16 _tmp8AF;return(_tmp8AF.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3DD,root,place_val),_tmp3DE),((_tmp8AF.f2=rval,_tmp8AF)));}_LL20A:;};};};}_LL15C: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp203=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp203->tag != 36)goto _LL15E;else{_tmp204=_tmp203->f1;}}_LL15D: {
# 1642
struct _tuple17 _tmp8B0;struct _tuple17 _tmp3E6=(_tmp8B0.f1=(env->fenv)->unknown_all,((_tmp8B0.f2=copy_ctxt,_tmp8B0)));
union Cyc_CfFlowInfo_FlowInfo _tmp3E7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp204,& _tmp3E6);
struct _tuple16 _tmp8B1;return(_tmp8B1.f1=_tmp3E7,((_tmp8B1.f2=_tmp3E6.f1,_tmp8B1)));}_LL15E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp205=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp205->tag != 1)goto _LL160;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp206=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp205->f2);if(_tmp206->tag != 0)goto _LL160;}}_LL15F:
# 1646
 goto _LL161;_LL160: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp207=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp207->tag != 35)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp208=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp208->tag != 24)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp209=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp209->tag != 38)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp20A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp193;if(_tmp20A->tag != 39)goto _LLFF;}_LL167: {
# 1651
struct Cyc_Core_Impossible_exn_struct _tmp8B7;const char*_tmp8B6;struct Cyc_Core_Impossible_exn_struct*_tmp8B5;(int)_throw((void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B7.tag=Cyc_Core_Impossible,((_tmp8B7.f1=((_tmp8B6="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8B6,sizeof(char),31))),_tmp8B7)))),_tmp8B5)))));}_LLFF:;};}
# 1661
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,struct Cyc_List_List*flds){
# 1669
struct Cyc_CfFlowInfo_FlowEnv*_tmp3ED=env->fenv;
void*_tmp3EE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3EF=_tmp3EE;void*_tmp3F1;union Cyc_Absyn_Constraint*_tmp3F2;union Cyc_Absyn_Constraint*_tmp3F3;_LL210: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto _LL212;else{_tmp3F1=(_tmp3F0->f1).elt_typ;_tmp3F2=((_tmp3F0->f1).ptr_atts).bounds;_tmp3F3=((_tmp3F0->f1).ptr_atts).zero_term;}}_LL211: {
# 1672
union Cyc_CfFlowInfo_FlowInfo _tmp3F4=f;struct Cyc_Dict_Dict _tmp3F5;struct Cyc_List_List*_tmp3F6;_LL215: if((_tmp3F4.BottomFL).tag != 1)goto _LL217;_LL216: {
struct _tuple18 _tmp8B8;return(_tmp8B8.f1=f,((_tmp8B8.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8B8)));}_LL217: if((_tmp3F4.ReachableFL).tag != 2)goto _LL214;_tmp3F5=((struct _tuple14)(_tmp3F4.ReachableFL).val).f1;_tmp3F6=((struct _tuple14)(_tmp3F4.ReachableFL).val).f2;_LL218:
# 1676
 if(Cyc_Tcutil_is_bound_one(_tmp3F2)){
struct _tuple15 _tmp3F8=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp3FA;struct Cyc_List_List*_tmp3FB;struct _tuple15 _tmp3F9=_tmp3F8;_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;
retry: {void*_tmp3FC=_tmp3FA;void*_tmp401;struct Cyc_List_List*_tmp402;enum Cyc_CfFlowInfo_InitLevel _tmp405;_LL21A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3FD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp3FD->tag != 8)goto _LL21C;}_LL21B: {
# 1680
struct Cyc_Core_Impossible_exn_struct _tmp8BE;const char*_tmp8BD;struct Cyc_Core_Impossible_exn_struct*_tmp8BC;(int)_throw((void*)((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BE.tag=Cyc_Core_Impossible,((_tmp8BE.f1=((_tmp8BD="named location in anal_derefL",_tag_dyneither(_tmp8BD,sizeof(char),30))),_tmp8BE)))),_tmp8BC)))));}_LL21C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp3FE->tag != 1)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3FF=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp3FF->tag != 2)goto _LL220;}_LL21F:
# 1683
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8C1;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8C0;e->annot=(void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=Cyc_CfFlowInfo_NotZero,((_tmp8C1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3F6),_tmp8C1)))),_tmp8C0))));}goto _LL219;_LL220: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp400=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp400->tag != 5)goto _LL222;else{_tmp401=(_tmp400->f1)->root;_tmp402=(_tmp400->f1)->fields;}}_LL221:
# 1685
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8C4;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8C3;e->annot=(void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C4.tag=Cyc_CfFlowInfo_NotZero,((_tmp8C4.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3F6),_tmp8C4)))),_tmp8C3))));}{
struct Cyc_CfFlowInfo_Place*_tmp8C7;struct _tuple18 _tmp8C6;return(_tmp8C6.f1=f,((_tmp8C6.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8C7=_region_malloc(_tmp3ED->r,sizeof(*_tmp8C7)),((_tmp8C7->root=_tmp401,((_tmp8C7->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3ED->r,_tmp402,flds),_tmp8C7))))))),_tmp8C6)));};_LL222: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp403=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp403->tag != 0)goto _LL224;}_LL223:
# 1688
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp8C8;return(_tmp8C8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8C8.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C8)));};_LL224: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp404=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3FC;if(_tmp404->tag != 3)goto _LL226;else{_tmp405=_tmp404->f1;}}_LL225:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp405,_tmp3FB);goto _LL227;_LL226:;_LL227:
# 1693
 if(passthrough_consumes){
void*_tmp410=_tmp3FA;void*_tmp412;_LL229: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp411=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp410;if(_tmp411->tag != 7)goto _LL22B;else{_tmp412=(void*)_tmp411->f3;}}_LL22A:
 _tmp3FA=_tmp412;goto retry;_LL22B:;_LL22C:
 goto _LL228;_LL228:;}{
# 1699
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8CB;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8CA;e->annot=(void*)((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA[0]=((_tmp8CB.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8CB.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3F6),_tmp8CB)))),_tmp8CA))));};_LL219:;}}else{
# 1703
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8CE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8CD;e->annot=(void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8CE.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8CE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3F6),_tmp8CE)))),_tmp8CD))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3ED,_tmp3F5,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8D1="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp8D1,sizeof(char),46))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}{
struct _tuple18 _tmp8D2;return(_tmp8D2.f1=f,((_tmp8D2.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8D2)));};_LL214:;}_LL212:;_LL213: {
# 1708
struct Cyc_Core_Impossible_exn_struct _tmp8D8;const char*_tmp8D7;struct Cyc_Core_Impossible_exn_struct*_tmp8D6;(int)_throw((void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D8.tag=Cyc_Core_Impossible,((_tmp8D8.f1=((_tmp8D7="left deref of non-pointer-type",_tag_dyneither(_tmp8D7,sizeof(char),31))),_tmp8D8)))),_tmp8D6)))));}_LL20F:;}
# 1716
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1719
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp41D=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp41E=inflow;struct Cyc_Dict_Dict _tmp41F;struct Cyc_List_List*_tmp420;_LL22E: if((_tmp41E.BottomFL).tag != 1)goto _LL230;_LL22F: {
struct _tuple18 _tmp8D9;return(_tmp8D9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8D9)));}_LL230: if((_tmp41E.ReachableFL).tag != 2)goto _LL22D;_tmp41F=((struct _tuple14)(_tmp41E.ReachableFL).val).f1;_tmp420=((struct _tuple14)(_tmp41E.ReachableFL).val).f2;_LL231:
# 1724
 d=_tmp41F;_LL22D:;}{
# 1728
void*_tmp422=e->r;void*_tmp423=_tmp422;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Vardecl*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Vardecl*_tmp432;struct Cyc_Absyn_Vardecl*_tmp435;struct Cyc_Absyn_Exp*_tmp437;struct _dyneither_ptr*_tmp438;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*_tmp43F;struct _dyneither_ptr*_tmp440;_LL233: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp424=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp424->tag != 13)goto _LL235;else{_tmp425=_tmp424->f2;}}_LL234:
 _tmp427=_tmp425;goto _LL236;_LL235: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp426=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp426->tag != 11)goto _LL237;else{_tmp427=_tmp426->f1;}}_LL236:
 _tmp429=_tmp427;goto _LL238;_LL237: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp428=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp428->tag != 12)goto _LL239;else{_tmp429=_tmp428->f1;}}_LL238:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp429,flds);_LL239: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp42A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp42A->tag != 1)goto _LL23B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp42B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp42A->f2);if(_tmp42B->tag != 3)goto _LL23B;else{_tmp42C=_tmp42B->f1;}}}_LL23A:
# 1733
 _tmp42F=_tmp42C;goto _LL23C;_LL23B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp42D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp42D->tag != 1)goto _LL23D;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp42D->f2);if(_tmp42E->tag != 4)goto _LL23D;else{_tmp42F=_tmp42E->f1;}}}_LL23C:
 _tmp432=_tmp42F;goto _LL23E;_LL23D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp430=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp430->tag != 1)goto _LL23F;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp431=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp430->f2);if(_tmp431->tag != 5)goto _LL23F;else{_tmp432=_tmp431->f1;}}}_LL23E: {
# 1736
struct Cyc_CfFlowInfo_Place*_tmp8E3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8E2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8E1;struct _tuple18 _tmp8E0;return(_tmp8E0.f1=inflow,((_tmp8E0.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8E3=_region_malloc(env->r,sizeof(*_tmp8E3)),((_tmp8E3->root=(void*)((_tmp8E1=_region_malloc(env->r,sizeof(*_tmp8E1)),((_tmp8E1[0]=((_tmp8E2.tag=0,((_tmp8E2.f1=_tmp432,_tmp8E2)))),_tmp8E1)))),((_tmp8E3->fields=flds,_tmp8E3))))))),_tmp8E0)));}_LL23F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp433=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp433->tag != 1)goto _LL241;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp434=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp433->f2);if(_tmp434->tag != 1)goto _LL241;else{_tmp435=_tmp434->f1;}}}_LL240: {
# 1738
struct _tuple18 _tmp8E4;return(_tmp8E4.f1=inflow,((_tmp8E4.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8E4)));}_LL241: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp436->tag != 21)goto _LL243;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}_LL242:
# 1741
{void*_tmp446=Cyc_Tcutil_compress((void*)_check_null(_tmp437->topt));void*_tmp447=_tmp446;void*_tmp449;_LL24C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447;if(_tmp448->tag != 5)goto _LL24E;else{_tmp449=(_tmp448->f1).elt_typ;}}_LL24D:
# 1743
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp449)){
Cyc_NewControlFlow_check_requires(_tmp437->loc,_tmp41D->r,_tmp449,_tmp438,inflow);{
struct Cyc_List_List*_tmp8E5;flds=((_tmp8E5=_region_malloc(env->r,sizeof(*_tmp8E5)),((_tmp8E5->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp449,_tmp438),((_tmp8E5->tl=flds,_tmp8E5))))));};}
# 1747
goto _LL24B;_LL24E:;_LL24F: {
struct Cyc_Core_Impossible_exn_struct _tmp8EB;const char*_tmp8EA;struct Cyc_Core_Impossible_exn_struct*_tmp8E9;(int)_throw((void*)((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EB.tag=Cyc_Core_Impossible,((_tmp8EB.f1=((_tmp8EA="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp8EA,sizeof(char),25))),_tmp8EB)))),_tmp8E9)))));}_LL24B:;}
# 1750
_tmp43A=_tmp437;goto _LL244;_LL243: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp439->tag != 19)goto _LL245;else{_tmp43A=_tmp439->f1;}}_LL244:
# 1753
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp43A->topt),1)){
# 1755
struct _tuple18 _tmp44E=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp43A->topt),1),passthrough_consumes,_tmp43A);
# 1755
union Cyc_CfFlowInfo_FlowInfo _tmp450;union Cyc_CfFlowInfo_AbsLVal _tmp451;struct _tuple18 _tmp44F=_tmp44E;_tmp450=_tmp44F.f1;_tmp451=_tmp44F.f2;{
# 1759
struct _tuple18 _tmp8EC;struct _tuple18 _tmp452=(_tmp8EC.f1=_tmp450,((_tmp8EC.f2=_tmp451,_tmp8EC)));struct _tuple18 _tmp453=_tmp452;struct Cyc_Dict_Dict _tmp454;struct Cyc_List_List*_tmp455;struct Cyc_CfFlowInfo_Place*_tmp456;_LL251: if(((_tmp453.f1).ReachableFL).tag != 2)goto _LL253;_tmp454=((struct _tuple14)((_tmp453.f1).ReachableFL).val).f1;_tmp455=((struct _tuple14)((_tmp453.f1).ReachableFL).val).f2;if(((_tmp453.f2).PlaceL).tag != 1)goto _LL253;_tmp456=(struct Cyc_CfFlowInfo_Place*)((_tmp453.f2).PlaceL).val;_LL252: {
# 1761
struct _tuple15 _tmp457=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,Cyc_CfFlowInfo_lookup_place(_tmp454,_tmp456));void*_tmp459;struct Cyc_List_List*_tmp45A;struct _tuple15 _tmp458=_tmp457;_tmp459=_tmp458.f1;_tmp45A=_tmp458.f2;{
void*_tmp45B=_tmp459;struct Cyc_Absyn_Vardecl*_tmp45D;void*_tmp45E;_LL256: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp45C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp45B;if(_tmp45C->tag != 8)goto _LL258;else{_tmp45D=_tmp45C->f1;_tmp45E=(void*)_tmp45C->f2;}}_LL257: {
# 1764
struct Cyc_Core_Impossible_exn_struct _tmp8F2;const char*_tmp8F1;struct Cyc_Core_Impossible_exn_struct*_tmp8F0;(int)_throw((void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F2.tag=Cyc_Core_Impossible,((_tmp8F2.f1=((_tmp8F1="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp8F1,sizeof(char),38))),_tmp8F2)))),_tmp8F0)))));}_LL258: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp45F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp45B;if(_tmp45F->tag != 7)goto _LL25A;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp460=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp45F->f3);if(_tmp460->tag != 5)goto _LL25A;}}_LL259:
 goto _LL25B;_LL25A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp461=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp45B;if(_tmp461->tag != 5)goto _LL25C;}_LL25B:
 return Cyc_NewControlFlow_anal_derefL(env,_tmp450,_tmp43A,_tmp450,_tmp459,passthrough_consumes,flds);_LL25C:;_LL25D: {
# 1773
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp41D,_tmp454,_tmp459);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp41D->unknown_all: _tmp41D->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp41D,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp43A->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8F5;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8F4;void*new_root=(void*)((_tmp8F4=_region_malloc(_tmp41D->r,sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=1,((_tmp8F5.f1=e,((_tmp8F5.f2=(void*)_check_null(e->topt),_tmp8F5)))))),_tmp8F4))));
struct Cyc_CfFlowInfo_Place*_tmp8F6;struct Cyc_CfFlowInfo_Place*place=(_tmp8F6=_region_malloc(_tmp41D->r,sizeof(*_tmp8F6)),((_tmp8F6->root=new_root,((_tmp8F6->fields=0,_tmp8F6)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8F9;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8F8;void*res=(void*)((_tmp8F8=_region_malloc(_tmp41D->r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=5,((_tmp8F9.f1=place,_tmp8F9)))),_tmp8F8))));
for(0;_tmp45A != 0;_tmp45A=_tmp45A->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8FC;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8FB;res=(void*)((_tmp8FB=_region_malloc(_tmp41D->r,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=8,((_tmp8FC.f1=(struct Cyc_Absyn_Vardecl*)_tmp45A->hd,((_tmp8FC.f2=res,_tmp8FC)))))),_tmp8FB))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp454=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp454,new_root,new_rval);
_tmp454=Cyc_CfFlowInfo_assign_place(_tmp41D,e->loc,_tmp454,env->all_changed,_tmp456,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp467=Cyc_CfFlowInfo_ReachableFL(_tmp454,_tmp455);
# 1788
return Cyc_NewControlFlow_anal_derefL(env,_tmp467,_tmp43A,_tmp467,res,passthrough_consumes,flds);};}_LL255:;};}_LL253:;_LL254:
# 1790
 goto _LL250;_LL250:;};}{
# 1793
struct _tuple16 _tmp46E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43A);union Cyc_CfFlowInfo_FlowInfo _tmp470;void*_tmp471;struct _tuple16 _tmp46F=_tmp46E;_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43A,_tmp470,_tmp471,passthrough_consumes,flds);};_LL245: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp43B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp43B->tag != 22)goto _LL247;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}_LL246: {
# 1797
void*_tmp472=Cyc_Tcutil_compress((void*)_check_null(_tmp43C->topt));void*_tmp473=_tmp472;union Cyc_Absyn_Constraint*_tmp476;_LL25F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp473;if(_tmp474->tag != 10)goto _LL261;}_LL260: {
# 1799
unsigned int _tmp477=(Cyc_Evexp_eval_const_uint_exp(_tmp43D)).f1;
struct Cyc_List_List*_tmp8FD;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp43C,((_tmp8FD=_region_malloc(env->r,sizeof(*_tmp8FD)),((_tmp8FD->hd=(void*)((int)_tmp477),((_tmp8FD->tl=flds,_tmp8FD)))))));}_LL261: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp473;if(_tmp475->tag != 5)goto _LL263;else{_tmp476=((_tmp475->f1).ptr_atts).bounds;}}_LL262: {
# 1802
struct _RegionHandle _tmp479=_new_region("temp");struct _RegionHandle*temp=& _tmp479;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp8FE[2];struct _tuple23 _tmp47A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp8FE[1]=_tmp43D,((_tmp8FE[0]=_tmp43C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8FE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1803
union Cyc_CfFlowInfo_FlowInfo _tmp47C;struct Cyc_List_List*_tmp47D;struct _tuple23 _tmp47B=_tmp47A;_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;{
# 1805
union Cyc_CfFlowInfo_FlowInfo _tmp47E=_tmp47C;
{union Cyc_CfFlowInfo_FlowInfo _tmp47F=_tmp47C;struct Cyc_Dict_Dict _tmp480;struct Cyc_List_List*_tmp481;_LL266: if((_tmp47F.ReachableFL).tag != 2)goto _LL268;_tmp480=((struct _tuple14)(_tmp47F.ReachableFL).val).f1;_tmp481=((struct _tuple14)(_tmp47F.ReachableFL).val).f2;_LL267:
# 1808
 if(Cyc_CfFlowInfo_initlevel(_tmp41D,_tmp480,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp47D))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp901;void*_tmp900;(_tmp900=0,Cyc_CfFlowInfo_aerr(_tmp43D->loc,((_tmp901="expression may not be initialized",_tag_dyneither(_tmp901,sizeof(char),34))),_tag_dyneither(_tmp900,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp484=Cyc_NewControlFlow_add_subscript_reln(_tmp41D->r,_tmp481,_tmp43C,_tmp43D);
if(_tmp481 != _tmp484)
_tmp47E=Cyc_CfFlowInfo_ReachableFL(_tmp480,_tmp484);
goto _LL265;};_LL268:;_LL269:
 goto _LL265;_LL265:;}{
# 1816
struct _tuple18 _tmp485=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43C,_tmp47C,(void*)((struct Cyc_List_List*)_check_null(_tmp47D))->hd,passthrough_consumes,flds);union Cyc_CfFlowInfo_FlowInfo _tmp487;union Cyc_CfFlowInfo_AbsLVal _tmp488;struct _tuple18 _tmp486=_tmp485;_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp489=_tmp487;_LL26B: if((_tmp489.BottomFL).tag != 1)goto _LL26D;_LL26C: {
struct _tuple18 _tmp902;struct _tuple18 _tmp48B=(_tmp902.f1=_tmp487,((_tmp902.f2=_tmp488,_tmp902)));_npop_handler(0);return _tmp48B;}_LL26D:;_LL26E: {
struct _tuple18 _tmp903;struct _tuple18 _tmp48D=(_tmp903.f1=_tmp47E,((_tmp903.f2=_tmp488,_tmp903)));_npop_handler(0);return _tmp48D;}_LL26A:;};};};}
# 1803
;_pop_region(temp);}_LL263:;_LL264: {
# 1822
struct Cyc_Core_Impossible_exn_struct _tmp909;const char*_tmp908;struct Cyc_Core_Impossible_exn_struct*_tmp907;(int)_throw((void*)((_tmp907=_cycalloc(sizeof(*_tmp907)),((_tmp907[0]=((_tmp909.tag=Cyc_Core_Impossible,((_tmp909.f1=((_tmp908="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp908,sizeof(char),33))),_tmp909)))),_tmp907)))));}_LL25E:;}_LL247: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp43E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp423;if(_tmp43E->tag != 20)goto _LL249;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}_LL248: {
# 1826
void*_tmp492=(void*)_check_null(_tmp43F->topt);
if(Cyc_Absyn_is_require_union_type(_tmp492))
Cyc_NewControlFlow_check_requires(_tmp43F->loc,_tmp41D->r,_tmp492,_tmp440,inflow);
# 1830
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp492)){
struct _tuple18 _tmp90A;return(_tmp90A.f1=inflow,((_tmp90A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp90A)));}{
# 1833
struct Cyc_List_List*_tmp90B;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp43F,(
(_tmp90B=_region_malloc(env->r,sizeof(*_tmp90B)),((_tmp90B->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp492,_tmp440),((_tmp90B->tl=flds,_tmp90B)))))));};}_LL249:;_LL24A: {
# 1836
struct _tuple18 _tmp90C;return(_tmp90C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp90C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp90C)));}_LL232:;};}
# 1840
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1843
struct _tuple18 _tmp496=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp498;union Cyc_CfFlowInfo_AbsLVal _tmp499;struct _tuple18 _tmp497=_tmp496;_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;{
struct _tuple18 _tmp90D;return(_tmp90D.f1=_tmp498,((_tmp90D.f2=_tmp499,_tmp90D)));};}
# 1850
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1852
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp49B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp49D;struct _tuple18 _tmp49C=_tmp49B;_tmp49D=_tmp49C.f1;
inflow=_tmp49D;}}}
# 1859
return inflow;}
# 1865
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1868
struct Cyc_CfFlowInfo_FlowEnv*_tmp49E=env->fenv;
# 1871
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp49F=_new_region("temp");struct _RegionHandle*temp=& _tmp49F;_push_region(temp);{
struct _tuple23 _tmp4A0=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp4A2;struct Cyc_List_List*_tmp4A3;struct _tuple23 _tmp4A1=_tmp4A0;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp4A3))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4A3->tl))->hd;
f=_tmp4A2;}{
# 1881
union Cyc_CfFlowInfo_FlowInfo _tmp4A4=f;struct Cyc_Dict_Dict _tmp4A5;struct Cyc_List_List*_tmp4A6;_LL270: if((_tmp4A4.BottomFL).tag != 1)goto _LL272;_LL271: {
struct _tuple19 _tmp90E;struct _tuple19 _tmp4A8=(_tmp90E.f1=f,((_tmp90E.f2=f,_tmp90E)));_npop_handler(0);return _tmp4A8;}_LL272: if((_tmp4A4.ReachableFL).tag != 2)goto _LL26F;_tmp4A5=((struct _tuple14)(_tmp4A4.ReachableFL).val).f1;_tmp4A6=((struct _tuple14)(_tmp4A4.ReachableFL).val).f2;_LL273: {
# 1884
struct Cyc_Absyn_Exp*_tmp4A9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp4AA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A5,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp911;void*_tmp910;(_tmp910=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp911="expression may not be initialized",_tag_dyneither(_tmp911,sizeof(char),34))),_tag_dyneither(_tmp910,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A5,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp914;void*_tmp913;(_tmp913=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp914="expression may not be initialized",_tag_dyneither(_tmp914,sizeof(char),34))),_tag_dyneither(_tmp913,sizeof(void*),0)));}{
# 1891
union Cyc_CfFlowInfo_FlowInfo _tmp4AF=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4B0=f;
# 1896
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp4B1=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp4B3;struct Cyc_List_List*_tmp4B4;struct _tuple15 _tmp4B2=_tmp4B1;_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{
struct _tuple15 _tmp4B5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp4B7;struct Cyc_List_List*_tmp4B8;struct _tuple15 _tmp4B6=_tmp4B5;_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;{
struct _tuple0 _tmp915;struct _tuple0 _tmp4B9=(_tmp915.f1=_tmp4B3,((_tmp915.f2=_tmp4B7,_tmp915)));struct _tuple0 _tmp4BA=_tmp4B9;enum Cyc_CfFlowInfo_InitLevel _tmp4BC;enum Cyc_CfFlowInfo_InitLevel _tmp4C0;_LL275:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4BB->tag != 3)goto _LL277;else{_tmp4BC=_tmp4BB->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4BD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4BD->tag != 0)goto _LL277;};_LL276: {
# 1903
struct _tuple19 _tmp4CF=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4A9,_tmp4BC,_tmp4B4);union Cyc_CfFlowInfo_FlowInfo _tmp4D1;union Cyc_CfFlowInfo_FlowInfo _tmp4D2;struct _tuple19 _tmp4D0=_tmp4CF;_tmp4D1=_tmp4D0.f1;_tmp4D2=_tmp4D0.f2;
switch(p){case Cyc_Absyn_Eq: _LL289:
 _tmp4AF=_tmp4D2;_tmp4B0=_tmp4D1;break;case Cyc_Absyn_Neq: _LL28A:
 _tmp4AF=_tmp4D1;_tmp4B0=_tmp4D2;break;default: _LL28B: {
struct Cyc_Core_Impossible_exn_struct _tmp91B;const char*_tmp91A;struct Cyc_Core_Impossible_exn_struct*_tmp919;(int)_throw((void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp91B.tag=Cyc_Core_Impossible,((_tmp91B.f1=((_tmp91A="anal_test, zero-split",_tag_dyneither(_tmp91A,sizeof(char),22))),_tmp91B)))),_tmp919)))));}}
# 1909
goto _LL274;}_LL277:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4BE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4BE->tag != 0)goto _LL279;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4BF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4BF->tag != 3)goto _LL279;else{_tmp4C0=_tmp4BF->f1;}};_LL278: {
# 1911
struct _tuple19 _tmp4D6=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4AA,_tmp4C0,_tmp4B8);union Cyc_CfFlowInfo_FlowInfo _tmp4D8;union Cyc_CfFlowInfo_FlowInfo _tmp4D9;struct _tuple19 _tmp4D7=_tmp4D6;_tmp4D8=_tmp4D7.f1;_tmp4D9=_tmp4D7.f2;
switch(p){case Cyc_Absyn_Eq: _LL28D:
 _tmp4AF=_tmp4D9;_tmp4B0=_tmp4D8;break;case Cyc_Absyn_Neq: _LL28E:
 _tmp4AF=_tmp4D8;_tmp4B0=_tmp4D9;break;default: _LL28F: {
struct Cyc_Core_Impossible_exn_struct _tmp921;const char*_tmp920;struct Cyc_Core_Impossible_exn_struct*_tmp91F;(int)_throw((void*)((_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp921.tag=Cyc_Core_Impossible,((_tmp921.f1=((_tmp920="anal_test, zero-split",_tag_dyneither(_tmp920,sizeof(char),22))),_tmp921)))),_tmp91F)))));}}
# 1917
goto _LL274;}_LL279:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4C1->tag != 0)goto _LL27B;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4C2->tag != 0)goto _LL27B;};_LL27A:
# 1919
 if(p == Cyc_Absyn_Eq)_tmp4B0=Cyc_CfFlowInfo_BottomFL();else{
_tmp4AF=Cyc_CfFlowInfo_BottomFL();}
goto _LL274;_LL27B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4C3->tag != 0)goto _LL27D;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4C4=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4C4->tag != 1)goto _LL27D;};_LL27C:
 goto _LL27E;_LL27D:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C5=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4C5->tag != 0)goto _LL27F;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4C6=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4C6->tag != 2)goto _LL27F;};_LL27E:
 goto _LL280;_LL27F:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C7=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4C7->tag != 0)goto _LL281;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4C8=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4C8->tag != 5)goto _LL281;};_LL280:
 goto _LL282;_LL281:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4C9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4C9->tag != 1)goto _LL283;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4CA->tag != 0)goto _LL283;};_LL282:
 goto _LL284;_LL283:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4CB=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4CB->tag != 2)goto _LL285;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CC=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4CC->tag != 0)goto _LL285;};_LL284:
 goto _LL286;_LL285:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4CD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f1;if(_tmp4CD->tag != 5)goto _LL287;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4BA.f2;if(_tmp4CE->tag != 0)goto _LL287;};_LL286:
# 1928
 if(p == Cyc_Absyn_Neq)_tmp4B0=Cyc_CfFlowInfo_BottomFL();else{
_tmp4AF=Cyc_CfFlowInfo_BottomFL();}
goto _LL274;_LL287:;_LL288:
 goto _LL274;_LL274:;};};}
# 1939
{struct _tuple0 _tmp922;struct _tuple0 _tmp4DE=(_tmp922.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp4A9->topt)),((_tmp922.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp4AA->topt)),_tmp922)));struct _tuple0 _tmp4DF=_tmp4DE;_LL292: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DF.f1;if(_tmp4E0->tag != 6)goto _LL294;else{if(_tmp4E0->f1 != Cyc_Absyn_Unsigned)goto _LL294;}}_LL293:
 goto _LL295;_LL294: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DF.f2;if(_tmp4E1->tag != 6)goto _LL296;else{if(_tmp4E1->f1 != Cyc_Absyn_Unsigned)goto _LL296;}}_LL295:
 goto _LL297;_LL296: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DF.f1;if(_tmp4E2->tag != 19)goto _LL298;}_LL297:
 goto _LL299;_LL298: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DF.f2;if(_tmp4E3->tag != 19)goto _LL29A;}_LL299:
 goto _LL291;_LL29A:;_LL29B: {
struct _tuple19 _tmp923;struct _tuple19 _tmp4E5=(_tmp923.f1=_tmp4AF,((_tmp923.f2=_tmp4B0,_tmp923)));_npop_handler(0);return _tmp4E5;}_LL291:;}
# 1948
switch(p){case Cyc_Absyn_Eq: _LL29C:
 goto _LL29D;case Cyc_Absyn_Neq: _LL29D: goto _LL29E;case Cyc_Absyn_Gt: _LL29E: goto _LL29F;case Cyc_Absyn_Gte: _LL29F: goto _LL2A0;case Cyc_Absyn_Lt: _LL2A0: goto _LL2A1;case Cyc_Absyn_Lte: _LL2A1: break;default: _LL2A2: {
struct _tuple19 _tmp924;struct _tuple19 _tmp4E8=(_tmp924.f1=_tmp4AF,((_tmp924.f2=_tmp4B0,_tmp924)));_npop_handler(0);return _tmp4E8;}}{
# 1953
struct _RegionHandle*_tmp4E9=(env->fenv)->r;
struct _tuple13 _tmp4EA=Cyc_Relations_primop2relation(_tmp4A9,p,_tmp4AA);struct Cyc_Absyn_Exp*_tmp4EC;enum Cyc_Relations_Relation _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct _tuple13 _tmp4EB=_tmp4EA;_tmp4EC=_tmp4EB.f1;_tmp4ED=_tmp4EB.f2;_tmp4EE=_tmp4EB.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 1958
if(Cyc_Relations_exp2relnop(_tmp4EC,& n1) && Cyc_Relations_exp2relnop(_tmp4EE,& n2)){
# 1960
struct Cyc_List_List*_tmp4EF=Cyc_Relations_add_relation(_tmp4E9,n1,_tmp4ED,n2,_tmp4A6);
# 1964
struct Cyc_List_List*_tmp4F0=Cyc_Relations_add_relation(_tmp4E9,n2,Cyc_Relations_flip_relation(_tmp4ED),n1,_tmp4A6);
struct _tuple19 _tmp925;struct _tuple19 _tmp4F1=(_tmp925.f1=_tmp4AF,((_tmp925.f2=_tmp4B0,_tmp925)));struct _tuple19 _tmp4F2=_tmp4F1;struct Cyc_Dict_Dict _tmp4F3;struct Cyc_Dict_Dict _tmp4F4;struct Cyc_Dict_Dict _tmp4F5;struct Cyc_Dict_Dict _tmp4F6;_LL2A5: if(((_tmp4F2.f1).ReachableFL).tag != 2)goto _LL2A7;_tmp4F3=((struct _tuple14)((_tmp4F2.f1).ReachableFL).val).f1;if(((_tmp4F2.f2).ReachableFL).tag != 2)goto _LL2A7;_tmp4F4=((struct _tuple14)((_tmp4F2.f2).ReachableFL).val).f1;_LL2A6: {
# 1967
struct _tuple19 _tmp926;struct _tuple19 _tmp4F8=(_tmp926.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4F3,_tmp4EF),((_tmp926.f2=Cyc_CfFlowInfo_ReachableFL(_tmp4F4,_tmp4F0),_tmp926)));_npop_handler(0);return _tmp4F8;}_LL2A7: if(((_tmp4F2.f2).ReachableFL).tag != 2)goto _LL2A9;_tmp4F5=((struct _tuple14)((_tmp4F2.f2).ReachableFL).val).f1;_LL2A8: {
# 1969
struct _tuple19 _tmp927;struct _tuple19 _tmp4FA=(_tmp927.f1=_tmp4AF,((_tmp927.f2=Cyc_CfFlowInfo_ReachableFL(_tmp4F5,_tmp4F0),_tmp927)));_npop_handler(0);return _tmp4FA;}_LL2A9: if(((_tmp4F2.f1).ReachableFL).tag != 2)goto _LL2AB;_tmp4F6=((struct _tuple14)((_tmp4F2.f1).ReachableFL).val).f1;_LL2AA: {
# 1971
struct _tuple19 _tmp928;struct _tuple19 _tmp4FC=(_tmp928.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4F6,_tmp4EF),((_tmp928.f2=_tmp4B0,_tmp928)));_npop_handler(0);return _tmp4FC;}_LL2AB:;_LL2AC: {
# 1973
struct _tuple19 _tmp929;struct _tuple19 _tmp4FE=(_tmp929.f1=_tmp4AF,((_tmp929.f2=_tmp4B0,_tmp929)));_npop_handler(0);return _tmp4FE;}_LL2A4:;}else{
# 1976
struct _tuple19 _tmp92A;struct _tuple19 _tmp501=(_tmp92A.f1=_tmp4AF,((_tmp92A.f2=_tmp4B0,_tmp92A)));_npop_handler(0);return _tmp501;}};};};}_LL26F:;};
# 1873
;_pop_region(temp);}
# 1982
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1984
struct Cyc_CfFlowInfo_FlowEnv*_tmp502=env->fenv;
void*_tmp503=e->r;void*_tmp504=_tmp503;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp513;enum Cyc_Absyn_Primop _tmp515;struct Cyc_List_List*_tmp516;_LL2AE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp505->tag != 5)goto _LL2B0;else{_tmp506=_tmp505->f1;_tmp507=_tmp505->f2;_tmp508=_tmp505->f3;}}_LL2AF: {
# 1987
struct _tuple19 _tmp517=Cyc_NewControlFlow_anal_test(env,inflow,_tmp506);union Cyc_CfFlowInfo_FlowInfo _tmp519;union Cyc_CfFlowInfo_FlowInfo _tmp51A;struct _tuple19 _tmp518=_tmp517;_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;{
struct _tuple19 _tmp51B=Cyc_NewControlFlow_anal_test(env,_tmp519,_tmp507);union Cyc_CfFlowInfo_FlowInfo _tmp51D;union Cyc_CfFlowInfo_FlowInfo _tmp51E;struct _tuple19 _tmp51C=_tmp51B;_tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;{
struct _tuple19 _tmp51F=Cyc_NewControlFlow_anal_test(env,_tmp51A,_tmp508);union Cyc_CfFlowInfo_FlowInfo _tmp521;union Cyc_CfFlowInfo_FlowInfo _tmp522;struct _tuple19 _tmp520=_tmp51F;_tmp521=_tmp520.f1;_tmp522=_tmp520.f2;{
struct _tuple19 _tmp92B;return(_tmp92B.f1=Cyc_CfFlowInfo_join_flow(_tmp502,env->all_changed,_tmp51D,_tmp521),((_tmp92B.f2=
Cyc_CfFlowInfo_join_flow(_tmp502,env->all_changed,_tmp51E,_tmp522),_tmp92B)));};};};}_LL2B0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp509->tag != 6)goto _LL2B2;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f2;}}_LL2B1: {
# 1993
struct _tuple19 _tmp524=Cyc_NewControlFlow_anal_test(env,inflow,_tmp50A);union Cyc_CfFlowInfo_FlowInfo _tmp526;union Cyc_CfFlowInfo_FlowInfo _tmp527;struct _tuple19 _tmp525=_tmp524;_tmp526=_tmp525.f1;_tmp527=_tmp525.f2;{
struct _tuple19 _tmp528=Cyc_NewControlFlow_anal_test(env,_tmp526,_tmp50B);union Cyc_CfFlowInfo_FlowInfo _tmp52A;union Cyc_CfFlowInfo_FlowInfo _tmp52B;struct _tuple19 _tmp529=_tmp528;_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;{
struct _tuple19 _tmp92C;return(_tmp92C.f1=_tmp52A,((_tmp92C.f2=Cyc_CfFlowInfo_join_flow(_tmp502,env->all_changed,_tmp527,_tmp52B),_tmp92C)));};};}_LL2B2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp50C->tag != 7)goto _LL2B4;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;}}_LL2B3: {
# 1997
struct _tuple19 _tmp52D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp50D);union Cyc_CfFlowInfo_FlowInfo _tmp52F;union Cyc_CfFlowInfo_FlowInfo _tmp530;struct _tuple19 _tmp52E=_tmp52D;_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;{
struct _tuple19 _tmp531=Cyc_NewControlFlow_anal_test(env,_tmp530,_tmp50E);union Cyc_CfFlowInfo_FlowInfo _tmp533;union Cyc_CfFlowInfo_FlowInfo _tmp534;struct _tuple19 _tmp532=_tmp531;_tmp533=_tmp532.f1;_tmp534=_tmp532.f2;{
struct _tuple19 _tmp92D;return(_tmp92D.f1=Cyc_CfFlowInfo_join_flow(_tmp502,env->all_changed,_tmp52F,_tmp533),((_tmp92D.f2=_tmp534,_tmp92D)));};};}_LL2B4: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp50F->tag != 8)goto _LL2B6;else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;}}_LL2B5: {
# 2001
struct _tuple16 _tmp536=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp510);union Cyc_CfFlowInfo_FlowInfo _tmp538;void*_tmp539;struct _tuple16 _tmp537=_tmp536;_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp538,_tmp511);}_LL2B6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp512->tag != 2)goto _LL2B8;else{if(_tmp512->f1 != Cyc_Absyn_Not)goto _LL2B8;if(_tmp512->f2 == 0)goto _LL2B8;_tmp513=(struct Cyc_Absyn_Exp*)(_tmp512->f2)->hd;if((_tmp512->f2)->tl != 0)goto _LL2B8;}}_LL2B7: {
# 2004
struct _tuple19 _tmp53A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp513);union Cyc_CfFlowInfo_FlowInfo _tmp53C;union Cyc_CfFlowInfo_FlowInfo _tmp53D;struct _tuple19 _tmp53B=_tmp53A;_tmp53C=_tmp53B.f1;_tmp53D=_tmp53B.f2;{
struct _tuple19 _tmp92E;return(_tmp92E.f1=_tmp53D,((_tmp92E.f2=_tmp53C,_tmp92E)));};}_LL2B8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp514=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp514->tag != 2)goto _LL2BA;else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL2B9:
# 2007
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp515,_tmp516);_LL2BA:;_LL2BB: {
# 2011
struct _tuple16 _tmp53F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp541;void*_tmp542;struct _tuple16 _tmp540=_tmp53F;_tmp541=_tmp540.f1;_tmp542=_tmp540.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp543=_tmp541;struct Cyc_Dict_Dict _tmp544;_LL2BD: if((_tmp543.BottomFL).tag != 1)goto _LL2BF;_LL2BE: {
struct _tuple19 _tmp92F;return(_tmp92F.f1=_tmp541,((_tmp92F.f2=_tmp541,_tmp92F)));}_LL2BF: if((_tmp543.ReachableFL).tag != 2)goto _LL2BC;_tmp544=((struct _tuple14)(_tmp543.ReachableFL).val).f1;_LL2C0: {
# 2015
struct _tuple15 _tmp546=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp542);void*_tmp548;struct Cyc_List_List*_tmp549;struct _tuple15 _tmp547=_tmp546;_tmp548=_tmp547.f1;_tmp549=_tmp547.f2;{
void*_tmp54A=_tmp548;struct Cyc_Absyn_Vardecl*_tmp54C;void*_tmp54D;enum Cyc_CfFlowInfo_InitLevel _tmp556;_LL2C2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp54B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp54B->tag != 8)goto _LL2C4;else{_tmp54C=_tmp54B->f1;_tmp54D=(void*)_tmp54B->f2;}}_LL2C3: {
# 2018
struct Cyc_Core_Impossible_exn_struct _tmp935;const char*_tmp934;struct Cyc_Core_Impossible_exn_struct*_tmp933;(int)_throw((void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp935.tag=Cyc_Core_Impossible,((_tmp935.f1=((_tmp934="anal_test: bad namedlocation",_tag_dyneither(_tmp934,sizeof(char),29))),_tmp935)))),_tmp933)))));}_LL2C4: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp54E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp54E->tag != 0)goto _LL2C6;}_LL2C5: {
struct _tuple19 _tmp936;return(_tmp936.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp936.f2=_tmp541,_tmp936)));}_LL2C6: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp54F=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp54F->tag != 2)goto _LL2C8;}_LL2C7:
 goto _LL2C9;_LL2C8: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp550=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp550->tag != 1)goto _LL2CA;}_LL2C9:
 goto _LL2CB;_LL2CA: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp551=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp551->tag != 5)goto _LL2CC;}_LL2CB: {
struct _tuple19 _tmp937;return(_tmp937.f1=_tmp541,((_tmp937.f2=Cyc_CfFlowInfo_BottomFL(),_tmp937)));}_LL2CC: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp552=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp552->tag != 3)goto _LL2CE;else{if(_tmp552->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2CE;}}_LL2CD:
 goto _LL2CF;_LL2CE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp553=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp553->tag != 4)goto _LL2D0;else{if(_tmp553->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2D0;}}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp554=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp554->tag != 7)goto _LL2D2;}_LL2D1:
# 2026
{const char*_tmp93A;void*_tmp939;(_tmp939=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp93A="expression may not be initialized",_tag_dyneither(_tmp93A,sizeof(char),34))),_tag_dyneither(_tmp939,sizeof(void*),0)));}{
struct _tuple19 _tmp93B;return(_tmp93B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp93B.f2=Cyc_CfFlowInfo_BottomFL(),_tmp93B)));};_LL2D2: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp555=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp555->tag != 3)goto _LL2D4;else{_tmp556=_tmp555->f1;}}_LL2D3:
# 2029
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp541,e,_tmp556,_tmp549);_LL2D4: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp557=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp557->tag != 4)goto _LL2D6;}_LL2D5: {
struct _tuple19 _tmp93C;return(_tmp93C.f1=_tmp541,((_tmp93C.f2=_tmp541,_tmp93C)));}_LL2D6: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp558=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54A;if(_tmp558->tag != 6)goto _LL2C1;}_LL2D7: {
struct Cyc_Core_Impossible_exn_struct _tmp942;const char*_tmp941;struct Cyc_Core_Impossible_exn_struct*_tmp940;(int)_throw((void*)((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940[0]=((_tmp942.tag=Cyc_Core_Impossible,((_tmp942.f1=((_tmp941="anal_test",_tag_dyneither(_tmp941,sizeof(char),10))),_tmp942)))),_tmp940)))));}_LL2C1:;};}_LL2BC:;};}_LL2AD:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2037
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2039
unsigned int _tmp566;struct Cyc_NewControlFlow_AnalEnv*_tmp567;struct Cyc_Dict_Dict _tmp568;struct _tuple27*_tmp565=ckenv;_tmp566=_tmp565->f1;_tmp567=_tmp565->f2;_tmp568=_tmp565->f3;{
void*_tmp569=root;struct Cyc_Absyn_Vardecl*_tmp56B;_LL2D9: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp56A=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp569;if(_tmp56A->tag != 0)goto _LL2DB;else{_tmp56B=_tmp56A->f1;}}_LL2DA:
# 2042
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp567->fenv)->r,_tmp56B->type)){
struct _tuple15 _tmp56C=Cyc_CfFlowInfo_unname_rval((_tmp567->fenv)->r,rval);void*_tmp56E;struct _tuple15 _tmp56D=_tmp56C;_tmp56E=_tmp56D.f1;{
void*_tmp56F=_tmp56E;_LL2DE: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp570=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp56F;if(_tmp570->tag != 7)goto _LL2E0;}_LL2DF:
 goto _LL2E1;_LL2E0: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp571=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp56F;if(_tmp571->tag != 0)goto _LL2E2;}_LL2E1:
 goto _LL2E3;_LL2E2: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp572=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp56F;if(_tmp572->tag != 3)goto _LL2E4;else{if(_tmp572->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2E4;}}_LL2E3:
 goto _LL2DD;_LL2E4:;_LL2E5:
# 2049
{const char*_tmp946;void*_tmp945[1];struct Cyc_String_pa_PrintArg_struct _tmp944;(_tmp944.tag=0,((_tmp944.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp56B->name)),((_tmp945[0]=& _tmp944,Cyc_Tcutil_warn(_tmp566,((_tmp946="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp946,sizeof(char),57))),_tag_dyneither(_tmp945,sizeof(void*),1)))))));}
goto _LL2DD;_LL2DD:;};}
# 2053
goto _LL2D8;_LL2DB:;_LL2DC:
 goto _LL2D8;_LL2D8:;};}
# 2058
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp576=flow;struct Cyc_Dict_Dict _tmp577;_LL2E7: if((_tmp576.BottomFL).tag != 1)goto _LL2E9;_LL2E8:
 return;_LL2E9: if((_tmp576.ReachableFL).tag != 2)goto _LL2E6;_tmp577=((struct _tuple14)(_tmp576.ReachableFL).val).f1;_LL2EA:
# 2062
{struct Cyc_List_List*_tmp578=env->param_roots;for(0;_tmp578 != 0;_tmp578=_tmp578->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp577,Cyc_CfFlowInfo_lookup_place(_tmp577,(struct Cyc_CfFlowInfo_Place*)_tmp578->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp949;void*_tmp948;(_tmp948=0,Cyc_CfFlowInfo_aerr(loc,((_tmp949="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp949,sizeof(char),76))),_tag_dyneither(_tmp948,sizeof(void*),0)));}}}
# 2066
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp94A;struct _tuple27 _tmp57B=(_tmp94A.f1=loc,((_tmp94A.f2=env,((_tmp94A.f3=_tmp577,_tmp94A)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp57B,_tmp577);}
# 2070
return;_LL2E6:;}
# 2074
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2077
struct Cyc_List_List*_tmp57D=0;
struct Cyc_List_List*_tmp57E=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp57F=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp581;struct Cyc_Absyn_Exp*_tmp582;struct _tuple24*_tmp580=_tmp57F;_tmp581=_tmp580->f1;_tmp582=_tmp580->f2;
if((_tmp581 != 0  && _tmp582 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp582->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp94D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp94C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp583=(_tmp94C=_region_malloc(frgn,sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94D.tag=0,((_tmp94D.f1=*_tmp581,_tmp94D)))),_tmp94C)));
struct Cyc_CfFlowInfo_Place*_tmp94E;struct Cyc_CfFlowInfo_Place*_tmp584=(_tmp94E=_region_malloc(frgn,sizeof(*_tmp94E)),((_tmp94E->root=(void*)_tmp583,((_tmp94E->fields=0,_tmp94E)))));
{struct Cyc_List_List*_tmp94F;_tmp57D=((_tmp94F=_region_malloc(frgn,sizeof(*_tmp94F)),((_tmp94F->hd=_tmp584,((_tmp94F->tl=_tmp57D,_tmp94F))))));}{
struct Cyc_List_List*_tmp950;_tmp57E=((_tmp950=_region_malloc(frgn,sizeof(*_tmp950)),((_tmp950->hd=_tmp582,((_tmp950->tl=_tmp57E,_tmp950))))));};}}}{
# 2089
struct _tuple1 _tmp951;return(_tmp951.f1=_tmp57D,((_tmp951.f2=_tmp57E,_tmp951)));};}
# 2098
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2103
{union Cyc_CfFlowInfo_FlowInfo _tmp58B=flow;_LL2EC: if((_tmp58B.BottomFL).tag != 1)goto _LL2EE;_LL2ED:
 return flow;_LL2EE:;_LL2EF:
 goto _LL2EB;_LL2EB:;}{
# 2107
int _tmp58C=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp58D=Cyc_Absyn_exp_unsigned_one;
int _tmp58E=0;
int _tmp58F=1;
# 2114
{struct Cyc_List_List*_tmp590=consumed_vals;for(0;_tmp590 != 0;
(_tmp590=_tmp590->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp591=flow;struct Cyc_Dict_Dict _tmp592;_LL2F1: if((_tmp591.ReachableFL).tag != 2)goto _LL2F3;_tmp592=((struct _tuple14)(_tmp591.ReachableFL).val).f1;_LL2F2: {
# 2124
void*_tmp593=Cyc_CfFlowInfo_lookup_place(_tmp592,(struct Cyc_CfFlowInfo_Place*)_tmp590->hd);
void*_tmp594=_tmp593;
struct Cyc_CfFlowInfo_Place*_tmp595=(struct Cyc_CfFlowInfo_Place*)_tmp590->hd;void*_tmp597;struct Cyc_List_List*_tmp598;struct Cyc_CfFlowInfo_Place*_tmp596=_tmp595;_tmp597=_tmp596->root;_tmp598=_tmp596->fields;
if(_tmp598 != 0){
const char*_tmp954;void*_tmp953;(_tmp953=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp954="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp954,sizeof(char),53))),_tag_dyneither(_tmp953,sizeof(void*),0)));}
{void*_tmp59B=_tmp597;struct Cyc_Absyn_Vardecl*_tmp59D;_LL2F6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp59C=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp59B;if(_tmp59C->tag != 0)goto _LL2F8;else{_tmp59D=_tmp59C->f1;}}_LL2F7: {
# 2137
int varok=0;
{void*_tmp59E=_tmp593;struct Cyc_Absyn_Vardecl*_tmp5A0;void*_tmp5A1;_LL2FB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp59F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp59E;if(_tmp59F->tag != 8)goto _LL2FD;else{_tmp5A0=_tmp59F->f1;_tmp5A1=(void*)_tmp59F->f2;}}_LL2FC:
# 2140
 if(_tmp59D == _tmp5A0){
_tmp594=_tmp5A1;
# 2143
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp59D->type)){
# 2145
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp58D,_tmp58F,_tmp594,& _tmp58E)){
if(!_tmp58C){
const char*_tmp958;void*_tmp957[1];struct Cyc_String_pa_PrintArg_struct _tmp956;(_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2149
Cyc_Absynpp_qvar2string(_tmp59D->name)),((_tmp957[0]=& _tmp956,Cyc_CfFlowInfo_aerr(loc,((_tmp958="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp958,sizeof(char),58))),_tag_dyneither(_tmp957,sizeof(void*),1)))))));}}else{
# 2152
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp592,_tmp594)!= Cyc_CfFlowInfo_AllIL  && !_tmp58C){
const char*_tmp95C;void*_tmp95B[1];struct Cyc_String_pa_PrintArg_struct _tmp95A;(_tmp95A.tag=0,((_tmp95A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2155
Cyc_Absynpp_qvar2string(_tmp59D->name)),((_tmp95B[0]=& _tmp95A,Cyc_CfFlowInfo_aerr(loc,((_tmp95C="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp95C,sizeof(char),84))),_tag_dyneither(_tmp95B,sizeof(void*),1)))))));}else{
# 2157
varok=1;}}}else{
# 2160
varok=1;}}else{
# 2163
goto _LL2FE;}
goto _LL2FA;_LL2FD:;_LL2FE:
# 2167
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp59D->type))
varok=1;else{
if(!_tmp58C){
const char*_tmp960;void*_tmp95F[1];struct Cyc_String_pa_PrintArg_struct _tmp95E;(_tmp95E.tag=0,((_tmp95E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2172
Cyc_Absynpp_qvar2string(_tmp59D->name)),((_tmp95F[0]=& _tmp95E,Cyc_CfFlowInfo_aerr(loc,((_tmp960="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp960,sizeof(char),85))),_tag_dyneither(_tmp95F,sizeof(void*),1)))))));}}
goto _LL2FA;_LL2FA:;}
# 2177
if(_tmp58C){
# 2183
struct _tuple18 _tmp5AB=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp5AD;union Cyc_CfFlowInfo_AbsLVal _tmp5AE;struct _tuple18 _tmp5AC=_tmp5AB;_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;{
# 2186
union Cyc_CfFlowInfo_AbsLVal _tmp5AF=_tmp5AE;struct Cyc_CfFlowInfo_Place*_tmp5B0;_LL300: if((_tmp5AF.PlaceL).tag != 1)goto _LL302;_tmp5B0=(struct Cyc_CfFlowInfo_Place*)(_tmp5AF.PlaceL).val;_LL301: {
# 2190
void*_tmp5B1=Cyc_CfFlowInfo_lookup_place(_tmp592,_tmp5B0);
{void*_tmp5B2=_tmp5B1;struct Cyc_Absyn_Vardecl*_tmp5B4;void*_tmp5B5;_LL305: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B2;if(_tmp5B3->tag != 8)goto _LL307;else{_tmp5B4=_tmp5B3->f1;_tmp5B5=(void*)_tmp5B3->f2;}}_LL306: {
# 2196
void*new_rval;
if(_tmp5B4 == _tmp59D){
# 2200
if(varok){
# 2202
_tmp5B1=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp5B5);{
# 2211
struct _tuple16 _tmp962;struct _tuple16 _tmp961;struct _tuple16 _tmp5B6=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2214
(_tmp961.f1=_tmp5AD,((_tmp961.f2=_tmp5B1,_tmp961)))),(
(_tmp962.f1=_tmp5AD,((_tmp962.f2=_tmp594,_tmp962)))));
# 2211
union Cyc_CfFlowInfo_FlowInfo _tmp5B8;void*_tmp5B9;struct _tuple16 _tmp5B7=_tmp5B6;_tmp5B8=_tmp5B7.f1;_tmp5B9=_tmp5B7.f2;
# 2216
_tmp5AD=_tmp5B8;new_rval=_tmp5B9;};}else{
# 2218
new_rval=_tmp5B5;}
# 2220
{union Cyc_CfFlowInfo_FlowInfo _tmp5BC=_tmp5AD;struct Cyc_Dict_Dict _tmp5BD;struct Cyc_List_List*_tmp5BE;_LL30A: if((_tmp5BC.ReachableFL).tag != 2)goto _LL30C;_tmp5BD=((struct _tuple14)(_tmp5BC.ReachableFL).val).f1;_tmp5BE=((struct _tuple14)(_tmp5BC.ReachableFL).val).f2;_LL30B:
# 2222
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp5BD,env->all_changed,_tmp5B0,new_rval),_tmp5BE);
# 2226
goto _LL309;_LL30C:;_LL30D: {
# 2228
const char*_tmp965;void*_tmp964;(_tmp964=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp965="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp965,sizeof(char),42))),_tag_dyneither(_tmp964,sizeof(void*),0)));}_LL309:;}
# 2230
goto _LL304;}else{
# 2232
goto _LL308;}
goto _LL304;}_LL307:;_LL308:
# 2239
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp59D->type)){
const char*_tmp969;void*_tmp968[1];struct Cyc_String_pa_PrintArg_struct _tmp967;(_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)unconsume_exps->hd)),((_tmp968[0]=& _tmp967,Cyc_CfFlowInfo_aerr(loc,((_tmp969="aliased expression %s was overwritten",_tag_dyneither(_tmp969,sizeof(char),38))),_tag_dyneither(_tmp968,sizeof(void*),1)))))));}
# 2255 "new_control_flow.cyc"
goto _LL304;_LL304:;}
# 2257
goto _LL2FF;}_LL302:;_LL303:
# 2263
 goto _LL2FF;_LL2FF:;};}
# 2266
goto _LL2F5;}_LL2F8:;_LL2F9: {
# 2269
const char*_tmp96C;void*_tmp96B;(_tmp96B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp96C="unconsume_params: root is not a varroot",_tag_dyneither(_tmp96C,sizeof(char),40))),_tag_dyneither(_tmp96B,sizeof(void*),0)));}_LL2F5:;}
# 2271
goto _LL2F0;}_LL2F3: if((_tmp591.BottomFL).tag != 1)goto _LL2F0;_LL2F4: {
# 2273
const char*_tmp96F;void*_tmp96E;(_tmp96E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp96F="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp96F,sizeof(char),38))),_tag_dyneither(_tmp96E,sizeof(void*),0)));}_LL2F0:;}}
# 2276
return flow;};}struct _tuple28{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2279
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2282
struct Cyc_CfFlowInfo_FlowEnv*_tmp5C8=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5C9=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Stmt*_tmp5CD;unsigned int _tmp5CE;struct Cyc_Absyn_Switch_clause*_tmp5CA=_tmp5C9;_tmp5CB=_tmp5CA->pat_vars;_tmp5CC=_tmp5CA->where_clause;_tmp5CD=_tmp5CA->body;_tmp5CE=_tmp5CA->loc;{
struct _tuple1 _tmp5CF=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CB))->v);struct Cyc_List_List*_tmp5D1;struct Cyc_List_List*_tmp5D2;struct _tuple1 _tmp5D0=_tmp5CF;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp5CB->v,_tmp5D1 != 0,_tmp5CE,exp_loc);
# 2290
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp5D3=env->unique_pat_vars;
{struct _tuple28*_tmp972;struct Cyc_List_List*_tmp971;env->unique_pat_vars=(
(_tmp971=_region_malloc(env->r,sizeof(*_tmp971)),((_tmp971->hd=((_tmp972=_region_malloc(env->r,sizeof(*_tmp972)),((_tmp972->f1=_tmp5CD,((_tmp972->f2=_tmp5D1,((_tmp972->f3=_tmp5D2,_tmp972)))))))),((_tmp971->tl=_tmp5D3,_tmp971))))));}
# 2295
if(_tmp5CC != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5CC;
struct _tuple19 _tmp5D6=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp5D8;union Cyc_CfFlowInfo_FlowInfo _tmp5D9;struct _tuple19 _tmp5D7=_tmp5D6;_tmp5D8=_tmp5D7.f1;_tmp5D9=_tmp5D7.f2;
inflow=_tmp5D9;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5D8,_tmp5CD,0);}else{
# 2301
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5CD,0);}
# 2303
env->unique_pat_vars=_tmp5D3;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DA=clause_outflow;_LL30F: if((_tmp5DA.BottomFL).tag != 1)goto _LL311;_LL310:
 goto _LL30E;_LL311:;_LL312:
# 2308
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5D1,_tmp5D2,clause_outflow,_tmp5CE);
# 2310
if(scs->tl == 0)
return clause_outflow;else{
# 2315
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp975;void*_tmp974;(_tmp974=0,Cyc_CfFlowInfo_aerr(_tmp5CD->loc,((_tmp975="switch clause may implicitly fallthru",_tag_dyneither(_tmp975,sizeof(char),38))),_tag_dyneither(_tmp974,sizeof(void*),0)));}else{
# 2318
const char*_tmp978;void*_tmp977;(_tmp977=0,Cyc_Tcutil_warn(_tmp5CD->loc,((_tmp978="switch clause may implicitly fallthru",_tag_dyneither(_tmp978,sizeof(char),38))),_tag_dyneither(_tmp977,sizeof(void*),0)));}
# 2320
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2322
goto _LL30E;_LL30E:;};};};}
# 2325
return Cyc_CfFlowInfo_BottomFL();}struct _tuple29{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2330
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple29*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2332
struct Cyc_NewControlFlow_AnalEnv*_tmp5E0;struct Cyc_Dict_Dict _tmp5E1;unsigned int _tmp5E2;struct _tuple29*_tmp5DF=vdenv;_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;_tmp5E2=_tmp5DF->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5E0->fenv)->r,vd->type)){
# 2335
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp979;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp979.tag=0,((_tmp979.f1=vd,_tmp979)));
# 2337
struct _tuple15 _tmp5E3=Cyc_CfFlowInfo_unname_rval((_tmp5E0->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5E1,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2337
void*_tmp5E5;struct _tuple15 _tmp5E4=_tmp5E3;_tmp5E5=_tmp5E4.f1;{
# 2339
void*_tmp5E6=_tmp5E5;_LL314: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5E7=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5E6;if(_tmp5E7->tag != 7)goto _LL316;}_LL315:
 goto _LL317;_LL316: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5E8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5E6;if(_tmp5E8->tag != 0)goto _LL318;}_LL317:
 goto _LL319;_LL318: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5E9=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5E6;if(_tmp5E9->tag != 3)goto _LL31A;else{if(_tmp5E9->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL31A;}}_LL319:
 goto _LL313;_LL31A:;_LL31B:
# 2344
{const char*_tmp97D;void*_tmp97C[1];struct Cyc_String_pa_PrintArg_struct _tmp97B;(_tmp97B.tag=0,((_tmp97B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp97C[0]=& _tmp97B,Cyc_Tcutil_warn(_tmp5E2,((_tmp97D="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp97D,sizeof(char),68))),_tag_dyneither(_tmp97C,sizeof(void*),1)))))));}
# 2346
goto _LL313;_LL313:;};}}
# 2351
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2353
{union Cyc_CfFlowInfo_FlowInfo _tmp5EE=inflow;struct Cyc_Dict_Dict _tmp5EF;_LL31D: if((_tmp5EE.ReachableFL).tag != 2)goto _LL31F;_tmp5EF=((struct _tuple14)(_tmp5EE.ReachableFL).val).f1;_LL31E: {
# 2355
struct _tuple29 _tmp97E;struct _tuple29 _tmp5F0=(_tmp97E.f1=env,((_tmp97E.f2=_tmp5EF,((_tmp97E.f3=decl->loc,_tmp97E)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5F1=env->fenv;
{void*_tmp5F2=decl->r;void*_tmp5F3=_tmp5F2;struct Cyc_Absyn_Vardecl*_tmp5F5;struct Cyc_List_List*_tmp5F7;struct Cyc_List_List*_tmp5F9;_LL322: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5F3;if(_tmp5F4->tag != 0)goto _LL324;else{_tmp5F5=_tmp5F4->f1;}}_LL323:
# 2359
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5F0,_tmp5F5);
goto _LL321;_LL324: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp5F6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5F3;if(_tmp5F6->tag != 2)goto _LL326;else{if(_tmp5F6->f2 == 0)goto _LL326;_tmp5F7=(struct Cyc_List_List*)(_tmp5F6->f2)->v;}}_LL325: {
# 2362
struct _tuple1 _tmp5FA=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp5F7);struct Cyc_List_List*_tmp5FC;struct _tuple1 _tmp5FB=_tmp5FA;_tmp5FC=_tmp5FB.f1;{
struct Cyc_List_List*_tmp5FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp5FC);
_tmp5F9=_tmp5FD;goto _LL327;};}_LL326: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp5F8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5F3;if(_tmp5F8->tag != 3)goto _LL328;else{_tmp5F9=_tmp5F8->f1;}}_LL327:
# 2366
((void(*)(void(*f)(struct _tuple29*,struct Cyc_Absyn_Vardecl*),struct _tuple29*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5F0,_tmp5F9);
goto _LL321;_LL328:;_LL329:
 goto _LL321;_LL321:;}
# 2370
goto _LL31C;}_LL31F:;_LL320:
 goto _LL31C;_LL31C:;}
# 2373
return;}
# 2379
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2382
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple28*_tmp5FF=(struct _tuple28*)x->hd;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_List_List*_tmp602;struct Cyc_List_List*_tmp603;struct _tuple28*_tmp600=_tmp5FF;_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;_tmp603=_tmp600->f3;
keep_going=0;{
# 2390
struct Cyc_Absyn_Stmt*_tmp604=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp604 != _tmp601){
struct Cyc_Absyn_Stmt*_tmp605=(Cyc_NewControlFlow_get_stmt_annot(_tmp604))->encloser;
if(_tmp605 == _tmp604){
# 2396
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp602,_tmp603,inflow,dest->loc);
keep_going=1;
break;}
# 2400
_tmp604=_tmp605;}};}}
# 2403
return inflow;}
# 2409
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2412
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp606=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp608;union Cyc_CfFlowInfo_FlowInfo*_tmp609;struct _tuple20 _tmp607=_tmp606;_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;
inflow=*_tmp609;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp60A=env->fenv;
# 2419
void*_tmp60B=s->r;void*_tmp60C=_tmp60B;struct Cyc_Absyn_Exp*_tmp611;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Stmt*_tmp615;struct Cyc_Absyn_Stmt*_tmp616;struct Cyc_Absyn_Exp*_tmp618;struct Cyc_Absyn_Stmt*_tmp619;struct Cyc_Absyn_Stmt*_tmp61A;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Stmt*_tmp61D;struct Cyc_Absyn_Stmt*_tmp61E;struct Cyc_Absyn_Stmt*_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Stmt*_tmp622;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Stmt*_tmp628;struct Cyc_Absyn_Stmt*_tmp629;struct Cyc_List_List*_tmp62B;struct Cyc_Absyn_Switch_clause*_tmp62C;struct Cyc_Absyn_Stmt*_tmp62E;struct Cyc_Absyn_Stmt*_tmp630;struct Cyc_Absyn_Stmt*_tmp632;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_List_List*_tmp635;struct Cyc_Absyn_Stmt*_tmp637;struct Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;unsigned int _tmp63D;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Decl*_tmp640;struct Cyc_Absyn_Stmt*_tmp641;struct Cyc_Absyn_Stmt*_tmp643;struct Cyc_Absyn_Exp*_tmp645;_LL32B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp60D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp60D->tag != 0)goto _LL32D;}_LL32C:
 return inflow;_LL32D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp60E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp60E->tag != 6)goto _LL32F;else{if(_tmp60E->f1 != 0)goto _LL32F;}}_LL32E:
# 2425
{union Cyc_CfFlowInfo_FlowInfo _tmp646=inflow;_LL356: if((_tmp646.ReachableFL).tag != 2)goto _LL358;_LL357:
# 2427
{void*_tmp647=Cyc_Tcutil_compress(env->return_type);void*_tmp648=_tmp647;_LL35B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp649=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp648;if(_tmp649->tag != 0)goto _LL35D;}_LL35C:
 goto _LL35A;_LL35D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp64A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp648;if(_tmp64A->tag != 7)goto _LL35F;}_LL35E:
 goto _LL360;_LL35F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp64B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp648;if(_tmp64B->tag != 6)goto _LL361;}_LL360:
# 2431
{const char*_tmp981;void*_tmp980;(_tmp980=0,Cyc_Tcutil_warn(s->loc,((_tmp981="break may cause function not to return a value",_tag_dyneither(_tmp981,sizeof(char),47))),_tag_dyneither(_tmp980,sizeof(void*),0)));}
goto _LL35A;_LL361:;_LL362:
# 2434
{const char*_tmp984;void*_tmp983;(_tmp983=0,Cyc_Tcutil_terr(s->loc,((_tmp984="break may cause function not to return a value",_tag_dyneither(_tmp984,sizeof(char),47))),_tag_dyneither(_tmp983,sizeof(void*),0)));}
goto _LL35A;_LL35A:;}
# 2437
goto _LL355;_LL358:;_LL359:
 goto _LL355;_LL355:;}
# 2440
goto _LL330;_LL32F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp60F=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp60F->tag != 3)goto _LL331;else{if(_tmp60F->f1 != 0)goto _LL331;}}_LL330:
# 2442
 if(env->noreturn){
const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp987="`noreturn' function might return",_tag_dyneither(_tmp987,sizeof(char),33))),_tag_dyneither(_tmp986,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL331: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp610=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp610->tag != 3)goto _LL333;else{_tmp611=_tmp610->f1;}}_LL332:
# 2448
 if(env->noreturn){
const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp98A="`noreturn' function might return",_tag_dyneither(_tmp98A,sizeof(char),33))),_tag_dyneither(_tmp989,sizeof(void*),0)));}{
struct _tuple16 _tmp654=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp611));union Cyc_CfFlowInfo_FlowInfo _tmp656;void*_tmp657;struct _tuple16 _tmp655=_tmp654;_tmp656=_tmp655.f1;_tmp657=_tmp655.f2;
_tmp656=Cyc_NewControlFlow_use_Rval(env,_tmp611->loc,_tmp656,_tmp657);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp656);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp656,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL333: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp612=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp612->tag != 1)goto _LL335;else{_tmp613=_tmp612->f1;}}_LL334: {
# 2457
struct _tuple17*_tmp658=rval_opt;void**_tmp659;int _tmp65A;_LL364: if(_tmp658 == 0)goto _LL366;_tmp659=(void**)& _tmp658->f1;_tmp65A=_tmp658->f2;_LL365: {
# 2459
struct _tuple16 _tmp65B=Cyc_NewControlFlow_anal_Rexp(env,_tmp65A,inflow,_tmp613);union Cyc_CfFlowInfo_FlowInfo _tmp65D;void*_tmp65E;struct _tuple16 _tmp65C=_tmp65B;_tmp65D=_tmp65C.f1;_tmp65E=_tmp65C.f2;
*_tmp659=_tmp65E;
return _tmp65D;}_LL366: if(_tmp658 != 0)goto _LL363;_LL367:
# 2463
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp613)).f1;_LL363:;}_LL335: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp614=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp614->tag != 2)goto _LL337;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;}}_LL336:
# 2467
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp615,0),_tmp616,rval_opt);_LL337: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp617=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp617->tag != 4)goto _LL339;else{_tmp618=_tmp617->f1;_tmp619=_tmp617->f2;_tmp61A=_tmp617->f3;}}_LL338: {
# 2470
struct _tuple19 _tmp65F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp618);union Cyc_CfFlowInfo_FlowInfo _tmp661;union Cyc_CfFlowInfo_FlowInfo _tmp662;struct _tuple19 _tmp660=_tmp65F;_tmp661=_tmp660.f1;_tmp662=_tmp660.f2;{
# 2477
union Cyc_CfFlowInfo_FlowInfo _tmp663=Cyc_NewControlFlow_anal_stmt(env,_tmp662,_tmp61A,0);
union Cyc_CfFlowInfo_FlowInfo _tmp664=Cyc_NewControlFlow_anal_stmt(env,_tmp661,_tmp619,0);
return Cyc_CfFlowInfo_join_flow(_tmp60A,env->all_changed,_tmp664,_tmp663);};}_LL339: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp61B=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp61B->tag != 5)goto _LL33B;else{_tmp61C=(_tmp61B->f1).f1;_tmp61D=(_tmp61B->f1).f2;_tmp61E=_tmp61B->f2;}}_LL33A: {
# 2485
struct _tuple20 _tmp665=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp61D);union Cyc_CfFlowInfo_FlowInfo*_tmp667;struct _tuple20 _tmp666=_tmp665;_tmp667=_tmp666.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp668=*_tmp667;
struct _tuple19 _tmp669=Cyc_NewControlFlow_anal_test(env,_tmp668,_tmp61C);union Cyc_CfFlowInfo_FlowInfo _tmp66B;union Cyc_CfFlowInfo_FlowInfo _tmp66C;struct _tuple19 _tmp66A=_tmp669;_tmp66B=_tmp66A.f1;_tmp66C=_tmp66A.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp66D=Cyc_NewControlFlow_anal_stmt(env,_tmp66B,_tmp61E,0);
Cyc_NewControlFlow_update_flow(env,_tmp61D,_tmp66D);
return _tmp66C;};};}_LL33B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp61F=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp61F->tag != 14)goto _LL33D;else{_tmp620=_tmp61F->f1;_tmp621=(_tmp61F->f2).f1;_tmp622=(_tmp61F->f2).f2;}}_LL33C: {
# 2495
union Cyc_CfFlowInfo_FlowInfo _tmp66E=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp620,0);
struct _tuple20 _tmp66F=Cyc_NewControlFlow_pre_stmt_check(env,_tmp66E,_tmp622);union Cyc_CfFlowInfo_FlowInfo*_tmp671;struct _tuple20 _tmp670=_tmp66F;_tmp671=_tmp670.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp672=*_tmp671;
struct _tuple19 _tmp673=Cyc_NewControlFlow_anal_test(env,_tmp672,_tmp621);union Cyc_CfFlowInfo_FlowInfo _tmp675;union Cyc_CfFlowInfo_FlowInfo _tmp676;struct _tuple19 _tmp674=_tmp673;_tmp675=_tmp674.f1;_tmp676=_tmp674.f2;
Cyc_NewControlFlow_update_flow(env,_tmp620,_tmp675);
return _tmp676;};}_LL33D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp623=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp623->tag != 9)goto _LL33F;else{_tmp624=_tmp623->f1;_tmp625=(_tmp623->f2).f1;_tmp626=(_tmp623->f2).f2;_tmp627=(_tmp623->f3).f1;_tmp628=(_tmp623->f3).f2;_tmp629=_tmp623->f4;}}_LL33E: {
# 2504
union Cyc_CfFlowInfo_FlowInfo _tmp677=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp624)).f1;
struct _tuple20 _tmp678=Cyc_NewControlFlow_pre_stmt_check(env,_tmp677,_tmp626);union Cyc_CfFlowInfo_FlowInfo*_tmp67A;struct _tuple20 _tmp679=_tmp678;_tmp67A=_tmp679.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp67B=*_tmp67A;
struct _tuple19 _tmp67C=Cyc_NewControlFlow_anal_test(env,_tmp67B,_tmp625);union Cyc_CfFlowInfo_FlowInfo _tmp67E;union Cyc_CfFlowInfo_FlowInfo _tmp67F;struct _tuple19 _tmp67D=_tmp67C;_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp680=Cyc_NewControlFlow_anal_stmt(env,_tmp67E,_tmp629,0);
struct _tuple20 _tmp681=Cyc_NewControlFlow_pre_stmt_check(env,_tmp680,_tmp628);union Cyc_CfFlowInfo_FlowInfo*_tmp683;struct _tuple20 _tmp682=_tmp681;_tmp683=_tmp682.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp684=*_tmp683;
union Cyc_CfFlowInfo_FlowInfo _tmp685=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp684,_tmp627)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp626,_tmp685);
return _tmp67F;};};};}_LL33F: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp62A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp62A->tag != 11)goto _LL341;else{_tmp62B=_tmp62A->f1;if(_tmp62A->f2 == 0)goto _LL341;_tmp62C=*_tmp62A->f2;}}_LL340: {
# 2516
struct _RegionHandle _tmp686=_new_region("temp");struct _RegionHandle*temp=& _tmp686;_push_region(temp);
{struct _tuple23 _tmp687=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp62B,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp689;struct Cyc_List_List*_tmp68A;struct _tuple23 _tmp688=_tmp687;_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;
# 2519
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp62C->body);{
# 2521
struct Cyc_List_List*_tmp68B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62C->pat_vars))->v)).f1);
_tmp689=Cyc_NewControlFlow_add_vars(_tmp60A,_tmp689,_tmp68B,_tmp60A->unknown_all,s->loc,0);
# 2524
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62C->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp68C=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp68E;struct Cyc_Absyn_Exp*_tmp68F;struct _tuple24*_tmp68D=_tmp68C;_tmp68E=_tmp68D->f1;_tmp68F=_tmp68D->f2;
if(_tmp68E != 0){
_tmp689=Cyc_NewControlFlow_do_initialize_var(_tmp60A,env,_tmp689,*_tmp68E,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp62B))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp68A))->hd,s->loc);
_tmp68A=_tmp68A->tl;
_tmp62B=_tmp62B->tl;}}}
# 2532
Cyc_NewControlFlow_update_flow(env,_tmp62C->body,_tmp689);{
union Cyc_CfFlowInfo_FlowInfo _tmp690=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp690;};};}
# 2517
;_pop_region(temp);}_LL341: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp62D=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp62D->tag != 6)goto _LL343;else{_tmp62E=_tmp62D->f1;}}_LL342:
# 2535
 _tmp630=_tmp62E;goto _LL344;_LL343: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp62F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp62F->tag != 7)goto _LL345;else{_tmp630=_tmp62F->f1;}}_LL344:
 _tmp632=_tmp630;goto _LL346;_LL345: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp631=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp631->tag != 8)goto _LL347;else{_tmp632=_tmp631->f2;}}_LL346:
# 2540
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp691=_tmp608->encloser;
struct Cyc_Absyn_Stmt*_tmp692=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp632)))->encloser;
while(_tmp692 != _tmp691){
struct Cyc_Absyn_Stmt*_tmp693=(Cyc_NewControlFlow_get_stmt_annot(_tmp691))->encloser;
if(_tmp693 == _tmp691){
{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp98D="goto enters local scope or exception handler",_tag_dyneither(_tmp98D,sizeof(char),45))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
break;}
# 2549
_tmp691=_tmp693;}}
# 2553
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp632));
# 2555
Cyc_NewControlFlow_update_flow(env,_tmp632,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL347: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp633=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp633->tag != 10)goto _LL349;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;}}_LL348:
# 2561
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp635,_tmp634->loc);_LL349: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp636=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp636->tag != 15)goto _LL34B;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;}}_LL34A: {
# 2566
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp637,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2574
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2577
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp638,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp696=scs_outflow;_LL369: if((_tmp696.BottomFL).tag != 1)goto _LL36B;_LL36A:
 goto _LL368;_LL36B:;_LL36C: {
const char*_tmp990;void*_tmp98F;(_tmp98F=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp990="last catch clause may implicitly fallthru",_tag_dyneither(_tmp990,sizeof(char),42))),_tag_dyneither(_tmp98F,sizeof(void*),0)));}_LL368:;}
# 2583
outflow=s1_outflow;
# 2585
return outflow;};};}_LL34B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp639=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp639->tag != 12)goto _LL34D;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp63A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp639->f1)->r;if(_tmp63A->tag != 2)goto _LL34D;else{if(_tmp63A->f2 == 0)goto _LL34D;_tmp63B=(struct Cyc_List_List*)(_tmp63A->f2)->v;_tmp63C=_tmp63A->f3;}}_tmp63D=(_tmp639->f1)->loc;_tmp63E=_tmp639->f2;}}_LL34C: {
# 2595
struct _tuple1 _tmp699=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp63B);struct Cyc_List_List*_tmp69B;struct Cyc_List_List*_tmp69C;struct _tuple1 _tmp69A=_tmp699;_tmp69B=_tmp69A.f1;_tmp69C=_tmp69A.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp60A,env,inflow,_tmp63B,_tmp69B != 0,_tmp63D,_tmp63C->loc);{
struct Cyc_List_List*_tmp69D=env->unique_pat_vars;
{struct _tuple28*_tmp993;struct Cyc_List_List*_tmp992;env->unique_pat_vars=(
(_tmp992=_region_malloc(env->r,sizeof(*_tmp992)),((_tmp992->hd=((_tmp993=_region_malloc(env->r,sizeof(*_tmp993)),((_tmp993->f1=_tmp63E,((_tmp993->f2=_tmp69B,((_tmp993->f3=_tmp69C,_tmp993)))))))),((_tmp992->tl=_tmp69D,_tmp992))))));}
# 2603
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp63E,rval_opt);
env->unique_pat_vars=_tmp69D;
# 2607
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp69B,_tmp69C,inflow,_tmp63D);
# 2611
return inflow;};}_LL34D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp63F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp63F->tag != 12)goto _LL34F;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL34E:
# 2695 "new_control_flow.cyc"
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp640),_tmp641,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp640);
return outflow;_LL34F: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp642=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp642->tag != 13)goto _LL351;else{_tmp643=_tmp642->f2;}}_LL350:
# 2700
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp643,rval_opt);_LL351: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp644=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp60C;if(_tmp644->tag != 16)goto _LL353;else{_tmp645=_tmp644->f1;}}_LL352: {
# 2703
struct _tuple16 _tmp6A0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp645);union Cyc_CfFlowInfo_FlowInfo _tmp6A2;void*_tmp6A3;struct _tuple16 _tmp6A1=_tmp6A0;_tmp6A2=_tmp6A1.f1;_tmp6A3=_tmp6A1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_NewControlFlow_use_Rval(env,_tmp645->loc,_tmp6A2,_tmp6A3);
void*_tmp6A5=Cyc_Tcutil_compress((void*)_check_null(_tmp645->topt));void*_tmp6A6=_tmp6A5;void*_tmp6A8;_LL36E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6A6;if(_tmp6A7->tag != 15)goto _LL370;else{_tmp6A8=(void*)_tmp6A7->f1;}}_LL36F:
# 2707
 return Cyc_CfFlowInfo_kill_flow_region(_tmp60A,_tmp6A2,_tmp6A8);_LL370:;_LL371: {
struct Cyc_Core_Impossible_exn_struct _tmp999;const char*_tmp998;struct Cyc_Core_Impossible_exn_struct*_tmp997;(int)_throw((void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((_tmp999.tag=Cyc_Core_Impossible,((_tmp999.f1=((_tmp998="anal_stmt -- reset_region",_tag_dyneither(_tmp998,sizeof(char),26))),_tmp999)))),_tmp997)))));}_LL36D:;};}_LL353:;_LL354: {
# 2711
struct Cyc_Core_Impossible_exn_struct _tmp99F;const char*_tmp99E;struct Cyc_Core_Impossible_exn_struct*_tmp99D;(int)_throw((void*)((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99F.tag=Cyc_Core_Impossible,((_tmp99F.f1=((_tmp99E="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp99E,sizeof(char),56))),_tmp99F)))),_tmp99D)))));}_LL32A:;};}
# 2716
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2719
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp6AF=env->fenv;
void*_tmp6B0=decl->r;void*_tmp6B1=_tmp6B0;struct Cyc_Absyn_Vardecl*_tmp6B3;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_Fndecl*_tmp6B7;struct Cyc_Absyn_Tvar*_tmp6B9;struct Cyc_Absyn_Vardecl*_tmp6BA;int _tmp6BB;struct Cyc_Absyn_Exp*_tmp6BC;_LL373: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6B1;if(_tmp6B2->tag != 0)goto _LL375;else{_tmp6B3=_tmp6B2->f1;}}_LL374: {
# 2729
struct Cyc_List_List _tmp9A0;struct Cyc_List_List _tmp6BD=(_tmp9A0.hd=_tmp6B3,((_tmp9A0.tl=0,_tmp9A0)));
inflow=Cyc_NewControlFlow_add_vars(_tmp6AF,inflow,& _tmp6BD,_tmp6AF->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp6BE=_tmp6B3->initializer;
if(_tmp6BE == 0)
return inflow;{
struct _tuple16 _tmp6BF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6BE);union Cyc_CfFlowInfo_FlowInfo _tmp6C1;void*_tmp6C2;struct _tuple16 _tmp6C0=_tmp6BF;_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp6AF,env,_tmp6C1,_tmp6B3,_tmp6BE,_tmp6C2,decl->loc);};};}_LL375: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp6B4=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6B1;if(_tmp6B4->tag != 3)goto _LL377;else{_tmp6B5=_tmp6B4->f1;}}_LL376:
# 2738
 return Cyc_NewControlFlow_add_vars(_tmp6AF,inflow,_tmp6B5,_tmp6AF->unknown_none,decl->loc,0);_LL377: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6B6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6B1;if(_tmp6B6->tag != 1)goto _LL379;else{_tmp6B7=_tmp6B6->f1;}}_LL378:
# 2741
 Cyc_NewControlFlow_check_nested_fun(_tmp6AF,inflow,_tmp6B7);{
void*t=(void*)_check_null(_tmp6B7->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp6C4=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6B7->fn_vardecl);
# 2747
struct Cyc_List_List*_tmp9A1;return Cyc_NewControlFlow_add_vars(_tmp6AF,inflow,((_tmp9A1=_region_malloc(env->r,sizeof(*_tmp9A1)),((_tmp9A1->hd=_tmp6C4,((_tmp9A1->tl=0,_tmp9A1)))))),_tmp6AF->unknown_all,decl->loc,0);};_LL379: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp6B8=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6B1;if(_tmp6B8->tag != 4)goto _LL37B;else{_tmp6B9=_tmp6B8->f1;_tmp6BA=_tmp6B8->f2;_tmp6BB=_tmp6B8->f3;_tmp6BC=_tmp6B8->f4;}}_LL37A:
# 2750
 if(_tmp6BC != 0){
struct Cyc_Absyn_Exp*_tmp6C6=_tmp6BC;
struct _tuple16 _tmp6C7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6C6);union Cyc_CfFlowInfo_FlowInfo _tmp6C9;void*_tmp6CA;struct _tuple16 _tmp6C8=_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp6C6->loc,_tmp6C9,_tmp6CA);}{
# 2755
struct Cyc_List_List _tmp9A2;struct Cyc_List_List _tmp6CB=(_tmp9A2.hd=_tmp6BA,((_tmp9A2.tl=0,_tmp9A2)));
return Cyc_NewControlFlow_add_vars(_tmp6AF,inflow,& _tmp6CB,_tmp6AF->unknown_all,decl->loc,0);};_LL37B:;_LL37C: {
# 2759
struct Cyc_Core_Impossible_exn_struct _tmp9A8;const char*_tmp9A7;struct Cyc_Core_Impossible_exn_struct*_tmp9A6;(int)_throw((void*)((_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A8.tag=Cyc_Core_Impossible,((_tmp9A8.f1=((_tmp9A7="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9A7,sizeof(char),35))),_tmp9A8)))),_tmp9A6)))));}_LL372:;}
# 2767
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp6D0=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6D0;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2769
;_pop_region(frgn);}
# 2774
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2778
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2782
struct _RegionHandle*_tmp6D1=fenv->r;
unsigned int _tmp6D2=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6D2,1);{
# 2788
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6D3=inflow;struct Cyc_Dict_Dict _tmp6D4;struct Cyc_List_List*_tmp6D5;_LL37E: if((_tmp6D3.BottomFL).tag != 1)goto _LL380;_LL37F: {
const char*_tmp9AB;void*_tmp9AA;(_tmp9AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AB="check_fun",_tag_dyneither(_tmp9AB,sizeof(char),10))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}_LL380: if((_tmp6D3.ReachableFL).tag != 2)goto _LL37D;_tmp6D4=((struct _tuple14)(_tmp6D3.ReachableFL).val).f1;_tmp6D5=((struct _tuple14)(_tmp6D3.ReachableFL).val).f2;_LL381: {
# 2795
struct Cyc_List_List*atts;
{void*_tmp6D8=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp6D9=_tmp6D8;struct Cyc_List_List*_tmp6DB;_LL383: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D9;if(_tmp6DA->tag != 9)goto _LL385;else{_tmp6DB=(_tmp6DA->f1).attributes;}}_LL384:
 atts=_tmp6DB;goto _LL382;_LL385:;_LL386: {
const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AE="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp9AE,sizeof(char),50))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}_LL382:;}
# 2802
for(0;atts != 0;atts=atts->tl){
void*_tmp6DE=(void*)atts->hd;void*_tmp6DF=_tmp6DE;int _tmp6E1;int _tmp6E3;int _tmp6E5;_LL388: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp6E0=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6DF;if(_tmp6E0->tag != 21)goto _LL38A;else{_tmp6E1=_tmp6E0->f1;}}_LL389: {
# 2805
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp6E6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6E1 - 1);
void*t=Cyc_Tcutil_compress(_tmp6E6->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6E7=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9B1;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9B0;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6E8=(_tmp9B0=_region_malloc(_tmp6D1,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=2,((_tmp9B1.f1=_tmp6E1,((_tmp9B1.f2=t,_tmp9B1)))))),_tmp9B0)));
struct Cyc_CfFlowInfo_Place*_tmp9B2;struct Cyc_CfFlowInfo_Place*_tmp6E9=(_tmp9B2=_region_malloc(_tmp6D1,sizeof(*_tmp9B2)),((_tmp9B2->root=(void*)_tmp6E8,((_tmp9B2->fields=0,_tmp9B2)))));
_tmp6D4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6D4,(void*)_tmp6E8,_tmp6E7);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9B8;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9B7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9B5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9B4;_tmp6D4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6D4,(void*)((_tmp9B4=_region_malloc(_tmp6D1,sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=0,((_tmp9B5.f1=_tmp6E6,_tmp9B5)))),_tmp9B4)))),(void*)((_tmp9B7=_region_malloc(_tmp6D1,sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=5,((_tmp9B8.f1=_tmp6E9,_tmp9B8)))),_tmp9B7)))));}
goto _LL387;}_LL38A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6E2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6DF;if(_tmp6E2->tag != 20)goto _LL38C;else{_tmp6E3=_tmp6E2->f1;}}_LL38B: {
# 2818
struct Cyc_Absyn_Vardecl*_tmp6F1=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6E3 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp6F1->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9BB;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9BA;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6F2=(_tmp9BA=_region_malloc(_tmp6D1,sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=2,((_tmp9BB.f1=_tmp6E3,((_tmp9BB.f2=elttype,_tmp9BB)))))),_tmp9BA)));
struct Cyc_CfFlowInfo_Place*_tmp9BC;struct Cyc_CfFlowInfo_Place*_tmp6F3=(_tmp9BC=_region_malloc(_tmp6D1,sizeof(*_tmp9BC)),((_tmp9BC->root=(void*)_tmp6F2,((_tmp9BC->fields=0,_tmp9BC)))));
_tmp6D4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6D4,(void*)_tmp6F2,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9C2;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9C1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9BF;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9BE;_tmp6D4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6D4,(void*)((_tmp9BE=_region_malloc(_tmp6D1,sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=0,((_tmp9BF.f1=_tmp6F1,_tmp9BF)))),_tmp9BE)))),(void*)((_tmp9C1=_region_malloc(_tmp6D1,sizeof(*_tmp9C1)),((_tmp9C1[0]=((_tmp9C2.tag=5,((_tmp9C2.f1=_tmp6F3,_tmp9C2)))),_tmp9C1)))));}
{struct Cyc_List_List*_tmp9C3;param_roots=((_tmp9C3=_region_malloc(_tmp6D1,sizeof(*_tmp9C3)),((_tmp9C3->hd=_tmp6F3,((_tmp9C3->tl=param_roots,_tmp9C3))))));}
goto _LL387;}_LL38C: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6E4=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6DF;if(_tmp6E4->tag != 22)goto _LL38E;else{_tmp6E5=_tmp6E4->f1;}}_LL38D: {
# 2827
struct Cyc_Absyn_Vardecl*_tmp6FC=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6E5 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp6FC->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9C6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9C5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6FD=(_tmp9C5=_region_malloc(_tmp6D1,sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=0,((_tmp9C6.f1=_tmp6FC,_tmp9C6)))),_tmp9C5)));
struct Cyc_CfFlowInfo_Place*_tmp9C7;struct Cyc_CfFlowInfo_Place*_tmp6FE=(_tmp9C7=_region_malloc(_tmp6D1,sizeof(*_tmp9C7)),((_tmp9C7->root=(void*)_tmp6FD,((_tmp9C7->fields=0,_tmp9C7)))));
struct Cyc_List_List*_tmp9C8;noconsume_roots=((_tmp9C8=_region_malloc(_tmp6D1,sizeof(*_tmp9C8)),((_tmp9C8->hd=_tmp6FE,((_tmp9C8->tl=noconsume_roots,_tmp9C8))))));}
# 2833
goto _LL387;}_LL38E:;_LL38F:
 goto _LL387;_LL387:;}
# 2836
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp6D4,_tmp6D5);}_LL37D:;}{
# 2839
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6D1,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp9C9;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp9C9=_region_malloc(_tmp6D1,sizeof(*_tmp9C9)),((_tmp9C9->r=_tmp6D1,((_tmp9C9->fenv=fenv,((_tmp9C9->iterate_again=1,((_tmp9C9->iteration_num=0,((_tmp9C9->in_try=0,((_tmp9C9->tryflow=inflow,((_tmp9C9->all_changed=0,((_tmp9C9->noreturn=noreturn,((_tmp9C9->return_type=fd->ret_type,((_tmp9C9->unique_pat_vars=0,((_tmp9C9->param_roots=param_roots,((_tmp9C9->noconsume_params=noconsume_roots,((_tmp9C9->flow_table=flow_table,_tmp9C9)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2849
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2852
union Cyc_CfFlowInfo_FlowInfo _tmp703=outflow;_LL391: if((_tmp703.BottomFL).tag != 1)goto _LL393;_LL392:
 goto _LL390;_LL393:;_LL394:
# 2855
 Cyc_NewControlFlow_check_init_params(_tmp6D2,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp6D2);
# 2859
if(noreturn){
const char*_tmp9CC;void*_tmp9CB;(_tmp9CB=0,Cyc_CfFlowInfo_aerr(_tmp6D2,((_tmp9CC="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp9CC,sizeof(char),46))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));}else{
# 2862
void*_tmp706=Cyc_Tcutil_compress(fd->ret_type);void*_tmp707=_tmp706;_LL396: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp708=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp707;if(_tmp708->tag != 0)goto _LL398;}_LL397:
 goto _LL395;_LL398: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp707;if(_tmp709->tag != 7)goto _LL39A;}_LL399:
 goto _LL39B;_LL39A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp707;if(_tmp70A->tag != 6)goto _LL39C;}_LL39B:
# 2866
{const char*_tmp9CF;void*_tmp9CE;(_tmp9CE=0,Cyc_Tcutil_warn(_tmp6D2,((_tmp9CF="function may not return a value",_tag_dyneither(_tmp9CF,sizeof(char),32))),_tag_dyneither(_tmp9CE,sizeof(void*),0)));}goto _LL395;_LL39C:;_LL39D:
# 2868
{const char*_tmp9D2;void*_tmp9D1;(_tmp9D1=0,Cyc_CfFlowInfo_aerr(_tmp6D2,((_tmp9D2="function may not return a value",_tag_dyneither(_tmp9D2,sizeof(char),32))),_tag_dyneither(_tmp9D1,sizeof(void*),0)));}goto _LL395;_LL395:;}
# 2870
goto _LL390;_LL390:;};};};}
# 2874
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp710=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp711=_tmp710;struct Cyc_Absyn_Fndecl*_tmp713;struct Cyc_List_List*_tmp715;struct Cyc_List_List*_tmp717;struct Cyc_List_List*_tmp719;_LL39F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp712=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp711;if(_tmp712->tag != 1)goto _LL3A1;else{_tmp713=_tmp712->f1;}}_LL3A0:
 Cyc_NewControlFlow_check_fun(_tmp713);goto _LL39E;_LL3A1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp714=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp711;if(_tmp714->tag != 11)goto _LL3A3;else{_tmp715=_tmp714->f1;}}_LL3A2:
 _tmp717=_tmp715;goto _LL3A4;_LL3A3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp716=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp711;if(_tmp716->tag != 10)goto _LL3A5;else{_tmp717=_tmp716->f2;}}_LL3A4:
 _tmp719=_tmp717;goto _LL3A6;_LL3A5: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp718=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp711;if(_tmp718->tag != 9)goto _LL3A7;else{_tmp719=_tmp718->f2;}}_LL3A6:
 Cyc_NewControlFlow_cf_check(_tmp719);goto _LL39E;_LL3A7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp71A=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp711;if(_tmp71A->tag != 12)goto _LL3A9;}_LL3A8:
 goto _LL39E;_LL3A9:;_LL3AA:
 goto _LL39E;_LL39E:;};}}
