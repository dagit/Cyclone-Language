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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
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
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 141
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 264
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 55
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 338
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 488
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 883
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 910
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 945
extern void*Cyc_Absyn_sint_typ;
# 947
void*Cyc_Absyn_float_typ(int);
# 970
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1093
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1104
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1111
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1125
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1131
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1154
int Cyc_Absyn_fntype_att(void*a);
# 1172
extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 115
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 122
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 137
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 294 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 71 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;};
# 91 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 93
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 96
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 105
return first;}}
# 110
int Cyc_Parse_no_register=0;struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 123
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 125
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 128
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 137
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 143
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 147
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 151
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct Cyc_Parse_Declarator _tmp1;struct Cyc_Absyn_Exp*_tmp2;struct _tuple11 _tmp0=ds->hd;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
{struct _tuple10*_tmp811;declarators=((_tmp811=_region_malloc(r,sizeof(*_tmp811)),((_tmp811->tl=declarators,((_tmp811->hd=_tmp1,_tmp811))))));}{
struct Cyc_List_List*_tmp812;exprs=((_tmp812=_region_malloc(r,sizeof(*_tmp812)),((_tmp812->hd=_tmp2,((_tmp812->tl=exprs,_tmp812))))));};}
# 158
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 165
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 168
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 170
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 175
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 178
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 181
static void*Cyc_Parse_parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp815;struct Cyc_Position_Exit_exn_struct*_tmp814;(int)_throw((void*)((_tmp814=_cycalloc_atomic(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=Cyc_Position_Exit,_tmp815)),_tmp814)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp81A;void*_tmp819[2];struct Cyc_String_pa_PrintArg_struct _tmp818;struct Cyc_String_pa_PrintArg_struct _tmp817;(_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp818.tag=0,((_tmp818.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp819[0]=& _tmp818,((_tmp819[1]=& _tmp817,Cyc_fprintf(Cyc_stderr,((_tmp81A="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp81A,sizeof(char),46))),_tag_dyneither(_tmp819,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_warn(loc,((_tmp81D="qualifier on type is ignored",_tag_dyneither(_tmp81D,sizeof(char),29))),_tag_dyneither(_tmp81C,sizeof(void*),0)));};}
# 199
return _tmpD;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 202
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 208
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp10=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmp18;void*_tmp1A;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp11=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp11->tag != 4)goto _LL3;}_LL2:
# 212
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp12=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp12->tag != 5)goto _LL5;}_LL4:
# 214
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp13->tag != 7)goto _LL7;}_LL6:
# 216
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp14->tag != 6)goto _LL9;}_LL8:
# 218
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp15->tag != 3)goto _LLB;}_LLA:
# 220
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp16->tag != 2)goto _LLD;}_LLC:
# 222
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp17->tag != 0)goto _LLF;else{_tmp18=_tmp17->f1;}}_LLE:
# 224
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp820;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp81F;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F[0]=((_tmp820.tag=1,((_tmp820.f1=_tmp18,_tmp820)))),_tmp81F)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp821;return(_tmp821.f1=nullable,((_tmp821.f2=bound,((_tmp821.f3=zeroterm,((_tmp821.f4=rgn,_tmp821)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp82E;const char*_tmp82D;void*_tmp82C[1];struct Cyc_Int_pa_PrintArg_struct _tmp82B;struct _tuple0*_tmp82A;return(_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A->f1=Cyc_Absyn_Rel_n(0),((_tmp82A->f2=(
(_tmp82E=_cycalloc(sizeof(*_tmp82E)),((_tmp82E[0]=(struct _dyneither_ptr)((_tmp82B.tag=1,((_tmp82B.f1=(unsigned long)enum_counter ++,((_tmp82C[0]=& _tmp82B,Cyc_aprintf(((_tmp82D="__anonymous_enum_%d__",_tag_dyneither(_tmp82D,sizeof(char),22))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))))),_tmp82E)))),_tmp82A)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct _tuple15*f1;struct Cyc_Absyn_Exp*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple16*field_info){
# 246
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct _tuple16*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=_tmp23->f2;
if(_tmp27 != 0){
const char*_tmp82F;Cyc_Parse_err(((_tmp82F="bad type params in struct field",_tag_dyneither(_tmp82F,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp830;Cyc_Parse_err(((_tmp830="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp830,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp831;return(_tmp831=_cycalloc(sizeof(*_tmp831)),((_tmp831->name=(*_tmp24).f2,((_tmp831->tq=_tmp25,((_tmp831->type=_tmp26,((_tmp831->width=_tmp29,((_tmp831->attributes=_tmp28,_tmp831)))))))))));};}
# 255
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp832;return(_tmp832.Signed_spec=0,((_tmp832.Unsigned_spec=0,((_tmp832.Short_spec=0,((_tmp832.Long_spec=0,((_tmp832.Long_Long_spec=0,((_tmp832.Valid_type_spec=0,((_tmp832.Type_spec=Cyc_Absyn_sint_typ,((_tmp832.loc=loc,_tmp832)))))))))))))));}
# 266
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2E=Cyc_Parse_empty_spec(loc);
_tmp2E.Type_spec=t;
_tmp2E.Valid_type_spec=1;
return _tmp2E;}
# 272
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2F=Cyc_Parse_empty_spec(loc);
_tmp2F.Signed_spec=1;
return _tmp2F;}
# 277
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp30=Cyc_Parse_empty_spec(loc);
_tmp30.Unsigned_spec=1;
return _tmp30;}
# 282
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Short_spec=1;
return _tmp31;}
# 287
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Long_spec=1;
return _tmp32;}
# 294
static void*Cyc_Parse_array2ptr(void*t,int argposn){
void*_tmp33=t;void*_tmp35;struct Cyc_Absyn_Tqual _tmp36;struct Cyc_Absyn_Exp*_tmp37;union Cyc_Absyn_Constraint*_tmp38;unsigned int _tmp39;_LL12: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 8)goto _LL14;else{_tmp35=(_tmp34->f1).elt_type;_tmp36=(_tmp34->f1).tq;_tmp37=(_tmp34->f1).num_elts;_tmp38=(_tmp34->f1).zero_term;_tmp39=(_tmp34->f1).zt_loc;}}_LL13: {
# 298
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp835;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp834;return Cyc_Absyn_starb_typ(_tmp35,
argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp36,
# 301
_tmp37 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((_tmp835.tag=1,((_tmp835.f1=_tmp37,_tmp835)))),_tmp834)))),_tmp38);}_LL14:;_LL15:
# 304
 return t;_LL11:;}struct _tuple17{struct _dyneither_ptr*f1;void*f2;};
# 317 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3C=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp3D=(struct _tuple8*)x->hd;struct _dyneither_ptr*_tmp3E;void*_tmp40;struct _dyneither_ptr _tmp41;void**_tmp44;_LL17: _tmp3E=_tmp3D->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3D->f3;if(_tmp3F->tag != 19)goto _LL19;else{_tmp40=(void*)_tmp3F->f1;}};if(!(_tmp3E != 0))goto _LL19;_LL18:
# 322
{void*_tmp45=_tmp40;void**_tmp47;_LL1E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45;if(_tmp46->tag != 1)goto _LL20;else{_tmp47=(void**)((void**)& _tmp46->f2);}}_LL1F: {
# 326
struct Cyc_String_pa_PrintArg_struct _tmp83D;void*_tmp83C[1];const char*_tmp83B;struct _dyneither_ptr*_tmp83A;struct _dyneither_ptr*nm=(_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=(struct _dyneither_ptr)((_tmp83D.tag=0,((_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E),((_tmp83C[0]=& _tmp83D,Cyc_aprintf(((_tmp83B="`%s",_tag_dyneither(_tmp83B,sizeof(char),4))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))))),_tmp83A)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp84C;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84B;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp84A;struct Cyc_Absyn_Tvar*_tmp849;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp848;*_tmp47=(void*)((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848[0]=((_tmp84C.tag=2,((_tmp84C.f1=((_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849->name=nm,((_tmp849->identity=- 1,((_tmp849->kind=(void*)((_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84A.tag=0,((_tmp84A.f1=& Cyc_Tcutil_ik,_tmp84A)))),_tmp84B)))),_tmp849)))))))),_tmp84C)))),_tmp848))));}
goto _LL1D;}_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 331
{struct _tuple17*_tmp84F;struct Cyc_List_List*_tmp84E;_tmp3C=((_tmp84E=_cycalloc(sizeof(*_tmp84E)),((_tmp84E->hd=((_tmp84F=_cycalloc(sizeof(*_tmp84F)),((_tmp84F->f1=_tmp3E,((_tmp84F->f2=_tmp40,_tmp84F)))))),((_tmp84E->tl=_tmp3C,_tmp84E))))));}goto _LL16;_LL19: if(_tmp3D->f1 == 0)goto _LL1B;_tmp41=*_tmp3D->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3D->f3;if(_tmp42->tag != 15)goto _LL1B;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp42->f1);if(_tmp43->tag != 1)goto _LL1B;else{_tmp44=(void**)((void**)& _tmp43->f2);}}};_LL1A: {
# 335
struct Cyc_String_pa_PrintArg_struct _tmp857;void*_tmp856[1];const char*_tmp855;struct _dyneither_ptr*_tmp854;struct _dyneither_ptr*nm=(_tmp854=_cycalloc(sizeof(*_tmp854)),((_tmp854[0]=(struct _dyneither_ptr)((_tmp857.tag=0,((_tmp857.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp856[0]=& _tmp857,Cyc_aprintf(((_tmp855="`%s",_tag_dyneither(_tmp855,sizeof(char),4))),_tag_dyneither(_tmp856,sizeof(void*),1)))))))),_tmp854)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp866;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp865;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp864;struct Cyc_Absyn_Tvar*_tmp863;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp862;*_tmp44=(void*)((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862[0]=((_tmp866.tag=2,((_tmp866.f1=((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863->name=nm,((_tmp863->identity=- 1,((_tmp863->kind=(void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp864.tag=0,((_tmp864.f1=& Cyc_Tcutil_rk,_tmp864)))),_tmp865)))),_tmp863)))))))),_tmp866)))),_tmp862))));}
goto _LL16;}_LL1B:;_LL1C:
 goto _LL16;_LL16:;}
# 341
return _tmp3C;}
# 345
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp5C=0;
for(0;x != 0;x=x->tl){
void*_tmp5D=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp5F;_LL23: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D;if(_tmp5E->tag != 19)goto _LL25;else{_tmp5F=(void*)_tmp5E->f1;}}_LL24:
# 350
{struct _tuple17*_tmp869;struct Cyc_List_List*_tmp868;_tmp5C=((_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868->hd=((_tmp869=_cycalloc(sizeof(*_tmp869)),((_tmp869->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp869->f2=_tmp5F,_tmp869)))))),((_tmp868->tl=_tmp5C,_tmp868))))));}goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 354
return _tmp5C;}
# 358
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp62=e->r;struct _dyneither_ptr*_tmp64;_LL28: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp62;if(_tmp63->tag != 1)goto _LL2A;else{if((((_tmp63->f1)->f1).Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(((_tmp63->f1)->f1).Rel_n).val != 0)goto _LL2A;_tmp64=(_tmp63->f1)->f2;}}_LL29:
# 361
{struct Cyc_List_List*_tmp65=tags;for(0;_tmp65 != 0;_tmp65=_tmp65->tl){
struct _dyneither_ptr*_tmp67;void*_tmp68;struct _tuple17*_tmp66=(struct _tuple17*)_tmp65->hd;_tmp67=_tmp66->f1;_tmp68=_tmp66->f2;
if(Cyc_strptrcmp(_tmp67,_tmp64)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp86C;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp86B;return Cyc_Absyn_new_exp((void*)((_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86C.tag=37,((_tmp86C.f1=Cyc_Tcutil_copy_type(_tmp68),_tmp86C)))),_tmp86B)))),e->loc);}}}
# 366
goto _LL27;_LL2A:;_LL2B:
 goto _LL27;_LL27:;}
# 369
return e;}
# 374
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp6B=t;void*_tmp6D;struct Cyc_Absyn_Tqual _tmp6E;struct Cyc_Absyn_Exp*_tmp6F;union Cyc_Absyn_Constraint*_tmp70;unsigned int _tmp71;void*_tmp73;struct Cyc_Absyn_Tqual _tmp74;void*_tmp75;union Cyc_Absyn_Constraint*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;struct Cyc_Absyn_PtrLoc*_tmp79;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B;if(_tmp6C->tag != 8)goto _LL2F;else{_tmp6D=(_tmp6C->f1).elt_type;_tmp6E=(_tmp6C->f1).tq;_tmp6F=(_tmp6C->f1).num_elts;_tmp70=(_tmp6C->f1).zero_term;_tmp71=(_tmp6C->f1).zt_loc;}}_LL2E: {
# 377
struct Cyc_Absyn_Exp*nelts2=_tmp6F;
if(_tmp6F != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6F);{
# 381
void*_tmp7A=Cyc_Parse_substitute_tags(tags,_tmp6D);
if(_tmp6F != nelts2  || _tmp6D != _tmp7A){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp872;struct Cyc_Absyn_ArrayInfo _tmp871;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp870;return(void*)((_tmp870=_cycalloc(sizeof(*_tmp870)),((_tmp870[0]=((_tmp872.tag=8,((_tmp872.f1=((_tmp871.elt_type=_tmp7A,((_tmp871.tq=_tmp6E,((_tmp871.num_elts=nelts2,((_tmp871.zero_term=_tmp70,((_tmp871.zt_loc=_tmp71,_tmp871)))))))))),_tmp872)))),_tmp870))));}
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B;if(_tmp72->tag != 5)goto _LL31;else{_tmp73=(_tmp72->f1).elt_typ;_tmp74=(_tmp72->f1).elt_tq;_tmp75=((_tmp72->f1).ptr_atts).rgn;_tmp76=((_tmp72->f1).ptr_atts).nullable;_tmp77=((_tmp72->f1).ptr_atts).bounds;_tmp78=((_tmp72->f1).ptr_atts).zero_term;_tmp79=((_tmp72->f1).ptr_atts).ptrloc;}}_LL30: {
# 386
void*_tmp7E=Cyc_Parse_substitute_tags(tags,_tmp73);
union Cyc_Absyn_Constraint*_tmp7F=_tmp77;
{union Cyc_Absyn_Constraint _tmp80=*_tmp77;struct Cyc_Absyn_Exp*_tmp82;_LL34: if((_tmp80.Eq_constr).tag != 1)goto _LL36;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp81=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp80.Eq_constr).val);if(_tmp81->tag != 1)goto _LL36;else{_tmp82=_tmp81->f1;}};_LL35:
# 390
 _tmp82=Cyc_Parse_substitute_tags_exp(tags,_tmp82);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp878;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp877;union Cyc_Absyn_Constraint*_tmp876;_tmp7F=((_tmp876=_cycalloc(sizeof(*_tmp876)),(((_tmp876->Eq_constr).val=(void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp877.tag=1,((_tmp877.f1=_tmp82,_tmp877)))),_tmp878)))),(((_tmp876->Eq_constr).tag=1,_tmp876))))));}
goto _LL33;_LL36:;_LL37:
 goto _LL33;_LL33:;}
# 395
if(_tmp7E != _tmp73  || _tmp7F != _tmp77){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp87E;struct Cyc_Absyn_PtrInfo _tmp87D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87C;return(void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp87E.tag=5,((_tmp87E.f1=((_tmp87D.elt_typ=_tmp7E,((_tmp87D.elt_tq=_tmp74,((_tmp87D.ptr_atts=(((_tmp87D.ptr_atts).rgn=_tmp75,(((_tmp87D.ptr_atts).nullable=_tmp76,(((_tmp87D.ptr_atts).bounds=_tmp7F,(((_tmp87D.ptr_atts).zero_term=_tmp78,(((_tmp87D.ptr_atts).ptrloc=_tmp79,_tmp87D.ptr_atts)))))))))),_tmp87D)))))),_tmp87E)))),_tmp87C))));}
goto _LL2C;}_LL31:;_LL32:
# 400
 goto _LL2C;_LL2C:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple18*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 416
struct _tuple18*_tmp87F;return(_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F->f1=(*t).f2,((_tmp87F->f2=(*t).f3,_tmp87F)))));}
# 419
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp8A=d->r;struct Cyc_Absyn_Vardecl*_tmp8C;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL3B;else{_tmp8C=_tmp8B->f1;}}_LL3A:
 return _tmp8C;_LL3B:;_LL3C: {
const char*_tmp882;void*_tmp881;(_tmp881=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp882="bad declaration in `forarray' statement",_tag_dyneither(_tmp882,sizeof(char),40))),_tag_dyneither(_tmp881,sizeof(void*),0)));}_LL38:;}
# 426
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL40;}_LL3F:
 return 1;_LL40:;_LL41:
 return 0;_LL3D:;}
# 435
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp883;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp883="`H",_tag_dyneither(_tmp883,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp884;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp884="`U",_tag_dyneither(_tmp884,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp885;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp885="`RC",_tag_dyneither(_tmp885,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 443
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88F;struct _dyneither_ptr*_tmp88E;struct Cyc_Absyn_Tvar*_tmp88D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp88C;return(void*)((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88F.tag=2,((_tmp88F.f1=((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D->name=((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=s,_tmp88E)))),((_tmp88D->identity=- 1,((_tmp88D->kind=k,_tmp88D)))))))),_tmp88F)))),_tmp88C))));}}}}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmp9B;void*_tmp9C;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp99=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL45;}_LL44:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp892;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp891;k=(void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((_tmp892.tag=1,((_tmp892.f1=0,_tmp892)))),_tmp891))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98;if(_tmp9A->tag != 2)goto _LL47;else{_tmp9B=_tmp9A->f2;}}_LL46:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp895;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp894;k=(void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=2,((_tmp895.f1=0,((_tmp895.f2=_tmp9B,_tmp895)))))),_tmp894))));}goto _LL42;_LL47: _tmp9C=_tmp98;_LL48:
 k=_tmp9C;goto _LL42;_LL42:;}{
# 453
struct Cyc_Absyn_Tvar*_tmp896;return(_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896->name=t->name,((_tmp896->identity=- 1,((_tmp896->kind=k,_tmp896)))))));};}
# 460
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA2=t;struct Cyc_Absyn_Tvar*_tmpA4;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2;if(_tmpA3->tag != 2)goto _LL4C;else{_tmpA4=_tmpA3->f1;}}_LL4B:
 return _tmpA4;_LL4C:;_LL4D: {
const char*_tmp899;void*_tmp898;(_tmp898=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp899="expecting a list of type variables, not types",_tag_dyneither(_tmp899,sizeof(char),46))),_tag_dyneither(_tmp898,sizeof(void*),0)));}_LL49:;}
# 466
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp89C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89B;return(void*)((_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89C.tag=2,((_tmp89C.f1=pr,_tmp89C)))),_tmp89B))));}
# 471
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpA9=Cyc_Tcutil_compress(t);void**_tmpAB;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA9;if(_tmpAA->tag != 2)goto _LL51;else{_tmpAB=(void**)&(_tmpAA->f1)->kind;}}_LL50: {
# 474
void*_tmpAC=Cyc_Absyn_compress_kb(*_tmpAB);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAD=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAC;if(_tmpAD->tag != 1)goto _LL56;}_LL55:
# 476
 if(!leq)*_tmpAB=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp89F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp89E;*_tmpAB=(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=2,((_tmp89F.f1=0,((_tmp89F.f2=k,_tmp89F)))))),_tmp89E))));}
return;_LL56:;_LL57:
 return;_LL53:;}_LL51:;_LL52:
# 481
 return;_LL4E:;}
# 486
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 492
if(tds == 0)return tms;
# 497
if(tms == 0)return 0;{
# 499
void*_tmpB0=(void*)tms->hd;void*_tmpB2;_LL59: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB1=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB0;if(_tmpB1->tag != 3)goto _LL5B;else{_tmpB2=(void*)_tmpB1->f1;}}_LL5A:
# 502
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 505
void*_tmpB3=_tmpB2;struct Cyc_List_List*_tmpB6;_LL5E: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB4=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB3;if(_tmpB4->tag != 1)goto _LL60;}_LL5F:
# 507
{const char*_tmp8A2;void*_tmp8A1;(_tmp8A1=0,Cyc_Tcutil_warn(loc,((_tmp8A2="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8A2,sizeof(char),93))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}
# 509
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB5=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB3;if(_tmpB5->tag != 0)goto _LL5D;else{_tmpB6=_tmpB5->f1;}}_LL61:
# 511
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8A5="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8A5,sizeof(char),73))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}{
# 515
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpB6 != 0;_tmpB6=_tmpB6->tl){
struct Cyc_List_List*_tmpBB=tds;
for(0;_tmpBB != 0;_tmpBB=_tmpBB->tl){
struct Cyc_Absyn_Decl*_tmpBC=(struct Cyc_Absyn_Decl*)_tmpBB->hd;
void*_tmpBD=_tmpBC->r;struct Cyc_Absyn_Vardecl*_tmpBF;_LL63: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBD;if(_tmpBE->tag != 0)goto _LL65;else{_tmpBF=_tmpBE->f1;}}_LL64:
# 522
 if(Cyc_zstrptrcmp((*_tmpBF->name).f2,(struct _dyneither_ptr*)_tmpB6->hd)!= 0)
continue;
if(_tmpBF->initializer != 0){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8A8="initializer found in parameter declaration",_tag_dyneither(_tmp8A8,sizeof(char),43))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpBF->name)){
const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8AB="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8AB,sizeof(char),47))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}
{struct _tuple8*_tmp8AE;struct Cyc_List_List*_tmp8AD;rev_new_params=(
(_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD->hd=((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->f1=(*_tmpBF->name).f2,((_tmp8AE->f2=_tmpBF->tq,((_tmp8AE->f3=_tmpBF->type,_tmp8AE)))))))),((_tmp8AD->tl=rev_new_params,_tmp8AD))))));}
# 531
goto L;_LL65:;_LL66: {
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8B1="nonvariable declaration in parameter type",_tag_dyneither(_tmp8B1,sizeof(char),42))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}_LL62:;}
# 535
L: if(_tmpBB == 0){
const char*_tmp8B5;void*_tmp8B4[1];struct Cyc_String_pa_PrintArg_struct _tmp8B3;(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpB6->hd)),((_tmp8B4[0]=& _tmp8B3,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B5="%s is not given a type",_tag_dyneither(_tmp8B5,sizeof(char),23))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))));}}{
# 538
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8C4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8C2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8C1;struct Cyc_List_List*_tmp8C0;return
(_tmp8C0=_region_malloc(yy,sizeof(*_tmp8C0)),((_tmp8C0->hd=(void*)((_tmp8C4=_region_malloc(yy,sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C1.tag=3,((_tmp8C1.f1=(void*)((_tmp8C3=_region_malloc(yy,sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C2.tag=1,((_tmp8C2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8C2.f2=0,((_tmp8C2.f3=0,((_tmp8C2.f4=0,((_tmp8C2.f5=0,_tmp8C2)))))))))))),_tmp8C3)))),_tmp8C1)))),_tmp8C4)))),((_tmp8C0->tl=0,_tmp8C0)))));};};_LL5D:;}
# 545
goto _LL5C;_LL5B:;_LL5C: {
struct Cyc_List_List*_tmp8C5;return(_tmp8C5=_region_malloc(yy,sizeof(*_tmp8C5)),((_tmp8C5->hd=(void*)tms->hd,((_tmp8C5->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8C5)))));}_LL58:;};}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 557
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8C6;d=((_tmp8C6.id=d.id,((_tmp8C6.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8C6))));}{
# 560
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 566
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 572
if(dso->sc != 0)
switch(*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc))){case Cyc_Parse_Extern_sc: _LL67:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL68:
 sc=Cyc_Absyn_Static;break;default: _LL69:
{const char*_tmp8C7;Cyc_Parse_err(((_tmp8C7="bad storage class on function",_tag_dyneither(_tmp8C7,sizeof(char),30))),loc);}break;}}{
# 579
void*_tmpD3=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct Cyc_Absyn_Tqual _tmpD5;void*_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple13 _tmpD4=Cyc_Parse_apply_tms(tq,_tmpD3,atts,d.tms);_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;_tmpD7=_tmpD4.f3;_tmpD8=_tmpD4.f4;
# 584
if(_tmpD7 != 0){
# 587
const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,Cyc_Tcutil_warn(loc,((_tmp8CA="bad type params, ignoring",_tag_dyneither(_tmp8CA,sizeof(char),26))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}{
# 589
void*_tmpDB=_tmpD6;struct Cyc_List_List*_tmpDD;void*_tmpDE;struct Cyc_Absyn_Tqual _tmpDF;void*_tmpE0;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*_tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 9)goto _LL6E;else{_tmpDD=(_tmpDC->f1).tvars;_tmpDE=(_tmpDC->f1).effect;_tmpDF=(_tmpDC->f1).ret_tqual;_tmpE0=(_tmpDC->f1).ret_typ;_tmpE1=(_tmpDC->f1).args;_tmpE2=(_tmpDC->f1).c_varargs;_tmpE3=(_tmpDC->f1).cyc_varargs;_tmpE4=(_tmpDC->f1).rgn_po;_tmpE5=(_tmpDC->f1).attributes;}}_LL6D: {
# 592
struct Cyc_List_List*_tmpE6=0;
{struct Cyc_List_List*_tmpE7=_tmpE1;for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
struct _dyneither_ptr*_tmpE9;struct Cyc_Absyn_Tqual _tmpEA;void*_tmpEB;struct _tuple8*_tmpE8=(struct _tuple8*)_tmpE7->hd;_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpEB=_tmpE8->f3;
if(_tmpE9 == 0){
{const char*_tmp8CB;Cyc_Parse_err(((_tmp8CB="missing argument variable in function prototype",_tag_dyneither(_tmp8CB,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8CE;struct Cyc_List_List*_tmp8CD;_tmpE6=((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD->hd=((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8CE->f2=_tmpEA,((_tmp8CE->f3=_tmpEB,_tmp8CE)))))))),((_tmp8CD->tl=_tmpE6,_tmp8CD))))));};}else{
# 599
struct _tuple8*_tmp8D1;struct Cyc_List_List*_tmp8D0;_tmpE6=((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0->hd=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1->f1=_tmpE9,((_tmp8D1->f2=_tmpEA,((_tmp8D1->f3=_tmpEB,_tmp8D1)))))))),((_tmp8D0->tl=_tmpE6,_tmp8D0))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp8D2;return(_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2->sc=sc,((_tmp8D2->name=d.id,((_tmp8D2->tvs=_tmpDD,((_tmp8D2->is_inline=is_inline,((_tmp8D2->effect=_tmpDE,((_tmp8D2->ret_tqual=_tmpDF,((_tmp8D2->ret_type=_tmpE0,((_tmp8D2->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE6),((_tmp8D2->c_varargs=_tmpE2,((_tmp8D2->cyc_varargs=_tmpE3,((_tmp8D2->rgn_po=_tmpE4,((_tmp8D2->body=body,((_tmp8D2->cached_typ=0,((_tmp8D2->param_vardecls=0,((_tmp8D2->fn_vardecl=0,((_tmp8D2->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE5,_tmpD8),_tmp8D2)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
const char*_tmp8D5;void*_tmp8D4;(_tmp8D4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8D5="declarator is not a function prototype",_tag_dyneither(_tmp8D5,sizeof(char),39))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}_LL6B:;};};};}static char _tmpF5[52]="at most one type may appear within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpF5,_tmpF5,_tmpF5 + 52};static char _tmpF6[63]="const or volatile may appear only once within a type specifier";
# 620
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpF6,_tmpF6,_tmpF6 + 63};static char _tmpF7[50]="type specifier includes more than one declaration";
# 622
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpF7,_tmpF7,_tmpF7 + 50};static char _tmpF8[60]="sign specifier may appear only once within a type specifier";
# 624
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpF8,_tmpF8,_tmpF8 + 60};
# 631
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 634
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp8D6;(_tmp8D6=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D6,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8D7;(_tmp8D7=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D7,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8D8;(_tmp8D8=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D8,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8D9;(_tmp8D9=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D9,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 656
return s1;}
# 662
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 665
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 672
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 684
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,Cyc_Tcutil_warn(loc,((_tmp8DC="missing type within specifier",_tag_dyneither(_tmp8DC,sizeof(char),30))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 689
if(seen_sign){
void*_tmpFF=t;enum Cyc_Absyn_Sign _tmp101;enum Cyc_Absyn_Size_of _tmp102;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFF;if(_tmp100->tag != 6)goto _LL73;else{_tmp101=_tmp100->f1;_tmp102=_tmp100->f2;}}_LL72:
# 692
 if(_tmp101 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp102);
goto _LL70;_LL73:;_LL74:
{const char*_tmp8DD;Cyc_Parse_err(((_tmp8DD="sign specification on non-integral type",_tag_dyneither(_tmp8DD,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 697
if(seen_size){
void*_tmp104=t;enum Cyc_Absyn_Sign _tmp106;enum Cyc_Absyn_Size_of _tmp107;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp105=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp104;if(_tmp105->tag != 6)goto _LL78;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;}}_LL77:
# 700
 if(_tmp107 != sz)
t=Cyc_Absyn_int_typ(_tmp106,sz);
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp104;if(_tmp108->tag != 7)goto _LL7A;}_LL79:
# 704
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
{const char*_tmp8DE;Cyc_Parse_err(((_tmp8DE="size qualifier on non-integral type",_tag_dyneither(_tmp8DE,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
# 708
return t;}
# 711
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 715
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp10A=d.id;
struct Cyc_Absyn_Tqual _tmp10C;void*_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple13 _tmp10B=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;_tmp10E=_tmp10B.f3;_tmp10F=_tmp10B.f4;
# 721
if(ds->tl == 0){
struct _tuple15*_tmp8E1;struct Cyc_List_List*_tmp8E0;return(_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0->hd=((_tmp8E1=_region_malloc(r,sizeof(*_tmp8E1)),((_tmp8E1->f1=_tmp10A,((_tmp8E1->f2=_tmp10C,((_tmp8E1->f3=_tmp10D,((_tmp8E1->f4=_tmp10E,((_tmp8E1->f5=_tmp10F,_tmp8E1)))))))))))),((_tmp8E0->tl=0,_tmp8E0)))));}else{
# 724
struct _tuple15*_tmp8E4;struct Cyc_List_List*_tmp8E3;return(_tmp8E3=_region_malloc(r,sizeof(*_tmp8E3)),((_tmp8E3->hd=((_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4->f1=_tmp10A,((_tmp8E4->f2=_tmp10C,((_tmp8E4->f3=_tmp10D,((_tmp8E4->f4=_tmp10E,((_tmp8E4->f5=_tmp10F,_tmp8E4)))))))))))),((_tmp8E3->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8E3)))));}};}
# 728
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 731
if(tms == 0){struct _tuple13 _tmp8E5;return(_tmp8E5.f1=tq,((_tmp8E5.f2=t,((_tmp8E5.f3=0,((_tmp8E5.f4=atts,_tmp8E5)))))));}{
void*_tmp115=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp117;unsigned int _tmp118;struct Cyc_Absyn_Exp*_tmp11A;union Cyc_Absyn_Constraint*_tmp11B;unsigned int _tmp11C;void*_tmp11E;struct Cyc_List_List*_tmp120;unsigned int _tmp121;struct Cyc_Absyn_PtrAtts _tmp123;struct Cyc_Absyn_Tqual _tmp124;unsigned int _tmp126;struct Cyc_List_List*_tmp127;_LL7D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp116=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp116->tag != 0)goto _LL7F;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;}}_LL7E:
# 734
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp117,_tmp118),atts,tms->tl);_LL7F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp119=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp119->tag != 1)goto _LL81;else{_tmp11A=_tmp119->f1;_tmp11B=_tmp119->f2;_tmp11C=_tmp119->f3;}}_LL80:
# 737
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp11A,_tmp11B,_tmp11C),atts,tms->tl);_LL81: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp11D=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp11D->tag != 3)goto _LL83;else{_tmp11E=(void*)_tmp11D->f1;}}_LL82: {
# 740
void*_tmp128=_tmp11E;struct Cyc_List_List*_tmp12A;int _tmp12B;struct Cyc_Absyn_VarargInfo*_tmp12C;void*_tmp12D;struct Cyc_List_List*_tmp12E;unsigned int _tmp130;_LL8A: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp129=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp128;if(_tmp129->tag != 1)goto _LL8C;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;_tmp12C=_tmp129->f3;_tmp12D=(void*)_tmp129->f4;_tmp12E=_tmp129->f5;}}_LL8B: {
# 742
struct Cyc_List_List*typvars=0;
# 744
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp8E6;fn_atts=((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6->hd=(void*)as->hd,((_tmp8E6->tl=fn_atts,_tmp8E6))))));}else{
# 749
struct Cyc_List_List*_tmp8E7;new_atts=((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7->hd=(void*)as->hd,((_tmp8E7->tl=new_atts,_tmp8E7))))));}}}
# 752
if(tms->tl != 0){
void*_tmp133=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp135;_LL8F: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp134=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp133;if(_tmp134->tag != 4)goto _LL91;else{_tmp135=_tmp134->f1;}}_LL90:
# 755
 typvars=_tmp135;
tms=tms->tl;
goto _LL8E;_LL91:;_LL92:
# 759
 goto _LL8E;_LL8E:;}
# 763
if(((((!_tmp12B  && _tmp12C == 0) && _tmp12A != 0) && _tmp12A->tl == 0) && (*((struct _tuple8*)_tmp12A->hd)).f1 == 0) && (*((struct _tuple8*)_tmp12A->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 768
_tmp12A=0;{
# 771
struct Cyc_List_List*_tmp136=Cyc_Parse_get_arg_tags(_tmp12A);
# 773
if(_tmp136 != 0)
t=Cyc_Parse_substitute_tags(_tmp136,t);
t=Cyc_Parse_array2ptr(t,0);
# 778
{struct Cyc_List_List*_tmp137=_tmp12A;for(0;_tmp137 != 0;_tmp137=_tmp137->tl){
struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Tqual _tmp13A;void**_tmp13B;struct _tuple8*_tmp138=(struct _tuple8*)_tmp137->hd;_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;_tmp13B=(void**)& _tmp138->f3;
if(_tmp136 != 0)
*_tmp13B=Cyc_Parse_substitute_tags(_tmp136,*_tmp13B);
*_tmp13B=Cyc_Parse_array2ptr(*_tmp13B,1);}}
# 790
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp12D,tq,t,_tmp12A,_tmp12B,_tmp12C,_tmp12E,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL8C: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp12F=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp128;if(_tmp12F->tag != 0)goto _LL89;else{_tmp130=_tmp12F->f2;}}_LL8D: {
# 796
const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp130,((_tmp8EA="function declaration without parameter types",_tag_dyneither(_tmp8EA,sizeof(char),45))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp11F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp11F->tag != 4)goto _LL85;else{_tmp120=_tmp11F->f1;_tmp121=_tmp11F->f2;}}_LL84:
# 803
 if(tms->tl == 0){
struct _tuple13 _tmp8EB;return(_tmp8EB.f1=tq,((_tmp8EB.f2=t,((_tmp8EB.f3=_tmp120,((_tmp8EB.f4=atts,_tmp8EB)))))));}{
# 808
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp121,((_tmp8EE="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp8EE,sizeof(char),68))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp122=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp122->tag != 2)goto _LL87;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL86: {
# 811
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8F4;struct Cyc_Absyn_PtrInfo _tmp8F3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F2;return Cyc_Parse_apply_tms(_tmp124,(void*)((_tmp8F2=_cycalloc(sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F4.tag=5,((_tmp8F4.f1=((_tmp8F3.elt_typ=t,((_tmp8F3.elt_tq=tq,((_tmp8F3.ptr_atts=_tmp123,_tmp8F3)))))),_tmp8F4)))),_tmp8F2)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp125=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp125->tag != 5)goto _LL7C;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL88:
# 816
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp127),tms->tl);_LL7C:;};}
# 822
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 831
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp145;struct Cyc_Absyn_Tqual _tmp146;void*_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp149;struct _tuple15*_tmp144=t;_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;_tmp147=_tmp144->f3;_tmp148=_tmp144->f4;_tmp149=_tmp144->f5;
# 834
Cyc_Lex_register_typedef(_tmp145);{
# 836
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp14A=_tmp147;struct Cyc_Core_Opt*_tmp14C;_LL94: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14A;if(_tmp14B->tag != 1)goto _LL96;else{_tmp14C=_tmp14B->f1;}}_LL95:
# 840
 type=0;
if(_tmp14C == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp14C;}
goto _LL93;_LL96:;_LL97:
 kind=0;type=_tmp147;goto _LL93;_LL93:;}{
# 846
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8FA;struct Cyc_Absyn_Typedefdecl*_tmp8F9;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp8F8;return Cyc_Absyn_new_decl((void*)((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8FA.tag=9,((_tmp8FA.f1=((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->name=_tmp145,((_tmp8F9->tvs=_tmp148,((_tmp8F9->kind=kind,((_tmp8F9->defn=type,((_tmp8F9->atts=_tmp149,((_tmp8F9->tq=_tmp146,_tmp8F9)))))))))))))),_tmp8FA)))),_tmp8F8)))),loc);};};}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp8FD;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp8FC;return Cyc_Absyn_new_stmt((void*)((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=12,((_tmp8FD.f1=d,((_tmp8FD.f2=s,_tmp8FD)))))),_tmp8FC)))),d->loc);}
# 859
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 868
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 872
struct _RegionHandle _tmp152=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp152;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp154;struct Cyc_Parse_Type_specifier _tmp155;struct Cyc_List_List*_tmp156;struct Cyc_Parse_Declaration_spec _tmp153=ds;_tmp154=_tmp153.tq;_tmp155=_tmp153.type_specs;_tmp156=_tmp153.attributes;
if(_tmp154.loc == 0)_tmp154.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp8FE;Cyc_Parse_err(((_tmp8FE="inline is allowed only on function definitions",_tag_dyneither(_tmp8FE,sizeof(char),47))),loc);}{
# 878
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0)
switch(*ds.sc){case Cyc_Parse_Typedef_sc: _LL98:
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL99:
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL9A:
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL9B:
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL9C:
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL9D:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL9E:
 s=Cyc_Absyn_Abstract;break;}{
# 894
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 898
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 906
void*_tmp158=Cyc_Parse_collapse_type_specifiers(_tmp155,loc);
if(declarators == 0){
# 910
void*_tmp159=_tmp158;struct Cyc_Absyn_Aggrdecl*_tmp15C;struct Cyc_Absyn_Enumdecl*_tmp15F;struct Cyc_Absyn_Datatypedecl*_tmp162;enum Cyc_Absyn_AggrKind _tmp164;struct _tuple0*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Datatypedecl**_tmp168;struct _tuple0*_tmp16A;int _tmp16B;struct Cyc_List_List*_tmp16C;struct _tuple0*_tmp16E;struct Cyc_List_List*_tmp170;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp15A->tag != 26)goto _LLA3;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp15B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp15A->f1)->r;if(_tmp15B->tag != 0)goto _LLA3;else{_tmp15C=_tmp15B->f1;}}}_LLA2:
# 912
 _tmp15C->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp15C->attributes,_tmp156);
