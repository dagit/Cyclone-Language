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
# 256 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 261
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 268
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
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 886
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 946
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 948
extern void*Cyc_Absyn_sint_typ;
# 950
void*Cyc_Absyn_float_typ(int);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1047
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
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1077
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1096
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1107
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1114
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1123
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1128
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1134
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1157
int Cyc_Absyn_fntype_att(void*a);
# 1177
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 309 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
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
struct _tuple11 _stmttmp0=ds->hd;struct Cyc_Parse_Declarator _tmp1;struct Cyc_Absyn_Exp*_tmp2;struct _tuple11 _tmp0=_stmttmp0;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
{struct _tuple10*_tmp828;declarators=((_tmp828=_region_malloc(r,sizeof(*_tmp828)),((_tmp828->tl=declarators,((_tmp828->hd=_tmp1,_tmp828))))));}{
struct Cyc_List_List*_tmp829;exprs=((_tmp829=_region_malloc(r,sizeof(*_tmp829)),((_tmp829->hd=_tmp2,((_tmp829->tl=exprs,_tmp829))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp82C;struct Cyc_Position_Exit_exn_struct*_tmp82B;(int)_throw((void*)((_tmp82B=_cycalloc_atomic(sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp82C.tag=Cyc_Position_Exit,_tmp82C)),_tmp82B)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp831;void*_tmp830[2];struct Cyc_String_pa_PrintArg_struct _tmp82F;struct Cyc_String_pa_PrintArg_struct _tmp82E;(_tmp82E.tag=0,((_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp830[0]=& _tmp82F,((_tmp830[1]=& _tmp82E,Cyc_fprintf(Cyc_stderr,((_tmp831="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp831,sizeof(char),46))),_tag_dyneither(_tmp830,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp834;void*_tmp833;(_tmp833=0,Cyc_Tcutil_warn(loc,((_tmp834="qualifier on type is ignored",_tag_dyneither(_tmp834,sizeof(char),29))),_tag_dyneither(_tmp833,sizeof(void*),0)));};}
# 199
return _tmpD;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 202
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 208
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_stmttmp1=(void*)pqs->hd;void*_tmp10=_stmttmp1;struct Cyc_Absyn_Exp*_tmp18;void*_tmp1A;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp11=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp11->tag != 4)goto _LL3;}_LL2:
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp837;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp836;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp836=_cycalloc(sizeof(*_tmp836)),((_tmp836[0]=((_tmp837.tag=1,((_tmp837.f1=_tmp18,_tmp837)))),_tmp836)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp838;return(_tmp838.f1=nullable,((_tmp838.f2=bound,((_tmp838.f3=zeroterm,((_tmp838.f4=rgn,_tmp838)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp845;const char*_tmp844;void*_tmp843[1];struct Cyc_Int_pa_PrintArg_struct _tmp842;struct _tuple0*_tmp841;return(_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841->f1=Cyc_Absyn_Rel_n(0),((_tmp841->f2=(
(_tmp845=_cycalloc(sizeof(*_tmp845)),((_tmp845[0]=(struct _dyneither_ptr)((_tmp842.tag=1,((_tmp842.f1=(unsigned long)enum_counter ++,((_tmp843[0]=& _tmp842,Cyc_aprintf(((_tmp844="__anonymous_enum_%d__",_tag_dyneither(_tmp844,sizeof(char),22))),_tag_dyneither(_tmp843,sizeof(void*),1)))))))),_tmp845)))),_tmp841)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp2A;struct _tuple17*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=(_tmp23->f2)->f1;_tmp2A=(_tmp23->f2)->f2;
if(_tmp27 != 0){
const char*_tmp846;Cyc_Parse_err(((_tmp846="bad type params in struct field",_tag_dyneither(_tmp846,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp847;Cyc_Parse_err(((_tmp847="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp847,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp848;return(_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848->name=(*_tmp24).f2,((_tmp848->tq=_tmp25,((_tmp848->type=_tmp26,((_tmp848->width=_tmp29,((_tmp848->attributes=_tmp28,((_tmp848->requires_clause=_tmp2A,_tmp848)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp849;return(_tmp849.Signed_spec=0,((_tmp849.Unsigned_spec=0,((_tmp849.Short_spec=0,((_tmp849.Long_spec=0,((_tmp849.Long_Long_spec=0,((_tmp849.Valid_type_spec=0,((_tmp849.Type_spec=Cyc_Absyn_sint_typ,((_tmp849.loc=loc,_tmp849)))))))))))))));}
# 268
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2F=Cyc_Parse_empty_spec(loc);
_tmp2F.Type_spec=t;
_tmp2F.Valid_type_spec=1;
return _tmp2F;}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp30=Cyc_Parse_empty_spec(loc);
_tmp30.Signed_spec=1;
return _tmp30;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Unsigned_spec=1;
return _tmp31;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Short_spec=1;
return _tmp32;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Long_spec=1;
return _tmp33;}
# 296
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 298
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 311 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp34=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_stmttmp2=(struct _tuple8*)x->hd;struct _tuple8*_tmp35=_stmttmp2;struct _dyneither_ptr*_tmp36;void*_tmp38;struct _dyneither_ptr _tmp39;void**_tmp3C;_LL12: _tmp36=_tmp35->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35->f3;if(_tmp37->tag != 19)goto _LL14;else{_tmp38=(void*)_tmp37->f1;}};if(!(_tmp36 != 0))goto _LL14;_LL13:
# 316
{void*_tmp3D=_tmp38;void**_tmp3F;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL1B;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}_LL1A: {
# 320
struct Cyc_String_pa_PrintArg_struct _tmp851;void*_tmp850[1];const char*_tmp84F;struct _dyneither_ptr*_tmp84E;struct _dyneither_ptr*nm=(_tmp84E=_cycalloc(sizeof(*_tmp84E)),((_tmp84E[0]=(struct _dyneither_ptr)((_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp36),((_tmp850[0]=& _tmp851,Cyc_aprintf(((_tmp84F="`%s",_tag_dyneither(_tmp84F,sizeof(char),4))),_tag_dyneither(_tmp850,sizeof(void*),1)))))))),_tmp84E)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp860;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp85F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp85E;struct Cyc_Absyn_Tvar*_tmp85D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp85C;*_tmp3F=(void*)((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp860.tag=2,((_tmp860.f1=((_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D->name=nm,((_tmp85D->identity=- 1,((_tmp85D->kind=(void*)((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp85E.tag=0,((_tmp85E.f1=& Cyc_Tcutil_ik,_tmp85E)))),_tmp85F)))),_tmp85D)))))))),_tmp860)))),_tmp85C))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp863;struct Cyc_List_List*_tmp862;_tmp34=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->hd=((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863->f1=_tmp36,((_tmp863->f2=_tmp38,_tmp863)))))),((_tmp862->tl=_tmp34,_tmp862))))));}goto _LL11;_LL14: if(_tmp35->f1 == 0)goto _LL16;_tmp39=*_tmp35->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35->f3;if(_tmp3A->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3A->f1);if(_tmp3B->tag != 1)goto _LL16;else{_tmp3C=(void**)((void**)& _tmp3B->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp86B;void*_tmp86A[1];const char*_tmp869;struct _dyneither_ptr*_tmp868;struct _dyneither_ptr*nm=(_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=(struct _dyneither_ptr)((_tmp86B.tag=0,((_tmp86B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39),((_tmp86A[0]=& _tmp86B,Cyc_aprintf(((_tmp869="`%s",_tag_dyneither(_tmp869,sizeof(char),4))),_tag_dyneither(_tmp86A,sizeof(void*),1)))))))),_tmp868)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp87A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp879;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp878;struct Cyc_Absyn_Tvar*_tmp877;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp876;*_tmp3C=(void*)((_tmp876=_cycalloc(sizeof(*_tmp876)),((_tmp876[0]=((_tmp87A.tag=2,((_tmp87A.f1=((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877->name=nm,((_tmp877->identity=- 1,((_tmp877->kind=(void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((_tmp878.tag=0,((_tmp878.f1=& Cyc_Tcutil_rk,_tmp878)))),_tmp879)))),_tmp877)))))))),_tmp87A)))),_tmp876))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 335
return _tmp34;}
# 339
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp54=0;
for(0;x != 0;x=x->tl){
void*_stmttmp3=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp55=_stmttmp3;void*_tmp57;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp55;if(_tmp56->tag != 19)goto _LL20;else{_tmp57=(void*)_tmp56->f1;}}_LL1F:
# 344
{struct _tuple18*_tmp87D;struct Cyc_List_List*_tmp87C;_tmp54=((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C->hd=((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp87D->f2=_tmp57,_tmp87D)))))),((_tmp87C->tl=_tmp54,_tmp87C))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 348
return _tmp54;}
# 352
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_stmttmp4=e->r;void*_tmp5A=_stmttmp4;struct _dyneither_ptr*_tmp5C;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A;if(_tmp5B->tag != 1)goto _LL25;else{if((((_tmp5B->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp5B->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp5C=(_tmp5B->f1)->f2;}}_LL24:
# 355
{struct Cyc_List_List*_tmp5D=tags;for(0;_tmp5D != 0;_tmp5D=_tmp5D->tl){
struct _tuple18*_stmttmp5=(struct _tuple18*)_tmp5D->hd;struct _dyneither_ptr*_tmp5F;void*_tmp60;struct _tuple18*_tmp5E=_stmttmp5;_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;
if(Cyc_strptrcmp(_tmp5F,_tmp5C)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp880;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp87F;return Cyc_Absyn_new_exp((void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((_tmp880.tag=38,((_tmp880.f1=Cyc_Tcutil_copy_type(_tmp60),_tmp880)))),_tmp87F)))),e->loc);}}}
# 360
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 363
return e;}
# 368
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp63=t;void*_tmp65;struct Cyc_Absyn_Tqual _tmp66;struct Cyc_Absyn_Exp*_tmp67;union Cyc_Absyn_Constraint*_tmp68;unsigned int _tmp69;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;void*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;union Cyc_Absyn_Constraint*_tmp6F;union Cyc_Absyn_Constraint*_tmp70;struct Cyc_Absyn_PtrLoc*_tmp71;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp64=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63;if(_tmp64->tag != 8)goto _LL2A;else{_tmp65=(_tmp64->f1).elt_type;_tmp66=(_tmp64->f1).tq;_tmp67=(_tmp64->f1).num_elts;_tmp68=(_tmp64->f1).zero_term;_tmp69=(_tmp64->f1).zt_loc;}}_LL29: {
# 371
struct Cyc_Absyn_Exp*nelts2=_tmp67;
if(_tmp67 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp67);{
# 375
void*_tmp72=Cyc_Parse_substitute_tags(tags,_tmp65);
if(_tmp67 != nelts2  || _tmp65 != _tmp72){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp886;struct Cyc_Absyn_ArrayInfo _tmp885;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp884;return(void*)((_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=((_tmp886.tag=8,((_tmp886.f1=((_tmp885.elt_type=_tmp72,((_tmp885.tq=_tmp66,((_tmp885.num_elts=nelts2,((_tmp885.zero_term=_tmp68,((_tmp885.zt_loc=_tmp69,_tmp885)))))))))),_tmp886)))),_tmp884))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp6A->tag != 5)goto _LL2C;else{_tmp6B=(_tmp6A->f1).elt_typ;_tmp6C=(_tmp6A->f1).elt_tq;_tmp6D=((_tmp6A->f1).ptr_atts).rgn;_tmp6E=((_tmp6A->f1).ptr_atts).nullable;_tmp6F=((_tmp6A->f1).ptr_atts).bounds;_tmp70=((_tmp6A->f1).ptr_atts).zero_term;_tmp71=((_tmp6A->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp76=Cyc_Parse_substitute_tags(tags,_tmp6B);
union Cyc_Absyn_Constraint*_tmp77=_tmp6F;
{union Cyc_Absyn_Constraint _stmttmp6=*_tmp6F;union Cyc_Absyn_Constraint _tmp78=_stmttmp6;struct Cyc_Absyn_Exp*_tmp7A;_LL2F: if((_tmp78.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp79=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp78.Eq_constr).val);if(_tmp79->tag != 1)goto _LL31;else{_tmp7A=_tmp79->f1;}};_LL30:
# 384
 _tmp7A=Cyc_Parse_substitute_tags_exp(tags,_tmp7A);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp88C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp88B;union Cyc_Absyn_Constraint*_tmp88A;_tmp77=((_tmp88A=_cycalloc(sizeof(*_tmp88A)),(((_tmp88A->Eq_constr).val=(void*)((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88B.tag=1,((_tmp88B.f1=_tmp7A,_tmp88B)))),_tmp88C)))),(((_tmp88A->Eq_constr).tag=1,_tmp88A))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp76 != _tmp6B  || _tmp77 != _tmp6F){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp892;struct Cyc_Absyn_PtrInfo _tmp891;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp890;return(void*)((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp892.tag=5,((_tmp892.f1=((_tmp891.elt_typ=_tmp76,((_tmp891.elt_tq=_tmp6C,((_tmp891.ptr_atts=(((_tmp891.ptr_atts).rgn=_tmp6D,(((_tmp891.ptr_atts).nullable=_tmp6E,(((_tmp891.ptr_atts).bounds=_tmp77,(((_tmp891.ptr_atts).zero_term=_tmp70,(((_tmp891.ptr_atts).ptrloc=_tmp71,_tmp891.ptr_atts)))))))))),_tmp891)))))),_tmp892)))),_tmp890))));}
goto _LL27;}_LL2C:;_LL2D:
# 394
 goto _LL27;_LL27:;}
# 396
return t;}
# 401
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 408
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 410
struct _tuple19*_tmp893;return(_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893->f1=(*t).f2,((_tmp893->f2=(*t).f3,_tmp893)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_stmttmp7=d->r;void*_tmp82=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp84;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp83=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp82;if(_tmp83->tag != 0)goto _LL36;else{_tmp84=_tmp83->f1;}}_LL35:
 return _tmp84;_LL36:;_LL37: {
const char*_tmp896;void*_tmp895;(_tmp895=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp896="bad declaration in `forarray' statement",_tag_dyneither(_tmp896,sizeof(char),40))),_tag_dyneither(_tmp895,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp87=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp88=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp87;if(_tmp88->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp897;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp897="`H",_tag_dyneither(_tmp897,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp898;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp898="`U",_tag_dyneither(_tmp898,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp899;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp899="`RC",_tag_dyneither(_tmp899,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A3;struct _dyneither_ptr*_tmp8A2;struct Cyc_Absyn_Tvar*_tmp8A1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8A0;return(void*)((_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0[0]=((_tmp8A3.tag=2,((_tmp8A3.f1=((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1->name=((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2[0]=s,_tmp8A2)))),((_tmp8A1->identity=- 1,((_tmp8A1->kind=k,_tmp8A1)))))))),_tmp8A3)))),_tmp8A0))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_stmttmp8=Cyc_Absyn_compress_kb(t->kind);void*_tmp90=_stmttmp8;struct Cyc_Absyn_Kind*_tmp93;void*_tmp94;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp91=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp91->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8A6;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8A5;k=(void*)((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A6.tag=1,((_tmp8A6.f1=0,_tmp8A6)))),_tmp8A5))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp92->tag != 2)goto _LL42;else{_tmp93=_tmp92->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8A9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A8;k=(void*)((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=2,((_tmp8A9.f1=0,((_tmp8A9.f2=_tmp93,_tmp8A9)))))),_tmp8A8))));}goto _LL3D;_LL42: _tmp94=_tmp90;_LL43:
 k=_tmp94;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8AA;return(_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA->name=t->name,((_tmp8AA->identity=- 1,((_tmp8AA->kind=k,_tmp8AA)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp9A=t;struct Cyc_Absyn_Tvar*_tmp9C;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f1;}}_LL46:
 return _tmp9C;_LL47:;_LL48: {
const char*_tmp8AD;void*_tmp8AC;(_tmp8AC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8AD="expecting a list of type variables, not types",_tag_dyneither(_tmp8AD,sizeof(char),46))),_tag_dyneither(_tmp8AC,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8B0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AF;return(void*)((_tmp8AF=_cycalloc(sizeof(*_tmp8AF)),((_tmp8AF[0]=((_tmp8B0.tag=2,((_tmp8B0.f1=pr,_tmp8B0)))),_tmp8AF))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_stmttmp9=Cyc_Tcutil_compress(t);void*_tmpA1=_stmttmp9;void**_tmpA3;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA1;if(_tmpA2->tag != 2)goto _LL4C;else{_tmpA3=(void**)&(_tmpA2->f1)->kind;}}_LL4B: {
# 468
void*_stmttmpA=Cyc_Absyn_compress_kb(*_tmpA3);void*_tmpA4=_stmttmpA;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA4;if(_tmpA5->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpA3=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8B3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8B2;*_tmpA3=(void*)((_tmp8B2=_cycalloc(sizeof(*_tmp8B2)),((_tmp8B2[0]=((_tmp8B3.tag=2,((_tmp8B3.f1=0,((_tmp8B3.f2=k,_tmp8B3)))))),_tmp8B2))));}
return;_LL51:;_LL52:
 return;_LL4E:;}_LL4C:;_LL4D:
# 475
 return;_LL49:;}
# 480
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 486
if(tds == 0)return tms;
# 491
if(tms == 0)return 0;{
# 493
void*_stmttmpB=(void*)tms->hd;void*_tmpA8=_stmttmpB;void*_tmpAA;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpA8;if(_tmpA9->tag != 3)goto _LL56;else{_tmpAA=(void*)_tmpA9->f1;}}_LL55:
# 496
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 499
void*_tmpAB=_tmpAA;struct Cyc_List_List*_tmpAE;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAC=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpAB;if(_tmpAC->tag != 1)goto _LL5B;}_LL5A:
# 501
{const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_warn(loc,((_tmp8B6="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8B6,sizeof(char),93))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpAD=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpAB;if(_tmpAD->tag != 0)goto _LL58;else{_tmpAE=_tmpAD->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8B9;void*_tmp8B8;(_tmp8B8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B9="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8B9,sizeof(char),73))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}{
# 509
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpAE != 0;_tmpAE=_tmpAE->tl){
struct Cyc_List_List*_tmpB3=tds;
for(0;_tmpB3 != 0;_tmpB3=_tmpB3->tl){
struct Cyc_Absyn_Decl*_tmpB4=(struct Cyc_Absyn_Decl*)_tmpB3->hd;
void*_stmttmpC=_tmpB4->r;void*_tmpB5=_stmttmpC;struct Cyc_Absyn_Vardecl*_tmpB7;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB6=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB5;if(_tmpB6->tag != 0)goto _LL60;else{_tmpB7=_tmpB6->f1;}}_LL5F:
# 516
 if(Cyc_zstrptrcmp((*_tmpB7->name).f2,(struct _dyneither_ptr*)_tmpAE->hd)!= 0)
continue;
if(_tmpB7->initializer != 0){
const char*_tmp8BC;void*_tmp8BB;(_tmp8BB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BC="initializer found in parameter declaration",_tag_dyneither(_tmp8BC,sizeof(char),43))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpB7->name)){
const char*_tmp8BF;void*_tmp8BE;(_tmp8BE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BF="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8BF,sizeof(char),47))),_tag_dyneither(_tmp8BE,sizeof(void*),0)));}
{struct _tuple8*_tmp8C2;struct Cyc_List_List*_tmp8C1;rev_new_params=(
(_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->hd=((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2->f1=(*_tmpB7->name).f2,((_tmp8C2->f2=_tmpB7->tq,((_tmp8C2->f3=_tmpB7->type,_tmp8C2)))))))),((_tmp8C1->tl=rev_new_params,_tmp8C1))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8C5;void*_tmp8C4;(_tmp8C4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8C5="nonvariable declaration in parameter type",_tag_dyneither(_tmp8C5,sizeof(char),42))),_tag_dyneither(_tmp8C4,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpB3 == 0){
const char*_tmp8C9;void*_tmp8C8[1];struct Cyc_String_pa_PrintArg_struct _tmp8C7;(_tmp8C7.tag=0,((_tmp8C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpAE->hd)),((_tmp8C8[0]=& _tmp8C7,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8C9="%s is not given a type",_tag_dyneither(_tmp8C9,sizeof(char),23))),_tag_dyneither(_tmp8C8,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8D7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8D6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8D5;struct Cyc_List_List*_tmp8D4;return
(_tmp8D4=_region_malloc(yy,sizeof(*_tmp8D4)),((_tmp8D4->hd=(void*)((_tmp8D8=_region_malloc(yy,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D5.tag=3,((_tmp8D5.f1=(void*)((_tmp8D7=_region_malloc(yy,sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D6.tag=1,((_tmp8D6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8D6.f2=0,((_tmp8D6.f3=0,((_tmp8D6.f4=0,((_tmp8D6.f5=0,_tmp8D6)))))))))))),_tmp8D7)))),_tmp8D5)))),_tmp8D8)))),((_tmp8D4->tl=0,_tmp8D4)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp8D9;return(_tmp8D9=_region_malloc(yy,sizeof(*_tmp8D9)),((_tmp8D9->hd=(void*)tms->hd,((_tmp8D9->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8D9)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8DA;d=((_tmp8DA.id=d.id,((_tmp8DA.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8DA))));}{
# 554
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 560
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 566
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _stmttmpD=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_stmttmpD){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp8DB;Cyc_Parse_err(((_tmp8DB="bad storage class on function",_tag_dyneither(_tmp8DB,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpCB=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _stmttmpE=Cyc_Parse_apply_tms(tq,_tmpCB,atts,d.tms);struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct _tuple13 _tmpCC=_stmttmpE;_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;
# 578
if(_tmpCF != 0){
# 581
const char*_tmp8DE;void*_tmp8DD;(_tmp8DD=0,Cyc_Tcutil_warn(loc,((_tmp8DE="bad type params, ignoring",_tag_dyneither(_tmp8DE,sizeof(char),26))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}{
# 583
void*_tmpD3=_tmpCE;struct Cyc_List_List*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Tqual _tmpD7;void*_tmpD8;struct Cyc_List_List*_tmpD9;int _tmpDA;struct Cyc_Absyn_VarargInfo*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*_tmpDD;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD3;if(_tmpD4->tag != 9)goto _LL69;else{_tmpD5=(_tmpD4->f1).tvars;_tmpD6=(_tmpD4->f1).effect;_tmpD7=(_tmpD4->f1).ret_tqual;_tmpD8=(_tmpD4->f1).ret_typ;_tmpD9=(_tmpD4->f1).args;_tmpDA=(_tmpD4->f1).c_varargs;_tmpDB=(_tmpD4->f1).cyc_varargs;_tmpDC=(_tmpD4->f1).rgn_po;_tmpDD=(_tmpD4->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpDE=0;
{struct Cyc_List_List*_tmpDF=_tmpD9;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
struct _tuple8*_stmttmpF=(struct _tuple8*)_tmpDF->hd;struct _dyneither_ptr*_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;void*_tmpE3;struct _tuple8*_tmpE0=_stmttmpF;_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;_tmpE3=_tmpE0->f3;
if(_tmpE1 == 0){
{const char*_tmp8DF;Cyc_Parse_err(((_tmp8DF="missing argument variable in function prototype",_tag_dyneither(_tmp8DF,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8E2;struct Cyc_List_List*_tmp8E1;_tmpDE=((_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1->hd=((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8E2->f2=_tmpE2,((_tmp8E2->f3=_tmpE3,_tmp8E2)))))))),((_tmp8E1->tl=_tmpDE,_tmp8E1))))));};}else{
# 593
struct _tuple8*_tmp8E5;struct Cyc_List_List*_tmp8E4;_tmpDE=((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4->hd=((_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->f1=_tmpE1,((_tmp8E5->f2=_tmpE2,((_tmp8E5->f3=_tmpE3,_tmp8E5)))))))),((_tmp8E4->tl=_tmpDE,_tmp8E4))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp8E6;return(_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6->sc=sc,((_tmp8E6->name=d.id,((_tmp8E6->tvs=_tmpD5,((_tmp8E6->is_inline=is_inline,((_tmp8E6->effect=_tmpD6,((_tmp8E6->ret_tqual=_tmpD7,((_tmp8E6->ret_type=_tmpD8,((_tmp8E6->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDE),((_tmp8E6->c_varargs=_tmpDA,((_tmp8E6->cyc_varargs=_tmpDB,((_tmp8E6->rgn_po=_tmpDC,((_tmp8E6->body=body,((_tmp8E6->cached_typ=0,((_tmp8E6->param_vardecls=0,((_tmp8E6->fn_vardecl=0,((_tmp8E6->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpDD,_tmpD0),_tmp8E6)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp8E9;void*_tmp8E8;(_tmp8E8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E9="declarator is not a function prototype",_tag_dyneither(_tmp8E9,sizeof(char),39))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}_LL66:;};};};}static char _tmpED[52]="at most one type may appear within a type specifier";
# 612
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpED,_tmpED,_tmpED + 52};static char _tmpEE[63]="const or volatile may appear only once within a type specifier";
# 614
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpEE,_tmpEE,_tmpEE + 63};static char _tmpEF[50]="type specifier includes more than one declaration";
# 616
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpEF,_tmpEF,_tmpEF + 50};static char _tmpF0[60]="sign specifier may appear only once within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpF0,_tmpF0,_tmpF0 + 60};
# 625
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 628
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8EC;(_tmp8EC=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EC,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8ED,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 650
return s1;}
# 656
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 659
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 666
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 678
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_warn(loc,((_tmp8F0="missing type within specifier",_tag_dyneither(_tmp8F0,sizeof(char),30))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmpF7=t;enum Cyc_Absyn_Sign _tmpF9;enum Cyc_Absyn_Size_of _tmpFA;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF7;if(_tmpF8->tag != 6)goto _LL6E;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LL6D:
# 686
 if(_tmpF9 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpFA);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp8F1;Cyc_Parse_err(((_tmp8F1="sign specification on non-integral type",_tag_dyneither(_tmp8F1,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmpFC=t;enum Cyc_Absyn_Sign _tmpFE;enum Cyc_Absyn_Size_of _tmpFF;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFC;if(_tmpFD->tag != 6)goto _LL73;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}}_LL72:
# 694
 if(_tmpFF != sz)
t=Cyc_Absyn_int_typ(_tmpFE,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFC;if(_tmp100->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp8F2;Cyc_Parse_err(((_tmp8F2="size qualifier on non-integral type",_tag_dyneither(_tmp8F2,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 702
return t;}
# 705
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 709
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp102=d.id;
struct _tuple13 _stmttmp10=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp104;void*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct _tuple13 _tmp103=_stmttmp10;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;_tmp106=_tmp103.f3;_tmp107=_tmp103.f4;
# 715
if(ds->tl == 0){
struct _tuple15*_tmp8F5;struct Cyc_List_List*_tmp8F4;return(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->hd=((_tmp8F5=_region_malloc(r,sizeof(*_tmp8F5)),((_tmp8F5->f1=_tmp102,((_tmp8F5->f2=_tmp104,((_tmp8F5->f3=_tmp105,((_tmp8F5->f4=_tmp106,((_tmp8F5->f5=_tmp107,_tmp8F5)))))))))))),((_tmp8F4->tl=0,_tmp8F4)))));}else{
# 718
struct _tuple15*_tmp8F8;struct Cyc_List_List*_tmp8F7;return(_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7->hd=((_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8->f1=_tmp102,((_tmp8F8->f2=_tmp104,((_tmp8F8->f3=_tmp105,((_tmp8F8->f4=_tmp106,((_tmp8F8->f5=_tmp107,_tmp8F8)))))))))))),((_tmp8F7->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8F7)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp8F9;return(_tmp8F9.f1=tq,((_tmp8F9.f2=t,((_tmp8F9.f3=0,((_tmp8F9.f4=atts,_tmp8F9)))))));}{
void*_stmttmp11=(void*)tms->hd;void*_tmp10D=_stmttmp11;union Cyc_Absyn_Constraint*_tmp10F;unsigned int _tmp110;struct Cyc_Absyn_Exp*_tmp112;union Cyc_Absyn_Constraint*_tmp113;unsigned int _tmp114;void*_tmp116;struct Cyc_List_List*_tmp118;unsigned int _tmp119;struct Cyc_Absyn_PtrAtts _tmp11B;struct Cyc_Absyn_Tqual _tmp11C;unsigned int _tmp11E;struct Cyc_List_List*_tmp11F;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp10E=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp10E->tag != 0)goto _LL7A;else{_tmp10F=_tmp10E->f1;_tmp110=_tmp10E->f2;}}_LL79:
# 728
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp10F,_tmp110),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp111=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp111->tag != 1)goto _LL7C;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;_tmp114=_tmp111->f3;}}_LL7B:
# 731
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp112,_tmp113,_tmp114),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp115=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp115->tag != 3)goto _LL7E;else{_tmp116=(void*)_tmp115->f1;}}_LL7D: {
# 734
void*_tmp120=_tmp116;struct Cyc_List_List*_tmp122;int _tmp123;struct Cyc_Absyn_VarargInfo*_tmp124;void*_tmp125;struct Cyc_List_List*_tmp126;unsigned int _tmp128;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp121=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp120;if(_tmp121->tag != 1)goto _LL87;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;_tmp124=_tmp121->f3;_tmp125=(void*)_tmp121->f4;_tmp126=_tmp121->f5;}}_LL86: {
# 736
struct Cyc_List_List*typvars=0;
# 738
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp8FA;fn_atts=((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA->hd=(void*)as->hd,((_tmp8FA->tl=fn_atts,_tmp8FA))))));}else{
# 743
struct Cyc_List_List*_tmp8FB;new_atts=((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB->hd=(void*)as->hd,((_tmp8FB->tl=new_atts,_tmp8FB))))));}}}
# 746
if(tms->tl != 0){
void*_stmttmp12=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp12B=_stmttmp12;struct Cyc_List_List*_tmp12D;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12C=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp12B;if(_tmp12C->tag != 4)goto _LL8C;else{_tmp12D=_tmp12C->f1;}}_LL8B:
# 749
 typvars=_tmp12D;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 753
 goto _LL89;_LL89:;}
# 757
if(((((!_tmp123  && _tmp124 == 0) && _tmp122 != 0) && _tmp122->tl == 0) && (*((struct _tuple8*)_tmp122->hd)).f1 == 0) && (*((struct _tuple8*)_tmp122->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 762
_tmp122=0;{
# 765
struct Cyc_List_List*_tmp12E=Cyc_Parse_get_arg_tags(_tmp122);
# 767
if(_tmp12E != 0)
t=Cyc_Parse_substitute_tags(_tmp12E,t);
t=Cyc_Parse_array2ptr(t,0);
# 772
{struct Cyc_List_List*_tmp12F=_tmp122;for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){
struct _tuple8*_stmttmp13=(struct _tuple8*)_tmp12F->hd;struct _dyneither_ptr*_tmp131;struct Cyc_Absyn_Tqual _tmp132;void**_tmp133;struct _tuple8*_tmp130=_stmttmp13;_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;_tmp133=(void**)& _tmp130->f3;
if(_tmp12E != 0)
*_tmp133=Cyc_Parse_substitute_tags(_tmp12E,*_tmp133);
*_tmp133=Cyc_Parse_array2ptr(*_tmp133,1);}}
# 784
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp125,tq,t,_tmp122,_tmp123,_tmp124,_tmp126,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp127=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp120;if(_tmp127->tag != 0)goto _LL84;else{_tmp128=_tmp127->f2;}}_LL88: {
# 790
const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp128,((_tmp8FE="function declaration without parameter types",_tag_dyneither(_tmp8FE,sizeof(char),45))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp117->tag != 4)goto _LL80;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp8FF;return(_tmp8FF.f1=tq,((_tmp8FF.f2=t,((_tmp8FF.f3=_tmp118,((_tmp8FF.f4=atts,_tmp8FF)))))));}{
# 802
const char*_tmp902;void*_tmp901;(_tmp901=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp119,((_tmp902="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp902,sizeof(char),68))),_tag_dyneither(_tmp901,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11A->tag != 2)goto _LL82;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp908;struct Cyc_Absyn_PtrInfo _tmp907;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp906;return Cyc_Parse_apply_tms(_tmp11C,(void*)((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906[0]=((_tmp908.tag=5,((_tmp908.f1=((_tmp907.elt_typ=t,((_tmp907.elt_tq=tq,((_tmp907.ptr_atts=_tmp11B,_tmp907)))))),_tmp908)))),_tmp906)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp11D=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL77;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LL83:
# 810
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp11F),tms->tl);_LL77:;};}
# 816
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 825
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp13D;struct Cyc_Absyn_Tqual _tmp13E;void*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp141;struct _tuple15*_tmp13C=t;_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;_tmp13F=_tmp13C->f3;_tmp140=_tmp13C->f4;_tmp141=_tmp13C->f5;
# 828
Cyc_Lex_register_typedef(_tmp13D);{
# 830
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp142=_tmp13F;struct Cyc_Core_Opt*_tmp144;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 1)goto _LL91;else{_tmp144=_tmp143->f1;}}_LL90:
# 834
 type=0;
if(_tmp144 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp144;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp13F;goto _LL8E;_LL8E:;}{
# 840
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp90E;struct Cyc_Absyn_Typedefdecl*_tmp90D;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp90C;return Cyc_Absyn_new_decl((void*)((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp90E.tag=9,((_tmp90E.f1=((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D->name=_tmp13D,((_tmp90D->tvs=_tmp140,((_tmp90D->kind=kind,((_tmp90D->defn=type,((_tmp90D->atts=_tmp141,((_tmp90D->tq=_tmp13E,_tmp90D)))))))))))))),_tmp90E)))),_tmp90C)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp911;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp910;return Cyc_Absyn_new_stmt((void*)((_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=12,((_tmp911.f1=d,((_tmp911.f2=s,_tmp911)))))),_tmp910)))),d->loc);}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 862
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 866
struct _RegionHandle _tmp14A=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp14A;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp14C;struct Cyc_Parse_Type_specifier _tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Parse_Declaration_spec _tmp14B=ds;_tmp14C=_tmp14B.tq;_tmp14D=_tmp14B.type_specs;_tmp14E=_tmp14B.attributes;
if(_tmp14C.loc == 0)_tmp14C.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp912;Cyc_Parse_err(((_tmp912="inline is allowed only on function definitions",_tag_dyneither(_tmp912,sizeof(char),47))),loc);}{
# 872
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _stmttmp14=*ds.sc;switch(_stmttmp14){case Cyc_Parse_Typedef_sc: _LL93:
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL94:
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL95:
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL96:
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL97:
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL98:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL99:
 s=Cyc_Absyn_Abstract;break;}}{
# 888
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 892
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 900
void*_tmp150=Cyc_Parse_collapse_type_specifiers(_tmp14D,loc);
if(declarators == 0){
# 904
void*_tmp151=_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp154;struct Cyc_Absyn_Enumdecl*_tmp157;struct Cyc_Absyn_Datatypedecl*_tmp15A;enum Cyc_Absyn_AggrKind _tmp15C;struct _tuple0*_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Datatypedecl**_tmp160;struct _tuple0*_tmp162;int _tmp163;struct Cyc_List_List*_tmp164;struct _tuple0*_tmp166;struct Cyc_List_List*_tmp168;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp152->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp153=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp152->f1)->r;if(_tmp153->tag != 0)goto _LL9E;else{_tmp154=_tmp153->f1;}}}_LL9D:
# 906
 _tmp154->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp154->attributes,_tmp14E);
_tmp154->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp918;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp917;struct Cyc_List_List*_tmp916;struct Cyc_List_List*_tmp16C=(_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->hd=Cyc_Absyn_new_decl((void*)((_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918[0]=((_tmp917.tag=6,((_tmp917.f1=_tmp154,_tmp917)))),_tmp918)))),loc),((_tmp916->tl=0,_tmp916)))));_npop_handler(0);return _tmp16C;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp155->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp156=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp155->f1)->r;if(_tmp156->tag != 1)goto _LLA0;else{_tmp157=_tmp156->f1;}}}_LL9F:
# 910
 if(_tmp14E != 0){const char*_tmp919;Cyc_Parse_err(((_tmp919="attributes on enum not supported",_tag_dyneither(_tmp919,sizeof(char),33))),loc);}
_tmp157->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp91F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp91E;struct Cyc_List_List*_tmp91D;struct Cyc_List_List*_tmp171=(_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D->hd=Cyc_Absyn_new_decl((void*)((_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp91E.tag=8,((_tmp91E.f1=_tmp157,_tmp91E)))),_tmp91F)))),loc),((_tmp91D->tl=0,_tmp91D)))));_npop_handler(0);return _tmp171;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp158->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp159=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp158->f1)->r;if(_tmp159->tag != 2)goto _LLA2;else{_tmp15A=_tmp159->f1;}}}_LLA1:
# 914
 if(_tmp14E != 0){const char*_tmp920;Cyc_Parse_err(((_tmp920="attributes on datatypes not supported",_tag_dyneither(_tmp920,sizeof(char),38))),loc);}
_tmp15A->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp926;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp925;struct Cyc_List_List*_tmp924;struct Cyc_List_List*_tmp176=(_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->hd=Cyc_Absyn_new_decl((void*)((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp925.tag=7,((_tmp925.f1=_tmp15A,_tmp925)))),_tmp926)))),loc),((_tmp924->tl=0,_tmp924)))));_npop_handler(0);return _tmp176;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp151;if(_tmp15B->tag != 11)goto _LLA4;else{if((((_tmp15B->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp15C=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f1;_tmp15D=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f2;_tmp15E=(_tmp15B->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15E);
struct Cyc_Absyn_Aggrdecl*_tmp927;struct Cyc_Absyn_Aggrdecl*_tmp178=(_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->kind=_tmp15C,((_tmp927->sc=s,((_tmp927->name=_tmp15D,((_tmp927->tvs=_tmp177,((_tmp927->impl=0,((_tmp927->attributes=0,_tmp927)))))))))))));
if(_tmp14E != 0){const char*_tmp928;Cyc_Parse_err(((_tmp928="bad attributes on type declaration",_tag_dyneither(_tmp928,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp92E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp92D;struct Cyc_List_List*_tmp92C;struct Cyc_List_List*_tmp17D=(_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->hd=Cyc_Absyn_new_decl((void*)((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92D.tag=6,((_tmp92D.f1=_tmp178,_tmp92D)))),_tmp92E)))),loc),((_tmp92C->tl=0,_tmp92C)))));_npop_handler(0);return _tmp17D;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp15F->tag != 3)goto _LLA6;else{if((((_tmp15F->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp160=(struct Cyc_Absyn_Datatypedecl**)(((_tmp15F->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp14E != 0){const char*_tmp92F;Cyc_Parse_err(((_tmp92F="bad attributes on datatype",_tag_dyneither(_tmp92F,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp935;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp934;struct Cyc_List_List*_tmp933;struct Cyc_List_List*_tmp183=(_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933->hd=Cyc_Absyn_new_decl((void*)((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935[0]=((_tmp934.tag=7,((_tmp934.f1=*_tmp160,_tmp934)))),_tmp935)))),loc),((_tmp933->tl=0,_tmp933)))));_npop_handler(0);return _tmp183;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp161->tag != 3)goto _LLA8;else{if((((_tmp161->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp162=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).name;_tmp163=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp164=(_tmp161->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp164);
struct Cyc_Absyn_Decl*_tmp185=Cyc_Absyn_datatype_decl(s,_tmp162,_tmp184,0,_tmp163,loc);
if(_tmp14E != 0){const char*_tmp936;Cyc_Parse_err(((_tmp936="bad attributes on datatype",_tag_dyneither(_tmp936,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp937;struct Cyc_List_List*_tmp188=(_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->hd=_tmp185,((_tmp937->tl=0,_tmp937)))));_npop_handler(0);return _tmp188;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp151;if(_tmp165->tag != 13)goto _LLAA;else{_tmp166=_tmp165->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp938;struct Cyc_Absyn_Enumdecl*_tmp189=(_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->sc=s,((_tmp938->name=_tmp166,((_tmp938->fields=0,_tmp938)))))));
if(_tmp14E != 0){const char*_tmp939;Cyc_Parse_err(((_tmp939="bad attributes on enum",_tag_dyneither(_tmp939,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp943;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp942;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp941;struct Cyc_List_List*_tmp940;struct Cyc_List_List*_tmp18F=(_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940->hd=((_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943->r=(void*)((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=8,((_tmp942.f1=_tmp189,_tmp942)))),_tmp941)))),((_tmp943->loc=loc,_tmp943)))))),((_tmp940->tl=0,_tmp940)))));_npop_handler(0);return _tmp18F;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp151;if(_tmp167->tag != 14)goto _LLAC;else{_tmp168=_tmp167->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp946;struct Cyc_Absyn_Enumdecl*_tmp945;struct Cyc_Absyn_Enumdecl*_tmp191=(_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945->sc=s,((_tmp945->name=Cyc_Parse_gensym_enum(),((_tmp945->fields=((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946->v=_tmp168,_tmp946)))),_tmp945)))))));
if(_tmp14E != 0){const char*_tmp947;Cyc_Parse_err(((_tmp947="bad attributes on enum",_tag_dyneither(_tmp947,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp951;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp950;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_List_List*_tmp197=(_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E->hd=((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->r=(void*)((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F[0]=((_tmp950.tag=8,((_tmp950.f1=_tmp191,_tmp950)))),_tmp94F)))),((_tmp951->loc=loc,_tmp951)))))),((_tmp94E->tl=0,_tmp94E)))));_npop_handler(0);return _tmp197;};}_LLAC:;_LLAD:
{const char*_tmp952;Cyc_Parse_err(((_tmp952="missing declarator",_tag_dyneither(_tmp952,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp19B=0;_npop_handler(0);return _tmp19B;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp19C=Cyc_Parse_apply_tmss(mkrgn,_tmp14C,_tmp150,declarators,_tmp14E);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp953;Cyc_Parse_err(((_tmp953="initializer in typedef declaration",_tag_dyneither(_tmp953,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp19C);
struct Cyc_List_List*_tmp19E=decls;_npop_handler(0);return _tmp19E;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp19F=_tmp19C;for(0;_tmp19F != 0;(_tmp19F=_tmp19F->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_stmttmp15=(struct _tuple15*)_tmp19F->hd;struct _tuple0*_tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct _tuple15*_tmp1A0=_stmttmp15;_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;_tmp1A4=_tmp1A0->f4;_tmp1A5=_tmp1A0->f5;
if(_tmp1A4 != 0){
const char*_tmp956;void*_tmp955;(_tmp955=0,Cyc_Tcutil_warn(loc,((_tmp956="bad type params, ignoring",_tag_dyneither(_tmp956,sizeof(char),26))),_tag_dyneither(_tmp955,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp959;void*_tmp958;(_tmp958=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp959="unexpected NULL in parse!",_tag_dyneither(_tmp959,sizeof(char),26))),_tag_dyneither(_tmp958,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1AA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1AB=Cyc_Absyn_new_vardecl(_tmp1A1,_tmp1A3,_tmp1AA);
_tmp1AB->tq=_tmp1A2;
_tmp1AB->sc=s;
_tmp1AB->attributes=_tmp1A5;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp95F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp95E;struct Cyc_Absyn_Decl*_tmp95D;struct Cyc_Absyn_Decl*_tmp1AC=(_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D->r=(void*)((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F[0]=((_tmp95E.tag=0,((_tmp95E.f1=_tmp1AB,_tmp95E)))),_tmp95F)))),((_tmp95D->loc=loc,_tmp95D)))));
struct Cyc_List_List*_tmp960;decls=((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960->hd=_tmp1AC,((_tmp960->tl=decls,_tmp960))))));};};}}{
# 970
struct Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1B1;};}}};};};};}
# 867
;_pop_region(mkrgn);}
# 977
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _stmttmp16=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_stmttmp16){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 987
{char _stmttmp18=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_stmttmp18){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 994
break;case 'T': _LLB5:
# 996
{char _stmttmp17=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_stmttmp17){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1003
break;default: _LLBC:
 break;}}
# 1006
{const char*_tmp965;void*_tmp964[2];struct Cyc_String_pa_PrintArg_struct _tmp963;struct Cyc_Int_pa_PrintArg_struct _tmp962;Cyc_Parse_err((struct _dyneither_ptr)((_tmp962.tag=1,((_tmp962.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp963.tag=0,((_tmp963.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp964[0]=& _tmp963,((_tmp964[1]=& _tmp962,Cyc_aprintf(((_tmp965="bad kind: %s; strlen=%d",_tag_dyneither(_tmp965,sizeof(char),24))),_tag_dyneither(_tmp964,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_stmttmp19=p->r;void*_tmp1B6=_stmttmp19;struct _tuple0*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1BA;struct Cyc_Absyn_Pat*_tmp1BD;enum Cyc_Absyn_Sign _tmp1C0;int _tmp1C1;char _tmp1C3;struct _dyneither_ptr _tmp1C5;int _tmp1C6;struct _tuple0*_tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;_LLC5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1B7=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1B7->tag != 14)goto _LLC7;else{_tmp1B8=_tmp1B7->f1;}}_LLC6:
 return Cyc_Absyn_unknownid_exp(_tmp1B8,p->loc);_LLC7: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1B9=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1B9->tag != 2)goto _LLC9;else{_tmp1BA=_tmp1B9->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1BB=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1B9->f2)->r;if(_tmp1BB->tag != 0)goto _LLC9;};}}_LLC8:
# 1015
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1BA->name,p->loc),p->loc);_LLC9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1BC=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1BC->tag != 5)goto _LLCB;else{_tmp1BD=_tmp1BC->f1;}}_LLCA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1BD),p->loc);_LLCB: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1BE=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1BE->tag != 8)goto _LLCD;}_LLCC:
 return Cyc_Absyn_null_exp(p->loc);_LLCD: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1BF=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1BF->tag != 9)goto _LLCF;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;}}_LLCE:
 return Cyc_Absyn_int_exp(_tmp1C0,_tmp1C1,p->loc);_LLCF: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1C2=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C2->tag != 10)goto _LLD1;else{_tmp1C3=_tmp1C2->f1;}}_LLD0:
 return Cyc_Absyn_char_exp(_tmp1C3,p->loc);_LLD1: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C4->tag != 11)goto _LLD3;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}_LLD2:
 return Cyc_Absyn_float_exp(_tmp1C5,_tmp1C6,p->loc);_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C7->tag != 15)goto _LLD5;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;if(_tmp1C7->f3 != 0)goto _LLD5;}}_LLD4: {
# 1022
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1C8,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C9);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLD5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1CA=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1CA->tag != 16)goto _LLD7;else{_tmp1CB=_tmp1CA->f1;}}_LLD6:
 return _tmp1CB;_LLD7:;_LLD8:
