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
# 259
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
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
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
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
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
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
{struct _tuple10*_tmp806;declarators=((_tmp806=_region_malloc(r,sizeof(*_tmp806)),((_tmp806->tl=declarators,((_tmp806->hd=_tmp1,_tmp806))))));}{
struct Cyc_List_List*_tmp807;exprs=((_tmp807=_region_malloc(r,sizeof(*_tmp807)),((_tmp807->hd=_tmp2,((_tmp807->tl=exprs,_tmp807))))));};}
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
static void*Cyc_Parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp80A;struct Cyc_Position_Exit_exn_struct*_tmp809;(int)_throw((void*)((_tmp809=_cycalloc_atomic(sizeof(*_tmp809)),((_tmp809[0]=((_tmp80A.tag=Cyc_Position_Exit,_tmp80A)),_tmp809)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp80F;void*_tmp80E[2];struct Cyc_String_pa_PrintArg_struct _tmp80D;struct Cyc_String_pa_PrintArg_struct _tmp80C;(_tmp80C.tag=0,((_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp80D.tag=0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp80E[0]=& _tmp80D,((_tmp80E[1]=& _tmp80C,Cyc_fprintf(Cyc_stderr,((_tmp80F="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp80F,sizeof(char),46))),_tag_dyneither(_tmp80E,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=(*_tmpB).f2;_tmpD=(*_tmpB).f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp812;void*_tmp811;(_tmp811=0,Cyc_Tcutil_warn(loc,((_tmp812="qualifier on type is ignored",_tag_dyneither(_tmp812,sizeof(char),29))),_tag_dyneither(_tmp811,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp815;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp814;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=1,((_tmp815.f1=_tmp18,_tmp815)))),_tmp814)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp816;return(_tmp816.f1=nullable,((_tmp816.f2=bound,((_tmp816.f3=zeroterm,((_tmp816.f4=rgn,_tmp816)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp823;const char*_tmp822;void*_tmp821[1];struct Cyc_Int_pa_PrintArg_struct _tmp820;struct _tuple0*_tmp81F;return(_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F->f1=Cyc_Absyn_Rel_n(0),((_tmp81F->f2=(
(_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=(struct _dyneither_ptr)((_tmp820.tag=1,((_tmp820.f1=(unsigned long)enum_counter ++,((_tmp821[0]=& _tmp820,Cyc_aprintf(((_tmp822="__anonymous_enum_%d__",_tag_dyneither(_tmp822,sizeof(char),22))),_tag_dyneither(_tmp821,sizeof(void*),1)))))))),_tmp823)))),_tmp81F)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct _tuple15*f1;struct Cyc_Absyn_Exp*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple16*field_info){
# 246
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct _tuple16*_tmp23=field_info;_tmp24=(*(*_tmp23).f1).f1;_tmp25=(*(*_tmp23).f1).f2;_tmp26=(*(*_tmp23).f1).f3;_tmp27=(*(*_tmp23).f1).f4;_tmp28=(*(*_tmp23).f1).f5;_tmp29=(*_tmp23).f2;
if(_tmp27 != 0){
const char*_tmp824;Cyc_Parse_err(((_tmp824="bad type params in struct field",_tag_dyneither(_tmp824,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp825;Cyc_Parse_err(((_tmp825="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp825,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp826;return(_tmp826=_cycalloc(sizeof(*_tmp826)),((_tmp826->name=(*_tmp24).f2,((_tmp826->tq=_tmp25,((_tmp826->type=_tmp26,((_tmp826->width=_tmp29,((_tmp826->attributes=_tmp28,_tmp826)))))))))));};}
# 255
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp827;return(_tmp827.Signed_spec=0,((_tmp827.Unsigned_spec=0,((_tmp827.Short_spec=0,((_tmp827.Long_spec=0,((_tmp827.Long_Long_spec=0,((_tmp827.Valid_type_spec=0,((_tmp827.Type_spec=Cyc_Absyn_sint_typ,((_tmp827.loc=loc,_tmp827)))))))))))))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp82A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp829;return Cyc_Absyn_starb_typ(_tmp35,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp36,
# 301
_tmp37 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)(
(_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=1,((_tmp82A.f1=(struct Cyc_Absyn_Exp*)_tmp37,_tmp82A)))),_tmp829))))),_tmp38);}_LL14:;_LL15:
# 304
 return t;_LL11:;}struct _tuple17{struct _dyneither_ptr*f1;void*f2;};
# 317 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3C=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp3D=(struct _tuple8*)x->hd;struct _dyneither_ptr*_tmp3E;void*_tmp40;struct _dyneither_ptr _tmp41;void**_tmp44;_LL17: _tmp3E=(*_tmp3D).f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)(*_tmp3D).f3;if(_tmp3F->tag != 19)goto _LL19;else{_tmp40=(void*)_tmp3F->f1;}};if(!(_tmp3E != 0))goto _LL19;_LL18:
# 322
{void*_tmp45=_tmp40;void**_tmp47;_LL1E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45;if(_tmp46->tag != 1)goto _LL20;else{_tmp47=(void**)((void**)& _tmp46->f2);}}_LL1F: {
# 326
struct Cyc_String_pa_PrintArg_struct _tmp832;void*_tmp831[1];const char*_tmp830;struct _dyneither_ptr*_tmp82F;struct _dyneither_ptr*nm=(_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F[0]=(struct _dyneither_ptr)((_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp3E)),((_tmp831[0]=& _tmp832,Cyc_aprintf(((_tmp830="`%s",_tag_dyneither(_tmp830,sizeof(char),4))),_tag_dyneither(_tmp831,sizeof(void*),1)))))))),_tmp82F)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp841;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp840;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp83F;struct Cyc_Absyn_Tvar*_tmp83E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp83D;*_tmp47=(void*)((_tmp83D=_cycalloc(sizeof(*_tmp83D)),((_tmp83D[0]=((_tmp841.tag=2,((_tmp841.f1=((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E->name=nm,((_tmp83E->identity=- 1,((_tmp83E->kind=(void*)((_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((_tmp83F.tag=0,((_tmp83F.f1=& Cyc_Tcutil_ik,_tmp83F)))),_tmp840)))),_tmp83E)))))))),_tmp841)))),_tmp83D))));}
goto _LL1D;}_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 331
{struct _tuple17*_tmp844;struct Cyc_List_List*_tmp843;_tmp3C=((_tmp843=_cycalloc(sizeof(*_tmp843)),((_tmp843->hd=((_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844->f1=(struct _dyneither_ptr*)_tmp3E,((_tmp844->f2=_tmp40,_tmp844)))))),((_tmp843->tl=_tmp3C,_tmp843))))));}goto _LL16;_LL19: if((*_tmp3D).f1 == 0)goto _LL1B;_tmp41=*(*_tmp3D).f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)(*_tmp3D).f3;if(_tmp42->tag != 15)goto _LL1B;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp42->f1);if(_tmp43->tag != 1)goto _LL1B;else{_tmp44=(void**)((void**)& _tmp43->f2);}}};_LL1A: {
# 335
struct Cyc_String_pa_PrintArg_struct _tmp84C;void*_tmp84B[1];const char*_tmp84A;struct _dyneither_ptr*_tmp849;struct _dyneither_ptr*nm=(_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849[0]=(struct _dyneither_ptr)((_tmp84C.tag=0,((_tmp84C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp84B[0]=& _tmp84C,Cyc_aprintf(((_tmp84A="`%s",_tag_dyneither(_tmp84A,sizeof(char),4))),_tag_dyneither(_tmp84B,sizeof(void*),1)))))))),_tmp849)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp85B;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp85A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp859;struct Cyc_Absyn_Tvar*_tmp858;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp857;*_tmp44=(void*)((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=((_tmp85B.tag=2,((_tmp85B.f1=((_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858->name=nm,((_tmp858->identity=- 1,((_tmp858->kind=(void*)((_tmp85A=_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp859.tag=0,((_tmp859.f1=& Cyc_Tcutil_rk,_tmp859)))),_tmp85A)))),_tmp858)))))))),_tmp85B)))),_tmp857))));}
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
{struct _tuple17*_tmp85E;struct Cyc_List_List*_tmp85D;_tmp5C=((_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D->hd=((_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp85E->f2=_tmp5F,_tmp85E)))))),((_tmp85D->tl=_tmp5C,_tmp85D))))));}goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 354
return _tmp5C;}
# 358
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp62=e->r;struct _dyneither_ptr*_tmp64;_LL28: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp62;if(_tmp63->tag != 1)goto _LL2A;else{if((((*_tmp63->f1).f1).Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(((*_tmp63->f1).f1).Rel_n).val != 0)goto _LL2A;_tmp64=(*_tmp63->f1).f2;}}_LL29:
# 361
{struct Cyc_List_List*_tmp65=tags;for(0;_tmp65 != 0;_tmp65=_tmp65->tl){
struct _dyneither_ptr*_tmp67;void*_tmp68;struct _tuple17*_tmp66=(struct _tuple17*)_tmp65->hd;_tmp67=(*_tmp66).f1;_tmp68=(*_tmp66).f2;
if(Cyc_strptrcmp(_tmp67,_tmp64)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp861;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp860;return Cyc_Absyn_new_exp((void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=37,((_tmp861.f1=(void*)Cyc_Tcutil_copy_type(_tmp68),_tmp861)))),_tmp860)))),e->loc);}}}
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
nelts2=(struct Cyc_Absyn_Exp*)Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp6F);{
# 381
void*_tmp7A=Cyc_Parse_substitute_tags(tags,_tmp6D);
if(_tmp6F != nelts2  || _tmp6D != _tmp7A){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp867;struct Cyc_Absyn_ArrayInfo _tmp866;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp865;return(void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp867.tag=8,((_tmp867.f1=((_tmp866.elt_type=_tmp7A,((_tmp866.tq=_tmp6E,((_tmp866.num_elts=nelts2,((_tmp866.zero_term=_tmp70,((_tmp866.zt_loc=_tmp71,_tmp866)))))))))),_tmp867)))),_tmp865))));}
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B;if(_tmp72->tag != 5)goto _LL31;else{_tmp73=(_tmp72->f1).elt_typ;_tmp74=(_tmp72->f1).elt_tq;_tmp75=((_tmp72->f1).ptr_atts).rgn;_tmp76=((_tmp72->f1).ptr_atts).nullable;_tmp77=((_tmp72->f1).ptr_atts).bounds;_tmp78=((_tmp72->f1).ptr_atts).zero_term;_tmp79=((_tmp72->f1).ptr_atts).ptrloc;}}_LL30: {
# 386
void*_tmp7E=Cyc_Parse_substitute_tags(tags,_tmp73);
union Cyc_Absyn_Constraint*_tmp7F=_tmp77;
{union Cyc_Absyn_Constraint _tmp80=*_tmp77;struct Cyc_Absyn_Exp*_tmp82;_LL34: if((_tmp80.Eq_constr).tag != 1)goto _LL36;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp81=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp80.Eq_constr).val);if(_tmp81->tag != 1)goto _LL36;else{_tmp82=_tmp81->f1;}};_LL35:
# 390
 _tmp82=Cyc_Parse_substitute_tags_exp(tags,_tmp82);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp86C;union Cyc_Absyn_Constraint*_tmp86B;_tmp7F=((_tmp86B=_cycalloc(sizeof(*_tmp86B)),(((_tmp86B->Eq_constr).val=(void*)((_tmp86D=_cycalloc(sizeof(*_tmp86D)),((_tmp86D[0]=((_tmp86C.tag=1,((_tmp86C.f1=_tmp82,_tmp86C)))),_tmp86D)))),(((_tmp86B->Eq_constr).tag=1,_tmp86B))))));}
goto _LL33;_LL36:;_LL37:
 goto _LL33;_LL33:;}
# 395
if(_tmp7E != _tmp73  || _tmp7F != _tmp77){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp877;struct Cyc_Absyn_PtrAtts _tmp876;struct Cyc_Absyn_PtrInfo _tmp875;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp874;return(void*)((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874[0]=((_tmp877.tag=5,((_tmp877.f1=((_tmp875.elt_typ=_tmp7E,((_tmp875.elt_tq=_tmp74,((_tmp875.ptr_atts=((_tmp876.rgn=_tmp75,((_tmp876.nullable=_tmp76,((_tmp876.bounds=_tmp7F,((_tmp876.zero_term=_tmp78,((_tmp876.ptrloc=_tmp79,_tmp876)))))))))),_tmp875)))))),_tmp877)))),_tmp874))));}
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
struct _tuple18*_tmp878;return(_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878->f1=(*t).f2,((_tmp878->f2=(*t).f3,_tmp878)))));}
# 419
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp8B=d->r;struct Cyc_Absyn_Vardecl*_tmp8D;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL3B;else{_tmp8D=_tmp8C->f1;}}_LL3A:
 return _tmp8D;_LL3B:;_LL3C: {
const char*_tmp87B;void*_tmp87A;(_tmp87A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp87B="bad declaration in `forarray' statement",_tag_dyneither(_tmp87B,sizeof(char),40))),_tag_dyneither(_tmp87A,sizeof(void*),0)));}_LL38:;}
# 426
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp90=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp91=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp90;if(_tmp91->tag != 4)goto _LL40;}_LL3F:
 return 1;_LL40:;_LL41:
 return 0;_LL3D:;}
# 435
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp87C;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp87C="`H",_tag_dyneither(_tmp87C,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp87D;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp87D="`U",_tag_dyneither(_tmp87D,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp87E;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp87E="`RC",_tag_dyneither(_tmp87E,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 443
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp888;struct _dyneither_ptr*_tmp887;struct Cyc_Absyn_Tvar*_tmp886;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp885;return(void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp888.tag=2,((_tmp888.f1=((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886->name=((_tmp887=_cycalloc(sizeof(*_tmp887)),((_tmp887[0]=s,_tmp887)))),((_tmp886->identity=- 1,((_tmp886->kind=k,_tmp886)))))))),_tmp888)))),_tmp885))));}}}}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp99=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL45;}_LL44:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp88B;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp88A;k=(void*)((_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=1,((_tmp88B.f1=0,_tmp88B)))),_tmp88A))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f2;}}_LL46:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp88E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp88D;k=(void*)((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88E.tag=2,((_tmp88E.f1=0,((_tmp88E.f2=_tmp9C,_tmp88E)))))),_tmp88D))));}goto _LL42;_LL47: _tmp9D=_tmp99;_LL48:
 k=_tmp9D;goto _LL42;_LL42:;}{
# 453
struct Cyc_Absyn_Tvar*_tmp88F;return(_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F->name=t->name,((_tmp88F->identity=- 1,((_tmp88F->kind=k,_tmp88F)))))));};}
# 460
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL4C;else{_tmpA5=_tmpA4->f1;}}_LL4B:
 return _tmpA5;_LL4C:;_LL4D: {
const char*_tmp892;void*_tmp891;(_tmp891=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp892="expecting a list of type variables, not types",_tag_dyneither(_tmp892,sizeof(char),46))),_tag_dyneither(_tmp891,sizeof(void*),0)));}_LL49:;}
# 466
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp895;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp894;return(void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=2,((_tmp895.f1=pr,_tmp895)))),_tmp894))));}
# 471
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void**_tmpAC;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAA;if(_tmpAB->tag != 2)goto _LL51;else{_tmpAC=(void**)&(*_tmpAB->f1).kind;}}_LL50: {
# 474
void*_tmpAD=Cyc_Absyn_compress_kb(*_tmpAC);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LL56;}_LL55:
# 476
 if(!leq)*_tmpAC=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp898;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp897;*_tmpAC=(void*)((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp898.tag=2,((_tmp898.f1=0,((_tmp898.f2=k,_tmp898)))))),_tmp897))));}
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
void*_tmpB1=(void*)tms->hd;void*_tmpB3;_LL59: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB2=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB1;if(_tmpB2->tag != 3)goto _LL5B;else{_tmpB3=(void*)_tmpB2->f1;}}_LL5A:
# 502
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 505
void*_tmpB4=_tmpB3;struct Cyc_List_List*_tmpB7;_LL5E: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB4;if(_tmpB5->tag != 1)goto _LL60;}_LL5F:
# 507
{const char*_tmp89B;void*_tmp89A;(_tmp89A=0,Cyc_Tcutil_warn(loc,((_tmp89B="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp89B,sizeof(char),93))),_tag_dyneither(_tmp89A,sizeof(void*),0)));}
# 509
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB6=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB4;if(_tmpB6->tag != 0)goto _LL5D;else{_tmpB7=_tmpB6->f1;}}_LL61:
# 511
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp89E;void*_tmp89D;(_tmp89D=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp89E="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp89E,sizeof(char),73))),_tag_dyneither(_tmp89D,sizeof(void*),0)));}{
# 515
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpB7 != 0;_tmpB7=_tmpB7->tl){
struct Cyc_List_List*_tmpBC=tds;
for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){
struct Cyc_Absyn_Decl*_tmpBD=(struct Cyc_Absyn_Decl*)_tmpBC->hd;
void*_tmpBE=_tmpBD->r;struct Cyc_Absyn_Vardecl*_tmpC0;_LL63: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpBF->tag != 0)goto _LL65;else{_tmpC0=_tmpBF->f1;}}_LL64:
# 522
 if(Cyc_zstrptrcmp((*_tmpC0->name).f2,(struct _dyneither_ptr*)_tmpB7->hd)!= 0)
continue;
if(_tmpC0->initializer != 0){
const char*_tmp8A1;void*_tmp8A0;(_tmp8A0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpBD->loc,((_tmp8A1="initializer found in parameter declaration",_tag_dyneither(_tmp8A1,sizeof(char),43))),_tag_dyneither(_tmp8A0,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC0->name)){
const char*_tmp8A4;void*_tmp8A3;(_tmp8A3=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpBD->loc,((_tmp8A4="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8A4,sizeof(char),47))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}
{struct _tuple8*_tmp8A7;struct Cyc_List_List*_tmp8A6;rev_new_params=(
(_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6->hd=((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7->f1=(struct _dyneither_ptr*)(*_tmpC0->name).f2,((_tmp8A7->f2=_tmpC0->tq,((_tmp8A7->f3=_tmpC0->type,_tmp8A7)))))))),((_tmp8A6->tl=rev_new_params,_tmp8A6))))));}
# 531
goto L;_LL65:;_LL66: {
const char*_tmp8AA;void*_tmp8A9;(_tmp8A9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpBD->loc,((_tmp8AA="nonvariable declaration in parameter type",_tag_dyneither(_tmp8AA,sizeof(char),42))),_tag_dyneither(_tmp8A9,sizeof(void*),0)));}_LL62:;}
# 535
L: if(_tmpBC == 0){
const char*_tmp8AE;void*_tmp8AD[1];struct Cyc_String_pa_PrintArg_struct _tmp8AC;(_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpB7->hd)),((_tmp8AD[0]=& _tmp8AC,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8AE="%s is not given a type",_tag_dyneither(_tmp8AE,sizeof(char),23))),_tag_dyneither(_tmp8AD,sizeof(void*),1)))))));}}{
# 538
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8BD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8BC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8BB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8BA;struct Cyc_List_List*_tmp8B9;return
(_tmp8B9=_region_malloc(yy,sizeof(*_tmp8B9)),((_tmp8B9->hd=(void*)((_tmp8BD=_region_malloc(yy,sizeof(*_tmp8BD)),((_tmp8BD[0]=((_tmp8BA.tag=3,((_tmp8BA.f1=(void*)((void*)((_tmp8BC=_region_malloc(yy,sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BB.tag=1,((_tmp8BB.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8BB.f2=0,((_tmp8BB.f3=0,((_tmp8BB.f4=(void*)0,((_tmp8BB.f5=0,_tmp8BB)))))))))))),_tmp8BC))))),_tmp8BA)))),_tmp8BD)))),((_tmp8B9->tl=0,_tmp8B9)))));};};_LL5D:;}
# 545
goto _LL5C;_LL5B:;_LL5C: {
struct Cyc_List_List*_tmp8BE;return(_tmp8BE=_region_malloc(yy,sizeof(*_tmp8BE)),((_tmp8BE->hd=(void*)tms->hd,((_tmp8BE->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8BE)))));}_LL58:;};}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 557
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8BF;d=((_tmp8BF.id=d.id,((_tmp8BF.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8BF))));}{
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
{const char*_tmp8C0;Cyc_Parse_err(((_tmp8C0="bad storage class on function",_tag_dyneither(_tmp8C0,sizeof(char),30))),loc);}break;}}{
# 579
void*_tmpD4=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct Cyc_Absyn_Tqual _tmpD6;void*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD9;struct _tuple13 _tmpD5=Cyc_Parse_apply_tms(tq,_tmpD4,atts,d.tms);_tmpD6=_tmpD5.f1;_tmpD7=_tmpD5.f2;_tmpD8=_tmpD5.f3;_tmpD9=_tmpD5.f4;
# 584
if(_tmpD8 != 0){
# 587
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_Tcutil_warn(loc,((_tmp8C3="bad type params, ignoring",_tag_dyneither(_tmp8C3,sizeof(char),26))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}{
# 589
void*_tmpDC=_tmpD7;struct Cyc_List_List*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE1;struct Cyc_List_List*_tmpE2;int _tmpE3;struct Cyc_Absyn_VarargInfo*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDC;if(_tmpDD->tag != 9)goto _LL6E;else{_tmpDE=(_tmpDD->f1).tvars;_tmpDF=(_tmpDD->f1).effect;_tmpE0=(_tmpDD->f1).ret_tqual;_tmpE1=(_tmpDD->f1).ret_typ;_tmpE2=(_tmpDD->f1).args;_tmpE3=(_tmpDD->f1).c_varargs;_tmpE4=(_tmpDD->f1).cyc_varargs;_tmpE5=(_tmpDD->f1).rgn_po;_tmpE6=(_tmpDD->f1).attributes;}}_LL6D: {
# 592
struct Cyc_List_List*_tmpE7=0;
{struct Cyc_List_List*_tmpE8=_tmpE2;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEC;struct _tuple8*_tmpE9=(struct _tuple8*)_tmpE8->hd;_tmpEA=(*_tmpE9).f1;_tmpEB=(*_tmpE9).f2;_tmpEC=(*_tmpE9).f3;
if(_tmpEA == 0){
{const char*_tmp8C4;Cyc_Parse_err(((_tmp8C4="missing argument variable in function prototype",_tag_dyneither(_tmp8C4,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8C7;struct Cyc_List_List*_tmp8C6;_tmpE7=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->hd=((_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8C7->f2=_tmpEB,((_tmp8C7->f3=_tmpEC,_tmp8C7)))))))),((_tmp8C6->tl=_tmpE7,_tmp8C6))))));};}else{
# 599
struct _tuple8*_tmp8CA;struct Cyc_List_List*_tmp8C9;_tmpE7=((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9->hd=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->f1=(struct _dyneither_ptr*)_tmpEA,((_tmp8CA->f2=_tmpEB,((_tmp8CA->f3=_tmpEC,_tmp8CA)))))))),((_tmp8C9->tl=_tmpE7,_tmp8C9))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp8CB;return(_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB->sc=sc,((_tmp8CB->name=d.id,((_tmp8CB->tvs=_tmpDE,((_tmp8CB->is_inline=is_inline,((_tmp8CB->effect=_tmpDF,((_tmp8CB->ret_tqual=_tmpE0,((_tmp8CB->ret_type=_tmpE1,((_tmp8CB->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE7),((_tmp8CB->c_varargs=_tmpE3,((_tmp8CB->cyc_varargs=_tmpE4,((_tmp8CB->rgn_po=_tmpE5,((_tmp8CB->body=body,((_tmp8CB->cached_typ=0,((_tmp8CB->param_vardecls=0,((_tmp8CB->fn_vardecl=0,((_tmp8CB->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE6,_tmpD9),_tmp8CB)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
const char*_tmp8CE;void*_tmp8CD;(_tmp8CD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8CE="declarator is not a function prototype",_tag_dyneither(_tmp8CE,sizeof(char),39))),_tag_dyneither(_tmp8CD,sizeof(void*),0)));}_LL6B:;};};};}static char _tmpF6[52]="at most one type may appear within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpF6,_tmpF6,_tmpF6 + 52};static char _tmpF7[63]="const or volatile may appear only once within a type specifier";
# 620
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpF7,_tmpF7,_tmpF7 + 63};static char _tmpF8[50]="type specifier includes more than one declaration";
# 622
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpF8,_tmpF8,_tmpF8 + 50};static char _tmpF9[60]="sign specifier may appear only once within a type specifier";
# 624
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpF9,_tmpF9,_tmpF9 + 60};
# 631
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 634
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp8CF;(_tmp8CF=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8CF,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D0,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8D1;(_tmp8D1=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D1,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D2,sizeof(void*),0)));}
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
const char*_tmp8D5;void*_tmp8D4;(_tmp8D4=0,Cyc_Tcutil_warn(loc,((_tmp8D5="missing type within specifier",_tag_dyneither(_tmp8D5,sizeof(char),30))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 689
if(seen_sign){
void*_tmp100=t;enum Cyc_Absyn_Sign _tmp102;enum Cyc_Absyn_Size_of _tmp103;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 6)goto _LL73;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;}}_LL72:
# 692
 if(_tmp102 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp103);
goto _LL70;_LL73:;_LL74:
{const char*_tmp8D6;Cyc_Parse_err(((_tmp8D6="sign specification on non-integral type",_tag_dyneither(_tmp8D6,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 697
if(seen_size){
void*_tmp105=t;enum Cyc_Absyn_Sign _tmp107;enum Cyc_Absyn_Size_of _tmp108;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp105;if(_tmp106->tag != 6)goto _LL78;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}_LL77:
# 700
 if(_tmp108 != sz)
t=Cyc_Absyn_int_typ(_tmp107,sz);
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp109=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp105;if(_tmp109->tag != 7)goto _LL7A;}_LL79:
# 704
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
{const char*_tmp8D7;Cyc_Parse_err(((_tmp8D7="size qualifier on non-integral type",_tag_dyneither(_tmp8D7,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
# 708
return t;}
# 711
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 715
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp10B=d.id;
struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp110;struct _tuple13 _tmp10C=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;_tmp10F=_tmp10C.f3;_tmp110=_tmp10C.f4;
# 721
if(ds->tl == 0){
struct _tuple15*_tmp8DA;struct Cyc_List_List*_tmp8D9;return(_tmp8D9=_region_malloc(r,sizeof(*_tmp8D9)),((_tmp8D9->hd=((_tmp8DA=_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA->f1=_tmp10B,((_tmp8DA->f2=_tmp10D,((_tmp8DA->f3=_tmp10E,((_tmp8DA->f4=_tmp10F,((_tmp8DA->f5=_tmp110,_tmp8DA)))))))))))),((_tmp8D9->tl=0,_tmp8D9)))));}else{
# 724
struct _tuple15*_tmp8DD;struct Cyc_List_List*_tmp8DC;return(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->hd=((_tmp8DD=_region_malloc(r,sizeof(*_tmp8DD)),((_tmp8DD->f1=_tmp10B,((_tmp8DD->f2=_tmp10D,((_tmp8DD->f3=_tmp10E,((_tmp8DD->f4=_tmp10F,((_tmp8DD->f5=_tmp110,_tmp8DD)))))))))))),((_tmp8DC->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8DC)))));}};}
# 728
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 731
if(tms == 0){struct _tuple13 _tmp8DE;return(_tmp8DE.f1=tq,((_tmp8DE.f2=t,((_tmp8DE.f3=0,((_tmp8DE.f4=atts,_tmp8DE)))))));}{
void*_tmp116=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp118;unsigned int _tmp119;struct Cyc_Absyn_Exp*_tmp11B;union Cyc_Absyn_Constraint*_tmp11C;unsigned int _tmp11D;void*_tmp11F;struct Cyc_List_List*_tmp121;unsigned int _tmp122;struct Cyc_Absyn_PtrAtts _tmp124;struct Cyc_Absyn_Tqual _tmp125;unsigned int _tmp127;struct Cyc_List_List*_tmp128;_LL7D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp117->tag != 0)goto _LL7F;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7E:
# 734
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp118,_tmp119),atts,tms->tl);_LL7F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11A->tag != 1)goto _LL81;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;_tmp11D=_tmp11A->f3;}}_LL80:
# 737
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp11B,_tmp11C,_tmp11D),atts,tms->tl);_LL81: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp11E=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11E->tag != 3)goto _LL83;else{_tmp11F=(void*)_tmp11E->f1;}}_LL82: {
# 740
void*_tmp129=_tmp11F;struct Cyc_List_List*_tmp12B;int _tmp12C;struct Cyc_Absyn_VarargInfo*_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;unsigned int _tmp131;_LL8A: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp12A->tag != 1)goto _LL8C;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=(void*)_tmp12A->f4;_tmp12F=_tmp12A->f5;}}_LL8B: {
# 742
struct Cyc_List_List*typvars=0;
# 744
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp8DF;fn_atts=((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF->hd=(void*)as->hd,((_tmp8DF->tl=fn_atts,_tmp8DF))))));}else{
# 749
struct Cyc_List_List*_tmp8E0;new_atts=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->hd=(void*)as->hd,((_tmp8E0->tl=new_atts,_tmp8E0))))));}}}
# 752
if(tms->tl != 0){
void*_tmp134=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp136;_LL8F: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp135=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp134;if(_tmp135->tag != 4)goto _LL91;else{_tmp136=_tmp135->f1;}}_LL90:
# 755
 typvars=_tmp136;
tms=tms->tl;
goto _LL8E;_LL91:;_LL92:
# 759
 goto _LL8E;_LL8E:;}