_tmp15C->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp904;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp903;struct Cyc_List_List*_tmp902;struct Cyc_List_List*_tmp174=(_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->hd=Cyc_Absyn_new_decl((void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp903.tag=6,((_tmp903.f1=_tmp15C,_tmp903)))),_tmp904)))),loc),((_tmp902->tl=0,_tmp902)))));_npop_handler(0);return _tmp174;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp15D->tag != 26)goto _LLA5;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp15E=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp15D->f1)->r;if(_tmp15E->tag != 1)goto _LLA5;else{_tmp15F=_tmp15E->f1;}}}_LLA4:
# 916
 if(_tmp156 != 0){const char*_tmp905;Cyc_Parse_err(((_tmp905="attributes on enum not supported",_tag_dyneither(_tmp905,sizeof(char),33))),loc);}
_tmp15F->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp90B;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp90A;struct Cyc_List_List*_tmp909;struct Cyc_List_List*_tmp179=(_tmp909=_cycalloc(sizeof(*_tmp909)),((_tmp909->hd=Cyc_Absyn_new_decl((void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90A.tag=8,((_tmp90A.f1=_tmp15F,_tmp90A)))),_tmp90B)))),loc),((_tmp909->tl=0,_tmp909)))));_npop_handler(0);return _tmp179;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp160->tag != 26)goto _LLA7;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp161=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp160->f1)->r;if(_tmp161->tag != 2)goto _LLA7;else{_tmp162=_tmp161->f1;}}}_LLA6:
# 920
 if(_tmp156 != 0){const char*_tmp90C;Cyc_Parse_err(((_tmp90C="attributes on datatypes not supported",_tag_dyneither(_tmp90C,sizeof(char),38))),loc);}
_tmp162->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp912;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp911;struct Cyc_List_List*_tmp910;struct Cyc_List_List*_tmp17E=(_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910->hd=Cyc_Absyn_new_decl((void*)((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp911.tag=7,((_tmp911.f1=_tmp162,_tmp911)))),_tmp912)))),loc),((_tmp910->tl=0,_tmp910)))));_npop_handler(0);return _tmp17E;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp159;if(_tmp163->tag != 11)goto _LLA9;else{if((((_tmp163->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA9;_tmp164=((struct _tuple2)(((_tmp163->f1).aggr_info).UnknownAggr).val).f1;_tmp165=((struct _tuple2)(((_tmp163->f1).aggr_info).UnknownAggr).val).f2;_tmp166=(_tmp163->f1).targs;}}_LLA8: {
# 924
struct Cyc_List_List*_tmp17F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp166);
struct Cyc_Absyn_Aggrdecl*_tmp913;struct Cyc_Absyn_Aggrdecl*_tmp180=(_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->kind=_tmp164,((_tmp913->sc=s,((_tmp913->name=_tmp165,((_tmp913->tvs=_tmp17F,((_tmp913->impl=0,((_tmp913->attributes=0,_tmp913)))))))))))));
if(_tmp156 != 0){const char*_tmp914;Cyc_Parse_err(((_tmp914="bad attributes on type declaration",_tag_dyneither(_tmp914,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp91A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp919;struct Cyc_List_List*_tmp918;struct Cyc_List_List*_tmp185=(_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->hd=Cyc_Absyn_new_decl((void*)((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp919.tag=6,((_tmp919.f1=_tmp180,_tmp919)))),_tmp91A)))),loc),((_tmp918->tl=0,_tmp918)))));_npop_handler(0);return _tmp185;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159;if(_tmp167->tag != 3)goto _LLAB;else{if((((_tmp167->f1).datatype_info).KnownDatatype).tag != 2)goto _LLAB;_tmp168=(struct Cyc_Absyn_Datatypedecl**)(((_tmp167->f1).datatype_info).KnownDatatype).val;}}_LLAA:
# 929
 if(_tmp156 != 0){const char*_tmp91B;Cyc_Parse_err(((_tmp91B="bad attributes on datatype",_tag_dyneither(_tmp91B,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp921;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp920;struct Cyc_List_List*_tmp91F;struct Cyc_List_List*_tmp18B=(_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F->hd=Cyc_Absyn_new_decl((void*)((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921[0]=((_tmp920.tag=7,((_tmp920.f1=*_tmp168,_tmp920)))),_tmp921)))),loc),((_tmp91F->tl=0,_tmp91F)))));_npop_handler(0);return _tmp18B;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159;if(_tmp169->tag != 3)goto _LLAD;else{if((((_tmp169->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLAD;_tmp16A=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp169->f1).datatype_info).UnknownDatatype).val).name;_tmp16B=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp169->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp16C=(_tmp169->f1).targs;}}_LLAC: {
# 932
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp16C);
struct Cyc_Absyn_Decl*_tmp18D=Cyc_Absyn_datatype_decl(s,_tmp16A,_tmp18C,0,_tmp16B,loc);
if(_tmp156 != 0){const char*_tmp922;Cyc_Parse_err(((_tmp922="bad attributes on datatype",_tag_dyneither(_tmp922,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp923;struct Cyc_List_List*_tmp190=(_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923->hd=_tmp18D,((_tmp923->tl=0,_tmp923)))));_npop_handler(0);return _tmp190;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp159;if(_tmp16D->tag != 13)goto _LLAF;else{_tmp16E=_tmp16D->f1;}}_LLAE: {
# 937
struct Cyc_Absyn_Enumdecl*_tmp924;struct Cyc_Absyn_Enumdecl*_tmp191=(_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->sc=s,((_tmp924->name=_tmp16E,((_tmp924->fields=0,_tmp924)))))));
if(_tmp156 != 0){const char*_tmp925;Cyc_Parse_err(((_tmp925="bad attributes on enum",_tag_dyneither(_tmp925,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp92F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp92E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp92D;struct Cyc_List_List*_tmp92C;struct Cyc_List_List*_tmp197=(_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->hd=((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->r=(void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=8,((_tmp92E.f1=_tmp191,_tmp92E)))),_tmp92D)))),((_tmp92F->loc=loc,_tmp92F)))))),((_tmp92C->tl=0,_tmp92C)))));_npop_handler(0);return _tmp197;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp159;if(_tmp16F->tag != 14)goto _LLB1;else{_tmp170=_tmp16F->f1;}}_LLB0: {
# 943
struct Cyc_Core_Opt*_tmp932;struct Cyc_Absyn_Enumdecl*_tmp931;struct Cyc_Absyn_Enumdecl*_tmp199=(_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->sc=s,((_tmp931->name=Cyc_Parse_gensym_enum(),((_tmp931->fields=((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->v=_tmp170,_tmp932)))),_tmp931)))))));
if(_tmp156 != 0){const char*_tmp933;Cyc_Parse_err(((_tmp933="bad attributes on enum",_tag_dyneither(_tmp933,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp93D;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp93C;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp93B;struct Cyc_List_List*_tmp93A;struct Cyc_List_List*_tmp19F=(_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->hd=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->r=(void*)((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=8,((_tmp93C.f1=_tmp199,_tmp93C)))),_tmp93B)))),((_tmp93D->loc=loc,_tmp93D)))))),((_tmp93A->tl=0,_tmp93A)))));_npop_handler(0);return _tmp19F;};}_LLB1:;_LLB2:
{const char*_tmp93E;Cyc_Parse_err(((_tmp93E="missing declarator",_tag_dyneither(_tmp93E,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1A3=0;_npop_handler(0);return _tmp1A3;};_LLA0:;}else{
# 950
struct Cyc_List_List*_tmp1A4=Cyc_Parse_apply_tmss(mkrgn,_tmp154,_tmp158,declarators,_tmp156);
if(istypedef){
# 955
if(!exps_empty){
const char*_tmp93F;Cyc_Parse_err(((_tmp93F="initializer in typedef declaration",_tag_dyneither(_tmp93F,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1A4);
struct Cyc_List_List*_tmp1A6=decls;_npop_handler(0);return _tmp1A6;};}else{
# 961
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1A7=_tmp1A4;for(0;_tmp1A7 != 0;(_tmp1A7=_tmp1A7->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple0*_tmp1A9;struct Cyc_Absyn_Tqual _tmp1AA;void*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple15*_tmp1A8=(struct _tuple15*)_tmp1A7->hd;_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;_tmp1AB=_tmp1A8->f3;_tmp1AC=_tmp1A8->f4;_tmp1AD=_tmp1A8->f5;
if(_tmp1AC != 0){
const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_warn(loc,((_tmp942="bad type params, ignoring",_tag_dyneither(_tmp942,sizeof(char),26))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp945;void*_tmp944;(_tmp944=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp945="unexpected NULL in parse!",_tag_dyneither(_tmp945,sizeof(char),26))),_tag_dyneither(_tmp944,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1B2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1B3=Cyc_Absyn_new_vardecl(_tmp1A9,_tmp1AB,_tmp1B2);
_tmp1B3->tq=_tmp1AA;
_tmp1B3->sc=s;
_tmp1B3->attributes=_tmp1AD;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp94B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp94A;struct Cyc_Absyn_Decl*_tmp949;struct Cyc_Absyn_Decl*_tmp1B4=(_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->r=(void*)((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94A.tag=0,((_tmp94A.f1=_tmp1B3,_tmp94A)))),_tmp94B)))),((_tmp949->loc=loc,_tmp949)))));
struct Cyc_List_List*_tmp94C;decls=((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->hd=_tmp1B4,((_tmp94C->tl=decls,_tmp94C))))));};};}}{
# 976
struct Cyc_List_List*_tmp1B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1B9;};}}};};};};}
# 873
;_pop_region(mkrgn);}
# 983
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB3:
 return& Cyc_Tcutil_ak;case 'M': _LLB4:
 return& Cyc_Tcutil_mk;case 'B': _LLB5:
 return& Cyc_Tcutil_bk;case 'R': _LLB6:
 return& Cyc_Tcutil_rk;case 'E': _LLB7:
 return& Cyc_Tcutil_ek;case 'I': _LLB8:
 return& Cyc_Tcutil_ik;case 'U': _LLB9:
# 993
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLBB:
 return& Cyc_Tcutil_urk;case 'A': _LLBC:
 return& Cyc_Tcutil_uak;case 'M': _LLBD:
 return& Cyc_Tcutil_umk;case 'B': _LLBE:
 return& Cyc_Tcutil_ubk;default: _LLBF:
 break;}
# 1000
break;case 'T': _LLBA:
# 1002
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLC2:
 return& Cyc_Tcutil_trk;case 'A': _LLC3:
 return& Cyc_Tcutil_tak;case 'M': _LLC4:
 return& Cyc_Tcutil_tmk;case 'B': _LLC5:
 return& Cyc_Tcutil_tbk;default: _LLC6:
 break;}
# 1009
break;default: _LLC1:
 break;}
# 1012
{const char*_tmp951;void*_tmp950[2];struct Cyc_String_pa_PrintArg_struct _tmp94F;struct Cyc_Int_pa_PrintArg_struct _tmp94E;Cyc_Parse_err((struct _dyneither_ptr)((_tmp94E.tag=1,((_tmp94E.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp94F.tag=0,((_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp950[0]=& _tmp94F,((_tmp950[1]=& _tmp94E,Cyc_aprintf(((_tmp951="bad kind: %s; strlen=%d",_tag_dyneither(_tmp951,sizeof(char),24))),_tag_dyneither(_tmp950,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BE=p->r;struct _tuple0*_tmp1C0;struct Cyc_Absyn_Vardecl*_tmp1C2;struct Cyc_Absyn_Pat*_tmp1C5;enum Cyc_Absyn_Sign _tmp1C8;int _tmp1C9;char _tmp1CB;struct _dyneither_ptr _tmp1CD;int _tmp1CE;struct _tuple0*_tmp1D0;struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;_LLCA: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1BF=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1BF->tag != 14)goto _LLCC;else{_tmp1C0=_tmp1BF->f1;}}_LLCB:
 return Cyc_Absyn_unknownid_exp(_tmp1C0,p->loc);_LLCC: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C1->tag != 2)goto _LLCE;else{_tmp1C2=_tmp1C1->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C3=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1C1->f2)->r;if(_tmp1C3->tag != 0)goto _LLCE;};}}_LLCD:
# 1021
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1C2->name,p->loc),p->loc);_LLCE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C4->tag != 5)goto _LLD0;else{_tmp1C5=_tmp1C4->f1;}}_LLCF:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1C5),p->loc);_LLD0: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1C6=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C6->tag != 8)goto _LLD2;}_LLD1:
 return Cyc_Absyn_null_exp(p->loc);_LLD2: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C7->tag != 9)goto _LLD4;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LLD3:
 return Cyc_Absyn_int_exp(_tmp1C8,_tmp1C9,p->loc);_LLD4: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1CA=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1CA->tag != 10)goto _LLD6;else{_tmp1CB=_tmp1CA->f1;}}_LLD5:
 return Cyc_Absyn_char_exp(_tmp1CB,p->loc);_LLD6: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1CC=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1CC->tag != 11)goto _LLD8;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;}}_LLD7:
 return Cyc_Absyn_float_exp(_tmp1CD,_tmp1CE,p->loc);_LLD8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1CF=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1CF->tag != 15)goto _LLDA;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;if(_tmp1CF->f3 != 0)goto _LLDA;}}_LLD9: {
# 1028
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1D0,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1D1);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLDA: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D2=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1D2->tag != 16)goto _LLDC;else{_tmp1D3=_tmp1D2->f1;}}_LLDB:
 return _tmp1D3;_LLDC:;_LLDD:
# 1033
{const char*_tmp952;Cyc_Parse_err(((_tmp952="cannot mix patterns and expressions in case",_tag_dyneither(_tmp952,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC9:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple19{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple19 val;};struct _tuple20{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple21{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple23 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1D6[14]="$(sign_t,int)";
# 1085 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 14}};
# 1087
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1D7=yy1;struct _tuple5 _tmp1D8;_LLDF: if((_tmp1D7.Int_tok).tag != 1)goto _LLE1;_tmp1D8=(struct _tuple5)(_tmp1D7.Int_tok).val;_LLE0:
# 1091
 yyzzz=_tmp1D8;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLDE:;}
# 1095
return yyzzz;}
# 1097
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp953;return((_tmp953.Int_tok).val=yy1,(((_tmp953.Int_tok).tag=1,_tmp953)));}static char _tmp1DB[5]="char";
# 1086 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 5}};
# 1088
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1DC=yy1;char _tmp1DD;_LLE4: if((_tmp1DC.Char_tok).tag != 2)goto _LLE6;_tmp1DD=(char)(_tmp1DC.Char_tok).val;_LLE5:
# 1092
 yyzzz=_tmp1DD;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLE3:;}
# 1096
return yyzzz;}
# 1098
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp954;return((_tmp954.Char_tok).val=yy1,(((_tmp954.Char_tok).tag=2,_tmp954)));}static char _tmp1E0[13]="string_t<`H>";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 13}};
# 1089
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1E1=yy1;struct _dyneither_ptr _tmp1E2;_LLE9: if((_tmp1E1.String_tok).tag != 3)goto _LLEB;_tmp1E2=(struct _dyneither_ptr)(_tmp1E1.String_tok).val;_LLEA:
# 1093
 yyzzz=_tmp1E2;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE8:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp955;return((_tmp955.String_tok).val=yy1,(((_tmp955.String_tok).tag=3,_tmp955)));}static char _tmp1E5[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1090 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E5,_tmp1E5,_tmp1E5 + 56}};
# 1092
struct _tuple20*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple20*yyzzz;
{union Cyc_YYSTYPE _tmp1E6=yy1;struct _tuple20*_tmp1E7;_LLEE: if((_tmp1E6.YY1).tag != 7)goto _LLF0;_tmp1E7=(struct _tuple20*)(_tmp1E6.YY1).val;_LLEF:
# 1096
 yyzzz=_tmp1E7;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLED:;}
# 1100
return yyzzz;}
# 1102
union Cyc_YYSTYPE Cyc_YY1(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp956;return((_tmp956.YY1).val=yy1,(((_tmp956.YY1).tag=7,_tmp956)));}static char _tmp1EA[19]="conref_t<bounds_t>";
# 1091 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 19}};
# 1093
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1EB=yy1;union Cyc_Absyn_Constraint*_tmp1EC;_LLF3: if((_tmp1EB.YY2).tag != 8)goto _LLF5;_tmp1EC=(union Cyc_Absyn_Constraint*)(_tmp1EB.YY2).val;_LLF4:
# 1097
 yyzzz=_tmp1EC;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLF2:;}
# 1101
return yyzzz;}
# 1103
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp957;return((_tmp957.YY2).val=yy1,(((_tmp957.YY2).tag=8,_tmp957)));}static char _tmp1EF[28]="list_t<offsetof_field_t,`H>";
# 1092 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 28}};
# 1094
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F0=yy1;struct Cyc_List_List*_tmp1F1;_LLF8: if((_tmp1F0.YY3).tag != 9)goto _LLFA;_tmp1F1=(struct Cyc_List_List*)(_tmp1F0.YY3).val;_LLF9:
# 1098
 yyzzz=_tmp1F1;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF7:;}
# 1102
return yyzzz;}
# 1104
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp958;return((_tmp958.YY3).val=yy1,(((_tmp958.YY3).tag=9,_tmp958)));}static char _tmp1F4[6]="exp_t";
# 1093 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1F4,_tmp1F4,_tmp1F4 + 6}};
# 1095
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1F5=yy1;struct Cyc_Absyn_Exp*_tmp1F6;_LLFD: if((_tmp1F5.YY4).tag != 10)goto _LLFF;_tmp1F6=(struct Cyc_Absyn_Exp*)(_tmp1F5.YY4).val;_LLFE:
# 1099
 yyzzz=_tmp1F6;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLFC:;}
# 1103
return yyzzz;}
# 1105
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp959;return((_tmp959.YY4).val=yy1,(((_tmp959.YY4).tag=10,_tmp959)));}static char _tmp1F9[10]="exp_opt_t";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 10}};
# 1103
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1FA=yy1;struct Cyc_Absyn_Exp*_tmp1FB;_LL102: if((_tmp1FA.YY5).tag != 11)goto _LL104;_tmp1FB=(struct Cyc_Absyn_Exp*)(_tmp1FA.YY5).val;_LL103:
# 1107
 yyzzz=_tmp1FB;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL101:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp95A;return((_tmp95A.YY5).val=yy1,(((_tmp95A.YY5).tag=11,_tmp95A)));}static char _tmp1FE[17]="list_t<exp_t,`H>";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 17}};
# 1104
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_List_List*_tmp200;_LL107: if((_tmp1FF.YY6).tag != 12)goto _LL109;_tmp200=(struct Cyc_List_List*)(_tmp1FF.YY6).val;_LL108:
# 1108
 yyzzz=_tmp200;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL106:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95B;return((_tmp95B.YY6).val=yy1,(((_tmp95B.YY6).tag=12,_tmp95B)));}static char _tmp203[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 47}};
# 1105
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp204=yy1;struct Cyc_List_List*_tmp205;_LL10C: if((_tmp204.YY7).tag != 13)goto _LL10E;_tmp205=(struct Cyc_List_List*)(_tmp204.YY7).val;_LL10D:
# 1109
 yyzzz=_tmp205;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL10B:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95C;return((_tmp95C.YY7).val=yy1,(((_tmp95C.YY7).tag=13,_tmp95C)));}static char _tmp208[9]="primop_t";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 9}};
# 1106
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp209=yy1;enum Cyc_Absyn_Primop _tmp20A;_LL111: if((_tmp209.YY8).tag != 14)goto _LL113;_tmp20A=(enum Cyc_Absyn_Primop)(_tmp209.YY8).val;_LL112:
# 1110
 yyzzz=_tmp20A;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL110:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp95D;return((_tmp95D.YY8).val=yy1,(((_tmp95D.YY8).tag=14,_tmp95D)));}static char _tmp20D[19]="opt_t<primop_t,`H>";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 19}};
# 1107
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp20E=yy1;struct Cyc_Core_Opt*_tmp20F;_LL116: if((_tmp20E.YY9).tag != 15)goto _LL118;_tmp20F=(struct Cyc_Core_Opt*)(_tmp20E.YY9).val;_LL117:
# 1111
 yyzzz=_tmp20F;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL115:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp95E;return((_tmp95E.YY9).val=yy1,(((_tmp95E.YY9).tag=15,_tmp95E)));}static char _tmp212[7]="qvar_t";
# 1106 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp212,_tmp212,_tmp212 + 7}};
# 1108
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp213=yy1;struct _tuple0*_tmp214;_LL11B: if((_tmp213.QualId_tok).tag != 5)goto _LL11D;_tmp214=(struct _tuple0*)(_tmp213.QualId_tok).val;_LL11C:
# 1112
 yyzzz=_tmp214;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL11A:;}
# 1116
return yyzzz;}
# 1118
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp95F;return((_tmp95F.QualId_tok).val=yy1,(((_tmp95F.QualId_tok).tag=5,_tmp95F)));}static char _tmp217[7]="stmt_t";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 7}};
# 1111
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp218=yy1;struct Cyc_Absyn_Stmt*_tmp219;_LL120: if((_tmp218.YY10).tag != 16)goto _LL122;_tmp219=(struct Cyc_Absyn_Stmt*)(_tmp218.YY10).val;_LL121:
# 1115
 yyzzz=_tmp219;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11F:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp960;return((_tmp960.YY10).val=yy1,(((_tmp960.YY10).tag=16,_tmp960)));}static char _tmp21C[27]="list_t<switch_clause_t,`H>";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 27}};
# 1115
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21D=yy1;struct Cyc_List_List*_tmp21E;_LL125: if((_tmp21D.YY11).tag != 17)goto _LL127;_tmp21E=(struct Cyc_List_List*)(_tmp21D.YY11).val;_LL126:
# 1119
 yyzzz=_tmp21E;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL124:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp961;return((_tmp961.YY11).val=yy1,(((_tmp961.YY11).tag=17,_tmp961)));}static char _tmp221[6]="pat_t";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp221,_tmp221,_tmp221 + 6}};
# 1116
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp222=yy1;struct Cyc_Absyn_Pat*_tmp223;_LL12A: if((_tmp222.YY12).tag != 18)goto _LL12C;_tmp223=(struct Cyc_Absyn_Pat*)(_tmp222.YY12).val;_LL12B:
# 1120
 yyzzz=_tmp223;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL129:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp962;return((_tmp962.YY12).val=yy1,(((_tmp962.YY12).tag=18,_tmp962)));}static char _tmp226[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 28}};
# 1121
struct _tuple21*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp227=yy1;struct _tuple21*_tmp228;_LL12F: if((_tmp227.YY13).tag != 19)goto _LL131;_tmp228=(struct _tuple21*)(_tmp227.YY13).val;_LL130:
# 1125
 yyzzz=_tmp228;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL12E:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY13(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp963;return((_tmp963.YY13).val=yy1,(((_tmp963.YY13).tag=19,_tmp963)));}static char _tmp22B[17]="list_t<pat_t,`H>";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 17}};
# 1122
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22C=yy1;struct Cyc_List_List*_tmp22D;_LL134: if((_tmp22C.YY14).tag != 20)goto _LL136;_tmp22D=(struct Cyc_List_List*)(_tmp22C.YY14).val;_LL135:
# 1126
 yyzzz=_tmp22D;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL133:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp964;return((_tmp964.YY14).val=yy1,(((_tmp964.YY14).tag=20,_tmp964)));}static char _tmp230[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 36}};
# 1123
struct _tuple22*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp231=yy1;struct _tuple22*_tmp232;_LL139: if((_tmp231.YY15).tag != 21)goto _LL13B;_tmp232=(struct _tuple22*)(_tmp231.YY15).val;_LL13A:
# 1127
 yyzzz=_tmp232;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL138:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY15(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp965;return((_tmp965.YY15).val=yy1,(((_tmp965.YY15).tag=21,_tmp965)));}static char _tmp235[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp235,_tmp235,_tmp235 + 47}};
# 1124
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp236=yy1;struct Cyc_List_List*_tmp237;_LL13E: if((_tmp236.YY16).tag != 22)goto _LL140;_tmp237=(struct Cyc_List_List*)(_tmp236.YY16).val;_LL13F:
# 1128
 yyzzz=_tmp237;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL13D:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.YY16).val=yy1,(((_tmp966.YY16).tag=22,_tmp966)));}static char _tmp23A[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 58}};
# 1125
struct _tuple21*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp23B=yy1;struct _tuple21*_tmp23C;_LL143: if((_tmp23B.YY17).tag != 23)goto _LL145;_tmp23C=(struct _tuple21*)(_tmp23B.YY17).val;_LL144:
# 1129
 yyzzz=_tmp23C;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL142:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY17(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.YY17).val=yy1,(((_tmp967.YY17).tag=23,_tmp967)));}static char _tmp23F[9]="fndecl_t";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 9}};
# 1126
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp240=yy1;struct Cyc_Absyn_Fndecl*_tmp241;_LL148: if((_tmp240.YY18).tag != 24)goto _LL14A;_tmp241=(struct Cyc_Absyn_Fndecl*)(_tmp240.YY18).val;_LL149:
# 1130
 yyzzz=_tmp241;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL147:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.YY18).val=yy1,(((_tmp968.YY18).tag=24,_tmp968)));}static char _tmp244[18]="list_t<decl_t,`H>";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp244,_tmp244,_tmp244 + 18}};
# 1127
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp245=yy1;struct Cyc_List_List*_tmp246;_LL14D: if((_tmp245.YY19).tag != 25)goto _LL14F;_tmp246=(struct Cyc_List_List*)(_tmp245.YY19).val;_LL14E:
# 1131
 yyzzz=_tmp246;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL14C:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY19).val=yy1,(((_tmp969.YY19).tag=25,_tmp969)));}static char _tmp249[12]="decl_spec_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp249,_tmp249,_tmp249 + 12}};
# 1129
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp24A=yy1;struct Cyc_Parse_Declaration_spec _tmp24B;_LL152: if((_tmp24A.YY20).tag != 26)goto _LL154;_tmp24B=(struct Cyc_Parse_Declaration_spec)(_tmp24A.YY20).val;_LL153:
# 1133
 yyzzz=_tmp24B;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL151:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY20).val=yy1,(((_tmp96A.YY20).tag=26,_tmp96A)));}static char _tmp24E[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp24E,_tmp24E,_tmp24E + 31}};
# 1130
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp24F=yy1;struct _tuple11 _tmp250;_LL157: if((_tmp24F.YY21).tag != 27)goto _LL159;_tmp250=(struct _tuple11)(_tmp24F.YY21).val;_LL158:
# 1134
 yyzzz=_tmp250;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL156:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY21).val=yy1,(((_tmp96B.YY21).tag=27,_tmp96B)));}static char _tmp253[23]="declarator_list_t<`yy>";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 23}};
# 1131
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp254=yy1;struct _tuple12*_tmp255;_LL15C: if((_tmp254.YY22).tag != 28)goto _LL15E;_tmp255=(struct _tuple12*)(_tmp254.YY22).val;_LL15D:
# 1135
 yyzzz=_tmp255;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL15B:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY22).val=yy1,(((_tmp96C.YY22).tag=28,_tmp96C)));}static char _tmp258[19]="storage_class_t@`H";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp258,_tmp258,_tmp258 + 19}};
# 1132
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp259=yy1;enum Cyc_Parse_Storage_class*_tmp25A;_LL161: if((_tmp259.YY23).tag != 29)goto _LL163;_tmp25A=(enum Cyc_Parse_Storage_class*)(_tmp259.YY23).val;_LL162:
# 1136
 yyzzz=_tmp25A;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL160:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY23).val=yy1,(((_tmp96D.YY23).tag=29,_tmp96D)));}static char _tmp25D[17]="type_specifier_t";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp25D,_tmp25D,_tmp25D + 17}};
# 1133
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp25E=yy1;struct Cyc_Parse_Type_specifier _tmp25F;_LL166: if((_tmp25E.YY24).tag != 30)goto _LL168;_tmp25F=(struct Cyc_Parse_Type_specifier)(_tmp25E.YY24).val;_LL167:
# 1137
 yyzzz=_tmp25F;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL165:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY24).val=yy1,(((_tmp96E.YY24).tag=30,_tmp96E)));}static char _tmp262[12]="aggr_kind_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp262,_tmp262,_tmp262 + 12}};
# 1135
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp263=yy1;enum Cyc_Absyn_AggrKind _tmp264;_LL16B: if((_tmp263.YY25).tag != 31)goto _LL16D;_tmp264=(enum Cyc_Absyn_AggrKind)(_tmp263.YY25).val;_LL16C:
# 1139
 yyzzz=_tmp264;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL16A:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY25).val=yy1,(((_tmp96F.YY25).tag=31,_tmp96F)));}static char _tmp267[8]="tqual_t";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 8}};
# 1136
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp268=yy1;struct Cyc_Absyn_Tqual _tmp269;_LL170: if((_tmp268.YY26).tag != 32)goto _LL172;_tmp269=(struct Cyc_Absyn_Tqual)(_tmp268.YY26).val;_LL171:
# 1140
 yyzzz=_tmp269;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16F:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY26).val=yy1,(((_tmp970.YY26).tag=32,_tmp970)));}static char _tmp26C[23]="list_t<aggrfield_t,`H>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp26C,_tmp26C,_tmp26C + 23}};
# 1137
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26D=yy1;struct Cyc_List_List*_tmp26E;_LL175: if((_tmp26D.YY27).tag != 33)goto _LL177;_tmp26E=(struct Cyc_List_List*)(_tmp26D.YY27).val;_LL176:
# 1141
 yyzzz=_tmp26E;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL174:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY27).val=yy1,(((_tmp971.YY27).tag=33,_tmp971)));}static char _tmp271[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp271,_tmp271,_tmp271 + 34}};
# 1138
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp272=yy1;struct Cyc_List_List*_tmp273;_LL17A: if((_tmp272.YY28).tag != 34)goto _LL17C;_tmp273=(struct Cyc_List_List*)(_tmp272.YY28).val;_LL17B:
# 1142
 yyzzz=_tmp273;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL179:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY28).val=yy1,(((_tmp972.YY28).tag=34,_tmp972)));}static char _tmp276[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp276,_tmp276,_tmp276 + 33}};
# 1139
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp277=yy1;struct Cyc_List_List*_tmp278;_LL17F: if((_tmp277.YY29).tag != 35)goto _LL181;_tmp278=(struct Cyc_List_List*)(_tmp277.YY29).val;_LL180:
# 1143
 yyzzz=_tmp278;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL17E:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY29).val=yy1,(((_tmp973.YY29).tag=35,_tmp973)));}static char _tmp27B[18]="declarator_t<`yy>";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp27B,_tmp27B,_tmp27B + 18}};
# 1140
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp27C=yy1;struct Cyc_Parse_Declarator _tmp27D;_LL184: if((_tmp27C.YY30).tag != 36)goto _LL186;_tmp27D=(struct Cyc_Parse_Declarator)(_tmp27C.YY30).val;_LL185:
# 1144
 yyzzz=_tmp27D;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL183:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY30).val=yy1,(((_tmp974.YY30).tag=36,_tmp974)));}static char _tmp280[26]="abstractdeclarator_t<`yy>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp280,_tmp280,_tmp280 + 26}};
# 1143
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp281=yy1;struct Cyc_Parse_Abstractdeclarator _tmp282;_LL189: if((_tmp281.YY31).tag != 37)goto _LL18B;_tmp282=(struct Cyc_Parse_Abstractdeclarator)(_tmp281.YY31).val;_LL18A:
# 1147
 yyzzz=_tmp282;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL188:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY31).val=yy1,(((_tmp975.YY31).tag=37,_tmp975)));}static char _tmp285[5]="bool";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp285,_tmp285,_tmp285 + 5}};
# 1144
int Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp286=yy1;int _tmp287;_LL18E: if((_tmp286.YY32).tag != 38)goto _LL190;_tmp287=(int)(_tmp286.YY32).val;_LL18F:
# 1148
 yyzzz=_tmp287;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL18D:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY32).val=yy1,(((_tmp976.YY32).tag=38,_tmp976)));}static char _tmp28A[8]="scope_t";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp28A,_tmp28A,_tmp28A + 8}};
# 1145
enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp28B=yy1;enum Cyc_Absyn_Scope _tmp28C;_LL193: if((_tmp28B.YY33).tag != 39)goto _LL195;_tmp28C=(enum Cyc_Absyn_Scope)(_tmp28B.YY33).val;_LL194:
# 1149
 yyzzz=_tmp28C;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL192:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY33).val=yy1,(((_tmp977.YY33).tag=39,_tmp977)));}static char _tmp28F[16]="datatypefield_t";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp28F,_tmp28F,_tmp28F + 16}};