# 1027
{const char*_tmp966;Cyc_Parse_err(((_tmp966="cannot mix patterns and expressions in case",_tag_dyneither(_tmp966,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC4:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1CE[14]="$(sign_t,int)";
# 1079 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 14}};
# 1081
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1CF=yy1;struct _tuple5 _tmp1D0;_LLDA: if((_tmp1CF.Int_tok).tag != 1)goto _LLDC;_tmp1D0=(struct _tuple5)(_tmp1CF.Int_tok).val;_LLDB:
# 1085
 yyzzz=_tmp1D0;
goto _LLD9;_LLDC:;_LLDD:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD9:;}
# 1089
return yyzzz;}
# 1091
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.Int_tok).val=yy1,(((_tmp967.Int_tok).tag=1,_tmp967)));}static char _tmp1D3[5]="char";
# 1080 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1D3,_tmp1D3,_tmp1D3 + 5}};
# 1082
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1D4=yy1;char _tmp1D5;_LLDF: if((_tmp1D4.Char_tok).tag != 2)goto _LLE1;_tmp1D5=(char)(_tmp1D4.Char_tok).val;_LLE0:
# 1086
 yyzzz=_tmp1D5;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDE:;}
# 1090
return yyzzz;}
# 1092
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.Char_tok).val=yy1,(((_tmp968.Char_tok).tag=2,_tmp968)));}static char _tmp1D8[13]="string_t<`H>";
# 1081 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1D8,_tmp1D8,_tmp1D8 + 13}};
# 1083
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D9=yy1;struct _dyneither_ptr _tmp1DA;_LLE4: if((_tmp1D9.String_tok).tag != 3)goto _LLE6;_tmp1DA=(struct _dyneither_ptr)(_tmp1D9.String_tok).val;_LLE5:
# 1087
 yyzzz=_tmp1DA;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE3:;}
# 1091
return yyzzz;}
# 1093
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.String_tok).val=yy1,(((_tmp969.String_tok).tag=3,_tmp969)));}static char _tmp1DD[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1084 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1DD,_tmp1DD,_tmp1DD + 56}};
# 1086
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1DE=yy1;struct _tuple21*_tmp1DF;_LLE9: if((_tmp1DE.YY1).tag != 7)goto _LLEB;_tmp1DF=(struct _tuple21*)(_tmp1DE.YY1).val;_LLEA:
# 1090
 yyzzz=_tmp1DF;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLE8:;}
# 1094
return yyzzz;}
# 1096
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY1).val=yy1,(((_tmp96A.YY1).tag=7,_tmp96A)));}static char _tmp1E2[19]="conref_t<bounds_t>";
# 1085 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1E2,_tmp1E2,_tmp1E2 + 19}};
# 1087
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1E3=yy1;union Cyc_Absyn_Constraint*_tmp1E4;_LLEE: if((_tmp1E3.YY2).tag != 8)goto _LLF0;_tmp1E4=(union Cyc_Absyn_Constraint*)(_tmp1E3.YY2).val;_LLEF:
# 1091
 yyzzz=_tmp1E4;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLED:;}
# 1095
return yyzzz;}
# 1097
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY2).val=yy1,(((_tmp96B.YY2).tag=8,_tmp96B)));}static char _tmp1E7[28]="list_t<offsetof_field_t,`H>";
# 1086 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1E7,_tmp1E7,_tmp1E7 + 28}};
# 1088
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E8=yy1;struct Cyc_List_List*_tmp1E9;_LLF3: if((_tmp1E8.YY3).tag != 9)goto _LLF5;_tmp1E9=(struct Cyc_List_List*)(_tmp1E8.YY3).val;_LLF4:
# 1092
 yyzzz=_tmp1E9;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF2:;}
# 1096
return yyzzz;}
# 1098
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY3).val=yy1,(((_tmp96C.YY3).tag=9,_tmp96C)));}static char _tmp1EC[6]="exp_t";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 6}};
# 1089
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1ED=yy1;struct Cyc_Absyn_Exp*_tmp1EE;_LLF8: if((_tmp1ED.YY4).tag != 10)goto _LLFA;_tmp1EE=(struct Cyc_Absyn_Exp*)(_tmp1ED.YY4).val;_LLF9:
# 1093
 yyzzz=_tmp1EE;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLF7:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY4).val=yy1,(((_tmp96D.YY4).tag=10,_tmp96D)));}static char _tmp1F1[10]="exp_opt_t";
# 1095 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1F1,_tmp1F1,_tmp1F1 + 10}};
# 1097
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1F2=yy1;struct Cyc_Absyn_Exp*_tmp1F3;_LLFD: if((_tmp1F2.YY5).tag != 11)goto _LLFF;_tmp1F3=(struct Cyc_Absyn_Exp*)(_tmp1F2.YY5).val;_LLFE:
# 1101
 yyzzz=_tmp1F3;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY5);_LLFC:;}
# 1105
return yyzzz;}
# 1107
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY5).val=yy1,(((_tmp96E.YY5).tag=11,_tmp96E)));}static char _tmp1F6[17]="list_t<exp_t,`H>";
# 1096 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 17}};
# 1098
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F7=yy1;struct Cyc_List_List*_tmp1F8;_LL102: if((_tmp1F7.YY6).tag != 12)goto _LL104;_tmp1F8=(struct Cyc_List_List*)(_tmp1F7.YY6).val;_LL103:
# 1102
 yyzzz=_tmp1F8;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL101:;}
# 1106
return yyzzz;}
# 1108
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY6).val=yy1,(((_tmp96F.YY6).tag=12,_tmp96F)));}static char _tmp1FB[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 47}};
# 1099
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FC=yy1;struct Cyc_List_List*_tmp1FD;_LL107: if((_tmp1FC.YY7).tag != 13)goto _LL109;_tmp1FD=(struct Cyc_List_List*)(_tmp1FC.YY7).val;_LL108:
# 1103
 yyzzz=_tmp1FD;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL106:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY7).val=yy1,(((_tmp970.YY7).tag=13,_tmp970)));}static char _tmp200[9]="primop_t";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp200,_tmp200,_tmp200 + 9}};
# 1100
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp201=yy1;enum Cyc_Absyn_Primop _tmp202;_LL10C: if((_tmp201.YY8).tag != 14)goto _LL10E;_tmp202=(enum Cyc_Absyn_Primop)(_tmp201.YY8).val;_LL10D:
# 1104
 yyzzz=_tmp202;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL10B:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY8).val=yy1,(((_tmp971.YY8).tag=14,_tmp971)));}static char _tmp205[19]="opt_t<primop_t,`H>";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp205,_tmp205,_tmp205 + 19}};
# 1101
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp206=yy1;struct Cyc_Core_Opt*_tmp207;_LL111: if((_tmp206.YY9).tag != 15)goto _LL113;_tmp207=(struct Cyc_Core_Opt*)(_tmp206.YY9).val;_LL112:
# 1105
 yyzzz=_tmp207;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL110:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY9).val=yy1,(((_tmp972.YY9).tag=15,_tmp972)));}static char _tmp20A[7]="qvar_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 7}};
# 1102
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp20B=yy1;struct _tuple0*_tmp20C;_LL116: if((_tmp20B.QualId_tok).tag != 5)goto _LL118;_tmp20C=(struct _tuple0*)(_tmp20B.QualId_tok).val;_LL117:
# 1106
 yyzzz=_tmp20C;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.QualId_tok).val=yy1,(((_tmp973.QualId_tok).tag=5,_tmp973)));}static char _tmp20F[7]="stmt_t";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp20F,_tmp20F,_tmp20F + 7}};
# 1105
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp210=yy1;struct Cyc_Absyn_Stmt*_tmp211;_LL11B: if((_tmp210.YY10).tag != 16)goto _LL11D;_tmp211=(struct Cyc_Absyn_Stmt*)(_tmp210.YY10).val;_LL11C:
# 1109
 yyzzz=_tmp211;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11A:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY10).val=yy1,(((_tmp974.YY10).tag=16,_tmp974)));}static char _tmp214[27]="list_t<switch_clause_t,`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp214,_tmp214,_tmp214 + 27}};
# 1109
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp215=yy1;struct Cyc_List_List*_tmp216;_LL120: if((_tmp215.YY11).tag != 17)goto _LL122;_tmp216=(struct Cyc_List_List*)(_tmp215.YY11).val;_LL121:
# 1113
 yyzzz=_tmp216;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL11F:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY11).val=yy1,(((_tmp975.YY11).tag=17,_tmp975)));}static char _tmp219[6]="pat_t";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 6}};
# 1110
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp21A=yy1;struct Cyc_Absyn_Pat*_tmp21B;_LL125: if((_tmp21A.YY12).tag != 18)goto _LL127;_tmp21B=(struct Cyc_Absyn_Pat*)(_tmp21A.YY12).val;_LL126:
# 1114
 yyzzz=_tmp21B;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL124:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY12).val=yy1,(((_tmp976.YY12).tag=18,_tmp976)));}static char _tmp21E[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp21E,_tmp21E,_tmp21E + 28}};
# 1115
struct _tuple22*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp21F=yy1;struct _tuple22*_tmp220;_LL12A: if((_tmp21F.YY13).tag != 19)goto _LL12C;_tmp220=(struct _tuple22*)(_tmp21F.YY13).val;_LL12B:
# 1119
 yyzzz=_tmp220;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL129:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY13).val=yy1,(((_tmp977.YY13).tag=19,_tmp977)));}static char _tmp223[17]="list_t<pat_t,`H>";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 17}};
# 1116
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp224=yy1;struct Cyc_List_List*_tmp225;_LL12F: if((_tmp224.YY14).tag != 20)goto _LL131;_tmp225=(struct Cyc_List_List*)(_tmp224.YY14).val;_LL130:
# 1120
 yyzzz=_tmp225;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL12E:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY14).val=yy1,(((_tmp978.YY14).tag=20,_tmp978)));}static char _tmp228[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 36}};
# 1117
struct _tuple23*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp229=yy1;struct _tuple23*_tmp22A;_LL134: if((_tmp229.YY15).tag != 21)goto _LL136;_tmp22A=(struct _tuple23*)(_tmp229.YY15).val;_LL135:
# 1121
 yyzzz=_tmp22A;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL133:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY15).val=yy1,(((_tmp979.YY15).tag=21,_tmp979)));}static char _tmp22D[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp22D,_tmp22D,_tmp22D + 47}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22E=yy1;struct Cyc_List_List*_tmp22F;_LL139: if((_tmp22E.YY16).tag != 22)goto _LL13B;_tmp22F=(struct Cyc_List_List*)(_tmp22E.YY16).val;_LL13A:
# 1122
 yyzzz=_tmp22F;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL138:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY16).val=yy1,(((_tmp97A.YY16).tag=22,_tmp97A)));}static char _tmp232[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 58}};
# 1119
struct _tuple22*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp233=yy1;struct _tuple22*_tmp234;_LL13E: if((_tmp233.YY17).tag != 23)goto _LL140;_tmp234=(struct _tuple22*)(_tmp233.YY17).val;_LL13F:
# 1123
 yyzzz=_tmp234;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL13D:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY17).val=yy1,(((_tmp97B.YY17).tag=23,_tmp97B)));}static char _tmp237[9]="fndecl_t";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 9}};
# 1120
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp238=yy1;struct Cyc_Absyn_Fndecl*_tmp239;_LL143: if((_tmp238.YY18).tag != 24)goto _LL145;_tmp239=(struct Cyc_Absyn_Fndecl*)(_tmp238.YY18).val;_LL144:
# 1124
 yyzzz=_tmp239;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL142:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY18).val=yy1,(((_tmp97C.YY18).tag=24,_tmp97C)));}static char _tmp23C[18]="list_t<decl_t,`H>";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23C,_tmp23C,_tmp23C + 18}};
# 1121
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23D=yy1;struct Cyc_List_List*_tmp23E;_LL148: if((_tmp23D.YY19).tag != 25)goto _LL14A;_tmp23E=(struct Cyc_List_List*)(_tmp23D.YY19).val;_LL149:
# 1125
 yyzzz=_tmp23E;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL147:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY19).val=yy1,(((_tmp97D.YY19).tag=25,_tmp97D)));}static char _tmp241[12]="decl_spec_t";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp241,_tmp241,_tmp241 + 12}};
# 1123
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp242=yy1;struct Cyc_Parse_Declaration_spec _tmp243;_LL14D: if((_tmp242.YY20).tag != 26)goto _LL14F;_tmp243=(struct Cyc_Parse_Declaration_spec)(_tmp242.YY20).val;_LL14E:
# 1127
 yyzzz=_tmp243;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL14C:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY20).val=yy1,(((_tmp97E.YY20).tag=26,_tmp97E)));}static char _tmp246[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp246,_tmp246,_tmp246 + 31}};
# 1124
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp247=yy1;struct _tuple11 _tmp248;_LL152: if((_tmp247.YY21).tag != 27)goto _LL154;_tmp248=(struct _tuple11)(_tmp247.YY21).val;_LL153:
# 1128
 yyzzz=_tmp248;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL151:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY21).val=yy1,(((_tmp97F.YY21).tag=27,_tmp97F)));}static char _tmp24B[23]="declarator_list_t<`yy>";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp24B,_tmp24B,_tmp24B + 23}};
# 1125
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp24C=yy1;struct _tuple12*_tmp24D;_LL157: if((_tmp24C.YY22).tag != 28)goto _LL159;_tmp24D=(struct _tuple12*)(_tmp24C.YY22).val;_LL158:
# 1129
 yyzzz=_tmp24D;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL156:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY22).val=yy1,(((_tmp980.YY22).tag=28,_tmp980)));}static char _tmp250[19]="storage_class_t@`H";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 19}};
# 1126
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp251=yy1;enum Cyc_Parse_Storage_class*_tmp252;_LL15C: if((_tmp251.YY23).tag != 29)goto _LL15E;_tmp252=(enum Cyc_Parse_Storage_class*)(_tmp251.YY23).val;_LL15D:
# 1130
 yyzzz=_tmp252;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL15B:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY23).val=yy1,(((_tmp981.YY23).tag=29,_tmp981)));}static char _tmp255[17]="type_specifier_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp255,_tmp255,_tmp255 + 17}};
# 1127
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp256=yy1;struct Cyc_Parse_Type_specifier _tmp257;_LL161: if((_tmp256.YY24).tag != 30)goto _LL163;_tmp257=(struct Cyc_Parse_Type_specifier)(_tmp256.YY24).val;_LL162:
# 1131
 yyzzz=_tmp257;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL160:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY24).val=yy1,(((_tmp982.YY24).tag=30,_tmp982)));}static char _tmp25A[12]="aggr_kind_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp25A,_tmp25A,_tmp25A + 12}};
# 1129
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp25B=yy1;enum Cyc_Absyn_AggrKind _tmp25C;_LL166: if((_tmp25B.YY25).tag != 31)goto _LL168;_tmp25C=(enum Cyc_Absyn_AggrKind)(_tmp25B.YY25).val;_LL167:
# 1133
 yyzzz=_tmp25C;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL165:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY25).val=yy1,(((_tmp983.YY25).tag=31,_tmp983)));}static char _tmp25F[8]="tqual_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp25F,_tmp25F,_tmp25F + 8}};
# 1130
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp260=yy1;struct Cyc_Absyn_Tqual _tmp261;_LL16B: if((_tmp260.YY26).tag != 32)goto _LL16D;_tmp261=(struct Cyc_Absyn_Tqual)(_tmp260.YY26).val;_LL16C:
# 1134
 yyzzz=_tmp261;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16A:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY26).val=yy1,(((_tmp984.YY26).tag=32,_tmp984)));}static char _tmp264[23]="list_t<aggrfield_t,`H>";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp264,_tmp264,_tmp264 + 23}};
# 1131
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp265=yy1;struct Cyc_List_List*_tmp266;_LL170: if((_tmp265.YY27).tag != 33)goto _LL172;_tmp266=(struct Cyc_List_List*)(_tmp265.YY27).val;_LL171:
# 1135
 yyzzz=_tmp266;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL16F:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY27).val=yy1,(((_tmp985.YY27).tag=33,_tmp985)));}static char _tmp269[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp269,_tmp269,_tmp269 + 34}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26A=yy1;struct Cyc_List_List*_tmp26B;_LL175: if((_tmp26A.YY28).tag != 34)goto _LL177;_tmp26B=(struct Cyc_List_List*)(_tmp26A.YY28).val;_LL176:
# 1136
 yyzzz=_tmp26B;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL174:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY28).val=yy1,(((_tmp986.YY28).tag=34,_tmp986)));}static char _tmp26E[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp26E,_tmp26E,_tmp26E + 33}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26F=yy1;struct Cyc_List_List*_tmp270;_LL17A: if((_tmp26F.YY29).tag != 35)goto _LL17C;_tmp270=(struct Cyc_List_List*)(_tmp26F.YY29).val;_LL17B:
# 1137
 yyzzz=_tmp270;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL179:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY29).val=yy1,(((_tmp987.YY29).tag=35,_tmp987)));}static char _tmp273[18]="declarator_t<`yy>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp273,_tmp273,_tmp273 + 18}};
# 1134
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp274=yy1;struct Cyc_Parse_Declarator _tmp275;_LL17F: if((_tmp274.YY30).tag != 36)goto _LL181;_tmp275=(struct Cyc_Parse_Declarator)(_tmp274.YY30).val;_LL180:
# 1138
 yyzzz=_tmp275;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL17E:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY30).val=yy1,(((_tmp988.YY30).tag=36,_tmp988)));}static char _tmp278[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp278,_tmp278,_tmp278 + 45}};
# 1135
struct _tuple24*Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp279=yy1;struct _tuple24*_tmp27A;_LL184: if((_tmp279.YY31).tag != 37)goto _LL186;_tmp27A=(struct _tuple24*)(_tmp279.YY31).val;_LL185:
# 1139
 yyzzz=_tmp27A;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL183:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY31).val=yy1,(((_tmp989.YY31).tag=37,_tmp989)));}static char _tmp27D[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp27D,_tmp27D,_tmp27D + 57}};
# 1136
struct Cyc_List_List*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp27E=yy1;struct Cyc_List_List*_tmp27F;_LL189: if((_tmp27E.YY32).tag != 38)goto _LL18B;_tmp27F=(struct Cyc_List_List*)(_tmp27E.YY32).val;_LL18A:
# 1140
 yyzzz=_tmp27F;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL188:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY32).val=yy1,(((_tmp98A.YY32).tag=38,_tmp98A)));}static char _tmp282[26]="abstractdeclarator_t<`yy>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp282,_tmp282,_tmp282 + 26}};
# 1137
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp283=yy1;struct Cyc_Parse_Abstractdeclarator _tmp284;_LL18E: if((_tmp283.YY33).tag != 39)goto _LL190;_tmp284=(struct Cyc_Parse_Abstractdeclarator)(_tmp283.YY33).val;_LL18F:
# 1141
 yyzzz=_tmp284;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL18D:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY33).val=yy1,(((_tmp98B.YY33).tag=39,_tmp98B)));}static char _tmp287[5]="bool";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp287,_tmp287,_tmp287 + 5}};
# 1138
int Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp288=yy1;int _tmp289;_LL193: if((_tmp288.YY34).tag != 40)goto _LL195;_tmp289=(int)(_tmp288.YY34).val;_LL194:
# 1142
 yyzzz=_tmp289;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL192:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY34).val=yy1,(((_tmp98C.YY34).tag=40,_tmp98C)));}static char _tmp28C[8]="scope_t";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28C,_tmp28C,_tmp28C + 8}};
# 1139
enum Cyc_Absyn_Scope Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp28D=yy1;enum Cyc_Absyn_Scope _tmp28E;_LL198: if((_tmp28D.YY35).tag != 41)goto _LL19A;_tmp28E=(enum Cyc_Absyn_Scope)(_tmp28D.YY35).val;_LL199:
# 1143
 yyzzz=_tmp28E;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL197:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY35).val=yy1,(((_tmp98D.YY35).tag=41,_tmp98D)));}static char _tmp291[16]="datatypefield_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp291,_tmp291,_tmp291 + 16}};
# 1140
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp292=yy1;struct Cyc_Absyn_Datatypefield*_tmp293;_LL19D: if((_tmp292.YY36).tag != 42)goto _LL19F;_tmp293=(struct Cyc_Absyn_Datatypefield*)(_tmp292.YY36).val;_LL19E:
# 1144
 yyzzz=_tmp293;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL19C:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.YY36).val=yy1,(((_tmp98E.YY36).tag=42,_tmp98E)));}static char _tmp296[27]="list_t<datatypefield_t,`H>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp296,_tmp296,_tmp296 + 27}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp297=yy1;struct Cyc_List_List*_tmp298;_LL1A2: if((_tmp297.YY37).tag != 43)goto _LL1A4;_tmp298=(struct Cyc_List_List*)(_tmp297.YY37).val;_LL1A3:
# 1145
 yyzzz=_tmp298;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A1:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98F;return((_tmp98F.YY37).val=yy1,(((_tmp98F.YY37).tag=43,_tmp98F)));}static char _tmp29B[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp29B,_tmp29B,_tmp29B + 41}};
# 1142
struct _tuple25 Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp29C=yy1;struct _tuple25 _tmp29D;_LL1A7: if((_tmp29C.YY38).tag != 44)goto _LL1A9;_tmp29D=(struct _tuple25)(_tmp29C.YY38).val;_LL1A8:
# 1146
 yyzzz=_tmp29D;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1A6:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.YY38).val=yy1,(((_tmp990.YY38).tag=44,_tmp990)));}static char _tmp2A0[17]="list_t<var_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A0,_tmp2A0,_tmp2A0 + 17}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A1=yy1;struct Cyc_List_List*_tmp2A2;_LL1AC: if((_tmp2A1.YY39).tag != 45)goto _LL1AE;_tmp2A2=(struct Cyc_List_List*)(_tmp2A1.YY39).val;_LL1AD:
# 1147
 yyzzz=_tmp2A2;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1AB:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.YY39).val=yy1,(((_tmp991.YY39).tag=45,_tmp991)));}static char _tmp2A5[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2A5,_tmp2A5,_tmp2A5 + 31}};
# 1144
struct _tuple8*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2A6=yy1;struct _tuple8*_tmp2A7;_LL1B1: if((_tmp2A6.YY40).tag != 46)goto _LL1B3;_tmp2A7=(struct _tuple8*)(_tmp2A6.YY40).val;_LL1B2:
# 1148
 yyzzz=_tmp2A7;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp992;return((_tmp992.YY40).val=yy1,(((_tmp992.YY40).tag=46,_tmp992)));}static char _tmp2AA[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2AA,_tmp2AA,_tmp2AA + 42}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AB=yy1;struct Cyc_List_List*_tmp2AC;_LL1B6: if((_tmp2AB.YY41).tag != 47)goto _LL1B8;_tmp2AC=(struct Cyc_List_List*)(_tmp2AB.YY41).val;_LL1B7:
# 1149
 yyzzz=_tmp2AC;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1B5:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp993;return((_tmp993.YY41).val=yy1,(((_tmp993.YY41).tag=47,_tmp993)));}static char _tmp2AF[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2AF,_tmp2AF,_tmp2AF + 115}};