# 763
if(((((!_tmp12C  && _tmp12D == 0) && _tmp12B != 0) && _tmp12B->tl == 0) && (*((struct _tuple8*)_tmp12B->hd)).f1 == 0) && (*((struct _tuple8*)_tmp12B->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 768
_tmp12B=0;{
# 771
struct Cyc_List_List*_tmp137=Cyc_Parse_get_arg_tags(_tmp12B);
# 773
if(_tmp137 != 0)
t=Cyc_Parse_substitute_tags(_tmp137,t);
t=Cyc_Parse_array2ptr(t,0);
# 778
{struct Cyc_List_List*_tmp138=_tmp12B;for(0;_tmp138 != 0;_tmp138=_tmp138->tl){
struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Tqual _tmp13B;void**_tmp13C;struct _tuple8*_tmp139=(struct _tuple8*)_tmp138->hd;_tmp13A=(*_tmp139).f1;_tmp13B=(*_tmp139).f2;_tmp13C=(void**)&(*_tmp139).f3;
if(_tmp137 != 0)
*_tmp13C=Cyc_Parse_substitute_tags(_tmp137,*_tmp13C);
*_tmp13C=Cyc_Parse_array2ptr(*_tmp13C,1);}}
# 790
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp12E,tq,t,_tmp12B,_tmp12C,_tmp12D,_tmp12F,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL8C: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp130=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp130->tag != 0)goto _LL89;else{_tmp131=_tmp130->f2;}}_LL8D: {
# 796
const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp131,((_tmp8E3="function declaration without parameter types",_tag_dyneither(_tmp8E3,sizeof(char),45))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp120=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp120->tag != 4)goto _LL85;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL84:
# 803
 if(tms->tl == 0){
struct _tuple13 _tmp8E4;return(_tmp8E4.f1=tq,((_tmp8E4.f2=t,((_tmp8E4.f3=_tmp121,((_tmp8E4.f4=atts,_tmp8E4)))))));}{
# 808
const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp122,((_tmp8E7="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp8E7,sizeof(char),68))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp123=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp123->tag != 2)goto _LL87;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LL86: {
# 811
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8ED;struct Cyc_Absyn_PtrInfo _tmp8EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8EB;return Cyc_Parse_apply_tms(_tmp125,(void*)((_tmp8EB=_cycalloc(sizeof(*_tmp8EB)),((_tmp8EB[0]=((_tmp8ED.tag=5,((_tmp8ED.f1=((_tmp8EC.elt_typ=t,((_tmp8EC.elt_tq=tq,((_tmp8EC.ptr_atts=_tmp124,_tmp8EC)))))),_tmp8ED)))),_tmp8EB)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp126=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp126->tag != 5)goto _LL7C;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LL88:
# 816
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp128),tms->tl);_LL7C:;};}
# 822
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 831
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp146;struct Cyc_Absyn_Tqual _tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp14A;struct _tuple15*_tmp145=t;_tmp146=(*_tmp145).f1;_tmp147=(*_tmp145).f2;_tmp148=(*_tmp145).f3;_tmp149=(*_tmp145).f4;_tmp14A=(*_tmp145).f5;
# 834
Cyc_Lex_register_typedef(_tmp146);{
# 836
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp14B=_tmp148;struct Cyc_Core_Opt*_tmp14D;_LL94: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14B;if(_tmp14C->tag != 1)goto _LL96;else{_tmp14D=_tmp14C->f1;}}_LL95:
# 840
 type=0;
if(_tmp14D == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;else{
kind=_tmp14D;}
goto _LL93;_LL96:;_LL97:
 kind=0;type=(void*)_tmp148;goto _LL93;_LL93:;}{
# 846
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8F3;struct Cyc_Absyn_Typedefdecl*_tmp8F2;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp8F1;return Cyc_Absyn_new_decl((void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F3.tag=9,((_tmp8F3.f1=((_tmp8F2=_cycalloc(sizeof(*_tmp8F2)),((_tmp8F2->name=_tmp146,((_tmp8F2->tvs=_tmp149,((_tmp8F2->kind=kind,((_tmp8F2->defn=type,((_tmp8F2->atts=_tmp14A,((_tmp8F2->tq=_tmp147,_tmp8F2)))))))))))))),_tmp8F3)))),_tmp8F1)))),loc);};};}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp8F6;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp8F5;return Cyc_Absyn_new_stmt((void*)((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F6.tag=12,((_tmp8F6.f1=d,((_tmp8F6.f2=s,_tmp8F6)))))),_tmp8F5)))),d->loc);}
# 859
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 868
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 872
struct _RegionHandle _tmp153=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp153;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp155;struct Cyc_Parse_Type_specifier _tmp156;struct Cyc_List_List*_tmp157;struct Cyc_Parse_Declaration_spec _tmp154=ds;_tmp155=_tmp154.tq;_tmp156=_tmp154.type_specs;_tmp157=_tmp154.attributes;
if(_tmp155.loc == 0)_tmp155.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp8F7;Cyc_Parse_err(((_tmp8F7="inline is allowed only on function definitions",_tag_dyneither(_tmp8F7,sizeof(char),47))),loc);}{
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
void*_tmp159=Cyc_Parse_collapse_type_specifiers(_tmp156,loc);
if(declarators == 0){
# 910
void*_tmp15A=_tmp159;struct Cyc_Absyn_Aggrdecl*_tmp15D;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Datatypedecl*_tmp163;enum Cyc_Absyn_AggrKind _tmp165;struct _tuple0*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Datatypedecl**_tmp169;struct _tuple0*_tmp16B;int _tmp16C;struct Cyc_List_List*_tmp16D;struct _tuple0*_tmp16F;struct Cyc_List_List*_tmp171;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15B->tag != 26)goto _LLA3;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp15C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(*_tmp15B->f1).r;if(_tmp15C->tag != 0)goto _LLA3;else{_tmp15D=_tmp15C->f1;}}}_LLA2:
# 912
 _tmp15D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp15D->attributes,_tmp157);{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp8FD;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp8FC;struct Cyc_List_List*_tmp8FB;struct Cyc_List_List*_tmp175=(_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB->hd=Cyc_Absyn_new_decl((void*)((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD[0]=((_tmp8FC.tag=6,((_tmp8FC.f1=_tmp15D,_tmp8FC)))),_tmp8FD)))),loc),((_tmp8FB->tl=0,_tmp8FB)))));_npop_handler(0);return _tmp175;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15E->tag != 26)goto _LLA5;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp15F=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(*_tmp15E->f1).r;if(_tmp15F->tag != 1)goto _LLA5;else{_tmp160=_tmp15F->f1;}}}_LLA4:
# 915
 if(_tmp157 != 0){const char*_tmp8FE;Cyc_Parse_err(((_tmp8FE="attributes on enum not supported",_tag_dyneither(_tmp8FE,sizeof(char),33))),loc);}{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp904;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp903;struct Cyc_List_List*_tmp902;struct Cyc_List_List*_tmp17A=(_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->hd=Cyc_Absyn_new_decl((void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp903.tag=8,((_tmp903.f1=_tmp160,_tmp903)))),_tmp904)))),loc),((_tmp902->tl=0,_tmp902)))));_npop_handler(0);return _tmp17A;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp161->tag != 26)goto _LLA7;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp162=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(*_tmp161->f1).r;if(_tmp162->tag != 2)goto _LLA7;else{_tmp163=_tmp162->f1;}}}_LLA6:
# 918
 if(_tmp157 != 0){const char*_tmp905;Cyc_Parse_err(((_tmp905="attributes on datatypes not supported",_tag_dyneither(_tmp905,sizeof(char),38))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp90B;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp90A;struct Cyc_List_List*_tmp909;struct Cyc_List_List*_tmp17F=(_tmp909=_cycalloc(sizeof(*_tmp909)),((_tmp909->hd=Cyc_Absyn_new_decl((void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90A.tag=7,((_tmp90A.f1=_tmp163,_tmp90A)))),_tmp90B)))),loc),((_tmp909->tl=0,_tmp909)))));_npop_handler(0);return _tmp17F;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15A;if(_tmp164->tag != 11)goto _LLA9;else{if((((_tmp164->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA9;_tmp165=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f1;_tmp166=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f2;_tmp167=(_tmp164->f1).targs;}}_LLA8: {
# 921
struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp167);
struct Cyc_Absyn_Aggrdecl*_tmp90C;struct Cyc_Absyn_Aggrdecl*_tmp181=(_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C->kind=_tmp165,((_tmp90C->sc=s,((_tmp90C->name=_tmp166,((_tmp90C->tvs=_tmp180,((_tmp90C->impl=0,((_tmp90C->attributes=0,_tmp90C)))))))))))));
if(_tmp157 != 0){const char*_tmp90D;Cyc_Parse_err(((_tmp90D="bad attributes on type declaration",_tag_dyneither(_tmp90D,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp913;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp912;struct Cyc_List_List*_tmp911;struct Cyc_List_List*_tmp186=(_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911->hd=Cyc_Absyn_new_decl((void*)((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913[0]=((_tmp912.tag=6,((_tmp912.f1=_tmp181,_tmp912)))),_tmp913)))),loc),((_tmp911->tl=0,_tmp911)))));_npop_handler(0);return _tmp186;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp168->tag != 3)goto _LLAB;else{if((((_tmp168->f1).datatype_info).KnownDatatype).tag != 2)goto _LLAB;_tmp169=(struct Cyc_Absyn_Datatypedecl**)(((_tmp168->f1).datatype_info).KnownDatatype).val;}}_LLAA:
# 926
 if(_tmp157 != 0){const char*_tmp914;Cyc_Parse_err(((_tmp914="bad attributes on datatype",_tag_dyneither(_tmp914,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp91A;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp919;struct Cyc_List_List*_tmp918;struct Cyc_List_List*_tmp18C=(_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->hd=Cyc_Absyn_new_decl((void*)((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp919.tag=7,((_tmp919.f1=*_tmp169,_tmp919)))),_tmp91A)))),loc),((_tmp918->tl=0,_tmp918)))));_npop_handler(0);return _tmp18C;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp16A->tag != 3)goto _LLAD;else{if((((_tmp16A->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLAD;_tmp16B=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).name;_tmp16C=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp16D=(_tmp16A->f1).targs;}}_LLAC: {
# 929
struct Cyc_List_List*_tmp18D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp16D);
struct Cyc_Absyn_Decl*_tmp18E=Cyc_Absyn_datatype_decl(s,_tmp16B,_tmp18D,0,_tmp16C,loc);
if(_tmp157 != 0){const char*_tmp91B;Cyc_Parse_err(((_tmp91B="bad attributes on datatype",_tag_dyneither(_tmp91B,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp91C;struct Cyc_List_List*_tmp191=(_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->hd=_tmp18E,((_tmp91C->tl=0,_tmp91C)))));_npop_handler(0);return _tmp191;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15A;if(_tmp16E->tag != 13)goto _LLAF;else{_tmp16F=_tmp16E->f1;}}_LLAE: {
# 934
struct Cyc_Absyn_Enumdecl*_tmp91D;struct Cyc_Absyn_Enumdecl*_tmp192=(_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D->sc=s,((_tmp91D->name=_tmp16F,((_tmp91D->fields=0,_tmp91D)))))));
if(_tmp157 != 0){const char*_tmp91E;Cyc_Parse_err(((_tmp91E="bad attributes on enum",_tag_dyneither(_tmp91E,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp928;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp927;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp926;struct Cyc_List_List*_tmp925;struct Cyc_List_List*_tmp198=(_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925->hd=((_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928->r=(void*)((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=8,((_tmp927.f1=_tmp192,_tmp927)))),_tmp926)))),((_tmp928->loc=loc,_tmp928)))))),((_tmp925->tl=0,_tmp925)))));_npop_handler(0);return _tmp198;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15A;if(_tmp170->tag != 14)goto _LLB1;else{_tmp171=_tmp170->f1;}}_LLB0: {
# 940
struct Cyc_Core_Opt*_tmp92B;struct Cyc_Absyn_Enumdecl*_tmp92A;struct Cyc_Absyn_Enumdecl*_tmp19A=(_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A->sc=s,((_tmp92A->name=Cyc_Parse_gensym_enum(),((_tmp92A->fields=((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->v=_tmp171,_tmp92B)))),_tmp92A)))))));
if(_tmp157 != 0){const char*_tmp92C;Cyc_Parse_err(((_tmp92C="bad attributes on enum",_tag_dyneither(_tmp92C,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp936;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp935;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp934;struct Cyc_List_List*_tmp933;struct Cyc_List_List*_tmp1A0=(_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933->hd=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->r=(void*)((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=8,((_tmp935.f1=_tmp19A,_tmp935)))),_tmp934)))),((_tmp936->loc=loc,_tmp936)))))),((_tmp933->tl=0,_tmp933)))));_npop_handler(0);return _tmp1A0;};}_LLB1:;_LLB2:
{const char*_tmp937;Cyc_Parse_err(((_tmp937="missing declarator",_tag_dyneither(_tmp937,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1A4=0;_npop_handler(0);return _tmp1A4;};_LLA0:;}else{
# 947
struct Cyc_List_List*_tmp1A5=Cyc_Parse_apply_tmss(mkrgn,_tmp155,_tmp159,declarators,_tmp157);
if(istypedef){
# 952
if(!exps_empty){
const char*_tmp938;Cyc_Parse_err(((_tmp938="initializer in typedef declaration",_tag_dyneither(_tmp938,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1A5);
struct Cyc_List_List*_tmp1A7=decls;_npop_handler(0);return _tmp1A7;};}else{
# 958
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1A8=_tmp1A5;for(0;_tmp1A8 != 0;(_tmp1A8=_tmp1A8->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple0*_tmp1AA;struct Cyc_Absyn_Tqual _tmp1AB;void*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct _tuple15*_tmp1A9=(struct _tuple15*)_tmp1A8->hd;_tmp1AA=(*_tmp1A9).f1;_tmp1AB=(*_tmp1A9).f2;_tmp1AC=(*_tmp1A9).f3;_tmp1AD=(*_tmp1A9).f4;_tmp1AE=(*_tmp1A9).f5;
if(_tmp1AD != 0){
const char*_tmp93B;void*_tmp93A;(_tmp93A=0,Cyc_Tcutil_warn(loc,((_tmp93B="bad type params, ignoring",_tag_dyneither(_tmp93B,sizeof(char),26))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp93E="unexpected NULL in parse!",_tag_dyneither(_tmp93E,sizeof(char),26))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1B3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1B4=Cyc_Absyn_new_vardecl(_tmp1AA,_tmp1AC,_tmp1B3);
_tmp1B4->tq=_tmp1AB;
_tmp1B4->sc=s;
_tmp1B4->attributes=_tmp1AE;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp944;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp943;struct Cyc_Absyn_Decl*_tmp942;struct Cyc_Absyn_Decl*_tmp1B5=(_tmp942=_cycalloc(sizeof(*_tmp942)),((_tmp942->r=(void*)((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944[0]=((_tmp943.tag=0,((_tmp943.f1=_tmp1B4,_tmp943)))),_tmp944)))),((_tmp942->loc=loc,_tmp942)))));
struct Cyc_List_List*_tmp945;decls=((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945->hd=_tmp1B5,((_tmp945->tl=decls,_tmp945))))));};};}}{
# 973
struct Cyc_List_List*_tmp1BA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1BA;};}}};};};};}
# 873
;_pop_region(mkrgn);}
# 980
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB3:
 return& Cyc_Tcutil_ak;case 'M': _LLB4:
 return& Cyc_Tcutil_mk;case 'B': _LLB5:
 return& Cyc_Tcutil_bk;case 'R': _LLB6:
 return& Cyc_Tcutil_rk;case 'E': _LLB7:
 return& Cyc_Tcutil_ek;case 'I': _LLB8:
 return& Cyc_Tcutil_ik;case 'U': _LLB9:
# 990
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLBB:
 return& Cyc_Tcutil_urk;case 'A': _LLBC:
 return& Cyc_Tcutil_uak;case 'M': _LLBD:
 return& Cyc_Tcutil_umk;case 'B': _LLBE:
 return& Cyc_Tcutil_ubk;default: _LLBF:
 break;}
# 997
break;case 'T': _LLBA:
# 999
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLC2:
 return& Cyc_Tcutil_trk;case 'A': _LLC3:
 return& Cyc_Tcutil_tak;case 'M': _LLC4:
 return& Cyc_Tcutil_tmk;case 'B': _LLC5:
 return& Cyc_Tcutil_tbk;default: _LLC6:
 break;}
# 1006
break;default: _LLC1:
 break;}
# 1009
{const char*_tmp94A;void*_tmp949[2];struct Cyc_String_pa_PrintArg_struct _tmp948;struct Cyc_Int_pa_PrintArg_struct _tmp947;Cyc_Parse_err((struct _dyneither_ptr)((_tmp947.tag=1,((_tmp947.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp948.tag=0,((_tmp948.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp949[0]=& _tmp948,((_tmp949[1]=& _tmp947,Cyc_aprintf(((_tmp94A="bad kind: %s; strlen=%d",_tag_dyneither(_tmp94A,sizeof(char),24))),_tag_dyneither(_tmp949,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BF=p->r;struct _tuple0*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C3;struct Cyc_Absyn_Pat*_tmp1C6;enum Cyc_Absyn_Sign _tmp1C9;int _tmp1CA;char _tmp1CC;struct _dyneither_ptr _tmp1CE;int _tmp1CF;struct _tuple0*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;_LLCA: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1C0=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C0->tag != 14)goto _LLCC;else{_tmp1C1=_tmp1C0->f1;}}_LLCB:
 return Cyc_Absyn_unknownid_exp(_tmp1C1,p->loc);_LLCC: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C2->tag != 2)goto _LLCE;else{_tmp1C3=_tmp1C2->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp1C2->f2).r;if(_tmp1C4->tag != 0)goto _LLCE;};}}_LLCD:
# 1018
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1C3->name,p->loc),p->loc);_LLCE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1C5=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C5->tag != 5)goto _LLD0;else{_tmp1C6=_tmp1C5->f1;}}_LLCF:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1C6),p->loc);_LLD0: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C7->tag != 8)goto _LLD2;}_LLD1:
 return Cyc_Absyn_null_exp(p->loc);_LLD2: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1C8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C8->tag != 9)goto _LLD4;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;}}_LLD3:
 return Cyc_Absyn_int_exp(_tmp1C9,_tmp1CA,p->loc);_LLD4: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1CB=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1CB->tag != 10)goto _LLD6;else{_tmp1CC=_tmp1CB->f1;}}_LLD5:
 return Cyc_Absyn_char_exp(_tmp1CC,p->loc);_LLD6: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1CD=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1CD->tag != 11)goto _LLD8;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}_LLD7:
 return Cyc_Absyn_float_exp(_tmp1CE,_tmp1CF,p->loc);_LLD8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1D0->tag != 15)goto _LLDA;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;if(_tmp1D0->f3 != 0)goto _LLDA;}}_LLD9: {
# 1025
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1D1,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1D2);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLDA: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1D3->tag != 16)goto _LLDC;else{_tmp1D4=_tmp1D3->f1;}}_LLDB:
 return _tmp1D4;_LLDC:;_LLDD:
# 1030
{const char*_tmp94B;Cyc_Parse_err(((_tmp94B="cannot mix patterns and expressions in case",_tag_dyneither(_tmp94B,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC9:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple19{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple19 val;};struct _tuple20{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple21{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple23 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1D7[14]="$(sign_t,int)";
# 1082 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 14}};
# 1084
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1D8=yy1;struct _tuple5 _tmp1D9;_LLDF: if((_tmp1D8.Int_tok).tag != 1)goto _LLE1;_tmp1D9=(struct _tuple5)(_tmp1D8.Int_tok).val;_LLE0:
# 1088
 yyzzz=_tmp1D9;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLDE:;}
# 1092
return yyzzz;}
# 1094
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp94C;return((_tmp94C.Int_tok).val=yy1,(((_tmp94C.Int_tok).tag=1,_tmp94C)));}static char _tmp1DC[5]="char";
# 1083 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1DC,_tmp1DC,_tmp1DC + 5}};
# 1085
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1DD=yy1;char _tmp1DE;_LLE4: if((_tmp1DD.Char_tok).tag != 2)goto _LLE6;_tmp1DE=(char)(_tmp1DD.Char_tok).val;_LLE5:
# 1089
 yyzzz=_tmp1DE;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLE3:;}
# 1093
return yyzzz;}
# 1095
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp94D;return((_tmp94D.Char_tok).val=yy1,(((_tmp94D.Char_tok).tag=2,_tmp94D)));}static char _tmp1E1[13]="string_t<`H>";
# 1084 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1E1,_tmp1E1,_tmp1E1 + 13}};
# 1086
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1E2=yy1;struct _dyneither_ptr _tmp1E3;_LLE9: if((_tmp1E2.String_tok).tag != 3)goto _LLEB;_tmp1E3=(struct _dyneither_ptr)(_tmp1E2.String_tok).val;_LLEA:
# 1090
 yyzzz=_tmp1E3;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE8:;}
# 1094
return yyzzz;}
# 1096
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp94E;return((_tmp94E.String_tok).val=yy1,(((_tmp94E.String_tok).tag=3,_tmp94E)));}static char _tmp1E6[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 56}};
# 1089
struct _tuple20*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple20*yyzzz;
{union Cyc_YYSTYPE _tmp1E7=yy1;struct _tuple20*_tmp1E8;_LLEE: if((_tmp1E7.YY1).tag != 7)goto _LLF0;_tmp1E8=(struct _tuple20*)(_tmp1E7.YY1).val;_LLEF:
# 1093
 yyzzz=_tmp1E8;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLED:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY1(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp94F;return((_tmp94F.YY1).val=yy1,(((_tmp94F.YY1).tag=7,_tmp94F)));}static char _tmp1EB[19]="conref_t<bounds_t>";
# 1088 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1EB,_tmp1EB,_tmp1EB + 19}};
# 1090
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1EC=yy1;union Cyc_Absyn_Constraint*_tmp1ED;_LLF3: if((_tmp1EC.YY2).tag != 8)goto _LLF5;_tmp1ED=(union Cyc_Absyn_Constraint*)(_tmp1EC.YY2).val;_LLF4:
# 1094
 yyzzz=_tmp1ED;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLF2:;}
# 1098
return yyzzz;}
# 1100
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp950;return((_tmp950.YY2).val=yy1,(((_tmp950.YY2).tag=8,_tmp950)));}static char _tmp1F0[6]="exp_t";
# 1089 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1F0,_tmp1F0,_tmp1F0 + 6}};
# 1091
struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1F1=yy1;struct Cyc_Absyn_Exp*_tmp1F2;_LLF8: if((_tmp1F1.YY3).tag != 9)goto _LLFA;_tmp1F2=(struct Cyc_Absyn_Exp*)(_tmp1F1.YY3).val;_LLF9:
# 1095
 yyzzz=_tmp1F2;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF7:;}
# 1099
return yyzzz;}
# 1101
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp951;return((_tmp951.YY3).val=yy1,(((_tmp951.YY3).tag=9,_tmp951)));}static char _tmp1F5[10]="exp_opt_t";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 10}};
# 1099
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1F6=yy1;struct Cyc_Absyn_Exp*_tmp1F7;_LLFD: if((_tmp1F6.YY4).tag != 10)goto _LLFF;_tmp1F7=(struct Cyc_Absyn_Exp*)(_tmp1F6.YY4).val;_LLFE:
# 1103
 yyzzz=_tmp1F7;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLFC:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp952;return((_tmp952.YY4).val=yy1,(((_tmp952.YY4).tag=10,_tmp952)));}static char _tmp1FA[17]="list_t<exp_t,`H>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 17}};
# 1100
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FB=yy1;struct Cyc_List_List*_tmp1FC;_LL102: if((_tmp1FB.YY5).tag != 11)goto _LL104;_tmp1FC=(struct Cyc_List_List*)(_tmp1FB.YY5).val;_LL103:
# 1104
 yyzzz=_tmp1FC;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL101:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp953;return((_tmp953.YY5).val=yy1,(((_tmp953.YY5).tag=11,_tmp953)));}static char _tmp1FF[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 47}};
# 1101
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp200=yy1;struct Cyc_List_List*_tmp201;_LL107: if((_tmp200.YY6).tag != 12)goto _LL109;_tmp201=(struct Cyc_List_List*)(_tmp200.YY6).val;_LL108:
# 1105
 yyzzz=_tmp201;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL106:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp954;return((_tmp954.YY6).val=yy1,(((_tmp954.YY6).tag=12,_tmp954)));}static char _tmp204[9]="primop_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 9}};
# 1102
enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp205=yy1;enum Cyc_Absyn_Primop _tmp206;_LL10C: if((_tmp205.YY7).tag != 13)goto _LL10E;_tmp206=(enum Cyc_Absyn_Primop)(_tmp205.YY7).val;_LL10D:
# 1106
 yyzzz=_tmp206;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL10B:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp955;return((_tmp955.YY7).val=yy1,(((_tmp955.YY7).tag=13,_tmp955)));}static char _tmp209[19]="opt_t<primop_t,`H>";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp209,_tmp209,_tmp209 + 19}};
# 1103
struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp20A=yy1;struct Cyc_Core_Opt*_tmp20B;_LL111: if((_tmp20A.YY8).tag != 14)goto _LL113;_tmp20B=(struct Cyc_Core_Opt*)(_tmp20A.YY8).val;_LL112:
# 1107
 yyzzz=_tmp20B;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL110:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp956;return((_tmp956.YY8).val=yy1,(((_tmp956.YY8).tag=14,_tmp956)));}static char _tmp20E[7]="qvar_t";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 7}};
# 1104
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp20F=yy1;struct _tuple0*_tmp210;_LL116: if((_tmp20F.QualId_tok).tag != 5)goto _LL118;_tmp210=(struct _tuple0*)(_tmp20F.QualId_tok).val;_LL117:
# 1108
 yyzzz=_tmp210;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp957;return((_tmp957.QualId_tok).val=yy1,(((_tmp957.QualId_tok).tag=5,_tmp957)));}static char _tmp213[7]="stmt_t";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 7}};
# 1107
struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp214=yy1;struct Cyc_Absyn_Stmt*_tmp215;_LL11B: if((_tmp214.YY9).tag != 15)goto _LL11D;_tmp215=(struct Cyc_Absyn_Stmt*)(_tmp214.YY9).val;_LL11C:
# 1111
 yyzzz=_tmp215;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL11A:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp958;return((_tmp958.YY9).val=yy1,(((_tmp958.YY9).tag=15,_tmp958)));}static char _tmp218[27]="list_t<switch_clause_t,`H>";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp218,_tmp218,_tmp218 + 27}};
# 1111
struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp219=yy1;struct Cyc_List_List*_tmp21A;_LL120: if((_tmp219.YY10).tag != 16)goto _LL122;_tmp21A=(struct Cyc_List_List*)(_tmp219.YY10).val;_LL121:
# 1115
 yyzzz=_tmp21A;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11F:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp959;return((_tmp959.YY10).val=yy1,(((_tmp959.YY10).tag=16,_tmp959)));}static char _tmp21D[6]="pat_t";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp21D,_tmp21D,_tmp21D + 6}};
# 1112
struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp21E=yy1;struct Cyc_Absyn_Pat*_tmp21F;_LL125: if((_tmp21E.YY11).tag != 17)goto _LL127;_tmp21F=(struct Cyc_Absyn_Pat*)(_tmp21E.YY11).val;_LL126:
# 1116
 yyzzz=_tmp21F;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL124:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp95A;return((_tmp95A.YY11).val=yy1,(((_tmp95A.YY11).tag=17,_tmp95A)));}static char _tmp222[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 28}};
# 1117
struct _tuple21*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp223=yy1;struct _tuple21*_tmp224;_LL12A: if((_tmp223.YY12).tag != 18)goto _LL12C;_tmp224=(struct _tuple21*)(_tmp223.YY12).val;_LL12B:
# 1121
 yyzzz=_tmp224;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL129:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY12(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp95B;return((_tmp95B.YY12).val=yy1,(((_tmp95B.YY12).tag=18,_tmp95B)));}static char _tmp227[17]="list_t<pat_t,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 17}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp228=yy1;struct Cyc_List_List*_tmp229;_LL12F: if((_tmp228.YY13).tag != 19)goto _LL131;_tmp229=(struct Cyc_List_List*)(_tmp228.YY13).val;_LL130:
# 1122
 yyzzz=_tmp229;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL12E:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95C;return((_tmp95C.YY13).val=yy1,(((_tmp95C.YY13).tag=19,_tmp95C)));}static char _tmp22C[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp22C,_tmp22C,_tmp22C + 36}};
# 1119
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp22D=yy1;struct _tuple22*_tmp22E;_LL134: if((_tmp22D.YY14).tag != 20)goto _LL136;_tmp22E=(struct _tuple22*)(_tmp22D.YY14).val;_LL135:
# 1123
 yyzzz=_tmp22E;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL133:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp95D;return((_tmp95D.YY14).val=yy1,(((_tmp95D.YY14).tag=20,_tmp95D)));}static char _tmp231[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp231,_tmp231,_tmp231 + 47}};
# 1120
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp232=yy1;struct Cyc_List_List*_tmp233;_LL139: if((_tmp232.YY15).tag != 21)goto _LL13B;_tmp233=(struct Cyc_List_List*)(_tmp232.YY15).val;_LL13A:
# 1124
 yyzzz=_tmp233;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL138:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95E;return((_tmp95E.YY15).val=yy1,(((_tmp95E.YY15).tag=21,_tmp95E)));}static char _tmp236[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp236,_tmp236,_tmp236 + 58}};
# 1121
struct _tuple21*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp237=yy1;struct _tuple21*_tmp238;_LL13E: if((_tmp237.YY16).tag != 22)goto _LL140;_tmp238=(struct _tuple21*)(_tmp237.YY16).val;_LL13F:
# 1125
 yyzzz=_tmp238;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL13D:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY16(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp95F;return((_tmp95F.YY16).val=yy1,(((_tmp95F.YY16).tag=22,_tmp95F)));}static char _tmp23B[9]="fndecl_t";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 9}};
# 1122
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp23C=yy1;struct Cyc_Absyn_Fndecl*_tmp23D;_LL143: if((_tmp23C.YY17).tag != 23)goto _LL145;_tmp23D=(struct Cyc_Absyn_Fndecl*)(_tmp23C.YY17).val;_LL144:
# 1126
 yyzzz=_tmp23D;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL142:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp960;return((_tmp960.YY17).val=yy1,(((_tmp960.YY17).tag=23,_tmp960)));}static char _tmp240[18]="list_t<decl_t,`H>";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 18}};
# 1123
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp241=yy1;struct Cyc_List_List*_tmp242;_LL148: if((_tmp241.YY18).tag != 24)goto _LL14A;_tmp242=(struct Cyc_List_List*)(_tmp241.YY18).val;_LL149:
# 1127
 yyzzz=_tmp242;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL147:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp961;return((_tmp961.YY18).val=yy1,(((_tmp961.YY18).tag=24,_tmp961)));}static char _tmp245[12]="decl_spec_t";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp245,_tmp245,_tmp245 + 12}};
# 1125
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp246=yy1;struct Cyc_Parse_Declaration_spec _tmp247;_LL14D: if((_tmp246.YY19).tag != 25)goto _LL14F;_tmp247=(struct Cyc_Parse_Declaration_spec)(_tmp246.YY19).val;_LL14E:
# 1129
 yyzzz=_tmp247;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL14C:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp962;return((_tmp962.YY19).val=yy1,(((_tmp962.YY19).tag=25,_tmp962)));}static char _tmp24A[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp24A,_tmp24A,_tmp24A + 31}};
# 1126
struct _tuple11 Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp24B=yy1;struct _tuple11 _tmp24C;_LL152: if((_tmp24B.YY20).tag != 26)goto _LL154;_tmp24C=(struct _tuple11)(_tmp24B.YY20).val;_LL153:
# 1130
 yyzzz=_tmp24C;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL151:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY20(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp963;return((_tmp963.YY20).val=yy1,(((_tmp963.YY20).tag=26,_tmp963)));}static char _tmp24F[23]="declarator_list_t<`yy>";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 23}};
# 1127
struct _tuple12*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp250=yy1;struct _tuple12*_tmp251;_LL157: if((_tmp250.YY21).tag != 27)goto _LL159;_tmp251=(struct _tuple12*)(_tmp250.YY21).val;_LL158:
# 1131
 yyzzz=_tmp251;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL156:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY21(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp964;return((_tmp964.YY21).val=yy1,(((_tmp964.YY21).tag=27,_tmp964)));}static char _tmp254[19]="storage_class_t@`H";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp254,_tmp254,_tmp254 + 19}};
# 1128
enum Cyc_Parse_Storage_class*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp255=yy1;enum Cyc_Parse_Storage_class*_tmp256;_LL15C: if((_tmp255.YY22).tag != 28)goto _LL15E;_tmp256=(enum Cyc_Parse_Storage_class*)(_tmp255.YY22).val;_LL15D:
# 1132
 yyzzz=_tmp256;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL15B:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp965;return((_tmp965.YY22).val=yy1,(((_tmp965.YY22).tag=28,_tmp965)));}static char _tmp259[17]="type_specifier_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp259,_tmp259,_tmp259 + 17}};
# 1129
struct Cyc_Parse_Type_specifier Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp25A=yy1;struct Cyc_Parse_Type_specifier _tmp25B;_LL161: if((_tmp25A.YY23).tag != 29)goto _LL163;_tmp25B=(struct Cyc_Parse_Type_specifier)(_tmp25A.YY23).val;_LL162:
# 1133
 yyzzz=_tmp25B;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL160:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.YY23).val=yy1,(((_tmp966.YY23).tag=29,_tmp966)));}static char _tmp25E[12]="aggr_kind_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp25E,_tmp25E,_tmp25E + 12}};
# 1131
enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp25F=yy1;enum Cyc_Absyn_AggrKind _tmp260;_LL166: if((_tmp25F.YY24).tag != 30)goto _LL168;_tmp260=(enum Cyc_Absyn_AggrKind)(_tmp25F.YY24).val;_LL167:
# 1135
 yyzzz=_tmp260;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL165:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.YY24).val=yy1,(((_tmp967.YY24).tag=30,_tmp967)));}static char _tmp263[8]="tqual_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp263,_tmp263,_tmp263 + 8}};
