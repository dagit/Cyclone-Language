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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
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
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
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
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
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
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
# 482
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
# 483
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 484
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
# 485
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
# 486
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 488
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
# 489
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
# 490
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
# 491
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
# 492
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
# 493
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
# 494
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
# 495
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
# 496
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
# 497
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
# 721
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 883
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
# 884
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 910
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 941
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
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1077
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1093
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1094
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1098
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
# 1102
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
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 133
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 290 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 71 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
# 72
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
# 73
void Cyc_Lex_leave_namespace();
# 74
void Cyc_Lex_enter_using(struct _tuple0*);
# 75
void Cyc_Lex_leave_using();struct _tuple10{struct _tuple10*tl;};struct Cyc_Parse_FlatList{struct _tuple10*tl;};
# 91 "parse.y"
typedef struct _tuple10*Cyc_Parse_flat_list_t;
# 93
struct _tuple10*Cyc_Parse_flat_imp_rev(struct _tuple10*x){
# 94
if(x == 0)return x;else{
# 96
struct _tuple10*first=x;
# 97
struct _tuple10*second=x->tl;
# 98
x->tl=0;
# 99
while(second != 0){
# 100
struct _tuple10*temp=second->tl;
# 101
second->tl=first;
# 102
first=second;
# 103
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
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple11{struct _tuple11*tl;struct Cyc_Parse_Declarator hd;};
# 144
typedef struct _tuple11*Cyc_Parse_declarators_t;struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple12 hd;};
# 145
typedef struct _tuple13*Cyc_Parse_declarator_list_t;
# 147
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple13*ds,struct _tuple11**decls,struct Cyc_List_List**es){
# 151
struct _tuple11*declarators=0;
# 152
struct Cyc_List_List*exprs=0;
# 153
for(0;ds != 0;ds=ds->tl){
# 154
struct Cyc_Parse_Declarator _tmp1;struct Cyc_Absyn_Exp*_tmp2;struct _tuple12 _tmp0=ds->hd;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
# 155
{struct _tuple11*_tmp84E;declarators=((_tmp84E=_region_malloc(r,sizeof(*_tmp84E)),((_tmp84E->tl=declarators,((_tmp84E->hd=_tmp1,_tmp84E))))));}{
# 156
struct Cyc_List_List*_tmp84F;exprs=((_tmp84F=_region_malloc(r,sizeof(*_tmp84F)),((_tmp84F->hd=_tmp2,((_tmp84F->tl=exprs,_tmp84F))))));};}
# 158
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
# 159
*decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 165
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 168
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 170
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 175
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 178
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
# 179
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 181
static void*Cyc_Parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
# 184
struct Cyc_Position_Exit_exn_struct _tmp852;struct Cyc_Position_Exit_exn_struct*_tmp851;(int)_throw((void*)((_tmp851=_cycalloc_atomic(sizeof(*_tmp851)),((_tmp851[0]=((_tmp852.tag=Cyc_Position_Exit,_tmp852)),_tmp851)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
# 187
{const char*_tmp857;void*_tmp856[2];struct Cyc_String_pa_PrintArg_struct _tmp855;struct Cyc_String_pa_PrintArg_struct _tmp854;(_tmp854.tag=0,((_tmp854.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 188
Cyc_Position_string_of_segment(sg)),((_tmp856[0]=& _tmp855,((_tmp856[1]=& _tmp854,Cyc_fprintf(Cyc_stderr,((_tmp857="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp857,sizeof(char),46))),_tag_dyneither(_tmp856,sizeof(void*),2)))))))))))));}
# 189
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct _tuple8 _tmpC;struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpB=tqt;_tmpC=*_tmpB;_tmpD=_tmpC.f2;_tmpE=_tmpC.f3;
# 195
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
# 196
if(_tmpD.loc != 0)loc=_tmpD.loc;{
# 197
const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_warn(loc,((_tmp85A="qualifier on type is ignored",_tag_dyneither(_tmp85A,sizeof(char),29))),_tag_dyneither(_tmp859,sizeof(void*),0)));};}
# 199
return _tmpE;}struct _tuple15{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 202
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 208
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
# 209
for(0;pqs != 0;pqs=pqs->tl){
# 210
void*_tmp11=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmp19;void*_tmp1B;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp12=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp12->tag != 4)goto _LL3;}_LL2:
# 212
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp13->tag != 5)goto _LL5;}_LL4:
# 214
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp14->tag != 7)goto _LL7;}_LL6:
# 216
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp15->tag != 6)goto _LL9;}_LL8:
# 218
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp16->tag != 3)goto _LLB;}_LLA:
# 220
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp17->tag != 2)goto _LLD;}_LLC:
# 222
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp18->tag != 0)goto _LLF;else{_tmp19=_tmp18->f1;}}_LLE:
# 224
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp85D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp85C;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85D.tag=1,((_tmp85D.f1=_tmp19,_tmp85D)))),_tmp85C)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1A=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp11;if(_tmp1A->tag != 1)goto _LL0;else{_tmp1B=(void*)_tmp1A->f1;}}_LL10:
# 226
 rgn=_tmp1B;goto _LL0;_LL0:;}{
# 229
struct _tuple15 _tmp85E;return(_tmp85E.f1=nullable,((_tmp85E.f2=bound,((_tmp85E.f3=zeroterm,((_tmp85E.f4=rgn,_tmp85E)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
# 238
struct _dyneither_ptr*_tmp86B;const char*_tmp86A;void*_tmp869[1];struct Cyc_Int_pa_PrintArg_struct _tmp868;struct _tuple0*_tmp867;return(_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867->f1=Cyc_Absyn_Rel_n(0),((_tmp867->f2=(
# 239
(_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=(struct _dyneither_ptr)((_tmp868.tag=1,((_tmp868.f1=(unsigned long)enum_counter ++,((_tmp869[0]=& _tmp868,Cyc_aprintf(((_tmp86A="__anonymous_enum_%d__",_tag_dyneither(_tmp86A,sizeof(char),22))),_tag_dyneither(_tmp869,sizeof(void*),1)))))))),_tmp86B)))),_tmp867)))));}struct _tuple16{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple17{struct _tuple16*f1;struct Cyc_Absyn_Exp*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 246
struct _tuple17 _tmp25;struct _tuple16*_tmp26;struct _tuple16 _tmp27;struct _tuple0*_tmp28;struct Cyc_Absyn_Tqual _tmp29;void*_tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;struct _tuple17*_tmp24=field_info;_tmp25=*_tmp24;_tmp26=_tmp25.f1;_tmp27=*_tmp26;_tmp28=_tmp27.f1;_tmp29=_tmp27.f2;_tmp2A=_tmp27.f3;_tmp2B=_tmp27.f4;_tmp2C=_tmp27.f5;_tmp2D=_tmp25.f2;
# 247
if(_tmp2B != 0){
# 248
const char*_tmp86C;Cyc_Parse_err(((_tmp86C="bad type params in struct field",_tag_dyneither(_tmp86C,sizeof(char),32))),loc);}
# 249
if(Cyc_Absyn_is_qvar_qualified(_tmp28)){
# 250
const char*_tmp86D;Cyc_Parse_err(((_tmp86D="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp86D,sizeof(char),59))),loc);}{
# 251
struct Cyc_Absyn_Aggrfield*_tmp86E;return(_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E->name=(*_tmp28).f2,((_tmp86E->tq=_tmp29,((_tmp86E->type=_tmp2A,((_tmp86E->width=_tmp2D,((_tmp86E->attributes=_tmp2C,_tmp86E)))))))))));};}
# 255
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
# 256
struct Cyc_Parse_Type_specifier _tmp86F;return(_tmp86F.Signed_spec=0,((_tmp86F.Unsigned_spec=0,((_tmp86F.Short_spec=0,((_tmp86F.Long_spec=0,((_tmp86F.Long_Long_spec=0,((_tmp86F.Valid_type_spec=0,((_tmp86F.Type_spec=Cyc_Absyn_sint_typ,((_tmp86F.loc=loc,_tmp86F)))))))))))))));}
# 266
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
# 267
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
# 268
_tmp32.Type_spec=t;
# 269
_tmp32.Valid_type_spec=1;
# 270
return _tmp32;}
# 272
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
# 273
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
# 274
_tmp33.Signed_spec=1;
# 275
return _tmp33;}
# 277
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
# 278
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
# 279
_tmp34.Unsigned_spec=1;
# 280
return _tmp34;}
# 282
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
# 283
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
# 284
_tmp35.Short_spec=1;
# 285
return _tmp35;}
# 287
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
# 288
struct Cyc_Parse_Type_specifier _tmp36=Cyc_Parse_empty_spec(loc);
# 289
_tmp36.Long_spec=1;
# 290
return _tmp36;}
# 294
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 295
void*_tmp37=t;struct Cyc_Absyn_ArrayInfo _tmp39;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;struct Cyc_Absyn_Exp*_tmp3C;union Cyc_Absyn_Constraint*_tmp3D;unsigned int _tmp3E;_LL12: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 8)goto _LL14;else{_tmp39=_tmp38->f1;_tmp3A=_tmp39.elt_type;_tmp3B=_tmp39.tq;_tmp3C=_tmp39.num_elts;_tmp3D=_tmp39.zero_term;_tmp3E=_tmp39.zt_loc;}}_LL13: {
# 298
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp872;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp871;return Cyc_Absyn_starb_typ(_tmp3A,
# 299
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp3B,
# 301
_tmp3C == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)(
# 302
(_tmp871=_cycalloc(sizeof(*_tmp871)),((_tmp871[0]=((_tmp872.tag=1,((_tmp872.f1=(struct Cyc_Absyn_Exp*)_tmp3C,_tmp872)))),_tmp871))))),_tmp3D);}_LL14:;_LL15:
# 304
 return t;_LL11:;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 317 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
# 318
struct Cyc_List_List*_tmp41=0;
# 319
for(0;x != 0;x=x->tl){
# 320
struct _tuple8*_tmp42=(struct _tuple8*)x->hd;struct _tuple8 _tmp43;struct _dyneither_ptr*_tmp44;void*_tmp45;void*_tmp47;struct _tuple8 _tmp48;struct _dyneither_ptr*_tmp49;struct _dyneither_ptr _tmp4A;void*_tmp4B;void*_tmp4D;void*_tmp4F;void**_tmp50;_LL17: _tmp43=*_tmp42;_tmp44=_tmp43.f1;_tmp45=_tmp43.f3;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp45;if(_tmp46->tag != 19)goto _LL19;else{_tmp47=(void*)_tmp46->f1;}};if(!(_tmp44 != 0))goto _LL19;_LL18:
# 322
{void*_tmp51=_tmp47;void*_tmp53;void**_tmp54;_LL1E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51;if(_tmp52->tag != 1)goto _LL20;else{_tmp53=(void**)& _tmp52->f2;_tmp54=(void**)((void**)& _tmp52->f2);}}_LL1F: {
# 326
struct Cyc_String_pa_PrintArg_struct _tmp87A;void*_tmp879[1];const char*_tmp878;struct _dyneither_ptr*_tmp877;struct _dyneither_ptr*nm=(_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=(struct _dyneither_ptr)((_tmp87A.tag=0,((_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp44)),((_tmp879[0]=& _tmp87A,Cyc_aprintf(((_tmp878="`%s",_tag_dyneither(_tmp878,sizeof(char),4))),_tag_dyneither(_tmp879,sizeof(void*),1)))))))),_tmp877)));
# 327
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp889;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp888;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp887;struct Cyc_Absyn_Tvar*_tmp886;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp885;*_tmp54=(void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp889.tag=2,((_tmp889.f1=((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886->name=nm,((_tmp886->identity=- 1,((_tmp886->kind=(void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp887.tag=0,((_tmp887.f1=& Cyc_Tcutil_ik,_tmp887)))),_tmp888)))),_tmp886)))))))),_tmp889)))),_tmp885))));}
# 328
goto _LL1D;}_LL20:;_LL21:
# 329
 goto _LL1D;_LL1D:;}
# 331
{struct _tuple18*_tmp88C;struct Cyc_List_List*_tmp88B;_tmp41=((_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B->hd=((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C->f1=(struct _dyneither_ptr*)_tmp44,((_tmp88C->f2=_tmp47,_tmp88C)))))),((_tmp88B->tl=_tmp41,_tmp88B))))));}goto _LL16;_LL19: _tmp48=*_tmp42;_tmp49=_tmp48.f1;if(_tmp49 == 0)goto _LL1B;_tmp4A=*_tmp49;_tmp4B=_tmp48.f3;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4B;if(_tmp4C->tag != 15)goto _LL1B;else{_tmp4D=(void*)_tmp4C->f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D;if(_tmp4E->tag != 1)goto _LL1B;else{_tmp4F=(void**)& _tmp4E->f2;_tmp50=(void**)((void**)& _tmp4E->f2);}};}};_LL1A: {
# 335
struct Cyc_String_pa_PrintArg_struct _tmp894;void*_tmp893[1];const char*_tmp892;struct _dyneither_ptr*_tmp891;struct _dyneither_ptr*nm=(_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=(struct _dyneither_ptr)((_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((_tmp893[0]=& _tmp894,Cyc_aprintf(((_tmp892="`%s",_tag_dyneither(_tmp892,sizeof(char),4))),_tag_dyneither(_tmp893,sizeof(void*),1)))))))),_tmp891)));
# 336
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A3;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8A2;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8A1;struct Cyc_Absyn_Tvar*_tmp8A0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89F;*_tmp50=(void*)((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F[0]=((_tmp8A3.tag=2,((_tmp8A3.f1=((_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0->name=nm,((_tmp8A0->identity=- 1,((_tmp8A0->kind=(void*)((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A1.tag=0,((_tmp8A1.f1=& Cyc_Tcutil_rk,_tmp8A1)))),_tmp8A2)))),_tmp8A0)))))))),_tmp8A3)))),_tmp89F))));}
# 337
goto _LL16;}_LL1B:;_LL1C:
# 338
 goto _LL16;_LL16:;}
# 341
return _tmp41;}
# 345
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
# 346
struct Cyc_List_List*_tmp69=0;
# 347
for(0;x != 0;x=x->tl){
# 348
void*_tmp6A=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp6C;_LL23: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6A;if(_tmp6B->tag != 19)goto _LL25;else{_tmp6C=(void*)_tmp6B->f1;}}_LL24:
# 350
{struct _tuple18*_tmp8A6;struct Cyc_List_List*_tmp8A5;_tmp69=((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->hd=((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8A6->f2=_tmp6C,_tmp8A6)))))),((_tmp8A5->tl=_tmp69,_tmp8A5))))));}goto _LL22;_LL25:;_LL26:
# 351
 goto _LL22;_LL22:;}
# 354
return _tmp69;}
# 358
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
# 359
{void*_tmp6F=e->r;struct _tuple0*_tmp71;struct _tuple0 _tmp72;union Cyc_Absyn_Nmspace _tmp73;struct Cyc_List_List*_tmp74;struct _dyneither_ptr*_tmp75;_LL28: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp70=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp6F;if(_tmp70->tag != 1)goto _LL2A;else{_tmp71=_tmp70->f1;_tmp72=*_tmp71;_tmp73=_tmp72.f1;if((_tmp73.Rel_n).tag != 1)goto _LL2A;_tmp74=(struct Cyc_List_List*)(_tmp73.Rel_n).val;if(_tmp74 != 0)goto _LL2A;_tmp75=_tmp72.f2;}}_LL29:
# 361
{struct Cyc_List_List*_tmp76=tags;for(0;_tmp76 != 0;_tmp76=_tmp76->tl){
# 362
struct _tuple18 _tmp78;struct _dyneither_ptr*_tmp79;void*_tmp7A;struct _tuple18*_tmp77=(struct _tuple18*)_tmp76->hd;_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;
# 363
if(Cyc_strptrcmp(_tmp79,_tmp75)== 0){
# 364
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8A9;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8A8;return Cyc_Absyn_new_exp((void*)((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=37,((_tmp8A9.f1=(void*)Cyc_Tcutil_copy_type(_tmp7A),_tmp8A9)))),_tmp8A8)))),e->loc);}}}
# 366
goto _LL27;_LL2A:;_LL2B:
# 367
 goto _LL27;_LL27:;}
# 369
return e;}
# 374
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
# 375
{void*_tmp7D=t;struct Cyc_Absyn_ArrayInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;struct Cyc_Absyn_Exp*_tmp82;union Cyc_Absyn_Constraint*_tmp83;unsigned int _tmp84;struct Cyc_Absyn_PtrInfo _tmp86;void*_tmp87;struct Cyc_Absyn_Tqual _tmp88;struct Cyc_Absyn_PtrAtts _tmp89;void*_tmp8A;union Cyc_Absyn_Constraint*_tmp8B;union Cyc_Absyn_Constraint*_tmp8C;union Cyc_Absyn_Constraint*_tmp8D;struct Cyc_Absyn_PtrLoc*_tmp8E;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7D;if(_tmp7E->tag != 8)goto _LL2F;else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_type;_tmp81=_tmp7F.tq;_tmp82=_tmp7F.num_elts;_tmp83=_tmp7F.zero_term;_tmp84=_tmp7F.zt_loc;}}_LL2E: {
# 377
struct Cyc_Absyn_Exp*nelts2=_tmp82;
# 378
if(_tmp82 != 0)
# 379
nelts2=(struct Cyc_Absyn_Exp*)Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp82);{
# 381
void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp80);
# 382
if(_tmp82 != nelts2  || _tmp80 != _tmp8F){
# 383
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8AF;struct Cyc_Absyn_ArrayInfo _tmp8AE;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8AD;return(void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AF.tag=8,((_tmp8AF.f1=((_tmp8AE.elt_type=_tmp8F,((_tmp8AE.tq=_tmp81,((_tmp8AE.num_elts=nelts2,((_tmp8AE.zero_term=_tmp83,((_tmp8AE.zt_loc=_tmp84,_tmp8AE)))))))))),_tmp8AF)))),_tmp8AD))));}
# 384
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7D;if(_tmp85->tag != 5)goto _LL31;else{_tmp86=_tmp85->f1;_tmp87=_tmp86.elt_typ;_tmp88=_tmp86.elt_tq;_tmp89=_tmp86.ptr_atts;_tmp8A=_tmp89.rgn;_tmp8B=_tmp89.nullable;_tmp8C=_tmp89.bounds;_tmp8D=_tmp89.zero_term;_tmp8E=_tmp89.ptrloc;}}_LL30: {
# 386
void*_tmp93=Cyc_Parse_substitute_tags(tags,_tmp87);
# 387
union Cyc_Absyn_Constraint*_tmp94=_tmp8C;
# 388
{union Cyc_Absyn_Constraint _tmp95=*_tmp8C;void*_tmp96;struct Cyc_Absyn_Exp*_tmp98;_LL34: if((_tmp95.Eq_constr).tag != 1)goto _LL36;_tmp96=(void*)(_tmp95.Eq_constr).val;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp97=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp96;if(_tmp97->tag != 1)goto _LL36;else{_tmp98=_tmp97->f1;}};_LL35:
# 390
 _tmp98=Cyc_Parse_substitute_tags_exp(tags,_tmp98);
# 391
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8B5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8B4;union Cyc_Absyn_Constraint*_tmp8B3;_tmp94=((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),(((_tmp8B3->Eq_constr).val=(void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B4.tag=1,((_tmp8B4.f1=_tmp98,_tmp8B4)))),_tmp8B5)))),(((_tmp8B3->Eq_constr).tag=1,_tmp8B3))))));}
# 392
goto _LL33;_LL36:;_LL37:
# 393
 goto _LL33;_LL33:;}
# 395
if(_tmp93 != _tmp87  || _tmp94 != _tmp8C){
# 396
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8BF;struct Cyc_Absyn_PtrAtts _tmp8BE;struct Cyc_Absyn_PtrInfo _tmp8BD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8BC;return(void*)((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BF.tag=5,((_tmp8BF.f1=((_tmp8BD.elt_typ=_tmp93,((_tmp8BD.elt_tq=_tmp88,((_tmp8BD.ptr_atts=((_tmp8BE.rgn=_tmp8A,((_tmp8BE.nullable=_tmp8B,((_tmp8BE.bounds=_tmp94,((_tmp8BE.zero_term=_tmp8D,((_tmp8BE.ptrloc=_tmp8E,_tmp8BE)))))))))),_tmp8BD)))))),_tmp8BF)))),_tmp8BC))));}
# 397
goto _LL2C;}_LL31:;_LL32:
# 400
 goto _LL2C;_LL2C:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
# 408
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 416
struct _tuple19*_tmp8C0;return(_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0->f1=(*t).f2,((_tmp8C0->f2=(*t).f3,_tmp8C0)))));}
# 419
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
# 420
void*_tmpA1=d->r;struct Cyc_Absyn_Vardecl*_tmpA3;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1;if(_tmpA2->tag != 0)goto _LL3B;else{_tmpA3=_tmpA2->f1;}}_LL3A:
# 421
 return _tmpA3;_LL3B:;_LL3C: {
# 422
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8C3="bad declaration in `forarray' statement",_tag_dyneither(_tmp8C3,sizeof(char),40))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}_LL38:;}
# 426
static int Cyc_Parse_is_typeparam(void*tm){
# 427
void*_tmpA6=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA7=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpA6;if(_tmpA7->tag != 4)goto _LL40;}_LL3F:
# 428
 return 1;_LL40:;_LL41:
# 429
 return 0;_LL3D:;}
# 435
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
# 436
const char*_tmp8C4;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C4="`H",_tag_dyneither(_tmp8C4,sizeof(char),3))))== 0)
# 437
return(void*)& Cyc_Absyn_HeapRgn_val;else{
# 438
const char*_tmp8C5;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C5="`U",_tag_dyneither(_tmp8C5,sizeof(char),3))))== 0)
# 439
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 440
const char*_tmp8C6;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C6="`RC",_tag_dyneither(_tmp8C6,sizeof(char),4))))== 0)
# 441
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 443
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D0;struct _dyneither_ptr*_tmp8CF;struct Cyc_Absyn_Tvar*_tmp8CE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8CD;return(void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8D0.tag=2,((_tmp8D0.f1=((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->name=((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=s,_tmp8CF)))),((_tmp8CE->identity=- 1,((_tmp8CE->kind=k,_tmp8CE)))))))),_tmp8D0)))),_tmp8CD))));}}}}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
# 447
void*k;
# 448
{void*_tmpAF=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmpB2;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL45;}_LL44:
# 449
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8D3;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8D2;k=(void*)((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=1,((_tmp8D3.f1=0,_tmp8D3)))),_tmp8D2))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL47;else{_tmpB2=_tmpB1->f2;}}_LL46:
# 450
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8D6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8D5;k=(void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D6.tag=2,((_tmp8D6.f1=0,((_tmp8D6.f2=_tmpB2,_tmp8D6)))))),_tmp8D5))));}goto _LL42;_LL47:;_LL48:
# 451
 k=_tmpAF;goto _LL42;_LL42:;}{
# 453
struct Cyc_Absyn_Tvar*_tmp8D7;return(_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7->name=t->name,((_tmp8D7->identity=- 1,((_tmp8D7->kind=k,_tmp8D7)))))));};}
# 460
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
# 461
void*_tmpB8=t;struct Cyc_Absyn_Tvar*_tmpBA;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB8;if(_tmpB9->tag != 2)goto _LL4C;else{_tmpBA=_tmpB9->f1;}}_LL4B:
# 462
 return _tmpBA;_LL4C:;_LL4D: {
# 463
const char*_tmp8DA;void*_tmp8D9;(_tmp8D9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8DA="expecting a list of type variables, not types",_tag_dyneither(_tmp8DA,sizeof(char),46))),_tag_dyneither(_tmp8D9,sizeof(void*),0)));}_LL49:;}
# 466
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
# 467
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8DD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8DC;return(void*)((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=2,((_tmp8DD.f1=pr,_tmp8DD)))),_tmp8DC))));}
# 471
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
# 472
void*_tmpBF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC1;struct Cyc_Absyn_Tvar _tmpC2;void*_tmpC3;void**_tmpC4;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBF;if(_tmpC0->tag != 2)goto _LL51;else{_tmpC1=_tmpC0->f1;_tmpC2=*_tmpC1;_tmpC3=_tmpC2.kind;_tmpC4=(void**)&(*_tmpC0->f1).kind;}}_LL50: {
# 474
void*_tmpC5=Cyc_Absyn_compress_kb(*_tmpC4);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC5;if(_tmpC6->tag != 1)goto _LL56;}_LL55:
# 476
 if(!leq)*_tmpC4=Cyc_Tcutil_kind_to_bound(k);else{
# 477
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8E0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8DF;*_tmpC4=(void*)((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E0.tag=2,((_tmp8E0.f1=0,((_tmp8E0.f2=k,_tmp8E0)))))),_tmp8DF))));}
# 478
return;_LL56:;_LL57:
# 479
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
void*_tmpC9=(void*)tms->hd;void*_tmpCB;_LL59: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpCA=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpC9;if(_tmpCA->tag != 3)goto _LL5B;else{_tmpCB=(void*)_tmpCA->f1;}}_LL5A:
# 502
 if(tms->tl == 0  || 
# 503
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 505
void*_tmpCC=_tmpCB;struct Cyc_List_List*_tmpCF;_LL5E: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpCD=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC;if(_tmpCD->tag != 1)goto _LL60;}_LL5F:
# 507
{const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,Cyc_Tcutil_warn(loc,((_tmp8E3="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8E3,sizeof(char),93))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}
# 509
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpCE=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpCC;if(_tmpCE->tag != 0)goto _LL5D;else{_tmpCF=_tmpCE->f1;}}_LL61:
# 511
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCF)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
# 512
const char*_tmp8E6;void*_tmp8E5;(_tmp8E5=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8E6="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8E6,sizeof(char),73))),_tag_dyneither(_tmp8E5,sizeof(void*),0)));}{
# 515
struct Cyc_List_List*rev_new_params=0;
# 516
for(0;_tmpCF != 0;_tmpCF=_tmpCF->tl){
# 517
struct Cyc_List_List*_tmpD4=tds;
# 518
for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){
# 519
struct Cyc_Absyn_Decl*_tmpD5=(struct Cyc_Absyn_Decl*)_tmpD4->hd;
# 520
void*_tmpD6=_tmpD5->r;struct Cyc_Absyn_Vardecl*_tmpD8;_LL63: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD7=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD6;if(_tmpD7->tag != 0)goto _LL65;else{_tmpD8=_tmpD7->f1;}}_LL64:
# 522
 if(Cyc_zstrptrcmp((*_tmpD8->name).f2,(struct _dyneither_ptr*)_tmpCF->hd)!= 0)
# 523
continue;
# 524
if(_tmpD8->initializer != 0){
# 525
const char*_tmp8E9;void*_tmp8E8;(_tmp8E8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD5->loc,((_tmp8E9="initializer found in parameter declaration",_tag_dyneither(_tmp8E9,sizeof(char),43))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}
# 526
if(Cyc_Absyn_is_qvar_qualified(_tmpD8->name)){
# 527
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD5->loc,((_tmp8EC="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8EC,sizeof(char),47))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
# 528
{struct _tuple8*_tmp8EF;struct Cyc_List_List*_tmp8EE;rev_new_params=(
# 529
(_tmp8EE=_cycalloc(sizeof(*_tmp8EE)),((_tmp8EE->hd=((_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF->f1=(struct _dyneither_ptr*)(*_tmpD8->name).f2,((_tmp8EF->f2=_tmpD8->tq,((_tmp8EF->f3=_tmpD8->type,_tmp8EF)))))))),((_tmp8EE->tl=rev_new_params,_tmp8EE))))));}
# 531
goto L;_LL65:;_LL66: {
# 532
const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD5->loc,((_tmp8F2="nonvariable declaration in parameter type",_tag_dyneither(_tmp8F2,sizeof(char),42))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}_LL62:;}
# 535
L: if(_tmpD4 == 0){
# 536
const char*_tmp8F6;void*_tmp8F5[1];struct Cyc_String_pa_PrintArg_struct _tmp8F4;(_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpCF->hd)),((_tmp8F5[0]=& _tmp8F4,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8F6="%s is not given a type",_tag_dyneither(_tmp8F6,sizeof(char),23))),_tag_dyneither(_tmp8F5,sizeof(void*),1)))))));}}{
# 538
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp905;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp904;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp903;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp902;struct Cyc_List_List*_tmp901;return
# 539
(_tmp901=_region_malloc(yy,sizeof(*_tmp901)),((_tmp901->hd=(void*)((_tmp905=_region_malloc(yy,sizeof(*_tmp905)),((_tmp905[0]=((_tmp902.tag=3,((_tmp902.f1=(void*)((void*)((_tmp904=_region_malloc(yy,sizeof(*_tmp904)),((_tmp904[0]=((_tmp903.tag=1,((_tmp903.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp903.f2=0,((_tmp903.f3=0,((_tmp903.f4=(void*)0,((_tmp903.f5=0,_tmp903)))))))))))),_tmp904))))),_tmp902)))),_tmp905)))),((_tmp901->tl=0,_tmp901)))));};};_LL5D:;}
# 545
goto _LL5C;_LL5B:;_LL5C: {
# 546
struct Cyc_List_List*_tmp906;return(_tmp906=_region_malloc(yy,sizeof(*_tmp906)),((_tmp906->hd=(void*)tms->hd,((_tmp906->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp906)))));}_LL58:;};}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 557
if(tds != 0){
# 558
struct Cyc_Parse_Declarator _tmp907;d=((_tmp907.id=d.id,((_tmp907.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp907))));}{
# 560
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
# 561
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
# 562
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
# 563
int is_inline=0;
# 564
struct Cyc_List_List*atts=0;
# 566
if(dso != 0){
# 567
tss=dso->type_specs;
# 568
tq=dso->tq;
# 569
is_inline=dso->is_inline;
# 570
atts=dso->attributes;
# 572
if(dso->sc != 0)
# 573
switch(*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc))){case Cyc_Parse_Extern_sc: _LL67:
# 574
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL68:
# 575
 sc=Cyc_Absyn_Static;break;default: _LL69:
# 576
{const char*_tmp908;Cyc_Parse_err(((_tmp908="bad storage class on function",_tag_dyneither(_tmp908,sizeof(char),30))),loc);}break;}}{
# 579
void*_tmpEC=Cyc_Parse_collapse_type_specifiers(tss,loc);
# 580
struct Cyc_Absyn_Tqual _tmpEE;void*_tmpEF;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF1;struct _tuple14 _tmpED=Cyc_Parse_apply_tms(tq,_tmpEC,atts,d.tms);_tmpEE=_tmpED.f1;_tmpEF=_tmpED.f2;_tmpF0=_tmpED.f3;_tmpF1=_tmpED.f4;
# 584
if(_tmpF0 != 0){
# 587
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_warn(loc,((_tmp90B="bad type params, ignoring",_tag_dyneither(_tmp90B,sizeof(char),26))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}{
# 589
void*_tmpF4=_tmpEF;struct Cyc_Absyn_FnInfo _tmpF6;struct Cyc_List_List*_tmpF7;void*_tmpF8;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpFA;struct Cyc_List_List*_tmpFB;int _tmpFC;struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFF;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF4;if(_tmpF5->tag != 9)goto _LL6E;else{_tmpF6=_tmpF5->f1;_tmpF7=_tmpF6.tvars;_tmpF8=_tmpF6.effect;_tmpF9=_tmpF6.ret_tqual;_tmpFA=_tmpF6.ret_typ;_tmpFB=_tmpF6.args;_tmpFC=_tmpF6.c_varargs;_tmpFD=_tmpF6.cyc_varargs;_tmpFE=_tmpF6.rgn_po;_tmpFF=_tmpF6.attributes;}}_LL6D: {
# 592
struct Cyc_List_List*_tmp100=0;
# 593
{struct Cyc_List_List*_tmp101=_tmpFB;for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
# 594
struct _tuple8 _tmp103;struct _dyneither_ptr*_tmp104;struct Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct _tuple8*_tmp102=(struct _tuple8*)_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;_tmp106=_tmp103.f3;
# 595
if(_tmp104 == 0){
# 596
{const char*_tmp90C;Cyc_Parse_err(((_tmp90C="missing argument variable in function prototype",_tag_dyneither(_tmp90C,sizeof(char),48))),loc);}{
# 597
struct _tuple8*_tmp90F;struct Cyc_List_List*_tmp90E;_tmp100=((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->hd=((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp90F->f2=_tmp105,((_tmp90F->f3=_tmp106,_tmp90F)))))))),((_tmp90E->tl=_tmp100,_tmp90E))))));};}else{
# 599
struct _tuple8*_tmp912;struct Cyc_List_List*_tmp911;_tmp100=((_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911->hd=((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912->f1=(struct _dyneither_ptr*)_tmp104,((_tmp912->f2=_tmp105,((_tmp912->f3=_tmp106,_tmp912)))))))),((_tmp911->tl=_tmp100,_tmp911))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp913;return(_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->sc=sc,((_tmp913->name=d.id,((_tmp913->tvs=_tmpF7,((_tmp913->is_inline=is_inline,((_tmp913->effect=_tmpF8,((_tmp913->ret_tqual=_tmpF9,((_tmp913->ret_type=_tmpFA,((_tmp913->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp100),((_tmp913->c_varargs=_tmpFC,((_tmp913->cyc_varargs=_tmpFD,((_tmp913->rgn_po=_tmpFE,((_tmp913->body=body,((_tmp913->cached_typ=0,((_tmp913->param_vardecls=0,((_tmp913->fn_vardecl=0,((_tmp913->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpFF,_tmpF1),_tmp913)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
# 614
const char*_tmp916;void*_tmp915;(_tmp915=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp916="declarator is not a function prototype",_tag_dyneither(_tmp916,sizeof(char),39))),_tag_dyneither(_tmp915,sizeof(void*),0)));}_LL6B:;};};};}static char _tmp110[52]="at most one type may appear within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static char _tmp111[63]="const or volatile may appear only once within a type specifier";
# 620
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static char _tmp112[50]="type specifier includes more than one declaration";
# 622
static struct _dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
# 624
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};
# 631
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 634
if(s1.Signed_spec  && s2.Signed_spec){
# 635
void*_tmp917;(_tmp917=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp917,sizeof(void*),0)));}
# 636
s1.Signed_spec |=s2.Signed_spec;
# 637
if(s1.Unsigned_spec  && s2.Unsigned_spec){
# 638
void*_tmp918;(_tmp918=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp918,sizeof(void*),0)));}
# 639
s1.Unsigned_spec |=s2.Unsigned_spec;
# 640
if(s1.Short_spec  && s2.Short_spec){
# 641
void*_tmp919;(_tmp919=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp919,sizeof(void*),0)));}
# 642
s1.Short_spec |=s2.Short_spec;
# 643
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
# 644
s1.Long_Long_spec  && s2.Long_spec) || 
# 645
s2.Long_Long_spec  && s1.Long_spec){
# 646
void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91A,sizeof(void*),0)));}
# 647
s1.Long_Long_spec=
# 648
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
# 649
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
# 650
if(s1.Valid_type_spec  && s2.Valid_type_spec)
# 651
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
# 652
if(s2.Valid_type_spec){
# 653
s1.Type_spec=s2.Type_spec;
# 654
s1.Valid_type_spec=1;}}
# 656
return s1;}
# 662
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 665
int seen_type=ts.Valid_type_spec;
# 666
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
# 667
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
# 668
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
# 669
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
# 670
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 672
if(ts.Signed_spec  && ts.Unsigned_spec)
# 673
Cyc_Parse_err(Cyc_Parse_msg4,loc);
# 674
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
# 675
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
# 676
ts.Long_spec  && ts.Long_Long_spec)
# 677
Cyc_Parse_err(Cyc_Parse_msg4,loc);
# 678
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
# 679
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
# 680
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 684
if(!seen_type){
# 685
if(!seen_sign  && !seen_size){
# 686
const char*_tmp91D;void*_tmp91C;(_tmp91C=0,Cyc_Tcutil_warn(loc,((_tmp91D="missing type within specifier",_tag_dyneither(_tmp91D,sizeof(char),30))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}
# 687
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 689
if(seen_sign){
# 690
void*_tmp11A=t;enum Cyc_Absyn_Sign _tmp11C;enum Cyc_Absyn_Size_of _tmp11D;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp11B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp11A;if(_tmp11B->tag != 6)goto _LL73;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL72:
# 692
 if(_tmp11C != sgn)
# 693
t=Cyc_Absyn_int_typ(sgn,_tmp11D);
# 694
goto _LL70;_LL73:;_LL74:
# 695
{const char*_tmp91E;Cyc_Parse_err(((_tmp91E="sign specification on non-integral type",_tag_dyneither(_tmp91E,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 697
if(seen_size){
# 698
void*_tmp11F=t;enum Cyc_Absyn_Sign _tmp121;enum Cyc_Absyn_Size_of _tmp122;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp120=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp11F;if(_tmp120->tag != 6)goto _LL78;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL77:
# 700
 if(_tmp122 != sz)
# 701
t=Cyc_Absyn_int_typ(_tmp121,sz);
# 702
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp123=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp11F;if(_tmp123->tag != 7)goto _LL7A;}_LL79:
# 704
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
# 705
{const char*_tmp91F;Cyc_Parse_err(((_tmp91F="size qualifier on non-integral type",_tag_dyneither(_tmp91F,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
# 708
return t;}
# 711
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 715
if(ds == 0)return 0;{
# 716
struct Cyc_Parse_Declarator d=ds->hd;
# 717
struct _tuple0*_tmp125=d.id;
# 718
struct Cyc_Absyn_Tqual _tmp127;void*_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_List_List*_tmp12A;struct _tuple14 _tmp126=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);_tmp127=_tmp126.f1;_tmp128=_tmp126.f2;_tmp129=_tmp126.f3;_tmp12A=_tmp126.f4;
# 721
if(ds->tl == 0){
# 722
struct _tuple16*_tmp922;struct Cyc_List_List*_tmp921;return(_tmp921=_region_malloc(r,sizeof(*_tmp921)),((_tmp921->hd=((_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922->f1=_tmp125,((_tmp922->f2=_tmp127,((_tmp922->f3=_tmp128,((_tmp922->f4=_tmp129,((_tmp922->f5=_tmp12A,_tmp922)))))))))))),((_tmp921->tl=0,_tmp921)))));}else{
# 724
struct _tuple16*_tmp925;struct Cyc_List_List*_tmp924;return(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->hd=((_tmp925=_region_malloc(r,sizeof(*_tmp925)),((_tmp925->f1=_tmp125,((_tmp925->f2=_tmp127,((_tmp925->f3=_tmp128,((_tmp925->f4=_tmp129,((_tmp925->f5=_tmp12A,_tmp925)))))))))))),((_tmp924->tl=
# 725
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp924)))));}};}
# 728
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 731
if(tms == 0){struct _tuple14 _tmp926;return(_tmp926.f1=tq,((_tmp926.f2=t,((_tmp926.f3=0,((_tmp926.f4=atts,_tmp926)))))));}{
# 732
void*_tmp130=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp132;unsigned int _tmp133;struct Cyc_Absyn_Exp*_tmp135;union Cyc_Absyn_Constraint*_tmp136;unsigned int _tmp137;void*_tmp139;struct Cyc_List_List*_tmp13B;unsigned int _tmp13C;struct Cyc_Absyn_PtrAtts _tmp13E;struct Cyc_Absyn_Tqual _tmp13F;unsigned int _tmp141;struct Cyc_List_List*_tmp142;_LL7D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp131=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp131->tag != 0)goto _LL7F;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;}}_LL7E:
# 734
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
# 735
Cyc_Absyn_array_typ(t,tq,0,_tmp132,_tmp133),atts,tms->tl);_LL7F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp134=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp134->tag != 1)goto _LL81;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;_tmp137=_tmp134->f3;}}_LL80:
# 737
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
# 738
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp135,_tmp136,_tmp137),atts,tms->tl);_LL81: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp138=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp138->tag != 3)goto _LL83;else{_tmp139=(void*)_tmp138->f1;}}_LL82: {
# 740
void*_tmp143=_tmp139;struct Cyc_List_List*_tmp145;int _tmp146;struct Cyc_Absyn_VarargInfo*_tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;unsigned int _tmp14B;_LL8A: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp144=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp143;if(_tmp144->tag != 1)goto _LL8C;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;_tmp147=_tmp144->f3;_tmp148=(void*)_tmp144->f4;_tmp149=_tmp144->f5;}}_LL8B: {
# 742
struct Cyc_List_List*typvars=0;
# 744
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
# 745
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
# 746
if(Cyc_Absyn_fntype_att((void*)as->hd)){
# 747
struct Cyc_List_List*_tmp927;fn_atts=((_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->hd=(void*)as->hd,((_tmp927->tl=fn_atts,_tmp927))))));}else{
# 749
struct Cyc_List_List*_tmp928;new_atts=((_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928->hd=(void*)as->hd,((_tmp928->tl=new_atts,_tmp928))))));}}}
# 752
if(tms->tl != 0){
# 753
void*_tmp14E=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp150;_LL8F: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp14F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp14E;if(_tmp14F->tag != 4)goto _LL91;else{_tmp150=_tmp14F->f1;}}_LL90:
# 755
 typvars=_tmp150;
# 756
tms=tms->tl;
# 757
goto _LL8E;_LL91:;_LL92:
# 759
 goto _LL8E;_LL8E:;}
# 763
if(((((!_tmp146  && _tmp147 == 0) && _tmp145 != 0) && _tmp145->tl == 0) && (*((struct _tuple8*)_tmp145->hd)).f1 == 0) && (*((struct _tuple8*)_tmp145->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 768
_tmp145=0;{
# 771
struct Cyc_List_List*_tmp151=Cyc_Parse_get_arg_tags(_tmp145);
# 773
if(_tmp151 != 0)
# 774
t=Cyc_Parse_substitute_tags(_tmp151,t);
# 775
t=Cyc_Parse_array2ptr(t,0);
# 778
{struct Cyc_List_List*_tmp152=_tmp145;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
# 779
struct _tuple8 _tmp154;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Tqual _tmp156;void*_tmp157;void**_tmp158;struct _tuple8*_tmp153=(struct _tuple8*)_tmp152->hd;_tmp154=*_tmp153;_tmp155=_tmp154.f1;_tmp156=_tmp154.f2;_tmp157=_tmp154.f3;_tmp158=(void**)&(*_tmp153).f3;
# 780
if(_tmp151 != 0)
# 781
*_tmp158=Cyc_Parse_substitute_tags(_tmp151,*_tmp158);
# 782
*_tmp158=Cyc_Parse_array2ptr(*_tmp158,1);}}
# 790
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
# 791
Cyc_Absyn_function_typ(typvars,_tmp148,tq,t,_tmp145,_tmp146,_tmp147,_tmp149,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL8C: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp14A=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp143;if(_tmp14A->tag != 0)goto _LL89;else{_tmp14B=_tmp14A->f2;}}_LL8D: {
# 796
const char*_tmp92B;void*_tmp92A;(_tmp92A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp14B,((_tmp92B="function declaration without parameter types",_tag_dyneither(_tmp92B,sizeof(char),45))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp13A=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp13A->tag != 4)goto _LL85;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;}}_LL84:
# 803
 if(tms->tl == 0){
# 804
struct _tuple14 _tmp92C;return(_tmp92C.f1=tq,((_tmp92C.f2=t,((_tmp92C.f3=_tmp13B,((_tmp92C.f4=atts,_tmp92C)))))));}{
# 808
const char*_tmp92F;void*_tmp92E;(_tmp92E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp13C,((_tmp92F="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp92F,sizeof(char),68))),_tag_dyneither(_tmp92E,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp13D=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp13D->tag != 2)goto _LL87;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL86: {
# 811
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp935;struct Cyc_Absyn_PtrInfo _tmp934;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp933;return Cyc_Parse_apply_tms(_tmp13F,(void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp935.tag=5,((_tmp935.f1=((_tmp934.elt_typ=t,((_tmp934.elt_tq=tq,((_tmp934.ptr_atts=_tmp13E,_tmp934)))))),_tmp935)))),_tmp933)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp140=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp130;if(_tmp140->tag != 5)goto _LL7C;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;}}_LL88:
# 816
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp142),tms->tl);_LL7C:;};}
# 822
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
# 823
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 831
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple16*t){
# 832
struct _tuple16 _tmp162;struct _tuple0*_tmp163;struct Cyc_Absyn_Tqual _tmp164;void*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_List_List*_tmp167;struct _tuple16*_tmp161=t;_tmp162=*_tmp161;_tmp163=_tmp162.f1;_tmp164=_tmp162.f2;_tmp165=_tmp162.f3;_tmp166=_tmp162.f4;_tmp167=_tmp162.f5;
# 834
Cyc_Lex_register_typedef(_tmp163);{
# 836
struct Cyc_Core_Opt*kind;
# 837
void*type;
# 838
{void*_tmp168=_tmp165;struct Cyc_Core_Opt*_tmp16A;_LL94: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp168;if(_tmp169->tag != 1)goto _LL96;else{_tmp16A=_tmp169->f1;}}_LL95:
# 840
 type=0;
# 841
if(_tmp16A == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;else{
# 842
kind=_tmp16A;}
# 843
goto _LL93;_LL96:;_LL97:
# 844
 kind=0;type=(void*)_tmp165;goto _LL93;_LL93:;}{
# 846
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp93B;struct Cyc_Absyn_Typedefdecl*_tmp93A;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp939;return Cyc_Absyn_new_decl((void*)((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[0]=((_tmp93B.tag=9,((_tmp93B.f1=((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->name=_tmp163,((_tmp93A->tvs=_tmp166,((_tmp93A->kind=kind,((_tmp93A->defn=type,((_tmp93A->atts=_tmp167,((_tmp93A->tq=_tmp164,_tmp93A)))))))))))))),_tmp93B)))),_tmp939)))),loc);};};}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
# 854
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp93E;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp93D;return Cyc_Absyn_new_stmt((void*)((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D[0]=((_tmp93E.tag=12,((_tmp93E.f1=d,((_tmp93E.f2=s,_tmp93E)))))),_tmp93D)))),d->loc);}
# 859
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
# 860
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 868
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned int tqual_loc,unsigned int loc){
# 872
struct _RegionHandle _tmp170=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp170;_push_region(mkrgn);
# 873
{struct Cyc_Absyn_Tqual _tmp172;struct Cyc_Parse_Type_specifier _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Parse_Declaration_spec _tmp171=ds;_tmp172=_tmp171.tq;_tmp173=_tmp171.type_specs;_tmp174=_tmp171.attributes;
# 874
if(_tmp172.loc == 0)_tmp172.loc=tqual_loc;
# 875
if(ds.is_inline){
# 876
const char*_tmp93F;Cyc_Parse_err(((_tmp93F="inline is allowed only on function definitions",_tag_dyneither(_tmp93F,sizeof(char),47))),loc);}{
# 878
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
# 879
int istypedef=0;
# 880
if(ds.sc != 0)
# 881
switch(*ds.sc){case Cyc_Parse_Typedef_sc: _LL98:
# 882
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL99:
# 883
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL9A:
# 884
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL9B:
# 885
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL9C:
# 886
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL9D:
# 887
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL9E:
# 888
 s=Cyc_Absyn_Abstract;break;}{
# 894
struct _tuple11*declarators=0;
# 895
struct Cyc_List_List*exprs=0;
# 896
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 898
int exps_empty=1;
# 899
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
# 900
if((struct Cyc_Absyn_Exp*)es->hd != 0){
# 901
exps_empty=0;
# 902
break;}}}{
# 906
void*_tmp176=Cyc_Parse_collapse_type_specifiers(_tmp173,loc);
# 907
if(declarators == 0){
# 910
void*_tmp177=_tmp176;struct Cyc_Absyn_TypeDecl*_tmp179;struct Cyc_Absyn_TypeDecl _tmp17A;void*_tmp17B;struct Cyc_Absyn_Aggrdecl*_tmp17D;struct Cyc_Absyn_TypeDecl*_tmp17F;struct Cyc_Absyn_TypeDecl _tmp180;void*_tmp181;struct Cyc_Absyn_Enumdecl*_tmp183;struct Cyc_Absyn_TypeDecl*_tmp185;struct Cyc_Absyn_TypeDecl _tmp186;void*_tmp187;struct Cyc_Absyn_Datatypedecl*_tmp189;struct Cyc_Absyn_AggrInfo _tmp18B;union Cyc_Absyn_AggrInfoU _tmp18C;struct _tuple2 _tmp18D;enum Cyc_Absyn_AggrKind _tmp18E;struct _tuple0*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_DatatypeInfo _tmp192;union Cyc_Absyn_DatatypeInfoU _tmp193;struct Cyc_Absyn_Datatypedecl**_tmp194;struct Cyc_Absyn_DatatypeInfo _tmp196;union Cyc_Absyn_DatatypeInfoU _tmp197;struct Cyc_Absyn_UnknownDatatypeInfo _tmp198;struct _tuple0*_tmp199;int _tmp19A;struct Cyc_List_List*_tmp19B;struct _tuple0*_tmp19D;struct Cyc_List_List*_tmp19F;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp177;if(_tmp178->tag != 26)goto _LLA3;else{_tmp179=_tmp178->f1;_tmp17A=*_tmp179;_tmp17B=_tmp17A.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp17C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp17B;if(_tmp17C->tag != 0)goto _LLA3;else{_tmp17D=_tmp17C->f1;}};}}_LLA2:
# 912
 _tmp17D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp17D->attributes,_tmp174);{
# 913
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp945;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp944;struct Cyc_List_List*_tmp943;struct Cyc_List_List*_tmp1A3=(_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943->hd=Cyc_Absyn_new_decl((void*)((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945[0]=((_tmp944.tag=6,((_tmp944.f1=_tmp17D,_tmp944)))),_tmp945)))),loc),((_tmp943->tl=0,_tmp943)))));_npop_handler(0);return _tmp1A3;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp177;if(_tmp17E->tag != 26)goto _LLA5;else{_tmp17F=_tmp17E->f1;_tmp180=*_tmp17F;_tmp181=_tmp180.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp182=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp181;if(_tmp182->tag != 1)goto _LLA5;else{_tmp183=_tmp182->f1;}};}}_LLA4:
# 915
 if(_tmp174 != 0){const char*_tmp946;Cyc_Parse_err(((_tmp946="attributes on enum not supported",_tag_dyneither(_tmp946,sizeof(char),33))),loc);}{
# 916
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94C;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp94B;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp1A8=(_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->hd=Cyc_Absyn_new_decl((void*)((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94B.tag=8,((_tmp94B.f1=_tmp183,_tmp94B)))),_tmp94C)))),loc),((_tmp94A->tl=0,_tmp94A)))));_npop_handler(0);return _tmp1A8;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp177;if(_tmp184->tag != 26)goto _LLA7;else{_tmp185=_tmp184->f1;_tmp186=*_tmp185;_tmp187=_tmp186.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp188=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp187;if(_tmp188->tag != 2)goto _LLA7;else{_tmp189=_tmp188->f1;}};}}_LLA6:
# 918
 if(_tmp174 != 0){const char*_tmp94D;Cyc_Parse_err(((_tmp94D="attributes on datatypes not supported",_tag_dyneither(_tmp94D,sizeof(char),38))),loc);}{
# 919
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp953;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp952;struct Cyc_List_List*_tmp951;struct Cyc_List_List*_tmp1AD=(_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->hd=Cyc_Absyn_new_decl((void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp952.tag=7,((_tmp952.f1=_tmp189,_tmp952)))),_tmp953)))),loc),((_tmp951->tl=0,_tmp951)))));_npop_handler(0);return _tmp1AD;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp177;if(_tmp18A->tag != 11)goto _LLA9;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18B.aggr_info;if((_tmp18C.UnknownAggr).tag != 1)goto _LLA9;_tmp18D=(struct _tuple2)(_tmp18C.UnknownAggr).val;_tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;_tmp190=_tmp18B.targs;}}_LLA8: {
# 921
struct Cyc_List_List*_tmp1AE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp190);
# 922
struct Cyc_Absyn_Aggrdecl*_tmp954;struct Cyc_Absyn_Aggrdecl*_tmp1AF=(_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954->kind=_tmp18E,((_tmp954->sc=s,((_tmp954->name=_tmp18F,((_tmp954->tvs=_tmp1AE,((_tmp954->impl=0,((_tmp954->attributes=0,_tmp954)))))))))))));
# 923
if(_tmp174 != 0){const char*_tmp955;Cyc_Parse_err(((_tmp955="bad attributes on type declaration",_tag_dyneither(_tmp955,sizeof(char),35))),loc);}{
# 924
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp95B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp95A;struct Cyc_List_List*_tmp959;struct Cyc_List_List*_tmp1B4=(_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959->hd=Cyc_Absyn_new_decl((void*)((_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95A.tag=6,((_tmp95A.f1=_tmp1AF,_tmp95A)))),_tmp95B)))),loc),((_tmp959->tl=0,_tmp959)))));_npop_handler(0);return _tmp1B4;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp177;if(_tmp191->tag != 3)goto _LLAB;else{_tmp192=_tmp191->f1;_tmp193=_tmp192.datatype_info;if((_tmp193.KnownDatatype).tag != 2)goto _LLAB;_tmp194=(struct Cyc_Absyn_Datatypedecl**)(_tmp193.KnownDatatype).val;}}_LLAA:
# 926
 if(_tmp174 != 0){const char*_tmp95C;Cyc_Parse_err(((_tmp95C="bad attributes on datatype",_tag_dyneither(_tmp95C,sizeof(char),27))),loc);}{
# 927
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp962;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp961;struct Cyc_List_List*_tmp960;struct Cyc_List_List*_tmp1BA=(_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960->hd=Cyc_Absyn_new_decl((void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((_tmp961.tag=7,((_tmp961.f1=*_tmp194,_tmp961)))),_tmp962)))),loc),((_tmp960->tl=0,_tmp960)))));_npop_handler(0);return _tmp1BA;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp195=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp177;if(_tmp195->tag != 3)goto _LLAD;else{_tmp196=_tmp195->f1;_tmp197=_tmp196.datatype_info;if((_tmp197.UnknownDatatype).tag != 1)goto _LLAD;_tmp198=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp197.UnknownDatatype).val;_tmp199=_tmp198.name;_tmp19A=_tmp198.is_extensible;_tmp19B=_tmp196.targs;}}_LLAC: {
# 929
struct Cyc_List_List*_tmp1BB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp19B);
# 930
struct Cyc_Absyn_Decl*_tmp1BC=Cyc_Absyn_datatype_decl(s,_tmp199,_tmp1BB,0,_tmp19A,loc);
# 931
if(_tmp174 != 0){const char*_tmp963;Cyc_Parse_err(((_tmp963="bad attributes on datatype",_tag_dyneither(_tmp963,sizeof(char),27))),loc);}{
# 932
struct Cyc_List_List*_tmp964;struct Cyc_List_List*_tmp1BF=(_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->hd=_tmp1BC,((_tmp964->tl=0,_tmp964)))));_npop_handler(0);return _tmp1BF;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp177;if(_tmp19C->tag != 13)goto _LLAF;else{_tmp19D=_tmp19C->f1;}}_LLAE: {
# 934
struct Cyc_Absyn_Enumdecl*_tmp965;struct Cyc_Absyn_Enumdecl*_tmp1C0=(_tmp965=_cycalloc(sizeof(*_tmp965)),((_tmp965->sc=s,((_tmp965->name=_tmp19D,((_tmp965->fields=0,_tmp965)))))));
# 935
if(_tmp174 != 0){const char*_tmp966;Cyc_Parse_err(((_tmp966="bad attributes on enum",_tag_dyneither(_tmp966,sizeof(char),23))),loc);}{
# 936
struct Cyc_Absyn_Decl*_tmp970;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp96F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp96E;struct Cyc_List_List*_tmp96D;struct Cyc_List_List*_tmp1C6=(_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D->hd=((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970->r=(void*)((_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=8,((_tmp96F.f1=_tmp1C0,_tmp96F)))),_tmp96E)))),((_tmp970->loc=loc,_tmp970)))))),((_tmp96D->tl=0,_tmp96D)))));_npop_handler(0);return _tmp1C6;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp177;if(_tmp19E->tag != 14)goto _LLB1;else{_tmp19F=_tmp19E->f1;}}_LLB0: {
# 940
struct Cyc_Core_Opt*_tmp973;struct Cyc_Absyn_Enumdecl*_tmp972;struct Cyc_Absyn_Enumdecl*_tmp1C8=(_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972->sc=s,((_tmp972->name=Cyc_Parse_gensym_enum(),((_tmp972->fields=((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973->v=_tmp19F,_tmp973)))),_tmp972)))))));
# 941
if(_tmp174 != 0){const char*_tmp974;Cyc_Parse_err(((_tmp974="bad attributes on enum",_tag_dyneither(_tmp974,sizeof(char),23))),loc);}{
# 942
struct Cyc_Absyn_Decl*_tmp97E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp97D;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp97C;struct Cyc_List_List*_tmp97B;struct Cyc_List_List*_tmp1CE=(_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B->hd=((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E->r=(void*)((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=8,((_tmp97D.f1=_tmp1C8,_tmp97D)))),_tmp97C)))),((_tmp97E->loc=loc,_tmp97E)))))),((_tmp97B->tl=0,_tmp97B)))));_npop_handler(0);return _tmp1CE;};}_LLB1:;_LLB2:
# 943
{const char*_tmp97F;Cyc_Parse_err(((_tmp97F="missing declarator",_tag_dyneither(_tmp97F,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1D2=0;_npop_handler(0);return _tmp1D2;};_LLA0:;}else{
# 947
struct Cyc_List_List*_tmp1D3=Cyc_Parse_apply_tmss(mkrgn,_tmp172,_tmp176,declarators,_tmp174);
# 948
if(istypedef){
# 952
if(!exps_empty){
# 953
const char*_tmp980;Cyc_Parse_err(((_tmp980="initializer in typedef declaration",_tag_dyneither(_tmp980,sizeof(char),35))),loc);}{
# 954
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple16*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1D3);
# 955
struct Cyc_List_List*_tmp1D5=decls;_npop_handler(0);return _tmp1D5;};}else{
# 958
struct Cyc_List_List*decls=0;
# 959
{struct Cyc_List_List*_tmp1D6=_tmp1D3;for(0;_tmp1D6 != 0;(_tmp1D6=_tmp1D6->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
# 960
struct _tuple16 _tmp1D8;struct _tuple0*_tmp1D9;struct Cyc_Absyn_Tqual _tmp1DA;void*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DD;struct _tuple16*_tmp1D7=(struct _tuple16*)_tmp1D6->hd;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;_tmp1DB=_tmp1D8.f3;_tmp1DC=_tmp1D8.f4;_tmp1DD=_tmp1D8.f5;
# 961
if(_tmp1DC != 0){
# 962
const char*_tmp983;void*_tmp982;(_tmp982=0,Cyc_Tcutil_warn(loc,((_tmp983="bad type params, ignoring",_tag_dyneither(_tmp983,sizeof(char),26))),_tag_dyneither(_tmp982,sizeof(void*),0)));}
# 963
if(exprs == 0){
# 964
const char*_tmp986;void*_tmp985;(_tmp985=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp986="unexpected NULL in parse!",_tag_dyneither(_tmp986,sizeof(char),26))),_tag_dyneither(_tmp985,sizeof(void*),0)));}{
# 965
struct Cyc_Absyn_Exp*_tmp1E2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
# 966
struct Cyc_Absyn_Vardecl*_tmp1E3=Cyc_Absyn_new_vardecl(_tmp1D9,_tmp1DB,_tmp1E2);
# 967
_tmp1E3->tq=_tmp1DA;
# 968
_tmp1E3->sc=s;
# 969
_tmp1E3->attributes=_tmp1DD;{
# 970
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp98C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp98B;struct Cyc_Absyn_Decl*_tmp98A;struct Cyc_Absyn_Decl*_tmp1E4=(_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->r=(void*)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98B.tag=0,((_tmp98B.f1=_tmp1E3,_tmp98B)))),_tmp98C)))),((_tmp98A->loc=loc,_tmp98A)))));
# 971
struct Cyc_List_List*_tmp98D;decls=((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->hd=_tmp1E4,((_tmp98D->tl=decls,_tmp98D))))));};};}}{
# 973
struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1E9;};}}};};};};}
# 873
;_pop_region(mkrgn);}
# 980
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
# 981
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)
# 982
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB3:
# 983
 return& Cyc_Tcutil_ak;case 'M': _LLB4:
# 984
 return& Cyc_Tcutil_mk;case 'B': _LLB5:
# 985
 return& Cyc_Tcutil_bk;case 'R': _LLB6:
# 986
 return& Cyc_Tcutil_rk;case 'E': _LLB7:
# 987
 return& Cyc_Tcutil_ek;case 'I': _LLB8:
# 988
 return& Cyc_Tcutil_ik;case 'U': _LLB9:
# 990
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLBB:
# 991
 return& Cyc_Tcutil_urk;case 'A': _LLBC:
# 992
 return& Cyc_Tcutil_uak;case 'M': _LLBD:
# 993
 return& Cyc_Tcutil_umk;case 'B': _LLBE:
# 994
 return& Cyc_Tcutil_ubk;default: _LLBF:
# 995
 break;}
# 997
break;case 'T': _LLBA:
# 999
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLC2:
# 1000
 return& Cyc_Tcutil_trk;case 'A': _LLC3:
# 1001
 return& Cyc_Tcutil_tak;case 'M': _LLC4:
# 1002
 return& Cyc_Tcutil_tmk;case 'B': _LLC5:
# 1003
 return& Cyc_Tcutil_tbk;default: _LLC6:
# 1004
 break;}
# 1006
break;default: _LLC1:
# 1007
 break;}
# 1009
{const char*_tmp992;void*_tmp991[2];struct Cyc_String_pa_PrintArg_struct _tmp990;struct Cyc_Int_pa_PrintArg_struct _tmp98F;Cyc_Parse_err((struct _dyneither_ptr)((_tmp98F.tag=1,((_tmp98F.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp990.tag=0,((_tmp990.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp991[0]=& _tmp990,((_tmp991[1]=& _tmp98F,Cyc_aprintf(((_tmp992="bad kind: %s; strlen=%d",_tag_dyneither(_tmp992,sizeof(char),24))),_tag_dyneither(_tmp991,sizeof(void*),2)))))))))))))),loc);}
# 1010
return& Cyc_Tcutil_bk;}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
# 1015
void*_tmp1EE=p->r;struct _tuple0*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_Absyn_Pat*_tmp1F3;struct Cyc_Absyn_Pat _tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Pat*_tmp1F8;enum Cyc_Absyn_Sign _tmp1FB;int _tmp1FC;char _tmp1FE;struct _dyneither_ptr _tmp200;int _tmp201;struct _tuple0*_tmp203;struct Cyc_List_List*_tmp204;int _tmp205;struct Cyc_Absyn_Exp*_tmp207;_LLCA: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1EF=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1EF->tag != 14)goto _LLCC;else{_tmp1F0=_tmp1EF->f1;}}_LLCB:
# 1016
 return Cyc_Absyn_unknownid_exp(_tmp1F0,p->loc);_LLCC: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1F1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1F1->tag != 2)goto _LLCE;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1F6=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1F5;if(_tmp1F6->tag != 0)goto _LLCE;};}}_LLCD:
# 1018
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1F2->name,p->loc),p->loc);_LLCE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1F7=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1F7->tag != 5)goto _LLD0;else{_tmp1F8=_tmp1F7->f1;}}_LLCF:
# 1019
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1F8),p->loc);_LLD0: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1F9=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1F9->tag != 8)goto _LLD2;}_LLD1:
# 1020
 return Cyc_Absyn_null_exp(p->loc);_LLD2: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1FA=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1FA->tag != 9)goto _LLD4;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;}}_LLD3:
# 1021
 return Cyc_Absyn_int_exp(_tmp1FB,_tmp1FC,p->loc);_LLD4: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1FD=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1FD->tag != 10)goto _LLD6;else{_tmp1FE=_tmp1FD->f1;}}_LLD5:
# 1022
 return Cyc_Absyn_char_exp(_tmp1FE,p->loc);_LLD6: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1FF=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp1FF->tag != 11)goto _LLD8;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;}}_LLD7:
# 1023
 return Cyc_Absyn_float_exp(_tmp200,_tmp201,p->loc);_LLD8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp202=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp202->tag != 15)goto _LLDA;else{_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;_tmp205=_tmp202->f3;if(_tmp205 != 0)goto _LLDA;}}_LLD9: {
# 1025
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp203,p->loc);
# 1026
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp204);
# 1027
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLDA: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp206=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1EE;if(_tmp206->tag != 16)goto _LLDC;else{_tmp207=_tmp206->f1;}}_LLDB:
# 1028
 return _tmp207;_LLDC:;_LLDD:
# 1030
{const char*_tmp993;Cyc_Parse_err(((_tmp993="cannot mix patterns and expressions in case",_tag_dyneither(_tmp993,sizeof(char),44))),p->loc);}
# 1031
return Cyc_Absyn_null_exp(p->loc);_LLC9:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple24 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp20A[14]="$(sign_t,int)";
# 1082 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 14}};
# 1084
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
# 1085
struct _tuple5 yyzzz;
# 1086
{union Cyc_YYSTYPE _tmp20B=yy1;struct _tuple5 _tmp20C;_LLDF: if((_tmp20B.Int_tok).tag != 1)goto _LLE1;_tmp20C=(struct _tuple5)(_tmp20B.Int_tok).val;_LLE0:
# 1088
 yyzzz=_tmp20C;
# 1089
goto _LLDE;_LLE1:;_LLE2:
# 1090
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLDE:;}
# 1092
return yyzzz;}
# 1094
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.Int_tok).val=yy1,(((_tmp994.Int_tok).tag=1,_tmp994)));}static char _tmp20F[5]="char";
# 1083 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp20F,_tmp20F,_tmp20F + 5}};
# 1085
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
# 1086
char yyzzz;
# 1087
{union Cyc_YYSTYPE _tmp210=yy1;char _tmp211;_LLE4: if((_tmp210.Char_tok).tag != 2)goto _LLE6;_tmp211=(char)(_tmp210.Char_tok).val;_LLE5:
# 1089
 yyzzz=_tmp211;
# 1090
goto _LLE3;_LLE6:;_LLE7:
# 1091
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLE3:;}
# 1093
return yyzzz;}
# 1095
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.Char_tok).val=yy1,(((_tmp995.Char_tok).tag=2,_tmp995)));}static char _tmp214[13]="string_t<`H>";
# 1084 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp214,_tmp214,_tmp214 + 13}};
# 1086
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
# 1087
struct _dyneither_ptr yyzzz;
# 1088
{union Cyc_YYSTYPE _tmp215=yy1;struct _dyneither_ptr _tmp216;_LLE9: if((_tmp215.String_tok).tag != 3)goto _LLEB;_tmp216=(struct _dyneither_ptr)(_tmp215.String_tok).val;_LLEA:
# 1090
 yyzzz=_tmp216;
# 1091
goto _LLE8;_LLEB:;_LLEC:
# 1092
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE8:;}
# 1094
return yyzzz;}
# 1096
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.String_tok).val=yy1,(((_tmp996.String_tok).tag=3,_tmp996)));}static char _tmp219[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 56}};
# 1089
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
# 1090
struct _tuple21*yyzzz;
# 1091
{union Cyc_YYSTYPE _tmp21A=yy1;struct _tuple21*_tmp21B;_LLEE: if((_tmp21A.YY1).tag != 7)goto _LLF0;_tmp21B=(struct _tuple21*)(_tmp21A.YY1).val;_LLEF:
# 1093
 yyzzz=_tmp21B;
# 1094
goto _LLED;_LLF0:;_LLF1:
# 1095
(int)_throw((void*)& Cyc_yyfail_YY1);_LLED:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY1).val=yy1,(((_tmp997.YY1).tag=7,_tmp997)));}static char _tmp21E[19]="conref_t<bounds_t>";
# 1088 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp21E,_tmp21E,_tmp21E + 19}};
# 1090
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
# 1091
union Cyc_Absyn_Constraint*yyzzz;
# 1092
{union Cyc_YYSTYPE _tmp21F=yy1;union Cyc_Absyn_Constraint*_tmp220;_LLF3: if((_tmp21F.YY2).tag != 8)goto _LLF5;_tmp220=(union Cyc_Absyn_Constraint*)(_tmp21F.YY2).val;_LLF4:
# 1094
 yyzzz=_tmp220;
# 1095
goto _LLF2;_LLF5:;_LLF6:
# 1096
(int)_throw((void*)& Cyc_yyfail_YY2);_LLF2:;}
# 1098
return yyzzz;}
# 1100
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY2).val=yy1,(((_tmp998.YY2).tag=8,_tmp998)));}static char _tmp223[6]="exp_t";
# 1089 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 6}};
# 1091
struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
# 1092
struct Cyc_Absyn_Exp*yyzzz;
# 1093
{union Cyc_YYSTYPE _tmp224=yy1;struct Cyc_Absyn_Exp*_tmp225;_LLF8: if((_tmp224.YY3).tag != 9)goto _LLFA;_tmp225=(struct Cyc_Absyn_Exp*)(_tmp224.YY3).val;_LLF9:
# 1095
 yyzzz=_tmp225;
# 1096
goto _LLF7;_LLFA:;_LLFB:
# 1097
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF7:;}
# 1099
return yyzzz;}
# 1101
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY3).val=yy1,(((_tmp999.YY3).tag=9,_tmp999)));}static char _tmp228[10]="exp_opt_t";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 10}};
# 1099
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
# 1100
struct Cyc_Absyn_Exp*yyzzz;
# 1101
{union Cyc_YYSTYPE _tmp229=yy1;struct Cyc_Absyn_Exp*_tmp22A;_LLFD: if((_tmp229.YY4).tag != 10)goto _LLFF;_tmp22A=(struct Cyc_Absyn_Exp*)(_tmp229.YY4).val;_LLFE:
# 1103
 yyzzz=_tmp22A;
# 1104
goto _LLFC;_LLFF:;_LL100:
# 1105
(int)_throw((void*)& Cyc_yyfail_YY4);_LLFC:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY4).val=yy1,(((_tmp99A.YY4).tag=10,_tmp99A)));}static char _tmp22D[17]="list_t<exp_t,`H>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp22D,_tmp22D,_tmp22D + 17}};
# 1100
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
# 1101
struct Cyc_List_List*yyzzz;
# 1102
{union Cyc_YYSTYPE _tmp22E=yy1;struct Cyc_List_List*_tmp22F;_LL102: if((_tmp22E.YY5).tag != 11)goto _LL104;_tmp22F=(struct Cyc_List_List*)(_tmp22E.YY5).val;_LL103:
# 1104
 yyzzz=_tmp22F;
# 1105
goto _LL101;_LL104:;_LL105:
# 1106
(int)_throw((void*)& Cyc_yyfail_YY5);_LL101:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY5).val=yy1,(((_tmp99B.YY5).tag=11,_tmp99B)));}static char _tmp232[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 47}};
# 1101
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
# 1102
struct Cyc_List_List*yyzzz;
# 1103
{union Cyc_YYSTYPE _tmp233=yy1;struct Cyc_List_List*_tmp234;_LL107: if((_tmp233.YY6).tag != 12)goto _LL109;_tmp234=(struct Cyc_List_List*)(_tmp233.YY6).val;_LL108:
# 1105
 yyzzz=_tmp234;
# 1106
goto _LL106;_LL109:;_LL10A:
# 1107
(int)_throw((void*)& Cyc_yyfail_YY6);_LL106:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY6).val=yy1,(((_tmp99C.YY6).tag=12,_tmp99C)));}static char _tmp237[9]="primop_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 9}};
# 1102
enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
# 1103
enum Cyc_Absyn_Primop yyzzz;
# 1104
{union Cyc_YYSTYPE _tmp238=yy1;enum Cyc_Absyn_Primop _tmp239;_LL10C: if((_tmp238.YY7).tag != 13)goto _LL10E;_tmp239=(enum Cyc_Absyn_Primop)(_tmp238.YY7).val;_LL10D:
# 1106
 yyzzz=_tmp239;
# 1107
goto _LL10B;_LL10E:;_LL10F:
# 1108
(int)_throw((void*)& Cyc_yyfail_YY7);_LL10B:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY7).val=yy1,(((_tmp99D.YY7).tag=13,_tmp99D)));}static char _tmp23C[19]="opt_t<primop_t,`H>";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp23C,_tmp23C,_tmp23C + 19}};
# 1103
struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
# 1104
struct Cyc_Core_Opt*yyzzz;
# 1105
{union Cyc_YYSTYPE _tmp23D=yy1;struct Cyc_Core_Opt*_tmp23E;_LL111: if((_tmp23D.YY8).tag != 14)goto _LL113;_tmp23E=(struct Cyc_Core_Opt*)(_tmp23D.YY8).val;_LL112:
# 1107
 yyzzz=_tmp23E;
# 1108
goto _LL110;_LL113:;_LL114:
# 1109
(int)_throw((void*)& Cyc_yyfail_YY8);_LL110:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY8).val=yy1,(((_tmp99E.YY8).tag=14,_tmp99E)));}static char _tmp241[7]="qvar_t";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp241,_tmp241,_tmp241 + 7}};
# 1104
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
# 1105
struct _tuple0*yyzzz;
# 1106
{union Cyc_YYSTYPE _tmp242=yy1;struct _tuple0*_tmp243;_LL116: if((_tmp242.QualId_tok).tag != 5)goto _LL118;_tmp243=(struct _tuple0*)(_tmp242.QualId_tok).val;_LL117:
# 1108
 yyzzz=_tmp243;
# 1109
goto _LL115;_LL118:;_LL119:
# 1110
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.QualId_tok).val=yy1,(((_tmp99F.QualId_tok).tag=5,_tmp99F)));}static char _tmp246[7]="stmt_t";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp246,_tmp246,_tmp246 + 7}};
# 1107
struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
# 1108
struct Cyc_Absyn_Stmt*yyzzz;
# 1109
{union Cyc_YYSTYPE _tmp247=yy1;struct Cyc_Absyn_Stmt*_tmp248;_LL11B: if((_tmp247.YY9).tag != 15)goto _LL11D;_tmp248=(struct Cyc_Absyn_Stmt*)(_tmp247.YY9).val;_LL11C:
# 1111
 yyzzz=_tmp248;
# 1112
goto _LL11A;_LL11D:;_LL11E:
# 1113
(int)_throw((void*)& Cyc_yyfail_YY9);_LL11A:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY9).val=yy1,(((_tmp9A0.YY9).tag=15,_tmp9A0)));}static char _tmp24B[27]="list_t<switch_clause_t,`H>";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp24B,_tmp24B,_tmp24B + 27}};
# 1111
struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
# 1112
struct Cyc_List_List*yyzzz;
# 1113
{union Cyc_YYSTYPE _tmp24C=yy1;struct Cyc_List_List*_tmp24D;_LL120: if((_tmp24C.YY10).tag != 16)goto _LL122;_tmp24D=(struct Cyc_List_List*)(_tmp24C.YY10).val;_LL121:
# 1115
 yyzzz=_tmp24D;
# 1116
goto _LL11F;_LL122:;_LL123:
# 1117
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11F:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY10).val=yy1,(((_tmp9A1.YY10).tag=16,_tmp9A1)));}static char _tmp250[6]="pat_t";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 6}};
# 1112
struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
# 1113
struct Cyc_Absyn_Pat*yyzzz;
# 1114
{union Cyc_YYSTYPE _tmp251=yy1;struct Cyc_Absyn_Pat*_tmp252;_LL125: if((_tmp251.YY11).tag != 17)goto _LL127;_tmp252=(struct Cyc_Absyn_Pat*)(_tmp251.YY11).val;_LL126:
# 1116
 yyzzz=_tmp252;
# 1117
goto _LL124;_LL127:;_LL128:
# 1118
(int)_throw((void*)& Cyc_yyfail_YY11);_LL124:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY11).val=yy1,(((_tmp9A2.YY11).tag=17,_tmp9A2)));}static char _tmp255[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp255,_tmp255,_tmp255 + 28}};
# 1117
struct _tuple22*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
# 1118
struct _tuple22*yyzzz;
# 1119
{union Cyc_YYSTYPE _tmp256=yy1;struct _tuple22*_tmp257;_LL12A: if((_tmp256.YY12).tag != 18)goto _LL12C;_tmp257=(struct _tuple22*)(_tmp256.YY12).val;_LL12B:
# 1121
 yyzzz=_tmp257;
# 1122
goto _LL129;_LL12C:;_LL12D:
# 1123
(int)_throw((void*)& Cyc_yyfail_YY12);_LL129:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY12(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY12).val=yy1,(((_tmp9A3.YY12).tag=18,_tmp9A3)));}static char _tmp25A[17]="list_t<pat_t,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp25A,_tmp25A,_tmp25A + 17}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
# 1119
struct Cyc_List_List*yyzzz;
# 1120
{union Cyc_YYSTYPE _tmp25B=yy1;struct Cyc_List_List*_tmp25C;_LL12F: if((_tmp25B.YY13).tag != 19)goto _LL131;_tmp25C=(struct Cyc_List_List*)(_tmp25B.YY13).val;_LL130:
# 1122
 yyzzz=_tmp25C;
# 1123
goto _LL12E;_LL131:;_LL132:
# 1124
(int)_throw((void*)& Cyc_yyfail_YY13);_LL12E:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.YY13).val=yy1,(((_tmp9A4.YY13).tag=19,_tmp9A4)));}static char _tmp25F[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp25F,_tmp25F,_tmp25F + 36}};
# 1119
struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
# 1120
struct _tuple23*yyzzz;
# 1121
{union Cyc_YYSTYPE _tmp260=yy1;struct _tuple23*_tmp261;_LL134: if((_tmp260.YY14).tag != 20)goto _LL136;_tmp261=(struct _tuple23*)(_tmp260.YY14).val;_LL135:
# 1123
 yyzzz=_tmp261;
# 1124
goto _LL133;_LL136:;_LL137:
# 1125
(int)_throw((void*)& Cyc_yyfail_YY14);_LL133:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY14).val=yy1,(((_tmp9A5.YY14).tag=20,_tmp9A5)));}static char _tmp264[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp264,_tmp264,_tmp264 + 47}};
# 1120
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
# 1121
struct Cyc_List_List*yyzzz;
# 1122
{union Cyc_YYSTYPE _tmp265=yy1;struct Cyc_List_List*_tmp266;_LL139: if((_tmp265.YY15).tag != 21)goto _LL13B;_tmp266=(struct Cyc_List_List*)(_tmp265.YY15).val;_LL13A:
# 1124
 yyzzz=_tmp266;
# 1125
goto _LL138;_LL13B:;_LL13C:
# 1126
(int)_throw((void*)& Cyc_yyfail_YY15);_LL138:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY15).val=yy1,(((_tmp9A6.YY15).tag=21,_tmp9A6)));}static char _tmp269[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp269,_tmp269,_tmp269 + 58}};
# 1121
struct _tuple22*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
# 1122
struct _tuple22*yyzzz;
# 1123
{union Cyc_YYSTYPE _tmp26A=yy1;struct _tuple22*_tmp26B;_LL13E: if((_tmp26A.YY16).tag != 22)goto _LL140;_tmp26B=(struct _tuple22*)(_tmp26A.YY16).val;_LL13F:
# 1125
 yyzzz=_tmp26B;
# 1126
goto _LL13D;_LL140:;_LL141:
# 1127
(int)_throw((void*)& Cyc_yyfail_YY16);_LL13D:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY16(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.YY16).val=yy1,(((_tmp9A7.YY16).tag=22,_tmp9A7)));}static char _tmp26E[9]="fndecl_t";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp26E,_tmp26E,_tmp26E + 9}};
# 1122
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
# 1123
struct Cyc_Absyn_Fndecl*yyzzz;
# 1124
{union Cyc_YYSTYPE _tmp26F=yy1;struct Cyc_Absyn_Fndecl*_tmp270;_LL143: if((_tmp26F.YY17).tag != 23)goto _LL145;_tmp270=(struct Cyc_Absyn_Fndecl*)(_tmp26F.YY17).val;_LL144:
# 1126
 yyzzz=_tmp270;
# 1127
goto _LL142;_LL145:;_LL146:
# 1128
(int)_throw((void*)& Cyc_yyfail_YY17);_LL142:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.YY17).val=yy1,(((_tmp9A8.YY17).tag=23,_tmp9A8)));}static char _tmp273[18]="list_t<decl_t,`H>";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp273,_tmp273,_tmp273 + 18}};
# 1123
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
# 1124
struct Cyc_List_List*yyzzz;
# 1125
{union Cyc_YYSTYPE _tmp274=yy1;struct Cyc_List_List*_tmp275;_LL148: if((_tmp274.YY18).tag != 24)goto _LL14A;_tmp275=(struct Cyc_List_List*)(_tmp274.YY18).val;_LL149:
# 1127
 yyzzz=_tmp275;
# 1128
goto _LL147;_LL14A:;_LL14B:
# 1129
(int)_throw((void*)& Cyc_yyfail_YY18);_LL147:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.YY18).val=yy1,(((_tmp9A9.YY18).tag=24,_tmp9A9)));}static char _tmp278[12]="decl_spec_t";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp278,_tmp278,_tmp278 + 12}};
# 1125
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
# 1126
struct Cyc_Parse_Declaration_spec yyzzz;
# 1127
{union Cyc_YYSTYPE _tmp279=yy1;struct Cyc_Parse_Declaration_spec _tmp27A;_LL14D: if((_tmp279.YY19).tag != 25)goto _LL14F;_tmp27A=(struct Cyc_Parse_Declaration_spec)(_tmp279.YY19).val;_LL14E:
# 1129
 yyzzz=_tmp27A;
# 1130
goto _LL14C;_LL14F:;_LL150:
# 1131
(int)_throw((void*)& Cyc_yyfail_YY19);_LL14C:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.YY19).val=yy1,(((_tmp9AA.YY19).tag=25,_tmp9AA)));}static char _tmp27D[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp27D,_tmp27D,_tmp27D + 31}};
# 1126
struct _tuple12 Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
# 1127
struct _tuple12 yyzzz;
# 1128
{union Cyc_YYSTYPE _tmp27E=yy1;struct _tuple12 _tmp27F;_LL152: if((_tmp27E.YY20).tag != 26)goto _LL154;_tmp27F=(struct _tuple12)(_tmp27E.YY20).val;_LL153:
# 1130
 yyzzz=_tmp27F;
# 1131
goto _LL151;_LL154:;_LL155:
# 1132
(int)_throw((void*)& Cyc_yyfail_YY20);_LL151:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY20(struct _tuple12 yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.YY20).val=yy1,(((_tmp9AB.YY20).tag=26,_tmp9AB)));}static char _tmp282[23]="declarator_list_t<`yy>";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp282,_tmp282,_tmp282 + 23}};
# 1127
struct _tuple13*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
# 1128
struct _tuple13*yyzzz;
# 1129
{union Cyc_YYSTYPE _tmp283=yy1;struct _tuple13*_tmp284;_LL157: if((_tmp283.YY21).tag != 27)goto _LL159;_tmp284=(struct _tuple13*)(_tmp283.YY21).val;_LL158:
# 1131
 yyzzz=_tmp284;
# 1132
goto _LL156;_LL159:;_LL15A:
# 1133
(int)_throw((void*)& Cyc_yyfail_YY21);_LL156:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY21(struct _tuple13*yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY21).val=yy1,(((_tmp9AC.YY21).tag=27,_tmp9AC)));}static char _tmp287[19]="storage_class_t@`H";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp287,_tmp287,_tmp287 + 19}};
# 1128
enum Cyc_Parse_Storage_class*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
# 1129
enum Cyc_Parse_Storage_class*yyzzz;
# 1130
{union Cyc_YYSTYPE _tmp288=yy1;enum Cyc_Parse_Storage_class*_tmp289;_LL15C: if((_tmp288.YY22).tag != 28)goto _LL15E;_tmp289=(enum Cyc_Parse_Storage_class*)(_tmp288.YY22).val;_LL15D:
# 1132
 yyzzz=_tmp289;
# 1133
goto _LL15B;_LL15E:;_LL15F:
# 1134
(int)_throw((void*)& Cyc_yyfail_YY22);_LL15B:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY22).val=yy1,(((_tmp9AD.YY22).tag=28,_tmp9AD)));}static char _tmp28C[17]="type_specifier_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp28C,_tmp28C,_tmp28C + 17}};
# 1129
struct Cyc_Parse_Type_specifier Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
# 1130
struct Cyc_Parse_Type_specifier yyzzz;
# 1131
{union Cyc_YYSTYPE _tmp28D=yy1;struct Cyc_Parse_Type_specifier _tmp28E;_LL161: if((_tmp28D.YY23).tag != 29)goto _LL163;_tmp28E=(struct Cyc_Parse_Type_specifier)(_tmp28D.YY23).val;_LL162:
# 1133
 yyzzz=_tmp28E;
# 1134
goto _LL160;_LL163:;_LL164:
# 1135
(int)_throw((void*)& Cyc_yyfail_YY23);_LL160:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY23).val=yy1,(((_tmp9AE.YY23).tag=29,_tmp9AE)));}static char _tmp291[12]="aggr_kind_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp291,_tmp291,_tmp291 + 12}};
# 1131
enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
# 1132
enum Cyc_Absyn_AggrKind yyzzz;
# 1133
{union Cyc_YYSTYPE _tmp292=yy1;enum Cyc_Absyn_AggrKind _tmp293;_LL166: if((_tmp292.YY24).tag != 30)goto _LL168;_tmp293=(enum Cyc_Absyn_AggrKind)(_tmp292.YY24).val;_LL167:
# 1135
 yyzzz=_tmp293;
# 1136
goto _LL165;_LL168:;_LL169:
# 1137
(int)_throw((void*)& Cyc_yyfail_YY24);_LL165:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY24).val=yy1,(((_tmp9AF.YY24).tag=30,_tmp9AF)));}static char _tmp296[8]="tqual_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp296,_tmp296,_tmp296 + 8}};
# 1132
struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
# 1133
struct Cyc_Absyn_Tqual yyzzz;
# 1134
{union Cyc_YYSTYPE _tmp297=yy1;struct Cyc_Absyn_Tqual _tmp298;_LL16B: if((_tmp297.YY25).tag != 31)goto _LL16D;_tmp298=(struct Cyc_Absyn_Tqual)(_tmp297.YY25).val;_LL16C:
# 1136
 yyzzz=_tmp298;
# 1137
goto _LL16A;_LL16D:;_LL16E:
# 1138
(int)_throw((void*)& Cyc_yyfail_YY25);_LL16A:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY25).val=yy1,(((_tmp9B0.YY25).tag=31,_tmp9B0)));}static char _tmp29B[23]="list_t<aggrfield_t,`H>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp29B,_tmp29B,_tmp29B + 23}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
# 1134
struct Cyc_List_List*yyzzz;
# 1135
{union Cyc_YYSTYPE _tmp29C=yy1;struct Cyc_List_List*_tmp29D;_LL170: if((_tmp29C.YY26).tag != 32)goto _LL172;_tmp29D=(struct Cyc_List_List*)(_tmp29C.YY26).val;_LL171:
# 1137
 yyzzz=_tmp29D;
# 1138
goto _LL16F;_LL172:;_LL173:
# 1139
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16F:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY26).val=yy1,(((_tmp9B1.YY26).tag=32,_tmp9B1)));}static char _tmp2A0[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2A0,_tmp2A0,_tmp2A0 + 34}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
# 1135
struct Cyc_List_List*yyzzz;
# 1136
{union Cyc_YYSTYPE _tmp2A1=yy1;struct Cyc_List_List*_tmp2A2;_LL175: if((_tmp2A1.YY27).tag != 33)goto _LL177;_tmp2A2=(struct Cyc_List_List*)(_tmp2A1.YY27).val;_LL176:
# 1138
 yyzzz=_tmp2A2;
# 1139
goto _LL174;_LL177:;_LL178:
# 1140
(int)_throw((void*)& Cyc_yyfail_YY27);_LL174:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY27).val=yy1,(((_tmp9B2.YY27).tag=33,_tmp9B2)));}static char _tmp2A5[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2A5,_tmp2A5,_tmp2A5 + 33}};
# 1135
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
# 1136
struct Cyc_List_List*yyzzz;
# 1137
{union Cyc_YYSTYPE _tmp2A6=yy1;struct Cyc_List_List*_tmp2A7;_LL17A: if((_tmp2A6.YY28).tag != 34)goto _LL17C;_tmp2A7=(struct Cyc_List_List*)(_tmp2A6.YY28).val;_LL17B:
# 1139
 yyzzz=_tmp2A7;
# 1140
goto _LL179;_LL17C:;_LL17D:
# 1141
(int)_throw((void*)& Cyc_yyfail_YY28);_LL179:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY28).val=yy1,(((_tmp9B3.YY28).tag=34,_tmp9B3)));}static char _tmp2AA[18]="declarator_t<`yy>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2AA,_tmp2AA,_tmp2AA + 18}};
# 1136
struct Cyc_Parse_Declarator Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
# 1137
struct Cyc_Parse_Declarator yyzzz;
# 1138
{union Cyc_YYSTYPE _tmp2AB=yy1;struct Cyc_Parse_Declarator _tmp2AC;_LL17F: if((_tmp2AB.YY29).tag != 35)goto _LL181;_tmp2AC=(struct Cyc_Parse_Declarator)(_tmp2AB.YY29).val;_LL180:
# 1140
 yyzzz=_tmp2AC;
# 1141
goto _LL17E;_LL181:;_LL182:
# 1142
(int)_throw((void*)& Cyc_yyfail_YY29);_LL17E:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY29).val=yy1,(((_tmp9B4.YY29).tag=35,_tmp9B4)));}static char _tmp2AF[26]="abstractdeclarator_t<`yy>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2AF,_tmp2AF,_tmp2AF + 26}};
# 1139
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
# 1140
struct Cyc_Parse_Abstractdeclarator yyzzz;
# 1141
{union Cyc_YYSTYPE _tmp2B0=yy1;struct Cyc_Parse_Abstractdeclarator _tmp2B1;_LL184: if((_tmp2B0.YY30).tag != 36)goto _LL186;_tmp2B1=(struct Cyc_Parse_Abstractdeclarator)(_tmp2B0.YY30).val;_LL185:
# 1143
 yyzzz=_tmp2B1;
# 1144
goto _LL183;_LL186:;_LL187:
# 1145
(int)_throw((void*)& Cyc_yyfail_YY30);_LL183:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY30).val=yy1,(((_tmp9B5.YY30).tag=36,_tmp9B5)));}static char _tmp2B4[5]="bool";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2B4,_tmp2B4,_tmp2B4 + 5}};
# 1140
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
# 1141
int yyzzz;
# 1142
{union Cyc_YYSTYPE _tmp2B5=yy1;int _tmp2B6;_LL189: if((_tmp2B5.YY31).tag != 37)goto _LL18B;_tmp2B6=(int)(_tmp2B5.YY31).val;_LL18A:
# 1144
 yyzzz=_tmp2B6;
# 1145
goto _LL188;_LL18B:;_LL18C:
# 1146
(int)_throw((void*)& Cyc_yyfail_YY31);_LL188:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY31).val=yy1,(((_tmp9B6.YY31).tag=37,_tmp9B6)));}static char _tmp2B9[8]="scope_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2B9,_tmp2B9,_tmp2B9 + 8}};
# 1141
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
# 1142
enum Cyc_Absyn_Scope yyzzz;
# 1143
{union Cyc_YYSTYPE _tmp2BA=yy1;enum Cyc_Absyn_Scope _tmp2BB;_LL18E: if((_tmp2BA.YY32).tag != 38)goto _LL190;_tmp2BB=(enum Cyc_Absyn_Scope)(_tmp2BA.YY32).val;_LL18F:
# 1145
 yyzzz=_tmp2BB;
# 1146
goto _LL18D;_LL190:;_LL191:
# 1147
(int)_throw((void*)& Cyc_yyfail_YY32);_LL18D:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY32).val=yy1,(((_tmp9B7.YY32).tag=38,_tmp9B7)));}static char _tmp2BE[16]="datatypefield_t";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2BE,_tmp2BE,_tmp2BE + 16}};
# 1142
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
# 1143
struct Cyc_Absyn_Datatypefield*yyzzz;
# 1144
{union Cyc_YYSTYPE _tmp2BF=yy1;struct Cyc_Absyn_Datatypefield*_tmp2C0;_LL193: if((_tmp2BF.YY33).tag != 39)goto _LL195;_tmp2C0=(struct Cyc_Absyn_Datatypefield*)(_tmp2BF.YY33).val;_LL194:
# 1146
 yyzzz=_tmp2C0;
# 1147
goto _LL192;_LL195:;_LL196:
# 1148
(int)_throw((void*)& Cyc_yyfail_YY33);_LL192:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY33).val=yy1,(((_tmp9B8.YY33).tag=39,_tmp9B8)));}static char _tmp2C3[27]="list_t<datatypefield_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2C3,_tmp2C3,_tmp2C3 + 27}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
# 1144
struct Cyc_List_List*yyzzz;
# 1145
{union Cyc_YYSTYPE _tmp2C4=yy1;struct Cyc_List_List*_tmp2C5;_LL198: if((_tmp2C4.YY34).tag != 40)goto _LL19A;_tmp2C5=(struct Cyc_List_List*)(_tmp2C4.YY34).val;_LL199:
# 1147
 yyzzz=_tmp2C5;
# 1148
goto _LL197;_LL19A:;_LL19B:
# 1149
(int)_throw((void*)& Cyc_yyfail_YY34);_LL197:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY34).val=yy1,(((_tmp9B9.YY34).tag=40,_tmp9B9)));}static char _tmp2C8[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2C8,_tmp2C8,_tmp2C8 + 41}};
# 1144
struct _tuple24 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
# 1145
struct _tuple24 yyzzz;
# 1146
{union Cyc_YYSTYPE _tmp2C9=yy1;struct _tuple24 _tmp2CA;_LL19D: if((_tmp2C9.YY35).tag != 41)goto _LL19F;_tmp2CA=(struct _tuple24)(_tmp2C9.YY35).val;_LL19E:
# 1148
 yyzzz=_tmp2CA;
# 1149
goto _LL19C;_LL19F:;_LL1A0:
# 1150
(int)_throw((void*)& Cyc_yyfail_YY35);_LL19C:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY35(struct _tuple24 yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY35).val=yy1,(((_tmp9BA.YY35).tag=41,_tmp9BA)));}static char _tmp2CD[17]="list_t<var_t,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2CD,_tmp2CD,_tmp2CD + 17}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
# 1146
struct Cyc_List_List*yyzzz;
# 1147
{union Cyc_YYSTYPE _tmp2CE=yy1;struct Cyc_List_List*_tmp2CF;_LL1A2: if((_tmp2CE.YY36).tag != 42)goto _LL1A4;_tmp2CF=(struct Cyc_List_List*)(_tmp2CE.YY36).val;_LL1A3:
# 1149
 yyzzz=_tmp2CF;
# 1150
goto _LL1A1;_LL1A4:;_LL1A5:
# 1151
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A1:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY36).val=yy1,(((_tmp9BB.YY36).tag=42,_tmp9BB)));}static char _tmp2D2[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2D2,_tmp2D2,_tmp2D2 + 31}};
# 1146
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
# 1147
struct _tuple8*yyzzz;
# 1148
{union Cyc_YYSTYPE _tmp2D3=yy1;struct _tuple8*_tmp2D4;_LL1A7: if((_tmp2D3.YY37).tag != 43)goto _LL1A9;_tmp2D4=(struct _tuple8*)(_tmp2D3.YY37).val;_LL1A8:
# 1150
 yyzzz=_tmp2D4;
# 1151
goto _LL1A6;_LL1A9:;_LL1AA:
# 1152
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A6:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY37).val=yy1,(((_tmp9BC.YY37).tag=43,_tmp9BC)));}static char _tmp2D7[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2D7,_tmp2D7,_tmp2D7 + 42}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
# 1148
struct Cyc_List_List*yyzzz;
# 1149
{union Cyc_YYSTYPE _tmp2D8=yy1;struct Cyc_List_List*_tmp2D9;_LL1AC: if((_tmp2D8.YY38).tag != 44)goto _LL1AE;_tmp2D9=(struct Cyc_List_List*)(_tmp2D8.YY38).val;_LL1AD:
# 1151
 yyzzz=_tmp2D9;
# 1152
goto _LL1AB;_LL1AE:;_LL1AF:
# 1153
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1AB:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY38).val=yy1,(((_tmp9BD.YY38).tag=44,_tmp9BD)));}static char _tmp2DC[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2DC,_tmp2DC,_tmp2DC + 115}};
# 1148
struct _tuple25*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
# 1149
struct _tuple25*yyzzz;
# 1150
{union Cyc_YYSTYPE _tmp2DD=yy1;struct _tuple25*_tmp2DE;_LL1B1: if((_tmp2DD.YY39).tag != 45)goto _LL1B3;_tmp2DE=(struct _tuple25*)(_tmp2DD.YY39).val;_LL1B2:
# 1152
 yyzzz=_tmp2DE;
# 1153
goto _LL1B0;_LL1B3:;_LL1B4:
# 1154
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY39(struct _tuple25*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY39).val=yy1,(((_tmp9BE.YY39).tag=45,_tmp9BE)));}static char _tmp2E1[18]="list_t<type_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2E1,_tmp2E1,_tmp2E1 + 18}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
# 1150
struct Cyc_List_List*yyzzz;
# 1151
{union Cyc_YYSTYPE _tmp2E2=yy1;struct Cyc_List_List*_tmp2E3;_LL1B6: if((_tmp2E2.YY40).tag != 46)goto _LL1B8;_tmp2E3=(struct Cyc_List_List*)(_tmp2E2.YY40).val;_LL1B7:
# 1153
 yyzzz=_tmp2E3;
# 1154
goto _LL1B5;_LL1B8:;_LL1B9:
# 1155
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B5:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY40).val=yy1,(((_tmp9BF.YY40).tag=46,_tmp9BF)));}static char _tmp2E6[24]="list_t<designator_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2E6,_tmp2E6,_tmp2E6 + 24}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
# 1152
struct Cyc_List_List*yyzzz;
# 1153
{union Cyc_YYSTYPE _tmp2E7=yy1;struct Cyc_List_List*_tmp2E8;_LL1BB: if((_tmp2E7.YY41).tag != 47)goto _LL1BD;_tmp2E8=(struct Cyc_List_List*)(_tmp2E7.YY41).val;_LL1BC:
# 1155
 yyzzz=_tmp2E8;
# 1156
goto _LL1BA;_LL1BD:;_LL1BE:
# 1157
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1BA:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY41).val=yy1,(((_tmp9C0.YY41).tag=47,_tmp9C0)));}static char _tmp2EB[13]="designator_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2EB,_tmp2EB,_tmp2EB + 13}};
# 1152
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
# 1153
void*yyzzz;
# 1154
{union Cyc_YYSTYPE _tmp2EC=yy1;void*_tmp2ED;_LL1C0: if((_tmp2EC.YY42).tag != 48)goto _LL1C2;_tmp2ED=(void*)(_tmp2EC.YY42).val;_LL1C1:
# 1156
 yyzzz=_tmp2ED;
# 1157
goto _LL1BF;_LL1C2:;_LL1C3:
# 1158
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BF:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY42).val=yy1,(((_tmp9C1.YY42).tag=48,_tmp9C1)));}static char _tmp2F0[7]="kind_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2F0,_tmp2F0,_tmp2F0 + 7}};
# 1153
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
# 1154
struct Cyc_Absyn_Kind*yyzzz;
# 1155
{union Cyc_YYSTYPE _tmp2F1=yy1;struct Cyc_Absyn_Kind*_tmp2F2;_LL1C5: if((_tmp2F1.YY43).tag != 49)goto _LL1C7;_tmp2F2=(struct Cyc_Absyn_Kind*)(_tmp2F1.YY43).val;_LL1C6:
# 1157
 yyzzz=_tmp2F2;
# 1158
goto _LL1C4;_LL1C7:;_LL1C8:
# 1159
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C4:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY43).val=yy1,(((_tmp9C2.YY43).tag=49,_tmp9C2)));}static char _tmp2F5[7]="type_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2F5,_tmp2F5,_tmp2F5 + 7}};
# 1154
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
# 1155
void*yyzzz;
# 1156
{union Cyc_YYSTYPE _tmp2F6=yy1;void*_tmp2F7;_LL1CA: if((_tmp2F6.YY44).tag != 50)goto _LL1CC;_tmp2F7=(void*)(_tmp2F6.YY44).val;_LL1CB:
# 1158
 yyzzz=_tmp2F7;
# 1159
goto _LL1C9;_LL1CC:;_LL1CD:
# 1160
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C9:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY44).val=yy1,(((_tmp9C3.YY44).tag=50,_tmp9C3)));}static char _tmp2FA[23]="list_t<attribute_t,`H>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2FA,_tmp2FA,_tmp2FA + 23}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
# 1156
struct Cyc_List_List*yyzzz;
# 1157
{union Cyc_YYSTYPE _tmp2FB=yy1;struct Cyc_List_List*_tmp2FC;_LL1CF: if((_tmp2FB.YY45).tag != 51)goto _LL1D1;_tmp2FC=(struct Cyc_List_List*)(_tmp2FB.YY45).val;_LL1D0:
# 1159
 yyzzz=_tmp2FC;
# 1160
goto _LL1CE;_LL1D1:;_LL1D2:
# 1161
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1CE:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY45).val=yy1,(((_tmp9C4.YY45).tag=51,_tmp9C4)));}static char _tmp2FF[12]="attribute_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2FF,_tmp2FF,_tmp2FF + 12}};
# 1156
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
# 1157
void*yyzzz;
# 1158
{union Cyc_YYSTYPE _tmp300=yy1;void*_tmp301;_LL1D4: if((_tmp300.YY46).tag != 52)goto _LL1D6;_tmp301=(void*)(_tmp300.YY46).val;_LL1D5:
# 1160
 yyzzz=_tmp301;
# 1161
goto _LL1D3;_LL1D6:;_LL1D7:
# 1162
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1D3:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY46).val=yy1,(((_tmp9C5.YY46).tag=52,_tmp9C5)));}static char _tmp304[12]="enumfield_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp304,_tmp304,_tmp304 + 12}};
# 1157
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
# 1158
struct Cyc_Absyn_Enumfield*yyzzz;
# 1159
{union Cyc_YYSTYPE _tmp305=yy1;struct Cyc_Absyn_Enumfield*_tmp306;_LL1D9: if((_tmp305.YY47).tag != 53)goto _LL1DB;_tmp306=(struct Cyc_Absyn_Enumfield*)(_tmp305.YY47).val;_LL1DA:
# 1161
 yyzzz=_tmp306;
# 1162
goto _LL1D8;_LL1DB:;_LL1DC:
# 1163
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D8:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY47).val=yy1,(((_tmp9C6.YY47).tag=53,_tmp9C6)));}static char _tmp309[23]="list_t<enumfield_t,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp309,_tmp309,_tmp309 + 23}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
# 1159
struct Cyc_List_List*yyzzz;
# 1160
{union Cyc_YYSTYPE _tmp30A=yy1;struct Cyc_List_List*_tmp30B;_LL1DE: if((_tmp30A.YY48).tag != 54)goto _LL1E0;_tmp30B=(struct Cyc_List_List*)(_tmp30A.YY48).val;_LL1DF:
# 1162
 yyzzz=_tmp30B;
# 1163
goto _LL1DD;_LL1E0:;_LL1E1:
# 1164
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1DD:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY48).val=yy1,(((_tmp9C7.YY48).tag=54,_tmp9C7)));}static char _tmp30E[11]="type_opt_t";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp30E,_tmp30E,_tmp30E + 11}};
# 1159
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
# 1160
void*yyzzz;
# 1161
{union Cyc_YYSTYPE _tmp30F=yy1;void*_tmp310;_LL1E3: if((_tmp30F.YY49).tag != 55)goto _LL1E5;_tmp310=(void*)(_tmp30F.YY49).val;_LL1E4:
# 1163
 yyzzz=_tmp310;
# 1164
goto _LL1E2;_LL1E5:;_LL1E6:
# 1165
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E2:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY49).val=yy1,(((_tmp9C8.YY49).tag=55,_tmp9C8)));}static char _tmp313[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp313,_tmp313,_tmp313 + 31}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
# 1161
struct Cyc_List_List*yyzzz;
# 1162
{union Cyc_YYSTYPE _tmp314=yy1;struct Cyc_List_List*_tmp315;_LL1E8: if((_tmp314.YY50).tag != 56)goto _LL1EA;_tmp315=(struct Cyc_List_List*)(_tmp314.YY50).val;_LL1E9:
# 1164
 yyzzz=_tmp315;
# 1165
goto _LL1E7;_LL1EA:;_LL1EB:
# 1166
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E7:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY50).val=yy1,(((_tmp9C9.YY50).tag=56,_tmp9C9)));}static char _tmp318[15]="conref_t<bool>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp318,_tmp318,_tmp318 + 15}};
# 1161
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
# 1162
union Cyc_Absyn_Constraint*yyzzz;
# 1163
{union Cyc_YYSTYPE _tmp319=yy1;union Cyc_Absyn_Constraint*_tmp31A;_LL1ED: if((_tmp319.YY51).tag != 57)goto _LL1EF;_tmp31A=(union Cyc_Absyn_Constraint*)(_tmp319.YY51).val;_LL1EE:
# 1165
 yyzzz=_tmp31A;
# 1166
goto _LL1EC;_LL1EF:;_LL1F0:
# 1167
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1EC:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY51).val=yy1,(((_tmp9CA.YY51).tag=57,_tmp9CA)));}static char _tmp31D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp31D,_tmp31D,_tmp31D + 45}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
# 1163
struct Cyc_List_List*yyzzz;
# 1164
{union Cyc_YYSTYPE _tmp31E=yy1;struct Cyc_List_List*_tmp31F;_LL1F2: if((_tmp31E.YY52).tag != 58)goto _LL1F4;_tmp31F=(struct Cyc_List_List*)(_tmp31E.YY52).val;_LL1F3:
# 1166
 yyzzz=_tmp31F;
# 1167
goto _LL1F1;_LL1F4:;_LL1F5:
# 1168
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1F1:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY52).val=yy1,(((_tmp9CB.YY52).tag=58,_tmp9CB)));}static char _tmp322[20]="pointer_qual_t<`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp322,_tmp322,_tmp322 + 20}};
# 1163
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
# 1164
void*yyzzz;
# 1165
{union Cyc_YYSTYPE _tmp323=yy1;void*_tmp324;_LL1F7: if((_tmp323.YY53).tag != 59)goto _LL1F9;_tmp324=(void*)(_tmp323.YY53).val;_LL1F8:
# 1167
 yyzzz=_tmp324;
# 1168
goto _LL1F6;_LL1F9:;_LL1FA:
# 1169
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F6:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY53).val=yy1,(((_tmp9CC.YY53).tag=59,_tmp9CC)));}static char _tmp327[21]="pointer_quals_t<`yy>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp327,_tmp327,_tmp327 + 21}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
# 1165
struct Cyc_List_List*yyzzz;
# 1166
{union Cyc_YYSTYPE _tmp328=yy1;struct Cyc_List_List*_tmp329;_LL1FC: if((_tmp328.YY54).tag != 60)goto _LL1FE;_tmp329=(struct Cyc_List_List*)(_tmp328.YY54).val;_LL1FD:
# 1168
 yyzzz=_tmp329;
# 1169
goto _LL1FB;_LL1FE:;_LL1FF:
# 1170
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1FB:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY54).val=yy1,(((_tmp9CD.YY54).tag=60,_tmp9CD)));}static char _tmp32C[21]="$(bool,string_t<`H>)";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp32C,_tmp32C,_tmp32C + 21}};
# 1165
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
# 1166
struct _tuple20 yyzzz;
# 1167
{union Cyc_YYSTYPE _tmp32D=yy1;struct _tuple20 _tmp32E;_LL201: if((_tmp32D.Asm_tok).tag != 6)goto _LL203;_tmp32E=(struct _tuple20)(_tmp32D.Asm_tok).val;_LL202:
# 1169
 yyzzz=_tmp32E;
# 1170
goto _LL200;_LL203:;_LL204:
# 1171
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL200:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.Asm_tok).val=yy1,(((_tmp9CE.Asm_tok).tag=6,_tmp9CE)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1187
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1191
struct Cyc_Yyltype Cyc_yynewloc(){
# 1192
struct Cyc_Yyltype _tmp9CF;return(_tmp9CF.timestamp=0,((_tmp9CF.first_line=0,((_tmp9CF.first_column=0,((_tmp9CF.last_line=0,((_tmp9CF.last_column=0,_tmp9CF)))))))));}
# 1194
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1205 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp331[2]="$";static char _tmp332[6]="error";static char _tmp333[12]="$undefined.";static char _tmp334[5]="AUTO";static char _tmp335[9]="REGISTER";static char _tmp336[7]="STATIC";static char _tmp337[7]="EXTERN";static char _tmp338[8]="TYPEDEF";static char _tmp339[5]="VOID";static char _tmp33A[5]="CHAR";static char _tmp33B[6]="SHORT";static char _tmp33C[4]="INT";static char _tmp33D[5]="LONG";static char _tmp33E[6]="FLOAT";static char _tmp33F[7]="DOUBLE";static char _tmp340[7]="SIGNED";static char _tmp341[9]="UNSIGNED";static char _tmp342[6]="CONST";static char _tmp343[9]="VOLATILE";static char _tmp344[9]="RESTRICT";static char _tmp345[7]="STRUCT";static char _tmp346[6]="UNION";static char _tmp347[5]="CASE";static char _tmp348[8]="DEFAULT";static char _tmp349[7]="INLINE";static char _tmp34A[7]="SIZEOF";static char _tmp34B[9]="OFFSETOF";static char _tmp34C[3]="IF";static char _tmp34D[5]="ELSE";static char _tmp34E[7]="SWITCH";static char _tmp34F[6]="WHILE";static char _tmp350[3]="DO";static char _tmp351[4]="FOR";static char _tmp352[5]="GOTO";static char _tmp353[9]="CONTINUE";static char _tmp354[6]="BREAK";static char _tmp355[7]="RETURN";static char _tmp356[5]="ENUM";static char _tmp357[8]="NULL_kw";static char _tmp358[4]="LET";static char _tmp359[6]="THROW";static char _tmp35A[4]="TRY";static char _tmp35B[6]="CATCH";static char _tmp35C[7]="EXPORT";static char _tmp35D[4]="NEW";static char _tmp35E[9]="ABSTRACT";static char _tmp35F[9]="FALLTHRU";static char _tmp360[6]="USING";static char _tmp361[10]="NAMESPACE";static char _tmp362[9]="DATATYPE";static char _tmp363[8]="XTUNION";static char _tmp364[7]="TUNION";static char _tmp365[7]="MALLOC";static char _tmp366[8]="RMALLOC";static char _tmp367[7]="CALLOC";static char _tmp368[8]="RCALLOC";static char _tmp369[5]="SWAP";static char _tmp36A[9]="REGION_T";static char _tmp36B[6]="TAG_T";static char _tmp36C[7]="REGION";static char _tmp36D[5]="RNEW";static char _tmp36E[8]="REGIONS";static char _tmp36F[13]="RESET_REGION";static char _tmp370[16]="NOZEROTERM_QUAL";static char _tmp371[14]="ZEROTERM_QUAL";static char _tmp372[12]="REGION_QUAL";static char _tmp373[7]="PORTON";static char _tmp374[8]="PORTOFF";static char _tmp375[12]="DYNREGION_T";static char _tmp376[6]="ALIAS";static char _tmp377[8]="NUMELTS";static char _tmp378[8]="VALUEOF";static char _tmp379[10]="VALUEOF_T";static char _tmp37A[9]="TAGCHECK";static char _tmp37B[13]="NUMELTS_QUAL";static char _tmp37C[10]="THIN_QUAL";static char _tmp37D[9]="FAT_QUAL";static char _tmp37E[13]="NOTNULL_QUAL";static char _tmp37F[14]="NULLABLE_QUAL";static char _tmp380[12]="TAGGED_QUAL";static char _tmp381[16]="EXTENSIBLE_QUAL";static char _tmp382[15]="RESETABLE_QUAL";static char _tmp383[7]="PTR_OP";static char _tmp384[7]="INC_OP";static char _tmp385[7]="DEC_OP";static char _tmp386[8]="LEFT_OP";static char _tmp387[9]="RIGHT_OP";static char _tmp388[6]="LE_OP";static char _tmp389[6]="GE_OP";static char _tmp38A[6]="EQ_OP";static char _tmp38B[6]="NE_OP";static char _tmp38C[7]="AND_OP";static char _tmp38D[6]="OR_OP";static char _tmp38E[11]="MUL_ASSIGN";static char _tmp38F[11]="DIV_ASSIGN";static char _tmp390[11]="MOD_ASSIGN";static char _tmp391[11]="ADD_ASSIGN";static char _tmp392[11]="SUB_ASSIGN";static char _tmp393[12]="LEFT_ASSIGN";static char _tmp394[13]="RIGHT_ASSIGN";static char _tmp395[11]="AND_ASSIGN";static char _tmp396[11]="XOR_ASSIGN";static char _tmp397[10]="OR_ASSIGN";static char _tmp398[9]="ELLIPSIS";static char _tmp399[11]="LEFT_RIGHT";static char _tmp39A[12]="COLON_COLON";static char _tmp39B[11]="IDENTIFIER";static char _tmp39C[17]="INTEGER_CONSTANT";static char _tmp39D[7]="STRING";static char _tmp39E[8]="WSTRING";static char _tmp39F[19]="CHARACTER_CONSTANT";static char _tmp3A0[20]="WCHARACTER_CONSTANT";static char _tmp3A1[18]="FLOATING_CONSTANT";static char _tmp3A2[9]="TYPE_VAR";static char _tmp3A3[13]="TYPEDEF_NAME";static char _tmp3A4[16]="QUAL_IDENTIFIER";static char _tmp3A5[18]="QUAL_TYPEDEF_NAME";static char _tmp3A6[10]="ATTRIBUTE";static char _tmp3A7[4]="ASM";static char _tmp3A8[4]="';'";static char _tmp3A9[4]="'{'";static char _tmp3AA[4]="'}'";static char _tmp3AB[4]="','";static char _tmp3AC[4]="'='";static char _tmp3AD[4]="'<'";static char _tmp3AE[4]="'>'";static char _tmp3AF[4]="'('";static char _tmp3B0[4]="')'";static char _tmp3B1[4]="'_'";static char _tmp3B2[4]="'$'";static char _tmp3B3[4]="':'";static char _tmp3B4[4]="'.'";static char _tmp3B5[4]="'['";static char _tmp3B6[4]="']'";static char _tmp3B7[4]="'*'";static char _tmp3B8[4]="'@'";static char _tmp3B9[4]="'?'";static char _tmp3BA[4]="'+'";static char _tmp3BB[4]="'|'";static char _tmp3BC[4]="'^'";static char _tmp3BD[4]="'&'";static char _tmp3BE[4]="'-'";static char _tmp3BF[4]="'/'";static char _tmp3C0[4]="'%'";static char _tmp3C1[4]="'~'";static char _tmp3C2[4]="'!'";static char _tmp3C3[5]="prog";static char _tmp3C4[17]="translation_unit";static char _tmp3C5[12]="export_list";static char _tmp3C6[19]="export_list_values";static char _tmp3C7[21]="external_declaration";static char _tmp3C8[20]="function_definition";static char _tmp3C9[21]="function_definition2";static char _tmp3CA[13]="using_action";static char _tmp3CB[15]="unusing_action";static char _tmp3CC[17]="namespace_action";static char _tmp3CD[19]="unnamespace_action";static char _tmp3CE[12]="declaration";static char _tmp3CF[19]="resetable_qual_opt";static char _tmp3D0[17]="declaration_list";static char _tmp3D1[23]="declaration_specifiers";static char _tmp3D2[24]="storage_class_specifier";static char _tmp3D3[15]="attributes_opt";static char _tmp3D4[11]="attributes";static char _tmp3D5[15]="attribute_list";static char _tmp3D6[10]="attribute";static char _tmp3D7[15]="type_specifier";static char _tmp3D8[25]="type_specifier_notypedef";static char _tmp3D9[5]="kind";static char _tmp3DA[15]="type_qualifier";static char _tmp3DB[15]="enum_specifier";static char _tmp3DC[11]="enum_field";static char _tmp3DD[22]="enum_declaration_list";static char _tmp3DE[26]="struct_or_union_specifier";static char _tmp3DF[16]="type_params_opt";static char _tmp3E0[16]="struct_or_union";static char _tmp3E1[24]="struct_declaration_list";static char _tmp3E2[25]="struct_declaration_list0";static char _tmp3E3[21]="init_declarator_list";static char _tmp3E4[22]="init_declarator_list0";static char _tmp3E5[16]="init_declarator";static char _tmp3E6[19]="struct_declaration";static char _tmp3E7[25]="specifier_qualifier_list";static char _tmp3E8[35]="notypedef_specifier_qualifier_list";static char _tmp3E9[23]="struct_declarator_list";static char _tmp3EA[24]="struct_declarator_list0";static char _tmp3EB[18]="struct_declarator";static char _tmp3EC[19]="datatype_specifier";static char _tmp3ED[14]="qual_datatype";static char _tmp3EE[19]="datatypefield_list";static char _tmp3EF[20]="datatypefield_scope";static char _tmp3F0[14]="datatypefield";static char _tmp3F1[11]="declarator";static char _tmp3F2[23]="declarator_withtypedef";static char _tmp3F3[18]="direct_declarator";static char _tmp3F4[30]="direct_declarator_withtypedef";static char _tmp3F5[8]="pointer";static char _tmp3F6[12]="one_pointer";static char _tmp3F7[14]="pointer_quals";static char _tmp3F8[13]="pointer_qual";static char _tmp3F9[23]="pointer_null_and_bound";static char _tmp3FA[14]="pointer_bound";static char _tmp3FB[18]="zeroterm_qual_opt";static char _tmp3FC[8]="rgn_opt";static char _tmp3FD[11]="tqual_list";static char _tmp3FE[20]="parameter_type_list";static char _tmp3FF[9]="type_var";static char _tmp400[16]="optional_effect";static char _tmp401[19]="optional_rgn_order";static char _tmp402[10]="rgn_order";static char _tmp403[16]="optional_inject";static char _tmp404[11]="effect_set";static char _tmp405[14]="atomic_effect";static char _tmp406[11]="region_set";static char _tmp407[15]="parameter_list";static char _tmp408[22]="parameter_declaration";static char _tmp409[16]="identifier_list";static char _tmp40A[17]="identifier_list0";static char _tmp40B[12]="initializer";static char _tmp40C[18]="array_initializer";static char _tmp40D[17]="initializer_list";static char _tmp40E[12]="designation";static char _tmp40F[16]="designator_list";static char _tmp410[11]="designator";static char _tmp411[10]="type_name";static char _tmp412[14]="any_type_name";static char _tmp413[15]="type_name_list";static char _tmp414[20]="abstract_declarator";static char _tmp415[27]="direct_abstract_declarator";static char _tmp416[10]="statement";static char _tmp417[13]="open_exp_opt";static char _tmp418[18]="labeled_statement";static char _tmp419[21]="expression_statement";static char _tmp41A[19]="compound_statement";static char _tmp41B[16]="block_item_list";static char _tmp41C[20]="selection_statement";static char _tmp41D[15]="switch_clauses";static char _tmp41E[20]="iteration_statement";static char _tmp41F[15]="jump_statement";static char _tmp420[12]="exp_pattern";static char _tmp421[20]="conditional_pattern";static char _tmp422[19]="logical_or_pattern";static char _tmp423[20]="logical_and_pattern";static char _tmp424[21]="inclusive_or_pattern";static char _tmp425[21]="exclusive_or_pattern";static char _tmp426[12]="and_pattern";static char _tmp427[17]="equality_pattern";static char _tmp428[19]="relational_pattern";static char _tmp429[14]="shift_pattern";static char _tmp42A[17]="additive_pattern";static char _tmp42B[23]="multiplicative_pattern";static char _tmp42C[13]="cast_pattern";static char _tmp42D[14]="unary_pattern";static char _tmp42E[16]="postfix_pattern";static char _tmp42F[16]="primary_pattern";static char _tmp430[8]="pattern";static char _tmp431[19]="tuple_pattern_list";static char _tmp432[20]="tuple_pattern_list0";static char _tmp433[14]="field_pattern";static char _tmp434[19]="field_pattern_list";static char _tmp435[20]="field_pattern_list0";static char _tmp436[11]="expression";static char _tmp437[22]="assignment_expression";static char _tmp438[20]="assignment_operator";static char _tmp439[23]="conditional_expression";static char _tmp43A[20]="constant_expression";static char _tmp43B[22]="logical_or_expression";static char _tmp43C[23]="logical_and_expression";static char _tmp43D[24]="inclusive_or_expression";static char _tmp43E[24]="exclusive_or_expression";static char _tmp43F[15]="and_expression";static char _tmp440[20]="equality_expression";static char _tmp441[22]="relational_expression";static char _tmp442[17]="shift_expression";static char _tmp443[20]="additive_expression";static char _tmp444[26]="multiplicative_expression";static char _tmp445[16]="cast_expression";static char _tmp446[17]="unary_expression";static char _tmp447[15]="unary_operator";static char _tmp448[19]="postfix_expression";static char _tmp449[19]="primary_expression";static char _tmp44A[25]="argument_expression_list";static char _tmp44B[26]="argument_expression_list0";static char _tmp44C[9]="constant";static char _tmp44D[20]="qual_opt_identifier";static char _tmp44E[17]="qual_opt_typedef";static char _tmp44F[18]="struct_union_name";static char _tmp450[11]="field_name";static char _tmp451[12]="right_angle";
# 1551 "parse.y"
static struct _dyneither_ptr Cyc_yytname[289]={{_tmp331,_tmp331,_tmp331 + 2},{_tmp332,_tmp332,_tmp332 + 6},{_tmp333,_tmp333,_tmp333 + 12},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 9},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 8},{_tmp339,_tmp339,_tmp339 + 5},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 6},{_tmp33C,_tmp33C,_tmp33C + 4},{_tmp33D,_tmp33D,_tmp33D + 5},{_tmp33E,_tmp33E,_tmp33E + 6},{_tmp33F,_tmp33F,_tmp33F + 7},{_tmp340,_tmp340,_tmp340 + 7},{_tmp341,_tmp341,_tmp341 + 9},{_tmp342,_tmp342,_tmp342 + 6},{_tmp343,_tmp343,_tmp343 + 9},{_tmp344,_tmp344,_tmp344 + 9},{_tmp345,_tmp345,_tmp345 + 7},{_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 5},{_tmp348,_tmp348,_tmp348 + 8},{_tmp349,_tmp349,_tmp349 + 7},{_tmp34A,_tmp34A,_tmp34A + 7},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 3},{_tmp34D,_tmp34D,_tmp34D + 5},{_tmp34E,_tmp34E,_tmp34E + 7},{_tmp34F,_tmp34F,_tmp34F + 6},{_tmp350,_tmp350,_tmp350 + 3},{_tmp351,_tmp351,_tmp351 + 4},{_tmp352,_tmp352,_tmp352 + 5},{_tmp353,_tmp353,_tmp353 + 9},{_tmp354,_tmp354,_tmp354 + 6},{_tmp355,_tmp355,_tmp355 + 7},{_tmp356,_tmp356,_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 8},{_tmp358,_tmp358,_tmp358 + 4},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 4},{_tmp35B,_tmp35B,_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 7},{_tmp35D,_tmp35D,_tmp35D + 4},{_tmp35E,_tmp35E,_tmp35E + 9},{_tmp35F,_tmp35F,_tmp35F + 9},{_tmp360,_tmp360,_tmp360 + 6},{_tmp361,_tmp361,_tmp361 + 10},{_tmp362,_tmp362,_tmp362 + 9},{_tmp363,_tmp363,_tmp363 + 8},{_tmp364,_tmp364,_tmp364 + 7},{_tmp365,_tmp365,_tmp365 + 7},{_tmp366,_tmp366,_tmp366 + 8},{_tmp367,_tmp367,_tmp367 + 7},{_tmp368,_tmp368,_tmp368 + 8},{_tmp369,_tmp369,_tmp369 + 5},{_tmp36A,_tmp36A,_tmp36A + 9},{_tmp36B,_tmp36B,_tmp36B + 6},{_tmp36C,_tmp36C,_tmp36C + 7},{_tmp36D,_tmp36D,_tmp36D + 5},{_tmp36E,_tmp36E,_tmp36E + 8},{_tmp36F,_tmp36F,_tmp36F + 13},{_tmp370,_tmp370,_tmp370 + 16},{_tmp371,_tmp371,_tmp371 + 14},{_tmp372,_tmp372,_tmp372 + 12},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 8},{_tmp375,_tmp375,_tmp375 + 12},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,_tmp377 + 8},{_tmp378,_tmp378,_tmp378 + 8},{_tmp379,_tmp379,_tmp379 + 10},{_tmp37A,_tmp37A,_tmp37A + 9},{_tmp37B,_tmp37B,_tmp37B + 13},{_tmp37C,_tmp37C,_tmp37C + 10},{_tmp37D,_tmp37D,_tmp37D + 9},{_tmp37E,_tmp37E,_tmp37E + 13},{_tmp37F,_tmp37F,_tmp37F + 14},{_tmp380,_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 16},{_tmp382,_tmp382,_tmp382 + 15},{_tmp383,_tmp383,_tmp383 + 7},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,_tmp385,_tmp385 + 7},{_tmp386,_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 9},{_tmp388,_tmp388,_tmp388 + 6},{_tmp389,_tmp389,_tmp389 + 6},{_tmp38A,_tmp38A,_tmp38A + 6},{_tmp38B,_tmp38B,_tmp38B + 6},{_tmp38C,_tmp38C,_tmp38C + 7},{_tmp38D,_tmp38D,_tmp38D + 6},{_tmp38E,_tmp38E,_tmp38E + 11},{_tmp38F,_tmp38F,_tmp38F + 11},{_tmp390,_tmp390,_tmp390 + 11},{_tmp391,_tmp391,_tmp391 + 11},{_tmp392,_tmp392,_tmp392 + 11},{_tmp393,_tmp393,_tmp393 + 12},{_tmp394,_tmp394,_tmp394 + 13},{_tmp395,_tmp395,_tmp395 + 11},{_tmp396,_tmp396,_tmp396 + 11},{_tmp397,_tmp397,_tmp397 + 10},{_tmp398,_tmp398,_tmp398 + 9},{_tmp399,_tmp399,_tmp399 + 11},{_tmp39A,_tmp39A,_tmp39A + 12},{_tmp39B,_tmp39B,_tmp39B + 11},{_tmp39C,_tmp39C,_tmp39C + 17},{_tmp39D,_tmp39D,_tmp39D + 7},{_tmp39E,_tmp39E,_tmp39E + 8},{_tmp39F,_tmp39F,_tmp39F + 19},{_tmp3A0,_tmp3A0,_tmp3A0 + 20},{_tmp3A1,_tmp3A1,_tmp3A1 + 18},{_tmp3A2,_tmp3A2,_tmp3A2 + 9},{_tmp3A3,_tmp3A3,_tmp3A3 + 13},{_tmp3A4,_tmp3A4,_tmp3A4 + 16},{_tmp3A5,_tmp3A5,_tmp3A5 + 18},{_tmp3A6,_tmp3A6,_tmp3A6 + 10},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 4},{_tmp3B6,_tmp3B6,_tmp3B6 + 4},{_tmp3B7,_tmp3B7,_tmp3B7 + 4},{_tmp3B8,_tmp3B8,_tmp3B8 + 4},{_tmp3B9,_tmp3B9,_tmp3B9 + 4},{_tmp3BA,_tmp3BA,_tmp3BA + 4},{_tmp3BB,_tmp3BB,_tmp3BB + 4},{_tmp3BC,_tmp3BC,_tmp3BC + 4},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,_tmp3C3,_tmp3C3 + 5},{_tmp3C4,_tmp3C4,_tmp3C4 + 17},{_tmp3C5,_tmp3C5,_tmp3C5 + 12},{_tmp3C6,_tmp3C6,_tmp3C6 + 19},{_tmp3C7,_tmp3C7,_tmp3C7 + 21},{_tmp3C8,_tmp3C8,_tmp3C8 + 20},{_tmp3C9,_tmp3C9,_tmp3C9 + 21},{_tmp3CA,_tmp3CA,_tmp3CA + 13},{_tmp3CB,_tmp3CB,_tmp3CB + 15},{_tmp3CC,_tmp3CC,_tmp3CC + 17},{_tmp3CD,_tmp3CD,_tmp3CD + 19},{_tmp3CE,_tmp3CE,_tmp3CE + 12},{_tmp3CF,_tmp3CF,_tmp3CF + 19},{_tmp3D0,_tmp3D0,_tmp3D0 + 17},{_tmp3D1,_tmp3D1,_tmp3D1 + 23},{_tmp3D2,_tmp3D2,_tmp3D2 + 24},{_tmp3D3,_tmp3D3,_tmp3D3 + 15},{_tmp3D4,_tmp3D4,_tmp3D4 + 11},{_tmp3D5,_tmp3D5,_tmp3D5 + 15},{_tmp3D6,_tmp3D6,_tmp3D6 + 10},{_tmp3D7,_tmp3D7,_tmp3D7 + 15},{_tmp3D8,_tmp3D8,_tmp3D8 + 25},{_tmp3D9,_tmp3D9,_tmp3D9 + 5},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 15},{_tmp3DC,_tmp3DC,_tmp3DC + 11},{_tmp3DD,_tmp3DD,_tmp3DD + 22},{_tmp3DE,_tmp3DE,_tmp3DE + 26},{_tmp3DF,_tmp3DF,_tmp3DF + 16},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 24},{_tmp3E2,_tmp3E2,_tmp3E2 + 25},{_tmp3E3,_tmp3E3,_tmp3E3 + 21},{_tmp3E4,_tmp3E4,_tmp3E4 + 22},{_tmp3E5,_tmp3E5,_tmp3E5 + 16},{_tmp3E6,_tmp3E6,_tmp3E6 + 19},{_tmp3E7,_tmp3E7,_tmp3E7 + 25},{_tmp3E8,_tmp3E8,_tmp3E8 + 35},{_tmp3E9,_tmp3E9,_tmp3E9 + 23},{_tmp3EA,_tmp3EA,_tmp3EA + 24},{_tmp3EB,_tmp3EB,_tmp3EB + 18},{_tmp3EC,_tmp3EC,_tmp3EC + 19},{_tmp3ED,_tmp3ED,_tmp3ED + 14},{_tmp3EE,_tmp3EE,_tmp3EE + 19},{_tmp3EF,_tmp3EF,_tmp3EF + 20},{_tmp3F0,_tmp3F0,_tmp3F0 + 14},{_tmp3F1,_tmp3F1,_tmp3F1 + 11},{_tmp3F2,_tmp3F2,_tmp3F2 + 23},{_tmp3F3,_tmp3F3,_tmp3F3 + 18},{_tmp3F4,_tmp3F4,_tmp3F4 + 30},{_tmp3F5,_tmp3F5,_tmp3F5 + 8},{_tmp3F6,_tmp3F6,_tmp3F6 + 12},{_tmp3F7,_tmp3F7,_tmp3F7 + 14},{_tmp3F8,_tmp3F8,_tmp3F8 + 13},{_tmp3F9,_tmp3F9,_tmp3F9 + 23},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 18},{_tmp3FC,_tmp3FC,_tmp3FC + 8},{_tmp3FD,_tmp3FD,_tmp3FD + 11},{_tmp3FE,_tmp3FE,_tmp3FE + 20},{_tmp3FF,_tmp3FF,_tmp3FF + 9},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 10},{_tmp403,_tmp403,_tmp403 + 16},{_tmp404,_tmp404,_tmp404 + 11},{_tmp405,_tmp405,_tmp405 + 14},{_tmp406,_tmp406,_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 15},{_tmp408,_tmp408,_tmp408 + 22},{_tmp409,_tmp409,_tmp409 + 16},{_tmp40A,_tmp40A,_tmp40A + 17},{_tmp40B,_tmp40B,_tmp40B + 12},{_tmp40C,_tmp40C,_tmp40C + 18},{_tmp40D,_tmp40D,_tmp40D + 17},{_tmp40E,_tmp40E,_tmp40E + 12},{_tmp40F,_tmp40F,_tmp40F + 16},{_tmp410,_tmp410,_tmp410 + 11},{_tmp411,_tmp411,_tmp411 + 10},{_tmp412,_tmp412,_tmp412 + 14},{_tmp413,_tmp413,_tmp413 + 15},{_tmp414,_tmp414,_tmp414 + 20},{_tmp415,_tmp415,_tmp415 + 27},{_tmp416,_tmp416,_tmp416 + 10},{_tmp417,_tmp417,_tmp417 + 13},{_tmp418,_tmp418,_tmp418 + 18},{_tmp419,_tmp419,_tmp419 + 21},{_tmp41A,_tmp41A,_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 16},{_tmp41C,_tmp41C,_tmp41C + 20},{_tmp41D,_tmp41D,_tmp41D + 15},{_tmp41E,_tmp41E,_tmp41E + 20},{_tmp41F,_tmp41F,_tmp41F + 15},{_tmp420,_tmp420,_tmp420 + 12},{_tmp421,_tmp421,_tmp421 + 20},{_tmp422,_tmp422,_tmp422 + 19},{_tmp423,_tmp423,_tmp423 + 20},{_tmp424,_tmp424,_tmp424 + 21},{_tmp425,_tmp425,_tmp425 + 21},{_tmp426,_tmp426,_tmp426 + 12},{_tmp427,_tmp427,_tmp427 + 17},{_tmp428,_tmp428,_tmp428 + 19},{_tmp429,_tmp429,_tmp429 + 14},{_tmp42A,_tmp42A,_tmp42A + 17},{_tmp42B,_tmp42B,_tmp42B + 23},{_tmp42C,_tmp42C,_tmp42C + 13},{_tmp42D,_tmp42D,_tmp42D + 14},{_tmp42E,_tmp42E,_tmp42E + 16},{_tmp42F,_tmp42F,_tmp42F + 16},{_tmp430,_tmp430,_tmp430 + 8},{_tmp431,_tmp431,_tmp431 + 19},{_tmp432,_tmp432,_tmp432 + 20},{_tmp433,_tmp433,_tmp433 + 14},{_tmp434,_tmp434,_tmp434 + 19},{_tmp435,_tmp435,_tmp435 + 20},{_tmp436,_tmp436,_tmp436 + 11},{_tmp437,_tmp437,_tmp437 + 22},{_tmp438,_tmp438,_tmp438 + 20},{_tmp439,_tmp439,_tmp439 + 23},{_tmp43A,_tmp43A,_tmp43A + 20},{_tmp43B,_tmp43B,_tmp43B + 22},{_tmp43C,_tmp43C,_tmp43C + 23},{_tmp43D,_tmp43D,_tmp43D + 24},{_tmp43E,_tmp43E,_tmp43E + 24},{_tmp43F,_tmp43F,_tmp43F + 15},{_tmp440,_tmp440,_tmp440 + 20},{_tmp441,_tmp441,_tmp441 + 22},{_tmp442,_tmp442,_tmp442 + 17},{_tmp443,_tmp443,_tmp443 + 20},{_tmp444,_tmp444,_tmp444 + 26},{_tmp445,_tmp445,_tmp445 + 16},{_tmp446,_tmp446,_tmp446 + 17},{_tmp447,_tmp447,_tmp447 + 15},{_tmp448,_tmp448,_tmp448 + 19},{_tmp449,_tmp449,_tmp449 + 19},{_tmp44A,_tmp44A,_tmp44A + 25},{_tmp44B,_tmp44B,_tmp44B + 26},{_tmp44C,_tmp44C,_tmp44C + 9},{_tmp44D,_tmp44D,_tmp44D + 20},{_tmp44E,_tmp44E,_tmp44E + 17},{_tmp44F,_tmp44F,_tmp44F + 18},{_tmp450,_tmp450,_tmp450 + 11},{_tmp451,_tmp451,_tmp451 + 12}};
# 1604
static short Cyc_yyr1[515]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,183,183,183,184,185,185,186,186,186,186,187,187,187,188,188,189,189,189,189,190,190,190,191,191,192,192,193,193,194,194,194,194,194,194,194,194,194,194,195,195,195,195,195,195,195,195,195,195,195,196,196,197,198,198,199,199,199,199,199,199,199,199,200,200,200,201,201,202,202,202,203,203,203,204,204,205,205,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,212,212,212,213,213,214,214,215,215,215,216,217,217,218,218,219,219,219,219,220,220,220,220,221,222,222,223,223,224,224,225,225,225,225,225,226,226,227,227,227,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,229,230,230,231,232,232,233,233,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,280,280,280,281,282,282,283,283,283,283,283,284,284,285,285,286,286,287,287,288,288};
# 1659
static short Cyc_yyr2[515]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1714
static short Cyc_yydefact[1030]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,0,0,0,0,0,0,0,38,505,213,507,506,508,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,158,0,183,186,85,162,111,55,54,48,0,99,504,0,505,500,501,502,503,111,0,373,0,0,0,0,236,0,375,376,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,196,197,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,509,510,111,111,37,39,37,0,20,0,215,0,171,159,184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,505,383,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,489,490,473,0,0,0,0,0,476,474,475,0,400,402,416,424,426,428,430,432,434,437,442,445,448,452,0,454,477,488,487,0,384,382,33,0,0,111,0,0,0,129,125,127,254,256,0,0,9,10,0,0,0,111,511,512,214,94,0,0,163,73,234,0,231,0,3,0,5,0,284,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,0,0,0,0,0,505,287,289,0,295,291,0,293,277,278,279,0,280,281,282,0,40,22,122,261,0,221,237,0,0,217,215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,377,0,0,397,0,0,0,0,250,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,0,0,0,0,0,0,455,456,37,0,0,0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,406,407,408,409,410,411,412,413,414,415,405,0,460,0,483,484,0,0,0,492,0,111,390,391,0,388,0,238,0,0,0,0,257,229,0,130,135,131,133,126,128,215,0,263,255,264,514,513,0,87,91,0,89,0,93,109,64,63,0,61,164,215,233,160,263,235,172,173,0,86,200,27,0,29,0,0,0,0,121,123,240,239,23,106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,324,325,326,0,0,328,0,0,0,0,296,292,122,294,290,288,0,170,222,0,0,0,228,216,223,167,0,0,0,217,169,203,202,165,201,0,0,207,59,112,0,105,423,102,98,386,387,253,0,394,249,251,381,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,491,498,0,497,401,425,0,427,429,431,433,435,436,440,441,438,439,443,444,446,447,449,450,451,404,403,482,479,0,481,0,0,0,378,0,385,32,0,379,0,0,258,136,132,134,0,217,0,201,0,265,0,215,0,276,260,0,0,111,0,0,0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,146,155,154,0,0,149,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,37,24,262,215,0,225,0,0,218,0,168,221,209,166,189,188,207,185,0,7,0,252,396,399,461,0,0,242,0,246,465,0,0,0,0,469,472,0,0,0,0,0,453,494,0,0,480,478,0,0,389,392,380,259,230,272,0,266,267,201,0,0,217,201,0,90,0,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,115,144,156,153,153,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,25,217,0,226,224,0,217,0,208,0,8,0,0,0,243,247,0,0,0,0,421,422,482,481,496,0,499,417,493,495,0,271,269,275,274,0,268,201,0,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,344,347,352,355,358,362,364,371,372,0,37,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,227,219,210,215,13,0,14,464,463,0,248,466,0,0,471,470,485,0,273,270,36,0,0,179,177,285,108,0,0,0,303,0,368,0,0,365,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,15,0,0,0,0,486,107,0,157,298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,350,351,348,349,353,354,356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,220,212,16,0,0,0,0,301,367,0,363,306,0,37,314,322,316,317,0,244,467,0,68,0,0,333,303,307,318,0,370,369,308,468,0,0,0};
# 1820
static short Cyc_yydefgoto[143]={1027,50,685,875,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,646,647,648,68,476,69,449,70,71,158,159,72,117,522,316,682,606,73,607,516,673,508,512,513,420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,293,294,295,760,296,297,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,851,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,563,564,216,217,75,876,244,435};
# 1838
static short Cyc_yypact[1030]={2748,- -32768,- -32768,- -32768,- -32768,116,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3328,206,3101,- -32768,32,- 49,- -32768,- 5,136,145,165,162,189,191,340,279,- -32768,- -32768,243,- -32768,- -32768,- -32768,250,487,285,286,312,312,- -32768,- -32768,2611,- -32768,454,536,- -32768,295,732,3328,3328,3328,- -32768,3328,- -32768,- -32768,252,- -32768,32,3247,381,37,684,651,- -32768,- -32768,333,349,359,- -32768,32,371,- -32768,4365,228,- -32768,- -32768,- -32768,- -32768,333,5752,- -32768,367,313,4365,383,389,425,- -32768,72,- -32768,- -32768,3474,3474,2611,2611,3474,429,5752,337,- -32768,79,434,565,459,79,3693,5752,- -32768,- -32768,- -32768,2611,2884,2611,2884,73,- -32768,469,474,- -32768,3165,- -32768,- -32768,- -32768,- -32768,3693,- -32768,- -32768,333,139,1623,- -32768,3247,732,- -32768,3474,3401,4122,- -32768,381,- -32768,- -32768,- -32768,484,486,- -32768,- -32768,- -32768,- -32768,- 28,651,3474,- -32768,2884,- -32768,496,510,504,32,99,- -32768,4365,- 15,958,5957,551,5752,5801,592,598,608,632,659,661,663,681,6006,6006,- -32768,- -32768,- -32768,2190,685,6055,6055,6055,- -32768,- -32768,- -32768,299,- -32768,- -32768,- 6,641,545,682,631,606,130,658,234,123,- -32768,1171,6055,193,- 24,- -32768,712,6196,735,- -32768,- -32768,740,5752,333,6196,727,220,3693,1096,3693,605,- -32768,- 20,- 20,- -32768,- -32768,50,744,323,333,- -32768,- -32768,- -32768,- -32768,71,730,- -32768,- -32768,647,373,- -32768,753,- -32768,777,- -32768,784,768,797,- -32768,565,4220,3247,- -32768,791,3693,- -32768,709,801,32,806,803,75,805,1324,816,811,809,824,4318,1324,218,820,818,- -32768,- -32768,823,1766,1766,732,1766,- -32768,- -32768,- -32768,829,- -32768,- -32768,- -32768,121,- -32768,- -32768,832,815,74,848,- -32768,11,836,826,284,837,701,828,3474,5752,- -32768,842,- -32768,- -32768,74,852,32,- -32768,5752,854,- -32768,833,851,- -32768,79,5752,4365,260,- -32768,- -32768,- -32768,856,857,2190,- -32768,3693,- -32768,- -32768,3730,- -32768,886,5752,5752,5752,5752,5752,5752,3693,859,2190,- -32768,- -32768,1909,853,377,5752,- -32768,- -32768,- -32768,5752,- -32768,6055,5752,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,5752,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5752,- -32768,79,- -32768,- -32768,4413,79,5752,- -32768,868,333,- -32768,- -32768,867,861,4365,- -32768,311,958,871,3474,- -32768,873,860,- -32768,1096,1096,1096,- -32768,- -32768,2958,4508,157,- -32768,500,- -32768,- -32768,11,- -32768,- -32768,3474,- -32768,894,- -32768,881,- -32768,876,877,883,- -32768,1173,- -32768,535,654,- -32768,- -32768,- -32768,3693,- -32768,- -32768,- -32768,2611,- -32768,2611,897,895,887,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,773,5752,898,891,- -32768,888,122,333,333,831,5752,5752,890,900,5752,991,2052,904,- -32768,- -32768,- -32768,390,982,- -32768,4606,5752,1324,2328,- -32768,- -32768,3165,- -32768,- -32768,- -32768,3474,- -32768,- -32768,3693,899,267,- -32768,- -32768,889,- -32768,11,903,3620,826,- -32768,- -32768,- -32768,- -32768,701,34,905,863,- -32768,- -32768,2474,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,901,- -32768,- -32768,- -32768,- -32768,2994,906,909,929,- -32768,- -32768,728,4220,910,914,917,919,393,915,920,562,921,923,5879,- -32768,- -32768,924,928,- -32768,641,272,545,682,631,606,130,130,658,658,658,658,234,234,123,123,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,926,- -32768,- 26,3474,4024,- -32768,6235,- -32768,- -32768,933,- -32768,98,943,- -32768,- -32768,- -32768,- -32768,930,826,931,701,927,500,3474,3547,4701,- -32768,- -32768,- 20,936,333,770,940,71,3031,944,333,3474,3401,4799,- -32768,535,- -32768,- -32768,- -32768,946,- -32768,968,893,- -32768,- -32768,709,5752,826,- -32768,- -32768,- -32768,954,32,506,399,419,5752,841,436,950,4897,4992,520,- -32768,- -32768,957,960,953,478,- -32768,3247,- -32768,815,962,3474,- -32768,967,11,- -32768,966,- -32768,175,- -32768,- -32768,- -32768,- -32768,863,- -32768,963,- -32768,2611,- -32768,- -32768,- -32768,973,208,970,- -32768,3828,- -32768,- -32768,5752,1070,5752,5801,- -32768,- -32768,79,79,973,969,4024,- -32768,- -32768,5752,5752,- -32768,- -32768,74,750,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,972,- -32768,- -32768,701,74,974,826,701,985,- -32768,5752,826,516,975,976,- -32768,- -32768,992,- -32768,74,993,826,995,701,990,5752,1006,- -32768,- -32768,3693,- -32768,1000,48,831,1324,1007,1001,6160,999,1009,1324,5752,5087,547,5182,557,5277,841,- -32768,1012,1015,- -32768,826,112,- -32768,- -32768,1022,826,3693,- -32768,249,- -32768,1010,1011,5752,- -32768,- -32768,4220,1013,1016,1017,- -32768,886,1014,1019,- -32768,762,- -32768,- -32768,- -32768,- -32768,4024,- -32768,- -32768,- -32768,- -32768,1020,- -32768,701,588,3693,1036,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1021,- -32768,- -32768,701,527,- -32768,1028,3693,- -32768,- -32768,1122,841,1032,6104,1029,2328,6055,1026,- -32768,- 3,- -32768,1066,1023,756,825,134,834,268,347,- -32768,- -32768,- -32768,- -32768,1067,6055,1909,- -32768,- -32768,531,1324,541,5372,1324,575,5467,5562,589,1038,- -32768,- -32768,- -32768,- -32768,1040,- -32768,962,- -32768,1039,656,- -32768,- -32768,324,- -32768,- -32768,3693,1138,- -32768,- -32768,- -32768,3926,- -32768,- -32768,- -32768,1044,1045,- -32768,- -32768,- -32768,- -32768,620,1324,1048,841,2190,- -32768,3693,1043,- -32768,1480,6055,5752,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,5752,- -32768,- -32768,1047,- -32768,1324,1324,645,- -32768,1324,1324,695,1324,706,5657,- -32768,11,826,- -32768,- -32768,337,5752,1050,1046,- -32768,- -32768,1064,- -32768,- -32768,- -32768,1053,1051,1057,6055,841,- -32768,641,334,545,682,682,606,130,130,658,658,658,658,234,234,123,123,- -32768,- -32768,- -32768,356,- -32768,- -32768,- -32768,1324,- -32768,- -32768,1324,- -32768,1324,1324,707,- -32768,- -32768,- -32768,802,1071,3693,1072,- -32768,973,293,- -32768,- -32768,5752,1480,- -32768,- -32768,- -32768,- -32768,1324,- -32768,- -32768,1073,- -32768,1075,1076,- -32768,841,- -32768,- -32768,1077,- -32768,- -32768,- -32768,- -32768,1197,1206,- -32768};
# 1944
static short Cyc_yypgoto[143]={- -32768,176,- -32768,261,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 46,- -32768,- 84,16,- -32768,- -32768,0,586,- -32768,91,- 177,1097,5,- -32768,- -32768,- 90,- -32768,194,1177,- 695,- -32768,- -32768,- -32768,956,952,697,480,- -32768,- -32768,578,- -32768,- -32768,187,- -32768,- -32768,283,- 162,1149,- 371,69,- -32768,1061,- -32768,- -32768,1175,- 87,- -32768,540,- 99,- 133,- 143,- 334,287,550,556,- 418,- 469,- 114,- 317,- 95,- -32768,- 258,- 149,- 532,- 319,- -32768,902,- 184,- 94,- 45,20,- 240,55,- -32768,- -32768,- -32768,- 59,- 255,- -32768,- 650,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,8,1008,- -32768,690,821,- -32768,- 8,587,- -32768,- 165,- 296,- 140,- 327,- 348,- 346,865,- 341,- 344,- 362,- 336,- 332,- 175,422,479,896,- -32768,- 331,- -32768,179,415,- 61,27,- 289,- 51};
# 1966
static short Cyc_yytable[6376]={59,250,308,465,136,62,358,233,234,143,341,237,574,575,576,577,616,361,362,363,568,59,140,569,549,317,62,344,572,573,571,96,499,500,537,502,345,78,398,566,536,671,578,579,112,262,307,136,580,581,310,59,423,644,645,825,62,100,59,59,59,714,59,62,62,62,432,62,59,147,263,589,509,62,130,131,132,324,133,630,405,198,300,140,142,39,366,448,442,907,169,60,137,288,419,299,364,674,326,239,315,220,559,59,59,433,630,712,62,62,587,406,60,327,590,319,891,434,866,101,59,59,59,59,39,62,62,62,62,59,367,510,719,908,62,240,432,631,38,59,149,59,60,38,62,142,62,41,147,60,60,60,41,60,543,289,544,142,531,60,432,679,59,44,662,531,518,62,556,- 150,558,434,437,511,797,433,638,443,325,257,899,38,359,436,677,241,438,434,453,611,41,668,224,242,60,60,505,258,225,433,631,482,97,468,483,170,453,582,583,584,611,60,60,60,60,414,299,374,375,524,60,915,916,171,76,718,409,119,38,422,60,530,60,409,424,434,40,41,42,870,504,288,288,364,288,423,423,423,471,434,956,430,506,60,376,377,961,382,917,918,102,97,59,160,103,383,384,62,528,451,269,492,97,721,549,399,400,401,142,235,236,507,172,427,104,624,105,59,59,428,59,694,62,62,306,62,253,254,255,256,429,511,- 215,289,289,- 215,289,531,750,624,1004,288,38,106,241,783,527,107,450,402,757,41,242,600,403,404,79,113,109,486,359,268,270,39,170,493,477,494,320,538,129,418,554,617,495,749,359,- 237,110,97,- 237,97,171,60,124,38,1020,59,38,567,19,20,62,40,41,42,40,41,42,1025,874,380,134,289,788,381,111,419,60,60,39,60,511,539,250,707,625,549,670,453,114,329,330,807,364,246,591,97,811,241,1016,784,710,306,97,921,517,242,819,922,453,667,115,794,795,74,453,665,415,219,596,364,422,422,422,301,365,424,424,424,597,615,116,364,441,787,678,80,98,869,99,656,666,38,872,364,364,623,60,629,440,40,41,42,947,140,364,160,59,74,59,738,873,62,1005,62,74,163,419,162,727,637,74,649,650,738,531,653,364,657,135,923,138,788,742,74,1006,59,663,924,925,167,62,218,166,454,429,98,43,364,455,59,221,142,561,144,62,145,98,659,97,222,364,146,726,364,450,142,74,74,699,364,723,135,674,773,755,74,741,59,880,681,743,778,62,74,74,74,74,511,477,364,238,301,787,799,756,713,223,60,792,60,664,970,971,972,973,74,364,793,245,964,625,761,965,966,730,725,788,968,969,967,501,120,121,774,453,74,60,453,962,740,166,974,975,98,247,98,259,976,977,97,60,38,97,339,260,342,342,928,364,593,41,716,43,771,772,355,356,994,313,615,314,44,342,342,342,43,321,299,60,47,48,49,612,753,613,323,754,787,629,322,614,98,632,342,633,804,812,767,98,808,364,813,702,400,401,764,766,364,904,960,43,364,895,122,123,821,929,626,905,627,800,364,452,59,860,628,931,364,62,38,642,643,805,74,863,340,927,364,41,142,452,369,479,59,681,484,402,817,62,44,477,703,404,372,373,364,948,47,48,49,935,252,74,684,477,890,940,477,364,364,897,1017,150,151,152,957,346,958,136,97,889,810,347,153,154,155,156,157,943,427,368,312,348,894,166,428,823,47,48,49,454,378,379,342,98,953,978,979,980,1019,342,38,857,859,60,862,349,865,38,40,41,42,520,521,852,364,40,41,42,371,985,447,97,945,60,879,946,428,447,47,48,49,1003,350,428,351,342,352,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,353,288,511,829,360,249,732,1014,38,856,364,47,48,49,370,988,40,41,42,198,98,364,364,98,267,407,990,1011,471,644,645,38,472,1018,412,249,47,48,49,413,41,467,692,693,125,933,417,59,937,939,446,44,62,288,782,452,758,759,452,47,48,49,439,289,801,802,74,456,74,733,734,735,38,16,17,18,886,887,136,452,40,41,42,43,461,452,359,342,911,912,81,457,471,963,525,603,604,605,459,59,47,48,49,463,62,469,930,913,914,934,488,981,919,920,478,289,1012,364,421,480,426,489,481,467,485,992,550,551,552,553,97,555,996,827,828,487,490,74,60,496,562,497,498,503,565,434,954,507,261,515,98,534,43,288,523,267,514,519,38,84,187,188,85,86,87,585,529,41,533,535,541,366,542,560,602,342,595,586,354,983,984,191,562,986,987,592,989,594,601,81,60,599,38,618,619,620,634,621,622,59,40,41,42,98,62,636,640,635,610,651,639,641,471,652,654,289,658,660,669,672,47,48,49,675,690,680,689,686,691,696,695,452,697,1007,698,700,1008,705,1009,1010,701,704,82,709,708,452,711,717,452,39,720,722,731,724,328,752,342,168,84,1021,736,85,86,87,739,746,41,747,751,762,768,88,769,770,306,562,781,89,562,90,91,775,329,330,777,92,706,785,790,796,60,93,803,74,806,814,815,7,8,9,10,11,12,13,14,15,16,17,18,19,20,809,816,818,342,820,822,249,824,826,830,831,854,855,867,342,22,868,871,467,877,878,883,881,884,882,892,249,27,885,888,893,896,898,249,900,28,29,903,906,909,926,941,944,910,942,949,32,951,982,952,34,955,959,999,998,98,1000,35,36,997,1001,1002,467,7,8,9,10,11,12,13,14,15,16,17,18,19,20,249,135,1028,1013,1015,1022,729,1023,1024,1026,249,1029,995,737,39,22,248,108,43,249,745,464,470,748,148,318,780,27,118,45,46,779,385,776,993,28,29,688,416,540,570,598,853,562,0,0,32,0,0,0,34,0,0,0,557,0,0,35,36,902,0,0,342,0,0,0,0,0,0,386,387,388,389,390,391,392,393,394,395,0,342,304,0,0,38,467,0,0,789,0,791,39,40,41,42,43,0,306,467,396,0,798,0,0,447,0,45,46,0,0,428,0,47,48,49,249,0,0,0,0,0,0,0,0,0,421,0,0,0,249,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,173,174,271,0,272,273,274,275,276,277,278,279,135,81,0,175,280,0,0,176,0,281,0,0,0,0,467,177,178,179,180,0,342,0,0,181,0,282,0,0,467,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,267,0,0,190,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,467,0,249,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,758,759,21,173,174,271,267,272,273,274,275,276,277,278,279,22,81,23,175,280,0,249,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 303,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,687,0,32,168,84,0,34,85,86,87,0,0,41,35,36,0,0,88,0,0,0,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,0,0,38,0,81,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,139,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,173,174,0,0,0,0,32,545,0,0,34,0,0,81,0,175,0,35,36,176,0,0,0,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,39,40,0,42,43,0,0,185,186,0,0,0,0,0,0,45,46,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,546,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,786,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,950,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,82,176,194,195,0,0,196,197,0,177,178,179,180,0,0,168,84,181,0,85,86,87,0,0,41,0,0,182,183,88,184,0,0,0,0,89,0,90,91,0,185,186,0,92,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,744,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,763,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,765,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,858,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,861,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,864,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,932,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,938,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,991,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,832,833,185,186,192,0,0,193,0,0,194,195,0,81,196,197,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,901,0,0,191,81,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,84,0,0,85,86,87,81,0,41,0,0,0,0,88,0,0,0,0,0,834,82,90,91,0,0,0,0,92,0,0,835,0,408,93,195,168,84,196,197,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,82,0,0,0,92,0,0,0,0,0,93,0,715,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93};
# 2607
static short Cyc_yycheck[6376]={0,115,145,261,65,0,190,101,102,68,175,105,374,375,376,377,434,192,193,194,368,21,68,369,343,158,21,176,372,373,371,23,287,288,330,290,176,21,213,366,329,510,378,379,44,129,145,108,380,381,145,51,229,5,6,750,51,106,58,59,60,593,62,58,59,60,86,62,68,69,129,402,61,68,58,59,60,167,62,450,104,89,141,129,68,113,92,249,17,92,82,0,65,139,227,141,122,515,113,107,128,93,357,103,104,125,477,133,103,104,399,135,21,128,403,160,811,137,768,124,120,121,122,123,113,120,121,122,123,129,136,120,601,136,129,108,86,454,106,139,71,141,51,106,139,129,141,115,148,58,59,60,115,62,338,139,340,141,323,68,86,127,162,126,495,330,309,162,352,121,354,137,122,306,706,125,472,106,170,106,830,106,190,234,518,106,237,137,249,429,115,508,120,114,103,104,122,124,126,125,517,126,23,262,129,106,267,382,383,384,450,120,121,122,123,223,262,87,88,313,129,87,88,124,108,127,218,51,106,229,139,321,141,225,229,137,114,115,116,127,119,287,288,122,290,422,423,424,126,137,900,231,303,162,124,125,906,134,124,125,124,82,262,124,119,142,143,262,319,249,131,279,93,607,593,82,83,84,262,103,104,106,88,126,119,447,124,287,288,132,290,549,287,288,119,290,120,121,122,123,231,434,127,287,288,130,290,472,642,471,960,357,106,124,106,107,316,126,249,126,651,115,114,417,131,132,120,44,49,274,338,137,138,113,106,280,267,119,162,331,57,121,350,437,126,641,354,119,105,170,122,172,124,262,59,106,1006,357,106,367,20,21,357,114,115,116,114,115,116,1019,121,137,120,357,693,141,126,510,287,288,113,290,515,123,498,560,447,706,121,450,105,131,132,727,122,112,404,218,732,106,107,690,130,119,225,137,122,114,742,141,471,505,126,702,703,0,477,501,224,106,412,122,422,423,424,142,127,422,423,424,119,431,120,122,240,693,523,22,23,773,25,487,501,106,778,122,122,447,357,449,127,114,115,116,130,501,122,124,458,44,460,623,779,458,130,460,51,108,601,120,613,471,57,481,482,637,641,485,122,487,65,134,67,802,627,70,130,487,496,142,143,120,487,126,79,122,427,82,117,122,127,501,119,487,127,124,501,126,93,119,331,122,122,132,613,122,447,501,103,104,127,122,609,108,942,668,127,112,627,529,788,526,627,676,529,120,121,122,123,672,471,122,113,260,802,710,127,592,123,458,699,460,497,915,916,917,918,142,122,699,126,909,623,127,910,911,617,612,887,913,914,912,289,119,120,669,637,162,487,640,907,626,167,919,920,170,127,172,119,921,922,412,501,106,415,173,122,175,176,854,122,407,115,595,117,127,665,185,186,943,126,611,126,126,192,193,194,117,122,665,529,134,135,136,124,119,126,123,122,887,630,121,132,218,458,213,460,724,122,119,225,728,122,127,82,83,84,655,656,122,834,906,117,122,127,119,120,744,127,124,835,126,713,122,249,665,119,132,127,122,665,106,478,479,725,260,119,126,853,122,115,665,267,138,269,685,681,272,126,740,685,126,623,131,132,89,90,122,882,134,135,136,127,116,289,529,637,119,119,640,122,122,826,1002,63,64,65,901,126,903,781,542,809,731,126,74,75,76,77,78,873,126,91,146,126,822,321,132,746,134,135,136,122,85,86,323,331,127,923,924,925,1006,330,106,762,763,665,765,126,767,106,114,115,116,63,64,758,122,114,115,116,140,127,126,595,119,685,785,122,132,126,134,135,136,959,126,132,126,366,126,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,126,854,942,755,126,115,619,998,106,761,122,134,135,136,139,127,114,115,116,834,412,122,122,415,134,120,127,127,126,5,6,106,130,1005,106,145,134,135,136,106,115,261,121,122,119,860,126,854,863,864,127,126,854,906,685,447,22,23,450,134,135,136,125,854,121,122,458,121,460,106,107,108,106,17,18,19,121,122,946,471,114,115,116,117,123,477,901,472,139,140,38,121,126,908,314,422,423,424,121,906,134,135,136,113,906,121,858,89,90,861,106,926,85,86,120,906,121,122,228,120,230,119,126,343,126,940,346,347,348,349,758,351,947,753,754,126,119,529,854,126,360,130,126,121,364,137,898,106,123,130,542,125,117,1006,133,265,127,127,106,107,108,109,110,111,112,385,121,115,121,125,121,92,122,127,121,560,122,397,126,931,932,129,402,935,936,124,938,127,122,38,906,127,106,106,120,126,106,127,122,1006,114,115,116,595,1006,125,122,119,428,126,119,130,126,120,30,1006,119,42,126,137,134,135,136,127,122,127,127,133,106,122,127,623,122,985,122,127,988,121,990,991,127,127,91,122,127,637,127,121,640,113,127,127,123,133,103,647,641,106,107,1011,127,110,111,112,127,126,115,106,121,126,120,120,119,127,119,495,120,126,498,128,129,121,131,132,125,134,120,124,25,127,1006,140,127,685,127,127,127,8,9,10,11,12,13,14,15,16,17,18,19,20,21,133,127,127,699,127,133,427,119,126,120,127,130,121,119,710,37,119,113,549,127,127,122,127,127,126,107,447,49,127,127,127,121,28,454,120,57,58,126,130,91,91,121,121,138,122,25,68,121,119,122,72,121,127,107,126,758,121,79,80,127,127,122,593,8,9,10,11,12,13,14,15,16,17,18,19,20,21,498,781,0,127,127,127,614,127,127,127,508,0,946,622,113,37,114,35,117,517,628,260,265,640,70,159,681,49,48,128,129,676,56,672,942,57,58,542,225,332,370,415,758,651,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,353,- 1,- 1,79,80,832,- 1,- 1,835,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,102,- 1,853,103,- 1,- 1,106,693,- 1,- 1,696,- 1,698,113,114,115,116,117,- 1,119,706,123,- 1,709,- 1,- 1,126,- 1,128,129,- 1,- 1,132,- 1,134,135,136,613,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,623,- 1,- 1,- 1,627,- 1,- 1,- 1,- 1,907,- 1,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,946,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,788,52,53,54,55,- 1,959,- 1,- 1,60,- 1,62,- 1,- 1,802,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1005,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,750,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,887,- 1,779,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,811,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,826,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,- 1,72,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,68,32,- 1,- 1,72,- 1,- 1,38,- 1,40,- 1,79,80,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,113,114,- 1,116,117,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,91,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,106,107,60,- 1,110,111,112,- 1,- 1,115,- 1,- 1,70,71,120,73,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,83,84,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,25,26,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,38,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,38,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,111,112,38,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,91,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,91,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,103,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple26{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple27{struct _dyneither_ptr f1;void*f2;};static char _tmp502[8]="stdcall";static char _tmp503[6]="cdecl";static char _tmp504[9]="fastcall";static char _tmp505[9]="noreturn";static char _tmp506[6]="const";static char _tmp507[8]="aligned";static char _tmp508[7]="packed";static char _tmp509[7]="shared";static char _tmp50A[7]="unused";static char _tmp50B[5]="weak";static char _tmp50C[10]="dllimport";static char _tmp50D[10]="dllexport";static char _tmp50E[23]="no_instrument_function";static char _tmp50F[12]="constructor";static char _tmp510[11]="destructor";static char _tmp511[22]="no_check_memory_usage";static char _tmp512[5]="pure";struct _tuple28{void*f1;void*f2;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9D7(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9D6,unsigned int*_tmp9D5,union Cyc_YYSTYPE**_tmp9D4){for(*_tmp9D6=0;*_tmp9D6 < *_tmp9D5;(*_tmp9D6)++){(*_tmp9D4)[*_tmp9D6]=*yylval;}}static void _tmp9DC(unsigned int*_tmp9DB,unsigned int*_tmp9DA,struct Cyc_Yyltype**_tmp9D9){for(*_tmp9DB=0;*_tmp9DB < *_tmp9DA;(*_tmp9DB)++){(*_tmp9D9)[*_tmp9DB]=
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9E3(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9E2,unsigned int*_tmp9E1,short**_tmp9DF){for(*_tmp9E2=0;*_tmp9E2 < *_tmp9E1;(*_tmp9E2)++){(*_tmp9DF)[*_tmp9E2]=
# 216
*_tmp9E2 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9E2)): 0;}}static void _tmp9E9(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9E8,unsigned int*_tmp9E7,union Cyc_YYSTYPE**_tmp9E5){for(*_tmp9E8=0;*_tmp9E8 < *_tmp9E7;(*_tmp9E8)++){(*_tmp9E5)[*_tmp9E8]=
# 219
*_tmp9E8 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9E8)):*yylval;}}static void _tmp9EF(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9EE,unsigned int*_tmp9ED,struct Cyc_Yyltype**_tmp9EB){for(*_tmp9EE=0;*_tmp9EE < *_tmp9ED;(*_tmp9EE)++){(*_tmp9EB)[*_tmp9EE]=
# 222
*_tmp9EE <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9EE)):
# 223
 Cyc_yynewloc();}}static void _tmpEBA(unsigned int*_tmpEB9,unsigned int*_tmpEB8,char**_tmpEB6){for(*_tmpEB9=0;*_tmpEB9 < *_tmpEB8;(*_tmpEB9)++){(*_tmpEB6)[*_tmpEB9]='\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp453=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp453;_push_region(yyregion);
# 141
{int yystate;
# 142
int yyn=0;
# 143
int yyerrstatus;
# 144
int yychar1=0;
# 146
int yychar;
# 147
union Cyc_YYSTYPE _tmp9D0;union Cyc_YYSTYPE yylval=((_tmp9D0.YYINITIALSVAL).val=0,(((_tmp9D0.YYINITIALSVAL).tag=61,_tmp9D0)));
# 148
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9D2;unsigned int _tmp9D1;struct _dyneither_ptr yyss=(_tmp9D1=200,((_tmp9D2=_region_calloc(yyregion,sizeof(short),_tmp9D1),_tag_dyneither(_tmp9D2,sizeof(short),_tmp9D1))));
# 158
int yyvsp_offset;
# 159
unsigned int _tmp9D6;unsigned int _tmp9D5;union Cyc_YYSTYPE*_tmp9D4;unsigned int _tmp9D3;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9D3=(unsigned int)200,((_tmp9D4=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9D3)),((((_tmp9D5=_tmp9D3,_tmp9D7(& yylval,& _tmp9D6,& _tmp9D5,& _tmp9D4))),_tmp9D4)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
# 164
unsigned int _tmp9DB;unsigned int _tmp9DA;struct Cyc_Yyltype*_tmp9D9;unsigned int _tmp9D8;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9D8=(unsigned int)200,((_tmp9D9=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9D8)),((((_tmp9DA=_tmp9D8,_tmp9DC(& _tmp9DB,& _tmp9DA,& _tmp9D9))),_tmp9D9)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
# 184
yyerrstatus=0;
# 185
yynerrs=0;
# 186
yychar=- 2;
# 193
yyssp_offset=- 1;
# 194
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
# 209
{const char*_tmp9DD;Cyc_yyerror(((_tmp9DD="parser stack overflow",_tag_dyneither(_tmp9DD,sizeof(char),22))));}
# 210
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
# 213
if(yystacksize > 10000)
# 214
yystacksize=10000;{
# 215
unsigned int _tmp9E2;unsigned int _tmp9E1;struct _dyneither_ptr _tmp9E0;short*_tmp9DF;unsigned int _tmp9DE;struct _dyneither_ptr yyss1=(_tmp9DE=(unsigned int)yystacksize,((_tmp9DF=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9DE)),((_tmp9E0=_tag_dyneither(_tmp9DF,sizeof(short),_tmp9DE),((((_tmp9E1=_tmp9DE,_tmp9E3(& yyssp_offset,& yyss,& _tmp9E2,& _tmp9E1,& _tmp9DF))),_tmp9E0)))))));
# 217
unsigned int _tmp9E8;unsigned int _tmp9E7;struct _dyneither_ptr _tmp9E6;union Cyc_YYSTYPE*_tmp9E5;unsigned int _tmp9E4;struct _dyneither_ptr yyvs1=
# 218
(_tmp9E4=(unsigned int)yystacksize,((_tmp9E5=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9E4)),((_tmp9E6=_tag_dyneither(_tmp9E5,sizeof(union Cyc_YYSTYPE),_tmp9E4),((((_tmp9E7=_tmp9E4,_tmp9E9(& yyvs,& yyssp_offset,& yylval,& _tmp9E8,& _tmp9E7,& _tmp9E5))),_tmp9E6)))))));
# 221
unsigned int _tmp9EE;unsigned int _tmp9ED;struct _dyneither_ptr _tmp9EC;struct Cyc_Yyltype*_tmp9EB;unsigned int _tmp9EA;struct _dyneither_ptr yyls1=(_tmp9EA=(unsigned int)yystacksize,((_tmp9EB=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9EA)),((_tmp9EC=_tag_dyneither(_tmp9EB,sizeof(struct Cyc_Yyltype),_tmp9EA),((((_tmp9ED=_tmp9EA,_tmp9EF(& yyssp_offset,& yyls,& _tmp9EE,& _tmp9ED,& _tmp9EB))),_tmp9EC)))))));
# 225
yyss=yyss1;
# 226
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
# 253
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
# 272
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 289;}
# 297 "cycbison.simple"
yyn +=yychar1;
# 298
if((yyn < 0  || yyn > 6375) || Cyc_yycheck[_check_known_subscript_notnull(6376,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6376,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
# 310
yyn=- yyn;
# 311
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1029){
# 316
int _tmp461=0;_npop_handler(0);return _tmp461;}
# 326 "cycbison.simple"
if(yychar != 0)
# 327
yychar=- 2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
# 339
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1030,yystate)];
# 345
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(515,yyn)];
# 352
if(yylen > 0)
# 353
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL205: {
# 1170 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1174
yyval=yyyyvsp[0];
# 1175
Cyc_Parse_parse_result=Cyc_yyget_YY18(yyyyvsp[0]);
# 1177
break;}case 2: _LL206: {
# 1176 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1180
struct Cyc_List_List*x;struct Cyc_List_List*y;
# 1181
{struct _handler_cons _tmp462;_push_handler(& _tmp462);{int _tmp464=0;if(setjmp(_tmp462.handler))_tmp464=1;if(!_tmp464){
# 1182
x=Cyc_yyget_YY18(yyyyvsp[0]);;_pop_handler();}else{void*_tmp463=(void*)_exn_thrown;void*_tmp466=_tmp463;_LL209: {struct Cyc_Core_Failure_exn_struct*_tmp467=(struct Cyc_Core_Failure_exn_struct*)_tmp466;if(_tmp467->tag != Cyc_Core_Failure)goto _LL20B;}_LL20A:
# 1185
 x=0;goto _LL208;_LL20B:;_LL20C:(void)_throw(_tmp466);_LL208:;}};}
# 1187
{struct _handler_cons _tmp468;_push_handler(& _tmp468);{int _tmp46A=0;if(setjmp(_tmp468.handler))_tmp46A=1;if(!_tmp46A){
# 1188
y=Cyc_yyget_YY18(yyyyvsp[1]);;_pop_handler();}else{void*_tmp469=(void*)_exn_thrown;void*_tmp46C=_tmp469;_LL20E: {struct Cyc_Core_Failure_exn_struct*_tmp46D=(struct Cyc_Core_Failure_exn_struct*)_tmp46C;if(_tmp46D->tag != Cyc_Core_Failure)goto _LL210;}_LL20F:
# 1191
 y=0;goto _LL20D;_LL210:;_LL211:(void)_throw(_tmp46C);_LL20D:;}};}
# 1193
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1195
break;}case 3: _LL207: {
# 1194 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1198
{struct Cyc_Absyn_Decl*_tmp9F9;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9F8;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9F7;struct Cyc_List_List*_tmp9F6;yyval=Cyc_YY18(((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6->hd=((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9->r=(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=11,((_tmp9F8.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9F8.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9F8)))))),_tmp9F7)))),((_tmp9F9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F9)))))),((_tmp9F6->tl=0,_tmp9F6)))))));}
# 1199
Cyc_Lex_leave_using();
# 1201
break;}case 4: _LL212: {
# 1198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1202
{struct Cyc_Absyn_Decl*_tmpA03;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA02;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA01;struct Cyc_List_List*_tmpA00;yyval=Cyc_YY18(((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->hd=((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->r=(void*)((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=11,((_tmpA02.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA02.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA02)))))),_tmpA01)))),((_tmpA03->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA03)))))),((_tmpA00->tl=Cyc_yyget_YY18(yyyyvsp[4]),_tmpA00)))))));}
# 1203
break;}case 5: _LL213: {
# 1201 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1205
{struct Cyc_Absyn_Decl*_tmpA12;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA11;struct _dyneither_ptr*_tmpA10;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA0F;struct Cyc_List_List*_tmpA0E;yyval=Cyc_YY18(((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->hd=((_tmpA12=_cycalloc(sizeof(*_tmpA12)),((_tmpA12->r=(void*)((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=((_tmpA11.tag=10,((_tmpA11.f1=((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA10)))),((_tmpA11.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA11)))))),_tmpA0F)))),((_tmpA12->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA12)))))),((_tmpA0E->tl=0,_tmpA0E)))))));}
# 1206
Cyc_Lex_leave_namespace();
# 1208
break;}case 6: _LL214: {
# 1205 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1210
struct _dyneither_ptr nspace;
# 1211
struct Cyc_List_List*x;struct Cyc_List_List*y;
# 1212
{struct _handler_cons _tmp47B;_push_handler(& _tmp47B);{int _tmp47D=0;if(setjmp(_tmp47B.handler))_tmp47D=1;if(!_tmp47D){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp47C=(void*)_exn_thrown;void*_tmp47F=_tmp47C;_LL217: {struct Cyc_Core_Failure_exn_struct*_tmp480=(struct Cyc_Core_Failure_exn_struct*)_tmp47F;if(_tmp480->tag != Cyc_Core_Failure)goto _LL219;}_LL218:
# 1213
{const char*_tmpA13;nspace=((_tmpA13="",_tag_dyneither(_tmpA13,sizeof(char),1)));}goto _LL216;_LL219:;_LL21A:(void)_throw(_tmp47F);_LL216:;}};}
# 1215
{struct _handler_cons _tmp482;_push_handler(& _tmp482);{int _tmp484=0;if(setjmp(_tmp482.handler))_tmp484=1;if(!_tmp484){x=Cyc_yyget_YY18(yyyyvsp[2]);;_pop_handler();}else{void*_tmp483=(void*)_exn_thrown;void*_tmp486=_tmp483;_LL21C: {struct Cyc_Core_Failure_exn_struct*_tmp487=(struct Cyc_Core_Failure_exn_struct*)_tmp486;if(_tmp487->tag != Cyc_Core_Failure)goto _LL21E;}_LL21D:
# 1216
 x=0;goto _LL21B;_LL21E:;_LL21F:(void)_throw(_tmp486);_LL21B:;}};}
# 1218
{struct _handler_cons _tmp488;_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=1;if(!_tmp48A){y=Cyc_yyget_YY18(yyyyvsp[4]);;_pop_handler();}else{void*_tmp489=(void*)_exn_thrown;void*_tmp48C=_tmp489;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp48D=(struct Cyc_Core_Failure_exn_struct*)_tmp48C;if(_tmp48D->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
# 1219
 y=0;goto _LL220;_LL223:;_LL224:(void)_throw(_tmp48C);_LL220:;}};}
# 1221
{struct Cyc_Absyn_Decl*_tmpA22;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA21;struct _dyneither_ptr*_tmpA20;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA1F;struct Cyc_List_List*_tmpA1E;yyval=Cyc_YY18(((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->hd=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->r=(void*)((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA21.tag=10,((_tmpA21.f1=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20[0]=nspace,_tmpA20)))),((_tmpA21.f2=x,_tmpA21)))))),_tmpA1F)))),((_tmpA22->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA22)))))),((_tmpA1E->tl=y,_tmpA1E)))))));}
# 1223
break;}case 7: _LL215: {
# 1220 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1224
struct _dyneither_ptr _tmp493=Cyc_yyget_String_tok(yyyyvsp[1]);
# 1225
{const char*_tmpA23;if(Cyc_strcmp((struct _dyneither_ptr)_tmp493,((_tmpA23="C",_tag_dyneither(_tmpA23,sizeof(char),2))))== 0){
# 1226
struct Cyc_Absyn_Decl*_tmpA2D;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA2C;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA2B;struct Cyc_List_List*_tmpA2A;yyval=Cyc_YY18(((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->hd=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->r=(void*)((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2C.tag=12,((_tmpA2C.f1=Cyc_yyget_YY18(yyyyvsp[3]),_tmpA2C)))),_tmpA2B)))),((_tmpA2D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2D)))))),((_tmpA2A->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmpA2A)))))));}else{
# 1228
{const char*_tmpA2E;if(Cyc_strcmp((struct _dyneither_ptr)_tmp493,((_tmpA2E="C include",_tag_dyneither(_tmpA2E,sizeof(char),10))))!= 0){
# 1229
const char*_tmpA2F;Cyc_Parse_err(((_tmpA2F="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA2F,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1231
struct Cyc_Absyn_Decl*_tmpA39;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA38;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA37;struct Cyc_List_List*_tmpA36;yyval=Cyc_YY18(((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39->r=(void*)((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA38.tag=13,((_tmpA38.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmpA38.f2=0,_tmpA38)))))),_tmpA37)))),((_tmpA39->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA39)))))),((_tmpA36->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmpA36)))))));};}}
# 1234
break;}case 8: _LL225: {
# 1231 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1235
{const char*_tmpA3A;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA3A="C include",_tag_dyneither(_tmpA3A,sizeof(char),10))))!= 0){
# 1236
const char*_tmpA3B;Cyc_Parse_err(((_tmpA3B="expecting \"C include\"",_tag_dyneither(_tmpA3B,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1237
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[5]);
# 1238
{struct Cyc_Absyn_Decl*_tmpA45;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA44;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY18(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->r=(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=13,((_tmpA44.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmpA44.f2=exs,_tmpA44)))))),_tmpA43)))),((_tmpA45->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA45)))))),((_tmpA42->tl=Cyc_yyget_YY18(yyyyvsp[6]),_tmpA42)))))));}
# 1240
break;};}case 9: _LL226: {
# 1237 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1241
{struct Cyc_Absyn_Decl*_tmpA48;struct Cyc_List_List*_tmpA47;yyval=Cyc_YY18(((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47->hd=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA48->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA48)))))),((_tmpA47->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA47)))))));}
# 1242
break;}case 10: _LL227: {
# 1239 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243
{struct Cyc_Absyn_Decl*_tmpA4B;struct Cyc_List_List*_tmpA4A;yyval=Cyc_YY18(((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->hd=((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA4B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4B)))))),((_tmpA4A->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA4A)))))));}
# 1244
break;}case 11: _LL228:
# 1240 "parse.y"
 yyval=Cyc_YY18(0);
# 1241
break;case 12: _LL229: {
# 1244 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1248
yyval=yyyyvsp[2];
# 1249
break;}case 13: _LL22A: {
# 1245 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1249
yyval=Cyc_YY52(0);
# 1250
break;}case 14: _LL22B: {
# 1249 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1253
{struct _tuple26*_tmpA4E;struct Cyc_List_List*_tmpA4D;yyval=Cyc_YY52(((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->hd=((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA4E->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA4E->f3=0,_tmpA4E)))))))),((_tmpA4D->tl=0,_tmpA4D)))))));}
# 1254
break;}case 15: _LL22C: {
# 1250 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1254
{struct _tuple26*_tmpA51;struct Cyc_List_List*_tmpA50;yyval=Cyc_YY52(((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->hd=((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA51->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA51->f3=0,_tmpA51)))))))),((_tmpA50->tl=0,_tmpA50)))))));}
# 1255
break;}case 16: _LL22D: {
# 1252 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1256
{struct _tuple26*_tmpA54;struct Cyc_List_List*_tmpA53;yyval=Cyc_YY52(((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53->hd=((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA54->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA54->f3=0,_tmpA54)))))))),((_tmpA53->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA53)))))));}
# 1257
break;}case 17: _LL22E: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1260
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA5A;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA59;struct Cyc_List_List*_tmpA58;yyval=Cyc_YY18(((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->hd=Cyc_Absyn_new_decl((void*)((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA59.tag=1,((_tmpA59.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpA59)))),_tmpA5A)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA58->tl=0,_tmpA58)))))));}
# 1261
break;}case 18: _LL22F: {
# 1257 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1261
yyval=yyyyvsp[0];
# 1262
break;}case 19: _LL230: {
# 1258 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1262
yyval=Cyc_YY18(0);
# 1263
break;}case 20: _LL231: {
# 1263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1267
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY29(yyyyvsp[0]),0,Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 1268
break;}case 21: _LL232: {
# 1265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1269
struct Cyc_Parse_Declaration_spec _tmp4B2=Cyc_yyget_YY19(yyyyvsp[0]);
# 1270
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp4B2,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 1271
break;}case 22: _LL233: {
# 1278 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1]),Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 1283
break;}case 23: _LL234: {
# 1280 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1284
struct Cyc_Parse_Declaration_spec _tmp4B3=Cyc_yyget_YY19(yyyyvsp[0]);
# 1285
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp4B3,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
# 1286
break;}case 24: _LL235: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1292
struct Cyc_Parse_Declaration_spec _tmp4B4=Cyc_yyget_YY19(yyyyvsp[0]);
# 1293
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp4B4,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 1294
break;}case 25: _LL236: {
# 1291 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1295
struct Cyc_Parse_Declaration_spec _tmp4B5=Cyc_yyget_YY19(yyyyvsp[0]);
# 1296
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp4B5,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
# 1297
break;}case 26: _LL237: {
# 1296 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1300
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
# 1301
break;}case 27: _LL238: {
# 1299 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1303
Cyc_Lex_leave_using();
# 1304
break;}case 28: _LL239: {
# 1302 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1306
{struct _dyneither_ptr*_tmpA5B;Cyc_Lex_enter_namespace(((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA5B)))));}yyval=yyyyvsp[1];
# 1307
break;}case 29: _LL23A: {
# 1305 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1309
Cyc_Lex_leave_namespace();
# 1310
break;}case 30: _LL23B: {
# 1311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1315
int _tmp4B7=(yyyylsp[0]).first_line;
# 1316
yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),0,(unsigned int)_tmp4B7,(unsigned int)_tmp4B7));
# 1317
break;}case 31: _LL23C: {
# 1314 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1318
yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY21(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
# 1319
break;}case 32: _LL23D: {
# 1317 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1321
{struct Cyc_List_List*_tmpA5C;yyval=Cyc_YY18(((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA5C->tl=0,_tmpA5C)))))));}
# 1322
break;}case 33: _LL23E: {
# 1319 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323
struct Cyc_List_List*_tmp4B9=0;
# 1324
{struct Cyc_List_List*_tmp4BA=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4BA != 0;_tmp4BA=_tmp4BA->tl){
# 1325
struct _dyneither_ptr*_tmp4BB=(struct _dyneither_ptr*)_tmp4BA->hd;
# 1326
struct _tuple0*_tmpA5D;struct _tuple0*qv=(_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=Cyc_Absyn_Rel_n(0),((_tmpA5D->f2=_tmp4BB,_tmpA5D)))));
# 1327
struct Cyc_Absyn_Vardecl*_tmp4BC=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
# 1328
struct Cyc_List_List*_tmpA5E;_tmp4B9=((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->hd=_tmp4BC,((_tmpA5E->tl=_tmp4B9,_tmpA5E))))));}}
# 1330
_tmp4B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B9);
# 1331
{struct Cyc_List_List*_tmpA5F;yyval=Cyc_YY18(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=Cyc_Absyn_letv_decl(_tmp4B9,(unsigned int)(yyyylsp[0]).first_line),((_tmpA5F->tl=0,_tmpA5F)))))));}
# 1333
break;}case 34: _LL23F: {
# 1332 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336
struct _dyneither_ptr _tmp4C0=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1337
{const char*_tmpA60;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C0,((_tmpA60="`H",_tag_dyneither(_tmpA60,sizeof(char),3))))== 0){
# 1338
const char*_tmpA63;void*_tmpA62;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA62=0,Cyc_aprintf(((_tmpA63="bad occurrence of heap region",_tag_dyneither(_tmpA63,sizeof(char),30))),_tag_dyneither(_tmpA62,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
# 1339
{const char*_tmpA64;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C0,((_tmpA64="`U",_tag_dyneither(_tmpA64,sizeof(char),3))))== 0){
# 1340
const char*_tmpA67;void*_tmpA66;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA66=0,Cyc_aprintf(((_tmpA67="bad occurrence of unique region",_tag_dyneither(_tmpA67,sizeof(char),32))),_tag_dyneither(_tmpA66,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
# 1341
struct _dyneither_ptr*_tmpA6A;struct Cyc_Absyn_Tvar*_tmpA69;struct Cyc_Absyn_Tvar*tv=(_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->name=((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=_tmp4C0,_tmpA6A)))),((_tmpA69->identity=- 1,((_tmpA69->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA69)))))));
# 1342
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA6D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA6C;void*t=(void*)((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6D.tag=2,((_tmpA6D.f1=tv,_tmpA6D)))),_tmpA6C))));
# 1343
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA73;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA72;struct _dyneither_ptr*_tmpA70;struct _tuple0*_tmpA6F;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->f1=Cyc_Absyn_Loc_n,((_tmpA6F->f2=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA70)))),_tmpA6F)))))),(void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA73.tag=15,((_tmpA73.f1=(void*)t,_tmpA73)))),_tmpA72)))),0);
# 1344
{struct Cyc_List_List*_tmpA74;yyval=Cyc_YY18(((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA74->tl=0,_tmpA74)))))));}
# 1346
break;};}case 35: _LL240: {
# 1344 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348
int _tmp4D0=Cyc_yyget_YY31(yyyyvsp[0]);
# 1349
struct _dyneither_ptr _tmp4D1=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1350
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_yyget_YY4(yyyyvsp[3]);
# 1351
{const char*_tmpA75;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D1,((_tmpA75="H",_tag_dyneither(_tmpA75,sizeof(char),2))))== 0){
# 1352
const char*_tmpA76;Cyc_Parse_err(((_tmpA76="bad occurrence of heap region `H",_tag_dyneither(_tmpA76,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
# 1353
{const char*_tmpA77;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D1,((_tmpA77="U",_tag_dyneither(_tmpA77,sizeof(char),2))))== 0){
# 1354
const char*_tmpA78;Cyc_Parse_err(((_tmpA78="bad occurrence of unique region `U",_tag_dyneither(_tmpA78,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
# 1355
if(_tmp4D0  && _tmp4D2 != 0){
# 1356
const char*_tmpA79;Cyc_Parse_err(((_tmpA79="open regions cannot be @resetable",_tag_dyneither(_tmpA79,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
# 1357
struct _dyneither_ptr*_tmpA86;const char*_tmpA85;void*_tmpA84[1];struct Cyc_String_pa_PrintArg_struct _tmpA83;struct Cyc_Absyn_Tvar*_tmpA82;struct Cyc_Absyn_Tvar*tv=(_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->name=((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=(struct _dyneither_ptr)((_tmpA83.tag=0,((_tmpA83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4D1),((_tmpA84[0]=& _tmpA83,Cyc_aprintf(((_tmpA85="`%s",_tag_dyneither(_tmpA85,sizeof(char),4))),_tag_dyneither(_tmpA84,sizeof(void*),1)))))))),_tmpA86)))),((_tmpA82->identity=- 1,((_tmpA82->kind=
# 1358
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA82)))))));
# 1359
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA89;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA88;void*t=(void*)((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=2,((_tmpA89.f1=tv,_tmpA89)))),_tmpA88))));
# 1360
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA8F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA8E;struct _dyneither_ptr*_tmpA8C;struct _tuple0*_tmpA8B;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->f1=Cyc_Absyn_Loc_n,((_tmpA8B->f2=((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=_tmp4D1,_tmpA8C)))),_tmpA8B)))))),(void*)((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=15,((_tmpA8F.f1=(void*)t,_tmpA8F)))),_tmpA8E)))),0);
# 1362
{struct Cyc_List_List*_tmpA90;yyval=Cyc_YY18(((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4D0,_tmp4D2,(unsigned int)(yyyylsp[0]).first_line),((_tmpA90->tl=0,_tmpA90)))))));}
# 1364
break;};}case 36: _LL241: {
# 1362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1366
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA98;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA97;struct _dyneither_ptr*_tmpA96;struct Cyc_Absyn_Tvar*_tmpA95;struct Cyc_Absyn_Tvar*tv=(_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->name=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA96)))),((_tmpA95->identity=- 1,((_tmpA95->kind=(void*)((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA97.tag=0,((_tmpA97.f1=& Cyc_Tcutil_rk,_tmpA97)))),_tmpA98)))),_tmpA95)))))));
# 1367
struct _dyneither_ptr*_tmpA9B;struct _tuple0*_tmpA9A;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->f1=Cyc_Absyn_Loc_n,((_tmpA9A->f2=((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpA9B)))),_tmpA9A)))))),(void*)& Cyc_Absyn_VoidType_val,0);
# 1368
{struct Cyc_List_List*_tmpA9C;yyval=Cyc_YY18(((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyyyvsp[6]),tv,vd,(unsigned int)(yyyylsp[0]).first_line),((_tmpA9C->tl=0,_tmpA9C)))))));}
# 1370
break;}case 37: _LL242:
# 1369 "parse.y"
 yyval=Cyc_YY31(0);
# 1370
break;case 38: _LL243: {
# 1370 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1374
yyval=Cyc_YY31(1);
# 1375
break;}case 39: _LL244: {
# 1375 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1379
yyval=yyyyvsp[0];
# 1380
break;}case 40: _LL245: {
# 1377 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1381
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1])));
# 1382
break;}case 41: _LL246: {
# 1383 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1387
{struct Cyc_Parse_Declaration_spec _tmpA9D;yyval=Cyc_YY19(((_tmpA9D.sc=(enum Cyc_Parse_Storage_class*)Cyc_yyget_YY22(yyyyvsp[0]),((_tmpA9D.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA9D.type_specs=
# 1388
Cyc_Parse_empty_spec(0),((_tmpA9D.is_inline=0,((_tmpA9D.attributes=0,_tmpA9D)))))))))));}
# 1389
break;}case 42: _LL247: {
# 1386 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1390
struct Cyc_Parse_Declaration_spec _tmp4EC=Cyc_yyget_YY19(yyyyvsp[1]);
# 1391
if(_tmp4EC.sc != 0){
# 1392
const char*_tmpAA0;void*_tmpA9F;(_tmpA9F=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAA0="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpAA0,sizeof(char),51))),_tag_dyneither(_tmpA9F,sizeof(void*),0)));}
# 1394
{struct Cyc_Parse_Declaration_spec _tmpAA1;yyval=Cyc_YY19(((_tmpAA1.sc=(enum Cyc_Parse_Storage_class*)Cyc_yyget_YY22(yyyyvsp[0]),((_tmpAA1.tq=_tmp4EC.tq,((_tmpAA1.type_specs=_tmp4EC.type_specs,((_tmpAA1.is_inline=_tmp4EC.is_inline,((_tmpAA1.attributes=_tmp4EC.attributes,_tmpAA1)))))))))));}
# 1398
break;}case 43: _LL248: {
# 1395 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1399
{struct Cyc_Parse_Declaration_spec _tmpAA2;yyval=Cyc_YY19(((_tmpAA2.sc=0,((_tmpAA2.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA2.type_specs=
# 1400
Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAA2.is_inline=0,((_tmpAA2.attributes=0,_tmpAA2)))))))))));}
# 1401
break;}case 44: _LL249: {
# 1398 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1402
struct Cyc_Parse_Declaration_spec _tmp4F1=Cyc_yyget_YY19(yyyyvsp[1]);
# 1403
{struct Cyc_Parse_Declaration_spec _tmpAA3;yyval=Cyc_YY19(((_tmpAA3.sc=_tmp4F1.sc,((_tmpAA3.tq=_tmp4F1.tq,((_tmpAA3.type_specs=
# 1404
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4F1.type_specs,Cyc_yyget_YY23(yyyyvsp[0])),((_tmpAA3.is_inline=_tmp4F1.is_inline,((_tmpAA3.attributes=_tmp4F1.attributes,_tmpAA3)))))))))));}
# 1408
break;}case 45: _LL24A: {
# 1405 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1409
{struct Cyc_Parse_Declaration_spec _tmpAA4;yyval=Cyc_YY19(((_tmpAA4.sc=0,((_tmpAA4.tq=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAA4.type_specs=Cyc_Parse_empty_spec(0),((_tmpAA4.is_inline=0,((_tmpAA4.attributes=0,_tmpAA4)))))))))));}
# 1410
break;}case 46: _LL24B: {
# 1407 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1411
struct Cyc_Parse_Declaration_spec _tmp4F4=Cyc_yyget_YY19(yyyyvsp[1]);
# 1412
{struct Cyc_Parse_Declaration_spec _tmpAA5;yyval=Cyc_YY19(((_tmpAA5.sc=_tmp4F4.sc,((_tmpAA5.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp4F4.tq),((_tmpAA5.type_specs=_tmp4F4.type_specs,((_tmpAA5.is_inline=_tmp4F4.is_inline,((_tmpAA5.attributes=_tmp4F4.attributes,_tmpAA5)))))))))));}
# 1416
break;}case 47: _LL24C: {
# 1413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1417
{struct Cyc_Parse_Declaration_spec _tmpAA6;yyval=Cyc_YY19(((_tmpAA6.sc=0,((_tmpAA6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA6.type_specs=
# 1418
Cyc_Parse_empty_spec(0),((_tmpAA6.is_inline=1,((_tmpAA6.attributes=0,_tmpAA6)))))))))));}
# 1419
break;}case 48: _LL24D: {
# 1416 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1420
struct Cyc_Parse_Declaration_spec _tmp4F7=Cyc_yyget_YY19(yyyyvsp[1]);
# 1421
{struct Cyc_Parse_Declaration_spec _tmpAA7;yyval=Cyc_YY19(((_tmpAA7.sc=_tmp4F7.sc,((_tmpAA7.tq=_tmp4F7.tq,((_tmpAA7.type_specs=_tmp4F7.type_specs,((_tmpAA7.is_inline=1,((_tmpAA7.attributes=_tmp4F7.attributes,_tmpAA7)))))))))));}
# 1424
break;}case 49: _LL24E: {
# 1421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1425
{struct Cyc_Parse_Declaration_spec _tmpAA8;yyval=Cyc_YY19(((_tmpAA8.sc=0,((_tmpAA8.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA8.type_specs=
# 1426
Cyc_Parse_empty_spec(0),((_tmpAA8.is_inline=0,((_tmpAA8.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAA8)))))))))));}
# 1427
break;}case 50: _LL24F: {
# 1424 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1428
struct Cyc_Parse_Declaration_spec _tmp4FA=Cyc_yyget_YY19(yyyyvsp[1]);
# 1429
{struct Cyc_Parse_Declaration_spec _tmpAA9;yyval=Cyc_YY19(((_tmpAA9.sc=_tmp4FA.sc,((_tmpAA9.tq=_tmp4FA.tq,((_tmpAA9.type_specs=_tmp4FA.type_specs,((_tmpAA9.is_inline=_tmp4FA.is_inline,((_tmpAA9.attributes=
# 1431
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4FA.attributes),_tmpAA9)))))))))));}
# 1432
break;}case 51: _LL250: {
# 1431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1435
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
# 1436
yyval=Cyc_YY22(& auto_sc);
# 1437
break;}case 52: _LL251: {
# 1433 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1437
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
# 1438
yyval=Cyc_YY22(& register_sc);
# 1439
break;}case 53: _LL252: {
# 1435 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1439
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
# 1440
yyval=Cyc_YY22(& static_sc);
# 1441
break;}case 54: _LL253: {
# 1437 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1441
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
# 1442
yyval=Cyc_YY22(& extern_sc);
# 1443
break;}case 55: _LL254: {
# 1440 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1444
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
# 1445
{const char*_tmpAAA;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAAA="C",_tag_dyneither(_tmpAAA,sizeof(char),2))))!= 0){
# 1446
const char*_tmpAAB;Cyc_Parse_err(((_tmpAAB="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAAB,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
# 1447
yyval=Cyc_YY22(& externC_sc);
# 1449
break;}case 56: _LL255: {
# 1445 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
# 1450
yyval=Cyc_YY22(& typedef_sc);
# 1451
break;}case 57: _LL256: {
# 1448 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1452
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
# 1453
yyval=Cyc_YY22(& abstract_sc);
# 1454
break;}case 58: _LL257:
# 1454 "parse.y"
 yyval=Cyc_YY45(0);
# 1455
break;case 59: _LL258: {
# 1455 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1459
yyval=yyyyvsp[0];
# 1460
break;}case 60: _LL259: {
# 1460 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1464
yyval=yyyyvsp[3];
# 1465
break;}case 61: _LL25A: {
# 1464 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1468
{struct Cyc_List_List*_tmpAAC;yyval=Cyc_YY45(((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAAC->tl=0,_tmpAAC)))))));}
# 1469
break;}case 62: _LL25B: {
# 1465 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1469
{struct Cyc_List_List*_tmpAAD;yyval=Cyc_YY45(((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAAD->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAAD)))))));}
# 1470
break;}case 63: _LL25C: {
# 1470 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1474
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
# 1475
static struct _tuple27 att_map[17]={{{_tmp502,_tmp502,_tmp502 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp503,_tmp503,_tmp503 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp504,_tmp504,_tmp504 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp505,_tmp505,_tmp505 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp506,_tmp506,_tmp506 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp507,_tmp507,_tmp507 + 8},(void*)& att_aligned},{{_tmp508,_tmp508,_tmp508 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp509,_tmp509,_tmp509 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp50A,_tmp50A,_tmp50A + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp50B,_tmp50B,_tmp50B + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp50C,_tmp50C,_tmp50C + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp50D,_tmp50D,_tmp50D + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp50E,_tmp50E,_tmp50E + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp50F,_tmp50F,_tmp50F + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp510,_tmp510,_tmp510 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp511,_tmp511,_tmp511 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp512,_tmp512,_tmp512 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1494
struct _dyneither_ptr _tmp500=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1496
if((((_get_dyneither_size(_tmp500,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp500,sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp500,sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp500,sizeof(char),(int)(_get_dyneither_size(_tmp500,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp500,sizeof(char),(int)(_get_dyneither_size(_tmp500,sizeof(char))- 3)))== '_')
# 1498
_tmp500=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp500,2,_get_dyneither_size(_tmp500,sizeof(char))- 5);{
# 1499
int i=0;
# 1500
for(0;i < 17;++ i){
# 1501
if(Cyc_strcmp((struct _dyneither_ptr)_tmp500,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
# 1502
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
# 1503
break;}}
# 1505
if(i == 17){
# 1506
{const char*_tmpAAE;Cyc_Parse_err(((_tmpAAE="unrecognized attribute",_tag_dyneither(_tmpAAE,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
# 1507
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1510
break;};}case 64: _LL25D: {
# 1506 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1510
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
# 1511
break;}case 65: _LL25E: {
# 1508 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1512
struct _dyneither_ptr _tmp514=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1513
int _tmp516;struct _tuple5 _tmp515=Cyc_yyget_Int_tok(yyyyvsp[2]);_tmp516=_tmp515.f2;{
# 1514
void*a;
# 1515
{const char*_tmpAB0;const char*_tmpAAF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAB0="regparm",_tag_dyneither(_tmpAB0,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAAF="__regparm__",_tag_dyneither(_tmpAAF,sizeof(char),12))))== 0){
# 1516
if(_tmp516 < 0  || _tmp516 > 3){
# 1517
const char*_tmpAB1;Cyc_Parse_err(((_tmpAB1="regparm requires value between 0 and 3",_tag_dyneither(_tmpAB1,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
# 1518
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAB4;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAB3;a=(void*)((_tmpAB3=_cycalloc_atomic(sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB4.tag=0,((_tmpAB4.f1=_tmp516,_tmpAB4)))),_tmpAB3))));};}else{
# 1519
const char*_tmpAB6;const char*_tmpAB5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAB6="aligned",_tag_dyneither(_tmpAB6,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAB5="__aligned__",_tag_dyneither(_tmpAB5,sizeof(char),12))))== 0){
# 1520
if(_tmp516 < 0){const char*_tmpAB7;Cyc_Parse_err(((_tmpAB7="aligned requires positive power of two",_tag_dyneither(_tmpAB7,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
# 1521
unsigned int j=(unsigned int)_tmp516;
# 1522
for(0;(j & 1)== 0;j=j >> 1){
# 1523
;}
# 1524
j=j >> 1;
# 1525
if(j != 0){const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="aligned requires positive power of two",_tag_dyneither(_tmpAB8,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
# 1526
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpABB;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpABA;a=(void*)((_tmpABA=_cycalloc_atomic(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=6,((_tmpABB.f1=_tmp516,_tmpABB)))),_tmpABA))));};};}else{
# 1527
const char*_tmpABD;const char*_tmpABC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpABD="initializes",_tag_dyneither(_tmpABD,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpABC="__initializes__",_tag_dyneither(_tmpABC,sizeof(char),16))))== 0){
# 1528
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpAC0;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpABF;a=(void*)((_tmpABF=_cycalloc_atomic(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=20,((_tmpAC0.f1=_tmp516,_tmpAC0)))),_tmpABF))));}else{
# 1529
const char*_tmpAC2;const char*_tmpAC1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAC2="noliveunique",_tag_dyneither(_tmpAC2,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAC1="__noliveunique__",_tag_dyneither(_tmpAC1,sizeof(char),17))))== 0){
# 1530
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAC5;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAC4;a=(void*)((_tmpAC4=_cycalloc_atomic(sizeof(*_tmpAC4)),((_tmpAC4[0]=((_tmpAC5.tag=21,((_tmpAC5.f1=_tmp516,_tmpAC5)))),_tmpAC4))));}else{
# 1531
const char*_tmpAC7;const char*_tmpAC6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAC7="noconsume",_tag_dyneither(_tmpAC7,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAC6="__noconsume__",_tag_dyneither(_tmpAC6,sizeof(char),14))))== 0){
# 1532
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpACA;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAC9;a=(void*)((_tmpAC9=_cycalloc_atomic(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=22,((_tmpACA.f1=_tmp516,_tmpACA)))),_tmpAC9))));}else{
# 1534
{const char*_tmpACB;Cyc_Parse_err(((_tmpACB="unrecognized attribute",_tag_dyneither(_tmpACB,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
# 1535
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1537
yyval=Cyc_YY46(a);
# 1539
break;};}case 66: _LL25F: {
# 1536 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1540
struct _dyneither_ptr _tmp52F=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1541
struct _dyneither_ptr _tmp530=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1542
void*a;
# 1543
{const char*_tmpACD;const char*_tmpACC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52F,((_tmpACD="section",_tag_dyneither(_tmpACD,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52F,((_tmpACC="__section__",_tag_dyneither(_tmpACC,sizeof(char),12))))== 0){
# 1544
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAD0;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpACF;a=(void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=8,((_tmpAD0.f1=_tmp530,_tmpAD0)))),_tmpACF))));}else{
# 1546
{const char*_tmpAD1;Cyc_Parse_err(((_tmpAD1="unrecognized attribute",_tag_dyneither(_tmpAD1,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
# 1547
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1549
yyval=Cyc_YY46(a);
# 1551
break;}case 67: _LL260: {
# 1548 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1552
struct _dyneither_ptr _tmp536=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1553
struct _dyneither_ptr _tmp537=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1554
void*a;
# 1556
{const char*_tmpAD2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp536,((_tmpAD2="__mode__",_tag_dyneither(_tmpAD2,sizeof(char),9))))== 0){
# 1557
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAD5;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAD4;a=(void*)((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD5.tag=24,((_tmpAD5.f1=_tmp537,_tmpAD5)))),_tmpAD4))));}else{
# 1559
{const char*_tmpAD6;Cyc_Parse_err(((_tmpAD6="unrecognized attribute",_tag_dyneither(_tmpAD6,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
# 1560
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1562
yyval=Cyc_YY46(a);
# 1564
break;}case 68: _LL261: {
# 1561 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1565
struct _dyneither_ptr _tmp53C=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1566
struct _dyneither_ptr _tmp53D=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1567
int _tmp53F;struct _tuple5 _tmp53E=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp53F=_tmp53E.f2;{
# 1568
int _tmp541;struct _tuple5 _tmp540=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp541=_tmp540.f2;{
# 1569
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
# 1570
{const char*_tmpAD8;const char*_tmpAD7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53C,((_tmpAD8="format",_tag_dyneither(_tmpAD8,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp53C,((_tmpAD7="__format__",_tag_dyneither(_tmpAD7,sizeof(char),11))))== 0){
# 1571
const char*_tmpAD9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53D,((_tmpAD9="printf",_tag_dyneither(_tmpAD9,sizeof(char),7))))== 0){
# 1572
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpADC;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpADB;a=(void*)((_tmpADB=_cycalloc_atomic(sizeof(*_tmpADB)),((_tmpADB[0]=((_tmpADC.tag=19,((_tmpADC.f1=Cyc_Absyn_Printf_ft,((_tmpADC.f2=_tmp53F,((_tmpADC.f3=_tmp541,_tmpADC)))))))),_tmpADB))));}else{
# 1573
const char*_tmpADD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53D,((_tmpADD="scanf",_tag_dyneither(_tmpADD,sizeof(char),6))))== 0){
# 1574
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAE0;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpADF;a=(void*)((_tmpADF=_cycalloc_atomic(sizeof(*_tmpADF)),((_tmpADF[0]=((_tmpAE0.tag=19,((_tmpAE0.f1=Cyc_Absyn_Scanf_ft,((_tmpAE0.f2=_tmp53F,((_tmpAE0.f3=_tmp541,_tmpAE0)))))))),_tmpADF))));}else{
# 1576
const char*_tmpAE1;Cyc_Parse_err(((_tmpAE1="unrecognized format type",_tag_dyneither(_tmpAE1,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1578
const char*_tmpAE2;Cyc_Parse_err(((_tmpAE2="unrecognized attribute",_tag_dyneither(_tmpAE2,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
# 1579
yyval=Cyc_YY46(a);
# 1581
break;};};}case 69: _LL262: {
# 1586 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1590
yyval=yyyyvsp[0];
# 1591
break;}case 70: _LL263: {
# 1588 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1592
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAE5;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE4;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4[0]=((_tmpAE5.tag=17,((_tmpAE5.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAE5.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpAE5.f3=0,((_tmpAE5.f4=(void*)0,_tmpAE5)))))))))),_tmpAE4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1593
break;}case 71: _LL264: {
# 1592 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1596
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
# 1597
break;}case 72: _LL265: {
# 1593 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1597
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
# 1598
break;}case 73: _LL266: {
# 1595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1599
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
# 1600
break;}case 74: _LL267: {
# 1596 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1600
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
# 1601
break;}case 75: _LL268: {
# 1597 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1601
yyval=Cyc_YY23(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
# 1602
break;}case 76: _LL269: {
# 1598 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1602
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
# 1603
break;}case 77: _LL26A: {
# 1599 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1603
yyval=Cyc_YY23(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
# 1604
break;}case 78: _LL26B: {
# 1600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
# 1605
break;}case 79: _LL26C: {
# 1601 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
# 1606
break;}case 80: _LL26D: {
# 1602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606
yyval=Cyc_YY23(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
# 1607
break;}case 81: _LL26E: {
# 1603 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607
yyval=Cyc_YY23(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
# 1608
break;}case 82: _LL26F: {
# 1604 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608
yyval=yyyyvsp[0];
# 1609
break;}case 83: _LL270: {
# 1605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609
yyval=yyyyvsp[0];
# 1610
break;}case 84: _LL271: {
# 1607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611
yyval=yyyyvsp[0];
# 1612
break;}case 85: _LL272: {
# 1609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 1614
break;}case 86: _LL273: {
# 1612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1616
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAE8;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAE7;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=10,((_tmpAE8.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
# 1617
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpAE8)))),_tmpAE7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1619
break;}case 87: _LL274: {
# 1616 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1620
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAEB;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAEA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=15,((_tmpAEB.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpAEB)))),_tmpAEA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1621
break;}case 88: _LL275: {
# 1618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAED;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=15,((_tmpAEE.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpAEE)))),_tmpAED)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1624
break;}case 89: _LL276: {
# 1621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1625
void*_tmp554=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);
# 1626
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAF1;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAF0;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF1.tag=16,((_tmpAF1.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAF1.f2=(void*)_tmp554,_tmpAF1)))))),_tmpAF0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1628
break;}case 90: _LL277: {
# 1625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1629
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAF4;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAF3;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=16,((_tmpAF4.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAF4.f2=(void*)Cyc_yyget_YY44(yyyyvsp[4]),_tmpAF4)))))),_tmpAF3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1630
break;}case 91: _LL278: {
# 1627 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1631
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAF7;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF6;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=19,((_tmpAF7.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpAF7)))),_tmpAF6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1632
break;}case 92: _LL279: {
# 1629 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1633
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAFA;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF9;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=19,((_tmpAFA.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpAFA)))),_tmpAF9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1634
break;}case 93: _LL27A: {
# 1631 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1635
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAFD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAFC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC[0]=((_tmpAFD.tag=18,((_tmpAFD.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpAFD)))),_tmpAFC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1636
break;}case 94: _LL27B: {
# 1636 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1640
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 1641
break;}case 95: _LL27C: {
# 1640 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
# 1645
{struct Cyc_Absyn_Tqual _tmpAFE;yyval=Cyc_YY25(((_tmpAFE.print_const=1,((_tmpAFE.q_volatile=0,((_tmpAFE.q_restrict=0,((_tmpAFE.real_const=1,((_tmpAFE.loc=loc,_tmpAFE)))))))))));}
# 1646
break;}case 96: _LL27D: {
# 1642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1646
{struct Cyc_Absyn_Tqual _tmpAFF;yyval=Cyc_YY25(((_tmpAFF.print_const=0,((_tmpAFF.q_volatile=1,((_tmpAFF.q_restrict=0,((_tmpAFF.real_const=0,((_tmpAFF.loc=0,_tmpAFF)))))))))));}
# 1647
break;}case 97: _LL27E: {
# 1643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1647
{struct Cyc_Absyn_Tqual _tmpB00;yyval=Cyc_YY25(((_tmpB00.print_const=0,((_tmpB00.q_volatile=0,((_tmpB00.q_restrict=1,((_tmpB00.real_const=0,((_tmpB00.loc=0,_tmpB00)))))))))));}
# 1648
break;}case 98: _LL27F: {
# 1649 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1653
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB0F;struct Cyc_Absyn_Enumdecl*_tmpB0E;struct Cyc_Core_Opt*_tmpB0D;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB0C;struct Cyc_Absyn_TypeDecl*_tmpB0B;struct Cyc_Absyn_TypeDecl*_tmp562=(_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->r=(void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB0C.tag=1,((_tmpB0C.f1=((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->sc=Cyc_Absyn_Public,((_tmpB0E->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB0E->fields=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB0D)))),_tmpB0E)))))))),_tmpB0C)))),_tmpB0F)))),((_tmpB0B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB0B)))));
# 1655
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB12;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB11;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=((_tmpB12.tag=26,((_tmpB12.f1=_tmp562,((_tmpB12.f2=0,_tmpB12)))))),_tmpB11)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1657
break;}case 99: _LL280: {
# 1654 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1658
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB15;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB14;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14[0]=((_tmpB15.tag=13,((_tmpB15.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB15.f2=0,_tmpB15)))))),_tmpB14)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1659
break;}case 100: _LL281: {
# 1656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1660
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB18;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB17;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=((_tmpB18.tag=14,((_tmpB18.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB18)))),_tmpB17)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1661
break;}case 101: _LL282: {
# 1662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1666
{struct Cyc_Absyn_Enumfield*_tmpB19;yyval=Cyc_YY47(((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB19->tag=0,((_tmpB19->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB19)))))))));}
# 1667
break;}case 102: _LL283: {
# 1664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1668
{struct Cyc_Absyn_Enumfield*_tmpB1A;yyval=Cyc_YY47(((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB1A->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpB1A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB1A)))))))));}
# 1669
break;}case 103: _LL284: {
# 1668 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1672
{struct Cyc_List_List*_tmpB1B;yyval=Cyc_YY48(((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB1B->tl=0,_tmpB1B)))))));}
# 1673
break;}case 104: _LL285: {
# 1669 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1673
{struct Cyc_List_List*_tmpB1C;yyval=Cyc_YY48(((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB1C->tl=0,_tmpB1C)))))));}
# 1674
break;}case 105: _LL286: {
# 1670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1674
{struct Cyc_List_List*_tmpB1D;yyval=Cyc_YY48(((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB1D->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB1D)))))));}
# 1675
break;}case 106: _LL287: {
# 1676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1680
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB20;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB1F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=12,((_tmpB20.f1=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB20.f2=Cyc_yyget_YY26(yyyyvsp[2]),_tmpB20)))))),_tmpB1F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1681
break;}case 107: _LL288: {
# 1681 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1686
struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
# 1687
struct Cyc_List_List*_tmp576=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
# 1688
struct Cyc_Absyn_TypeDecl*_tmp577=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp575,(struct Cyc_Absyn_AggrdeclImpl*)
# 1689
Cyc_Absyn_aggrdecl_impl(_tmp576,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY26(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1691
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB23;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB22;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB23.tag=26,((_tmpB23.f1=_tmp577,((_tmpB23.f2=0,_tmpB23)))))),_tmpB22)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1693
break;}case 108: _LL289: {
# 1692 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1697
struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
# 1698
struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
# 1699
struct Cyc_Absyn_TypeDecl*_tmp57C=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp57A,(struct Cyc_Absyn_AggrdeclImpl*)
# 1700
Cyc_Absyn_aggrdecl_impl(_tmp57B,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY26(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1702
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB26;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB25;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=26,((_tmpB26.f1=_tmp57C,((_tmpB26.f2=0,_tmpB26)))))),_tmpB25)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1704
break;}case 109: _LL28A: {
# 1701 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1705
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB30;struct Cyc_Core_Opt*_tmpB2F;struct Cyc_Absyn_AggrInfo _tmpB2E;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB2D;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB30.tag=11,((_tmpB30.f1=((_tmpB2E.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB2F=_cycalloc_atomic(sizeof(*_tmpB2F)),((_tmpB2F->v=(void*)1,_tmpB2F))))),((_tmpB2E.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB2E)))),_tmpB30)))),_tmpB2D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1708
break;}case 110: _LL28B: {
# 1705 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1709
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_AggrInfo _tmpB35;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB34;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34[0]=((_tmpB36.tag=11,((_tmpB36.f1=((_tmpB35.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB35.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB35)))),_tmpB36)))),_tmpB34)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1712
break;}case 111: _LL28C:
# 1712 "parse.y"
 yyval=Cyc_YY40(0);
# 1713
break;case 112: _LL28D: {
# 1714 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1718
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
# 1719
break;}case 113: _LL28E: {
# 1718 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1722
yyval=Cyc_YY24(Cyc_Absyn_StructA);
# 1723
break;}case 114: _LL28F: {
# 1719 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1723
yyval=Cyc_YY24(Cyc_Absyn_UnionA);
# 1724
break;}case 115: _LL290:
# 1724 "parse.y"
 yyval=Cyc_YY26(0);
# 1725
break;case 116: _LL291: {
# 1726 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1730
struct Cyc_List_List*decls=0;
# 1731
{struct Cyc_List_List*_tmp586=Cyc_yyget_YY27(yyyyvsp[0]);for(0;_tmp586 != 0;_tmp586=_tmp586->tl){
# 1732
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp586->hd,decls);}}{
# 1734
struct Cyc_List_List*_tmp587=Cyc_Parse_get_argrfield_tags(decls);
# 1735
if(_tmp587 != 0)
# 1736
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp587,decls);
# 1737
yyval=Cyc_YY26(decls);
# 1739
break;};}case 117: _LL292: {
# 1740 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1744
{struct Cyc_List_List*_tmpB37;yyval=Cyc_YY27(((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->hd=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB37->tl=0,_tmpB37)))))));}
# 1745
break;}case 118: _LL293: {
# 1742 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1746
{struct Cyc_List_List*_tmpB38;yyval=Cyc_YY27(((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->hd=Cyc_yyget_YY26(yyyyvsp[1]),((_tmpB38->tl=Cyc_yyget_YY27(yyyyvsp[0]),_tmpB38)))))));}
# 1747
break;}case 119: _LL294: {
# 1746 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1750
yyval=Cyc_YY21(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
# 1751
break;}case 120: _LL295: {
# 1752 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1756
{struct _tuple13*_tmpB39;yyval=Cyc_YY21(((_tmpB39=_region_malloc(yyr,sizeof(*_tmpB39)),((_tmpB39->tl=0,((_tmpB39->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpB39)))))));}
# 1757
break;}case 121: _LL296: {
# 1754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1758
{struct _tuple13*_tmpB3A;yyval=Cyc_YY21(((_tmpB3A=_region_malloc(yyr,sizeof(*_tmpB3A)),((_tmpB3A->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB3A->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpB3A)))))));}
# 1759
break;}case 122: _LL297: {
# 1759 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1763
{struct _tuple12 _tmpB3B;yyval=Cyc_YY20(((_tmpB3B.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB3B.f2=0,_tmpB3B)))));}
# 1764
break;}case 123: _LL298: {
# 1761 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1765
{struct _tuple12 _tmpB3C;yyval=Cyc_YY20(((_tmpB3C.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB3C.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpB3C)))));}
# 1766
break;}case 124: _LL299: {
# 1766 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1778 "parse.y"
struct _RegionHandle _tmp58E=_new_region("temp");struct _RegionHandle*temp=& _tmp58E;_push_region(temp);{
# 1779
struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Parse_Type_specifier _tmp591;struct Cyc_List_List*_tmp592;struct _tuple24 _tmp58F=Cyc_yyget_YY35(yyyyvsp[0]);_tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_tmp592=_tmp58F.f3;
# 1780
if(_tmp590.loc == 0)_tmp590.loc=(unsigned int)(yyyylsp[0]).first_line;{
# 1781
struct _tuple11*decls=0;
# 1782
struct Cyc_List_List*widths=0;
# 1783
Cyc_Parse_decl_split(temp,Cyc_yyget_YY21(yyyyvsp[1]),& decls,& widths);{
# 1784
void*_tmp593=Cyc_Parse_speclist2typ(_tmp591,(unsigned int)(yyyylsp[0]).first_line);
# 1785
struct Cyc_List_List*_tmp594=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
# 1786
Cyc_Parse_apply_tmss(temp,_tmp590,_tmp593,decls,_tmp592),widths);
# 1787
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp594));};};}
# 1790
_npop_handler(0);break;
# 1778
;_pop_region(temp);}case 125: _LL29A: {
# 1794 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1798
{struct _tuple24 _tmpB3D;yyval=Cyc_YY35(((_tmpB3D.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB3D.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB3D.f3=0,_tmpB3D)))))));}
# 1799
break;}case 126: _LL29B: {
# 1796 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1800
struct _tuple24 _tmp596=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple24 _tmpB3E;yyval=Cyc_YY35(((_tmpB3E.f1=_tmp596.f1,((_tmpB3E.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp596.f2),((_tmpB3E.f3=_tmp596.f3,_tmpB3E)))))));}
# 1801
break;}case 127: _LL29C: {
# 1798 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1802
{struct _tuple24 _tmpB3F;yyval=Cyc_YY35(((_tmpB3F.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB3F.f2=Cyc_Parse_empty_spec(0),((_tmpB3F.f3=0,_tmpB3F)))))));}
# 1803
break;}case 128: _LL29D: {
# 1800 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1804
struct _tuple24 _tmp599=Cyc_yyget_YY35(yyyyvsp[1]);
# 1805
{struct _tuple24 _tmpB40;yyval=Cyc_YY35(((_tmpB40.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp599.f1),((_tmpB40.f2=_tmp599.f2,((_tmpB40.f3=_tmp599.f3,_tmpB40)))))));}
# 1806
break;}case 129: _LL29E: {
# 1803 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1807
{struct _tuple24 _tmpB41;yyval=Cyc_YY35(((_tmpB41.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB41.f2=Cyc_Parse_empty_spec(0),((_tmpB41.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB41)))))));}
# 1808
break;}case 130: _LL29F: {
# 1805 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1809
struct _tuple24 _tmp59C=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple24 _tmpB42;yyval=Cyc_YY35(((_tmpB42.f1=_tmp59C.f1,((_tmpB42.f2=_tmp59C.f2,((_tmpB42.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp59C.f3),_tmpB42)))))));}
# 1810
break;}case 131: _LL2A0: {
# 1811 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1815
{struct _tuple24 _tmpB43;yyval=Cyc_YY35(((_tmpB43.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB43.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB43.f3=0,_tmpB43)))))));}
# 1816
break;}case 132: _LL2A1: {
# 1813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1817
struct _tuple24 _tmp59F=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple24 _tmpB44;yyval=Cyc_YY35(((_tmpB44.f1=_tmp59F.f1,((_tmpB44.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp59F.f2),((_tmpB44.f3=_tmp59F.f3,_tmpB44)))))));}
# 1818
break;}case 133: _LL2A2: {
# 1815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1819
{struct _tuple24 _tmpB45;yyval=Cyc_YY35(((_tmpB45.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB45.f2=Cyc_Parse_empty_spec(0),((_tmpB45.f3=0,_tmpB45)))))));}
# 1820
break;}case 134: _LL2A3: {
# 1817 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1821
struct _tuple24 _tmp5A2=Cyc_yyget_YY35(yyyyvsp[1]);
# 1822
{struct _tuple24 _tmpB46;yyval=Cyc_YY35(((_tmpB46.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp5A2.f1),((_tmpB46.f2=_tmp5A2.f2,((_tmpB46.f3=_tmp5A2.f3,_tmpB46)))))));}
# 1823
break;}case 135: _LL2A4: {
# 1820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1824
{struct _tuple24 _tmpB47;yyval=Cyc_YY35(((_tmpB47.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB47.f2=Cyc_Parse_empty_spec(0),((_tmpB47.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB47)))))));}
# 1825
break;}case 136: _LL2A5: {
# 1822 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1826
struct _tuple24 _tmp5A5=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple24 _tmpB48;yyval=Cyc_YY35(((_tmpB48.f1=_tmp5A5.f1,((_tmpB48.f2=_tmp5A5.f2,((_tmpB48.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5A5.f3),_tmpB48)))))));}
# 1827
break;}case 137: _LL2A6: {
# 1826 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1830
yyval=Cyc_YY21(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
# 1831
break;}case 138: _LL2A7: {
# 1832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1836
{struct _tuple13*_tmpB49;yyval=Cyc_YY21(((_tmpB49=_region_malloc(yyr,sizeof(*_tmpB49)),((_tmpB49->tl=0,((_tmpB49->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpB49)))))));}
# 1837
break;}case 139: _LL2A8: {
# 1834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1838
{struct _tuple13*_tmpB4A;yyval=Cyc_YY21(((_tmpB4A=_region_malloc(yyr,sizeof(*_tmpB4A)),((_tmpB4A->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB4A->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpB4A)))))));}
# 1839
break;}case 140: _LL2A9: {
# 1839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1843
{struct _tuple12 _tmpB4B;yyval=Cyc_YY20(((_tmpB4B.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB4B.f2=0,_tmpB4B)))));}
# 1844
break;}case 141: _LL2AA: {
# 1841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1847
{struct Cyc_Parse_Declarator _tmpB51;struct _tuple0*_tmpB50;struct _tuple12 _tmpB4F;yyval=Cyc_YY20(((_tmpB4F.f1=((_tmpB51.id=((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->f1=Cyc_Absyn_Rel_n(0),((_tmpB50->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB50)))))),((_tmpB51.tms=0,_tmpB51)))),((_tmpB4F.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),_tmpB4F)))));}
# 1849
break;}case 142: _LL2AB:
# 1848 "parse.y"
{struct Cyc_Parse_Declarator _tmpB57;struct _tuple0*_tmpB56;struct _tuple12 _tmpB55;yyval=Cyc_YY20(((_tmpB55.f1=((_tmpB57.id=((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->f1=Cyc_Absyn_Rel_n(0),((_tmpB56->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB56)))))),((_tmpB57.tms=0,_tmpB57)))),((_tmpB55.f2=0,_tmpB55)))));}
# 1850
break;case 143: _LL2AC: {
# 1851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1855
{struct _tuple12 _tmpB58;yyval=Cyc_YY20(((_tmpB58.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB58.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpB58)))));}
# 1856
break;}case 144: _LL2AD: {
# 1857 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1861
int _tmp5B3=Cyc_yyget_YY31(yyyyvsp[0]);
# 1862
struct Cyc_List_List*_tmp5B4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
# 1863
struct Cyc_Core_Opt*_tmpB59;struct Cyc_Absyn_TypeDecl*_tmp5B5=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5B4,((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB59)))),_tmp5B3,(unsigned int)(yyyylsp[0]).first_line);
# 1865
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB5C;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5B;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5C.tag=26,((_tmpB5C.f1=_tmp5B5,((_tmpB5C.f2=0,_tmpB5C)))))),_tmpB5B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1867
break;}case 145: _LL2AE: {
# 1864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1868
int _tmp5B9=Cyc_yyget_YY31(yyyyvsp[0]);
# 1869
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB66;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB65;struct Cyc_Absyn_DatatypeInfo _tmpB64;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB63;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63[0]=((_tmpB66.tag=3,((_tmpB66.f1=((_tmpB64.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB65.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB65.is_extensible=_tmp5B9,_tmpB65))))),((_tmpB64.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB64)))),_tmpB66)))),_tmpB63)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1871
break;}case 146: _LL2AF: {
# 1868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1872
int _tmp5BE=Cyc_yyget_YY31(yyyyvsp[0]);
# 1873
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB70;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB6F;struct Cyc_Absyn_DatatypeFieldInfo _tmpB6E;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB6D;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D[0]=((_tmpB70.tag=4,((_tmpB70.f1=((_tmpB6E.field_info=
# 1874
Cyc_Absyn_UnknownDatatypefield(((_tmpB6F.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB6F.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB6F.is_extensible=_tmp5BE,_tmpB6F))))))),((_tmpB6E.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpB6E)))),_tmpB70)))),_tmpB6D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1877
break;}case 147: _LL2B0: {
# 1876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1880
yyval=Cyc_YY31(0);
# 1881
break;}case 148: _LL2B1: {
# 1877 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1881
yyval=Cyc_YY31(1);
# 1882
break;}case 149: _LL2B2: {
# 1881 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1885
{struct Cyc_List_List*_tmpB71;yyval=Cyc_YY34(((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB71->tl=0,_tmpB71)))))));}
# 1886
break;}case 150: _LL2B3: {
# 1882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1886
{struct Cyc_List_List*_tmpB72;yyval=Cyc_YY34(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB72->tl=0,_tmpB72)))))));}
# 1887
break;}case 151: _LL2B4: {
# 1883 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1887
{struct Cyc_List_List*_tmpB73;yyval=Cyc_YY34(((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB73->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB73)))))));}
# 1888
break;}case 152: _LL2B5: {
# 1884 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1888
{struct Cyc_List_List*_tmpB74;yyval=Cyc_YY34(((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB74->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB74)))))));}
# 1889
break;}case 153: _LL2B6:
# 1888 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
# 1889
break;case 154: _LL2B7: {
# 1889 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1893
yyval=Cyc_YY32(Cyc_Absyn_Extern);
# 1894
break;}case 155: _LL2B8: {
# 1890 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1894
yyval=Cyc_YY32(Cyc_Absyn_Static);
# 1895
break;}case 156: _LL2B9: {
# 1894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1898
{struct Cyc_Absyn_Datatypefield*_tmpB75;yyval=Cyc_YY33(((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB75->typs=0,((_tmpB75->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB75->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB75)))))))))));}
# 1899
break;}case 157: _LL2BA: {
# 1896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1900
struct Cyc_List_List*_tmp5C8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
# 1901
{struct Cyc_Absyn_Datatypefield*_tmpB76;yyval=Cyc_YY33(((_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB76->typs=_tmp5C8,((_tmpB76->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB76->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB76)))))))))));}
# 1902
break;}case 158: _LL2BB: {
# 1902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1906
yyval=yyyyvsp[0];
# 1907
break;}case 159: _LL2BC: {
# 1904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1908
struct Cyc_Parse_Declarator _tmp5CA=Cyc_yyget_YY29(yyyyvsp[1]);
# 1909
{struct Cyc_Parse_Declarator _tmpB77;yyval=Cyc_YY29(((_tmpB77.id=_tmp5CA.id,((_tmpB77.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp5CA.tms),_tmpB77)))));}
# 1910
break;}case 160: _LL2BD: {
# 1911 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1915
yyval=yyyyvsp[0];
# 1916
break;}case 161: _LL2BE: {
# 1913 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1917
struct Cyc_Parse_Declarator _tmp5CC=Cyc_yyget_YY29(yyyyvsp[1]);
# 1918
{struct Cyc_Parse_Declarator _tmpB78;yyval=Cyc_YY29(((_tmpB78.id=_tmp5CC.id,((_tmpB78.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp5CC.tms),_tmpB78)))));}
# 1919
break;}case 162: _LL2BF: {
# 1919 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1923
{struct Cyc_Parse_Declarator _tmpB79;yyval=Cyc_YY29(((_tmpB79.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB79.tms=0,_tmpB79)))));}
# 1924
break;}case 163: _LL2C0: {
# 1921 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1925
yyval=yyyyvsp[1];
# 1926
break;}case 164: _LL2C1: {
# 1925 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1929
struct Cyc_Parse_Declarator _tmp5CF=Cyc_yyget_YY29(yyyyvsp[2]);
# 1930
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB7F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB7E;struct Cyc_List_List*_tmpB7D;_tmp5CF.tms=((_tmpB7D=_region_malloc(yyr,sizeof(*_tmpB7D)),((_tmpB7D->hd=(void*)((_tmpB7F=_region_malloc(yyr,sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB7E.tag=5,((_tmpB7E.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB7E.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpB7E)))))),_tmpB7F)))),((_tmpB7D->tl=_tmp5CF.tms,_tmpB7D))))));}
# 1931
yyval=yyyyvsp[2];
# 1933
break;}case 165: _LL2C2: {
# 1930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1934
{struct Cyc_List_List*_tmpB89;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB88;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB87;struct Cyc_Parse_Declarator _tmpB86;yyval=Cyc_YY29(((_tmpB86.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB86.tms=((_tmpB89=_region_malloc(yyr,sizeof(*_tmpB89)),((_tmpB89->hd=(void*)((_tmpB87=_region_malloc(yyr,sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB88.tag=0,((_tmpB88.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpB88.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB88)))))),_tmpB87)))),((_tmpB89->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB89)))))),_tmpB86)))));}
# 1935
break;}case 166: _LL2C3: {
# 1932 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1936
{struct Cyc_List_List*_tmpB93;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB92;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB91;struct Cyc_Parse_Declarator _tmpB90;yyval=Cyc_YY29(((_tmpB90.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB90.tms=(
# 1937
(_tmpB93=_region_malloc(yyr,sizeof(*_tmpB93)),((_tmpB93->hd=(void*)((_tmpB91=_region_malloc(yyr,sizeof(*_tmpB91)),((_tmpB91[0]=((_tmpB92.tag=1,((_tmpB92.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpB92.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpB92.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB92)))))))),_tmpB91)))),((_tmpB93->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB93)))))),_tmpB90)))));}
# 1938
break;}case 167: _LL2C4: {
# 1935 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1939
struct _tuple25 _tmp5DC;struct Cyc_List_List*_tmp5DD;int _tmp5DE;struct Cyc_Absyn_VarargInfo*_tmp5DF;void*_tmp5E0;struct Cyc_List_List*_tmp5E1;struct _tuple25*_tmp5DB=Cyc_yyget_YY39(yyyyvsp[2]);_tmp5DC=*_tmp5DB;_tmp5DD=_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;_tmp5DF=_tmp5DC.f3;_tmp5E0=_tmp5DC.f4;_tmp5E1=_tmp5DC.f5;
# 1940
{struct Cyc_List_List*_tmpBA8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBA7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBA6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBA5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBA4;struct Cyc_Parse_Declarator _tmpBA3;yyval=Cyc_YY29(((_tmpBA3.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBA3.tms=((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8->hd=(void*)((_tmpBA4=_region_malloc(yyr,sizeof(*_tmpBA4)),((_tmpBA4[0]=((_tmpBA7.tag=3,((_tmpBA7.f1=(void*)((void*)((_tmpBA5=_region_malloc(yyr,sizeof(*_tmpBA5)),((_tmpBA5[0]=((_tmpBA6.tag=1,((_tmpBA6.f1=_tmp5DD,((_tmpBA6.f2=_tmp5DE,((_tmpBA6.f3=_tmp5DF,((_tmpBA6.f4=(void*)_tmp5E0,((_tmpBA6.f5=_tmp5E1,_tmpBA6)))))))))))),_tmpBA5))))),_tmpBA7)))),_tmpBA4)))),((_tmpBA8->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBA8)))))),_tmpBA3)))));}
# 1942
break;}case 168: _LL2C5: {
# 1939 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1943
{struct Cyc_List_List*_tmpBBD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBBC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBBB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBBA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBB9;struct Cyc_Parse_Declarator _tmpBB8;yyval=Cyc_YY29(((_tmpBB8.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBB8.tms=(
# 1944
(_tmpBBD=_region_malloc(yyr,sizeof(*_tmpBBD)),((_tmpBBD->hd=(void*)((_tmpBB9=_region_malloc(yyr,sizeof(*_tmpBB9)),((_tmpBB9[0]=((_tmpBBC.tag=3,((_tmpBBC.f1=(void*)((void*)((_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=1,((_tmpBBB.f1=0,((_tmpBBB.f2=0,((_tmpBBB.f3=0,((_tmpBBB.f4=(void*)
# 1946
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBBB.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpBBB)))))))))))),_tmpBBA))))),_tmpBBC)))),_tmpBB9)))),((_tmpBBD->tl=(
# 1947
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBBD)))))),_tmpBB8)))));}
# 1949
break;}case 169: _LL2C6: {
# 1946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1950
{struct Cyc_List_List*_tmpBD2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD1;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBD0;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBCF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBCE;struct Cyc_Parse_Declarator _tmpBCD;yyval=Cyc_YY29(((_tmpBCD.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBCD.tms=((_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2->hd=(void*)((_tmpBCE=_region_malloc(yyr,sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBD1.tag=3,((_tmpBD1.f1=(void*)((void*)((_tmpBCF=_region_malloc(yyr,sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD0.tag=0,((_tmpBD0.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpBD0.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBD0)))))),_tmpBCF))))),_tmpBD1)))),_tmpBCE)))),((_tmpBD2->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBD2)))))),_tmpBCD)))));}
# 1951
break;}case 170: _LL2C7: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953
struct Cyc_List_List*_tmp5F4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
# 1954
{struct Cyc_List_List*_tmpBDC;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBDB;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBDA;struct Cyc_Parse_Declarator _tmpBD9;yyval=Cyc_YY29(((_tmpBD9.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBD9.tms=((_tmpBDC=_region_malloc(yyr,sizeof(*_tmpBDC)),((_tmpBDC->hd=(void*)((_tmpBDA=_region_malloc(yyr,sizeof(*_tmpBDA)),((_tmpBDA[0]=((_tmpBDB.tag=4,((_tmpBDB.f1=_tmp5F4,((_tmpBDB.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBDB.f3=0,_tmpBDB)))))))),_tmpBDA)))),((_tmpBDC->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBDC)))))),_tmpBD9)))));}
# 1956
break;}case 171: _LL2C8: {
# 1953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1957
{struct Cyc_List_List*_tmpBE6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBE5;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBE4;struct Cyc_Parse_Declarator _tmpBE3;yyval=Cyc_YY29(((_tmpBE3.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBE3.tms=((_tmpBE6=_region_malloc(yyr,sizeof(*_tmpBE6)),((_tmpBE6->hd=(void*)((_tmpBE4=_region_malloc(yyr,sizeof(*_tmpBE4)),((_tmpBE4[0]=((_tmpBE5.tag=5,((_tmpBE5.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBE5.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBE5)))))),_tmpBE4)))),((_tmpBE6->tl=(
# 1958
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBE6)))))),_tmpBE3)))));}
# 1960
break;}case 172: _LL2C9: {
# 1967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1971
{struct Cyc_Parse_Declarator _tmpBE7;yyval=Cyc_YY29(((_tmpBE7.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBE7.tms=0,_tmpBE7)))));}
# 1972
break;}case 173: _LL2CA: {
# 1969 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1973
{struct Cyc_Parse_Declarator _tmpBE8;yyval=Cyc_YY29(((_tmpBE8.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBE8.tms=0,_tmpBE8)))));}
# 1974
break;}case 174: _LL2CB: {
# 1971 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1975
yyval=yyyyvsp[1];
# 1976
break;}case 175: _LL2CC: {
# 1975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1979
struct Cyc_Parse_Declarator _tmp5FF=Cyc_yyget_YY29(yyyyvsp[2]);
# 1980
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBEE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBED;struct Cyc_List_List*_tmpBEC;_tmp5FF.tms=((_tmpBEC=_region_malloc(yyr,sizeof(*_tmpBEC)),((_tmpBEC->hd=(void*)((_tmpBEE=_region_malloc(yyr,sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBED.tag=5,((_tmpBED.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBED.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBED)))))),_tmpBEE)))),((_tmpBEC->tl=_tmp5FF.tms,_tmpBEC))))));}
# 1981
yyval=yyyyvsp[2];
# 1983
break;}case 176: _LL2CD: {
# 1980 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1984
struct Cyc_Parse_Declarator _tmp603=Cyc_yyget_YY29(yyyyvsp[0]);
# 1985
{struct Cyc_List_List*_tmpBF8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBF7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBF6;struct Cyc_Parse_Declarator _tmpBF5;yyval=Cyc_YY29(((_tmpBF5.id=_tmp603.id,((_tmpBF5.tms=(
# 1986
(_tmpBF8=_region_malloc(yyr,sizeof(*_tmpBF8)),((_tmpBF8->hd=(void*)((_tmpBF6=_region_malloc(yyr,sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF7.tag=0,((_tmpBF7.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBF7.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBF7)))))),_tmpBF6)))),((_tmpBF8->tl=_tmp603.tms,_tmpBF8)))))),_tmpBF5)))));}
# 1987
break;}case 177: _LL2CE: {
# 1984 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1988
struct Cyc_Parse_Declarator _tmp608=Cyc_yyget_YY29(yyyyvsp[0]);
# 1989
{struct Cyc_List_List*_tmpC02;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC01;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC00;struct Cyc_Parse_Declarator _tmpBFF;yyval=Cyc_YY29(((_tmpBFF.id=_tmp608.id,((_tmpBFF.tms=(
# 1990
(_tmpC02=_region_malloc(yyr,sizeof(*_tmpC02)),((_tmpC02->hd=(void*)((_tmpC00=_region_malloc(yyr,sizeof(*_tmpC00)),((_tmpC00[0]=((_tmpC01.tag=1,((_tmpC01.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpC01.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC01.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC01)))))))),_tmpC00)))),((_tmpC02->tl=_tmp608.tms,_tmpC02)))))),_tmpBFF)))));}
# 1992
break;}case 178: _LL2CF: {
# 1989 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1993
struct _tuple25 _tmp60E;struct Cyc_List_List*_tmp60F;int _tmp610;struct Cyc_Absyn_VarargInfo*_tmp611;void*_tmp612;struct Cyc_List_List*_tmp613;struct _tuple25*_tmp60D=Cyc_yyget_YY39(yyyyvsp[2]);_tmp60E=*_tmp60D;_tmp60F=_tmp60E.f1;_tmp610=_tmp60E.f2;_tmp611=_tmp60E.f3;_tmp612=_tmp60E.f4;_tmp613=_tmp60E.f5;{
# 1994
struct Cyc_Parse_Declarator _tmp614=Cyc_yyget_YY29(yyyyvsp[0]);
# 1995
{struct Cyc_List_List*_tmpC17;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC16;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC15;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC14;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC13;struct Cyc_Parse_Declarator _tmpC12;yyval=Cyc_YY29(((_tmpC12.id=_tmp614.id,((_tmpC12.tms=((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17->hd=(void*)((_tmpC13=_region_malloc(yyr,sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC16.tag=3,((_tmpC16.f1=(void*)((void*)((_tmpC14=_region_malloc(yyr,sizeof(*_tmpC14)),((_tmpC14[0]=((_tmpC15.tag=1,((_tmpC15.f1=_tmp60F,((_tmpC15.f2=_tmp610,((_tmpC15.f3=_tmp611,((_tmpC15.f4=(void*)_tmp612,((_tmpC15.f5=_tmp613,_tmpC15)))))))))))),_tmpC14))))),_tmpC16)))),_tmpC13)))),((_tmpC17->tl=_tmp614.tms,_tmpC17)))))),_tmpC12)))));}
# 1997
break;};}case 179: _LL2D0: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1998
struct Cyc_Parse_Declarator _tmp61B=Cyc_yyget_YY29(yyyyvsp[0]);
# 1999
{struct Cyc_List_List*_tmpC2C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC2B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC2A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC29;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC28;struct Cyc_Parse_Declarator _tmpC27;yyval=Cyc_YY29(((_tmpC27.id=_tmp61B.id,((_tmpC27.tms=(
# 2000
(_tmpC2C=_region_malloc(yyr,sizeof(*_tmpC2C)),((_tmpC2C->hd=(void*)((_tmpC28=_region_malloc(yyr,sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC2B.tag=3,((_tmpC2B.f1=(void*)((void*)((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=1,((_tmpC2A.f1=0,((_tmpC2A.f2=0,((_tmpC2A.f3=0,((_tmpC2A.f4=(void*)
# 2002
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC2A.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpC2A)))))))))))),_tmpC29))))),_tmpC2B)))),_tmpC28)))),((_tmpC2C->tl=_tmp61B.tms,_tmpC2C)))))),_tmpC27)))));}
# 2005
break;}case 180: _LL2D1: {
# 2002 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2006
struct Cyc_Parse_Declarator _tmp622=Cyc_yyget_YY29(yyyyvsp[0]);
# 2007
{struct Cyc_List_List*_tmpC41;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC40;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC3F;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC3E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC3D;struct Cyc_Parse_Declarator _tmpC3C;yyval=Cyc_YY29(((_tmpC3C.id=_tmp622.id,((_tmpC3C.tms=((_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41->hd=(void*)((_tmpC3D=_region_malloc(yyr,sizeof(*_tmpC3D)),((_tmpC3D[0]=((_tmpC40.tag=3,((_tmpC40.f1=(void*)((void*)((_tmpC3E=_region_malloc(yyr,sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC3F.tag=0,((_tmpC3F.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC3F.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC3F)))))),_tmpC3E))))),_tmpC40)))),_tmpC3D)))),((_tmpC41->tl=_tmp622.tms,_tmpC41)))))),_tmpC3C)))));}
# 2008
break;}case 181: _LL2D2: {
# 2006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2010
struct Cyc_List_List*_tmp629=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
# 2011
struct Cyc_Parse_Declarator _tmp62A=Cyc_yyget_YY29(yyyyvsp[0]);
# 2012
{struct Cyc_List_List*_tmpC4B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC4A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC49;struct Cyc_Parse_Declarator _tmpC48;yyval=Cyc_YY29(((_tmpC48.id=_tmp62A.id,((_tmpC48.tms=((_tmpC4B=_region_malloc(yyr,sizeof(*_tmpC4B)),((_tmpC4B->hd=(void*)((_tmpC49=_region_malloc(yyr,sizeof(*_tmpC49)),((_tmpC49[0]=((_tmpC4A.tag=4,((_tmpC4A.f1=_tmp629,((_tmpC4A.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC4A.f3=0,_tmpC4A)))))))),_tmpC49)))),((_tmpC4B->tl=_tmp62A.tms,_tmpC4B)))))),_tmpC48)))));}
# 2014
break;}case 182: _LL2D3: {
# 2011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2015
struct Cyc_Parse_Declarator _tmp62F=Cyc_yyget_YY29(yyyyvsp[0]);
# 2016
{struct Cyc_List_List*_tmpC55;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC54;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC53;struct Cyc_Parse_Declarator _tmpC52;yyval=Cyc_YY29(((_tmpC52.id=_tmp62F.id,((_tmpC52.tms=((_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55->hd=(void*)((_tmpC53=_region_malloc(yyr,sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=5,((_tmpC54.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC54.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC54)))))),_tmpC53)))),((_tmpC55->tl=_tmp62F.tms,_tmpC55)))))),_tmpC52)))));}
# 2018
break;}case 183: _LL2D4: {
# 2024 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2028
yyval=yyyyvsp[0];
# 2029
break;}case 184: _LL2D5: {
# 2025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2029
yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),Cyc_yyget_YY28(yyyyvsp[1])));
# 2030
break;}case 185: _LL2D6: {
# 2029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2033
struct Cyc_List_List*ans=0;
# 2034
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
# 2035
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC5B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC5A;struct Cyc_List_List*_tmpC59;ans=((_tmpC59=_region_malloc(yyr,sizeof(*_tmpC59)),((_tmpC59->hd=(void*)((_tmpC5B=_region_malloc(yyr,sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5A.tag=5,((_tmpC5A.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC5A.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC5A)))))),_tmpC5B)))),((_tmpC59->tl=ans,_tmpC59))))));}{
# 2037
struct Cyc_Absyn_PtrLoc*ptrloc=0;
# 2038
unsigned int _tmp638;union Cyc_Absyn_Constraint*_tmp639;union Cyc_Absyn_Constraint*_tmp63A;struct _tuple21 _tmp637=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp638=_tmp637.f1;_tmp639=_tmp637.f2;_tmp63A=_tmp637.f3;
# 2039
if(Cyc_Absyn_porting_c_code){
# 2040
struct Cyc_Absyn_PtrLoc*_tmpC5C;ptrloc=((_tmpC5C=_cycalloc_atomic(sizeof(*_tmpC5C)),((_tmpC5C->ptr_loc=_tmp638,((_tmpC5C->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC5C->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC5C))))))));}{
# 2042
union Cyc_Absyn_Constraint*_tmp63D;union Cyc_Absyn_Constraint*_tmp63E;union Cyc_Absyn_Constraint*_tmp63F;void*_tmp640;struct _tuple15 _tmp63C=Cyc_Parse_collapse_pointer_quals(_tmp638,_tmp639,_tmp63A,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;_tmp63F=_tmp63C.f3;_tmp640=_tmp63C.f4;
# 2043
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC66;struct Cyc_Absyn_PtrAtts _tmpC65;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC64;struct Cyc_List_List*_tmpC63;ans=((_tmpC63=_region_malloc(yyr,sizeof(*_tmpC63)),((_tmpC63->hd=(void*)((_tmpC66=_region_malloc(yyr,sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC64.tag=2,((_tmpC64.f1=((_tmpC65.rgn=_tmp640,((_tmpC65.nullable=_tmp63D,((_tmpC65.bounds=_tmp63E,((_tmpC65.zero_term=_tmp63F,((_tmpC65.ptrloc=ptrloc,_tmpC65)))))))))),((_tmpC64.f2=Cyc_yyget_YY25(yyyyvsp[4]),_tmpC64)))))),_tmpC66)))),((_tmpC63->tl=ans,_tmpC63))))));}
# 2044
yyval=Cyc_YY28(ans);
# 2046
break;};};}case 186: _LL2D7:
# 2044 "parse.y"
 yyval=Cyc_YY54(0);
# 2045
break;case 187: _LL2D8: {
# 2045 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2049
{struct Cyc_List_List*_tmpC67;yyval=Cyc_YY54(((_tmpC67=_region_malloc(yyr,sizeof(*_tmpC67)),((_tmpC67->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpC67->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpC67)))))));}
# 2050
break;}case 188: _LL2D9: {
# 2050 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2054
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC6A;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC69;yyval=Cyc_YY53((void*)((_tmpC69=_region_malloc(yyr,sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=0,((_tmpC6A.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpC6A)))),_tmpC69)))));}
# 2055
break;}case 189: _LL2DA: {
# 2052 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2056
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC6D;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC6C;yyval=Cyc_YY53((void*)((_tmpC6C=_region_malloc(yyr,sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=1,((_tmpC6D.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpC6D)))),_tmpC6C)))));}
# 2057
break;}case 190: _LL2DB: {
# 2054 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2058
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC70;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC6F;yyval=Cyc_YY53((void*)((_tmpC6F=_region_malloc(yyr,sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC70.tag=2,_tmpC70)),_tmpC6F)))));}
# 2059
break;}case 191: _LL2DC: {
# 2056 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2060
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC73;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC72;yyval=Cyc_YY53((void*)((_tmpC72=_region_malloc(yyr,sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=3,_tmpC73)),_tmpC72)))));}
# 2061
break;}case 192: _LL2DD: {
# 2058 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2062
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC76;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC75;yyval=Cyc_YY53((void*)((_tmpC75=_region_malloc(yyr,sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC76.tag=4,_tmpC76)),_tmpC75)))));}
# 2063
break;}case 193: _LL2DE: {
# 2060 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2064
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC79;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC78;yyval=Cyc_YY53((void*)((_tmpC78=_region_malloc(yyr,sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=5,_tmpC79)),_tmpC78)))));}
# 2065
break;}case 194: _LL2DF: {
# 2062 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2066
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC7C;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC7B;yyval=Cyc_YY53((void*)((_tmpC7B=_region_malloc(yyr,sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7C.tag=6,_tmpC7C)),_tmpC7B)))));}
# 2067
break;}case 195: _LL2E0: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2068
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC7F;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC7E;yyval=Cyc_YY53((void*)((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=7,_tmpC7F)),_tmpC7E)))));}
# 2069
break;}case 196: _LL2E1: {
# 2069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2074
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
# 2075
{struct _tuple21*_tmpC80;yyval=Cyc_YY1(((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->f1=loc,((_tmpC80->f2=Cyc_Absyn_true_conref,((_tmpC80->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC80)))))))));}
# 2077
break;}case 197: _LL2E2: {
# 2074 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2078
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
# 2079
{struct _tuple21*_tmpC81;yyval=Cyc_YY1(((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->f1=loc,((_tmpC81->f2=Cyc_Absyn_false_conref,((_tmpC81->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC81)))))))));}
# 2081
break;}case 198: _LL2E3: {
# 2078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2082
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
# 2083
{struct _tuple21*_tmpC82;yyval=Cyc_YY1(((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->f1=loc,((_tmpC82->f2=Cyc_Absyn_true_conref,((_tmpC82->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC82)))))))));}
# 2085
break;}case 199: _LL2E4:
# 2083 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
# 2084
break;case 200: _LL2E5: {
# 2084 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2088
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC85;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC84;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84[0]=((_tmpC85.tag=1,((_tmpC85.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpC85)))),_tmpC84))))));}
# 2089
break;}case 201: _LL2E6:
# 2087 "parse.y"
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
# 2088
break;case 202: _LL2E7: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2092
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
# 2093
break;}case 203: _LL2E8: {
# 2089 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2093
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
# 2094
break;}case 204: _LL2E9:
# 2094 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
# 2095
break;case 205: _LL2EA: {
# 2095 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2099
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
# 2100
break;}case 206: _LL2EB: {
# 2096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2100
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
# 2101
break;}case 207: _LL2EC:
# 2100 "parse.y"
 yyval=Cyc_YY25(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
# 2101
break;case 208: _LL2ED: {
# 2101 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2105
yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_YY25(yyyyvsp[1])));
# 2106
break;}case 209: _LL2EE: {
# 2106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2110
{struct _tuple25*_tmpC86;yyval=Cyc_YY39(((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC86->f2=0,((_tmpC86->f3=0,((_tmpC86->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpC86->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpC86)))))))))))));}
# 2111
break;}case 210: _LL2EF: {
# 2108 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2112
{struct _tuple25*_tmpC87;yyval=Cyc_YY39(((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC87->f2=1,((_tmpC87->f3=0,((_tmpC87->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC87->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC87)))))))))))));}
# 2113
break;}case 211: _LL2F0: {
# 2111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2115
struct _tuple8 _tmp65E;struct _dyneither_ptr*_tmp65F;struct Cyc_Absyn_Tqual _tmp660;void*_tmp661;struct _tuple8*_tmp65D=Cyc_yyget_YY37(yyyyvsp[2]);_tmp65E=*_tmp65D;_tmp65F=_tmp65E.f1;_tmp660=_tmp65E.f2;_tmp661=_tmp65E.f3;{
# 2116
struct Cyc_Absyn_VarargInfo*_tmpC88;struct Cyc_Absyn_VarargInfo*_tmp662=(_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88->name=_tmp65F,((_tmpC88->tq=_tmp660,((_tmpC88->type=_tmp661,((_tmpC88->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpC88)))))))));
# 2117
{struct _tuple25*_tmpC89;yyval=Cyc_YY39(((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->f1=0,((_tmpC89->f2=0,((_tmpC89->f3=_tmp662,((_tmpC89->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC89->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC89)))))))))))));}
# 2119
break;};}case 212: _LL2F1: {
# 2117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2121
struct _tuple8 _tmp666;struct _dyneither_ptr*_tmp667;struct Cyc_Absyn_Tqual _tmp668;void*_tmp669;struct _tuple8*_tmp665=Cyc_yyget_YY37(yyyyvsp[4]);_tmp666=*_tmp665;_tmp667=_tmp666.f1;_tmp668=_tmp666.f2;_tmp669=_tmp666.f3;{
# 2122
struct Cyc_Absyn_VarargInfo*_tmpC8A;struct Cyc_Absyn_VarargInfo*_tmp66A=(_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->name=_tmp667,((_tmpC8A->tq=_tmp668,((_tmpC8A->type=_tmp669,((_tmpC8A->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpC8A)))))))));
# 2123
{struct _tuple25*_tmpC8B;yyval=Cyc_YY39(((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC8B->f2=0,((_tmpC8B->f3=_tmp66A,((_tmpC8B->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpC8B->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpC8B)))))))))))));}
# 2125
break;};}case 213: _LL2F2: {
# 2125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2129
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC8E;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC8D;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=1,((_tmpC8E.f1=0,_tmpC8E)))),_tmpC8D))))));}
# 2130
break;}case 214: _LL2F3: {
# 2126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2130
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
# 2131
break;}case 215: _LL2F4:
# 2129 "parse.y"
 yyval=Cyc_YY49(0);
# 2130
break;case 216: _LL2F5: {
# 2130 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2134
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC91;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC90;yyval=Cyc_YY49((void*)((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90[0]=((_tmpC91.tag=24,((_tmpC91.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpC91)))),_tmpC90)))));}
# 2135
break;}case 217: _LL2F6:
# 2134 "parse.y"
 yyval=Cyc_YY50(0);
# 2135
break;case 218: _LL2F7: {
# 2135 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2139
yyval=yyyyvsp[1];
# 2140
break;}case 219: _LL2F8: {
# 2140 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2147
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC94;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC93;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp671=(_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=2,((_tmpC94.f1=0,((_tmpC94.f2=& Cyc_Tcutil_trk,_tmpC94)))))),_tmpC93)));
# 2148
struct _dyneither_ptr _tmp672=Cyc_yyget_String_tok(yyyyvsp[2]);
# 2149
void*_tmp673=Cyc_Parse_id2type(_tmp672,(void*)_tmp671);
# 2150
{struct _tuple28*_tmpC9E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC9D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC9C;struct Cyc_List_List*_tmpC9B;yyval=Cyc_YY50(((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->hd=((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=(void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=24,((_tmpC9D.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpC9D)))),_tmpC9C)))),((_tmpC9E->f2=_tmp673,_tmpC9E)))))),((_tmpC9B->tl=0,_tmpC9B)))))));}
# 2152
break;}case 220: _LL2F9: {
# 2149 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2154
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCA1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCA0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp67A=(_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpCA1.tag=2,((_tmpCA1.f1=0,((_tmpCA1.f2=& Cyc_Tcutil_trk,_tmpCA1)))))),_tmpCA0)));
# 2155
struct _dyneither_ptr _tmp67B=Cyc_yyget_String_tok(yyyyvsp[2]);
# 2156
void*_tmp67C=Cyc_Parse_id2type(_tmp67B,(void*)_tmp67A);
# 2157
{struct _tuple28*_tmpCAB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCAA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCA9;struct Cyc_List_List*_tmpCA8;yyval=Cyc_YY50(((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->hd=((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=(void*)((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=24,((_tmpCAA.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCAA)))),_tmpCA9)))),((_tmpCAB->f2=_tmp67C,_tmpCAB)))))),((_tmpCA8->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCA8)))))));}
# 2159
break;}case 221: _LL2FA:
# 2159 "parse.y"
 yyval=Cyc_YY31(0);
# 2160
break;case 222: _LL2FB: {
# 2161 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2165
{const char*_tmpCAC;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCAC="inject",_tag_dyneither(_tmpCAC,sizeof(char),7))))!= 0){
# 2166
const char*_tmpCAD;Cyc_Parse_err(((_tmpCAD="missing type in function declaration",_tag_dyneither(_tmpCAD,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
# 2167
yyval=Cyc_YY31(1);
# 2169
break;}case 223: _LL2FC: {
# 2168 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2172
yyval=yyyyvsp[0];
# 2173
break;}case 224: _LL2FD: {
# 2169 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2173
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
# 2174
break;}case 225: _LL2FE: {
# 2173 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2177
yyval=Cyc_YY40(0);
# 2178
break;}case 226: _LL2FF: {
# 2174 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2178
yyval=yyyyvsp[1];
# 2179
break;}case 227: _LL300: {
# 2176 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2180
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCB3;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCB2;struct Cyc_List_List*_tmpCB1;yyval=Cyc_YY40(((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=(void*)((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB2.tag=25,((_tmpCB2.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpCB2)))),_tmpCB3)))),((_tmpCB1->tl=0,_tmpCB1)))))));}
# 2181
break;}case 228: _LL301: {
# 2178 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2182
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
# 2183
{struct Cyc_List_List*_tmpCB4;yyval=Cyc_YY40(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCB4->tl=0,_tmpCB4)))))));}
# 2185
break;}case 229: _LL302: {
# 2186 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2190
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
# 2191
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCBA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCB9;struct Cyc_List_List*_tmpCB8;yyval=Cyc_YY40(((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->hd=(void*)((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCB9.tag=23,((_tmpCB9.f1=(void*)Cyc_yyget_YY44(yyyyvsp[0]),_tmpCB9)))),_tmpCBA)))),((_tmpCB8->tl=0,_tmpCB8)))))));}
# 2193
break;}case 230: _LL303: {
# 2190 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2194
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
# 2195
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCC0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCBF;struct Cyc_List_List*_tmpCBE;yyval=Cyc_YY40(((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->hd=(void*)((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCBF.tag=23,((_tmpCBF.f1=(void*)Cyc_yyget_YY44(yyyyvsp[0]),_tmpCBF)))),_tmpCC0)))),((_tmpCBE->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCBE)))))));}
# 2197
break;}case 231: _LL304: {
# 2198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2202
{struct Cyc_List_List*_tmpCC1;yyval=Cyc_YY38(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpCC1->tl=0,_tmpCC1)))))));}
# 2203
break;}case 232: _LL305: {
# 2200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2204
{struct Cyc_List_List*_tmpCC2;yyval=Cyc_YY38(((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpCC2->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpCC2)))))));}
# 2205
break;}case 233: _LL306: {
# 2206 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2210
struct Cyc_Absyn_Tqual _tmp692;struct Cyc_Parse_Type_specifier _tmp693;struct Cyc_List_List*_tmp694;struct _tuple24 _tmp691=Cyc_yyget_YY35(yyyyvsp[0]);_tmp692=_tmp691.f1;_tmp693=_tmp691.f2;_tmp694=_tmp691.f3;
# 2211
if(_tmp692.loc == 0)_tmp692.loc=(unsigned int)(yyyylsp[0]).first_line;{
# 2212
struct _tuple0*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_Parse_Declarator _tmp695=Cyc_yyget_YY29(yyyyvsp[1]);_tmp696=_tmp695.id;_tmp697=_tmp695.tms;{
# 2213
void*_tmp698=Cyc_Parse_speclist2typ(_tmp693,(unsigned int)(yyyylsp[0]).first_line);
# 2214
struct Cyc_Absyn_Tqual _tmp69A;void*_tmp69B;struct Cyc_List_List*_tmp69C;struct Cyc_List_List*_tmp69D;struct _tuple14 _tmp699=Cyc_Parse_apply_tms(_tmp692,_tmp698,_tmp694,_tmp697);_tmp69A=_tmp699.f1;_tmp69B=_tmp699.f2;_tmp69C=_tmp699.f3;_tmp69D=_tmp699.f4;
# 2215
if(_tmp69C != 0){
# 2216
const char*_tmpCC3;Cyc_Parse_err(((_tmpCC3="parameter with bad type params",_tag_dyneither(_tmpCC3,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
# 2217
if(Cyc_Absyn_is_qvar_qualified(_tmp696)){
# 2218
const char*_tmpCC4;Cyc_Parse_err(((_tmpCC4="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCC4,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
# 2219
struct _dyneither_ptr*idopt=(struct _dyneither_ptr*)(*_tmp696).f2;
# 2220
if(_tmp69D != 0){
# 2221
const char*_tmpCC7;void*_tmpCC6;(_tmpCC6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCC7="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCC7,sizeof(char),40))),_tag_dyneither(_tmpCC6,sizeof(void*),0)));}
# 2222
{struct _tuple8*_tmpCC8;yyval=Cyc_YY37(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->f1=idopt,((_tmpCC8->f2=_tmp69A,((_tmpCC8->f3=_tmp69B,_tmpCC8)))))))));}
# 2224
break;};};};}case 234: _LL307: {
# 2221 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2225
struct Cyc_Absyn_Tqual _tmp6A4;struct Cyc_Parse_Type_specifier _tmp6A5;struct Cyc_List_List*_tmp6A6;struct _tuple24 _tmp6A3=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6A4=_tmp6A3.f1;_tmp6A5=_tmp6A3.f2;_tmp6A6=_tmp6A3.f3;
# 2226
if(_tmp6A4.loc == 0)_tmp6A4.loc=(unsigned int)(yyyylsp[0]).first_line;{
# 2227
void*_tmp6A7=Cyc_Parse_speclist2typ(_tmp6A5,(unsigned int)(yyyylsp[0]).first_line);
# 2228
if(_tmp6A6 != 0){
# 2229
const char*_tmpCCB;void*_tmpCCA;(_tmpCCA=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCB="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCCB,sizeof(char),38))),_tag_dyneither(_tmpCCA,sizeof(void*),0)));}
# 2230
{struct _tuple8*_tmpCCC;yyval=Cyc_YY37(((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->f1=0,((_tmpCCC->f2=_tmp6A4,((_tmpCCC->f3=_tmp6A7,_tmpCCC)))))))));}
# 2232
break;};}case 235: _LL308: {
# 2229 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2233
struct Cyc_Absyn_Tqual _tmp6AC;struct Cyc_Parse_Type_specifier _tmp6AD;struct Cyc_List_List*_tmp6AE;struct _tuple24 _tmp6AB=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6AC=_tmp6AB.f1;_tmp6AD=_tmp6AB.f2;_tmp6AE=_tmp6AB.f3;
# 2234
if(_tmp6AC.loc == 0)_tmp6AC.loc=(unsigned int)(yyyylsp[0]).first_line;{
# 2235
void*_tmp6AF=Cyc_Parse_speclist2typ(_tmp6AD,(unsigned int)(yyyylsp[0]).first_line);
# 2236
struct Cyc_List_List*_tmp6B0=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
# 2237
struct Cyc_Absyn_Tqual _tmp6B2;void*_tmp6B3;struct Cyc_List_List*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct _tuple14 _tmp6B1=Cyc_Parse_apply_tms(_tmp6AC,_tmp6AF,_tmp6AE,_tmp6B0);_tmp6B2=_tmp6B1.f1;_tmp6B3=_tmp6B1.f2;_tmp6B4=_tmp6B1.f3;_tmp6B5=_tmp6B1.f4;
# 2238
if(_tmp6B4 != 0){
# 2239
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCF="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCCF,sizeof(char),49))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}
# 2241
if(_tmp6B5 != 0){
# 2242
const char*_tmpCD2;void*_tmpCD1;(_tmpCD1=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCD2="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCD2,sizeof(char),38))),_tag_dyneither(_tmpCD1,sizeof(void*),0)));}
# 2243
{struct _tuple8*_tmpCD3;yyval=Cyc_YY37(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->f1=0,((_tmpCD3->f2=_tmp6B2,((_tmpCD3->f3=_tmp6B3,_tmpCD3)))))))));}
# 2245
break;};}case 236: _LL309: {
# 2245 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2249
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
# 2250
break;}case 237: _LL30A: {
# 2249 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2253
{struct _dyneither_ptr*_tmpCD6;struct Cyc_List_List*_tmpCD5;yyval=Cyc_YY36(((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCD6)))),((_tmpCD5->tl=0,_tmpCD5)))))));}
# 2254
break;}case 238: _LL30B: {
# 2251 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2255
{struct _dyneither_ptr*_tmpCD9;struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY36(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCD9)))),((_tmpCD8->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpCD8)))))));}
# 2256
break;}case 239: _LL30C: {
# 2255 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2259
yyval=yyyyvsp[0];
# 2260
break;}case 240: _LL30D: {
# 2256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2260
yyval=yyyyvsp[0];
# 2261
break;}case 241: _LL30E: {
# 2261 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2265
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCDC;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCDB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=34,((_tmpCDC.f1=0,((_tmpCDC.f2=0,_tmpCDC)))))),_tmpCDB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2266
break;}case 242: _LL30F: {
# 2263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2267
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCDF;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCDE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDF.tag=34,((_tmpCDF.f1=0,((_tmpCDF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpCDF)))))),_tmpCDE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2268
break;}case 243: _LL310: {
# 2265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2269
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCE2;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCE1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=34,((_tmpCE2.f1=0,((_tmpCE2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpCE2)))))),_tmpCE1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2270
break;}case 244: _LL311: {
# 2267 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2271
struct _dyneither_ptr*_tmpCE5;struct _tuple0*_tmpCE4;struct Cyc_Absyn_Vardecl*_tmp6C5=Cyc_Absyn_new_vardecl(((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->f1=Cyc_Absyn_Loc_n,((_tmpCE4->f2=((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCE5)))),_tmpCE4)))))),Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)
# 2272
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2274
(_tmp6C5->tq).real_const=1;
# 2275
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCE8;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCE7;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=26,((_tmpCE8.f1=_tmp6C5,((_tmpCE8.f2=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpCE8.f3=Cyc_yyget_YY3(yyyyvsp[6]),((_tmpCE8.f4=0,_tmpCE8)))))))))),_tmpCE7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2277
break;}case 245: _LL312: {
# 2278 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2282
{struct _tuple29*_tmpCEB;struct Cyc_List_List*_tmpCEA;yyval=Cyc_YY6(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->f1=0,((_tmpCEB->f2=Cyc_yyget_YY3(yyyyvsp[0]),_tmpCEB)))))),((_tmpCEA->tl=0,_tmpCEA)))))));}
# 2283
break;}case 246: _LL313: {
# 2280 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2284
{struct _tuple29*_tmpCEE;struct Cyc_List_List*_tmpCED;yyval=Cyc_YY6(((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->hd=((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpCEE->f2=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCEE)))))),((_tmpCED->tl=0,_tmpCED)))))));}
# 2285
break;}case 247: _LL314: {
# 2282 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2286
{struct _tuple29*_tmpCF1;struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY6(((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=0,((_tmpCF1->f2=Cyc_yyget_YY3(yyyyvsp[2]),_tmpCF1)))))),((_tmpCF0->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCF0)))))));}
# 2287
break;}case 248: _LL315: {
# 2284 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2288
{struct _tuple29*_tmpCF4;struct Cyc_List_List*_tmpCF3;yyval=Cyc_YY6(((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpCF4->f2=Cyc_yyget_YY3(yyyyvsp[3]),_tmpCF4)))))),((_tmpCF3->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCF3)))))));}
# 2289
break;}case 249: _LL316: {
# 2288 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2292
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
# 2293
break;}case 250: _LL317: {
# 2293 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2297
{struct Cyc_List_List*_tmpCF5;yyval=Cyc_YY41(((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpCF5->tl=0,_tmpCF5)))))));}
# 2298
break;}case 251: _LL318: {
# 2294 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2298
{struct Cyc_List_List*_tmpCF6;yyval=Cyc_YY41(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpCF6->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCF6)))))));}
# 2299
break;}case 252: _LL319: {
# 2298 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2302
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCF9;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCF8;yyval=Cyc_YY42((void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF9.tag=0,((_tmpCF9.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCF9)))),_tmpCF8)))));}
# 2303
break;}case 253: _LL31A: {
# 2299 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2303
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCFF;struct _dyneither_ptr*_tmpCFE;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCFD;yyval=Cyc_YY42((void*)((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD[0]=((_tmpCFF.tag=1,((_tmpCFF.f1=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCFE)))),_tmpCFF)))),_tmpCFD)))));}
# 2304
break;}case 254: _LL31B: {
# 2304 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2308
struct Cyc_Absyn_Tqual _tmp6DA;struct Cyc_Parse_Type_specifier _tmp6DB;struct Cyc_List_List*_tmp6DC;struct _tuple24 _tmp6D9=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6DA=_tmp6D9.f1;_tmp6DB=_tmp6D9.f2;_tmp6DC=_tmp6D9.f3;{
# 2309
void*_tmp6DD=Cyc_Parse_speclist2typ(_tmp6DB,(unsigned int)(yyyylsp[0]).first_line);
# 2310
if(_tmp6DC != 0){
# 2311
const char*_tmpD02;void*_tmpD01;(_tmpD01=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD02="ignoring attributes in type",_tag_dyneither(_tmpD02,sizeof(char),28))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}
# 2312
{struct _tuple8*_tmpD03;yyval=Cyc_YY37(((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->f1=0,((_tmpD03->f2=_tmp6DA,((_tmpD03->f3=_tmp6DD,_tmpD03)))))))));}
# 2314
break;};}case 255: _LL31C: {
# 2311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2315
struct Cyc_Absyn_Tqual _tmp6E2;struct Cyc_Parse_Type_specifier _tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple24 _tmp6E1=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=_tmp6E1.f3;{
# 2316
void*_tmp6E5=Cyc_Parse_speclist2typ(_tmp6E3,(unsigned int)(yyyylsp[0]).first_line);
# 2317
struct Cyc_List_List*_tmp6E6=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
# 2318
struct _tuple14 _tmp6E7=Cyc_Parse_apply_tms(_tmp6E2,_tmp6E5,_tmp6E4,_tmp6E6);
# 2319
if(_tmp6E7.f3 != 0){
# 2321
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD06="bad type params, ignoring",_tag_dyneither(_tmpD06,sizeof(char),26))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
# 2322
if(_tmp6E7.f4 != 0){
# 2323
const char*_tmpD09;void*_tmpD08;(_tmpD08=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD09="bad specifiers, ignoring",_tag_dyneither(_tmpD09,sizeof(char),25))),_tag_dyneither(_tmpD08,sizeof(void*),0)));}
# 2324
{struct _tuple8*_tmpD0A;yyval=Cyc_YY37(((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->f1=0,((_tmpD0A->f2=_tmp6E7.f1,((_tmpD0A->f3=_tmp6E7.f2,_tmpD0A)))))))));}
# 2326
break;};}case 256: _LL31D: {
# 2325 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2329
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 2330
break;}case 257: _LL31E: {
# 2326 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2330
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD0D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD0C;yyval=Cyc_YY44((void*)((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C[0]=((_tmpD0D.tag=24,((_tmpD0D.f1=0,_tmpD0D)))),_tmpD0C)))));}
# 2331
break;}case 258: _LL31F: {
# 2327 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2331
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD10;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD0F;yyval=Cyc_YY44((void*)((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F[0]=((_tmpD10.tag=24,((_tmpD10.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD10)))),_tmpD0F)))));}
# 2332
break;}case 259: _LL320: {
# 2328 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2332
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD13;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD12;yyval=Cyc_YY44((void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD13.tag=25,((_tmpD13.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpD13)))),_tmpD12)))));}
# 2333
break;}case 260: _LL321: {
# 2329 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2333
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD19;struct Cyc_List_List*_tmpD18;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD17;yyval=Cyc_YY44((void*)((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD19.tag=24,((_tmpD19.f1=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD18->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD18)))))),_tmpD19)))),_tmpD17)))));}
# 2334
break;}case 261: _LL322: {
# 2335 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2339
{struct Cyc_List_List*_tmpD1A;yyval=Cyc_YY40(((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD1A->tl=0,_tmpD1A)))))));}
# 2340
break;}case 262: _LL323: {
# 2336 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2340
{struct Cyc_List_List*_tmpD1B;yyval=Cyc_YY40(((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD1B->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD1B)))))));}
# 2341
break;}case 263: _LL324: {
# 2341 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2345
{struct Cyc_Parse_Abstractdeclarator _tmpD1C;yyval=Cyc_YY30(((_tmpD1C.tms=Cyc_yyget_YY28(yyyyvsp[0]),_tmpD1C)));}
# 2346
break;}case 264: _LL325: {
# 2343 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2347
yyval=yyyyvsp[0];
# 2348
break;}case 265: _LL326: {
# 2345 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2349
{struct Cyc_Parse_Abstractdeclarator _tmpD1D;yyval=Cyc_YY30(((_tmpD1D.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD1D)));}
# 2350
break;}case 266: _LL327: {
# 2350 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2354
yyval=yyyyvsp[1];
# 2355
break;}case 267: _LL328: {
# 2352 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2356
{struct Cyc_List_List*_tmpD27;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD26;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD25;struct Cyc_Parse_Abstractdeclarator _tmpD24;yyval=Cyc_YY30(((_tmpD24.tms=((_tmpD27=_region_malloc(yyr,sizeof(*_tmpD27)),((_tmpD27->hd=(void*)((_tmpD25=_region_malloc(yyr,sizeof(*_tmpD25)),((_tmpD25[0]=((_tmpD26.tag=0,((_tmpD26.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD26.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD26)))))),_tmpD25)))),((_tmpD27->tl=0,_tmpD27)))))),_tmpD24)));}
# 2357
break;}case 268: _LL329: {
# 2354 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2358
{struct Cyc_List_List*_tmpD31;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD30;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2F;struct Cyc_Parse_Abstractdeclarator _tmpD2E;yyval=Cyc_YY30(((_tmpD2E.tms=((_tmpD31=_region_malloc(yyr,sizeof(*_tmpD31)),((_tmpD31->hd=(void*)((_tmpD2F=_region_malloc(yyr,sizeof(*_tmpD2F)),((_tmpD2F[0]=((_tmpD30.tag=0,((_tmpD30.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD30.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD30)))))),_tmpD2F)))),((_tmpD31->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD31)))))),_tmpD2E)));}
# 2359
break;}case 269: _LL32A: {
# 2356 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2360
{struct Cyc_List_List*_tmpD3B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD3A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD39;struct Cyc_Parse_Abstractdeclarator _tmpD38;yyval=Cyc_YY30(((_tmpD38.tms=((_tmpD3B=_region_malloc(yyr,sizeof(*_tmpD3B)),((_tmpD3B->hd=(void*)((_tmpD39=_region_malloc(yyr,sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD3A.tag=1,((_tmpD3A.f1=Cyc_yyget_YY3(yyyyvsp[1]),((_tmpD3A.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD3A.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD3A)))))))),_tmpD39)))),((_tmpD3B->tl=0,_tmpD3B)))))),_tmpD38)));}
# 2361
break;}case 270: _LL32B: {
# 2358 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2362
{struct Cyc_List_List*_tmpD45;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD44;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD43;struct Cyc_Parse_Abstractdeclarator _tmpD42;yyval=Cyc_YY30(((_tmpD42.tms=((_tmpD45=_region_malloc(yyr,sizeof(*_tmpD45)),((_tmpD45->hd=(void*)((_tmpD43=_region_malloc(yyr,sizeof(*_tmpD43)),((_tmpD43[0]=((_tmpD44.tag=1,((_tmpD44.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpD44.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD44.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD44)))))))),_tmpD43)))),((_tmpD45->tl=(
# 2363
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD45)))))),_tmpD42)));}
# 2365
break;}case 271: _LL32C: {
# 2362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2366
{struct Cyc_List_List*_tmpD5A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD59;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD58;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD57;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD56;struct Cyc_Parse_Abstractdeclarator _tmpD55;yyval=Cyc_YY30(((_tmpD55.tms=((_tmpD5A=_region_malloc(yyr,sizeof(*_tmpD5A)),((_tmpD5A->hd=(void*)((_tmpD56=_region_malloc(yyr,sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD59.tag=3,((_tmpD59.f1=(void*)((void*)((_tmpD57=_region_malloc(yyr,sizeof(*_tmpD57)),((_tmpD57[0]=((_tmpD58.tag=1,((_tmpD58.f1=0,((_tmpD58.f2=0,((_tmpD58.f3=0,((_tmpD58.f4=(void*)Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD58.f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpD58)))))))))))),_tmpD57))))),_tmpD59)))),_tmpD56)))),((_tmpD5A->tl=0,_tmpD5A)))))),_tmpD55)));}
# 2368
break;}case 272: _LL32D: {
# 2365 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2369
struct _tuple25 _tmp711;struct Cyc_List_List*_tmp712;int _tmp713;struct Cyc_Absyn_VarargInfo*_tmp714;void*_tmp715;struct Cyc_List_List*_tmp716;struct _tuple25*_tmp710=Cyc_yyget_YY39(yyyyvsp[1]);_tmp711=*_tmp710;_tmp712=_tmp711.f1;_tmp713=_tmp711.f2;_tmp714=_tmp711.f3;_tmp715=_tmp711.f4;_tmp716=_tmp711.f5;
# 2370
{struct Cyc_List_List*_tmpD6F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD6E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD6D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD6B;struct Cyc_Parse_Abstractdeclarator _tmpD6A;yyval=Cyc_YY30(((_tmpD6A.tms=((_tmpD6F=_region_malloc(yyr,sizeof(*_tmpD6F)),((_tmpD6F->hd=(void*)((_tmpD6B=_region_malloc(yyr,sizeof(*_tmpD6B)),((_tmpD6B[0]=((_tmpD6E.tag=3,((_tmpD6E.f1=(void*)((void*)((_tmpD6C=_region_malloc(yyr,sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=1,((_tmpD6D.f1=_tmp712,((_tmpD6D.f2=_tmp713,((_tmpD6D.f3=_tmp714,((_tmpD6D.f4=(void*)_tmp715,((_tmpD6D.f5=_tmp716,_tmpD6D)))))))))))),_tmpD6C))))),_tmpD6E)))),_tmpD6B)))),((_tmpD6F->tl=0,_tmpD6F)))))),_tmpD6A)));}
# 2372
break;}case 273: _LL32E: {
# 2369 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2373
{struct Cyc_List_List*_tmpD84;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD83;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD80;struct Cyc_Parse_Abstractdeclarator _tmpD7F;yyval=Cyc_YY30(((_tmpD7F.tms=((_tmpD84=_region_malloc(yyr,sizeof(*_tmpD84)),((_tmpD84->hd=(void*)((_tmpD80=_region_malloc(yyr,sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD83.tag=3,((_tmpD83.f1=(void*)((void*)((_tmpD81=_region_malloc(yyr,sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=1,((_tmpD82.f1=0,((_tmpD82.f2=0,((_tmpD82.f3=0,((_tmpD82.f4=(void*)Cyc_yyget_YY49(yyyyvsp[2]),((_tmpD82.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpD82)))))))))))),_tmpD81))))),_tmpD83)))),_tmpD80)))),((_tmpD84->tl=(
# 2374
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD84)))))),_tmpD7F)));}
# 2376
break;}case 274: _LL32F: {
# 2373 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2377
struct _tuple25 _tmp724;struct Cyc_List_List*_tmp725;int _tmp726;struct Cyc_Absyn_VarargInfo*_tmp727;void*_tmp728;struct Cyc_List_List*_tmp729;struct _tuple25*_tmp723=Cyc_yyget_YY39(yyyyvsp[2]);_tmp724=*_tmp723;_tmp725=_tmp724.f1;_tmp726=_tmp724.f2;_tmp727=_tmp724.f3;_tmp728=_tmp724.f4;_tmp729=_tmp724.f5;
# 2378
{struct Cyc_List_List*_tmpD99;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD98;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD97;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD96;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD95;struct Cyc_Parse_Abstractdeclarator _tmpD94;yyval=Cyc_YY30(((_tmpD94.tms=((_tmpD99=_region_malloc(yyr,sizeof(*_tmpD99)),((_tmpD99->hd=(void*)((_tmpD95=_region_malloc(yyr,sizeof(*_tmpD95)),((_tmpD95[0]=((_tmpD98.tag=3,((_tmpD98.f1=(void*)((void*)((_tmpD96=_region_malloc(yyr,sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=1,((_tmpD97.f1=_tmp725,((_tmpD97.f2=_tmp726,((_tmpD97.f3=_tmp727,((_tmpD97.f4=(void*)_tmp728,((_tmpD97.f5=_tmp729,_tmpD97)))))))))))),_tmpD96))))),_tmpD98)))),_tmpD95)))),((_tmpD99->tl=(
# 2379
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD99)))))),_tmpD94)));}
# 2381
break;}case 275: _LL330: {
# 2379 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2383
struct Cyc_List_List*_tmp730=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
# 2384
{struct Cyc_List_List*_tmpDA3;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDA2;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDA1;struct Cyc_Parse_Abstractdeclarator _tmpDA0;yyval=Cyc_YY30(((_tmpDA0.tms=((_tmpDA3=_region_malloc(yyr,sizeof(*_tmpDA3)),((_tmpDA3->hd=(void*)((_tmpDA1=_region_malloc(yyr,sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA2.tag=4,((_tmpDA2.f1=_tmp730,((_tmpDA2.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDA2.f3=0,_tmpDA2)))))))),_tmpDA1)))),((_tmpDA3->tl=(
# 2385
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDA3)))))),_tmpDA0)));}
# 2387
break;}case 276: _LL331: {
# 2384 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2388
{struct Cyc_List_List*_tmpDAD;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDAC;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDAB;struct Cyc_Parse_Abstractdeclarator _tmpDAA;yyval=Cyc_YY30(((_tmpDAA.tms=((_tmpDAD=_region_malloc(yyr,sizeof(*_tmpDAD)),((_tmpDAD->hd=(void*)((_tmpDAB=_region_malloc(yyr,sizeof(*_tmpDAB)),((_tmpDAB[0]=((_tmpDAC.tag=5,((_tmpDAC.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDAC.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpDAC)))))),_tmpDAB)))),((_tmpDAD->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDAD)))))),_tmpDAA)));}
# 2390
break;}case 277: _LL332: {
# 2390 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2394
yyval=yyyyvsp[0];
# 2395
break;}case 278: _LL333: {
# 2391 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2395
yyval=yyyyvsp[0];
# 2396
break;}case 279: _LL334: {
# 2392 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2396
yyval=yyyyvsp[0];
# 2397
break;}case 280: _LL335: {
# 2393 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2397
yyval=yyyyvsp[0];
# 2398
break;}case 281: _LL336: {
# 2394 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2398
yyval=yyyyvsp[0];
# 2399
break;}case 282: _LL337: {
# 2395 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2399
yyval=yyyyvsp[0];
# 2400
break;}case 283: _LL338: {
# 2398 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2402
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDB0;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDAF;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDB0.tag=16,((_tmpDB0.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpDB0)))),_tmpDAF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2403
break;}case 284: _LL339:
# 2402 "parse.y"
 yyval=Cyc_YY4(0);
# 2403
break;case 285: _LL33A: {
# 2404 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2408
{const char*_tmpDB1;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDB1="open",_tag_dyneither(_tmpDB1,sizeof(char),5))))!= 0){
# 2409
const char*_tmpDB2;Cyc_Parse_err(((_tmpDB2="expecting `open'",_tag_dyneither(_tmpDB2,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
# 2410
yyval=Cyc_YY4((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]));
# 2412
break;}case 286: _LL33B: {
# 2413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2417
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDB8;struct _dyneither_ptr*_tmpDB7;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDB6;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB8.tag=13,((_tmpDB8.f1=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDB7)))),((_tmpDB8.f2=Cyc_yyget_YY9(yyyyvsp[2]),_tmpDB8)))))),_tmpDB6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2418
break;}case 287: _LL33C: {
# 2417 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2421
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
# 2422
break;}case 288: _LL33D: {
# 2418 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2422
yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 2423
break;}case 289: _LL33E: {
# 2423 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2427
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
# 2428
break;}case 290: _LL33F: {
# 2424 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2428
yyval=yyyyvsp[1];
# 2429
break;}case 291: _LL340: {
# 2429 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2433
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
# 2434
break;}case 292: _LL341: {
# 2430 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2434
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1])));
# 2435
break;}case 293: _LL342: {
# 2431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2435
yyval=yyyyvsp[0];
# 2436
break;}case 294: _LL343: {
# 2432 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2436
yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2437
break;}case 295: _LL344: {
# 2433 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2437
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDBB;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDBA;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA[0]=((_tmpDBB.tag=1,((_tmpDBB.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpDBB)))),_tmpDBA)))),(unsigned int)(yyyylsp[0]).first_line),
# 2438
Cyc_Absyn_skip_stmt(0)));}
# 2439
break;}case 296: _LL345: {
# 2436 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2440
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDBE;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDBD;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBE.tag=1,((_tmpDBE.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpDBE)))),_tmpDBD)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY9(yyyyvsp[1])));}
# 2441
break;}case 297: _LL346: {
# 2441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2445
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
# 2446
break;}case 298: _LL347: {
# 2443 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2447
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2448
break;}case 299: _LL348: {
# 2449 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2453
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
# 2454
break;}case 300: _LL349: {
# 2452 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2456
struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
# 2457
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp744,Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
# 2458
break;}case 301: _LL34A: {
# 2456 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2460
struct Cyc_Absyn_Exp*_tmp745=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
# 2461
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp745,Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2463
break;}case 302: _LL34B: {
# 2462 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2466
yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2467
break;}case 303: _LL34C:
# 2476 "parse.y"
 yyval=Cyc_YY10(0);
# 2477
break;case 304: _LL34D: {
# 2478 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2482
{struct Cyc_Absyn_Switch_clause*_tmpDC1;struct Cyc_List_List*_tmpDC0;yyval=Cyc_YY10(((_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->hd=((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDC1->pat_vars=0,((_tmpDC1->where_clause=0,((_tmpDC1->body=
# 2483
Cyc_yyget_YY9(yyyyvsp[2]),((_tmpDC1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDC1)))))))))))),((_tmpDC0->tl=0,_tmpDC0)))))));}
# 2485
break;}case 305: _LL34E: {
# 2482 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2486
{struct Cyc_Absyn_Switch_clause*_tmpDC4;struct Cyc_List_List*_tmpDC3;yyval=Cyc_YY10(((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->hd=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpDC4->pat_vars=0,((_tmpDC4->where_clause=0,((_tmpDC4->body=
# 2487
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDC4->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDC4)))))))))))),((_tmpDC3->tl=
# 2488
Cyc_yyget_YY10(yyyyvsp[3]),_tmpDC3)))))));}
# 2489
break;}case 306: _LL34F: {
# 2486 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2490
{struct Cyc_Absyn_Switch_clause*_tmpDC7;struct Cyc_List_List*_tmpDC6;yyval=Cyc_YY10(((_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->hd=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpDC7->pat_vars=0,((_tmpDC7->where_clause=0,((_tmpDC7->body=Cyc_yyget_YY9(yyyyvsp[3]),((_tmpDC7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDC7)))))))))))),((_tmpDC6->tl=Cyc_yyget_YY10(yyyyvsp[4]),_tmpDC6)))))));}
# 2491
break;}case 307: _LL350: {
# 2488 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2492
{struct Cyc_Absyn_Switch_clause*_tmpDCA;struct Cyc_List_List*_tmpDC9;yyval=Cyc_YY10(((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->hd=((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpDCA->pat_vars=0,((_tmpDCA->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpDCA->body=
# 2493
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDCA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDCA)))))))))))),((_tmpDC9->tl=
# 2494
Cyc_yyget_YY10(yyyyvsp[5]),_tmpDC9)))))));}
# 2495
break;}case 308: _LL351: {
# 2492 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2496
{struct Cyc_Absyn_Switch_clause*_tmpDCD;struct Cyc_List_List*_tmpDCC;yyval=Cyc_YY10(((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->hd=((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpDCD->pat_vars=0,((_tmpDCD->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpDCD->body=Cyc_yyget_YY9(yyyyvsp[5]),((_tmpDCD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDCD)))))))))))),((_tmpDCC->tl=Cyc_yyget_YY10(yyyyvsp[6]),_tmpDCC)))))));}
# 2497
break;}case 309: _LL352: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2503
yyval=Cyc_YY9(Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2504
break;}case 310: _LL353: {
# 2503 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2507
yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2508
break;}case 311: _LL354: {
# 2507 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2511
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
# 2512
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
# 2513
break;}case 312: _LL355: {
# 2510 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2514
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
# 2515
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2516
break;}case 313: _LL356: {
# 2513 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2517
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
# 2518
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2519
break;}case 314: _LL357: {
# 2516 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2520
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
# 2521
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
# 2522
break;}case 315: _LL358: {
# 2519 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2523
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
# 2524
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2525
break;}case 316: _LL359: {
# 2522 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2526
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[5]),
# 2527
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
# 2528
break;}case 317: _LL35A: {
# 2525 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2529
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
# 2530
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
# 2531
break;}case 318: _LL35B: {
# 2528 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2532
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY3(yyyyvsp[6]),
# 2533
Cyc_yyget_YY9(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
# 2534
break;}case 319: _LL35C: {
# 2531 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2535
struct Cyc_List_List*_tmp750=Cyc_yyget_YY18(yyyyvsp[2]);
# 2536
struct Cyc_Absyn_Stmt*_tmp751=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
# 2537
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
# 2538
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp750,_tmp751));
# 2540
break;}case 320: _LL35D: {
# 2537 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2541
struct Cyc_List_List*_tmp752=Cyc_yyget_YY18(yyyyvsp[2]);
# 2542
struct Cyc_Absyn_Stmt*_tmp753=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
# 2543
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
# 2544
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp752,_tmp753));
# 2546
break;}case 321: _LL35E: {
# 2543 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2547
struct Cyc_List_List*_tmp754=Cyc_yyget_YY18(yyyyvsp[2]);
# 2548
struct Cyc_Absyn_Stmt*_tmp755=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
# 2549
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
# 2550
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp754,_tmp755));
# 2552
break;}case 322: _LL35F: {
# 2549 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2553
struct Cyc_List_List*_tmp756=Cyc_yyget_YY18(yyyyvsp[2]);
# 2554
struct Cyc_Absyn_Stmt*_tmp757=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
# 2555
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
# 2556
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp756,_tmp757));
# 2558
break;}case 323: _LL360: {
# 2559 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2561
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2563
{struct _dyneither_ptr*_tmpDCE;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDCE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2564
break;}case 324: _LL361: {
# 2560 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2564
yyval=Cyc_YY9(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
# 2565
break;}case 325: _LL362: {
# 2561 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2565
yyval=Cyc_YY9(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
# 2566
break;}case 326: _LL363: {
# 2562 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2566
yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
# 2567
break;}case 327: _LL364: {
# 2563 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2567
yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2568
break;}case 328: _LL365: {
# 2565 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2567
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2569
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
# 2570
break;}case 329: _LL366: {
# 2566 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2570
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
# 2571
break;}case 330: _LL367: {
# 2568 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2572
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2573
break;}case 331: _LL368: {
# 2577 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2581
yyval=yyyyvsp[0];
# 2582
break;}case 332: _LL369: {
# 2580 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2584
yyval=yyyyvsp[0];
# 2585
break;}case 333: _LL36A: {
# 2582 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2586
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
# 2587
break;}case 334: _LL36B: {
# 2585 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2589
yyval=yyyyvsp[0];
# 2590
break;}case 335: _LL36C: {
# 2587 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2591
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2592
break;}case 336: _LL36D: {
# 2590 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2594
yyval=yyyyvsp[0];
# 2595
break;}case 337: _LL36E: {
# 2592 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2596
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2597
break;}case 338: _LL36F: {
# 2595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599
yyval=yyyyvsp[0];
# 2600
break;}case 339: _LL370: {
# 2597 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2601
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2602
break;}case 340: _LL371: {
# 2600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604
yyval=yyyyvsp[0];
# 2605
break;}case 341: _LL372: {
# 2602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2607
break;}case 342: _LL373: {
# 2605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609
yyval=yyyyvsp[0];
# 2610
break;}case 343: _LL374: {
# 2607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2612
break;}case 344: _LL375: {
# 2610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614
yyval=yyyyvsp[0];
# 2615
break;}case 345: _LL376: {
# 2612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2617
break;}case 346: _LL377: {
# 2614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2618
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2619
break;}case 347: _LL378: {
# 2617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2621
yyval=yyyyvsp[0];
# 2622
break;}case 348: _LL379: {
# 2619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2623
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2624
break;}case 349: _LL37A: {
# 2621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2625
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2626
break;}case 350: _LL37B: {
# 2623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2627
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2628
break;}case 351: _LL37C: {
# 2625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2629
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2630
break;}case 352: _LL37D: {
# 2628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2632
yyval=yyyyvsp[0];
# 2633
break;}case 353: _LL37E: {
# 2630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2634
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2635
break;}case 354: _LL37F: {
# 2632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2636
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2637
break;}case 355: _LL380: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2639
yyval=yyyyvsp[0];
# 2640
break;}case 356: _LL381: {
# 2637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2641
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2642
break;}case 357: _LL382: {
# 2639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2643
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2644
break;}case 358: _LL383: {
# 2642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2646
yyval=yyyyvsp[0];
# 2647
break;}case 359: _LL384: {
# 2644 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2649
break;}case 360: _LL385: {
# 2646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2651
break;}case 361: _LL386: {
# 2648 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
# 2653
break;}case 362: _LL387: {
# 2651 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2655
yyval=yyyyvsp[0];
# 2656
break;}case 363: _LL388: {
# 2653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2657
void*_tmp759=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
# 2658
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp759,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2660
break;}case 364: _LL389: {
# 2659 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2663
yyval=yyyyvsp[0];
# 2664
break;}case 365: _LL38A: {
# 2662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2666
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
# 2667
break;}case 366: _LL38B: {
# 2664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2668
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
# 2669
break;}case 367: _LL38C: {
# 2666 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2670
void*_tmp75A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 2671
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp75A,(unsigned int)(yyyylsp[0]).first_line)));
# 2673
break;}case 368: _LL38D: {
# 2670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2674
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
# 2675
break;}case 369: _LL38E: {
# 2672 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2676
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpDD4;struct _dyneither_ptr*_tmpDD3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpDD2;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,(void*)(
# 2677
(_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD4.tag=0,((_tmpDD4.f1=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpDD3)))),_tmpDD4)))),_tmpDD2)))),(unsigned int)(yyyylsp[0]).first_line)));}
# 2678
break;}case 370: _LL38F: {
# 2676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2680
void*_tmp75E=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 2681
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpDD7;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpDD6;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp(_tmp75E,(void*)((_tmpDD6=_cycalloc_atomic(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD7.tag=1,((_tmpDD7.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpDD7)))),_tmpDD6)))),(unsigned int)(yyyylsp[0]).first_line)));}
# 2683
break;}case 371: _LL390: {
# 2683 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2687
yyval=yyyyvsp[0];
# 2688
break;}case 372: _LL391: {
# 2692 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2696
yyval=yyyyvsp[0];
# 2697
break;}case 373: _LL392: {
# 2697 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2701
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
# 2702
break;}case 374: _LL393: {
# 2699 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2703
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
# 2704
break;}case 375: _LL394: {
# 2701 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2705
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyyyvsp[0]);
# 2706
{void*_tmp761=e->r;union Cyc_Absyn_Cnst _tmp763;struct _tuple3 _tmp764;enum Cyc_Absyn_Sign _tmp765;char _tmp766;union Cyc_Absyn_Cnst _tmp768;struct _tuple4 _tmp769;enum Cyc_Absyn_Sign _tmp76A;short _tmp76B;union Cyc_Absyn_Cnst _tmp76D;struct _tuple5 _tmp76E;enum Cyc_Absyn_Sign _tmp76F;int _tmp770;union Cyc_Absyn_Cnst _tmp772;struct _tuple7 _tmp773;struct _dyneither_ptr _tmp774;int _tmp775;union Cyc_Absyn_Cnst _tmp777;int _tmp778;union Cyc_Absyn_Cnst _tmp77A;struct _dyneither_ptr _tmp77B;union Cyc_Absyn_Cnst _tmp77D;struct _dyneither_ptr _tmp77E;union Cyc_Absyn_Cnst _tmp780;struct _tuple6 _tmp781;_LL397: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp762=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp762->tag != 0)goto _LL399;else{_tmp763=_tmp762->f1;if((_tmp763.Char_c).tag != 2)goto _LL399;_tmp764=(struct _tuple3)(_tmp763.Char_c).val;_tmp765=_tmp764.f1;_tmp766=_tmp764.f2;}}_LL398:
# 2709
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDDA;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDD9;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDD9=_cycalloc_atomic(sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDDA.tag=10,((_tmpDDA.f1=_tmp766,_tmpDDA)))),_tmpDD9)))),e->loc));}goto _LL396;_LL399: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp767=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp767->tag != 0)goto _LL39B;else{_tmp768=_tmp767->f1;if((_tmp768.Short_c).tag != 4)goto _LL39B;_tmp769=(struct _tuple4)(_tmp768.Short_c).val;_tmp76A=_tmp769.f1;_tmp76B=_tmp769.f2;}}_LL39A:
# 2711
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDDD;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDDC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDDC=_cycalloc_atomic(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDD.tag=9,((_tmpDDD.f1=_tmp76A,((_tmpDDD.f2=(int)_tmp76B,_tmpDDD)))))),_tmpDDC)))),e->loc));}goto _LL396;_LL39B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp76C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp76C->tag != 0)goto _LL39D;else{_tmp76D=_tmp76C->f1;if((_tmp76D.Int_c).tag != 5)goto _LL39D;_tmp76E=(struct _tuple5)(_tmp76D.Int_c).val;_tmp76F=_tmp76E.f1;_tmp770=_tmp76E.f2;}}_LL39C:
# 2713
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDE0;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDDF;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDDF=_cycalloc_atomic(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE0.tag=9,((_tmpDE0.f1=_tmp76F,((_tmpDE0.f2=_tmp770,_tmpDE0)))))),_tmpDDF)))),e->loc));}goto _LL396;_LL39D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp771=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp771->tag != 0)goto _LL39F;else{_tmp772=_tmp771->f1;if((_tmp772.Float_c).tag != 7)goto _LL39F;_tmp773=(struct _tuple7)(_tmp772.Float_c).val;_tmp774=_tmp773.f1;_tmp775=_tmp773.f2;}}_LL39E:
# 2715
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDE3;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDE2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=11,((_tmpDE3.f1=_tmp774,((_tmpDE3.f2=_tmp775,_tmpDE3)))))),_tmpDE2)))),e->loc));}goto _LL396;_LL39F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp776=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp776->tag != 0)goto _LL3A1;else{_tmp777=_tmp776->f1;if((_tmp777.Null_c).tag != 1)goto _LL3A1;_tmp778=(int)(_tmp777.Null_c).val;}}_LL3A0:
# 2717
 yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL396;_LL3A1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp779=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp779->tag != 0)goto _LL3A3;else{_tmp77A=_tmp779->f1;if((_tmp77A.String_c).tag != 8)goto _LL3A3;_tmp77B=(struct _dyneither_ptr)(_tmp77A.String_c).val;}}_LL3A2:
# 2719
{const char*_tmpDE4;Cyc_Parse_err(((_tmpDE4="strings cannot occur within patterns",_tag_dyneither(_tmpDE4,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp77C->tag != 0)goto _LL3A5;else{_tmp77D=_tmp77C->f1;if((_tmp77D.Wstring_c).tag != 9)goto _LL3A5;_tmp77E=(struct _dyneither_ptr)(_tmp77D.Wstring_c).val;}}_LL3A4:
# 2721
{const char*_tmpDE5;Cyc_Parse_err(((_tmpDE5="strings cannot occur within patterns",_tag_dyneither(_tmpDE5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp761;if(_tmp77F->tag != 0)goto _LL3A7;else{_tmp780=_tmp77F->f1;if((_tmp780.LongLong_c).tag != 6)goto _LL3A7;_tmp781=(struct _tuple6)(_tmp780.LongLong_c).val;}}_LL3A6:
# 2723
{const char*_tmpDE6;Cyc_Parse_unimp(((_tmpDE6="long long's in patterns",_tag_dyneither(_tmpDE6,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A7:;_LL3A8: {
# 2725
const char*_tmpDE7;Cyc_Parse_err(((_tmpDE7="bad constant in case",_tag_dyneither(_tmpDE7,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL396:;}
# 2728
break;}case 376: _LL395: {
# 2725 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2729
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDEA;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDE9;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEA.tag=14,((_tmpDEA.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDEA)))),_tmpDE9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2730
break;}case 377: _LL3A9: {
# 2727 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2731
{const char*_tmpDEB;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDEB="as",_tag_dyneither(_tmpDEB,sizeof(char),3))))!= 0){
# 2732
const char*_tmpDEC;Cyc_Parse_err(((_tmpDEC="expecting `as'",_tag_dyneither(_tmpDEC,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
# 2733
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDF6;struct _dyneither_ptr*_tmpDF5;struct _tuple0*_tmpDF4;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDF3;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF6.tag=1,((_tmpDF6.f1=Cyc_Absyn_new_vardecl(((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->f1=Cyc_Absyn_Loc_n,((_tmpDF4->f2=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF5)))),_tmpDF4)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDF6.f2=
# 2734
Cyc_yyget_YY11(yyyyvsp[2]),_tmpDF6)))))),_tmpDF3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2736
break;}case 378: _LL3AA: {
# 2733 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2737
struct Cyc_List_List*_tmp797;int _tmp798;struct _tuple22 _tmp796=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp797=_tmp796.f1;_tmp798=_tmp796.f2;
# 2738
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDF9;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDF8;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF9.tag=4,((_tmpDF9.f1=_tmp797,((_tmpDF9.f2=_tmp798,_tmpDF9)))))),_tmpDF8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2740
break;}case 379: _LL3AB: {
# 2737 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2741
struct Cyc_List_List*_tmp79C;int _tmp79D;struct _tuple22 _tmp79B=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp79C=_tmp79B.f1;_tmp79D=_tmp79B.f2;
# 2742
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDFC;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDFB;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFC.tag=15,((_tmpDFC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDFC.f2=_tmp79C,((_tmpDFC.f3=_tmp79D,_tmpDFC)))))))),_tmpDFB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2744
break;}case 380: _LL3AC: {
# 2741 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2745
struct Cyc_List_List*_tmp7A1;int _tmp7A2;struct _tuple22 _tmp7A0=*Cyc_yyget_YY16(yyyyvsp[3]);_tmp7A1=_tmp7A0.f1;_tmp7A2=_tmp7A0.f2;{
# 2746
struct Cyc_List_List*_tmp7A3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
# 2747
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE02;struct Cyc_Absyn_AggrInfo*_tmpE01;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE00;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00[0]=((_tmpE02.tag=6,((_tmpE02.f1=((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE01->targs=0,_tmpE01)))))),((_tmpE02.f2=_tmp7A3,((_tmpE02.f3=_tmp7A1,((_tmpE02.f4=_tmp7A2,_tmpE02)))))))))),_tmpE00)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2750
break;};}case 381: _LL3AD: {
# 2747 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751
struct Cyc_List_List*_tmp7A8;int _tmp7A9;struct _tuple22 _tmp7A7=*Cyc_yyget_YY16(yyyyvsp[2]);_tmp7A8=_tmp7A7.f1;_tmp7A9=_tmp7A7.f2;{
# 2752
struct Cyc_List_List*_tmp7AA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
# 2753
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE05;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE04;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE05.tag=6,((_tmpE05.f1=0,((_tmpE05.f2=_tmp7AA,((_tmpE05.f3=_tmp7A8,((_tmpE05.f4=_tmp7A9,_tmpE05)))))))))),_tmpE04)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2755
break;};}case 382: _LL3AE: {
# 2752 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2756
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE08;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE07;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE08.tag=5,((_tmpE08.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE08)))),_tmpE07)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2757
break;}case 383: _LL3AF: {
# 2754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2758
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE12;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE11;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE10;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE0F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE12.tag=5,((_tmpE12.f1=Cyc_Absyn_new_pat((void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=5,((_tmpE11.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE11)))),_tmpE10)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE12)))),_tmpE0F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2759
break;}case 384: _LL3B0: {
# 2756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2760
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE1C;struct _dyneither_ptr*_tmpE1B;struct _tuple0*_tmpE1A;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE19;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1C.tag=2,((_tmpE1C.f1=Cyc_Absyn_new_vardecl(((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->f1=Cyc_Absyn_Loc_n,((_tmpE1A->f2=((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE1B)))),_tmpE1A)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE1C.f2=
# 2762
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE1C)))))),_tmpE19)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2764
break;}case 385: _LL3B1: {
# 2761 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2765
{const char*_tmpE1D;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE1D="as",_tag_dyneither(_tmpE1D,sizeof(char),3))))!= 0){
# 2766
const char*_tmpE1E;Cyc_Parse_err(((_tmpE1E="expecting `as'",_tag_dyneither(_tmpE1E,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
# 2767
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE28;struct _dyneither_ptr*_tmpE27;struct _tuple0*_tmpE26;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE25;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE28.tag=2,((_tmpE28.f1=Cyc_Absyn_new_vardecl(((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->f1=Cyc_Absyn_Loc_n,((_tmpE26->f2=((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE27)))),_tmpE26)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE28.f2=
# 2769
Cyc_yyget_YY11(yyyyvsp[3]),_tmpE28)))))),_tmpE25)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2771
break;}case 386: _LL3B2: {
# 2768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2772
void*_tmp7BD=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
# 2773
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE39;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE38;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE37;struct _dyneither_ptr*_tmpE36;struct _tuple0*_tmpE35;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE34;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE39.tag=3,((_tmpE39.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7BD),((_tmpE39.f2=
# 2774
Cyc_Absyn_new_vardecl(((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->f1=Cyc_Absyn_Loc_n,((_tmpE35->f2=((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE36)))),_tmpE35)))))),(void*)(
# 2775
(_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE38.tag=19,((_tmpE38.f1=(void*)_tmp7BD,_tmpE38)))),_tmpE37)))),0),_tmpE39)))))),_tmpE34)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2777
break;}case 387: _LL3B3: {
# 2774 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2778
struct Cyc_Absyn_Tvar*_tmp7C4=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
# 2779
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE55;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE54;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE53;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE52;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE51;struct _dyneither_ptr*_tmpE50;struct _tuple0*_tmpE4F;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE4E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE55.tag=3,((_tmpE55.f1=_tmp7C4,((_tmpE55.f2=
# 2780
Cyc_Absyn_new_vardecl(((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->f1=Cyc_Absyn_Loc_n,((_tmpE4F->f2=((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE50)))),_tmpE4F)))))),(void*)(
# 2781
(_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE54.tag=19,((_tmpE54.f1=(void*)((void*)((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=2,((_tmpE53.f1=_tmp7C4,_tmpE53)))),_tmpE52))))),_tmpE54)))),_tmpE51)))),0),_tmpE55)))))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2783
break;}case 388: _LL3B4: {
# 2782 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2786
{struct _tuple22*_tmpE56;yyval=Cyc_YY12(((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE56->f2=0,_tmpE56)))))));}
# 2787
break;}case 389: _LL3B5: {
# 2783 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2787
{struct _tuple22*_tmpE57;yyval=Cyc_YY12(((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE57->f2=1,_tmpE57)))))));}
# 2788
break;}case 390: _LL3B6: {
# 2784 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2788
{struct _tuple22*_tmpE58;yyval=Cyc_YY12(((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58->f1=0,((_tmpE58->f2=1,_tmpE58)))))));}
# 2789
break;}case 391: _LL3B7: {
# 2789 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2793
{struct Cyc_List_List*_tmpE59;yyval=Cyc_YY13(((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59->hd=Cyc_yyget_YY11(yyyyvsp[0]),((_tmpE59->tl=0,_tmpE59)))))));}
# 2794
break;}case 392: _LL3B8: {
# 2791 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2795
{struct Cyc_List_List*_tmpE5A;yyval=Cyc_YY13(((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A->hd=Cyc_yyget_YY11(yyyyvsp[2]),((_tmpE5A->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpE5A)))))));}
# 2796
break;}case 393: _LL3B9: {
# 2796 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2800
{struct _tuple23*_tmpE5B;yyval=Cyc_YY14(((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B->f1=0,((_tmpE5B->f2=Cyc_yyget_YY11(yyyyvsp[0]),_tmpE5B)))))));}
# 2801
break;}case 394: _LL3BA: {
# 2798 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2802
{struct _tuple23*_tmpE5C;yyval=Cyc_YY14(((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpE5C->f2=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE5C)))))));}
# 2803
break;}case 395: _LL3BB: {
# 2801 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2805
{struct _tuple22*_tmpE5D;yyval=Cyc_YY16(((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE5D->f2=0,_tmpE5D)))))));}
# 2806
break;}case 396: _LL3BC: {
# 2802 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2806
{struct _tuple22*_tmpE5E;yyval=Cyc_YY16(((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE5E->f2=1,_tmpE5E)))))));}
# 2807
break;}case 397: _LL3BD: {
# 2803 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2807
{struct _tuple22*_tmpE5F;yyval=Cyc_YY16(((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F->f1=0,((_tmpE5F->f2=1,_tmpE5F)))))));}
# 2808
break;}case 398: _LL3BE: {
# 2808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2812
{struct Cyc_List_List*_tmpE60;yyval=Cyc_YY15(((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60->hd=Cyc_yyget_YY14(yyyyvsp[0]),((_tmpE60->tl=0,_tmpE60)))))));}
# 2813
break;}case 399: _LL3BF: {
# 2810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2814
{struct Cyc_List_List*_tmpE61;yyval=Cyc_YY15(((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61->hd=Cyc_yyget_YY14(yyyyvsp[2]),((_tmpE61->tl=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE61)))))));}
# 2815
break;}case 400: _LL3C0: {
# 2816 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820
yyval=yyyyvsp[0];
# 2821
break;}case 401: _LL3C1: {
# 2818 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2822
yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2823
break;}case 402: _LL3C2: {
# 2823 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2827
yyval=yyyyvsp[0];
# 2828
break;}case 403: _LL3C3: {
# 2825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2829
yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY8(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2830
break;}case 404: _LL3C4: {
# 2827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2831
yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2832
break;}case 405: _LL3C5: {
# 2831 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2835
yyval=Cyc_YY8(0);
# 2836
break;}case 406: _LL3C6: {
# 2832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2836
{struct Cyc_Core_Opt*_tmpE62;yyval=Cyc_YY8(((_tmpE62=_cycalloc_atomic(sizeof(*_tmpE62)),((_tmpE62->v=(void*)Cyc_Absyn_Times,_tmpE62)))));}
# 2837
break;}case 407: _LL3C7: {
# 2833 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2837
{struct Cyc_Core_Opt*_tmpE63;yyval=Cyc_YY8(((_tmpE63=_cycalloc_atomic(sizeof(*_tmpE63)),((_tmpE63->v=(void*)Cyc_Absyn_Div,_tmpE63)))));}
# 2838
break;}case 408: _LL3C8: {
# 2834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2838
{struct Cyc_Core_Opt*_tmpE64;yyval=Cyc_YY8(((_tmpE64=_cycalloc_atomic(sizeof(*_tmpE64)),((_tmpE64->v=(void*)Cyc_Absyn_Mod,_tmpE64)))));}
# 2839
break;}case 409: _LL3C9: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2839
{struct Cyc_Core_Opt*_tmpE65;yyval=Cyc_YY8(((_tmpE65=_cycalloc_atomic(sizeof(*_tmpE65)),((_tmpE65->v=(void*)Cyc_Absyn_Plus,_tmpE65)))));}
# 2840
break;}case 410: _LL3CA: {
# 2836 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2840
{struct Cyc_Core_Opt*_tmpE66;yyval=Cyc_YY8(((_tmpE66=_cycalloc_atomic(sizeof(*_tmpE66)),((_tmpE66->v=(void*)Cyc_Absyn_Minus,_tmpE66)))));}
# 2841
break;}case 411: _LL3CB: {
# 2837 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2841
{struct Cyc_Core_Opt*_tmpE67;yyval=Cyc_YY8(((_tmpE67=_cycalloc_atomic(sizeof(*_tmpE67)),((_tmpE67->v=(void*)Cyc_Absyn_Bitlshift,_tmpE67)))));}
# 2842
break;}case 412: _LL3CC: {
# 2838 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2842
{struct Cyc_Core_Opt*_tmpE68;yyval=Cyc_YY8(((_tmpE68=_cycalloc_atomic(sizeof(*_tmpE68)),((_tmpE68->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE68)))));}
# 2843
break;}case 413: _LL3CD: {
# 2839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2843
{struct Cyc_Core_Opt*_tmpE69;yyval=Cyc_YY8(((_tmpE69=_cycalloc_atomic(sizeof(*_tmpE69)),((_tmpE69->v=(void*)Cyc_Absyn_Bitand,_tmpE69)))));}
# 2844
break;}case 414: _LL3CE: {
# 2840 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2844
{struct Cyc_Core_Opt*_tmpE6A;yyval=Cyc_YY8(((_tmpE6A=_cycalloc_atomic(sizeof(*_tmpE6A)),((_tmpE6A->v=(void*)Cyc_Absyn_Bitxor,_tmpE6A)))));}
# 2845
break;}case 415: _LL3CF: {
# 2841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2845
{struct Cyc_Core_Opt*_tmpE6B;yyval=Cyc_YY8(((_tmpE6B=_cycalloc_atomic(sizeof(*_tmpE6B)),((_tmpE6B->v=(void*)Cyc_Absyn_Bitor,_tmpE6B)))));}
# 2846
break;}case 416: _LL3D0: {
# 2846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850
yyval=yyyyvsp[0];
# 2851
break;}case 417: _LL3D1: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2852
yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2853
break;}case 418: _LL3D2: {
# 2851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2855
yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2856
break;}case 419: _LL3D3: {
# 2854 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2858
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2859
break;}case 420: _LL3D4: {
# 2856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2860
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2861
break;}case 421: _LL3D5: {
# 2858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2862
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2863
break;}case 422: _LL3D6: {
# 2860 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2864
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
# 2865
break;}case 423: _LL3D7: {
# 2864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2868
yyval=yyyyvsp[0];
# 2869
break;}case 424: _LL3D8: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2872
yyval=yyyyvsp[0];
# 2873
break;}case 425: _LL3D9: {
# 2870 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2874
yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2875
break;}case 426: _LL3DA: {
# 2874 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2878
yyval=yyyyvsp[0];
# 2879
break;}case 427: _LL3DB: {
# 2876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2880
yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2881
break;}case 428: _LL3DC: {
# 2880 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2884
yyval=yyyyvsp[0];
# 2885
break;}case 429: _LL3DD: {
# 2882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2886
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2887
break;}case 430: _LL3DE: {
# 2886 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2890
yyval=yyyyvsp[0];
# 2891
break;}case 431: _LL3DF: {
# 2888 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2892
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2893
break;}case 432: _LL3E0: {
# 2892 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2896
yyval=yyyyvsp[0];
# 2897
break;}case 433: _LL3E1: {
# 2894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2898
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2899
break;}case 434: _LL3E2: {
# 2898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2902
yyval=yyyyvsp[0];
# 2903
break;}case 435: _LL3E3: {
# 2900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2904
yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2905
break;}case 436: _LL3E4: {
# 2902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906
yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2907
break;}case 437: _LL3E5: {
# 2906 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910
yyval=yyyyvsp[0];
# 2911
break;}case 438: _LL3E6: {
# 2908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2912
yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2913
break;}case 439: _LL3E7: {
# 2910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2914
yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2915
break;}case 440: _LL3E8: {
# 2912 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2916
yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2917
break;}case 441: _LL3E9: {
# 2914 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2918
yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2919
break;}case 442: _LL3EA: {
# 2918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2922
yyval=yyyyvsp[0];
# 2923
break;}case 443: _LL3EB: {
# 2920 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2924
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2925
break;}case 444: _LL3EC: {
# 2922 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2927
break;}case 445: _LL3ED: {
# 2926 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930
yyval=yyyyvsp[0];
# 2931
break;}case 446: _LL3EE: {
# 2928 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2932
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2933
break;}case 447: _LL3EF: {
# 2930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2934
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2935
break;}case 448: _LL3F0: {
# 2934 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938
yyval=yyyyvsp[0];
# 2939
break;}case 449: _LL3F1: {
# 2936 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2940
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2941
break;}case 450: _LL3F2: {
# 2938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2942
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2943
break;}case 451: _LL3F3: {
# 2940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2944
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 2945
break;}case 452: _LL3F4: {
# 2944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2948
yyval=yyyyvsp[0];
# 2949
break;}case 453: _LL3F5: {
# 2946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2950
void*_tmp7E3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
# 2951
yyval=Cyc_YY3(Cyc_Absyn_cast_exp(_tmp7E3,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2953
break;}case 454: _LL3F6: {
# 2952 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956
yyval=yyyyvsp[0];
# 2957
break;}case 455: _LL3F7: {
# 2953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2957
yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2958
break;}case 456: _LL3F8: {
# 2954 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2958
yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2959
break;}case 457: _LL3F9: {
# 2955 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2959
yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2960
break;}case 458: _LL3FA: {
# 2956 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2960
yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2961
break;}case 459: _LL3FB: {
# 2957 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2961
yyval=yyyyvsp[1];
# 2962
break;}case 460: _LL3FC: {
# 2958 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2962
yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2963
break;}case 461: _LL3FD: {
# 2960 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2964
void*_tmp7E4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 2965
yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp(_tmp7E4,(unsigned int)(yyyylsp[0]).first_line));
# 2967
break;}case 462: _LL3FE: {
# 2963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2967
yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
# 2968
break;}case 463: _LL3FF: {
# 2965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2969
void*_tmp7E5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 2970
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE71;struct _dyneither_ptr*_tmpE70;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE6F;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7E5,(void*)((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F[0]=((_tmpE71.tag=0,((_tmpE71.f1=((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE70)))),_tmpE71)))),_tmpE6F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2972
break;}case 464: _LL400: {
# 2970 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2974
void*_tmp7E9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 2975
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE74;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE73;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7E9,(void*)((_tmpE73=_cycalloc_atomic(sizeof(*_tmpE73)),((_tmpE73[0]=((_tmpE74.tag=1,((_tmpE74.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpE74)))),_tmpE73)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2977
break;}case 465: _LL401: {
# 2975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2979
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE7A;struct Cyc_Absyn_MallocInfo _tmpE79;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE78;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE7A.tag=32,((_tmpE7A.f1=((_tmpE79.is_calloc=0,((_tmpE79.rgn=0,((_tmpE79.elt_type=0,((_tmpE79.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE79.fat_result=0,_tmpE79)))))))))),_tmpE7A)))),_tmpE78)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2981
break;}case 466: _LL402: {
# 2978 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2982
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE80;struct Cyc_Absyn_MallocInfo _tmpE7F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE7E;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E[0]=((_tmpE80.tag=32,((_tmpE80.f1=((_tmpE7F.is_calloc=0,((_tmpE7F.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE7F.elt_type=0,((_tmpE7F.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE7F.fat_result=0,_tmpE7F)))))))))),_tmpE80)))),_tmpE7E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2984
break;}case 467: _LL403: {
# 2981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2985
void*_tmp7F2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
# 2986
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE8A;void**_tmpE89;struct Cyc_Absyn_MallocInfo _tmpE88;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE87;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE8A.tag=32,((_tmpE8A.f1=((_tmpE88.is_calloc=1,((_tmpE88.rgn=0,((_tmpE88.elt_type=((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=_tmp7F2,_tmpE89)))),((_tmpE88.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE88.fat_result=0,_tmpE88)))))))))),_tmpE8A)))),_tmpE87)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2988
break;}case 468: _LL404: {
# 2986 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2990
void*_tmp7F7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
# 2991
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE94;void**_tmpE93;struct Cyc_Absyn_MallocInfo _tmpE92;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE91;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE94.tag=32,((_tmpE94.f1=((_tmpE92.is_calloc=1,((_tmpE92.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE92.elt_type=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93[0]=_tmp7F7,_tmpE93)))),((_tmpE92.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE92.fat_result=0,_tmpE92)))))))))),_tmpE94)))),_tmpE91)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2993
break;}case 469: _LL405: {
# 2990 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2994
{struct Cyc_Absyn_Exp*_tmpE95[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE95[0]=Cyc_yyget_YY3(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE95,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2995
break;}case 470: _LL406: {
# 2992 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2996
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE9B;struct _dyneither_ptr*_tmpE9A;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE99;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99[0]=((_tmpE9B.tag=36,((_tmpE9B.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE9B.f2=((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE9A)))),_tmpE9B)))))),_tmpE99)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2997
break;}case 471: _LL407: {
# 2994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2998
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEA1;struct _dyneither_ptr*_tmpEA0;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE9F;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F[0]=((_tmpEA1.tag=36,((_tmpEA1.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEA1.f2=((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEA0)))),_tmpEA1)))))),_tmpE9F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2999
break;}case 472: _LL408: {
# 2996 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3000
void*_tmp803=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
# 3001
yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp803,(unsigned int)(yyyylsp[0]).first_line));
# 3002
break;}case 473: _LL409: {
# 2999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3003
int _tmp805;struct _dyneither_ptr _tmp806;struct _tuple20 _tmp804=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp805=_tmp804.f1;_tmp806=_tmp804.f2;
# 3004
yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp805,_tmp806,(unsigned int)(yyyylsp[0]).first_line));
# 3005
break;}case 474: _LL40A: {
# 3004 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3008
yyval=Cyc_YY7(Cyc_Absyn_Bitnot);
# 3009
break;}case 475: _LL40B: {
# 3005 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3009
yyval=Cyc_YY7(Cyc_Absyn_Not);
# 3010
break;}case 476: _LL40C: {
# 3006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3010
yyval=Cyc_YY7(Cyc_Absyn_Minus);
# 3011
break;}case 477: _LL40D: {
# 3011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3015
yyval=yyyyvsp[0];
# 3016
break;}case 478: _LL40E: {
# 3013 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3017
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 3018
break;}case 479: _LL40F: {
# 3015 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3019
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
# 3020
break;}case 480: _LL410: {
# 3017 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3021
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 3022
break;}case 481: _LL411: {
# 3019 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3023
{struct _dyneither_ptr*_tmpEA2;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEA2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3024
break;}case 482: _LL412: {
# 3021 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3025
{struct _dyneither_ptr*_tmpEA3;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEA3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3026
break;}case 483: _LL413: {
# 3023 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3027
yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3028
break;}case 484: _LL414: {
# 3025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3029
yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3030
break;}case 485: _LL415: {
# 3027 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3031
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEA6;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEA5;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=((_tmpEA6.tag=24,((_tmpEA6.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEA6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpEA6)))))),_tmpEA5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3032
break;}case 486: _LL416: {
# 3029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3033
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEA9;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEA8;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=((_tmpEA9.tag=24,((_tmpEA9.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEA9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpEA9)))))),_tmpEA8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3034
break;}case 487: _LL417: {
# 3035 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3039
yyval=Cyc_YY3(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3040
break;}case 488: _LL418: {
# 3037 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3041
yyval=yyyyvsp[0];
# 3042
break;}case 489: _LL419: {
# 3039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3043
yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3044
break;}case 490: _LL41A: {
# 3041 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3045
yyval=Cyc_YY3(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3046
break;}case 491: _LL41B: {
# 3043 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3047
yyval=yyyyvsp[1];
# 3048
break;}case 492: _LL41C: {
# 3048 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3052
yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3053
break;}case 493: _LL41D: {
# 3050 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3054
yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
# 3055
break;}case 494: _LL41E: {
# 3053 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3057
yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 3058
break;}case 495: _LL41F: {
# 3056 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3060
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEAC;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEAB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=((_tmpEAC.tag=27,((_tmpEAC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEAC.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpEAC.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[3])),((_tmpEAC.f4=0,_tmpEAC)))))))))),_tmpEAB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3061
break;}case 496: _LL420: {
# 3059 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3063
yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
# 3064
break;}case 497: _LL421: {
# 3063 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3067
yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[0])));
# 3068
break;}case 498: _LL422: {
# 3069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3073
{struct Cyc_List_List*_tmpEAD;yyval=Cyc_YY5(((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD->hd=Cyc_yyget_YY3(yyyyvsp[0]),((_tmpEAD->tl=0,_tmpEAD)))))));}
# 3074
break;}case 499: _LL423: {
# 3071 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3075
{struct Cyc_List_List*_tmpEAE;yyval=Cyc_YY5(((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE->hd=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpEAE->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpEAE)))))));}
# 3076
break;}case 500: _LL424: {
# 3077 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3081
struct _tuple5 _tmp811=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY3(Cyc_Absyn_int_exp(_tmp811.f1,_tmp811.f2,(unsigned int)(yyyylsp[0]).first_line));
# 3082
break;}case 501: _LL425: {
# 3078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3082
yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3083
break;}case 502: _LL426: {
# 3079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3083
yyval=Cyc_YY3(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
# 3084
break;}case 503: _LL427: {
# 3080 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3085
struct _dyneither_ptr _tmp812=Cyc_yyget_String_tok(yyyyvsp[0]);
# 3086
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp812);
# 3087
int i=1;
# 3088
if(l > 0){
# 3089
char c=*((const char*)_check_dyneither_subscript(_tmp812,sizeof(char),l - 1));
# 3090
if(c == 'f'  || c == 'F')i=0;else{
# 3091
if(c == 'l'  || c == 'L')i=2;}}
# 3093
yyval=Cyc_YY3(Cyc_Absyn_float_exp(_tmp812,i,(unsigned int)(yyyylsp[0]).first_line));
# 3095
break;}case 504: _LL428: {
# 3092 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3096
yyval=Cyc_YY3(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
# 3097
break;}case 505: _LL429: {
# 3096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3100
{struct _dyneither_ptr*_tmpEB1;struct _tuple0*_tmpEB0;yyval=Cyc_QualId_tok(((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0->f1=Cyc_Absyn_Rel_n(0),((_tmpEB0->f2=((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEB1)))),_tmpEB0)))))));}
# 3101
break;}case 506: _LL42A: {
# 3097 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101
yyval=yyyyvsp[0];
# 3102
break;}case 507: _LL42B: {
# 3100 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3104
{struct _dyneither_ptr*_tmpEB4;struct _tuple0*_tmpEB3;yyval=Cyc_QualId_tok(((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3->f1=Cyc_Absyn_Rel_n(0),((_tmpEB3->f2=((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEB4)))),_tmpEB3)))))));}
# 3105
break;}case 508: _LL42C: {
# 3101 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3105
yyval=yyyyvsp[0];
# 3106
break;}case 509: _LL42D: {
# 3106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3110
yyval=yyyyvsp[0];
# 3111
break;}case 510: _LL42E: {
# 3107 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3111
yyval=yyyyvsp[0];
# 3112
break;}case 511: _LL42F: {
# 3110 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3114
yyval=yyyyvsp[0];
# 3115
break;}case 512: _LL430: {
# 3111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3115
yyval=yyyyvsp[0];
# 3116
break;}case 513: _LL431: {
# 3115 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120
break;}case 514: _LL432: {
# 3116 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120
yylex_buf->lex_curr_pos -=1;
# 3121
break;}default: _LL433:
# 3125
 break;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
# 372
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
# 392
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
# 395
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
# 396
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
# 397
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
# 402
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(515,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(143,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
# 414
if((yystate >= 0  && yystate <= 6375) && Cyc_yycheck[_check_known_subscript_notnull(6376,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
# 415
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6376,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(143,yyn - 146)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
# 431
if(yyn > - 32768  && yyn < 6375){
# 433
int sze=0;
# 434
struct _dyneither_ptr msg;
# 435
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6376,x + yyn)]== x)
# 442
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)])+ 15,count ++);}
# 443
{unsigned int _tmpEB9;unsigned int _tmpEB8;struct _dyneither_ptr _tmpEB7;char*_tmpEB6;unsigned int _tmpEB5;msg=((_tmpEB5=(unsigned int)(sze + 15),((_tmpEB6=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEB5 + 1)),((_tmpEB7=_tag_dyneither(_tmpEB6,sizeof(char),_tmpEB5 + 1),((((_tmpEB8=_tmpEB5,((_tmpEBA(& _tmpEB9,& _tmpEB8,& _tmpEB6),_tmpEB6[_tmpEB8]=(char)0)))),_tmpEB7))))))));}
# 444
{const char*_tmpEBB;Cyc_strcpy(msg,((_tmpEBB="parse error",_tag_dyneither(_tmpEBB,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
# 449
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6376,x + yyn)]== x){
# 453
{const char*_tmpEBD;const char*_tmpEBC;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
# 454
(_tmpEBD=", expecting `",_tag_dyneither(_tmpEBD,sizeof(char),14))):(struct _dyneither_ptr)(
# 455
(_tmpEBC=" or `",_tag_dyneither(_tmpEBC,sizeof(char),6)))));}
# 456
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)]);
# 457
{const char*_tmpEBE;Cyc_strcat(msg,((_tmpEBE="'",_tag_dyneither(_tmpEBE,sizeof(char),2))));}
# 458
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEBF;Cyc_yyerror(((_tmpEBF="parse error",_tag_dyneither(_tmpEBF,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
# 477
int _tmp820=1;_npop_handler(0);return _tmp820;}
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
 if(yyssp_offset == 0){int _tmp821=1;_npop_handler(0);return _tmp821;}
# 508
-- yyvsp_offset;
# 509
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
# 527
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
# 528
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
# 531
if((yyn < 0  || yyn > 6375) || Cyc_yycheck[_check_known_subscript_notnull(6376,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6376,yyn)];
# 534
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
# 537
yyn=- yyn;
# 538
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1029){
# 543
int _tmp822=0;_npop_handler(0);return _tmp822;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3119 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
# 3120
union Cyc_YYSTYPE _tmp82C=v;struct _tuple5 _tmp82D;int _tmp82E;char _tmp82F;struct _dyneither_ptr _tmp830;struct _tuple0*_tmp831;struct _tuple0 _tmp832;union Cyc_Absyn_Nmspace _tmp833;struct _dyneither_ptr*_tmp834;_LL436: if((_tmp82C.Int_tok).tag != 1)goto _LL438;_tmp82D=(struct _tuple5)(_tmp82C.Int_tok).val;_tmp82E=_tmp82D.f2;_LL437:
# 3121
{const char*_tmpEC3;void*_tmpEC2[1];struct Cyc_Int_pa_PrintArg_struct _tmpEC1;(_tmpEC1.tag=1,((_tmpEC1.f1=(unsigned long)_tmp82E,((_tmpEC2[0]=& _tmpEC1,Cyc_fprintf(Cyc_stderr,((_tmpEC3="%d",_tag_dyneither(_tmpEC3,sizeof(char),3))),_tag_dyneither(_tmpEC2,sizeof(void*),1)))))));}goto _LL435;_LL438: if((_tmp82C.Char_tok).tag != 2)goto _LL43A;_tmp82F=(char)(_tmp82C.Char_tok).val;_LL439:
# 3122
{const char*_tmpEC7;void*_tmpEC6[1];struct Cyc_Int_pa_PrintArg_struct _tmpEC5;(_tmpEC5.tag=1,((_tmpEC5.f1=(unsigned long)((int)_tmp82F),((_tmpEC6[0]=& _tmpEC5,Cyc_fprintf(Cyc_stderr,((_tmpEC7="%c",_tag_dyneither(_tmpEC7,sizeof(char),3))),_tag_dyneither(_tmpEC6,sizeof(void*),1)))))));}goto _LL435;_LL43A: if((_tmp82C.String_tok).tag != 3)goto _LL43C;_tmp830=(struct _dyneither_ptr)(_tmp82C.String_tok).val;_LL43B:
# 3123
{const char*_tmpECB;void*_tmpECA[1];struct Cyc_String_pa_PrintArg_struct _tmpEC9;(_tmpEC9.tag=0,((_tmpEC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp830),((_tmpECA[0]=& _tmpEC9,Cyc_fprintf(Cyc_stderr,((_tmpECB="\"%s\"",_tag_dyneither(_tmpECB,sizeof(char),5))),_tag_dyneither(_tmpECA,sizeof(void*),1)))))));}goto _LL435;_LL43C: if((_tmp82C.QualId_tok).tag != 5)goto _LL43E;_tmp831=(struct _tuple0*)(_tmp82C.QualId_tok).val;_tmp832=*_tmp831;_tmp833=_tmp832.f1;_tmp834=_tmp832.f2;_LL43D: {
# 3125
struct Cyc_List_List*_tmp83E=0;
# 3126
{union Cyc_Absyn_Nmspace _tmp83F=_tmp833;struct Cyc_List_List*_tmp840;struct Cyc_List_List*_tmp841;struct Cyc_List_List*_tmp842;int _tmp843;_LL441: if((_tmp83F.Rel_n).tag != 1)goto _LL443;_tmp840=(struct Cyc_List_List*)(_tmp83F.Rel_n).val;_LL442:
# 3127
 _tmp83E=_tmp840;goto _LL440;_LL443: if((_tmp83F.Abs_n).tag != 2)goto _LL445;_tmp841=(struct Cyc_List_List*)(_tmp83F.Abs_n).val;_LL444:
# 3128
 _tmp83E=_tmp841;goto _LL440;_LL445: if((_tmp83F.C_n).tag != 3)goto _LL447;_tmp842=(struct Cyc_List_List*)(_tmp83F.C_n).val;_LL446:
# 3129
 _tmp83E=_tmp842;goto _LL440;_LL447: if((_tmp83F.Loc_n).tag != 4)goto _LL440;_tmp843=(int)(_tmp83F.Loc_n).val;_LL448:
# 3130
 goto _LL440;_LL440:;}
# 3132
for(0;_tmp83E != 0;_tmp83E=_tmp83E->tl){
# 3133
const char*_tmpECF;void*_tmpECE[1];struct Cyc_String_pa_PrintArg_struct _tmpECD;(_tmpECD.tag=0,((_tmpECD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp83E->hd)),((_tmpECE[0]=& _tmpECD,Cyc_fprintf(Cyc_stderr,((_tmpECF="%s::",_tag_dyneither(_tmpECF,sizeof(char),5))),_tag_dyneither(_tmpECE,sizeof(void*),1)))))));}
# 3134
{const char*_tmpED3;void*_tmpED2[1];struct Cyc_String_pa_PrintArg_struct _tmpED1;(_tmpED1.tag=0,((_tmpED1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp834),((_tmpED2[0]=& _tmpED1,Cyc_fprintf(Cyc_stderr,((_tmpED3="%s::",_tag_dyneither(_tmpED3,sizeof(char),5))),_tag_dyneither(_tmpED2,sizeof(void*),1)))))));}
# 3135
goto _LL435;}_LL43E:;_LL43F:
# 3136
{const char*_tmpED6;void*_tmpED5;(_tmpED5=0,Cyc_fprintf(Cyc_stderr,((_tmpED6="?",_tag_dyneither(_tmpED6,sizeof(char),2))),_tag_dyneither(_tmpED5,sizeof(void*),0)));}goto _LL435;_LL435:;}
# 3141
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
# 3142
Cyc_Parse_parse_result=0;{
# 3143
struct _RegionHandle _tmp84C=_new_region("yyr");struct _RegionHandle*yyr=& _tmp84C;_push_region(yyr);
# 3144
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
# 3145
struct Cyc_List_List*_tmp84D=Cyc_Parse_parse_result;_npop_handler(0);return _tmp84D;};
# 3144
;_pop_region(yyr);};}