# 1146
struct _tuple26*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2B0=yy1;struct _tuple26*_tmp2B1;_LL1BB: if((_tmp2B0.YY42).tag != 48)goto _LL1BD;_tmp2B1=(struct _tuple26*)(_tmp2B0.YY42).val;_LL1BC:
# 1150
 yyzzz=_tmp2B1;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BA:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.YY42).val=yy1,(((_tmp994.YY42).tag=48,_tmp994)));}static char _tmp2B4[18]="list_t<type_t,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2B4,_tmp2B4,_tmp2B4 + 18}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B5=yy1;struct Cyc_List_List*_tmp2B6;_LL1C0: if((_tmp2B5.YY43).tag != 49)goto _LL1C2;_tmp2B6=(struct Cyc_List_List*)(_tmp2B5.YY43).val;_LL1C1:
# 1151
 yyzzz=_tmp2B6;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1BF:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.YY43).val=yy1,(((_tmp995.YY43).tag=49,_tmp995)));}static char _tmp2B9[24]="list_t<designator_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2B9,_tmp2B9,_tmp2B9 + 24}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BA=yy1;struct Cyc_List_List*_tmp2BB;_LL1C5: if((_tmp2BA.YY44).tag != 50)goto _LL1C7;_tmp2BB=(struct Cyc_List_List*)(_tmp2BA.YY44).val;_LL1C6:
# 1153
 yyzzz=_tmp2BB;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C4:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.YY44).val=yy1,(((_tmp996.YY44).tag=50,_tmp996)));}static char _tmp2BE[13]="designator_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2BE,_tmp2BE,_tmp2BE + 13}};
# 1150
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2BF=yy1;void*_tmp2C0;_LL1CA: if((_tmp2BF.YY45).tag != 51)goto _LL1CC;_tmp2C0=(void*)(_tmp2BF.YY45).val;_LL1CB:
# 1154
 yyzzz=_tmp2C0;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C9:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY45).val=yy1,(((_tmp997.YY45).tag=51,_tmp997)));}static char _tmp2C3[7]="kind_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2C3,_tmp2C3,_tmp2C3 + 7}};
# 1151
struct Cyc_Absyn_Kind*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2C4=yy1;struct Cyc_Absyn_Kind*_tmp2C5;_LL1CF: if((_tmp2C4.YY46).tag != 52)goto _LL1D1;_tmp2C5=(struct Cyc_Absyn_Kind*)(_tmp2C4.YY46).val;_LL1D0:
# 1155
 yyzzz=_tmp2C5;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1CE:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY46).val=yy1,(((_tmp998.YY46).tag=52,_tmp998)));}static char _tmp2C8[7]="type_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2C8,_tmp2C8,_tmp2C8 + 7}};
# 1152
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C9=yy1;void*_tmp2CA;_LL1D4: if((_tmp2C9.YY47).tag != 53)goto _LL1D6;_tmp2CA=(void*)(_tmp2C9.YY47).val;_LL1D5:
# 1156
 yyzzz=_tmp2CA;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D3:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY47).val=yy1,(((_tmp999.YY47).tag=53,_tmp999)));}static char _tmp2CD[23]="list_t<attribute_t,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2CD,_tmp2CD,_tmp2CD + 23}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CE=yy1;struct Cyc_List_List*_tmp2CF;_LL1D9: if((_tmp2CE.YY48).tag != 54)goto _LL1DB;_tmp2CF=(struct Cyc_List_List*)(_tmp2CE.YY48).val;_LL1DA:
# 1157
 yyzzz=_tmp2CF;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1D8:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY48).val=yy1,(((_tmp99A.YY48).tag=54,_tmp99A)));}static char _tmp2D2[12]="attribute_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D2,_tmp2D2,_tmp2D2 + 12}};
# 1154
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D3=yy1;void*_tmp2D4;_LL1DE: if((_tmp2D3.YY49).tag != 55)goto _LL1E0;_tmp2D4=(void*)(_tmp2D3.YY49).val;_LL1DF:
# 1158
 yyzzz=_tmp2D4;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1DD:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY49).val=yy1,(((_tmp99B.YY49).tag=55,_tmp99B)));}static char _tmp2D7[12]="enumfield_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2D7,_tmp2D7,_tmp2D7 + 12}};
# 1155
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2D8=yy1;struct Cyc_Absyn_Enumfield*_tmp2D9;_LL1E3: if((_tmp2D8.YY50).tag != 56)goto _LL1E5;_tmp2D9=(struct Cyc_Absyn_Enumfield*)(_tmp2D8.YY50).val;_LL1E4:
# 1159
 yyzzz=_tmp2D9;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E2:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY50).val=yy1,(((_tmp99C.YY50).tag=56,_tmp99C)));}static char _tmp2DC[23]="list_t<enumfield_t,`H>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2DC,_tmp2DC,_tmp2DC + 23}};
# 1156
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2DD=yy1;struct Cyc_List_List*_tmp2DE;_LL1E8: if((_tmp2DD.YY51).tag != 57)goto _LL1EA;_tmp2DE=(struct Cyc_List_List*)(_tmp2DD.YY51).val;_LL1E9:
# 1160
 yyzzz=_tmp2DE;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1E7:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY51).val=yy1,(((_tmp99D.YY51).tag=57,_tmp99D)));}static char _tmp2E1[11]="type_opt_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E1,_tmp2E1,_tmp2E1 + 11}};
# 1157
void*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E2=yy1;void*_tmp2E3;_LL1ED: if((_tmp2E2.YY52).tag != 58)goto _LL1EF;_tmp2E3=(void*)(_tmp2E2.YY52).val;_LL1EE:
# 1161
 yyzzz=_tmp2E3;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1EC:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY52).val=yy1,(((_tmp99E.YY52).tag=58,_tmp99E)));}static char _tmp2E6[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2E6,_tmp2E6,_tmp2E6 + 31}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E7=yy1;struct Cyc_List_List*_tmp2E8;_LL1F2: if((_tmp2E7.YY53).tag != 59)goto _LL1F4;_tmp2E8=(struct Cyc_List_List*)(_tmp2E7.YY53).val;_LL1F3:
# 1162
 yyzzz=_tmp2E8;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F1:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY53).val=yy1,(((_tmp99F.YY53).tag=59,_tmp99F)));}static char _tmp2EB[15]="conref_t<bool>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2EB,_tmp2EB,_tmp2EB + 15}};
# 1159
union Cyc_Absyn_Constraint*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2EC=yy1;union Cyc_Absyn_Constraint*_tmp2ED;_LL1F7: if((_tmp2EC.YY54).tag != 60)goto _LL1F9;_tmp2ED=(union Cyc_Absyn_Constraint*)(_tmp2EC.YY54).val;_LL1F8:
# 1163
 yyzzz=_tmp2ED;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1F6:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY54).val=yy1,(((_tmp9A0.YY54).tag=60,_tmp9A0)));}static char _tmp2F0[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F0,_tmp2F0,_tmp2F0 + 45}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F1=yy1;struct Cyc_List_List*_tmp2F2;_LL1FC: if((_tmp2F1.YY55).tag != 61)goto _LL1FE;_tmp2F2=(struct Cyc_List_List*)(_tmp2F1.YY55).val;_LL1FD:
# 1164
 yyzzz=_tmp2F2;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL1FB:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY55).val=yy1,(((_tmp9A1.YY55).tag=61,_tmp9A1)));}static char _tmp2F5[20]="pointer_qual_t<`yy>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY56={Cyc_Core_Failure,{_tmp2F5,_tmp2F5,_tmp2F5 + 20}};
# 1161
void*Cyc_yyget_YY56(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F6=yy1;void*_tmp2F7;_LL201: if((_tmp2F6.YY56).tag != 62)goto _LL203;_tmp2F7=(void*)(_tmp2F6.YY56).val;_LL202:
# 1165
 yyzzz=_tmp2F7;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY56);_LL200:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY56).val=yy1,(((_tmp9A2.YY56).tag=62,_tmp9A2)));}static char _tmp2FA[21]="pointer_quals_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY57={Cyc_Core_Failure,{_tmp2FA,_tmp2FA,_tmp2FA + 21}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2FB=yy1;struct Cyc_List_List*_tmp2FC;_LL206: if((_tmp2FB.YY57).tag != 63)goto _LL208;_tmp2FC=(struct Cyc_List_List*)(_tmp2FB.YY57).val;_LL207:
# 1166
 yyzzz=_tmp2FC;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_YY57);_LL205:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY57).val=yy1,(((_tmp9A3.YY57).tag=63,_tmp9A3)));}static char _tmp2FF[21]="$(bool,string_t<`H>)";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2FF,_tmp2FF,_tmp2FF + 21}};