# 1132
struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp264=yy1;struct Cyc_Absyn_Tqual _tmp265;_LL16B: if((_tmp264.YY25).tag != 31)goto _LL16D;_tmp265=(struct Cyc_Absyn_Tqual)(_tmp264.YY25).val;_LL16C:
# 1136
 yyzzz=_tmp265;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL16A:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.YY25).val=yy1,(((_tmp968.YY25).tag=31,_tmp968)));}static char _tmp268[23]="list_t<aggrfield_t,`H>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp268,_tmp268,_tmp268 + 23}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp269=yy1;struct Cyc_List_List*_tmp26A;_LL170: if((_tmp269.YY26).tag != 32)goto _LL172;_tmp26A=(struct Cyc_List_List*)(_tmp269.YY26).val;_LL171:
# 1137
 yyzzz=_tmp26A;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16F:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY26).val=yy1,(((_tmp969.YY26).tag=32,_tmp969)));}static char _tmp26D[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp26D,_tmp26D,_tmp26D + 34}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26E=yy1;struct Cyc_List_List*_tmp26F;_LL175: if((_tmp26E.YY27).tag != 33)goto _LL177;_tmp26F=(struct Cyc_List_List*)(_tmp26E.YY27).val;_LL176:
# 1138
 yyzzz=_tmp26F;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL174:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY27).val=yy1,(((_tmp96A.YY27).tag=33,_tmp96A)));}static char _tmp272[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp272,_tmp272,_tmp272 + 33}};
# 1135
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp273=yy1;struct Cyc_List_List*_tmp274;_LL17A: if((_tmp273.YY28).tag != 34)goto _LL17C;_tmp274=(struct Cyc_List_List*)(_tmp273.YY28).val;_LL17B:
# 1139
 yyzzz=_tmp274;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL179:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY28).val=yy1,(((_tmp96B.YY28).tag=34,_tmp96B)));}static char _tmp277[18]="declarator_t<`yy>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp277,_tmp277,_tmp277 + 18}};
# 1136
struct Cyc_Parse_Declarator Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp278=yy1;struct Cyc_Parse_Declarator _tmp279;_LL17F: if((_tmp278.YY29).tag != 35)goto _LL181;_tmp279=(struct Cyc_Parse_Declarator)(_tmp278.YY29).val;_LL180:
# 1140
 yyzzz=_tmp279;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL17E:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY29).val=yy1,(((_tmp96C.YY29).tag=35,_tmp96C)));}static char _tmp27C[26]="abstractdeclarator_t<`yy>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp27C,_tmp27C,_tmp27C + 26}};
# 1139
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp27D=yy1;struct Cyc_Parse_Abstractdeclarator _tmp27E;_LL184: if((_tmp27D.YY30).tag != 36)goto _LL186;_tmp27E=(struct Cyc_Parse_Abstractdeclarator)(_tmp27D.YY30).val;_LL185:
# 1143
 yyzzz=_tmp27E;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL183:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY30).val=yy1,(((_tmp96D.YY30).tag=36,_tmp96D)));}static char _tmp281[5]="bool";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp281,_tmp281,_tmp281 + 5}};
# 1140
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp282=yy1;int _tmp283;_LL189: if((_tmp282.YY31).tag != 37)goto _LL18B;_tmp283=(int)(_tmp282.YY31).val;_LL18A:
# 1144
 yyzzz=_tmp283;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL188:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY31).val=yy1,(((_tmp96E.YY31).tag=37,_tmp96E)));}static char _tmp286[8]="scope_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp286,_tmp286,_tmp286 + 8}};
# 1141
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp287=yy1;enum Cyc_Absyn_Scope _tmp288;_LL18E: if((_tmp287.YY32).tag != 38)goto _LL190;_tmp288=(enum Cyc_Absyn_Scope)(_tmp287.YY32).val;_LL18F:
# 1145
 yyzzz=_tmp288;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL18D:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY32).val=yy1,(((_tmp96F.YY32).tag=38,_tmp96F)));}static char _tmp28B[16]="datatypefield_t";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp28B,_tmp28B,_tmp28B + 16}};
# 1142
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp28C=yy1;struct Cyc_Absyn_Datatypefield*_tmp28D;_LL193: if((_tmp28C.YY33).tag != 39)goto _LL195;_tmp28D=(struct Cyc_Absyn_Datatypefield*)(_tmp28C.YY33).val;_LL194:
# 1146
 yyzzz=_tmp28D;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL192:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY33).val=yy1,(((_tmp970.YY33).tag=39,_tmp970)));}static char _tmp290[27]="list_t<datatypefield_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp290,_tmp290,_tmp290 + 27}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp291=yy1;struct Cyc_List_List*_tmp292;_LL198: if((_tmp291.YY34).tag != 40)goto _LL19A;_tmp292=(struct Cyc_List_List*)(_tmp291.YY34).val;_LL199:
# 1147
 yyzzz=_tmp292;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL197:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY34).val=yy1,(((_tmp971.YY34).tag=40,_tmp971)));}static char _tmp295[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp295,_tmp295,_tmp295 + 41}};
# 1144
struct _tuple23 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple23 yyzzz;
{union Cyc_YYSTYPE _tmp296=yy1;struct _tuple23 _tmp297;_LL19D: if((_tmp296.YY35).tag != 41)goto _LL19F;_tmp297=(struct _tuple23)(_tmp296.YY35).val;_LL19E:
# 1148
 yyzzz=_tmp297;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL19C:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY35(struct _tuple23 yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY35).val=yy1,(((_tmp972.YY35).tag=41,_tmp972)));}static char _tmp29A[17]="list_t<var_t,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp29A,_tmp29A,_tmp29A + 17}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29B=yy1;struct Cyc_List_List*_tmp29C;_LL1A2: if((_tmp29B.YY36).tag != 42)goto _LL1A4;_tmp29C=(struct Cyc_List_List*)(_tmp29B.YY36).val;_LL1A3:
# 1149
 yyzzz=_tmp29C;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A1:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY36).val=yy1,(((_tmp973.YY36).tag=42,_tmp973)));}static char _tmp29F[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp29F,_tmp29F,_tmp29F + 31}};
# 1146
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2A0=yy1;struct _tuple8*_tmp2A1;_LL1A7: if((_tmp2A0.YY37).tag != 43)goto _LL1A9;_tmp2A1=(struct _tuple8*)(_tmp2A0.YY37).val;_LL1A8:
# 1150
 yyzzz=_tmp2A1;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A6:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY37).val=yy1,(((_tmp974.YY37).tag=43,_tmp974)));}static char _tmp2A4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2A4,_tmp2A4,_tmp2A4 + 42}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A5=yy1;struct Cyc_List_List*_tmp2A6;_LL1AC: if((_tmp2A5.YY38).tag != 44)goto _LL1AE;_tmp2A6=(struct Cyc_List_List*)(_tmp2A5.YY38).val;_LL1AD:
# 1151
 yyzzz=_tmp2A6;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1AB:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY38).val=yy1,(((_tmp975.YY38).tag=44,_tmp975)));}static char _tmp2A9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A9,_tmp2A9,_tmp2A9 + 115}};
# 1148
struct _tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp2AA=yy1;struct _tuple24*_tmp2AB;_LL1B1: if((_tmp2AA.YY39).tag != 45)goto _LL1B3;_tmp2AB=(struct _tuple24*)(_tmp2AA.YY39).val;_LL1B2:
# 1152
 yyzzz=_tmp2AB;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY39).val=yy1,(((_tmp976.YY39).tag=45,_tmp976)));}static char _tmp2AE[18]="list_t<type_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2AE,_tmp2AE,_tmp2AE + 18}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AF=yy1;struct Cyc_List_List*_tmp2B0;_LL1B6: if((_tmp2AF.YY40).tag != 46)goto _LL1B8;_tmp2B0=(struct Cyc_List_List*)(_tmp2AF.YY40).val;_LL1B7:
# 1153
 yyzzz=_tmp2B0;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B5:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY40).val=yy1,(((_tmp977.YY40).tag=46,_tmp977)));}static char _tmp2B3[24]="list_t<designator_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2B3,_tmp2B3,_tmp2B3 + 24}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B4=yy1;struct Cyc_List_List*_tmp2B5;_LL1BB: if((_tmp2B4.YY41).tag != 47)goto _LL1BD;_tmp2B5=(struct Cyc_List_List*)(_tmp2B4.YY41).val;_LL1BC:
# 1155
 yyzzz=_tmp2B5;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1BA:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY41).val=yy1,(((_tmp978.YY41).tag=47,_tmp978)));}static char _tmp2B8[13]="designator_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B8,_tmp2B8,_tmp2B8 + 13}};
# 1152
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B9=yy1;void*_tmp2BA;_LL1C0: if((_tmp2B9.YY42).tag != 48)goto _LL1C2;_tmp2BA=(void*)(_tmp2B9.YY42).val;_LL1C1:
# 1156
 yyzzz=_tmp2BA;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BF:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY42).val=yy1,(((_tmp979.YY42).tag=48,_tmp979)));}static char _tmp2BD[7]="kind_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2BD,_tmp2BD,_tmp2BD + 7}};
# 1153
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2BE=yy1;struct Cyc_Absyn_Kind*_tmp2BF;_LL1C5: if((_tmp2BE.YY43).tag != 49)goto _LL1C7;_tmp2BF=(struct Cyc_Absyn_Kind*)(_tmp2BE.YY43).val;_LL1C6:
# 1157
 yyzzz=_tmp2BF;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C4:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY43).val=yy1,(((_tmp97A.YY43).tag=49,_tmp97A)));}static char _tmp2C2[7]="type_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2C2,_tmp2C2,_tmp2C2 + 7}};
# 1154
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C3=yy1;void*_tmp2C4;_LL1CA: if((_tmp2C3.YY44).tag != 50)goto _LL1CC;_tmp2C4=(void*)(_tmp2C3.YY44).val;_LL1CB:
# 1158
 yyzzz=_tmp2C4;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C9:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY44).val=yy1,(((_tmp97B.YY44).tag=50,_tmp97B)));}static char _tmp2C7[23]="list_t<attribute_t,`H>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C7,_tmp2C7,_tmp2C7 + 23}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C8=yy1;struct Cyc_List_List*_tmp2C9;_LL1CF: if((_tmp2C8.YY45).tag != 51)goto _LL1D1;_tmp2C9=(struct Cyc_List_List*)(_tmp2C8.YY45).val;_LL1D0:
# 1159
 yyzzz=_tmp2C9;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1CE:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY45).val=yy1,(((_tmp97C.YY45).tag=51,_tmp97C)));}static char _tmp2CC[12]="attribute_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2CC,_tmp2CC,_tmp2CC + 12}};
# 1156
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2CD=yy1;void*_tmp2CE;_LL1D4: if((_tmp2CD.YY46).tag != 52)goto _LL1D6;_tmp2CE=(void*)(_tmp2CD.YY46).val;_LL1D5:
# 1160
 yyzzz=_tmp2CE;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1D3:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY46).val=yy1,(((_tmp97D.YY46).tag=52,_tmp97D)));}static char _tmp2D1[12]="enumfield_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2D1,_tmp2D1,_tmp2D1 + 12}};
# 1157
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2D2=yy1;struct Cyc_Absyn_Enumfield*_tmp2D3;_LL1D9: if((_tmp2D2.YY47).tag != 53)goto _LL1DB;_tmp2D3=(struct Cyc_Absyn_Enumfield*)(_tmp2D2.YY47).val;_LL1DA:
# 1161
 yyzzz=_tmp2D3;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D8:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY47).val=yy1,(((_tmp97E.YY47).tag=53,_tmp97E)));}static char _tmp2D6[23]="list_t<enumfield_t,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D6,_tmp2D6,_tmp2D6 + 23}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D7=yy1;struct Cyc_List_List*_tmp2D8;_LL1DE: if((_tmp2D7.YY48).tag != 54)goto _LL1E0;_tmp2D8=(struct Cyc_List_List*)(_tmp2D7.YY48).val;_LL1DF:
# 1162
 yyzzz=_tmp2D8;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1DD:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY48).val=yy1,(((_tmp97F.YY48).tag=54,_tmp97F)));}static char _tmp2DB[11]="type_opt_t";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2DB,_tmp2DB,_tmp2DB + 11}};
# 1159
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2DC=yy1;void*_tmp2DD;_LL1E3: if((_tmp2DC.YY49).tag != 55)goto _LL1E5;_tmp2DD=(void*)(_tmp2DC.YY49).val;_LL1E4:
# 1163
 yyzzz=_tmp2DD;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E2:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY49).val=yy1,(((_tmp980.YY49).tag=55,_tmp980)));}static char _tmp2E0[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2E0,_tmp2E0,_tmp2E0 + 31}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E1=yy1;struct Cyc_List_List*_tmp2E2;_LL1E8: if((_tmp2E1.YY50).tag != 56)goto _LL1EA;_tmp2E2=(struct Cyc_List_List*)(_tmp2E1.YY50).val;_LL1E9:
# 1164
 yyzzz=_tmp2E2;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E7:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY50).val=yy1,(((_tmp981.YY50).tag=56,_tmp981)));}static char _tmp2E5[15]="conref_t<bool>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2E5,_tmp2E5,_tmp2E5 + 15}};
# 1161
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2E6=yy1;union Cyc_Absyn_Constraint*_tmp2E7;_LL1ED: if((_tmp2E6.YY51).tag != 57)goto _LL1EF;_tmp2E7=(union Cyc_Absyn_Constraint*)(_tmp2E6.YY51).val;_LL1EE:
# 1165
 yyzzz=_tmp2E7;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1EC:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY51).val=yy1,(((_tmp982.YY51).tag=57,_tmp982)));}static char _tmp2EA[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2EA,_tmp2EA,_tmp2EA + 45}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2EB=yy1;struct Cyc_List_List*_tmp2EC;_LL1F2: if((_tmp2EB.YY52).tag != 58)goto _LL1F4;_tmp2EC=(struct Cyc_List_List*)(_tmp2EB.YY52).val;_LL1F3:
# 1166
 yyzzz=_tmp2EC;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1F1:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY52).val=yy1,(((_tmp983.YY52).tag=58,_tmp983)));}static char _tmp2EF[20]="pointer_qual_t<`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2EF,_tmp2EF,_tmp2EF + 20}};
# 1163
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F0=yy1;void*_tmp2F1;_LL1F7: if((_tmp2F0.YY53).tag != 59)goto _LL1F9;_tmp2F1=(void*)(_tmp2F0.YY53).val;_LL1F8:
# 1167
 yyzzz=_tmp2F1;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F6:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY53).val=yy1,(((_tmp984.YY53).tag=59,_tmp984)));}static char _tmp2F4[21]="pointer_quals_t<`yy>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2F4,_tmp2F4,_tmp2F4 + 21}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F5=yy1;struct Cyc_List_List*_tmp2F6;_LL1FC: if((_tmp2F5.YY54).tag != 60)goto _LL1FE;_tmp2F6=(struct Cyc_List_List*)(_tmp2F5.YY54).val;_LL1FD:
# 1168
 yyzzz=_tmp2F6;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1FB:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY54).val=yy1,(((_tmp985.YY54).tag=60,_tmp985)));}static char _tmp2F9[21]="$(bool,string_t<`H>)";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F9,_tmp2F9,_tmp2F9 + 21}};