# 1146
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp290=yy1;struct Cyc_Absyn_Datatypefield*_tmp291;_LL198: if((_tmp290.YY34).tag != 40)goto _LL19A;_tmp291=(struct Cyc_Absyn_Datatypefield*)(_tmp290.YY34).val;_LL199:
# 1150
 yyzzz=_tmp291;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL197:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY34).val=yy1,(((_tmp978.YY34).tag=40,_tmp978)));}static char _tmp294[27]="list_t<datatypefield_t,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp294,_tmp294,_tmp294 + 27}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp295=yy1;struct Cyc_List_List*_tmp296;_LL19D: if((_tmp295.YY35).tag != 41)goto _LL19F;_tmp296=(struct Cyc_List_List*)(_tmp295.YY35).val;_LL19E:
# 1151
 yyzzz=_tmp296;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL19C:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY35).val=yy1,(((_tmp979.YY35).tag=41,_tmp979)));}static char _tmp299[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp299,_tmp299,_tmp299 + 41}};
# 1148
struct _tuple23 Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct _tuple23 yyzzz;
{union Cyc_YYSTYPE _tmp29A=yy1;struct _tuple23 _tmp29B;_LL1A2: if((_tmp29A.YY36).tag != 42)goto _LL1A4;_tmp29B=(struct _tuple23)(_tmp29A.YY36).val;_LL1A3:
# 1152
 yyzzz=_tmp29B;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A1:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY36(struct _tuple23 yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY36).val=yy1,(((_tmp97A.YY36).tag=42,_tmp97A)));}static char _tmp29E[17]="list_t<var_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp29E,_tmp29E,_tmp29E + 17}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29F=yy1;struct Cyc_List_List*_tmp2A0;_LL1A7: if((_tmp29F.YY37).tag != 43)goto _LL1A9;_tmp2A0=(struct Cyc_List_List*)(_tmp29F.YY37).val;_LL1A8:
# 1153
 yyzzz=_tmp2A0;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A6:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY37).val=yy1,(((_tmp97B.YY37).tag=43,_tmp97B)));}static char _tmp2A3[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2A3,_tmp2A3,_tmp2A3 + 31}};
# 1150
struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2A4=yy1;struct _tuple8*_tmp2A5;_LL1AC: if((_tmp2A4.YY38).tag != 44)goto _LL1AE;_tmp2A5=(struct _tuple8*)(_tmp2A4.YY38).val;_LL1AD:
# 1154
 yyzzz=_tmp2A5;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1AB:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY38).val=yy1,(((_tmp97C.YY38).tag=44,_tmp97C)));}static char _tmp2A8[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A8,_tmp2A8,_tmp2A8 + 42}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A9=yy1;struct Cyc_List_List*_tmp2AA;_LL1B1: if((_tmp2A9.YY39).tag != 45)goto _LL1B3;_tmp2AA=(struct Cyc_List_List*)(_tmp2A9.YY39).val;_LL1B2:
# 1155
 yyzzz=_tmp2AA;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY39).val=yy1,(((_tmp97D.YY39).tag=45,_tmp97D)));}static char _tmp2AD[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2AD,_tmp2AD,_tmp2AD + 115}};
# 1152
struct _tuple24*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp2AE=yy1;struct _tuple24*_tmp2AF;_LL1B6: if((_tmp2AE.YY40).tag != 46)goto _LL1B8;_tmp2AF=(struct _tuple24*)(_tmp2AE.YY40).val;_LL1B7:
# 1156
 yyzzz=_tmp2AF;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B5:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY40(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY40).val=yy1,(((_tmp97E.YY40).tag=46,_tmp97E)));}static char _tmp2B2[18]="list_t<type_t,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2B2,_tmp2B2,_tmp2B2 + 18}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B3=yy1;struct Cyc_List_List*_tmp2B4;_LL1BB: if((_tmp2B3.YY41).tag != 47)goto _LL1BD;_tmp2B4=(struct Cyc_List_List*)(_tmp2B3.YY41).val;_LL1BC:
# 1157
 yyzzz=_tmp2B4;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1BA:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY41).val=yy1,(((_tmp97F.YY41).tag=47,_tmp97F)));}static char _tmp2B7[24]="list_t<designator_t,`H>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B7,_tmp2B7,_tmp2B7 + 24}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B8=yy1;struct Cyc_List_List*_tmp2B9;_LL1C0: if((_tmp2B8.YY42).tag != 48)goto _LL1C2;_tmp2B9=(struct Cyc_List_List*)(_tmp2B8.YY42).val;_LL1C1:
# 1159
 yyzzz=_tmp2B9;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BF:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY42).val=yy1,(((_tmp980.YY42).tag=48,_tmp980)));}static char _tmp2BC[13]="designator_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2BC,_tmp2BC,_tmp2BC + 13}};
# 1156
void*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2BD=yy1;void*_tmp2BE;_LL1C5: if((_tmp2BD.YY43).tag != 49)goto _LL1C7;_tmp2BE=(void*)(_tmp2BD.YY43).val;_LL1C6:
# 1160
 yyzzz=_tmp2BE;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C4:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY43).val=yy1,(((_tmp981.YY43).tag=49,_tmp981)));}static char _tmp2C1[7]="kind_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2C1,_tmp2C1,_tmp2C1 + 7}};
# 1157
struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2C2=yy1;struct Cyc_Absyn_Kind*_tmp2C3;_LL1CA: if((_tmp2C2.YY44).tag != 50)goto _LL1CC;_tmp2C3=(struct Cyc_Absyn_Kind*)(_tmp2C2.YY44).val;_LL1CB:
# 1161
 yyzzz=_tmp2C3;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C9:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY44).val=yy1,(((_tmp982.YY44).tag=50,_tmp982)));}static char _tmp2C6[7]="type_t";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C6,_tmp2C6,_tmp2C6 + 7}};
# 1158
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C7=yy1;void*_tmp2C8;_LL1CF: if((_tmp2C7.YY45).tag != 51)goto _LL1D1;_tmp2C8=(void*)(_tmp2C7.YY45).val;_LL1D0:
# 1162
 yyzzz=_tmp2C8;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1CE:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY45).val=yy1,(((_tmp983.YY45).tag=51,_tmp983)));}static char _tmp2CB[23]="list_t<attribute_t,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2CB,_tmp2CB,_tmp2CB + 23}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CC=yy1;struct Cyc_List_List*_tmp2CD;_LL1D4: if((_tmp2CC.YY46).tag != 52)goto _LL1D6;_tmp2CD=(struct Cyc_List_List*)(_tmp2CC.YY46).val;_LL1D5:
# 1163
 yyzzz=_tmp2CD;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1D3:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY46).val=yy1,(((_tmp984.YY46).tag=52,_tmp984)));}static char _tmp2D0[12]="attribute_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2D0,_tmp2D0,_tmp2D0 + 12}};
# 1160
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D1=yy1;void*_tmp2D2;_LL1D9: if((_tmp2D1.YY47).tag != 53)goto _LL1DB;_tmp2D2=(void*)(_tmp2D1.YY47).val;_LL1DA:
# 1164
 yyzzz=_tmp2D2;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D8:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY47).val=yy1,(((_tmp985.YY47).tag=53,_tmp985)));}static char _tmp2D5[12]="enumfield_t";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D5,_tmp2D5,_tmp2D5 + 12}};
# 1161
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2D6=yy1;struct Cyc_Absyn_Enumfield*_tmp2D7;_LL1DE: if((_tmp2D6.YY48).tag != 54)goto _LL1E0;_tmp2D7=(struct Cyc_Absyn_Enumfield*)(_tmp2D6.YY48).val;_LL1DF:
# 1165
 yyzzz=_tmp2D7;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1DD:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY48).val=yy1,(((_tmp986.YY48).tag=54,_tmp986)));}static char _tmp2DA[23]="list_t<enumfield_t,`H>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2DA,_tmp2DA,_tmp2DA + 23}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2DB=yy1;struct Cyc_List_List*_tmp2DC;_LL1E3: if((_tmp2DB.YY49).tag != 55)goto _LL1E5;_tmp2DC=(struct Cyc_List_List*)(_tmp2DB.YY49).val;_LL1E4:
# 1166
 yyzzz=_tmp2DC;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E2:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY49).val=yy1,(((_tmp987.YY49).tag=55,_tmp987)));}static char _tmp2DF[11]="type_opt_t";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2DF,_tmp2DF,_tmp2DF + 11}};
# 1163
void*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E0=yy1;void*_tmp2E1;_LL1E8: if((_tmp2E0.YY50).tag != 56)goto _LL1EA;_tmp2E1=(void*)(_tmp2E0.YY50).val;_LL1E9:
# 1167
 yyzzz=_tmp2E1;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E7:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY50).val=yy1,(((_tmp988.YY50).tag=56,_tmp988)));}static char _tmp2E4[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2E4,_tmp2E4,_tmp2E4 + 31}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E5=yy1;struct Cyc_List_List*_tmp2E6;_LL1ED: if((_tmp2E5.YY51).tag != 57)goto _LL1EF;_tmp2E6=(struct Cyc_List_List*)(_tmp2E5.YY51).val;_LL1EE:
# 1168
 yyzzz=_tmp2E6;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1EC:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY51).val=yy1,(((_tmp989.YY51).tag=57,_tmp989)));}static char _tmp2E9[15]="conref_t<bool>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E9,_tmp2E9,_tmp2E9 + 15}};
# 1165
union Cyc_Absyn_Constraint*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2EA=yy1;union Cyc_Absyn_Constraint*_tmp2EB;_LL1F2: if((_tmp2EA.YY52).tag != 58)goto _LL1F4;_tmp2EB=(union Cyc_Absyn_Constraint*)(_tmp2EA.YY52).val;_LL1F3:
# 1169
 yyzzz=_tmp2EB;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1F1:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY52(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY52).val=yy1,(((_tmp98A.YY52).tag=58,_tmp98A)));}static char _tmp2EE[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2EE,_tmp2EE,_tmp2EE + 45}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2EF=yy1;struct Cyc_List_List*_tmp2F0;_LL1F7: if((_tmp2EF.YY53).tag != 59)goto _LL1F9;_tmp2F0=(struct Cyc_List_List*)(_tmp2EF.YY53).val;_LL1F8:
# 1170
 yyzzz=_tmp2F0;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F6:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY53).val=yy1,(((_tmp98B.YY53).tag=59,_tmp98B)));}static char _tmp2F3[20]="pointer_qual_t<`yy>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2F3,_tmp2F3,_tmp2F3 + 20}};
# 1167
void*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F4=yy1;void*_tmp2F5;_LL1FC: if((_tmp2F4.YY54).tag != 60)goto _LL1FE;_tmp2F5=(void*)(_tmp2F4.YY54).val;_LL1FD:
# 1171
 yyzzz=_tmp2F5;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1FB:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY54).val=yy1,(((_tmp98C.YY54).tag=60,_tmp98C)));}static char _tmp2F8[21]="pointer_quals_t<`yy>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F8,_tmp2F8,_tmp2F8 + 21}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F9=yy1;struct Cyc_List_List*_tmp2FA;_LL201: if((_tmp2F9.YY55).tag != 61)goto _LL203;_tmp2FA=(struct Cyc_List_List*)(_tmp2F9.YY55).val;_LL202:
# 1172
 yyzzz=_tmp2FA;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL200:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY55).val=yy1,(((_tmp98D.YY55).tag=61,_tmp98D)));}static char _tmp2FD[21]="$(bool,string_t<`H>)";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2FD,_tmp2FD,_tmp2FD + 21}};