# 1163
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp300=yy1;struct _tuple20 _tmp301;_LL20B: if((_tmp300.Asm_tok).tag != 6)goto _LL20D;_tmp301=(struct _tuple20)(_tmp300.Asm_tok).val;_LL20C:
# 1167
 yyzzz=_tmp301;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL20A:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.Asm_tok).val=yy1,(((_tmp9A4.Asm_tok).tag=6,_tmp9A4)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1185
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1189
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9A5;return(_tmp9A5.timestamp=0,((_tmp9A5.first_line=0,((_tmp9A5.first_column=0,((_tmp9A5.last_line=0,((_tmp9A5.last_column=0,_tmp9A5)))))))));}
# 1192
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1203 "parse.y"
static short Cyc_yytranslate[375]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp304[2]="$";static char _tmp305[6]="error";static char _tmp306[12]="$undefined.";static char _tmp307[5]="AUTO";static char _tmp308[9]="REGISTER";static char _tmp309[7]="STATIC";static char _tmp30A[7]="EXTERN";static char _tmp30B[8]="TYPEDEF";static char _tmp30C[5]="VOID";static char _tmp30D[5]="CHAR";static char _tmp30E[6]="SHORT";static char _tmp30F[4]="INT";static char _tmp310[5]="LONG";static char _tmp311[6]="FLOAT";static char _tmp312[7]="DOUBLE";static char _tmp313[7]="SIGNED";static char _tmp314[9]="UNSIGNED";static char _tmp315[6]="CONST";static char _tmp316[9]="VOLATILE";static char _tmp317[9]="RESTRICT";static char _tmp318[7]="STRUCT";static char _tmp319[6]="UNION";static char _tmp31A[5]="CASE";static char _tmp31B[8]="DEFAULT";static char _tmp31C[7]="INLINE";static char _tmp31D[7]="SIZEOF";static char _tmp31E[9]="OFFSETOF";static char _tmp31F[3]="IF";static char _tmp320[5]="ELSE";static char _tmp321[7]="SWITCH";static char _tmp322[6]="WHILE";static char _tmp323[3]="DO";static char _tmp324[4]="FOR";static char _tmp325[5]="GOTO";static char _tmp326[9]="CONTINUE";static char _tmp327[6]="BREAK";static char _tmp328[7]="RETURN";static char _tmp329[5]="ENUM";static char _tmp32A[8]="NULL_kw";static char _tmp32B[4]="LET";static char _tmp32C[6]="THROW";static char _tmp32D[4]="TRY";static char _tmp32E[6]="CATCH";static char _tmp32F[7]="EXPORT";static char _tmp330[4]="NEW";static char _tmp331[9]="ABSTRACT";static char _tmp332[9]="FALLTHRU";static char _tmp333[6]="USING";static char _tmp334[10]="NAMESPACE";static char _tmp335[9]="DATATYPE";static char _tmp336[8]="XTUNION";static char _tmp337[7]="TUNION";static char _tmp338[7]="MALLOC";static char _tmp339[8]="RMALLOC";static char _tmp33A[7]="CALLOC";static char _tmp33B[8]="RCALLOC";static char _tmp33C[5]="SWAP";static char _tmp33D[9]="REGION_T";static char _tmp33E[6]="TAG_T";static char _tmp33F[7]="REGION";static char _tmp340[5]="RNEW";static char _tmp341[8]="REGIONS";static char _tmp342[13]="RESET_REGION";static char _tmp343[16]="NOZEROTERM_QUAL";static char _tmp344[14]="ZEROTERM_QUAL";static char _tmp345[12]="REGION_QUAL";static char _tmp346[7]="PORTON";static char _tmp347[8]="PORTOFF";static char _tmp348[12]="DYNREGION_T";static char _tmp349[6]="ALIAS";static char _tmp34A[8]="NUMELTS";static char _tmp34B[8]="VALUEOF";static char _tmp34C[10]="VALUEOF_T";static char _tmp34D[9]="TAGCHECK";static char _tmp34E[13]="NUMELTS_QUAL";static char _tmp34F[10]="THIN_QUAL";static char _tmp350[9]="FAT_QUAL";static char _tmp351[13]="NOTNULL_QUAL";static char _tmp352[14]="NULLABLE_QUAL";static char _tmp353[14]="REQUIRES_QUAL";static char _tmp354[12]="TAGGED_QUAL";static char _tmp355[16]="EXTENSIBLE_QUAL";static char _tmp356[15]="RESETABLE_QUAL";static char _tmp357[7]="PTR_OP";static char _tmp358[7]="INC_OP";static char _tmp359[7]="DEC_OP";static char _tmp35A[8]="LEFT_OP";static char _tmp35B[9]="RIGHT_OP";static char _tmp35C[6]="LE_OP";static char _tmp35D[6]="GE_OP";static char _tmp35E[6]="EQ_OP";static char _tmp35F[6]="NE_OP";static char _tmp360[7]="AND_OP";static char _tmp361[6]="OR_OP";static char _tmp362[11]="MUL_ASSIGN";static char _tmp363[11]="DIV_ASSIGN";static char _tmp364[11]="MOD_ASSIGN";static char _tmp365[11]="ADD_ASSIGN";static char _tmp366[11]="SUB_ASSIGN";static char _tmp367[12]="LEFT_ASSIGN";static char _tmp368[13]="RIGHT_ASSIGN";static char _tmp369[11]="AND_ASSIGN";static char _tmp36A[11]="XOR_ASSIGN";static char _tmp36B[10]="OR_ASSIGN";static char _tmp36C[9]="ELLIPSIS";static char _tmp36D[11]="LEFT_RIGHT";static char _tmp36E[12]="COLON_COLON";static char _tmp36F[11]="IDENTIFIER";static char _tmp370[17]="INTEGER_CONSTANT";static char _tmp371[7]="STRING";static char _tmp372[8]="WSTRING";static char _tmp373[19]="CHARACTER_CONSTANT";static char _tmp374[20]="WCHARACTER_CONSTANT";static char _tmp375[18]="FLOATING_CONSTANT";static char _tmp376[9]="TYPE_VAR";static char _tmp377[13]="TYPEDEF_NAME";static char _tmp378[16]="QUAL_IDENTIFIER";static char _tmp379[18]="QUAL_TYPEDEF_NAME";static char _tmp37A[10]="ATTRIBUTE";static char _tmp37B[4]="ASM";static char _tmp37C[4]="';'";static char _tmp37D[4]="'{'";static char _tmp37E[4]="'}'";static char _tmp37F[4]="','";static char _tmp380[4]="'='";static char _tmp381[4]="'<'";static char _tmp382[4]="'>'";static char _tmp383[4]="'('";static char _tmp384[4]="')'";static char _tmp385[4]="'_'";static char _tmp386[4]="'$'";static char _tmp387[4]="':'";static char _tmp388[4]="'.'";static char _tmp389[4]="'['";static char _tmp38A[4]="']'";static char _tmp38B[4]="'*'";static char _tmp38C[4]="'@'";static char _tmp38D[4]="'?'";static char _tmp38E[4]="'+'";static char _tmp38F[4]="'|'";static char _tmp390[4]="'^'";static char _tmp391[4]="'&'";static char _tmp392[4]="'-'";static char _tmp393[4]="'/'";static char _tmp394[4]="'%'";static char _tmp395[4]="'~'";static char _tmp396[4]="'!'";static char _tmp397[5]="prog";static char _tmp398[17]="translation_unit";static char _tmp399[12]="export_list";static char _tmp39A[19]="export_list_values";static char _tmp39B[21]="external_declaration";static char _tmp39C[15]="optional_comma";static char _tmp39D[20]="function_definition";static char _tmp39E[21]="function_definition2";static char _tmp39F[13]="using_action";static char _tmp3A0[15]="unusing_action";static char _tmp3A1[17]="namespace_action";static char _tmp3A2[19]="unnamespace_action";static char _tmp3A3[12]="declaration";static char _tmp3A4[19]="resetable_qual_opt";static char _tmp3A5[17]="declaration_list";static char _tmp3A6[23]="declaration_specifiers";static char _tmp3A7[24]="storage_class_specifier";static char _tmp3A8[15]="attributes_opt";static char _tmp3A9[11]="attributes";static char _tmp3AA[15]="attribute_list";static char _tmp3AB[10]="attribute";static char _tmp3AC[15]="type_specifier";static char _tmp3AD[25]="type_specifier_notypedef";static char _tmp3AE[5]="kind";static char _tmp3AF[15]="type_qualifier";static char _tmp3B0[15]="enum_specifier";static char _tmp3B1[11]="enum_field";static char _tmp3B2[22]="enum_declaration_list";static char _tmp3B3[26]="struct_or_union_specifier";static char _tmp3B4[16]="type_params_opt";static char _tmp3B5[16]="struct_or_union";static char _tmp3B6[24]="struct_declaration_list";static char _tmp3B7[25]="struct_declaration_list0";static char _tmp3B8[21]="init_declarator_list";static char _tmp3B9[22]="init_declarator_list0";static char _tmp3BA[16]="init_declarator";static char _tmp3BB[19]="struct_declaration";static char _tmp3BC[25]="specifier_qualifier_list";static char _tmp3BD[35]="notypedef_specifier_qualifier_list";static char _tmp3BE[23]="struct_declarator_list";static char _tmp3BF[24]="struct_declarator_list0";static char _tmp3C0[18]="struct_declarator";static char _tmp3C1[19]="datatype_specifier";static char _tmp3C2[14]="qual_datatype";static char _tmp3C3[19]="datatypefield_list";static char _tmp3C4[20]="datatypefield_scope";static char _tmp3C5[14]="datatypefield";static char _tmp3C6[11]="declarator";static char _tmp3C7[23]="declarator_withtypedef";static char _tmp3C8[18]="direct_declarator";static char _tmp3C9[30]="direct_declarator_withtypedef";static char _tmp3CA[8]="pointer";static char _tmp3CB[12]="one_pointer";static char _tmp3CC[14]="pointer_quals";static char _tmp3CD[13]="pointer_qual";static char _tmp3CE[23]="pointer_null_and_bound";static char _tmp3CF[14]="pointer_bound";static char _tmp3D0[18]="zeroterm_qual_opt";static char _tmp3D1[8]="rgn_opt";static char _tmp3D2[11]="tqual_list";static char _tmp3D3[20]="parameter_type_list";static char _tmp3D4[9]="type_var";static char _tmp3D5[16]="optional_effect";static char _tmp3D6[19]="optional_rgn_order";static char _tmp3D7[10]="rgn_order";static char _tmp3D8[16]="optional_inject";static char _tmp3D9[11]="effect_set";static char _tmp3DA[14]="atomic_effect";static char _tmp3DB[11]="region_set";static char _tmp3DC[15]="parameter_list";static char _tmp3DD[22]="parameter_declaration";static char _tmp3DE[16]="identifier_list";static char _tmp3DF[17]="identifier_list0";static char _tmp3E0[12]="initializer";static char _tmp3E1[18]="array_initializer";static char _tmp3E2[17]="initializer_list";static char _tmp3E3[12]="designation";static char _tmp3E4[16]="designator_list";static char _tmp3E5[11]="designator";static char _tmp3E6[10]="type_name";static char _tmp3E7[14]="any_type_name";static char _tmp3E8[15]="type_name_list";static char _tmp3E9[20]="abstract_declarator";static char _tmp3EA[27]="direct_abstract_declarator";static char _tmp3EB[10]="statement";static char _tmp3EC[13]="open_exp_opt";static char _tmp3ED[18]="labeled_statement";static char _tmp3EE[21]="expression_statement";static char _tmp3EF[19]="compound_statement";static char _tmp3F0[16]="block_item_list";static char _tmp3F1[20]="selection_statement";static char _tmp3F2[15]="switch_clauses";static char _tmp3F3[20]="iteration_statement";static char _tmp3F4[15]="jump_statement";static char _tmp3F5[12]="exp_pattern";static char _tmp3F6[20]="conditional_pattern";static char _tmp3F7[19]="logical_or_pattern";static char _tmp3F8[20]="logical_and_pattern";static char _tmp3F9[21]="inclusive_or_pattern";static char _tmp3FA[21]="exclusive_or_pattern";static char _tmp3FB[12]="and_pattern";static char _tmp3FC[17]="equality_pattern";static char _tmp3FD[19]="relational_pattern";static char _tmp3FE[14]="shift_pattern";static char _tmp3FF[17]="additive_pattern";static char _tmp400[23]="multiplicative_pattern";static char _tmp401[13]="cast_pattern";static char _tmp402[14]="unary_pattern";static char _tmp403[16]="postfix_pattern";static char _tmp404[16]="primary_pattern";static char _tmp405[8]="pattern";static char _tmp406[19]="tuple_pattern_list";static char _tmp407[20]="tuple_pattern_list0";static char _tmp408[14]="field_pattern";static char _tmp409[19]="field_pattern_list";static char _tmp40A[20]="field_pattern_list0";static char _tmp40B[11]="expression";static char _tmp40C[22]="assignment_expression";static char _tmp40D[20]="assignment_operator";static char _tmp40E[23]="conditional_expression";static char _tmp40F[20]="constant_expression";static char _tmp410[22]="logical_or_expression";static char _tmp411[23]="logical_and_expression";static char _tmp412[24]="inclusive_or_expression";static char _tmp413[24]="exclusive_or_expression";static char _tmp414[15]="and_expression";static char _tmp415[20]="equality_expression";static char _tmp416[22]="relational_expression";static char _tmp417[17]="shift_expression";static char _tmp418[20]="additive_expression";static char _tmp419[26]="multiplicative_expression";static char _tmp41A[16]="cast_expression";static char _tmp41B[17]="unary_expression";static char _tmp41C[15]="unary_operator";static char _tmp41D[19]="postfix_expression";static char _tmp41E[17]="field_expression";static char _tmp41F[19]="primary_expression";static char _tmp420[25]="argument_expression_list";static char _tmp421[26]="argument_expression_list0";static char _tmp422[9]="constant";static char _tmp423[20]="qual_opt_identifier";static char _tmp424[17]="qual_opt_typedef";static char _tmp425[18]="struct_union_name";static char _tmp426[11]="field_name";static char _tmp427[12]="right_angle";
# 1553 "parse.y"
static struct _dyneither_ptr Cyc_yytname[292]={{_tmp304,_tmp304,_tmp304 + 2},{_tmp305,_tmp305,_tmp305 + 6},{_tmp306,_tmp306,_tmp306 + 12},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 9},{_tmp309,_tmp309,_tmp309 + 7},{_tmp30A,_tmp30A,_tmp30A + 7},{_tmp30B,_tmp30B,_tmp30B + 8},{_tmp30C,_tmp30C,_tmp30C + 5},{_tmp30D,_tmp30D,_tmp30D + 5},{_tmp30E,_tmp30E,_tmp30E + 6},{_tmp30F,_tmp30F,_tmp30F + 4},{_tmp310,_tmp310,_tmp310 + 5},{_tmp311,_tmp311,_tmp311 + 6},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,_tmp313,_tmp313 + 7},{_tmp314,_tmp314,_tmp314 + 9},{_tmp315,_tmp315,_tmp315 + 6},{_tmp316,_tmp316,_tmp316 + 9},{_tmp317,_tmp317,_tmp317 + 9},{_tmp318,_tmp318,_tmp318 + 7},{_tmp319,_tmp319,_tmp319 + 6},{_tmp31A,_tmp31A,_tmp31A + 5},{_tmp31B,_tmp31B,_tmp31B + 8},{_tmp31C,_tmp31C,_tmp31C + 7},{_tmp31D,_tmp31D,_tmp31D + 7},{_tmp31E,_tmp31E,_tmp31E + 9},{_tmp31F,_tmp31F,_tmp31F + 3},{_tmp320,_tmp320,_tmp320 + 5},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 6},{_tmp323,_tmp323,_tmp323 + 3},{_tmp324,_tmp324,_tmp324 + 4},{_tmp325,_tmp325,_tmp325 + 5},{_tmp326,_tmp326,_tmp326 + 9},{_tmp327,_tmp327,_tmp327 + 6},{_tmp328,_tmp328,_tmp328 + 7},{_tmp329,_tmp329,_tmp329 + 5},{_tmp32A,_tmp32A,_tmp32A + 8},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 4},{_tmp32E,_tmp32E,_tmp32E + 6},{_tmp32F,_tmp32F,_tmp32F + 7},{_tmp330,_tmp330,_tmp330 + 4},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 9},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 10},{_tmp335,_tmp335,_tmp335 + 9},{_tmp336,_tmp336,_tmp336 + 8},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 7},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 5},{_tmp33D,_tmp33D,_tmp33D + 9},{_tmp33E,_tmp33E,_tmp33E + 6},{_tmp33F,_tmp33F,_tmp33F + 7},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 13},{_tmp343,_tmp343,_tmp343 + 16},{_tmp344,_tmp344,_tmp344 + 14},{_tmp345,_tmp345,_tmp345 + 12},{_tmp346,_tmp346,_tmp346 + 7},{_tmp347,_tmp347,_tmp347 + 8},{_tmp348,_tmp348,_tmp348 + 12},{_tmp349,_tmp349,_tmp349 + 6},{_tmp34A,_tmp34A,_tmp34A + 8},{_tmp34B,_tmp34B,_tmp34B + 8},{_tmp34C,_tmp34C,_tmp34C + 10},{_tmp34D,_tmp34D,_tmp34D + 9},{_tmp34E,_tmp34E,_tmp34E + 13},{_tmp34F,_tmp34F,_tmp34F + 10},{_tmp350,_tmp350,_tmp350 + 9},{_tmp351,_tmp351,_tmp351 + 13},{_tmp352,_tmp352,_tmp352 + 14},{_tmp353,_tmp353,_tmp353 + 14},{_tmp354,_tmp354,_tmp354 + 12},{_tmp355,_tmp355,_tmp355 + 16},{_tmp356,_tmp356,_tmp356 + 15},{_tmp357,_tmp357,_tmp357 + 7},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 8},{_tmp35B,_tmp35B,_tmp35B + 9},{_tmp35C,_tmp35C,_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 6},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 6},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 11},{_tmp365,_tmp365,_tmp365 + 11},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 12},{_tmp368,_tmp368,_tmp368 + 13},{_tmp369,_tmp369,_tmp369 + 11},{_tmp36A,_tmp36A,_tmp36A + 11},{_tmp36B,_tmp36B,_tmp36B + 10},{_tmp36C,_tmp36C,_tmp36C + 9},{_tmp36D,_tmp36D,_tmp36D + 11},{_tmp36E,_tmp36E,_tmp36E + 12},{_tmp36F,_tmp36F,_tmp36F + 11},{_tmp370,_tmp370,_tmp370 + 17},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 8},{_tmp373,_tmp373,_tmp373 + 19},{_tmp374,_tmp374,_tmp374 + 20},{_tmp375,_tmp375,_tmp375 + 18},{_tmp376,_tmp376,_tmp376 + 9},{_tmp377,_tmp377,_tmp377 + 13},{_tmp378,_tmp378,_tmp378 + 16},{_tmp379,_tmp379,_tmp379 + 18},{_tmp37A,_tmp37A,_tmp37A + 10},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 4},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 5},{_tmp398,_tmp398,_tmp398 + 17},{_tmp399,_tmp399,_tmp399 + 12},{_tmp39A,_tmp39A,_tmp39A + 19},{_tmp39B,_tmp39B,_tmp39B + 21},{_tmp39C,_tmp39C,_tmp39C + 15},{_tmp39D,_tmp39D,_tmp39D + 20},{_tmp39E,_tmp39E,_tmp39E + 21},{_tmp39F,_tmp39F,_tmp39F + 13},{_tmp3A0,_tmp3A0,_tmp3A0 + 15},{_tmp3A1,_tmp3A1,_tmp3A1 + 17},{_tmp3A2,_tmp3A2,_tmp3A2 + 19},{_tmp3A3,_tmp3A3,_tmp3A3 + 12},{_tmp3A4,_tmp3A4,_tmp3A4 + 19},{_tmp3A5,_tmp3A5,_tmp3A5 + 17},{_tmp3A6,_tmp3A6,_tmp3A6 + 23},{_tmp3A7,_tmp3A7,_tmp3A7 + 24},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 11},{_tmp3AA,_tmp3AA,_tmp3AA + 15},{_tmp3AB,_tmp3AB,_tmp3AB + 10},{_tmp3AC,_tmp3AC,_tmp3AC + 15},{_tmp3AD,_tmp3AD,_tmp3AD + 25},{_tmp3AE,_tmp3AE,_tmp3AE + 5},{_tmp3AF,_tmp3AF,_tmp3AF + 15},{_tmp3B0,_tmp3B0,_tmp3B0 + 15},{_tmp3B1,_tmp3B1,_tmp3B1 + 11},{_tmp3B2,_tmp3B2,_tmp3B2 + 22},{_tmp3B3,_tmp3B3,_tmp3B3 + 26},{_tmp3B4,_tmp3B4,_tmp3B4 + 16},{_tmp3B5,_tmp3B5,_tmp3B5 + 16},{_tmp3B6,_tmp3B6,_tmp3B6 + 24},{_tmp3B7,_tmp3B7,_tmp3B7 + 25},{_tmp3B8,_tmp3B8,_tmp3B8 + 21},{_tmp3B9,_tmp3B9,_tmp3B9 + 22},{_tmp3BA,_tmp3BA,_tmp3BA + 16},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 25},{_tmp3BD,_tmp3BD,_tmp3BD + 35},{_tmp3BE,_tmp3BE,_tmp3BE + 23},{_tmp3BF,_tmp3BF,_tmp3BF + 24},{_tmp3C0,_tmp3C0,_tmp3C0 + 18},{_tmp3C1,_tmp3C1,_tmp3C1 + 19},{_tmp3C2,_tmp3C2,_tmp3C2 + 14},{_tmp3C3,_tmp3C3,_tmp3C3 + 19},{_tmp3C4,_tmp3C4,_tmp3C4 + 20},{_tmp3C5,_tmp3C5,_tmp3C5 + 14},{_tmp3C6,_tmp3C6,_tmp3C6 + 11},{_tmp3C7,_tmp3C7,_tmp3C7 + 23},{_tmp3C8,_tmp3C8,_tmp3C8 + 18},{_tmp3C9,_tmp3C9,_tmp3C9 + 30},{_tmp3CA,_tmp3CA,_tmp3CA + 8},{_tmp3CB,_tmp3CB,_tmp3CB + 12},{_tmp3CC,_tmp3CC,_tmp3CC + 14},{_tmp3CD,_tmp3CD,_tmp3CD + 13},{_tmp3CE,_tmp3CE,_tmp3CE + 23},{_tmp3CF,_tmp3CF,_tmp3CF + 14},{_tmp3D0,_tmp3D0,_tmp3D0 + 18},{_tmp3D1,_tmp3D1,_tmp3D1 + 8},{_tmp3D2,_tmp3D2,_tmp3D2 + 11},{_tmp3D3,_tmp3D3,_tmp3D3 + 20},{_tmp3D4,_tmp3D4,_tmp3D4 + 9},{_tmp3D5,_tmp3D5,_tmp3D5 + 16},{_tmp3D6,_tmp3D6,_tmp3D6 + 19},{_tmp3D7,_tmp3D7,_tmp3D7 + 10},{_tmp3D8,_tmp3D8,_tmp3D8 + 16},{_tmp3D9,_tmp3D9,_tmp3D9 + 11},{_tmp3DA,_tmp3DA,_tmp3DA + 14},{_tmp3DB,_tmp3DB,_tmp3DB + 11},{_tmp3DC,_tmp3DC,_tmp3DC + 15},{_tmp3DD,_tmp3DD,_tmp3DD + 22},{_tmp3DE,_tmp3DE,_tmp3DE + 16},{_tmp3DF,_tmp3DF,_tmp3DF + 17},{_tmp3E0,_tmp3E0,_tmp3E0 + 12},{_tmp3E1,_tmp3E1,_tmp3E1 + 18},{_tmp3E2,_tmp3E2,_tmp3E2 + 17},{_tmp3E3,_tmp3E3,_tmp3E3 + 12},{_tmp3E4,_tmp3E4,_tmp3E4 + 16},{_tmp3E5,_tmp3E5,_tmp3E5 + 11},{_tmp3E6,_tmp3E6,_tmp3E6 + 10},{_tmp3E7,_tmp3E7,_tmp3E7 + 14},{_tmp3E8,_tmp3E8,_tmp3E8 + 15},{_tmp3E9,_tmp3E9,_tmp3E9 + 20},{_tmp3EA,_tmp3EA,_tmp3EA + 27},{_tmp3EB,_tmp3EB,_tmp3EB + 10},{_tmp3EC,_tmp3EC,_tmp3EC + 13},{_tmp3ED,_tmp3ED,_tmp3ED + 18},{_tmp3EE,_tmp3EE,_tmp3EE + 21},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 16},{_tmp3F1,_tmp3F1,_tmp3F1 + 20},{_tmp3F2,_tmp3F2,_tmp3F2 + 15},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 15},{_tmp3F5,_tmp3F5,_tmp3F5 + 12},{_tmp3F6,_tmp3F6,_tmp3F6 + 20},{_tmp3F7,_tmp3F7,_tmp3F7 + 19},{_tmp3F8,_tmp3F8,_tmp3F8 + 20},{_tmp3F9,_tmp3F9,_tmp3F9 + 21},{_tmp3FA,_tmp3FA,_tmp3FA + 21},{_tmp3FB,_tmp3FB,_tmp3FB + 12},{_tmp3FC,_tmp3FC,_tmp3FC + 17},{_tmp3FD,_tmp3FD,_tmp3FD + 19},{_tmp3FE,_tmp3FE,_tmp3FE + 14},{_tmp3FF,_tmp3FF,_tmp3FF + 17},{_tmp400,_tmp400,_tmp400 + 23},{_tmp401,_tmp401,_tmp401 + 13},{_tmp402,_tmp402,_tmp402 + 14},{_tmp403,_tmp403,_tmp403 + 16},{_tmp404,_tmp404,_tmp404 + 16},{_tmp405,_tmp405,_tmp405 + 8},{_tmp406,_tmp406,_tmp406 + 19},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 14},{_tmp409,_tmp409,_tmp409 + 19},{_tmp40A,_tmp40A,_tmp40A + 20},{_tmp40B,_tmp40B,_tmp40B + 11},{_tmp40C,_tmp40C,_tmp40C + 22},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,_tmp40E + 23},{_tmp40F,_tmp40F,_tmp40F + 20},{_tmp410,_tmp410,_tmp410 + 22},{_tmp411,_tmp411,_tmp411 + 23},{_tmp412,_tmp412,_tmp412 + 24},{_tmp413,_tmp413,_tmp413 + 24},{_tmp414,_tmp414,_tmp414 + 15},{_tmp415,_tmp415,_tmp415 + 20},{_tmp416,_tmp416,_tmp416 + 22},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 20},{_tmp419,_tmp419,_tmp419 + 26},{_tmp41A,_tmp41A,_tmp41A + 16},{_tmp41B,_tmp41B,_tmp41B + 17},{_tmp41C,_tmp41C,_tmp41C + 15},{_tmp41D,_tmp41D,_tmp41D + 19},{_tmp41E,_tmp41E,_tmp41E + 17},{_tmp41F,_tmp41F,_tmp41F + 19},{_tmp420,_tmp420,_tmp420 + 25},{_tmp421,_tmp421,_tmp421 + 26},{_tmp422,_tmp422,_tmp422 + 9},{_tmp423,_tmp423,_tmp423 + 20},{_tmp424,_tmp424,_tmp424 + 17},{_tmp425,_tmp425,_tmp425 + 18},{_tmp426,_tmp426,_tmp426 + 11},{_tmp427,_tmp427,_tmp427 + 12}};
# 1608
static short Cyc_yyr1[523]={0,147,148,148,148,148,148,148,148,148,148,148,149,149,150,150,150,151,151,151,152,152,153,153,153,153,154,154,155,156,157,158,159,159,159,159,159,159,159,160,160,161,161,162,162,162,162,162,162,162,162,162,162,163,163,163,163,163,163,163,164,164,165,166,166,167,167,167,167,167,167,168,168,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,170,171,171,171,172,172,172,173,173,174,174,174,175,175,175,175,175,176,176,177,177,178,178,179,179,180,181,181,182,182,183,184,184,184,184,184,184,185,185,185,185,185,185,186,187,187,188,188,188,188,188,189,189,189,190,190,191,191,191,191,192,192,192,193,193,194,194,195,195,196,196,196,196,196,196,196,196,196,196,197,197,197,197,197,197,197,197,197,197,197,198,198,199,200,200,201,201,201,201,201,201,201,201,202,202,202,203,203,204,204,204,205,205,205,206,206,207,207,207,207,208,208,209,209,210,210,211,211,212,212,213,213,214,214,214,214,215,215,216,216,217,217,217,218,219,219,220,220,221,221,221,221,221,222,222,222,222,223,224,224,225,225,226,226,227,227,227,227,227,228,228,229,229,229,230,230,230,230,230,230,230,230,230,230,230,231,231,231,231,231,231,231,232,232,233,234,234,235,235,236,236,236,236,236,236,236,236,237,237,237,237,237,237,238,238,238,238,238,238,239,239,239,239,239,239,239,239,239,239,239,239,239,239,240,240,240,240,240,240,240,240,241,242,242,243,243,244,244,245,245,246,246,247,247,248,248,248,249,249,249,249,249,250,250,250,251,251,251,252,252,252,252,253,253,254,254,254,254,254,254,255,256,257,257,257,257,257,257,257,257,257,257,257,257,257,257,257,258,258,258,259,259,260,260,261,261,261,262,262,263,263,264,264,264,265,265,265,265,265,265,265,265,265,265,265,266,266,266,266,266,266,266,267,268,268,269,269,270,270,271,271,272,272,273,273,273,274,274,274,274,274,275,275,275,276,276,276,277,277,277,277,278,278,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,280,280,280,281,281,281,281,281,281,281,281,281,281,282,282,282,282,283,283,283,283,283,283,283,283,283,283,284,285,285,286,286,286,286,286,287,287,288,288,289,289,290,290,291,291};
# 1664
static short Cyc_yyr2[523]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1720
static short Cyc_yydefact[1041]={0,19,53,54,55,56,58,73,76,77,78,79,80,81,82,83,97,98,99,115,116,49,0,0,59,0,0,150,90,94,0,0,0,0,0,0,0,40,513,216,515,514,516,0,0,74,0,202,202,201,1,0,17,0,0,18,0,0,43,51,45,71,47,84,85,0,86,0,39,161,0,186,189,87,165,113,57,56,50,0,101,512,0,513,508,509,510,511,113,0,378,0,0,0,0,239,0,380,381,28,30,0,0,0,0,0,0,0,0,151,0,0,0,0,0,0,0,199,200,2,0,0,0,0,0,32,0,121,122,124,44,52,46,48,117,517,518,113,113,39,41,39,0,21,0,218,0,174,162,187,196,195,0,0,193,194,197,198,207,189,0,72,0,57,105,0,103,0,513,388,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,498,477,0,0,0,0,0,480,478,479,0,405,407,421,429,431,433,435,437,439,442,447,450,453,457,0,459,481,496,495,0,389,387,35,0,0,113,0,0,0,131,127,129,258,260,0,0,9,10,0,0,0,113,519,520,217,96,0,0,166,75,237,0,234,0,3,0,5,0,288,0,33,0,0,39,21,0,118,119,144,112,0,148,0,0,0,0,0,0,0,0,0,0,0,0,513,291,293,0,301,295,0,299,281,282,283,0,284,285,286,0,42,21,124,20,22,265,0,224,240,0,0,220,218,0,204,0,0,0,209,60,208,190,0,0,106,102,0,0,382,0,0,402,0,0,0,0,254,398,403,0,400,0,467,0,423,457,0,424,425,0,0,0,0,0,0,0,0,0,460,461,39,0,0,0,463,464,462,0,379,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,411,412,413,414,415,416,417,418,419,420,410,0,465,0,487,488,0,0,0,500,0,113,395,396,0,393,0,241,0,0,0,0,261,232,0,132,137,133,135,128,130,218,0,267,259,268,522,521,0,89,93,0,91,39,95,111,66,65,0,63,167,218,236,163,267,238,175,176,0,88,203,29,0,31,0,0,0,0,123,125,243,242,21,23,108,120,0,0,0,139,140,142,0,113,113,156,0,0,0,0,0,513,0,39,0,330,331,332,0,0,334,0,0,39,0,302,296,124,300,294,292,24,0,173,225,0,0,0,231,219,226,170,0,0,0,220,172,206,205,168,204,0,0,210,61,114,0,107,428,104,100,391,392,257,0,399,253,255,386,0,0,0,0,244,249,0,0,0,0,0,0,0,0,0,0,0,0,0,499,506,0,505,406,430,0,432,434,436,438,440,441,445,446,443,444,448,449,451,452,454,455,456,409,408,486,483,0,485,0,0,0,383,0,390,34,0,384,0,0,262,138,134,136,0,220,0,204,0,269,0,218,0,280,264,0,38,113,0,0,0,131,0,113,0,218,0,185,164,235,4,6,0,37,0,25,0,143,126,144,0,0,220,149,158,157,0,0,152,0,0,0,309,0,0,0,0,0,0,329,333,0,0,0,0,297,290,39,26,266,218,0,228,0,0,221,0,171,224,212,169,192,191,210,188,0,7,0,256,401,404,466,0,0,245,0,250,469,0,0,0,0,473,476,0,0,0,0,0,458,502,0,0,484,482,0,0,394,397,385,263,233,276,0,270,271,204,0,0,220,204,0,92,220,0,0,0,62,64,0,177,0,0,220,0,204,0,0,0,141,146,145,117,147,159,156,156,0,0,0,0,0,0,0,0,0,0,0,0,0,309,335,0,0,298,27,220,0,229,227,0,220,0,211,0,8,492,0,491,0,246,251,0,0,0,0,426,427,486,485,504,0,507,422,501,503,0,275,273,279,278,0,272,204,117,0,69,67,68,178,184,181,0,183,179,204,0,36,0,0,155,154,303,309,0,0,0,0,0,0,337,338,340,342,344,346,348,350,353,358,361,364,368,370,376,377,0,39,306,315,0,0,0,0,0,0,0,0,0,0,336,287,214,230,222,213,218,13,0,14,468,0,0,252,470,0,0,475,474,489,0,277,274,0,0,182,180,289,110,0,0,0,309,0,374,0,0,371,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,372,310,0,317,0,0,0,325,0,0,0,0,0,0,308,0,220,12,15,0,494,493,0,0,0,490,109,0,160,304,305,0,0,0,0,309,311,341,0,343,345,347,349,351,352,356,357,354,355,359,360,362,363,365,366,367,0,316,318,319,0,327,326,0,321,0,0,0,223,215,16,0,0,0,0,0,307,373,0,369,312,0,39,320,328,322,323,0,248,247,471,0,70,0,339,309,313,324,0,375,314,472,0,0,0};
# 1827
static short Cyc_yydefgoto[145]={1038,50,694,885,51,303,52,287,53,460,54,462,55,56,141,57,58,531,228,446,447,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,427,476,477,478,66,67,653,654,655,68,479,69,451,70,71,158,159,72,117,527,318,691,611,73,612,521,682,513,517,518,422,311,251,94,95,552,468,553,333,334,335,232,304,305,613,433,290,464,291,292,293,294,295,769,296,297,846,847,848,849,850,851,852,853,854,855,856,857,858,859,860,861,336,412,413,337,338,339,298,199,399,200,537,201,202,203,204,205,206,207,208,209,210,211,212,213,214,794,215,568,569,216,217,75,886,244,437};
# 1845
static short Cyc_yypact[1041]={2878,- -32768,- -32768,- -32768,- -32768,- 61,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3621,147,3308,- -32768,137,- 23,- -32768,- 19,26,37,90,120,132,158,170,253,- -32768,- -32768,168,- -32768,- -32768,- -32768,211,749,244,243,231,231,- -32768,- -32768,2740,- -32768,195,259,- -32768,360,910,3621,3621,3621,- -32768,3621,- -32768,- -32768,575,- -32768,137,3456,320,99,650,1049,- -32768,- -32768,323,343,368,- -32768,137,390,- -32768,3949,123,- -32768,- -32768,- -32768,- -32768,323,6204,- -32768,401,437,3949,436,446,485,- -32768,- 41,- -32768,- -32768,3695,3695,2740,2740,3695,514,6204,541,- -32768,298,505,854,511,298,3917,6204,- -32768,- -32768,- -32768,2740,3015,2740,3015,52,- -32768,532,553,- -32768,3373,- -32768,- -32768,- -32768,- -32768,3917,- -32768,- -32768,323,241,1601,- -32768,3456,910,558,3695,1218,4557,- -32768,320,- -32768,- -32768,- -32768,562,571,- -32768,- -32768,- -32768,- -32768,- 18,1049,3695,- -32768,3015,- -32768,580,619,587,137,106,- -32768,3949,109,871,6441,622,6204,6283,627,629,633,635,641,643,654,665,6520,6520,- -32768,- -32768,- -32768,2316,669,6599,6599,6599,- -32768,- -32768,- -32768,289,- -32768,- -32768,- 32,691,671,672,673,311,24,495,389,138,- -32768,1061,6599,100,- 48,- -32768,699,1312,715,- -32768,- -32768,728,6204,323,1312,710,293,3917,3991,3917,581,- -32768,73,73,- -32768,- -32768,78,736,295,323,- -32768,- -32768,- -32768,- -32768,35,738,- -32768,- -32768,688,309,- -32768,746,- -32768,748,- -32768,751,764,761,- -32768,854,4656,3456,558,772,3917,- -32768,1063,762,137,774,766,159,770,4062,783,791,781,796,4755,4062,263,786,768,- -32768,- -32768,795,1745,1745,910,1745,- -32768,- -32768,- -32768,803,- -32768,- -32768,- -32768,101,- -32768,558,802,- -32768,- -32768,790,94,822,- -32768,84,804,807,216,805,605,800,3695,6204,- -32768,821,- -32768,- -32768,94,818,137,- -32768,6204,819,- -32768,824,825,- -32768,298,6204,3949,167,- -32768,- -32768,- -32768,826,830,2316,- -32768,3917,- -32768,- -32768,4161,- -32768,859,6204,6204,6204,6204,6204,6204,3917,664,2316,- -32768,- -32768,1889,839,316,6204,- -32768,- -32768,- -32768,6204,- -32768,6599,6204,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6204,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6204,- -32768,298,- -32768,- -32768,4851,298,6204,- -32768,832,323,- -32768,- -32768,840,835,3949,- -32768,269,871,841,3695,- -32768,850,855,- -32768,3991,3991,3991,- -32768,- -32768,3164,4947,171,- -32768,500,- -32768,- -32768,84,- -32768,- -32768,3695,- -32768,3539,- -32768,865,- -32768,849,860,870,- -32768,3090,- -32768,513,190,- -32768,- -32768,- -32768,3917,- -32768,- -32768,- -32768,2740,- -32768,2740,887,875,873,- -32768,- -32768,- -32768,- -32768,558,- -32768,- -32768,- -32768,944,6204,876,874,- -32768,- 29,226,323,323,726,6204,6204,878,881,6204,877,977,2177,890,- -32768,- -32768,- -32768,364,969,- -32768,5046,6204,2033,2455,- -32768,- -32768,3373,- -32768,- -32768,- -32768,- -32768,3695,- -32768,- -32768,3917,886,321,- -32768,- -32768,882,- -32768,84,896,3843,807,- -32768,- -32768,- -32768,- -32768,605,122,899,771,- -32768,- -32768,2602,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,880,- -32768,- -32768,- -32768,- -32768,1176,900,892,922,- -32768,- -32768,656,4656,903,909,911,912,386,908,914,277,916,918,6362,- -32768,- -32768,920,926,- -32768,691,332,671,672,673,311,24,24,495,495,495,495,389,389,138,138,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,924,- -32768,9,3695,4458,- -32768,3200,- -32768,- -32768,931,- -32768,175,942,- -32768,- -32768,- -32768,- -32768,930,807,936,605,932,500,3695,3769,5142,- -32768,- -32768,73,- -32768,323,725,939,35,3238,940,323,3695,1218,5241,- -32768,513,- -32768,- -32768,- -32768,943,- -32768,962,- -32768,829,- -32768,- -32768,1063,6204,6204,807,- -32768,- -32768,- -32768,950,137,560,394,402,6204,779,457,4062,946,5340,5436,589,- -32768,- -32768,953,957,954,461,1745,- -32768,3456,- -32768,790,958,3695,- -32768,961,84,- -32768,959,- -32768,152,- -32768,- -32768,- -32768,- -32768,771,- -32768,963,- -32768,2740,- -32768,- -32768,- -32768,966,278,971,- -32768,4260,- -32768,- -32768,6204,1066,6204,6283,- -32768,- -32768,298,298,966,970,4458,- -32768,- -32768,6204,6204,- -32768,- -32768,94,684,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,972,- -32768,- -32768,605,94,973,807,605,960,- -32768,807,463,974,976,- -32768,- -32768,978,- -32768,94,979,807,980,605,965,6204,985,- -32768,- -32768,- -32768,3917,- -32768,983,41,726,4062,998,992,6729,991,975,4062,6204,5532,613,5628,617,5724,779,- -32768,1010,1011,- -32768,- -32768,807,193,- -32768,- -32768,1018,807,3917,- -32768,260,- -32768,- -32768,415,- -32768,6204,- -32768,- -32768,4656,1005,1007,1012,- -32768,859,1008,1014,- -32768,704,- -32768,- -32768,- -32768,- -32768,4458,- -32768,- -32768,- -32768,- -32768,1015,- -32768,605,3917,1030,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1017,- -32768,- -32768,605,476,- -32768,1024,3917,- -32768,- -32768,1111,779,1027,6678,1025,2455,6599,1020,- -32768,- 27,- -32768,1062,1026,720,787,153,793,464,214,- -32768,- -32768,- -32768,- -32768,1074,6599,1889,- -32768,- -32768,477,4062,501,5820,4062,527,5916,6012,623,1045,- -32768,- -32768,- -32768,- -32768,1050,- -32768,958,- -32768,1046,630,- -32768,351,347,- -32768,- -32768,3917,1147,- -32768,- -32768,- -32768,4359,- -32768,- -32768,1053,1058,- -32768,- -32768,- -32768,- -32768,547,4062,1060,779,2316,- -32768,3917,1055,- -32768,1457,6599,6204,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6599,6204,- -32768,- -32768,1069,- -32768,4062,4062,582,- -32768,4062,4062,592,4062,599,6108,- -32768,84,807,- -32768,- -32768,541,- -32768,- -32768,2455,1064,1059,- -32768,- -32768,1083,- -32768,- -32768,- -32768,1071,1067,1073,6599,779,- -32768,691,358,671,672,672,311,24,24,495,495,495,495,389,389,138,138,- -32768,- -32768,- -32768,385,- -32768,- -32768,- -32768,4062,- -32768,- -32768,4062,- -32768,4062,4062,606,- -32768,- -32768,- -32768,1075,759,1076,3917,1077,- -32768,966,278,- -32768,- -32768,6204,1457,- -32768,- -32768,- -32768,- -32768,4062,- -32768,- -32768,- -32768,1078,- -32768,494,- -32768,779,- -32768,- -32768,1079,- -32768,- -32768,- -32768,1201,1203,- -32768};
# 1952
static short Cyc_yypgoto[145]={- -32768,225,- -32768,254,- -32768,- 218,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,47,- -32768,- 110,110,- -32768,- -32768,0,583,- -32768,32,- 189,1094,5,- -32768,- -32768,- 149,- -32768,348,1177,- 732,- -32768,- -32768,- -32768,951,955,458,433,- -32768,- -32768,569,- -32768,- -32768,128,- -32768,- -32768,80,- 148,1148,- 277,185,- -32768,1065,- -32768,- -32768,1171,- 349,- -32768,531,- 125,- 152,- 114,- 268,271,538,559,- 427,- 466,- 108,- 375,- 120,- -32768,- 246,- 173,- 557,- 337,- -32768,907,- 147,- 68,- 118,- 177,- 357,148,- -32768,- -32768,- -32768,- 44,- 252,- -32768,- 707,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,107,1019,- -32768,696,828,- -32768,267,555,- -32768,- 145,- 293,- 153,- 346,- 342,- 355,879,- 344,- 373,- 260,- 370,- 369,- 115,468,479,893,235,- -32768,- 400,- -32768,- 6,475,- 53,6,- 91,- 9};
# 1974
static short Cyc_yytable[6876]={59,577,578,346,594,62,319,250,554,621,583,584,136,585,586,467,574,97,326,262,309,59,571,347,143,312,62,835,573,576,343,310,60,233,234,503,504,237,506,542,425,723,321,360,112,471,651,652,76,680,647,59,444,60,432,136,62,407,59,59,59,368,59,62,62,62,916,62,59,147,876,137,453,62,616,421,97,363,364,365,224,636,509,60,100,263,225,97,408,900,60,60,60,683,60,616,39,300,400,670,60,450,648,59,59,369,101,564,62,62,917,317,376,377,240,140,579,580,581,582,59,59,59,59,113,62,62,62,62,59,96,78,366,908,62,60,60,129,677,59,728,59,445,721,62,514,62,636,147,378,379,102,60,60,60,60,516,103,808,257,434,60,59,- 153,97,434,97,62,130,131,132,60,133,60,535,635,140,258,142,687,536,434,644,401,402,403,288,536,299,169,19,20,246,548,60,549,455,523,39,435,220,439,967,635,435,515,38,561,972,563,104,436,97,170,455,41,436,510,470,97,435,508,301,328,366,438,44,404,440,424,170,171,405,406,426,425,425,425,329,142,541,924,925,- 240,38,105,- 240,529,171,289,688,142,642,41,38,686,149,106,766,512,436,554,59,41,1016,732,38,62,79,587,588,589,308,384,110,41,119,327,926,927,- 218,385,386,- 218,516,107,485,59,59,486,59,544,62,62,60,62,511,38,429,331,332,629,109,727,430,40,41,42,703,299,592,1032,533,436,595,120,121,449,532,60,60,880,60,430,1036,411,629,97,235,236,536,436,411,38,288,288,308,288,111,522,301,40,41,42,730,253,254,255,256,932,114,605,116,474,757,758,198,933,934,59,711,402,403,421,62,799,160,38,516,505,115,622,142,269,239,40,41,42,554,122,123,759,884,498,815,241,793,322,819,602,499,60,366,242,250,674,630,289,289,455,289,374,375,831,404,241,288,39,97,712,406,97,366,242,883,420,431,367,366,124,781,455,490,442,424,424,424,455,497,426,426,426,456,620,452,39,172,457,43,366,543,59,676,679,566,144,62,145,160,628,716,634,480,146,421,366,798,361,241,956,59,675,59,719,162,62,242,62,818,289,366,899,821,60,643,74,799,163,958,722,746,366,829,903,667,268,270,366,623,1017,416,59,60,735,60,746,62,80,98,734,99,59,536,536,736,59,62,749,366,366,62,167,751,748,708,879,1018,366,750,74,882,601,764,60,683,366,74,382,218,516,765,383,74,60,59,803,690,60,664,62,135,97,138,887,219,74,496,888,672,979,980,142,140,890,166,804,221,98,985,986,799,987,988,783,976,977,60,798,98,222,973,788,417,249,810,630,975,978,74,74,366,380,381,135,366,770,822,74,443,780,455,823,267,455,97,74,74,74,74,366,366,142,930,249,904,938,931,361,795,223,784,142,937,739,431,142,620,74,43,559,805,806,1035,361,366,617,888,618,238,940,782,43,245,619,452,634,572,74,631,247,632,341,166,344,344,98,633,98,38,673,366,798,259,357,358,944,40,41,42,480,344,344,344,971,981,982,983,984,525,526,456,252,59,596,59,964,260,62,302,62,762,344,38,763,1005,637,423,638,428,315,40,41,42,98,59,690,134,913,316,62,98,314,81,323,60,366,60,725,429,776,996,325,366,811,430,366,47,48,49,288,999,299,366,267,454,816,60,1001,906,366,914,651,652,870,1023,74,366,873,136,827,366,324,454,949,482,959,366,487,936,342,954,656,657,955,348,660,349,598,665,693,350,97,351,968,74,969,1031,671,352,952,353,38,84,187,188,85,86,87,701,702,41,354,289,370,142,47,48,49,16,17,18,356,355,344,191,38,362,957,166,516,344,767,768,40,41,42,812,813,98,673,371,1007,372,480,373,449,469,989,990,991,409,430,414,47,48,49,896,897,480,649,650,480,741,742,743,415,344,419,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,1015,38,608,609,610,920,921,441,1027,59,41,448,43,458,62,459,530,1030,461,862,465,44,922,923,928,929,1025,366,481,47,48,49,249,463,98,837,838,98,484,472,483,60,488,492,501,469,493,136,555,556,557,558,249,560,81,491,288,839,500,249,59,494,567,866,792,62,570,502,795,454,507,261,454,436,512,773,775,519,524,528,74,38,74,520,43,534,538,590,344,40,41,42,60,546,454,539,540,368,547,591,454,474,597,600,567,249,38,288,82,47,48,49,565,599,604,41,249,740,606,289,330,625,607,168,84,249,44,85,86,87,615,624,41,626,47,48,49,88,627,639,640,645,646,89,641,90,91,659,331,332,658,92,662,661,74,666,668,93,678,695,699,939,38,59,943,681,833,98,62,684,289,41,689,698,700,125,704,705,344,706,707,709,44,867,869,714,872,710,875,713,47,48,49,717,718,60,38,720,726,567,965,39,567,729,40,41,42,43,889,731,288,733,744,747,755,754,474,760,771,777,98,249,778,308,47,48,49,779,785,791,787,423,715,994,995,249,801,997,998,820,1000,796,865,807,832,814,817,824,454,825,834,826,828,830,469,836,198,150,151,152,344,344,387,454,840,841,454,864,153,154,155,156,157,289,761,877,878,881,891,892,893,894,942,901,907,946,948,895,898,1019,902,905,1020,909,1021,1022,915,912,469,918,388,389,390,391,392,393,394,395,396,397,919,935,950,953,74,38,1033,960,951,738,962,344,361,40,41,42,963,966,970,974,398,1010,344,753,993,474,1011,1009,1012,475,1013,1014,1024,47,48,49,1039,992,1040,1026,1028,1034,1037,248,1006,745,466,108,567,81,756,1003,267,148,118,473,790,1004,789,320,1008,7,8,9,10,11,12,13,14,15,16,17,18,19,20,786,545,98,697,418,603,863,249,562,1029,0,575,0,0,0,22,0,469,0,0,800,0,802,0,0,0,135,27,82,0,469,0,0,809,0,28,29,0,0,267,696,0,0,168,84,0,32,85,86,87,34,0,41,0,249,0,0,88,35,36,0,0,0,89,0,90,91,0,331,332,911,92,0,344,0,0,0,93,0,0,0,0,306,0,0,307,0,0,0,0,0,344,39,40,0,42,43,0,308,0,0,0,0,0,0,0,0,45,46,0,81,0,0,0,469,0,0,0,0,0,0,0,0,0,0,0,0,0,469,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,344,0,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,0,82,0,0,0,0,0,0,0,0,0,0,0,410,0,0,168,84,0,0,85,86,87,0,0,41,0,135,0,0,88,0,0,0,0,344,89,0,90,91,0,0,0,0,92,0,0,0,0,469,93,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,767,768,21,173,174,271,344,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 309,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 39,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,489,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,663,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,359,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,692,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,306,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,308,0,0,27,0,0,0,449,0,45,46,28,29,430,0,47,48,49,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,306,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,308,0,0,27,0,0,0,429,82,45,46,28,29,430,0,47,48,49,0,0,724,0,32,168,84,0,34,85,86,87,0,0,41,0,35,36,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,0,0,0,38,81,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,474,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 39,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,306,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,308,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,685,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,81,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,82,0,0,0,0,45,46,28,29,0,0,0,0,0,0,168,84,0,32,85,86,87,34,0,41,0,0,0,0,88,35,36,0,0,0,89,0,90,91,0,0,0,0,92,0,0,173,174,271,93,272,273,274,275,276,277,278,279,0,81,0,175,280,0,39,176,0,281,43,0,0,0,0,177,178,179,180,0,0,45,46,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,489,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,550,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,345,551,0,173,174,0,190,0,0,191,0,331,332,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,345,797,0,173,174,0,190,0,0,191,0,331,332,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,345,961,0,173,174,0,190,0,0,191,0,331,332,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,345,0,0,173,174,0,190,0,0,191,0,331,332,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,313,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,345,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,495,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,593,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,614,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,669,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,737,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,752,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,772,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,774,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,868,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,871,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,874,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,941,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,945,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,947,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,1002,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,345,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,715,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,340,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,356,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,842,843,0,0,0,0,0,0,185,186,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,910,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,82,0,196,197,0,0,0,0,0,0,0,0,0,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,844,0,90,91,0,0,0,0,92,0,0,845,0,0,93,195,0,0,196,197};
# 2665
static short Cyc_yycheck[6876]={0,374,375,176,404,0,158,115,345,436,380,381,65,382,383,261,371,23,167,129,145,21,368,176,68,145,21,759,370,373,175,145,0,101,102,287,288,105,290,332,229,598,160,190,44,263,5,6,109,515,79,51,17,21,231,108,51,105,58,59,60,93,62,58,59,60,93,62,68,69,777,65,249,68,431,227,82,192,193,194,121,456,300,51,107,129,127,93,136,821,58,59,60,520,62,452,114,141,213,499,68,249,131,103,104,137,125,359,103,104,137,129,88,89,108,68,376,377,378,379,120,121,122,123,44,120,121,122,123,129,23,21,123,840,129,103,104,57,513,139,606,141,107,134,139,61,141,522,148,125,126,125,120,121,122,123,308,120,715,107,87,129,162,122,170,87,172,162,58,59,60,139,62,141,323,452,129,125,68,528,325,87,475,83,84,85,139,332,141,82,20,21,112,340,162,342,249,311,114,126,93,123,909,480,126,121,107,354,915,356,120,138,218,107,267,116,138,123,262,225,126,120,142,114,123,234,127,127,237,229,107,125,132,133,229,424,425,426,129,129,331,88,89,120,107,125,123,315,125,139,128,141,470,116,107,523,71,125,658,107,138,598,262,116,971,614,107,262,121,384,385,386,120,135,106,116,51,170,125,126,128,143,144,131,436,127,127,287,288,130,290,124,287,288,262,290,305,107,127,132,133,449,49,128,133,115,116,117,554,262,401,1018,321,138,405,120,121,127,318,287,288,128,290,133,1031,218,474,333,103,104,475,138,225,107,287,288,120,290,127,123,260,115,116,117,612,120,121,122,123,135,106,419,121,127,647,648,89,143,144,359,83,84,85,515,359,702,125,107,520,289,127,439,262,132,107,115,116,117,715,120,121,649,122,120,733,107,108,162,737,120,127,359,123,115,502,505,449,287,288,452,290,90,91,752,127,107,359,114,414,132,133,417,123,115,789,122,231,128,123,59,672,474,274,128,424,425,426,480,280,424,425,426,123,433,249,114,88,128,118,123,333,441,510,122,128,125,441,127,125,449,565,451,267,133,606,123,702,190,107,108,460,505,462,131,121,460,115,462,736,359,123,820,740,441,474,0,813,109,131,597,628,123,750,832,120,137,138,123,441,131,223,491,460,618,462,643,491,22,23,617,25,501,647,648,618,505,501,632,123,123,505,121,632,631,128,783,131,123,632,44,788,414,128,491,951,123,51,138,127,681,128,142,57,501,534,708,531,505,491,534,65,547,67,128,107,70,279,132,501,922,923,441,505,799,79,708,120,82,928,929,897,930,931,677,919,920,534,813,93,123,916,685,224,115,719,628,918,921,103,104,123,86,87,108,123,128,123,112,240,128,643,128,134,646,600,120,121,122,123,123,123,491,138,145,128,128,142,340,699,124,678,501,864,622,429,505,616,142,118,352,711,712,128,356,123,125,132,127,114,128,674,118,127,133,449,635,369,162,125,128,127,173,167,175,176,170,133,172,107,501,123,897,120,185,186,128,115,116,117,474,192,193,194,915,924,925,926,927,63,64,123,116,672,406,674,128,123,672,120,674,120,213,107,123,952,460,228,462,230,127,115,116,117,218,694,690,121,844,127,694,225,146,38,123,672,123,674,600,127,120,128,124,123,722,133,123,135,136,137,672,128,674,123,265,249,734,694,128,836,123,845,5,6,120,128,260,123,120,791,748,123,122,267,120,269,892,123,272,863,127,120,484,485,123,127,488,127,409,491,534,127,767,127,910,289,912,1018,500,127,883,127,107,108,109,110,111,112,113,122,123,116,127,672,92,674,135,136,137,17,18,19,127,127,325,130,107,127,888,323,951,332,22,23,115,116,117,122,123,333,661,139,958,140,628,141,127,261,932,933,934,121,133,107,135,136,137,122,123,643,481,482,646,107,108,109,107,368,127,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,970,107,424,425,426,140,141,126,1010,864,116,128,118,122,864,122,316,1017,122,767,114,127,90,91,86,87,122,123,121,135,136,137,429,124,414,762,763,417,127,122,121,864,127,107,131,345,120,955,348,349,350,351,449,353,38,127,864,764,127,456,915,120,362,770,694,915,366,127,1014,449,122,124,452,138,107,663,664,128,128,134,460,107,462,131,118,122,122,387,475,115,116,117,915,122,474,126,126,93,123,399,480,127,125,123,404,502,107,915,92,135,136,137,128,128,128,116,513,624,123,864,104,127,122,107,108,522,127,111,112,113,430,121,116,128,135,136,137,121,123,107,120,120,123,127,126,129,130,121,132,133,127,135,30,131,534,120,42,141,127,134,123,868,107,1018,871,138,754,547,1018,128,915,116,128,128,107,120,128,123,565,123,123,128,127,771,772,122,774,128,776,128,135,136,137,128,123,1018,107,128,122,499,907,114,502,128,115,116,117,118,796,128,1018,134,128,128,107,127,127,122,127,121,600,618,120,120,135,136,137,128,122,121,126,628,121,940,941,632,25,944,945,134,947,125,122,128,134,128,128,128,628,128,120,128,128,128,554,127,844,63,64,65,647,648,56,643,121,128,646,131,74,75,76,77,78,1018,654,120,120,114,128,127,123,128,870,108,28,873,874,128,128,996,128,122,999,121,1001,1002,131,127,598,92,94,95,96,97,98,99,100,101,102,103,139,92,122,122,694,107,1023,25,123,619,122,708,910,115,116,117,123,122,128,917,124,127,719,633,120,127,108,128,122,131,128,123,122,135,136,137,0,935,0,128,128,128,128,114,955,627,260,35,658,38,646,949,759,70,48,265,690,951,685,159,958,8,9,10,11,12,13,14,15,16,17,18,19,20,21,681,334,767,547,225,417,767,789,355,1014,- 1,372,- 1,- 1,- 1,37,- 1,702,- 1,- 1,705,- 1,707,- 1,- 1,- 1,791,49,92,- 1,715,- 1,- 1,718,- 1,57,58,- 1,- 1,821,104,- 1,- 1,107,108,- 1,68,111,112,113,72,- 1,116,- 1,836,- 1,- 1,121,80,81,- 1,- 1,- 1,127,- 1,129,130,- 1,132,133,842,135,- 1,845,- 1,- 1,- 1,141,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,863,114,115,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,38,- 1,- 1,- 1,799,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,916,- 1,918,919,920,921,922,923,924,925,926,927,928,929,930,931,932,933,934,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,955,- 1,- 1,121,- 1,- 1,- 1,- 1,970,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,897,141,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1017,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,37,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,49,- 1,- 1,127,- 1,129,130,- 1,57,58,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,- 1,- 1,114,115,116,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,- 1,129,130,57,58,133,- 1,135,136,137,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,92,129,130,57,58,133,- 1,135,136,137,- 1,- 1,104,- 1,68,107,108,- 1,72,111,112,113,- 1,- 1,116,- 1,80,81,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,- 1,- 1,107,38,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,107,108,- 1,45,111,112,113,49,- 1,116,- 1,- 1,- 1,- 1,121,57,58,59,- 1,- 1,127,- 1,129,130,- 1,- 1,68,69,135,- 1,72,- 1,- 1,- 1,141,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,124,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,- 1,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,129,130,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,38,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,92,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,68,111,112,113,72,- 1,116,- 1,- 1,- 1,- 1,121,80,81,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,25,26,27,141,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,114,44,- 1,46,118,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,129,130,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,32,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,92,- 1,145,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E2[8]="stdcall";static char _tmp4E3[6]="cdecl";static char _tmp4E4[9]="fastcall";static char _tmp4E5[9]="noreturn";static char _tmp4E6[6]="const";static char _tmp4E7[8]="aligned";static char _tmp4E8[7]="packed";static char _tmp4E9[7]="shared";static char _tmp4EA[7]="unused";static char _tmp4EB[5]="weak";static char _tmp4EC[10]="dllimport";static char _tmp4ED[10]="dllexport";static char _tmp4EE[23]="no_instrument_function";static char _tmp4EF[12]="constructor";static char _tmp4F0[11]="destructor";static char _tmp4F1[22]="no_check_memory_usage";static char _tmp4F2[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9AD(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9AC,unsigned int*_tmp9AB,union Cyc_YYSTYPE**_tmp9AA){for(*_tmp9AC=0;*_tmp9AC < *_tmp9AB;(*_tmp9AC)++){(*_tmp9AA)[*_tmp9AC]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9B2(unsigned int*_tmp9B1,unsigned int*_tmp9B0,struct Cyc_Yyltype**_tmp9AF){for(*_tmp9B1=0;*_tmp9B1 < *_tmp9B0;(*_tmp9B1)++){(*_tmp9AF)[*_tmp9B1]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9B9(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9B8,unsigned int*_tmp9B7,short**_tmp9B5){for(*_tmp9B8=0;*_tmp9B8 < *_tmp9B7;(*_tmp9B8)++){(*_tmp9B5)[*_tmp9B8]=(short)(
# 216
*_tmp9B8 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9B8)): 0);}}static void _tmp9BF(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9BE,unsigned int*_tmp9BD,union Cyc_YYSTYPE**_tmp9BB){for(*_tmp9BE=0;*_tmp9BE < *_tmp9BD;(*_tmp9BE)++){(*_tmp9BB)[*_tmp9BE]=(union Cyc_YYSTYPE)(
# 219
*_tmp9BE <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9BE)):*yylval);}}static void _tmp9C5(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9C4,unsigned int*_tmp9C3,struct Cyc_Yyltype**_tmp9C1){for(*_tmp9C4=0;*_tmp9C4 < *_tmp9C3;(*_tmp9C4)++){(*_tmp9C1)[*_tmp9C4]=(struct Cyc_Yyltype)(
# 222
*_tmp9C4 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9C4)):
 Cyc_yynewloc());}}static void _tmpEB3(unsigned int*_tmpEB2,unsigned int*_tmpEB1,char**_tmpEAF){for(*_tmpEB2=0;*_tmpEB2 < *_tmpEB1;(*_tmpEB2)++){(*_tmpEAF)[*_tmpEB2]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp429=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp429;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9A6;union Cyc_YYSTYPE yylval=((_tmp9A6.YYINITIALSVAL).val=0,(((_tmp9A6.YYINITIALSVAL).tag=64,_tmp9A6)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9A8;unsigned int _tmp9A7;struct _dyneither_ptr yyss=(_tmp9A7=200,((_tmp9A8=_region_calloc(yyregion,sizeof(short),_tmp9A7),_tag_dyneither(_tmp9A8,sizeof(short),_tmp9A7))));
# 158
int yyvsp_offset;
unsigned int _tmp9AC;unsigned int _tmp9AB;union Cyc_YYSTYPE*_tmp9AA;unsigned int _tmp9A9;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9A9=(unsigned int)200,((_tmp9AA=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A9)),((((_tmp9AB=_tmp9A9,_tmp9AD(& yylval,& _tmp9AC,& _tmp9AB,& _tmp9AA))),_tmp9AA)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9B1;unsigned int _tmp9B0;struct Cyc_Yyltype*_tmp9AF;unsigned int _tmp9AE;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9AE=(unsigned int)200,((_tmp9AF=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9AE)),((((_tmp9B0=_tmp9AE,_tmp9B2(& _tmp9B1,& _tmp9B0,& _tmp9AF))),_tmp9AF)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
yychar=-2;
# 193
yyssp_offset=-1;
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
{const char*_tmp9B3;Cyc_yyerror(((_tmp9B3="parser stack overflow",_tag_dyneither(_tmp9B3,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9B8;unsigned int _tmp9B7;struct _dyneither_ptr _tmp9B6;short*_tmp9B5;unsigned int _tmp9B4;struct _dyneither_ptr yyss1=(_tmp9B4=(unsigned int)yystacksize,((_tmp9B5=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9B4)),((_tmp9B6=_tag_dyneither(_tmp9B5,sizeof(short),_tmp9B4),((((_tmp9B7=_tmp9B4,_tmp9B9(& yyssp_offset,& yyss,& _tmp9B8,& _tmp9B7,& _tmp9B5))),_tmp9B6)))))));
# 217
unsigned int _tmp9BE;unsigned int _tmp9BD;struct _dyneither_ptr _tmp9BC;union Cyc_YYSTYPE*_tmp9BB;unsigned int _tmp9BA;struct _dyneither_ptr yyvs1=
(_tmp9BA=(unsigned int)yystacksize,((_tmp9BB=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9BA)),((_tmp9BC=_tag_dyneither(_tmp9BB,sizeof(union Cyc_YYSTYPE),_tmp9BA),((((_tmp9BD=_tmp9BA,_tmp9BF(& yyvs,& yyssp_offset,& yylval,& _tmp9BE,& _tmp9BD,& _tmp9BB))),_tmp9BC)))))));
# 221
unsigned int _tmp9C4;unsigned int _tmp9C3;struct _dyneither_ptr _tmp9C2;struct Cyc_Yyltype*_tmp9C1;unsigned int _tmp9C0;struct _dyneither_ptr yyls1=(_tmp9C0=(unsigned int)yystacksize,((_tmp9C1=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9C0)),((_tmp9C2=_tag_dyneither(_tmp9C1,sizeof(struct Cyc_Yyltype),_tmp9C0),((((_tmp9C3=_tmp9C0,_tmp9C5(& yyssp_offset,& yyls,& _tmp9C4,& _tmp9C3,& _tmp9C1))),_tmp9C2)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1041,yystate)];
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
yychar1=yychar > 0  && yychar <= 374?(int)Cyc_yytranslate[_check_known_subscript_notnull(375,yychar)]: 292;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6875) || Cyc_yycheck[_check_known_subscript_notnull(6876,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6876,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1040){
int _tmp437=0;_npop_handler(0);return _tmp437;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1041,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(523,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL20F: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1168 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY19(yyyyvsp[0]);
# 1171
break;}case 2: _LL210: {
# 1173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1174 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp438;_push_handler(& _tmp438);{int _tmp43A=0;if(setjmp(_tmp438.handler))_tmp43A=1;if(!_tmp43A){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp439=(void*)_exn_thrown;void*_tmp43C=_tmp439;void*_tmp43E;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp43D=(struct Cyc_Core_Failure_exn_struct*)_tmp43C;if(_tmp43D->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1179
 x=0;goto _LL212;_LL215: _tmp43E=_tmp43C;_LL216:(void)_throw(_tmp43E);_LL212:;}};}
# 1181
{struct _handler_cons _tmp43F;_push_handler(& _tmp43F);{int _tmp441=0;if(setjmp(_tmp43F.handler))_tmp441=1;if(!_tmp441){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp440=(void*)_exn_thrown;void*_tmp443=_tmp440;void*_tmp445;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp444=(struct Cyc_Core_Failure_exn_struct*)_tmp443;if(_tmp444->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1185
 y=0;goto _LL217;_LL21A: _tmp445=_tmp443;_LL21B:(void)_throw(_tmp445);_LL217:;}};}
# 1187
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1189
break;}case 3: _LL211: {
# 1191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1192 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9CF;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9CE;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9CD;struct Cyc_List_List*_tmp9CC;yyval=Cyc_YY19(((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC->hd=((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF->r=(void*)((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CE.tag=11,((_tmp9CE.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9CE.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9CE)))))),_tmp9CD)))),((_tmp9CF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9CF)))))),((_tmp9CC->tl=0,_tmp9CC)))))));}
Cyc_Lex_leave_using();
# 1195
break;}case 4: _LL21C: {
# 1197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1196 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9D9;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9D8;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9D7;struct Cyc_List_List*_tmp9D6;yyval=Cyc_YY19(((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6->hd=((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->r=(void*)((_tmp9D7=_cycalloc(sizeof(*_tmp9D7)),((_tmp9D7[0]=((_tmp9D8.tag=11,((_tmp9D8.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9D8.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D8)))))),_tmp9D7)))),((_tmp9D9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D9)))))),((_tmp9D6->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9D6)))))));}
break;}case 5: _LL21D: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1199 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9E8;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9E7;struct _dyneither_ptr*_tmp9E6;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E5;struct Cyc_List_List*_tmp9E4;yyval=Cyc_YY19(((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4->hd=((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8->r=(void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E7.tag=10,((_tmp9E7.f1=((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9E6)))),((_tmp9E7.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9E7)))))),_tmp9E5)))),((_tmp9E8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E8)))))),((_tmp9E4->tl=0,_tmp9E4)))))));}
Cyc_Lex_leave_namespace();
# 1202
break;}case 6: _LL21E: {
# 1204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1204 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp453;_push_handler(& _tmp453);{int _tmp455=0;if(setjmp(_tmp453.handler))_tmp455=1;if(!_tmp455){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp454=(void*)_exn_thrown;void*_tmp457=_tmp454;void*_tmp459;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp458=(struct Cyc_Core_Failure_exn_struct*)_tmp457;if(_tmp458->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmp9E9;nspace=((_tmp9E9="",_tag_dyneither(_tmp9E9,sizeof(char),1)));}goto _LL220;_LL223: _tmp459=_tmp457;_LL224:(void)_throw(_tmp459);_LL220:;}};}
# 1209
{struct _handler_cons _tmp45B;_push_handler(& _tmp45B);{int _tmp45D=0;if(setjmp(_tmp45B.handler))_tmp45D=1;if(!_tmp45D){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp45C=(void*)_exn_thrown;void*_tmp45F=_tmp45C;void*_tmp461;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp460=(struct Cyc_Core_Failure_exn_struct*)_tmp45F;if(_tmp460->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp461=_tmp45F;_LL229:(void)_throw(_tmp461);_LL225:;}};}
# 1212
{struct _handler_cons _tmp462;_push_handler(& _tmp462);{int _tmp464=0;if(setjmp(_tmp462.handler))_tmp464=1;if(!_tmp464){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp463=(void*)_exn_thrown;void*_tmp466=_tmp463;void*_tmp468;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp467=(struct Cyc_Core_Failure_exn_struct*)_tmp466;if(_tmp467->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp468=_tmp466;_LL22E:(void)_throw(_tmp468);_LL22A:;}};}
# 1215
{struct Cyc_Absyn_Decl*_tmp9F8;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9F7;struct _dyneither_ptr*_tmp9F6;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F5;struct Cyc_List_List*_tmp9F4;yyval=Cyc_YY19(((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4->hd=((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->r=(void*)((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F7.tag=10,((_tmp9F7.f1=((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=nspace,_tmp9F6)))),((_tmp9F7.f2=x,_tmp9F7)))))),_tmp9F5)))),((_tmp9F8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F8)))))),((_tmp9F4->tl=y,_tmp9F4)))))));}
# 1217
break;}case 7: _LL21F: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1218 "parse.y"
struct _dyneither_ptr _tmp46E=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9F9;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46E,((_tmp9F9="C",_tag_dyneither(_tmp9F9,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA03;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA02;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA01;struct Cyc_List_List*_tmpA00;yyval=Cyc_YY19(((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->hd=((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->r=(void*)((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=12,((_tmpA02.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA02)))),_tmpA01)))),((_tmpA03->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA03)))))),((_tmpA00->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA00)))))));}else{
# 1222
{const char*_tmpA04;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46E,((_tmpA04="C include",_tag_dyneither(_tmpA04,sizeof(char),10))))!= 0){
const char*_tmpA05;Cyc_Parse_err(((_tmpA05="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA05,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1225
struct Cyc_Absyn_Decl*_tmpA0F;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA0E;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA0D;struct Cyc_List_List*_tmpA0C;yyval=Cyc_YY19(((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->hd=((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F->r=(void*)((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0E.tag=13,((_tmpA0E.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA0E.f2=0,_tmpA0E)))))),_tmpA0D)))),((_tmpA0F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0F)))))),((_tmpA0C->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA0C)))))));};}}
# 1228
break;}case 8: _LL22F: {
# 1230
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1232
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1229 "parse.y"
{const char*_tmpA10;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA10="C include",_tag_dyneither(_tmpA10,sizeof(char),10))))!= 0){
const char*_tmpA11;Cyc_Parse_err(((_tmpA11="expecting \"C include\"",_tag_dyneither(_tmpA11,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA1B;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA1A;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA19;struct Cyc_List_List*_tmpA18;yyval=Cyc_YY19(((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->r=(void*)((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1A.tag=13,((_tmpA1A.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA1A.f2=exs,_tmpA1A)))))),_tmpA19)))),((_tmpA1B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1B)))))),((_tmpA18->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA18)))))));}
# 1234
break;};}case 9: _LL230: {
# 1236
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1238
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1235 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1E;struct Cyc_List_List*_tmpA1D;yyval=Cyc_YY19(((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->hd=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA1E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1E)))))),((_tmpA1D->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1D)))))));}
break;}case 10: _LL231: {
# 1238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1237 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA21;struct Cyc_List_List*_tmpA20;yyval=Cyc_YY19(((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->hd=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA21->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA21)))))),((_tmpA20->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA20)))))));}
break;}case 11: _LL232:
# 1240
 yyval=Cyc_YY19(0);