# 1165
struct _tuple19 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple19 yyzzz;
{union Cyc_YYSTYPE _tmp2FA=yy1;struct _tuple19 _tmp2FB;_LL201: if((_tmp2FA.Asm_tok).tag != 6)goto _LL203;_tmp2FB=(struct _tuple19)(_tmp2FA.Asm_tok).val;_LL202:
# 1169
 yyzzz=_tmp2FB;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL200:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple19 yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.Asm_tok).val=yy1,(((_tmp986.Asm_tok).tag=6,_tmp986)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1187
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1191
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp987;return(_tmp987.timestamp=0,((_tmp987.first_line=0,((_tmp987.first_column=0,((_tmp987.last_line=0,((_tmp987.last_column=0,_tmp987)))))))));}
# 1194
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1205 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp2FE[2]="$";static char _tmp2FF[6]="error";static char _tmp300[12]="$undefined.";static char _tmp301[5]="AUTO";static char _tmp302[9]="REGISTER";static char _tmp303[7]="STATIC";static char _tmp304[7]="EXTERN";static char _tmp305[8]="TYPEDEF";static char _tmp306[5]="VOID";static char _tmp307[5]="CHAR";static char _tmp308[6]="SHORT";static char _tmp309[4]="INT";static char _tmp30A[5]="LONG";static char _tmp30B[6]="FLOAT";static char _tmp30C[7]="DOUBLE";static char _tmp30D[7]="SIGNED";static char _tmp30E[9]="UNSIGNED";static char _tmp30F[6]="CONST";static char _tmp310[9]="VOLATILE";static char _tmp311[9]="RESTRICT";static char _tmp312[7]="STRUCT";static char _tmp313[6]="UNION";static char _tmp314[5]="CASE";static char _tmp315[8]="DEFAULT";static char _tmp316[7]="INLINE";static char _tmp317[7]="SIZEOF";static char _tmp318[9]="OFFSETOF";static char _tmp319[3]="IF";static char _tmp31A[5]="ELSE";static char _tmp31B[7]="SWITCH";static char _tmp31C[6]="WHILE";static char _tmp31D[3]="DO";static char _tmp31E[4]="FOR";static char _tmp31F[5]="GOTO";static char _tmp320[9]="CONTINUE";static char _tmp321[6]="BREAK";static char _tmp322[7]="RETURN";static char _tmp323[5]="ENUM";static char _tmp324[8]="NULL_kw";static char _tmp325[4]="LET";static char _tmp326[6]="THROW";static char _tmp327[4]="TRY";static char _tmp328[6]="CATCH";static char _tmp329[7]="EXPORT";static char _tmp32A[4]="NEW";static char _tmp32B[9]="ABSTRACT";static char _tmp32C[9]="FALLTHRU";static char _tmp32D[6]="USING";static char _tmp32E[10]="NAMESPACE";static char _tmp32F[9]="DATATYPE";static char _tmp330[8]="XTUNION";static char _tmp331[7]="TUNION";static char _tmp332[7]="MALLOC";static char _tmp333[8]="RMALLOC";static char _tmp334[7]="CALLOC";static char _tmp335[8]="RCALLOC";static char _tmp336[5]="SWAP";static char _tmp337[9]="REGION_T";static char _tmp338[6]="TAG_T";static char _tmp339[7]="REGION";static char _tmp33A[5]="RNEW";static char _tmp33B[8]="REGIONS";static char _tmp33C[13]="RESET_REGION";static char _tmp33D[16]="NOZEROTERM_QUAL";static char _tmp33E[14]="ZEROTERM_QUAL";static char _tmp33F[12]="REGION_QUAL";static char _tmp340[7]="PORTON";static char _tmp341[8]="PORTOFF";static char _tmp342[12]="DYNREGION_T";static char _tmp343[6]="ALIAS";static char _tmp344[8]="NUMELTS";static char _tmp345[8]="VALUEOF";static char _tmp346[10]="VALUEOF_T";static char _tmp347[9]="TAGCHECK";static char _tmp348[13]="NUMELTS_QUAL";static char _tmp349[10]="THIN_QUAL";static char _tmp34A[9]="FAT_QUAL";static char _tmp34B[13]="NOTNULL_QUAL";static char _tmp34C[14]="NULLABLE_QUAL";static char _tmp34D[12]="TAGGED_QUAL";static char _tmp34E[16]="EXTENSIBLE_QUAL";static char _tmp34F[15]="RESETABLE_QUAL";static char _tmp350[7]="PTR_OP";static char _tmp351[7]="INC_OP";static char _tmp352[7]="DEC_OP";static char _tmp353[8]="LEFT_OP";static char _tmp354[9]="RIGHT_OP";static char _tmp355[6]="LE_OP";static char _tmp356[6]="GE_OP";static char _tmp357[6]="EQ_OP";static char _tmp358[6]="NE_OP";static char _tmp359[7]="AND_OP";static char _tmp35A[6]="OR_OP";static char _tmp35B[11]="MUL_ASSIGN";static char _tmp35C[11]="DIV_ASSIGN";static char _tmp35D[11]="MOD_ASSIGN";static char _tmp35E[11]="ADD_ASSIGN";static char _tmp35F[11]="SUB_ASSIGN";static char _tmp360[12]="LEFT_ASSIGN";static char _tmp361[13]="RIGHT_ASSIGN";static char _tmp362[11]="AND_ASSIGN";static char _tmp363[11]="XOR_ASSIGN";static char _tmp364[10]="OR_ASSIGN";static char _tmp365[9]="ELLIPSIS";static char _tmp366[11]="LEFT_RIGHT";static char _tmp367[12]="COLON_COLON";static char _tmp368[11]="IDENTIFIER";static char _tmp369[17]="INTEGER_CONSTANT";static char _tmp36A[7]="STRING";static char _tmp36B[8]="WSTRING";static char _tmp36C[19]="CHARACTER_CONSTANT";static char _tmp36D[20]="WCHARACTER_CONSTANT";static char _tmp36E[18]="FLOATING_CONSTANT";static char _tmp36F[9]="TYPE_VAR";static char _tmp370[13]="TYPEDEF_NAME";static char _tmp371[16]="QUAL_IDENTIFIER";static char _tmp372[18]="QUAL_TYPEDEF_NAME";static char _tmp373[10]="ATTRIBUTE";static char _tmp374[4]="ASM";static char _tmp375[4]="';'";static char _tmp376[4]="'{'";static char _tmp377[4]="'}'";static char _tmp378[4]="','";static char _tmp379[4]="'='";static char _tmp37A[4]="'<'";static char _tmp37B[4]="'>'";static char _tmp37C[4]="'('";static char _tmp37D[4]="')'";static char _tmp37E[4]="'_'";static char _tmp37F[4]="'$'";static char _tmp380[4]="':'";static char _tmp381[4]="'.'";static char _tmp382[4]="'['";static char _tmp383[4]="']'";static char _tmp384[4]="'*'";static char _tmp385[4]="'@'";static char _tmp386[4]="'?'";static char _tmp387[4]="'+'";static char _tmp388[4]="'|'";static char _tmp389[4]="'^'";static char _tmp38A[4]="'&'";static char _tmp38B[4]="'-'";static char _tmp38C[4]="'/'";static char _tmp38D[4]="'%'";static char _tmp38E[4]="'~'";static char _tmp38F[4]="'!'";static char _tmp390[5]="prog";static char _tmp391[17]="translation_unit";static char _tmp392[12]="export_list";static char _tmp393[19]="export_list_values";static char _tmp394[21]="external_declaration";static char _tmp395[20]="function_definition";static char _tmp396[21]="function_definition2";static char _tmp397[13]="using_action";static char _tmp398[15]="unusing_action";static char _tmp399[17]="namespace_action";static char _tmp39A[19]="unnamespace_action";static char _tmp39B[12]="declaration";static char _tmp39C[19]="resetable_qual_opt";static char _tmp39D[17]="declaration_list";static char _tmp39E[23]="declaration_specifiers";static char _tmp39F[24]="storage_class_specifier";static char _tmp3A0[15]="attributes_opt";static char _tmp3A1[11]="attributes";static char _tmp3A2[15]="attribute_list";static char _tmp3A3[10]="attribute";static char _tmp3A4[15]="type_specifier";static char _tmp3A5[25]="type_specifier_notypedef";static char _tmp3A6[5]="kind";static char _tmp3A7[15]="type_qualifier";static char _tmp3A8[15]="enum_specifier";static char _tmp3A9[11]="enum_field";static char _tmp3AA[22]="enum_declaration_list";static char _tmp3AB[26]="struct_or_union_specifier";static char _tmp3AC[16]="type_params_opt";static char _tmp3AD[16]="struct_or_union";static char _tmp3AE[24]="struct_declaration_list";static char _tmp3AF[25]="struct_declaration_list0";static char _tmp3B0[21]="init_declarator_list";static char _tmp3B1[22]="init_declarator_list0";static char _tmp3B2[16]="init_declarator";static char _tmp3B3[19]="struct_declaration";static char _tmp3B4[25]="specifier_qualifier_list";static char _tmp3B5[35]="notypedef_specifier_qualifier_list";static char _tmp3B6[23]="struct_declarator_list";static char _tmp3B7[24]="struct_declarator_list0";static char _tmp3B8[18]="struct_declarator";static char _tmp3B9[19]="datatype_specifier";static char _tmp3BA[14]="qual_datatype";static char _tmp3BB[19]="datatypefield_list";static char _tmp3BC[20]="datatypefield_scope";static char _tmp3BD[14]="datatypefield";static char _tmp3BE[11]="declarator";static char _tmp3BF[23]="declarator_withtypedef";static char _tmp3C0[18]="direct_declarator";static char _tmp3C1[30]="direct_declarator_withtypedef";static char _tmp3C2[8]="pointer";static char _tmp3C3[12]="one_pointer";static char _tmp3C4[14]="pointer_quals";static char _tmp3C5[13]="pointer_qual";static char _tmp3C6[23]="pointer_null_and_bound";static char _tmp3C7[14]="pointer_bound";static char _tmp3C8[18]="zeroterm_qual_opt";static char _tmp3C9[8]="rgn_opt";static char _tmp3CA[11]="tqual_list";static char _tmp3CB[20]="parameter_type_list";static char _tmp3CC[9]="type_var";static char _tmp3CD[16]="optional_effect";static char _tmp3CE[19]="optional_rgn_order";static char _tmp3CF[10]="rgn_order";static char _tmp3D0[16]="optional_inject";static char _tmp3D1[11]="effect_set";static char _tmp3D2[14]="atomic_effect";static char _tmp3D3[11]="region_set";static char _tmp3D4[15]="parameter_list";static char _tmp3D5[22]="parameter_declaration";static char _tmp3D6[16]="identifier_list";static char _tmp3D7[17]="identifier_list0";static char _tmp3D8[12]="initializer";static char _tmp3D9[18]="array_initializer";static char _tmp3DA[17]="initializer_list";static char _tmp3DB[12]="designation";static char _tmp3DC[16]="designator_list";static char _tmp3DD[11]="designator";static char _tmp3DE[10]="type_name";static char _tmp3DF[14]="any_type_name";static char _tmp3E0[15]="type_name_list";static char _tmp3E1[20]="abstract_declarator";static char _tmp3E2[27]="direct_abstract_declarator";static char _tmp3E3[10]="statement";static char _tmp3E4[13]="open_exp_opt";static char _tmp3E5[18]="labeled_statement";static char _tmp3E6[21]="expression_statement";static char _tmp3E7[19]="compound_statement";static char _tmp3E8[16]="block_item_list";static char _tmp3E9[20]="selection_statement";static char _tmp3EA[15]="switch_clauses";static char _tmp3EB[20]="iteration_statement";static char _tmp3EC[15]="jump_statement";static char _tmp3ED[12]="exp_pattern";static char _tmp3EE[20]="conditional_pattern";static char _tmp3EF[19]="logical_or_pattern";static char _tmp3F0[20]="logical_and_pattern";static char _tmp3F1[21]="inclusive_or_pattern";static char _tmp3F2[21]="exclusive_or_pattern";static char _tmp3F3[12]="and_pattern";static char _tmp3F4[17]="equality_pattern";static char _tmp3F5[19]="relational_pattern";static char _tmp3F6[14]="shift_pattern";static char _tmp3F7[17]="additive_pattern";static char _tmp3F8[23]="multiplicative_pattern";static char _tmp3F9[13]="cast_pattern";static char _tmp3FA[14]="unary_pattern";static char _tmp3FB[16]="postfix_pattern";static char _tmp3FC[16]="primary_pattern";static char _tmp3FD[8]="pattern";static char _tmp3FE[19]="tuple_pattern_list";static char _tmp3FF[20]="tuple_pattern_list0";static char _tmp400[14]="field_pattern";static char _tmp401[19]="field_pattern_list";static char _tmp402[20]="field_pattern_list0";static char _tmp403[11]="expression";static char _tmp404[22]="assignment_expression";static char _tmp405[20]="assignment_operator";static char _tmp406[23]="conditional_expression";static char _tmp407[20]="constant_expression";static char _tmp408[22]="logical_or_expression";static char _tmp409[23]="logical_and_expression";static char _tmp40A[24]="inclusive_or_expression";static char _tmp40B[24]="exclusive_or_expression";static char _tmp40C[15]="and_expression";static char _tmp40D[20]="equality_expression";static char _tmp40E[22]="relational_expression";static char _tmp40F[17]="shift_expression";static char _tmp410[20]="additive_expression";static char _tmp411[26]="multiplicative_expression";static char _tmp412[16]="cast_expression";static char _tmp413[17]="unary_expression";static char _tmp414[15]="unary_operator";static char _tmp415[19]="postfix_expression";static char _tmp416[19]="primary_expression";static char _tmp417[25]="argument_expression_list";static char _tmp418[26]="argument_expression_list0";static char _tmp419[9]="constant";static char _tmp41A[20]="qual_opt_identifier";static char _tmp41B[17]="qual_opt_typedef";static char _tmp41C[18]="struct_union_name";static char _tmp41D[11]="field_name";static char _tmp41E[12]="right_angle";
# 1551 "parse.y"
static struct _dyneither_ptr Cyc_yytname[289]={{_tmp2FE,_tmp2FE,_tmp2FE + 2},{_tmp2FF,_tmp2FF,_tmp2FF + 6},{_tmp300,_tmp300,_tmp300 + 12},{_tmp301,_tmp301,_tmp301 + 5},{_tmp302,_tmp302,_tmp302 + 9},{_tmp303,_tmp303,_tmp303 + 7},{_tmp304,_tmp304,_tmp304 + 7},{_tmp305,_tmp305,_tmp305 + 8},{_tmp306,_tmp306,_tmp306 + 5},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 4},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,_tmp30B,_tmp30B + 6},{_tmp30C,_tmp30C,_tmp30C + 7},{_tmp30D,_tmp30D,_tmp30D + 7},{_tmp30E,_tmp30E,_tmp30E + 9},{_tmp30F,_tmp30F,_tmp30F + 6},{_tmp310,_tmp310,_tmp310 + 9},{_tmp311,_tmp311,_tmp311 + 9},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,_tmp313,_tmp313 + 6},{_tmp314,_tmp314,_tmp314 + 5},{_tmp315,_tmp315,_tmp315 + 8},{_tmp316,_tmp316,_tmp316 + 7},{_tmp317,_tmp317,_tmp317 + 7},{_tmp318,_tmp318,_tmp318 + 9},{_tmp319,_tmp319,_tmp319 + 3},{_tmp31A,_tmp31A,_tmp31A + 5},{_tmp31B,_tmp31B,_tmp31B + 7},{_tmp31C,_tmp31C,_tmp31C + 6},{_tmp31D,_tmp31D,_tmp31D + 3},{_tmp31E,_tmp31E,_tmp31E + 4},{_tmp31F,_tmp31F,_tmp31F + 5},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,_tmp321 + 6},{_tmp322,_tmp322,_tmp322 + 7},{_tmp323,_tmp323,_tmp323 + 5},{_tmp324,_tmp324,_tmp324 + 8},{_tmp325,_tmp325,_tmp325 + 4},{_tmp326,_tmp326,_tmp326 + 6},{_tmp327,_tmp327,_tmp327 + 4},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 7},{_tmp32A,_tmp32A,_tmp32A + 4},{_tmp32B,_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C,_tmp32C + 9},{_tmp32D,_tmp32D,_tmp32D + 6},{_tmp32E,_tmp32E,_tmp32E + 10},{_tmp32F,_tmp32F,_tmp32F + 9},{_tmp330,_tmp330,_tmp330 + 8},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 8},{_tmp334,_tmp334,_tmp334 + 7},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 5},{_tmp337,_tmp337,_tmp337 + 9},{_tmp338,_tmp338,_tmp338 + 6},{_tmp339,_tmp339,_tmp339 + 7},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 13},{_tmp33D,_tmp33D,_tmp33D + 16},{_tmp33E,_tmp33E,_tmp33E + 14},{_tmp33F,_tmp33F,_tmp33F + 12},{_tmp340,_tmp340,_tmp340 + 7},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 6},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 8},{_tmp346,_tmp346,_tmp346 + 10},{_tmp347,_tmp347,_tmp347 + 9},{_tmp348,_tmp348,_tmp348 + 13},{_tmp349,_tmp349,_tmp349 + 10},{_tmp34A,_tmp34A,_tmp34A + 9},{_tmp34B,_tmp34B,_tmp34B + 13},{_tmp34C,_tmp34C,_tmp34C + 14},{_tmp34D,_tmp34D,_tmp34D + 12},{_tmp34E,_tmp34E,_tmp34E + 16},{_tmp34F,_tmp34F,_tmp34F + 15},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 9},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 6},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 11},{_tmp35C,_tmp35C,_tmp35C + 11},{_tmp35D,_tmp35D,_tmp35D + 11},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 12},{_tmp361,_tmp361,_tmp361 + 13},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 10},{_tmp365,_tmp365,_tmp365 + 9},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 12},{_tmp368,_tmp368,_tmp368 + 11},{_tmp369,_tmp369,_tmp369 + 17},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 8},{_tmp36C,_tmp36C,_tmp36C + 19},{_tmp36D,_tmp36D,_tmp36D + 20},{_tmp36E,_tmp36E,_tmp36E + 18},{_tmp36F,_tmp36F,_tmp36F + 9},{_tmp370,_tmp370,_tmp370 + 13},{_tmp371,_tmp371,_tmp371 + 16},{_tmp372,_tmp372,_tmp372 + 18},{_tmp373,_tmp373,_tmp373 + 10},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 5},{_tmp391,_tmp391,_tmp391 + 17},{_tmp392,_tmp392,_tmp392 + 12},{_tmp393,_tmp393,_tmp393 + 19},{_tmp394,_tmp394,_tmp394 + 21},{_tmp395,_tmp395,_tmp395 + 20},{_tmp396,_tmp396,_tmp396 + 21},{_tmp397,_tmp397,_tmp397 + 13},{_tmp398,_tmp398,_tmp398 + 15},{_tmp399,_tmp399,_tmp399 + 17},{_tmp39A,_tmp39A,_tmp39A + 19},{_tmp39B,_tmp39B,_tmp39B + 12},{_tmp39C,_tmp39C,_tmp39C + 19},{_tmp39D,_tmp39D,_tmp39D + 17},{_tmp39E,_tmp39E,_tmp39E + 23},{_tmp39F,_tmp39F,_tmp39F + 24},{_tmp3A0,_tmp3A0,_tmp3A0 + 15},{_tmp3A1,_tmp3A1,_tmp3A1 + 11},{_tmp3A2,_tmp3A2,_tmp3A2 + 15},{_tmp3A3,_tmp3A3,_tmp3A3 + 10},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 25},{_tmp3A6,_tmp3A6,_tmp3A6 + 5},{_tmp3A7,_tmp3A7,_tmp3A7 + 15},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 11},{_tmp3AA,_tmp3AA,_tmp3AA + 22},{_tmp3AB,_tmp3AB,_tmp3AB + 26},{_tmp3AC,_tmp3AC,_tmp3AC + 16},{_tmp3AD,_tmp3AD,_tmp3AD + 16},{_tmp3AE,_tmp3AE,_tmp3AE + 24},{_tmp3AF,_tmp3AF,_tmp3AF + 25},{_tmp3B0,_tmp3B0,_tmp3B0 + 21},{_tmp3B1,_tmp3B1,_tmp3B1 + 22},{_tmp3B2,_tmp3B2,_tmp3B2 + 16},{_tmp3B3,_tmp3B3,_tmp3B3 + 19},{_tmp3B4,_tmp3B4,_tmp3B4 + 25},{_tmp3B5,_tmp3B5,_tmp3B5 + 35},{_tmp3B6,_tmp3B6,_tmp3B6 + 23},{_tmp3B7,_tmp3B7,_tmp3B7 + 24},{_tmp3B8,_tmp3B8,_tmp3B8 + 18},{_tmp3B9,_tmp3B9,_tmp3B9 + 19},{_tmp3BA,_tmp3BA,_tmp3BA + 14},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 20},{_tmp3BD,_tmp3BD,_tmp3BD + 14},{_tmp3BE,_tmp3BE,_tmp3BE + 11},{_tmp3BF,_tmp3BF,_tmp3BF + 23},{_tmp3C0,_tmp3C0,_tmp3C0 + 18},{_tmp3C1,_tmp3C1,_tmp3C1 + 30},{_tmp3C2,_tmp3C2,_tmp3C2 + 8},{_tmp3C3,_tmp3C3,_tmp3C3 + 12},{_tmp3C4,_tmp3C4,_tmp3C4 + 14},{_tmp3C5,_tmp3C5,_tmp3C5 + 13},{_tmp3C6,_tmp3C6,_tmp3C6 + 23},{_tmp3C7,_tmp3C7,_tmp3C7 + 14},{_tmp3C8,_tmp3C8,_tmp3C8 + 18},{_tmp3C9,_tmp3C9,_tmp3C9 + 8},{_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 20},{_tmp3CC,_tmp3CC,_tmp3CC + 9},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 19},{_tmp3CF,_tmp3CF,_tmp3CF + 10},{_tmp3D0,_tmp3D0,_tmp3D0 + 16},{_tmp3D1,_tmp3D1,_tmp3D1 + 11},{_tmp3D2,_tmp3D2,_tmp3D2 + 14},{_tmp3D3,_tmp3D3,_tmp3D3 + 11},{_tmp3D4,_tmp3D4,_tmp3D4 + 15},{_tmp3D5,_tmp3D5,_tmp3D5 + 22},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{_tmp3D7,_tmp3D7,_tmp3D7 + 17},{_tmp3D8,_tmp3D8,_tmp3D8 + 12},{_tmp3D9,_tmp3D9,_tmp3D9 + 18},{_tmp3DA,_tmp3DA,_tmp3DA + 17},{_tmp3DB,_tmp3DB,_tmp3DB + 12},{_tmp3DC,_tmp3DC,_tmp3DC + 16},{_tmp3DD,_tmp3DD,_tmp3DD + 11},{_tmp3DE,_tmp3DE,_tmp3DE + 10},{_tmp3DF,_tmp3DF,_tmp3DF + 14},{_tmp3E0,_tmp3E0,_tmp3E0 + 15},{_tmp3E1,_tmp3E1,_tmp3E1 + 20},{_tmp3E2,_tmp3E2,_tmp3E2 + 27},{_tmp3E3,_tmp3E3,_tmp3E3 + 10},{_tmp3E4,_tmp3E4,_tmp3E4 + 13},{_tmp3E5,_tmp3E5,_tmp3E5 + 18},{_tmp3E6,_tmp3E6,_tmp3E6 + 21},{_tmp3E7,_tmp3E7,_tmp3E7 + 19},{_tmp3E8,_tmp3E8,_tmp3E8 + 16},{_tmp3E9,_tmp3E9,_tmp3E9 + 20},{_tmp3EA,_tmp3EA,_tmp3EA + 15},{_tmp3EB,_tmp3EB,_tmp3EB + 20},{_tmp3EC,_tmp3EC,_tmp3EC + 15},{_tmp3ED,_tmp3ED,_tmp3ED + 12},{_tmp3EE,_tmp3EE,_tmp3EE + 20},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 21},{_tmp3F2,_tmp3F2,_tmp3F2 + 21},{_tmp3F3,_tmp3F3,_tmp3F3 + 12},{_tmp3F4,_tmp3F4,_tmp3F4 + 17},{_tmp3F5,_tmp3F5,_tmp3F5 + 19},{_tmp3F6,_tmp3F6,_tmp3F6 + 14},{_tmp3F7,_tmp3F7,_tmp3F7 + 17},{_tmp3F8,_tmp3F8,_tmp3F8 + 23},{_tmp3F9,_tmp3F9,_tmp3F9 + 13},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 16},{_tmp3FC,_tmp3FC,_tmp3FC + 16},{_tmp3FD,_tmp3FD,_tmp3FD + 8},{_tmp3FE,_tmp3FE,_tmp3FE + 19},{_tmp3FF,_tmp3FF,_tmp3FF + 20},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 11},{_tmp404,_tmp404,_tmp404 + 22},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,_tmp406 + 23},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 22},{_tmp409,_tmp409,_tmp409 + 23},{_tmp40A,_tmp40A,_tmp40A + 24},{_tmp40B,_tmp40B,_tmp40B + 24},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,_tmp40E + 22},{_tmp40F,_tmp40F,_tmp40F + 17},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 26},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 15},{_tmp415,_tmp415,_tmp415 + 19},{_tmp416,_tmp416,_tmp416 + 19},{_tmp417,_tmp417,_tmp417 + 25},{_tmp418,_tmp418,_tmp418 + 26},{_tmp419,_tmp419,_tmp419 + 9},{_tmp41A,_tmp41A,_tmp41A + 20},{_tmp41B,_tmp41B,_tmp41B + 17},{_tmp41C,_tmp41C,_tmp41C + 18},{_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,_tmp41E,_tmp41E + 12}};
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
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple25{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple26{struct _dyneither_ptr f1;void*f2;};static char _tmp4D4[8]="stdcall";static char _tmp4D5[6]="cdecl";static char _tmp4D6[9]="fastcall";static char _tmp4D7[9]="noreturn";static char _tmp4D8[6]="const";static char _tmp4D9[8]="aligned";static char _tmp4DA[7]="packed";static char _tmp4DB[7]="shared";static char _tmp4DC[7]="unused";static char _tmp4DD[5]="weak";static char _tmp4DE[10]="dllimport";static char _tmp4DF[10]="dllexport";static char _tmp4E0[23]="no_instrument_function";static char _tmp4E1[12]="constructor";static char _tmp4E2[11]="destructor";static char _tmp4E3[22]="no_check_memory_usage";static char _tmp4E4[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp98F(union Cyc_YYSTYPE*yylval,unsigned int*_tmp98E,unsigned int*_tmp98D,union Cyc_YYSTYPE**_tmp98C){for(*_tmp98E=0;*_tmp98E < *_tmp98D;(*_tmp98E)++){(*_tmp98C)[*_tmp98E]=*yylval;}}static void _tmp994(unsigned int*_tmp993,unsigned int*_tmp992,struct Cyc_Yyltype**_tmp991){for(*_tmp993=0;*_tmp993 < *_tmp992;(*_tmp993)++){(*_tmp991)[*_tmp993]=
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp99B(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp99A,unsigned int*_tmp999,short**_tmp997){for(*_tmp99A=0;*_tmp99A < *_tmp999;(*_tmp99A)++){(*_tmp997)[*_tmp99A]=
# 216
*_tmp99A <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp99A)): 0;}}static void _tmp9A1(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9A0,unsigned int*_tmp99F,union Cyc_YYSTYPE**_tmp99D){for(*_tmp9A0=0;*_tmp9A0 < *_tmp99F;(*_tmp9A0)++){(*_tmp99D)[*_tmp9A0]=
# 219
*_tmp9A0 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9A0)):*yylval;}}static void _tmp9A7(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9A6,unsigned int*_tmp9A5,struct Cyc_Yyltype**_tmp9A3){for(*_tmp9A6=0;*_tmp9A6 < *_tmp9A5;(*_tmp9A6)++){(*_tmp9A3)[*_tmp9A6]=
# 222
*_tmp9A6 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9A6)):
 Cyc_yynewloc();}}static void _tmpE72(unsigned int*_tmpE71,unsigned int*_tmpE70,char**_tmpE6E){for(*_tmpE71=0;*_tmpE71 < *_tmpE70;(*_tmpE71)++){(*_tmpE6E)[*_tmpE71]='\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp420=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp420;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp988;union Cyc_YYSTYPE yylval=((_tmp988.YYINITIALSVAL).val=0,(((_tmp988.YYINITIALSVAL).tag=61,_tmp988)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp98A;unsigned int _tmp989;struct _dyneither_ptr yyss=(_tmp989=200,((_tmp98A=_region_calloc(yyregion,sizeof(short),_tmp989),_tag_dyneither(_tmp98A,sizeof(short),_tmp989))));
# 158
int yyvsp_offset;
unsigned int _tmp98E;unsigned int _tmp98D;union Cyc_YYSTYPE*_tmp98C;unsigned int _tmp98B;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp98B=(unsigned int)200,((_tmp98C=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp98B)),((((_tmp98D=_tmp98B,_tmp98F(& yylval,& _tmp98E,& _tmp98D,& _tmp98C))),_tmp98C)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp993;unsigned int _tmp992;struct Cyc_Yyltype*_tmp991;unsigned int _tmp990;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp990=(unsigned int)200,((_tmp991=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp990)),((((_tmp992=_tmp990,_tmp994(& _tmp993,& _tmp992,& _tmp991))),_tmp991)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp995;Cyc_yyerror(((_tmp995="parser stack overflow",_tag_dyneither(_tmp995,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp99A;unsigned int _tmp999;struct _dyneither_ptr _tmp998;short*_tmp997;unsigned int _tmp996;struct _dyneither_ptr yyss1=(_tmp996=(unsigned int)yystacksize,((_tmp997=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp996)),((_tmp998=_tag_dyneither(_tmp997,sizeof(short),_tmp996),((((_tmp999=_tmp996,_tmp99B(& yyssp_offset,& yyss,& _tmp99A,& _tmp999,& _tmp997))),_tmp998)))))));
# 217
unsigned int _tmp9A0;unsigned int _tmp99F;struct _dyneither_ptr _tmp99E;union Cyc_YYSTYPE*_tmp99D;unsigned int _tmp99C;struct _dyneither_ptr yyvs1=
(_tmp99C=(unsigned int)yystacksize,((_tmp99D=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp99C)),((_tmp99E=_tag_dyneither(_tmp99D,sizeof(union Cyc_YYSTYPE),_tmp99C),((((_tmp99F=_tmp99C,_tmp9A1(& yyvs,& yyssp_offset,& yylval,& _tmp9A0,& _tmp99F,& _tmp99D))),_tmp99E)))))));
# 221
unsigned int _tmp9A6;unsigned int _tmp9A5;struct _dyneither_ptr _tmp9A4;struct Cyc_Yyltype*_tmp9A3;unsigned int _tmp9A2;struct _dyneither_ptr yyls1=(_tmp9A2=(unsigned int)yystacksize,((_tmp9A3=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9A2)),((_tmp9A4=_tag_dyneither(_tmp9A3,sizeof(struct Cyc_Yyltype),_tmp9A2),((((_tmp9A5=_tmp9A2,_tmp9A7(& yyssp_offset,& yyls,& _tmp9A6,& _tmp9A5,& _tmp9A3))),_tmp9A4)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
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
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 289;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6375) || Cyc_yycheck[_check_known_subscript_notnull(6376,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6376,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1029){
int _tmp42E=0;_npop_handler(0);return _tmp42E;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1030,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(515,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL205: {
# 1170 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1174
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY18(yyyyvsp[0]);
# 1177
break;}case 2: _LL206: {
# 1176 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1180
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp42F;_push_handler(& _tmp42F);{int _tmp431=0;if(setjmp(_tmp42F.handler))_tmp431=1;if(!_tmp431){
x=Cyc_yyget_YY18(yyyyvsp[0]);;_pop_handler();}else{void*_tmp430=(void*)_exn_thrown;void*_tmp433=_tmp430;void*_tmp435;_LL209: {struct Cyc_Core_Failure_exn_struct*_tmp434=(struct Cyc_Core_Failure_exn_struct*)_tmp433;if(_tmp434->tag != Cyc_Core_Failure)goto _LL20B;}_LL20A:
# 1185
 x=0;goto _LL208;_LL20B: _tmp435=_tmp433;_LL20C:(void)_throw(_tmp435);_LL208:;}};}
# 1187
{struct _handler_cons _tmp436;_push_handler(& _tmp436);{int _tmp438=0;if(setjmp(_tmp436.handler))_tmp438=1;if(!_tmp438){
y=Cyc_yyget_YY18(yyyyvsp[1]);;_pop_handler();}else{void*_tmp437=(void*)_exn_thrown;void*_tmp43A=_tmp437;void*_tmp43C;_LL20E: {struct Cyc_Core_Failure_exn_struct*_tmp43B=(struct Cyc_Core_Failure_exn_struct*)_tmp43A;if(_tmp43B->tag != Cyc_Core_Failure)goto _LL210;}_LL20F:
# 1191
 y=0;goto _LL20D;_LL210: _tmp43C=_tmp43A;_LL211:(void)_throw(_tmp43C);_LL20D:;}};}
# 1193
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1195
break;}case 3: _LL207: {
# 1194 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1198
{struct Cyc_Absyn_Decl*_tmp9B1;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9B0;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9AF;struct Cyc_List_List*_tmp9AE;yyval=Cyc_YY18(((_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE->hd=((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->r=(void*)((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF[0]=((_tmp9B0.tag=11,((_tmp9B0.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9B0.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9B0)))))),_tmp9AF)))),((_tmp9B1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9B1)))))),((_tmp9AE->tl=0,_tmp9AE)))))));}
Cyc_Lex_leave_using();
# 1201
break;}case 4: _LL212: {
# 1198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1202
{struct Cyc_Absyn_Decl*_tmp9BB;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9BA;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9B9;struct Cyc_List_List*_tmp9B8;yyval=Cyc_YY18(((_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8->hd=((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB->r=(void*)((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9[0]=((_tmp9BA.tag=11,((_tmp9BA.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9BA.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9BA)))))),_tmp9B9)))),((_tmp9BB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9BB)))))),((_tmp9B8->tl=Cyc_yyget_YY18(yyyyvsp[4]),_tmp9B8)))))));}
break;}case 5: _LL213: {
# 1201 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1205
{struct Cyc_Absyn_Decl*_tmp9CA;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9C9;struct _dyneither_ptr*_tmp9C8;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9C7;struct Cyc_List_List*_tmp9C6;yyval=Cyc_YY18(((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->hd=((_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA->r=(void*)((_tmp9C7=_cycalloc(sizeof(*_tmp9C7)),((_tmp9C7[0]=((_tmp9C9.tag=10,((_tmp9C9.f1=((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9C8)))),((_tmp9C9.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9C9)))))),_tmp9C7)))),((_tmp9CA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9CA)))))),((_tmp9C6->tl=0,_tmp9C6)))))));}
Cyc_Lex_leave_namespace();
# 1208
break;}case 6: _LL214: {
# 1205 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1210
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44A;_push_handler(& _tmp44A);{int _tmp44C=0;if(setjmp(_tmp44A.handler))_tmp44C=1;if(!_tmp44C){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp44B=(void*)_exn_thrown;void*_tmp44E=_tmp44B;void*_tmp450;_LL217: {struct Cyc_Core_Failure_exn_struct*_tmp44F=(struct Cyc_Core_Failure_exn_struct*)_tmp44E;if(_tmp44F->tag != Cyc_Core_Failure)goto _LL219;}_LL218:
{const char*_tmp9CB;nspace=((_tmp9CB="",_tag_dyneither(_tmp9CB,sizeof(char),1)));}goto _LL216;_LL219: _tmp450=_tmp44E;_LL21A:(void)_throw(_tmp450);_LL216:;}};}
# 1215
{struct _handler_cons _tmp452;_push_handler(& _tmp452);{int _tmp454=0;if(setjmp(_tmp452.handler))_tmp454=1;if(!_tmp454){x=Cyc_yyget_YY18(yyyyvsp[2]);;_pop_handler();}else{void*_tmp453=(void*)_exn_thrown;void*_tmp456=_tmp453;void*_tmp458;_LL21C: {struct Cyc_Core_Failure_exn_struct*_tmp457=(struct Cyc_Core_Failure_exn_struct*)_tmp456;if(_tmp457->tag != Cyc_Core_Failure)goto _LL21E;}_LL21D:
 x=0;goto _LL21B;_LL21E: _tmp458=_tmp456;_LL21F:(void)_throw(_tmp458);_LL21B:;}};}
# 1218
{struct _handler_cons _tmp459;_push_handler(& _tmp459);{int _tmp45B=0;if(setjmp(_tmp459.handler))_tmp45B=1;if(!_tmp45B){y=Cyc_yyget_YY18(yyyyvsp[4]);;_pop_handler();}else{void*_tmp45A=(void*)_exn_thrown;void*_tmp45D=_tmp45A;void*_tmp45F;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp45E=(struct Cyc_Core_Failure_exn_struct*)_tmp45D;if(_tmp45E->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
 y=0;goto _LL220;_LL223: _tmp45F=_tmp45D;_LL224:(void)_throw(_tmp45F);_LL220:;}};}
# 1221
{struct Cyc_Absyn_Decl*_tmp9DA;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9D9;struct _dyneither_ptr*_tmp9D8;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9D7;struct Cyc_List_List*_tmp9D6;yyval=Cyc_YY18(((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6->hd=((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->r=(void*)((_tmp9D7=_cycalloc(sizeof(*_tmp9D7)),((_tmp9D7[0]=((_tmp9D9.tag=10,((_tmp9D9.f1=((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8[0]=nspace,_tmp9D8)))),((_tmp9D9.f2=x,_tmp9D9)))))),_tmp9D7)))),((_tmp9DA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9DA)))))),((_tmp9D6->tl=y,_tmp9D6)))))));}
# 1223
break;}case 7: _LL215: {
# 1220 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1224
struct _dyneither_ptr _tmp465=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9DB;if(Cyc_strcmp((struct _dyneither_ptr)_tmp465,((_tmp9DB="C",_tag_dyneither(_tmp9DB,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmp9E5;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp9E4;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9E3;struct Cyc_List_List*_tmp9E2;yyval=Cyc_YY18(((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2->hd=((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5->r=(void*)((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=12,((_tmp9E4.f1=Cyc_yyget_YY18(yyyyvsp[3]),_tmp9E4)))),_tmp9E3)))),((_tmp9E5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E5)))))),((_tmp9E2->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmp9E2)))))));}else{
# 1228
{const char*_tmp9E6;if(Cyc_strcmp((struct _dyneither_ptr)_tmp465,((_tmp9E6="C include",_tag_dyneither(_tmp9E6,sizeof(char),10))))!= 0){
const char*_tmp9E7;Cyc_Parse_err(((_tmp9E7="expecting \"C\" or \"C include\"",_tag_dyneither(_tmp9E7,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1231
struct Cyc_Absyn_Decl*_tmp9F1;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp9F0;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9EF;struct Cyc_List_List*_tmp9EE;yyval=Cyc_YY18(((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE->hd=((_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1->r=(void*)((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9F0.tag=13,((_tmp9F0.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmp9F0.f2=0,_tmp9F0)))))),_tmp9EF)))),((_tmp9F1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F1)))))),((_tmp9EE->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmp9EE)))))));};}}
# 1234
break;}case 8: _LL225: {
# 1231 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1235
{const char*_tmp9F2;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmp9F2="C include",_tag_dyneither(_tmp9F2,sizeof(char),10))))!= 0){
const char*_tmp9F3;Cyc_Parse_err(((_tmp9F3="expecting \"C include\"",_tag_dyneither(_tmp9F3,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmp9FD;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp9FC;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9FB;struct Cyc_List_List*_tmp9FA;yyval=Cyc_YY18(((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->hd=((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD->r=(void*)((_tmp9FB=_cycalloc(sizeof(*_tmp9FB)),((_tmp9FB[0]=((_tmp9FC.tag=13,((_tmp9FC.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmp9FC.f2=exs,_tmp9FC)))))),_tmp9FB)))),((_tmp9FD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9FD)))))),((_tmp9FA->tl=Cyc_yyget_YY18(yyyyvsp[6]),_tmp9FA)))))));}
# 1240
break;};}case 9: _LL226: {
# 1237 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1241
{struct Cyc_Absyn_Decl*_tmpA00;struct Cyc_List_List*_tmp9FF;yyval=Cyc_YY18(((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->hd=((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA00->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA00)))))),((_tmp9FF->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9FF)))))));}
break;}case 10: _LL227: {
# 1239 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243
{struct Cyc_Absyn_Decl*_tmpA03;struct Cyc_List_List*_tmpA02;yyval=Cyc_YY18(((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->hd=((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA03->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA03)))))),((_tmpA02->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA02)))))));}
break;}case 11: _LL228:
# 1240 "parse.y"
 yyval=Cyc_YY18(0);
break;case 12: _LL229: {
# 1244 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1248
yyval=yyyyvsp[2];
break;}case 13: _LL22A: {
# 1245 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1249
yyval=Cyc_YY52(0);
break;}case 14: _LL22B: {
# 1249 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1253
{struct _tuple25*_tmpA06;struct Cyc_List_List*_tmpA05;yyval=Cyc_YY52(((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->hd=((_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA06->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA06->f3=0,_tmpA06)))))))),((_tmpA05->tl=0,_tmpA05)))))));}
break;}case 15: _LL22C: {
# 1250 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1254
{struct _tuple25*_tmpA09;struct Cyc_List_List*_tmpA08;yyval=Cyc_YY52(((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08->hd=((_tmpA09=_cycalloc(sizeof(*_tmpA09)),((_tmpA09->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA09->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA09->f3=0,_tmpA09)))))))),((_tmpA08->tl=0,_tmpA08)))))));}
break;}case 16: _LL22D: {
# 1252 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1256
{struct _tuple25*_tmpA0C;struct Cyc_List_List*_tmpA0B;yyval=Cyc_YY52(((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->hd=((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA0C->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0C->f3=0,_tmpA0C)))))))),((_tmpA0B->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA0B)))))));}
break;}case 17: _LL22E: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1260
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA12;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA11;struct Cyc_List_List*_tmpA10;yyval=Cyc_YY18(((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=Cyc_Absyn_new_decl((void*)((_tmpA12=_cycalloc(sizeof(*_tmpA12)),((_tmpA12[0]=((_tmpA11.tag=1,((_tmpA11.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpA11)))),_tmpA12)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA10->tl=0,_tmpA10)))))));}
break;}case 18: _LL22F: {
# 1257 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1261
yyval=yyyyvsp[0];
break;}case 19: _LL230: {
# 1258 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1262
yyval=Cyc_YY18(0);
break;}case 20: _LL231: {
# 1263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1267
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY29(yyyyvsp[0]),0,Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 21: _LL232: {
# 1265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1269
struct Cyc_Parse_Declaration_spec _tmp484=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp484,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 22: _LL233: {
# 1278 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1]),Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL234: {
# 1280 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1284
struct Cyc_Parse_Declaration_spec _tmp485=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp485,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL235: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1292
struct Cyc_Parse_Declaration_spec _tmp486=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp486,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL236: {
# 1291 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1295
struct Cyc_Parse_Declaration_spec _tmp487=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,(struct Cyc_Parse_Declaration_spec*)& _tmp487,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL237: {
# 1296 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1300
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 27: _LL238: {
# 1299 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1303
Cyc_Lex_leave_using();
break;}case 28: _LL239: {
# 1302 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1306
{struct _dyneither_ptr*_tmpA13;Cyc_Lex_enter_namespace(((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA13)))));}yyval=yyyyvsp[1];
break;}case 29: _LL23A: {
# 1305 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1309
Cyc_Lex_leave_namespace();
break;}case 30: _LL23B: {
# 1311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1315
int _tmp489=(yyyylsp[0]).first_line;
yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),0,(unsigned int)_tmp489,(unsigned int)_tmp489));
break;}case 31: _LL23C: {
# 1314 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1318
yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY21(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 32: _LL23D: {
# 1317 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1321
{struct Cyc_List_List*_tmpA14;yyval=Cyc_YY18(((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA14->tl=0,_tmpA14)))))));}
break;}case 33: _LL23E: {
# 1319 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323
struct Cyc_List_List*_tmp48B=0;
{struct Cyc_List_List*_tmp48C=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp48C != 0;_tmp48C=_tmp48C->tl){
struct _dyneither_ptr*_tmp48D=(struct _dyneither_ptr*)_tmp48C->hd;
struct _tuple0*_tmpA15;struct _tuple0*qv=(_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->f1=Cyc_Absyn_Rel_n(0),((_tmpA15->f2=_tmp48D,_tmpA15)))));
struct Cyc_Absyn_Vardecl*_tmp48E=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA16;_tmp48B=((_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16->hd=_tmp48E,((_tmpA16->tl=_tmp48B,_tmpA16))))));}}
# 1330
_tmp48B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48B);
{struct Cyc_List_List*_tmpA17;yyval=Cyc_YY18(((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17->hd=Cyc_Absyn_letv_decl(_tmp48B,(unsigned int)(yyyylsp[0]).first_line),((_tmpA17->tl=0,_tmpA17)))))));}
# 1333
break;}case 34: _LL23F: {
# 1332 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336
struct _dyneither_ptr _tmp492=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,((_tmpA18="`H",_tag_dyneither(_tmpA18,sizeof(char),3))))== 0){
const char*_tmpA1B;void*_tmpA1A;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA1A=0,Cyc_aprintf(((_tmpA1B="bad occurrence of heap region",_tag_dyneither(_tmpA1B,sizeof(char),30))),_tag_dyneither(_tmpA1A,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA1C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,((_tmpA1C="`U",_tag_dyneither(_tmpA1C,sizeof(char),3))))== 0){
const char*_tmpA1F;void*_tmpA1E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA1E=0,Cyc_aprintf(((_tmpA1F="bad occurrence of unique region",_tag_dyneither(_tmpA1F,sizeof(char),32))),_tag_dyneither(_tmpA1E,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA22;struct Cyc_Absyn_Tvar*_tmpA21;struct Cyc_Absyn_Tvar*tv=(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->name=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22[0]=_tmp492,_tmpA22)))),((_tmpA21->identity=- 1,((_tmpA21->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA21)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA25;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA24;void*t=(void*)((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA25.tag=2,((_tmpA25.f1=tv,_tmpA25)))),_tmpA24))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA2B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA2A;struct _dyneither_ptr*_tmpA28;struct _tuple0*_tmpA27;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27->f1=Cyc_Absyn_Loc_n,((_tmpA27->f2=((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA28)))),_tmpA27)))))),(void*)((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A[0]=((_tmpA2B.tag=15,((_tmpA2B.f1=(void*)t,_tmpA2B)))),_tmpA2A)))),0);
{struct Cyc_List_List*_tmpA2C;yyval=Cyc_YY18(((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA2C->tl=0,_tmpA2C)))))));}
# 1346
break;};}case 35: _LL240: {
# 1344 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348
int _tmp4A2=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_yyget_YY4(yyyyvsp[3]);
{const char*_tmpA2D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA2D="H",_tag_dyneither(_tmpA2D,sizeof(char),2))))== 0){
const char*_tmpA2E;Cyc_Parse_err(((_tmpA2E="bad occurrence of heap region `H",_tag_dyneither(_tmpA2E,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA2F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA2F="U",_tag_dyneither(_tmpA2F,sizeof(char),2))))== 0){
const char*_tmpA30;Cyc_Parse_err(((_tmpA30="bad occurrence of unique region `U",_tag_dyneither(_tmpA30,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4A2  && _tmp4A4 != 0){
const char*_tmpA31;Cyc_Parse_err(((_tmpA31="open regions cannot be @resetable",_tag_dyneither(_tmpA31,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA3E;const char*_tmpA3D;void*_tmpA3C[1];struct Cyc_String_pa_PrintArg_struct _tmpA3B;struct Cyc_Absyn_Tvar*_tmpA3A;struct Cyc_Absyn_Tvar*tv=(_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->name=((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=(struct _dyneither_ptr)((_tmpA3B.tag=0,((_tmpA3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A3),((_tmpA3C[0]=& _tmpA3B,Cyc_aprintf(((_tmpA3D="`%s",_tag_dyneither(_tmpA3D,sizeof(char),4))),_tag_dyneither(_tmpA3C,sizeof(void*),1)))))))),_tmpA3E)))),((_tmpA3A->identity=- 1,((_tmpA3A->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA3A)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA41;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA40;void*t=(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=2,((_tmpA41.f1=tv,_tmpA41)))),_tmpA40))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA46;struct _dyneither_ptr*_tmpA44;struct _tuple0*_tmpA43;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->f1=Cyc_Absyn_Loc_n,((_tmpA43->f2=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=_tmp4A3,_tmpA44)))),_tmpA43)))))),(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=15,((_tmpA47.f1=(void*)t,_tmpA47)))),_tmpA46)))),0);
# 1362
{struct Cyc_List_List*_tmpA48;yyval=Cyc_YY18(((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4A2,_tmp4A4,(unsigned int)(yyyylsp[0]).first_line),((_tmpA48->tl=0,_tmpA48)))))));}
# 1364
break;};}case 36: _LL241: {
# 1362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1366
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA50;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA4F;struct _dyneither_ptr*_tmpA4E;struct Cyc_Absyn_Tvar*_tmpA4D;struct Cyc_Absyn_Tvar*tv=(_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->name=((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA4E)))),((_tmpA4D->identity=- 1,((_tmpA4D->kind=(void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((_tmpA4F.tag=0,((_tmpA4F.f1=& Cyc_Tcutil_rk,_tmpA4F)))),_tmpA50)))),_tmpA4D)))))));
struct _dyneither_ptr*_tmpA53;struct _tuple0*_tmpA52;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->f1=Cyc_Absyn_Loc_n,((_tmpA52->f2=((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpA53)))),_tmpA52)))))),(void*)& Cyc_Absyn_VoidType_val,0);
{struct Cyc_List_List*_tmpA54;yyval=Cyc_YY18(((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyyyvsp[6]),tv,vd,(unsigned int)(yyyylsp[0]).first_line),((_tmpA54->tl=0,_tmpA54)))))));}
# 1370
break;}case 37: _LL242:
# 1369 "parse.y"
 yyval=Cyc_YY31(0);