# 1169
struct _tuple19 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple19 yyzzz;
{union Cyc_YYSTYPE _tmp2FE=yy1;struct _tuple19 _tmp2FF;_LL206: if((_tmp2FE.Asm_tok).tag != 6)goto _LL208;_tmp2FF=(struct _tuple19)(_tmp2FE.Asm_tok).val;_LL207:
# 1173
 yyzzz=_tmp2FF;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL205:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple19 yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.Asm_tok).val=yy1,(((_tmp98E.Asm_tok).tag=6,_tmp98E)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1191
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1195
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp98F;return(_tmp98F.timestamp=0,((_tmp98F.first_line=0,((_tmp98F.first_column=0,((_tmp98F.last_line=0,((_tmp98F.last_column=0,_tmp98F)))))))));}
# 1198
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1209 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp302[2]="$";static char _tmp303[6]="error";static char _tmp304[12]="$undefined.";static char _tmp305[5]="AUTO";static char _tmp306[9]="REGISTER";static char _tmp307[7]="STATIC";static char _tmp308[7]="EXTERN";static char _tmp309[8]="TYPEDEF";static char _tmp30A[5]="VOID";static char _tmp30B[5]="CHAR";static char _tmp30C[6]="SHORT";static char _tmp30D[4]="INT";static char _tmp30E[5]="LONG";static char _tmp30F[6]="FLOAT";static char _tmp310[7]="DOUBLE";static char _tmp311[7]="SIGNED";static char _tmp312[9]="UNSIGNED";static char _tmp313[6]="CONST";static char _tmp314[9]="VOLATILE";static char _tmp315[9]="RESTRICT";static char _tmp316[7]="STRUCT";static char _tmp317[6]="UNION";static char _tmp318[5]="CASE";static char _tmp319[8]="DEFAULT";static char _tmp31A[7]="INLINE";static char _tmp31B[7]="SIZEOF";static char _tmp31C[9]="OFFSETOF";static char _tmp31D[3]="IF";static char _tmp31E[5]="ELSE";static char _tmp31F[7]="SWITCH";static char _tmp320[6]="WHILE";static char _tmp321[3]="DO";static char _tmp322[4]="FOR";static char _tmp323[5]="GOTO";static char _tmp324[9]="CONTINUE";static char _tmp325[6]="BREAK";static char _tmp326[7]="RETURN";static char _tmp327[5]="ENUM";static char _tmp328[8]="NULL_kw";static char _tmp329[4]="LET";static char _tmp32A[6]="THROW";static char _tmp32B[4]="TRY";static char _tmp32C[6]="CATCH";static char _tmp32D[7]="EXPORT";static char _tmp32E[4]="NEW";static char _tmp32F[9]="ABSTRACT";static char _tmp330[9]="FALLTHRU";static char _tmp331[6]="USING";static char _tmp332[10]="NAMESPACE";static char _tmp333[9]="DATATYPE";static char _tmp334[8]="XTUNION";static char _tmp335[7]="TUNION";static char _tmp336[7]="MALLOC";static char _tmp337[8]="RMALLOC";static char _tmp338[7]="CALLOC";static char _tmp339[8]="RCALLOC";static char _tmp33A[5]="SWAP";static char _tmp33B[9]="REGION_T";static char _tmp33C[6]="TAG_T";static char _tmp33D[7]="REGION";static char _tmp33E[5]="RNEW";static char _tmp33F[8]="REGIONS";static char _tmp340[13]="RESET_REGION";static char _tmp341[16]="NOZEROTERM_QUAL";static char _tmp342[14]="ZEROTERM_QUAL";static char _tmp343[12]="REGION_QUAL";static char _tmp344[7]="PORTON";static char _tmp345[8]="PORTOFF";static char _tmp346[12]="DYNREGION_T";static char _tmp347[6]="ALIAS";static char _tmp348[8]="NUMELTS";static char _tmp349[8]="VALUEOF";static char _tmp34A[10]="VALUEOF_T";static char _tmp34B[9]="TAGCHECK";static char _tmp34C[13]="NUMELTS_QUAL";static char _tmp34D[10]="THIN_QUAL";static char _tmp34E[9]="FAT_QUAL";static char _tmp34F[13]="NOTNULL_QUAL";static char _tmp350[14]="NULLABLE_QUAL";static char _tmp351[12]="TAGGED_QUAL";static char _tmp352[16]="EXTENSIBLE_QUAL";static char _tmp353[15]="RESETABLE_QUAL";static char _tmp354[7]="PTR_OP";static char _tmp355[7]="INC_OP";static char _tmp356[7]="DEC_OP";static char _tmp357[8]="LEFT_OP";static char _tmp358[9]="RIGHT_OP";static char _tmp359[6]="LE_OP";static char _tmp35A[6]="GE_OP";static char _tmp35B[6]="EQ_OP";static char _tmp35C[6]="NE_OP";static char _tmp35D[7]="AND_OP";static char _tmp35E[6]="OR_OP";static char _tmp35F[11]="MUL_ASSIGN";static char _tmp360[11]="DIV_ASSIGN";static char _tmp361[11]="MOD_ASSIGN";static char _tmp362[11]="ADD_ASSIGN";static char _tmp363[11]="SUB_ASSIGN";static char _tmp364[12]="LEFT_ASSIGN";static char _tmp365[13]="RIGHT_ASSIGN";static char _tmp366[11]="AND_ASSIGN";static char _tmp367[11]="XOR_ASSIGN";static char _tmp368[10]="OR_ASSIGN";static char _tmp369[9]="ELLIPSIS";static char _tmp36A[11]="LEFT_RIGHT";static char _tmp36B[12]="COLON_COLON";static char _tmp36C[11]="IDENTIFIER";static char _tmp36D[17]="INTEGER_CONSTANT";static char _tmp36E[7]="STRING";static char _tmp36F[8]="WSTRING";static char _tmp370[19]="CHARACTER_CONSTANT";static char _tmp371[20]="WCHARACTER_CONSTANT";static char _tmp372[18]="FLOATING_CONSTANT";static char _tmp373[9]="TYPE_VAR";static char _tmp374[13]="TYPEDEF_NAME";static char _tmp375[16]="QUAL_IDENTIFIER";static char _tmp376[18]="QUAL_TYPEDEF_NAME";static char _tmp377[10]="ATTRIBUTE";static char _tmp378[4]="ASM";static char _tmp379[4]="';'";static char _tmp37A[4]="'{'";static char _tmp37B[4]="'}'";static char _tmp37C[4]="','";static char _tmp37D[4]="'='";static char _tmp37E[4]="'<'";static char _tmp37F[4]="'>'";static char _tmp380[4]="'('";static char _tmp381[4]="')'";static char _tmp382[4]="'_'";static char _tmp383[4]="'$'";static char _tmp384[4]="':'";static char _tmp385[4]="'.'";static char _tmp386[4]="'['";static char _tmp387[4]="']'";static char _tmp388[4]="'*'";static char _tmp389[4]="'@'";static char _tmp38A[4]="'?'";static char _tmp38B[4]="'+'";static char _tmp38C[4]="'|'";static char _tmp38D[4]="'^'";static char _tmp38E[4]="'&'";static char _tmp38F[4]="'-'";static char _tmp390[4]="'/'";static char _tmp391[4]="'%'";static char _tmp392[4]="'~'";static char _tmp393[4]="'!'";static char _tmp394[5]="prog";static char _tmp395[17]="translation_unit";static char _tmp396[12]="export_list";static char _tmp397[19]="export_list_values";static char _tmp398[21]="external_declaration";static char _tmp399[20]="function_definition";static char _tmp39A[21]="function_definition2";static char _tmp39B[13]="using_action";static char _tmp39C[15]="unusing_action";static char _tmp39D[17]="namespace_action";static char _tmp39E[19]="unnamespace_action";static char _tmp39F[12]="declaration";static char _tmp3A0[19]="resetable_qual_opt";static char _tmp3A1[17]="declaration_list";static char _tmp3A2[23]="declaration_specifiers";static char _tmp3A3[24]="storage_class_specifier";static char _tmp3A4[15]="attributes_opt";static char _tmp3A5[11]="attributes";static char _tmp3A6[15]="attribute_list";static char _tmp3A7[10]="attribute";static char _tmp3A8[15]="type_specifier";static char _tmp3A9[25]="type_specifier_notypedef";static char _tmp3AA[5]="kind";static char _tmp3AB[15]="type_qualifier";static char _tmp3AC[15]="enum_specifier";static char _tmp3AD[11]="enum_field";static char _tmp3AE[22]="enum_declaration_list";static char _tmp3AF[26]="struct_or_union_specifier";static char _tmp3B0[16]="type_params_opt";static char _tmp3B1[16]="struct_or_union";static char _tmp3B2[24]="struct_declaration_list";static char _tmp3B3[25]="struct_declaration_list0";static char _tmp3B4[21]="init_declarator_list";static char _tmp3B5[22]="init_declarator_list0";static char _tmp3B6[16]="init_declarator";static char _tmp3B7[19]="struct_declaration";static char _tmp3B8[25]="specifier_qualifier_list";static char _tmp3B9[35]="notypedef_specifier_qualifier_list";static char _tmp3BA[23]="struct_declarator_list";static char _tmp3BB[24]="struct_declarator_list0";static char _tmp3BC[18]="struct_declarator";static char _tmp3BD[19]="datatype_specifier";static char _tmp3BE[14]="qual_datatype";static char _tmp3BF[19]="datatypefield_list";static char _tmp3C0[20]="datatypefield_scope";static char _tmp3C1[14]="datatypefield";static char _tmp3C2[11]="declarator";static char _tmp3C3[23]="declarator_withtypedef";static char _tmp3C4[18]="direct_declarator";static char _tmp3C5[30]="direct_declarator_withtypedef";static char _tmp3C6[8]="pointer";static char _tmp3C7[12]="one_pointer";static char _tmp3C8[14]="pointer_quals";static char _tmp3C9[13]="pointer_qual";static char _tmp3CA[23]="pointer_null_and_bound";static char _tmp3CB[14]="pointer_bound";static char _tmp3CC[18]="zeroterm_qual_opt";static char _tmp3CD[8]="rgn_opt";static char _tmp3CE[11]="tqual_list";static char _tmp3CF[20]="parameter_type_list";static char _tmp3D0[9]="type_var";static char _tmp3D1[16]="optional_effect";static char _tmp3D2[19]="optional_rgn_order";static char _tmp3D3[10]="rgn_order";static char _tmp3D4[16]="optional_inject";static char _tmp3D5[11]="effect_set";static char _tmp3D6[14]="atomic_effect";static char _tmp3D7[11]="region_set";static char _tmp3D8[15]="parameter_list";static char _tmp3D9[22]="parameter_declaration";static char _tmp3DA[16]="identifier_list";static char _tmp3DB[17]="identifier_list0";static char _tmp3DC[12]="initializer";static char _tmp3DD[18]="array_initializer";static char _tmp3DE[17]="initializer_list";static char _tmp3DF[12]="designation";static char _tmp3E0[16]="designator_list";static char _tmp3E1[11]="designator";static char _tmp3E2[10]="type_name";static char _tmp3E3[14]="any_type_name";static char _tmp3E4[15]="type_name_list";static char _tmp3E5[20]="abstract_declarator";static char _tmp3E6[27]="direct_abstract_declarator";static char _tmp3E7[10]="statement";static char _tmp3E8[13]="open_exp_opt";static char _tmp3E9[18]="labeled_statement";static char _tmp3EA[21]="expression_statement";static char _tmp3EB[19]="compound_statement";static char _tmp3EC[16]="block_item_list";static char _tmp3ED[20]="selection_statement";static char _tmp3EE[15]="switch_clauses";static char _tmp3EF[20]="iteration_statement";static char _tmp3F0[15]="jump_statement";static char _tmp3F1[12]="exp_pattern";static char _tmp3F2[20]="conditional_pattern";static char _tmp3F3[19]="logical_or_pattern";static char _tmp3F4[20]="logical_and_pattern";static char _tmp3F5[21]="inclusive_or_pattern";static char _tmp3F6[21]="exclusive_or_pattern";static char _tmp3F7[12]="and_pattern";static char _tmp3F8[17]="equality_pattern";static char _tmp3F9[19]="relational_pattern";static char _tmp3FA[14]="shift_pattern";static char _tmp3FB[17]="additive_pattern";static char _tmp3FC[23]="multiplicative_pattern";static char _tmp3FD[13]="cast_pattern";static char _tmp3FE[14]="unary_pattern";static char _tmp3FF[16]="postfix_pattern";static char _tmp400[16]="primary_pattern";static char _tmp401[8]="pattern";static char _tmp402[19]="tuple_pattern_list";static char _tmp403[20]="tuple_pattern_list0";static char _tmp404[14]="field_pattern";static char _tmp405[19]="field_pattern_list";static char _tmp406[20]="field_pattern_list0";static char _tmp407[11]="expression";static char _tmp408[22]="assignment_expression";static char _tmp409[20]="assignment_operator";static char _tmp40A[23]="conditional_expression";static char _tmp40B[20]="constant_expression";static char _tmp40C[22]="logical_or_expression";static char _tmp40D[23]="logical_and_expression";static char _tmp40E[24]="inclusive_or_expression";static char _tmp40F[24]="exclusive_or_expression";static char _tmp410[15]="and_expression";static char _tmp411[20]="equality_expression";static char _tmp412[22]="relational_expression";static char _tmp413[17]="shift_expression";static char _tmp414[20]="additive_expression";static char _tmp415[26]="multiplicative_expression";static char _tmp416[16]="cast_expression";static char _tmp417[17]="unary_expression";static char _tmp418[15]="unary_operator";static char _tmp419[19]="postfix_expression";static char _tmp41A[17]="field_expression";static char _tmp41B[19]="primary_expression";static char _tmp41C[25]="argument_expression_list";static char _tmp41D[26]="argument_expression_list0";static char _tmp41E[9]="constant";static char _tmp41F[20]="qual_opt_identifier";static char _tmp420[17]="qual_opt_typedef";static char _tmp421[18]="struct_union_name";static char _tmp422[11]="field_name";static char _tmp423[12]="right_angle";
# 1554 "parse.y"
static struct _dyneither_ptr Cyc_yytname[290]={{_tmp302,_tmp302,_tmp302 + 2},{_tmp303,_tmp303,_tmp303 + 6},{_tmp304,_tmp304,_tmp304 + 12},{_tmp305,_tmp305,_tmp305 + 5},{_tmp306,_tmp306,_tmp306 + 9},{_tmp307,_tmp307,_tmp307 + 7},{_tmp308,_tmp308,_tmp308 + 7},{_tmp309,_tmp309,_tmp309 + 8},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,_tmp30B,_tmp30B + 5},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 4},{_tmp30E,_tmp30E,_tmp30E + 5},{_tmp30F,_tmp30F,_tmp30F + 6},{_tmp310,_tmp310,_tmp310 + 7},{_tmp311,_tmp311,_tmp311 + 7},{_tmp312,_tmp312,_tmp312 + 9},{_tmp313,_tmp313,_tmp313 + 6},{_tmp314,_tmp314,_tmp314 + 9},{_tmp315,_tmp315,_tmp315 + 9},{_tmp316,_tmp316,_tmp316 + 7},{_tmp317,_tmp317,_tmp317 + 6},{_tmp318,_tmp318,_tmp318 + 5},{_tmp319,_tmp319,_tmp319 + 8},{_tmp31A,_tmp31A,_tmp31A + 7},{_tmp31B,_tmp31B,_tmp31B + 7},{_tmp31C,_tmp31C,_tmp31C + 9},{_tmp31D,_tmp31D,_tmp31D + 3},{_tmp31E,_tmp31E,_tmp31E + 5},{_tmp31F,_tmp31F,_tmp31F + 7},{_tmp320,_tmp320,_tmp320 + 6},{_tmp321,_tmp321,_tmp321 + 3},{_tmp322,_tmp322,_tmp322 + 4},{_tmp323,_tmp323,_tmp323 + 5},{_tmp324,_tmp324,_tmp324 + 9},{_tmp325,_tmp325,_tmp325 + 6},{_tmp326,_tmp326,_tmp326 + 7},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 8},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 6},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 4},{_tmp32F,_tmp32F,_tmp32F + 9},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 6},{_tmp332,_tmp332,_tmp332 + 10},{_tmp333,_tmp333,_tmp333 + 9},{_tmp334,_tmp334,_tmp334 + 8},{_tmp335,_tmp335,_tmp335 + 7},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 8},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 5},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 13},{_tmp341,_tmp341,_tmp341 + 16},{_tmp342,_tmp342,_tmp342 + 14},{_tmp343,_tmp343,_tmp343 + 12},{_tmp344,_tmp344,_tmp344 + 7},{_tmp345,_tmp345,_tmp345 + 8},{_tmp346,_tmp346,_tmp346 + 12},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 8},{_tmp349,_tmp349,_tmp349 + 8},{_tmp34A,_tmp34A,_tmp34A + 10},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 13},{_tmp34D,_tmp34D,_tmp34D + 10},{_tmp34E,_tmp34E,_tmp34E + 9},{_tmp34F,_tmp34F,_tmp34F + 13},{_tmp350,_tmp350,_tmp350 + 14},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 16},{_tmp353,_tmp353,_tmp353 + 15},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 7},{_tmp356,_tmp356,_tmp356 + 7},{_tmp357,_tmp357,_tmp357 + 8},{_tmp358,_tmp358,_tmp358 + 9},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 11},{_tmp361,_tmp361,_tmp361 + 11},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 12},{_tmp365,_tmp365,_tmp365 + 13},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 11},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 11},{_tmp36B,_tmp36B,_tmp36B + 12},{_tmp36C,_tmp36C,_tmp36C + 11},{_tmp36D,_tmp36D,_tmp36D + 17},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 8},{_tmp370,_tmp370,_tmp370 + 19},{_tmp371,_tmp371,_tmp371 + 20},{_tmp372,_tmp372,_tmp372 + 18},{_tmp373,_tmp373,_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 13},{_tmp375,_tmp375,_tmp375 + 16},{_tmp376,_tmp376,_tmp376 + 18},{_tmp377,_tmp377,_tmp377 + 10},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 4},{_tmp394,_tmp394,_tmp394 + 5},{_tmp395,_tmp395,_tmp395 + 17},{_tmp396,_tmp396,_tmp396 + 12},{_tmp397,_tmp397,_tmp397 + 19},{_tmp398,_tmp398,_tmp398 + 21},{_tmp399,_tmp399,_tmp399 + 20},{_tmp39A,_tmp39A,_tmp39A + 21},{_tmp39B,_tmp39B,_tmp39B + 13},{_tmp39C,_tmp39C,_tmp39C + 15},{_tmp39D,_tmp39D,_tmp39D + 17},{_tmp39E,_tmp39E,_tmp39E + 19},{_tmp39F,_tmp39F,_tmp39F + 12},{_tmp3A0,_tmp3A0,_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 17},{_tmp3A2,_tmp3A2,_tmp3A2 + 23},{_tmp3A3,_tmp3A3,_tmp3A3 + 24},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 11},{_tmp3A6,_tmp3A6,_tmp3A6 + 15},{_tmp3A7,_tmp3A7,_tmp3A7 + 10},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 25},{_tmp3AA,_tmp3AA,_tmp3AA + 5},{_tmp3AB,_tmp3AB,_tmp3AB + 15},{_tmp3AC,_tmp3AC,_tmp3AC + 15},{_tmp3AD,_tmp3AD,_tmp3AD + 11},{_tmp3AE,_tmp3AE,_tmp3AE + 22},{_tmp3AF,_tmp3AF,_tmp3AF + 26},{_tmp3B0,_tmp3B0,_tmp3B0 + 16},{_tmp3B1,_tmp3B1,_tmp3B1 + 16},{_tmp3B2,_tmp3B2,_tmp3B2 + 24},{_tmp3B3,_tmp3B3,_tmp3B3 + 25},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 22},{_tmp3B6,_tmp3B6,_tmp3B6 + 16},{_tmp3B7,_tmp3B7,_tmp3B7 + 19},{_tmp3B8,_tmp3B8,_tmp3B8 + 25},{_tmp3B9,_tmp3B9,_tmp3B9 + 35},{_tmp3BA,_tmp3BA,_tmp3BA + 23},{_tmp3BB,_tmp3BB,_tmp3BB + 24},{_tmp3BC,_tmp3BC,_tmp3BC + 18},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 14},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 20},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 23},{_tmp3C4,_tmp3C4,_tmp3C4 + 18},{_tmp3C5,_tmp3C5,_tmp3C5 + 30},{_tmp3C6,_tmp3C6,_tmp3C6 + 8},{_tmp3C7,_tmp3C7,_tmp3C7 + 12},{_tmp3C8,_tmp3C8,_tmp3C8 + 14},{_tmp3C9,_tmp3C9,_tmp3C9 + 13},{_tmp3CA,_tmp3CA,_tmp3CA + 23},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 18},{_tmp3CD,_tmp3CD,_tmp3CD + 8},{_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 9},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,_tmp3D2,_tmp3D2 + 19},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 11},{_tmp3D6,_tmp3D6,_tmp3D6 + 14},{_tmp3D7,_tmp3D7,_tmp3D7 + 11},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,_tmp3D9 + 22},{_tmp3DA,_tmp3DA,_tmp3DA + 16},{_tmp3DB,_tmp3DB,_tmp3DB + 17},{_tmp3DC,_tmp3DC,_tmp3DC + 12},{_tmp3DD,_tmp3DD,_tmp3DD + 18},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 11},{_tmp3E2,_tmp3E2,_tmp3E2 + 10},{_tmp3E3,_tmp3E3,_tmp3E3 + 14},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 20},{_tmp3E6,_tmp3E6,_tmp3E6 + 27},{_tmp3E7,_tmp3E7,_tmp3E7 + 10},{_tmp3E8,_tmp3E8,_tmp3E8 + 13},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 21},{_tmp3EB,_tmp3EB,_tmp3EB + 19},{_tmp3EC,_tmp3EC,_tmp3EC + 16},{_tmp3ED,_tmp3ED,_tmp3ED + 20},{_tmp3EE,_tmp3EE,_tmp3EE + 15},{_tmp3EF,_tmp3EF,_tmp3EF + 20},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,_tmp3F1 + 12},{_tmp3F2,_tmp3F2,_tmp3F2 + 20},{_tmp3F3,_tmp3F3,_tmp3F3 + 19},{_tmp3F4,_tmp3F4,_tmp3F4 + 20},{_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 21},{_tmp3F7,_tmp3F7,_tmp3F7 + 12},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 23},{_tmp3FD,_tmp3FD,_tmp3FD + 13},{_tmp3FE,_tmp3FE,_tmp3FE + 14},{_tmp3FF,_tmp3FF,_tmp3FF + 16},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 8},{_tmp402,_tmp402,_tmp402 + 19},{_tmp403,_tmp403,_tmp403 + 20},{_tmp404,_tmp404,_tmp404 + 14},{_tmp405,_tmp405,_tmp405 + 19},{_tmp406,_tmp406,_tmp406 + 20},{_tmp407,_tmp407,_tmp407 + 11},{_tmp408,_tmp408,_tmp408 + 22},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 23},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 22},{_tmp40D,_tmp40D,_tmp40D + 23},{_tmp40E,_tmp40E,_tmp40E + 24},{_tmp40F,_tmp40F,_tmp40F + 24},{_tmp410,_tmp410,_tmp410 + 15},{_tmp411,_tmp411,_tmp411 + 20},{_tmp412,_tmp412,_tmp412 + 22},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 20},{_tmp415,_tmp415,_tmp415 + 26},{_tmp416,_tmp416,_tmp416 + 16},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 15},{_tmp419,_tmp419,_tmp419 + 19},{_tmp41A,_tmp41A,_tmp41A + 17},{_tmp41B,_tmp41B,_tmp41B + 19},{_tmp41C,_tmp41C,_tmp41C + 25},{_tmp41D,_tmp41D,_tmp41D + 26},{_tmp41E,_tmp41E,_tmp41E + 9},{_tmp41F,_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 17},{_tmp421,_tmp421,_tmp421 + 18},{_tmp422,_tmp422,_tmp422 + 11},{_tmp423,_tmp423,_tmp423 + 12}};
# 1608
static short Cyc_yyr1[517]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,183,183,183,184,185,185,186,186,186,186,187,187,187,188,188,189,189,189,189,190,190,190,191,191,192,192,193,193,194,194,194,194,194,194,194,194,194,194,195,195,195,195,195,195,195,195,195,195,195,196,196,197,198,198,199,199,199,199,199,199,199,199,200,200,200,201,201,202,202,202,203,203,203,204,204,205,205,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,212,212,212,213,213,214,214,215,215,215,216,217,217,218,218,219,219,219,219,220,220,220,220,221,222,222,223,223,224,224,225,225,225,225,225,226,226,227,227,227,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,229,230,230,231,232,232,233,233,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,281,281,281,281,281,281,281,281,281,281,282,283,283,284,284,284,284,284,285,285,286,286,287,287,288,288,289,289};
# 1663
static short Cyc_yyr2[517]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1718
static short Cyc_yydefact[1028]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,0,0,0,0,0,0,0,38,507,213,509,508,510,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,158,0,183,186,85,162,111,55,54,48,0,99,506,0,507,502,503,504,505,111,0,372,0,0,0,0,236,0,374,375,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,196,197,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,511,512,111,111,37,39,37,0,20,0,215,0,171,159,184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,507,382,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,491,492,471,0,0,0,0,0,474,472,473,0,399,401,415,423,425,427,429,431,433,436,441,444,447,451,0,453,475,490,489,0,383,381,33,0,0,111,0,0,0,129,125,127,254,256,0,0,9,10,0,0,0,111,513,514,214,94,0,0,163,73,234,0,231,0,3,0,5,0,284,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,0,0,0,0,0,507,287,289,0,295,291,0,293,277,278,279,0,280,281,282,0,40,22,122,261,0,221,237,0,0,217,215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,376,0,0,396,0,0,0,0,250,392,397,0,394,0,461,0,417,451,0,418,419,0,0,0,0,0,0,0,0,0,454,455,37,0,0,0,457,458,456,0,373,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,405,406,407,408,409,410,411,412,413,414,404,0,459,0,481,482,0,0,0,494,0,111,389,390,0,387,0,238,0,0,0,0,257,229,0,130,135,131,133,126,128,215,0,263,255,264,516,515,0,87,91,0,89,37,93,109,64,63,0,61,164,215,233,160,263,235,172,173,0,86,200,27,0,29,0,0,0,0,121,123,240,239,23,106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,324,325,326,0,0,328,0,0,0,0,296,292,122,294,290,288,0,170,222,0,0,0,228,216,223,167,0,0,0,217,169,203,202,165,201,0,0,207,59,112,0,105,422,102,98,385,386,253,0,393,249,251,380,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,493,500,0,499,400,424,0,426,428,430,432,434,435,439,440,437,438,442,443,445,446,448,449,450,403,402,480,477,0,479,0,0,0,377,0,384,32,0,378,0,0,258,136,132,134,0,217,0,201,0,265,0,215,0,276,260,0,36,111,0,0,0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,146,155,154,0,0,149,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,37,24,262,215,0,225,0,0,218,0,168,221,209,166,189,188,207,185,0,7,0,252,395,398,460,0,0,242,0,246,463,0,0,0,0,467,470,0,0,0,0,0,452,496,0,0,478,476,0,0,388,391,379,259,230,272,0,266,267,201,0,0,217,201,0,90,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,115,144,156,153,153,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,25,217,0,226,224,0,217,0,208,0,8,486,0,485,0,243,247,0,0,0,0,420,421,480,479,498,0,501,416,495,497,0,271,269,275,274,0,268,201,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,344,347,352,355,358,362,364,370,371,0,37,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,227,219,210,215,13,0,14,462,0,0,248,464,0,0,469,468,483,0,273,270,0,0,179,177,285,108,0,0,0,303,0,368,0,0,365,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,15,0,488,487,0,0,0,484,107,0,157,298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,350,351,348,349,353,354,356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,220,212,16,0,0,0,0,301,367,0,363,306,0,37,314,322,316,317,0,244,465,0,68,0,333,303,307,318,0,369,308,466,0,0,0};
# 1824
static short Cyc_yydefgoto[144]={1025,50,685,874,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,646,647,648,68,476,69,449,70,71,158,159,72,117,522,316,682,606,73,607,516,673,508,512,513,420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,293,294,295,759,296,297,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,212,213,214,783,215,563,564,216,217,75,875,244,435};
# 1842
static short Cyc_yypact[1028]={2656,- -32768,- -32768,- -32768,- -32768,48,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3391,208,3082,- -32768,- 16,63,- -32768,57,83,112,126,161,198,173,473,288,- -32768,- -32768,245,- -32768,- -32768,- -32768,235,174,332,358,346,346,- -32768,- -32768,2519,- -32768,406,485,- -32768,410,230,3391,3391,3391,- -32768,3391,- -32768,- -32768,552,- -32768,- 16,3228,440,46,316,878,- -32768,- -32768,385,443,480,- -32768,- 16,476,- -32768,4461,65,- -32768,- -32768,- -32768,- -32768,385,5848,- -32768,488,510,4461,527,539,548,- -32768,191,- -32768,- -32768,3464,3464,2519,2519,3464,581,5848,583,- -32768,277,570,300,601,277,3683,5848,- -32768,- -32768,- -32768,2519,2792,2519,2792,12,- -32768,589,588,- -32768,3146,- -32768,- -32768,- -32768,- -32768,3683,- -32768,- -32768,385,289,1669,- -32768,3228,230,- -32768,3464,1034,4218,- -32768,440,- -32768,- -32768,- -32768,604,620,- -32768,- -32768,- -32768,- -32768,124,878,3464,- -32768,2792,- -32768,622,646,642,- 16,111,- -32768,4461,141,795,6053,643,5848,5897,649,653,659,669,671,680,687,695,6102,6102,- -32768,- -32768,- -32768,2236,697,6151,6151,6151,- -32768,- -32768,- -32768,143,- -32768,- -32768,- 35,712,708,676,685,550,117,636,371,67,- -32768,1356,6151,193,- 49,- -32768,730,2975,751,- -32768,- -32768,753,5848,385,2975,737,317,3683,3756,3683,206,- -32768,99,99,- -32768,- -32768,- 20,763,292,385,- -32768,- -32768,- -32768,- -32768,62,738,- -32768,- -32768,1051,319,- -32768,768,- -32768,773,- -32768,775,774,790,- -32768,300,4316,3228,- -32768,779,3683,- -32768,384,788,- 16,791,786,229,796,3826,804,807,800,813,4414,3826,306,808,784,- -32768,- -32768,810,1812,1812,230,1812,- -32768,- -32768,- -32768,816,- -32768,- -32768,- -32768,164,- -32768,- -32768,815,802,- 9,840,- -32768,50,822,820,268,824,689,825,3464,5848,- -32768,843,- -32768,- -32768,- 9,836,- 16,- -32768,5848,860,- -32768,857,861,- -32768,277,5848,4461,240,- -32768,- -32768,- -32768,864,866,2236,- -32768,3683,- -32768,- -32768,1354,- -32768,897,5848,5848,5848,5848,5848,5848,3683,909,2236,- -32768,- -32768,1955,863,340,5848,- -32768,- -32768,- -32768,5848,- -32768,6151,5848,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,5848,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5848,- -32768,277,- -32768,- -32768,4509,277,5848,- -32768,868,385,- -32768,- -32768,867,874,4461,- -32768,421,795,870,3464,- -32768,876,879,- -32768,3756,3756,3756,- -32768,- -32768,2939,4604,207,- -32768,518,- -32768,- -32768,50,- -32768,- -32768,3464,- -32768,3310,- -32768,881,- -32768,873,875,885,- -32768,2866,- -32768,609,658,- -32768,- -32768,- -32768,3683,- -32768,- -32768,- -32768,2519,- -32768,2519,902,884,887,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,493,5848,890,892,- -32768,895,145,385,385,812,5848,5848,901,908,5848,1000,2098,912,- -32768,- -32768,- -32768,467,990,- -32768,4702,5848,3826,1244,- -32768,- -32768,3146,- -32768,- -32768,- -32768,3464,- -32768,- -32768,3683,910,343,- -32768,- -32768,900,- -32768,50,907,3610,820,- -32768,- -32768,- -32768,- -32768,689,- 31,914,793,- -32768,- -32768,2382,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,906,- -32768,- -32768,- -32768,- -32768,967,932,942,959,- -32768,- -32768,716,4316,939,946,947,950,400,949,954,286,958,965,5975,- -32768,- -32768,961,968,- -32768,712,351,708,676,685,550,117,117,636,636,636,636,371,371,67,67,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,962,- -32768,151,3464,4120,- -32768,6236,- -32768,- -32768,979,- -32768,66,991,- -32768,- -32768,- -32768,- -32768,978,820,981,689,976,518,3464,3537,4797,- -32768,- -32768,99,- -32768,385,724,983,62,3012,984,385,3464,1034,4895,- -32768,609,- -32768,- -32768,- -32768,993,- -32768,1010,713,- -32768,- -32768,384,5848,820,- -32768,- -32768,- -32768,999,- 16,541,438,460,5848,819,475,995,4993,5088,587,- -32768,- -32768,1002,1004,998,491,- -32768,3228,- -32768,802,1007,3464,- -32768,1006,50,- -32768,1008,- -32768,199,- -32768,- -32768,- -32768,- -32768,793,- -32768,1009,- -32768,2519,- -32768,- -32768,- -32768,1012,347,1011,- -32768,3924,- -32768,- -32768,5848,1109,5848,5897,- -32768,- -32768,277,277,1012,1014,4120,- -32768,- -32768,5848,5848,- -32768,- -32768,- 9,722,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1015,- -32768,- -32768,689,- 9,1017,820,689,1003,- -32768,820,495,1019,1022,- -32768,- -32768,1025,- -32768,- 9,1027,820,1031,689,1028,5848,1020,- -32768,- -32768,3683,- -32768,1038,75,812,3826,1040,1041,1105,1039,1050,3826,5848,5183,644,5278,664,5373,819,- -32768,1053,1055,- -32768,820,175,- -32768,- -32768,1062,820,3683,- -32768,517,- -32768,- -32768,401,- -32768,5848,- -32768,- -32768,4316,1049,1052,1057,- -32768,897,1054,1061,- -32768,740,- -32768,- -32768,- -32768,- -32768,4120,- -32768,- -32768,- -32768,- -32768,1063,- -32768,689,3683,1077,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1064,- -32768,- -32768,689,535,- -32768,1068,3683,- -32768,- -32768,1164,819,1073,6200,1069,1244,6151,1067,- -32768,- 21,- -32768,1103,1060,728,781,139,787,434,262,- -32768,- -32768,- -32768,- -32768,1110,6151,1955,- -32768,- -32768,547,3826,560,5468,3826,590,5563,5658,677,1079,- -32768,- -32768,- -32768,- -32768,1080,- -32768,1007,- -32768,1082,682,- -32768,430,355,- -32768,- -32768,3683,1179,- -32768,- -32768,- -32768,4022,- -32768,- -32768,1084,1085,- -32768,- -32768,- -32768,- -32768,596,3826,1087,819,2236,- -32768,3683,1083,- -32768,1526,6151,5848,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,6151,5848,- -32768,- -32768,1095,- -32768,3826,3826,610,- -32768,3826,3826,612,3826,618,5753,- -32768,50,820,- -32768,- -32768,583,- -32768,- -32768,5848,1092,1096,- -32768,- -32768,1099,- -32768,- -32768,- -32768,1100,1101,1104,6151,819,- -32768,712,374,708,676,676,550,117,117,636,636,636,636,371,371,67,67,- -32768,- -32768,- -32768,393,- -32768,- -32768,- -32768,3826,- -32768,- -32768,3826,- -32768,3826,3826,621,- -32768,- -32768,- -32768,754,1108,3683,1111,- -32768,1012,347,- -32768,- -32768,5848,1526,- -32768,- -32768,- -32768,- -32768,3826,- -32768,- -32768,1114,- -32768,553,- -32768,819,- -32768,- -32768,1116,- -32768,- -32768,- -32768,1223,1230,- -32768};
# 1948
static short Cyc_yypgoto[144]={- -32768,135,- -32768,303,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 101,16,- -32768,- -32768,0,626,- -32768,91,- 175,1118,5,- -32768,- -32768,- 131,- -32768,271,1209,- 696,- -32768,- -32768,- -32768,1013,986,586,412,- -32768,- -32768,627,- -32768,- -32768,132,- -32768,- -32768,86,- 151,1196,- 258,154,- -32768,1112,- -32768,- -32768,1220,- 376,- -32768,591,- 122,- 133,- 130,- 48,334,599,605,- 427,- 461,- 114,- 384,- 139,- -32768,- 234,- 164,- 552,- 295,- -32768,944,- 174,- 63,- 110,- 188,- 319,127,- -32768,- -32768,- -32768,- 22,- 270,- -32768,- 630,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 10,1058,- -32768,736,865,- -32768,- 7,531,- -32768,- 165,- 290,- 147,- 355,- 354,- 339,915,- 349,- 370,- 200,- 344,- 372,- 161,595,522,933,285,- -32768,- 357,- -32768,109,533,- 61,27,- 310,- 37};
# 1970
static short Cyc_yytable[6377]={59,250,572,573,136,62,310,616,580,581,341,566,344,96,568,308,358,499,500,536,502,59,571,307,140,317,62,465,262,345,569,361,362,363,578,579,324,78,233,234,537,714,237,430,112,589,143,136,549,671,319,59,398,824,423,405,62,366,59,59,59,451,59,62,62,62,432,62,59,147,631,905,169,62,130,131,132,432,133,442,644,645,198,220,142,140,406,559,674,587,38,60,137,590,419,288,679,299,448,41,239,367,437,59,59,433,434,263,62,62,611,509,60,505,889,906,433,434,257,300,59,59,59,59,668,62,62,62,62,59,113,611,97,631,62,240,258,865,662,59,719,59,60,129,62,142,62,678,147,60,60,60,38,60,797,289,76,142,531,60,325,41,59,39,543,531,544,62,443,100,510,170,44,511,574,575,576,577,556,518,558,101,638,359,- 237,432,119,- 237,453,171,530,97,630,718,60,60,- 150,436,246,897,438,382,97,434,374,375,453,102,409,383,384,60,60,60,60,409,414,170,299,630,60,582,583,584,433,149,913,914,301,422,60,103,60,723,424,171,434,39,235,236,468,376,377,288,288,104,288,423,423,423,524,38,315,60,326,253,254,255,256,40,41,42,59,915,916,364,506,62,956,327,365,471,492,364,961,399,400,401,142,97,38,97,528,504,712,105,364,59,59,41,59,43,62,62,756,62,624,320,549,107,44,511,869,289,289,507,289,531,47,48,49,224,434,288,38,694,527,225,306,402,624,538,106,41,403,404,- 215,97,79,- 215,1004,359,427,427,97,38,38,109,428,428,47,48,49,554,41,41,301,359,804,125,110,748,808,60,600,482,44,59,483,172,567,111,62,539,47,48,49,820,702,400,401,329,330,289,617,501,1019,419,60,60,784,60,511,241,250,429,625,306,1023,453,517,242,794,795,872,618,921,591,788,707,665,486,596,450,922,923,38,493,268,270,453,549,402,160,364,41,453,703,404,440,269,477,422,422,422,494,44,424,424,424,39,615,495,888,47,48,49,114,418,59,97,454,667,656,62,892,455,623,60,629,47,48,49,241,782,142,39,140,59,787,59,242,364,62,670,62,116,561,419,124,677,637,737,364,649,650,531,364,653,666,657,710,713,727,115,947,737,59,742,663,38,726,62,19,20,415,364,741,40,41,42,59,725,142,1005,740,62,788,380,160,471,441,381,674,472,364,739,142,47,48,49,97,364,1006,97,120,121,699,876,59,60,681,877,74,62,792,241,945,772,511,597,968,969,364,242,799,777,976,977,60,962,60,793,964,879,80,98,43,99,721,364,967,625,162,144,754,145,946,787,965,966,919,146,974,975,920,453,74,60,453,730,429,364,926,74,716,659,755,163,364,74,788,60,632,749,633,167,364,135,38,138,450,760,74,122,123,773,40,41,42,43,615,166,364,218,98,219,811,770,471,60,299,812,38,664,477,98,47,48,49,629,40,41,42,960,43,74,74,873,372,373,135,612,771,613,74,221,252,763,765,614,97,787,74,74,74,74,364,38,902,752,222,893,753,684,59,40,41,42,364,62,223,134,903,927,74,800,312,593,807,1022,142,364,810,877,59,681,929,805,38,62,925,784,818,238,74,245,40,41,42,166,249,816,98,97,98,766,948,259,364,260,895,364,970,971,972,973,933,454,136,267,378,379,953,868,957,43,958,247,871,313,249,364,626,364,627,1018,985,822,988,364,628,941,364,321,990,314,851,1011,642,643,98,520,521,856,858,60,861,98,864,978,979,980,859,38,323,364,322,339,340,342,342,40,41,42,346,60,477,878,347,355,356,452,862,447,348,364,342,342,342,428,477,467,74,477,349,938,350,1003,364,452,943,479,368,944,484,351,511,342,288,16,17,18,352,421,370,426,644,645,38,781,353,74,360,1014,371,198,40,41,42,732,733,734,81,603,604,605,692,693,471,1017,757,758,801,802,525,369,47,48,49,407,267,931,59,166,935,937,412,62,413,288,885,886,417,98,446,97,909,910,289,911,912,917,918,467,1012,364,550,551,552,553,828,555,136,826,827,82,855,439,456,731,562,359,994,457,565,459,461,328,963,469,168,84,463,59,85,86,87,478,62,41,480,481,488,497,88,585,981,342,489,289,89,485,90,91,342,329,330,586,92,487,992,490,562,496,93,498,503,261,434,996,150,151,152,60,98,507,81,98,514,515,519,153,154,155,156,157,529,523,610,43,342,288,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,452,533,534,452,928,541,535,932,542,366,560,74,592,74,594,60,595,599,601,620,602,619,621,635,452,81,59,622,634,639,452,62,636,249,640,38,84,187,188,85,86,87,289,954,41,641,562,651,652,562,654,658,660,249,675,354,669,672,191,686,249,680,7,8,9,10,11,12,13,14,15,16,17,18,19,20,983,984,82,689,986,987,74,989,690,691,695,342,696,697,687,22,698,168,84,98,700,85,86,87,467,701,41,27,249,704,705,88,708,711,709,28,29,89,249,90,91,60,329,330,717,92,32,249,39,720,34,93,722,724,735,738,1007,35,36,1008,746,1009,1010,745,750,761,767,768,467,769,306,774,98,780,831,832,706,776,790,785,809,304,1020,823,305,796,803,81,806,729,813,39,40,814,42,43,815,306,817,342,452,38,819,744,829,821,45,46,825,40,41,42,830,853,452,854,866,452,867,870,880,447,881,882,751,883,562,428,890,47,48,49,884,894,887,891,896,898,907,901,82,904,908,249,939,924,940,942,949,951,999,952,955,421,959,168,84,249,982,85,86,87,74,997,41,1000,998,1026,467,88,1002,789,1001,791,1027,833,248,90,91,1013,342,467,1015,92,798,1021,834,1024,108,93,195,995,736,196,197,470,7,8,9,10,11,12,13,14,15,16,17,18,19,20,148,747,118,173,174,318,779,464,993,778,540,775,688,852,598,22,81,416,175,570,557,1016,176,0,98,0,0,27,342,0,177,178,179,180,0,28,29,0,181,342,0,0,0,0,0,0,32,135,182,183,34,184,0,467,0,0,0,35,36,0,0,185,186,0,0,0,0,467,0,267,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,249,0,0,0,0,0,190,0,45,286,0,0,0,0,192,173,174,193,0,0,194,195,545,0,196,197,0,0,81,0,175,0,267,0,176,0,0,0,0,0,0,0,177,178,179,180,0,249,385,0,181,0,0,467,0,0,0,0,0,0,182,183,900,184,0,342,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,342,0,386,387,388,389,390,391,392,393,394,395,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,546,0,135,0,396,190,0,0,191,0,329,330,0,192,0,0,193,0,0,194,195,0,0,196,197,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,757,758,21,173,174,271,342,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,342,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 303,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,0,45,46,28,29,428,0,47,48,49,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,408,0,32,168,84,0,34,85,86,87,0,0,41,35,36,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,0,0,38,0,81,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,271,0,272,273,274,275,276,277,278,279,0,81,0,175,280,0,39,176,0,281,43,0,0,0,0,177,178,179,180,0,0,45,46,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,786,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,950,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,82,176,194,195,0,0,196,197,0,177,178,179,180,0,0,168,84,181,0,85,86,87,0,0,41,0,0,182,183,88,184,0,0,0,0,89,0,90,91,0,185,186,0,92,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,743,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,762,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,764,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,857,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,860,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,863,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,930,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,934,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,991,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,81,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,899,82,0,191,0,0,0,0,192,0,0,193,0,715,194,195,168,84,196,197,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93};
# 2611
static short Cyc_yycheck[6377]={0,115,372,373,65,0,145,434,380,381,175,366,176,23,368,145,190,287,288,329,290,21,371,145,68,158,21,261,129,176,369,192,193,194,378,379,167,21,101,102,330,593,105,231,44,402,68,108,343,510,160,51,213,749,229,104,51,92,58,59,60,249,62,58,59,60,86,62,68,69,454,92,82,68,58,59,60,86,62,17,5,6,89,93,68,129,135,357,515,399,106,0,65,403,227,139,127,141,249,115,107,136,122,103,104,125,137,129,103,104,429,61,21,122,810,136,125,137,106,141,120,121,122,123,508,120,121,122,123,129,44,450,23,517,129,108,124,767,495,139,601,141,51,57,139,129,141,523,148,58,59,60,106,62,706,139,108,141,323,68,170,115,162,113,338,330,340,162,106,106,120,106,126,306,374,375,376,377,352,309,354,124,472,190,119,86,51,122,249,124,321,82,450,127,103,104,121,234,112,829,237,134,93,137,87,88,267,124,218,142,143,120,121,122,123,225,223,106,262,477,129,382,383,384,125,71,87,88,142,229,139,119,141,609,229,124,137,113,103,104,262,124,125,287,288,119,290,422,423,424,313,106,128,162,113,120,121,122,123,114,115,116,262,124,125,122,303,262,898,128,127,126,279,122,904,82,83,84,262,170,106,172,319,119,133,124,122,287,288,115,290,117,287,288,651,290,447,162,593,126,126,434,127,287,288,106,290,472,134,135,136,120,137,357,106,549,316,126,119,126,471,331,124,115,131,132,127,218,120,130,960,338,126,126,225,106,106,49,132,132,134,135,136,350,115,115,260,354,724,119,105,641,728,262,417,126,126,357,129,88,367,126,357,123,134,135,136,743,82,83,84,131,132,357,437,289,1006,510,287,288,690,290,515,106,498,231,447,119,1018,450,122,114,702,703,778,439,134,404,693,560,501,274,412,249,142,143,106,280,137,138,471,706,126,124,122,115,477,131,132,127,131,267,422,423,424,119,126,422,423,424,113,431,126,809,134,135,136,105,121,439,331,122,505,487,439,821,127,447,357,449,134,135,136,106,107,439,113,501,458,693,460,114,122,458,121,460,120,127,601,59,518,471,623,122,481,482,641,122,485,501,487,130,592,613,126,130,637,487,627,496,106,613,487,20,21,224,122,627,114,115,116,501,612,487,130,627,501,802,137,124,126,240,141,940,130,122,626,501,134,135,136,412,122,130,415,119,120,127,127,529,439,526,131,0,529,699,106,107,668,672,119,911,912,122,114,710,676,919,920,458,905,460,699,907,788,22,23,117,25,607,122,910,623,120,124,127,126,877,802,908,909,137,132,917,918,141,637,44,487,640,617,427,122,853,51,595,119,127,108,122,57,886,501,458,642,460,120,122,65,106,67,447,127,70,119,120,669,114,115,116,117,611,79,122,126,82,106,122,127,126,529,665,127,106,497,471,93,134,135,136,630,114,115,116,904,117,103,104,121,89,90,108,124,665,126,112,119,116,655,656,132,542,886,120,121,122,123,122,106,833,119,122,127,122,529,665,114,115,116,122,665,123,120,834,127,142,713,146,407,727,127,665,122,731,131,685,681,127,725,106,685,852,1002,741,113,162,126,114,115,116,167,115,739,170,595,172,119,881,119,122,122,825,122,913,914,915,916,127,122,780,134,85,86,127,772,899,117,901,127,777,126,145,122,124,122,126,1006,127,745,127,122,132,872,122,122,127,126,757,127,478,479,218,63,64,761,762,665,764,225,766,921,922,923,119,106,123,122,121,173,126,175,176,114,115,116,126,685,623,785,126,185,186,249,119,126,126,122,192,193,194,132,637,261,260,640,126,119,126,959,122,267,119,269,91,122,272,126,940,213,853,17,18,19,126,228,139,230,5,6,106,685,126,289,126,998,140,833,114,115,116,106,107,108,38,422,423,424,121,122,126,1005,22,23,121,122,314,138,134,135,136,120,265,859,853,321,862,863,106,853,106,904,121,122,126,331,127,757,139,140,853,89,90,85,86,343,121,122,346,347,348,349,754,351,944,752,753,91,760,125,121,619,360,899,941,121,364,121,123,103,906,121,106,107,113,904,110,111,112,120,904,115,120,126,106,130,120,385,924,323,119,904,126,126,128,129,330,131,132,397,134,126,938,119,402,126,140,126,121,123,137,947,63,64,65,853,412,106,38,415,127,130,127,74,75,76,77,78,121,133,428,117,366,1006,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,447,121,125,450,857,121,125,860,122,92,127,458,124,460,127,904,122,127,122,126,121,120,127,119,471,38,1006,122,106,119,477,1006,125,427,122,106,107,108,109,110,111,112,1006,896,115,130,495,126,120,498,30,119,42,447,127,126,126,137,129,133,454,127,8,9,10,11,12,13,14,15,16,17,18,19,20,21,929,930,91,127,933,934,529,936,122,106,127,472,122,122,103,37,122,106,107,542,127,110,111,112,549,127,115,49,498,127,121,120,127,127,122,57,58,126,508,128,129,1006,131,132,121,134,68,517,113,127,72,140,127,133,127,127,985,79,80,988,106,990,991,126,121,126,120,119,593,127,119,121,595,120,25,26,120,125,25,124,133,103,1011,119,106,127,127,38,127,614,127,113,114,127,116,117,127,119,127,560,623,106,127,628,120,133,128,129,126,114,115,116,127,130,637,121,119,640,119,113,127,126,126,122,647,127,651,132,107,134,135,136,127,121,127,127,28,120,91,126,91,130,138,613,121,91,122,121,25,121,107,122,121,623,127,106,107,627,119,110,111,112,685,127,115,121,126,0,693,120,122,696,127,698,0,126,114,128,129,127,641,706,127,134,709,127,137,127,35,140,141,944,622,144,145,265,8,9,10,11,12,13,14,15,16,17,18,19,20,21,70,640,48,25,26,159,681,260,940,676,332,672,542,757,415,37,38,225,40,370,353,1002,44,- 1,757,- 1,- 1,49,699,- 1,52,53,54,55,- 1,57,58,- 1,60,710,- 1,- 1,- 1,- 1,- 1,- 1,68,780,70,71,72,73,- 1,788,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,802,- 1,749,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,778,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,25,26,137,- 1,- 1,140,141,32,- 1,144,145,- 1,- 1,38,- 1,40,- 1,810,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,825,56,- 1,60,- 1,- 1,886,- 1,- 1,- 1,- 1,- 1,- 1,70,71,831,73,- 1,834,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,852,- 1,93,94,95,96,97,98,99,100,101,102,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,944,- 1,123,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,905,- 1,907,908,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,959,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,1005,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,- 1,72,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,- 1,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,113,44,- 1,46,117,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,128,129,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,91,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,106,107,60,- 1,110,111,112,- 1,- 1,115,- 1,- 1,70,71,120,73,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,83,84,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,38,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,91,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple25{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple26{struct _dyneither_ptr f1;void*f2;};static char _tmp4DD[8]="stdcall";static char _tmp4DE[6]="cdecl";static char _tmp4DF[9]="fastcall";static char _tmp4E0[9]="noreturn";static char _tmp4E1[6]="const";static char _tmp4E2[8]="aligned";static char _tmp4E3[7]="packed";static char _tmp4E4[7]="shared";static char _tmp4E5[7]="unused";static char _tmp4E6[5]="weak";static char _tmp4E7[10]="dllimport";static char _tmp4E8[10]="dllexport";static char _tmp4E9[23]="no_instrument_function";static char _tmp4EA[12]="constructor";static char _tmp4EB[11]="destructor";static char _tmp4EC[22]="no_check_memory_usage";static char _tmp4ED[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp997(union Cyc_YYSTYPE*yylval,unsigned int*_tmp996,unsigned int*_tmp995,union Cyc_YYSTYPE**_tmp994){for(*_tmp996=0;*_tmp996 < *_tmp995;(*_tmp996)++){(*_tmp994)[*_tmp996]=*yylval;}}static void _tmp99C(unsigned int*_tmp99B,unsigned int*_tmp99A,struct Cyc_Yyltype**_tmp999){for(*_tmp99B=0;*_tmp99B < *_tmp99A;(*_tmp99B)++){(*_tmp999)[*_tmp99B]=
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9A3(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9A2,unsigned int*_tmp9A1,short**_tmp99F){for(*_tmp9A2=0;*_tmp9A2 < *_tmp9A1;(*_tmp9A2)++){(*_tmp99F)[*_tmp9A2]=
# 216
*_tmp9A2 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9A2)): 0;}}static void _tmp9A9(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9A8,unsigned int*_tmp9A7,union Cyc_YYSTYPE**_tmp9A5){for(*_tmp9A8=0;*_tmp9A8 < *_tmp9A7;(*_tmp9A8)++){(*_tmp9A5)[*_tmp9A8]=
# 219
*_tmp9A8 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9A8)):*yylval;}}static void _tmp9AF(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9AE,unsigned int*_tmp9AD,struct Cyc_Yyltype**_tmp9AB){for(*_tmp9AE=0;*_tmp9AE < *_tmp9AD;(*_tmp9AE)++){(*_tmp9AB)[*_tmp9AE]=
# 222
*_tmp9AE <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9AE)):
 Cyc_yynewloc();}}static void _tmpE89(unsigned int*_tmpE88,unsigned int*_tmpE87,char**_tmpE85){for(*_tmpE88=0;*_tmpE88 < *_tmpE87;(*_tmpE88)++){(*_tmpE85)[*_tmpE88]='\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp425=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp425;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp990;union Cyc_YYSTYPE yylval=((_tmp990.YYINITIALSVAL).val=0,(((_tmp990.YYINITIALSVAL).tag=62,_tmp990)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp992;unsigned int _tmp991;struct _dyneither_ptr yyss=(_tmp991=200,((_tmp992=_region_calloc(yyregion,sizeof(short),_tmp991),_tag_dyneither(_tmp992,sizeof(short),_tmp991))));
# 158
int yyvsp_offset;
unsigned int _tmp996;unsigned int _tmp995;union Cyc_YYSTYPE*_tmp994;unsigned int _tmp993;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp993=(unsigned int)200,((_tmp994=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp993)),((((_tmp995=_tmp993,_tmp997(& yylval,& _tmp996,& _tmp995,& _tmp994))),_tmp994)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp99B;unsigned int _tmp99A;struct Cyc_Yyltype*_tmp999;unsigned int _tmp998;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp998=(unsigned int)200,((_tmp999=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp998)),((((_tmp99A=_tmp998,_tmp99C(& _tmp99B,& _tmp99A,& _tmp999))),_tmp999)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=- 2;
# 193
yyssp_offset=- 1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
{const char*_tmp99D;Cyc_yyerror(((_tmp99D="parser stack overflow",_tag_dyneither(_tmp99D,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9A2;unsigned int _tmp9A1;struct _dyneither_ptr _tmp9A0;short*_tmp99F;unsigned int _tmp99E;struct _dyneither_ptr yyss1=(_tmp99E=(unsigned int)yystacksize,((_tmp99F=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp99E)),((_tmp9A0=_tag_dyneither(_tmp99F,sizeof(short),_tmp99E),((((_tmp9A1=_tmp99E,_tmp9A3(& yyssp_offset,& yyss,& _tmp9A2,& _tmp9A1,& _tmp99F))),_tmp9A0)))))));
# 217
unsigned int _tmp9A8;unsigned int _tmp9A7;struct _dyneither_ptr _tmp9A6;union Cyc_YYSTYPE*_tmp9A5;unsigned int _tmp9A4;struct _dyneither_ptr yyvs1=
(_tmp9A4=(unsigned int)yystacksize,((_tmp9A5=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A4)),((_tmp9A6=_tag_dyneither(_tmp9A5,sizeof(union Cyc_YYSTYPE),_tmp9A4),((((_tmp9A7=_tmp9A4,_tmp9A9(& yyvs,& yyssp_offset,& yylval,& _tmp9A8,& _tmp9A7,& _tmp9A5))),_tmp9A6)))))));
# 221
unsigned int _tmp9AE;unsigned int _tmp9AD;struct _dyneither_ptr _tmp9AC;struct Cyc_Yyltype*_tmp9AB;unsigned int _tmp9AA;struct _dyneither_ptr yyls1=(_tmp9AA=(unsigned int)yystacksize,((_tmp9AB=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9AA)),((_tmp9AC=_tag_dyneither(_tmp9AB,sizeof(struct Cyc_Yyltype),_tmp9AA),((((_tmp9AD=_tmp9AA,_tmp9AF(& yyssp_offset,& yyls,& _tmp9AE,& _tmp9AD,& _tmp9AB))),_tmp9AC)))))));
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 290;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6376) || Cyc_yycheck[_check_known_subscript_notnull(6377,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6377,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1027){
int _tmp433=0;_npop_handler(0);return _tmp433;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=- 2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1028,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(517,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL20A: {
# 1174 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1178
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY19(yyyyvsp[0]);
# 1181
break;}case 2: _LL20B: {
# 1180 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1184
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp434;_push_handler(& _tmp434);{int _tmp436=0;if(setjmp(_tmp434.handler))_tmp436=1;if(!_tmp436){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp435=(void*)_exn_thrown;void*_tmp438=_tmp435;void*_tmp43A;_LL20E: {struct Cyc_Core_Failure_exn_struct*_tmp439=(struct Cyc_Core_Failure_exn_struct*)_tmp438;if(_tmp439->tag != Cyc_Core_Failure)goto _LL210;}_LL20F:
# 1189
 x=0;goto _LL20D;_LL210: _tmp43A=_tmp438;_LL211:(void)_throw(_tmp43A);_LL20D:;}};}
# 1191
{struct _handler_cons _tmp43B;_push_handler(& _tmp43B);{int _tmp43D=0;if(setjmp(_tmp43B.handler))_tmp43D=1;if(!_tmp43D){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp43C=(void*)_exn_thrown;void*_tmp43F=_tmp43C;void*_tmp441;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp440=(struct Cyc_Core_Failure_exn_struct*)_tmp43F;if(_tmp440->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1195
 y=0;goto _LL212;_LL215: _tmp441=_tmp43F;_LL216:(void)_throw(_tmp441);_LL212:;}};}
# 1197
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1199
break;}case 3: _LL20C: {
# 1198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1202
{struct Cyc_Absyn_Decl*_tmp9B9;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9B8;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9B7;struct Cyc_List_List*_tmp9B6;yyval=Cyc_YY19(((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6->hd=((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->r=(void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=11,((_tmp9B8.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9B8.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9B8)))))),_tmp9B7)))),((_tmp9B9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9B9)))))),((_tmp9B6->tl=0,_tmp9B6)))))));}
Cyc_Lex_leave_using();
# 1205
break;}case 4: _LL217: {
# 1202 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1206
{struct Cyc_Absyn_Decl*_tmp9C3;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9C2;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9C1;struct Cyc_List_List*_tmp9C0;yyval=Cyc_YY19(((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0->hd=((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3->r=(void*)((_tmp9C1=_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1[0]=((_tmp9C2.tag=11,((_tmp9C2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9C2.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9C2)))))),_tmp9C1)))),((_tmp9C3->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9C3)))))),((_tmp9C0->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9C0)))))));}
break;}case 5: _LL218: {
# 1205 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1209
{struct Cyc_Absyn_Decl*_tmp9D2;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9D1;struct _dyneither_ptr*_tmp9D0;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9CF;struct Cyc_List_List*_tmp9CE;yyval=Cyc_YY19(((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE->hd=((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2->r=(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D1.tag=10,((_tmp9D1.f1=((_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((_tmp9D0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9D0)))),((_tmp9D1.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D1)))))),_tmp9CF)))),((_tmp9D2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D2)))))),((_tmp9CE->tl=0,_tmp9CE)))))));}
Cyc_Lex_leave_namespace();
# 1212
break;}case 6: _LL219: {
# 1209 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1214
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44F;_push_handler(& _tmp44F);{int _tmp451=0;if(setjmp(_tmp44F.handler))_tmp451=1;if(!_tmp451){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp450=(void*)_exn_thrown;void*_tmp453=_tmp450;void*_tmp455;_LL21C: {struct Cyc_Core_Failure_exn_struct*_tmp454=(struct Cyc_Core_Failure_exn_struct*)_tmp453;if(_tmp454->tag != Cyc_Core_Failure)goto _LL21E;}_LL21D:
{const char*_tmp9D3;nspace=((_tmp9D3="",_tag_dyneither(_tmp9D3,sizeof(char),1)));}goto _LL21B;_LL21E: _tmp455=_tmp453;_LL21F:(void)_throw(_tmp455);_LL21B:;}};}
# 1219
{struct _handler_cons _tmp457;_push_handler(& _tmp457);{int _tmp459=0;if(setjmp(_tmp457.handler))_tmp459=1;if(!_tmp459){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp458=(void*)_exn_thrown;void*_tmp45B=_tmp458;void*_tmp45D;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp45C=(struct Cyc_Core_Failure_exn_struct*)_tmp45B;if(_tmp45C->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
 x=0;goto _LL220;_LL223: _tmp45D=_tmp45B;_LL224:(void)_throw(_tmp45D);_LL220:;}};}
# 1222
{struct _handler_cons _tmp45E;_push_handler(& _tmp45E);{int _tmp460=0;if(setjmp(_tmp45E.handler))_tmp460=1;if(!_tmp460){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp45F=(void*)_exn_thrown;void*_tmp462=_tmp45F;void*_tmp464;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp463=(struct Cyc_Core_Failure_exn_struct*)_tmp462;if(_tmp463->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 y=0;goto _LL225;_LL228: _tmp464=_tmp462;_LL229:(void)_throw(_tmp464);_LL225:;}};}
# 1225
{struct Cyc_Absyn_Decl*_tmp9E2;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9E1;struct _dyneither_ptr*_tmp9E0;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9DF;struct Cyc_List_List*_tmp9DE;yyval=Cyc_YY19(((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE->hd=((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2->r=(void*)((_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF[0]=((_tmp9E1.tag=10,((_tmp9E1.f1=((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=nspace,_tmp9E0)))),((_tmp9E1.f2=x,_tmp9E1)))))),_tmp9DF)))),((_tmp9E2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E2)))))),((_tmp9DE->tl=y,_tmp9DE)))))));}
# 1227
break;}case 7: _LL21A: {
# 1224 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1228
struct _dyneither_ptr _tmp46A=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9E3;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46A,((_tmp9E3="C",_tag_dyneither(_tmp9E3,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmp9ED;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp9EC;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9EB;struct Cyc_List_List*_tmp9EA;yyval=Cyc_YY19(((_tmp9EA=_cycalloc(sizeof(*_tmp9EA)),((_tmp9EA->hd=((_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED->r=(void*)((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB[0]=((_tmp9EC.tag=12,((_tmp9EC.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmp9EC)))),_tmp9EB)))),((_tmp9ED->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9ED)))))),((_tmp9EA->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9EA)))))));}else{
# 1232
{const char*_tmp9EE;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46A,((_tmp9EE="C include",_tag_dyneither(_tmp9EE,sizeof(char),10))))!= 0){
const char*_tmp9EF;Cyc_Parse_err(((_tmp9EF="expecting \"C\" or \"C include\"",_tag_dyneither(_tmp9EF,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1235
struct Cyc_Absyn_Decl*_tmp9F9;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp9F8;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9F7;struct Cyc_List_List*_tmp9F6;yyval=Cyc_YY19(((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6->hd=((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9->r=(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=13,((_tmp9F8.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmp9F8.f2=0,_tmp9F8)))))),_tmp9F7)))),((_tmp9F9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F9)))))),((_tmp9F6->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9F6)))))));};}}
# 1238
break;}case 8: _LL22A: {
# 1235 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1239
{const char*_tmp9FA;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmp9FA="C include",_tag_dyneither(_tmp9FA,sizeof(char),10))))!= 0){
const char*_tmp9FB;Cyc_Parse_err(((_tmp9FB="expecting \"C include\"",_tag_dyneither(_tmp9FB,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY53(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA05;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA04;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA03;struct Cyc_List_List*_tmpA02;yyval=Cyc_YY19(((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->hd=((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->r=(void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=13,((_tmpA04.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA04.f2=exs,_tmpA04)))))),_tmpA03)))),((_tmpA05->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA05)))))),((_tmpA02->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA02)))))));}
# 1244
break;};}case 9: _LL22B: {
# 1241 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1245
{struct Cyc_Absyn_Decl*_tmpA08;struct Cyc_List_List*_tmpA07;yyval=Cyc_YY19(((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->hd=((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA08->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA08)))))),((_tmpA07->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA07)))))));}
break;}case 10: _LL22C: {
# 1243 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1247
{struct Cyc_Absyn_Decl*_tmpA0B;struct Cyc_List_List*_tmpA0A;yyval=Cyc_YY19(((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->hd=((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA0B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0B)))))),((_tmpA0A->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA0A)))))));}
break;}case 11: _LL22D:
# 1244 "parse.y"
 yyval=Cyc_YY19(0);