break;case 12: _LL233: {
# 1243
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1242 "parse.y"
yyval=yyyyvsp[2];
break;}case 13: _LL234: {
# 1245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243 "parse.y"
yyval=Cyc_YY55(0);
break;}case 14: _LL235: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1247 "parse.y"
{struct _tuple27*_tmpA24;struct Cyc_List_List*_tmpA23;yyval=Cyc_YY55(((_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23->hd=((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA24->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA24->f3=0,_tmpA24)))))))),((_tmpA23->tl=0,_tmpA23)))))));}
break;}case 15: _LL236: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1248 "parse.y"
{struct _tuple27*_tmpA27;struct Cyc_List_List*_tmpA26;yyval=Cyc_YY55(((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->hd=((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA27->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA27->f3=0,_tmpA27)))))))),((_tmpA26->tl=0,_tmpA26)))))));}
break;}case 16: _LL237: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct _tuple27*_tmpA2A;struct Cyc_List_List*_tmpA29;yyval=Cyc_YY55(((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->hd=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA2A->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA2A->f3=0,_tmpA2A)))))))),((_tmpA29->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA29)))))));}
break;}case 17: _LL238: {
# 1253
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1254 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA30;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA2F;struct Cyc_List_List*_tmpA2E;yyval=Cyc_YY19(((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->hd=Cyc_Absyn_new_decl((void*)((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=((_tmpA2F.tag=1,((_tmpA2F.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA2F)))),_tmpA30)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA2E->tl=0,_tmpA2E)))))));}
break;}case 18: _LL239: {
# 1257
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1255 "parse.y"
yyval=yyyyvsp[0];
break;}case 19: _LL23A: {
# 1258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1256 "parse.y"
yyval=Cyc_YY19(0);
break;}case 22: _LL23B: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1265 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL23C: {
# 1268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1267 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48D=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48D,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23D: {
# 1271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1280 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL23E: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1282 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48E=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48E,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23F: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1290 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48F=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48F,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL240: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1293 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp490=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp490,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL241: {
# 1297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1298 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 29: _LL242: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1301 "parse.y"
Cyc_Lex_leave_using();
break;}case 30: _LL243: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1304 "parse.y"
{struct _dyneither_ptr*_tmpA31;Cyc_Lex_enter_namespace(((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA31)))));}yyval=yyyyvsp[1];
break;}case 31: _LL244: {
# 1307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1307 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 32: _LL245: {
# 1310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp492=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp492,(unsigned int)_tmp492));
break;}case 33: _LL246: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
int _tmp493=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp493));
break;}case 34: _LL247: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1320 "parse.y"
{struct Cyc_List_List*_tmpA32;yyval=Cyc_YY19(((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA32->tl=0,_tmpA32)))))));}
break;}case 35: _LL248: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1322 "parse.y"
struct Cyc_List_List*_tmp495=0;
{struct Cyc_List_List*_tmp496=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp496 != 0;_tmp496=_tmp496->tl){
struct _dyneither_ptr*_tmp497=(struct _dyneither_ptr*)_tmp496->hd;
struct _tuple0*_tmpA33;struct _tuple0*qv=(_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->f1=Cyc_Absyn_Rel_n(0),((_tmpA33->f2=_tmp497,_tmpA33)))));
struct Cyc_Absyn_Vardecl*_tmp498=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA34;_tmp495=((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=_tmp498,((_tmpA34->tl=_tmp495,_tmpA34))))));}}
# 1329
_tmp495=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp495);
{struct Cyc_List_List*_tmpA35;yyval=Cyc_YY19(((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->hd=Cyc_Absyn_letv_decl(_tmp495,(unsigned int)(yyyylsp[0]).first_line),((_tmpA35->tl=0,_tmpA35)))))));}
# 1332
break;}case 36: _LL249: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1335 "parse.y"
struct _dyneither_ptr _tmp49C=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA36;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49C,((_tmpA36="`H",_tag_dyneither(_tmpA36,sizeof(char),3))))== 0){
const char*_tmpA39;void*_tmpA38;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA38=0,Cyc_aprintf(((_tmpA39="bad occurrence of heap region",_tag_dyneither(_tmpA39,sizeof(char),30))),_tag_dyneither(_tmpA38,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA3A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49C,((_tmpA3A="`U",_tag_dyneither(_tmpA3A,sizeof(char),3))))== 0){
const char*_tmpA3D;void*_tmpA3C;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA3C=0,Cyc_aprintf(((_tmpA3D="bad occurrence of unique region",_tag_dyneither(_tmpA3D,sizeof(char),32))),_tag_dyneither(_tmpA3C,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA40;struct Cyc_Absyn_Tvar*_tmpA3F;struct Cyc_Absyn_Tvar*tv=(_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F->name=((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=_tmp49C,_tmpA40)))),((_tmpA3F->identity=- 1,((_tmpA3F->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA3F)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA43;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA42;void*t=(void*)((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=2,((_tmpA43.f1=tv,_tmpA43)))),_tmpA42))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA49;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA48;struct _dyneither_ptr*_tmpA46;struct _tuple0*_tmpA45;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->f1=Cyc_Absyn_Loc_n,((_tmpA45->f2=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA46)))),_tmpA45)))))),(void*)((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=15,((_tmpA49.f1=t,_tmpA49)))),_tmpA48)))),0);
{struct Cyc_List_List*_tmpA4A;yyval=Cyc_YY19(((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA4A->tl=0,_tmpA4A)))))));}
# 1345
break;};}case 37: _LL24A: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1347 "parse.y"
int _tmp4AC=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4AD=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA4B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AD,((_tmpA4B="H",_tag_dyneither(_tmpA4B,sizeof(char),2))))== 0){
const char*_tmpA4C;Cyc_Parse_err(((_tmpA4C="bad occurrence of heap region `H",_tag_dyneither(_tmpA4C,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA4D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AD,((_tmpA4D="U",_tag_dyneither(_tmpA4D,sizeof(char),2))))== 0){
const char*_tmpA4E;Cyc_Parse_err(((_tmpA4E="bad occurrence of unique region `U",_tag_dyneither(_tmpA4E,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4AC  && _tmp4AE != 0){
const char*_tmpA4F;Cyc_Parse_err(((_tmpA4F="open regions cannot be @resetable",_tag_dyneither(_tmpA4F,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA5C;const char*_tmpA5B;void*_tmpA5A[1];struct Cyc_String_pa_PrintArg_struct _tmpA59;struct Cyc_Absyn_Tvar*_tmpA58;struct Cyc_Absyn_Tvar*tv=(_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->name=((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=(struct _dyneither_ptr)((_tmpA59.tag=0,((_tmpA59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4AD),((_tmpA5A[0]=& _tmpA59,Cyc_aprintf(((_tmpA5B="`%s",_tag_dyneither(_tmpA5B,sizeof(char),4))),_tag_dyneither(_tmpA5A,sizeof(void*),1)))))))),_tmpA5C)))),((_tmpA58->identity=- 1,((_tmpA58->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA58)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA5F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA5E;void*t=(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=2,((_tmpA5F.f1=tv,_tmpA5F)))),_tmpA5E))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA65;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA64;struct _dyneither_ptr*_tmpA62;struct _tuple0*_tmpA61;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->f1=Cyc_Absyn_Loc_n,((_tmpA61->f2=((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=_tmp4AD,_tmpA62)))),_tmpA61)))))),(void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=15,((_tmpA65.f1=t,_tmpA65)))),_tmpA64)))),0);
# 1361
{struct Cyc_List_List*_tmpA66;yyval=Cyc_YY19(((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4AC,_tmp4AE,(unsigned int)(yyyylsp[0]).first_line),((_tmpA66->tl=0,_tmpA66)))))));}
# 1363
break;};}case 38: _LL24B: {
# 1365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1365 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA6D;struct _dyneither_ptr*_tmpA6C;struct Cyc_Absyn_Tvar*_tmpA6B;struct Cyc_Absyn_Tvar*tv=(_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->name=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA6C)))),((_tmpA6B->identity=- 1,((_tmpA6B->kind=(void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6D.tag=0,((_tmpA6D.f1=& Cyc_Tcutil_rk,_tmpA6D)))),_tmpA6E)))),_tmpA6B)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA71;void*_tmpA70;(_tmpA70=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA71="too many declarations in alias",_tag_dyneither(_tmpA71,sizeof(char),31))),_tag_dyneither(_tmpA70,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_stmttmp35=d->r;void*_tmp4C2=_stmttmp35;struct Cyc_Absyn_Vardecl*_tmp4C4;_LL24E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C2;if(_tmp4C3->tag != 0)goto _LL250;else{_tmp4C4=_tmp4C3->f1;}}_LL24F:
# 1372
{struct Cyc_List_List*_tmpA72;yyval=Cyc_YY19(((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=Cyc_Absyn_alias_decl(tv,_tmp4C4,(unsigned int)(yyyylsp[0]).first_line),((_tmpA72->tl=0,_tmpA72)))))));}
goto _LL24D;_LL250:;_LL251:
# 1375
{const char*_tmpA73;Cyc_Parse_err(((_tmpA73="expecting variable declaration",_tag_dyneither(_tmpA73,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY19(0);_LL24D:;}
# 1379
break;};}case 39: _LL24C:
# 1381
 yyval=Cyc_YY34(0);
break;case 40: _LL252: {
# 1384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1383 "parse.y"
yyval=Cyc_YY34(1);
break;}case 41: _LL253: {
# 1386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1388 "parse.y"
yyval=yyyyvsp[0];
break;}case 42: _LL254: {
# 1391
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1390 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 43: _LL255: {
# 1393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpA74;yyval=Cyc_YY20(((_tmpA74.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA74.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA74.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA74.is_inline=0,((_tmpA74.attributes=0,_tmpA74)))))))))));}
break;}case 44: _LL256: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1399 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CC=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4CC.sc != 0){
const char*_tmpA77;void*_tmpA76;(_tmpA76=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA77="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA77,sizeof(char),51))),_tag_dyneither(_tmpA76,sizeof(void*),0)));}
# 1403
{struct Cyc_Parse_Declaration_spec _tmpA78;yyval=Cyc_YY20(((_tmpA78.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA78.tq=_tmp4CC.tq,((_tmpA78.type_specs=_tmp4CC.type_specs,((_tmpA78.is_inline=_tmp4CC.is_inline,((_tmpA78.attributes=_tmp4CC.attributes,_tmpA78)))))))))));}
# 1407
break;}case 45: _LL257: {
# 1409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1408 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA79;yyval=Cyc_YY20(((_tmpA79.sc=0,((_tmpA79.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA79.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA79.is_inline=0,((_tmpA79.attributes=0,_tmpA79)))))))))));}
break;}case 46: _LL258: {
# 1412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1411 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D1=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7A;yyval=Cyc_YY20(((_tmpA7A.sc=_tmp4D1.sc,((_tmpA7A.tq=_tmp4D1.tq,((_tmpA7A.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4D1.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA7A.is_inline=_tmp4D1.is_inline,((_tmpA7A.attributes=_tmp4D1.attributes,_tmpA7A)))))))))));}
# 1417
break;}case 47: _LL259: {
# 1419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1418 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7B;yyval=Cyc_YY20(((_tmpA7B.sc=0,((_tmpA7B.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA7B.type_specs=Cyc_Parse_empty_spec(0),((_tmpA7B.is_inline=0,((_tmpA7B.attributes=0,_tmpA7B)))))))))));}
break;}case 48: _LL25A: {
# 1421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1420 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D4=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7C;yyval=Cyc_YY20(((_tmpA7C.sc=_tmp4D4.sc,((_tmpA7C.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4D4.tq),((_tmpA7C.type_specs=_tmp4D4.type_specs,((_tmpA7C.is_inline=_tmp4D4.is_inline,((_tmpA7C.attributes=_tmp4D4.attributes,_tmpA7C)))))))))));}
# 1425
break;}case 49: _LL25B: {
# 1427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1426 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7D;yyval=Cyc_YY20(((_tmpA7D.sc=0,((_tmpA7D.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7D.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7D.is_inline=1,((_tmpA7D.attributes=0,_tmpA7D)))))))))));}
break;}case 50: _LL25C: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1429 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D7=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7E;yyval=Cyc_YY20(((_tmpA7E.sc=_tmp4D7.sc,((_tmpA7E.tq=_tmp4D7.tq,((_tmpA7E.type_specs=_tmp4D7.type_specs,((_tmpA7E.is_inline=1,((_tmpA7E.attributes=_tmp4D7.attributes,_tmpA7E)))))))))));}
# 1433
break;}case 51: _LL25D: {
# 1435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1434 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7F;yyval=Cyc_YY20(((_tmpA7F.sc=0,((_tmpA7F.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7F.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7F.is_inline=0,((_tmpA7F.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpA7F)))))))))));}
break;}case 52: _LL25E: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1437 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4DA=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA80;yyval=Cyc_YY20(((_tmpA80.sc=_tmp4DA.sc,((_tmpA80.tq=_tmp4DA.tq,((_tmpA80.type_specs=_tmp4DA.type_specs,((_tmpA80.is_inline=_tmp4DA.is_inline,((_tmpA80.attributes=
# 1440
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4DA.attributes),_tmpA80)))))))))));}
break;}case 53: _LL25F: {
# 1443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1444 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 54: _LL260: {
# 1448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1446 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 55: _LL261: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1448 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 56: _LL262: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1450 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 57: _LL263: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1453 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA81;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA81="C",_tag_dyneither(_tmpA81,sizeof(char),2))))!= 0){
const char*_tmpA82;Cyc_Parse_err(((_tmpA82="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA82,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1458
break;}case 58: _LL264: {
# 1460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1458 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 59: _LL265: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1461 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 60: _LL266:
# 1465
 yyval=Cyc_YY48(0);
break;case 61: _LL267: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1468 "parse.y"
yyval=yyyyvsp[0];
break;}case 62: _LL268: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1473 "parse.y"
yyval=yyyyvsp[3];
break;}case 63: _LL269: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1477 "parse.y"
{struct Cyc_List_List*_tmpA83;yyval=Cyc_YY48(((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA83->tl=0,_tmpA83)))))));}
break;}case 64: _LL26A: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1478 "parse.y"
{struct Cyc_List_List*_tmpA84;yyval=Cyc_YY48(((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA84->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpA84)))))));}
break;}case 65: _LL26B: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1483 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4E2,_tmp4E2,_tmp4E2 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 8},(void*)& att_aligned},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1503
struct _dyneither_ptr _tmp4E0=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1505
if((((_get_dyneither_size(_tmp4E0,sizeof(char))> 4  && ((const char*)_tmp4E0.curr)[0]== '_') && ((const char*)_tmp4E0.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4E0,sizeof(char),(int)(_get_dyneither_size(_tmp4E0,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4E0,sizeof(char),(int)(_get_dyneither_size(_tmp4E0,sizeof(char))- 3)))== '_')
# 1507
_tmp4E0=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4E0,2,_get_dyneither_size(_tmp4E0,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4E0,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1514
if(i == 17){
{const char*_tmpA85;Cyc_Parse_err(((_tmpA85="unrecognized attribute",_tag_dyneither(_tmpA85,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1519
break;};}case 66: _LL26C: {
# 1521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1519 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 67: _LL26D: {
# 1522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1521 "parse.y"
struct _dyneither_ptr _tmp4F4=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _stmttmp34=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp4F6;struct _tuple5 _tmp4F5=_stmttmp34;_tmp4F6=_tmp4F5.f2;{
void*a;
{const char*_tmpA87;const char*_tmpA86;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA87="regparm",_tag_dyneither(_tmpA87,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA86="__regparm__",_tag_dyneither(_tmpA86,sizeof(char),12))))== 0){
if(_tmp4F6 < 0  || _tmp4F6 > 3){
const char*_tmpA88;Cyc_Parse_err(((_tmpA88="regparm requires value between 0 and 3",_tag_dyneither(_tmpA88,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA8B;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA8A;a=(void*)((_tmpA8A=_cycalloc_atomic(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=0,((_tmpA8B.f1=_tmp4F6,_tmpA8B)))),_tmpA8A))));};}else{
const char*_tmpA8D;const char*_tmpA8C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA8D="aligned",_tag_dyneither(_tmpA8D,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA8C="__aligned__",_tag_dyneither(_tmpA8C,sizeof(char),12))))== 0){
if(_tmp4F6 < 0){const char*_tmpA8E;Cyc_Parse_err(((_tmpA8E="aligned requires positive power of two",_tag_dyneither(_tmpA8E,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4F6;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA8F;Cyc_Parse_err(((_tmpA8F="aligned requires positive power of two",_tag_dyneither(_tmpA8F,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA92;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA91;a=(void*)((_tmpA91=_cycalloc_atomic(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=6,((_tmpA92.f1=_tmp4F6,_tmpA92)))),_tmpA91))));};};}else{
const char*_tmpA94;const char*_tmpA93;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA94="initializes",_tag_dyneither(_tmpA94,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA93="__initializes__",_tag_dyneither(_tmpA93,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA97;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA96;a=(void*)((_tmpA96=_cycalloc_atomic(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=20,((_tmpA97.f1=_tmp4F6,_tmpA97)))),_tmpA96))));}else{
const char*_tmpA99;const char*_tmpA98;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA99="noliveunique",_tag_dyneither(_tmpA99,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA98="__noliveunique__",_tag_dyneither(_tmpA98,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA9C;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA9B;a=(void*)((_tmpA9B=_cycalloc_atomic(sizeof(*_tmpA9B)),((_tmpA9B[0]=((_tmpA9C.tag=21,((_tmpA9C.f1=_tmp4F6,_tmpA9C)))),_tmpA9B))));}else{
const char*_tmpA9E;const char*_tmpA9D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA9E="noconsume",_tag_dyneither(_tmpA9E,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F4,((_tmpA9D="__noconsume__",_tag_dyneither(_tmpA9D,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAA1;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAA0;a=(void*)((_tmpAA0=_cycalloc_atomic(sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA1.tag=22,((_tmpAA1.f1=_tmp4F6,_tmpAA1)))),_tmpAA0))));}else{
# 1543
{const char*_tmpAA2;Cyc_Parse_err(((_tmpAA2="unrecognized attribute",_tag_dyneither(_tmpAA2,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1546
yyval=Cyc_YY49(a);
# 1548
break;};}case 68: _LL26E: {
# 1550
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1549 "parse.y"
struct _dyneither_ptr _tmp50F=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp510=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAA4;const char*_tmpAA3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAA4="section",_tag_dyneither(_tmpAA4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAA3="__section__",_tag_dyneither(_tmpAA3,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAA7;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAA6;a=(void*)((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA7.tag=8,((_tmpAA7.f1=_tmp510,_tmpAA7)))),_tmpAA6))));}else{
# 1555
{const char*_tmpAA8;Cyc_Parse_err(((_tmpAA8="unrecognized attribute",_tag_dyneither(_tmpAA8,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1558
yyval=Cyc_YY49(a);
# 1560
break;}case 69: _LL26F: {
# 1562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1561 "parse.y"
struct _dyneither_ptr _tmp516=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1565
{const char*_tmpAA9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp516,((_tmpAA9="__mode__",_tag_dyneither(_tmpAA9,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAAC;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAAB;a=(void*)((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAC.tag=24,((_tmpAAC.f1=_tmp517,_tmpAAC)))),_tmpAAB))));}else{
# 1568
{const char*_tmpAAD;Cyc_Parse_err(((_tmpAAD="unrecognized attribute",_tag_dyneither(_tmpAAD,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1571
yyval=Cyc_YY49(a);
# 1573
break;}case 70: _LL270: {
# 1575
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1574 "parse.y"
struct _dyneither_ptr _tmp51C=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51D=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _stmttmp32=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp51F;struct _tuple5 _tmp51E=_stmttmp32;_tmp51F=_tmp51E.f2;{
struct _tuple5 _stmttmp33=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp521;struct _tuple5 _tmp520=_stmttmp33;_tmp521=_tmp520.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAAF;const char*_tmpAAE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51C,((_tmpAAF="format",_tag_dyneither(_tmpAAF,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51C,((_tmpAAE="__format__",_tag_dyneither(_tmpAAE,sizeof(char),11))))== 0){
const char*_tmpAB0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51D,((_tmpAB0="printf",_tag_dyneither(_tmpAB0,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB3;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB2;a=(void*)((_tmpAB2=_cycalloc_atomic(sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB3.tag=19,((_tmpAB3.f1=Cyc_Absyn_Printf_ft,((_tmpAB3.f2=_tmp51F,((_tmpAB3.f3=_tmp521,_tmpAB3)))))))),_tmpAB2))));}else{
const char*_tmpAB4;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51D,((_tmpAB4="scanf",_tag_dyneither(_tmpAB4,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB7;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB6;a=(void*)((_tmpAB6=_cycalloc_atomic(sizeof(*_tmpAB6)),((_tmpAB6[0]=((_tmpAB7.tag=19,((_tmpAB7.f1=Cyc_Absyn_Scanf_ft,((_tmpAB7.f2=_tmp51F,((_tmpAB7.f3=_tmp521,_tmpAB7)))))))),_tmpAB6))));}else{
# 1585
const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="unrecognized format type",_tag_dyneither(_tmpAB8,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1587
const char*_tmpAB9;Cyc_Parse_err(((_tmpAB9="unrecognized attribute",_tag_dyneither(_tmpAB9,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1590
break;};};}case 71: _LL271: {
# 1592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1599 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL272: {
# 1602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1601 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpABC;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpABB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=((_tmpABC.tag=17,((_tmpABC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpABC.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpABC.f3=0,((_tmpABC.f4=0,_tmpABC)))))))))),_tmpABB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL273: {
# 1604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL274: {
# 1608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL275: {
# 1609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1608 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL276: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL277: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL278: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL279: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL27A: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL27B: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL27C: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL27D: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 84: _LL27E: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27F: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL280: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1620 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL281: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL282: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1625 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpABF;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpABE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABF.tag=10,((_tmpABF.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpABF)))),_tmpABE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1628
break;}case 89: _LL283: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1629 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC2;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1[0]=((_tmpAC2.tag=15,((_tmpAC2.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAC2)))),_tmpAC1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL284: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1631 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4[0]=((_tmpAC5.tag=15,((_tmpAC5.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAC5)))),_tmpAC4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1633
break;}case 91: _LL285: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1634 "parse.y"
void*_tmp534=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAC8;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAC7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=16,((_tmpAC8.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAC8.f2=_tmp534,_tmpAC8)))))),_tmpAC7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1637
break;}case 92: _LL286: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1638 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpACB;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpACA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=((_tmpACB.tag=16,((_tmpACB.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpACB.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpACB)))))),_tmpACA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL287: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1640 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpACE;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpACD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=((_tmpACE.tag=19,((_tmpACE.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpACE)))),_tmpACD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL288: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1642 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAD1;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD0;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD1.tag=19,((_tmpAD1.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAD1)))),_tmpAD0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL289: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1644 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAD4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD3;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD4.tag=18,((_tmpAD4.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAD4)))),_tmpAD3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL28A: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 97: _LL28B: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAD5;yyval=Cyc_YY26(((_tmpAD5.print_const=1,((_tmpAD5.q_volatile=0,((_tmpAD5.q_restrict=0,((_tmpAD5.real_const=1,((_tmpAD5.loc=loc,_tmpAD5)))))))))));}
break;}case 98: _LL28C: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD6;yyval=Cyc_YY26(((_tmpAD6.print_const=0,((_tmpAD6.q_volatile=1,((_tmpAD6.q_restrict=0,((_tmpAD6.real_const=0,((_tmpAD6.loc=0,_tmpAD6)))))))))));}
break;}case 99: _LL28D: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD7;yyval=Cyc_YY26(((_tmpAD7.print_const=0,((_tmpAD7.q_volatile=0,((_tmpAD7.q_restrict=1,((_tmpAD7.real_const=0,((_tmpAD7.loc=0,_tmpAD7)))))))))));}
break;}case 100: _LL28E: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAE6;struct Cyc_Absyn_Enumdecl*_tmpAE5;struct Cyc_Core_Opt*_tmpAE4;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAE3;struct Cyc_Absyn_TypeDecl*_tmpAE2;struct Cyc_Absyn_TypeDecl*_tmp542=(_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->r=(void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE3.tag=1,((_tmpAE3.f1=((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->sc=Cyc_Absyn_Public,((_tmpAE5->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAE5->fields=((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpAE4)))),_tmpAE5)))))))),_tmpAE3)))),_tmpAE6)))),((_tmpAE2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAE2)))));
# 1664
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE9;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=26,((_tmpAE9.f1=_tmp542,((_tmpAE9.f2=0,_tmpAE9)))))),_tmpAE8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1666
break;}case 101: _LL28F: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1667 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAEC;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAEB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=13,((_tmpAEC.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAEC.f2=0,_tmpAEC)))))),_tmpAEB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL290: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1669 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAEF;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAEE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=14,((_tmpAEF.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAEF)))),_tmpAEE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL291: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpAF0;yyval=Cyc_YY50(((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF0->tag=0,((_tmpAF0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAF0)))))))));}
break;}case 104: _LL292: {
# 1678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1677 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpAF1;yyval=Cyc_YY50(((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF1->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpAF1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAF1)))))))));}
break;}case 105: _LL293: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1681 "parse.y"
{struct Cyc_List_List*_tmpAF2;yyval=Cyc_YY51(((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF2->tl=0,_tmpAF2)))))));}
break;}case 106: _LL294: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1682 "parse.y"
{struct Cyc_List_List*_tmpAF3;yyval=Cyc_YY51(((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF3->tl=0,_tmpAF3)))))));}
break;}case 107: _LL295: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1683 "parse.y"
{struct Cyc_List_List*_tmpAF4;yyval=Cyc_YY51(((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF4->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAF4)))))));}
break;}case 108: _LL296: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAF7;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAF6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=12,((_tmpAF7.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAF7.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAF7)))))),_tmpAF6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL297: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1695 "parse.y"
struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp556=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp557=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp555,
Cyc_Absyn_aggrdecl_impl(_tmp556,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1700
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAFA;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAF9;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=26,((_tmpAFA.f1=_tmp557,((_tmpAFA.f2=0,_tmpAFA)))))),_tmpAF9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1702
break;}case 110: _LL298: {
# 1704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1706 "parse.y"
struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp55C=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp55A,
Cyc_Absyn_aggrdecl_impl(_tmp55B,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1711
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAFD;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAFC;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC[0]=((_tmpAFD.tag=26,((_tmpAFD.f1=_tmp55C,((_tmpAFD.f2=0,_tmpAFD)))))),_tmpAFC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1713
break;}case 111: _LL299: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1714 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB07;struct Cyc_Core_Opt*_tmpB06;struct Cyc_Absyn_AggrInfo _tmpB05;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB04;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB07.tag=11,((_tmpB07.f1=((_tmpB05.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB06=_cycalloc_atomic(sizeof(*_tmpB06)),((_tmpB06->v=(void*)1,_tmpB06))))),((_tmpB05.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB05)))),_tmpB07)))),_tmpB04)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1717
break;}case 112: _LL29A: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1718 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB0D;struct Cyc_Absyn_AggrInfo _tmpB0C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB0B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0D.tag=11,((_tmpB0D.f1=((_tmpB0C.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB0C.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB0C)))),_tmpB0D)))),_tmpB0B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1721
break;}case 113: _LL29B:
# 1723
 yyval=Cyc_YY43(0);