break;case 38: _LL243: {
# 1370 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1374
yyval=Cyc_YY31(1);
break;}case 39: _LL244: {
# 1375 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1379
yyval=yyyyvsp[0];
break;}case 40: _LL245: {
# 1377 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1381
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1])));
break;}case 41: _LL246: {
# 1383 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1387
{struct Cyc_Parse_Declaration_spec _tmpA55;yyval=Cyc_YY19(((_tmpA55.sc=(enum Cyc_Parse_Storage_class*)Cyc_yyget_YY22(yyyyvsp[0]),((_tmpA55.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA55.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA55.is_inline=0,((_tmpA55.attributes=0,_tmpA55)))))))))));}
break;}case 42: _LL247: {
# 1386 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1390
struct Cyc_Parse_Declaration_spec _tmp4BE=Cyc_yyget_YY19(yyyyvsp[1]);
if(_tmp4BE.sc != 0){
const char*_tmpA58;void*_tmpA57;(_tmpA57=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA58="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA58,sizeof(char),51))),_tag_dyneither(_tmpA57,sizeof(void*),0)));}
# 1394
{struct Cyc_Parse_Declaration_spec _tmpA59;yyval=Cyc_YY19(((_tmpA59.sc=(enum Cyc_Parse_Storage_class*)Cyc_yyget_YY22(yyyyvsp[0]),((_tmpA59.tq=_tmp4BE.tq,((_tmpA59.type_specs=_tmp4BE.type_specs,((_tmpA59.is_inline=_tmp4BE.is_inline,((_tmpA59.attributes=_tmp4BE.attributes,_tmpA59)))))))))));}
# 1398
break;}case 43: _LL248: {
# 1395 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1399
{struct Cyc_Parse_Declaration_spec _tmpA5A;yyval=Cyc_YY19(((_tmpA5A.sc=0,((_tmpA5A.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA5A.type_specs=
Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA5A.is_inline=0,((_tmpA5A.attributes=0,_tmpA5A)))))))))));}
break;}case 44: _LL249: {
# 1398 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1402
struct Cyc_Parse_Declaration_spec _tmp4C3=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA5B;yyval=Cyc_YY19(((_tmpA5B.sc=_tmp4C3.sc,((_tmpA5B.tq=_tmp4C3.tq,((_tmpA5B.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4C3.type_specs,Cyc_yyget_YY23(yyyyvsp[0])),((_tmpA5B.is_inline=_tmp4C3.is_inline,((_tmpA5B.attributes=_tmp4C3.attributes,_tmpA5B)))))))))));}
# 1408
break;}case 45: _LL24A: {
# 1405 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1409
{struct Cyc_Parse_Declaration_spec _tmpA5C;yyval=Cyc_YY19(((_tmpA5C.sc=0,((_tmpA5C.tq=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpA5C.type_specs=Cyc_Parse_empty_spec(0),((_tmpA5C.is_inline=0,((_tmpA5C.attributes=0,_tmpA5C)))))))))));}
break;}case 46: _LL24B: {
# 1407 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1411
struct Cyc_Parse_Declaration_spec _tmp4C6=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA5D;yyval=Cyc_YY19(((_tmpA5D.sc=_tmp4C6.sc,((_tmpA5D.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp4C6.tq),((_tmpA5D.type_specs=_tmp4C6.type_specs,((_tmpA5D.is_inline=_tmp4C6.is_inline,((_tmpA5D.attributes=_tmp4C6.attributes,_tmpA5D)))))))))));}
# 1416
break;}case 47: _LL24C: {
# 1413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1417
{struct Cyc_Parse_Declaration_spec _tmpA5E;yyval=Cyc_YY19(((_tmpA5E.sc=0,((_tmpA5E.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA5E.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA5E.is_inline=1,((_tmpA5E.attributes=0,_tmpA5E)))))))))));}
break;}case 48: _LL24D: {
# 1416 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1420
struct Cyc_Parse_Declaration_spec _tmp4C9=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA5F;yyval=Cyc_YY19(((_tmpA5F.sc=_tmp4C9.sc,((_tmpA5F.tq=_tmp4C9.tq,((_tmpA5F.type_specs=_tmp4C9.type_specs,((_tmpA5F.is_inline=1,((_tmpA5F.attributes=_tmp4C9.attributes,_tmpA5F)))))))))));}
# 1424
break;}case 49: _LL24E: {
# 1421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1425
{struct Cyc_Parse_Declaration_spec _tmpA60;yyval=Cyc_YY19(((_tmpA60.sc=0,((_tmpA60.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA60.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA60.is_inline=0,((_tmpA60.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpA60)))))))))));}
break;}case 50: _LL24F: {
# 1424 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1428
struct Cyc_Parse_Declaration_spec _tmp4CC=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA61;yyval=Cyc_YY19(((_tmpA61.sc=_tmp4CC.sc,((_tmpA61.tq=_tmp4CC.tq,((_tmpA61.type_specs=_tmp4CC.type_specs,((_tmpA61.is_inline=_tmp4CC.is_inline,((_tmpA61.attributes=
# 1431
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4CC.attributes),_tmpA61)))))))))));}
break;}case 51: _LL250: {
# 1431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1435
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY22(& auto_sc);
break;}case 52: _LL251: {
# 1433 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1437
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY22(& register_sc);
break;}case 53: _LL252: {
# 1435 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1439
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY22(& static_sc);
break;}case 54: _LL253: {
# 1437 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1441
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY22(& extern_sc);
break;}case 55: _LL254: {
# 1440 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1444
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA62;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA62="C",_tag_dyneither(_tmpA62,sizeof(char),2))))!= 0){
const char*_tmpA63;Cyc_Parse_err(((_tmpA63="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA63,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY22(& externC_sc);
# 1449
break;}case 56: _LL255: {
# 1445 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY22(& typedef_sc);
break;}case 57: _LL256: {
# 1448 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1452
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY22(& abstract_sc);
break;}case 58: _LL257:
# 1454 "parse.y"
 yyval=Cyc_YY45(0);
break;case 59: _LL258: {
# 1455 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1459
yyval=yyyyvsp[0];
break;}case 60: _LL259: {
# 1460 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1464
yyval=yyyyvsp[3];
break;}case 61: _LL25A: {
# 1464 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1468
{struct Cyc_List_List*_tmpA64;yyval=Cyc_YY45(((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpA64->tl=0,_tmpA64)))))));}
break;}case 62: _LL25B: {
# 1465 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1469
{struct Cyc_List_List*_tmpA65;yyval=Cyc_YY45(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpA65->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpA65)))))));}
break;}case 63: _LL25C: {
# 1470 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1474
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple26 att_map[17]={{{_tmp4D4,_tmp4D4,_tmp4D4 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4D5,_tmp4D5,_tmp4D5 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4D6,_tmp4D6,_tmp4D6 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4D7,_tmp4D7,_tmp4D7 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4D8,_tmp4D8,_tmp4D8 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4D9,_tmp4D9,_tmp4D9 + 8},(void*)& att_aligned},{{_tmp4DA,_tmp4DA,_tmp4DA + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4DB,_tmp4DB,_tmp4DB + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4DC,_tmp4DC,_tmp4DC + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4DD,_tmp4DD,_tmp4DD + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4DE,_tmp4DE,_tmp4DE + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4DF,_tmp4DF,_tmp4DF + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4E0,_tmp4E0,_tmp4E0 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1494
struct _dyneither_ptr _tmp4D2=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1496
if((((_get_dyneither_size(_tmp4D2,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp4D2,sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D2,sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D2,sizeof(char),(int)(_get_dyneither_size(_tmp4D2,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D2,sizeof(char),(int)(_get_dyneither_size(_tmp4D2,sizeof(char))- 3)))== '_')
# 1498
_tmp4D2=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4D2,2,_get_dyneither_size(_tmp4D2,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4D2,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1505
if(i == 17){
{const char*_tmpA66;Cyc_Parse_err(((_tmpA66="unrecognized attribute",_tag_dyneither(_tmpA66,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1510
break;};}case 64: _LL25D: {
# 1506 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1510
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL25E: {
# 1508 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1512
struct _dyneither_ptr _tmp4E6=Cyc_yyget_String_tok(yyyyvsp[0]);
int _tmp4E8;struct _tuple5 _tmp4E7=Cyc_yyget_Int_tok(yyyyvsp[2]);_tmp4E8=_tmp4E7.f2;{
void*a;
{const char*_tmpA68;const char*_tmpA67;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA68="regparm",_tag_dyneither(_tmpA68,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA67="__regparm__",_tag_dyneither(_tmpA67,sizeof(char),12))))== 0){
if(_tmp4E8 < 0  || _tmp4E8 > 3){
const char*_tmpA69;Cyc_Parse_err(((_tmpA69="regparm requires value between 0 and 3",_tag_dyneither(_tmpA69,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA6C;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA6B;a=(void*)((_tmpA6B=_cycalloc_atomic(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=0,((_tmpA6C.f1=_tmp4E8,_tmpA6C)))),_tmpA6B))));};}else{
const char*_tmpA6E;const char*_tmpA6D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA6E="aligned",_tag_dyneither(_tmpA6E,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA6D="__aligned__",_tag_dyneither(_tmpA6D,sizeof(char),12))))== 0){
if(_tmp4E8 < 0){const char*_tmpA6F;Cyc_Parse_err(((_tmpA6F="aligned requires positive power of two",_tag_dyneither(_tmpA6F,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4E8;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA70;Cyc_Parse_err(((_tmpA70="aligned requires positive power of two",_tag_dyneither(_tmpA70,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA73;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA72;a=(void*)((_tmpA72=_cycalloc_atomic(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA73.tag=6,((_tmpA73.f1=_tmp4E8,_tmpA73)))),_tmpA72))));};};}else{
const char*_tmpA75;const char*_tmpA74;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA75="initializes",_tag_dyneither(_tmpA75,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA74="__initializes__",_tag_dyneither(_tmpA74,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA78;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA77;a=(void*)((_tmpA77=_cycalloc_atomic(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=20,((_tmpA78.f1=_tmp4E8,_tmpA78)))),_tmpA77))));}else{
const char*_tmpA7A;const char*_tmpA79;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA7A="noliveunique",_tag_dyneither(_tmpA7A,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA79="__noliveunique__",_tag_dyneither(_tmpA79,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA7D;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA7C;a=(void*)((_tmpA7C=_cycalloc_atomic(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=21,((_tmpA7D.f1=_tmp4E8,_tmpA7D)))),_tmpA7C))));}else{
const char*_tmpA7F;const char*_tmpA7E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA7F="noconsume",_tag_dyneither(_tmpA7F,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E6,((_tmpA7E="__noconsume__",_tag_dyneither(_tmpA7E,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA82;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA81;a=(void*)((_tmpA81=_cycalloc_atomic(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=22,((_tmpA82.f1=_tmp4E8,_tmpA82)))),_tmpA81))));}else{
# 1534
{const char*_tmpA83;Cyc_Parse_err(((_tmpA83="unrecognized attribute",_tag_dyneither(_tmpA83,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp501=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp502=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpA85;const char*_tmpA84;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp501,((_tmpA85="section",_tag_dyneither(_tmpA85,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp501,((_tmpA84="__section__",_tag_dyneither(_tmpA84,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA88;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpA87;a=(void*)((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=8,((_tmpA88.f1=_tmp502,_tmpA88)))),_tmpA87))));}else{
# 1546
{const char*_tmpA89;Cyc_Parse_err(((_tmpA89="unrecognized attribute",_tag_dyneither(_tmpA89,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp508=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp509=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1556
{const char*_tmpA8A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp508,((_tmpA8A="__mode__",_tag_dyneither(_tmpA8A,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA8D;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpA8C;a=(void*)((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8D.tag=24,((_tmpA8D.f1=_tmp509,_tmpA8D)))),_tmpA8C))));}else{
# 1559
{const char*_tmpA8E;Cyc_Parse_err(((_tmpA8E="unrecognized attribute",_tag_dyneither(_tmpA8E,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50F=Cyc_yyget_String_tok(yyyyvsp[2]);
int _tmp511;struct _tuple5 _tmp510=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp511=_tmp510.f2;{
int _tmp513;struct _tuple5 _tmp512=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp513=_tmp512.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpA90;const char*_tmpA8F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpA90="format",_tag_dyneither(_tmpA90,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpA8F="__format__",_tag_dyneither(_tmpA8F,sizeof(char),11))))== 0){
const char*_tmpA91;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpA91="printf",_tag_dyneither(_tmpA91,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA94;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA93;a=(void*)((_tmpA93=_cycalloc_atomic(sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=19,((_tmpA94.f1=Cyc_Absyn_Printf_ft,((_tmpA94.f2=_tmp511,((_tmpA94.f3=_tmp513,_tmpA94)))))))),_tmpA93))));}else{
const char*_tmpA95;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpA95="scanf",_tag_dyneither(_tmpA95,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA98;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA97;a=(void*)((_tmpA97=_cycalloc_atomic(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=19,((_tmpA98.f1=Cyc_Absyn_Scanf_ft,((_tmpA98.f2=_tmp511,((_tmpA98.f3=_tmp513,_tmpA98)))))))),_tmpA97))));}else{
# 1576
const char*_tmpA99;Cyc_Parse_err(((_tmpA99="unrecognized format type",_tag_dyneither(_tmpA99,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1578
const char*_tmpA9A;Cyc_Parse_err(((_tmpA9A="unrecognized attribute",_tag_dyneither(_tmpA9A,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1581
break;};};}case 69: _LL262: {
# 1586 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1590
yyval=yyyyvsp[0];
break;}case 70: _LL263: {
# 1588 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1592
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA9D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA9C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=17,((_tmpA9D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA9D.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpA9D.f3=0,((_tmpA9D.f4=(void*)0,_tmpA9D)))))))))),_tmpA9C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 71: _LL264: {
# 1592 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1596
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 72: _LL265: {
# 1593 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1597
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL266: {
# 1595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1599
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL267: {
# 1596 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1600
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL268: {
# 1597 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1601
yyval=Cyc_YY23(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL269: {
# 1598 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1602
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL26A: {
# 1599 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1603
yyval=Cyc_YY23(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL26B: {
# 1600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL26C: {
# 1601 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL26D: {
# 1602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606
yyval=Cyc_YY23(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL26E: {
# 1603 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607
yyval=Cyc_YY23(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL26F: {
# 1604 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608
yyval=yyyyvsp[0];
break;}case 83: _LL270: {
# 1605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609
yyval=yyyyvsp[0];
break;}case 84: _LL271: {
# 1607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611
yyval=yyyyvsp[0];
break;}case 85: _LL272: {
# 1609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 86: _LL273: {
# 1612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1616
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAA0;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA9F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=10,((_tmpAA0.f1=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpAA0)))),_tmpA9F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1619
break;}case 87: _LL274: {
# 1616 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1620
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA3;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA2;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA3.tag=15,((_tmpAA3.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpAA3)))),_tmpAA2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL275: {
# 1618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA5;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=15,((_tmpAA6.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpAA6)))),_tmpAA5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1624
break;}case 89: _LL276: {
# 1621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1625
void*_tmp526=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAA9;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAA8;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=16,((_tmpAA9.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAA9.f2=(void*)_tmp526,_tmpAA9)))))),_tmpAA8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1628
break;}case 90: _LL277: {
# 1625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1629
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAAC;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAAB;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAC.tag=16,((_tmpAAC.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAAC.f2=(void*)Cyc_yyget_YY44(yyyyvsp[4]),_tmpAAC)))))),_tmpAAB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL278: {
# 1627 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1631
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAAF;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAAE;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=19,((_tmpAAF.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpAAF)))),_tmpAAE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL279: {
# 1629 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1633
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAB2;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAB1;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=19,((_tmpAB2.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpAB2)))),_tmpAB1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL27A: {
# 1631 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1635
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAB5;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAB4;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=18,((_tmpAB5.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpAB5)))),_tmpAB4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL27B: {
# 1636 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1640
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 95: _LL27C: {
# 1640 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAB6;yyval=Cyc_YY25(((_tmpAB6.print_const=1,((_tmpAB6.q_volatile=0,((_tmpAB6.q_restrict=0,((_tmpAB6.real_const=1,((_tmpAB6.loc=loc,_tmpAB6)))))))))));}
break;}case 96: _LL27D: {
# 1642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1646
{struct Cyc_Absyn_Tqual _tmpAB7;yyval=Cyc_YY25(((_tmpAB7.print_const=0,((_tmpAB7.q_volatile=1,((_tmpAB7.q_restrict=0,((_tmpAB7.real_const=0,((_tmpAB7.loc=0,_tmpAB7)))))))))));}
break;}case 97: _LL27E: {
# 1643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1647
{struct Cyc_Absyn_Tqual _tmpAB8;yyval=Cyc_YY25(((_tmpAB8.print_const=0,((_tmpAB8.q_volatile=0,((_tmpAB8.q_restrict=1,((_tmpAB8.real_const=0,((_tmpAB8.loc=0,_tmpAB8)))))))))));}
break;}case 98: _LL27F: {
# 1649 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1653
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAC7;struct Cyc_Absyn_Enumdecl*_tmpAC6;struct Cyc_Core_Opt*_tmpAC5;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAC4;struct Cyc_Absyn_TypeDecl*_tmpAC3;struct Cyc_Absyn_TypeDecl*_tmp534=(_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->r=(void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC4.tag=1,((_tmpAC4.f1=((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->sc=Cyc_Absyn_Public,((_tmpAC6->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAC6->fields=((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpAC5)))),_tmpAC6)))))))),_tmpAC4)))),_tmpAC7)))),((_tmpAC3->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAC3)))));
# 1655
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpACA;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAC9;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=26,((_tmpACA.f1=_tmp534,((_tmpACA.f2=0,_tmpACA)))))),_tmpAC9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1657
break;}case 99: _LL280: {
# 1654 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1658
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpACD;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpACC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpACD.tag=13,((_tmpACD.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpACD.f2=0,_tmpACD)))))),_tmpACC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL281: {
# 1656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1660
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAD0;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpACF;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=14,((_tmpAD0.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAD0)))),_tmpACF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL282: {
# 1662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1666
{struct Cyc_Absyn_Enumfield*_tmpAD1;yyval=Cyc_YY47(((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAD1->tag=0,((_tmpAD1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAD1)))))))));}
break;}case 102: _LL283: {
# 1664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1668
{struct Cyc_Absyn_Enumfield*_tmpAD2;yyval=Cyc_YY47(((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAD2->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpAD2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAD2)))))))));}
break;}case 103: _LL284: {
# 1668 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1672
{struct Cyc_List_List*_tmpAD3;yyval=Cyc_YY48(((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpAD3->tl=0,_tmpAD3)))))));}
break;}case 104: _LL285: {
# 1669 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1673
{struct Cyc_List_List*_tmpAD4;yyval=Cyc_YY48(((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpAD4->tl=0,_tmpAD4)))))));}
break;}case 105: _LL286: {
# 1670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1674
{struct Cyc_List_List*_tmpAD5;yyval=Cyc_YY48(((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpAD5->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAD5)))))));}
break;}case 106: _LL287: {
# 1676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1680
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAD8;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAD7;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=((_tmpAD8.tag=12,((_tmpAD8.f1=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpAD8.f2=Cyc_yyget_YY26(yyyyvsp[2]),_tmpAD8)))))),_tmpAD7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 107: _LL288: {
# 1681 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1686
struct Cyc_List_List*_tmp547=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp548=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp549=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp547,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp548,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY26(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1691
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpADB;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpADA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA[0]=((_tmpADB.tag=26,((_tmpADB.f1=_tmp549,((_tmpADB.f2=0,_tmpADB)))))),_tmpADA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1693
break;}case 108: _LL289: {
# 1692 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1697
struct Cyc_List_List*_tmp54C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp54D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp54E=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp54C,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp54D,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY26(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1702
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpADE;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpADD;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=26,((_tmpADE.f1=_tmp54E,((_tmpADE.f2=0,_tmpADE)))))),_tmpADD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1704
break;}case 109: _LL28A: {
# 1701 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1705
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAE8;struct Cyc_Core_Opt*_tmpAE7;struct Cyc_Absyn_AggrInfo _tmpAE6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAE5;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=((_tmpAE8.tag=11,((_tmpAE8.f1=((_tmpAE6.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpAE7=_cycalloc_atomic(sizeof(*_tmpAE7)),((_tmpAE7->v=(void*)1,_tmpAE7))))),((_tmpAE6.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpAE6)))),_tmpAE8)))),_tmpAE5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1708
break;}case 110: _LL28B: {
# 1705 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1709
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_AggrInfo _tmpAED;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAEC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAEE.tag=11,((_tmpAEE.f1=((_tmpAED.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpAED.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpAED)))),_tmpAEE)))),_tmpAEC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1712
break;}case 111: _LL28C:
# 1712 "parse.y"
 yyval=Cyc_YY40(0);