break;case 12: _LL22E: {
# 1248 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1252
yyval=yyyyvsp[2];
break;}case 13: _LL22F: {
# 1249 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1253
yyval=Cyc_YY53(0);
break;}case 14: _LL230: {
# 1253 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1257
{struct _tuple25*_tmpA0E;struct Cyc_List_List*_tmpA0D;yyval=Cyc_YY53(((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->hd=((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA0E->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0E->f3=0,_tmpA0E)))))))),((_tmpA0D->tl=0,_tmpA0D)))))));}
break;}case 15: _LL231: {
# 1254 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1258
{struct _tuple25*_tmpA11;struct Cyc_List_List*_tmpA10;yyval=Cyc_YY53(((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA11->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA11->f3=0,_tmpA11)))))))),((_tmpA10->tl=0,_tmpA10)))))));}
break;}case 16: _LL232: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1260
{struct _tuple25*_tmpA14;struct Cyc_List_List*_tmpA13;yyval=Cyc_YY53(((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->hd=((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA14->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA14->f3=0,_tmpA14)))))))),((_tmpA13->tl=Cyc_yyget_YY53(yyyyvsp[2]),_tmpA13)))))));}
break;}case 17: _LL233: {
# 1260 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1264
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA1A;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA19;struct Cyc_List_List*_tmpA18;yyval=Cyc_YY19(((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=Cyc_Absyn_new_decl((void*)((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA19.tag=1,((_tmpA19.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA19)))),_tmpA1A)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA18->tl=0,_tmpA18)))))));}
break;}case 18: _LL234: {
# 1261 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1265
yyval=yyyyvsp[0];
break;}case 19: _LL235: {
# 1262 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1266
yyval=Cyc_YY19(0);
break;}case 20: _LL236: {
# 1267 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1271
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 21: _LL237: {
# 1269 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1273
struct Cyc_Parse_Declaration_spec _tmp489=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp489,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 22: _LL238: {
# 1282 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1286
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL239: {
# 1284 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1288
struct Cyc_Parse_Declaration_spec _tmp48A=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48A,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23A: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1296
struct Cyc_Parse_Declaration_spec _tmp48B=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48B,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL23B: {
# 1295 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1299
struct Cyc_Parse_Declaration_spec _tmp48C=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48C,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23C: {
# 1300 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1304
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 27: _LL23D: {
# 1303 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1307
Cyc_Lex_leave_using();
break;}case 28: _LL23E: {
# 1306 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1310
{struct _dyneither_ptr*_tmpA1B;Cyc_Lex_enter_namespace(((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA1B)))));}yyval=yyyyvsp[1];
break;}case 29: _LL23F: {
# 1309 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1313
Cyc_Lex_leave_namespace();
break;}case 30: _LL240: {
# 1315 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1319
int _tmp48E=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp48E,(unsigned int)_tmp48E));
break;}case 31: _LL241: {
# 1318 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1322
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 32: _LL242: {
# 1321 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1325
{struct Cyc_List_List*_tmpA1C;yyval=Cyc_YY19(((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA1C->tl=0,_tmpA1C)))))));}
break;}case 33: _LL243: {
# 1323 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1327
struct Cyc_List_List*_tmp490=0;
{struct Cyc_List_List*_tmp491=Cyc_yyget_YY37(yyyyvsp[1]);for(0;_tmp491 != 0;_tmp491=_tmp491->tl){
struct _dyneither_ptr*_tmp492=(struct _dyneither_ptr*)_tmp491->hd;
struct _tuple0*_tmpA1D;struct _tuple0*qv=(_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->f1=Cyc_Absyn_Rel_n(0),((_tmpA1D->f2=_tmp492,_tmpA1D)))));
struct Cyc_Absyn_Vardecl*_tmp493=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA1E;_tmp490=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->hd=_tmp493,((_tmpA1E->tl=_tmp490,_tmpA1E))))));}}
# 1334
_tmp490=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp490);
{struct Cyc_List_List*_tmpA1F;yyval=Cyc_YY19(((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->hd=Cyc_Absyn_letv_decl(_tmp490,(unsigned int)(yyyylsp[0]).first_line),((_tmpA1F->tl=0,_tmpA1F)))))));}
# 1337
break;}case 34: _LL244: {
# 1336 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1340
struct _dyneither_ptr _tmp497=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA20;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,((_tmpA20="`H",_tag_dyneither(_tmpA20,sizeof(char),3))))== 0){
const char*_tmpA23;void*_tmpA22;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA22=0,Cyc_aprintf(((_tmpA23="bad occurrence of heap region",_tag_dyneither(_tmpA23,sizeof(char),30))),_tag_dyneither(_tmpA22,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA24;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,((_tmpA24="`U",_tag_dyneither(_tmpA24,sizeof(char),3))))== 0){
const char*_tmpA27;void*_tmpA26;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA26=0,Cyc_aprintf(((_tmpA27="bad occurrence of unique region",_tag_dyneither(_tmpA27,sizeof(char),32))),_tag_dyneither(_tmpA26,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA2A;struct Cyc_Absyn_Tvar*_tmpA29;struct Cyc_Absyn_Tvar*tv=(_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->name=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A[0]=_tmp497,_tmpA2A)))),((_tmpA29->identity=- 1,((_tmpA29->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA29)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA2D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA2C;void*t=(void*)((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=((_tmpA2D.tag=2,((_tmpA2D.f1=tv,_tmpA2D)))),_tmpA2C))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA33;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA32;struct _dyneither_ptr*_tmpA30;struct _tuple0*_tmpA2F;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F->f1=Cyc_Absyn_Loc_n,((_tmpA2F->f2=((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA30)))),_tmpA2F)))))),(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=15,((_tmpA33.f1=t,_tmpA33)))),_tmpA32)))),0);
{struct Cyc_List_List*_tmpA34;yyval=Cyc_YY19(((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY32(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA34->tl=0,_tmpA34)))))));}
# 1350
break;};}case 35: _LL245: {
# 1348 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1352
int _tmp4A7=Cyc_yyget_YY32(yyyyvsp[0]);
struct _dyneither_ptr _tmp4A8=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4A9=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA35;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A8,((_tmpA35="H",_tag_dyneither(_tmpA35,sizeof(char),2))))== 0){
const char*_tmpA36;Cyc_Parse_err(((_tmpA36="bad occurrence of heap region `H",_tag_dyneither(_tmpA36,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA37;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A8,((_tmpA37="U",_tag_dyneither(_tmpA37,sizeof(char),2))))== 0){
const char*_tmpA38;Cyc_Parse_err(((_tmpA38="bad occurrence of unique region `U",_tag_dyneither(_tmpA38,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4A7  && _tmp4A9 != 0){
const char*_tmpA39;Cyc_Parse_err(((_tmpA39="open regions cannot be @resetable",_tag_dyneither(_tmpA39,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA46;const char*_tmpA45;void*_tmpA44[1];struct Cyc_String_pa_PrintArg_struct _tmpA43;struct Cyc_Absyn_Tvar*_tmpA42;struct Cyc_Absyn_Tvar*tv=(_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->name=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=(struct _dyneither_ptr)((_tmpA43.tag=0,((_tmpA43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A8),((_tmpA44[0]=& _tmpA43,Cyc_aprintf(((_tmpA45="`%s",_tag_dyneither(_tmpA45,sizeof(char),4))),_tag_dyneither(_tmpA44,sizeof(void*),1)))))))),_tmpA46)))),((_tmpA42->identity=- 1,((_tmpA42->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA42)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA49;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA48;void*t=(void*)((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=2,((_tmpA49.f1=tv,_tmpA49)))),_tmpA48))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA4F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA4E;struct _dyneither_ptr*_tmpA4C;struct _tuple0*_tmpA4B;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B->f1=Cyc_Absyn_Loc_n,((_tmpA4B->f2=((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=_tmp4A8,_tmpA4C)))),_tmpA4B)))))),(void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=15,((_tmpA4F.f1=t,_tmpA4F)))),_tmpA4E)))),0);
# 1366
{struct Cyc_List_List*_tmpA50;yyval=Cyc_YY19(((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4A7,_tmp4A9,(unsigned int)(yyyylsp[0]).first_line),((_tmpA50->tl=0,_tmpA50)))))));}
# 1368
break;};}case 36: _LL246: {
# 1366 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1370
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA58;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA57;struct _dyneither_ptr*_tmpA56;struct Cyc_Absyn_Tvar*_tmpA55;struct Cyc_Absyn_Tvar*tv=(_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->name=((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA56)))),((_tmpA55->identity=- 1,((_tmpA55->kind=(void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA57.tag=0,((_tmpA57.f1=& Cyc_Tcutil_rk,_tmpA57)))),_tmpA58)))),_tmpA55)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA5B;void*_tmpA5A;(_tmpA5A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA5B="too many declarations in alias",_tag_dyneither(_tmpA5B,sizeof(char),31))),_tag_dyneither(_tmpA5A,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_tmp4BD=d->r;struct Cyc_Absyn_Vardecl*_tmp4BF;_LL249: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4BE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4BD;if(_tmp4BE->tag != 0)goto _LL24B;else{_tmp4BF=_tmp4BE->f1;}}_LL24A:
# 1377
{struct Cyc_List_List*_tmpA5C;yyval=Cyc_YY19(((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->hd=Cyc_Absyn_alias_decl(tv,_tmp4BF,(unsigned int)(yyyylsp[0]).first_line),((_tmpA5C->tl=0,_tmpA5C)))))));}
goto _LL248;_LL24B:;_LL24C:
# 1380
{const char*_tmpA5D;Cyc_Parse_err(((_tmpA5D="expecting variable declaration",_tag_dyneither(_tmpA5D,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY19(0);_LL248:;}
# 1384
break;};}case 37: _LL247:
# 1383 "parse.y"
 yyval=Cyc_YY32(0);
break;case 38: _LL24D: {
# 1384 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1388
yyval=Cyc_YY32(1);
break;}case 39: _LL24E: {
# 1389 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1393
yyval=yyyyvsp[0];
break;}case 40: _LL24F: {
# 1391 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1395
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 41: _LL250: {
# 1397 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1401
{struct Cyc_Parse_Declaration_spec _tmpA5E;yyval=Cyc_YY20(((_tmpA5E.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA5E.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA5E.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA5E.is_inline=0,((_tmpA5E.attributes=0,_tmpA5E)))))))))));}
break;}case 42: _LL251: {
# 1400 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1404
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4C7.sc != 0){
const char*_tmpA61;void*_tmpA60;(_tmpA60=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA61="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA61,sizeof(char),51))),_tag_dyneither(_tmpA60,sizeof(void*),0)));}
# 1408
{struct Cyc_Parse_Declaration_spec _tmpA62;yyval=Cyc_YY20(((_tmpA62.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA62.tq=_tmp4C7.tq,((_tmpA62.type_specs=_tmp4C7.type_specs,((_tmpA62.is_inline=_tmp4C7.is_inline,((_tmpA62.attributes=_tmp4C7.attributes,_tmpA62)))))))))));}
# 1412
break;}case 43: _LL252: {
# 1409 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1413
{struct Cyc_Parse_Declaration_spec _tmpA63;yyval=Cyc_YY20(((_tmpA63.sc=0,((_tmpA63.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA63.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA63.is_inline=0,((_tmpA63.attributes=0,_tmpA63)))))))))));}
break;}case 44: _LL253: {
# 1412 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1416
struct Cyc_Parse_Declaration_spec _tmp4CC=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA64;yyval=Cyc_YY20(((_tmpA64.sc=_tmp4CC.sc,((_tmpA64.tq=_tmp4CC.tq,((_tmpA64.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4CC.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA64.is_inline=_tmp4CC.is_inline,((_tmpA64.attributes=_tmp4CC.attributes,_tmpA64)))))))))));}
# 1422
break;}case 45: _LL254: {
# 1419 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1423
{struct Cyc_Parse_Declaration_spec _tmpA65;yyval=Cyc_YY20(((_tmpA65.sc=0,((_tmpA65.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA65.type_specs=Cyc_Parse_empty_spec(0),((_tmpA65.is_inline=0,((_tmpA65.attributes=0,_tmpA65)))))))))));}
break;}case 46: _LL255: {
# 1421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1425
struct Cyc_Parse_Declaration_spec _tmp4CF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA66;yyval=Cyc_YY20(((_tmpA66.sc=_tmp4CF.sc,((_tmpA66.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4CF.tq),((_tmpA66.type_specs=_tmp4CF.type_specs,((_tmpA66.is_inline=_tmp4CF.is_inline,((_tmpA66.attributes=_tmp4CF.attributes,_tmpA66)))))))))));}
# 1430
break;}case 47: _LL256: {
# 1427 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1431
{struct Cyc_Parse_Declaration_spec _tmpA67;yyval=Cyc_YY20(((_tmpA67.sc=0,((_tmpA67.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA67.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA67.is_inline=1,((_tmpA67.attributes=0,_tmpA67)))))))))));}
break;}case 48: _LL257: {
# 1430 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1434
struct Cyc_Parse_Declaration_spec _tmp4D2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA68;yyval=Cyc_YY20(((_tmpA68.sc=_tmp4D2.sc,((_tmpA68.tq=_tmp4D2.tq,((_tmpA68.type_specs=_tmp4D2.type_specs,((_tmpA68.is_inline=1,((_tmpA68.attributes=_tmp4D2.attributes,_tmpA68)))))))))));}
# 1438
break;}case 49: _LL258: {
# 1435 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1439
{struct Cyc_Parse_Declaration_spec _tmpA69;yyval=Cyc_YY20(((_tmpA69.sc=0,((_tmpA69.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA69.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA69.is_inline=0,((_tmpA69.attributes=Cyc_yyget_YY46(yyyyvsp[0]),_tmpA69)))))))))));}
break;}case 50: _LL259: {
# 1438 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1442
struct Cyc_Parse_Declaration_spec _tmp4D5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA6A;yyval=Cyc_YY20(((_tmpA6A.sc=_tmp4D5.sc,((_tmpA6A.tq=_tmp4D5.tq,((_tmpA6A.type_specs=_tmp4D5.type_specs,((_tmpA6A.is_inline=_tmp4D5.is_inline,((_tmpA6A.attributes=
# 1445
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp4D5.attributes),_tmpA6A)))))))))));}
break;}case 51: _LL25A: {
# 1445 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 52: _LL25B: {
# 1447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1451
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 53: _LL25C: {
# 1449 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1453
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 54: _LL25D: {
# 1451 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1455
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 55: _LL25E: {
# 1454 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1458
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA6B;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA6B="C",_tag_dyneither(_tmpA6B,sizeof(char),2))))!= 0){
const char*_tmpA6C;Cyc_Parse_err(((_tmpA6C="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA6C,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1463
break;}case 56: _LL25F: {
# 1459 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1463
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 57: _LL260: {
# 1462 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1466
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 58: _LL261:
# 1468 "parse.y"
 yyval=Cyc_YY46(0);
break;case 59: _LL262: {
# 1469 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1473
yyval=yyyyvsp[0];
break;}case 60: _LL263: {
# 1474 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1478
yyval=yyyyvsp[3];
break;}case 61: _LL264: {
# 1478 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1482
{struct Cyc_List_List*_tmpA6D;yyval=Cyc_YY46(((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpA6D->tl=0,_tmpA6D)))))));}
break;}case 62: _LL265: {
# 1479 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1483
{struct Cyc_List_List*_tmpA6E;yyval=Cyc_YY46(((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpA6E->tl=Cyc_yyget_YY46(yyyyvsp[2]),_tmpA6E)))))));}
break;}case 63: _LL266: {
# 1484 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1488
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple26 att_map[17]={{{_tmp4DD,_tmp4DD,_tmp4DD + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4DE,_tmp4DE,_tmp4DE + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4DF,_tmp4DF,_tmp4DF + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E0,_tmp4E0,_tmp4E0 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 8},(void*)& att_aligned},{{_tmp4E3,_tmp4E3,_tmp4E3 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1508
struct _dyneither_ptr _tmp4DB=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1510
if((((_get_dyneither_size(_tmp4DB,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp4DB,sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DB,sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DB,sizeof(char),(int)(_get_dyneither_size(_tmp4DB,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DB,sizeof(char),(int)(_get_dyneither_size(_tmp4DB,sizeof(char))- 3)))== '_')
# 1512
_tmp4DB=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DB,2,_get_dyneither_size(_tmp4DB,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DB,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY47((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1519
if(i == 17){
{const char*_tmpA6F;Cyc_Parse_err(((_tmpA6F="unrecognized attribute",_tag_dyneither(_tmpA6F,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY47((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1524
break;};}case 64: _LL267: {
# 1520 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1524
yyval=Cyc_YY47((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL268: {
# 1522 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1526
struct _dyneither_ptr _tmp4EF=Cyc_yyget_String_tok(yyyyvsp[0]);
int _tmp4F1;struct _tuple5 _tmp4F0=Cyc_yyget_Int_tok(yyyyvsp[2]);_tmp4F1=_tmp4F0.f2;{
void*a;
{const char*_tmpA71;const char*_tmpA70;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA71="regparm",_tag_dyneither(_tmpA71,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA70="__regparm__",_tag_dyneither(_tmpA70,sizeof(char),12))))== 0){
if(_tmp4F1 < 0  || _tmp4F1 > 3){
const char*_tmpA72;Cyc_Parse_err(((_tmpA72="regparm requires value between 0 and 3",_tag_dyneither(_tmpA72,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA75;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA74;a=(void*)((_tmpA74=_cycalloc_atomic(sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=0,((_tmpA75.f1=_tmp4F1,_tmpA75)))),_tmpA74))));};}else{
const char*_tmpA77;const char*_tmpA76;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA77="aligned",_tag_dyneither(_tmpA77,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA76="__aligned__",_tag_dyneither(_tmpA76,sizeof(char),12))))== 0){
if(_tmp4F1 < 0){const char*_tmpA78;Cyc_Parse_err(((_tmpA78="aligned requires positive power of two",_tag_dyneither(_tmpA78,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4F1;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA79;Cyc_Parse_err(((_tmpA79="aligned requires positive power of two",_tag_dyneither(_tmpA79,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA7C;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA7B;a=(void*)((_tmpA7B=_cycalloc_atomic(sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7C.tag=6,((_tmpA7C.f1=_tmp4F1,_tmpA7C)))),_tmpA7B))));};};}else{
const char*_tmpA7E;const char*_tmpA7D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA7E="initializes",_tag_dyneither(_tmpA7E,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA7D="__initializes__",_tag_dyneither(_tmpA7D,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA81;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA80;a=(void*)((_tmpA80=_cycalloc_atomic(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=20,((_tmpA81.f1=_tmp4F1,_tmpA81)))),_tmpA80))));}else{
const char*_tmpA83;const char*_tmpA82;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA83="noliveunique",_tag_dyneither(_tmpA83,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA82="__noliveunique__",_tag_dyneither(_tmpA82,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA86;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA85;a=(void*)((_tmpA85=_cycalloc_atomic(sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA86.tag=21,((_tmpA86.f1=_tmp4F1,_tmpA86)))),_tmpA85))));}else{
const char*_tmpA88;const char*_tmpA87;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA88="noconsume",_tag_dyneither(_tmpA88,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA87="__noconsume__",_tag_dyneither(_tmpA87,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA8B;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA8A;a=(void*)((_tmpA8A=_cycalloc_atomic(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=22,((_tmpA8B.f1=_tmp4F1,_tmpA8B)))),_tmpA8A))));}else{
# 1548
{const char*_tmpA8C;Cyc_Parse_err(((_tmpA8C="unrecognized attribute",_tag_dyneither(_tmpA8C,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1551
yyval=Cyc_YY47(a);
# 1553
break;};}case 66: _LL269: {
# 1550 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1554
struct _dyneither_ptr _tmp50A=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50B=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpA8E;const char*_tmpA8D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50A,((_tmpA8E="section",_tag_dyneither(_tmpA8E,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50A,((_tmpA8D="__section__",_tag_dyneither(_tmpA8D,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA91;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpA90;a=(void*)((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=8,((_tmpA91.f1=_tmp50B,_tmpA91)))),_tmpA90))));}else{
# 1560
{const char*_tmpA92;Cyc_Parse_err(((_tmpA92="unrecognized attribute",_tag_dyneither(_tmpA92,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1563
yyval=Cyc_YY47(a);
# 1565
break;}case 67: _LL26A: {
# 1562 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1566
struct _dyneither_ptr _tmp511=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp512=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1570
{const char*_tmpA93;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp511,((_tmpA93="__mode__",_tag_dyneither(_tmpA93,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA96;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpA95;a=(void*)((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA96.tag=24,((_tmpA96.f1=_tmp512,_tmpA96)))),_tmpA95))));}else{
# 1573
{const char*_tmpA97;Cyc_Parse_err(((_tmpA97="unrecognized attribute",_tag_dyneither(_tmpA97,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1576
yyval=Cyc_YY47(a);
# 1578
break;}case 68: _LL26B: {
# 1575 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1579
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp518=Cyc_yyget_String_tok(yyyyvsp[2]);
int _tmp51A;struct _tuple5 _tmp519=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp51A=_tmp519.f2;{
int _tmp51C;struct _tuple5 _tmp51B=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp51C=_tmp51B.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpA99;const char*_tmpA98;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpA99="format",_tag_dyneither(_tmpA99,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpA98="__format__",_tag_dyneither(_tmpA98,sizeof(char),11))))== 0){
const char*_tmpA9A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,((_tmpA9A="printf",_tag_dyneither(_tmpA9A,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA9D;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA9C;a=(void*)((_tmpA9C=_cycalloc_atomic(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=19,((_tmpA9D.f1=Cyc_Absyn_Printf_ft,((_tmpA9D.f2=_tmp51A,((_tmpA9D.f3=_tmp51C,_tmpA9D)))))))),_tmpA9C))));}else{
const char*_tmpA9E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,((_tmpA9E="scanf",_tag_dyneither(_tmpA9E,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAA1;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAA0;a=(void*)((_tmpAA0=_cycalloc_atomic(sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA1.tag=19,((_tmpAA1.f1=Cyc_Absyn_Scanf_ft,((_tmpAA1.f2=_tmp51A,((_tmpAA1.f3=_tmp51C,_tmpAA1)))))))),_tmpAA0))));}else{
# 1590
const char*_tmpAA2;Cyc_Parse_err(((_tmpAA2="unrecognized format type",_tag_dyneither(_tmpAA2,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1592
const char*_tmpAA3;Cyc_Parse_err(((_tmpAA3="unrecognized attribute",_tag_dyneither(_tmpAA3,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY47(a);
# 1595
break;};};}case 69: _LL26C: {
# 1600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604
yyval=yyyyvsp[0];
break;}case 70: _LL26D: {
# 1602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1606
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAA6;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAA5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=17,((_tmpAA6.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAA6.f2=Cyc_yyget_YY41(yyyyvsp[1]),((_tmpAA6.f3=0,((_tmpAA6.f4=0,_tmpAA6)))))))))),_tmpAA5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 71: _LL26E: {
# 1606 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 72: _LL26F: {
# 1607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL270: {
# 1609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1613
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY44(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL271: {
# 1610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL272: {
# 1611 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL273: {
# 1612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL274: {
# 1613 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL275: {
# 1614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL276: {
# 1615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL277: {
# 1616 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1620
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL278: {
# 1617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1621
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL279: {
# 1618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622
yyval=yyyyvsp[0];
break;}case 83: _LL27A: {
# 1619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1623
yyval=yyyyvsp[0];
break;}case 84: _LL27B: {
# 1621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1625
yyval=yyyyvsp[0];
break;}case 85: _LL27C: {
# 1623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1627
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY45(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 86: _LL27D: {
# 1626 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1630
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAA9;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAA8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=10,((_tmpAA9.f1=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[2]))),_tmpAA9)))),_tmpAA8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1633
break;}case 87: _LL27E: {
# 1630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1634
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAAC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAAB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAC.tag=15,((_tmpAAC.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAAC)))),_tmpAAB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL27F: {
# 1632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1636
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAAF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAAE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=15,((_tmpAAF.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAAF)))),_tmpAAE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1638
break;}case 89: _LL280: {
# 1635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1639
void*_tmp52F=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAB2;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=16,((_tmpAB2.f1=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpAB2.f2=_tmp52F,_tmpAB2)))))),_tmpAB1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1642
break;}case 90: _LL281: {
# 1639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1643
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAB5;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=16,((_tmpAB5.f1=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpAB5.f2=Cyc_yyget_YY45(yyyyvsp[4]),_tmpAB5)))))),_tmpAB4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL282: {
# 1641 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1645
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAB8;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAB7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=((_tmpAB8.tag=19,((_tmpAB8.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAB8)))),_tmpAB7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL283: {
# 1643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1647
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpABB;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpABA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=19,((_tmpABB.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpABB)))),_tmpABA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL284: {
# 1645 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1649
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpABE;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpABD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=18,((_tmpABE.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpABE)))),_tmpABD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL285: {
# 1650 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654
yyval=Cyc_YY44(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 95: _LL286: {
# 1654 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpABF;yyval=Cyc_YY26(((_tmpABF.print_const=1,((_tmpABF.q_volatile=0,((_tmpABF.q_restrict=0,((_tmpABF.real_const=1,((_tmpABF.loc=loc,_tmpABF)))))))))));}
break;}case 96: _LL287: {
# 1656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660
{struct Cyc_Absyn_Tqual _tmpAC0;yyval=Cyc_YY26(((_tmpAC0.print_const=0,((_tmpAC0.q_volatile=1,((_tmpAC0.q_restrict=0,((_tmpAC0.real_const=0,((_tmpAC0.loc=0,_tmpAC0)))))))))));}
break;}case 97: _LL288: {
# 1657 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1661
{struct Cyc_Absyn_Tqual _tmpAC1;yyval=Cyc_YY26(((_tmpAC1.print_const=0,((_tmpAC1.q_volatile=0,((_tmpAC1.q_restrict=1,((_tmpAC1.real_const=0,((_tmpAC1.loc=0,_tmpAC1)))))))))));}
break;}case 98: _LL289: {
# 1663 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1667
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAD0;struct Cyc_Absyn_Enumdecl*_tmpACF;struct Cyc_Core_Opt*_tmpACE;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpACD;struct Cyc_Absyn_TypeDecl*_tmpACC;struct Cyc_Absyn_TypeDecl*_tmp53D=(_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->r=(void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpACD.tag=1,((_tmpACD.f1=((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->sc=Cyc_Absyn_Public,((_tmpACF->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpACF->fields=((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->v=Cyc_yyget_YY49(yyyyvsp[3]),_tmpACE)))),_tmpACF)))))))),_tmpACD)))),_tmpAD0)))),((_tmpACC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpACC)))));
# 1669
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAD3;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAD2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=26,((_tmpAD3.f1=_tmp53D,((_tmpAD3.f2=0,_tmpAD3)))))),_tmpAD2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1671
break;}case 99: _LL28A: {
# 1668 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1672
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAD6;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAD5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD6.tag=13,((_tmpAD6.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAD6.f2=0,_tmpAD6)))))),_tmpAD5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL28B: {
# 1670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1674
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAD9;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAD8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=14,((_tmpAD9.f1=Cyc_yyget_YY49(yyyyvsp[2]),_tmpAD9)))),_tmpAD8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL28C: {
# 1676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1680
{struct Cyc_Absyn_Enumfield*_tmpADA;yyval=Cyc_YY48(((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpADA->tag=0,((_tmpADA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpADA)))))))));}
break;}case 102: _LL28D: {
# 1678 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1682
{struct Cyc_Absyn_Enumfield*_tmpADB;yyval=Cyc_YY48(((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpADB->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpADB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpADB)))))))));}
break;}case 103: _LL28E: {
# 1682 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1686
{struct Cyc_List_List*_tmpADC;yyval=Cyc_YY49(((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpADC->tl=0,_tmpADC)))))));}
break;}case 104: _LL28F: {
# 1683 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1687
{struct Cyc_List_List*_tmpADD;yyval=Cyc_YY49(((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpADD->tl=0,_tmpADD)))))));}
break;}case 105: _LL290: {
# 1684 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1688
{struct Cyc_List_List*_tmpADE;yyval=Cyc_YY49(((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpADE->tl=Cyc_yyget_YY49(yyyyvsp[2]),_tmpADE)))))));}
break;}case 106: _LL291: {
# 1690 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1694
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAE1;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE0;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0[0]=((_tmpAE1.tag=12,((_tmpAE1.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAE1.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAE1)))))),_tmpAE0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 107: _LL292: {
# 1695 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1700
struct Cyc_List_List*_tmp550=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY41(yyyyvsp[3]));
struct Cyc_List_List*_tmp551=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY41(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp552=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp550,
Cyc_Absyn_aggrdecl_impl(_tmp551,Cyc_yyget_YY51(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1705
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE4;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE3;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=((_tmpAE4.tag=26,((_tmpAE4.f1=_tmp552,((_tmpAE4.f2=0,_tmpAE4)))))),_tmpAE3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1707
break;}case 108: _LL293: {
# 1706 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1711
struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY41(yyyyvsp[2]));
struct Cyc_List_List*_tmp556=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY41(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp557=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp555,
Cyc_Absyn_aggrdecl_impl(_tmp556,Cyc_yyget_YY51(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1716
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE7;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE7.tag=26,((_tmpAE7.f1=_tmp557,((_tmpAE7.f2=0,_tmpAE7)))))),_tmpAE6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1718
break;}case 109: _LL294: {
# 1715 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1719
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAF1;struct Cyc_Core_Opt*_tmpAF0;struct Cyc_Absyn_AggrInfo _tmpAEF;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAEE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAF1.tag=11,((_tmpAF1.f1=((_tmpAEF.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpAF0=_cycalloc_atomic(sizeof(*_tmpAF0)),((_tmpAF0->v=(void*)1,_tmpAF0))))),((_tmpAEF.targs=Cyc_yyget_YY41(yyyyvsp[3]),_tmpAEF)))),_tmpAF1)))),_tmpAEE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1722
break;}case 110: _LL295: {
# 1719 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1723
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAF7;struct Cyc_Absyn_AggrInfo _tmpAF6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAF5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=((_tmpAF7.tag=11,((_tmpAF7.f1=((_tmpAF6.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpAF6.targs=Cyc_yyget_YY41(yyyyvsp[2]),_tmpAF6)))),_tmpAF7)))),_tmpAF5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1726
break;}case 111: _LL296:
# 1726 "parse.y"
 yyval=Cyc_YY41(0);