break;case 114: _LL29C: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1727 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 115: _LL29D: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1731 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 116: _LL29E: {
# 1734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1732 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 117: _LL29F:
# 1735
 yyval=Cyc_YY27(0);
break;case 118: _LL2A0: {
# 1738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1739 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp566=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp566 != 0;_tmp566=_tmp566->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp566->hd,decls);}}{
# 1743
struct Cyc_List_List*_tmp567=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp567 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp567,decls);
yyval=Cyc_YY27(decls);
# 1748
break;};}case 119: _LL2A1: {
# 1750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB0E;yyval=Cyc_YY28(((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB0E->tl=0,_tmpB0E)))))));}
break;}case 120: _LL2A2: {
# 1756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1755 "parse.y"
{struct Cyc_List_List*_tmpB0F;yyval=Cyc_YY28(((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB0F->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB0F)))))));}
break;}case 121: _LL2A3: {
# 1758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1759 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 122: _LL2A4: {
# 1762
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB10;yyval=Cyc_YY22(((_tmpB10=_region_malloc(yyr,sizeof(*_tmpB10)),((_tmpB10->tl=0,((_tmpB10->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB10)))))));}
break;}case 123: _LL2A5: {
# 1768
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1767 "parse.y"
{struct _tuple12*_tmpB11;yyval=Cyc_YY22(((_tmpB11=_region_malloc(yyr,sizeof(*_tmpB11)),((_tmpB11->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB11->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB11)))))));}
break;}case 124: _LL2A6: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1772 "parse.y"
{struct _tuple11 _tmpB12;yyval=Cyc_YY21(((_tmpB12.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB12.f2=0,_tmpB12)))));}
break;}case 125: _LL2A7: {
# 1775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1774 "parse.y"
{struct _tuple11 _tmpB13;yyval=Cyc_YY21(((_tmpB13.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB13.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB13)))));}
break;}case 126: _LL2A8: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1780 "parse.y"
struct _RegionHandle _tmp56E=_new_region("temp");struct _RegionHandle*temp=& _tmp56E;_push_region(temp);{
struct _tuple25 _stmttmp30=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp570;struct Cyc_Parse_Type_specifier _tmp571;struct Cyc_List_List*_tmp572;struct _tuple25 _tmp56F=_stmttmp30;_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;_tmp572=_tmp56F.f3;
if(_tmp570.loc == 0)_tmp570.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp573=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp573 != 0;_tmp573=_tmp573->tl){
struct _tuple24*_stmttmp31=(struct _tuple24*)_tmp573->hd;struct Cyc_Parse_Declarator _tmp575;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp577;struct _tuple24*_tmp574=_stmttmp31;_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;_tmp577=_tmp574->f3;
{struct _tuple10*_tmpB14;decls=((_tmpB14=_region_malloc(temp,sizeof(*_tmpB14)),((_tmpB14->tl=decls,((_tmpB14->hd=_tmp575,_tmpB14))))));}{
struct _tuple16*_tmpB17;struct Cyc_List_List*_tmpB16;widths_and_reqs=(
(_tmpB16=_region_malloc(temp,sizeof(*_tmpB16)),((_tmpB16->hd=((_tmpB17=_region_malloc(temp,sizeof(*_tmpB17)),((_tmpB17->f1=_tmp576,((_tmpB17->f2=_tmp577,_tmpB17)))))),((_tmpB16->tl=widths_and_reqs,_tmpB16))))));};}}
# 1791
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp57B=Cyc_Parse_speclist2typ(_tmp571,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp570,_tmp57B,decls,_tmp572),widths_and_reqs);
# 1797
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57C));};};}
# 1800
_npop_handler(0);break;
# 1780
;_pop_region(temp);}case 127: _LL2A9: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1808 "parse.y"
{struct _tuple25 _tmpB18;yyval=Cyc_YY38(((_tmpB18.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB18.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB18.f3=0,_tmpB18)))))));}
break;}case 128: _LL2AA: {
# 1811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1810 "parse.y"
struct _tuple25 _tmp57E=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB19;yyval=Cyc_YY38(((_tmpB19.f1=_tmp57E.f1,((_tmpB19.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp57E.f2),((_tmpB19.f3=_tmp57E.f3,_tmpB19)))))));}
break;}case 129: _LL2AB: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812 "parse.y"
{struct _tuple25 _tmpB1A;yyval=Cyc_YY38(((_tmpB1A.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB1A.f2=Cyc_Parse_empty_spec(0),((_tmpB1A.f3=0,_tmpB1A)))))));}
break;}case 130: _LL2AC: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814 "parse.y"
struct _tuple25 _tmp581=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB1B;yyval=Cyc_YY38(((_tmpB1B.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp581.f1),((_tmpB1B.f2=_tmp581.f2,((_tmpB1B.f3=_tmp581.f3,_tmpB1B)))))));}
break;}case 131: _LL2AD: {
# 1818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1817 "parse.y"
{struct _tuple25 _tmpB1C;yyval=Cyc_YY38(((_tmpB1C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1C.f2=Cyc_Parse_empty_spec(0),((_tmpB1C.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB1C)))))));}
break;}case 132: _LL2AE: {
# 1820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1819 "parse.y"
struct _tuple25 _tmp584=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1D;yyval=Cyc_YY38(((_tmpB1D.f1=_tmp584.f1,((_tmpB1D.f2=_tmp584.f2,((_tmpB1D.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp584.f3),_tmpB1D)))))));}
break;}case 133: _LL2AF: {
# 1822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB1E;yyval=Cyc_YY38(((_tmpB1E.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1E.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB1E.f3=0,_tmpB1E)))))));}
break;}case 134: _LL2B0: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1827 "parse.y"
struct _tuple25 _tmp587=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1F;yyval=Cyc_YY38(((_tmpB1F.f1=_tmp587.f1,((_tmpB1F.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp587.f2),((_tmpB1F.f3=_tmp587.f3,_tmpB1F)))))));}
break;}case 135: _LL2B1: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829 "parse.y"
{struct _tuple25 _tmpB20;yyval=Cyc_YY38(((_tmpB20.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB20.f2=Cyc_Parse_empty_spec(0),((_tmpB20.f3=0,_tmpB20)))))));}
break;}case 136: _LL2B2: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831 "parse.y"
struct _tuple25 _tmp58A=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB21;yyval=Cyc_YY38(((_tmpB21.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp58A.f1),((_tmpB21.f2=_tmp58A.f2,((_tmpB21.f3=_tmp58A.f3,_tmpB21)))))));}
break;}case 137: _LL2B3: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1834 "parse.y"
{struct _tuple25 _tmpB22;yyval=Cyc_YY38(((_tmpB22.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB22.f2=Cyc_Parse_empty_spec(0),((_tmpB22.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB22)))))));}
break;}case 138: _LL2B4: {
# 1837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1836 "parse.y"
struct _tuple25 _tmp58D=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB23;yyval=Cyc_YY38(((_tmpB23.f1=_tmp58D.f1,((_tmpB23.f2=_tmp58D.f2,((_tmpB23.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp58D.f3),_tmpB23)))))));}
break;}case 139: _LL2B5: {
# 1839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1840 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 140: _LL2B6: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB24;yyval=Cyc_YY32(((_tmpB24=_region_malloc(yyr,sizeof(*_tmpB24)),((_tmpB24->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB24->tl=0,_tmpB24)))))));}
break;}case 141: _LL2B7: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1848 "parse.y"
{struct Cyc_List_List*_tmpB25;yyval=Cyc_YY32(((_tmpB25=_region_malloc(yyr,sizeof(*_tmpB25)),((_tmpB25->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB25->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB25)))))));}
break;}case 142: _LL2B8: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1853 "parse.y"
{struct _tuple24*_tmpB26;yyval=Cyc_YY31(((_tmpB26=_region_malloc(yyr,sizeof(*_tmpB26)),((_tmpB26->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB26->f2=0,((_tmpB26->f3=0,_tmpB26)))))))));}
break;}case 143: _LL2B9: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1857 "parse.y"
{struct Cyc_Parse_Declarator _tmpB2C;struct _tuple0*_tmpB2B;struct _tuple24*_tmpB2A;yyval=Cyc_YY31(((_tmpB2A=_region_malloc(yyr,sizeof(*_tmpB2A)),((_tmpB2A->f1=((_tmpB2C.id=((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B->f1=Cyc_Absyn_Rel_n(0),((_tmpB2B->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB2B)))))),((_tmpB2C.tms=0,_tmpB2C)))),((_tmpB2A->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB2A->f3=0,_tmpB2A)))))))));}
# 1859
break;}case 144: _LL2BA:
# 1863
{struct Cyc_Parse_Declarator _tmpB32;struct _tuple0*_tmpB31;struct _tuple24*_tmpB30;yyval=Cyc_YY31(((_tmpB30=_region_malloc(yyr,sizeof(*_tmpB30)),((_tmpB30->f1=((_tmpB32.id=((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->f1=Cyc_Absyn_Rel_n(0),((_tmpB31->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB31)))))),((_tmpB32.tms=0,_tmpB32)))),((_tmpB30->f2=0,((_tmpB30->f3=0,_tmpB30)))))))));}
# 1865
break;case 145: _LL2BB: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1865 "parse.y"
{struct _tuple24*_tmpB33;yyval=Cyc_YY31(((_tmpB33=_region_malloc(yyr,sizeof(*_tmpB33)),((_tmpB33->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB33->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB33->f3=0,_tmpB33)))))))));}
break;}case 146: _LL2BC: {
# 1868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1867 "parse.y"
{struct _tuple24*_tmpB34;yyval=Cyc_YY31(((_tmpB34=_region_malloc(yyr,sizeof(*_tmpB34)),((_tmpB34->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB34->f2=0,((_tmpB34->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB34)))))))));}
break;}case 147: _LL2BD: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp59C=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB35;struct Cyc_Absyn_TypeDecl*_tmp59E=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp59D,((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB35)))),_tmp59C,(unsigned int)(yyyylsp[0]).first_line);
# 1877
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB38;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB37;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=26,((_tmpB38.f1=_tmp59E,((_tmpB38.f2=0,_tmpB38)))))),_tmpB37)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1879
break;}case 148: _LL2BE: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1880 "parse.y"
int _tmp5A2=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB42;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB41;struct Cyc_Absyn_DatatypeInfo _tmpB40;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB3F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F[0]=((_tmpB42.tag=3,((_tmpB42.f1=((_tmpB40.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB41.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB41.is_extensible=_tmp5A2,_tmpB41))))),((_tmpB40.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB40)))),_tmpB42)))),_tmpB3F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1883
break;}case 149: _LL2BF: {
# 1885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1884 "parse.y"
int _tmp5A7=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB4C;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB4B;struct Cyc_Absyn_DatatypeFieldInfo _tmpB4A;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB49;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=((_tmpB4C.tag=4,((_tmpB4C.f1=((_tmpB4A.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB4B.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB4B.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB4B.is_extensible=_tmp5A7,_tmpB4B))))))),((_tmpB4A.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB4A)))),_tmpB4C)))),_tmpB49)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1889
break;}case 150: _LL2C0: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1892 "parse.y"
yyval=Cyc_YY34(0);
break;}case 151: _LL2C1: {
# 1895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1893 "parse.y"
yyval=Cyc_YY34(1);
break;}case 152: _LL2C2: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1897 "parse.y"
{struct Cyc_List_List*_tmpB4D;yyval=Cyc_YY37(((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4D->tl=0,_tmpB4D)))))));}
break;}case 153: _LL2C3: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1898 "parse.y"
{struct Cyc_List_List*_tmpB4E;yyval=Cyc_YY37(((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4E->tl=0,_tmpB4E)))))));}
break;}case 154: _LL2C4: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpB4F;yyval=Cyc_YY37(((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4F->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB4F)))))));}
break;}case 155: _LL2C5: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1900 "parse.y"
{struct Cyc_List_List*_tmpB50;yyval=Cyc_YY37(((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB50->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB50)))))));}
break;}case 156: _LL2C6:
# 1903
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 157: _LL2C7: {
# 1906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1905 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 158: _LL2C8: {
# 1908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1906 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 159: _LL2C9: {
# 1909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1910 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB51;yyval=Cyc_YY36(((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB51->typs=0,((_tmpB51->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB51->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB51)))))))))));}
break;}case 160: _LL2CA: {
# 1913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1912 "parse.y"
struct Cyc_List_List*_tmp5B1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB52;yyval=Cyc_YY36(((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB52->typs=_tmp5B1,((_tmpB52->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB52->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB52)))))))))));}
break;}case 161: _LL2CB: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1918 "parse.y"
yyval=yyyyvsp[0];
break;}case 162: _LL2CC: {
# 1921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1920 "parse.y"
struct Cyc_Parse_Declarator _tmp5B3=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB53;yyval=Cyc_YY30(((_tmpB53.id=_tmp5B3.id,((_tmpB53.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B3.tms),_tmpB53)))));}
break;}case 163: _LL2CD: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 164: _LL2CE: {
# 1930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1929 "parse.y"
struct Cyc_Parse_Declarator _tmp5B5=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB54;yyval=Cyc_YY30(((_tmpB54.id=_tmp5B5.id,((_tmpB54.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B5.tms),_tmpB54)))));}
break;}case 165: _LL2CF: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1935 "parse.y"
{struct Cyc_Parse_Declarator _tmpB55;yyval=Cyc_YY30(((_tmpB55.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB55.tms=0,_tmpB55)))));}
break;}case 166: _LL2D0: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1937 "parse.y"
yyval=yyyyvsp[1];
break;}case 167: _LL2D1: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1941 "parse.y"
struct Cyc_Parse_Declarator _tmp5B8=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB5B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB5A;struct Cyc_List_List*_tmpB59;_tmp5B8.tms=((_tmpB59=_region_malloc(yyr,sizeof(*_tmpB59)),((_tmpB59->hd=(void*)((_tmpB5B=_region_malloc(yyr,sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5A.tag=5,((_tmpB5A.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB5A.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB5A)))))),_tmpB5B)))),((_tmpB59->tl=_tmp5B8.tms,_tmpB59))))));}
yyval=yyyyvsp[2];
# 1945
break;}case 168: _LL2D2: {
# 1947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1946 "parse.y"
{struct Cyc_List_List*_tmpB65;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB64;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB63;struct Cyc_Parse_Declarator _tmpB62;yyval=Cyc_YY30(((_tmpB62.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB62.tms=((_tmpB65=_region_malloc(yyr,sizeof(*_tmpB65)),((_tmpB65->hd=(void*)((_tmpB63=_region_malloc(yyr,sizeof(*_tmpB63)),((_tmpB63[0]=((_tmpB64.tag=0,((_tmpB64.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB64.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB64)))))),_tmpB63)))),((_tmpB65->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB65)))))),_tmpB62)))));}
break;}case 169: _LL2D3: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1948 "parse.y"
{struct Cyc_List_List*_tmpB6F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB6E;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB6D;struct Cyc_Parse_Declarator _tmpB6C;yyval=Cyc_YY30(((_tmpB6C.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB6C.tms=(
(_tmpB6F=_region_malloc(yyr,sizeof(*_tmpB6F)),((_tmpB6F->hd=(void*)((_tmpB6D=_region_malloc(yyr,sizeof(*_tmpB6D)),((_tmpB6D[0]=((_tmpB6E.tag=1,((_tmpB6E.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB6E.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB6E.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB6E)))))))),_tmpB6D)))),((_tmpB6F->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB6F)))))),_tmpB6C)))));}
break;}case 170: _LL2D4: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1951 "parse.y"
struct _tuple26*_stmttmp2F=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5C5;int _tmp5C6;struct Cyc_Absyn_VarargInfo*_tmp5C7;void*_tmp5C8;struct Cyc_List_List*_tmp5C9;struct _tuple26*_tmp5C4=_stmttmp2F;_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;_tmp5C7=_tmp5C4->f3;_tmp5C8=_tmp5C4->f4;_tmp5C9=_tmp5C4->f5;
{struct Cyc_List_List*_tmpB84;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB83;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB80;struct Cyc_Parse_Declarator _tmpB7F;yyval=Cyc_YY30(((_tmpB7F.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB7F.tms=((_tmpB84=_region_malloc(yyr,sizeof(*_tmpB84)),((_tmpB84->hd=(void*)((_tmpB80=_region_malloc(yyr,sizeof(*_tmpB80)),((_tmpB80[0]=((_tmpB83.tag=3,((_tmpB83.f1=(void*)((_tmpB81=_region_malloc(yyr,sizeof(*_tmpB81)),((_tmpB81[0]=((_tmpB82.tag=1,((_tmpB82.f1=_tmp5C5,((_tmpB82.f2=_tmp5C6,((_tmpB82.f3=_tmp5C7,((_tmpB82.f4=_tmp5C8,((_tmpB82.f5=_tmp5C9,_tmpB82)))))))))))),_tmpB81)))),_tmpB83)))),_tmpB80)))),((_tmpB84->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB84)))))),_tmpB7F)))));}
# 1954
break;}case 171: _LL2D5: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1955 "parse.y"
{struct Cyc_List_List*_tmpB99;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB98;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB97;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB96;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB95;struct Cyc_Parse_Declarator _tmpB94;yyval=Cyc_YY30(((_tmpB94.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB94.tms=(
(_tmpB99=_region_malloc(yyr,sizeof(*_tmpB99)),((_tmpB99->hd=(void*)((_tmpB95=_region_malloc(yyr,sizeof(*_tmpB95)),((_tmpB95[0]=((_tmpB98.tag=3,((_tmpB98.f1=(void*)((_tmpB96=_region_malloc(yyr,sizeof(*_tmpB96)),((_tmpB96[0]=((_tmpB97.tag=1,((_tmpB97.f1=0,((_tmpB97.f2=0,((_tmpB97.f3=0,((_tmpB97.f4=
# 1958
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpB97.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpB97)))))))))))),_tmpB96)))),_tmpB98)))),_tmpB95)))),((_tmpB99->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB99)))))),_tmpB94)))));}
# 1961
break;}case 172: _LL2D6: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1962 "parse.y"
{struct Cyc_List_List*_tmpBAE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAD;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBAC;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBAB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBAA;struct Cyc_Parse_Declarator _tmpBA9;yyval=Cyc_YY30(((_tmpBA9.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA9.tms=((_tmpBAE=_region_malloc(yyr,sizeof(*_tmpBAE)),((_tmpBAE->hd=(void*)((_tmpBAA=_region_malloc(yyr,sizeof(*_tmpBAA)),((_tmpBAA[0]=((_tmpBAD.tag=3,((_tmpBAD.f1=(void*)((_tmpBAB=_region_malloc(yyr,sizeof(*_tmpBAB)),((_tmpBAB[0]=((_tmpBAC.tag=0,((_tmpBAC.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBAC.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBAC)))))),_tmpBAB)))),_tmpBAD)))),_tmpBAA)))),((_tmpBAE->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBAE)))))),_tmpBA9)))));}
break;}case 173: _LL2D7: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1965 "parse.y"
struct Cyc_List_List*_tmp5DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBB8;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBB7;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBB6;struct Cyc_Parse_Declarator _tmpBB5;yyval=Cyc_YY30(((_tmpBB5.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB5.tms=((_tmpBB8=_region_malloc(yyr,sizeof(*_tmpBB8)),((_tmpBB8->hd=(void*)((_tmpBB6=_region_malloc(yyr,sizeof(*_tmpBB6)),((_tmpBB6[0]=((_tmpBB7.tag=4,((_tmpBB7.f1=_tmp5DC,((_tmpBB7.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB7.f3=0,_tmpBB7)))))))),_tmpBB6)))),((_tmpBB8->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBB8)))))),_tmpBB5)))));}
# 1968
break;}case 174: _LL2D8: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1969 "parse.y"
{struct Cyc_List_List*_tmpBC2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBC0;struct Cyc_Parse_Declarator _tmpBBF;yyval=Cyc_YY30(((_tmpBBF.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBBF.tms=((_tmpBC2=_region_malloc(yyr,sizeof(*_tmpBC2)),((_tmpBC2->hd=(void*)((_tmpBC0=_region_malloc(yyr,sizeof(*_tmpBC0)),((_tmpBC0[0]=((_tmpBC1.tag=5,((_tmpBC1.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC1.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC1)))))),_tmpBC0)))),((_tmpBC2->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBC2)))))),_tmpBBF)))));}
# 1972
break;}case 175: _LL2D9: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1983 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC3;yyval=Cyc_YY30(((_tmpBC3.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC3.tms=0,_tmpBC3)))));}
break;}case 176: _LL2DA: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1985 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC4;yyval=Cyc_YY30(((_tmpBC4.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC4.tms=0,_tmpBC4)))));}
break;}case 177: _LL2DB: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1987 "parse.y"
yyval=yyyyvsp[1];
break;}case 178: _LL2DC: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1991 "parse.y"
struct Cyc_Parse_Declarator _tmp5E7=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBCA;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC9;struct Cyc_List_List*_tmpBC8;_tmp5E7.tms=((_tmpBC8=_region_malloc(yyr,sizeof(*_tmpBC8)),((_tmpBC8->hd=(void*)((_tmpBCA=_region_malloc(yyr,sizeof(*_tmpBCA)),((_tmpBCA[0]=((_tmpBC9.tag=5,((_tmpBC9.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC9.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC9)))))),_tmpBCA)))),((_tmpBC8->tl=_tmp5E7.tms,_tmpBC8))))));}
yyval=yyyyvsp[2];
# 1995
break;}case 179: _LL2DD: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1996 "parse.y"
struct Cyc_Parse_Declarator _tmp5EB=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD4;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD3;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBD2;struct Cyc_Parse_Declarator _tmpBD1;yyval=Cyc_YY30(((_tmpBD1.id=_tmp5EB.id,((_tmpBD1.tms=(
(_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4->hd=(void*)((_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD3.tag=0,((_tmpBD3.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBD3.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBD3)))))),_tmpBD2)))),((_tmpBD4->tl=_tmp5EB.tms,_tmpBD4)))))),_tmpBD1)))));}
break;}case 180: _LL2DE: {
# 2001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp5F0=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBDE;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBDD;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBDC;struct Cyc_Parse_Declarator _tmpBDB;yyval=Cyc_YY30(((_tmpBDB.id=_tmp5F0.id,((_tmpBDB.tms=(
(_tmpBDE=_region_malloc(yyr,sizeof(*_tmpBDE)),((_tmpBDE->hd=(void*)((_tmpBDC=_region_malloc(yyr,sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDD.tag=1,((_tmpBDD.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBDD.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBDD.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBDD)))))))),_tmpBDC)))),((_tmpBDE->tl=_tmp5F0.tms,_tmpBDE)))))),_tmpBDB)))));}
# 2004
break;}case 181: _LL2DF: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2005 "parse.y"
struct _tuple26*_stmttmp2E=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5F6;int _tmp5F7;struct Cyc_Absyn_VarargInfo*_tmp5F8;void*_tmp5F9;struct Cyc_List_List*_tmp5FA;struct _tuple26*_tmp5F5=_stmttmp2E;_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=_tmp5F5->f3;_tmp5F9=_tmp5F5->f4;_tmp5FA=_tmp5F5->f5;{
struct Cyc_Parse_Declarator _tmp5FB=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF3;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBF1;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBF0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBEF;struct Cyc_Parse_Declarator _tmpBEE;yyval=Cyc_YY30(((_tmpBEE.id=_tmp5FB.id,((_tmpBEE.tms=((_tmpBF3=_region_malloc(yyr,sizeof(*_tmpBF3)),((_tmpBF3->hd=(void*)((_tmpBEF=_region_malloc(yyr,sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF2.tag=3,((_tmpBF2.f1=(void*)((_tmpBF0=_region_malloc(yyr,sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBF1.tag=1,((_tmpBF1.f1=_tmp5F6,((_tmpBF1.f2=_tmp5F7,((_tmpBF1.f3=_tmp5F8,((_tmpBF1.f4=_tmp5F9,((_tmpBF1.f5=_tmp5FA,_tmpBF1)))))))))))),_tmpBF0)))),_tmpBF2)))),_tmpBEF)))),((_tmpBF3->tl=_tmp5FB.tms,_tmpBF3)))))),_tmpBEE)))));}
# 2009
break;};}case 182: _LL2E0: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2010 "parse.y"
struct Cyc_Parse_Declarator _tmp602=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC08;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC07;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC06;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC05;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC04;struct Cyc_Parse_Declarator _tmpC03;yyval=Cyc_YY30(((_tmpC03.id=_tmp602.id,((_tmpC03.tms=(
(_tmpC08=_region_malloc(yyr,sizeof(*_tmpC08)),((_tmpC08->hd=(void*)((_tmpC04=_region_malloc(yyr,sizeof(*_tmpC04)),((_tmpC04[0]=((_tmpC07.tag=3,((_tmpC07.f1=(void*)((_tmpC05=_region_malloc(yyr,sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpC06.tag=1,((_tmpC06.f1=0,((_tmpC06.f2=0,((_tmpC06.f3=0,((_tmpC06.f4=
# 2014
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC06.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC06)))))))))))),_tmpC05)))),_tmpC07)))),_tmpC04)))),((_tmpC08->tl=_tmp602.tms,_tmpC08)))))),_tmpC03)))));}
# 2017
break;}case 183: _LL2E1: {
# 2019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2018 "parse.y"
struct Cyc_Parse_Declarator _tmp609=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC1D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC1C;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC1B;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC1A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC19;struct Cyc_Parse_Declarator _tmpC18;yyval=Cyc_YY30(((_tmpC18.id=_tmp609.id,((_tmpC18.tms=((_tmpC1D=_region_malloc(yyr,sizeof(*_tmpC1D)),((_tmpC1D->hd=(void*)((_tmpC19=_region_malloc(yyr,sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC1C.tag=3,((_tmpC1C.f1=(void*)((_tmpC1A=_region_malloc(yyr,sizeof(*_tmpC1A)),((_tmpC1A[0]=((_tmpC1B.tag=0,((_tmpC1B.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC1B.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC1B)))))),_tmpC1A)))),_tmpC1C)))),_tmpC19)))),((_tmpC1D->tl=_tmp609.tms,_tmpC1D)))))),_tmpC18)))));}
break;}case 184: _LL2E2: {
# 2022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2022 "parse.y"
struct Cyc_List_List*_tmp610=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC27;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC26;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC25;struct Cyc_Parse_Declarator _tmpC24;yyval=Cyc_YY30(((_tmpC24.id=_tmp611.id,((_tmpC24.tms=((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27->hd=(void*)((_tmpC25=_region_malloc(yyr,sizeof(*_tmpC25)),((_tmpC25[0]=((_tmpC26.tag=4,((_tmpC26.f1=_tmp610,((_tmpC26.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC26.f3=0,_tmpC26)))))))),_tmpC25)))),((_tmpC27->tl=_tmp611.tms,_tmpC27)))))),_tmpC24)))));}
# 2026
break;}case 185: _LL2E3: {
# 2028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2027 "parse.y"
struct Cyc_Parse_Declarator _tmp616=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC31;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC30;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC2F;struct Cyc_Parse_Declarator _tmpC2E;yyval=Cyc_YY30(((_tmpC2E.id=_tmp616.id,((_tmpC2E.tms=((_tmpC31=_region_malloc(yyr,sizeof(*_tmpC31)),((_tmpC31->hd=(void*)((_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC30.tag=5,((_tmpC30.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC30.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC30)))))),_tmpC2F)))),((_tmpC31->tl=_tmp616.tms,_tmpC31)))))),_tmpC2E)))));}
# 2030
break;}case 186: _LL2E4: {
# 2032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2040 "parse.y"
yyval=yyyyvsp[0];
break;}case 187: _LL2E5: {
# 2043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2041 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 188: _LL2E6: {
# 2044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2045 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC37;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC36;struct Cyc_List_List*_tmpC35;ans=((_tmpC35=_region_malloc(yyr,sizeof(*_tmpC35)),((_tmpC35->hd=(void*)((_tmpC37=_region_malloc(yyr,sizeof(*_tmpC37)),((_tmpC37[0]=((_tmpC36.tag=5,((_tmpC36.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC36.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC36)))))),_tmpC37)))),((_tmpC35->tl=ans,_tmpC35))))));}{
# 2049
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _stmttmp2C=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp61F;union Cyc_Absyn_Constraint*_tmp620;union Cyc_Absyn_Constraint*_tmp621;struct _tuple21 _tmp61E=_stmttmp2C;_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=_tmp61E.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC38;ptrloc=((_tmpC38=_cycalloc_atomic(sizeof(*_tmpC38)),((_tmpC38->ptr_loc=_tmp61F,((_tmpC38->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC38->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC38))))))));}{
# 2054
struct _tuple14 _stmttmp2D=Cyc_Parse_collapse_pointer_quals(_tmp61F,_tmp620,_tmp621,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp624;union Cyc_Absyn_Constraint*_tmp625;union Cyc_Absyn_Constraint*_tmp626;void*_tmp627;struct _tuple14 _tmp623=_stmttmp2D;_tmp624=_tmp623.f1;_tmp625=_tmp623.f2;_tmp626=_tmp623.f3;_tmp627=_tmp623.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC42;struct Cyc_Absyn_PtrAtts _tmpC41;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC40;struct Cyc_List_List*_tmpC3F;ans=((_tmpC3F=_region_malloc(yyr,sizeof(*_tmpC3F)),((_tmpC3F->hd=(void*)((_tmpC42=_region_malloc(yyr,sizeof(*_tmpC42)),((_tmpC42[0]=((_tmpC40.tag=2,((_tmpC40.f1=((_tmpC41.rgn=_tmp627,((_tmpC41.nullable=_tmp624,((_tmpC41.bounds=_tmp625,((_tmpC41.zero_term=_tmp626,((_tmpC41.ptrloc=ptrloc,_tmpC41)))))))))),((_tmpC40.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC40)))))),_tmpC42)))),((_tmpC3F->tl=ans,_tmpC3F))))));}
yyval=Cyc_YY29(ans);
# 2058
break;};};}case 189: _LL2E7:
# 2060
 yyval=Cyc_YY57(0);