break;case 112: _LL28D: {
# 1714 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1718
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 113: _LL28E: {
# 1718 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1722
yyval=Cyc_YY24(Cyc_Absyn_StructA);
break;}case 114: _LL28F: {
# 1719 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1723
yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;}case 115: _LL290:
# 1724 "parse.y"
 yyval=Cyc_YY26(0);
break;case 116: _LL291: {
# 1726 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1730
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp558=Cyc_yyget_YY27(yyyyvsp[0]);for(0;_tmp558 != 0;_tmp558=_tmp558->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp558->hd,decls);}}{
# 1734
struct Cyc_List_List*_tmp559=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp559 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp559,decls);
yyval=Cyc_YY26(decls);
# 1739
break;};}case 117: _LL292: {
# 1740 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1744
{struct Cyc_List_List*_tmpAEF;yyval=Cyc_YY27(((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpAEF->tl=0,_tmpAEF)))))));}
break;}case 118: _LL293: {
# 1742 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1746
{struct Cyc_List_List*_tmpAF0;yyval=Cyc_YY27(((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->hd=Cyc_yyget_YY26(yyyyvsp[1]),((_tmpAF0->tl=Cyc_yyget_YY27(yyyyvsp[0]),_tmpAF0)))))));}
break;}case 119: _LL294: {
# 1746 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1750
yyval=Cyc_YY21(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
break;}case 120: _LL295: {
# 1752 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1756
{struct _tuple12*_tmpAF1;yyval=Cyc_YY21(((_tmpAF1=_region_malloc(yyr,sizeof(*_tmpAF1)),((_tmpAF1->tl=0,((_tmpAF1->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpAF1)))))));}
break;}case 121: _LL296: {
# 1754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1758
{struct _tuple12*_tmpAF2;yyval=Cyc_YY21(((_tmpAF2=_region_malloc(yyr,sizeof(*_tmpAF2)),((_tmpAF2->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAF2->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpAF2)))))));}
break;}case 122: _LL297: {
# 1759 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1763
{struct _tuple11 _tmpAF3;yyval=Cyc_YY20(((_tmpAF3.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpAF3.f2=0,_tmpAF3)))));}
break;}case 123: _LL298: {
# 1761 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1765
{struct _tuple11 _tmpAF4;yyval=Cyc_YY20(((_tmpAF4.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpAF4.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpAF4)))));}
break;}case 124: _LL299: {
# 1766 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1778 "parse.y"
struct _RegionHandle _tmp560=_new_region("temp");struct _RegionHandle*temp=& _tmp560;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp562;struct Cyc_Parse_Type_specifier _tmp563;struct Cyc_List_List*_tmp564;struct _tuple23 _tmp561=Cyc_yyget_YY35(yyyyvsp[0]);_tmp562=_tmp561.f1;_tmp563=_tmp561.f2;_tmp564=_tmp561.f3;
if(_tmp562.loc == 0)_tmp562.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths=0;
Cyc_Parse_decl_split(temp,Cyc_yyget_YY21(yyyyvsp[1]),& decls,& widths);{
void*_tmp565=Cyc_Parse_speclist2typ(_tmp563,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp562,_tmp565,decls,_tmp564),widths);
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple16*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp566));};};}
# 1790
_npop_handler(0);break;
# 1778
;_pop_region(temp);}case 125: _LL29A: {
# 1794 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1798
{struct _tuple23 _tmpAF5;yyval=Cyc_YY35(((_tmpAF5.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF5.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAF5.f3=0,_tmpAF5)))))));}
break;}case 126: _LL29B: {
# 1796 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1800
struct _tuple23 _tmp568=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpAF6;yyval=Cyc_YY35(((_tmpAF6.f1=_tmp568.f1,((_tmpAF6.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp568.f2),((_tmpAF6.f3=_tmp568.f3,_tmpAF6)))))));}
break;}case 127: _LL29C: {
# 1798 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1802
{struct _tuple23 _tmpAF7;yyval=Cyc_YY35(((_tmpAF7.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAF7.f2=Cyc_Parse_empty_spec(0),((_tmpAF7.f3=0,_tmpAF7)))))));}
break;}case 128: _LL29D: {
# 1800 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1804
struct _tuple23 _tmp56B=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple23 _tmpAF8;yyval=Cyc_YY35(((_tmpAF8.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp56B.f1),((_tmpAF8.f2=_tmp56B.f2,((_tmpAF8.f3=_tmp56B.f3,_tmpAF8)))))));}
break;}case 129: _LL29E: {
# 1803 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1807
{struct _tuple23 _tmpAF9;yyval=Cyc_YY35(((_tmpAF9.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF9.f2=Cyc_Parse_empty_spec(0),((_tmpAF9.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAF9)))))));}
break;}case 130: _LL29F: {
# 1805 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1809
struct _tuple23 _tmp56E=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpAFA;yyval=Cyc_YY35(((_tmpAFA.f1=_tmp56E.f1,((_tmpAFA.f2=_tmp56E.f2,((_tmpAFA.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp56E.f3),_tmpAFA)))))));}
break;}case 131: _LL2A0: {
# 1811 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1815
{struct _tuple23 _tmpAFB;yyval=Cyc_YY35(((_tmpAFB.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFB.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAFB.f3=0,_tmpAFB)))))));}
break;}case 132: _LL2A1: {
# 1813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1817
struct _tuple23 _tmp571=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpAFC;yyval=Cyc_YY35(((_tmpAFC.f1=_tmp571.f1,((_tmpAFC.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp571.f2),((_tmpAFC.f3=_tmp571.f3,_tmpAFC)))))));}
break;}case 133: _LL2A2: {
# 1815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1819
{struct _tuple23 _tmpAFD;yyval=Cyc_YY35(((_tmpAFD.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAFD.f2=Cyc_Parse_empty_spec(0),((_tmpAFD.f3=0,_tmpAFD)))))));}
break;}case 134: _LL2A3: {
# 1817 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1821
struct _tuple23 _tmp574=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple23 _tmpAFE;yyval=Cyc_YY35(((_tmpAFE.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp574.f1),((_tmpAFE.f2=_tmp574.f2,((_tmpAFE.f3=_tmp574.f3,_tmpAFE)))))));}
break;}case 135: _LL2A4: {
# 1820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1824
{struct _tuple23 _tmpAFF;yyval=Cyc_YY35(((_tmpAFF.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFF.f2=Cyc_Parse_empty_spec(0),((_tmpAFF.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAFF)))))));}
break;}case 136: _LL2A5: {
# 1822 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1826
struct _tuple23 _tmp577=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpB00;yyval=Cyc_YY35(((_tmpB00.f1=_tmp577.f1,((_tmpB00.f2=_tmp577.f2,((_tmpB00.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp577.f3),_tmpB00)))))));}
break;}case 137: _LL2A6: {
# 1826 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1830
yyval=Cyc_YY21(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
break;}case 138: _LL2A7: {
# 1832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1836
{struct _tuple12*_tmpB01;yyval=Cyc_YY21(((_tmpB01=_region_malloc(yyr,sizeof(*_tmpB01)),((_tmpB01->tl=0,((_tmpB01->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpB01)))))));}
break;}case 139: _LL2A8: {
# 1834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1838
{struct _tuple12*_tmpB02;yyval=Cyc_YY21(((_tmpB02=_region_malloc(yyr,sizeof(*_tmpB02)),((_tmpB02->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB02->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpB02)))))));}
break;}case 140: _LL2A9: {
# 1839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1843
{struct _tuple11 _tmpB03;yyval=Cyc_YY20(((_tmpB03.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB03.f2=0,_tmpB03)))));}
break;}case 141: _LL2AA: {
# 1841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1847
{struct Cyc_Parse_Declarator _tmpB09;struct _tuple0*_tmpB08;struct _tuple11 _tmpB07;yyval=Cyc_YY20(((_tmpB07.f1=((_tmpB09.id=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->f1=Cyc_Absyn_Rel_n(0),((_tmpB08->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB08)))))),((_tmpB09.tms=0,_tmpB09)))),((_tmpB07.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),_tmpB07)))));}
# 1849
break;}case 142: _LL2AB:
# 1848 "parse.y"
{struct Cyc_Parse_Declarator _tmpB0F;struct _tuple0*_tmpB0E;struct _tuple11 _tmpB0D;yyval=Cyc_YY20(((_tmpB0D.f1=((_tmpB0F.id=((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->f1=Cyc_Absyn_Rel_n(0),((_tmpB0E->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB0E)))))),((_tmpB0F.tms=0,_tmpB0F)))),((_tmpB0D.f2=0,_tmpB0D)))));}
# 1850
break;case 143: _LL2AC: {
# 1851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1855
{struct _tuple11 _tmpB10;yyval=Cyc_YY20(((_tmpB10.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB10.f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpB10)))));}
break;}case 144: _LL2AD: {
# 1857 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1861
int _tmp585=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB11;struct Cyc_Absyn_TypeDecl*_tmp587=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp586,((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB11)))),_tmp585,(unsigned int)(yyyylsp[0]).first_line);
# 1865
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB14;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB13;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=26,((_tmpB14.f1=_tmp587,((_tmpB14.f2=0,_tmpB14)))))),_tmpB13)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1867
break;}case 145: _LL2AE: {
# 1864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1868
int _tmp58B=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB1E;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB1D;struct Cyc_Absyn_DatatypeInfo _tmpB1C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB1B;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1E.tag=3,((_tmpB1E.f1=((_tmpB1C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB1D.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB1D.is_extensible=_tmp58B,_tmpB1D))))),((_tmpB1C.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB1C)))),_tmpB1E)))),_tmpB1B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1871
break;}case 146: _LL2AF: {
# 1868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1872
int _tmp590=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB28;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB27;struct Cyc_Absyn_DatatypeFieldInfo _tmpB26;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB25;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB28.tag=4,((_tmpB28.f1=((_tmpB26.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB27.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB27.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB27.is_extensible=_tmp590,_tmpB27))))))),((_tmpB26.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpB26)))),_tmpB28)))),_tmpB25)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1877
break;}case 147: _LL2B0: {
# 1876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1880
yyval=Cyc_YY31(0);
break;}case 148: _LL2B1: {
# 1877 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1881
yyval=Cyc_YY31(1);
break;}case 149: _LL2B2: {
# 1881 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1885
{struct Cyc_List_List*_tmpB29;yyval=Cyc_YY34(((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB29->tl=0,_tmpB29)))))));}
break;}case 150: _LL2B3: {
# 1882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1886
{struct Cyc_List_List*_tmpB2A;yyval=Cyc_YY34(((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB2A->tl=0,_tmpB2A)))))));}
break;}case 151: _LL2B4: {
# 1883 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1887
{struct Cyc_List_List*_tmpB2B;yyval=Cyc_YY34(((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB2B->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB2B)))))));}
break;}case 152: _LL2B5: {
# 1884 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1888
{struct Cyc_List_List*_tmpB2C;yyval=Cyc_YY34(((_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB2C->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB2C)))))));}
break;}case 153: _LL2B6:
# 1888 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 154: _LL2B7: {
# 1889 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1893
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 155: _LL2B8: {
# 1890 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1894
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 156: _LL2B9: {
# 1894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1898
{struct Cyc_Absyn_Datatypefield*_tmpB2D;yyval=Cyc_YY33(((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB2D->typs=0,((_tmpB2D->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB2D->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB2D)))))))))));}
break;}case 157: _LL2BA: {
# 1896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1900
struct Cyc_List_List*_tmp59A=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB2E;yyval=Cyc_YY33(((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB2E->typs=_tmp59A,((_tmpB2E->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB2E->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB2E)))))))))));}
break;}case 158: _LL2BB: {
# 1902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1906
yyval=yyyyvsp[0];
break;}case 159: _LL2BC: {
# 1904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1908
struct Cyc_Parse_Declarator _tmp59C=Cyc_yyget_YY29(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB2F;yyval=Cyc_YY29(((_tmpB2F.id=_tmp59C.id,((_tmpB2F.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp59C.tms),_tmpB2F)))));}
break;}case 160: _LL2BD: {
# 1911 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1915
yyval=yyyyvsp[0];
break;}case 161: _LL2BE: {
# 1913 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1917
struct Cyc_Parse_Declarator _tmp59E=Cyc_yyget_YY29(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB30;yyval=Cyc_YY29(((_tmpB30.id=_tmp59E.id,((_tmpB30.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp59E.tms),_tmpB30)))));}
break;}case 162: _LL2BF: {
# 1919 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1923
{struct Cyc_Parse_Declarator _tmpB31;yyval=Cyc_YY29(((_tmpB31.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB31.tms=0,_tmpB31)))));}
break;}case 163: _LL2C0: {
# 1921 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1925
yyval=yyyyvsp[1];
break;}case 164: _LL2C1: {
# 1925 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1929
struct Cyc_Parse_Declarator _tmp5A1=Cyc_yyget_YY29(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB37;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB36;struct Cyc_List_List*_tmpB35;_tmp5A1.tms=((_tmpB35=_region_malloc(yyr,sizeof(*_tmpB35)),((_tmpB35->hd=(void*)((_tmpB37=_region_malloc(yyr,sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB36.tag=5,((_tmpB36.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB36.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpB36)))))),_tmpB37)))),((_tmpB35->tl=_tmp5A1.tms,_tmpB35))))));}
yyval=yyyyvsp[2];
# 1933
break;}case 165: _LL2C2: {
# 1930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1934
{struct Cyc_List_List*_tmpB41;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB40;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB3F;struct Cyc_Parse_Declarator _tmpB3E;yyval=Cyc_YY29(((_tmpB3E.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB3E.tms=((_tmpB41=_region_malloc(yyr,sizeof(*_tmpB41)),((_tmpB41->hd=(void*)((_tmpB3F=_region_malloc(yyr,sizeof(*_tmpB3F)),((_tmpB3F[0]=((_tmpB40.tag=0,((_tmpB40.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpB40.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB40)))))),_tmpB3F)))),((_tmpB41->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB41)))))),_tmpB3E)))));}
break;}case 166: _LL2C3: {
# 1932 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1936
{struct Cyc_List_List*_tmpB4B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB4A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB49;struct Cyc_Parse_Declarator _tmpB48;yyval=Cyc_YY29(((_tmpB48.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB48.tms=(
(_tmpB4B=_region_malloc(yyr,sizeof(*_tmpB4B)),((_tmpB4B->hd=(void*)((_tmpB49=_region_malloc(yyr,sizeof(*_tmpB49)),((_tmpB49[0]=((_tmpB4A.tag=1,((_tmpB4A.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpB4A.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpB4A.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB4A)))))))),_tmpB49)))),((_tmpB4B->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB4B)))))),_tmpB48)))));}
break;}case 167: _LL2C4: {
# 1935 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1939
struct Cyc_List_List*_tmp5AE;int _tmp5AF;struct Cyc_Absyn_VarargInfo*_tmp5B0;void*_tmp5B1;struct Cyc_List_List*_tmp5B2;struct _tuple24*_tmp5AD=Cyc_yyget_YY39(yyyyvsp[2]);_tmp5AE=(*_tmp5AD).f1;_tmp5AF=(*_tmp5AD).f2;_tmp5B0=(*_tmp5AD).f3;_tmp5B1=(*_tmp5AD).f4;_tmp5B2=(*_tmp5AD).f5;
{struct Cyc_List_List*_tmpB60;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB5F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB5E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB5D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5C;struct Cyc_Parse_Declarator _tmpB5B;yyval=Cyc_YY29(((_tmpB5B.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB5B.tms=((_tmpB60=_region_malloc(yyr,sizeof(*_tmpB60)),((_tmpB60->hd=(void*)((_tmpB5C=_region_malloc(yyr,sizeof(*_tmpB5C)),((_tmpB5C[0]=((_tmpB5F.tag=3,((_tmpB5F.f1=(void*)((void*)((_tmpB5D=_region_malloc(yyr,sizeof(*_tmpB5D)),((_tmpB5D[0]=((_tmpB5E.tag=1,((_tmpB5E.f1=_tmp5AE,((_tmpB5E.f2=_tmp5AF,((_tmpB5E.f3=_tmp5B0,((_tmpB5E.f4=(void*)_tmp5B1,((_tmpB5E.f5=_tmp5B2,_tmpB5E)))))))))))),_tmpB5D))))),_tmpB5F)))),_tmpB5C)))),((_tmpB60->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB60)))))),_tmpB5B)))));}
# 1942
break;}case 168: _LL2C5: {
# 1939 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1943
{struct Cyc_List_List*_tmpB75;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB74;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB73;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB72;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB71;struct Cyc_Parse_Declarator _tmpB70;yyval=Cyc_YY29(((_tmpB70.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB70.tms=(
(_tmpB75=_region_malloc(yyr,sizeof(*_tmpB75)),((_tmpB75->hd=(void*)((_tmpB71=_region_malloc(yyr,sizeof(*_tmpB71)),((_tmpB71[0]=((_tmpB74.tag=3,((_tmpB74.f1=(void*)((void*)((_tmpB72=_region_malloc(yyr,sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=1,((_tmpB73.f1=0,((_tmpB73.f2=0,((_tmpB73.f3=0,((_tmpB73.f4=(void*)
# 1946
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpB73.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpB73)))))))))))),_tmpB72))))),_tmpB74)))),_tmpB71)))),((_tmpB75->tl=(
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB75)))))),_tmpB70)))));}
# 1949
break;}case 169: _LL2C6: {
# 1946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1950
{struct Cyc_List_List*_tmpB8A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB89;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpB88;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB87;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB86;struct Cyc_Parse_Declarator _tmpB85;yyval=Cyc_YY29(((_tmpB85.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB85.tms=((_tmpB8A=_region_malloc(yyr,sizeof(*_tmpB8A)),((_tmpB8A->hd=(void*)((_tmpB86=_region_malloc(yyr,sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB89.tag=3,((_tmpB89.f1=(void*)((void*)((_tmpB87=_region_malloc(yyr,sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB88.tag=0,((_tmpB88.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpB88.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpB88)))))),_tmpB87))))),_tmpB89)))),_tmpB86)))),((_tmpB8A->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB8A)))))),_tmpB85)))));}
break;}case 170: _LL2C7: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953
struct Cyc_List_List*_tmp5C5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpB94;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpB93;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpB92;struct Cyc_Parse_Declarator _tmpB91;yyval=Cyc_YY29(((_tmpB91.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB91.tms=((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94->hd=(void*)((_tmpB92=_region_malloc(yyr,sizeof(*_tmpB92)),((_tmpB92[0]=((_tmpB93.tag=4,((_tmpB93.f1=_tmp5C5,((_tmpB93.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpB93.f3=0,_tmpB93)))))))),_tmpB92)))),((_tmpB94->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB94)))))),_tmpB91)))));}
# 1956
break;}case 171: _LL2C8: {
# 1953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1957
{struct Cyc_List_List*_tmpB9E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB9D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB9C;struct Cyc_Parse_Declarator _tmpB9B;yyval=Cyc_YY29(((_tmpB9B.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB9B.tms=((_tmpB9E=_region_malloc(yyr,sizeof(*_tmpB9E)),((_tmpB9E->hd=(void*)((_tmpB9C=_region_malloc(yyr,sizeof(*_tmpB9C)),((_tmpB9C[0]=((_tmpB9D.tag=5,((_tmpB9D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB9D.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpB9D)))))),_tmpB9C)))),((_tmpB9E->tl=(
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB9E)))))),_tmpB9B)))));}
# 1960
break;}case 172: _LL2C9: {
# 1967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1971
{struct Cyc_Parse_Declarator _tmpB9F;yyval=Cyc_YY29(((_tmpB9F.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB9F.tms=0,_tmpB9F)))));}
break;}case 173: _LL2CA: {
# 1969 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1973
{struct Cyc_Parse_Declarator _tmpBA0;yyval=Cyc_YY29(((_tmpBA0.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA0.tms=0,_tmpBA0)))));}
break;}case 174: _LL2CB: {
# 1971 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1975
yyval=yyyyvsp[1];
break;}case 175: _LL2CC: {
# 1975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1979
struct Cyc_Parse_Declarator _tmp5D0=Cyc_yyget_YY29(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBA6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBA5;struct Cyc_List_List*_tmpBA4;_tmp5D0.tms=((_tmpBA4=_region_malloc(yyr,sizeof(*_tmpBA4)),((_tmpBA4->hd=(void*)((_tmpBA6=_region_malloc(yyr,sizeof(*_tmpBA6)),((_tmpBA6[0]=((_tmpBA5.tag=5,((_tmpBA5.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBA5.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBA5)))))),_tmpBA6)))),((_tmpBA4->tl=_tmp5D0.tms,_tmpBA4))))));}
yyval=yyyyvsp[2];
# 1983
break;}case 176: _LL2CD: {
# 1980 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1984
struct Cyc_Parse_Declarator _tmp5D4=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBB0;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBAF;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBAE;struct Cyc_Parse_Declarator _tmpBAD;yyval=Cyc_YY29(((_tmpBAD.id=_tmp5D4.id,((_tmpBAD.tms=(
(_tmpBB0=_region_malloc(yyr,sizeof(*_tmpBB0)),((_tmpBB0->hd=(void*)((_tmpBAE=_region_malloc(yyr,sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBAF.tag=0,((_tmpBAF.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBAF.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBAF)))))),_tmpBAE)))),((_tmpBB0->tl=_tmp5D4.tms,_tmpBB0)))))),_tmpBAD)))));}
break;}case 177: _LL2CE: {
# 1984 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1988
struct Cyc_Parse_Declarator _tmp5D9=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBBA;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBB9;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBB8;struct Cyc_Parse_Declarator _tmpBB7;yyval=Cyc_YY29(((_tmpBB7.id=_tmp5D9.id,((_tmpBB7.tms=(
(_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA->hd=(void*)((_tmpBB8=_region_malloc(yyr,sizeof(*_tmpBB8)),((_tmpBB8[0]=((_tmpBB9.tag=1,((_tmpBB9.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpBB9.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBB9.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBB9)))))))),_tmpBB8)))),((_tmpBBA->tl=_tmp5D9.tms,_tmpBBA)))))),_tmpBB7)))));}
# 1992
break;}case 178: _LL2CF: {
# 1989 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1993
struct Cyc_List_List*_tmp5DF;int _tmp5E0;struct Cyc_Absyn_VarargInfo*_tmp5E1;void*_tmp5E2;struct Cyc_List_List*_tmp5E3;struct _tuple24*_tmp5DE=Cyc_yyget_YY39(yyyyvsp[2]);_tmp5DF=(*_tmp5DE).f1;_tmp5E0=(*_tmp5DE).f2;_tmp5E1=(*_tmp5DE).f3;_tmp5E2=(*_tmp5DE).f4;_tmp5E3=(*_tmp5DE).f5;{
struct Cyc_Parse_Declarator _tmp5E4=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBCF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBCE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBCD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBCC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBCB;struct Cyc_Parse_Declarator _tmpBCA;yyval=Cyc_YY29(((_tmpBCA.id=_tmp5E4.id,((_tmpBCA.tms=((_tmpBCF=_region_malloc(yyr,sizeof(*_tmpBCF)),((_tmpBCF->hd=(void*)((_tmpBCB=_region_malloc(yyr,sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCE.tag=3,((_tmpBCE.f1=(void*)((void*)((_tmpBCC=_region_malloc(yyr,sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCD.tag=1,((_tmpBCD.f1=_tmp5DF,((_tmpBCD.f2=_tmp5E0,((_tmpBCD.f3=_tmp5E1,((_tmpBCD.f4=(void*)_tmp5E2,((_tmpBCD.f5=_tmp5E3,_tmpBCD)))))))))))),_tmpBCC))))),_tmpBCE)))),_tmpBCB)))),((_tmpBCF->tl=_tmp5E4.tms,_tmpBCF)))))),_tmpBCA)))));}
# 1997
break;};}case 179: _LL2D0: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1998
struct Cyc_Parse_Declarator _tmp5EB=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBE4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBE1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE0;struct Cyc_Parse_Declarator _tmpBDF;yyval=Cyc_YY29(((_tmpBDF.id=_tmp5EB.id,((_tmpBDF.tms=(
(_tmpBE4=_region_malloc(yyr,sizeof(*_tmpBE4)),((_tmpBE4->hd=(void*)((_tmpBE0=_region_malloc(yyr,sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE3.tag=3,((_tmpBE3.f1=(void*)((void*)((_tmpBE1=_region_malloc(yyr,sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE2.tag=1,((_tmpBE2.f1=0,((_tmpBE2.f2=0,((_tmpBE2.f3=0,((_tmpBE2.f4=(void*)
# 2002
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBE2.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpBE2)))))))))))),_tmpBE1))))),_tmpBE3)))),_tmpBE0)))),((_tmpBE4->tl=_tmp5EB.tms,_tmpBE4)))))),_tmpBDF)))));}
# 2005
break;}case 180: _LL2D1: {
# 2002 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2006
struct Cyc_Parse_Declarator _tmp5F2=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF8;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBF7;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBF6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF5;struct Cyc_Parse_Declarator _tmpBF4;yyval=Cyc_YY29(((_tmpBF4.id=_tmp5F2.id,((_tmpBF4.tms=((_tmpBF9=_region_malloc(yyr,sizeof(*_tmpBF9)),((_tmpBF9->hd=(void*)((_tmpBF5=_region_malloc(yyr,sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF8.tag=3,((_tmpBF8.f1=(void*)((void*)((_tmpBF6=_region_malloc(yyr,sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF7.tag=0,((_tmpBF7.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpBF7.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBF7)))))),_tmpBF6))))),_tmpBF8)))),_tmpBF5)))),((_tmpBF9->tl=_tmp5F2.tms,_tmpBF9)))))),_tmpBF4)))));}
break;}case 181: _LL2D2: {
# 2006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2010
struct Cyc_List_List*_tmp5F9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp5FA=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC03;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC02;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC01;struct Cyc_Parse_Declarator _tmpC00;yyval=Cyc_YY29(((_tmpC00.id=_tmp5FA.id,((_tmpC00.tms=((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03->hd=(void*)((_tmpC01=_region_malloc(yyr,sizeof(*_tmpC01)),((_tmpC01[0]=((_tmpC02.tag=4,((_tmpC02.f1=_tmp5F9,((_tmpC02.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC02.f3=0,_tmpC02)))))))),_tmpC01)))),((_tmpC03->tl=_tmp5FA.tms,_tmpC03)))))),_tmpC00)))));}
# 2014
break;}case 182: _LL2D3: {
# 2011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2015
struct Cyc_Parse_Declarator _tmp5FF=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC0C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC0B;struct Cyc_Parse_Declarator _tmpC0A;yyval=Cyc_YY29(((_tmpC0A.id=_tmp5FF.id,((_tmpC0A.tms=((_tmpC0D=_region_malloc(yyr,sizeof(*_tmpC0D)),((_tmpC0D->hd=(void*)((_tmpC0B=_region_malloc(yyr,sizeof(*_tmpC0B)),((_tmpC0B[0]=((_tmpC0C.tag=5,((_tmpC0C.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC0C.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC0C)))))),_tmpC0B)))),((_tmpC0D->tl=_tmp5FF.tms,_tmpC0D)))))),_tmpC0A)))));}
# 2018
break;}case 183: _LL2D4: {
# 2024 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2028
yyval=yyyyvsp[0];
break;}case 184: _LL2D5: {
# 2025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2029
yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),Cyc_yyget_YY28(yyyyvsp[1])));
break;}case 185: _LL2D6: {
# 2029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2033
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC13;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC12;struct Cyc_List_List*_tmpC11;ans=((_tmpC11=_region_malloc(yyr,sizeof(*_tmpC11)),((_tmpC11->hd=(void*)((_tmpC13=_region_malloc(yyr,sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC12.tag=5,((_tmpC12.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC12.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC12)))))),_tmpC13)))),((_tmpC11->tl=ans,_tmpC11))))));}{
# 2037
struct Cyc_Absyn_PtrLoc*ptrloc=0;
unsigned int _tmp608;union Cyc_Absyn_Constraint*_tmp609;union Cyc_Absyn_Constraint*_tmp60A;struct _tuple20 _tmp607=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;_tmp60A=_tmp607.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC14;ptrloc=((_tmpC14=_cycalloc_atomic(sizeof(*_tmpC14)),((_tmpC14->ptr_loc=_tmp608,((_tmpC14->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC14->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC14))))))));}{
# 2042
union Cyc_Absyn_Constraint*_tmp60D;union Cyc_Absyn_Constraint*_tmp60E;union Cyc_Absyn_Constraint*_tmp60F;void*_tmp610;struct _tuple14 _tmp60C=Cyc_Parse_collapse_pointer_quals(_tmp608,_tmp609,_tmp60A,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;_tmp60F=_tmp60C.f3;_tmp610=_tmp60C.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC1E;struct Cyc_Absyn_PtrAtts _tmpC1D;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC1C;struct Cyc_List_List*_tmpC1B;ans=((_tmpC1B=_region_malloc(yyr,sizeof(*_tmpC1B)),((_tmpC1B->hd=(void*)((_tmpC1E=_region_malloc(yyr,sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1C.tag=2,((_tmpC1C.f1=((_tmpC1D.rgn=_tmp610,((_tmpC1D.nullable=_tmp60D,((_tmpC1D.bounds=_tmp60E,((_tmpC1D.zero_term=_tmp60F,((_tmpC1D.ptrloc=ptrloc,_tmpC1D)))))))))),((_tmpC1C.f2=Cyc_yyget_YY25(yyyyvsp[4]),_tmpC1C)))))),_tmpC1E)))),((_tmpC1B->tl=ans,_tmpC1B))))));}
yyval=Cyc_YY28(ans);
# 2046
break;};};}case 186: _LL2D7:
# 2044 "parse.y"
 yyval=Cyc_YY54(0);
break;case 187: _LL2D8: {
# 2045 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2049
{struct Cyc_List_List*_tmpC1F;yyval=Cyc_YY54(((_tmpC1F=_region_malloc(yyr,sizeof(*_tmpC1F)),((_tmpC1F->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpC1F->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpC1F)))))));}
break;}case 188: _LL2D9: {
# 2050 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2054
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC22;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC21;yyval=Cyc_YY53((void*)((_tmpC21=_region_malloc(yyr,sizeof(*_tmpC21)),((_tmpC21[0]=((_tmpC22.tag=0,((_tmpC22.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpC22)))),_tmpC21)))));}
break;}case 189: _LL2DA: {
# 2052 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2056
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC25;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC24;yyval=Cyc_YY53((void*)((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC25.tag=1,((_tmpC25.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpC25)))),_tmpC24)))));}
break;}case 190: _LL2DB: {
# 2054 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2058
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC28;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC27;yyval=Cyc_YY53((void*)((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC28.tag=2,_tmpC28)),_tmpC27)))));}
break;}case 191: _LL2DC: {
# 2056 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2060
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC2B;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC2A;yyval=Cyc_YY53((void*)((_tmpC2A=_region_malloc(yyr,sizeof(*_tmpC2A)),((_tmpC2A[0]=((_tmpC2B.tag=3,_tmpC2B)),_tmpC2A)))));}
break;}case 192: _LL2DD: {
# 2058 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2062
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC2E;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC2D;yyval=Cyc_YY53((void*)((_tmpC2D=_region_malloc(yyr,sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2E.tag=4,_tmpC2E)),_tmpC2D)))));}
break;}case 193: _LL2DE: {
# 2060 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2064
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC31;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC30;yyval=Cyc_YY53((void*)((_tmpC30=_region_malloc(yyr,sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC31.tag=5,_tmpC31)),_tmpC30)))));}
break;}case 194: _LL2DF: {
# 2062 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2066
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC34;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC33;yyval=Cyc_YY53((void*)((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=6,_tmpC34)),_tmpC33)))));}
break;}case 195: _LL2E0: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2068
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC37;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC36;yyval=Cyc_YY53((void*)((_tmpC36=_region_malloc(yyr,sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC37.tag=7,_tmpC37)),_tmpC36)))));}
break;}case 196: _LL2E1: {
# 2069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2074
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC38;yyval=Cyc_YY1(((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38->f1=loc,((_tmpC38->f2=Cyc_Absyn_true_conref,((_tmpC38->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC38)))))))));}
# 2077
break;}case 197: _LL2E2: {
# 2074 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2078
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC39;yyval=Cyc_YY1(((_tmpC39=_cycalloc(sizeof(*_tmpC39)),((_tmpC39->f1=loc,((_tmpC39->f2=Cyc_Absyn_false_conref,((_tmpC39->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC39)))))))));}
# 2081
break;}case 198: _LL2E3: {
# 2078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2082
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC3A;yyval=Cyc_YY1(((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->f1=loc,((_tmpC3A->f2=Cyc_Absyn_true_conref,((_tmpC3A->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC3A)))))))));}
# 2085
break;}case 199: _LL2E4:
# 2083 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2E5: {
# 2084 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2088
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC3D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC3C;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=1,((_tmpC3D.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpC3D)))),_tmpC3C))))));}
break;}case 201: _LL2E6:
# 2087 "parse.y"
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2E7: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2092
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 203: _LL2E8: {
# 2089 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2093
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 204: _LL2E9:
# 2094 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 205: _LL2EA: {
# 2095 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2099
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 206: _LL2EB: {
# 2096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2100
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;}case 207: _LL2EC:
# 2100 "parse.y"
 yyval=Cyc_YY25(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 208: _LL2ED: {
# 2101 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2105
yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_YY25(yyyyvsp[1])));
break;}case 209: _LL2EE: {
# 2106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2110
{struct _tuple24*_tmpC3E;yyval=Cyc_YY39(((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC3E->f2=0,((_tmpC3E->f3=0,((_tmpC3E->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpC3E->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpC3E)))))))))))));}
break;}case 210: _LL2EF: {
# 2108 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2112
{struct _tuple24*_tmpC3F;yyval=Cyc_YY39(((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC3F->f2=1,((_tmpC3F->f3=0,((_tmpC3F->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC3F->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC3F)))))))))))));}
break;}case 211: _LL2F0: {
# 2111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2115
struct _dyneither_ptr*_tmp62E;struct Cyc_Absyn_Tqual _tmp62F;void*_tmp630;struct _tuple8*_tmp62D=Cyc_yyget_YY37(yyyyvsp[2]);_tmp62E=(*_tmp62D).f1;_tmp62F=(*_tmp62D).f2;_tmp630=(*_tmp62D).f3;{
struct Cyc_Absyn_VarargInfo*_tmpC40;struct Cyc_Absyn_VarargInfo*_tmp631=(_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40->name=_tmp62E,((_tmpC40->tq=_tmp62F,((_tmpC40->type=_tmp630,((_tmpC40->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpC40)))))))));
{struct _tuple24*_tmpC41;yyval=Cyc_YY39(((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41->f1=0,((_tmpC41->f2=0,((_tmpC41->f3=_tmp631,((_tmpC41->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC41->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC41)))))))))))));}
# 2119
break;};}case 212: _LL2F1: {
# 2117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2121
struct _dyneither_ptr*_tmp635;struct Cyc_Absyn_Tqual _tmp636;void*_tmp637;struct _tuple8*_tmp634=Cyc_yyget_YY37(yyyyvsp[4]);_tmp635=(*_tmp634).f1;_tmp636=(*_tmp634).f2;_tmp637=(*_tmp634).f3;{
struct Cyc_Absyn_VarargInfo*_tmpC42;struct Cyc_Absyn_VarargInfo*_tmp638=(_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42->name=_tmp635,((_tmpC42->tq=_tmp636,((_tmpC42->type=_tmp637,((_tmpC42->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpC42)))))))));
{struct _tuple24*_tmpC43;yyval=Cyc_YY39(((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC43->f2=0,((_tmpC43->f3=_tmp638,((_tmpC43->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpC43->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpC43)))))))))))));}
# 2125
break;};}case 213: _LL2F2: {
# 2125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2129
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC46;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC45;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=1,((_tmpC46.f1=0,_tmpC46)))),_tmpC45))))));}
break;}case 214: _LL2F3: {
# 2126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2130
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 215: _LL2F4:
# 2129 "parse.y"
 yyval=Cyc_YY49(0);
break;case 216: _LL2F5: {
# 2130 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2134
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC49;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC48;yyval=Cyc_YY49((void*)((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=24,((_tmpC49.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpC49)))),_tmpC48)))));}
break;}case 217: _LL2F6:
# 2134 "parse.y"
 yyval=Cyc_YY50(0);
break;case 218: _LL2F7: {
# 2135 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2139
yyval=yyyyvsp[1];
break;}case 219: _LL2F8: {
# 2140 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2147
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC4C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC4B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp63F=(_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4C.tag=2,((_tmpC4C.f1=0,((_tmpC4C.f2=& Cyc_Tcutil_trk,_tmpC4C)))))),_tmpC4B)));
struct _dyneither_ptr _tmp640=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp641=Cyc_Parse_id2type(_tmp640,(void*)_tmp63F);
{struct _tuple27*_tmpC56;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC55;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC54;struct Cyc_List_List*_tmpC53;yyval=Cyc_YY50(((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=((_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->f1=(void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=24,((_tmpC55.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpC55)))),_tmpC54)))),((_tmpC56->f2=_tmp641,_tmpC56)))))),((_tmpC53->tl=0,_tmpC53)))))));}
# 2152
break;}case 220: _LL2F9: {
# 2149 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2154
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC59;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC58;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp648=(_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=2,((_tmpC59.f1=0,((_tmpC59.f2=& Cyc_Tcutil_trk,_tmpC59)))))),_tmpC58)));
struct _dyneither_ptr _tmp649=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp64A=Cyc_Parse_id2type(_tmp649,(void*)_tmp648);
{struct _tuple27*_tmpC63;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC62;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC61;struct Cyc_List_List*_tmpC60;yyval=Cyc_YY50(((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->hd=((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->f1=(void*)((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=24,((_tmpC62.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpC62)))),_tmpC61)))),((_tmpC63->f2=_tmp64A,_tmpC63)))))),((_tmpC60->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC60)))))));}
# 2159
break;}case 221: _LL2FA:
# 2159 "parse.y"
 yyval=Cyc_YY31(0);