break;case 112: _LL297: {
# 1728 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1732
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[1])));
break;}case 113: _LL298: {
# 1732 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1736
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 114: _LL299: {
# 1733 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1737
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 115: _LL29A:
# 1738 "parse.y"
 yyval=Cyc_YY27(0);
break;case 116: _LL29B: {
# 1740 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1744
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp561=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp561 != 0;_tmp561=_tmp561->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp561->hd,decls);}}{
# 1748
struct Cyc_List_List*_tmp562=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp562 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp562,decls);
yyval=Cyc_YY27(decls);
# 1753
break;};}case 117: _LL29C: {
# 1754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1758
{struct Cyc_List_List*_tmpAF8;yyval=Cyc_YY28(((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpAF8->tl=0,_tmpAF8)))))));}
break;}case 118: _LL29D: {
# 1756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1760
{struct Cyc_List_List*_tmpAF9;yyval=Cyc_YY28(((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpAF9->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpAF9)))))));}
break;}case 119: _LL29E: {
# 1760 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1764
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 120: _LL29F: {
# 1766 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1770
{struct _tuple12*_tmpAFA;yyval=Cyc_YY22(((_tmpAFA=_region_malloc(yyr,sizeof(*_tmpAFA)),((_tmpAFA->tl=0,((_tmpAFA->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpAFA)))))));}
break;}case 121: _LL2A0: {
# 1768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1772
{struct _tuple12*_tmpAFB;yyval=Cyc_YY22(((_tmpAFB=_region_malloc(yyr,sizeof(*_tmpAFB)),((_tmpAFB->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpAFB->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpAFB)))))));}
break;}case 122: _LL2A1: {
# 1773 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1777
{struct _tuple11 _tmpAFC;yyval=Cyc_YY21(((_tmpAFC.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpAFC.f2=0,_tmpAFC)))));}
break;}case 123: _LL2A2: {
# 1775 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1779
{struct _tuple11 _tmpAFD;yyval=Cyc_YY21(((_tmpAFD.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpAFD.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAFD)))));}
break;}case 124: _LL2A3: {
# 1780 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1792 "parse.y"
struct _RegionHandle _tmp569=_new_region("temp");struct _RegionHandle*temp=& _tmp569;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp56B;struct Cyc_Parse_Type_specifier _tmp56C;struct Cyc_List_List*_tmp56D;struct _tuple23 _tmp56A=Cyc_yyget_YY36(yyyyvsp[0]);_tmp56B=_tmp56A.f1;_tmp56C=_tmp56A.f2;_tmp56D=_tmp56A.f3;
if(_tmp56B.loc == 0)_tmp56B.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths=0;
Cyc_Parse_decl_split(temp,Cyc_yyget_YY22(yyyyvsp[1]),& decls,& widths);{
void*_tmp56E=Cyc_Parse_speclist2typ(_tmp56C,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp56F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp56B,_tmp56E,decls,_tmp56D),widths);
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple16*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp56F));};};}
# 1804
_npop_handler(0);break;
# 1792
;_pop_region(temp);}case 125: _LL2A4: {
# 1808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812
{struct _tuple23 _tmpAFE;yyval=Cyc_YY36(((_tmpAFE.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFE.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpAFE.f3=0,_tmpAFE)))))));}
break;}case 126: _LL2A5: {
# 1810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814
struct _tuple23 _tmp571=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpAFF;yyval=Cyc_YY36(((_tmpAFF.f1=_tmp571.f1,((_tmpAFF.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp571.f2),((_tmpAFF.f3=_tmp571.f3,_tmpAFF)))))));}
break;}case 127: _LL2A6: {
# 1812 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1816
{struct _tuple23 _tmpB00;yyval=Cyc_YY36(((_tmpB00.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB00.f2=Cyc_Parse_empty_spec(0),((_tmpB00.f3=0,_tmpB00)))))));}
break;}case 128: _LL2A7: {
# 1814 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1818
struct _tuple23 _tmp574=Cyc_yyget_YY36(yyyyvsp[1]);
{struct _tuple23 _tmpB01;yyval=Cyc_YY36(((_tmpB01.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp574.f1),((_tmpB01.f2=_tmp574.f2,((_tmpB01.f3=_tmp574.f3,_tmpB01)))))));}
break;}case 129: _LL2A8: {
# 1817 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1821
{struct _tuple23 _tmpB02;yyval=Cyc_YY36(((_tmpB02.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB02.f2=Cyc_Parse_empty_spec(0),((_tmpB02.f3=Cyc_yyget_YY46(yyyyvsp[0]),_tmpB02)))))));}
break;}case 130: _LL2A9: {
# 1819 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1823
struct _tuple23 _tmp577=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB03;yyval=Cyc_YY36(((_tmpB03.f1=_tmp577.f1,((_tmpB03.f2=_tmp577.f2,((_tmpB03.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp577.f3),_tmpB03)))))));}
break;}case 131: _LL2AA: {
# 1825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829
{struct _tuple23 _tmpB04;yyval=Cyc_YY36(((_tmpB04.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB04.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB04.f3=0,_tmpB04)))))));}
break;}case 132: _LL2AB: {
# 1827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831
struct _tuple23 _tmp57A=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB05;yyval=Cyc_YY36(((_tmpB05.f1=_tmp57A.f1,((_tmpB05.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp57A.f2),((_tmpB05.f3=_tmp57A.f3,_tmpB05)))))));}
break;}case 133: _LL2AC: {
# 1829 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1833
{struct _tuple23 _tmpB06;yyval=Cyc_YY36(((_tmpB06.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB06.f2=Cyc_Parse_empty_spec(0),((_tmpB06.f3=0,_tmpB06)))))));}
break;}case 134: _LL2AD: {
# 1831 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1835
struct _tuple23 _tmp57D=Cyc_yyget_YY36(yyyyvsp[1]);
{struct _tuple23 _tmpB07;yyval=Cyc_YY36(((_tmpB07.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp57D.f1),((_tmpB07.f2=_tmp57D.f2,((_tmpB07.f3=_tmp57D.f3,_tmpB07)))))));}
break;}case 135: _LL2AE: {
# 1834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1838
{struct _tuple23 _tmpB08;yyval=Cyc_YY36(((_tmpB08.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB08.f2=Cyc_Parse_empty_spec(0),((_tmpB08.f3=Cyc_yyget_YY46(yyyyvsp[0]),_tmpB08)))))));}
break;}case 136: _LL2AF: {
# 1836 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1840
struct _tuple23 _tmp580=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB09;yyval=Cyc_YY36(((_tmpB09.f1=_tmp580.f1,((_tmpB09.f2=_tmp580.f2,((_tmpB09.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp580.f3),_tmpB09)))))));}
break;}case 137: _LL2B0: {
# 1840 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1844
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 138: _LL2B1: {
# 1846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1850
{struct _tuple12*_tmpB0A;yyval=Cyc_YY22(((_tmpB0A=_region_malloc(yyr,sizeof(*_tmpB0A)),((_tmpB0A->tl=0,((_tmpB0A->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB0A)))))));}
break;}case 139: _LL2B2: {
# 1848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1852
{struct _tuple12*_tmpB0B;yyval=Cyc_YY22(((_tmpB0B=_region_malloc(yyr,sizeof(*_tmpB0B)),((_tmpB0B->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB0B->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB0B)))))));}
break;}case 140: _LL2B3: {
# 1853 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1857
{struct _tuple11 _tmpB0C;yyval=Cyc_YY21(((_tmpB0C.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB0C.f2=0,_tmpB0C)))));}
break;}case 141: _LL2B4: {
# 1855 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861
{struct Cyc_Parse_Declarator _tmpB12;struct _tuple0*_tmpB11;struct _tuple11 _tmpB10;yyval=Cyc_YY21(((_tmpB10.f1=((_tmpB12.id=((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->f1=Cyc_Absyn_Rel_n(0),((_tmpB11->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB11)))))),((_tmpB12.tms=0,_tmpB12)))),((_tmpB10.f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpB10)))));}
# 1863
break;}case 142: _LL2B5:
# 1862 "parse.y"
{struct Cyc_Parse_Declarator _tmpB18;struct _tuple0*_tmpB17;struct _tuple11 _tmpB16;yyval=Cyc_YY21(((_tmpB16.f1=((_tmpB18.id=((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->f1=Cyc_Absyn_Rel_n(0),((_tmpB17->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB17)))))),((_tmpB18.tms=0,_tmpB18)))),((_tmpB16.f2=0,_tmpB16)))));}
# 1864
break;case 143: _LL2B6: {
# 1865 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1869
{struct _tuple11 _tmpB19;yyval=Cyc_YY21(((_tmpB19.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB19.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB19)))));}
break;}case 144: _LL2B7: {
# 1871 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1875
int _tmp58E=Cyc_yyget_YY32(yyyyvsp[0]);
struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY41(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB1A;struct Cyc_Absyn_TypeDecl*_tmp590=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp58F,((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->v=Cyc_yyget_YY35(yyyyvsp[4]),_tmpB1A)))),_tmp58E,(unsigned int)(yyyylsp[0]).first_line);
# 1879
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB1D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1C;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=26,((_tmpB1D.f1=_tmp590,((_tmpB1D.f2=0,_tmpB1D)))))),_tmpB1C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1881
break;}case 145: _LL2B8: {
# 1878 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1882
int _tmp594=Cyc_yyget_YY32(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB27;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB26;struct Cyc_Absyn_DatatypeInfo _tmpB25;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB24;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB27.tag=3,((_tmpB27.f1=((_tmpB25.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB26.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB26.is_extensible=_tmp594,_tmpB26))))),((_tmpB25.targs=Cyc_yyget_YY41(yyyyvsp[2]),_tmpB25)))),_tmpB27)))),_tmpB24)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1885
break;}case 146: _LL2B9: {
# 1882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1886
int _tmp599=Cyc_yyget_YY32(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB31;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB30;struct Cyc_Absyn_DatatypeFieldInfo _tmpB2F;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB2E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((_tmpB31.tag=4,((_tmpB31.f1=((_tmpB2F.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB30.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB30.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB30.is_extensible=_tmp599,_tmpB30))))))),((_tmpB2F.targs=Cyc_yyget_YY41(yyyyvsp[4]),_tmpB2F)))),_tmpB31)))),_tmpB2E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1891
break;}case 147: _LL2BA: {
# 1890 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1894
yyval=Cyc_YY32(0);
break;}case 148: _LL2BB: {
# 1891 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1895
yyval=Cyc_YY32(1);
break;}case 149: _LL2BC: {
# 1895 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1899
{struct Cyc_List_List*_tmpB32;yyval=Cyc_YY35(((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB32->tl=0,_tmpB32)))))));}
break;}case 150: _LL2BD: {
# 1896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1900
{struct Cyc_List_List*_tmpB33;yyval=Cyc_YY35(((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB33->tl=0,_tmpB33)))))));}
break;}case 151: _LL2BE: {
# 1897 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1901
{struct Cyc_List_List*_tmpB34;yyval=Cyc_YY35(((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB34->tl=Cyc_yyget_YY35(yyyyvsp[2]),_tmpB34)))))));}
break;}case 152: _LL2BF: {
# 1898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1902
{struct Cyc_List_List*_tmpB35;yyval=Cyc_YY35(((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB35->tl=Cyc_yyget_YY35(yyyyvsp[2]),_tmpB35)))))));}
break;}case 153: _LL2C0:
# 1902 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Public);
break;case 154: _LL2C1: {
# 1903 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1907
yyval=Cyc_YY33(Cyc_Absyn_Extern);
break;}case 155: _LL2C2: {
# 1904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1908
yyval=Cyc_YY33(Cyc_Absyn_Static);
break;}case 156: _LL2C3: {
# 1908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1912
{struct Cyc_Absyn_Datatypefield*_tmpB36;yyval=Cyc_YY34(((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB36->typs=0,((_tmpB36->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB36->sc=Cyc_yyget_YY33(yyyyvsp[0]),_tmpB36)))))))))));}
break;}case 157: _LL2C4: {
# 1910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1914
struct Cyc_List_List*_tmp5A3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB37;yyval=Cyc_YY34(((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB37->typs=_tmp5A3,((_tmpB37->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB37->sc=Cyc_yyget_YY33(yyyyvsp[0]),_tmpB37)))))))))));}
break;}case 158: _LL2C5: {
# 1916 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1920
yyval=yyyyvsp[0];
break;}case 159: _LL2C6: {
# 1918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1922
struct Cyc_Parse_Declarator _tmp5A5=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB38;yyval=Cyc_YY30(((_tmpB38.id=_tmp5A5.id,((_tmpB38.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5A5.tms),_tmpB38)))));}
break;}case 160: _LL2C7: {
# 1925 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1929
yyval=yyyyvsp[0];
break;}case 161: _LL2C8: {
# 1927 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1931
struct Cyc_Parse_Declarator _tmp5A7=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB39;yyval=Cyc_YY30(((_tmpB39.id=_tmp5A7.id,((_tmpB39.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5A7.tms),_tmpB39)))));}
break;}case 162: _LL2C9: {
# 1933 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1937
{struct Cyc_Parse_Declarator _tmpB3A;yyval=Cyc_YY30(((_tmpB3A.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB3A.tms=0,_tmpB3A)))));}
break;}case 163: _LL2CA: {
# 1935 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1939
yyval=yyyyvsp[1];
break;}case 164: _LL2CB: {
# 1939 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1943
struct Cyc_Parse_Declarator _tmp5AA=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB40;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB3F;struct Cyc_List_List*_tmpB3E;_tmp5AA.tms=((_tmpB3E=_region_malloc(yyr,sizeof(*_tmpB3E)),((_tmpB3E->hd=(void*)((_tmpB40=_region_malloc(yyr,sizeof(*_tmpB40)),((_tmpB40[0]=((_tmpB3F.tag=5,((_tmpB3F.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB3F.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpB3F)))))),_tmpB40)))),((_tmpB3E->tl=_tmp5AA.tms,_tmpB3E))))));}
yyval=yyyyvsp[2];
# 1947
break;}case 165: _LL2CC: {
# 1944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1948
{struct Cyc_List_List*_tmpB4A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB49;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB48;struct Cyc_Parse_Declarator _tmpB47;yyval=Cyc_YY30(((_tmpB47.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB47.tms=((_tmpB4A=_region_malloc(yyr,sizeof(*_tmpB4A)),((_tmpB4A->hd=(void*)((_tmpB48=_region_malloc(yyr,sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB49.tag=0,((_tmpB49.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpB49.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB49)))))),_tmpB48)))),((_tmpB4A->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB4A)))))),_tmpB47)))));}
break;}case 166: _LL2CD: {
# 1946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1950
{struct Cyc_List_List*_tmpB54;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB53;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB52;struct Cyc_Parse_Declarator _tmpB51;yyval=Cyc_YY30(((_tmpB51.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB51.tms=(
(_tmpB54=_region_malloc(yyr,sizeof(*_tmpB54)),((_tmpB54->hd=(void*)((_tmpB52=_region_malloc(yyr,sizeof(*_tmpB52)),((_tmpB52[0]=((_tmpB53.tag=1,((_tmpB53.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB53.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpB53.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB53)))))))),_tmpB52)))),((_tmpB54->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB54)))))),_tmpB51)))));}
break;}case 167: _LL2CE: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953
struct Cyc_List_List*_tmp5B7;int _tmp5B8;struct Cyc_Absyn_VarargInfo*_tmp5B9;void*_tmp5BA;struct Cyc_List_List*_tmp5BB;struct _tuple24*_tmp5B6=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;_tmp5B9=_tmp5B6->f3;_tmp5BA=_tmp5B6->f4;_tmp5BB=_tmp5B6->f5;
{struct Cyc_List_List*_tmpB69;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB68;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB67;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB66;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB65;struct Cyc_Parse_Declarator _tmpB64;yyval=Cyc_YY30(((_tmpB64.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB64.tms=((_tmpB69=_region_malloc(yyr,sizeof(*_tmpB69)),((_tmpB69->hd=(void*)((_tmpB65=_region_malloc(yyr,sizeof(*_tmpB65)),((_tmpB65[0]=((_tmpB68.tag=3,((_tmpB68.f1=(void*)((_tmpB66=_region_malloc(yyr,sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB67.tag=1,((_tmpB67.f1=_tmp5B7,((_tmpB67.f2=_tmp5B8,((_tmpB67.f3=_tmp5B9,((_tmpB67.f4=_tmp5BA,((_tmpB67.f5=_tmp5BB,_tmpB67)))))))))))),_tmpB66)))),_tmpB68)))),_tmpB65)))),((_tmpB69->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB69)))))),_tmpB64)))));}
# 1956
break;}case 168: _LL2CF: {
# 1953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1957
{struct Cyc_List_List*_tmpB7E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB7D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB7C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB7B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB7A;struct Cyc_Parse_Declarator _tmpB79;yyval=Cyc_YY30(((_tmpB79.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB79.tms=(
(_tmpB7E=_region_malloc(yyr,sizeof(*_tmpB7E)),((_tmpB7E->hd=(void*)((_tmpB7A=_region_malloc(yyr,sizeof(*_tmpB7A)),((_tmpB7A[0]=((_tmpB7D.tag=3,((_tmpB7D.f1=(void*)((_tmpB7B=_region_malloc(yyr,sizeof(*_tmpB7B)),((_tmpB7B[0]=((_tmpB7C.tag=1,((_tmpB7C.f1=0,((_tmpB7C.f2=0,((_tmpB7C.f3=0,((_tmpB7C.f4=
# 1960
Cyc_yyget_YY50(yyyyvsp[2]),((_tmpB7C.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB7C)))))))))))),_tmpB7B)))),_tmpB7D)))),_tmpB7A)))),((_tmpB7E->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB7E)))))),_tmpB79)))));}
# 1963
break;}case 169: _LL2D0: {
# 1960 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1964
{struct Cyc_List_List*_tmpB93;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB92;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpB91;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB90;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB8F;struct Cyc_Parse_Declarator _tmpB8E;yyval=Cyc_YY30(((_tmpB8E.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB8E.tms=((_tmpB93=_region_malloc(yyr,sizeof(*_tmpB93)),((_tmpB93->hd=(void*)((_tmpB8F=_region_malloc(yyr,sizeof(*_tmpB8F)),((_tmpB8F[0]=((_tmpB92.tag=3,((_tmpB92.f1=(void*)((_tmpB90=_region_malloc(yyr,sizeof(*_tmpB90)),((_tmpB90[0]=((_tmpB91.tag=0,((_tmpB91.f1=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpB91.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpB91)))))),_tmpB90)))),_tmpB92)))),_tmpB8F)))),((_tmpB93->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB93)))))),_tmpB8E)))));}
break;}case 170: _LL2D1: {
# 1963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1967
struct Cyc_List_List*_tmp5CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
{struct Cyc_List_List*_tmpB9D;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpB9C;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpB9B;struct Cyc_Parse_Declarator _tmpB9A;yyval=Cyc_YY30(((_tmpB9A.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB9A.tms=((_tmpB9D=_region_malloc(yyr,sizeof(*_tmpB9D)),((_tmpB9D->hd=(void*)((_tmpB9B=_region_malloc(yyr,sizeof(*_tmpB9B)),((_tmpB9B[0]=((_tmpB9C.tag=4,((_tmpB9C.f1=_tmp5CE,((_tmpB9C.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpB9C.f3=0,_tmpB9C)))))))),_tmpB9B)))),((_tmpB9D->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB9D)))))),_tmpB9A)))));}
# 1970
break;}case 171: _LL2D2: {
# 1967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1971
{struct Cyc_List_List*_tmpBA7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBA6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBA5;struct Cyc_Parse_Declarator _tmpBA4;yyval=Cyc_YY30(((_tmpBA4.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA4.tms=((_tmpBA7=_region_malloc(yyr,sizeof(*_tmpBA7)),((_tmpBA7->hd=(void*)((_tmpBA5=_region_malloc(yyr,sizeof(*_tmpBA5)),((_tmpBA5[0]=((_tmpBA6.tag=5,((_tmpBA6.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBA6.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpBA6)))))),_tmpBA5)))),((_tmpBA7->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBA7)))))),_tmpBA4)))));}
# 1974
break;}case 172: _LL2D3: {
# 1981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1985
{struct Cyc_Parse_Declarator _tmpBA8;yyval=Cyc_YY30(((_tmpBA8.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA8.tms=0,_tmpBA8)))));}
break;}case 173: _LL2D4: {
# 1983 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1987
{struct Cyc_Parse_Declarator _tmpBA9;yyval=Cyc_YY30(((_tmpBA9.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA9.tms=0,_tmpBA9)))));}
break;}case 174: _LL2D5: {
# 1985 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1989
yyval=yyyyvsp[1];
break;}case 175: _LL2D6: {
# 1989 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1993
struct Cyc_Parse_Declarator _tmp5D9=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBAF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBAE;struct Cyc_List_List*_tmpBAD;_tmp5D9.tms=((_tmpBAD=_region_malloc(yyr,sizeof(*_tmpBAD)),((_tmpBAD->hd=(void*)((_tmpBAF=_region_malloc(yyr,sizeof(*_tmpBAF)),((_tmpBAF[0]=((_tmpBAE.tag=5,((_tmpBAE.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBAE.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpBAE)))))),_tmpBAF)))),((_tmpBAD->tl=_tmp5D9.tms,_tmpBAD))))));}
yyval=yyyyvsp[2];
# 1997
break;}case 176: _LL2D7: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1998
struct Cyc_Parse_Declarator _tmp5DD=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBB9;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBB8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBB7;struct Cyc_Parse_Declarator _tmpBB6;yyval=Cyc_YY30(((_tmpBB6.id=_tmp5DD.id,((_tmpBB6.tms=(
(_tmpBB9=_region_malloc(yyr,sizeof(*_tmpBB9)),((_tmpBB9->hd=(void*)((_tmpBB7=_region_malloc(yyr,sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=0,((_tmpBB8.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpBB8.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBB8)))))),_tmpBB7)))),((_tmpBB9->tl=_tmp5DD.tms,_tmpBB9)))))),_tmpBB6)))));}
break;}case 177: _LL2D8: {
# 1998 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2002
struct Cyc_Parse_Declarator _tmp5E2=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBC3;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBC2;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBC1;struct Cyc_Parse_Declarator _tmpBC0;yyval=Cyc_YY30(((_tmpBC0.id=_tmp5E2.id,((_tmpBC0.tms=(
(_tmpBC3=_region_malloc(yyr,sizeof(*_tmpBC3)),((_tmpBC3->hd=(void*)((_tmpBC1=_region_malloc(yyr,sizeof(*_tmpBC1)),((_tmpBC1[0]=((_tmpBC2.tag=1,((_tmpBC2.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBC2.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpBC2.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBC2)))))))),_tmpBC1)))),((_tmpBC3->tl=_tmp5E2.tms,_tmpBC3)))))),_tmpBC0)))));}
# 2006
break;}case 178: _LL2D9: {
# 2003 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2007
struct Cyc_List_List*_tmp5E8;int _tmp5E9;struct Cyc_Absyn_VarargInfo*_tmp5EA;void*_tmp5EB;struct Cyc_List_List*_tmp5EC;struct _tuple24*_tmp5E7=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;_tmp5EA=_tmp5E7->f3;_tmp5EB=_tmp5E7->f4;_tmp5EC=_tmp5E7->f5;{
struct Cyc_Parse_Declarator _tmp5ED=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBD6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBD5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBD4;struct Cyc_Parse_Declarator _tmpBD3;yyval=Cyc_YY30(((_tmpBD3.id=_tmp5ED.id,((_tmpBD3.tms=((_tmpBD8=_region_malloc(yyr,sizeof(*_tmpBD8)),((_tmpBD8->hd=(void*)((_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD7.tag=3,((_tmpBD7.f1=(void*)((_tmpBD5=_region_malloc(yyr,sizeof(*_tmpBD5)),((_tmpBD5[0]=((_tmpBD6.tag=1,((_tmpBD6.f1=_tmp5E8,((_tmpBD6.f2=_tmp5E9,((_tmpBD6.f3=_tmp5EA,((_tmpBD6.f4=_tmp5EB,((_tmpBD6.f5=_tmp5EC,_tmpBD6)))))))))))),_tmpBD5)))),_tmpBD7)))),_tmpBD4)))),((_tmpBD8->tl=_tmp5ED.tms,_tmpBD8)))))),_tmpBD3)))));}
# 2011
break;};}case 179: _LL2DA: {
# 2008 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2012
struct Cyc_Parse_Declarator _tmp5F4=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBED;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBEC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBEB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBEA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE9;struct Cyc_Parse_Declarator _tmpBE8;yyval=Cyc_YY30(((_tmpBE8.id=_tmp5F4.id,((_tmpBE8.tms=(
(_tmpBED=_region_malloc(yyr,sizeof(*_tmpBED)),((_tmpBED->hd=(void*)((_tmpBE9=_region_malloc(yyr,sizeof(*_tmpBE9)),((_tmpBE9[0]=((_tmpBEC.tag=3,((_tmpBEC.f1=(void*)((_tmpBEA=_region_malloc(yyr,sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBEB.tag=1,((_tmpBEB.f1=0,((_tmpBEB.f2=0,((_tmpBEB.f3=0,((_tmpBEB.f4=
# 2016
Cyc_yyget_YY50(yyyyvsp[2]),((_tmpBEB.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpBEB)))))))))))),_tmpBEA)))),_tmpBEC)))),_tmpBE9)))),((_tmpBED->tl=_tmp5F4.tms,_tmpBED)))))),_tmpBE8)))));}
# 2019
break;}case 180: _LL2DB: {
# 2016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2020
struct Cyc_Parse_Declarator _tmp5FB=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC02;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC01;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC00;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBFF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBFE;struct Cyc_Parse_Declarator _tmpBFD;yyval=Cyc_YY30(((_tmpBFD.id=_tmp5FB.id,((_tmpBFD.tms=((_tmpC02=_region_malloc(yyr,sizeof(*_tmpC02)),((_tmpC02->hd=(void*)((_tmpBFE=_region_malloc(yyr,sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpC01.tag=3,((_tmpC01.f1=(void*)((_tmpBFF=_region_malloc(yyr,sizeof(*_tmpBFF)),((_tmpBFF[0]=((_tmpC00.tag=0,((_tmpC00.f1=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpC00.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC00)))))),_tmpBFF)))),_tmpC01)))),_tmpBFE)))),((_tmpC02->tl=_tmp5FB.tms,_tmpC02)))))),_tmpBFD)))));}
break;}case 181: _LL2DC: {
# 2020 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2024
struct Cyc_List_List*_tmp602=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp603=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0C;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC0B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC0A;struct Cyc_Parse_Declarator _tmpC09;yyval=Cyc_YY30(((_tmpC09.id=_tmp603.id,((_tmpC09.tms=((_tmpC0C=_region_malloc(yyr,sizeof(*_tmpC0C)),((_tmpC0C->hd=(void*)((_tmpC0A=_region_malloc(yyr,sizeof(*_tmpC0A)),((_tmpC0A[0]=((_tmpC0B.tag=4,((_tmpC0B.f1=_tmp602,((_tmpC0B.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC0B.f3=0,_tmpC0B)))))))),_tmpC0A)))),((_tmpC0C->tl=_tmp603.tms,_tmpC0C)))))),_tmpC09)))));}
# 2028
break;}case 182: _LL2DD: {
# 2025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2029
struct Cyc_Parse_Declarator _tmp608=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC16;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC15;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC14;struct Cyc_Parse_Declarator _tmpC13;yyval=Cyc_YY30(((_tmpC13.id=_tmp608.id,((_tmpC13.tms=((_tmpC16=_region_malloc(yyr,sizeof(*_tmpC16)),((_tmpC16->hd=(void*)((_tmpC14=_region_malloc(yyr,sizeof(*_tmpC14)),((_tmpC14[0]=((_tmpC15.tag=5,((_tmpC15.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC15.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpC15)))))),_tmpC14)))),((_tmpC16->tl=_tmp608.tms,_tmpC16)))))),_tmpC13)))));}
# 2032
break;}case 183: _LL2DE: {
# 2038 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2042
yyval=yyyyvsp[0];
break;}case 184: _LL2DF: {
# 2039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2043
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 185: _LL2E0: {
# 2043 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2047
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC1C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC1B;struct Cyc_List_List*_tmpC1A;ans=((_tmpC1A=_region_malloc(yyr,sizeof(*_tmpC1A)),((_tmpC1A->hd=(void*)((_tmpC1C=_region_malloc(yyr,sizeof(*_tmpC1C)),((_tmpC1C[0]=((_tmpC1B.tag=5,((_tmpC1B.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC1B.f2=Cyc_yyget_YY46(yyyyvsp[3]),_tmpC1B)))))),_tmpC1C)))),((_tmpC1A->tl=ans,_tmpC1A))))));}{
# 2051
struct Cyc_Absyn_PtrLoc*ptrloc=0;
unsigned int _tmp611;union Cyc_Absyn_Constraint*_tmp612;union Cyc_Absyn_Constraint*_tmp613;struct _tuple20 _tmp610=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp611=_tmp610.f1;_tmp612=_tmp610.f2;_tmp613=_tmp610.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC1D;ptrloc=((_tmpC1D=_cycalloc_atomic(sizeof(*_tmpC1D)),((_tmpC1D->ptr_loc=_tmp611,((_tmpC1D->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC1D->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC1D))))))));}{
# 2056
union Cyc_Absyn_Constraint*_tmp616;union Cyc_Absyn_Constraint*_tmp617;union Cyc_Absyn_Constraint*_tmp618;void*_tmp619;struct _tuple14 _tmp615=Cyc_Parse_collapse_pointer_quals(_tmp611,_tmp612,_tmp613,Cyc_yyget_YY45(yyyyvsp[2]),Cyc_yyget_YY55(yyyyvsp[1]));_tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_tmp618=_tmp615.f3;_tmp619=_tmp615.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC27;struct Cyc_Absyn_PtrAtts _tmpC26;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC25;struct Cyc_List_List*_tmpC24;ans=((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24->hd=(void*)((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC25.tag=2,((_tmpC25.f1=((_tmpC26.rgn=_tmp619,((_tmpC26.nullable=_tmp616,((_tmpC26.bounds=_tmp617,((_tmpC26.zero_term=_tmp618,((_tmpC26.ptrloc=ptrloc,_tmpC26)))))))))),((_tmpC25.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC25)))))),_tmpC27)))),((_tmpC24->tl=ans,_tmpC24))))));}
yyval=Cyc_YY29(ans);
# 2060
break;};};}case 186: _LL2E1:
# 2058 "parse.y"
 yyval=Cyc_YY55(0);
break;case 187: _LL2E2: {
# 2059 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2063
{struct Cyc_List_List*_tmpC28;yyval=Cyc_YY55(((_tmpC28=_region_malloc(yyr,sizeof(*_tmpC28)),((_tmpC28->hd=Cyc_yyget_YY54(yyyyvsp[0]),((_tmpC28->tl=Cyc_yyget_YY55(yyyyvsp[1]),_tmpC28)))))));}
break;}case 188: _LL2E3: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2068
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC2B;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC2A;yyval=Cyc_YY54((void*)((_tmpC2A=_region_malloc(yyr,sizeof(*_tmpC2A)),((_tmpC2A[0]=((_tmpC2B.tag=0,((_tmpC2B.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC2B)))),_tmpC2A)))));}
break;}case 189: _LL2E4: {
# 2066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2070
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC2E;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC2D;yyval=Cyc_YY54((void*)((_tmpC2D=_region_malloc(yyr,sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2E.tag=1,((_tmpC2E.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpC2E)))),_tmpC2D)))));}
break;}case 190: _LL2E5: {
# 2068 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC31;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC30;yyval=Cyc_YY54((void*)((_tmpC30=_region_malloc(yyr,sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC31.tag=2,_tmpC31)),_tmpC30)))));}
break;}case 191: _LL2E6: {
# 2070 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC34;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC33;yyval=Cyc_YY54((void*)((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=3,_tmpC34)),_tmpC33)))));}
break;}case 192: _LL2E7: {
# 2072 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC37;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC36;yyval=Cyc_YY54((void*)((_tmpC36=_region_malloc(yyr,sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC37.tag=4,_tmpC37)),_tmpC36)))));}
break;}case 193: _LL2E8: {
# 2074 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC3A;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC39;yyval=Cyc_YY54((void*)((_tmpC39=_region_malloc(yyr,sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3A.tag=5,_tmpC3A)),_tmpC39)))));}
break;}case 194: _LL2E9: {
# 2076 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2080
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC3D;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC3C;yyval=Cyc_YY54((void*)((_tmpC3C=_region_malloc(yyr,sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=6,_tmpC3D)),_tmpC3C)))));}
break;}case 195: _LL2EA: {
# 2078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2082
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC40;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC3F;yyval=Cyc_YY54((void*)((_tmpC3F=_region_malloc(yyr,sizeof(*_tmpC3F)),((_tmpC3F[0]=((_tmpC40.tag=7,_tmpC40)),_tmpC3F)))));}
break;}case 196: _LL2EB: {
# 2083 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2088
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC41;yyval=Cyc_YY1(((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41->f1=loc,((_tmpC41->f2=Cyc_Absyn_true_conref,((_tmpC41->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC41)))))))));}
# 2091
break;}case 197: _LL2EC: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2092
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC42;yyval=Cyc_YY1(((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42->f1=loc,((_tmpC42->f2=Cyc_Absyn_false_conref,((_tmpC42->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC42)))))))));}
# 2095
break;}case 198: _LL2ED: {
# 2092 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2096
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC43;yyval=Cyc_YY1(((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->f1=loc,((_tmpC43->f2=Cyc_Absyn_true_conref,((_tmpC43->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC43)))))))));}
# 2099
break;}case 199: _LL2EE:
# 2097 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2EF: {
# 2098 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2102
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC46;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC45;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=1,((_tmpC46.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC46)))),_tmpC45))))));}
break;}case 201: _LL2F0:
# 2101 "parse.y"
 yyval=Cyc_YY52(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2F1: {
# 2102 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2106
yyval=Cyc_YY52(Cyc_Absyn_true_conref);
break;}case 203: _LL2F2: {
# 2103 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2107
yyval=Cyc_YY52(Cyc_Absyn_false_conref);
break;}case 204: _LL2F3:
# 2108 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 205: _LL2F4: {
# 2109 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2113
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 206: _LL2F5: {
# 2110 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2114
yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 207: _LL2F6:
# 2114 "parse.y"
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 208: _LL2F7: {
# 2115 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2119
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 209: _LL2F8: {
# 2120 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2124
{struct _tuple24*_tmpC47;yyval=Cyc_YY40(((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC47->f2=0,((_tmpC47->f3=0,((_tmpC47->f4=Cyc_yyget_YY50(yyyyvsp[1]),((_tmpC47->f5=Cyc_yyget_YY51(yyyyvsp[2]),_tmpC47)))))))))))));}
break;}case 210: _LL2F9: {
# 2122 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2126
{struct _tuple24*_tmpC48;yyval=Cyc_YY40(((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC48->f2=1,((_tmpC48->f3=0,((_tmpC48->f4=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC48->f5=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC48)))))))))))));}
break;}case 211: _LL2FA: {
# 2125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2129
struct _dyneither_ptr*_tmp637;struct Cyc_Absyn_Tqual _tmp638;void*_tmp639;struct _tuple8*_tmp636=Cyc_yyget_YY38(yyyyvsp[2]);_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;_tmp639=_tmp636->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC49;struct Cyc_Absyn_VarargInfo*_tmp63A=(_tmpC49=_cycalloc(sizeof(*_tmpC49)),((_tmpC49->name=_tmp637,((_tmpC49->tq=_tmp638,((_tmpC49->type=_tmp639,((_tmpC49->inject=Cyc_yyget_YY32(yyyyvsp[1]),_tmpC49)))))))));
{struct _tuple24*_tmpC4A;yyval=Cyc_YY40(((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->f1=0,((_tmpC4A->f2=0,((_tmpC4A->f3=_tmp63A,((_tmpC4A->f4=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC4A->f5=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC4A)))))))))))));}
# 2133
break;};}case 212: _LL2FB: {
# 2131 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2135
struct _dyneither_ptr*_tmp63E;struct Cyc_Absyn_Tqual _tmp63F;void*_tmp640;struct _tuple8*_tmp63D=Cyc_yyget_YY38(yyyyvsp[4]);_tmp63E=_tmp63D->f1;_tmp63F=_tmp63D->f2;_tmp640=_tmp63D->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC4B;struct Cyc_Absyn_VarargInfo*_tmp641=(_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->name=_tmp63E,((_tmpC4B->tq=_tmp63F,((_tmpC4B->type=_tmp640,((_tmpC4B->inject=Cyc_yyget_YY32(yyyyvsp[3]),_tmpC4B)))))))));
{struct _tuple24*_tmpC4C;yyval=Cyc_YY40(((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC4C->f2=0,((_tmpC4C->f3=_tmp641,((_tmpC4C->f4=Cyc_yyget_YY50(yyyyvsp[5]),((_tmpC4C->f5=Cyc_yyget_YY51(yyyyvsp[6]),_tmpC4C)))))))))))));}
# 2139
break;};}case 213: _LL2FC: {
# 2139 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2143
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC4F;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC4E;yyval=Cyc_YY45(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC4F.tag=1,((_tmpC4F.f1=0,_tmpC4F)))),_tmpC4E))))));}
break;}case 214: _LL2FD: {
# 2140 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2144
yyval=Cyc_YY45(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY44(yyyyvsp[2]))));
break;}case 215: _LL2FE:
# 2143 "parse.y"
 yyval=Cyc_YY50(0);