break;case 190: _LL2E8: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2061 "parse.y"
{struct Cyc_List_List*_tmpC43;yyval=Cyc_YY57(((_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC43->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC43)))))));}
break;}case 191: _LL2E9: {
# 2064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2066 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC46;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC45;yyval=Cyc_YY56((void*)((_tmpC45=_region_malloc(yyr,sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=0,((_tmpC46.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC46)))),_tmpC45)))));}
break;}case 192: _LL2EA: {
# 2069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2068 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC49;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC48;yyval=Cyc_YY56((void*)((_tmpC48=_region_malloc(yyr,sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=1,((_tmpC49.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC49)))),_tmpC48)))));}
break;}case 193: _LL2EB: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2070 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC4C;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC4B;yyval=Cyc_YY56((void*)((_tmpC4B=_region_malloc(yyr,sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4C.tag=2,_tmpC4C)),_tmpC4B)))));}
break;}case 194: _LL2EC: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC4F;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC4E;yyval=Cyc_YY56((void*)((_tmpC4E=_region_malloc(yyr,sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC4F.tag=3,_tmpC4F)),_tmpC4E)))));}
break;}case 195: _LL2ED: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC52;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC51;yyval=Cyc_YY56((void*)((_tmpC51=_region_malloc(yyr,sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC52.tag=4,_tmpC52)),_tmpC51)))));}
break;}case 196: _LL2EE: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC55;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC54;yyval=Cyc_YY56((void*)((_tmpC54=_region_malloc(yyr,sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=5,_tmpC55)),_tmpC54)))));}
break;}case 197: _LL2EF: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC58;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC57;yyval=Cyc_YY56((void*)((_tmpC57=_region_malloc(yyr,sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC58.tag=6,_tmpC58)),_tmpC57)))));}
break;}case 198: _LL2F0: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2080 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC5B;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC5A;yyval=Cyc_YY56((void*)((_tmpC5A=_region_malloc(yyr,sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5B.tag=7,_tmpC5B)),_tmpC5A)))));}
break;}case 199: _LL2F1: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2086 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5C;yyval=Cyc_YY1(((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C->f1=loc,((_tmpC5C->f2=Cyc_Absyn_true_conref,((_tmpC5C->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5C)))))))));}
# 2089
break;}case 200: _LL2F2: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2090 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5D;yyval=Cyc_YY1(((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D->f1=loc,((_tmpC5D->f2=Cyc_Absyn_false_conref,((_tmpC5D->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5D)))))))));}
# 2093
break;}case 201: _LL2F3: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2094 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5E;yyval=Cyc_YY1(((_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E->f1=loc,((_tmpC5E->f2=Cyc_Absyn_true_conref,((_tmpC5E->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC5E)))))))));}
# 2097
break;}case 202: _LL2F4:
# 2099
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 203: _LL2F5: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2100 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC61;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC60;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC61.tag=1,((_tmpC61.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC61)))),_tmpC60))))));}
break;}case 204: _LL2F6:
# 2103
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 205: _LL2F7: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2104 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 206: _LL2F8: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2105 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 207: _LL2F9:
# 2108
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 208: _LL2FA: {
# 2111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2111 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 209: _LL2FB: {
# 2114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2112 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 210: _LL2FC:
# 2115
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 211: _LL2FD: {
# 2118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2117 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 212: _LL2FE: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2122 "parse.y"
{struct _tuple26*_tmpC62;yyval=Cyc_YY42(((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC62->f2=0,((_tmpC62->f3=0,((_tmpC62->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC62->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC62)))))))))))));}
break;}case 213: _LL2FF: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2124 "parse.y"
{struct _tuple26*_tmpC63;yyval=Cyc_YY42(((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC63->f2=1,((_tmpC63->f3=0,((_tmpC63->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC63->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC63)))))))))))));}
break;}case 214: _LL300: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2127 "parse.y"
struct _tuple8*_stmttmp2B=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp645;struct Cyc_Absyn_Tqual _tmp646;void*_tmp647;struct _tuple8*_tmp644=_stmttmp2B;_tmp645=_tmp644->f1;_tmp646=_tmp644->f2;_tmp647=_tmp644->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC64;struct Cyc_Absyn_VarargInfo*_tmp648=(_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->name=_tmp645,((_tmpC64->tq=_tmp646,((_tmpC64->type=_tmp647,((_tmpC64->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC64)))))))));
{struct _tuple26*_tmpC65;yyval=Cyc_YY42(((_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65->f1=0,((_tmpC65->f2=0,((_tmpC65->f3=_tmp648,((_tmpC65->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC65->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC65)))))))))))));}
# 2131
break;};}case 215: _LL301: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2133 "parse.y"
struct _tuple8*_stmttmp2A=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp64C;struct Cyc_Absyn_Tqual _tmp64D;void*_tmp64E;struct _tuple8*_tmp64B=_stmttmp2A;_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;_tmp64E=_tmp64B->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC66;struct Cyc_Absyn_VarargInfo*_tmp64F=(_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->name=_tmp64C,((_tmpC66->tq=_tmp64D,((_tmpC66->type=_tmp64E,((_tmpC66->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC66)))))))));
{struct _tuple26*_tmpC67;yyval=Cyc_YY42(((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC67->f2=0,((_tmpC67->f3=_tmp64F,((_tmpC67->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC67->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC67)))))))))))));}
# 2137
break;};}case 216: _LL302: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2141 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC6A;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC69;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=1,((_tmpC6A.f1=0,_tmpC6A)))),_tmpC69))))));}
break;}case 217: _LL303: {
# 2144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2142 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 218: _LL304:
# 2145
 yyval=Cyc_YY52(0);
break;case 219: _LL305: {
# 2148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2146 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC6D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC6C;yyval=Cyc_YY52((void*)((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=24,((_tmpC6D.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC6D)))),_tmpC6C)))));}
break;}case 220: _LL306:
# 2149
 yyval=Cyc_YY53(0);
break;case 221: _LL307: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2151 "parse.y"
yyval=yyyyvsp[1];
break;}case 222: _LL308: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2159 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC70;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp656=(_tmpC6F=_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC70.tag=2,((_tmpC70.f1=0,((_tmpC70.f2=& Cyc_Tcutil_trk,_tmpC70)))))),_tmpC6F)));
struct _dyneither_ptr _tmp657=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp658=Cyc_Parse_id2type(_tmp657,(void*)_tmp656);
{struct _tuple29*_tmpC7A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC79;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC78;struct Cyc_List_List*_tmpC77;yyval=Cyc_YY53(((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77->hd=((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->f1=(void*)((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=24,((_tmpC79.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC79)))),_tmpC78)))),((_tmpC7A->f2=_tmp658,_tmpC7A)))))),((_tmpC77->tl=0,_tmpC77)))))));}
# 2164
break;}case 223: _LL309: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2166 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC7D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC7C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65F=(_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=2,((_tmpC7D.f1=0,((_tmpC7D.f2=& Cyc_Tcutil_trk,_tmpC7D)))))),_tmpC7C)));
struct _dyneither_ptr _tmp660=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp661=Cyc_Parse_id2type(_tmp660,(void*)_tmp65F);
{struct _tuple29*_tmpC87;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC86;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC85;struct Cyc_List_List*_tmpC84;yyval=Cyc_YY53(((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->f1=(void*)((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85[0]=((_tmpC86.tag=24,((_tmpC86.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC86)))),_tmpC85)))),((_tmpC87->f2=_tmp661,_tmpC87)))))),((_tmpC84->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC84)))))));}
# 2171
break;}case 224: _LL30A:
# 2173
 yyval=Cyc_YY34(0);
break;case 225: _LL30B: {
# 2176
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2177 "parse.y"
{const char*_tmpC88;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC88="inject",_tag_dyneither(_tmpC88,sizeof(char),7))))!= 0){
const char*_tmpC89;Cyc_Parse_err(((_tmpC89="missing type in function declaration",_tag_dyneither(_tmpC89,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2181
break;}case 226: _LL30C: {
# 2183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2184 "parse.y"
yyval=yyyyvsp[0];
break;}case 227: _LL30D: {
# 2187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2185 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 228: _LL30E: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2189 "parse.y"
yyval=Cyc_YY43(0);
break;}case 229: _LL30F: {
# 2192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2190 "parse.y"
yyval=yyyyvsp[1];
break;}case 230: _LL310: {
# 2193
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2192 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC8F;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC8E;struct Cyc_List_List*_tmpC8D;yyval=Cyc_YY43(((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->hd=(void*)((_tmpC8F=_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC8E.tag=25,((_tmpC8E.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC8E)))),_tmpC8F)))),((_tmpC8D->tl=0,_tmpC8D)))))));}
break;}case 231: _LL311: {
# 2195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2194 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC90;yyval=Cyc_YY43(((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpC90->tl=0,_tmpC90)))))));}
# 2197
break;}case 232: _LL312: {
# 2199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC96;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC95;struct Cyc_List_List*_tmpC94;yyval=Cyc_YY43(((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->hd=(void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC95.tag=23,((_tmpC95.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC95)))),_tmpC96)))),((_tmpC94->tl=0,_tmpC94)))))));}
# 2205
break;}case 233: _LL313: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2206 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC9C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC9B;struct Cyc_List_List*_tmpC9A;yyval=Cyc_YY43(((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->hd=(void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9B.tag=23,((_tmpC9B.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC9B)))),_tmpC9C)))),((_tmpC9A->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpC9A)))))));}
# 2209
break;}case 234: _LL314: {
# 2211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpC9D;yyval=Cyc_YY41(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpC9D->tl=0,_tmpC9D)))))));}
break;}case 235: _LL315: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2216 "parse.y"
{struct Cyc_List_List*_tmpC9E;yyval=Cyc_YY41(((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpC9E->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC9E)))))));}
break;}case 236: _LL316: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _stmttmp27=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp677;struct Cyc_Parse_Type_specifier _tmp678;struct Cyc_List_List*_tmp679;struct _tuple25 _tmp676=_stmttmp27;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;
if(_tmp677.loc == 0)_tmp677.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _stmttmp28=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_Parse_Declarator _tmp67A=_stmttmp28;_tmp67B=_tmp67A.id;_tmp67C=_tmp67A.tms;{
void*_tmp67D=Cyc_Parse_speclist2typ(_tmp678,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _stmttmp29=Cyc_Parse_apply_tms(_tmp677,_tmp67D,_tmp679,_tmp67C);struct Cyc_Absyn_Tqual _tmp67F;void*_tmp680;struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp682;struct _tuple13 _tmp67E=_stmttmp29;_tmp67F=_tmp67E.f1;_tmp680=_tmp67E.f2;_tmp681=_tmp67E.f3;_tmp682=_tmp67E.f4;
if(_tmp681 != 0){
const char*_tmpC9F;Cyc_Parse_err(((_tmpC9F="parameter with bad type params",_tag_dyneither(_tmpC9F,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp67B)){
const char*_tmpCA0;Cyc_Parse_err(((_tmpCA0="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCA0,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp67B).f2;
if(_tmp682 != 0){
const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA3="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCA3,sizeof(char),40))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}
{struct _tuple8*_tmpCA4;yyval=Cyc_YY40(((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->f1=idopt,((_tmpCA4->f2=_tmp67F,((_tmpCA4->f3=_tmp680,_tmpCA4)))))))));}
# 2236
break;};};};}case 237: _LL317: {
# 2238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2237 "parse.y"
struct _tuple25 _stmttmp26=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp689;struct Cyc_Parse_Type_specifier _tmp68A;struct Cyc_List_List*_tmp68B;struct _tuple25 _tmp688=_stmttmp26;_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;_tmp68B=_tmp688.f3;
if(_tmp689.loc == 0)_tmp689.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp68C=Cyc_Parse_speclist2typ(_tmp68A,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp68B != 0){
const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA7="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCA7,sizeof(char),38))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}
{struct _tuple8*_tmpCA8;yyval=Cyc_YY40(((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=0,((_tmpCA8->f2=_tmp689,((_tmpCA8->f3=_tmp68C,_tmpCA8)))))))));}
# 2244
break;};}case 238: _LL318: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2245 "parse.y"
struct _tuple25 _stmttmp24=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp691;struct Cyc_Parse_Type_specifier _tmp692;struct Cyc_List_List*_tmp693;struct _tuple25 _tmp690=_stmttmp24;_tmp691=_tmp690.f1;_tmp692=_tmp690.f2;_tmp693=_tmp690.f3;
if(_tmp691.loc == 0)_tmp691.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp694=Cyc_Parse_speclist2typ(_tmp692,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp695=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _stmttmp25=Cyc_Parse_apply_tms(_tmp691,_tmp694,_tmp693,_tmp695);struct Cyc_Absyn_Tqual _tmp697;void*_tmp698;struct Cyc_List_List*_tmp699;struct Cyc_List_List*_tmp69A;struct _tuple13 _tmp696=_stmttmp25;_tmp697=_tmp696.f1;_tmp698=_tmp696.f2;_tmp699=_tmp696.f3;_tmp69A=_tmp696.f4;
if(_tmp699 != 0){
const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAB="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCAB,sizeof(char),49))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}
# 2253
if(_tmp69A != 0){
const char*_tmpCAE;void*_tmpCAD;(_tmpCAD=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAE="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCAE,sizeof(char),38))),_tag_dyneither(_tmpCAD,sizeof(void*),0)));}
{struct _tuple8*_tmpCAF;yyval=Cyc_YY40(((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->f1=0,((_tmpCAF->f2=_tmp697,((_tmpCAF->f3=_tmp698,_tmpCAF)))))))));}
# 2257
break;};}case 239: _LL319: {
# 2259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2261 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 240: _LL31A: {
# 2264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2265 "parse.y"
{struct _dyneither_ptr*_tmpCB2;struct Cyc_List_List*_tmpCB1;yyval=Cyc_YY39(((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCB2)))),((_tmpCB1->tl=0,_tmpCB1)))))));}
break;}case 241: _LL31B: {
# 2268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2267 "parse.y"
{struct _dyneither_ptr*_tmpCB5;struct Cyc_List_List*_tmpCB4;yyval=Cyc_YY39(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCB5)))),((_tmpCB4->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCB4)))))));}
break;}case 242: _LL31C: {
# 2270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2271 "parse.y"
yyval=yyyyvsp[0];
break;}case 243: _LL31D: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2272 "parse.y"
yyval=yyyyvsp[0];
break;}case 244: _LL31E: {
# 2275
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2277 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCB8;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCB7;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7[0]=((_tmpCB8.tag=35,((_tmpCB8.f1=0,((_tmpCB8.f2=0,_tmpCB8)))))),_tmpCB7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31F: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2279 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBB;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBA;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBB.tag=35,((_tmpCBB.f1=0,((_tmpCBB.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBB)))))),_tmpCBA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL320: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2281 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBE;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBD;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBE.tag=35,((_tmpCBE.f1=0,((_tmpCBE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBE)))))),_tmpCBD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 247: _LL321: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2283 "parse.y"
struct _dyneither_ptr*_tmpCC1;struct _tuple0*_tmpCC0;struct Cyc_Absyn_Vardecl*_tmp6AA=Cyc_Absyn_new_vardecl(((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->f1=Cyc_Absyn_Loc_n,((_tmpCC0->f2=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCC1)))),_tmpCC0)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2286
(_tmp6AA->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCC4;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCC3;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC4.tag=26,((_tmpCC4.f1=_tmp6AA,((_tmpCC4.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC4.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCC4.f4=0,_tmpCC4)))))))))),_tmpCC3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2289
break;}case 248: _LL322: {
# 2291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2290 "parse.y"
void*_tmp6AF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCC7;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCC6;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC7.tag=27,((_tmpCC7.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC7.f2=_tmp6AF,((_tmpCC7.f3=0,_tmpCC7)))))))),_tmpCC6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2293
break;}case 249: _LL323: {
# 2295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpCCA;struct Cyc_List_List*_tmpCC9;yyval=Cyc_YY7(((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->hd=((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=0,((_tmpCCA->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCCA)))))),((_tmpCC9->tl=0,_tmpCC9)))))));}
break;}case 250: _LL324: {
# 2301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2300 "parse.y"
{struct _tuple30*_tmpCCD;struct Cyc_List_List*_tmpCCC;yyval=Cyc_YY7(((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->hd=((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCCD->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCCD)))))),((_tmpCCC->tl=0,_tmpCCC)))))));}
break;}case 251: _LL325: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2302 "parse.y"
{struct _tuple30*_tmpCD0;struct Cyc_List_List*_tmpCCF;yyval=Cyc_YY7(((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->hd=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->f1=0,((_tmpCD0->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCD0)))))),((_tmpCCF->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCCF)))))));}
break;}case 252: _LL326: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2304 "parse.y"
{struct _tuple30*_tmpCD3;struct Cyc_List_List*_tmpCD2;yyval=Cyc_YY7(((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->hd=((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD3->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCD3)))))),((_tmpCD2->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCD2)))))));}
break;}case 253: _LL327: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2308 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 254: _LL328: {
# 2311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2313 "parse.y"
{struct Cyc_List_List*_tmpCD4;yyval=Cyc_YY44(((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCD4->tl=0,_tmpCD4)))))));}
break;}case 255: _LL329: {
# 2316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2314 "parse.y"
{struct Cyc_List_List*_tmpCD5;yyval=Cyc_YY44(((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCD5->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCD5)))))));}
break;}case 256: _LL32A: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2318 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCD8;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCD7;yyval=Cyc_YY45((void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD8.tag=0,((_tmpCD8.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCD8)))),_tmpCD7)))));}
break;}case 257: _LL32B: {
# 2321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2319 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCDE;struct _dyneither_ptr*_tmpCDD;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCDC;yyval=Cyc_YY45((void*)((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDE.tag=1,((_tmpCDE.f1=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCDD)))),_tmpCDE)))),_tmpCDC)))));}
break;}case 258: _LL32C: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2324 "parse.y"
struct _tuple25 _stmttmp23=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C2;struct Cyc_Parse_Type_specifier _tmp6C3;struct Cyc_List_List*_tmp6C4;struct _tuple25 _tmp6C1=_stmttmp23;_tmp6C2=_tmp6C1.f1;_tmp6C3=_tmp6C1.f2;_tmp6C4=_tmp6C1.f3;{
void*_tmp6C5=Cyc_Parse_speclist2typ(_tmp6C3,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6C4 != 0){
const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE1="ignoring attributes in type",_tag_dyneither(_tmpCE1,sizeof(char),28))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}
{struct _tuple8*_tmpCE2;yyval=Cyc_YY40(((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->f1=0,((_tmpCE2->f2=_tmp6C2,((_tmpCE2->f3=_tmp6C5,_tmpCE2)))))))));}
# 2330
break;};}case 259: _LL32D: {
# 2332
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2331 "parse.y"
struct _tuple25 _stmttmp22=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6CA;struct Cyc_Parse_Type_specifier _tmp6CB;struct Cyc_List_List*_tmp6CC;struct _tuple25 _tmp6C9=_stmttmp22;_tmp6CA=_tmp6C9.f1;_tmp6CB=_tmp6C9.f2;_tmp6CC=_tmp6C9.f3;{
void*_tmp6CD=Cyc_Parse_speclist2typ(_tmp6CB,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6CE=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6CF=Cyc_Parse_apply_tms(_tmp6CA,_tmp6CD,_tmp6CC,_tmp6CE);
if(_tmp6CF.f3 != 0){
# 2337
const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE5="bad type params, ignoring",_tag_dyneither(_tmpCE5,sizeof(char),26))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
if(_tmp6CF.f4 != 0){
const char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE8="bad specifiers, ignoring",_tag_dyneither(_tmpCE8,sizeof(char),25))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}
{struct _tuple8*_tmpCE9;yyval=Cyc_YY40(((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->f1=0,((_tmpCE9->f2=_tmp6CF.f1,((_tmpCE9->f3=_tmp6CF.f2,_tmpCE9)))))))));}
# 2342
break;};}case 260: _LL32E: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2345 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 261: _LL32F: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2346 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCEC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEB;yyval=Cyc_YY47((void*)((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB[0]=((_tmpCEC.tag=24,((_tmpCEC.f1=0,_tmpCEC)))),_tmpCEB)))));}
break;}case 262: _LL330: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2347 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCEF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEE;yyval=Cyc_YY47((void*)((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE[0]=((_tmpCEF.tag=24,((_tmpCEF.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCEF)))),_tmpCEE)))));}
break;}case 263: _LL331: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2348 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCF1;yyval=Cyc_YY47((void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=25,((_tmpCF2.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCF2)))),_tmpCF1)))));}
break;}case 264: _LL332: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2349 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF8;struct Cyc_List_List*_tmpCF7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF6;yyval=Cyc_YY47((void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF8.tag=24,((_tmpCF8.f1=((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF7->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCF7)))))),_tmpCF8)))),_tmpCF6)))));}
break;}case 265: _LL333: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCF9;yyval=Cyc_YY43(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF9->tl=0,_tmpCF9)))))));}
break;}case 266: _LL334: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2356 "parse.y"
{struct Cyc_List_List*_tmpCFA;yyval=Cyc_YY43(((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpCFA->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCFA)))))));}
break;}case 267: _LL335: {
# 2359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2361 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCFB;yyval=Cyc_YY33(((_tmpCFB.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCFB)));}
break;}case 268: _LL336: {
# 2364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2363 "parse.y"
yyval=yyyyvsp[0];
break;}case 269: _LL337: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2365 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCFC;yyval=Cyc_YY33(((_tmpCFC.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpCFC)));}
break;}case 270: _LL338: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2370 "parse.y"
yyval=yyyyvsp[1];
break;}case 271: _LL339: {
# 2373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2372 "parse.y"
{struct Cyc_List_List*_tmpD06;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD05;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD04;struct Cyc_Parse_Abstractdeclarator _tmpD03;yyval=Cyc_YY33(((_tmpD03.tms=((_tmpD06=_region_malloc(yyr,sizeof(*_tmpD06)),((_tmpD06->hd=(void*)((_tmpD04=_region_malloc(yyr,sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=0,((_tmpD05.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD05.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD05)))))),_tmpD04)))),((_tmpD06->tl=0,_tmpD06)))))),_tmpD03)));}
break;}case 272: _LL33A: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2374 "parse.y"
{struct Cyc_List_List*_tmpD10;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD0F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD0E;struct Cyc_Parse_Abstractdeclarator _tmpD0D;yyval=Cyc_YY33(((_tmpD0D.tms=((_tmpD10=_region_malloc(yyr,sizeof(*_tmpD10)),((_tmpD10->hd=(void*)((_tmpD0E=_region_malloc(yyr,sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=0,((_tmpD0F.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD0F.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD0F)))))),_tmpD0E)))),((_tmpD10->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD10)))))),_tmpD0D)));}
break;}case 273: _LL33B: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376 "parse.y"
{struct Cyc_List_List*_tmpD1A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD19;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD18;struct Cyc_Parse_Abstractdeclarator _tmpD17;yyval=Cyc_YY33(((_tmpD17.tms=((_tmpD1A=_region_malloc(yyr,sizeof(*_tmpD1A)),((_tmpD1A->hd=(void*)((_tmpD18=_region_malloc(yyr,sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=1,((_tmpD19.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD19.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD19.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD19)))))))),_tmpD18)))),((_tmpD1A->tl=0,_tmpD1A)))))),_tmpD17)));}
break;}case 274: _LL33C: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2378 "parse.y"
{struct Cyc_List_List*_tmpD24;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD23;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD22;struct Cyc_Parse_Abstractdeclarator _tmpD21;yyval=Cyc_YY33(((_tmpD21.tms=((_tmpD24=_region_malloc(yyr,sizeof(*_tmpD24)),((_tmpD24->hd=(void*)((_tmpD22=_region_malloc(yyr,sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD23.tag=1,((_tmpD23.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD23.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD23.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD23)))))))),_tmpD22)))),((_tmpD24->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD24)))))),_tmpD21)));}
# 2381
break;}case 275: _LL33D: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2382 "parse.y"
{struct Cyc_List_List*_tmpD39;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD38;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD37;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD36;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD35;struct Cyc_Parse_Abstractdeclarator _tmpD34;yyval=Cyc_YY33(((_tmpD34.tms=((_tmpD39=_region_malloc(yyr,sizeof(*_tmpD39)),((_tmpD39->hd=(void*)((_tmpD35=_region_malloc(yyr,sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD38.tag=3,((_tmpD38.f1=(void*)((_tmpD36=_region_malloc(yyr,sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=1,((_tmpD37.f1=0,((_tmpD37.f2=0,((_tmpD37.f3=0,((_tmpD37.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD37.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD37)))))))))))),_tmpD36)))),_tmpD38)))),_tmpD35)))),((_tmpD39->tl=0,_tmpD39)))))),_tmpD34)));}
# 2384
break;}case 276: _LL33E: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2385 "parse.y"
struct _tuple26*_stmttmp21=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp6F9;int _tmp6FA;struct Cyc_Absyn_VarargInfo*_tmp6FB;void*_tmp6FC;struct Cyc_List_List*_tmp6FD;struct _tuple26*_tmp6F8=_stmttmp21;_tmp6F9=_tmp6F8->f1;_tmp6FA=_tmp6F8->f2;_tmp6FB=_tmp6F8->f3;_tmp6FC=_tmp6F8->f4;_tmp6FD=_tmp6F8->f5;
{struct Cyc_List_List*_tmpD4E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD4D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD4C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD4B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4A;struct Cyc_Parse_Abstractdeclarator _tmpD49;yyval=Cyc_YY33(((_tmpD49.tms=((_tmpD4E=_region_malloc(yyr,sizeof(*_tmpD4E)),((_tmpD4E->hd=(void*)((_tmpD4A=_region_malloc(yyr,sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD4D.tag=3,((_tmpD4D.f1=(void*)((_tmpD4B=_region_malloc(yyr,sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4C.tag=1,((_tmpD4C.f1=_tmp6F9,((_tmpD4C.f2=_tmp6FA,((_tmpD4C.f3=_tmp6FB,((_tmpD4C.f4=_tmp6FC,((_tmpD4C.f5=_tmp6FD,_tmpD4C)))))))))))),_tmpD4B)))),_tmpD4D)))),_tmpD4A)))),((_tmpD4E->tl=0,_tmpD4E)))))),_tmpD49)));}
# 2388
break;}case 277: _LL33F: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2389 "parse.y"
{struct Cyc_List_List*_tmpD63;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD62;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD61;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD60;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5F;struct Cyc_Parse_Abstractdeclarator _tmpD5E;yyval=Cyc_YY33(((_tmpD5E.tms=((_tmpD63=_region_malloc(yyr,sizeof(*_tmpD63)),((_tmpD63->hd=(void*)((_tmpD5F=_region_malloc(yyr,sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD62.tag=3,((_tmpD62.f1=(void*)((_tmpD60=_region_malloc(yyr,sizeof(*_tmpD60)),((_tmpD60[0]=((_tmpD61.tag=1,((_tmpD61.f1=0,((_tmpD61.f2=0,((_tmpD61.f3=0,((_tmpD61.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD61.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD61)))))))))))),_tmpD60)))),_tmpD62)))),_tmpD5F)))),((_tmpD63->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD63)))))),_tmpD5E)));}
# 2392
break;}case 278: _LL340: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2393 "parse.y"
struct _tuple26*_stmttmp20=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp70B;int _tmp70C;struct Cyc_Absyn_VarargInfo*_tmp70D;void*_tmp70E;struct Cyc_List_List*_tmp70F;struct _tuple26*_tmp70A=_stmttmp20;_tmp70B=_tmp70A->f1;_tmp70C=_tmp70A->f2;_tmp70D=_tmp70A->f3;_tmp70E=_tmp70A->f4;_tmp70F=_tmp70A->f5;
{struct Cyc_List_List*_tmpD78;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD77;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD76;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD75;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD74;struct Cyc_Parse_Abstractdeclarator _tmpD73;yyval=Cyc_YY33(((_tmpD73.tms=((_tmpD78=_region_malloc(yyr,sizeof(*_tmpD78)),((_tmpD78->hd=(void*)((_tmpD74=_region_malloc(yyr,sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD77.tag=3,((_tmpD77.f1=(void*)((_tmpD75=_region_malloc(yyr,sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD76.tag=1,((_tmpD76.f1=_tmp70B,((_tmpD76.f2=_tmp70C,((_tmpD76.f3=_tmp70D,((_tmpD76.f4=_tmp70E,((_tmpD76.f5=_tmp70F,_tmpD76)))))))))))),_tmpD75)))),_tmpD77)))),_tmpD74)))),((_tmpD78->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD78)))))),_tmpD73)));}
# 2397
break;}case 279: _LL341: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2399 "parse.y"
struct Cyc_List_List*_tmp716=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD82;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD81;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD80;struct Cyc_Parse_Abstractdeclarator _tmpD7F;yyval=Cyc_YY33(((_tmpD7F.tms=((_tmpD82=_region_malloc(yyr,sizeof(*_tmpD82)),((_tmpD82->hd=(void*)((_tmpD80=_region_malloc(yyr,sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=4,((_tmpD81.f1=_tmp716,((_tmpD81.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD81.f3=0,_tmpD81)))))))),_tmpD80)))),((_tmpD82->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD82)))))),_tmpD7F)));}
# 2403
break;}case 280: _LL342: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2404 "parse.y"
{struct Cyc_List_List*_tmpD8C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD8B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8A;struct Cyc_Parse_Abstractdeclarator _tmpD89;yyval=Cyc_YY33(((_tmpD89.tms=((_tmpD8C=_region_malloc(yyr,sizeof(*_tmpD8C)),((_tmpD8C->hd=(void*)((_tmpD8A=_region_malloc(yyr,sizeof(*_tmpD8A)),((_tmpD8A[0]=((_tmpD8B.tag=5,((_tmpD8B.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD8B.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpD8B)))))),_tmpD8A)))),((_tmpD8C->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD8C)))))),_tmpD89)));}
# 2406
break;}case 281: _LL343: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2410 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL344: {
# 2413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2411 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL345: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2412 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL346: {
# 2415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL347: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2414 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL348: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2415 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL349: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2418 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD8F;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD8E;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=((_tmpD8F.tag=16,((_tmpD8F.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD8F)))),_tmpD8E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 288: _LL34A:
# 2421
 yyval=Cyc_YY5(0);
break;case 289: _LL34B: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2424 "parse.y"
{const char*_tmpD90;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD90="open",_tag_dyneither(_tmpD90,sizeof(char),5))))!= 0){
const char*_tmpD91;Cyc_Parse_err(((_tmpD91="expecting `open'",_tag_dyneither(_tmpD91,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2428
break;}case 290: _LL34C: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD97;struct _dyneither_ptr*_tmpD96;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD95;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95[0]=((_tmpD97.tag=13,((_tmpD97.f1=((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD96)))),((_tmpD97.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD97)))))),_tmpD95)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 291: _LL34D: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2437 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL34E: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2438 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34F: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2443 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL350: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2444 "parse.y"
yyval=yyyyvsp[1];
break;}case 295: _LL351: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2449 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 296: _LL352: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2450 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 297: _LL353: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2451 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD9D;struct _dyneither_ptr*_tmpD9C;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD9B;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9D.tag=13,((_tmpD9D.f1=((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD9C)))),((_tmpD9D.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpD9D)))))),_tmpD9B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2453
break;}case 298: _LL354: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2453 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDA3;struct _dyneither_ptr*_tmpDA2;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDA1;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA3.tag=13,((_tmpDA3.f1=((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDA2)))),((_tmpDA3.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDA3)))))),_tmpDA1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2455
break;}case 299: _LL355: {
# 2457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2455 "parse.y"
yyval=yyyyvsp[0];
break;}case 300: _LL356: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2456 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL357: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2457 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA6;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA5;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5[0]=((_tmpDA6.tag=1,((_tmpDA6.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA6)))),_tmpDA5)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 302: _LL358: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2460 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA9;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA8;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8[0]=((_tmpDA9.tag=1,((_tmpDA9.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA9)))),_tmpDA8)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 303: _LL359: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2465 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL35A: {
# 2468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2467 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL35B: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL35C: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2476 "parse.y"
struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp730,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL35D: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2480 "parse.y"
struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp731,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2483
break;}case 308: _LL35E: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2486 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL35F:
# 2489
 yyval=Cyc_YY11(0);