break;case 222: _LL2FB: {
# 2161 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2165
{const char*_tmpC64;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC64="inject",_tag_dyneither(_tmpC64,sizeof(char),7))))!= 0){
const char*_tmpC65;Cyc_Parse_err(((_tmpC65="missing type in function declaration",_tag_dyneither(_tmpC65,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2169
break;}case 223: _LL2FC: {
# 2168 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2172
yyval=yyyyvsp[0];
break;}case 224: _LL2FD: {
# 2169 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2173
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 225: _LL2FE: {
# 2173 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2177
yyval=Cyc_YY40(0);
break;}case 226: _LL2FF: {
# 2174 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2178
yyval=yyyyvsp[1];
break;}case 227: _LL300: {
# 2176 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2180
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC6B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC6A;struct Cyc_List_List*_tmpC69;yyval=Cyc_YY40(((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->hd=(void*)((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6A.tag=25,((_tmpC6A.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpC6A)))),_tmpC6B)))),((_tmpC69->tl=0,_tmpC69)))))));}
break;}case 228: _LL301: {
# 2178 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2182
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC6C;yyval=Cyc_YY40(((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpC6C->tl=0,_tmpC6C)))))));}
# 2185
break;}case 229: _LL302: {
# 2186 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2190
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC72;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC71;struct Cyc_List_List*_tmpC70;yyval=Cyc_YY40(((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70->hd=(void*)((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC71.tag=23,((_tmpC71.f1=(void*)Cyc_yyget_YY44(yyyyvsp[0]),_tmpC71)))),_tmpC72)))),((_tmpC70->tl=0,_tmpC70)))))));}
# 2193
break;}case 230: _LL303: {
# 2190 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2194
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC78;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC77;struct Cyc_List_List*_tmpC76;yyval=Cyc_YY40(((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=(void*)((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC77.tag=23,((_tmpC77.f1=(void*)Cyc_yyget_YY44(yyyyvsp[0]),_tmpC77)))),_tmpC78)))),((_tmpC76->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpC76)))))));}
# 2197
break;}case 231: _LL304: {
# 2198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2202
{struct Cyc_List_List*_tmpC79;yyval=Cyc_YY38(((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpC79->tl=0,_tmpC79)))))));}
break;}case 232: _LL305: {
# 2200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2204
{struct Cyc_List_List*_tmpC7A;yyval=Cyc_YY38(((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpC7A->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpC7A)))))));}
break;}case 233: _LL306: {
# 2206 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2210
struct Cyc_Absyn_Tqual _tmp660;struct Cyc_Parse_Type_specifier _tmp661;struct Cyc_List_List*_tmp662;struct _tuple23 _tmp65F=Cyc_yyget_YY35(yyyyvsp[0]);_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;
if(_tmp660.loc == 0)_tmp660.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple0*_tmp664;struct Cyc_List_List*_tmp665;struct Cyc_Parse_Declarator _tmp663=Cyc_yyget_YY29(yyyyvsp[1]);_tmp664=_tmp663.id;_tmp665=_tmp663.tms;{
void*_tmp666=Cyc_Parse_speclist2typ(_tmp661,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_Absyn_Tqual _tmp668;void*_tmp669;struct Cyc_List_List*_tmp66A;struct Cyc_List_List*_tmp66B;struct _tuple13 _tmp667=Cyc_Parse_apply_tms(_tmp660,_tmp666,_tmp662,_tmp665);_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;_tmp66A=_tmp667.f3;_tmp66B=_tmp667.f4;
if(_tmp66A != 0){
const char*_tmpC7B;Cyc_Parse_err(((_tmpC7B="parameter with bad type params",_tag_dyneither(_tmpC7B,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp664)){
const char*_tmpC7C;Cyc_Parse_err(((_tmpC7C="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC7C,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(struct _dyneither_ptr*)(*_tmp664).f2;
if(_tmp66B != 0){
const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC7F="extra attributes on parameter, ignoring",_tag_dyneither(_tmpC7F,sizeof(char),40))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}
{struct _tuple8*_tmpC80;yyval=Cyc_YY37(((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->f1=idopt,((_tmpC80->f2=_tmp668,((_tmpC80->f3=_tmp669,_tmpC80)))))))));}
# 2224
break;};};};}case 234: _LL307: {
# 2221 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2225
struct Cyc_Absyn_Tqual _tmp672;struct Cyc_Parse_Type_specifier _tmp673;struct Cyc_List_List*_tmp674;struct _tuple23 _tmp671=Cyc_yyget_YY35(yyyyvsp[0]);_tmp672=_tmp671.f1;_tmp673=_tmp671.f2;_tmp674=_tmp671.f3;
if(_tmp672.loc == 0)_tmp672.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp675=Cyc_Parse_speclist2typ(_tmp673,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp674 != 0){
const char*_tmpC83;void*_tmpC82;(_tmpC82=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC83="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC83,sizeof(char),38))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}
{struct _tuple8*_tmpC84;yyval=Cyc_YY37(((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->f1=0,((_tmpC84->f2=_tmp672,((_tmpC84->f3=_tmp675,_tmpC84)))))))));}
# 2232
break;};}case 235: _LL308: {
# 2229 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2233
struct Cyc_Absyn_Tqual _tmp67A;struct Cyc_Parse_Type_specifier _tmp67B;struct Cyc_List_List*_tmp67C;struct _tuple23 _tmp679=Cyc_yyget_YY35(yyyyvsp[0]);_tmp67A=_tmp679.f1;_tmp67B=_tmp679.f2;_tmp67C=_tmp679.f3;
if(_tmp67A.loc == 0)_tmp67A.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp67D=Cyc_Parse_speclist2typ(_tmp67B,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp67E=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct Cyc_Absyn_Tqual _tmp680;void*_tmp681;struct Cyc_List_List*_tmp682;struct Cyc_List_List*_tmp683;struct _tuple13 _tmp67F=Cyc_Parse_apply_tms(_tmp67A,_tmp67D,_tmp67C,_tmp67E);_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;_tmp683=_tmp67F.f4;
if(_tmp682 != 0){
const char*_tmpC87;void*_tmpC86;(_tmpC86=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC87="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpC87,sizeof(char),49))),_tag_dyneither(_tmpC86,sizeof(void*),0)));}
# 2241
if(_tmp683 != 0){
const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8A="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC8A,sizeof(char),38))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}
{struct _tuple8*_tmpC8B;yyval=Cyc_YY37(((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->f1=0,((_tmpC8B->f2=_tmp680,((_tmpC8B->f3=_tmp681,_tmpC8B)))))))));}
# 2245
break;};}case 236: _LL309: {
# 2245 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2249
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 237: _LL30A: {
# 2249 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2253
{struct _dyneither_ptr*_tmpC8E;struct Cyc_List_List*_tmpC8D;yyval=Cyc_YY36(((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->hd=((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpC8E)))),((_tmpC8D->tl=0,_tmpC8D)))))));}
break;}case 238: _LL30B: {
# 2251 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2255
{struct _dyneither_ptr*_tmpC91;struct Cyc_List_List*_tmpC90;yyval=Cyc_YY36(((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpC91)))),((_tmpC90->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpC90)))))));}
break;}case 239: _LL30C: {
# 2255 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2259
yyval=yyyyvsp[0];
break;}case 240: _LL30D: {
# 2256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2260
yyval=yyyyvsp[0];
break;}case 241: _LL30E: {
# 2261 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2265
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC94;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC93;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=34,((_tmpC94.f1=0,((_tmpC94.f2=0,_tmpC94)))))),_tmpC93)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 242: _LL30F: {
# 2263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2267
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC97;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC96;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=34,((_tmpC97.f1=0,((_tmpC97.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpC97)))))),_tmpC96)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL310: {
# 2265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2269
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9A;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC99;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99[0]=((_tmpC9A.tag=34,((_tmpC9A.f1=0,((_tmpC9A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpC9A)))))),_tmpC99)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL311: {
# 2267 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2271
struct _dyneither_ptr*_tmpC9D;struct _tuple0*_tmpC9C;struct Cyc_Absyn_Vardecl*_tmp693=Cyc_Absyn_new_vardecl(((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->f1=Cyc_Absyn_Loc_n,((_tmpC9C->f2=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpC9D)))),_tmpC9C)))))),Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2274
(_tmp693->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCA0;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC9F;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=26,((_tmpCA0.f1=_tmp693,((_tmpCA0.f2=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpCA0.f3=Cyc_yyget_YY3(yyyyvsp[6]),((_tmpCA0.f4=0,_tmpCA0)))))))))),_tmpC9F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2277
break;}case 245: _LL312: {
# 2278 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2282
{struct _tuple28*_tmpCA3;struct Cyc_List_List*_tmpCA2;yyval=Cyc_YY6(((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3->f1=0,((_tmpCA3->f2=Cyc_yyget_YY3(yyyyvsp[0]),_tmpCA3)))))),((_tmpCA2->tl=0,_tmpCA2)))))));}
break;}case 246: _LL313: {
# 2280 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2284
{struct _tuple28*_tmpCA6;struct Cyc_List_List*_tmpCA5;yyval=Cyc_YY6(((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5->hd=((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpCA6->f2=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCA6)))))),((_tmpCA5->tl=0,_tmpCA5)))))));}
break;}case 247: _LL314: {
# 2282 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2286
{struct _tuple28*_tmpCA9;struct Cyc_List_List*_tmpCA8;yyval=Cyc_YY6(((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->hd=((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9->f1=0,((_tmpCA9->f2=Cyc_yyget_YY3(yyyyvsp[2]),_tmpCA9)))))),((_tmpCA8->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCA8)))))));}
break;}case 248: _LL315: {
# 2284 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2288
{struct _tuple28*_tmpCAC;struct Cyc_List_List*_tmpCAB;yyval=Cyc_YY6(((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpCAC->f2=Cyc_yyget_YY3(yyyyvsp[3]),_tmpCAC)))))),((_tmpCAB->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCAB)))))));}
break;}case 249: _LL316: {
# 2288 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2292
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 250: _LL317: {
# 2293 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2297
{struct Cyc_List_List*_tmpCAD;yyval=Cyc_YY41(((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpCAD->tl=0,_tmpCAD)))))));}
break;}case 251: _LL318: {
# 2294 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2298
{struct Cyc_List_List*_tmpCAE;yyval=Cyc_YY41(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpCAE->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCAE)))))));}
break;}case 252: _LL319: {
# 2298 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2302
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCB1;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCB0;yyval=Cyc_YY42((void*)((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=((_tmpCB1.tag=0,((_tmpCB1.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCB1)))),_tmpCB0)))));}
break;}case 253: _LL31A: {
# 2299 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2303
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCB7;struct _dyneither_ptr*_tmpCB6;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCB5;yyval=Cyc_YY42((void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB7.tag=1,((_tmpCB7.f1=((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCB6)))),_tmpCB7)))),_tmpCB5)))));}
break;}case 254: _LL31B: {
# 2304 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2308
struct Cyc_Absyn_Tqual _tmp6A8;struct Cyc_Parse_Type_specifier _tmp6A9;struct Cyc_List_List*_tmp6AA;struct _tuple23 _tmp6A7=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6A8=_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;_tmp6AA=_tmp6A7.f3;{
void*_tmp6AB=Cyc_Parse_speclist2typ(_tmp6A9,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6AA != 0){
const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCBA="ignoring attributes in type",_tag_dyneither(_tmpCBA,sizeof(char),28))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}
{struct _tuple8*_tmpCBB;yyval=Cyc_YY37(((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->f1=0,((_tmpCBB->f2=_tmp6A8,((_tmpCBB->f3=_tmp6AB,_tmpCBB)))))))));}
# 2314
break;};}case 255: _LL31C: {
# 2311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2315
struct Cyc_Absyn_Tqual _tmp6B0;struct Cyc_Parse_Type_specifier _tmp6B1;struct Cyc_List_List*_tmp6B2;struct _tuple23 _tmp6AF=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6B0=_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;_tmp6B2=_tmp6AF.f3;{
void*_tmp6B3=Cyc_Parse_speclist2typ(_tmp6B1,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6B4=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6B5=Cyc_Parse_apply_tms(_tmp6B0,_tmp6B3,_tmp6B2,_tmp6B4);
if(_tmp6B5.f3 != 0){
# 2321
const char*_tmpCBE;void*_tmpCBD;(_tmpCBD=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCBE="bad type params, ignoring",_tag_dyneither(_tmpCBE,sizeof(char),26))),_tag_dyneither(_tmpCBD,sizeof(void*),0)));}
if(_tmp6B5.f4 != 0){
const char*_tmpCC1;void*_tmpCC0;(_tmpCC0=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCC1="bad specifiers, ignoring",_tag_dyneither(_tmpCC1,sizeof(char),25))),_tag_dyneither(_tmpCC0,sizeof(void*),0)));}
{struct _tuple8*_tmpCC2;yyval=Cyc_YY37(((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=_tmp6B5.f1,((_tmpCC2->f3=_tmp6B5.f2,_tmpCC2)))))))));}
# 2326
break;};}case 256: _LL31D: {
# 2325 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2329
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 257: _LL31E: {
# 2326 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2330
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCC5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC4;yyval=Cyc_YY44((void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=24,((_tmpCC5.f1=0,_tmpCC5)))),_tmpCC4)))));}
break;}case 258: _LL31F: {
# 2327 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2331
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCC8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC7;yyval=Cyc_YY44((void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=24,((_tmpCC8.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCC8)))),_tmpCC7)))));}
break;}case 259: _LL320: {
# 2328 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2332
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCCB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCCA;yyval=Cyc_YY44((void*)((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=25,((_tmpCCB.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpCCB)))),_tmpCCA)))));}
break;}case 260: _LL321: {
# 2329 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2333
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD1;struct Cyc_List_List*_tmpCD0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCCF;yyval=Cyc_YY44((void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD1.tag=24,((_tmpCD1.f1=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCD0->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCD0)))))),_tmpCD1)))),_tmpCCF)))));}
break;}case 261: _LL322: {
# 2335 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2339
{struct Cyc_List_List*_tmpCD2;yyval=Cyc_YY40(((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCD2->tl=0,_tmpCD2)))))));}
break;}case 262: _LL323: {
# 2336 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2340
{struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY40(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD3->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCD3)))))));}
break;}case 263: _LL324: {
# 2341 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2345
{struct Cyc_Parse_Abstractdeclarator _tmpCD4;yyval=Cyc_YY30(((_tmpCD4.tms=Cyc_yyget_YY28(yyyyvsp[0]),_tmpCD4)));}
break;}case 264: _LL325: {
# 2343 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2347
yyval=yyyyvsp[0];
break;}case 265: _LL326: {
# 2345 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2349
{struct Cyc_Parse_Abstractdeclarator _tmpCD5;yyval=Cyc_YY30(((_tmpCD5.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpCD5)));}
break;}case 266: _LL327: {
# 2350 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2354
yyval=yyyyvsp[1];
break;}case 267: _LL328: {
# 2352 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2356
{struct Cyc_List_List*_tmpCDF;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCDE;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCDD;struct Cyc_Parse_Abstractdeclarator _tmpCDC;yyval=Cyc_YY30(((_tmpCDC.tms=((_tmpCDF=_region_malloc(yyr,sizeof(*_tmpCDF)),((_tmpCDF->hd=(void*)((_tmpCDD=_region_malloc(yyr,sizeof(*_tmpCDD)),((_tmpCDD[0]=((_tmpCDE.tag=0,((_tmpCDE.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpCDE.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpCDE)))))),_tmpCDD)))),((_tmpCDF->tl=0,_tmpCDF)))))),_tmpCDC)));}
break;}case 268: _LL329: {
# 2354 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2358
{struct Cyc_List_List*_tmpCE9;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCE8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCE7;struct Cyc_Parse_Abstractdeclarator _tmpCE6;yyval=Cyc_YY30(((_tmpCE6.tms=((_tmpCE9=_region_malloc(yyr,sizeof(*_tmpCE9)),((_tmpCE9->hd=(void*)((_tmpCE7=_region_malloc(yyr,sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=0,((_tmpCE8.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpCE8.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpCE8)))))),_tmpCE7)))),((_tmpCE9->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpCE9)))))),_tmpCE6)));}
break;}case 269: _LL32A: {
# 2356 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2360
{struct Cyc_List_List*_tmpCF3;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpCF2;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpCF1;struct Cyc_Parse_Abstractdeclarator _tmpCF0;yyval=Cyc_YY30(((_tmpCF0.tms=((_tmpCF3=_region_malloc(yyr,sizeof(*_tmpCF3)),((_tmpCF3->hd=(void*)((_tmpCF1=_region_malloc(yyr,sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=1,((_tmpCF2.f1=Cyc_yyget_YY3(yyyyvsp[1]),((_tmpCF2.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpCF2.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpCF2)))))))),_tmpCF1)))),((_tmpCF3->tl=0,_tmpCF3)))))),_tmpCF0)));}
break;}case 270: _LL32B: {
# 2358 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2362
{struct Cyc_List_List*_tmpCFD;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpCFC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpCFB;struct Cyc_Parse_Abstractdeclarator _tmpCFA;yyval=Cyc_YY30(((_tmpCFA.tms=((_tmpCFD=_region_malloc(yyr,sizeof(*_tmpCFD)),((_tmpCFD->hd=(void*)((_tmpCFB=_region_malloc(yyr,sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=1,((_tmpCFC.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpCFC.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpCFC.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpCFC)))))))),_tmpCFB)))),((_tmpCFD->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpCFD)))))),_tmpCFA)));}
# 2365
break;}case 271: _LL32C: {
# 2362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2366
{struct Cyc_List_List*_tmpD12;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD11;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD10;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD0F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD0E;struct Cyc_Parse_Abstractdeclarator _tmpD0D;yyval=Cyc_YY30(((_tmpD0D.tms=((_tmpD12=_region_malloc(yyr,sizeof(*_tmpD12)),((_tmpD12->hd=(void*)((_tmpD0E=_region_malloc(yyr,sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD11.tag=3,((_tmpD11.f1=(void*)((void*)((_tmpD0F=_region_malloc(yyr,sizeof(*_tmpD0F)),((_tmpD0F[0]=((_tmpD10.tag=1,((_tmpD10.f1=0,((_tmpD10.f2=0,((_tmpD10.f3=0,((_tmpD10.f4=(void*)Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD10.f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpD10)))))))))))),_tmpD0F))))),_tmpD11)))),_tmpD0E)))),((_tmpD12->tl=0,_tmpD12)))))),_tmpD0D)));}
# 2368
break;}case 272: _LL32D: {
# 2365 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2369
struct Cyc_List_List*_tmp6DF;int _tmp6E0;struct Cyc_Absyn_VarargInfo*_tmp6E1;void*_tmp6E2;struct Cyc_List_List*_tmp6E3;struct _tuple24*_tmp6DE=Cyc_yyget_YY39(yyyyvsp[1]);_tmp6DF=(*_tmp6DE).f1;_tmp6E0=(*_tmp6DE).f2;_tmp6E1=(*_tmp6DE).f3;_tmp6E2=(*_tmp6DE).f4;_tmp6E3=(*_tmp6DE).f5;
{struct Cyc_List_List*_tmpD27;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD26;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD25;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD24;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD23;struct Cyc_Parse_Abstractdeclarator _tmpD22;yyval=Cyc_YY30(((_tmpD22.tms=((_tmpD27=_region_malloc(yyr,sizeof(*_tmpD27)),((_tmpD27->hd=(void*)((_tmpD23=_region_malloc(yyr,sizeof(*_tmpD23)),((_tmpD23[0]=((_tmpD26.tag=3,((_tmpD26.f1=(void*)((void*)((_tmpD24=_region_malloc(yyr,sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD25.tag=1,((_tmpD25.f1=_tmp6DF,((_tmpD25.f2=_tmp6E0,((_tmpD25.f3=_tmp6E1,((_tmpD25.f4=(void*)_tmp6E2,((_tmpD25.f5=_tmp6E3,_tmpD25)))))))))))),_tmpD24))))),_tmpD26)))),_tmpD23)))),((_tmpD27->tl=0,_tmpD27)))))),_tmpD22)));}
# 2372
break;}case 273: _LL32E: {
# 2369 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2373
{struct Cyc_List_List*_tmpD3C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD3B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD3A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD39;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD38;struct Cyc_Parse_Abstractdeclarator _tmpD37;yyval=Cyc_YY30(((_tmpD37.tms=((_tmpD3C=_region_malloc(yyr,sizeof(*_tmpD3C)),((_tmpD3C->hd=(void*)((_tmpD38=_region_malloc(yyr,sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD3B.tag=3,((_tmpD3B.f1=(void*)((void*)((_tmpD39=_region_malloc(yyr,sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD3A.tag=1,((_tmpD3A.f1=0,((_tmpD3A.f2=0,((_tmpD3A.f3=0,((_tmpD3A.f4=(void*)Cyc_yyget_YY49(yyyyvsp[2]),((_tmpD3A.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpD3A)))))))))))),_tmpD39))))),_tmpD3B)))),_tmpD38)))),((_tmpD3C->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD3C)))))),_tmpD37)));}
# 2376
break;}case 274: _LL32F: {
# 2373 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2377
struct Cyc_List_List*_tmp6F1;int _tmp6F2;struct Cyc_Absyn_VarargInfo*_tmp6F3;void*_tmp6F4;struct Cyc_List_List*_tmp6F5;struct _tuple24*_tmp6F0=Cyc_yyget_YY39(yyyyvsp[2]);_tmp6F1=(*_tmp6F0).f1;_tmp6F2=(*_tmp6F0).f2;_tmp6F3=(*_tmp6F0).f3;_tmp6F4=(*_tmp6F0).f4;_tmp6F5=(*_tmp6F0).f5;
{struct Cyc_List_List*_tmpD51;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD50;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD4F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD4E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4D;struct Cyc_Parse_Abstractdeclarator _tmpD4C;yyval=Cyc_YY30(((_tmpD4C.tms=((_tmpD51=_region_malloc(yyr,sizeof(*_tmpD51)),((_tmpD51->hd=(void*)((_tmpD4D=_region_malloc(yyr,sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD50.tag=3,((_tmpD50.f1=(void*)((void*)((_tmpD4E=_region_malloc(yyr,sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=1,((_tmpD4F.f1=_tmp6F1,((_tmpD4F.f2=_tmp6F2,((_tmpD4F.f3=_tmp6F3,((_tmpD4F.f4=(void*)_tmp6F4,((_tmpD4F.f5=_tmp6F5,_tmpD4F)))))))))))),_tmpD4E))))),_tmpD50)))),_tmpD4D)))),((_tmpD51->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD51)))))),_tmpD4C)));}
# 2381
break;}case 275: _LL330: {
# 2379 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2383
struct Cyc_List_List*_tmp6FC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD5B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD5A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD59;struct Cyc_Parse_Abstractdeclarator _tmpD58;yyval=Cyc_YY30(((_tmpD58.tms=((_tmpD5B=_region_malloc(yyr,sizeof(*_tmpD5B)),((_tmpD5B->hd=(void*)((_tmpD59=_region_malloc(yyr,sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5A.tag=4,((_tmpD5A.f1=_tmp6FC,((_tmpD5A.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD5A.f3=0,_tmpD5A)))))))),_tmpD59)))),((_tmpD5B->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD5B)))))),_tmpD58)));}
# 2387
break;}case 276: _LL331: {
# 2384 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2388
{struct Cyc_List_List*_tmpD65;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD64;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD63;struct Cyc_Parse_Abstractdeclarator _tmpD62;yyval=Cyc_YY30(((_tmpD62.tms=((_tmpD65=_region_malloc(yyr,sizeof(*_tmpD65)),((_tmpD65->hd=(void*)((_tmpD63=_region_malloc(yyr,sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD64.tag=5,((_tmpD64.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD64.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpD64)))))),_tmpD63)))),((_tmpD65->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD65)))))),_tmpD62)));}
# 2390
break;}case 277: _LL332: {
# 2390 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2394
yyval=yyyyvsp[0];
break;}case 278: _LL333: {
# 2391 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2395
yyval=yyyyvsp[0];
break;}case 279: _LL334: {
# 2392 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2396
yyval=yyyyvsp[0];
break;}case 280: _LL335: {
# 2393 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2397
yyval=yyyyvsp[0];
break;}case 281: _LL336: {
# 2394 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2398
yyval=yyyyvsp[0];
break;}case 282: _LL337: {
# 2395 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2399
yyval=yyyyvsp[0];
break;}case 283: _LL338: {
# 2398 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2402
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD68;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD67;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD68.tag=16,((_tmpD68.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpD68)))),_tmpD67)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 284: _LL339:
# 2402 "parse.y"
 yyval=Cyc_YY4(0);
break;case 285: _LL33A: {
# 2404 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2408
{const char*_tmpD69;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD69="open",_tag_dyneither(_tmpD69,sizeof(char),5))))!= 0){
const char*_tmpD6A;Cyc_Parse_err(((_tmpD6A="expecting `open'",_tag_dyneither(_tmpD6A,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY4((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]));
# 2412
break;}case 286: _LL33B: {
# 2413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2417
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD70;struct _dyneither_ptr*_tmpD6F;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD6E;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD70.tag=13,((_tmpD70.f1=((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD6F)))),((_tmpD70.f2=Cyc_yyget_YY9(yyyyvsp[2]),_tmpD70)))))),_tmpD6E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL33C: {
# 2417 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2421
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 288: _LL33D: {
# 2418 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2422
yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 289: _LL33E: {
# 2423 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2427
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL33F: {
# 2424 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2428
yyval=yyyyvsp[1];
break;}case 291: _LL340: {
# 2429 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2433
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 292: _LL341: {
# 2430 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2434
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1])));
break;}case 293: _LL342: {
# 2431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2435
yyval=yyyyvsp[0];
break;}case 294: _LL343: {
# 2432 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2436
yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL344: {
# 2433 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2437
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD73;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD72;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=1,((_tmpD73.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpD73)))),_tmpD72)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 296: _LL345: {
# 2436 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2440
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD76;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD75;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD76.tag=1,((_tmpD76.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpD76)))),_tmpD75)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY9(yyyyvsp[1])));}
break;}case 297: _LL346: {
# 2441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2445
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 298: _LL347: {
# 2443 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2447
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL348: {
# 2449 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2453
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL349: {
# 2452 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2456
struct Cyc_Absyn_Exp*_tmp710=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp710,Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL34A: {
# 2456 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2460
struct Cyc_Absyn_Exp*_tmp711=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp711,Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2463
break;}case 302: _LL34B: {
# 2462 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2466
yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL34C:
# 2476 "parse.y"
 yyval=Cyc_YY10(0);
break;case 304: _LL34D: {
# 2478 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2482
{struct Cyc_Absyn_Switch_clause*_tmpD79;struct Cyc_List_List*_tmpD78;yyval=Cyc_YY10(((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->hd=((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpD79->pat_vars=0,((_tmpD79->where_clause=0,((_tmpD79->body=
Cyc_yyget_YY9(yyyyvsp[2]),((_tmpD79->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD79)))))))))))),((_tmpD78->tl=0,_tmpD78)))))));}
# 2485
break;}case 305: _LL34E: {
# 2482 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2486
{struct Cyc_Absyn_Switch_clause*_tmpD7C;struct Cyc_List_List*_tmpD7B;yyval=Cyc_YY10(((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->hd=((_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD7C->pat_vars=0,((_tmpD7C->where_clause=0,((_tmpD7C->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpD7C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD7C)))))))))))),((_tmpD7B->tl=
Cyc_yyget_YY10(yyyyvsp[3]),_tmpD7B)))))));}
break;}case 306: _LL34F: {
# 2486 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2490
{struct Cyc_Absyn_Switch_clause*_tmpD7F;struct Cyc_List_List*_tmpD7E;yyval=Cyc_YY10(((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->hd=((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD7F->pat_vars=0,((_tmpD7F->where_clause=0,((_tmpD7F->body=Cyc_yyget_YY9(yyyyvsp[3]),((_tmpD7F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD7F)))))))))))),((_tmpD7E->tl=Cyc_yyget_YY10(yyyyvsp[4]),_tmpD7E)))))));}
break;}case 307: _LL350: {
# 2488 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2492
{struct Cyc_Absyn_Switch_clause*_tmpD82;struct Cyc_List_List*_tmpD81;yyval=Cyc_YY10(((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD82->pat_vars=0,((_tmpD82->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpD82->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpD82->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD82)))))))))))),((_tmpD81->tl=
Cyc_yyget_YY10(yyyyvsp[5]),_tmpD81)))))));}
break;}case 308: _LL351: {
# 2492 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2496
{struct Cyc_Absyn_Switch_clause*_tmpD85;struct Cyc_List_List*_tmpD84;yyval=Cyc_YY10(((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->hd=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD85->pat_vars=0,((_tmpD85->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpD85->body=Cyc_yyget_YY9(yyyyvsp[5]),((_tmpD85->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD85)))))))))))),((_tmpD84->tl=Cyc_yyget_YY10(yyyyvsp[6]),_tmpD84)))))));}
break;}case 309: _LL352: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2503
yyval=Cyc_YY9(Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL353: {
# 2503 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2507
yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL354: {
# 2507 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2511
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL355: {
# 2510 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2514
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL356: {
# 2513 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2517
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL357: {
# 2516 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2520
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL358: {
# 2519 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2523
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL359: {
# 2522 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2526
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL35A: {
# 2525 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2529
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL35B: {
# 2528 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2532
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY3(yyyyvsp[6]),
Cyc_yyget_YY9(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL35C: {
# 2531 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2535
struct Cyc_List_List*_tmp71C=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp71C,_tmp71D));
# 2540
break;}case 320: _LL35D: {
# 2537 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2541
struct Cyc_List_List*_tmp71E=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp71F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp71E,_tmp71F));
# 2546
break;}case 321: _LL35E: {
# 2543 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2547
struct Cyc_List_List*_tmp720=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp721=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp720,_tmp721));
# 2552
break;}case 322: _LL35F: {
# 2549 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2553
struct Cyc_List_List*_tmp722=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp723=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp722,_tmp723));
# 2558
break;}case 323: _LL360: {
# 2559 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2561
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2563
{struct _dyneither_ptr*_tmpD86;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD86)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 324: _LL361: {
# 2560 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2564
yyval=Cyc_YY9(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL362: {
# 2561 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2565
yyval=Cyc_YY9(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL363: {
# 2562 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2566
yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL364: {
# 2563 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2567
yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL365: {
# 2565 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2567
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2569
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL366: {
# 2566 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2570
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL367: {
# 2568 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2572
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL368: {
# 2577 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2581
yyval=yyyyvsp[0];
break;}case 332: _LL369: {
# 2580 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2584
yyval=yyyyvsp[0];
break;}case 333: _LL36A: {
# 2582 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2586
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 334: _LL36B: {
# 2585 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2589
yyval=yyyyvsp[0];
break;}case 335: _LL36C: {
# 2587 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2591
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 336: _LL36D: {
# 2590 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2594
yyval=yyyyvsp[0];
break;}case 337: _LL36E: {
# 2592 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2596
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 338: _LL36F: {
# 2595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599
yyval=yyyyvsp[0];
break;}case 339: _LL370: {
# 2597 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2601
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL371: {
# 2600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604
yyval=yyyyvsp[0];
break;}case 341: _LL372: {
# 2602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL373: {
# 2605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609
yyval=yyyyvsp[0];
break;}case 343: _LL374: {
# 2607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL375: {
# 2610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614
yyval=yyyyvsp[0];
break;}case 345: _LL376: {
# 2612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL377: {
# 2614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2618
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL378: {
# 2617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2621
yyval=yyyyvsp[0];
break;}case 348: _LL379: {
# 2619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2623
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL37A: {
# 2621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2625
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL37B: {
# 2623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2627
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL37C: {
# 2625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2629
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL37D: {
# 2628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2632
yyval=yyyyvsp[0];
break;}case 353: _LL37E: {
# 2630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2634
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL37F: {
# 2632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2636
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL380: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2639
yyval=yyyyvsp[0];
break;}case 356: _LL381: {
# 2637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2641
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL382: {
# 2639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2643
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL383: {
# 2642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2646
yyval=yyyyvsp[0];
break;}case 359: _LL384: {
# 2644 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL385: {
# 2646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL386: {
# 2648 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL387: {
# 2651 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2655
yyval=yyyyvsp[0];
break;}case 363: _LL388: {
# 2653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2657
void*_tmp725=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp725,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2660
break;}case 364: _LL389: {
# 2659 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2663
yyval=yyyyvsp[0];
break;}case 365: _LL38A: {
# 2662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2666
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
break;}case 366: _LL38B: {
# 2664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2668
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL38C: {
# 2666 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2670
void*_tmp726=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp726,(unsigned int)(yyyylsp[0]).first_line)));
# 2673
break;}case 368: _LL38D: {
# 2670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2674
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL38E: {
# 2672 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2676
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD8C;struct _dyneither_ptr*_tmpD8B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD8A;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,(void*)(
(_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A[0]=((_tmpD8C.tag=0,((_tmpD8C.f1=((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpD8B)))),_tmpD8C)))),_tmpD8A)))),(unsigned int)(yyyylsp[0]).first_line)));}
break;}case 370: _LL38F: {
# 2676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2680
void*_tmp72A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpD8F;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpD8E;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp(_tmp72A,(void*)((_tmpD8E=_cycalloc_atomic(sizeof(*_tmpD8E)),((_tmpD8E[0]=((_tmpD8F.tag=1,((_tmpD8F.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpD8F)))),_tmpD8E)))),(unsigned int)(yyyylsp[0]).first_line)));}
# 2683
break;}case 371: _LL390: {
# 2683 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2687
yyval=yyyyvsp[0];
break;}case 372: _LL391: {
# 2692 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2696
yyval=yyyyvsp[0];
break;}case 373: _LL392: {
# 2697 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2701
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 374: _LL393: {
# 2699 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2703
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
break;}case 375: _LL394: {
# 2701 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2705
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyyyvsp[0]);
{void*_tmp72D=e->r;enum Cyc_Absyn_Sign _tmp72F;char _tmp730;enum Cyc_Absyn_Sign _tmp732;short _tmp733;enum Cyc_Absyn_Sign _tmp735;int _tmp736;struct _dyneither_ptr _tmp738;int _tmp739;_LL397: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp72E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp72E->tag != 0)goto _LL399;else{if(((_tmp72E->f1).Char_c).tag != 2)goto _LL399;_tmp72F=((struct _tuple3)((_tmp72E->f1).Char_c).val).f1;_tmp730=((struct _tuple3)((_tmp72E->f1).Char_c).val).f2;}}_LL398:
# 2709
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpD92;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpD91;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD91=_cycalloc_atomic(sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD92.tag=10,((_tmpD92.f1=_tmp730,_tmpD92)))),_tmpD91)))),e->loc));}goto _LL396;_LL399: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp731=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp731->tag != 0)goto _LL39B;else{if(((_tmp731->f1).Short_c).tag != 4)goto _LL39B;_tmp732=((struct _tuple4)((_tmp731->f1).Short_c).val).f1;_tmp733=((struct _tuple4)((_tmp731->f1).Short_c).val).f2;}}_LL39A:
# 2711
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD95;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD94;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD94=_cycalloc_atomic(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD95.tag=9,((_tmpD95.f1=_tmp732,((_tmpD95.f2=(int)_tmp733,_tmpD95)))))),_tmpD94)))),e->loc));}goto _LL396;_LL39B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp734=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp734->tag != 0)goto _LL39D;else{if(((_tmp734->f1).Int_c).tag != 5)goto _LL39D;_tmp735=((struct _tuple5)((_tmp734->f1).Int_c).val).f1;_tmp736=((struct _tuple5)((_tmp734->f1).Int_c).val).f2;}}_LL39C:
# 2713
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD98;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD97;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD97=_cycalloc_atomic(sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD98.tag=9,((_tmpD98.f1=_tmp735,((_tmpD98.f2=_tmp736,_tmpD98)))))),_tmpD97)))),e->loc));}goto _LL396;_LL39D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp737=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp737->tag != 0)goto _LL39F;else{if(((_tmp737->f1).Float_c).tag != 7)goto _LL39F;_tmp738=((struct _tuple7)((_tmp737->f1).Float_c).val).f1;_tmp739=((struct _tuple7)((_tmp737->f1).Float_c).val).f2;}}_LL39E:
# 2715
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpD9B;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpD9A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9B.tag=11,((_tmpD9B.f1=_tmp738,((_tmpD9B.f2=_tmp739,_tmpD9B)))))),_tmpD9A)))),e->loc));}goto _LL396;_LL39F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp73A->tag != 0)goto _LL3A1;else{if(((_tmp73A->f1).Null_c).tag != 1)goto _LL3A1;}}_LL3A0:
# 2717
 yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL396;_LL3A1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp73B->tag != 0)goto _LL3A3;else{if(((_tmp73B->f1).String_c).tag != 8)goto _LL3A3;}}_LL3A2:
# 2719
{const char*_tmpD9C;Cyc_Parse_err(((_tmpD9C="strings cannot occur within patterns",_tag_dyneither(_tmpD9C,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp73C->tag != 0)goto _LL3A5;else{if(((_tmp73C->f1).Wstring_c).tag != 9)goto _LL3A5;}}_LL3A4:
# 2721
{const char*_tmpD9D;Cyc_Parse_err(((_tmpD9D="strings cannot occur within patterns",_tag_dyneither(_tmpD9D,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp72D;if(_tmp73D->tag != 0)goto _LL3A7;else{if(((_tmp73D->f1).LongLong_c).tag != 6)goto _LL3A7;}}_LL3A6:
# 2723
{const char*_tmpD9E;Cyc_Parse_unimp(((_tmpD9E="long long's in patterns",_tag_dyneither(_tmpD9E,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL396;_LL3A7:;_LL3A8: {
# 2725
const char*_tmpD9F;Cyc_Parse_err(((_tmpD9F="bad constant in case",_tag_dyneither(_tmpD9F,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL396:;}
# 2728
break;}case 376: _LL395: {
# 2725 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2729
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDA2;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDA1;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA2.tag=14,((_tmpDA2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDA2)))),_tmpDA1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 377: _LL3A9: {
# 2727 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2731
{const char*_tmpDA3;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDA3="as",_tag_dyneither(_tmpDA3,sizeof(char),3))))!= 0){
const char*_tmpDA4;Cyc_Parse_err(((_tmpDA4="expecting `as'",_tag_dyneither(_tmpDA4,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDAE;struct _dyneither_ptr*_tmpDAD;struct _tuple0*_tmpDAC;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDAB;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB[0]=((_tmpDAE.tag=1,((_tmpDAE.f1=Cyc_Absyn_new_vardecl(((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->f1=Cyc_Absyn_Loc_n,((_tmpDAC->f2=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDAD)))),_tmpDAC)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDAE.f2=
Cyc_yyget_YY11(yyyyvsp[2]),_tmpDAE)))))),_tmpDAB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2736
break;}case 378: _LL3AA: {
# 2733 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2737
struct Cyc_List_List*_tmp753;int _tmp754;struct _tuple21 _tmp752=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp753=_tmp752.f1;_tmp754=_tmp752.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDB1;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDB0;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDB1.tag=4,((_tmpDB1.f1=_tmp753,((_tmpDB1.f2=_tmp754,_tmpDB1)))))),_tmpDB0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2740
break;}case 379: _LL3AB: {
# 2737 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2741
struct Cyc_List_List*_tmp758;int _tmp759;struct _tuple21 _tmp757=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp758=_tmp757.f1;_tmp759=_tmp757.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDB4;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDB3;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB4.tag=15,((_tmpDB4.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDB4.f2=_tmp758,((_tmpDB4.f3=_tmp759,_tmpDB4)))))))),_tmpDB3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2744
break;}case 380: _LL3AC: {
# 2741 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2745
struct Cyc_List_List*_tmp75D;int _tmp75E;struct _tuple21 _tmp75C=*Cyc_yyget_YY16(yyyyvsp[3]);_tmp75D=_tmp75C.f1;_tmp75E=_tmp75C.f2;{
struct Cyc_List_List*_tmp75F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDBA;struct Cyc_Absyn_AggrInfo*_tmpDB9;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDB8;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDBA.tag=6,((_tmpDBA.f1=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDB9->targs=0,_tmpDB9)))))),((_tmpDBA.f2=_tmp75F,((_tmpDBA.f3=_tmp75D,((_tmpDBA.f4=_tmp75E,_tmpDBA)))))))))),_tmpDB8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2750
break;};}case 381: _LL3AD: {
# 2747 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751
struct Cyc_List_List*_tmp764;int _tmp765;struct _tuple21 _tmp763=*Cyc_yyget_YY16(yyyyvsp[2]);_tmp764=_tmp763.f1;_tmp765=_tmp763.f2;{
struct Cyc_List_List*_tmp766=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDBD;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDBC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=6,((_tmpDBD.f1=0,((_tmpDBD.f2=_tmp766,((_tmpDBD.f3=_tmp764,((_tmpDBD.f4=_tmp765,_tmpDBD)))))))))),_tmpDBC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2755
break;};}case 382: _LL3AE: {
# 2752 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2756
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC0;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDBF;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=5,((_tmpDC0.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpDC0)))),_tmpDBF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3AF: {
# 2754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2758
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDCA;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC9;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC8;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC7;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDCA.tag=5,((_tmpDCA.f1=Cyc_Absyn_new_pat((void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=5,((_tmpDC9.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpDC9)))),_tmpDC8)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDCA)))),_tmpDC7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 384: _LL3B0: {
# 2756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2760
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDD4;struct _dyneither_ptr*_tmpDD3;struct _tuple0*_tmpDD2;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDD1;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD4.tag=2,((_tmpDD4.f1=Cyc_Absyn_new_vardecl(((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->f1=Cyc_Absyn_Loc_n,((_tmpDD2->f2=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDD3)))),_tmpDD2)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD4.f2=
# 2762
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDD4)))))),_tmpDD1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2764
break;}case 385: _LL3B1: {
# 2761 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2765
{const char*_tmpDD5;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDD5="as",_tag_dyneither(_tmpDD5,sizeof(char),3))))!= 0){
const char*_tmpDD6;Cyc_Parse_err(((_tmpDD6="expecting `as'",_tag_dyneither(_tmpDD6,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDE0;struct _dyneither_ptr*_tmpDDF;struct _tuple0*_tmpDDE;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDDD;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDE0.tag=2,((_tmpDE0.f1=Cyc_Absyn_new_vardecl(((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->f1=Cyc_Absyn_Loc_n,((_tmpDDE->f2=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDDF)))),_tmpDDE)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDE0.f2=
# 2769
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDE0)))))),_tmpDDD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2771
break;}case 386: _LL3B2: {
# 2768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2772
void*_tmp779=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpDF1;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDF0;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDEF;struct _dyneither_ptr*_tmpDEE;struct _tuple0*_tmpDED;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpDEC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[0]=((_tmpDF1.tag=3,((_tmpDF1.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp779),((_tmpDF1.f2=
Cyc_Absyn_new_vardecl(((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->f1=Cyc_Absyn_Loc_n,((_tmpDED->f2=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDEE)))),_tmpDED)))))),(void*)(
(_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF0.tag=19,((_tmpDF0.f1=(void*)_tmp779,_tmpDF0)))),_tmpDEF)))),0),_tmpDF1)))))),_tmpDEC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2777
break;}case 387: _LL3B3: {
# 2774 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2778
struct Cyc_Absyn_Tvar*_tmp780=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE0D;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE0B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE0A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE09;struct _dyneither_ptr*_tmpE08;struct _tuple0*_tmpE07;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE06;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE0D.tag=3,((_tmpE0D.f1=_tmp780,((_tmpE0D.f2=
Cyc_Absyn_new_vardecl(((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->f1=Cyc_Absyn_Loc_n,((_tmpE07->f2=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE08)))),_tmpE07)))))),(void*)(
(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE0C.tag=19,((_tmpE0C.f1=(void*)((void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=2,((_tmpE0B.f1=_tmp780,_tmpE0B)))),_tmpE0A))))),_tmpE0C)))),_tmpE09)))),0),_tmpE0D)))))),_tmpE06)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2783
break;}case 388: _LL3B4: {
# 2782 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2786
{struct _tuple21*_tmpE0E;yyval=Cyc_YY12(((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE0E->f2=0,_tmpE0E)))))));}
break;}case 389: _LL3B5: {
# 2783 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2787
{struct _tuple21*_tmpE0F;yyval=Cyc_YY12(((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE0F->f2=1,_tmpE0F)))))));}
break;}case 390: _LL3B6: {
# 2784 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2788
{struct _tuple21*_tmpE10;yyval=Cyc_YY12(((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->f1=0,((_tmpE10->f2=1,_tmpE10)))))));}
break;}case 391: _LL3B7: {
# 2789 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2793
{struct Cyc_List_List*_tmpE11;yyval=Cyc_YY13(((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->hd=Cyc_yyget_YY11(yyyyvsp[0]),((_tmpE11->tl=0,_tmpE11)))))));}
break;}case 392: _LL3B8: {
# 2791 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2795
{struct Cyc_List_List*_tmpE12;yyval=Cyc_YY13(((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=Cyc_yyget_YY11(yyyyvsp[2]),((_tmpE12->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpE12)))))));}
break;}case 393: _LL3B9: {
# 2796 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2800
{struct _tuple22*_tmpE13;yyval=Cyc_YY14(((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->f1=0,((_tmpE13->f2=Cyc_yyget_YY11(yyyyvsp[0]),_tmpE13)))))));}
break;}case 394: _LL3BA: {
# 2798 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2802
{struct _tuple22*_tmpE14;yyval=Cyc_YY14(((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpE14->f2=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE14)))))));}
break;}case 395: _LL3BB: {
# 2801 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2805
{struct _tuple21*_tmpE15;yyval=Cyc_YY16(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE15->f2=0,_tmpE15)))))));}
break;}case 396: _LL3BC: {
# 2802 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2806
{struct _tuple21*_tmpE16;yyval=Cyc_YY16(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE16->f2=1,_tmpE16)))))));}
break;}case 397: _LL3BD: {
# 2803 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2807
{struct _tuple21*_tmpE17;yyval=Cyc_YY16(((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->f1=0,((_tmpE17->f2=1,_tmpE17)))))));}
break;}case 398: _LL3BE: {
# 2808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2812
{struct Cyc_List_List*_tmpE18;yyval=Cyc_YY15(((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=Cyc_yyget_YY14(yyyyvsp[0]),((_tmpE18->tl=0,_tmpE18)))))));}
break;}case 399: _LL3BF: {
# 2810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2814
{struct Cyc_List_List*_tmpE19;yyval=Cyc_YY15(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->hd=Cyc_yyget_YY14(yyyyvsp[2]),((_tmpE19->tl=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE19)))))));}
break;}case 400: _LL3C0: {
# 2816 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820
yyval=yyyyvsp[0];
break;}case 401: _LL3C1: {
# 2818 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2822
yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 402: _LL3C2: {
# 2823 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2827
yyval=yyyyvsp[0];
break;}case 403: _LL3C3: {
# 2825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2829
yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY8(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 404: _LL3C4: {
# 2827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2831
yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 405: _LL3C5: {
# 2831 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2835
yyval=Cyc_YY8(0);
break;}case 406: _LL3C6: {
# 2832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2836
{struct Cyc_Core_Opt*_tmpE1A;yyval=Cyc_YY8(((_tmpE1A=_cycalloc_atomic(sizeof(*_tmpE1A)),((_tmpE1A->v=(void*)Cyc_Absyn_Times,_tmpE1A)))));}
break;}case 407: _LL3C7: {
# 2833 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2837
{struct Cyc_Core_Opt*_tmpE1B;yyval=Cyc_YY8(((_tmpE1B=_cycalloc_atomic(sizeof(*_tmpE1B)),((_tmpE1B->v=(void*)Cyc_Absyn_Div,_tmpE1B)))));}
break;}case 408: _LL3C8: {
# 2834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2838
{struct Cyc_Core_Opt*_tmpE1C;yyval=Cyc_YY8(((_tmpE1C=_cycalloc_atomic(sizeof(*_tmpE1C)),((_tmpE1C->v=(void*)Cyc_Absyn_Mod,_tmpE1C)))));}
break;}case 409: _LL3C9: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2839
{struct Cyc_Core_Opt*_tmpE1D;yyval=Cyc_YY8(((_tmpE1D=_cycalloc_atomic(sizeof(*_tmpE1D)),((_tmpE1D->v=(void*)Cyc_Absyn_Plus,_tmpE1D)))));}
break;}case 410: _LL3CA: {
# 2836 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2840
{struct Cyc_Core_Opt*_tmpE1E;yyval=Cyc_YY8(((_tmpE1E=_cycalloc_atomic(sizeof(*_tmpE1E)),((_tmpE1E->v=(void*)Cyc_Absyn_Minus,_tmpE1E)))));}
break;}case 411: _LL3CB: {
# 2837 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2841
{struct Cyc_Core_Opt*_tmpE1F;yyval=Cyc_YY8(((_tmpE1F=_cycalloc_atomic(sizeof(*_tmpE1F)),((_tmpE1F->v=(void*)Cyc_Absyn_Bitlshift,_tmpE1F)))));}
break;}case 412: _LL3CC: {
# 2838 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2842
{struct Cyc_Core_Opt*_tmpE20;yyval=Cyc_YY8(((_tmpE20=_cycalloc_atomic(sizeof(*_tmpE20)),((_tmpE20->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE20)))));}
break;}case 413: _LL3CD: {
# 2839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2843
{struct Cyc_Core_Opt*_tmpE21;yyval=Cyc_YY8(((_tmpE21=_cycalloc_atomic(sizeof(*_tmpE21)),((_tmpE21->v=(void*)Cyc_Absyn_Bitand,_tmpE21)))));}
break;}case 414: _LL3CE: {
# 2840 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2844
{struct Cyc_Core_Opt*_tmpE22;yyval=Cyc_YY8(((_tmpE22=_cycalloc_atomic(sizeof(*_tmpE22)),((_tmpE22->v=(void*)Cyc_Absyn_Bitxor,_tmpE22)))));}
break;}case 415: _LL3CF: {
# 2841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2845
{struct Cyc_Core_Opt*_tmpE23;yyval=Cyc_YY8(((_tmpE23=_cycalloc_atomic(sizeof(*_tmpE23)),((_tmpE23->v=(void*)Cyc_Absyn_Bitor,_tmpE23)))));}
break;}case 416: _LL3D0: {
# 2846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850
yyval=yyyyvsp[0];
break;}case 417: _LL3D1: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2852
yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 418: _LL3D2: {
# 2851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2855
yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3D3: {
# 2854 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2858
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 420: _LL3D4: {
# 2856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2860
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3D5: {
# 2858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2862
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3D6: {
# 2860 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2864
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3D7: {
# 2864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2868
yyval=yyyyvsp[0];
break;}case 424: _LL3D8: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2872
yyval=yyyyvsp[0];
break;}case 425: _LL3D9: {
# 2870 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2874
yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3DA: {
# 2874 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2878
yyval=yyyyvsp[0];
break;}case 427: _LL3DB: {
# 2876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2880
yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3DC: {
# 2880 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2884
yyval=yyyyvsp[0];
break;}case 429: _LL3DD: {
# 2882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2886
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3DE: {
# 2886 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2890
yyval=yyyyvsp[0];
break;}case 431: _LL3DF: {
# 2888 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2892
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3E0: {
# 2892 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2896
yyval=yyyyvsp[0];
break;}case 433: _LL3E1: {
# 2894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2898
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 434: _LL3E2: {
# 2898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2902
yyval=yyyyvsp[0];
break;}case 435: _LL3E3: {
# 2900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2904
yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3E4: {
# 2902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906
yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3E5: {
# 2906 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910
yyval=yyyyvsp[0];
break;}case 438: _LL3E6: {
# 2908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2912
yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3E7: {
# 2910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2914
yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3E8: {
# 2912 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2916
yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3E9: {
# 2914 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2918
yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3EA: {
# 2918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2922
yyval=yyyyvsp[0];
break;}case 443: _LL3EB: {
# 2920 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2924
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3EC: {
# 2922 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3ED: {
# 2926 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930
yyval=yyyyvsp[0];
break;}case 446: _LL3EE: {
# 2928 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2932
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3EF: {
# 2930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2934
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3F0: {
# 2934 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938
yyval=yyyyvsp[0];
break;}case 449: _LL3F1: {
# 2936 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2940
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3F2: {
# 2938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2942
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3F3: {
# 2940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2944
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3F4: {
# 2944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2948
yyval=yyyyvsp[0];
break;}case 453: _LL3F5: {
# 2946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2950
void*_tmp79F=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_cast_exp(_tmp79F,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2953
break;}case 454: _LL3F6: {
# 2952 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956
yyval=yyyyvsp[0];
break;}case 455: _LL3F7: {
# 2953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2957
yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL3F8: {
# 2954 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2958
yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL3F9: {
# 2955 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2959
yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL3FA: {
# 2956 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2960
yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL3FB: {
# 2957 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2961
yyval=yyyyvsp[1];
break;}case 460: _LL3FC: {
# 2958 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2962
yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL3FD: {
# 2960 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2964
void*_tmp7A0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp(_tmp7A0,(unsigned int)(yyyylsp[0]).first_line));
# 2967
break;}case 462: _LL3FE: {
# 2963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2967
yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL3FF: {
# 2965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2969
void*_tmp7A1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE29;struct _dyneither_ptr*_tmpE28;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE27;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7A1,(void*)((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27[0]=((_tmpE29.tag=0,((_tmpE29.f1=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE28)))),_tmpE29)))),_tmpE27)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2972
break;}case 464: _LL400: {
# 2970 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2974
void*_tmp7A5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE2C;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE2B;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7A5,(void*)((_tmpE2B=_cycalloc_atomic(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2C.tag=1,((_tmpE2C.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpE2C)))),_tmpE2B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2977
break;}case 465: _LL401: {
# 2975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2979
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE32;struct Cyc_Absyn_MallocInfo _tmpE31;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE30;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE32.tag=32,((_tmpE32.f1=((_tmpE31.is_calloc=0,((_tmpE31.rgn=0,((_tmpE31.elt_type=0,((_tmpE31.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE31.fat_result=0,_tmpE31)))))))))),_tmpE32)))),_tmpE30)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2981
break;}case 466: _LL402: {
# 2978 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2982
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE38;struct Cyc_Absyn_MallocInfo _tmpE37;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE36;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=((_tmpE38.tag=32,((_tmpE38.f1=((_tmpE37.is_calloc=0,((_tmpE37.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE37.elt_type=0,((_tmpE37.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE37.fat_result=0,_tmpE37)))))))))),_tmpE38)))),_tmpE36)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2984
break;}case 467: _LL403: {
# 2981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2985
void*_tmp7AE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE42;void**_tmpE41;struct Cyc_Absyn_MallocInfo _tmpE40;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE3F;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE42.tag=32,((_tmpE42.f1=((_tmpE40.is_calloc=1,((_tmpE40.rgn=0,((_tmpE40.elt_type=((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=_tmp7AE,_tmpE41)))),((_tmpE40.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE40.fat_result=0,_tmpE40)))))))))),_tmpE42)))),_tmpE3F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2988
break;}case 468: _LL404: {
# 2986 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2990
void*_tmp7B3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE4C;void**_tmpE4B;struct Cyc_Absyn_MallocInfo _tmpE4A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE49;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4C.tag=32,((_tmpE4C.f1=((_tmpE4A.is_calloc=1,((_tmpE4A.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE4A.elt_type=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B[0]=_tmp7B3,_tmpE4B)))),((_tmpE4A.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE4A.fat_result=0,_tmpE4A)))))))))),_tmpE4C)))),_tmpE49)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2993
break;}case 469: _LL405: {
# 2990 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2994
{struct Cyc_Absyn_Exp*_tmpE4D[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE4D[0]=Cyc_yyget_YY3(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE4D,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 470: _LL406: {
# 2992 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2996
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE53;struct _dyneither_ptr*_tmpE52;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE51;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE53.tag=36,((_tmpE53.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE53.f2=((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE52)))),_tmpE53)))))),_tmpE51)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 471: _LL407: {
# 2994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2998
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE59;struct _dyneither_ptr*_tmpE58;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE57;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE59.tag=36,((_tmpE59.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE59.f2=((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE58)))),_tmpE59)))))),_tmpE57)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 472: _LL408: {
# 2996 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3000
void*_tmp7BF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp7BF,(unsigned int)(yyyylsp[0]).first_line));
break;}case 473: _LL409: {
# 2999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3003
int _tmp7C1;struct _dyneither_ptr _tmp7C2;struct _tuple19 _tmp7C0=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp7C1=_tmp7C0.f1;_tmp7C2=_tmp7C0.f2;
yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp7C1,_tmp7C2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 474: _LL40A: {
# 3004 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3008
yyval=Cyc_YY7(Cyc_Absyn_Bitnot);
break;}case 475: _LL40B: {
# 3005 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3009
yyval=Cyc_YY7(Cyc_Absyn_Not);
break;}case 476: _LL40C: {
# 3006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3010
yyval=Cyc_YY7(Cyc_Absyn_Minus);
break;}case 477: _LL40D: {
# 3011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3015
yyval=yyyyvsp[0];
break;}case 478: _LL40E: {
# 3013 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3017
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL40F: {
# 3015 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3019
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL410: {
# 3017 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3021
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL411: {
# 3019 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3023
{struct _dyneither_ptr*_tmpE5A;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE5A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 482: _LL412: {
# 3021 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3025
{struct _dyneither_ptr*_tmpE5B;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE5B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 483: _LL413: {
# 3023 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3027
yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL414: {
# 3025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3029
yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL415: {
# 3027 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3031
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE5D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=((_tmpE5E.tag=24,((_tmpE5E.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpE5E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpE5E)))))),_tmpE5D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL416: {
# 3029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3033
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE61;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE60;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=24,((_tmpE61.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpE61.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpE61)))))),_tmpE60)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL417: {
# 3035 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3039
yyval=Cyc_YY3(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL418: {
# 3037 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3041
yyval=yyyyvsp[0];
break;}case 489: _LL419: {
# 3039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3043
yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL41A: {
# 3041 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3045
yyval=Cyc_YY3(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL41B: {
# 3043 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3047
yyval=yyyyvsp[1];
break;}case 492: _LL41C: {
# 3048 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3052
yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 493: _LL41D: {
# 3050 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3054
yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL41E: {
# 3053 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3057
yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 495: _LL41F: {
# 3056 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3060
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE64;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE63;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=27,((_tmpE64.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE64.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpE64.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[3])),((_tmpE64.f4=0,_tmpE64)))))))))),_tmpE63)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 496: _LL420: {
# 3059 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3063
yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL421: {
# 3063 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3067
yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[0])));
break;}case 498: _LL422: {
# 3069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3073
{struct Cyc_List_List*_tmpE65;yyval=Cyc_YY5(((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65->hd=Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE65->tl=0,_tmpE65)))))));}
break;}case 499: _LL423: {
# 3071 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3075
{struct Cyc_List_List*_tmpE66;yyval=Cyc_YY5(((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->hd=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE66->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpE66)))))));}
break;}case 500: _LL424: {
# 3077 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3081
struct _tuple5 _tmp7CD=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY3(Cyc_Absyn_int_exp(_tmp7CD.f1,_tmp7CD.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL425: {
# 3078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3082
yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL426: {
# 3079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3083
yyval=Cyc_YY3(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL427: {
# 3080 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3085
struct _dyneither_ptr _tmp7CE=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7CE);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7CE,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3093
yyval=Cyc_YY3(Cyc_Absyn_float_exp(_tmp7CE,i,(unsigned int)(yyyylsp[0]).first_line));
# 3095
break;}case 504: _LL428: {
# 3092 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3096
yyval=Cyc_YY3(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL429: {
# 3096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3100
{struct _dyneither_ptr*_tmpE69;struct _tuple0*_tmpE68;yyval=Cyc_QualId_tok(((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68->f1=Cyc_Absyn_Rel_n(0),((_tmpE68->f2=((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE69)))),_tmpE68)))))));}
break;}case 506: _LL42A: {
# 3097 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101
yyval=yyyyvsp[0];
break;}case 507: _LL42B: {
# 3100 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3104
{struct _dyneither_ptr*_tmpE6C;struct _tuple0*_tmpE6B;yyval=Cyc_QualId_tok(((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->f1=Cyc_Absyn_Rel_n(0),((_tmpE6B->f2=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE6C)))),_tmpE6B)))))));}
break;}case 508: _LL42C: {
# 3101 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3105
yyval=yyyyvsp[0];
break;}case 509: _LL42D: {
# 3106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3110
yyval=yyyyvsp[0];
break;}case 510: _LL42E: {
# 3107 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3111
yyval=yyyyvsp[0];
break;}case 511: _LL42F: {
# 3110 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3114
yyval=yyyyvsp[0];
break;}case 512: _LL430: {
# 3111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3115
yyval=yyyyvsp[0];
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
break;}default: _LL433:
# 3125
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(515,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(143,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6375) && Cyc_yycheck[_check_known_subscript_notnull(6376,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
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
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6376,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)])+ 15,count ++);}
{unsigned int _tmpE71;unsigned int _tmpE70;struct _dyneither_ptr _tmpE6F;char*_tmpE6E;unsigned int _tmpE6D;msg=((_tmpE6D=(unsigned int)(sze + 15),((_tmpE6E=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpE6D + 1)),((_tmpE6F=_tag_dyneither(_tmpE6E,sizeof(char),_tmpE6D + 1),((((_tmpE70=_tmpE6D,((_tmpE72(& _tmpE71,& _tmpE70,& _tmpE6E),_tmpE6E[_tmpE70]=(char)0)))),_tmpE6F))))))));}
{const char*_tmpE73;Cyc_strcpy(msg,((_tmpE73="parse error",_tag_dyneither(_tmpE73,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6376,x + yyn)]== x){
# 453
{const char*_tmpE75;const char*_tmpE74;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpE75=", expecting `",_tag_dyneither(_tmpE75,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpE74=" or `",_tag_dyneither(_tmpE74,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)]);
{const char*_tmpE76;Cyc_strcat(msg,((_tmpE76="'",_tag_dyneither(_tmpE76,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpE77;Cyc_yyerror(((_tmpE77="parse error",_tag_dyneither(_tmpE77,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7DC=1;_npop_handler(0);return _tmp7DC;}
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
 if(yyssp_offset == 0){int _tmp7DD=1;_npop_handler(0);return _tmp7DD;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6375) || Cyc_yycheck[_check_known_subscript_notnull(6376,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6376,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1029){
int _tmp7DE=0;_npop_handler(0);return _tmp7DE;}
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
union Cyc_YYSTYPE _tmp7E8=v;int _tmp7E9;char _tmp7EA;struct _dyneither_ptr _tmp7EB;union Cyc_Absyn_Nmspace _tmp7EC;struct _dyneither_ptr*_tmp7ED;_LL436: if((_tmp7E8.Int_tok).tag != 1)goto _LL438;_tmp7E9=((struct _tuple5)(_tmp7E8.Int_tok).val).f2;_LL437:
{const char*_tmpE7B;void*_tmpE7A[1];struct Cyc_Int_pa_PrintArg_struct _tmpE79;(_tmpE79.tag=1,((_tmpE79.f1=(unsigned long)_tmp7E9,((_tmpE7A[0]=& _tmpE79,Cyc_fprintf(Cyc_stderr,((_tmpE7B="%d",_tag_dyneither(_tmpE7B,sizeof(char),3))),_tag_dyneither(_tmpE7A,sizeof(void*),1)))))));}goto _LL435;_LL438: if((_tmp7E8.Char_tok).tag != 2)goto _LL43A;_tmp7EA=(char)(_tmp7E8.Char_tok).val;_LL439:
{const char*_tmpE7F;void*_tmpE7E[1];struct Cyc_Int_pa_PrintArg_struct _tmpE7D;(_tmpE7D.tag=1,((_tmpE7D.f1=(unsigned long)((int)_tmp7EA),((_tmpE7E[0]=& _tmpE7D,Cyc_fprintf(Cyc_stderr,((_tmpE7F="%c",_tag_dyneither(_tmpE7F,sizeof(char),3))),_tag_dyneither(_tmpE7E,sizeof(void*),1)))))));}goto _LL435;_LL43A: if((_tmp7E8.String_tok).tag != 3)goto _LL43C;_tmp7EB=(struct _dyneither_ptr)(_tmp7E8.String_tok).val;_LL43B:
{const char*_tmpE83;void*_tmpE82[1];struct Cyc_String_pa_PrintArg_struct _tmpE81;(_tmpE81.tag=0,((_tmpE81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7EB),((_tmpE82[0]=& _tmpE81,Cyc_fprintf(Cyc_stderr,((_tmpE83="\"%s\"",_tag_dyneither(_tmpE83,sizeof(char),5))),_tag_dyneither(_tmpE82,sizeof(void*),1)))))));}goto _LL435;_LL43C: if((_tmp7E8.QualId_tok).tag != 5)goto _LL43E;_tmp7EC=(*((struct _tuple0*)(_tmp7E8.QualId_tok).val)).f1;_tmp7ED=(*((struct _tuple0*)(_tmp7E8.QualId_tok).val)).f2;_LL43D: {
# 3125
struct Cyc_List_List*_tmp7F7=0;
{union Cyc_Absyn_Nmspace _tmp7F8=_tmp7EC;struct Cyc_List_List*_tmp7F9;struct Cyc_List_List*_tmp7FA;struct Cyc_List_List*_tmp7FB;_LL441: if((_tmp7F8.Rel_n).tag != 1)goto _LL443;_tmp7F9=(struct Cyc_List_List*)(_tmp7F8.Rel_n).val;_LL442:
 _tmp7F7=_tmp7F9;goto _LL440;_LL443: if((_tmp7F8.Abs_n).tag != 2)goto _LL445;_tmp7FA=(struct Cyc_List_List*)(_tmp7F8.Abs_n).val;_LL444:
 _tmp7F7=_tmp7FA;goto _LL440;_LL445: if((_tmp7F8.C_n).tag != 3)goto _LL447;_tmp7FB=(struct Cyc_List_List*)(_tmp7F8.C_n).val;_LL446:
 _tmp7F7=_tmp7FB;goto _LL440;_LL447: if((_tmp7F8.Loc_n).tag != 4)goto _LL440;_LL448:
 goto _LL440;_LL440:;}
# 3132
for(0;_tmp7F7 != 0;_tmp7F7=_tmp7F7->tl){
const char*_tmpE87;void*_tmpE86[1];struct Cyc_String_pa_PrintArg_struct _tmpE85;(_tmpE85.tag=0,((_tmpE85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp7F7->hd)),((_tmpE86[0]=& _tmpE85,Cyc_fprintf(Cyc_stderr,((_tmpE87="%s::",_tag_dyneither(_tmpE87,sizeof(char),5))),_tag_dyneither(_tmpE86,sizeof(void*),1)))))));}
{const char*_tmpE8B;void*_tmpE8A[1];struct Cyc_String_pa_PrintArg_struct _tmpE89;(_tmpE89.tag=0,((_tmpE89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7ED),((_tmpE8A[0]=& _tmpE89,Cyc_fprintf(Cyc_stderr,((_tmpE8B="%s::",_tag_dyneither(_tmpE8B,sizeof(char),5))),_tag_dyneither(_tmpE8A,sizeof(void*),1)))))));}
goto _LL435;}_LL43E:;_LL43F:
{const char*_tmpE8E;void*_tmpE8D;(_tmpE8D=0,Cyc_fprintf(Cyc_stderr,((_tmpE8E="?",_tag_dyneither(_tmpE8E,sizeof(char),2))),_tag_dyneither(_tmpE8D,sizeof(void*),0)));}goto _LL435;_LL435:;}
# 3141
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp804=_new_region("yyr");struct _RegionHandle*yyr=& _tmp804;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp805=Cyc_Parse_parse_result;_npop_handler(0);return _tmp805;};
# 3144
;_pop_region(yyr);};}