break;case 216: _LL2FF: {
# 2144 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2148
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC52;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC51;yyval=Cyc_YY50((void*)((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC52.tag=24,((_tmpC52.f1=Cyc_yyget_YY41(yyyyvsp[1]),_tmpC52)))),_tmpC51)))));}
break;}case 217: _LL300:
# 2148 "parse.y"
 yyval=Cyc_YY51(0);
break;case 218: _LL301: {
# 2149 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2153
yyval=yyyyvsp[1];
break;}case 219: _LL302: {
# 2154 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2161
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC55;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC54;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp648=(_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=2,((_tmpC55.f1=0,((_tmpC55.f2=& Cyc_Tcutil_trk,_tmpC55)))))),_tmpC54)));
struct _dyneither_ptr _tmp649=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp64A=Cyc_Parse_id2type(_tmp649,(void*)_tmp648);
{struct _tuple27*_tmpC5F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC5D;struct Cyc_List_List*_tmpC5C;yyval=Cyc_YY51(((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C->hd=((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F->f1=(void*)((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC5E.tag=24,((_tmpC5E.f1=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC5E)))),_tmpC5D)))),((_tmpC5F->f2=_tmp64A,_tmpC5F)))))),((_tmpC5C->tl=0,_tmpC5C)))))));}
# 2166
break;}case 220: _LL303: {
# 2163 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2168
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC62;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC61;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp651=(_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=2,((_tmpC62.f1=0,((_tmpC62.f2=& Cyc_Tcutil_trk,_tmpC62)))))),_tmpC61)));
struct _dyneither_ptr _tmp652=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp653=Cyc_Parse_id2type(_tmp652,(void*)_tmp651);
{struct _tuple27*_tmpC6C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC6B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC6A;struct Cyc_List_List*_tmpC69;yyval=Cyc_YY51(((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->hd=((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->f1=(void*)((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=24,((_tmpC6B.f1=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC6B)))),_tmpC6A)))),((_tmpC6C->f2=_tmp653,_tmpC6C)))))),((_tmpC69->tl=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC69)))))));}
# 2173
break;}case 221: _LL304:
# 2173 "parse.y"
 yyval=Cyc_YY32(0);
break;case 222: _LL305: {
# 2175 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2179
{const char*_tmpC6D;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC6D="inject",_tag_dyneither(_tmpC6D,sizeof(char),7))))!= 0){
const char*_tmpC6E;Cyc_Parse_err(((_tmpC6E="missing type in function declaration",_tag_dyneither(_tmpC6E,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY32(1);
# 2183
break;}case 223: _LL306: {
# 2182 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2186
yyval=yyyyvsp[0];
break;}case 224: _LL307: {
# 2183 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2187
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY41(yyyyvsp[0]),Cyc_yyget_YY41(yyyyvsp[2])));
break;}case 225: _LL308: {
# 2187 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2191
yyval=Cyc_YY41(0);
break;}case 226: _LL309: {
# 2188 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2192
yyval=yyyyvsp[1];
break;}case 227: _LL30A: {
# 2190 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2194
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC74;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC73;struct Cyc_List_List*_tmpC72;yyval=Cyc_YY41(((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72->hd=(void*)((_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC73.tag=25,((_tmpC73.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpC73)))),_tmpC74)))),((_tmpC72->tl=0,_tmpC72)))))));}
break;}case 228: _LL30B: {
# 2192 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2196
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC75;yyval=Cyc_YY41(((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpC75->tl=0,_tmpC75)))))));}
# 2199
break;}case 229: _LL30C: {
# 2200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2204
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC7B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC7A;struct Cyc_List_List*_tmpC79;yyval=Cyc_YY41(((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->hd=(void*)((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7A.tag=23,((_tmpC7A.f1=Cyc_yyget_YY45(yyyyvsp[0]),_tmpC7A)))),_tmpC7B)))),((_tmpC79->tl=0,_tmpC79)))))));}
# 2207
break;}case 230: _LL30D: {
# 2204 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2208
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC81;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC80;struct Cyc_List_List*_tmpC7F;yyval=Cyc_YY41(((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->hd=(void*)((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC80.tag=23,((_tmpC80.f1=Cyc_yyget_YY45(yyyyvsp[0]),_tmpC80)))),_tmpC81)))),((_tmpC7F->tl=Cyc_yyget_YY41(yyyyvsp[2]),_tmpC7F)))))));}
# 2211
break;}case 231: _LL30E: {
# 2212 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2216
{struct Cyc_List_List*_tmpC82;yyval=Cyc_YY39(((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->hd=Cyc_yyget_YY38(yyyyvsp[0]),((_tmpC82->tl=0,_tmpC82)))))));}
break;}case 232: _LL30F: {
# 2214 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2218
{struct Cyc_List_List*_tmpC83;yyval=Cyc_YY39(((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=Cyc_yyget_YY38(yyyyvsp[2]),((_tmpC83->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpC83)))))));}
break;}case 233: _LL310: {
# 2220 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2224
struct Cyc_Absyn_Tqual _tmp669;struct Cyc_Parse_Type_specifier _tmp66A;struct Cyc_List_List*_tmp66B;struct _tuple23 _tmp668=Cyc_yyget_YY36(yyyyvsp[0]);_tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;_tmp66B=_tmp668.f3;
if(_tmp669.loc == 0)_tmp669.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple0*_tmp66D;struct Cyc_List_List*_tmp66E;struct Cyc_Parse_Declarator _tmp66C=Cyc_yyget_YY30(yyyyvsp[1]);_tmp66D=_tmp66C.id;_tmp66E=_tmp66C.tms;{
void*_tmp66F=Cyc_Parse_speclist2typ(_tmp66A,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_Absyn_Tqual _tmp671;void*_tmp672;struct Cyc_List_List*_tmp673;struct Cyc_List_List*_tmp674;struct _tuple13 _tmp670=Cyc_Parse_apply_tms(_tmp669,_tmp66F,_tmp66B,_tmp66E);_tmp671=_tmp670.f1;_tmp672=_tmp670.f2;_tmp673=_tmp670.f3;_tmp674=_tmp670.f4;
if(_tmp673 != 0){
const char*_tmpC84;Cyc_Parse_err(((_tmpC84="parameter with bad type params",_tag_dyneither(_tmpC84,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp66D)){
const char*_tmpC85;Cyc_Parse_err(((_tmpC85="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC85,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp66D).f2;
if(_tmp674 != 0){
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC88="extra attributes on parameter, ignoring",_tag_dyneither(_tmpC88,sizeof(char),40))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}
{struct _tuple8*_tmpC89;yyval=Cyc_YY38(((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->f1=idopt,((_tmpC89->f2=_tmp671,((_tmpC89->f3=_tmp672,_tmpC89)))))))));}
# 2238
break;};};};}case 234: _LL311: {
# 2235 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2239
struct Cyc_Absyn_Tqual _tmp67B;struct Cyc_Parse_Type_specifier _tmp67C;struct Cyc_List_List*_tmp67D;struct _tuple23 _tmp67A=Cyc_yyget_YY36(yyyyvsp[0]);_tmp67B=_tmp67A.f1;_tmp67C=_tmp67A.f2;_tmp67D=_tmp67A.f3;
if(_tmp67B.loc == 0)_tmp67B.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp67E=Cyc_Parse_speclist2typ(_tmp67C,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp67D != 0){
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8C="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC8C,sizeof(char),38))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}
{struct _tuple8*_tmpC8D;yyval=Cyc_YY38(((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->f1=0,((_tmpC8D->f2=_tmp67B,((_tmpC8D->f3=_tmp67E,_tmpC8D)))))))));}
# 2246
break;};}case 235: _LL312: {
# 2243 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2247
struct Cyc_Absyn_Tqual _tmp683;struct Cyc_Parse_Type_specifier _tmp684;struct Cyc_List_List*_tmp685;struct _tuple23 _tmp682=Cyc_yyget_YY36(yyyyvsp[0]);_tmp683=_tmp682.f1;_tmp684=_tmp682.f2;_tmp685=_tmp682.f3;
if(_tmp683.loc == 0)_tmp683.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp686=Cyc_Parse_speclist2typ(_tmp684,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp687=(Cyc_yyget_YY31(yyyyvsp[1])).tms;
struct Cyc_Absyn_Tqual _tmp689;void*_tmp68A;struct Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68C;struct _tuple13 _tmp688=Cyc_Parse_apply_tms(_tmp683,_tmp686,_tmp685,_tmp687);_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;_tmp68B=_tmp688.f3;_tmp68C=_tmp688.f4;
if(_tmp68B != 0){
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC90="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpC90,sizeof(char),49))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
# 2255
if(_tmp68C != 0){
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC93="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC93,sizeof(char),38))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}
{struct _tuple8*_tmpC94;yyval=Cyc_YY38(((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->f1=0,((_tmpC94->f2=_tmp689,((_tmpC94->f3=_tmp68A,_tmpC94)))))))));}
# 2259
break;};}case 236: _LL313: {
# 2259 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2263
yyval=Cyc_YY37(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY37(yyyyvsp[0])));
break;}case 237: _LL314: {
# 2263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2267
{struct _dyneither_ptr*_tmpC97;struct Cyc_List_List*_tmpC96;yyval=Cyc_YY37(((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96->hd=((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpC97)))),((_tmpC96->tl=0,_tmpC96)))))));}
break;}case 238: _LL315: {
# 2265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2269
{struct _dyneither_ptr*_tmpC9A;struct Cyc_List_List*_tmpC99;yyval=Cyc_YY37(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpC9A)))),((_tmpC99->tl=Cyc_yyget_YY37(yyyyvsp[0]),_tmpC99)))))));}
break;}case 239: _LL316: {
# 2269 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2273
yyval=yyyyvsp[0];
break;}case 240: _LL317: {
# 2270 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2274
yyval=yyyyvsp[0];
break;}case 241: _LL318: {
# 2275 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2279
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9D;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC9C;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=34,((_tmpC9D.f1=0,((_tmpC9D.f2=0,_tmpC9D)))))),_tmpC9C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 242: _LL319: {
# 2277 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2281
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCA0;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC9F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=34,((_tmpCA0.f1=0,((_tmpCA0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCA0)))))),_tmpC9F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL31A: {
# 2279 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2283
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCA3;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCA2;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=34,((_tmpCA3.f1=0,((_tmpCA3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCA3)))))),_tmpCA2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL31B: {
# 2281 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2285
struct _dyneither_ptr*_tmpCA6;struct _tuple0*_tmpCA5;struct Cyc_Absyn_Vardecl*_tmp69C=Cyc_Absyn_new_vardecl(((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5->f1=Cyc_Absyn_Loc_n,((_tmpCA5->f2=((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCA6)))),_tmpCA5)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2288
(_tmp69C->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCA9;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCA8;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=26,((_tmpCA9.f1=_tmp69C,((_tmpCA9.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCA9.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCA9.f4=0,_tmpCA9)))))))))),_tmpCA8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2291
break;}case 245: _LL31C: {
# 2292 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2296
{struct _tuple28*_tmpCAC;struct Cyc_List_List*_tmpCAB;yyval=Cyc_YY7(((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=0,((_tmpCAC->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCAC)))))),((_tmpCAB->tl=0,_tmpCAB)))))));}
break;}case 246: _LL31D: {
# 2294 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2298
{struct _tuple28*_tmpCAF;struct Cyc_List_List*_tmpCAE;yyval=Cyc_YY7(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->hd=((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->f1=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpCAF->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCAF)))))),((_tmpCAE->tl=0,_tmpCAE)))))));}
break;}case 247: _LL31E: {
# 2296 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2300
{struct _tuple28*_tmpCB2;struct Cyc_List_List*_tmpCB1;yyval=Cyc_YY7(((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->f1=0,((_tmpCB2->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCB2)))))),((_tmpCB1->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCB1)))))));}
break;}case 248: _LL31F: {
# 2298 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2302
{struct _tuple28*_tmpCB5;struct Cyc_List_List*_tmpCB4;yyval=Cyc_YY7(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->f1=Cyc_yyget_YY42(yyyyvsp[2]),((_tmpCB5->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCB5)))))),((_tmpCB4->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCB4)))))));}
break;}case 249: _LL320: {
# 2302 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2306
yyval=Cyc_YY42(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY42(yyyyvsp[0])));
break;}case 250: _LL321: {
# 2307 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2311
{struct Cyc_List_List*_tmpCB6;yyval=Cyc_YY42(((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->hd=Cyc_yyget_YY43(yyyyvsp[0]),((_tmpCB6->tl=0,_tmpCB6)))))));}
break;}case 251: _LL322: {
# 2308 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2312
{struct Cyc_List_List*_tmpCB7;yyval=Cyc_YY42(((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->hd=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpCB7->tl=Cyc_yyget_YY42(yyyyvsp[0]),_tmpCB7)))))));}
break;}case 252: _LL323: {
# 2312 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2316
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCBA;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCB9;yyval=Cyc_YY43((void*)((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=0,((_tmpCBA.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCBA)))),_tmpCB9)))));}
break;}case 253: _LL324: {
# 2313 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2317
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCC0;struct _dyneither_ptr*_tmpCBF;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCBE;yyval=Cyc_YY43((void*)((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCC0.tag=1,((_tmpCC0.f1=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCBF)))),_tmpCC0)))),_tmpCBE)))));}
break;}case 254: _LL325: {
# 2318 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2322
struct Cyc_Absyn_Tqual _tmp6B1;struct Cyc_Parse_Type_specifier _tmp6B2;struct Cyc_List_List*_tmp6B3;struct _tuple23 _tmp6B0=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;_tmp6B3=_tmp6B0.f3;{
void*_tmp6B4=Cyc_Parse_speclist2typ(_tmp6B2,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6B3 != 0){
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCC3="ignoring attributes in type",_tag_dyneither(_tmpCC3,sizeof(char),28))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}
{struct _tuple8*_tmpCC4;yyval=Cyc_YY38(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=0,((_tmpCC4->f2=_tmp6B1,((_tmpCC4->f3=_tmp6B4,_tmpCC4)))))))));}
# 2328
break;};}case 255: _LL326: {
# 2325 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2329
struct Cyc_Absyn_Tqual _tmp6B9;struct Cyc_Parse_Type_specifier _tmp6BA;struct Cyc_List_List*_tmp6BB;struct _tuple23 _tmp6B8=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;_tmp6BB=_tmp6B8.f3;{
void*_tmp6BC=Cyc_Parse_speclist2typ(_tmp6BA,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6BD=(Cyc_yyget_YY31(yyyyvsp[1])).tms;
struct _tuple13 _tmp6BE=Cyc_Parse_apply_tms(_tmp6B9,_tmp6BC,_tmp6BB,_tmp6BD);
if(_tmp6BE.f3 != 0){
# 2335
const char*_tmpCC7;void*_tmpCC6;(_tmpCC6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCC7="bad type params, ignoring",_tag_dyneither(_tmpCC7,sizeof(char),26))),_tag_dyneither(_tmpCC6,sizeof(void*),0)));}
if(_tmp6BE.f4 != 0){
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCCA="bad specifiers, ignoring",_tag_dyneither(_tmpCCA,sizeof(char),25))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}
{struct _tuple8*_tmpCCB;yyval=Cyc_YY38(((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->f1=0,((_tmpCCB->f2=_tmp6BE.f1,((_tmpCCB->f3=_tmp6BE.f2,_tmpCCB)))))))));}
# 2340
break;};}case 256: _LL327: {
# 2339 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2343
yyval=Cyc_YY45(Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 257: _LL328: {
# 2340 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2344
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCCE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCCD;yyval=Cyc_YY45((void*)((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=24,((_tmpCCE.f1=0,_tmpCCE)))),_tmpCCD)))));}
break;}case 258: _LL329: {
# 2341 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2345
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD0;yyval=Cyc_YY45((void*)((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=24,((_tmpCD1.f1=Cyc_yyget_YY41(yyyyvsp[1]),_tmpCD1)))),_tmpCD0)))));}
break;}case 259: _LL32A: {
# 2342 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2346
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCD4;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCD3;yyval=Cyc_YY45((void*)((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=25,((_tmpCD4.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpCD4)))),_tmpCD3)))));}
break;}case 260: _LL32B: {
# 2343 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2347
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDA;struct Cyc_List_List*_tmpCD9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD8;yyval=Cyc_YY45((void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCDA.tag=24,((_tmpCDA.f1=((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCD9->tl=Cyc_yyget_YY41(yyyyvsp[2]),_tmpCD9)))))),_tmpCDA)))),_tmpCD8)))));}
break;}case 261: _LL32C: {
# 2349 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2353
{struct Cyc_List_List*_tmpCDB;yyval=Cyc_YY41(((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCDB->tl=0,_tmpCDB)))))));}
break;}case 262: _LL32D: {
# 2350 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2354
{struct Cyc_List_List*_tmpCDC;yyval=Cyc_YY41(((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->hd=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpCDC->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCDC)))))));}
break;}case 263: _LL32E: {
# 2355 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2359
{struct Cyc_Parse_Abstractdeclarator _tmpCDD;yyval=Cyc_YY31(((_tmpCDD.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCDD)));}
break;}case 264: _LL32F: {
# 2357 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2361
yyval=yyyyvsp[0];
break;}case 265: _LL330: {
# 2359 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2363
{struct Cyc_Parse_Abstractdeclarator _tmpCDE;yyval=Cyc_YY31(((_tmpCDE.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY31(yyyyvsp[1])).tms),_tmpCDE)));}
break;}case 266: _LL331: {
# 2364 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2368
yyval=yyyyvsp[1];
break;}case 267: _LL332: {
# 2366 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2370
{struct Cyc_List_List*_tmpCE8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCE7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCE6;struct Cyc_Parse_Abstractdeclarator _tmpCE5;yyval=Cyc_YY31(((_tmpCE5.tms=((_tmpCE8=_region_malloc(yyr,sizeof(*_tmpCE8)),((_tmpCE8->hd=(void*)((_tmpCE6=_region_malloc(yyr,sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE7.tag=0,((_tmpCE7.f1=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpCE7.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpCE7)))))),_tmpCE6)))),((_tmpCE8->tl=0,_tmpCE8)))))),_tmpCE5)));}
break;}case 268: _LL333: {
# 2368 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2372
{struct Cyc_List_List*_tmpCF2;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCF0;struct Cyc_Parse_Abstractdeclarator _tmpCEF;yyval=Cyc_YY31(((_tmpCEF.tms=((_tmpCF2=_region_malloc(yyr,sizeof(*_tmpCF2)),((_tmpCF2->hd=(void*)((_tmpCF0=_region_malloc(yyr,sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF1.tag=0,((_tmpCF1.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpCF1.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpCF1)))))),_tmpCF0)))),((_tmpCF2->tl=(Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpCF2)))))),_tmpCEF)));}
break;}case 269: _LL334: {
# 2370 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2374
{struct Cyc_List_List*_tmpCFC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpCFB;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpCFA;struct Cyc_Parse_Abstractdeclarator _tmpCF9;yyval=Cyc_YY31(((_tmpCF9.tms=((_tmpCFC=_region_malloc(yyr,sizeof(*_tmpCFC)),((_tmpCFC->hd=(void*)((_tmpCFA=_region_malloc(yyr,sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFB.tag=1,((_tmpCFB.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpCFB.f2=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpCFB.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpCFB)))))))),_tmpCFA)))),((_tmpCFC->tl=0,_tmpCFC)))))),_tmpCF9)));}
break;}case 270: _LL335: {
# 2372 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2376
{struct Cyc_List_List*_tmpD06;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD05;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD04;struct Cyc_Parse_Abstractdeclarator _tmpD03;yyval=Cyc_YY31(((_tmpD03.tms=((_tmpD06=_region_malloc(yyr,sizeof(*_tmpD06)),((_tmpD06->hd=(void*)((_tmpD04=_region_malloc(yyr,sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=1,((_tmpD05.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD05.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpD05.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD05)))))))),_tmpD04)))),((_tmpD06->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD06)))))),_tmpD03)));}
# 2379
break;}case 271: _LL336: {
# 2376 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2380
{struct Cyc_List_List*_tmpD1B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD1A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD19;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD18;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD17;struct Cyc_Parse_Abstractdeclarator _tmpD16;yyval=Cyc_YY31(((_tmpD16.tms=((_tmpD1B=_region_malloc(yyr,sizeof(*_tmpD1B)),((_tmpD1B->hd=(void*)((_tmpD17=_region_malloc(yyr,sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD1A.tag=3,((_tmpD1A.f1=(void*)((_tmpD18=_region_malloc(yyr,sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=1,((_tmpD19.f1=0,((_tmpD19.f2=0,((_tmpD19.f3=0,((_tmpD19.f4=Cyc_yyget_YY50(yyyyvsp[1]),((_tmpD19.f5=Cyc_yyget_YY51(yyyyvsp[2]),_tmpD19)))))))))))),_tmpD18)))),_tmpD1A)))),_tmpD17)))),((_tmpD1B->tl=0,_tmpD1B)))))),_tmpD16)));}
# 2382
break;}case 272: _LL337: {
# 2379 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2383
struct Cyc_List_List*_tmp6E8;int _tmp6E9;struct Cyc_Absyn_VarargInfo*_tmp6EA;void*_tmp6EB;struct Cyc_List_List*_tmp6EC;struct _tuple24*_tmp6E7=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6E8=_tmp6E7->f1;_tmp6E9=_tmp6E7->f2;_tmp6EA=_tmp6E7->f3;_tmp6EB=_tmp6E7->f4;_tmp6EC=_tmp6E7->f5;
{struct Cyc_List_List*_tmpD30;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD2F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD2E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD2D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD2C;struct Cyc_Parse_Abstractdeclarator _tmpD2B;yyval=Cyc_YY31(((_tmpD2B.tms=((_tmpD30=_region_malloc(yyr,sizeof(*_tmpD30)),((_tmpD30->hd=(void*)((_tmpD2C=_region_malloc(yyr,sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2F.tag=3,((_tmpD2F.f1=(void*)((_tmpD2D=_region_malloc(yyr,sizeof(*_tmpD2D)),((_tmpD2D[0]=((_tmpD2E.tag=1,((_tmpD2E.f1=_tmp6E8,((_tmpD2E.f2=_tmp6E9,((_tmpD2E.f3=_tmp6EA,((_tmpD2E.f4=_tmp6EB,((_tmpD2E.f5=_tmp6EC,_tmpD2E)))))))))))),_tmpD2D)))),_tmpD2F)))),_tmpD2C)))),((_tmpD30->tl=0,_tmpD30)))))),_tmpD2B)));}
# 2386
break;}case 273: _LL338: {
# 2383 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2387
{struct Cyc_List_List*_tmpD45;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD44;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD43;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD42;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD41;struct Cyc_Parse_Abstractdeclarator _tmpD40;yyval=Cyc_YY31(((_tmpD40.tms=((_tmpD45=_region_malloc(yyr,sizeof(*_tmpD45)),((_tmpD45->hd=(void*)((_tmpD41=_region_malloc(yyr,sizeof(*_tmpD41)),((_tmpD41[0]=((_tmpD44.tag=3,((_tmpD44.f1=(void*)((_tmpD42=_region_malloc(yyr,sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD43.tag=1,((_tmpD43.f1=0,((_tmpD43.f2=0,((_tmpD43.f3=0,((_tmpD43.f4=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpD43.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpD43)))))))))))),_tmpD42)))),_tmpD44)))),_tmpD41)))),((_tmpD45->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD45)))))),_tmpD40)));}
# 2390
break;}case 274: _LL339: {
# 2387 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2391
struct Cyc_List_List*_tmp6FA;int _tmp6FB;struct Cyc_Absyn_VarargInfo*_tmp6FC;void*_tmp6FD;struct Cyc_List_List*_tmp6FE;struct _tuple24*_tmp6F9=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6FA=_tmp6F9->f1;_tmp6FB=_tmp6F9->f2;_tmp6FC=_tmp6F9->f3;_tmp6FD=_tmp6F9->f4;_tmp6FE=_tmp6F9->f5;
{struct Cyc_List_List*_tmpD5A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD59;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD58;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD57;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD56;struct Cyc_Parse_Abstractdeclarator _tmpD55;yyval=Cyc_YY31(((_tmpD55.tms=((_tmpD5A=_region_malloc(yyr,sizeof(*_tmpD5A)),((_tmpD5A->hd=(void*)((_tmpD56=_region_malloc(yyr,sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD59.tag=3,((_tmpD59.f1=(void*)((_tmpD57=_region_malloc(yyr,sizeof(*_tmpD57)),((_tmpD57[0]=((_tmpD58.tag=1,((_tmpD58.f1=_tmp6FA,((_tmpD58.f2=_tmp6FB,((_tmpD58.f3=_tmp6FC,((_tmpD58.f4=_tmp6FD,((_tmpD58.f5=_tmp6FE,_tmpD58)))))))))))),_tmpD57)))),_tmpD59)))),_tmpD56)))),((_tmpD5A->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD5A)))))),_tmpD55)));}
# 2395
break;}case 275: _LL33A: {
# 2393 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2397
struct Cyc_List_List*_tmp705=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD64;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD63;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD62;struct Cyc_Parse_Abstractdeclarator _tmpD61;yyval=Cyc_YY31(((_tmpD61.tms=((_tmpD64=_region_malloc(yyr,sizeof(*_tmpD64)),((_tmpD64->hd=(void*)((_tmpD62=_region_malloc(yyr,sizeof(*_tmpD62)),((_tmpD62[0]=((_tmpD63.tag=4,((_tmpD63.f1=_tmp705,((_tmpD63.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD63.f3=0,_tmpD63)))))))),_tmpD62)))),((_tmpD64->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD64)))))),_tmpD61)));}
# 2401
break;}case 276: _LL33B: {
# 2398 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2402
{struct Cyc_List_List*_tmpD6E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD6D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD6C;struct Cyc_Parse_Abstractdeclarator _tmpD6B;yyval=Cyc_YY31(((_tmpD6B.tms=((_tmpD6E=_region_malloc(yyr,sizeof(*_tmpD6E)),((_tmpD6E->hd=(void*)((_tmpD6C=_region_malloc(yyr,sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=5,((_tmpD6D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD6D.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpD6D)))))),_tmpD6C)))),((_tmpD6E->tl=(Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD6E)))))),_tmpD6B)));}
# 2404
break;}case 277: _LL33C: {
# 2404 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2408
yyval=yyyyvsp[0];
break;}case 278: _LL33D: {
# 2405 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2409
yyval=yyyyvsp[0];
break;}case 279: _LL33E: {
# 2406 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2410
yyval=yyyyvsp[0];
break;}case 280: _LL33F: {
# 2407 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2411
yyval=yyyyvsp[0];
break;}case 281: _LL340: {
# 2408 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2412
yyval=yyyyvsp[0];
break;}case 282: _LL341: {
# 2409 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413
yyval=yyyyvsp[0];
break;}case 283: _LL342: {
# 2412 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2416
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD71;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD70;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((_tmpD71.tag=16,((_tmpD71.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD71)))),_tmpD70)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 284: _LL343:
# 2416 "parse.y"
 yyval=Cyc_YY5(0);