break;case 310: _LL360: {
# 2492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2502 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAC;struct Cyc_List_List*_tmpDAB;yyval=Cyc_YY11(((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB->hd=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDAC->pat_vars=0,((_tmpDAC->where_clause=0,((_tmpDAC->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDAC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAC)))))))))))),((_tmpDAB->tl=0,_tmpDAB)))))));}
# 2505
break;}case 311: _LL361: {
# 2507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2506 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAF;struct Cyc_List_List*_tmpDAE;yyval=Cyc_YY11(((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDAF->pat_vars=0,((_tmpDAF->where_clause=0,((_tmpDAF->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDAF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAF)))))))))))),((_tmpDAE->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDAE)))))));}
break;}case 312: _LL362: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2510 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB2;struct Cyc_List_List*_tmpDB1;yyval=Cyc_YY11(((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->hd=((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB2->pat_vars=0,((_tmpDB2->where_clause=0,((_tmpDB2->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDB2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB2)))))))))))),((_tmpDB1->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDB1)))))));}
break;}case 313: _LL363: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2512 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB5;struct Cyc_List_List*_tmpDB4;yyval=Cyc_YY11(((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->hd=((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB5->pat_vars=0,((_tmpDB5->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB5->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDB5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB5)))))))))))),((_tmpDB4->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDB4)))))));}
break;}case 314: _LL364: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2516 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB8;struct Cyc_List_List*_tmpDB7;yyval=Cyc_YY11(((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB8->pat_vars=0,((_tmpDB8->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB8->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDB8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB8)))))))))))),((_tmpDB7->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDB7)))))));}
break;}case 315: _LL365: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2523 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL366: {
# 2526
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2527 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL367: {
# 2530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2531 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL368: {
# 2535
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2537
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2534 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL369: {
# 2538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2537 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL36A: {
# 2541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2540 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL36B: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2543 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL36C: {
# 2547
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2549
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2546 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL36D: {
# 2550
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2549 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL36E: {
# 2553
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2555
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2552 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL36F: {
# 2556
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2558
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2555 "parse.y"
struct Cyc_List_List*_tmp73C=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73C,_tmp73D));
# 2560
break;}case 326: _LL370: {
# 2562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2561 "parse.y"
struct Cyc_List_List*_tmp73E=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73E,_tmp73F));
# 2566
break;}case 327: _LL371: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2567 "parse.y"
struct Cyc_List_List*_tmp740=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp741=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp740,_tmp741));
# 2572
break;}case 328: _LL372: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2573 "parse.y"
struct Cyc_List_List*_tmp742=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp743=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp742,_tmp743));
# 2578
break;}case 329: _LL373: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDB9;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDB9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 330: _LL374: {
# 2586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2584 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL375: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2585 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL376: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2586 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL377: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2587 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL378: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2589 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL379: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2590 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL37A: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2592 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL37B: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2601 "parse.y"
yyval=yyyyvsp[0];
break;}case 338: _LL37C: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL37D: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2606 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL37E: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL37F: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL380: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL381: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL382: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2619 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL383: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2621 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL384: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2624 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL385: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2626 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL386: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2629 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL387: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2631 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL388: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2634 "parse.y"
yyval=yyyyvsp[0];
break;}case 351: _LL389: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2636 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL38A: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2638 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL38B: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2641 "parse.y"
yyval=yyyyvsp[0];
break;}case 354: _LL38C: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2643 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL38D: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2645 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL38E: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2647 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38F: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2649 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL390: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2652 "parse.y"
yyval=yyyyvsp[0];
break;}case 359: _LL391: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2654 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL392: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2656 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL393: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2659 "parse.y"
yyval=yyyyvsp[0];
break;}case 362: _LL394: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2661 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL395: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2663 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL396: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2666 "parse.y"
yyval=yyyyvsp[0];
break;}case 365: _LL397: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2668 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL398: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2670 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL399: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2672 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL39A: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2675 "parse.y"
yyval=yyyyvsp[0];
break;}case 369: _LL39B: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2677 "parse.y"
void*_tmp745=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp745,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2680
break;}case 370: _LL39C: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2683 "parse.y"
yyval=yyyyvsp[0];
break;}case 371: _LL39D: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2686 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 372: _LL39E: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2688 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 373: _LL39F: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2690 "parse.y"
void*_tmp746=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp746,(unsigned int)(yyyylsp[0]).first_line)));
# 2693
break;}case 374: _LL3A0: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2694 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL3A1: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2696 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL3A2: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2701 "parse.y"
yyval=yyyyvsp[0];
break;}case 377: _LL3A3: {
# 2704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2710 "parse.y"
yyval=yyyyvsp[0];
break;}case 378: _LL3A4: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2715 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 379: _LL3A5: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2717 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 380: _LL3A6: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2719 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_stmttmp1F=e->r;void*_tmp747=_stmttmp1F;enum Cyc_Absyn_Sign _tmp749;char _tmp74A;enum Cyc_Absyn_Sign _tmp74C;short _tmp74D;enum Cyc_Absyn_Sign _tmp74F;int _tmp750;struct _dyneither_ptr _tmp752;int _tmp753;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp748=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp748->tag != 0)goto _LL3AB;else{if(((_tmp748->f1).Char_c).tag != 2)goto _LL3AB;_tmp749=((struct _tuple3)((_tmp748->f1).Char_c).val).f1;_tmp74A=((struct _tuple3)((_tmp748->f1).Char_c).val).f2;}}_LL3AA:
# 2723
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDBC;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDBB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBB=_cycalloc_atomic(sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBC.tag=10,((_tmpDBC.f1=_tmp74A,_tmpDBC)))),_tmpDBB)))),e->loc));}goto _LL3A8;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp74B->tag != 0)goto _LL3AD;else{if(((_tmp74B->f1).Short_c).tag != 4)goto _LL3AD;_tmp74C=((struct _tuple4)((_tmp74B->f1).Short_c).val).f1;_tmp74D=((struct _tuple4)((_tmp74B->f1).Short_c).val).f2;}}_LL3AC:
# 2725
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDBF;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDBE;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBE=_cycalloc_atomic(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDBF.tag=9,((_tmpDBF.f1=_tmp74C,((_tmpDBF.f2=(int)_tmp74D,_tmpDBF)))))),_tmpDBE)))),e->loc));}goto _LL3A8;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp74E->tag != 0)goto _LL3AF;else{if(((_tmp74E->f1).Int_c).tag != 5)goto _LL3AF;_tmp74F=((struct _tuple5)((_tmp74E->f1).Int_c).val).f1;_tmp750=((struct _tuple5)((_tmp74E->f1).Int_c).val).f2;}}_LL3AE:
# 2727
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDC2;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDC1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC1=_cycalloc_atomic(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=9,((_tmpDC2.f1=_tmp74F,((_tmpDC2.f2=_tmp750,_tmpDC2)))))),_tmpDC1)))),e->loc));}goto _LL3A8;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp751=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp751->tag != 0)goto _LL3B1;else{if(((_tmp751->f1).Float_c).tag != 7)goto _LL3B1;_tmp752=((struct _tuple7)((_tmp751->f1).Float_c).val).f1;_tmp753=((struct _tuple7)((_tmp751->f1).Float_c).val).f2;}}_LL3B0:
# 2729
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDC5;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDC4;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=11,((_tmpDC5.f1=_tmp752,((_tmpDC5.f2=_tmp753,_tmpDC5)))))),_tmpDC4)))),e->loc));}goto _LL3A8;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp754=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp754->tag != 0)goto _LL3B3;else{if(((_tmp754->f1).Null_c).tag != 1)goto _LL3B3;}}_LL3B2:
# 2731
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A8;_LL3B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp755=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp755->tag != 0)goto _LL3B5;else{if(((_tmp755->f1).String_c).tag != 8)goto _LL3B5;}}_LL3B4:
# 2733
{const char*_tmpDC6;Cyc_Parse_err(((_tmpDC6="strings cannot occur within patterns",_tag_dyneither(_tmpDC6,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp756=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp756->tag != 0)goto _LL3B7;else{if(((_tmp756->f1).Wstring_c).tag != 9)goto _LL3B7;}}_LL3B6:
# 2735
{const char*_tmpDC7;Cyc_Parse_err(((_tmpDC7="strings cannot occur within patterns",_tag_dyneither(_tmpDC7,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp757=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp747;if(_tmp757->tag != 0)goto _LL3B9;else{if(((_tmp757->f1).LongLong_c).tag != 6)goto _LL3B9;}}_LL3B8:
# 2737
{const char*_tmpDC8;Cyc_Parse_unimp(((_tmpDC8="long long's in patterns",_tag_dyneither(_tmpDC8,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B9:;_LL3BA: {
# 2739
const char*_tmpDC9;Cyc_Parse_err(((_tmpDC9="bad constant in case",_tag_dyneither(_tmpDC9,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A8:;}
# 2742
break;}case 381: _LL3A7: {
# 2744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2743 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDCC;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDCB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=14,((_tmpDCC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDCC)))),_tmpDCB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 382: _LL3BB: {
# 2746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2745 "parse.y"
{const char*_tmpDCD;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDCD="as",_tag_dyneither(_tmpDCD,sizeof(char),3))))!= 0){
const char*_tmpDCE;Cyc_Parse_err(((_tmpDCE="expecting `as'",_tag_dyneither(_tmpDCE,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDD8;struct _dyneither_ptr*_tmpDD7;struct _tuple0*_tmpDD6;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDD5;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD8.tag=1,((_tmpDD8.f1=Cyc_Absyn_new_vardecl(((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->f1=Cyc_Absyn_Loc_n,((_tmpDD6->f2=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD7)))),_tmpDD6)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD8.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDD8)))))),_tmpDD5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2750
break;}case 383: _LL3BC: {
# 2752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751 "parse.y"
struct _tuple22 _stmttmp1E=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp76D;int _tmp76E;struct _tuple22 _tmp76C=_stmttmp1E;_tmp76D=_tmp76C.f1;_tmp76E=_tmp76C.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDDB;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDDA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDB.tag=4,((_tmpDDB.f1=_tmp76D,((_tmpDDB.f2=_tmp76E,_tmpDDB)))))),_tmpDDA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2754
break;}case 384: _LL3BD: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2755 "parse.y"
struct _tuple22 _stmttmp1D=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp772;int _tmp773;struct _tuple22 _tmp771=_stmttmp1D;_tmp772=_tmp771.f1;_tmp773=_tmp771.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDDE;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDDD;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDE.tag=15,((_tmpDDE.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDDE.f2=_tmp772,((_tmpDDE.f3=_tmp773,_tmpDDE)))))))),_tmpDDD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2758
break;}case 385: _LL3BE: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2759 "parse.y"
struct _tuple22 _stmttmp1C=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp777;int _tmp778;struct _tuple22 _tmp776=_stmttmp1C;_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;{
struct Cyc_List_List*_tmp779=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE4;struct Cyc_Absyn_AggrInfo*_tmpDE3;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE4.tag=6,((_tmpDE4.f1=((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDE3->targs=0,_tmpDE3)))))),((_tmpDE4.f2=_tmp779,((_tmpDE4.f3=_tmp777,((_tmpDE4.f4=_tmp778,_tmpDE4)))))))))),_tmpDE2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2764
break;};}case 386: _LL3BF: {
# 2766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2765 "parse.y"
struct _tuple22 _stmttmp1B=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp77E;int _tmp77F;struct _tuple22 _tmp77D=_stmttmp1B;_tmp77E=_tmp77D.f1;_tmp77F=_tmp77D.f2;{
struct Cyc_List_List*_tmp780=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE7;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE6;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE7.tag=6,((_tmpDE7.f1=0,((_tmpDE7.f2=_tmp780,((_tmpDE7.f3=_tmp77E,((_tmpDE7.f4=_tmp77F,_tmpDE7)))))))))),_tmpDE6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2769
break;};}case 387: _LL3C0: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2770 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDEA;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDE9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEA.tag=5,((_tmpDEA.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDEA)))),_tmpDE9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3C1: {
# 2773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2772 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF4;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF3;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDF2;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDF1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF4.tag=5,((_tmpDF4.f1=Cyc_Absyn_new_pat((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF3.tag=5,((_tmpDF3.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDF3)))),_tmpDF2)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDF4)))),_tmpDF1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3C2: {
# 2775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2774 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDFE;struct _dyneither_ptr*_tmpDFD;struct _tuple0*_tmpDFC;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDFB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFE.tag=2,((_tmpDFE.f1=Cyc_Absyn_new_vardecl(((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->f1=Cyc_Absyn_Loc_n,((_tmpDFC->f2=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDFD)))),_tmpDFC)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDFE.f2=
# 2776
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDFE)))))),_tmpDFB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2778
break;}case 390: _LL3C3: {
# 2780
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2779 "parse.y"
{const char*_tmpDFF;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDFF="as",_tag_dyneither(_tmpDFF,sizeof(char),3))))!= 0){
const char*_tmpE00;Cyc_Parse_err(((_tmpE00="expecting `as'",_tag_dyneither(_tmpE00,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE0A;struct _dyneither_ptr*_tmpE09;struct _tuple0*_tmpE08;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE07;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE0A.tag=2,((_tmpE0A.f1=Cyc_Absyn_new_vardecl(((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->f1=Cyc_Absyn_Loc_n,((_tmpE08->f2=((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE09)))),_tmpE08)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE0A.f2=
# 2783
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE0A)))))),_tmpE07)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2785
break;}case 391: _LL3C4: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2786 "parse.y"
void*_tmp793=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE1B;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE1A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE19;struct _dyneither_ptr*_tmpE18;struct _tuple0*_tmpE17;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE16;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=((_tmpE1B.tag=3,((_tmpE1B.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp793),((_tmpE1B.f2=
Cyc_Absyn_new_vardecl(((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->f1=Cyc_Absyn_Loc_n,((_tmpE17->f2=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE18)))),_tmpE17)))))),(void*)(
(_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1A.tag=19,((_tmpE1A.f1=_tmp793,_tmpE1A)))),_tmpE19)))),0),_tmpE1B)))))),_tmpE16)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2791
break;}case 392: _LL3C5: {
# 2793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2792 "parse.y"
struct Cyc_Absyn_Tvar*_tmp79A=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE37;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE36;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE35;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE34;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE33;struct _dyneither_ptr*_tmpE32;struct _tuple0*_tmpE31;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE30;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE37.tag=3,((_tmpE37.f1=_tmp79A,((_tmpE37.f2=
Cyc_Absyn_new_vardecl(((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->f1=Cyc_Absyn_Loc_n,((_tmpE31->f2=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE32)))),_tmpE31)))))),(void*)(
(_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE36.tag=19,((_tmpE36.f1=(void*)((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE35.tag=2,((_tmpE35.f1=_tmp79A,_tmpE35)))),_tmpE34)))),_tmpE36)))),_tmpE33)))),0),_tmpE37)))))),_tmpE30)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2797
break;}case 393: _LL3C6: {
# 2799
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2800 "parse.y"
{struct _tuple22*_tmpE38;yyval=Cyc_YY13(((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE38->f2=0,_tmpE38)))))));}
break;}case 394: _LL3C7: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2801 "parse.y"
{struct _tuple22*_tmpE39;yyval=Cyc_YY13(((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE39->f2=1,_tmpE39)))))));}
break;}case 395: _LL3C8: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2802 "parse.y"
{struct _tuple22*_tmpE3A;yyval=Cyc_YY13(((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->f1=0,((_tmpE3A->f2=1,_tmpE3A)))))));}
break;}case 396: _LL3C9: {
# 2805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2807 "parse.y"
{struct Cyc_List_List*_tmpE3B;yyval=Cyc_YY14(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE3B->tl=0,_tmpE3B)))))));}
break;}case 397: _LL3CA: {
# 2810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2809 "parse.y"
{struct Cyc_List_List*_tmpE3C;yyval=Cyc_YY14(((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE3C->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE3C)))))));}
break;}case 398: _LL3CB: {
# 2812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2814 "parse.y"
{struct _tuple23*_tmpE3D;yyval=Cyc_YY15(((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->f1=0,((_tmpE3D->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE3D)))))));}
break;}case 399: _LL3CC: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2816 "parse.y"
{struct _tuple23*_tmpE3E;yyval=Cyc_YY15(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE3E->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE3E)))))));}
break;}case 400: _LL3CD: {
# 2819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2819 "parse.y"
{struct _tuple22*_tmpE3F;yyval=Cyc_YY17(((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE3F->f2=0,_tmpE3F)))))));}
break;}case 401: _LL3CE: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2820 "parse.y"
{struct _tuple22*_tmpE40;yyval=Cyc_YY17(((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE40->f2=1,_tmpE40)))))));}
break;}case 402: _LL3CF: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2821 "parse.y"
{struct _tuple22*_tmpE41;yyval=Cyc_YY17(((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->f1=0,((_tmpE41->f2=1,_tmpE41)))))));}
break;}case 403: _LL3D0: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2826 "parse.y"
{struct Cyc_List_List*_tmpE42;yyval=Cyc_YY16(((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE42->tl=0,_tmpE42)))))));}
break;}case 404: _LL3D1: {
# 2829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2828 "parse.y"
{struct Cyc_List_List*_tmpE43;yyval=Cyc_YY16(((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE43->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE43)))))));}
break;}case 405: _LL3D2: {
# 2831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 406: _LL3D3: {
# 2837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2836 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 407: _LL3D4: {
# 2839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2841 "parse.y"
yyval=yyyyvsp[0];
break;}case 408: _LL3D5: {
# 2844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2843 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3D6: {
# 2846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2845 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3D7: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849 "parse.y"
yyval=Cyc_YY9(0);
break;}case 411: _LL3D8: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850 "parse.y"
{struct Cyc_Core_Opt*_tmpE44;yyval=Cyc_YY9(((_tmpE44=_cycalloc_atomic(sizeof(*_tmpE44)),((_tmpE44->v=(void*)Cyc_Absyn_Times,_tmpE44)))));}
break;}case 412: _LL3D9: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851 "parse.y"
{struct Cyc_Core_Opt*_tmpE45;yyval=Cyc_YY9(((_tmpE45=_cycalloc_atomic(sizeof(*_tmpE45)),((_tmpE45->v=(void*)Cyc_Absyn_Div,_tmpE45)))));}
break;}case 413: _LL3DA: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
{struct Cyc_Core_Opt*_tmpE46;yyval=Cyc_YY9(((_tmpE46=_cycalloc_atomic(sizeof(*_tmpE46)),((_tmpE46->v=(void*)Cyc_Absyn_Mod,_tmpE46)))));}
break;}case 414: _LL3DB: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853 "parse.y"
{struct Cyc_Core_Opt*_tmpE47;yyval=Cyc_YY9(((_tmpE47=_cycalloc_atomic(sizeof(*_tmpE47)),((_tmpE47->v=(void*)Cyc_Absyn_Plus,_tmpE47)))));}
break;}case 415: _LL3DC: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_Core_Opt*_tmpE48;yyval=Cyc_YY9(((_tmpE48=_cycalloc_atomic(sizeof(*_tmpE48)),((_tmpE48->v=(void*)Cyc_Absyn_Minus,_tmpE48)))));}
break;}case 416: _LL3DD: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855 "parse.y"
{struct Cyc_Core_Opt*_tmpE49;yyval=Cyc_YY9(((_tmpE49=_cycalloc_atomic(sizeof(*_tmpE49)),((_tmpE49->v=(void*)Cyc_Absyn_Bitlshift,_tmpE49)))));}
break;}case 417: _LL3DE: {
# 2858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856 "parse.y"
{struct Cyc_Core_Opt*_tmpE4A;yyval=Cyc_YY9(((_tmpE4A=_cycalloc_atomic(sizeof(*_tmpE4A)),((_tmpE4A->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE4A)))));}
break;}case 418: _LL3DF: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857 "parse.y"
{struct Cyc_Core_Opt*_tmpE4B;yyval=Cyc_YY9(((_tmpE4B=_cycalloc_atomic(sizeof(*_tmpE4B)),((_tmpE4B->v=(void*)Cyc_Absyn_Bitand,_tmpE4B)))));}
break;}case 419: _LL3E0: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858 "parse.y"
{struct Cyc_Core_Opt*_tmpE4C;yyval=Cyc_YY9(((_tmpE4C=_cycalloc_atomic(sizeof(*_tmpE4C)),((_tmpE4C->v=(void*)Cyc_Absyn_Bitxor,_tmpE4C)))));}
break;}case 420: _LL3E1: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2859 "parse.y"
{struct Cyc_Core_Opt*_tmpE4D;yyval=Cyc_YY9(((_tmpE4D=_cycalloc_atomic(sizeof(*_tmpE4D)),((_tmpE4D->v=(void*)Cyc_Absyn_Bitor,_tmpE4D)))));}
break;}case 421: _LL3E2: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2864 "parse.y"
yyval=yyyyvsp[0];
break;}case 422: _LL3E3: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2866 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3E4: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2869 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E5: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2872 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E6: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2874 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E7: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2876 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E8: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2878 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E9: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2882 "parse.y"
yyval=yyyyvsp[0];
break;}case 429: _LL3EA: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2886 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3EB: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2888 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3EC: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2892 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3ED: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2894 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3EE: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2898 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3EF: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2900 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3F0: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2904 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3F1: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3F2: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3F3: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2912 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F4: {
# 2915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2916 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL3F5: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2918 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F6: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2920 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F7: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL3F8: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F9: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2928 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3FA: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2930 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3FB: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2932 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3FC: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2936 "parse.y"
yyval=yyyyvsp[0];
break;}case 448: _LL3FD: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2938 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3FE: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2940 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FF: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2944 "parse.y"
yyval=yyyyvsp[0];
break;}case 451: _LL400: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2946 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL401: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2948 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL402: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2952 "parse.y"
yyval=yyyyvsp[0];
break;}case 454: _LL403: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2954 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL404: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2956 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL405: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2958 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL406: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2962 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL407: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2964 "parse.y"
void*_tmp7B9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7B9,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2967
break;}case 459: _LL408: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2970 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL409: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL40A: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2972 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL40B: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2973 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL40C: {
# 2976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2974 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL40D: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2975 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL40E: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2976 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40F: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2978 "parse.y"
void*_tmp7BA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7BA,(unsigned int)(yyyylsp[0]).first_line));
# 2981
break;}case 467: _LL410: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2981 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL411: {
# 2984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2983 "parse.y"
void*_tmp7BB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7BB,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2986
break;}case 469: _LL412: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2988 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE53;struct Cyc_Absyn_MallocInfo _tmpE52;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE51;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE53.tag=33,((_tmpE53.f1=((_tmpE52.is_calloc=0,((_tmpE52.rgn=0,((_tmpE52.elt_type=0,((_tmpE52.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE52.fat_result=0,_tmpE52)))))))))),_tmpE53)))),_tmpE51)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2990
break;}case 470: _LL413: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2991 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE59;struct Cyc_Absyn_MallocInfo _tmpE58;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE57;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE59.tag=33,((_tmpE59.f1=((_tmpE58.is_calloc=0,((_tmpE58.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE58.elt_type=0,((_tmpE58.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE58.fat_result=0,_tmpE58)))))))))),_tmpE59)))),_tmpE57)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2993
break;}case 471: _LL414: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2994 "parse.y"
void*_tmp7C2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE63;void**_tmpE62;struct Cyc_Absyn_MallocInfo _tmpE61;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE60;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE63.tag=33,((_tmpE63.f1=((_tmpE61.is_calloc=1,((_tmpE61.rgn=0,((_tmpE61.elt_type=((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=_tmp7C2,_tmpE62)))),((_tmpE61.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE61.fat_result=0,_tmpE61)))))))))),_tmpE63)))),_tmpE60)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2997
break;}case 472: _LL415: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2999 "parse.y"
void*_tmp7C7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE6D;void**_tmpE6C;struct Cyc_Absyn_MallocInfo _tmpE6B;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE6A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6D.tag=33,((_tmpE6D.f1=((_tmpE6B.is_calloc=1,((_tmpE6B.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE6B.elt_type=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=_tmp7C7,_tmpE6C)))),((_tmpE6B.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE6B.fat_result=0,_tmpE6B)))))))))),_tmpE6D)))),_tmpE6A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3002
break;}case 473: _LL416: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3003 "parse.y"
{struct Cyc_Absyn_Exp*_tmpE6E[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE6E[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE6E,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL417: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3005 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE74;struct _dyneither_ptr*_tmpE73;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE72;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE74.tag=37,((_tmpE74.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE74.f2=((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE73)))),_tmpE74)))))),_tmpE72)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL418: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3007 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE7A;struct _dyneither_ptr*_tmpE79;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE78;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE7A.tag=37,((_tmpE7A.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE7A.f2=((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE79)))),_tmpE7A)))))),_tmpE78)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL419: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3009 "parse.y"
void*_tmp7D3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7D3,(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL41A: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3012 "parse.y"
struct _tuple20 _stmttmp1A=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp7D5;struct _dyneither_ptr _tmp7D6;struct _tuple20 _tmp7D4=_stmttmp1A;_tmp7D5=_tmp7D4.f1;_tmp7D6=_tmp7D4.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7D5,_tmp7D6,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL41B: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3017 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 479: _LL41C: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 480: _LL41D: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3019 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 481: _LL41E: {
# 3022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3024 "parse.y"
yyval=yyyyvsp[0];
break;}case 482: _LL41F: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3026 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL420: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL421: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3030 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL422: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3032 "parse.y"
{struct _dyneither_ptr*_tmpE7B;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE7B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL423: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3034 "parse.y"
{struct _dyneither_ptr*_tmpE7C;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE7C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL424: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3036 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL425: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3038 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL426: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3040 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE7F;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE7E;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E[0]=((_tmpE7F.tag=24,((_tmpE7F.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE7F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE7F)))))),_tmpE7E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL427: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3042 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE82;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE81;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81[0]=((_tmpE82.tag=24,((_tmpE82.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE82.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE82)))))),_tmpE81)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL428: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3047 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE8C;struct _dyneither_ptr*_tmpE8B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE8A;struct Cyc_List_List*_tmpE89;yyval=Cyc_YY3(((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89->hd=(void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8A.tag=0,((_tmpE8A.f1=((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE8B)))),_tmpE8A)))),_tmpE8C)))),((_tmpE89->tl=0,_tmpE89)))))));}
break;}case 492: _LL429: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3050 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE92;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE91;struct Cyc_List_List*_tmpE90;yyval=Cyc_YY3(((_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90->hd=(void*)((_tmpE92=_cycalloc_atomic(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE91.tag=1,((_tmpE91.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE91)))),_tmpE92)))),((_tmpE90->tl=0,_tmpE90)))))));}
break;}case 493: _LL42A: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3052 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE9C;struct _dyneither_ptr*_tmpE9B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE9A;struct Cyc_List_List*_tmpE99;yyval=Cyc_YY3(((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->hd=(void*)((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=((_tmpE9A.tag=0,((_tmpE9A.f1=((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE9B)))),_tmpE9A)))),_tmpE9C)))),((_tmpE99->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE99)))))));}
break;}case 494: _LL42B: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3055 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEA2;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEA1;struct Cyc_List_List*_tmpEA0;yyval=Cyc_YY3(((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->hd=(void*)((_tmpEA2=_cycalloc_atomic(sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA1.tag=1,((_tmpEA1.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpEA1)))),_tmpEA2)))),((_tmpEA0->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpEA0)))))));}
break;}case 495: _LL42C: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL42D: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3063 "parse.y"
yyval=yyyyvsp[0];
break;}case 497: _LL42E: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3065 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42F: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3067 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL430: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3069 "parse.y"
yyval=yyyyvsp[1];
break;}case 500: _LL431: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3074 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL432: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3076 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL433: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3079 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL434: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3082 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEA5;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEA4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA5.tag=28,((_tmpEA5.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEA5.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpEA5.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpEA5.f4=0,_tmpEA5)))))))))),_tmpEA4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 504: _LL435: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3085 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL436: {
# 3088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3089 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 506: _LL437: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpEA6;yyval=Cyc_YY6(((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEA6->tl=0,_tmpEA6)))))));}
break;}case 507: _LL438: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3097 "parse.y"
{struct Cyc_List_List*_tmpEA7;yyval=Cyc_YY6(((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA7->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEA7)))))));}
break;}case 508: _LL439: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp7EF=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7EF.f1,_tmp7EF.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL43A: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3104 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL43B: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3105 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL43C: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3107 "parse.y"
struct _dyneither_ptr _tmp7F0=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7F0);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7F0,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3115
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7F0,i,(unsigned int)(yyyylsp[0]).first_line));
# 3117
break;}case 512: _LL43D: {
# 3119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3118 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL43E: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3122 "parse.y"
{struct _dyneither_ptr*_tmpEAA;struct _tuple0*_tmpEA9;yyval=Cyc_QualId_tok(((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9->f1=Cyc_Absyn_Rel_n(0),((_tmpEA9->f2=((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEAA)))),_tmpEA9)))))));}
break;}case 514: _LL43F: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3123 "parse.y"
yyval=yyyyvsp[0];
break;}case 515: _LL440: {
# 3126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
{struct _dyneither_ptr*_tmpEAD;struct _tuple0*_tmpEAC;yyval=Cyc_QualId_tok(((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->f1=Cyc_Absyn_Rel_n(0),((_tmpEAC->f2=((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEAD)))),_tmpEAC)))))));}
break;}case 516: _LL441: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3127 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL442: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3132 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL443: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3133 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL444: {
# 3136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL445: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3137 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL446: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3142 "parse.y"
break;}case 522: _LL447: {
# 3144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3142 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL448:
# 3147
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(523,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(145,yyn - 147)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6875) && Cyc_yycheck[_check_known_subscript_notnull(6876,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6876,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(145,yyn - 147)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1041,yystate)];
# 431
if(yyn > - 32768  && yyn < 6875){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 292 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6876,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(292,x)])+ 15,count ++);}
{unsigned int _tmpEB2;unsigned int _tmpEB1;struct _dyneither_ptr _tmpEB0;char*_tmpEAF;unsigned int _tmpEAE;msg=((_tmpEAE=(unsigned int)(sze + 15),((_tmpEAF=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEAE + 1)),((_tmpEB0=_tag_dyneither(_tmpEAF,sizeof(char),_tmpEAE + 1),((((_tmpEB1=_tmpEAE,((_tmpEB3(& _tmpEB2,& _tmpEB1,& _tmpEAF),_tmpEAF[_tmpEB1]=(char)0)))),_tmpEB0))))))));}
{const char*_tmpEB4;Cyc_strcpy(msg,((_tmpEB4="parse error",_tag_dyneither(_tmpEB4,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 292 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6876,x + yyn)]== x){
# 453
{const char*_tmpEB6;const char*_tmpEB5;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEB6=", expecting `",_tag_dyneither(_tmpEB6,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEB5=" or `",_tag_dyneither(_tmpEB5,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(292,x)]);
{const char*_tmpEB7;Cyc_strcat(msg,((_tmpEB7="'",_tag_dyneither(_tmpEB7,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEB8;Cyc_yyerror(((_tmpEB8="parse error",_tag_dyneither(_tmpEB8,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7FE=1;_npop_handler(0);return _tmp7FE;}
# 485
yychar=-2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp7FF=1;_npop_handler(0);return _tmp7FF;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1041,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6875) || Cyc_yycheck[_check_known_subscript_notnull(6876,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6876,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1040){
int _tmp800=0;_npop_handler(0);return _tmp800;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3145 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp80A=v;int _tmp80B;char _tmp80C;struct _dyneither_ptr _tmp80D;union Cyc_Absyn_Nmspace _tmp80E;struct _dyneither_ptr*_tmp80F;_LL44B: if((_tmp80A.Int_tok).tag != 1)goto _LL44D;_tmp80B=((struct _tuple5)(_tmp80A.Int_tok).val).f2;_LL44C:
{const char*_tmpEBC;void*_tmpEBB[1];struct Cyc_Int_pa_PrintArg_struct _tmpEBA;(_tmpEBA.tag=1,((_tmpEBA.f1=(unsigned long)_tmp80B,((_tmpEBB[0]=& _tmpEBA,Cyc_fprintf(Cyc_stderr,((_tmpEBC="%d",_tag_dyneither(_tmpEBC,sizeof(char),3))),_tag_dyneither(_tmpEBB,sizeof(void*),1)))))));}goto _LL44A;_LL44D: if((_tmp80A.Char_tok).tag != 2)goto _LL44F;_tmp80C=(char)(_tmp80A.Char_tok).val;_LL44E:
{const char*_tmpEC0;void*_tmpEBF[1];struct Cyc_Int_pa_PrintArg_struct _tmpEBE;(_tmpEBE.tag=1,((_tmpEBE.f1=(unsigned long)((int)_tmp80C),((_tmpEBF[0]=& _tmpEBE,Cyc_fprintf(Cyc_stderr,((_tmpEC0="%c",_tag_dyneither(_tmpEC0,sizeof(char),3))),_tag_dyneither(_tmpEBF,sizeof(void*),1)))))));}goto _LL44A;_LL44F: if((_tmp80A.String_tok).tag != 3)goto _LL451;_tmp80D=(struct _dyneither_ptr)(_tmp80A.String_tok).val;_LL450:
{const char*_tmpEC4;void*_tmpEC3[1];struct Cyc_String_pa_PrintArg_struct _tmpEC2;(_tmpEC2.tag=0,((_tmpEC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp80D),((_tmpEC3[0]=& _tmpEC2,Cyc_fprintf(Cyc_stderr,((_tmpEC4="\"%s\"",_tag_dyneither(_tmpEC4,sizeof(char),5))),_tag_dyneither(_tmpEC3,sizeof(void*),1)))))));}goto _LL44A;_LL451: if((_tmp80A.QualId_tok).tag != 5)goto _LL453;_tmp80E=((struct _tuple0*)(_tmp80A.QualId_tok).val)->f1;_tmp80F=((struct _tuple0*)(_tmp80A.QualId_tok).val)->f2;_LL452: {
# 3151
struct Cyc_List_List*_tmp819=0;
{union Cyc_Absyn_Nmspace _tmp81A=_tmp80E;struct Cyc_List_List*_tmp81B;struct Cyc_List_List*_tmp81C;struct Cyc_List_List*_tmp81D;_LL456: if((_tmp81A.Rel_n).tag != 1)goto _LL458;_tmp81B=(struct Cyc_List_List*)(_tmp81A.Rel_n).val;_LL457:
 _tmp819=_tmp81B;goto _LL455;_LL458: if((_tmp81A.Abs_n).tag != 2)goto _LL45A;_tmp81C=(struct Cyc_List_List*)(_tmp81A.Abs_n).val;_LL459:
 _tmp819=_tmp81C;goto _LL455;_LL45A: if((_tmp81A.C_n).tag != 3)goto _LL45C;_tmp81D=(struct Cyc_List_List*)(_tmp81A.C_n).val;_LL45B:
 _tmp819=_tmp81D;goto _LL455;_LL45C: if((_tmp81A.Loc_n).tag != 4)goto _LL455;_LL45D:
 goto _LL455;_LL455:;}
# 3158
for(0;_tmp819 != 0;_tmp819=_tmp819->tl){
const char*_tmpEC8;void*_tmpEC7[1];struct Cyc_String_pa_PrintArg_struct _tmpEC6;(_tmpEC6.tag=0,((_tmpEC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp819->hd)),((_tmpEC7[0]=& _tmpEC6,Cyc_fprintf(Cyc_stderr,((_tmpEC8="%s::",_tag_dyneither(_tmpEC8,sizeof(char),5))),_tag_dyneither(_tmpEC7,sizeof(void*),1)))))));}
{const char*_tmpECC;void*_tmpECB[1];struct Cyc_String_pa_PrintArg_struct _tmpECA;(_tmpECA.tag=0,((_tmpECA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp80F),((_tmpECB[0]=& _tmpECA,Cyc_fprintf(Cyc_stderr,((_tmpECC="%s::",_tag_dyneither(_tmpECC,sizeof(char),5))),_tag_dyneither(_tmpECB,sizeof(void*),1)))))));}
goto _LL44A;}_LL453:;_LL454:
{const char*_tmpECF;void*_tmpECE;(_tmpECE=0,Cyc_fprintf(Cyc_stderr,((_tmpECF="?",_tag_dyneither(_tmpECF,sizeof(char),2))),_tag_dyneither(_tmpECE,sizeof(void*),0)));}goto _LL44A;_LL44A:;}
# 3167
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp826=_new_region("yyr");struct _RegionHandle*yyr=& _tmp826;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp827=Cyc_Parse_parse_result;_npop_handler(0);return _tmp827;};
# 3170
;_pop_region(yyr);};}