break;case 285: _LL344: {
# 2418 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2422
{const char*_tmpD72;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD72="open",_tag_dyneither(_tmpD72,sizeof(char),5))))!= 0){
const char*_tmpD73;Cyc_Parse_err(((_tmpD73="expecting `open'",_tag_dyneither(_tmpD73,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2426
break;}case 286: _LL345: {
# 2427 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2431
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD79;struct _dyneither_ptr*_tmpD78;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD77;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77[0]=((_tmpD79.tag=13,((_tmpD79.f1=((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD78)))),((_tmpD79.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD79)))))),_tmpD77)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL346: {
# 2431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2435
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 288: _LL347: {
# 2432 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2436
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 289: _LL348: {
# 2437 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2441
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL349: {
# 2438 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2442
yyval=yyyyvsp[1];
break;}case 291: _LL34A: {
# 2443 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2447
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 292: _LL34B: {
# 2444 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2448
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 293: _LL34C: {
# 2445 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2449
yyval=yyyyvsp[0];
break;}case 294: _LL34D: {
# 2446 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2450
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL34E: {
# 2447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2451
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD7C;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD7B;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B[0]=((_tmpD7C.tag=1,((_tmpD7C.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpD7C)))),_tmpD7B)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 296: _LL34F: {
# 2450 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2454
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD7F;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD7E;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7F.tag=1,((_tmpD7F.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpD7F)))),_tmpD7E)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 297: _LL350: {
# 2455 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2459
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 298: _LL351: {
# 2457 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2461
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL352: {
# 2463 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2467
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL353: {
# 2466 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2470
struct Cyc_Absyn_Exp*_tmp719=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp719,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL354: {
# 2470 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2474
struct Cyc_Absyn_Exp*_tmp71A=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp71A,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2477
break;}case 302: _LL355: {
# 2476 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2480
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL356:
# 2490 "parse.y"
 yyval=Cyc_YY11(0);
break;case 304: _LL357: {
# 2492 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2496
{struct Cyc_Absyn_Switch_clause*_tmpD82;struct Cyc_List_List*_tmpD81;yyval=Cyc_YY11(((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpD82->pat_vars=0,((_tmpD82->where_clause=0,((_tmpD82->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpD82->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD82)))))))))))),((_tmpD81->tl=0,_tmpD81)))))));}
# 2499
break;}case 305: _LL358: {
# 2496 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2500
{struct Cyc_Absyn_Switch_clause*_tmpD85;struct Cyc_List_List*_tmpD84;yyval=Cyc_YY11(((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->hd=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD85->pat_vars=0,((_tmpD85->where_clause=0,((_tmpD85->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpD85->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD85)))))))))))),((_tmpD84->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpD84)))))));}
break;}case 306: _LL359: {
# 2500 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2504
{struct Cyc_Absyn_Switch_clause*_tmpD88;struct Cyc_List_List*_tmpD87;yyval=Cyc_YY11(((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD88->pat_vars=0,((_tmpD88->where_clause=0,((_tmpD88->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpD88->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD88)))))))))))),((_tmpD87->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpD87)))))));}
break;}case 307: _LL35A: {
# 2502 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2506
{struct Cyc_Absyn_Switch_clause*_tmpD8B;struct Cyc_List_List*_tmpD8A;yyval=Cyc_YY11(((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD8B->pat_vars=0,((_tmpD8B->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpD8B->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpD8B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD8B)))))))))))),((_tmpD8A->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpD8A)))))));}
break;}case 308: _LL35B: {
# 2506 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2510
{struct Cyc_Absyn_Switch_clause*_tmpD8E;struct Cyc_List_List*_tmpD8D;yyval=Cyc_YY11(((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD8E->pat_vars=0,((_tmpD8E->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpD8E->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpD8E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD8E)))))))))))),((_tmpD8D->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpD8D)))))));}
break;}case 309: _LL35C: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2517
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL35D: {
# 2517 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2521
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL35E: {
# 2521 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2525
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL35F: {
# 2524 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2528
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL360: {
# 2527 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2531
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL361: {
# 2530 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2534
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL362: {
# 2533 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2537
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL363: {
# 2536 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2540
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL364: {
# 2539 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2543
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL365: {
# 2542 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2546
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL366: {
# 2545 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2549
struct Cyc_List_List*_tmp725=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp726=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp725,_tmp726));
# 2554
break;}case 320: _LL367: {
# 2551 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2555
struct Cyc_List_List*_tmp727=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp728=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp727,_tmp728));
# 2560
break;}case 321: _LL368: {
# 2557 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2561
struct Cyc_List_List*_tmp729=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp72A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp729,_tmp72A));
# 2566
break;}case 322: _LL369: {
# 2563 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2567
struct Cyc_List_List*_tmp72B=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp72C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp72B,_tmp72C));
# 2572
break;}case 323: _LL36A: {
# 2573 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2575
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2577
{struct _dyneither_ptr*_tmpD8F;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD8F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 324: _LL36B: {
# 2574 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2578
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL36C: {
# 2575 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2579
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL36D: {
# 2576 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2580
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL36E: {
# 2577 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2581
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL36F: {
# 2579 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2581
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2583
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL370: {
# 2580 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2584
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL371: {
# 2582 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2586
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL372: {
# 2591 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2595
yyval=yyyyvsp[0];
break;}case 332: _LL373: {
# 2594 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2598
yyval=yyyyvsp[0];
break;}case 333: _LL374: {
# 2596 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2600
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 334: _LL375: {
# 2599 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2603
yyval=yyyyvsp[0];
break;}case 335: _LL376: {
# 2601 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2605
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 336: _LL377: {
# 2604 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2608
yyval=yyyyvsp[0];
break;}case 337: _LL378: {
# 2606 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2610
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 338: _LL379: {
# 2609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2613
yyval=yyyyvsp[0];
break;}case 339: _LL37A: {
# 2611 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2615
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL37B: {
# 2614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2618
yyval=yyyyvsp[0];
break;}case 341: _LL37C: {
# 2616 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2620
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL37D: {
# 2619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2623
yyval=yyyyvsp[0];
break;}case 343: _LL37E: {
# 2621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2625
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL37F: {
# 2624 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2628
yyval=yyyyvsp[0];
break;}case 345: _LL380: {
# 2626 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2630
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL381: {
# 2628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2632
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL382: {
# 2631 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2635
yyval=yyyyvsp[0];
break;}case 348: _LL383: {
# 2633 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2637
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL384: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2639
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL385: {
# 2637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2641
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL386: {
# 2639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2643
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL387: {
# 2642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2646
yyval=yyyyvsp[0];
break;}case 353: _LL388: {
# 2644 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL389: {
# 2646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL38A: {
# 2649 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2653
yyval=yyyyvsp[0];
break;}case 356: _LL38B: {
# 2651 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2655
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38C: {
# 2653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2657
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL38D: {
# 2656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2660
yyval=yyyyvsp[0];
break;}case 359: _LL38E: {
# 2658 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL38F: {
# 2660 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2664
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL390: {
# 2662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2666
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL391: {
# 2665 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2669
yyval=yyyyvsp[0];
break;}case 363: _LL392: {
# 2667 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2671
void*_tmp72E=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp72E,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2674
break;}case 364: _LL393: {
# 2673 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2677
yyval=yyyyvsp[0];
break;}case 365: _LL394: {
# 2676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2680
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 366: _LL395: {
# 2678 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2682
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL396: {
# 2680 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2684
void*_tmp72F=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp72F,(unsigned int)(yyyylsp[0]).first_line)));
# 2687
break;}case 368: _LL397: {
# 2684 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2688
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL398: {
# 2686 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2690
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY38(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 370: _LL399: {
# 2691 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2695
yyval=yyyyvsp[0];
break;}case 371: _LL39A: {
# 2700 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2704
yyval=yyyyvsp[0];
break;}case 372: _LL39B: {
# 2705 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2709
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 373: _LL39C: {
# 2707 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2711
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 374: _LL39D: {
# 2709 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2713
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp730=e->r;enum Cyc_Absyn_Sign _tmp732;char _tmp733;enum Cyc_Absyn_Sign _tmp735;short _tmp736;enum Cyc_Absyn_Sign _tmp738;int _tmp739;struct _dyneither_ptr _tmp73B;int _tmp73C;_LL3A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp731=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp731->tag != 0)goto _LL3A2;else{if(((_tmp731->f1).Char_c).tag != 2)goto _LL3A2;_tmp732=((struct _tuple3)((_tmp731->f1).Char_c).val).f1;_tmp733=((struct _tuple3)((_tmp731->f1).Char_c).val).f2;}}_LL3A1:
# 2717
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpD92;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpD91;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD91=_cycalloc_atomic(sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD92.tag=10,((_tmpD92.f1=_tmp733,_tmpD92)))),_tmpD91)))),e->loc));}goto _LL39F;_LL3A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp734=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp734->tag != 0)goto _LL3A4;else{if(((_tmp734->f1).Short_c).tag != 4)goto _LL3A4;_tmp735=((struct _tuple4)((_tmp734->f1).Short_c).val).f1;_tmp736=((struct _tuple4)((_tmp734->f1).Short_c).val).f2;}}_LL3A3:
# 2719
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD95;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD94;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD94=_cycalloc_atomic(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD95.tag=9,((_tmpD95.f1=_tmp735,((_tmpD95.f2=(int)_tmp736,_tmpD95)))))),_tmpD94)))),e->loc));}goto _LL39F;_LL3A4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp737=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp737->tag != 0)goto _LL3A6;else{if(((_tmp737->f1).Int_c).tag != 5)goto _LL3A6;_tmp738=((struct _tuple5)((_tmp737->f1).Int_c).val).f1;_tmp739=((struct _tuple5)((_tmp737->f1).Int_c).val).f2;}}_LL3A5:
# 2721
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD98;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD97;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD97=_cycalloc_atomic(sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD98.tag=9,((_tmpD98.f1=_tmp738,((_tmpD98.f2=_tmp739,_tmpD98)))))),_tmpD97)))),e->loc));}goto _LL39F;_LL3A6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp73A->tag != 0)goto _LL3A8;else{if(((_tmp73A->f1).Float_c).tag != 7)goto _LL3A8;_tmp73B=((struct _tuple7)((_tmp73A->f1).Float_c).val).f1;_tmp73C=((struct _tuple7)((_tmp73A->f1).Float_c).val).f2;}}_LL3A7:
# 2723
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpD9B;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpD9A;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9B.tag=11,((_tmpD9B.f1=_tmp73B,((_tmpD9B.f2=_tmp73C,_tmpD9B)))))),_tmpD9A)))),e->loc));}goto _LL39F;_LL3A8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp73D->tag != 0)goto _LL3AA;else{if(((_tmp73D->f1).Null_c).tag != 1)goto _LL3AA;}}_LL3A9:
# 2725
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL39F;_LL3AA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp73E->tag != 0)goto _LL3AC;else{if(((_tmp73E->f1).String_c).tag != 8)goto _LL3AC;}}_LL3AB:
# 2727
{const char*_tmpD9C;Cyc_Parse_err(((_tmpD9C="strings cannot occur within patterns",_tag_dyneither(_tmpD9C,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39F;_LL3AC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp73F->tag != 0)goto _LL3AE;else{if(((_tmp73F->f1).Wstring_c).tag != 9)goto _LL3AE;}}_LL3AD:
# 2729
{const char*_tmpD9D;Cyc_Parse_err(((_tmpD9D="strings cannot occur within patterns",_tag_dyneither(_tmpD9D,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39F;_LL3AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp740=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp730;if(_tmp740->tag != 0)goto _LL3B0;else{if(((_tmp740->f1).LongLong_c).tag != 6)goto _LL3B0;}}_LL3AF:
# 2731
{const char*_tmpD9E;Cyc_Parse_unimp(((_tmpD9E="long long's in patterns",_tag_dyneither(_tmpD9E,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39F;_LL3B0:;_LL3B1: {
# 2733
const char*_tmpD9F;Cyc_Parse_err(((_tmpD9F="bad constant in case",_tag_dyneither(_tmpD9F,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL39F:;}
# 2736
break;}case 375: _LL39E: {
# 2733 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2737
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDA2;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDA1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA2.tag=14,((_tmpDA2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDA2)))),_tmpDA1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 376: _LL3B2: {
# 2735 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2739
{const char*_tmpDA3;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDA3="as",_tag_dyneither(_tmpDA3,sizeof(char),3))))!= 0){
const char*_tmpDA4;Cyc_Parse_err(((_tmpDA4="expecting `as'",_tag_dyneither(_tmpDA4,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDAE;struct _dyneither_ptr*_tmpDAD;struct _tuple0*_tmpDAC;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDAB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB[0]=((_tmpDAE.tag=1,((_tmpDAE.f1=Cyc_Absyn_new_vardecl(((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->f1=Cyc_Absyn_Loc_n,((_tmpDAC->f2=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDAD)))),_tmpDAC)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDAE.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDAE)))))),_tmpDAB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2744
break;}case 377: _LL3B3: {
# 2741 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2745
struct Cyc_List_List*_tmp756;int _tmp757;struct _tuple21 _tmp755=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp756=_tmp755.f1;_tmp757=_tmp755.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDB1;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDB0;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDB1.tag=4,((_tmpDB1.f1=_tmp756,((_tmpDB1.f2=_tmp757,_tmpDB1)))))),_tmpDB0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2748
break;}case 378: _LL3B4: {
# 2745 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2749
struct Cyc_List_List*_tmp75B;int _tmp75C;struct _tuple21 _tmp75A=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp75B=_tmp75A.f1;_tmp75C=_tmp75A.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDB4;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDB3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB4.tag=15,((_tmpDB4.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDB4.f2=_tmp75B,((_tmpDB4.f3=_tmp75C,_tmpDB4)))))))),_tmpDB3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2752
break;}case 379: _LL3B5: {
# 2749 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2753
struct Cyc_List_List*_tmp760;int _tmp761;struct _tuple21 _tmp75F=*Cyc_yyget_YY17(yyyyvsp[3]);_tmp760=_tmp75F.f1;_tmp761=_tmp75F.f2;{
struct Cyc_List_List*_tmp762=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY41(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDBA;struct Cyc_Absyn_AggrInfo*_tmpDB9;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDB8;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDBA.tag=6,((_tmpDBA.f1=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDB9->targs=0,_tmpDB9)))))),((_tmpDBA.f2=_tmp762,((_tmpDBA.f3=_tmp760,((_tmpDBA.f4=_tmp761,_tmpDBA)))))))))),_tmpDB8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2758
break;};}case 380: _LL3B6: {
# 2755 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2759
struct Cyc_List_List*_tmp767;int _tmp768;struct _tuple21 _tmp766=*Cyc_yyget_YY17(yyyyvsp[2]);_tmp767=_tmp766.f1;_tmp768=_tmp766.f2;{
struct Cyc_List_List*_tmp769=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY41(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDBD;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDBC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=6,((_tmpDBD.f1=0,((_tmpDBD.f2=_tmp769,((_tmpDBD.f3=_tmp767,((_tmpDBD.f4=_tmp768,_tmpDBD)))))))))),_tmpDBC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2763
break;};}case 381: _LL3B7: {
# 2760 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2764
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC0;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDBF;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=5,((_tmpDC0.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDC0)))),_tmpDBF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 382: _LL3B8: {
# 2762 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2766
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDCA;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC9;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC8;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDCA.tag=5,((_tmpDCA.f1=Cyc_Absyn_new_pat((void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=5,((_tmpDC9.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDC9)))),_tmpDC8)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDCA)))),_tmpDC7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3B9: {
# 2764 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2768
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDD4;struct _dyneither_ptr*_tmpDD3;struct _tuple0*_tmpDD2;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDD1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD4.tag=2,((_tmpDD4.f1=Cyc_Absyn_new_vardecl(((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->f1=Cyc_Absyn_Loc_n,((_tmpDD2->f2=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDD3)))),_tmpDD2)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD4.f2=
# 2770
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDD4)))))),_tmpDD1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2772
break;}case 384: _LL3BA: {
# 2769 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2773
{const char*_tmpDD5;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDD5="as",_tag_dyneither(_tmpDD5,sizeof(char),3))))!= 0){
const char*_tmpDD6;Cyc_Parse_err(((_tmpDD6="expecting `as'",_tag_dyneither(_tmpDD6,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDE0;struct _dyneither_ptr*_tmpDDF;struct _tuple0*_tmpDDE;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDDD;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDE0.tag=2,((_tmpDE0.f1=Cyc_Absyn_new_vardecl(((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->f1=Cyc_Absyn_Loc_n,((_tmpDDE->f2=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDDF)))),_tmpDDE)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDE0.f2=
# 2777
Cyc_yyget_YY12(yyyyvsp[3]),_tmpDE0)))))),_tmpDDD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2779
break;}case 385: _LL3BB: {
# 2776 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2780
void*_tmp77C=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpDF1;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDF0;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDEF;struct _dyneither_ptr*_tmpDEE;struct _tuple0*_tmpDED;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpDEC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[0]=((_tmpDF1.tag=3,((_tmpDF1.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp77C),((_tmpDF1.f2=
Cyc_Absyn_new_vardecl(((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->f1=Cyc_Absyn_Loc_n,((_tmpDED->f2=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDEE)))),_tmpDED)))))),(void*)(
(_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF0.tag=19,((_tmpDF0.f1=_tmp77C,_tmpDF0)))),_tmpDEF)))),0),_tmpDF1)))))),_tmpDEC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2785
break;}case 386: _LL3BC: {
# 2782 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2786
struct Cyc_Absyn_Tvar*_tmp783=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE0D;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE0B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE0A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE09;struct _dyneither_ptr*_tmpE08;struct _tuple0*_tmpE07;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE06;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE0D.tag=3,((_tmpE0D.f1=_tmp783,((_tmpE0D.f2=
Cyc_Absyn_new_vardecl(((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->f1=Cyc_Absyn_Loc_n,((_tmpE07->f2=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE08)))),_tmpE07)))))),(void*)(
(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE0C.tag=19,((_tmpE0C.f1=(void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=2,((_tmpE0B.f1=_tmp783,_tmpE0B)))),_tmpE0A)))),_tmpE0C)))),_tmpE09)))),0),_tmpE0D)))))),_tmpE06)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2791
break;}case 387: _LL3BD: {
# 2790 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2792
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2794
{struct _tuple21*_tmpE0E;yyval=Cyc_YY13(((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE0E->f2=0,_tmpE0E)))))));}
break;}case 388: _LL3BE: {
# 2791 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2795
{struct _tuple21*_tmpE0F;yyval=Cyc_YY13(((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE0F->f2=1,_tmpE0F)))))));}
break;}case 389: _LL3BF: {
# 2792 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2796
{struct _tuple21*_tmpE10;yyval=Cyc_YY13(((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->f1=0,((_tmpE10->f2=1,_tmpE10)))))));}
break;}case 390: _LL3C0: {
# 2797 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2801
{struct Cyc_List_List*_tmpE11;yyval=Cyc_YY14(((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE11->tl=0,_tmpE11)))))));}
break;}case 391: _LL3C1: {
# 2799 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2803
{struct Cyc_List_List*_tmpE12;yyval=Cyc_YY14(((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE12->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE12)))))));}
break;}case 392: _LL3C2: {
# 2804 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2808
{struct _tuple22*_tmpE13;yyval=Cyc_YY15(((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->f1=0,((_tmpE13->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE13)))))));}
break;}case 393: _LL3C3: {
# 2806 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2810
{struct _tuple22*_tmpE14;yyval=Cyc_YY15(((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->f1=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpE14->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE14)))))));}
break;}case 394: _LL3C4: {
# 2809 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2813
{struct _tuple21*_tmpE15;yyval=Cyc_YY17(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE15->f2=0,_tmpE15)))))));}
break;}case 395: _LL3C5: {
# 2810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2814
{struct _tuple21*_tmpE16;yyval=Cyc_YY17(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE16->f2=1,_tmpE16)))))));}
break;}case 396: _LL3C6: {
# 2811 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2815
{struct _tuple21*_tmpE17;yyval=Cyc_YY17(((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->f1=0,((_tmpE17->f2=1,_tmpE17)))))));}
break;}case 397: _LL3C7: {
# 2816 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820
{struct Cyc_List_List*_tmpE18;yyval=Cyc_YY16(((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE18->tl=0,_tmpE18)))))));}
break;}case 398: _LL3C8: {
# 2818 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2822
{struct Cyc_List_List*_tmpE19;yyval=Cyc_YY16(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE19->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE19)))))));}
break;}case 399: _LL3C9: {
# 2824 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2828
yyval=yyyyvsp[0];
break;}case 400: _LL3CA: {
# 2826 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2830
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 401: _LL3CB: {
# 2831 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2835
yyval=yyyyvsp[0];
break;}case 402: _LL3CC: {
# 2833 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2837
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 403: _LL3CD: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2839
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 404: _LL3CE: {
# 2839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2843
yyval=Cyc_YY9(0);
break;}case 405: _LL3CF: {
# 2840 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2844
{struct Cyc_Core_Opt*_tmpE1A;yyval=Cyc_YY9(((_tmpE1A=_cycalloc_atomic(sizeof(*_tmpE1A)),((_tmpE1A->v=(void*)Cyc_Absyn_Times,_tmpE1A)))));}
break;}case 406: _LL3D0: {
# 2841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2845
{struct Cyc_Core_Opt*_tmpE1B;yyval=Cyc_YY9(((_tmpE1B=_cycalloc_atomic(sizeof(*_tmpE1B)),((_tmpE1B->v=(void*)Cyc_Absyn_Div,_tmpE1B)))));}
break;}case 407: _LL3D1: {
# 2842 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2846
{struct Cyc_Core_Opt*_tmpE1C;yyval=Cyc_YY9(((_tmpE1C=_cycalloc_atomic(sizeof(*_tmpE1C)),((_tmpE1C->v=(void*)Cyc_Absyn_Mod,_tmpE1C)))));}
break;}case 408: _LL3D2: {
# 2843 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2847
{struct Cyc_Core_Opt*_tmpE1D;yyval=Cyc_YY9(((_tmpE1D=_cycalloc_atomic(sizeof(*_tmpE1D)),((_tmpE1D->v=(void*)Cyc_Absyn_Plus,_tmpE1D)))));}
break;}case 409: _LL3D3: {
# 2844 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848
{struct Cyc_Core_Opt*_tmpE1E;yyval=Cyc_YY9(((_tmpE1E=_cycalloc_atomic(sizeof(*_tmpE1E)),((_tmpE1E->v=(void*)Cyc_Absyn_Minus,_tmpE1E)))));}
break;}case 410: _LL3D4: {
# 2845 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849
{struct Cyc_Core_Opt*_tmpE1F;yyval=Cyc_YY9(((_tmpE1F=_cycalloc_atomic(sizeof(*_tmpE1F)),((_tmpE1F->v=(void*)Cyc_Absyn_Bitlshift,_tmpE1F)))));}
break;}case 411: _LL3D5: {
# 2846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850
{struct Cyc_Core_Opt*_tmpE20;yyval=Cyc_YY9(((_tmpE20=_cycalloc_atomic(sizeof(*_tmpE20)),((_tmpE20->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE20)))));}
break;}case 412: _LL3D6: {
# 2847 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851
{struct Cyc_Core_Opt*_tmpE21;yyval=Cyc_YY9(((_tmpE21=_cycalloc_atomic(sizeof(*_tmpE21)),((_tmpE21->v=(void*)Cyc_Absyn_Bitand,_tmpE21)))));}
break;}case 413: _LL3D7: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852
{struct Cyc_Core_Opt*_tmpE22;yyval=Cyc_YY9(((_tmpE22=_cycalloc_atomic(sizeof(*_tmpE22)),((_tmpE22->v=(void*)Cyc_Absyn_Bitxor,_tmpE22)))));}
break;}case 414: _LL3D8: {
# 2849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853
{struct Cyc_Core_Opt*_tmpE23;yyval=Cyc_YY9(((_tmpE23=_cycalloc_atomic(sizeof(*_tmpE23)),((_tmpE23->v=(void*)Cyc_Absyn_Bitor,_tmpE23)))));}
break;}case 415: _LL3D9: {
# 2854 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858
yyval=yyyyvsp[0];
break;}case 416: _LL3DA: {
# 2856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2860
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 417: _LL3DB: {
# 2859 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2863
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 418: _LL3DC: {
# 2862 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2866
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3DD: {
# 2864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2868
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 420: _LL3DE: {
# 2866 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2870
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3DF: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2872
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3E0: {
# 2872 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2876
yyval=yyyyvsp[0];
break;}case 423: _LL3E1: {
# 2876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2880
yyval=yyyyvsp[0];
break;}case 424: _LL3E2: {
# 2878 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2882
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E3: {
# 2882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2886
yyval=yyyyvsp[0];
break;}case 426: _LL3E4: {
# 2884 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2888
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E5: {
# 2888 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2892
yyval=yyyyvsp[0];
break;}case 428: _LL3E6: {
# 2890 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2894
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3E7: {
# 2894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2898
yyval=yyyyvsp[0];
break;}case 430: _LL3E8: {
# 2896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2900
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3E9: {
# 2900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2904
yyval=yyyyvsp[0];
break;}case 432: _LL3EA: {
# 2902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3EB: {
# 2906 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910
yyval=yyyyvsp[0];
break;}case 434: _LL3EC: {
# 2908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2912
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3ED: {
# 2910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2914
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3EE: {
# 2914 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2918
yyval=yyyyvsp[0];
break;}case 437: _LL3EF: {
# 2916 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2920
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 438: _LL3F0: {
# 2918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2922
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F1: {
# 2920 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2924
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3F2: {
# 2922 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F3: {
# 2926 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930
yyval=yyyyvsp[0];
break;}case 442: _LL3F4: {
# 2928 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2932
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL3F5: {
# 2930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2934
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F6: {
# 2934 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938
yyval=yyyyvsp[0];
break;}case 445: _LL3F7: {
# 2936 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2940
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3F8: {
# 2938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2942
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F9: {
# 2942 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2946
yyval=yyyyvsp[0];
break;}case 448: _LL3FA: {
# 2944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2948
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3FB: {
# 2946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2950
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FC: {
# 2948 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2952
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3FD: {
# 2952 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956
yyval=yyyyvsp[0];
break;}case 452: _LL3FE: {
# 2954 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2958
void*_tmp7A2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7A2,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2961
break;}case 453: _LL3FF: {
# 2960 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2964
yyval=yyyyvsp[0];
break;}case 454: _LL400: {
# 2961 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2965
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL401: {
# 2962 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2966
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL402: {
# 2963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2967
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL403: {
# 2964 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2968
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL404: {
# 2965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2969
yyval=yyyyvsp[1];
break;}case 459: _LL405: {
# 2966 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL406: {
# 2968 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2972
void*_tmp7A3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7A3,(unsigned int)(yyyylsp[0]).first_line));
# 2975
break;}case 461: _LL407: {
# 2971 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2975
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL408: {
# 2973 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2977
void*_tmp7A4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7A4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2980
break;}case 463: _LL409: {
# 2978 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2982
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE29;struct Cyc_Absyn_MallocInfo _tmpE28;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE27;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27[0]=((_tmpE29.tag=32,((_tmpE29.f1=((_tmpE28.is_calloc=0,((_tmpE28.rgn=0,((_tmpE28.elt_type=0,((_tmpE28.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE28.fat_result=0,_tmpE28)))))))))),_tmpE29)))),_tmpE27)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2984
break;}case 464: _LL40A: {
# 2981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2985
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE2F;struct Cyc_Absyn_MallocInfo _tmpE2E;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE2D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2F.tag=32,((_tmpE2F.f1=((_tmpE2E.is_calloc=0,((_tmpE2E.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE2E.elt_type=0,((_tmpE2E.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE2E.fat_result=0,_tmpE2E)))))))))),_tmpE2F)))),_tmpE2D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2987
break;}case 465: _LL40B: {
# 2984 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2988
void*_tmp7AB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE39;void**_tmpE38;struct Cyc_Absyn_MallocInfo _tmpE37;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE36;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=((_tmpE39.tag=32,((_tmpE39.f1=((_tmpE37.is_calloc=1,((_tmpE37.rgn=0,((_tmpE37.elt_type=((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=_tmp7AB,_tmpE38)))),((_tmpE37.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE37.fat_result=0,_tmpE37)))))))))),_tmpE39)))),_tmpE36)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2991
break;}case 466: _LL40C: {
# 2989 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2993
void*_tmp7B0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE43;void**_tmpE42;struct Cyc_Absyn_MallocInfo _tmpE41;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE40;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=((_tmpE43.tag=32,((_tmpE43.f1=((_tmpE41.is_calloc=1,((_tmpE41.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE41.elt_type=((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42[0]=_tmp7B0,_tmpE42)))),((_tmpE41.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE41.fat_result=0,_tmpE41)))))))))),_tmpE43)))),_tmpE40)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2996
break;}case 467: _LL40D: {
# 2993 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2997
{struct Cyc_Absyn_Exp*_tmpE44[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE44[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE44,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 468: _LL40E: {
# 2995 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2999
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE4A;struct _dyneither_ptr*_tmpE49;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE48;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48[0]=((_tmpE4A.tag=36,((_tmpE4A.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE4A.f2=((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE49)))),_tmpE4A)))))),_tmpE48)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 469: _LL40F: {
# 2997 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3001
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE50;struct _dyneither_ptr*_tmpE4F;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE4E;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE50.tag=36,((_tmpE50.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE50.f2=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE4F)))),_tmpE50)))))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 470: _LL410: {
# 2999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3003
void*_tmp7BC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7BC,(unsigned int)(yyyylsp[0]).first_line));
break;}case 471: _LL411: {
# 3002 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3006
int _tmp7BE;struct _dyneither_ptr _tmp7BF;struct _tuple19 _tmp7BD=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp7BE=_tmp7BD.f1;_tmp7BF=_tmp7BD.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7BE,_tmp7BF,(unsigned int)(yyyylsp[0]).first_line));
break;}case 472: _LL412: {
# 3007 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3011
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 473: _LL413: {
# 3008 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3012
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 474: _LL414: {
# 3009 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3013
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 475: _LL415: {
# 3014 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018
yyval=yyyyvsp[0];
break;}case 476: _LL416: {
# 3016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3020
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL417: {
# 3018 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3022
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL418: {
# 3020 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3024
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL419: {
# 3022 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3026
{struct _dyneither_ptr*_tmpE51;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE51)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 480: _LL41A: {
# 3024 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028
{struct _dyneither_ptr*_tmpE52;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE52)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 481: _LL41B: {
# 3026 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3030
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 482: _LL41C: {
# 3028 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3032
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL41D: {
# 3030 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3034
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE55;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE54;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=24,((_tmpE55.f1=Cyc_yyget_YY38(yyyyvsp[1]),((_tmpE55.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE55)))))),_tmpE54)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 484: _LL41E: {
# 3032 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3036
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE58;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE57;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=24,((_tmpE58.f1=Cyc_yyget_YY38(yyyyvsp[1]),((_tmpE58.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE58)))))),_tmpE57)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 485: _LL41F: {
# 3037 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3041
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE62;struct _dyneither_ptr*_tmpE61;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE60;struct Cyc_List_List*_tmpE5F;yyval=Cyc_YY3(((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F->hd=(void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE60.tag=0,((_tmpE60.f1=((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE61)))),_tmpE60)))),_tmpE62)))),((_tmpE5F->tl=0,_tmpE5F)))))));}
break;}case 486: _LL420: {
# 3040 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3044
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE68;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE67;struct Cyc_List_List*_tmpE66;yyval=Cyc_YY3(((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->hd=(void*)((_tmpE68=_cycalloc_atomic(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE67.tag=1,((_tmpE67.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE67)))),_tmpE68)))),((_tmpE66->tl=0,_tmpE66)))))));}
break;}case 487: _LL421: {
# 3042 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3046
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE72;struct _dyneither_ptr*_tmpE71;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE70;struct Cyc_List_List*_tmpE6F;yyval=Cyc_YY3(((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->hd=(void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE70.tag=0,((_tmpE70.f1=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE71)))),_tmpE70)))),_tmpE72)))),((_tmpE6F->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE6F)))))));}
break;}case 488: _LL422: {
# 3045 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3049
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE78;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE77;struct Cyc_List_List*_tmpE76;yyval=Cyc_YY3(((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->hd=(void*)((_tmpE78=_cycalloc_atomic(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE77.tag=1,((_tmpE77.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpE77)))),_tmpE78)))),((_tmpE76->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE76)))))));}
break;}case 489: _LL423: {
# 3051 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3055
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL424: {
# 3053 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3057
yyval=yyyyvsp[0];
break;}case 491: _LL425: {
# 3055 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3059
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 492: _LL426: {
# 3057 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3061
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 493: _LL427: {
# 3059 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3063
yyval=yyyyvsp[1];
break;}case 494: _LL428: {
# 3064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3068
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 495: _LL429: {
# 3066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3070
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL42A: {
# 3069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3073
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL42B: {
# 3072 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3076
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE7B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE7A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7B.tag=27,((_tmpE7B.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE7B.f2=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpE7B.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpE7B.f4=0,_tmpE7B)))))))))),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 498: _LL42C: {
# 3075 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3079
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL42D: {
# 3079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3083
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 500: _LL42E: {
# 3085 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3089
{struct Cyc_List_List*_tmpE7C;yyval=Cyc_YY6(((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7C->tl=0,_tmpE7C)))))));}
break;}case 501: _LL42F: {
# 3087 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3091
{struct Cyc_List_List*_tmpE7D;yyval=Cyc_YY6(((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE7D->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpE7D)))))));}
break;}case 502: _LL430: {
# 3093 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3097
struct _tuple5 _tmp7D8=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7D8.f1,_tmp7D8.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL431: {
# 3094 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3098
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL432: {
# 3095 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3099
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL433: {
# 3096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101
struct _dyneither_ptr _tmp7D9=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7D9);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7D9,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3109
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7D9,i,(unsigned int)(yyyylsp[0]).first_line));
# 3111
break;}case 506: _LL434: {
# 3108 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3112
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL435: {
# 3112 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3116
{struct _dyneither_ptr*_tmpE80;struct _tuple0*_tmpE7F;yyval=Cyc_QualId_tok(((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->f1=Cyc_Absyn_Rel_n(0),((_tmpE7F->f2=((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE80)))),_tmpE7F)))))));}
break;}case 508: _LL436: {
# 3113 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117
yyval=yyyyvsp[0];
break;}case 509: _LL437: {
# 3116 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120
{struct _dyneither_ptr*_tmpE83;struct _tuple0*_tmpE82;yyval=Cyc_QualId_tok(((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->f1=Cyc_Absyn_Rel_n(0),((_tmpE82->f2=((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE83)))),_tmpE82)))))));}
break;}case 510: _LL438: {
# 3117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121
yyval=yyyyvsp[0];
break;}case 511: _LL439: {
# 3122 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126
yyval=yyyyvsp[0];
break;}case 512: _LL43A: {
# 3123 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3127
yyval=yyyyvsp[0];
break;}case 513: _LL43B: {
# 3126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3130
yyval=yyyyvsp[0];
break;}case 514: _LL43C: {
# 3127 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131
yyval=yyyyvsp[0];
break;}case 515: _LL43D: {
# 3131 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136
break;}case 516: _LL43E: {
# 3132 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136
yylex_buf->lex_curr_pos -=1;
break;}default: _LL43F:
# 3141
 break;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(517,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6376) && Cyc_yycheck[_check_known_subscript_notnull(6377,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6377,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(144,yyn - 146)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];
# 431
if(yyn > - 32768  && yyn < 6376){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6377,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)])+ 15,count ++);}
{unsigned int _tmpE88;unsigned int _tmpE87;struct _dyneither_ptr _tmpE86;char*_tmpE85;unsigned int _tmpE84;msg=((_tmpE84=(unsigned int)(sze + 15),((_tmpE85=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpE84 + 1)),((_tmpE86=_tag_dyneither(_tmpE85,sizeof(char),_tmpE84 + 1),((((_tmpE87=_tmpE84,((_tmpE89(& _tmpE88,& _tmpE87,& _tmpE85),_tmpE85[_tmpE87]=(char)0)))),_tmpE86))))))));}
{const char*_tmpE8A;Cyc_strcpy(msg,((_tmpE8A="parse error",_tag_dyneither(_tmpE8A,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6377,x + yyn)]== x){
# 453
{const char*_tmpE8C;const char*_tmpE8B;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpE8C=", expecting `",_tag_dyneither(_tmpE8C,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpE8B=" or `",_tag_dyneither(_tmpE8B,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)]);
{const char*_tmpE8D;Cyc_strcat(msg,((_tmpE8D="'",_tag_dyneither(_tmpE8D,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpE8E;Cyc_yyerror(((_tmpE8E="parse error",_tag_dyneither(_tmpE8E,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7E7=1;_npop_handler(0);return _tmp7E7;}
# 485
yychar=- 2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp7E8=1;_npop_handler(0);return _tmp7E8;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6376) || Cyc_yycheck[_check_known_subscript_notnull(6377,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6377,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1027){
int _tmp7E9=0;_npop_handler(0);return _tmp7E9;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3135 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp7F3=v;int _tmp7F4;char _tmp7F5;struct _dyneither_ptr _tmp7F6;union Cyc_Absyn_Nmspace _tmp7F7;struct _dyneither_ptr*_tmp7F8;_LL442: if((_tmp7F3.Int_tok).tag != 1)goto _LL444;_tmp7F4=((struct _tuple5)(_tmp7F3.Int_tok).val).f2;_LL443:
{const char*_tmpE92;void*_tmpE91[1];struct Cyc_Int_pa_PrintArg_struct _tmpE90;(_tmpE90.tag=1,((_tmpE90.f1=(unsigned long)_tmp7F4,((_tmpE91[0]=& _tmpE90,Cyc_fprintf(Cyc_stderr,((_tmpE92="%d",_tag_dyneither(_tmpE92,sizeof(char),3))),_tag_dyneither(_tmpE91,sizeof(void*),1)))))));}goto _LL441;_LL444: if((_tmp7F3.Char_tok).tag != 2)goto _LL446;_tmp7F5=(char)(_tmp7F3.Char_tok).val;_LL445:
{const char*_tmpE96;void*_tmpE95[1];struct Cyc_Int_pa_PrintArg_struct _tmpE94;(_tmpE94.tag=1,((_tmpE94.f1=(unsigned long)((int)_tmp7F5),((_tmpE95[0]=& _tmpE94,Cyc_fprintf(Cyc_stderr,((_tmpE96="%c",_tag_dyneither(_tmpE96,sizeof(char),3))),_tag_dyneither(_tmpE95,sizeof(void*),1)))))));}goto _LL441;_LL446: if((_tmp7F3.String_tok).tag != 3)goto _LL448;_tmp7F6=(struct _dyneither_ptr)(_tmp7F3.String_tok).val;_LL447:
{const char*_tmpE9A;void*_tmpE99[1];struct Cyc_String_pa_PrintArg_struct _tmpE98;(_tmpE98.tag=0,((_tmpE98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F6),((_tmpE99[0]=& _tmpE98,Cyc_fprintf(Cyc_stderr,((_tmpE9A="\"%s\"",_tag_dyneither(_tmpE9A,sizeof(char),5))),_tag_dyneither(_tmpE99,sizeof(void*),1)))))));}goto _LL441;_LL448: if((_tmp7F3.QualId_tok).tag != 5)goto _LL44A;_tmp7F7=((struct _tuple0*)(_tmp7F3.QualId_tok).val)->f1;_tmp7F8=((struct _tuple0*)(_tmp7F3.QualId_tok).val)->f2;_LL449: {
# 3141
struct Cyc_List_List*_tmp802=0;
{union Cyc_Absyn_Nmspace _tmp803=_tmp7F7;struct Cyc_List_List*_tmp804;struct Cyc_List_List*_tmp805;struct Cyc_List_List*_tmp806;_LL44D: if((_tmp803.Rel_n).tag != 1)goto _LL44F;_tmp804=(struct Cyc_List_List*)(_tmp803.Rel_n).val;_LL44E:
 _tmp802=_tmp804;goto _LL44C;_LL44F: if((_tmp803.Abs_n).tag != 2)goto _LL451;_tmp805=(struct Cyc_List_List*)(_tmp803.Abs_n).val;_LL450:
 _tmp802=_tmp805;goto _LL44C;_LL451: if((_tmp803.C_n).tag != 3)goto _LL453;_tmp806=(struct Cyc_List_List*)(_tmp803.C_n).val;_LL452:
 _tmp802=_tmp806;goto _LL44C;_LL453: if((_tmp803.Loc_n).tag != 4)goto _LL44C;_LL454:
 goto _LL44C;_LL44C:;}
# 3148
for(0;_tmp802 != 0;_tmp802=_tmp802->tl){
const char*_tmpE9E;void*_tmpE9D[1];struct Cyc_String_pa_PrintArg_struct _tmpE9C;(_tmpE9C.tag=0,((_tmpE9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp802->hd)),((_tmpE9D[0]=& _tmpE9C,Cyc_fprintf(Cyc_stderr,((_tmpE9E="%s::",_tag_dyneither(_tmpE9E,sizeof(char),5))),_tag_dyneither(_tmpE9D,sizeof(void*),1)))))));}
{const char*_tmpEA2;void*_tmpEA1[1];struct Cyc_String_pa_PrintArg_struct _tmpEA0;(_tmpEA0.tag=0,((_tmpEA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F8),((_tmpEA1[0]=& _tmpEA0,Cyc_fprintf(Cyc_stderr,((_tmpEA2="%s::",_tag_dyneither(_tmpEA2,sizeof(char),5))),_tag_dyneither(_tmpEA1,sizeof(void*),1)))))));}
goto _LL441;}_LL44A:;_LL44B:
{const char*_tmpEA5;void*_tmpEA4;(_tmpEA4=0,Cyc_fprintf(Cyc_stderr,((_tmpEA5="?",_tag_dyneither(_tmpEA5,sizeof(char),2))),_tag_dyneither(_tmpEA4,sizeof(void*),0)));}goto _LL441;_LL441:;}
# 3157
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp80F=_new_region("yyr");struct _RegionHandle*yyr=& _tmp80F;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp810=Cyc_Parse_parse_result;_npop_handler(0);return _tmp810;};
# 3160
;_pop_region(yyr);};}
